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
#include "pwr.h"
#include "state_mach.h"
#include "../mac_reg.h"

static u32 pwr_cmd_poll(struct mac_ax_adapter *adapter, struct mac_pwr_cfg *seq)
{
	u8 val;
	u32 addr;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	cnt = PWR_POLL_CNT;
	addr = seq->addr;

	if (seq->base == PWR_INTF_MSK_SDIO)
		addr = seq->addr | SDIO_LOCAL_BASE_ADDR;

	while (cnt--) {
		val = MAC_REG_R8(addr);
		val &= seq->msk;
		if (val == (seq->val & seq->msk))
			return 0;
		PLTFM_DELAY_MS(300);
	}

	PLTFM_MSG_ERR("[ERR] Polling timeout\n");
	PLTFM_MSG_ERR("[ERR] addr: %X, %X\n", addr, seq->addr);
	PLTFM_MSG_ERR("[ERR] val: %X, %X\n", val, seq->val);

	return MACPOLLTO;
}

static u32 sub_pwr_seq_start(struct mac_ax_adapter *adapter,
			     u8 cut_msk, u8 intf_msk, struct mac_pwr_cfg *seq)
{
	u8 val;
	u32 addr;
	u32 ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	do {
		if (!(seq->intf_msk & intf_msk) || !(seq->cut_msk & cut_msk))
			goto next_seq;

		switch (seq->cmd) {
		case PWR_CMD_WRITE:
			addr = seq->addr;

			if (seq->base == PWR_BASE_SDIO)
				addr |= SDIO_LOCAL_BASE_ADDR;

			val = MAC_REG_R8(addr);
			val &= ~(seq->msk);
			val |= (seq->val & seq->msk);

			MAC_REG_W8(addr, val);
			break;
		case PWR_CMD_POLL:
			ret = pwr_cmd_poll(adapter, seq);
			if (ret)
				return ret;
			break;
		case PWR_CMD_DELAY:
			if (seq->val == PWR_DELAY_US)
				PLTFM_DELAY_US(seq->addr);
			else
				PLTFM_DELAY_MS(seq->addr);
			break;
		case PWR_CMD_END:
			return 0;
		default:
			return MACNOITEM;
		}
next_seq:
		seq++;
	} while (1);

	return MACPWRSW;
}

static u32 pwr_seq_start(struct mac_ax_adapter *adapter,
			 struct mac_pwr_cfg **seq)
{
	u8 cut;
	u8 intf;
	u32 ret;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	struct mac_pwr_cfg *sub_seq = *seq;

	switch (hw_info->chip_cut) {
	case MAC_AX_CHIP_CUT_A:
		cut = PWR_CUT_MSK_A;
		break;
	case MAC_AX_CHIP_CUT_B:
		cut = PWR_CUT_MSK_B;
		break;
	case MAC_AX_CHIP_CUT_C:
		cut = PWR_CUT_MSK_C;
		break;
	case MAC_AX_CHIP_CUT_D:
		cut = PWR_CUT_MSK_D;
		break;
	case MAC_AX_CHIP_CUT_E:
		cut = PWR_CUT_MSK_E;
		break;
	case MAC_AX_CHIP_CUT_F:
		cut = PWR_CUT_MSK_F;
		break;
	case MAC_AX_CHIP_CUT_G:
		cut = PWR_CUT_MSK_G;
		break;
	case MAC_AX_CHIP_CUT_TEST:
		cut = PWR_CUT_MSK_TEST;
		break;
	default:
		PLTFM_MSG_ERR("[ERR]cut version\n");
		return MACNOITEM;
	}

	switch (hw_info->intf) {
	case MAC_AX_INTF_SDIO:
		intf = PWR_INTF_MSK_SDIO;
		break;
	case MAC_AX_INTF_USB:
		intf = PWR_INTF_MSK_USB;
		break;
	case MAC_AX_INTF_PCIE:
		intf = PWR_INTF_MSK_PCIE;
		break;
	default:
		PLTFM_MSG_ERR("[ERR]interface\n");
		return MACNOITEM;
	}

	while (sub_seq) {
		ret = sub_pwr_seq_start(adapter, cut, intf, sub_seq);
		if (ret)
			return ret;
		seq++;
		sub_seq = *seq;
	}

	return 0;
}

u32 mac_pwr_switch(struct mac_ax_adapter *adapter, u8 on)
{
	u32 ret;
	struct mac_pwr_cfg **pwr_seq;

	adapter->mac_pwr_info.pwr_seq_proc = 1;

	if (on)
		pwr_seq = adapter->hw_info->pwr_on_seq;
	else
		pwr_seq = adapter->hw_info->pwr_off_seq;

	ret = pwr_seq_start(adapter, pwr_seq);
	if (ret) {
		adapter->sm.pwr = MAC_AX_PWR_ERR;
		return ret;
	}

	if (on)
		adapter->sm.pwr = MAC_AX_PWR_ON;
	else
		adapter->sm.pwr = MAC_AX_PWR_OFF;

	adapter->mac_pwr_info.pwr_seq_proc = 0;
	return 0;
}