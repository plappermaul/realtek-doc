/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_HAL_I2S_H_
#define _RTK_HAL_I2S_H_

#include "basic_types.h"
//#include "rtk_i2s_base.h"
#include "peripheral.h"

/*
typedef struct _GDMA_CH_LLI_ELE_ {
    u32                   Sarx;
    u32                   Darx;
    u32                   Llpx;
    u32                   CtlxLow;
    u32                   CtlxUp;
    u32                   Temp;
}GDMA_CH_LLI_ELE, *PGDMA_CH_LLI_ELE;
#if 1
#if 0
typedef struct _GDMA_CH_LLI_ {
    PGDMA_CH_LLI_ELE      pLliEle;
    PGDMA_CH_LLI          pNextLli;
}GDMA_CH_LLI, *PGDMA_CH_LLI;

typedef struct _BLOCK_SIZE_LIST_ {
    u32                  BlockSize;
    PBLOCK_SIZE_LIST     pNextBlockSiz;
}BLOCK_SIZE_LIST, *PBLOCK_SIZE_LIST;
#else
struct GDMA_CH_LLI {
    PGDMA_CH_LLI_ELE                pLliEle;
    struct GDMA_CH_LLI             *pNextLli;
};

struct BLOCK_SIZE_LIST {
    u32                             BlockSize;
    struct BLOCK_SIZE_LIST          *pNextBlockSiz;
};

#endif

#endif
typedef struct _HAL_I2S_ADAPTER_ {
    u32                   ChSar;
    u32                   ChDar;
    GDMA_CHANNEL_NUM      ChEn;
    GDMA_CTL_REG          GdmaCtl;
    GDMA_CFG_REG          GdmaCfg;
    u32                   PacketLen;
    u32                   BlockLen;
    u32                   MuliBlockCunt;
    u32                   MaxMuliBlock;
    struct GDMA_CH_LLI          *pLlix;
    struct BLOCK_SIZE_LIST      *pBlockSizeList;

    PGDMA_CH_LLI_ELE            pLli;
    u32                         NextPlli;
    u8                    TestItem;
    u8                          ChNum;
    u8                          GdmaIndex;
    u8                          IsrCtrl:1;
    u8                          GdmaOnOff:1;
    u8                          Llpctrl:1;
    u8                          Lli0:1;
    u8                          Rsvd4to7:4;
    u8                          GdmaIsrType;
}HAL_I2S_ADAPTER, *PHAL_I2S_ADAPTER;

*/

/**********************************************************************/
/* I2S HAL initial data structure */
typedef struct _HAL_I2S_INIT_DAT_ {
    u8                  I2SIdx;         /*I2S index used*/
    u8                  I2SEn;          /*I2S module enable tx/rx/tx+rx*/
    u8                  I2SMaster;      /*I2S Master or Slave mode*/
    u8                  I2SWordLen;     /*I2S Word length 16 or 24bits*/

    u8                  I2SChNum;       /*I2S Channel number mono or stereo*/
    u8                  I2SPageNum;     /*I2S Page Number 2~4*/
    u16                 I2SPageSize;    /*I2S page Size 1~4096 word*/

    u8                  *I2STxData;     /*I2S Tx data pointer*/

    u8                  *I2SRxData;     /*I2S Rx data pointer*/

    u32                 I2STxIntrMSK;   /*I2S Tx Interrupt Mask*/
    u32                 I2STxIntrClr;   /*I2S Tx Interrupt register to clear */

    u32                 I2SRxIntrMSK;   /*I2S Rx Interrupt Mask*/
    u32                 I2SRxIntrClr;   /*I2S Rx Interrupt register to clear*/

	u16                 I2STxIdx;       /*I2S TX page index */
	u16                 I2SRxIdx;       /*I2S RX page index */

	u16                 I2SHWTxIdx;       /*I2S HW TX page index */
	u16                 I2SHWRxIdx;       /*I2S HW RX page index */

	
    u16                 I2SRate;        /*I2S sample rate*/
    u8                  I2STRxAct;      /*I2S tx rx act*/	
}HAL_I2S_INIT_DAT, *PHAL_I2S_INIT_DAT;

/**********************************************************************/
/* I2S Data Structures */
/* I2S Module Selection */
typedef enum _I2S_MODULE_SEL_ {
        I2S0_SEL    =   0x0,
        I2S1_SEL    =   0x1,
}I2S_MODULE_SEL,*PI2S_MODULE_SEL;

typedef struct _HAL_I2S_ADAPTER_ {
    u32                       Enable:1;
    I2S_CTL_REG               I2sCtl;
    I2S_SETTING_REG           I2sSetting;
    u32                       abc;
    u8                        I2sIndex;
}HAL_I2S_ADAPTER, *PHAL_I2S_ADAPTER;

/* I2S HAL Operations */
typedef struct _HAL_I2S_OP_ {
    RTK_STATUS  (*HalI2SInit)       (VOID *Data);
    RTK_STATUS  (*HalI2SDeInit)     (VOID *Data);
    RTK_STATUS  (*HalI2STx)       (VOID *Data, u8 *pBuff);
    RTK_STATUS  (*HalI2SRx)       (VOID *Data, u8 *pBuff);
    RTK_STATUS  (*HalI2SEnable)     (VOID *Data);
    RTK_STATUS  (*HalI2SIntrCtrl)   (VOID *Data);
    u32         (*HalI2SReadReg)    (VOID *Data, u8 I2SReg);
    RTK_STATUS  (*HalI2SSetRate)    (VOID *Data);
    RTK_STATUS  (*HalI2SSetWordLen) (VOID *Data);
    RTK_STATUS  (*HalI2SSetChNum)   (VOID *Data);
    RTK_STATUS  (*HalI2SSetPageNum) (VOID *Data);
    RTK_STATUS  (*HalI2SSetPageSize) (VOID *Data);

    RTK_STATUS  (*HalI2SClrIntr)    (VOID *Data);
    RTK_STATUS  (*HalI2SClrAllIntr) (VOID *Data);
    RTK_STATUS  (*HalI2SDMACtrl)    (VOID *Data); 
/*
    VOID (*HalI2sOnOff)(VOID *Data);
    BOOL (*HalI2sInit)(VOID *Data);
    BOOL (*HalI2sSetting)(VOID *Data);
    BOOL (*HalI2sEn)(VOID *Data);
    BOOL (*HalI2sIsrEnAndDis) (VOID *Data);
    BOOL (*HalI2sDumpReg)(VOID *Data);
    BOOL (*HalI2s)(VOID *Data);
*/
}HAL_I2S_OP, *PHAL_I2S_OP;


VOID HalI2SOpInit(
    IN  VOID *Data
);


#endif

