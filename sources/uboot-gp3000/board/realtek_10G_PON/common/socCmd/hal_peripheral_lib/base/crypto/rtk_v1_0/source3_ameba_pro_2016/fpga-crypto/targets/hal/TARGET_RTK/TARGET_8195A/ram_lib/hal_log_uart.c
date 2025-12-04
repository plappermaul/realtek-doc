/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"

extern HAL_Status RuartIsTimeout (u32 StartCount, u32 TimeoutCnt);
extern VOID UartLogIrqHandleRam(VOID * Data);
extern HAL_TIMER_OP HalTimerOp;

static VOID HalLogUartIrqLineStatusHandle(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u8 line_status;

    line_status = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
    pUartAdapter->LineStatus = line_status;
    if (pUartAdapter->LineStatusCallback != NULL) {
        pUartAdapter->LineStatusCallback (pUartAdapter->LineStatusCbPara, line_status);
    }
}

static VOID HalLogUartIrqRxRdyHandle(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u8 *pRxBuf = (u8*)pUartAdapter->pRxBuf;
    volatile u8 line_status;

    if (pRxBuf) {
        while (pUartAdapter->RxCount > 0) {
            line_status = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
            if (line_status & LSR_DR) {
                *(pRxBuf) = HAL_UART_READ8(UART_REV_BUF_OFF);
                pRxBuf++;
                pUartAdapter->RxCount--;
            } else {
                // no more rx data
                // Adjust the RX FIFO trigger level: 1 char
                if (pUartAdapter->RxCount < (LOG_UART_RX_FIFO_DEPTH/2)) {
                    pUartAdapter->FIFOControl &= ~FCR_RX_TRIG_MASK;
                    pUartAdapter->FIFOControl |= FCR_RX_TRIG_1CH;
                    HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl);
                }
                break;  // break the while loop
            }
        }

        if (pUartAdapter->RxCount == 0) {
            // Disable RX Interrupt
            pUartAdapter->IntEnReg &= ~(IER_ERBFI|IER_ELSI);
            HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
            if (pUartAdapter->pRxBuf != pRxBuf) {
                // at least receive 1 byte and got wanted data length
                if (pUartAdapter->RxCompCallback != NULL) {
                    pUartAdapter->RxCompCallback (pUartAdapter->RxCompCbPara);
                }
            }

            // Adjust the RX FIFO trigger level: 1 char
            if ((pUartAdapter->FIFOControl & FCR_RX_TRIG_MASK) != FCR_RX_TRIG_1CH) {
                pUartAdapter->FIFOControl &= ~FCR_RX_TRIG_MASK;
                pUartAdapter->FIFOControl |= FCR_RX_TRIG_1CH;
                HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl);
            }
        }
        pUartAdapter->pRxBuf = pRxBuf;

    } else {
        DBG_UART_WARN("HalLogUartIrqRxDataHandle: No RX Buffer\n");
    }
}

static VOID HalLogUartIrqTxEmptyHandle(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u8 *pTxBuf = (u8*)pUartAdapter->pTxBuf;
    volatile u8 line_status;

    if (NULL != pTxBuf) {
        while (pUartAdapter->TxCount > 0) {
            line_status = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
//            if (line_status & LSR_THRE) {
            if ((line_status & LSR_FIFOF)==0) {
                // FIFO not full yet
                HAL_UART_WRITE8(UART_TRAN_HOLD_OFF, *(pTxBuf));
                pTxBuf++;
                pUartAdapter->TxCount--;
            }
        }
        pUartAdapter->pTxBuf = pTxBuf;

        if (pUartAdapter->TxCount == 0) {
            // Disable THRE mode
            pUartAdapter->IntEnReg &= ~(IER_PTIME);
            HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
            line_status = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);

            if (line_status & LSR_THRE) {
                // FIFO is empty now
                // Disable TX FIFO empty Interrupt
                pUartAdapter->IntEnReg &= ~(IER_ETBEI);
                HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

                // Call user TX complete callback
                if (NULL != pUartAdapter->TxCompCallback) {
                    pUartAdapter->TxCompCallback(pUartAdapter->TxCompCbPara);
                }
            }
        }
    }    else {
        pUartAdapter->IntEnReg &= ~(IER_ETBEI);
        HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
    }
}

VOID HalLogUartIrqHandle(VOID * Data)
{
    HAL_LOG_UART_ADAPTER *pUartAdapter=Data;
    volatile u32 iir;
    u32 i;

    for (i=0; i<7; i++) {
        iir = HAL_UART_READ32(UART_INTERRUPT_IDEN_REG_OFF) & 0x0F;
        switch (iir) {
            case IIR_RX_LINE_STATUS:
                // Overrun/parity/framing errors or break interrupt
                HalLogUartIrqLineStatusHandle (pUartAdapter);
                break;

            case IIR_RX_RDY:
                // Receiver data available
                HalLogUartIrqRxRdyHandle (pUartAdapter);
                break;

            case IIR_CHAR_TIMEOUT:
                // No characters in or out of the RCVR FIFO during the
                // last 4 character times and there is at least 1
                // character in it during this time
                // TODO:
                HalLogUartIrqRxRdyHandle (pUartAdapter);
                break;

            case IIR_THR_EMPTY:
                // Transmitter holding register empty
                HalLogUartIrqTxEmptyHandle(pUartAdapter);
                break;

            case IIR_MODEM_STATUS:
                // Clear to send or data set ready or
                // ring indicator or data carrier detect.
                // TODO:
                break;

            case IIR_BUSY:
                // master has tried to write to the Line
                // Control Register while the DW_apb_uart is busy
                // TODO:
                break;

            case IIR_NO_PENDING:
                return;

            default:
                DBG_UART_WARN("HalLogUartIrqHandle: UnKnown Interrupt ID\n");
                break;
        }

        // Call User API Interrupt Handler
        if (pUartAdapter->api_irq_handler != NULL) {
            (pUartAdapter->api_irq_handler)(pUartAdapter->api_irq_id, iir);
        }
    }
}

VOID HalLogUartSetBaudRate(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u32 CtlReg;
    u32 Divisor;
    u32 Dlh;
    u32 Dll;
    u32 SysClock;
    u32 SampleRate,Remaind;

#if CONFIG_CHIP_A_CUT
    SysClock = (StartupHalGetCpuClk()>>2);
#else
    SysClock = (HalGetCpuClk()>>2);
#endif
    SampleRate = (16 * (pUartAdapter->BaudRate));

    if (SampleRate > SysClock) {
        DBG_UART_ERR("Cannot support Baud Sample Rate which bigger than Serial Clk\n");
        return;
    }
    Divisor= SysClock/SampleRate;

    Remaind = ((SysClock*10)/SampleRate) - (Divisor*10);

    if (Remaind>4) {
        Divisor++;
    }

    Dll = Divisor & 0xff;
    Dlh = (Divisor & 0xff00)>>8;
    // set DLAB bit to 1
    CtlReg = HAL_UART_READ32(UART_LINE_CTL_REG_OFF);
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, (CtlReg|0x80));

    // set up buad rate division
    HAL_UART_WRITE32(UART_DLL_OFF, Dll);
    HAL_UART_WRITE32(UART_DLH_OFF, Dlh);

    // clear DLAB bit
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, (CtlReg&0x7F));
}

VOID HalLogUartSetLineCtrl(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u32 SetData;

    SetData = pUartAdapter->Parity | pUartAdapter->Stop | pUartAdapter->DataLength;
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, SetData);
}

VOID HalLogUartSetIntEn(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
}

u32 HalLogUartInitSetting(HAL_LOG_UART_ADAPTER *pUartAdapter)
{
    u32 SetData;
    u32 Divisor;
    u32 Dlh;
    u32 Dll;
    u32 SysClock;
    u32 SampleRate,Remaind;
    PIRQ_HANDLE pIrqHandle;

    LOC_UART_FCTRL(OFF);
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);

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

#if CONFIG_CHIP_A_CUT
    SysClock = (StartupHalGetCpuClk()>>2);
#else
    SysClock = (HalGetCpuClk()>>2);
#endif
    SampleRate = (16 * (pUartAdapter->BaudRate));

    Divisor= SysClock/SampleRate;

    Remaind = ((SysClock*10)/SampleRate) - (Divisor*10);

    if (Remaind>4) {
        Divisor++;
    }

    Dll = Divisor & 0xff;
    Dlh = (Divisor & 0xff00)>>8;
    HAL_UART_WRITE32(UART_DLL_OFF, Dll);
    HAL_UART_WRITE32(UART_DLH_OFF, Dlh);

    // clear DLAB bit
    HAL_UART_WRITE32(UART_LINE_CTL_REG_OFF, 0);

    // set data format
    SetData = pUartAdapter->Parity | pUartAdapter->Stop | pUartAdapter->DataLength;
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
    HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl);

    /*
        Interrupt Enable Register
        7: THRE Interrupt Mode enable
        2: Enable Receiver Line status Interrupt
        1: Enable Transmit Holding register empty INT32
        0: Enable received data available interrupt
        */
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

    // Register IRQ
    pIrqHandle = &pUartAdapter->IrqHandle;

    pIrqHandle->Data = (u32)pUartAdapter;
    pIrqHandle->IrqNum = UART_LOG_IRQ;
    pIrqHandle->IrqFun = (IRQ_FUN) HalLogUartIrqHandle;
    pIrqHandle->Priority = 14;

    // Re-Register IRQ
    InterruptUnRegister(pIrqHandle); // un_register old IRQ first
    InterruptRegister(pIrqHandle);
    InterruptEn(pIrqHandle);

    return 0;
}

u32
HalLogUartRecv(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter,
    IN u8  *pRxData,
    IN u32 Length,
    IN u32 TimeoutMS      // Timeout, in ms
)
{
    volatile u8 LineStatus;
    u32 rx_bytes=0;
    u32 TimeoutCount=0, StartCount;

    if ((pRxData == NULL) || (Length == 0)) {
        DBG_UART_ERR("HalLogUartRecv: Err: pRxData=0x%x,  Length=%d\n", pRxData, Length);
        return 0;
    }

    if ((TimeoutMS != 0) && (TimeoutMS != LOG_UART_WAIT_FOREVER)) {
        TimeoutCount = (TimeoutMS*1000/TIMER_TICK_US);
        StartCount = HalTimerOp.HalTimerReadCount(1);
    }

    while (rx_bytes < Length) {
        LineStatus = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);

        if (LineStatus & LSR_DR) {
            *(pRxData+rx_bytes) = HAL_UART_READ8(UART_REV_BUF_OFF);
            rx_bytes++;
        } else {
            // check timeout
            if (TimeoutCount > 0) {
                if (HAL_TIMEOUT == RuartIsTimeout(StartCount, TimeoutCount)) {
                    DBG_UART_INFO("HalLogUartRecv: Rx Timeout, RxCount=%d\n", rx_bytes);
                    break;
                }
            } else {
                if (TimeoutMS == 0) {
                    break;
                }
            }
        }
    }

    pUartAdapter->LineStatus = LineStatus;

    return rx_bytes;
}

u32
HalLogUartSend(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter,
    IN u8 *pTxData,
    IN u32 Length,
    IN u32 TimeoutMS
)
{
    u32 TimeoutCount=0;
    u32 StartCount;
    u32 tx_bytes=0;
    volatile u8 LineStatus;

    if ((pTxData == NULL) || (Length == 0)) {
        DBG_UART_ERR("HalLogUartSend: Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
        return 0;
    }

    if ((TimeoutMS != 0) && (TimeoutMS != LOG_UART_WAIT_FOREVER)) {
        TimeoutCount = (TimeoutMS*1000/TIMER_TICK_US);
        StartCount = HalTimerOp.HalTimerReadCount(1);
    }

    while (tx_bytes < Length) {
        LineStatus = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
        if (LineStatus & LSR_THRE) {
            HAL_UART_WRITE32(UART_TRAN_HOLD_OFF, (*(pTxData+tx_bytes)));
            tx_bytes++;
        } else {
            // check timeout
            if (TimeoutCount > 0) {
                if (HAL_TIMEOUT == RuartIsTimeout(StartCount, TimeoutCount)) {
                    DBG_UART_INFO("HalLogUartSend: Tx Timeout, TxCount=%d\n", tx_bytes);
                    break;
                }
            } else {
                if (TimeoutMS == 0) {
                    // No wait
                    break;
                }
            }
        }
    }

    if (tx_bytes == Length) {
        // wait all data in TX FIFO be transmitted
        while (1) {
            LineStatus = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
            if (LineStatus & LSR_TEMT) {
                break;
            } else {
                if (TimeoutCount > 0) {
                    if (HAL_TIMEOUT == RuartIsTimeout(StartCount, TimeoutCount)) {
                        break;
                    }
                }
            }
        }
    }

    return tx_bytes;
}

HAL_Status
HalLogUartIntSend(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter,
    IN u8 *pTxData,
    IN u32 Length
)
{
//    volatile u8 LineStatus;

    if ((pTxData == NULL) || (Length == 0)) {
        DBG_UART_ERR("HalLogUartIntSend: Err: pTxData=0x%x, Length=%d\n", pTxData, Length);
        return HAL_ERR_PARA;
    }

    pUartAdapter->pTxBuf = pTxData;
    pUartAdapter->pTxStartAddr = pTxData;
    pUartAdapter->TxCount = Length;
#if 0
    while (pUartAdapter->TxCount > 0) {
        LineStatus = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
        if (LineStatus & LSR_THRE) {
            HAL_UART_WRITE32(UART_TRAN_HOLD_OFF, (*(pUartAdapter->pTxBuf)));
            pUartAdapter->TxCount--;
            pUartAdapter->pTxBuf++;
        } else {
            break;
        }
    }
#endif
    // Enable Tx FIFO empty interrupt
    pUartAdapter->IntEnReg |= (IER_PTIME | IER_ETBEI);
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

    return HAL_OK;
}

HAL_Status
HalLogUartIntRecv(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter,
    IN u8  *pRxData,
    IN u32 Length
)
{
    if ((pRxData == NULL) || (Length == 0)) {
        DBG_UART_ERR("HalLogUartIntRecv: Err: pRxData=0x%x, Length=%d\n", pRxData, Length);
        return HAL_ERR_PARA;
    }

    pUartAdapter->pRxBuf = pRxData;
    pUartAdapter->pRxStartAddr = pRxData;
    pUartAdapter->RxCount = Length;

    if (Length > (LOG_UART_RX_FIFO_DEPTH/2)) {
        pUartAdapter->FIFOControl |= FCR_RX_TRIG_HF;
        HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, pUartAdapter->FIFOControl);
    }

    // Eable Rx data ready & Line Status interrupt
    pUartAdapter->IntEnReg |= (IER_ELSI | IER_ERBFI);
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

    return HAL_OK;
}

VOID
HalLogUartAbortIntSend(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter
)
{
    // Disable Tx FIFO empty interrupt
    pUartAdapter->IntEnReg &= ~(IER_PTIME | IER_ETBEI);
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);
}

VOID
HalLogUartAbortIntRecv(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter
)
{
    volatile u8 LineStatus;

    // Disable Rx data ready & Line Status interrupt
    pUartAdapter->IntEnReg &= ~(IER_ELSI | IER_ERBFI);
    HAL_UART_WRITE32(UART_INTERRUPT_EN_REG_OFF, pUartAdapter->IntEnReg);

    while (pUartAdapter->RxCount > 0) {
        LineStatus = HAL_UART_READ8(UART_LINE_STATUS_REG_OFF);
        if (LineStatus & LSR_DR) {
            *(pUartAdapter->pRxBuf) = HAL_UART_READ8(UART_REV_BUF_OFF);
            pUartAdapter->pRxBuf++;
            pUartAdapter->RxCount--;
        } else {
            break;  // no more rx data, break the while loop
        }
    }
}

// Reset both TX and RX FIFO
HAL_Status
HalLogUartRstFIFO(HAL_LOG_UART_ADAPTER *pUartAdapter, u8 RstCtrl)
{
    volatile u32 RegValue;
    volatile u8 line_status;
    u32 timeout;

    /* Step 1: Enable clear_txfifo & clear_rxfifo */
    RegValue = pUartAdapter->FIFOControl;
    if (RstCtrl & LOG_UART_RST_TX_FIFO) {
        RegValue |= FCR_RST_TX;
    }

    if (RstCtrl & LOG_UART_RST_RX_FIFO) {
        RegValue |= FCR_RST_RX;
    }

    HAL_UART_WRITE32(UART_FIFO_CTL_REG_OFF, RegValue);

    // Wait THR & TSR empty
    if (RstCtrl & LOG_UART_RST_TX_FIFO) {
        RegValue = HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & 0xFF;
        timeout = 100;  // wait 10 ms
        while (((RegValue & LSR_TEMT)==0) && (timeout > 0)) {
            HalDelayUs(100);
            RegValue = HAL_UART_READ32(UART_LINE_STATUS_REG_OFF) & 0xFF;
            timeout--;
        }
    }
    return HAL_OK;
}

VOID
HalLogUartEnable(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter
)
{
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);

    PinCtrl(LOG_UART,S0,ON);
}

VOID
HalLogUartDisable(
    IN HAL_LOG_UART_ADAPTER *pUartAdapter
)
{
    LOC_UART_FCTRL(OFF);
    ACTCK_LOG_UART_CCTRL(OFF);

    PinCtrl(LOG_UART,S0,OFF);
}

void HalInitLogUart(void)
{
    IRQ_HANDLE          UartIrqHandle;
    LOG_UART_ADAPTER    UartAdapter;

    //4 Release log uart reset and clock
    LOC_UART_FCTRL(OFF);
    LOC_UART_FCTRL(ON);
    ACTCK_LOG_UART_CCTRL(ON);
    PinCtrl(LOG_UART, S0, ON);

    //4 Inital Log uart
    UartAdapter.BaudRate = UART_BAUD_RATE_38400;
    UartAdapter.DataLength = UART_DATA_LEN_8BIT;
    UartAdapter.FIFOControl = 0xC1;
    UartAdapter.IntEnReg = 0x05;///0x00;
    UartAdapter.Parity = UART_PARITY_DISABLE;
    UartAdapter.Stop = UART_STOP_1BIT;

    HalLogUartInit(UartAdapter);

    //4 Register Log Uart Callback function
    UartIrqHandle.Data = (u32)NULL;//(u32)&UartAdapter;
    UartIrqHandle.IrqNum = UART_LOG_IRQ;
    UartIrqHandle.IrqFun = (IRQ_FUN) UartLogIrqHandleRam;
    UartIrqHandle.Priority = 0;

    InterruptUnRegister(&UartIrqHandle);
    InterruptRegister(&UartIrqHandle);
}

void HalDeinitLogUart(void)
{
    while(1) {
        if (HAL_READ8(LOG_UART_REG_BASE,0x14)&BIT6) {
            break;
        }
        HalDelayUs(20);
    }

    PinCtrl(LOG_UART, S0, OFF);
}

