/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _CA_HAL_RTC_H_
#define _CA_HAL_RTC_H_

#include "peripheral.h"

typedef struct _HAL_CA_RTC_OP_ {
    BOOL (*HalRTCInit)(VOID *Data);
    BOOL (*HalRTCdeInit)(VOID *Data);
    VOID (*HalRTCClrIntr)(VOID *Data);
    VOID (*HalRTCEnableIntr)(VOID *Data);
}HAL_CA_RTC_OP, *PHAL_CA_RTC_OP;

typedef struct _HAL_CA_RTC_ADAPTER_{
    //PHAL_DW_I2C_DAT_ADAPTER    pI2CIrqDat;
    PHAL_CA_RTC_OP             pRTCOp;
}HAL_CA_RTC_ADAPTER,*PHAL_CA_RTC_ADAPTER;


typedef struct _PRTC_TIME_{
        u8  year;
        u8  month;
        u8  week;
        u8  day;
        u8  hour;
        u8  min;
        u8  sec;
}RTC_TIME,*PRTC_TIME;

typedef struct _RTC_ADAPTER_ {
    u8  IC_INRT_MSK;
    u8  alarm_type;
    u8  wakeup;
    RTC_TIME    settime;
    RTC_TIME    readtime;
}RTC_ADAPTER, *PRTC_ADAPTER;


VOID HalRTCOpInit(
    IN  VOID *Data
);


#endif

