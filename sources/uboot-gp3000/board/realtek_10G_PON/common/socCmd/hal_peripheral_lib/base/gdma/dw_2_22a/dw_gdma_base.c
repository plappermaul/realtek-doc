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
//#include "dw_gdma_base.h"
//#include "dw_hal_gdma.h"

#if 0	//defined (SOC_TYPE_8198E)		// Carl: TO DO check

#elif defined(__96F_PHERIPHERAL_H__)	//defined (SOC_TYPE_8196F)
//#define VIR_2_PHY(Addr) (((Addr & 0xf0000000)==0xb0000000) ? (Addr & (~0xA0000000)) : (Addr))
#define VIR_2_PHY(Addr) 	(Addr & (~0xA0000000))
#elif defined(__97G_PHERIPHERAL_H__)
#define VIR_2_PHY(Addr) 	(Addr & (~0xA0000000))
#elif defined(__98F_PHERIPHERAL_H__)
#define VIR_2_PHY(Addr) 	(Addr)
#elif defined(__98X_PHERIPHERAL_H__)
#define VIR_2_PHY(Addr)         (Addr)
#endif

extern void *
memset( void *s, int c, SIZE_T n );

VOID
HalGdmaOnOffDWCommon (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                      REG_GDMA_DMAC_CFG,
                      pHalGdmaAdapter->GdmaOnOff
                      );

}

BOOL
HalGdamChInitDWCommon(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    memset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));

    //4 1) Initial Cfg Register

    //4 2) Initial Ctl Register
    pHalGdmaAdapter->GdmaCtl.IntEn = 1;
    pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
    pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    pHalGdmaAdapter->GdmaCtl.SrcMsize = MsizeEight;
    pHalGdmaAdapter->GdmaCtl.DestMsize = MsizeEight;
    pHalGdmaAdapter->GdmaCtl.SrcGatherEn = 0;
    pHalGdmaAdapter->GdmaCtl.DstScatterEn = 0;
    pHalGdmaAdapter->GdmaCtl.Sinc = 0;
    pHalGdmaAdapter->GdmaCtl.Dinc = 0;

//Initial SGR and DSR
    pHalGdmaAdapter->GdmaSgr.Sgi =0;
    pHalGdmaAdapter->GdmaSgr.Sgc =0;
    pHalGdmaAdapter->GdmaDsr.Dsi =0;
    pHalGdmaAdapter->GdmaDsr.Dsc =0;

    return _TRUE;
}


BOOL
HalGdmaChSetingDWCommon(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u8  PendingIsrIndex;
    u8  GdmaIndex = pHalGdmaAdapter->GdmaIndex;
    u32 CtlxLow, CtlxUp, CfgxLow, CfgxUp, Dsrlow, Sgrlow;
    u32 ChEn = pHalGdmaAdapter->ChEn;
    u8  GdmaChIsrBitmap = (ChEn & 0xFF);
    u8  ChNum = pHalGdmaAdapter->ChNum;

    //4 1) Check chanel is avaliable
    if (HAL_GDMAX_READ32(GdmaIndex, REG_GDMA_CH_EN) & ChEn) {
        //4 Disable Channel
        printf("Channel had used; Disable Channel!!!!\n");

        HalGdmaChDisDWCommon(Data);

    }

    //4 2) Check if there are the pending isr; TFR, Block, Src Tran, Dst Tran,  Error
    for (PendingIsrIndex=0; PendingIsrIndex<5;PendingIsrIndex++) {

        u32 PendRaw, PendStstus;
        PendRaw = HAL_GDMAX_READ32(GdmaIndex,
                                        (REG_GDMA_RAW_INT_BASE + PendingIsrIndex*8));
        PendStstus = HAL_GDMAX_READ32(GdmaIndex,
                                        (REG_GDMA_STATUS_INT_BASE + PendingIsrIndex*8));

        if ((PendRaw & GdmaChIsrBitmap) || (PendStstus & GdmaChIsrBitmap)) {
            //4 Clear Pending Isr
            HAL_GDMAX_WRITE32(GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE + PendingIsrIndex*8),
                              (PendStstus & (GdmaChIsrBitmap))
                              );

        }
    }


    //4 3) Process CTLx
    CtlxLow = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF));

        //4 Clear CTLx low register bits
    CtlxLow &= (BIT_INVC_CTLX_LO_INT_EN &
                BIT_INVC_CTLX_LO_DST_TR_WIDTH &
                BIT_INVC_CTLX_LO_SRC_TR_WIDTH &
                BIT_INVC_CTLX_LO_DINC &
                BIT_INVC_CTLX_LO_SINC &
                BIT_INVC_CTLX_LO_SRC_GATHER_EN &
                BIT_INVC_CTLX_LO_DST_SCATTER_EN &
                BIT_INVC_CTLX_LO_DEST_MSIZE &
                BIT_INVC_CTLX_LO_SRC_MSIZE &
                BIT_INVC_CTLX_LO_TT_FC &
                BIT_INVC_CTLX_LO_LLP_DST_EN &
                BIT_INVC_CTLX_LO_LLP_SRC_EN);

    CtlxUp = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF + 4));


            //4 Clear CTLx upper register bits
        /* Clear BLOCK_TS, DONE */
    CtlxUp &= (BIT_INVC_CTLX_UP_BLOCK_BS &
               BIT_INVC_CTLX_UP_DONE);

    CtlxLow = BIT_CTLX_LO_INT_EN(pHalGdmaAdapter->GdmaCtl.IntEn) |
              BIT_CTLX_LO_DST_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.DstTrWidth) |
              BIT_CTLX_LO_SRC_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.SrcTrWidth) |
              BIT_CTLX_LO_DINC(pHalGdmaAdapter->GdmaCtl.Dinc) |
              BIT_CTLX_LO_SINC(pHalGdmaAdapter->GdmaCtl.Sinc) |
              BIT_CTLX_LO_DEST_MSIZE(pHalGdmaAdapter->GdmaCtl.DestMsize) |
              BIT_CTLX_LO_SRC_MSIZE(pHalGdmaAdapter->GdmaCtl.SrcMsize) |
              BIT_CTLX_LO_SRC_GATHER_EN(pHalGdmaAdapter->GdmaCtl.SrcGatherEn) |
              BIT_CTLX_LO_DST_SCATTER_EN(pHalGdmaAdapter->GdmaCtl.DstScatterEn) |
              BIT_CTLX_LO_TT_FC(pHalGdmaAdapter->GdmaCtl.TtFc) |
              BIT_CTLX_LO_LLP_DST_EN(pHalGdmaAdapter->GdmaCtl.LlpDstEn) |
              BIT_CTLX_LO_LLP_SRC_EN(pHalGdmaAdapter->GdmaCtl.LlpSrcEn) |
              CtlxLow;

    CtlxUp = BIT_CTLX_UP_BLOCK_BS(pHalGdmaAdapter->GdmaCtl.BlockSize) |
             BIT_CTLX_UP_DONE(pHalGdmaAdapter->GdmaCtl.Done) |
             CtlxUp;


            //4 Fill in SARx register
        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_SAR + ChNum*REG_GDMA_CH_OFF),
                          VIR_2_PHY(pHalGdmaAdapter->ChSar)
                          );


            //4 Fill in DARx register
        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_DAR + ChNum*REG_GDMA_CH_OFF),
                          VIR_2_PHY(pHalGdmaAdapter->ChDar)
                          );

            //4 Fill in CTLx register
        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF),
                          CtlxLow
                          );

        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF +4),
                          CtlxUp
                          );

#if FLYSKY_DEBUG
    CtlxLow = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF));
            printf("%s %d: GdmaIndex=%d, ChNum=%d,  CtlxLow=0x%08x\n ", __func__, __LINE__, GdmaIndex, ChNum, CtlxLow);
#endif
        //4 4) Program CFGx

        CfgxLow = HAL_GDMAX_READ32(GdmaIndex,
                                  (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF));

        CfgxLow &= (BIT_INVC_CFGX_LO_CH_PRIOR &
                    BIT_INVC_CFGX_LO_CH_SUSP &
                    BIT_INVC_CFGX_LO_HS_SEL_DST &
                    BIT_INVC_CFGX_LO_HS_SEL_SRC &
                    BIT_INVC_CFGX_LO_LOCK_CH_L &
                    BIT_INVC_CFGX_LO_LOCK_B_L &
                    BIT_INVC_CFGX_LO_LOCK_CH &
                    BIT_INVC_CFGX_LO_LOCK_B &
                    BIT_INVC_CFGX_LO_RELOAD_SRC &
                    BIT_INVC_CFGX_LO_RELOAD_DST);

        CfgxUp = HAL_GDMAX_READ32(GdmaIndex,
                                  (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF + 4));

        CfgxUp &= (BIT_INVC_CFGX_UP_FIFO_MODE &
                   BIT_INVC_CFGX_UP_DS_UPD_EN &
                   BIT_INVC_CFGX_UP_SS_UPD_EN &
                   BIT_INVC_CFGX_UP_SRC_PER &
                   BIT_INVC_CFGX_UP_DEST_PER);

        CfgxLow = BIT_CFGX_LO_CH_PRIOR(pHalGdmaAdapter->GdmaCfg.ChPrior) |
                  BIT_CFGX_LO_CH_SUSP(pHalGdmaAdapter->GdmaCfg.ChSusp) |
                  BIT_CFGX_LO_HS_SEL_DST(pHalGdmaAdapter->GdmaCfg.HsSelDst) |
                  BIT_CFGX_LO_HS_SEL_SRC(pHalGdmaAdapter->GdmaCfg.HsSelSrc) |
                  BIT_CFGX_LO_LOCK_CH_L(pHalGdmaAdapter->GdmaCfg.LockChL) |
                  BIT_CFGX_LO_LOCK_B_L(pHalGdmaAdapter->GdmaCfg.LockBL) |
                  BIT_CFGX_LO_LOCK_CH(pHalGdmaAdapter->GdmaCfg.LockCh) |
                  BIT_CFGX_LO_LOCK_B(pHalGdmaAdapter->GdmaCfg.LockB) |
                  BIT_CFGX_LO_RELOAD_SRC(pHalGdmaAdapter->GdmaCfg.ReloadSrc) |
                  BIT_CFGX_LO_RELOAD_DST(pHalGdmaAdapter->GdmaCfg.ReloadDst) |
                  CfgxLow;

        CfgxUp = BIT_CFGX_UP_FIFO_MODE(pHalGdmaAdapter->GdmaCfg.FifoMode) |
                 BIT_CFGX_UP_DS_UPD_EN(pHalGdmaAdapter->GdmaCfg.DsUpdEn) |
                 BIT_CFGX_UP_SS_UPD_EN(pHalGdmaAdapter->GdmaCfg.SsUpdEn) |
                 BIT_CFGX_UP_SRC_PER(pHalGdmaAdapter->GdmaCfg.SrcPer) |
                 BIT_CFGX_UP_DEST_PER(pHalGdmaAdapter->GdmaCfg.DestPer) |
                 CfgxUp;


        //printf("CFG Low data:0x%x\n",CfgxLow);

        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF),
                          CfgxLow
                          );

        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF +4),
                          CfgxUp
                          );


       /* DSR or SGR */
        Dsrlow = pHalGdmaAdapter->GdmaDsr.Dsc<<20;
        Dsrlow |= pHalGdmaAdapter->GdmaDsr.Dsi;


        HAL_GDMAX_WRITE32(GdmaIndex,
                          (REG_GDMA_CH_DSR + ChNum*REG_GDMA_CH_OFF ),
                          Dsrlow
                          );
#if FLYSKY_DEBUG
        printf("%s %d: GdmaIndex=%d, ChNum=%d,  dsrlow=0x%x\n ", __func__, __LINE__, GdmaIndex, ChNum, Dsrlow);
        printf("%s %d:  DSR: 0x%x\n ", __func__, __LINE__,HAL_GDMAX_READ32(GdmaIndex,
                                  (REG_GDMA_CH_DSR + ChNum*REG_GDMA_CH_OFF )) );
#endif
        Sgrlow = pHalGdmaAdapter->GdmaSgr.Sgc<<20 | pHalGdmaAdapter->GdmaSgr.Sgi;
        HAL_GDMAX_WRITE32(GdmaIndex,    (REG_GDMA_CH_SGR + ChNum*REG_GDMA_CH_OFF ),      Sgrlow);
#if FLYSKY_DEBUG
        printf("%s %d: SGR:  0x%x\n ", __func__, __LINE__,HAL_GDMAX_READ32(GdmaIndex,
                                  (REG_GDMA_CH_SGR + ChNum*REG_GDMA_CH_OFF )) );
#endif

    return _TRUE;
}

BOOL
HalGdmaChBlockSetingDWCommon(
    IN  VOID     *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    PGDMA_CH_LLI_ELE pLliEle;
    struct GDMA_CH_LLI *pGdmaChLli;
    struct BLOCK_SIZE_LIST *pGdmaChBkLi;
    u32 MultiBlockCount = pHalGdmaAdapter->MaxMuliBlock;
    u32 CtlxLow, CtlxUp, CfgxLow, CfgxUp;
    u8  GdmaIndex = pHalGdmaAdapter->GdmaIndex;
    u8  ChNum = pHalGdmaAdapter->ChNum;
    u32 ChEn = pHalGdmaAdapter->ChEn;
    u8  GdmaChIsrBitmap = (ChEn & 0xFF);
    u8  PendingIsrIndex;

    PGDMA_CH_LLI_ELE pLliEle_head;

    //printf("%s(%d) \n", __FUNCTION__, __LINE__);

    pLliEle = pHalGdmaAdapter->pLlix->pLliEle;
    pGdmaChLli = pHalGdmaAdapter->pLlix->pNextLli;
    pGdmaChBkLi = pHalGdmaAdapter->pBlockSizeList;

    //printf("%s(%d) \n", __FUNCTION__, __LINE__);

    //4 1) Check chanel is avaliable
    if (HAL_GDMAX_READ32(GdmaIndex, REG_GDMA_CH_EN) & ChEn) {
        //4 Disable Channel
        printf("Channel had used; Disable Channel!!!!\n");

        HalGdmaChDisDWCommon(Data);

    }

    //printf("%s(%d) \n", __FUNCTION__, __LINE__);

    //4 2) Check if there are the pending isr; TFR, Block, Src Tran, Dst Tran,  Error
    for (PendingIsrIndex=0; PendingIsrIndex<5;PendingIsrIndex++) {

        u32 PendRaw, PendStstus;
        PendRaw = HAL_GDMAX_READ32(GdmaIndex,
                                        (REG_GDMA_RAW_INT_BASE + PendingIsrIndex*8));
        PendStstus = HAL_GDMAX_READ32(GdmaIndex,
                                        (REG_GDMA_STATUS_INT_BASE + PendingIsrIndex*8));

        if ((PendRaw & GdmaChIsrBitmap) || (PendStstus & GdmaChIsrBitmap)) {
            //4 Clear Pending Isr
            HAL_GDMAX_WRITE32(GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE + PendingIsrIndex*8),
                              (PendStstus & (GdmaChIsrBitmap))
                              );

        }
    }

    //printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, pHalGdmaAdapter->ChSar, pHalGdmaAdapter->ChDar);

        //4 Fill in SARx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_SAR + ChNum*REG_GDMA_CH_OFF),
                      VIR_2_PHY(pHalGdmaAdapter->ChSar)
                      );


        //4 Fill in DARx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_DAR + ChNum*REG_GDMA_CH_OFF),
                      VIR_2_PHY(pHalGdmaAdapter->ChDar)
                      );



    //4 3) Process CTLx
    CtlxLow = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF));

        //4 Clear Config low register bits
    CtlxLow &= (BIT_INVC_CTLX_LO_INT_EN &
                BIT_INVC_CTLX_LO_DST_TR_WIDTH &
                BIT_INVC_CTLX_LO_SRC_TR_WIDTH &
                BIT_INVC_CTLX_LO_DINC &
                BIT_INVC_CTLX_LO_SINC &
                BIT_INVC_CTLX_LO_DEST_MSIZE &
                BIT_INVC_CTLX_LO_SRC_MSIZE &
                BIT_INVC_CTLX_LO_TT_FC &
                BIT_INVC_CTLX_LO_LLP_DST_EN &
                BIT_INVC_CTLX_LO_LLP_SRC_EN);

    CtlxUp = HAL_GDMAX_READ32(GdmaIndex,
                               (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF + 4));

        //4 Clear Config upper register bits
    CtlxUp &= (BIT_INVC_CTLX_UP_BLOCK_BS &
               BIT_INVC_CTLX_UP_DONE);


    CtlxLow = BIT_CTLX_LO_INT_EN(pHalGdmaAdapter->GdmaCtl.IntEn) |
              BIT_CTLX_LO_DST_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.DstTrWidth) |
              BIT_CTLX_LO_SRC_TR_WIDTH(pHalGdmaAdapter->GdmaCtl.SrcTrWidth) |
              BIT_CTLX_LO_DINC(pHalGdmaAdapter->GdmaCtl.Dinc) |
              BIT_CTLX_LO_SINC(pHalGdmaAdapter->GdmaCtl.Sinc) |
              BIT_CTLX_LO_DEST_MSIZE(pHalGdmaAdapter->GdmaCtl.DestMsize) |
              BIT_CTLX_LO_SRC_MSIZE(pHalGdmaAdapter->GdmaCtl.SrcMsize) |
              BIT_CTLX_LO_TT_FC(pHalGdmaAdapter->GdmaCtl.TtFc) |
              BIT_CTLX_LO_LLP_DST_EN(pHalGdmaAdapter->GdmaCtl.LlpDstEn) |
              BIT_CTLX_LO_LLP_SRC_EN(pHalGdmaAdapter->GdmaCtl.LlpSrcEn) |
              CtlxLow;

    CtlxUp = BIT_CTLX_UP_BLOCK_BS(pGdmaChBkLi->BlockSize) |
             BIT_CTLX_UP_DONE(pHalGdmaAdapter->GdmaCtl.Done) |
             CtlxUp;

        //4 Fill in CTLx register
    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF),
                      CtlxLow
                      );

    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CTL + ChNum*REG_GDMA_CH_OFF +4),
                      CtlxUp
                      );

    //4 4) Program CFGx

    CfgxLow = HAL_GDMAX_READ32(GdmaIndex,
                              (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF));

    CfgxLow &= (BIT_INVC_CFGX_LO_CH_PRIOR &
                BIT_INVC_CFGX_LO_CH_SUSP &
                BIT_INVC_CFGX_LO_HS_SEL_DST &
                BIT_INVC_CFGX_LO_HS_SEL_SRC &
                BIT_INVC_CFGX_LO_LOCK_CH_L &
                BIT_INVC_CFGX_LO_LOCK_B_L &
                BIT_INVC_CFGX_LO_LOCK_CH &
                BIT_INVC_CFGX_LO_LOCK_B &
                BIT_INVC_CFGX_LO_RELOAD_SRC &
                BIT_INVC_CFGX_LO_RELOAD_DST);

    CfgxUp = HAL_GDMAX_READ32(GdmaIndex,
                              (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF + 4));

    CfgxUp &= (BIT_INVC_CFGX_UP_FIFO_MODE &
               BIT_INVC_CFGX_UP_DS_UPD_EN &
               BIT_INVC_CFGX_UP_SS_UPD_EN &
               BIT_INVC_CFGX_UP_SRC_PER &
               BIT_INVC_CFGX_UP_DEST_PER);

    CfgxLow = BIT_CFGX_LO_CH_PRIOR(pHalGdmaAdapter->GdmaCfg.ChPrior) |
              BIT_CFGX_LO_CH_SUSP(pHalGdmaAdapter->GdmaCfg.ChSusp) |
              BIT_CFGX_LO_HS_SEL_DST(pHalGdmaAdapter->GdmaCfg.HsSelDst) |
              BIT_CFGX_LO_HS_SEL_SRC(pHalGdmaAdapter->GdmaCfg.HsSelSrc) |
              BIT_CFGX_LO_LOCK_CH_L(pHalGdmaAdapter->GdmaCfg.LockChL) |
              BIT_CFGX_LO_LOCK_B_L(pHalGdmaAdapter->GdmaCfg.LockBL) |
              BIT_CFGX_LO_LOCK_CH(pHalGdmaAdapter->GdmaCfg.LockCh) |
              BIT_CFGX_LO_LOCK_B(pHalGdmaAdapter->GdmaCfg.LockB) |
              BIT_CFGX_LO_RELOAD_SRC(pHalGdmaAdapter->GdmaCfg.ReloadSrc) |
              BIT_CFGX_LO_RELOAD_DST(pHalGdmaAdapter->GdmaCfg.ReloadDst) |
              CfgxLow;

    CfgxUp = BIT_CFGX_UP_FIFO_MODE(pHalGdmaAdapter->GdmaCfg.FifoMode) |
             BIT_CFGX_UP_DS_UPD_EN(pHalGdmaAdapter->GdmaCfg.DsUpdEn) |
             BIT_CFGX_UP_SS_UPD_EN(pHalGdmaAdapter->GdmaCfg.SsUpdEn) |
             BIT_CFGX_UP_SRC_PER(pHalGdmaAdapter->GdmaCfg.SrcPer) |
             BIT_CFGX_UP_DEST_PER(pHalGdmaAdapter->GdmaCfg.DestPer) |
             CfgxUp;

    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF),
                      CfgxLow
                      );

    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_CFG + ChNum*REG_GDMA_CH_OFF +4),
                      CfgxUp
                      );



    //4 Check 4 Bytes Alignment
    if ((u32)(pLliEle) & 0x3) {
        //printf("LLi Addr: 0x%x not 4 bytes alignment!!!!\n", pHalGdmaAdapter->pLli);
        return _FALSE;
    }

    HAL_GDMAX_WRITE32(GdmaIndex,
                      (REG_GDMA_CH_LLP + ChNum*REG_GDMA_CH_OFF),
                      VIR_2_PHY((u32)pLliEle)
                      );

    pLliEle_head = pLliEle;

    //4 Update the first llp0
    pLliEle->CtlxLow = CtlxLow;
    pLliEle->CtlxUp = CtlxUp;
    pLliEle->Llpx = VIR_2_PHY((u32)pGdmaChLli->pLliEle);
    //printf("Block Count %d\n", MultiBlockCount);

    pGdmaChBkLi = pGdmaChBkLi->pNextBlockSiz;

    while (MultiBlockCount > 1) {
        MultiBlockCount--;
        //printf("Block Count %d\n", MultiBlockCount);
        pLliEle = pGdmaChLli->pLliEle;

        if (NULL == pLliEle) {
            //printf("pLliEle Null Point!!!!!\n");
            return _FALSE;
        }

        //4 Clear the last element llp enable bit
        if (1 == MultiBlockCount) {
            CtlxLow &= (BIT_INVC_CTLX_LO_LLP_DST_EN &
                       BIT_INVC_CTLX_LO_LLP_SRC_EN);
        }
        //4 Update block size for transfer
        CtlxUp &= (BIT_INVC_CTLX_UP_BLOCK_BS);
        CtlxUp |= BIT_CTLX_UP_BLOCK_BS(pGdmaChBkLi->BlockSize);

        //printf("%s(%d) \n", __FUNCTION__, __LINE__);
        //4 Update tje Lli and Block size list point to next llp
        pGdmaChLli = pGdmaChLli->pNextLli;
        pGdmaChBkLi = pGdmaChBkLi->pNextBlockSiz;

        //printf("%s(%d) \n", __FUNCTION__, __LINE__);
        //4 Updatethe Llpx context
        pLliEle->CtlxLow = CtlxLow;
        pLliEle->CtlxUp = CtlxUp;

        //printf("%s(%d): 0x%x \n", __FUNCTION__, __LINE__, pGdmaChLli);

        if (pGdmaChLli == NULL) {
            //printf("%s(%d) pGdmaChLli is NULL !!! \n", __FUNCTION__, __LINE__);
            pLliEle->Llpx = pGdmaChLli;
            break;
        } else {
            pLliEle->Llpx = VIR_2_PHY((u32)pGdmaChLli->pLliEle);
        }

        //printf("%s(%d) \n", __FUNCTION__, __LINE__);
    }

    flush_cache(pLliEle_head, sizeof(struct GDMA_CH_LLI) * 16);
    //printf("%s(%d): pLliEle_head:0x%x \n", __FUNCTION__, __LINE__, pLliEle_head);

    return _TRUE;
}



VOID
HalGdmaChIsrEnAndDisDWCommon (
    IN  VOID              *Data
)
{
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


}



VOID
HalGdmaChEnDWCommon (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                      REG_GDMA_CH_EN,
                      (HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)|
                        (pHalGdmaAdapter->ChEn))
                      );

}

VOID
HalGdmaChDisDWCommon (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                      REG_GDMA_CH_EN,
                      ((HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN)&
                        ~(pHalGdmaAdapter->ChEn & 0xFF)) | (pHalGdmaAdapter->ChEn & 0xFF00))
                      );
}

u8
HalGdmaChIsrCleanDWCommon (
    IN  VOID              *Data
)
{
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) Data;
    u32 IsrStatus;
    u8  IsrTypeIndex = 0, IsrActBitMap = 0;

    for (IsrTypeIndex=0; IsrTypeIndex<5; IsrTypeIndex++) {

        IsrStatus = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex,
                                    (REG_GDMA_RAW_INT_BASE + IsrTypeIndex*8));


        IsrStatus = (IsrStatus & (pHalGdmaAdapter->ChEn & 0xFF));

#if 1	 //#ifndef MY_STUDY
   //    if (BIT_(IsrTypeIndex) & pHalGdmaAdapter->GdmaIsrType)
#endif
	{
#if	1	//#ifndef MY_STUDY
            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE+ (IsrTypeIndex*8)),
                              (IsrStatus)// & (pHalGdmaAdapter->ChEn & 0xFF))
                              );
#else
		printf(" ----Isr_Status %d\n",  HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_STATUS_INT_BASE + IsrTypeIndex*8)));	// Carl
		printf(" ----StatusInt %d\n",  HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_STATUS_INT + IsrTypeIndex*0)));	// Carl

            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE+ (IsrTypeIndex*8)),
                              (IsrStatus)// & (pHalGdmaAdapter->ChEn & 0xFF))	// need to clear, or will always ISR
                              );
	printf("@@@ study mode @@@\n");
#endif
            IsrActBitMap |= BIT_(IsrTypeIndex);

        }

    }
    return IsrActBitMap;

}


VOID
HalGdmaChCleanAutoSrcDWCommon (
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

    //printf("CFG Low data(ChNum=%d, 0X%08X):0x%x\n", pHalGdmaAdapter->ChNum, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF), HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));
}

VOID
HalGdmaChCleanAutoDstDWCommon (
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
    //printf("CFG Low data:0x%x\n", HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, (REG_GDMA_CH_CFG + pHalGdmaAdapter->ChNum*REG_GDMA_CH_OFF)));

}

