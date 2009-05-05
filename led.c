#include "led.h"

uint16_t leds_val = 0;

void leds_set(uint16_t bit, uint16_t mode)
{
	uint16_t prev = leds_val;
	uint16_t on = 0, off = 0;

	bit &= LEDS_MASK;
	
	switch (mode) {
	case LEDS_NORMAL:
		leds_val = bit;
		on = (prev & bit) ^ bit;
		off = prev & ~bit;
		break;
	case LEDS_ON:
		leds_val |= bit;
		on = prev ^ leds_val;
		break;
	case LEDS_OFF:
		leds_val &= ~bit;
		off = prev ^ leds_val;
		break;
	case LEDS_TOGGLE:
		leds_val ^= bit;
		on = (prev & bit) ^ bit;
		off = prev & bit;
		break;
	}

	if (on) dev_leds_on(on);
	if (off) dev_leds_off(off);
}

