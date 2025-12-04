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

#include "mac_ax.h"
#include "mac_ax/init.h"
#include "mac_ax/fwcmd.h"
#include "mac_ax/role.h"
#include "mac_ax/efuse.h"
#if MAC_AX_SDIO_SUPPORT
#include "mac_ax/_sdio.h"
#endif

#define CHIP_ID_HW_DEF_8852A	0xA

static u8 chk_pltfm_cb(void *drv_adapter, enum mac_ax_intf intf,
			 struct mac_ax_pltfm_cb *pltfm_cb)
{
	if (!pltfm_cb)
		return 0;

	if (!pltfm_cb->msg_print)
		return 0;

#if MAC_AX_SDIO_SUPPORT
	if (!pltfm_cb->sdio_cmd52_r8 || !pltfm_cb->sdio_cmd53_r8 ||
	    !pltfm_cb->sdio_cmd53_r16 || !pltfm_cb->sdio_cmd53_r32 ||
	    !pltfm_cb->sdio_cmd53_rn || !pltfm_cb->sdio_cmd52_w8 ||
	    !pltfm_cb->sdio_cmd53_w8 || !pltfm_cb->sdio_cmd53_w16 ||
	    !pltfm_cb->sdio_cmd53_w32 || !pltfm_cb->sdio_cmd53_wn ||
	    !pltfm_cb->sdio_cmd52_cia_r8) {
		pltfm_cb->msg_print(drv_adapter, "[ERR]CB-SDIO\n");
		return 0;
	}
#endif

#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	if (!pltfm_cb->reg_r8 || !pltfm_cb->reg_r16 ||
	    !pltfm_cb->reg_r32 || !pltfm_cb->reg_w8 ||
	    !pltfm_cb->reg_w16 || !pltfm_cb->reg_w32) {
		pltfm_cb->msg_print(drv_adapter, "[ERR]CB-USB or PCIE\n");
		return 0;
	}
#endif
	if (!pltfm_cb->rtl_free || !pltfm_cb->rtl_malloc ||
	    !pltfm_cb->rtl_memcpy || !pltfm_cb->rtl_memset ||
	    !pltfm_cb->rtl_delay_us || !pltfm_cb->rtl_delay_ms ||
	    !pltfm_cb->mutex_init || !pltfm_cb->mutex_deinit ||
	    !pltfm_cb->mutex_lock || !pltfm_cb->mutex_unlock ||
	    !pltfm_cb->msg_print) {
		pltfm_cb->msg_print(drv_adapter, "[ERR]CB-OS\n");
		return 0;
	}

	return 1;
}

static u8 chk_pltfm_endian(void)
{
	u32 num = 1;
	u8 *num_ptr = (u8*)&num;

	if (*num_ptr != MAC_AX_IS_LITTLE_ENDIAN)
		return 0;

	return 1;
}

static u32 get_chip_info(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
			 enum mac_ax_intf intf, u8 *id, u8 *cut)
{
	u8 cur_id;

	if (!cut || !id)
		return MACNPTR;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		cur_id = pltfm_cb->sdio_cmd52_r8(drv_adapter, R_AX_SYS_CFG2);
		*cut = pltfm_cb->sdio_cmd52_r8(drv_adapter,
					       R_AX_SYS_CFG1 + 1) >> 4;
		break;
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	case MAC_AX_INTF_USB:
	case MAC_AX_INTF_PCIE:
		cur_id = pltfm_cb->reg_r8(drv_adapter, R_AX_SYS_CFG2);
		*cut = pltfm_cb->reg_r8(drv_adapter, R_AX_SYS_CFG1 + 1) >> 4;
		break;
#endif
	default:
		return MACINTF;
	}

	switch (cur_id) {
	case CHIP_ID_HW_DEF_8852A:
		*id = MAC_AX_CHIP_ID_8852A;
		break;
	default:
		return MACCHIPID;
	}

	return 0;
}

u32 mac_ax_ops_init(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
		    enum mac_ax_intf intf,
		    struct mac_ax_adapter **mac_adapter,
		    struct mac_ax_ops **mac_ops)
{
	u32 ret;
	u8 chip_id;
	u8 chip_cut;
	struct mac_ax_adapter *adapter;

	if (!chk_pltfm_cb(drv_adapter, intf, pltfm_cb))
		return MACPFCB;

	if (!chk_pltfm_endian())
		return MACPFED;

	pltfm_cb->msg_print(drv_adapter,
			    "MAC_AX_MAJOR_VER = %d\n"
			    "MAC_AX_CHIP_VER = %d\n"
			    "MAC_AX_SUB_VER = %d\n"
			    "MAC_AX_PATCH_VER = %d\n",
			    MAC_AX_MAJOR_VER, MAC_AX_CHIP_VER,
			    MAC_AX_SUB_VER, MAC_AX_PATCH_VER);
#if 0
	ret = get_chip_info(drv_adapter, pltfm_cb, intf, &chip_id, &chip_ver);
	if (ret)
		return ret;
#else
	ret = 0;
	chip_id = MAC_AX_CHIP_ID_8852A;
	chip_cut = MAC_AX_CHIP_CUT_A;
#endif
	adapter = get_mac_ax_adapter(intf, chip_id, chip_cut, drv_adapter,
				     pltfm_cb);
	if (!adapter) {
		pltfm_cb->msg_print(drv_adapter, "[ERR]Get MAC adapter\n");
		return MACADAPTER;
	}

	*mac_adapter = adapter;
	*mac_ops = adapter->ops;

	ret = h2cb_init(adapter);
	if (ret)
		return ret;

	ret = role_tbl_init(adapter);
	if (ret)
		return ret;

	ret = efuse_tbl_init(adapter);
	if (ret)
		return ret;
#if MAC_AX_SDIO_SUPPORT
	ret = sdio_tbl_init(adapter);
	if (ret)
		return ret;
#endif
	return 0;
}

u32 mac_ax_ops_exit(struct mac_ax_adapter *adapter)
{
	u32 ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	ret = h2cb_exit(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]h2c buffer exit\n");
		return ret;
	}

	ret = role_tbl_exit(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]role table exit\n");
		return ret;
	}

	ret = efuse_tbl_exit(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]efuse table exit\n");
		return ret;
	}
#if MAC_AX_SDIO_SUPPORT
	ret = sdio_tbl_exit(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]efuse table exit\n");
		return ret;
	}
#endif
	if (efuse_param->efuse_map) {
		PLTFM_FREE(efuse_param->efuse_map,
			   adapter->hw_info->efuse_size);
		efuse_param->efuse_map = (u8 *)NULL;
	}

	if (efuse_param->bt_efuse_map) {
		PLTFM_FREE(efuse_param->bt_efuse_map,
			   adapter->hw_info->bt_efuse_size);
		efuse_param->bt_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->log_efuse_map) {
		PLTFM_FREE(efuse_param->log_efuse_map,
			   adapter->hw_info->bt_efuse_size);
		efuse_param->log_efuse_map = (u8 *)NULL;
	}

	if (adapter->fw_info) {
		PLTFM_FREE(adapter->fw_info, sizeof(struct mac_ax_fw_info));
		adapter->fw_info = NULL;
	}

	return 0;
}
