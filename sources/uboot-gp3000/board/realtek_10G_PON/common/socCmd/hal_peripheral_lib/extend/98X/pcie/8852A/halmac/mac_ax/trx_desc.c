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

#include "trx_desc.h"
#include "../txdesc.h"
#include "../rxdesc.h"
#include "role.h"

#define RXD_RPKT_TYPE_INVALID	0xFF
#define TXD_AC_TYPE_MSK		0x3
#define TXD_TID_IND_SH		2

enum wd_info_pkt_type {
	WD_INFO_PKT_NORMAL,

	/* keep last */
	WD_INFO_PKT_LAST,
	WD_INFO_PKT_MAX = WD_INFO_PKT_LAST,
};

static u8 wd_info_tmpl[WD_INFO_PKT_MAX][24] = {
	/* normal packet */
	{0x00, 0x06, 0x8b, 0x50, 0x00, 0x00, 0x0b, 0x0c,
	 0x00, 0x01, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x08,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

u32 mac_txdesc_len(struct mac_ax_adapter *adapter,
		   struct mac_ax_txpkt_info *info)
{
	u32 len;
	enum mac_ax_pkt_t pkt_type = info->type;

	len = WD_BODY_LEN;

	if (pkt_type != MAC_AX_PKT_H2C && pkt_type != MAC_AX_PKT_FWDL)
		len += WD_INFO_LEN;

	return len;
}

static u32 txdes_proc_h2c_fwdl(struct mac_ax_adapter *adapter,
			       struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct wd_body_t *wdb;

	if (len != mac_txdesc_len(adapter, info))
		return MACBUFSZ;

	wdb = (struct wd_body_t *)buf;
	wdb->dword0 = cpu_to_le32(SET_WORD(MAC_AX_CH_DMA_FWCMD, TXD_CH_DMA) |
				  (info->type == MAC_AX_PKT_FWDL ?
				   TXD_FWDL_EN : 0));
	wdb->dword1 = 0;
	wdb->dword2 = cpu_to_le32(SET_WORD(info->pktsize, TXD_TXPKTSIZE));
	wdb->dword3 = 0;
	wdb->dword4 = 0;
	wdb->dword5 = 0;

	return 0;
}

static u32 txdes_proc_data(struct mac_ax_adapter *adapter,
			   struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct wd_body_t *wdb;
	struct wd_info_t *wdi;
	struct mac_ax_pkt_data *datai = &info->u.data;
	struct mac_role_tbl *role;
	u8 ac_type = datai->ac_type & TXD_AC_TYPE_MSK;
	u8 tid_ind = datai->ac_type >> TXD_TID_IND_SH;

	if (len != mac_txdesc_len(adapter, info))
		return MACBUFSZ;

	if (datai->ch > MAC_AX_CH_DMA_CH11) {
		PLTFM_MSG_ERR("[ERR]txd ch: %d\n", datai->ch);
		return MACNOITEM;
	}

	role = mac_role_srch(adapter, datai->macid);
	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", datai->macid);
		return MACNOITEM;
	}

	/* only use ch0 in initial development phase, */
	/* and modify it for normal using later.*/
	/* wd_info is always appended in initial development phase */
	wdb = (struct wd_body_t *)buf;
	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO)
		wdb->dword0 =
			cpu_to_le32(TXD_STF_MODE |
				    SET_WORD(datai->hdr_len, TXD_HDR_LLC_LEN) |
				    SET_WORD(datai->ch, TXD_CH_DMA) |
				    TXD_WDINFO_EN);
	else
		wdb->dword0 =
			cpu_to_le32(SET_WORD(datai->hdr_len, TXD_HDR_LLC_LEN) |
				  SET_WORD(datai->ch, TXD_CH_DMA) |
				  TXD_WDINFO_EN);
	wdb->dword1 = 0;
	/* Get bb and qsel from qsel by according MAC ID */
	wdb->dword2 =
		cpu_to_le32(SET_WORD(info->pktsize, TXD_TXPKTSIZE) |
			    SET_WORD((role->wmm << 2 | ac_type), TXD_QSEL) |
			    (tid_ind ? TXD_TID_IND : 0) |
				  SET_WORD(role->macid, TXD_MACID));
	wdb->dword3 = cpu_to_le32(datai->agg_en ? TXD_AGG_EN : 0);
	wdb->dword4 = 0;
	wdb->dword5 = 0;

	wdi = (struct wd_info_t *)wd_info_tmpl[WD_INFO_PKT_NORMAL];
        wdi->dword0 = cpu_to_le32((datai->userate > 0 ? TXD_USERATE_SEL : 0) |
				  SET_WORD(datai->data_rate, TXD_DATARATE) |
				  SET_WORD(datai->data_bw, TXD_DATA_BW) |
				  SET_WORD(datai->data_gi_ltf, TXD_GI_LTF) |
				  (datai->data_er > 0 ? TXD_DATA_ER : 0) |
				  (datai->data_dcm > 0 ? TXD_DATA_DCM : 0) |
				  SET_WORD(datai->data_stbc, TXD_DATA_STBC) |
				  (datai->data_ldpc > 0 ? TXD_DATA_LDPC : 0) |
				  (datai->dis_data_fb > 0 ?
				   TXD_DISDATAFB : 0) |
				  (datai->dis_rts_fb > 0 ? TXD_DISRTSFB : 0));
        wdi->dword1 =
		cpu_to_le32(SET_WORD(datai->max_agg_num, TXD_MAX_AGG_NUM));
        wdi->dword2 =
		cpu_to_le32(SET_WORD(datai->lifetime_sel, TXD_LIFETIME_SEL) |
			    SET_WORD(datai->ampdu_density, TXD_AMPDU_DENSITY));

	PLTFM_MEMCPY(buf + WD_BODY_LEN, (u8 *)wdi, WD_INFO_LEN);

	return 0;
}

static struct txd_proc_type txdes_proc_mac[] = {
	{MAC_AX_PKT_H2C, txdes_proc_h2c_fwdl},
	{MAC_AX_PKT_FWDL, txdes_proc_h2c_fwdl},
	{MAC_AX_PKT_DATA, txdes_proc_data},
	{MAC_AX_PKT_INVALID, NULL},
};

u32 mac_build_txdesc(struct mac_ax_adapter *adapter,
		     struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct txd_proc_type *proc = txdes_proc_mac;
	enum mac_ax_pkt_t pkt_type = info->type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_txpkt_info *info, u8 *buf, u32 len) = NULL;

	for (; proc->type != MAC_AX_PKT_INVALID; proc++) {
		if (pkt_type == proc->type) {
			handler = proc->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null type handler type: %X\n", proc->type);
		return MACNOITEM;
	}

	return handler(adapter, info, buf, len);
}

static u32 rxdes_parse_wifi(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val;

	info->type = MAC_AX_PKT_DATA;
	info->rxdlen = RXD_LONG_LEN;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);
	info->pktsize = GET_FIELD(hdr_val, RXD_RPKT_LEN);

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword3);
	info->u.data.crc_err = !!(hdr_val & RXD_CRC32_ERR);
	info->u.data.icv_err = !!(hdr_val & RXD_ICV_ERR);

	return 0;
}

static u32 rxdes_parse_c2h(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val;

	info->type = MAC_AX_PKT_C2H;
	info->rxdlen = RXD_SHORT_LEN;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);
	info->pktsize = GET_FIELD(hdr_val, RXD_RPKT_LEN);

	return 0;
}

static struct rxd_parse_type rxdes_parse_mac[] = {
	{RXD_S_RPKT_TYPE_WIFI, rxdes_parse_wifi},
	{RXD_S_RPKT_TYPE_C2H, rxdes_parse_c2h},
	{RXD_RPKT_TYPE_INVALID, NULL},
};

u32 mac_parse_rxdesc(struct mac_ax_adapter *adapter,
		     struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	struct rxd_parse_type *parse = rxdes_parse_mac;
	u8 rpkt_type;
	u32 hdr_val;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len) = NULL;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);
	rpkt_type = GET_FIELD(hdr_val, RXD_RPKT_TYPE);

	for (; parse->type != RXD_RPKT_TYPE_INVALID; parse++) {
		if (rpkt_type == parse->type) {
			handler = parse->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null type handler type: %X\n", parse->type);
		return MACNOITEM;
	}

	return handler(adapter, info, buf, len);
}
