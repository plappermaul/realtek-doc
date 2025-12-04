/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "hal_timer.h"
#include "section_config.h"

TIMER_ROM_TEXT_SECTION
VOID 
HalTimerOpInit(
    IN  VOID *Data
)
{
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) Data;

    pHalTimerOp->HalGetTimerId = HalGetTimerIdRtl8198e;
    pHalTimerOp->HalTimerInit = HalTimerInitRtl8198e;
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountRtl8198e;
    pHalTimerOp->HalTimerIrqClear = HalTimerIrqClearRtl8198e;
    pHalTimerOp->HalTimerDis = HalTimerDisRtl8198e;
    pHalTimerOp->HalTimerEn = HalTimerEnRtl8198e;
    pHalTimerOp->HalTimerDumpReg = HalTimerDumpRegRtl8198e;
}

