/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_HAL_UART_H_
#define _RTK_HAL_UART_H_

#include "peripheral.h"
//#include "rtk_uart_base.h"

/**
  * LOG Configurations
  */
#define LOG_TAG           "NoTag"
#define LOG_INFO_HEADER   "I"
#define LOG_DEBUG_HEADER  "D"
#define LOG_ERROR_HEADER  "E"

#define LOG_INFO(...)  do {\
        DiagPrintf("\r"LOG_INFO_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_DEBUG(...)  do {\
        DiagPrintf("\r"LOG_DEBUG_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOG_ERROR(...)  do {\
        DiagPrintf("\r"LOG_ERROR_HEADER"/"LOG_TAG": " __VA_ARGS__);\
}while(0)

#define LOGI LOG_INFO
#define LOGD LOG_DEBUG
#define LOGE LOG_ERROR

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
 * RUART Configurations
 */
#define UART_DEBUG 0

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG  "RUART"
#endif

typedef struct _UART_DMA_CONFIG_ {
    u8 DmaEnable;
    u8 TxDmaEnable;
    u8 RxDmaEnable;
    u8 TxDmaBurstSize;
    u8 RxDmaBurstSize;
}UART_DMA_CONFIG, *PUART_DMA_CONFIG;

typedef struct _HAL_RUART_ADAPTER_ {
    u32 BaudRate;
    u32 FlowControl;
    u32 FifoControl;
    u32 Interrupts;
    u32 Test_Times;
    PUART_DMA_CONFIG DmaConfig;
    u8 BinaryData;
    u8 UartIndex;
    u8 DataBits;
    u8 Parity;
    u8 StopBits;
    u8 DmaEnable;
    u8 TestCaseNumber;
    u8 PinmuxSelect;
    BOOL PullMode;
    u8 LoopBackMode:    1, //John add
       LoopBackMode_2:  1,
       spare: 6;
    u8 G2Uart;
}HAL_RUART_ADAPTER, *PHAL_RUART_ADAPTER;

typedef struct _HAL_RUART_OP_ {
    VOID (*HalRuartResetRxFifo)(VOID *Data);
    BOOL (*HalRuartInit)(VOID *Data);
    VOID (*HalRuartSend)(VOID *Data);
    u8   (*HalRuartRecv)(VOID *Data);
    u32  (*HalRuartGetInterruptEnReg)(VOID *Data);
    VOID (*HalRuartSetInterruptEnReg)(VOID *Data);
    u32  (*HalRuartGetDebugValue)(VOID *Data, u32 DbgSel);
    VOID (*HalRuartDmaInit)(VOID *Data);
    VOID (*HalRuartRequestToSend)(VOID *Data);
}HAL_RUART_OP, *PHAL_RUART_OP;

typedef struct _RUART_DATA_ {
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    BOOL PullMode;
    u8   BinaryData;
    u8   SendBuffer;
    u8   RecvBuffer;
}RUART_DATA, *PRUART_DATA;

VOID
HalRuartOpInit(
        IN VOID *Data
);

#endif

