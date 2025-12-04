/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "platform_autoconf.h"
#include "basic_types.h"
#include "hal_diag.h"
#include "diag.h"
#include "build_info.h"
#include "build_info_rom.h"
#include "hal_vector_table.h"
#include "hal_spi_flash.h"
#include "hal_platform.h"
//#include "osdep_api.h"
#include "section_config.h"


static VOID HalShowBuildInfo(VOID);
#ifdef CONFIG_TIMER_MODULE
static VOID HalInitPlatformTimer(VOID);
u32 HalDelayUs(u32 us);
#endif
static VOID HalBootFlow(VOID);
static VOID HalSpiInit(u8 InitBaudRate);
VOID HalCpuClkConfig(u8 CpuType);
static VOID HalReInitPlatformLogUart(VOID);
static VOID HalInitPlatformLogUart(VOID);

#ifdef CONFIG_TIMER_MODULE
HAL_RAM_BSS_SECTION 
HAL_TIMER_OP        HalTimerOp;
#endif

extern VOID UartLogIrqHandle(VOID* Data);
extern VOID InfraStart(VOID) __attribute__ ((weak));

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __ram_start_table_start__[];

#ifdef CONFIG_BOOT_PROCEDURE
extern VOID RtlConsolInit(u32 Boot ,u32 TBLSz ,VOID *pTBL);
extern VOID RtlConsolRom(u32 WaitCount);
extern VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);
#endif
//_Sema gSema;

extern void *memset( void *s, int c, SIZE_T n );
extern COMMAND_TABLE  UartLogRomCmdTable[];
extern u32 GetRomCmdNum(VOID);

HAL_ROM_TEXT_SECTION
VOID
HalResetVsr(VOID)
{
#ifndef CONFIG_BOOT_PROCEDURE
    PRAM_START_FUNCTION pRamStartFun = (PRAM_START_FUNCTION) __ram_start_table_start__;
#endif
    PRAM_START_FUNCTION pRamWakeupFun = (PRAM_START_FUNCTION) __ram_start_table_start__ + 4;

    //3 Check boot or wake up from power saving
    if ((HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT(31))) {
        pRamWakeupFun->RamStartFun();
    }

    //3 Rom Bss Iitial
    u32 BssLen = (__rom_bss_end__ - __rom_bss_start__);

    memset((void *) __rom_bss_start__, 0, BssLen);

    //2 Need Modify
    VectorTableInitRtl8195A(0x1FFFFFFF);

    //3 Enable devide 0 and non-alignment access
    NVIC_SetCCR();

    //3 Initial Log Uart    
    HalInitPlatformLogUart();

    //3 Initial hardware timer
#ifdef CONFIG_TIMER_MODULE
    HalInitPlatformTimer();
#endif

    HalShowBuildInfo();

//    u8* ptemp;
//    ptemp = RtlZmalloc(76);

//    RtlInitSema(&gSema, 1);    
    
#ifdef CONFIG_BOOT_PROCEDURE
    HalBootFlow();
#else
    pRamStartFun->RamStartFun();
#endif

    for(;;);
}


HAL_ROM_TEXT_SECTION
static VOID
HalBootFlow(
    VOID
)
{
    u8  Value8, CpuType, InitBaudRate;

    // Load Efuse Setting
    Value8 = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_EFUSE_SYSCFG6) & 0xFF000000)
                >> 24);
    CpuType = Value8 & 0x3;
    InitBaudRate = ((Value8 & 0xC)>>2);

    // Make sure InitBaudRate != 0
    if (!InitBaudRate) {
        InitBaudRate +=1;
    }
    
    //3 1) boot check
    DBG_8195A("eFuse value: 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_STATUS0) & BIT_(27));

    if (!(HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_STATUS0) & BIT_(27))) {

         DBG_8195A("Boot from Rom\n");
#ifdef CONFIG_RTL_SIM
        RtlConsolRom(1);//each delay is 100us
#else
        RtlConsolRom(400000);//each delay is 100us
#endif
        if (CpuType) {
            HalCpuClkConfig(CpuType);
            HalReInitPlatformLogUart();
        }
        
#ifdef CONFIG_SPIC_MODULE        
        HalSpiInit(InitBaudRate);           
#endif
    }
    else {
        DBG_8195A("Check boot type form eFuse\n");

        if (CpuType) {
            HalCpuClkConfig(CpuType);
            HalReInitPlatformLogUart();
        }

        switch (Value8 & 0x3) {
            case BOOT_FROM_FLASH:
#ifdef CONFIG_SPIC_MODULE        
                HalSpiInit(InitBaudRate);           
#else
                DBG_8195A("SPIC boor not ready !!!!!!!\n");
                RtlConsolRom(100000);//each delay is 100us
#endif
                break;
            case BOOT_FROM_SDIO:
                DBG_8195A("SDIO boor not ready !!!!!!!\n");
                RtlConsolRom(100000);//each delay is 100us
                break;
            case BOOT_FROM_USB:
                DBG_8195A("USB boor not ready !!!!!!!\n");
                RtlConsolRom(100000);//each delay is 100us
                break;
            default:
                DBG_8195A("No support boot tyep !!!!!!!\n");
                RtlConsolRom(100000);//each delay is 100us
                break;
        }
    }

}

HAL_ROM_TEXT_SECTION
VOID
HalCpuClkConfig(
    IN  u8  CpuType
)
{
    u8 CpuClk=0;
    switch(CpuType) {
        case CLK_25M:
            CpuClk = 3;
            break;
        case CLK_200M:
            CpuClk = 0;
            break;
        case CLK_100M:            
            CpuClk = 1;
            break;
        case CLK_50M:            
            CpuClk = 2;
            break;
        case CLK_12_5M:            
            CpuClk = 4;
            break;
        case CLK_4M:            
            CpuClk = 5;
            break;
    }

    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_CLK_CTRL1, 
                ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_CLK_CTRL1) & (~0x70))
                 |CpuClk));
}


HAL_ROM_TEXT_SECTION
u32
HalGetCpuClk(
    VOID
)
{
    u32  CpuType=0, CpuClk=0;

    CpuType = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_CLK_CTRL1) & (0x70))>>4);

    switch(CpuType) {
        case 3:
            CpuClk = 25000000;
            break;
        case 0:
            CpuClk = 200000000;
            break;
        case 1:            
            CpuClk = 100000000;
            break;
        case 2:            
            CpuClk = 50000000;
            break;
        case 4:            
            CpuClk = 12500000;
            break;
        case 5:            
            CpuClk = 4000000;
            break;
    }

    return CpuClk;
}


HAL_ROM_TEXT_SECTION
static VOID
HalSpiInit(
    IN  u8 InitBaudRate
)
{
    u32 ImageLen = 0, ImageIndex = 0, ImageAddr = 0, SpicImageIndex = 0;
    PRAM_START_FUNCTION pRamStartFun = (PRAM_START_FUNCTION)__ram_start_table_start__;

        
#ifdef CONFIG_SPIC_MODULE 
    DBG_8195A("SPI Initial\n");
    FLASH_FCTRL(ON);
    ACTCK_FLASH_CCTRL(ON);
    SLPCK_FLASH_CCTRL(ON);
    PinCtrl(SPI_FLASH,S0,ON);
    SpicInitRtl8195A(InitBaudRate, SpicOneBitMode);

    ImageLen = HAL_READ32(SPI_FLASH_BASE, 0x10);
    ImageAddr = HAL_READ32(SPI_FLASH_BASE, 0x14);
    DBG_8195A("Image1 length: 0x%x, Image Addr: 0x%x\n",ImageLen, ImageAddr);

    for (SpicImageIndex = 0x20; SpicImageIndex < (ImageLen+0x20); SpicImageIndex = SpicImageIndex + 4) {
        HAL_WRITE32(ImageAddr, ImageIndex,
                    HAL_READ32(SPI_FLASH_BASE, SpicImageIndex));
        ImageIndex += 4;
        }

        //Goto Ram code
    pRamStartFun->RamStartFun();
#endif

    }


HAL_ROM_TEXT_SECTION
static VOID
HalInitPlatformLogUart(
    VOID
)
{
    IRQ_HANDLE          UartIrqHandle;
    LOG_UART_ADAPTER    UartAdapter;
    
    //4 Release log uart reset and clock
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);

    PinCtrl(LOG_UART,S0,ON);

    //4 Register Log Uart Callback function
    UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandle;
    UartIrqHandle.Priority = 0;

    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    //4 Initial Log Uart
    HalLogUartInit(UartAdapter);
    
    //4 Register Isr handle
    InterruptRegister(&UartIrqHandle); 
    
    UartAdapter.IntEnReg = 0x05;

    //4 Initial Log Uart for Interrupt
    HalLogUartInit(UartAdapter);

    //4 initial uart log parameters before any uartlog operation
    RtlConsolInit(ROM_STAGE,GetRomCmdNum(),(VOID*)&UartLogRomCmdTable);// executing boot seq., 
}

HAL_ROM_TEXT_SECTION
static VOID
HalReInitPlatformLogUart(
    VOID
)
{
    LOG_UART_ADAPTER    UartAdapter;

    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x05;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    //4 Initial Log Uart for Interrupt
    HalLogUartInit(UartAdapter);

}


#ifdef CONFIG_TIMER_MODULE
HAL_ROM_TEXT_SECTION
static VOID
HalInitPlatformTimer(
VOID
)
{
    TIMER_ADAPTER       TimerAdapter;

    OSC32K_CKGEN_CTRL(ON);
    GTIMER_FCTRL(ON);
    ACTCK_TIMER_CCTRL(ON);
    SLPCK_TIMER_CCTRL(ON);

    TimerAdapter.IrqDis = ON;
//    TimerAdapter.IrqHandle = (IRQ_FUN)NULL;
    TimerAdapter.TimerId = 1;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 0;
    TimerAdapter.TimerMode = FREE_RUN_MODE;

    HalTimerOpInit((VOID*)(&HalTimerOp));
    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);
    
}

HAL_ROM_TEXT_SECTION
u32 
HalDelayUs(
    IN  u32 us
)
{
    u32 CurrentCount, ExpireCount, WaitCount, StartCount;
    BOOL WakeUpStatus = _FALSE;
    WaitCount = (us/TIMER_TICK_US);

    if (!WaitCount) {
        WaitCount = 1;
    }

    StartCount = HalTimerOp.HalTimerReadCount(1);
    
    while (!WakeUpStatus) {
        CurrentCount = HalTimerOp.HalTimerReadCount(1);
    
        if (StartCount < CurrentCount) {
            ExpireCount =  0xFFFFFFFF + StartCount - CurrentCount;
        }
        else {
            ExpireCount = StartCount - CurrentCount;            
        }
        
        if (WaitCount < ExpireCount) {
            WakeUpStatus = _TRUE;
        }
    };

    return 0;
}



#endif

HAL_ROM_TEXT_SECTION
static VOID
HalShowBuildInfo(VOID)
{
    DBG_8195A("=========================================================\n\n");
    DBG_8195A("Build Time: "UTS_ROM_VERSION"\n");
    DBG_8195A("Build Author: "RTL8195AFW_ROM_COMPILE_BY"\n");    
    DBG_8195A("Build Host: "RTL8195AFW_ROM_COMPILE_HOST"\n");
    DBG_8195A("Build ToolChain Version: "RTL195AFW_ROM_COMPILER"\n\n");    
    DBG_8195A("==========================================================\n");

}



HAL_ROM_TEXT_SECTION
VOID
HalNMIHandler(VOID)
{
   DBG_8195A_HAL("NMI Error!!!!\n");
   for(;;);
}

HAL_ROM_TEXT_SECTION
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
    DBG_8195A_HAL("PriMask 0x%x\n", __get_PRIMASK());
    DBG_8195A_HAL("BasePri 0x%x\n", __get_BASEPRI());
    DBG_8195A_HAL("SVC priority: 0x%02x\n", HAL_READ8(0xE000ED1F, 0));
    DBG_8195A_HAL("PendSVC priority: 0x%02x\n", HAL_READ8(0xE000ED22, 0));
    DBG_8195A_HAL("Systick priority: 0x%02x\n", HAL_READ8(0xE000ED23, 0));
    
    for(;;);    
}


HAL_ROM_TEXT_SECTION
VOID
HalMemManageHandler(VOID)
{
    DBG_8195A_HAL("Memory Manage Error!!!!\n");
    for(;;);    
}

HAL_ROM_TEXT_SECTION
VOID
HalBusFaultHandler(VOID)
{
    DBG_8195A_HAL("Bus Fault Error!!!!\n");
    for(;;);    
}


HAL_ROM_TEXT_SECTION
VOID
HalUsageFaultHandler(VOID)
{
    DBG_8195A_HAL("Usage Fault Error!!!!\n");
    for(;;);    
}

