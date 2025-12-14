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
 * aal_common_iros.h: Common definitions exported for iROS
 *
 */
#ifndef __AAL_COMMON_IROS_H__
#define __AAL_COMMON_IROS_H__

#include "ca_types.h"

typedef struct {
	/* L4 */
	ca_uint64_t	l4_chksum_zero		: 1;
	ca_uint64_t	tcp_rdp_ctrl		: 9;
	ca_uint64_t	l4_dp_exact_range	: 16;
	ca_uint64_t	l4_sp_exact_range	: 16;
	/* L3 */
	ca_uint64_t	l3_chksum_err		: 1;
	ca_uint64_t	spi			: 32;
	ca_uint64_t	spi_vld			: 3;
	ca_uint64_t	icmp_type		: 8;
	ca_uint64_t	icmp_vld		: 3;
	ca_uint64_t	ipv6_doh		: 1;
	ca_uint64_t	ipv6_rh			: 1;
	ca_uint64_t	ipv6_hbh		: 1;
	ca_uint64_t	ip_fragment_flag	: 1;
	ca_uint64_t	ip_da_sa_equal		: 1;
	ca_uint64_t	ip_options		: 1;
	ca_uint64_t	ip_ttl			: 8;
	ca_uint64_t	ipv6_flow_lbl		: 20;
	ca_uint64_t	ip_da_0			: 32;
	ca_uint64_t	ip_da_1			: 32;
	ca_uint64_t	ip_da_2			: 32;
	ca_uint64_t	ip_da_3			: 32;
	ca_uint64_t	ip_sa_0			: 32;
	ca_uint64_t	ip_sa_1			: 32;
	ca_uint64_t	ip_sa_2			: 32;
	ca_uint64_t	ip_sa_3			: 32;
	ca_uint64_t	ip_l4_type		: 3;
	ca_uint64_t	ip_protocol		: 8;
	ca_uint64_t	ip_ecn			: 2;
	ca_uint64_t	ip_dscp			: 6;
	ca_uint64_t	ip_ver			: 1;
	ca_uint64_t	ip_vld			: 1;
	/* PPP / PPPoE */
	ca_uint64_t	ppp_protocol_enc	: 4;
	ca_uint64_t	pppoe_session_id	: 16;
	ca_uint64_t	pppoe_code_enc		: 4;
	ca_uint64_t	pppoe_type		: 2;
	/* VLAN */
	ca_uint64_t	inner_dei		: 1;
	ca_uint64_t	inner_8021p		: 3;
	ca_uint64_t	inner_vid		: 12;
	ca_uint64_t	inner_tpid_enc		: 3;
	ca_uint64_t	top_dei			: 1;
	ca_uint64_t	top_8021p		: 3;
	ca_uint64_t	top_vid			: 12;
	ca_uint64_t	top_tpid_enc		: 3;
	ca_uint64_t	vlan_cnt		: 2;
	/* L2 Format */
	ca_uint64_t	llc_type_enc		: 2;
	ca_uint64_t	llc_snap		: 2;
	ca_uint64_t	pktlen_rng_match_vec	: 4;
	ca_uint64_t	len_encoded		: 1;
	/* L2 */
	ca_uint64_t	ethertype_enc		: 6;
	ca_uint64_t	ethertype		: 16;
	ca_uint64_t	mac_sa_0		: 8;
	ca_uint64_t	mac_sa_1		: 8;
	ca_uint64_t	mac_sa_2		: 8;
	ca_uint64_t	mac_sa_3		: 8;
	ca_uint64_t	mac_sa_4		: 8;
	ca_uint64_t	mac_sa_5		: 8;
	ca_uint64_t	mac_da_rsvd		: 1;
	ca_uint64_t	mac_da_rng		: 1;
	ca_uint64_t	mac_da_ip_mc		: 1;
	ca_uint64_t	mac_da_an_sel		: 4;
	ca_uint64_t	mac_da_0		: 8;
	ca_uint64_t	mac_da_1		: 8;
	ca_uint64_t	mac_da_2		: 8;
	ca_uint64_t	mac_da_3		: 8;
	ca_uint64_t	mac_da_4		: 8;
	ca_uint64_t	mac_da_5		: 8;
	/* special packet */
	ca_uint64_t	spcl_pkt_hdr_mtch	: 8;
	ca_uint64_t	spcl_pkt_enc		: 6;
	/* MDATA */
	ca_uint64_t	mdata			: 64;
	/* POL ID */
	ca_uint64_t	qos_premark		: 1;
	ca_uint64_t	pol_grp_id		: 3;
	ca_uint64_t	pol_id			: 9;
	/* COS */
	ca_uint64_t	cos			: 3;
	/* Dest Port ID */
	ca_uint64_t	mcgid			: 10;
	ca_uint64_t	mc			: 1;
	/* Source Port ID */
	ca_uint64_t	mc_idx_vld		: 1;
	ca_uint64_t	orig_lspid		: 6;
	ca_uint64_t	lspid			: 6;
	/* Hash Control */
	ca_uint64_t	hkey_id			: 6;
	ca_uint64_t	ctrl_set_id		: 4;
	ca_uint64_t	table_id		: 4;
	ca_uint64_t	reserved		: 4;
	/* TODO: padding */
} __attribute__((packed)) aal_hash_key_t;

typedef struct {
	/* L4 */
	ca_uint64_t	l4_chksum_zero		: 1;
	ca_uint64_t	tcp_rdp_ctrl		: 9;
	ca_uint64_t	l4_dp_exact_range	: 17;
	ca_uint64_t	l4_sp_exact_range	: 17;

	/* L3 */
	ca_uint64_t	l3_chksum_err		: 1;
	ca_uint64_t	spi			: 1;
	ca_uint64_t	spi_vld			: 1;
	ca_uint64_t	icmp_type		: 1;
	ca_uint64_t	icmp_vld		: 1;
	ca_uint64_t	ipv6_doh		: 1;
	ca_uint64_t	ipv6_rh			: 1;
	ca_uint64_t	ipv6_hbh		: 1;
	ca_uint64_t	ip_fragment_flag	: 1;
	ca_uint64_t	ip_da_sa_equal		: 1;
	ca_uint64_t	ip_options		: 1;
	ca_uint64_t	ip_ttl			: 2;
	ca_uint64_t	ipv6_flow_lbl		: 1;
	ca_uint64_t	ip_da			: 9;
	ca_uint64_t	ip_sa			: 9;
	ca_uint64_t	ip_l4_type		: 1;
	ca_uint64_t	ip_protocol		: 1;
	ca_uint64_t	ip_ecn			: 2;
	ca_uint64_t	ip_dscp			: 6;
	ca_uint64_t	ip_ver			: 1;
	ca_uint64_t	ip_vld			: 1;

	/* PPP / PPPoE */
	ca_uint64_t	ppp_protocol_enc	: 1;
	ca_uint64_t	pppoe_session_id	: 1;
	ca_uint64_t	pppoe_code_enc		: 1;
	ca_uint64_t	pppoe_type		: 1;

	/* VLAN */
	ca_uint64_t	inner_dei		: 1;
	ca_uint64_t	inner_8021p		: 1;
	ca_uint64_t	inner_vid		: 1;
	ca_uint64_t	inner_tpid_enc		: 1;
	ca_uint64_t	top_dei			: 1;
	ca_uint64_t	top_8021p		: 1;
	ca_uint64_t	top_vid			: 1;
	ca_uint64_t	top_tpid_enc		: 1;
	ca_uint64_t	vlan_cnt		: 1;

	/* L2 Format */
	ca_uint64_t	llc_type_enc		: 1;
	ca_uint64_t	llc_snap		: 1;
	ca_uint64_t	pktlen_rng_match_vec	: 4;
	ca_uint64_t	len_encoded		: 1;

	/* L2 */
	ca_uint64_t	ethertype_enc		: 1;
	ca_uint64_t	ethertype		: 1;
	ca_uint64_t	mac_sa			: 6;
	ca_uint64_t	mac_da_rsvd		: 1;
	ca_uint64_t	mac_da_rng		: 1;
	ca_uint64_t	mac_da_ip_mc		: 1;
	ca_uint64_t	mac_da_an_sel		: 1;
	ca_uint64_t	mac_da			: 6;

	/* special packet */
	ca_uint64_t	spcl_pkt_hdr_mtch	: 8;
	ca_uint64_t	spcl_pkt_enc		: 1;

	/* MDATA */
	ca_uint64_t	mdata			: 64;

	/* POL ID */
	ca_uint64_t	qos_premark		: 1;
	ca_uint64_t	pol_grp_id		: 1;
	ca_uint64_t	pol_id			: 1;

	/* COS */
	ca_uint64_t	cos			: 1;

	/* Dest Port ID */
	ca_uint64_t	mcgid			: 10;
	ca_uint64_t	mc			: 1;

	/* Source Port ID */
	ca_uint64_t	mc_idx_vld		: 1;
	ca_uint64_t	orig_lspid		: 1;
	ca_uint64_t	lspid			: 1;

	/* Hash Control */
	ca_uint64_t	hkey_id			: 1;
	ca_uint64_t	ctrl_set_id		: 1;
	ca_uint64_t	table_id		: 1;

	/* hash mask bit range is [220:0], padding to 32-bit alignment */
	ca_uint64_t	padding			: 3;
} __attribute__((packed)) aal_hash_mask_t;

/* NOTE: This must be same as aal_hash_action_t in aal_common.h. */
typedef struct {
	/* group 0 */
	ca_uint64_t	chk_l4_dp_vld		: 1;
	ca_uint64_t	chk_l4_dp		: 16;
	ca_uint64_t	chk_l4_sp_vld		: 1;
	ca_uint64_t	chk_l4_sp		: 16;
	ca_uint64_t	chk_ip_da_vld		: 1;
	ca_uint64_t	chk_ip_da		: 32;
	ca_uint64_t	chk_ip_sa_vld		: 1;
	ca_uint64_t	chk_ip_sa		: 32;
	ca_uint64_t	chk_mac_da_vld		: 1;
	ca_uint64_t	chk_mac_da		: 24;
	ca_uint64_t	chk_mac_sa_vld		: 1;
	ca_uint64_t	chk_mac_sa		: 24;

	/* group 1 */
	ca_uint64_t	l4_dp_vld		: 1;
	ca_uint64_t	l4_dp			: 16;

	/* group 2 */
	ca_uint64_t	l4_sp_vld		: 1;
	ca_uint64_t	l4_sp			: 16;

	/* group 3 */
	ca_uint64_t	ip_da_l_vld		: 1;
	ca_uint64_t	ip_da_0			: 32; /* LSB */

	/* group 4 */
	ca_uint64_t	ip_da_h_vld		: 1;
	ca_uint64_t	ip_da_1			: 32;
	ca_uint64_t	ip_da_2			: 32;
	ca_uint64_t	ip_da_3			: 32; /* MSB */

	/* group 5 */
	ca_uint64_t	ip_sa_l_vld		: 1;
	ca_uint64_t	ip_sa_0			: 32; /* LSB */

	/* group 6 */
	ca_uint64_t	ip_sa_h_vld		: 1;
	ca_uint64_t	ip_sa_1			: 32;
	ca_uint64_t	ip_sa_2			: 32;
	ca_uint64_t	ip_sa_3			: 32; /* MSB */

	/* group 7 */
	ca_uint64_t	pppoe_vld		: 1;
	ca_uint64_t	pppoe_set		: 1;
	ca_uint64_t	pppoe_session_id	: 16;

	/* group 8 */
	ca_uint64_t	ip_ttl_vld		: 1;
	ca_uint64_t	ip_ttl_zero_discard_en	: 1;
	ca_uint64_t	ip_ttl_cmd		: 2;
	ca_uint64_t	ip_ttl			: 8;
	ca_uint64_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint64_t	six_rd_ipda_from_v6	: 1;
	ca_uint64_t	ip_mtu_enc_vld		: 1;
	ca_uint64_t	ip_mtu_enc		: 2;
	ca_uint64_t	l2_format_vld		: 1;
	ca_uint64_t	l2_format		: 1;
	ca_uint64_t	l3_egress_if_vld	: 1;

	/* group 9 */
	ca_uint64_t	vlan_vld		: 1;
	ca_uint64_t	inner_dei_sel		: 2;
	ca_uint64_t	inner_dei		: 1;
	ca_uint64_t	inner_vid		: 12;
	ca_uint64_t	inner_tpid_enc		: 3;
	ca_uint64_t	top_dei_sel		: 2;
	ca_uint64_t	top_dei			: 1;
	ca_uint64_t	top_vid			: 12;
	ca_uint64_t	top_tpid_enc		: 3;
	ca_uint64_t	vlan_cnt		: 2;

	/* group 10 */
	ca_uint64_t	mac_sa_vld		: 1;
	ca_uint64_t	mac_sa_0		: 8; /* LSB */
	ca_uint64_t	mac_sa_1		: 8;
	ca_uint64_t	mac_sa_2		: 8;
	ca_uint64_t	mac_sa_3		: 8;
	ca_uint64_t	mac_sa_4		: 8;
	ca_uint64_t	mac_sa_5		: 8; /* MSB */

	/* group 11 */
	ca_uint64_t	mac_da_vld		: 1;
	ca_uint64_t	mac_da_0		: 8; /* LSB */
	ca_uint64_t	mac_da_1		: 8;
	ca_uint64_t	mac_da_2		: 8;
	ca_uint64_t	mac_da_3		: 8;
	ca_uint64_t	mac_da_4		: 8;
	ca_uint64_t	mac_da_5		: 8; /* MSB */

	/* group 12 */
	ca_uint64_t	mdata_w_vld_0		: 4;
	ca_uint64_t	mdata_w_0		: 16;

	/* group 13 */
	ca_uint64_t	mdata_w_vld_1		: 4;
	ca_uint64_t	mdata_w_1		: 16;

	/* group 14 */
	ca_uint64_t	mdata_w_vld_2		: 4;
	ca_uint64_t	mdata_w_2		: 16;

	/* group 15 */
	ca_uint64_t	mdata_w_vld_3		: 4;
	ca_uint64_t	mdata_w_3		: 16;

	/* group 16 */
	ca_uint64_t	pol_vld			: 1;
	ca_uint64_t	qos_premark		: 1;
	ca_uint64_t	pol_all_bypass		: 1;
	ca_uint64_t	pol_en			: 2;
	ca_uint64_t	pol_base		: 9;
	ca_uint64_t	pol_sel			: 3;
	ca_uint64_t	pol_table_sel		: 1;
	ca_uint64_t	pol_grp_vld		: 1;
	ca_uint64_t	pol_grp_id		: 3;

	/* group 17 */
	ca_uint64_t	ip_ecn_vld		: 1;
	ca_uint64_t	ip_ecn_en		: 1;
	ca_uint64_t	ip_tos_vld		: 1;
	ca_uint64_t	ip_tos_6		: 1;
	ca_uint64_t	ip_dscp_marked_vld	: 1;
	ca_uint64_t	ip_dscp_marked_down	: 6;
	ca_uint64_t	ip_dscp_markdown_en	: 1;
	ca_uint64_t	ip_dscp_update_en	: 2;
	ca_uint64_t	ip_dscp_sel		: 3;
	ca_uint64_t	ip_dscp			: 6;
	ca_uint64_t	dscp_table_sel		: 1;
	ca_uint64_t	inner_802_1p_sel	: 3;
	ca_uint64_t	inner_802_1p		: 3;
	ca_uint64_t	top_802_1p_sel		: 3;
	ca_uint64_t	top_802_1p		: 3;
	ca_uint64_t	qos_802_1p_table_sel	: 1;
	ca_uint64_t	cos_table_sel		: 1;
	ca_uint64_t	cos_sel			: 2;
	ca_uint64_t	cos			: 3;

	/* group 18 */
	ca_uint64_t	mrr_vld			: 1;
	ca_uint64_t	mrr_en			: 1;
	ca_uint64_t	no_drop_vld		: 1;
	ca_uint64_t	no_drop			: 1;
	ca_uint64_t	dpid_vld		: 1;
	ca_uint64_t	dpid_pri		: 1;
	ca_uint64_t	permit			: 1;
	ca_uint64_t	deepq			: 1;
	ca_uint64_t	mcgid			: 10;
	ca_uint64_t	mc			: 1;

	/* group 19 */
	ca_uint64_t	chk_msk_ptr		: 6;
	ca_uint64_t	cache_ctrl		: 2;
	ca_uint64_t	pop_l3_vld		: 1;
	ca_uint64_t	pop_l3_chk_ecn_en	: 1;
	ca_uint64_t	pop_l3_en		: 1;
	ca_uint64_t	keep_ts_vld		: 1;
	ca_uint64_t	keep_ts_en		: 1;
	ca_uint64_t	keep_orig_pkt_vld	: 1;
	ca_uint64_t	keep_orig_pkt		: 1;
	ca_uint64_t	stage3_ctrl_vld		: 1;
	ca_uint64_t	stage3_ctrl		: 1;
	ca_uint64_t	stage2_ctrl_vld		: 1;
	ca_uint64_t	stage2_ctrl		: 1;
	ca_uint64_t	t5_ctrl_vld		: 1;
	ca_uint64_t	t5_ctrl			: 4;
	ca_uint64_t	t4_ctrl_vld		: 1;
	ca_uint64_t	t4_ctrl			: 4;
	ca_uint64_t	t3_ctrl_vld		: 1;
	ca_uint64_t	t3_ctrl			: 4;
	ca_uint64_t	t2_ctrl_vld		: 1;
	ca_uint64_t	t2_ctrl			: 4;
} __attribute__((packed)) aal_hash_action_t;

typedef struct {
	/* L4 */
	ca_uint32_t	l4_chksum		: 32;
	ca_uint32_t	l4_chksum_zero		: 1;
	ca_uint32_t	tcp_rdp_ctrl		: 9;
	ca_uint32_t	l4_port_rng_match_vec	: 32;
	ca_uint32_t	l4_dp			: 16;
	ca_uint32_t	l4_sp			: 16;

	/* IP */
	ca_uint32_t	l3_chksum_err		: 1;
	ca_uint32_t	l3_chksum		: 16;
	ca_uint32_t	spi			: 32;
	ca_uint32_t	spi_vld			: 3;
	ca_uint32_t	icmp_type		: 8;
	ca_uint32_t	icmp_vld		: 3;
	ca_uint32_t	ipv6_doh		: 1;
	ca_uint32_t	ipv6_rh			: 1;
	ca_uint32_t	ipv6_hbh		: 1;
	ca_uint32_t	ipv6_ndp		: 1;
	ca_uint32_t	ip_fragment		: 1;
	ca_uint32_t	ip_options		: 1;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	ipv6_flow_lbl		: 20;
	ca_uint32_t	ip_da_sa_equal		: 1;
	ca_uint32_t	ip_da_mc		: 1;
	ca_uint32_t	ip_da_enc		: 4;
	ca_uint32_t	ip_da_0			: 32;
	ca_uint32_t	ip_da_1			: 32;
	ca_uint32_t	ip_da_2			: 32;
	ca_uint32_t	ip_da_3			: 32;
	ca_uint32_t	ip_sa_enc		: 4;
	ca_uint32_t	ip_sa_0			: 32;
	ca_uint32_t	ip_sa_1			: 32;
	ca_uint32_t	ip_sa_2			: 32;
	ca_uint32_t	ip_sa_3			: 32;
	ca_uint32_t	ip_l4_type		: 3;
	ca_uint32_t	ip_protocol		: 8;

	/* QoS DSCP */
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_ecn			: 2;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp			: 6;

	/* L3 */
	ca_uint32_t	ip_mtu_en		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	ip_ver			: 1;
	ca_uint32_t	ip_vld			: 1;

	/* PPP / PPPoE */
	ca_uint32_t	ppp_protocol_enc	: 4;
	ca_uint32_t	pppoe_session_id	: 16;
	ca_uint32_t	pppoe_code_enc		: 4;
	ca_uint32_t	pppoe_type		: 2;

	/* 802.1p */
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p		: 3;

	/* VLAN */
	ca_uint32_t	inner_dei		: 1;
	ca_uint32_t	inner_vid		: 12;
	ca_uint32_t	inner_tpid_enc		: 3;
	ca_uint32_t	top_dei			: 1;
	ca_uint32_t	top_vid			: 12;
	ca_uint32_t	top_tpid_enc		: 3;
	ca_uint32_t	vlan_cnt		: 2;

	/* L2 format */
	ca_uint32_t	llc_type_enc		: 2;
	ca_uint32_t	pktlen_rng_match_vec	: 4;
	ca_uint32_t	len_encoded		: 1;
	ca_uint32_t	llc_snap		: 2;

	/* L2 */
	ca_uint32_t	ethertype_enc		: 6;
	ca_uint32_t	ethertype		: 16;
	ca_uint32_t	mac_sa_0		: 8;
	ca_uint32_t	mac_sa_1		: 8;
	ca_uint32_t	mac_sa_2		: 8;
	ca_uint32_t	mac_sa_3		: 8;
	ca_uint32_t	mac_sa_4		: 8;
	ca_uint32_t	mac_sa_5		: 8;
	ca_uint32_t	mac_da_rsvd		: 1;
	ca_uint32_t	mac_da_rng		: 1;
	ca_uint32_t	mac_da_ip_mc		: 1;
	ca_uint32_t	l2_pkt_type		: 3;
	ca_uint32_t	mac_da_an_sel		: 4;
	ca_uint32_t	mac_da_0		: 8;
	ca_uint32_t	mac_da_1		: 8;
	ca_uint32_t	mac_da_2		: 8;
	ca_uint32_t	mac_da_3		: 8;
	ca_uint32_t	mac_da_4		: 8;
	ca_uint32_t	mac_da_5		: 8;

	/* Special Packet */
	ca_uint32_t	spcl_pkt_hdr_mtch	: 8;
	ca_uint32_t	spcl_pkt_enc		: 6;

	/* MDATA */
	ca_uint32_t	mdata_l			: 32;
	ca_uint32_t	mdata_h			: 32;

	/* Policer ID */
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_id			: 9;
	ca_uint32_t	pol_grp_id		: 3;

	/* COS */
	ca_uint32_t	cos			: 3;

	/* Destination Port ID */
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	ca_uint32_t	dpid_pri		: 1;

	/* Source Port ID */
	ca_uint32_t	o_lspid			: 6;
	ca_uint32_t	lspid			: 6;
	ca_uint32_t	pspid			: 4;

	/* CPU Header */
	ca_uint32_t	hash_idx		: 21;
	ca_uint32_t	cls_action		: 12;
	ca_uint32_t	cache_way		: 5;
	ca_uint32_t	hash_dbl_chk_fail	: 3;
	ca_uint32_t	fwd_vld			: 9;

	/* Info for PE module */
	ca_uint32_t	drop_src		: 6;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	l3_outer_popped		: 1;
	ca_uint32_t	l4_rdp_hdr_len		: 8;
	ca_uint32_t	l3_total_len		: 14;
	ca_uint32_t	l2_payload_len		: 14;
	ca_uint32_t	l2_orig_hdr_len		: 7;
	ca_uint32_t	l3_inner_offset		: 7;
	ca_uint32_t	l3_offset		: 7;
	ca_uint32_t	l4_offset		: 7;
	ca_uint32_t	orig_packet_len		: 14;
	ca_uint32_t	ip_ihl			: 4;

	/* Table Control */
	ca_uint32_t	hdr_ptp			: 1;
	ca_uint32_t	keep_ts			: 1;
	ca_uint32_t	keep_org_pkt		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl			: 4;
	ca_uint32_t	t1_ctrl			: 4;
	ca_uint32_t	t0_ctrl			: 1;

	/* align to 32-bit */
	ca_uint32_t	padding			: 26;
} __attribute__((packed)) L3FE_HDR_I_t;

ca_status_t ca_hash_mask_unmask (
	CA_IN		ca_device_id_t			device_id,
	CA_IN_OUT	aal_hash_mask_t			*hash_mask );

#endif /* __AAL_COMMON_IROS_H__ */

