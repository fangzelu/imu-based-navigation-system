/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2002 and 2005
 * Nanotron Technologies
 *
 * Author: S. Rohdemann and B. Jozefini
 *
 * Description :
 *    This file contains the source code for the utility functions
 *    of the Hardware Timer Function.
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include	"config.h"
#include    "ntrxtypes.h"
#include	"hwclock.h"
#include <io.h>
#ifndef __OPTIMIZE__
#	define __OPTIMIZE__ 0
#endif
#include <delay.h>
#include <signal.h>
//#include <interrupt.h>
//#include <wdt.h>

#include "util.h"
#include "led.h"
#include "usart.h"

#define	MILLISECONDS_PER_TICK	10 /* at 7,3728 MHz prescaler 1024 -> TCNT0 = 256 - 72 for 10ms tick */
//#define	STATE_HIGH	5
//#define	STATE_LOW	-5

/*MyBoolT	key_flags[NKEYS] = {
	FALSE, FALSE, FALSE, FALSE
};*/

static	MyDword32T	jiffies = 0;
static	MyDword32T	bogo_mips = 0;



/**
 * hwdelay:
 *
 * Delays the CPU @t microseconds.
 *
 * Returns: none.
 */
/*void	hwdelay(MyDword32T t)
{
  unsigned long lc = t >> 8;

  if (t == 0) return;

  //_delay_loop_2() delays by 4 cycles i.e. 543 ns at 7.373 MHz
  while(lc--) _delay_loop_2(512);
  _delay_loop_2((t & 0xFF)<<1);
}*/

/**
 * hwclockRestart:
 *
 * clock tick initialization function.
 *
 * Returns: none.
 */
#define MAX_TIMER_TICK 320
void hwclockRestart (MyDword32T start)
{
	/*TCCR0 = (1 << CS02) | (1 << CS01)| (1 << CS00);  //Timer0 clk/1024 normal mode int every 0.13888 ms
		TIMSK = (1 << TOIE0);
		TCNT0 = 256 - 72;*/

	// Initialize TimerA 
	TACTL = TACLR + MC_0; // Halt TimerA

	// Initialize TimerB  
	TBCTL = TBCLR + MC_0; // Halt TimerB
	/*TBCTL = TBSSEL_ACLK + TBCLR + ID_DIV1 + TBIE; // 32.768KHz
	TBCCTL0 = CCIE;
	TBR = 0;
	TBCCR0 = MAX_TIMER_TICK;
	TBCTL |= MC_2; // continuous mode */

	jiffies = start;
	bogo_mips = 0;
}

/**
 * hwclock_init:
 *
 * clock tick initialization function.
 *
 * Returns: none.
 */
void hwclock_init(void)
{
  //Switches are on Port E
	/*PORTE = (1 << PORTE2)                           //pull-ups for switches
           |(1 << PORTE3)
           |(1 << PORTE6);*/
	//sei ();
	EINT();
	hwclockRestart(0);
}


/**
 * hwclock:
 *
 * clock tick function.
 *
 * Returns: The elapsed time since program start in milliseconds.
 */
MyDword32T	hwclock(void)
{
	return	jiffies;
}

/*#define READ_TBR()    \
	({            \
	 uint16_t t0;    \
	 uint16_t t1 = TBR;  \
	 do { t0 = t1; t1 = TBR; } while( t0 != t1 );  \
	 t1; \
	 })
*/
/**
 * TIMER1 overflow interrupt:
 *
 * Interrupt service routine for the hardware timer
 *
 * Returns: none
 */
//SIGNAL(SIG_OVERFLOW0)
interrupt(26) tbccr0_handler()  
{
	//TBCTL = TBCLR + MC_0;
	//TBCTL = TBSSEL_ACLK + TBCLR + ID_DIV1 + TBIE; // 32.768KHz
	TBR = 0;
	TBCCR0 = MAX_TIMER_TICK;
	//TBCTL |= MC_2;

	/*READ_TBR();
	TBCCR0 = MAX_TIMER_TICK;*/

	//led3_toggle();
//	int					i;
//	static		int		state[NKEYS] = { 0, 0, 0, 0};
//	unsigned char		portPin[NKEYS] = { 0x40, 0x08, 0x04, 0x01};
//	TCNT0 = 256 - 72;

/*	for(i = 0; i < NKEYS; i++)	{
		if (PINE & portPin[i])
		{
			if(state[i] > STATE_LOW)	{
				state[i]--;
			}
		}	else	{
			if(state[i] < STATE_HIGH)	{
				state[i]++;
			}
		}
		
		if(state[i] == STATE_HIGH)	{
			key_flags[i] = TRUE;
		}
		
		if(state[i] == STATE_LOW)	{
			key_flags[i] = FALSE;
		}
	}
*/
	jiffies += MILLISECONDS_PER_TICK;
}


