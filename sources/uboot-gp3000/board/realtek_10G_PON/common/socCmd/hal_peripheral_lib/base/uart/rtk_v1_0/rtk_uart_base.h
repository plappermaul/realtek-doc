/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTK_UART_BASE_H_
#define _RTK_UART_BASE_H_

#include "hal_api.h"

#define RUART_DLL_OFF                   0x00
#define RUART_DLM_OFF                   0x04    //RW, DLAB = 1
#define RUART_INTERRUPT_EN_REG_OFF      0x04
#define RUART_IER_ERBI                  0x01    //BIT0, Enable Received Data Available Interrupt (rx trigger)
#define RUART_IER_ETBEI                 (1<<1)  //BIT1, Enable Transmitter FIFO Empty Interrupt (tx fifo empty)
#define RUART_IER_ELSI                  (1<<2)  //BIT2, Enable Receiver Line Status Interrupt (receiver line status)
#define RUART_IER_EDSSI                 (1<<3)  //BIT3, Enable Modem Status Interrupt (modem status transition)

#define RUART_INT_ID_REG_OFF            0x08    //[R]
#define RUART_IIR_INT_PEND              0x01
#define RUART_IIR_INT_ID                (0x07<<1) //011(3), 010(2), 110(6), 001(1), 000(0)
#define RUART_FIFO_CTL_REG_OFF          0x08    //[W]
#define RUART_FIFO_CTL_REG_CLEAR_RXFIFO (1<<1)  //BIT1, 0x02, Write 1 clear
#define RUART_FIFO_CTL_REG_DMA_ENABLE   0x08    //BIT3

#define FIFO_CTL_DEFAULT_WITH_FIFO_DMA  0xC9
#define FIFO_CTL_DEFAULT_WITH_FIFO      0xC1

#define RUART_MODEM_CTL_REG_OFF         0x10
#define RUART_MCR_RTS                   BIT1
#define RUART_MCR_LOOPBACK              (1<<4)  //BIT4, 0x10
#define RUART_MCL_AUTOFLOW_ENABLE       (1<<5)  //BIT5, 0x20

#define RUART_LINE_CTL_REG_OFF          0x0C
#define RUART_LINE_CTL_REG_DLAB_ENABLE  (1<<7)  //BIT7, 0x80

#define RUART_LINE_STATUS_REG_OFF       0x14
#define RUART_LINE_STATUS_REG_DR        0x01    //BIT0, Data Ready indicator
#define RUART_LINE_STATUS_REG_THRE      (1<<5)  //BIT5, 0x20, Transmit Holding Register Empty Interrupt enable
#define RUART_LINE_STATUS_REG_TEMT      (1<<6)  //BIT6, 0x40, Transmitter Empty indicator(bit)
#define RUART_LINE_STATUS_REG_OE		0x2


#define RUART_MODEM_STATUS_REG_OFF      0x18    //Modem Status Register
#define	RUART_MODEM_STATUS_REG_CTS		0x1			
#define RUART_MODEM_STATUS_REG_DSR		0x2
#define RUART_MODEM_STATUS_REG_DCD		0x8
#define	RUART_MODEM_STATUS_REG_RCTS		0x10	

#define RUART_SCRATCH_PAD_REG_OFF       0x1C    //Scratch Pad Register
#define RUART_SP_REG_PIN_LB_TEST        0x8     //BIT3, 0x08, Write 1 to enable PIN Loopback test.
#define RUART_SP_REG_RXBREAK_INT_STATUS (1<<7)  //BIT7, 0x80, Write 1 clear
#define RUART_SP_REG_DBG_SEL            0x0F<<8 //[11:8], Debug port selection
#define RUART_SP_REG_XFACTOR_ADJ        0x7FF<<16  //[26:16]

#define RUART_STS_REG_OFF               0x20
#define RUART_STS_REG_RESET_RCV         (1<<3)  //BIT3, 0x08, Reset Uart Receiver
#define RUART_STS_REG_XFACTOR           0xF<<4

#if 1 //8277C
#define RUART_REV_BUF_REG_OFF           0x0  //Receiver Buffer Register
#define RUART_TRAN_HOLD_REG_OFF         0x0  //Transmitter Holding Register
#else
#define RUART_REV_BUF_REG_OFF           0x24  //Receiver Buffer Register
#define RUART_TRAN_HOLD_REG_OFF         0x24  //Transmitter Holding Register
#endif

#define RUART_MISC_CTL_REG_OFF          0x28
#define RUART_TXDMA_BURSTSIZE_MASK      0xF8    //7:3
#define RUART_RXDMA_BURSTSIZE_MASK      0x1F00  //12:8

#if 1 //8277C
#define RUART_DATA_READ_CTL_REG_OFF     0x38
#define RUART_DATA_READ_CTL_REG_FIFO_SYNC_EN     0x1
#endif

#define RUART_DEBUG_REG_OFF             0x3C

#define RUART_BAUD_RATE_2400     2400
#define RUART_BAUD_RATE_4800     4800
#define RUART_BAUD_RATE_9600     9600
#define RUART_BAUD_RATE_19200    19200
#define RUART_BAUD_RATE_38400    38400
#define RUART_BAUD_RATE_57600    57600
#define RUART_BAUD_RATE_115200   115200
#define RUART_BAUD_RATE_921600   921600
#define RUART_BAUD_RATE_1152000  1152000

#define HAL_RUART_READ32(UartIndex, addr)    \
    HAL_READ32(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr)
#define HAL_RUART_WRITE32(UartIndex, addr, value)    \
    HAL_WRITE32(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr, value)
#define HAL_RUART_READ16(UartIndex, addr)    \
    HAL_READ16(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr)
#define HAL_RUART_WRITE16(UartIndex, addr, value)    \
    HAL_WRITE16(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr, value)
#define HAL_RUART_READ8(UartIndex, addr)    \
    HAL_READ8(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr)
#define HAL_RUART_WRITE8(UartIndex, addr, value)    \
    HAL_WRITE8(UART0_REG_BASE+ (UartIndex*RUART_REG_OFF), addr, value)

#define G2UART_CFG_REG_OFF               0x0
#define G2UART_CFG_REG_CHAR_CNT          0x3
#define G2UART_CFG_REG_TX_SM_ENABLE      (1<<5) //BIT5, 0x20
#define G2UART_CFG_REG_RX_SM_ENABLE      (1<<6) //BIT6, 0x40
#define G2UART_CFG_REG_UART_EN           (1<<7) //BIT7, 0x80
#define G2UART_CFG_REG_BAUD_COUNT        0xFFFFFF<<8 //[31:8]

#define G2UART_FC_REG_OFF                0x4
#define G2UART_FC_REG_RX_WM              (1<<3) //BIT3, 0x8
#define G2UART_FC_REG_NO_CTS             (1<<7) //BIT7, 0x80
#define G2UART_FC_REG_CTS_REG            (1<<8) //BIT8, 0x100
#define G2UART_FC_REG_NO_RTS             (1<<10) //BIT10, 0x400

#define G2UART_RX_SAMPLE_REG_OFF         0x8
#define G2UART_RX_SAMPLE_REG_RX_CENTER   0x6c

#define G2UART_TXDAT_REG_OFF             0x10 //UART Transmit data
#define G2UART_RXDAT_REG_OFF             0x14 //UART Receive data

#define G2UART_INFO_REG_OFF              0x18
#define G2UART_INFO_REG_RX_FIFO_EMPTY    (1<<1) //BIT1, 0x2
#define G2UART_INFO_REG_TX_FIFO_EMPTY    (1<<3) //BIT3, 0x8

#define G2UART_IE_REG_OFF                0x1c
#define G2UART_IE_REG_RX_FIFO_NONEMPTYE  (1<<6) //BIT6, 0x40

#define G2UART_INT_REG_OFF               0x24
#define G2UART_INT_REG_RX_FIFO_NONEMPTYI (1<<6) //BIT6

#define HAL_G2UART_READ32(UartIndex, addr)    \
    HAL_READ32(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr)
#define HAL_G2UART_WRITE32(UartIndex, addr, value)    \
    HAL_WRITE32(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr, value)
#define HAL_RUART_READ16(UartIndex, addr)    \
    HAL_READ16(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr)
#define HAL_RUART_WRITE16(UartIndex, addr, value)    \
    HAL_WRITE16(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr, value)
#define HAL_G2UART_READ8(UartIndex, addr)    \
    HAL_READ8(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr)
#define HAL_G2UART_WRITE8(UartIndex, addr, value)    \
    HAL_WRITE8(G2UART0_REG_BASE+ (UartIndex*G2UART_REG_OFF), addr, value)

typedef enum _UART_RXFIFO_TRIGGER_LEVEL_ {
    OneByte       = 0x00,
    FourBytes     = 0x01,
    EightBytes    = 0x10,
    FourteenBytes = 0x11
}UART_RXFIFO_TRIGGER_LEVEL, *PUART_RXFIFO_TRIGGER_LEVEL;

typedef enum _RUART0_PINMUX_SELECT_ {
    RUART0_MUX_TO_GPIOC   = DW_MUX_0,
    RUART0_MUX_TO_GPIOE   = DW_MUX_1,
    RUART0_MUX_TO_GPIO_GK = DW_MUX_2
}RUART0_PINMUX_SELECT, *PRUART0_PINMUX_SELECT;

typedef enum _RUART1_PINMUX_SELECT_ {
    RUART1_MUX_TO_GPIOD = DW_MUX_0,
    RUART1_MUX_TO_GPIOE = DW_MUX_1,
    RUART1_MUX_TO_GPIOH = DW_MUX_2
}RUART1_PINMUX_SELECT, *PRUART1_PINMUX_SELECT;

typedef enum _RUART2_PINMUX_SELECT_ {
    RUART2_MUX_TO_GPIOA = DW_MUX_0,
    RUART2_MUX_TO_GPIOI = DW_MUX_1
}RUART2_PINMUX_SELECT, *PRUART2_PINMUX_SELECT;

typedef enum _RUART_FLOW_CONTROL_ {
    AUTOFLOW_DISABLE = 0,
    AUTOFLOW_ENABLE  = 1
}RUART_FLOW_CONTROL, *PRUART_FLOW_CONTROL;

u32
HalRuartGetDebugValueRTKCommon(
        IN VOID* Data,
        IN u32    DbgSel
        );

u32
FindElementIndex(
        u32 Element,
        u32* Array
        );

VOID
RuartResetRxFifoRTKCommon(
        IN u8 UartIndex
        );

VOID
RuartBusDomainEnableRTKCommon(
        IN u8 UartIndex
        );

BOOL
HalRuartResetRxFifoRTKCommon(
        IN VOID *Data
        );

BOOL
HalRuartInitRTKCommon(
        IN VOID *Data
        );

VOID
SetUartDmaBurstSize(
        IN u8 UartIndex
        );

VOID
HalRuartSendRTKCommon(
        IN VOID *Data
        );

u8
HalRuartRecvRTKCommon(
        IN VOID *Data
        );

u32
HalRuartGetInterruptEnRegRTKCommon(
        IN VOID *Data
        );

VOID
HalRuartSetInterruptEnRegRTKCommon(
        IN VOID *Data
        );

VOID
HalRuartDmaInitRTKCommon(
        IN VOID *Data
        );

VOID
HalRuartRequestToSendRTKCommon(
        IN VOID *Data
        );

#endif //_UART_BASE_H_

