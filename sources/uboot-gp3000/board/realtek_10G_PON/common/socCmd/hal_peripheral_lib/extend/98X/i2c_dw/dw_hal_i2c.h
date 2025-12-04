/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_HAL_I2C_H_
#define _DW_HAL_I2C_H_

#include "peripheral.h"
//#include "dw_i2c_base.h"

typedef struct _HAL_DW_I2C_DAT_ADAPTER_ {
    u8                  Idx;
    u8                  ModulEn;
    u8                  PinOutEn;
    u8                  PinMux;    
    u8                  Master;    
    u8                  AddrMd; //addressing mode.
    u8                  BusLd;  //bus load (pf)
    u8                  SpdMd;
    u32                 ICClk;  //(MHz)
    u32                 SpdHz;    
    u8                  ReSTR;
    u8                  Spec;
    u8                  GC_STR;
    u8                  RWCmd;
    u8                  STP;
    u8                  DatLen;
    u8                  *RWDat;
    u8                  DMAEn;
    u16                 SDAHd;    
    u16                 TSarAddr; //    
    u16                 IC_INRT_MSK;//I2C Interrupt Mask
    u16                 IC_Intr_Clr;//I2C Interrupt register to clear    
}HAL_DW_I2C_DAT_ADAPTER, *PHAL_DW_I2C_DAT_ADAPTER;


typedef struct _HAL_DW_I2C_OP_ {
    BOOL (*HalI2CModuleEn)(VOID *Data);
    BOOL (*HalI2CInitMaster)(VOID *Data);
    BOOL (*HalI2CInitSlave)(VOID *Data);
    BOOL (*HalI2CSetCLK)(VOID *Data);
    VOID (*HalI2CDATWrite)(VOID *Data);
    u8   (*HalI2CDATRead)(VOID *Data);
    VOID (*HalI2CClrIntr)(VOID *Data);
    VOID (*HalI2CEnableIntr)(VOID *Data);
    VOID (*HalI2CClrAllIntr)(VOID *Data);
    VOID (*HalI2CEnableDMA)(VOID *Data);
    VOID (*HalI2CDisableDMA)(VOID *Data);
}HAL_DW_I2C_OP, *PHAL_DW_I2C_OP;

typedef struct _HAL_DW_I2C_ADAPTER_{
    PHAL_DW_I2C_DAT_ADAPTER    pI2CIrqDat;
    PHAL_DW_I2C_OP             pI2CIrqOp;
}HAL_DW_I2C_ADAPTER,*PHAL_DW_I2C_ADAPTER;

VOID HalI2COpInit(
    IN  VOID *Data
);


#endif

