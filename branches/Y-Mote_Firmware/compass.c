#include <io.h>
#include <signal.h>
#include <stdio.h>
#include <sys/inttypes.h>

#include "common_def.h"
#include "ymote2_io.h"
#include "led.h"
#include "delay.h"
#include "i2c.h"
#include "compass.h"

#define I2C_COMPASS_ADD		((char)(0x1E))


union HMC5843_CREGA_REG		CompassCRAReg;
union HMC5843_CREGB_REG		CompassCRBReg;
union HMC5843_MODEREG_REG	CompassMRReg;

//******************************************************************************
//	func : ReadCompassStatusReg
//	description : Read Status Register of HMC5843(Digital Compass)
//******************************************************************************
uint8_t ReadCompassStatusReg(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, STATUSREG, 1, Buf);

	return Buf[0];
	
}


//******************************************************************************
//	func : ReadCompassIREGA
//	description : Read Identification Register A of HMC5843(Digital Compass)
//******************************************************************************
uint8_t ReadCompassIREGA(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, IREGA, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : ReadCompassIREGB
//	description : Read Identification Register B of HMC5843(Digital Compass)
//******************************************************************************
uint8_t ReadCompassIREGB(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, IREGB, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : ReadCompassIREGC
//	description : Read Identification Register C of HMC5843(Digital Compass)
//******************************************************************************
uint8_t ReadCompassIREGC(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, IREGC, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : ReadCompassXAxis
//	description : Read Data output X Axis of HMC5843(Digital Compass)
//******************************************************************************
uint16_t ReadCompassXAxis(void)
{
	uint8_t Buf[2];
	uint16_t Data;
	
	ReadI2CDeviceREG(I2C_COMPASS_ADD, X_AXIS_MSBREG, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]);

	return Data;
}

//******************************************************************************
//	func : ReadCompassYAxis
//	description : Read Data output Y Axis of HMC5843(Digital Compass)
//******************************************************************************
uint16_t ReadCompassYAxis(void)
{
	uint8_t Buf[2];
	uint16_t Data;
	
	ReadI2CDeviceREG(I2C_COMPASS_ADD, Y_AXIS_MSBREG, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]);

	return Data;
}

//******************************************************************************
//	func : ReadCompassZAxis
//	description : Read Data output Z Axis of HMC5843(Digital Compass)
//******************************************************************************
uint16_t ReadCompassZAxis(void)
{
	uint8_t Buf[2];
	uint16_t Data;
	
	ReadI2CDeviceREG(I2C_COMPASS_ADD, Z_AXIS_MSBREG, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]);

	return Data;
}


void WriteCompassCREGA(uint8_t Buf)
{
	WriteI2CDeviceREG(I2C_COMPASS_ADD, CREGA, 1, &Buf);
}

uint8_t ReadCompassCREGA(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, CREGA, 1, Buf);

	return Buf[0];
}

void WriteCompassCREGB(uint8_t Buf)
{
	WriteI2CDeviceREG(I2C_COMPASS_ADD, CREGB, 1, &Buf);
}

uint8_t ReadCompassCREGB(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, CREGB, 1, Buf);

	return Buf[0];
}

void WriteCompassMODEREG(uint8_t Buf)
{
	WriteI2CDeviceREG(I2C_COMPASS_ADD, MODEREG, 1, &Buf);
}

uint8_t ReadCompassMODEREG(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_COMPASS_ADD, MODEREG, 1, Buf);

	return Buf[0];
}



















//EOF =========
