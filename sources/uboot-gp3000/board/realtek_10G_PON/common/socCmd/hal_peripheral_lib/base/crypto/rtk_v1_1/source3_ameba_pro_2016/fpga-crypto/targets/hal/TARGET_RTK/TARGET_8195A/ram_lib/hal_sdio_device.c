/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

#ifdef CONFIG_SDIO_DEVICE_EN

/******************************************************************************
 * Global Variable Declaration
 ******************************************************************************/
PHAL_SDIO_ADAPTER pgSDIODev = NULL;

/******************************************************************************
 * Function Prototype Declaration
 ******************************************************************************/
VOID HalSdioOpInit(
    IN  VOID *Data
);
VOID HalSdioInit(VOID);
VOID HalSdioDeInit(VOID);


VOID HalSdioOpInit(
    IN  VOID *Data
)
{
    PHAL_SDIO_OP pHalSdioOp = (PHAL_SDIO_OP) Data;

    pHalSdioOp->HalSdioDevInit = SDIO_Device_Init;
    pHalSdioOp->HalSdioDevDeInit = SDIO_Device_DeInit;
    pHalSdioOp->HalSdioSendC2HIOMsg = SDIO_Send_C2H_IOMsg;
    pHalSdioOp->HalSdioSendC2HPktMsg = SDIO_Send_C2H_PktMsg;
    pHalSdioOp->HalSdioRegTxCallback = SDIO_Register_Tx_Callback;
    pHalSdioOp->HalSdioRxCallback = SDIO_Rx_Callback;
#if SDIO_API_DEFINED
    pHalSdioOp->HalSdioRegRxDoneCallback = SDIO_Register_Rx_Done_Callback;
#endif
#if SDIO_MP_MODE
    pHalSdioOp->HalSdioDevMPApp = SDIO_DeviceMPApp;
#endif
}

VOID HalSdioInit(VOID)
{
    PHAL_SDIO_OP pHalSdioOp=NULL;

    pgSDIODev = (PHAL_SDIO_ADAPTER)RtlZmalloc(sizeof(HAL_SDIO_ADAPTER));
    if (NULL == pgSDIODev) {
        DBG_SDIO_ERR("HalSdioInit: Malloc for SDIO Adapter Err!!\n");
        goto HAL_SDIO_INIT_ERR;
    }

    pHalSdioOp = (PHAL_SDIO_OP) RtlMalloc(sizeof(HAL_SDIO_OP));
    if (NULL == pHalSdioOp) {
        DBG_SDIO_ERR("HAL SDIO Alloc OP Err!!\n");
        goto HAL_SDIO_INIT_ERR;
    }
    pgSDIODev->pHalOp = (VOID *)pHalSdioOp;

    HalSdioOpInit((VOID *)(pHalSdioOp));

#if SDIO_API_DEFINED
    pgSDIODev->spdio_priv = (VOID *)g_spdio_priv;
#endif

    if (SUCCESS != pHalSdioOp->HalSdioDevInit(pgSDIODev)) {
        DBG_SDIO_ERR("HAL SDIO Init Err!!\n");
        goto HAL_SDIO_INIT_ERR;
    }
    return;

HAL_SDIO_INIT_ERR:
    if (NULL != pgSDIODev) {
        RtlMfree((u8 *)pgSDIODev, sizeof(HAL_SDIO_ADAPTER));
        pgSDIODev = NULL;
    }

    if (NULL != pHalSdioOp) {
        RtlMfree((u8 *)pHalSdioOp, sizeof(HAL_SDIO_OP));
    }
}

VOID HalSdioDeInit(VOID)
{
    PHAL_SDIO_OP pHalSdioOp=NULL;

    if (NULL != pgSDIODev->pHalOp) {
        pHalSdioOp = pgSDIODev->pHalOp;
        pHalSdioOp->HalSdioDevDeInit(pgSDIODev);
        RtlMfree((u8 *)pgSDIODev->pHalOp, sizeof(HAL_SDIO_OP));
        pgSDIODev->pHalOp = NULL;
    }

    if (NULL != pgSDIODev) {
        RtlMfree((u8 *)pgSDIODev, sizeof(HAL_SDIO_ADAPTER));
        pgSDIODev = NULL;
    }
}

#if SDIO_API_DEFINED
s8 HalSdioRxCallback(
    IN VOID *pData,
    IN u16 Offset,
    IN u16 PktSize,
    IN u8 CmdType
)
{
    PHAL_SDIO_OP pHalSdioOp=NULL;
    if(pgSDIODev == NULL) {
        DBG_SDIO_ERR("HAL SDIO isn't Init yet!!\n");
        return FAIL;
    }
    pHalSdioOp = pgSDIODev->pHalOp;
    return pHalSdioOp->HalSdioRxCallback(pgSDIODev, pData, Offset, PktSize, CmdType);
}

VOID HalSdioRegisterTxCallback(
    IN s8 (*CallbackFun)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize, u8 Type),
    IN VOID *pAdapter
)
{
    PHAL_SDIO_OP pHalSdioOp=NULL;
    if(pgSDIODev == NULL) {
        DBG_SDIO_ERR("HAL SDIO isn't Init yet!!\n");
        return;
    }
    pHalSdioOp = pgSDIODev->pHalOp;
    pHalSdioOp->HalSdioRegTxCallback(pgSDIODev, CallbackFun, pAdapter);
}

VOID HalSdioRegisterRxDoneCallback(
    IN s8 (*CallbackFun)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize, u8 Type),
    IN VOID *pAdapter
)
{
    PHAL_SDIO_OP pHalSdioOp=NULL;
    if(pgSDIODev == NULL) {
        DBG_SDIO_ERR("HAL SDIO isn't Init yet!!\n");
        return;
    }
    pHalSdioOp = pgSDIODev->pHalOp;
    pHalSdioOp->HalSdioRegRxDoneCallback(pgSDIODev, CallbackFun, pAdapter);
}
#endif

#endif //#ifdef CONFIG_SDIO_DEVICE_EN
