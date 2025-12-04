/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"

#if 1//CONFIG_CHIP_C_CUT
C_CUT_ROM_DATA_SECTION
u32 Timer2To7HandlerData[MAX_TIMER_VECTOR_TABLE_NUM];

extern IRQ_FUN Timer2To7VectorTable[MAX_TIMER_VECTOR_TABLE_NUM];
extern u32 gTimerRecord;

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
HalTimerIrq2To7HandleV02(
    IN  VOID    *Data
)
{
    u32 TimerIrqStatus = 0, CheckIndex;
    IRQ_FUN pHandler;

    TimerIrqStatus = HAL_TIMER_READ32(TIMERS_INT_STATUS_OFF);

    DBG_TIMER_INFO("HalTimerIrq2To7HandleV02:TimerIrqStatus: 0x%x\n", TimerIrqStatus);

    for (CheckIndex = 2; CheckIndex<8; CheckIndex++) {

        //3 Check IRQ status bit and Timer X IRQ enable bit
        if ((TimerIrqStatus & BIT_(CheckIndex)) &&
                (HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_CTL_REG_OFF) & BIT0)) {
            //3 Execute Timer callback function
            pHandler = Timer2To7VectorTable[CheckIndex-2];
            if (pHandler != NULL) {
                pHandler(Timer2To7HandlerData[CheckIndex-2]);
            }
            //3 Clear Timer ISR
            HAL_TIMER_READ32(TIMER_INTERVAL*CheckIndex + TIMER_EOI_OFF);
        }
    }
}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
HAL_Status
HalTimerIrqRegisterRtl8195aV02(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    IRQ_HANDLE          TimerIrqHandle;
    IRQ_FUN             BackUpIrqFun = NULL;

    if (pHalTimerAdap->TimerId > 7) {
        DBG_TIMER_ERR("HalTimerIrqRegisterRtl8195aV02: No Support Timer ID %d!\r\n", pHalTimerAdap->TimerId);
        return HAL_ERR_PARA;
    } else {
        if (pHalTimerAdap->TimerId > 1) {

            TimerIrqHandle.IrqNum = TIMER2_7_IRQ;
            TimerIrqHandle.IrqFun = (IRQ_FUN) HalTimerIrq2To7HandleV02;

            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] =
                (IRQ_FUN) pHalTimerAdap->IrqHandle.IrqFun;
            Timer2To7HandlerData[pHalTimerAdap->TimerId-2] =
                (IRQ_FUN) pHalTimerAdap->IrqHandle.Data;
        } else {
            TimerIrqHandle.IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);
            TimerIrqHandle.IrqFun = (IRQ_FUN) pHalTimerAdap->IrqHandle.IrqFun;
        }
        TimerIrqHandle.Data = (u32)pHalTimerAdap;
        InterruptRegister(&TimerIrqHandle);
    }

    return HAL_OK;
}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
HAL_Status
HalTimerInitRtl8195aV02(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    HAL_Status ret=HAL_OK;
    u32 ControlReg = 0, LoadCount = 0;

    if ((gTimerRecord & (1<<pHalTimerAdap->TimerId)) != 0) {
        DBG_TIMER_ERR ("HalTimerInitRtl8195aV02:Error! Timer %d is occupied!\r\n", pHalTimerAdap->TimerId);
        return HAL_BUSY;
    }

    ControlReg = ((u32)pHalTimerAdap->TimerMode<<1)|((u32)pHalTimerAdap->IrqDis<<2);

    if (pHalTimerAdap->TimerMode) {
        //User-defined Mode
        if (pHalTimerAdap->TimerLoadValueUs < TIMER_TICK_US) {
            DBG_TIMER_WARN("HalTimerInitRtl8195aV02 : Timer Load Count = 1!\r\n");
            LoadCount = 1;
        } else {
            LoadCount = (pHalTimerAdap->TimerLoadValueUs-(TIMER_TICK_US>>1))/TIMER_TICK_US + 1;  // to get the most closed integer
        }
    } else {
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
            ret = HalTimerIrqRegisterRtl8195aV02(pHalTimerAdap);
            if (HAL_OK != ret) {
                DBG_TIMER_ERR ("HalTimerInitRtl8195aV02: Timer %d Register IRQ Err!\r\n", pHalTimerAdap->TimerId);
                return ret;
            }
            //4 2.2) Enable TimerIRQ for Platform
            InterruptEn((PIRQ_HANDLE)&pHalTimerAdap->IrqHandle);

        } else {
            DBG_TIMER_ERR ("HalTimerInitRtl8195aV02: Timer %d ISR Handler is NULL!\r\n", pHalTimerAdap->TimerId);
            return HAL_ERR_PARA;
        }

    }

    //4 4) Enable Timer
//    HAL_TIMER_WRITE32((TIMER_INTERVAL*pHalTimerAdap->TimerId + TIMER_CTL_REG_OFF),
//                        (ControlReg|0x1));

    gTimerRecord |= (1<<pHalTimerAdap->TimerId);

    return ret;
}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
u32
HalTimerReadCountRtl8195aV02(
    IN  u32 TimerId
)
{
    u32 TimerCountOld;
    u32 TimerCountNew;
    u32 TimerRDCnt;

    TimerRDCnt = 0;
    TimerCountOld = HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + TIMER_CURRENT_VAL_OFF);
    while(1) {
        TimerCountNew = HAL_TIMER_READ32(TimerId*TIMER_INTERVAL + TIMER_CURRENT_VAL_OFF);

        if (TimerCountOld == TimerCountNew) {
            return (u32)TimerCountOld;
        } else {
            TimerRDCnt++;
            TimerCountOld = TimerCountNew;

            if (TimerRDCnt >= 2) {
                return (u32)TimerCountOld;
            }
        }
    }
}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
HalTimerReLoadRtl8195aV02(
    IN  u32 TimerId,
    IN  u32 LoadUs
)
{
    u32 LoadCount = 0;


    //User-defined Mode
    if (LoadUs < TIMER_TICK_US) {
        DBG_TIMER_WARN("HalTimerReLoadRtl8195a Warning : Timer Load Count = 1!\n");
        LoadCount = 1;
    } else {
        LoadCount = (LoadUs-(TIMER_TICK_US>>1))/TIMER_TICK_US + 1;  // to get the most closed integer
    }

    // set TimerLoadCount Register
    HAL_TIMER_WRITE32((TIMER_INTERVAL*TimerId + TIMER_LOAD_COUNT_OFF),
                      LoadCount);

}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
HAL_Status
HalTimerIrqUnRegisterRtl8195aV02(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER      pHalTimerAdap = (PTIMER_ADAPTER) Data;
    PIRQ_HANDLE         pTimerIrqHandle;
    u32 i;

    pTimerIrqHandle = &pHalTimerAdap->IrqHandle;

    if (pHalTimerAdap->TimerId > 7) {
        DBG_TIMER_ERR("HalTimerIrqUnRegisterRtl8195aV02:Error: No Support Timer ID!\n");
        return HAL_ERR_PARA;
    } else {
        if (pHalTimerAdap->TimerId > 1) {
            pTimerIrqHandle->IrqNum = TIMER2_7_IRQ;
            Timer2To7VectorTable[pHalTimerAdap->TimerId-2] = NULL;
            for (i=0; i<MAX_TIMER_VECTOR_TABLE_NUM; i++) {
                if (Timer2To7VectorTable[i] != NULL) {
                    break;
                }
            }

            if (i == MAX_TIMER_VECTOR_TABLE_NUM) {
                // All timer UnRegister Interrupt
                InterruptDis((PIRQ_HANDLE)&pHalTimerAdap->IrqHandle);
                InterruptUnRegister(pTimerIrqHandle);
            }
        } else {
            pTimerIrqHandle->IrqNum = (pHalTimerAdap->TimerId ? TIMER1_IRQ : TIMER0_IRQ);
            InterruptUnRegister(pTimerIrqHandle);
        }

    }

    return HAL_OK;
}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
HalTimerDeInitRtl8195aV02(
    IN  VOID    *Data
)
{
    PTIMER_ADAPTER pHalTimerAdap = (PTIMER_ADAPTER) Data;
    u32 timer_id;

    timer_id = pHalTimerAdap->TimerId;
    HalTimerDisRtl8195a (timer_id);
    if (!pHalTimerAdap->IrqDis) {
        if (pHalTimerAdap->IrqHandle.IrqFun != NULL) {
            HalTimerIrqUnRegisterRtl8195aV02(pHalTimerAdap);
        }
    }

    gTimerRecord &= ~(1<<pHalTimerAdap->TimerId);
}

#endif
// end of "#if CONFIG_CHIP_C_CUT"
