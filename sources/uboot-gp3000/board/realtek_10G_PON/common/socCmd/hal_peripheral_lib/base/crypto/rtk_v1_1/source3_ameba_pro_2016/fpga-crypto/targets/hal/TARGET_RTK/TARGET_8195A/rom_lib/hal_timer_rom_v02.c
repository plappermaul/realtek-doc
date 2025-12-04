/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

/***************** C-Cut Patch functions *********************/
#if CONFIG_CHIP_C_CUT
C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
HalTimerOpInitV02(
    IN  VOID *Data
)
{
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) Data;

    pHalTimerOp->HalGetTimerId = HalGetTimerIdRtl8195a;
    pHalTimerOp->HalTimerInit = HalTimerInitRtl8195aV02;
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountRtl8195aV02;
    pHalTimerOp->HalTimerIrqClear = HalTimerIrqClearRtl8195a;
    pHalTimerOp->HalTimerDis = HalTimerDisRtl8195a;
    pHalTimerOp->HalTimerEn = HalTimerEnRtl8195a;
    pHalTimerOp->HalTimerDumpReg = HalTimerDumpRegRtl8195a;
}

#endif  // end of "#if CONFIG_CHIP_C_CUT"
