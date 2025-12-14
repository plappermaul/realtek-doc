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
#include "dw_spi_base_test.h"
#include "dw_spi_base.h"
#include "peripheral.h"
#include <stdarg.h>
#include "common.h"

#define YR_DBG_FLAG 1

//#define DMA_TO_TX_AND_RX_TO_DMA 1

#define SOC_HAL_W32(base, addr, value)      ((*((volatile u32*)(base + addr))) = value)
#define SOC_HAL_R32(base, addr)             (*((volatile u32*)(base + addr)))
#define SOC_HAL_W16(base, addr, value)      ((*((volatile u16*)(base + addr))) = value)
#define SOC_HAL_R16(base, addr)             (*((volatile u16*)(base + addr)))
#define SOC_HAL_W8(base, addr, value)       ((*((volatile u8*)(base + addr))) = value)
#define SOC_HAL_R8(base, addr)              (*((volatile u8*)(base + addr)))

#ifdef CONFIG_RTL8198F //8198FH
#define SPI0_BASE_REG 0xf432e100
#define SPI1_BASE_REG 0xf432e200
#define SSI_DR_OFFSET 0x60
#endif
#ifdef CONFIG_RTL8197G
#define SPI0_BASE_REG 0xb801c000
#define SPI1_BASE_REG 0xb801c100
#define SSI_DR_OFFSET 0x60
#endif
#ifdef CONFIG_CA8277B
#define SPI0_BASE_REG 0xf4329600
#define SPI1_BASE_REG 0xf4329600
#define SSI_DR_OFFSET 0x60
#endif

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

typedef struct _SSI_ADAPTER_ {
    PHAL_SSI_OP      pHalSsiOp;
    PHAL_SSI_ADAPTER pHalSsiAdapter;
}SSI_ADAPTER, *PSSI_ADAPTER;

#define DATA_SIZE 512
#define BAUDR_MULTIPLIER  100  // 1*100: 40000000 / 100  = 400000  = 400KHz
//                         10  // 1*10 : 40000000 / 10   = 4000000 = 4MHz

HAL_SSI_OP  HalSsiOp;
SSI_ADAPTER SsiAdapter_Master0;
//SSI_ADAPTER SsiAdapter_Master1;
HAL_SSI_ADAPTER HalSsiAdapter_Master0;
//HAL_SSI_ADAPTER HalSsiAdapter_Master1;

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



u16 RsvdMemForTx[DATA_SIZE] __attribute__ ((aligned (CONFIG_SYS_CACHELINE_SIZE)));
u16 RsvdMemForRx[DATA_SIZE] __attribute__ ((aligned (CONFIG_SYS_CACHELINE_SIZE)));
volatile u32 RsvdMemForRx_Index = 0;
volatile BOOL RxTransferDone = _FALSE;
BOOL MICROWIRE = _FALSE;
BOOL TI_SSP    = _FALSE;


VOID
SsiTxGdmaIrqHandle (
        IN VOID *Data
        );

VOID
SsiRxGdmaIrqHandle (
        IN VOID *Data
        );

/*
TX
1. Enable SSI writing 1 to the SSIENR (0xF432_9608)
2. Setup Control register (0xF432_9600-0xF432_9604)
3. Write data into TX FIFO Data Register (0xF432_9660)
4. Polling status Register Busy (0xF432_9628)
5. Check ISR Register TX FIFO Empty (0xF432_9630)
6. Disable SSI writing 0 to the SSIENR (0xF432_9608)
*/

/*VOID SsiTx
{
u32 SsiRegTempValue;

//1. Enable SSI writing 1 to the SSIENR
SsiRegTempValue &= 0;
SsiRegTempValue = 1;
SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_SSIENR, SsiRegTempValue);

//2. Setup Control register
SsiRegTempValue &= 0;
SsiRegTempValue = SOC_HAL_R32(SPI0_BASE_REG, REG_DW_SSI_CTRLR0);
SsiRegTempValue =  
SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_CTRLR0, SsiRegTempValue);

SsiRegTempValue &= 0;
SsiRegTempValue = SOC_HAL_R32(SPI0_BASE_REG, REG_DW_SSI_CTRLR1);
SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_CTRLR1, SsiRegTempValue);

//3. Write data into TX FIFO Data Register
SsiRegTempValue &= 0;
SsiRegTempValue = SOC_HAL_R32(SPI0_BASE_REG, );
SOC_HAL_W32(SPI0_BASE_REG, , SsiRegTempValue);

//4. Polling status Register Busy
SsiRegTempValue &= 0;
SsiRegTempValue = SOC_HAL_R32(SPI0_BASE_REG, addr);

//5. Check ISR Register TX FIFO Empty
SsiRegTempValue &= 0;
SsiRegTempValue = SOC_HAL_R32(SPI0_BASE_REG, addr);

//6. Disable SSI writing 0 to the SSIENR
SsiRegTempValue &= 0;
SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_SSIENR, SsiRegTempValue)

}

VOID SsiRx
{
//TODO: SPI RX
}

*/

/**
 * Main Entrance
 * ------------------------------------------------------------
 * Usage
 * socTest SSI
 * Para0         0   TestFrameFormat [0-2]
 *                      0   SPI        Motorola Serial Peripheral Interface
 *                      1   SSP        Texas Instruments Serial Protocol
 *                      2   Micorwire  National Semiconductor Microwire
 * Para1         0   SSI0Index
 * Para2         1   SSI1Index
 * Para3         0   TestType [0-3]
 *                      0   SSI_TT_BASIC_IO
 *                      1   SSI_TT_INTERRUPT
 *                      2   SSI_TT_DMA_TO_TX
 *                      3   SSI_TT_RX_TO_DMA
 * Para4         x   BaudRateBase (BAUDR = x * 100)
 * Para5         0   R/W DR (2 bytes) commands [1-10]
 *                      1   SPI0: SOC_HAL_R16
 *                      2   SPI1: SOC_HAL_R16
 *                      3   SPI0: SOC_HAL_W16
 *                      4   SPI1: SOC_HAL_W16
 *                      5   dump whole SPI0 DR data
 *                      6   dump whole SPI1 DR data
 *                      7
 *                      8
 *                      9
 *                     10
 * Para6         0   Switch command [1-2]
 *                      1   Disable SSI
 *                      2   Enable SSI
 * Para7         0   Write Data
 * Para8        16   TestTime
* ------------------------------------------------------------
 */

VOID
SsiTestApp(
        IN VOID *Data
        )
{
    // DBG_ENTRANCE
    PSSI_VERIFY_PARAMETER pSsiVerifyData = (PSSI_VERIFY_PARAMETER) Data;

    // DBG_SSI_PARA
#if YR_DBG_FLAG
    printf("YR %s(%d)\n", __FUNCTION__, __LINE__);
    printf("Para0 = %d\n", pSsiVerifyData->Para0);
    printf("Para1 = %d\n", pSsiVerifyData->Para1);
    printf("Para2 = %d\n", pSsiVerifyData->Para2);
    printf("Para3 = %d\n", pSsiVerifyData->Para3);
    printf("Para4 = %d\n", pSsiVerifyData->Para4);
    printf("Para5 = %d\n", pSsiVerifyData->Para5);
    printf("Para6 = %d\n", pSsiVerifyData->Para6);
    printf("Para7 = %d\n", pSsiVerifyData->Para7);
    printf("Para8 = %d\n", pSsiVerifyData->Para8);
#endif

    // Test Parameters
    u8 TestFrameFormat = pSsiVerifyData->Para0;
    u8 SSI0Index       = pSsiVerifyData->Para1;
    u8 SSI1Index       = pSsiVerifyData->Para2;
    u8 TestType        = pSsiVerifyData->Para3;
    u8 BaudRateBase    = pSsiVerifyData->Para4;
    u8 RW_2Bytes_Cmd   = pSsiVerifyData->Para5;
    u8 SWITCH_Cmd      = pSsiVerifyData->Para6;
    u16 WriteData      = (u16) pSsiVerifyData->Para7 + 0x1000;
    // u8 WriteData      = pSsiVerifyData->Para7;
    u8 TestTimes       = pSsiVerifyData->Para8;  //SSI_TX_FIFO_DEPTH;
    u32 TestLoops = 1;

    if(SSI0Index == 1)
    {
        // for regression test
        printf("Pass\n");
        return;
    }

    /**
     * Read/Write 2 bytes Test
     */
    //RW_2Bytes_Test(RW_2Bytes_Cmd, WriteData);
    switch(RW_2Bytes_Cmd)  // Para5
    {
	case 1: // Read 2 bytes from SPI0 DR
        {
		u16 tmp_ReadData;
		tmp_ReadData = SOC_HAL_R16(SPI0_BASE_REG, SSI_DR_OFFSET);
		printf("Flysky: %s(%d) Read SPI0 DR = 0x%04x\n", __FUNCTION__, __LINE__, tmp_ReadData);
		return;
	}
	break;
	case 2: // Read 2 bytes from SPI1 DR
	{
		//u2Byte tmp_ReadData = SOC_HAL_R16(SPI1_BASE_REG, SSI_DR_OFFSET);
		//printf("Flysky: %s(%d) Read SPI1 DR = 0x%04x\n", __FUNCTION__, __LINE__, tmp_ReadData);
		return;
	}
	break;
	case 3: // Write 2 bytes into SPI0 DR
	{
		SOC_HAL_W16(SPI0_BASE_REG, SSI_DR_OFFSET, WriteData&0xffff);
		printf("Flysky: %s(%d) Write SPI0 DR = 0x%04x\n", __FUNCTION__, __LINE__, WriteData&0xffff);
		return;
	}
	break;
	case 4: // Write 2 bytes into SPI1 DR
	{
		//SOC_HAL_W16(SPI1_BASE_REG, SSI_DR_OFFSET, WriteData&0xffff);
		//printf("Flysky: %s(%d) Write SPI1 DR = 0x%04x\n", __FUNCTION__, __LINE__, WriteData&0xffff);
		return;
	}
	break;
	case 5: // dump whole SPI0 DR data
	{
		u8 i = SSI_DR_OFFSET;
		u16 tmp_ReadData;
		for ( ; i <= 0xec; i = i + 0x8)
		{
			tmp_ReadData = SOC_HAL_R16(SPI0_BASE_REG, i);
			printf("Flysky: %s(%d) Read SPI0 DR 0x%04x: 0x%02x\n", __FUNCTION__, __LINE__, i, tmp_ReadData);
		}
		return;
	}
	break;
	case 6: // dump whole SPI1 DR data
	{
		//u8 i = SSI_DR_OFFSET;
		//u2Byte tmp_ReadData;
		//for ( ; i <= 0xec; i = i + 0x8)
		//{
		//	tmp_ReadData = SOC_HAL_R16(SPI1_BASE_REG, i);
		//	//printf("Flysky: %s(%d) Read SPI1 DR 0x%04x: 0x%02x\n", __FUNCTION__, __LINE__, i, tmp_ReadData);
		//}
		return;
	}
	break;
	default:
	{
		if (0 != RW_2Bytes_Cmd)
		{
			printf("Flysky: %s(%d) Wrong socTest SSI Para[5]!", __FUNCTION__, __LINE__);
			return;
		}
	}
	break;
    }
    /**
     * Disable/Enable SSI
     */
    switch(SWITCH_Cmd) // Para6
    {
    case 1: // Disable SSI
        #if 0
        u2Byte tmp_ReadData0;
        u2Byte tmp_ReadData1;
        do {
            tmp_ReadData0 = SOC_HAL_R16(SPI0_BASE_REG, SSI_DR_OFFSET);
            tmp_ReadData1 = SOC_HAL_R16(SPI1_BASE_REG, SSI_DR_OFFSET);
            printf("YR: %s(%d) SPI0 DR data = 0x%04x\n", __FUNCTION__, __LINE__, tmp_ReadData0);
            printf("YR: %s(%d) SPI1 DR data = 0x%04x\n", __FUNCTION__, __LINE__, tmp_ReadData1);
        }
        while((tmp_ReadData0 != 0x0) || (tmp_ReadData1 != 0x0));
        #endif

        printf("Flysky: %s(%d) Disable SSI!\n", __FUNCTION__, __LINE__);
        SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_SSIENR, 0x0);
        SOC_HAL_W32(SPI1_BASE_REG, REG_DW_SSI_SSIENR, 0x0);
        return;
    break;
    case 2: // Enable SSI
        printf("Flysky: %s(%d) Enable SSI!\n", __FUNCTION__, __LINE__);
        SOC_HAL_W32(SPI0_BASE_REG, REG_DW_SSI_SSIENR, 0x1);
        SOC_HAL_W32(SPI1_BASE_REG, REG_DW_SSI_SSIENR, 0x1);
        return;
    break;
    default:
        if (0 != SWITCH_Cmd)
        {
            printf("Flysky: %s(%d) Wrong socTest SSI Para[6]!\n", __FUNCTION__, __LINE__);
            return;
        }
    break;
    }
    /**
     * Initial RsvdMemForTx[] and RsvdMemForRx[]
     * ------------------------------------------------------------
     * [RsvdMemForTx, RsvdMemForTx + (2 * DATA_SIZE)]: random data
     * [RsvdMemForRx, RsvdMemForTx + (2 * DATA_SIZE)]: all 0's
     * ------------------------------------------------------------
     */
    SsiGenerateRandomTxData(RsvdMemForTx, DATA_SIZE, SSI_TEST_SRCDATA_RND, MICROWIRE);
    memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));

    /**
     * Serial Transfer STEP_00:Initialization
     * ------------------------------------------------------------
     * Initialize SSI0 to be a slave one
     * Initialize SSI1 to be a master one
     * Initialize SSI operations
     * Enable SSI
     * ------------------------------------------------------------
     */
#if YR_DBG_FLAG
    printf("Flysky: Initial DW_apb_ssi...\n");
#endif

    /* SPI0 and SPI1 are always as Master. */
    PSSI_ADAPTER pSsiAdapter_Master0        = &SsiAdapter_Master0;
    //PSSI_ADAPTER pSsiAdapter_Master1        = &SsiAdapter_Master1;
    PHAL_SSI_ADAPTER pHalSsiAdapter_Master0 = &HalSsiAdapter_Master0;
    //PHAL_SSI_ADAPTER pHalSsiAdapter_Master1 = &HalSsiAdapter_Master1;
    /* SSI OP Init */
    PHAL_SSI_OP pHalSsiOp = (PHAL_SSI_OP) &HalSsiOp;
    HalSsiOpInit((VOID*)(pHalSsiOp));
    /* SSI0 Init: to be a master one and select pinmux */
    pHalSsiAdapter_Master0->Index        = SSI0Index;
/*    pHalSsiAdapter_Master0->PinmuxSelect = SSI0_MUX_TO_GPIOE;
*/    pSsiAdapter_Master0->pHalSsiAdapter  = pHalSsiAdapter_Master0;
    pSsiAdapter_Master0->pHalSsiOp       = pHalSsiOp;

    /* SSI1 Init: to be a master one and select pinmux */
    //pHalSsiAdapter_Master1->Index        = SSI1Index;
    //pHalSsiAdapter_Master1->PinmuxSelect = SSI1_MUX_TO_GPIOB;
    //pSsiAdapter_Master1->pHalSsiAdapter  = pHalSsiAdapter_Master1;
    //pSsiAdapter_Master1->pHalSsiOp       = pHalSsiOp;

    /* 97G SPI IP active */
    printf("%s(%d) Flysky: 4-wired SPI enable\n", __FUNCTION__, __LINE__);
    //SOC_HAL_W32(0xf43203a8, 0, 0xC2430000);
    // Global Enable Ip 0 Secure Register 0xf43203a8
    // Default value      0xC2C30000 (C: 1100)
    //                                   /\
    // dw ssi0 master(1) slave(0) select   dw ssi1 master(1) slave(0) select
    //                                   \/
    // active SSI         0xC2430000 (4: 0100)

    //SOC_HAL_W32(0xb8000000, 0x0014, 0xffffffff);
    // REG_CLK_MANAGE2
    // Default value      0x00000000
    // active all         0xffffffff

    //SOC_HAL_W32(0xb8000000, 0x0050, 0xffffffff);
    // REG_ENABLE_IP
    // Default value      0x
    // active all         0xffffffff

    //SOC_HAL_W32(0xb8000000, 0x0050, 0xffffffff);
    // REG_ENABLE_IP

    /* 97G SPI pinmux */
    //SOC_HAL_W32(0xb8000000, 0x0800, 0x32220000);
    // REG_PINMUX_00
    //                                  Pinmux selection
    // Bit[31:28] BIT_REG_IOCFG_P0TXD0  0011:SPI1
    // Bit[27:24] BIT_REG_IOCFG_P0TXD1  0010:SPI0
    // Bit[23:20] BIT_REG_IOCFG_P0TXD2  0010:SPI0
    // Bit[19:16] BIT_REG_IOCFG_P0TXD3  0010:SPI0
    // Bit[15:12] --                    Reserved
    // Bit[11: 8] --                    Reserved
    // Bit[ 7: 4] --                    Reserved
    // Bit[ 3: 0] --                    Reserved
    //SOC_HAL_W32(0xb8000000, 0x0808, 0x02232000); // 4-wire
    //SOC_HAL_W32(0xb8000000, 0x0808, 0x02243000); // 3-wire
    // REG_PINMUX_02
    //
    //                                  Pinmux selection
    // Bit[31:28] --                    Reserved
    // Bit[27:24] BIT_REG_IOCFG_P0TXC   0010(0x2):SPI1
    // Bit[23:20] BIT_REG_IOCFG_P0TXCTL 0010(0x2):SPI1
    // Bit[19:16] BIT_REG_IOCFG_P0RXC   0011(0x3):SPI0
    //                                  0100(0x4):SPI0_3W
    // Bit[15:12] BIT_REG_IOCFG_P0RXCTL 0010(0x2):SPI1
    //                                  0011(0x3):SPI1_3W
    // Bit[11: 8] BIT_REG_IOCFG_P0MDC   0000(0x0): MMI
    // Bit[ 7: 4] BIT_REG_IOCFG_P0MDIO  0000(0x0): MMI
    // Bit[ 3: 0] --                    Reserved


    /**
     * Serial Transfer STEP_01: Disable DW_apb_ssi
     * ------------------------------------------------------------
     * write 0 to the SSI Enable register (SSIENR)
     * ------------------------------------------------------------
     */

#if YR_DBG_FLAG
    printf("Flysky: Disable DW_apb_ssi...\n");
#endif

    pHalSsiOp->HalSsiDisable((VOID*)pHalSsiAdapter_Master0);
    //pHalSsiOp->HalSsiDisable((VOID*)pHalSsiAdapter_Master1);

    /**
     * Serial Transfer STEP_02: SSI Configuration
     * ------------------------------------------------------------
     * SSI0 (master) configuration
     * SSI1 (master) configuration
     * Interrupt configuration
     * Initial SSI Configuration & Enable SSI
     * ------------------------------------------------------------
     */

#if YR_DBG_FLAG
    printf("Flysky: SSI Configuration...\n");
#endif

    /* SSI0 - Master */
    pHalSsiAdapter_Master0->Role              = SSI_MASTER;
    pHalSsiAdapter_Master0->DataFrameSize     = DFS_16_BITS;
    pHalSsiAdapter_Master0->DataFrameFormat   = FRF_MOTOROLA_SPI;
    pHalSsiAdapter_Master0->SclkPhase         = SCPH_TOGGLES_IN_MIDDLE;
    //pHalSsiAdapter_Master0->SclkPolarity      = SCPOL_INACTIVE_IS_HIGH;
    pHalSsiAdapter_Master0->SclkPolarity      = SCPOL_INACTIVE_IS_LOW;
    //
    pHalSsiAdapter_Master0->TransferMode      = TMOD_TR;
    //pHalSsiAdapter_Master0->TransferMode      = TMOD_TO;
    //pHalSsiAdapter_Master0->TransferMode     = TMOD_RO;
    //
    pHalSsiAdapter_Master0->ControlFrameSize  = CFS_1_BIT;
    pHalSsiAdapter_Master0->SlaveSelectEnable = 0x1;
    pHalSsiAdapter_Master0->ClockDivider      = BaudRateBase * BAUDR_MULTIPLIER;
    pHalSsiAdapter_Master0->TxThresholdLevel  = 1;
    pHalSsiAdapter_Master0->RxThresholdLevel  = 0;
    //pHalSsiAdapter_Master0->InterruptMask     = 0;
    /* pHalSsiAdapter_Master0->DataFrameNumber   = */  // <---- Master && (Receive Only || EEPROM Read)
#if 0
    /* SSI1 - Master */
    //pHalSsiAdapter_Master1->Role              = SSI_MASTER;
    //pHalSsiAdapter_Master1->DataFrameSize     = DFS_16_BITS;
    //pHalSsiAdapter_Master1->DataFrameFormat   = FRF_MOTOROLA_SPI;
    //pHalSsiAdapter_Master1->SclkPhase         = SCPH_TOGGLES_IN_MIDDLE;
    //pHalSsiAdapter_Master1->SclkPolarity      = SCPOL_INACTIVE_IS_HIGH;
    //pHalSsiAdapter_Master1->SclkPolarity      = SCPOL_INACTIVE_IS_LOW;
    //
    // pHalSsiAdapter_Master1->TransferMode      = TMOD_TR;
    // pHalSsiAdapter_Master1->TransferMode      = TMOD_TO;
    //pHalSsiAdapter_Master1->TransferMode     = TMOD_RO;
    //
    //pHalSsiAdapter_Master1->ControlFrameSize  = CFS_1_BIT;
    //pHalSsiAdapter_Master1->SlaveSelectEnable = 0x1;
    //pHalSsiAdapter_Master1->ClockDivider      = BaudRateBase * BAUDR_MULTIPLIER;
    //pHalSsiAdapter_Master1->TxThresholdLevel  = 1;
    //pHalSsiAdapter_Master1->RxThresholdLevel  = 0;
    //pHalSsiAdapter_Master1->InterruptMask     = 0;
    /* pHalSsiAdapter_Master1->DataFrameNumber   = */  // <---- Master && (Receive Only || EEPROM Read)

    // Rx Sample Delay Register setting
    //SOC_HAL_W32(SPI0_BASE_REG, 0xf0, 0x100);
    //SOC_HAL_W32(SPI1_BASE_REG, 0xf0, 0x100);
#endif
#if 0
    /* SSI0 - Slave */
    pHalSsiAdapter_Slave->Role              = SSI_SLAVE;
    pHalSsiAdapter_Slave->DataFrameSize     = DFS_16_BITS;
    pHalSsiAdapter_Slave->DataFrameFormat   = FRF_MOTOROLA_SPI;
    pHalSsiAdapter_Slave->SclkPhase         = SCPH_TOGGLES_IN_MIDDLE;
    //pHalSsiAdapter_Slave->SclkPolarity      = SCPOL_INACTIVE_IS_HIGH;
    pHalSsiAdapter_Slave->SclkPolarity      = SCPOL_INACTIVE_IS_LOW;
    //
    pHalSsiAdapter_Slave->TransferMode     = TMOD_TR;
    // pHalSsiAdapter_Slave->TransferMode     = TMOD_TO;
    // pHalSsiAdapter_Slave->TransferMode      = TMOD_RO;
    //
    // pHalSsiAdapter_Slave->SlaveOutputEnable = SLV_TXD_DISABLE;
    pHalSsiAdapter_Slave->SlaveOutputEnable = SLV_TXD_ENABLE;
    //
    pHalSsiAdapter_Slave->ControlFrameSize  = CFS_1_BIT;
    pHalSsiAdapter_Slave->TxThresholdLevel  = 1;
    pHalSsiAdapter_Slave->RxThresholdLevel  = 0;
    pHalSsiAdapter_Slave->InterruptMask     = 0;
#endif

    /**
     * Motorola SPI Interrupts Configurations
     * BIT_IMR_MSTIM | BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM | BIT_IMR_TXOIM | BIT_IMR_TXEIM
     */
    #if 1
    //pHalSsiAdapter_Master1->InterruptMask = 0x3F;
    pHalSsiAdapter_Master0->InterruptMask  = 0x7F;
    if ((TestType == SSI_TT_INTERRUPT) || (TestType == SSI_TT_DMA_TO_TX) || (TestType == SSI_TT_RX_TO_DMA)) {
        IRQ_HANDLE  SsiIrqHandle_Master0;
        //IRQ_HANDLE  SsiIrqHandle_Master1;
        if (TestType == SSI_TT_INTERRUPT) {
            //pHalSsiAdapter_Master1->InterruptMask = 0;
            //pHalSsiAdapter_Master0->InterruptMask  = 0; //BIT_IMR_RXFIM;
			pHalSsiAdapter_Master0->InterruptMask  = BIT_IMR_RXOIM | BIT_IMR_RXUIM | BIT_IMR_TXOIM;
        }
        if (TestType == SSI_TT_DMA_TO_TX) {
            //pHalSsiAdapter_Master1->InterruptMask = 0;
            pHalSsiAdapter_Master0->InterruptMask  = BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_TXOIM;
        }
        if (TestType == SSI_TT_RX_TO_DMA) {
            //pHalSsiAdapter_Master1->InterruptMask = 0;
            pHalSsiAdapter_Master0->InterruptMask  = 0;
        }
        /* Common Interrupt Configurations */
        //SsiIrqHandle_Master1.Data     = (u32) (pSsiAdapter_Master1);
        //SsiIrqHandle_Master1.IrqNum   = GetSsiIrqNumber(pHalSsiAdapter_Master1->Index);
        //SsiIrqHandle_Master1.IrqFun   = (IRQ_FUN) SsiAIrqHandle;
        //SsiIrqHandle_Master1.Priority = 0;
        
        //InterruptRegister(&SsiIrqHandle_Master);  20150116
        //InterruptEn(&SsiIrqHandle_Master);
        //irq_install_handler(SsiIrqHandle_Master1.IrqNum, SsiIrqHandle_Master1.IrqFun, SsiIrqHandle_Master1.Data);
        SsiIrqHandle_Master0.Data     = (u32) (pSsiAdapter_Master0);
        //SsiIrqHandle_Master0.IrqNum   = GetSsiIrqNumber(pHalSsiAdapter_Master0->Index);
        SsiIrqHandle_Master0.IrqNum   = 61;
        SsiIrqHandle_Master0.IrqFun   = (IRQ_FUN) SsiAIrqHandle;
        SsiIrqHandle_Master0.Priority = 0;
        //InterruptRegister(&SsiIrqHandle_Slave);
        //InterruptEn(&SsiIrqHandle_Slave);
        irq_install_handler(SsiIrqHandle_Master0.IrqNum, SsiIrqHandle_Master0.IrqFun, SsiIrqHandle_Master0.Data);
    }    
    #endif

    /* SSI Adapter GDMA Configurations */
    if ((TestType == SSI_TT_DMA_TO_TX) || (TestType == SSI_TT_RX_TO_DMA)) {

        if (TestType == SSI_TT_DMA_TO_TX) {
            pHalSsiAdapter_Master0->DmaControl = SSI_TXDMA_ENABLE;
            //pHalSsiAdapter_Slave->DmaControl = SSI_NODMA;
        }

        if (TestType == SSI_TT_RX_TO_DMA) {
#ifdef DMA_TO_TX_AND_RX_TO_DMA
            pHalSsiAdapter_Master0->DmaControl = SSI_TXDMA_ENABLE | SSI_RXDMA_ENABLE;
            //pHalSsiAdapter_Slave->DmaControl  = SSI_RXDMA_ENABLE;
#else
            pHalSsiAdapter_Master0->DmaControl = SSI_RXDMA_ENABLE;
            //pHalSsiAdapter_Slave->DmaControl  = SSI_RXDMA_ENABLE;
#endif
        }

        /* Common Configs */
        pHalSsiAdapter_Master0->DmaTxDataLevel = 48;
        pHalSsiAdapter_Master0->DmaRxDataLevel =  7;
        //pHalSsiAdapter_Slave->DmaTxDataLevel  = 48;
        //pHalSsiAdapter_Slave->DmaRxDataLevel  =  7;
#if 1
        printf("pHalSsiAdapter_Master0->DmaControl     = %d\n", pHalSsiAdapter_Master0->DmaControl);
        printf("pHalSsiAdapter_Master0->DmaTxDataLevel = %d\n", pHalSsiAdapter_Master0->DmaTxDataLevel);
        //printf("pHalSsiAdapter_Slave->DmaControl      = %d\n", pHalSsiAdapter_Slave->DmaControl);
        //printf("pHalSsiAdapter_Slave->DmaRxDataLevel  = %d\n", pHalSsiAdapter_Slave->DmaRxDataLevel);
#endif
    }

    /* Initial SSI Configuration & Enable SSI */
    pHalSsiOp->HalSsiInit((VOID*)pHalSsiAdapter_Master0);
    //pHalSsiOp->HalSsiInit((VOID*)pHalSsiAdapter_Master1);
    pHalSsiOp->HalSsiEnable((VOID*)pHalSsiAdapter_Master0);
    //pHalSsiOp->HalSsiEnable((VOID*)pHalSsiAdapter_Master1);

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
        printf("GDMA Configurations (Tx/Rx)\n");
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
            printf("GDMA Configurations (Tx)(SSI_TT_DMA_TO_TX)\n");
            pSrc = RsvdMemForTx;
            pDst = (u16*) (SSI0_REG_BASE + REG_DW_SSI_DR);  //Master(SPI0)(Tx)
            GdmaIndex_Tx   = 0;
            GdmaChannel_Tx = 0; // TODO: ??/
            GdmaType = SSI_TT_DMA_TO_TX;
            printf("pSrc: %X, pDst: %X, GdmaType: %d\n", pSrc, pDst, GdmaType);
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
#ifdef DMA_TO_TX_AND_RX_TO_DMA

#else
            irq_install_handler(Gdma0IrqHandle_Tx.IrqNum, Gdma0IrqHandle_Tx.IrqFun, Gdma0IrqHandle_Tx.Data);
#endif
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
            pHalGdmaAdapter_Tx->GdmaCfg.DestPer   = 4;//4;  //<---- SPI0 Tx
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
            pHalGdmaAdapter_Tx->IsrCtrl     = 1/*ENABLE*/;
            pHalGdmaAdapter_Tx->GdmaOnOff   = ON;
            pHalGdmaAdapter_Tx->TestItem    = GdmaType;
#if 1//DEBUG_GDMA_CONFIG
            printf("pHalGdmaAdapter_Tx->GdmaIndex = %d\n", pHalGdmaAdapter_Tx->GdmaIndex);
            printf("pHalGdmaAdapter_Tx->ChNum     = %d\n", pHalGdmaAdapter_Tx->ChNum);
            printf("pHalGdmaAdapter_Tx->ChEn      = %04X\n", pHalGdmaAdapter_Tx->ChEn);
#endif
            pHalGdmaAdapter_Tx->GdmaCtl.IntEn      = 1/*ENABLE*/;
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
            printf("GDMA Configurations (Rx)(SSI_TT_RX_TO_DMA)\n");
            pSrc = (u16*) (SSI0_REG_BASE + REG_DW_SSI_DR);
            pDst = RsvdMemForRx;
            GdmaIndex_Rx   = 0;
            GdmaChannel_Rx = 1; // Carl 20150303  2=>0      Carl 20170510 0=>1 not yet tested...
            GdmaType = SSI_TT_RX_TO_DMA;
            printf("pSrc: %X, pDst: %X, GdmaType: %d\n", pSrc, pDst, GdmaType);
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
#ifdef DMA_TO_TX_AND_RX_TO_DMA

#else
            irq_install_handler(Gdma0IrqHandle_Rx.IrqNum, Gdma0IrqHandle_Rx.IrqFun, Gdma0IrqHandle_Rx.Data);
#endif
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
            pHalGdmaAdapter_Rx->ChEn        = GetGdmaChannelEnableValue(GdmaChannel_Rx);    // 20170522
            pHalGdmaAdapter_Rx->GdmaIsrType = (BlockType|TransferType|ErrType);
            pHalGdmaAdapter_Rx->IsrCtrl     = 1/*ENABLE*/;
            pHalGdmaAdapter_Rx->GdmaOnOff   = ON;
            pHalGdmaAdapter_Rx->TestItem    = GdmaType;
#if 1//DEBUG_GDMA_CONFIG
            printf("pHalGdmaAdapter_Rx->GdmaIndex = %d\n", pHalGdmaAdapter_Rx->GdmaIndex);
            printf("pHalGdmaAdapter_Rx->ChNum     = %d\n", pHalGdmaAdapter_Rx->ChNum);
            printf("pHalGdmaAdapter_Rx->ChEn      = %04X\n", pHalGdmaAdapter_Rx->ChEn);
#endif
            pHalGdmaAdapter_Rx->GdmaCtl.IntEn      = 1/*ENABLE*/;
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
     * Serial Transfer STEP_03: Basic I/O transfer testing
     * ------------------------------------------------------------
     * Data generation and put them into SPI0/SPI1 TX FIFO
     * [M->S] Receive Slave Rx FIFO Data
     * [S->M] Receive Master Rx FIFO Data
     * ------------------------------------------------------------
     */

    u32 RegValue;
    u32 Counter;
    u32 WaitCounter;

#if YR_DBG_FLAG
    printf("Flysky: Basic I/O transfer testing...\n");
#endif

    if (TestType == SSI_TT_BASIC_IO)
    {

#if YR_DBG_FLAG
        printf("Flysky: TestType == SSI_TT_BASIC_IO(%d)\n", SSI_TT_BASIC_IO);
        printf("Flysky: %s(%d) send data checking...\n", __FUNCTION__, __LINE__);
#endif
#if 1
        SOC_HAL_W16(SPI0_BASE_REG, 0x60, 0x1234);
        printf("YR: [SPI0] 01-SOC_HAL_W16(0x%08x, 0x60, 0x0123)\n", SPI0_BASE_REG);
        SOC_HAL_W16(SPI0_BASE_REG, 0x60, 0x1111);
        printf("YR: [SPI1] 01-SOC_HAL_W16(0x%08x, 0x60, 0x4567)\n", SPI1_BASE_REG);
        mdelay(10);

        SOC_HAL_W16(SPI0_BASE_REG, 0x60, 0xeeee);
        printf("YR: [SPI0] 02-SOC_HAL_W16(0x%08x, 0x60, 0x89ab)\n", SPI0_BASE_REG);
        SOC_HAL_W16(SPI0_BASE_REG, 0x60, 0x2222);
        printf("YR: [SPI1] 02-SOC_HAL_W16(0x%08x, 0x60, 0xcdef)\n", SPI1_BASE_REG);
        mdelay(10);

        SOC_HAL_W32(SPI0_BASE_REG, 0x60, 0xdddddddd);
        printf("YR: [SPI0] 03-SOC_HAL_W32(0x%08x, 0x60, 0x01234567)\n", SPI0_BASE_REG);
        SOC_HAL_W32(SPI0_BASE_REG, 0x60, 0x33333333);
        printf("YR: [SPI1] 03-SOC_HAL_W32(0x%08x, 0x60, 0x456789ab)\n", SPI1_BASE_REG);
        mdelay(10);

        SOC_HAL_W32(SPI0_BASE_REG, 0x60, 0xcccccccc);
        printf("YR: [SPI0] 03-SOC_HAL_W32(0x%08x, 0x60, 0x89abcaef)\n", SPI0_BASE_REG);
        SOC_HAL_W32(SPI0_BASE_REG, 0x60, 0x44444444);
        printf("YR: [SPI1] 03-SOC_HAL_W32(0x%08x, 0x60, 0xcdef0123)\n", SPI1_BASE_REG);
        mdelay(10);

        printf("\n");

        u4Byte SPI0_ReadData;
        u4Byte SPI1_ReadData;

        SPI0_ReadData = SOC_HAL_R16(SPI0_BASE_REG, 0x60);
        SPI1_ReadData = SOC_HAL_R16(SPI1_BASE_REG, 0x60);
        printf("YR: [SPI0] 01-SOC_HAL_R16(0x%08x, 0x60, 0x%04x)\n", SPI0_BASE_REG, SPI0_ReadData);
        printf("YR: [SPI1] 01-SOC_HAL_R16(0x%08x, 0x60, 0x%04x)\n", SPI1_BASE_REG, SPI1_ReadData);

        SPI0_ReadData = SOC_HAL_R16(SPI0_BASE_REG, 0x60);
        SPI1_ReadData = SOC_HAL_R16(SPI1_BASE_REG, 0x60);
        printf("YR: [SPI0] 02-SOC_HAL_R16(0x%08x, 0x60, 0x%04x)\n", SPI0_BASE_REG, SPI0_ReadData);
        printf("YR: [SPI1] 02-SOC_HAL_R16(0x%08x, 0x60, 0x%04x)\n", SPI1_BASE_REG, SPI1_ReadData);

        SPI0_ReadData = SOC_HAL_R32(SPI0_BASE_REG, 0x60);
        SPI1_ReadData = SOC_HAL_R32(SPI1_BASE_REG, 0x60);
        printf("YR: [SPI0] 03-SOC_HAL_R32(0x%08x, 0x60, 0x%04x)\n", SPI0_BASE_REG, SPI0_ReadData);
        printf("YR: [SPI1] 03-SOC_HAL_R32(0x%08x, 0x60, 0x%04x)\n", SPI1_BASE_REG, SPI1_ReadData);

        SPI0_ReadData = SOC_HAL_R32(SPI0_BASE_REG, 0x60);
        SPI1_ReadData = SOC_HAL_R32(SPI1_BASE_REG, 0x60);
        printf("YR: [SPI0] 04-SOC_HAL_R32(0x%08x, 0x60, 0x%04x)\n", SPI0_BASE_REG, SPI0_ReadData);
        printf("YR: [SPI1] 04-SOC_HAL_R32(0x%08x, 0x60, 0x%04x)\n", SPI1_BASE_REG, SPI1_ReadData);
#endif
#if 1
        Counter = 0;
        WaitCounter = 0;

        u32 cnt0 = 0x0;
        u32 cnt1 = 0xFFFF;

        while (Counter < TestTimes)
        {
            pHalSsiAdapter_Master0->TxData = cnt0;
            //pHalSsiAdapter_Slave->TxData = cnt1;
            cnt0++;
            cnt1--;

            //pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Slave);
            pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master0);
            Counter++;
        }
        // printf("YR: SPI0_REG_BASE + (Index*SSI_REG_OFF) = 0x%x, 
        //        addr = 0x%x, value = 0x%x\n", 
        //        SPI0_REG_BASE + (Index*SSI_REG_OFF), addr, value);
        
        /* wait slave transfer complete */
        do
        {
#if YR_DBG_FLAG
            printf("Master/Slave Receiving...\n");
#endif
            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master0)) |
                    0;//(pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave));

            printf("YR: %s(%d) RegValue = %d\n", __FUNCTION__, __LINE__, RegValue);

            WaitCounter++;
            if (WaitCounter > 40)
                break;
        }
        while (RegValue & BIT_SR_BUSY);
        

        /**
         * Master --> Slave
         * Receive Slave Rx FIFO Data
         */
#if 0
        Counter = 0;
        RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave)) & BIT_SR_RFNE;
        printf("YR %s(%d) RegValue = %d\n", __FUNCTION__, __LINE__, RegValue);

        while (RegValue) // Rx Fifo Not Empty
        {
            printf("[M->S] Check Slave RecvFifoNotEmpty (RFNE) Status = %d\n", RegValue);
            RegValue = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter_Slave);
            printf("YR: %d valid data entries in Slave (SPI%d) RxFIFO\n", RegValue, pHalSsiAdapter_Slave->Index);
        
            while (RegValue--)
            {
                Counter++;
                printf("YR: RecvData = %04x\n", pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter_Slave));
            }
            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave)) & BIT_SR_RFNE;
            printf("YR: Slave Rx FIFO Receive Counter = %d\n", Counter);
        }
#endif
        /**
         * Slave --> Master
         * Receive Master Rx FIFO Data
         */
        Counter = 0;
        RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master0)) & BIT_SR_RFNE;
        printf("YR %s(%d) RegValue = %d\n", __FUNCTION__, __LINE__, RegValue);

        while (RegValue) // Rx Fifo Not Empty
        {
            printf("[S->M] Check Master RecvFifoNotEmpty (RFNE) Status = %d\n", RegValue);
            RegValue = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter_Master0);
            printf("YR: %d valid data entries in Master (SPI%d) RxFIFO\n", RegValue, pHalSsiAdapter_Master0->Index);
    
            while (RegValue--)
            {
                Counter++;
                printf("YR: RecvData = %04x\n", pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter_Master0));
            }
            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master0)) & BIT_SR_RFNE;
            printf("YR: Master Rx FIFO Receive Counter = %d\n", Counter);
        }
#endif
    }

    /**
     * Serial Transfer STEP_04: 
     * ------------------------------------------------------------
     * 
     * ------------------------------------------------------------
     */

#if YR_DBG_FLAG
    printf("Flysky TODO: STEP_04...\n");
#endif

    if (TestType == SSI_TT_INTERRUPT)
    {
#if YR_DBG_FLAG
        printf("TestType == SSI_TT_INTERRUPT(%d)\n", SSI_TT_INTERRUPT);
#endif

#if 1
        u32 TestLoops = 0;
        while (TestLoops < 1)
        {
            // Reset Global Interrupt Test variable value
            Counter = 0;
            RsvdMemForRx_Index = 0;
            RxTransferDone = _FALSE;

		memset(RsvdMemForRx, 0x0, (DATA_SIZE * 2));
            unsigned int i;

            /**/
            #if YR_DBG_FLAG
            for (i = 0; i < 4; i++)
            {
                ("YR Check_Point(%d) = 0x%08x vs 0x%08x \n", __LINE__, RsvdMemForTx[i], RsvdMemForRx[i]);
            }
            #endif

            printf("Send data from SPI1-Master to SPI0-Slave\n");
            while (Counter < DATA_SIZE)
            {
                RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master0);

                // Master -> Slave
                if (RegValue & BIT_SR_TFNF) {
                    pHalSsiAdapter_Master0->TxData = RsvdMemForTx[Counter++];
                    pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master0);
                }
                else {
                    printf("SPI1-Master Tx FIFO Full...\n");
                }
            }

            #if 0
            do {
                RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave);
                printf("SPI0-Slave Receiving...\n");
            }
            while (RegValue & BIT_SR_BUSY);
            #endif

            #if 0
            /**/
            #if YR_DBG_FLAG
            for (i = 0; i < 4; i++)
            {
                ("YR Check_Point(%d) = 0x%08x vs 0x%08x \n", __LINE__, RsvdMemForTx[i], RsvdMemForRx[i]);
            }
            #endif

            while (1)
            {
                printf("%s(%d)\n", __FUNCTION__, __LINE__);
                if (RxTransferDone == _TRUE)
                {
                    if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                        printf("SSI1-SSI_TT_INTERRUPT Success!\n");
                    else
                        printf("SSI1-SSI_TT_INTERRUPT Failed!\n");
                    break;
                }
            }
            #endif

            printf("RsvdMemForRx_Index: %d\n", RsvdMemForRx_Index);
            TestLoops++;
            printf("Round %d\n", TestLoops);
        }
#endif
    }

    if (TestType == SSI_TT_DMA_TO_TX)
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
    printf("!trbuffer 0x%08x, 0x%08x\n",(u32)RsvdMemForTx, (u32)RsvdMemForRx);
            printf(ANSI_COLOR_YELLOW"Test Type: SSI_TT_DMA_TO_TX(%d)\n"ANSI_COLOR_RESET, SSI_TT_DMA_TO_TX);
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

                printf("-- DMA %d Channel 0x%X Enable --\n",
                        pHalGdmaAdapter_Tx->GdmaIndex, GetGdmaChannelEnableValue(GdmaChannel_Tx));
                pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter_Tx));

                WaitCounter = 0;
                /* Slave is busy receiving something... */
                while (RsvdMemForRx_Index < DATA_SIZE) {
                    printf("[%d] Not yet, Busy waiting...\n", RsvdMemForRx_Index);
					//
					WaitCounter++;
					if (WaitCounter > 40)
						break;
                    mdelay(2);
                }

                WaitCounter = 0;
                while (1) {
                    if (RxTransferDone == _TRUE) {
                        /* printf(ANSI_COLOR_RED"Rx Complete !\n"ANSI_COLOR_RESET); */

                        if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                            printf(ANSI_COLOR_CYAN"SSI_TT_DMA_TO_TX Success!\n"ANSI_COLOR_RESET);
                        else
                            printf(ANSI_COLOR_RED"SSI_TT_DMA_TO_TX Failed!\n"ANSI_COLOR_RESET);

                        /* GDMA Channel Disable and On/Off */
                        pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter_Tx));

                        break;
                    }
					WaitCounter++;
					if (WaitCounter > 40)
						break;
                }

                /* printf(ANSI_COLOR_CYAN"RsvdMemForRx_Index: %d\n"ANSI_COLOR_RESET, RsvdMemForRx_Index); */
                TestLoops++;
                printf(ANSI_COLOR_RED"Round %d\n"ANSI_COLOR_RESET, TestLoops);

            } //while loop

#if 1	// Carl 20150302
unsigned int iCheckCount = 0;
for (iCheckCount = 0; iCheckCount<DATA_SIZE;iCheckCount++)
{
	printf("Check_DATA(%d) = 0x%08x vs 0x%08x \n", __LINE__,RsvdMemForTx[iCheckCount], RsvdMemForRx[iCheckCount]);
}
#endif

#if DEBUG_GDMA_CONFIG
                /**
                 * Print GDMA Settings
                 */
                printf("------------------------\n");
                printf(ANSI_COLOR_CYAN"GDMA Configurations:\n"ANSI_COLOR_RESET);
                printf("\tGdmaCtl.BlockSize : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.BlockSize);
                printf("\tGdmaCtl.TtFc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.TtFc);
                printf("\tGdmaCtl.Done      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Done);
                printf("\tGdmaCfg.ReloadDst : %d\n", pHalGdmaAdapter_Tx->GdmaCfg.ReloadDst);
                printf("\tGdmaCfg.DestPer   : %d\n", pHalGdmaAdapter_Tx->GdmaCfg.DestPer);
                printf("\tMuliBlockCunt     : %d\n", pHalGdmaAdapter_Tx->MuliBlockCunt);
                printf("\tMaxMuliBlock      : %d\n", pHalGdmaAdapter_Tx->MaxMuliBlock);
                printf("\tChSar             : %X\n", pHalGdmaAdapter_Tx->ChSar);
                printf("\tChDar             : %X\n", pHalGdmaAdapter_Tx->ChDar);
                printf("\tGdmaIndex         : %d\n", pHalGdmaAdapter_Tx->GdmaIndex);
                printf("\tChNum             : %d\n", pHalGdmaAdapter_Tx->ChNum);
                printf("\tChEn              : %X\n", pHalGdmaAdapter_Tx->ChEn);
                printf("\tGdmaIsrType       : %d\n", pHalGdmaAdapter_Tx->GdmaIsrType);
                printf("\tIsrCtrl           : %d\n", pHalGdmaAdapter_Tx->IsrCtrl);
                printf("\tGdmaOnOff         : %d\n", pHalGdmaAdapter_Tx->GdmaOnOff);
                printf("\tTestItem          : %d\n", pHalGdmaAdapter_Tx->TestItem);
                printf("\tGdmaCtl.IntEn     : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.IntEn);
                printf("\tGdmaCtl.SrcMsize  : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.SrcMsize);
                printf("\tGdmaCtl.DestMsize : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.DestMsize);
                printf("\tGdmaCtl.SrcTrWidth: %d\n", pHalGdmaAdapter_Tx->GdmaCtl.SrcTrWidth);
                printf("\tGdmaCtl.DstTrWidth: %d\n", pHalGdmaAdapter_Tx->GdmaCtl.DstTrWidth);
                printf("\tGdmaCtl.Dinc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Dinc);
                printf("\tGdmaCtl.Sinc      : %d\n", pHalGdmaAdapter_Tx->GdmaCtl.Sinc);
#endif

    }

    if (TestType == SSI_TT_RX_TO_DMA)
    {
            printf(ANSI_COLOR_YELLOW"Test Type: SSI_TT_RX_TO_DMA(%d)\n"ANSI_COLOR_RESET, SSI_TT_RX_TO_DMA);
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

			u32 start = (u32)RsvdMemForRx & ~(ARCH_DMA_MINALIGN - 1);
			u32 end = (u32)(RsvdMemForRx + DATA_SIZE + ARCH_DMA_MINALIGN) & ~(ARCH_DMA_MINALIGN - 1);   // 20170412
			printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, start, end);
#if 1
			flush_cache(start, (end-start));
			//invalidate_dcache_range(start, end);
			printf("flush_cache start: 0x%08x, end: 0x%08x\n",(u32)RsvdMemForRx, (u32)(&RsvdMemForRx[0]), (u32)(&RsvdMemForRx[DATA_SIZE-1]));
#endif
	
                pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter_Rx));
                pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter_Rx));

                printf("-- DMA %d Channel 0x%X Enable --\n",
                        pHalGdmaAdapter_Rx->GdmaIndex, GetGdmaChannelEnableValue(GdmaChannel_Rx));
                pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter_Rx));

#ifdef DMA_TO_TX_AND_RX_TO_DMA

#if 1
    u32 start1 = (u32)RsvdMemForTx & ~(ARCH_DMA_MINALIGN - 1);
    u32 end1 = (u32)(RsvdMemForTx + DATA_SIZE*2);
    
    printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, start1, end1);
    
    #if 1
    //flush_cache(start1, (end1-start1));
    //invalidate_dcache_range(start1, end1);  // 20141222

    printf("!!!!! 0x%08x, 0x%08x\n",(u32)RsvdMemForTx, (u32)(&RsvdMemForTx[0]));
    #endif
#endif

                SOC_HAL_W32(0xF4360000, 0x0, &RsvdMemForTx[0]);
                SOC_HAL_W32(0xF4360000, 0x8, SPI0_BASE_REG + 0x60);

                RegValue = 0;
                do {
                    RegValue ++;
                    printf("enable RX to DMA & setting TX DMA config");  // Need to wait some time?
                }
                while (RegValue < 100);
                printf("\n"); 

                printf("-- DMA %d Channel 0x%X Enable --\n",
                        pHalGdmaAdapter_Rx->GdmaIndex, GetGdmaChannelEnableValue(GdmaChannel_Tx));
                SOC_HAL_W32(0xF4360000, 0x3a0, 0x101);

                RegValue = 0;
                do {
                    RegValue ++;
                    printf("DMA to TX wait...");  // Need to wait some time
                }
                while (RegValue < 1000);
                printf("\n");
#else
                while (Counter < DATA_SIZE) {
                    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Master0);
                    
                    if (RegValue & BIT_SR_TFNF) {
                        pHalSsiAdapter_Master0->TxData = RsvdMemForTx[Counter++];
                        pHalSsiOp->HalSsiWriter((VOID*)pHalSsiAdapter_Master0);
                    }
                    else
                        printf(ANSI_COLOR_YELLOW"Tx FIFO Full...\n"ANSI_COLOR_RESET);
                    mdelay(2);
				}
#endif
                /**
                 * Wait Slave transfer complete.
                 */
                //do {
                //    RegValue = pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter_Slave);
                //    printf("Slave Receiving...\n");  // Need to print some dummy message.
                //}
                //while (RegValue & BIT_SR_BUSY);

#if 1   // 20150306 invalidate cache
     //u32 start = (u32)RsvdMemForRx & ~(ARCH_DMA_MINALIGN - 1);
     //end = (u32)(RsvdMemForRx + DATA_SIZE); // TODO: need to check why add DATA_SIZE

     //u32 end = (u32)(RsvdMemForRx + DATA_SIZE + ARCH_DMA_MINALIGN) & ~(ARCH_DMA_MINALIGN - 1);   // 20170412
    
    //printf("%s(%d): 0x%x, 0x%x \n", __FUNCTION__, __LINE__, start, end);
    
    #if 1
    //flush_cache(start, (end-start));
    //invalidate_dcache_range(start, end);

   // printf("!!!!! 0x%08x, 0x%08x ~ 0x%08x\n",(u32)RsvdMemForRx, (u32)(&RsvdMemForRx[0]), (u32)(&RsvdMemForRx[DATA_SIZE-1]));
    #endif
#endif

#if 1   // 20170412
                RegValue = 0;
                do {
                    RegValue ++;
                    printf("wait...wait...");  // Need to print some dummy message.
                }
                while (RegValue < 200);
                printf("\n"); 
#endif

                /**
                 * No need to wait some signal.
                 * Assuming DMA is fast enougth to finish the job.
                 */
                if (SsiDataCompare(RsvdMemForTx, RsvdMemForRx, DATA_SIZE))
                    printf(ANSI_COLOR_CYAN"SSI_TT_RX_TO_DMA Success!\n"ANSI_COLOR_RESET);
                else
                    printf(ANSI_COLOR_RED"SSI_TT_RX_TO_DMA Failed!\n"ANSI_COLOR_RESET);

                /* Too fast ??? */
                /* GDMA Channel Disable and On/Off */
                pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter_Rx));

                TestLoops++;
                printf(ANSI_COLOR_RED"Round %d\n"ANSI_COLOR_RESET, TestLoops);
            } //while loop
#if DEBUG_GDMA_CONFIG
                /**
                 * Print GDMA Settings
                 */
                printf("------------------------\n");
                printf(ANSI_COLOR_CYAN"GDMA Configurations:\n"ANSI_COLOR_RESET);
                printf("\tGdmaCtl.BlockSize : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.BlockSize);
                printf("\tGdmaCtl.TtFc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.TtFc);
                printf("\tGdmaCtl.Done      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Done);
                printf("\tGdmaCfg.ReloadDst : %d\n", pHalGdmaAdapter_Rx->GdmaCfg.ReloadDst);
                printf("\tGdmaCfg.DestPer   : %d\n", pHalGdmaAdapter_Rx->GdmaCfg.DestPer);
                printf("\tMuliBlockCunt     : %d\n", pHalGdmaAdapter_Rx->MuliBlockCunt);
                printf("\tMaxMuliBlock      : %d\n", pHalGdmaAdapter_Rx->MaxMuliBlock);
                printf("\tChSar             : %X\n", pHalGdmaAdapter_Rx->ChSar);
                printf("\tChDar             : %X\n", pHalGdmaAdapter_Rx->ChDar);
                printf("\tGdmaIndex         : %d\n", pHalGdmaAdapter_Rx->GdmaIndex);
                printf("\tChNum             : %d\n", pHalGdmaAdapter_Rx->ChNum);
                printf("\tChEn              : %X\n", pHalGdmaAdapter_Rx->ChEn);
                printf("\tGdmaIsrType       : %d\n", pHalGdmaAdapter_Rx->GdmaIsrType);
                printf("\tIsrCtrl           : %d\n", pHalGdmaAdapter_Rx->IsrCtrl);
                printf("\tGdmaOnOff         : %d\n", pHalGdmaAdapter_Rx->GdmaOnOff);
                printf("\tTestItem          : %d\n", pHalGdmaAdapter_Rx->TestItem);
                printf("\tGdmaCtl.IntEn     : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.IntEn);
                printf("\tGdmaCtl.SrcMsize  : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.SrcMsize);
                printf("\tGdmaCtl.DestMsize : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.DestMsize);
                printf("\tGdmaCtl.SrcTrWidth: %d\n", pHalGdmaAdapter_Rx->GdmaCtl.SrcTrWidth);
                printf("\tGdmaCtl.DstTrWidth: %d\n", pHalGdmaAdapter_Rx->GdmaCtl.DstTrWidth);
                printf("\tGdmaCtl.Dinc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Dinc);
                printf("\tGdmaCtl.Sinc      : %d\n", pHalGdmaAdapter_Rx->GdmaCtl.Sinc);
#endif
            

	}
}

VOID
SsiAIrqHandle(
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
    printf("SSI%d Interrupt Status = %02X\n", Index, InterruptStatus);

    /* Transmit FIFO Empty Interrupt (ssi_txe_intr)
     * Set when the transmit FIFO is equal to or below its threshold value. 
     * This interrupt is cleared by hardware when data are written into the transmit FIFO buffer.
     */
    if (InterruptStatus & BIT_ISR_TXEIS) {
        printf("[INT] Transmit FIFO Empty Interrupt\n");
    }

    /* Transmit FIFO Overflow Interrupt (ssi_txo_intr)
     * Set when an APB access attempts to write into the transmit FIFO after it has been completely filled.
     * This interrupt remains set until you read the transmit FIFO overflow interrupt clear register (TXOICR).
     */
    if (InterruptStatus & BIT_ISR_TXOIS) {
        printf("[INT] Transmit FIFO Overflow Interrupt\n");
        HAL_SSI_READ32(Index, REG_DW_SSI_TXOICR);
    }

    // TO DO: Receive FIFO Full Interrupt
    /* Receive FIFO Full Interrupt (ssi_rxf_intr)
     * Set when the receive FIFO is equal to or above its threshold value plus 1.
     * This interrupt is cleared by hardware when data are read from the receive FIFO buffer.
     */
    if ((InterruptStatus & BIT_ISR_RXFIS) ) {
        //printf("[INT] Receive FIFO Full Interrupt\n");
		//not print avoid data lost at tx rx test
        HAL_SSI_READ32(Index, BIT_ISR_RXFIS);
    }

    /* Receive FIFO Overflow Interrupt (ssi_rxo_intr)
     * Set when the receive printfc attempts to place data into the receive FIFO after it has been completely filled.
     * This interrupt remains set until you read the receive FIFO overflow interrupt clear register (RXOICR).
     */
    if (InterruptStatus & BIT_ISR_RXOIS) {
        printf("[INT] Receive FIFO Overflow Interrupt\n");
        HAL_SSI_READ32(Index, REG_DW_SSI_RXOICR);
    }

    /* Receive FIFO Underflow Interrupt (ssi_rxu_intr)
     * Set when an APB access attempts to read from the receive FIFO when it is empty.
     * This interrupt remains set until you read the receive FIFO underflow interrupt clear register (RXUICR).
     */
    if (InterruptStatus & BIT_ISR_RXUIS) {
        printf("[INT] Receive FIFO Underflow Interrupt\n");
        HAL_SSI_READ32(Index, REG_DW_SSI_RXUICR);
    }

    /* Multi-Master Contention Interrupt (ssi_mst_intr)
     * Present only when the DW_apb_ssi component is configured as a serial-master device.
     * The interrupt is set when another serial master on the serial bus selects the DW_apb_ssi master as a serial-slave device and is actively transferring data.
     * This interrupt remains set until you read the multi-master interrupt clear register (MSTICR).
     */
    if (InterruptStatus & BIT_ISR_MSTIS) {
        printf("[INT] Multi-Master Contention Interrupt\n");
        /* Another master is actively transferring data */
        /* Do reading data... */
        HAL_SSI_READ32(Index, REG_DW_SSI_MSTICR);
    }

    /* Combined Interrupt Request (ssi_intr)
     * OR'ed result of all the above interrupt requests after masking.
     * To mask this interrupt signal, you must mask all other DW_apb_ssi interrupt requests.
     */

    #if 1
    //printf("[INT] Rx FIFO is equal to threshold(+1), Receive Data...\n");
	//not print avoid data lost at tx rx test
    if ((InterruptStatus & BIT_ISR_RXFIS) ) {
        Counter = 0;
        /* RsvdMemForRx_Index_Local = RsvdMemForRx_Index; */
        RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter)) & BIT_SR_RFNE;

        while (RegValue) {  //Rx Fifo Not Empty
            Counter = pHalSsiOp->HalSsiGetRxFifoLevel(pHalSsiAdapter);

            while (Counter--) {
                /* RsvdMemForRx[RsvdMemForRx_Index_Local++] = pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter); */
                RsvdMemForRx[RsvdMemForRx_Index++] = pHalSsiOp->HalSsiReader((VOID*)pHalSsiAdapter); //OK
                /* printf(ANSI_COLOR_RED"%04X\n"ANSI_COLOR_RESET, RsvdMemForRx[RsvdMemForRx_Index-1]); */

                if (RsvdMemForRx_Index == DATA_SIZE) {
                    RxTransferDone = _TRUE;
                }
            }

            RegValue = (pHalSsiOp->HalSsiGetStatus((VOID*)pHalSsiAdapter)) & BIT_SR_RFNE;
        }
        /* printf("RsvdMemForRx_Index = %d\n", RsvdMemForRx_Index);  // Need this to avoid GCC compiler optimization. */
        /* printf("RsvdMemForRx_Index_Local = %d\n", RsvdMemForRx_Index_Local); */
        /* RsvdMemForRx_Index = RsvdMemForRx_Index_Local; */
    }
    #endif
}

VOID
SsiBIrqHandle(
        IN VOID *Data
        )
{
    /* DBG_ENTRANCE; */
    u32 RegValue;
    u32 InterruptStatus;
    PSSI_ADAPTER pSsiAdapter = (PSSI_ADAPTER) Data;
    PHAL_SSI_OP  pHalSsiOp   = pSsiAdapter->pHalSsiOp;
    PHAL_SSI_ADAPTER pHalSsiAdapter = pSsiAdapter->pHalSsiAdapter;
    u8 Index = pHalSsiAdapter->Index;
    u8 Role  = pHalSsiAdapter->Role;
#if YR_DBG_FLAG
    printf("SSI%d(%d) InterruptHandler\n", Index, Role);
#endif
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
    printf("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case SSI_TT_DMA_TO_TX:
            printf(ANSI_COLOR_MAGENTA"DMA --> Tx (SSI_TT_DMA_TO_TX)\n"ANSI_COLOR_RESET);
            break;
        default:
            break;
    }

    /* Clear Pending ISR */
    IsrTypeMap = HalGdmaOpSPI.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
#if 0
    if (IsrTypeMap & BlockType) {
        printf("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt);
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
    printf("GDMA %d; Channel %d, Isr Type: %d\n",
                  pHalGdmaAdapter->GdmaIndex,
                  pHalGdmaAdapter->ChNum,
                  pHalGdmaAdapter->GdmaIsrType);
#endif
    switch (pHalGdmaAdapter->TestItem) {
        case SSI_TT_RX_TO_DMA:
#ifdef DMA_TO_TX_AND_RX_TO_DMA
            printf(ANSI_COLOR_MAGENTA"DMA->Tx then Rx ->DMA\n"ANSI_COLOR_RESET);
#else
            printf(ANSI_COLOR_MAGENTA"Rx --> DMA (SSI_TT_RX_TO_DMA)\n"ANSI_COLOR_RESET);
#endif
            break;
        default:
            break;
    }

    /* Clear Pending ISR */
    IsrTypeMap = HalGdmaOpSPI.HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    /* Maintain Block Count */
#if 0
    if (IsrTypeMap & BlockType) {
        printf("DMA Block %d\n",pHalGdmaAdapter->MuliBlockCunt);
        pHalGdmaAdapter->MuliBlockCunt++;
    }
#endif
}


VOID
SsiIpEnable();

u16
GetGdmaChannelEnableValue(
        u8 GdmaChannel
        )
{
    /* DBG_ENTRANCE; */
    u16 ChannelEnableValue;
#if 1
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
     printf("ChannelEnableValue = %04X\n", ChannelEnableValue); 

#else
    //ChannelEnableValue = 1;
#endif
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
        case 0:  {SsiIrqNumber = DW_SSI_0_IP_IRQ; break;}
        case 1:  {SsiIrqNumber = DW_SSI_1_IE_IRQ; break;}
        case 2:  {SsiIrqNumber = DW_SSI_0_IP_IRQ; break;}
        default: {SsiIrqNumber = 0;        break;}
    }

    printf("SSI%d IRQ: %d\n", SsiIndex, SsiIrqNumber);
    return SsiIrqNumber;
}

VOID
DumpSsiRegInfo(
        u8 SsiIndex
        )
{}

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
#if 1
        printf("pSrc[%d] = %04X\n", Index, pSrc[Index]);
        printf("pDst[%d] = %04X\n", Index, pDst[Index]);
#endif
        if (pSrc[Index] != pDst[Index])
        {
#if YR_DBG_FLAG
            printf(" 0x%08x  0x%08x Failed index=%d (%x != %x)\n", pSrc, pDst,Index, pSrc[Index], pDst[Index]);
#endif
            for (Index=0; Index < Length; ++Index)
            {
                printf("pSrc[%d] = %04X\n", Index, pSrc[Index]);
                printf("pDst[%d] = %04X\n", Index, pDst[Index]);                
            }
            return _FALSE;
        }
    }
    printf("Pass\n");
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
