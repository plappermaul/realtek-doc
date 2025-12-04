/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_HAL_PCIE_H_
#define _DW_HAL_PCIE_H_

#define PADDR(addr)  ((addr) & 0xFFFFFFFF)

// Used Memory: DDR (0x0000_0000 - 0x1000_0000)
#define PCIE0_TX_DESC_ADDR_DDR	0x00100000
#define PCIE0_RX_DESC_ADDR_DDR	0x00100400

#define PCIE0_TX_BUFF_ADDR_DDR	0x00100800
#define PCIE0_RX_BUFF_ADDR_DDR	0x00101800

#define PCIE1_TX_DESC_ADDR_DDR	0x00103000
#define PCIE1_RX_DESC_ADDR_DDR	0x00103400

#define PCIE1_TX_BUFF_ADDR_DDR	0x00103800
#define PCIE1_RX_BUFF_ADDR_DDR	0x00104800


// Used Memory: SRAM (0xC000_0000 - 0xC002_0000)
#define PCIE0_TX_DESC_ADDR_SRAM	0xC0008000
#define PCIE0_RX_DESC_ADDR_SRAM	0xC0008400

#define PCIE0_TX_BUFF_ADDR_SRAM	0xC0008800
#define PCIE0_RX_BUFF_ADDR_SRAM	0xC0009800

#define PCIE1_TX_DESC_ADDR_SRAM	0xC000b000
#define PCIE1_RX_DESC_ADDR_SRAM	0xC000b400

#define PCIE1_TX_BUFF_ADDR_SRAM	0xC000b800
#define PCIE1_RX_BUFF_ADDR_SRAM	0xC000c800


// Used Memory: DMEM in Taroko CPU (0xF600_0000 - 0xF601_0000)
#define PCIE0_TX_DESC_ADDR_DMEM	0xF6001000
#define PCIE0_RX_DESC_ADDR_DMEM	0xF6001400

#define PCIE0_TX_BUFF_ADDR_DMEM	0xF6001800
#define PCIE0_RX_BUFF_ADDR_DMEM	0xF6002800

#define PCIE1_TX_DESC_ADDR_DMEM	0xF6004000
#define PCIE1_RX_DESC_ADDR_DMEM	0xF6004400

#define PCIE1_TX_BUFF_ADDR_DMEM	0xF6004800
#define PCIE1_RX_BUFF_ADDR_DMEM	0xF6005800

// Used Memory: XRAM  (0xF450_0000 - 0xF450_2000)
#define PCIE0_TX_DESC_ADDR_XRAM	0x00100000
#define PCIE0_RX_DESC_ADDR_XRAM	0x00100400

#define PCIE0_TX_BUFF_ADDR_XRAM	0xF4500000
#define PCIE0_RX_BUFF_ADDR_XRAM	0xF4501000

#define PCIE1_TX_DESC_ADDR_XRAM	0x00100800
#define PCIE1_RX_DESC_ADDR_XRAM	0x00100c00

#define PCIE1_TX_BUFF_ADDR_XRAM	0xF4500000
#define PCIE1_RX_BUFF_ADDR_XRAM	0xF4501000


typedef struct _HAL_PCIE_ADAPTER_ {
	u32	Idx;
	u32 loopbackEn;
	u32 length;
	u32 mdioRegAddr;
	u16 mdioData;
	u32 loopCnt;
	u32 usedMem;
	u8	hwCCEn;
}HAL_PCIE_ADAPTER, *PHAL_PCIE_ADAPTER;


typedef struct _HAL_PCIE_OP_ {
	int (*HalPCIeRCInit)(void *Data);
	void (*HalPCIeWarmReset)(void *Data);
	void (*HalPCIeHotReset)(void *Data);
	void (*HalPCIeEPInit)(void *Data);
	void (*HalPCIeEPTx)(void *Data);
	int	 (*HalPCIeEPRx)(void *Data);
	int (*HalPCIeCheckLink)(void *Data);
	void (*HalPCIeISRVerification)(void *Data);
	void (*HalPCIePmeToMsg)(void *Data);
	void (*HalPCIeHWCacheCoherent)(void *Data);
}HAL_PCIE_OP, *PHAL_PCIE_OP;


/* *****************************************************************
*							Macro
*******************************************************************/
#define HAL_PCIE_RC_CONFIG_WRITE32(PCIeIdx, PCIeReg, PCIeVal)			HalPCIeRCConfigWrite32(PCIeIdx, PCIeReg, PCIeVal)
#define HAL_PCIE_RC_CONFIG_READ32(PCIeIdx, PCIeReg)						HalPCIeRCConfigRead32(PCIeIdx, PCIeReg)
#define HAL_PCIE_RC_EXTEND_CONFIG_WRITE32(PCIeIdx, PCIeReg, PCIeVal)	HalPCIeRCExtendConfigWrite32(PCIeIdx, PCIeReg, PCIeVal)
#define HAL_PCIE_RC_EXTEND_CONFIG_READ32(PCIeIdx, PCIeReg)				HalPCIeRCExtendConfigRead32(PCIeIdx, PCIeReg)
#define HAL_PCIE_EP_CONFIG_WRITE32(PCIeIdx, PCIeReg, PCIeVal)			HalPCIeEPConfigWrite32(PCIeIdx, PCIeReg, PCIeVal)
#define HAL_PCIE_EP_CONFIG_READ32(PCIeIdx, PCIeReg)						HalPCIeEPConfigRead32(PCIeIdx, PCIeReg)


/* *****************************************************************
*							Function prototype
*******************************************************************/
void HalPCIeOpInit(void);

int HalPCIeRCInit(IN u32 PCIeIdx);
void HalPCIeWarmReset(IN u32 PCIeIdx);
void HalPCIeHotReset(IN u32 PCIeIdx);
void HalPCIeEPInit(IN u32 PCIeIdx, IN u32 loopbackEn, IN u32 usedMem);
void HalPCIeEPTx(IN u32 PCIeIdx, IN u32 length);
int	 HalPCIeEPRx(IN u32 PCIeIdx, IN u32 length);
int HalPCIeCheckLink(IN u32 PCIeIdx);
void HalPCIeISRVerification(IN u32 PCIeIdx);
void HalPCIePmeToMsg(IN u32 PCIeIdx);
void HalPCIeHWCacheCoherent(IN u32 PCIeIdx, IN u32 loopCnt, IN u8 hwCCEn);

#endif // _DW_HAL_PCIE_H_

