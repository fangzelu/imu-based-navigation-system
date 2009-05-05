#ifndef _UTIL_H_
#define _UTIL_H_

#include <io.h>

// interrupt enable / disable 
#define EINT()  asm volatile ("eint")
#define DINT()  asm volatile ("dint\n\tnop")

// Critical Section Macros
#define CRITICAL_SECTION_START() \
{ \
	uint16_t __critical_section = __make_critical_section(); 

#define CRITICAL_SECTION_END() \
	__quit_critical_section(__critical_section); \
}

#define CRITICAL_SECTION_END_NO_BRACE() __quit_critical_section(__critical_section)

#define CS_START  CRITICAL_SECTION_START
#define CS_END    CRITICAL_SECTION_END

void error_handler(uint16_t led, uint16_t delay);
void disp_led(uint8_t val);
void halt();

// Critical section primitive
uint16_t __make_critical_section();
void __quit_critical_section(uint16_t __critical_section);

void SetUSART0_SPI_MODE();
void SetUSART0_UART_MODE();

#endif

