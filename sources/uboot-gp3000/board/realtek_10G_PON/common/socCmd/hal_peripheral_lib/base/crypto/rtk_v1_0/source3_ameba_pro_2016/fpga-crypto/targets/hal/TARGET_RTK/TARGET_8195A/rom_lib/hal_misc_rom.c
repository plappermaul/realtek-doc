/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include "build_info.h"
//#include "osdep_api.h"

extern _LONG_CALL_ROM_ u32 HalDelayUs(u32 us);

static VOID HalShowBuildInfo(VOID);
static VOID HalBootFlow(VOID);
static VOID HalInitialROMCodeGlobalVar(VOID);
extern VOID HalRomInfo(VOID);

#ifdef CONFIG_SPIC_MODULE
_LONG_CALL_
VOID
SpicReleaseDeepPowerDownFlashRtl8195A(
    VOID
);
#endif

VOID
HalSpiInitV02(
    IN  u8 InitBaudRate
);

VOID
HalBootFlowV02(
    VOID
);

_LONG_CALL_ VOID
HalInitialROMCodeGlobalVarV02(VOID);

VOID
HalResetVsrV02(VOID);



#ifdef CONFIG_TIMER_MODULE
static VOID HalInitPlatformTimer(VOID);
#endif

static VOID HalSpiInit(u8 InitBaudRate);
VOID HalCpuClkConfig(u8 CpuType);
static VOID HalReInitPlatformLogUart(VOID);
static VOID HalInitPlatformLogUart(VOID);

#ifdef CONFIG_TIMER_MODULE
HAL_RAM_BSS_SECTION
HAL_TIMER_OP        HalTimerOp;
#endif

extern u32 STACK_TOP;   // which is defined in vectors.s
extern VOID UartLogIrqHandle(VOID* Data);
extern VOID InfraStart(VOID) __attribute__ ((weak));

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __ram_start_table_start__[];
extern u8 __image1_validate_code__[];

#ifdef CONFIG_BOOT_PROCEDURE
extern VOID RtlConsolInit(u32 Boot ,u32 TBLSz ,VOID *pTBL);
extern VOID RtlConsolRom(u32 WaitCount);
extern VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);
#endif
//_Sema gSema;

extern COMMAND_TABLE  UartLogRomCmdTable[];
extern u32 GetRomCmdNum(VOID);


__weak HAL_ROM_TEXT_SECTION
VOID
HalResetVsr(VOID)
{
    PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE) __ram_start_table_start__;

    //3 Rom Bss Iitial
    u32 BssLen = (__rom_bss_end__ - __rom_bss_start__);

    memset((void *) __rom_bss_start__, 0, BssLen);

    //3 ROM Information
    HalRomInfo();

    //3 Init vendor reg
    ACTCK_VENDOR_CCTRL(ON);
    SLPCK_VENDOR_CCTRL(ON);

    //3 Enable JTAG
    PinCtrl(JTAG, S0, ON);
    //Make sure JTAG TRST is high
    HAL_WRITE32(PERI_ON_BASE, REG_GPIO_PULL_CTRL2,((HAL_READ32(PERI_ON_BASE,REG_GPIO_PULL_CTRL2)&0xFFFFFFFC)|0x2));

    //3 Check boot or wake up from power saving
    if ((HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & BIT(29))) {
        pRamStartFun->RamWakeupFun();
    }

    //PATCH function 1
    if ((HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & BIT(27))) {
        pRamStartFun->RamPatchFun1();
    }

    //Change cpu clk for WDG reset
    HalCpuClkConfig(CLK_50M);

    // Initial Global Variable
    HalInitialROMCodeGlobalVar();

    //2 Need Modify
    VectorTableInitRtl8195A(0x1FFFFFFC);

    //3 Enable devide 0 and non-alignment access
#ifdef CONFIG_ALIGNMENT_EXCEPTION_ENABLE
    NVIC_SetCCR();
#endif

    //3 Initial Log Uart
    HalInitPlatformLogUart();

    //3 Initial hardware timer
#ifdef CONFIG_TIMER_MODULE
    HalInitPlatformTimer();
#endif

    HalShowBuildInfo();

    //Patch function 2
    if ((HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & BIT(26))) {
        pRamStartFun->RamPatchFun2();
    }

    pRamStartFun->RamStartFun();

    for(;;);

}

#define BOOTCLK20P8 1
#define BOOTCLK83P3 2

HAL_ROM_TEXT_SECTION
static VOID
HalBootFlow(
    VOID
)
{
    u8  Value8, BootCpuClk, BootType, InitBaudRate;

    // Load Efuse Setting
    Value8 = ((u8)((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & 0xFF000000)
                   >> 24));
    BootCpuClk = ((Value8 & 0xC) >> 2);
    InitBaudRate = 1;
    BootType = ((Value8 & 0x30) >> 4);

    //3 Switch Boot Clock
    if (BootCpuClk) {

        if ( BootCpuClk == BOOTCLK20P8 ) {
            HalCpuClkConfig(CLK_25M);
        } else if ( BootCpuClk == BOOTCLK83P3 ) {
            HalCpuClkConfig(CLK_100M);
        }

        HalReInitPlatformLogUart();
    }

    //3 Boot type sel from Efuse
    DBG_8195A("Check boot type form eFuse\n");

    switch (BootType) {
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


__weak HAL_ROM_TEXT_SECTION
VOID
HalCpuClkConfig(
    IN  u8  CpuType
)
{

    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1,
                ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (~0x70))
                 |(CpuType << 4)));

}


HAL_ROM_DATA_SECTION const u32 CpkClkTbl[]= {
    200000000,
    100000000,
    50000000,
    25000000,
    12500000,
    4000000
};

__weak HAL_ROM_TEXT_SECTION
u32
HalGetCpuClk(
    VOID
)
{
    u32  CpuType = 0, CpuClk = 0, FreqDown = 0;

    CpuType = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    FreqDown = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) & BIT17);

    CpuClk = CpkClkTbl[CpuType];

    if ( !FreqDown ) {
        if ( CpuClk > 4000000 ) {
            CpuClk = (CpuClk*5/6);
        }
    }

    return CpuClk;
}


__weak HAL_ROM_TEXT_SECTION
VOID
HalRomInfo(
    VOID
)
{
    u32 RomInfo = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG2) & 0xFFFFFF00)|ROMINFORMATION);

    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG2, RomInfo);
}


__weak HAL_ROM_TEXT_SECTION
u8
HalGetRomInfo(
    VOID
)
{
    u8 RomInfo = ((u8)(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG2) & 0xFF));
    return RomInfo;
}

HAL_ROM_DATA_SECTION const u8 ROM_IMG1_VALID_PATTEN[] = {
    0x23, 0x79, 0x16, 0x88, 0xff
};

HAL_ROM_TEXT_SECTION
static VOID
HalSpiInit(
    IN  u8 InitBaudRate
)
{
    u32 ImageLen = 0, ImageIndex = 0, ImageAddr = 0, SpicImageIndex = 0, PinLocat = 0;
    PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;
    u32 i;


#ifdef CONFIG_SPIC_MODULE
    DBG_8195A("SPI Initial\n");

    //3 Check ICFG mode
    if ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_TRP_ICFG(0xF)) == BIT_SYSCFG_TRP_ICFG(3)) {
        PinLocat = S1;
    }

    FLASH_FCTRL(ON);
    ACTCK_FLASH_CCTRL(ON);
    SLPCK_FLASH_CCTRL(ON);
    PinCtrl(SPI_FLASH, PinLocat, ON);
    SpicInitRtl8195A(InitBaudRate, SpicOneBitMode);

    ImageLen = HAL_READ32(SPI_FLASH_BASE, 0x10);
    ImageAddr = HAL_READ32(SPI_FLASH_BASE, 0x14);
    DBG_8195A("Image1 length: 0x%x, Image Addr: 0x%x\n",ImageLen, ImageAddr);

    for (SpicImageIndex = 0x20; SpicImageIndex < (ImageLen+0x20); SpicImageIndex = SpicImageIndex + 4) {
        HAL_WRITE32(ImageAddr, ImageIndex,
                    HAL_READ32(SPI_FLASH_BASE, SpicImageIndex));
        ImageIndex += 4;
    }
    //DBG_8195A("========Image1 Load Done!, Validate @ %x\n", __image1_validate_code__);
    // Validate the loaded image before we jump to image1, the start of image1 must has a pre-defined patten
    i = 0;
    while (ROM_IMG1_VALID_PATTEN[i] != 0xff) {
        if (__image1_validate_code__[i] != ROM_IMG1_VALID_PATTEN[i]) {
            // image1 validation patten miss match, so don't jump to image1
            DBG_8195A("Image1 Validation Incorrect !!!\n");
            while (1) {
                // let the program stuck here
                DBG_8195A("Please Re-boot and try again, or re-burn the flash image\n");
                RtlConsolRom(100000);
            }
        }
        i++;
    }

    DBG_8195A("Image1 Validate OK, Going jump to Image1\n");

    //4 Disable JTAG
    PinCtrl(JTAG, S0, OFF);

    //4 Goto Ram code
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

    //Disable 32K ISO
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_PON_ISO_CTRL, (HAL_READ32(SYSTEM_CTRL_BASE, REG_PON_ISO_CTRL)&(~BIT_ISO_OSC32K_EN)));

    TimerAdapter.IrqDis = ON;
    TimerAdapter.TimerId = 1;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 0;
    TimerAdapter.TimerMode = FREE_RUN_MODE;

    HalTimerOpInit((VOID*)(&HalTimerOp));
    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);

}

__weak HAL_ROM_TEXT_SECTION
_LONG_CALL_  u32
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
            ExpireCount = 0xFFFFFFFF - CurrentCount + StartCount;
        } else {
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
    u32 RomInfo = HalGetRomInfo();

    DBG_8195A("=========================================================\n\n");
    DBG_8195A("ROM Version: %d.%d\n\n", ((RomInfo>>4) & 0x0f), (RomInfo&0x0f));
    DBG_8195A("Build ToolChain Version: "RTL195AFW_COMPILER"\n\n");
    DBG_8195A("=========================================================\n");
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

HAL_ROM_TEXT_SECTION
VOID
HalInitialROMCodeGlobalVar(VOID)
{
    // to initial ROM code using global variable
    ConfigDebugErr = 0xffffffff;
}

#if defined(CONFIG_CHIP_C_CUT) || defined(CONFIG_CHIP_E_CUT)


C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_ROM_ VOID
HalReInitPlatformLogUartV02(
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



#ifdef CONFIG_SPIC_MODULE
__weak _LONG_CALL_ROM_ C_CUT_ROM_TEXT_SECTION
VOID
SpicReleaseDeepPowerDownFlashRtl8195A(
    VOID
)
{

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // set ctrlr0: TX mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    (HAL_SPI_READ32(REG_SPIC_CTRLR0)& 0xFFF0FCFF));

    // set flash_cmd: wren to fifo
    HAL_SPI_WRITE8(REG_SPIC_DR0, FLASH_CMD_RDP);

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
    {
        u32 SrBsuyCheck = 0;

        do {
            SrBsuyCheck = ((HAL_SPI_READ32(REG_SPIC_SR)) & BIT_BUSY);
        } while(SrBsuyCheck == 1);
    }

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HalDelayUs(10);

}
#endif  //#ifdef CONFIG_SPIC_MODULE

C_CUT_ROM_TEXT_SECTION
VOID
HalSpiInitV02(
    IN  u8 InitBaudRate
)
{
    u32 ImageLen = 0, ImageIndex = 0, ImageAddr = 0, SpicImageIndex = 0, PinLocat = 0;
    PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;
    u32 i;


#ifdef CONFIG_SPIC_MODULE
    DBG_8195A("SPI Initial\n");

    //3 Check ICFG mode
    if ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_TRP_ICFG(0xF)) == BIT_SYSCFG_TRP_ICFG(3)) {
        PinLocat = S1;
    }

    FLASH_FCTRL(ON);
    ACTCK_FLASH_CCTRL(ON);
    SLPCK_FLASH_CCTRL(ON);
    PinCtrl(SPI_FLASH, PinLocat, ON);
    SpicInitRtl8195A(InitBaudRate, SpicOneBitMode);

    if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_ALDN_STS ) {

        if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & BIT_SYS_AUTOLOAD_SUS ) {

            if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT(25) ) {

                SpicReleaseDeepPowerDownFlashRtl8195A();
            }
        }
    }

    ImageLen = HAL_READ32(SPI_FLASH_BASE, 0x10);
    ImageAddr = HAL_READ32(SPI_FLASH_BASE, 0x14);
    DBG_8195A("Image1 length: 0x%x, Image Addr: 0x%x\n",ImageLen, ImageAddr);

    for (SpicImageIndex = 0x20; SpicImageIndex < (ImageLen+0x20); SpicImageIndex = SpicImageIndex + 4) {
        HAL_WRITE32(ImageAddr, ImageIndex,
                    HAL_READ32(SPI_FLASH_BASE, SpicImageIndex));
        ImageIndex += 4;
    }

    i = 0;
    while (ROM_IMG1_VALID_PATTEN[i] != 0xff) {
        if (__image1_validate_code__[i] != ROM_IMG1_VALID_PATTEN[i]) {
            // image1 validation patten miss match, so don't jump to image1
            DBG_8195A("Image1 Validation Incorrect !!!\n");
            while (1) {
                // let the program stuck here
                DBG_8195A("Please Re-boot and try again, or re-burn the flash image\n");
                RtlConsolRom(100000);
            }
        }
        i++;
    }

    DBG_8195A("Image1 Validate OK, Going jump to Image1\n");

    //4 Disable JTAG
    PinCtrl(JTAG, S0, OFF);

    //4 Goto Ram code
    pRamStartFun->RamStartFun();
#endif

}


#ifdef CONFIG_USB_MODULE
C_CUT_ROM_TEXT_SECTION
VOID
BootFromUSB(
    VOID)
{
    DBG_8195A("USB REPLACE HERE\n");
}

#define USBBootEntry    BootFromUSB
#endif

#ifdef CONFIG_SDIO_BOOT_ROM
extern VOID SDIO_Boot_Up(VOID);

#define SDIOBootEntry   SDIO_Boot_Up
#endif


C_CUT_ROM_TEXT_SECTION
VOID
HalBootFlowV02(
    VOID
)
{
    u8  Value8, BootCpuClk, BootType, InitBaudRate;

    // Load Efuse Setting
    Value8 = ((u8)((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & 0xFF000000)
                   >> 24));
    BootCpuClk = ((Value8 & 0xC) >> 2);
    InitBaudRate = 1;
    BootType = ((Value8 & 0x30) >> 4);

    //3 Switch Boot Clock
    if (BootCpuClk) {

        if ( BootCpuClk == BOOTCLK20P8 ) {
            HalCpuClkConfig(CLK_25M);
        } else if ( BootCpuClk == BOOTCLK83P3 ) {
            HalCpuClkConfig(CLK_100M);
        }

        HalReInitPlatformLogUartV02();
    }

    //3 Boot type sel from Efuse
    DBG_8195A("Check boot type form eFuse\n");

    switch (BootType) {
        case BOOT_FROM_FLASH:
#ifdef CONFIG_SPIC_MODULE
            HalSpiInitV02(InitBaudRate);
#else
            DBG_8195A("SPIC boot not ready !!!!!!!\n");
            RtlConsolRom(100000);//each delay is 100us
#endif
            break;
        case BOOT_FROM_SDIO:
#ifdef CONFIG_SDIO_BOOT_ROM
            SDIOBootEntry();
#else
            DBG_8195A("SDIO boot not ready !!!!!!!\n");
            RtlConsolRom(100000);//each delay is 100us
#endif
            break;
        case BOOT_FROM_USB:
#ifdef    CONFIG_USB_MODULE
            USBBootEntry();
#else
            DBG_8195A("USB boot not ready !!!!!!!\n");
            RtlConsolRom(100000);//each delay is 100us
#endif
            break;
        default:
            DBG_8195A("No support boot tyep !!!!!!!\n");
            RtlConsolRom(100000);//each delay is 100us
            break;
    }

}


C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_ROM_ VOID
HalInitialROMCodeGlobalVarV02(VOID)
{
    // to initial ROM code using global variable
    ConfigDebugErr = 0xffffffff;
    ConfigDebugInfo = _DBG_SDIO_;
    ConfigDebugWarn = _DBG_SDIO_;

    if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_ALDN_STS ) {

        if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & BIT_SYS_AUTOLOAD_SUS ) {

            if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT15 ) {

                ConfigDebugErr = 0x0;
                ConfigDebugInfo = 0x0;
                ConfigDebugWarn = 0x0;
            }
        }
    }
}



#endif  //CONFIG_CHIP_C_CUT


