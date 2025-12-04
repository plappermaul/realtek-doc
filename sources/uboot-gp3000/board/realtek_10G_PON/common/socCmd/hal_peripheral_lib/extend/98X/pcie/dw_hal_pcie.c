/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


//#include "dw_pcie_base.h"
#include "peripheral.h"

#include "dw_pcie_extend.h"
#include "dw_hal_pcie.h"



/* Global variable */
HAL_PCIE_OP HalPCIeOp;

void
HalPCIeOpInit(void)
{
    PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;

	pHalPCIeOp->HalPCIeRCInit = 				PCIeRCInit;
	pHalPCIeOp->HalPCIeWarmReset = 				PCIeWarmReset;
#if 1
	pHalPCIeOp->HalPCIeHotReset = 				PCIeHotResetSBR;
#else
	pHalPCIeOp->HalPCIeHotReset = 				PCIeHotResetApp;
#endif
    pHalPCIeOp->HalPCIeEPInit = 				PCIeEPInitDWCommon;
	pHalPCIeOp->HalPCIeEPTx = 					PCIeEPTxDWCommon;
	pHalPCIeOp->HalPCIeEPRx = 					PCIeEPRxDWCommon;
	pHalPCIeOp->HalPCIeCheckLink = 				PCIeCheckLinkDWCommon;
	pHalPCIeOp->HalPCIeISRVerification =		PCIeISRVerification;
	pHalPCIeOp->HalPCIePmeToMsg =				PCIePmeToMsg;
	pHalPCIeOp->HalPCIeHWCacheCoherent =		PCIeHWCacheCoherent;

	return;
}


int
HalPCIeRCInit(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	
	return pHalPCIeOp->HalPCIeRCInit((void *)&HalPCIeAdapter);
}

void
HalPCIeWarmReset(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	
	pHalPCIeOp->HalPCIeWarmReset((void *)&HalPCIeAdapter);

	return;
}


void
HalPCIeHotReset(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	
	pHalPCIeOp->HalPCIeHotReset((void *)&HalPCIeAdapter);

	return;
}


void
HalPCIeEPInit(IN u32 PCIeIdx, IN u32 loopbackEn, IN u32 usedMem)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	HalPCIeAdapter.loopbackEn = loopbackEn;
	HalPCIeAdapter.usedMem = usedMem;
	
	pHalPCIeOp->HalPCIeEPInit((void *)&HalPCIeAdapter);

	return;
}


void
HalPCIeEPTx(IN u32 PCIeIdx, IN u32 length)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	HalPCIeAdapter.length = length;
	
	pHalPCIeOp->HalPCIeEPTx((void *)&HalPCIeAdapter);

	return;
}


int
HalPCIeEPRx(IN u32 PCIeIdx, IN u32 length)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	int result;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	HalPCIeAdapter.length = length;
	
	result = pHalPCIeOp->HalPCIeEPRx((void *)&HalPCIeAdapter);

	return result;
}


int 
HalPCIeCheckLink(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;
	
	HalPCIeAdapter.Idx = PCIeIdx;
	
	return pHalPCIeOp->HalPCIeCheckLink((void *)&HalPCIeAdapter);
}

void
HalPCIeISRVerification(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;

	HalPCIeAdapter.Idx = PCIeIdx;
	
	pHalPCIeOp->HalPCIeISRVerification((void *)&HalPCIeAdapter);

	return;
}

void
HalPCIePmeToMsg(IN u32 PCIeIdx)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;

	HalPCIeAdapter.Idx = PCIeIdx;
	
	pHalPCIeOp->HalPCIePmeToMsg((void *)&HalPCIeAdapter);

	return;
}

void
HalPCIeHWCacheCoherent(IN u32 PCIeIdx, IN u32 loopCnt, IN u8 hwCCEn)
{
	PHAL_PCIE_OP pHalPCIeOp = &HalPCIeOp;
	HAL_PCIE_ADAPTER HalPCIeAdapter;

	HalPCIeAdapter.Idx = PCIeIdx;
	HalPCIeAdapter.loopCnt = loopCnt;
	HalPCIeAdapter.hwCCEn = hwCCEn;
	
	
	pHalPCIeOp->HalPCIeHWCacheCoherent((void *)&HalPCIeAdapter);

	return;
}


