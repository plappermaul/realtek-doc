/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_I2S_BASE_BIT_H_
#define _RTK_I2S_BASE_BIT_H_

// REG_I2S_CONTROL
#define BIT_CTLX_I2S_EN                         BIT(0)
#define BIT_SHIFT_CTLX_I2S_EN                   0
#define BIT_MASK_CTLX_I2S_EN                    0x1
#define BIT_CTRL_CTLX_I2S_EN(x)                 (((x) & BIT_MASK_CTLX_I2S_EN) << BIT_SHIFT_CTLX_I2S_EN)

#define BIT_SHIFT_CTLX_I2S_TRX_ACT              1
#define BIT_MASK_CTLX_I2S_TRX_ACT               0x3
#define BIT_CTRL_CTLX_I2S_TRX_ACT(x)            (((x) & BIT_MASK_CTLX_I2S_TRX_ACT) << BIT_SHIFT_CTLX_I2S_TRX_ACT)
#define BIT_GET_CTLX_I2S_TRX_ACT(x)             (((x) >> BIT_SHIFT_CTLX_I2S_TRX_ACT) & BIT_MASK_CTLX_I2S_TRX_ACT)

#define BIT_SHIFT_CTLX_I2S_CH_NUM               3
#define BIT_MASK_CTLX_I2S_CH_NUM                0x3
#define BIT_CTRL_CTLX_I2S_CH_NUM(x)             (((x) & BIT_MASK_CTLX_I2S_CH_NUM) << BIT_SHIFT_CTLX_I2S_CH_NUM)
#define BIT_GET_CTLX_I2S_CH_NUM(x)              (((x) >> BIT_SHIFT_CTLX_I2S_CH_NUM) & BIT_MASK_CTLX_I2S_CH_NUM)

#define BIT_CTLX_I2S_WL                         BIT(29)
#define BIT_SHIFT_CTLX_I2S_WL                   29
#define BIT_MASK_CTLX_I2S_WL                    0x3
#define BIT_CTRL_CTLX_I2S_WL(x)                 (((x) & BIT_MASK_CTLX_I2S_WL) << BIT_SHIFT_CTLX_I2S_WL)

#define BIT_CTLX_I2S_LRSWAP                     BIT(10)
#define BIT_SHIFT_CTLX_I2S_LRSWAP               10
#define BIT_MASK_CTLX_I2S_LRSWAP                0x1
#define BIT_CTRL_CTLX_I2S_LRSWAP(x)             (((x) & BIT_MASK_CTLX_I2S_LRSWAP) << BIT_SHIFT_CTLX_I2S_LRSWAP)

#define BIT_CTLX_I2S_SCK_INV                    BIT(11)
#define BIT_SHIFT_CTLX_I2S_SCK_INV              11
#define BIT_MASK_CTLX_I2S_SCK_INV               0x1
#define BIT_CTRL_CTLX_I2S_SCK_INV(x)            (((x) & BIT_MASK_CTLX_I2S_SCK_INV) << BIT_SHIFT_CTLX_I2S_SCK_INV)

#define BIT_CTLX_I2S_ENDIAN_SWAP                BIT(12)
#define BIT_SHIFT_CTLX_I2S_ENDIAN_SWAP          12
#define BIT_MASK_CTLX_I2S_ENDIAN_SWAP           0x1
#define BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(x)        (((x) & BIT_MASK_CTLX_I2S_ENDIAN_SWAP) << BIT_SHIFT_CTLX_I2S_ENDIAN_SWAP)

#define BIT_CTLX_I2S_EDGE_SWAP                BIT(5)
#define BIT_SHIFT_CTLX_I2S_EDGE_SWAP          5
#define BIT_MASK_CTLX_I2S_EDGE_SWAP           0x1
#define BIT_CTRL_CTLX_I2S_EDGE_SWAP(x)        (((x) & BIT_MASK_CTLX_I2S_EDGE_SWAP) << BIT_SHIFT_CTLX_I2S_EDGE_SWAP)

#define BIT_CTLX_I2S_SLAVE_MODE                 BIT(28)
#define BIT_SHIFT_CTLX_I2S_SLAVE_MODE           28
#define BIT_MASK_CTLX_I2S_SLAVE_MODE            0x1
#define BIT_CTRL_CTLX_I2S_SLAVE_MODE(x)         (((x) & BIT_MASK_CTLX_I2S_SLAVE_MODE) << BIT_SHIFT_CTLX_I2S_SLAVE_MODE)

// 8198e change the clock source form control to setting 0xc
//#define BIT_CTLX_I2S_CLK_SRC                    BIT(30)
//#define BIT_SHIFT_CTLX_I2S_CLK_SRC              30
//#define BIT_MASK_CTLX_I2S_CLK_SRC               0x1
//#define BIT_CTRL_CTLX_I2S_CLK_SRC(x)            (((x) & BIT_MASK_CTLX_I2S_CLK_SRC) << BIT_SHIFT_CTLX_I2S_CLK_SRC)

#define BIT_CTLX_I2S_SW_RSTN                    BIT(31)
#define BIT_SHIFT_CTLX_I2S_SW_RSTN              31
#define BIT_MASK_CTLX_I2S_SW_RSTN               0x1
#define BIT_CTRL_CTLX_I2S_SW_RSTN(x)            (((x) & BIT_MASK_CTLX_I2S_SW_RSTN) << BIT_SHIFT_CTLX_I2S_SW_RSTN)

#define BIT_SHIFT_CTLX_I2S_BURST_LENGTH    18
#define BIT_MASK_CTLX_I2S_BURST_LENGTH     0x1f
#define BIT_CTRL_CTLX_I2S_BURST_LENGTH(x)     (((x) & BIT_MASK_CTLX_I2S_BURST_LENGTH) << BIT_SHIFT_CTLX_I2S_BURST_LENGTH)

// REG_I2S_SETTING
#define BIT_SHIFT_SETTING_I2S_PAGE_SZ           0
#define BIT_MASK_SETTING_I2S_PAGE_SZ            0xFFF
#define BIT_CTRL_SETTING_I2S_PAGE_SZ(x)         (((x) & BIT_MASK_SETTING_I2S_PAGE_SZ) << BIT_SHIFT_SETTING_I2S_PAGE_SZ)
#define BIT_GET_SETTING_I2S_PAGE_SZ(x)          (((x) >> BIT_SHIFT_SETTING_I2S_PAGE_SZ) & BIT_MASK_SETTING_I2S_PAGE_SZ)

#define BIT_SHIFT_SETTING_I2S_PAGE_NUM          12
#define BIT_MASK_SETTING_I2S_PAGE_NUM           0x3
#define BIT_CTRL_SETTING_I2S_PAGE_NUM(x)        (((x) & BIT_MASK_SETTING_I2S_PAGE_NUM) << BIT_SHIFT_SETTING_I2S_PAGE_NUM)
#define BIT_GET_SETTING_I2S_PAGE_NUM(x)         (((x) >> BIT_SHIFT_SETTING_I2S_PAGE_NUM) & BIT_MASK_SETTING_I2S_PAGE_NUM)

#define BIT_SHIFT_SETTING_I2S_SAMPLE_RATE       14
#define BIT_MASK_SETTING_I2S_SAMPLE_RATE        0xf
#define BIT_CTRL_SETTING_I2S_SAMPLE_RATE(x)     (((x) & BIT_MASK_SETTING_I2S_SAMPLE_RATE) << BIT_SHIFT_SETTING_I2S_SAMPLE_RATE)
#define BIT_GET_SETTING_I2S_SAMPLE_RATE(x)      (((x) >> BIT_SHIFT_SETTING_I2S_SAMPLE_RATE) & BIT_MASK_SETTING_I2S_SAMPLE_RATE)

// 8198e change the clock source form control to setting 0xc
#define BIT_SETTING_I2S_CLK_SRC                    BIT(18)
#define BIT_SHIFT_SETTING_I2S_CLK_SRC              18
#define BIT_MASK_SETTING_I2S_CLK_SRC               0x1
#define BIT_CTRL_SETTING_I2S_CLK_SRC(x)            (((x) & BIT_MASK_SETTING_I2S_CLK_SRC) << BIT_SHIFT_SETTING_I2S_CLK_SRC)

// i2s trx page own bit
#define BIT_PAGE_I2S_OWN_BIT                    BIT(31)
#define BIT_SHIFT_PAGE_I2S_OWN_BIT              31
#define BIT_MASK_PAGE_I2S_OWN_BIT               0x1
#define BIT_CTRL_PAGE_I2S_OWN_BIT(x)            (((x) & BIT_MASK_PAGE_I2S_OWN_BIT) << BIT_SHIFT_PAGE_I2S_OWN_BIT)


#endif // _I2S_BASE_BIT_H_

