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

#include "dbgpkg.h"
#include "fwcmd.h"
#include "trx_desc.h"

#define	SHARED_BUF_BASE_ADDR		0x18700000
#define	DMAC_TBL_BASE_ADDR		0x18800000
#define	STA_SCHED_BASE_ADDR		0x18808000
#define	UL_GRP_TBL_BASE_ADDR		0x18810000
#define	MU_SCORE_TBL_BASE_ADDR		0x18811000
#define	MACHDR_SHORTCUT_BASE_ADDR	0x18812000
#define	RXPLD_FLTR_CAM_BASE_ADDR	0x18813000
#define	SECURITY_CAM_BASE_ADDR		0x18814000
#define	WOW_CAM_BASE_ADDR		0x18815000
#define	CMAC_TBL_BASE_ADDR		0x18840000
#define	ADDR_CAM_BASE_ADDR		0x18850000
#define	BA_CAM_BASE_ADDR		0x18854000
#define	BCN_IE_CAM_BASE_ADDR		0x18855000

/* base address mapping table of enum mac_ax_mem_sel */
static u32 base_addr_map_tbl[MAC_AX_MEM_MAX] = {
	SHARED_BUF_BASE_ADDR,
	DMAC_TBL_BASE_ADDR,
	STA_SCHED_BASE_ADDR,
	UL_GRP_TBL_BASE_ADDR,
	MU_SCORE_TBL_BASE_ADDR,
	MACHDR_SHORTCUT_BASE_ADDR,
	RXPLD_FLTR_CAM_BASE_ADDR,
	SECURITY_CAM_BASE_ADDR,
	WOW_CAM_BASE_ADDR,
	CMAC_TBL_BASE_ADDR,
	ADDR_CAM_BASE_ADDR,
	BA_CAM_BASE_ADDR,
	BCN_IE_CAM_BASE_ADDR,
};

u32 mac_fwcmd_lb(struct mac_ax_adapter *adapter, u32 len, u8 burst)
{
	u32 i;
	u32 ret;
	struct h2c_buf *h2cb;
	enum h2c_buf_class buf_class;
	u8 *buf;

	if (!burst) {
		ret = fwcmd_wq_idle(adapter,
				    SET_FWCMD_ID(FWCMD_TYPE_H2C,
				    		 FWCMD_H2C_CAT_SYS,
				    		 FWCMD_H2C_CL_CMD_PATH,
				    		 FWCMD_H2C_FUNC_CMD_LB));
		if (ret)
			return ret;
	}

	if (len <= H2C_CMD_LEN)
		buf_class = H2CB_CLASS_CMD;
	else if (len <= H2C_DATA_LEN)
		buf_class = H2CB_CLASS_DATA;
	else
		buf_class = H2CB_CLASS_LONG_DATA;

	h2cb = h2cb_alloc(adapter, buf_class);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, len - FWCMD_HDR_LEN);
	if (!buf)
		return MACNPTR;

	for (i = 0; i < len - FWCMD_HDR_LEN; i++)
		buf[i] = (u8)(i & 0xFF);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_SYS,
			      FWCMD_H2C_CL_CMD_PATH,
			      FWCMD_H2C_FUNC_CMD_LB,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		return ret;
	}

	ret = fwcmd_wq_enqueue(adapter, h2cb);
	if (ret)
		return ret;

	return 0;
}

static u32 c2h_sys_cmd_lb(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	u32 hdr1;
	struct h2c_buf *h2cb;
	u32 c2h_len;

	h2cb = fwcmd_wq_dequeue(adapter, SET_FWCMD_ID(FWCMD_TYPE_H2C,
						      FWCMD_H2C_CAT_SYS,
						      FWCMD_H2C_CL_CMD_PATH,
						      FWCMD_H2C_FUNC_CMD_LB));
	if (!h2cb)
		return MACNPTR;

	hdr1 = ((struct fwcmd_hdr *)buf)->hdr1;
	hdr1 = le32_to_cpu(hdr1);

	c2h_len = GET_FIELD(hdr1, C2H_HDR_LEN);
	if (c2h_len != h2cb->len) {
		PLTFM_MSG_ERR("[ERR]fwcmd lb wrong len\n");
		return MACCMP;
	}

	if (PLTFM_MEMCMP(buf + FWCMD_HDR_LEN, h2cb->data + FWCMD_HDR_LEN,
			 c2h_len - FWCMD_HDR_LEN))
		return MACCMP;

	h2cb_free(adapter, h2cb);

	return 0;
}

static struct c2h_proc_func c2h_proc_sys_cmd_path[] = {
	{FWCMD_C2H_FUNC_CMD_LB, c2h_sys_cmd_lb},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_sys_cmd_path(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct c2h_proc_func *proc = c2h_proc_sys_cmd_path;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)(buf + RXD_SHORT_LEN))->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, FWCMD_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", proc->id);
		return MACNOITEM;
	}

	return handler(adapter, buf + RXD_SHORT_LEN, len);
}

static u32 __dump_mac_mem(struct mac_ax_adapter *adapter,
			  enum mac_ax_mem_sel sel, u32 offset, u8 *buf, u32 len)
{
#define MAC_MEM_ACCESS_STRT_REG		0x40000
#define MAC_MEM_DUMP_PAGE_SIZE		0x40000 /* 256KB */
	u32 base_addr;
	u32 strt_pg;
	u32 residue;
	u32 i;
	u32 cnt = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (sel >= MAC_AX_MEM_LAST)
		return MACNOITEM;

	strt_pg = offset / MAC_MEM_DUMP_PAGE_SIZE;
	residue = offset % MAC_MEM_DUMP_PAGE_SIZE;
	base_addr = base_addr_map_tbl[sel] + strt_pg * MAC_MEM_DUMP_PAGE_SIZE;

	do {
		/* TODO: Add base address register setting later. */

		for (i = MAC_MEM_ACCESS_STRT_REG + residue;
		     i <= MAC_MEM_ACCESS_STRT_REG + MAC_MEM_DUMP_PAGE_SIZE;
		     i += 4) {
			*(u32 *)(buf + cnt) = le32_to_cpu(MAC_REG_R32(i));
			cnt += 4;
			if (cnt == len)
				goto out;
		}

		residue = 0;
		base_addr += MAC_MEM_DUMP_PAGE_SIZE;
	} while (1);
out:
	/* TODO: Set base address register setting to zero later. */

	return 0;
#undef MAC_MEM_ACCESS_STRT_REG
#undef MAC_MEM_DUMP_PAGE_SIZE
}

u32 mac_mem_dump(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel,
		 u32 strt_addr, u8 *buf, u32 len)
{
	u32 ret;

	if (len & (4 - 1)) {
		PLTFM_MSG_ERR("[ERR]not 4byte alignment\n");
		return MACBUFSZ;
	}

	if (!buf){
		PLTFM_MSG_ERR("[ERR]null pointer buf\n");
		return MACNPTR;
	}

	ret = __dump_mac_mem(adapter, sel, strt_addr, buf, len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]dump mac memory\n");
		return ret;
	}

	return 0;
}
