/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_EFUSE_REG_H_
#define _RTK_EFUSE_REG_H_

#define     EFUSE_BASE_ADDR                     0x700
#define     REG_EFUSE_BASE_ADDR                 SYSTEM_REG_BASE + EFUSE_BASE_ADDR  // 0x18000700

#define     REG_EFUSE_CMD                       (REG_EFUSE_BASE_ADDR + 0x00000000)
#define     REG_EFUSE_CONFIG                    (REG_EFUSE_BASE_ADDR + 0x00000004)
#define     REG_EFUSE_TIMING_CTRL               (REG_EFUSE_BASE_ADDR + 0x00000008)

#define     REG_EFUSE_WRITE_DATA_0              (REG_EFUSE_BASE_ADDR + 0x00000010)
#define     REG_EFUSE_WRITE_DATA_1              (REG_EFUSE_BASE_ADDR + 0x00000014)
#define     REG_EFUSE_WRITE_DATA_2              (REG_EFUSE_BASE_ADDR + 0x00000018)
#define     REG_EFUSE_WRITE_DATA_3              (REG_EFUSE_BASE_ADDR + 0x0000001C)
#define     REG_EFUSE_WRITE_DATA_BIT_MASK_0     (REG_EFUSE_BASE_ADDR + 0x00000020)
#define     REG_EFUSE_WRITE_DATA_BIT_MASK_1     (REG_EFUSE_BASE_ADDR + 0x00000024)
#define     REG_EFUSE_WRITE_DATA_BIT_MASK_2     (REG_EFUSE_BASE_ADDR + 0x00000028)
#define     REG_EFUSE_WRITE_DATA_BIT_MASK_3     (REG_EFUSE_BASE_ADDR + 0x0000002C)
#define     REG_EFUSE_BURN_CHECK_FAIL_0         (REG_EFUSE_BASE_ADDR + 0x00000030)
#define     REG_EFUSE_BURN_CHECK_FAIL_1         (REG_EFUSE_BASE_ADDR + 0x00000034)
#define     REG_EFUSE_BURN_CHECK_FAIL_2         (REG_EFUSE_BASE_ADDR + 0x00000038)
#define     REG_EFUSE_BURN_CHECK_FAIL_3         (REG_EFUSE_BASE_ADDR + 0x0000003C)

#define     REG_EFUSE_P0_0                      (REG_EFUSE_BASE_ADDR + 0x00000080)
#define     REG_EFUSE_P0_1                      (REG_EFUSE_BASE_ADDR + 0x00000084)
#define     REG_EFUSE_P0_2                      (REG_EFUSE_BASE_ADDR + 0x00000088)
#define     REG_EFUSE_P0_3                      (REG_EFUSE_BASE_ADDR + 0x0000008C)
#define     REG_EFUSE_P1_0                      (REG_EFUSE_BASE_ADDR + 0x00000090)
#define     REG_EFUSE_P1_1                      (REG_EFUSE_BASE_ADDR + 0x00000094)
#define     REG_EFUSE_P1_2                      (REG_EFUSE_BASE_ADDR + 0x00000098)
#define     REG_EFUSE_P1_3                      (REG_EFUSE_BASE_ADDR + 0x0000009C)
#define     REG_EFUSE_P2_0                      (REG_EFUSE_BASE_ADDR + 0x000000A0)
#define     REG_EFUSE_P2_1                      (REG_EFUSE_BASE_ADDR + 0x000000A4)
#define     REG_EFUSE_P2_2                      (REG_EFUSE_BASE_ADDR + 0x000000A8)
#define     REG_EFUSE_P2_3                      (REG_EFUSE_BASE_ADDR + 0x000000AC)
#define     REG_EFUSE_P3_0                      (REG_EFUSE_BASE_ADDR + 0x000000B0)
#define     REG_EFUSE_P3_1                      (REG_EFUSE_BASE_ADDR + 0x000000B4)
#define     REG_EFUSE_P3_2                      (REG_EFUSE_BASE_ADDR + 0x000000B8)
#define     REG_EFUSE_P3_3                      (REG_EFUSE_BASE_ADDR + 0x000000BC)
#define     REG_EFUSE_P4_0                      (REG_EFUSE_BASE_ADDR + 0x000000C0)
#define     REG_EFUSE_P4_1                      (REG_EFUSE_BASE_ADDR + 0x000000C4)
#define     REG_EFUSE_P4_2                      (REG_EFUSE_BASE_ADDR + 0x000000C8)
#define     REG_EFUSE_P4_3                      (REG_EFUSE_BASE_ADDR + 0x000000CC)
#define     REG_EFUSE_P5_0                      (REG_EFUSE_BASE_ADDR + 0x000000D0)
#define     REG_EFUSE_P5_1                      (REG_EFUSE_BASE_ADDR + 0x000000D4)
#define     REG_EFUSE_P5_2                      (REG_EFUSE_BASE_ADDR + 0x000000D8)
#define     REG_EFUSE_P5_3                      (REG_EFUSE_BASE_ADDR + 0x000000DC)
#define     REG_EFUSE_P6_0                      (REG_EFUSE_BASE_ADDR + 0x000000E0)
#define     REG_EFUSE_P6_1                      (REG_EFUSE_BASE_ADDR + 0x000000E4)
#define     REG_EFUSE_P6_2                      (REG_EFUSE_BASE_ADDR + 0x000000E8)
#define     REG_EFUSE_P6_3                      (REG_EFUSE_BASE_ADDR + 0x000000EC)
#define     REG_EFUSE_P7_0                      (REG_EFUSE_BASE_ADDR + 0x000000F0)
#define     REG_EFUSE_P7_1                      (REG_EFUSE_BASE_ADDR + 0x000000F4)
#define     REG_EFUSE_P7_2                      (REG_EFUSE_BASE_ADDR + 0x000000F8)
#define     REG_EFUSE_P7_3                      (REG_EFUSE_BASE_ADDR + 0x000000FC)

#define     EFUSE_PORT_MAX                      8
#define     EFUSE_PORT_IDX_MAX                  4

// REG_EFUSE_CMD
#define     BIT_SHIFT_EFUSE_CMD_ADDR            0
#define     BIT_MASK_EFUSE_CMD_ADDR             0x7
#define     BIT_EFUSE_CMD_ADDR(x)               (((x) & BIT_MASK_EFUSE_CMD_ADDR) << BIT_SHIFT_EFUSE_CMD_ADDR)
#define     BIT_EFUSE_RW_CTRL                   BIT(15)
#define     BIT_EFUSE_CTRL_STATE                BIT(16)

// REG_EFUSE_CONFIG
#define     BIT_EFUSE_CONFIG_PWR_GATE_EN        BIT(0)
#define     BIT_EFUSE_CONFIG_INT_STATUS         BIT(8)
#define     BIT_EFUSE_CONFIG_INT_MASK           BIT(9)
#define     BIT_EFUSE_CONFIG_BURN_CHECK         BIT(31)


// return status
#define     STATUS_EFUSE_SUCCESS                0
#define     STATUS_EFUSE_FAIL                   1
#define     STATUS_EFUSE_TIMEOUT                2
#define     STATUS_EFUSE_EMPTY                  3

#endif

