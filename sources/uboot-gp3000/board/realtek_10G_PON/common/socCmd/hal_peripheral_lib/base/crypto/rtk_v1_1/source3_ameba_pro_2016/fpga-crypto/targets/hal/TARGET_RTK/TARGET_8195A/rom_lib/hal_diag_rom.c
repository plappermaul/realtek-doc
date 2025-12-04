/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"

extern u32 HalGetCpuClk(VOID);

#if 0
UART_T  Uart;
UINT8 is_uart_init;
UINT8   uart_rev_len;
UINT16 uart_dbg_wptr;
UINT8 uart_dbg_data[REV_FIFO_LEN];

static void  S3C2410UartTrans_TxData(void)
{
    UCHAR           *buf_rd_ptr;
    UINT16          len_available_in_fifo;
    UINT16          length_to_write;
    UINT32          n = 0;

// Disable UART1 interrupts
    /*
    Interrupt enable Register
    7: THRE Interrupt Mode Enable
    2: Enable Receiver Line Status Interrupt
    1: Enable Transmit Holding Register Empty Interrupt
    0: Enable Received Data Available Interrupt
    */
// disable all interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0);

    buf_rd_ptr = (UCHAR *)(Uart.DataPtr + Uart.LenWritten);

// FIFO available length
    len_available_in_fifo = UART_FIFO_MAX_LENGTH - UART_READ(UART_TFL_OFF);
    length_to_write = (Uart.Len - Uart.LenWritten);

    if (length_to_write > len_available_in_fifo) {
        length_to_write = len_available_in_fifo;
    }
    Uart.LenWritten += length_to_write;

// Load the data into the UART1 fifo
    for(n=0; n<length_to_write; n++) {
        // push data to UART FIFO
        UART_WRITE(UART_SEND_BUF_OFF, buf_rd_ptr[n]);
    }

// enable UART1 Interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x83);

    return;
}

void S3C2410_UART_Reset_tx_path(void)
{
    Uart.LenWritten = 0;
    Uart.Len = 0;
    Uart.State = UART_FREE;
}

BOOLEAN S3C2410UartTransInit(UART_RX_FUNC_PTR uart_rx_func,
                             UART_TX_COMP_FUNC_PTR tx_comp_func)
{
    Uart.State = UART_FREE;

    /* Populate the Global UART structure with the call backs */
    Uart.TxCompFunc = tx_comp_func;
    Uart.RxFunc = uart_rx_func;
    Uart.DataPtr = NULL;
    Uart.Len = 0;
    Uart.LenWritten = 0;
// UART init function
    if(uart_use_48M_clk==0) {
        UARTInit(otp_str_data.SYS_hci_uart_baudrate, UART_PARITY_DISABLE, UART_STOP_1BIT,
                 UART_DATA_LEN_8BIT, 0xc1, 0x01);
    }
#ifndef _ENABLE_EFUSE_
    else {
        UARTInit(otp_str_data.SYS_48M_hci_uart_baudrate, UART_PARITY_DISABLE, UART_STOP_1BIT,
                 UART_DATA_LEN_8BIT, 0xc1, 0x01);
    }
#endif
    return TRUE;
}

BOOLEAN S3C2410UartTransWr(HCI_TRANSPORT_PKT_TYPE pkt_type, UCHAR *buf, UINT16 len)
{
    /* Check the state of the UART to see if it is free */
    if(Uart.State != UART_FREE) {
        /* Platform logger transport not initialized yet!! */
        BZ_ASSERT_NOL(0);
        return FALSE;
    }

    Uart.State = UART_TRANSMITTING;

    /* Store the Data to be written in the UART's buffer */
    Uart.DataPtr = buf;
    Uart.Len = len;
    Uart.LenWritten = 0;

    UART_WRITE(UART_SEND_BUF_OFF, pkt_type);

    S3C2410UartTrans_TxData();

    return TRUE;
}

void MINT_trans_uart_tx(void)
{
// disable UART1 Interrupts

    if(Uart.Len != 0) {
        if(Uart.LenWritten == Uart.Len) { // One FULL Packet has been Transmitted
            UINT16 len;

            len = Uart.Len;
            // RESET the Transport Logic Structures for a NEW Pkt.
            Uart.LenWritten = 0;
            Uart.Len = 0;
            Uart.State = UART_FREE;

            // Call the callback to indicate that the previous pkt
            // transmission has finished
            Uart.TxCompFunc(Uart.DataPtr, len, 0);
            UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x01);
        } else {
            S3C2410UartTrans_TxData();
        }
    } else {
        Uart.LenWritten = 0;
        Uart.Len = 0;
        Uart.State = UART_FREE;
        UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x01);
    }
}

BOOLEAN uart_trans_transport_is_free(void)
{
    return (BOOLEAN)(Uart.State == UART_FREE);
}

#ifdef ENABLE_LOGGER
static void S3C2410UartLog_TxData(void)
{
    UCHAR           *buf_rd_ptr;
    UINT16          len_available_in_fifo;
    UINT16          length_to_write;
    UINT32          n = 0;

// Disable UART1 interrupts
    /*
    Interrupt enable Register
    7: THRE Interrupt Mode Enable
    2: Enable Receiver Line Status Interrupt
    1: Enable Transmit Holding Register Empty Interrupt
    0: Enable Received Data Available Interrupt
    */
// disable all interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0);

    buf_rd_ptr = (UCHAR *)(Uart.DataPtr + Uart.LenWritten);

// FIFO available length
    len_available_in_fifo = UART_FIFO_MAX_LENGTH-UART_READ(UART_TFL_OFF);
    length_to_write = (Uart.Len - Uart.LenWritten);

    if (length_to_write > len_available_in_fifo) {
        length_to_write = len_available_in_fifo;
    }
    Uart.LenWritten += length_to_write;

// Load the data into the UART1 fifo
    for(n=0; n<length_to_write; n++) {
        // push data to UART FIFO
        UART_WRITE(UART_SEND_BUF_OFF, buf_rd_ptr[n]);
    }

// enable UART1 Interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x87);


    return;
}

BOOLEAN uart_logger_transport_is_free(void)
{
    if (IS_SUPPORT_HCI_LOG_PKT &&
            (dbg_vendor_log_interface != VENDOR_LOG_PACKET_TYPE_INVALID)) {
        return (BOOLEAN)(dbg_vendor_log_buf == NULL);
    } else {
        return (BOOLEAN)(Uart.State == UART_FREE);
    }
}

BOOLEAN S3C2410UartLogInit(UART_TX_COMP_FUNC_PTR tx_comp_func,
                           UART_DBG_RECD_FUNC_PTR dbg_rx_func)
{
    if (tx_comp_func == NULL) {
        Uart.State = UART_NOT_INIT;
    } else {
        Uart.State = UART_FREE;
    }

    /* Populate the Global UART structure with the call backs */
    Uart.TxCompFunc = tx_comp_func;
#ifdef _UART_RX_ENABLE_
    Uart.RxFunc = dbg_rx_func;
#endif
    Uart.DataPtr = NULL;
    Uart.Len = 0;
    Uart.LenWritten = 0;

// UART init function
    UARTInit(otp_str_data.SYS_log_uart_baudrate, UART_PARITY_DISABLE, UART_STOP_1BIT,
             UART_DATA_LEN_8BIT, 0x81, 0x05);

    return TRUE;
}

BOOLEAN S3C2410UartLogWr(UCHAR *buf, UINT16 len)
{
    /* Check the state of the UART to see if it is free */
    if (Uart.State != UART_FREE) {
        /* Platform logger transport not initialized yet!! */
        BZ_ASSERT_NOL(0);
        return FALSE;
    }

    Uart.State = UART_TRANSMITTING;
    /* Store the Data to be written in the UART's buffer */
    Uart.DataPtr = buf;
    Uart.Len = len;
    Uart.LenWritten = 0;

    S3C2410UartLog_TxData();

    return TRUE;
}

void MINT_logger_uart_tx(void)
{
    if (Uart.Len != 0) {
        if (Uart.LenWritten == Uart.Len) { // One FULL Packet has been Transmitted
            UINT16 len;

            len = Uart.Len;
            // RESET the Transport Logic Structures for a NEW Pkt.
            Uart.LenWritten = 0;
            Uart.Len = 0;
            Uart.State = UART_FREE;

            // Call the callback to indicate that the previous pkt
            // transmission has finished
            Uart.TxCompFunc(Uart.DataPtr, len, 0);
        } else {
            S3C2410UartLog_TxData();
        }
    } else {
        Uart.LenWritten = 0;
        Uart.Len = 0;
        Uart.State = UART_FREE;
    }
}

#endif // ENABLE_LOGGER


// UART interrupt handler
SECTION_ISR_LOW void UartIntrHandler(void)
{
    UINT32 interrupt_status = 0;
    UINT32 fifo_enable = 0;
    UINT32 line_status = 0;
    UINT32 read_data = 0;
    UINT8  data_len = 0;
    UINT8  index = 0;
    UINT16 wptr;


// UART interrupt
    interrupt_status = UART_READ(UART_INTERRUPT_IDEN_REG_OFF);
    fifo_enable = interrupt_status & 0xc0;
    interrupt_status = interrupt_status & 0x0f;

// disable UART1 Interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0);

    switch(interrupt_status) {
        case 0x04:  // UART received data available interrupt or character timeout interrupt
        case 0x0c: {
            wptr = uart_dbg_wptr;
            if (fifo_enable != 0) {
                data_len = UART_READ(UART_RFL_OFF);

                for(index=0; index<data_len; index++) {
                    uart_dbg_data[wptr] = UART_READ(UART_REV_BUF_OFF);
                    wptr = (wptr + 1) & (REV_FIFO_LEN - 1);
                }

#ifdef _UART_RX_ENABLE_
                Uart.RxFunc(&uart_dbg_data[uart_dbg_wptr], data_len);
#endif
            } else {
                line_status = UART_READ(UART_LINE_STATUS_REG_OFF);
                if((line_status & 0x01) == 1) {
                    uart_dbg_data[wptr] = UART_READ(UART_REV_BUF_OFF);
                    wptr = (wptr + 1) & (REV_FIFO_LEN - 1);

#ifdef ENABLE_LOGGER
#ifdef _UART_RX_ENABLE_
                    uart_rev_len++;

                    if (uart_rev_len == UART_FIFO_MAX_LENGTH) {
                        Uart.RxFunc(&uart_dbg_data[uart_dbg_wptr], UART_FIFO_MAX_LENGTH);
                        uart_rev_len = 0;
                    }
#endif
#endif
                }
            }
            uart_dbg_wptr = wptr;
        }

        break;

        case 0x06: { //Receiver Line Status interrupt
            line_status = UART_READ(UART_LINE_STATUS_REG_OFF);
        }
        break;

        case 0x02:  //Transmit Holding Register empty
            if(Uart.Len != 0 && Uart.LenWritten == Uart.Len) {
                if(uart_hci == 1) {
                    MINT_trans_uart_tx();
                } else {
#ifdef ENABLE_LOGGER
                    MINT_logger_uart_tx();
#endif //ENABLE_LOGGER
                }
            } else {
                S3C2410_task_send_signal(UART_TX_ISR_TO_PF_TASK_SIGNAL, NULL);
                UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x01);
                return;
            }
            break;

        case 0x0: { //Modem status register interrupt
            read_data = UART_READ(UART_MODEM_STATUS_REG_OFF);
        }
        break;

        case 0x07: { //Busy detect indication interrupt
            read_data = UART_READ(UART_STATUS_REG_OFF);
        }
        break;

        default:
            break;
    }

// enable UART Interrupts
    UART_WRITE(UART_INTERRUPT_EN_REG_OFF, 0x05);
    return;
}
#endif
#if 0
VOID
HalLogUartHandle(void)
{
    u32 UartIrqEn = HAL_UART_READ32(UART_INTERRUPT_EN_REG_OFF);
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, 0);
    //2 ToDo: Implement Log Uart Rx
    //2 {

    //2 }
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, UartIrqEn);
}
#endif
HAL_ROM_TEXT_SECTION
_LONG_CALL_ u32
HalLogUartInit(
    IN  LOG_UART_ADAPTER    UartAdapter
)
{
    u32 SetData;
    u32 Divisor;
    u32 Dlh;
    u32 Dll;
    u32 SysClock;
    u32 SampleRate,Remaind;
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

    SysClock = (HalGetCpuClk()>>2);

    SampleRate = (16 * (UartAdapter.BaudRate));

    Divisor= SysClock/SampleRate;

    Remaind = ((SysClock*10)/SampleRate) - (Divisor*10);

    if (Remaind>4) {
        Divisor++;
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

HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalSerialPutcRtl8195a(
    IN  u8 c
)
{
    u32 CounterIndex = 0;

    while(1) {
        CounterIndex++;
        if (CounterIndex >=6540)
            break;

        if (HAL_UART_READ8(UART_LINE_STATUS_REG_OFF) & 0x60)
            break;
    }


#if CONFIG_POST_SIM
    HAL_WRITE8(0x10000000,0x6FFFC,c);
#else
    HAL_UART_WRITE8(UART_TRAN_HOLD_OFF, c);
#endif

    if (c == 0x0a) {
#if CONFIG_POST_SIM
        HAL_WRITE8(0x10000000,0x6FFFC,0x0d);
#else
        HAL_UART_WRITE8(UART_TRAN_HOLD_OFF, 0x0d);
#endif
    }
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_ u8
HalSerialGetcRtl8195a(
    IN  BOOL    PullMode
)
{
    if (PullMode) {
        while(1) {
            if (HAL_UART_READ8(UART_LINE_STATUS_REG_OFF) & BIT0) {
                return HAL_UART_READ8(UART_REV_BUF_OFF);
            }
        }
    } else {
        return HAL_UART_READ8(UART_REV_BUF_OFF);
    }
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_ u32
HalSerialGetIsrEnRegRtl8195a(VOID)
{
    return HAL_UART_READ32(UART_INTERRUPT_EN_REG_OFF);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalSerialSetIrqEnRegRtl8195a (
    IN  u32 SetValue
)
{
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, SetValue);
}
