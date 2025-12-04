#ifndef _RTL8198E_TIMER_TEST_H_
#define _RTL8198E_TIMER_TEST_H_
typedef struct _GTIMER_VERIFY_PARAMETER_ {
    u32  TimeoutCount;  
    u32  TimerValueUs;  
    u32  LoopCount;   
}GTIMER_VERIFY_PARAMETER, *PGTIMER_VERIFY_PARAMETER;
#endif