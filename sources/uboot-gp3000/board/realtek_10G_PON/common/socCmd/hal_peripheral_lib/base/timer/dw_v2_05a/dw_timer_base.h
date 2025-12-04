/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_TIMER_BASE_H_
#define _DW_TIMER_BASE_H_

#include "peripheral.h"
#include "dw_timer_base_bit.h"
#include "dw_timer_base_reg.h"

#define TIMER_TICK_US               31
#define TIMER_CLKHZ					32000
#define TIMER_INTERVAL              0x14
#ifdef CONFIG_RTL8198F
#define HAL_TIMER_READ32(addr)            SOC_HAL_R32(TIMER_REG_BASE, addr)//(*((volatile u32*)(TIMER_REG_BASE + addr)))//HAL_READ32(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE32(addr, value)    SOC_HAL_W32(TIMER_REG_BASE, addr, value)//((*((volatile u32*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE32(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ16(addr)            SOC_HAL_R16(TIMER_REG_BASE, addr)//(*((volatile u16*)(TIMER_REG_BASE + addr)))//HAL_READ16(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE16(addr, value)    SOC_HAL_W16(TIMER_REG_BASE, addr, value)//((*((volatile u16*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE16(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ8(addr)             SOC_HAL_R8(TIMER_REG_BASE, addr)//(*((volatile u8*)(TIMER_REG_BASE + addr)))//HAL_READ8(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE8(addr, value)     SOC_HAL_W8(TIMER_REG_BASE, addr, value)//((*((volatile u8*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE8(TIMER_REG_BASE, addr, value)
#else
#define HAL_TIMER_READ32(addr)            (*((volatile u32*)(TIMER_REG_BASE + addr)))//HAL_READ32(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE32(addr, value)    ((*((volatile u32*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE32(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ16(addr)            (*((volatile u16*)(TIMER_REG_BASE + addr)))//HAL_READ16(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE16(addr, value)    ((*((volatile u16*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE16(TIMER_REG_BASE, addr, value)
#define HAL_TIMER_READ8(addr)             (*((volatile u8*)(TIMER_REG_BASE + addr)))//HAL_READ8(TIMER_REG_BASE, addr)
#define HAL_TIMER_WRITE8(addr, value)     ((*((volatile u8*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE8(TIMER_REG_BASE, addr, value)

#endif

u32
HalGetTimerIdCommon(
    IN  u32     *TimerID
);

BOOL
HalTimerInitCommon(
    IN  VOID    *Data
);

u32
HalTimerReadCountCommon(
    IN  u32     TimerId
);

VOID
HalTimerIrqClearCommon(
    IN  u32 TimerId
);

VOID
HalTimerDisCommon(
    IN  u32 TimerId
);

VOID
HalTimerEnCommon(
    IN  u32 TimerId
);

VOID
HalTimerDumpRegCommon(
    IN  u32 TimerId
);

BOOL
HalTimerIsDisabledCommon(
    IN  u32 TimerId
);

#endif //_RTK_TIMER_BASE_H_
