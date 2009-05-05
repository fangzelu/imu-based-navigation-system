/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2002
 * Nanotron Technologies
 *
 * Author: B. Jozefini
 *
 * Description :
 *    This file contains data types and prototype functions for SPI-Bus 
 *    operation
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef  _NNSPI_H
#define  _NNSPI_H

#include "config.h"
#include "ntrxtypes.h"


typedef enum 
{
    READ_CMD = 0x00,                          /* SPI read command */
    WRITE_CMD = 0x80                          /* SPI write command */
} CMDT;

#define NANONETRESETDELAY   1000             /* reset of TRX in us */
//#define CONFIG_SPI_TRACE 1


void NanoReset	(void);
void InitSPI	(void);
void SetupSPI 	(void);
void NTRXReadSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len);
void NTRXWriteSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len);
//#define NTRXWrite1SPI(p,q) NTRXWriteSPI (p, q, 1)
//#define NTRXRead1SPI(p,q) NTRXReadSPI (p, q, 1)
// #define NTRXWrite1SPI(p,q)  { MyByte8T r = q; NTRXWriteSPI (p, &r, 1); }
// #define NTRXRead1SPI(p,q)   NTRXReadSPI (p, &(q), 1)
void NTRXReadSingleSPI(MyByte8T address, MyByte8T *buffer);
void NTRXWriteSingleSPI(MyByte8T address, MyByte8T buffer);

#ifdef CONFIG_SPI_TRACE
#define TRACE_DATA_LEN 2
#define TRACE_BUFFER_SIZE 200
typedef struct traceS
{
	unsigned char cmd;
	unsigned char len;
	unsigned char addr;
	unsigned char data[TRACE_DATA_LEN];
	int	id;
} traceT;

void printSpiTrace (int min, int max);
void traceSpiReset (void);
extern void traceSpi(CMDT command, MyByte8T address,
			  MyByte8T *buffer, MyByte8T len);

#endif /* CONFIG_SPI_TRACE */
#endif   /* _NNSPI_H */
