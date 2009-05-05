/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2007
 * Nanotron Technologies
 *
 * Author: C. Bock
 *
 * Description :
 *    This file contains the definitions of the utility functions for the
 *    NTRX driver
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef NTRXRANGING_H
#define NTRXRANGING_H

#include "ntrxtypes.h"

/**
 * getDistance(..)
 *
 * If a one ranging cycle is done, getDistance returns the distance
 * in [m] to the destination which was chosen in RangingMode.
 *
 * Return: distance in [m] to RangingMode destination address
 */
double getDistance(void);

/**
 * RangingCallback_Rx(..):
 *
 * Received ranging packets must be given to this function.
 * The function collects the ranging answer of a RangingMode packet.
 *
 * MyByte8T *payload : payload of ranging packet
 * MyInt16T val : length of RangingPacket
 *
 * Returns: nothing
 */
void RangingCallback_Rx(MyByte8T *payload, MyByte8T len);

/**
 * RangingCallback_Ack(..):
 *
 * After each RangingPacket(RangingMode) has been sent, we need to collect
 * data. The function must be called after the ranging transmission.
 *
 * MyByte8T arqCount : how many retransmissions (successfully sent or not?)
 *
 * Returns: nothing
 */
void RangingCallback_Ack(MyByte8T arqCount);

/**
 * RangingMode(..):
 *
 * The function sends out a Ranging Packet to the given destination. 
 *
 * MyAddrT dest : 6 Byte Address; when dest is changed, all collected data will be 	
 * 					  deleted
 *
 * Returns: nothing
 */
void RangingMode(MyAddrT dest);

#endif /* NTRXRANGING_H */
