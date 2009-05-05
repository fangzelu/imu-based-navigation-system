#include <io.h>
#include <signal.h>

#include "usart.h"
#include "common_def.h"
#include "ymote2_io.h"
#include "delay.h"
#include "imu.h"


/*interrupt(18) uart0_rx_interrupt()
{
}*/

void uart_init()
{
	msdelay(1);
	//P3.1,P3.3  I2C pin not select
	BIT_CLEAR(P3SEL, I2C_SDA + I2C_SCL);
	
	//UART0 Pin Setting
	P3SEL |= (1<<4) | (1<<5); //P3.4/UTXD0, P3.5/URXD0 Module Func 
	//msp430 uart0 init. 57600bps 8n1

	U0CTL = 0;//R-Configuring the USART Module for UART(uart <= i2c or spi)
//	U0CTL = SWRST;
	
	U0CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	ME1 = UTXE0 + URXE0; //UTXE0, URXE0 module enable
	U0TCTL = SSEL_SMCLK; //57600 bps / SMCLK (4 MHz)
	U0BR1 = 0x00;
	U0BR0 = 0x12;
	U0MCTL = 0x84;
	U0CTL &= ~SWRST; // Start USART

	//msp430 UART0 init. 9600 bps 8n1
	//U0CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	//ME1 = UTXE0 + URXE0; //UTXE0, URXE0 module enable
	//U0TCTL = SSEL_ACLK; //9600 bps / ACLK (32 kHz)
	//U0BR1 = 0x00;
	//U0BR0 = 0x03;
	//U0MCTL = 0x4A;
	//U0CTL &= ~SWRST; // Start USART

	BIT_CLEAR(IFG1, UTXIFG0);
	BIT_CLEAR(IFG1, URXIFG0);

	//IFG1 &= ~URXIFG0; IE1 |= URXIE0; //UART0RX interrupt ENABLE
	//IFG1 &= ~UTXIFG0;	IE1 |= UTXIE0; //UART0TX interrupt ENABLE
	
	//IFG1 &= ~URXIFG0; IE1 &= ~URXIE0; //UART0RX interrupt DISABLE
	//IFG1 &= ~UTXIFG0; IE1 &= ~UTXIE0; //UART0TX interrupt DISABLE

	///////////////////////////////////////////////////////////////////
	/*
	//UART1 Pin Setting
	P3SEL |= (1<<6) | (1<<7); //P3.6/UTXD1, P3.7/URXD1 Module Func 
	
	//msp430 uart1 init. 57600bps 8n1
	U1CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	ME2 = UTXE1 + URXE1; //UTXE1, URXE1 module enable
	U1TCTL = SSEL_SMCLK; //57600 bps / SMCLK (4 MHz)
	U1BR1 = 0x00;
	U1BR0 = 0x12;
	U1MCTL = 0x84;
	U1CTL &= ~SWRST; // Start USART

	//msp430 UART1 init. 9600 bps 8n1
	//U1CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	//ME2 = UTXE1 + URXE1;  //UTXE1, URXE1 module enable
	//U1TCTL = SSEL_ACLK; //9600 bps / ACLK (32 kHz)
	//U1BR1 = 0x00;
	//U1BR0 = 0x03;
	//U1MCTL = 0x4A;
	//U1CTL &= ~SWRST; // Start USART

	IFG2 &= ~URXIFG1; IE2 |= URXIE1; //UART1RX interrupt ENABLE
	//IFG2 &= ~UTXIFG1;	IE2 |= UTXIE1; //UART1TX interrupt ENABLE
	
	//IFG2 &= ~URXIFG1; IE2 &= ~URXIE1; //UART1RX interrupt DISABLE
	//IFG2 &= ~UTXIFG1; IE2 &= ~UTXIE1; //UART1TX interrupt DISABLE
	*/

}

void TxPrintf(char *Form, ...)
{
	char Buff[TRACE_MAX_BUFFER_SIZE];
	char *Str;
	
	va_list ArgPtr;
	va_start(ArgPtr,Form);
	vsprintf(Buff, Form, ArgPtr);
	va_end(ArgPtr);
	Str = Buff;

	CRITICAL_SECTION_START();

	while(*Str) 
    {
        if(*Str == '\n')
		{
            TXBUF0 = '\r';
			while (!(IFG1 & UTXIFG0));
        }
		
        TXBUF0 = *Str++;
		while (!(IFG1 & UTXIFG0));

    }
	CRITICAL_SECTION_END();
}

void TxBinary(void *target, uint8_t size)
{
	uint8_t i;
	uint8_t* val = (uint8_t*)target;

	CRITICAL_SECTION_START();

	for(i=0;i<size;++i)
	{
		TXBUF0 = *val++;
		while (!(IFG1 & UTXIFG0));
	}

	CRITICAL_SECTION_END();
}
