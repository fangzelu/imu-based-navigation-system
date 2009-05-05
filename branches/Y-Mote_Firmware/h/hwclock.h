/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2002
 * Nanotron Technologies
 *
 * Author: S. Rohdemann
 *
 * Description :
 *    This file contains the type- / data- and function definitions
 *    for the hardware timer driver
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef	_HWCLOCK_H
#define	_HWCLOCK_H

#include "config.h"
#include "ntrxtypes.h"
#include "util.h"


#define NKEYS 4

extern	void		hwclock_init(void);
extern  void 		hwclock_stop (void);
extern  void 		StartTimer2 (void);
extern	MyDword32T	hwclock(void);
extern	void		hwdelay(MyDword32T t);
void	hwclockRestart(MyDword32T start);
void 	SetWdtCounter (MyDword32T value);

//#define ENTER_TASK {unsigned char cSREG=SREG; cli();
//#define LEAVE_TASK  SREG=cSREG; sei();}

#define ENTER_TASK { uint16_t cSR; \
	__asm volatile ("mov r2, %0" : "=r"((uint16_t)cSR)); \
	CRITICAL_SECTION_START();

#define LEAVE_TASK __asm volatile ("mov %0, r2" : "=r"((uint16_t)cSR)); \
	CRITICAL_SECTION_END(); }


#ifdef CONFIG_IMPULSE_COUNTER
MyDword32T GetImpuleCounter (MyByte8T i);
MyDword32T GetAndResetImpuleCounter (MyByte8T i);
#endif


#ifdef CONFIG_OPTO_COUNTER
MyDword32T GetOptoCounter (void);
MyDword32T GetAndResetOptoCounter (void);
#endif
#endif	/* _HWCLOCK_H */
