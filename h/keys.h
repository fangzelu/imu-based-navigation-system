/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2004
 * Nanotron Technologies
 *
 * Author: S. Rohdemann
 *
 * Description :
 *    This file contains the macros for nanoNET mfc board key handling.
 *
 ****************************************************************************/

/*
 * $Log$
 */
#ifndef	_KEYS_H
#define	_KEYS_H
#include "config.h"
#include "ntrxtypes.h"


// #define	NKEYS	4

#ifdef	CONFIG_PORTATION_ATMEGA128_BOARD
extern	MyBoolT	key_flags[];

#define	key_k1()	key_flags[0]
#define	key_k2()	key_flags[1]
#define	key_k3()	key_flags[2]
#endif	/* CONFIG_PORTATION_ATMEGA128_BOARD */

#endif	/* _KEYS_H */
