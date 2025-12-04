
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_GDMA_BASE_H_
#define _DW_GDMA_BASE_H_

#include "hal_api.h"
#include "dw_gdma_base_reg.h"
#include "dw_gdma_base_bit.h"

#define HAL_GDMAX_READ32(GdmaIndex, addr)     \
                HAL_READ32(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE32(GdmaIndex, addr, value)     \
                HAL_WRITE32((GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF)), addr, value)
#define HAL_GDMAX_READ16(GdmaIndex, addr)     \
                HAL_READ16(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE16(GdmaIndex, addr, value)     \
                HAL_WRITE16(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr, value)
#define HAL_GDMAX_READ8(GdmaIndex, addr)     \
                HAL_READ8(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE8(GdmaIndex, addr, value)     \
                HAL_WRITE8(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr, value)


typedef enum _GDMA_CHANNEL_NUM_ {
        GdmaNoCh    = 0x0000,
        GdmaCh0     = 0x0101,
        GdmaCh1     = 0x0202,
        GdmaCh2     = 0x0404,
        GdmaCh3     = 0x0808,
#if 0
        GdmaCh4     = 0x1010,
        GdmaCh5     = 0x2020,
        GdmaCh6     = 0x4040,
        GdmaCh7     = 0x8080,
#endif
        GdmaAllCh   = 0xffff
}GDMA_CHANNEL_NUM, *PGDMA_CHANNEL_NUM;


//3 CTL register struct

typedef enum _GDMA_CTL_TT_FC_TYPE_ {
        TTFCMemToMem    = 0x00,
        TTFCMemToPeri   = 0x01,
        TTFCPeriToMem   = 0x02
}GDMA_CTL_TT_FC_TYPE, *PGDMA_CTL_TT_FC_TYPE;

//Max type = Bus Width
typedef enum _GDMA_CTL_TR_WIDTH_ {
        TrWidthOneByte    = 0x00,
        TrWidthTwoBytes   = 0x01,
        TrWidthFourBytes  = 0x02
}GDMA_CTL_TR_WIDTH, *PGDMA_CTL_TR_WIDTH;

typedef enum _GDMA_CTL_MSIZE_ {
        MsizeOne        = 0x00,
        MsizeFour       = 0x01,
        MsizeEight      = 0x02
}GDMA_CTL_MSIZE, *PGDMA_CTL_MSIZE;

typedef enum _GDMA_INC_TYPE_ {
        IncType         = 0x00,
        DecType         = 0x01,
        NoChange        = 0x02
}GDMA_INC_TYPE, *PGDMA_INC_TYPE;


typedef struct _GDMA_CTL_REG_ {
        GDMA_CTL_TT_FC_TYPE     TtFc;
        GDMA_CTL_TR_WIDTH       DstTrWidth;
        GDMA_CTL_TR_WIDTH       SrcTrWidth;
        GDMA_INC_TYPE           Dinc;
        GDMA_INC_TYPE           Sinc;
        GDMA_CTL_MSIZE          DestMsize;
        GDMA_CTL_MSIZE          SrcMsize;

        u8                      IntEn           :1; // Bit 0
        u8                      SrcGatherEn     :1; // Bit 1
        u8                      DstScatterEn    :1; // Bit 2
        u8                      LlpDstEn        :1; // Bit 3
        u8                      LlpSrcEn        :1; // Bit 4
        u8                      Done            :1; // Bit 5
        u8                      Rsvd6To7        :2; //Bit 6 -7
        u16                     BlockSize;
        
}GDMA_CTL_REG, *PGDMA_CTL_REG;


//3 CFG Register Structure

typedef enum _GDMA_CH_PRIORITY_ {
    Prior0          = 0,
    Prior1          = 1,
    Prior2          = 2,
    Prior3          = 3,
    Prior4          = 4,
    Prior5          = 5,
    Prior6          = 6,
    Prior7          = 7
}GDMA_CH_PRIORITY, *PGDMA_CH_PRIORITY;

typedef enum _GDMA_LOCK_LEVEL_ {
    OverComplDmaTransfer        = 0x00,
    OverComplDmaBlockTransfer   = 0x01,
    OverComplDmaTransation      = 0x02
}GDMA_LOCK_LEVEL, *PGDMA_LOCK_LEVEL;


typedef struct _GDMA_CFG_REG_ {
    GDMA_CH_PRIORITY        ChPrior;
    GDMA_LOCK_LEVEL         LockBL;     
    GDMA_LOCK_LEVEL         LockChL;     
    u16                     MaxAbrst;
    u8                      SrcPer;
    u8                      DestPer;
    u16                     ChSusp      :1; //Bit 0
    u16                     FifoEmpty   :1; //Bit 1
    u16                     HsSelDst    :1; //Bit 2
    u16                     HsSelSrc    :1; //Bit 3
    u16                     LockCh      :1; //Bit 4
    u16                     LockB       :1; //Bit 5
    u16                     DstHsPol    :1; //Bit 6
    u16                     SrcHsPol    :1; //Bit 7
    u16                     ReloadSrc   :1; //Bit 8
    u16                     ReloadDst   :1; //Bit 9
    u16                     FifoMode    :1; //Bit 10
    u16                     DsUpdEn     :1; //Bit 11
    u16                     SsUpdEn     :1; //Bit 12
    u16                     Rsvd13To15  :3;
}GDMA_CFG_REG, *PGDMA_CFG_REG;

typedef struct _GDMA_SGR_REG_ {
     u32                    Sgi:20;
     u32                    Sgc:12;
}GDMA_SGR_REG, *PGDMA_SGR_REG;

typedef struct _GDMA_DSR_REG_ {
     u32                    Dsi:20;
     u32                    Dsc:12;
}GDMA_DSR_REG, *PGDMA_DSR_REG;


typedef enum _GDMA_ISR_TYPE_ {
    TransferType        = 0x1,
    BlockType           = 0x2,
    SrcTransferType     = 0x4,
    DstTransferType     = 0x8,
    ErrType             = 0x10
}GDMA_ISR_TYPE, *PGDMA_ISR_TYPE;


                
VOID
HalGdmaOnOffDWCommon (
    IN  VOID              *Data
);

BOOL
HalGdamChInitDWCommon(
    IN  VOID     *Data
);

BOOL
HalGdmaChSetingDWCommon(
    IN  VOID     *Data
);

BOOL
HalGdmaChBlockSetingDWCommon(
    IN  VOID     *Data
);


VOID
HalGdmaChDisDWCommon (
    IN  VOID              *Data
);

VOID
HalGdmaChEnDWCommon (
    IN  VOID              *Data
);

VOID
HalGdmaChIsrEnAndDisDWCommon (
    IN  VOID              *Data
);

u8
HalGdmaChIsrCleanDWCommon (
    IN  VOID              *Data
);

VOID
HalGdmaChCleanAutoSrcDWCommon (
    IN  VOID              *Data
);

VOID
HalGdmaChCleanAutoDstDWCommon (
    IN  VOID              *Data
); 

#endif //#ifndef _GDMA_BASE_H_

