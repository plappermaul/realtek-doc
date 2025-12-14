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

#include "../pwr.h"

static struct mac_pwr_cfg mac_pwron_8852a[] = {
	{0x0006,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_WRITE, BIT(0) | BIT(1), 3},
	{0x0200,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_DELAY, 0, PWR_DELAY_MS},
	{0x0005,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_WRITE, BIT(0), 1},
	{0x0200,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_DELAY, 0, PWR_DELAY_MS},
	{0x0088,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0x0200,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_DELAY, 0, PWR_DELAY_MS},
	{0x0003,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_POLL, BIT(3) | BIT(7), BIT(3) | BIT(7)},
	{0x0088,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_WRITE, BIT(1), BIT(1)},
	{0x0200,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_DELAY, 0, PWR_DELAY_MS},
	{0xFFFF,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 0,
	 PWR_CMD_END, 0, 0},
};

static struct mac_pwr_cfg mac_pwroff_8852a[] = {
	{0x0005,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_WRITE, BIT(1), 1},
	{0x0200,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 PWR_BASE_MAC,
	 PWR_CMD_DELAY, 0, PWR_DELAY_MS},
	{0xFFFF,
	 PWR_CUT_MSK_ALL,
	 PWR_INTF_MSK_ALL,
	 0,
	 PWR_CMD_END, 0, 0},
};

/* Power on sequence */
struct mac_pwr_cfg *pwr_on_seq_8852a[] = {
	mac_pwron_8852a,
	NULL
};

/* Power off sequence */
struct mac_pwr_cfg *pwr_off_seq_8852a[] = {
	mac_pwroff_8852a,
	NULL
};