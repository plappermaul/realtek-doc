/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtk_hal_pcm.h"

VOID HalPcmOpInit(
    IN  VOID *Data
)
{
    PHAL_PCM_OP pHalPcmOp = (PHAL_PCM_OP) Data;

    pHalPcmOp->HalPcmOnOff = HalPcmOnOffRTKCommon;
    pHalPcmOp->HalPcmInit = HalPcmInitRTKCommon;
    pHalPcmOp->HalPcmSetting = HalPcmSettingRTKCommon;
    pHalPcmOp->HalPcmEn = HalPcmEnRTKCommon;
    pHalPcmOp->HalPcmIsrEnAndDis= HalPcmIsrEnAndDisRTKCommon;
    pHalPcmOp->HalPcmDumpReg= HalPcmDumpRegRTKCommon;
    pHalPcmOp->HalPcm= HalPcmRTKCommon;
}


