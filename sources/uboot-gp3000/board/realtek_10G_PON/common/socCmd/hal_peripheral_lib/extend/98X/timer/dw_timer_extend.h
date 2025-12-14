/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_TIMER_EXTEND_H_
#define _DW_TIMER_EXTEND_H_

#include "peripheral.h"
#include "dw_timer_extend_bit.h"
#include "dw_timer_extend_reg.h"

BOOL HalTimerIrqRegister98F(IN  VOID *Data);
void HalWatchdogMode96F(IN void *Data);
void HalChangeCpuFreqMode96F(IN void *Data);
void EnableTimerIP98F();


#endif

