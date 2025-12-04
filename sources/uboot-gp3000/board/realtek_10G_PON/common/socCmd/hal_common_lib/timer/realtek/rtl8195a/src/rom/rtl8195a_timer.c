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
#include "rtl8195a_timer.h"
#include "diag.h"

#define MAX_TIMER_VECTOR_TABLE_NUM                  6

SECTION_RAM_TIMER2TO7_VECTOR_TABLE 
IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM];

SECTION_RAM_BSS_TIMER_RECORDER_TABLE
u32 gTimerRecord = 0;


//=================   Function Declare ===========================

static BOOL
HalTimerIrqRegisterRtl8195a(
    IN  VOID    *Data
);


VOID
HalTimerIrq2To7Handle(
    IN  VOID    *Data
);

//========================================================
TIMER_ROM_TEXT_SECTION
u32
HalGetTimerIdRtl8195a(
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

TIMER_ROM_TEXT_SECTION
BOOL
HalTimerInitRtl8195a(
    IN  VOID    *Data
)
{
    
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    u32 ControlReg = 0, LoadCount = 0;

    ControlReg = ((u32)pHalTimerAdap->TimerMode<<1)|((u32)pHalTimerAdap->IrqDis<<2);

    if (pHalTimerAdap->TimerMode) {
        //User-defined Mode
        if (pHalTimerAdap->TimerLoadValueUs < TIMER_TICK_US) {
            DBG_8195A_DRIVER("Warning : Timer Load Count = 1!!!!!!!\n");
            LoadCount = 1;
        }
        else {
            LoadCount = pHalTimerAdap->TimerLoadValueUs/TIMER_TICK_US;
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
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_CTL_REG_OFF), 
                        ControlReg);

    // set TimerLoadCount Register
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_LOAD_COUNT_OFF),
                        LoadCount);

    //4  2) Setting Timer IRQ
    if (!pHalTimerAdap->IrqDis) {
        if (pHalTimerAdap->IrqHandle.IrqFun != NULL) {
            //4 2.1) Initial TimerIRQHandle
            if (!HalTimerIrqRegisterRtl8195a(pHalTimerAdap)) {
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
    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_CTL_REG_OFF), 
                        (ControlReg|0x1));

    //DBG_8195A("<====Enable Timer\n");
    //HalTimerDumpRegRtl8195a(pHalTimerAdap->TimerId);

    return _TRUE;
}

TIMER_ROM_TEXT_SECTION
VOID
HalTimerDisRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId) & (~BIT0));    
}

TIMER_ROM_TEXT_SECTION
VOID
HalTimerEnRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF), 
                        HAL_TIMER_READ32(TIMER_INTERVAL*TimerId) | (BIT0));
}



TIMER_ROM_TEXT_SECTION
static BOOL
HalTimerIrqRegisterRtl8195a(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    PIRQ_HANDLE         pTimerIrqHandle;
    IRQ_FUN             BackUpIrqFun = NULL;
    
    pTimerIrqHandle = &pHalTimerAdap->IrqHandle;

    if (pHalTimerAdap->TimerId > 7) {
        DBG_8195A_DRIVER("Error: No Support Timer ID!!!!!!!\n");
        return _FALSE;
    }
    else {
        if (pHalTimerAdap->TimerId > 1) {

            pTimerIrqHandle->IrqNum = TIMER2_7_IRQ;

            BackUpIrqFun = (IRQ_FUN) pTimerIrqHandle->IrqFun;

            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] = 
                (IRQ_FUN) pTimerIrqHandle->IrqFun;
            pTimerIrqHandle->IrqFun = (IRQ_FUN) HalTimerIrq2To7Handle;

        }
        else {

            pTimerIrqHandle->IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);

        }
        pTimerIrqHandle->Data = (u32)pHalTimerAdap;
        InterruptRegister(pTimerIrqHandle);

        if (BackUpIrqFun) {
            pTimerIrqHandle->IrqFun = BackUpIrqFun;
        }
        
    }
    
    return _TRUE;
}


TIMER_ROM_TEXT_SECTION
VOID
HalTimerIrq2To7Handle(
    IN  VOID    *Data
)
{
    u32 TimerIrqStatus = 0, CheckIndex;

    DBG_8195A("Timer2_7 ISR\n");
    
    TimerIrqStatus = HAL_TIMER_READ32(TIMERS_INT_STATUS_OFF);

    DBG_8195A("TimerIrqStatus: 0x%x\n", TimerIrqStatus);
    
    for (CheckIndex = 2; CheckIndex<8; CheckIndex++) {

        //3 Check IRQ status bit and Timer X IRQ enable bit
        if ((TimerIrqStatus & BIT_(CheckIndex)) && 
            (HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_CTL_REG_OFF)&
            BIT0)) {

            DBG_8195A("Execute Sub rout\n");    
            //3 Execute Timer callback function
            Timer2To7VectorTable[CheckIndex-2](NULL);

            //3 Clear Timer ISR
            HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_EOI_OFF);
            
        }
    }

}


TIMER_ROM_TEXT_SECTION
u32
HalTimerReadCountRtl8195a(
    IN  u32 TimerId
)
{
    return HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + 
                            TIMER_CURRENT_VAL_OFF);
}

TIMER_ROM_TEXT_SECTION
VOID
HalTimerIrqClearRtl8195a(
    IN  u32 TimerId
)
{
    HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + TIMER_EOI_OFF);
}

TIMER_ROM_TEXT_SECTION
VOID
HalTimerDumpRegRtl8195a(
    IN  u32 TimerId
)
{
    DBG_8195A_DRIVER("Control Reg: 0x%x\n", 
                    HAL_TIMER_READ32((TIMER_INTERVAL*TimerId + TIMER_CTL_REG_OFF)));
    DBG_8195A_DRIVER("Load Count Reg: 0x%x\n", 
                    HAL_TIMER_READ32((TIMER_INTERVAL*TimerId + TIMER_LOAD_COUNT_OFF)));
    
}
