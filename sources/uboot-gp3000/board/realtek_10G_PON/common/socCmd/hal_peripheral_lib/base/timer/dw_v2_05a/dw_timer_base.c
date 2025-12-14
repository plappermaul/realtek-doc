/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h" 
#include "section_config.h"
#include "hal_platform.h"
#include "dw_timer_base.h"
#include "diag.h"


#define MAX_TIMER_VECTOR_TABLE_NUM                  6

//SECTION_RAM_TIMER2TO7_VECTOR_TABLE 
IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM];

//SECTION_RAM_BSS_TIMER_RECORDER_TABLE
u32 gTimerRecord = 0;


//=================   Function Declare ===========================

extern BOOL
HalTimerIrqRegisterCommon(
    IN  VOID    *Data
);


VOID
HalTimerIrqHandle(
    IN  VOID    *Data
);

extern VOID
TimerHandleUser(VOID    *Data);

//========================================================
//TIMER_ROM_TEXT_SECTION
u32
HalGetTimerIdCommon(
    IN  u32     *TimerID
)
{
    u32 SearchIndex = 0;
    BOOLEAN SearchResult = _FALSE;
    for (SearchIndex=2; SearchIndex<8; SearchIndex++) {
        if (!(gTimerRecord & BIT(SearchIndex))){
            gTimerRecord |= BIT(SearchIndex);
            *TimerID = SearchIndex;
            SearchResult = _TRUE;
            break;
        }
    }

    return SearchResult;
}

//TIMER_ROM_TEXT_SECTION
BOOL
HalTimerInitCommon(
    IN  VOID    *Data
)
{
    
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    u32 ControlReg = 0, LoadCount = 0;

    ControlReg = (BIT_CTRL_TIMER_CTL_TIMER_MODE(pHalTimerAdap->TimerMode) | BIT_CTRL_TIMER_CTL_INT_MASK(pHalTimerAdap->IrqDis));

    if (pHalTimerAdap->TimerMode) {
        //User-defined Mode
        
				
       if (pHalTimerAdap->TimerLoadValueUs < TIMER_TICK_US) {
            printf("Warning : Timer Load Count = 1!!!!!!!\n");
            LoadCount = 1;
        }
        else {
            LoadCount = pHalTimerAdap->TimerLoadValueUs/31; //floating point computing is not working here, so we fixed the number in code. it should be TimerLoadValueUs/(1/25)
            printf("load count = %d\n",LoadCount);
        }
 
    } 
    else {
        LoadCount = 0xFFFFFFFF;
    }

    
    //4 1) Config Timer Setting
    /*
        set TimerControlReg
        0: Timer enable (0,disable; 1,enable)
        1: Timer Mode (0, free-running mode; 1, user-defined count mode)
        2: Timer Interrupt Mask (0, not masked; 1,masked)
        */
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + REG_TIMER_CTL_REG), 
                        ControlReg);

    // set TimerLoadCount Register
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + REG_TIMER_LOAD_COUNT),
                        LoadCount);


#if 0 //move out from init function
    //4  2) Setting Timer IRQ
    if (!pHalTimerAdap->IrqDis) {
        if (pHalTimerAdap->IrqHandle.IrqFun != NULL) {
			
            //4 2.1) Initial TimerIRQHandle
            if (!HalTimerIrqRegisterCommon(pHalTimerAdap)) {
                return _FALSE;
            }
            //4 2.2) Enable TimerIRQ for Platform
            InterruptEn((PIRQ_HANDLE)&pHalTimerAdap->IrqHandle);
			
        }
        else {
			
            return _FALSE;
        }

    }
    //DBG_8195A("Enable Timer====>\n");
    
    //4 4) Enable Timer
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + REG_TIMER_CTL_REG), 
                        (ControlReg|BIT_TIMER_CTL_TIMER_ENABLE));

    printf("<====Enable Timer%d\n",pHalTimerAdap->TimerId);
#endif    
    //HalTimerDumpRegRtl8195a(pHalTimerAdap->TimerId);

    return _TRUE;
}

//TIMER_ROM_TEXT_SECTION
VOID
HalTimerDisCommon(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + REG_TIMER_CTL_REG), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId + REG_TIMER_CTL_REG) & (~BIT_TIMER_CTL_TIMER_ENABLE));    
}

//TIMER_ROM_TEXT_SECTION
VOID
HalTimerEnCommon(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + REG_TIMER_CTL_REG), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId+REG_TIMER_CTL_REG) | (BIT_TIMER_CTL_TIMER_ENABLE));
}
BOOL
HalTimerIsDisabledCommon(
    IN  u32 TimerId
)
{
	printf("TimerId=%d,TIMER_CTL_REG_OFF=%x\n",TimerId,REG_TIMER_CTL_REG);
    /*u32 timer_ctrl = HAL_TIMER_READ32(TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF);
	if(timer_ctrl & BIT0)
		return _FALSE;
	else
		return _TRUE;
	*/
	return _TRUE;
}

//TIMER_ROM_TEXT_SECTION
VOID
HalTimerIrqHandle(
    IN  VOID    *Data
)
{
    u32 TimerIrqStatus = 0, CheckIndex;

	PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
		
    DBG_8195A("Timer ISR\n");
    
    TimerIrqStatus = HAL_TIMER_READ32(REG_TIMERS_INT_STATUS);

    DBG_8195A("TimerIrqStatus: 0x%x\n", TimerIrqStatus);
    
//    for (CheckIndex = 0; CheckIndex<8; CheckIndex++) {
	CheckIndex = pHalTimerAdap->TimerId;
        //3 Check IRQ status bit and Timer X IRQ enable bit
        if ((TimerIrqStatus & BIT_(CheckIndex)) && 
            (HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + REG_TIMER_CTL_REG)&
            BIT_TIMER_CTL_TIMER_ENABLE)) {

            //DBG_8195A("Execute Sub rout\n");    
            //3  Execute Timer callback function
            //Timer2To7VectorTable[CheckIndex-2](NULL);
			TimerHandleUser(pHalTimerAdap);
			
						
            //3 Clear Timer ISR
            HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + REG_TIMER_EOI);
			
           	
        }
//    }

}


//TIMER_ROM_TEXT_SECTION
u32
HalTimerReadCountCommon(
    IN  u32 TimerId
)
{
    return HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + 
                            REG_TIMER_CURRENT_VAL);
}

//TIMER_ROM_TEXT_SECTION
VOID
HalTimerIrqClearCommon(
    IN  u32 TimerId
)
{
    HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + REG_TIMER_EOI);
}

//TIMER_ROM_TEXT_SECTION
VOID
HalTimerDumpRegCommon(
    IN  u32 TimerId
)
{
    DBG_8195A_DRIVER("Control Reg: 0x%x\n", 
                    HAL_TIMER_READ32((TIMER_INTERVAL*TimerId + REG_TIMER_CTL_REG)));
    DBG_8195A_DRIVER("Load Count Reg: 0x%x\n", 
                    HAL_TIMER_READ32((TIMER_INTERVAL*TimerId + REG_TIMER_LOAD_COUNT)));
    
}
