//*************************************************************************************
//
//				INA219.C
//				by hhy
//
//
//*************************************************************************************

#include <sys/inttypes.h>
#include "common_def.h"
#include "ymote2_io.h"
#include "i2c.h"
#include "usart.h"
#include "ina219.h"



union INA219_CONFigure_REG INA219ConfigreReg;



//******************************************************************************
//	func : WriteINA219ConfigReg
//	description : Write Configuration Register of INA219
//******************************************************************************
void WriteINA219ConfigReg(int8_t SlaveAddress, uint16_t Data)
{
	uint8_t Buf[2];

	Buf[0] = (uint8_t)((Data >> 8) & 0xFF);
	Buf[1] = (uint8_t)(Data & 0xFF);
	
	WriteI2CDeviceREG(SlaveAddress, CONFIG_REG, 2, Buf);
}

//******************************************************************************
//	func : WriteINA219CalibrationReg
//	description : Write Calibration Register of INA219
//******************************************************************************
void WriteINA219CalibrationReg(int8_t SlaveAddress, uint16_t Data)
{
	uint8_t Buf[2];

	Buf[0] = (uint8_t)((Data >> 8) & 0xFF);
	Buf[1] = (uint8_t)(Data & 0xFF);
	
	WriteI2CDeviceREG(SlaveAddress, CALIBRATION, 2, Buf);
}


//******************************************************************************
//	func : ReadINA219ConfigReg
//	description : Read Configuration Register of INA219
//******************************************************************************
uint16_t ReadINA219ConfigReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];

	ReadI2CDeviceREG(SlaveAddress, CONFIG_REG, 2, Buf);


	return BYTES2UINT(Buf[0], Buf[1]);
}

//******************************************************************************
//	func : ReadINA219ShuntVoltReg
//	description : Read Shunt voltage Register of INA219
//	return : xxx uV
//  how about offset error!!
//******************************************************************************
int32_t ReadINA219ShuntVoltReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];
	int32_t Data;

	ReadI2CDeviceREG(SlaveAddress, SHUNT_VOLT, 2, Buf);

	Data = (int32_t)BYTES2INT(Buf[0], Buf[1]) * 10;//  ... uV  //LSB = 10uV, result * 10
	
	return Data;
	
}

//******************************************************************************
//	func : ReadINA219BusVoltReg
//	description : Read Bus voltage Register of INA219
//******************************************************************************
uint16_t ReadINA219BusVoltReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];
	uint16_t  Data;

	ReadI2CDeviceREG(SlaveAddress, BUS_VOLT, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]) >> 1;//  >> 3 * 4//  mV, LSB = 4mV

	return Data;
}

//******************************************************************************
//	func : ReadINA219PowerReg
//	description : Read Bus Power  Register of INA219
//******************************************************************************
uint16_t ReadINA219PowerReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];

	ReadI2CDeviceREG(SlaveAddress, POWER, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}


//******************************************************************************
//	func : ReadINA219CurrentReg
//	description : Read Bus Current Register of INA219
//******************************************************************************
uint16_t ReadINA219CurrentReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];

	ReadI2CDeviceREG(SlaveAddress, CURRENT, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}


//******************************************************************************
//	func : ReadINA219CalibrationReg
//	description : Read Calibration Register of INA219
//******************************************************************************
uint16_t ReadINA219CalibrationReg(int8_t SlaveAddress)
{
	uint8_t Buf[2];

	ReadI2CDeviceREG(SlaveAddress, CALIBRATION, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}


//EOF
