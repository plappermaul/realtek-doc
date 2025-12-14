/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_NFBI_BASE_H_
#define _RTK_NFBI_BASE_H_

#include "peripheral.h"
#include "rtk_nfbi_base_bit.h"
#include "rtk_nfbi_base_reg.h"

#ifdef CONFIG_RTL8198F
#define HAL_NFBI_READ32(addr)            SOC_HAL_R32(NFBI_REG_BASE, addr)//(*((volatile u32*)(TIMER_REG_BASE + addr)))//HAL_READ32(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE32(addr, value)    SOC_HAL_W32(NFBI_REG_BASE, addr, value)//((*((volatile u32*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE32(TIMER_REG_BASE, addr, value)
#define HAL_NFBI_READ16(addr)            SOC_HAL_R16(NFBI_REG_BASE, addr)//(*((volatile u16*)(TIMER_REG_BASE + addr)))//HAL_READ16(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE16(addr, value)    SOC_HAL_W16(NFBI_REG_BASE, addr, value)//((*((volatile u16*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE16(TIMER_REG_BASE, addr, value)
#define HAL_NFBI_READ8(addr)             SOC_HAL_R8(NFBI_REG_BASE, addr)//(*((volatile u8*)(TIMER_REG_BASE + addr)))//HAL_READ8(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE8(addr, value)     SOC_HAL_W8(NFBI_REG_BASE, addr, value)//((*((volatile u8*)(TIMER_REG_BASE + addr))) = value)//HAL_WRITE8(TIMER_REG_BASE, addr, value)
#else
#define HAL_NFBI_READ32(addr)            (*((volatile u32*)(NFBI_REG_BASE + addr)))//HAL_READ32(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE32(addr, value)    ((*((volatile u32*)(NFBI_REG_BASE + addr))) = value)//HAL_WRITE32(TIMER_REG_BASE, addr, value)
#define HAL_NFBI_READ16(addr)            (*((volatile u16*)(NFBI_REG_BASE + addr)))//HAL_READ16(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE16(addr, value)    ((*((volatile u16*)(NFBI_REG_BASE + addr))) = value)//HAL_WRITE16(TIMER_REG_BASE, addr, value)
#define HAL_NFBI_READ8(addr)             (*((volatile u8*)(NFBI_REG_BASE + addr)))//HAL_READ8(TIMER_REG_BASE, addr)
#define HAL_NFBI_WRITE8(addr, value)     ((*((volatile u8*)(NFBI_REG_BASE + addr))) = value)//HAL_WRITE8(TIMER_REG_BASE, addr, value)

#endif


BOOL
HalNFBIInitCommon(
    IN  VOID    *Data
);

VOID
HalNFBIIrqClearCommon(
);


#endif //_RTK_NFBI_BASE_H_
