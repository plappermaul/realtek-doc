/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "ca_hal_rtc.h"

VOID HalRTCOpInit(
    IN  VOID *Data
)
{

    PHAL_CA_RTC_OP pHalRTCOp = (PHAL_CA_RTC_OP) Data;

    pHalRTCOp->HalRTCInit = HalRTCInitCACommon;
    pHalRTCOp->HalRTCdeInit = HalRTCdeInitCACommon;        
    pHalRTCOp->HalRTCClrIntr = HalRTCClrIntrCACommon;
    pHalRTCOp->HalRTCEnableIntr = HalRTCEnableIntrCACommon;
}

