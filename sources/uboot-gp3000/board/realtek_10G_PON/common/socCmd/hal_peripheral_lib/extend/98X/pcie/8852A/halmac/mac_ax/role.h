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

#ifndef _MAC_AX_ROLE_H_
#define _MAC_AX_ROLE_H_

#include "../type.h"

struct mac_role_tbl_head {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	u32 qlen;
	mac_ax_mutex lock;
};

struct mac_role_tbl {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	u8 macid;
	u8 wmm;
};

u32 role_tbl_init(struct mac_ax_adapter *adapter);
u32 role_tbl_exit(struct mac_ax_adapter *adapter);
u32 mac_add_role(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info);
u32 mac_remove_role(struct mac_ax_adapter *adapter, u8 macid);
struct mac_role_tbl *mac_role_srch(struct mac_ax_adapter *adapter, u8 macid);

#endif
