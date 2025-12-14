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
#include "_usb.h"

u8 reg_read8_usb(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R8(addr);
}

void reg_write8_usb(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	PLTFM_REG_W8(addr, val);
}

u16 reg_read16_usb(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R16(addr);
}

void reg_write16_usb(struct mac_ax_adapter *adapter, u32 addr, u16 val)
{
	PLTFM_REG_W16(addr, val);
}

u32 reg_read32_usb(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R32(addr);
}

void reg_write32_usb(struct mac_ax_adapter *adapter, u32 addr, u32 val)
{
	PLTFM_REG_W32(addr, val);
}
