/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#if 0
#include "platform_autoconf.h"
#include "basic_types.h"
#include "hal_diag.h"
#include "diag.h"
#include "build_info.h"

static VOID
HalShowBuildInfo(VOID);

extern u32 __bss_end__;
extern BOOL SramTest(
    IN u8 SramType
);

extern VOID 
ROM_BANK0_END_FUN(VOID);

extern VOID 
ROM_BANK1_START_FUN(VOID);

extern VOID 
ROM_BANK1_END_FUN(VOID);

extern 
VOID 
ROM_BANK2_START_FUN(VOID);

extern 
VOID 
ROM_BANK2_END_FUN(VOID);


VOID
HalResetVsr(VOID)
{
    u32 VerifyLoopIndex = 0;
    LOG_UART_ADAPTER    UartAdapter;

    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x5;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;
    
    //Initial Log Uart
    HalLogUartInit(UartAdapter);

    HalShowBuildInfo();

    while (1) {
        VerifyLoopIndex++;
        DBG_8195A_HAL("===============  Verify Num:%d  =================\n", VerifyLoopIndex);
        //4 Fetch ROM instruction Test
        ROM_BANK0_END_FUN();
        ROM_BANK1_START_FUN();
        ROM_BANK1_END_FUN();
        ROM_BANK2_START_FUN();
        ROM_BANK2_END_FUN();

#ifdef CONFIG_RTL8195A_MEMORY_TCM_TEST
        //4 Access TCM SRAM Test
        if (SramTest(0x3)){
            DBG_8195A_HAL("TCM Sram Test: OK\n");
        }
        else {
            DBG_8195A_HAL("TCM Sram Test: Fail\n");            
            break;
        }
        
#else
        //4 Access SRAM Test
        SramTest(0x0);
        SramTest(0x1);
        SramTest(0x2);
#endif
        DBG_8195A_HAL("====================================================\n\n");

    }

    for(;;);
}

static VOID
HalShowBuildInfo(VOID)
{
    DBG_8195A("=========================================================\n\n");
    DBG_8195A("Build Time: "UTS_VERSION"\n");
    DBG_8195A("Build Author: "RTL8195AFW_COMPILE_BY"\n");    
    DBG_8195A("Build Host: "RTL8195AFW_COMPILE_HOST"\n");
    DBG_8195A("Build ToolChain Version: "RTL195AFW_COMPILER"\n\n");    
    DBG_8195A("==========================================================\n");

}



VOID
HalNMIHandler(VOID)
{
   DBG_8195A_HAL("NMI Error!!!!\n");
   for(;;);
}

VOID
HalHardFaultHandler(u32 HardDefaultArg)
{
    DBG_8195A_HAL("Hard Fault Error!!!!\n");

    u32 StackR0;
    u32 StackR1;
    u32 StackR2;
    u32 StackR3;
    u32 StackR12;
    u32 StackLr;
    u32 StackPc;
    u32 StackPsr;
    u32 Bfar;
    u32 Cfsr;
    u32 Hfsr;
    u32 Dfsr;
    u32 Afsr;

    StackR0 = HAL_READ32(HardDefaultArg, 0x0);
    StackR1 = HAL_READ32(HardDefaultArg, 0x4);
    StackR2 = HAL_READ32(HardDefaultArg, 0x8);
    StackR3 = HAL_READ32(HardDefaultArg, 0xC);
    StackR12 = HAL_READ32(HardDefaultArg, 0x10);
    StackLr = HAL_READ32(HardDefaultArg, 0x14);
    StackPc = HAL_READ32(HardDefaultArg, 0x18);
    StackPsr = HAL_READ32(HardDefaultArg, 0x1C);    
    Bfar = HAL_READ32(0xE000ED38, 0x0);
    Cfsr = HAL_READ32(0xE000ED28, 0x0);
    Hfsr = HAL_READ32(0xE000ED2C, 0x0);
    Dfsr = HAL_READ32(0xE000ED30, 0x0);
    Afsr = HAL_READ32(0xE000ED3C, 0x0);

    DBG_8195A_HAL("R0 = 0x%x\n", StackR0);
    DBG_8195A_HAL("R1 = 0x%x\n", StackR1);
    DBG_8195A_HAL("R2 = 0x%x\n", StackR2);
    DBG_8195A_HAL("R3 = 0x%x\n", StackR3);
    DBG_8195A_HAL("R12 = 0x%x\n", StackR12);
    DBG_8195A_HAL("LR = 0x%x\n", StackLr);
    DBG_8195A_HAL("PC = 0x%x\n", StackPc);
    DBG_8195A_HAL("PSR = 0x%x\n", StackPsr);
    DBG_8195A_HAL("BFAR = 0x%x\n", Bfar);
    DBG_8195A_HAL("CFSR = 0x%x\n", Cfsr);
    DBG_8195A_HAL("HFSR = 0x%x\n", Hfsr);
    DBG_8195A_HAL("DFSR = 0x%x\n", Dfsr);
    DBG_8195A_HAL("AFSR = 0x%x\n", Afsr);
    
    for(;;);    
}

VOID
HalMemManageHandler(VOID)
{
    DBG_8195A_HAL("Memory Manage Error!!!!\n");
    for(;;);    
}


VOID
HalBusFaultHandler(VOID)
{
    DBG_8195A_HAL("Bus Fault Error!!!!\n");
    for(;;);    
}


VOID
HalUsageFaultHandler(VOID)
{
    DBG_8195A_HAL("Usage Fault Error!!!!\n");
    for(;;);    
}
#endif
