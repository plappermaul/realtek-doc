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

#include "trxcfg.h"
#include "../mac_reg.h"

#define TRXCFG_WAIT_CNT		2000
#define TRXCFG_BOUND_UNIT	(8 * 1024)

#define TRXCFG_RX_FLTR_OPT	(B_AX_SNIFFER_MODE | B_AX_A_A1_MATCH | \
				 B_AX_A_BC | B_AX_A_MC | \
				 B_AX_A_UC_CAM_MATCH | B_AX_A_BC_CAM_MATCH)

/* control frames: drop, mgnt frames: to host, data frame: to host */
#define TRXCFG_RX_FLTR_CTRL_FRAME	0x55555555
#define TRXCFG_RX_FLTR_MGNT_FRAME	0x55555555
#define TRXCFG_RX_FLTR_DATA_FRAME	0x55555555

/* MPDU Processor Control */
#define TRXCFG_MPDU_PROC_ACT_FRWD	0x02A95A95
#define TRXCFG_MPDU_PROC_TF_FRWD	0x0000AA55
#define TRXCFG_MPDU_PROC_CUT_CTRL	0x010E05F0

static struct dle_size_cfg dle_size0 = {
	64, /* pge_size */
	4032, /* lnk_pge_num */
	64, /* unlnk_pge_num */
};

static struct dle_size_cfg dle_size1 = {
	128, /* pge_size */
	1472, /* lnk_pge_num */
	64, /* unlnk_pge_num */
};

static struct dle_size_cfg dle_size2 = {
	64, /* pge_size */
	704, /* lnk_pge_num */
	64, /* unlnk_pge_num */
};

static struct dle_size_cfg dle_size3 = {
	128, /* pge_size */
	3136, /* lnk_pge_num */
	64, /* unlnk_pge_num */
};

static struct wde_quota_cfg wde_qt0 = {
	3832, /* hif */
	196, /* wcpu */
	2, /* pkt_in */
	2, /* cpu_io */
};

static struct wde_quota_cfg wde_qt1 = {
	512, /* hif */
	196, /* wcpu */
	2, /* pkt_in */
	1, /* cpu_io */
};

static struct ple_quota_cfg ple_qt0 = {
	588, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	16, /* h2c */
	26, /* wcpu */
	26, /* mpdu_proc */
	356, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	1, /* cpu_io */
};

static struct ple_quota_cfg ple_qt1 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	1536, /* c2h */
	16, /* h2c */
	64, /* wcpu */
	1536, /* mpdu_proc */
	1536, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	1, /* cpu_io */
};

static struct ple_quota_cfg ple_qt2 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	16, /* h2c */
	26, /* wcpu */
	26, /* mpdu_proc */
	360, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	1, /* cpu_io */
};

static struct ple_quota_cfg ple_qt3 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	1163, /* c2h */
	16, /* h2c */
	64, /* wcpu */
	1173, /* mpdu_proc */
	1507, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	1, /* cpu_io */
};

#if MAC_AX_PCIE_SUPPORT
static struct dle_mem_cfg dle_mem_pcie[] = {
	{MAC_AX_TRX_NORMAL, /* trx_mode */
	 &dle_size0, &dle_size1, /* wde_size, ple_size */
	 &wde_qt0, &wde_qt0, /* wde_min_qt, wde_max_qt */
	 &ple_qt0, &ple_qt1}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_LOOPBACK, /* trx_mode */
	 &dle_size0, &dle_size1, /* wde_size, ple_size */
	 &wde_qt0, &wde_qt0, /* wde_min_qt, wde_max_qt */
	 &ple_qt0, &ple_qt1}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

#if MAC_AX_USB_SUPPORT
static struct dle_mem_cfg dle_mem_usb[] = {
	{MAC_AX_TRX_NORMAL, /* trx_mode */
	 &dle_size2, &dle_size3, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt2, &ple_qt3}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_LOOPBACK, /* trx_mode */
	 &dle_size2, &dle_size3, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt2, &ple_qt3}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

#if MAC_AX_SDIO_SUPPORT
static struct dle_mem_cfg dle_mem_sdio[] = {
	{MAC_AX_TRX_NORMAL, /* trx_mode */
	 &dle_size2, &dle_size3, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt2, &ple_qt3}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_LOOPBACK, /* trx_mode */
	 &dle_size2, &dle_size3, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt2, &ple_qt3}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_TRX_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

static inline struct dle_mem_cfg *get_dle_mem_cfg(enum mac_ax_intf intf,
						  enum mac_ax_trx_mode mode)
{
	struct dle_mem_cfg *cfg;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		cfg = dle_mem_sdio;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		cfg = dle_mem_usb;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		cfg = dle_mem_pcie;
		break;
#endif
	default:
		cfg = NULL;
		break;
	}

	if (!cfg)
		return NULL;

	for (; cfg->mode != MAC_AX_TRX_INVALID; cfg++) {
		if (cfg->mode == mode)
			return cfg;
	}

	return NULL;
}

static inline u32 dle_used_size(struct dle_size_cfg *wde,
				struct dle_size_cfg *ple)
{
	return wde->pge_size * (wde->lnk_pge_num + wde->unlnk_pge_num) +
		ple->pge_size * (ple->lnk_pge_num + ple->unlnk_pge_num);
}

static inline void dle_func_en(struct mac_ax_adapter *adapter, u8 en)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	if (en)
		val32 |= (B_AX_DLE_WDE_EN | B_AX_DLE_PLE_EN);
	else
		val32 &= ~(B_AX_DLE_WDE_EN | B_AX_DLE_PLE_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);
}

static inline void dle_clk_en(struct mac_ax_adapter *adapter, u8 en)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_DMAC_CLK_EN);
	if (en)
		val32 |= (B_AX_DLE_WDE_CLK_EN | B_AX_DLE_PLE_CLK_EN);
	else
		val32 &= ~(B_AX_DLE_WDE_CLK_EN | B_AX_DLE_PLE_CLK_EN);
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);
}

static inline void dle_size_cfg(struct mac_ax_adapter *adapter,
				struct dle_mem_cfg *cfg)
{
	u8 bound;
	u32 val32;
	struct dle_size_cfg *size_cfg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_WDE_PKTBUF_CFG);
	bound = 0;
	size_cfg = cfg->wde_size;

	switch (size_cfg->pge_size) {
	default:
	case 64:
		val32 = SET_CLR_WORD(val32, S_AX_WDE_PAGE_SEL_64,
				     B_AX_WDE_PAGE_SEL);
		break;
	case 128:
		val32 = SET_CLR_WORD(val32, S_AX_WDE_PAGE_SEL_128,
				     B_AX_WDE_PAGE_SEL);
		break;
	case 256:
		val32 = SET_CLR_WORD(val32, S_AX_WDE_PAGE_SEL_256,
				     B_AX_WDE_PAGE_SEL);
		break;
	}

	val32 = SET_CLR_WORD(val32, bound, B_AX_WDE_START_BOUND);
	val32 = SET_CLR_WORD(val32, size_cfg->lnk_pge_num,
			     B_AX_WDE_FREE_PAGE_NUM);
	MAC_REG_W32(R_AX_WDE_PKTBUF_CFG, val32);

	val32 = MAC_REG_R32(R_AX_PLE_PKTBUF_CFG);
	bound = (size_cfg->lnk_pge_num + size_cfg->unlnk_pge_num)
				* size_cfg->pge_size / TRXCFG_BOUND_UNIT;
	size_cfg = cfg->ple_size;

	switch (size_cfg->pge_size) {
	default:
	case 64:
		val32 = SET_CLR_WORD(val32, S_AX_PLE_PAGE_SEL_64,
				     B_AX_PLE_PAGE_SEL);
		break;
	case 128:
		val32 = SET_CLR_WORD(val32, S_AX_PLE_PAGE_SEL_128,
				     B_AX_PLE_PAGE_SEL);
		break;
	case 256:
		val32 = SET_CLR_WORD(val32, S_AX_PLE_PAGE_SEL_256,
				     B_AX_PLE_PAGE_SEL);
		break;
	}

	val32 = SET_CLR_WORD(val32, bound, B_AX_PLE_START_BOUND);
	val32 = SET_CLR_WORD(val32, size_cfg->lnk_pge_num,
			     B_AX_PLE_FREE_PAGE_NUM);
	MAC_REG_W32(R_AX_PLE_PKTBUF_CFG, val32);
}

static inline void wde_quota_cfg(struct mac_ax_adapter *adapter,
				 struct wde_quota_cfg *min_cfg,
				 struct wde_quota_cfg *max_cfg)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = SET_WORD(min_cfg->hif, B_AX_WDE_Q0_MIN_SIZE) |
		SET_WORD(max_cfg->hif, B_AX_WDE_Q0_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA0_CFG, val32);

	val32 = SET_WORD(min_cfg->wcpu, B_AX_WDE_Q1_MIN_SIZE) |
		SET_WORD(max_cfg->wcpu, B_AX_WDE_Q1_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA1_CFG, val32);

	val32 = SET_WORD(min_cfg->pkt_in, B_AX_WDE_Q3_MIN_SIZE) |
		SET_WORD(max_cfg->pkt_in, B_AX_WDE_Q3_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA3_CFG, val32);

	val32 = SET_WORD(min_cfg->cpu_io, B_AX_WDE_Q4_MIN_SIZE) |
		SET_WORD(max_cfg->cpu_io, B_AX_WDE_Q4_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA4_CFG, val32);
}

static inline void ple_quota_cfg(struct mac_ax_adapter *adapter,
				 struct ple_quota_cfg *min_cfg,
				 struct ple_quota_cfg *max_cfg)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = SET_WORD(min_cfg->cma0_tx, B_AX_PLE_Q0_MIN_SIZE) |
		SET_WORD(max_cfg->cma0_tx, B_AX_PLE_Q0_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA0_CFG, val32);

	//val32 = SET_WORD(min_cfg->cma1_tx, B_AX_PLE_Q0_MIN_SIZE) |
	//	SET_WORD(max_cfg->cma1_tx, B_AX_PLE_Q0_MAX_SIZE);
	//MAC_REG_W32(R_AX_PLE_QTA1_CFG, val32);

	val32 = SET_WORD(min_cfg->c2h, B_AX_PLE_Q2_MIN_SIZE) |
		SET_WORD(max_cfg->c2h, B_AX_PLE_Q2_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA2_CFG, val32);

	val32 = SET_WORD(min_cfg->h2c, B_AX_PLE_Q3_MIN_SIZE) |
		SET_WORD(max_cfg->h2c, B_AX_PLE_Q3_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA3_CFG, val32);

	val32 = SET_WORD(min_cfg->wcpu, B_AX_PLE_Q4_MIN_SIZE) |
		SET_WORD(max_cfg->wcpu, B_AX_PLE_Q4_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA4_CFG, val32);

	val32 = SET_WORD(min_cfg->mpdu_proc, B_AX_PLE_Q5_MIN_SIZE) |
		SET_WORD(max_cfg->mpdu_proc, B_AX_PLE_Q5_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA5_CFG, val32);

	val32 = SET_WORD(min_cfg->cma0_dma, B_AX_PLE_Q6_MIN_SIZE) |
		SET_WORD(max_cfg->cma0_dma, B_AX_PLE_Q6_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA6_CFG, val32);

	val32 = SET_WORD(min_cfg->cma1_dma, B_AX_PLE_Q7_MIN_SIZE) |
		SET_WORD(max_cfg->cma1_dma, B_AX_PLE_Q7_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA7_CFG, val32);

	val32 = SET_WORD(min_cfg->wd_rel, B_AX_PLE_Q9_MIN_SIZE) |
		SET_WORD(max_cfg->wd_rel, B_AX_PLE_Q9_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA9_CFG, val32);

	val32 = SET_WORD(min_cfg->cpu_io, B_AX_PLE_Q10_MIN_SIZE) |
		SET_WORD(max_cfg->cpu_io, B_AX_PLE_Q10_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA10_CFG, val32);
}

static inline void dle_quota_cfg(struct mac_ax_adapter *adapter,
				 struct dle_mem_cfg *cfg)
{
	wde_quota_cfg(adapter, cfg->wde_min_qt, cfg->wde_max_qt);
	ple_quota_cfg(adapter, cfg->ple_min_qt, cfg->ple_max_qt);
}

static u32 sta_sch_init(struct mac_ax_adapter *adapter)
{
	u8 val8;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val8 = MAC_REG_R8(R_AX_SS_CTRL);
	val8 |= B_AX_SS_EN | B_AX_SS_HW_PARAM_EN | B_AX_SS_HW_LEN_EN;
	MAC_REG_W8(R_AX_SS_CTRL, val8);

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if (MAC_REG_R8(R_AX_SS_DBG_0) & B_AX_SS_INIT_DONE)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]STA scheduler init\n");
		return MACPOLLTO;
	}

	return 0;
}

static inline void scheduler_init(struct mac_ax_adapter *adapter)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_CCA_CFG_0);
	val32 &= ~B_AX_BTCCA_EN;
	MAC_REG_W32(R_AX_CCA_CFG_0, val32);
}

static u32 dle_init(struct mac_ax_adapter *adapter,
			enum mac_ax_trx_mode mode)
{
#define WDE_MGN_INI_RDY (B_AX_WDE_Q_MGN_INI_RDY | B_AX_WDE_BUF_MGN_INI_RDY)
#define PLE_MGN_INI_RDY (B_AX_PLE_Q_MGN_INI_RDY | B_AX_PLE_BUF_MGN_INI_RDY)
	u32 ret = 0;
	u32 cnt;
	struct dle_mem_cfg *cfg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	cfg = get_dle_mem_cfg(adapter->hw_info->intf, mode);
	if (!cfg) {
		ret = MACNOITEM;
		goto error;
	}

	if (dle_used_size(cfg->wde_size, cfg->ple_size) !=
		adapter->hw_info->fifo_size) {
		PLTFM_MSG_ERR("[ERR]wd/dle mem cfg\n");
		ret = MACFFCFG;
		goto error;
	}

	dle_func_en(adapter, 0);
	dle_clk_en(adapter, 1);

	dle_size_cfg(adapter, cfg);
	dle_quota_cfg(adapter, cfg);

	dle_func_en(adapter, 1);

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if ((MAC_REG_R32(R_AX_WDE_INI_STATUS) & WDE_MGN_INI_RDY)
		    == WDE_MGN_INI_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]WDE cfg ready\n");
		return MACPOLLTO;
	}

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if ((MAC_REG_R32(R_AX_PLE_INI_STATUS) & PLE_MGN_INI_RDY)
		    == PLE_MGN_INI_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]PLE cfg ready\n");
		return MACPOLLTO;
	}

	return 0;
error:
	dle_func_en(adapter, 0);
	PLTFM_MSG_ERR("[ERR]trxcfg wde 0x8900 = %x\n",
		      MAC_REG_R32(R_AX_WDE_INI_STATUS));
	PLTFM_MSG_ERR("[ERR]trxcfg ple 0x8D00 = %x\n",
		      MAC_REG_R32(R_AX_PLE_INI_STATUS));

	return ret;
#undef WDE_MGN_INI_RDY
#undef PLE_MGN_INI_RDY
}

static void rx_fltr_opt_init(struct mac_ax_adapter *adapter)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_RX_FLTR_OPT);
	val32 |= TRXCFG_RX_FLTR_OPT;
	MAC_REG_W32(R_AX_RX_FLTR_OPT, val32);
}

static void rx_fltr_type_init(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	MAC_REG_W32(R_AX_CTRL_FLTR, TRXCFG_RX_FLTR_CTRL_FRAME);
	MAC_REG_W32(R_AX_MGNT_FLTR, TRXCFG_RX_FLTR_MGNT_FRAME);
	MAC_REG_W32(R_AX_DATA_FLTR, TRXCFG_RX_FLTR_DATA_FRAME);
}

static inline void mpdu_proc_init(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	MAC_REG_W32(R_AX_ACTION_FWD0, TRXCFG_MPDU_PROC_ACT_FRWD);
	MAC_REG_W32(R_AX_TF_FWD, TRXCFG_MPDU_PROC_TF_FRWD);
	val32 = MAC_REG_R32(R_AX_MPDU_PROC);
	val32 |= (B_AX_APPEND_FCS | B_AX_A_ICV_ERR);
	MAC_REG_W32(R_AX_MPDU_PROC, val32);
	MAC_REG_W32(R_AX_CUT_AMSDU_CTRL, TRXCFG_MPDU_PROC_CUT_CTRL);
}

static inline void set_loopback_mode(struct mac_ax_adapter *adapter,u8 en)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_MAC_LOOPBACK);
	if (en)
		val32 |= B_AX_MAC_LOOPBACK_EN;
	else
		val32 &= ~B_AX_MAC_LOOPBACK_EN;
	MAC_REG_W32(R_AX_MAC_LOOPBACK, val32);
}

static inline u32 addr_cam_init(struct mac_ax_adapter *adapter)
{
	u16 val16;
	u32 cnt = TRXCFG_WAIT_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val16 = MAC_REG_R16(R_AX_ADDR_CAM_CTRL);
	val16 |= (B_AX_ADDR_CAM_CLR | B_AX_ADDR_CAM_EN);
	val16 = SET_CLR_WORD(val16, 0, B_AX_ADDR_CAM_RANGE);
	MAC_REG_W32(R_AX_ADDR_CAM_CTRL, val16);

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if (!(MAC_REG_R16(R_AX_ADDR_CAM_CTRL) & B_AX_ADDR_CAM_CLR))
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]ADDR_CAM reset\n");
		return MACPOLLTO;
	}

	/* Use the spcial method to simplify the address CAM circuit using, */
	/* and only for initial development phase. Remove it later. */
	val16 = MAC_REG_R16(R_AX_ADDR_CAM_CTRL);
	val16 &= ~B_AX_ADDR_CAM_EN;
	MAC_REG_W32(R_AX_ADDR_CAM_CTRL, val16);

	return 0;
}

u32 mac_trx_init(struct mac_ax_adapter *adapter, enum mac_ax_trx_mode mode)
{
	u32 ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	/* Check TRX status is idle later. */

	ret = dle_init(adapter, mode);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]DLE init\n");
		return ret;
	}

	sta_sch_init(adapter);
	scheduler_init(adapter);

	ret = addr_cam_init(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]ADDR_CAM reset\n");
		return ret;
	}

	rx_fltr_opt_init(adapter);
	rx_fltr_type_init(adapter);

	mpdu_proc_init(adapter);

	set_loopback_mode(adapter, mode == MAC_AX_TRX_LOOPBACK ? 1 : 0);

	return 0;
}

u32 mac_gen_tx_frame(struct mac_ax_adapter *adapter,
			struct mac_ax_frame_info *info, u8 *mem)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 hdr_len = 0;

	//gen header
	switch(info->type){
	case MAC_AX_PKT_DATA:
		break;
	case MAC_AX_PKT_BASIC_TRIGGER:
		break;
	case MAC_AX_PKT_BQRP:
		break;
	case MAC_AX_PKT_BFRP:
		break;
	case MAC_AX_PKT_NFRP:
		break;
	default:
		PLTFM_MSG_ERR("[ERR]non-support data type \n");
	}

	//gen payload

	//return payload+header size
	return hdr_len+info->pld_len;
}
