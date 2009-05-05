#ifndef		__ADC_H__
#define		__ADC_H__

#include "pinmacros.h"

#define	GYRO_PTATS			ADC0_CH
#define ACC_X_CHANNEL		ADC5_CH
#define ACC_Y_CHANNEL		ADC4_CH 
#define ACC_Z_CHANNEL		ADC3_CH	
#define GYRO1_X_CHANNEL		ADC1_CH
#define GYRO1_Y_CHANNEL		ADC2_CH
#define GYRO2_X_CHANNEL		ADC6_CH
#define GYRO2_Y_CHANNEL		ADC7_CH

#define ADC0_CH			0
#define ADC1_CH			1
#define ADC2_CH			2
#define ADC3_CH			3
#define ADC4_CH			4
#define ADC5_CH			5
#define ADC6_CH			6
#define ADC7_CH			7

#define	ADC_DELAY		1

#define WAIT_ADC()		msdelay(ADC_DELAY)


MAKE_PORT_MACRO(ADC0, 6, 0);
MAKE_PORT_MACRO(ADC1, 6, 0);
MAKE_PORT_MACRO(ADC2, 6, 0);
MAKE_PORT_MACRO(ADC3, 6, 0);
MAKE_PORT_MACRO(ADC4, 6, 0);
MAKE_PORT_MACRO(ADC5, 6, 0);
MAKE_PORT_MACRO(ADC6, 6, 0);
MAKE_PORT_MACRO(ADC7, 6, 0);






extern uint8_t ADCChannel;
extern uint16_t ADCResult[8];

extern void adc_sensor_init();
extern uint8_t adc_get(uint8_t channel);


#endif
