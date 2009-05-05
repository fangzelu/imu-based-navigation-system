#include <io.h>
#include <signal.h>
#include <sys/inttypes.h>
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "common_def.h"
#include "adc.h"
//#include "ymote2_io.h"


static uint8_t is_adc_busy = 0;

uint8_t		ADCChannel = 0;
uint16_t	ADCResult[8] = {0,};

interrupt(ADC12_VECTOR) adc_int(void)
{
	uint16_t data = ADC12MEM0;
	
	ADCResult[ADCChannel] = data;

	//TRACE("\t%4u\n\r", data);

	is_adc_busy = 0;
}

void adc_sensor_init()
{
	//adc port all enable	
	//BIT_CLEAR(P6DIR, P6_0 | P6_1 | P6_2 | P6_3 | P6_4 | P6_5 | P6_6 | P6_7);
	SET_PORT_IN_ADC0_PIN();
	SET_PORT_IN_ADC1_PIN();
	SET_PORT_IN_ADC2_PIN();
	SET_PORT_IN_ADC3_PIN();
	SET_PORT_IN_ADC4_PIN();
	SET_PORT_IN_ADC5_PIN();
	SET_PORT_IN_ADC6_PIN();
	SET_PORT_IN_ADC7_PIN();

	//BIT_SET(P6SEL, P6_0 | P6_1 | P6_2 | P6_3 | P6_4 | P6_5 | P6_6 | P6_7);
	SET_PORT_MOD_ADC0_PIN();
	SET_PORT_MOD_ADC1_PIN();
	SET_PORT_MOD_ADC2_PIN();
	SET_PORT_MOD_ADC3_PIN();
	SET_PORT_MOD_ADC4_PIN();
	SET_PORT_MOD_ADC5_PIN();
	SET_PORT_MOD_ADC6_PIN();
	SET_PORT_MOD_ADC7_PIN();
}

uint8_t adc_get(uint8_t channel)
{
	CRITICAL_SECTION_START();
	is_adc_busy = 1;
	CRITICAL_SECTION_END();

	// sample-and-hole time : SHT0_6 (128 clock cycle)
	ADC12CTL0 = ADC12ON | REFON | REF2_5V | SHT0_6; 

	// sample-and-hole time : SHT0_4 (64 clock cycle)
	//ADC12CTL0 = ADC12ON | REFON | REF2_5V | SHT0_4;//REF2_5V | REFON;

	ADC12MCTL0 = channel + SREF_0;	// select channel Ach, Vref+
	ADC12IE = 0x01;					// Enable ADC12IFG.0
	ADC12CTL1 = SHP | ADC12SSEL_3;	// set ADC12 clk source to SMCLK (1Mhz)
	
	// start the conversion
	ADC12CTL0 |= ENC | ADC12SC;		


	ADCChannel = channel;
	return 0;
}


