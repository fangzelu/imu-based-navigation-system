/* $Id$ */

/**
 * @file app.c
 * @date 2007-10-08
 * @c Copyright 2007- Nanotron Technologies
 * @author Christian Bock
 *
 * @brief This file contains the source code for the ranging demo
 *
 * Give a more detailled description here.
 *
 */

/*
 * $Log$
 */


#include	"ntrxdil.h"
#include	"hwclock.h"
#include	"ntrxutil.h"

#include	"nnspi.h"
#include	<stdio.h>
#include	<stdlib.h>

#include "usart.h"

/*
 * variables used by the demo applicataion
 */
typedef struct appMemS
{
	MyAddrT    src;
	MyAddrT    dest;
} appMemT;

appMemT appM;
appMemT *app;

/*******************************************************************/
/**
 * @brief Upstream message handling.
 * @par payload containing data
 * @par len length of payload
 *
 * ApplCallback represents the upstream part of the application.
 *
 */
void ApplCallback (MyAddrT source, MyByte8T *payload, MyInt16T len)
{
}



/*******************************************************************/
/**
 * @brief Poll function of the application.
 *
 * Will be called from the system main loop in frequent intervalls.
*/
uint32_t RF_seq = 0;
int16_t RF_frac = 0, RF_number = 0;

#include <delay.h>
void	PollApplication(void)
{

#if (defined(RANGE_DEMO_ANCHOR))
	MyDouble32T dist;
	
	dist = NTRXRange(app->dest);

	//printf_P(PSTR("%7.2f\n"),dist);
	//TRACE("%d\n\r", (int)dist);

	if (dist < 0.0)
	{
		//TRACE("%d\n\r", (int)dist);
		RF_number = dist;
		RF_frac = 0;
	}
	else
	{
		RF_frac = (uint16_t)(dist*100.0) - ((uint16_t)dist*100);
		RF_number = (uint16_t)dist;
		//TRACE("%lu: ", RF_seq);
		//TRACE("%3u.%02u\n\r", RF_number, RF_frac);
		//TRACE("%5u.%02u\n\r", number, frac);
	}

	RF_seq++;

	/*if ((int)dist != -1)
		error_handler(7,1000);*/

#ifdef USE_UCIRQ
	msdelay(10);
#endif

#elif (defined(RANGE_DEMO_TAG))

#else
	#error "Neither ANCHOR or TAG."
#endif
}



/*******************************************************************/
/**
 * @brief Initialize the application.
 *
 * Initialize the application.
*/
void	InitApplication(void)
{


	app = &appM;


#if (defined(RANGE_DEMO_ANCHOR))
/***************** ANCHOR CODE *****************/
	app->src[0] = 0x10;
	app->src[1] = 0x00;
	app->src[2] = 0x00;
	app->src[3] = 0x00;
	app->src[4] = 0x00;
	app->src[5] = 0x00;
	/* write the source address to the TRX chip */
	NTRXSetStaAddress (app->src);

	app->dest[0] = 0x11;
	app->dest[1] = 0x00;
	app->dest[2] = 0x00;
	app->dest[3] = 0x00;
	app->dest[4] = 0x00;
	app->dest[5] = 0x00;
#elif (defined(RANGE_DEMO_TAG))
	/****************** TAG CODE ******************/
	app->src[0] = 0x11;
	app->src[1] = 0x00;
	app->src[2] = 0x00;
	app->src[3] = 0x00;
	app->src[4] = 0x00;
	app->src[5] = 0x00;
	/* write the source address to the TRX chip */
	NTRXSetStaAddress (app->src);

	app->dest[0] = 0x00;
	app->dest[1] = 0x00;
	app->dest[2] = 0x00;
	app->dest[3] = 0x00;
	app->dest[4] = 0x00;
	app->dest[5] = 0x00;
#else
	#error "Neither ANCHOR or TAG."
#endif

}





