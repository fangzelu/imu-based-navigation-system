// DCO initialization code (4096 MHz)

#include <io.h>
#include <signal.h>
#include "util.h"
#include "dco.h"

// MSP430 DCO Timer Calibration 
// - This code is written with referencing TinyOS msp430 calibration code.
// - TODO: Test this calibration code
#define ACLK_CALIB_PERIOD 128
#define ACLK_KHZ 32
#define TARGET_DCO_KHZ 4096
#define TARGET_DCO_DELTA (TARGET_DCO_KHZ / ACLK_KHZ * ACLK_CALIB_PERIOD)

#define TARGET_DELTA  2048  // number of 32khz ticks during 65536 ticks at 1mhz
#define MAX_DEVIATION 7   // about 0.35% error

uint16_t msp430_calib_prev;
//uint16_t is_dco_done = 0;

/*static inline void msp430_dco_calib_overflow()
{
	uint16_t now = TBR; 
	uint16_t delta = now - msp430_calib_prev;
	msp430_calib_prev = now;

	if (delta > TARGET_DELTA + MAX_DEVIATION) {
		if (DCOCTL < 0xe0) {
			DCOCTL++;
		} else if ((BCSCTL1 & 7) < 7) {
			BCSCTL1++;
			DCOCTL = 96;
		}
	} else if (delta < TARGET_DELTA - MAX_DEVIATION) {
		if (DCOCTL > 0) {
			DCOCTL--;
		}
		else if ((BCSCTL1 & 7) > 0) {
			BCSCTL1--;
			DCOCTL = 128;
		}
	}
}

INTERRUPT(10) msp430_timer_taiv_handler()
{
	int n = TA0IV;

	switch (n) {
		case 10: 
			if (!is_dco_done) //to make sure DCO does not drift whtn TimeA is being used.
				msp430_dco_calib_overflow();
			break;
	}
}*/


void set_dco_calib(int calib)
{
	BCSCTL1 = (BCSCTL1 & ~0x07) | ((calib >> 8) & 0x07);
	DCOCTL = calib & 0xff;
}

uint16_t test_calib_busywait_delta(int calib)
{
	int8_t aclk_count = 2;
	uint16_t dco_prev = 0;
	uint16_t dco_curr = 0;

	set_dco_calib(calib);

	while (aclk_count-- > 0) {
		TBCCR0 = TBR + ACLK_CALIB_PERIOD; // set next interrupt
		TBCCTL0 &= ~CCIFG; // clear pending interrupt
		while ((TBCCTL0 & CCIFG) == 0); // busy wait
		dco_prev = dco_curr;
		dco_curr = TAR;
	}
	return dco_curr - dco_prev;
}

void busy_calibrate_dco()
{
	int calib;
	int step;

	// setup
	TACTL = TASSEL1 | MC1; // source SMCLK, continuous mode, everything else 0
	TBCTL = TBSSEL0 | MC1;
	BCSCTL1 = XT2OFF | RSEL2;
	BCSCTL2 = 0;
	TBCCTL0 = CM0;

	// calibrate 
	// : Binary search for RSEL,DCO,DCOMOD.
	// : It's okay that RSEL isn't monotonic.

	for (calib = 0, step = 0x800; step != 0; step >>= 1) {
		// if the step is not past the target, commit it
		if (test_calib_busywait_delta(calib|step) <= TARGET_DCO_DELTA)
			calib |= step;
	}

	// if DCOx is 7 (0x0e0 in calib), then the 5-bit MODx is not useable, 
	// set it to 0
	if ((calib & 0x0e0) == 0x0e0)
		calib &= ~0x01f;

	set_dco_calib( calib );
}

void msp430_timer_init()
{
	TACTL = TACLR;
	TAIV = 0;
	TBCTL = TBCLR;
	TBIV = 0;

	busy_calibrate_dco(); // calibration

	DINT();//CRITICAL_SECTION_START();
	BCSCTL1 = 0x80 | (BCSCTL1 & ((0x04 | 0x02) | 0x01));
	BCSCTL2 = 0x04;
	IE1 &= ~(1 << 1); 
	//TAR = 0;
	//TACTL = 0x0200 | 0x0002;
	//TBR = 0;
	//TBCTL = 0x0100 | 0x0002;
	EINT();//CRITICAL_SECTION_END();
}
