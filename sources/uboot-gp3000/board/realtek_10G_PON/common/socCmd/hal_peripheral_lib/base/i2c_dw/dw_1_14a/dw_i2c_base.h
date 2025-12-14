/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_I2C_BASE_H_
#define _DW_I2C_BASE_H_

#include "hal_api.h"
#include "dw_i2c_base_bit.h"
#include "dw_i2c_base_reg.h"

VOID
HalI2CWrite32(
    IN  u8      I2CIdx,
    IN  u8      I2CReg,
    IN  u32     I2CVal
);


u32
HalI2CRead32(
    IN  u8      I2CIdx,
    IN  u8      I2CReg
);


#define HAL_I2C_WRITE32(I2CIdx, addr, value)    HalI2CWrite32(I2CIdx,addr,value)
#define HAL_I2C_READ32(I2CIdx, addr)            HalI2CRead32(I2CIdx,addr)

//4 I2C debug output
#define I2C_PREFIX      "RTL8195A[i2c]: "
#define I2C_PREFIX_LVL  "    [i2c_DBG]: "

typedef enum _I2C_DBG_LVL_ {
    HAL_I2C_LVL         =   0x01,
    VERI_I2C_LVL        =   0x02,
}I2C_DBG_LVL,*PI2C_DBG_LVL;

#ifdef CONFIG_DEBUG_LOG
#ifdef CONFIG_DEBUG_LOG_I2C_HAL

    #define DBG_8195A_I2C(...)  do{ \
        _DbgDump("\r"I2C_PREFIX __VA_ARGS__);\
    }while(0)


    #define I2CDBGLVL   0xFF   
    #define DBG_8195A_I2C_LVL(LVL,...)  do{\
            if (LVL&I2CDBGLVL){\
                _DbgDump("\r"I2C_PREFIX_LVL __VA_ARGS__);\
            }\
    }while(0)
#else
    #define DBG_I2C_LOG_PERD    100
    #define DBG_8195A_I2C(...)
    #define DBG_8195A_I2C_LVL(...)
#endif
#endif

//4 I2C params
typedef enum _I2C_SPD_MD_   {
    SS_MODE         =   1,
    FS_MODE         =   2,
    HS_MODE         =   3,
}I2C_SPD_MD,*PI2C_SPD_MD;

typedef enum _I2C_ADDR_MD_   {
    ADDR_7BIT       =   0,
    ADDR_10BIT      =   1,  
}I2C_ADDR_MD,*PI2C_ADDR_MD;

#define I2C_SS_MIN_SCL_HTIME    4000    //the unit is ns.
#define I2C_SS_MIN_SCL_LTIME    4700    //the unit is ns.

#define I2C_FS_MIN_SCL_HTIME    600     //the unit is ns.
#define I2C_FS_MIN_SCL_LTIME    1300    //the unit is ns.

#define I2C_HS_MIN_SCL_HTIME_100    60      //the unit is ns, with bus loading = 100pf
#define I2C_HS_MIN_SCL_LTIME_100    120     //the unit is ns., with bus loading = 100pf

#define I2C_HS_MIN_SCL_HTIME_400    160     //the unit is ns, with bus loading = 400pf
#define I2C_HS_MIN_SCL_LTIME_400    320     //the unit is ns., with bus loading = 400pf


typedef enum _I2C_DEV_ADDR_ {
    I2C0_ADDR       =   0x0010,
    I2C1_ADDR       =   0x0011,
    I2C2_ADDR       =   0x0012,
    I2C3_ADDR       =   0x0013,
}I2C_DEV_ADDR,*PI2C_DEV_ADDR;

//4 I2C Module Selection
typedef enum _I2C_MODULE_SEL_ {
        I2C0_SEL    =   0x0,
        I2C1_SEL    =   0x1,
        I2C2_SEL    =   0x2,
        I2C3_SEL    =   0x3,
}I2C_MODULE_SEL,*PI2C_MODULE_SEL;

//4 I2C Pinmux Selection
typedef enum _I2C0_PINMUX_ {
        I2C0_TO_D   =   0x0,
        I2C0_TO_H   =   0x1,
        I2C0_TO_I   =   0x2,
        I2C0_TO_E   =   0x3,
}I2C0_PINMUX, *PI2C0_PINMUX;

typedef enum _I2C1_PINMUX_ {
        I2C1_TO_C   =   0x0,
        I2C1_TO_H   =   0x1,
        I2C1_TO_J   =   0x2,
}I2C1_PINMUX, *PI2C1_PINMUX;

typedef enum _I2C2_PINMUX_ {
        I2C2_TO_B   =   0x0,
        I2C2_TO_E   =   0x1,
        I2C2_TO_J   =   0x2,
}I2C2_PINMUX, *PI2C2_PINMUX;

typedef enum _I2C3_PINMUX_ {
        I2C3_TO_B   =   0x0,
        I2C3_TO_E23 =   0x1,
        I2C3_TO_E45 =   0x2,
        I2C3_TO_IJ  =   0x3,
}I2C3_PINMUX, *PI2C3_PINMUX;


//4 Hal I2C function prototype
BOOL
HalI2CModuleEnDWCommon(
    IN  VOID    *Data
);

BOOL
HalI2CInitMasterDWCommon(
    IN  VOID    *Data
);


BOOL
HalI2CInitSlaveDWCommon(
    IN  VOID    *Data
);


BOOL
HalI2CSetCLKDWCommon(
    IN  VOID    *Data
);


VOID
HalI2CDATWriteDWCommon(
    IN  VOID    *Data
);

u8
HalI2CDATReadDWCommon(
    IN  VOID    *Data
);

VOID
HalI2CClrIntrDWCommon(
    IN  VOID    *Data
);


VOID
HalI2CClrAllIntrDWCommon(
    IN  VOID    *Data
);

VOID
HalI2CClrAllIntrDWCommon(
    IN  VOID    *Data
);

VOID
HalI2CEnableIntrDWCommon(
    IN  VOID    *Data
);

VOID
HalI2CEnableDMADWCommon(
    IN  VOID    *Data
);

VOID
HalI2CDisableDMADWCommon(
        IN  VOID    *Data
);

#endif // _I2C_BASE_H_

