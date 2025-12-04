/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _BUS_BASE_H_
#define _BUS_BASE_H_

#include "hal_api.h"
#include "bus_base_bit.h"
#include "bus_base_reg.h"

#define HAL_BUS_WRITE32(addr, value)    HAL_WRITE32(BUS_REG_BASE, addr, value)
#define HAL_BUS_READ32(addr)            HAL_READ32(BUS_REG_BASE, addr)

#define HAL_BUS_WRITE16(addr, value)    HAL_WRITE16(BUS_REG_BASE, addr, value)
#define HAL_BUS_READ16(addr)            HAL_READ16(BUS_REG_BASE, addr)

#define HAL_BUS_WRITE8(addr, value)    HAL_WRITE8(BUS_REG_BASE, addr, value)
#define HAL_BUS_READ8(addr)            HAL_READ8(BUS_REG_BASE, addr)

#define BUS_PREFIX      "[bus]: "
#define BUS_PREFIX_LVL  "[bus_DBG]: "

typedef enum _BUS_DBG_LVL_ {
    HAL_BUS_LVL         =   0x01,
    VERI_BUS_LVL        =   0x02,
}BUS_DBG_LVL,*PBUS_DBG_LVL;

BOOL
HalBUSInitCommon(
    IN  VOID    *Data
);

VOID
HalBUSExecCommon(
    IN  VOID    *Data
);

#endif // _BUS_BASE_H_

