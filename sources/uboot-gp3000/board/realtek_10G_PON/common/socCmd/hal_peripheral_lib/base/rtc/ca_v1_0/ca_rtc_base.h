/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_RTC_BASE_H_
#define _DW_RTC_BASE_H_

#include "hal_api.h"
#include "ca_rtc_base_bit.h"
#include "ca_rtc_base_reg.h"

//3 All writes must be secure access; otherwise slave error will be signaled.
#define HAL_RTC_WRITE32(addr, value)    HAL_WRITE32(RTC_REG_BASE, addr, value)
#define HAL_RTC_READ32(addr)            HAL_READ32(RTC_REG_BASE, addr)

#define HAL_RTC_WRITE16(addr, value)    HAL_WRITE16(RTC_REG_BASE, addr, value)
#define HAL_RTC_READ16(addr)            HAL_READ16(RTC_REG_BASE, addr)

#define HAL_RTC_WRITE8(addr, value)    HAL_WRITE8(RTC_REG_BASE, addr, value)
#define HAL_RTC_READ8(addr)            HAL_READ8(RTC_REG_BASE, addr)

//4 RTC debug output
#define RTC_PREFIX      "[rtc]: "
#define RTC_PREFIX_LVL  "    [rtc_DBG]: "

typedef enum _RTC_DBG_LVL_ {
    HAL_RTC_LVL         =   0x01,
    VERI_RTC_LVL        =   0x02,
}RTC_DBG_LVL,*PRTC_DBG_LVL;




//4 Hal RTC function prototype
VOID
RTCInterruptHandle(
    IN  VOID *Data

);

BOOL
HalRTCInitCACommon(
    IN  VOID    *Data
);

BOOL
HalRTCdeInitCACommon(
    IN  VOID    *Data
);


VOID
HalRTCClrIntrCACommon(
    IN  VOID    *Data
);

VOID
HalRTCClrAllIntrCACommon(
);

VOID
HalRTCEnableIntrCACommon(
    IN  VOID    *Data
);

VOID
ReadCurrentTimeCACommon(
    IN  VOID    *Data
);

VOID
ReadRippleCounterCACommon(

);

VOID
LoadNewTimeCACommon(
    IN  VOID    *Data
);

VOID
SetAlarmTimeCACommon(
    IN  VOID    *Adap
);

VOID
SetWakeupSelCACommon(
);

VOID
ClrWakeupSelCACommon(
);


#endif // _RTC_BASE_H_

