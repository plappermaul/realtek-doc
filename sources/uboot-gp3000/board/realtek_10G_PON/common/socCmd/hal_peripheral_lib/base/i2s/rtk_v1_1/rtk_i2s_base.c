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
#include "rtk_i2s_base.h"
//#include "rtk_hal_i2s.h"
#include "peripheral.h"


extern void *
memset( void *s, int c, SIZE_T n );
/*
VOID
HalI2sOnOffRTKCommon (
    IN  VOID              *Data
)
{
    PHAL_I2S_ADAPTER pHalI2sAdapter = (PHAL_I2S_ADAPTER) Data;
    //todo on off i2s

}
*/
VOID
HalI2SWrite32(
    IN  u8      I2SIdx,
    IN  u8      I2SReg,
    IN  u32     I2SVal
)
{
    switch(I2SIdx)
    {
        case I2S0_SEL:
            HAL_WRITE32(I2S0_REG_BASE, I2SReg, I2SVal);
            break;

        case I2S1_SEL:
#if 0
            HAL_WRITE32(I2S1_REG_BASE, I2SReg, I2SVal);
#endif
            break;

        default:
            break;

    }
}

u32
HalI2SRead32(
    IN  u8      I2SIdx,
    IN  u8      I2SReg
)
{
    u32 I2SDatRd = 0;

    switch(I2SIdx)
    {
        case I2S0_SEL:
            I2SDatRd = HAL_READ32(I2S0_REG_BASE, I2SReg);
            break;

        case I2S1_SEL:
#if 0
            I2SDatRd = HAL_READ32(I2S1_REG_BASE, I2SReg);
#endif
            break;

        default:
            break;

    }

    return I2SDatRd;
}

RTK_STATUS
HalI2SInitRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
    u8  I2SEn;
    u8  I2SMaster;
    u8  I2SWordLen;
    u8  I2SChNum;
    u8  I2SPageNum;
    u16 I2SPageSize;
    u16 I2SRate;
    u32 I2STxIntrMSK;
    u32 I2SRxIntrMSK;
    u8  I2STRxAct;
    u8  *I2STxData;
    u8  *I2SRxData;

    u32 Tmp;

    I2SIdx         = pHalI2SInitData->I2SIdx;
    I2SEn          = pHalI2SInitData->I2SEn;
    I2SMaster      = pHalI2SInitData->I2SMaster;
    I2SWordLen     = pHalI2SInitData->I2SWordLen;
    I2SChNum       = pHalI2SInitData->I2SChNum;
    I2SPageNum     = pHalI2SInitData->I2SPageNum;
    I2SPageSize    = pHalI2SInitData->I2SPageSize;
    I2SRate        = pHalI2SInitData->I2SRate;
    I2STRxAct      = pHalI2SInitData->I2STRxAct;
    I2STxData      = pHalI2SInitData->I2STxData;
    I2SRxData      = pHalI2SInitData->I2SRxData;

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "[%s:%d\n", __func__, __LINE__);

    /* Disable the I2S first, and reset to default */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
                                         BIT_CTRL_CTLX_I2S_SW_RSTN(1));
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "[%s:%d\n", __func__, __LINE__);
//    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
//                                         BIT_CTRL_CTLX_I2S_SW_RSTN(0));
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "[%s:%d\n", __func__, __LINE__);
//    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, BIT_CTRL_CTLX_I2S_EN(0) | 
//                                         BIT_CTRL_CTLX_I2S_SW_RSTN(1));

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "[%s:%d\n", __func__, __LINE__);
    Tmp = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    Tmp |= BIT_CTRL_CTLX_I2S_ENDIAN_SWAP(1);

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    if (I2SRate&0x10) 
    {
        //Tmp |= BIT_CTRL_CTLX_I2S_CLK_SRC(1);
    }

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    Tmp |= (BIT_CTRL_CTLX_I2S_WL(I2SWordLen) | BIT_CTRL_CTLX_I2S_CH_NUM(I2SChNum) |
            BIT_CTRL_CTLX_I2S_SLAVE_MODE(I2SMaster) | BIT_CTRL_CTLX_I2S_TRX_ACT(I2STRxAct));
    /* set 44.1khz clock source, word length, channel number, master or slave, trx act */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, Tmp);

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    Tmp = BIT_CTRL_SETTING_I2S_PAGE_SZ(I2SPageSize) | BIT_CTRL_SETTING_I2S_PAGE_NUM(I2SPageNum) |
          BIT_CTRL_SETTING_I2S_SAMPLE_RATE(I2SRate);
    if (I2SRate&0x10) 
    {
        Tmp |= BIT_CTRL_SETTING_I2S_CLK_SRC(1);
    }
    /* set page size, page number, sample rate */
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_SETTING, Tmp);

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    /* need tx rx buffer? need rx page own bit */
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE_PTR, I2STxData);
    HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE_PTR, I2SRxData);
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    if ((I2STRxAct == TxOnly) || (I2STRxAct == TXRX))
    {
        memset(I2STxData, 0, 4*(I2SPageSize+1)*(I2SPageNum+1));
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE0_OWN, 1<<31);
		HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE1_OWN, 0);
		HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE2_OWN, 0);
		HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE3_OWN, 0);
    } else {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE0_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE1_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE2_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE3_OWN, 0);
    }
    if ((I2STRxAct == RxOnly) || (I2STRxAct == TXRX))
    {
        if(I2SRxData != 0)
            memset(I2SRxData, 0, 4*(I2SPageSize+1)*(I2SPageNum+1));
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE0_OWN, 1<<31);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE1_OWN, 1<<31);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE2_OWN, 1<<31);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE3_OWN, 1<<31);
    } else {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE0_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE1_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE2_OWN, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE3_OWN, 0);

    }
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
	pHalI2SInitData->I2STxIdx = 1;
	pHalI2SInitData->I2SRxIdx = 0;
        pHalI2SInitData->I2SHWTxIdx = 0;
        pHalI2SInitData->I2SHWRxIdx = 0;
    /* I2S Clear all interrupts first */
    HalI2SClrAllIntrRTKCommon(pHalI2SInitData);
DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);

    /* I2S Disable all interrupts first */
    I2STxIntrMSK   = pHalI2SInitData->I2STxIntrMSK;
    I2SRxIntrMSK   = pHalI2SInitData->I2SRxIntrMSK;
    pHalI2SInitData->I2STxIntrMSK = 0;
    pHalI2SInitData->I2SRxIntrMSK = 0;
    HalI2SIntrCtrlRTKCommon(pHalI2SInitData);
    pHalI2SInitData->I2STxIntrMSK = I2STxIntrMSK;
    pHalI2SInitData->I2SRxIntrMSK = I2SRxIntrMSK;

DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s:%d\n", __func__, __LINE__);
    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SDeInitRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
    u32 Tmp;
    
    I2SIdx = pHalI2SInitData->I2SIdx;
    Tmp = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL);
    Tmp &= (~(BIT_CTRL_CTLX_I2S_EN(1) | BIT_CTRL_CTLX_I2S_SW_RSTN(1)));

    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, Tmp);

#ifdef CONFIG_DEBUG_LOG
    DBG_8195A_I2S_LVL(HAL_I2S_LVL, "I2S_DEINIT_%x(): %x\n", I2SIdx, HAL_I2S_READ32(I2SIdx, REG_I2S_CTL));


#endif
    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2STxRTKCommon(
    IN  VOID    *Data,
	IN  u8      *pBuff
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
	u16 I2STxIdx = pHalI2SInitData->I2STxIdx;
	u8  I2SPageNum = pHalI2SInitData->I2SPageNum;
	u16 I2SPageSize = pHalI2SInitData->I2SPageSize;
    u32 Tmp;
    
    I2SIdx = pHalI2SInitData->I2SIdx;
    
	memcpy(((u32)(pHalI2SInitData->I2STxData)) + I2STxIdx*((I2SPageSize+1)*4),
	       pBuff, (I2SPageSize+1)*4);
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_PAGE0_OWN+4*I2STxIdx, 1<<31);
    I2STxIdx++;
    I2STxIdx = I2STxIdx%(I2SPageNum+1);
    pHalI2SInitData->I2STxIdx = I2STxIdx;

    return _EXIT_SUCCESS;
}


RTK_STATUS
HalI2SRxRTKCommon(
    IN  VOID    *Data,
	OUT u8      *pBuff
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
	u16 I2SRxIdx = pHalI2SInitData->I2SRxIdx;
	u8  I2SPageNum = pHalI2SInitData->I2SPageNum;
	u16 I2SPageSize = pHalI2SInitData->I2SPageSize;

    I2SIdx = pHalI2SInitData->I2SIdx;

	memcpy(pBuff, ((u32)(pHalI2SInitData->I2SRxData)) + I2SRxIdx*((I2SPageSize+1)*4),
	       (I2SPageSize+1)*4);
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_PAGE0_OWN+4*I2SRxIdx, 1<<31);
    I2SRxIdx++;
    I2SRxIdx = I2SRxIdx%(I2SPageNum+1);
    pHalI2SInitData->I2SRxIdx = I2SRxIdx;

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SEnableRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
    u8  I2SIdx;
    u8  I2SEn;
    u8  I2STRxAct;
    u32 Tmp;
	u32 I2STxIntrMSK;
	u32 I2SRxIntrMSK;

    I2SIdx    = pHalI2SInitData->I2SIdx;
    I2SEn     = pHalI2SInitData->I2SEn;
    I2STRxAct = pHalI2SInitData->I2STRxAct;
	I2STxIntrMSK = pHalI2SInitData->I2STxIntrMSK;
	I2SRxIntrMSK = pHalI2SInitData->I2SRxIntrMSK;

    if (I2SEn == 1) // enable i2s
    {
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_MASK_INT, I2STxIntrMSK);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_MASK_INT, I2SRxIntrMSK);
        //HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_MASK_INT, 0);
        //HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_MASK_INT, 0);


    } else { // disable i2s
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_MASK_INT, 0);
        HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_MASK_INT, 0);


    }
    Tmp = HAL_I2S_READ32(I2SIdx, REG_I2S_CTL) & (~BIT_CTRL_CTLX_I2S_EN(1));
    Tmp |= BIT_CTRL_CTLX_I2S_EN(I2SEn);

    HAL_I2S_WRITE32(I2SIdx, REG_I2S_CTL, Tmp);

#ifdef CONFIG_DEBUG_LOG

    DBG_8195A_I2S_LVL(HAL_I2S_LVL, "I2S_ENABLE_%x(): %x\n", I2SIdx, HAL_I2S_READ32(I2SIdx, REG_I2S_CTL));

#endif

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SIntrCtrlRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
    u32 I2STxIntrMSK;
    u32 I2SRxIntrMSK;

    I2SIdx = pHalI2SInitData->I2SIdx;
    I2STxIntrMSK = pHalI2SInitData->I2STxIntrMSK;
    I2SRxIntrMSK = pHalI2SInitData->I2SRxIntrMSK;

    HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_MASK_INT, I2STxIntrMSK);

    HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_MASK_INT, I2SRxIntrMSK);
    return _EXIT_SUCCESS;
}

u32
HalI2SReadRegRTKCommon(
    IN  VOID    *Data,
    IN  u8      I2SReg
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;

    u8  I2SIdx;
    
    I2SIdx = pHalI2SInitData->I2SIdx;

    return HAL_I2S_READ32(I2SIdx, I2SReg);
}

RTK_STATUS
HalI2SSetRateRTKCommon(
    IN  VOID    *Data
)
{
    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetWordLenRTKCommon(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetChNumRTKCommon(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetPageNumRTKCommon(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SSetPageSizeRTKCommon(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SClrIntrRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
	
	u8  I2SIdx;
	u32 I2STxIntrClr;
	u32 I2SRxIntrClr;
	
	I2SIdx           =  pHalI2SInitData->I2SIdx;
	I2STxIntrClr     =  pHalI2SInitData->I2STxIntrClr;
	I2SRxIntrClr     =  pHalI2SInitData->I2SRxIntrClr;
	
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_STATUS_INT, I2STxIntrClr);
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_STATUS_INT, I2SRxIntrClr);
	
    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SClrAllIntrRTKCommon(
    IN  VOID    *Data
)
{
    PHAL_I2S_INIT_DAT pHalI2SInitData = (PHAL_I2S_INIT_DAT)Data;
	
	u8  I2SIdx;
	u32 I2STxIntrClr;
	u32 I2SRxIntrClr;
	
	I2SIdx           =  pHalI2SInitData->I2SIdx;
	I2STxIntrClr     =  0x3f;
	I2SRxIntrClr     =  0x3f;
	
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_TX_STATUS_INT, I2STxIntrClr);
	HAL_I2S_WRITE32(I2SIdx, REG_I2S_RX_STATUS_INT, I2SRxIntrClr);

    return _EXIT_SUCCESS;
}

RTK_STATUS
HalI2SDMACtrlRTKCommon(
    IN  VOID    *Data
)
{

    return _EXIT_SUCCESS;
}

#if 0 //old code

//default sampling rate 8khz, stereo, 4page, 10ms frame size, i2s mode, tx+rx
//          master mode, enable endian swap
// Question: need local tx/rx page?
BOOL
HalI2sInitRTKCommon(
    IN  VOID     *Data
)
{

    PHAL_I2S_ADAPTER pHalI2sAdapter = (PHAL_I2S_ADAPTER) Data;
    memset((void *)pHalI2sAdapter, 0, sizeof(HAL_I2S_ADAPTER));

    //4 1) Initial Setting Register
    pHalI2sAdapter->I2sSetting.PageNum     = I2s4Page;
    pHalI2sAdapter->I2sSetting.SampleRate  = I2sSR8K;
    pHalI2sAdapter->I2sSetting.PageSize    = 79; // 80word= 8khz*0.01s*2ch*2byte/4byte
    //4 2) Initial Ctl Register

    pHalI2sAdapter->I2sCtl.I2s_En      = 0;
    pHalI2sAdapter->I2sCtl.TrxAct      = TXRX;
    pHalI2sAdapter->I2sCtl.ChNum       = ChannelStereo;
    pHalI2sAdapter->I2sCtl.EdgeSw      = 0;
    pHalI2sAdapter->I2sCtl.WordLength  = 0;
    pHalI2sAdapter->I2sCtl.LoopBack    = 0;
    pHalI2sAdapter->I2sCtl.Format      = FormatI2s;
    pHalI2sAdapter->I2sCtl.DacLrSwap   = 0;
    pHalI2sAdapter->I2sCtl.SckInv      = 0;
    pHalI2sAdapter->I2sCtl.EndianSwap  = 1;
    pHalI2sAdapter->I2sCtl.SlaveMode   = 0;
    pHalI2sAdapter->I2sCtl.SR44p1KHz   = 0;
    pHalI2sAdapter->I2sCtl.SwRstn      = 1; // todo : need 1 or 0? 20130814

    return _TRUE;        
}


BOOL
HalI2sSettingRTKCommon(
    IN  VOID     *Data
)
{

    PHAL_I2S_ADAPTER pHalI2sAdapter = (PHAL_I2S_ADAPTER) Data;
    u8  I2sIndex = pHalI2sAdapter->I2sIndex;
    u32 RegCtl, RegSetting;
    u32 PendTxStatus, PendRxStatus;
    
    //4 1) Check I2s index is avaliable
    if (HAL_I2SX_READ32(I2sIndex, REG_I2S_CTL) & BIT_CTLX_IIS_EN(1)) {
        //4 I2s index is running, stop first
        DBG_8195A_DMA("Error, I2S %d is running; stop first!\n", I2sIndex);

        return _FALSE;
    }

    //4 2) Check if there are the pending isr


        PendTxStatus = HAL_I2SX_READ32(I2sIndex, REG_I2S_TX_STATUS_INT);
        PendRxStatus = HAL_I2SX_READ32(I2sIndex, REG_I2S_RX_STATUS_INT);

            //4 Clear Pending Isr
        HAL_I2SX_WRITE32(I2sIndex, REG_I2S_TX_STATUS_INT, PendTxStatus);
        HAL_I2SX_WRITE32(I2sIndex, REG_I2S_RX_STATUS_INT, PendRxStatus);
        //} 


    //4 3) Process RegCtl 
    RegCtl = HAL_I2SX_READ32(I2sIndex, REG_I2S_CTL);

        //4 Clear Ctl register bits
    RegCtl &= ( BIT_INV_CTLX_TRX &
                BIT_INV_CTLX_CH_NUM &
                BIT_INV_CTLX_EDGE_SW &
                BIT_INV_CTLX_WL &
                BIT_INV_CTLX_LOOP_BACK &
                BIT_INV_CTLX_FORMAT &
                BIT_INV_CTLX_LRSWAP &
                BIT_INV_CTLX_SCK_INV &
                BIT_INV_CTLX_ENDIAN_SWAP &
                BIT_INV_CTLX_SLAVE_SEL &
                BIT_INV_CTLX_SW_RSTN);

    RegCtl =  BIT_CTLX_TRX(pHalI2sAdapter->I2sCtl.TrxAct) |
              BIT_CTLX_CH_NUM(pHalI2sAdapter->I2sCtl.ChNum) |
              BIT_CTLX_EDGE_SW(pHalI2sAdapter->I2sCtl.EdgeSw) |
              BIT_CTLX_WL(pHalI2sAdapter->I2sCtl.WordLength) |
              BIT_CTLX_LOOP_BACK(pHalI2sAdapter->I2sCtl.LoopBack) |
              BIT_CTLX_FORMAT(pHalI2sAdapter->I2sCtl.Format) |
              BIT_CTLX_LRSWAP(pHalI2sAdapter->I2sCtl.DacLrSwap) |
              BIT_CTLX_SCK_INV(pHalI2sAdapter->I2sCtl.SckInv) |
              BIT_CTLX_ENDIAN_SWAP(pHalI2sAdapter->I2sCtl.EndianSwap) |
              BIT_CTLX_SLAVE_SEL(pHalI2sAdapter->I2sCtl.SlaveMode) |
              BIT_CTLX_CLK_SRC(pHalI2sAdapter->I2sCtl.SR44p1KHz) |
              RegCtl; 

        //4 4) Program Setting Register
        
        RegSetting = HAL_I2SX_READ32(I2sIndex, REG_I2S_SETTING);

        RegSetting &= (BIT_INV_SETTING_PAGE_SZ &
                    BIT_INV_SETTING_PAGE_NUM &
                    BIT_INV_SETTING_SAMPLE_RATE);

        RegSetting = BIT_SETTING_PAGE_SZ(pHalI2sAdapter->I2sSetting.PageSize) |
                     BIT_SETTING_PAGE_NUM(pHalI2sAdapter->I2sSetting.PageNum) |
                     BIT_SETTING_SAMPLE_RATE(pHalI2sAdapter->I2sSetting.SampleRate) |
                     RegSetting;
        
        DBG_8195A_DMA("RegSetting data:0x%x\n", RegSetting);

        HAL_I2SX_WRITE32(I2sIndex, REG_I2S_SETTING, RegSetting);
        
    return _TRUE;
}

BOOL
HalI2sEnRTKCommon(
    IN  VOID     *Data
)
{

    PHAL_I2S_ADAPTER pHalI2sAdapter = (PHAL_I2S_ADAPTER) Data;
    u8  I2sIndex = pHalI2sAdapter->I2sIndex;
    u32 RegCtl;

    pHalI2sAdapter->Enable = 1;


    //4 1) Check I2s index is avaliable
    RegCtl = HAL_I2SX_READ32(I2sIndex, REG_I2S_CTL);
    if (RegCtl & BIT_CTLX_IIS_EN(1)) {
        //4 I2s index is running, stop first
        DBG_8195A_DMA("Error, I2S %d is running; stop first!\n", I2sIndex);

        return _FALSE;
    }

    HAL_I2SX_WRITE32(I2sIndex, REG_I2S_CTL, RegCtl|BIT_CTLX_IIS_EN(1));
    pHalI2sAdapter->I2sCtl.I2s_En = 1;

    return _TRUE;
}

BOOL
HalI2sDisRTKCommon(
    IN  VOID     *Data
)
{

    PHAL_I2S_ADAPTER pHalI2sAdapter = (PHAL_I2S_ADAPTER) Data;
    u8  I2sIndex = pHalI2sAdapter->I2sIndex;
    u32 RegCtl;

    pHalI2sAdapter->Enable = 0;


    //4 1) Check I2s index is avaliable
    RegCtl = HAL_I2SX_READ32(I2sIndex, REG_I2S_CTL);
    if (RegCtl & BIT_CTLX_IIS_EN(1)) {
        //4 I2s index is running, stop first
        DBG_8195A_DMA("Error, I2S %d is running; stop first!\n", I2sIndex);

        return _FALSE;
    }

    HAL_I2SX_WRITE32(I2sIndex, REG_I2S_CTL, RegCtl&(~BIT_CTLX_IIS_EN(1)));
    pHalI2sAdapter->I2sCtl.I2s_En = 0;

    return _TRUE;
}



BOOL
HalI2sIsrEnAndDisRTKCommon (
    IN  VOID              *Data
)
{
/*
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u32 IsrMask, Addr, IsrCtrl;
    u8  IsrTypeIndex = 0;

    for (IsrTypeIndex=0; IsrTypeIndex<5; IsrTypeIndex++) {
        
        if (BIT_(IsrTypeIndex) & pHalGdmaAdapter->GdmaIsrType) {
            Addr =  (REG_GDMA_MASK_INT_BASE + IsrTypeIndex*8);

            IsrMask = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, Addr);

            IsrCtrl = ((pHalGdmaAdapter->IsrCtrl)?(pHalGdmaAdapter->ChEn | IsrMask):
                                                 ((~pHalGdmaAdapter->ChEn) & IsrMask));

            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              Addr, 
                              IsrCtrl
                              );
                    
        }   
    }
*/
    return _TRUE;
}



BOOL
HalI2sDumpRegRTKCommon (
    IN  VOID              *Data
)
{
/*    
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex, 
                      REG_GDMA_CH_EN, 
                      (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)| 
                        (pHalGdmaAdapter->ChEn))
                      );    
*/
    return _TRUE;    
}

BOOL
HalI2sRTKCommon (
    IN  VOID              *Data
)
{
/*    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex, 
                      REG_GDMA_CH_EN, 
                      (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)& 
                        ~(pHalGdmaAdapter->ChEn))
                      );    
*/		      
    return _TRUE;		      
}
/*
u8
HalGdmaChIsrCleanRTKCommon (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u32 IsrStatus;
    u8  IsrTypeIndex = 0, IsrActBitMap = 0;

    for (IsrTypeIndex=0; IsrTypeIndex<5; IsrTypeIndex++) {

        IsrStatus = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 
                                    (REG_GDMA_RAW_INT_BASE + IsrTypeIndex*8));

//        DBG_8195A_DMA("Isr Type %d: Isr Status 0x%x\n", IsrTypeIndex, IsrStatus);

        IsrStatus = (IsrStatus & (pHalGdmaAdapter->ChEn & 0xFF));
        
        if (BIT_(IsrTypeIndex) & pHalGdmaAdapter->GdmaIsrType) {
            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE+ (IsrTypeIndex*8)), 
                              (IsrStatus)// & (pHalGdmaAdapter->ChEn & 0xFF))
                              );
            IsrActBitMap |= BIT_(IsrTypeIndex);
            
        }
        
    }
    return IsrActBitMap;
     
}


VOID
HalGdmaChCleanAutoSrcRTKCommon (
    IN  VOID              *Data
) 
{
    u32 CfgxLow;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    CfgxLow = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= BIT_INVC_CFGX_LO_RELOAD_SRC;

    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                  (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF), 
                  CfgxLow
                  );
    
    DBG_8195A_DMA("CFG Low data:0x%x\n",
    HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));
}

VOID
HalGdmaChCleanAutoDstRTKCommon (
    IN  VOID              *Data
) 
{
    u32 CfgxLow;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    CfgxLow = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= BIT_INVC_CFGX_LO_RELOAD_DST;

    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                  (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF), 
                  CfgxLow
                  );
    DBG_8195A_DMA("CFG Low data:0x%x\n",
    HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));
        
}
*/

#endif

