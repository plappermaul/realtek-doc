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

#ifndef _MAC_AX_TRXCFG_H_
#define _MAC_AX_TRXCFG_H_

#include "../type.h"

struct dle_size_cfg {
	u16 pge_size;
	u16 lnk_pge_num;
	u16 unlnk_pge_num;
};

struct wde_quota_cfg {
	u16 hif;
	u16 wcpu;
	u16 pkt_in;
	u16 cpu_io;
};

struct ple_quota_cfg {
	u16 cma0_tx;
	u16 cma1_tx;
	u16 c2h;
	u16 h2c;
	u16 wcpu;
	u16 mpdu_proc;
	u16 cma0_dma;
	u16 cma1_dma;
	u16 bb_rpt;
	u16 wd_rel;
	u16 cpu_io;
};

struct dle_mem_cfg {
	enum mac_ax_trx_mode mode;
	struct dle_size_cfg *wde_size;
	struct dle_size_cfg *ple_size;
	struct wde_quota_cfg *wde_min_qt;
	struct wde_quota_cfg *wde_max_qt;
	struct ple_quota_cfg *ple_min_qt;
	struct ple_quota_cfg *ple_max_qt;
};

u32 mac_trx_init(struct mac_ax_adapter *adapter, enum mac_ax_trx_mode mode);
u32 mac_gen_tx_frame(struct mac_ax_adapter *adapter,
		     struct mac_ax_frame_info *info, u8 *mem);
#endif
