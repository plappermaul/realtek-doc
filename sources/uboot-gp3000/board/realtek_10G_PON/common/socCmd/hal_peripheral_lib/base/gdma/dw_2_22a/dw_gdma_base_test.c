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
//#include "dw_hal_gdma.h"
#include "peripheral.h"
#include "rand.h"
#include "section_config.h"

#include "common.h"

#define CACHE_COHERENCE       // this is bug ??????  20151123
#define REG32(reg)      (*(volatile u32 *)(reg))
//#define RUN_IN_SRAM

//#define GATHER_SCATTER_TEST       // needed if test gatter or scatter

//#define UNCACHE_DRAM          // maybe no use 20151123

//#define RUN_IN_98F_SRAM   // 20170616

//extern VOID
//InfraStart(VOID);

/* ARM GIC Setting */
#define GIC_SPI_PENDING		0x4f8000284
#ifdef CONFIG_TARGET_RTK_ELNATH
#define HW_DW_GDMA_IP_IRQ	29
#else
#define HW_DW_GDMA_IP_IRQ	31
#endif
#define GIC_SPI_PENDING_GDMA    (1<<HW_DW_GDMA_IP_IRQ)
#define IRQ_WAIT_FOR_TRANS	200

#define DATA_SIZE				2048
#define BLOCK_SIZE				128
#define ENABLE	    				1
#define GDMA_IRQ_STATUS		1
#define FLYSKY_DEBUG			1
#define DW_GDMA_IP_IRQ		(HW_DW_GDMA_IP_IRQ + 32)

#define GDMA_IRQ_REQUEST_EN 0



#define VIR_2_PHY(Addr) 	       (Addr)
#if defined(__96F_PHERIPHERAL_H__) //96F 97F
#define VIR_2_PHY_2(Addr) 	(Addr & (~0xA0000000))
#elif defined(__97G_PHERIPHERAL_H__) //defined (SOC_TYPE_8197G)
#define VIR_2_PHY_2(Addr) 	(Addr & (~0xA0000000))
#elif defined(__98F_PHERIPHERAL_H__) //defined (SOC_TYPE_8198F)
#define VIR_2_PHY_2(Addr) 	       (Addr)
#elif defined(__98X_PHERIPHERAL_H__) //defined (SOC_TYPE_8198X)
#define VIR_2_PHY_2(Addr)              (Addr)
#endif


/*Highlight the address for debug used*/

#define DEBUG_WITH_COLOR

#ifdef DEBUG_WITH_COLOR
#define ADDR_COLOR  "\033[1;30m "
#define ADDR_COLOR_END  "\033[0m "
#define ERROR_COLOR  "\033[1;31m"
#define ERROR_COLOR_END  "\033[0m"
#else
#define ADDR_COLOR
#define ADDR_COLOR_END
#define ERROR_COLOR
#define ERROR_COLOR_END
#endif


typedef struct _GDMA_ADAPTER_ {
    PHAL_GDMA_OP pHalGdmaOp;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
}GDMA_ADAPTER, *PGDMA_ADAPTER;

typedef struct _GDMA_LLP_BUF_ {
    u8  LliTest[BLOCK_SIZE];
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


GDMA_ADAPTER            GdmaAdapte;
GDMA_VERIFY_PARAMETER   GdmaVerifyPere;




#ifdef	RUN_IN_SRAM
SRAM_BD_DATA_SECTION
u8*  BDSrcTest;//[DATA_SIZE];
SRAM_BD_DATA_SECTION
u8*  BDDstTest;//[DATA_SIZE];

SRAM_BF_DATA_SECTION
u8*  BFSrcTest;//[DATA_SIZE];
SRAM_BF_DATA_SECTION
u8*  BFDstTest;//[DATA_SIZE];
#else
SRAM_BD_DATA_SECTION
u8 BDSrcTest[DATA_SIZE] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
#if 0 //def CACHE_COHERENCE
SRAM_BD_DATA_SECTION
u8* BDDstTest;
#else
SRAM_BD_DATA_SECTION
//u8 BDDstTest[DATA_SIZE] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
u8 BDDstTest[DATA_SIZE*2] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));

#endif

SRAM_BF_DATA_SECTION
u8 BFSrcTest[DATA_SIZE] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
SRAM_BF_DATA_SECTION
u8 BFDstTest[DATA_SIZE*2] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
#endif

HAL_GDMA_OP HalGdmaOp;
HAL_GDMA_ADAPTER HalGdmaAdapter;

#define LLP_NUM     16

volatile GDMA_CH_LLI_ELE GdmaChLli[LLP_NUM];
struct GDMA_CH_LLI Lli[LLP_NUM];
struct BLOCK_SIZE_LIST BlockSizeList[LLP_NUM];

volatile BOOL gDataCheckValue = _TRUE;

SRAM_BD_DATA_SECTION
GDMA_LLP_BUF BDSrcLliTest[LLP_NUM] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
SRAM_BD_DATA_SECTION
GDMA_LLP_BUF BDDstLliTest[LLP_NUM] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));

SRAM_BF_DATA_SECTION
GDMA_LLP_BUF BFSrcLliTest[LLP_NUM] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));
SRAM_BF_DATA_SECTION
GDMA_LLP_BUF BFDstLliTest[LLP_NUM] __attribute__((aligned(CONFIG_SYS_CACHELINE_SIZE)));


extern void * memset( void *s, int c, size_t n );

typedef enum _GDMA_TEST_CH_NUM_{
    Gdma_CH_0	= 0,
    Gdma_CH_1	= 1,
    Gdma_CH_2	= 2,
    Gdma_CH_3	= 3,
#if 0 // Note: there are  4 channels in 8196F
    Gdma_CH_4	= 4,
    Gdma_CH_5	= 5,
    Gdma_CH_6	= 6,
    Gdma_CH_7	= 7
#endif
}GDMA_TEST_CH_NUM;

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
    LlpSarLlpDar    = 10,
    SignalBlockDsr  = 11,
    SignalBlockSgr  =12,
}GDMA_TEST_ITEM_TYPE, *PGDMA_TEST_ITEM_TYPE;


typedef enum _GDMA_MEMORY_TEST_TYPE_ {
    BdSramToBdSram = 0,
    BdSramToBfSram = 1,
    BfSramToBfSram = 2,
    BfSramToBdSram = 3,
    SpiflashToBdSram = 4
}GDMA_MEMORY_TEST_TYPE;


typedef enum _GDMA_TEST_CONFIG_ {
    SeflSetTest = 0,
    PartAutoTest = 1,
    FullAutoTest = 2
}GDMA_TEST_CONFIG, *PGDMA_TEST_CONFIG;

typedef enum _GDMA_TEST_PATTERN_ {
    Pattern5A = 0,
    PatternIncremental = 1
}GDMA_TEST_PATTERN, *PGDMA_TEST_PATTERN;

//================ Static Function ==============================
VOID
GdmaVerifyCtrlStart(
    IN  VOID      *Data
);

VOID
Gdma0CombineIrqHandle
(
    IN  VOID        *Data
);

VOID
Gdma0Ch0IrqHandle
(
    VOID * Data
);

VOID
Gdma0Ch4IrqHandle
(
    IN  VOID        *Data
);

    VOID
Gdma0Ch5IrqHandle
(
    IN  VOID        *Data
);


BOOL
GdmaDataCompare
(
    IN  u8      *pSrc,
    IN  u8      *pDst,
    IN  u32     PacketLen
);

VOID
Gdma0Ch3IrqHandle_SGR_DSR
(
    IN  VOID        *Data
);
//=========================================================

#if 0
VOID
InfraStart(VOID)
{
    //3 1) Initial Prestart function
    //3 2) Enter App start function
    //3 3)Enable Schedule
//printf("%s(%d) Flysky\n", __FUNCTION__, __LINE__);
}
#endif

VOID
GDMAPolinStatus() //Flysky
{
	if(GDMA_IRQ_STATUS==0)
	{
		while(BIT_INVC_CTLX_UP_DONE==0)
		{
			udelay(1000);
		}
		//printf("%s(%d) Flysky\n", __FUNCTION__, __LINE__);
	}
}

void GDMAPolinIRQ_GIC( PGDMA_ADAPTER Data, PIRQ_HANDLE irq_handle){
	u32 timeout = 1000;
		printf("%s: Check HW IRQ(%d) up.\n",__func__, DW_GDMA_IP_IRQ );
	while ( timeout && ((readl(GIC_SPI_PENDING) & GIC_SPI_PENDING_GDMA) == 0)){
		mdelay(1);
		timeout--;
	}
	if(timeout){
		printf("Check HW IRQ(%d) up.\n",DW_GDMA_IP_IRQ );
		//Gdma0CombineIrqHandle(Data);
		irq_handle->IrqFun(Data);
 		mdelay(IRQ_WAIT_FOR_TRANS);
		if(readl(GIC_SPI_PENDING) & GIC_SPI_PENDING_GDMA){
				GDMAPolinIRQ_GIC(Data, irq_handle);
		}
	}else{
		printf("Error: HW IRQ(%d) isn't triggered. timeout!\n",DW_GDMA_IP_IRQ );
		gDataCheckValue = _FALSE;
	}
	return;
}

void GDMA_SGR_DSR_DATA(u32 *p, u32 leng){
    int i;
    for(i=0;i<(leng/sizeof(int));i++){
        *p =( i | 0xC0000000);
        p++;
    }
}

VOID
GdmaTestApp(
    IN  VOID      *Data
)
{
    u32      BuffIndex = 0;
    u32      RandValue, VerifyLoopIndex, MemInitIndex;

    PGDMA_VERIFY_PARAMETER pTempPara = (PGDMA_VERIFY_PARAMETER)Data;
    PGDMA_VERIFY_PARAMETER pGdmaVerifyPara = (PGDMA_VERIFY_PARAMETER)&GdmaVerifyPere;

#if 1 //FLYSKY_DEBUG
    printf("GdmaAutoTest = %d\n", pTempPara->GdmaAutoTest);
    printf("GdmaMemoryTestType = %d\n", pTempPara->GdmaMemoryTestType);
    printf("GdmaType = %d\n", pTempPara->GdmaType);
    printf("GdmaIndex = %d\n", pTempPara->GdmaIndex);
    printf("GdamPattern = %d\n", pTempPara->GdamPattern);
    printf("Rsvd = %d\n", pTempPara->Rsvd);
    printf("GdmaTransferLen = %d\n", pTempPara->GdmaTransferLen);
    printf("GdmaVerifyLoop = %d\n", pTempPara->GdmaVerifyLoop);
    printf("pSrc = 0x%x\n", pTempPara->pSrc);
    printf("pDst = 0x%x\n", pTempPara->pDst);
    printf("pSrcLli = %d\n", pTempPara->pSrcLli);
    printf("pDstLli = %d\n", pTempPara->pDstLli);
    printf("MaxLlp = %d\n", pTempPara->MaxLlp);
#endif

#ifdef	RUN_IN_SRAM
#if 0
BDSrcTest = (u8*)0xBFE00800;
BDDstTest = (u8*)0xBFE01000;
BFSrcTest = (u8*)0xBFE01800;
BFDstTest = (u8*)0xBFE02000;
#else
BDSrcTest = (u8*)0x82E00800;
BDDstTest = (u8*)0x82E01000;
BFSrcTest = (u8*)0x82E01800;
BFDstTest = (u8*)0x82E02000;
#endif

#if 0 //FLYSKY_DEBUG
        printf("BDSrcTest: 0x%8x\n", &BDSrcTest);
        printf("BDDstTest: 0x%8x\n", &BDDstTest);
        printf("BFSrcTest: 0x%8x\n", &BFSrcTest);
        printf("BFDstTest: 0x%8x\n", &BFDstTest);
#endif
#endif

#ifdef UNCACHE_DRAM
#if 0
u8* pBDSrcTest = (u8 *)((u32)(&BDSrcTest[0]) | 0x20000000);
u8* pBDDstTest = (u8 *)((u32)(&BDDstTest[0]) | 0x20000000);
u8* pBFSrcTest = (u8 *)((u32)(&BFSrcTest[0]) | 0x20000000);
u8* pBFDstTest = (u8 *)((u32)(&BFDstTest[0]) | 0x20000000);
#else
//0x05000000
u8* pBDSrcTest = (u8 *)((u32)(&BDSrcTest[0]) | 0x00500000);
u8* pBDDstTest = (u8 *)((u32)(&BDDstTest[0]) | 0x00500000);
u8* pBFSrcTest = (u8 *)((u32)(&BFSrcTest[0]) | 0x00500000);
u8* pBFDstTest = (u8 *)((u32)(&BFDstTest[0]) | 0x00500000);
#endif
#if 0 //FLYSKY_DEBUG
	printf("BD Source LLp0 Addr: 0x%x\n", &BDSrcLliTest[0]);
	printf("BD Dest LLp0 Addr: 0x%x\n", &BDDstLliTest[0]);
	printf("BF Source LLp0 Addr: 0x%x\n", &BFSrcLliTest[0]);
	printf("BF Dest LLp0 Addr: 0x%x\n", &BFDstLliTest[0]);
#endif
#if 0
u8* pBDSrcLliTest = (u8 *)((u32)(&BDSrcLliTest[0]) | 0x20000000);
u8* pBDDstLliTest = (u8 *)((u32)(&BDDstLliTest[0]) | 0x20000000);
u8* pBFSrcLliTest = (u8 *)((u32)(&BFSrcLliTest[0]) | 0x20000000);
u8* pBFDstLliTest = (u8 *)((u32)(&BFDstLliTest[0]) | 0x20000000);
#else
u8* pBDSrcLliTest = (u8 *)((u32)(&BDSrcLliTest[0]) | 0x00500000);
u8* pBDDstLliTest = (u8 *)((u32)(&BDDstLliTest[0]) | 0x00500000);
u8* pBFSrcLliTest = (u8 *)((u32)(&BFSrcLliTest[0]) | 0x00500000);
u8* pBFDstLliTest = (u8 *)((u32)(&BFDstLliTest[0]) | 0x00500000);
#endif
#if 0 //FLYSKY_DEBUG
	printf("BD Source LLp0 Addr: 0x%x\n", &BDSrcLliTest[0]);
	printf("BD Dest LLp0 Addr: 0x%x\n", &BDDstLliTest[0]);
	printf("BF Source LLp0 Addr: 0x%x\n", &BFSrcLliTest[0]);
	printf("BF Dest LLp0 Addr: 0x%x\n", &BFDstLliTest[0]);
#endif
#endif

#ifdef RUN_IN_98F_SRAM
u8* pBDDstTest = (u8 *)((u32)SRAM_BASE);
#endif

#if FLYSKY_DEBUG
    printf("(%d): Address: 0x%x, 0x%x, 0x%x, 0x%x \n", __LINE__, &BDSrcTest, &BDDstTest, &BFSrcTest, &BFDstTest);
#endif


    gDataCheckValue = _TRUE;

    pGdmaVerifyPara->GdmaAutoTest = pTempPara->GdmaAutoTest;
    pGdmaVerifyPara->GdmaVerifyLoop = pTempPara->GdmaVerifyLoop;

#if 0 //def CACHE_COHERENCE  // Carl 20141009
printf("%d - before align (u32)BDDst=0x%08x\n",__LINE__,(u32)BDDstTest);

ALLOC_CACHE_ALIGN_BUFFER(u8,Dst_Temp,DATA_SIZE);
BDDstTest = Dst_Temp;

printf("%d - align (u32)BDDst=0x%08x\n",__LINE__,(u32)BDDstTest);
#endif


    for(VerifyLoopIndex=0; VerifyLoopIndex<pGdmaVerifyPara->GdmaVerifyLoop; VerifyLoopIndex++) {
        if (gDataCheckValue) {
            switch (pGdmaVerifyPara->GdmaAutoTest) {
                case FullAutoTest: {
                        RandValue = Rand()&0xF;

                        pGdmaVerifyPara->GdmaMemoryTestType = (GDMA_MEMORY_TEST_TYPE)(RandValue & 0x7);
                        pGdmaVerifyPara->GdmaType = RandValue%10;
                        if (!pGdmaVerifyPara->GdmaType) {
                            pGdmaVerifyPara->GdmaType = 1;
                        }
                        pGdmaVerifyPara->GdmaIndex = Rand() & BIT_(0);
                         pGdmaVerifyPara->GdamPattern = Rand() & BIT_(0);
                    }
                    break;
                case PartAutoTest:
                case SeflSetTest: {
                        if (PartAutoTest == pGdmaVerifyPara->GdmaAutoTest) {
                            RandValue = Rand()&0xF;
                            pGdmaVerifyPara->GdmaMemoryTestType = (GDMA_MEMORY_TEST_TYPE)(RandValue & 0x7);
                            pGdmaVerifyPara->GdamPattern = pTempPara->GdamPattern;

                        }
                        else {
                            RandValue = pTempPara->GdmaTransferLen;
                            pGdmaVerifyPara->GdmaMemoryTestType = (GDMA_MEMORY_TEST_TYPE)
                                                                (pTempPara->GdmaMemoryTestType & 0x7);
                            pGdmaVerifyPara->GdamPattern = pTempPara->GdamPattern;
                        }

                        pGdmaVerifyPara->GdmaType = pTempPara->GdmaType;
                        pGdmaVerifyPara->GdmaIndex = pTempPara->GdmaIndex;
                    }
                break;
            }

            // Max Length 2k
            pGdmaVerifyPara->GdmaTransferLen = (0x7FF & RandValue) & (~0x3);

           while (!pGdmaVerifyPara->GdmaTransferLen) {
#if FLYSKY_DEBUG
                //printf("Transfer Length == 0\n");
#endif
                RandValue = Rand();
                pGdmaVerifyPara->GdmaTransferLen = (0x7FF & RandValue) & (~0x3);
            }
            pGdmaVerifyPara->MaxLlp = pGdmaVerifyPara->GdmaTransferLen>>7;

            if  (SignalBlock == pGdmaVerifyPara->GdmaType) {
                pGdmaVerifyPara->MaxLlp = 1;
            }
            else {
                if (pGdmaVerifyPara->GdmaTransferLen & 0x7F) {
                    pGdmaVerifyPara->MaxLlp += 1;
                }
            }

#if FLYSKY_DEBUG
            printf("(%d)Loop Index: %d; Test Item: %d\n", __LINE__, VerifyLoopIndex, pGdmaVerifyPara->GdmaType);
            printf("(%d)BD Source Addr: 0x%x; BF Source Addr: 0x%x\n", __LINE__, BDSrcTest, BFSrcTest);
            printf("(%d)BD Dest Addr: 0x%x; BF Dest Addr: 0x%x\n", __LINE__, BDDstTest, BFDstTest);


            printf("(%d)GdmaType: %d; Memory Test: %d; Transfer Len: %d; Max Block Size: %d\n", __LINE__,
                            pGdmaVerifyPara->GdmaType, pGdmaVerifyPara->GdmaMemoryTestType,
                            pGdmaVerifyPara->GdmaTransferLen, pGdmaVerifyPara->MaxLlp);
#endif
            switch (pGdmaVerifyPara->GdamPattern) {
                case Pattern5A: {
#ifdef	UNCACHE_DRAM
                        memset(pBDSrcTest, 0x5A, DATA_SIZE);
                        memset(pBFSrcTest, 0x5A, DATA_SIZE);

			for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                            memset( (pBDSrcLliTest + sizeof(GDMA_LLP_BUF)*BuffIndex) , 0x5A+BuffIndex, BLOCK_SIZE);
                            memset( (pBFSrcLliTest + sizeof(GDMA_LLP_BUF)*BuffIndex) , 0x5A+BuffIndex, BLOCK_SIZE);
                        }
#else
                        memset(BDSrcTest, 0x5A, DATA_SIZE);
                        memset(BFSrcTest, 0x5A, DATA_SIZE);

				flush_cache(BDSrcTest, BDSrcTest+DATA_SIZE);
				flush_cache(BFSrcTest, BFSrcTest+DATA_SIZE);
                        for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                            memset(BDSrcLliTest[BuffIndex].LliTest, 0x5A+BuffIndex, BLOCK_SIZE);
                            memset(BFSrcLliTest[BuffIndex].LliTest, 0x5A+BuffIndex, BLOCK_SIZE);
                        }
#endif
                    }
                    break;
                case PatternIncremental: {
                        RandValue = Rand()&0xFF;
                        for (MemInitIndex=0; MemInitIndex<DATA_SIZE; MemInitIndex++) {
#ifdef	UNCACHE_DRAM
				*(pBDSrcTest + MemInitIndex) = (RandValue + MemInitIndex) & 0xFF;
				*(pBFSrcTest  + MemInitIndex)  = (RandValue + MemInitIndex) & 0xFF;
#else
                            BDSrcTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
                            BFSrcTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
#endif
                        }
#if FLYSKY_DEBUG
                        printf("(%d)BD Dest Addr: 0x%x; BF Dest Addr: 0x%x\n", __LINE__, BDDstTest, BFDstTest);
#endif
                        for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                            RandValue += BuffIndex;
				  if(3 == BuffIndex) break;
                            for (MemInitIndex=0; MemInitIndex<DATA_SIZE; MemInitIndex++) {
					if(3 == MemInitIndex) break;
#if FLYSKY_DEBUG
			printf("(%d)BDSrcLliTest[%d]=0x%x, .LliTest[%d]=0x%x\n",__LINE__, BuffIndex, &BDSrcLliTest[BuffIndex], MemInitIndex, &(BDSrcLliTest[BuffIndex].LliTest[MemInitIndex]));
#endif
					//BDSrcLliTest[BuffIndex].LliTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
                                 //BFSrcLliTest[BuffIndex].LliTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
                            }
                        }
#if FLYSKY_DEBUG
			printf("(%d)BD Dest Addr: 0x%x; BF Dest Addr: 0x%x\n", __LINE__, BDDstTest, BFDstTest);
#endif
#ifdef	UNCACHE_DRAM
                        for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                            RandValue += BuffIndex;
                            for (MemInitIndex=0; MemInitIndex<BLOCK_SIZE; MemInitIndex++) {	//BLOCK_SIZE Flysky
				     *(pBDSrcLliTest+ sizeof(GDMA_LLP_BUF)*BuffIndex + MemInitIndex) = (RandValue + MemInitIndex) & 0xFF;
				     *(pBFSrcLliTest+ sizeof(GDMA_LLP_BUF)*BuffIndex + MemInitIndex)  = (RandValue + MemInitIndex) & 0xFF;
                            }
                        }
#else
                        for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                            RandValue += BuffIndex;
                            for (MemInitIndex=0; MemInitIndex<BLOCK_SIZE; MemInitIndex++) { // TODO: why DATA_SIZE??? it should be BLOCK_SIZE
                                BDSrcLliTest[BuffIndex].LliTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
                                BFSrcLliTest[BuffIndex].LliTest[MemInitIndex] = (RandValue + MemInitIndex) & 0xFF;
                            }
                        }
#endif
#if FLYSKY_DEBUG
			printf("(%d)BD Dest Addr: 0x%x; BF Dest Addr: 0x%x\n", __LINE__, BDDstTest, BFDstTest);
#endif
                  }
                    break;
            }
#if FLYSKY_DEBUG
            printf("(%d)BD Source Addr: 0x%x; BF Source Addr: 0x%x\n", __LINE__, BDSrcTest, BFSrcTest);
            printf("(%d)BD Dest Addr: 0x%x; BF Dest Addr: 0x%x\n", __LINE__, BDDstTest, BFDstTest);
#endif

#ifdef UNCACHE_DRAM
            memset(pBDDstTest, 0x00, DATA_SIZE);
            memset(pBFDstTest, 0x00, DATA_SIZE);
#else
            memset(BDDstTest, 0x00, sizeof(BDDstTest));
            memset(BFDstTest, 0x00, sizeof(BFDstTest));

		flush_cache(BDDstTest, BDDstTest+DATA_SIZE);
		flush_cache(BFDstTest, BFDstTest+DATA_SIZE);
#endif

#if FLYSKY_DEBUG
            printf("(%d)Source LLp0 Addr: 0x%x\n", __LINE__, &BDSrcLliTest[0]);
            printf("(%d)Dest LLp0 Addr: 0x%x\n", __LINE__, &BDDstLliTest[0]);
#endif

#ifdef UNCACHE_DRAM
            for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                memset((pBDDstLliTest + sizeof(GDMA_LLP_BUF)*BuffIndex) , 0x00, BLOCK_SIZE);
                memset((pBFDstLliTest + sizeof(GDMA_LLP_BUF)*BuffIndex) , 0x00, BLOCK_SIZE);
                memset((void*)&GdmaChLli[BuffIndex], 0x00, sizeof(GDMA_CH_LLI_ELE));
            }
#else
            for (BuffIndex=0; BuffIndex< LLP_NUM; BuffIndex++) {
                memset(BDDstLliTest[BuffIndex].LliTest, 0x00, BLOCK_SIZE);
                memset(BFDstLliTest[BuffIndex].LliTest, 0x00, BLOCK_SIZE);
                memset((void*)&GdmaChLli[BuffIndex], 0x00, sizeof(GDMA_CH_LLI_ELE));
            }
#endif
            switch (pGdmaVerifyPara->GdmaMemoryTestType) {
                case BdSramToBdSram:
#ifdef UNCACHE_DRAM
                    pGdmaVerifyPara->pSrc = pBDSrcTest;
                    pGdmaVerifyPara->pDst = pBDDstTest;
                    pGdmaVerifyPara->pSrcLli = pBDSrcLliTest;
                    pGdmaVerifyPara->pDstLli = pBDDstLliTest;
#else
                    pGdmaVerifyPara->pSrc = BDSrcTest;
#ifdef RUN_IN_98F_SRAM
                    pGdmaVerifyPara->pDst = pBDDstTest;
#else
                    pGdmaVerifyPara->pDst = BDDstTest;
#endif
                    pGdmaVerifyPara->pSrcLli = BDSrcLliTest;
                    pGdmaVerifyPara->pDstLli = BDDstLliTest;
#endif
                    break;
                case BdSramToBfSram:
                    pGdmaVerifyPara->pSrc = BDSrcTest;
                    pGdmaVerifyPara->pDst = BFDstTest;
                    pGdmaVerifyPara->pSrcLli = BDSrcLliTest;
                    pGdmaVerifyPara->pDstLli = BFDstLliTest;
                    break;
                case BfSramToBfSram:
                    pGdmaVerifyPara->pSrc = BFSrcTest;
                    pGdmaVerifyPara->pDst = BFDstTest;
                    pGdmaVerifyPara->pSrcLli = BFSrcLliTest;
                    pGdmaVerifyPara->pDstLli = BFDstLliTest;
                    break;
                case BfSramToBdSram:
                    pGdmaVerifyPara->pSrc = BFSrcTest;
                    pGdmaVerifyPara->pDst = BDDstTest;
                    pGdmaVerifyPara->pSrcLli = BFSrcLliTest;
                    pGdmaVerifyPara->pDstLli = BDDstLliTest;
                    break;
                case SpiflashToBdSram:
                    pGdmaVerifyPara->pSrc = (u8*)SPI_FLASH_BASE;
                    pGdmaVerifyPara->pDst = BDDstTest;
                    pGdmaVerifyPara->pSrcLli = BFSrcLliTest;
                    pGdmaVerifyPara->pDstLli = BDDstLliTest;
                    break;
                default:
                    break;
            }

            GdmaVerifyCtrlStart(pGdmaVerifyPara); // Start ***************
	    //udelay(1000000);	/* 1000 ms */ // Flysky: let ISR first

#if FLYSKY_DEBUG
	    printf("(%d)Loop from 0~%d\n", __LINE__, VerifyLoopIndex);
#endif
        }
        else {
            printf("Fail\n");
            printf("Test Stop!!!! (%d)%s\n", __LINE__, __FUNCTION__);
            break;
        }
    }
}


/*Visualize link list items on console 
    show item's mem layout and point to next item if exist
*/

void show_llp_dma_decriptor(int llp_num, struct GDMA_CH_LLI *Lli, GDMA_CH_LLI_ELE *GdmaChLli)
{
	char out[100];
	int index=0;
	int printed_llp_num=0;
	int cur_print_llp_num=0;
	while( printed_llp_num < llp_num)
	{
		cur_print_llp_num = ((llp_num -printed_llp_num) >= 6) ? 6: (llp_num -printed_llp_num) ;
		printf("\n\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, " %-7s|%-10s|%-4s"," ","----------"," ");
			printf("%s",out);
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, "LLPX%d",index+printed_llp_num);
			printf(" %-7s|0x%08x|%-4s",out,Lli[index+printed_llp_num].pNextLli,"__" );
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, " %-7s|%-10s|  |%-1s"," "," "," ");
			printf("%s",out);
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, "DARX");
			printf(" %-7s|0x%08x|  |%-1s",out,GdmaChLli[index+printed_llp_num].Darx,"" );
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, " %-7s|%-10s|  |%-1s"," "," "," ");
			printf("%s",out);
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, "SARX");
			printf(" %-7s|0x%08x|  |%-1s",out,GdmaChLli[index+printed_llp_num].Sarx,"" );
		}
		printf("\n");
		for(index=0 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, " %-7s|%-10s|  |%-1s"," ","__________"," ");
			printf("%s",out);
		}
		printf("\n");
		sprintf(out, "%-8s0x%08x %-4s","",(unsigned long) (&Lli[printed_llp_num]),"   ---->");
		printf("%s",out);
		for(index=1 ; index < cur_print_llp_num ; index ++)
		{
			sprintf(out, " %-5s0x%08x%-4s","", (unsigned long) (&Lli[index+printed_llp_num]) ,"   ---->");
			printf("%s",out);
		}
		printed_llp_num += cur_print_llp_num;
	}
	printf("NULL\n\n");
}
VOID
GdmaVerifyCtrlStart(
    IN  VOID      *Data
)
{
    PGDMA_VERIFY_PARAMETER pGdmaVerifyPara = (PGDMA_VERIFY_PARAMETER)Data;
    PGDMA_ADAPTER       pGdmaAdapte;
    PHAL_GDMA_ADAPTER   pHalGdmaAdapter;
    PHAL_GDMA_OP        pHalGdmaOp;
#if GDMA_IRQ_STATUS
    IRQ_HANDLE          Gdma0IrqHandle;
    IRQ_HANDLE          Gdma4IrqHandle;
    IRQ_HANDLE          Gdma5IrqHandle;
	IRQ_HANDLE			*GdmaUsedIrqHandleP;
#endif
    GDMA_MEMORY_TEST_TYPE GdmaMemoryTestType;
    u32                 BuffIndex = 0, GdmaTransferLen, RandValue;
    u8                  MaxLlp, GdmaIndex, GdmaType;
    u8                  *pSrc = NULL, *pDst = NULL;
    PGDMA_LLP_BUF       pSrcLli = NULL, pDstLli = NULL;

    pGdmaAdapte = &GdmaAdapte;
    pGdmaAdapte->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter;
    pGdmaAdapte->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp;
    pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp;

    GdmaIndex = pGdmaVerifyPara->GdmaIndex;
    GdmaTransferLen = pGdmaVerifyPara->GdmaTransferLen;
    GdmaType = pGdmaVerifyPara->GdmaType;
    MaxLlp = pGdmaVerifyPara->MaxLlp;
    pSrc = pGdmaVerifyPara->pSrc;
    pDst = pGdmaVerifyPara->pDst;
    pSrcLli = pGdmaVerifyPara->pSrcLli;
    pDstLli = pGdmaVerifyPara->pDstLli;

    if (1 == MaxLlp) {
#if FLYSKY_DEBUG
	//printf("(%d)MaxLlp=1 , do single block test!\n",__LINE__);	// Flysky
#endif
        GdmaType = SignalBlock;
    }

    //3 Initial Gdma Hal Operation
    HalGdmaOpInit((VOID*) (pHalGdmaOp));
    pHalGdmaOp->HalGdamChInit((VOID*)(pHalGdmaAdapter));
    pHalGdmaAdapter->TestItem = GdmaType;

    switch (GdmaType) {
        case SignalBlock: {
                printf("\n\nTestItem 1: SignalBlock Test\n\n");

#if GDMA_IRQ_STATUS
                Gdma0IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma0IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch0IrqHandle
				GdmaUsedIrqHandleP = &Gdma0IrqHandle;
				printf("%s %d: %p , %p\n",  Gdma0IrqHandle.IrqFun, Gdma0CombineIrqHandle);
                Gdma0IrqHandle.Priority = 0;

                //InterruptRegister(&Gdma0IrqHandle);
                //InterruptEn(&Gdma0IrqHandle);
#if GDMA_IRQ_REQUEST_EN
		irq_install_handler(Gdma0IrqHandle.IrqNum, Gdma0IrqHandle.IrqFun, Gdma0IrqHandle.Data);     //20151123 marked for debug
#endif
#endif
                pHalGdmaAdapter->GdmaCtl.BlockSize = DATA_SIZE/4;
		  //pHalGdmaAdapter->GdmaCtl.BlockSize = 2;	// MY_STUDY
                pHalGdmaAdapter->GdmaCtl.Done = 1;
                pHalGdmaAdapter->ChSar = (u32)pSrc;
                pHalGdmaAdapter->ChDar = (u32)pDst;
#if FLYSKY_DEBUG
                printf("(%d): pHalGdmaAdapter->ChSar=0x%x, pHalGdmaAdapter->ChDar=0x%x\n", __LINE__, pHalGdmaAdapter->ChSar, pHalGdmaAdapter->ChDar);
#endif
                pHalGdmaAdapter->ChNum = 0; //0: ch0 for single block  , 2:ch2 for gather and scatter
#ifdef GATHER_SCATTER_TEST
                pHalGdmaAdapter->ChNum = 2;
#endif
                pHalGdmaAdapter->GdmaIndex = 0;
                pHalGdmaAdapter->ChEn = GdmaCh0;//GdmaCh0;
#ifdef GATHER_SCATTER_TEST
                pHalGdmaAdapter->ChEn = GdmaCh2;
#endif
                pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl = ENABLE;
                pHalGdmaAdapter->GdmaOnOff = ON;
                pHalGdmaAdapter->PacketLen = DATA_SIZE;	//Flysky
                //pHalGdmaAdapter->PacketLen = 8;	// MY_STUDY
		GDMAPolinStatus();  //Flysky
            }
            break;
        case ConSarAutoDar:{
                printf("\n\nTestItem 2: ConSarAutoDar Test\n\n");
#if GDMA_IRQ_STATUS
                Gdma4IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma4IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch4IrqHandle
                Gdma4IrqHandle.Priority = 0;
				GdmaUsedIrqHandleP = &Gdma4IrqHandle;
#if FLYSKY_DEBUG
		printf("(%d)ISR4 register\n", __LINE__);
#endif
                //InterruptRegister(&Gdma4IrqHandle);
                //InterruptEn(&Gdma4IrqHandle);
#if GDMA_IRQ_REQUEST_EN
		   irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data);
#endif
#endif
                pHalGdmaAdapter->GdmaCtl.BlockSize = BLOCK_SIZE/4;
                pHalGdmaAdapter->GdmaCtl.Done = 1;
                pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;
                pHalGdmaAdapter->MuliBlockCunt = 1;
                pHalGdmaAdapter->MaxMuliBlock = MaxLlp;
                pHalGdmaAdapter->ChSar = (u32)pSrc;
                pHalGdmaAdapter->ChDar = (u32)pDst;
                pHalGdmaAdapter->ChNum = 3;
                pHalGdmaAdapter->GdmaIndex = 0;
                pHalGdmaAdapter->ChEn = GdmaCh3;
                pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl = ENABLE;
                pHalGdmaAdapter->GdmaOnOff = ON;
                pHalGdmaAdapter->PacketLen = DATA_SIZE;
                pHalGdmaAdapter->BlockLen = BLOCK_SIZE;
		GDMAPolinStatus(); //Flysky
            }
            break;
        case AutoSarConDar: {
                printf("\n\nTestItem 3: AutoSarConDar Test\n\n");
#if GDMA_IRQ_STATUS
                Gdma4IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma4IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch4IrqHandle
                Gdma4IrqHandle.Priority = 0;
				GdmaUsedIrqHandleP = &Gdma4IrqHandle;
                //InterruptRegister(&Gdma4IrqHandle);
                //InterruptEn(&Gdma4IrqHandle);
#if GDMA_IRQ_REQUEST_EN
		  irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data);
#endif
#endif
                pHalGdmaAdapter->GdmaCtl.BlockSize = BLOCK_SIZE/4;
                pHalGdmaAdapter->GdmaCtl.Done = 1;
                pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;
                pHalGdmaAdapter->MuliBlockCunt = 1;
                pHalGdmaAdapter->MaxMuliBlock = MaxLlp;
                pHalGdmaAdapter->ChSar = (u32)pSrc;
                pHalGdmaAdapter->ChDar = (u32)pDst;
                pHalGdmaAdapter->ChNum = 3;
                pHalGdmaAdapter->GdmaIndex = 0;
                pHalGdmaAdapter->ChEn = GdmaCh3;
                pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl = ENABLE;
                pHalGdmaAdapter->GdmaOnOff = ON;
                pHalGdmaAdapter->PacketLen = DATA_SIZE;
                pHalGdmaAdapter->BlockLen = BLOCK_SIZE;
		GDMAPolinStatus(); //Flysky
            }
            break;
        case AutoSarAutoDar: {
                printf("\n\nTestItem 4: AutoSarAutoDar Test\n\n");
#if GDMA_IRQ_STATUS
                Gdma4IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma4IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch4IrqHandle
                Gdma4IrqHandle.Priority = 0;
				GdmaUsedIrqHandleP = &Gdma4IrqHandle;
                //InterruptRegister(&Gdma4IrqHandle);
                //InterruptEn(&Gdma4IrqHandle);
#if GDMA_IRQ_REQUEST_EN
		   irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data);
#endif
#endif
                pHalGdmaAdapter->GdmaCtl.BlockSize = BLOCK_SIZE/4;
                pHalGdmaAdapter->GdmaCtl.Done = 1;
                pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;
                pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;
                pHalGdmaAdapter->MuliBlockCunt = 1;
                pHalGdmaAdapter->MaxMuliBlock = MaxLlp;
                pHalGdmaAdapter->ChSar = (u32)pSrc;
                pHalGdmaAdapter->ChDar = (u32)pDst;
                pHalGdmaAdapter->ChNum = 3;
                pHalGdmaAdapter->GdmaIndex = 0;
                pHalGdmaAdapter->ChEn = GdmaCh3;
                pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl = ENABLE;
                pHalGdmaAdapter->GdmaOnOff = ON;
                pHalGdmaAdapter->PacketLen = DATA_SIZE;
                pHalGdmaAdapter->BlockLen = BLOCK_SIZE;
		GDMAPolinStatus(); //Flysky
            }
            break;

        case SignalBlockLlp:
        case ConSarLlpDar:
        case AutoSarLlpDar:
        case LlpSarConDar:
        case LlpSarAutoDar:
        case LlpSarLlpDar: {
                u32 LliIndex;
#if FLYSKY_DEBUG
                printf("(%d)Llp Related Test %d\n", __LINE__, GdmaType);
                printf("(%d)GdmaChLli 0: &GdmaChLli[0]=0x%x\n",__LINE__, &GdmaChLli[0]);
#endif
#if GDMA_IRQ_STATUS
                Gdma5IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma5IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma5IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch5IrqHandle
                Gdma5IrqHandle.Priority = 0;
				GdmaUsedIrqHandleP = &Gdma5IrqHandle;
                //InterruptRegister(&Gdma5IrqHandle);
                //InterruptEn(&Gdma5IrqHandle);
#if GDMA_IRQ_REQUEST_EN
		         irq_install_handler(Gdma5IrqHandle.IrqNum, Gdma5IrqHandle.IrqFun, Gdma5IrqHandle.Data);
#endif
#endif
                pHalGdmaAdapter->ChNum = 2;
                pHalGdmaAdapter->GdmaIndex = 0;
                pHalGdmaAdapter->ChEn = GdmaCh2;
                pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl = ENABLE;
                pHalGdmaAdapter->GdmaOnOff = ON;
                pHalGdmaAdapter->PacketLen = GdmaTransferLen;
                pHalGdmaAdapter->BlockLen = BLOCK_SIZE;
                pHalGdmaAdapter->Llpctrl = 1;
                pHalGdmaAdapter->MaxMuliBlock = MaxLlp;
                pHalGdmaAdapter->MuliBlockCunt = 1;
                pHalGdmaAdapter->GdmaCtl.Done = 1;
                pHalGdmaAdapter->ChSar = (u32)pSrcLli[0].LliTest;
                pHalGdmaAdapter->ChDar = (u32)pDstLli[0].LliTest;
		        GDMAPolinStatus(); //Flysky
                if (MaxLlp > 1) {
                    switch (GdmaType) {
                     case ConSarLlpDar:
                            printf("\n\nTestItem 6: ConSarLlpDar Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;
                            pHalGdmaAdapter->ChSar = (u32)pSrc;
                               break;
                     case AutoSarLlpDar:
                            printf("\n\nTestItem 7: AutoSarLlpDar Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;
                            pHalGdmaAdapter->ChSar = (u32)pSrc;
                            pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;
                            pHalGdmaAdapter->GdmaIsrType |= BlockType;//BlockType; TransferType
                                break;
                    case LlpSarConDar:
                            printf("\n\nTestItem 8: LlpSarConDar Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;
                            pHalGdmaAdapter->ChDar = (u32)pDst;
                                break;
                      case LlpSarAutoDar:
                            printf("\n\nTestItem 9: LlpSarAutoDar Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;
                            pHalGdmaAdapter->ChDar = (u32)pDst;
                            pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;
                            pHalGdmaAdapter->GdmaIsrType |= BlockType;//BlockType; TransferType
                                break;
                      case SignalBlockLlp:
                            printf("\n\nTestItem 5: SignalBlockLlp Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;
                            pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;
                                break;
                      case LlpSarLlpDar:
                            printf("\n\nTestItem 10: LlpSarLlpDar Test\n\n");
                            pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;
                            pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;
                                break;
                    }
                }
                else {
                    pHalGdmaAdapter->TestItem = SignalBlockLlp;
                }

                for (LliIndex = 0; LliIndex<MaxLlp; LliIndex++) {

                    BlockSizeList[LliIndex].pNextBlockSiz = &BlockSizeList[LliIndex+1];

                    if (pHalGdmaAdapter->GdmaCtl.LlpSrcEn) {
                        GdmaChLli[LliIndex].Sarx = VIR_2_PHY_2((u32)pSrcLli[LliIndex].LliTest);
                    }

                    if (pHalGdmaAdapter->GdmaCtl.LlpDstEn) {
                        GdmaChLli[LliIndex].Darx = VIR_2_PHY_2((u32)pDstLli[LliIndex].LliTest);
                    }
                    Lli[LliIndex].pLliEle = VIR_2_PHY((u32)(&GdmaChLli[LliIndex])); // Flysky add (u32)
                    if ((MaxLlp-1) == LliIndex) {
                        Lli[LliIndex].pNextLli = NULL;
                        BlockSizeList[LliIndex].BlockSize = (GdmaTransferLen - (BLOCK_SIZE*(MaxLlp-1)))>>2;
                        BlockSizeList[LliIndex].pNextBlockSiz = NULL;
                    }
                    else {
                        Lli[LliIndex].pNextLli = VIR_2_PHY((u32)(&Lli[LliIndex+1])); // Flysky add (u32)
                        BlockSizeList[LliIndex].BlockSize = BLOCK_SIZE>>2;
                        BlockSizeList[LliIndex].pNextBlockSiz = &BlockSizeList[LliIndex+1];
                    }

                }

                pHalGdmaAdapter->pLlix = VIR_2_PHY((u32)(&Lli)); // Flysky add (u32)
                pHalGdmaAdapter->pBlockSizeList = &BlockSizeList;
		show_llp_dma_decriptor(MaxLlp, Lli,  GdmaChLli);
	        	GDMAPolinStatus(); //Flysky
            }

            break;
        case SignalBlockDsr: {
                        printf("\n\nTestItem 11: SignalBlockDsr Test\n\n");
                        GDMA_SGR_DSR_DATA(BDSrcTest,  DATA_SIZE);
                        GDMA_SGR_DSR_DATA(BFSrcTest,  DATA_SIZE);
#if GDMA_IRQ_STATUS
                        Gdma0IrqHandle.Data = (u32) (pGdmaAdapte);
                        Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                        Gdma0IrqHandle.IrqFun = (IRQ_FUN) Gdma0Ch3IrqHandle_SGR_DSR;
                        Gdma0IrqHandle.Priority = 0;
						GdmaUsedIrqHandleP = &Gdma0IrqHandle;
#if GDMA_IRQ_REQUEST_EN
                irq_install_handler(Gdma0IrqHandle.IrqNum, Gdma0IrqHandle.IrqFun, Gdma0IrqHandle.Data);     //20151123 marked for debug
#endif
#endif
                        //pHalGdmaAdapter->GdmaCtl.BlockSize = DATA_SIZE/4;
                        /*  2048/((2+4)*4) ) *16 */
                       // 2048/64 = 32 (SECTION), 32*4(dsc)*4 = 512, so scatter 512B data will cross 2048B raneg.
                        pHalGdmaAdapter->GdmaCtl.BlockSize = 512/4;
                        pHalGdmaAdapter->GdmaCtl.Done = 1;

                        pHalGdmaAdapter->GdmaCtl.DstScatterEn = 1;
                        pHalGdmaAdapter->GdmaCtl.Dinc = 0;
                        pHalGdmaAdapter->GdmaDsr.Dsi =12; //Interval= 12 * 4(TRWidth) = 48
                        pHalGdmaAdapter->GdmaDsr.Dsc =4;//4*4(Trwidth)

                        pHalGdmaAdapter->ChSar = (u32)pSrc;
                        pHalGdmaAdapter->ChDar = (u32)pDst;

                        pHalGdmaAdapter->ChNum = 3; //ch3 , only 2,3 support
                        pHalGdmaAdapter->GdmaIndex = 0;
                        pHalGdmaAdapter->ChEn = GdmaCh3;//GdmaCh1;
                        pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
                        pHalGdmaAdapter->IsrCtrl = ENABLE;
                        pHalGdmaAdapter->GdmaOnOff = ON;
                        pHalGdmaAdapter->PacketLen = DATA_SIZE; //Flysky

			    printf("%-40s: 4 bytes \n","TR width");
			    printf("%-40s: %d bytes \n", "DSR DSI(Dest Scatter Increment)", pHalGdmaAdapter->GdmaDsr.Dsi*4);
			    printf("%-40s: %d bytes \n","DSR DSC(Dest Scatter Count)", pHalGdmaAdapter->GdmaDsr.Dsc*4);
			    printf("%-40s: %d bytes \n\n", "PacketLen", pHalGdmaAdapter->PacketLen);
                }
            break;
        case SignalBlockSgr:{
                        printf("\n\nTestItem 12: SignalBlockSgr Test\n\n");
                        GDMA_SGR_DSR_DATA(BDSrcTest,  DATA_SIZE);
                        GDMA_SGR_DSR_DATA(BFSrcTest,  DATA_SIZE);
#if GDMA_IRQ_STATUS
                        Gdma0IrqHandle.Data = (u32) (pGdmaAdapte);
                        Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                        Gdma0IrqHandle.IrqFun = (IRQ_FUN) Gdma0Ch3IrqHandle_SGR_DSR;
                        Gdma0IrqHandle.Priority = 0;
						GdmaUsedIrqHandleP = &Gdma0IrqHandle;
#if GDMA_IRQ_REQUEST_EN
                irq_install_handler(Gdma0IrqHandle.IrqNum, Gdma0IrqHandle.IrqFun, Gdma0IrqHandle.Data);     //20151123 marked for debug
#endif
#endif
                        //pHalGdmaAdapter->GdmaCtl.BlockSize = DATA_SIZE/4;
                        /*  2048/((2+4)*4) ) *16 */
                       // 2048/64 = 32 (SECTION), 32*4(dsc)*4 = 512, so scatter 512B data will cross 2048B raneg.
                        pHalGdmaAdapter->GdmaCtl.BlockSize = 512/4;
                        pHalGdmaAdapter->GdmaCtl.Done = 1;

                        pHalGdmaAdapter->GdmaCtl.SrcGatherEn = 1;
                        pHalGdmaAdapter->GdmaCtl.Sinc = 0;
                        pHalGdmaAdapter->GdmaSgr.Sgi =12; //Interval= 12 * 4(TRWidth) = 48
                        pHalGdmaAdapter->GdmaSgr.Sgc =4;//4*4(Trwidth)

                        pHalGdmaAdapter->ChSar = (u32)pSrc;
                        pHalGdmaAdapter->ChDar = (u32)pDst;

                        pHalGdmaAdapter->ChNum = 3; //ch3 , only 2,3 support
                        pHalGdmaAdapter->GdmaIndex = 0;
                        pHalGdmaAdapter->ChEn = GdmaCh3;//GdmaCh1;
                        pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
                        pHalGdmaAdapter->IsrCtrl = ENABLE;
                        pHalGdmaAdapter->GdmaOnOff = ON;
                        pHalGdmaAdapter->PacketLen = DATA_SIZE; //Flysky
                        printf("%-40s: 4 bytes \n", "TR width");
			    printf("%-40s: %d bytes \n", "SAR SGI(Src Gather Increment)", pHalGdmaAdapter->GdmaSgr.Sgi*4);
			    printf("%-40s: %d bytes \n", "SAR SGC(Src Gather Count)", pHalGdmaAdapter->GdmaSgr.Sgc*4);
			    printf("%-40s: %d bytes \n", "PacketLen", pHalGdmaAdapter->PacketLen);
                }
            break;

        default : {
                printf("Error Test Item %d\n",GdmaType);
            }
            break;

    }//    switch (TestItem)

    pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));

    if (pHalGdmaAdapter->Llpctrl) {/*  LINK-LIST */
        pHalGdmaOp->HalGdmaChBlockSeting((VOID*)(pHalGdmaAdapter));
    }
    else {
        pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
    }

#ifdef CACHE_COHERENCE  // Flysky
    flush_cache(BDSrcTest, DATA_SIZE*sizeof(u8));
#if FLYSKY_DEBUG
    printf("(%d) Before flush BDSrcLliTest addr=0x%08x, len=0x%x \n",__LINE__,(u32)BDSrcLliTest, sizeof(GDMA_LLP_BUF) * LLP_NUM);
#endif
    flush_cache(BDSrcLliTest, sizeof(GDMA_LLP_BUF) * LLP_NUM);  // BD or BF

    u32 start = (u32)BDDstTest & ~(ARCH_DMA_MINALIGN - 1);  // TODO: maybe error after alignment....??
    u32 end = ALIGN(start + DATA_SIZE, ARCH_DMA_MINALIGN);
#if FLYSKY_DEBUG
    printf("(%d): start=0x%x, end=0x%x \n", __LINE__, start, end);
#endif
    invalidate_dcache_range(start,end);
#if FLYSKY_DEBUG
    printf("(%d): BDDstLliTest=0x%x, ARCH_DMA_MINALIGN=0x%x \n", __LINE__, BDDstLliTest, ARCH_DMA_MINALIGN);
#endif
    start = (u32)BDDstLliTest & ~(ARCH_DMA_MINALIGN - 1);
    end = (u32)(BDDstLliTest + LLP_NUM);

    if (start != (u32)BDDstLliTest) {
        printf("(%d)start != BDDstLliTest\n", __LINE__);
    }
#if FLYSKY_DEBUG
    printf("(%d): start=0x%x, end=0x%x \n", __LINE__, start, end);
#endif
#if 1
    flush_cache(start, (end-start));
    invalidate_dcache_range(start, end);  // 20141222
#if FLYSKY_DEBUG
    printf("(%d) BDDstLliTest=0x%08x, &BDDstLliTest[0])=0x%08x\n",__LINE__, (u32)BDDstLliTest, (u32)(&BDDstLliTest[0]));
    printf("(%d) invalidate BDDstTest=0x%08x pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__, (u32)BDDstTest,(u32)pHalGdmaAdapter->ChDar,ARCH_DMA_MINALIGN,start,end);
#endif
#endif
#endif
#if FLYSKY_DEBUG
    //printf("(%d): srcReg:0x%x, dstReg:0x%x \n", __LINE__, REG32(0xb801b0b0), REG32(0xb801b0b8)); //8197G
#endif
#ifdef GATHER_SCATTER_TEST
    printf("(%d)Enter gather_and_scatter test, single block test is fail !\n",__LINE__);
#endif

#if 1 //ndef MY_STUDY
    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));
#if FLYSKY_DEBUG
   // printf("(%d): srcReg:0x%x, dstReg:0x%x \n", __LINE__, REG32(0xb801b0b0), REG32(0xb801b0b8)); //8197G
#endif
#endif

	GDMAPolinIRQ_GIC(pGdmaAdapte, GdmaUsedIrqHandleP);

}

VOID
Gdma0CombineIrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
#if FLYSKY_DEBUG
    printf("(%d)IRQ TEST\n",__LINE__);
    printf("(%d)Enter Gdma0 Combine ISr\n",__LINE__);
#endif
    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#if 0
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8  IsrTypeMap;

    printf("\nEnter Gdma0 Channel 0 ISr =====>\n");

    printf("GMDA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#if 0	// #ifdefine MY_STUDY


	       printf("0x%08x \n",HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 0x0));
		printf("0x%08x \n",HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 0x4));
		printf("0x%08x \n",HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 0x8));
		printf("0x%08x \n",HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, 0xc));

            HAL_GDMAX_WRITE32(pHalGdmaAdapter->GdmaIndex,
                              (REG_GDMA_CLEAR_INT_BASE),
                              (0x1)//(IsrStatus)// & (pHalGdmaAdapter->ChEn & 0xFF))	// need to clear, or will always ISR
                              );

#else
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
#if FLYSKY_DEBUG
    printf("Isr Type 0x%x\n",IsrTypeMap);
    printf("Sar=0x%08x , Dar=0x%08x\n",pHalGdmaAdapter->ChSar,pHalGdmaAdapter->ChDar);	// Flysky debug Msg 20140929
#endif
#endif

    printf("<===== Leave Gdma0 Channel 0 ISr\n");
#else
#ifdef GATHER_SCATTER_TEST
   			Gdma0Ch0IrqHandle(Data);
                    printf("(%d)Leave Gdma0 Combine ISr (gather_and_scatter config)\n",__LINE__);
                    return;
#endif
	switch(pHalGdmaAdapter->ChNum){
		case Gdma_CH_0:
   			Gdma0Ch0IrqHandle(Data);
			break;
   		case Gdma_CH_1:
   			Gdma0Ch1IrqHandle(Data);
			break;
		case Gdma_CH_3:
			Gdma0Ch3IrqHandle(Data);
			break;
		case Gdma_CH_2:
			Gdma0Ch2IrqHandle(Data);
			break;
	}

   printf("(%d)Leave Gdma0 Combine ISr (MY_STUDY)\n",__LINE__);

#endif
}



VOID
Gdma0Ch0IrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8  IsrTypeMap;

    printf("(%d)Enter Gdma0 Channel 0 ISr",__LINE__);

    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    printf("(%d)Isr Type 0x%x\n",__LINE__,IsrTypeMap);
#if FLYSKY_DEBUG
    printf("(%d)Sar=0x%08x , Dar=0x%08x\n",__LINE__,pHalGdmaAdapter->ChSar,pHalGdmaAdapter->ChDar);	// Flysky debug Msg 20140929
#endif
    if (!GdmaDataCompare((u8*)pHalGdmaAdapter->ChSar,
                          (u8*)pHalGdmaAdapter->ChDar,
                          pHalGdmaAdapter->PacketLen)) {
        printf("(%d)DMA Error: Check Data Error!!!\n",__LINE__);
        gDataCheckValue = _FALSE;
    }
    else {
        printf("(%d)DMA Data OK\n",__LINE__);
        printf("Pass\n");
    }


    printf("(%d)Leave Gdma0 Channel 0 ISr\n",__LINE__);
}


VOID
Gdma0Ch1IrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8  IsrTypeMap;

    printf("(%d)Enter Gdma0 Channel 1 ISr",__LINE__);

    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    printf("(%d)Isr Type 0x%x\n",__LINE__,IsrTypeMap);
#if FLYSKY_DEBUG
    printf("(%d)Sar=0x%08x , Dar=0x%08x\n",__LINE__,pHalGdmaAdapter->ChSar,pHalGdmaAdapter->ChDar);	// Flysky debug Msg 20140929
#endif
    if (!GdmaDataCompare((u8*)pHalGdmaAdapter->ChSar,
                          (u8*)pHalGdmaAdapter->ChDar,
                          pHalGdmaAdapter->PacketLen)) {
        printf("(%d)DMA Error: Check Data Error!!!\n",__LINE__);
        gDataCheckValue = _FALSE;
    }
    else {
        printf("(%d)DMA Data OK\n",__LINE__);
        printf("Pass\n");
    }


    printf("(%d)Leave %s  ISr\n",__LINE__, __func__);
}


VOID
Gdma0Ch3IrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8 *pSrc = NULL, *pDst = NULL;
    u8  IsrTypeMap = 0;
#if FLYSKY_DEBUG
    printf("(%d)Enter Gdma0 Channel 4 ISr\n",__LINE__);
    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case ConSarAutoDar: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    printf("(%d)Clean Auto Reload Dst\n",__LINE__);
                    HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)(pHalGdmaAdapter->ChSar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
        case AutoSarConDar: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    printf("(%d)Clean Auto Reload Src\n",__LINE__);
                    HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                }

                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)(pHalGdmaAdapter->ChDar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
            }
            break;
        case AutoSarAutoDar: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
                    printf("(%d)Clean Auto Reload Src and Dst\n",__LINE__);
                    HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                    HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
    }

#ifdef CACHE_COHERENCE

    u32 start1 = (u32)BDSrcTest & ~(ARCH_DMA_MINALIGN - 1);
    u32 end1 = ALIGN(start1+ DATA_SIZE, ARCH_DMA_MINALIGN);

    u32 start2 = (u32)BDDstTest & ~(ARCH_DMA_MINALIGN - 1);
    u32 end2 = ALIGN(start2 + DATA_SIZE, ARCH_DMA_MINALIGN);

    invalidate_dcache_range(start1,end1);
    invalidate_dcache_range(start2,end2);
#if FLYSKY_DEBUG
    printf("(%d) invalidate BDSrcTest=0x%08x pSrc=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDSrcTest,(u32)pHalGdmaAdapter->ChSar,ARCH_DMA_MINALIGN,start1,end1);
    printf("(%d) invalidate BDDstTest=0x%08x pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDDstTest,(u32)pHalGdmaAdapter->ChDar,ARCH_DMA_MINALIGN,start2,end2);
#endif
#endif

    printf("compare SRC=0x%x DST=0x%x!\n", pSrc, pDst);

    //3 Compare Data
    if (!GdmaDataCompare(pSrc,
                          pDst,
                          pHalGdmaAdapter->BlockLen)) {
        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__, pHalGdmaAdapter->MuliBlockCunt);

        gDataCheckValue = _FALSE;
    }
    else {
        if (AutoSarAutoDar == pHalGdmaAdapter->TestItem) {
            printf("SKIP line=%d !!!\n",__LINE__+1); // 20170510
            //memset(pSrc, (pSrc + BLOCK_SIZE*pHalGdmaAdapter->MuliBlockCunt), BLOCK_SIZE);
        }
#if FLYSKY_DEBUG
        printf("(%d)DMA Block %d Data OK\n",__LINE__, pHalGdmaAdapter->MuliBlockCunt);
#endif
        if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt){
        printf("(%d)no Dst 6B memset to 0!!!\n",__LINE__); // 20170510
        }else{
        //printf("D[%d]=0x%x SRAM[0]=0x%x\n",0,pDst[0],SOC_HAL_R8(SRAM_BASE, 0));
        //printf("D[%d]=0x%x SRAM[1]=0x%x\n",1,pDst[1],SOC_HAL_R8(SRAM_BASE, 1));
        printf("D[%d]=0x%x\n",2,pDst[2]);
        printf("D[%d]=0x%x\n",3,pDst[3]);
        printf("D[%d]=0x%x\n",4,pDst[4]);
        printf("D[%d]=0x%x\n",5,pDst[5]);
        printf("D[%d]=0x%x\n",6,pDst[6]);
        printf("D[%d]=0x%x\n",7,pDst[7]);
#if 0
        printf("Dst 6B memset to 0 (line %d) !!!\n",__LINE__+1); // 20170510
        memset(pDst, 0x00, 6);//memset(pDst, 0x00, BLOCK_SIZE);
        SOC_HAL_W8(SRAM_BASE, 0, 0x77);
        SOC_HAL_W8(SRAM_BASE, 1, 0x88);
#endif
#if 1
   BDSrcTest[0] = 0x94;
   BDSrcTest[1] = 0x87;
   flush_cache(BDSrcTest, DATA_SIZE*sizeof(u8));
   printf("(%d)BDSrcTest 2Bytes set to 0x94 0x87 !!!\n",__LINE__); // 20170510
#endif
        }
#if 0
        printf("D[%d]=0x%x SRAM[0]=0x%x\n",0,pDst[0],SOC_HAL_R8(SRAM_BASE, 0));
        printf("D[%d]=0x%x SRAM[1]=0x%x\n",1,pDst[1],SOC_HAL_R8(SRAM_BASE, 1));
        printf("D[%d]=0x%x\n",2,pDst[2]);
        printf("D[%d]=0x%x\n",3,pDst[3]);
        printf("D[%d]=0x%x\n",4,pDst[4]);
        printf("D[%d]=0x%x\n",5,pDst[5]);
        printf("D[%d]=0x%x\n",6,pDst[6]);
        printf("D[%d]=0x%x\n",7,pDst[7]);
#endif
        //memset(pSrc, 0x87, 1);//memset(pDst, 0x00, BLOCK_SIZE);
    }

    //3 Clear Pending ISR
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
    //IsrTypeMap = 0x2;

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
#if FLYSKY_DEBUG
        printf("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt);
#endif
        pHalGdmaAdapter->MuliBlockCunt++;
    }


    printf("(%d)Leave Gdma0 Channel 4 ISr\n",__LINE__);
}

VOID
Gdma0Ch2IrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    PGDMA_CH_LLI_ELE pGdmaChLliEle;
    struct GDMA_CH_LLI *pGdmaChLli = pHalGdmaAdapter->pLlix;
    struct BLOCK_SIZE_LIST *pBlockSizeList = pHalGdmaAdapter->pBlockSizeList;
    u32 TotalBlockSize = 0;
    u8  IsrTypeMap, BlockIndex;
    u8 *pSrc = NULL, *pDst = NULL;
#if FLYSKY_DEBUG
    printf("(%d)Enter Gdma0 Channel 2 ISr\n",__LINE__);
    /*printf("%s(%d): srcReg:0x%x, dstReg:0x%x \n", __FUNCTION__, __LINE__, REG32(0xb801b0b0), REG32(0xb801b0b8));*/

    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);

    printf("(%d)Sar=0x%08x , Dar=0x%08x\n",__LINE__,pHalGdmaAdapter->ChSar,pHalGdmaAdapter->ChDar);	// Flysky debug Msg
#endif


    if ((pHalGdmaAdapter->TestItem != AutoSarLlpDar) &&
        (pHalGdmaAdapter->TestItem != LlpSarAutoDar)) {
        IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
        printf("(%d)Isr Type 0x%x\n",__LINE__ ,IsrTypeMap);
    }

    switch (pHalGdmaAdapter->TestItem) {
        case AutoSarLlpDar:
        case LlpSarAutoDar: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == (pHalGdmaAdapter->MuliBlockCunt +1)) {   // fix bug 20170619 ???
                    printf("(%d)Clean Auto Reload Dst\n",__LINE__);
                    if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
                        HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter); // fix bug 20170619
                    }
                    else {
                        HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter); // fix debug 20170619
                    }
                }

                //Search the correct Llix
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;

                    if (BlockIndex == (pHalGdmaAdapter->MuliBlockCunt-1)) {
                        if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
                            pSrc = (u8*)pGdmaChLliEle->Sarx;
                            pDst = (u8*)pHalGdmaAdapter->ChDar;
                        }
                        else {
                            //AutoSarLlpDar
                            pSrc = (u8*)pHalGdmaAdapter->ChSar;
                            pDst = (u8*)pGdmaChLliEle->Darx;
                        }
#if 1   // marked this => 98F will PASS  20170619
                        if (!GdmaDataCompare(pSrc,
                                              pDst,
                                              (pBlockSizeList->BlockSize<<2))) {
                            printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);

                            gDataCheckValue = _FALSE;
                            break;
                        }
                        else {
                            printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                        }
#endif
#if 1 // invalid... 20170619 for 98F ARM cache
			     if ((pHalGdmaAdapter->MaxMuliBlock) != (pHalGdmaAdapter->MuliBlockCunt)){

			                    pGdmaChLli = pGdmaChLli->pNextLli;
			                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;

			                    pGdmaChLliEle = pGdmaChLli->pLliEle;

			                    if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
			                        pSrc = (u8*)pGdmaChLliEle->Sarx;
			                        pDst = (u8*)pHalGdmaAdapter->ChDar;
			                    }
			                    else {
			                        //AutoSarLlpDar
			                        pSrc = (u8*)pHalGdmaAdapter->ChSar;
			                        pDst = (u8*)pGdmaChLliEle->Darx;
			                    }

						u32 start1 = (u32)pSrc & ~(ARCH_DMA_MINALIGN - 1);
						u32 end1 = ALIGN(start1+ (pBlockSizeList->BlockSize<<2), ARCH_DMA_MINALIGN);

						u32 start2 = (u32)pDst & ~(ARCH_DMA_MINALIGN - 1);
						u32 end2 = ALIGN(start2+ (pBlockSizeList->BlockSize<<2), ARCH_DMA_MINALIGN);

						invalidate_dcache_range(start1,end1);
						invalidate_dcache_range(start2,end2);
#if FLYSKY_DEBUG
						printf("(%d) invalidate pSrc=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)pSrc,ARCH_DMA_MINALIGN,start1,end1);
						printf("(%d) invalidate pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)pDst,ARCH_DMA_MINALIGN,start2,end2);
#endif
			    }
#endif
                        break;
                    }
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }

                IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
                printf("(%d)Isr Type 0x%x\n",__LINE__ ,IsrTypeMap);


                //3 Maintain Block Count
                if (IsrTypeMap & BlockType) {
#if FLYSKY_DEBUG
                    printf("(%d)DMA Block %d\n",__LINE__,pHalGdmaAdapter->MuliBlockCunt);
#endif
                    pHalGdmaAdapter->MuliBlockCunt++;
                }


            }
            break;
        case ConSarLlpDar: {
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
                    if (!GdmaDataCompare((u8*)(pHalGdmaAdapter->ChSar + TotalBlockSize),
                                          (u8*)pGdmaChLliEle->Darx,
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                    }
                    TotalBlockSize += (pBlockSizeList->BlockSize<<2);
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }
            }
            break;
        case LlpSarConDar:
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
#if FLYSKY_DEBUG
                    printf("(%d)Sar: 0x%x; Dst: 0x%x\n",__LINE__, pGdmaChLliEle->Sarx,(pHalGdmaAdapter->ChDar + TotalBlockSize));
#endif
                    if (!GdmaDataCompare((u8*)pGdmaChLliEle->Sarx,
                                          (u8*)(pHalGdmaAdapter->ChDar + TotalBlockSize),
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                    }

                    TotalBlockSize += (pBlockSizeList->BlockSize<<2);
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }

            break;
        case LlpSarLlpDar:
        case SignalBlockLlp:
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
#if FLYSKY_DEBUG
                    printf("%s(%d): 0x%x, 0x%x\n", __FUNCTION__, __LINE__, pGdmaChLliEle->Sarx, pGdmaChLliEle->Darx);
#endif
#if 0
                    printf("%s(%d): 0x%x, 0x%x\n", __FUNCTION__, __LINE__,
                                REG32(pGdmaChLliEle->Sarx|0x20000000), REG32(pGdmaChLliEle->Darx|0x20000000));

                    printf("%s(%d): 0x%x, 0x%x, 0x%x, 0x%x\n", __FUNCTION__, __LINE__,
                                pGdmaChLliEle->Sarx, pGdmaChLliEle->Darx,
                                REG32(pGdmaChLliEle->Sarx), REG32(pGdmaChLliEle->Darx));
#endif

                    if (!GdmaDataCompare((u8*)pGdmaChLliEle->Sarx,
                                          (u8*)pGdmaChLliEle->Darx,
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                    }
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }
            break;
    }




    printf("(%d)Leave Gdma0 Channel 2 ISr\n",__LINE__);

}



VOID
Gdma0Ch5IrqHandle
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    PGDMA_CH_LLI_ELE pGdmaChLliEle;
    struct GDMA_CH_LLI *pGdmaChLli = pHalGdmaAdapter->pLlix;
    struct BLOCK_SIZE_LIST *pBlockSizeList = pHalGdmaAdapter->pBlockSizeList;
    u32 TotalBlockSize = 0;
    u8  IsrTypeMap, BlockIndex;
    u8 *pSrc = NULL, *pDst = NULL;
#if FLYSKY_DEBUG
    printf("(%d)Enter Gdma0 Channel 5 ISr\n",__LINE__);
    /*printf("%s(%d): srcReg:0x%x, dstReg:0x%x \n", __FUNCTION__, __LINE__, REG32(0xb801b0b0), REG32(0xb801b0b8));*/

    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);

    printf("(%d)Sar=0x%08x , Dar=0x%08x\n",__LINE__,pHalGdmaAdapter->ChSar,pHalGdmaAdapter->ChDar);	// Flysky debug Msg
#endif
#if 0 //def CACHE_COHERENCE  // Carl     20141222

    //u32 start1 = (u32)BDSrcTest & ~(ARCH_DMA_MINALIGN - 1);
    //u32 end1 = ALIGN(start1+ DATA_SIZE, ARCH_DMA_MINALIGN);

    //u32 start2 = (u32)BDDstTest & ~(ARCH_DMA_MINALIGN - 1);
    //u32 end2 = ALIGN(start2 + DATA_SIZE, ARCH_DMA_MINALIGN);

    invalidate_dcache_range(start1,end1);
    invalidate_dcache_range(start2,end2);

    printf("%d - invalidate BDSrcTest=0x%08x pSrc=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDSrcTest,(u32)pHalGdmaAdapter->ChSar,ARCH_DMA_MINALIGN,start1,end1);
    printf("%d - invalidate BDDstTest=0x%08x pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDDstTest,(u32)pHalGdmaAdapter->ChDar,ARCH_DMA_MINALIGN,start2,end2);

#endif

    if ((pHalGdmaAdapter->TestItem != AutoSarLlpDar) &&
        (pHalGdmaAdapter->TestItem != LlpSarAutoDar)) {
        IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
        printf("(%d)Isr Type 0x%x\n",__LINE__ ,IsrTypeMap);
    }

    switch (pHalGdmaAdapter->TestItem) {
        case AutoSarLlpDar:
        case LlpSarAutoDar: {
                //4 Clean Auto Reload Bit
                if ((pHalGdmaAdapter->MaxMuliBlock) == (pHalGdmaAdapter->MuliBlockCunt +1)) {   // fix bug 20170619 ???
                    printf("(%d)Clean Auto Reload Dst\n",__LINE__);
                    if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
                        HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter); // fix bug 20170619
                    }
                    else {
                        HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter); // fix debug 20170619
                    }
                }

                //Search the correct Llix
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;

                    if (BlockIndex == (pHalGdmaAdapter->MuliBlockCunt-1)) {
                        if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
                            pSrc = (u8*)pGdmaChLliEle->Sarx;
                            pDst = (u8*)pHalGdmaAdapter->ChDar;
                        }
                        else {
                            //AutoSarLlpDar
                            pSrc = (u8*)pHalGdmaAdapter->ChSar;
                            pDst = (u8*)pGdmaChLliEle->Darx;
                        }
#if 1   // marked this => 98F will PASS  20170619
                        if (!GdmaDataCompare(pSrc,
                                              pDst,
                                              (pBlockSizeList->BlockSize<<2))) {
                            printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);

                            gDataCheckValue = _FALSE;
                            break;
                        }
                        else {
                            printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                        }
#endif
#if 1 // invalid... 20170619 for 98F ARM cache
     if ((pHalGdmaAdapter->MaxMuliBlock) != (pHalGdmaAdapter->MuliBlockCunt)){

                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;

                    pGdmaChLliEle = pGdmaChLli->pLliEle;

                        if (pHalGdmaAdapter->TestItem == LlpSarAutoDar) {
                            pSrc = (u8*)pGdmaChLliEle->Sarx;
                            pDst = (u8*)pHalGdmaAdapter->ChDar;
                        }
                        else {
                            //AutoSarLlpDar
                            pSrc = (u8*)pHalGdmaAdapter->ChSar;
                            pDst = (u8*)pGdmaChLliEle->Darx;
                        }

    u32 start1 = (u32)pSrc & ~(ARCH_DMA_MINALIGN - 1);
    u32 end1 = ALIGN(start1+ (pBlockSizeList->BlockSize<<2), ARCH_DMA_MINALIGN);

    u32 start2 = (u32)pDst & ~(ARCH_DMA_MINALIGN - 1);
    u32 end2 = ALIGN(start2+ (pBlockSizeList->BlockSize<<2), ARCH_DMA_MINALIGN);

    invalidate_dcache_range(start1,end1);
    invalidate_dcache_range(start2,end2);
#if FLYSKY_DEBUG
    printf("(%d) invalidate pSrc=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)pSrc,ARCH_DMA_MINALIGN,start1,end1);
    printf("(%d) invalidate pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)pDst,ARCH_DMA_MINALIGN,start2,end2);
#endif
    }
#endif
                        break;
                    }
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }

                IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
                printf("(%d)Isr Type 0x%x\n",__LINE__ ,IsrTypeMap);


                //3 Maintain Block Count
                if (IsrTypeMap & BlockType) {
#if FLYSKY_DEBUG
                    printf("(%d)DMA Block %d\n",__LINE__,pHalGdmaAdapter->MuliBlockCunt);
#endif
                    pHalGdmaAdapter->MuliBlockCunt++;
                }


            }
            break;
        case ConSarLlpDar: {
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
                    if (!GdmaDataCompare((u8*)(pHalGdmaAdapter->ChSar + TotalBlockSize),
                                          (u8*)pGdmaChLliEle->Darx,
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                    }
                    TotalBlockSize += (pBlockSizeList->BlockSize<<2);
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }
            }
            break;
        case LlpSarConDar:
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
#if FLYSKY_DEBUG
                    printf("(%d)Sar: 0x%x; Dst: 0x%x\n",__LINE__, pGdmaChLliEle->Sarx,(pHalGdmaAdapter->ChDar + TotalBlockSize));
#endif
                    if (!GdmaDataCompare((u8*)pGdmaChLliEle->Sarx,
                                          (u8*)(pHalGdmaAdapter->ChDar + TotalBlockSize),
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("(%d)DMA Block %d Data OK\n",__LINE__,BlockIndex);
                    }

                    TotalBlockSize += (pBlockSizeList->BlockSize<<2);
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }

            break;
        case LlpSarLlpDar:
        case SignalBlockLlp:
                for (BlockIndex=0; BlockIndex< pHalGdmaAdapter->MaxMuliBlock; BlockIndex++) {
                    pGdmaChLliEle = pGdmaChLli->pLliEle;
#if FLYSKY_DEBUG
                    printf("%s(%d): 0x%x, 0x%x\n", __FUNCTION__, __LINE__, pGdmaChLliEle->Sarx, pGdmaChLliEle->Darx);
#endif
#if 0
                    printf("%s(%d): 0x%x, 0x%x\n", __FUNCTION__, __LINE__,
                                REG32(pGdmaChLliEle->Sarx|0x20000000), REG32(pGdmaChLliEle->Darx|0x20000000));

                    printf("%s(%d): 0x%x, 0x%x, 0x%x, 0x%x\n", __FUNCTION__, __LINE__,
                                pGdmaChLliEle->Sarx, pGdmaChLliEle->Darx,
                                REG32(pGdmaChLliEle->Sarx), REG32(pGdmaChLliEle->Darx));
#endif

                    if (!GdmaDataCompare((u8*)pGdmaChLliEle->Sarx,
                                          (u8*)pGdmaChLliEle->Darx,
                                          (pBlockSizeList->BlockSize<<2))) {
                        printf("(%d)DMA Block %d Error: Check Data Error!!!\n",__LINE__,BlockIndex);
                        gDataCheckValue = _FALSE;
                        break;
                    }
                    else {
                        printf("DMA Block %d Data OK\n",BlockIndex);
                    }
                    pGdmaChLli = pGdmaChLli->pNextLli;
                    pBlockSizeList = pBlockSizeList->pNextBlockSiz;
                }
            break;
    }




    printf("(%d)Leave Gdma0 Channel 5 ISr\n",__LINE__);

}





void  show_sgr_dsr_dataview( u32 PacketLen,
									     u32* pSrc,
									     u32* pDst,
			                                             PHAL_GDMA_ADAPTER pHalGdmaAdapter
										)
{
		u32 *pSrcAddress ;
		u32 *pDstAddress ;
		PGDMA_SGR_REG   sgr_reg;
		PGDMA_DSR_REG dsr_reg;
		u32 CheckIndex = 0;
		u32 sIndex = 0;
		u32 i=0;
		u32 sgc_count,dsc_count,sgi_count,dsi_count;
		u16 width=4;
		sgr_reg = &pHalGdmaAdapter->GdmaSgr;
		dsr_reg = &pHalGdmaAdapter->GdmaDsr;
		pSrcAddress = (u32*)pSrc;
		pDstAddress = (u32*)pDst;

		 sgc_count = sgr_reg->Sgc*width; 		/*source gather count*/
		 dsc_count = dsr_reg->Dsc*width; 		/*destination scatter count*/
		 sgi_count = sgr_reg->Sgi*width;		/*source gather increment*/
		 dsi_count =dsr_reg->Dsi*width;		/*destination scatter increment*/
		while (CheckIndex < PacketLen && sIndex  < PacketLen) {

			if( (pHalGdmaAdapter->GdmaCtl.DstScatterEn ) && (dsc_count == 0 ))
			{
				/* disable source memory print, print dst memory only*/
				printf("%-57s|" ,"");
			}
			else{
				printf(ADDR_COLOR" %08X:	" ADDR_COLOR_END,pSrcAddress);
				printf( "%08X  %08X  %08X  %08X  |"
				,*(pSrcAddress)
				,*(pSrcAddress+1)
				,*(pSrcAddress+2)
				,*(pSrcAddress+3)
				);
				pSrcAddress+=4;
				sIndex +=16;
			}

			if((pHalGdmaAdapter->GdmaCtl.SrcGatherEn) && (sgc_count ==0) )
			{
				printf( "\n");
				sgi_count -= 16;
				if(sgi_count == 0)
					sgc_count = sgr_reg->Sgc*width;
			}
			else{
				printf(ADDR_COLOR" %08X:	" ADDR_COLOR_END,pDstAddress);
				printf( "%08X  %08X  %08X  %08X  \n"
				,*(pDstAddress)
				,*(pDstAddress+1)
				,*(pDstAddress+2)
				,*(pDstAddress+3)
				);
				pDstAddress+=4;
				CheckIndex +=16;
				if((pHalGdmaAdapter->GdmaCtl.DstScatterEn) )
				{
					if( dsc_count != 0)
					{
						dsc_count -=16;
						if( dsc_count == 0)
							dsi_count =dsr_reg->Dsi*width;
					}
					else
					{
						dsi_count -= 16;
						if(dsi_count == 0)
							dsc_count = dsr_reg->Dsc*width;
					}
				}
				if((pHalGdmaAdapter->GdmaCtl.SrcGatherEn) )
				{
					if( sgc_count != 0)
					{
						sgc_count -=16;
						if( sgc_count == 0)
							sgi_count = sgr_reg->Sgi*width;
					}
				}
			}

		  }

}


BOOL
GdmaData_SGR_DSR_Compare
(
    IN  u8      *pSrc, IN  u8      *pDst, PHAL_GDMA_ADAPTER pHalGdmaAdapter
)
{
    u32 CheckIndex = 0;
    u32 sIndex = 0;
    PGDMA_SGR_REG   sgr_reg;
    PGDMA_DSR_REG dsr_reg;
    u32 dsr_count = 0;
    u32 sgr_count = 0;
    u32 interval = 0;
    u32 PacketLen =pHalGdmaAdapter->PacketLen;
    u32 width = 4;// FourByte, pHalGdmaAdapter->GdmaCtl.DstTrWidth. DST/SRC Should be 4B

    u32 src_transfer_total_amount = 0;
    u32 dst_rece_total_amount = 0;
    sgr_reg = &pHalGdmaAdapter->GdmaSgr;
    dsr_reg = &pHalGdmaAdapter->GdmaDsr;

    CheckIndex=0;

	show_sgr_dsr_dataview(PacketLen, pSrc, pDst, pHalGdmaAdapter );
     while (CheckIndex < PacketLen && sIndex  < PacketLen) {
        if (pSrc[sIndex]!=pDst[CheckIndex]) {
            return _FALSE;
        }
	 else{
		src_transfer_total_amount +=1;
		dst_rece_total_amount +=1;
	 }
        dsr_count++;
        sgr_count++;
        if((pHalGdmaAdapter->GdmaCtl.DstScatterEn)&& (dsr_count == (dsr_reg->Dsc*width))){
                CheckIndex += (dsr_reg->Dsi*width);
                dsr_count = 0;
        }
        if((pHalGdmaAdapter->GdmaCtl.SrcGatherEn)&&(sgr_count == (sgr_reg->Sgc*width))){
                sIndex += (sgr_reg->Sgi*width);
                sgr_count = 0;
        }
		CheckIndex++;
		sIndex++;
    }

   if((pHalGdmaAdapter->GdmaCtl.SrcGatherEn))
	printf("SRC Gather Total Counts : %d bytes\n", src_transfer_total_amount);

   if((pHalGdmaAdapter->GdmaCtl.DstScatterEn))
	printf("DST Scatter Total Counts : %d bytes\n", dst_rece_total_amount);	
    printf("Pass\n");
    return _TRUE;
}

VOID
Gdma0Ch3IrqHandle_SGR_DSR
(
    IN  VOID        *Data
)
{
    PGDMA_ADAPTER pGdmaAdapte = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapte->pHalGdmaAdapter;
    u8 *pSrc = NULL, *pDst = NULL;
    u8  IsrTypeMap = 0;
#if FLYSKY_DEBUG
    printf("(%s %d)Enter Gdma0 Channel 3 ISr\n",__func__, __LINE__);
    printf("(%d)GMDA %d; Channel %d, Isr Type: %d\n",__LINE__,
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case SignalBlockDsr: {
                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)pHalGdmaAdapter->ChDar;
            }
            break;
        default:
                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)pHalGdmaAdapter->ChDar;
                break;
    }

#ifdef CACHE_COHERENCE

    u32 start1 = (u32)BDSrcTest & ~(ARCH_DMA_MINALIGN - 1);
    u32 end1 = ALIGN(start1+ DATA_SIZE, ARCH_DMA_MINALIGN);

    u32 start2 = (u32)BDDstTest & ~(ARCH_DMA_MINALIGN - 1);
    u32 end2 = ALIGN(start2 + DATA_SIZE, ARCH_DMA_MINALIGN);

    invalidate_dcache_range(start1,end1);
    invalidate_dcache_range(start2,end2);
#if FLYSKY_DEBUG
    printf("(%d) invalidate BDSrcTest=0x%08x pSrc=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDSrcTest,(u32)pHalGdmaAdapter->ChSar,ARCH_DMA_MINALIGN,start1,end1);
    printf("(%d) invalidate BDDstTest=0x%08x pDst=0x%08x %d 0x%08x-0x%08x(debug)\n",__LINE__,(u32)BDDstTest,(u32)pHalGdmaAdapter->ChDar,ARCH_DMA_MINALIGN,start2,end2);
#endif
#endif

    printf("compare SRC=0x%x DST=0x%x!\n", pSrc, pDst);

    //3 Compare Data
    if (!GdmaData_SGR_DSR_Compare(pSrc, pDst, pHalGdmaAdapter)) {
        printf("(%s %d)DMA Error: Check Data Error!!!\n",__func__, __LINE__);
        gDataCheckValue = _FALSE;
    }
    else {


    }

    //3 Clear Pending ISR
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
    //IsrTypeMap = 0x2;

    //printf("(%s %d)Leave.\n", __func__, __LINE__);
}






BOOL
GdmaDataCompare
(
    IN  u8      *pSrc,
    IN  u8      *pDst,
    IN  u32     PacketLen
)
{
    u32 CheckIndex = 0;
    bool bPass =1;
#if defined(__96F_PHERIPHERAL_H__)
    if( ((u32)pSrc & 0xf0000000) == 0x00000000){
        printf("***compare Ori-SrcAddr= 0x%08x \n", pSrc);
        pSrc = (u32)pSrc | 0xa0000000;  // cached:0x80000000   uncached:0xa0000000
        printf("***compare transfer-SrcAddr= 0x%08x \n", pSrc);
    }
    if( ((u32)pDst & 0xf0000000) == 0x00000000){
        printf("***compare Ori-DstAddr= 0x%08x \n", pDst);
        pDst = (u32)pDst | 0xa0000000;
        printf("***compare transfer-DstAddr= 0x%08x \n", pDst);
    }


    if( ((u32)pSrc & 0xf0000000) == 0x80000000){
        printf("---compare Ori-SrcAddr= 0x%08x \n", pSrc);
        pSrc = (u32)pSrc | 0xa0000000;  // cached:0x80000000   uncached:0xa0000000
        printf("---compare transfer-SrcAddr= 0x%08x \n", pSrc);
    }
    if( ((u32)pDst & 0xf0000000) == 0x80000000){
        printf("---compare Ori-DstAddr= 0x%08x \n", pDst);
        pDst = (u32)pDst | 0xa0000000;
        printf("---compare transfer-DstAddr= 0x%08x \n", pDst);
    }
#elif defined(__98F_PHERIPHERAL_H__)
        printf("---compare transfer-SrcAddr= 0x%08x \n", pSrc);
        printf("---compare transfer-DstAddr= 0x%08x \n", pDst);
#elif defined(__98X_PHERIPHERAL_H__)
	printf("---compare transfer-SrcAddr= 0x%08x \n", pSrc);
	printf("---compare transfer-DstAddr= 0x%08x \n", pDst);
#endif

#if FLYSKY_DEBUG
	printf("PacketLen =%d\n", PacketLen); // Flysky
	{
		u32 *pSrcAddress ;
		u32 *pDstAddress ;
		u32 LenIndex;
		u32 i=0;
		s32  j=0;

		for(LenIndex = 0, pSrcAddress = (u32*)pSrc,  pDstAddress = (u32*)pDst; LenIndex < PacketLen/4; LenIndex+=4, pSrcAddress+=4, pDstAddress+=4) {

			printf(ADDR_COLOR" %08X:    " ADDR_COLOR_END,pSrcAddress);
			printf( "%08X  %08X  %08X  %08X  |"
				,*(pSrcAddress)
				,*(pSrcAddress+1)
				,*(pSrcAddress+2)
				,*(pSrcAddress+3)
			    );
			printf( ADDR_COLOR" %08X:    " ADDR_COLOR_END,pDstAddress  );

			for(i=0 ;i < 4 ;i++)
			{
				if( *(pDstAddress+i) == *(pSrcAddress+i))
					printf( "%08X  ",*(pDstAddress+i)   );
				else {
					u8 *u8SrcAddr = (u8 *)(pSrcAddress+i);
					u8 *u8DstAddr = (u8 *)(pDstAddress+i);
					for(j=3; j >= 0 ;j--)
					{
						if( *(u8SrcAddr+j) == *(u8DstAddr+j))
							printf( "%02X",*(u8DstAddr+j)   );
						else
							printf(ERROR_COLOR"%02X"ERROR_COLOR_END,*(u8DstAddr+j)   );
					}
					printf( "  ");
					bPass = 0;
				}
			}
			printf( "\n");
	      }

		if(bPass == 0)
			return _FALSE;
	}
	#if 0
	for (CheckIndex=0; CheckIndex < PacketLen; CheckIndex++) {
		if( (PacketLen < 10) || (CheckIndex%50==0) )
			printf("S[%d]=0x%04x,  D[%d]=0x%04x \n",CheckIndex,pSrc[CheckIndex],CheckIndex,pDst[CheckIndex]); // Flysky

		if(pSrc[CheckIndex]!=pDst[CheckIndex])
		{
			printf("**********S[%d]=0x%04x ,D[%d]=0x%04x **********\n",CheckIndex,pSrc[CheckIndex],CheckIndex,pDst[CheckIndex]); // Flysky
			if(CheckIndex > 250)	return _FALSE;
		}
	}
	#endif
#else
    for (CheckIndex=3; CheckIndex < PacketLen; CheckIndex++) {
        if (pSrc[CheckIndex]!=pDst[CheckIndex]) {
            return _FALSE;
        }
    }
#endif
    //printf("Pass\n"); // Flysky add for regression.
    return _TRUE;
}
