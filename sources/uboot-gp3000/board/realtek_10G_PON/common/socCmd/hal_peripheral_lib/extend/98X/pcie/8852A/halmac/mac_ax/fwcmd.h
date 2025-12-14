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

#ifndef _MAC_AX_FW_CMD_H_
#define _MAC_AX_FW_CMD_H_

#include "../type.h"
#include "fwcmd_intf.h"

enum h2c_buf_class {
	H2CB_CLASS_CMD,		/* FW command */
	H2CB_CLASS_DATA,	/* FW command + data */
	H2CB_CLASS_LONG_DATA,	/* FW command + long data */

	/* keep last */
	H2CB_CLASS_LAST,
	H2CB_CLASS_MAX = H2CB_CLASS_LAST,
	H2CB_CLASS_INVALID = H2CB_CLASS_LAST,
};

struct h2c_buf_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u8 *pool;
	u32 size;
	u32 qlen;
	u8 suspend;
	mac_ax_mutex lock;
};

struct fwcmd_wkb_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u32 qlen;
	mac_ax_mutex lock;
};

struct h2c_buf {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	enum h2c_buf_class _class_;
	u32 id;
	u8 master;
	u32 len;
	u8 *head;
	u8 *end;
	u8 *data;
	u8 *tail;
	u32 hdr_len;
#define H2CB_FLAGS_FREED	BIT(0)
	u32 flags;
};

struct c2h_proc_class {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len);
};

struct c2h_proc_func {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len);
};

u32 h2cb_init(struct mac_ax_adapter *adapter);
u32 h2cb_exit(struct mac_ax_adapter *adapter);
struct h2c_buf *h2cb_alloc(struct mac_ax_adapter *adapter,
			   enum h2c_buf_class buf_class);
void h2cb_free(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
u8 *h2cb_push(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_pull(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_put(struct h2c_buf *h2cb, u32 len);
u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u8 rack, u8 dack);
u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
u32 fwcmd_wq_enqueue(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
struct h2c_buf *fwcmd_wq_dequeue(struct mac_ax_adapter *adapter, u32 id);
u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id);
u32 mac_process_c2h(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

#endif
