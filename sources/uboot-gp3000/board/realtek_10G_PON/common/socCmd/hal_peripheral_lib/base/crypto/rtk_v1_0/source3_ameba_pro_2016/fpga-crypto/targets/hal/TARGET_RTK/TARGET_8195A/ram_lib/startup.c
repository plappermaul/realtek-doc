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
#ifdef PLATFORM_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#if CONFIG_PERI_UPDATE_IMG
//#include "xmodem.h"
#endif

#if defined ( __ICCARM__ )
#pragma section=".ram.bss"
#pragma section=".rom.bss"
#pragma section=".ram.start.table"
#pragma section=".ram_image1.bss"
#pragma section=".image2.start.table1"
#pragma section=".image2.start.table2"

u8* __bss_start__;
u8* __bss_end__;
HAL_RAM_DATA_SECTION u8* __image2_entry_func__;
HAL_RAM_DATA_SECTION u8* __image2_validate_code__;
HAL_RAM_DATA_SECTION u8* __image1_bss_start__;
HAL_RAM_DATA_SECTION u8* __image1_bss_end__;
//extern u8* __rom_bss_start__;
//extern u8* __rom_bss_end__;
//extern u8* __ram_start_table_start__;

HAL_RAM_TEXT_SECTION
void __iar_data_init_boot(void)
{
    // only need __bss_start__, __bss_end__
    __image2_entry_func__		= (u8*)__section_begin(".image2.start.table1");
    __image2_validate_code__	= __image2_entry_func__+4;//(u8*)__section_begin(".image2.start.table2");
    __image1_bss_start__		= (u8*)__section_begin(".ram_image1.bss");
    __image1_bss_end__			= (u8*)__section_end(".ram_image1.bss");
    // __rom_bss_start__, __rom_bss_end__, __ram_start_table_start__
    // are fixed address in rom code. NO need to setup
    //__rom_bss_start__           = (u8*)__section_begin(".rom.bss");
    //__rom_bss_end__             = (u8*)__section_end(".rom.bss");
    //__ram_start_table_start__   = (u8*)__section_begin(".ram.start.table");
    //DBG_8195A("__image2_validate_code__ 0x%08x, __image2_entry_func__ @ 0x%08x\n\r", (u32)__image2_validate_code__, (u32)__image2_entry_func__);
}

void __iar_data_init_app(void)
{
    __bss_start__               = (u8*)__section_begin(".ram.bss");
    __bss_end__                 = (u8*)__section_end(".ram.bss");
}
#else
extern u8 __bss_start__[];
extern u8 __bss_end__[];
extern u8 __image2_entry_func__[];
extern u8 __image2_validate_code__[];
extern u8 __image1_bss_start__[];
extern u8 __image1_bss_end__[];
#endif

extern HAL_VECTOR_FUN  NewVectorTable[];
_LONG_CALL_ extern VOID HalWdgIntrHandle(VOID); // for handle Watchdog Timeout in NMI handler
_LONG_CALL_ extern VOID HalHardFaultHandler(u32); // for handle memory fault in hardfault handler
_LONG_CALL_ extern VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);
#ifdef CONFIG_CHIP_C_CUT
_LONG_CALL_ extern VOID SpicReleaseDeepPowerDownFlashRtl8195A(VOID);
#endif
_LONG_CALL_ extern VOID SpicWaitBusyDoneRtl8195A(VOID);
_LONG_CALL_ extern u32 HALEFUSEOneByteReadROM(u32 CtrlSetting, u16 Addr, u8 *Data, u8 L25OutVoltage);

#ifdef CONFIG_TIMER_MODULE
extern HAL_TIMER_OP        HalTimerOp;
#endif

extern _WEAK VOID InfraStart(VOID);// __attribute__ ((weak));

#if defined(CONFIG_KERNEL) && defined(PLATFORM_FREERTOS)
extern void vTaskStartScheduler( void );
#endif

#ifdef CONFIG_KERNEL
extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
#endif

extern _LONG_CALL_ u32 HalDelayUs(u32 us);
extern _LONG_CALL_ VOID UartLogIrqHandle(VOID* Data);
extern _LONG_CALL_ VOID HalCpuClkConfig(IN  u8  CpuType);
extern _LONG_CALL_ u32 HalGetCpuClk(VOID);
extern _LONG_CALL_ VOID RtlConsolRom(u32 WaitCount);

VOID PreProcessForVendor(VOID);
extern VOID En32KCalibration(VOID);
extern void SystemCoreClockUpdate (void);
extern u32 SdrControllerInit(VOID);
extern VOID SdrCtrlInit(VOID);
extern VOID ShowRamBuildInfo(VOID);
extern void _AppStart(void);

#ifdef CONFIG_SOC_PS_MODULE
extern VOID InitSoCPM(VOID);
#endif

#ifdef CONFIG_PERI_UPDATE_IMG
extern void OTU_FW_Update(u8 uart_idx, u8 pin_mux, u32 baud_rate);
#endif

#if defined(CONFIG_SDIO_DEVICE_EN) && defined(CONFIG_SDIO_BOOT_SIM)
// for SDIO test
extern VOID SDIO_Boot_Up(VOID);
#endif
//3
#ifdef CONFIG_BOOT_FROM_JTAG
void RtlBootToSram(VOID);
START_RAM_FUN_A_SECTION
RAM_START_FUNCTION gRamStartFun = {PreProcessForVendor};

START_RAM_FUN_B_SECTION
RAM_START_FUNCTION gRamPatchWAKE = {RtlBootToSram};

START_RAM_FUN_C_SECTION
RAM_START_FUNCTION gRamPatchFun0 = {RtlBootToSram};

START_RAM_FUN_D_SECTION
RAM_START_FUNCTION gRamPatchFun1 = {RtlBootToSram};

START_RAM_FUN_E_SECTION
RAM_START_FUNCTION gRamPatchFun2 = {RtlBootToSram};

#elif defined(CONFIG_COMPILE_FLASH_DOWNLOAD_CODE)
void RtlFlashProgram(VOID);

START_RAM_FUN_A_SECTION
RAM_START_FUNCTION gRamStartFun = {PreProcessForVendor};

START_RAM_FUN_B_SECTION
RAM_START_FUNCTION gRamPatchWAKE = {RtlFlashProgram};

START_RAM_FUN_C_SECTION
RAM_START_FUNCTION gRamPatchFun0 = {RtlFlashProgram};

START_RAM_FUN_D_SECTION
RAM_START_FUNCTION gRamPatchFun1 = {RtlFlashProgram};

START_RAM_FUN_E_SECTION
RAM_START_FUNCTION gRamPatchFun2 = {RtlFlashProgram};

#else
START_RAM_FUN_A_SECTION
RAM_START_FUNCTION gRamStartFun = {PreProcessForVendor};

START_RAM_FUN_B_SECTION
RAM_START_FUNCTION gRamPatchWAKE = {PreProcessForVendor};

START_RAM_FUN_C_SECTION
RAM_START_FUNCTION gRamPatchFun0 = {PreProcessForVendor};

START_RAM_FUN_D_SECTION
RAM_START_FUNCTION gRamPatchFun1 = {PreProcessForVendor};

START_RAM_FUN_E_SECTION
RAM_START_FUNCTION gRamPatchFun2 = {PreProcessForVendor};
#endif

IMAGE2_START_RAM_FUN_SECTION
RAM_START_FUNCTION gImage2EntryFun0 = {InfraStart};

IMAGE1_VALID_PATTEN_SECTION const u8 RAM_IMG1_VALID_PATTEN[] = {
    0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
};

IMAGE2_VALID_PATTEN_SECTION const u8 RAM_IMG2_VALID_PATTEN[20] = {
    'R', 'T', 'K', 'W', 'i', 'n', 0x0, 0xff,
    (FW_VERSION&0xff), ((FW_VERSION >> 8)&0xff),
    (FW_SUBVERSION&0xff), ((FW_SUBVERSION >> 8)&0xff),
    (FW_CHIP_ID&0xff), ((FW_CHIP_ID >> 8)&0xff),
    (FW_CHIP_VER),
    (FW_BUS_TYPE),
    (FW_INFO_RSV1),
    (FW_INFO_RSV2),
    (FW_INFO_RSV3),
    (FW_INFO_RSV4)
};

#if defined(CONFIG_CHIP_C_CUT) && defined(CONFIG_LINK_ROM_SYMB)
// An Ugly fixing: this variable is declared in ROM code origionaly and it is not in the symbol file
// so we need to declare it again to make the linker reserv this memory.
// And declare this variable here is because to keep variable location sequence same as ROM code
C_CUT_ROM_DATA_SECTION u32 Timer2To7HandlerData[MAX_TIMER_VECTOR_TABLE_NUM];
#endif

#if defined(CONFIG_CHIP_C_CUT)
// This variable is not using now, but it existed when generating C-Cut ROM code, so we cannot remove it.
C_CUT_ROM_DATA_SECTION u8 OTA_Img2Signature[]="81958711";
#endif

extern u32 ConfigDebugErr;
extern u32 ConfigDebugInfo;
extern u32 ConfigDebugWarn;
extern SPIC_INIT_PARA SpicInitParaAllClk[3][CPU_CLK_TYPE_NO];
#if CONFIG_CHIP_E_CUT
extern SPIC_INIT_PARA SpicInitParaID;
#endif

#ifdef CONFIG_GPIO_EN
HAL_RAM_DATA_SECTION HAL_GPIO_ADAPTER gBoot_Gpio_Adapter;
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;
#endif

static
HAL_RAM_TEXT_SECTION
VOID
StartupHalInitialROMCodeGlobalVar(VOID)
{
    // to initial ROM code using global variable
#ifdef CONFIG_DEBUG_ERR_MSG
    ConfigDebugErr = 0xFFFFFFFF;//_DBG_MISC_;]
#else
    ConfigDebugErr = 0;
#endif
#ifdef CONFIG_DEBUG_WARN_MSG
    ConfigDebugWarn = 0xFFFFFFFF;
#else
    ConfigDebugWarn = 0;
#endif
#ifdef CONFIG_DEBUG_INFO_MSG
    ConfigDebugInfo = 0xFFFFFFFF;
#else
    ConfigDebugInfo = 0;
#endif
}

#ifdef CONFIG_CHIP_A_CUT
const INFRA_RAM_TEXT_SECTION u32 StartupCpkClkTbl[]= {
    200000000,
    100000000,
    50000000,
    25000000,
    12500000,
    4000000
};


u32
HAL_RAM_TEXT_SECTION
StartupHalGetCpuClk(
    VOID
)
{
    u32  CpuType = 0, CpuClk = 0, FreqDown = 0;

    CpuType = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    FreqDown = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) & BIT17;

    CpuClk = StartupCpkClkTbl[CpuType];

    if ( !FreqDown ) {
        if ( CpuClk > 4000000 ) {
            CpuClk = (CpuClk*5/6);
        }
    }

    return CpuClk;
}
#endif

static
HAL_RAM_TEXT_SECTION
u32
StartupHalLogUartInit(
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
#else
    {
        u32 SampleRate,Remaind;

#ifdef CONFIG_CHIP_A_CUT
        SysClock = (StartupHalGetCpuClk()>>2);
#else
        SysClock = (HalGetCpuClk()>>2);
#endif
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


static
HAL_RAM_TEXT_SECTION
VOID
StartupHalInitPlatformLogUart(
    VOID
)
{
//    IRQ_HANDLE          UartIrqHandle;
    LOG_UART_ADAPTER    UartAdapter;

    //4 Release log uart reset and clock
    LOC_UART_FCTRL(OFF);
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);
    //PinCtrl(LOG_UART,S0,ON);

#if 0
    //4 Register Log Uart Callback function
    UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandle;
    UartIrqHandle.Priority = 0;
#endif
    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x05;///0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    //4 Initial Log Uart
    StartupHalLogUartInit(UartAdapter);
}


#ifdef CONFIG_TIMER_MODULE
static
HAL_RAM_TEXT_SECTION
VOID
StartupBFJHalInitPlatformTimer(
    VOID
)
{
    TIMER_ADAPTER       TimerAdapter;

    HAL_WRITE32(SYSTEM_CTRL_BASE, 0x204, 0x3);

    OSC32K_CKGEN_CTRL(ON);
    GTIMER_FCTRL(ON);
    ACTCK_TIMER_CCTRL(ON);
    SLPCK_TIMER_CCTRL(ON);
    TimerAdapter.IrqDis = ON;
    TimerAdapter.TimerId = 1;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 0;
    TimerAdapter.TimerMode = FREE_RUN_MODE;


    HalTimerInitRtl8195a((VOID*) &TimerAdapter);

}
#endif



#ifdef CONFIG_BOOT_FROM_JTAG
extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];

static
HAL_RAM_TEXT_SECTION
VOID
StartupBFJHalInitPlatformLogUart(
    VOID
)
{
    IRQ_HANDLE          UartIrqHandle;
    LOG_UART_ADAPTER    UartAdapter;

    //4 Release log uart reset and clock
    LOC_UART_FCTRL(OFF);
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);
    //PinCtrl(LOG_UART,S0,ON);

    //4 Register Log Uart Callback function
    UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandle;
    UartIrqHandle.Priority = 5;

    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x0;///0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    //4 Initial Log Uart
    StartupHalLogUartInit(UartAdapter);

    //4 Register Isr handle
    InterruptRegister(&UartIrqHandle);

    UartAdapter.IntEnReg = 0x05;

    //4 Initial Log Uart for Interrupt
    StartupHalLogUartInit(UartAdapter);
}





HAL_RAM_TEXT_SECTION
void RtlBootToSram(
    VOID
)
{
#if defined ( __ICCARM__ )
    __iar_data_init_boot();
#endif
    PRAM_START_FUNCTION Image2EntryFun=(PRAM_START_FUNCTION)__image2_entry_func__;

    {
        //3 Rom Bss Iitial
        u32 BssLen = (__rom_bss_end__ - __rom_bss_start__);

        // for initial spic flash
        u32 SpicBitMode;
        u32 PinLocat=0;
        memset((void *) __rom_bss_start__, 0, BssLen);

        ACTCK_VENDOR_CCTRL(ON);
        SLPCK_VENDOR_CCTRL(ON);
        PinCtrl(JTAG, S0, ON);
        HAL_WRITE32(0x40000000, 0x320, 0x7FF);
        SPI_FLASH_PIN_FCTRL(ON);
        LOG_UART_PIN_FCTRL(ON);

        //2 Need Modify
        VectorTableInitRtl8195A(0x1FFFFFFC);

        //3 Enable devide 0 and non-alignment access
//        NVIC_SetCCR();
        //for initial spic flash
        FLASH_FCTRL(ON);
        ACTCK_FLASH_CCTRL(ON);
        SLPCK_FLASH_CCTRL(ON);
        PinCtrl(SPI_FLASH, PinLocat, ON);

#if CONFIG_SPIC_EN && SPIC_CALIBRATION_IN_NVM
        SpicNVMCalLoadAll();
#endif

        //3 Change CPU Clk
        HAL_WRITE8(SYSTEM_CTRL_BASE, 0x14,
                   (HAL_READ8(SYSTEM_CTRL_BASE,0x14)&(~0x70))|CPU_CLOCK_SEL_VALUE);

        //3 Initial Log Uart
        StartupHalInitialROMCodeGlobalVar();
        StartupBFJHalInitPlatformLogUart();

        //3 Initial hardware timer
#ifdef CONFIG_TIMER_MODULE
        StartupBFJHalInitPlatformTimer();
#endif


#ifdef CONFIG_SPIC_MODULE
        // Config spic dual mode
#ifdef CONFIG_MP
        SpicBitMode = SpicOneBitMode;
#else
        SpicBitMode = SpicDualBitMode;
#endif  //CONFIG_MP
        SpicInitRtl8195A(1,SpicBitMode);
        SpicFlashInit(SpicBitMode);
#endif  //CONFIG_SPIC_MODULE


        DBG_8195A("===== Enter Image 1.5 ====\n");

    }

#if defined(CONFIG_SDIO_DEVICE_EN) && defined(CONFIG_SDIO_BOOT_SIM)
    // for SDIO Boot test
    SDIO_Boot_Up();
#endif

    //InfraStart();
    DBG_8195A("Img2 Sign: %s, InfaStart @ 0x%08x\n",__image2_validate_code__, (u32)(Image2EntryFun->RamStartFun));
    if (_strcmp(__image2_validate_code__, "RTKWin")) {
        DBG_8195A("Invalid Image2 Signature\n");
        RtlConsolRom(1000);//each delay is 100us
    }

    Image2EntryFun->RamStartFun();
}
#endif

/**
  * @brief  SYSCpuClkConfig. To config cpu clock by using the new value.
  *            Updating SystemCoreClock, flash timing calibration and Log Uart
  *            initialization are also being done.
  *
  * @param  IN  u8  SysCpuClk: the new CPU clock in _EFUSE_CPU_CLK_ format.
  *
  * @retval None
  */
HAL_RAM_TEXT_SECTION
VOID
SYSCpuClkConfig(
    IN  u8  ChipID,
    IN  u8  SysCpuClk
)
{
    u8 isFlashEn;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SysCpuClk);
#ifdef CONFIG_SPIC_MODULE
    if ((HAL_PERI_ON_READ32(REG_SOC_FUNC_EN) & BIT_SOC_FLASH_EN) != 0) {
        isFlashEn = 1;
        SpicWaitWipRtl8195A();
    } else {
        isFlashEn = 0;
    }
#endif

    /* Change to the new CPU clock */
#ifdef CONFIG_CHIP_A_CUT
    HAL_WRITE8(SYSTEM_CTRL_BASE, 0x14,
               (HAL_READ8(SYSTEM_CTRL_BASE,0x14)&(~0x70))|CPU_CLOCK_SEL_VALUE);
#else
    if (ChipID == CHIP_ID_8710AF) {
        if (SysCpuClk == 0) {
            SysCpuClk = 1;
        }
    }
    HalCpuClkConfig(SysCpuClk);
#endif
    /* Update value of SystemCoreClock */
    //SystemCoreClockUpdate();
#ifdef CONFIG_TIMER_MODULE
    HalDelayUs(1000);
#endif

    /* Initialize Log Uart again */
    StartupHalInitPlatformLogUart();

#ifdef CONFIG_SPIC_MODULE
#ifndef CONFIG_CP
    /* Flash calibration for one-bit mode */
    if (isFlashEn) {
        SpicOneBitCalibrationRtl8195A(SysCpuClk);
    }
#endif
#endif


}

#if defined (CONFIG_BOOT_TO_UPGRADE_IMG2) && defined (CONFIG_GPIO_EN)

HAL_RAM_TEXT_SECTION
s32
IsForceLoadDefaultImg2(
    VOID
)
{
    u32 boot_gpio;
    u8 gpio_pin[2];
    s32 force_to_default_img2=0;
    u32 i;
    HAL_GPIO_PIN  GPIO_Pin;
    u32 active_state;

    // polling GPIO to check if user force boot to default image2
    boot_gpio = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR+8);
    gpio_pin[0] = boot_gpio & 0xff;
    gpio_pin[1] = (boot_gpio >> 8) & 0xff;
    _pHAL_Gpio_Adapter = &gBoot_Gpio_Adapter;

    for (i=0; i<2; i++) {
        if (gpio_pin[i] != 0xff) {
            // pin coding: [7]: active level, [6:4]: port, [3:0]: pin
            GPIO_Pin.pin_name = HAL_GPIO_GetIPPinName_8195a((gpio_pin[i] & 0x0f) | (((gpio_pin[i] >> 4) & 0x07) << 4));
            if (gpio_pin[i]&0x80) {
                // High active
                GPIO_Pin.pin_mode = DIN_PULL_LOW;
                active_state = GPIO_PIN_HIGH;
            } else {
                // Low active
                GPIO_Pin.pin_mode = DIN_PULL_HIGH;
                active_state = GPIO_PIN_LOW;
            }

            HAL_GPIO_Init_8195a(&GPIO_Pin);
            if (HAL_GPIO_ReadPin_8195a(&GPIO_Pin) == active_state) {
                force_to_default_img2 = 1;
            } else {
                force_to_default_img2 = 0;
            }
            HAL_GPIO_DeInit_8195a(&GPIO_Pin);
        }
    }
    _pHAL_Gpio_Adapter = NULL;

    return (force_to_default_img2);
}
#endif  // end of "#if defined (CONFIG_BOOT_TO_UPGRADE_IMG2) && defined (CONFIG_GPIO_EN)"

#if defined (CONFIG_PERI_UPDATE_IMG) && defined (CONFIG_GPIO_EN)

_WEAK u8 OTU_check_uart(u32 UpdateImgCfg)
{
    return 0;
}

_WEAK void OTU_FW_Update(u8 uart_idx, u8 pin_mux, u32 baud_rate)
{
    return;
}

HAL_RAM_TEXT_SECTION
s32
IsEnablePeriUpdateImg(
    u32 *UpdateCfg
)
{
    u32 boot_gpio;
    u8 gpio_pin;
    u8 uart_port;
    u8 gpio_pin_bar;
    u8 uart_port_bar;
    s32 enable_update_img2=0;
    HAL_GPIO_PIN  GPIO_Pin;
    u32 active_state;

    // polling GPIO to check if user force boot to default image2
    boot_gpio = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR+0x0c);
    gpio_pin = boot_gpio & 0xff;
    gpio_pin_bar = (boot_gpio >> 8) & 0xff;
    uart_port = (boot_gpio >> 16) & 0xff;
    uart_port_bar = (boot_gpio >> 24) & 0xff;

    _pHAL_Gpio_Adapter = &gBoot_Gpio_Adapter;

    if (((gpio_pin^gpio_pin_bar) == 0xff) &&
            ((uart_port^uart_port_bar) == 0xff)) {
        // pin coding: [7]: active level, [6:4]: port, [3:0]: pin
        *UpdateCfg = uart_port;
        GPIO_Pin.pin_name = HAL_GPIO_GetIPPinName_8195a((gpio_pin & 0x0f) | (((gpio_pin >> 4) & 0x07) << 4));
        if (gpio_pin&0x80) {
            // High active
            GPIO_Pin.pin_mode = DIN_PULL_LOW;
            active_state = GPIO_PIN_HIGH;
        } else {
            // Low active
            GPIO_Pin.pin_mode = DIN_PULL_HIGH;
            active_state = GPIO_PIN_LOW;
        }

        HAL_GPIO_Init_8195a(&GPIO_Pin);
        if (HAL_GPIO_ReadPin_8195a(&GPIO_Pin) == active_state) {
            enable_update_img2 = 1;
        }
        HAL_GPIO_DeInit_8195a(&GPIO_Pin);

        if(OTU_check_uart(uart_port)) {
            enable_update_img2 = 1;
        }
    }

    _pHAL_Gpio_Adapter = NULL;

    return (enable_update_img2);
}
#endif  // end of "#if defined (CONFIG_BOOT_TO_UPGRADE_IMG2) && defined (CONFIG_GPIO_EN)"


VOID
HalNMIHandler_Patch(VOID)
{
#ifdef CONFIG_WDG_NORMAL
    u32 temp;
    WDG_REG *CtrlReg;
#endif

    DBG_8195A_HAL("%s:NMI Error!!!!\n", __FUNCTION__);
#ifdef CONFIG_WDG_NORMAL
    // Check if this NMI is triggered by Watchdog Timer
    temp = HAL_READ32(VENDOR_REG_BASE, 0);
    CtrlReg = (WDG_REG*)&temp;
    if (CtrlReg->WdgToISR) {
        HalWdgIntrHandle();
    }
#endif
}

VOID
HalHardFaultHandler_Patch_c(u32 HardDefaultArg)
{
    u32 Cfsr;
    u32 Bfar;

    u32 StackPc;
    u16 asmcode;

    Cfsr = HAL_READ32(0xE000ED28, 0x0);

    // Violation to memory access protection
    if (Cfsr & 0x82) {

        Bfar = HAL_READ32(0xE000ED38, 0x0);

        // invalid access to wifi register, usually happened in LPS 32K or IPS
        if (Bfar >= WIFI_REG_BASE && Bfar < WIFI_REG_BASE + 0x40000) {

            //__BKPT(0);

            DBG_8195A(".");

            /* Get the MemManage fault PC, and step to next command.
             * Otherwise it will keep hitting MemMange Fault on the same assembly code.
             *
             * To step to next command, we need parse the assembly code to check if it is 16-bit or 32-bit command.
             * Ref: ARM Architecture Reference Manual (ARMv7-A and ARMv7-R edition), Chapter A6 - Thumb Instruction Set Encoding
             *
             * However, the fault assembly code (Ex. LDR or ADR) is not actually executed,
             * So the register value is un-predictable.
             **/
            StackPc = HAL_READ32(HardDefaultArg, 0x18);

            asmcode = HAL_READ16(StackPc, 0);
            if ((asmcode & 0xF800) > 0xE000) {
                // 32-bit instruction, (opcode[15:11] = 0b11111, 0b11110, 0b11101)
                HAL_WRITE32(HardDefaultArg, 0x18, StackPc+4);
            } else {
                // 16-bit instruction
                HAL_WRITE32(HardDefaultArg, 0x18, StackPc+2);
            }

            // clear Hard Fault Status Register
            HAL_WRITE32(0xE000ED2C, 0x0, HAL_READ32(0xE000ED2C, 0x0));

            return;
        }
    }

    HalHardFaultHandler(HardDefaultArg);
}

#if defined ( __ICCARM__ )
VOID
HalHardFaultHandler_Patch_asm(void)
{
    asm("TST LR, #4\n"
        "ITE EQ\n"
        "MRSEQ R0, MSP\n"
        "MRSNE R0, PSP\n"
        "B HalHardFaultHandler_Patch_c");
}
#endif

// Override original Interrupt Vector Table
INFRA_START_SECTION
VOID
VectorTableOverrideRtl8195A(
    IN  u32 StackP
)
{
    // Override NMI Handler
    //4 Initial NMI
    NewVectorTable[2] = (HAL_VECTOR_FUN)HalNMIHandler_Patch;

#if defined ( __ICCARM__ )
    NewVectorTable[3] = (HAL_VECTOR_FUN)HalHardFaultHandler_Patch_asm;
#endif
}

#if 0//#if CONFIG_IMAGE_SEPARATE
HAL_RAM_TEXT_SECTION
s32
IsValidUpdatedImg2Addr(
    u32 Image2Addr,
    u32 DefImage2Addr,
    u32 DefImage2Size
)
{
    if (Image2Addr == 0xffffffff) {
        // Upgraded Image2 isn't exist
        return 0;   // invalid address
    }

    if ((Image2Addr & 0xfff) != 0) {
        // Not 4K aligned
        return 0;   // invalid address
    }

    if (Image2Addr <= DefImage2Addr) {
        // Updated image2 address must bigger than the addrss of default image2
        return 0;   // invalid address
    }

    if (Image2Addr < (DefImage2Addr+DefImage2Size)) {
        // Updated image2 overlap with the default image2
        return 0;   // invalid address
    }

    return 1;   // this address is valid
}

HAL_RAM_TEXT_SECTION
s32
IsValidImg2Sign(
    u32 Image2LoadAddr
)
{
    u32 img2_sig[3];
    s32 sign_valid=0;

    // Image2 header: Size(4B) + Addr(4B) + Signature(8B)
    img2_sig[0] = HAL_READ32(SPI_FLASH_BASE, Image2LoadAddr + 8);
    img2_sig[1] = HAL_READ32(SPI_FLASH_BASE, Image2LoadAddr + 12);
    img2_sig[2] = 0x00; // end of string
    if (_strcmp((char*)img2_sig, (char*)OTA_Img2Signature)) {
        DBG_BOOT_INFO("Invalid Upgrade Image2 Signature:%s\n", img2_sig);
    } else {
        sign_valid = 1;
    }

    return sign_valid;

}

#endif


#ifdef CONFIG_SPIC_MODULE
HAL_RAM_TEXT_SECTION VOID StartupHalSpicInit(
    IN  u8 InitBaudRate
)
{
    u32 PinLocat = 0;

    //3 Check ICFG mode
    if ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_TRP_ICFG(0xF)) == BIT_SYSCFG_TRP_ICFG(3)) {
        PinLocat = S1;
    }

    FLASH_FCTRL(ON);
    ACTCK_FLASH_CCTRL(ON);
    SLPCK_FLASH_CCTRL(ON);
    PinCtrl(SPI_FLASH, PinLocat, ON);
    SpicInitRtl8195A(InitBaudRate, SpicOneBitMode);

#ifdef CONFIG_CHIP_C_CUT
    if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_ALDN_STS ) {

        if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & BIT_SYS_AUTOLOAD_SUS ) {

            if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT(25) ) {

                SpicReleaseDeepPowerDownFlashRtl8195A();
            }
        }
    }
#endif
}
#endif  // end of "#ifdef CONFIG_SPIC_MODULE "

//3 Image 1
HAL_RAM_TEXT_SECTION
VOID
PreProcessForVendor(
    VOID
)
{
#ifdef CONFIG_TIMER_MODULE
    // Re-init G-Timer HAL Function pointer with ROM Patch
    if (HalCommonInit() != HAL_OK) {
        DBG_8195A("Hal Common Init Failed.\n");
    }

    StartupBFJHalInitPlatformTimer();
    HalDelayUs(1000);
#endif

    //3 3) Jump to image 2
    InfraStart();

}


#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
extern VOID wlan_network(VOID);
#endif

#if 0
static
VOID
HalShowRamBuildInfo(VOID)
{

    DBG_8195A("=========================================================\n\n");
    //DBG_8195A("Build Time: "UTS_VERSION"\n");
    DBG_8195A("Build Time: "RTL8195AFW_COMPILE_TIME"\n");
    DBG_8195A("Build Author: "RTL8195AFW_COMPILE_BY"\n");
    DBG_8195A("Build Host: "RTL8195AFW_COMPILE_HOST"\n");
    DBG_8195A("Build ToolChain Version: "RTL195AFW_COMPILER"\n\n");
    DBG_8195A("=========================================================\n");

}
#endif

_WEAK VOID
SDIO_Device_Off(VOID)
{
    /* Disable Clock for SDIO function */
    ACTCK_SDIOD_CCTRL(OFF);

    /* SDIO Function Disable */
    SDIOD_ON_FCTRL(OFF);
    SDIOD_OFF_FCTRL(OFF);
    // SDIO Pin Mux off
    SDIOD_PIN_FCTRL(OFF);
}

INFRA_START_SECTION
VOID
SYSPlatformInit(
    VOID
)
{
#ifdef CONFIG_CHIP_A_CUT
    //Set SPS lower voltage
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0, (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0)&0xf0ffffff));
#else   // B-Cut & C-Cut
    //Set SPS lower voltage
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0, ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0)&0xf0ffffff)|0x6000000));
#endif

    //xtal buffer driving current
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_XTAL_CTRL1,
                ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_XTAL_CTRL1)&(~(BIT_MASK_SYS_XTAL_DRV_RF1<<BIT_SHIFT_SYS_XTAL_DRV_RF1)))|(BIT_SYS_XTAL_DRV_RF1(1))));
}


//3 Imgae 2
INFRA_START_SECTION
VOID
InfraStart(VOID)
{
    u8 isFlashEn;


    DBG_8195A("===== Enter Image 2 ====\n");

    ShowRamBuildInfo();

    //3 0) Vendor Config function
    //4 Ram Bss Iitial
    u32 BssLen = (__bss_end__ - __bss_start__);

    memset((void *) __bss_start__, 0, BssLen);

#ifdef CONFIG_KERNEL
    InterruptForOSInit((VOID*)vPortSVCHandler,
                       (VOID*)xPortPendSVHandler,
                       (VOID*)xPortSysTickHandler);
#endif


    _AppStart();


}


