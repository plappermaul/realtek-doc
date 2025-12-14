/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_EFUSE_H_
#define _RTK_EFUSE_H_

#include "hal_api.h"
#include "rtk_efuse_base_bit.h"
#include "rtk_efuse_base_reg.h"

u8 write_efuse_byte(u32 data, u32 mask, u32 dataPort, u32 dataIdx);
u8 load_efuse_data_to_reg(void);
u32 read_efuse_byte(u32 idx);
void dump_efuse_reg_data(void);
extern void efuse_rw_test(void);
void efuse_isr_verification(void);

#ifndef REG32
#define REG32(reg)      (*(volatile u32 *)(reg))
#endif

#endif

