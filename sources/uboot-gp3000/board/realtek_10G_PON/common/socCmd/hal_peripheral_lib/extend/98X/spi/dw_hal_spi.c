/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "dw_hal_spi.h"

VOID
HalSsiOpInit(
        IN VOID *Data
        )
{
    PHAL_SSI_OP pHalSsiOp = (PHAL_SSI_OP) Data;

    pHalSsiOp->HalSsiPinmuxEnable       = HalSsiPinmuxEnableDWCommon;
    pHalSsiOp->HalSsiEnable             = HalSsiEnableDWCommon;
    pHalSsiOp->HalSsiDisable            = HalSsiDisableDWCommon;
    pHalSsiOp->HalSsiInit               = HalSsiInitDWCommon;
    pHalSsiOp->HalSsiSetSclkPolarity    = HalSsiSetSclkPolarityDWCommon;
    pHalSsiOp->HalSsiSetSclkPhase       = HalSsiSetSclkPhaseDWCommon;
    pHalSsiOp->HalSsiWriter             = HalSsiWriterDWCommon;
    pHalSsiOp->HalSsiReader             = HalSsiReaderDWCommon;
    pHalSsiOp->HalSsiGetRxFifoLevel     = HalSsiGetRxFifoLevelDWCommon;
    pHalSsiOp->HalSsiGetTxFifoLevel     = HalSsiGetTxFifoLevelDWCommon;
    pHalSsiOp->HalSsiGetStatus          = HalSsiGetStatusDWCommon;
    pHalSsiOp->HalSsiGetInterruptStatus = HalSsiGetInterruptStatusDWCommon;
}

