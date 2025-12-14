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
//#include "dw_hal_spi.h"
//#include "dw_hal_gdma.h"
#include "dw_spi_base_test.h"
#include "peripheral.h"

#include <stdarg.h>

#include "common.h"	// Carl 20140302

/**
 * Debug Definition
 */
#define DEBUG_SSI          1
#define DEBUG_SSI_PARA     1
#define DEBUG_SSI_PINMUX   1
#define DEBUG_GDMA_CONFIG  1	// Carl TODO check

/**
 * SSI Global Variables
 */
typedef struct _SSI_VERIFY_PARAMETER_ {
    u8  Para0;
    u8  Para1;
    u8  Para2;
    u8  Para3;
    u8  Para4;
    u8  Para5;
    u8  Para6;
    u8  Para7;
    u8  Para8;
    u8  Para9;
}SSI_VERIFY_PARAMETER, *PSSI_VERIFY_PARAMETER;

typedef struct _SSI_ADAPTER_ {
    PHAL_SSI_OP      pHalSsiOp;
    PHAL_SSI_ADAPTER pHalSsiAdapter;
}SSI_ADAPTER, *PSSI_ADAPTER;

HAL_SSI_OP  HalSsiOp;
SSI_ADAPTER SsiAdapter_Slave;
SSI_ADAPTER SsiAdapter_Master;
HAL_SSI_ADAPTER HalSsiAdapter_Slave;
HAL_SSI_ADAPTER HalSsiAdapter_Master;

/**
 * GDMA
 */
typedef struct _GDMA_ADAPTER_ {
    PHAL_GDMA_OP pHalGdmaOp;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
}GDMA_ADAPTER, *PGDMA_ADAPTER;

HAL_GDMA_OP HalGdmaOpSPI;
GDMA_ADAPTER GdmaAdapter_Tx;
GDMA_ADAPTER GdmaAdapter_Rx;
HAL_GDMA_ADAPTER HalGdmaAdapter_Tx;
HAL_GDMA_ADAPTER HalGdmaAdapter_Rx;

/**
 * MISC.
 * SYSTEM_CLK  40000000
 */
#define DATA_SIZE  512
/* #define BAUDR_MULTIPLIER  1000  // 1*1000: 40000000 / 1000 = 40000   = 40KHz, Suggest: 1 */
#define BAUDR_MULTIPLIER  100   //  1*100: 40000000 / 100  = 400000  = 400KHz, Suggest: 10
//#define BAUDR_MULTIPLIER  10    //   1*10: 40000000 / 10   = 4000000 = 4MHz
//#define BAUDR_MULTIPLIER  1		// Carl TODO check

#define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

#define write_reg(par, ...)                                              \
do {                                                                     \
      write_reg8_bus8_SPI1(par, NUMARGS(__VA_ARGS__), __VA_ARGS__);    \
} while (0)

#define write_reg_memwr(par, ...)                                              \
do {                                                                     \
      write_reg8_bus8_SPI1_MEMWR(par, NUMARGS(__VA_ARGS__), __VA_ARGS__);    \
} while (0)

//extern void set_DCX_(int value);

#define write_reg_cmd(par, ...)                                              \
do {                                                                     \
	HAL_WRITE32(0xb8000000,0x350c, 0x0);  \
	udelay(10000);   \
	write_reg8_bus8_SPI1(par, NUMARGS(__VA_ARGS__), __VA_ARGS__);    \
	HAL_WRITE32(0xb8000000,0x350c, 0x2);  \
	udelay(10000);   \
} while (0)

//par->fbtftops.write_register(par, NUMARGS(__VA_ARGS__), __VA_ARGS__); \

u16 RsvdMemForTx[DATA_SIZE];
u16 RsvdMemForRx[DATA_SIZE];
volatile u32 RsvdMemForRx_Index = 0;
volatile BOOL RxTransferDone = _FALSE;

typedef enum _SSI_TEST_TYPE_ {
    SSI_TT_BASIC_IO,
    SSI_TT_INTERRUPT,
    SSI_TT_DMA_TO_TX,
    SSI_TT_RX_TO_DMA
}SSI_TEST_TYPE, *PSSI_TEST_TYPE;

typedef enum _SSI_TEST_SRC_DATA_MODE_ {
    SSI_TEST_SRCDATA_SEQ,
    SSI_TEST_SRCDATA_RND
}SSI_TEST_SRC_DATA_MODE, *PSSI_TEST_SRC_DATA_MODE;


/**
 * Interrupt Handler
 */
VOID
SsiAIrqHandle (
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    u32 Counter;
    u32 RegValue;
    u32 InterruptStatus;
    /* u32 RsvdMemForRx_Index_Local; */
    PSSI_ADAPTER pSsiAdapter = (PSSI_ADAPTER) Data;
    PHAL_SSI_OP  pHalSsiOp   = pSsiAdapter->pHalSsiOp;
    PHAL_SSI_ADAPTER pHalSsiAdapter = pSsiAdapter->pHalSsiAdapter;
    u8 Index = pHalSsiAdapter->Index;
    u8 Role  = pHalSsiAdapter->Role;

    InterruptStatus = pHalSsiOp->HalSsiGetInterruptStatus(pHalSsiAdapter);
     LOGD("SSI%d Interrupt Status = %02X\n", Index, InterruptStatus);

    if (InterruptStatus & BIT_ISR_TXOIS) {
        LOGI("[INT] Transmit FIFO Overflow Interrupt\n");
        HAL_SSI_READ32(Index, REG_DW_SSI_TXOICR);
    }

    if (InterruptStatus & BIT_ISR_RXUIS) {
        LOGI("[INT] Receive FIFO Underflow Interrupt\n");
        HAL_SSI_READ32(Index, REG_DW_SSI_RXUICR);
    }

    if (InterruptStatus & BIT_ISR_RXOIS) {
        /* LOGI("[INT] Receive FIFO Overflow Interrupt\n"); */
        LOGI(ANSI_COLOR_RED"[INT] Receive FIFO Overflow Interrupt\n"ANSI_COLOR_RESET);
        HAL_SSI_READ32(Index, REG_DW_SSI_RXOICR);
    }

    if (InterruptStatus & BIT_ISR_TXEIS) {
        /* Tx FIFO is empty, need to transfer data */
        LOGI("[INT] Transmit FIFO Empty Interrupt\n");
    }

    if (InterruptStatus & BIT_ISR_MSTIS) {
        LOGI("[INT] Multi-Master Contention Interrupt\n");
        /* Another master is actively transferring data */
        /* Do reading data... */
        HAL_SSI_READ32(Index, REG_DW_SSI_MSTICR);
    }

    /* LOGD("[INT] Rx FIFO is equal to threshold(+1), Receive Data...\n"); */
    if ((InterruptStatus & BIT_ISR_RXFIS) ) {
        Counter = 0;
        /* RsvdMemForRx_Index_Local = RsvdMemForRx_Index; */
        RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter)) & BIT_SR_RFNE;

        while (RegValue) {  //Rx Fifo Not Empty
            Counter = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter);

            while (Counter--) {
                /* RsvdMemForRx[RsvdMemForRx_Index_Local++] = pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter); */
                RsvdMemForRx[RsvdMemForRx_Index++] = pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter); //OK
                /* LOGD(ANSI_COLOR_RED"%04X\n"ANSI_COLOR_RESET, RsvdMemForRx[RsvdMemForRx_Index-1]); */

                if (RsvdMemForRx_Index == DATA_SIZE) {
                    RxTransferDone = _TRUE;
                }
            }

            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter)) & BIT_SR_RFNE;
        }
        /* LOGD("RsvdMemForRx_Index = %d\n", RsvdMemForRx_Index);  // Need to this to avoid GCC compiler optimization. */
        /* LOGD("RsvdMemForRx_Index_Local = %d\n", RsvdMemForRx_Index_Local); */
        /* RsvdMemForRx_Index = RsvdMemForRx_Index_Local; */
    }
}

VOID
SsiBIrqHandle (
        IN VOID *Data
        )
{
    DBG_ENTRANCE;
    u32 RegValue;
    u32 InterruptStatus;
    PSSI_ADAPTER pSsiAdapter = (PSSI_ADAPTER) Data;
    PHAL_SSI_OP  pHalSsiOp   = pSsiAdapter->pHalSsiOp;
    PHAL_SSI_ADAPTER pHalSsiAdapter = pSsiAdapter->pHalSsiAdapter;
    u8 Index = pHalSsiAdapter->Index;
    u8 Role  = pHalSsiAdapter->Role;

    LOGD(ANSI_COLOR_YELLOW"SSI%d(%d) InterruptHandler\n"ANSI_COLOR_RESET, Index, Role);
}

/**
 * GDMA IRQ Handler
 */
VOID
SsiTxGdmaIrqHandle (
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PGDMA_ADAPTER pGdmaAdapter = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapter->pHalGdmaAdapter;
    u8  IsrTypeMap = 0;
#if 0
    LOGD("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case SSI_TT_DMA_TO_TX:
            LOGD(ANSI_COLOR_MAGENTA"DMA --> Tx (SSI_TT_DMA_TO_TX)\n"ANSI_COLOR_RESET);
            break;
        default:
            break;
    }

    /* Clear Pending ISR */
    IsrTypeMap = HalGdmaOpSPI.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
#if 0
    if (IsrTypeMap & BlockType) {
        LOGD("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt);
        pHalGdmaAdapter->MuliBlockCunt++;
    }
#endif
}

VOID
SsiRxGdmaIrqHandle (
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PGDMA_ADAPTER pGdmaAdapter = (PGDMA_ADAPTER) Data;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter = pGdmaAdapter->pHalGdmaAdapter;
    u8  IsrTypeMap = 0;
#if 0
    LOGD("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case SSI_TT_RX_TO_DMA:
            LOGD(ANSI_COLOR_MAGENTA"Rx --> DMA (SSI_TT_RX_TO_DMA)\n"ANSI_COLOR_RESET);
            break;
        default:
            break;
    }

    /* Clear Pending ISR */
    IsrTypeMap = HalGdmaOpSPI.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
#if 0
    if (IsrTypeMap & BlockType) {
        LOGD("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt);
        pHalGdmaAdapter->MuliBlockCunt++;
    }
#endif
}

/**
 * Software Workaround (only for test program)
 */
VOID
SsiIpEnable()
{
    u32 RegValue;

    /* Diable JTAG Function */
    //RegValue = HAL_READ32(PERI_ON_BASE, REG_CPU_PERIPHERAL_CTRL);
    //RegValue &= ~(BIT16);
    ////RegValue &= ~(BIT0);  //SPI NOR flash pin function enable
    //HAL_WRITE32(PERI_ON_BASE, REG_CPU_PERIPHERAL_CTRL, RegValue);
	JTAG_PIN_FCTRL(OFF);
}

VOID
GdmaIpEnable()
{
    u32 RegValue;

    /* Clock */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_CTRL);
    RegValue = RegValue | BIT16 | BIT18;
    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_CLK_CTRL, RegValue);

    /* Function Enable */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
    RegValue = RegValue | BIT13 | BIT14;
    HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, RegValue);

    /* Handshake Interface Configuration */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
    /* RegValue &= ~(1<<19);  //3 */
    //
    RegValue &= ~(1<<20);  //4
    RegValue &= ~(1<<21);  //5
    RegValue &= ~(1<<22);  //6
    RegValue &= ~(1<<23);  //7
    //
    /* RegValue &= ~(1<<24);  //8 */
    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, RegValue);
}

u8
GetGdmaChannelIrq(
        u8 GdmaIndex, u8 GdmaChannel
        )
{
    /* DBG_ENTRANCE; */
    u8 IrqBase;
    u8 GdmaChannelIrq;

    if (GdmaIndex == 0)
        IrqBase = 20;
    else if (GdmaIndex == 1)
        IrqBase = 26;

    GdmaChannelIrq = IrqBase + GdmaChannel;
    LOGD("GDMA[%d, %d], IRQ: %d\n", GdmaIndex, GdmaChannel, GdmaChannelIrq);
    return GdmaChannelIrq;
}

u16
GetGdmaChannelEnableValue(
        u8 GdmaChannel
        )
{
    /* DBG_ENTRANCE; */
    u16 ChannelEnableValue;

    switch (GdmaChannel)
    {
        case 0:  {ChannelEnableValue = GdmaCh0;   break;}
        case 1:  {ChannelEnableValue = GdmaCh1;   break;}
        case 2:  {ChannelEnableValue = GdmaCh2;   break;}
        case 3:  {ChannelEnableValue = GdmaCh3;   break;}
        //case 4:  {ChannelEnableValue = GdmaCh4;   break;}
        //case 5:  {ChannelEnableValue = GdmaCh5;   break;}
        //case 6:  {ChannelEnableValue = GdmaCh6;   break;}
        //case 7:  {ChannelEnableValue = GdmaCh7;   break;}
        default: {ChannelEnableValue = GdmaAllCh; break;}
    }
    /* LOGD("ChannelEnableValue = %04X\n", ChannelEnableValue); */
    return ChannelEnableValue;
}

u32
GetSsiIrqNumber(
        u8 SsiIndex
        )
{
    DBG_ENTRANCE;
    u32 SsiIrqNumber;

    switch (SsiIndex)
    {
        case 0:  {SsiIrqNumber = DW_SSI_0_IP_IRQ; break;}	// Carl
        case 1:  {SsiIrqNumber = DW_SSI_1_IE_IRQ; break;}
        case 2:  {SsiIrqNumber = DW_SSI_0_IP_IRQ; break;}
        default: {SsiIrqNumber = 0;        break;}
    }

    LOGD(ANSI_COLOR_MAGENTA"SSI%d IRQ: %d\n", SsiIndex, SsiIrqNumber);
    return SsiIrqNumber;
}

VOID
DumpSsiRegInfo(
        u8 SsiIndex
        )
{
    /* Dump SR, RISR */
    /* LOGD(ANSI_COLOR_YELLOW"SSI%d SR(0x28)  : %X\n"ANSI_COLOR_RESET, SsiIndex, */
    /*         HAL_SSI_READ32(SsiIndex, REG_DW_SSI_SR)); */
    /* LOGD(ANSI_COLOR_YELLOW"SSI%d RISR(0x28): %X\n"ANSI_COLOR_RESET, SsiIndex, */
    /*         HAL_SSI_READ32(SsiIndex, REG_DW_SSI_RISR)); */
}


BOOL
SsiDataCompare(
        IN u16 *pSrc,
        IN u16 *pDst,
        IN u32 Length
        )
{
    /* DBG_ENTRANCE; */
    int Index;

    for (Index=0; Index < Length; ++Index) {
#if 0
        LOGD("pSrc[%d] = %04X\n", Index, pSrc[Index]);
        LOGD("pDst[%d] = %04X\n", Index, pDst[Index]);
#endif
        if (pSrc[Index] != pDst[Index])
        {
            LOGT(ANSI_COLOR_RED" 0x%08x  0x%08x Failed index=%d (%x != %x)\n"ANSI_COLOR_RESET, pSrc, pDst,Index, pSrc[Index], pDst[Index]);
            for (Index=0; Index < Length; ++Index)
            {
                LOGD("pSrc[%d] = %04X\n", Index, pSrc[Index]);
                LOGD("pDst[%d] = %04X\n", Index, pDst[Index]);                
            }
                
            return _FALSE;
        }
    }

    return _TRUE;
}

VOID
SsiGenerateRandomTxData(
        IN u16 *pTxArray,
        IN u32 Length,
        IN u32 Mode,
        IN BOOL MICROWIRE
        )
{
    u32 DummyData = 0;
    u32 RegValue  = 0;

    memset(pTxArray, 0x0, (Length * 2));

    switch (Mode)
    {
        case SSI_TEST_SRCDATA_SEQ:
        {
            if (MICROWIRE) {
                for (RegValue = 0; RegValue < Length; ++RegValue, ++DummyData) {
                    if ((RegValue % 2) == 0)
                        continue;
                    HAL_WRITE16(RsvdMemForTx, RegValue, DummyData);
                }
            }
            else {
                for (RegValue = 0; RegValue < Length; ++RegValue, ++DummyData) {
                    HAL_WRITE16(RsvdMemForTx, RegValue, DummyData);
                }
            }
            break;
        }
        case SSI_TEST_SRCDATA_RND:
        {
            if (MICROWIRE) {
                for (RegValue = 0; RegValue < Length; ++RegValue) {
                    if ((RegValue % 2) == 0)
                        continue;
                    DummyData = Rand() %  Length;
                    HAL_WRITE16(RsvdMemForTx, RegValue, DummyData);
                }
            }
            else {
                for (RegValue = 0; RegValue < Length; ++RegValue) {
                    //DummyData = Rand() %  Length;
                    DummyData = Rand() %  0xff;
                    HAL_WRITE16(RsvdMemForTx, RegValue, DummyData);
                }
            }
            break;
        }
        default:
            break;
    }
}

VOID
SsiDumpReg() {
printf("%s(%d) Carl 0x%08x\n", __FUNCTION__, __LINE__,PERI_ON_BASE);
    u32 RegValue;
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_PERI_FUNC0_EN);
    LOGD("------------------------------------------------\n");
    LOGD("  REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X (8|10)\n", REG_SOC_PERI_FUNC0_EN, RegValue);
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CLK_CTRL0);
    LOGD("  REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X (16|20)\n", REG_PESOC_PERI_CLK_CTRL0, RegValue);
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL);
    LOGD("  REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X\n", REG_SPI_MUX_CTRL, RegValue);
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_PERI_CTRL);
    LOGD("  REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X (b8=0)\n", REG_PESOC_PERI_CTRL, RegValue);
    RegValue = HAL_READ32(PERI_ON_BASE, REG_CPU_PERIPHERAL_CTRL);
    LOGD("	REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X (b8=0)\n", REG_CPU_PERIPHERAL_CTRL, RegValue);
    LOGD("------------------------------------------------\n");
}

VOID
write_reg8_bus8_SPI1(int ipar, int len, ...)
{
    va_list args; 
    u32 i;
    u32 buf[10];
    va_start(args, len);
    
        for (i = 0; i < len; i++) {                                   
            buf[i] = (u32)va_arg(args, unsigned int);
            //printf("%s(%d):  buf[%d]=0x%04x \n", __FUNCTION__, __LINE__,i,buf[i]);

            HAL_WRITE16(0xb801c100,0x60, buf[i]&0xffff);
            mdelay(1);
        }
     va_end(args);

}

VOID
write_reg8_bus8_SPI1_MEMWR(int ipar, int len, ...)
{
    va_list args; 
    u32 i;
    u32 buf[10];
    va_start(args, len);
    
        for (i = 0; i < len; i++) {                                   
            buf[i] = (u32)va_arg(args, unsigned int);
            //printf("%s(%d):  buf[%d]=0x%04x \n", __FUNCTION__, __LINE__,i,buf[i]);

            HAL_WRITE16(0xb801c100,0x60, buf[i]&0xffff);
            //udelay(150); *100
            //udelay(70); // *10
            udelay(0); // *1 - don't marked
        }
     va_end(args);

}

/**
 * Main Function
 * ---------------------------------------------------------------------------
 *            P0      P1    P2    P3         P4
 * Usage: ssi  0       0     1     0          1 [1-10]
 *            Format  SSI0  SSI1  TEST_TYPE  MUL        TestLoops
 *
 * ---------------------------------------------------------------------------
 */
VOID
SsiTestApp(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    PSSI_VERIFY_PARAMETER pSsiVerifyData = (PSSI_VERIFY_PARAMETER) Data;
    u32 index;
    u32 RegValue;
    u32 TestType;
    u32 TestTimes;
    u32 TestLoops;
    u32 Counter;
	u32 WaitCounter;
    u32 TestFrameFormat;
    BOOL MICROWIRE;
    BOOL TI_SSP;

    u32 par = 0;
    u32 iforLoop = 0;
    u32 jforLoop = 0;

#if 0//DEBUG_SSI_PARA
    LOGD("Para0 = %d\n", pSsiVerifyData->Para0);
    LOGD("Para1 = %d\n", pSsiVerifyData->Para1);
    LOGD("Para2 = %d\n", pSsiVerifyData->Para2);
    LOGD("Para3 = %d\n", pSsiVerifyData->Para3);
    LOGD("Para4 = %d\n", pSsiVerifyData->Para4);
    LOGD("Para5 = %d\n", pSsiVerifyData->Para5);
    LOGD("Para6 = %d\n", pSsiVerifyData->Para6);
    LOGD("Para7 = %d\n", pSsiVerifyData->Para7);
    LOGD("Para8 = %d\n", pSsiVerifyData->Para8);
    LOGD("Para9 = %d\n", pSsiVerifyData->Para9);
#endif

#if 1
if(1 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = HAL_READ16(0xb801c000, 0x60);
    printf("%s(%d):  Read DR=0x%04x \n", __FUNCTION__, __LINE__,tmp_ReadData);
    return;
}

#if 1
if(2 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = SOC_HAL_R16(0xf432e100, 0x60);
    printf("%s(%d):  Read 98F SPI0 DR=0x%04x \n", __FUNCTION__, __LINE__,tmp_ReadData);
    return;
}
if(3 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = SOC_HAL_R16(0xf432e200, 0x60);
    printf("%s(%d):  Read 98F SPI1 DR=0x%04x \n", __FUNCTION__, __LINE__,tmp_ReadData);
    return;
}

if(4 == pSsiVerifyData->Para5)
{
    SOC_HAL_W16(0xf432e200,0x60, pSsiVerifyData->Para7&0xffff);
    //printf("%s(%d):  Write DR=0x%04x \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff);
    printf("%s(%d):  Write 98F SPI1 DR=0x%04x (0xf43203a8= 0x%08x) \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff,SOC_HAL_R32(0xf4320000, 0x3a8));
    return;
//HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, TxData);
}

if(5 == pSsiVerifyData->Para5)
{
    SOC_HAL_W16(0xf432e100,0x60, pSsiVerifyData->Para7&0xffff);
    //printf("%s(%d):  Write DR=0x%04x \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff);
    printf("%s(%d):  Write 98F SPI0 DR=0x%04x (0xf43203a8= 0x%08x) \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff,SOC_HAL_R32(0xf4320000, 0x3a8));
    return;
//HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, TxData);
}

#if 0
if(6 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = SOC_HAL_R32(0xf432e200, 0x34);
    printf("%s(%d):  Read 98F SPI1 RISR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData);
    return;
}

if(7 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = SOC_HAL_R32(0xf432e100, 0x34);
    printf("%s(%d):  Read 98F SPI0 RISR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData);
    return;
}

if(8 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData_0 = SOC_HAL_R32(0xf432e200, 0x20);
    u2Byte tmp_ReadData_1 = SOC_HAL_R32(0xf432e200, 0x24);
    printf("%s(%d):  Read 98F SPI1 TXFLR=0x%08x RXFLR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData_0, tmp_ReadData_1);
    return;
}

if(9 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData_0 = SOC_HAL_R32(0xf432e100, 0x20);
    u2Byte tmp_ReadData_1 = SOC_HAL_R32(0xf432e100, 0x24);
    printf("%s(%d):  Read 98F SPI0 TXFLR=0x%08x RXFLR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData_0, tmp_ReadData_1);
    return;
}
#else

if(6 == pSsiVerifyData->Para5)
{
    u2Byte tmp_ReadData = SOC_HAL_R32(0xf432e200, 0x34);
    printf("%s(%d):  Read 98F SPI1 RISR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData);
               tmp_ReadData = SOC_HAL_R32(0xf432e100, 0x34);
    printf("%s(%d):  Read 98F SPI0 RISR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData);

    u2Byte tmp_ReadData_0 = SOC_HAL_R32(0xf432e200, 0x20);
    u2Byte tmp_ReadData_1 = SOC_HAL_R32(0xf432e200, 0x24);
    printf("%s(%d):  Read 98F SPI1 TXFLR=0x%08x RXFLR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData_0, tmp_ReadData_1);

               tmp_ReadData_0 = SOC_HAL_R32(0xf432e100, 0x20);
               tmp_ReadData_1 = SOC_HAL_R32(0xf432e100, 0x24);
    printf("%s(%d):  Read 98F SPI0 TXFLR=0x%08x RXFLR=0x%08x \n", __FUNCTION__, __LINE__,tmp_ReadData_0, tmp_ReadData_1);

    return;
}

//VOID change_to_non_secure(VOID)
if(7 == pSsiVerifyData->Para5)
{
    unsigned int var;

    asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r" (var));
    var |= 1;
    asm volatile ("mcr p15, 0, %0, c1, c1, 0" : : "r" (var));

    printf("%s(%d)\n",__func__,__LINE__);

    return;
}

#endif

/*
#elif defined(CONFIG_RTL8198F)
#include <asm/io.h>
#define SOC_HAL_W32(base, addr, value)  writel(value, (base+addr))
#define SOC_HAL_R32(base, addr)         readl((base+addr))
#define SOC_HAL_W16(base, addr, value)  writew(value, (base+addr))
#define SOC_HAL_R16(base, addr)         readw((base+addr))
#define SOC_HAL_W8(base, addr, value)   writeb(value, (base+addr))
#define SOC_HAL_R8(base, addr)          readb((base+addr))
#endif
*/

#endif

if(1 == pSsiVerifyData->Para6)
{
    HAL_WRITE16(0xb801c100,0x60, pSsiVerifyData->Para7&0xffff);
    //printf("%s(%d):  Write DR=0x%04x \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff);
    printf("%s(%d):  Write DR=0x%04x (0xb800350c= 0x%08x) \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff,HAL_READ32(0xb8000000, 0x350c));
    return;
//HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, TxData);
}

if(2 == pSsiVerifyData->Para6)
{
    HAL_WRITE32(0xb8000000,0x350c, pSsiVerifyData->Para7&0xffffffff);
    printf("%s(%d):  Write 0x%08x=0x%08x \n", __FUNCTION__, __LINE__,0xb8000000+0x350c,pSsiVerifyData->Para7&0xffffffff);
    mdelay(10);
    return;
//HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, TxData);
}

if(3 == pSsiVerifyData->Para6)
{
    HAL_WRITE32(0xb8000000,0x350c, 0x0&0xffffffff);
    mdelay(10);
    
    HAL_WRITE16(0xb801c100,0x60, pSsiVerifyData->Para7&0xffff);
    printf("%s(%d):  Write DR=0x%04x (0xb800350c= 0x%08x) \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff,HAL_READ32(0xb8000000, 0x350c));
    //printf("%s(%d):  Write DR=0x%04x \n", __FUNCTION__, __LINE__,pSsiVerifyData->Para7&0xffff);

    HAL_WRITE32(0xb8000000,0x350c, 0x2&0xffffffff);
    mdelay(10);
    return;
}

if(4 == pSsiVerifyData->Para6)
{
    HAL_WRITE32(0xb8000000,0x350c, 0x0);    // SPI0_CMD_GPIO & SPI1_CMD_GPIO are low 
    mdelay(10);

//----------
#if 0   // only coding test
	write_reg_cmd(par, 0xCF, 0xff);
	write_reg(par, 0x00, 0x83, 0x30);
return;
#endif
    write_reg_cmd(par, 0x11); /* sleep out */
    mdelay(100);

    write_reg_cmd(par, 0x29); /* display on */
    mdelay(200);

    /* startup sequence for MI0283QT-9A */
    //set_DCX_(0);  = HAL_WRITE32(0xb8000000,0x350c, 0x0);    // SPI0_CMD_GPIO & SPI1_CMD_GPIO are low 		
    //mdelay(1);
    write_reg_cmd(par, 0x01); /* software reset */

	mdelay(5);
	write_reg_cmd(par, 0x28); /* display off */
	/* --------------------------------------------------------- */
	write_reg_cmd(par, 0xCF);
	write_reg(par, 0x00, 0x83, 0x30);
	write_reg_cmd(par, 0xED);
	write_reg(par,  0x64, 0x03, 0x12, 0x81);
	write_reg_cmd(par, 0xE8);
	write_reg(par,  0x85, 0x01, 0x79);
	write_reg_cmd(par, 0xCB);
	write_reg(par,  0x39, 0X2C, 0x00, 0x34, 0x02);
	write_reg_cmd(par, 0xF7);
	write_reg(par,  0x20);
	write_reg_cmd(par, 0xEA);
	write_reg(par, 0x00, 0x00);
	
	/* ------------power control-------------------------------- */
	write_reg_cmd(par, 0xC0);
	write_reg(par,  0x26);
	write_reg_cmd(par, 0xC1);
	write_reg(par, 0x11);
	/* ------------VCOM --------- */
	write_reg_cmd(par, 0xC5);
	write_reg(par,  0x35, 0x3E);
	write_reg_cmd(par, 0xC7);
	write_reg(par, 0xBE);
	/* ------------memory access control------------------------ */
	write_reg_cmd(par, 0x3A);
	write_reg(par, 0x55); /* 16bit pixel */
	/* ------------frame rate----------------------------------- */
	write_reg_cmd(par, 0xB1);
	write_reg(par, 0x00, 0x1B);
	/* ------------Gamma---------------------------------------- */
	/* write_reg(par, 0xF2, 0x08); */ /* Gamma Function Disable */
	write_reg_cmd(par, 0x26);
	write_reg(par, 0x01);
	/* ------------display-------------------------------------- */
	write_reg_cmd(par, 0xB7);
	write_reg(par, 0x07); /* entry mode set */
	write_reg_cmd(par, 0xB6);
	write_reg(par,  0x0A, 0x82, 0x27, 0x00);
	write_reg_cmd(par, 0x36);
    write_reg(par, 0x28);
	write_reg_cmd(par, 0x11); /* sleep out */
	mdelay(100);
	write_reg_cmd(par, 0x29); /* display on */
	mdelay(20);
	printf("\r\n\r\nDone displayr\n\r\n");

//----------
    return;
}

if(5 == pSsiVerifyData->Para6)
{  
	write_reg_cmd(par,0x2a);
    write_reg(par,0x00, 0x00, 0x01, 0x3f);
	write_reg_cmd(par,0x2b);
    write_reg(par, 0x00, 0x00, 0x00, 0xef);
    write_reg_cmd(par,0x2c);           
            //gpio_set_value(2,1);  ??
            //fbtft_write_spi(par, buf, buflen*2);
#if 0
    for(iforLoop=0; iforLoop<(320*240); iforLoop++)
    {
        write_reg_memwr(par, 0xf8, 0x00);
    }
#else
        switch(pSsiVerifyData->Para7)
       {
            case 0:
                for(iforLoop=0; iforLoop<(320*120); iforLoop++)
                {
                    write_reg_memwr(par, 0xf8, 0x00 , 0xf8, 0x00);
                }
                break;
            case 1:
                for(iforLoop=0; iforLoop<(320*120); iforLoop++)
                {
                    write_reg_memwr(par, 0x07, 0xe0 , 0x07, 0xe0);
                }
                break;
            case 2:
                for(iforLoop=0; iforLoop<(320*120); iforLoop++)
                {
                    write_reg_memwr(par, 0x00, 0x1f , 0x00, 0x1f);
                }
                break;
            case 3:
                for(iforLoop=0; iforLoop<(320*120); iforLoop++)
                {
                    write_reg_memwr(par, 0x00, 0x00 , 0x00, 0x00);
                }
                break;
            case 4:
                for(iforLoop=0; iforLoop<(320*30); iforLoop++)
                {
                    write_reg_memwr(par, 0xf8, 0x00 , 0xf8, 0x00);
                }
                for(iforLoop=0; iforLoop<(320*30); iforLoop++)
                {
                    write_reg_memwr(par, 0x07, 0xe0 , 0x07, 0xe0);
                }
                for(iforLoop=0; iforLoop<(320*30); iforLoop++)
                {
                    write_reg_memwr(par, 0x00, 0x1f , 0x00, 0x1f);
                }
                for(iforLoop=0; iforLoop<(320*30); iforLoop++)
                {
                    write_reg_memwr(par, 0x00, 0x00 , 0x00, 0x00);
                }
                break; 
            default:
                break;
            
       }
    
#endif
    mdelay(500);
    printf("\r\n\r\nDown color\n\r\n");
    return;
}

if(6 == pSsiVerifyData->Para6)
{    
    for(iforLoop=0; iforLoop<(6000); iforLoop++)
    {
    u32 posRand = Rand();
    u32 pos_A = posRand%320;
    u32 pos_B = posRand%240;
        
	write_reg_cmd(par,0x2a);
    write_reg(par,(pos_A & 0xff00) >> 8 , (pos_A & 0xff), 0x01, 0x3f);
	write_reg_cmd(par,0x2b);
    write_reg(par,(pos_B & 0xff00) >> 8 , (pos_B & 0xff), 0x00, 0xef);

    write_reg_cmd(par,0x2c);    
        switch(iforLoop%3)
       {
            case 0:
                write_reg(par, 0xf8, 0x00);
                break;
            case 1:
                write_reg(par, 0x07, 0xe0);
                break;
            case 2:
                write_reg(par, 0x00, 0x1f);
                break;                
            default:
                break;
            
       }
    }
    mdelay(500);
    printf("\r\n\r\nDown three colors with randon position\n\r\n");
    return;
}
#endif

    TestFrameFormat = pSsiVerifyData->Para0;
    TestType        = pSsiVerifyData->Para3;
    MICROWIRE       = _FALSE;
    TI_SSP          = _FALSE;

    if (TestFrameFormat == FRF_TI_SSP)
        TI_SSP = _TRUE;

    if (TestFrameFormat == FRF_NS_MICROWIRE)
        MICROWIRE = _TRUE;

printf("XDXDXD carl\n");
#ifdef LOGD
#undef LOGD 
#define LOGD printf
#else
#error_no_define_LOGO
#endif

    /* Replace with new PinCtrl API */
    //SsiIpEnable();  //Only disable JTAG
printf("%s(%d) Carl - TODO:check GdmaIpEnable()\n", __FUNCTION__, __LINE__);
    //GdmaIpEnable();

    LOGD("Baudrate Divider = %d\n", pSsiVerifyData->Para4 * BAUDR_MULTIPLIER);
    LOGD("RsvdMemForTx[%d] Address: %X\n", DATA_SIZE, RsvdMemForTx);
    LOGD("RsvdMemForRx[%d] Address: %X\n", DATA_SIZE, RsvdMemForRx);

#if 0	// Carl 20150226
unsigned int iCheckCount = 0;
for (iCheckCount = 0; iCheckCount<DATA_SIZE;iCheckCount++)
{
	LOGD("Check_Point_DATA(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iCheckCount], RsvdMemForRx[iCheckCount]);
}
#endif

    /* SsiGenerateRandomTxData(RsvdMemForTx, DATA_SIZE, SSI_TEST_SRCDATA_SEQ, MICROWIRE);  // Sequential */
    SsiGenerateRandomTxData(RsvdMemForTx, DATA_SIZE, SSI_TEST_SRCDATA_RND, MICROWIRE);  // Random
    memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));

#if 0	// Carl 20150226
//unsigned int iCheckCount = 0;
for (iCheckCount = 0; iCheckCount<DATA_SIZE;iCheckCount++)
{
	LOGD("Check_Point_DATA_(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iCheckCount], RsvdMemForRx[iCheckCount]);
}
#endif

#if 1	// Carl 20150116
unsigned int iJustCount = 0;
for (iJustCount = 0; iJustCount<4;iJustCount++)
{
	LOGD("Check_Point(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iJustCount], RsvdMemForRx[iJustCount]);
}
#endif

    /**
     * Only SPI0 can be Slave, SPI1 and SPI2 are always as Master.
     */
    PSSI_ADAPTER pSsiAdapter_Slave  = &SsiAdapter_Slave;
    PSSI_ADAPTER pSsiAdapter_Master = &SsiAdapter_Master;
    PHAL_SSI_ADAPTER pHalSsiAdapter_Slave = &HalSsiAdapter_Slave;
    PHAL_SSI_ADAPTER pHalSsiAdapter_Master = &HalSsiAdapter_Master;

    /* SSI OP Init */
    PHAL_SSI_OP pHalSsiOp = (PHAL_SSI_OP) &HalSsiOp;
    HalSsiOpInit((VOID*)(pHalSsiOp));
printf("%s(%d) Carl\n", __FUNCTION__, __LINE__);
    /* SSI0 Basic Configuration */
    pHalSsiAdapter_Slave->Index        = pSsiVerifyData->Para1;
    pHalSsiAdapter_Slave->PinmuxSelect = SSI0_MUX_TO_GPIOE;
    pSsiAdapter_Slave->pHalSsiAdapter  = pHalSsiAdapter_Slave;
    pSsiAdapter_Slave->pHalSsiOp       = pHalSsiOp;

    /* SSI1 Basic Configuration */
    pHalSsiAdapter_Master->Index        = pSsiVerifyData->Para2;
    pHalSsiAdapter_Master->PinmuxSelect = SSI1_MUX_TO_GPIOB;
    pSsiAdapter_Master->pHalSsiAdapter  = pHalSsiAdapter_Master;
    pSsiAdapter_Master->pHalSsiOp       = pHalSsiOp;
    /* Select Pinmux */
    printf("%s(%d) Carl - TODO:check HalSsiPinmuxEnable()\n", __FUNCTION__, __LINE__);

    // enable SSI1 to master
    u4Byte sys_base_addr = 0xB8000000;
    u4Byte sys_addr_offset = 0x50;
#if 1	//0129 patch	// Carl TODO check
// 0x50[8] BIT_SSI0_TX_OEN: 0=>enable
// 0x50[7] BIT_SSI1_TX_OEN: 0=>enable
// 0x50[6] BIT_SSI0_SSI_MAS_SEL: 1=> SSI0 is master ; 0=> SSI0 is slave. 
// 0x50[5] BIT_SSI1_SSI_MAS_SEL: 1=> SSI1 is master ; 0=> SSI1 is slave. 
    u4Byte tmp_u4val = HAL_READ32(sys_base_addr, sys_addr_offset);
    printf("%s(%d):  0x%x(0x%x) \n", __FUNCTION__, __LINE__, sys_base_addr+sys_addr_offset, tmp_u4val);
////    HAL_WRITE8(sys_base_addr, sys_addr_offset, 0x2c);	// [8:5]= 4b'0001
    //HAL_WRITE8(sys_base_addr, sys_addr_offset, 0xac);	// [8:5]= 4b'0101	=> disable SSI1_output_enable for RX
    printf("%s(%d):  0x%x(0x%x) \n", __FUNCTION__, __LINE__, sys_base_addr+sys_addr_offset, tmp_u4val);
#endif
#if 0   // 0331 patch for 0310 ???   only for trytry see 
    HAL_WRITE32_SYS(sys_addr_offset,0x12c); //0xac  0x12c 
#endif
    u1Byte tmp_val = HAL_READ8(sys_base_addr, sys_addr_offset);
    HAL_WRITE8(sys_base_addr, sys_addr_offset, tmp_val | 0x20);
    printf("%s(%d):  0x%x(0x%x) \n", __FUNCTION__, __LINE__, sys_base_addr+sys_addr_offset, HAL_READ8(sys_base_addr, sys_addr_offset));
    printf("%s(%d):  0x%x(0x%x) \n", __FUNCTION__, __LINE__, SYSTEM_REG_BASE+sys_addr_offset, HAL_READ32_SYS(sys_addr_offset));
	
    //pHalSsiOp->HalSsiPinmuxEnable((VOID*)pHalSsiAdapter_Slave);
    //pHalSsiOp->HalSsiPinmuxEnable((VOID*)pHalSsiAdapter_Master);
//!!!!
printf("%s(%d) Carl - TODO:check SsiDumpReg()\n", __FUNCTION__, __LINE__);
//SsiDumpReg();
printf("%s(%d) Carl - TODO:check SsiIpEnable()\n", __FUNCTION__, __LINE__);
//SsiIpEnable();  //Only disable JTAG when SSI0 mux to GPIOE
printf("%s(%d) Carl\n", __FUNCTION__, __LINE__);
//!!!! Check JTAG Status
#if 0	 //TODO:check
RegValue = HAL_READ32(PERI_ON_BASE, REG_CPU_PERIPHERAL_CTRL);
LOGD("	REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X (b8=0)\n", REG_CPU_PERIPHERAL_CTRL, RegValue);
#endif
printf("%s(%d) Carl\n", __FUNCTION__, __LINE__);
    /**
     * Motorola SPI Configurations
     */
    pHalSsiOp->HalSsiDisable((VOID*)pHalSsiAdapter_Slave);
    pHalSsiOp->HalSsiDisable((VOID*)pHalSsiAdapter_Master);
//!!!! Workaround

#if 0	 //TODO:check
RegValue = HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL);
LOGD("	[B]REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X\n", REG_SPI_MUX_CTRL, RegValue);
RegValue |= BIT0;  //SPI0 Pin Enable
HAL_WRITE32(PERI_ON_BASE, REG_SPI_MUX_CTRL, RegValue);
RegValue = HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL);
LOGD("	[A]REG_SOC_PERI_FUNC0_EN(0x%03X): 0x%X\n", REG_SPI_MUX_CTRL, RegValue);
#endif

    /* SSI Configuration */
    /* SSI 1 - Master */
    pHalSsiAdapter_Master->Role              = SSI_MASTER;
    pHalSsiAdapter_Master->DataFrameSize     = DFS_8_BITS;	// 16 9
    pHalSsiAdapter_Master->DataFrameFormat   = FRF_MOTOROLA_SPI;
    pHalSsiAdapter_Master->SclkPhase         = SCPH_TOGGLES_IN_MIDDLE;//SCPH_TOGGLES_IN_MIDDLE;
    //pHalSsiAdapter_Master->SclkPolarity      = SCPOL_INACTIVE_IS_HIGH;
    pHalSsiAdapter_Master->SclkPolarity      = SCPOL_INACTIVE_IS_LOW;
    //
    /* pHalSsiAdapter_Master->TransferMode      = TMOD_TR; */
    pHalSsiAdapter_Master->TransferMode      = TMOD_TO;
    /* pHalSsiAdapter_Master->TransferMode      = TMOD_RO; */
    //
    pHalSsiAdapter_Master->ControlFrameSize  = CFS_1_BIT;	// microwire 16bits   9?
    pHalSsiAdapter_Master->SlaveSelectEnable = 0x1;  // <----
    pHalSsiAdapter_Master->ClockDivider      = pSsiVerifyData->Para4 * BAUDR_MULTIPLIER;   // <----
    pHalSsiAdapter_Master->TxThresholdLevel  = 1;
    pHalSsiAdapter_Master->RxThresholdLevel  = 0;
    pHalSsiAdapter_Master->InterruptMask     = 0;
    /* pHalSsiAdapter_Master->DataFrameNumber   = */  // <---- Master && (Receive Only || EEPROM Read)

    /* SSI 0 - Slave */
    pHalSsiAdapter_Slave->Role              = SSI_SLAVE;
    pHalSsiAdapter_Slave->DataFrameSize     = DFS_8_BITS;	// 16 9
    pHalSsiAdapter_Slave->DataFrameFormat   = FRF_MOTOROLA_SPI;
    pHalSsiAdapter_Slave->SclkPhase         = SCPH_TOGGLES_IN_MIDDLE; //SCPH_TOGGLES_IN_MIDDLE;
    //pHalSsiAdapter_Slave->SclkPolarity      = SCPOL_INACTIVE_IS_HIGH;
    pHalSsiAdapter_Slave->SclkPolarity      = SCPOL_INACTIVE_IS_LOW;
    //
    /* pHalSsiAdapter_Slave->TransferMode      = TMOD_TR; */
    /* pHalSsiAdapter_Slave->TransferMode      = TMOD_TO; */
    pHalSsiAdapter_Slave->TransferMode      = TMOD_RO;
    //
    /* pHalSsiAdapter_Slave->SlaveOutputEnable = SLV_TXD_DISABLE;  // <---- */
    pHalSsiAdapter_Slave->SlaveOutputEnable = SLV_TXD_ENABLE;  // <----
    //
    pHalSsiAdapter_Slave->ControlFrameSize  = CFS_1_BIT;
    pHalSsiAdapter_Slave->TxThresholdLevel  = 1;
    pHalSsiAdapter_Slave->RxThresholdLevel  = 0;
    pHalSsiAdapter_Slave->InterruptMask     = 0;

    if (TI_SSP) {
        pHalSsiAdapter_Master->DataFrameFormat = FRF_TI_SSP;
        pHalSsiAdapter_Slave->DataFrameFormat  = FRF_TI_SSP;
    }

    if (MICROWIRE) {
        pHalSsiAdapter_Master->DataFrameFormat = FRF_NS_MICROWIRE;
        pHalSsiAdapter_Master->TransferMode    = TMOD_TR;
        pHalSsiAdapter_Master->DataFrameNumber = 63;
        pHalSsiAdapter_Master->MicrowireHandshaking  = MW_HANDSHAKE_DISABLE;
        pHalSsiAdapter_Master->MicrowireDirection    = MW_DIRECTION_MASTER_TO_SLAVE;
	 pHalSsiAdapter_Master->MicrowireDirection    = pSsiVerifyData->Para7;
        //
        pHalSsiAdapter_Master->MicrowireTransferMode = MW_TMOD_NONSEQUENTIAL;
        /* pHalSsiAdapter_Master->MicrowireTransferMode = MW_TMOD_SEQUENTIAL; */
	 pHalSsiAdapter_Master->ControlFrameSize  = CFS_9_BITS;	// microwire 16bits   9?

        pHalSsiAdapter_Slave->DataFrameFormat  = FRF_NS_MICROWIRE;
        pHalSsiAdapter_Slave->TransferMode     = TMOD_TR;
        pHalSsiAdapter_Slave->DataFrameNumber = 63;
        /* pHalSsiAdapter_Slave->MicrowireDirection    = MW_DIRECTION_SLAVE_TO_MASTER; */
        //
        pHalSsiAdapter_Slave->MicrowireTransferMode = MW_TMOD_NONSEQUENTIAL;
        /* pHalSsiAdapter_Slave->MicrowireTransferMode = MW_TMOD_SEQUENTIAL; */
	 pHalSsiAdapter_Slave->ControlFrameSize  = CFS_16_BITS;
    }

    /**
     * Motorola SPI Interrupts Configurations
     * BIT_IMR_MSTIM | BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM | BIT_IMR_TXOIM | BIT_IMR_TXEIM
     */

    pHalSsiAdapter_Master->InterruptMask = 0x3F;
    pHalSsiAdapter_Slave->InterruptMask  = 0x1F;

	
    if ((TestType == SSI_TT_INTERRUPT) || (TestType == SSI_TT_DMA_TO_TX) || (TestType == SSI_TT_RX_TO_DMA)) {
        IRQ_HANDLE  SsiIrqHandle_Slave;
        IRQ_HANDLE  SsiIrqHandle_Master;

        if (TestType == SSI_TT_INTERRUPT) {
            pHalSsiAdapter_Master->InterruptMask = 0;
            pHalSsiAdapter_Slave->InterruptMask  = BIT_IMR_RXFIM;
        }

        if (TestType == SSI_TT_DMA_TO_TX) {
            pHalSsiAdapter_Master->InterruptMask = 0;
            pHalSsiAdapter_Slave->InterruptMask  = BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_TXOIM;
        }

        if (TestType == SSI_TT_RX_TO_DMA) {
            pHalSsiAdapter_Master->InterruptMask = 0;
            pHalSsiAdapter_Slave->InterruptMask  = 0;
        }

        /* Common Interrupt Configurations */
        SsiIrqHandle_Master.Data     = (u32) (pSsiAdapter_Master);
        SsiIrqHandle_Master.IrqNum   = GetSsiIrqNumber(pHalSsiAdapter_Master->Index);
        SsiIrqHandle_Master.IrqFun   = (IRQ_FUN) SsiAIrqHandle;
        SsiIrqHandle_Master.Priority = 0;
		
        //InterruptRegister(&SsiIrqHandle_Master);	20150116
        //InterruptEn(&SsiIrqHandle_Master);
        irq_install_handler(SsiIrqHandle_Master.IrqNum, SsiIrqHandle_Master.IrqFun, SsiIrqHandle_Master.Data);

        SsiIrqHandle_Slave.Data     = (u32) (pSsiAdapter_Slave);
        SsiIrqHandle_Slave.IrqNum   = GetSsiIrqNumber(pHalSsiAdapter_Slave->Index);
        SsiIrqHandle_Slave.IrqFun   = (IRQ_FUN) SsiAIrqHandle;
        SsiIrqHandle_Slave.Priority = 0;
        //InterruptRegister(&SsiIrqHandle_Slave);
        //InterruptEn(&SsiIrqHandle_Slave);
	 irq_install_handler(SsiIrqHandle_Slave.IrqNum, SsiIrqHandle_Slave.IrqFun, SsiIrqHandle_Slave.Data);
    }

    /* SSI Adapter GDMA Configurations */
    if ((TestType == SSI_TT_DMA_TO_TX) || (TestType == SSI_TT_RX_TO_DMA)) {

        if (TestType == SSI_TT_DMA_TO_TX) {
            pHalSsiAdapter_Master->DmaControl = SSI_TXDMA_ENABLE;
            pHalSsiAdapter_Slave->DmaControl = SSI_NODMA;
        }

        if (TestType == SSI_TT_RX_TO_DMA) {
            pHalSsiAdapter_Master->DmaControl = SSI_NODMA;
            pHalSsiAdapter_Slave->DmaControl  = SSI_RXDMA_ENABLE;
        }

        /* Common Configs */
        pHalSsiAdapter_Master->DmaTxDataLevel = 48;
        pHalSsiAdapter_Master->DmaRxDataLevel =  7;
        pHalSsiAdapter_Slave->DmaTxDataLevel  = 48;
        pHalSsiAdapter_Slave->DmaRxDataLevel  =  7;
#if DEBUG_GDMA_CONFIG
        LOGD("pHalSsiAdapter_Master->DmaControl     = %d\n", pHalSsiAdapter_Master->DmaControl);
        LOGD("pHalSsiAdapter_Master->DmaTxDataLevel = %d\n", pHalSsiAdapter_Master->DmaTxDataLevel);
        LOGD("pHalSsiAdapter_Slave->DmaControl      = %d\n", pHalSsiAdapter_Slave->DmaControl);
        LOGD("pHalSsiAdapter_Slave->DmaRxDataLevel  = %d\n", pHalSsiAdapter_Slave->DmaRxDataLevel);
#endif
    }

    /* Initial SSI Configuration & Enable SSI */
    pHalSsiOp->HalSsiInit((VOID*)pHalSsiAdapter_Slave);
    pHalSsiOp->HalSsiInit((VOID*)pHalSsiAdapter_Master);

    pHalSsiOp->HalSsiEnable((VOID*)pHalSsiAdapter_Slave);
    pHalSsiOp->HalSsiEnable((VOID*)pHalSsiAdapter_Master);

    /**
     * GDMA Configurations
     */
    PGDMA_ADAPTER     pGdmaAdapter_Tx = &GdmaAdapter_Tx;
    PGDMA_ADAPTER     pGdmaAdapter_Rx = &GdmaAdapter_Rx;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter_Tx = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter_Tx;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter_Rx = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter_Rx;
    PHAL_GDMA_OP      pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOpSPI;
    IRQ_HANDLE        Gdma0IrqHandle_Tx;
    IRQ_HANDLE        Gdma0IrqHandle_Rx;
    u16 *pSrc=NULL, *pDst=NULL;
    u8  GdmaIndex_Tx, GdmaChannel_Tx;
    u8  GdmaIndex_Rx, GdmaChannel_Rx;
    u8  GdmaType;
    u8  MaxLlp = 1;


    if ((TestType == SSI_TT_DMA_TO_TX) || (TestType == SSI_TT_RX_TO_DMA)) {
        LOGI("GDMA Configurations (Tx/Rx)\n");
        pGdmaAdapter_Tx->pHalGdmaOp = pHalGdmaOp;
        pGdmaAdapter_Tx->pHalGdmaAdapter = pHalGdmaAdapter_Tx;
        pGdmaAdapter_Rx->pHalGdmaOp = pHalGdmaOp;
        pGdmaAdapter_Rx->pHalGdmaAdapter = pHalGdmaAdapter_Rx;

        HalGdmaOpInit((VOID*)pHalGdmaOp);
        memset((void*)pHalGdmaAdapter_Tx, 0, sizeof(HAL_GDMA_ADAPTER));
        memset((void*)pHalGdmaAdapter_Rx, 0, sizeof(HAL_GDMA_ADAPTER));
        /* pHalGdmaOp->HalGdamChInit((VOID*)(pHalGdmaAdapter_Tx)); */
        /* pHalGdmaOp->HalGdamChInit((VOID*)(pHalGdmaAdapter_Rx)); */
    }

    switch (TestType) {
        case SSI_TT_DMA_TO_TX:
        {
            LOGI("GDMA Configurations (Tx)(SSI_TT_DMA_TO_TX)\n");
            pSrc = RsvdMemForTx;
            pDst = (u16*) (SSI1_REG_BASE + REG_DW_SSI_DR);  //Master(SPI1)(Tx)
            GdmaIndex_Tx   = 0;
            GdmaChannel_Tx = 0; // TODO: ??/
            GdmaType = SSI_TT_DMA_TO_TX;
            LOGD("pSrc: %X, pDst: %X, GdmaType: %d\n", pSrc, pDst, GdmaType);
            /**
             * Gdma Interrupt Configuration
             */
            Gdma0IrqHandle_Tx.Data = (u32) (pGdmaAdapter_Tx);
            //Gdma0IrqHandle_Tx.IrqNum = GetGdmaChannelIrq(GdmaIndex_Tx, GdmaChannel_Tx);
            Gdma0IrqHandle_Tx.IrqNum = DW_GDMA_IP_IRQ;
            Gdma0IrqHandle_Tx.IrqFun = (IRQ_FUN) SsiTxGdmaIrqHandle;
            Gdma0IrqHandle_Tx.Priority = 0;
            //InterruptRegister(&Gdma0IrqHandle_Tx);
            //InterruptEn(&Gdma0IrqHandle_Tx);
            irq_install_handler(Gdma0IrqHandle_Tx.IrqNum, Gdma0IrqHandle_Tx.IrqFun, Gdma0IrqHandle_Tx.Data);

/*
                Gdma0IrqHandle.Data = (u32) (pGdmaAdapte);
                Gdma0IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
                Gdma0IrqHandle.IrqFun = (IRQ_FUN) Gdma0CombineIrqHandle;	//Gdma0Ch0IrqHandle
                Gdma0IrqHandle.Priority = 0;

                //InterruptRegister(&Gdma0IrqHandle);
                //InterruptEn(&Gdma0IrqHandle);
		  irq_install_handler(Gdma0IrqHandle.IrqNum, Gdma0IrqHandle.IrqFun, Gdma0IrqHandle.Data);
*/

            /**
             * Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.)
             *  64 = SSI_RX_FIFO_DEPTH
             * 256 = DATA_SIZE
             */
              // TODO: Why is DATA_SIZE/2 ?
            pHalGdmaAdapter_Tx->GdmaCtl.BlockSize = DATA_SIZE / 2;
            //
            /* Tx Specific Configs */
            pHalGdmaAdapter_Tx->GdmaCtl.TtFc      = TTFCMemToPeri;  // 0x01
            //pHalGdmaAdapter_Tx->GdmaCtl.TtFc      = TTFCMemToMem;
            /*pHalGdmaAdapter_Tx->GdmaCfg.ReloadDst = 1; */
            pHalGdmaAdapter_Tx->GdmaCfg.DestPer   = 6;//6;  //<---- SPI1 Tx
            //
            pHalGdmaAdapter_Tx->GdmaCtl.Done      = 1;
            pHalGdmaAdapter_Tx->MuliBlockCunt     = 0;
            pHalGdmaAdapter_Tx->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
            pHalGdmaAdapter_Tx->ChSar = (u32)pSrc;  //<----
            pHalGdmaAdapter_Tx->ChDar = (u32)pDst;  //<----
            pHalGdmaAdapter_Tx->GdmaIndex   = GdmaIndex_Tx;
            pHalGdmaAdapter_Tx->ChNum       = GdmaChannel_Tx;
            pHalGdmaAdapter_Tx->ChEn        = GetGdmaChannelEnableValue(GdmaChannel_Tx);
            pHalGdmaAdapter_Tx->GdmaIsrType = (BlockType|TransferType|ErrType);
            pHalGdmaAdapter_Tx->IsrCtrl     = ENABLE;
            pHalGdmaAdapter_Tx->GdmaOnOff   = ON;
            pHalGdmaAdapter_Tx->TestItem    = GdmaType;
#if DEBUG_GDMA_CONFIG
            LOGD("pHalGdmaAdapter_Tx->GdmaIndex = %d\n", pHalGdmaAdapter_Tx->GdmaIndex);
            LOGD("pHalGdmaAdapter_Tx->ChNum     = %d\n", pHalGdmaAdapter_Tx->ChNum);
            LOGD("pHalGdmaAdapter_Tx->ChEn      = %04X\n", pHalGdmaAdapter_Tx->ChEn);
#endif
            pHalGdmaAdapter_Tx->GdmaCtl.IntEn      = ENABLE;
            pHalGdmaAdapter_Tx->GdmaCtl.SrcMsize   = MsizeFour;
            pHalGdmaAdapter_Tx->GdmaCtl.DestMsize  = MsizeEight;
            pHalGdmaAdapter_Tx->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
            pHalGdmaAdapter_Tx->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
            pHalGdmaAdapter_Tx->GdmaCtl.Dinc = NoChange;
            pHalGdmaAdapter_Tx->GdmaCtl.Sinc = IncType;

            break;
        }
        case SSI_TT_RX_TO_DMA:
        {
            LOGI("GDMA Configurations (Rx)(SSI_TT_RX_TO_DMA)\n");
            pSrc = (u16*) (SSI0_REG_BASE + REG_DW_SSI_DR);
            pDst = RsvdMemForRx;
            GdmaIndex_Rx   = 0;
            GdmaChannel_Rx = 0; // Carl 20150303  2=>0
            GdmaType = SSI_TT_RX_TO_DMA;
            LOGD("pSrc: %X, pDst: %X, GdmaType: %d\n", pSrc, pDst, GdmaType);
            /**
             * Gdma Interrupt Configuration
             */
            Gdma0IrqHandle_Rx.Data = (u32) (pGdmaAdapter_Rx);
            //Gdma0IrqHandle_Rx.IrqNum = GetGdmaChannelIrq(GdmaIndex_Rx, GdmaChannel_Rx);
            Gdma0IrqHandle_Rx.IrqNum = DW_GDMA_IP_IRQ;
            Gdma0IrqHandle_Rx.IrqFun = (IRQ_FUN) SsiRxGdmaIrqHandle;
            Gdma0IrqHandle_Rx.Priority = 0;
            //InterruptRegister(&Gdma0IrqHandle_Rx);
            //InterruptEn(&Gdma0IrqHandle_Rx);
            irq_install_handler(Gdma0IrqHandle_Rx.IrqNum, Gdma0IrqHandle_Rx.IrqFun, Gdma0IrqHandle_Rx.Data);

            /**
             * Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.)
             *  64 = SSI_RX_FIFO_DEPTH
             * 256 = DATA_SIZE
             */
            pHalGdmaAdapter_Rx->GdmaCtl.BlockSize = DATA_SIZE;
            //
            /* Rx Specific Configs */
            pHalGdmaAdapter_Rx->GdmaCtl.TtFc      = TTFCPeriToMem;  // 0x02
            pHalGdmaAdapter_Rx->GdmaCfg.ReloadSrc = 1;  //<---- Rx --> DMA needed
            pHalGdmaAdapter_Rx->GdmaCfg.SrcPer    = 5;  //<---- SPI0 Rx
            //
            pHalGdmaAdapter_Rx->GdmaCtl.Done      = 1;
            pHalGdmaAdapter_Rx->MuliBlockCunt     = 0;
            pHalGdmaAdapter_Rx->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
            pHalGdmaAdapter_Rx->ChSar = (u32)pSrc;  //<----
            pHalGdmaAdapter_Rx->ChDar = (u32)pDst;  //<----
            pHalGdmaAdapter_Rx->GdmaIndex   = GdmaIndex_Rx;
            pHalGdmaAdapter_Rx->ChNum       = GdmaChannel_Rx;
            pHalGdmaAdapter_Rx->ChEn        = GetGdmaChannelEnableValue(GdmaChannel_Tx);
            pHalGdmaAdapter_Rx->GdmaIsrType = (BlockType|TransferType|ErrType);
            pHalGdmaAdapter_Rx->IsrCtrl     = ENABLE;
            pHalGdmaAdapter_Rx->GdmaOnOff   = ON;
            pHalGdmaAdapter_Rx->TestItem    = GdmaType;
#if DEBUG_GDMA_CONFIG
            LOGD("pHalGdmaAdapter_Rx->GdmaIndex = %d\n", pHalGdmaAdapter_Rx->GdmaIndex);
            LOGD("pHalGdmaAdapter_Rx->ChNum     = %d\n", pHalGdmaAdapter_Rx->ChNum);
            LOGD("pHalGdmaAdapter_Rx->ChEn      = %04X\n", pHalGdmaAdapter_Rx->ChEn);
#endif
            pHalGdmaAdapter_Rx->GdmaCtl.IntEn      = ENABLE;
            pHalGdmaAdapter_Rx->GdmaCtl.SrcMsize   = MsizeEight;
            pHalGdmaAdapter_Rx->GdmaCtl.DestMsize  = MsizeFour;
            pHalGdmaAdapter_Rx->GdmaCtl.SrcTrWidth = TrWidthTwoBytes;
            pHalGdmaAdapter_Rx->GdmaCtl.DstTrWidth = TrWidthFourBytes;
            pHalGdmaAdapter_Rx->GdmaCtl.Dinc = IncType;
            pHalGdmaAdapter_Rx->GdmaCtl.Sinc = NoChange;
            break;
        }
        default:
            break;
    }

    /**
     * Fire in the hole!
     */
    switch (TestType)
    {
        case SSI_TT_BASIC_IO:
        {
            LOGI(ANSI_COLOR_YELLOW"Test Type: SSI_TT_BASIC_IO(%d)\n"ANSI_COLOR_RESET, SSI_TT_BASIC_IO);
            TestTimes = pSsiVerifyData->Para8;	//SSI_TX_FIFO_DEPTH;
            Counter   = 0;
			//
			WaitCounter = 0;

            //pHalSsiAdapter_Master->TxData = 0xAA;
            //pHalSsiAdapter_Slave->TxData = 0x33;

printf("%s(%d) Carl - TODO:check Send data...\n", __FUNCTION__, __LINE__);
//break;

#if 0	// Carl
printf("%s(%d) Carl - config down, ready for SPI TXRX....\n", __FUNCTION__, __LINE__);
break;
#endif

	unsigned int cnt0 = 0x0;
	unsigned int cnt1 = 0xFFFF;		// 0xFF
            LOGI(ANSI_COLOR_GREEN"Send data...\n"ANSI_COLOR_RESET);
            while (Counter < TestTimes) {
                //pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master);	// Carl 20150114

#if 1
	            pHalSsiAdapter_Master->TxData = cnt0;
	            pHalSsiAdapter_Slave->TxData = cnt1;
			cnt0++;
			if(  1 == Counter%2 ){cnt0--;}	// Carl 20150202
			cnt1--;
#else
	            pHalSsiAdapter_Master->TxData = Rand() % 65536;
	            pHalSsiAdapter_Slave->TxData = Rand() % 65536;
		if(Counter < 8)
		     printf("%s(%d) Carl - Master TX data= 0x%08x\n", __FUNCTION__, __LINE__,pHalSsiAdapter_Master->TxData);

#endif
				
                pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Slave);
		   pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master);
                ++Counter;
            }

            /**
             * Wait Slave transfer complete.
             */
            do {
                LOGI("Master/Slave Receiving...\n");
                RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master)) |
                    (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave));
				//
				WaitCounter++;
				if (WaitCounter > 40)
					break;
            }
            while (RegValue & BIT_SR_BUSY);

            /**
             * Master --> Slave
             * Receive Slave Rx FIFO Data
             */
            Counter = 0;
            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave)) & BIT_SR_RFNE;

            while (RegValue) {  //Rx Fifo Not Empty
                LOGD("[M->S] Check Slave RecvFifoNotEmpty(RFNE) Status = %d\n", RegValue);

                RegValue = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter_Slave);
                LOGD("\t"
                        ANSI_COLOR_MAGENTA"%d"ANSI_COLOR_RESET
                        " valid data entries in Slave(SPI%d) RxFIFO\n", RegValue, pHalSsiAdapter_Slave->Index);

#if 0	// Carl
printf("%s(%d) Carl - M->S break....\n", __FUNCTION__, __LINE__);
break;
#endif

                while (RegValue--) {
                    ++Counter;
                    LOGD("\t\tRecvData = %04X\n", pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter_Slave));
                }

                RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave)) & BIT_SR_RFNE;
            }
            LOGD("Slave Rx FIFO Receive Counter = "ANSI_COLOR_MAGENTA"%d"ANSI_COLOR_RESET"\n", Counter);

            /**
             * Slave --> Master
             * Receive Master Rx FIFO Data
             */
            Counter = 0;
            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master)) & BIT_SR_RFNE;

            while (RegValue) {
                LOGD("[S->M] Check Master RecvFifoNotEmpty(RFNE) Status = %d\n", RegValue);

                RegValue = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter_Master);
                LOGD("\t%"
                        ANSI_COLOR_MAGENTA"%d"ANSI_COLOR_RESET
                        " valid data entries in Master(SPI%d) RxFIFO\n", RegValue, pHalSsiAdapter_Master->Index);

                while (RegValue--) {
                    ++Counter;
                    LOGD("\t\tRecvData = %04X\n", pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter_Master));
                }

                RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master)) & BIT_SR_RFNE;
            }
            LOGD("Master Rx FIFO Receive Counter = "ANSI_COLOR_MAGENTA"%d"ANSI_COLOR_RESET"\n", Counter);

            break;
        }
        case SSI_TT_INTERRUPT:
        {
            LOGI(ANSI_COLOR_YELLOW"Test Type: SSI_TT_INTERRUPT(%d)\n"ANSI_COLOR_RESET, SSI_TT_INTERRUPT);
            TestLoops = 0;

            while (TestLoops < 1) {
                /* Reset Global Interrupt Test variable value */
                Counter = 0;
                RsvdMemForRx_Index = 0;
                RxTransferDone = _FALSE;
                memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));


#if 1	// Carl 20150116
//unsigned int iJustCount = 0;
for (iJustCount = 0; iJustCount<4;iJustCount++)
{
	LOGD("Check_Point(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iJustCount], RsvdMemForRx[iJustCount]);
}
#endif


                LOGI(ANSI_COLOR_GREEN"Send data from Master to  Slave(SPI0)\n"ANSI_COLOR_RESET);
                while (Counter < DATA_SIZE) {
                    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master);

                    if (RegValue & BIT_SR_TFNF) {
                        pHalSsiAdapter_Master->TxData = RsvdMemForTx[Counter++];
                        pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master);  // Master --> Slave
                    }
                    else {
                        LOGI(ANSI_COLOR_YELLOW"Tx FIFO Full...\n"ANSI_COLOR_RESET);
                    }
                }

                do {
                    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave);
                    LOGI("Slave Receiving...\n");
                }
                while (RegValue & BIT_SR_BUSY);

#if 1	// Carl 20150116
//unsigned int iJustCount = 0;
for (iJustCount = 0; iJustCount<4;iJustCount++)
{
	LOGD("Check_Point(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iJustCount], RsvdMemForRx[iJustCount]);
}
#endif

                while (1) {
			printf("%s(%d)\n", __FUNCTION__, __LINE__);
			//break; // TODO: ....just break it...	20150116
                    if (RxTransferDone == _TRUE) {
                        if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                            LOGT(ANSI_COLOR_CYAN"SSI_TT_INTERRUPT Success!\n"ANSI_COLOR_RESET);
                        else
                            LOGT(ANSI_COLOR_RED"SSI_TT_INTERRUPT Failed!\n"ANSI_COLOR_RESET);

                        break;
                    }
                }

                LOGD(ANSI_COLOR_CYAN"RsvdMemForRx_Index: %d\n"ANSI_COLOR_RESET, RsvdMemForRx_Index);
                TestLoops++;
                LOGI(ANSI_COLOR_RED"Round %d\n"ANSI_COLOR_RESET, TestLoops);
            } //while loop

            break;
        }
        case SSI_TT_DMA_TO_TX:
        {

#if 1
    u32 start = (u32)RsvdMemForTx & ~(ARCH_DMA_MINALIGN - 1);
    u32 end = (u32)(RsvdMemForTx + DATA_SIZE*2);
    
    printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, start, end);
    
    #if 1
    flush_cache(start, (end-start));
    invalidate_dcache_range(start, end);  // 20141222

    printf("!!!!! 0x%08x, 0x%08x\n",(u32)RsvdMemForTx, (u32)(&RsvdMemForTx[0]));
    #endif
#endif

            LOGI(ANSI_COLOR_YELLOW"Test Type: SSI_TT_DMA_TO_TX(%d)\n"ANSI_COLOR_RESET, SSI_TT_DMA_TO_TX);
            pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter_Tx));
            TestLoops = 0;

            while(TestLoops < 1) {
                /* Reset GDMA Settings */
                pHalGdmaAdapter_Tx->MuliBlockCunt = 0;
                pHalGdmaAdapter_Tx->ChSar = (u32)pSrc;
                pHalGdmaAdapter_Tx->ChDar = (u32)pDst;
                RxTransferDone = _FALSE;
                RsvdMemForRx_Index = 0;
                memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));

                pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter_Tx));
                pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter_Tx));

                LOGI("-- DMA %d Channel 0x%X Enable --\n",
                        pHalGdmaAdapter_Tx->GdmaIndex, GetGdmaChannelEnableValue(GdmaChannel_Tx));
                pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter_Tx));

                WaitCounter = 0;
                /* Slave is busy receiving something... */
                while (RsvdMemForRx_Index < DATA_SIZE) {
                    LOGI("[%d] Not yet, Busy waiting...\n", RsvdMemForRx_Index);
					//
					WaitCounter++;
					if (WaitCounter > 40)
						break;
                }

                WaitCounter = 0;
                while (1) {
                    if (RxTransferDone == _TRUE) {
                        /* LOGD(ANSI_COLOR_RED"Rx Complete !\n"ANSI_COLOR_RESET); */

                        if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                            LOGT(ANSI_COLOR_CYAN"SSI_TT_DMA_TO_TX Success!\n"ANSI_COLOR_RESET);
                        else
                            LOGT(ANSI_COLOR_RED"SSI_TT_DMA_TO_TX Failed!\n"ANSI_COLOR_RESET);

                        /* GDMA Channel Disable and On/Off */
                        pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter_Tx));

                        break;
                    }
					WaitCounter++;
					if (WaitCounter > 40)
						break;
                }

                /* LOGD(ANSI_COLOR_CYAN"RsvdMemForRx_Index: %d\n"ANSI_COLOR_RESET, RsvdMemForRx_Index); */
                TestLoops++;
                LOGI(ANSI_COLOR_RED"Round %d\n"ANSI_COLOR_RESET, TestLoops);

            } //while loop

#if 1	// Carl 20150302
unsigned int iCheckCount = 0;
for (iCheckCount = 0; iCheckCount<DATA_SIZE;iCheckCount++)
{
	LOGD("Check_DATA(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iCheckCount], RsvdMemForRx[iCheckCount]);
}
#endif

#if DEBUG_GDMA_CONFIG
                /**
                 * Print GDMA Settings
                 */
                LOGD("------------------------\n");
                LOGD(ANSI_COLOR_CYAN"GDMA Configurations:\n"ANSI_COLOR_RESET);
                LOGD("\tGdmaCtl.BlockSize : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.BlockSize);
                LOGD("\tGdmaCtl.TtFc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.TtFc);
                LOGD("\tGdmaCtl.Done      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Done);
                LOGD("\tGdmaCfg.ReloadDst : %d\n", pHalGdmaAdapter_Tx->GdmaCfg.ReloadDst);
                LOGD("\tGdmaCfg.DestPer   : %d\n", pHalGdmaAdapter_Tx->GdmaCfg.DestPer);
                LOGD("\tMuliBlockCunt     : %d\n", pHalGdmaAdapter_Tx->MuliBlockCunt);
                LOGD("\tMaxMuliBlock      : %d\n", pHalGdmaAdapter_Tx->MaxMuliBlock);
                LOGD("\tChSar             : %X\n", pHalGdmaAdapter_Tx->ChSar);
                LOGD("\tChDar             : %X\n", pHalGdmaAdapter_Tx->ChDar);
                LOGD("\tGdmaIndex         : %d\n", pHalGdmaAdapter_Tx->GdmaIndex);
                LOGD("\tChNum             : %d\n", pHalGdmaAdapter_Tx->ChNum);
                LOGD("\tChEn              : %X\n", pHalGdmaAdapter_Tx->ChEn);
                LOGD("\tGdmaIsrType       : %d\n", pHalGdmaAdapter_Tx->GdmaIsrType);
                LOGD("\tIsrCtrl           : %d\n", pHalGdmaAdapter_Tx->IsrCtrl);
                LOGD("\tGdmaOnOff         : %d\n", pHalGdmaAdapter_Tx->GdmaOnOff);
                LOGD("\tTestItem          : %d\n", pHalGdmaAdapter_Tx->TestItem);
                LOGD("\tGdmaCtl.IntEn     : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.IntEn);
                LOGD("\tGdmaCtl.SrcMsize  : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.SrcMsize);
                LOGD("\tGdmaCtl.DestMsize : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.DestMsize);
                LOGD("\tGdmaCtl.SrcTrWidth: %d\n", pHalGdmaAdapter_Tx->GdmaCtl.SrcTrWidth);
                LOGD("\tGdmaCtl.DstTrWidth: %d\n", pHalGdmaAdapter_Tx->GdmaCtl.DstTrWidth);
                LOGD("\tGdmaCtl.Dinc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Dinc);
                LOGD("\tGdmaCtl.Sinc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Sinc);
#endif
            break;
        }
        case SSI_TT_RX_TO_DMA:
        {
            LOGI(ANSI_COLOR_YELLOW"Test Type: SSI_TT_RX_TO_DMA(%d)\n"ANSI_COLOR_RESET, SSI_TT_RX_TO_DMA);
            pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter_Rx));

            TestLoops = 0;
            while(TestLoops < 1) {
                /* Reset GDMA Settings */
                pHalGdmaAdapter_Rx->MuliBlockCunt = 0;
                pHalGdmaAdapter_Rx->ChSar = (u32)pSrc;
                pHalGdmaAdapter_Rx->ChDar = (u32)pDst;
                Counter =0;
                RsvdMemForRx_Index = 0;
                memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));

                pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter_Rx));
                pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter_Rx));

                LOGI("-- DMA %d Channel 0x%X Enable --\n",
                        pHalGdmaAdapter_Rx->GdmaIndex, GetGdmaChannelEnableValue(GdmaChannel_Rx));
                pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter_Rx));

                while (Counter < DATA_SIZE) {
                    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master);

                    if (RegValue & BIT_SR_TFNF) {
                        pHalSsiAdapter_Master->TxData = RsvdMemForTx[Counter++];
                        pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master);
                    }
                    else
                        LOGI(ANSI_COLOR_YELLOW"Tx FIFO Full...\n"ANSI_COLOR_RESET);
                }

                /**
                 * Wait Slave transfer complete.
                 */
                do {
                    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave);
                    LOGI("Slave Receiving...\n");  // Need to print some dummy message.
                }
                while (RegValue & BIT_SR_BUSY);

#if 1   // 20150306 invalidate cache
    u32 start = (u32)RsvdMemForRx & ~(ARCH_DMA_MINALIGN - 1);
    //u32 end = (u32)(RsvdMemForRx + DATA_SIZE); // TODO: need to check why add DATA_SIZE

    u32 end = (u32)(RsvdMemForRx + DATA_SIZE + ARCH_DMA_MINALIGN) & ~(ARCH_DMA_MINALIGN - 1);   // 20170412
    
    printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, start, end);
    
    #if 1
    //flush_cache(start, (end-start));
    invalidate_dcache_range(start, end);

    printf("!!!!! 0x%08x, 0x%08x ~ 0x%08x\n",(u32)RsvdMemForRx, (u32)(&RsvdMemForRx[0]), (u32)(&RsvdMemForRx[DATA_SIZE-1]));
    #endif
#endif

#if 1   // 20170412
                RegValue = 0;
                do {
                    RegValue ++;
                    LOGI("wait...wait...");  // Need to print some dummy message.
                }
                while (RegValue < 200);
                LOGI("\n"); 
#endif

                /**
                 * No need to wait some signal.
                 * Assuming DMA is fast enougth to finish the job.
                 */
                if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                    LOGT(ANSI_COLOR_CYAN"SSI_TT_RX_TO_DMA Success!\n"ANSI_COLOR_RESET);
                else
                    LOGT(ANSI_COLOR_RED"SSI_TT_RX_TO_DMA Failed!\n"ANSI_COLOR_RESET);

                /* Too fast ??? */
                /* GDMA Channel Disable and On/Off */
                pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter_Rx));

                TestLoops++;
                LOGI(ANSI_COLOR_RED"Round %d\n"ANSI_COLOR_RESET, TestLoops);
            } //while loop
#if DEBUG_GDMA_CONFIG
                /**
                 * Print GDMA Settings
                 */
                LOGD("------------------------\n");
                LOGD(ANSI_COLOR_CYAN"GDMA Configurations:\n"ANSI_COLOR_RESET);
                LOGD("\tGdmaCtl.BlockSize : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.BlockSize);
                LOGD("\tGdmaCtl.TtFc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.TtFc);
                LOGD("\tGdmaCtl.Done      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Done);
                LOGD("\tGdmaCfg.ReloadDst : %d\n", pHalGdmaAdapter_Rx->GdmaCfg.ReloadDst);
                LOGD("\tGdmaCfg.DestPer   : %d\n", pHalGdmaAdapter_Rx->GdmaCfg.DestPer);
                LOGD("\tMuliBlockCunt     : %d\n", pHalGdmaAdapter_Rx->MuliBlockCunt);
                LOGD("\tMaxMuliBlock      : %d\n", pHalGdmaAdapter_Rx->MaxMuliBlock);
                LOGD("\tChSar             : %X\n", pHalGdmaAdapter_Rx->ChSar);
                LOGD("\tChDar             : %X\n", pHalGdmaAdapter_Rx->ChDar);
                LOGD("\tGdmaIndex         : %d\n", pHalGdmaAdapter_Rx->GdmaIndex);
                LOGD("\tChNum             : %d\n", pHalGdmaAdapter_Rx->ChNum);
                LOGD("\tChEn              : %X\n", pHalGdmaAdapter_Rx->ChEn);
                LOGD("\tGdmaIsrType       : %d\n", pHalGdmaAdapter_Rx->GdmaIsrType);
                LOGD("\tIsrCtrl           : %d\n", pHalGdmaAdapter_Rx->IsrCtrl);
                LOGD("\tGdmaOnOff         : %d\n", pHalGdmaAdapter_Rx->GdmaOnOff);
                LOGD("\tTestItem          : %d\n", pHalGdmaAdapter_Rx->TestItem);
                LOGD("\tGdmaCtl.IntEn     : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.IntEn);
                LOGD("\tGdmaCtl.SrcMsize  : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.SrcMsize);
                LOGD("\tGdmaCtl.DestMsize : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.DestMsize);
                LOGD("\tGdmaCtl.SrcTrWidth: %d\n", pHalGdmaAdapter_Rx->GdmaCtl.SrcTrWidth);
                LOGD("\tGdmaCtl.DstTrWidth: %d\n", pHalGdmaAdapter_Rx->GdmaCtl.DstTrWidth);
                LOGD("\tGdmaCtl.Dinc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Dinc);
                LOGD("\tGdmaCtl.Sinc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Sinc);
#endif
            
            break;
        }
        default:
        {
            LOGI(ANSI_COLOR_YELLOW"Unknown Test Type (%d)\n"ANSI_COLOR_RESET, TestType);
            break;
        }
    }
}

