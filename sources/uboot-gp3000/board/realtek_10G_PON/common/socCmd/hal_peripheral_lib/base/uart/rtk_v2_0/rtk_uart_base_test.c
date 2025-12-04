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
//#include "rtk_hal_uart.h"
#include "rand.h"
#include "section_config.h"
#include "rtk_uart_base_test.h"
//#include "dw_hal_gdma.h"
#include "common.h"
#include "peripheral.h"

//extern u32 HalDelayUs(u32 us);

/* Function Prototype */
VOID
GetUartDebugSelValue(
        IN char *Message
        );

u32
RandBaudrate(
        VOID
        );
/**
 * DEBUG Define
 */
#define DEBUG_UART        1
#define DEBUG_UART_PARA   0
#define DEBUG_UART_DBGSEL 0
#define DEBUG_GDMA            1
#define DEBUG_GDMA_INIT       1
#define DEBUG_GDMA_INTHANDLER 0
#define DEBUG_GDMA_REGDUMP    0
#define DEBUG_TEST_DUMP_WORD  1

extern void *
memset( void *s, int c, SIZE_T n );

typedef struct _RUART_ADAPTER_ {
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
}RUART_ADAPTER, *PRUART_ADAPTER;

typedef enum _UART_TEST_TYPE_ {
    PullingIo       = 0,
    InterruptHandle = 1,
    InterruptHandle2= 2,
    InterruptHandle3= 3,
    DmaToTx         = 4,
    RxToDma         = 5,
    TxRxDma			= 6
}UART_TEST_TYPE, *PUART_TEST_TYPE;

typedef enum _UART_INT_ID_ {
    ModemStatus           = 0,
    TxFifoEmpty           = 1,
    ReceiverDataAvailable = 2,
    ReceivLineStatus      = 3,
    TimeoutIndication     = 6
}UART_INT_ID, *PUART_INT_ID;

/* UART Global Variables */
HAL_RUART_OP  HalRuartOp;
HAL_RUART_ADAPTER HalRuartAdapter_A;
HAL_RUART_ADAPTER HalRuartAdapter_B;
RUART_ADAPTER RuartAdapter_A;
RUART_ADAPTER RuartAdapter_B;

RUART_DATA RuartData_A;
RUART_DATA RuartData_B;


#define UART_FIFO_SIZE  16

// BLOCK_SIZE == TOTAL_LENGTH
#define DATA_SIZE  256
#define BLOCK_SIZE DATA_SIZE

volatile u8 RsvdMemForTxTransmission[DATA_SIZE];
volatile u8 RsvdMemForRxReception[DATA_SIZE];
//u8* RsvdMemForRxReception;


volatile u32 RecvDataIndex = 0;


EXE_BUF RTKExe_Sender;
EXE_BUF RTKExe_Receiver;
PEXE_BUF pRTKExe_Sender;
PEXE_BUF pRTKExe_Receiver;


typedef struct _GDMA_ADAPTER_ {
    PHAL_GDMA_OP pHalGdmaOp;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
}GDMA_ADAPTER, *PGDMA_ADAPTER;

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

/* GDMA Global Variable  */
GDMA_ADAPTER GdmaAdapterUART;
HAL_GDMA_OP HalGdmaOpUART;
HAL_GDMA_ADAPTER HalGdmaAdapterUART;

GDMA_ADAPTER GdmaAdapterUARTTx;
HAL_GDMA_OP HalGdmaOpUARTTx;
HAL_GDMA_ADAPTER HalGdmaAdapterUARTTx;

GDMA_ADAPTER GdmaAdapterUARTRx;
HAL_GDMA_OP HalGdmaOpUARTRx;
HAL_GDMA_ADAPTER HalGdmaAdapterUARTRx;

u32 TestType;


/* GDMA IRQ Handler */
VOID
UartGdma0Ch4IrqHandle
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

	if(TestType == TxRxDma){
		rawBlk = HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_RAW_INT_BASE+8); //RawBlock
		printf("rawBlk=%d\n",rawBlk);
		
		while(1){
			if(rawBlk==1 || rawBlk==0){
				break;
			}
			rawBlk = rawBlk >> 1;
			chNum++;
		}
		if(GdmaAdapterUARTTx.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterUARTTx.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterTx, CH%d\n",chNum);
		}
		else if(GdmaAdapterUARTRx.pHalGdmaAdapter->ChNum == chNum){
			pHalGdmaAdapter = GdmaAdapterUARTRx.pHalGdmaAdapter;
			printf("GDMA INT GdmaAdapterRx, CH%d\n",chNum);
		}
		else
			printf("find NO GdmaAdapter\n");
	}
		
#if 0
    LOGD("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case DmaToTx: {
#if DEBUG_GDMA_INTHANDLER
                LOGD(ANSI_COLOR_CYAN"Tx DMA - ConSarAutoDar(2)\n"ANSI_COLOR_RESET);
#endif
                /* if (1) { */
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
#if DEBUG_GDMA_INTHANDLER
                    LOGI(ANSI_COLOR_CYAN"Clean Auto Reload Dst\n"ANSI_COLOR_RESET);
#endif
                    HalGdmaOpUART.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
                }
                pSrc = (u8*)(pHalGdmaAdapter->ChSar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalGdmaAdapter->ChDar;
#if DEBUG_GDMA_INTHANDLER
                LOGD("pSrc(%X) = %X, pDst(%X)\n", pSrc, *pSrc, pDst);
#endif
            }
            break;
        /* case AutoSarConDar: { */
        case RxToDma: {
#if DEBUG_GDMA_INTHANDLER
                LOGD(ANSI_COLOR_CYAN"Rx DMA - AutoSarConDar(3)\n"ANSI_COLOR_RESET);
#endif
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
#if DEBUG_GDMA_INTHANDLER
                    LOGI(ANSI_COLOR_CYAN"Clean Auto Reload Src\n"ANSI_COLOR_RESET);
#endif
                    HalGdmaOpUART.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
                }

                pSrc = (u8*)pHalGdmaAdapter->ChSar;
                pDst = (u8*)(pHalGdmaAdapter->ChDar +
                                pHalGdmaAdapter->BlockLen*(pHalGdmaAdapter->MuliBlockCunt-1));
#if DEBUG_GDMA_INTHANDLER
                /* LOGD("pSrc(%X), pDst(%X) = %X\n", pSrc, pDst, *pDst); */
#endif
            }
            break;
    }

    /* Clear Pending ISR */
    IsrTypeMap = HalGdmaOpUART.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
    if (IsrTypeMap & BlockType) {
        /* LOGD("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt); */
        pHalGdmaAdapter->MuliBlockCunt++;
    }
    /* LOGD("<===== Leave Gdma0 Channel 4 ISr\n"); */
}

//TODO: Use enum to avoid global variable

VOID
GetUartDebugSelValue(
        IN char *Message
        )
{
    /* Get Debug Value */
    LOGD(ANSI_COLOR_YELLOW"-- %s --\n"ANSI_COLOR_RESET, Message);
#if DEBUG_UART_DBGSEL
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_A, 2);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_A, 3);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_A, 5);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_A, 11);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_B, 2);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_B, 3);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_B, 5);
    HalRuartOp.HalRuartGetDebugValue((VOID*)&HalRuartAdapter_B, 11);
#endif
}


BOOL
CompareArrayData
(
    IN  u8  *pSrc,
    IN  u8  *pDst,
    IN  u32 ArrayLength
)
{
    u32  Index = 0;
    BOOL CompareResult = _TRUE;

    for (Index=0; Index < ArrayLength; Index++) {
        if (pSrc[Index] != pDst[Index]) {
            LOGI(ANSI_COLOR_MAGENTA"DATA ERROR: %X, %X\n"ANSI_COLOR_RESET, pSrc[Index], pDst[Index]);
            CompareResult = _FALSE;
            break;
        }
    }
    return CompareResult;
}


u32
RandBaudrate(VOID)
{
    /* u32 BaudrateNumber = 29; */
    u32 BaudrateNumber = 18;

    u32 Index = Rand() % BaudrateNumber;

    /*return BR_TABLE[Index];*/
   	return BR_TABLE[17];
}

VOID
UartAIrqHandle(
        IN VOID *Data
)
{
    /*DBG_ENTRANCE;*/
    BOOL DebugIntHndlr = _TRUE;
    u32 RegValue;
    u32 UartIndex;
	u32 idx;
	u32 tmpvalue;
    u32 InterruptIdentification;
	u32	SendCounter = 0;
	u8  RxTriggerLevel;
	u8  RecvCounter;
	u8 TestBinaryData = 0;
    PRUART_ADAPTER pRuartAdapter = (PRUART_ADAPTER) Data;
    PHAL_RUART_OP pHalRuartOp = pRuartAdapter->pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
	PRUART_DATA pRuartData_T;
    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RUART_READ32(UartIndex, RUART_INT_ID_REG_OFF);
    InterruptIdentification = (RegValue & RUART_IIR_INT_ID) >> 1;

    switch (InterruptIdentification) {
        case ModemStatus:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ModemStatus(0)\n"ANSI_COLOR_RESET, UartIndex);
			RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_STATUS_REG_OFF);
			if (DebugIntHndlr)
                printf("UART%d: [INT]ModemStatus(%d), [R]MSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_MODEM_STATUS_REG_OFF, RegValue);
			
			if(RegValue & RUART_MODEM_STATUS_REG_RCTS){ //loopback test mode
				if(RuartData_A.pHalRuartAdapter->UartIndex!=UartIndex)
					idx = RuartData_A.pHalRuartAdapter->UartIndex;
				else
					idx = RuartData_B.pHalRuartAdapter->UartIndex;
	            //reset RTS to avoid endless CTS interrupt 
				tmpvalue = HAL_RUART_READ32(idx, RUART_MODEM_CTL_REG_OFF);
	    		tmpvalue &= ~RUART_MCR_RTS;
	    		HAL_RUART_WRITE32(idx, RUART_MODEM_CTL_REG_OFF, tmpvalue);
				
				if(RuartData_A.pHalRuartAdapter->UartIndex == UartIndex)
					pRuartData_T = &RuartData_A;
				else
					pRuartData_T = &RuartData_B;
				
				/* for (SendCounter = 0; SendCounter < 16; ++SendCounter) { */
				for (SendCounter = 0; SendCounter < 16; ++SendCounter, ++TestBinaryData) {
					pRuartData_T->BinaryData = TestBinaryData;
					pHalRuartOp->HalRuartSend((VOID*)pRuartData_T);
					/* LOGD("%02X\n", TestBinaryData); */
				}
			}
            break;
        case TxFifoEmpty:
            RegValue = HAL_RUART_READ32(UartIndex, RUART_INT_ID_REG_OFF);
            if (DebugIntHndlr)
                printf("UART%d: [INT]TxFifoEmpty(%d), [R]IIR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_INT_ID_REG_OFF, RegValue);
			//to disable tx fifo empty INT, or sys will hang due to handle it.
			HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF) & ~RUART_IER_ETBEI);
            break;
        case ReceiverDataAvailable:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ReceiverDataAvailable(2)\n"ANSI_COLOR_RESET, UartIndex);
			if (DebugIntHndlr)
                printf("UART%d: [INT]ReceiverDataAvailable(%d)\n",
                        UartIndex, InterruptIdentification);
			
            /* RxTriggerLevel = UART_FIFO_SIZE; */
            RxTriggerLevel = 14;  //Default Rx FIFO Threshold Level
            for (RecvCounter = 0; RecvCounter < RxTriggerLevel; ++RecvCounter) {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
				 RsvdMemForRxReception[RecvDataIndex++]= pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                //if (DebugIntHndlr)
                //    LOGD("RecvData = %02X\n", RecvBinaryData);
            }
            break;
        case ReceivLineStatus:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ReceivLineStatus(3)\n"ANSI_COLOR_RESET, UartIndex);
			if (DebugIntHndlr)
                printf("UART%d: [INT]ReceivLineStatus(%d), [R]LSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_LINE_STATUS_REG_OFF, RegValue);
			
			RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);

            break;
        case TimeoutIndication:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: TimeoutIndication(6)\n"ANSI_COLOR_RESET, UartIndex);
			if (DebugIntHndlr)
                printf("UART%d: [INT]TimeoutIndication(%d)\n",
                        UartIndex, InterruptIdentification);
            do {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
                RsvdMemForRxReception[RecvDataIndex++] = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                //if (DebugIntHndlr)
                //    LOGD("RecvData = %02X\n", RecvBinaryData);
                RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
            }
            while (RegValue & RUART_LINE_STATUS_REG_DR);
            break;
        default:
            LOGE(ANSI_COLOR_RED"Unknown Interrupt Type\n"ANSI_COLOR_RESET);
            break;
    }
}

VOID
UartBIrqHandle(
        IN VOID *Data
)
{
    /* DBG_ENTRANCE; */
    BOOL DebugIntHndlr = _TRUE;
    u32 RegValue;
    u32 UartIndex;
	u32 idx;
	u32 tmpvalue;
    u32 InterruptIdentification;
    u32 RecvBinaryData;
    u8  RxTriggerLevel;
    u8  RecvCounter;
	u32	SendCounter = 0;
	u8 TestBinaryData = 16;
	PRUART_DATA pRuartData_T;
    PRUART_ADAPTER pRuartAdapter = (PRUART_ADAPTER) Data;
    PHAL_RUART_OP pHalRuartOp = pRuartAdapter->pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RUART_READ32(UartIndex, RUART_INT_ID_REG_OFF);
    if (DebugIntHndlr)
        LOGD("UART%d INT_ID(0x08) = %X\n", UartIndex, RegValue);

    InterruptIdentification = (RegValue & RUART_IIR_INT_ID) >> 1;
    if (DebugIntHndlr)
        LOGD("InterruptIdentification = %X\n", InterruptIdentification);

    switch (InterruptIdentification) {
        case ModemStatus:
            RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_STATUS_REG_OFF);
			//if(RegValue & RUART_MODEM_STATUS_REG_CTS)pRTKExe_Sender->ExecuteInfo[CTS]++;
			//if(RegValue & RUART_MODEM_STATUS_REG_DSR)pRTKExe_Sender->ExecuteInfo[DSR]++;
			//if(RegValue & RUART_MODEM_STATUS_REG_DCD)pRTKExe_Sender->ExecuteInfo[DCD]++;
            if (DebugIntHndlr)
                printf("UART%d: [INT]ModemStatus(%d), [R]MSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_MODEM_STATUS_REG_OFF, RegValue);
			if(RegValue & RUART_MODEM_STATUS_REG_RCTS){ //loopback test mode
				if(RuartData_A.pHalRuartAdapter->UartIndex!=UartIndex)
					idx = RuartData_A.pHalRuartAdapter->UartIndex;
				else
					idx = RuartData_B.pHalRuartAdapter->UartIndex;
	            //reset RTS to avoid endless CTS interrupt 
				tmpvalue = HAL_RUART_READ32(idx, RUART_MODEM_CTL_REG_OFF);
	    		tmpvalue &= ~RUART_MCR_RTS;
	    		HAL_RUART_WRITE32(idx, RUART_MODEM_CTL_REG_OFF, tmpvalue);
				
				if(RuartData_A.pHalRuartAdapter->UartIndex == UartIndex)
					pRuartData_T = &RuartData_A;
				else
					pRuartData_T = &RuartData_B;
				
				/* for (SendCounter = 0; SendCounter < 16; ++SendCounter) { */
				for (SendCounter = 0; SendCounter < 16; ++SendCounter, --TestBinaryData) {
					pRuartData_T->BinaryData = TestBinaryData;
					pHalRuartOp->HalRuartSend((VOID*)pRuartData_T);
					/* LOGD("%02X\n", TestBinaryData); */
				}
			}
            break;
        case TxFifoEmpty:
            RegValue = HAL_RUART_READ32(UartIndex, RUART_INT_ID_REG_OFF);
            if (DebugIntHndlr)
                printf("UART%d: [INT]TxFifoEmpty(%d), [R]IIR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_INT_ID_REG_OFF, RegValue);
			//to disable tx fifo empty INT, or sys will hang due to handle it.
			HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_INTERRUPT_EN_REG_OFF) & ~RUART_IER_ETBEI);
            break;
        case ReceiverDataAvailable:
            if (DebugIntHndlr)
                printf("UART%d: [INT]ReceiverDataAvailable(%d)\n",
                        UartIndex, InterruptIdentification);
			
            /* RxTriggerLevel = UART_FIFO_SIZE; */
            RxTriggerLevel = 14;  //Default Rx FIFO Threshold Level
            for (RecvCounter = 0; RecvCounter < RxTriggerLevel; ++RecvCounter) {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
				 RsvdMemForRxReception[RecvDataIndex++]= pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                //if (DebugIntHndlr)
                //    LOGD("RecvData = %02X\n", RecvBinaryData);
            }
			
            break;
        case ReceivLineStatus:
            RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
			if(RegValue & RUART_LINE_STATUS_REG_OE) pRTKExe_Receiver->ExecuteInfo[OV_ERR]++;
			pRTKExe_Receiver->ExecuteInfo[RLS_INT]++;

            if (DebugIntHndlr)
                printf("UART%d: [INT]ReceivLineStatus(%d), [R]LSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RUART_LINE_STATUS_REG_OFF, RegValue);
            break;
        case TimeoutIndication:
            if (DebugIntHndlr)
                printf("UART%d: [INT]TimeoutIndication(%d)\n",
                        UartIndex, InterruptIdentification);
            do {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
                RsvdMemForRxReception[RecvDataIndex++] = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                //if (DebugIntHndlr)
                //    LOGD("RecvData = %02X\n", RecvBinaryData);
                RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
            }
            while (RegValue & RUART_LINE_STATUS_REG_DR);
            /* while (0); */
            break;
        default:
            if (DebugIntHndlr)
                LOGE(ANSI_COLOR_RED"UART%d: [INT]Unknown type\n"ANSI_COLOR_RESET, UartIndex);
            break;
    }
}

VOID
RuartGdmaIpEnable()
{
    u32 RegValue;

    /* Clock */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_CTRL);
#if DEBUG_GDMA_INIT
    LOGD("[1][R] REG_PESOC_CLK_CTRL(%X) = %X\n", PERI_ON_BASE + REG_PESOC_CLK_CTRL, RegValue);
#endif
    RegValue = RegValue | BIT16 | BIT18;
#if DEBUG_GDMA_INIT
    LOGD("[2][R] REG_PESOC_CLK_CTRL(%X) = %X\n", PERI_ON_BASE + REG_PESOC_CLK_CTRL, RegValue);
#endif
    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_CLK_CTRL, RegValue);

    /* Function Enable */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
#if DEBUG_GDMA_INIT
    LOGD("[1][R] REG_SOC_FUNC_EN(%X) = %X\n", PERI_ON_BASE + REG_SOC_FUNC_EN, RegValue);
#endif
    RegValue = RegValue | BIT13 | BIT14;
#if DEBUG_GDMA_INIT
    LOGD("[2][R] REG_SOC_FUNC_EN(%X) = %X\n", PERI_ON_BASE + REG_SOC_FUNC_EN, RegValue);
#endif
    HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, RegValue);

    /* Handshake Interface Configuration */
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
#if DEBUG_GDMA_INIT
    LOGD("[1][R] REG_PESOC_SOC_CTRL(%X) = %X\n", PERI_ON_BASE + REG_PESOC_SOC_CTRL, RegValue);
#endif
    RegValue &= ~(1<<16);  //0
    RegValue &= ~(1<<17);  //1
    RegValue &= ~(1<<18);  //2
    RegValue &= ~(1<<19);  //3
#if DEBUG_GDMA_INIT
    LOGD("[2][R] REG_PESOC_SOC_CTRL(%X) = %X\n", PERI_ON_BASE + REG_PESOC_SOC_CTRL, RegValue);
#endif
    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, RegValue);
}

u32 GetRandomTestDataLength(IN u32 TestDataSize) {
    u32 RandomDataLength = 0;

    do {
        RandomDataLength = Rand() % (TestDataSize + 1);
    }
    while (RandomDataLength == 0);

    return RandomDataLength;
}
/*
 * main
 * uart 0 1 0(type) 1(time)
 */
VOID
RuartTestApp(
    IN VOID *Data
)
{
    //DBG_ENTRANCE;
    u32 RandBaudrateValue;
    u32 RegValue;
    u32 TestTimes;
    u32 SendCounter;
    u32 RecvCounter;
    PRUART_ADAPTER pRuartAdapter_A = &RuartAdapter_A;
    PRUART_ADAPTER pRuartAdapter_B = &RuartAdapter_B;
    IRQ_HANDLE  UartIrqHandle_A;
    IRQ_HANDLE  UartIrqHandle_B;
    u32 MemOffset = 0;
    u32 TestDataLength = 0;
    u8  DummyData = 0;
    u8  RecvBinaryData;
    u8 TestBinaryData = 0x00;

	pRTKExe_Sender = &RTKExe_Sender;
	pRTKExe_Receiver= &RTKExe_Receiver;
	/* //ARM use	
	ALLOC_CACHE_ALIGN_BUFFER(u8, RsvdMemForRxReception2, DATA_SIZE);
	RsvdMemForRxReception = RsvdMemForRxReception2;
	*/

    /* Sequential Test Data */
    /* for( RegValue = 0; RegValue < DATA_SIZE; ++RegValue, ++DummyData) { */
    /*     HAL_WRITE8(RsvdMemForTxTransmission, RegValue, DummyData); */
    /* } */

    HAL_WRITE32(0xb8000000,0x14,0x1d0);//enable UART

    /* Random Test Data */
    for( RegValue = 0; RegValue < DATA_SIZE; ++RegValue) {
        DummyData = Rand() % 256;
        HAL_WRITE8(RsvdMemForTxTransmission, RegValue, DummyData);
    }
    /* Clean Reserved Memory for Rx */
    memset(RsvdMemForRxReception, 0x00, DATA_SIZE);

    LOGI("RsvdMemForTxTransmission[%d] Address: %X\n", DATA_SIZE, RsvdMemForTxTransmission);
    LOGI("RsvdMemForRxReception[%d]    Address: %X\n", DATA_SIZE, RsvdMemForRxReception);

    RandBaudrateValue = RandBaudrate();
    LOGI("RandBaudrateValue = %d\n", RandBaudrateValue);

    PRUART_VERIFY_PARAMETER pTempData = (PRUART_VERIFY_PARAMETER) Data;
#if DEBUG_UART_PARA
    LOGD("TestPara0 = %d\n", pTempData->TestPara0);
    LOGD("TestPara1 = %d\n", pTempData->TestPara1);
    LOGD("TestPara2 = %d\n", pTempData->TestPara2);
    LOGD("TestPara3 = %d\n", pTempData->TestPara3);
#endif
    /**
      * Assign parameter values to RUART Adapter.
      */
    TestType  = pTempData->TestPara0; //test item
    TestTimes = pTempData->TestPara1; //loop count

    /* UART 1 */
    HalRuartAdapter_A.UartIndex    = 1;  // pTempData->TestPara0
#if 0
    HalRuartAdapter_A.PinmuxSelect = RUART0_MUX_TO_GPIOC;
#endif
    HalRuartAdapter_A.Test_Times   = TestTimes;
	if(TestType == InterruptHandle3)
		HalRuartAdapter_A.FlowControl  = AUTOFLOW_DISABLE;
	else
    	HalRuartAdapter_A.FlowControl  = AUTOFLOW_ENABLE;

    /**
     * RUART_IER_ERBI : Received Data Available:
     * RUART_IER_ETBEI: Transmitter FIFO Empty :
     * RUART_IER_ELSI : Receiver Line Status   :
     * RUART_IER_EDSSI: Modem Status           :
     */
    /* UART0 Interrupt Settings */
    switch(TestType) {
        case PullingIo:
            {
                RegValue = 0x0;
                /* RegValue = RUART_IER_EDSSI;  // autoflow */
                break;
            }
        case InterruptHandle:
            {
                RegValue = RUART_IER_EDSSI;
                /* RegValue = RUART_IER_ELSI | RUART_IER_EDSSI; */
                break;
            }
		case InterruptHandle2:
			{
				RegValue = RUART_IER_ELSI;
				break;
			}
		case InterruptHandle3:
			{
				RegValue = RUART_IER_EDSSI;
				break;
			}
        case DmaToTx:  //Tx: ConSarAutoDar
            {
                RegValue = RUART_IER_ELSI | RUART_IER_EDSSI;
                break;
            }
        case RxToDma:  //Rx: ConSarAutoDar
            {
                RegValue = RUART_IER_ELSI | RUART_IER_EDSSI;
                break;
            }
		case TxRxDma:
			{
				RegValue = RUART_IER_ELSI | RUART_IER_EDSSI;
				break;
			}
        default:
            break;
    }

    PRUART_DATA pRuartData_A = &RuartData_A;
    HalRuartAdapter_A.PullMode            = _FALSE;
    HalRuartAdapter_A.Interrupts          = RegValue;
    HalRuartAdapter_A.BaudRate            = RandBaudrateValue;
    HalRuartAdapter_A.DmaEnable           = RUART_FIFO_CTL_REG_DMA_ENABLE;

    PHAL_RUART_ADAPTER pHalRuartAdapter_A = &HalRuartAdapter_A;
    pRuartData_A->pHalRuartAdapter        = pHalRuartAdapter_A;

    /* UART 2 */
    HalRuartAdapter_B.UartIndex    = 2;  // pTempData->TestPara1
#if 0
    HalRuartAdapter_B.PinmuxSelect = RUART1_MUX_TO_GPIOD;
#endif
    HalRuartAdapter_B.Test_Times   = TestTimes;
	if(TestType == InterruptHandle3)
		HalRuartAdapter_B.FlowControl  = AUTOFLOW_DISABLE;
	else
    	HalRuartAdapter_B.FlowControl  = AUTOFLOW_ENABLE;

    /* UART1 Interrupt Settings */
    switch(TestType) {
        case PullingIo:
            {
                RegValue = 0x0;
                /* RegValue = RUART_IER_EDSSI;  // autoflow */
                break;
            }
        case InterruptHandle:
            {
                RegValue = RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_EDSSI;
                break;
            }
		case InterruptHandle2:
			{
				RegValue = RUART_IER_ELSI;
                break;
			}
		case InterruptHandle3:
			{
				RegValue = RUART_IER_EDSSI;
                break;
			}
        case DmaToTx:  // Tx: ConSarAutoDar
            {
                RegValue = RUART_IER_ERBI | RUART_IER_ELSI | RUART_IER_EDSSI;
                break;
            }
        case RxToDma:  // Rx: ConSarAutoDar
            {
                RegValue = 0x0;
                break;
            }
		case TxRxDma:
			{
				RegValue = RUART_IER_ELSI | RUART_IER_EDSSI;
				break;
			}
        default:
            break;
    }

    PRUART_DATA pRuartData_B = &RuartData_B;
    HalRuartAdapter_B.PullMode            = _FALSE;
    HalRuartAdapter_B.Interrupts          = RegValue;
    HalRuartAdapter_B.BaudRate            = RandBaudrateValue;
    HalRuartAdapter_B.DmaEnable           = RUART_FIFO_CTL_REG_DMA_ENABLE;
    PHAL_RUART_ADAPTER pHalRuartAdapter_B = &HalRuartAdapter_B;
    pRuartData_B->pHalRuartAdapter        = pHalRuartAdapter_B;

    /* Init RUART HAL Operation */
    PHAL_RUART_OP pHalRuartOp = (PHAL_RUART_OP) &HalRuartOp;
    HalRuartOpInit((VOID*)(pHalRuartOp));

    /**
     * ======== READ DEBUG VALUE BELOW THIS LINE ========
     */
    GetUartDebugSelValue("After InitUartOp");
	
    /**
     * New power control API.
     */
#if 0
    /* UART 0 */
    ACTCK_UART0_CCTRL(ON);
    PinCtrl(UART0,pHalRuartAdapter_A->PinmuxSelect,ON);
    UART0_FCTRL(ON);

    /* UART 1 */
    ACTCK_UART1_CCTRL(ON);
    PinCtrl(UART1,pHalRuartAdapter_B->PinmuxSelect,ON);
    UART1_FCTRL(ON);
#endif
    /**
     * UART Initialization
     */

    pHalRuartOp->HalRuartInit((VOID*)pHalRuartAdapter_A);
    pHalRuartOp->HalRuartInit((VOID*)pHalRuartAdapter_B);

    /* Init UART0 again! */
    /* pHalRuartOp->HalRuartInit((VOID*)pHalRuartAdapter_A); */

    GetUartDebugSelValue("After Init UART & Before UART INT Enable");

    /**
     * Init UART Interrupt Configuration
     */
    pRuartAdapter_A->pHalRuartOp      = pHalRuartOp;
    pRuartAdapter_A->pHalRuartAdapter = pHalRuartAdapter_A;
    pRuartAdapter_B->pHalRuartOp      = pHalRuartOp;
    pRuartAdapter_B->pHalRuartAdapter = pHalRuartAdapter_B;
    //
#if 1   //97F    
    UartIrqHandle_A.Data = (u32) (pRuartAdapter_A);

    if(HalRuartAdapter_A.UartIndex == 1)
        UartIrqHandle_A.IrqNum = DW_UART_1_IE_IRQ;
    else if(HalRuartAdapter_A.UartIndex == 2)
        UartIrqHandle_A.IrqNum = DW_UART_2_IE_IRQ;
    else
    {
        printf("Wrong IRQ assignment.\n");
        return;
    }
    UartIrqHandle_A.IrqFun = (IRQ_FUN) UartAIrqHandle;
    UartIrqHandle_A.Priority = 0;
    InterruptRegister(&UartIrqHandle_A);
    InterruptEn(&UartIrqHandle_A);
#else
    UartIrqHandle_A.IrqNum = uartirda_top;
    UartIrqHandle_A.IrqFun = (IRQ_FUN) UartAIrqHandle;
    UartIrqHandle_A.Data = (u32) (pRuartAdapter_A);  //add test
    irq_install_handler(UartIrqHandle_A.IrqNum, UartIrqHandle_A.IrqFun, UartIrqHandle_A.Data); //98F arm
    UartIrqHandle_A.Priority = 0;
#endif
    //
#if 1   //97F

    UartIrqHandle_B.Data = (u32) (pRuartAdapter_B);
    if(HalRuartAdapter_B.UartIndex == 1)
        UartIrqHandle_B.IrqNum = DW_UART_1_IE_IRQ;
    else if(HalRuartAdapter_B.UartIndex == 2)
        UartIrqHandle_B.IrqNum = DW_UART_2_IE_IRQ;
    else
    {
        printf("Wrong IRQ assignment.\n");
        return;
    }

    UartIrqHandle_B.IrqFun = (IRQ_FUN) UartBIrqHandle;
    UartIrqHandle_B.Priority = 0;
    InterruptRegister(&UartIrqHandle_B);
    InterruptEn(&UartIrqHandle_B);
#else
    UartIrqHandle_B.IrqNum = uartirda_top_1;
    UartIrqHandle_B.IrqFun = (IRQ_FUN) UartBIrqHandle;
    UartIrqHandle_B.Data = (u32) (pRuartAdapter_B);  //add test
    irq_install_handler(UartIrqHandle_B.IrqNum, UartIrqHandle_B.IrqFun, UartIrqHandle_B.Data); //98F arm
    UartIrqHandle_B.Priority = 0;
#endif
    /**
     * UART Interrupts Register Enable
     */
    pHalRuartOp->HalRuartSetInterruptEnReg((VOID*)pHalRuartAdapter_A);
    pHalRuartOp->HalRuartSetInterruptEnReg((VOID*)pHalRuartAdapter_B);

	pHalRuartOp->HalRuartGetInterruptEnReg((VOID*)pHalRuartAdapter_A); //print INT reg
	pHalRuartOp->HalRuartGetInterruptEnReg((VOID*)pHalRuartAdapter_B); //print INT reg
	   

    GetUartDebugSelValue("After UART INT Enable");
#if 0
    /* Enable GDMA IP */
   // RuartGdmaIpEnable();

    /**
     * Init GDMA HAL Operation
     */
    PGDMA_ADAPTER     pGdmaAdapter;
	PGDMA_ADAPTER	  pGdmaAdapterTx;
	PGDMA_ADAPTER	  pGdmaAdapterRx;

    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
	PHAL_GDMA_ADAPTER pHalGdmaAdapterTx;
	PHAL_GDMA_ADAPTER pHalGdmaAdapterRx;

    PHAL_GDMA_OP      pHalGdmaOp;
	PHAL_GDMA_OP      pHalGdmaOpTx;
	PHAL_GDMA_OP      pHalGdmaOpRx;
	
    IRQ_HANDLE        Gdma4IrqHandle;  // UART 0
    IRQ_HANDLE        Gdma5IrqHandle;  // UART 1
    IRQ_HANDLE        GdmaTxIrqHandle;  // UART 1
    IRQ_HANDLE        GdmaRxIrqHandle;  // UART 0
    
    u8  GdmaIndex, GdmaType, MaxLlp;
    u8  *pSrc=NULL, *pDst=NULL;
	u8	*pTxSrc=NULL, *pTxDst=NULL;
	u8	*pRxSrc=NULL, *pRxDst=NULL;
	
    /**
     * GDMA Common Configurations
     */
    GdmaIndex = 0;

    /* MaxLlp = DATA_SIZE / BLOCK_SIZE; */
    MaxLlp = 1;
	
    switch(TestType) {
        case PullingIo:
        case InterruptHandle:
		case InterruptHandle2:	
		case InterruptHandle3:
        case DmaToTx: {
            GdmaType = DmaToTx;  //Tx: ConSarAutoDar
            pSrc = (u8 *)((u32)RsvdMemForTxTransmission ); //RsvdMemForTxTransmission;
            pDst = (u8*) ((UART1_REG_BASE ) + RUART_TRAN_HOLD_REG_OFF);//(RT_UART1_REG_BASE + RUART_TRAN_HOLD_REG_OFF);
            break;
        }
        case RxToDma: {
            GdmaType = RxToDma;  // Rx: ConSarAutoDar
            pSrc = (u8*) ((UART2_REG_BASE ) + RUART_REV_BUF_REG_OFF);//(RT_UART2_REG_BASE + RUART_REV_BUF_REG_OFF);
            pDst = (u8 *)((u32)RsvdMemForRxReception ); //RsvdMemForRxReception;
            break;
        }
        case TxRxDma: {
            GdmaType = TxRxDma;
            pTxSrc = (u8 *)((u32)RsvdMemForTxTransmission );
            pTxDst = (u8*) ((UART1_REG_BASE ) + RUART_TRAN_HOLD_REG_OFF);
            pRxSrc = (u8*) ((UART2_REG_BASE ) + RUART_REV_BUF_REG_OFF);
            pRxDst = (u8 *)((u32)RsvdMemForRxReception );
            break;
        }
        default:
            break;
    }
#if DEBUG_GDMA
    printf("pSrc: %X, pDst: %X\n", pSrc, pDst);
#endif

#if 1
    /* Initial GDMA HAL Operation */
    pGdmaAdapter = &GdmaAdapterUART;
    pGdmaAdapter->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUART;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUART;
    pGdmaAdapter->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOpUART;
    pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOpUART;

    HalGdmaOpInit((VOID*) (pHalGdmaOp));
    memset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));
    pHalGdmaAdapter->TestItem = GdmaType;


	pGdmaAdapterTx = &GdmaAdapterUARTTx;
    pGdmaAdapterTx->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUARTTx;
    pHalGdmaAdapterTx = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUARTTx;
    pGdmaAdapterTx->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOpUARTTx;
    pHalGdmaOpTx = (PHAL_GDMA_OP) &HalGdmaOpUARTTx;

    HalGdmaOpInit((VOID*) (pHalGdmaOpTx));
    memset((void *)pHalGdmaAdapterTx, 0, sizeof(HAL_GDMA_ADAPTER));
    pHalGdmaAdapterTx->TestItem = DmaToTx;


	pGdmaAdapterRx = &GdmaAdapterUARTRx;
    pGdmaAdapterRx->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUARTRx;
    pHalGdmaAdapterRx = (PHAL_GDMA_ADAPTER) &HalGdmaAdapterUARTRx;
    pGdmaAdapterRx->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOpUARTRx;
    pHalGdmaOpRx = (PHAL_GDMA_OP) &HalGdmaOpUARTRx;

    HalGdmaOpInit((VOID*) (pHalGdmaOpRx));
    memset((void *)pHalGdmaAdapterRx, 0, sizeof(HAL_GDMA_ADAPTER));
    pHalGdmaAdapterRx->TestItem = RxToDma;


    GetUartDebugSelValue("After InitGDMAOp");

    switch(TestType) {
        case PullingIo: {
            break;
        }
        case InterruptHandle: {
            break;
        }
		case InterruptHandle2:{
            break;
        }
		case InterruptHandle3:{
            break;
        }		
        case DmaToTx:         {  // Tx: ConSarAutoDar
        /**
         * Gdma0 Channel4 Interrupt Configuration
         */
#if 1   //97F                
        Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
        Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        Gdma4IrqHandle.Priority = 0;
        InterruptRegister(&Gdma4IrqHandle);
        InterruptEn(&Gdma4IrqHandle);
#else
        Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
        Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data); //98F arm
        Gdma4IrqHandle.Priority = 0;
#endif

        /* Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.) */
        pHalGdmaAdapter->GdmaCtl.BlockSize = BLOCK_SIZE/4;
        pHalGdmaAdapter->GdmaCtl.TtFc      = TTFCMemToPeri;  // 0x01
        pHalGdmaAdapter->GdmaCtl.Done      = 1;
        /* pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;  //<---- */
        pHalGdmaAdapter->GdmaCfg.DestPer   = 0;  //<---- UART0 TX (GDMA0)
        pHalGdmaAdapter->MuliBlockCunt     = 0;
        pHalGdmaAdapter->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
        pHalGdmaAdapter->ChSar = (u32)pSrc;  //<----
        pHalGdmaAdapter->ChDar = (u32)pDst;  //<----
        pHalGdmaAdapter->GdmaIndex   = GdmaIndex;
        pHalGdmaAdapter->ChNum       = 0;
        pHalGdmaAdapter->ChEn        = GdmaCh0; //= GdmaCh3;
        pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapter->IsrCtrl     = ENABLE;
        pHalGdmaAdapter->GdmaOnOff   = ON;
        //
        pHalGdmaAdapter->GdmaCtl.IntEn      = 1;

        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;

        pHalGdmaAdapter->GdmaCtl.Dinc = NoChange;
        pHalGdmaAdapter->GdmaCtl.Sinc = IncType;
        break;
        }
        case RxToDma:            {// Rx: ConSarAutoDar
        
        /**
         * Gdma0 Channel4 Interrupt Configuration
         */
#if 1
        Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
        Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        Gdma4IrqHandle.Priority = 0;
        InterruptRegister(&Gdma4IrqHandle);
        InterruptEn(&Gdma4IrqHandle);
#else
        Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
        Gdma4IrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        irq_install_handler(Gdma4IrqHandle.IrqNum, Gdma4IrqHandle.IrqFun, Gdma4IrqHandle.Data); //98F arm
        Gdma4IrqHandle.Priority = 0;
#endif

        /* Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.) */
        pHalGdmaAdapter->GdmaCtl.BlockSize = BLOCK_SIZE/1;  //BLOCK_SIZE=1
        pHalGdmaAdapter->GdmaCtl.TtFc      = 2;  // TTFCPeriToMem
        pHalGdmaAdapter->GdmaCtl.Done      = 1;
        pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;  //<----
        pHalGdmaAdapter->GdmaCfg.SrcPer    = 3;  //<---- UART1 RX (GDMA0)
        pHalGdmaAdapter->MuliBlockCunt     = 0;
        pHalGdmaAdapter->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
        pHalGdmaAdapter->ChSar = (u32)pSrc;  //<----
        pHalGdmaAdapter->ChDar = (u32)pDst;  //<----
        pHalGdmaAdapter->GdmaIndex   = GdmaIndex;
        /* pHalGdmaAdapter->ChNum       = 4; */
        /* pHalGdmaAdapter->ChEn        = GdmaCh4; */
        pHalGdmaAdapter->ChNum       = 0;
        pHalGdmaAdapter->ChEn        = GdmaCh0;
        pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapter->IsrCtrl     = ENABLE;
        pHalGdmaAdapter->GdmaOnOff   = ON;
        //
        pHalGdmaAdapter->GdmaCtl.IntEn      = 1;

        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;

        pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        pHalGdmaAdapter->GdmaCtl.Dinc = IncType;
        pHalGdmaAdapter->GdmaCtl.Sinc = NoChange;
        break;
        }
		case TxRxDma:
			{
#if 1
        GdmaTxIrqHandle.Data = (u32) (pGdmaAdapterTx);
        GdmaTxIrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        GdmaTxIrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        GdmaTxIrqHandle.Priority = 0;
        InterruptRegister(&GdmaTxIrqHandle);
        InterruptEn(&GdmaTxIrqHandle);
#else
        GdmaTxIrqHandle.Data = (u32) (pGdmaAdapterTx);
        GdmaTxIrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        GdmaTxIrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        irq_install_handler(GdmaTxIrqHandle.IrqNum, GdmaTxIrqHandle.IrqFun, GdmaTxIrqHandle.Data); //98F arm
        GdmaTxIrqHandle.Priority = 0;
#endif

        /* Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.) */
        pHalGdmaAdapterTx->GdmaCtl.BlockSize = BLOCK_SIZE/4;
        pHalGdmaAdapterTx->GdmaCtl.TtFc      = 1;  // 0x01
        pHalGdmaAdapterTx->GdmaCtl.Done      = 1;
        /* pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;  //<---- */
        pHalGdmaAdapterTx->GdmaCfg.DestPer   = 0;  //<---- UART0 TX (GDMA0)
        pHalGdmaAdapterTx->MuliBlockCunt     = 0;
        pHalGdmaAdapterTx->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
        pHalGdmaAdapterTx->ChSar = (u32)pTxSrc;  //<----
        pHalGdmaAdapterTx->ChDar = (u32)pTxDst;  //<----
        pHalGdmaAdapterTx->GdmaIndex   = GdmaIndex;
        pHalGdmaAdapterTx->ChNum       = 0;
        pHalGdmaAdapterTx->ChEn        = GdmaCh0;
        pHalGdmaAdapterTx->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterTx->IsrCtrl     = ENABLE;
        pHalGdmaAdapterTx->GdmaOnOff   = ON;
        //
        pHalGdmaAdapterTx->GdmaCtl.IntEn      = 1;
        pHalGdmaAdapterTx->GdmaCtl.SrcMsize   = MsizeOne;
        pHalGdmaAdapterTx->GdmaCtl.DestMsize  = MsizeFour;
        pHalGdmaAdapterTx->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        pHalGdmaAdapterTx->GdmaCtl.DstTrWidth = TrWidthOneByte;
        pHalGdmaAdapterTx->GdmaCtl.Dinc = NoChange;
        pHalGdmaAdapterTx->GdmaCtl.Sinc = IncType;

        //-------
#if 0
        GdmaRxIrqHandle.Data = (u32) (pGdmaAdapterRx);
        GdmaRxIrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        GdmaRxIrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        GdmaRxIrqHandle.Priority = 0;
        InterruptRegister(&GdmaMsrRxIrqHandle);
        InterruptEn(&GdmaMsrRxIrqHandle);
#else
        GdmaRxIrqHandle.Data = (u32) (pGdmaAdapterRx);
        GdmaRxIrqHandle.IrqNum = DW_GDMA_IP_IRQ;
        GdmaRxIrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
        irq_install_handler(GdmaRxIrqHandle.IrqNum, GdmaRxIrqHandle.IrqFun, GdmaRxIrqHandle.Data); //98F arm
        GdmaRxIrqHandle.Priority = 0;
#endif

        /* Do not forget change BLOCK_SIZE & GDMA Settings (Sinc,Dinc ... etc.) */
        pHalGdmaAdapterRx->GdmaCtl.BlockSize = BLOCK_SIZE/1;  //BLOCK_SIZE=1
        pHalGdmaAdapterRx->GdmaCtl.TtFc      = 2;  // TTFCPeriToMem
        pHalGdmaAdapterRx->GdmaCtl.Done      = 1;
        pHalGdmaAdapterRx->GdmaCfg.ReloadSrc = 1;  //<----
        pHalGdmaAdapterRx->GdmaCfg.SrcPer    = 3;  //<---- UART1 RX (GDMA0)
        pHalGdmaAdapterRx->MuliBlockCunt     = 0;
        pHalGdmaAdapterRx->MaxMuliBlock      = MaxLlp;  // MaxLlp = 1
        pHalGdmaAdapterRx->ChSar = (u32)pRxSrc;  //<----
        pHalGdmaAdapterRx->ChDar = (u32)pRxDst;  //<----
        pHalGdmaAdapterRx->GdmaIndex   = GdmaIndex;
        /* pHalGdmaAdapter->ChNum       = 4; */
        /* pHalGdmaAdapter->ChEn        = GdmaCh4; */
        pHalGdmaAdapterRx->ChNum       = 1;
        pHalGdmaAdapterRx->ChEn        = GdmaCh1;
        pHalGdmaAdapterRx->GdmaIsrType = (BlockType|TransferType|ErrType);
        pHalGdmaAdapterRx->IsrCtrl     = ENABLE;
        pHalGdmaAdapterRx->GdmaOnOff   = ON;
        //
        pHalGdmaAdapterRx->GdmaCtl.IntEn      = 1;
        pHalGdmaAdapterRx->GdmaCtl.SrcMsize   = MsizeFour;
        pHalGdmaAdapterRx->GdmaCtl.DestMsize  = MsizeOne;
        pHalGdmaAdapterRx->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapterRx->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        pHalGdmaAdapterRx->GdmaCtl.Dinc = IncType;
        pHalGdmaAdapterRx->GdmaCtl.Sinc = NoChange;
        break;
    }
        default:
            break;
    }

    GetUartDebugSelValue("After GDMA Configurations and Before GDMA Channel Enable");
    #endif
#endif
    u32 round = 0;
    u32 DmaTestLoop  = 0;
    u32 CompareCount = 0;
    //
    u32 Len      = DATA_SIZE/2;
    u32 LenIndex = 0;
    u32 *pAddress = (u32*)RsvdMemForTxTransmission;

    /**
     * Fire in the Hole!
     */
    switch(TestType) {
        case PullingIo:
            {
                TestBinaryData     = 0x00;
                RecvCounter        = 0;
                u32 SuccessCounter = 0;
                u32 FailedCounter  = 0;
                #if 0
                /* clear DLAB bit */ //tingchu
                RegValue = HAL_RUART_READ32(1, RUART_LINE_CTL_REG_OFF);
                RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
                HAL_RUART_WRITE32(1, RUART_LINE_CTL_REG_OFF, RegValue);

                RegValue = HAL_RUART_READ32(2, RUART_LINE_CTL_REG_OFF);
                RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
                HAL_RUART_WRITE32(2, RUART_LINE_CTL_REG_OFF, RegValue);
                #endif
                /* UART0 -> UART1 */
                printf(">> UART0 -> UART1\n");
                /* for (SendCounter = 0; SendCounter < 16; ++SendCounter) { */
                for (SendCounter = 0; SendCounter < 16; ++SendCounter, ++TestBinaryData) {
                    pRuartData_A->BinaryData = TestBinaryData;
                    pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    LOGD("%02X\n", TestBinaryData); 
                }

                /* If you use low baud rate, you need to wait data transfer complete */
                udelay(1000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_B->pHalRuartAdapter));
                    printf("RecvData: %02X\n", RecvBinaryData);
                }

                /* UART1 -> UART0 */
                printf(">> UART1 -> UART0\n");
                TestBinaryData = 0x0F;
                for (SendCounter = 0; SendCounter < 16; ++SendCounter, --TestBinaryData) {
                    pRuartData_B->BinaryData = TestBinaryData;
                    pHalRuartOp->HalRuartSend((VOID*)pRuartData_B);
                     LOGD("%02X\n", TestBinaryData); 
                }

                /* If you use low baud rate, you need to wait data transfer complete */
                udelay(1000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_A->pHalRuartAdapter));
                    printf("RecvData: %02X\n", RecvBinaryData);
                }
                return;
                break;
            }
        case InterruptHandle:
            {
                /**
                 * Send data from UART0(Tx) to UART1(Rx)
                 */
                round = 0;
                while(round < 1) {
                /* while(1) { */
                    round++;
                    RecvDataIndex  = 0;
                    TestDataLength = GetRandomTestDataLength(DATA_SIZE);
                    printf("TestDataLength = %d\n", TestDataLength);
                    memset(RsvdMemForRxReception, 0x00, DATA_SIZE);
					//send RTS to test modem status interrupt --> no use
					//pHalRuartOp->HalRuartRequestToSend((VOID*)pRuartData_A);
					
                    /* TestBinaryData = 0x00; */
                    /* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter, TestBinaryData++) { */
                    /* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter) { */
                    for (SendCounter = 0; SendCounter < TestDataLength; ++SendCounter) {
                        pRuartData_A->BinaryData = RsvdMemForTxTransmission[SendCounter];
                        pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    }

                    /* If you use low baud rate, you need to wait data transfer complete */
                    udelay(1000);
#if DEBUG_TEST_DUMP_WORD
                    /* for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < Len ; LenIndex+=4, pAddress+=4) {    */
                    for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%X:    %X    %X    %X    %X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    printf("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%X:    %X    %X    %X    %X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    /* if (CompareArrayData(RsvdMemForTxTransmission, RsvdMemForRxReception, DATA_SIZE/4)) { */
                    if (CompareArrayData(RsvdMemForTxTransmission, RsvdMemForRxReception, TestDataLength)) {
                        printf("Round[%d]: OK\n", round);
                    }
                    else {
                        printf("Round[%d]: ERROR\n", round);
                    }
                }
                return;
                break;
            }
		case InterruptHandle2: //test ReceiverLine Interrupt, no need
			{
#if 0                
				round = 0;
				while(round < TestTimes) {
				/* while(1) { */
					round++;
					RecvDataIndex  = 0;
					TestDataLength = 30; //exceed rx fifo buf
					printf("TestDataLength = %d\n", TestDataLength);
					memset(RsvdMemForRxReception, 0x00, DATA_SIZE);

					memset(pRTKExe_Sender, 0x00, ExecuteInfo_num);
					memset(pRTKExe_Receiver, 0x00, ExecuteInfo_num);

					/* TestBinaryData = 0x00; */
					/* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter, TestBinaryData++) { */
					/* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter) { */
					for (SendCounter = 0; SendCounter < TestDataLength; ++SendCounter) {
						pRuartData_A->BinaryData = RsvdMemForTxTransmission[SendCounter];
						pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
						//printf("data: %x\n",RsvdMemForTxTransmission[SendCounter]);
					}


					/* If you use low baud rate, you need to wait data transfer complete */
					udelay(1000);

					/*
					u32 I2CTimeOutCnt = 0;
					while(1)
					{
						if(pRTKExe_Receiver->ExecuteInfo[OV_ERR]>= 1) break;
		                if (I2CTimeOutCnt++ > (10*(8*(SYSTEM_CLK/100000))))
		                {
		                    printf("     **** Timeout Error ****\n\n"); 
		                    break;
		                }
						udelay(100);
					}
					*/

					for (SendCounter = 0; SendCounter < TestDataLength; ++SendCounter) {
						RsvdMemForRxReception[RecvDataIndex++] = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_B->pHalRuartAdapter));
						//LOGD("RecvData: %02X\n", RecvBinaryData);
					}

#if DEBUG_TEST_DUMP_WORD
					/* for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < Len ; LenIndex+=4, pAddress+=4) {	*/
					for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
						printf("%08X:	%08X	%08X	%08X	%08X\n",
						pAddress, *(pAddress), *(pAddress+1),
						*(pAddress+2), *(pAddress+3));
					}
					printf("---------------------------------------------------------\n");
					for(LenIndex = 0, pAddress = (u32*)RsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
						printf("%08X:	%08X	%08X	%08X	%08X\n",
						pAddress, *(pAddress), *(pAddress+1),
						*(pAddress+2), *(pAddress+3));
					}
#endif
					/* if (CompareArrayData(RsvdMemForTxTransmission, RsvdMemForRxReception, DATA_SIZE/4)) { */
					if (CompareArrayData(RsvdMemForTxTransmission, RsvdMemForRxReception, TestDataLength))
						//pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]++;
						printf("Round[%d]: OK\n", round);
					else
						printf("Round[%d]: ERROR\n", round);
					if(pHalRuartAdapter_A->PinmuxSelect==11) 
						printf("pHalRuartAdapter_A has ReceiverLine Interrupt\n");
					else if(pHalRuartAdapter_B->PinmuxSelect==11)
						printf("pHalRuartAdapter_B has ReceiverLine Interrupt\n");
					else
						printf("no ReceiverLine Interrupt\n");

#ifdef EXE_RESULT
					u8 ExecuteInfo_i=0;
			
					for(;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
						LOGD("Sender ExecuteInfo[%d]=%d  ",pRTKExe_Sender->ExecuteInfo[ExecuteInfo_i]);
					}
					LOGD("\n");
					for(ExecuteInfo_i=0;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
						LOGD("Receiver ExecuteInfo[%d]=%d  ",pRTKExe_Receiver->ExecuteInfo[ExecuteInfo_i]);
					}
					LOGD("\n");
#endif
					//Success condition
//					if((pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]==1) && (pRTKExe_Sender->ExecuteInfo[TXFIFO_EMP]>0) && (pRTKExe_Receiver->ExecuteInfo[OV_ERR]==1))
//						LOGI("[SUCCESS]Interrupt2 round %d\n",round);
//					else
//						LOGI("[FAIL] Interrupt2 round %d\n",round);

										
				}
#endif //#if 0				
				break;
			}
		case InterruptHandle3: //modem status interruot test
			{
				TestBinaryData     = 0x00;
                RecvCounter        = 0;
                u32 SuccessCounter = 0;
                u32 FailedCounter  = 0;
			
				//printf("INT reg: %x\n",pHalRuartOp->HalRuartGetInterruptEnReg((VOID*)pRuartData_B));
#if 1
				HAL_RUART_READ32(pHalRuartAdapter_A->UartIndex, RUART_MODEM_STATUS_REG_OFF); //clear the status
				//no auto flow control
				//uart1 request to send, uart0 get a CTS interrupt
				pHalRuartOp->HalRuartRequestToSend((VOID*)pHalRuartAdapter_B);

                /* UART0 -> UART1 */
                printf(">> UART1 -> UART2\n");


                /* If you use low baud rate, you need to wait data transfer complete */
                udelay(1000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_B->pHalRuartAdapter));
                    printf("RecvData: %02X\n", RecvBinaryData);
                }
#endif				
#if 1	
				HAL_RUART_READ32(pHalRuartAdapter_B->UartIndex, RUART_MODEM_STATUS_REG_OFF); //clear the status
				//uart0 request to send, uart1 get a CTS interrupt
				//printf("INT reg: %x\n",pHalRuartOp->HalRuartGetInterruptEnReg((VOID*)pRuartData_A));
				pHalRuartOp->HalRuartRequestToSend((VOID*)pHalRuartAdapter_A);
                /* UART1 -> UART0 */
                printf(">> UART2 -> UART1\n");

                /* If you use low baud rate, you need to wait data transfer complete */
                udelay(1000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_A->pHalRuartAdapter));
                    printf("RecvData: %02X\n", RecvBinaryData);
                }
#endif
				break;
			}
#if 0
        case DmaToTx:  // Tx: ConSarAutoDar
            {
                DmaTestLoop  = 0;

                pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
	
                /* while(1) { */
                while(DmaTestLoop < 1) {
                    TestDataLength = 128; //GetRandomTestDataLength(DATA_SIZE); --> fixed to 16 to avoid "over_run INT".
                    LOGI(ANSI_COLOR_GREEN"[O] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    if ((TestDataLength % 4 != 0) || (TestDataLength < 4))
                        TestDataLength = ((TestDataLength / 4) + 1) * 4;
                    printf("TestDataLength = %d\n", TestDataLength);
                    /* Reset GDMA Settings */
                    pHalGdmaAdapter->GdmaCtl.BlockSize = TestDataLength/4;
                    pHalGdmaAdapter->MuliBlockCunt = 0;
                    pHalGdmaAdapter->ChSar = (u32)pSrc;
                    pHalGdmaAdapter->ChDar = (u32)pDst;

                    /* Reset Test Environment */
                    CompareCount  = 0;
                    RecvDataIndex = 0;
                    memset(RsvdMemForRxReception, 0x00, DATA_SIZE);

					/*memset(pSrc, 0x00, TestDataLength);
					u8 src_i = 0;
					for(src_i = 0; src_i<TestDataLength;src_i++)
						pSrc[src_i]= Rand() % 256;
					*/

					//*****flush data before DMA******
					flush_cache(&RsvdMemForTxTransmission[0], DATA_SIZE);
					
                    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
                    pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
                    printf("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh0);
                    //printf("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh4);
                    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

                    DmaTestLoop++;

                    /* You need to wait data transfer complete. Because DMA is too fast */
                    udelay(1000);

		            u32 I2CTimeOutCnt = 0;
		            while (RecvDataIndex<TestDataLength)
		            {
		                if (I2CTimeOutCnt++ > (10*(8*(SYSTEM_CLK/100000))))
		                {
		                    //printf("     **** Timeout Error ****\n\n"); 
		                    break;
		                }
		            }
			        
                    /* GDMA Channel Disable and On/Off */
                    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
#if DEBUG_TEST_DUMP_WORD
                    for(LenIndex = 0, pAddress = (u32*)/*pSrc*/RsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    printf("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    while (CompareCount < 3)
                    {
                        if (CompareArrayData(/*pSrc*/RsvdMemForTxTransmission, RsvdMemForRxReception, TestDataLength)) {
                            printf("Round[%d]: OK\n", DmaTestLoop);
                            break;
                        }
                        else {
                            printf("(%d)Round[%d]: ERROR\n", CompareCount+1, DmaTestLoop);
                            /* Wait data transfer complete to avoid compare failure. */
                            udelay(15000);
                            CompareCount++;
                        }
                    }
                } //while(1)
                return;
                break;
            }
        case RxToDma:  // Rx: ConSarAutoDar
            {
				DmaTestLoop  = 0;
                pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));

                /* while(1) { */
                while(DmaTestLoop < 1) {
                    TestDataLength = 128;//GetRandomTestDataLength(DATA_SIZE);
                    LOGI(ANSI_COLOR_GREEN"[O] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    if ((TestDataLength % 4 != 0) || (TestDataLength < 4))
                        TestDataLength = ((TestDataLength / 4) + 1) * 4;
                    printf("TestDataLength = %d\n", TestDataLength);

                    /* Reset GDMA Settings */
                    pHalGdmaAdapter->GdmaCtl.BlockSize = TestDataLength/1;

                    /* Reset Test Environment */
                    memset(RsvdMemForRxReception, 0x00, DATA_SIZE);
					//memset(myDst, 0x00, TestDataLength);
                    pHalGdmaAdapter->MuliBlockCunt = 0;

					//*****invalidate cache before DMA******
					u32 start = (u32)RsvdMemForRxReception & ~(ARCH_DMA_MINALIGN - 1);	
					u32 end = ALIGN(start + DATA_SIZE, ARCH_DMA_MINALIGN);
					invalidate_dcache_range(start,end);
					
                    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));

                    if (pHalGdmaAdapter->Llpctrl) {
                        pHalGdmaOp->HalGdmaChBlockSeting((VOID*)(pHalGdmaAdapter));
                    }
                    else {
                        pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
                    }
                    LOGI("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh0);
                    //LOGI("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh4);
                    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

                    GetUartDebugSelValue("After GDMA Channel Enable");

                    /* Tx Send: Fixed Lenghth and Sequential Data */
                    /* TestBinaryData = 0x00; */
                    /* for (round = 0; round < DATA_SIZE; ++round, ++TestBinaryData) { */
                    /*     pRuartData_A->BinaryData = TestBinaryData; */
                    /*     pHalRuartOp->HalRuartSend((VOID*)pRuartData_A); */
                    /* } */

                    /* Tx Send: Random Length and Data */
                    for (round = 0; round < TestDataLength; ++round) {
                        pRuartData_A->BinaryData = RsvdMemForTxTransmission[round];
                        pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    }

                    DmaTestLoop++;

                    udelay(1500);

                    /* GDMA Channel Disable and On/Off */
                    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
#if DEBUG_TEST_DUMP_WORD
                    for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    printf("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)/*pDst*/RsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    if (CompareArrayData(RsvdMemForTxTransmission, /*myDst*/RsvdMemForRxReception, TestDataLength)) {
                        printf("Round[%d]: OK\n", DmaTestLoop);
                    }
                    else {
                        printf("Round[%d]: ERROR\n", DmaTestLoop);
                    }
                } //while
                break;
            }
		case TxRxDma:
			{
				DmaTestLoop  = 0;


                /* while(1) { */
                while(DmaTestLoop < 1) {
                    TestDataLength = 128; //GetRandomTestDataLength(DATA_SIZE); --> fixed to 16 to avoid "over_run INT".
                    LOGI(ANSI_COLOR_GREEN"[O] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    if ((TestDataLength % 4 != 0) || (TestDataLength < 4))
                        TestDataLength = ((TestDataLength / 4) + 1) * 4;
                    printf("TestDataLength = %d\n", TestDataLength);
                    /* Reset GDMA Settings */
					
					//4 TX DMA Initialization
					pHalGdmaAdapterTx->GdmaCtl.BlockSize = TestDataLength/4;
					pHalGdmaAdapterTx->MuliBlockCunt = 0;
                    pHalGdmaAdapterTx->ChSar = (u32)pTxSrc;
                    pHalGdmaAdapterTx->ChDar = (u32)pTxDst;
					pHalGdmaOpTx->HalGdmaOnOff((VOID*)(pHalGdmaAdapterTx));
					pHalGdmaOpTx->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterTx));
					pHalGdmaOpTx->HalGdmaChSeting((VOID*)(pHalGdmaAdapterTx));		 
					
					//4 RX DMA Initialization
					pHalGdmaAdapterRx->GdmaCtl.BlockSize = TestDataLength/1;
					pHalGdmaAdapterRx->MuliBlockCunt = 0;
		
					pHalGdmaOpRx->HalGdmaOnOff((VOID*)(pHalGdmaAdapterRx));
					pHalGdmaOpRx->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapterRx));
					pHalGdmaOpRx->HalGdmaChSeting((VOID*)(pHalGdmaAdapterRx));

					
                    /* Reset Test Environment */
                    CompareCount  = 0;
                    RecvDataIndex = 0;
                    memset(RsvdMemForRxReception, 0x00, DATA_SIZE);

					/*memset(pSrc, 0x00, TestDataLength);
					u8 src_i = 0;
					for(src_i = 0; src_i<TestDataLength;src_i++)
						pSrc[src_i]= Rand() % 256;
					*/

					//*****flush data before DMA******
					flush_cache(&RsvdMemForTxTransmission[0], DATA_SIZE);
					//*****invalidate cache before DMA******
					u32 start = (u32)RsvdMemForRxReception & ~(ARCH_DMA_MINALIGN - 1);	
					u32 end = ALIGN(start + DATA_SIZE, ARCH_DMA_MINALIGN);
					invalidate_dcache_range(start,end);

					pHalGdmaOpRx->HalGdmaChEn((VOID*)(pHalGdmaAdapterRx));
					pHalGdmaOpTx->HalGdmaChEn((VOID*)(pHalGdmaAdapterTx));
                  
                    printf("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh3);
					printf("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh2);
					
                    DmaTestLoop++;

                    /* You need to wait data transfer complete. Because DMA is too fast */
                    udelay(1000);
			        
                    /* GDMA Channel Disable and On/Off */
                    pHalGdmaOpTx->HalGdmaChDis((VOID*)(pHalGdmaAdapterTx));
					pHalGdmaOpRx->HalGdmaChDis((VOID*)(pHalGdmaAdapterRx));
#if DEBUG_TEST_DUMP_WORD
                    for(LenIndex = 0, pAddress = (u32*)/*pSrc*/RsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    printf("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        printf("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    while (CompareCount < 3)
                    {
                        if (CompareArrayData(/*pSrc*/RsvdMemForTxTransmission, RsvdMemForRxReception, TestDataLength)) {
                            printf("Round[%d]: OK\n", DmaTestLoop);
                            break;
                        }
                        else {
                            printf("(%d)Round[%d]: ERROR\n", CompareCount+1, DmaTestLoop);
                            /* Wait data transfer complete to avoid compare failure. */
                            udelay(15000);
                            CompareCount++;
                        }
                    }
                } //while(1)
                return;
				break;
			}
#endif
        default:
        	LOGI(ANSI_COLOR_RED"Invalid Test Number!\n"ANSI_COLOR_RESET);
            break;
    }

    return;

}

