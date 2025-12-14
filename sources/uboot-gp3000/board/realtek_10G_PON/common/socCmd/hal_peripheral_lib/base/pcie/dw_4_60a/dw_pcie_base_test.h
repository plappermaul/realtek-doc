/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_PCIE_BASE_TEST_H_
#define _DW_PCIE_BASE_TEST_H_

/* *****************************************************************
*							Definition 
*******************************************************************/
/* For paramter PCIe Index */
#define PCIE_VERI_PARA_PCIE_INDEX_MAX	2

#define MAC_ADDR_LEN			6

/* For Endpoint information */
#define PCIE_EP_DEVICE_VENDOR_ID_RTL8168	0x816810EC

#define PCIE_EP_DEVICE_VENDOR_ID_RTL8169	0x816910EC



/* *****************************************************************
*							Macro 
*******************************************************************/



/* *****************************************************************
*							Enumeration 
*******************************************************************/

/* For paramter item */
typedef enum _PCIE_VERI_PARA_ITEM_ {
	PCIE_TEST_ITEM_RESET			= 0,
	PCIE_TEST_ITEM_TRX_LOOPBACK		= 1,
	PCIE_TEST_ITEM_POWER_SAVING		= 2,
	PCIE_TEST_ITEM_INTERRUPT		= 3,
	PCIE_TEST_ITEM_END				= 4
}PCIE_VERI_PARA_ITEM, *PPCIE_VERI_PARA_ITEM;


/* For paramter sub-item */
	/* Initial */
typedef enum _PCIE_VERI_PARA_SUB_ITEM_INIT_ {
	PCIE_TEST_SUB_ITEM_COLD_RESET			= 0,
	PCIE_TEST_SUB_ITEM_WARM_RESET			= 1,
	PCIE_TEST_SUB_ITEM_HOT_RESET			= 2,
	PCIE_TEST_SUB_ITEM_INIT_END				= 3
}PCIE_VERI_PARA_SUB_ITEM_INIT, *PPCIE_VERI_PARA_SUB_ITEM_INIT;

	/* TRX Loopback */
typedef enum _PCIE_VERI_PARA_SUB_ITEM_TRX_LOOPBACK_ {
	PCIE_TEST_SUB_ITEM_TRX_LOOPBACK_WITHOUT_HW_CC	= 0,
	PCIE_TEST_SUB_ITEM_TRX_LOOPBACK_WITH_HW_CC		= 1,
	PCIE_TEST_SUB_ITEM_TRX_LOOPBACK_END				= 2
}PCIE_VERI_PARA_SUB_ITEM_TRX_LOOPBACK, *PPCIE_VERI_PARA_SUB_ITEM_TRX_LOOPBACK;

typedef enum _PCIE_TRX_LOOPBACK_RESULT_ {
	PCIE_TRX_LOOPBACK_FAIL 		= 0,
	PCIE_TRX_LOOPBACK_SUCCESS 	= 1
}PCIE_TRX_LOOPBACK_RESULT, *PPCIE_TRX_LOOPBACK_RESULT;


typedef enum _PCIE_TRX_USED_MEM_ {
	PCIE_TRX_USED_MEM_DDR 	= 0,
	PCIE_TRX_USED_MEM_SRAM 	= 1,
	PCIE_TRX_USED_MEM_DMEM 	= 2,
	PCIE_TRX_USED_MEM_XRAM 	= 3
}PCIE_TRX_USED_MEM;

	/* Power Saving */
typedef enum _PCIE_VERI_PARA_SUB_ITEM_POWER_SAVING_ {
	PCIE_TEST_SUB_ITEM_POWER_SAVING_D0_L0				= 0,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_D3HOT_L1			= 1,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_D3COLD_L2			= 2,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_L2_L3_HANDSHAKE		= 3,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L1_L0			= 4,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_L0_L2_L0			= 5,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S			= 6,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L1				= 7,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_ASPM_L0S_L1			= 8,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_DISABLE_ASPM		= 9,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_ENABLE_ASPM_8812B	= 10,
	PCIE_TEST_SUB_ITEM_POWER_SAVING_END					= 11 
}PCIE_VERI_PARA_SUB_ITEM_POWER_SAVING, *PPCIE_VERI_PARA_SUB_ITEM_POWER_SAVING;

	/* Interrupt */
typedef enum _PCIE_VERI_PARA_SUB_ITEM_INTERRUPT_ {
	PCIE_TEST_SUB_ITEM_INTERRUPT			= 0,
	PCIE_TEST_SUB_ITEM_INTERRUPT_END		= 1
}PCIE_VERI_PARA_SUB_ITEM_INTERRUPT, *PPCIE_VERI_PARA_SUB_ITEM_INTERRUPT;


/* For paramter PCIe Index */


/* For paramter loop */



/* *****************************************************************
*							Data structure
*******************************************************************/

typedef struct _PCIE_VERI_PARA_ {
	u32		VeriItem;
	u32		VeriSubItem;
	u32		VeriPCIeIdx;
	u32		VeriLoopCnt;
}PCIE_VERI_PARA,*PPCIE_VERI_PARA;



/* *****************************************************************
*							Function prototype
*******************************************************************/

void PCIeHostDataLoopback(IN u32 PCIeIdx, IN u32 loopCnt, IN u32 dataPath);
void PCIeTestApp(void *Data);


#endif //_PCIE_BASE_TEST_H_

