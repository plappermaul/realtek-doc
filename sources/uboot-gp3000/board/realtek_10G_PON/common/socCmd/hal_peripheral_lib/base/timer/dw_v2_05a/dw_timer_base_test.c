/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "diag.h"
//#include "dw_timer_base.h"
//#include "dw_timer_test.h"
#include "peripheral.h"
//#include "dw_hal_timer.h"
#include "basic_types.h"
#include "section_config.h"
#include "rtl_utility.h"

HAL_TIMER_OP HalTimerOp;

//extern VOID
//InfraStart(VOID);

VOID
TimerHandleUser(VOID* Data);

VOID
Timer2HandleUser(VOID);

TIMER_ADAPTER TimerAdap;

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
    PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) &HalTimerOp;

	PGTIMER_VERIFY_PARAMETER TimerTestPara = (PGTIMER_VERIFY_PARAMETER)Data;
	TimesupCount = TimerTestPara->TimeoutCount;
	
	u32 loop_num = 0;
	u32 LoopCount = TimerTestPara->LoopCount;

    if(TimerTestPara->timerId > 7)
        return;
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
	printf("LoopCount=%d , TimesupCount=%d , TimerValueUs=%d\n",LoopCount,TimesupCount,TimerTestPara->TimerValueUs);

    HalTimerOpInit((VOID*) (pHalTimerOp));	

	//user-defined count mode
	for(loop_num=0;loop_num<LoopCount;loop_num++)
	{
	    //Set User define mode and isr enable
	    //for (TimerLoopIndex = 0; TimerLoopIndex < TIMER_NUM; TimerLoopIndex++) {
	        TimerAdap.TimerId = TimerTestPara->timerId;
	        TimerAdap.IrqDis = 0;
	        TimerAdap.TimerLoadValueUs = TimerTestPara->TimerValueUs; // if 1 us, value=1
	        TimerAdap.TimerMode = 1;
	        TimerAdap.IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser;
	        TimerAdap.IrqHandle.Priority = 0;
		  	TimerAdap.temp_count = 0;
	    //}

        pHalTimerOp->HalTimerInit(&TimerAdap);
        if (!TimerAdap.IrqDis) {
            if (TimerAdap.IrqHandle.IrqFun != NULL) {
    			
               pHalTimerOp->HalTimerIrqRegister(&TimerAdap);
            }
            else 
                return _FALSE;

        }

        pHalTimerOp->HalTimerEn(TimerTestPara->timerId);

        printf("<====Enable Timer%d\n",TimerTestPara->timerId);
		/*
	    HalTimerInitCommon(&TimerAdap[1]);
	    HalTimerInitCommon(&TimerAdap[2]);
	    HalTimerInitCommon(&TimerAdap[3]);
	    HalTimerInitCommon(&TimerAdap[4]);
	    HalTimerInitCommon(&TimerAdap[5]);
	    HalTimerInitCommon(&TimerAdap[6]);
	    HalTimerInitCommon(&TimerAdap[7]);
	    */
		
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
			//if((TimerAdap[0].temp_count>=TimesupCount) && (TimerAdap[1].temp_count>=TimesupCount) && (TimerAdap[2].temp_count>=TimesupCount) && (TimerAdap[3].temp_count>=TimesupCount) 
			//	&& (TimerAdap[4].temp_count>=TimesupCount) && (TimerAdap[5].temp_count>=TimesupCount) && (TimerAdap[6].temp_count>=TimesupCount) && (TimerAdap[7].temp_count>=TimesupCount))
			if(TimerAdap.temp_count >=TimesupCount)
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
    printf("Timer %d User ISR\n",TimerId);    
    HalTimerIrqClearCommon(TimerId);


	//HalTimerDumpRegRtl8198e(TimerId);
    printf("[%d] Timer%d Count : 0x%x\n", TimerAdap.temp_count,TimerId,
					HalTimerReadCountCommon(TimerId));

	

    if (TimerAdap.temp_count >= (TimesupCount-1)) {
        HalTimerDisCommon(TimerId);
		 printf("<====Disable Timer%d\n",TimerId);
    }
    
    TimerAdap.temp_count++;
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



VOID
WatchdogTestApp(IN VOID *Data)
{
	u32			Mode = 0;
	u32 		TimerValue = 0;
	PWD_VERIFY_PARAMETER pWDTestPara = (PWD_VERIFY_PARAMETER)Data;

	Mode = pWDTestPara->Mode;
	TimerValue = pWDTestPara->TimerValue;


	switch(Mode)
	{
		case WD_TEST_MODE_INIT:
			/* Initialize call-back function */
			HalWatchdogOpInit();
			break;
		case WD_TEST_MODE_START_AND_RESET:
		case WD_TEST_MODE_START_AND_CLEAR_COUNTER:
		case WD_TEST_MODE_STOP:
			break;
		default:
			printf("ERROR: Unsupported Mode!\r\n");
			return;
	}

	HalWatchdogMode(Mode, TimerValue);

	return;
}


VOID
ChangeCpuFreqTestApp(IN VOID *Data)
{
    u32 Mode = 0;
    //u32 TimerValue = 0;
    PCCF_VERIFY_PARAMETER pTestPara = (PCCF_VERIFY_PARAMETER)Data;
    Mode = pTestPara->Mode;

    HalChangeCpuFreqOpInit();
    HalChangeCpuFreqMode(Mode);
}

