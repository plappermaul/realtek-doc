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
#include "_pcie.h"
#include "../mac_reg.h"

u8 reg_read8_pcie(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R8(addr);
}

void reg_write8_pcie(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	PLTFM_REG_W8(addr, val);
}

u16 reg_read16_pcie(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R16(addr);
}

void reg_write16_pcie(struct mac_ax_adapter *adapter, u32 addr, u16 val)
{
	PLTFM_REG_W16(addr, val);
}

u32 reg_read32_pcie(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R32(addr);
}

void reg_write32_pcie(struct mac_ax_adapter *adapter, u32 addr, u32 val)
{
	PLTFM_REG_W32(addr, val);
}


u32 dbi_read32_pcie(struct mac_ax_adapter *adapter, u16 addr)
{
	u16 read_addr = addr & 0x0ffc;
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u32 ret = 0;

	PLTFM_REG_W16(R_AX_DBI_FLAG, read_addr);

	PLTFM_REG_W8(R_AX_DBI_FLAG + 2, 0x2);
	tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);
		cnt--;
	}

	if (tmp_u1b) {
		ret  = 0xFFFF;
		PLTFM_MSG_ERR("[ERR]DBI read fail!\n");
	} else {
		ret = PLTFM_REG_R32(R_AX_DBI_RDATA);
		PLTFM_MSG_TRACE("[TRACE]Value-R = %x\n", ret);
	}

	return ret;
}


void dbi_write32_pcie(struct mac_ax_adapter *adapter, u16 addr, u32 data)
{
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u16 write_addr = 0;

	PLTFM_REG_W32(R_AX_DBI_WDATA, data);

	write_addr = ((addr & 0x0ffc) | (0x000F << 12));
	PLTFM_REG_W16(R_AX_DBI_FLAG, write_addr);

	PLTFM_MSG_TRACE("[TRACE]Addr-W = %x\n", write_addr);

	PLTFM_REG_W8(R_AX_DBI_FLAG + 2, 0x01);
	tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);
		cnt--;
	}

	if (tmp_u1b) {
		PLTFM_MSG_ERR("[ERR]DBI write fail!\n");
	}

}



u8 dbi_read8_pcie(struct mac_ax_adapter *adapter, u16 addr)
{
	u16 read_addr = addr & 0x0ffc;
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u8 ret = 0;

	PLTFM_REG_W16(R_AX_DBI_FLAG, read_addr);
	PLTFM_REG_W8(R_AX_DBI_FLAG + 2, 0x2);

	tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);
		cnt--;
	}

	if (tmp_u1b) {
		ret  = 0xFF;
		PLTFM_MSG_ERR("[ERR]DBI read fail!\n");
	} else {
		ret = PLTFM_REG_R8(R_AX_DBI_RDATA + (addr & (4 - 1)));
		PLTFM_MSG_TRACE("[TRACE]Value-R = %x\n", ret);
	}

	return ret;

}

void dbi_write8_pcie(struct mac_ax_adapter *adapter, u16 addr, u8 data)
{
	u8 tmp_u1b = 0;
	u32 cnt = 0;
	u16 write_addr = 0;
	u16 remainder = addr & (4 - 1);

	PLTFM_REG_W8(R_AX_DBI_WDATA + remainder, data);

	write_addr = ((addr & 0x0ffc) | (BIT(0) << (remainder + 12)));

	PLTFM_REG_W16(R_AX_DBI_FLAG, write_addr);

	PLTFM_MSG_TRACE("[TRACE]Addr-W = %x\n", write_addr);

	PLTFM_REG_W8(R_AX_DBI_FLAG + 2, 0x01);

	tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);

	cnt = 20;
	while (tmp_u1b && (cnt != 0)) {
		PLTFM_DELAY_US(10);
		tmp_u1b = PLTFM_REG_R8(R_AX_DBI_FLAG + 2);
		cnt--;
	}

	if (tmp_u1b) {
		PLTFM_MSG_ERR("[ERR]DBI write fail!\n");
	}

}


