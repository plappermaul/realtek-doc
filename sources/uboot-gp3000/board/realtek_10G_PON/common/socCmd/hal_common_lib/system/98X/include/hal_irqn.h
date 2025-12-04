/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_IRQN_H_
#define _HAL_IRQN_H_

//#include <asm/gic.h>
#if 1
typedef enum _IRQn_Type_ {



#if 0
    NON_MASKABLE_INT_IRQ            = -14,
    HARD_FAULT_IRQ                  = -13,
    MEM_MANAGE_FAULT_IRQ            = -12,
    BUS_FAULT_IRQ                   = -11,
    USAGE_FAULT_IRQ                 = -10,
    SVCALL_IRQ                      = -5,
    DEBUG_MONITOR_IRQ               = -4,
    PENDSVC_IRQ                     = -2,
    SYSTICK_IRQ                     = -1,
/******  RTL8195A Specific Interrupt Numbers ************/

    SYSTEM_ON_IRQ                   =  0,
    WDG_IRQ                         =  1,
    TIMER0_IRQ                      =  2,
    TIMER1_IRQ                      =  3,
    TIMER2_7_IRQ                    =  5,
    SPI0_IRQ                        =  6,
    GPIO_IRQ                        =  7,
    UART0_IRQ                       =  8,
    SPI_FLASH_IRQ                   =  9,
    USB_OTG_IRQ                     =  10,
    SDIO_HOST_IRQ                   =  11,
    SDIO_DEVICE_IRQ                 =  12,
    I2S0_PCM0_IRQ                   =  13,
    I2S1_PCM1_IRQ                   =  14,
    WL_DMA_IRQ                      =  15,
    WLXXX1_IRQ                      =  16,
    WL_PROTOCOL_IRQ                 =  17,
    WLXXX3_IRQ                      =  18,
    PERIPHERAL_IRQ                  =  19,
#endif     

#if 1
    PER_UART1_INT_0_IRQ             = 37, // 32+5
    INT_DMA_UART0_IRQ               = 59, // 32+27
    INT_DMA_UART1_IRQ               = 60, // 32+28
    DW_SSI_0_IP_IRQ                 = 61, // 32+29
    DW_GDMA_IP_IRQ                  = 63, // 32+31
    I2S_IRQ                         = 64, // 32+32
    RTC_IRQ                         = 86, // 32+54
	CRYPTO_IRQ                      = 89, // 32+57
#endif

    TIMER0_IRQ                      =  2,
    TIMER1_IRQ                      =  3,
    TIMER2_7_IRQ                    =  5,

    GDMA0_CHANNEL0_IRQ              =  20,
    GDMA0_CHANNEL1_IRQ              =  21,
    GDMA0_CHANNEL2_IRQ              =  22,
    GDMA0_CHANNEL3_IRQ              =  23,
    GDMA0_CHANNEL4_IRQ              =  24,
    GDMA0_CHANNEL5_IRQ              =  25,
    GDMA1_CHANNEL0_IRQ              =  26,
    GDMA1_CHANNEL1_IRQ              =  27,
    GDMA1_CHANNEL2_IRQ              =  28,
    GDMA1_CHANNEL3_IRQ              =  29,
    GDMA1_CHANNEL4_IRQ              =  30,
    GDMA1_CHANNEL5_IRQ              =  31,
    WLAN_IRQ                        =  51,

    I2C0_IRQ                        =  5,//RTK_I2C_IP,
    I2C1_IRQ                        =  6,//RTK_INT_DW_I2C0,
    I2C2_IRQ                        =  7, //RTK_INT_DW_I2C1,
    I2C3_IRQ                        =  4,    
    SPI1_IRQ                        =  72,// 8 + 64,
    SPI2_IRQ                        =  73,// 9 + 64,
    UART1_IRQ                       =  80,// 16 + 64,
    UART2_IRQ                       =  81,// 17 + 64,
    UART_LOG_IRQ                    =  88,// 24 + 64,
    ADC_IRQ                         =  89,// 25 + 64,
    DAC0_IRQ                        =  91,// 27 + 64,
    DAC1_IRQ                        =  92,// 28 + 64,
    RXI300_IRQ                      =  93// 29 + 64

} IRQn_Type, *PIRQn_Type;

#endif 
typedef VOID (*HAL_VECTOR_FUN) (VOID);

typedef enum _VECTOR_TABLE_TYPE_{
    DEDECATED_VECTRO_TABLE,
    PERIPHERAL_VECTOR_TABLE
}VECTOR_TABLE_TYPE, *PVECTOR_TABLE_TYPE;


typedef u32 (*IRQ_FUN)(VOID *Data);

typedef struct _IRQ_HANDLE_ {
    IRQ_FUN     IrqFun;
    IRQn_Type   IrqNum;
    u32         Data;
    u32         Priority;
}IRQ_HANDLE, *PIRQ_HANDLE;


#endif //_HAL_IRQN_H_
