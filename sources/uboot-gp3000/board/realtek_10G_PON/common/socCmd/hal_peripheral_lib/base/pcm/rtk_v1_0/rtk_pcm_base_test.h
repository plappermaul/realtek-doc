/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_PCM_BASE_TEST_H_
#define _RTK_PCM_BASE_TEST_H_

//#include "osdep_api.h"


typedef enum _PCM_VERI_PROC_CMD_ {
        PCM_TEST_INIT       = 0,
        PCM_TEST_SWCH       = 1,
        PCM_TEST_DEINIT     = 2,
        PCM_TEST_GETCNT     = 3
}PCM_VERI_PROC_CMD, *PPCM_VERI_PROC_CMD;

typedef enum _PCM_VERI_ITEM_ {
        //I2C_TEST_S_NINT     = 1,     //single rw without DMA without INT
        //I2C_TEST_S_RINT     = 2,     //single rw without DMA with INT
        //I2C_TEST_D_INT      = 3,     //single rw without DMA with INT
        //I2C_TEST_DTR_INT    = 4,     //single rw using DMA with INT
#if TASK_SCHEDULER_DISABLED
        PCM_TEST_S_GCINT    = 7
#else
        //I2C_TEST_S_GCINT    = 7,
        PCM_TEST_N_DEINT    = 10
#endif
}PCM_VERI_ITEM, *PPCM_VERI_ITEM;

typedef struct _PCM_VERI_PARA_ {
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
        xTaskHandle PCMTask;
#else
        u32         PCMTask;
#endif
        u8          SpdMod;
        u8          AddrMod;
#if !TASK_SCHEDULER_DISABLED        
        u8          VeriStop;
        u8          Revd;
#else
        u16          Revd;
#endif
}PCM_VERI_PARA,*PPCM_VERI_PARA;


#endif // _PCM_BASE_TEST_H_

