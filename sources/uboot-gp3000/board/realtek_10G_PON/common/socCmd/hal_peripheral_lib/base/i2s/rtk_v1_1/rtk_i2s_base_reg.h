/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_I2S_BASE_REG_H_
#define _RTK_I2S_BASE_REG_H_

#define REG_I2S_PAGE_OWN_OFF   0x004

#define REG_I2S_CTL            0x000
#define REG_I2S_TX_PAGE_PTR    0x004
#define REG_I2S_RX_PAGE_PTR    0x008
#define REG_I2S_SETTING        0x00C

#define REG_I2S_TX_MASK_INT    0x010
#define REG_I2S_TX_STATUS_INT  0x014
#define REG_I2S_RX_MASK_INT    0x018
#define REG_I2S_RX_STATUS_INT  0x01c


#define REG_I2S_TX_PAGE0_OWN   0x020
#define REG_I2S_TX_PAGE1_OWN   0x024
#define REG_I2S_TX_PAGE2_OWN   0x028
#define REG_I2S_TX_PAGE3_OWN   0x02C
#define REG_I2S_RX_PAGE0_OWN   0x030
#define REG_I2S_RX_PAGE1_OWN   0x034
#define REG_I2S_RX_PAGE2_OWN   0x038
#define REG_I2S_RX_PAGE3_OWN   0x03C

#define REG_I2S_VERSION_ID     0x040





#endif //_I2S_BASE_REG_H_

