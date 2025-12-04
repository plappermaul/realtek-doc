/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "peripheral.h"
#include "section_config.h"

#include "dw_timer_extend.h"
#include "dw_hal_timer.h"

HAL_WATCHDOG_OP HalWatchdogOp;

//TIMER_ROM_TEXT_SECTION
VOID 
HalTimerOpInit(
    IN  VOID *Data
)
{
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) Data;

    pHalTimerOp->HalGetTimerId = HalGetTimerIdCommon;
    pHalTimerOp->HalTimerInit = HalTimerInitCommon;
    pHalTimerOp->HalTimerReadCount = HalTimerReadCountCommon;
    pHalTimerOp->HalTimerIrqClear = HalTimerIrqClearCommon;
    pHalTimerOp->HalTimerDis = HalTimerDisCommon;
    pHalTimerOp->HalTimerEn = HalTimerEnCommon;
    pHalTimerOp->HalTimerDumpReg = HalTimerDumpRegCommon;
	pHalTimerOp->HalTimerIsDisabled = HalTimerIsDisabledCommon;
	pHalTimerOp->HalTimerIrqRegister = HalTimerIrqRegister98F;
   	pHalTimerOp->HalEnableTimerIP = EnableTimerIP98F;
}


VOID 
HalWatchdogOpInit(VOID)
{
    PHAL_WATCHDOG_OP pHalWatchdogOp = &HalWatchdogOp;
	
	pHalWatchdogOp->HalWatchdogMode = HalWatchdogMode96F;

	return;
}


VOID
HalWatchdogMode(IN u32 Mode, IN u32 TimerValue)
{
	PHAL_WATCHDOG_OP pHalWatchdogOp = &HalWatchdogOp;
	WATCHDOG_ADAPTER WatchdogAdapter;
	
	WatchdogAdapter.Mode = Mode;
	WatchdogAdapter.TimerValue = TimerValue;
	
	pHalWatchdogOp->HalWatchdogMode((void *)&WatchdogAdapter);

	return;
}

HAL_CHANGE_CPU_FREQ_OP HalChangeCpuFreqOp;

VOID 
HalChangeCpuFreqOpInit(VOID)
{
    PHAL_CHANGE_CPU_FREQ_OP pHalChangeCpuFreqOp = &HalChangeCpuFreqOp;
	pHalChangeCpuFreqOp->HalChangeCpuFreqMode = HalChangeCpuFreqMode96F;

	return;
}


VOID
HalChangeCpuFreqMode(IN u32 Mode)
{
	PHAL_CHANGE_CPU_FREQ_OP pHalChangeCpuFreqOp = &HalChangeCpuFreqOp;
	CHANGE_CPU_FREQ_ADAPTER ChangeCpuFreqAdapter;
	
	ChangeCpuFreqAdapter.Mode = Mode;
	pHalChangeCpuFreqOp->HalChangeCpuFreqMode((void *)&ChangeCpuFreqAdapter);

	return;
}

