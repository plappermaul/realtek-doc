/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "basic_types.h"
#include "build_info.h"
#include "diag.h"
#include "hal_api.h"
#include "diag.h"
#include "hal_platform.h"
#include "section_config.h"



extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __ram_start_table_start__[];
extern VOID UartLogIrqHandleRam(VOID* Data);
#ifdef CONFIG_BOOT_PROCEDURE
extern VOID RtlConsolInit(u32 Boot ,u32 TBLSz ,VOID *pTBL);
extern VOID RtlConsolRom(u32 WaitCount);
extern VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);
#endif
extern VOID SpicLoadInitParaFromClockRtl8195A(u8 CpuClkMode, u8 BaudRate, PSPIC_INIT_PARA pSpicInitPara);

extern void *memset( void *s, int c, SIZE_T n );
extern COMMAND_TABLE  UartLogRomCmdTable[];
extern u32 GetRomCmdNum(VOID);
#ifdef CONFIG_TIMER_MODULE
extern HAL_TIMER_OP        HalTimerOp;
#endif

extern VOID InfraStart(VOID) __attribute__ ((weak));
extern u32 
SdrControllerInit(
VOID
);

void RtlBootToSram(
    VOID
);

PATCH_START_RAM_FUN_SECTION
RAM_START_FUNCTION gBoot2SRAMFun1={RtlBootToSram};


PATCH_START_RAM_FUN_SECTION
RAM_START_FUNCTION gBoot2SRAMFun2={RtlBootToSram};

PATCH_START_RAM_FUN_SECTION
RAM_START_FUNCTION gBoot2SRAMFun3={RtlBootToSram};


PATCH_START_RAM_FUN_SECTION
RAM_START_FUNCTION gBoot2SRAMFun4={RtlBootToSram};


SRAM_BF_TEXT_SECTION
u32 
PatchHalLogUartInit(
    IN  LOG_UART_ADAPTER    UartAdapter
)
{
    u32 SetData;
    u32 Divisor;
    u32 Dlh;
    u32 Dll;
    u32 SysClock;

    /*
        Interrupt enable Register
        7: THRE Interrupt Mode Enable
        2: Enable Receiver Line Status Interrupt
        1: Enable Transmit Holding Register Empty Interrupt
        0: Enable Received Data Available Interrupt
        */
    // disable all interrupts
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, 0);

    /*
        Line Control Register
        7:   DLAB, enable reading and writing DLL and DLH register, and must be cleared after
        initial baud rate setup
        3:   PEN, parity enable/disable
        2:   STOP, stop bit
        1:0  DLS, data length
        */

    // set DLAB bit to 1
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, 0x80);

    // set up buad rate division 

#ifdef CONFIG_FPGA
    SysClock = SYSTEM_CLK;
    Divisor = (SysClock / (16 * (UartAdapter.BaudRate)));
#else
    {
        u32 SampleRate,Remaind;

        SysClock = (HalGetCpuClk()>>2);

        SampleRate = (16 * (UartAdapter.BaudRate));

        Divisor= SysClock/SampleRate;

        Remaind = ((SysClock*10)/SampleRate) - (Divisor*10);
        
        if (Remaind>4) {
            Divisor++;
        }        
    }
#endif


    Dll = Divisor & 0xff;
    Dlh = (Divisor & 0xff00)>>8;
    HAL_UART_WRITE32(UART_DLL_OFF, Dll);
    HAL_UART_WRITE32(UART_DLH_OFF, Dlh);

    // clear DLAB bit 
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, 0);

    // set data format
    SetData = UartAdapter.Parity | UartAdapter.Stop | UartAdapter.DataLength;
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, SetData);

    /* FIFO Control Register
        7:6  level of receive data available interrupt
        5:4  level of TX empty trigger
        2    XMIT FIFO reset
        1    RCVR FIFO reset
        0    FIFO enable/disable
        */
    // FIFO setting, enable FIFO and set trigger level (2 less than full when receive
    // and empty when transfer 
    HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, UartAdapter.FIFOControl);

    /*
        Interrupt Enable Register
        7: THRE Interrupt Mode enable
        2: Enable Receiver Line status Interrupt
        1: Enable Transmit Holding register empty INT32
        0: Enable received data available interrupt
        */
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, UartAdapter.IntEnReg);

    if (UartAdapter.IntEnReg) {
        // Enable Peripheral_IRQ Setting for Log_Uart
        HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN, 0x1000000);

        // Enable ARM Cortex-M3 IRQ
        NVIC_SetPriorityGrouping(0x3);
        NVIC_SetPriority(PERIPHERAL_IRQ, 14);
        NVIC_EnableIRQ(PERIPHERAL_IRQ);
    }   


    return 0;
}


SRAM_BF_TEXT_SECTION
static VOID
PatchHalInitPlatformLogUart(
    VOID
)
{
    IRQ_HANDLE          UartIrqHandle;
    LOG_UART_ADAPTER    UartAdapter;
    
    //4 Release log uart reset and clock
    LOC_UART_FCTRL(OFF);
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);

    PinCtrl(LOG_UART,S0,ON);

    //4 Register Log Uart Callback function
    UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandleRam;
    UartIrqHandle.Priority = 0;

    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    //4 Initial Log Uart
    PatchHalLogUartInit(UartAdapter);
    
    //4 Register Isr handle
    InterruptRegister(&UartIrqHandle); 
    
    UartAdapter.IntEnReg = 0x05;

    //4 Initial Log Uart for Interrupt
    PatchHalLogUartInit(UartAdapter);

    //4 initial uart log parameters before any uartlog operation
    RtlConsolInit(ROM_STAGE,GetRomCmdNum(),(VOID*)&UartLogRomCmdTable);// executing boot seq., 
}



#ifdef CONFIG_TIMER_MODULE
SRAM_BF_TEXT_SECTION
static VOID
PatchHalInitPlatformTimer(
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
#endif


SRAM_BF_TEXT_SECTION
static VOID
PatchHalShowBuildInfo(VOID)
{
    DBG_8195A("=========================================================\n\n");
    DBG_8195A("Build Time: "UTS_VERSION"\n");
    DBG_8195A("Build Author: "RTL8195AFW_COMPILE_BY"\n");    
    DBG_8195A("Build Host: "RTL8195AFW_COMPILE_HOST"\n");
    DBG_8195A("Build ToolChain Version: "RTL195AFW_COMPILER"\n\n");    
    DBG_8195A("==========================================================\n");
}


SRAM_BF_TEXT_SECTION  
void RtlBootToSram(
    VOID
)
{
    
    {
        //3 Rom Bss Iitial
        u32 BssLen = (__rom_bss_end__ - __rom_bss_start__);

        memset((void *) __rom_bss_start__, 0, BssLen);
        ACTCK_VENDOR_CCTRL(ON);
        SLPCK_VENDOR_CCTRL(ON);
        PinCtrl(JTAG, S0, ON);
        HAL_WRITE32(0x40000000, 0x320, 0x7FF);
        SPI_FLASH_PIN_FCTRL(ON);
		JTAG_PIN_FCTRL(ON);
		LOG_UART_PIN_FCTRL(ON);

        //2 Need Modify
        VectorTableInitRtl8195A(0x1FFFFFFF);

        //3 Enable devide 0 and non-alignment access
//        NVIC_SetCCR();

        //3 Initial Log Uart    
        PatchHalInitPlatformLogUart();

        //3 Initial hardware timer
#ifdef CONFIG_TIMER_MODULE
        PatchHalInitPlatformTimer();
#endif

        PatchHalShowBuildInfo();
    }


    InfraStart();

}

