/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

extern "C" {

#include "rtl8195a.h"
#include "build_info.h"


//3 Monitor App Function
    extern VOID RtlConsolInitRam(u32 Boot, u32 TBLSz, VOID *pTBL);

    extern COMMAND_TABLE    UartLogRamCmdTable[];
    extern u32 GetRamCmdNum(VOID);
    extern VOID UartLogIrqHandleRam(VOID * Data);

}


void app_start(int, char**)
{
    DiagPrintf("Hello World!\r\n");
}

extern "C" {

    static VOID
    ReRegisterPlatformLogUart(
        VOID
    )
    {
        IRQ_HANDLE          UartIrqHandle;

        //4 Register Log Uart Callback function
        UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
        UartIrqHandle.IrqNum = UART_LOG_IRQ;
        UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandleRam;
        UartIrqHandle.Priority = 5;


        //4 Register Isr handle
        InterruptUnRegister(&UartIrqHandle);
        InterruptRegister(&UartIrqHandle);
        RtlConsolInitRam((u32)RAM_STAGE,(u32)GetRamCmdNum(),(VOID*)&UartLogRamCmdTable);
    }

} // extern C

//#include "mbed/minar/minar.h"

//default main
extern "C" int main(void)
{
#if 1
    ReRegisterPlatformLogUart();
    // init serial if it has not been invoked prior main
#else
    minar::Scheduler::postCallback(
        mbed::util::FunctionPointer2<void, int, char**>(&app_start).bind(0, NULL)
    );
    return minar::Scheduler::start();
#endif
    return 0;
}





extern "C" {
    extern HAL_VECTOR_FUN  NewVectorTable[];

    extern	void SVC_Handler (void);
    extern	void PendSV_Handler (void);
    extern	void SysTick_Handler (void);


    VOID ShowRamBuildInfo(VOID)
    {
        /*
        DBG_8195A("=========================================================\n\n");
        //DBG_8195A("Build Time: "UTS_VERSION"\n");
        DBG_8195A("Build Time: "RTL8195AFW_COMPILE_TIME"\n");
        DBG_8195A("Build Author: "RTL8195AFW_COMPILE_BY"\n");
        DBG_8195A("Build Host: "RTL8195AFW_COMPILE_HOST"\n");
        DBG_8195A("Build ToolChain Version: "RTL195AFW_COMPILER"\n\n");
        DBG_8195A("=========================================================\n");
        */
    }

    static inline void vector_table_setting_for_os(void)
    {
        //4 Initial SVC
        NewVectorTable[11] =
            (HAL_VECTOR_FUN)SVC_Handler;

        //4 Initial Pend SVC
        NewVectorTable[14] =
            (HAL_VECTOR_FUN)PendSV_Handler;

        //4 Initial System Tick fun
        NewVectorTable[15] =
            (HAL_VECTOR_FUN)SysTick_Handler;
    }


// The Main App entry point
    void _AppStart(void)
    {
#if 0 // to call main function directly
        __asm (
            "ldr   r0, =_start\n"
            "bx    r0\n"
        );

        for(;;);
#else
        vector_table_setting_for_os();

        __asm (
            "ldr   r0, =SystemInit\n"
            "blx   r0\n"
            "ldr   r0, =_start\n"
            "bx    r0\n"
        );
#endif
    }

} // extern C


