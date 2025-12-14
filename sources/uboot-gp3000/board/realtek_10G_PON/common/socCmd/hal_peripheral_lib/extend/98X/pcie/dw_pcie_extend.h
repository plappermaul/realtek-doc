/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_PCIE_EXTEND_H_
#define _DW_PCIE_EXTEND_H_

enum _PCIE_DATA_PATH_{
	// For non-cache coherent
	PCIE_DP_HSIO_MEMS0_DDR = 0,
	PCIE_DP_HSIO_MEMS0_SRAM = 1,
	PCIE_DP_HSIO_MEMS0_DMEM = 2,
	PCIE_DP_HSIO_MEMS0_XRAM = 3,

	// For cache coherent
	PCIE_DP_HSIO_MEMS0_CCIS0_DDR = 4,
	PCIE_DP_HSIO_MEMS0_CCIS1_DDR = 5,	
	PCIE_DP_HSIO_CCIS2_DDR = 6,
	PCIE_DP_HSIO_MEMS0_CCIS0_SRAM = 7,
	PCIE_DP_HSIO_MEMS0_CCIS1_SRAM = 8,
	PCIE_DP_HSIO_CCIS2_SRAM = 9,

	PCIE_DP_END = 10
};

int PCIeRCInit(IN void *Data);
void PCIeWarmReset(IN VOID *Data);
void PCIeHotResetSBR(IN void *Data);
void PCIeHotResetApp(IN void *Data);

void PCIeISRVerification(IN void *Data);
void PCIePmeToMsg(IN void *Data);
void PCIeHWCacheCoherent(IN void *Data);

#endif //_PCIE_EXTEND_H_

