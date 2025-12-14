/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "common.h"
#include "dw_ne_base.h"
#include "peripheral.h"

//#include "dw_hal_pcie.h"

#ifndef CONFIG_CMD_RTK_PCIE
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
PCIeEPInitDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 loopbackEn = pHalPCIeAdapter->loopbackEn;
	u32 value32, Bar0, Bar2, count;	
	u32 MapIOBase, MapMemBase, TxDescAddr, RxDescAddr;
	u8 *pTxDescAddr, *pRxDescAddr;

	/* Endpoint is RTL8168 (PCIE ethernet NIC card) */
	
	if(PCIeIdx == PCIE_MODULE_SEL_0)
	{
		MapIOBase = PCIE0_IO_BASE;
		MapMemBase = PCIE0_MEM_BASE;
		TxDescAddr = PCIE0_TX_DESC_ADDR;
		RxDescAddr = PCIE0_RX_DESC_ADDR;
		pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR;
		pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR;
	}else if (PCIeIdx == PCIE_MODULE_SEL_1)
	{
		MapIOBase = PCIE1_IO_BASE;
		MapMemBase = PCIE1_MEM_BASE;
		TxDescAddr = PCIE1_TX_DESC_ADDR;
		RxDescAddr = PCIE1_RX_DESC_ADDR;
		pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR;
		pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR;
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
	HalPCIeEPMemWrite32(PCIeIdx, TxDescStartAddrLow, PADDR(TxDescAddr));
	HalPCIeEPMemWrite32(PCIeIdx, TxDescStartAddrHigh, 0);
	HalPCIeEPMemWrite32(PCIeIdx, RxDescAddrLow, PADDR(RxDescAddr));
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
	u32 RxBuffAddr, TxBuffAddr;
	u8 *pTxDescAddr, *pRxDescAddr;
	
	if(PCIeIdx == PCIE_MODULE_SEL_0)
	{
		TxBuffAddr = PCIE0_TX_BUFF_ADDR;
		RxBuffAddr = PCIE0_RX_BUFF_ADDR;
		pTxDescAddr = (u8 *)PCIE0_TX_DESC_ADDR;
		pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR;
	}else if (PCIeIdx == PCIE_MODULE_SEL_1)
	{
		TxBuffAddr = PCIE1_TX_BUFF_ADDR;
		RxBuffAddr = PCIE1_RX_BUFF_ADDR;
		pTxDescAddr = (u8 *)PCIE1_TX_DESC_ADDR;
		pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR;
	}else
	{
		printf("Error parameter!\n");
		return;
	}

	// Fill RX Descriptor
	HAL_WRITE32_MEM(pRxDescAddr + 0x08, PADDR(RxBuffAddr));	// RX Buffer Address
	HAL_WRITE32_MEM(pRxDescAddr + 0x18, PADDR(RxBuffAddr));	// RX Buffer Address
	
	// Ensure Descriptor is updated
	HAL_READ32_MEM(pRxDescAddr + 0x8);
	
	// Enable TX/RX (This seems to trigger NIC prefetching RX descriptor)
	HalPCIeEPMemWrite8(PCIeIdx, ChipCmd, 0x0c);
	
	// Fill Descriptor
	HAL_WRITE32_MEM(pTxDescAddr + 0x0, (DescOwn | FirstFrag | LastFrag | RingEnd | length));
	HAL_WRITE32_MEM(pTxDescAddr + 0x8, PADDR(TxBuffAddr));
	
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
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 length = pHalPCIeAdapter->length;
	u8  *pRxDescAddr, *pRxBuffAddr, *pTxBuffAddr;
	u32 count, loopcount = 0;

	if(PCIeIdx == PCIE_MODULE_SEL_0)
	{
		pRxDescAddr = (u8 *)PCIE0_RX_DESC_ADDR;
		pTxBuffAddr = (u8 *)PCIE0_TX_BUFF_ADDR;
		pRxBuffAddr = (u8 *)PCIE0_RX_BUFF_ADDR;
	}else if (PCIeIdx == PCIE_MODULE_SEL_1)
	{
		pRxDescAddr = (u8 *)PCIE1_RX_DESC_ADDR;
		pTxBuffAddr = (u8 *)PCIE1_TX_BUFF_ADDR;
		pRxBuffAddr = (u8 *)PCIE1_RX_BUFF_ADDR;
	}else
	{
		printf("Error parameter!\n");
		return PCIE_EP_RX_STATUS_FAIL;
	}

	
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

void 
PCIeMDIOResetDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 sysPCIePHY;

	if(PCIeIdx == 0){
		sysPCIePHY = SYS_PCIE_PHY0;
	}else if(PCIeIdx == 1){
		sysPCIePHY = SYS_PCIE_PHY1;
	}else{
		return;
	}
	
	HAL_WRITE32_SYS(sysPCIePHY, BIT_CTRL_SYS_RX50_LINK(1) | BIT_CTRL_SYS_LOAD_DONE(0) | BIT_CTRL_SYS_MDRST_N(0));	//mdio reset=0,		//0x08
	HAL_WRITE32_SYS(sysPCIePHY, BIT_CTRL_SYS_RX50_LINK(1) | BIT_CTRL_SYS_LOAD_DONE(0) | BIT_CTRL_SYS_MDRST_N(1));	//mdio reset=1,		//0x09
	HAL_WRITE32_SYS(sysPCIePHY, BIT_CTRL_SYS_RX50_LINK(1) | BIT_CTRL_SYS_LOAD_DONE(1) | BIT_CTRL_SYS_MDRST_N(1));	//bit1 load_done=1,		//0x0b

	return;
}


void 
PCIePHYResetDWCommon(IN void *Data)
{	
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;

	//bit7: PHY reset=0   bit0: Enable LTSSM=1
	HalPCIeRCExtendConfigWrite32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR, BIT_CTRL_DW_PCIE_APP_LTSSM_EN(0) | BIT_CTRL_DW_PCIE_PHY_SRST_N(0));

	mdelay(10);
	
	//bit7: PHY reset=1   bit0: Enable LTSSM=1
	HalPCIeRCExtendConfigWrite32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_PWRCR, BIT_CTRL_DW_PCIE_APP_LTSSM_EN(1) | BIT_CTRL_DW_PCIE_PHY_SRST_N(1));

	return;
}


void 
PCIeMDIOWriteDWCommon(IN u32 PCIeIdx, IN u32 mdioRegAddr, IN u32 mdioData)
{
	volatile int count;

	HalPCIeRCExtendConfigWrite32(PCIeIdx, REG_DW_PCIE_EXTENDED_REG_MDIO, BIT_CTRL_DW_PCIE_MDIO_REGADDR(mdioRegAddr) | BIT_CTRL_DW_PCIE_MDIO_DATA(mdioData) | BIT_CTRL_DW_PCIE_MDIO_RDWR(1));
		
	for(count = 0; count < 5555; count++)
	{
		// do nothing, just delay
	}

	return;
}


void 
PCIeCheckLinkDWCommon(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
        u32 link_speed, bar0, bar2;
	volatile int count = 5;

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
		printf("PCIE_P%d ->  Cannot LinkUP\r\n", PCIeIdx);
		return;
	}
	else  //already  linkup
	{
		HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_TYPE1_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));
		
		printf("Find Port=%x Device:Vender ID=%x\n", PCIeIdx, HalPCIeEPConfigRead32(PCIeIdx, REG_DW_PCIE_DEVICE_ID_VENDOR_ID) );

                // Mem, IO Enable
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_STATUS_COMMAND, BIT_CTRL_DW_PCIE_CAP_LIST(1) | BIT_CTRL_DW_PCIE_INT_STATUS(1) | BIT_CTRL_DW_PCIE_BME(1) | BIT_CTRL_DW_PCIE_MSE(1) | BIT_CTRL_DW_PCIE_IO_EN(1));

                // Set BAR
                bar0 = PADDR(PCIE0_IO_BASE | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_IO));
                bar2 = PADDR(PCIE0_MEM_BASE | BIT_CTRL_DW_PCIE_BAR0_MEM_IO(DW_PCIE_BAR_MEM_IO_MEM) | BIT_CTRL_DW_PCIE_BAR0_TYPE(DW_PCIE_BAR_TYPE_64BIT_ADDRESS));
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR0, bar0);
                HalPCIeEPConfigWrite32(PCIeIdx, REG_DW_PCIE_BAR2, bar2);

                link_speed = (HalPCIeRCConfigRead32(PCIeIdx, REG_DW_PCIE_LINK_CONTROL_LINK_STATUS) >> BIT_SHIFT_DW_PCIE_PCIE_CAP_LINK_SPEED ) & BIT_MASK_DW_PCIE_PCIE_CAP_LINK_SPEED;
                printf("Link speed: Gen%d\n", link_speed);
	}
	
	return;
}


void 
PCIeMDIOPHYParameterSetRLE0269B(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;

	PCIeMDIOWriteDWCommon(PCIeIdx, 0x00, 0x1045);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x02, 0x4d16);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x06, 0xf018);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x08, 0x18d2);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x09, 0x930c);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0a, 0x03c9);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0d, 0x1566);

	return;
}


void 
PCIeMDIOPHYParameterSetRLE0269C(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0d, 0x1566);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x06, 0xe440);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0a, 0x119b);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0b, 0x051d);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x00, 0x1043);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x01, 0x0003);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x02, 0x26f6);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x03, 0x0001);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0b, 0x0711);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x19, 0x6c00);

	return;
}


void 
PCIeMDIOPHYParameterSetRLE0600(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x00, 0x5083);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x04, 0xf048);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x06, 0x19e0);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x19, 0x6c69);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x1d, 0x0000);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x01, 0x0000);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x08, 0x9cc3);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x09, 0x4380);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x03, 0x7b44);

	return;
}

void 
PCIeMDIOPHYParameterSetRLE0591(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x00, 0x0031);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x06, 0x18c1);

#if 0
	/* 40MHz CLK */
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x0f, 0x12f6);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x00, 0x0031);
	PCIeMDIOWriteDWCommon(PCIeIdx, 0x06, 0x1ac1);	
#endif

	return;
}

#endif //endif ndef CONFIG_CMD_RTK_PCIE

void
HalNeEPMemWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal)
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
HalNeEPMemRead32(IN u32 NeIdx, IN u32 NeReg)
{
	u32 PCIeDatRd = 0;

	switch(NeIdx)
	{
		case PCIE_MODULE_SEL_0:
			PCIeDatRd = HAL_READ32(PCIE0_MEM_BASE, NeReg);
			break;

		case PCIE_MODULE_SEL_1:
			PCIeDatRd = HAL_READ32(PCIE1_MEM_BASE, NeReg);
			break;

		default:
			break;
	}

	return PCIeDatRd;
}
