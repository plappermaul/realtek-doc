/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_V2_UART_BASE_H_
#define _RTK_V2_UART_BASE_H_

#include "hal_api.h"


#define RTKUART_DLL_OFF                   0x00
#define RTKUART_DLM_OFF                   0x04    //RW, DLAB = 1
#define RTKUART_INTERRUPT_EN_REG_OFF      0x04
#define RTKUART_IER_ERBI                  (1<<24)    //BIT24, Enable Received Data Available Interrupt (rx trigger)
#define RTKUART_IER_ETBEI                 (1<<25)  //BIT25, Enable Transmitter Holding Register Empty Interrupt (tx fifo empty)
#define RTKUART_IER_ELSI                  (1<<26)  //BIT26, Enable Receiver Line Status Interrupt (receiver line status)
#define RTKUART_IER_EDSSI                 (1<<27)  //BIT27, Enable Modem Status Interrupt (modem status transition)

#define RTKUART_INT_ID_REG_OFF            0x08    //[R]
#define RTKUART_IIR_INT_PEND              (1<<24)
#define RTKUART_IIR_INT_ID                0x07	//(0x07<<1) //011(3), 010(2), 110(6), 001(1), 000(0)
#define RTKUART_FIFO_CTL_REG_OFF          0x08    //[W]
#define RTKUART_FIFO_CTL_REG_CLEAR_RXFIFO (1<<25)  //BIT25,  Write 1 clear
//#define RTKUART_FIFO_CTL_REG_DMA_ENABLE   0x08    //BIT3

//#define RTKFIFO_CTL_DEFAULT_WITH_FIFO_DMA  0xC9
#define RTKFIFO_CTL_DEFAULT_WITH_FIFO      0xC1000000

#define RTKUART_MODEM_CTL_REG_OFF         0x10
#define RTKUART_MCR_RTS                   (1<<25)
#define RTKUART_MCL_AUTOFLOW_ENABLE       (1<<29)  //BIT29,
#define RTKUART_MSR_CTS					BIT27
#define RTKUART_MSR_DSR					BIT26
#define RTKUART_MSR_DCD					BIT24

#define RTKUART_LINE_CTL_REG_OFF          0x0C
#define RTKUART_LINE_CTL_REG_DLAB_ENABLE  (1<<31)  //BIT31, 

#define RTKUART_LINE_STATUS_REG_OFF       0x14
#define RTKUART_LINE_STATUS_REG_DR        (1<<24)    //BIT24, Data Ready indicator
#define RTKUART_LINE_STATUS_REG_OE        (1<<25)	//BIT25,Overrun Error (OE) indicator
#define RTKUART_LINE_STATUS_REG_THRE      (1<<29)  //BIT29, Transmit Holding Register Empty Interrupt enable
#define RTKUART_LINE_STATUS_REG_TEMT      (1<<30)  //BIT30, Transmitter Empty indicator(bit)

#define RTKUART_MODEM_STATUS_REG_OFF      0x18    //Modem Status Register
#define RTKUART_SCRATCH_PAD_REG_OFF       0x1C    //Scratch Pad Register
//----no other SCRATCH_PAD BIT definition, SCRATCH_PAD is no use here
//#define RTKUART_SP_REG_RXBREAK_INT_STATUS (1<<7)  //BIT7, 0x80, Write 1 clear
//#define RTKUART_SP_REG_DBG_SEL            0x0F<<8 //[11:8], Debug port selection
//#define RTKUART_SP_REG_XFACTOR_ADJ        0x7FF<<16  //[26:16]
//----no STS register here
//#define RTKUART_STS_REG_OFF               0x20
//#define RTKUART_STS_REG_RESET_RCV         (1<<3)  //BIT3, 0x08, Reset Uart Receiver
//#define RTKUART_STS_REG_XFACTOR           0xF<<4

#define RTKUART_REV_BUF_REG_OFF           0x00  //Receiver Buffer Register
#define RTKUART_TRAN_HOLD_REG_OFF         0x00  //Transmitter Holding Register
//----no MISC register here
//#define RTKUART_MISC_CTL_REG_OFF          0x28
//#define RTKUART_TXDMA_BURSTSIZE_MASK      0xF8    //7:3
//#define RTKUART_RXDMA_BURSTSIZE_MASK      0x1F00  //12:8
//----no DEBUG register here
//#define RTKUART_DEBUG_REG_OFF             0x3C

#define RTKUART_BAUD_RATE_2400     2400
#define RTKUART_BAUD_RATE_4800     4800
#define RTKUART_BAUD_RATE_9600     9600
#define RTKUART_BAUD_RATE_19200    19200
#define RTKUART_BAUD_RATE_38400    38400
#define RTKUART_BAUD_RATE_57600    57600
#define RTKUART_BAUD_RATE_115200   115200
#define RTKUART_BAUD_RATE_921600   921600
#define RTKUART_BAUD_RATE_1152000  1152000

#define HAL_RTKUART_READ32(UartIndex, addr)    \
    HAL_READ32(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr)
#define HAL_RTKUART_WRITE32(UartIndex, addr, value)    \
    HAL_WRITE32(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr, value)
#define HAL_RTKUART_READ16(UartIndex, addr)    \
    HAL_READ16(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr)
#define HAL_RTKUART_WRITE16(UartIndex, addr, value)    \
    HAL_WRITE16(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr, value)
#define HAL_RTKUART_READ8(UartIndex, addr)    \
    HAL_READ8(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr)
#define HAL_RTKUART_WRITE8(UartIndex, addr, value)    \
    HAL_WRITE8(RTKUART0_REG_BASE+ (UartIndex*RTKUART_REG_OFF), addr, value)


typedef enum _RTKUART0_PINMUX_SELECT_ {
    RTKUART0_MUX_TO_GPIOC   = S0,
    RTKUART0_MUX_TO_GPIOE   = S1,
    RTKUART0_MUX_TO_GPIO_GK = S2
}RTKUART0_PINMUX_SELECT, *PRTKUART0_PINMUX_SELECT;

typedef enum _RTKUART1_PINMUX_SELECT_ {
    RTKUART1_MUX_TO_GPIOD = S0,
    RTKUART1_MUX_TO_GPIOE = S1,
    RTKUART1_MUX_TO_GPIOH = S2
}RTKUART1_PINMUX_SELECT, *PRTKUART1_PINMUX_SELECT;

typedef enum _RTKUART2_PINMUX_SELECT_ {
    RTKUART2_MUX_TO_GPIOA = S0,
    RTKUART2_MUX_TO_GPIOI = S1
}RTKUART2_PINMUX_SELECT, *PRTKUART2_PINMUX_SELECT;

typedef enum _RTKUART_FLOW_CONTROL_ {
    AUTOFLOW_DISABLE = 0,
    AUTOFLOW_ENABLE  = 1
}RTKUART_FLOW_CONTROL, *PRTKUART_FLOW_CONTROL;

u32
HalRTKuartGetDebugValueCommon(
        IN VOID* Data,
        IN u32    DbgSel
        );

u32
RTKFindElementIndex(
        u32 Element,
        u32* Array
        );

VOID
RTKuartResetRxFifoCommon(
        IN u8 UartIndex
        );

VOID
RTKuartBusDomainEnableCommon(
        IN u8 UartIndex
        );

BOOL
HalRTKuartResetRxFifoCommon(
        IN VOID *Data
        );

BOOL
HalRTKuartInitCommon(
        IN VOID *Data
        );


VOID
HalRTKuartSendCommon(
        IN VOID *Data
        );

u8
HalRTKuartRecvCommon(
        IN VOID *Data
        );

u32
HalRTKuartGetInterruptEnRegCommon(
        IN VOID *Data
        );

VOID
HalRTKuartSetInterruptEnRegCommon(
        IN VOID *Data
        );
#if 0 //no DMA here
VOID
HalRTKuartDmaInitCommon(
        IN VOID *Data
        );
#endif
VOID
HalRTKuartRequestToSendCommon(
        IN VOID *Data
        );


#endif //_UART_BASE_H_

