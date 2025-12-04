/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_l3_if.h: Hardware Abstraction Layer for Egress L3 If table to access hardware regsiters
 *
 */
#ifndef __AAL_L3_IF_H__
#define __AAL_L3_IF_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#define FE_L3E_IF_TBL_ENTRY_MAX	64	/* 2^6 */

typedef struct fe_l3e_if_tbl_entry_s {
	ca_uint32_t	mtu_vld			: 1;
	ca_uint32_t	mtu_enc			: 2;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	vlan_vld		: 1;
	ca_uint32_t	inner_dei_sel		: 2;
	ca_uint32_t	inner_dei		: 1;
	ca_uint32_t	inner_vid		: 12;
	ca_uint32_t	inner_tpid_enc		: 3;
	ca_uint32_t	top_dei_sel		: 2;
	ca_uint32_t	top_dei			: 1;
	ca_uint32_t	top_vid			: 12;
	ca_uint32_t	top_tpid_enc		: 3;
	ca_uint32_t	vlan_cnt		: 2;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	ca_uint32_t	mac_da_vld		: 1;
	ca_uint32_t	mac_da_an_sel		: 4;
	ca_uint32_t	mac_sa_vld		: 1;
	ca_uint32_t	mac_sa_an_sel		: 4;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	ldpid_vld		: 1;
	ca_uint32_t	ldpid			: 6;
	ca_uint32_t	deep_q			: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	reserved		: 1;
} __attribute__((packed)) fe_l3e_if_tbl_entry_t;

int aal_fe_l3e_if_tbl_init(void);
void aal_fe_l3e_if_tbl_exit(void);
int aal_l3_if_id_get(/*CODEGEN_IGNORE_TAG*/unsigned int lpid);

/* AAL CLI debug functions */
ca_status_t aal_l3_if_get(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_OUT fe_l3e_if_tbl_entry_t *entry
);

ca_status_t aal_l3_if_set(
        CA_IN ca_device_id_t dev,
        CA_IN ca_uint16_t entry_idx,
        CA_IN fe_l3e_if_tbl_entry_t *entry
);

#endif /* __AAL_L3_IF_H__ */
