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
 * aal_l3_stg0.h: Hardware Abstraction Layer for L3 STG0 Table
 *
 */
#ifndef __AAL_L3_STG0_H__
#define __AAL_L3_STG0_H__

typedef struct {
	ca_uint32_t rule_0               :  6 ;
	ca_uint32_t lpb_idx_0            :  2 ;
	ca_uint32_t rule_1               :  6 ;
	ca_uint32_t lpb_idx_1            :  2 ;
	ca_uint32_t rule_2               :  6 ;
	ca_uint32_t lpb_idx_2            :  2 ;
	ca_uint32_t lpb_idx_3            :  2 ;
	ca_uint32_t rsrvd1               :  6 ;
} __attribute__((packed)) l3_stg0_ldpid_map_t;

typedef union {
	ca_uint32_t u32;

	struct {
		ca_uint32_t rsvd                 : 8;
		ca_uint32_t mru_redir_pol_grp_id : 1;
		ca_uint32_t mru_redir_pol_id     : 1;
		ca_uint32_t mru_redir_pol_en     : 1;
		ca_uint32_t mru_redir_pol_cos    : 1;
		ca_uint32_t mru_redir_ldpid      : 1;
		ca_uint32_t mru_redir_en         : 1;
		ca_uint32_t mru_len              : 1;
		ca_uint32_t t5_ctrl              : 1;
		ca_uint32_t t1_ctrl              : 1;
		ca_uint32_t pol_grp_id           : 1;
		ca_uint32_t pol_id               : 1;
		ca_uint32_t pol_en               : 1;
		ca_uint32_t mrr_vld              : 1;
		ca_uint32_t mrr_en               : 1;
		ca_uint32_t default_cos          : 1;
		ca_uint32_t default_cos_sel      : 1;
		ca_uint32_t default_cos_tbl_sel  : 1;
		ca_uint32_t default_ldpid        : 1;
		ca_uint32_t mac_da_an_mask       : 1;
		ca_uint32_t mac_da_match_en      : 1;
		ca_uint32_t spcl_pkt_sel         : 1;
		ca_uint32_t spcl_pkt_en          : 1;
		ca_uint32_t default_802_1p       : 1;
		ca_uint32_t lspid                : 1;
	}s;
} __attribute__((packed)) aal_l3fe_lpb_tbl_cfg_mask_t;

typedef struct {
	ca_uint32_t	mru_redir_pol_grp_id	: 3;
	ca_uint32_t	mru_redir_pol_id	: 9;
	ca_uint32_t	mru_redir_pol_en	: 2;
	ca_uint32_t	mru_redir_pol_cos	: 3;
	ca_uint32_t	mru_redir_ldpid		: 6;
	ca_uint32_t	mru_redir_en		: 1;
	ca_uint32_t	mru_len			: 14;

	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t1_ctrl			: 4;

	ca_uint32_t	pol_grp_id		: 3;
	ca_uint32_t	pol_id			: 9;
	ca_uint32_t	pol_en			: 2;

	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;

	ca_uint32_t	default_cos		: 3;
	ca_uint32_t	default_cos_sel		: 2;
	ca_uint32_t	default_cos_tbl_sel	: 1;
	ca_uint32_t	default_ldpid		: 6;

	ca_uint32_t	mac_da_an_mask		: 8;
	ca_uint32_t	mac_da_match_en		: 1;

	ca_uint32_t	spcl_pkt_sel		: 1;
	ca_uint32_t	spcl_pkt_en		: 1;

	ca_uint32_t	default_802_1p		: 3;

	ca_uint32_t	lspid			: 6;

	ca_uint32_t	reserved		: 2;
} __attribute__((packed)) aal_l3fe_lpb_tbl_cfg_t;

ca_status_t aal_l3_stg0_ldpid_map_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT l3_stg0_ldpid_map_t *config);

ca_status_t aal_l3_stg0_ldpid_map_set(
    CA_IN ca_device_id_t device_id,
    CA_IN l3_stg0_ldpid_map_t *config);

ca_status_t aal_l3_stg0_lpb_tbl_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t  tbl_idx,
    CA_OUT aal_l3fe_lpb_tbl_cfg_t *config);

ca_status_t aal_l3_stg0_lpb_tbl_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t  tbl_idx,
    CA_IN aal_l3fe_lpb_tbl_cfg_mask_t mask,
    CA_OUT aal_l3fe_lpb_tbl_cfg_t *config);

#endif /* __AAL_L3_STG0_H__ */
