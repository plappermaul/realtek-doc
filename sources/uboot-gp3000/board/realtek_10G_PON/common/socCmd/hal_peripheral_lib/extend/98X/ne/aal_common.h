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
 * aal_common.h:
 *
 */
#ifndef __AAL_COMMON_H__
#define __AAL_COMMON_H__

//#include "os_core.h"
//#include "osal_cmn.h"
//#include "ca_types.h"
#if 0//marklin remove
#include <soc/cortina/registers.h>
#else//marklin add
#include "g3lite_registers.h"
#include <linux/types.h>
//#include <asm/types.h>

#define uint8_t        ca_uint8_t
#define uint16_t        ca_uint16_t
#define uint32_t        ca_uint32_t
#define uint64_t        ca_uint64_t

#define CONFIG_98F_UBOOT_NE_DBG        0

typedef unsigned int    __kernel_size_t;

#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t size_t;
#endif

typedef     unsigned int SIZE_T;
extern void *memset( void *s, int c, SIZE_T n );

#ifndef bool
typedef unsigned char    bool;
#endif

typedef unsigned long   uintptr_t;

#endif

/*************************************************************************
 * AAL HW info
 *************************************************************************/

typedef struct {
	ca_uint32_t hardware_id;
	ca_uint32_t hardware_rev;
	ca_uint32_t hardware_date;
	ca_uint32_t api_version;
} aal_info_t;

int aal_hw_info_get(aal_info_t *info);

/*************************************************************************
 * hash tuple
 *************************************************************************/

typedef enum {
	TUPLE_TYPE_UNUSED = -1, // Unused tuple; flag for software only
	TUPLE_TYPE_FWD = 0,     // Type-0 tuple, Forwarding tuple
	TUPLE_TYPE_QOS = 1,     // Type-1 tuple, QoS tuple
} aal_hash_tuple_type_e;

/*************************************************************************
 * hash action group
 *************************************************************************/

/* action group bitmask */
#define ACTION_GROUP_00		0x00000001	/* Hash Check */
#define ACTION_GROUP_01		0x00000002	/* L4 DP */
#define ACTION_GROUP_02		0x00000004	/* L4 SP */
#define ACTION_GROUP_03		0x00000008	/* IP DA Low 32 bits */
#define ACTION_GROUP_04		0x00000010	/* IP DA High 96 bits */
#define ACTION_GROUP_05		0x00000020	/* IP SA Low 32 bits */
#define ACTION_GROUP_06		0x00000040	/* IP SA High 96 bits */
#define ACTION_GROUP_07		0x00000080	/* PPPoE */
#define ACTION_GROUP_08		0x00000100	/* L3 Egress Interface */
#define ACTION_GROUP_09		0x00000200	/* VLAN */
#define ACTION_GROUP_10		0x00000400	/* MAC SA */
#define ACTION_GROUP_11		0x00000800	/* MAC DA */
#define ACTION_GROUP_12		0x00001000	/* MDATA Word 0 */
#define ACTION_GROUP_13		0x00002000	/* MDATA Word 1 */
#define ACTION_GROUP_14		0x00004000	/* MDATA Word 2 */
#define ACTION_GROUP_15		0x00008000	/* MDATA Word 3 */
#define ACTION_GROUP_16		0x00010000	/* pol */
#define ACTION_GROUP_17		0x00020000	/* QoS */
#define ACTION_GROUP_18		0x00040000	/* Dest Port ID */
#define ACTION_GROUP_19		0x00080000	/* Control */

/* the size of each action group in bits */
#define ACTION_GROUP_00_BITS	150
#define ACTION_GROUP_01_BITS	17
#define ACTION_GROUP_02_BITS	17
#define ACTION_GROUP_03_BITS	33
#define ACTION_GROUP_04_BITS	97
#define ACTION_GROUP_05_BITS	33
#define ACTION_GROUP_06_BITS	97
#define ACTION_GROUP_07_BITS	18
#define ACTION_GROUP_08_BITS	20
#define ACTION_GROUP_09_BITS	39
#define ACTION_GROUP_10_BITS	49
#define ACTION_GROUP_11_BITS	49
#define ACTION_GROUP_12_BITS	20
#define ACTION_GROUP_13_BITS	20
#define ACTION_GROUP_14_BITS	20
#define ACTION_GROUP_15_BITS	20
#define ACTION_GROUP_16_BITS	22
#define ACTION_GROUP_17_BITS	43
#define ACTION_GROUP_18_BITS	19
#define ACTION_GROUP_19_BITS	39

/*************************************************************************
 * hash action
 *************************************************************************/

/* 150 bits */
#define HASH_ACTION_GROUP_00	\
	ca_uint64_t	chk_l4_dp_vld		: 1;	\
	ca_uint64_t	chk_l4_dp		: 16;	\
	ca_uint64_t	chk_l4_sp_vld		: 1;	\
	ca_uint64_t	chk_l4_sp		: 16;	\
	ca_uint64_t	chk_ip_da_vld		: 1;	\
	ca_uint64_t	chk_ip_da		: 32;	\
	ca_uint64_t	chk_ip_sa_vld		: 1;	\
	ca_uint64_t	chk_ip_sa		: 32;	\
	ca_uint64_t	chk_mac_da_vld		: 1;	\
	ca_uint64_t	chk_mac_da		: 24;	\
	ca_uint64_t	chk_mac_sa_vld		: 1;	\
	ca_uint64_t	chk_mac_sa		: 24;

/* 17 bits */
#define HASH_ACTION_GROUP_01	\
	ca_uint64_t	l4_dp_vld		: 1;	\
	ca_uint64_t	l4_dp			: 16;

/* 17 bits */
#define HASH_ACTION_GROUP_02	\
	ca_uint64_t	l4_sp_vld		: 1;	\
	ca_uint64_t	l4_sp			: 16;

/* 33 bits */
#define HASH_ACTION_GROUP_03	\
	ca_uint64_t	ip_da_l_vld		: 1;	\
	ca_uint64_t	ip_da_0			: 32; /* LSB */

/* 97 bits */
#define HASH_ACTION_GROUP_04	\
	ca_uint64_t	ip_da_h_vld		: 1;	\
	ca_uint64_t	ip_da_1			: 32;	\
	ca_uint64_t	ip_da_2			: 32;	\
	ca_uint64_t	ip_da_3			: 32; /* MSB */

/* 33 bits */
#define HASH_ACTION_GROUP_05	\
	ca_uint64_t	ip_sa_l_vld		: 1;	\
	ca_uint64_t	ip_sa_0			: 32; /* LSB */

/* 97 bits */
#define HASH_ACTION_GROUP_06	\
	ca_uint64_t	ip_sa_h_vld		: 1;	\
	ca_uint64_t	ip_sa_1			: 32;	\
	ca_uint64_t	ip_sa_2			: 32;	\
	ca_uint64_t	ip_sa_3			: 32; /* MSB */

/* 18 bits */
#define HASH_ACTION_GROUP_07	\
	ca_uint64_t	pppoe_vld		: 1;	\
	ca_uint64_t	pppoe_set		: 1;	\
	ca_uint64_t	pppoe_session_id	: 16;

/* 20 bits */
#define HASH_ACTION_GROUP_08	\
	ca_uint64_t	ip_ttl_vld		: 1;	\
	ca_uint64_t	ip_ttl_zero_discard_en	: 1;	\
	ca_uint64_t	ip_ttl_cmd		: 2;	\
	ca_uint64_t	ip_ttl			: 8;	\
	ca_uint64_t	six_rd_ipda_ctrl_vld	: 1;	\
	ca_uint64_t	six_rd_ipda_from_v6	: 1;	\
	ca_uint64_t	ip_mtu_enc_vld		: 1;	\
	ca_uint64_t	ip_mtu_enc		: 2;	\
	ca_uint64_t	l2_format_vld		: 1;	\
	ca_uint64_t	l2_format		: 1;	\
	ca_uint64_t	l3_egress_if_vld	: 1;

/* 39 bits */
#define HASH_ACTION_GROUP_09	\
	ca_uint64_t	vlan_vld		: 1;	\
	ca_uint64_t	inner_dei_sel		: 2;	\
	ca_uint64_t	inner_dei		: 1;	\
	ca_uint64_t	inner_vid		: 12;	\
	ca_uint64_t	inner_tpid_enc		: 3;	\
	ca_uint64_t	top_dei_sel		: 2;	\
	ca_uint64_t	top_dei			: 1;	\
	ca_uint64_t	top_vid			: 12;	\
	ca_uint64_t	top_tpid_enc		: 3;	\
	ca_uint64_t	vlan_cnt		: 2;

/* 49 bits */
#define HASH_ACTION_GROUP_10	\
	ca_uint64_t	mac_sa_vld		: 1;		\
	ca_uint64_t	mac_sa_0		: 8; /* LSB */	\
	ca_uint64_t	mac_sa_1		: 8;		\
	ca_uint64_t	mac_sa_2		: 8;		\
	ca_uint64_t	mac_sa_3		: 8;		\
	ca_uint64_t	mac_sa_4		: 8;		\
	ca_uint64_t	mac_sa_5		: 8; /* MSB */

/* 49 bits */
#define HASH_ACTION_GROUP_11	\
	ca_uint64_t	mac_da_vld		: 1;		\
	ca_uint64_t	mac_da_0		: 8; /* LSB */	\
	ca_uint64_t	mac_da_1		: 8;		\
	ca_uint64_t	mac_da_2		: 8;		\
	ca_uint64_t	mac_da_3		: 8;		\
	ca_uint64_t	mac_da_4		: 8;		\
	ca_uint64_t	mac_da_5		: 8; /* MSB */

/* 20 bits */
#define HASH_ACTION_GROUP_12	\
	ca_uint64_t	mdata_w_vld_0		: 4;	\
	ca_uint64_t	mdata_w_0		: 16;

/* 20 bits */
#define HASH_ACTION_GROUP_13	\
	ca_uint64_t	mdata_w_vld_1		: 4;	\
	ca_uint64_t	mdata_w_1		: 16;

/* 20 bits */
#define HASH_ACTION_GROUP_14	\
	ca_uint64_t	mdata_w_vld_2		: 4;	\
	ca_uint64_t	mdata_w_2		: 16;

/* 20 bits */
#define HASH_ACTION_GROUP_15	\
	ca_uint64_t	mdata_w_vld_3		: 4;	\
	ca_uint64_t	mdata_w_3		: 16;

/* 22 bits */
#define HASH_ACTION_GROUP_16	\
	ca_uint64_t	pol_vld			: 1;	\
	ca_uint64_t	qos_premark		: 1;	\
	ca_uint64_t	pol_all_bypass		: 1;	\
	ca_uint64_t	pol_en			: 2;	\
	ca_uint64_t	pol_base		: 9;	\
	ca_uint64_t	pol_sel			: 3;	\
	ca_uint64_t	pol_table_sel		: 1;	\
	ca_uint64_t	pol_grp_en		: 1;	\
	ca_uint64_t	pol_grp_id		: 3;

/* 43 bits */
#define HASH_ACTION_GROUP_17	\
	ca_uint64_t	ip_ecn_vld		: 1;	\
	ca_uint64_t	ip_ecn_en		: 1;	\
	ca_uint64_t	ip_tos_vld		: 1;	\
	ca_uint64_t	ip_tos_6		: 1;	\
	ca_uint64_t	ip_dscp_markdown_vld	: 1;	\
	ca_uint64_t	ip_dscp_marked_down	: 6;	\
	ca_uint64_t	ip_dscp_markdown_en	: 1;	\
	ca_uint64_t	ip_dscp_update_en	: 2;	\
	ca_uint64_t	ip_dscp_sel		: 3;	\
	ca_uint64_t	ip_dscp			: 6;	\
	ca_uint64_t	dscp_table_sel		: 1;	\
	ca_uint64_t	inner_802_1p_sel	: 3;	\
	ca_uint64_t	inner_802_1p		: 3;	\
	ca_uint64_t	top_802_1p_sel		: 3;	\
	ca_uint64_t	top_802_1p		: 3;	\
	ca_uint64_t	qos_802_1p_table_sel	: 1;	\
	ca_uint64_t	cos_table_sel		: 1;	\
	ca_uint64_t	cos_sel			: 2;	\
	ca_uint64_t	cos			: 3;

/* 19 bits */
#define HASH_ACTION_GROUP_18	\
	ca_uint64_t	mrr_vld			: 1;	\
	ca_uint64_t	mrr_en			: 1;	\
	ca_uint64_t	no_drop_vld		: 1;	\
	ca_uint64_t	no_drop			: 1;	\
	ca_uint64_t	dpid_vld		: 1;	\
	ca_uint64_t	dpid_pri		: 1;	\
	ca_uint64_t	permit			: 1;	\
	ca_uint64_t	deepq			: 1;	\
	ca_uint64_t	mcgid			: 10;	\
	ca_uint64_t	mc			: 1;	\

/* 39 bits */
#define HASH_ACTION_GROUP_19	\
	ca_uint64_t	chk_msk_ptr		: 6;	\
	ca_uint64_t	cache_ctrl		: 2;	\
	ca_uint64_t	pop_l3_vld		: 1;	\
	ca_uint64_t	pop_l3_chk_ecn_en	: 1;	\
	ca_uint64_t	pop_l3_en		: 1;	\
	ca_uint64_t	keep_ts_vld		: 1;	\
	ca_uint64_t	keep_ts_en		: 1;	\
	ca_uint64_t	keep_orig_pkt_vld	: 1;	\
	ca_uint64_t	keep_orig_pkt		: 1;	\
	ca_uint64_t	stage3_ctrl_vld		: 1;	\
	ca_uint64_t	stage3_ctrl		: 1;	\
	ca_uint64_t	stage2_ctrl_vld		: 1;	\
	ca_uint64_t	stage2_ctrl		: 1;	\
	ca_uint64_t	t5_ctrl_vld		: 1;	\
	ca_uint64_t	t5_ctrl			: 4;	\
	ca_uint64_t	t4_ctrl_vld		: 1;	\
	ca_uint64_t	t4_ctrl			: 4;	\
	ca_uint64_t	t3_ctrl_vld		: 1;	\
	ca_uint64_t	t3_ctrl			: 4;	\
	ca_uint64_t	t2_ctrl_vld		: 1;	\
	ca_uint64_t	t2_ctrl			: 4;

/* NOTE:
 *  1. This is dummy struct for compiling. Don't use it except in iROS.
 *  2. This must be same as aal_hash_action_t in aal_common_iros.h.
 */
typedef struct aal_hash_action_s {
	HASH_ACTION_GROUP_00;
	HASH_ACTION_GROUP_01;
	HASH_ACTION_GROUP_02;
	HASH_ACTION_GROUP_03;
	HASH_ACTION_GROUP_04;
	HASH_ACTION_GROUP_05;
	HASH_ACTION_GROUP_06;
	HASH_ACTION_GROUP_07;
	HASH_ACTION_GROUP_08;
	HASH_ACTION_GROUP_09;
	HASH_ACTION_GROUP_10;
	HASH_ACTION_GROUP_11;
	HASH_ACTION_GROUP_12;
	HASH_ACTION_GROUP_13;
	HASH_ACTION_GROUP_14;
	HASH_ACTION_GROUP_15;
	HASH_ACTION_GROUP_16;
	HASH_ACTION_GROUP_17;
	HASH_ACTION_GROUP_18;
	HASH_ACTION_GROUP_19;
} __attribute__((packed)) aal_hash_action_t;

/*************************************************************************
 * hash mask
 *************************************************************************/

typedef struct {
	/* L4 */
	ca_uint64_t	l4_chksum_zero		: 1;
	ca_uint64_t	tcp_rdp_ctrl		: 9;	/* TCP flag {N,C,E,U,A,P,R,S,F}, or RUDP flag {0,S,A,E,R,N,C,T,0} */
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
	ca_uint64_t	table_id		: 1;	/* obsolete; always set to 1 */

	/* hash mask bit range is [220:0], padding to 32-bit alignment */
	ca_uint64_t	padding			: 4;
} __attribute__((packed)) aal_hash_mask_t;

/*************************************************************************
 * hash key
 *************************************************************************/

typedef struct {
	/* L4 */
	ca_uint64_t	l4_chksum_zero		: 1;
	ca_uint64_t	tcp_rdp_ctrl		: 9;	/* TCP flag {N,C,E,U,A,P,R,S,F}, or RUDP flag {0,S,A,E,R,N,C,T,0} */
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
	ca_uint64_t	ip_da_0			: 32;	/* IPv4_DA or IPv6_DA LSB; host order */
	ca_uint64_t	ip_da_1			: 32;
	ca_uint64_t	ip_da_2			: 32;
	ca_uint64_t	ip_da_3			: 32;	/* IPv6_DA MSB; host order */
	ca_uint64_t	ip_sa_0			: 32;	/* IPv4_SA or IPv6_SA LSB; host order */
	ca_uint64_t	ip_sa_1			: 32;
	ca_uint64_t	ip_sa_2			: 32;
	ca_uint64_t	ip_sa_3			: 32;	/* IPv6_SA MSB; host order */
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
	ca_uint64_t	mac_sa_0		: 8;	/* LSB; bit[7:0] */
	ca_uint64_t	mac_sa_1		: 8;
	ca_uint64_t	mac_sa_2		: 8;
	ca_uint64_t	mac_sa_3		: 8;
	ca_uint64_t	mac_sa_4		: 8;
	ca_uint64_t	mac_sa_5		: 8;	/* MSB; bit[47:40] */
	ca_uint64_t	mac_da_rsvd		: 1;
	ca_uint64_t	mac_da_rng		: 1;
	ca_uint64_t	mac_da_ip_mc		: 1;
	ca_uint64_t	mac_da_an_sel		: 4;
	ca_uint64_t	mac_da_0		: 8;	/* LSB; bit[7:0] */
	ca_uint64_t	mac_da_1		: 8;
	ca_uint64_t	mac_da_2		: 8;
	ca_uint64_t	mac_da_3		: 8;
	ca_uint64_t	mac_da_4		: 8;
	ca_uint64_t	mac_da_5		: 8;	/* MSB; bit[47:40] */
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
	ca_uint64_t	hkey_id			: 6;	/* hashmask index in Mask Table */
	ca_uint64_t	ctrl_set_id		: 4;	/* profile ID */
	ca_uint64_t	table_id		: 4;	/* obsolete */
	ca_uint64_t	reserved		: 4;
	/* TODO: padding */
} __attribute__((packed)) aal_hash_key_t;

/*************************************************************************
 * hash value
 *************************************************************************/

typedef struct {
	ca_uint32_t	crc32		: 32;

	/* Hash Table needn't the following fields, but code need crc16 for indexing */
	ca_uint32_t	crc16		: 16;
	ca_uint32_t	valid		: 1;
	ca_uint32_t	reserved	: 15;
} __attribute__((packed)) aal_hash_value_t;

/*************************************************************************
 * macro for indirect access
 *************************************************************************/

#define AAL_ASSERT_RET(x, ret) \
	do { \
		if (!(x)) { \
			printf("%s() _%d_: assert fail\n", __FUNCTION__, __LINE__); \
			return ret; \
		} \
	} while (0)

#if 0
typedef struct {
	APB0_L2FE_t L2FE;
} G3_DEV_t;
#endif

#define CA_REG_READ_1(REG_ADDR)		*(volatile ca_uint32*)REG_ADDR

#define CA_REG_WRITE_1(REG_ADDR, REG_DATA) \
	do {\
		*(volatile ca_uint32*)(REG_ADDR) = (REG_DATA); \
	} while (0)

#if 0//Yocto
#define CA_NE_REG_READ(addr)			CA_REG_READ((uintptr_t)AAL_NE_REG_ADDR(addr))
#define CA_NE_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_NE_REG_ADDR(addr))

#define CA_GLB_NE_INTR_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_GLB_NE_INTR_REG_ADDR(addr))
#define CA_GLB_NE_INTR_REG_WRITE(data, addr)	CA_REG_WRITE(data, (uintptr_t)AAL_GLB_NE_INTR_REG_ADDR(addr))

#define CA_L2FE_REG_READ(addr)			CA_REG_READ((uintptr_t)AAL_NE_REG_ADDR(addr))
#define CA_L2FE_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_NE_REG_ADDR(addr))

#define CA_NI_REG_READ(addr)			CA_REG_READ((uintptr_t)AAL_NE_REG_ADDR(addr))
#define CA_NI_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_NE_REG_ADDR(addr))
#else//sd1 uboot for 98f
#define CA_NE_REG_READ(addr)			CA_REG_READ((volatile uintptr_t)(addr))
#define CA_NE_REG_WRITE(data, addr)		CA_REG_WRITE(data, (volatile uintptr_t)(addr))

#define CA_GLB_NE_INTR_REG_READ(addr)		CA_REG_READ((volatile uintptr_t)(addr))
#define CA_GLB_NE_INTR_REG_WRITE(data, addr)	CA_REG_WRITE(data, (volatile uintptr_t)(addr))

#define CA_L2FE_REG_READ(addr)			CA_REG_READ((volatile uintptr_t)(addr))
#define CA_L2FE_REG_WRITE(data, addr)		CA_REG_WRITE(data, (volatile uintptr_t)(addr))

#define CA_NI_REG_READ(addr)			CA_REG_READ((volatile uintptr_t)(addr))
#define CA_NI_REG_WRITE(data, addr)		CA_REG_WRITE(data, (volatile uintptr_t)(addr))
#endif

#define CA_PER_MDIO_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_PER_MDIO_REG_ADDR(addr))
#define CA_PER_MDIO_REG_WRITE(data, addr)	CA_REG_WRITE(data, (uintptr_t)AAL_PER_MDIO_REG_ADDR(addr))

#define FIND_INDIRCT_ADDRESS(rbw, index)	(((ca_uint32_t)1<<31)|((ca_uint32_t)(rbw)<<30)|(ca_uint32_t)(index))
#define CHECK_INDIRCT_OPERATE_STATE(dev,addr)	do{\
                                                    ca_uint32_t i=0;\
                                                    ca_uint32_t val32;\
                                                    for(i = 0; i<10000; i++){\
                                                       val32 = CA_NE_REG_READ((ca_uint32_t)addr);\
                                                       if((val32 & ((ca_uint32)1 <<31)) == 0){\
                                                        break;\
                                                        }\
                                                    }\
                                                    if (i>= 10000) \
                                                        printf("%s(): access %s timeout.\n", __func__, #addr);\
                                                }while(0)


#define DO_INDIRCT_OP(dev,rbw, indx, raddr) CA_NE_REG_WRITE(FIND_INDIRCT_ADDRESS(rbw, indx), raddr); \
                                  CHECK_INDIRCT_OPERATE_STATE(dev,raddr)

#define WRITE_INDIRCT_REG(dev,index,addr) DO_INDIRCT_OP(dev,1,index,addr)
#define READ_INDIRCT_REG(dev,index,addr) DO_INDIRCT_OP(dev,0,index,addr)

#define REG_L2FE_FIELD_READ(reg, field, val)                  do{\
                                                         reg##_t reg_data;\
                                                         reg_data.wrd = CA_NE_REG_READ(reg);\
                                                         (val) = reg_data.bf.field;\
                                                         }while(0)

#define REG_L2FE_FIELD_WRITE(reg, field, val)                 do{\
                                                         reg##_t reg_data;\
                                                         reg_data.wrd = CA_NE_REG_READ(reg);\
                                                         reg_data.bf.field = (val);\
                                                         CA_NE_REG_WRITE(reg_data.wrd, reg);\
                                                         }while(0)

/************************************************************************
**                   AAL table access opeartion
*************************************************************************/
typedef enum {
	AAL_TBL_OP_FLAG_RD = 0,
	AAL_TBL_OP_FLAG_WR,
} aal_tbl_op_flag_t;

#define AAL_TBL_OP_TIMEOUT	0xfff

/************************************************************************
**                  HEADER A
*************************************************************************/
/* from G3 Header_v32.xlsm */
typedef union {
	uint64_t bits64;
	struct {
		uint32_t bits32_h		: 32;
		uint32_t bits32_l		: 32;
	} __attribute__((packed)) bits32;
	struct header_a {

		union {
			struct {
				uint32_t mcgid			: 8;	/* When packet type is 2'b10(CPU), redefined as L2 learn info */
				uint32_t drop_code		: 3;	/* Drop event code, used by TE */
				uint32_t rx_pkt_type		: 2;	/* 00:UC, 01:BC, 10:MC, 11:Unknown-UC
									   for l3fe->l2fe traffic, use uc(00) to disable l2 learning
									   uuc(11) to enable l2 learning */
				uint32_t no_drop		: 1;	/* No-Drop flag for high priority packet */
				uint32_t mirror			: 1;	/* Packet to be mirrored */
				uint32_t mark			: 1;	/* Conjestion experienced in QM */
				uint32_t pol_en			: 2;	/* 2'b00: Policer disable
									   2'b01: L2 Policer enable
									   2'b10: L3 Policer enable
									   2'b11: both L2,L3 Policer enable
									 */

				uint32_t pol_id			: 9;	/* Used for Policing in TE/QTM */
				uint32_t pol_grp_id		: 3;	/* Policer Group ID, group 0 is NULL */
				uint32_t deep_q			: 1;	/* Packet target to DeepQ in QM */
				uint32_t cpu_flg		: 1;	/* CPU header is included */
			} pkt_info;

			struct {
				uint32_t ptp_info		: 31;	/* Ingress PTP traffic, FE copies TS to PTP_INFO.
									   Egress PTP traffic carries PTP_CMD in PTP_INFO field.
									   This field is overloaded with {POL_EN,POL_GRP_ID,POL_ID,MIRROR,
									   DEEP_Q,MARK,CPU_FLAG,NO_DROP, RESERVED,RX_PKT_TYPE,MCGID}fields,
									   valid when HDR_TYPE is 11
									 */
				uint32_t cpu_flg		: 1;	/* CPU header is included */
			} ptp_info;

			struct {
				uint32_t cmd_asel		: 2;	/* select one of 4 asymmetry value programmed in CSRs */
				uint32_t cmd_op			: 2;	/* 00: NO_OP, take timestamp to global CSR (2 pass mode), interrupt CPU
									   01: TS_CORRECT, use timestamp without asymmetry offset
									   10: TS_P_OFFSET, use timestamp and (+) offset
									   11: TS_N_OFFSET, use timestamp and (-) offset
									 */
				uint32_t reserved_0		: 1;
				uint32_t cmd_mode		: 1;	/* 1: timestamp mode, if cmd_op != NO_OP, write timestamp to
									      location defined by cmd_offset
									   0: correction field mode, if cmd_op != NO_OP, take location
									      defined by cmd_offset, and timestamp, write it back to same
									      location (correction field)
									 */
				uint32_t ptp_type		: 2;	/* PTP packet type
									   00: PTP over Ethernet
									   01: CFM
									   10: PTP over UDP over IPv4; need to update UDP cksum if it is non-zero
									   11: PTP over UDP over IPv6; need to update UDP cksum
									 */
				uint32_t cmd_offset		: 7;	/* Start of Timestamp or correction field. This is the byte number
									   from the SOP of the packet
									 */
				uint32_t reserved_1		: 1;
				uint32_t udp_cksum_offset	: 7;	/* offset for UDP checksum field if ptp_type is PTP over UDP */
				uint32_t reserved_2		: 8;
				uint32_t cpu_flg		: 1;	/* CPU header is included */
			} ptp_cmd;
		} bits_32_63;

			uint32_t cos			: 3;	/* COS */
			uint32_t ldpid			: 6;	/* Logical Destination Port ID, Packet from MC Engine, this field carries MC_INDEX */
			uint32_t lspid			: 6;	/* Original Logical Source Port ID */
			uint32_t pkt_size		: 14;	/* Packet size */
			uint32_t fe_bypass		: 1;	/* Bypass L2FE and L3FE processing */
			uint32_t hdr_type		: 2;	/* 00: Generic* 01: MC HDR
								 * 01: MC HDR
								 * 10: CPU bound pkt
								 * 11: PTP ingress or egress pkt
								 */

	} bits;
} HEADER_A_T;

/*************************************************************************
 * hash related
 *************************************************************************/

uint32_t ca_crc32(uint32_t crc, unsigned char *p, size_t len);
uint16_t ca_crc16(uint16_t crc, unsigned char *p, size_t len);

void hash_mask_unmask(aal_hash_mask_t *mask);
int hash_value_calculate(aal_hash_key_t *hash_key, aal_hash_mask_t *hash_mask, aal_hash_value_t *hash_value);
void hash_key_dump(aal_hash_key_t *hash_key);

/*************************************************************************
 * scfg related
 *************************************************************************/

ca_status_t aal_scfg_read(ca_uint8_t *id, int len, void *value);

/*************************************************************************
 * FPGA only
 *************************************************************************/

#define MODULE_NI		(1 << 0)
#define MODULE_L2		(1 << 0)
#define MODULE_L3FE		(1 << 1)
#define MODULE_PCIE		(1 << 2)
#define MODULE_SATA		(1 << 3)
#define MODULE_USB3		(1 << 4)

//bool aal_ni_is_l3fe_enabled(void);
bool aal_soc_has_module(unsigned int module);

#define SOC_HAS_NI()		(aal_soc_has_module(MODULE_NI))
#define SOC_HAS_L2()		(aal_soc_has_module(MODULE_L2))
#define SOC_HAS_L3FE()		(aal_soc_has_module(MODULE_L3FE))
#define SOC_HAS_PCIE()		(aal_soc_has_module(MODULE_PCIE))
#define SOC_HAS_SATA()		(aal_soc_has_module(MODULE_SATA))
#define SOC_HAS_USB3()		(aal_soc_has_module(MODULE_USB3))

/*************************************************************************
 * base address
 *************************************************************************/

extern void *g3_ne_reg_iobase;
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
extern void *hgu_8279_ne_reg_iobase;
#endif
extern void *g3_dma_reg_iobase;
extern void *g3_glb_reg_iobase;
extern void *g3_xram_reg_iobase;
extern void *g3_per_mdio_reg_iobase;
extern void *g3_glb_ne_intr_reg_iobase;

#define AAL_NE_REG_ADDR(addr)		(volatile)((((ca_uint32_t)(addr)) & 0x0000FFFF) + g3_ne_reg_iobase)
#define AAL_GLB_NE_INTR_REG_ADDR(addr)	(addr - GLOBAL_NETWORK_ENGINE_INTERRUPT_0 + g3_glb_ne_intr_reg_iobase)

#define AAL_DMA_REG_ADDR(addr)		(addr + g3_dma_reg_iobase)
#define AAL_GLB_REG_ADDR(addr)		(addr + g3_glb_reg_iobase)
#define AAL_XRAM_REG_ADDR(addr)		(addr + g3_xram_reg_iobase)
#define AAL_PER_MDIO_REG_ADDR(addr)	(addr - PER_MDIO_CFG + g3_per_mdio_reg_iobase)

void aal_ne_reg_base_init(void *iobase);
void aal_dma_reg_base_init(void *iobase);
void aal_glb_reg_base_init(void *iobase);
void aal_xram_reg_base_init(void *iobase);
void aal_per_mdio_reg_base_init(void *iobase);
void aal_glb_ne_intr_reg_base_init(void *iobase);

#ifndef CONFIG_CORTINA_BOARD_FPGA
extern void *g3_ne_phy_reg_iobase;
void aal_ne_phy_reg_base_init(void *iobase);

#define AAL_NE_PHY_REG_ADDR(addr)		((((ca_uint32_t)(addr)) & 0x00000FFF) + g3_ne_phy_reg_iobase)
#if 0//yocto
#define CA_NE_PHY_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_NE_PHY_REG_ADDR(addr))
#define CA_NE_PHY_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_NE_PHY_REG_ADDR(addr))
#else//sd1 uboot for 98f
#define CA_NE_PHY_REG_READ(addr)		CA_REG_READ((volatile uintptr_t)(addr))
#define CA_NE_PHY_REG_WRITE(data, addr)		CA_REG_WRITE(data, (volatile uintptr_t)(addr))
#endif//sd1 uboot for 98f

#endif

#if defined(CONFIG_CA_NE_AAL_XFI)
extern void *g3_ne_xfi_reg_iobase;
void aal_ne_xfi_reg_base_init(void *iobase);
#define AAL_NE_XFI_REG_ADDR(addr)               ((((ca_uint32_t)(addr)) - XFI_RG00) + g3_ne_xfi_reg_iobase)
#define CA_NE_XFI_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_NE_XFI_REG_ADDR(addr))
#define CA_NE_XFI_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_NE_XFI_REG_ADDR(addr))
#endif


static inline uint32_t ca_ne_reg_read(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, uint32_t addr)
{

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
	if (device_id == 1)
		return CA_HGU_8279_NE_REG_READ(addr);
#endif
	return CA_NE_REG_READ(addr);
}

static inline void ca_ne_reg_write(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, uint32_t data, uint32_t addr)
{
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
	if (device_id == 1) {
		CA_HGU_8279_NE_REG_WRITE(data, addr);
		return;
	}
#endif
	CA_NE_REG_WRITE(data, addr);
}

#define NOCOLOR		"\033[m"
#define RED				"\033[0;32;31m"
#define LIGHT_RED		"\033[1;31m"
#define GREEN			"\033[0;32;32m"
#define LIGHT_GREEN		"\033[1;32m"
#define BLUE				"\033[0;32;34m"
#define LIGHT_BLUE		"\033[1;34m"
#define DARK_GRAY		"\033[1;30m"
#define CYAN				"\033[0;36m"
#define LIGHT_CYAN		"\033[1;36m"
#define PURPLE			"\033[0;35m"
#define LIGHT_PURPLE	"\033[1;35m"
#define BROWN			"\033[0;33m"
#define YELLOW			"\033[0;33m"
#define LIGHT_YELLOW	"\033[1;33m"
#define LIGHT_GRAY		"\033[0;37m"
#define WHITE			"\033[1;37m"

#endif /* __AAL_COMMON_H__ */

