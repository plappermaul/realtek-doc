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
#include "efuse.h"
#include "state_mach.h"
#include "../mac_reg.h"

static struct mac_efuse_tbl efuse_tbl;

static u32 efuse_map_init(struct mac_ax_adapter *adapter,
			  enum efuse_map_sel map_sel);
static u32 efuse_fwcmd_ck(struct mac_ax_adapter *adapter);
static u32 efuse_proc_ck(struct mac_ax_adapter *adapter);
static u32 cnv_efuse_state(struct mac_ax_adapter *adapter, u8 dest_state);
static u32 switch_efuse_bank(struct mac_ax_adapter *adapter,
			     enum mac_ax_efuse_bank bank);
static u32 proc_dump_efuse(struct mac_ax_adapter *adapter,
			   enum mac_ax_efuse_read_cfg cfg);
static u32 read_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			 u8 *map);
static u32 write_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u8 value);
static u32 cmp_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u16 val);
static u32 eeprom_parser(struct mac_ax_adapter *adapter, u8 *phy_map,
			 u8 *log_map, enum mac_ax_efuse_parser_cfg cfg);
static u32 read_log_efuse_map(struct mac_ax_adapter *adapter, u8 *map);
static u32 proc_pg_efuse_by_map(struct mac_ax_adapter *adapter,
				struct mac_ax_pg_efuse_info *info,
				enum mac_ax_efuse_read_cfg cfg);
static u32 dump_efuse_drv(struct mac_ax_adapter *adapter);
static u32 dump_efuse_fw(struct mac_ax_adapter *adapter);
static u32 proc_write_log_efuse(struct mac_ax_adapter *adapter, u32 offset,
				u8 value);
static u32 read_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
		      u8 *map);
static u32 update_eeprom_mask(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask);
static u32 check_efuse_enough(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask);
static u32 proc_pg_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 word_en,
			 u8 pre_word_en, u32 eeprom_offset);
static u32 program_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 *updated_mask);
static void mask_eeprom(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info);

u32 mac_dump_efuse_map_wl(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg)
{
	u32 ret,stat;

	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = proc_dump_efuse(adapter, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump efuse!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_dump_efuse_map_bt(struct mac_ax_adapter *adapter)
{
	u32 ret, stat;
	u8 *map = NULL;
	u32 efuse_size = adapter->hw_info->bt_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_BT);
	if (ret != 0)
		return ret;

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	if (efuse_param->bt_efuse_map_valid == 0) {
		ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_ERR("[ERR]malloc!!\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return MACBUFALLOC;
		}

		ret = read_hw_efuse(adapter, 0, efuse_size, map);
		if (ret != 0) {
			PLTFM_FREE(map, efuse_size);
			PLTFM_MSG_ERR("[ERR]read hw efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(efuse_param->bt_efuse_map, map, efuse_size);
		efuse_param->bt_efuse_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
		PLTFM_FREE(map, efuse_size);
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_write_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val,
		    enum mac_ax_efuse_bank bank)
{
	u32 ret, stat, efuse_size;

	if (bank == MAC_AX_EFUSE_BANK_WIFI)
		efuse_size = adapter->hw_info->wl_efuse_size;
	else if (bank == MAC_AX_EFUSE_BANK_BT)
		efuse_size = adapter->hw_info->bt_efuse_size;

	if (addr >= efuse_size) {
		PLTFM_MSG_ERR("[ERR]Offset is too large\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, bank);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = write_hw_efuse(adapter, addr, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write physical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return ret;
}

u32 mac_read_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size, u8 *val,
		   enum mac_ax_efuse_bank bank)
{
	u32 ret, stat, efuse_size;

	if (bank == MAC_AX_EFUSE_BANK_WIFI)
		efuse_size = adapter->hw_info->wl_efuse_size;
	else if (bank == MAC_AX_EFUSE_BANK_BT)
		efuse_size = adapter->hw_info->bt_efuse_size;

	if (addr >= efuse_size || addr + size > efuse_size) {
		PLTFM_MSG_ERR("[ERR] Wrong efuse index\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, bank);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = read_hw_efuse(adapter, addr, size, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]read hw efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_get_efuse_avl_size(struct mac_ax_adapter *adapter, u32 *size)
{
	u32 ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	ret = mac_dump_log_efuse(adapter, MAC_AX_EFUSE_PARSER_MAP,
				 MAC_AX_EFUSE_R_DRV);
	if (ret != 0)
		return ret;

	*size = adapter->hw_info->efuse_size -
		efuse_param->efuse_end;

	return 0;
}

u32 mac_dump_log_efuse(struct mac_ax_adapter *adapter,
		       enum mac_ax_efuse_parser_cfg parser_cfg,
		       enum mac_ax_efuse_read_cfg cfg)
{
	u8 *map = NULL;
	u32 ret, stat;
	u32 efuse_size = adapter->hw_info->log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
	if (ret != 0)
		return ret;

	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	if (efuse_param->log_efuse_map_valid == 0) {
		ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		ret = proc_dump_efuse(adapter, cfg);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]dump efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		if (efuse_param->efuse_map_valid == 1) {
			map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return MACBUFALLOC;
			}
			PLTFM_MEMSET(map, 0xFF, efuse_size);

			ret = eeprom_parser(adapter, efuse_param->efuse_map,
					    map, parser_cfg);
			if ( ret != 0) {
				PLTFM_FREE(map, efuse_size);
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return ret;
			}
			PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
			PLTFM_MEMCPY(efuse_param->log_efuse_map, map,
				     efuse_size);
			efuse_param->log_efuse_map_valid = 1;
			PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
			PLTFM_FREE(map, efuse_size);
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_read_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
		       u8 *val)
{
	u8 *map = NULL;
	u32 ret, stat;
	u32 efuse_size = adapter->hw_info->log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	if (addr >= efuse_size || addr + size > efuse_size) {
		PLTFM_MSG_ERR("[ERR] Wrong efuse index\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	map = (u8 *)PLTFM_MALLOC(efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, efuse_size);

	ret = read_log_efuse_map(adapter, map);
	if (ret != 0) {
		PLTFM_FREE(map, efuse_size);
		PLTFM_MSG_ERR("[ERR]read logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	PLTFM_MEMCPY(val, map + addr, size);
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	PLTFM_FREE(map, efuse_size);

	stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (stat != 0)
		return stat;

	return 0;
}

u32 mac_write_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	u32 ret, stat;

	if (addr >= adapter->hw_info->log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]addr is too large\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = proc_write_log_efuse(adapter, addr, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_pg_efuse_by_map(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info,
			enum mac_ax_efuse_read_cfg cfg)
{
	u32 ret, stat;

	if (info->efuse_map_size != adapter->hw_info->log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_ERR("[ERR]not multiple of 16\n");
		return MACEFUSESIZE;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_ERR("[ERR]mask size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	if (!info->efuse_mask) {
		PLTFM_MSG_ERR("[ERR]mask is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = proc_pg_efuse_by_map(adapter, info, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_mask_log_efuse(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info)
{
	if (info->efuse_map_size != adapter->hw_info->log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_ERR("[ERR]not multiple of 16\n");
		return MACEFUSESIZE;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_ERR("[ERR]mask size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	if (!info->efuse_mask) {
		PLTFM_MSG_ERR("[ERR]mask is NULL\n");
		return MACNPTR;
	}

	mask_eeprom(adapter, info);

	return 0;
}

u32 mac_pg_sec_data_by_map(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info)
{
	u32 ret, stat;
	u32 addr, efuse_size, sec_data_size;
	u8 *map_pg;
	u32 mac_addr_size = 6;

	/*Soar TBD add MAC address PG*/

	map_pg = info->efuse_map;
	efuse_size = adapter->hw_info->efuse_size;
	sec_data_size = adapter->hw_info->sec_data_efuse_size;

	if (info->efuse_map_size != adapter->hw_info->sec_data_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	for (addr = mac_addr_size; addr < sec_data_size; addr++) {
		ret = write_hw_efuse(adapter, addr + efuse_size,
				     *(map_pg + addr));
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]write physical efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

u32 mac_cmp_sec_data_by_map(struct mac_ax_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info)
{
	u32 ret, stat;
	u32 addr, start_addr, sec_data_size;
	u16 val16;
	u8 *map_pg;

	map_pg = info->efuse_map;
	start_addr = adapter->hw_info->efuse_size;
	sec_data_size = adapter->hw_info->sec_data_efuse_size;

	if (info->efuse_map_size != adapter->hw_info->sec_data_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	for (addr = 0;
	     addr < sec_data_size; addr += 2) {
	     	val16 = *(map_pg + addr) | (*(map_pg + addr + 1) << 8);
		ret = cmp_hw_efuse(adapter, addr + start_addr, val16);
		if (ret == MACEFUSECMP) {
			*(map_pg + addr) = 0xFF;
			*(map_pg + addr + 1) = 0xFF;
		} else if (ret == MACEFUSEREAD) {
			PLTFM_MSG_ERR("[ERR]compare hw efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
		} else if (ret == 0) {
			*(map_pg + addr) = 0x00;
			*(map_pg + addr + 1) = 0x00;
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return 0;
}

void cfg_efuse_auto_ck(struct mac_ax_adapter *adapter, u8 enable)
{
	/*Soar TBD* move to set hw value/
	/*
	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);
	adapter->efuse_param.auto_ck_en = enable;
	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);
	*/
}

u32 efuse_tbl_init(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_INIT(&efuse_tbl.lock);

	return 0;
}

u32 efuse_tbl_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_DEINIT(&efuse_tbl.lock);

	return 0;
}

static u32 efuse_map_init(struct mac_ax_adapter *adapter,
			  enum efuse_map_sel map_sel)
{
	u32 size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	if (map_sel == EFUSE_MAP_SEL_PHY_WL) {
		size = adapter->hw_info->efuse_size;
		if (!efuse_param->efuse_map) {
			efuse_param->efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
	} else if (map_sel == EFUSE_MAP_SEL_PHY_BT) {
		size = adapter->hw_info->bt_efuse_size;
		if (!efuse_param->bt_efuse_map) {
			efuse_param->bt_efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->bt_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
	} else if (map_sel == EFUSE_MAP_SEL_LOG) {
		size = adapter->hw_info->log_efuse_size;
		if (!efuse_param->log_efuse_map) {
			efuse_param->log_efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->log_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
	} else {
		return MACEFUSEBANK;
	}

	return 0;
}

static u32 efuse_fwcmd_ck(struct mac_ax_adapter *adapter)
{
/*Soar TBD*/
/*
	u32 ret;
	ret = fwcmd_wq_idle(adapter,
			    SET_FWCMD_ID(FWCMD_TYPE_H2C,
					 FWCMD_H2C_CAT_MAC,
					 TBD,
					 TBD));
	if (ret != 0) {
		PLTFM_MSG_WARN("[WARN]H2C not idle(efuse)\n");
		return ret;
	}
*/

	return 0;
}

static u32 efuse_proc_ck(struct mac_ax_adapter *adapter)
{
/*Soar TBD*/
/*
	u32 ret;
	ret = efuse_fwcmd_ck(adapter);
	if (ret != 0)
		return ret;
*/
	if (adapter->sm.efuse != MAC_AX_EFUSE_IDLE) {
		PLTFM_MSG_WARN("[WARN]Proc not idle(efuse)\n");
		return MACPROCBUSY;
	}

	if (adapter->sm.pwr != MAC_AX_PWR_ON)
		PLTFM_MSG_ERR("[ERR]Access efuse in suspend\n");

	return 0;
}

static u32 cnv_efuse_state(struct mac_ax_adapter *adapter, u8 dest_state)
{
	if (adapter->sm.efuse >= MAC_AX_EFUSE_MAX)
		return MACPROCERR;

	if (adapter->sm.efuse == dest_state)
		return MACPROCERR;

	if (dest_state != MAC_AX_EFUSE_IDLE) {
		if (adapter->sm.efuse != MAC_AX_EFUSE_IDLE)
			return MACPROCERR;
	}

	adapter->sm.efuse = dest_state;

	return 0;
}

static u32 switch_efuse_bank(struct mac_ax_adapter *adapter,
			     enum mac_ax_efuse_bank bank)
{
	u8 reg_value;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	reg_value = MAC_REG_R8(R_AX_EFUSE_CTRL_1 + 1);

	if (bank == (reg_value & B_AX_EF_CELL_SEL_MSK))
		return 0;

	reg_value &= ~B_AX_EF_CELL_SEL_MSK;
	reg_value |= bank;
	MAC_REG_W8(R_AX_EFUSE_CTRL_1 + 1, reg_value);

	reg_value = MAC_REG_R8(R_AX_EFUSE_CTRL_1 + 1);
	if ((reg_value & B_AX_EF_CELL_SEL_MSK) != bank)
		return MACEFUSEBANK;

	return 0;
}

static u32 proc_dump_efuse(struct mac_ax_adapter *adapter,
			   enum mac_ax_efuse_read_cfg cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

	if (cfg == MAC_AX_EFUSE_R_AUTO) {
		if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
			ret = dump_efuse_drv(adapter);
		else
			ret = dump_efuse_fw(adapter);
	} else if (cfg == MAC_AX_EFUSE_R_FW) {
		ret = dump_efuse_fw(adapter);
	} else {
		ret = dump_efuse_drv(adapter);
	}

	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump efsue drv/fw\n");
		return ret;
	}

	return 0;
}

static u32 read_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			 u8 *map)
{
	u32 addr;
	u32 tmp32;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	for (addr = offset; addr < offset + size; addr++) {
		MAC_REG_W32(R_AX_EFUSE_CTRL,
			    (addr & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH);

		cnt = EFUSE_WAIT_CNT;
		while (cnt--) {
			tmp32 = MAC_REG_R32(R_AX_EFUSE_CTRL);
			if (tmp32 & B_AX_EF_RDY)
				break;
			PLTFM_DELAY_US(1);
		}

		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]read efuse\n");
			return MACEFUSEREAD;
		}

		*(map + addr - offset) = (u8)(tmp32 & 0xFF);
	}

	return 0;
}

static u32 write_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u8 value)
{
	const u8 unlock_code = 0x69;
	u8 value_read = 0;
	u32 value32;
	u32 cnt;
	u32 ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	efuse_param->efuse_map_valid = 0;
	efuse_param->bt_efuse_map_valid = 0;
	efuse_param->log_efuse_map_valid = 0;
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

	MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, unlock_code);

	value32 = value | ((offset & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH);
	MAC_REG_W32(R_AX_EFUSE_CTRL, value32 | (0x2 << B_AX_EF_MODE_SEL_SH));

	cnt = EFUSE_WAIT_CNT;
	while (cnt--) {
		if (MAC_REG_R32(R_AX_EFUSE_CTRL) & B_AX_EF_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]write efuse\n");
		return MACEFUSEWRITE;
	}

	MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, 0x00);

	if (efuse_param->auto_ck_en == 1) {
		ret = read_hw_efuse(adapter, offset, 1, &value_read);
		if (ret != 0)
			return ret;
		if (value_read != value) {
			PLTFM_MSG_ERR("[ERR]efuse compare\n");
			return MACEFUSEWRITE;
		}
	}

	return 0;
}

static u32 cmp_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u16 val)
{
	u32 val32;
	u32 tmp32;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = val | ((offset & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH);
	MAC_REG_W32(R_AX_EFUSE_CTRL, val32 | (0x3 << B_AX_EF_MODE_SEL_SH));

	cnt = EFUSE_WAIT_CNT;
	while (cnt--) {
		tmp32 = MAC_REG_R32(R_AX_EFUSE_CTRL);
		if (tmp32 & B_AX_EF_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]compare efuse\n");
		return MACEFUSEREAD;
	}

	if (0 == (tmp32 & B_AX_EF_COMP_RESULT))
		return MACEFUSECMP;

	return 0;
}

static u32 eeprom_parser(struct mac_ax_adapter *adapter, u8 *phy_map,
			 u8 *log_map, enum mac_ax_efuse_parser_cfg cfg)
{
	u8 i;
	u8 value8;
	u8 blk_idx;
	u8 word_en;
	u8 valid;
	u8 hdr = 0;
	u8 hdr2 = 0;
	u32 eeprom_idx;
	u32 sec_ctrl_size = adapter->hw_info->sec_ctrl_efuse_size;
	u32 efuse_idx = sec_ctrl_size;
	u32 efuse_size = adapter->hw_info->efuse_size;
	u32 log_efuse_size = adapter->hw_info->log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	PLTFM_MEMSET(log_map, 0xFF, log_efuse_size);

	do {
		value8 = *(phy_map + efuse_idx);
		hdr = value8;
		if (hdr == 0xff)
			break;
		efuse_idx++;

		value8 = *(phy_map + efuse_idx);
		hdr2 = value8;
		if (hdr2 == 0xff)
			break;

		blk_idx = ((hdr2 & 0xF0) >> 4) | ((hdr & 0x0F) << 4);
		word_en = hdr2 & 0x0F;

		efuse_idx++;

		if (efuse_idx >= efuse_size - sec_ctrl_size - 1)
			return MACEFUSEPARSE;

		for (i = 0; i < 4; i++) {
			valid = (u8)((~(word_en >> i)) & BIT(0));
			if (valid == 1) {
				eeprom_idx = (blk_idx << 3) + (i << 1);

				if ((eeprom_idx + 1) > log_efuse_size) {
					PLTFM_MSG_ERR("[ERR]efuse idx:0x%X\n",
						      efuse_idx - 1);

					PLTFM_MSG_ERR("[ERR]read hdr:0x%X\n",
						      hdr);

					PLTFM_MSG_ERR("[ERR]rad hdr2:0x%X\n",
						      hdr2);

					return MACEFUSEPARSE;
				}

				if (cfg == MAC_AX_EFUSE_PARSER_MAP) {
					value8 = *(phy_map + efuse_idx);
					*(log_map + eeprom_idx) = value8;
				} else if (cfg == MAC_AX_EFUSE_PARSER_MASK) {
					*(log_map + eeprom_idx) = 0x00;
				}

				eeprom_idx++;
				efuse_idx++;

				if (efuse_idx > efuse_size - sec_ctrl_size - 1)
					return MACEFUSEPARSE;

				if (cfg == MAC_AX_EFUSE_PARSER_MAP) {
					value8 = *(phy_map + efuse_idx);
					*(log_map + eeprom_idx) = value8;
				} else if (cfg == MAC_AX_EFUSE_PARSER_MASK) {
					*(log_map + eeprom_idx) = 0x00;
				}

				efuse_idx++;

				if (efuse_idx > efuse_size - sec_ctrl_size)
					return MACEFUSEPARSE;
			}
		}
	} while (1);

	efuse_param->efuse_end = efuse_idx;

	return 0;
}

static u32 read_log_efuse_map(struct mac_ax_adapter *adapter, u8 *map)
{
	u8 *local_map = NULL;
	u32 efuse_size, ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	if (efuse_param->log_efuse_map_valid == 0) {
		if (efuse_param->efuse_map_valid == 0) {
			efuse_size = adapter->hw_info->efuse_size;

			local_map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!local_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}

			ret = read_efuse(adapter, 0, efuse_size, local_map);
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]read efuse\n");
				PLTFM_FREE(local_map, efuse_size);
				return ret;
			}

			ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
			if (ret != 0) {
				PLTFM_FREE(local_map, efuse_size);
				return ret;
			}

			PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
			PLTFM_MEMCPY(efuse_param->efuse_map, local_map,
				     efuse_size);
			efuse_param->efuse_map_valid = 1;
			PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

			PLTFM_FREE(local_map, efuse_size);
		}

		ret = eeprom_parser(adapter, efuse_param->efuse_map, map,
				    MAC_AX_EFUSE_PARSER_MAP);
		if (ret != 0)
			return ret;

		efuse_size = adapter->hw_info->log_efuse_size;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
		if (ret != 0)
			return ret;

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(efuse_param->log_efuse_map, map, efuse_size);
		efuse_param->log_efuse_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	} else {
		efuse_size = adapter->hw_info->log_efuse_size;
		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(map, efuse_param->log_efuse_map, efuse_size);
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	}

	return 0;
}

static u32 proc_pg_efuse_by_map(struct mac_ax_adapter *adapter,
				struct mac_ax_pg_efuse_info *info,
				enum mac_ax_efuse_read_cfg cfg)
{
	u8 *updated_mask = NULL;
	u32 ret;
	u32 mask_size = adapter->hw_info->log_efuse_size >> 4;

	updated_mask = (u8 *)PLTFM_MALLOC(mask_size);
	if (!updated_mask) {
		PLTFM_MSG_ERR("[ERR]malloc updated mask\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(updated_mask, 0x00, mask_size);

	ret = update_eeprom_mask(adapter, info, updated_mask);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]update eeprom mask\n");
		PLTFM_FREE(updated_mask, mask_size);
		return ret;
	}

	ret = check_efuse_enough(adapter, info, updated_mask);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]chk efuse enough\n");
		PLTFM_FREE(updated_mask, mask_size);
		return ret;
	}

	ret = program_efuse(adapter, info, updated_mask);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		PLTFM_FREE(updated_mask, mask_size);
		return ret;
	}

	PLTFM_FREE(updated_mask, mask_size);

	return 0;
}

static u32 dump_efuse_drv(struct mac_ax_adapter *adapter)
{
	u8 *map = NULL;
	u32 efuse_size = adapter->hw_info->efuse_size;
	u32 ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
	if (ret != 0)
		return ret;

	if (efuse_param->efuse_map_valid == 0) {
		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		ret = read_hw_efuse(adapter, 0, efuse_size, map);
		if (ret != 0) {
			PLTFM_FREE(map, efuse_size);
			return ret;
		}

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(efuse_param->efuse_map, map, efuse_size);
		efuse_param->efuse_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
		PLTFM_FREE(map, efuse_size);
	}

	return 0;
}

static u32 dump_efuse_fw(struct mac_ax_adapter *adapter)
{
/*Soar TBD*/
/*
	u8 h2c_buf[H2C_PKT_SIZE_88XX] = { 0 };
	u16 seq_num = 0;
	u32 efuse_size = adapter->hw_cfg_info.efuse_size;
	struct halmac_h2c_header_info hdr_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	hdr_info.sub_cmd_id = SUB_CMD_ID_DUMP_PHYSICAL_EFUSE;
	hdr_info.content_size = 0;
	hdr_info.ack = 1;
	set_h2c_pkt_hdr_88xx(adapter, h2c_buf, &hdr_info, &seq_num);

	adapter->halmac_state.efuse_state.seq_num = seq_num;

	if (!adapter->efuse_map) {
		adapter->efuse_map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!adapter->efuse_map) {
			PLTFM_MSG_ERR("[ERR]malloc adapter map\n");
			reset_ofld_feature_88xx(adapter,
						FEATURE_DUMP_PHY_EFUSE);
			return HALMAC_RET_MALLOC_FAIL;
		}
	}

	if (adapter->efuse_map_valid == 0) {
		status = send_h2c_pkt_88xx(adapter, h2c_buf);
		if (status != HALMAC_RET_SUCCESS) {
			PLTFM_MSG_ERR("[ERR]send h2c pkt\n");
			reset_ofld_feature_88xx(adapter,
						FEATURE_DUMP_PHY_EFUSE);
			return status;
		}
	}
*/
	return 0;
}

static u32 proc_write_log_efuse(struct mac_ax_adapter *adapter, u32 offset,
				u8 value)
{
	u8 byte1;
	u8 byte2;
	u8 blk;
	u8 blk_idx;
	u8 hdr;
	u8 hdr2;
	u8 *map = NULL;
	u32 log_efuse_size = adapter->hw_info->log_efuse_size;
	u32 end, ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, log_efuse_size);

	if (efuse_param->log_efuse_map_valid == 0) {
		ret = read_log_efuse_map(adapter, map);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]read logical efuse\n");
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}
	} else {
		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(map, efuse_param->log_efuse_map, log_efuse_size);
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	}

	if (*(map + offset) != value) {
		end = efuse_param->efuse_end;
		blk = (u8)(offset >> 3);
		blk_idx = (u8)((offset & (8 - 1)) >> 1);

		hdr = ((blk & 0xF0) >> 4) | 0x30;
		hdr2 = (u8)(((blk & 0x0F) << 4) + ((0x1 << blk_idx) ^ 0x0F));

		if ((offset & 1) == 0) {
			byte1 = value;
			byte2 = *(map + offset + 1);
		} else {
			byte1 = *(map + offset - 1);
			byte2 = value;
		}

		if (adapter->hw_info->efuse_size <= 4 + end) {
			PLTFM_FREE(map, log_efuse_size);
			return MACEFUSESIZE;
		}

		ret = write_hw_efuse(adapter, end, hdr);
		if (ret != 0) {
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}

		ret = write_hw_efuse(adapter, end + 1, hdr2);
		if (ret != 0) {
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}

		ret = write_hw_efuse(adapter, end + 2, byte1);
		if (ret != 0) {
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}

		ret = write_hw_efuse(adapter, end + 3, byte2);
		if (ret != 0) {
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}
	}

	PLTFM_FREE(map, log_efuse_size);

	return 0;
}

static u32 read_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
		      u8 *map)
{
	u32 ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}

	if (efuse_param->efuse_map_valid == 1) {
		PLTFM_MEMCPY(map, efuse_param->efuse_map + offset, size);
	} else {
		ret = read_hw_efuse(adapter, offset, size, map);
		if (ret != 0)
			return ret;
	}

	return 0;
}

static u32 update_eeprom_mask(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask)
{
	u8 *map = NULL;
	u8 clr_bit = 0;
	u32 log_efuse_size = adapter->hw_info->log_efuse_size;
	u8 *map_pg;
	u8 *efuse_mask;
	u16 i;
	u16 j;
	u16 map_offset;
	u16 mask_offset;
	u32 ret;

	map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, log_efuse_size);

	PLTFM_MEMSET(updated_mask, 0x00, info->efuse_mask_size);

	ret = read_log_efuse_map(adapter, map);
	if (ret != 0) {
		PLTFM_FREE(map, log_efuse_size);
		return ret;
	}

	map_pg = info->efuse_map;
	efuse_mask = info->efuse_mask;

	for (i = 0; i < info->efuse_mask_size; i++)
		*(updated_mask + i) = *(efuse_mask + i);

	for (i = 0; i < info->efuse_map_size; i += 16) {
		for (j = 0; j < 16; j += 2) {
			map_offset = i + j;
			mask_offset = i >> 4;
			if (*(u16 *)(map_pg + map_offset) ==
			    *(u16 *)(map + map_offset)) {
				switch (j) {
				case 0:
					clr_bit = BIT(4);
					break;
				case 2:
					clr_bit = BIT(5);
					break;
				case 4:
					clr_bit = BIT(6);
					break;
				case 6:
					clr_bit = BIT(7);
					break;
				case 8:
					clr_bit = BIT(0);
					break;
				case 10:
					clr_bit = BIT(1);
					break;
				case 12:
					clr_bit = BIT(2);
					break;
				case 14:
					clr_bit = BIT(3);
					break;
				default:
					break;
				}
				*(updated_mask + mask_offset) &= ~clr_bit;
			}
		}
	}

	PLTFM_FREE(map, log_efuse_size);

	return 0;
}

static u32 check_efuse_enough(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask)
{
	u8 pre_word_en;
	u16 i;
	u16 j;
	u32 eeprom_offset;
	u32 pg_num = 0;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		eeprom_offset = i;

		if ((eeprom_offset & 7) > 0)
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
		else
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);

		if (pre_word_en > 0) {
			if (eeprom_offset > 0x7f) {
				pg_num += 2;
				for (j = 0; j < 4; j++) {
					if (((pre_word_en >> j) & 0x1) > 0)
						pg_num += 2;
				}
			} else {
				pg_num++;
				for (j = 0; j < 4; j++) {
					if (((pre_word_en >> j) & 0x1) > 0)
						pg_num += 2;
				}
			}
		}
	}

	if (adapter->hw_info->efuse_size <=
	    (pg_num + efuse_param->efuse_end))
		return MACEFUSESIZE;

	return 0;
}

static u32 proc_pg_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 word_en,
			 u8 pre_word_en, u32 eeprom_offset)
{
	u8 blk;
	u8 hdr;
	u8 hdr2;
	u16 i;
	u32 efuse_end, ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	efuse_end = efuse_param->efuse_end;

	blk = (u8)(eeprom_offset >> 3);
	hdr = ((blk & 0xF0) >> 4) | 0x30;
	hdr2 = (u8)(((blk & 0x0F) << 4) + word_en);

	ret = write_hw_efuse(adapter, efuse_end, hdr);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write efuse\n");
		return ret;
	}

	ret = write_hw_efuse(adapter, efuse_end + 1, hdr2);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write efuse(+1)\n");
		return ret;
	}

	efuse_end = efuse_end + 2;
	for (i = 0; i < 4; i++) {
		if (((pre_word_en >> i) & 0x1) > 0) {
			ret = write_hw_efuse(adapter, efuse_end,
						     *(info->efuse_map +
						     eeprom_offset +
						     (i << 1)));
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]write efuse(<<1)\n");
				return ret;
			}

			ret = write_hw_efuse(adapter, efuse_end + 1,
						     *(info->efuse_map +
						     eeprom_offset + (i << 1)
						     + 1));
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]write efuse(<<1)+1\n");
				return ret;
			}
			efuse_end = efuse_end + 2;
		}
	}
	efuse_param->efuse_end = efuse_end;
	return 0;
}

static u32 program_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 *updated_mask)
{
	u8 pre_word_en;
	u8 word_en;
	u16 i;
	u32 eeprom_offset, ret;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		eeprom_offset = i;

		if (((eeprom_offset >> 3) & 1) > 0) {
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
			word_en = pre_word_en ^ 0x0F;
		} else {
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);
			word_en = pre_word_en ^ 0x0F;
		}

		if (pre_word_en > 0) {
			ret = proc_pg_efuse(adapter, info,
						    word_en,
						    pre_word_en,
						    eeprom_offset);
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]pg efuse");
				return ret;
			}
		}
	}

	return 0;
}

static void mask_eeprom(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info)
{
	u8 pre_word_en;
	u8 *updated_mask;
	u8 *efuse_map;
	u16 i;
	u16 j;
	u32 offset;

	updated_mask = info->efuse_mask;
	efuse_map = info->efuse_map;

	for (i = 0; i < info->efuse_map_size; i = i + 8) {
		offset = i;

		if (((offset >> 3) & 1) > 0)
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
		else
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);

		for (j = 0; j < 4; j++) {
			if (((pre_word_en >> j) & 0x1) == 0) {
				*(efuse_map + offset + (j << 1)) = 0xFF;
				*(efuse_map + offset + (j << 1) + 1) = 0xFF;
			}
		}
	}
}
