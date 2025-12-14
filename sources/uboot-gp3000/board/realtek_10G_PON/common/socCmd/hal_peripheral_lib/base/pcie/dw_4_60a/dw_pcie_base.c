/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "common.h"
#include "dw_pcie_base.h"
#include "peripheral.h"
#include "dw_pcie_base_test.h"

//#include "dw_hal_pcie.h"

u8 *pTxDescAddr = 0;
u8 *pRxDescAddr = 0;

u8 *pRxBuffAddr = 0;
u8 *pTxBuffAddr = 0;

#define REG(reg)  (*(volatile unsigned int *)(reg))
void
HalPCIeRCConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE32(PCIE0_DBI_REG_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE32(PCIE1_DBI_REG_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u32
HalPCIeRCConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u32 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ32(PCIE0_DBI_REG_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ32(PCIE1_DBI_REG_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}


void
HalPCIeRCConfigWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE8(PCIE0_DBI_REG_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE8(PCIE1_DBI_REG_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u8
HalPCIeRCConfigRead8(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u8 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ8(PCIE0_DBI_REG_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ8(PCIE1_DBI_REG_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}


void
HalPCIeRCExtendConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE32(PCIE0_EXTENDED_REG_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE32(PCIE1_EXTENDED_REG_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u32
HalPCIeRCExtendConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u32 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ32(PCIE0_EXTENDED_REG_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ32(PCIE1_EXTENDED_REG_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}


void
HalPCIeEPConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE32(PCIE0_CFG0_REG_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE32(PCIE1_CFG0_REG_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u32
HalPCIeEPConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u32 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ32(PCIE0_CFG0_REG_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ32(PCIE1_CFG0_REG_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}


void
HalPCIeEPConfigWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE8(PCIE0_CFG0_REG_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE8(PCIE1_CFG0_REG_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u8
HalPCIeEPConfigRead8(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u8 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ8(PCIE0_CFG0_REG_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ8(PCIE1_CFG0_REG_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}

void
HalPCIeEPMemWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE32(PCIE0_MEM_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE32(PCIE1_MEM_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u32
HalPCIeEPMemRead32(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u32 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ32(PCIE0_MEM_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ32(PCIE1_MEM_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}

void
HalPCIeEPMemWrite16(IN u32 PCIeIdx, IN u32 PCIeReg, IN u16 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE16(PCIE0_MEM_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE16(PCIE1_MEM_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u16
HalPCIeEPMemRead16(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u16 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ16(PCIE0_MEM_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ16(PCIE1_MEM_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}

void
HalPCIeEPMemWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal)
{
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			HAL_WRITE8(PCIE0_MEM_BASE, PCIeReg, PCIeVal);
			break;

		case PCIE_MODULE_SEL_1:
			HAL_WRITE8(PCIE1_MEM_BASE, PCIeReg, PCIeVal);
			break;

		default:
			break;
	}

	return;
}

u8
HalPCIeEPMemRead8(IN u32 PCIeIdx, IN u32 PCIeReg)
{
	u8 PCIeDatRd = 0;

	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ8(PCIE0_MEM_BASE, PCIeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ8(PCIE1_MEM_BASE, PCIeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}

void 
HalPCIePHYParameterWrite(int offset, int value)
{
        HAL_WRITE32_MEM( (PCIE_PHY_BASE | (offset << 2) ), value);
        return;
}


void
PCIeEPInitDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 loopbackEn = pHalPCIeAdapter->loopbackEn;
	u32 usedMem = pHalPCIeAdapter->usedMem;
	
	u32 value32, Bar0, Bar2, count;	
	u32 MapIOBase, MapMemBase;

	/* Endpoint is RTL8168 (PCIE ethernet NIC card) */
	
	if(PCIeIdx == PCIE_MODULE_SEL_0)
	{
		MapIOBase = PCIE0_IO_BASE;
		MapMemBase = PCIE0_MEM_BASE;

		switch(usedMem)
		{
			case PCIE_TRX_USED_MEM_DDR:
				pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR_DDR;
				pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR_DDR;
				
				pTxBuffAddr = (u8 *)PCIE0_TX_BUFF_ADDR_DDR;
				pRxBuffAddr = (u8 *)PCIE0_RX_BUFF_ADDR_DDR;
				break;
			case PCIE_TRX_USED_MEM_SRAM:
				pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR_SRAM;
				pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR_SRAM;
				
				pTxBuffAddr = (u8 *)PCIE0_TX_BUFF_ADDR_SRAM;
				pRxBuffAddr = (u8 *)PCIE0_RX_BUFF_ADDR_SRAM;
				break;
			case PCIE_TRX_USED_MEM_DMEM:
				pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR_DMEM;
				pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR_DMEM;
				
				pTxBuffAddr = (u8 *)PCIE0_TX_BUFF_ADDR_DMEM;
				pRxBuffAddr = (u8 *)PCIE0_RX_BUFF_ADDR_DMEM;
				break;
			case PCIE_TRX_USED_MEM_XRAM:
				pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR_XRAM;
				pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR_XRAM;
				
				pTxBuffAddr = (u8 *)PCIE0_TX_BUFF_ADDR_XRAM;
				pRxBuffAddr = (u8 *)PCIE0_RX_BUFF_ADDR_XRAM;
				break;
			default:
				break;
		}
	}else if (PCIeIdx == PCIE_MODULE_SEL_1)
	{
		MapIOBase = PCIE1_IO_BASE;
		MapMemBase = PCIE1_MEM_BASE;
		
		switch(usedMem)
		{
			case PCIE_TRX_USED_MEM_DDR:
				pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR_DDR;
				pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR_DDR;
				
				pTxBuffAddr = (u8 *)PCIE1_TX_BUFF_ADDR_DDR;
				pRxBuffAddr = (u8 *)PCIE1_RX_BUFF_ADDR_DDR;
				break;
			case PCIE_TRX_USED_MEM_SRAM:
				pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR_SRAM;
				pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR_SRAM;
				
				pTxBuffAddr = (u8 *)PCIE1_TX_BUFF_ADDR_SRAM;
				pRxBuffAddr = (u8 *)PCIE1_RX_BUFF_ADDR_SRAM;
				break;
			case PCIE_TRX_USED_MEM_DMEM:
				pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR_DMEM;
				pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR_DMEM;
				
				pTxBuffAddr = (u8 *)PCIE1_TX_BUFF_ADDR_DMEM;
				pRxBuffAddr = (u8 *)PCIE1_RX_BUFF_ADDR_DMEM;
				break;
			case PCIE_TRX_USED_MEM_XRAM:
				pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR_XRAM;
				pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR_XRAM;
				
				pTxBuffAddr = (u8 *)PCIE1_TX_BUFF_ADDR_XRAM;
				pRxBuffAddr = (u8 *)PCIE1_RX_BUFF_ADDR_XRAM;
				break;
			default:
				break;
		}
	}else
	{
		printf("Error parameter!\n");
		return;
	}
	
	//2 0. Set PCIe Root Complex
	HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_TYPE1_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));

	// Set MAX_PAYLOAD_SIZE to 128B
	value32 = HalPCIeRCConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_CONTROL_DEVICE_STATUS);
	value32 = value32 & (~ BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS(BIT_MASK_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS));
	value32 = value32 | (BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS(DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_128));
	HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_DEVICE_CONTROL_DEVICE_STATUS, value32);


	//2 1. Set 8111C EP
	// Mem, IO Enable
	HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_INT_STATUS(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));

	// Set BAR
	Bar0 = PADDR(MapIOBase | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_IO));
	Bar2 = PADDR(MapMemBase | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_MEM) | BIT_CTRL_DW_PCIE_BAR0_TYPE(DW_PCIE_BAR_TYPE_64BIT_ADDRESS));

	HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR0, Bar0);
	HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR2, Bar2);

	// Set MAX_PAYLOAD_SIZE to 128B
	value32 = HalPCIeEPConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_CONTROL_DEVICE_STATUS);
	value32 = value32 & (~ BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS(BIT_MASK_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS)) & (~ BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_READ_REQ_SIZE(BIT_MASK_DW_PCIE_PCIE_CAP_MAX_READ_REQ_SIZE)) ;
	value32 = value32 | ( BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS(DW_PCIE_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_128) | BIT_CTRL_DW_PCIE_PCIE_CAP_MAX_READ_REQ_SIZE(DW_PCIE_PCIE_CAP_MAX_READ_REQ_SIZE_256));
	HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_DEVICE_CONTROL_DEVICE_STATUS, value32);

	
	//2 2. Reset EP
	HalPCIeEPMemWrite8(PCIeIdx, ChipCmd, 0x10);

	
	//2 3. Set MAC Loopback & Disable TX CRC & TxDMA Size	
	value32 = HalPCIeEPMemRead32(PCIeIdx, TxConfig);
	value32 = value32 & (~0x700);
	if(loopbackEn)
	{
		value32 = value32 | (TxDMA1KB | TxMACLoopBack | (1 << 16));
	}else
	{
		value32 = value32 | (TxDMA1KB | (0 << 16));
	}
	HalPCIeEPMemWrite32(PCIeIdx, TxConfig, value32);

	// Enable Runt & Error Accept of RX Config
	value32 = HalPCIeEPMemRead32(PCIeIdx, RxConfig);
	value32 = value32 & (~0x700);
	value32 = value32 | (RxDMA512B | AcceptErr | AcceptRunt | (1 << 7));
	HalPCIeEPMemWrite32(PCIeIdx, RxConfig, value32);
	

	//2 4. Set TX/RX Desciptor Starting Address
	HalPCIeEPMemWrite32(PCIeIdx, TxDescStartAddrLow, PADDR((unsigned int)pTxDescAddr));
	HalPCIeEPMemWrite32(PCIeIdx, TxDescStartAddrHigh, 0);
	HalPCIeEPMemWrite32(PCIeIdx, RxDescAddrLow, PADDR((unsigned int)pRxDescAddr));
	HalPCIeEPMemWrite32(PCIeIdx, RxDescAddrHigh, 0);
		
	//2 5. Set TX Ring - Descriptor Assigned to CPU
	memset((unsigned char *) pTxDescAddr, 0x0, PCIE_NUM_TX_DESC * PCIE_TX_DESC_SIZE);
	for (count = 0; count < PCIE_NUM_TX_DESC; count++)
	{
		if(count == (PCIE_NUM_TX_DESC - 1)){
			HAL_WRITE32_MEM(pTxDescAddr + PCIE_TX_DESC_SIZE * count, RingEnd);
		}
	}
	
	//2 6. Set RX Ring - Descriptor Assigned to NIC
	memset((unsigned char *) pRxDescAddr, 0x0, PCIE_NUM_RX_DESC * PCIE_RX_DESC_SIZE);
	for (count = 0; count < PCIE_NUM_RX_DESC; count++)
	{
		if(count == (PCIE_NUM_RX_DESC - 1)){
			HAL_WRITE32_MEM(pRxDescAddr + PCIE_RX_DESC_SIZE * count, (DescOwn | RingEnd | PCIE_RX_BUFF_SIZE));
		}else{
			HAL_WRITE32_MEM(pRxDescAddr + PCIE_RX_DESC_SIZE * count, (DescOwn | PCIE_RX_BUFF_SIZE));
		}
	}
	
#if USING_INTERRUPT
	EnIRQ(0, 1);
#endif

	return;
}


void
PCIeEPTxDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 length = pHalPCIeAdapter->length;

	// Fill RX Descriptor
	HAL_WRITE32_MEM(pRxDescAddr + 0x08, PADDR((unsigned int)pRxBuffAddr));	// RX Buffer Address
	HAL_WRITE32_MEM(pRxDescAddr + 0x18, PADDR((unsigned int)pRxBuffAddr));	// RX Buffer Address
	
	// Ensure Descriptor is updated
	HAL_READ32_MEM(pRxDescAddr + 0x8);
	
	// Enable TX/RX (This seems to trigger NIC prefetching RX descriptor)
	HalPCIeEPMemWrite8(PCIeIdx, ChipCmd, 0x0c);
	
	// Fill Descriptor
	HAL_WRITE32_MEM(pTxDescAddr + 0x0, (DescOwn | FirstFrag | LastFrag | RingEnd | length));
	HAL_WRITE32_MEM(pTxDescAddr + 0x8, PADDR((unsigned int)pTxBuffAddr));
	
	// Ensure Descriptor is updated
	HAL_READ32_MEM(pTxDescAddr + 0x8);

	// Tx trigger
	HalPCIeEPMemWrite8(PCIeIdx, TxPoll, 0x40);
	
	return;
}


int
PCIeEPRxDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 length = pHalPCIeAdapter->length;
	u32 count, loopcount = 0;
	
	// Wait RX Packet   
	while ((HAL_READ32_MEM(pRxDescAddr) & DescOwn) != 0)
	{
		mdelay(10);
		loopcount++;
		//Memory own => "0":CPU ; "1":PCIE IP
		//loop here when OWN bit=1
		if (loopcount == 200)
		{
			printf("Can't find Rx packet!\n");
			return PCIE_EP_RX_STATUS_FAIL;
		}
	}

	// Check RX Packet Content
	for (count = 0; count < length; count++)        
	{
		if (HAL_READ8_MEM(pRxBuffAddr + count) != pTxBuffAddr[count])
		{
			printf("Compare Error, Length: %d, Offset: %d, Content: 0x%X, Expected: 0x%X\n",
							length, count, HAL_READ8_MEM(pRxBuffAddr + count), pTxBuffAddr[count]);
			return PCIE_EP_RX_STATUS_FAIL;
		}
	}

	return PCIE_EP_RX_STATUS_SUCCESS;
}

int 
PCIeCheckLinkDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
        u32 link_speed, bar0, bar2;
	volatile int count = 50;

	//wait for LinkUP
	mdelay(10);
	
	while(--count)
	{
		mdelay(10);	  
		if( ( HalPCIeRCConfigRead32(PCIeIdx, REG_DW_PCIE_PL_DEBUG0) & 0x1f) == 0x11)
		{
			break;
		}
	}
	
	if(count == 0){	
#ifdef CONFIG_98FH_MP_ENABLE
printf("{F015}\n");REG(0xf4320734)++;
#else 
		printf("PCIE_P%d ->  Cannot LinkUP\r\n", PCIeIdx);
#endif
		return PCIE_LINK_FAIL;
	}
	else  //already  linkup
	{
	        HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_TYPE1_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));
 #ifndef CONFIG_98FH_MP_ENABLE
                printf("Find Port=%x, Device:Vender ID=%x\n", PCIeIdx, HalPCIeEPConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_ID_VENDOR_ID) );
#endif
                // Mem, IO Enable
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_INT_STATUS(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));

                // Set BAR
                bar0 = PADDR(PCIE0_IO_BASE | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_IO));
                bar2 = PADDR(PCIE0_MEM_BASE | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_MEM) | BIT_CTRL_DW_PCIE_BAR0_TYPE(DW_PCIE_BAR_TYPE_64BIT_ADDRESS));
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR0, bar0);
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR2, bar2);

                link_speed = (HalPCIeRCConfigRead32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS) >> BIT_SHIFT_DW_PCIE_PCIE_CAP_LINK_SPEED ) & BIT_MASK_DW_PCIE_PCIE_CAP_LINK_SPEED;
#ifdef CONFIG_98FH_MP_ENABLE
                if((link_speed == 2) && (HalPCIeEPConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_ID_VENDOR_ID) != 0xffffffff)) {
                        printf("{P015}\n");
                        REG(0xf4320738)++;
                } else {
                        printf("{F015}\n");
                        REG(0xf4320734)++;
                        printf("Device:Vender ID=%x\n" ,HalPCIeEPConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_ID_VENDOR_ID));
                        printf("Link speed: Gen%d\n", link_speed);
                }
#else 
                printf("Link speed: Gen%d\n", link_speed);
#endif
	}
	
    if(link_speed==1)
        return PCIE_LINK_GEN1;
    else if(link_speed==2)
        return PCIE_LINK_GEN2;
    else
           return PCIE_LINK_FAIL;
}



