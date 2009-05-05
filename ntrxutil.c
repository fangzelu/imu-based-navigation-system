/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2006, 2007
 * Nanotron Technologies
 *
 * Author: S. Radtke
 *
 * Description :
 *    This file contains the source code for the implementation of the
 *    NTRX helper functions
 *
 ****************************************************************************/

/*
 * $Log$
 */

#include    <stdlib.h>
#include    <string.h>
#include    "config.h"
#include    "ntrxtypes.h"
#ifdef	CONFIG_CONSOLE
#include 	"usart.h"
#endif
#include    "ntrxutil.h"

#include    "hwclock.h"
#include    "nnspi.h"
#include    "ntrxdil.h"
#include    "ntrxiqpar.h"

//#include 	<avr/interrupt.h>
#include "led.h"
#include "util.h"

#define NTRX_DUMMY_REG  0xff


extern MyBoolT NTRXRestart (void);

static const MyByte8T txLoAdjValues[2] = {0x0b, 0x09}; /* start value */
static const MyByte8T rxLoAdjValues[2] = {0x48, 0x0f}; /* start value for Lo Adjust */
static const MyByte8T logCh[6][2] ={{0x68, 0x5a}, /* Channel 0: 2.44175 GHz, center of the band, 
																										 shall be used for 80 MHz mode */
	{0x62, 0x67}, /* Channel 1: 2.412 GHz, Center frequency of
									 nonoverlapping channel no. 1 (Europe or USA) */
	{0x68, 0x67}, /* Channel 2: 2.442 GHz, Center frequency of
									 nonoverlapping channel no. 2 (Europe) */
	{0x6e, 0x67}, /* Channel 3: 2.472 GHz, Center frequency of
									 nonoverlapping channel no. 3 (Europe) */
	{0x67, 0x67}, /* Channel 4: 2.437 GHz, Center frequency of
									 nonoverlapping channel no. 2 (USA) */
	{0x6c, 0x67}};  /* Channel 5: 2.462 GHz, Center frequency of
										 nonoverlapping channel no. 3 (USA) */


MyBoolT bRxCrc2Mode=FALSE;
MyByte8T ntrxShadowReg[SHADOWREGCOUNT];

SettingsT settingVal;
static MyByte8T firstTime = 0;
MyBoolT  ntrxRun;
MyByte8T ntrxIrqStatus[2];
MyAddressT ntrxMacAddr;
MyByte8T   ntrxState;
MyByte8T   ntrxCal;
MyByte8T   curCh = 0;

//#define memcpy PPSLibMemCopy

MyByte8T ntrxInitShadowReg[] PROGMEM =
{
	/* 0x00 */   0,   0,   0,   0,   0,   0,   0,   6,
	/* 0x08 */   0,   0,   0,   0,   0,   0,   0,   0,
	/* 0x10 */   0,   0,   0,   0,   0,   0,  64,   0,
	/* 0x18 */  32,  64,   0,  32,   0,   0,   0,   3,
	/* 0x20 */   6, 152,  12,   0,   2,  63,  30,   6,
	/* 0x28 */   0,   0, 171,  44, 213, 146, 148, 202,
	/* 0x30 */ 105, 171,  48,   0,   0,   0,   0,   0,
	/* 0x38 */   0,   0,   0,   0, 224,   4,   0,   1,
	/* 0x40 */   3,   7,   0,   3,  63,  63,  15,  15,
	/* 0x48 */ 115,   0,  16,  16,  67,  20,  16,   0,
	/* 0x50 */   0, 127,   0,   0,   0,   0,   0,   0,
	/* 0x58 */   0,   0,  11,  95,   5,   7, 213,  98,
	/* 0x60 */   0,   0,   0,  12,  10,   0,   0,   0,
	/* 0x68 */   0,   0,   0,   0,   0,   0,   0,   0,
	/* 0x70 */   0,   0,   0,   0,   0,   0,   0,   0,
	/* 0x78 */   0,   0,   0,   0,   0,  80,   0,   0
};

/**
 * NTRXInitShadowRegister:
 *
 * NTRXInitShadowRegister() initializes the shadow registers for the
 * nanoNET TRX chip
 *
 * Returns: none
 */
void NTRXInitShadowRegister (void)
{
	MyByte8T i;

	/*
	 * set shadow registers to the initial value of the nanoNET TRX chip
	 */
	for (i = 0; i < SHADOWREGCOUNT; i++)
	{
		ntrxShadowReg[i] = ReadRomByte(ntrxInitShadowReg[i]);
	}
}

/*
 * semaphore for the receiver start function
 */
static MyBoolT rxStart = FALSE;

/**
 * SetRxStop:
 *
 * SetRxStop() sets the semaphore to ensure that the receiver is only started once.
 *
 * Returns: none
 *
 */
void SetRxStop (void)
{
	rxStart = FALSE;
}


/**
 * SetRxStart:
 *
 * SetRxStart() sets the semaphore to ensure that the receiver is only started once.
 *
 * Returns: none
 *
 */
void SetRxStart (void)
{
	rxStart = TRUE;
}

/**
 * NTRXSetChannel:
 *
 * @value: -input- channel index
 *
 * NTRXSetChannel() sets the channel for transmission and reception.
 *
 * Returns: none
 *
 */
void NTRXSetChannel (MyByte8T value)
{
	if (value < 6)
	{
		curCh = value;
	}

	NTRXAllCalibration ();
}

/**
 * NTRXGetChannel:
 *
 * NTRXGetChannel() returns the channel index selected for transmission and reception.
 *
 * Returns: channel index
 *
 */
MyByte8T NTRXGetChannel (void)
{
	return (curCh);
}


/**
 * NTRXSetIndexReg:
 *
 * @page: -input- address index
 *
 * NTRXSetIndexReg() sets the index registers for the nanoNET TRX chip
 *
 * Returns: none
 */
void NTRXSetIndexReg (MyByte8T page)
{
	if (page != ntrxShadowReg[NA_RamIndex_O])
	{
		ntrxShadowReg[NA_RamIndex_O] = page;
		NTRXWriteSingleSPI (NA_RamIndex_O, page);
	}
}

/**
 * NTRXSetRamIndex:
 *
 * @page: -input- address index
 *
 * NTRXSetRamIndex() sets the index registers for the nanoNET TRX chip
 *
 * Returns: none
 */
void NTRXSetRamIndex (MyByte8T page)
{
	page &= 0x03;
	if (page != (ntrxShadowReg[NA_RamIndex_O] & 0x03))
	{
		ntrxShadowReg[NA_RamIndex_O] &= 0xf0;
		ntrxShadowReg[NA_RamIndex_O] |= page;
		page = ntrxShadowReg[NA_RamIndex_O];
		NTRXWriteSingleSPI (NA_RamIndex_O, page);
	}
}

/**
 * NTRXSetStaAddress:
 *
 * @address: -input- 6 byte address field containing the MAC address
 *
 * NTRXSetStaAddress() sets the MAC address in the chip. This address is used
 * for address matching when this feature is enabled.
 *
 * Returns: none
 *
 */
void NTRXSetStaAddress (MyByte8T *address)
{
	NTRXSetNRegister (NA_RamStaAddr0, address);
}

/**
 * NTRXRxLoCalibration:
 *
 * NTRXRxLoCalibration() callibrates the local oscillator
 *
 * Returns: none
 *
 */
void NTRXRxLoCalibration (void)
{
	MyByte8T data[3];


	NTRXSetRegister (NA_EnableLO, TRUE);
	NTRXSetRegister (NA_EnableLOdiv10, TRUE);
	NTRXSetRegister (NA_UseLoRxCaps, TRUE);

	data[0] = 0x03;
	NTRXWriteSingleSPI (NA_LoIntsReset_O, data[0]);
	data[0] = logCh[curCh][1]; 
	data[1] = logCh[curCh][0]; 
	NTRXSetNRegister (NA_LoTargetValue, data);

	do
	{
		NTRXReadSingleSPI (NA_LoIntsRawStat_O, &data[0]);
	} while ((data[0] & (1 << NA_LoTuningReady_B)) != (1 << NA_LoTuningReady_B));

	/* Read out caps values if needed */
	//    NTRXReadSPI (0x16, data, 3);
	NTRXSetRegister (NA_UseLoRxCaps, FALSE);
	NTRXSetRegister (NA_EnableLOdiv10, FALSE);
	NTRXSetRegister (NA_EnableLO, FALSE);
}

/**
 * NTRXTxLoCalibration:
 *
 * NTRXTxLoCalibration() callibrates the local oscillator
 *
 * Returns: none
 *
 */
void NTRXTxLoCalibration (void)
{
	MyByte8T data[3];

	NTRXSetRegister (NA_EnableLO, TRUE);
	NTRXSetRegister (NA_EnableLOdiv10, TRUE);
	NTRXSetRegister (NA_UseLoRxCaps, FALSE);

	data[0] = 0x03;
	NTRXWriteSingleSPI (NA_LoIntsReset_O, data[0]);

	data[0] = logCh[curCh][1]; 
	data[1] = logCh[curCh][0]; 
	NTRXSetNRegister (NA_LoTargetValue, data);

	do
	{
		NTRXReadSingleSPI (NA_LoIntsRawStat_O, &(data[0]));
	} while ((data[0] & (1 << NA_LoTuningReady_B)) != (1 << NA_LoTuningReady_B));

	/* Read out caps values if needed */
	//    NTRXReadSPI (0x16, data, 3);
	NTRXSetRegister (NA_EnableLOdiv10, FALSE);
	NTRXSetRegister (NA_EnableLO, FALSE);
}

/**
 * NTRXAllCalibration:
 *
 * NTRXAllCalibration() calibrates the local RF oscillators
 *
 * Returns: none
 *
 */
void NTRXAllCalibration (void)
{
	MyByte8T value;

	if (ntrxState != TxIDLE)
	{
		ntrxCal = AllCAL;
		return;
	}

	value = (1 << NA_LoEnableFastTuning_B) | (1 << NA_LoEnableLsbNeg_B) | (4 << NA_LoFastTuningLevel_LSB);
	NTRXWriteSingleSPI (NA_LoEnableFastTuning_O, value);
	ntrxShadowReg[NA_LoEnableFastTuning_O] = value;

	NTRXSetRegister (NA_RxCmdStop, TRUE);
	NTRXSetRegister (NA_RxIntsReset, 0xff);
	NTRXSetRegister (NA_RxBufferCmd, 3);
	rxIrqStatus = 0;

	NTRXRxLoCalibration ();
	NTRXTxLoCalibration ();

	NTRXSetRegister (NA_RxCmdStart, TRUE);
	ntrxCal = 0;
}

/*
 * Table of available commands that are supported by the general purpose register get and set functions
 * The table provides the address in low and high part, the mask marks the valid bits or bytes of a function.
 * The operation column tells the functions if it is a bit / boolean value function or a bit field function.
 * The fifth column marks read/write, read only or write only function.
 */
#define BOOL_OP         0xff

#define NTRX_RW_REG     1
#define NTRX_NC_REG     2
#define NTRX_RO_REG     3
#define NTRX_WO_REG     4

#define NTRX_TC_REG 0
/*const*/ NTRXRegCmdT regCmd[] PROGMEM =
{
	// Addr Hi ,                            Addr Lo,                                    Mask,                                           Operation,              ShadowReg,  Reg Index,  Init Value
	// 0x00
	/* NA_SpiBitOrder */
	{(NA_SpiBitOrder_O >> 8),               (NA_SpiBitOrder_O & 0xff),                  (0x01 << NA_SpiBitOrder_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_SpiBitOrder_I},
	/* NA_SpiTxDriver */
	{(NA_SpiTxDriver_O >> 8),               (NA_SpiTxDriver_O & 0xff),                  (0x01 << NA_SpiTxDriver_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_SpiTxDriver_I},
	/* NA_IrqPolarity */
	{(NA_IrqPolarity_O >> 8),               (NA_IrqPolarity_O & 0xff),                  (0x01 << NA_IrqPolarity_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_IrqPolarity_I},
	/* NA_IrqDriver */
	{(NA_IrqDriver_O >> 8),                 (NA_IrqDriver_O & 0xff),                    (0x01 << NA_IrqDriver_B),                       BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_IrqDriver_I},
	//0x01
	/* NA_Version */
	{(NA_Version_O >> 8),                   (NA_Version_O & 0xff),                      1,                                              0,                          NTRX_RO_REG,    NTRX_NC_REG,    NA_Version_I},
	/* NA_WakeUpTimeByte */
	{(NA_WakeUpTimeByte_O >> 8),            (NA_WakeUpTimeByte_O & 0xff),               0xff,                                           0,                          NTRX_WO_REG,    NTRX_DUMMY_REG, NA_WakeUpTimeByte_I},
	//0x02
	/* NA_Revision */
	{(NA_Revision_O >> 8),                  (NA_Revision_O & 0xff),                     1,                                              0,                          NTRX_RO_REG,    NTRX_NC_REG,    NA_Revision_I},
	/* NA_WakeUpTimeWe */
	{(NA_WakeUpTimeWe_O >> 8),              (NA_WakeUpTimeWe_O & 0xff),                 (0x07 << NA_WakeUpTimeWe_LSB),                  NA_WakeUpTimeWe_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_WakeUpTimeWe_I},
	//0x03
	/* NA_BattMgmtEnable */
	{(NA_BattMgmtEnable_O >> 8),            (NA_BattMgmtEnable_O & 0xff),               (0x01 << NA_BattMgmtEnable_B),                  BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_BattMgmtEnable_I},
	/* NA_BattMgmtThreshold */
	{(NA_BattMgmtThreshold_O >> 8),         (NA_BattMgmtThreshold_O & 0xff),            (0x0f << NA_BattMgmtThreshold_LSB),             NA_BattMgmtThreshold_LSB,   NTRX_RW_REG,    NTRX_TC_REG,    NA_BattMgmtThreshold_I},
	/* NA_BattMgmtCompare */ /* 10 */
	{(NA_BattMgmtCompare_O >> 8),           (NA_BattMgmtCompare_O & 0xff),              (0x01 << NA_BattMgmtCompare_B),                 BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_BattMgmtCompare_I},
	//0x04
	/* NA_DioDirection */
	{(NA_DioDirection_O >> 8),              (NA_DioDirection_O & 0xff),                 (0x01 << NA_DioDirection_B),                    BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioDirection_I},
	/* NA_DioOutValueAlarmEnable */
	{(NA_DioOutValueAlarmEnable_O >> 8),    (NA_DioOutValueAlarmEnable_O & 0xff),       (0x01 << NA_DioOutValueAlarmEnable_B),          BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioOutValueAlarmEnable_I},
	/* NA_DioAlarmStart */
	{(NA_DioAlarmStart_O >> 8),             (NA_DioAlarmStart_O & 0xff),                (0x01 << NA_DioAlarmStart_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioAlarmStart_I},
	/* NA_DioAlarmPolarity */
	{(NA_DioAlarmPolarity_O >> 8),          (NA_DioAlarmPolarity_O & 0xff),             (0x01 << NA_DioAlarmPolarity_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioAlarmPolarity_I},
	/* NA_DioUsePullup */
	{(NA_DioUsePullup_O >> 8),              (NA_DioUsePullup_O & 0xff),                 (0x01 << NA_DioUsePullup_B),                    BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioUsePullup_I},
	/* NA_DioUsePulldown */
	{(NA_DioUsePulldown_O >> 8),            (NA_DioUsePulldown_O & 0xff),               (0x01 << NA_DioUsePulldown_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_DioUsePulldown_I},
	/* NA_DioInValueAlarmStatus */
	{(NA_DioInValueAlarmStatus_O >> 8),     (NA_DioInValueAlarmStatus_O & 0xff),        (0x0f << NA_DioInValueAlarmStatus_LSB),         NA_DioInValueAlarmStatus_LSB,NTRX_RO_REG,   NTRX_NC_REG,    NA_DioInValueAlarmStatus_I},
	//0x05
	/* NA_DioPortWe */
	{(NA_DioPortWe_O >> 8),                 (NA_DioPortWe_O & 0xff),                    (0x0f << NA_DioPortWe_LSB),                     NA_DioPortWe_LSB,           NTRX_WO_REG,    NTRX_NC_REG,    NA_DioPortWe_I},
	//0x06
	/* NA_EnableWakeUpRtc */
	{(NA_EnableWakeUpRtc_O >> 8),           (NA_EnableWakeUpRtc_O & 0xff),              (0x01 << NA_EnableWakeUpRtc_B),                 BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_EnableWakeUpRtc_I},
	/* NA_EnableWakeUpDio */ /* 20 */
	{(NA_EnableWakeUpDio_O >> 8),           (NA_EnableWakeUpDio_O & 0xff),              (0x01 << NA_EnableWakeUpDio_B),                 BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_EnableWakeUpDio_I},
	/* NA_PowerUpTime */
	{(NA_PowerUpTime_O >> 8),               (NA_PowerUpTime_O & 0xff),                  (0x07 << NA_PowerUpTime_LSB),                   NA_PowerUpTime_LSB,         NTRX_RW_REG,    NTRX_TC_REG,    NA_PowerUpTime_I},
	/* NA_PowerDownMode */
	{(NA_PowerDownMode_O >> 8),             (NA_PowerDownMode_O & 0xff),                (0x01 << NA_PowerDownMode_B),                   BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_PowerDownMode_I},
	//0x07
	/* NA_PowerDown */
	{(NA_PowerDown_O >> 8),                 (NA_PowerDown_O & 0xff),                    (0x01 << NA_PowerDown_B),                       BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_PowerDown_I},
	/* NA_ResetBbClockGate */
	{(NA_ResetBbClockGate_O >> 8),          (NA_ResetBbClockGate_O & 0xff),             (0x01 << NA_ResetBbClockGate_B),                BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_ResetBbClockGate_I},
	/* NA_ResetBbRadioCtrl */
	{(NA_ResetBbRadioCtrl_O >> 8),          (NA_ResetBbRadioCtrl_O & 0xff),             (0x01 << NA_ResetBbRadioCtrl_B),                BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_ResetBbRadioCtrl_I},
	/* NA_UsePullup4Test */
	{(NA_UsePullup4Test_O >> 8),            (NA_UsePullup4Test_O & 0xff),               (0x01 << NA_UsePullup4Test_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Test_I},
	/* NA_UsePulldown4Test */
	{(NA_UsePulldown4Test_O >> 8),          (NA_UsePulldown4Test_O & 0xff),             (0x01 << NA_UsePulldown4Test_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Test_I},
	//0x08
	/* NA_EnableBbCrystal */
	{(NA_EnableBbCrystal_O >> 8),           (NA_EnableBbCrystal_O & 0xff),              (0x01 << NA_EnableBbCrystal_B),                 BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_EnableBbCrystal_I},
	/* NA_EnableBbClock */
	{(NA_EnableBbClock_O >> 8),             (NA_EnableBbClock_O & 0xff),                (0x01 << NA_EnableBbClock_B),                   BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_EnableBbClock_I},
	/* NA_BypassBbCrystal */ /* 30 */
	{(NA_BypassBbCrystal_O >> 8),           (NA_BypassBbCrystal_O & 0xff),              (0x01 << NA_BypassBbCrystal_B),                 BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_BypassBbCrystal_I},
	/* NA_FeatureClockFreq */
	{(NA_FeatureClockFreq_O >> 8),          (NA_FeatureClockFreq_O & 0xff),             (0x07 << NA_FeatureClockFreq_LSB),              NA_FeatureClockFreq_LSB,    NTRX_RW_REG,    NTRX_TC_REG,    NA_FeatureClockFreq_I},
	/* NA_EnableFeatureClock */
	{(NA_EnableFeatureClock_O >> 8),        (NA_EnableFeatureClock_O & 0xff),           (0x01 << NA_EnableFeatureClock_B),              BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_EnableFeatureClock_I},
	//0x09
	/* NA_UsePullup4Spiclk */
	{(NA_UsePullup4Spiclk_O >> 8),          (NA_UsePullup4Spiclk_O & 0xff),             (0x01 << NA_UsePullup4Spiclk_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Spiclk_I},
	/* NA_UsePulldown4Spiclk */
	{(NA_UsePulldown4Spiclk_O >> 8),        (NA_UsePulldown4Spiclk_O & 0xff),           (0x01 << NA_UsePulldown4Spiclk_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Spiclk_I},
	/* NA_UsePullup4Spissn */
	{(NA_UsePullup4Spissn_O >> 8),          (NA_UsePullup4Spissn_O & 0xff),             (0x01 << NA_UsePullup4Spissn_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Spissn_I},
	/* NA_UsePulldown4Spissn */
	{(NA_UsePulldown4Spissn_O >> 8),        (NA_UsePulldown4Spissn_O & 0xff),           (0x01 << NA_UsePulldown4Spissn_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Spissn_I},
	/* NA_UsePullup4Spirxd */
	{(NA_UsePullup4Spirxd_O >> 8),          (NA_UsePullup4Spirxd_O & 0xff),             (0x01 << NA_UsePullup4Spirxd_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Spirxd_I},
	/* NA_UsePulldown4Spirxd */
	{(NA_UsePulldown4Spirxd_O >> 8),        (NA_UsePulldown4Spirxd_O & 0xff),           (0x01 << NA_UsePulldown4Spirxd_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Spirxd_I},
	/* NA_UsePullup4Spitxd */
	{(NA_UsePullup4Spitxd_O >> 8),          (NA_UsePullup4Spitxd_O & 0xff),             (0x01 << NA_UsePullup4Spitxd_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Spitxd_I},
	/* NA_UsePulldown4Spitxd */ /* 40 */
	{(NA_UsePulldown4Spitxd_O >> 8),        (NA_UsePulldown4Spitxd_O & 0xff),           (0x01 << NA_UsePulldown4Spitxd_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Spitxd_I},
	//0x0a
	/* NA_UsePullup4Por */
	{(NA_UsePullup4Por_O >> 8),             (NA_UsePullup4Por_O & 0xff),                (0x01 << NA_UsePullup4Por_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Por_I},
	/* NA_UsePulldown4Por */
	{(NA_UsePulldown4Por_O >> 8),           (NA_UsePulldown4Por_O & 0xff),              (0x01 << NA_UsePulldown4Por_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Por_I},
	/* NA_UsePullup4Pamp */
	{(NA_UsePullup4Pamp_O >> 8),            (NA_UsePullup4Pamp_O & 0xff),               (0x01 << NA_UsePullup4Pamp_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Pamp_I},
	/* NA_UsePulldown4Pamp */
	{(NA_UsePulldown4Pamp_O >> 8),          (NA_UsePulldown4Pamp_O & 0xff),             (0x01 << NA_UsePulldown4Pamp_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Pamp_I},
	/* NA_UsePullup4Ucirq */
	{(NA_UsePullup4Ucirq_O >> 8),           (NA_UsePullup4Ucirq_O & 0xff),              (0x01 << NA_UsePullup4Ucirq_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Ucirq_I},
	/* NA_UsePulldown4Ucirq */
	{(NA_UsePulldown4Ucirq_O >> 8),         (NA_UsePulldown4Ucirq_O & 0xff),            (0x01 << NA_UsePulldown4Ucirq_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Ucirq_I},
	/* NA_UsePullup4Ucrst */
	{(NA_UsePullup4Ucrst_O >> 8),           (NA_UsePullup4Ucrst_O & 0xff),              (0x01 << NA_UsePullup4Ucrst_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePullup4Ucrst_I},
	/* NA_UsePulldown4Ucrst */
	{(NA_UsePulldown4Ucrst_O >> 8),         (NA_UsePulldown4Ucrst_O & 0xff),            (0x01 << NA_UsePulldown4Ucrst_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UsePulldown4Ucrst_I},
	//0x0b
	/* NA_WritePulls4Spi */
	{(NA_WritePulls4Spi_O >> 8),            (NA_WritePulls4Spi_O & 0xff),               (0x01 << NA_WritePulls4Spi_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_WritePulls4Spi_I},
	/* NA_WritePulls4Pads */ /* 50 */
	{(NA_WritePulls4Pads_O >> 8),           (NA_WritePulls4Pads_O & 0xff),              (0x01 << NA_WritePulls4Pads_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_WritePulls4Pads_I},
	//0x0e
	/* NA_RamIndex */
	{(NA_RamIndex_O >> 8),                  (NA_RamIndex_O & 0xff),                     (0x03 << NA_RamIndex_LSB),                      NA_RamIndex_LSB,            NTRX_RW_REG,    NTRX_TC_REG,    NA_RamIndex_I},
	/* NA_DeviceSelect */
	{(NA_DeviceSelect_O >> 8),              (NA_DeviceSelect_O & 0xff),                 (0x03 << NA_DeviceSelect_LSB),                  NA_DeviceSelect_LSB,        NTRX_RW_REG,    NTRX_TC_REG,    NA_DeviceSelect_I},
	//0x0f
	/* NA_TxIrqEnable */
	{(NA_TxIrqEnable_O >> 8),               (NA_TxIrqEnable_O & 0xff),                  (0x01 << NA_TxIrqEnable_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_TxIrqEnable_I},
	/* NA_RxIrqEnable */
	{(NA_RxIrqEnable_O >> 8),               (NA_RxIrqEnable_O & 0xff),                  (0x01 << NA_RxIrqEnable_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_RxIrqEnable_I},
	/* NA_BbTimerIrqEnable */
	{(NA_BbTimerIrqEnable_O >> 8),          (NA_BbTimerIrqEnable_O & 0xff),             (0x01 << NA_BbTimerIrqEnable_B),                BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_BbTimerIrqEnable_I},
	/* NA_LoIrqEnable */
	{(NA_LoIrqEnable_O >> 8),               (NA_LoIrqEnable_O & 0xff),                  (0x01 << NA_LoIrqEnable_B),                     BOOL_OP,                    NTRX_RW_REG,    NTRX_TC_REG,    NA_LoIrqEnable_I},
	/* NA_TxIrqStatus */
	{(NA_TxIrqStatus_O >> 8),               (NA_TxIrqStatus_O & 0xff),                  (0x01 << NA_TxIrqStatus_B),                     BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_TxIrqStatus_I},
	/* NA_RxIrqStatus */ /* 60 */
	{(NA_RxIrqStatus_O >> 8),               (NA_RxIrqStatus_O & 0xff),                  (0x01 << NA_RxIrqStatus_B),                     BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxIrqStatus_I},
	/* NA_BbTimerIrqStatus */
	{(NA_BbTimerIrqStatus_O >> 8),          (NA_BbTimerIrqStatus_O & 0xff),             (0x01 << NA_BbTimerIrqStatus_B),                BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_BbTimerIrqStatus_I},
	/* NA_LoIrqStatus */
	{(NA_LoIrqStatus_O >> 8),               (NA_LoIrqStatus_O & 0xff),                  (0x01 << NA_LoIrqStatus_B),                     BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_LoIrqStatus_I},
	//0x10
	/* NA_TxIntsRawStat */
	{(NA_TxIntsRawStat_O >> 8),             (NA_TxIntsRawStat_O & 0xff),                (0x3f << NA_TxIntsRawStat_LSB),                 NA_TxIntsRawStat_LSB,       NTRX_RO_REG,    NTRX_TC_REG,    NA_TxIntsRawStat_I},
	/* NA_TxIntsReset */
	{(NA_TxIntsReset_O >> 8),               (NA_TxIntsReset_O & 0xff),                  (0x3f << NA_TxIntsReset_LSB),                   NA_TxIntsReset_LSB,         NTRX_WO_REG,    NTRX_TC_REG,    NA_TxIntsReset_I},
	//0x11
	/* NA_RxIntsRawStat */
	{(NA_RxIntsRawStat_O >> 8),             (NA_RxIntsRawStat_O & 0xff),                (0x7f << NA_RxIntsRawStat_LSB),                 NA_RxIntsRawStat_LSB,       NTRX_RO_REG,    NTRX_TC_REG,    NA_RxIntsRawStat_I},
	/* NA_RxIntsReset */
	{(NA_RxIntsReset_O >> 8),               (NA_RxIntsReset_O & 0xff),                  (0x7f << NA_RxIntsReset_LSB),                   NA_RxIntsReset_LSB,         NTRX_WO_REG,    NTRX_TC_REG,    NA_RxIntsReset_I},
	//0x12
	/* NA_LoIntsRawStat */
	{(NA_LoIntsRawStat_O >> 8),             (NA_LoIntsRawStat_O & 0xff),                (0x01 << NA_LoIntsRawStat_B),                   BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_LoIntsRawStat_I},
	/* NA_LoIntsReset */
	{(NA_LoIntsReset_O >> 8),               (NA_LoIntsReset_O & 0xff),                  (0x03 << NA_LoIntsReset_LSB),                   NA_LoIntsReset_LSB,         NTRX_WO_REG,    NTRX_TC_REG,    NA_LoIntsReset_I},
	/* NA_ClearBasebandTimerInt */
	{(NA_ClearBasebandTimerInt_O >> 8),     (NA_ClearBasebandTimerInt_O & 0xff),        (0x01 << NA_ClearBasebandTimerInt_B),           BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_ClearBasebandTimerInt_I},
	//0x13
	/* NA_TxIntsEn */ /* 70 */
	{(NA_TxIntsEn_O >> 8),                  (NA_TxIntsEn_O & 0xff),                     (0x3f << NA_TxIntsEn_LSB),                      NA_TxIntsEn_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_TxIntsEn_I},
	//0x14
	/* NA_RxIntsEn */
	{(NA_RxIntsEn_O >> 8),                  (NA_RxIntsEn_O & 0xff),                     (0x7f << NA_RxIntsEn_LSB),                      NA_RxIntsEn_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_RxIntsEn_I},
	//0x15
	/* NA_LoIntsEn */
	{(NA_LoIntsEn_O >> 8),                  (NA_LoIntsEn_O & 0xff),                     (0x01 << NA_LoIntsEn_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_LoIntsEn_I},

	// LO related registers
	//0x1c
	/* NA_LoEnableFastTuning */
	{(NA_LoEnableFastTuning_O >> 8),        (NA_LoEnableFastTuning_O & 0xff),           (0x01 << NA_LoEnableFastTuning_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_LoEnableFastTuning_I},
	/* NA_LoFastTuningLevel */
	{(NA_LoFastTuningLevel_O >> 8),         (NA_LoFastTuningLevel_O & 0xff),            (0x07 << NA_LoFastTuningLevel_LSB),             NA_LoFastTuningLevel_LSB,   NTRX_WO_REG,    NTRX_TC_REG,    NA_LoFastTuningLevel_I},
	/* NA_LoEnableLsbNeg */
	{(NA_LoEnableLsbNeg_O >> 8),            (NA_LoEnableLsbNeg_O & 0xff),               (0x01 << NA_LoEnableLsbNeg_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_LoEnableLsbNeg_I},
	/* NA_UseLoRxCaps */
	{(NA_UseLoRxCaps_O >> 8),               (NA_UseLoRxCaps_O & 0xff),                  (0x01 << NA_UseLoRxCaps_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UseLoRxCaps_I},
	//0x1d
	/* NA_LoTargetValue */
	{(NA_LoTargetValue_O >> 8),             (NA_LoTargetValue_O & 0xff),                2,                                              0,                          NTRX_WO_REG,    NTRX_RW_REG,    0},
	//0x1f
	/* NA_AgcThresHold1 */
	{(NA_AgcThresHold1_O >> 8),             (NA_AgcThresHold1_O & 0xff),                (0xff << NA_AgcThresHold1_LSB),                 NA_AgcThresHold1_LSB,       NTRX_WO_REG,    NTRX_NC_REG,    NA_AgcThresHold1_I},
	//0x20
	/* NA_AgcThresHold2 */ /* 80 */
	{(NA_AgcThresHold2_O >> 8),             (NA_AgcThresHold2_O & 0xff),                (0xff << NA_AgcThresHold2_LSB),                 NA_AgcThresHold2_LSB,       NTRX_WO_REG,    NTRX_NC_REG,    NA_AgcThresHold2_I},
	//0x21
	/* NA_HoldAgcInBitSync */
	{(NA_HoldAgcInBitSync_O >> 8),          (NA_HoldAgcInBitSync_O & 0xff),             (0x7f << NA_HoldAgcInBitSync_LSB),              NA_HoldAgcInBitSync_LSB,    NTRX_WO_REG,    NTRX_TC_REG,    NA_HoldAgcInBitSync_I},
	/* NA_HoldAgcInFrameSync */
	{(NA_HoldAgcInFrameSync_O >> 8),        (NA_HoldAgcInFrameSync_O & 0xff),           (0x01 << NA_HoldAgcInFrameSync_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_HoldAgcInFrameSync_I},
	//0x22
	/* NA_AgcDeadTime */
	{(NA_AgcDeadTime_O >> 8),               (NA_AgcDeadTime_O & 0xff),                  (0x3f << NA_AgcDeadTime_LSB),                   NA_AgcDeadTime_LSB,         NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcDeadTime_I},
	/* NA_AgcNregLength */
	{(NA_AgcNregLength_O >> 8),             (NA_AgcNregLength_O & 0xff),                (0x03 << NA_AgcNregLength_LSB),                 NA_AgcNregLength_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcNregLength_I},
	//0x23
	/* NA_AgcIntTime */
	{(NA_AgcIntTime_O >> 8),                (NA_AgcIntTime_O & 0xff),                   2,                                              0,                          NTRX_WO_REG,    NTRX_TC_REG,    0},
	// AGC related registers
	//0x25
	/* NA_AgcValue */
	{(NA_AgcValue_O >> 8),                  (NA_AgcValue_O & 0xff),                     (0x3f << NA_AgcValue_LSB),                      NA_AgcValue_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcValue_I},
	/* NA_AgcDefaultEn */
	{(NA_AgcDefaultEn_O >> 8),              (NA_AgcDefaultEn_O & 0xff),                 (0x01 << NA_AgcDefaultEn_B),                    BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcDefaultEn_I},
	/* NA_AgcHold */
	{(NA_AgcHold_O >> 8),                   (NA_AgcHold_O & 0xff),                      (0x01 << NA_AgcHold_B),                         BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcHold_I},
	//0x26
	/* NA_AgcRssiThres */
	{(NA_AgcRssiThres_O >> 8),              (NA_AgcRssiThres_O & 0xff),                 (0x3f << NA_AgcRssiThres_LSB),                  NA_AgcRssiThres_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcRssiThres_I},
	/* NA_AgcGain */ /* 90 */
	{(NA_AgcGain_O >> 8),                   (NA_AgcGain_O & 0xff),                      (0x3f << NA_AgcGain_LSB),                       NA_AgcGain_LSB,             NTRX_RO_REG,    NTRX_TC_REG,    NA_AgcGain_I},
	//0x27
	/* NA_ChirpFilterCaps */
	{(NA_ChirpFilterCaps_O >> 8),           (NA_ChirpFilterCaps_O & 0xff),              (0x0f << NA_ChirpFilterCaps_LSB),               NA_ChirpFilterCaps_LSB,     NTRX_WO_REG,    NTRX_TC_REG,    NA_ChirpFilterCaps_I},
	/* NA_FctClockEn */
	{(NA_FctClockEn_O >> 8),                (NA_FctClockEn_O & 0xff),                   (0x01 << NA_FctClockEn_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_FctClockEn_I},
	/* NA_StartFctMeasure */
	{(NA_StartFctMeasure_O >> 8),           (NA_StartFctMeasure_O & 0xff),              (0x01 << NA_StartFctMeasure_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_StartFctMeasure_I},

	/* NA_EnableTx */
	{(NA_EnableTx_O >> 8),                  (NA_EnableTx_O & 0xff),                     (0x01 << NA_EnableTx_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_EnableTx_I},
	/* NA_FctPeriod */
	{(NA_FctPeriod_O >> 8),                 (NA_FctPeriod_O & 0xff),                    (0x0f << NA_FctPeriod_LSB),                     NA_FctPeriod_LSB,           NTRX_WO_REG,    NTRX_TC_REG,    NA_FctPeriod_I},
	//0x2b
	/* NA_ToaOffsetMeanDataValid */
	{(NA_ToaOffsetMeanDataValid_O >> 8),    (NA_ToaOffsetMeanDataValid_O & 0xff),       (0x01 << NA_ToaOffsetMeanDataValid_B),          BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_ToaOffsetMeanDataValid_I},
	//0x2e
	/* NA_PhaseOffsetData */
	{(NA_PhaseOffsetData_O >> 8),           (NA_PhaseOffsetData_O & 0xff),              (0x07 << NA_PhaseOffsetData_LSB),               NA_PhaseOffsetData_LSB,     NTRX_RO_REG,    NTRX_TC_REG,    NA_PhaseOffsetData_I},
	/* NA_PhaseOffsetAck */
	{(NA_PhaseOffsetAck_O >> 8),            (NA_PhaseOffsetAck_O & 0xff),               (0x07 << NA_PhaseOffsetAck_LSB),                NA_PhaseOffsetAck_LSB,      NTRX_RO_REG,    NTRX_TC_REG,    NA_PhaseOffsetAck_I},
	//0x30
	/* NA_ToaOffsetMeanAckValid */ /* 100 */
	{(NA_ToaOffsetMeanAckValid_O >> 8),     (NA_ToaOffsetMeanAckValid_O & 0xff),        (0x01 << NA_ToaOffsetMeanAckValid_B),           BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_ToaOffsetMeanAckValid_I},
	//0x31
	/* NA_RxPacketType */
	{(NA_RxPacketType_O >> 8),              (NA_RxPacketType_O & 0xff),                 (0x0f << NA_RxPacketType_LSB),                  NA_RxPacketType_LSB,        NTRX_RO_REG,    NTRX_TC_REG,    NA_RxPacketType_I},
	/* NA_RxAddrMatch */
	{(NA_RxAddrMatch_O >> 8),               (NA_RxAddrMatch_O & 0xff),                  (0x0f << NA_RxAddrMatch_LSB),                   NA_RxAddrMatch_LSB,         NTRX_RO_REG,    NTRX_TC_REG,    NA_RxAddrMatch_I},
	/* NA_RxCrc1Stat */
	{(NA_RxCrc1Stat_O >> 8),                (NA_RxCrc1Stat_O & 0xff),                   (0x01 << NA_RxCrc1Stat_B),                      BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCrc1Stat_I},
	/* NA_RxCrc2Stat */
	{(NA_RxCrc2Stat_O >> 8),                (NA_RxCrc2Stat_O & 0xff),                   (0x01 << NA_RxCrc2Stat_B),                      BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCrc2Stat_I},
	//0x32
	/* NA_RxCorrBitErr */
	{(NA_RxCorrBitErr_O >> 8),              (NA_RxCorrBitErr_O & 0xff),                 (0x0f << NA_RxCorrBitErr_LSB),                  NA_RxCorrBitErr_LSB,        NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCorrBitErr_I},
	/* NA_RxCorrErrThres */
	{(NA_RxCorrErrThres_O >> 8),            (NA_RxCorrErrThres_O & 0xff),               (0x0f << NA_RxCorrErrThres_LSB),                NA_RxCorrErrThres_LSB,      NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCorrErrThres_I},
	//0x33
	/* NA_RxAddrSegEsMatch */
	{(NA_RxAddrSegEsMatch_O >> 8),          (NA_RxAddrSegEsMatch_O & 0xff),             (0x01 << NA_RxAddrSegEsMatch_B),                BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxAddrSegEsMatch_I},
	/* NA_RxAddrSegIsMatch */
	{(NA_RxAddrSegIsMatch_O >> 8),          (NA_RxAddrSegIsMatch_O & 0xff),             (0x01 << NA_RxAddrSegIsMatch_B),                BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxAddrSegIsMatch_I},
	/* NA_RxCryptEn */
	{(NA_RxCryptEn_O >> 8),                 (NA_RxCryptEn_O & 0xff),                    (0x01 << NA_RxCryptEn_B),                       BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCryptEn_I},
	/* NA_RxCryptId */ /* 110 */
	{(NA_RxCryptId_O >> 8),                 (NA_RxCryptId_O & 0xff),                    (0x03 << NA_RxCryptId_LSB),                     NA_RxCryptId_LSB,           NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCryptId_I},
	/* NA_RxCryptSeqN */
	{(NA_RxCryptSeqN_O >> 8),               (NA_RxCryptSeqN_O & 0xff),                  (0x01 << NA_RxCryptSeqN_B),                     BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxCryptSeqN_I},
	//0x37
	/* NA_TxTimeSlotControl */
	{(NA_TxTimeSlotControl_O >> 8),         (NA_TxTimeSlotControl_O & 0xff),            (0x01 << NA_TxTimeSlotControl_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxTimeSlotControl_I},
	/* NA_RxTimeSlotControl */
	{(NA_RxTimeSlotControl_O >> 8),         (NA_RxTimeSlotControl_O & 0xff),            (0x01 << NA_RxTimeSlotControl_B),               BOOL_OP,                    NTRX_RO_REG,    NTRX_TC_REG,    NA_RxTimeSlotControl_I},
	//0x3c
	/* NA_TxArqCnt */
	{(NA_TxArqCnt_O >> 8),                  (NA_TxArqCnt_O & 0xff),                     (0x0f << NA_TxArqCnt_LSB),                      NA_TxArqCnt_LSB,            NTRX_RO_REG,    NTRX_TC_REG,    NA_TxArqCnt_I},
	/* NA_TxArqMax */
	{(NA_TxArqMax_O >> 8),                  (NA_TxArqMax_O & 0xff),                     (0x0f << NA_TxArqMax_LSB),                      NA_TxArqMax_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_TxArqMax_I},
	//0x3d
	/* NA_CsqDitherValue */
	{(NA_CsqDitherValue_O >> 8),            (NA_CsqDitherValue_O & 0xff),               (0x03 << NA_CsqDitherValue_LSB),                NA_CsqDitherValue_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqDitherValue_I},
	/* NA_CsqUsePhaseShift */
	{(NA_CsqUsePhaseShift_O >> 8),          (NA_CsqUsePhaseShift_O & 0xff),             (0x01 << NA_CsqUsePhaseShift_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqUsePhaseShift_I},
	/* NA_CsqUse4Phases */
	{(NA_CsqUse4Phases_O >> 8),             (NA_CsqUse4Phases_O & 0xff),                (0x01 << NA_CsqUse4Phases_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqUse4Phases_I},
	/* NA_CsqAsyMode */
	{(NA_CsqAsyMode_O >> 8),                (NA_CsqAsyMode_O & 0xff),                   (0x01 << NA_CsqAsyMode_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqAsyMode_I},
	/* NA_CsqMemAddrInit */ /* 120 */
	{(NA_CsqMemAddrInit_O >> 8),            (NA_CsqMemAddrInit_O & 0xff),               (0x01 << NA_CsqMemAddrInit_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqMemAddrInit_I},
	/* NA_CsqUseRam */
	{(NA_CsqUseRam_O >> 8),                 (NA_CsqUseRam_O & 0xff),                    (0x01 << NA_CsqUseRam_B),                       BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_CsqUseRam_I},
	//0x3f
	/* NA_D3lFixnMap */
	{(NA_D3lFixnMap_O >> 8),                (NA_D3lFixnMap_O & 0xff),                   (0x01 << NA_D3lFixnMap_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_D3lFixnMap_I},
	/* NA_D3lPomEn */
	{(NA_D3lPomEn_O >> 8),                  (NA_D3lPomEn_O & 0xff),                     (0x01 << NA_D3lPomEn_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_D3lPomEn_I},
	/* NA_D3lPomLen */
	{(NA_D3lPomLen_O >> 8),                 (NA_D3lPomLen_O & 0xff),                    (0x03 << NA_D3lPomLen_LSB),                     NA_D3lPomLen_LSB,           NTRX_WO_REG,    NTRX_TC_REG,    NA_D3lPomLen_I},
	/* NA_D3lUpDownEx */
	{(NA_D3lUpDownEx_O >> 8),               (NA_D3lUpDownEx_O & 0xff),                  (0x01 << NA_D3lUpDownEx_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_D3lUpDownEx_I},
	//0x40
	/* NA_LeaveMapThresh1InBitsync */ /* 130 */
	{(NA_LeaveMapThresh1InBitsync_O >> 8),  (NA_LeaveMapThresh1InBitsync_O & 0xff),     (0x7f << NA_LeaveMapThresh1InBitsync_LSB),      NA_LeaveMapThresh1InBitsync_LSB,NTRX_WO_REG,    NTRX_TC_REG,    NA_LeaveMapThresh1InBitsync_I},
	/* NA_UseMapThresh1InFramesync */
	{(NA_UseMapThresh1InFramesync_O >> 8),  (NA_UseMapThresh1InFramesync_O & 0xff),     (0x01 << NA_UseMapThresh1InFramesync_B),        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UseMapThresh1InFramesync_I},
	//0x41
	/* NA_Go2MapThresh1InBitsync */
	{(NA_Go2MapThresh1InBitsync_O >> 8),    (NA_Go2MapThresh1InBitsync_O & 0xff),       (0x7f << NA_Go2MapThresh1InBitsync_LSB),        NA_Go2MapThresh1InBitsync_LSB,NTRX_WO_REG,  NTRX_TC_REG,    NA_Go2MapThresh1InBitsync_I},
	//0x42
	/* NA_EnableLO */
	{(NA_EnableLO_O >> 8),                  (NA_EnableLO_O & 0xff),                     (0x01 << NA_EnableLO_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_EnableLO_I},
	/* NA_EnableLOdiv10 */
	{(NA_EnableLOdiv10_O >> 8),             (NA_EnableLOdiv10_O & 0xff),                (0x01 << NA_EnableLOdiv10_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_EnableLOdiv10_I},
	/* NA_EnableCsqClock */
	{(NA_EnableCsqClock_O >> 8),            (NA_EnableCsqClock_O & 0xff),               (0x01 << NA_EnableCsqClock_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_EnableCsqClock_I},
	/* NA_InvertRxClock */
	{(NA_InvertRxClock_O >> 8),             (NA_InvertRxClock_O & 0xff),                (0x01 << NA_InvertRxClock_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_InvertRxClock_I},
	/* NA_EnableExtPA */
	{(NA_EnableExtPA_O >> 8),               (NA_EnableExtPA_O & 0xff),                  (0x01 << NA_EnableExtPA_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_EnableExtPA_I},
	//0x43
	/* NA_LnaFreqAdjust */
	{(NA_LnaFreqAdjust_O >> 8),             (NA_LnaFreqAdjust_O & 0xff),                (0x7 << NA_LnaFreqAdjust_LSB),                  NA_LnaFreqAdjust_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_LnaFreqAdjust_I},
	/* NA_TxPaBias */
	{(NA_TxPaBias_O >> 8),                  (NA_TxPaBias_O & 0xff),                     (0x7 << NA_TxPaBias_LSB),                       NA_TxPaBias_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_TxPaBias_I},
	//0x44
	/* NA_TxOutputPower0 */
	{(NA_TxOutputPower0_O >> 8),            (NA_TxOutputPower0_O & 0xff),               (0x3f << NA_TxOutputPower0_LSB),                NA_TxOutputPower0_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_TxOutputPower0_I},
	//0x45
	/* NA_TxOutputPower1 */
	{(NA_TxOutputPower1_O >> 8),            (NA_TxOutputPower1_O & 0xff),               (0x3f << NA_TxOutputPower1_LSB),                NA_TxOutputPower1_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_TxOutputPower1_I},
	//0x46
	/* NA_RfRxCompValueI */
	{(NA_RfRxCompValueI_O >> 8),            (NA_RfRxCompValueI_O & 0xff),               (0x1f << NA_RfRxCompValueI_LSB),                NA_RfRxCompValueI_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_RfRxCompValueI_I},
	//0x47
	/* NA_RfRxCompValueQ */
	{(NA_RfRxCompValueQ_O >> 8),            (NA_RfRxCompValueQ_O & 0xff),               (0x1f << NA_RfRxCompValueQ_LSB),                NA_RfRxCompValueQ_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_RfRxCompValueQ_I},
	//0x48
	/* NA_SymbolDur */
	{(NA_SymbolDur_O >> 8),                 (NA_SymbolDur_O & 0xff),                    (0x7 << NA_SymbolDur_LSB),                      NA_SymbolDur_LSB,           NTRX_WO_REG,    NTRX_TC_REG,    NA_SymbolDur_I},
	/* NA_SymbolRate */
	{(NA_SymbolRate_O >> 8),                (NA_SymbolRate_O & 0xff),                   (0x7 << NA_SymbolRate_LSB),                     NA_SymbolRate_LSB,          NTRX_WO_REG,    NTRX_TC_REG,    NA_SymbolRate_I},
	/* NA_ModulationSystem */ /* 150 */
	{(NA_ModulationSystem_O >> 8),          (NA_ModulationSystem_O & 0xff),             (0x01 << NA_ModulationSystem_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_ModulationSystem_I},
	//0x49
	/* NA_Crc2Type */
	{(NA_Crc2Type_O >> 8),                  (NA_Crc2Type_O & 0xff),                     (0x3 << NA_Crc2Type_LSB),                       NA_Crc2Type_LSB,            NTRX_WO_REG,    NTRX_TC_REG,    NA_Crc2Type_I},
	/* NA_UseFec */
	{(NA_UseFec_O >> 8),                    (NA_UseFec_O & 0xff),                       (0x01 << NA_UseFec_B),                          BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UseFec_I},
	/* NA_TxRxCryptCrc2Mode */
	{(NA_TxRxCryptCrc2Mode_O >> 8),         (NA_TxRxCryptCrc2Mode_O & 0xff),            (0x01 << NA_TxRxCryptCrc2Mode_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxRxCryptCrc2Mode_I},
	/* NA_TxRxCryptClkMode */
	{(NA_TxRxCryptClkMode_O >> 8),          (NA_TxRxCryptClkMode_O & 0xff),             (0x0f << NA_TxRxCryptClkMode_LSB),              NA_TxRxCryptClkMode_LSB,    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxRxCryptClkMode_I},
	//0x4a
	/* NA_SwapBbBuffers */
	{(NA_SwapBbBuffers_O >> 8),             (NA_SwapBbBuffers_O & 0xff),                (0x01 << NA_SwapBbBuffers_B),                   BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_SwapBbBuffers_I},
	/* NA_TxRxBbBufferMode1 */
	{(NA_TxRxBbBufferMode1_O >> 8),         (NA_TxRxBbBufferMode1_O & 0xff),            (0x01 << NA_TxRxBbBufferMode1_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxRxBbBufferMode1_I},
	/* NA_TxRxBbBufferMode0 */
	{(NA_TxRxBbBufferMode0_O >> 8),         (NA_TxRxBbBufferMode0_O & 0xff),            (0x01 << NA_TxRxBbBufferMode0_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxRxBbBufferMode0_I},
	/* NA_FdmaEnable */
	{(NA_FdmaEnable_O >> 8),                (NA_FdmaEnable_O & 0xff),                   (0x01 << NA_FdmaEnable_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_FdmaEnable_I},
	/* NA_TxRxMode */
	{(NA_TxRxMode_O >> 8),                  (NA_TxRxMode_O & 0xff),                     (0x01 << NA_TxRxMode_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxRxMode_I},
	//0x4b
	/* NA_ChirpMatrix0 */ /* 160 */
	{(NA_ChirpMatrix0_O >> 8),              (NA_ChirpMatrix0_O & 0xff),                 (0x07 << NA_ChirpMatrix0_LSB),                  NA_ChirpMatrix0_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_ChirpMatrix0_I},
	/* NA_ChirpMatrix1 */
	{(NA_ChirpMatrix1_O >> 8),              (NA_ChirpMatrix1_O & 0xff),                 (0x07 << NA_ChirpMatrix1_LSB),                  NA_ChirpMatrix1_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_ChirpMatrix1_I},
	//0x4c
	/* NA_ChirpMatrix2 */
	{(NA_ChirpMatrix2_O >> 8),              (NA_ChirpMatrix2_O & 0xff),                 (0x07 << NA_ChirpMatrix2_LSB),                  NA_ChirpMatrix2_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_ChirpMatrix2_I},
	/* NA_ChirpMatrix3 */
	{(NA_ChirpMatrix3_O >> 8),              (NA_ChirpMatrix3_O & 0xff),                 (0x07 << NA_ChirpMatrix3_LSB),                  NA_ChirpMatrix3_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_ChirpMatrix3_I},
	//0x4d
	/* NA_TxPreTrailMatrix0 */
	{(NA_TxPreTrailMatrix0_O >> 8),         (NA_TxPreTrailMatrix0_O & 0xff),            (0x03 << NA_TxPreTrailMatrix0_LSB),             NA_TxPreTrailMatrix0_LSB,   NTRX_WO_REG,    NTRX_TC_REG,    NA_TxPreTrailMatrix0_I},
	/* NA_TxPreTrailMatrix1 */
	{(NA_TxPreTrailMatrix1_O >> 8),         (NA_TxPreTrailMatrix1_O & 0xff),            (0x03 << NA_TxPreTrailMatrix1_LSB),             NA_TxPreTrailMatrix1_LSB,   NTRX_WO_REG,    NTRX_TC_REG,    NA_TxPreTrailMatrix1_I},
	/* NA_TxUnderrunIgnore */
	{(NA_TxUnderrunIgnore_O >> 8),          (NA_TxUnderrunIgnore_O & 0xff),             (0x01 << NA_TxUnderrunIgnore_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxUnderrunIgnore_I},
	/* NA_TxMacCifsDis */
	{(NA_TxMacCifsDis_O >> 8),              (NA_TxMacCifsDis_O & 0xff),                 (0x01 << NA_TxMacCifsDis_B),                    BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxMacCifsDis_I},
	//0x4e
	/* NA_TxVCarrSens */
	{(NA_TxVCarrSens_O >> 8),               (NA_TxVCarrSens_O & 0xff),                  (0x01 << NA_TxVCarrSens_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxVCarrSens_I},
	/* NA_TxPhCarrSenseMode */
	{(NA_TxPhCarrSenseMode_O >> 8),         (NA_TxPhCarrSenseMode_O & 0xff),            (0x03 << NA_TxPhCarrSenseMode_LSB),             NA_TxPhCarrSenseMode_LSB,   NTRX_WO_REG,    NTRX_TC_REG,    NA_TxPhCarrSenseMode_I},
	/* NA_TxVCarrSensAck */  /* 170 */
	{(NA_TxVCarrSensAck_O >> 8),            (NA_TxVCarrSensAck_O & 0xff),               (0x01 << NA_TxVCarrSensAck_B),                  BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxVCarrSensAck_I},
	/* NA_TxArq */
	{(NA_TxArq_O >> 8),                     (NA_TxArq_O & 0xff),                        (0x01 << NA_TxArq_B),                           BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxArq_I},
	/* NA_Tx3Way */
	{(NA_Tx3Way_O >> 8),                    (NA_Tx3Way_O & 0xff),                       (0x01 << NA_Tx3Way_B),                          BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_Tx3Way_I},
	/* NA_TxBackOffAlg */
	{(NA_TxBackOffAlg_O >> 8),              (NA_TxBackOffAlg_O & 0xff),                 (0x01 << NA_TxBackOffAlg_B),                    BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxBackOffAlg_I},
	/* NA_TxFragPrio */
	{(NA_TxFragPrio_O >> 8),                (NA_TxFragPrio_O & 0xff),                   (0x01 << NA_TxFragPrio_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxFragPrio_I},
	//0x4f
	/* NA_TxBackOffSeed */
	{(NA_TxBackOffSeed_O >> 8),             (NA_TxBackOffSeed_O & 0xff),                (0xff << NA_TxBackOffSeed_LSB),                 NA_TxBackOffSeed_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_TxBackOffSeed_I},
	//0x50
	/* NA_TxCryptSeqReset */
	{(NA_TxCryptSeqReset_O >> 8),           (NA_TxCryptSeqReset_O & 0xff),              (0x0f << NA_TxCryptSeqReset_LSB),               NA_TxCryptSeqReset_LSB,     NTRX_WO_REG,    NTRX_TC_REG,    NA_TxCryptSeqReset_I},
	/* NA_TxCryptEn */
	{(NA_TxCryptEn_O >> 8),                 (NA_TxCryptEn_O & 0xff),                    (0x01 << NA_TxCryptEn_B),                       BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxCryptEn_I},
	/* NA_TxCryptId */
	{(NA_TxCryptId_O >> 8),                 (NA_TxCryptId_O & 0xff),                    (0x03 << NA_TxCryptId_LSB),                     NA_TxCryptId_LSB,           NTRX_WO_REG,    NTRX_TC_REG,    NA_TxCryptId_I},
	/* NA_TxCryptSeqN */
	{(NA_TxCryptSeqN_O >> 8),               (NA_TxCryptSeqN_O & 0xff),                  (0x01 << NA_TxCryptSeqN_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxCryptSeqN_I},
	//0x51
	/* NA_TxScrambInit */ /* 180 */
	{(NA_TxScrambInit_O >> 8),              (NA_TxScrambInit_O & 0xff),                 (0x7f << NA_TxScrambInit_LSB),                  NA_TxScrambInit_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_TxScrambInit_I},
	/* NA_TxScrambEn */
	{(NA_TxScrambEn_O >> 8),                (NA_TxScrambEn_O & 0xff),                   (0x01 << NA_TxScrambEn_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxScrambEn_I},
	//0x54
	/* NA_TxPacketType */
	{(NA_TxPacketType_O >> 8),              (NA_TxPacketType_O & 0xff),                 (0x0f << NA_TxPacketType_LSB),                  NA_TxPacketType_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_TxPacketType_I},
	/* NA_TxAddrSlct */
	{(NA_TxAddrSlct_O >> 8),                (NA_TxAddrSlct_O & 0xff),                   (0x01 << NA_TxAddrSlct_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_TxAddrSlct_I},
	//0x55
	/* NA_TxCmdStop */
	{(NA_TxCmdStop_O >> 8),                 (NA_TxCmdStop_O & 0xff),                    (0x01 << NA_TxCmdStop_B),                       BOOL_OP,                    NTRX_NC_REG,    NTRX_TC_REG,    NA_TxCmdStop_I},
	/* NA_TxCmdStart */
	{(NA_TxCmdStart_O >> 8),                (NA_TxCmdStart_O & 0xff),                   (0x01 << NA_TxCmdStart_B),                      BOOL_OP,                    NTRX_NC_REG,    NTRX_TC_REG,    NA_TxCmdStart_I},
	/* NA_TxBufferCmd */
	{(NA_TxBufferCmd_O >> 8),               (NA_TxBufferCmd_O & 0xff),                  (0x03 << NA_TxBufferCmd_LSB),                   NA_TxBufferCmd_LSB,         NTRX_NC_REG,    NTRX_TC_REG,    NA_TxBufferCmd_I},
	//0x56
	/* NA_RxCmdStop */
	{(NA_RxCmdStop_O >> 8),                 (NA_RxCmdStop_O & 0xff),                    (0x01 << NA_RxCmdStop_B),                       BOOL_OP,                    NTRX_NC_REG,    NTRX_TC_REG,    NA_RxCmdStop_I},    /* NA_RxCmdStart */
	/* NA_RxCmdStart */
	{(NA_RxCmdStart_O >> 8),                (NA_RxCmdStart_O & 0xff),                   (0x01 << NA_RxCmdStart_B),                      BOOL_OP,                    NTRX_NC_REG,    NTRX_TC_REG,    NA_RxCmdStart_I},
	/* NA_RxBufferCmd */
	{(NA_RxBufferCmd_O >> 8),               (NA_RxBufferCmd_O & 0xff),                  (0x03 << NA_RxBufferCmd_LSB),                   NA_RxBufferCmd_LSB,         NTRX_NC_REG,    NTRX_TC_REG,    NA_RxBufferCmd_I},
	//0x57
	/* NA_RxCryptSeqReset */ /* 190 */
	{(NA_RxCryptSeqReset_O >> 8),           (NA_RxCryptSeqReset_O & 0xff),              (0x0f << NA_RxCryptSeqReset_LSB),               NA_RxCryptSeqReset_LSB,     NTRX_WO_REG,    NTRX_TC_REG,    NA_RxCryptSeqReset_I},
	//0x5a
	/* NA_RxTimeBCrc1Mode */
	{(NA_RxTimeBCrc1Mode_O >> 8),           (NA_RxTimeBCrc1Mode_O & 0xff),              (0x01 << NA_RxTimeBCrc1Mode_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxTimeBCrc1Mode_I},
	/* NA_RxCrc2Mode */
	{(NA_RxCrc2Mode_O >> 8),                (NA_RxCrc2Mode_O & 0xff),                   (0x01 << NA_RxCrc2Mode_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxCrc2Mode_I},
	/* NA_RxArqMode */
	{(NA_RxArqMode_O >> 8),                 (NA_RxArqMode_O & 0xff),                    (0x03 << NA_RxArqMode_LSB),                     NA_RxArqMode_LSB,           NTRX_WO_REG,    NTRX_TC_REG,    NA_RxArqMode_I},
	/* NA_RxAddrSegEsMode */
	{(NA_RxAddrSegEsMode_O >> 8),           (NA_RxAddrSegEsMode_O & 0xff),              (0x01 << NA_RxAddrSegEsMode_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxAddrSegEsMode_I},
	/* NA_RxAddrSegIsMode */
	{(NA_RxAddrSegIsMode_O >> 8),           (NA_RxAddrSegIsMode_O & 0xff),              (0x01 << NA_RxAddrSegIsMode_B),                 BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxAddrSegIsMode_I},
	/* NA_RxAddrSegDevIdL */
	{(NA_RxAddrSegDevIdL_O >> 8),           (NA_RxAddrSegDevIdL_O & 0xff),              (0x03 << NA_RxAddrSegDevIdL_LSB),               NA_RxAddrSegDevIdL_LSB,     NTRX_WO_REG,    NTRX_TC_REG,    NA_RxAddrSegDevIdL_I},
	//0x5b
	/* NA_RxDataEn */
	{(NA_RxDataEn_O >> 8),                  (NA_RxDataEn_O & 0xff),                     (0x01 << NA_RxDataEn_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxDataEn_I},
	/* NA_RxBrdcastEn */
	{(NA_RxBrdcastEn_O >> 8),               (NA_RxBrdcastEn_O & 0xff),                  (0x01 << NA_RxBrdcastEn_B),                     BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxBrdcastEn_I},
	/* NA_RxTimeBEn */
	{(NA_RxTimeBEn_O >> 8),                 (NA_RxTimeBEn_O & 0xff),                    (0x01 << NA_RxTimeBEn_B),                       BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxTimeBEn_I},
	/* NA_RxAddrMode */ /* 200 */
	{(NA_RxAddrMode_O >> 8),                (NA_RxAddrMode_O & 0xff),                   (0x01 << NA_RxAddrMode_B),                      BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RxAddrMode_I},
	/* NA_RangingPulses */
	{(NA_RangingPulses_O >> 8),             (NA_RangingPulses_O & 0xff),                (0x0f << NA_RangingPulses_LSB),                 NA_RangingPulses_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_RangingPulses_I},
	//0x5c
	/* NA_PulseDetDelay */
	{(NA_PulseDetDelay_O >> 8),             (NA_PulseDetDelay_O & 0xff),                (0x1f << NA_PulseDetDelay_LSB),                 NA_PulseDetDelay_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_PulseDetDelay_I},
	//0x5d
	/* NA_GateAdjThreshold */
	{(NA_GateAdjThreshold_O >> 8),          (NA_GateAdjThreshold_O & 0xff),             (0x07 << NA_GateAdjThreshold_LSB),              NA_GateAdjThreshold_LSB,    NTRX_WO_REG,    NTRX_TC_REG,    NA_GateAdjThreshold_I},
	/* NA_DownPulseDetectDis */
	{(NA_DownPulseDetectDis_O >> 8),        (NA_DownPulseDetectDis_O & 0xff),           (0x01 << NA_DownPulseDetectDis_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RangingPulses_I},
	/* NA_UpPulseDetectDis */
	{(NA_UpPulseDetectDis_O >> 8),          (NA_UpPulseDetectDis_O & 0xff),             (0x01 << NA_UpPulseDetectDis_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UpPulseDetectDis_I},
	//0x5e
	/* NA_GateSizeUnsync */
	{(NA_GateSizeUnsync_O >> 8),            (NA_GateSizeUnsync_O & 0xff),               (0x03 << NA_GateSizeUnsync_LSB),                NA_GateSizeUnsync_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_GateSizeUnsync_I},
	/* NA_GateSizeBitsync */
	{(NA_GateSizeBitsync_O >> 8),           (NA_GateSizeBitsync_O & 0xff),              (0x03 << NA_GateSizeBitsync_LSB),               NA_GateSizeBitsync_LSB,     NTRX_WO_REG,    NTRX_TC_REG,    NA_GateSizeBitsync_I},
	/* NA_GateSizeFramesync */
	{(NA_GateSizeFramesync_O >> 8),         (NA_GateSizeFramesync_O & 0xff),            (0x03 << NA_GateSizeFramesync_LSB),             NA_GateSizeFramesync_LSB,   NTRX_WO_REG,    NTRX_TC_REG,    NA_GateSizeFramesync_I},
	/* NA_GateAdjBitsyncEn */
	{(NA_GateAdjBitsyncEn_O >> 8),          (NA_GateAdjBitsyncEn_O & 0xff),             (0x01 << NA_GateAdjBitsyncEn_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_GateAdjBitsyncEn_I},
	/* NA_GateAdjFramesyncEn */ /* 210 */
	{(NA_GateAdjFramesyncEn_O >> 8),        (NA_GateAdjFramesyncEn_O & 0xff),           (0x01 << NA_GateAdjFramesyncEn_B),              BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_GateAdjFramesyncEn_I},
	// Not needed for 22Mhz default start
	//0x5f
	/* NA_Go2BitsyncThreshold */
	{(NA_Go2BitsyncThreshold_O >> 8),       (NA_Go2BitsyncThreshold_O & 0xff),          (0x07 << NA_Go2BitsyncThreshold_LSB),           NA_Go2BitsyncThreshold_LSB, NTRX_WO_REG,    NTRX_TC_REG,    NA_Go2BitsyncThreshold_I},
	/* NA_LeaveBitsyncThreshold */
	{(NA_LeaveBitsyncThreshold_O >> 8),     (NA_LeaveBitsyncThreshold_O & 0xff),        (0x07 << NA_LeaveBitsyncThreshold_LSB),         NA_LeaveBitsyncThreshold_LSB,NTRX_WO_REG,   NTRX_TC_REG,    NA_LeaveBitsyncThreshold_I},
	//0x60
	/* NA_RtcTimeBTxAdj */
	{(NA_RtcTimeBTxAdj_O >> 8),             (NA_RtcTimeBTxAdj_O & 0xff),                (0xff << NA_RtcTimeBTxAdj_LSB),                 NA_RtcTimeBTxAdj_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_RtcTimeBTxAdj_I},
	//0x61
	/* NA_RtcTimeBRxAdj */
	{(NA_RtcTimeBRxAdj_O >> 8),             (NA_RtcTimeBRxAdj_O & 0xff),                (0xff << NA_RtcTimeBRxAdj_LSB),                 NA_RtcTimeBRxAdj_LSB,       NTRX_WO_REG,    NTRX_TC_REG,    NA_RtcTimeBRxAdj_I},
	//0x62
	/* NA_RtcCmdWr */
	{(NA_RtcCmdWr_O >> 8),                  (NA_RtcCmdWr_O & 0xff),                     (0x01 << NA_RtcCmdWr_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RtcCmdWr_I},
	/* NA_RtcCmdRd */
	{(NA_RtcCmdRd_O >> 8),                  (NA_RtcCmdRd_O & 0xff),                     (0x01 << NA_RtcCmdRd_B),                        BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RtcCmdRd_I},
	/* NA_RtcTimeBAutoMode */
	{(NA_RtcTimeBAutoMode_O >> 8),          (NA_RtcTimeBAutoMode_O & 0xff),             (0x01 << NA_RtcTimeBAutoMode_B),                BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_RtcTimeBAutoMode_I},
	//0x63
	/* NA_AgcAmplitude */
	{(NA_AgcAmplitude_O >> 8),              (NA_AgcAmplitude_O & 0xff),                 (0xff << NA_AgcAmplitude_LSB),                  NA_AgcAmplitude_LSB,        NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcAmplitude_I},
	//0x64
	/* NA_AgcRangeOffset */
	{(NA_AgcRangeOffset_O >> 8),            (NA_AgcRangeOffset_O & 0xff),               (0xff << NA_AgcRangeOffset_LSB),                NA_AgcRangeOffset_LSB,      NTRX_WO_REG,    NTRX_TC_REG,    NA_AgcRangeOffset_I},
	/* NA_UseAlternativeAgc */
	{(NA_UseAlternativeAgc_O >> 8),         (NA_UseAlternativeAgc_O & 0xff),            (0x01 << NA_UseAlternativeAgc_B),               BOOL_OP,                    NTRX_WO_REG,    NTRX_TC_REG,    NA_UseAlternativeAgc_I},
	//0x7f
	/* NA_RamTxLength */
	{(NA_RamTxLength_O >> 8),               (NA_RamTxLength_O & 0xff),                  2,                                              0,                          NTRX_RW_REG,    NTRX_TC_REG,    0},

	/* NA_RamStaAddr0 */
	{(NA_RamStaAddr0_O >> 8),               (NA_RamStaAddr0_O & 0xff),                  6,                                              0,                          NTRX_RW_REG,    NTRX_TC_REG,    0},
	/* NA_SyncWord */
	{(NA_SyncWord_O >> 8),                  (NA_SyncWord_O & 0xff),                     8,                                              0,                          NTRX_RW_REG,    NTRX_TC_REG,    0},
	/* NA_ToaOffsetMeanData */
	{(NA_ToaOffsetMeanData_O >> 8),         (NA_ToaOffsetMeanData_O & 0xff),            2,                                              0,                          NTRX_RO_REG,    NTRX_TC_REG,       0},
	/* NA_TxRespTime */
	{(NA_TxRespTime_O >> 8),                (NA_TxRespTime_O & 0xff),                   2,                                              0,                          NTRX_RO_REG,    NTRX_TC_REG,       0},
	/* NA_ToaOffsetMeanAck */
	{(NA_ToaOffsetMeanAck_O >> 8),          (NA_ToaOffsetMeanAck_O & 0xff),             2,                                              0,                          NTRX_RO_REG,    NTRX_TC_REG,       0},
	{0,1,1,2,3,5,0}//{0,1,1,2,3,5}
};

/**
 * NTRXCheckTable:
 * 
 * NTRXCheckTable() is a testfunction to check consistency between the
 * register function table regCmd and the enum type NTRXCmdTE.
 * In case of a mismatch it is recommended to terminate the application
 * and fix the problem because it is a compile time problem and not
 * a runtime problem
 */
MyBoolT NTRXCheckTable (void)
{
	if ((ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].addrHi) == 0)
			&& (ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].addrLo) == 1)
			&& (ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].mask) == 1)
			&& (ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].operation) == 2)
			&& (ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].shadow) == 3)
			&& (ReadRomByte(regCmd[TRX_CHIRP_GEN_DATA].regId) == 5))
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * NTRXSetRegister:
 *
 * @cmd: -input- register address to be modified
 * @value: -input- new register setting to be written
 *
 * NTRXSetRegister() is a general purpose function for setting nanoNET TRX chip registers.
 * This function is aware of partialy used registers (bits or bit fields) and will properly
 * modify only those parts of the register. This function can only set 8 bit registers.
 *
 * Returns: none
 *
 */
void NTRXSetRegister (NTRXCmdTE cmd, MyByte8T value)
{
	static MyByte8T reg;

	/*
	 * get the register offset from the table
	 */
	if  ((ReadRomByte(regCmd[cmd].shadow) == NTRX_RW_REG)
			|| (ReadRomByte(regCmd[cmd].shadow) == NTRX_WO_REG))
	{

		reg = ntrxShadowReg[ReadRomByte(regCmd[cmd].addrLo)];
	}
	else
	{
		reg = 0x00;
	}

	/*
	 * if it is a single bit value use the mask
	 */
	if (ReadRomByte(regCmd[cmd].operation) == BOOL_OP) /* TRUE/FALSE value */
	{
		if (value == TRUE)
		{
			value = ReadRomByte(regCmd[cmd].mask);
		}
	}
	else if (value != 0)
	{
		/*
		 * for bit fields we have to shift the value according to the table
		 */
		value <<= ReadRomByte(regCmd[cmd].operation);
	}

	/*
	 * first reset the corresponding bits and then set only the new value
	 */
	if (ReadRomByte(regCmd[cmd].mask) != BOOL_OP)
	{
		reg &= ~(ReadRomByte(regCmd[cmd].mask));
		reg |= (ReadRomByte(regCmd[cmd].mask) & value);
	}
	else
	{
		reg = value;
	}

	/*
	 * if it is a read/write register store the new register value in
	 * the shadow register
	 */
	if ((ReadRomByte(regCmd[cmd].shadow) == NTRX_RW_REG)
			|| (ReadRomByte(regCmd[cmd].shadow) == NTRX_WO_REG))
	{
		ntrxShadowReg[ReadRomByte(regCmd[cmd].addrLo)] = reg;
	}

	NTRXSetRamIndex (ReadRomByte(regCmd[cmd].addrHi));

	NTRXWriteSingleSPI (ReadRomByte(regCmd[cmd].addrLo), reg);
}

/**
 * NTRXGetRegister:
 *
 * @cmd: -input- register address to be read
 * @reg: -input- new register setting to be read
 *
 * NTRXGetRegister() is a general purpose function for reading nanoNET TRX chip registers.
 * This function is aware of partialy used registers (bits or bit fields) and will read out
 * only valid bits and align them. This function is optimized for reading 8 bit registers.
 *
 * Returns: none
 *
 */
void NTRXGetRegister (NTRXCmdTE cmd, MyByte8T *reg)
{
	NTRXSetRamIndex (ReadRomByte(regCmd[cmd].addrHi));

	if (ReadRomByte(regCmd[cmd].shadow) == NTRX_WO_REG)
	{
		*reg = ntrxShadowReg[ReadRomByte(regCmd[cmd].addrLo)]; 	
	}
	else
	{
		NTRXReadSPI (ReadRomByte(regCmd[cmd].addrLo), reg, 1);
	}

	*reg &= ReadRomByte(regCmd[cmd].mask);

	if (ReadRomByte(regCmd[cmd].operation) == BOOL_OP) /* TRUE/FALSE value */
	{
		if (*reg != FALSE)
		{
			*reg = TRUE;
		}
	}
	else
	{
		*reg >>= ReadRomByte(regCmd[cmd].operation);
	}
}

/**
 * NTRXSetNRegister:
 *
 * @cmd: -input- register address to be modified
 * @data: -input- pointer to new register setting to be written
 *
 * NTRXSetRegister() is a general purpose function for setting nanoNET TRX chip registers.
 * This function doesn't use a mask therefore it will set full octets.
 *
 * Returns: none
 *
 */
void NTRXSetNRegister (NTRXCmdTE cmd, MyByte8T *data)
{
	NTRXSetRamIndex (ReadRomByte(regCmd[cmd].addrHi));

	NTRXWriteSPI (ReadRomByte(regCmd[cmd].addrLo), data, ReadRomByte(regCmd[cmd].mask));
}

/**
 * NTRXGetNRegister:
 *
 * @cmd: -input- register address to be modified
 * @reg: -input- pointer to memory for return value
 *
 * NTRXGetNRegister() is a general purpose function for reading nanoNET TRX chip registers.
 * This function doesn't use a mask therefore it will set full octets.
 *
 * Returns: none
 *
 */
void NTRXGetNRegister (NTRXCmdTE cmd, MyByte8T *reg)
{
	NTRXSetRamIndex (ReadRomByte(regCmd[cmd].addrHi));

	NTRXReadSPI (ReadRomByte(regCmd[cmd].addrLo), reg, ReadRomByte(regCmd[cmd].mask));
}

/**
 * NTRXProcessTable:
 *
 * @table: -input- table of register offsets and values to be processed
 *
 * NTRXProcessTable() is a general purpose function for setting nanoNET TRX chip registers.
 * This function processes a list of commands. The number of entries is variable.
 * The list end is recognized by an illegal command.
 *
 * Returns: none
 *
 */
void NTRXProcessTable (const MyByte8T *table)
{
	short index_ = 0;
	short timer;
	MyByte8T data[2];
	short i;

	while (ReadRomByte (table[index_]) < CMD_ERROR)
	{
		switch (ReadRomByte (table[index_]))
		{
			case TRX_DELAY:         timer = ReadRomByte (table[index_ + 1]);
															hwdelay (timer * 1000);
															break;

			case NA_LoTargetValue:
			case NA_AgcIntTime:
															for (i = 0; i < 2; i++)
															{
																data[i] = ReadRomByte (table[index_ + 1 + i]);
															}
															NTRXSetNRegister (ReadRomByte (table[index_]), data);
															index_++;
															break;

			case NA_RamTxLength:
															data[0] = 0x00;
															ntrxShadowReg[NA_RamIndex_O] = data[0];
															NTRXWriteSingleSPI (NA_RamIndex_O, data[0]);
															for (i = 0; i < 2; i++)
															{
																data[i] = ReadRomByte (table[index_ + 1 + i]);
															}
															NTRXSetNRegister (ReadRomByte (table[index_]), data);
															index_++;
															break;

			default:                NTRXSetRegister (ReadRomByte (table[index_]),
																	ReadRomByte (table[index_ + 1]));
															break;
		}
		index_ += 2;
	}
}

void NTRXProcessSequence (const MyByte8T *table)
{
	MyByte8T i;
	MyByte8T payload[64];

	payload[0] = table[0];
	NTRXWriteSingleSPI (0x0e, payload[0]);
	ntrxShadowReg[0x0e] = table[0];

	for (i = 0; i < table[2]; i++)
	{
		payload[i] = table[3 + i];
	}
	NTRXWriteSPI (table[1], payload, table[2]);
}

/**
 * NTRXCheckVerRev:
 *
 * NTRXCheckVerRev() compares VerRev from the nanoNET TRX chip with the
 * expected Version and Revision number from the software.
 *
 * Returns: TRUE if equal, False if not equal or SPI access failed
 *
 */
MyBoolT NTRXCheckVerRev (void)
{
	MyByte8T buff[2] = { 0x00, 0x00 };

	NTRXReadSPI (NA_Version_O, buff, 1);
	NTRXReadSPI (NA_Revision_O, &(buff[1]), 1);

	//disp_led(buff[0]);
	//disp_led(buff[1]);

#   if ((CONFIG_PRINTF) && (CONFIG_LOGO))
	printf("ver:%d, rev:%d\n", buff[0], buff[1]);
#   endif

	if (buff[0] == NA_Version_I && buff[1] == NA_Revision_I)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * NTRXGetTxARQmax:
 *
 * NTRXGetTxARQmax() reads the threshold value for packet retransmissions.
 *
 * Returns: tx arq maximum value in the range of (0x0...0xE).
 */

MyByte8T NTRXGetTxARQmax (void)
{
	//    return (ntrxShadowReg[NA_TxArqMax_O] & 0x0f);
	return ((ntrxShadowReg[NA_TxArqMax_O] >> 4) & 0x0f);
}

/**
 * NTRXGetRxCRC2mode:
 *
 * NTRXGetRxCRC2mode() reads out the setting for CRC-2 mode in the nanoNET TRX
 *
 * Returns: bool On (TRUE) or Off (FALSE)
 */
MyBoolT NTRXGetRxCRC2mode (void)
{
	return ((ntrxShadowReg[NA_RxCrc2Mode_O] & (1<< NA_RxCrc2Mode_B)) >> NA_RxCrc2Mode_B);
}

/**
 * NTRXGetRfOutputPower:
 *
 * @type: -input- enum the type of frame the output power to be get for
 *
 * NTRXGetRfOutputPower() reads output power either for the frame types Data,
 * Broadcast and TimeB, or for the frame types Ack, Request-to-send and
 * Clear-to-send.
 *
 * Returns:  the value of the output power (0 ... 0x3F)
 */
MyByte8T NTRXGetRfOutputPower (MyByte8T type)
{
	MyByte8T offset;

	switch (type)
	{
		case pwrOutData:
			offset = NA_TxOutputPower0_O;
			break;
		case pwrOutReqst:
			offset = NA_TxOutputPower1_O;
			break;
		default:
			return 0;
	}
	return 0x3F & ntrxShadowReg[offset];
}

static const MyByte8T NTRXPwrOutLat[] =
{
	0,  0,  0,  0,  0,
	1,  1,  1,  1,  1,  1,
	2,  2,  2,  2,  2,  2,  2,
	3,  3,  3,  3,  3,  3,  3,
	4,  4,  4,  4,  4,  4,
	5,  5,  5,  5,  5,  5,
	21, 21, 21, 21, 21,
	22, 22, 22, 22,
	23, 23, 23,
	39, 39, 39,
	40, 40,
	41, 41,
	57, 57,
	58,
	59,
	60,
	61,
	62,
	63
};

/**
 * NTRXGetPowerEntry:
 *
 * @value: -input- enum the type of frame the output power to be get for
 *
 * NTRXGetPowerEntry() reads output the power either for the frame types Data,
 * Broadcast and TimeB, or for the frame types Ack, Request-to-send and
 * Clear-to-send.
 *
 * Returns: the value of the output power (0 ... 0x3F)
 */
MyByte8T NTRXGetPowerEntry (MyByte8T value)
{
	return NTRXPwrOutLat[0x3f & value];
}


void NTRXFctCal (void)
{
	int capsVal = 7;
	int targetValMax = 174;
	int targetValMin = 152;
	int fctSum = 0;
	int ctref;
	unsigned char payload;
	// unsigned long t;

	payload = ( 1 << NA_FctClockEn_B ) | capsVal;
	NTRXWriteSingleSPI (NA_ChirpFilterCaps_O, payload);
	hwdelay (10); /* 10 us */

	while ((capsVal < 16) && (capsVal >= 0))
	{
		fctSum = 0;
		for (ctref = 0; ctref < 4; ctref++)
		{
			payload = ( 1 << NA_FctClockEn_B ) | ( 1 << NA_StartFctMeasure_B ) | capsVal;
			NTRXWriteSingleSPI (NA_ChirpFilterCaps_O, payload);
			hwdelay (15); /* 10 us */

			NTRXReadSingleSPI (NA_FctPeriod_O, &payload);
			fctSum += payload;
		}
		if (fctSum >= targetValMax)
		{
			capsVal--;
		}
		else if (fctSum <= targetValMin)
		{
			capsVal++;
		}
		else
		{
			payload = capsVal;
			NTRXWriteSingleSPI (NA_ChirpFilterCaps_O, payload);
			return;
		}
	}
	payload = 6;
	NTRXWriteSingleSPI (NA_ChirpFilterCaps_O, payload);
}

void NTRXSetSyncWord (MyByte8T *value)
{
	NTRXSetNRegister (NA_SyncWord, value);
}


#ifdef CONFIG_22MHZ_DEF_ONLY
const MyByte8T NTRXInitTable1[] PROGMEM =
{
	NA_RxCmdStop, TRUE,
	NA_RxIntsReset, 0xff,
	NA_RxBufferCmd, 3,

	NA_ResetBbRadioCtrl, TRUE,
	NA_ResetBbRadioCtrl, FALSE,
	NA_EnableBbClock, FALSE,
	NA_ResetBbClockGate, TRUE,
	NA_EnableBbCrystal, FALSE,
	NA_ResetBbClockGate, TRUE,
	NA_EnableBbCrystal, TRUE,
	TRX_DELAY, 5,
	NA_ResetBbClockGate, FALSE,
	NA_EnableBbClock, TRUE,
	NA_ResetBbRadioCtrl, TRUE,
	NA_ResetBbRadioCtrl, FALSE,
	NA_GateAdjFramesyncEn, FALSE,
	NA_GateAdjBitsyncEn, FALSE,

	/* Set AGC Values */
	NA_AgcHold, TRUE,
	NA_AgcDefaultEn, TRUE,
	NA_AgcValue, 35,
	NA_PulseDetDelay, 2,
	NA_GateSizeFramesync, NA_GateSize9Slots_VC_C,

	NA_UseFec, TRUE,
	CMD_ERROR
};

const unsigned char NAInitTxTable2tx[] PROGMEM =
{
	//    NA_TxArqMax, NA_MAX_ARQ_CNT,					/* def 14 */
	NA_EnableExtPA, TRUE,
	NA_CsqUsePhaseShift, FALSE,
	CMD_ERROR
};

void NTRXSetupTRxMode (MyByte8T fdma, MyByte8T sd, MyByte8T sr)
{
	//	printf ("Calling Setup\n");
	settingVal.bw = fdma;
	settingVal.sd = sd;
	settingVal.sr = sr;
	settingVal.fdma = TRUE;
	settingVal.fixnmap = NA_FIX_MODE;
	curCh = 1;

	NTRXSetIndexReg (0x00);
	/* first part of initial register setting */
	NTRXProcessTable (NTRXInitTable1);

	//    NTRXSetRxIQMatrix2 ();
	NTRXSetIndexReg (0x00);

	NTRXSetCorrThreshold2 ();

	/* Tx Part */
	NTRXProcessTable (NAInitTxTable2tx);
	//    NTRXSetTxIQMatrix2 ();
	NTRXSetIndexReg (0x00);

	NTRXFctCal ();

	bRxCrc2Mode=TRUE;
#   ifdef CONFIG_NTRX_AUTO_RECALIB
	NTRXAllCalibration ();
#	endif /* CONFIG_NTRX_AUTO_RECALIB */
	NTRXRestart ();
}
#else
const MyByte8T NTRXInitTable1[] PROGMEM =
{
	NA_ResetBbClockGate, TRUE,
	NA_EnableBbCrystal, TRUE,
	TRX_DELAY, 5,
	NA_ResetBbClockGate, FALSE,
	NA_EnableBbClock, TRUE,
	NA_ResetBbRadioCtrl, TRUE,
	NA_ResetBbRadioCtrl, FALSE,
	CMD_ERROR
};

const MyByte8T NTRXInitTable2rx[] PROGMEM =
{
	//  NA_RxArqMode, NA_RxArqModeNone_VC_C,
	NA_RxArqMode, NA_RxArqModeCrc2_VC_C,
	//  NA_RxCrc2Mode, FALSE,
	NA_RxTimeBCrc1Mode, FALSE, 
	NA_RxCrc2Mode, TRUE,
	NA_RfRxCompValueI, 14,
	NA_RfRxCompValueQ, 14,
	CMD_ERROR
};

const MyByte8T NTRXInitTable3rx[] PROGMEM =
{
	NA_DeviceSelect, NA_DeviceSelectBbRam0_C,
	NA_HoldAgcInFrameSync, TRUE,
	NA_HoldAgcInBitSync, 24,
	NA_UseAlternativeAgc, FALSE, /* (SAP) */
	NA_ChirpFilterCaps, 0,
	NA_GateAdjFramesyncEn, FALSE,
	NA_GateAdjBitsyncEn, FALSE,
	CMD_ERROR
};

const MyByte8T NTRXInitTable4[] PROGMEM =
{
	NA_DeviceSelect, NA_DeviceSelectBbRam0_C,
	TRX_DELAY, 2,
	CMD_ERROR
};
const unsigned char NAInitTxTable2tx[] PROGMEM =
{
	NA_TxArq, TRUE,
	NA_TxArqMax, NA_MAX_ARQ_CNT,
	NA_TxScrambEn, TRUE,
	NA_EnableExtPA, TRUE,
	NA_RxAddrMode, TRUE,
	NA_CsqUsePhaseShift, FALSE,
	NA_EnableCsqClock, TRUE,
	NA_CsqUseRam,  TRUE,
	NA_CsqAsyMode, FALSE,
	NA_TxOutputPower0, RfTxOutputPowerData_DEFAULT,
	NA_TxOutputPower1, RfTxOutputPowerReq_DEFAULT,
	/* if needed
		 NA_ChirpMatrix0, NA_ChirpDown_VC_C,
		 NA_ChirpMatrix1, NA_ChirpUp_VC_C,
		 NA_ChirpMatrix2, NA_ChirpFoldPlus_VC_C,
		 NA_ChirpMatrix3, NA_ChirpOff_VC_C,
		 NA_TxPreTrailMatrix0, 0,
		 NA_TxPreTrailMatrix1, 1,
		 NA_ModulationSystem, NA_ModulationSystem2ary_BC_C
	 */
	CMD_ERROR
};

const unsigned char NAInitTable3tx[] PROGMEM =
{
	NA_EnableLO, FALSE,
	NA_EnableLOdiv10, FALSE,
	NA_TxAddrSlct, 0,
	CMD_ERROR
};

void NTRXSetupTRxMode (MyByte8T fdma, MyByte8T sd, MyByte8T sr)
{
	settingVal.bw = fdma;
	settingVal.sd = sd;
	settingVal.sr = sr;
	if (fdma == NA_22MHz)
	{
		curCh = 1;
		settingVal.fdma= TRUE;
		settingVal.fixnmap=NA_FIX_MODE;
	}
	else
	{
		curCh = 0;
		settingVal.fdma= FALSE;
		settingVal.fixnmap=NA_FIX_MODE;
	}

	if (firstTime++ != 0)
	{	
		NTRXSetIndexReg (0x00);
		NTRXSetRegister (NA_RxCmdStop, TRUE);
		NTRXSetRegister (NA_RxIntsReset, 0xff);
		NTRXSetRegister (NA_RxBufferCmd, 3);

		NTRXSetRegister (NA_ResetBbRadioCtrl, TRUE);
		NTRXSetRegister (NA_ResetBbRadioCtrl, FALSE);
		NTRXSetRegister (NA_EnableBbClock, FALSE);
		NTRXSetRegister (NA_ResetBbClockGate, TRUE);
		NTRXSetRegister (NA_EnableBbCrystal, FALSE);
	}


	/* first part of initial register setting */
	NTRXProcessTable (NTRXInitTable1);
	//NTRXSetAgcValues (fdma, sd, sr);
	NTRXProcessTable (NTRXInitTable2rx);
	NTRXSetRxIQMatrix (fdma, sd);
	NTRXProcessTable (NTRXInitTable3rx);
	NTRXSetCorrThreshold (fdma, sd);
	NTRXProcessTable (NTRXInitTable4);

	/* Tx Part */
	NTRXProcessTable (NAInitTxTable2tx);
	NTRXSetTxIQMatrix (fdma, sd);
	if (fdma != NA_22MHz_HR) 
	{
		NTRXFctCal ();
	} else 
	{ /* 22MHz_over_80MHz ranging */
		unsigned char ctcaps = 15;
		NTRXWriteSingleSPI (NA_ChirpFilterCaps_O, ctcaps);
	}
	NTRXProcessTable (NAInitTable3tx);

	bRxCrc2Mode=TRUE;
	NTRXAllCalibration ();
	NTRXSetAgcValues (fdma, sd, sr);
	NTRXSetRegister (NA_UseFec, TRUE);
	NTRXRestart ();
}
#endif /* CONFIG_22MHZ_DEF_ONLY */

void NTRXSetTransparentMode( MyBoolT on )
{
	MyByte8T len[2];

	if( on == TRUE )
	{
		len[0] = 0x80;
		len[1] = 0;
		NTRXWriteSPI( NA_RxTransBytes_O, len, 2 );

		NTRXSetRegister( NA_TxRxMode, TRUE );
		NTRXSetRegister( NA_TxRxBbBufferMode0, TRUE );
		NTRXSetRegister( NA_TxScrambEn, FALSE );
		NTRXSetRegister( NA_TxScrambInit, 0 );
		NTRXSetRegister( NA_TxArq, FALSE );
		NTRXSetRegister( NA_TxArqMax, 0 );
		NTRXSetRegister( NA_RxArqMode, NA_RxArqModeNone_VC_C );
		NTRXSetRegister( NA_RxCrc2Mode, FALSE );
	}
	else
	{
		NTRXSetRegister( NA_TxRxMode, FALSE );
		NTRXSetRegister( NA_TxScrambEn, TRUE );
		NTRXSetRegister( NA_TxScrambInit, 0x7f );
		NTRXSetRegister( NA_TxArq, TRUE );
		NTRXSetRegister( NA_TxArqMax, NA_MAX_ARQ_CNT );

		NTRXSetRegister( NA_RxArqMode, NA_RxArqModeCrc2_VC_C );
		NTRXSetRegister( NA_RxCrc2Mode, TRUE );
	}
}

MyDword32T NTRXGetRTC (MyBoolT force)
{
	/*FIXME disabled for debugging!*/
	static MyDword32T currentRTC;					/**< stores the last RTC value*/
	static MyDword32T currentHwclock = 0xFFFFFFFF;	/**< stores the hwclock value when RTC was read*/
	MyDword32T now = hwclock();

	/*
		 Avoid redundant access to RTC to save time.
		 force == TRUE lets the function read the RTC anyway.
	 */
	if ((now != currentHwclock) || (force)){
		MyByte8T rtc[5];
		MyByte8T reg;


		/*read current RTC*/
		ENTER_TASK;
		reg = 1<<NA_RtcCmdRd_B;
		NTRXWriteSingleSPI(NA_RtcCmdRd_O, reg);
		hwdelay(130);
		NTRXReadSPI(NA_RamRtcReg_O + 1, rtc, 5);
		reg = 0;
		NTRXWriteSingleSPI(NA_RtcCmdRd_O, reg);

		/*update current RTC and hwclock entries*/
		currentRTC = (
				(((MyDword32T)rtc[0])   <<  0)
				| (((MyDword32T)rtc[1]) <<  8)
				| (((MyDword32T)rtc[2]) << 16)
				| (((MyDword32T)rtc[3]) << 24)
				);
		/*FIXME what can we do with the most significant byte?!*/
		//currentRTC = (currentRTC >> 7) | (rtc[5] << 1);
		currentHwclock = now;
		LEAVE_TASK;
	}
	return currentRTC;
}

/**
 * NTRXPowerdownMode:
 *
 * @mode: -input- mode 0 is full and 1 is for pad mode
 * @seconds: -input- number of seconds to stay in powerdown mode 0 means to timelimit
 * NTRXPowerdownMode() sets the RTC alarm time to the requested time in seconds
 * and puts the transceiver into the requested powerdown mode.
 * Clear-to-send.
 *
 * Returns: none
 */
void NTRXPowerdownMode (MyByte8T mode, MyDword32T seconds)
{
	MyByte8T 	value, valNull = 0;
	MyByte8T 	i;
	MyByte8T 	wakeupTime8;
	MyDword32T wakeupTime32;

	/*Stop receiver*/
	NTRXSetRegister (NA_RxCmdStop, TRUE);

	if (seconds > 0)
	{
		wakeupTime32 = (seconds << 7) + NTRXGetRTC(1);

		/*write down wake up time*/
		for (i = NA_WakeUpTimeWe_LSB; i <= NA_WakeUpTimeWe_MSB; i++)
		{

			/*convert 32bit integer wakeup time -> RTC value*/
			wakeupTime8 = (MyByte8T)(wakeupTime32 & 0xFF);
			wakeupTime32 >>= 8;

			/*write one of the 3 bytes into the RtcWakeUpTime Register*/
			NTRXWriteSingleSPI( NA_WakeUpTimeByte_O, wakeupTime8 );

			/*generate Strobe to store value in the right place*/
			value = 1 << (i);
			NTRXWriteSingleSPI( NA_WakeUpTimeWe_O, value );
			NTRXWriteSingleSPI( NA_WakeUpTimeWe_O, valNull );
		}
	}

	value = ( 0x00 | ( 1 << NA_ResetBbClockGate_B ));
	NTRXWriteSingleSPI( NA_PowerDown_O, value );
	value = 0;
	NTRXWriteSingleSPI( 0x8, value );
	/*set wakeup parameter*/
	if (seconds > 0)
	{
		value = (0x00
				| ( 1 << NA_EnableWakeUpRtc_B )
				| ( 1 << NA_PowerUpTime_LSB )
				| (( 0x01 & mode ) << NA_PowerDownMode_B ));
	}
	else
	{
		value = (( 0x01 & mode ) << NA_PowerDownMode_B );
	}	
	NTRXWriteSingleSPI( NA_EnableWakeUpRtc_O, value );

	value =(0x00
			| ( 1 << NA_PowerDown_B )
			| ( 1 << NA_ResetBbClockGate_B ));
	NTRXWriteSingleSPI( NA_PowerDown_O, value );
}

/**
 * Starts a countdown from startvalue to zero ( in us )
 */
void NTRXStartBasebandTimer (MyInt16T startvalue)
{
	NTRXWriteSPI (NA_BasebandTimerStartValue_O, (MyByte8T *)(&startvalue), 2);
}

/**
 * Stops the baseband-counter
 */
void NTRXStopBasebandTimer (void)
{
	NTRXSetRegister (NA_ClearBasebandTimerInt, TRUE);
}

