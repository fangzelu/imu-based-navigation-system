//====================================================================
//		
//				Battery Monitor(DS2782)
//====================================================================

#include "bm.h"
#include "i2c.h"
#include "common_def.h"
#include "ymote2_io.h"



#define PER_VOLT			(float)(4.88)
#define PER_TEMP			(float)(0.125)

#define SHUNT_RESISTOR		200//mohm


#if	SHUNT_RESISTOR == 1000


#define PER_IAVG			(float)(1.5625)
#define PER_CURRENT			(float)(1.5625)
#define PER_ACRL			(float)(1.5625)
#define PER_ACR				(float)(6.25)

#elif  SHUNT_RESISTOR == 200


#define PER_IAVG			(float)(7.8125)
#define PER_CURRENT			(float)(7.8125)
#define PER_ACRL			(float)(7.8125)
#define PER_ACR				(float)(31.25)


#endif


//******************************************************************************
//	func : ReadDS2782StatusReg
//	description : Read Status Register of DS2782
//******************************************************************************
uint8_t ReadDS2782StatusReg(void)
{
	uint8_t Data;
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, STATUS, 1, &Data);

	return Data;
}

//******************************************************************************
//	func : WriteDS2782StatusReg
//	description : write Status Register of DS2782
//******************************************************************************
void WriteDS2782StatusReg(uint8_t Data)
{
	WriteI2CDeviceREG(I2C_DS2782_ADD, STATUS, 1, &Data);
}

//******************************************************************************
//	func : ReadDS2782RAACReg
//	description : read RAAC(Remaining Active Absolute Capacity)  register of DS2782 
//******************************************************************************
uint16_t ReadDS2782RAACReg(void)
{
	uint8_t Buf[2];
		
	ReadI2CDeviceREG(I2C_DS2782_ADD, RAAC_M, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);

}

//******************************************************************************
//	func : ReadDS2782RSACReg
//	description : read RSAC(Remaining Standby Absolute Capacity)  register of DS2782 
//******************************************************************************
uint16_t ReadDS2782RSACReg(void)
{
	uint8_t Buf[2];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, RSAC_M, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}

//******************************************************************************
//	func : ReadDS2782RARCReg
//	description : read RARC(Remaining Active Relative Capacity)  register of DS2782 
//******************************************************************************
uint8_t ReadDS2782RARCReg(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_DS2782_ADD, RARC, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : ReadDS2782RSRCReg
//	description : read RSRC(Remaining Standby Relative Capacity)  register of DS2782 
//******************************************************************************
uint8_t ReadDS2782RSRCReg(void)
{
	uint8_t Buf[1];

	ReadI2CDeviceREG(I2C_DS2782_ADD, RSRC, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : ReadDS2782IAVGReg
//	description : read IAVG(Average Current)  register of DS2782 
//******************************************************************************
int32_t ReadDS2782IAVGReg(void)
{
	uint8_t Buf[2];
	int32_t Data;
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, IAVG_M, 2, Buf);

	Data = BYTES2INT(Buf[0], Buf[1]);


	if(Data > 0)
		Data = (int32_t)((float)Data * PER_IAVG + (float)0.5);
	else
		Data = (int32_t)((float)Data * PER_IAVG - (float)0.5);
	
	return Data;

}

//******************************************************************************
//	func : ReadDS2782TEMPReg
//	description : read TEMP(Temperature)  register of DS2782 
//******************************************************************************
uint16_t ReadDS2782TEMPReg(void)
{
	uint8_t		Buf[2];
	uint16_t	Data;

	ReadI2CDeviceREG(I2C_DS2782_ADD, TEMP_M, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]) >> 5;// (MSB + LSB) >> 5

	Data = (uint16_t)((float)Data * PER_TEMP + (float)0.5);

	return Data;

}

//******************************************************************************
//	func : ReadDS2782VOLTReg
//	description : read VOLT(voltage) register of DS2782
//******************************************************************************
int16_t ReadDS2782VOLTReg(void)
{
	uint8_t Buf[2];
	int16_t Data;
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, VOLT_M, 2, Buf);
	
	Data = BYTES2INT(Buf[0], Buf[1]) >> 5;// (MSB + LSB) >> 5 
	Data = (int)((float)Data * PER_VOLT + (float)0.5);

	return Data;
}

//******************************************************************************
//	func : ReadDS2782CURRENTReg
//	description : read CURRENT(current) register of DS2782
//******************************************************************************
int32_t ReadDS2782CURRENTReg(void)
{
	uint8_t Buf[2];
	int32_t Data;
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, CURRENT_M, 2, Buf);

	Data = (int32_t)BYTES2INT(Buf[0], Buf[1]);

	if(Data > 0)
		Data = (int32_t)((float)Data * PER_CURRENT + (float)0.5);
	else
		Data = (int32_t)((float)Data * PER_CURRENT - (float)0.5);
	
	return Data;
}

//******************************************************************************
//	func : ReadDS2782ACRReg
//	description : read ACR(Accumulated Current) register of DS2782
//******************************************************************************
uint32_t ReadDS2782ACRReg(void)
{
	uint8_t Buf[2];
	uint32_t Data;


	ReadI2CDeviceREG(I2C_DS2782_ADD, ACR_M, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]);

	Data = (uint32_t)((float)Data * PER_ACR + (float)0.5);
	
	return Data;

}

//******************************************************************************
//	func : WriteDS2782ACRReg
//	description : write ACR(Accumulated Current) register of DS2782
//******************************************************************************
void WriteDS2782ACRReg(uint16_t Data)
{
	uint8_t Buf[2];

	Buf[0] = (uint8_t)((Data >> 8) & 0xFF);
	Buf[1] = (uint8_t)(Data & 0xFF);
	
	WriteI2CDeviceREG(I2C_DS2782_ADD, ACR_M, 2, Buf);
	
}

//******************************************************************************
//	func : ReadDS2782ACRLReg
//	description : read ACRL(Low Accumulated Current) register of DS2782
//******************************************************************************
uint16_t ReadDS2782ACRLReg(void)
{
	uint8_t Buf[2];
	uint16_t Data;
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, ACRL_M, 2, Buf);

	Data = BYTES2UINT(Buf[0], Buf[1]) >> 4;// (MSB + LSB) >> 4

	Data = (uint16_t)((float)Data * PER_ACRL+ (float)0.5);

	return Data;
}

//******************************************************************************
//	func : ReadDS2782ASReg
//	description : read AS(Age Scalar) register of DS2782
//******************************************************************************
uint8_t ReadDS2782ASReg(void)
{
	uint8_t Buf[1];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, AS, 1, Buf);

	return Buf[0];
	
}

//******************************************************************************
//	func : WriteDS2782ASReg
//	description : write AS(Age Scalar) register of DS2782
//******************************************************************************
void WriteDS2782ASReg(uint8_t Data)
{	
	WriteI2CDeviceREG(I2C_DS2782_ADD, AS, 1, &Data);
}

//******************************************************************************
//	func : ReadDS2782SFRReg
//	description : read SFR(Special Feature) register of DS2782
//******************************************************************************
uint8_t ReadDS2782SFRReg(void)
{
	uint8_t Buf[1];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, SFR, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : WriteDS2782SFRReg
//	description : write SFR(Special Feature) register of DS2782
//******************************************************************************
void WriteDS2782SFRReg(uint8_t Data)
{
	WriteI2CDeviceREG(I2C_DS2782_ADD, SFR, 1, &Data);
}

//******************************************************************************
//	func : ReadDS2782FULLReg
//	description : read FULL(Full Capacity) register of DS2782
//******************************************************************************
uint16_t ReadDS2782FULLReg(void)
{
	uint8_t Buf[2];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, FULL_M, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}

//******************************************************************************
//	func : ReadDS2782AEReg
//	description : read AE(Active Empty) register of DS2782
//******************************************************************************
uint16_t ReadDS2782AEReg(void)
{
	uint8_t Buf[2];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, AE_M, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}

//******************************************************************************
//	func : ReadDS2782SEReg
//	description : read SE(Standby Empty) register of DS2782
//******************************************************************************
uint16_t ReadDS2782SEReg(void)
{
	uint8_t Buf[2];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, SE_M, 2, Buf);

	return BYTES2UINT(Buf[0], Buf[1]);
}

//******************************************************************************
//	func : ReadDS2782EEPROMReg
//	description : read EEPROM register of DS2782
//******************************************************************************
uint8_t ReadDS2782EEPROMREG(void)
{
	uint8_t Buf[1];
	
	ReadI2CDeviceREG(I2C_DS2782_ADD, EEPROM_REG, 1, Buf);

	return Buf[0];
}

//******************************************************************************
//	func : WriteDS2782EEPROMReg
//	description : write EEPROM register of DS2782
//******************************************************************************
void WriteDS2782EEPROMREG(uint8_t Data)
{	
	WriteI2CDeviceREG(I2C_DS2782_ADD, EEPROM_REG, 1, &Data);
}


//******************************************************************************
//	func : FunctionCMDDS2782
//	description : Function CMD Send
//******************************************************************************
void FunctionCMDDS2782(enum DS2782_CMD Command)
{
	WriteI2CDeviceREG(I2C_DS2782_ADD, EEPROM_FCR, 1, (uint8_t *)&Command);
}


/*
void ReadDS2782EEPROMBlock(void)
{
	unsigned char i = 0;
	for(i = 0; i < 0x20; i++)
	{
		ReadI2CDeviceREG(SlaveAddress, EEPROM_CTL+i);
		DS2782EEPROMBlock[i] = I2CBuffer[0];
		
		TxPrintf("DS2782EEPROMBLOCK %x : %x\r\n", i + 0x60, DS2782EEPROMBlock[i]);
	}
}

void WriteDS2782EEPROMBlock(void)
{
	byte Buff[1];
	uint8_t i;

	for(i = 0; i < 0x20; i++)
	{
		Buff[0] = DS2782EEPROMBlock[i];

		WriteI2CDeviceREG(SlaveAddress, EEPROM_CTL+i, 1, Buff);
	}

	
}

*/
