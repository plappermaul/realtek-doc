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

#include "rtk_uart_base.h"
//#include "rtk_hal_uart.h"
#include "peripheral.h"


#include "uart_baudrate.h"

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
HalRuartGetDebugValueRTKCommon(
        IN VOID* Data,   ///< RUART Adapter
        IN u32   DbgSel  ///< Debug Port Value
        )
{
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;

#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"DbgSel = %d\n"ANSI_COLOR_RESET, DbgSel);
#endif
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
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

    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);
#if UART_DEBUG
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    LOGD("[R][After] UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
#endif

    DbgSel = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    DbgSel = (DbgSel & RUART_SP_REG_DBG_SEL) >> 8;
#if UART_DEBUG
    LOGD(ANSI_COLOR_RED"DbgSel = %d\n"ANSI_COLOR_RESET, DbgSel);
#endif

    RegValue = HAL_RUART_READ32(UartIndex, RUART_DEBUG_REG_OFF);
#if UART_DEBUG
    LOGD(ANSI_COLOR_YELLOW"[R] UART%d DBG_SEL(%d) = %X\n"ANSI_COLOR_RESET,
            UartIndex, DbgSel, RegValue);
#endif
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
FindElementIndex(
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
RuartResetRxFifoRTKCommon(
        IN u8 UartIndex  ///< RUART Index
        )
{
    /* DBG_ENTRANCE; */
    u32 RegValue;

    /* Step 1: Enable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue |= RUART_STS_REG_RESET_RCV;
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    /* Step 2: Enable clear_rxfifo */ //not always nessesary. in 96F FPGA, sometimes cause sys hang, so marked it.
    //HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, RUART_FIFO_CTL_REG_CLEAR_RXFIFO);

    //TODO: Check Defautl Value
    /* HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, FIFO_CTL_DEFAULT_WITH_FIFO); */
    //HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, FIFO_CTL_DEFAULT_WITH_FIFO_DMA); //already set by init.

    /* Step 3: Disable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_RESET_RCV);
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

}

/**
 * Reset RUART Receiver and Rx FIFO wrapper.
 *
 * Reset RUART Receiver and Rx FIFO wrapper function.
 * It will check LINE_STATUS_REG until reset action completion.
 *
 * @return BOOL
 */
#if 1 //always reset RX. 
BOOL
HalRuartResetRxFifoRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u8  RegValue;
    u32 ResetCounter = 0;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;

    while(ResetCounter < 2) {
		
        RuartResetRxFifoRTKCommon(UartIndex);

        RegValue = HAL_RUART_READ8(UartIndex, RUART_LINE_STATUS_REG_OFF);

        if (RegValue == (RUART_LINE_STATUS_REG_THRE | RUART_LINE_STATUS_REG_TEMT)) {
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
#else //only if RX is not empty, then reset RX.
BOOL
HalRuartResetRxFifoRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u8  RegValue;
    u32 ResetCounter = 0;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;

    while(ResetCounter < 2) {

        RegValue = HAL_RUART_READ8(UartIndex, RUART_LINE_STATUS_REG_OFF);

        if (RegValue == (RUART_LINE_STATUS_REG_THRE | RUART_LINE_STATUS_REG_TEMT)) {
#if UART_DEBUG
            LOGD(ANSI_COLOR_GREEN"%d) Reset UART%d RxFIFO! LSR(0x14) = 0x%02X\n"ANSI_COLOR_RESET,
                    ResetCounter + 1, UartIndex, RegValue);
#endif

            return _TRUE;
        }
        else {
			RuartResetRxFifoRTKCommon(UartIndex);
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

#endif
/**
 * Enable RUART Bus Domain.
 *
 * Enable RUART Bus Domain by RUART index.
 *
 * @return VOID
 */
VOID
RuartBusDomainEnableRTKCommon(
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
HalRuartInitRTKCommon(
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
    u8 G2Uart;
    //
    u32 ElementIndex;
    RUART_SPEED_SETTING RuartSpeedSetting;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    G2Uart = pHalRuartAdapter->G2Uart;

    /* Enable UART Bus Domain */
    //RuartBusDomainEnableRTKCommon(UartIndex);

    if (G2Uart != 1) {
        /* Reset RX FIFO */
        HalRuartResetRxFifoRTKCommon(Data);
#if UART_DEBUG
        LOGD(ANSI_COLOR_CYAN"HAL UART Init[UART %d]\n"ANSI_COLOR_RESET, UartIndex);
#endif

        /* Disable all interrupts */
        HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, 0x00);

        /* Set DLAB bit to 1 to access DLL/DLM */
        RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
        RegValue |= RUART_LINE_CTL_REG_DLAB_ENABLE;
        HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
#if UART_DEBUG
        LOGD("[R] RUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));
#endif

        /* Set Baudrate Division */

        ElementIndex = FindElementIndex(pHalRuartAdapter->BaudRate, BAUDRATE);

        RuartSpeedSetting.BaudRate = BAUDRATE[ElementIndex];
#if 1	
        RuartSpeedSetting.Ovsr     =     OVSR[ElementIndex];
        RuartSpeedSetting.Div      =      DIV[ElementIndex];
        RuartSpeedSetting.Ovsr_adj = OVSR_ADJ[ElementIndex];
#if UART_DEBUG
        LOGD("%d, %d, %d, %X\n",
            RuartSpeedSetting.BaudRate,
            RuartSpeedSetting.Ovsr,
            RuartSpeedSetting.Div,
            RuartSpeedSetting.Ovsr_adj
            );
#endif
#endif

        /* Divisor = (SYSTEM_CLK / ((ovsr + 5 + ovsr_adj/11) * (UartAdapter.BaudRate))); */

        /* Set Divisor */
#if 1
        Divisor = RuartSpeedSetting.Div;
        Dll = Divisor & 0xFF;
        Dlm = (Divisor & 0xFF00) >> 8;
#else	
        Divisor = SYSTEM_CLK/16/RuartSpeedSetting.BaudRate;
        Dll = Divisor & 0xff;
        Dlm = (Divisor & 0xff00) >> 8;
#endif
	
#if UART_DEBUG
        LOGD("Calculated Dll, Dlm = %02x, %02x\n", Dll, Dlm);
        LOGD("---- Before setting baud rate ----\n");
        LOGD("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLL_OFF));
        LOGD("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLM_OFF));
#endif
        HAL_RUART_WRITE32(UartIndex, RUART_DLL_OFF, Dll);
        HAL_RUART_WRITE32(UartIndex, RUART_DLM_OFF, Dlm);
#if UART_DEBUG
        LOGD("---- After setting baud rate ----\n");
        LOGD("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLL_OFF));
        LOGD("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLM_OFF));
#endif

#if UART_DEBUG
        LOGD(ANSI_COLOR_CYAN"---- Befor OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
        LOGD("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
        LOGD("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);
#endif
        /**
          * Clean Rx break signal interrupt status at initial stage.
          */
        RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
        RegValue |= RUART_SP_REG_RXBREAK_INT_STATUS;
        HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);
#if 1 //test formal uart baudrate, no need of these setting
        /* Set OVSR(xfactor) */
        RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
        RegValue &= ~(RUART_STS_REG_XFACTOR);
        RegValue |= (((RuartSpeedSetting.Ovsr - 5) << 4) & RUART_STS_REG_XFACTOR);
        HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

        /* Set OVSR_ADJ[10:0] (xfactor_adj[26:16]) */
        RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
        RegValue &= ~(RUART_SP_REG_XFACTOR_ADJ);
        RegValue |= ((RuartSpeedSetting.Ovsr_adj << 16) & RUART_SP_REG_XFACTOR_ADJ);
        HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

#if UART_DEBUG
        LOGD(ANSI_COLOR_CYAN"---- After OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
        LOGD("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
        LOGD("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);
#endif
#endif
        /* clear DLAB bit */
        RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
        RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
        HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
#if UART_DEBUG
        LOGD("[R] RUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));
#endif
#if UART_DEBUG
        LOGD("[R] UART%d INT_EN(0x04) = %x\n", UartIndex, pHalRuartAdapter->Interrupts);
#endif
        RegValue = HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF);
        RegValue |= ((pHalRuartAdapter->Interrupts) & 0xFF);
        HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, RegValue);
#if UART_DEBUG
        LOGD("[W] UART%d INT_EN(0x04) = %x\n", UartIndex, RegValue);
#endif

        /* Configure FlowControl */
        if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
            //RegValue |= RUART_MCL_AUTOFLOW_ENABLE;
            RegValue |= (RUART_MCL_AUTOFLOW_ENABLE | RUART_MCR_RTS);
            HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
        }
        else
        {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
            //RegValue &= ~RUART_MCL_AUTOFLOW_ENABLE;
            RegValue &= ~(RUART_MCL_AUTOFLOW_ENABLE | RUART_MCR_RTS);
            HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
	}

        /* LoopBack */
        if (pHalRuartAdapter->LoopBackMode == 1) {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
            RegValue |= RUART_MCR_LOOPBACK;
            HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
        }
        else
        {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
            RegValue &= ~RUART_MCR_LOOPBACK;
            HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
        }

        /* LoopBack (reverse) */
        if (pHalRuartAdapter->LoopBackMode_2 == 1) {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
            RegValue |= RUART_SP_REG_PIN_LB_TEST;
            HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);
        }
        else
        {
            RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
            RegValue &= ~RUART_SP_REG_PIN_LB_TEST;
            HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);
        }

        /* Need to assert RTS during initial stage. */
        if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
            HalRuartRequestToSendRTKCommon(Data);
        }
        else
            printf("no auto flow\n");

        /* RUART DMA Initialization */
        HalRuartDmaInitRTKCommon(pHalRuartAdapter);
    } else {
        /* Legacy UART init */
        /* Disable all interrupts */
        HAL_G2UART_WRITE32(UartIndex, G2UART_IE_REG_OFF, 0x00);

        ElementIndex = FindElementIndex(pHalRuartAdapter->BaudRate, BAUDRATE);

        RuartSpeedSetting.BaudRate = BAUDRATE[ElementIndex];
        RuartSpeedSetting.BaudCount = BAUDCOUNT[ElementIndex];

	/* Settings of the UART*/
        RegValue = HAL_G2UART_READ32(UartIndex, G2UART_CFG_REG_OFF);
        RegValue |= (G2UART_CFG_REG_CHAR_CNT | G2UART_CFG_REG_TX_SM_ENABLE | G2UART_CFG_REG_RX_SM_ENABLE | G2UART_CFG_REG_UART_EN);
        /* Set Baud Count */
        RegValue |= (((RuartSpeedSetting.BaudCount + 1) << 8) & G2UART_CFG_REG_BAUD_COUNT);
	HAL_G2UART_WRITE32(UartIndex, G2UART_CFG_REG_OFF, RegValue);

        /* Set the configuration for RX/TX flow control of UART */
        RegValue = HAL_G2UART_READ32(UartIndex, G2UART_FC_REG_OFF);
        RegValue |= (G2UART_FC_REG_RX_WM | G2UART_FC_REG_NO_CTS | G2UART_FC_REG_CTS_REG | G2UART_FC_REG_NO_RTS);
        HAL_G2UART_WRITE32(UartIndex, G2UART_FC_REG_OFF, RegValue);

        /* Set the sample point for the receive UART */
        RegValue = G2UART_RX_SAMPLE_REG_RX_CENTER;
        HAL_G2UART_WRITE32(UartIndex, G2UART_RX_SAMPLE_REG_OFF, RegValue);
    }

    return _TRUE;
}

/**
 * Initialize RUART DMA.
 *
 * Initialize RUART DMA.
 * RUART use GDMA IP.
 *
 *   RUART DMA Burst Size Table:
 *     Peripheral DMA FIFO threshold                    SRC_MSIZE       DEST_MSIZE      SRC_TR_WIDTH    DST_TR_WIDTH
 *     UART TX txdma_burstsize (MISCR[7:3])  = 7(00111) 000(1 item)     001(4 items)    010(32 bits)    000(8 bits)
 *     UART RX rxdma_burstsize (MISCR[12:8]) = 4(00100) 001(4 items)    000(1 item)     000(8 bits)     010(32 bits)
 *
 * @return VOID
 */
VOID
SetUartDmaBurstSize(
        IN u8 UartIndex  ///< RUART Index
        )
{
    u32 RegValue;
    u8  TxBurstSize = 8;
    u8  RxBurstSize = 4;

    RegValue = HAL_RUART_READ32(UartIndex, RUART_MISC_CTL_REG_OFF);
    RegValue &= ~(RUART_TXDMA_BURSTSIZE_MASK);
    RegValue |= (TxBurstSize<<3);
    HAL_RUART_WRITE32(UartIndex, RUART_MISC_CTL_REG_OFF, RegValue);

    RegValue = HAL_RUART_READ32(UartIndex, RUART_MISC_CTL_REG_OFF);
    RegValue &= ~(RUART_RXDMA_BURSTSIZE_MASK);
    RegValue |= (RxBurstSize<<8);

    /* Tx */
    /* RegValue &= ~(1<<1); */
    /* Rx */
    /* RegValue &= ~(1<<2); */

    HAL_RUART_WRITE32(UartIndex, RUART_MISC_CTL_REG_OFF, RegValue);
#if UART_DEBUG
    RegValue = HAL_RUART_READ32(UartIndex, RUART_MISC_CTL_REG_OFF);
    LOGD("UART%d RUART_MISC_CTL_REG_OFF: %X\n", UartIndex, RegValue);
#endif
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
HalRuartSendRTKCommon(
        IN VOID *Data  ///< RUART Data Structure (RUART_DATA)
        )
{
    /* DBG_ENTRANCE; */
    PRUART_DATA pRuartData = (PRUART_DATA) Data;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartData->pHalRuartAdapter;

    u32 CounterIndex = 0;
    u8  UartIndex    = pHalRuartAdapter->UartIndex;
    u8  BinaryData   = pRuartData->BinaryData;
    u8  G2Uart       = pHalRuartAdapter->G2Uart;
#if 0
    LOGD("[HAL][Send][RUART%d] Data: %02X, LSR: %02X\n", UartIndex, BinaryData,
            HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF));
#endif
    while(1) {
        CounterIndex++;

        if (CounterIndex >= 6540)  //TODO: Why 6540?
            break;

        if (G2Uart != 1) {
            if (HAL_RUART_READ8(UartIndex, RUART_LINE_STATUS_REG_OFF) &
                    (RUART_LINE_STATUS_REG_THRE | RUART_LINE_STATUS_REG_TEMT)) {
                break;
            }
	} else {
            if (HAL_G2UART_READ8(UartIndex, G2UART_INFO_REG_OFF) & G2UART_INFO_REG_TX_FIFO_EMPTY)
                break;
        }
    }

    if (G2Uart != 1)
        HAL_RUART_WRITE8(UartIndex, RUART_TRAN_HOLD_REG_OFF, BinaryData);
    else
        HAL_G2UART_WRITE8(UartIndex, G2UART_TXDAT_REG_OFF, BinaryData);
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
HalRuartRecvRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    u32  RegValue;
    BOOL PullMode  = pHalRuartAdapter->PullMode;
    u8   UartIndex = pHalRuartAdapter->UartIndex;
    u8   G2Uart    = pHalRuartAdapter->G2Uart;
    u8   ReturnData;

    if (PullMode) {
        while(1) {
            RegValue = (HAL_RUART_READ8(UartIndex, RUART_LINE_STATUS_REG_OFF));

            if (RegValue & RUART_LINE_STATUS_REG_DR) {
                /* LOGD("UART%d LSR(0x14):%X, Data Ready(0x01)\n", UartIndex, RegValue); */
                ReturnData = HAL_RUART_READ8(UartIndex, RUART_REV_BUF_REG_OFF);
                break;
            }
        }
    }
    else {
        if (G2Uart != 1)
            ReturnData = HAL_RUART_READ8(UartIndex, RUART_REV_BUF_REG_OFF);
        else
            ReturnData = HAL_G2UART_READ8(UartIndex, G2UART_RXDAT_REG_OFF);
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
HalRuartGetInterruptEnRegRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    RegValue = HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF);
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
HalRuartSetInterruptEnRegRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    u32 InterruptSetting;
    u8 G2Uart;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    InterruptSetting = pHalRuartAdapter->Interrupts;
    G2Uart = pHalRuartAdapter->G2Uart;

    if (G2Uart != 1) {
#if UART_DEBUG
        LOGD("[B] UART%d INT_EN(%02X) = %X\n", UartIndex, RUART_INTERRUPT_EN_REG_OFF,
                HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF));
#endif
        RegValue = 0x00;
        RegValue |= InterruptSetting;
        HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, RegValue);
#if UART_DEBUG
        LOGD("[A] UART%d INT_EN(%02X) = %X\n", UartIndex, RUART_INTERRUPT_EN_REG_OFF,
                HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF));
#endif
    } else {
#if UART_DEBUG
        printf("[B] UART%d INT_EN(%02X) = %X\n", UartIndex, G2UART_IE_REG_OFF,
                HAL_G2UART_READ32(UartIndex, G2UART_IE_REG_OFF));
#endif
        RegValue = 0x00;
        RegValue |= InterruptSetting;
        HAL_G2UART_WRITE32(UartIndex, G2UART_IE_REG_OFF, RegValue);
#if UART_DEBUG
        printf("[A] UART%d INT_EN(%02X) = %X\n", UartIndex, G2UART_IE_REG_OFF,
                HAL_G2UART_READ32(UartIndex, G2UART_IE_REG_OFF));
#endif
    }
}

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
HalRuartDmaInitRTKCommon(
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

    HAL_RUART_WRITE8(UartIndex, RUART_FIFO_CTL_REG_OFF, RegValue);

}

/**
 * Assert RUART RTS Signal.
 *
 * Assert RUART RTS Signal.
 *
 * @return VOID
 */
VOID
HalRuartRequestToSendRTKCommon(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 UartIndex;
    u32 RegValue;
    u8 G2Uart;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    G2Uart = pHalRuartAdapter->G2Uart;

    RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
    RegValue |= RUART_MCR_RTS;
    HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);

    printf("UART%d request to send\n",UartIndex);
	
}

