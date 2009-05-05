#include <io.h>
#include "hbm2x1m.h"
#include "delay.h"
#include <signal.h>

void HBM2X1M_reset()
{
	//P1OUT |= (1<<5);
	//msdelay(100);
	//P1OUT &= ~(1<<5);
	
	SET_PORT_HIGH_BTRST_PIN();
	msdelay(100);
	SET_PORT_LOW_BTRST_PIN();
}

void HBM2X1M_init()
{
	// BTRST (P1.5/TA0)
	//P1DIR |= (1<<5);
	//P1SEL &= ~(1<<5);
	SET_PORT_OUT_BTRST_PIN();
	SET_PORT_IO_BTRST_PIN();
		
	// P3.2 BT_On/Off
	//P3DIR |= (1<<2);
	//P3SEL &= ~(1<<2);
	//P3OUT |= (1<<2);
	SET_PORT_OUT_BTLDOCE_PIN();
	SET_PORT_IO_BTLDOCE_PIN();
	SET_PORT_HIGH_BTLDOCE_PIN();


	// BTRTS (P1.6/TA1)
	//P1DIR &= ~(1<<6);
	//P1SEL &= ~(1<<6);
	SET_PORT_IN_BTRTS_PIN();
	SET_PORT_IO_BTRTS_PIN();
	
	
	/*P1DIR |= (1<<6);
	P1SEL &= ~(1<<6);
	P1OUT &= ~(1<<6);
	//P1OUT |= (1<<6);*/

	// BTCTS (P1.7/TA2)
	//P1DIR |= (1<<7);
	//P1SEL &= ~(1<<7);
	//P1OUT &= ~(1<<7);
	SET_PORT_OUT_BTCTS_PIN();
	SET_PORT_IO_BTCTS_PIN();
	SET_PORT_LOW_BTCTS_PIN();
	
	//P1OUT |= (1<<7);
	_DINT();
	InitBTUart();
	_EINT();
	
	HBM2X1M_reset();

}

void InitBTUart(void)
{
	msdelay(1);

	//P3.1 P3.3 I2C pin not select
	P3SEL &= ~((1<<1) | (1<<3));
	
	P3SEL |= (1<<4) | (1<<5); //UART0 RX TX

	U0CTL = 0;//R-Configuring the USART Module for UART(uart <= i2c or spi)

	//msp430 UART0 init. 9600 bps 8n1
	/*U0CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	ME1 = UTXE0 + URXE0; //UTXE0, URXE0 module enable
	U0TCTL = SSEL_ACLK; //9600 bps / ACLK (32 kHz)
	U0BR1 = 0x00;
	U0BR0 = 0x03;
	U0MCTL = 0x4A;
	U0CTL &= ~SWRST; // Start USART*/


	//msp430 uart0 init. 57600bps 8n1
	UCTL0 = CHAR + SWRST; // 8-bit data, S/W Reset
	ME1 = UTXE0 + URXE0; //UTXE1, URXE1 module enable
	U0TCTL = SSEL0 | SSEL1; // SMCLK
	U0BR1 = 0x00;
	U0BR0 = 0x12; //57600 bps
	U0MCTL = 0x84;
	U0CTL &= ~SWRST; // Start USART
	
	IFG1 &= ~(UTXIFG0);
	IFG1 &= ~(URXIFG0);

	IFG1 &= ~URXIFG0; IE1 |= URXIE0; //UART0RX interrupt ENABLE
	//IFG1 &= ~UTXIFG0;	IE1 |= UTXIE0; //UART0TX interrupt ENABLE
	
	//IFG1 &= ~URXIFG0; IE1 &= ~URXIE0; //UART0RX interrupt DISABLE
	//IFG1 &= ~UTXIFG0; IE1 &= ~UTXIE0; //UART0TX interrupt DISABLE


}

void bt_send_cmd(const char *cmd_str)
{
	int i = 0;

	while( cmd_str[i] != '\0' )
	{
		TXBUF0 = cmd_str[i++]; 
		//TXBUF1 = cmd_str[i++];
		while (!(IFG1 & UTXIFG0));
	}
}









//EOF ===================
