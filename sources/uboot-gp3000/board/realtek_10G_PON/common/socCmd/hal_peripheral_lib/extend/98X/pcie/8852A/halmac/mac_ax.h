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

#ifndef _MAC_AX_H_
#define _MAC_AX_H_

#include "type.h"
#include "mac_reg.h"
#include "chip_cfg.h"

#define MAC_AX_MAJOR_VER	0
#define MAC_AX_CHIP_VER		0
#define MAC_AX_SUB_VER		1
#define MAC_AX_PATCH_VER	0

#define MAC_AX_SRC_VER(a, b, c, d)                                             \
				(((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

u32 mac_ax_ops_init(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
		    enum mac_ax_intf intf,
		    struct mac_ax_adapter **mac_adapter,
		    struct mac_ax_ops **mac_ops);
u32 mac_ax_ops_exit(struct mac_ax_adapter *adapter);

#endif
