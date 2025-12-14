/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "dw_hal_i2c.h"

VOID HalI2COpInit(
    IN  VOID *Data
)
{

    PHAL_DW_I2C_OP pHalI2COp = (PHAL_DW_I2C_OP) Data;

    pHalI2COp->HalI2CModuleEn = HalI2CModuleEnDWCommon;
    pHalI2COp->HalI2CInitMaster = HalI2CInitMasterDWCommon;
    pHalI2COp->HalI2CInitSlave = HalI2CInitSlaveDWCommon;
    pHalI2COp->HalI2CSetCLK = HalI2CSetCLKDWCommon;
    pHalI2COp->HalI2CDATWrite = HalI2CDATWriteDWCommon;
    pHalI2COp->HalI2CDATRead = HalI2CDATReadDWCommon;    
    pHalI2COp->HalI2CClrIntr = HalI2CClrIntrDWCommon;
    pHalI2COp->HalI2CClrAllIntr = HalI2CClrAllIntrDWCommon;
    pHalI2COp->HalI2CEnableIntr = HalI2CEnableIntrDWCommon;
    pHalI2COp->HalI2CEnableDMA = HalI2CEnableDMADWCommon;
    pHalI2COp->HalI2CDisableDMA= HalI2CDisableDMADWCommon;
}

