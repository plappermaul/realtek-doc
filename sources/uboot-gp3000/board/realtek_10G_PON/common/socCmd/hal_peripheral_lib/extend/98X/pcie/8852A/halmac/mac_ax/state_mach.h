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

#ifndef _MAC_AX_STATE_MACH_H_
#define _MAC_AX_STATE_MACH_H_

struct mac_ax_state_mach {
#define MAC_AX_PWR_OFF	0
#define MAC_AX_PWR_ON	1
#define MAC_AX_PWR_ERR	2
	u8 pwr;
#define MAC_AX_FWDL_IDLE		0
#define MAC_AX_FWDL_H2C_PATH_RDY	1
#define MAC_AX_FWDL_PATH_RDY		2
#define MAC_AX_FWDL_INIT_RDY		3
	u8 fwdl;
#define MAC_AX_EFUSE_IDLE		0
#define MAC_AX_EFUSE_PHY		1
#define MAC_AX_EFUSE_LOG_MAP		2
#define MAC_AX_EFUSE_LOG_MASK		3
#define MAC_AX_EFUSE_MAX		4
	u8 efuse;
};

#endif
