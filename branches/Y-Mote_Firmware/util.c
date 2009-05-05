#include <io.h>
#include "util.h"
#include "led.h"
#include "delay.h"

//error_handler:
//error_handler() print out the error number and stop the system.
//Returns: None
void	error_handler(uint16_t led, uint16_t delay)
{
#ifdef	CONFIG_CONSOLE
#ifdef	CONFIG_PRINTF
	//TRACE("ErrorHandler: error %d\n\r", led);
	//TRACE("System halted!\n");
#endif	//CONFIG_PRINTF 
#endif	//CONFIG_CONSOLE
	leds_set(0, LEDS_NORMAL);
	while(1)
	{
		leds_set(led, LEDS_TOGGLE);
		msdelay(delay);
	}
}

#define DELIMETER() \
	msdelay(500); \
	leds_set(0, LEDS_NORMAL); \
	msdelay(250); \
	leds_set(7, LEDS_NORMAL); \
	msdelay(25); \
	leds_set(0, LEDS_NORMAL); \
	msdelay(500);

void disp_led(uint8_t val)
{
	DELIMETER();
	leds_set(val, LEDS_NORMAL);
	DELIMETER();
	leds_set(val>>3, LEDS_NORMAL);
	DELIMETER();
	leds_set(val>>6, LEDS_NORMAL);
	DELIMETER();
}

void halt()
{
	while(1);
}

uint16_t __make_critical_section()
{
	uint16_t ret = (
			({
			 uint16_t __x;
			 __asm volatile ("mov r2, %0" : "=r"((uint16_t )__x));
			 __x;
			 }) 
			& 0x0008) != 0;
	DINT();
	return ret;
}

void __quit_critical_section(uint16_t __critical_section)
{
	if (__critical_section) EINT(); 
}

void SetUSART0_SPI_MODE()
{
	msdelay(1);

	//switch pins from UART to SPI
	P3DIR |= (1<<4) | (1<<5); //P3.4/UTXD0, P3.5/URXD0 DIR Output
	P3SEL &= ~((1<<4) | (1<<5)); //P3.4/UTXD0, P3.5/URXD0 I/O Func
	P3SEL |= (1<<1)|(1<<2)|(1<<3); //3pin SPI pins Module Func	
	
	//SPI0 Init. (for NanoLOC)
	UCTL0 = CHAR + SYNC + MM + SWRST; //8bit data, SPI function is selected, Master mode, logic hold
	ME1 = USPIE0; //Enable SPI0
	UTCTL0 = CKPL + SSEL1 + SSEL0 + STC; //UCLK Inative High, SMCLK, 3-pin SPI mode
	UBR00 = 0X02; //Baudrate: UCLK/2
	UBR10 = 0X00;
	UMCTL0 = 0; //No Modulation
	UCTL0 &= ~SWRST; //Init USART State Machine
}

void SetUSART0_UART_MODE()
{
	msdelay(1);

	//switch pins from SPI to UART
	P3DIR |= (1<<1)|(1<<2)|(1<<3); //SPI 3pins DIR Output
	P3SEL &= ~((1<<1)|(1<<2)|(1<<3)); //3pin SPI pins I/O Func
	P3SEL |= (1<<4) | (1<<5); //P3.4/UTXD0, P3.5/URXD0 Module Func 

	//UART0 Init. 57600bps 8n1
	UCTL0 = CHAR + SWRST; // 8-bit data, S/W Reset
	ME1 = UTXE0 + URXE0; //UTXE0, URXE0 module enable
	U0TCTL = SSEL0 | SSEL1; // SMCLK
	U0BR0 = 0x0012 & 0x0FF; //57600 bps
	U0BR1 = (0x0012 >> 8) & 0x0FF;
	U0MCTL = 0x84;
	U0CTL &= ~SWRST; // Start USART
}

