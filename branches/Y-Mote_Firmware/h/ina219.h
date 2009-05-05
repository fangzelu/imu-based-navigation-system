#ifndef __INA219_H__
#define __INA219_H__

#include <sys/inttypes.h>


#define I2C_INA219_MSP_ADD		((char)(0x40))
#define I2C_INA219_RADIO_ADD	((char)(0x41))
#define I2C_INA219_SENSOR_ADD	((char)(0x44))

#define INA219_CAL(x)			(uint16_t)((float)40960 / (float)x + 0.5)

enum INA219_REG
{
	CONFIG_REG = 0,
	SHUNT_VOLT,
	BUS_VOLT,
	POWER,
	CURRENT,
	CALIBRATION
};


struct INA219_CONFIGURE_BIT
{
	uint16_t	 MODE:3;	// 0x00: Power Down, 0x01: S_V_T, 0x02: B_V_T, 0x03: S_B_V_T, 0x04: ADCoff, 0x05: S_V_C, 0x06: B_V_C, 0x07: S_B_V_C
	uint16_t	SADC:4;	// 0x00: 9bit, 0x01: 10bit, 0x02: 11bit, 0x03: 12bit ...
	uint16_t	BADC:4;	// 0x00: 9bit, 0x01: 10bit, 0x02: 11bit, 0x03: 12bit ...
	uint16_t	PG:2;	// 00: 40mV, 01: 80mV, 10: 160mV, 11: 320mV,
	uint16_t	BRANG:1;	// 0: 16V FSR, 1: 32V FSR
	uint16_t	NC:1;
	uint16_t	RST:1; 	// Same Power On reset
	
};

union INA219_CONFigure_REG
{
	struct		INA219_CONFIGURE_BIT bit;
	uint16_t	all;
};

extern union	INA219_CONFigure_REG INA219ConfigreReg;	


extern void		WriteINA219ConfigReg(int8_t SlaveAddress, uint16_t Data);
extern void		WriteINA219CalibrationReg(int8_t SlaveAddress, uint16_t Data);


extern uint16_t		ReadINA219ConfigReg(int8_t SlaveAddress);
extern int32_t		ReadINA219ShuntVoltReg(int8_t SlaveAddress);
extern uint16_t		ReadINA219BusVoltReg(int8_t SlaveAddress);
extern uint16_t		ReadINA219PowerReg(int8_t SlaveAddress);
extern uint16_t		ReadINA219CurrentReg(int8_t SlaveAddress);
extern uint16_t		ReadINA219CalibrationReg(int8_t SlaveAddress);

#endif
