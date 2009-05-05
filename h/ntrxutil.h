/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2006
 * Nanotron Technologies
 *
 * Author: S. Radtke
 *
 * Description :
 *    This file contains the definitions for utility functions for the
 *    NTRX driver
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef NTRXUTIL_H
#define NTRXUTIL_H

//#include <string.h>
#include "config.h"
#include "ntrxtypes.h"
#include    "OffstMapInit.h"

typedef struct Settings
{
        MyByte8T    bw;
        MyByte8T    sd;
        MyByte8T    sr;
        MyByte8T    fdma;
        MyByte8T    fixnmap;
	MyDouble32T    rangingConst;
}SettingsT;

extern SettingsT settingVal;


extern MyBoolT bRxCrc2Mode;
extern MyBoolT  ntrxRun;
extern MyByte8T ntrxIrqStatus[2];
extern MyAddressT ntrxMacAddr;
extern MyByte8T ntrxState;
extern MyByte8T ntrxCal;


#define SHADOWREGCOUNT        128
/* COldfire timer for 66Mhz */
#define DELAY_20us      hwdelay(40) /* 20 */
#define DELAY_100us     hwdelay(130)
#define DELAY_800us     hwdelay(3200) /* 1100 */
#define DELAY_20ms  { hwdelay(8000); hwdelay(8000); hwdelay (8000); hwdelay (3000); }
#define RfTxOutputPowerData_DEFAULT         0x3f
#define RfTxOutputPowerReq_DEFAULT          0x3f

extern MyByte8T ntrxShadowReg[SHADOWREGCOUNT];

#define TxIDLE      0x00
#define TxSEND      0x01
#define TxWAIT      0x02

#define NoCAL       0X00
#define TxCAL       0x01
#define RxCAL       0x02
#define AllCAL      0x03
#define NA_MAX_ARQ_CNT  0x0A

// #define RXENDIRQ         (1<<Silbadero_RxEnd_B)
// #define RXOFLOWIRQ       (1<<Silbadero_RxOverflow_B)

#ifdef CONFIG_PORTATION_ATMEGA128_BOARD
//#   include     <pgmspace.h>
#   define ReadRomByte(p) pgm_read_byte(&(p))
#       define ReadRomWord(p) pgm_read_word(&(p))
#else
#   define memcpy_P memcpy
#   define PROGMEM
#   define ReadRomByte(p) p
#       define ReadRomWord(p) p
#endif

#define txIrqStatus (ntrxIrqStatus[0])
#define rxIrqStatus (ntrxIrqStatus[1])

MyBoolT rcosc_adj (MyByte8T*);
#if (CONFIG_REG_MAP_NR <= 1097)
void pabc_adj (const MyByte8T*, const MyByte8T*);
#endif /* (CONFIG_REG_MAP_NR <= 1097) */
void NTRXTxLoAdjust (void);
void NTRXRxLoAdjust (void);
//void NTRXLoAdjust (void);
void SetRxStop (void);
void NTRXAllCalibration (void);
void NTRXSetStaAddress (MyByte8T *address);
void NTRXRxLoCalibration (void);
void NTRXTxLoCalibration (void);
MyBoolT NTRXCheckVerRev (void);
void NTRXInitShadowRegister (void);
void NTRXSetRfChirpGenData (void);
MyByte8T NTRXGetTxARQmax (void);
MyBoolT NTRXGetRxCRC2mode (void);
//void setTxDPushPull (MyBoolT OnOff);
void setRxIRQ (MyByte8T *pBitPattern);
#if defined (CONFIG_NTRX_GET_OUT_PWR) || defined (CONFIG_NTRX_STM_FULL)
MyByte8T NTRXGetRfOutputPower (MyByte8T type);
#endif /* CONFIG_NTRX_GET_OUT_PWR   */
#if defined (CONFIG_NTRX_GET_PWR_ENTRY) || defined (CONFIG_NTRX_STM_FULL)
MyByte8T NTRXGetPowerEntry (MyByte8T value);
#endif /* CONFIG_NTRX_GET_PWR_ENTRY */
void NTRXProcessTable (const MyByte8T *table);
void NTRXSetDataRate (MyByte8T value);
void NTRXSetStaAddress (MyByte8T *address);

void NTRXProcessSequence (const MyByte8T *table);
void NTRXFctCal (void);
void NTRX1MbitInit (void);
//void NTRXSPIInit (void);
MyBoolT NTRXCheckTable (void);
void NTRXSetChannel (MyByte8T value);
MyByte8T NTRXGetChannel (void);


typedef struct NTRXRegCmd
{
    MyByte8T addrHi;
    MyByte8T addrLo;
    MyByte8T mask;
    MyByte8T operation;
    MyByte8T shadow;
    MyByte8T regId;
    MyByte8T initValue;
} NTRXRegCmdT;

typedef enum
{
// 0x00
    NA_SpiBitOrder,
    NA_SpiTxDriver,
    NA_IrqPolarity,
    NA_IrqDriver,
//0x01
    NA_Version,
    NA_WakeUpTimeByte,
//0x02
    NA_Revision,
    NA_WakeUpTimeWe,
//0x03
    NA_BattMgmtEnable,
    NA_BattMgmtThreshold,
    NA_BattMgmtCompare, /* 10 */
//0x04
    NA_DioDirection,
    NA_DioOutValueAlarmEnable,
    NA_DioAlarmStart,
    NA_DioAlarmPolarity,
    NA_DioUsePullup,
    NA_DioUsePulldown,
    NA_DioInValueAlarmStatus,
//0x05
    NA_DioPortWe,
//0x06
    NA_EnableWakeUpRtc,
    NA_EnableWakeUpDio, /* 20 */
    NA_PowerUpTime,
    NA_PowerDownMode,
//0x07
    NA_PowerDown,
    NA_ResetBbClockGate,
    NA_ResetBbRadioCtrl,
    NA_UsePullup4Test,
    NA_UsePulldown4Test,
//0x08
    NA_EnableBbCrystal,
    NA_EnableBbClock,
    NA_BypassBbCrystal, /* 30 */
    NA_FeatureClockFreq,
    NA_EnableFeatureClock,
//0x09
    NA_UsePullup4Spiclk,
    NA_UsePulldown4Spiclk,
    NA_UsePullup4Spissn,
    NA_UsePulldown4Spissn,
    NA_UsePullup4Spirxd,
    NA_UsePulldown4Spirxd,
    NA_UsePullup4Spitxd,
    NA_UsePulldown4Spitxd, /* 40 */
//0x0a
    NA_UsePullup4Por,
    NA_UsePulldown4Por,
    NA_UsePullup4Pamp,
    NA_UsePulldown4Pamp,
    NA_UsePullup4Ucirq,
    NA_UsePulldown4Ucirq,
    NA_UsePullup4Ucrst,
    NA_UsePulldown4Ucrst,
//0x0b
    NA_WritePulls4Spi,
    NA_WritePulls4Pads, /* 50 */
//0x0e
    NA_RamIndex,
    NA_DeviceSelect,
//0x0f
    NA_TxIrqEnable,
    NA_RxIrqEnable,
    NA_BbTimerIrqEnable,
    NA_LoIrqEnable,
    NA_TxIrqStatus,
    NA_RxIrqStatus, /* 60 */
    NA_BbTimerIrqStatus,
    NA_LoIrqStatus,
//0x10
    NA_TxIntsRawStat,
    NA_TxIntsReset,
//0x11
    NA_RxIntsRawStat,
    NA_RxIntsReset,
//0x12
    NA_LoIntsRawStat,
    NA_LoIntsReset,
    NA_ClearBasebandTimerInt,
//0x13
    NA_TxIntsEn, /* 70 */
//0x14
    NA_RxIntsEn,
//0x15
    NA_LoIntsEn,
//0x1c
    NA_LoEnableFastTuning,
    NA_LoFastTuningLevel,
    NA_LoEnableLsbNeg,
    NA_UseLoRxCaps,
//0x1d
    NA_LoTargetValue,
//0x1f
    NA_AgcThresHold1,
//0x20
    NA_AgcThresHold2, /* 80 */
//0x21
    NA_HoldAgcInBitSync,
    NA_HoldAgcInFrameSync,
//0x22
    NA_AgcDeadTime,
    NA_AgcNregLength,
//0x23
    NA_AgcIntTime,
//0x25
    NA_AgcValue,
    NA_AgcDefaultEn,
    NA_AgcHold,
//0x26
    NA_AgcRssiThres,
    NA_AgcGain, /* 90 */
//0x27
    NA_ChirpFilterCaps,
    NA_FctClockEn,
    NA_StartFctMeasure,
    NA_EnableTx,
    NA_FctPeriod,
//0x2b
    NA_ToaOffsetMeanDataValid,
//0x2e
    NA_PhaseOffsetData,
    NA_PhaseOffsetAck,
//0x30
    NA_ToaOffsetMeanAckValid, /* 100 */
//0x31
    NA_RxPacketType,
    NA_RxAddrMatch,
    NA_RxCrc1Stat,
    NA_RxCrc2Stat,
//0x32
    NA_RxCorrBitErr,
    NA_RxCorrErrThres,
//0x33
    NA_RxAddrSegEsMatch,
    NA_RxAddrSegIsMatch,
    NA_RxCryptEn,
    NA_RxCryptId, /* 110 */
    NA_RxCryptSeqN,
//0x37
    NA_TxTimeSlotControl,
    NA_RxTimeSlotControl,
//0x3c
    NA_TxArqCnt,
    NA_TxArqMax,
//0x3d
    NA_CsqDitherValue,
    NA_CsqUsePhaseShift,
    NA_CsqUse4Phases,
    NA_CsqAsyMode,
    NA_CsqMemAddrInit, /* 120 */
    NA_CsqUseRam,
//0x3f
    NA_D3lFixnMap,
    NA_D3lPomEn,
    NA_D3lPomLen,
    NA_D3lUpDownEx,
//0x40
    NA_LeaveMapThresh1InBitsync, /* 130 */
    NA_UseMapThresh1InFramesync,
//0x41
    NA_Go2MapThresh1InBitsync,
//0x42
    NA_EnableLO,
    NA_EnableLOdiv10,
    NA_EnableCsqClock,
    NA_InvertRxClock,
    NA_EnableExtPA,
//0x43
    NA_LnaFreqAdjust,
    NA_TxPaBias,
//0x44
    NA_TxOutputPower0,
//0x45
    NA_TxOutputPower1,
//0x46
    NA_RfRxCompValueI,
//0x47
    NA_RfRxCompValueQ,
//0x48
    NA_SymbolDur,
    NA_SymbolRate,
    NA_ModulationSystem, /* 150 */
//0x49
    NA_Crc2Type,
    NA_UseFec,
    NA_TxRxCryptCrc2Mode,
    NA_TxRxCryptClkMode,
//0x4a
    NA_SwapBbBuffers,
    NA_TxRxBbBufferMode1,
    NA_TxRxBbBufferMode0,
    NA_FdmaEnable,
    NA_TxRxMode,
//0x4b
    NA_ChirpMatrix0, /* 160 */
    NA_ChirpMatrix1,
//0x4c
    NA_ChirpMatrix2,
    NA_ChirpMatrix3,
//0x4d
    NA_TxPreTrailMatrix0,
    NA_TxPreTrailMatrix1,
    NA_TxUnderrunIgnore,
    NA_TxMacCifsDis,
//0x4e
    NA_TxVCarrSens,
    NA_TxPhCarrSenseMode,
    NA_TxVCarrSensAck, /* 170 */
    NA_TxArq,
    NA_Tx3Way,
    NA_TxBackOffAlg,
    NA_TxFragPrio,
//0x4f
    NA_TxBackOffSeed,
//0x50
    NA_TxCryptSeqReset,
    NA_TxCryptEn,
    NA_TxCryptId,
    NA_TxCryptSeqN,
//0x51
    NA_TxScrambInit, /* 180 */
    NA_TxScrambEn,
//0x54
    NA_TxPacketType,
    NA_TxAddrSlct,
//0x55
    NA_TxCmdStop,
    NA_TxCmdStart,
    NA_TxBufferCmd,
//0x56
    NA_RxCmdStop,
    NA_RxCmdStart,
    NA_RxBufferCmd,
//0x57
    NA_RxCryptSeqReset, /* 190 */
//0x5a
    NA_RxTimeBCrc1Mode,
    NA_RxCrc2Mode,
    NA_RxArqMode,
    NA_RxAddrSegEsMode,
    NA_RxAddrSegIsMode,
    NA_RxAddrSegDevIdL,
//0x5b
    NA_RxDataEn,
    NA_RxBrdcastEn,
    NA_RxTimeBEn,
    NA_RxAddrMode, /* 200 */
    NA_RangingPulses,
//0x5c
    NA_PulseDetDelay,
//0x5d
    NA_GateAdjThreshold,
    NA_DownPulseDetectDis,
    NA_UpPulseDetectDis,
//0x5e
    NA_GateSizeUnsync,
    NA_GateSizeBitsync,
    NA_GateSizeFramesync,
    NA_GateAdjBitsyncEn,
    NA_GateAdjFramesyncEn, /* 210 */
//0x5f
    NA_Go2BitsyncThreshold,
    NA_LeaveBitsyncThreshold,
//0x60
    NA_RtcTimeBTxAdj,
//0x61
    NA_RtcTimeBRxAdj,
 //0x62
    NA_RtcCmdWr,
    NA_RtcCmdRd,
    NA_RtcTimeBAutoMode,
//0x63
    NA_AgcAmplitude,
//0x64
    NA_AgcRangeOffset,
    NA_UseAlternativeAgc,
//0x7f
    NA_RamTxLength,
    NA_RamStaAddr0,
    NA_SyncWord,
    NA_ToaOffsetMeanData,
    NA_TxRespTime,
    NA_ToaOffsetMeanAck,
    /* Psydo Regs */
    TRX_CHIRP_GEN_DATA,
    TRX_ADJ,
    TRX_DELAY,

    CMD_ERROR
} NTRXCmdTE;

enum FRMTYPE
{
    frmTypeData    = NA_TypeCodeData_VC_C,
    frmTypeAck     = NA_TypeCodeAck_VC_C,
    frmTypeTimeB   = NA_TypeCodeTimeB_VC_C,
    frmTypeBrdcast = NA_TypeCodeBrdcast_VC_C,
    frmTypeReq2s   = NA_TypeCodeReq2s_VC_C,
    frmTypeClr2s   = NA_TypeCodeClr2s_VC_C,
    frmTypeUndef   = NA_TypeCodeUndef_VC_C
};

enum DILDATARATE
{
    rate500kS = NA_SymbolRate500kSymbols_VC_C,
    rate1MS = NA_SymbolRate1MSymbols_VC_C
};

enum DILMODSYSTEM
{
    mod2ary = NA_ModulationSystem2ary_BC_C,
    mod4ary = NA_ModulationSystem4ary_BC_C
};

enum SETTXRXSWORD
{
    sWordTx = 0,
    sWordRx = 1
};

enum DILSTAADDNO
{
    staAdd0 = 0,
    staAdd1 = 1
};

enum CHIRPBYTE
{
    chirpByte0 = 0x02,
    chirpByte1 = 0x04,
    chirpByte2 = 0x08
};

enum DILPRETRAILSYMBOLS
{
    chirpSymb0 = 0,
    chirpSymb1 = 1,
    chirpSymb2 = 2,
    chirpSymb3 = 3
};
/*
enum CRCTYPE
{
    crcType1 = Silbadero_TxRxCrcType1_VC,
    crcType2 = Silbadero_TxRxCrcType2_VC,
    crcType3 = Silbadero_TxRxCrcType3_VC
};
*/
enum ARQMODE
{
    arqModeNone = NA_RxArqModeNone_VC_C,
    arqModeCrc1 = NA_RxArqModeCrc1_VC_C,
    arqModeCrc2 = NA_RxArqModeCrc2_VC_C
};

enum PHCARRSENSMODE
{
    phCarrSensOff     = NA_TxPhCarrSensModeOff_VC_C,
    phCarrSensSymbols = NA_TxPhCarrSensModeSymbols_VC_C,
    phCarrSensRSSI    = NA_TxPhCarrSensModeRssi_VC_C,
    phCarrSensSymRSSI = NA_TxPhCarrSensModeSymbolsRssi_VC_C
};

enum PWROUTTYPE
{
    pwrOutData = 0,
    pwrOutReqst = 1
};


void NTRXSetIndexReg (MyByte8T page);
void NTRXSetRegister (NTRXCmdTE cmd, MyByte8T value);
void NTRXGetRegister (NTRXCmdTE cmd, MyByte8T *reg);
void NTRXSetNRegister (NTRXCmdTE cmd, MyByte8T *data);
void NTRXGetNRegister (NTRXCmdTE cmd, MyByte8T *reg);
void NTRXSetSyncWord (MyByte8T *value);
void NTRXSetupSpiInterface (MyBoolT lsb);
void NTRX250kbitInit_22MHz (void);
void NTRXSetAgcValues (MyByte8T bandwidth, MyByte8T symbolDur, MyByte8T symbolRate);
void NTRXSetRxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetTxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetThreshold (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetupTRxMode (MyByte8T fdma, MyByte8T sd, MyByte8T sr);
void SetRxStart(void);
void SetRxStop (void);
void NTRXSetTransparentMode( MyBoolT on );
void NTRXPowerdownMode (MyByte8T mode, MyDword32T seconds);
void NTRXStartBasebandTimer (MyInt16T startvalue);
void NTRXStopBasebandTimer (void);


/* this defines are defined in ranging.c*/
#define NA_80MHz            0
#define NA_22MHz            1
#define NA_22MHz_HR    2

#define NA_31k25_S NA_SymbolRate31k25Symbols_VC_C
#define NA_62k5_S  NA_SymbolRate62k5Symbols_VC_C
#define NA_125k_S  NA_SymbolRate125kSymbols_VC_C
#define NA_250k_S  NA_SymbolRate250kSymbols_VC_C
#define NA_500k_S  NA_SymbolRate500kSymbols_VC_C
#define NA_1M_S    NA_SymbolRate1MSymbols_VC_C
#define NA_2M_S    NA_SymbolRate2MSymbols_VC_C

#define NA_500ns    NA_SymbolDur500ns_C
#define NA_1us      NA_SymbolDur1000ns_C
#define NA_2us      NA_SymbolDur2000ns_C
#define NA_4us      NA_SymbolDur4000ns_C
#define NA_8us      NA_SymbolDur8000ns_C
#define NA_16us     NA_SymbolDur16000ns_C

#define NA_MAP_MODE 0
#define NA_FIX_MODE 1


#endif /* NTRXUTIL_H */
