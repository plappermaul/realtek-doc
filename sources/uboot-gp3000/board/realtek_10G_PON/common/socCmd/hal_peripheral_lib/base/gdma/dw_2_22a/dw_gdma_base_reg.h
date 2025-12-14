/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_GDMA_REG_H_
#define _DW_GDMA_REG_H_




#define GDMA_CH_MAX                 0x06

#define REG_GDMA_CH_OFF                 0x058
#define REG_GDMA_CH_SAR                 0x000
#define REG_GDMA_CH_DAR                 0x008
#define REG_GDMA_CH_LLP                 0x010
#define REG_GDMA_CH_CTL                 0x018
#define REG_GDMA_CH_SSTAT               0x020
#define REG_GDMA_CH_DSTAT               0x028
#define REG_GDMA_CH_SSTATAR             0x030
#define REG_GDMA_CH_DSTATAR             0x038
#define REG_GDMA_CH_CFG                 0x040
#define REG_GDMA_CH_SGR                 0x048
#define REG_GDMA_CH_DSR                 0x050

//3 Interrupt Registers
#define REG_GDMA_RAW_INT_BASE           0x2C0
#define REG_GDMA_RAW_INT_TFR            0x2C0
#define REG_GDMA_RAW_INT_BLOCK          0x2c8
#define REG_GDMA_RAW_INT_SRC_TRAN       0x2D0
#define REG_GDMA_RAW_INT_DST_TRAN       0x2D8
#define REG_GDMA_RAW_INT_ERR            0x2E0

#define REG_GDMA_STATUS_INT_BASE        0x2E8
#define REG_GDMA_STATUS_INT_TFR         0x2E8
#define REG_GDMA_STATUS_INT_BLOCK       0x2F0
#define REG_GDMA_STATUS_INT_SRC_TRAN    0x2F8
#define REG_GDMA_STATUS_INT_DST_TRAN    0x300
#define REG_GDMA_STATUS_INT_ERR         0x308

#define REG_GDMA_MASK_INT_BASE          0x310
#define REG_GDMA_MASK_INT_TFR           0x310
#define REG_GDMA_MASK_INT_BLOCK         0x318
#define REG_GDMA_MASK_INT_SRC_TRAN      0x320
#define REG_GDMA_MASK_INT_DST_TRAN      0x328
#define REG_GDMA_MASK_INT_INT_ERR       0x330

#define REG_GDMA_CLEAR_INT_BASE         0x338
#define REG_GDMA_CLEAR_INT_TFR          0x338
#define REG_GDMA_CLEAR_INT_BLOCK        0x340
#define REG_GDMA_CLEAR_INT_SRC_TRAN     0x348
#define REG_GDMA_CLEAR_INT_DST_TRAN     0x350
#define REG_GDMA_CLEAR_INT_ERR          0x358
#define REG_GDMA_STATUS_INT             0x360

//3 Software handshaking Registers
#define REG_GDMA_REQ_SRC                0x368
#define REG_GDMA_REQ_DST                0x370
#define REG_GDMA_REQ_SGL_REQ            0x378
#define REG_GDMA_REQ_DST_REQ            0x380
#define REG_GDMA_REQ_LST_SRC            0x388
#define REG_GDMA_REQ_LST_DST            0x390

//3 Miscellaneous Registers
#define REG_GDMA_DMAC_CFG               0x398
#define REG_GDMA_CH_EN                  0x3A0
#define REG_GDMA_DMA_ID                 0x3A8
#define REG_GDMA_DMA_TEST               0x3B0
#define REG_GDMA_DMA_COM_PARAMS6        0x3C8
#define REG_GDMA_DMA_COM_PARAMS5        0x3D0
#define REG_GDMA_DMA_COM_PARAMS4        0x3D8
#define REG_GDMA_DMA_COM_PARAMS3        0x3E0
#define REG_GDMA_DMA_COM_PARAMS2        0x3E8
#define REG_GDMA_DMA_COM_PARAMS1        0x3F0
#define REG_GDMA_DMA_COM_PARAMS0        0x3F8


#endif
