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
#include "peripheral.h"
#include "dw_spi_base.h"
//#include "dw_hal_spi.h"

extern void *
memset( void *s, int c, SIZE_T n);

#define ENABLE 1

BOOL
HalSsiPinmuxEnableDWCommon(
        IN VOID *Data
        )
{
    DBG_ENTRANCE;
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    BOOL Result;
    u32  RegValue;
    u8   Index;
    u8   PinmuxSelect;

    Index = pHalSsiAdapter->Index;
    PinmuxSelect = pHalSsiAdapter->PinmuxSelect;

    LOGD("PERI_ON_BASE + REG_SPI_MUX_CTRL = %X\n", PERI_ON_BASE + REG_SPI_MUX_CTRL);
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL);
    LOGD("[1][R] REG_SSI_MUX_CTRL(%X) = %X\n", PERI_ON_BASE + REG_SPI_MUX_CTRL, RegValue);

    switch (Index)
    {
        case 0:
            {
                ACTCK_SPI0_CCTRL(ON);
                //PinCtrl(SPI0,PinmuxSelect,ON);
                SPI0_FCTRL(ON);
                Result = _TRUE;
                break;
            }
        case 1:
            {
                ACTCK_SPI1_CCTRL(ON);
                //PinCtrl(SPI1,PinmuxSelect,ON);
                SPI1_FCTRL(ON);
                Result = _TRUE;
                break;
            }
        case 2:
            {
                ACTCK_SPI2_CCTRL(ON);
                //PinCtrl(SPI2,PinmuxSelect,ON);
                SPI2_FCTRL(ON);
                Result = _TRUE;
                break;
            }
        default:
            {
                LOGE(ANSI_COLOR_MAGENTA"Invalid SSI Index\n"ANSI_COLOR_RESET);
                Result = _FALSE;
                break;
            }
    }

    RegValue = HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL);
    LOGD("[2][R] REG_SSI_MUX_CTRL(%X) = %X\n", PERI_ON_BASE + REG_SPI_MUX_CTRL, RegValue);

    return Result;
}

BOOL
HalSsiEnableDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    BOOL Result;
    u32  RegValue;
    u8   Index;

    Index = pHalSsiAdapter->Index;

    RegValue = HAL_SSI_READ32(Index, REG_DW_SSI_SSIENR);
    RegValue |= BIT_SSIENR_SSI_EN(ENABLE);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_SSIENR, RegValue);

    Result = _TRUE;
    return Result;
}

BOOL HalSsiDisableDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    BOOL Result;
    u32  RegValue;
    u8   Index = pHalSsiAdapter->Index;

    RegValue = HAL_SSI_READ32(Index, REG_DW_SSI_SSIENR);
    RegValue &= BIT_INVC_SSIENR_SSI_EN;
    HAL_SSI_WRITE32(Index, REG_DW_SSI_SSIENR, RegValue);

    Result = _TRUE;
    return Result;
}

BOOL HalSsiInitDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    BOOL Result;
    u32  Ctrlr0Value  = 0;
    u32  Ctrlr1Value  = 0;
    u32  SerValue     = 0;
    u32  BaudrValue   = 0;
    u32  TxftlrValue  = 0;
    u32  RxftlrValue  = 0;
    u32  ImrValue     = 0;
    u32  DmacrValue   = 0;
    u32  MwcrValue    = 0;
    u32  DmatdlrValue = 0;
    u32  DmardlrValue = 0;
    u8   MicrowireTransferMode = pHalSsiAdapter->MicrowireTransferMode;
    u8   DataFrameFormat = pHalSsiAdapter->DataFrameFormat;
    u8   TransferMode = pHalSsiAdapter->TransferMode;
    u8   Index = pHalSsiAdapter->Index;
    u8   Role  = pHalSsiAdapter->Role;
    /* REG_DW_SSI_CTRLR0 */
    Ctrlr0Value |= BIT_CTRLR0_DFS_32(pHalSsiAdapter->DataFrameSize);
    Ctrlr0Value |= BIT_CTRLR0_FRF(pHalSsiAdapter->DataFrameFormat);
    Ctrlr0Value |= BIT_CTRLR0_SCPH(pHalSsiAdapter->SclkPhase);
    Ctrlr0Value |= BIT_CTRLR0_SCPOL(pHalSsiAdapter->SclkPolarity);
    Ctrlr0Value |= BIT_CTRLR0_TMOD(pHalSsiAdapter->TransferMode);
    Ctrlr0Value |= BIT_CTRLR0_CFS(pHalSsiAdapter->ControlFrameSize);

    if (Role == SSI_SLAVE)
    {
        Ctrlr0Value |= BIT_CTRLR0_SLV_OE(pHalSsiAdapter->SlaveOutputEnable);
    }
    HAL_SSI_WRITE32(Index, REG_DW_SSI_CTRLR0, Ctrlr0Value);
    /* REG_DW_SSI_TXFTLR */
    TxftlrValue |= BIT_TXFTLR_TFT(pHalSsiAdapter->TxThresholdLevel);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_TXFTLR, TxftlrValue);
    /* REG_DW_SSI_RXFTLR */
    RxftlrValue |= BIT_RXFTLR_RFT(pHalSsiAdapter->RxThresholdLevel);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_RXFTLR, RxftlrValue);
    /* REG_DW_SSI_IMR */
    ImrValue |= pHalSsiAdapter->InterruptMask;
    HAL_SSI_WRITE32(Index, REG_DW_SSI_IMR, ImrValue);
    /**
     * Master Only
     * REG_DW_SSI_CTRLR1, REG_DW_SSI_SER, REG_DW_SSI_BAUDR
     */
    if (Role & SSI_MASTER) {
        if ((TransferMode == TMOD_RO) || (TransferMode == TMOD_EEPROM_R) ||
                ((DataFrameFormat == FRF_NS_MICROWIRE) && (MicrowireTransferMode == MW_TMOD_SEQUENTIAL))) {
            Ctrlr1Value |= BIT_CTRLR1_NDF(pHalSsiAdapter->DataFrameNumber);
            HAL_SSI_WRITE32(Index, REG_DW_SSI_CTRLR1, Ctrlr1Value);
        }
        SerValue |= BIT_SER_SER(pHalSsiAdapter->SlaveSelectEnable);
        HAL_SSI_WRITE32(Index, REG_DW_SSI_SER, SerValue);
        BaudrValue |= BIT_BAUDR_SCKDV(pHalSsiAdapter->ClockDivider);
        HAL_SSI_WRITE32(Index, REG_DW_SSI_BAUDR, BaudrValue);
    }

    /**
     * DMA Configurations
     */
    DmacrValue |= pHalSsiAdapter->DmaControl;
    HAL_SSI_WRITE32(Index, REG_DW_SSI_DMACR, DmacrValue);
    /* LOGD("DmacrValue = %X\n",DmacrValue); */
    DmatdlrValue |= BIT_DMATDLR_DMATDL(pHalSsiAdapter->DmaTxDataLevel);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_DMATDLR, DmatdlrValue);
    /* LOGD("DmatdlrValue = %X\n", DmatdlrValue); */
    DmardlrValue |= BIT_DMARDLR_DMARDL(pHalSsiAdapter->DmaRxDataLevel);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_DMARDLR, DmardlrValue);
    /* LOGD("DmardlrValue = %X\n", DmardlrValue); */

    MwcrValue |= BIT_MWCR_MWMOD(pHalSsiAdapter->MicrowireTransferMode);
    MwcrValue |= BIT_MWCR_MDD(pHalSsiAdapter->MicrowireDirection);
    MwcrValue |= BIT_MWCR_MHS(pHalSsiAdapter->MicrowireHandshaking);
    HAL_SSI_WRITE32(Index, REG_DW_SSI_MWCR, MwcrValue);

    Result = _TRUE;
    return Result;
}

BOOL HalSsiSetSclkPolarityDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    /* PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data; */
    /* u8 Index = pHalSsiAdapter->Index; */
    return _TRUE;
}

BOOL HalSsiSetSclkPhaseDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    /* PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data; */
    /* u8 Index = pHalSsiAdapter->Index; */
    return _TRUE;
}

BOOL HalSsiWriterDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u16 TxData = BIT_DR_DR(pHalSsiAdapter->TxData);
    u8  Index = pHalSsiAdapter->Index;

    HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, TxData);

    return _TRUE;
}

u16 HalSsiReaderDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u8 Index = pHalSsiAdapter->Index;
    u16 RxData = HAL_SSI_READ16(Index, REG_DW_SSI_DR);

    return RxData;
}

u32 HalSsiGetRxFifoLevelDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u32 RxtflValue;
    u8  Index = pHalSsiAdapter->Index;

    RxtflValue = HAL_SSI_READ32(Index, REG_DW_SSI_RXFLR) & BIT_MASK_RXFLR_RXTFL;

    return RxtflValue;
}

u32 HalSsiGetTxFifoLevelDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u32 TxtflValue;
    u8  Index = pHalSsiAdapter->Index;

    TxtflValue = HAL_SSI_READ32(Index, REG_DW_SSI_TXFLR) & BIT_MASK_TXFLR_TXTFL;

    return TxtflValue;
}

u32 HalSsiGetStatusDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u32 StatusValue;
    u8  Index = pHalSsiAdapter->Index;

    StatusValue = HAL_SSI_READ32(Index, REG_DW_SSI_SR);

    return StatusValue;
}

u32 HalSsiGetInterruptStatusDWCommon(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE */
    PHAL_SSI_ADAPTER pHalSsiAdapter = (PHAL_SSI_ADAPTER) Data;
    u32 InterruptStatus;
    u8  Index = pHalSsiAdapter->Index;

    InterruptStatus = HAL_SSI_READ32(Index, REG_DW_SSI_ISR);

    return InterruptStatus;
}
