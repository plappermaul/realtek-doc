/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_HAL_PWM_H_
#define _RTK_HAL_PWM_H_

#include "peripheral.h"


typedef struct _HAL_PWM_OP_ {
    BOOL (*HalPWMEn)(VOID *Data);
    BOOL (*HalPWMInit)(VOID *Data);
}HAL_PWM_OP, *PHAL_PWM_OP;

typedef struct _HAL_GTEVENT_OP_ {
    BOOL (*HalGTEventEn)(VOID *Data);
    BOOL (*HalGTEventInit)(VOID *Data);
}HAL_GTEVENT_OP, *PHAL_GTEVENT_OP;


typedef struct _HAL_PWM_DAT_ADAPTER_ {
    u8                  Idx;
    u8                  En;
	u8					GT_SEL;
	u8					Duty;
	u8					Period;
//    u8                  PinOutEn;
    u8                  PinMux;           
}HAL_PWM_DAT_ADAPTER, *PHAL_PWM_DAT_ADAPTER;

typedef struct _HAL_GTEVENT_DAT_ADAPTER_ {
    u8                  Idx;
    u8                  En;
	u8					Src_Sel;
	u8					Pulse_dur;
//    u8                  PinOutEn;
    u8                  PinMux;           
}HAL_GTEVENT_DAT_ADAPTER, *PHAL_GTEVENT_DAT_ADAPTER;

VOID HalPWMOpInit(
    IN  VOID *Data
);
VOID HalGTEventOpInit(
    IN  VOID *Data
);



#endif
