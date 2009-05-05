#ifndef __COMPASS_H__
#define __COMPASS_H__

#include <sys/inttypes.h>



enum HMC5843
{
	CREGA = 0,		//Configuration Register A
	CREGB,			//Configuration Register B
	MODEREG,		//Mode Register
	X_AXIS_MSBREG,	//Data output X MSB Register
	X_AXIS_LSBREG,	//Data output X LSB Register
	Y_AXIS_MSBREG,	//Data output Y MSB Register
	Y_AXIS_LSBREG,	//Data output Y LSB Register
	Z_AXIS_MSBREG,	//Data output Z MSB Register
	Z_AXIS_LSBREG,	//Data output Z LSB Register
	STATUSREG,		//Status Register
	IREGA,			//Identification Register
	IREGB,			//Identification Register
	IREGC			//Identification Register
};

struct HMC5843_CREGA_BITS
{
	uint8_t MS:2;
	uint8_t DO:3;
	uint8_t NC:3;
};

union HMC5843_CREGA_REG
{
	struct HMC5843_CREGA_BITS bit;
	uint8_t	all;
};

struct HMC5843_CREGB_BITS
{
	uint8_t DL:4;
	uint8_t NC:1;
	uint8_t GL:3;
};

union HMC5843_CREGB_REG
{
	struct HMC5843_CREGB_BITS bit;
	uint8_t all;
};


struct HMC5843_MODEREG_BITS
{
	uint8_t MD:2;
	uint8_t NC:6;
};

union HMC5843_MODEREG_REG
{
	struct HMC5843_MODEREG_BITS bit;
	uint8_t all;
};


extern union HMC5843_CREGA_REG		CompassCRAReg;
extern union HMC5843_CREGB_REG		CompassCRBReg;
extern union HMC5843_MODEREG_REG	CompassMRReg;

extern uint8_t		ReadCompassIREGA(void);
extern uint8_t		ReadCompassIREGB(void);
extern uint8_t		ReadCompassIREGC(void);
extern uint8_t		ReadCompassStatusReg(void);
extern uint16_t		ReadCompassXAxis(void);
extern uint16_t		ReadCompassYAxis(void);
extern uint16_t		ReadCompassZAxis(void);
extern uint8_t		ReadCompassCREGA(void);
extern uint8_t		ReadCompassCREGB(void);
extern uint8_t		ReadCompassMODEREG(void);

extern void			WriteCompassCREGA(uint8_t Buf);
extern void			WriteCompassCREGB(uint8_t Buf);
extern void			WriteCompassMODEREG(uint8_t Buf);

#endif


//EOF ============
