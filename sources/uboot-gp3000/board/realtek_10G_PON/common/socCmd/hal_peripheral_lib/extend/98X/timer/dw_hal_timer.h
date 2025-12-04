/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_TIMER_H_
#define _HAL_TIMER_H_
#include "basic_types.h"
#include "hal_platform.h"
//#include "dw_timer_base.h"

//////////////////// Timer
typedef enum _TIMER_MODE_ {
    FREE_RUN_MODE = 0,
    USER_DEFINED = 1
}TIMER_MODE, *PTIMER_MODE;


typedef struct _TIMER_ADAPTER_ {

    u32         TimerLoadValueUs;
    u32         TimerIrqPriority;
	u32			temp_count;
    TIMER_MODE  TimerMode;
    IRQ_HANDLE  IrqHandle;
    u8          TimerId;    
    u8          IrqDis;

}TIMER_ADAPTER, *PTIMER_ADAPTER;


typedef struct _HAL_TIMER_OP_ {
    u32  (*HalGetTimerId)(u32 *TimerId);
    BOOL (*HalTimerInit)(VOID *Data);
    u32  (*HalTimerReadCount)(u32 TimerId);
    VOID (*HalTimerIrqClear)(u32 TimerId);
    VOID (*HalTimerDis)(u32 TimerId);
    VOID (*HalTimerEn)(u32 TimerId);
    VOID (*HalTimerDumpReg)(u32 TimerId);
	BOOL (*HalTimerIsDisabled)(u32 TimerId);
    BOOL (*HalTimerIrqRegister)(VOID *Data);
    VOID (*HalEnableTimerIP)();
}HAL_TIMER_OP, *PHAL_TIMER_OP;


//////////////////// WatchDog
typedef struct _WATCHDOG_ADAPTER_ {
	u32 Mode;
	u32	TimerValue;
}WATCHDOG_ADAPTER, *PWATCHDOG_ADAPTER;


typedef struct _HAL_WATCHDOG_OP_ {
	VOID (*HalWatchdogMode)(VOID *Data);
}HAL_WATCHDOG_OP, *PHAL_WATCHDOG_OP;


//////////////////// Change Cpu Freq

typedef struct _CHANGE_CPU_FREQ_ADAPTER_ {
	u32 Mode;
} CHANGE_CPU_FREQ_ADAPTER, *PCHANGE_CPU_FREQ_ADAPTER;

typedef struct _HAL_CHANGE_CPU_FREQ_OP_ {
	VOID (*HalChangeCpuFreqMode)(VOID *Data);
} HAL_CHANGE_CPU_FREQ_OP, *PHAL_CHANGE_CPU_FREQ_OP;


//////////////////////////////////

VOID HalTimerOpInit(
    IN  VOID *Data
);

VOID HalWatchdogOpInit(VOID);

VOID HalWatchdogMode(IN u32 Mode, IN u32 TimerValue);

VOID HalChangeCpuFreqOpInit(VOID);

VOID HalChangeCpuFreqMode(IN u32 Mode);

/*
extern u32
HalGetTimerIdCommon(
    IN  u32     *TimerID
);

extern BOOL
HalTimerInitCommon(
    IN  VOID    *Data
);

extern u32
HalTimerReadCountCommon(
    IN  u32     TimerId
);

extern VOID
HalTimerIrqClearCommon(
    IN  u32 TimerId
);

extern VOID
HalTimerDisCommon(
    IN  u32 TimerId
);

extern VOID
HalTimerEnCommon(
    IN  u32 TimerId
);

extern VOID
HalTimerDumpRegCommon(
    IN  u32 TimerId
);

extern BOOL
HalTimerIsDisabledCommon(
    IN  u32 TimerId
);
*/

#endif
