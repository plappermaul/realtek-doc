/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_PCIE_BASE_REG_H_
#define _DW_PCIE_BASE_REG_H_

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */

/* Type 0 Configuration Space */
#define REG_DW_PCIE_DEVICE_ID_VENDOR_ID										0x00
#define REG_DW_PCIE_STATUS_COMMAND											0x04
#define REG_DW_PCIE_CLASS_CODE_REVISION_ID									0x08
#define REG_DW_PCIE_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE				0x0c
#define REG_DW_PCIE_BAR0													0x10
#define REG_DW_PCIE_BAR1													0x14
#define REG_DW_PCIE_BAR2													0x18
#define REG_DW_PCIE_BAR3													0x1c
#define REG_DW_PCIE_BAR4													0x20
#define REG_DW_PCIE_BAR5													0x24
#define REG_DW_PCIE_CARDBUS_CIS_PTR											0x28
#define REG_DW_PCIE_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID						0x2c
#define REG_DW_PCIE_EXP_ROM_BASE_ADDR										0x30
#define REG_DW_PCIE_PCI_CAP_PTR												0x34
#define REG_DW_PCIE_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE		0x3c


/* Type 1 Configuration Space */
#define REG_DW_PCIE_TYPE1_DEV_ID_VEND_ID						0x00
#define REG_DW_PCIE_TYPE1_STATUS_COMMAND						0x04
#define REG_DW_PCIE_TYPE1_CLASS_CODE_REV_ID						0x08
#define REG_DW_PCIE_TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE		0x0c

#define REG_DW_PCIE_SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS		0x18
#define REG_DW_PCIE_SEC_STAT_IO_LIMIT_IO_BASE					0x1c
#define REG_DW_PCIE_MEM_LIMIT_MEM_BASE							0x20
#define REG_DW_PCIE_PREF_MEM_LIMIT_PREF_MEM_BASE				0x24
#define REG_DW_PCIE_PREF_BASE_UPPER								0x28
#define REG_DW_PCIE_PREF_LIMIT_UPPER							0x2c
#define REG_DW_PCIE_IO_LIMIT_UPPER_IO_BASE_UPPER				0x30
#define REG_DW_PCIE_TYPE1_CAP_PTR								0x34
#define REG_DW_PCIE_TYPE1_EXP_ROM_BASE							0x38
#define REG_DW_PCIE_BRIDGE_CTRL_INT_PIN_INT_LINE				0x3c


#define REG_DW_PCIE_CON_STATUS									0x44
#define REG_DW_PCIE_DEVICE_CONTROL_DEVICE_STATUS				0x78
#define REG_DW_PCIE_LINK_CONTROL_LINK_STATUS					0x80

/* PCIE Extended Configuration space */
#define REG_DW_PCIE_ACK_F_ASPM_CTRL			0x70c
#define REG_DW_PCIE_PL_DEBUG0				0x728


/* PCIE Extended Register */
//2 PCIE0_EXTENDED_REG_BASE, PCIE1_EXTENDED_REG_BASE
#define REG_DW_PCIE_EXTENDED_REG_MDIO				0x00
#define REG_DW_PCIE_EXTENDED_REG_INTSTR				0x04
#define REG_DW_PCIE_EXTENDED_REG_PWRCR				0x08
#define REG_DW_PCIE_EXTENDED_REG_IPCFG				0x0c
#define REG_DW_PCIE_EXTENDED_REG_BISTFAIL			0x10

#endif //_PCIE_BASE_REG_H_

