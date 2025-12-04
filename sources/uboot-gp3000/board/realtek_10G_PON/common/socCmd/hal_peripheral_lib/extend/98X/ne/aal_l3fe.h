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
 * aal_qm.h: Hardware Abstraction Layer for Layer-3 Forwarding Engine
 *
 */
#ifndef __AAL_L3FE_H__
#define __AAL_L3FE_H__

#include "ca_types.h"
#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif
#include "osal_cmn.h"
#include "aal_common.h"
#include "aal_table.h"
#include "aal_ni.h"

#define T1_CTRL_BYPASS	0xF
#define T2_CTRL_BYPASS	0xF
#define T3_CTRL_BYPASS	0xF
#define T4_CTRL_BYPASS	0xF
#define T5_CTRL_ENABLE	0x0
#define T5_CTRL_BYPASS	0xF

#define STAGE2_CTRL_UPDATE	0
#define STAGE2_CTRL_BYPASS	1
#define STAGE3_CTRL_UPDATE	0
#define STAGE3_CTRL_BYPASS	1

extern uint32_t ca_bypass_l3fe;

/***************************************************************************
 * GLB
 ***************************************************************************/

typedef volatile union {
  struct {
	  ca_uint32 lpid                 :  6 ; /* bits 5:0 */
	  ca_uint32 tpid                 : 16 ; /* bits 21:6 */
	  ca_uint32 _8021p                :  3 ; /* bits 24:22 */
	  ca_uint32 o_lspid_vld          :  1 ; /* bits 25 */
	  ca_uint32 lspid		 :  6 ; /* bits 31:26 */
  } bf ;
  ca_uint32     wrd ;
} L3FE_GLB_PORT_DEFAULT_BEHAVIOR_0_VCT_t;

typedef volatile union {
  struct {
          ca_uint32 top_tpid_ctrl        :  4 ; /* bits 3:0 */
          ca_uint32 inner_tpid_ctrl      :  4 ; /* bits 7:4 */
          ca_uint32 stage_ctrl      	 :  3 ; /* bits 10:8 */
          ca_uint32 cls_tbl_ctrl      	 :  4 ; /* bits 14:11 */
          ca_uint32 hash_tbl_ctrl      	 :  4 ; /* bits 18:15 */
          ca_uint32 lpm_tbl_ctrl      	 :  4 ; /* bits 22:19 */
          ca_uint32 nhop_tbl_ctrl      	 :  4 ; /* bits 26:23 */
          ca_uint32 reserved      	 :  2 ; /* bits 28:27 */
          ca_uint32 default_cos      	 :  3 ; /* bits 31:29 */
  } bf ;
  ca_uint32     wrd ;
} L3FE_GLB_PORT_DEFAULT_BEHAVIOR_1_VCT_t;

typedef volatile union {
  struct {
	  ca_uint32 default_ldpid        :  6 ; /* bits 5:0 */
	  ca_uint32 ip_rtu_chk_en        :  1 ; /* bits 6 */
	  ca_uint32 ip_rtu_enc           :  2 ; /* bits 8:7 */
	  ca_uint32 rate_lmt_bypass      :  1 ; /* bits 9 */
	  ca_uint32 police_en            :  1 ; /* bits 10 */
	  ca_uint32 police_id            :  9 ; /* bits 19:11 */
	  ca_uint32 spcl_pkt_tbl_en      :  1 ; /* bits 30 */
	  ca_uint32 reserved             : 11 ; /* bits 31:21 */
  } bf ;
  ca_uint32     wrd ;
} L3FE_GLB_PORT_DEFAULT_BEHAVIOR_2_VCT_t;

/* defined in G3 Header_v25.xlsm */
typedef volatile union {
  struct {
          ca_uint32 mru_redir_pol_grp_id      :  3 ; /* bits 2:0, Policer group id for mru redir traffic */
          ca_uint32 mru_redir_pol_id          :  9 ; /* bits 11:3, Choose a policer id for mru fail redir traffic */
          ca_uint32 mru_redir_pol_en          :  1 ; /* bits 12:12, Choose a policer id for mru fail redir traffic */
          ca_uint32 mru_redir_cos             :  4 ; /* bits 16:13, cos for packet failed egress length check */
          ca_uint32 mru_redir_ldpid           :  6 ; /* bits 22:17, ldpid for packet failed egress length check */
          ca_uint32 mru_redir_en              :  1 ; /* bits 23:23, IP MRU check enable */
          ca_uint32 mru_redir_len             : 14 ; /* bits 37:24, Max receive ip packet length */
          ca_uint32 t1_ctrl                   :  4 ; /* bits 41:38, 0-1: index to one of 2 classification engine profiles, 2-14: reserved, 15: bypass CLE */
          ca_uint32 pol_grp_id                :  3 ; /* bits 44:42, used to set default value of hd_i.pol_grp_id */
          ca_uint32 pol_id                    :  9 ; /* bits 53:45, used to set default value of hd_i.pol_id */
          ca_uint32 pol_en                    :  1 ; /* bits 54:54, used to set default value of hd_i.pol_bypass */
          ca_uint32 default_cos               :  4 ; /* bits 58:55, default receive cos (CPU port) for packet failed MAC_DA matching */
          ca_uint32 default_cos_sel           :  2 ; /* bits 60:59, set hdr_i.cos, refer to G3 Header file for detail values */
          ca_uint32 default_cos_table_sel     :  1 ; /* bits 61:61, when lpb_default_cos_sel[1:0]=2 or 3, then chose 1 of 2 mapping table sets */
          ca_uint32 default_ldpid             :  6 ; /* bits 67:62, default receive port (CPU port) for packet failed MAC_DA matching */
          ca_uint32 mac_da_an_sel             :  4 ; /* bits 71:68, when lpb_mac_filter_en is set to 1, only permit MC,BC and packet with this field matches psr_mac_da_an_sel[3:0] */
          ca_uint32 mac_da_match_en           :  1 ; /* bits 72:72, 0: promiscuous mode, 1: enable MAC filtering */
          ca_uint32 special_sel               :  1 ; /* bits 73:73, select one of 2 special packet resolution control table sets */
          ca_uint32 special_en                :  1 ; /* bits 74:74, 0: bypass special packet resultion, 1: use special packet resolution */
          ca_uint32 default_802_1p            :  3 ; /* bits 77:75, port default 802_1p for untagged packet */
          ca_uint32 set_olspid                :  1 ; /* bits 78:78, 0: keep olspid value unchanged, 1: set olspid field with lpb_lspid  */
          ca_uint32 lspid                     :  6 ; /* bits 84:79, the logical port corresponding to the physical port */
  } bf ;
  ca_uint32     wrd[3] ;
} L3FE_GLB_PORT_INGRESS_BEHAVIOR_VCT_t;

typedef enum {
	/* Header I between... */
	MONITOR_HDR_I_PP_STG0		= 0,
	MONITOR_HDR_I_STG0_STG1		= 1,
	MONITOR_HDR_I_STG1_STG2		= 2,
	MONITOR_CLS_RESULT		= 3,

	MONITOR_DUMP_MIN		= 0,
	MONITOR_DUMP_MAX		= 7,
} l3fe_glb_monitor_vector_e;

typedef enum {
	/* Header I between... */
	DBG_HDR_I_T1_T2			= 0,	/* T1 (CLS) ~ T2 (Hash) */
	DBG_HDR_I_T2_STG2		= 1,	/* T2 (Hash) ~ STG2 */
	DBG_HDR_I_STG2_T3		= 2,	/* STG2 ~ T3 (LPM) */
	DBG_HDR_I_T3_T4			= 3,	/* T3 (LPM) ~ T4 (HashLite) */
	DBG_HDR_I_T4_STG3_U1		= 4,	/* T4 (HashLite) ~ STG3.U1 */
	DBG_HDR_I_STG3_U1_STG3_U2	= 5,	/* STG3.U1 ~ STG3.U2 */
	DBG_HDR_I_STG3_U2_T5		= 6,	/* STG3.U2 ~ T5 (L2 Lookup) */
	DBG_HDR_I_T5_TE			= 7,	/* T5 (L2 Lookup) ~ TE Control */
	DBG_HDR_I_TE_PE			= 8,	/* TE Control ~ Packet Editor */

	/* Header A */
	DBG_HDR_A_PE			= 14,	/* Header A after Packet Editor */

	/* L3FE packet count */
	DBG_L3FE_PKT_COUNT		= 15,	/* L3FE packet count */

	DBG_DUMP_MIN			= 0,
	DBG_DUMP_MAX			= 31,
} l3fe_glb_dbg_vector_e;

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

typedef struct {
	ca_uint32_t	cos			: 3;
	ca_uint32_t	ldpid			: 6;
	ca_uint32_t	lspid			: 6;
	ca_uint32_t	pkt_size		: 14;
	ca_uint32_t	fe_bypass		: 1;
	ca_uint32_t	hdr_type		: 2;
	ca_uint32_t	mcgid			: 8;
	ca_uint32_t	drop_code		: 3;
	ca_uint32_t	rx_pkt_type		: 2;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	mirror			: 1;
	ca_uint32_t	mark			: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_id			: 9;
	ca_uint32_t	pol_grp_id		: 3;
	ca_uint32_t	deep_q			: 1;
	ca_uint32_t	cpu_flg			: 1;
} __attribute__((packed)) L3FE_HDR_A_t;

typedef struct {
	/* IP */
	ca_uint32_t	l3_chksum_err		: 1;
	ca_uint32_t	l3_chksum		: 16;
	ca_uint32_t	ipv6_doh		: 1;
	ca_uint32_t	ipv6_rh			: 1;
	ca_uint32_t	ipv6_hbh		: 1;
	ca_uint32_t	ipv6_ndp		: 1;
	ca_uint32_t	ip_fragment		: 2;
	ca_uint32_t	ip_options		: 1;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	ipv6_flow_lbl		: 20;
	ca_uint32_t	ip_da_mc		: 1;
	ca_uint32_t	ip_da_0			: 32;
	ca_uint32_t	ip_da_1			: 32;
	ca_uint32_t	ip_da_2			: 32;
	ca_uint32_t	ip_da_3			: 32;
	ca_uint32_t	ip_sa_0			: 32;
	ca_uint32_t	ip_sa_1			: 32;
	ca_uint32_t	ip_sa_2			: 32;
	ca_uint32_t	ip_sa_3			: 32;
	ca_uint32_t	ip_protocol		: 8;

	/* QoS DSCP */
	ca_uint32_t	ip_ecn			: 2;
	ca_uint32_t	ip_dscp			: 6;

	/* L3 */
	ca_uint32_t	ip_ver			: 1;
	ca_uint32_t	l3_total_len		: 14;
	ca_uint32_t	ip_ihl			: 4;
	ca_uint32_t	ip_vld			: 1;

	/* align to 32-bit */
	ca_uint32_t	padding			: 7;
} __attribute__((packed)) L3FE_HDR_INNER_IP_t;

/* L3FE 10-bit Packet Count */
typedef struct {
	ca_uint32_t	cnt_l3fe_in		: 16;	/* pkt count of entering L3FE; before T0 (PP) */
	ca_uint32_t	cnt_l3fe_out		: 16;	/* pkt count of leaving L3FE; after Packet Editor */
	ca_uint32_t	cnt_t1_t2		: 16;	/* pkt count between T1 (CLS) and T2 (Hash) */
	ca_uint32_t	cnt_stage3_pe		: 16;	/* pkt count between Stage3 and Packet Editor */
} __attribute__((packed)) L3FE_DBG_PKT_COUNT_t;

int aal_l3fe_glb_cls_stg_monitor_get(l3fe_glb_monitor_vector_e where, void *data, size_t size);
int aal_l3fe_glb_dbg_get(l3fe_glb_dbg_vector_e where, void *data, size_t size);

/***************************************************************************
 * STG0
 ***************************************************************************/

/*0 define G3 header_v32.xlsm */
typedef volatile union {
  struct {
          ca_uint32 mru_redir_pol_grp_id      :  3 ; /* bits 2:0, Policer group id for mru redir traffic */
          ca_uint32 mru_redir_pol_id          :  9 ; /* bits 11:3, Choose a policer id for mru fail redir traffic */
          ca_uint32 mru_redir_pol_en          :  2 ; /* bits 13:12, MRU redirection traffic policing enable */
          ca_uint32 mru_redir_cos             :  3 ; /* bits 16:14, cos for packet failed egress length check */
          ca_uint32 mru_redir_ldpid           :  6 ; /* bits 22:17, ldpid for packet failed egress length check. To drop the packet set this value to BLACKHOLE */
          ca_uint32 mru_redir_en              :  1 ; /* bits 23:23, IP MRU check enable */
          ca_uint32 mru_len                   :  8 ; /* bits 31:24, Max receive ip packet length */
  } bf;
  ca_uint32 wrd;
} L3FE_STG0_LPB_TBL_LOW_t;

typedef volatile union {
  struct {
          ca_uint32 mru_len                   :  6 ; /* bits 37:32, Max receive ip packet length */
          ca_uint32 t5_ctrl                   :  4 ; /* bits 41:38, 0: lookup L2 FE at T5, apply L2 lookup results, 1-14: reserved, 15: bypass T2 lookup */
          ca_uint32 t1_ctrl                   :  4 ; /* bits 45:42, 0-1: index to one of 2 classification engine profiles, 2-14: reserved, 15: bypass CLE */
          ca_uint32 pol_grp_id                :  3 ; /* bits 48:46, used to set default value of hd_i.pol_grp_id */
          ca_uint32 pol_id                    :  9 ; /* bits 57:49, used to set default value of hd_i.pol_id */
          ca_uint32 pol_en                    :  2 ; /* bits 59:58, enable flow policers */
          ca_uint32 merr_vld                  :  1 ; /* bits 60:60, Mirror valid */
          ca_uint32 merr_en                   :  1 ; /* bits 61:61, Enable Mirror */
          ca_uint32 default_cos               :  2 ; /* bits 63:62, default receive cos */
  } bf;
  ca_uint32 wrd;
} L3FE_STG0_LPB_TBL_MID_t;

typedef volatile union {
  struct {
          ca_uint32 default_cos               :  1 ; /* bits 64:64, default receive cos */
          ca_uint32 default_cos_sel           :  2 ; /* bits 66:65, copy cos for hdr_a, refer to G3 Header file for detail values */
          ca_uint32 default_cos_tbl_sel       :  1 ; /* bits 67:67, when lpb_default_cos_sel[1:0]=2 or 3, then chose 1 of 2 mapping table sets */
          ca_uint32 default_ldpid             :  6 ; /* bits 73:68, default destination port. To drop the packet set this value to BLACKHOLE. Use this value to set default hdr_i.ldpid */
          ca_uint32 mac_da_an_mask            :  8 ; /* bits 81:74, when lpb_mac_filter_en is set to 1, only permit MC,BC and packet with this field matches psr_mac_da_an_sel[3:0] */
          ca_uint32 mac_da_match_en           :  1 ; /* bits 82:82, 0: disable MAC filtering, 1: enable MAC filtering */
          ca_uint32 special_sel               :  1 ; /* bits 83:83, select one of 2 special packet resolution control table sets */
          ca_uint32 special_en                :  1 ; /* bits 84:84, 0: bypass special packet resultion, 1: use special packet resolution */
          ca_uint32 default_802_1p            :  3 ; /* bits 87:85, port default 802_1p for untagged packet, for signle tag packet it's value is assigned to hdr_i.inner_802_1p */
          ca_uint32 lspid                     :  6 ; /* bits 93:88, The logical port corresponding to the physical port */
          ca_uint32 reserved                  :  2 ; /* bits 95:84, reserved */
  } bf ;
  ca_uint32     wrd;
} L3FE_STG0_LPB_TBL_HI_t;

#define AAL_L3FE_LPB_TBL_NUM   4

/* functions proto-type */
int aal_l3fe_stg0_ctrl_set(ca_device_id_t device_id, L3FE_STG0_CTRL_t *ctrl);
int aal_l3fe_stg0_ctrl_get(ca_device_id_t device_id, L3FE_STG0_CTRL_t *ctrl);
void aal_l3fe_lpb_l2_l3_wan(ca_uint8_t wan_lspid, ca_uint8_t lan_ldpid);
void aal_l3fe_lpb_l2_l3_lan(ca_uint8_t lan_lspid, ca_uint8_t wan_ldpid);
void aal_l3fe_map_l2_l3_wan(ca_uint8_t l2fe_ldpid, ca_uint8_t l3fe_lspid);
void aal_l3fe_map_l2_l3_lan(ca_uint8_t l2fe_ldpid, ca_uint8_t l3fe_lspid);

/***************************************************************************
 * STG1
 ***************************************************************************/

typedef volatile union {
	struct {
		ca_uint32_t sixrd_ecn_keep_outer :  1 ;
		ca_uint32_t sixrd_dscp_keep_outer :  1 ;
		ca_uint32_t sixrd_ttl_keep_outer :  1 ;
		ca_uint32_t dsl_ecn_keep_outer   :  1 ;
		ca_uint32_t dsl_dscp_keep_outer  :  1 ;
		ca_uint32_t dsl_ttl_keep_outer   :  1 ;
		ca_uint32_t dsl_rpf_chk_en       :  1 ;
		ca_uint32_t dsl_ipmc_addr_consistency_chk_en :  1 ;
		ca_uint32_t dsl_ipmc_addr_prfx_chk_en :  1 ;
		ca_uint32_t dsl_ipmc_ext_det_en  :  1 ;
		ca_uint32_t dsl_ipmc_ilg_chk_en  :  1 ;
		ca_uint32_t sixrd_ipmc_ilg_chk_en :  1 ;
		ca_uint32_t rsrvd1               :  4 ;
		ca_uint32_t ecn_chk_fail_fwd_ctl :  1 ;	/* set FIB.pop_l3_chk_ecn_en to enable this check */
		ca_uint32_t intf_chk_fail_fwd_ctl :  2 ;
		ca_uint32_t sixrd_decap_chk_fail_fwd_ctl :  2 ;
		ca_uint32_t dsl_decap_chk_fail_fwd_ctl :  2 ;
		ca_uint32_t rsrvd2               :  1 ;
		ca_uint32_t sixrd_ipmc_fwd_ctl   :  1 ;
		ca_uint32_t dsl_ipmc_fwd_ctl     :  1 ;
		ca_uint32_t sixrd_ipmc_new_mac_en :  1 ;
		ca_uint32_t dsl_ipmc_new_mac_en  :  1 ;
		ca_uint32_t rsrvd3               :  4 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_ipnip_ctrl_reg_t;

typedef volatile union {
	struct {
		ca_uint32_t t2_ctrl              :  4 ;
		ca_uint32_t t3_ctrl              :  4 ;
		ca_uint32_t t4_ctrl              :  4 ;
		ca_uint32_t t5_ctrl              :  4 ;
		ca_uint32_t cos                  :  3 ;
		ca_uint32_t ldpid                :  6 ;
		ca_uint32_t keep_orig_pkt        :  1 ;
		ca_uint32_t rsrvd1               :  6 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_ecn_fail_fwd_ctrl_t;

typedef volatile union {
	struct {
		ca_uint32_t t2_ctrl              :  4 ;
		ca_uint32_t t3_ctrl              :  4 ;
		ca_uint32_t t4_ctrl              :  4 ;
		ca_uint32_t t5_ctrl              :  4 ;
		ca_uint32_t cos                  :  3 ;
		ca_uint32_t ldpid                :  6 ;
		ca_uint32_t keep_orig_pkt        :  1 ;
		ca_uint32_t rsrvd1               :  6 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_ipnip_fail_fwd_ctrl_t;

typedef volatile union {
	struct {
		ca_uint32_t t2_ctrl              :  4 ;
		ca_uint32_t t3_ctrl              :  4 ;
		ca_uint32_t t4_ctrl              :  4 ;
		ca_uint32_t t5_ctrl              :  4 ;
		ca_uint32_t cos                  :  3 ;
		ca_uint32_t ldpid                :  6 ;
		ca_uint32_t keep_orig_pkt        :  1 ;
		ca_uint32_t rsrvd1               :  6 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_sixrd_ipmc_fwd_ctrl_t;

typedef volatile union {
	struct {
		ca_uint32_t t2_ctrl              :  4 ;
		ca_uint32_t t3_ctrl              :  4 ;
		ca_uint32_t t4_ctrl              :  4 ;
		ca_uint32_t t5_ctrl              :  4 ;
		ca_uint32_t cos                  :  3 ;
		ca_uint32_t ldpid                :  6 ;
		ca_uint32_t keep_orig_pkt        :  1 ;
		ca_uint32_t rsrvd1               :  6 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_dsl_ipmc_fwd_ctrl_t;

typedef volatile union {
	struct {
		ca_uint32_t addr_prfx            : 16 ;
		ca_uint32_t rsrvd1               : 16 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_sixrd_ipmc_new_mac_da_t;

typedef volatile union {
	struct {
		ca_uint32_t addr_prfx            : 25 ;
		ca_uint32_t rsrvd1               :  7 ;
	} bf ;
	ca_uint32_t     wrd ;
} l3fe_stg1_dsl_ipmc_new_mac_da_t;

int aal_l3fe_stg1_ipnip_ctrl_set(l3fe_stg1_ipnip_ctrl_reg_t *data);
int aal_l3fe_stg1_ipnip_ctrl_get(l3fe_stg1_ipnip_ctrl_reg_t *data);

int aal_l3fe_stg1_ecn_fail_fwd_ctrl_set(l3fe_stg1_ecn_fail_fwd_ctrl_t *data);
int aal_l3fe_stg1_ecn_fail_fwd_ctrl_get(l3fe_stg1_ecn_fail_fwd_ctrl_t *data);

int aal_l3fe_stg1_ipnip_fail_fwd_ctrl_set(l3fe_stg1_ipnip_fail_fwd_ctrl_t *data);
int aal_l3fe_stg1_ipnip_fail_fwd_ctrl_get(l3fe_stg1_ipnip_fail_fwd_ctrl_t *data);

int aal_l3fe_stg1_sixrd_ipmc_fwd_ctrl_set(l3fe_stg1_sixrd_ipmc_fwd_ctrl_t *data);
int aal_l3fe_stg1_sixrd_ipmc_fwd_ctrl_get(l3fe_stg1_sixrd_ipmc_fwd_ctrl_t *data);

int aal_l3fe_stg1_dsl_ipmc_fwd_ctrl_set(l3fe_stg1_dsl_ipmc_fwd_ctrl_t *data);
int aal_l3fe_stg1_dsl_ipmc_fwd_ctrl_get(l3fe_stg1_dsl_ipmc_fwd_ctrl_t *data);

int aal_l3fe_stg1_sixrd_ipmc_new_mac_da_set(l3fe_stg1_sixrd_ipmc_new_mac_da_t *data);
int aal_l3fe_stg1_sixrd_ipmc_new_mac_da_get(l3fe_stg1_sixrd_ipmc_new_mac_da_t *data);

int aal_l3fe_stg1_dsl_ipmc_new_mac_da_set(l3fe_stg1_dsl_ipmc_new_mac_da_t *data);
int aal_l3fe_stg1_dsl_ipmc_new_mac_da_get(l3fe_stg1_dsl_ipmc_new_mac_da_t *data);

/***************************************************************************
 * PP (Packet Parser)
 ***************************************************************************/

#define L3FE_CAM_ACCESS_TIMEOUT		1000000

/* CAM indication */
typedef enum {
        /* 0 */
        L3FE_DPORT_CAM,
        L3FE_SPORT_CAM,
        L3FE_ETHERTYPE_CAM,
        L3FE_MAC_DA_CAM,
        L3FE_HTTP_MSGS_CAM,
        L3FE_LLC_CAM,
        L3FE_PKT_LEN_RNG_CAM,
	L3FE_PPP_CODE_CAM,
	L3FE_PPP_PROT_CAM,
	L3FE_SPCL_HDR_CAM,
} l3fe_cam_indication_e;

/* CAM indication address */
#define L3FE_CAM_IND_MAX_DPORT	15
#define L3FE_CAM_IND_MAX_SPORT	15
#define L3FE_CAM_IND_MAX_ETHERTYPE	62
#define L3FE_CAM_IND_MAX_MAC_DA	14
#define L3FE_CAM_IND_MAX_HTTP_MSGS	15
#define L3FE_CAM_IND_MAX_LLC		2
#define L3FE_CAM_IND_MAX_PKT_LEN_RNG	3
#define L3FE_CAM_IND_MAX_PPP_CODE	14
#define L3FE_CAM_IND_MAX_PPP_PROT	14
#define L3FE_CAM_IND_MAX_SPCL_HDR	7

typedef volatile union {
  struct {
	  ca_uint32 address		:  8 ; /* bits 7:0 */
	  ca_uint32 reserved		:  8 ; /* bits 15:8 */
	  ca_uint32 cam_indication	:  4 ; /* bits 19:16 */
  } bf ;
  ca_uint32     wrd ;
} L3FE_PP_FIELD_CAM_ACCESS_ADDR_t;

ca_status_t aal_l3fe_lpb_idx_tbl_get(CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t  lspid,
    CA_OUT ca_uint8_t *lpb_profile);

ca_status_t aal_l3fe_lpb_idx_tbl_set(CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t lspid,
    CA_IN ca_uint8_t  lpb_profile);

void aal_l3fe_l4_chksum(ca_uint8_t enable);

/***************************************************************************
 * Initialization
 ***************************************************************************/

int aal_l3fe_stg0_set_normal(void);
int aal_l3fe_stg0_set_bypass_to_cpu(void);

int aal_l3fe_init(void);

extern ca_uint8_t l3fe_classifier_profile_wan;
extern ca_uint8_t l3fe_classifier_profile_lan;
extern ca_uint8_t l3fe_main_hash_profile_wan;
extern ca_uint8_t l3fe_main_hash_profile_lan;
extern ca_uint8_t l3fe_main_hash_profile_mc;
extern ca_uint8_t l3fe_main_hash_profile_class;
extern ca_uint8_t l3fe_hash_lite_profile;
extern ca_uint8_t l3fe_nat_enable;
extern ca_uint8_t l3fe_t5_ctrl_support;

#endif /* __AAL_L3FE_H__ */

