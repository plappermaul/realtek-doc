#ifndef _DW_TIMER_TEST_H_
#define _DW_TIMER_TEST_H_


/* For watchdog paramter mode */
typedef enum _WD_VERI_PARA_MODE_ {
	WD_TEST_MODE_INIT						= 0,
	WD_TEST_MODE_START_AND_RESET			= 1,
	WD_TEST_MODE_START_AND_CLEAR_COUNTER	= 2,
	WD_TEST_MODE_STOP						= 3
}WD_VERI_PARA_MODE, *PWD_VERI_PARA_MODE;

typedef struct _GTIMER_VERIFY_PARAMETER_ {
    u32  TimeoutCount;  
    u32  TimerValueUs;  
    u32  LoopCount;
    u32  timerId;
}GTIMER_VERIFY_PARAMETER, *PGTIMER_VERIFY_PARAMETER;

typedef struct _WD_VERIFY_PARAMETER_ {
    u32  Mode;
    u32  TimerValue;
}WD_VERIFY_PARAMETER, *PWD_VERIFY_PARAMETER;

#define TIMER_NUM 8

typedef struct _CCF_VERIFY_PARAMETER_ {
    u32  Mode;
    //u32  TimerValue;
}CCF_VERIFY_PARAMETER, *PCCF_VERIFY_PARAMETER;

#endif

