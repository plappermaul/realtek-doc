/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "diag.h"
#include "rtl8195a_timer.h"
#include "hal_timer.h"

//extern VOID
//InfraStart(VOID);

VOID
TimerHandleUser(VOID);

VOID
Timer2HandleUser(VOID);

TIMER_ADAPTER TimerAdap[8];
u32 temp_count = 0;

#if 0
VOID
InfraStart(VOID)
{
    u32 TimerLoopIndex = 0;
    
    //3 1) Initial Prestart function 

    //3 2) Enter App start function 
    
    //3 3)Enable Schedule
}
#endif
VOID
TimerTestApp(
    IN  u8  TestItem
)
{
    u32 TimerLoopIndex = 0;

    //Set User define mode and isr enable
    for (TimerLoopIndex = 0; TimerLoopIndex<8; TimerLoopIndex++) {
        TimerAdap[TimerLoopIndex].TimerId = TimerLoopIndex;
        TimerAdap[TimerLoopIndex].IrqDis = 0;
        TimerAdap[TimerLoopIndex].TimerLoadValueUs = 1*1000; // 100ms
        TimerAdap[TimerLoopIndex].TimerMode = 0;
        TimerAdap[TimerLoopIndex].IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser;
        TimerAdap[TimerLoopIndex].IrqHandle.Priority = 0;
    }

    TimerAdap[2].IrqHandle.IrqFun = (IRQ_FUN)Timer2HandleUser;
    temp_count = 0;

    HalTimerInitRtl8195a(TimerAdap[0]);

#if 0
    HalTimerInitRtl8195a(TimerAdap[1]);
    HalTimerInitRtl8195a(TimerAdap[2]);
    HalTimerInitRtl8195a(TimerAdap[3]);
    HalTimerInitRtl8195a(TimerAdap[4]);
    HalTimerInitRtl8195a(TimerAdap[5]);
    HalTimerInitRtl8195a(TimerAdap[6]);
    HalTimerInitRtl8195a(TimerAdap[7]);

    for (TimerLoopIndex = 0; TimerLoopIndex<100; TimerLoopIndex++) {
        DBG_8195A_DRIVER("Timer Count Value =========================>\n");

        DBG_8195A_DRIVER("Timer Count 0: 0x%x\n",HalTimerReadCountRtl8195a(0));
        DBG_8195A_DRIVER("Timer Count 1: 0x%x\n",HalTimerReadCountRtl8195a(1));
        DBG_8195A_DRIVER("Timer Count 2: 0x%x\n",HalTimerReadCountRtl8195a(2));
        DBG_8195A_DRIVER("Timer Count 3: 0x%x\n",HalTimerReadCountRtl8195a(3));
        DBG_8195A_DRIVER("Timer Count 4: 0x%x\n",HalTimerReadCountRtl8195a(4));
        DBG_8195A_DRIVER("Timer Count 5: 0x%x\n",HalTimerReadCountRtl8195a(5));
        DBG_8195A_DRIVER("Timer Count 6: 0x%x\n",HalTimerReadCountRtl8195a(6));
        DBG_8195A_DRIVER("Timer Count 7: 0x%x\n",HalTimerReadCountRtl8195a(7));

        DBG_8195A_DRIVER("<========================= Timer Count Value\n");
    }
#endif

}


VOID
TimerHandleUser(VOID)
{
    DBG_8195A_DRIVER("Timer 0 ISR\n");    
    HalTimerIrqClearRtl8195a(0);


    if (temp_count > 20) {
        HalTimerDisRtl8195a(0);
    }
    else {
        HalTimerDumpRegRtl8195a(0);
        DBG_8195A_DRIVER("[%d] Timer Count 0: 0x%x\n", temp_count,
                         HalTimerReadCountRtl8195a(0));
    }

    temp_count++;
}

VOID
Timer2HandleUser(VOID)
{
    DBG_8195A_DRIVER("Timer 2_7 ISR\n");    

    if (temp_count > 20) {
        HalTimerDisRtl8195a(2);
    }
    else {
        HalTimerDumpRegRtl8195a(2);
        DBG_8195A_DRIVER("[%d] Timer Count 0: 0x%x\n", temp_count,
                         HalTimerReadCountRtl8195a(2));
    }

    temp_count++;
}

