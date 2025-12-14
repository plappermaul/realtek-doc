/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include "build_info.h"
//#include "osdep_api.h"

#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)
extern HAL_TIMER_OP HalTimerOp;


VOID
HalReInitPlatformTimer(
    VOID
)
{
    TIMER_ADAPTER       TimerAdapter;

    OSC32K_CKGEN_CTRL(ON);
    GTIMER_FCTRL(ON);
    ACTCK_TIMER_CCTRL(ON);
    SLPCK_TIMER_CCTRL(ON);

    //Disable 32K ISO
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_PON_ISO_CTRL, (HAL_READ32(SYSTEM_CTRL_BASE, REG_PON_ISO_CTRL)&(~BIT_ISO_OSC32K_EN)));

    TimerAdapter.IrqDis = ON;
//    TimerAdapter.IrqHandle = (IRQ_FUN)NULL;
    TimerAdapter.TimerId = 1;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 0;
    TimerAdapter.TimerMode = FREE_RUN_MODE;

    HalTimerOpInit((VOID*)(&HalTimerOp));
    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);
    HalTimerOp.HalTimerEn(1);
}


#endif  // #if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)