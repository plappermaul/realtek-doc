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
//#include "dw_i2c_base.h"
//#include "dw_hal_i2c.h"
#include "peripheral.h"


VOID
HalI2CWrite32(
    IN  u8      I2CIdx,
    IN  u8      I2CReg,
    IN  u32     I2CVal
){
    switch(I2CIdx)
    {
        case I2C0_SEL:
            HAL_WRITE32(I2C0_REG_BASE,I2CReg,I2CVal);
            break;
            
        case I2C1_SEL:
            HAL_WRITE32(I2C1_REG_BASE,I2CReg,I2CVal);
            break;
            
        case I2C2_SEL:
            HAL_WRITE32(I2C2_REG_BASE,I2CReg,I2CVal);
            break;
            
        case I2C3_SEL:
            HAL_WRITE32(I2C3_REG_BASE,I2CReg,I2CVal);
            break;

        default:
            break;
            
    }
}

u32
HalI2CRead32(
    IN  u8      I2CIdx,
    IN  u8      I2CReg
){
    u32 I2CDatRd = 0;
    
    switch(I2CIdx)
    {
        case I2C0_SEL:
            I2CDatRd = HAL_READ32(I2C0_REG_BASE,I2CReg);
            break;
            
        case I2C1_SEL:
            I2CDatRd = HAL_READ32(I2C1_REG_BASE,I2CReg);
            break;
            
        case I2C2_SEL:
            I2CDatRd = HAL_READ32(I2C2_REG_BASE,I2CReg);
            break;
            
        case I2C3_SEL:
            I2CDatRd = HAL_READ32(I2C3_REG_BASE,I2CReg);
            break;

        default:
            break;
            
    }

    return I2CDatRd;
}



BOOL
HalI2CModuleEnDWCommon(
    IN  VOID    *Data
)
{
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx;
    u8  I2CICEn;

    I2CIdx = pHalI2CDatAdapter->Idx;
    I2CICEn = pHalI2CDatAdapter->ModulEn;

    
    HAL_I2C_WRITE32(I2CIdx, REG_DW_I2C_IC_ENABLE, BIT_CTRL_IC_ENABLE(I2CICEn));


    
    #if CONFIG_DEBUG_LOG
        switch (I2CIdx)
        {
            case I2C0_SEL:
                printf("I2C_IC_ENABLE_%2x(%x): %x\n", I2CIdx, (I2C0_REG_BASE+REG_DW_I2C_IC_ENABLE), HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_ENABLE));
                break;
                
            case I2C1_SEL:
                printf( "I2C_IC_ENABLE_%2x(%x): %x\n", I2CIdx, (I2C1_REG_BASE+REG_DW_I2C_IC_ENABLE), HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_ENABLE));
                break;
            case I2C2_SEL:
                printf( "I2C_IC_ENABLE_%2x(%x): %x\n", I2CIdx, (I2C2_REG_BASE+REG_DW_I2C_IC_ENABLE), HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_ENABLE));
                break;
            case I2C3_SEL:
                printf( "I2C_IC_ENABLE_%2x(%x): %x\n", I2CIdx, (I2C3_REG_BASE+REG_DW_I2C_IC_ENABLE), HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_ENABLE));
                break;
            default:
                break;
        }        
    #endif
    
    return _TRUE;
}

BOOL
HalI2CInitMasterDWCommon(
    IN  VOID    *Data
)
{
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8 Master;
    u8 I2CIdx;
    u8 SpdMd;
    u8 AddrMd;
    u8 ReSTR;
    u8 Spec;
    u8 GC;
    u16 TSarAddr;
    u16 SdaHd;
    
    I2CIdx = pHalI2CDatAdapter->Idx;
    SpdMd = pHalI2CDatAdapter->SpdMd;
    AddrMd = pHalI2CDatAdapter->AddrMd;
    TSarAddr = pHalI2CDatAdapter->TSarAddr;
    Master = pHalI2CDatAdapter->Master;
    SdaHd = pHalI2CDatAdapter->SDAHd;
    ReSTR = pHalI2CDatAdapter->ReSTR;
    Spec = pHalI2CDatAdapter->Spec;
    GC = pHalI2CDatAdapter->GC_STR;
       
    //4 Disable the IC first
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_ENABLE,BIT_CTRL_IC_ENABLE(0));

    //4 To set IC_CON
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_CON, 
                            (BIT_CTRL_IC_CON_IC_SLAVE_DISABLE(1) |
                             BIT_CTRL_IC_CON_IC_RESTART_EN(ReSTR) |
                             BIT_CTRL_IC_CON_IC_10BITADDR_MASTER(AddrMd) |
                             BIT_CTRL_IC_CON_SPEED(SpdMd) |                            
                             BIT_CTRL_IC_CON_MASTER_MODE(Master)));
#if CONFIG_DEBUG_LOG    
    printf( "Init master, IC_CON%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_CON, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_CON));
#endif

    //4 To set target addr.
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_TAR,
                            (BIT_CTRL_IC_TAR_IC_10BITADDR_MASTER(AddrMd) |
                             BIT_CTRL_IC_TAR_SPECIAL(Spec)  |
                             BIT_CTRL_IC_TAR_GC_OR_START(GC)   |                            
                             BIT_CTRL_IC_TAR(TSarAddr)));

#if CONFIG_DEBUG_LOG
    printf( "Init master, IC_TAR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_TAR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_TAR));
#endif

    //4 To set SDA hold time
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SDA_HOLD,BIT_CTRL_IC_SDA_HOLD(SdaHd));

    //4 To set TX_Empty Level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_TX_TL,1);
    
    //4 To set RX_FULl Level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_RX_TL,0);
    
    //4 To set TX/RX FIFO level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_TDLR,0x09);
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_RDLR,0x03);
#if CONFIG_DEBUG_LOG
    printf( "Init master, I2C_IC_DMA_TDLR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_DMA_TDLR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_TDLR));
    printf( "Init master, I2C_IC_DMA_RDLR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_DMA_RDLR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_RDLR));
#endif

    return _TRUE;
}


BOOL
HalI2CInitSlaveDWCommon(
    IN  VOID    *Data
)
{
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8 Master;
    u8 I2CIdx;
    u8 SpdMd;
    u8 AddrMd;
    u8 GC;
    u16 TSarAddr;
    u16 SdaHd;
    
    I2CIdx = pHalI2CDatAdapter->Idx;
    SpdMd = pHalI2CDatAdapter->SpdMd;
    AddrMd = pHalI2CDatAdapter->AddrMd;
    TSarAddr = pHalI2CDatAdapter->TSarAddr;
    Master = pHalI2CDatAdapter->Master;
    SdaHd = pHalI2CDatAdapter->SDAHd;
    GC = pHalI2CDatAdapter->GC_STR;
    
    //4 Disable the IC first
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_ENABLE,BIT_CTRL_IC_ENABLE(0));

    //4 Write slave SAR
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SAR,BIT_CTRL_IC_SAR(TSarAddr));

#if CONFIG_DEBUG_LOG    
    printf( "Init slave, IC_SAR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_SAR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_SAR));
#endif     

    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_CON,
                            BIT_CTRL_IC_CON_IC_10BITADDR_SLAVE(AddrMd) |
                            BIT_CTRL_IC_CON_IC_SLAVE_DISABLE(Master) |
                            BIT_CTRL_IC_CON_SPEED(SpdMd)|
                            BIT_CTRL_IC_CON_MASTER_MODE(Master));

#if CONFIG_DEBUG_LOG    
    printf( "Init slave, IC_CON%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_CON, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_CON));
#endif

    //4 Set general call
    HAL_I2C_WRITE32(pHalI2CDatAdapter->Idx,REG_DW_I2C_IC_ACK_GENERAL_CALL,BIT_CTRL_IC_ACK_GENERAL_CALL(GC));

#if CONFIG_DEBUG_LOG
    printf( "Init slave, I2C_IC_ACK_GC%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_ACK_GENERAL_CALL, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_ACK_GENERAL_CALL));
#endif
    //4 to set SDA hold time
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SDA_HOLD,BIT_CTRL_IC_SDA_HOLD(SdaHd));
    //4 
    //4 to set SDA setup time
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SDA_SETUP,0x12);

    //4 To set TX_Empty Level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_TX_TL,1);
    
    //4 To set RX_FULl Level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_RX_TL,0);
    
    //4 To set TX/RX FIFO level
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_TDLR,0x09);
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_RDLR,0x03);
    
#if CONFIG_DEBUG_LOG
    printf( "Init slave, I2C_IC_DMA_TDLR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_DMA_TDLR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_TDLR));
    printf( "Init slave, I2C_IC_DMA_RDLR%d[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_DMA_RDLR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_RDLR));
#endif
    return _TRUE;
}

//only for Master mode
BOOL
HalI2CSetCLKDWCommon(
    IN  VOID    *Data
)
{
   PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
	u8  SpdMd = pHalI2CDatAdapter->SpdMd;
    u32 IcClk = pHalI2CDatAdapter->ICClk;
    u32 I2CClk = pHalI2CDatAdapter->SpdHz;
    u8  I2CIdx = pHalI2CDatAdapter->Idx;
    u32 ICHLcnt;

    u32 ICHtime;
    u32 ICLtime;
#if 0
    /* Get the IC-Clk setting first for the following process*/
#ifdef CONFIG_FPGA
    u32 IcClk = SYSTEM_CLK/1000000;
#else
    u32 IcClk;
    u32 ClkSELTmp = 0;
        
    ClkSELTmp = HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_SEL);

    if ((I2CClk > 0) && (I2CClk <= 400)) {
        ClkSELTmp &= (~(BIT_PESOC_PERI_SCLK_SEL(3)));
        ClkSELTmp |= BIT_PESOC_PERI_SCLK_SEL(3);
        HAL_WRITE32(PERI_ON_BASE,REG_PESOC_CLK_SEL,ClkSELTmp);
        IcClk = 12;     /*actually it's 12.5MHz*/
    }
    else {
        ClkSELTmp &= (~(BIT_PESOC_PERI_SCLK_SEL(3)));
        HAL_WRITE32(PERI_ON_BASE,REG_PESOC_CLK_SEL,ClkSELTmp);
        IcClk = 100;    
    }
#endif
#endif //if 0
    switch (SpdMd)  
    {
        case SS_MODE:
        {
            ICHtime = ((1000000/I2CClk)*I2C_SS_MIN_SCL_HTIME)/(I2C_SS_MIN_SCL_HTIME+I2C_SS_MIN_SCL_LTIME);
            ICLtime = ((1000000/I2CClk)*I2C_SS_MIN_SCL_LTIME)/(I2C_SS_MIN_SCL_HTIME+I2C_SS_MIN_SCL_LTIME);
            
            ICHLcnt = (ICHtime * IcClk)/1000;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SS_SCL_HCNT,ICHLcnt);
            
#ifdef CONFIG_DEBUG_LOG
            printf( "IC_SS_SCL_HCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_SS_SCL_HCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_SS_SCL_HCNT));
#endif

            ICHLcnt = (ICLtime * IcClk)/1000;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SS_SCL_LCNT,ICHLcnt);

#ifdef CONFIG_DEBUG_LOG
            printf( "IC_SS_SCL_LCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_SS_SCL_LCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_SS_SCL_LCNT));
#endif
            break;
        }
#if 1        
        case FS_MODE:
        {
            ICHtime = ((1000000/I2CClk)*I2C_FS_MIN_SCL_HTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);
            ICLtime = ((1000000/I2CClk)*I2C_FS_MIN_SCL_LTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);
            
            ICHLcnt = (ICHtime * IcClk)/1000;
            if (ICHLcnt>4)/*this part is according to the fine-tune result*/
                ICHLcnt -= 4;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_FS_SCL_HCNT,ICHLcnt);
            
#ifdef CONFIG_DEBUG_LOG
            printf( "IC_FS_SCL_HCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_FS_SCL_HCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_FS_SCL_HCNT));
#endif
            
            ICHLcnt = (ICLtime * IcClk)/1000;
            if (ICHLcnt>3)/*this part is according to the fine-tune result*/
                ICHLcnt -= 3;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_FS_SCL_LCNT,ICHLcnt);
            
#ifdef CONFIG_DEBUG_LOG
            printf( "IC_FS_SCL_LCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_FS_SCL_LCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_FS_SCL_LCNT));
#endif
            break;
        }

        case HS_MODE:
        {
            ICHLcnt = 400;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SS_SCL_HCNT,ICHLcnt);
            
            ICHLcnt = 470;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_SS_SCL_LCNT,ICHLcnt);
            
            ICHLcnt = 60;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_FS_SCL_HCNT,ICHLcnt);
            
            ICHLcnt = 130;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_FS_SCL_LCNT,ICHLcnt);

            ICHtime = ((1000000/I2CClk)*I2C_HS_MIN_SCL_HTIME_100)/(I2C_HS_MIN_SCL_HTIME_100+I2C_HS_MIN_SCL_LTIME_100);
            ICLtime = ((1000000/I2CClk)*I2C_HS_MIN_SCL_LTIME_100)/(I2C_HS_MIN_SCL_HTIME_100+I2C_HS_MIN_SCL_LTIME_100);

#ifdef CONFIG_DEBUG_LOG            
            printf("ICHtime:%x\n",ICHtime);
            printf("ICLtime:%x\n",ICLtime);
#endif

            ICHLcnt = (ICHtime * IcClk)/1000;
            if (ICHLcnt>8)/*this part is according to the fine-tune result*/
                ICHLcnt -= 3;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_HS_SCL_HCNT,ICHLcnt);
            
#ifdef CONFIG_DEBUG_LOG            
            printf( "IC_HS_SCL_HCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_HS_SCL_HCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_HS_SCL_HCNT));
#endif

            ICHLcnt = (ICLtime * IcClk)/1000;
            if (ICHLcnt>6)/*this part is according to the fine-tune result*/
                ICHLcnt -= 6;
            HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_HS_SCL_LCNT,ICHLcnt);
            
#ifdef CONFIG_DEBUG_LOG
            printf( "IC_HS_SCL_LCNT%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_HS_SCL_LCNT, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_HS_SCL_LCNT));
#endif

            break;
        }    
#endif
        default:
            break;
    }

    return _TRUE;
}


//3 For I2C RW
VOID
HalI2CDATWriteDWCommon(
    IN  VOID    *Data
){
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u8  I2CCmd  = pHalI2CDatAdapter->RWCmd;
    u8  I2CSTP;  //= pHalI2CDatAdapter->STP;
    u8  I2CDatLen = pHalI2CDatAdapter->DatLen;
    u8  *pDat   = pHalI2CDatAdapter->RWDat;
    u8  DatCnt = 0;

//DBG_8195A(">>>\n");
    for (DatCnt = 0; DatCnt < I2CDatLen; DatCnt++)
    {
        I2CSTP = 0;
        if (DatCnt == (I2CDatLen - 1))
        {
            I2CSTP = 1;
        }
//        DBG_8195A("datcnt: %x\n",DatCnt);
        HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DATA_CMD, 
                    *(pDat+DatCnt) |
                    BIT_CTRL_IC_DATA_CMD_CMD(I2CCmd) |
                    BIT_CTRL_IC_DATA_CMD_STOP(I2CSTP));    
    }
    
//    DBG_8195A("<<<\n");

}

u8
HalI2CDATReadDWCommon(
    IN  VOID    *Data
){
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u8  I2CRetDat = 0xFF;
    
    I2CRetDat = HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DATA_CMD);

    return (I2CRetDat);
}


//3 For I2C clear intr
VOID
HalI2CClrIntrDWCommon(
    IN  VOID    *Data
){
    //all of the I2C intr is cleared by reading the corresponding register;    
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u32 I2CIntrToClr = pHalI2CDatAdapter->IC_Intr_Clr;

    HAL_I2C_READ32(I2CIdx,I2CIntrToClr);
}

VOID
HalI2CClrAllIntrDWCommon(
    IN  VOID    *Data
){
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;

    HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_CLR_INTR);
}

VOID
HalI2CEnableIntrDWCommon(
    IN  VOID    *Data
){

    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u32 I2CIntrToEn = pHalI2CDatAdapter->IC_INRT_MSK;

#if 0//CONFIG_DEBUG_LOG    
    printf( "I2CIntrToEn:%x\n", I2CIntrToEn);
    
    printf( "In %s IC_INTR_MASK%d[%2x]: %x\n", __func__,I2CIdx,
                        REG_DW_I2C_IC_INTR_MASK, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK));

    printf( "I2CIntrToEn:%x\n", I2CIntrToEn);
#endif

    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_INTR_MASK, 
                                (I2CIntrToEn));

#if 0//CONFIG_DEBUG_LOG
    printf( "After Unmask INTR, IC_INTR_MASK%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_INTR_MASK, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK));
#endif
    
}

VOID
HalI2CEnableDMADWCommon(
        IN  VOID    *Data
){
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u8  I2CDMAEn;

    I2CDMAEn = HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_CR);
    I2CDMAEn |= pHalI2CDatAdapter->DMAEn;

    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_CR,I2CDMAEn);
    
#if 0//CONFIG_DEBUG_LOG
    printf( "IC_DMA_CR%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_DMA_CR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_CR));
#endif
}

VOID
HalI2CDisableDMADWCommon(
        IN  VOID    *Data
){
    PHAL_DW_I2C_DAT_ADAPTER pHalI2CDatAdapter = (PHAL_DW_I2C_DAT_ADAPTER) Data;
    u8  I2CIdx  = pHalI2CDatAdapter->Idx;
    u8  I2CDMAEn;

    I2CDMAEn = HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_CR);
    I2CDMAEn &= (~(pHalI2CDatAdapter->DMAEn));
    
    HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_DMA_CR,I2CDMAEn);
    
#if CONFIG_DEBUG_LOG
    printf( "IC_DMA_CR%d[%2x]: %x\n", I2CIdx,
                        REG_DW_I2C_IC_DMA_CR, HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DMA_CR));
#endif
}

