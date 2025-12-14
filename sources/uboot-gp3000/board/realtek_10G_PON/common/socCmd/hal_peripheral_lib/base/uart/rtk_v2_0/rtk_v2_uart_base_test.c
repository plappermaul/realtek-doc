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
#include "rtk_v2_hal_uart.h"
#include "rand.h"
#include "section_config.h"
#include "rtk_v2_uart_base_test.h"
#include "dw_hal_gdma.h"


/* Function Prototype */
VOID
GetRTKUartDebugSelValue(
        IN char *Message
        );

u32
RTKRandBaudrate(
        VOID
        );
/**
 * DEBUG Define
 */
#define DEBUG_UART        1
#define DEBUG_UART_PARA   0
#define DEBUG_UART_DBGSEL 0
#define DEBUG_GDMA            0
#define DEBUG_GDMA_INIT       0
#define DEBUG_GDMA_INTHANDLER 0
#define DEBUG_GDMA_REGDUMP    0
#define DEBUG_TEST_DUMP_WORD  0

extern void *
memset( void *s, int c, SIZE_T n );

typedef struct _RUART_ADAPTER_ {
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
}RUART_ADAPTER, *PRUART_ADAPTER;

typedef enum _UART_TEST_TYPE_ {
    PullingIo       = 0,
    InterruptHandle = 1,
    DmaToTx         = 2,
    RxToDma         = 3,
    InterruptHandle2 = 4
}UART_TEST_TYPE, *PUART_TEST_TYPE;

typedef enum _UART_INT_ID_ {
    ModemStatus           = 0,
    TxFifoEmpty           = 1,
    ReceiverDataAvailable = 2,
    ReceivLineStatus      = 3,
    TimeoutIndication     = 6
}UART_INT_ID, *PUART_INT_ID;

/* UART Global Variables */
HAL_RUART_OP  HalRTKuartOp;
HAL_RUART_ADAPTER HalRTKuartAdapter_A;
HAL_RUART_ADAPTER HalRTKuartAdapter_B;
RUART_ADAPTER RTKuartAdapter_A;
RUART_ADAPTER RTKuartAdapter_B;

RUART_DATA RTKuartData_A;
RUART_DATA RTKuartData_B;


#define UART_FIFO_SIZE  16
#define RX_TRIGGER_LVL	14

// BLOCK_SIZE == TOTAL_LENGTH
#define DATA_SIZE  256
#define BLOCK_SIZE DATA_SIZE

volatile u8 RTKRsvdMemForTxTransmission[DATA_SIZE];
volatile u8 RTKRsvdMemForRxReception[DATA_SIZE];

volatile u32 RTKRecvDataIndex = 0;


EXE_BUF RTKExe_Sender;
EXE_BUF RTKExe_Receiver;
PEXE_BUF pRTKExe_Sender;
PEXE_BUF pRTKExe_Receiver;


#if 0 //no DMA here
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
GDMA_ADAPTER GdmaAdapter;
HAL_GDMA_OP HalGdmaOp;
HAL_GDMA_ADAPTER HalGdmaAdapter;


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

#if 0
    LOGD("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case 2: {
#if DEBUG_GDMA_INTHANDLER
                LOGD(ANSI_COLOR_CYAN"Tx DMA - ConSarAutoDar(2)\n"ANSI_COLOR_RESET);
#endif
                /* if (1) { */
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
#if DEBUG_GDMA_INTHANDLER
                    LOGI(ANSI_COLOR_CYAN"Clean Auto Reload Dst\n"ANSI_COLOR_RESET);
#endif
                    HalGdmaOp.HalGdmaChCleanAutoDst(pHalGdmaAdapter);
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
        case 3: {
#if DEBUG_GDMA_INTHANDLER
                LOGD(ANSI_COLOR_CYAN"Rx DMA - AutoSarConDar(3)\n"ANSI_COLOR_RESET);
#endif
                if ((pHalGdmaAdapter->MaxMuliBlock) == pHalGdmaAdapter->MuliBlockCunt+1) {
#if DEBUG_GDMA_INTHANDLER
                    LOGI(ANSI_COLOR_CYAN"Clean Auto Reload Src\n"ANSI_COLOR_RESET);
#endif
                    HalGdmaOp.HalGdmaChCleanAutoSrc(pHalGdmaAdapter);
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
    IsrTypeMap = HalGdmaOp.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
    if (IsrTypeMap & BlockType) {
        /* LOGD("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt); */
        pHalGdmaAdapter->MuliBlockCunt++;
    }
    /* LOGD("<===== Leave Gdma0 Channel 4 ISr\n"); */
}
#endif //no DMA here
//TODO: Use enum to avoid global variable

VOID
GetRTKUartDebugSelValue(
        IN char *Message
        )
{
    /* Get Debug Value */
    LOGD(ANSI_COLOR_YELLOW"-- %s --\n"ANSI_COLOR_RESET, Message);
#if DEBUG_UART_DBGSEL
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_A, 2);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_A, 3);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_A, 5);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_A, 11);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_B, 2);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_B, 3);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_B, 5);
    HalRTKuartOp.HalRuartGetDebugValue((VOID*)&HalRTKuartAdapter_B, 11);
#endif
}


BOOL
RTKUartCompareArrayData
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
RTKRandBaudrate(VOID)
{
    /* u32 BaudrateNumber = 29; */
    u32 BaudrateNumber = 28;

    u32 Index = Rand() % BaudrateNumber;

    /* return BR_TABLE[Index]; */
    return RTK_BR_TABLE[22];
}

VOID
Uart0IrqHandle(
        IN VOID *Data
)
{
    /*DBG_ENTRANCE;*/
    u32 RegValue;
    u32 UartIndex;
    u32 InterruptIdentification;
    PRUART_ADAPTER pRuartAdapter = (PRUART_ADAPTER) Data;
    PHAL_RUART_OP pHalRuartOp = pRuartAdapter->pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RUART_READ32(UartIndex, RTKUART_INT_ID_REG_OFF);
    //InterruptIdentification = (RegValue & RUART_IIR_INT_ID) >> 1;
    InterruptIdentification = (RegValue & RTKUART_IIR_INT_ID);

    switch (InterruptIdentification) {
        case ModemStatus:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ModemStatus(0)\n"ANSI_COLOR_RESET, UartIndex);
            RegValue = HAL_RUART_READ32(UartIndex, RTKUART_MODEM_STATUS_REG_OFF);
			if(RegValue & RTKUART_MSR_CTS)pRTKExe_Sender->ExecuteInfo[CTS]++;
			if(RegValue & RTKUART_MSR_DSR)pRTKExe_Sender->ExecuteInfo[DSR]++;
			if(RegValue & RTKUART_MSR_DCD)pRTKExe_Sender->ExecuteInfo[DCD]++;
            break;
            break;
        case TxFifoEmpty:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: TxFifoEmpty(1)\n"ANSI_COLOR_RESET, UartIndex);
            pRTKExe_Sender->ExecuteInfo[TXFIFO_EMP]++;
            break;
        case ReceiverDataAvailable:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ReceiverDataAvailable(2)\n"ANSI_COLOR_RESET, UartIndex);
            break;
        case ReceivLineStatus:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: ReceivLineStatus(3)\n"ANSI_COLOR_RESET, UartIndex);
            break;
        case TimeoutIndication:
            LOGI(ANSI_COLOR_YELLOW"UART%d INT_ID: TimeoutIndication(6)\n"ANSI_COLOR_RESET, UartIndex);
            break;
        default:
            LOGE(ANSI_COLOR_RED"Unknown Interrupt Type\n"ANSI_COLOR_RESET);
            break;
    }
}

VOID
Uart1rqHandle(
        IN VOID *Data
)
{
    /* DBG_ENTRANCE; */
    BOOL DebugIntHndlr = _FALSE;
    u32 RegValue;
    u32 UartIndex;
    u32 InterruptIdentification;
    u32 RecvBinaryData;
    u8  RxTriggerLevel;
    u8  RecvCounter;
    PRUART_ADAPTER pRuartAdapter = (PRUART_ADAPTER) Data;
    PHAL_RUART_OP pHalRuartOp = pRuartAdapter->pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RUART_READ32(UartIndex, RTKUART_INT_ID_REG_OFF);
    if (DebugIntHndlr)
        LOGD("UART%d INT_ID(0x08) = %X\n", UartIndex, RegValue);

    //InterruptIdentification = (RegValue & RUART_IIR_INT_ID) >> 1;
    InterruptIdentification = (RegValue & RTKUART_IIR_INT_ID);
    if (DebugIntHndlr)
        LOGD("InterruptIdentification = %X\n", InterruptIdentification);

    switch (InterruptIdentification) {
        case ModemStatus:
            RegValue = HAL_RUART_READ32(UartIndex, RTKUART_MODEM_STATUS_REG_OFF);
			if(RegValue & RTKUART_MSR_CTS)pRTKExe_Receiver->ExecuteInfo[CTS]++;
			if(RegValue & RTKUART_MSR_DSR)pRTKExe_Receiver->ExecuteInfo[DSR]++;
			if(RegValue & RTKUART_MSR_DCD)pRTKExe_Receiver->ExecuteInfo[DCD]++;
            if (DebugIntHndlr)
                LOGD("UART%d: [INT]ModemStatus(%d), [R]MSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RTKUART_MODEM_STATUS_REG_OFF, RegValue);
            break;
        case TxFifoEmpty:
            RegValue = HAL_RUART_READ32(UartIndex, RTKUART_INT_ID_REG_OFF);
            if (DebugIntHndlr)
                LOGD("UART%d: [INT]TxFifoEmpty(%d), [R]IIR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RTKUART_INT_ID_REG_OFF, RegValue);
            break;
        case ReceiverDataAvailable:
            if (DebugIntHndlr)
                LOGD("UART%d: [INT]ReceiverDataAvailable(%d)\n",
                        UartIndex, InterruptIdentification);

            /* RxTriggerLevel = UART_FIFO_SIZE; */
            RxTriggerLevel = 14;  //Default Rx FIFO Threshold Level
            for (RecvCounter = 0; RecvCounter < RxTriggerLevel; ++RecvCounter) {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
                RTKRsvdMemForRxReception[RTKRecvDataIndex++] = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                if (DebugIntHndlr)
                    LOGD("RecvData = %02X\n", RecvBinaryData);
            }
			pRTKExe_Receiver->ExecuteInfo[DATA_OK_INT]++;
            break;
        case ReceivLineStatus:
            RegValue = HAL_RUART_READ32(UartIndex, RTKUART_LINE_STATUS_REG_OFF);
			if(RegValue & RTKUART_LINE_STATUS_REG_OE) pRTKExe_Receiver->ExecuteInfo[OV_ERR]++;
			pRTKExe_Receiver->ExecuteInfo[RLS_INT]++;
			
            if (DebugIntHndlr)
                LOGD("UART%d: [INT]ReceivLineStatus(%d), [R]LSR(%X)=%X\n",
                        UartIndex, InterruptIdentification, RTKUART_LINE_STATUS_REG_OFF, RegValue);
            break;
        case TimeoutIndication:
            if (DebugIntHndlr)
                LOGD("UART%d: [INT]TimeoutIndication(%d)\n",
                        UartIndex, InterruptIdentification);
            do {
                /* RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter); */
                RTKRsvdMemForRxReception[RTKRecvDataIndex++] = pHalRuartOp->HalRuartRecv((VOID*)pHalRuartAdapter);
                if (DebugIntHndlr)
                    LOGD("RecvData = %02X\n", RecvBinaryData);
                RegValue = HAL_RUART_READ32(UartIndex, RTKUART_LINE_STATUS_REG_OFF);
            }
            while (RegValue & RTKUART_LINE_STATUS_REG_DR);
			pRTKExe_Receiver->ExecuteInfo[DATA_TIMEOUT]++;
            /* while (0); */
            break;
        default:
            if (DebugIntHndlr)
                LOGE(ANSI_COLOR_RED"UART%d: [INT]Unknown type\n"ANSI_COLOR_RESET, UartIndex);
            break;
    }
}
#if 0 //no DMA here
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
#endif // no DMA here
u32 RTKGetRandomTestDataLength(IN u32 TestDataSize) {
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
RTKuartTestApp(
    IN VOID *Data
)
{
    //DBG_ENTRANCE;
    u32 RandBaudrateValue;
    u32 RegValue;
    u32 TestType;
    u32 TestTimes;
    u32 SendCounter;
    u32 RecvCounter;
    PRUART_ADAPTER pRuartAdapter_A = &RTKuartAdapter_A;
    PRUART_ADAPTER pRuartAdapter_B = &RTKuartAdapter_B;
    IRQ_HANDLE  UartIrqHandle_A;
    IRQ_HANDLE  UartIrqHandle_B;
    u32 MemOffset = 0;
    u32 TestDataLength = 0;
    u8  DummyData = 0;
    u8  RecvBinaryData;
    u8 TestBinaryData = 0x00;
	
	pRTKExe_Sender = (PEXE_BUF)&RTKExe_Sender;
	pRTKExe_Receiver = (PEXE_BUF)&RTKExe_Receiver;

    /* Sequential Test Data */
    /* for( RegValue = 0; RegValue < DATA_SIZE; ++RegValue, ++DummyData) { */
    /*     HAL_WRITE8(RTKRsvdMemForTxTransmission, RegValue, DummyData); */
    /* } */

    /* Random Test Data */
    for( RegValue = 0; RegValue < DATA_SIZE; ++RegValue) {
        DummyData = Rand() % 256;
        HAL_WRITE8(RTKRsvdMemForTxTransmission, RegValue, DummyData);
    }
    /* Clean Reserved Memory for Rx */
    memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);

    LOGI("RTKRsvdMemForTxTransmission[%d] Address: %X\n", DATA_SIZE, RTKRsvdMemForTxTransmission);
    LOGI("RTKRsvdMemForRxReception[%d]    Address: %X\n", DATA_SIZE, RTKRsvdMemForRxReception);

    RandBaudrateValue = RTKRandBaudrate();
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
    TestType  = pTempData->TestPara2;
    TestTimes = pTempData->TestPara3;

    /* UART 0 */
    HalRTKuartAdapter_A.UartIndex    = 0;  // pTempData->TestPara0
    HalRTKuartAdapter_A.PinmuxSelect = RTKUART0_MUX_TO_GPIOC;
    HalRTKuartAdapter_A.Test_Times   = TestTimes;
    HalRTKuartAdapter_A.FlowControl  = AUTOFLOW_ENABLE;

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
                //RegValue = 0x0;
                RegValue = RTKUART_IER_EDSSI;
                /* RegValue = RUART_IER_ELSI | RUART_IER_EDSSI; */
                break;
            }
		case InterruptHandle2:
			{
				RegValue = RTKUART_IER_ETBEI;
				break;
			}
        case DmaToTx:  //Tx: ConSarAutoDar
            {
                RegValue = RTKUART_IER_ELSI | RTKUART_IER_EDSSI;
                break;
            }
        case RxToDma:  //Rx: ConSarAutoDar
            {
                RegValue = 0x0;
                break;
            }
        default:
            break;
    }

    PRUART_DATA pRuartData_A = &RTKuartData_A;
    HalRTKuartAdapter_A.PullMode            = _FALSE;
    HalRTKuartAdapter_A.Interrupts          = RegValue;
    HalRTKuartAdapter_A.BaudRate            = RandBaudrateValue;
	//----no DMA here
    //HalRTKuartAdapter_A.DmaEnable           = RUART_FIFO_CTL_REG_DMA_ENABLE;

    PHAL_RUART_ADAPTER pHalRuartAdapter_A = &HalRTKuartAdapter_A;
    pRuartData_A->pHalRuartAdapter        = pHalRuartAdapter_A;

    /* UART 1 */
    HalRTKuartAdapter_B.UartIndex    = 1;  // pTempData->TestPara1
    HalRTKuartAdapter_B.PinmuxSelect = RTKUART1_MUX_TO_GPIOD;
    HalRTKuartAdapter_B.Test_Times   = TestTimes;
    HalRTKuartAdapter_B.FlowControl  = AUTOFLOW_ENABLE;

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
                RegValue = RTKUART_IER_ERBI | RTKUART_IER_ELSI | RTKUART_IER_EDSSI;
                break;
            }
		case InterruptHandle2:
			{
				RegValue = RTKUART_IER_ELSI ;
				break;
			}
        case DmaToTx:  // Tx: ConSarAutoDar
            {
                RegValue = RTKUART_IER_ERBI | RTKUART_IER_ELSI | RTKUART_IER_EDSSI;
                break;
            }
        case RxToDma:  // Rx: ConSarAutoDar
            {
                RegValue = 0x0;
                break;
            }
        default:
            break;
    }

    PRUART_DATA pRuartData_B = &RTKuartData_B;
    HalRTKuartAdapter_B.PullMode            = _FALSE;
    HalRTKuartAdapter_B.Interrupts          = RegValue;
    HalRTKuartAdapter_B.BaudRate            = RandBaudrateValue;
	//----no DMA here
    //HalRTKuartAdapter_B.DmaEnable           = RUART_FIFO_CTL_REG_DMA_ENABLE;
    PHAL_RUART_ADAPTER pHalRuartAdapter_B = &HalRTKuartAdapter_B;
    pRuartData_B->pHalRuartAdapter        = pHalRuartAdapter_B;

    /* Init RUART HAL Operation */
    PHAL_RUART_OP pHalRuartOp = (PHAL_RUART_OP) &HalRTKuartOp;
    HalRTKuartOpInit((VOID*)(pHalRuartOp));

    /**
     * ======== READ DEBUG VALUE BELOW THIS LINE ========
     */
    GetRTKUartDebugSelValue("After InitUartOp");

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

    GetRTKUartDebugSelValue("After Init UART & Before UART INT Enable");

    /**
     * Init UART Interrupt Configuration
     */
    pRuartAdapter_A->pHalRuartOp      = pHalRuartOp;
    pRuartAdapter_A->pHalRuartAdapter = pHalRuartAdapter_A;
    pRuartAdapter_B->pHalRuartOp      = pHalRuartOp;
    pRuartAdapter_B->pHalRuartAdapter = pHalRuartAdapter_B;
    //
    UartIrqHandle_A.Data = (u32) (pRuartAdapter_A);
//    UartIrqHandle_A.IrqNum = UART0_IRQ;
    UartIrqHandle_A.IrqFun = (IRQ_FUN) Uart0IrqHandle;
    UartIrqHandle_A.Priority = 0;
    InterruptRegister(&UartIrqHandle_A); 	
    InterruptEn(&UartIrqHandle_A);		
    //
    UartIrqHandle_B.Data = (u32) (pRuartAdapter_B);
//	UartIrqHandle_B.IrqNum = UART1_IRQ;
    UartIrqHandle_B.IrqFun = (IRQ_FUN) Uart1rqHandle;
    UartIrqHandle_B.Priority = 0;
    InterruptRegister(&UartIrqHandle_B);	
    InterruptEn(&UartIrqHandle_B);		

    /**
     * UART Interrupts Register Enable
     */
    pHalRuartOp->HalRuartSetInterruptEnReg((VOID*)pHalRuartAdapter_A);
    pHalRuartOp->HalRuartSetInterruptEnReg((VOID*)pHalRuartAdapter_B);

    GetRTKUartDebugSelValue("After UART INT Enable");
#if 0 //no DMA here
    /* Enable GDMA IP */
    RuartGdmaIpEnable();

    /**
     * Init GDMA HAL Operation
     */
    PGDMA_ADAPTER     pGdmaAdapter;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP      pHalGdmaOp;
    IRQ_HANDLE        Gdma4IrqHandle;  // UART 0
    IRQ_HANDLE        Gdma5IrqHandle;  // UART 1
    u8  GdmaIndex, GdmaType, MaxLlp;
    u8  *pSrc=NULL, *pDst=NULL;

    /**
     * GDMA Common Configurations
     */
    GdmaIndex = 0;

    /* MaxLlp = DATA_SIZE / BLOCK_SIZE; */
    MaxLlp = 1;

    switch(TestType) {
        case PullingIo:
        case InterruptHandle:
        case DmaToTx:
            {
                GdmaType = DmaToTx;  //Tx: ConSarAutoDar
                pSrc = RTKRsvdMemForTxTransmission;
                pDst = (u8*) (UART0_REG_BASE + RUART_TRAN_HOLD_REG_OFF);
                break;
            }
        case RxToDma:
            {
                GdmaType = RxToDma;  // Rx: ConSarAutoDar
                pSrc = (u8*) (UART1_REG_BASE + RUART_REV_BUF_REG_OFF);
                pDst = RTKRsvdMemForRxReception;
                break;
            }
        default:
            break;
    }
#if DEBUG_GDMA
    LOGD("pSrc: %X, pDst: %X\n", pSrc, pDst);
#endif

    /* Initial GDMA HAL Operation */
    pGdmaAdapter = &GdmaAdapter;
    pGdmaAdapter->pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER) &HalGdmaAdapter;
    pGdmaAdapter->pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp;
    pHalGdmaOp = (PHAL_GDMA_OP) &HalGdmaOp;

    HalGdmaOpInit((VOID*) (pHalGdmaOp));
    memset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));
    pHalGdmaAdapter->TestItem = GdmaType;

    GetRTKUartDebugSelValue("After InitGDMAOp");

    switch(TestType) {
        case PullingIo:
            {
                break;
            }
        case InterruptHandle:
            {
                break;
            }
        case DmaToTx:  // Tx: ConSarAutoDar
            {
                /**
                 * Gdma0 Channel4 Interrupt Configuration
                 */
                Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
                Gdma4IrqHandle.IrqNum = GDMA0_CHANNEL4_IRQ;
                Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
                Gdma4IrqHandle.Priority = 0;
                InterruptRegister(&Gdma4IrqHandle);
                InterruptEn(&Gdma4IrqHandle);

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
                pHalGdmaAdapter->ChNum       = 4;
                pHalGdmaAdapter->ChEn        = GdmaCh4;
                pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl     = ENABLE;
                pHalGdmaAdapter->GdmaOnOff   = ON;
                //
                pHalGdmaAdapter->GdmaCtl.IntEn      = 1;
                pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
                pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
                pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
                pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
                pHalGdmaAdapter->GdmaCtl.Dinc = NoChange;
                pHalGdmaAdapter->GdmaCtl.Sinc = IncType;
                break;
            }
        case RxToDma:  // Rx: ConSarAutoDar
            {
                /**
                 * Gdma0 Channel4 Interrupt Configuration
                 */
                Gdma4IrqHandle.Data = (u32) (pGdmaAdapter);
                Gdma4IrqHandle.IrqNum = GDMA0_CHANNEL4_IRQ;
                Gdma4IrqHandle.IrqFun = (IRQ_FUN) UartGdma0Ch4IrqHandle;
                Gdma4IrqHandle.Priority = 0;
                InterruptRegister(&Gdma4IrqHandle);
                InterruptEn(&Gdma4IrqHandle);

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
                pHalGdmaAdapter->ChNum       = 5;
                pHalGdmaAdapter->ChEn        = GdmaCh5;
                pHalGdmaAdapter->GdmaIsrType = (BlockType|TransferType|ErrType);
                pHalGdmaAdapter->IsrCtrl     = ENABLE;
                pHalGdmaAdapter->GdmaOnOff   = ON;
                //
                pHalGdmaAdapter->GdmaCtl.IntEn      = 1;
                pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
                pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeOne;
                pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
                pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
                pHalGdmaAdapter->GdmaCtl.Dinc = IncType;
                pHalGdmaAdapter->GdmaCtl.Sinc = NoChange;
                break;
            }
        default:
            break;
    }

    GetRTKUartDebugSelValue("After GDMA Configurations and Before GDMA Channel Enable");
#endif //no DMA here

    u32 round = 0;
    u32 DmaTestLoop  = 0;
    u32 CompareCount = 0;
    //
    u32 Len      = DATA_SIZE/2;
    u32 LenIndex = 0;
    u32 *pAddress = (u32*)RTKRsvdMemForTxTransmission;

    /**
     * Fire in the Hole!
     */
    switch(TestType) {
        case PullingIo:
            {
				round = 0;
                while(round < TestTimes) {
					round++;
                TestBinaryData     = 0x00;
                RecvCounter        = 0;
                u32 SuccessCounter = 0;
                u32 FailedCounter  = 0;
				memset(pRTKExe_Sender, 0x00, ExecuteInfo_num);
				memset(pRTKExe_Receiver, 0x00, ExecuteInfo_num);
				
				memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);

                /* UART0 -> UART1 */
                LOGD(">> UART0 -> UART1\n");
                /* for (SendCounter = 0; SendCounter < 16; ++SendCounter) { */
                for (SendCounter = 0; SendCounter < 16; ++SendCounter, ++TestBinaryData) {
                    pRuartData_A->BinaryData = TestBinaryData;
                    pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    /* LOGD("%02X\n", TestBinaryData); */
                }

                /* If you use low baud rate, you need to wait data transfer complete */
                //HalDelayUs(15000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_B->pHalRuartAdapter));
					RTKRsvdMemForRxReception[SendCounter]=RecvBinaryData;
                    LOGD("RecvData: %02X\n", RecvBinaryData);
                }

				if(RTKUartCompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, 16))
										pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]++;
				
				memset(pRTKExe_Sender, 0x00, ExecuteInfo_num);
				memset(pRTKExe_Receiver, 0x00, ExecuteInfo_num);

				memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);

                /* UART1 -> UART0 */
                LOGD(">> UART1 -> UART0\n");
                TestBinaryData = 0x0F;
                for (SendCounter = 0; SendCounter < 16; ++SendCounter, --TestBinaryData) {
                    pRuartData_B->BinaryData = TestBinaryData;
                    pHalRuartOp->HalRuartSend((VOID*)pRuartData_B);
                    /* LOGD("%02X\n", TestBinaryData); */
                }

                /* If you use low baud rate, you need to wait data transfer complete */
                //HalDelayUs(15000);

                for (SendCounter = 0; SendCounter < 16; ++SendCounter) {
                    RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_A->pHalRuartAdapter));
					RTKRsvdMemForRxReception[SendCounter]=RecvBinaryData;
                    LOGD("RecvData: %02X\n", RecvBinaryData);
                }

				if(CompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, 16))
						pRTKExe_Sender->ExecuteInfo[RTKUART_COMPARE_GOOD]++;

#if EXE_RESULT
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
				if((pRTKExe_Sender->ExecuteInfo[RTKUART_COMPARE_GOOD]==1) && (pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]==1)) //UART0->UART1 & UART1->UART0
					LOGI("[SUCCESS]Pulling round %d\n",round);
				else
					LOGI("[FAIL] Pulling round %d\n",round);
					

                }
               // return;
                break;
            }
        case InterruptHandle:
            {
                /**
                 * Send data from UART0(Tx) to UART1(Rx)
                 */
                round = 0;
                while(round < TestTimes) {
                /* while(1) { */
                    round++;
                    RTKRecvDataIndex  = 0;
                    TestDataLength = RTKGetRandomTestDataLength(DATA_SIZE);
                    LOGI(ANSI_COLOR_GREEN"TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);
					memset(pRTKExe_Sender, 0x00, ExecuteInfo_num);
					memset(pRTKExe_Receiver, 0x00, ExecuteInfo_num);

                    /* TestBinaryData = 0x00; */
                    /* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter, TestBinaryData++) { */
                    /* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter) { */
                    for (SendCounter = 0; SendCounter < TestDataLength; ++SendCounter) {
                        pRuartData_A->BinaryData = RTKRsvdMemForTxTransmission[SendCounter];
                        pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    }

                    /* If you use low baud rate, you need to wait data transfer complete */
                    //HalDelayUs(15000);
#if DEBUG_TEST_DUMP_WORD
                    /* for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForTxTransmission; LenIndex < Len ; LenIndex+=4, pAddress+=4) {    */
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    DBG_8195A("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    /* if (CompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, DATA_SIZE/4)) { */
                    if (RTKUartCompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, TestDataLength)) 
                       pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]++;
#if EXE_RESULT
					u8 ExecuteInfo_i=0;
			
					for(;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
						LOGD("Sender ExecuteInfo[%d]=%d  ",pRTKExe_Sender->ExecuteInfo[ExecuteInfo_i]);
					}
					LOGD("\n");
					for(ExecuteInfo_i=0;ExecuteInfo_i<ExecuteInfo_num;ExecuteInfo_i++){
						LOGD("Receiver ExecuteInfo[%d]=%d	",pRTKExe_Receiver->ExecuteInfo[ExecuteInfo_i]);
					}
					LOGD("\n");
#endif

					//Success condition
                    if((pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]==1) && (pRTKExe_Receiver->ExecuteInfo[DATA_OK_INT]==(TestDataLength/RX_TRIGGER_LVL)) && (pRTKExe_Receiver->ExecuteInfo[DATA_TIMEOUT]==(TestDataLength%RX_TRIGGER_LVL))
						&& (pRTKExe_Sender->ExecuteInfo[CTS]>0) && (pRTKExe_Receiver->ExecuteInfo[DSR]>0) && (pRTKExe_Receiver->ExecuteInfo[DCD]>0) && (pRTKExe_Receiver->ExecuteInfo[RLS_INT]==0)) 
						LOGI("[SUCCESS]Interrupt round %d\n",round);
					else
						LOGI("[FAIL] Interrupt round %d\n",round);                
                }
                //return;
                break;
            }
#if 0 //no DMA here		
        case DmaToTx:  // Tx: ConSarAutoDar
            {
                DmaTestLoop  = 0;

                pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));

                /* while(1) { */
                while(DmaTestLoop < 1) {
                    TestDataLength = RTKGetRandomTestDataLength(DATA_SIZE);
                    LOGI(ANSI_COLOR_GREEN"[O] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    if ((TestDataLength % 4 != 0) || (TestDataLength < 4))
                        TestDataLength = ((TestDataLength / 4) + 1) * 4;
                    LOGI(ANSI_COLOR_GREEN"[N] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    /* Reset GDMA Settings */
                    pHalGdmaAdapter->GdmaCtl.BlockSize = TestDataLength/4;
                    pHalGdmaAdapter->MuliBlockCunt = 0;
                    pHalGdmaAdapter->ChSar = (u32)pSrc;
                    pHalGdmaAdapter->ChDar = (u32)pDst;

                    /* Reset Test Environment */
                    CompareCount  = 0;
                    RTKRecvDataIndex = 0;
                    memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);

                    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
                    pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));

                    LOGI("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh4);
                    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

                    DmaTestLoop++;

                    /* You need to wait data transfer complete. Because DMA is too fast */
                    //HalDelayUs(15000);

                    /* GDMA Channel Disable and On/Off */
                    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
#if DEBUG_TEST_DUMP_WORD
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    DBG_8195A("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    while (CompareCount < 3)
                    {
                        if (RTKUartCompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, TestDataLength)) {
                            LOGI(ANSI_COLOR_GREEN"Round[%d]: OK\n"ANSI_COLOR_RESET, DmaTestLoop);
                            break;
                        }
                        else {
                            LOGI(ANSI_COLOR_MAGENTA"(%d)Round[%d]: ERROR\n"ANSI_COLOR_RESET, CompareCount+1, DmaTestLoop);
                            /* Wait data transfer complete to avoid compare failure. */
                            HalDelayUs(15000);
                            CompareCount++;
                        }
                    }
                } //while(1)
                return;
                break;
            }
        case RxToDma:  // Rx: ConSarAutoDar
            {
                pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));

                /* while(1) { */
                while(DmaTestLoop < 1) {
                    TestDataLength = RTKGetRandomTestDataLength(DATA_SIZE);
                    LOGI(ANSI_COLOR_GREEN"[O] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
                    if ((TestDataLength % 4 != 0) || (TestDataLength < 4))
                        TestDataLength = ((TestDataLength / 4) + 1) * 4;
                    LOGI(ANSI_COLOR_GREEN"[N] TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);

                    /* Reset GDMA Settings */
                    pHalGdmaAdapter->GdmaCtl.BlockSize = TestDataLength/1;

                    /* Reset Test Environment */
                    memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);
                    pHalGdmaAdapter->MuliBlockCunt = 0;

                    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));

                    if (pHalGdmaAdapter->Llpctrl) {
                        pHalGdmaOp->HalGdmaChBlockSeting((VOID*)(pHalGdmaAdapter));
                    }
                    else {
                        pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
                    }

                    LOGI("-- DMA %d Channel 0x%X Enable --\n", GdmaIndex, GdmaCh4);
                    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

                    GetRTKUartDebugSelValue("After GDMA Channel Enable");

                    /* Tx Send: Fixed Lenghth and Sequential Data */
                    /* TestBinaryData = 0x00; */
                    /* for (round = 0; round < DATA_SIZE; ++round, ++TestBinaryData) { */
                    /*     pRuartData_A->BinaryData = TestBinaryData; */
                    /*     pHalRuartOp->HalRuartSend((VOID*)pRuartData_A); */
                    /* } */

                    /* Tx Send: Random Length and Data */
                    for (round = 0; round < TestDataLength; ++round) {
                        pRuartData_A->BinaryData = RTKRsvdMemForTxTransmission[round];
                        pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
                    }

                    DmaTestLoop++;

                    //HalDelayUs(15000);

                    /* GDMA Channel Disable and On/Off */
                    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
#if DEBUG_TEST_DUMP_WORD
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
                    DBG_8195A("---------------------------------------------------------\n");
                    for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
                        DBG_8195A("%08X:    %08X    %08X    %08X    %08X\n",
                        pAddress, *(pAddress), *(pAddress+1),
                        *(pAddress+2), *(pAddress+3));
                    }
#endif
                    if (RTKUartCompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, TestDataLength)) {
                        LOGI(ANSI_COLOR_GREEN"Round[%d]: OK\n"ANSI_COLOR_RESET, DmaTestLoop);
                    }
                    else {
                        LOGI(ANSI_COLOR_MAGENTA"Round[%d]: ERROR\n"ANSI_COLOR_RESET, DmaTestLoop);
                    }
                } //while
                return;
                break;
            }
#endif //no DMA here
		case InterruptHandle2:
			{
				round = 0;
				while(round < TestTimes) {
				/* while(1) { */
					round++;
					RTKRecvDataIndex  = 0;
					TestDataLength = 17;
					LOGI(ANSI_COLOR_GREEN"TestDataLength = %d\n"ANSI_COLOR_RESET, TestDataLength);
					memset(RTKRsvdMemForRxReception, 0x00, DATA_SIZE);

					memset(pRTKExe_Sender, 0x00, ExecuteInfo_num);
					memset(pRTKExe_Receiver, 0x00, ExecuteInfo_num);
					
					/* TestBinaryData = 0x00; */
					/* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter, TestBinaryData++) { */
					/* for (SendCounter = 0; SendCounter < DATA_SIZE; ++SendCounter) { */
					for (SendCounter = 0; SendCounter < TestDataLength; ++SendCounter) {
						pRuartData_A->BinaryData = RTKRsvdMemForTxTransmission[SendCounter];
						pHalRuartOp->HalRuartSend((VOID*)pRuartData_A);
					}

					/* If you use low baud rate, you need to wait data transfer complete */
					udelay(15000);
					
					while(1)
					{
						if(pRTKExe_Receiver->ExecuteInfo[OV_ERR]==1) break;
						udelay(100);
					}
					for (SendCounter = 0; SendCounter < UART_FIFO_SIZE; ++SendCounter) {
						RecvBinaryData = pHalRuartOp->HalRuartRecv((VOID*)(pRuartAdapter_B->pHalRuartAdapter));
						LOGD("RecvData: %02X\n", RecvBinaryData);
					}
					
#if DEBUG_TEST_DUMP_WORD
					/* for(LenIndex = 0, pAddress = (u32*)RsvdMemForTxTransmission; LenIndex < Len ; LenIndex+=4, pAddress+=4) {	*/
					for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForTxTransmission; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
						DBG_8195A("%08X:	%08X	%08X	%08X	%08X\n",
						pAddress, *(pAddress), *(pAddress+1),
						*(pAddress+2), *(pAddress+3));
					}
					DBG_8195A("---------------------------------------------------------\n");
					for(LenIndex = 0, pAddress = (u32*)RTKRsvdMemForRxReception; LenIndex < TestDataLength/4; LenIndex+=4, pAddress+=4) {
						DBG_8195A("%08X:	%08X	%08X	%08X	%08X\n",
						pAddress, *(pAddress), *(pAddress+1),
						*(pAddress+2), *(pAddress+3));
					}
#endif
					/* if (CompareArrayData(RsvdMemForTxTransmission, RsvdMemForRxReception, DATA_SIZE/4)) { */
					if (CompareArrayData(RTKRsvdMemForTxTransmission, RTKRsvdMemForRxReception, TestDataLength))
						pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]++;

#if EXE_RESULT
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
					if((pRTKExe_Receiver->ExecuteInfo[RTKUART_COMPARE_GOOD]==1) && (pRTKExe_Sender->ExecuteInfo[TXFIFO_EMP]>0) && (pRTKExe_Receiver->ExecuteInfo[OV_ERR]==1))
						LOGI("[SUCCESS]Interrupt2 round %d\n",round);
					else
						LOGI("[FAIL] Interrupt2 round %d\n",round);

										
				}
				return;
				break;
			}

        default:
        	LOGI(ANSI_COLOR_RED"Invalid Test Number!\n"ANSI_COLOR_RESET);
            break;
    }

    return;

}
