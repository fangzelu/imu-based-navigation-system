/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2006
 * Nanotron Technologies
 *
 * Author: S. Radtke
 *
 * Description :
 *    This file contains the definitions for the iq values
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef NTRXIQPAR_H
#define NTRXIQPAR_H

#include    "config.h"
#include    "ntrxtypes.h"

void NTRXSetAgcValues (MyByte8T bandwidth, MyByte8T symbolDur, MyByte8T symbolRate);
void NTRXSetTxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetRxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetCorrThreshold (MyByte8T bandwidth, MyByte8T symbolDur);



#endif
