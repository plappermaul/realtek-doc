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

#ifndef _MAC_AX_DBGPKG_H_
#define _MAC_AX_DBGPKG_H_

#include "../type.h"

u32 mac_fwcmd_lb(struct mac_ax_adapter *adapter, u32 len, u8 burst);
u32 c2h_sys_cmd_path(struct mac_ax_adapter *adapter,  u8 *buf, u32 len);
u32 mac_mem_dump(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel,
		 u32 strt_addr, u8 *buf, u32 len);

#endif
