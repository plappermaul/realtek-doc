/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "diag.h"
#include "rtl8198e_timer.h"
#include "rtl8198e_timer_test.h"

#include "hal_timer.h"

//extern VOID
//InfraStart(VOID);

VOID
TimerHandleUser(VOID* Data);

VOID
Timer2HandleUser(VOID);

TIMER_ADAPTER TimerAdap[8];

volatile u32 temp_count[8];
u32 TimesupCount = 0;

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
     IN VOID *Data
)
{
    u32 TimerLoopIndex = 0;

	PGTIMER_VERIFY_PARAMETER TimerTestPara = (PGTIMER_VERIFY_PARAMETER)Data;
	TimesupCount = TimerTestPara->TimeoutCount;
	
	u32 loop_num = 0;
	u32 LoopCount = TimerTestPara->LoopCount;
#if 0 	//free-running mode will run for too long, no test for now	(4294967295*32/1000/1000/60/60=38.2 hrs)
	//free-running mode
	for(loop_num=0;loop_num<LoopCount;loop_num++)
	{

	    for (TimerLoopIndex = 0; TimerLoopIndex<8; TimerLoopIndex++) {
	        TimerAdap[TimerLoopIndex].TimerId = TimerLoopIndex;
	        TimerAdap[TimerLoopIndex].IrqDis = 0;
	        TimerAdap[TimerLoopIndex].TimerLoadValueUs = 0x0; 
	        TimerAdap[TimerLoopIndex].TimerMode = 0;
	      //  TimerAdap[TimerLoopIndex].IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser;
	        TimerAdap[TimerLoopIndex].IrqHandle.Priority = 0;
		  	
	    }

	    //TimerAdap[2].IrqHandle.IrqFun = (IRQ_FUN)Timer2HandleUser;
	    temp_count[] = {0};


	    HalTimerInitRtl8198e(&TimerAdap[0]);
	    HalTimerInitRtl8198e(&TimerAdap[1]);
	    HalTimerInitRtl8198e(&TimerAdap[2]);
	    HalTimerInitRtl8198e(&TimerAdap[3]);
	    HalTimerInitRtl8198e(&TimerAdap[4]);
	    HalTimerInitRtl8198e(&TimerAdap[5]);
	    HalTimerInitRtl8198e(&TimerAdap[6]);
	    HalTimerInitRtl8198e(&TimerAdap[7]);
		for()
#if 0
	    for (TimerLoopIndex = 0; TimerLoopIndex<100; TimerLoopIndex++) {
	        DBG_8195A_DRIVER("Timer Count Value =========================>\n");

	        DBG_8195A_DRIVER("Timer Count 0: 0x%x\n",HalTimerReadCountRtl8198e(0));
	        DBG_8195A_DRIVER("Timer Count 1: 0x%x\n",HalTimerReadCountRtl8198e(1));
	        DBG_8195A_DRIVER("Timer Count 2: 0x%x\n",HalTimerReadCountRtl8198e(2));
	        DBG_8195A_DRIVER("Timer Count 3: 0x%x\n",HalTimerReadCountRtl8198e(3));
	        DBG_8195A_DRIVER("Timer Count 4: 0x%x\n",HalTimerReadCountRtl8198e(4));
	        DBG_8195A_DRIVER("Timer Count 5: 0x%x\n",HalTimerReadCountRtl8198e(5));
	        DBG_8195A_DRIVER("Timer Count 6: 0x%x\n",HalTimerReadCountRtl8198e(6));
	        DBG_8195A_DRIVER("Timer Count 7: 0x%x\n",HalTimerReadCountRtl8198e(7));

	        DBG_8195A_DRIVER("<========================= Timer Count Value\n");
	    }
#endif

	}
#endif //free-running mode will run for too long, no test for now	
	//user-defined count mode
	for(loop_num=0;loop_num<LoopCount;loop_num++)
	{
	    //Set User define mode and isr enable
	    for (TimerLoopIndex = 0; TimerLoopIndex<8; TimerLoopIndex++) {
	        TimerAdap[TimerLoopIndex].TimerId = TimerLoopIndex;
	        TimerAdap[TimerLoopIndex].IrqDis = 0;
	        TimerAdap[TimerLoopIndex].TimerLoadValueUs = TimerTestPara->TimerValueUs; // if 1 us, value=1
	        TimerAdap[TimerLoopIndex].TimerMode = 1;
	      //  TimerAdap[TimerLoopIndex].IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser;
	        TimerAdap[TimerLoopIndex].IrqHandle.Priority = 0;
		  	
	    }

	    //TimerAdap[2].IrqHandle.IrqFun = (IRQ_FUN)Timer2HandleUser;
		
		memset(temp_count, 0x0000, 8);


	    HalTimerInitRtl8198e(&TimerAdap[0]);
	    HalTimerInitRtl8198e(&TimerAdap[1]);
	    HalTimerInitRtl8198e(&TimerAdap[2]);
	    HalTimerInitRtl8198e(&TimerAdap[3]);
	    HalTimerInitRtl8198e(&TimerAdap[4]);
	    HalTimerInitRtl8198e(&TimerAdap[5]);
	    HalTimerInitRtl8198e(&TimerAdap[6]);
	    HalTimerInitRtl8198e(&TimerAdap[7]);
		
#if 0
	    for (TimerLoopIndex = 0; TimerLoopIndex<100; TimerLoopIndex++) {
	        DBG_8195A_DRIVER("Timer Count Value =========================>\n");

	        DBG_8195A_DRIVER("Timer Count 0: 0x%x\n",HalTimerReadCountRtl8198e(0));
	        DBG_8195A_DRIVER("Timer Count 1: 0x%x\n",HalTimerReadCountRtl8198e(1));
	        DBG_8195A_DRIVER("Timer Count 2: 0x%x\n",HalTimerReadCountRtl8198e(2));
	        DBG_8195A_DRIVER("Timer Count 3: 0x%x\n",HalTimerReadCountRtl8198e(3));
	        DBG_8195A_DRIVER("Timer Count 4: 0x%x\n",HalTimerReadCountRtl8198e(4));
	        DBG_8195A_DRIVER("Timer Count 5: 0x%x\n",HalTimerReadCountRtl8198e(5));
	        DBG_8195A_DRIVER("Timer Count 6: 0x%x\n",HalTimerReadCountRtl8198e(6));
	        DBG_8195A_DRIVER("Timer Count 7: 0x%x\n",HalTimerReadCountRtl8198e(7));

	        DBG_8195A_DRIVER("<========================= Timer Count Value\n");
	    }
#endif

		while(1)
		{
			if((temp_count[0]>=TimesupCount) && (temp_count[1]>=TimesupCount) && (temp_count[2]>=TimesupCount) && (temp_count[3]>=TimesupCount) 
				&& (temp_count[4]>=TimesupCount) && (temp_count[5]>=TimesupCount) && (temp_count[6]>=TimesupCount) && (temp_count[7]>=TimesupCount))
				break;
			udelay(100);
		}

	}
}


VOID
TimerHandleUser(VOID *Data)
{
	PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
	u8 TimerId = pHalTimerAdap->TimerId;
    DBG_8195A("Timer %d User ISR\n",TimerId);    
    HalTimerIrqClearRtl8198e(TimerId);


	//HalTimerDumpRegRtl8198e(TimerId);
    DBG_8195A_DRIVER("[%d] Timer%8 Count : 0x%x\n", temp_count[TimerId],TimerId,
					HalTimerReadCountRtl8198e(TimerId));

	

    if (temp_count[TimerId] >= (TimesupCount-1)) {
        HalTimerDisRtl8198e(TimerId);
		 DBG_8195A("<====Disable Timer\n");
    }
    
    temp_count[TimerId]++;
}

#if 0
VOID
Timer2HandleUser(VOID)
{
    DBG_8195A_DRIVER("Timer 2_7 ISR\n");    

    if (temp_count > 20) {
        HalTimerDisRtl8198e(2);
    }
    else {
        HalTimerDumpRegRtl8198e(2);
        DBG_8195A_DRIVER("[%d] Timer Count 0: 0x%x\n", temp_count,
                         HalTimerReadCountRtl8198e(2));
    }

    temp_count++;
}
#endif


