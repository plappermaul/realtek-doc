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

#include "tblupd.h"
#include "fwcmd.h"
#include "trx_desc.h"

#define DLRU_CLASS_GRP_TBL	0
#define ULRU_CLASS_GRP_TBL	1
#define CLASS_RUSTA_INFO	2
#define DLRU_CLASS_FIXED_TBL	3
#define ULRU_CLASS_FIXED_TBL	4
#define CLASS_BA_INFOTBL	5
#define CLASS_MU_FIXINFO	6
#define CLASS_UL_FIXINFO	7

u32 mac_upd_dlru_fixtbl(struct mac_ax_adapter *adapter,
			struct mac_ax_dlru_fixtbl *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_dlru_fixtbl *tbl;
	struct mac_ax_dl_fix_sta_ent *sta_ent;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dlru_fixtbl));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_dlru_fixtbl *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(DLRU_CLASS_FIXED_TBL, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->max_sta_num, FWCMD_DLRU_FIXTBL_MAXSTA) |
		    SET_WORD(info->min_sta_num, FWCMD_DLRU_FIXTBL_MINSTA) |
		    SET_WORD(info->doppler, FWCMD_DLRU_FIXTBL_DPLR) |
		    SET_WORD(info->stbc, FWCMD_DLRU_FIXTBL_STBC) |
		    SET_WORD(info->gi_ltf, FWCMD_DLRU_FIXTBL_GILTF) |
		    SET_WORD(info->ma_type, FWCMD_DLRU_FIXTBL_MA_TYPE) |
		    SET_WORD(info->fixru_flag, FWCMD_DLRU_FIXTBL_FIXRU));

	sta_ent = &info->sta[0];
	tbl->dword2 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_DL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_DL_FIXSTA_RU_POS1)|
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(sta_ent->fix_rate, FWCMD_DL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_DL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->fix_txbf, FWCMD_DL_FIXSTA_FIXTXBF) |
		    SET_WORD(sta_ent->fix_pwr_fac, FWCMD_DL_FIXSTA_FIXPWRFAC) |
		    SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_DL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_DL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_DL_RATE_MCS) |
		    SET_WORD(sta_ent->txbf, FWCMD_DL_FIXSTA_TXBF) |
		    SET_WORD(sta_ent->coding, FWCMD_DL_FIXSTA_CODING) |
		    SET_WORD(sta_ent->pwr_boost_fac, FWCMD_DL_FIXSTA_PWR_FAC));

	sta_ent = &info->sta[1];
	tbl->dword4 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_DL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_DL_FIXSTA_RU_POS1)|
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(sta_ent->fix_rate, FWCMD_DL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_DL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->fix_txbf, FWCMD_DL_FIXSTA_FIXTXBF) |
		    SET_WORD(sta_ent->fix_pwr_fac, FWCMD_DL_FIXSTA_FIXPWRFAC) |
		    SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_DL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_DL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_DL_RATE_MCS) |
		    SET_WORD(sta_ent->txbf, FWCMD_DL_FIXSTA_TXBF) |
		    SET_WORD(sta_ent->coding, FWCMD_DL_FIXSTA_CODING) |
		    SET_WORD(sta_ent->pwr_boost_fac, FWCMD_DL_FIXSTA_PWR_FAC));

	sta_ent = &info->sta[2];
	tbl->dword6 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_DL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_DL_FIXSTA_RU_POS1)|
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(sta_ent->fix_rate, FWCMD_DL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_DL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->fix_txbf, FWCMD_DL_FIXSTA_FIXTXBF) |
		    SET_WORD(sta_ent->fix_pwr_fac, FWCMD_DL_FIXSTA_FIXPWRFAC) |
		    SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_DL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_DL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_DL_RATE_MCS) |
		    SET_WORD(sta_ent->txbf, FWCMD_DL_FIXSTA_TXBF) |
		    SET_WORD(sta_ent->coding, FWCMD_DL_FIXSTA_CODING) |
		    SET_WORD(sta_ent->pwr_boost_fac, FWCMD_DL_FIXSTA_PWR_FAC));

	sta_ent = &info->sta[3];
	tbl->dword8 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_DL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_DL_FIXSTA_RU_POS1)|
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(sta_ent->fix_rate, FWCMD_DL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_DL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->fix_txbf, FWCMD_DL_FIXSTA_FIXTXBF) |
		    SET_WORD(sta_ent->fix_pwr_fac, FWCMD_DL_FIXSTA_FIXPWRFAC) |
		    SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_DL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_DL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_DL_RATE_MCS) |
		    SET_WORD(sta_ent->txbf, FWCMD_DL_FIXSTA_TXBF) |
		    SET_WORD(sta_ent->coding, FWCMD_DL_FIXSTA_CODING) |
		    SET_WORD(sta_ent->pwr_boost_fac, FWCMD_DL_FIXSTA_PWR_FAC));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

u32 mac_upd_ulru_fixtbl(struct mac_ax_adapter *adapter,
			struct mac_ax_ulru_fixtbl *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_ulru_fixtbl *tbl;
	struct mac_ax_ul_fix_sta_ent *sta_ent;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ulru_fixtbl));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_ulru_fixtbl *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(ULRU_CLASS_FIXED_TBL, FWCMD_TBLHDR_CLASS));
	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->max_sta_num, FWCMD_DLRU_FIXTBL_MAXSTA) |
		    SET_WORD(info->min_sta_num, FWCMD_DLRU_FIXTBL_MINSTA) |
		    SET_WORD(info->doppler, FWCMD_DLRU_FIXTBL_DPLR) |
		    SET_WORD(info->ma_type, FWCMD_ULRU_FIXTBL_MA_TYPE) |
		    SET_WORD(info->gi_ltf, FWCMD_ULRU_FIXTBL_GILTF) |
		    SET_WORD(info->stbc, FWCMD_ULRU_FIXTBL_STBC) |
		    SET_WORD(info->fix_tb_t_pe_nom, FWCMD_ULRU_FIXTBL_FIX_NOM) |
		    SET_WORD(info->tb_t_pe_nom, FWCMD_ULRU_FIXTBL_TB_NOM) |
		    SET_WORD(info->fixru_flag, FWCMD_ULRU_FIXTBL_FIXRU));

	sta_ent = &info->sta[0];
	tbl->dword2 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_UL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_UL_FIXSTA_RU_POS1) |
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(sta_ent->tgt_rssi[0], FWCMD_UL_FIXSTA_TGT_RSSI0) |
		    SET_WORD(sta_ent->tgt_rssi[1], FWCMD_UL_FIXSTA_TGT_RSSI1) |
		    SET_WORD(sta_ent->tgt_rssi[2], FWCMD_UL_FIXSTA_TGT_RSSI2) |
		    SET_WORD(sta_ent->fix_tgt_rssi, FWCMD_UL_FIXSTA_FIX_RSSI) |
		    SET_WORD(sta_ent->fix_rate, FWCMD_UL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_UL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->coding, FWCMD_UL_FIXSTA_CODING));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_UL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_UL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_UL_RATE_MCS));

	sta_ent = &info->sta[1];
	tbl->dword5 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_UL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_UL_FIXSTA_RU_POS1) |
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword6 =
	cpu_to_le32(SET_WORD(sta_ent->tgt_rssi[0], FWCMD_UL_FIXSTA_TGT_RSSI0) |
		    SET_WORD(sta_ent->tgt_rssi[1], FWCMD_UL_FIXSTA_TGT_RSSI1) |
		    SET_WORD(sta_ent->tgt_rssi[2], FWCMD_UL_FIXSTA_TGT_RSSI2) |
		    SET_WORD(sta_ent->fix_tgt_rssi, FWCMD_UL_FIXSTA_FIX_RSSI) |
		    SET_WORD(sta_ent->fix_rate, FWCMD_UL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_UL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->coding, FWCMD_UL_FIXSTA_CODING));

	tbl->dword7 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_UL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_UL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_UL_RATE_MCS));

	sta_ent = &info->sta[2];
	tbl->dword8 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_UL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_UL_FIXSTA_RU_POS1) |
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(sta_ent->tgt_rssi[0], FWCMD_UL_FIXSTA_TGT_RSSI0) |
		    SET_WORD(sta_ent->tgt_rssi[1], FWCMD_UL_FIXSTA_TGT_RSSI1) |
		    SET_WORD(sta_ent->tgt_rssi[2], FWCMD_UL_FIXSTA_TGT_RSSI2) |
		    SET_WORD(sta_ent->fix_tgt_rssi, FWCMD_UL_FIXSTA_FIX_RSSI) |
		    SET_WORD(sta_ent->fix_rate, FWCMD_UL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_UL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->coding, FWCMD_UL_FIXSTA_CODING));

	tbl->dword10 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_UL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_UL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_UL_RATE_MCS));

	sta_ent = &info->sta[3];
	tbl->dword11 =
	cpu_to_le32(SET_WORD(sta_ent->mac_id, FWCMD_UL_FIXSTA_MACID) |
		    SET_WORD(sta_ent->ru_pos[0], FWCMD_DL_FIXSTA_RU_POS0) |
		    SET_WORD(sta_ent->ru_pos[1], FWCMD_UL_FIXSTA_RU_POS1) |
		    SET_WORD(sta_ent->ru_pos[2], FWCMD_DL_FIXSTA_RU_POS2));

	tbl->dword12 =
	cpu_to_le32(SET_WORD(sta_ent->tgt_rssi[0], FWCMD_UL_FIXSTA_TGT_RSSI0) |
		    SET_WORD(sta_ent->tgt_rssi[1], FWCMD_UL_FIXSTA_TGT_RSSI1) |
		    SET_WORD(sta_ent->tgt_rssi[2], FWCMD_UL_FIXSTA_TGT_RSSI2) |
		    SET_WORD(sta_ent->fix_tgt_rssi, FWCMD_UL_FIXSTA_FIX_RSSI) |
		    SET_WORD(sta_ent->fix_rate, FWCMD_UL_FIXSTA_FIXRATE) |
		    SET_WORD(sta_ent->fix_coding, FWCMD_UL_FIXSTA_FIXCODE) |
		    SET_WORD(sta_ent->coding, FWCMD_UL_FIXSTA_CODING));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_FIXSTA_UL_RATE_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_FIXSTA_UL_RATE_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_FIXSTA_UL_RATE_MCS));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}


u32 mac_upd_dlru_grptbl(struct mac_ax_adapter *adapter,
			struct mac_ax_dl_ru_grptbl *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_dlru_grptbl *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_dlru_grptbl));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_dlru_grptbl *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(DLRU_CLASS_GRP_TBL, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->ppdu_bw, FWCMD_DLRU_GRPTBL_PPDUBW) |
		    SET_WORD(info->tx_pwr, FWCMD_DLRU_GRPTBL_TXPWR) |
		    SET_WORD(info->pwr_boost_fac, FWCMD_DLRU_GRPTBL_PWRFAC) |
		    SET_WORD(info->fix_mode_flag, FWCMD_DLRU_GRPTBL_FIXFLAG));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->tf.fix_ba, FWCMD_GRPTBL_TFBA_FIXBA)|
		    SET_WORD(info->tf.ru_psd, FWCMD_GRPTBL_TFBA_RUPSD) |
		    SET_WORD(info->tf.tf_rate, FWCMD_GRPTBL_TFBA_RATE)|
		    SET_WORD(info->tf.rf_gain_fix, FWCMD_GRPTBL_TFBA_RFGFIX)|
		    SET_WORD(info->tf.rf_gain_idx, FWCMD_GRPTBL_TFBA_RFGIDX)|
		    SET_WORD(info->tf.tb_ppdu_bw, FWCMD_GRPTBL_TFBA_TBBW));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->tf.rate.dcm, FWCMD_DLRU_GRPTBL_RATE_DCM) |
		    SET_WORD(info->tf.rate.ss, FWCMD_DLRU_GRPTBL_RATE_SS) |
		    SET_WORD(info->tf.rate.mcs, FWCMD_DLRU_GRPTBL_RATE_MCS) |
		    SET_WORD(info->tf.gi_ltf, FWCMD_GRPTBL_TFBA_GILTF)|
		    SET_WORD(info->tf.doppler, FWCMD_GRPTBL_TFBA_DPLR) |
		    SET_WORD(info->tf.stbc, FWCMD_GRPTBL_TFBA_STBC)|
		    SET_WORD(info->tf.sta_coding, FWCMD_GRPTBL_TFBA_CODING)|
		    SET_WORD(info->tf.tb_t_pe_nom, FWCMD_GRPTBL_TFBA_TB_NOM)|
		    SET_WORD(info->tf.pr20_bw_en, FWCMD_GRPTBL_TFBA_PR20BW));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

u32 mac_upd_ulru_grptbl(struct mac_ax_adapter *adapter,
			struct mac_ax_ul_ru_grptbl *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_ulru_grptbl *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ulru_grptbl));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_ulru_grptbl *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(ULRU_CLASS_GRP_TBL, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->grp_psd_max, FWCMD_ULRU_GRPTBL_PSDMAX) |
		    SET_WORD(info->grp_psd_min, FWCMD_ULRU_GRPTBL_PSDMIN) |
		    SET_WORD(info->tf_rate, FWCMD_ULRU_GRPTBL_TFRATE) |
		    SET_WORD(info->fix_tf_rate, FWCMD_ULRU_GRPTBL_FIXTFR));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->ppdu_bw, FWCMD_ULRU_GRPTBL_PPDUBW)|
		    SET_WORD(info->rf_gain_fix, FWCMD_ULRU_GRPTBL_RFGFIX) |
		    SET_WORD(info->rf_gain_idx, FWCMD_ULRU_GRPTBL_RFGIDX)|
		    SET_WORD(info->fix_mode_flag, FWCMD_ULRU_GRPTBL_FIXFLAG));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}


u32 mac_upd_rusta_info(struct mac_ax_adapter *adapter,
			struct mac_ax_bb_stainfo *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_ru_stainfo *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ru_stainfo));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_ru_stainfo *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(CLASS_RUSTA_INFO, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->gi_ltf_48spt, FWCMD_GILTF_48SPT) |
		    SET_WORD(info->gi_ltf_18spt, FWCMD_GILTF_18SPT) |
		    SET_WORD(info->dlsu_info_en, FWCMD_DLSU_INFO_EN) |
		    SET_WORD(info->dlsu_bw, FWCMD_DLSU_BW) |
		    SET_WORD(info->dlsu_gi_ltf, FWCMD_DLSU_GILTF) |
		    SET_WORD(info->dlsu_doppler_ctrl, FWCMD_DLSU_DOOPLER_CTRL) |
		    SET_WORD(info->dlsu_coding, FWCMD_DLSU_CODING) |
		    SET_WORD(info->dlsu_txbf, FWCMD_DLSU_TXBF) |
		    SET_WORD(info->dlsu_stbc, FWCMD_DLSU_STBC) |
		    SET_WORD(info->dl_fwcqi_flag, FWCMD_DL_FWCQI_FLAG) |
		    SET_WORD(info->dlru_ratetbl_ridx, FWCMD_DLRU_RATETBL_RIDX) |
		    SET_WORD(info->csi_info_bitmap, FWCMD_CSIINFO_BITMAP));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->dl_swgrp_bitmap, FWCMD_DL_SWGRP_BITMAP));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->dlsu_dcm, FWCMD_DLSU_DCM)|
		    SET_WORD(info->dlsu_rate, FWCMD_DLSU_RATE) |
		    SET_WORD(info->dlsu_pwr, FWCMD_DLSU_PWR));

	tbl->dword4 =
	cpu_to_le32(SET_WORD(info->ulsu_info_en, FWCMD_ULSU_INFO_EN)|
		    SET_WORD(info->ulsu_bw, FWCMD_ULSU_BW) |
		    SET_WORD(info->ulsu_gi_ltf, FWCMD_ULSU_GILTF) |
		    SET_WORD(info->ulsu_doppler_ctrl, FWCMD_ULSU_DOOPLER_CTRL) |
		    SET_WORD(info->ulsu_dcm, FWCMD_ULSU_DCM) |
		    SET_WORD(info->ulsu_ss, FWCMD_ULSU_SS) |
		    SET_WORD(info->ulsu_mcs, FWCMD_ULSU_MCS) |
		    SET_WORD(info->ul_fwcqi_flag, FWCMD_UL_FWCQI_FLAG) |
		    SET_WORD(info->ulru_ratetbl_ridx, FWCMD_ULRU_RATETBL_RIDX) |
		    SET_WORD(info->ulsu_stbc, FWCMD_ULSU_STBC) |
		    SET_WORD(info->ulsu_coding, FWCMD_ULSU_CODING) |
		    SET_WORD(info->ulsu_rssi_m, FWCMD_ULSU_RSSI_M));

	tbl->dword5 =
	cpu_to_le32(SET_WORD(info->ul_swgrp_bitmap, FWCMD_UL_SWGRP_BITMAP));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

u32 mac_upd_ba_infotbl(struct mac_ax_adapter *adapter,
			struct mac_ax_ba_infotbl *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_ba_infotbl *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ba_infotbl));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_ba_infotbl *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(CLASS_RUSTA_INFO, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->fix_ba, FWCMD_GRPTBL_TFBA_FIXBA)|
		    SET_WORD(info->ru_psd, FWCMD_GRPTBL_TFBA_RUPSD) |
		    SET_WORD(info->tf_rate, FWCMD_GRPTBL_TFBA_RATE)|
		    SET_WORD(info->rf_gain_fix, FWCMD_GRPTBL_TFBA_RFGFIX)|
		    SET_WORD(info->rf_gain_idx, FWCMD_GRPTBL_TFBA_RFGIDX)|
		    SET_WORD(info->tb_ppdu_bw, FWCMD_GRPTBL_TFBA_TBBW));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->rate.dcm, FWCMD_DLRU_GRPTBL_RATE_DCM) |
		    SET_WORD(info->rate.ss, FWCMD_DLRU_GRPTBL_RATE_SS) |
		    SET_WORD(info->rate.mcs, FWCMD_DLRU_GRPTBL_RATE_MCS) |
		    SET_WORD(info->gi_ltf, FWCMD_GRPTBL_TFBA_GILTF)|
		    SET_WORD(info->doppler, FWCMD_GRPTBL_TFBA_DPLR) |
		    SET_WORD(info->stbc, FWCMD_GRPTBL_TFBA_STBC)|
		    SET_WORD(info->sta_coding, FWCMD_GRPTBL_TFBA_CODING)|
		    SET_WORD(info->tb_t_pe_nom, FWCMD_GRPTBL_TFBA_TB_NOM)|
		    SET_WORD(info->pr20_bw_en, FWCMD_GRPTBL_TFBA_PR20BW));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

u32 mac_upd_mu_info(struct mac_ax_adapter *adapter,
			struct mac_ax_mu_fixinfo *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_mu_fixinfo *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mu_fixinfo));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_mu_fixinfo *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(CLASS_MU_FIXINFO, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->mu_thold_en, FWCMD_MUINFO_THOLDEN) |
		    SET_WORD(info->mu_thold, FWCMD_MUINFO_THOLD) |
		    SET_WORD(info->bypass_thold, FWCMD_BYPASS_THOLD));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->fix_mu_en, FWCMD_MUINFO_FIXMU_EN) |
		    SET_WORD(info->fix_mu_macid, FWCMD_MUINFO_FIXMU_ID));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

u32 mac_upd_ul_fixinfo(struct mac_ax_adapter *adapter,
			struct mac_ax_ul_fixinfo *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_ul_fixinfo *tbl;
	struct mac_ul_macid_info *sta_info;
	struct mac_ax_ulru_out_sta_ent *sta_ent;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ul_fixinfo));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_ul_fixinfo *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tbl_hdr.rw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tbl_hdr.idx, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tbl_hdr.offset, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tbl_hdr.len, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tbl_hdr.type, FWCMD_TBLHDR_TYPE) |
		    SET_WORD(ULRU_CLASS_FIXED_TBL, FWCMD_TBLHDR_CLASS));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->ndpa_dur, FWCMD_ULINFO_NDPADUR) |
		    SET_WORD(info->tf_type, FWCMD_ULINFO_TFTYPE) |
		    SET_WORD(info->sig_ta_pkten, FWCMD_ULINFO_SIGEN) |
		    SET_WORD(info->sig_ta_pktsc, FWCMD_ULINFO_SIGSC) |
		    SET_WORD(info->murts_flag, FWCMD_ULINFO_MURTS) |
		    SET_WORD(info->ndpa, FWCMD_ULINFO_NDPA) |
		    SET_WORD(info->snd_pkt_sel, FWCMD_ULINFO_SNDPKT) |
		    SET_WORD(info->gi_ltf, FWCMD_ULINFO_GI_LTF));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->data_rate, FWCMD_ULINFO_DATART)|
		    SET_WORD(info->data_er, FWCMD_ULINFO_DATAER) |
		    SET_WORD(info->data_bw, FWCMD_ULINFO_DATABW) |
		    SET_WORD(info->data_stbc, FWCMD_ULINFO_STBC) |
		    SET_WORD(info->data_ldpc, FWCMD_ULINFO_LDPC) |
		    SET_WORD(info->data_dcm, FWCMD_ULINFO_DATADCM) |
		    SET_WORD(info->apep_len, FWCMD_ULINFO_APEPLEN) |
		    SET_WORD(info->more_tf, FWCMD_ULINFO_MORETF) |
		    SET_WORD(info->data_bw_er, FWCMD_ULINFO_DATA_BWER));

	tbl->dword3 =
	cpu_to_le32(SET_WORD(info->multiport_id, FWCMD_ULINFO_MULPORT) |
		    SET_WORD(info->mbssid, FWCMD_ULINFO_MBSSID) |
		    SET_WORD(info->txpwr_mode, FWCMD_ULINFO_TXPWR_MODE));

	sta_info = &info->sta[0];
	tbl->word4 =
	cpu_to_le32(SET_WORD(sta_info->macid, FWCMD_ULINFO_MACID) |
		    SET_WORD(sta_info->pref_AC, FWCMD_ULINFO_PREF_AC));

	sta_info = &info->sta[1];
	tbl->word5 =
	cpu_to_le32(SET_WORD(sta_info->macid, FWCMD_ULINFO_MACID) |
		    SET_WORD(sta_info->pref_AC, FWCMD_ULINFO_PREF_AC));

	sta_info = &info->sta[2];
	tbl->word6 =
	cpu_to_le32(SET_WORD(sta_info->macid, FWCMD_ULINFO_MACID) |
		    SET_WORD(sta_info->pref_AC, FWCMD_ULINFO_PREF_AC));

	sta_info = &info->sta[3];
	tbl->word7 =
	cpu_to_le32(SET_WORD(sta_info->macid, FWCMD_ULINFO_MACID) |
		    SET_WORD(sta_info->pref_AC, FWCMD_ULINFO_PREF_AC));

	tbl->dword8 =
	cpu_to_le32(SET_WORD(info->ulrua.ru2su, FWCMD_ULRUA_RU2SU) |
		    SET_WORD(info->ulrua.ppdu_bw, FWCMD_ULRUA_PPDW_BW) |
		    SET_WORD(info->ulrua.gi_ltf, FWCMD_ULRUA_GI_LTF) |
		    SET_WORD(info->ulrua.stbc, FWCMD_ULRUA_STBC) |
		    SET_WORD(info->ulrua.doppler, FWCMD_ULRUA_DPLR) |
		    SET_WORD(info->ulrua.rf_gain_fix, FWCMD_ULRUA_RFGFIX) |
		    SET_WORD(info->ulrua.rf_gain_idx, FWCMD_ULRUA_RFGIDX) |
		    SET_WORD(info->ulrua.tb_t_pe_nom, FWCMD_ULRUA_TB_NOM) |
		    SET_WORD(info->ulrua.n_ltf_and_ma, FWCMD_ULRUA_LTF_MA) |
		    SET_WORD(info->ulrua.sta_num, FWCMD_ULRUA_STANUM));

	tbl->dword9 =
	cpu_to_le32(SET_WORD(info->ulrua.grp_mode, FWCMD_ULRUA_GRPMODE) |
		    SET_WORD(info->ulrua.grp_id, FWCMD_ULRUA_GRPID) |
		    SET_WORD(info->ulrua.fix_mode, FWCMD_ULRUA_FIXMODE));

	sta_ent = &info->ulrua.sta[0];
	tbl->dword10 =
	cpu_to_le32(SET_WORD(sta_ent->dropping, FWCMD_ULRUA_DROP)|
		    SET_WORD(sta_ent->tgt_rssi,FWCMD_ULRUA_TGT_RSSI) |
		    SET_WORD(sta_ent->mac_id, FWCMD_ULRUA_MACID) |
		    SET_WORD(sta_ent->ru_pos, FWCMD_ULRUA_RUPOS) |
		    SET_WORD(sta_ent->coding, FWCMD_ULRUA_CODE) |
		    SET_WORD(sta_ent->vip_flag, FWCMD_ULRUA_VIP));

	tbl->dword11 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_ULRUA_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_ULRUA_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_ULRUA_MCS) |
		    SET_WORD(sta_ent->rpt.rt_tblcol, FWCMD_ULRUA_RT_TBLCOL) |
		    SET_WORD(sta_ent->rpt.prtl_alloc, FWCMD_ULRUA_PRTL_ALLOC) |
		    SET_WORD(sta_ent->rpt.rate_chg, FWCMD_ULRUA_RATE_CHG));

	sta_ent = &info->ulrua.sta[1];
	tbl->dword12 =
	cpu_to_le32(SET_WORD(sta_ent->dropping, FWCMD_ULRUA_DROP) |
		    SET_WORD(sta_ent->tgt_rssi, FWCMD_ULRUA_TGT_RSSI) |
		    SET_WORD(sta_ent->mac_id, FWCMD_ULRUA_MACID) |
		    SET_WORD(sta_ent->ru_pos, FWCMD_ULRUA_RUPOS) |
		    SET_WORD(sta_ent->coding, FWCMD_ULRUA_CODE) |
		    SET_WORD(sta_ent->vip_flag, FWCMD_ULRUA_VIP));

	tbl->dword13 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_ULRUA_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_ULRUA_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_ULRUA_MCS) |
		    SET_WORD(sta_ent->rpt.rt_tblcol, FWCMD_ULRUA_RT_TBLCOL) |
		    SET_WORD(sta_ent->rpt.prtl_alloc, FWCMD_ULRUA_PRTL_ALLOC) |
		    SET_WORD(sta_ent->rpt.rate_chg, FWCMD_ULRUA_RATE_CHG));

	sta_ent = &info->ulrua.sta[2];
	tbl->dword14 =
	cpu_to_le32(SET_WORD(sta_ent->dropping, FWCMD_ULRUA_DROP) |
		    SET_WORD(sta_ent->tgt_rssi, FWCMD_ULRUA_TGT_RSSI) |
		    SET_WORD(sta_ent->mac_id, FWCMD_ULRUA_MACID) |
		    SET_WORD(sta_ent->ru_pos, FWCMD_ULRUA_RUPOS) |
		    SET_WORD(sta_ent->coding, FWCMD_ULRUA_CODE) |
		    SET_WORD(sta_ent->vip_flag, FWCMD_ULRUA_VIP));

	tbl->dword15 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_ULRUA_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_ULRUA_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_ULRUA_MCS) |
		    SET_WORD(sta_ent->rpt.rt_tblcol, FWCMD_ULRUA_RT_TBLCOL) |
		    SET_WORD(sta_ent->rpt.prtl_alloc, FWCMD_ULRUA_PRTL_ALLOC) |
		    SET_WORD(sta_ent->rpt.rate_chg, FWCMD_ULRUA_RATE_CHG));

	sta_ent = &info->ulrua.sta[3];
	tbl->dword16 =
	cpu_to_le32(SET_WORD(sta_ent->dropping, FWCMD_ULRUA_DROP) |
		    SET_WORD(sta_ent->tgt_rssi, FWCMD_ULRUA_TGT_RSSI) |
		    SET_WORD(sta_ent->mac_id, FWCMD_ULRUA_MACID) |
		    SET_WORD(sta_ent->ru_pos, FWCMD_ULRUA_RUPOS) |
		    SET_WORD(sta_ent->coding, FWCMD_ULRUA_CODE) |
		    SET_WORD(sta_ent->vip_flag, FWCMD_ULRUA_VIP));

	tbl->dword17 =
	cpu_to_le32(SET_WORD(sta_ent->rate.dcm, FWCMD_ULRUA_DCM) |
		    SET_WORD(sta_ent->rate.ss, FWCMD_ULRUA_SS) |
		    SET_WORD(sta_ent->rate.mcs, FWCMD_ULRUA_MCS) |
		    SET_WORD(sta_ent->rpt.rt_tblcol, FWCMD_ULRUA_RT_TBLCOL) |
		    SET_WORD(sta_ent->rpt.prtl_alloc, FWCMD_ULRUA_PRTL_ALLOC) |
		    SET_WORD(sta_ent->rpt.rate_chg, FWCMD_ULRUA_RATE_CHG));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}

#if 0
u32 mac_f2p_test_cmd(struct mac_ax_adapter *adapter,
			struct mac_ax_f2p_test_para *info)
{
	u32 ret;
	u8 *buf;
	struct h2c_buf *h2cb;
	struct fwcmd_test_para *tbl;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_test_para));
	if (!buf)
		return MACNPTR;

	tbl = (struct fwcmd_test_para *)buf;
	tbl->dword0 =
	cpu_to_le32(SET_WORD(info->tf_pkt.ul_bw, FWCMD_TBLHDR_RW) |
		    SET_WORD(info->tf_pkt.gi_ltf, FWCMD_TBLHDR_IDX) |
		    SET_WORD(info->tf_pkt.num_he_ltf, FWCMD_TBLHDR_OFFSET) |
		    SET_WORD(info->tf_pkt.ul_stbc, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.doppler, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.ap_tx_power, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.user_num, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.ul_stbc, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.pktNum, FWCMD_TBLHDR_LEN) |
		    SET_WORD(info->tf_pkt.pri20_ch_bitmap, FWCMD_TBLHDR_LEN));

	tbl->dword1 =
	cpu_to_le32(SET_WORD(info->mu_thold_en, FWCMD_MUINFO_THOLDEN) |
		    SET_WORD(info->mu_thold, FWCMD_MUINFO_THOLD) |
		    SET_WORD(info->bypass_thold, FWCMD_BYPASS_THOLD));

	tbl->dword2 =
	cpu_to_le32(SET_WORD(info->fix_mu_en, FWCMD_MUINFO_FIXMU_EN) |
		    SET_WORD(info->fix_mu_macid, FWCMD_MUINFO_FIXMU_ID));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_TBLUD,
			      FWCMD_H2C_FUNC_TBLUD,
			      0,
			      0);
	if (ret)
		return ret;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		return ret;

	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (!ret)
		return ret;

	h2cb_free(adapter, h2cb);

	return 0;
}
#endif