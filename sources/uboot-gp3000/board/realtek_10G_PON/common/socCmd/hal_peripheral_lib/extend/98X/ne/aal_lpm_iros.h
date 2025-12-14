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
 * aal_lpm_iros.h: LPM AAL API exported for iROS
 *
 * NOTE: NEVER include this file in this driver. This is for iROS only.
 */
#ifndef __AAL_LPM_IROS_H__
#define __AAL_LPM_IROS_H__

#include "ca_types.h"
#include "aal_common_iros.h"

typedef struct lpm_action_ctrl_s {
	ca_uint32_t	default_idx		: 12;
	ca_uint32_t	default_ctrl		: 4;
	ca_uint32_t	default_dpid		: 6;
	ca_uint32_t	default_dpid_vld	: 1;
	ca_uint32_t	reserved_0		: 1;
	ca_uint32_t	default_stg		: 2;
	ca_uint32_t	default_stg_ud		: 2;
	ca_uint32_t	reserved_1		: 4;
} __attribute__((packed)) lpm_action_ctrl_t;

typedef struct lpm_tbl_entry_s {
	ca_uint64_t	data		: 32;
	ca_uint64_t	mask		: 6;
	ca_uint64_t	attr		: 2;
	ca_uint64_t	rst_idx		: 12;
	ca_uint64_t	rst_ctrl	: 1;
	ca_uint64_t	valid		: 1;

	/* NOT set to register; SW only */
	ca_uint64_t	profile		: 2;

	ca_uint64_t	reserved	: 8;
} __attribute__((packed)) lpm_tbl_entry_t;

ca_status_t ca_aal_lpm_add (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	lpm_tbl_entry_t		*entry,/*[4:4]*/
	CA_OUT	ca_uint32_t		*rslt_idx );

ca_status_t ca_aal_lpm_delete (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		idx );

ca_status_t ca_aal_lpm_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		idx,
	CA_IN	lpm_tbl_entry_t		*entry/*[4:4]*/ );

ca_status_t ca_aal_lpm_default_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	lpm_action_ctrl_t	*entry );

ca_status_t ca_aal_lpm_default_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_OUT	lpm_action_ctrl_t	*entry );

ca_status_t ca_aal_lpm_default_t4ctrl_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	ca_uint32_t		t4ctrl_0,
	CA_IN	ca_uint32_t		t4ctrl_1 );

ca_status_t ca_aal_lpm_default_t4ctrl_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_OUT	ca_uint32_t		*t4ctrl_0,
	CA_OUT	ca_uint32_t		*t4ctrl_1 );

ca_status_t ca_aal_lpm_first_row_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	ca_uint32_t		profile,
	CA_IN	ca_uint32_t		row );

ca_status_t ca_aal_lpm_first_row_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	ca_uint32_t		profile,
	CA_OUT	ca_uint32_t		*row );

ca_status_t ca_aal_lpm_last_row_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	ca_uint32_t		profile,
	CA_IN	ca_uint32_t		row );

ca_status_t ca_aal_lpm_last_row_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ip_afi_t		afi,
	CA_IN	ca_uint32_t		profile,
	CA_OUT	ca_uint32_t		*row );

#endif /* __AAL_LPM_IROS_H__ */

