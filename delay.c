#include <io.h>
#include "delay.h"

inline void usdelay(uint16_t us)
{
	uint16_t i;
	if (us < 500) {
		for (i = 2; i < us; i++) {
			__asm volatile (
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t");
		}
	} else {
		for (i = 0; i < us; i++) {
			__asm volatile (
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t");
		}
	}
}

inline void msdelay(uint16_t ms)
{
	  while (ms-- != 0)
			usdelay(1000);
}

inline void hwdelay(uint32_t us)
{ 
	usdelay(us); 
}

