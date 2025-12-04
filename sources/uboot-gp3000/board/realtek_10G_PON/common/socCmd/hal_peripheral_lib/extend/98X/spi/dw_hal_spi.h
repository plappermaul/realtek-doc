/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_HAL_SSI_H_
#define _DW_HAL_SSI_H_

#include "peripheral.h"
//#include "dw_spi_base.h"

/**
 * LOG Configurations
 */
// #define NOLOG

#define LOG_TAG           "NoTag"
#define LOG_INFO_HEADER   "I"
#define LOG_DEBUG_HEADER  "D"
#define LOG_ERROR_HEADER  "E"
#define LOG_TEST_HEADER   "T"

#define LOG_INFO(...)  do {\
            DiagPrintf("\r"LOG_INFO_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_DEBUG(...)  do {\
            DiagPrintf("\r"LOG_DEBUG_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_ERROR(...)  do {\
            DiagPrintf("\r"LOG_ERROR_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_TEST(...)  do {\
            DiagPrintf("\r"LOG_TEST_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#ifdef NOLOG
    #define LOGI
    #define LOGD
    #define LOGE
#else
    #define LOGI LOG_INFO
    #define LOGD LOG_DEBUG
    #define LOGE LOG_ERROR
#endif

#define LOGT LOG_TEST

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define DBG_ENTRANCE LOGI(ANSI_COLOR_GREEN "%s()<%s>\n" ANSI_COLOR_RESET, \
        __func__, __FILE__)

/**
 * DesignWare SSI Configurations
 */

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG  "SSI"
#endif

typedef struct _HAL_SSI_ADAPTER_ {
    u32 Clock;
    u32 TxThresholdLevel;
    u32 RxThresholdLevel;
    u32 DmaTxDataLevel;
    u32 DmaRxDataLevel;
    u16 DataFrameNumber;   /* Master && (Receive Only || EEPROM Read) */
    u16 ClockDivider;
    //
    u16 TxData;
    u16 RxData;
    //
    u8  PollMode;
    u8  PinmuxSelect;
    u8  Index;
    u8  Role;              /* Master/Slave */
    u8  DataFrameSize;
    u8  DataFrameFormat;   /* SPI/SSP/MicroWire */
    u8  SclkPolarity;
    u8  SclkPhase;
    u8  TransferMode;      /* TR/TO/RO/EEPROM */
    u8  SlaveOutputEnable;
    u8  ControlFrameSize;
    u8  SlaveSelectEnable;
    u8  InterruptMask;
    u8  DmaControl;
    u8  MicrowireHandshaking;
    u8  MicrowireDirection;
    u8  MicrowireTransferMode;
}HAL_SSI_ADAPTER, *PHAL_SSI_ADAPTER;

typedef struct _HAL_SSI_OP_{
    BOOL (*HalSsiPinmuxEnable)(VOID *Data);
    BOOL (*HalSsiEnable)(VOID *Data);
    BOOL (*HalSsiDisable)(VOID *Data);
    BOOL (*HalSsiInit)(VOID *Data);
    BOOL (*HalSsiSetSclkPolarity)(VOID *Data);
    BOOL (*HalSsiSetSclkPhase)(VOID *Data);
    BOOL (*HalSsiWriter)(VOID *Data);
    u16  (*HalSsiReader)(VOID *Data);
    u32  (*HalSsiGetRxFifoLevel)(VOID *Data);
    u32  (*HalSsiGetTxFifoLevel)(VOID *Data);
    u32  (*HalSsiGetStatus)(VOID *Data);
    u32  (*HalSsiGetInterruptStatus)(VOID *Data);
}HAL_SSI_OP, *PHAL_SSI_OP;

VOID
HalSsiOpInit(
        IN VOID *Data
        );

#endif

