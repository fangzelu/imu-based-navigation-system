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
 *    NA5TRx initialisation
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include "config.h"
#include "ntrxtypes.h"
// #include <stdio.h>

#include    "ntrxutil.h"
#include    "hwclock.h"
#include    "nnspi.h"
#include    "led.h"
//#include    <avr/pgmspace.h>
//#include		"usart.h"

//#define ReadRomByte(p) pgm_read_byte(&(p))


//extern void error_handler (MyInt16T e);

/*
 * do not change this syncword, since not every 64 bit sequence is valid
 */
const MyByte8T TRX_SYNC_WORD[] = {0xAB,0x69,0xCA,0x94,0x92,0xD5,0x2C,0xAB};
const MyByte8T iVal[] = {0x12, 0x03, 0x55, 0x05, 0x66, 0x06, 0x77, 0x07};
MyBoolT	first = TRUE;

/*void test()
{
	TRACE("TEST!\n\r");
}*/

/**
 * NtrxInit:
 *
 * NtrxInit() initializes the entire necessary hardware parts of
 * the nanoNET TRX chip. It also initializes all necessary software data.
 *
 * Returns: TRUE if everything O.K. else FALSE
 */
MyBoolT NtrxInit(void)
{
	/*TRACE("NTRXInit()\n\r");*/


	if (first == TRUE)
	{
		first = FALSE;
#		ifdef CONFIG_DEFAULT_TRX_22MHZ_1000NS
		settingVal.bw = NA_22MHz;
		settingVal.sd = NA_1us;
		settingVal.sr = NA_250k_S;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=0.0; /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_1000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_2000NS
		settingVal.bw = NA_22MHz;
		settingVal.sd = NA_2us;
		settingVal.sr = NA_500k_S;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=0.0; /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_2000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_4000NS
		settingVal.bw = NA_22MHz;
		settingVal.sd = NA_4us;
		settingVal.sr = NA_250k_S;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=0.0; /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_4000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_8000NS
		settingVal.bw = NA_22MHz;
		settingVal.sd = NA_8us;
		settingVal.sr = NA_125k_S;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=0.0; /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_8000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_16000NS
		settingVal.bw = NA_22MHz;
		settingVal.sd = NA_8us;
		settingVal.sr = NA_62k5_S;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=0.0; /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_16000NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_500NS
		settingVal.bw = NA_80MHz;
		settingVal.sd = NA_500ns;
		settingVal.sr = NA_2M_S;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=68.929336; 
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_500NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_1000NS
		settingVal.bw = NA_80MHz;
		settingVal.sd = NA_1us;
		settingVal.sr = NA_1M_S;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=122.492363;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_1000NS */ 

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_2000NS
		settingVal.bw = NA_80MHz;
		settingVal.sd = NA_2us;
		settingVal.sr = NA_500k_S;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=229.490053;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_2000NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_4000NS
		settingVal.bw = NA_80MHz;
		settingVal.sd = NA_4us;
		settingVal.sr = NA_250k_S;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_FIX_MODE;
		settingVal.rangingConst=445.584595;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_4000NS */


#		ifdef CONFIG_DEFAULT_TRX_22MHZ_HR_4000NS
		settingVal.bw = NA_22MHz_HR;
		settingVal.sd = NA_4us;
		settingVal.sr = NA_250k_S;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_MAP_MODE;
		settingVal.rangingConst=445.553589;
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_HR_4000NS */


		if (NTRXCheckTable() == FALSE)
		{
			error_handler(1, 1000);
		}

		/* init SPI HW&SW */
		InitSPI();
		NanoReset();

		//TRACE("Q\n\r");

		/*
		 * configure SPI output of chip MSB first / push pull
		 */
		SetupSPI ();
		
		//TRACE("W\n\r");

		/* initialize shadow registers */
		NTRXInitShadowRegister ();

		hwdelay(0); //!! Some short delay seems necessary here??
		/* check connection and firmware version and revision */
		if (!NTRXCheckVerRev())
		{
			error_handler(2, 1000);
		}
	}
	else
	{
		/* configure SPI output of chip MSB first / push pull */
		SetupSPI ();
		/* initialize shadow registers */
		NTRXInitShadowRegister ();
	}

	//TRACE("A\n\r");

	NTRXSetupTRxMode (settingVal.bw, settingVal.sd, settingVal.sr);

	//TRACE("B\n\r");

	ntrxRun = FALSE;
	
	return TRUE;
}
