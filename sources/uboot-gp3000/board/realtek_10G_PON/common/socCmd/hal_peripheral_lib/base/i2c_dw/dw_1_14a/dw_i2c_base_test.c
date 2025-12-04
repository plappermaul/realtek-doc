/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"

#include "rand.h"
#include "section_config.h"
#include "dw_i2c_base_test.h"
#include "rtl_utility.h"
#include "peripheral.h"
#include "common.h"

/* GDMA */
//#include "dw_hal_gdma.h"
//#include "dw_hal_i2c.h"
#if 1
//3 GDMA-------------------------------------
#define DATA_SIZE_I2C   32
#define BLOCK_SIZE_I2C  I2C_VERI_DATA_LEN

typedef struct _GDMA_ADAPTER_ {
    PHAL_GDMA_OP pHalGdmaOp;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
}GDMA_ADAPTER, *PGDMA_ADAPTER;

typedef struct _GDMA_LLP_BUF_ {
    u8  LliTest[BLOCK_SIZE_I2C];
}GDMA_LLP_BUF, *PGDMA_LLP_BUF;

typedef struct _GDMA_VERIFY_PARAMETER_ {
    u8  GdmaAutoTest;
    u8  GdmaMemoryTestType;
    u8  GdmaType;
    u8  GdmaIndex;
    u8  GdamPattern;
    u8  Rsvd[3];
    u32 GdmaTransferLen;
    u32 GdmaVerifyLoop;
    u8  *pSrc;
    u8  *pDst;
    PGDMA_LLP_BUF pSrcLli;
    PGDMA_LLP_BUF pDstLli;
    u8  MaxLlp;
}GDMA_VERIFY_PARAMETER, *PGDMA_VERIFY_PARAMETER;

typedef enum _GDMA_TEST_ITEM_TYPE_ {
    SignalBlock     = 1,
    ConSarAutoDar   = 2,
    AutoSarConDar   = 3,
    AutoSarAutoDar  = 4,
    SignalBlockLlp  = 5,
    ConSarLlpDar    = 6,
    AutoSarLlpDar   = 7,
    LlpSarConDar    = 8,
    LlpSarAutoDar   = 9,
    LlpSarLlpDar    = 10
}GDMA_TEST_ITEM_TYPE, *PGDMA_TEST_ITEM_TYPE;

GDMA_ADAPTER GdmaAdapter;
HAL_GDMA_OP HalGdmaOp_I2C;
HAL_GDMA_ADAPTER HalGdmaAdapter_I2C;

GDMA_ADAPTER GdmaAdapterTx,GdmaAdapterRx;
GDMA_ADAPTER GdmaAdapterTxSlv,GdmaAdapterRxSlv;

//HAL_GDMA_OP HalGdmaOp_I2C;
HAL_GDMA_ADAPTER HalGdmaAdapterTx,HalGdmaAdapterRx;
HAL_GDMA_ADAPTER HalGdmaAdapterTxSlv,HalGdmaAdapterRxSlv; 


//3 GDMA-------------------------------------
#endif

//3 I2C Test Data
//SRAM_BF_DATA_SECTION
HAL_DW_I2C_ADAPTER TestI2CMaster;
//SRAM_BF_DATA_SECTION
HAL_DW_I2C_ADAPTER TestI2CSlave;
//SRAM_BF_DATA_SECTION
HAL_DW_I2C_DAT_ADAPTER TestI2CMasterDat;
//SRAM_BF_DATA_SECTION
HAL_DW_I2C_DAT_ADAPTER TestI2CSlaveDat;
//SRAM_BF_DATA_SECTION
HAL_DW_I2C_OP HalI2COp;

//SRAM_BF_DATA_SECTION
PI2C_VERI_PARA  pI2CVeriAdp;

//SRAM_BF_DATA_SECTION
volatile u32  I2CVeriGCnt;


//4 I2C IRQ handle
//SRAM_BF_DATA_SECTION
IRQ_HANDLE  I2CIrqHandle_Mtr;
//SRAM_BF_DATA_SECTION
IRQ_HANDLE  I2CIrqHandle_Slv;

//4 I2C Verification Data
//SRAM_BF_DATA_SECTION
volatile u8  I2CVeriDatSrc[I2C_VERI_DATA_LEN];
//SRAM_BF_DATA_SECTION
volatile u8  I2CVeriDatDes[I2C_VERI_DATA_LEN];
//u8*  I2CVeriDatDes;

//SRAM_BF_DATA_SECTION
volatile u16 I2CVeriDatCmdSrcMtr[I2C_VERI_DATA_LEN];
//u16* I2CVeriDatCmdSrcMtr; 

//SRAM_BF_DATA_SECTION
volatile u16 I2CVeriDatCmdSrcSlv[I2C_VERI_DATA_LEN];

//SRAM_BF_DATA_SECTION
volatile u32 I2CVeriCntRd;    
//SRAM_BF_DATA_SECTION
volatile u32 SlaveRXDone;

//SRAM_BF_DATA_SECTION
u8  I2CVeriPattern[I2C_VERI_PANT_CNT];// = {0x55,0xAA,0x5A,0xA5,0xFF,0x11};

u32 i2cTest;


#if 1
VOID
I2CGdma0Ch0IrqHandle
(
    IN  VOID        *Data
)
{
    /* DBG_ENTRANCE; */
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8 *pSrc = NULL, *pDst = NULL;
    u8  IsrTypeMap = 0;

    switch (pHalGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case 2: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)(pHalGdmaAdapter->ChSar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
        /* case AutoSarConDar: { */
        case 3: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                }

                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)(pHalGdmaAdapter->ChDar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
            }
            break;
    }

    //3 Clear Pending ISR
    IsrTypeMap = HalGdmaOp_I2C.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalGdmaAdapter->MuliBlockCunt++;
    }
    
}

VOID
I2CGdma0Ch1IrqHandle
(
    IN  VOID        *Data
)
{
    /* DBG_ENTRANCE; */
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8 *pSrc = NULL, *pDst = NULL;
    u8  IsrTypeMap = 0;
    
    switch (pHalGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case 2: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)(pHalGdmaAdapter->ChSar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
        /* case AutoSarConDar: { */
        case 3: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                }

                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)(pHalGdmaAdapter->ChDar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
            }
            break;
    }

    //3 Clear Pending ISR
    IsrTypeMap = HalGdmaOp_I2C.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalGdmaAdapter->MuliBlockCunt++;
    }
    
}

VOID
I2CGdma0Ch4IrqHandle
(
    IN  VOID        *Data
)
{
    /* DBG_ENTRANCE; */
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8 *pSrc = NULL, *pDst = NULL;
    u8  IsrTypeMap = 0;
	u32 rawBlk = 0;
	u8 chNum = 0;
    printf("DMA finish, interrupt is triggered.\n");
	if(i2cTest == I2C_TEST_DTR_INT){
		rawBlk = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_RAW_INT_BASE+8); //RawBlock
		printf("rawBlk=%d\n",rawBlk);
		
		while(1){
			if(rawBlk==1 || rawBlk==0){
				break;
			}
			rawBlk = rawBlk >> 1;
			chNum++;
		}
		if(GdmaAdapterTx.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterTx.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterTx, CH%d\n",chNum);
		}
        else if(GdmaAdapterRxSlv.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterRxSlv.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterRxSlv, CH%d\n",chNum);
		}
		else if(GdmaAdapterRx.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterRx.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterTx, CH%d\n",chNum);
		}
		else if(GdmaAdapterTxSlv.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterTxSlv.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterTx, CH%d\n",chNum);
		}
		else
			printf("find NO GdmaAdapter\n");
	}
		
    switch (pHalGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case 2: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)(pHalGdmaAdapter->ChSar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
        /* case AutoSarConDar: { */
        case 3: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    HalGdmaOp_I2C.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                }

                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)(pHalGdmaAdapter->ChDar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
            }
            break;
    }

    //3 Clear Pending ISR
    IsrTypeMap = HalGdmaOp_I2C.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalGdmaAdapter->MuliBlockCunt++;
    }
    
}
#endif

//3 i2c 1 IRQ handler
VOID
I2C1IrqHandle(
    IN  VOID *Data
){
    PHAL_DW_I2C_ADAPTER pHalI2CAdapter = (PHAL_DW_I2C_ADAPTER)Data;    
    PHAL_DW_I2C_DAT_ADAPTER pI2CAdapterIrq0 = (PHAL_DW_I2C_DAT_ADAPTER)pHalI2CAdapter->pI2CIrqDat;
    
    u8  I2CIdx  = pI2CAdapterIrq0->Idx;

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1))
    {
        printf("I2C%d INTR_GEN_CALL\n",I2CIdx);
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_GEN_CALL;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    }
        
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_DONE(1))
    {
        SlaveRXDone = 1;
        printf("I2C%d INTR_RX_DONE\n",I2CIdx);
        //printf("I2C%d IC_RXFLR:%2x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_RXFLR));
        
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_DONE;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
        
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_ABRT(1))
    {
        printf("!!!I2C%d INTR_TX_ABRT!!!\n",I2CIdx);
        printf("I2C%d IC_TX_ABRT_SOURCE[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_TX_ABRT_SOURCE, 
                                        HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_TX_ABRT_SOURCE));
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_ABRT;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1))
    {          
        //4 Check if the DMA is enabled.
        //4 ->Enabled: to do DMA operation (Enable slave DMA and GDMA channel 1 for TX)
        //4 ->Disabled: to do FIFO write by self
        if (pI2CAdapterIrq0->DMAEn == BIT_CTRL_IC_DMA_CR_TDMAE(1))
        {
            pHalI2CAdapter->pI2CIrqOp->HalI2CEnableDMA(pI2CAdapterIrq0);
        }
        else
        {            
            HalI2CDATWriteDWCommon(pI2CAdapterIrq0);
            pI2CAdapterIrq0->RWDat++;
        }
        
        printf("I2C%d INTR_RD_REQ\n",I2CIdx);
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RD_REQ;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    } 

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_EMPTY(1))
    {
        printf("!!!I2C%d INTR_TX_EMPTY!!!\n",I2CIdx);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_OVER(1))
    {
        printf("!!!I2C%d INTR_TX_OVER!!!\n",I2CIdx);
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_FULL(1))
    {
        while(HAL_I2C_READ32(pI2CAdapterIrq0->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
        {
                I2CVeriDatDes[I2CVeriCntRd] = pHalI2CAdapter->pI2CIrqOp->HalI2CDATRead(pI2CAdapterIrq0);
				//printf("I2CVeriCntRd=%x, data=%x\n",I2CVeriCntRd,I2CVeriDatDes[I2CVeriCntRd]);
                I2CVeriCntRd++;
				
        }
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_OVER(1))
    {   
        printf("I2C%d INTR_RX_OVER\n",I2CIdx);
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_UNDER(1))
    {
       printf("!!!I2C%d INTR_RX_UNDER!!!\n",I2CIdx);
        pI2CAdapterIrq0->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_UNDER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq0);
    }

    
}

//3 i2c 0 IRQ handler
VOID
I2C0IrqHandle(
    IN  VOID *Data
){
    PHAL_DW_I2C_ADAPTER pHalI2CAdapter = (PHAL_DW_I2C_ADAPTER)Data;    
    PHAL_DW_I2C_DAT_ADAPTER pI2CAdapterIrq1 = (PHAL_DW_I2C_DAT_ADAPTER)pHalI2CAdapter->pI2CIrqDat;
    
    u8  I2CIdx  = pI2CAdapterIrq1->Idx; 
    u32 I2CIntrSTS;
    I2CIntrSTS = HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT);
        
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1))
    {
        printf("I2C%d INTR_GEN_CALL\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_GEN_CALL;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_DONE(1))
    {
        printf("I2C%d INTR_RX_DONE\n",I2CIdx);
        printf("I2C%d IC_RXFLR:%2x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_RXFLR));
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_DONE;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_ABRT(1))
    {
        printf("!!!I2C%d INTR_TX_ABRT!!!\n",I2CIdx);
        printf("I2C%d IC_TX_ABRT_SOURCE:%x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_TX_ABRT_SOURCE));
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_ABRT;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }

    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1))
    {   
        printf("I2C%d INTR_RD_REQ\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RD_REQ;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }    

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_FULL(1))
    {   
        //printf("I2C%d INTR_RX_FULL\n",I2CIdx);
		while(HAL_I2C_READ32(pI2CAdapterIrq1->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
        {
                I2CVeriDatDes[I2CVeriCntRd] = pHalI2CAdapter->pI2CIrqOp->HalI2CDATRead(pI2CAdapterIrq1);
                I2CVeriCntRd++;
                printf("I2C read idx=%d\n",I2CVeriCntRd);
        }
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_OVER(1))
    {   
        printf("I2C%d INTR_RX_OVER\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }
	if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_EMPTY(1))
    {
        printf("!!!I2C%d INTR_TX_EMPTY!!!\n",I2CIdx);
		//clear the tx empty imr, or sys will hang due to processing this endless up-coming irq.
		HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_INTR_MASK, 
                                (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK) & ~BIT_IC_INTR_MASK_M_TX_EMPTY));
    }
	if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_OVER(1))
    {
        printf("!!!I2C%d INTR_TX_OVER!!!\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
    }
#if 1
	if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_ACTIVITY(1))
    {
        printf("I2C%d INTR_ACTIVITY\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_ACTIVITY;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
//		HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_INTR_MASK, 
//                                (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK) & ~BIT_IC_INTR_MASK_M_ACTIVITY));
    }
#endif
	if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_START_DET(1))
    {
        printf("I2C%d INTR_START_DET\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_START_DET;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
//		HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_INTR_MASK, 
//                                (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK) & ~BIT_IC_INTR_MASK_M_START_DET));
    }
	if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_STOP_DET(1))
    {
        printf("I2C%d INTR_STOP_DET\n",I2CIdx);
        pI2CAdapterIrq1->IC_Intr_Clr = REG_DW_I2C_IC_CLR_STOP_DET;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq1);
//		HAL_I2C_WRITE32(I2CIdx,REG_DW_I2C_IC_INTR_MASK, 
//                                (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_MASK) & ~BIT_IC_INTR_MASK_M_STOP_DET));
    }
}

//3 i2c 2 IRQ handler
VOID
I2C2IrqHandle(
    IN  VOID *Data
){
    PHAL_DW_I2C_ADAPTER pHalI2CAdapter = (PHAL_DW_I2C_ADAPTER)Data;    
    PHAL_DW_I2C_DAT_ADAPTER pI2CAdapterIrq2 = (PHAL_DW_I2C_DAT_ADAPTER)pHalI2CAdapter->pI2CIrqDat;
    
    u8  I2CIdx  = pI2CAdapterIrq2->Idx;

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1))
    {
        printf("I2C%d INTR_GEN_CALL\n",I2CIdx);
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_GEN_CALL;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    }
        
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_DONE(1))
    {
        SlaveRXDone = 1;
        printf("I2C%d INTR_RX_DONE\n",I2CIdx);
        printf("I2C%d IC_RXFLR:%2x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_RXFLR));
        
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_DONE;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
        
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_ABRT(1))
    {
        printf("!!!I2C%d INTR_TX_ABRT!!!\n",I2CIdx);
        printf("I2C%d IC_TX_ABRT_SOURCE[%2x]: %x\n", I2CIdx, REG_DW_I2C_IC_TX_ABRT_SOURCE, 
                                        HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_TX_ABRT_SOURCE));
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_ABRT;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1))
    {          
        //4 Check if the DMA is enabled.
        //4 ->Enabled: to do DMA operation (Enable slave DMA and GDMA channel 1 for TX)
        //4 ->Disabled: to do FIFO write by self
        if (pI2CAdapterIrq2->DMAEn == BIT_CTRL_IC_DMA_CR_TDMAE(1))
        {
            pHalI2CAdapter->pI2CIrqOp->HalI2CEnableDMA(pI2CAdapterIrq2);
        }
        else
        {            
            HalI2CDATWriteDWCommon(pI2CAdapterIrq2);
            pI2CAdapterIrq2->RWDat++;
        }
        
        printf("I2C%d INTR_RD_REQ\n",I2CIdx);
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RD_REQ;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    } 

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_EMPTY(1))
    {
        printf("!!!I2C%d INTR_TX_EMPTY!!!\n",I2CIdx);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_OVER(1))
    {
        printf("!!!I2C%d INTR_TX_OVER!!!\n",I2CIdx);
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_FULL(1))
    {
        while(HAL_I2C_READ32(pI2CAdapterIrq2->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
        {
                I2CVeriDatDes[I2CVeriCntRd] = pHalI2CAdapter->pI2CIrqOp->HalI2CDATRead(pI2CAdapterIrq2);
                I2CVeriCntRd++;
        }
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_OVER(1))
    {   
        printf("I2C%d INTR_RX_OVER\n",I2CIdx);
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_UNDER(1))
    {
        printf("!!!I2C%d INTR_RX_UNDER!!!\n",I2CIdx);
        pI2CAdapterIrq2->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_UNDER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq2);
    }

    
}

//3 i2c 3 IRQ handler
VOID
I2C3IrqHandle(
    IN  VOID *Data
){
    PHAL_DW_I2C_ADAPTER pHalI2CAdapter = (PHAL_DW_I2C_ADAPTER)Data;    
    PHAL_DW_I2C_DAT_ADAPTER pI2CAdapterIrq3 = (PHAL_DW_I2C_DAT_ADAPTER)pHalI2CAdapter->pI2CIrqDat;
    
    u8  I2CIdx  = pI2CAdapterIrq3->Idx; 
    u32 I2CIntrSTS;
    I2CIntrSTS = HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT);
        
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1))
    {
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_GEN_CALL\n",I2CIdx);
        pI2CAdapterIrq3->IC_Intr_Clr = REG_DW_I2C_IC_CLR_GEN_CALL;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq3);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_DONE(1))
    {
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_RX_DONE\n",I2CIdx);
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d IC_RXFLR:%2x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_RXFLR));
        pI2CAdapterIrq3->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_DONE;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq3);
    }
    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_TX_ABRT(1))
    {
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"!!!I2C%d INTR_TX_ABRT!!!\n",I2CIdx);
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d IC_TX_ABRT_SOURCE:%x\n",I2CIdx,HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_TX_ABRT_SOURCE));
        pI2CAdapterIrq3->IC_Intr_Clr = REG_DW_I2C_IC_CLR_TX_ABRT;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq3);
    }

    
    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1))
    {   
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_RD_REQ\n",I2CIdx);
        pI2CAdapterIrq3->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RD_REQ;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq3);
    }    

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_FULL(1))
    {   
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_RX_FULL\n",I2CIdx);
    }

    if (HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RX_OVER(1))
    {   
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_RX_OVER\n",I2CIdx);
        pI2CAdapterIrq3->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RX_OVER;
        HalI2CClrIntrDWCommon(pI2CAdapterIrq3);
    }
}


VOID
I2CTestMrtWR(
    IN  VOID    *I2CMtr,
    IN  VOID    *I2CSlv,
    IN  VOID    *I2CVeriPara
)
{
    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;
    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;
    
    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
    u32 I2CVeriMtrWr = pI2CVeriPara->MtrRW; //0 for write, 1 for read
    u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
    
    for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
    {
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
        if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
        {
            printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
            printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
        }
#endif
#if !TASK_SCHEDULER_DISABLED
        if (pI2CVeriPara->VeriStop == _TRUE)
        {
            break;
        }
#endif  
        I2CVeriGCnt++;
        printf("    <I2C_TEST_S_NINT Loop %d>\n",I2CVeriLoopCnt);
        RtlMemset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
        RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
        
        I2CVeriPktCnt = Rand() % I2C_VERI_RXBUF_LEN;
        if (!I2CVeriPktCnt)
            I2CVeriPktCnt = 16;

        printf("     I2C_TEST_S_NINT PktCnt = %d \n",I2CVeriPktCnt);               

        //4 to set master read/write
        if (!I2CVeriMtrWr)
            pI2CDatAdapterMtr->RWCmd = 0;//master write
        else
            pI2CDatAdapterMtr->RWCmd = 1;//master read
                  
        for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
        {
            I2CPtn = Rand() % I2C_VERI_PANT_CNT;
            
            if (!I2CVeriMtrWr)//master write
            {                               
                pI2CDatAdapterMtr->RWDat = &I2CVeriPattern[I2CPtn];
                pI2CDatAdapterMtr->DatLen = 1;
                pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);                
            }
            else            //master read
            {                
                pI2CDatAdapterMtr->DatLen = 1;
                pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
            }
            
            I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];            
        }

              
        if (I2CVeriMtrWr)//master read
        {   
            I2CVeriCntRd = 0;
            for (;I2CVeriCntRd<I2CVeriPktCnt;)
            {
                //to deal with transmittion error
                if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
                {
                    break;
                }
                
                if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_RAW_INTR_STAT) & BIT_CTRL_IC_INTR_STAT_R_RD_REQ(1))
                {   
                    pI2CDatAdapterSlv->RWCmd = 0;
                    pI2CDatAdapterSlv->RWDat = (u8 *)&I2CVeriDatSrc[I2CVeriCntRd];
                    pI2CDatAdapterSlv->DatLen = 1;
                    pI2CAdapterSlv->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterSlv);

                    //to clear RD_REQ intr.
                    pI2CDatAdapterSlv->IC_Intr_Clr = REG_DW_I2C_IC_CLR_RD_REQ;
                    pI2CAdapterSlv->pI2CIrqOp->HalI2CClrIntr(pI2CDatAdapterSlv);
                    
                    I2CVeriCntRd++;
                    if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
                        break;                            
                }
            }
        }   

        
        if (!I2CVeriMtrWr)//master write
        {   
            I2CTimeOutCnt = 0;
            I2CVeriCntRd = 0;
            for (;I2CVeriCntRd<I2CVeriPktCnt;)
            {
#if 1            
                //to deal with transmittion error
                if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
                {
                    printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;
                    break;
                }
#endif                
                if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
                {   
                    I2CTimeOutCnt = 0;
                    I2CVeriDatDes[I2CVeriCntRd] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterSlv);//HalI2CDATRead(pI2CDatAdapterMtr);
                    I2CVeriCntRd++;
                    if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
                        break;                            
                }
            }
        }
        else             //master read
        {
            I2CTimeOutCnt = 0;
            I2CVeriCntRd = 0;
            for (;I2CVeriCntRd<I2CVeriPktCnt;)
            {
#if 1            
                //to deal with transmittion error
                if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
                {
                    printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;
                    break;
                }
#endif                
                if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
                {
                    I2CTimeOutCnt = 0;
                    I2CVeriDatDes[I2CVeriCntRd] = pI2CAdapterMtr->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterMtr);//HalI2CDATRead(pI2CDatAdapterMtr);
                    I2CVeriCntRd++;
                    if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
                        break;                            
                }
            }
        }

        printf("     I2C_TEST_S_NINT Result:\n");
        for(I2CVeriCnt=0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
        {            
            if (I2CVeriDatSrc[I2CVeriCnt] != I2CVeriDatDes[I2CVeriCnt])
            {
                printf("Error takes place in Loop %x.\n",I2CVeriGCnt);
                printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
                printf("     **** Compare Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                pI2CVeriPara->VeriStop = _TRUE;
#endif            
                I2CErrFlag = _TRUE;
                break;                
            }
            else
            {
               	printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
            }
        }
        
        printf("     >>>>>Compare Done<<<<<\n\n");

        //4 Leave the verification when compare fails
        if (I2CErrFlag == _TRUE)
            break;
    }
    

}            

#if 1
VOID
I2CTestMrtRD_INTR(
    IN  VOID    *I2CMtr,
    IN  VOID    *I2CSlv,
    IN  VOID    *I2CVeriPara
)
{
    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;
    
    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
    u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
    
    for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
    {
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
        if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
        {
            printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
            printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
        }
#endif    
#if !TASK_SCHEDULER_DISABLED
        if (pI2CVeriPara->VeriStop == _TRUE)
        {
            break;
        }
#endif            

        I2CVeriGCnt++;
        I2CVeriCntRd = 0;
        printf("    <I2C_TEST_S_RINT Loop %d>\n",I2CVeriLoopCnt);
        RtlMemset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
        RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);

        //clear all pending intrs.
        pI2CDatAdapterSlv->IC_INRT_MSK = 0;
        pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
        
        pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
        pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv);        

        if (pI2CVeriPara->MtrRW)
            I2CVeriPktCnt = Rand()%I2C_VERI_RXBUF_LEN;
        else
            I2CVeriPktCnt = Rand()%I2C_VERI_RXBUF_LEN;//I2C_VERI_DATA_LEN;
        
        if (!I2CVeriPktCnt)
            I2CVeriPktCnt = 16;
        
        printf("     I2C_TEST_S_RINT PktCnt = %d \n",I2CVeriPktCnt);               

        //4 to set master read/write
        if (pI2CVeriPara->MtrRW)
        {
            pI2CDatAdapterMtr->RWCmd = 1;//master read

            //set slave transmitter data source pointer
            pI2CDatAdapterSlv->RWDat = (u8 *)&I2CVeriDatSrc[0];
            pI2CDatAdapterSlv->DatLen = I2CVeriPktCnt;             

            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;            
                I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
            }        

            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RD_REQ(1) | BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | BIT_CTRL_IC_INTR_MASK_M_RX_DONE(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);

			//drive master read                            
            pI2CDatAdapterMtr->DatLen = I2CVeriPktCnt;
            pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr); 

            //master read data back
            I2CVeriCnt = 0;
            I2CTimeOutCnt = 0;
            for ( ;I2CVeriCnt<I2CVeriPktCnt; )
            {
                if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
                {            
                    I2CVeriDatDes[I2CVeriCnt] = pI2CAdapterMtr->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterMtr);//HalI2CDATRead(pI2CDatAdapterMtr);
                    I2CVeriCnt++;
                    if (I2CVeriCnt > I2C_VERI_DATA_LEN)
                        break;                            
                }
				//wait for master receive data
				if ((I2CTimeOutCnt++) > (10*I2C_VERI_TIMEOUT_CNT))
                {
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;

                    break;
                }
            }       
        }
        else
        {   

            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_FULL(1) | BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | BIT_CTRL_IC_INTR_MASK_M_RX_DONE(1) 
                                                | BIT_CTRL_IC_INTR_MASK_M_RX_OVER(1);
            
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
            I2CVeriCntRd = 0;

            //drive master write
            pI2CDatAdapterMtr->RWCmd = 0;//master Write
            pI2CDatAdapterMtr->DatLen = 1;
            pI2CDatAdapterMtr->RWDat = (u8 *)&I2CVeriDatSrc[0];

            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;            
                I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
            }

            //create data pattern
            I2CVeriCnt= 0;
            for (; I2CVeriCnt < I2CVeriPktCnt ;)
            {
                if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_TFNF(1))
                {
                    pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
                    pI2CDatAdapterMtr->RWDat++;
                    I2CVeriCnt++;
                }
            }

			//wait for slave receive data
            I2CTimeOutCnt = 0;
            for (;I2CVeriCntRd<I2CVeriPktCnt;)
            {
                if ((I2CTimeOutCnt++) > (10*I2C_VERI_TIMEOUT_CNT))
                {
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;
                    break;
                }
                
            }
        }         

        //compare data
        printf("     Start to Compare Data>>>>>\n");
        printf("     I2C_TEST_S_RINT Result:\n");        
        for(I2CVeriCnt=0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
        {    
            if (I2CVeriDatSrc[I2CVeriCnt] != I2CVeriDatDes[I2CVeriCnt])
            {
                printf("Error takes place in Loop %x.\n",I2CVeriGCnt);
                printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                    I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
                printf("     **** Compare Error ****\n\n");

#if !TASK_SCHEDULER_DISABLED
                pI2CVeriPara->VeriStop = _TRUE;
#endif            
                I2CErrFlag = _TRUE;
                break;
            }
            else
            {
                printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                    I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
            }
        }
        printf("     >>>>>Compare Done<<<<<\n\n");

        //4 Leave the verification when compare fails
        if (I2CErrFlag == _TRUE)
            break;
    }           
}            

VOID
I2CTest_MrtWR_INTERR(IN  VOID    *I2CMtr, IN  VOID    *I2CSlv, IN  VOID    *I2CVeriPara )
{
	PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;

    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
	u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
	
	for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
	{
//		memset((void *)pExe_Master->ExecuteInfo, 0x00, ExecuteInfo_num);
//		memset((void *)pExe_Slave->ExecuteInfo, 0x00, ExecuteInfo_num);
		
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
		if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
		{
			printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
			printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
		}
#endif    
#if !TASK_SCHEDULER_DISABLED
		if (pI2CVeriPara->VeriStop == _TRUE)
		{
			break;
		}
#endif            

		I2CVeriGCnt++;
		I2CVeriCntRd = 0;
		printf("	<I2C_TEST_S_RINT Loop %d>\n",I2CVeriLoopCnt);
		memset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
		memset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);

		//clear all pending intrs.
		pI2CDatAdapterSlv->IC_INRT_MSK = 0;
		pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
		
		pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
		pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv); 	   


		//to test TX_OVER
		I2CVeriPktCnt = Rand()%I2C_VERI_DATA_LEN;
		//I2CVeriPktCnt = I2C_VERI_TXBUF_LEN+5;
		
		
		printf("	 I2C_TEST_S_RINT PktCnt = %d \n",I2CVeriPktCnt);		

		pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_OVER(1)| BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
										 /*BIT_CTRL_IC_INTR_MASK_M_RX_FULL(1)| BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | BIT_CTRL_IC_INTR_MASK_M_RX_DONE(1);*/
		pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);

     
		I2CVeriCntRd = 0;
		
		//drive master write
		pI2CDatAdapterMtr->RWCmd = 0;//master Write
		pI2CDatAdapterMtr->DatLen = 1;
		pI2CDatAdapterMtr->RWDat = I2CVeriDatSrc;
		
		//create data pattern
		for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
		{
			I2CPtn = Rand()%I2C_VERI_PANT_CNT;			  
			I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
		}
     
		//send data
		I2CVeriCnt= 0;
		for (; I2CVeriCnt < I2CVeriPktCnt ;)
		{
			pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
			pI2CDatAdapterMtr->RWDat++;
			I2CVeriCnt++;
			
		}
     
		// wait for all data transfered.
		while(1) {
			if (HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_TFE(1)) break;
			 //HalDelayUs(100);
			 udelay(100);
		}

		     
		I2CTimeOutCnt = 0;
		I2CVeriCntRd = 0;
		for (;I2CVeriCntRd<I2C_VERI_RXBUF_LEN;) //due to RX_OVER, only I2C_VERI_RXBUF_LEN data is received
		{
			if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
			{	
				I2CTimeOutCnt = 0;
				I2CVeriDatDes[I2CVeriCntRd] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterSlv);
				I2CVeriCntRd++;
				if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
					break;							  
			}
			          
			//to deal with transmittion error
			if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
			{
				printf("	  **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
				pI2CVeriPara->VeriStop = _TRUE;
#endif            
				//I2CErrFlag = _TRUE; //because we are testing error, we need test go on
				break;
			}
		}

		
		//to generate RX_UNDER, read one more time
		//HAL_I2C_READ32(I2CIdx,REG_DW_I2C_IC_DATA_CMD);
		pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterSlv);

		//compare data
		printf("	 Start to Compare Data>>>>>\n");
		printf("	 I2CTest_MrtWR_INTERR Result:\n",I2CVeriPktCnt); 	   
		for(I2CVeriCnt=0; I2CVeriCnt < I2C_VERI_RXBUF_LEN; I2CVeriCnt++) //
		{	 
			if (I2CVeriDatSrc[I2CVeriCnt] != I2CVeriDatDes[I2CVeriCnt])
			{
				printf("Error takes place in Loop %x.\n",I2CVeriGCnt);
				printf("			  Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
									I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
				printf("	  **** Compare Error ****\n\n");

#if !TASK_SCHEDULER_DISABLED
				pI2CVeriPara->VeriStop = _TRUE;
#endif            
				//I2CErrFlag = _TRUE; //because we are testing error, we need test go on
				break;
			}
			else
			{
				printf("			 Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
									I2CVeriDatSrc[I2CVeriCnt], I2CVeriDatDes[I2CVeriCnt]);
			}
		}
		
//		if (I2CErrFlag == _FALSE)
//			pExe_Slave->ExecuteInfo[COMPARE_GOOD]++;

#ifdef EXE_RESULT
		u8 ExecuteInfo_i=0;
		for(;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Master ExecuteInfo[%d]=%d  ",pExe_Master->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
		for(ExecuteInfo_i=0;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Slave ExecuteInfo[%d]=%d  ",pExe_Slave->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
#endif



		printf("	 >>>>>Compare Done<<<<<\n\n");
//
//		if(CheckSuccessCondition_INT_ERR())
//			printf("[SUCCESS]I2C VERI Item: INT_ERR\n");
//		else
//			printf("[FAIL]I2C VERI Item: INT_ERR\n");
//	
		//4 Leave the verification when compare fails
		if (I2CErrFlag == _TRUE)
			break;

	}
}
VOID
I2CTest_MrtWR_INT_GenCall(IN  VOID    *I2CMtr,IN  VOID    *I2CSlv, IN  VOID    *I2CVeriPara )
{
	PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA)I2CVeriPara;

    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
	u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
	
	for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
	{
//		memset((void *)pExe_Master->ExecuteInfo, 0x00, ExecuteInfo_num);
//		memset((void *)pExe_Slave->ExecuteInfo, 0x00, ExecuteInfo_num);
		
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
		if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
		{
			printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
			printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
		}
#endif    
#if !TASK_SCHEDULER_DISABLED
		if (pI2CVeriPara->VeriStop == _TRUE)
		{
			break;
		}
#endif            

		I2CVeriGCnt++;
		I2CVeriCntRd = 0;
		printf("	<I2C_TEST_S_RINT Loop %d>\n",I2CVeriLoopCnt);
		memset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
		memset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);

		//clear all pending intrs.
		pI2CDatAdapterSlv->IC_INRT_MSK = 0;
		pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
		
		pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
		pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv); 	   

		I2CVeriPktCnt=1; // one general call

		printf("	 I2C_TEST_S_RINT PktCnt = %d \n",I2CVeriPktCnt);		

		pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_GEN_CALL(1) |
										BIT_CTRL_IC_INTR_MASK_M_RX_OVER(1)| 
										BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
		
		pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
		I2CVeriCntRd = 0;
		
		//drive master write
		pI2CDatAdapterMtr->RWCmd = 0;//master Write
		pI2CDatAdapterMtr->DatLen = 1;
		pI2CDatAdapterMtr->RWDat = I2CVeriDatSrc;
		
		//create data pattern
		I2CVeriDatSrc[0] = I2CVeriPattern[0];
		
	
		//create data pattern
		I2CVeriCnt= 0;
		
		pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
		pI2CDatAdapterMtr->RWDat++;
		

		while(1) 
		{
			if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))break;
			//HalDelayUs(100);
			udelay(100);
		}
        
#if 0           
        I2CVeriDatDes[0] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterMtr);
             


		//compare data
		printf("	 Start to Compare Data>>>>>\n");
		printf("	 I2CTest_MrtWR_INT_GenCall Result:\n"); 	   

		
			if (I2CVeriDatSrc[0] != I2CVeriDatDes[0])
			{
				printf("Error takes place in Loop %x.\n",I2CVeriGCnt);
				printf("			  Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
									I2CVeriDatSrc[0], I2CVeriDatDes[0]);
				printf("	  **** Compare Error ****\n\n");

#if !TASK_SCHEDULER_DISABLED
				pI2CVeriPara->VeriStop = _TRUE;
#endif            
				I2CErrFlag = _TRUE;
				break;
			}
			else
			{
				printf("			 Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
									I2CVeriDatSrc[0], I2CVeriDatDes[0]);
			}
		

//		if(I2CErrFlag == _FALSE)
//			pExe_Slave->ExecuteInfo[COMPARE_GOOD]++;
		
#ifdef EXE_RESULT
		u8 ExecuteInfo_i=0;

		for(;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Master ExecuteInfo[%d]=%d  ",pExe_Master->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
		for(ExecuteInfo_i=0;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Slave ExecuteInfo[%d]=%d  ",pExe_Slave->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
#endif

		printf("	 >>>>>Compare Done<<<<<\n\n");

//		if(CheckSuccessCondition_INT_GenCall())
//			printf("[SUCCESS]I2C VERI Item: INT_GEN_CALL\n");
//		else
//			printf("[FAIL]I2C VERI Item: INT_GEN_CALL\n");

		//4 Leave the verification when compare fails
		if (I2CErrFlag == _TRUE)
			break;
#endif
	}
}

VOID
I2CTest_MrtWR_INT_TXABRT(IN  VOID    *I2CMtr,IN  VOID    *I2CSlv,  IN  VOID    *I2CVeriPara)
{
	PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;

    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
	u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
	
	for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
	{
//		memset((void *)pExe_Master->ExecuteInfo, 0x00, ExecuteInfo_num);
//		memset((void *)pExe_Slave->ExecuteInfo, 0x00, ExecuteInfo_num);
		
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
		if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
		{
			printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
			printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
		}
#endif    
#if !TASK_SCHEDULER_DISABLED
		if (pI2CVeriPara->VeriStop == _TRUE)
		{
			break;
		}
#endif            

		I2CVeriGCnt++;
		I2CVeriCntRd = 0;
		printf("	<I2C_TEST_S_RINT Loop %d>\n",I2CVeriLoopCnt);
		memset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
		memset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
 

		I2CVeriPktCnt=1; // one general call

		printf("	 I2C_TEST_S_RINT PktCnt = %d \n",I2CVeriPktCnt);		

		pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1)	| BIT_CTRL_IC_INTR_MASK_M_GEN_CALL(1);
		
		pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
		I2CVeriCntRd = 0;
		
		//drive master write
		pI2CDatAdapterMtr->RWCmd = 0;//master Write
		pI2CDatAdapterMtr->DatLen = 1;
		pI2CDatAdapterMtr->RWDat = I2CVeriDatSrc;
		
		//create data pattern
		I2CVeriDatSrc[0] = I2CVeriPattern[0];
		
	
		//create data pattern
		I2CVeriCnt= 0;
		
		pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);
		pI2CDatAdapterMtr->RWDat++;


		//HalDelayUs(1000);
		udelay(100);

		//no data will be in RX FIFO
        //pI2CVeriDatDes[0] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterMtr);
             
		//to gen TX_ABRT
		//pI2CAdapterMtr->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterMtr);


		udelay(100);

		//compare data
		printf("	 Start to Compare Data>>>>>\n");
		printf("	 I2CTest_MrtWR_INT_TXABRT Result:\n"); 	   
		//no data is transfered.		

#ifdef EXE_RESULT
		u8 ExecuteInfo_i=0;

		for(;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Master ExecuteInfo[%d]=%d  ",pExe_Master->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
		for(ExecuteInfo_i=0;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
			printf("Slave ExecuteInfo[%d]=%d  ",pExe_Slave->ExecuteInfo[ExecuteInfo_i]);
		}
		printf("\n");
#endif


//		if(CheckSuccessCondition_INT_TXABRT())
//			printf("[SUCCESS]I2C VERI Item: INT_TXABRT\n");
//		else
//			printf("[FAIL]I2C VERI Item: INT_TXABRT\n");

		printf("	 >>>>>Compare Done<<<<<\n\n");

		//4 Leave the verification when compare fails
		if (I2CErrFlag == _TRUE)
			break;

	}
}


VOID
I2CTestTxRxDMA(
    IN  VOID    *I2CMtr,
    IN  VOID    *I2CSlv,
    IN  VOID    *I2CVeriPara
){

    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

	//I2CVeriDatCmdSrcMtr = (u16*)0xBFE00000; //SRAM address
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;

    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
    u32 I2CVeriMtrWr = pI2CVeriPara->MtrRW; //0 for write, 1 for read
    u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
    
    //4 GDMA>>>>>>
    PGDMA_ADAPTER     pGdmaAdapter;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP      pHalGdmaOp;
    IRQ_HANDLE        Gdma4IrqHandle;
    u32 GdmaTransferLen;
    u8  GdmaIndex, /*GdmaType,*/ MaxLlp;
    u8  *pSrc=NULL, *pDst=NULL;

    GdmaIndex = 0;
    MaxLlp = 1;

    GdmaTransferLen = I2C_VERI_DATA_LEN;
    pGdmaAdapter = &GdmaAdapter;
    pGdmaAdapter->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter_I2C;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter_I2C;
    pGdmaAdapter->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;
    pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;

    if (!I2CVeriMtrWr)       //master write
    {
        if (pI2CDatAdapterMtr->Idx==I2C0_SEL)
        {
        	//physical address
            
#if 0
            pSrc = (u8 *)((u32)I2CVeriDatCmdSrcMtr & ~0x80000000);//I2CVeriDatCmdSrcMtr;
            pDst = (u8 *)(I2C0_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD; //(I2C0_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
#else
            pSrc = (u8 *)(I2CVeriDatCmdSrcMtr);
            pDst = (u8 *)(I2C0_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
            
#endif
            
        }
        else
        {
        	//physical address
            #if 0
            pSrc = (u8 *)((u32)I2CVeriDatCmdSrcMtr & ~0x80000000);//I2CVeriDatCmdSrcMtr;
            pDst = (u8 *)(I2C1_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD; //(I2C1_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
            #else
            pSrc = (u8 *)(I2CVeriDatCmdSrcMtr);
            pDst = (u8 *)(I2C1_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
            #endif
        }
        
    }
    else                    //master read
    {
        if (pI2CDatAdapterMtr->Idx==I2C0_SEL)
        {
        	//physical address
            
#if 0
            pSrc = (u8 *)(I2C0_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;//(I2C0_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
            pDst = (u8 *)((u32)I2CVeriDatDes & ~0x80000000);//I2CVeriDatDes;   
#else
            pSrc = (u8 *)(I2C0_REG_BASE+REG_DW_I2C_IC_DATA_CMD);//(I2C0_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
            pDst = (u8 *)(I2CVeriDatDes);//I2CVeriDatDes;   
#endif
            
        }
        else
        {
        	//physical address
            
#if 0
            pSrc = (u8 *)(I2C1_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD; //(I2C1_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
            pDst = (u8 *)((u32)I2CVeriDatDes & ~0x80000000);//I2CVeriDatDes;   
#else
            pSrc = (u8 *)(I2C1_REG_BASE+REG_DW_I2C_IC_DATA_CMD); //(I2C1_REG_BASE + REG_DW_I2C_IC_DATA_CMD);
            pDst = (u8 *)(I2CVeriDatDes);//I2CVeriDatDes;   
#endif
            
        }
    }
    

    /* Initial GDMA HAL Operation */
    HalGdmaOpInit((VOID*) (pHalGdmaOp));
    
    if(!I2CVeriMtrWr)
    {
        RtlMemset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));
        pHalGdmaAdapter->GdmaCtl.IntEn = 1;

        //I2C TX DMA
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
       // pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes; //tingchu
        pHalGdmaAdapter->GdmaCtl.SrcMsize = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DestMsize = MsizeEight;   //tingchu
        //pHalGdmaAdapter->GdmaCtl.DestMsize = MsizeFour;//MsizeEight;   //tingchu

        pHalGdmaAdapter->GdmaCtl.Dinc = NoChange;
    	pHalGdmaAdapter->GdmaCtl.Sinc = IncType;

        pHalGdmaAdapter->TestItem = 2;
    }
    else
    {
        RtlMemset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));
        pHalGdmaAdapter->GdmaCtl.IntEn = 1;

        //I2C RX DMA
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        pHalGdmaAdapter->GdmaCtl.SrcMsize = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DestMsize = MsizeOne;

        pHalGdmaAdapter->GdmaCtl.Dinc = IncType;
    	pHalGdmaAdapter->GdmaCtl.Sinc = NoChange;

        pHalGdmaAdapter->TestItem = 3;
    }
    
#if 1
    Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
#if defined(_HAL_IRQN_H_96F) 
    Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
#elif defined(_HAL_IRQN_H_98F)
    Gdma4IrqHandle.IrqNum = DW_ahb_dmac_top;
#endif
    Gdma4IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch4IrqHandle;
    Gdma4IrqHandle.Priority = 0;
    InterruptRegister(&Gdma4IrqHandle);
    InterruptEn(&Gdma4IrqHandle);
    //irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data); //98F arm

    
#else
    Gdma0IrqHandle.Data = (u32) (pGdmaAdapter);
    Gdma0IrqHandle.IrqNum = GDMA0_CHANNEL0_IRQ;
    Gdma0IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch0IrqHandle;
    Gdma0IrqHandle.Priority = 0;
    InterruptRegister(&Gdma0IrqHandle);
    InterruptEn(&Gdma0IrqHandle);
#endif

    /* Rx DMA settings */        
    pHalGdmaAdapter->GdmaCtl.Done = 1;
    if (!I2CVeriMtrWr)
    {
        pHalGdmaAdapter->GdmaCtl.TtFc = 1;
        pHalGdmaAdapter->GdmaCfg.DestPer = 8;
        //pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;  //<----
    }
    else
    {
        pHalGdmaAdapter->GdmaCtl.TtFc = 2;
        pHalGdmaAdapter->GdmaCfg.SrcPer  = 9;
        pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;
    }

    pHalGdmaAdapter->MuliBlockCunt = 0;
    pHalGdmaAdapter->MaxMuliBlock = MaxLlp;
    pHalGdmaAdapter->ChSar = (u32)pSrc;
    pHalGdmaAdapter->ChDar = (u32)pDst;
    pHalGdmaAdapter->ChNum = 1;//3;
    pHalGdmaAdapter->GdmaIndex = GdmaIndex;
    pHalGdmaAdapter->ChEn = GdmaCh1;
    pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
    pHalGdmaAdapter->IsrCtrl = ENABLE;
    pHalGdmaAdapter->GdmaOnOff = ON;
    pHalGdmaAdapter->PacketLen = DATA_SIZE_I2C;
    pHalGdmaAdapter->BlockLen = BLOCK_SIZE_I2C;

    //To start test loops
    for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
    {
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
        if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
        {
            printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
            printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
        }
#endif 
#if !TASK_SCHEDULER_DISABLED
        if (pI2CVeriPara->VeriStop == _TRUE)
        {
            break;
        }
#endif            

        I2CVeriGCnt++;
        printf("    <I2C_TEST_D_INT Loop %d>\n",I2CVeriLoopCnt);
        //To generate packet count
        if (!I2CVeriMtrWr)
        {
            I2CVeriPktCnt = Rand()%I2C_VERI_DATA_LEN; //-->fixed to 16 to avoid rx over
#if 1	//test small data first
            if (I2CVeriPktCnt < 128)
            {
                I2CVeriPktCnt += 128;
            }
            else if (I2CVeriPktCnt < 256)
            {
                I2CVeriPktCnt += (Rand()%256);
            }
            else if (I2CVeriPktCnt < 512)
            {
                I2CVeriPktCnt += (Rand()%512);
            }
#endif
        }
        else
        {
            I2CVeriPktCnt = Rand()%I2C_VERI_TXBUF_LEN;
        }
        
        
        if (!I2CVeriPktCnt)
            I2CVeriPktCnt = 16;

        I2CVeriPktCnt = 4;
        if (!I2CVeriMtrWr)
        {
            if (I2CVeriPktCnt&0x01)
                //I2CVeriPktCnt++;  tingchu masked
            
            pHalGdmaAdapter->GdmaCtl.BlockSize = I2CVeriPktCnt/2;
            pHalGdmaAdapter->GdmaCtl.BlockSize = I2CVeriPktCnt;
        }
        else
        {
            pHalGdmaAdapter->GdmaCtl.BlockSize = I2CVeriPktCnt;
        }
     
        pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
        pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
        pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));       
		printf("DMA CH3 CFG=%x\n",HAL_READ32(GDMA0_REG_BASE, 0x14c));
    //4 GDMA<<<<<<

        //3 I2C TXRX>>>>
        //clear all pending intrs.
        pI2CDatAdapterSlv->IC_INRT_MSK = 0;
        pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
        
        pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
        pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv);          
        
        printf("     I2C_TEST_D_INT PktCnt = %d \n",I2CVeriPktCnt);               

        //4 to set master read/write
        if (!I2CVeriMtrWr)          //master write
        {            
            RtlMemset((void *)I2CVeriDatCmdSrcMtr, 0x0000, I2C_VERI_DATA_LEN);
            RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
            I2CVeriCntRd = 0;
            pI2CDatAdapterMtr->RWCmd = 0;

            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;
                I2CVeriDatCmdSrcMtr[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
                
                if (I2CVeriCnt == (I2CVeriPktCnt-1))
                    I2CVeriDatCmdSrcMtr[I2CVeriCnt] |= BIT_CTRL_IC_DATA_CMD_STOP(1);
            }

            //enable I2C RX intr.        
            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_OVER(1) | BIT_CTRL_IC_INTR_MASK_M_RX_FULL(1) | BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1) |
                                                BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
			
            //*****flush data before DMA******
			flush_cache(&I2CVeriDatCmdSrcMtr[0], I2C_VERI_DATA_LEN*sizeof(u16));
			printf("I2CVeriDatCmdSrcMtr=%x\n",I2CVeriDatCmdSrcMtr);

            
			pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr); 
            
			printf("i2c DMA enable=%x\n",HAL_I2C_READ32(0,REG_DW_I2C_IC_DMA_CR));
            
        }
        else                        //master read
        {
            SlaveRXDone = 0;
            I2CVeriCntRd = 0;
			RtlMemset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);
            RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
			
			//*****invalidate cache before DMA******
			u32 start = (u32)I2CVeriDatDes & ~(ARCH_DMA_MINALIGN - 1);	
			u32 end = ALIGN(start + I2C_VERI_DATA_LEN, ARCH_DMA_MINALIGN);
			invalidate_dcache_range(start,end);
			
            //enable GDMA channel.
            pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));
			

            //enable I2C DMA
            if (pI2CVeriPara->MtrRW)
            {
                pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_RDMAE(1);
                pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr);
            }
            
            
            pI2CDatAdapterMtr->RWCmd = 1;
            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;            
                I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];//I2CVeriCnt&0x00FF;
            }
			
            //set slave transmitter data source pointer
            pI2CDatAdapterSlv->RWDat = (u8 *)&I2CVeriDatSrc[0];
            pI2CDatAdapterSlv->DatLen = I2CVeriPktCnt;        
            //enable I2C RX intr.        
            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RD_REQ(1) | BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | BIT_CTRL_IC_INTR_MASK_M_RX_DONE(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
            //master read cmds                           
            pI2CDatAdapterMtr->DatLen = I2CVeriPktCnt;
			pI2CDatAdapterMtr->RWDat = (u8 *)&I2CVeriDatSrc[0]; //just to avoid null pointer
            pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);       
			

            
        }
        
        if (!I2CVeriMtrWr)// master write
        {
            I2CTimeOutCnt = 0;
            while (I2CVeriCntRd<I2CVeriPktCnt)
            {
                if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
                {
                    printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;
                    break;
                }
            }
        }
        else
        {
            //wait for slave RX Done       
            for (;SlaveRXDone==0;){
                if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
                    {
                        printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                        pI2CVeriPara->VeriStop = _TRUE;
#endif            
                        I2CErrFlag = _TRUE;
                        break;
                    }
            }
                
                
        }
        
        //compare data
        printf("     Start to Compare Data>>>>>\n");
        printf("     I2C_TEST_D_INT Result:\n",I2CVeriPktCnt);
        for(I2CVeriCnt=0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
        {            
            if (!I2CVeriMtrWr)
            {                
                if ((I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0x00FF) != I2CVeriDatDes[I2CVeriCnt])
                {
                    I2CErrFlag = _TRUE;
                    printf("             Pkt%2d(S,D): %4x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
                    printf("     **** Compare Error ****\n\n");
                    //break;
                }
				else{
					
					printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
				}
            }
            else
            {
                
                if (I2CVeriDatSrc[I2CVeriCnt] != *(I2CVeriDatDes+I2CVeriCnt))
                {
                    I2CErrFlag = _TRUE;
                    printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                                I2CVeriDatSrc[I2CVeriCnt], *(I2CVeriDatDes+I2CVeriCnt));
                    printf("     **** Compare Error ****\n\n");
                    break;
                }
                else
                {
                    printf("             Pkt%2d(S,D): %2x, %2x\n",I2CVeriCnt, 
                                        I2CVeriDatSrc[I2CVeriCnt], *(I2CVeriDatDes+I2CVeriCnt));
                }
            }
        }

        printf("     >>>>>Compare Done<<<<<\n\n");
        if (!I2CVeriMtrWr)
        {
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CDisableDMA(pI2CDatAdapterMtr);
        }
        else
        {
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_RDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CDisableDMA(pI2CDatAdapterMtr);
        }
        
        //disable all intr first
        pI2CDatAdapterMtr->IC_INRT_MSK = 0;
        pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr);
        pI2CDatAdapterSlv->IC_INRT_MSK = 0x0;
        pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
        //3 I2C TXRX<<<<     
        
        //4 GDMA>>>>>>
        pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
        //4 GDMA<<<<<<

        //4 Leave the verification when compare fails
        if (I2CErrFlag == _TRUE)
            break;
    }  
}


//=====================================================//
//Function:
//              I2C0: I2C Master
//              I2C3: I2C Slave
//              GDMA0, CH0: For TX DMA
//              GDMA0, CH4: For RX DMA
//=====================================================//
VOID
I2CTestTRxDMA(
    IN  VOID    *I2CMtr,
    IN  VOID    *I2CSlv,
    IN  VOID    *I2CVeriPara
){

    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;

    u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
    u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
    u32 I2CVeriMtrWr = pI2CVeriPara->MtrRW; //0 for write, 1 for read  tingchu 20160907
    
    PGDMA_ADAPTER     pGdmaAdapterTx,pGdmaAdapterRx;
    PGDMA_ADAPTER     pGdmaAdapterTxSlv;//for master read
    PGDMA_ADAPTER     pGdmaAdapterRxSlv;//for master Write tingchu20160907
    PHAL_GDMA_ADAPTER pHalGdmaAdapterTx,pHalGdmaAdapterRx;
    PHAL_GDMA_ADAPTER pHalGdmaAdapterTxSlv;//for master read
    PHAL_GDMA_ADAPTER pHalGdmaAdapterRxSlv;//for master Write tingchu 
    PHAL_GDMA_OP      pHalGdmaOpTx,pHalGdmaOpRx;
    PHAL_GDMA_OP      pHalGdmaOpTxSlv;//for master read
    PHAL_GDMA_OP      pHalGdmaOpRxSlv;//for master Write tingchu
    IRQ_HANDLE        Gdma4IrqHandle;
    IRQ_HANDLE        Gdma0IrqHandle;
    IRQ_HANDLE        Gdma1IrqHandle;//for master read
    IRQ_HANDLE        Gdma2IrqHandle;//for master Write tingchu  
    
    u32 GdmaTransferLen;
    u8  GdmaIndex, /*GdmaType,*/ MaxLlp;
    u8  *pSrcTx=NULL, *pDstTx=NULL,*pSrcRx=NULL, *pDstRx=NULL;
    u8  *pSrcTxSlv=NULL, *pDstTxSlv=NULL;//for master read
    u8  *pSrcRxSlv=NULL, *pDstRxSlv=NULL;//for master write tingchu
    GdmaIndex = 0;
    MaxLlp = 1;
    GdmaTransferLen = I2C_VERI_DATA_LEN;
    
    //4 Master Tx DMA Setting(1)
    pGdmaAdapterTx= &GdmaAdapterTx;
    pGdmaAdapterTx->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterTx;
    pHalGdmaAdapterTx= (PHAL_GDMA_ADAPTER) &HalGdmaAdapterTx;
    pGdmaAdapterTx->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;
    pHalGdmaOpTx= (PHAL_GDMA_OP) &HalGdmaOp_I2C;
    /* DMA Address Settings */
    if (pI2CDatAdapterMtr->Idx == I2C0_SEL)
    {
    #if 0
        pSrcTx = (u8 *)((u32)I2CVeriDatCmdSrcMtr & ~0x80000000);//&I2CVeriDatCmdSrcMtr[0];
        pDstTx = (u8 *)(I2C0_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
    #else
        pSrcTx = (u8 *)(I2CVeriDatCmdSrcMtr);//&I2CVeriDatCmdSrcMtr[0];
        pDstTx = (u8 *)(I2C0_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
    #endif
    }
    else
    {
    #if 0
        pSrcTx = (u8 *)((u32)I2CVeriDatCmdSrcMtr & ~0x80000000);//&I2CVeriDatCmdSrcMtr[0];
        pDstTx = (u8 *)(I2C1_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
    #else
        pSrcTx = (u8 *)((u32)I2CVeriDatCmdSrcMtr);//&I2CVeriDatCmdSrcMtr[0];
        pDstTx = (u8 *)(I2C1_REG_BASE+REG_DW_I2C_IC_DATA_CMD);
    #endif
    }
    
    if (!I2CVeriMtrWr){
        //4 Slave Rx DMA Setting(1) for master read  tingchu
        pGdmaAdapterRxSlv= &GdmaAdapterRxSlv;
        pGdmaAdapterRxSlv->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterRxSlv;
        pHalGdmaAdapterRxSlv= (PHAL_GDMA_ADAPTER) &HalGdmaAdapterRxSlv;
        pGdmaAdapterRxSlv->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        pHalGdmaOpRxSlv= (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        /* DMA Address Settings */
        if (pI2CDatAdapterSlv->Idx == I2C0_SEL)
        {
            pSrcRxSlv = (u8 *)(I2C0_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
            pDstRxSlv = (u8 *)((u32)I2CVeriDatDes);//&I2CVeriDatDes[0];
        }
        else
        {
            pSrcRxSlv = (u8 *)(I2C1_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
            pDstRxSlv = (u8 *)((u32)I2CVeriDatDes);//&I2CVeriDatDes[0];
        }
    }
    else{
        //4 Master Rx DMA Setting(1)
        pGdmaAdapterRx = &GdmaAdapterRx;
        pGdmaAdapterRx->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterRx;
        pHalGdmaAdapterRx= (PHAL_GDMA_ADAPTER) &HalGdmaAdapterRx;
        pGdmaAdapterRx->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        pHalGdmaOpRx= (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        /* DMA Address Settings */   
        if (pI2CDatAdapterMtr->Idx == I2C0_SEL)
        {
        #if 0
            pSrcRx = (u8 *)(I2C0_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
            pDstRx = (u8 *)((u32)I2CVeriDatDes & ~0x80000000);//&I2CVeriDatDes[0];
        #else
            pSrcRx = (u8 *)(I2C0_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
            pDstRx = (u8 *)(I2CVeriDatDes);//&I2CVeriDatDes[0];
        #endif
        }
        else
        {
        #if 0
            pSrcRx = (u8 *)(I2C1_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
            pDstRx = (u8 *)((u32)I2CVeriDatDes & ~0x80000000);//&I2CVeriDatDes[0];        
        #else
            pSrcRx = (u8 *)(I2C1_REG_BASE+REG_DW_I2C_IC_DATA_CMD);
            pDstRx = (u8 *)(I2CVeriDatDes);//&I2CVeriDatDes[0];        
        #endif
        }
        
        
        //4 Slave Tx DMA Setting(1) for master read
        pGdmaAdapterTxSlv= &GdmaAdapterTxSlv;
        pGdmaAdapterTxSlv->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterTxSlv;
        pHalGdmaAdapterTxSlv= (PHAL_GDMA_ADAPTER) &HalGdmaAdapterTxSlv;
        pGdmaAdapterTxSlv->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        pHalGdmaOpTxSlv= (PHAL_GDMA_OP) &HalGdmaOp_I2C;
        /* DMA Address Settings */
        if (pI2CDatAdapterSlv->Idx == I2C0_SEL)
        {
        #if 0
            pSrcTxSlv = (u8 *)((u32)I2CVeriDatCmdSrcSlv & ~0x80000000);//&I2CVeriDatCmdSrcSlv[0];
            pDstTxSlv = (u8 *)(I2C0_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
        #else
            pSrcTxSlv = (u8 *)((u32)I2CVeriDatCmdSrcSlv);//&I2CVeriDatCmdSrcSlv[0];
            pDstTxSlv = (u8 *)(I2C0_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
        #endif
        }
        else
        {
        #if 0
            pSrcTxSlv = (u8 *)((u32)I2CVeriDatCmdSrcSlv & ~0x80000000);//&I2CVeriDatCmdSrcSlv[0];
            pDstTxSlv = (u8 *)(I2C1_REG_BASE & ~0xA0000000)+REG_DW_I2C_IC_DATA_CMD;
        #else
            pSrcTxSlv = (u8 *)((u32)I2CVeriDatCmdSrcSlv);//&I2CVeriDatCmdSrcSlv[0];
            pDstTxSlv = (u8 *)(I2C1_REG_BASE +REG_DW_I2C_IC_DATA_CMD);
        #endif
        }
    }

    //4 Initial GDMA HAL Operation

    if (!I2CVeriMtrWr){
        HalGdmaOpInit((VOID*) (pHalGdmaOpTx));
        HalGdmaOpInit((VOID*) (pHalGdmaOpRxSlv));
    }
    else{
        HalGdmaOpInit((VOID*) (pHalGdmaOpTx));
        HalGdmaOpInit((VOID*) (pHalGdmaOpRx));    
        HalGdmaOpInit((VOID*) (pHalGdmaOpTxSlv));
    }
    

    
    
    
    //4 TX DMA Setting(2)
    RtlMemset((void *)pHalGdmaAdapterTx, 0, sizeof(HAL_GDMA_ADAPTER));
    pHalGdmaAdapterTx->GdmaCtl.IntEn = 1;
    pHalGdmaAdapterTx->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
    pHalGdmaAdapterTx->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
    //pHalGdmaAdapterTx->GdmaCtl.DstTrWidth = TrWidthFourBytes;  //tingchu
    pHalGdmaAdapterTx->GdmaCtl.SrcMsize = MsizeFour;
    pHalGdmaAdapterTx->GdmaCtl.DestMsize = MsizeEight;//MsizeEight;
    //pHalGdmaAdapterTx->GdmaCtl.DestMsize = MsizeFour;//MsizeEight; //tingchu
    pHalGdmaAdapterTx->GdmaCtl.Sinc = IncType;
    pHalGdmaAdapterTx->GdmaCtl.Dinc = NoChange;	
    pHalGdmaAdapterTx->TestItem = 2;

    if (!I2CVeriMtrWr){
        //4 Slv RX DMA Setting(2) for master write tingchu
        RtlMemset((void *)pHalGdmaAdapterRxSlv, 0, sizeof(HAL_GDMA_ADAPTER));
        pHalGdmaAdapterRxSlv->GdmaCtl.IntEn = 1;
        pHalGdmaAdapterRxSlv->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapterRxSlv->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        pHalGdmaAdapterRxSlv->GdmaCtl.SrcMsize = MsizeFour;
        pHalGdmaAdapterRxSlv->GdmaCtl.DestMsize = MsizeOne;
        pHalGdmaAdapterRxSlv->GdmaCtl.Sinc = NoChange;
        pHalGdmaAdapterRxSlv->GdmaCtl.Dinc = IncType;
        pHalGdmaAdapterRxSlv->TestItem = 3;
    }
    else{
        //4 RX DMA Setting(2)
        RtlMemset((void *)pHalGdmaAdapterRx, 0, sizeof(HAL_GDMA_ADAPTER));
        pHalGdmaAdapterRx->GdmaCtl.IntEn = 1;
        pHalGdmaAdapterRx->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapterRx->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        pHalGdmaAdapterRx->GdmaCtl.SrcMsize = MsizeFour;
        pHalGdmaAdapterRx->GdmaCtl.DestMsize = MsizeOne;
        pHalGdmaAdapterRx->GdmaCtl.Sinc = NoChange;
        pHalGdmaAdapterRx->GdmaCtl.Dinc = IncType;
        pHalGdmaAdapterRx->TestItem = 3;

        //4 Slv TX DMA Setting(2) for master read
        RtlMemset((void *)pHalGdmaAdapterTxSlv, 0, sizeof(HAL_GDMA_ADAPTER));
        pHalGdmaAdapterTxSlv->GdmaCtl.IntEn = 1;
        pHalGdmaAdapterTxSlv->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        pHalGdmaAdapterTxSlv->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
        //pHalGdmaAdapterTx->GdmaCtl.DstTrWidth = TrWidthFourBytes;  //tingchu
        pHalGdmaAdapterTxSlv->GdmaCtl.SrcMsize = MsizeFour;
        pHalGdmaAdapterTxSlv->GdmaCtl.DestMsize = MsizeEight;//MsizeEight;
        //pHalGdmaAdapterTxSlv->GdmaCtl.DestMsize = MsizeFour;//tingchu
        pHalGdmaAdapterTxSlv->GdmaCtl.Sinc = IncType;
        pHalGdmaAdapterTxSlv->GdmaCtl.Dinc = NoChange;	
        pHalGdmaAdapterTxSlv->TestItem = 2;
    }
    //4 
    
    //4 TX DMA Intr Setting
    Gdma4IrqHandle.Data = (u32) (pGdmaAdapterTx);
#if defined(_HAL_IRQN_H_96F) 
    Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
#elif defined(_HAL_IRQN_H_98F)
    Gdma4IrqHandle.IrqNum = DW_ahb_dmac_top;
#endif
    Gdma4IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch4IrqHandle;
    Gdma4IrqHandle.Priority = 0;
    InterruptRegister(&Gdma4IrqHandle);
    InterruptEn(&Gdma4IrqHandle); 

    if (!I2CVeriMtrWr){
        //4 Slv Rx DMA Intr Setting for master Write
        Gdma0IrqHandle.Data = (u32) (pGdmaAdapterRxSlv);
#if defined(_HAL_IRQN_H_96F) 
        Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
#elif defined(_HAL_IRQN_H_98F)
        Gdma0IrqHandle.IrqNum = DW_ahb_dmac_top;
#endif
        Gdma0IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch4IrqHandle;
        Gdma0IrqHandle.Priority = 0;
        //InterruptRegister(&Gdma1IrqHandle);
        //InterruptEn(&Gdma1IrqHandle);
    }
    else{
            //4 RX DMA Intr Setting
        Gdma0IrqHandle.Data = (u32) (pGdmaAdapterRx);
#if defined(_HAL_IRQN_H_96F) 
        Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
#elif defined(_HAL_IRQN_H_98F)
        Gdma0IrqHandle.IrqNum = DW_ahb_dmac_top;
#endif
        Gdma0IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch4IrqHandle;
        Gdma0IrqHandle.Priority = 0;
       	//InterruptRegister(&Gdma0IrqHandle);
        //InterruptEn(&Gdma0IrqHandle);

        //4 Slv TX DMA Intr Setting for master read
        Gdma1IrqHandle.Data = (u32) (pGdmaAdapterTxSlv);
#if defined(_HAL_IRQN_H_96F)
        Gdma1IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
#elif defined(_HAL_IRQN_H_98F)
        Gdma1IrqHandle.IrqNum = DW_ahb_dmac_top;
#endif
        Gdma1IrqHandle.IrqFun = (IRQ_FUN) I2CGdma0Ch4IrqHandle;
        Gdma1IrqHandle.Priority = 0;
        //InterruptRegister(&Gdma1IrqHandle);
        //InterruptEn(&Gdma1IrqHandle);
    }

    if (!I2CVeriMtrWr){
        //4 TX DMA Setting(3)
        pHalGdmaAdapterTx->GdmaCtl.Done = 1;
        pHalGdmaAdapterTx->GdmaCtl.TtFc = 1;
        pHalGdmaAdapterTx->GdmaCfg.DestPer = 8;
    
        pHalGdmaAdapterTx->MuliBlockCunt = 0;
        pHalGdmaAdapterTx->MaxMuliBlock = MaxLlp;
        pHalGdmaAdapterTx->ChSar = (u32)pSrcTx;
        pHalGdmaAdapterTx->ChDar = (u32)pDstTx;
        pHalGdmaAdapterTx->ChNum = 1;
        pHalGdmaAdapterTx->GdmaIndex = GdmaIndex;
        pHalGdmaAdapterTx->ChEn = GdmaCh1;
        pHalGdmaAdapterTx->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterTx->IsrCtrl = ENABLE;
        pHalGdmaAdapterTx->GdmaOnOff = ON;
        pHalGdmaAdapterTx->PacketLen = DATA_SIZE_I2C;
        pHalGdmaAdapterTx->BlockLen = BLOCK_SIZE_I2C;
        //4 Slv RX DMA Setting(3)
        pHalGdmaAdapterRxSlv->GdmaCtl.Done = 1;
        pHalGdmaAdapterRxSlv->GdmaCtl.TtFc = 2;
        pHalGdmaAdapterRxSlv->GdmaCfg.SrcPer  = 11;
        pHalGdmaAdapterRxSlv->GdmaCfg.ReloadSrc = 1;

        pHalGdmaAdapterRxSlv->MuliBlockCunt = 0;
        pHalGdmaAdapterRxSlv->MaxMuliBlock = MaxLlp;
        pHalGdmaAdapterRxSlv->ChSar = (u32)pSrcRxSlv;
        pHalGdmaAdapterRxSlv->ChDar = (u32)pDstRxSlv;
        pHalGdmaAdapterRxSlv->ChNum = 0;
        pHalGdmaAdapterRxSlv->GdmaIndex = GdmaIndex;
        pHalGdmaAdapterRxSlv->ChEn = GdmaCh0;
        pHalGdmaAdapterRxSlv->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterRxSlv->IsrCtrl = ENABLE;
        pHalGdmaAdapterRxSlv->GdmaOnOff = ON;
        pHalGdmaAdapterRxSlv->PacketLen = DATA_SIZE_I2C;
        pHalGdmaAdapterRxSlv->BlockLen = BLOCK_SIZE_I2C;
    }
    else{
        
        //4 TX DMA Setting(3)
        pHalGdmaAdapterTx->GdmaCtl.Done = 1;
        pHalGdmaAdapterTx->GdmaCtl.TtFc = 1;
        pHalGdmaAdapterTx->GdmaCfg.DestPer = 8;

        pHalGdmaAdapterTx->MuliBlockCunt = 0;
        pHalGdmaAdapterTx->MaxMuliBlock = MaxLlp;
        pHalGdmaAdapterTx->ChSar = (u32)pSrcTx;
        pHalGdmaAdapterTx->ChDar = (u32)pDstTx;
        pHalGdmaAdapterTx->ChNum = 3;
        pHalGdmaAdapterTx->GdmaIndex = GdmaIndex;
        pHalGdmaAdapterTx->ChEn = GdmaCh3;
        pHalGdmaAdapterTx->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterTx->IsrCtrl = ENABLE;
        pHalGdmaAdapterTx->GdmaOnOff = ON;
        pHalGdmaAdapterTx->PacketLen = DATA_SIZE_I2C;
        pHalGdmaAdapterTx->BlockLen = BLOCK_SIZE_I2C;

        //4 RX DMA Setting(3)
        pHalGdmaAdapterRx->GdmaCtl.Done = 1;
        pHalGdmaAdapterRx->GdmaCtl.TtFc = 2;
        pHalGdmaAdapterRx->GdmaCfg.SrcPer  = 9;
        pHalGdmaAdapterRx->GdmaCfg.ReloadSrc = 1;

        pHalGdmaAdapterRx->MuliBlockCunt = 0;
        pHalGdmaAdapterRx->MaxMuliBlock = MaxLlp;
        pHalGdmaAdapterRx->ChSar = (u32)pSrcRx;
        pHalGdmaAdapterRx->ChDar = (u32)pDstRx;
        pHalGdmaAdapterRx->ChNum = 0;
        pHalGdmaAdapterRx->GdmaIndex = GdmaIndex;
        pHalGdmaAdapterRx->ChEn = GdmaCh0;
        pHalGdmaAdapterRx->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterRx->IsrCtrl = ENABLE;
        pHalGdmaAdapterRx->GdmaOnOff = ON;
        pHalGdmaAdapterRx->PacketLen = DATA_SIZE_I2C;
        pHalGdmaAdapterRx->BlockLen = BLOCK_SIZE_I2C;

        //4 Slv TX DMA Setting(3) for master read
        pHalGdmaAdapterTxSlv->GdmaCtl.Done = 1;
        pHalGdmaAdapterTxSlv->GdmaCtl.TtFc = 1;
        pHalGdmaAdapterTxSlv->GdmaCfg.DestPer = 10;

        pHalGdmaAdapterTxSlv->MuliBlockCunt = 0;
        pHalGdmaAdapterTxSlv->MaxMuliBlock = MaxLlp;
        pHalGdmaAdapterTxSlv->ChSar = (u32)pSrcTxSlv;
        pHalGdmaAdapterTxSlv->ChDar = (u32)pDstTxSlv;
        pHalGdmaAdapterTxSlv->ChNum = 1;
        pHalGdmaAdapterTxSlv->GdmaIndex = GdmaIndex;
        pHalGdmaAdapterTxSlv->ChEn = GdmaCh1;
        pHalGdmaAdapterTxSlv->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterTxSlv->IsrCtrl = ENABLE;
        pHalGdmaAdapterTxSlv->GdmaOnOff = ON;
        pHalGdmaAdapterTxSlv->PacketLen = DATA_SIZE_I2C;
        pHalGdmaAdapterTxSlv->BlockLen = BLOCK_SIZE_I2C;
    }

    
    //To start test loops
    for (I2CVeriLoopCnt=0; I2CVeriLoopCnt<I2CVeriLoop; I2CVeriLoopCnt++)
    {
#ifndef CONFIG_DEBUG_LOG_I2C_HAL
        if ((I2CVeriGCnt%DBG_I2C_LOG_PERD)==0)
        {
            printf("%s,I2C Verify Loop Cnt:%x\n",__func__,I2CVeriGCnt);
            printf("SpdMode:%2x, AddMode:%2x\n",pI2CDatAdapterMtr->SpdMd,pI2CDatAdapterMtr->AddrMd);
        }
#endif    
#if !TASK_SCHEDULER_DISABLED
        if (pI2CVeriPara->VeriStop == _TRUE)
        {
            break;
        }
#endif            

        I2CVeriGCnt++;
        SlaveRXDone = 0;
        printf("    <I2C_TEST_DTR_INT Loop %d>\n",I2CVeriLoopCnt);
        //To generate packet count
        //test small size 
        I2CVeriPktCnt = Rand()%I2C_VERI_DATA_LEN;
        
        if (I2CVeriPktCnt < 128)
        {
            I2CVeriPktCnt += 128;
        }
        else if (I2CVeriPktCnt < 256)
        {
            I2CVeriPktCnt += (Rand()%256);
        }
        else if (I2CVeriPktCnt < 512)
        {
            I2CVeriPktCnt += (Rand()%512);
        }        
        
        if (!I2CVeriPktCnt)
            I2CVeriPktCnt = 1024;

        if (I2CVeriPktCnt&0x01)
                I2CVeriPktCnt++;

        //I2CVeriPktCnt = 16;

        //4 TX DMA Initialization
        pHalGdmaAdapterTx->GdmaCtl.BlockSize = I2CVeriPktCnt/2;
        //pHalGdmaAdapterTx->GdmaCtl.BlockSize = I2CVeriPktCnt; //tingchu
        pHalGdmaOpTx->HalGdmaOnOff((VOID*)(pHalGdmaAdapterTx));
        pHalGdmaOpTx->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterTx));
        pHalGdmaOpTx->HalGdmaChSeting((VOID*)(pHalGdmaAdapterTx));       

        if (!I2CVeriMtrWr){
            
            //4 Slv RX DMA Initialization
            pHalGdmaAdapterRxSlv->GdmaCtl.BlockSize = I2CVeriPktCnt;
            pHalGdmaOpRxSlv->HalGdmaOnOff((VOID*)(pHalGdmaAdapterRxSlv));
            pHalGdmaOpRxSlv->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterRxSlv));
            pHalGdmaOpRxSlv->HalGdmaChSeting((VOID*)(pHalGdmaAdapterRxSlv));
        }
        else{
                //4 RX DMA Initialization
            pHalGdmaAdapterRx->GdmaCtl.BlockSize = I2CVeriPktCnt;
            pHalGdmaOpRx->HalGdmaOnOff((VOID*)(pHalGdmaAdapterRx));
            pHalGdmaOpRx->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterRx));
            pHalGdmaOpRx->HalGdmaChSeting((VOID*)(pHalGdmaAdapterRx));

            //4 Slv TX DMA Initialization for master read
            pHalGdmaAdapterTxSlv->GdmaCtl.BlockSize = I2CVeriPktCnt/2;
            //pHalGdmaAdapterTxSlv->GdmaCtl.BlockSize = I2CVeriPktCnt;//tingchu
            pHalGdmaOpTxSlv->HalGdmaOnOff((VOID*)(pHalGdmaAdapterTxSlv));
            pHalGdmaOpTxSlv->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterTxSlv));
            pHalGdmaOpTxSlv->HalGdmaChSeting((VOID*)(pHalGdmaAdapterTxSlv));  
        }
        
        
        
        //3 I2C TXRX>>>>
        //clear all pending intrs.
        pI2CDatAdapterSlv->IC_INRT_MSK = 0;
        pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
        
        pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
        pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv);          
        
        printf("     I2C_TEST_DTR_INT PktCnt = %d \n",I2CVeriPktCnt);               

        //4 to set master read/write
        RtlMemset((void *)I2CVeriDatCmdSrcMtr, 0x0000, I2C_VERI_DATA_LEN);
        RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
        RtlMemset((void *)I2CVeriDatCmdSrcSlv, 0x00, I2C_VERI_DATA_LEN);//for master read
		//RtlMemset((void *)I2CVeriDatSrc, 0x00, I2C_VERI_DATA_LEN);

        if (!I2CVeriMtrWr)
        {            
            //RtlMemset((void *)I2CVeriDatCmdSrcMtr, 0x0000, I2C_VERI_DATA_LEN);
            //RtlMemset((void *)I2CVeriDatDes, 0x00, I2C_VERI_DATA_LEN);
            I2CVeriCntRd = 0;
            pI2CDatAdapterMtr->RWCmd = 0;

            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;
                I2CVeriDatCmdSrcMtr[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
                
                if (I2CVeriCnt == (I2CVeriPktCnt-1))
                    I2CVeriDatCmdSrcMtr[I2CVeriCnt] |= BIT_CTRL_IC_DATA_CMD_STOP(1);
            }

            //enable I2C RX intr.        
            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_OVER(1) | BIT_CTRL_IC_INTR_MASK_M_RX_FULL(0) | BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1) |
                                                BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
			
            //*****flush data before DMA******
			//flush_cache(&I2CVeriDatCmdSrcMtr[0], I2C_VERI_DATA_LEN*sizeof(u16));
            flush_dcache_range(&I2CVeriDatCmdSrcMtr[0],(&I2CVeriDatCmdSrcMtr[0])+I2C_VERI_DATA_LEN*sizeof(u16) );
			printf("I2CVeriDatCmdSrcMtr=%x\n",I2CVeriDatCmdSrcMtr);
            //flush_cache(&I2CVeriDatDes[0], I2C_VERI_DATA_LEN*sizeof(u16));
    		//printf("I2CVeriDatDes=%x\n",I2CVeriDatDes);
            #if 0
			pHalGdmaOpTx->HalGdmaChEn((VOID*)(pHalGdmaAdapterTx));
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr);
            #else
            //*****invalidate cache before DMA******
    		u32 start = (u32)I2CVeriDatDes & ~(ARCH_DMA_MINALIGN - 1);	
    		u32 end = ALIGN(start + I2C_VERI_DATA_LEN, ARCH_DMA_MINALIGN);
    		//invalidate_dcache_range(start,end);
            flush_dcache_range(start,end);
            
            //4 Enable master DMA and GDMA channel 0 for RX
            pHalGdmaOpRxSlv->HalGdmaChEn((VOID*)(pHalGdmaAdapterRxSlv));
            pI2CDatAdapterSlv->DMAEn = BIT_CTRL_IC_DMA_CR_RDMAE(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterSlv);
			//printf("i2c DMA enable=%x\n",HAL_I2C_READ32(0,REG_DW_I2C_IC_DMA_CR));
			//4 Enable master DMA and GDMA channel 4 for TX
            pHalGdmaOpTx->HalGdmaChEn((VOID*)(pHalGdmaAdapterTx));
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr);
			#endif
            
        }
        else{
            pI2CDatAdapterMtr->RWCmd = 1;

            //create data pattern
            for (I2CVeriCnt= 0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
            {
                I2CPtn = Rand()%I2C_VERI_PANT_CNT;
                //create master read command
                I2CVeriDatCmdSrcMtr[I2CVeriCnt] = 0xFF;//dummy data
                I2CVeriDatCmdSrcMtr[I2CVeriCnt] |= BIT_CTRL_IC_DATA_CMD_CMD(1);//read command
                
                if (I2CVeriCnt == (I2CVeriPktCnt-1))
                    I2CVeriDatCmdSrcMtr[I2CVeriCnt] |= BIT_CTRL_IC_DATA_CMD_STOP(1);

                //create slave reply data
                I2CVeriDatCmdSrcSlv[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
    			//I2CVeriDatSrc[I2CVeriCnt] = I2CVeriPattern[I2CPtn];
            }

            
            //Enable slave intr.
            pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RD_REQ(1) | BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | BIT_CTRL_IC_INTR_MASK_M_RX_DONE(1);
            pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);

    		//*****flush data before DMA******
    		//flush_cache(&I2CVeriDatCmdSrcMtr[0], I2C_VERI_DATA_LEN*sizeof(u16));
            flush_dcache_range(&I2CVeriDatCmdSrcMtr[0],(&I2CVeriDatCmdSrcMtr[0])+I2C_VERI_DATA_LEN*sizeof(u16) );
    		printf("I2CVeriDatCmdSrcMtr=%x\n",I2CVeriDatCmdSrcMtr);
    		//flush_cache(&I2CVeriDatCmdSrcSlv[0], I2C_VERI_DATA_LEN*sizeof(u16));
            flush_dcache_range(&I2CVeriDatCmdSrcSlv[0],(&I2CVeriDatCmdSrcSlv[0])+I2C_VERI_DATA_LEN*sizeof(u16) );
    		printf("I2CVeriDatCmdSrcSlv=%x\n",I2CVeriDatCmdSrcSlv);

    		//*****invalidate cache before DMA******
    		u32 start = (u32)I2CVeriDatDes & ~(ARCH_DMA_MINALIGN - 1);	
    		u32 end = ALIGN(start + I2C_VERI_DATA_LEN, ARCH_DMA_MINALIGN);
    		//invalidate_dcache_range(start,end);
    		flush_dcache_range(start,end);
    		
            //4 Enable slave DMA and GDMA channel 1 for TX
            pHalGdmaOpTxSlv->HalGdmaChEn((VOID*)(pHalGdmaAdapterTxSlv));
            pI2CDatAdapterSlv->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            
            //4 Enable master DMA and GDMA channel 0 for RX
            pHalGdmaOpRx->HalGdmaChEn((VOID*)(pHalGdmaAdapterRx));
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_RDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr);

            //4 Enable master DMA and GDMA channel 4 for TX
            pHalGdmaOpTx->HalGdmaChEn((VOID*)(pHalGdmaAdapterTx));
            pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1);
            pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableDMA(pI2CDatAdapterMtr);            
        }
        udelay(10000);//tingchu delay test
         if (!I2CVeriMtrWr)
         {
            I2CTimeOutCnt = 0;
            while (I2CVeriCntRd<I2CVeriPktCnt)
            {
                if (I2CTimeOutCnt++ > (1000*I2C_VERI_TIMEOUT_CNT)) //change to 1000
                {
                    printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                    pI2CVeriPara->VeriStop = _TRUE;
#endif            
                    I2CErrFlag = _TRUE;
                    break;
                }   
            }
        }
         else{
            //wait for slave RX Done 
            I2CTimeOutCnt = 0;
            for (;SlaveRXDone==0;){
                if (I2CTimeOutCnt++ > (1000*I2C_VERI_TIMEOUT_CNT))//change to 1000
                    {
                        printf("     **** Timeout Error ****\n\n");
#if !TASK_SCHEDULER_DISABLED
                        pI2CVeriPara->VeriStop = _TRUE;
#endif            
                        I2CErrFlag = _TRUE;
                        break;
                    }
            }
         }
        
            
        
        //compare data
        printf("     Start to Compare Data>>>>>\n");
        printf("     I2C_TEST_DTR_INT Result:\n",I2CVeriPktCnt);
        for(I2CVeriCnt=0; I2CVeriCnt < I2CVeriPktCnt; I2CVeriCnt++)
        {
            if (!I2CVeriMtrWr){
                if ((I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0x00FF) != I2CVeriDatDes[I2CVeriCnt])
                {
                    I2CErrFlag = _TRUE;
                    printf("             Pkt%4d(S,D): %4x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
                    printf("     **** Compare Error ****\n\n");
                    break;
                }
    			else
    			{
    				printf("             Pkt%4d(S,D): %4x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcMtr[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
    			}
            }
            else{
                if ((I2CVeriDatCmdSrcSlv[I2CVeriCnt]&0x00FF) != I2CVeriDatDes[I2CVeriCnt])
                {
                    I2CErrFlag = _TRUE;
                    printf("             Pkt%4d(S,D): %4x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcSlv[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
                    printf("     **** Compare Error ****\n\n");
                    break;
                }
    			else
    			{
    				printf("             Pkt%4d(S,D): %4x, %2x\n",I2CVeriCnt, 
                                        (I2CVeriDatCmdSrcSlv[I2CVeriCnt]&0xFFFF), I2CVeriDatDes[I2CVeriCnt]);
    			}
            }
         
            
        }

        printf("     >>>>>Compare Done<<<<<\n\n");
        pI2CDatAdapterMtr->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1) | BIT_CTRL_IC_DMA_CR_RDMAE(1);
        pI2CAdapterMtr->pI2CIrqOp->HalI2CDisableDMA(pI2CDatAdapterMtr);
        pI2CDatAdapterSlv->DMAEn = BIT_CTRL_IC_DMA_CR_TDMAE(1) | BIT_CTRL_IC_DMA_CR_RDMAE(1);
        pI2CAdapterSlv->pI2CIrqOp->HalI2CDisableDMA(pI2CDatAdapterSlv);
        
        //disable all intr first
        pI2CDatAdapterMtr->IC_INRT_MSK = 0;
        pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr);
        pI2CDatAdapterSlv->IC_INRT_MSK = 0x0;
        pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
        //3 I2C TXRX<<<<     
        
        //4 Disable GDMA channel
        pHalGdmaOpTxSlv->HalGdmaChDis((VOID*)(pHalGdmaAdapterTxSlv));
        pHalGdmaOpTx->HalGdmaChDis((VOID*)(pHalGdmaAdapterTx));
        pHalGdmaOpRx->HalGdmaChDis((VOID*)(pHalGdmaAdapterRx));

        //4 Leave the verification when compare fails
        if (I2CErrFlag == _TRUE)
            break;
    }  
}

VOID
I2CTestScan(
	IN	VOID	*I2CMtr,
	IN	VOID	*I2CSlv,
	IN	VOID	*I2CVeriPara
){
	PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
    
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;

    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;

	u32 I2CVeriLoopCnt;
    u32 I2CVeriPktCnt = 1;
    u32 I2CVeriCnt;
    u32 I2CVeriLoop = pI2CVeriPara->VeriLoop;
	u8  I2CPtn = 0;
    u8  I2CErrFlag = _FALSE;
    u32 I2CTimeOutCnt = 0;
	u32 TarValue;

	pI2CDatAdapterMtr->TSarAddr= I2C1_ADDR;
	printf("TAR addr = %x\n",pI2CDatAdapterMtr->TSarAddr);
	
	TarValue = HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR);
	HAL_I2C_WRITE32(pI2CDatAdapterMtr->Idx ,REG_DW_I2C_IC_TAR,
                        ((TarValue & ~(BIT_MASK_IC_TAR << BIT_SHIFT_IC_TAR)) |                            
                         BIT_CTRL_IC_TAR(pI2CDatAdapterMtr->TSarAddr)));
	
	printf( "IC_TAR%d[%2x]: %x\n", pI2CDatAdapterMtr->Idx, REG_DW_I2C_IC_TAR, HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR));


	pI2CDatAdapterSlv->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
	pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);

	
	printf("BB/Master raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));
	printf("BB/Slave raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));

	pI2CDatAdapterMtr->RWDat = &I2CVeriPattern[0];
	pI2CDatAdapterMtr->DatLen = 1;
	pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);      

	I2CTimeOutCnt = 0;
    I2CVeriCntRd = 0;
    for (;I2CVeriCntRd<I2CVeriPktCnt;)
    {     
        //to deal with transmittion error
        if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
        {
            printf("     **** Timeout Error ****\n\n");
            break;
        }
		
        if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
        {   
            I2CTimeOutCnt = 0;
            I2CVeriDatDes[I2CVeriCntRd] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterSlv);//HalI2CDATRead(pI2CDatAdapterMtr);
            printf("Receive: %x\n", I2CVeriDatDes[I2CVeriCntRd]);
            I2CVeriCntRd++;
            if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
                break;                            
        }
    }

	printf("B/Master raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));
	printf("B/Slave raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));

	
	pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
	pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv); 	 
	udelay(1000);
	printf("A/Master raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));
	printf("A/Slave raw INT status:%x\n", HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_RAW_INTR_STAT));

	for(I2CVeriLoopCnt = 0; I2CVeriLoopCnt<10 ; I2CVeriLoopCnt++)
	{
		//3 back to right address
		pI2CDatAdapterMtr->TSarAddr= I2C0_ADDR;
		printf("TAR addr = %x\n",pI2CDatAdapterMtr->TSarAddr);
		
		TarValue = HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR);
		HAL_I2C_WRITE32(pI2CDatAdapterMtr->Idx ,REG_DW_I2C_IC_TAR,
                            ((TarValue & ~(BIT_MASK_IC_TAR << BIT_SHIFT_IC_TAR)) |                            
                             BIT_CTRL_IC_TAR(pI2CDatAdapterMtr->TSarAddr)));
		
		printf( "IC_TAR%d[%2x]: %x\n", pI2CDatAdapterMtr->Idx, REG_DW_I2C_IC_TAR, HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR));
		
		pI2CDatAdapterMtr->RWDat = &I2CVeriPattern[0];
    	pI2CDatAdapterMtr->DatLen = 1;
    	pI2CAdapterMtr->pI2CIrqOp->HalI2CDATWrite(pI2CDatAdapterMtr);      

		I2CTimeOutCnt = 0;
        I2CVeriCntRd = 0;
        for (;I2CVeriCntRd<I2CVeriPktCnt;)
        {     
            //to deal with transmittion error
            if (I2CTimeOutCnt++ > (10*I2C_VERI_TIMEOUT_CNT))
            {
                printf("     **** Timeout Error ****\n\n");
                break;
            }
			
            if (HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_STATUS) & BIT_CTRL_IC_STATUS_RFNE(1))
            {   
                I2CTimeOutCnt = 0;
                I2CVeriDatDes[I2CVeriCntRd] = pI2CAdapterSlv->pI2CIrqOp->HalI2CDATRead(pI2CDatAdapterSlv);//HalI2CDATRead(pI2CDatAdapterMtr);
                printf("Receive: %x\n", I2CVeriDatDes[I2CVeriCntRd]);
                I2CVeriCntRd++;
                if (I2CVeriCntRd > I2C_VERI_DATA_LEN)
                    break;                            
            }
        }

		udelay(1000);
	}
	
}

VOID
Test_Loop(IN  VOID    *I2CMtr,IN  VOID    *I2CSlv,  IN  VOID    *I2CVeriPara){

	PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = (PHAL_DW_I2C_ADAPTER)(I2CMtr);
	PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = (PHAL_DW_I2C_ADAPTER)(I2CSlv);
	
	PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterMtr->pI2CIrqDat;
	PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = (PHAL_DW_I2C_DAT_ADAPTER)pI2CAdapterSlv->pI2CIrqDat;
	
	PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CVeriPara;
	
    s8  I2CVeriAddrMod = ADDR_7BIT;
    u8  I2CVeriSpdMod = SS_MODE;

	u32 i2cTest;
	i2cTest = pI2CVeriPara->VeriItem;
	
	for (I2CVeriSpdMod = SS_MODE; I2CVeriSpdMod<= SS_MODE; I2CVeriSpdMod++)
	{	
#if !TASK_SCHEDULER_DISABLED
		if (pI2CVeriPara->VeriStop == _TRUE)
		{
			break;
		}
#endif            
		//To set speed mode
		pI2CDatAdapterSlv->SpdMd = I2CVeriSpdMod;
		pI2CDatAdapterMtr->SpdMd = I2CVeriSpdMod;
		switch (I2CVeriSpdMod)
		{
			case SS_MODE:
				pI2CDatAdapterMtr->SpdHz = 100;
				break;
				
			case FS_MODE:
				pI2CDatAdapterMtr->SpdHz = 400;
				break; 

			case HS_MODE:
				pI2CDatAdapterMtr->SpdHz = 400;
				break; 
				
			default:
				break;
		}
		for (I2CVeriAddrMod = ADDR_7BIT; I2CVeriAddrMod <= ADDR_7BIT; I2CVeriAddrMod++)
		{
#if !TASK_SCHEDULER_DISABLED
			if (pI2CVeriPara->VeriStop == _TRUE)
			{
				break;
			}
#endif            
		
			//4 to set address mode
			pI2CDatAdapterSlv->AddrMd = I2CVeriAddrMod;
			pI2CDatAdapterMtr->AddrMd = I2CVeriAddrMod;

			//4 RESTART MUST be set in these condition in Master mode. 
			//4 But it might be NOT compatible in old slaves.
			if ((I2CVeriAddrMod == ADDR_10BIT) || (I2CVeriSpdMod==HS_MODE))
				pI2CDatAdapterMtr->ReSTR = 1;
			
			//master/slave init.
			pI2CAdapterSlv->pI2CIrqOp->HalI2CInitSlave(pI2CDatAdapterSlv);
			printf("	--->  I2C slave addr: %x\n",HAL_I2C_READ32(pI2CDatAdapterSlv->Idx,REG_DW_I2C_IC_SAR));
			pI2CAdapterMtr->pI2CIrqOp->HalI2CInitMaster(pI2CDatAdapterMtr);
			printf("	--->  I2C tar addr in master: %x\n",HAL_I2C_READ32(pI2CDatAdapterMtr->Idx,REG_DW_I2C_IC_TAR));
			pI2CAdapterMtr->pI2CIrqOp->HalI2CSetCLK(pI2CDatAdapterMtr);
			
			printf("	--->  I2C Master Idx: %x\n",pI2CDatAdapterMtr->Idx);
			printf("	--->  I2C Slave Idx: %x\n",pI2CDatAdapterSlv->Idx);
			printf("	--->  I2C Speed Mode: %x\n",I2CVeriSpdMod);
			printf("	--->  I2C Addr Mode: %x\n",I2CVeriAddrMod);


			//4 To set master/slave interrupt
			//disable all intr first
			pI2CDatAdapterMtr->IC_INRT_MSK = 0;
			pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr); 		 
			
			//master intr.
			switch(i2cTest)
			{
				case I2C_TEST_S_NINT:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = 0x0;
					break;
				}
				case I2C_TEST_S_RINT:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | 
								                     //BIT_CTRL_IC_INTR_MASK_M_TX_EMPTY(1) |
                                                     BIT_CTRL_IC_INTR_MASK_M_TX_OVER(1) | 
                                                     BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
					break;
				}
				case I2C_TEST_D_INT:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | 
													BIT_CTRL_IC_INTR_MASK_M_TX_OVER(1) |  
													BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
					break;
				}
				case I2C_TEST_DTR_INT:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | 
													BIT_CTRL_IC_INTR_MASK_M_TX_OVER(1) |
												 	BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
					break;
				}
				case I2C_TEST_INT_ERR:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | 
													BIT_CTRL_IC_INTR_MASK_M_TX_OVER(1) |
												 	BIT_CTRL_IC_INTR_MASK_M_TX_EMPTY(1) |
												 	BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
					break;
				}
				case I2C_TEST_INT_GenCallvsStatus:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1) | 
													BIT_CTRL_IC_INTR_MASK_M_ACTIVITY(1) |
													BIT_CTRL_IC_INTR_MASK_M_START_DET(1) |
													BIT_CTRL_IC_INTR_MASK_M_STOP_DET(1)	|
													BIT_CTRL_IC_INTR_MASK_M_GEN_CALL(1);
					break;
				}
				case I2C_TEST_INT_TXABRT:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_TX_ABRT(1)	|
													BIT_CTRL_IC_INTR_MASK_M_GEN_CALL(1);
					break;
				}
				case I2C_ADDR_SCAN:
				{
					pI2CDatAdapterMtr->IC_INRT_MSK = BIT_CTRL_IC_INTR_MASK_M_RX_UNDER(1);
					break;
				}
				default:
					break;
				
			}
			pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr);

			//disable all intr first
			pI2CDatAdapterSlv->IC_INRT_MSK = 0x0;
			pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);

			//enable master/slave after init.
			pI2CDatAdapterMtr->ModulEn = 1;
			pI2CDatAdapterSlv->ModulEn = 1;
			pI2CAdapterSlv->pI2CIrqOp->HalI2CModuleEn(pI2CDatAdapterSlv);												 
			pI2CAdapterMtr->pI2CIrqOp->HalI2CModuleEn(pI2CDatAdapterMtr);
			
			//clear all pending intrs.
			pI2CAdapterMtr->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterMtr);
			pI2CAdapterSlv->pI2CIrqOp->HalI2CClrAllIntr(pI2CDatAdapterSlv);

			switch(i2cTest)
			{
				case I2C_TEST_S_NINT:
				{
					I2CTestMrtWR(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_S_RINT:
				{
					I2CTestMrtRD_INTR(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_D_INT:
				{
					I2CTestTxRxDMA(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_DTR_INT:
				{
					
					I2CTestTRxDMA(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_INT_ERR:
				{
					I2CTest_MrtWR_INTERR(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_INT_GenCallvsStatus:
				{
					I2CTest_MrtWR_INT_GenCall(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_TEST_INT_TXABRT:
				{
					I2CTest_MrtWR_INT_TXABRT(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				case I2C_ADDR_SCAN:
				{
					I2CTestScan(pI2CAdapterMtr,pI2CAdapterSlv,pI2CVeriPara);
					break;
				}
				default:
					break;
			}
			//disable all intr first
			pI2CDatAdapterMtr->IC_INRT_MSK = 0;
			pI2CAdapterMtr->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterMtr);
			pI2CDatAdapterSlv->IC_INRT_MSK = 0x0;
			pI2CAdapterSlv->pI2CIrqOp->HalI2CEnableIntr(pI2CDatAdapterSlv);
			
			pI2CDatAdapterMtr->ModulEn = 0;
			pI2CDatAdapterSlv->ModulEn = 0;
			pI2CAdapterMtr->pI2CIrqOp->HalI2CModuleEn(pI2CDatAdapterMtr);
			pI2CAdapterSlv->pI2CIrqOp->HalI2CModuleEn(pI2CDatAdapterSlv);
		   
		}
	}

}
#endif

VOID    
I2CTestProc(
    IN  VOID    *I2CTestData
)
{
    u32 i2ctemp; 
    s8  I2CVeriAddrMod = ADDR_7BIT;
    u8  I2CVeriSpdMod = SS_MODE;

	/*  //ARM use
	ALLOC_CACHE_ALIGN_BUFFER(u8, I2CVeriDatDes3, I2C_VERI_DATA_LEN);
	I2CVeriDatDes = I2CVeriDatDes3;
	printf("init I2CVeriDatDes=%x\n",I2CVeriDatDes);
	*/
	//I2CVeriDatDes =  (u8*)0xBFD00000; //ROM address
	printf("init I2CVeriDatDes=%x\n",I2CVeriDatDes);

    //4 assign pattern array
    I2CVeriPattern[0] = 0x55;    I2CVeriPattern[1] = 0xAA;
    I2CVeriPattern[2] = 0x5A;    I2CVeriPattern[3] = 0xA5;
    I2CVeriPattern[4] = 0xFF;    I2CVeriPattern[5] = 0x11;
    
    PHAL_DW_I2C_OP pHalI2COp = (PHAL_DW_I2C_OP)&HalI2COp;
      
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterMtr = &TestI2CMasterDat;
    PHAL_DW_I2C_DAT_ADAPTER pI2CDatAdapterSlv = &TestI2CSlaveDat;
    
    PI2C_VERI_PARA pI2CVeriPara = (PI2C_VERI_PARA) I2CTestData;

    PHAL_DW_I2C_ADAPTER pI2CAdapterMtr = &TestI2CMaster;
    PHAL_DW_I2C_ADAPTER pI2CAdapterSlv = &TestI2CSlave;

    HalI2COpInit(pHalI2COp);
    
    i2cTest = pI2CVeriPara->VeriItem;

    printf("i2cTest Item = %d\n",i2cTest);    
    printf("i2cTest loop = %d\n",pI2CVeriPara->VeriLoop);
    printf("i2cTest Master Idx = %d\n",pI2CVeriPara->VeriMaster);
    printf("i2cTest Slave Idx = %d\n",pI2CVeriPara->VeriSlave);
    printf("i2cTest Master WR? = %d\n",pI2CVeriPara->MtrRW);

	
    //4 Slave Setting    
    pI2CDatAdapterSlv->Idx = pI2CVeriPara->VeriSlave;//I2C1_SEL;
#if 0
    pI2CDatAdapterSlv->PinMux = I2C0_TO_E;//I2C1_TO_C;    
#endif
    pI2CDatAdapterSlv->Master = 0;
    pI2CDatAdapterSlv->AddrMd = I2CVeriAddrMod;
    pI2CDatAdapterSlv->TSarAddr= I2C0_ADDR;//slave addr
    pI2CDatAdapterSlv->ModulEn = 1;
    pI2CDatAdapterSlv->SDAHd = 7;//mini value:7
    pI2CDatAdapterSlv->GC_STR = 1;//enable GC ack or not, 1 for enable
    pI2CAdapterSlv->pI2CIrqOp = (PHAL_DW_I2C_OP)pHalI2COp;
    pI2CAdapterSlv->pI2CIrqDat = (PHAL_DW_I2C_DAT_ADAPTER)pI2CDatAdapterSlv;

    //4 Slave IRQ Setting
    I2CIrqHandle_Slv.Data = (u32) (pI2CAdapterSlv);
    switch (pI2CDatAdapterSlv->Idx){
        case I2C0_SEL:                
        #if defined(_HAL_IRQN_H_96F)
            I2CIrqHandle_Slv.IrqNum = DW_I2C_0_IP_IRQ;
        #elif defined(_HAL_IRQN_H_98F)
            I2CIrqHandle_Slv.IrqNum = DW_apb_i2c_top;
        #endif
            I2CIrqHandle_Slv.IrqFun = (IRQ_FUN) I2C0IrqHandle;            
            break;
            
        case I2C1_SEL:                
        #if defined(_HAL_IRQN_H_96F)
            I2CIrqHandle_Slv.IrqNum = DW_I2C_1_IE_IRQ;
        #elif defined(_HAL_IRQN_H_98F) 
            I2CIrqHandle_Slv.IrqNum = DW_apb_i2c_1_top;
        #endif
            I2CIrqHandle_Slv.IrqFun = (IRQ_FUN) I2C1IrqHandle;            
            break;
#if 0            
        case I2C2_SEL:                
            I2CIrqHandle_Slv.IrqNum = I2C2_IRQ;
            I2CIrqHandle_Slv.IrqFun = (IRQ_FUN) I2C2IrqHandle;            
            break;
            
        case I2C3_SEL:                
            I2CIrqHandle_Slv.IrqNum = I2C3_IRQ;
            I2CIrqHandle_Slv.IrqFun = (IRQ_FUN) I2C3IrqHandle;            
            break;
#endif            
        default:
            break;
    }
    I2CIrqHandle_Slv.Priority = 1;
#if 0 //97F
    InterruptRegister(&I2CIrqHandle_Slv);
    InterruptEn(&I2CIrqHandle_Slv);
#else //98F ARM
    irq_install_handler(I2CIrqHandle_Slv.IrqNum, I2CIrqHandle_Slv.IrqFun, I2CIrqHandle_Slv.Data); //98F arm
#endif
    
    //4 Master Setting
    pI2CDatAdapterMtr->Idx = pI2CVeriPara->VeriMaster;//I2C0_SEL;
#if 0
    pI2CDatAdapterMtr->PinMux = I2C1_TO_C;//I2C0_TO_E;
#endif
    pI2CDatAdapterMtr->ICClk = SYSTEM_CLK/1000000;
    pI2CDatAdapterMtr->SpdMd = I2CVeriSpdMod;
    pI2CDatAdapterMtr->Master = 1;
    pI2CDatAdapterMtr->AddrMd = I2CVeriAddrMod;
    pI2CDatAdapterMtr->TSarAddr= I2C0_ADDR;//slave addr
    pI2CDatAdapterMtr->ModulEn = 1;
    pI2CDatAdapterMtr->SDAHd = 1;//mini value:1
    pI2CDatAdapterMtr->Spec = 0;
    pI2CDatAdapterMtr->GC_STR = 0;//0 for using GC
    pI2CAdapterMtr->pI2CIrqOp = (PHAL_DW_I2C_OP)pHalI2COp;
    pI2CAdapterMtr->pI2CIrqDat = (PHAL_DW_I2C_DAT_ADAPTER)pI2CDatAdapterMtr;

    //4 Master IRQ Setting
    I2CIrqHandle_Mtr.Data = (u32) (pI2CAdapterMtr);    
    switch (pI2CDatAdapterMtr->Idx){
        case I2C0_SEL:                
        #if defined(_HAL_IRQN_H_96F)
            I2CIrqHandle_Mtr.IrqNum = DW_I2C_0_IP_IRQ;
        #elif defined(_HAL_IRQN_H_98F)
            I2CIrqHandle_Mtr.IrqNum = DW_apb_i2c_top;
        #endif
            I2CIrqHandle_Mtr.IrqFun = (IRQ_FUN) I2C0IrqHandle;
            break;
            
        case I2C1_SEL:                
        #if defined(_HAL_IRQN_H_96F)
            I2CIrqHandle_Mtr.IrqNum = DW_I2C_1_IE_IRQ;
        #elif defined(_HAL_IRQN_H_98F)
            I2CIrqHandle_Mtr.IrqNum = DW_apb_i2c_1_top;
        #endif
            I2CIrqHandle_Mtr.IrqFun = (IRQ_FUN) I2C1IrqHandle;
            break;
#if 0            
        case I2C2_SEL:                
            I2CIrqHandle_Mtr.IrqNum = I2C2_IRQ;
            I2CIrqHandle_Mtr.IrqFun = (IRQ_FUN) I2C2IrqHandle;
            break;
            
        case I2C3_SEL:                
            I2CIrqHandle_Mtr.IrqNum = I2C3_IRQ;
            I2CIrqHandle_Mtr.IrqFun = (IRQ_FUN) I2C3IrqHandle;
            break;
#endif            
        default:
            break;
    }
    I2CIrqHandle_Mtr.Priority = 1;
#if 0 //97F 
    InterruptRegister(&I2CIrqHandle_Mtr);
    InterruptEn(&I2CIrqHandle_Mtr);
#else  //98F ARM
    irq_install_handler(I2CIrqHandle_Mtr.IrqNum, I2CIrqHandle_Mtr.IrqFun, I2CIrqHandle_Mtr.Data);
#endif

#if 0    
    //3 !!!
    {
        //4 To release EEPROM pinmux if I2C used
        if ((pI2CVeriPara->VeriMaster == I2C3_SEL) || (pI2CVeriPara->VeriSlave== I2C3_SEL))
        {
            HAL_WRITE32(SYSTEM_CTRL_BASE,0xA4,0);
        }        

        //4 To Disable SDR pinmux if I2C used
        PinCtrl(SDR,S0,OFF);

        //4 Turn off shutdown mode
        switch (pI2CDatAdapterMtr->Idx){
            case I2C0_SEL:                
                ACTCK_I2C0_CCTRL(ON);
                PinCtrl(I2C0,pI2CDatAdapterMtr->PinMux,ON);
                I2C0_FCTRL(ON);
                break;
                
            case I2C1_SEL:                
                ACTCK_I2C1_CCTRL(ON);
                PinCtrl(I2C1,pI2CDatAdapterMtr->PinMux,ON);
                I2C1_FCTRL(ON);
                break;
                
            case I2C2_SEL:                
                ACTCK_I2C2_CCTRL(ON);
                PinCtrl(I2C2,pI2CDatAdapterMtr->PinMux,ON);
                I2C2_FCTRL(ON);
                break;
                
            case I2C3_SEL:                
                ACTCK_I2C3_CCTRL(ON);
                PinCtrl(I2C3,pI2CDatAdapterMtr->PinMux,ON);
                I2C3_FCTRL(ON);
                break;
                
            default:
                break;
        }

        switch (pI2CDatAdapterSlv->Idx){
            case I2C0_SEL:
                ACTCK_I2C0_CCTRL(ON);
                PinCtrl(I2C0,pI2CDatAdapterSlv->PinMux,ON);
                I2C0_FCTRL(ON);
                break;
                
            case I2C1_SEL:
                ACTCK_I2C1_CCTRL(ON);
                PinCtrl(I2C1,pI2CDatAdapterSlv->PinMux,ON);
                I2C1_FCTRL(ON);
                break;
                
            case I2C2_SEL:
                ACTCK_I2C2_CCTRL(ON);
                PinCtrl(I2C2,pI2CDatAdapterSlv->PinMux,ON);
                I2C2_FCTRL(ON);
                break;
                
            case I2C3_SEL:
                ACTCK_I2C3_CCTRL(ON);
                PinCtrl(I2C3,pI2CDatAdapterSlv->PinMux,ON);
                I2C3_FCTRL(ON);
                break;
                
            default:
                break;
        }
    }
    //3 !!!
#endif

#if !TASK_SCHEDULER_DISABLED//#if !TASK_SCHEDULER_DISABLED(>>)
    for (;;)//start of for(;;)
    {   
        //4 waiting for start command
        RtlDownSema(&(pI2CVeriPara->VeriSema));
        i2cTest = pI2CVeriPara->VeriItem;
#endif                      //#if !TASK_SCHEDULER_DISABLED(<<)
    switch (i2cTest)
    {
        case I2C_TEST_S_NINT://Without DMA, Without INTR
            {   
                printf("<<<<  I2C VERI Item: I2C_TEST_S_NINT  >>>>\n");
                printf("<<<<                START             >>>>\n");
				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_S_NINT  >>>>\n");
                printf("<<<<                 END              >>>>\n\n");                        
                break;
            } 
        
        case I2C_TEST_S_RINT://only master read, without DMA, with INTR
            {
                printf("<<<<  I2C VERI Item: I2C_TEST_S_RINT  >>>>\n");
                printf("<<<<                START             >>>>\n");
				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);
    
                printf("<<<<  I2C VERI Item: I2C_TEST_S_RINT  >>>>\n");
                printf("<<<<                 END              >>>>\n\n");
                break;
            }
		
		case I2C_TEST_INT_ERR: //test TX_OVER, RX_UNDER, TX_EMPTY, RX_OVER
			{
				printf("<<<<  I2C VERI Item: I2C_TEST_INT_TX_EMP_RX_OV  >>>>\n");
                printf("<<<<                START                       >>>>\n");
               	Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_INT_TX_EMP_RX_OV  >>>>\n");
                printf("<<<<                 END                        >>>>\n\n");
                break;
			}
		
		case I2C_TEST_INT_GenCallvsStatus: //test ACTIVITY, START_DET, STOP_DET, GEN_CALL
			{
				printf("<<<<  I2C VERI Item: I2C_TEST_INT_GenCallvsStatus  >>>>\n");
                printf("<<<<                START                       >>>>\n");

			    pI2CDatAdapterMtr->Spec=1;
				pI2CDatAdapterMtr->GC_STR=0;
				//pI2CDatAdapterMtr->TSarAddr= 0;
				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_INT_GenCallvsStatus  >>>>\n");
                printf("<<<<                 END                        >>>>\n\n");
                break;
			}
		case I2C_TEST_INT_TXABRT:
			{
				printf("<<<<  I2C VERI Item: I2C_TEST_INT_TXABRT  >>>>\n");
                printf("<<<<                START                       >>>>\n");

			    pI2CDatAdapterMtr->Spec=1;
				pI2CDatAdapterMtr->GC_STR=0;
				pI2CDatAdapterSlv->GC_STR=0;
				//pI2CDatAdapterMtr->TSarAddr= 0;
				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_INT_TXABRT  >>>>\n");
                printf("<<<<                 END                        >>>>\n\n");
                break;
			}
		
        case I2C_TEST_D_INT://With DMA, with INTR(the RX_OVER intr is enabled in master write verification)
            {
                printf("<<<<  I2C VERI Item: I2C_TEST_D_INT  >>>>\n");
                printf("<<<<                START            >>>>\n");
#if 0				
				//4 !!!
				i2ctemp = HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL); // set GDMA hw handshaking I/F mapping
				printf("In I2C, defaut GDMA I/F mapping[%x]: %x\n", (PERI_ON_BASE+REG_PESOC_SOC_CTRL), i2ctemp);
				i2ctemp &= (~(0x01<<27));//I/F 11, I2C1 RX
				i2ctemp &= (~(0x01<<26));//I/F 10, I2C1 TX
				i2ctemp &= (~(0x01<<25));
				i2ctemp &= (~(0x01<<24));
				HAL_WRITE32(PERI_ON_BASE,REG_PESOC_SOC_CTRL,i2ctemp);
				printf("In I2C, GDMA I/F mapping[%x]: %x\n", (PERI_ON_BASE+REG_PESOC_SOC_CTRL), HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL));

				//4 GDMA Enable
				ACTCK_GDMA0_CCTRL(ON);
				GDMA0_FCTRL(ON);
				//4 !!!
#endif

				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_D_INT  >>>>\n");
                printf("<<<<                 END              >>>>\n\n");
                break;
            }
        
        case I2C_TEST_DTR_INT://only for master read
            {
            #if 0
                if (!pI2CVeriPara->MtrRW)
                {
                    printf("---- This test in only valid for Master Read! ----\n");
                    return;
                }
            #endif
                
               	printf("<<<<  I2C VERI Item: I2C_TEST_DTR_INT  >>>>\n");
                printf("<<<<                START              >>>>\n");
#if 0				
				//4 !!!
				i2ctemp = HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL); // set GDMA hw handshaking I/F mapping
				DBG_8195A_I2C_LVL(VERI_I2C_LVL,"In I2C, defaut GDMA I/F mapping[%x]: %x\n", (PERI_ON_BASE+REG_PESOC_SOC_CTRL), i2ctemp);
				i2ctemp &= (~(0x01<<27));//I/F 11, I2C1 RX
				i2ctemp &= (~(0x01<<26));//I/F 10, I2C1 TX
				i2ctemp &= (~(0x01<<25));//I/F 9, I2C0 RX
				i2ctemp &= (~(0x01<<24));//I/F 8, I2C0 TX				 
				HAL_WRITE32(PERI_ON_BASE,REG_PESOC_SOC_CTRL,i2ctemp);
				DBG_8195A_I2C_LVL(VERI_I2C_LVL,"In I2C, GDMA I/F mapping[%x]: %x\n", (PERI_ON_BASE+REG_PESOC_SOC_CTRL), HAL_READ32(PERI_ON_BASE,REG_PESOC_SOC_CTRL));

				//4 GDMA Enable
				ACTCK_GDMA0_CCTRL(ON);
				GDMA0_FCTRL(ON);
				//4 !!!
#endif

				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);

                printf("<<<<  I2C VERI Item: I2C_TEST_DTR_INT  >>>>\n");
                printf("<<<<                 END              >>>>\n\n");
                break;
            }
		case I2C_ADDR_SCAN:
			{
				printf("<<<<  I2C VERI Item: I2C_ADDR_SCAN  >>>>\n");
                printf("<<<<                START              >>>>\n");
				Test_Loop(pI2CAdapterMtr, pI2CAdapterSlv, pI2CVeriPara);
				
				printf("<<<<  I2C VERI Item: I2C_ADDR_SCAN  >>>>\n");
                printf("<<<<                 END              >>>>\n\n");
				break;
			}
        default:
            goto ENDI2CTASK;
            break;
    }
#if !TASK_SCHEDULER_DISABLED
    }//end of for(;;)
#endif

ENDI2CTASK:
#if !TASK_SCHEDULER_DISABLED
    vTaskDelete(NULL);
#else
    ;
#endif
}

VOID    
I2CTestApp(
    IN  VOID    *I2CTestData
){

    PI2C_VERI_PARA pI2CVerParaTmp = (PI2C_VERI_PARA)I2CTestData;
    
    switch(pI2CVerParaTmp->VeriProcCmd)
    {
        //4 I2C_TEST_INIT
        case I2C_TEST_INIT:
            {
                I2CVeriGCnt = 0;
#if TASK_SCHEDULER_DISABLED
                pI2CVeriAdp = (PI2C_VERI_PARA)I2CTestData;
                I2CTestProc(pI2CVeriAdp);
#else
                u32 I2CTmpSts;
                pI2CVeriAdp = NULL;
                pI2CVeriAdp = (PI2C_VERI_PARA)RtlZmalloc(sizeof(I2C_VERI_PARA));
                RtlMemcpy((void*)pI2CVeriAdp,(void*)I2CTestData,6*(sizeof(u32)));//only get the first 6 data from Uart console
                RtlInitSema(&(pI2CVeriAdp->VeriSema),0);
                pI2CVeriAdp->VeriStop = _FALSE;
                I2CTmpSts = xTaskCreate( I2CTestProc, (const signed char *)"I2CVERI_TASK", 
                                    ((1024*4)/sizeof(portBASE_TYPE)), (void *)pI2CVeriAdp, 1, pI2CVeriAdp->I2CTask);
#endif
                break;
            }
        //4 I2C_TEST_SWCH
        case I2C_TEST_SWCH:
            {
#if TASK_SCHEDULER_DISABLED
                DBG_8195A_I2C_LVL(VERI_I2C_LVL,"This command is ONLY applicable when scheduler enabled.\n");         
#else
                if (pI2CVerParaTmp->VeriItem == ON)    //turn on the verification
                {
                    pI2CVeriAdp->VeriStop = _FALSE;
                    RtlUpSema(&(pI2CVeriAdp->VeriSema));
                }
                else                                //turn off the verification
                {
                    pI2CVeriAdp->VeriStop = _TRUE;
                }
#endif
                break;
            }
        //4 I2C_TEST_DEINIT
        case I2C_TEST_DEINIT:
            {
#if TASK_SCHEDULER_DISABLED
                DBG_8195A_I2C_LVL(VERI_I2C_LVL,"This command is ONLY applicable when scheduler enabled.\n");         
#else
                //4 force the task goes to end
                pI2CVeriAdp->VeriItem = I2C_TEST_N_DEINT;
                RtlUpSema(&(pI2CVeriAdp->VeriSema));
                
                //4 free the task semaphore
                RtlFreeSema(&(pI2CVeriAdp->VeriSema));
                pI2CVeriAdp->VeriSema = NULL;

                //4 free the adaptor
                RtlMfree((u8 *)pI2CVeriAdp,sizeof(I2C_VERI_PARA));
                pI2CVeriAdp = NULL;
#endif
                break;
            }
        //4 I2C_TEST_GETCNT
        case I2C_TEST_GETCNT:
            {
                DBG_8195A("    [i2c_DBG]:Total Verification Loop:%x\n",I2CVeriGCnt);
                break;
            }
        //4 default
        default:
            break;
    }
}

