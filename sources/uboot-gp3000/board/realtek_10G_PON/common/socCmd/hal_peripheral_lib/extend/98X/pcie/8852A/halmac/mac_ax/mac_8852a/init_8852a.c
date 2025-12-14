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

#include "init_8852a.h"
#include "../pwr.h"
#include "../efuse.h"
#include "../init.h"
#include "../trxcfg.h"
#include "pwr_seq_8852a.h"
#include "../hw.h"
#include "../trx_desc.h"
#include "../../feature_cfg.h"
#include "../fwcmd.h"
#include "../fwdl.h"
#include "../role.h"
#include "../tblupd.h"
#if MAC_AX_SDIO_SUPPORT
#include "../_sdio.h"
#endif
#if MAC_AX_USB_SUPPORT
#include "../_usb.h"
#endif
#if MAC_AX_PCIE_SUPPORT
#include "../_pcie.h"
#endif
#if MAC_AX_FEATURE_DBGPKG
#include "../dbgpkg.h"
#endif

#if MAC_AX_SDIO_SUPPORT
static struct mac_ax_intf_ops mac8852a_sdio_ops = {
	reg_read8_sdio, /* reg_read8 */
	reg_write8_sdio, /* reg_write8 */
	reg_read16_sdio, /* reg_read16 */
	reg_write16_sdio, /* reg_write16 */
	reg_read32_sdio, /* reg_read32 */
	reg_write32_sdio, /* reg_write32 */
	tx_allow_sdio, /* tx_allow_sdio */
	tx_cmd_addr_sdio, /* tx_cmd_addr_sdio */
	sdio_init, /* intf_init */
};
#endif

#if MAC_AX_USB_SUPPORT
static struct mac_ax_intf_ops mac8852a_usb_ops = {
	reg_read8_usb, /* reg_read8 */
	reg_write8_usb, /* reg_write8 */
	reg_read16_usb, /* reg_read16 */
	reg_write16_usb, /* reg_write16 */
	reg_read32_usb, /* reg_read32 */
	reg_write32_usb, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	NULL, /* intf_init */
};
#endif

#if MAC_AX_PCIE_SUPPORT
static struct mac_ax_intf_ops mac8852a_pcie_ops = {
	reg_read8_pcie, /* reg_read8 */
	reg_write8_pcie, /* reg_write8 */
	reg_read16_pcie, /* reg_read16 */
	reg_write16_pcie, /* reg_write16 */
	reg_read32_pcie, /* reg_read32 */
	reg_write32_pcie, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	NULL, /* intf_init */
};
#endif

static struct mac_ax_ops mac8852a_ops = {
	NULL, /* intf_ops */
	mac_pwr_switch, /* pwr_switch */
	mac_sys_init, /* sys_init */
	mac_trx_init, /* init */
	mac_fwdl, /* fwdl */
	mac_romdl, /* romdl */
	mac_get_hw_info, /* get_hw_info */
	mac_add_role, /* add_role */
	mac_remove_role, /* remove_role */
	mac_txdesc_len, /* txdesc_len */
	mac_build_txdesc, /* build_txdesc */
	mac_parse_rxdesc, /* parse_rxdesc */
	mac_process_c2h, /* process_c2h */
	mac_upd_dlru_fixtbl, /* upd_dlru_fixtbl */
	mac_upd_dlru_grptbl, /* upd_dlru_grptbl */
	mac_upd_ulru_fixtbl, /* upd_dlru_fixtbl */
	mac_upd_ulru_grptbl, /* upd_dlru_grptbl */
	mac_upd_rusta_info, /* upd_rusta_info */
	mac_upd_ba_infotbl, /* upd_ba_infotbl */
	mac_upd_mu_info, /* upd_ba_infotbl */
	mac_upd_ul_fixinfo, /* upd_ul_fixinfo */
	mac_dump_efuse_map_wl, /* dump_wl_efuse*/
	mac_dump_efuse_map_bt, /* dump_bt_efuse */
	mac_write_efuse, /* write_wl_bt_efuse */
	mac_read_efuse, /* read_wl_bt_efuse */
	mac_get_efuse_avl_size, /* get_available_efuse_size */
	mac_dump_log_efuse, /* dump_logical_efuse */
	mac_read_log_efuse, /* read_logical_efuse */
	mac_write_log_efuse, /* write_logical_efuse */
	mac_pg_efuse_by_map, /* program_efuse_map */
	mac_mask_log_efuse, /* mask_logical_efuse_map */
	mac_pg_sec_data_by_map, /* program_secure_data_map */
	mac_cmp_sec_data_by_map, /* compare_secure_data_map */
	mac_gen_tx_frame, /*for gen different type pkt for test*/
#if MAC_AX_FEATURE_DBGPKG
	mac_fwcmd_lb, /* fwcmd_lb */
#endif
#if MAC_AX_FEATURE_HV
#endif
};

static struct mac_ax_hw_info mac8852a_hw_info = {
	0, /* done */
	MAC_AX_CHIP_ID_8852A, /* chip_id */
	0xFF, /* chip_cut */
	MAC_AX_INTF_INVALID, /* intf */
	19, /* tx_ch_num */
	10, /* tx_data_ch_num */
	WD_BODY_LEN, /* wd_body_len */
	WD_INFO_LEN, /* wd_info_len */
	pwr_on_seq_8852a, /* pwr_on_seq */
	pwr_off_seq_8852a, /* pwr_off_seq */
	PWR_SEQ_VER_8852A, /* pwr_seq_ver */
	458752, /* fifo_size */
	128, /* macid_num */
	1536, /* wl_efuse_size */
	1216, /* wl_zone2_efuse_size */
	1024, /* log_efuse_size */
	128, /* bt_efuse_size */
	4, /* sec_ctrl_efuse_size */
	192, /* sec_data_efuse_size */
};

static struct mac_ax_adapter mac_8852a_adapter =  {
	&mac8852a_ops, /* ops */
	NULL, /* drv_adapter */
	NULL, /* pltfm_cb */
	{MAC_AX_PWR_OFF, MAC_AX_FWDL_IDLE, MAC_AX_EFUSE_IDLE}, /* sm */
	&mac8852a_hw_info, /* hw_info */
	NULL, /* fw_info */
	{NULL, NULL, NULL, 0, 0, 0, 0, 0}, /* efuse_param */
	{0, 0}, /* mac_pwr_info */
	{MAC_AX_SDIO_4BYTE_MODE_DISABLE, MAC_AX_SDIO_TX_MODE_AGG,
	512, 0}, /* sdio_info */
};

struct mac_ax_adapter *get_mac_8852a_adapter(enum mac_ax_intf intf,
					     u8 chip_cut, void *drv_adapter,
					     struct mac_ax_pltfm_cb *pltfm_cb)
{
	struct mac_ax_adapter *adapter = &mac_8852a_adapter;

	if (!pltfm_cb)
		return NULL;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		adapter->ops->intf_ops = &mac8852a_sdio_ops;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		adapter->ops->intf_ops = &mac8852a_usb_ops;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		adapter->ops->intf_ops = &mac8852a_pcie_ops;
		break;
#endif
	default:
		return NULL;
	}

	adapter->drv_adapter = drv_adapter;
	adapter->pltfm_cb = pltfm_cb;
	adapter->hw_info->chip_cut = chip_cut;
	adapter->hw_info->intf = intf;
	adapter->hw_info->done = 1;

	return adapter;
}