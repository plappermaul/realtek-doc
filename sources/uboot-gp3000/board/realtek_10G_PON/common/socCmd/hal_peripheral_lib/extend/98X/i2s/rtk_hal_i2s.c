/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtk_hal_i2s.h"

VOID HalI2SOpInit(
    IN  VOID *Data
)
{
    PHAL_I2S_OP pHalI2SOp = (PHAL_I2S_OP) Data;

    pHalI2SOp->HalI2SInit        = HalI2SInitRTKCommon;
    pHalI2SOp->HalI2SDeInit      = HalI2SDeInitRTKCommon;
    pHalI2SOp->HalI2STx          = HalI2STxRTKCommon;
    pHalI2SOp->HalI2SRx          = HalI2SRxRTKCommon;
    pHalI2SOp->HalI2SEnable      = HalI2SEnableRTKCommon;
    pHalI2SOp->HalI2SIntrCtrl    = HalI2SIntrCtrlRTKCommon;
    pHalI2SOp->HalI2SReadReg     = HalI2SReadRegRTKCommon;
    pHalI2SOp->HalI2SSetRate     = HalI2SSetRateRTKCommon;
    pHalI2SOp->HalI2SSetWordLen  = HalI2SSetWordLenRTKCommon;
    pHalI2SOp->HalI2SSetChNum    = HalI2SSetChNumRTKCommon;
    pHalI2SOp->HalI2SSetPageNum  = HalI2SSetPageNumRTKCommon;
    pHalI2SOp->HalI2SSetPageSize = HalI2SSetPageSizeRTKCommon;
    pHalI2SOp->HalI2SClrIntr     = HalI2SClrIntrRTKCommon;
    pHalI2SOp->HalI2SClrAllIntr  = HalI2SClrAllIntrRTKCommon;
    pHalI2SOp->HalI2SDMACtrl     = HalI2SDMACtrlRTKCommon;
}


