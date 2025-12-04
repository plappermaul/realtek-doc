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
 * aal_l3_l2lookup.h: Hardware Abstraction Layer for L2 Lookup Table
 *
 */
#ifndef __AAL_L3L2LOOKUP_H__
#define __AAL_L3L2LOOKUP_H__

typedef struct {
	ca_uint32_t	ingr_known_vlan_only	: 1;
	ca_uint32_t	inner_c_tpid_en		: 1;
	ca_uint32_t	top_c_tpid_en		: 1;
	ca_uint32_t	top_s_tpid_en		: 1;
	ca_uint32_t	c_tpid_match		: 4;
	ca_uint32_t	s_tpid_match		: 4;
	ca_uint32_t	drop_dbltag_pkt		: 1;
	ca_uint32_t	drop_pritag_pkt		: 1;
	ca_uint32_t	drop_sngl_tag_pkt	: 1;
	ca_uint32_t	drop_untag_pkt		: 1;
	ca_uint32_t	drop_ctag_pkt		: 1;
	ca_uint32_t	drop_stag_pkt		: 1;
	ca_uint32_t	dft_noinnertag_cmd	: 1;
	ca_uint32_t	dft_inner_vlan_cmd	: 2;
	ca_uint32_t	inner_vlan_cmd_sel_bm	: 1;
	ca_uint32_t	dft_untag_cmd		: 1;
	ca_uint32_t	dft_top_vlan_cmd	: 2;
	ca_uint32_t	top_vlan_cmd_sel_bm	: 1;
	ca_uint32_t	sc_ind			: 1;
	ca_uint32_t	dft_vlan_id		: 12;
	ca_uint32_t	l2lrn_en		: 1;
	ca_uint32_t	lspid			: 6;
	ca_uint32_t	wan_ind			: 1;
	ca_uint32_t	rsvd			: 17;
} __attribute__((packed)) l3fe_glb_ilpb_t;

typedef struct {
	ca_uint32_t	valid0			: 1;
	ca_uint32_t	ldpid0			: 6;
	ca_uint32_t	rsvd0			: 1;
	ca_uint32_t	valid1			: 1;
	ca_uint32_t	ldpid1			: 6;
	ca_uint32_t	rsvd1			: 1;
	ca_uint32_t	valid2			: 1;
	ca_uint32_t	ldpid2			: 6;
	ca_uint32_t	valid3			: 1;
	ca_uint32_t	rsvd3			: 8;
} __attribute__((packed)) l3fe_glb_ilpb_ldpid_t;

typedef struct {
	ca_uint64_t	ldpid_vec		: 64;
} __attribute__((packed)) l3fe_glb_elpb_t;

typedef struct {
	ca_uint64_t	deepq_ldpid_valid_vec	: 64;
} __attribute__((packed)) l3fe_glb_deepq_vld_t;

typedef struct {
	ca_uint64_t	deepq_ldpid_vec		: 64;
} __attribute__((packed)) l3fe_glb_deepq_t;

typedef struct {
	ca_uint32_t	ldpid_event_5		: 1;
	ca_uint32_t	ldpid_event_6		: 1;
	ca_uint32_t	ldpid_event_7		: 1;
	ca_uint32_t	ldpid_event_8		: 1;
	ca_uint32_t	ldpid_event_9		: 1;
	ca_uint32_t	ldpid_event_10		: 1;
	ca_uint32_t	ldpid_event_11		: 1;
	ca_uint32_t	ldpid_event_12		: 1;
	ca_uint32_t	ldpid_event_13		: 1;
	ca_uint32_t	igr_mcgid_hdr		: 1;
	ca_uint32_t	igr_ve_reslt		: 2;
	ca_uint32_t	rsvd			: 20;
} __attribute__((packed)) l3fe_glb_t5_opt_t;

ca_status_t aal_l3fe_glb_ilpb_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT l3fe_glb_ilpb_t *entry
);

ca_status_t aal_l3fe_glb_ilpb_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l3fe_glb_ilpb_t *entry
);

ca_status_t aal_l3fe_glb_ilpb_ldpid_get(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_ilpb_ldpid_t *entry
);

ca_status_t aal_l3fe_glb_ilpb_ldpid_set(
	CA_IN ca_device_id_t dev,
	CA_IN l3fe_glb_ilpb_ldpid_t *entry
);

ca_status_t aal_l3fe_glb_elpb_get(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_elpb_t *entry
);

ca_status_t aal_l3fe_glb_elpb_set(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_elpb_t *entry
);

ca_status_t aal_l3fe_glb_elpb_deepq_vld_get(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_deepq_vld_t *entry
);

ca_status_t aal_l3fe_glb_elpb_deepq_vld_set(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_deepq_vld_t *entry
);

ca_status_t aal_l3fe_glb_elpb_deepq_get(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_deepq_t *entry
);

ca_status_t aal_l3fe_glb_elpb_deepq_set(
	CA_IN ca_device_id_t dev,
	CA_OUT l3fe_glb_deepq_t *entry
);

#endif
