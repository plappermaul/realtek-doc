/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtk_hal_uart.h"

VOID
HalRuartOpInit(
        IN VOID *Data
)
{
    PHAL_RUART_OP pHalRuartOp = (PHAL_RUART_OP) Data;

    pHalRuartOp->HalRuartResetRxFifo        = HalRuartResetRxFifoRTKCommon;
    pHalRuartOp->HalRuartInit               = HalRuartInitRTKCommon;
    pHalRuartOp->HalRuartSend               = HalRuartSendRTKCommon;
    pHalRuartOp->HalRuartRecv               = HalRuartRecvRTKCommon;
    pHalRuartOp->HalRuartGetInterruptEnReg  = HalRuartGetInterruptEnRegRTKCommon;
    pHalRuartOp->HalRuartSetInterruptEnReg  = HalRuartSetInterruptEnRegRTKCommon;
    pHalRuartOp->HalRuartGetDebugValue      = HalRuartGetDebugValueRTKCommon;
    pHalRuartOp->HalRuartDmaInit            = HalRuartDmaInitRTKCommon;
    pHalRuartOp->HalRuartRequestToSend      = HalRuartRequestToSendRTKCommon;
}
