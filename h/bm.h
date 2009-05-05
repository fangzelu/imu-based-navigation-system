#ifndef __BM_H__
#define __BM_H__

#include <sys/inttypes.h>


enum DS2782_REG
{
	RSV = 0,
	STATUS,
	RAAC_M,
	RAAC_L,
	RSAC_M,
	RSAC_L,		// 5
	RARC,		
	RSRC,
	IAVG_M,
	IAVG_L,
	TEMP_M,		// 0A
	TEMP_L,		
	VOLT_M,
	VOLT_L,
	CURRENT_M,	// 0E
	CURRENT_L,
	ACR_M,		
	ACR_L,
	ACRL_M,
	ACRL_L,		// 13
	AS,
	SFR,		
	FULL_M,
	FULL_L,
	AE_M,		// 18
	AE_L,
	SE_M,
	SE_L,		// 1B
	EEPROM_REG 	 = 0x1F,
	EEPROM_CTL 	 = 0x60,
	EEPROM_S_ADD = 0x7E,
	EEPROM_FCR 	 = 0xFE,
};

enum DS2782_CMD
{
	COPY_B0 = 0x42,
	COPY_B1 = 0x44,
	RECALL_B0 = 0xB2,
	RECALL_B1 = 0xB4,
	LOCK_B0 = 0x63,
	LOCK_B1 = 0x66
};


#define I2C_DS2782_ADD		(unsigned char)(0x34)


extern uint8_t ReadDS2782StatusReg(void);
extern void WriteDS2782StatusReg(unsigned char Data);
extern int ReadDS2782VOLTReg(void);
extern int32_t ReadDS2782CURRENTReg(void);
extern uint16_t ReadDS2782TEMPReg(void);

#endif

