/* $Id$ */
/*****************************************************************************
 *
 * Copyright 2006, 2007
 * Nanotron Technologies
 *
 * Author: S. Radtke
 *
 * Description :
 *    This file contains the source code for the implementation of the
 *    tx/rx part of the driver
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include    "ntrxranging.h"
#include "config.h"
#include "ntrxtypes.h"
#include    "ntrxdil.h"
#include    "hwclock.h"
#include    "ntrxutil.h"
#include    "nnspi.h"

#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#include "usart.h"

#define DIMADDRESS  6
MyBoolT TxARQorgVal;
/*
 * static buffer for receiving and processing only one message
 * at a time. If you need more buffers in receive direction
 * you have to modify the access to the receive buffer by
 * a different method (like a buffer pool and queues).
 */
MyByte8T buff_rx[128];


/* timeout for ranging in [ms] */
#define RANGING_TIMEOUT 10000
/* ready means : nothing in process */
#define RANGING_READY   0
/* start means : try to send first rangingpacket */
#define RANGING_START   1
/* answer1 means : waiting for first ranging answer */
#define RANGING_ANSWER1 2
/* answer2 means : waiting for second ranging answer */
#define RANGING_ANSWER2 3
/* successfull means : all data is collected, ready to calculate now */
#define RANGING_SUCCESSFULL 4

static MyInt16T rcwd = 0;

#ifndef CONFIG_NTRX_IRQ
static MyBoolT trxPollMode = TRUE;
#endif

#ifdef CONFIG_TRAFFIC_LED
#include "led.h"
MyDword32T txLED;
MyDword32T rxLED;
MyDword32T errLED;
MyDword32T clbLED;
#define LED_DELAY 5
#define TRIGGER_LED0();     {   txLED = hwclock () + LED_DELAY; \
                                LED (4, LED_ON); \
                            }
#define TRIGGER_LED1();     {   rxLED = hwclock () + LED_DELAY; \
                                LED (4, LED_ON); \
                            }
#define TRIGGER_LED2();     {   errLED = hwclock () + LED_DELAY; \
                                LED (5, LED_ON); \
                            }
#define TRIGGER_LED3();     {   clbLED = hwclock () + LED_DELAY; \
                                LED (6, LED_ON); \
                            }
#define UpdateLEDs();       {   MyDword32T tiStamp; \
                                tiStamp = hwclock (); \
                                if (txLED < tiStamp) LED (4, LED_OFF);\
                                if (rxLED < tiStamp) LED (4, LED_OFF); \
                                if (errLED < tiStamp) LED (5, LED_OFF); \
                                if (clbLED < tiStamp) LED (6, LED_OFF); \
                            }
#else
#   define TRIGGER_LED0();
#   define TRIGGER_LED1();
#   define TRIGGER_LED2();
#   define TRIGGER_LED3();
#   define UpdateLEDs();
#endif /* CONFIG_TRAFFIC_LED */

extern void ApplCallback (MyByte8T *addr, MyByte8T *payload, MyInt16T len);

extern void Callback_Ack(MyByte8T arqCount);


#define SendMsgUp(p,q)  ApplCallback (p,q)

/* Interrupt flags in the nanoNET TRX chip */
#define NTRX_TX_START (0x01 << NA_TxCmdStart_B)
#define NTRX_TX_BUFF0 (0x01 << NA_TxBufferCmd_LSB)
#define NTRX_TX_BUFF1 (0x01 << NA_TxBufferCmd_MSB)
#define NTRX_TX_ARQ   (0x01 << NA_TxArq_B)
#define TxEND         (0x01 << NA_TxEnd_B)
#define RxEND         (0x01 << NA_RxEnd_B)
#define RxOVERFLOW    (0x01 << NA_RxOverflow_B)

#define TX_IRQ_MASK TxEND
#define RX_IRQ_MASK RxEND
#define NTRX_TX_HEADER (NA_RamTxDstAddr_O + 8)

/* delay time for recalibration */
MyDword32T calDelay = CONFIG_NTRX_RECAL_DELAY;
MyDword32T tiRecal;
/* local variables used by the ntrx driver */
MyByte8T txIrq;
MyByte8T rxIrq;
MyBoolT  ntrxTxPoll;
MyByte8T ntrxArqCount;

MyByte8T bbIrq;
MyByte8T bbtimerStatus;
MyByte8T lState,rState,rDest;



void NTRXTxEnd (void);

/*void NTRXSetPollMode (MyByte8T mode)
{
	trxPollMode = mode;
}*/

void NTRXSetCalInterval (MyDword32T ci)
{
	calDelay = ci;
}

/**
 * NTRXStart:
 *
 * NTRXStart() enables the tx and rx interrupts of the nanoNET TRX chip
 * and resets interrupt variables. This function should only be executed
 * once the nanoNET TRX chip is initialized.
 *
 * Returns: TRUE if everything O.K. else FALSE
 *
 */
MyBoolT NTRXRestart (void)
{
    MyByte8T rxIrq_ = 0xFF;
    MyByte8T txIrq_ = TX_IRQ_MASK;

    /*
     * clear any pending interrupts in the nanoNET TRX chip
     */
    NTRXSetRegister (NA_TxIntsReset, rxIrq_);
    NTRXSetRegister (NA_RxIntsReset, rxIrq_);

    /*
     * allow some events in the receiver to trigger an interrupt
     */
    if (NTRXGetRxCRC2mode() == TRUE)
    {rxIrq_ =  RxEND;}
    else
    {rxIrq_ = (RxEND | RxOVERFLOW);}

    /*
     * select specific events in the nanoNET TRX chip to generate interrupts
     */
    NTRXSetRegister (NA_RxIntsEn, rxIrq_);
    NTRXSetRegister (NA_TxIntsEn, txIrq_);
    txIrqStatus = 0;
    rxIrqStatus = 0;
    txIrq_ = 0;
    rxIrq_ = 0;
    bbIrq = 0;

    /*
     * allow the the nanoNET TRX chip to drive the interrupt line
     */
    NTRXSetRegister (NA_RxIrqEnable, TRUE);
    NTRXSetRegister (NA_TxIrqEnable, TRUE);

    /*
     * start the receiver of the TRX chip
     */
    NTRXSetRegister (NA_RxCmdStart, TRUE);

    ntrxRun      = TRUE;
    ntrxState    = TxIDLE;
    ntrxTxPoll   = FALSE;
    /*
     * select message type date for transmitted messages
     */
    NTRXSetRegister (NA_TxPacketType, frmTypeData);
    /*
     * enable retransmissions
     */
    NTRXSetRegister (NA_TxArq, TRUE);
    return TRUE;
}

/**
 * NTRXStart:
 *
 * NTRXStart() enables all interrupts of the nanoNET TRX chip
 *
 * Returns: TRUE if everything O.K. else FALSE
 *
 */
MyBoolT NTRXStart (void)
{
  return NTRXRestart ();
}



/**
 * NTRXTxData:
 *
 * @payload: -input- data to be transmitted to destination
 * @len:     -input- length of payload to be written into transmit buffer
 *
 * NTRXTxData() writes len bytes of payload to transmit buffer of nanoNET TRX chip
 *
 * Returns: none
 *
 */
void NTRXTxData (MyPtrT payload, MyWord16T len)
{
    NTRXSetIndexReg (3);
    NTRXWriteSPI (NA_RamTxBuffer_O & 0xff, (MyByte8T *)payload, (MyByte8T)(len & 0xff));
}

/**
 * NTRXTxHeader:
 *
 * @addr: -input- data to be transmitted to destination
 * @len:  -input- length of payload to be written into transmit buffer
 * @rsvd: -input- reserved for future use
 * @seq_n: -input- sequence bit for detecting dublicated messages
 * @flush: -input- flush bit for syncronizing with peer
 *
 * NTRXTxData() writes len bytes of payload to transmit buffer of nanoNET TRX chip
 *
 * Returns: none
 *
 */
void NTRXTxHeader (MyByte8T *addr, MyWord16T len, MyByte8T rsvd, MyByte8T seq_n, MyByte8T flush)
{
    MyByte8T txLen[8];

    NTRXSetIndexReg (0);

    /* copy the destination address to temp buffer */
    NTRXWriteSPI (NA_RamTxDstAddr_O, addr, 6);


    /* merge the three bits into the temp buffer */
    txLen[0] = (MyByte8T)len;
    txLen[1] = (len & 0x1F00) >> 8;
    txLen[1] |= (rsvd == 1)  ? 0x20 : 0;
    txLen[1] |= (seq_n == 1) ? 0x40 : 0;
    txLen[1] |= (flush == 1) ? 0x80 : 0;

    NTRXWriteSPI (NA_RamTxLength_O, txLen, 2);
}


/**
 * NTRXTxStart:
 *
 * NTRXTxStart() marks the transmit buffer as valid and starts the transmission
 *
 * Returns: none
 *
 */
void NTRXTxStart (void)
{
    MyByte8T value;

    /* mark buffers as valid and start transmission */
    value = (NTRX_TX_BUFF0 | NTRX_TX_BUFF1 | NTRX_TX_START);
    NTRXWriteSingleSPI (NA_TxBufferCmd_O, value);
#   ifdef CONFIG_TRAFFIC_LED
    TRIGGER_LED0();
#   endif /* CONFIG_TRAFFIC_LED */
}

/**
 * NTRXSendMessage:
 *
 * @payload: -input- data to be transmitted to destination
 * @len:     -input- length of payload to be written into transmit buffer
 *
 * NTRXSendMessage() writes len bytes of payload to transmit buffer of
 * nanonet TRX chip and set MAC header and start transmission
 *
 * Returns: none
 *
 */
void NTRXSendMessage (MyByte8T *addr, MyPtrT payload, MyWord16T len)
{
    MyByte8T FragC, SeqN, LCh;

    FragC=(lState&0x01)?1:0;        /* fragmentation bit - not used in TRX driver demo */
    SeqN=(lState&0x02)?1:0;         /* sequence N bit - not used in TRX driver demo */
    LCh=(lState&0x04)?1:0;          /* logical channel bit - not used in TRX driver demo */

    /* write user data to transmit buffer in ntrx chip */
    NTRXTxData (payload, len);

    /* write header data to ntrx chip */
    NTRXTxHeader (addr, len, FragC, SeqN, LCh);

    ntrxState     = TxSEND;
    NTRXTxStart ();

    if (ntrxState != TxIDLE)
    {
        while (ntrxState != TxWAIT) 
					nanoInterrupt ();
        NTRXTxEnd ();
    }

}


/**
 * NTRXGetTxARQCnt:
 *
 * NTRXGetTxARQCnt() provides the number of transmission tries needed by the
 * nanoNET TRX chip for determining a successful or unsuccessful transmission
 *
 * Returns: number of transmissions tries needed for last message
 *
 */
MyByte8T NTRXGetTxARQCnt (void)
{
    return ntrxArqCount;
}

/**
 * NTRXTxEnd:
 *
 * NTRXTxEnd() finish transmission and reset internal state.
 *
 * Returns: none
 *
 */
void NTRXTxEnd (void)
{
    MyByte8T maxArqCount;
    /*
     * get number of transmissions needed to last message
     */
    NTRXGetRegister (NA_TxArqCnt, &ntrxArqCount);
    maxArqCount = NTRXGetTxARQmax();

    if ((ntrxArqCount > maxArqCount) && (ntrxState == TxWAIT))
    {
	rcwd++;
	txIrq &= ~(TxEND);
	ntrxState = TxIDLE;
	if (ntrxCal != 0)
	{
#	   ifdef CONFIG_NTRX_AUTO_RECALIB
       tiRecal = hwclock() + calDelay;
#      endif
       NTRXAllCalibration ();
       rcwd = 0;
#      ifdef CONFIG_RECALIB_LED
       TRIGGER_LED3();
#      endif /*  CONFIG_RECALIB_LED */
	}
	lState = RANGING_READY;

#   	ifdef CONFIG_TRAFFIC_LED
    	TRIGGER_LED2();
#   	endif /* CONFIG_TRAFFIC_LED */

    }else{
	txIrq &= ~(TxEND);
	ntrxState = TxIDLE;
	if (lState != RANGING_READY)
	{
		RangingCallback_Ack(ntrxArqCount);
		lState = RANGING_ANSWER1;
	}

    }
}




/**
 * NTRXRxReceive:
 *
 * NTRXRxReceive() read out the payload of a received message and
 * calls the upper layer/application
 *
 * Returns: none
 *
 */
void NTRXRxReceive (void)
{
    MyByte8T       status;
    MyByte8T       *rxPayload = 0;
    MyByte8T       len = 0;
    MyByte8T       value;
    MyByte8T       source[DIMADDRESS];
    MyByte8T       dest[DIMADDRESS];
    MyByte8T       reg[2];

    SetRxStop ();
#   ifdef CONFIG_TRAFFIC_LED
    TRIGGER_LED1 ();
#   endif /* CONFIG_TRAFFIC_LED */

    NTRXReadSPI (NA_RxCrc2Stat_O, &status, 1);

    /* check if data is valid */
    if ((status & (1 << NA_RxCrc2Stat_B)) == (1 << NA_RxCrc2Stat_B))
    {
        rxPayload = buff_rx;
        NTRXSetIndexReg (0);
        /* read destination address */
        NTRXReadSPI (NA_RamRxDstAddr_O, dest, sizeof(MyAddressT));
        /* read source address */
        NTRXReadSPI (NA_RamRxSrcAddr_O, source, sizeof(MyAddressT));
        /* read length plus additionl bits */
        NTRXReadSPI (NA_RamRxLength_O, reg, 2);

        rState = reg[1]>>5;

		len  = (reg[0]  | (reg[1] & 0x1F) << 8);
        if (len > 128)
        {
            len = 0;
        }
        else
        {
            NTRXSetIndexReg (2);
            /* read user data */
            NTRXReadSPI ((NA_RamRxBuffer_O & 0xFF), rxPayload, len);
        }
    }
    value = (MyByte8T)(ntrxShadowReg[NA_RxCmdStart_O]
                            | (1 << NA_RxCmdStart_B)
                            | (1 << NA_RxBufferCmd_LSB)
                            | (1 << NA_RxBufferCmd_MSB));
    /* restart receiver */
    NTRXWriteSingleSPI (NA_RxCmdStart_O, value);
    rxIrq = 0;

    if (len > 0)
    {
        if(lState == RANGING_READY && rState == RANGING_START)
		{
			rDest = source[0];
			
			/* received ranging data to RangingCallback_Rx 
			 * (without protocol header stuff) 
			 */
			RangingCallback_Rx(rxPayload, len);
		
			lState = RANGING_ANSWER1;
			/* send ranging packet */
			RangingMode(source);
				
			lState = RANGING_ANSWER2;
			/* send ranging packet with TxEnd information 
			* from RangingMode before */
			RangingMode(source);
			lState = RANGING_READY;
		}else if(lState == RANGING_ANSWER1 && rDest == source[0] && rState == RANGING_ANSWER1)
		{
			/* received ranging data to RangingCallback_Rx 
			* (without protocol header stuff) 
			*/
			RangingCallback_Rx(rxPayload, len);
			lState = RANGING_ANSWER2;
		}
		else if(lState == RANGING_ANSWER2 && rDest == source[0] && rState == RANGING_ANSWER2)
		{
			/* received ranging data to RangingCallback_Rx 
			*(without protocol header stuff) 
			*/
			RangingCallback_Rx(rxPayload, len);
			lState = RANGING_SUCCESSFULL;
		}else if(lState == RANGING_READY && rState == RANGING_READY)
		{
			/* call application with received data */
			ApplCallback (source, rxPayload, len);
		}
    }
}


/**
 * NTRXUpdate:
 *
 * NTRXUpdate() operates the complete receive part of the driver. It serves
 * receiver interrupt flags and picks up the received frame.
 *
 * Returns: none
 *
 */
void NTRXUpdate (void)
{
#   ifndef CONFIG_NTRX_IRQ
    if (trxPollMode == TRUE)
    {
        nanoInterrupt ();
    }
#   endif

#   ifdef CONFIG_TRAFFIC_LED
    UpdateLEDs ();
#   endif /* CONFIG_TRAFFIC_LED */

    if (ntrxState == TxWAIT)
    {
        NTRXTxEnd ();
    }

    /* check if nanoNET TRX chip has received valid data */
    if (ntrxState == TxIDLE)
    {
        if ((rxIrq & RxEND) == RxEND )
        {
                NTRXRxReceive ();

        }
    }
#   ifdef CONFIG_NTRX_AUTO_RECALIB
    if (calDelay != 0)
    {
        if ((rcwd > 3) || (tiRecal < hwclock()))
        {
#           ifdef CONFIG_NTRX_AUTO_RECALIB
            tiRecal = hwclock() + calDelay;
#           endif
            NTRXAllCalibration ();
            rcwd = 0;
#           ifdef CONFIG_RECALIB_LED
            TRIGGER_LED3();
#           endif /*  CONFIG_RECALIB_LED */
        }
    }
#   endif
}

/**
 * NTRXRange:
 *
 * @dest: -input- destination address of ranging partner
 *
 * NTRXRange() This function does all necessary steps for a full
 *             ranging cycle and return the result to the caller.
 *
 * Returns: ranging result
 *
 */
MyDouble32T NTRXRange(MyAddrT dest)
{

	if (lState != RANGING_READY) 
	{
		return -2;
	}

	lState = RANGING_START;
	rDest = dest[0];

	RangingMode(dest);

	bbIrq = 0;
	NTRXStartBasebandTimer(RANGING_TIMEOUT);

	do{
		
		nanoInterrupt ();

		/* check if nanoNET TRX chip has received valid data */
		if (ntrxState == TxIDLE)
		{
			if ((rxIrq & RxEND) == RxEND )
			{
				NTRXRxReceive ();
			}
		}
		
		if(bbIrq) 
		{
			lState = RANGING_READY;
			return -3;
		}
	}while(lState < RANGING_SUCCESSFULL);
	
	NTRXStopBasebandTimer();

	lState = RANGING_READY;

	return getDistance();
}


/**
 * nanoInterrupt:
 *
 * nanoInterrupt() is an interrupt service routine of the nanochip.
 * It updates the TX and RX status flags.
 *
 */
extern int is_pending;
#include "led.h"
void __attribute((interrupt(2))) msp430_port2_handler()
{	
	int n = P2IFG & P2IE;

	if (n & (1 << 0)) {
		P2IFG &= ~(1 << 0);
		return;
	}
	if (n & (1 << 1)) {
		P2IFG &= ~(1 << 1);
		return;
	}
	if (n & (1 << 2)) {
		P2IFG &= ~(1 << 2);
		return;
	}
	if (n & (1 << 3)) {
		P2IFG &= ~(1 << 3);
		return;
	}
	if (n & (1 << 4)) {
		P2IFG &= ~(1 << 4);
		return;
	}
	if (n & (1 << 5)) {
		P2IFG &= ~(1 << 5);
		return;
	}
	if (n & (1 << 6)) {
		P2IFG &= ~(1 << 6);
		return;
	}
	if (n & (1 << 7)) {
		P2IFG &= ~(1 << 7);
		//nanoInterrupt();
	
		//TODO: Need this!
		//enqueue_bh(NTRXUpdate, NULL);

#ifdef USE_UCIRQ
		if (!is_pending)
			 is_pending = 1;
#endif

		led2_toggle();

		//NTRXUpdate();

		//TRACE("UCIRQ!\n\r");
		return;
	}

}

//P2.7/TA0 UCIRQ
inline void nanoInterrupt (void)
{
	/* we have received an interrupt and neede to find out what caused it */
	NTRXReadSPI (NA_TxIntsRawStat_O, ntrxIrqStatus, 2);

	/* check if it was the transmitter */
	if (txIrqStatus != 0)
	{
		/* clear interrupts */
		NTRXSetRegister (NA_TxIntsReset, txIrqStatus);
		txIrq |= txIrqStatus & TX_IRQ_MASK;
		if ((txIrq & TxEND) == TxEND)
		{
			ntrxState = TxWAIT;
		}
	}
	/* check if it was the receiver */
	if (rxIrqStatus != 0)
	{
		/* clear interrupts */
		NTRXSetRegister (NA_RxIntsReset, rxIrqStatus);
		rxIrq |= rxIrqStatus & RX_IRQ_MASK;
	}

	//BASEBAND
	NTRXReadSPI(NA_BbTimerIrqStatus_O,&bbtimerStatus,1);
	/* check if it was the basebandtimer */
	if ((bbtimerStatus&0x40) != 0)
	{
		NTRXStopBasebandTimer();
		bbIrq = 1;
		bbtimerStatus = 0;
	}	
}

