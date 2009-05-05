#ifndef	_LED_H_
#define	_LED_H_

#include <io.h>
#include "ymote2_io.h"
#include "common_def.h"


#define LEDS_MASK 0x1F // 5 leds

static inline void dev_leds_on(uint16_t bit)
{
	if(bit & (1 << 0)) BIT_CLEAR(P5OUT, LED1);
	if(bit & (1 << 1)) BIT_CLEAR(P5OUT, LED2);
	if(bit & (1 << 2)) BIT_CLEAR(P5OUT, LED3);
	if(bit & (1 << 3)) BIT_CLEAR(P1OUT, LED4);
	if(bit & (1 << 4)) BIT_CLEAR(P2OUT, LED5);

}

static inline void dev_leds_off(uint16_t bit)
{

	if(bit & (1 << 0)) BIT_SET(P5OUT, LED1);
	if(bit & (1 << 1)) BIT_SET(P5OUT, LED2);
	if(bit & (1 << 2)) BIT_SET(P5OUT, LED3);
	if(bit & (1 << 3)) BIT_SET(P1OUT, LED4);
	if(bit & (1 << 4)) BIT_SET(P2OUT, LED5);
}

static inline void dev_leds_init(void)
{
	//led1, led2, led3	
	BIT_SET(P5DIR, (LED1 | LED2 | LED3));
	BIT_CLEAR(P5SEL, (LED1 | LED2 | LED3));
	BIT_SET(P5OUT, (LED1 | LED2 | LED3));

	//led4
	BIT_SET(P1DIR, LED4);
	BIT_CLEAR(P1SEL, LED4);
	BIT_SET(P1OUT, LED4);

	//led5
	BIT_SET(P2DIR, LED5);
	BIT_CLEAR(P2SEL, LED5);
	BIT_SET(P2OUT, LED5);

}

void leds_set(uint16_t bit, uint16_t mode);

// leds modes
#define LEDS_NORMAL	0
#define LEDS_ON		1
#define LEDS_OFF	2
#define LEDS_TOGGLE	3

// leds lib.
static inline void led1_on() { leds_set(1 << 0, LEDS_ON); }
static inline void led2_on() { leds_set(1 << 1, LEDS_ON); }
static inline void led3_on() { leds_set(1 << 2, LEDS_ON); }
static inline void led4_on() { leds_set(1 << 3, LEDS_ON); }
static inline void led5_on() { leds_set(1 << 4, LEDS_ON); }


static inline void led1_off() { leds_set(1 << 0, LEDS_OFF); }
static inline void led2_off() { leds_set(1 << 1, LEDS_OFF); }
static inline void led3_off() { leds_set(1 << 2, LEDS_OFF); }
static inline void led4_off() { leds_set(1 << 3, LEDS_OFF); }
static inline void led5_off() { leds_set(1 << 4, LEDS_OFF); }

static inline void led1_toggle() { leds_set(1 << 0, LEDS_TOGGLE); }
static inline void led2_toggle() { leds_set(1 << 1, LEDS_TOGGLE); }
static inline void led3_toggle() { leds_set(1 << 2, LEDS_TOGGLE); }
static inline void led4_toggle() { leds_set(1 << 3, LEDS_TOGGLE); }
static inline void led5_toggle() { leds_set(1 << 4, LEDS_TOGGLE); }

#endif

