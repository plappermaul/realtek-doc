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

#ifndef _MAC_AX_TRX_DESC_H_
#define _MAC_AX_TRX_DESC_H_

#include "../type.h"

struct wd_body_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
};

struct wd_info_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
};

#define WD_BODY_LEN	(sizeof(struct wd_body_t))
#define WD_INFO_LEN	(sizeof(struct wd_info_t))

struct rxd_short_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
};

struct rxd_long_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
};

#define RXD_SHORT_LEN	(sizeof(struct rxd_short_t))
#define RXD_LONG_LEN	(sizeof(struct rxd_long_t))

struct txd_proc_type {
	enum mac_ax_pkt_t type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_txpkt_info *info, u8 *buf, u32 len);
};

struct rxd_parse_type {
	u8 type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
};

u32 mac_txdesc_len(struct mac_ax_adapter *adapter,
		   struct mac_ax_txpkt_info *info);
u32 mac_build_txdesc(struct mac_ax_adapter *adapter,
		     struct mac_ax_txpkt_info *info, u8 *buf, u32 len);
u32 mac_parse_rxdesc(struct mac_ax_adapter *adapter,
		     struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);

#endif
