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

#ifndef _MAC_AX_PCIE_H_
#define _MAC_AX_PCIE_H_

#include "../type.h"

u8 reg_read8_pcie(struct mac_ax_adapter *adapter, u32 addr);
void reg_write8_pcie(struct mac_ax_adapter *adapter, u32 addr, u8 val);
u16 reg_read16_pcie(struct mac_ax_adapter *adapter, u32 addr);
void reg_write16_pcie(struct mac_ax_adapter *adapter, u32 addr, u16 val);
u32 reg_read32_pcie(struct mac_ax_adapter *adapter, u32 addr);
void reg_write32_pcie(struct mac_ax_adapter *adapter, u32 addr, u32 val);
u8 dbi_read8_pcie(struct mac_ax_adapter *adapter, u16 addr);
void dbi_write8_pcie(struct mac_ax_adapter *adapter, u16 addr, u8 data);
u32 dbi_read32_pcie(struct mac_ax_adapter *adapter, u16 addr);
void dbi_write32_pcie(struct mac_ax_adapter *adapter, u16 addr, u32 data);

#endif
