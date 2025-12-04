/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_I2C_BASE_TEST_H_
#define _DW_I2C_BASE_TEST_H_

//#include "osdep_api.h"

#define I2C_VERI_DATA_LEN   1024
#define I2C_VERI_RXBUF_LEN    16
#define I2C_VERI_TXBUF_LEN    24

#define I2C_VERI_PANT_CNT   6
#define I2C_VERI_TIMEOUT_CNT    (8*(SYSTEM_CLK/100000))

typedef enum _I2C_VERI_PROC_CMD_ {
        I2C_TEST_INIT       = 0,
        I2C_TEST_SWCH       = 1,
        I2C_TEST_DEINIT     = 2,
        I2C_TEST_GETCNT     = 3
}I2C_VERI_PROC_CMD, *PI2C_VERI_PROC_CMD;


typedef enum _I2C_VERI_ITEM_ {
		I2C_TEST_S_NINT 	= 1,	 //single rw without DMA without INT
		I2C_TEST_S_RINT 	= 2,	 //single rw without DMA with INT
		I2C_TEST_D_INT		= 3,	 //single rw with DMA with INT
		I2C_TEST_DTR_INT	= 4,	 //single rw using DMA with INT
		I2C_TEST_INT_ERR	= 5,	 //err INT test
		I2C_TEST_INT_GenCallvsStatus	= 6, //test gen call
		I2C_TEST_INT_TXABRT =7,
		I2C_ADDR_SCAN		=9,
#if TASK_SCHEDULER_DISABLED
        I2C_TEST_S_GCINT    = 8,
#else
        I2C_TEST_S_GCINT    = 8,
        I2C_TEST_N_DEINT    = 10
#endif
}I2C_VERI_ITEM, *PI2C_VERI_ITEM;

typedef struct _I2C_VERI_PARA_ {
        u32         VeriProcCmd;
        u32         VeriItem;
        u32         VeriLoop;
        u32         VeriMaster;
        u32         VeriSlave;
        u32         MtrRW;
#if !TASK_SCHEDULER_DISABLED
        _Sema       VeriSema;
#else
        u32         VeriSema;
#endif  
#ifdef PLATFORM_FREERTOS
        xTaskHandle I2CTask;
#else
        u32         I2CTask;
#endif
        u8          SpdMod;
        u8          AddrMod;
#if !TASK_SCHEDULER_DISABLED        
        u8          VeriStop;
        u8          Revd;
#else
        u16          Revd;
#endif
}I2C_VERI_PARA,*PI2C_VERI_PARA;


#endif //_I2C_BASE_TEST_H_
