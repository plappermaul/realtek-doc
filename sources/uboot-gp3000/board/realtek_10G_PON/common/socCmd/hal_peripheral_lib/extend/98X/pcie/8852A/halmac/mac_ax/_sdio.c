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
#include "_sdio.h"
#include "state_mach.h"
#include "../mac_reg.h"

static struct mac_sdio_tbl sdio_tbl;

static u8 r_indir_cmd52_sdio(struct mac_ax_adapter *adapter, u32 adr);
static u32 r_indir_cmd53_sdio(struct mac_ax_adapter *adapter, u32 adr);
static u32 r8_indir_sdio(struct mac_ax_adapter *adapter, u32 adr);
static u32 r16_indir_sdio(struct mac_ax_adapter *adapter, u32 adr);
static u32 r32_indir_sdio(struct mac_ax_adapter *adapter, u32 adr);
static u32 r_indir_sdio(struct mac_ax_adapter *adapter, u32 adr,
			enum sdio_io_size size);
static void w_indir_cmd52_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			       enum sdio_io_size size);
static void w_indir_cmd53_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			       enum sdio_io_size size);
static void w8_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val);
static void w16_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val);
static void w32_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val);
static void w_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			 enum sdio_io_size size);
static u32 cnv_to_sdio_bus_offset(struct mac_ax_adapter *adapter, u32 *adr);

u8 reg_read8_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 val8;
	enum mac_ax_sdio_4byte_mode cmd53_4byte = adapter->sdio_info.sdio_4byte;

	if ((adr & WLAN_IOREG_MSK) == 0) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		if (adapter->mac_pwr_info.pwr_seq_proc == 1 ||
		    adapter->sm.pwr == MAC_AX_PWR_OFF ||
		    adapter->mac_pwr_info.pwr_in_lps == 1 ||
		    cmd53_4byte == MAC_AX_SDIO_4BYTE_MODE_RW ||
		    cmd53_4byte == MAC_AX_SDIO_4BYTE_MODE_R) {
			val8 = (u8)r_indir_sdio(adapter, adr, SDIO_IO_BYTE);
		} else {
			val8 = (u8)PLTFM_SDIO_CMD53_R8(adr);
}
	} else {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		val8 = PLTFM_SDIO_CMD52_R8(adr);
	}

	return val8;
}

void reg_write8_sdio(struct mac_ax_adapter *adapter, u32 adr, u8 val)
{
	enum mac_ax_sdio_4byte_mode cmd53_4byte = adapter->sdio_info.sdio_4byte;

	if ((adapter->mac_pwr_info.pwr_seq_proc == 1 ||
	    adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1 ||
	    cmd53_4byte == MAC_AX_SDIO_4BYTE_MODE_RW ||
	    cmd53_4byte == MAC_AX_SDIO_4BYTE_MODE_R) &&
	    (adr & WLAN_IOREG_MSK) == 0) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		w_indir_sdio(adapter, adr, val, SDIO_IO_BYTE);
	} else {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		PLTFM_SDIO_CMD52_W8(adr, val);
	}
}

u16 reg_read16_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	union {
		__le16 word;
		u8 byte[2];
	} value16 = { 0x0000 };

	if ((adr & WLAN_IOREG_MSK) == 0 &&
	    (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1)) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		value16.byte[0] = (u8)r_indir_sdio(adapter, adr, SDIO_IO_BYTE);
		value16.byte[1] = (u8)r_indir_sdio(adapter, adr + 1,
						   SDIO_IO_BYTE);
		return le16_to_cpu(value16.word);
	} else if ((adr & WLAN_IOREG_MSK) != 0 &&
		   (adapter->sm.pwr == MAC_AX_PWR_OFF ||
		   adapter->mac_pwr_info.pwr_in_lps == 1)) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		value16.byte[0] = PLTFM_SDIO_CMD52_R8(adr);
		value16.byte[1] = PLTFM_SDIO_CMD52_R8(adr + 1);
		return le16_to_cpu(value16.word);
	}

	if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	if (((adr & (2 - 1)) != 0) ||
	    adapter->sdio_info.sdio_4byte == MAC_AX_SDIO_4BYTE_MODE_RW ||
	    adapter->sdio_info.sdio_4byte == MAC_AX_SDIO_4BYTE_MODE_R) {
		value16.byte[0] = (u8)PLTFM_SDIO_CMD53_R32(adr);
		value16.byte[1] = (u8)PLTFM_SDIO_CMD53_R32(adr + 1);
		return le16_to_cpu(value16.word);
}

	return PLTFM_SDIO_CMD53_R16(adr);
}

void reg_write16_sdio(struct mac_ax_adapter *adapter, u32 adr, u16 val)
{
	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1 ||
	    ((adr & (2 - 1)) != 0) ||
	    adapter->sdio_info.sdio_4byte == MAC_AX_SDIO_4BYTE_MODE_RW ||
	    adapter->sdio_info.sdio_4byte == MAC_AX_SDIO_4BYTE_MODE_W) {
		if ((adr & WLAN_IOREG_MSK) == 0 && ((adr & (2 - 1)) == 0)) {
			if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
				PLTFM_MSG_ERR("[ERR]convert adr\n");

			w_indir_sdio(adapter, adr, val, SDIO_IO_WORD);
		} else {
			if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
				PLTFM_MSG_ERR("[ERR]convert adr\n");

			PLTFM_SDIO_CMD52_W8(adr, (u8)(val & 0xFF));
			PLTFM_SDIO_CMD52_W8(adr + 1,
					    (u8)((val & 0xFF00) >> 8));
	}
	} else {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		PLTFM_SDIO_CMD53_W16(adr, val);
	}
}

u32 reg_read32_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	union {
		__le32 dword;
		u8 byte[4];
	} value32 = { 0x00000000 };

	if (((adr & WLAN_IOREG_MSK) == 0) &&
	    (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1)) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		value32.byte[0] = (u8)r_indir_sdio(adapter, adr, SDIO_IO_BYTE);
		value32.byte[1] = (u8)r_indir_sdio(adapter, adr + 1,
						   SDIO_IO_BYTE);
		value32.byte[2] = (u8)r_indir_sdio(adapter, adr + 2,
						   SDIO_IO_BYTE);
		value32.byte[3] = (u8)r_indir_sdio(adapter, adr + 3,
						   SDIO_IO_BYTE);
		return le32_to_cpu(value32.dword);
	} else if (((adr & WLAN_IOREG_MSK) != 0) &&
		   (adapter->sm.pwr == MAC_AX_PWR_OFF ||
		   adapter->mac_pwr_info.pwr_in_lps == 1)) {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		value32.byte[0] = PLTFM_SDIO_CMD52_R8(adr);
		value32.byte[1] = PLTFM_SDIO_CMD52_R8(adr + 1);
		value32.byte[2] = PLTFM_SDIO_CMD52_R8(adr + 2);
		value32.byte[3] = PLTFM_SDIO_CMD52_R8(adr + 3);
		return le32_to_cpu(value32.dword);
	}

	if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	if ((adr & (4 - 1)) != 0) {
		value32.byte[0] = (u8)PLTFM_SDIO_CMD53_R32(adr);
		value32.byte[1] = (u8)PLTFM_SDIO_CMD53_R32(adr + 1);
		value32.byte[2] = (u8)PLTFM_SDIO_CMD53_R32(adr + 2);
		value32.byte[3] = (u8)PLTFM_SDIO_CMD53_R32(adr + 3);
		return le32_to_cpu(value32.dword);
	}

	return PLTFM_SDIO_CMD53_R32(adr);
}

void reg_write32_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val)
{
	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1 ||
	    (adr & (4 - 1)) !=  0) {
		if ((adr & WLAN_IOREG_MSK) == 0 && ((adr & (4 - 1)) == 0)) {
			if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
				PLTFM_MSG_ERR("[ERR]convert adr\n");

			w_indir_sdio(adapter, adr, val, SDIO_IO_DWORD);
		} else {
			if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
				PLTFM_MSG_ERR("[ERR]convert adr\n");

			PLTFM_SDIO_CMD52_W8(adr, (u8)(val & 0xFF));
			PLTFM_SDIO_CMD52_W8(adr + 1, (u8)((val >> 8) & 0xFF));
			PLTFM_SDIO_CMD52_W8(adr + 2, (u8)((val >> 16) & 0xFF));
			PLTFM_SDIO_CMD52_W8(adr + 3, (u8)((val >> 24) & 0xFF));
	}
	} else {
		if (cnv_to_sdio_bus_offset(adapter, &adr) != 0)
			PLTFM_MSG_ERR("[ERR]convert adr\n");

		PLTFM_SDIO_CMD53_W32(adr, val);
	}
}

u32 tx_allow_sdio(struct mac_ax_adapter *adapter,
		  struct mac_ax_sdio_tx_info *info)
{
	return 0;
}

u32 tx_cmd_addr_sdio(struct mac_ax_adapter *adapter,
		     struct mac_ax_sdio_tx_info *info, u32 *cmd_addr)
{
	struct mac_ax_sdio_info *sdio_info = &adapter->sdio_info;
	u16 block_size = sdio_info->block_size;
	u32 len_unit1, len_unit8, val32, size;

	size = info->size;

	if (size == 0) {
		PLTFM_MSG_ERR("size is 0!!\n");
		return MACBUFSZ;
	}

	if (info->ch_dma < 13)
		*cmd_addr = info->ch_dma << SDIO_CMD_ADDR_TXFF_SHIFT;
	else
		return MACTXCHDMA;

	len_unit8 = (size >> 3) + ((size & (8 - 1)) ? 1 : 0);
	len_unit1 = (len_unit8 << 3);

	if (sdio_info->tx_mode == MAC_AX_SDIO_TX_MODE_AGG ||
	    (sdio_info->tx_mode == MAC_AX_SDIO_TX_MODE_DUMMY_AUTO &&
	     len_unit1 < block_size)) {
		*cmd_addr |= len_unit8 & SDIO_8BYTE_LEN_MASK;
	} else if (sdio_info->tx_mode == MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK ||
		   (sdio_info->tx_mode == MAC_AX_SDIO_TX_MODE_DUMMY_AUTO &&
		    len_unit1 >= block_size)) {
		val32 = len_unit1 % block_size;
		if (val32)
			val32 = (block_size - val32) >> 2;
		*cmd_addr = sdio_info->tx_seq | *cmd_addr |
				((val32 & SDIO_4BYTE_LEN_MASK) << 1);
		sdio_info->tx_seq = ~sdio_info->tx_seq & 0x01;
	} else {
		PLTFM_MSG_ERR("[ERR]tx_mode is undefined\n");
	}

	return 0;
}

u32 sdio_init(struct mac_ax_adapter *adapter)
{
	if (adapter->hw_info->intf != MAC_AX_INTF_SDIO)
		return MACINTF;

	return 0;
}

u32 sdio_tbl_init(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_INIT(&sdio_tbl.lock);

	return 0;
}

u32 sdio_tbl_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_DEINIT(&sdio_tbl.lock);

	return 0;
}

static u8 r_indir_cmd52_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 tmp;
	u32 reg_addr = R_AX_SDIO_INDIRECT_ADDR;
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;
	u32 reg_cfg = R_AX_SDIO_INDIRECT_CTRL;
	u32 cnt;

	if (cnv_to_sdio_bus_offset(adapter, &reg_addr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_cfg) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	PLTFM_SDIO_CMD52_W8(reg_addr, (u8)adr);
	PLTFM_SDIO_CMD52_W8(reg_addr + 1, (u8)(adr >> 8));
	PLTFM_SDIO_CMD52_W8(reg_addr + 2, (u8)(adr >> 16));
	PLTFM_SDIO_CMD52_W8(reg_addr + 3,
			    (u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	PLTFM_SDIO_CMD52_W8(reg_cfg, (u8)B_AX_INDIRECT_REG_R);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(reg_addr + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD52 read\n");

	return PLTFM_SDIO_CMD52_R8(reg_data);
}

static u32 r_indir_cmd53_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 val[12];
	u32 cnt, i;
	u32 reg_addr = R_AX_SDIO_INDIRECT_ADDR;
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;
	u32 reg_cfg = R_AX_SDIO_INDIRECT_CTRL;

	union {
		__le32 dword;
		u8 byte[4];
	} value32 = { 0x00000000 };

	if (cnv_to_sdio_bus_offset(adapter, &reg_addr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_cfg) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	for (i = 0; i < 3; i++)
		*(val + i) = (u8)(adr >> (i << 3));
	*(val + 3) = (u8)((adr | B_AX_INDIRECT_RDY) >> 24);
	*(val + 8) = (u8)(B_AX_INDIRECT_REG_R);
	PLTFM_SDIO_CMD53_WN(reg_addr, sizeof(val), val);

	cnt = SDIO_WAIT_CNT;
	do {
		PLTFM_SDIO_CMD53_RN(reg_addr + 3, 8, val);
		cnt--;
	} while (((val[0] & BIT(7)) == 0) && (cnt > 0));

	if (((val[0] & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD53 read\n");

	value32.byte[0] = val[1];
	value32.byte[1] = val[2];
	value32.byte[2] = val[3];
	value32.byte[3] = val[4];

	return le32_to_cpu(value32.dword);
}

static u32 r8_indir_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	union {
		__le32 dword;
		u8 byte[4];
	} val32 = { 0x00000000 };

	if (adapter->mac_pwr_info.pwr_seq_proc == 1 ||
	    adapter->mac_pwr_info.pwr_in_lps == 1 ||
	    adapter->sm.pwr == MAC_AX_PWR_OFF) {
		val32.byte[0] = r_indir_cmd52_sdio(adapter, adr);
		return le32_to_cpu(val32.dword);
	}

	return r_indir_cmd53_sdio(adapter, adr);
}

static u32 r16_indir_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;

	union {
		__le32 dword;
		u8 byte[4];
	} val32 = { 0x00000000 };

	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1) {
		if (0 != (adr & (2 - 1))) {
			val32.byte[0] = r_indir_cmd52_sdio(adapter, adr);
			val32.byte[1] = r_indir_cmd52_sdio(adapter, adr + 1);
		} else {
			val32.byte[0] = r_indir_cmd52_sdio(adapter, adr);
			val32.byte[1] = PLTFM_SDIO_CMD52_R8(reg_data + 1);
		}

		return  le32_to_cpu(val32.dword);
	}

	if (0 != (adr & (2 - 1))) {
		val32.byte[0] = (u8)r_indir_cmd53_sdio(adapter, adr);
		val32.byte[1] = (u8)r_indir_cmd53_sdio(adapter, adr + 1);

		return le32_to_cpu(val32.dword);
	}

	return r_indir_cmd53_sdio(adapter, adr);
}

static u32 r32_indir_sdio(struct mac_ax_adapter *adapter, u32 adr)
{
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;

	union {
		__le32 dword;
		u8 byte[4];
	} val32 = { 0x00000000 };

	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1) {
		if (0 != (adr & (4 - 1))) {
			val32.byte[0] = r_indir_cmd52_sdio(adapter, adr);
			val32.byte[1] = r_indir_cmd52_sdio(adapter, adr + 1);
			val32.byte[2] = r_indir_cmd52_sdio(adapter, adr + 2);
			val32.byte[3] = r_indir_cmd52_sdio(adapter, adr + 3);
		} else {
			val32.byte[0] = r_indir_cmd52_sdio(adapter, adr);
			val32.byte[1] = PLTFM_SDIO_CMD52_R8(reg_data + 1);
			val32.byte[2] = PLTFM_SDIO_CMD52_R8(reg_data + 2);
			val32.byte[3] = PLTFM_SDIO_CMD52_R8(reg_data + 3);
		}

		return le32_to_cpu(val32.dword);
	}

	if (0 != (adr & (4 - 1))) {
		val32.byte[0] = (u8)r_indir_cmd53_sdio(adapter, adr);
		val32.byte[1] = (u8)r_indir_cmd53_sdio(adapter, adr + 1);
		val32.byte[2] = (u8)r_indir_cmd53_sdio(adapter, adr + 2);
		val32.byte[3] = (u8)r_indir_cmd53_sdio(adapter, adr + 3);

		return le32_to_cpu(val32.dword);
	}

	return r_indir_cmd53_sdio(adapter, adr);
}

static u32 r_indir_sdio(struct mac_ax_adapter *adapter, u32 adr,
			enum sdio_io_size size)
{
	u32 value32 = 0;

	PLTFM_MUTEX_LOCK(&sdio_tbl.lock);

	switch (size) {
	case SDIO_IO_BYTE:
		value32 = r8_indir_sdio(adapter, adr);
		break;
	case SDIO_IO_WORD:
		value32 = r16_indir_sdio(adapter, adr);
		break;
	case SDIO_IO_DWORD:
		value32 = r32_indir_sdio(adapter, adr);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	PLTFM_MUTEX_UNLOCK(&sdio_tbl.lock);

	return value32;
}

static void w_indir_cmd52_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			       enum sdio_io_size size)
{
	u8 tmp;
	u32 reg_addr = R_AX_SDIO_INDIRECT_ADDR;
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;
	u32 reg_cfg = R_AX_SDIO_INDIRECT_CTRL;
	u32 cnt;

	if (cnv_to_sdio_bus_offset(adapter, &reg_addr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_cfg) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	PLTFM_SDIO_CMD52_W8(reg_addr, (u8)adr);
	PLTFM_SDIO_CMD52_W8(reg_addr + 1, (u8)(adr >> 8));
	PLTFM_SDIO_CMD52_W8(reg_addr + 2, (u8)(adr >> 16));
	PLTFM_SDIO_CMD52_W8(reg_addr + 3,
			    (u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	switch (size) {
	case SDIO_IO_BYTE:
		PLTFM_SDIO_CMD52_W8(reg_data, (u8)val);
		PLTFM_SDIO_CMD52_W8(reg_cfg, (u8)(B_AX_INDIRECT_REG_W));
		break;
	case SDIO_IO_WORD:
		PLTFM_SDIO_CMD52_W8(reg_data, (u8)val);
		PLTFM_SDIO_CMD52_W8(reg_data + 1, (u8)(val >> 8));
		PLTFM_SDIO_CMD52_W8(reg_cfg, (u8)(B_AX_INDIRECT_REG_W | 0x1));
		break;
	case SDIO_IO_DWORD:
		PLTFM_SDIO_CMD52_W8(reg_data, (u8)val);
		PLTFM_SDIO_CMD52_W8(reg_data + 1, (u8)(val >> 8));
		PLTFM_SDIO_CMD52_W8(reg_data + 2, (u8)(val >> 16));
		PLTFM_SDIO_CMD52_W8(reg_data + 3, (u8)(val >> 24));
		PLTFM_SDIO_CMD52_W8(reg_cfg, (u8)(B_AX_INDIRECT_REG_W | 0x2));
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(reg_addr + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD52 write\n");
}

static void w_indir_cmd53_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			       enum sdio_io_size size)
{
	u8 value[12];
	u8 tmp;
	u32 reg_addr = R_AX_SDIO_INDIRECT_ADDR;
	u32 reg_data = R_AX_SDIO_INDIRECT_DATA;
	u32 reg_cfg = R_AX_SDIO_INDIRECT_CTRL;
	u32 cnt, i;

	if (cnv_to_sdio_bus_offset(adapter, &reg_addr) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_data) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");
	if (cnv_to_sdio_bus_offset(adapter, &reg_cfg) != 0)
		PLTFM_MSG_ERR("[ERR]convert adr\n");

	switch (size) {
	case SDIO_IO_BYTE:
		*(value + 8) = (u8)B_AX_INDIRECT_REG_W;
		break;
	case SDIO_IO_WORD:
		*(value + 8) = (u8)(B_AX_INDIRECT_REG_W | 0x1);
		break;
	case SDIO_IO_DWORD:
		*(value + 8) = (u8)(B_AX_INDIRECT_REG_W | 0x2);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	for (i = 0; i < 3; i++)
		*(value + i) = (u8)(adr >> (i << 3));
	*(value + 3) = (u8)((adr | B_AX_INDIRECT_RDY) >> 24);
	for (i = 4; i < 8; i++)
		*(value + i) = (u8)(val >> (i << 3));
	for (i = 9; i < 12; i++)
		*(value + i) = 0;
	PLTFM_SDIO_CMD53_WN(reg_addr, sizeof(value), value);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(reg_addr + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD53 read\n");
}

static void w8_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val)
{
	if (adapter->mac_pwr_info.pwr_seq_proc == 1 ||
	    adapter->mac_pwr_info.pwr_in_lps == 1 ||
	    adapter->sm.pwr == MAC_AX_PWR_OFF)
		w_indir_cmd52_sdio(adapter, adr, val, SDIO_IO_BYTE);
	else
		w_indir_cmd53_sdio(adapter, adr, val, SDIO_IO_BYTE);
}

static void w16_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val)
{
	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1) {
		if (0 != (adr & (2 - 1))) {
			w_indir_cmd52_sdio(adapter, adr, val, SDIO_IO_BYTE);
			w_indir_cmd52_sdio(adapter, adr + 1, val >> 8,
					   SDIO_IO_BYTE);
		} else {
			w_indir_cmd52_sdio(adapter, adr, val, SDIO_IO_WORD);
		}
	} else {
		if (0 != (adr & (2 - 1))) {
			w_indir_cmd53_sdio(adapter, adr, val, SDIO_IO_BYTE);
			w_indir_cmd53_sdio(adapter, adr + 1, val >> 8,
					   SDIO_IO_BYTE);
		} else {
			w_indir_cmd53_sdio(adapter, adr, val, SDIO_IO_WORD);
		}
	}
}

static void w32_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val)
{
	if (adapter->sm.pwr == MAC_AX_PWR_OFF ||
	    adapter->mac_pwr_info.pwr_in_lps == 1) {
		if (0 != (adr & (4 - 1))) {
			w_indir_cmd52_sdio(adapter, adr, val, SDIO_IO_BYTE);
			w_indir_cmd52_sdio(adapter, adr + 1, val >> 8,
					   SDIO_IO_BYTE);
			w_indir_cmd52_sdio(adapter, adr + 2, val >> 16,
					   SDIO_IO_BYTE);
			w_indir_cmd52_sdio(adapter, adr + 3, val >> 24,
					   SDIO_IO_BYTE);
		} else {
			w_indir_cmd52_sdio(adapter, adr, val, SDIO_IO_DWORD);
		}
	} else {
		if (0 != (adr & (4 - 1))) {
			w_indir_cmd53_sdio(adapter, adr, val, SDIO_IO_BYTE);
			w_indir_cmd53_sdio(adapter, adr + 1, val >> 8,
					   SDIO_IO_BYTE);
			w_indir_cmd53_sdio(adapter, adr + 2, val >> 16,
					   SDIO_IO_BYTE);
			w_indir_cmd53_sdio(adapter, adr + 3, val >> 24,
					   SDIO_IO_BYTE);
		} else {
			w_indir_cmd53_sdio(adapter, adr, val, SDIO_IO_DWORD);
		}
	}
}

static void w_indir_sdio(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			 enum sdio_io_size size)
{
	PLTFM_MUTEX_LOCK(&sdio_tbl.lock);

	switch (size) {
	case SDIO_IO_BYTE:
		w8_indir_sdio(adapter, adr, val);
		break;
	case SDIO_IO_WORD:
		w16_indir_sdio(adapter, adr, val);
		break;
	case SDIO_IO_DWORD:
		w32_indir_sdio(adapter, adr, val);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	PLTFM_MUTEX_UNLOCK(&sdio_tbl.lock);
}

static u32 cnv_to_sdio_bus_offset(struct mac_ax_adapter *adapter, u32 *adr)
{
	switch ((*adr) & WLAN_IOREG_MSK) {
	case WLAN_IOREG_BASE:
		break;
	case SDIO_LOCAL_BASE:
		*adr &= SDIO_LOCAL_MSK;
		*adr |= SDIO_LOCAL_SHIFT;
		break;
	default:
		*adr = 0xFFFFFFFF;
		PLTFM_MSG_ERR("[ERR]base address!! 0x%X\n", (*adr));
		return MACBADDR;
	}

	return 0;
}
