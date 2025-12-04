/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "common.h"
#include "peripheral.h"
#include <asm/io.h>
#include "atu.h"
#include "../../../base/pcie/dw_4_60a/dw_pcie_base_test.h"
#include "dw_pcie_extend.h"


static void
PCIePHYRL6386Set()
{
        // PHY parameter for Gen 1
        HalPCIePHYParameterWrite(0x01, 0xa812);
        HalPCIePHYParameterWrite(0x02, 0x6042);
        HalPCIePHYParameterWrite(0x04, 0x5000);
        HalPCIePHYParameterWrite(0x05, 0x23da);
        HalPCIePHYParameterWrite(0x06, 0x001f);
        HalPCIePHYParameterWrite(0x07, 0x2e40);
        HalPCIePHYParameterWrite(0x0a, 0x9670);
        HalPCIePHYParameterWrite(0x0c, 0xc008);
        HalPCIePHYParameterWrite(0x0d, 0xef1a);
        HalPCIePHYParameterWrite(0x0e, 0x1000);
        HalPCIePHYParameterWrite(0x1b, 0xaea1);
        HalPCIePHYParameterWrite(0x1e, 0x24eb);
        HalPCIePHYParameterWrite(0x20, 0x94aa);
        HalPCIePHYParameterWrite(0x21, 0x66aa);
        HalPCIePHYParameterWrite(0x22, 0x0053);
        HalPCIePHYParameterWrite(0x23, 0xaa26);
        HalPCIePHYParameterWrite(0x2a, 0x1040);
        HalPCIePHYParameterWrite(0x28, 0xf802);
        HalPCIePHYParameterWrite(0x29, 0xff04);
        HalPCIePHYParameterWrite(0x2f, 0x65bd);
        HalPCIePHYParameterWrite(0x0b, 0x7905);
        HalPCIePHYParameterWrite(0x09, 0x520c);
        HalPCIePHYParameterWrite(0x09, 0x500c);
        HalPCIePHYParameterWrite(0x09, 0x520c);

        // PHY parameter for Gen 2
        HalPCIePHYParameterWrite(0x41, 0xa819);
        HalPCIePHYParameterWrite(0x42, 0x6042);
        HalPCIePHYParameterWrite(0x44, 0x5000);
        HalPCIePHYParameterWrite(0x45, 0x23da);
        HalPCIePHYParameterWrite(0x46, 0x001f);
        HalPCIePHYParameterWrite(0x47, 0x2e40);
        HalPCIePHYParameterWrite(0x4a, 0x9670);
        HalPCIePHYParameterWrite(0x4c, 0xc008);
        HalPCIePHYParameterWrite(0x4d, 0xef1a);
        HalPCIePHYParameterWrite(0x4e, 0x1000);
        HalPCIePHYParameterWrite(0x4f, 0x0009);
        HalPCIePHYParameterWrite(0x5b, 0xaea1);
        HalPCIePHYParameterWrite(0x5e, 0x24eb);
        HalPCIePHYParameterWrite(0x60, 0x9488);
        HalPCIePHYParameterWrite(0x61, 0x8877);
        HalPCIePHYParameterWrite(0x62, 0x0053);
        HalPCIePHYParameterWrite(0x63, 0xaa26);
        HalPCIePHYParameterWrite(0x68, 0xf802);
        HalPCIePHYParameterWrite(0x69, 0xff04);
        HalPCIePHYParameterWrite(0x6a, 0x1040);
        HalPCIePHYParameterWrite(0x6f, 0x65bd);
        HalPCIePHYParameterWrite(0x4b, 0x7905);
        HalPCIePHYParameterWrite(0x49, 0x520c);
        HalPCIePHYParameterWrite(0x49, 0x500c);
        HalPCIePHYParameterWrite(0x49, 0x520c);

        return;
}

int 
PCIeRCInit(IN VOID *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 value32, PCIeIdx = pHalPCIeAdapter->Idx;
    int link_result;

        /* Note: this initial flow must be in secure world! */
#if 0
	// HV_DDR_CTRL_DDR_SCH_CTRL
        //fpga mode
	HAL_WRITE32_MEM(0xf4328864, 0x01f74020);

	// HV_GLOBAL_MASTER_OVERRIDE_SECURITY
	HAL_WRITE32_MEM(0xf4320100, 0x00000009);

	// HV_GLOBAL_PIN_MUX
	HAL_WRITE32_MEM(0xf4320038, 0x80000000);

	// HV_GLOBAL_SECURITY_OVERRIDE
	HAL_WRITE32_MEM(0xf43200fc, 0x0029853c);
#endif
	// HV_GLOBAL_PHY_CONTROL
	//HAL_WRITE32_MEM(0xf4320020, 0x00000a80);
	value32 = HAL_READ32_MEM(0xf4320020);
        value32 = (value32 & ~ BIT(8)) | BIT(7) | BIT(9);
        HAL_WRITE32_MEM(0xf4320020, value32);

	// HV_GLOBAL_DPHY_RESET
	HAL_WRITE32_MEM(0xf4320010, 0x00000002);
	HAL_WRITE32_MEM(0xf4320010, 0x00000000);

	// HV_GLOBAL_BLOCK_RESET_SECURE
	//HAL_WRITE32_MEM(0xf432000c, 0x00002080);
	//HAL_WRITE32_MEM(0xf432000c, 0x00002000);
	value32 = HAL_READ32_MEM(0xf432000c);
        value32 = value32 | BIT(7);
        HAL_WRITE32_MEM(0xf432000c, value32);

        value32 = value32 & ~ BIT(7);
        HAL_WRITE32_MEM(0xf432000c, value32);

	// HV_GLOBAL_ENABLE_IP_1_SECURE
	//HAL_WRITE32_MEM(0xf43203ac, 0xf801e67c);
	value32 = HAL_READ32_MEM(0xf43203ac);
        value32 = value32 | BIT(9);
        HAL_WRITE32_MEM(0xf43203ac, value32);

        mdelay(10);

#ifndef CONFIG_RTL8198F_FPGA   // For ASIC
        PCIePHYRL6386Set();
#endif

#ifdef CONFIG_RTL8198F_FPGA   // For FPGA
	// device reset output gpio
	HAL_WRITE32_MEM(0xf4329280, 0xFFFFFFFD);

	// device assert
	HAL_WRITE32_MEM(0xf4329284, 0x00000000);

	// device deassert
	HAL_WRITE32_MEM(0xf4329284, 0x00000002);
#else   // For ASIC
	// pcie rst, PERST
	//HAL_WRITE32_MEM(0xf4320014, 0x400);
	value32 = HAL_READ32_MEM(0xf4320014);
        value32 = value32 | BIT(10);
        HAL_WRITE32_MEM(0xf4320014, value32);

	// pcie rst, PERST
	//HAL_WRITE32_MEM(0xf4320014, 0x000);
	value32 = value32 & ~ BIT(10);
        HAL_WRITE32_MEM(0xf4320014, value32);
#endif

        mdelay(1);

	/*=================================
	 Set up RC
	=================================*/
	HAL_WRITE32_MEM(0xf4326040, 0xf1000000);
	HAL_WRITE32_MEM(0xf4326044, 0xf1000FFF);
	
	//HAL_WRITE32_MEM(0xf1000710, 0x00010120);	//link in gen 1 only
	HAL_WRITE32_MEM(0xf1000710, 0x00030120);

	// HV_PCIE_X1_GLBL_INTERRUPT_ENABLE_0
	HAL_WRITE32_MEM(0xf4326004, 0x00000101);	// For A17
	HAL_WRITE32_MEM(0xf432600c, 0x00000101);	// For taroko
		
	//HAL_WRITE32_MEM(0xf100080C, 0x00020180);	// link in gen 1 only
	HAL_WRITE32_MEM(0xf100080C, 0x0003020F);

	HAL_WRITE32_MEM(0xf100003C, 0x000001FF);
	HAL_WRITE32_MEM(0xf1000020, 0xF200F100);
	HAL_WRITE32_MEM(0xf1000004, 0x00100107);

	
	/*=================================
	 PCIe0 enable
	=================================*/
	// Core Config, ltssm enable
	HAL_WRITE32_MEM(0xf4326018 , 0x00000021);
	
	
	/*=================================
	 EP Configuration Space ATU (0xf100_1000/0x1000)
	=================================*/
	// dw_pcie_prog_outbound_atu
	HAL_WRITE32_MEM(0xf4326120, 0x0);
	HAL_WRITE32_MEM(0xf100090c, PCIE0_CFG0_REG_BASE);
	HAL_WRITE32_MEM(0xf1000910, 0x0);
	HAL_WRITE32_MEM(0xf1000914, (PCIE0_CFG0_REG_BASE + 0x1000 - 0x1));
	HAL_WRITE32_MEM(0xf1000918, 0x0);
	HAL_WRITE32_MEM(0xf100091c, 0x0);
	HAL_WRITE32_MEM(0xf1000904, 0x4);
	HAL_WRITE32_MEM(0xf1000908, 0x80000000);
	
	/*=================================
	 EP I/O Space ATU (0xf101_0000/0x1_0000)
	=================================*/
	// dw_pcie_prog_outbound_atu
	HAL_WRITE32_MEM(0xf4326120, 0x2);
	HAL_WRITE32_MEM(0xf100090c, PCIE0_IO_BASE);
	HAL_WRITE32_MEM(0xf1000910, 0x0);
	HAL_WRITE32_MEM(0xf1000914, (PCIE0_IO_BASE + 0x10000 - 0x1));
	HAL_WRITE32_MEM(0xf1000918, PCIE0_IO_BASE);
	HAL_WRITE32_MEM(0xf100091c, 0x0);
	HAL_WRITE32_MEM(0xf1000904, 0x2);
	HAL_WRITE32_MEM(0xf1000908, 0x80000000);
	
	/*=================================
	 EP Memory Space ATU (0xf102_0000/0xfe_0000)
	=================================*/
	// dw_pcie_prog_outbound_atu
	HAL_WRITE32_MEM(0xf4326120, 0x1);
	HAL_WRITE32_MEM(0xf100090c, PCIE0_MEM_BASE);
	HAL_WRITE32_MEM(0xf1000910, 0x0);
	HAL_WRITE32_MEM(0xf1000914, 0xf1ffffff);
	HAL_WRITE32_MEM(0xf1000918, PCIE0_MEM_BASE);
	HAL_WRITE32_MEM(0xf100091c, 0x0);
	HAL_WRITE32_MEM(0xf1000904, 0x0);
	HAL_WRITE32_MEM(0xf1000908, 0x80000000);

	// Check link
	link_result = HalPCIeCheckLink(PCIeIdx);

	return link_result;
}

void
PCIePendingRequestCheck(void)
{
	u32	value32;
	
	/* Check if AXI bridge master have any pending request */
	while( (HAL_READ32_MEM(0xf4326118) >> 30 ) != 0)
	{
		printf("AXI master port have pending request!\n");
		value32 = HAL_READ32_MEM(0xf4326018);
		value32|= (1<<9);
		HAL_WRITE32_MEM(0xf4326018, value32);
		mdelay(1);
	}

	/* Check if AXI bridge slave have any pending request */
	while( (HAL_READ32_MEM(0xf432611c) & 0x3) != 0)
	{
		printf("AXI slave port have pending request!\n");
		mdelay(1);
	}

	return;
}

void 
PCIeWarmReset(IN VOID *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32	value32;

	// Endpoint, set to D3Hot
	value32 = HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, REG_DW_PCIE_CON_STATUS);
	value32 = value32 & (~ BIT_CTRL_DW_PCIE_POWER_STATE(BIT_MASK_DW_PCIE_POWER_STATE));
	value32 = value32 | BIT_CTRL_DW_PCIE_POWER_STATE(DW_PCIE_POWER_STATE_D3HOT);
	HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, REG_DW_PCIE_CON_STATUS, value32);

	// Pending Request check
	PCIePendingRequestCheck();

	mdelay(10);
	
	// RC broadcast PME Turn off message
    PCIePmeToMsg(NULL);

	mdelay(500);

	PCIeRCInit(Data);

}


void 
PCIeHotResetSBR(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32 value32;
	
	value32 = HalPCIeRCConfigRead32(PCIeIdx, REG_DW_PCIE_BRIDGE_CTRL_INT_PIN_INT_LINE);
	value32 = value32 | BIT_DW_PCIE_BRIDGE_CTRL_SBR;
	HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_BRIDGE_CTRL_INT_PIN_INT_LINE, value32);

	mdelay(1);

	// Pending Request check
	PCIePendingRequestCheck();

	value32 = value32 & (~BIT_DW_PCIE_BRIDGE_CTRL_SBR);
	HalPCIeRCConfigWrite32(PCIeIdx, REG_DW_PCIE_BRIDGE_CTRL_INT_PIN_INT_LINE, value32);

	PCIeCheckLinkDWCommon(Data);
	
	return;
}


void 
PCIeHotResetApp(IN void *Data)
{	
	HAL_WRITE32_MEM(0xf4326018, 0x61);

	mdelay(1);

	// Pending Request check
	PCIePendingRequestCheck();

	HAL_WRITE32_MEM(0xf4326018, 0x21);


	PCIeCheckLinkDWCommon(Data);
	
	return;
}

static void
PCIeIrqHandler98F(IN VOID *Data)
{
	u32 PCIeIdx = PCIE_MODULE_SEL_0;
	u32	value32;
	
	printf("CPU calls PCIE IRQ handler!\n");

	// Endpoint, check interrupt status
	value32 = HalPCIeEPMemRead32(PCIeIdx, IntrMask);
	if((value32 & (1<<24)) == _FALSE)
	{
		printf("Device doesn't send interrupt message, something wrong!\n");
		return;
	}

	// Endpoint, clear interrupt status (write 1 to clear)
	value32 |= (1<<24);
	HalPCIeEPMemWrite32(PCIeIdx, IntrMask, value32);

	while(_TRUE)
	{		
		// Endpoint, check interrupt status
		if((HalPCIeEPMemRead32(PCIeIdx, IntrMask) & (1<<24)) == _FALSE)
		{
			break;
		}
	}

	// clear interrupt for A17!
	HAL_WRITE32_MEM(0xf4326000, 0x00000003);	// G3: 0x00000100

	// clear interrupt for Taroko!
	HAL_WRITE32_MEM(0xf4326008, 0x00000003);	// G3: 0x00000100
	return;
	
}



void 
PCIeISRVerification(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u32	value32;
	IRQ_HANDLE	PCIeIrqHandle;
	IRQ_FUN     IrqFun;
    IRQn_Type	IrqNum;
	
	switch(PCIeIdx)
	{
		case PCIE_MODULE_SEL_0:
			IrqFun = (IRQ_FUN)PCIeIrqHandler98F;
			IrqNum = g3_pcie_1_core_top;
			break;
		default:
			printf("This PCIeIdx(%d) is not supported in 98F.\n", PCIeIdx);
			return;
	}

	// Register IRQ
    PCIeIrqHandle.Data = (u32)NULL;
    PCIeIrqHandle.IrqNum = IrqNum;
    PCIeIrqHandle.IrqFun = IrqFun;
    PCIeIrqHandle.Priority = 0;
    InterruptRegister(&PCIeIrqHandle);

	// Endpoint, enable interrupt
	value32 = HalPCIeEPMemRead32(PCIeIdx, IntrMask);
	value32 |= (1<<8);
	HalPCIeEPMemWrite32(PCIeIdx, IntrMask, value32);

	printf("Endpoint starts to send interrupt message!\n");
	// Endpoint, kick interrupt message
	value32 = HalPCIeEPMemRead32(PCIeIdx, TxPoll);
	value32 |= (1<<0);
	HalPCIeEPMemWrite32(PCIeIdx, TxPoll, value32);
	
	return;

}


void 
PCIePmeToMsg(IN void *Data)
{
	u32 value32;
	
	// RC broadcast PME Turn off message
	value32 = HAL_READ32_MEM(0xf4326088);
	value32 = value32 | BIT(11);
	HAL_WRITE32_MEM(0xf4326088, value32);
	
	return;
}

void 
PCIeHWCacheCoherent(IN void *Data)
{
	PHAL_PCIE_ADAPTER pHalPCIeAdapter = (PHAL_PCIE_ADAPTER)Data;
	u32 PCIeIdx = pHalPCIeAdapter->Idx;
	u16 loopCnt = pHalPCIeAdapter->loopCnt;
	u8 	hwCCEn 	= pHalPCIeAdapter->hwCCEn;
	u32	dataPath = 0, usedMem = 0;

	/* CCI-400 + global reg config */
	HAL_WRITE32_MEM(0xf7195000, 0xc0000003);
	HAL_WRITE32_MEM(0xf7190008, 0x00000001);
	//HAL_WRITE32_MEM(0xf43200fc, 0xfffffff7);
	//HAL_WRITE32_MEM(0xf43200ac, 0xffffffff);
	HAL_WRITE32_MEM(0xf432005c, 0x2); //enable ACE

	HAL_WRITE32_MEM(0xf7195000, 0xc0000003);
	HAL_WRITE32_MEM(0xf7190008, 0x00000001);

	HAL_WRITE32_MEM(0xf7191000, 0x40000001);
	HAL_WRITE32_MEM(0xf7192000, 0x40000001);
	HAL_WRITE32_MEM(0xf7194000, 0x40000001);

	HAL_WRITE32_MEM(0xf719a000, 0x00);
	HAL_WRITE32_MEM(0xf719b000, 0x02);
	HAL_WRITE32_MEM(0xf719c000, 0x0A);
	HAL_WRITE32_MEM(0xf719d000, 0x03);
					
	
	for(dataPath = PCIE_DP_HSIO_MEMS0_DDR; dataPath < PCIE_DP_END ; dataPath++)
	{
		switch(dataPath)
		{
			case PCIE_DP_HSIO_MEMS0_DDR:
				if(hwCCEn == _TRUE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_DDR;

#if 0
				/* Disable ATU */
				pcie1_atu_write_data0(0x0);
				pcie1_atu_write_data1(0x0);
				pcie1_atu_write_data2(0x0);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x0);
#else
				/* DMA to secure memory */
				pcie1_atu_write_data0(0x0010);
				pcie1_atu_write_data1(0xffff);
				pcie1_atu_write_data2(0x0010);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);
#endif
				break;
				
			case PCIE_DP_HSIO_MEMS0_SRAM:
				if(hwCCEn == _TRUE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_SRAM;

#if 0
				/* Disable ATU */
				pcie1_atu_write_data0(0x0);
				pcie1_atu_write_data1(0x0);
				pcie1_atu_write_data2(0x0);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x0);
#else
				/* DMA to secure memory */
				pcie1_atu_write_data0(0xc000);
				pcie1_atu_write_data1(0xfff0);
				pcie1_atu_write_data2(0xc000);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);
#endif
				break;

			case PCIE_DP_HSIO_MEMS0_DMEM:
				if(hwCCEn == _TRUE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_DMEM;

#if 0
				/* Disable ATU */
				pcie1_atu_write_data0(0x0);
				pcie1_atu_write_data1(0x0);
				pcie1_atu_write_data2(0x0);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x0);
#else
				/* DMA to secure memory */
				pcie1_atu_write_data0(0xf600);
				pcie1_atu_write_data1(0xff00);
				pcie1_atu_write_data2(0xf600);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);
#endif
				break;

			case PCIE_DP_HSIO_MEMS0_XRAM:
				if(hwCCEn == _TRUE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_XRAM;
				
				/* Disable ATU, XRAM always non-secure */
				pcie1_atu_write_data0(0x0);
				pcie1_atu_write_data1(0x0);
				pcie1_atu_write_data2(0x0);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
				pcie1_atu_write_data4(0x0);
				pcie1_atu_write_data5(0x0);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x0);
				break;

			case PCIE_DP_HSIO_MEMS0_CCIS0_DDR:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_DDR;
				
				/* Enable ATU */
				pcie1_atu_write_data0( 0x0010);
				pcie1_atu_write_data1( 0xfff0);
				pcie1_atu_write_data2(0x20010);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0xf, 0xf, 0x1, 0x1, 0x1, 0x0);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x1);

				HAL_WRITE32_MEM(0xf719a000, 0x00);
				HAL_WRITE32_MEM(0xf719b000, 0x02);
				HAL_WRITE32_MEM(0xf719c000, 0x0A);
				HAL_WRITE32_MEM(0xf719d000, 0x03);
				break;

			case PCIE_DP_HSIO_MEMS0_CCIS1_DDR:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_DDR;
				
				/* Enable ATU */
				pcie1_atu_write_data0( 0x0010);
				pcie1_atu_write_data1( 0xfff0);
				pcie1_atu_write_data2(0x40010);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0xf, 0xf, 0x1, 0x1, 0x1, 0x0);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x1);

				HAL_WRITE32_MEM(0xf719a000, 0x20);
				HAL_WRITE32_MEM(0xf719b000, 0x22);
				HAL_WRITE32_MEM(0xf719c000, 0x2A);
				HAL_WRITE32_MEM(0xf719d000, 0x23);
				break;
				
			case PCIE_DP_HSIO_CCIS2_DDR:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_DDR;
				
				/* Enable ATU */
				pcie1_atu_write_data0(  0x0010);
				pcie1_atu_write_data1(  0xfff0);
				pcie1_atu_write_data2(0x100010);
				pcie1_atu_write_data3(0x0, 0x0, 0x0, 0x0, 0xf, 0xf, 0x1, 0x1, 0x1, 0x0);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x0);
				pcie1_atu_ctl_entry_enable(0x1);

				HAL_WRITE32_MEM(0xf719a000, 0x40);
				HAL_WRITE32_MEM(0xf719b000, 0x42);
				HAL_WRITE32_MEM(0xf719c000, 0x4A);
				HAL_WRITE32_MEM(0xf719d000, 0x43);
				break;
				
			case PCIE_DP_HSIO_MEMS0_CCIS0_SRAM:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_SRAM;
				
				/* Enable ATU */
				pcie1_atu_write_data0( 0xc000);
				pcie1_atu_write_data1( 0xfff0);
				pcie1_atu_write_data2(0x2c000);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0xf, 0xf, 0x1, 0x1, 0x1, 0x1);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);

				HAL_WRITE32_MEM(0xf719a000, 0x00);
				HAL_WRITE32_MEM(0xf719b000, 0x02);
				HAL_WRITE32_MEM(0xf719c000, 0x0A);
				HAL_WRITE32_MEM(0xf719d000, 0x03);
				break;
				
			case PCIE_DP_HSIO_MEMS0_CCIS1_SRAM:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_SRAM;
				
				/* Enable ATU */
				pcie1_atu_write_data0( 0xc000);
				pcie1_atu_write_data1( 0xfff0);
				pcie1_atu_write_data2(0x4c000);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0xf, 0xf, 0x1, 0x1, 0x1, 0x1);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);

				HAL_WRITE32_MEM(0xf719a000, 0x20);
				HAL_WRITE32_MEM(0xf719b000, 0x22);
				HAL_WRITE32_MEM(0xf719c000, 0x2A);
				HAL_WRITE32_MEM(0xf719d000, 0x23);
				break;
				
			case PCIE_DP_HSIO_CCIS2_SRAM:
				if(hwCCEn == _FALSE)
				{
					continue;
				}
				usedMem = PCIE_TRX_USED_MEM_SRAM;
				
				/* Enable ATU */
				pcie1_atu_write_data0(  0xc000);
				pcie1_atu_write_data1(  0xfff0);
				pcie1_atu_write_data2(0x10c000);
				pcie1_atu_write_data3(0x0, 0x0, 0x7, 0x7, 0xf, 0xf, 0x1, 0x1, 0x1, 0x1);
				pcie1_atu_write_data4(0x80);
				pcie1_atu_write_data5(0x80);
				pcie1_atu_ctl_entry_write(0x1);
				pcie1_atu_ctl_entry_enable(0x2);

				HAL_WRITE32_MEM(0xf719a000, 0x40);
				HAL_WRITE32_MEM(0xf719b000, 0x42);
				HAL_WRITE32_MEM(0xf719c000, 0x4A);
				HAL_WRITE32_MEM(0xf719d000, 0x43);
				break;
				
			default:
				printf("Unsupported datapath!\n");
				return;
		}

		/* Enable CCI-400 event counter */
		HAL_WRITE32_MEM(0xf719a008, 0x1);
		HAL_WRITE32_MEM(0xf719b008, 0x1);
		HAL_WRITE32_MEM(0xf719c008, 0x1);
		HAL_WRITE32_MEM(0xf719d008, 0x1);
		HAL_WRITE32_MEM(0xf7190100, 0x6);
		HAL_WRITE32_MEM(0xf7190100, 0x1);
		
		printf("Data Path type %d!!!\n", dataPath);
		PCIeHostDataLoopback(PCIeIdx, loopCnt, usedMem);

		/* Check CCI-400 event counter after Tx/Rx */
		printf("0xf719a004 = %x\n",HAL_READ32_MEM(0xf719a004));
		printf("0xf719b004 = %x\n",HAL_READ32_MEM(0xf719b004));
		printf("0xf719c004 = %x\n",HAL_READ32_MEM(0xf719c004));
		printf("0xf719d004 = %x\n",HAL_READ32_MEM(0xf719d004));
	}
	
	return;
}



