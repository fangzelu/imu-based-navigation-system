/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2002 and 2005
 * Nanotron Technologies
 *
 * Author: B. Jozefini
 *
 * Description :
 *    This file contains the source code for the initialization and operation
 *    of the SPI bus
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include <io.h>
#include "config.h"
#include "ntrxtypes.h"
#include "nnspi.h"
#include "hwclock.h"
#include "led.h"
#include "usart.h"
#include "common_def.h"
#include "na5tr1.h"

/**
 * NanoReset:
 *
 * NanoReset() resets the nanoNET chip and adjusts the pin level.
 *
 * Returns: none
 */
void NanoReset(void)
{
	SET_PORT_LOW_RF_PONRESET_PIN();
	//P3OUT &= ~(1<<0); //P3.0 POnReset
	
	hwdelay(1000);

	SET_PORT_HIGH_RF_PONRESET_PIN();
	//P3OUT |= (1<<0); //P3.0 POnReset
	
	hwdelay (1000);	
}

/**
 * InitSPI:
 *
 * InitSPI() initializes the SCI2 module and adjusts the pin level
 *
 * Returns: none
 */

void InitSPI(void)
{
	
	CRITICAL_SECTION_START();

	//NA5TR1 Initial Pin Settings
	//P2DIR |= (1<<6); //SPISSN: Output (P2.6/ADC12CLK)
	//P2SEL &= ~(1<<6);
	//P2OUT |= (1<<6); //High
	SET_PORT_OUT_RF_SPISSN_PIN();
	SET_PORT_IO_RF_SPISSN_PIN();
	SET_PORT_HIGH_RF_SPISSN_PIN();

	//P3DIR |= (1<<0); //POnReset: Output (P3.0/STE0)
	//P3SEL &= ~(1<<0);
	//P3OUT |= (1<<0); //High
	SET_PORT_OUT_RF_PONRESET_PIN();
	SET_PORT_IO_RF_PONRESET_PIN();
	SET_PORT_HIGH_RF_PONRESET_PIN();

	//P5DIR |= (1<<1); //SIMO1 (P5.1/SIMO1)
	//P5SEL |= (1<<1);
	SET_PORT_OUT_RF_SPIRXD_PIN();
	SET_PORT_MOD_RF_SPIRXD_PIN();

	//P5DIR &= ~(1<<2); //SOMI1 (P5.2/SOMI1)
	//P5SEL |= (1<<2);
	SET_PORT_IN_RF_SPITXD_PIN();
	SET_PORT_MOD_RF_SPITXD_PIN();

	//P5DIR |= (1<<3); //UCLK1 (P5.3/UCLK1)
	//P5SEL |= (1<<3);
	SET_PORT_OUT_RF_SPICLK_PIN();
	SET_PORT_MOD_RF_SPICLK_PIN();

	//P2DIR &= ~(1<<7); //UCIRQ (P2.7/TA0)
	//P2SEL &= ~(1<<7);
	//P2IE |= (1<<7);   //UCIRQ enable
	//P2IFG &= ~(1<<7); //UCIRQ clear
	//P2IES &= ~(1<<7); //UCIRQ high active
	INIT_RF_UCIRQ_INT_PIN();
	SET_POSEDGE_RF_UCIRQ_INT();	


	//P1DIR &= ~(1<<0); //UCRESET (P1.0/TACLK0)
	//P1SEL &= ~(1<<0);
	SET_PORT_IN_RF_UCRESET_PIN();
	SET_PORT_IO_RF_UCRESET_PIN();

	//P1DIR &= ~(1<<3); //TX/RX (P1.3/TA2)
	//P1SEL &= ~(1<<3);
	SET_PORT_IN_RF_TXRX_PIN();
	SET_PORT_IO_RF_TXRX_PIN();

	//P4DIR |= (1<<2); //Nano_On/Off (P4.2/TB2)
	//P4SEL &= ~(1<<2);
	//P4OUT |= (1<<2);
	SET_PORT_OUT_RF_LDO_CE_PIN();
	SET_PORT_IO_RF_LDO_CE_PIN();
	SET_PORT_HIGH_RF_LDO_CE_PIN();
	
	//Init SPI1
	UCTL1 = CHAR + SYNC + MM + SWRST; //8bit data, SPI function is selected, Master mode, logic hold
	ME2 = USPIE1; //Enable SPI0
	UTCTL1 = CKPL + SSEL1 + SSEL0 + STC; //UCLK Inactive High, SMCLK, 3-pin SPI mode
	UBR01 = 0X02; //Baudrate: UCLK/2
	UBR11 = 0X00;
	UMCTL1 = 0; //No Modulation
	UCTL1 &= ~SWRST; //Init USART State Machine

	CRITICAL_SECTION_END();

}

void SetupSPI (void)
{
	//uint8_t buff[1] = { 0xA5 };

	CRITICAL_SECTION_START();

	//MyByte8T value = 0xC3; //IRQ open-drain, low active, SPI Tx push-pull, MSB First (mirrored)
	//MyByte8T value = 0xDB; //IRQ push-pull, low active, SPI Tx push-pull, MSB First (mirrored)

	MyByte8T value = 0xFF; //IRQ push-pull, high active, SPI Tx push-pull, MSB First (mirrored)
	//MyByte8T value = 0xE7; //IRQ open-drain, high active, SPI Tx push-pull, MSB First (mirrored)
	NTRXWriteSPI(0x00, &value, 1);

	//0x08 : LoIRQ Enable
	//0x04 : BbTimer Enable
	//0x02 : RxIRQ Enable
	//0x01 : TxIRQ Enable
	//value = 0x07; //TxIRQ, RxIRQ, BbTimerIRQ Enable.
	value = 0x02; //RxIRQ Enable.
	NTRXWriteSPI(0x0F, &value, 1);
	
	//NTRXWriteSPI(0x00, &value, 1);

	//NTRXReadSPI(0x0F, buff, 1);
	//NTRXReadSPI(0x00, buff, 1);
	//TRACE("buff=0x%02X\n\r", buff[0]);
	//TRACE("Hello?\n\r");
	//disp_led(buff[0]);
	//TRACE("Hi!!");
	//halt();

	CRITICAL_SECTION_END();
}

void NTRXReadSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{
  if (len > 0x80 || len == 0) 
		return;

	CRITICAL_SECTION_START();

	//PORTD &= ~(1<<PORTD6);
	//P2OUT &= ~(1<<6); //P2.6 SPISSN Clear
	SET_PORT_LOW_RF_SPISSN_PIN();

	//SPDR = (len & 0x7F);
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = (len & 0x7F); 

	while(!(IFG2 & URXIFG1));

	//SPDR = address;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = address;

	while(!(IFG2 & URXIFG1));

	while( len-- > 0 )
	{
		//SPDR = 0xff;
		//while(!(SPSR &(1 << SPIF)));
		//*buffer++ = SPDR;
		IFG2 &= ~URXIFG1;
		U1TXBUF = 0xFF;
		while(!(IFG2 & URXIFG1));
		*buffer++ = U1RXBUF;
	}

	//PORTD |= 1<<PORTD6;
	//P2OUT |= (1<<6); //P2.6 SPISSN Set
	SET_PORT_HIGH_RF_SPISSN_PIN();

	CRITICAL_SECTION_END();
}

void NTRXWriteSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{
	CRITICAL_SECTION_START();

	//PORTD &= ~(1<<PORTD6);
	//P2OUT &= ~(1<<6); //P2.6 SPISSN Clear
	SET_PORT_LOW_RF_SPISSN_PIN();

	//SPDR = 0x80 | (len & 0x7F);
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = 0x80 | (len & 0x7F);
	
	while(!(IFG2 & URXIFG1));

	//SPDR = address;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = address;

	while(!(IFG2 & URXIFG1));

	while(len-- > 0)
	{
		//SPDR = *buffer++;
		//while(!(SPSR &(1 << SPIF)));
		IFG2 &= ~URXIFG1;
		U1TXBUF = *buffer++;
		while(!(IFG2 & URXIFG1));
	}

	//PORTD |= 1<<PORTD6;
	//P2OUT |= (1<<6); //P2.6 SPISSN Set
	SET_PORT_HIGH_RF_SPISSN_PIN();

	CRITICAL_SECTION_END();
}

void NTRXReadSingleSPI(MyByte8T address, MyByte8T *buffer)
{
	CRITICAL_SECTION_START();

	//PORTD &= ~(1<<PORTD6);
	//P2OUT &= ~(1<<6); //P2.6 SPISSN Clear
	SET_PORT_LOW_RF_SPISSN_PIN();
	//SPDR = 1;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = 1;
	while(!(IFG2 & URXIFG1));

	//SPDR = address;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = address;
	while(!(IFG2 & URXIFG1));

	//SPDR = 0xff;
	//while(!(SPSR &(1 << SPIF)));
	//*buffer = SPDR;
	IFG2 &= ~URXIFG1;
	U1TXBUF = 0xFF;
	while(!(IFG2 & URXIFG1));
	*buffer = U1RXBUF;

	//PORTD |= 1<<PORTD6;
	//P2OUT |= (1<<6); //P2.6 SPISSN Set
	SET_PORT_HIGH_RF_SPISSN_PIN();

	CRITICAL_SECTION_END();
}

void NTRXWriteSingleSPI(MyByte8T address, MyByte8T buffer)
{
	CRITICAL_SECTION_START();

	//PORTD &= ~(1<<PORTD6);
	//P2OUT &= ~(1<<6); //P2.6 SPISSN Clear
	SET_PORT_LOW_RF_SPISSN_PIN();

	//SPDR = 0x81;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = 0x81;
	while(!(IFG2 & URXIFG1));

	//SPDR = address;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = address;
	while(!(IFG2 & URXIFG1));

	//SPDR = buffer;
	//while(!(SPSR &(1 << SPIF)));
	IFG2 &= ~URXIFG1;
	U1TXBUF = buffer;
	while(!(IFG2 & URXIFG1));

	//PORTD |= 1<<PORTD6;
	//P2OUT |= (1<<6); //P2.6 SPISSN Set
	SET_PORT_HIGH_RF_SPISSN_PIN();	
	CRITICAL_SECTION_END();
}

/**
 * nanosetIRQ:
 * @value: -input- boolean enables (TRUE) or disables (FALSE) the external
 *                 interrupt request (interrupts from the nanochip)
 *
 * Returns: none
 */

void nanosetIRQ(MyBoolT value)
{
	CRITICAL_SECTION_START();

  if (value == TRUE)
	{
		//EIMSK |= (1 << INT4);
		//P2IE |= (1<<7); //UCIRQ P2.7 Interrupt Enable
		ENABLE_RF_UCIRQ_INT();	
		//P2IFG &= ~(1<<7);
	}
	else
	{
		//EIMSK &= ~(1 << INT4);
		//P2IE &= ~(1<<7); //UCIRQ P2.7 Interrupt Disable
		DISABLE_RF_UCIRQ_INT();
		//P2IFG &= ~(1<<7);
	}

	CRITICAL_SECTION_END();
}

