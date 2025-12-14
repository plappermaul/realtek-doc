/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"
//#include "rtk_hal_pwm.h"
#include "section_config.h"
//#include "rtk_pwm_base_test.h"
#include "rtl_utility.h"
#include "rand.h"

/* GTimer*/
//#include "hal_timer.h"
#include "peripheral.h"

#define GTIMER_NUM 8
#define PWM_DUTY_MAX 5
#define PWM_PERIOD_MAX 5
#define GTIMER_TICK_MAX PWM_DUTY_MAX*4

PPWM_VERI_PARA  pPWMVeriAdp;
TIMER_ADAPTER TimerAdap0;
HAL_TIMER_OP HalTimerOpPWM;
HAL_PWM_DAT_ADAPTER PWMAdapt;
HAL_PWM_OP HalPWMOp;
HAL_GTEVENT_DAT_ADAPTER GTEventAdapt;
HAL_GTEVENT_OP HalGTEventOp;

VOID
TimerHandleUser0(VOID* Data);


volatile u32 TimesupCount0 = 0;



VOID
PWMProc(
	IN  VOID    *PWMTestData,
	IN  u1Byte  subItem
)
{
	PPWM_VERI_PARA pPWMVeriPara = (PPWM_VERI_PARA) PWMTestData;
	PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) &HalTimerOpPWM;
	PHAL_PWM_OP pHalPWMOp = (PHAL_PWM_OP) &HalPWMOp;

	u32 pwmTest;
	u32 pwmVeriLoop;
	u32 GTimerSel;
	u32 TimerValueUs;
	u32 PWMSel;
	u32 pwm_duty;
	u32 pwm_period;
	
	pwmTest = pPWMVeriPara->VeriItem;
	pwmVeriLoop = pPWMVeriPara->VeriLoop;
	GTimerSel = pPWMVeriPara->TimerSel;
	TimerValueUs = pPWMVeriPara->TimerValueUs;
	TimesupCount0 =  2;//GTIMER_TICK_MAX;
	PWMSel = pPWMVeriPara->PWM_GTE_Sel;
	
	printf("PWM Test Item = %d\n",pwmTest);    
    printf("PWM Test loop = %d\n",pwmVeriLoop);
	printf("PWM Test PWM = %d\n",PWMSel);
	printf("PWM Test GTimer = %d\n",GTimerSel);
	printf("PWM Test GTimer TimerValueUs= %d\n",TimerValueUs);
	printf("PWM Test GTimer Timesup count= %d\n",TimesupCount0);

	//3 Config GTimer
	if(GTimerSel < GTIMER_NUM){
        TimerAdap0.TimerId = GTimerSel;
        TimerAdap0.IrqDis = 0;
        TimerAdap0.TimerLoadValueUs = TimerValueUs; // if 1 us, value=1
        TimerAdap0.TimerMode = 1;
        TimerAdap0.IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser0;
        TimerAdap0.IrqHandle.Priority = 0;
		TimerAdap0.temp_count = 0;
	}

    else
		printf("GTimer sel is not in the range 0-7.\n");
	

	//Initial GTimer HAL Operation 
    HalTimerOpInit((VOID*) (pHalTimerOp));	

	//3 Config PWM
	HalPWMOpInit((VOID*) (pHalPWMOp));

    if(subItem == 1){
	    pwm_duty = 1;//Rand() % PWM_DUTY_MAX;
    	pwm_period = 2;//Rand() % PWM_PERIOD_MAX;
    }else if(subItem == 2){
        pwm_duty = 2;//Rand() % PWM_DUTY_MAX;
    	pwm_period = 5;//Rand() % PWM_PERIOD_MAX;
    }
    
	if(pwm_duty >= pwm_period)
		pwm_period = pwm_duty*2;

    printf("pwm_duty=%x,pwm_period=%x\n",pwm_duty,pwm_period);
    

    TimesupCount0 =  pwm_period;
    
	PWMAdapt.Idx = PWMSel;
	PWMAdapt.GT_SEL = GTimerSel;
	PWMAdapt.Duty = pwm_duty; // 1
	PWMAdapt.Period = pwm_period; // 2
	PWMAdapt.En = 1;

    pHalTimerOp->HalEnableTimerIP();
    
	pHalPWMOp->HalPWMInit(&PWMAdapt);


	//PWM enable
	pHalPWMOp->HalPWMEn(&PWMAdapt);

	// Timer init and "start timer"
	pHalTimerOp->HalTimerInit(&TimerAdap0);
	
    if (!TimerAdap0.IrqDis) {
        if (TimerAdap0.IrqHandle.IrqFun != NULL) {
            
           pHalTimerOp->HalTimerIrqRegister(&TimerAdap0);
        }
        else 
            return _FALSE;

    }

    pHalTimerOp->HalTimerEn(TimerAdap0.TimerId);


	while(1){

		if(TimerAdap0.temp_count>=TimesupCount0)
				break;
		udelay(100);
	}
	PWMAdapt.En = 0; //disable
	pHalPWMOp->HalPWMEn(&PWMAdapt);

	

}

VOID
GTEventProc(
	IN  VOID    *PWMTestData
)
{
	PPWM_VERI_PARA pPWMVeriPara = (PPWM_VERI_PARA) PWMTestData;
	PHAL_TIMER_OP pHalTimerOp = (PHAL_TIMER_OP) &HalTimerOpPWM;
	PHAL_GTEVENT_OP pHalGTEventOp = (PHAL_GTEVENT_OP) &HalGTEventOp;
	
	u32 GTEventTest;
	u32 GTEventVeriLoop;
	u32 GTimerSel;
	u32 TimerValueUs;
	u32 GTEventSel;
	u32 pwm_duty;
	u32 pwm_period;
	
	GTEventTest = pPWMVeriPara->VeriItem;
	GTEventVeriLoop = pPWMVeriPara->VeriLoop;
	GTimerSel = pPWMVeriPara->TimerSel;
	TimerValueUs = pPWMVeriPara->TimerValueUs;
	TimesupCount0 =	2;
	GTEventSel = pPWMVeriPara->PWM_GTE_Sel;
	
	printf("GTEvent Test Item = %d\n",GTEventTest);    
	printf("GTEvent Test loop = %d\n",GTEventVeriLoop);
	printf("GTEvent Test PWM = %d\n",GTEventSel);
	printf("GTEvent Test GTimer = %d\n",GTimerSel);
	printf("GTEvent Test GTimer TimerValueUs= %d\n",TimerValueUs);
	printf("GTEvent Test GTimer Timesup count= %d\n",TimesupCount0);

	//3 Config GTimer
	if(GTimerSel < GTIMER_NUM){
        TimerAdap0.TimerId = GTimerSel;
        TimerAdap0.IrqDis = 0;
        TimerAdap0.TimerLoadValueUs = TimerValueUs; // if 1 us, value=1
        TimerAdap0.TimerMode = 1;
        TimerAdap0.IrqHandle.IrqFun = (IRQ_FUN) TimerHandleUser0;
        TimerAdap0.IrqHandle.Priority = 0;
		
	}

    else
		printf("GTimer sel is not in the range 0-7.\n");


	//Initial GTimer HAL Operation 
    HalTimerOpInit((VOID*) (pHalTimerOp));

	//3 Config GT Event
	HalGTEventOpInit((VOID*) (pHalGTEventOp));

	u8 pulse_dur = 0;
	for(pulse_dur=0; pulse_dur<4; pulse_dur++)
	{
		TimerAdap0.temp_count = 0;
		GTEventAdapt.Idx = GTEventSel;
		GTEventAdapt.Pulse_dur = pulse_dur;
		GTEventAdapt.Src_Sel = GTimerSel;
		GTEventAdapt.En = 1;
		
		pHalGTEventOp->HalGTEventInit(&GTEventAdapt);

		//GT Event enable
		pHalGTEventOp->HalGTEventEn(&GTEventAdapt);

		
		// Timer init and "start timer"
		pHalTimerOp->HalTimerInit(&TimerAdap0);

		if (!TimerAdap0.IrqDis) {
            if (TimerAdap0.IrqHandle.IrqFun != NULL) {
                
               pHalTimerOp->HalTimerIrqRegister(&TimerAdap0);
            }
            else 
                return _FALSE;

        }

        pHalTimerOp->HalTimerEn(TimerAdap0.TimerId);
		
		while(1)
		{
			if(TimerAdap0.temp_count>=TimesupCount0)
				break;
			udelay(100);
				
		}
	}
	GTEventAdapt.En = 0;
	pHalGTEventOp->HalGTEventEn(&GTEventAdapt);
}

VOID
TimerHandleUser0(VOID *Data)
{
	PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
	u8 TimerId = pHalTimerAdap->TimerId;
    //printf("Timer %d User ISR\n",TimerId);    
   	HalTimerOpPWM.HalTimerIrqClear(TimerId);
		//HalTimerIrqClearRtl8198e(TimerId);


	//HalTimerDumpRegRtl8198e(TimerId);
    printf("[%d] Timer%d Count : 0x%x\n", pHalTimerAdap->temp_count,TimerId,
					HalTimerOpPWM.HalTimerReadCount(TimerId));//HalTimerReadCountRtl8198e(TimerId));

    if (pHalTimerAdap->temp_count >= (TimesupCount0-1)) {
		HalTimerOpPWM.HalTimerDis(TimerId);
        //HalTimerDisRtl8198e(TimerId);
		 printf("<====Disable Timer\n");
    }
    
    pHalTimerAdap->temp_count++;
}



VOID    
PWMTestApp(
    IN  VOID    *PWMTestData
){

	PPWM_VERI_PARA pPWMVerParaTmp = (PPWM_VERI_PARA)PWMTestData;
	pPWMVeriAdp = (PPWM_VERI_PARA)PWMTestData;
	switch(pPWMVerParaTmp->VeriItem)
    {
    	case PWM_TEST:
			PWMProc(pPWMVeriAdp,1);
            PWMProc(pPWMVeriAdp,2);
		break;

		case GTEvent_TEST:
			GTEventProc(pPWMVeriAdp);
		break;

		default:
		break;
	}
	
}
