#ifndef	_USART_H_
#define	_USART_H_

#include <io.h>
#include <stdio.h>
#include "util.h"

void uart_init();
extern void TxPrintf(char *Form, ...);
extern void TxBinary(uint8_t* target, uint16_t size);

#ifdef TRACE_DEBUG

#define TRACE_MAX_BUFFER_SIZE 256

// Bus Switching... SPI0 -> UART0 -> SPI0
#define TRACE(__format, ...) \
	/*SetUSART0_UART_MODE();*/ \
	TRACE_FAST(__format, ##__VA_ARGS__); \
	/*SetUSART0_SPI_MODE()*/

#define TRACE_FAST(__format, ...) \
	do { \
		int __size, __idx; \
		char buf[TRACE_MAX_BUFFER_SIZE], *pBuf = buf; \
		__size = sprintf(buf, __format, ##__VA_ARGS__); \
		CRITICAL_SECTION_START(); \
		/* disable recv and transmit interrupts */ \
		/*IE1 &= ~(URXIE0); IE1 &= ~(UTXIE0);*/ \
		/* send buf through UART1 */ \
		for (__idx=0; __idx<__size; __idx++) { \
			/* Transmit data */ \
			TXBUF0 = pBuf[__idx]; \
			/* Wait if a byte is being transmitted */ \
			while (!(IFG1 & UTXIFG0)); \
		} \
		/* enable recv and transmit interrupts */ \
		/*IFG1 &= ~(URXIFG0); IE1 |= URXIE0;  IFG1 &= ~(UTXIFG0); IE1 |= UTXIE0;*/ \
		CRITICAL_SECTION_END(); \
	} while (0) 

#else //#ifdef TRACE_DEBUG	

#define TRACE(__format, ...) 

#endif

#endif

