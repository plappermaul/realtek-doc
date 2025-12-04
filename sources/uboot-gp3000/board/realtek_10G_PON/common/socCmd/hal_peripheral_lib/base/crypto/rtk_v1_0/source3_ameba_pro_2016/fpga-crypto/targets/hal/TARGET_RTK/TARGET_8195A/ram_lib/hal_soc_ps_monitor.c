/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifdef CONFIG_SOC_PS_MODULE

#include "rtl8195a.h"
#include "hal_soc_ps_monitor.h"

#include "PinNames.h"
#include "gpio_api.h"


extern VOID UartLogIrqHandleRam(VOID * Data);
#if defined (__ICCARM__)
extern void xPortPendSVHandler( void );
#elif defined (__GNUC__)
extern void xPortPendSVHandler( void ) __attribute__ (( naked ));
#endif
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );
extern u32 HalGetCpuClk(VOID);
extern _LONG_CALL_  u32 HalDelayUs(u32 us);

extern COMMAND_TABLE  UartLogRomCmdTable[];
extern HAL_TIMER_OP HalTimerOp;
extern u32 STACK_TOP;   // which is defined in vectors.s

SYS_ADAPTER SYSAdapte;

Power_Mgn PwrAdapter;

VOID ReFillCpuClk(VOID);
extern u8 __ram_start_table_start__[];

#ifdef CONFIG_GPIO_EN
extern HAL_GPIO_ADAPTER gHAL_Gpio_Adapter;
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;
#endif

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

#if 0//def CONFIG_FPGA
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


VOID
PSHalInitPlatformLogUart(
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
    //RtlConsolInit(ROM_STAGE,GetRomCmdNum(),(VOID*)&UartLogRomCmdTable);// executing boot seq.,
    //pUartLogCtl->TaskRdy = 1;
}


#ifdef CONFIG_SDR_EN
VOID
SDRWakeUp(
    VOID
)
{
    ACTCK_SDR_CCTRL(ON);
    SDR_PIN_FCTRL(ON);
    HalDelayUs(10);
    HAL_WRITE32(0x40005000, 0x34, 0x3);
    HAL_WRITE32(0x40005000, 0x10, HAL_READ32(0x40005000, 0x10)&(~BIT28));
}
#if 0
VOID
SDRSleep(
    VOID
)
{
    gpio_t gpio_obj;

    HAL_WRITE32(0x40005000, 0X10, HAL_READ32(0x40005000, 0x10)|BIT28);
    ACTCK_SDR_CCTRL(OFF);
    gpio_init(&gpio_obj, PG_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_HIGH);

    gpio_init(&gpio_obj, PG_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);

    gpio_init(&gpio_obj, PG_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);

    gpio_init(&gpio_obj, PG_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);

    gpio_init(&gpio_obj, PJ_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);

    gpio_init(&gpio_obj, PJ_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);

    SDR_PIN_FCTRL(OFF);
    HAL_WRITE32(0x40005000, 0x34, 0x1);
    gpio_init(&gpio_obj, PJ_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_dir(&gpio_obj, PIN_OUTPUT);
    gpio_write(&gpio_obj, GPIO_PIN_LOW);
}
#else

VOID
SDRSleep(
    VOID
)
{
#ifdef CONFIG_GPIO_EN
    HAL_GPIO_PIN  GPIO_Pin;
    u32 i;
    u32 pin_name;
    const u8 SDR_PWrDn_PinCtrl[][2] = {
        {PG_1, 1},      // SDR_WE
        {PG_2, 0},      // SDR_CAS
        {PG_3, 0},      // SDR_RAS
        {PG_4, 0},      // SDR_CS
        {PJ_1, 0},      // SDR_CLK
        {PJ_2, 0},      // SDR_CKE

        {0xFF, 0xFF}    // end of table, maximum 64 entity
    };
#endif

    HAL_WRITE32(0x40005000, 0X10, HAL_READ32(0x40005000, 0x10)|BIT28);
    ACTCK_SDR_CCTRL(OFF);

#ifdef CONFIG_GPIO_EN
    if (_pHAL_Gpio_Adapter == NULL) {
        _pHAL_Gpio_Adapter = &gHAL_Gpio_Adapter;
    }
    GPIO_Pin.pin_mode = DOUT_PUSH_PULL;
    for (i=0; i<64; i++) {
        pin_name = SDR_PWrDn_PinCtrl[i][0];
        if (pin_name == 0xFF) {
            break;
        }

        GPIO_Pin.pin_name = HAL_GPIO_GetIPPinName_8195a(pin_name);
//        GPIO_Pin.pin_mode = DOUT_PUSH_PULL;
        HAL_GPIO_Init_8195a(&GPIO_Pin);
        if (SDR_PWrDn_PinCtrl[i][1] == 0) {
            GPIO_PullCtrl_8195a(pin_name, HAL_GPIO_PULL_LOW);
            HAL_GPIO_WritePin_8195a(&GPIO_Pin, 0);
        } else {
            GPIO_PullCtrl_8195a(pin_name, HAL_GPIO_PULL_HIGH);
            HAL_GPIO_WritePin_8195a(&GPIO_Pin, 1);
        }
    }
#endif  // #ifdef CONFIG_GPIO_EN

    SDR_PIN_FCTRL(OFF);
    HAL_WRITE32(0x40005000, 0x34, 0x1);

#ifdef CONFIG_GPIO_EN
    GPIO_Pin.pin_name = HAL_GPIO_GetIPPinName_8195a(PJ_1);
//    GPIO_Pin.pin_mode = DOUT_PUSH_PULL;
    HAL_GPIO_Init_8195a(&GPIO_Pin);
    GPIO_PullCtrl_8195a(PJ_1, HAL_GPIO_PULL_HIGH);
    HAL_GPIO_WritePin_8195a(&GPIO_Pin, 1);
#endif
}
#endif  // #if 0
#endif

VOID
SYSIrqHandle
(
    IN  VOID        *Data
)
{
    u32 Rtemp;

    //change cpu clk
    ReFillCpuClk();
    HalDelayUs(100);

    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN) | 0x40000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN, Rtemp);

    if (PwrAdapter.SDREn) {
#ifdef CONFIG_SDR_EN
        SDRWakeUp();
#endif
    }

    //disable DSTBY timer
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, 0);

    //clear wake event IMR
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, 0);

    //clear wake event ISR
    Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0, Rtemp);

    //set event flag
    PwrAdapter.WakeEventFlag = _TRUE;
}

VOID
InitSYSIRQ(VOID)
{
    IRQ_HANDLE          SysHandle;
    PSYS_ADAPTER        pSYSAdapte;
    pSYSAdapte          = &SYSAdapte;
    SysHandle.Data       = (u32) (pSYSAdapte);
    SysHandle.IrqNum     = SYSTEM_ON_IRQ;
    SysHandle.IrqFun     = (IRQ_FUN) SYSIrqHandle;
    SysHandle.Priority   = 0;

    InterruptRegister(&SysHandle);
    InterruptEn(&SysHandle);
    PwrAdapter.WakeEventFlag = _FALSE;
}

void vWFSSVCHandler( void )
{
#if defined (__ICCARM__)
    // TODO: IAR has different way using assembly
#elif defined (__GNUC__)
    asm volatile
    (
        "svcing:\n"
        "   mov r0, %0                      \n"
        "	ldmia r0!, {r4-r7}				\n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
        "	ldmia r0!, {r8-r11}				\n"
        "	msr psp, r0						\n" /* Restore the task stack pointer. */
        "	orr r14, #0xd					      \n"
        "	bx r14							\n"
        ::"r"(PwrAdapter.CPUPSP):"r0"
    );
#endif
}


VOID
WakeFromSLPPG(
    VOID
)
{
    //release shutdone
    HAL_WRITE32(PERI_ON_BASE, REG_GPIO_SHTDN_CTRL, 0x7FF);
    //HAL_WRITE32(PERI_ON_BASE, REG_CPU_PERIPHERAL_CTRL, 0x110001);
    //JTAG rst pull high
    HAL_WRITE32(PERI_ON_BASE, REG_GPIO_PULL_CTRL2, 0x05555556);

    ReFillCpuClk();

    //3 Need Modify
    VectorTableInitRtl8195A(0x1FFFFFFC);

    //3 Make PendSV, CallSV and SysTick the same priroity as the kernel.
    HAL_WRITE32(0xE000ED00, 0x20, 0xF0F00000);

    //3 Initial Log Uart
    PSHalInitPlatformLogUart();

#ifdef CONFIG_KERNEL
    InterruptForOSInit((VOID*)vWFSSVCHandler,
                       (VOID*)xPortPendSVHandler,
                       (VOID*)xPortSysTickHandler);
#endif
    //CPURegbackup[13] = CPURegbackup[13]-4;
    PwrAdapter.CPURegbackup[16] |= 0x1000000 ;

    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-4) ) )= PwrAdapter.CPURegbackup[16]; //PSR
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-8) ) )= PwrAdapter.CPURegbackup[15]; //PC
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-12) ) )= PwrAdapter.CPURegbackup[14]; //LR
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-16) ) )= PwrAdapter.CPURegbackup[12]; //R12
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-20) ) )= PwrAdapter.CPURegbackup[3];  //R3
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-24) ) )= PwrAdapter.CPURegbackup[2];  //R2
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-28) ) )= PwrAdapter.CPURegbackup[1];  //R1
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-32) ) )= PwrAdapter.CPURegbackup[0];  //R0
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-36) ) )= PwrAdapter.CPURegbackup[11]; //R11
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-40) ) )= PwrAdapter.CPURegbackup[10]; //R10
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-44) ) )= PwrAdapter.CPURegbackup[9];  //R9
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-48) ) )= PwrAdapter.CPURegbackup[8];  //R8
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-52) ) )= PwrAdapter.CPURegbackup[7];  //R7
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-56) ) )= PwrAdapter.CPURegbackup[6];  //R6
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-60) ) )= PwrAdapter.CPURegbackup[5];  //R5
    ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[13]-64) ) )= PwrAdapter.CPURegbackup[4];  //R4
    PwrAdapter.CPURegbackup[13] = PwrAdapter.CPURegbackup[13]-64;  //PSP
    PwrAdapter.CPUPSP = PwrAdapter.CPURegbackup[13];
    //CPURegBackUp();

    asm volatile(
        " cpsie i				\n" /* Globally enable interrupts. */
        " svc 0					\n" /* System call to start first task. */
        " nop					\n"
    );
}

VOID
DurationScaleAndPeriodOP(
    IN  u32 SDuration,
    OUT u32 *ScaleTemp,
    OUT u32 *PeriodTemp
)
{
    u8 Idx = 0;
    if (SDuration > 8355) {
        SDuration = 0x20A3;
    }

    //in unit 128us
    SDuration = ((SDuration*125)/16);

    for (Idx = 8; Idx < 32; Idx++) {

        if ( (SDuration & 0xFFFFFF00) > 0 ) {
            (*ScaleTemp) = (*ScaleTemp) + 1;
            SDuration = (SDuration >> 1);
        } else {
            break;
        }
    }

    *ScaleTemp = ((*ScaleTemp) << 8);
    *PeriodTemp = SDuration;
}


u32
CLKCal(
    IN  u8  ClkSel
)
{
    u32 Rtemp = 0;
    u32 RRTemp = 0;

    if( ClkSel ) {
        //a33_ck
        Rtemp |= 0x10000;
    }

    //Enable cal
    Rtemp |= 0x800000;
    HAL_WRITE32(VENDOR_REG_BASE, REG_VDR_ANACK_CAL_CTRL, Rtemp);

    while( (HAL_READ32(VENDOR_REG_BASE, REG_VDR_ANACK_CAL_CTRL) & BIT23) != 0 );
    Rtemp = ((HAL_READ32(VENDOR_REG_BASE, REG_VDR_ANACK_CAL_CTRL) & 0x3FFF))+1;

    if( ClkSel ) {
        //a33_ck
        RRTemp = (Rtemp);
    } else {
        //anack
        RRTemp = ((2133/Rtemp) - 1);
    }

    //DiagPrintf("CAL : 0x%x\n", RRTemp);

    return RRTemp;
}

VOID
BackupCPUClk(
    VOID
)
{
    u32 Cpubp;
    Cpubp = ((HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0)&0xFFFFFFF0)|((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4));
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0,Cpubp);
}

VOID
ReFillCpuClk(
    VOID
)
{
    u8 CpuClk = ((u8)(HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0)& (0xF)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1,
                ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (~0x70))
                 |(CpuClk << 4)));
}

VOID
SleepClkGatted(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 ScaleTemp = 0;
    u32 PeriodTemp = 0;
    u32 CalTemp = 0;

    //Backup CPU CLK
    BackupCPUClk();

    //truncate duration
    SDuration &= 0x0003FFFC;
    //2  CSleep
    //3 1.1 Set TU timer timescale
    //0x4000_0090[21:16] = 6'h1F
    //0x4000_0090[15] = 1'b0 => Disable timer
    CalTemp = (CLKCal(ANACK) << 16);
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL)& 0xffff7fff & 0xffc0ffff) | CalTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[11:8]  => Time scale
    //0x4000_0090[7:0]    => Time period
    //max duration 0x7FFFFF us, min 0x80
    DurationScaleAndPeriodOP(SDuration, &ScaleTemp, &PeriodTemp);

    Rtemp = (((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) & 0xfffff000) | ScaleTemp) | PeriodTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[15] = 1'b1 => Enable timer
    Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //3 1.2 Configure platform wake event
    //0x4000_0100[0] = 1'b1 => Enable timer and GT as wakeup event to wakeup CPU
    Rtemp = 0x00000001;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);

    //3 1.3 Configure power state option:
    // 1.4.3   0x120[15:8]: sleep power mode option0 [11] = 1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION) & 0xffff00ff) | 0x74000A00);//A
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 1.4.4   0x124[7:0]: sleep power mode option1 [0] =1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00) | 0x00000001);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    //3 1.5 Enable low power mode
    // 1.5.1   0x4000_0118[2] = 1 => for sleep mode
    Rtemp = 0x00000004;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000004;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //3 1.6 Wait CHIP enter low power mode
    // 1.7 Wait deep standby timer timeout
    // 1.8 Wait CHIP resume to norm power mode
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();

}


VOID SleepPwrGatted(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 ScaleTemp = 0;
    u32 PeriodTemp = 0;
    u32 CalTemp = 0;

    //Backup CPU CLK
    BackupCPUClk();

    //truncate duration
    SDuration &= 0x0003FFFC;

    //2  PSleep
    //3 1.1 Set TU timer timescale
    //0x4000_0090[21:16] = 6'h1F
    //0x4000_0090[15] = 1'b0 => Disable timer
    CalTemp = (CLKCal(ANACK) << 16);
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL)& 0xffff7fff & 0xffc0ffff) | CalTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[11:8]  => Time scale
    //0x4000_0090[7:0]    => Time period
    //max duration 0x7FFFFF us, min 0x80
    DurationScaleAndPeriodOP(SDuration, &ScaleTemp, &PeriodTemp);

    Rtemp = (((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) & 0xfffff000) | ScaleTemp) | PeriodTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[15] = 1'b1 => Enable timer
    Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //3 1.2 Configure platform wake event
    //0x4000_0100[0] = 1'b1 => Enable timer and GT as wakeup event to wakeup CPU
    Rtemp = 0x00000003;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);

    //3 1.4 Configure power state option:
    // 1.4.3   0x120[15:8]: sleep power mode option0:
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION) & 0x00ff00ff) | 0x74000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 1.4.4   0x124[7:0]: sleep power mode option1:
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00) | 0x00000003);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    //3 1.5 Enable low power mode
    // 1.5.1   0x4000_0118[2] = 1 => for sleep mode
    Rtemp = 0x00000004;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000004;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //3 1.6 Wait CHIP enter low power mode
    // 1.7 Wait deep standby timer timeout
    // 1.8 Wait CHIP resume to norm power mode
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();
    DiagPrintf("YOU CAN'T SEE ME ~~~~!!!!!!!!!!!!!!!!!!!~~~~~slppg~~~~!!!!!!!!!!");
}


VOID
DStandby(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 ScaleTemp = 0;
    u32 PeriodTemp = 0;
    u32 CalTemp = 0;

    //Backup CPU CLK
    BackupCPUClk();

    //Clear A33 timer event
    //Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_SLP_WAKE_EVENT_STATUS0);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_SLP_WAKE_EVENT_STATUS0, Rtemp);

    //2 Deep Standby mode
    //3 1.1 Set TU timer timescale
    //0x4000_0090[21:16] = 6'h1F
    //0x4000_0090[15] = 1'b0 => Disable timer
    CalTemp = (CLKCal(ANACK) << 16);
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL)& 0xffff7fff & 0xffc0ffff) | CalTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[11:8]  => Time scale
    //0x4000_0090[7:0]    => Time period
    //max duration 0x7FFFFF us, min 0x80
    DurationScaleAndPeriodOP(SDuration, &ScaleTemp, &PeriodTemp);

    Rtemp = (((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) & 0xfffff000) | ScaleTemp) | PeriodTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //0x4000_0090[15] = 1'b1 => Enable timer
    Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

    //3 1.3 Configure platform wake event
    // 1.3.1   0x4000_0100[0] = 1'b1 => Enable deep standby timer wakeup event to wakeup CPU
    Rtemp = 0x00000001;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);

    //3 1.4 Configure power state option:
    // 1.4.4   0x120[7:0]: deep standby power mode option:
    Rtemp = 0x74000000;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 1.4.5   0x124[7:0]: sleep power mode option1 [0] =1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00) | 0x00000001);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    //3 1.5 Enable low power mode
    // [0x4000_0118[1] = 1 => for deep standby mode]
    Rtemp = 0x00000002;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //3 1.6 Wait CHIP enter low power mode
    // 1.7 Wait deep standby timer timeout
    // 1.8 Wait CHIP resume to norm power mode
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();

    DiagPrintf("YOU CAN'T SEE ME ~~~~!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~!!!!!!!!!!");
}


VOID
DSleep(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    //u32 ScaleTemp = 0;
    //u32 PeriodTemp = 0;
    u32 UTemp = 0;
    u32 MaxTemp = 0;

    u32 Reada335 = 0;

    //2 Deep Sleep mode:
    //3 2.1 Set TU timer timescale

    //3 2.2 Configure deep sleep timer:
    //2.2.1   Enable REGU access interface 0x4000_0094[31] = 1
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2 Calibration A33 CLK
    UTemp = CLKCal(A33CK);

    //Calculate the max value base on the a33 duration
    MaxTemp = 0x7FFFFF*0x100/100000*UTemp/100*0x80;
    //DiagPrintf("MaxTemp : 0x%x\n", MaxTemp);

    if ( SDuration >= MaxTemp ) {
        SDuration = 0x7FFFFF;
    } else {
        //In unit of A33 CLK : max num is bounded by anaclk = 1.5k
        SDuration = ((((SDuration)/UTemp)*25/16*25/16*125));
        //DiagPrintf("SDuration : 0x%x\n", SDuration);
    }

    DiagPrintf("SDuration : 0x%x\n", SDuration);

    //3 2.2.2   Initialize deep sleep counter
    //2.2.2.0 0x4000_0094[15:0] = 16'hD300 => Disable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D300);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
    //2.2.2.0.1 Clear event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT));
    //2.2.2.1 0x4000_0094[15:0] = 16'h9008 => set counter[7:0]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009000 | ((u8)SDuration));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.2 0x4000_0094[15:0] = 16'h9100 => set counter[15:8]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009100 | ((u8)(SDuration >> 8)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.3 0x4000_0094[15:0] = 16'h9200 => set counter[22:16]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009200 | ((u8)(SDuration >> 16)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.4 0x4000_0094[15:0] = 16'hD380 => Enable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D380);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    HalDelayUs(1000);
    Reada335 = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CAL_CTRL);
    DiagPrintf("a33 timer : 0x%x\n", Reada335);

    HalDelayUs(8000);

    //3 2.2.3
    //2.3 Enable low power mode: 0x4000_0118[0] = 1'b1;
    Rtemp = 0x00000001;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000001;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //2.4 Wait CHIP enter deep sleep mode
    //2.5 Wait deep sleep counter timeout
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();

    DiagPrintf("YOU CAN'T SEE ME ~~~~!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~!!!!!!!!!!");
}

VOID
MSBackupProcess(
    void
)
{

    u8 i = 0;

    //backup main stack
    for (i = 0; i < (MAX_BACKUP_SIZE-1); i++) {
        PwrAdapter.MSPbackup[i] = HAL_READ32(0x1FFFFE00, (0x1FC - (i*4)));
    }

    asm volatile
    (
        "MRS r0, MSP\n"
        "MOV %0, r0\n"
        :"=r"(PwrAdapter.MSPbackup[MAX_BACKUP_SIZE-1])
        ::"memory"
    );
}


VOID
MSReFillProcess(
    VOID
)
{
    u8 i = 0;

    for (i = 0; i < (MAX_BACKUP_SIZE-1); i++) {

        HAL_WRITE32(0x1FFFFE00, (0x1FC - (i*4)), PwrAdapter.MSPbackup[i]);
    }

    asm volatile
    (
        "MSR MSP, %0\n"
        ::"r"(PwrAdapter.MSPbackup[MAX_BACKUP_SIZE-1]):"memory"
    );
}


VOID
SoCPSGPIOCtrl(
    VOID
)
{
    HAL_WRITE32(PERI_ON_BASE,0x330,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x334,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x338,0x05555555);
    HAL_WRITE32(PERI_ON_BASE,0x33c,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x340,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x344,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x348,0x55555555);
    HAL_WRITE32(PERI_ON_BASE,0x320,0x0);
}


VOID
InitSoCPM(
    VOID
)
{
    u8 Idx = 0;
    PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE) __ram_start_table_start__;

    PwrAdapter.ActFuncCount = 0;
    PwrAdapter.CurrentState = ACT;
    for (Idx = 0; Idx < MAXSTATE; Idx++) {
        PwrAdapter.PwrState[Idx].FuncIdx = 0xFF;
        PwrAdapter.PwrState[Idx].PowerState = 0xFF;
    }
    PwrAdapter.SDREn = _FALSE;
    InitSYSIRQ();
    pRamStartFun->RamWakeupFun = WakeFromSLPPG;
}

u8
ChangeSoCPwrState(
    IN  u8  RequestState,
    IN  u32 ReqCount
)
{

    //DiagPrintf("Go to sleep");

    while(1) {

        HalDelayUs(100);

        if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {

            break;
        }
    }

    switch (RequestState) {

        case ACT:
            break;

        case WFE:
            __WFE();
            break;

        case WFI:
            __WFI();
            break;

        //case SNOOZE:
        //break;

        case SLPCG:
            SleepClkGatted(ReqCount);
            break;

        case SLPPG:
            //Resume jump to wakeup function
            //HAL_WRITE32(PERI_ON_BASE, 0x218, (HAL_READ32(PERI_ON_BASE,0x218)|BIT31));

            SoCPSGPIOCtrl();
            SleepPwrGatted(ReqCount);
            break;

        case DSTBY:
            SoCPSGPIOCtrl();
            DStandby(ReqCount);
            break;

        case DSLP:
        case INACT:
            SoCPSGPIOCtrl();
            DSleep(ReqCount);
            break;
    }
    return 0;
}


u32
SoCPwrChk(
    IN  u8  ReqState,
    OUT u8* FailfuncIdx,
    OUT u8* FailState
)
{
    u8 Idx       = 0;
    u32 Result = _FALSE;

    if ( PwrAdapter.ActFuncCount ) {

        for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {

            if (PwrAdapter.PwrState[Idx].PowerState < ReqState) {
                *FailfuncIdx = PwrAdapter.PwrState[Idx].FuncIdx;
                *FailState   = PwrAdapter.PwrState[Idx].PowerState;
                Result =  _FALSE;
                break;
            }
        }
    } else {
        *FailfuncIdx = PwrAdapter.PwrState[Idx].FuncIdx;
        *FailState   = PwrAdapter.PwrState[Idx].PowerState;
        Result = _TRUE;
    }
    return Result;
}


VOID
RegPowerState(
    REG_POWER_STATE RegPwrState
)
{
    u8 Idx;
    u8 StateIdx  = 0;
    u8 FState = 0;

    for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {
        if (PwrAdapter.PwrState[Idx].FuncIdx == RegPwrState.FuncIdx) {
            StateIdx = Idx;
            FState = _TRUE;
            break;
        }
    }

    switch (RegPwrState.PwrState) {

        case INACT :
            if (FState) {
                for (Idx = StateIdx; Idx < PwrAdapter.ActFuncCount; Idx++) {
                    PwrAdapter.PwrState[Idx].FuncIdx     = PwrAdapter.PwrState[Idx+1].FuncIdx;
                    PwrAdapter.PwrState[Idx].PowerState  = PwrAdapter.PwrState[Idx+1].PowerState;
                }
                PwrAdapter.ActFuncCount--;
            } else {
            }
            break;

        default:

            if (FState) {
                PwrAdapter.PwrState[StateIdx].PowerState    = RegPwrState.PwrState;
            } else {
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].FuncIdx        = RegPwrState.FuncIdx;
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].PowerState     = RegPwrState.PwrState;
                PwrAdapter.ActFuncCount++;
            }

            break;
    }

    //for debug
#if 0
    for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {
        DiagPrintf("RegPwrIdx : %d \n", Idx);
        DiagPrintf("FuncIdx : %d \n", PwrAdapter.PwrState[Idx].FuncIdx);
        DiagPrintf("PowerState : 0x%x \n", PwrAdapter.PwrState[Idx].PowerState);
    }
#endif
}


VOID
ReadHWPwrState(
    IN   u8  FuncIdx,
    OUT  u8* HwState
)
{

    switch (FuncIdx) {
        case UART0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_UART0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_UART0_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case UART1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_UART1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_UART1_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case UART2:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_UART2_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_UART2_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case SPI0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_SPI0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_SPI0_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case SPI1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_SPI1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_SPI1_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case SPI2:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_SPI2_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0) & BIT_SOC_ACTCK_SPI2_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2C0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2C0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2C0_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2C1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2C1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2C1_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2C2:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2C2_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2C2_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2C3:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2C3_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2C3_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2S0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2S0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2S_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case I2S1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_I2S1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_I2S_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case PCM0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_PCM0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_PCM_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case PCM1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN) & BIT_PERI_PCM1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_PCM_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case ADC0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC1_EN) & BIT_PERI_ADC0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_ADC_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case DAC0:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC1_EN) & BIT_PERI_DAC0_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_DAC_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case DAC1:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC1_EN) & BIT_PERI_DAC1_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL1) & BIT_SOC_ACTCK_DAC_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case SDIOD:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & BIT_SOC_HCI_SDIOD_ON_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) & BIT_SOC_ACTCK_SDIO_DEV_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case SDIOH:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & BIT_SOC_HCI_SDIOH_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) & BIT_SOC_ACTCK_SDIO_HST_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case USBOTG:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & BIT_SOC_HCI_OTG_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) & BIT_SOC_ACTCK_OTG_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case MII:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN) & BIT_SOC_HCI_MII_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0) & BIT_SOC_ACTCK_MII_MPHY_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        case PWM0:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_PWM0_CTRL) & BIT_PERI_PWM0_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case PWM1:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_PWM1_CTRL) & BIT_PERI_PWM1_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case PWM2:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_PWM2_CTRL) & BIT_PERI_PWM2_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case PWM3:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_PWM3_CTRL) & BIT_PERI_PWM3_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case ETE0:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_TIM_EVT_CTRL) & BIT_PERI_GT_EVT0_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case ETE1:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_TIM_EVT_CTRL) & BIT_PERI_GT_EVT1_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case ETE2:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_TIM_EVT_CTRL) & BIT_PERI_GT_EVT2_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case ETE3:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_TIM_EVT_CTRL) & BIT_PERI_GT_EVT3_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case EGTIM:
            if (HAL_READ32(PERI_ON_BASE, REG_PERI_EGTIM_CTRL) & BIT_PERI_EGTIM_EN) {
                *HwState = HWACT;
            } else {
                *HwState = HWINACT;
            }
            break;

        case LOG_UART:
            if (HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN) & BIT_SOC_LOG_UART_EN) {
                if (HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_CTRL) & BIT_SOC_ACTCK_LOG_UART_EN) {
                    *HwState = HWACT;
                } else {
                    *HwState = HWCG;
                }
            } else {
                *HwState = HWINACT;
            }
            break;

        default:
            *HwState = UNDEF;
            break;
    }

}

VOID
QueryRegPwrState(
    IN  u8  FuncIdx,
    OUT u8* RegState,
    OUT u8* HwState
)
{
    u8 Idx = 0;
    u8 StateIdx = INACT;

    for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {
        if (PwrAdapter.PwrState[Idx].FuncIdx == FuncIdx) {
            StateIdx = PwrAdapter.PwrState[Idx].PowerState;
            break;
        }
    }

    *RegState = StateIdx;
    ReadHWPwrState(FuncIdx, HwState);
}


VOID
SetSYSTimer(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 ScaleTemp = 0;
    u32 PeriodTemp = 0;
    u32 CalTemp = 0;

    //0x4000_0090[15] = 1'b0 => Disable timer
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, 0);

    //calculate scale and period
    CalTemp = (CLKCal(ANACK) << 16);
    DurationScaleAndPeriodOP(SDuration, &ScaleTemp, &PeriodTemp);

    Rtemp = ((CalTemp | ScaleTemp) | PeriodTemp);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);
}

VOID
SleepCG(
    IN  u8  Option,
    IN  u32 SDuration,
    IN  u8  ClkSourceEn,
    IN  u8  SDREn

)
{
    u32 Backupvalue = 0;
    u32 Rtemp = 0;
    u32 WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0);

    Backupvalue = HAL_READ32(0x8000E000, 0x60000100);
    HAL_WRITE32(0x60008000, 0x80006180, PS_MASK);

    //Backup CPU CLK
    BackupCPUClk();

    //Clear event
    PwrAdapter.WakeEventFlag = _FALSE;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0));

    //3 2 Configure power state option:
    // 2.1 power mode option:
    if (ClkSourceEn) {
        Rtemp = 0x74003b00;
    } else {
        Rtemp = 0x74000900;
    }

    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 2.2  sleep power mode option1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00)|0x2);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    if (Option & SLP_STIMER) {

        //Set TU timer timescale
        SetSYSTimer(SDuration);

        //0x4000_0090[15] = 1'b1 => Enable timer
        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

        //Enable wake event
        WakeEvent |= BIT0;
    }

    if (Option & SLP_GTIMER) {

        //Enable wake event
        WakeEvent |= BIT1;
    }

    if (Option & SLP_GPIO) {

        //Enable wake event
        WakeEvent |= BIT4;
    }

    if (Option & SLP_WL) {

        //Enable wake event
        WakeEvent |= BIT8;
    }

    if (Option & SLP_NFC) {

        //Enable wake event
        WakeEvent |= BIT28;
    }

    if (Option & SLP_SDIO) {

        //Enable wake event
        WakeEvent |= BIT14;
    }

    if (Option & SLP_USB) {

        //Enable wake event
        //WakeEvent |= BIT16;
    }

    if (Option & SLP_TIMER33) {

        //Enable wake event
        WakeEvent |= BIT28;
    }

    while(1) {

        HalDelayUs(100);

        if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {

            break;
        }
    }

    //Set Event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, WakeEvent);

    //3 Enable low power mode
    //Enable low power mode:
    if ((*((volatile u8*)(&PwrAdapter.WakeEventFlag)))!= _TRUE) {

        PwrAdapter.SDREn = SDREn;
        if (PwrAdapter.SDREn) {
#ifdef CONFIG_SDR_EN
            SDRSleep();
#endif
        }

        Rtemp = 0x00000004;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

        //3 Wait CHIP enter low power mode
        // Wait deep standby timer timeout
        // Wait CHIP resume to norm power mode
        HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
        //__WFI();

        HalDelayUs(300);
        HAL_WRITE32(0x60008000, 0x80006100, Backupvalue);
    }
}


VOID
SleepPG(
    IN  u8  Option,
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0);

    //Backup CPU CLK
    BackupCPUClk();

    //Clear event
    PwrAdapter.WakeEventFlag = _FALSE;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0));

    //3 2 Configure power state option:
    // 2.1 power mode option:
    Rtemp = 0x74000100;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 2.2  sleep power mode option1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00)|0x2);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    if (Option & SLP_STIMER) {

        //Set TU timer timescale
        SetSYSTimer(SDuration);

        //0x4000_0090[15] = 1'b1 => Enable timer
        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

        //Enable wake event
        WakeEvent |= BIT0;
    }

    if (Option & SLP_GTIMER) {

        //Enable wake event
        WakeEvent |= BIT1;
    }

    if (Option & SLP_GPIO) {

        //Enable wake event
        WakeEvent |= BIT4;
    }

    if (Option & SLP_WL) {

        //Enable wake event
        WakeEvent |= BIT8;
    }

    if (Option & SLP_NFC) {

        //Enable wake event
        WakeEvent |= BIT28;
    }

    if (Option & SLP_SDIO) {

        //Enable wake event
        WakeEvent |= BIT14;
    }

    if (Option & SLP_USB) {

        //Enable wake event
        //WakeEvent |= BIT16;
    }

    if (Option & SLP_TIMER33) {

        //Enable wake event
        WakeEvent |= BIT28;
    }

    while(1) {

        HalDelayUs(100);

        if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {

            break;
        }
    }

    //Set Event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, WakeEvent);

    //3 Enable low power mode
    //Enable low power mode:
    if (PwrAdapter.WakeEventFlag != _TRUE) {

#ifdef CONFIG_SDR_EN
        LDO25M_CTRL(OFF);
#endif

        Rtemp = 0x00000004;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

        //3 Wait CHIP enter low power mode
        // Wait deep standby timer timeout
        // Wait CHIP resume to norm power mode
        HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
        __WFI();
    }
}



VOID
DSTBYGpioCtrl(
    IN u8 PinEn,
    IN u8 WMode
)
{
    u32 Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_GPIO_DSTBY_WAKE_CTRL0)|PinEn|(PinEn<<16)|(PinEn<<24));
    u32 Stemp = (PinEn<<8);

    if (WMode) {
        Rtemp = (Rtemp|Stemp);
    } else {
        Rtemp = (Rtemp & (~Stemp));
    }

    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_GPIO_DSTBY_WAKE_CTRL0, Rtemp);

    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_GPIO_DSTBY_WAKE_CTRL1)|PinEn|(PinEn<<16)|BIT9);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_GPIO_DSTBY_WAKE_CTRL1, Rtemp);
}


VOID
DeepStandby(
    IN  u8  Option,
    IN  u32 SDuration,
    IN  u8  GpioOption
)
{
    u32 Rtemp = 0;

    HAL_WRITE32(0x60008000, 0x80006180, PS_MASK);

    //Clear event
    PwrAdapter.WakeEventFlag = _FALSE;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0));

    //3 2 Configure power state option:
    // 2.1  deep standby power mode option:
    Rtemp = 0x74000100;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION, Rtemp);

    // 2.2   sleep power mode option1
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT) & 0xffffff00));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_OPTION_EXT, Rtemp);

    if (Option & DSTBY_STIMER) {

        //3 3.1 Set TU timer timescale
        SetSYSTimer(SDuration);

        //3 3.2 Configure platform wake event
        // 1.3.1   0x4000_0100[0] = 1'b1 => Enable deep standby timer wakeup event to wakeup CPU
        Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0)|BIT0);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);

        //0x4000_0090[15] = 1'b1 => Enable timer
        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);
    }

    if (Option & DSTBY_NFC) {
        //Enable wake event
        Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0)|BIT28);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);
    }

    if (Option & DSTBY_TIMER33) {
        //Enable wake event
        Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0)|BIT28);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);
    }

    if (Option & DSTBY_GPIO) {

        if (GpioOption & BIT0) {
            DSTBYGpioCtrl(BIT0, (GpioOption & BIT4));
        }

        if (GpioOption & BIT1) {
            DSTBYGpioCtrl(BIT1, (GpioOption & BIT5));
        }

        if (GpioOption & BIT2) {
            DSTBYGpioCtrl(BIT2, (GpioOption & BIT6));
        }

        if (GpioOption & BIT3) {
            DSTBYGpioCtrl(BIT3, (GpioOption & BIT7));
        }

        //Enable wake event
        if (GpioOption & 0xF) {
            Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0)|BIT29);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);
        }
    }


    //3 Enable low power mode
    //Enable low power mode:
    if (PwrAdapter.WakeEventFlag != _TRUE) {

        SpicDeepPowerDownFlashRtl8195A();

#ifdef CONFIG_SDR_EN
        LDO25M_CTRL(OFF);
#endif

        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_GPIO_SHTDN_CTRL, 0x0);

        Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN) & 0xBFFFFFFF);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN, Rtemp);

        Rtemp = 0x00000002;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

        //3 Wait CHIP enter low power mode
        // Wait deep standby timer timeout
        // Wait CHIP resume to norm power mode
        HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
        __WFI();
    }
}



VOID
DeepSleep(
    IN  u8  Option,
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 UTemp = 0;
    u32 MaxTemp = 0;

    HAL_WRITE32(0x60008000, 0x80006180, PS_MASK);

    //1.1.1   Enable REGU access interface 0x4000_0094[31] = 1
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //1.1.2 0x4000_0094[15:0] = 16'hD300 => Disable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D300);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};

    //1.1.3 Clear event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT));

    if (Option & DS_TIMER33) {
        //2.1.1 Calibration A33 CLK
        UTemp = CLKCal(A33CK);

        //Calculate the max value base on the a33 duration
        MaxTemp = ((((0x7FFFFF/3)*500)/UTemp)*25);

        if ( SDuration >= MaxTemp ) {
            SDuration = 0x7FFFFF;
        } else {
            //In unit of A33 CLK : max num is bounded by anaclk = 1.5k
            SDuration = ((((SDuration/3)*500)/UTemp)*25);
        }

        //2.1.2   Initialize deep sleep counter
        //2.1.3.1 0x4000_0094[15:0] = 16'h9008 => set counter[7:0]
        Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009000 | ((u8)SDuration));
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

        while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};

        //2.1.3.2 0x4000_0094[15:0] = 16'h9100 => set counter[15:8]
        Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009100 | ((u8)(SDuration >> 8)));
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

        while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};

        //2.1.3.3 0x4000_0094[15:0] = 16'h9200 => set counter[22:16]
        Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009200 | ((u8)(SDuration >> 16)));
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

        while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};

        //2.1.3.4 0x4000_0094[15:0] = 16'hD380 => Enable deep sleep counter
        Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D380);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

        while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};
    }

    if (Option & DS_GPIO) {
        //2.2 en GPIO
        Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009410);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
        while(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15) {};
    }


    //0x4000_0100[28] = 1'b1 => Enable A33 wakeup event to wakeup CPU
    PwrAdapter.WakeEventFlag = _FALSE;
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0)|BIT28);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);

    //3 2.3
    //2.3 Enable low power mode: 0x4000_0118[0] = 1'b1;
    if (PwrAdapter.WakeEventFlag != _TRUE) {

        SpicDeepPowerDownFlashRtl8195A();

#ifdef CONFIG_SDR_EN
        LDO25M_CTRL(OFF);
#endif

        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_GPIO_SHTDN_CTRL, 0x0);

        Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN) & 0xBFFFFFFF);
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_FUNC_EN, Rtemp);

        Rtemp = 0x00000001;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000001;
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

        //2.4 Wait CHIP enter deep sleep mode
        //2.5 Wait deep sleep counter timeout
        HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
        __WFI();
    }
}


VOID
DSleep_GPIO(
    VOID
)
{
    u32 Rtemp = 0;

    //1.1 Clear event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT));

    //2 Deep Sleep mode:
    //3 2.2 Configure GPIO:
    //2.2.1   Enable REGU access interface 0x4000_0094[31] = 1
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009410);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2
    //2.3 Enable low power mode: 0x4000_0118[0] = 1'b1;
    Rtemp = 0x00000001;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000001;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //2.4 Wait CHIP enter deep sleep mode
    //2.5 Wait deep sleep counter timeout
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();
}

VOID
DSleep_Timer(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    u32 UTemp = 0;
    u32 MaxTemp = 0;

    //2 Deep Sleep mode:
    //3 2.1 Set TU timer timescale

    //3 2.2 Configure deep sleep timer:
    //2.2.1   Enable REGU access interface 0x4000_0094[31] = 1
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2 0x4000_0094[15:0] = 16'hD300 => Disable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D300);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.3 Clear event
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT, HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_WEVENT));

    //2.2.4 Calibration A33 CLK
    UTemp = CLKCal(A33CK);

    //Calculate the max value base on the a33 duration
    MaxTemp = 0x7FFFFF*0x100/100000*UTemp/100*0x80;

    if ( SDuration >= MaxTemp ) {
        SDuration = 0x7FFFFF;
    } else {
        //In unit of A33 CLK : max num is bounded by anaclk = 1.5k
        SDuration = ((((SDuration)/UTemp)*25/16*25/16*125));
    }

    //DiagPrintf("SDuration : 0x%x\n", SDuration);

    //2.2.5   Initialize deep sleep counter
    //2.2.5.1 0x4000_0094[15:0] = 16'h9008 => set counter[7:0]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009000 | ((u8)SDuration));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.5.2 0x4000_0094[15:0] = 16'h9100 => set counter[15:8]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009100 | ((u8)(SDuration >> 8)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.5.3 0x4000_0094[15:0] = 16'h9200 => set counter[22:16]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009200 | ((u8)(SDuration >> 16)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.5.4 0x4000_0094[15:0] = 16'hD380 => Enable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D380);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //HalDelayUs(1000);
    //Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CAL_CTRL);
    //DiagPrintf("a33 timer : 0x%x\n", Rtemp);
    HalDelayUs(8000);

    //3 2.3
    //2.3 Enable low power mode: 0x4000_0118[0] = 1'b1;
    Rtemp = 0x00000001;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000001;
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

    //2.4 Wait CHIP enter deep sleep mode
    //2.5 Wait deep sleep counter timeout
    HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL);
    __WFI();
}


VOID
SoCPwrReinitProcess(
    VOID
)
{
    //clear resume jumping condition
    HAL_WRITE32(PERI_ON_BASE, 0x218, (HAL_READ32(PERI_ON_BASE,0x218)&(~BIT31)));

#ifdef CONFIG_KERNEL
    InterruptForOSInit((VOID*)vPortSVCHandler,
                       (VOID*)xPortPendSVHandler,
                       (VOID*)xPortSysTickHandler);
#endif

    //msp stack
    MSReFillProcess();

    //init sys timer
    ( * ( ( volatile unsigned long * ) 0xe000e014 ) ) = 0xc34f;//portNVIC_SYSTICK_LOAD_REG
    ( * ( ( volatile unsigned long * ) 0xe000e010 ) ) = 0x10007;//portNVIC_SYSTICK_CTRL_REG

    //3 Reinit SYS int
    {
        IRQ_HANDLE          SysHandle;
        PSYS_ADAPTER        pSYSAdapte;
        pSYSAdapte          = &SYSAdapte;
        SysHandle.Data       = (u32) (pSYSAdapte);
        SysHandle.IrqNum     = SYSTEM_ON_IRQ;
        SysHandle.IrqFun     = (IRQ_FUN) SYSIrqHandle;
        SysHandle.Priority   = 0;

        InterruptRegister(&SysHandle);
        InterruptEn(&SysHandle);
    }
    //DiagPrintf("REINIT IRQ0!!!!!!!!!!\n");
    //HAL_WRITE32(0xE000ED00, 0x14, 0x200);

}


VOID
SoCEnterPS(
    VOID
)
{
}


VOID
SoCPWRIdleTaskHandle(
    VOID
)
{
    //static u32 IdleLoopCount = 0;
    static u32 IdleCount = 0;
    //u8 Chktemp = 0;
    //u32 CMDTemp[6];
    //u32 Rtemp,Rtemp1,Rtemp2;

    //IdleCount++;
    //HalDelayUs(1000);
    //if ((IdleCount > 5000)||(HAL_READ8(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0+1) == 0x12))
    if (HAL_READ8(SYSTEM_CTRL_BASE, 0xf2) == 0xda) {//        {

        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SOC_FUNC_EN, (HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_FUNC_EN)|BIT29));

        HAL_WRITE32(SYSTEM_CTRL_BASE,0xf0,0);

#if 0 //slp pg
        //backup cpu reg
        CPURegBackUp();

        //backup main stack
        MSBackupProcess();

        //Wait for LogUart print out
        while(1) {
            HalDelayUs(100);
            if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {
                break;
            }
        }

        SoCPSGPIOCtrl();

        ChangeSoCPwrState(SLPPG, 0xFFFFF);

        asm volatile
        (
            "SLPPG_WAKEUP_POINT:\n"
        );

        SoCPwrReinitProcess();

        //DiagPrintf("idle~~~~~~~~~~~~~~~~~\n");
        DiagPrintf("SLP_PG = %d\n", HAL_READ32(SYSTEM_CTRL_BASE,0xf8));
#endif
        asm volatile
        (
            "SLPPG_WAKEUP_POINT:\n"
        );

#if 1 //dslp
        //Wait for LogUart print out
        while(1) {
            HalDelayUs(100);
            if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {
                break;
            }
        }

        ChangeSoCPwrState(DSTBY, 0xFFFFF);
#endif

    }

    if (IdleCount>500) {
        IdleCount = 0;
        if (HAL_READ32(SYSTEM_CTRL_BASE,0xf4) ==0) {
            HAL_WRITE32(SYSTEM_CTRL_BASE,0xf0,HAL_READ32(SYSTEM_CTRL_BASE,0xf0)|0xda0000);
            HAL_WRITE32(SYSTEM_CTRL_BASE,0xf8,HAL_READ32(SYSTEM_CTRL_BASE,0xf8)+1);
            DiagPrintf("DSTBY = %d\n", HAL_READ32(SYSTEM_CTRL_BASE,0xf8));
        }
        //DiagPrintf("idle~~~~~~~~~~~~~~~~~\n");
    } else {
        HalDelayUs(100000);
        IdleCount++;
    }
}

#ifdef CONFIG_SOC_PS_VERIFY
#if 0
VOID
SoCPwrDecision(
    void
)
{
    u8 Idx       = 0;
    u8 StateIdx  = 0;
    u8 State     = _TRUE;
    u8 NextState = 0;
    u32 CurrentCount, RemainCount, PTTemp;

    if ( PwrAdapter.ActFuncCount ) {

        //update remaining count
        CurrentCount = HalTimerOp.HalTimerReadCount(1);

        for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {

            if (PwrAdapter.PwrState[Idx].RegCount < CurrentCount) {
                PTTemp = (0xFFFFFFFF - CurrentCount + PwrAdapter.PwrState[Idx].RegCount);
            } else {
                PTTemp = (PwrAdapter.PwrState[Idx].RegCount - CurrentCount);
            }

            if ( PTTemp < PwrAdapter.PwrState[Idx].ReqDuration ) {
                PwrAdapter.PwrState[Idx].RemainDuration = PwrAdapter.PwrState[Idx].ReqDuration - PTTemp;
            } else {
                //active this function
                if ( PwrAdapter.PwrState[Idx].PowerState > SLPPG ) {
                    //Todo: re-initial function as GPIO wake
                }
                PwrAdapter.PwrState[Idx].PowerState = ACT;
                PwrAdapter.PwrState[Idx].RemainDuration = 0;
                PwrAdapter.PwrState[Idx].ReqDuration = 0;
            }
        }

        //Select next power mode
        for (StateIdx = DSLP; StateIdx >= ACT; StateIdx--) {

            for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {

                State = _TRUE;
                if (PwrAdapter.PwrState[Idx].PowerState < StateIdx) {
                    State = _FALSE;
                    break;
                }
            }

            if ( State ) {
                NextState = StateIdx;
                break;
            }
        }

        //fine min sleep time
        RemainCount = PwrAdapter.PwrState[0].RemainDuration;
        for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {

            if ( RemainCount > PwrAdapter.PwrState[Idx].RemainDuration ) {

                RemainCount = PwrAdapter.PwrState[Idx].RemainDuration;
            }
        }

        //for debug
#if 1
        DiagPrintf("RemainCount : 0x%x \n", RemainCount);
        DiagPrintf("NextState : 0x%x \n", NextState);
#endif
#if 0
        //Change state
        if ( NextState > SLPCG ) {
            if ( RemainCount > 640 ) {
                ChangeSoCPwrState(NextState, RemainCount);
            } else {
                ChangeSoCPwrState(SLPCG, RemainCount);
            }
        } else {
            if (NextState != ACT ) {
                ChangeSoCPwrState(NextState, RemainCount);
            }
        }
#endif
    } else {
        //todo: go to DSLP
    }
}


VOID
RegPowerState(
    REG_POWER_STATE RegPwrState
)
{
    u8 Idx = 0;
    u8 StateIdx;
    u8 FState = 0;

    for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {
        if (PwrAdapter.PwrState[Idx].FuncIdx == RegPwrState.FuncIdx) {
            StateIdx = Idx;
            FState = _TRUE;
        }
    }

    switch (RegPwrState.PwrState) {

        case INACT :
            if (FState) {
                for (Idx = StateIdx; Idx < PwrAdapter.ActFuncCount; Idx++) {
                    PwrAdapter.PwrState[Idx].FuncIdx     = PwrAdapter.PwrState[Idx+1].FuncIdx;
                    PwrAdapter.PwrState[Idx].PowerState  = PwrAdapter.PwrState[Idx+1].PowerState;
                    PwrAdapter.PwrState[Idx].ReqDuration = PwrAdapter.PwrState[Idx+1].ReqDuration;
                    PwrAdapter.PwrState[Idx].RegCount    = PwrAdapter.PwrState[Idx+1].RegCount;
                }
                PwrAdapter.ActFuncCount--;
            } else {
            }
            break;

        default:

            if (FState) {
                PwrAdapter.PwrState[StateIdx].PowerState    = RegPwrState.PwrState;
                PwrAdapter.PwrState[StateIdx].ReqDuration   = RegPwrState.ReqDuration;
                PwrAdapter.PwrState[StateIdx].RegCount      = HalTimerOp.HalTimerReadCount(1);
            } else {
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].FuncIdx        = RegPwrState.FuncIdx;
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].PowerState     = RegPwrState.PwrState;
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].ReqDuration    = RegPwrState.ReqDuration;
                PwrAdapter.PwrState[PwrAdapter.ActFuncCount].RegCount       = HalTimerOp.HalTimerReadCount(1);
                PwrAdapter.ActFuncCount++;
            }

            break;
    }

    //for debug
#if 1
    for (Idx = 0; Idx < PwrAdapter.ActFuncCount; Idx++) {
        DiagPrintf("RegPwrIdx : %d \n", Idx);
        DiagPrintf("FuncIdx : %d \n", PwrAdapter.PwrState[Idx].FuncIdx);
        DiagPrintf("PowerState : 0x%x \n", PwrAdapter.PwrState[Idx].PowerState);
        DiagPrintf("ReqDuration : 0x%x \n", PwrAdapter.PwrState[Idx].ReqDuration);
        DiagPrintf("RegCount : 0x%x \n", PwrAdapter.PwrState[Idx].RegCount);
    }
#endif
}
#endif

#if 0
VOID
En32KCalibration(
    VOID
)
{
    u32 Rtemp;
    u32 Ttemp = 0;

    while(1) {

        //set parameter
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
        Rtemp = 0x80f880;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);

        DiagPrintf("cal en\n");

        //Polling LOCK
        Rtemp = 0x110000;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
        DiagPrintf("polling lock\n");

        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL1);
        if ((Rtemp & 0x3000) != 0x0) {
            break;
        } else {
            Ttemp++;
            DiagPrintf("check lock: %d\n", Ttemp);
        }
    }

    Rtemp = 0x884000;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
}
#endif

VOID
SYSTestIrqHandle
(
    IN  VOID        *Data
)
{
    u32 Rtemp;
    static u32 Ttemp = 0;

    //change cpu clk
    ReFillCpuClk();
    HalDelayUs(100);

    //JTAG rst pull high
    HAL_WRITE32(PERI_ON_BASE, REG_GPIO_PULL_CTRL2, 0x0202aaaa);

    //release shutdone
    //HAL_WRITE32(PERI_ON_BASE, REG_GPIO_SHTDN_CTRL, 0x7ff);

    //disable DSTBY timer
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, 0);

    //clear wake event IMR
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, 0);

    //clear wake event ISR
    Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_STATUS0, Rtemp);

    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO0, Ttemp);

    //DiagPrintf("Ttemp : %d\n", Ttemp);

    Ttemp++;
    //Rtemp = HalTimerOp.HalTimerReadCount(1);
    //DiagPrintf("32k counter : %x\n", Rtemp);//32k counter :
    //DiagPrintf("\n");

    //PwrAdapter.SleepFlag = 1;
    //DiagPrintf("\n");
    //DiagPrintf("0x234 after slp : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,0x234)+1);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO0, HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0)+1);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO1, HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO1)+1);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO2, HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO2)+1);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO3, HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO3)+1);
    //DiagPrintf("f0 counter : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0));
    //DiagPrintf("f1 counter : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO1));
    //DiagPrintf("f2 counter : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO2));
    //DiagPrintf("f3 counter : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO3));
    //DiagPrintf("\n");
    //DiagPrintf("ya ~~~~\n");

    PwrAdapter.WakeEventFlag = _TRUE;
}

VOID
InitSYSTestIRQ(VOID)
{
    IRQ_HANDLE          SysHandle;
    PSYS_ADAPTER        pSYSAdapte;
    pSYSAdapte          = &SYSAdapte;
    SysHandle.Data       = (u32) (pSYSAdapte);
    SysHandle.IrqNum     = SYSTEM_ON_IRQ;
    SysHandle.IrqFun     = (IRQ_FUN) SYSIrqHandle;
    SysHandle.Priority   = 0;

    InterruptRegister(&SysHandle);
    InterruptEn(&SysHandle);
    PwrAdapter.WakeEventFlag = _FALSE;
}

VOID
SetA33Timer(
    IN  u32 SDuration
)
{
    u32 Rtemp = 0;
    //u32 ScaleTemp = 0;
    //u32 PeriodTemp = 0;
    u32 UTemp = 0;
    u32 MaxTemp = 0;

    //2 Deep Sleep mode:
    //3 2.1 Set TU timer timescale

    //3 2.2 Configure deep sleep timer:
    //2.2.1   Enable REGU access interface 0x4000_0094[31] = 1
    Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2 Calibration A33 CLK
    UTemp = CLKCal(A33CK);
    DiagPrintf("CAL : 0x%x\n", UTemp);

    //Calculate the max value base on the a33 duration
    MaxTemp = 0x7FFFFF*0x100/100000*UTemp/100*0x80;
    DiagPrintf("MaxTemp : 0x%x\n", MaxTemp);

    if ( SDuration >= MaxTemp ) {
        SDuration = 0x7FFFFF;
    } else {
        //In unit of A33 CLK : max num is bounded by anaclk = 1.5k
        SDuration = ((((SDuration)/UTemp)*25/16*25/16*125));
        DiagPrintf("SDuration : 0x%x\n", SDuration);

    }

    //3 2.2.2   Initialize deep sleep counter
    //2.2.2.1 0x4000_0094[15:0] = 16'h9008 => set counter[7:0]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009000 | ((u8)SDuration));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.2 0x4000_0094[15:0] = 16'h9100 => set counter[15:8]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009100 | ((u8)(SDuration >> 8)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.3 0x4000_0094[15:0] = 16'h9200 => set counter[22:16]
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009200 | ((u8)(SDuration >> 16)));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    //2.2.2.4 0x4000_0094[15:0] = 16'hD380 => Enable deep sleep counter
    Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D380);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

    DiagPrintf("a33 timer : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CAL_CTRL));
}


VOID
PrintCPU(
    VOID
)
{

#if 0
    DiagPrintf("r13 : 0x%x\n", PwrAdapter.CPURegbackup[24]);
    DiagPrintf("pc : 0x%x\n", PwrAdapter.CPURegbackup[23]);
    DiagPrintf("control : 0x%x\n", PwrAdapter.CPURegbackup[22]);
    DiagPrintf("psp : 0x%x\n", PwrAdapter.CPURegbackup[21]);
    DiagPrintf("msp : 0x%x\n", PwrAdapter.CPURegbackup[20]);
#endif

#if 0
    u8 i;
    for (i = 0; i < 21; i++) {
        PwrAdapter.CPURegbackup[i] = ( * ( ( volatile unsigned long * ) (PwrAdapter.CPURegbackup[24]+(i*4)) ) );
    }
#endif

    u8 i;
    for (i = 0; i < 25; i++) {
        DiagPrintf("CPURegbackup_idx : %d , 0x%x\n", i, PwrAdapter.CPURegbackup[i]);
    }


#if 1
    for (i = 0; i < 21; i++) {
        DiagPrintf("backup_idx : 0x%x , 0x%x\n", PwrAdapter.CPUPSP+(i*4),( * ( ( volatile unsigned long * ) (PwrAdapter.CPUPSP+(i*4)) ) ));//CPURegbackup[1]
    }
#endif

#if 0
    {
        u32 cpupspc;
        asm volatile
        (
            "MRS %0, PSP\n"
            :"=r"(cpupspc)
            ::"memory"
        );
        for (i = 0; i < 21; i++) {
            DiagPrintf("stack addr : 0x%x , 0x%x\n", (cpupspc+(i*4)),( * ( ( volatile unsigned long * ) (cpupspc+(i*4)) ) ));//CPURegbackup[1]
        }
    }
#endif
}


VOID
SoCPSMEMTestInit(
    IN  u32 StartAddr,
    IN  u32 Length,
    IN  u32 Pattern
)
{
    u32 Idx;
    for( Idx = 0; Idx < Length; Idx += 4 ) {

        HAL_WRITE32(StartAddr,Idx,Pattern);
    }
}

u8
SoCPSMEMTestChk(
    IN  u32 StartAddr,
    IN  u32 Length,
    IN  u32 Pattern
)
{
    u32 Idx;

    for( Idx = 0; Idx < Length; Idx += 4  ) {
        if (HAL_READ32(StartAddr,Idx) != Pattern) {
            DiagPrintf("addr 0x%x fail\n", (StartAddr+Idx));
            return 0;
        }
    }
    DiagPrintf("addr 0x%x pass\n", StartAddr);
    return 1;
}


VOID
SoCPWRIdleTaskHandleTest(
    VOID
)
{
    static u32 IdleTemp = 0;
    u32 Rtemp,Rtemp1,Rtemp2;
    u8 RRtemp,CMDTemp[8],Chktemp;

    if (0) { //(HAL_READ8(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0) == 0x0) {

        IdleTemp++;
        HalDelayUs(1000);

        if (IdleTemp >= 15000) {
            DiagPrintf("\n");
            DiagPrintf("Go to sleep ~~~~ \n");
            HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0,0x12345678);
            DiagPrintf("0xf0 : 0x%x\n",HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0));
            //a33 reg chk
            Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, 0x80008400);
            HalDelayUs(1000);
            if ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15)==0) {
                RRtemp = ((u8)HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL))+1;
            }
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009400|RRtemp);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
            DiagPrintf("a33 0x4 : 0x%x\n",RRtemp);

            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, 0x80008500);
            HalDelayUs(1000);
            if ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL)&BIT15)==0) {
                DiagPrintf("a33 0x5 before : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL));
                RRtemp = ((u8)HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL))+1;
            }
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009500|RRtemp);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
            DiagPrintf("a33 0x5 : 0x%x\n",RRtemp);

            ChangeSoCPwrState(7,0xE8800);
        }
    }

    ////debug
    if (PwrAdapter.SleepFlag) {
        PwrAdapter.SleepFlag = 0;
        HAL_WRITE32(SYSTEM_CTRL_BASE,0x234, 0xdddddddd);
        DiagPrintf("0x234 before slp : %x\n", HAL_READ32(SYSTEM_CTRL_BASE,0x234));
        //cal 32k
        //En32KCalibration();
        HalDelayUs(1000);

        ChangeSoCPwrState(5,0xb000);
        //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_DSTBY_INFO1, PwrAdapter.SleepFlag);
    }

    if (0) { //(HAL_READ8(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0) == 0x0) {

        IdleTemp++;
        HalDelayUs(1000);
        if (IdleTemp > 0xfffff) {
            IdleTemp = 0;
            __WFI();
        }
    }

    if (0) { //((HAL_READ8(SYSTEM_CTRL_BASE,REG_SOC_SYSON_DSTBY_INFO0) == 0x0)) {
        IdleTemp++;
        HalDelayUs(1000);
        if ((IdleTemp > 5000)||(HAL_READ8(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO0+1) == 0x12)) {

            DiagPrintf("\n");
            DiagPrintf("0x20080000 : 0x%x\n", HAL_READ32(0x20080000,0));
            DiagPrintf("0x20080004 : 0x%x\n", HAL_READ32(0x20080000,4));
            DiagPrintf("0x2009F404 : 0x%x\n", HAL_READ32(0x2009F400,4));
            DiagPrintf("0x2009F408 : 0x%x\n", HAL_READ32(0x2009F400,8));
            DiagPrintf("\n");

            HAL_WRITE32(0x40000000,0x330,0x55559555);//0x55552a2a
            //slp pg GPIOD GPIOE
            HAL_WRITE32(0x40000000,0x334,0x55555555);
            HAL_WRITE32(0x40000000,0x338,0x05555555);
            HAL_WRITE32(0x40000000,0x33c,0x55555555);
            HAL_WRITE32(0x40000000,0x340,0x55555555);
            HAL_WRITE32(0x40000000,0x344,0x55555555);
            HAL_WRITE32(0x40000000,0x320,0x0);

            HAL_WRITE32(0x20080000, 0, (HAL_READ32(0x20080000,0)+1));
            HAL_WRITE32(0x20080000, 4, (HAL_READ32(0x20080000,4)+1));
            HAL_WRITE32(0x2009F404, 0, (HAL_READ32(0x2009F400,4)+1));
            HAL_WRITE32(0x2009F408, 0, (HAL_READ32(0x2009F400,8)+1));
            HalDelayUs(10000);
            ChangeSoCPwrState(SLPPG, 0xFFFFF);
        }
    }
    //mem test
    if (HAL_READ8(0x40000000,0xf1) == 0xaa) {

        CMDTemp[0] = 8;
        SOCPSTestApp((VOID*)CMDTemp);
        Rtemp = HAL_READ32(0x40080000,0x824);
        Rtemp2 = Rtemp;
        Rtemp2 = ((Rtemp2 & 0x807fffff) | 0x80000000);
        HAL_WRITE32(0x40080000,0x824,Rtemp&0x7fffffff);
        HAL_WRITE32(0x40080000,0x824,Rtemp2);
        HAL_WRITE32(0x40080000,0x824,(Rtemp|0x80000000));
        Rtemp1 = HAL_READ32(0x40080000,0x820)&BIT8;
        if (Rtemp1) {
            Rtemp = HAL_READ32(0x40080000,0x8b8)&0xfffff;
        } else {
            Rtemp = HAL_READ32(0x40080000,0x8a0)&0xfffff;
        }
        if(Rtemp== 0x00045678) {
            Chktemp = 1;
        }

        Chktemp &= SoCPSMEMTestChk(0x20010000,0x20000,0x12345678)&SoCPSMEMTestChk(0x200a0000,0x0FFE0,0x12345678)
                   &SoCPSMEMTestChk(0x1FFF4000,0x5000,0x12345678);

        if (Chktemp) {
            HAL_WRITE32(0x40080000,0x4,(HAL_READ32(0x40080000,0x4)&0xFFFFFFF0));
            HAL_WRITE32(0x40000000,0xfc,(HAL_READ32(0x40000000,0xfc)+1));
            DiagPrintf("run %d times\n", HAL_READ32(0x40000000,0xfc));
            CMDTemp[0] = 1;
            CMDTemp[1] = 5;
            CMDTemp[2] = 0xff;
            SOCPSTestApp((VOID*)CMDTemp);
        } else {
            HAL_WRITE32(0x40000000,0xf0,0);
        }

    }
}

//30
VOID
TimerHandleTset(
    IN  VOID        *Data
)
{
#if 0
    //static u32 temp = 0;
    TIMER_ADAPTER       TimerAdapter;

    TimerAdapter.IrqDis = OFF;
    //TimerAdapter.IrqHandle = (IRQ_FUN) TimerHandleTset;
    TimerAdapter.IrqHandle.IrqFun = (IRQ_FUN) TimerHandleTset;
    //DBG_8195A("IrqFun : 0x%x\n", TimerAdapter.IrqHandle.IrqFun);
    TimerAdapter.IrqHandle.IrqNum = TIMER2_7_IRQ;
    TimerAdapter.IrqHandle.Priority = 0;
    TimerAdapter.IrqHandle.Data = NULL;
    TimerAdapter.TimerId = 2;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 4000;
    TimerAdapter.TimerMode = USER_DEFINED;
    //temp++;
    //DBG_8195A("time : 0x%x\n", temp);

    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);
#endif
    DBG_8195A("<<<    time out    >>>\n");
}

extern IRQ_FUN Timer2To7VectorTable[6];

//30
VOID
InitTimerTest(
    //IN  VOID *Data
    VOID
)
{
    //[0]:type, [1]: timerID, [2]: timerMode, [3]: IrqDIS, [4]:period
    TIMER_ADAPTER       TimerAdapter;
    //u32 *TestParameter;

    //TestParameter = (u32*) Data;

    TimerAdapter.IrqDis = 0; // off :0
    //TimerAdapter.IrqHandle = (IRQ_FUN) TimerHandleTset;
    TimerAdapter.IrqHandle.IrqFun = (IRQ_FUN) TimerHandleTset;
    //DBG_8195A("IrqFun : 0x%x\n", TimerAdapter.IrqHandle.IrqFun);
    TimerAdapter.IrqHandle.IrqNum = TIMER2_7_IRQ;
    TimerAdapter.IrqHandle.Priority = 0;
    TimerAdapter.IrqHandle.Data = NULL;
    TimerAdapter.TimerId = 5;
    TimerAdapter.TimerIrqPriority = 0;
    TimerAdapter.TimerLoadValueUs = 0x4EC14;
    TimerAdapter.TimerMode = 1; // user_define :1


    //mer2To7VectorTable[0] = (IRQ_FUN) TimerHandleTset;

    HalTimerOp.HalTimerInit((VOID*) &TimerAdapter);
    //mer2To7VectorTable[0] = (IRQ_FUN) TimerHandleTset;


}


VOID
GpioPsPullCtrl(
    VOID
)
{
    gpio_t gpio_obj;

    gpio_init(&gpio_obj, PA_0);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_5);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_6);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PA_7);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PB_0);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PB_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_5);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_6);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PB_7);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PC_0);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_5);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_6);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_7);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_8);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PC_9);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PD_0);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_5);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_6);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_7);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_8);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PD_9);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PE_0);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_5);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_6);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_7);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_8);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_9);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PE_A);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PF_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PF_1);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PF_2);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PF_3);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PF_4);
    gpio_mode(&gpio_obj, PullDown);
    gpio_init(&gpio_obj, PF_5);
    gpio_mode(&gpio_obj, PullDown);

    gpio_init(&gpio_obj, PG_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_3);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_4);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_5);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_6);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PG_7);
    gpio_mode(&gpio_obj, PullUp);

    gpio_init(&gpio_obj, PH_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_3);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_4);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_5);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_6);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PH_7);
    gpio_mode(&gpio_obj, PullUp);

    gpio_init(&gpio_obj, PI_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_3);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_4);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_5);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_6);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PI_7);
    gpio_mode(&gpio_obj, PullUp);

    gpio_init(&gpio_obj, PJ_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_3);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_4);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_5);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PJ_6);
    gpio_mode(&gpio_obj, PullUp);


    gpio_init(&gpio_obj, PK_0);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_1);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_2);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_3);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_4);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_5);
    gpio_mode(&gpio_obj, PullUp);
    gpio_init(&gpio_obj, PK_6);
    gpio_mode(&gpio_obj, PullUp);
}


VOID
SOCPSTestApp(
    VOID *Data
)
{
    u32 *TestParameter;
    TestParameter = (u32*) Data;
    unsigned int Rtemp, Rtemp1, Rtemp2;//, CalTemp32k, CalTempa33;
    static u32 Read32k5 = 0;
    static u32 Reada335 = 0;
    DiagPrintf("TestParameter[0]: 0x%x\n",TestParameter[0]);

    switch (TestParameter[0]) {

        case 0:
            DiagPrintf("SoC PWR Init wlan\n");
            Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,0x214)|BIT16;
            HAL_WRITE32(SYSTEM_CTRL_BASE,0x214,Rtemp);

            Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,0x244)|BIT0;
            HAL_WRITE32(SYSTEM_CTRL_BASE,0x244,Rtemp);

            Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,0x210)|BIT2;
            HAL_WRITE32(SYSTEM_CTRL_BASE,0x210,Rtemp);

            HalDelayUs(100);

            Rtemp = HAL_READ32(WIFI_REG_BASE,0x0)|BIT0;
            HAL_WRITE32(WIFI_REG_BASE,0x0,Rtemp);
#if 0
            DiagPrintf("SoC PWR debug setting\n");
            Rtemp = 0;
            HAL_WRITE32(SYSTEM_CTRL_BASE,0x33c,Rtemp);

            Rtemp = 0;
            HAL_WRITE32(SYSTEM_CTRL_BASE,0x334,Rtemp);

#if 0
            //en debug
            Rtemp = 1;//HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYS_DEBUG_CTRL);
            HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SOC_SYS_DEBUG_CTRL,Rtemp);

            //debug port sel
            Rtemp = 0xf0f10004;//HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_EFUSE_SYSCFG3)|0xf0000000;
            HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SOC_EFUSE_SYSCFG3, Rtemp);
#endif

            //cal 32k
            //En32KCalibration();

            //en gpio
            GPIO_FCTRL(ON);
            SLPCK_GPIO_CCTRL(ON);
            ACTCK_GPIO_CCTRL(ON);

            //DiagPrintf("debug sel 0x2C : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_EFUSE_SYSCFG3));
            //DiagPrintf("debug EN 0xA0 : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYS_DEBUG_CTRL));
            //DiagPrintf("PULL CTRL 0x33c: 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,0x33C));
            //DiagPrintf("debug port : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,REG_SOC_SYS_DEBUG_REG));
            DiagPrintf("0x90 : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,0x90));
#endif
            break;

        case 1:
            DiagPrintf("SoC PWR TEST : Enter = %d, Period = %d\n",TestParameter[1], TestParameter[2]);
            Rtemp = HalTimerOp.HalTimerReadCount(1);
            //GPIO
            //HAL_WRITE32(0x40001000,0x4,0x4000000);

            //SIC EN
            //HAL_WRITE32(0x40000000,0x8,0x81000010);
            //HAL_WRITE32(0x40000000,0xA4,0x00000001);

            //Wait for LogUart print out
            while(1) {
                HalDelayUs(100);
                if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {
                    break;
                }
            }

#if 0

            HAL_WRITE32(0x40000000,0x330,0x55559555);//0x55552a2a
            //slp pg GPIOD GPIOE
            HAL_WRITE32(0x40000000,0x334,0x55555555);
            HAL_WRITE32(0x40000000,0x338,0x05555555);
            HAL_WRITE32(0x40000000,0x33c,0x55555555);
            HAL_WRITE32(0x40000000,0x340,0x55555555);
            HAL_WRITE32(0x40000000,0x344,0x55555555);
            HAL_WRITE32(0x40000000,0x320,0x0);
#endif

            ChangeSoCPwrState(TestParameter[1], TestParameter[2]);

            Rtemp2 = HalTimerOp.HalTimerReadCount(1);
            DiagPrintf("before : %x\n", Rtemp);
            DiagPrintf("after : %x\n", Rtemp2);
            DiagPrintf("period : %d\n", Rtemp-Rtemp2);
            DiagPrintf("0x90 : 0x%x\n", HAL_READ32(SYSTEM_CTRL_BASE,0x90));
            break;

        case 2:
#if 1

            HAL_WRITE32(0x40000000,0x320,0x7ff);
            HAL_WRITE32(0x40000000,0x330,0x5565A555);
            //slp pg GPIOD GPIOE
            HAL_WRITE32(0x40000000,0x334,0x55555555);
            HAL_WRITE32(0x40000000,0x338,0x05555555);
            HAL_WRITE32(0x40000000,0x33c,0x55555555);
            HAL_WRITE32(0x40000000,0x340,0x55555555);
            HAL_WRITE32(0x40000000,0x344,0x55555555);
            HAL_WRITE32(0x40000000,0x348,0x55555555);
            HAL_WRITE32(0x40000000,0x320,0x0);

            HAL_WRITE32(0x40000000,0x8,0x80000011);
#endif
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X120, TestParameter[1]);
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X124, TestParameter[2]);

            if (TestParameter[4] == 0xff) {
                //SIC EN
                HAL_WRITE32(0x40000000,0x320,0x4);
                HAL_WRITE32(0x40000000,0x8,0xC1000010);
                HAL_WRITE32(0x40000000,0xA4,0x00000001);
            }

            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, TestParameter[3]);
#if 0
            //clear isr
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_ANA_TIM_CTRL)& 0xffff7fff));
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYS_ANA_TIM_CTRL, Rtemp);

            Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_SLP_WAKE_EVENT_STATUS0);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYSON_SLP_WAKE_EVENT_STATUS0, Rtemp);
#endif
            break;

        case 3:
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X120, 0x74000e00);
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X124, 2);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, TestParameter[1]);
#if 0
            {
                u32 targetunit = 0;

                //cal a33
                Rtemp = CLKCal(A33CK);

                targetunit = (TestParameter[1]/3*250)/Rtemp*50;
                if (targetunit > 0x7fffff) {
                    targetunit = 0x7fffff;
                }

                DBG_8195A("targeunit = 0x%08x\n",targetunit);

                targetunit = (50*TestParameter[1]/Rtemp)*250/3;
                if (targetunit > 0x7fffff) {
                    targetunit = 0x7fffff;
                }

                DBG_8195A("targeunit = 0x%08x\n",targetunit);


            }
#endif
#if 0
            //cal a33
            Rtemp = CLKCal(A33CK);
            Rtemp1 = (((((TestParameter[1] & 0x0FFFFFFF)<<4)/Rtemp)*20)-1);
            DiagPrintf("Rtemp : 0x%x\n", Rtemp);
            DiagPrintf("Vendor 0xA0 : 0x%x\n", HAL_READ32(VENDOR_REG_BASE,0xA0));
            DiagPrintf("way1 : 0x%x\n", Rtemp1);
            Rtemp2 = (((((TestParameter[1] & 0x0FFFFFFF))/Rtemp)*320)-1);
            DiagPrintf("way2 : 0x%x\n", Rtemp2);

            Rtemp = Rtemp1/6;
            DiagPrintf("Rtemp1 : %d\n", Rtemp);
            Rtemp = 0x7fffffff;
            DiagPrintf("Rtemp1 : %d\n", Rtemp);
#endif
            break;

        case 4:
            DiagPrintf("set timer\n");
            SetA33Timer(TestParameter[1]);
            Rtemp = HalTimerOp.HalTimerReadCount(1);
            DiagPrintf("32k timer : 0x%x\n", Rtemp);
            break;

        case 5:
            DiagPrintf("read timer\n");
            Reada335 = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CAL_CTRL);
            DiagPrintf("a33 timer : 0x%x\n", Reada335);
            Read32k5 = HalTimerOp.HalTimerReadCount(1);
            DiagPrintf("32k timer : 0x%x\n", Read32k5);
            break;

        case 6:
#if 0
            DiagPrintf("interval cal\n");
            Rtemp1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_DSLP_TIM_CAL_CTRL);
            Rtemp2 = HalTimerOp.HalTimerReadCount(1);
            DiagPrintf("Reada335 : 0x%x\n", Reada335);
            DiagPrintf("Read32k5 : 0x%x\n", Read32k5);
            DiagPrintf("a33 timer : 0x%x\n", Rtemp1);
            DiagPrintf("32k timer : 0x%x\n", Rtemp2);
            CalTemp32k = (Read32k5 - Rtemp2);
            CalTempa33 = (((Reada335 - Rtemp1)*((HAL_READ32(VENDOR_REG_BASE, REG_VDR_ANACK_CAL_CTRL) & 0x3FFF)+1))/5);
            DiagPrintf("a33 timer interval : 0x%x\n", CalTempa33);
            DiagPrintf("32k timer interval : 0x%x\n", CalTemp32k);
            Read32k5 = Rtemp2;
            Reada335 = Rtemp1;
#endif
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) & 0xffff7000) | 0x7ff);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

            //0x4000_0090[15] = 1'b1 => Enable timer
            Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL) | 0x00008000;
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_ANA_TIM_CTRL, Rtemp);

            Rtemp = 0x00000001;
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SLP_WAKE_EVENT_MSK0, Rtemp);
#if 0
            HAL_WRITE32(0x40000000,0x330,0x55559555);//0x55552a2a
            HAL_WRITE32(0x40000000,0x2C0,0x100001);
            //slp pg GPIOD GPIOE
            HAL_WRITE32(0x40000000,0x334,0x55555555);
            HAL_WRITE32(0x40000000,0x338,0x05555555);
            HAL_WRITE32(0x40000000,0x33c,0x55555555);
            HAL_WRITE32(0x40000000,0x340,0x55555555);
            HAL_WRITE32(0x40000000,0x344,0x55555555);
            HAL_WRITE32(0x40000000,0x320,0x0);
#endif
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X120, TestParameter[1]);
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X124, TestParameter[2]);

            if (HAL_READ32(0x40000000,0xf4) == 0x11) {
                HAL_WRITE32(0x40000000,0x8,0x80000011);
            }

            if (TestParameter[4] == 0xff) {
                //SIC EN
                HAL_WRITE32(0x40000000,0x8,0x81000010);
                HAL_WRITE32(0x40000000,0xA4,0x00000001);
            }

            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, TestParameter[3]);
            break;

        case 7: {
            u32 Rtemp = 0;
            u32 UTemp = 0;
            u32 MaxTemp = 0;
            u32 Reada335 = 0;

            //2 Deep Sleep mode:
            //3 2.1 Set TU timer timescale

            //3 2.2 Configure deep sleep timer:
            //2.2.1   Enable REGU access interface 0x4000_0094[31] = 1
            Rtemp = (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) | 0x80000000);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

            DiagPrintf("SDuration : 0x%x\n", TestParameter[1]);

            //3 2.2.2   Initialize deep sleep counter
            //2.2.2.0 0x4000_0094[15:0] = 16'hD300 => Disable deep sleep counter
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D300);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);
            //2.2.2.1 0x4000_0094[15:0] = 16'h9008 => set counter[7:0]
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009000 | ((u8)TestParameter[1]));
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

            //2.2.2.2 0x4000_0094[15:0] = 16'h9100 => set counter[15:8]
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009100 | ((u8)(TestParameter[1] >> 8)));
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

            //2.2.2.3 0x4000_0094[15:0] = 16'h9200 => set counter[22:16]
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x00009200 | ((u8)(TestParameter[1] >> 16)));
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

            //2.2.2.4 0x4000_0094[15:0] = 16'hD380 => Enable deep sleep counter
            Rtemp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL) & 0xffff0000) | 0x0000D380);
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CTRL, Rtemp);

            HalDelayUs(1000);
            Reada335 = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_DSLP_TIM_CAL_CTRL);
            DiagPrintf("a33 timer : 0x%x\n", Reada335);

            HalDelayUs(8000);

            //3 2.2.3
            //2.3 Enable low power mode: 0x4000_0118[0] = 1'b1;
            Rtemp = 0x00000001;//HAL_READ32(SYSTEM_CTRL_BASE, REG_SYSON_PWRMGT_CTRL) | 0x00000001;
            HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PWRMGT_CTRL, Rtemp);

            //2.4 Wait CHIP enter deep sleep mode
            //2.5 Wait deep sleep counter timeout
            //__WFI();

            DiagPrintf("YOU CAN'T SEE ME ~~~~!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~!!!!!!!!!!");
        }
        break;

        case 8:
            DiagPrintf("enable wifi\n");

            Rtemp = HAL_READ32(0x40000000,0x214)|0x10000;
            HAL_WRITE32(0x40000000,0x214,Rtemp);
            Rtemp = HAL_READ32(0x40000000,0x244)|0x1;
            HAL_WRITE32(0x40000000,0x244,Rtemp);
            Rtemp = HAL_READ32(0x40000000,0x210)|0x4;
            HAL_WRITE32(0x40000000,0x210,Rtemp);

            Rtemp = HAL_READ32(0x40080000,0x0)&0xFFFFFFDF;
            HAL_WRITE32(0x40080000,0x0,Rtemp);
            Rtemp = HAL_READ32(0x40080000,0x4)|0x1;
            HAL_WRITE32(0x40080000,0x4,Rtemp);
            Rtemp = HAL_READ32(0x40080000,0x20)|0x1;
            HAL_WRITE32(0x40080000,0x20,Rtemp);
            while( (HAL_READ32(0x40080000,0x20)&BIT0)!=0);

            Rtemp = HAL_READ32(0x40080000,0x4)|0x30000;
            HAL_WRITE32(0x40080000,0x4,Rtemp);
            Rtemp = HAL_READ32(0x40080000,0x4)|0x7000000;
            HAL_WRITE32(0x40080000,0x4,Rtemp);
            Rtemp = HAL_READ32(0x40080000,0x50)&0xFFFFFF00;
            HAL_WRITE32(0x40080000,0x50,Rtemp);
            break;

        case 9:
#if 0
            PwrAdapter.CPURegbackup[13] = 0x12340;
            PwrAdapter.CPUPSP = PwrAdapter.CPURegbackup[13];

            asm volatile
            (

                "	ldr	r3, pxCPUPSPConst23		    \n" /* Restore the context. */
                "MOV %0, r3\n"
                "	ldr r1, [r3]					\n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
                "MOV %1, r1\n"
                "	ldr r0, [r1]					\n"
                "MOV %2, r0\n"
                "	.align 2						\n"
                "pxCPUPSPConst23: .word PwrAdapter.CPUPSP				\n"
                :"=r"(PwrAdapter.CPURegbackup[0]),"=r"(PwrAdapter.CPURegbackup[1]),"=r"(PwrAdapter.CPURegbackup[2]),"=r"(PwrAdapter.CPURegbackup[3])
                :"r"(PwrAdapter.CPUPSP)
                :"memory"
            );
            PrintCPU();
#endif
            break;

        case 10:
            Rtemp = HAL_READ32(0x40080000,0x824);
            Rtemp2 = Rtemp;
            Rtemp2 = Rtemp2 & 0x807fffff | (TestParameter[1]<<23) | 0x80000000;
            HAL_WRITE32(0x40080000,0x824,Rtemp&0x7fffffff);
            HAL_WRITE32(0x40080000,0x824,Rtemp2);
            HAL_WRITE32(0x40080000,0x824,Rtemp|0x80000000);
            Rtemp1 = HAL_READ32(0x40080000,0x820)&BIT8;
            if (Rtemp1) {
                Rtemp = HAL_READ32(0x40080000,0x8b8)&0xfffff;
            } else {
                Rtemp = HAL_READ32(0x40080000,0x8a0)&0xfffff;
            }
            DiagPrintf("rf offset: 0x%x, 0x%x\n", TestParameter[1], Rtemp);
            break;

        case 11://addr [1]; date [2]
            TestParameter[1] &= 0x3f;
            Rtemp = (TestParameter[1]<<20)|(TestParameter[2]&0x000fffff)&0x0fffffff;
            HAL_WRITE32(0x40080000,0x840,Rtemp);

            //SoCPWRIdleTaskHandle();
            break;

        case 12:
            SoCPSMEMTestInit(TestParameter[1],TestParameter[2],TestParameter[3]);
            break;

        case 13:
            Rtemp = SoCPSMEMTestChk(TestParameter[1],TestParameter[2],TestParameter[3]);
            break;

        case 14:
            HAL_WRITE32(0x40000000,TestParameter[1],0x12345678);
            DiagPrintf("w32: 0x%x\n", HAL_READ32(0x40000000,TestParameter[1]));
            HAL_WRITE32(0x40000000,TestParameter[1],0);
            HAL_WRITE16(0x40000000,TestParameter[1],0x1234);
            DiagPrintf("w16: 0x%x\n", HAL_READ32(0x40000000,TestParameter[1]));
            HAL_WRITE32(0x40000000,TestParameter[1],0);
            HAL_WRITE8(0x40000000,TestParameter[1],0x12);
            DiagPrintf("w8: 0x%x\n", HAL_READ32(0x40000000,TestParameter[1]));
            HAL_WRITE32(0x40000000,TestParameter[1],0x12345678);
            DiagPrintf("R32: 0x%x\n", HAL_READ32(0x40000000,TestParameter[1]));
            DiagPrintf("R16: 0x%x\n", HAL_READ16(0x40000000,TestParameter[1]));
            DiagPrintf("R8: 0x%x\n", HAL_READ8(0x40000000,TestParameter[1]));
            Rtemp = ((HAL_READ32(0x40000000,0xf4))?1:0);
            DiagPrintf("R: 0x%x\n", Rtemp);
            break;

        case 15:
            asm volatile
            (
                "MRS R0, BASEPRI\n"
                "MOV %0, R0\n"
                :"=r"(Rtemp)
                ::"memory"
            );
            DiagPrintf("basepri: 0x%x\n", Rtemp);
            break;
        case 16:
            HalDelayUs(10000000);
            DSleep_GPIO();
            break;
        case 17:
            DSleep_Timer(TestParameter[1]);
            break;
        case 18:
            DiagPrintf("WDG CAL\n");
            {
                u8 CountId;
                u16 DivFactor;
                u32 CountTemp;
                u32 CountProcess = 0;
                u32 DivFacProcess = 0;
                u32 MinPeriodTemp = 0xFFFFFFFF;
                u32 PeriodTemp = 0;

                DBG_8195A(" Period = %d\n", TestParameter[1]);

                for (CountId = 0; CountId < 12; CountId++) {
                    CountTemp = ((0x00000001 << (CountId+1))-1);
                    DivFactor = (u16)((100*TestParameter[1])/(CountTemp*3));

                    if (DivFactor > 0) {
                        PeriodTemp = 3*(DivFactor+1)*CountTemp;
                        DBG_8195A("PeriodTemp = %d\n", PeriodTemp);
                        if ((100*TestParameter[1])<PeriodTemp) {
                            if (MinPeriodTemp > PeriodTemp) {
                                MinPeriodTemp = PeriodTemp;
                                CountProcess = CountTemp;
                                DivFacProcess = DivFactor;
                            }
                        }
                    }
                }
                DBG_8195A("MinPeriodTemp = %d\n", MinPeriodTemp);
                DBG_8195A("WdgScalar = 0x%08x\n", DivFacProcess);
                DBG_8195A("WdgCunLimit = 0x%08x\n", CountProcess);
            }
            break;

        case 19:
            DBG_8195A("DeepStandby~~~\n");
            DeepStandby(TestParameter[1],TestParameter[2],TestParameter[3]);
            break;

        case 20:
            DBG_8195A("SleepCG~~~\n");
            if (TestParameter[1]&BIT1) {
                InitTimerTest();
            }
            SleepCG(TestParameter[1],TestParameter[2],TestParameter[3],TestParameter[4]);
            break;

        case 25: {
            //dslp
            DBG_8195A("DSLP~~~\n");

            HalDelayUs(3000000);

            GpioPsPullCtrl();

            DeepSleep(TestParameter[1],TestParameter[2]);
        }
        break;

        case 26:
            //dstby
            DBG_8195A("DSTBY~~~\n");

            GpioPsPullCtrl();

            DeepStandby(TestParameter[1],TestParameter[2],TestParameter[3]);
            break;

        case 28:
            //slpcg
            DBG_8195A("SLPCG~~~\n");
            while(1) {

                HalDelayUs(100);

                if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {

                    break;
                }
            }
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0X2c0, 0x0);

            GpioPsPullCtrl();

            SleepCG(TestParameter[2],TestParameter[3],TestParameter[4],TestParameter[5]);
            break;

        default:
            break;
    }


}
#endif  //CONFIG_SOC_PS_VERIFY
#endif  //CONFIG_SOC_PS_MODULE

