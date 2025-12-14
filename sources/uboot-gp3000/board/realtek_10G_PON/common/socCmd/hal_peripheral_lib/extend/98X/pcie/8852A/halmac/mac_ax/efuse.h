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

#ifndef _MAC_AX_EFUSE_H_
#define _MAC_AX_EFUSE_H_

#include "../type.h"

#define RSVD_EFUSE_SIZE		16
#define RSVD_CS_EFUSE_SIZE	24
#define EFUSE_WAIT_CNT		1000000

struct mac_efuse_tbl {
	mac_ax_mutex lock;
};

enum efuse_map_sel {
	EFUSE_MAP_SEL_PHY_WL,
	EFUSE_MAP_SEL_PHY_BT,
	EFUSE_MAP_SEL_LOG,

	/* keep last */
	EFUSE_MAP_SEL_LAST,
	EFUSE_MAP_SEL_MAX = EFUSE_MAP_SEL_LAST,
	EFUSE_MAP_SEL_INVALID = EFUSE_MAP_SEL_LAST,
};

u32 mac_dump_efuse_map_wl(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg);
u32 mac_dump_efuse_map_bt(struct mac_ax_adapter *adapter);
u32 mac_write_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val,
		    enum mac_ax_efuse_bank bank);
u32 mac_read_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size, u8 *val,
		   enum mac_ax_efuse_bank bank);
u32 mac_get_efuse_avl_size(struct mac_ax_adapter *adapter, u32 *size);
u32 mac_dump_log_efuse(struct mac_ax_adapter *adapter,
		       enum mac_ax_efuse_parser_cfg parser_cfg,
		       enum mac_ax_efuse_read_cfg cfg);
u32 mac_read_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
		       u8 *val);
u32 mac_write_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val);
u32 mac_pg_efuse_by_map(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info,
			enum mac_ax_efuse_read_cfg cfg);
u32 mac_mask_log_efuse(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info);
u32 mac_pg_sec_data_by_map(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info);
u32 mac_cmp_sec_data_by_map(struct mac_ax_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info);

void cfg_efuse_auto_ck(struct mac_ax_adapter *adapter, u8 enable);
u32 efuse_tbl_init(struct mac_ax_adapter *adapter);
u32 efuse_tbl_exit(struct mac_ax_adapter *adapter);

#endif
