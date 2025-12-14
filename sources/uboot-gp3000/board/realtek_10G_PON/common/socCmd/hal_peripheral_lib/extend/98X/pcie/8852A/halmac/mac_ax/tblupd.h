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

#ifndef _TABLEUPD_H2C_H_
#define _TABLEUPD_H2C_H_

#include "../type.h"

u32 mac_upd_dlru_fixtbl(struct mac_ax_adapter *adapter,
			struct mac_ax_dlru_fixtbl *info);
u32 mac_upd_dlru_grptbl(struct mac_ax_adapter *adapter,
			struct mac_ax_dl_ru_grptbl *info);
u32 mac_upd_ulru_fixtbl(struct mac_ax_adapter *adapter,
			struct mac_ax_ulru_fixtbl *info);
u32 mac_upd_ulru_grptbl(struct mac_ax_adapter *adapter,
			struct mac_ax_ul_ru_grptbl *info);
u32 mac_upd_rusta_info(struct mac_ax_adapter *adapter,
		       struct mac_ax_bb_stainfo *info);
u32 mac_upd_ba_infotbl(struct mac_ax_adapter *adapter,
		       struct mac_ax_ba_infotbl *info);
u32 mac_upd_mu_info(struct mac_ax_adapter *adapter,
		    struct mac_ax_mu_fixinfo *info);
u32 mac_upd_ul_fixinfo(struct mac_ax_adapter *adapter,
		       struct mac_ax_ul_fixinfo *info);
#endif
