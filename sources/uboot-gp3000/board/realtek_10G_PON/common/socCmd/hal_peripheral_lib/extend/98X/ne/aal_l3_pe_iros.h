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
 * aal_l3_pe_iros_iros.h: L3FE Packet Editor AAL API exported for iROS
 *
 */
#ifndef __AAL_L3_PE_IROS_H__
#define __AAL_L3_PE_IROS_H__

typedef struct l3pe_sixrd_tbl_entry_s {
	ca_uint32_t	valid		: 1;
	ca_uint32_t	ingr_chk_en	: 1;
	ca_uint32_t	br_only		: 1;
	ca_uint32_t	ipsa_mtch	: 1;
	ca_uint32_t	id_end		: 16;
	ca_uint32_t	id_start	: 16;
	ca_uint32_t	ttl_en		: 1;
	ca_uint32_t	ttl		: 8;
	ca_uint32_t	tos_en		: 1;
	ca_uint32_t	tos		: 8;
	ca_uint32_t	br_v4_ip	: 32;
	ca_uint64_t	v6_prfx		: 64;
	ca_uint32_t	v6_prfx_len	: 6;
	ca_uint32_t	v4_msk_len	: 5;
	ca_uint32_t	ce_v4_ip	: 32;
	ca_uint32_t	reserved	: 3;
} __attribute__((packed)) l3pe_sixrd_tbl_entry_t;

typedef struct l3pe_dsl_tbl_entry_s {
	ca_uint32_t	valid		: 1;
	ca_uint32_t	mc		: 1;
	ca_uint32_t	flwlabel	: 20;
	ca_uint32_t	hoplimit_en	: 1;
	ca_uint32_t	hoplimit	: 8;
	ca_uint32_t	tc_en		: 1;
	ca_uint32_t	tc		: 8;
	ca_uint32_t	aftr_ip0	: 32;
	ca_uint32_t	aftr_ip1	: 32;
	ca_uint32_t	aftr_ip2	: 32;
	ca_uint32_t	aftr_ip3	: 32;
	ca_uint32_t	b4_ip0		: 32;
	ca_uint32_t	b4_ip1		: 32;
	ca_uint32_t	b4_ip2		: 32;
	ca_uint32_t	b4_ip3		: 32;
	ca_uint32_t	reserved	: 24;
} __attribute__((packed)) l3pe_dsl_tbl_entry_t;

ca_status_t ca_aal_l3pe_mtu_size_set (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		ca_uint32_t			idx,
	CA_IN		ca_uint32_t			mtu );

ca_status_t ca_aal_l3pe_mtu_size_get (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		ca_uint32_t			idx,
	CA_OUT		ca_uint32_t			*mtu );

ca_status_t ca_aal_sixrd_add (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		l3pe_sixrd_tbl_entry_t		*entry,
	CA_OUT		ca_uint32_t			*rslt_idx );

ca_status_t ca_aal_sixrd_add_by_idx (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		l3pe_sixrd_tbl_entry_t		*entry,
	CA_IN		ca_uint32_t			idx );

ca_status_t ca_aal_sixrd_delete (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		ca_uint32_t			idx );

ca_status_t ca_aal_dsl_add (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		l3pe_dsl_tbl_entry_t		*entry,
	CA_OUT		ca_uint32_t			*rslt_idx );

ca_status_t ca_aal_dsl_add_by_idx (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		l3pe_dsl_tbl_entry_t		*entry,
	CA_IN		ca_uint32_t			idx );

ca_status_t ca_aal_dsl_delete (
	CA_IN		ca_device_id_t			device_id,
	CA_IN		ca_uint32_t			idx );


#endif /* __AAL_L3_PE_IROS_H__ */

