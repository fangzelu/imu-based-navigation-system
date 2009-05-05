#ifndef __I2C_H__
#define __I2C_H__

#include "pinmacros.h"

#define I2C_OWN_ADD		((char)(0x49))




MAKE_PORT_MACRO(I2C_SDA,	3, 1);
MAKE_PORT_MACRO(I2C_SCL,	3, 3);
MAKE_PORT_MACRO(UTX0,		3, 4);
MAKE_PORT_MACRO(URX0,		3, 5);





extern void I2CSPIMuxSelect(unsigned char type);
extern void WriteI2CDeviceREG(char SlaveAddress, char SlaveRegAddress, int WriteCnt, unsigned char * pWriteBuf);
extern char ReadI2CDeviceREG(char SlaveAddress, char SlaveRegAddress, int ReadCnt, unsigned char *pReadBuf); 
extern void InitI2CModule(void);
extern void InitI2CTx(void);
extern void InitI2CRx(void);


#endif
