/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2007
 * Nanotron Technologies
 *
 * Author: C. Bock
 *
 * Description :
 *    This file contains the source code for the implementation of the
 *    NTRX ranging functions
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include	"ntrxtypes.h"
#include	"OffstMapInit.h"
#include	"ntrxutil.h"
#include	"ntrxdil.h"
#include	"ntrxranging.h"
#include	"nnspi.h"
#include	<string.h>

#define	PHASEOFFSETACK		0
#define	TOAOFFSETMEANACK_L	1
#define	TOAOFFSETMEANACK_H	2
#define	TXRESPTIME_L		3
#define	TXRESPTIME_H		4
#define	PHASEOFFSETDATA		5
#define	TOAOFFSETMEANDATA_L	6
#define	TOAOFFSETMEANDATA_H	7
#define	PREVRXID		8
#define	PREVTXID		9
#define ACTTXID			10

/* length of ranging packet */
#define ARRYLEN  11 

/* speed of our signal in AIR or CABLE [um/ps] */
#define SPEED_OF_AIR 299.792458
#define SPEED_OF_CABLE 299.792458*0.66

/* Measurement difference between remote and local station isn't allowed to
 * be bigger than MAX_DIFF_ALLOWED
 *
 * If its bigger than N [m], it will droped */
#define MAX_DIFF_ALLOWED 6

static MyDouble32T dist(MyByte8T *p1, MyByte8T *p2);
static MyDouble32T delay(MyByte8T *p);

static MyByte8T lrv[ARRYLEN]; /* Local ranging values  */
static MyByte8T rrv[ARRYLEN]; /* Remote ranging values */
static MyByte8T set1[ARRYLEN], set2[ARRYLEN]; /* this holds the collected data for calc */

extern SettingsT settingVal;

/**
 * @brief collects ranging data after receiving a ranging packet
 * @par payload data of received packet
 * @par val length of payload
 *
 * the functions is automaticly called in NTRXReceive
 *
 */
void RangingCallback_Rx(MyByte8T *payload, MyByte8T len)
{
	MyByte8T	ToaOffsetMeanDataValid;

	lrv[ACTTXID]=0x00; 

	/* Read Rx Ranging Registers */
	NTRXReadSPI (NA_ToaOffsetMeanDataValid_O, &ToaOffsetMeanDataValid, 1);
	ToaOffsetMeanDataValid &= (0x01 << NA_ToaOffsetMeanDataValid_B);
	ToaOffsetMeanDataValid = ToaOffsetMeanDataValid>>NA_ToaOffsetMeanDataValid_B;


	if(ToaOffsetMeanDataValid != 1)
	{
		/* TODO: report error */

		memset(lrv,'\0',sizeof(lrv));
		memset(rrv,'\0',sizeof(rrv));
	}else{
			
		if (len==ARRYLEN)
		{
			/* Get the remote ranging values */
			memcpy(rrv,payload,ARRYLEN);
				
			/* Check what matches our local values */
			switch(rrv[ACTTXID])
			{
				case 1 : 
					/* combine data for locally initiated round trip */
					set1[PHASEOFFSETACK]=lrv[PHASEOFFSETACK];
					set1[TOAOFFSETMEANACK_H]=lrv[TOAOFFSETMEANACK_H];
					set1[TOAOFFSETMEANACK_L]=lrv[TOAOFFSETMEANACK_L];
					set1[TXRESPTIME_H]=lrv[TXRESPTIME_H];
					set1[TXRESPTIME_L]=lrv[TXRESPTIME_L];

					set1[PHASEOFFSETDATA]=rrv[PHASEOFFSETDATA];
					set1[TOAOFFSETMEANDATA_H]=rrv[TOAOFFSETMEANDATA_H];
					set1[TOAOFFSETMEANDATA_L]=rrv[TOAOFFSETMEANDATA_L];
					break;
				case 2 :
					/* combine data for remotely initiated round trip */
					set2[PHASEOFFSETACK]=rrv[PHASEOFFSETACK];
					set2[TOAOFFSETMEANACK_H]=rrv[TOAOFFSETMEANACK_H];
					set2[TOAOFFSETMEANACK_L]=rrv[TOAOFFSETMEANACK_L];
					set2[TXRESPTIME_H]=rrv[TXRESPTIME_H];
					set2[TXRESPTIME_L]=rrv[TXRESPTIME_L];
							
					set2[PHASEOFFSETDATA]=lrv[PHASEOFFSETDATA];
					set2[TOAOFFSETMEANDATA_H]=lrv[TOAOFFSETMEANDATA_H];
					set2[TOAOFFSETMEANDATA_L]=lrv[TOAOFFSETMEANDATA_L];
					break;
				default:
					break;
			}

			/* now we can update our local ranging values */
			lrv[PREVRXID]=rrv[ACTTXID];

			NTRXReadSPI (NA_PhaseOffsetData_O, &lrv[PHASEOFFSETDATA], 1);
			lrv[PHASEOFFSETDATA] &= (0x07 << NA_PhaseOffsetData_LSB);
			lrv[PHASEOFFSETDATA] = (lrv[PHASEOFFSETDATA] >> NA_PhaseOffsetData_LSB);	
			
			NTRXReadSPI (NA_ToaOffsetMeanData_O, &lrv[TOAOFFSETMEANDATA_L], 2);
			lrv[TOAOFFSETMEANDATA_H] &= 0x1f;
		}
	}
}

/**
 * @brief collects rangingdata after sending an rangingpacket
 * @par arqCount amount of retransmissions for the last transmitted packet
 *
 * the functions is automaticly called in NTRXTxEnd
 *
 */
void RangingCallback_Ack(MyByte8T arqCount)
{
	MyByte8T	ToaOffsetMeanAckValid;
	/* Read Tx Ranging Registers */
        NTRXReadSPI (NA_ToaOffsetMeanAckValid_O, &ToaOffsetMeanAckValid, 1);
	ToaOffsetMeanAckValid &= (0x01 << NA_ToaOffsetMeanAckValid_B);
	ToaOffsetMeanAckValid = ToaOffsetMeanAckValid>>NA_ToaOffsetMeanAckValid_B;

	if(ToaOffsetMeanAckValid != 1)
	{
		/* TODO: report error */

		memset(lrv,'\0',sizeof(lrv));
		memset(rrv,'\0',sizeof(rrv));		
	}
	else
	{
		NTRXReadSPI (NA_PhaseOffsetAck_O, &lrv[PHASEOFFSETACK], 1);
		lrv[PHASEOFFSETACK] &= (0x07 << NA_PhaseOffsetAck_LSB);
		lrv[PHASEOFFSETACK] = lrv[PHASEOFFSETACK]>>NA_PhaseOffsetAck_LSB;
			
		NTRXReadSPI (NA_ToaOffsetMeanAck_O, &lrv[TOAOFFSETMEANACK_L], 2);	
		lrv[TOAOFFSETMEANACK_H] &= 0x1f;	
			
        	NTRXReadSPI ( NA_TxRespTime_O, &lrv[TXRESPTIME_L], 2);
	}
	
	lrv[PREVTXID]=lrv[ACTTXID]; 
}



/**
 * @brief start ranging
 * @par dest destination address
 *
 * RangingMode handles the start and continue of ranging at all.
 *
 */
void RangingMode(MyAddrT dest)
{
	lrv[ACTTXID]++;
	NTRXSendMessage (dest, lrv, ARRYLEN);
}


/**
 * @brief userinterface for distance calculation
 * @return distance in [m] or [-1] in case of error
 *
 * getDistance calculates indirectly the distance.
 *
 */
MyDouble32T getDistance(void)
{
	MyDouble32T distance;

	distance = dist(&set1[0],&set2[0]);

	/* measurements between 0.00[m] and -0.99[m] will changed to 0.0[m]
	 * measurements below -0.99[m] will changed to -1.0[m] (ERROR) */
	if ((distance < 0.0) && (distance > -1.0)) 
	{
		return 0.0;
	}else if(distance >= 0.0)
	{
		return distance;
	}

	return -1.0;
}

/**
 * @brief distance calculation
 * @par p1 is one set of data which is necessary for calculation distance1
 * @par p2 is one set of data which is necessary for calculation distance2
 * @return distance in [m]
 *
 * dist calculates the distance.
 *
 */
MyDouble32T dist(MyByte8T *p1, MyByte8T *p2) 
{
	MyDouble32T avg = -1.0;
	MyDouble32T speedofmedium = SPEED_OF_AIR;

	/* calculate the one way airtime for local station */
	MyDouble32T distanceD2R = delay(p1);
	/* calculate the one way airtime for remote station */
	MyDouble32T distanceR2D = delay(p2);
	
	
	/* calculate the distance in [m] for local station */
	distanceR2D *=(speedofmedium);
	/* calculate the distance in [m] for remote station */
	distanceD2R *=(speedofmedium);

	/* the difference between the measurement results from local-
	 * and remote station should not be to large */
	if(distanceR2D>distanceD2R)
	{
		if((distanceR2D-distanceD2R)>MAX_DIFF_ALLOWED) return -1.0;
	}else{
		if((distanceD2R-distanceR2D)>MAX_DIFF_ALLOWED) return -1.0;
	}

	avg  = ((distanceR2D + distanceD2R )/2.0); /* [m] */

	return avg;
}


/**
 * @brief delay calculation
 * @par p is one set of data which is necessary for calculation
 * @return delay
 *
 * Delay calculates the airtime of packet WITHOUT processing time in chip.
 * Its a helper function for dist(..,..);
 *
 */
MyDouble32T delay(MyByte8T *p)
{
	/* clock period [MHz] */
	const MyDouble32T clk_4MHz  = 4; 
	const MyDouble32T clk_32MHz = 32;
	/* Scaled 1:20 divider's clock period [MHz] */
	const MyDouble32T clk_lod20 = (2000.0/244175);

	const MyWord16T PulseDetUcMax = 5;
	const MyWord16T PulseDetUcMax_table[16] = {1, 2, 4, 8, 16, 24, 32, 40, 48, 56, 64, 1, 1, 1, 1, 1};

	MyDouble32T res;

	MyDword32T TxRespTime = (p[TXRESPTIME_H]        << 8) | (p[TXRESPTIME_L]);
	MyLong32T  RxUcSum    = (p[TOAOFFSETMEANACK_H]  << 8) | (p[TOAOFFSETMEANACK_L]);
	MyLong32T  TxUcSum    = (p[TOAOFFSETMEANDATA_H] << 8) | (p[TOAOFFSETMEANDATA_L]);

	MyLong32T  RxGateOff  = p[PHASEOFFSETACK]  == 7 ? 7 : 6 - p[PHASEOFFSETACK];
	MyLong32T  TxGateOff  = p[PHASEOFFSETDATA] == 7 ? 7 : 6 - p[PHASEOFFSETDATA];

	if (settingVal.fdma == 0) 
	{
		res = ((TxRespTime)/clk_4MHz 
        	 	- ((TxGateOff+RxGateOff))/clk_32MHz
			- (TxUcSum+RxUcSum)*clk_lod20/(2.0*PulseDetUcMax_table[PulseDetUcMax]))/2.0
			- settingVal.rangingConst;
	}else{
		res = (TxRespTime/clk_4MHz - (TxGateOff+RxGateOff)/clk_32MHz
			-(TxUcSum+RxUcSum)/clk_32MHz*(2.0*PulseDetUcMax_table[PulseDetUcMax]))/2.0
			- settingVal.rangingConst;
	}	
	
	return res;
}


