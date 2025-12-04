/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h"
#include "diag.h"

#include "rtk_v2_uart_base.h"
#include "rtk_v2_hal_uart.h"

#include "uart_v2_baudrate.h"


extern void *
memset( void *s, int c, SIZE_T n);

/**
 * Get RUART runtime status.
 *
 * Get RUART runtime status by setting different debug port.
 *
 *   Debug Ports:
 *      2: rx_dma_single(10), rx_dma_req
 *      3: utxd(13)
 *      5: burstrxdma_req(31)
 *     11: ir_rxd(rxd)(20)
 *
 * @return runtime status value.
 */
u32
HalRTKuartGetDebugValueCommon(
        IN VOID* Data,   ///< RUART Adapter
        IN u32   DbgSel  ///< Debug Port Value
        )
{
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;

#if 0 //not implement yet
	
#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"DbgSel = %d\n"ANSI_COLOR_RESET, DbgSel);
#endif
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
#if UART_DEBUG
    LOGD("[R] UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
#endif

    RegValue &= ~(RUART_SP_REG_DBG_SEL);
#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"RegValue = %X\n"ANSI_COLOR_RESET,RegValue);
#endif
    RegValue |= (DbgSel << 8);
#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"RegValue = %X\n"ANSI_COLOR_RESET,RegValue);
#endif

    HAL_RTKUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);
#if UART_DEBUG
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    LOGD("[R][After] UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
#endif

    DbgSel = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    DbgSel = (DbgSel & RUART_SP_REG_DBG_SEL) >> 8;
#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"DbgSel = %d\n"ANSI_COLOR_RESET, DbgSel);
#endif

    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_DEBUG_REG_OFF);
#if UART_DEBUG
    LOGD(ANSI_COLOR_YELLOW"[R] UART%d DBG_SEL(%d) = %X\n"ANSI_COLOR_RESET,
            UartIndex, DbgSel, RegValue);
#endif
#endif //not implement yet
    return RegValue;
}

/**
 * Get Selected baudrate value index.
 *
 * Get the selected baudrate value's index
 * in pre-defined baudrate array.
 *
 * @return Baudrate Array Index
 */
u32
RTKFindElementIndex(
        u32 Element,  ///< RUART Baudrate
        u32* Array    ///< Pre-defined Baudrate Array
        )
{
    /* DBG_ENTRANCE; */
    u32 BaudRateNumber = 29;
    u32 Result = -1;
    u32 Index = 0;

    for (Index = 0; Index < BaudRateNumber && Result == -1; Index++) {
        if (Element == Array[Index])
            Result = Index;
    }
    return Result;  //TODO: Error handling
}

/**
 * Reset RUART Receiver and Rx FIFO.
 *
 * Reset RUART Receiver and Rx FIFO.
 *
 *   Reset RxFifo Steps:
 *     1. Enable  Reset_rcv
 *     2. Enable  clear_rxfifo
 *     3. Disable Reset_rcv
 *
 *   RUART_FIFO_CTL_REG_OFF Default Values:
 *     1. FIFO_CTL_DEFAULT_WITH_FIFO     (11000001)(C1)
 *     2. FIFO_CTL_DEFAULT_WITH_FIFO_DMA (11001001)(C9)
 *
 * @return VOID.
 */
VOID
RTKuartResetRxFifoCommon(
        IN u8 UartIndex  ///< RUART Index
        )
{
    /* DBG_ENTRANCE; */
    u32 RegValue;

    /* Step 1: Enable Reset_rcv ----->no STS reg here
    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_STS_REG_OFF);
    RegValue |= RUART_STS_REG_RESET_RCV;
    HAL_RTKUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);
	*/
    /* Step 2: Enable clear_rxfifo */
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_FIFO_CTL_REG_OFF, RTKUART_FIFO_CTL_REG_CLEAR_RXFIFO);

    //TODO: Check Defautl Value
     HAL_RTKUART_WRITE32(UartIndex, RTKUART_FIFO_CTL_REG_OFF, RTKFIFO_CTL_DEFAULT_WITH_FIFO); 
    /*HAL_RTKUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, FIFO_CTL_DEFAULT_WITH_FIFO_DMA);*/

    /* Step 3: Disable Reset_rcv -----> no STS reg here
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_RESET_RCV);
    HAL_RTKUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);
    	*/
}

/**
 * Reset RUART Receiver and Rx FIFO wrapper.
 *
 * Reset RUART Receiver and Rx FIFO wrapper function.
 * It will check LINE_STATUS_REG until reset action completion.
 *
 * @return BOOL
 */
BOOL
HalRTKuartResetRxFifoCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u8  RegValue;
    u32 ResetCounter = 0;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;

    while(ResetCounter < 2) {
        RTKuartResetRxFifoCommon(UartIndex);

        RegValue = HAL_RTKUART_READ8(UartIndex, RTKUART_LINE_STATUS_REG_OFF);

        if (RegValue == (RTKUART_LINE_STATUS_REG_THRE | RTKUART_LINE_STATUS_REG_TEMT)) {
#if UART_DEBUG
            LOGD(ANSI_COLOR_GREEN"%d) Reset UART%d RxFIFO! LSR(0x14) = 0x%02X\n"ANSI_COLOR_RESET,
                    ResetCounter + 1, UartIndex, RegValue);
#endif
            return _TRUE;
        }
        else {
#if UART_DEBUG
            LOGD(ANSI_COLOR_MAGENTA"%d) Reset UART%d RxFIFO! LSR(0x14) = 0x%02X\n"ANSI_COLOR_RESET,
                    ResetCounter + 1, UartIndex, RegValue);
#endif
            ++ResetCounter;
        }
    } //while(ResetCounter)
#if UART_DEBUG
    LOGD(ANSI_COLOR_MAGENTA"Reset UART%d RxFIFO FAILED!\n"ANSI_COLOR_RESET, UartIndex);
#endif
    return _FALSE;
}

/**
 * Enable RUART Bus Domain.
 *
 * Enable RUART Bus Domain by RUART index.
 *
 * @return VOID
 */
VOID
RTKuartBusDomainEnableCommon(
        IN u8 UartIndex  ///< RUART Index
        )
{
    u32 RegValue;
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_BD_FUNC0_EN);

    switch (UartIndex)
    {
        case 0:
            RegValue |= BIT0;
            break;
        case 1:
            RegValue |= BIT1;
            break;
        case 2:
            RegValue |= BIT2;
            break;
        default:
            break;
    }

    HAL_WRITE32(PERI_ON_BASE, REG_SOC_PERI_BD_FUNC0_EN, RegValue);
}

/**
 * Initialize RUART.
 *
 * Initialze RUART Setps:
 *   1. Enable Bus Domain
 *   2. Reset Recevier and Rx FIFO
 *   3. Configure Baudrate
 *   4. Configure Interrupts
 *   5. Configure FlowControl
 *   6. Assert RTS (Request to Send)
 *   7. DMA Initialization
 *
 * Interrupt Enable Register (0x04)
 *   7:4  Reserved
 *   3    Enable Modem Status Interrupt (EDSSI)
 *   2    Enable Receiver Line Status Interrupt (ELSI)
 *   1    Enable Transmitter FIFO Empty interrupt (ETBEI)
 *   0    Enable Received Data Available Interrupt (ERBFI)
 *
 * @return runtime status value.
 */
BOOL
HalRTKuartInitCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 RegValue;
    u32 Divisor;
    u32 Dll;
    u32 Dlm;
    u8 UartIndex;
    u8 DmaEnable;
    //
    u32 ElementIndex;
    RUART_SPEED_SETTING RuartSpeedSetting;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;

    /* Enable UART Bus Domain */
    RTKuartBusDomainEnableCommon(UartIndex);

    /* Reset RX FIFO */
    HalRTKuartResetRxFifoCommon(Data);
#if UART_DEBUG
    LOGD(ANSI_COLOR_CYAN"HAL UART Init[UART %d]\n"ANSI_COLOR_RESET, UartIndex);
#endif

    /* Disable all interrupts */
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF, 0x00);

    /* Set DLAB bit to 1 to access DLL/DLM */
    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_LINE_CTL_REG_OFF);
    RegValue |= RTKUART_LINE_CTL_REG_DLAB_ENABLE;
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_LINE_CTL_REG_OFF, RegValue);
#if UART_DEBUG
    LOGD("[R] RTKUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_LINE_CTL_REG_OFF));
#endif

    /* Set Baudrate Division */
    ElementIndex = RTKFindElementIndex(pHalRuartAdapter->BaudRate, RTKBAUDRATE);

    RuartSpeedSetting.BaudRate = RTKBAUDRATE[ElementIndex];
	/* no need
    	RuartSpeedSetting.Ovsr     =     OVSR[ElementIndex];
    	RuartSpeedSetting.Div      =      DIV[ElementIndex];
    	RuartSpeedSetting.Ovsr_adj = OVSR_ADJ[ElementIndex];
    	*/
#if UART_DEBUG
    LOGD("%d\n",
            RuartSpeedSetting.BaudRate
            );
#endif
	/* Set Divisor */	
	Divisor = (SYSTEM_CLK / (16* RuartSpeedSetting.BaudRate))-1; 
	//Divisor = (SYSTEM_CLK / ((ovsr + 5 + ovsr_adj/11) * (UartAdapter.BaudRate))); 

    /* Set Divisor */
    //Divisor = RuartSpeedSetting.Div;

    Dll = (Divisor & 0xFF)>>6;
    Dlm = ((Divisor & 0xFF00) >> 8) >> 6;
	
#if UART_DEBUG
    LOGD("Calculated Dll, Dlm = %02x, %02x\n", Dll, Dlm);
    LOGD("---- Before setting baud rate ----\n");
    LOGD("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_DLL_OFF));
    LOGD("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_DLM_OFF));
#endif
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_DLL_OFF, Dll);
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_DLM_OFF, Dlm);
#if UART_DEBUG
    LOGD("---- After setting baud rate ----\n");
    LOGD("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_DLL_OFF));
    LOGD("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_DLM_OFF));
#endif

#if 0
    LOGD(ANSI_COLOR_CYAN"---- Befor OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    LOGD("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_STS_REG_OFF);
    LOGD("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);
#endif
#if 0 //no need here
    /**
      * Clean Rx break signal interrupt status at initial stage.
      */
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue |= RUART_SP_REG_RXBREAK_INT_STATUS;
    HAL_RTKUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

    /* Set OVSR(xfactor) */
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_XFACTOR);
    RegValue |= (((RuartSpeedSetting.Ovsr - 5) << 4) & RUART_STS_REG_XFACTOR);
    HAL_RTKUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    /* Set OVSR_ADJ[10:0] (xfactor_adj[26:16]) */
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue &= ~(RUART_SP_REG_XFACTOR_ADJ);
    RegValue |= ((RuartSpeedSetting.Ovsr_adj << 16) & RUART_SP_REG_XFACTOR_ADJ);
    HAL_RTKUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

#if UART_DEBUG
    LOGD(ANSI_COLOR_CYAN"---- After OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    LOGD("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
    RegValue = HAL_RTKUART_READ32(UartIndex, RUART_STS_REG_OFF);
    LOGD("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);
#endif
#endif

    /* clear DLAB bit */
    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_LINE_CTL_REG_OFF);
    RegValue &= ~(RTKUART_LINE_CTL_REG_DLAB_ENABLE);
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_LINE_CTL_REG_OFF, RegValue);
#if UART_DEBUG
    LOGD("[R] RUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RTKUART_READ32(UartIndex, RTKUART_LINE_CTL_REG_OFF));
#endif
#if UART_DEBUG
    LOGD("[R] UART%d INT_EN(0x04) = %x\n", UartIndex, pHalRuartAdapter->Interrupts);
#endif
    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF);
    RegValue |= ((pHalRuartAdapter->Interrupts) & 0xFF);
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF, RegValue);
#if UART_DEBUG
    LOGD("[W] UART%d INT_EN(0x04) = %x\n", UartIndex, RegValue);
#endif

    /* Configure FlowControl */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_MODEM_CTL_REG_OFF);
        RegValue |= RTKUART_MCL_AUTOFLOW_ENABLE;
        HAL_RTKUART_WRITE32(UartIndex, RTKUART_MODEM_CTL_REG_OFF, RegValue);
    }

    /* Need to assert RTS during initial stage. */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        HalRTKuartRequestToSendCommon(Data);
    }
	
#if 0 //no DMA
    /* RUART DMA Initialization */
    HalRuartDmaInitRtl8195a(pHalRuartAdapter);
#endif

    return _TRUE;
}


/**
 * RUART send data.
 *
 * RUART send data.
 * Send one byte each time.
 *
 * @return VOID
 */
VOID
HalRTKuartSendCommon(
        IN VOID *Data  ///< RUART Data Structure (RUART_DATA)
        )
{
    /* DBG_ENTRANCE; */
    PRUART_DATA pRuartData = (PRUART_DATA) Data;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartData->pHalRuartAdapter;

    u32 CounterIndex = 0;
    u8  UartIndex    = pHalRuartAdapter->UartIndex;
    u8  BinaryData   = pRuartData->BinaryData;
#if 0
    LOGD("[HAL][Send][RUART%d] Data: %02X, LSR: %02X\n", UartIndex, BinaryData,
            HAL_RTKUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF));
#endif
    while(1) {
        CounterIndex++;

        if (CounterIndex >= 6540)  //TODO: Why 6540?
            break;

        if (HAL_RTKUART_READ8(UartIndex, RTKUART_LINE_STATUS_REG_OFF) &
                (RTKUART_LINE_STATUS_REG_THRE | RTKUART_LINE_STATUS_REG_TEMT)) {
            break;
        }
    }

    HAL_RTKUART_WRITE8(UartIndex, RTKUART_TRAN_HOLD_REG_OFF, BinaryData);
}

/**
 * RUART Receive data.
 *
 * RUART Receive data.
 * Receive one byte each time.
 *
 * @return u8
 */
u8
HalRTKuartRecvCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    u32  RegValue;
    BOOL PullMode  = pHalRuartAdapter->PullMode;
    u8   UartIndex = pHalRuartAdapter->UartIndex;
    u8   ReturnData;

    if (PullMode) {
        while(1) {
            RegValue = (HAL_RTKUART_READ8(UartIndex, RTKUART_LINE_STATUS_REG_OFF));

            if (RegValue & RTKUART_LINE_STATUS_REG_DR) {
                /* LOGD("UART%d LSR(0x14):%X, Data Ready(0x01)\n", UartIndex, RegValue); */
                ReturnData = HAL_RTKUART_READ8(UartIndex, RTKUART_REV_BUF_REG_OFF);
                break;
            }
        }
    }
    else {
        ReturnData = HAL_RTKUART_READ8(UartIndex, RTKUART_REV_BUF_REG_OFF);
    }
#if 0
    LOGD("UART%d RD-RBR: %02X\n", UartIndex, ReturnData);
#endif
    return ReturnData;
}

/**
 * Get RUART Interrupt Value.
 *
 * Get RUART Interrupt Value.
 *
 * @return u32
 */
u32
HalRTKuartGetInterruptEnRegCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF);
#if UART_DEBUG
    LOGD("UART%d INT_EN(0x04): %X\n", UartIndex, RegValue);
#endif
    return RegValue;
}

/**
 * Set RUART Interrupt Value.
 *
 * Set RUART Interrupt Value.
 *
 * @return VOID
 */
VOID
HalRTKuartSetInterruptEnRegCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    u32 InterruptSetting;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    InterruptSetting = pHalRuartAdapter->Interrupts;

#if UART_DEBUG
    LOGD("[B] UART%d INT_EN(%02X) = %X\n", UartIndex, RTKUART_INTERRUPT_EN_REG_OFF,
            HAL_RTKUART_READ32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF));
#endif
    RegValue = 0x00;
    RegValue |= InterruptSetting;
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF, RegValue);
#if UART_DEBUG
    LOGD("[A] UART%d INT_EN(%02X) = %X\n", UartIndex, RTKUART_INTERRUPT_EN_REG_OFF,
            HAL_RTKUART_READ32(UartIndex, RTKUART_INTERRUPT_EN_REG_OFF));
#endif
}

#if 0 //no DMA here
/**
 * Initialize RUART DMA.
 *
 * Initialize RUART DMA.
 * RUART use GDMA IP.
 *
 *   Initialize Steps:
 *     1. Set DMA burst size
 *     2. Enable/Disable RURAT DMA
 *
 * @return VOID
 */
VOID
HalRTKuartDmaInitCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    u8  DmaEnable;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    DmaEnable = (pHalRuartAdapter->DmaEnable) & 0xFF;

    /* Set suggested Tx/Rx DMA burst size */
    SetUartDmaBurstSize(UartIndex);

    /* Set DMA Enable/Disable */
    RegValue = (FIFO_CTL_DEFAULT_WITH_FIFO_DMA &
            (~RUART_FIFO_CTL_REG_DMA_ENABLE)) | DmaEnable;
#if UART_DEBUG
    LOGD("FIFO Control RegValue: %X\n", RegValue);
#endif
    HAL_RTKUART_WRITE8(UartIndex, RUART_FIFO_CTL_REG_OFF, RegValue);
}
#endif //no DMA here

/**
 * Assert RUART RTS Signal.
 *
 * Assert RUART RTS Signal.
 *
 * @return VOID
 */
VOID
HalRTKuartRequestToSendCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RTKUART_READ32(UartIndex, RTKUART_MODEM_CTL_REG_OFF);
    RegValue |= RTKUART_MCR_RTS;
    HAL_RTKUART_WRITE32(UartIndex, RTKUART_MODEM_CTL_REG_OFF, RegValue);
}
