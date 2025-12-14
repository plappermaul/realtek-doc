/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_PWR_H_
#define _MAC_AX_PWR_H_

#include "../type.h"

/* SDIO local register */
#define SDIO_LOCAL_BASE_ADDR	0x80000000

/**
 * Power switch command description
 * @PWR_CMD_WRITE:
 * @PWR_CMD_POLL:
 * @PWR_CMD_DELAY:
 * @PWR_CMD_END:
 */
#define	PWR_CMD_WRITE		0
#define	PWR_CMD_POLL		1
#define	PWR_CMD_DELAY		2
#define	PWR_CMD_END		3

#define	PWR_CUT_MSK_A		BIT(0)
#define	PWR_CUT_MSK_B		BIT(1)
#define	PWR_CUT_MSK_C		BIT(2)
#define	PWR_CUT_MSK_D		BIT(3)
#define	PWR_CUT_MSK_E		BIT(4)
#define	PWR_CUT_MSK_F		BIT(5)
#define	PWR_CUT_MSK_G		BIT(6)
#define	PWR_CUT_MSK_TEST	BIT(7)
#define	PWR_CUT_MSK_ALL		0xFF

#define	PWR_INTF_MSK_SDIO	BIT(0)
#define	PWR_INTF_MSK_USB	BIT(1)
#define	PWR_INTF_MSK_PCIE	BIT(2)
#define	PWR_INTF_MSK_ALL	0x7

#define PWR_BASE_MAC		0
#define PWR_BASE_USB		1
#define PWR_BASE_PCIE		2
#define PWR_BASE_SDIO		3

#define	PWR_DELAY_US		0
#define	PWR_DELAY_MS		1

#define PWR_POLL_CNT		20000

struct mac_pwr_cfg {
	u16 addr;
	u8 cut_msk;
	u8 intf_msk;
	u8 base:4;
	u8 cmd:4;
	u8 msk;
	u8 val;
};

u32 mac_pwr_switch(struct mac_ax_adapter *adapter, u8 on);

#endif
