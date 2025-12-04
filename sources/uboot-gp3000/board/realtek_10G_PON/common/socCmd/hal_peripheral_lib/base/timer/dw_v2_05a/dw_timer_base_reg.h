
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_TIMER_BASE_REG_H_
#define _DW_TIMER_BASE_REG_H_

#define REG_TIMER_LOAD_COUNT        0x00
#define REG_TIMER_CURRENT_VAL       0x04
#define REG_TIMER_CTL_REG           0x08
#define REG_TIMER_EOI               0x0c
#define REG_TIMER_INT_STATUS        0x10

#define REG_TIMERS_INT_STATUS       0xa0
#define REG_TIMERS_EOI              0xa4
#define REG_TIMERS_RAW_INT_STATUS   0xa8
#define REG_TIMERS_COMP_VER         0xac

#endif //#ifndef _DW_TIMER_BASE_REG_H_
