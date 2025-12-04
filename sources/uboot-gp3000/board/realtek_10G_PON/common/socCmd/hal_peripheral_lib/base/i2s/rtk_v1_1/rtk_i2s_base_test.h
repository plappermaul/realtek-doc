/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_I2S_BASE_TEST_H_
#define _RTK_I2S_BASE_TEST_H_

//#include "osdep_api.h"


typedef enum _I2S_VERI_PROC_CMD_ {
        I2S_TEST_INIT       = 0,
        I2S_TEST_SWCH       = 1,
        I2S_TEST_DEINIT     = 2,
        I2S_TEST_GETCNT     = 3
}I2S_VERI_PROC_CMD, *PI2S_VERI_PROC_CMD;

typedef enum _I2S_VERI_ITEM_ {
        //I2C_TEST_S_NINT     = 1,     //single rw without DMA without INT
        //I2C_TEST_S_RINT     = 2,     //single rw without DMA with INT
        //I2C_TEST_D_INT      = 3,     //single rw without DMA with INT
        //I2C_TEST_DTR_INT    = 4,     //single rw using DMA with INT
#if TASK_SCHEDULER_DISABLED
        I2S_TEST_S_GCINT    = 7
#else
        //I2C_TEST_S_GCINT    = 7,
        I2S_TEST_N_DEINT    = 10
#endif
}I2S_VERI_ITEM, *PI2S_VERI_ITEM;

typedef struct _I2S_VERI_PARA_ {
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
        xTaskHandle I2STask;
#else
        u32         I2STask;
#endif
        u8          SpdMod;
        u8          AddrMod;
#if !TASK_SCHEDULER_DISABLED        
        u8          VeriStop;
        u8          Revd;
#else
        u16          Revd;
#endif
}I2S_VERI_PARA,*PI2S_VERI_PARA;


#endif //_I2S_BASE_TEST_H_

