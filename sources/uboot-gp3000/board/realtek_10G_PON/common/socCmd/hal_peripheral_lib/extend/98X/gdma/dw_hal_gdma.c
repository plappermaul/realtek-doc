/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "dw_hal_gdma.h"

VOID HalGdmaOpInit(
    IN  VOID *Data
)
{
    PHAL_GDMA_OP pHalGdmaOp = (PHAL_GDMA_OP) Data;

    pHalGdmaOp->HalGdmaOnOff = HalGdmaOnOffDWCommon;
    pHalGdmaOp->HalGdamChInit = HalGdamChInitDWCommon;
    pHalGdmaOp->HalGdmaChDis = HalGdmaChDisDWCommon;
    pHalGdmaOp->HalGdmaChEn = HalGdmaChEnDWCommon;
    pHalGdmaOp->HalGdmaChSeting = HalGdmaChSetingDWCommon;
    pHalGdmaOp->HalGdmaChBlockSeting = HalGdmaChBlockSetingDWCommon;
    pHalGdmaOp->HalGdmaChIsrEnAndDis = HalGdmaChIsrEnAndDisDWCommon;
    pHalGdmaOp->HalGdmaChIsrClean = HalGdmaChIsrCleanDWCommon;
    pHalGdmaOp->HalGdmaChCleanAutoSrc = HalGdmaChCleanAutoSrcDWCommon;
    pHalGdmaOp->HalGdmaChCleanAutoDst = HalGdmaChCleanAutoDstDWCommon;
}
