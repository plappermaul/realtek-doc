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
 * aal_l3_classidier.h: Hardware Abstraction Layer for L3 classifier to access hardware regsiters
 *
 */
#ifndef __AAL_CLS_H__
#define __AAL_CLS_H__

#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#define L3_CLS_PROFILE_MAX 2
#define L3_CLS_FIB_TBL_DEF_ENTRY_MAX 8
#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define L3_CLS_KEY_TBL_ENTRY_MAX	64
#define L3_CLS_FIB_TBL_ENTRY_MAX	264
#else
#define L3_CLS_KEY_TBL_ENTRY_MAX	128
#define L3_CLS_FIB_TBL_ENTRY_MAX	520
#endif
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
#define HGU_L3_CLS_KEY_TBL_ENTRY_MAX	64
#define HGU_L3_CLS_FIB_TBL_ENTRY_MAX		264
#endif
#define L3_CLS_STD_FIB_ENTRY_NUM	2
#define L3_CLS_LONG_KEY_ENTRY_NUM	2
#define CL_4_KEY_NUM 4
#define CL_2_KEY_NUM 2
#define /*CODEGEN_IGNORE_TAG*/ CLS_RUL_ID(devid, ktyp, fmod, tidx, kidx) ((ktyp << 28) | (fmod << 24) | (devid << 20) | (tidx << 8) | kidx)
#define /*CODEGEN_IGNORE_TAG*/ CLS_TBL_ID(ridx) ((ridx >> 8) & 0x7f)
#define /*CODEGEN_IGNORE_TAG*/ CLS_KEY_ID(ridx) (ridx & 0xff)
#define /*CODEGEN_IGNORE_TAG*/ CLS_KEY_TYPE(ridx) ((ridx >> 28) & 0xf)
#define /*CODEGEN_IGNORE_TAG*/ CLS_FIB_MODE(ridx) ((ridx >> 24) & 0xf)
#define /*CODEGEN_IGNORE_TAG*/ CLS_DEV_ID(ridx) ((ridx >> 20) & 0xf)


typedef enum {
	CL_RUL_PRIO_MIN = 0,
	CL_RUL_PRIO_L3_INTF_BCAST = 6,
	CL_RUL_PRIO_L3_INTF_P2P = CL_RUL_PRIO_L3_INTF_BCAST,
	CL_RUL_PRIO_L3_INTF_CPU = 7,
	CL_RUL_PRIO_L3_INTF_LB = CL_RUL_PRIO_L3_INTF_CPU,
	CL_RUL_PRIO_L3_INTF_TUNNEL_PPPOE = 8,
	CL_RUL_PRIO_L3_INTF_MCAST = 9,
	CL_RUL_PRIO_L3_INTF_TUNNEL = 10,
	CL_RUL_PRIO_L3_SCPL_PKT = 11,
	CL_RUL_PRIO_MAX = 15,
} cls_rule_prio_t;

typedef enum {
	CL_RSLT_TYPE_0 = 0,
	CL_RSLT_TYPE_1 = 1,
} cls_rslt_type_t;

typedef enum {
	CL_IF_ID_KEY = 0,
	CL_IPV4_TUNNEL_ID_KEY,
	CL_IPV4_SHORT_KEY,
	CL_IPV6_SHORT_KEY,
	CL_SPCL_PKT_KEY,
	CL_MCST_KEY,
	CL_FULL_KEY
} l3_cls_key_type_t;

typedef enum {
	CL_FIB_MOD_0 = 0,
	CL_FIB_MOD_1,
	CL_FIB_MOD_2,
	CL_FIB_MOD_3,
	CL_FIB_MOD_4
} l3_cls_fib_mode_t;


/*		CLS	KEY       */
typedef struct cls_ip_addr_s {
	ca_uint32_t ip_addr_0;
	ca_uint32_t ip_addr_1;
	ca_uint32_t ip_addr_2;
	ca_uint32_t ip_addr_3;
} __attribute__((packed)) cls_ip_addr_t;

typedef struct cls_mdata_s {
	ca_uint32_t mdata_0;
	ca_uint32_t mdata_1;
} __attribute__((packed)) cls_mdata_t;

/* CL_IF_ID_KEY 83 Bits */
typedef struct cl_if_id_key_s {
	ca_uint32_t msk_ip_fragment_0 :1;
	ca_uint32_t ip_fragment_0 :1;

	ca_uint32_t msk_ip_da_sa_equal_0 :1;
	ca_uint32_t ip_da_sa_equal_0 :1;
	ca_uint32_t msk_ip_da_mc_0 :1;
	ca_uint32_t ip_da_mc_0 :1;

	ca_uint32_t msk_ip_ver_0 :1;
	ca_uint32_t ip_ver_0 :1;
	ca_uint32_t msk_ip_vld_0 :1;
	ca_uint32_t ip_vld_0 :1;

	ca_uint32_t msk_pppoe_session_0 :1;
	ca_uint32_t pppoe_session_id_0 :16;

	ca_uint32_t msk_top_vid_ptag_0 :1;
	ca_uint32_t top_vid_ptag_0 :1;

	ca_uint32_t msk_top_vid_0 :1;
	ca_uint32_t top_vid_0 :12;
	ca_uint32_t msk_top_tpid_enc_0 :1;
	ca_uint32_t top_tpid_enc_0 :3;
	ca_uint32_t msk_vlan_cnt_0 :1;
	ca_uint32_t vlan_cnt_0 :2;

	ca_uint32_t msk_ethertype_enc_0 :1;
	ca_uint32_t ethertype_enc_0 :6;

	ca_uint32_t msk_mac_da_ip_mc_0 :1;
	ca_uint32_t mac_da_ip_mc_0 :1;
	ca_uint32_t msk_mac_da_mc_0 :1;
	ca_uint32_t mac_da_mc_0 :1;
	ca_uint32_t msk_mac_da_bc_0 :1;
	ca_uint32_t mac_da_bc_0 :1;
	ca_uint32_t msk_mac_da_an_hit_0 :1;
	ca_uint32_t mac_da_an_hit_0 :1;
	ca_uint32_t msk_mac_da_an_sel_0 :1;
	ca_uint32_t mac_da_an_sel_0 :4;

	ca_uint32_t msk_o_lspid_0 :1;
	ca_uint32_t o_lspid_0 :6;
	ca_uint32_t msk_lspid_0 :1;
	ca_uint32_t lspid_0 :6;

	ca_uint32_t msk_ip_fragment_1 :1;
	ca_uint32_t ip_fragment_1 :1;

	ca_uint32_t msk_ip_da_sa_equal_1 :1;
	ca_uint32_t ip_da_sa_equal_1 :1;
	ca_uint32_t msk_ip_da_mc_1 :1;
	ca_uint32_t ip_da_mc_1 :1;

	ca_uint32_t msk_ip_ver_1 :1;
	ca_uint32_t ip_ver_1 :1;
	ca_uint32_t msk_ip_vld_1 :1;
	ca_uint32_t ip_vld_1 :1;

	ca_uint32_t msk_pppoe_session_1 :1;
	ca_uint32_t pppoe_session_id_1 :16;

	ca_uint32_t msk_top_vid_ptag_1 :1;
	ca_uint32_t top_vid_ptag_1 :1;

	ca_uint32_t msk_top_vid_1 :1;
	ca_uint32_t top_vid_1 :12;
	ca_uint32_t msk_top_tpid_enc_1 :1;
	ca_uint32_t top_tpid_enc_1 :3;
	ca_uint32_t msk_vlan_cnt_1 :1;
	ca_uint32_t vlan_cnt_1 :2;

	ca_uint32_t msk_ethertype_enc_1 :1;
	ca_uint32_t ethertype_enc_1 :6;

	ca_uint32_t msk_mac_da_ip_mc_1 :1;
	ca_uint32_t mac_da_ip_mc_1 :1;
	ca_uint32_t msk_mac_da_mc_1 :1;
	ca_uint32_t mac_da_mc_1 :1;
	ca_uint32_t msk_mac_da_bc_1 :1;
	ca_uint32_t mac_da_bc_1 :1;
	ca_uint32_t msk_mac_da_an_hit_1 :1;
	ca_uint32_t mac_da_an_hit_1 :1;
	ca_uint32_t msk_mac_da_an_sel_1 :1;
	ca_uint32_t mac_da_an_sel_1 :4;

	ca_uint32_t msk_o_lspid_1 :1;
	ca_uint32_t o_lspid_1 :6;
	ca_uint32_t msk_lspid_1 :1;
	ca_uint32_t lspid_1 :6;

	ca_uint32_t msk_ip_fragment_2 :1;
	ca_uint32_t ip_fragment_2 :1;

	ca_uint32_t msk_ip_da_sa_equal_2 :1;
	ca_uint32_t ip_da_sa_equal_2 :1;
	ca_uint32_t msk_ip_da_mc_2 :1;
	ca_uint32_t ip_da_mc_2 :1;

	ca_uint32_t msk_ip_ver_2 :1;
	ca_uint32_t ip_ver_2 :1;
	ca_uint32_t msk_ip_vld_2 :1;
	ca_uint32_t ip_vld_2 :1;

	ca_uint32_t msk_pppoe_session_2 :1;
	ca_uint32_t pppoe_session_id_2 :16;

	ca_uint32_t msk_top_vid_ptag_2 :1;
	ca_uint32_t top_vid_ptag_2 :1;

	ca_uint32_t msk_top_vid_2 :1;
	ca_uint32_t top_vid_2 :12;
	ca_uint32_t msk_top_tpid_enc_2 :1;
	ca_uint32_t top_tpid_enc_2 :3;
	ca_uint32_t msk_vlan_cnt_2 :1;
	ca_uint32_t vlan_cnt_2 :2;

	ca_uint32_t msk_ethertype_enc_2 :1;
	ca_uint32_t ethertype_enc_2 :6;

	ca_uint32_t msk_mac_da_ip_mc_2 :1;
	ca_uint32_t mac_da_ip_mc_2 :1;
	ca_uint32_t msk_mac_da_mc_2 :1;
	ca_uint32_t mac_da_mc_2 :1;
	ca_uint32_t msk_mac_da_bc_2 :1;
	ca_uint32_t mac_da_bc_2 :1;
	ca_uint32_t msk_mac_da_an_hit_2 :1;
	ca_uint32_t mac_da_an_hit_2 :1;
	ca_uint32_t msk_mac_da_an_sel_2 :1;
	ca_uint32_t mac_da_an_sel_2 :4;

	ca_uint32_t msk_o_lspid_2 :1;
	ca_uint32_t o_lspid_2 :6;
	ca_uint32_t msk_lspid_2 :1;
	ca_uint32_t lspid_2 :6;

	ca_uint32_t msk_ip_fragment_3 :1;
	ca_uint32_t ip_fragment_3 :1;

	ca_uint32_t msk_ip_da_sa_equal_3 :1;
	ca_uint32_t ip_da_sa_equal_3 :1;
	ca_uint32_t msk_ip_da_mc_3 :1;
	ca_uint32_t ip_da_mc_3 :1;

	ca_uint32_t msk_ip_ver_3 :1;
	ca_uint32_t ip_ver_3 :1;
	ca_uint32_t msk_ip_vld_3 :1;
	ca_uint32_t ip_vld_3 :1;

	ca_uint32_t msk_pppoe_session_3 :1;
	ca_uint32_t pppoe_session_id_3 :16;

	ca_uint32_t msk_top_vid_ptag_3 :1;
	ca_uint32_t top_vid_ptag_3 :1;

	ca_uint32_t msk_top_vid_3 :1;
	ca_uint32_t top_vid_3 :12;
	ca_uint32_t msk_top_tpid_enc_3 :1;
	ca_uint32_t top_tpid_enc_3 :3;
	ca_uint32_t msk_vlan_cnt_3 :1;
	ca_uint32_t vlan_cnt_3 :2;

	ca_uint32_t msk_ethertype_enc_3 :1;
	ca_uint32_t ethertype_enc_3 :6;

	ca_uint32_t msk_mac_da_ip_mc_3 :1;
	ca_uint32_t mac_da_ip_mc_3 :1;
	ca_uint32_t msk_mac_da_mc_3 :1;
	ca_uint32_t mac_da_mc_3 :1;
	ca_uint32_t msk_mac_da_bc_3 :1;
	ca_uint32_t mac_da_bc_3 :1;
	ca_uint32_t msk_mac_da_an_hit_3 :1;
	ca_uint32_t mac_da_an_hit_3 :1;
	ca_uint32_t msk_mac_da_an_sel_3 :1;
	ca_uint32_t mac_da_an_sel_3 :4;

	ca_uint32_t msk_o_lspid_3 :1;
	ca_uint32_t o_lspid_3 :6;
	ca_uint32_t msk_lspid_3 :1;
	ca_uint32_t lspid_3 :6;

	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv :7;
} __attribute__((packed)) cl_if_id_key_t;

/* CL_IPV4_TUNNEL_ID_KEY 235 Bits */
typedef struct cl_ipv4_tunnel_id_key_s {
	ca_uint32_t msk_l4_port :3;
	ca_uint32_t l4_dp :16;
	ca_uint32_t l4_sp :16;
	ca_uint32_t msk_l4_vld :1;
	ca_uint32_t l4_vld :1;

	ca_uint32_t msk_spi :1;

	ca_uint32_t msk_ip_fragment :1;
	ca_uint32_t ip_fragment :1;

	ca_uint32_t msk_ip_da_sa_equal :1;
	ca_uint32_t ip_da_sa_equal :1;
	ca_uint32_t msk_ip_da_mc :1;
	ca_uint32_t ip_da_mc :1;
	ca_uint32_t msk_ip_da_enc :1;
	ca_uint32_t ip_da_enc :4;
	ca_uint32_t msk_ip_da_l :7;
	ca_uint32_t ip_da :32;

	ca_uint32_t msk_ip_sa_enc :1;
	ca_uint32_t ip_sa_enc :4;
	ca_uint32_t msk_ip_sa_l :7;
	ca_uint32_t ip_sa :32;
	ca_uint32_t msk_ip_protocol :1;
	ca_uint32_t ip_protocol :8;

	ca_uint32_t msk_ip_ver :1;
	ca_uint32_t ip_ver :1;
	ca_uint32_t msk_ip_vld :1;
	ca_uint32_t ip_vld :1;

	ca_uint32_t msk_pppoe_session :1;
	ca_uint32_t pppoe_session_id :16;

	ca_uint32_t msk_inner_vid :1;
	ca_uint32_t inner_vid :12;
	ca_uint32_t msk_inner_tpid_enc :1;
	ca_uint32_t inner_tpid_enc :3;

	ca_uint32_t msk_top_vid_ptag :1;
	ca_uint32_t top_vid_ptag :1;

	ca_uint32_t msk_top_vid :1;
	ca_uint32_t top_vid :12;
	ca_uint32_t msk_top_tpid_enc :1;
	ca_uint32_t top_tpid_enc :3;
	ca_uint32_t msk_vlan_cnt :1;
	ca_uint32_t vlan_cnt :2;

	ca_uint32_t msk_ethertype_enc :1;
	ca_uint32_t ethertype_enc :6;

	ca_uint32_t msk_mac_da_ip_mc :1;
	ca_uint32_t mac_da_ip_mc :1;
	ca_uint32_t msk_mac_da_mc :1;
	ca_uint32_t mac_da_mc :1;
	ca_uint32_t msk_mac_da_bc :1;
	ca_uint32_t mac_da_bc :1;
	ca_uint32_t msk_mac_da_an_hit :1;
	ca_uint32_t mac_da_an_hit :1;
	ca_uint32_t msk_mac_da_an_sel :1;
	ca_uint32_t mac_da_an_sel :4;

	ca_uint32_t msk_o_lspid :1;
	ca_uint32_t o_lspid :6;
	ca_uint32_t msk_lspid :1;
	ca_uint32_t lspid :6;
	ca_uint32_t	rsv1 :21;
	ca_uint32_t	rsv2 :32;
	ca_uint32_t	rsv3 :32;

	ca_uint32_t	rsv4 :12;
	ca_uint32_t	cls_entry_mode :1;
	ca_uint32_t	cls_pri	:4;
	ca_uint32_t	cls_rslt_type :1;
	ca_uint32_t	cls_key_type :3;
	ca_uint32_t	cls_key_valid :4;
	ca_uint32_t	rsv5 :7;
} __attribute__((packed)) cl_ipv4_tunnel_id_key_t;

/* CL_IPV4_SHORT_KEY 147 Bits */
typedef struct cl_ipv4_short_key_s {
	ca_uint32_t msk_l4_port_0 :3;
	ca_uint32_t l4_dp_0 :16;
	ca_uint32_t l4_sp_0 :16;
	ca_uint32_t msk_l4_vld_0 :1;
	ca_uint32_t l4_vld_0 :1;
	ca_uint32_t msk_l3_chksum_err_0 :1;
	ca_uint32_t l3_chksum_err_0 :1;
	ca_uint32_t msk_spi_0 :1;

	ca_uint32_t msk_ip_fragment_0 :1;
	ca_uint32_t ip_fragment_0 :1;

	ca_uint32_t ip_addr_type_0 :1;
	ca_uint32_t msk_ip_addr_l_0 :7;
	ca_uint32_t ip_addr_0 :32;

	ca_uint32_t msk_ip_da_sa_equal_0 :1;
	ca_uint32_t ip_da_sa_equal_0 :1;
	ca_uint32_t msk_ip_da_mc_0 :1;
	ca_uint32_t ip_da_mc_0 :1;

	ca_uint32_t msk_ip_protocol_0 :1;
	ca_uint32_t ip_protocol_0 :8;
	ca_uint32_t msk_ip_ecn_0 :1;
	ca_uint32_t ip_ecn_0 :2;
	ca_uint32_t msk_dscp_0 :1;
	ca_uint32_t ip_dscp_0 :6;

	ca_uint32_t msk_top_dei_0 :1;
	ca_uint32_t top_dei_0 :1;
	ca_uint32_t msk_top_vl_802_1p_0 :1;
	ca_uint32_t top_802_1p_0 :3;
	ca_uint32_t msk_top_vid_ptag_0 :1;
	ca_uint32_t top_vid_ptag_0 :1;

	ca_uint32_t msk_top_vid_0 :1;
	ca_uint32_t top_vid_0 :12;
	ca_uint32_t msk_top_tpid_enc_0 :1;
	ca_uint32_t top_tpid_enc_0 :3;
	ca_uint32_t msk_vlan_cnt_0 :1;
	ca_uint32_t vlan_cnt_0 :2;

	ca_uint32_t msk_o_lspid_0 :1;
	ca_uint32_t o_lspid_0 :6;
	ca_uint32_t msk_lspid_0 :1;
	ca_uint32_t lspid_0 :6;

	ca_uint32_t msk_l4_port_1 :3;
	ca_uint32_t l4_dp_1 :16;
	ca_uint32_t l4_sp_1 :16;
	ca_uint32_t msk_l4_vld_1 :1;
	ca_uint32_t l4_vld_1 :1;
	ca_uint32_t msk_l3_chksum_err_1 :1;
	ca_uint32_t l3_chksum_err_1 :1;
	ca_uint32_t msk_spi_1 :1;

	ca_uint32_t msk_ip_fragment_1 :1;
	ca_uint32_t ip_fragment_1 :1;

	ca_uint32_t ip_addr_type_1 :1;
	ca_uint32_t msk_ip_addr_l_1 :7;
	ca_uint32_t ip_addr_1 :32;

	ca_uint32_t msk_ip_da_sa_equal_1 :1;
	ca_uint32_t ip_da_sa_equal_1 :1;
	ca_uint32_t msk_ip_da_mc_1 :1;
	ca_uint32_t ip_da_mc_1 :1;

	ca_uint32_t msk_ip_protocol_1 :1;
	ca_uint32_t ip_protocol_1 :8;
	ca_uint32_t msk_ip_ecn_1 :1;
	ca_uint32_t ip_ecn_1 :2;
	ca_uint32_t msk_dscp_1 :1;
	ca_uint32_t ip_dscp_1 :6;

	ca_uint32_t msk_top_dei_1 :1;
	ca_uint32_t top_dei_1 :1;
	ca_uint32_t msk_top_vl_802_1p_1 :1;
	ca_uint32_t top_802_1p_1 :3;
	ca_uint32_t msk_top_vid_ptag_1 :1;
	ca_uint32_t top_vid_ptag_1 :1;

	ca_uint32_t msk_top_vid_1 :1;
	ca_uint32_t top_vid_1 :12;
	ca_uint32_t msk_top_tpid_enc_1 :1;
	ca_uint32_t top_tpid_enc_1 :3;
	ca_uint32_t msk_vlan_cnt_1 :1;
	ca_uint32_t vlan_cnt_1 :2;

	ca_uint32_t msk_o_lspid_1 :1;
	ca_uint32_t o_lspid_1 :6;
	ca_uint32_t msk_lspid_1 :1;
	ca_uint32_t lspid_1 :6;

	ca_uint32_t rsv1 :32;
	ca_uint32_t rsv2 :6;
	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv3 :7;
} __attribute__((packed)) cl_ipv4_short_key_t;

/* CL_IPV6_SHORT_KEY 264 Bits */
typedef struct cl_ipv6_short_key_s {
	ca_uint32_t msk_l4_port :3;
	ca_uint32_t l4_dp :16;
	ca_uint32_t l4_sp :16;
	ca_uint32_t msk_l4_vld :1;
	ca_uint32_t l4_vld :1;

	ca_uint32_t msk_spi :1;

	ca_uint32_t msk_ip_fragment :1;
	ca_uint32_t ip_fragment :1;

	ca_uint32_t msk_ipv6_flow_lbl :1;
	ca_uint32_t ipv6_flow_lbl :20;
	ca_uint32_t ip_addr_type :1;
	ca_uint32_t msk_ip_addr_l :7;
	ca_uint32_t msk_ip_addr_h :2;
	ca_uint32_t ip_addr_0 :32;
	ca_uint32_t ip_addr_1 :32;
	ca_uint32_t ip_addr_2 :32;
	ca_uint32_t ip_addr_3 :32;

	ca_uint32_t msk_ip_da_sa_equal :1;
	ca_uint32_t ip_da_sa_equal :1;
	ca_uint32_t msk_ip_da_mc :1;
	ca_uint32_t ip_da_mc :1;

	ca_uint32_t msk_ip_protocol :1;
	ca_uint32_t ip_protocol :8;
	ca_uint32_t msk_ip_ecn :1;
	ca_uint32_t ip_ecn :2;
	ca_uint32_t msk_dscp :1;
	ca_uint32_t ip_dscp :6;

	ca_uint32_t msk_top_dei :1;
	ca_uint32_t top_dei :1;
	ca_uint32_t msk_top_vl_802_1p :1;
	ca_uint32_t top_802_1p :3;
	ca_uint32_t msk_top_vid_ptag :1;
	ca_uint32_t top_vid_ptag :1;

	ca_uint32_t msk_top_vid :1;
	ca_uint32_t top_vid :12;
	ca_uint32_t msk_top_tpid_enc :1;
	ca_uint32_t top_tpid_enc :3;
	ca_uint32_t msk_vlan_cnt :1;
	ca_uint32_t vlan_cnt :2;

	ca_uint32_t msk_o_lspid :1;
	ca_uint32_t o_lspid :6;
	ca_uint32_t msk_lspid :1;
	ca_uint32_t lspid :6;

	ca_uint32_t rsv1 :32;
	ca_uint32_t rsv2 :32;
	ca_uint32_t rsv3 :4;
	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv4 :7;
} __attribute__((packed)) cl_ipv6_short_key_t;

/* CL_SPCL_PKT_KEY 163 Bits */
typedef struct cl_spcl_pkt_key_s {
	ca_uint32_t spcl_pkt_hdr_mtch_msk_0 :1;
	ca_uint32_t spcl_pkt_hdr_mtch_0 :8;
	ca_uint32_t spcl_pkt_enc_msk_0 :1;
	ca_uint32_t spcl_pkt_enc_0 :6;

	ca_uint32_t msk_icmp_type_0 :1;
	ca_uint32_t icmp_type_0 :8;
	ca_uint32_t msk_icmp_vld_0 :1;
	ca_uint32_t icmp_vld_0 :3;
	ca_uint32_t msk_ipv6_doh_0 :1;
	ca_uint32_t ipv6_doh_0 :1;
	ca_uint32_t msk_ipv6_rh_0 :1;
	ca_uint32_t ipv6_rh_0 :1;
	ca_uint32_t msk_ipv6_hbh_0 :1;
	ca_uint32_t ipv6_hbh_0 :1;
	ca_uint32_t msk_ipv6_ndp_0 :1;
	ca_uint32_t ipv6_ndp_0 :1;
	ca_uint32_t msk_ip_fragment_0 :1;
	ca_uint32_t ip_fragment_0 :1;
	ca_uint32_t msk_ip_options_0 :1;
	ca_uint32_t ip_options_0 :1;

	ca_uint32_t msk_ip_protocol_0 :1;
	ca_uint32_t ip_protocol_0 :8;

	ca_uint32_t msk_top_dei_0 :1;
	ca_uint32_t top_dei_0 :1;
	ca_uint32_t msk_top_vl_802_1p_0 :1;
	ca_uint32_t top_802_1p_0 :3;
	ca_uint32_t msk_top_vid_ptag_0 :1;
	ca_uint32_t top_vid_ptag_0 :1;

	ca_uint32_t msk_top_vid_0 :1;
	ca_uint32_t top_vid_0 :12;
	ca_uint32_t msk_top_tpid_enc_0 :1;
	ca_uint32_t top_tpid_enc_0 :3;
	ca_uint32_t msk_vlan_cnt_0 :1;
	ca_uint32_t vlan_cnt_0 :2;

	ca_uint32_t msk_ethertype_enc_0 :1;
	ca_uint32_t ethertype_enc_0 :6;

	ca_uint32_t msk_mac_da_rsvd_0 :1;
	ca_uint32_t mac_da_rsvd_0 :1;
	ca_uint32_t msk_mac_da_ip_mc_0 :1;
	ca_uint32_t mac_da_ip_mc_0 :1;
	ca_uint32_t msk_mac_da_mc_0 :1;
	ca_uint32_t mac_da_mc_0 :1;
	ca_uint32_t msk_mac_da_bc_0 :1;
	ca_uint32_t mac_da_bc_0 :1;
	ca_uint32_t msk_mac_da_an_hit_0 :1;
	ca_uint32_t mac_da_an_hit_0 :1;
	ca_uint32_t msk_mac_da_an_sel_0 :1;
	ca_uint32_t mac_da_an_sel_0 :4;
	ca_uint32_t msk_mac_da_0 :1;
	ca_uint32_t mac_da0_0:8;
	ca_uint32_t mac_da1_0:8;
	ca_uint32_t mac_da2_0:8;
	ca_uint32_t mac_da3_0:8;
	ca_uint32_t mac_da4_0:8;
	ca_uint32_t mac_da5_0:8;

	ca_uint32_t msk_o_lspid_0 :1;
	ca_uint32_t o_lspid_0 :6;
	ca_uint32_t msk_lspid_0 :1;
	ca_uint32_t lspid_0 :6;

	ca_uint32_t spcl_pkt_hdr_mtch_msk_1 :1;
	ca_uint32_t spcl_pkt_hdr_mtch_1 :8;
	ca_uint32_t spcl_pkt_enc_msk_1 :1;
	ca_uint32_t spcl_pkt_enc_1 :6;

	ca_uint32_t msk_icmp_type_1 :1;
	ca_uint32_t icmp_type_1 :8;
	ca_uint32_t msk_icmp_vld_1 :1;
	ca_uint32_t icmp_vld_1 :3;
	ca_uint32_t msk_ipv6_doh_1 :1;
	ca_uint32_t ipv6_doh_1 :1;
	ca_uint32_t msk_ipv6_rh_1 :1;
	ca_uint32_t ipv6_rh_1 :1;
	ca_uint32_t msk_ipv6_hbh_1 :1;
	ca_uint32_t ipv6_hbh_1 :1;
	ca_uint32_t msk_ipv6_ndp_1 :1;
	ca_uint32_t ipv6_ndp_1 :1;
	ca_uint32_t msk_ip_fragment_1 :1;
	ca_uint32_t ip_fragment_1 :1;
	ca_uint32_t msk_ip_options_1 :1;
	ca_uint32_t ip_options_1 :1;

	ca_uint32_t msk_ip_protocol_1 :1;
	ca_uint32_t ip_protocol_1 :8;

	ca_uint32_t msk_top_dei_1 :1;
	ca_uint32_t top_dei_1 :1;
	ca_uint32_t msk_top_vl_802_1p_1 :1;
	ca_uint32_t top_802_1p_1 :3;
	ca_uint32_t msk_top_vid_ptag_1 :1;
	ca_uint32_t top_vid_ptag_1 :1;

	ca_uint32_t msk_top_vid_1 :1;
	ca_uint32_t top_vid_1 :12;
	ca_uint32_t msk_top_tpid_enc_1 :1;
	ca_uint32_t top_tpid_enc_1 :3;
	ca_uint32_t msk_vlan_cnt_1 :1;
	ca_uint32_t vlan_cnt_1 :2;

	ca_uint32_t msk_ethertype_enc_1 :1;
	ca_uint32_t ethertype_enc_1 :6;

	ca_uint32_t msk_mac_da_rsvd_1 :1;
	ca_uint32_t mac_da_rsvd_1 :1;
	ca_uint32_t msk_mac_da_ip_mc_1 :1;
	ca_uint32_t mac_da_ip_mc_1 :1;
	ca_uint32_t msk_mac_da_mc_1 :1;
	ca_uint32_t mac_da_mc_1 :1;
	ca_uint32_t msk_mac_da_bc_1 :1;
	ca_uint32_t mac_da_bc_1 :1;
	ca_uint32_t msk_mac_da_an_hit_1 :1;
	ca_uint32_t mac_da_an_hit_1 :1;
	ca_uint32_t msk_mac_da_an_sel_1 :1;
	ca_uint32_t mac_da_an_sel_1 :4;
	ca_uint32_t msk_mac_da_1 :1;
	ca_uint32_t mac_da0_1:8;
	ca_uint32_t mac_da1_1:8;
	ca_uint32_t mac_da2_1:8;
	ca_uint32_t mac_da3_1:8;
	ca_uint32_t mac_da4_1:8;
	ca_uint32_t mac_da5_1:8;

	ca_uint32_t msk_o_lspid_1 :1;
	ca_uint32_t o_lspid_1 :6;
	ca_uint32_t msk_lspid_1 :1;
	ca_uint32_t lspid_1 :6;

	ca_uint32_t rsv1 :6;
	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv2 :7;
} __attribute__((packed)) cl_spcl_pkt_key_t;

/* CL_MCST_KEY 231 Bits */
typedef struct cl_mcst_key_s {
	ca_uint32_t msk_ip_da_sa_equal :1;
	ca_uint32_t ip_da_sa_equal :1;
	ca_uint32_t msk_ip_da_mc :1;
	ca_uint32_t ip_da_mc :1;

	ca_uint32_t msk_ip_ver :1;
	ca_uint32_t ip_ver :1;
	ca_uint32_t msk_ip_vld :1;
	ca_uint32_t ip_vld :1;

	ca_uint32_t msk_pppoe_session :1;
	ca_uint32_t pppoe_session_id :16;

	ca_uint32_t msk_inner_vid :1;
	ca_uint32_t inner_vid :12;
	ca_uint32_t msk_inner_tpid_enc :1;
	ca_uint32_t inner_tpid_enc :3;
	ca_uint32_t msk_top_dei :1;
	ca_uint32_t top_dei :1;
	ca_uint32_t msk_top_vl_802_1p :1;
	ca_uint32_t top_802_1p :3;
	ca_uint32_t msk_top_vid_ptag :1;
	ca_uint32_t top_vid_ptag :1;

	ca_uint32_t msk_top_vid :1;
	ca_uint32_t top_vid :12;
	ca_uint32_t msk_top_tpid_enc :1;
	ca_uint32_t top_tpid_enc :3;
	ca_uint32_t msk_vlan_cnt :1;
	ca_uint32_t vlan_cnt :2;

	ca_uint32_t msk_mac_da_ip_mc :1;
	ca_uint32_t mac_da_ip_mc :1;
	ca_uint32_t msk_mac_da_mc :1;
	ca_uint32_t mac_da_mc :1;
	ca_uint32_t msk_mac_da_bc :1;
	ca_uint32_t mac_da_bc :1;

	ca_uint32_t msk_dest_addr :1;
	ca_uint32_t dest_addr_type :1;
	ca_uint32_t dest_addr_0 :32;
	ca_uint32_t dest_addr_1 :32;
	ca_uint32_t dest_addr_2 :32;
	ca_uint32_t dest_addr_3 :32;

	ca_uint32_t msk_mcgid :1;
	ca_uint32_t mcgid :10;
	ca_uint32_t msk_o_lspid :1;
	ca_uint32_t o_lspid :6;
	ca_uint32_t msk_lspid :1;
	ca_uint32_t lspid :6;

	ca_uint32_t rsv1 :32;
	ca_uint32_t rsv2 :32;
	ca_uint32_t rsv3 :32;
	ca_uint32_t rsv4 :5;
	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv5 :7;
} __attribute__((packed)) cl_mcst_key_t;

/* CL_FULL_KEY 615 Bits */
typedef struct cl_full_key_s{
	ca_uint32_t msk_tcp_rdp_ctrl :1;
	ca_uint32_t tcp_rdp_ctrl :9;
	ca_uint32_t msk_l4_port :3;
	ca_uint32_t l4_dp :16;
	ca_uint32_t l4_sp :16;
	ca_uint32_t msk_l4_vld :1;
	ca_uint32_t l4_vld :1;
	ca_uint32_t msk_l3_chksum_err :1;
	ca_uint32_t l3_chksum_err :1;
	ca_uint32_t msk_spi :1;

	ca_uint32_t msk_ip_fragment :1;
	ca_uint32_t ip_fragment :1;
	ca_uint32_t msk_ip_options :1;
	ca_uint32_t ip_options :1;

	ca_uint32_t msk_ip_da_sa_equal :1;
	ca_uint32_t ip_da_sa_equal :1;
	ca_uint32_t msk_ip_da_mc :1;
	ca_uint32_t ip_da_mc :1;

	ca_uint32_t msk_ip_da_l :7;
	ca_uint32_t msk_ip_da_h :2;
	ca_uint32_t ip_da_0 :32;
	ca_uint32_t ip_da_1 :32;
	ca_uint32_t ip_da_2 :32;
	ca_uint32_t ip_da_3 :32;

	ca_uint32_t msk_ip_sa_l :7;
	ca_uint32_t msk_ip_sa_h :2;
	ca_uint32_t ip_sa_0 :32;
	ca_uint32_t ip_sa_1 :32;
	ca_uint32_t ip_sa_2 :32;
	ca_uint32_t ip_sa_3 :32;

	ca_uint32_t cls_entry_mode :1;
	ca_uint32_t cls_pri :4;
	ca_uint32_t cls_rslt_type :1;
	ca_uint32_t cls_key_type :3;
	ca_uint32_t cls_key_valid :4;
	ca_uint32_t rsv4 :7;

	ca_uint32_t msk_ip_protocol :1;
	ca_uint32_t ip_protocol :8;
	ca_uint32_t msk_ip_ecn :1;
	ca_uint32_t ip_ecn :2;
	ca_uint32_t msk_dscp :1;
	ca_uint32_t ip_dscp :6;
	ca_uint32_t msk_ip_ver :1;
	ca_uint32_t ip_ver :1;
	ca_uint32_t msk_ip_vld :1;
	ca_uint32_t ip_vld :1;
	ca_uint32_t msk_ppp_protocol :1;
	ca_uint32_t ppp_protocol_enc :4;
	ca_uint32_t msk_pppoe_session :1;
	ca_uint32_t pppoe_session_id :16;
	ca_uint32_t msk_inner_dei :1;
	ca_uint32_t inner_dei :1;
	ca_uint32_t msk_inner_802_1p :1;
	ca_uint32_t inner_802_1p :3;
	ca_uint32_t msk_inner_vid :1;
	ca_uint32_t inner_vid :12;
	ca_uint32_t msk_inner_tpid_enc :1;
	ca_uint32_t inner_tpid_enc :3;
	ca_uint32_t msk_top_dei :1;
	ca_uint32_t top_dei :1;
	ca_uint32_t msk_top_vl_802_1p :1;
	ca_uint32_t top_802_1p :3;
	ca_uint32_t msk_top_vid_ptag :1;
	ca_uint32_t top_vid_ptag :1;
	ca_uint32_t vid_ctl :2;

	ca_uint32_t top_vid :12;
	ca_uint32_t msk_top_tpid_enc :1;
	ca_uint32_t top_tpid_enc :3;
	ca_uint32_t msk_vlan_cnt :1;
	ca_uint32_t vlan_cnt :2;
	ca_uint32_t llc_type_enc_mask :1;
	ca_uint32_t llc_type_enc :2;
	ca_uint32_t pktlen_rng_match_mask :1;
	ca_uint32_t pktlen_rng_match_vec :4;
	ca_uint32_t msk_len_encoded :1;
	ca_uint32_t len_encoded :1;
	ca_uint32_t msk_ethertype_enc :1;
	ca_uint32_t ethertype_enc :6;
	ca_uint32_t msk_mac_sa :1;
	ca_uint32_t mac_sa0:8;
	ca_uint32_t mac_sa1:8;
	ca_uint32_t mac_sa2:8;
	ca_uint32_t mac_sa3:8;
	ca_uint32_t mac_sa4:8;
	ca_uint32_t mac_sa5:8;

	ca_uint32_t msk_mac_da_rsvd :1;
	ca_uint32_t mac_da_rsvd :1;
	ca_uint32_t msk_mac_da_ip_mc :1;
	ca_uint32_t mac_da_ip_mc :1;
	ca_uint32_t msk_mac_da_mc :1;
	ca_uint32_t mac_da_mc :1;
	ca_uint32_t msk_mac_da_bc :1;
	ca_uint32_t mac_da_bc :1;
	ca_uint32_t msk_mac_da_an_hit :1;
	ca_uint32_t mac_da_an_hit :1;
	ca_uint32_t msk_mac_da_an_sel :1;
	ca_uint32_t mac_da_an_sel :4;

	ca_uint32_t msk_mdata :16;
	ca_uint32_t mdata_0 :32;
	ca_uint32_t mdata_1 :32;

	ca_uint32_t msk_mcgid :1;
	ca_uint32_t mcgid :10;
	ca_uint32_t msk_o_lspid :1;
	ca_uint32_t o_lspid :6;
	ca_uint32_t msk_lspid :1;
	ca_uint32_t lspid :6;


	ca_uint32_t rsv4_2 :17;
	ca_uint32_t rsv4_3 :32;
	ca_uint32_t cls_entry_mode_1 :1;
	ca_uint32_t cls_pri_1 :4;
	ca_uint32_t cls_rslt_type_1 :1;
	ca_uint32_t cls_key_type_1 :3;
	ca_uint32_t cls_key_valid_1 :4;
	ca_uint32_t rsv4_1 :7;
} __attribute__((packed)) cl_full_key_t;

typedef struct l3_cls_key_tbl_entry_s {
	ca_uint32_t	cls_key_tbl_data_0;
	ca_uint32_t	cls_key_tbl_data_1;
	ca_uint32_t	cls_key_tbl_data_2;
	ca_uint32_t	cls_key_tbl_data_3;
	ca_uint32_t	cls_key_tbl_data_4;
	ca_uint32_t	cls_key_tbl_data_5;
	ca_uint32_t	cls_key_tbl_data_6;
	ca_uint32_t	cls_key_tbl_data_7;
	ca_uint32_t	cls_key_tbl_data_8;
	ca_uint32_t	cls_key_tbl_data_9;
	ca_uint32_t	cls_key_tbl_data_10		:12;
	ca_uint32_t	cls_entry_mode			:1;
	ca_uint32_t	cls_pri					:4;
	ca_uint32_t	cls_rslt_type			:1;
	ca_uint32_t	cls_key_type			:3;
	ca_uint32_t	cls_key_valid			:4;
	ca_uint32_t	reserved				:7;
} __attribute__((packed)) l3_cls_key_tbl_entry_t;

typedef struct l3_cls_key_s {
	union {
		cl_if_id_key_t cl_if_id_key;
		cl_ipv4_tunnel_id_key_t cl_ipv4_tunnel_key;
		cl_ipv4_short_key_t cl_ipv4_short_key;
		cl_ipv6_short_key_t cl_ipv6_short_key;
		cl_spcl_pkt_key_t cl_spcl_pkt_key;
		cl_mcst_key_t cl_mcst_key;
		cl_full_key_t cl_full_key;
		l3_cls_key_tbl_entry_t key_tbl_data[2];
	} cls_key_info;
} __attribute__((packed)) l3_cls_key_t;


/******************************************************
			FIB
*******************************************************/
typedef struct cls_fib_mod_0_s {
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
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
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_fib_mod_0_t;

typedef struct cls_fib_mod_1_s {
	/* msk_vlan */
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
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_fib_mod_1_t;

typedef struct cls_fib_mod_2_s {
	/* msk_pppoe */
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
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
	/* msk_mac_sa */
	ca_uint32_t	mac_sa_vld		: 1;
	ca_uint32_t	mac_sa_0		: 8;
	ca_uint32_t	mac_sa_1		: 8;
	ca_uint32_t	mac_sa_2		: 8;
	ca_uint32_t	mac_sa_3		: 8;
	ca_uint32_t	mac_sa_4		: 8;
	ca_uint32_t	mac_sa_5		: 8;
	/* msk_mac_da */
	ca_uint32_t	mac_da_vld		: 1;
	ca_uint32_t	mac_da_0		: 8;
	ca_uint32_t	mac_da_1		: 8;
	ca_uint32_t	mac_da_2		: 8;
	ca_uint32_t	mac_da_3		: 8;
	ca_uint32_t	mac_da_4		: 8;
	ca_uint32_t	mac_da_5		: 8;
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1_0		: 7;

	ca_uint32_t rsv0 : 18;

	ca_uint32_t	mdata_w_1_1		: 9;
	/* msk_mdata_w2 */
	ca_uint32_t	mdata_w_vld_2		: 4;
	ca_uint32_t	mdata_w_2		: 16;
	/* msk_mdata_w3 */
	ca_uint32_t	mdata_w_vld_3		: 4;
	ca_uint32_t	mdata_w_3		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_fib_mod_2_t;

typedef struct cls_fib_mod_3_s {
	/* msk_l4_sp */
	ca_uint32_t	l4_sp_vld		: 1;
	ca_uint32_t	l4_sp			: 16;
	/* msk_ip_sa_l */
	ca_uint32_t	ip_sa_l_vld		: 1;
	ca_uint32_t	ip_sa_0			: 32;
	/* msk_ip_sa_h */
	ca_uint32_t	ip_sa_h_vld		: 1;
	ca_uint32_t	ip_sa_1			: 32;
	ca_uint32_t	ip_sa_2			: 32;
	ca_uint32_t	ip_sa_3			: 32;
	/* msk_pppoe */
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
	ca_uint32_t	vlan_vld		: 1;
	ca_uint32_t	inner_dei_sel		: 2;
	ca_uint32_t	inner_dei		: 1;
	ca_uint32_t	inner_vid		: 12;
	ca_uint32_t	inner_tpid_enc		: 3;
	ca_uint32_t	top_dei_sel		: 2;

	ca_uint32_t rsv0 : 18;

	ca_uint32_t	top_dei			: 1;
	ca_uint32_t	top_vid			: 12;
	ca_uint32_t	top_tpid_enc		: 3;
	ca_uint32_t	vlan_cnt		: 2;
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_fib_mod_3_t;

typedef struct cls_fib_mod_4_s {
	/* msk_l4_dp */
	ca_uint32_t	l4_dp_vld		: 1;
	ca_uint32_t	l4_dp			: 16;
	/* msk_ip_da_l */
	ca_uint32_t	ip_da_l_vld		: 1;
	ca_uint32_t	ip_da_0			: 32;
	/* msk_ip_da_h */
	ca_uint32_t	ip_da_h_vld		: 1;
	ca_uint32_t	ip_da_1			: 32;
	ca_uint32_t	ip_da_2			: 32;
	ca_uint32_t	ip_da_3			: 32;
	/* msk_pppoe */
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
	ca_uint32_t	vlan_vld		: 1;
	ca_uint32_t	inner_dei_sel		: 2;
	ca_uint32_t	inner_dei		: 1;
	ca_uint32_t	inner_vid		: 12;
	ca_uint32_t	inner_tpid_enc		: 3;
	ca_uint32_t	top_dei_sel		: 2;

	ca_uint32_t rsv0 : 18;

	ca_uint32_t	top_dei			: 1;
	ca_uint32_t	top_vid			: 12;
	ca_uint32_t	top_tpid_enc		: 3;
	ca_uint32_t	vlan_cnt		: 2;
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_fib_mod_4_t;

typedef struct l3_cls_fib_tbl_entry_s {
	ca_uint32_t	fib_tbl_data_0;
	ca_uint32_t	fib_tbl_data_1;
	ca_uint32_t	fib_tbl_data_2;
	ca_uint32_t	fib_tbl_data_3;
	ca_uint32_t	fib_tbl_data_4;
	ca_uint32_t	fib_tbl_data_5;
	ca_uint32_t	fib_tbl_data_6	:11;
	ca_uint32_t	fib_format_sel	:3;
	ca_uint32_t	reserved		:18;
} __attribute__((packed)) l3_cls_fib_tbl_entry_t;

typedef struct cls_fib_s {
	union {
		cls_fib_mod_0_t fib_mod_0;
		cls_fib_mod_1_t fib_mod_1;
		cls_fib_mod_2_t fib_mod_2;
		cls_fib_mod_3_t fib_mod_3;
		cls_fib_mod_4_t fib_mod_4;
		l3_cls_fib_tbl_entry_t fib_tbl_data[2];
	} cls_fib_data;
} __attribute__((packed)) cls_fib_t;

/******************************************************
			Debug FIB RESULT
*******************************************************/
typedef struct cls_dbg_fib_data_s {
	/* msk_l4_dp */
	ca_uint32_t	l4_dp_vld		: 1;
	ca_uint32_t	l4_dp			: 16;
	/* msk_l4_sp */
	ca_uint32_t	l4_sp_vld		: 1;
	ca_uint32_t	l4_sp			: 16;
	/* msk_ip_da_l */
	ca_uint32_t	ip_da_l_vld		: 1;
	ca_uint32_t	ip_da_0			: 32;
	/* msk_ip_da_h */
	ca_uint32_t	ip_da_h_vld		: 1;
	ca_uint32_t	ip_da_1			: 32;
	ca_uint32_t	ip_da_2			: 32;
	ca_uint32_t	ip_da_3			: 32;
	/* msk_ip_sa_l */
	ca_uint32_t	ip_sa_l_vld		: 1;
	ca_uint32_t	ip_sa_0			: 32;
	/* msk_ip_sa_h */
	ca_uint32_t	ip_sa_h_vld		: 1;
	ca_uint32_t	ip_sa_1			: 32;
	ca_uint32_t	ip_sa_2			: 32;
	ca_uint32_t	ip_sa_3			: 32;
	/* msk_pppoe */
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
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
	/* msk_mac_sa */
	ca_uint32_t	mac_sa_vld		: 1;
	ca_uint32_t	mac_sa_0		: 8;
	ca_uint32_t	mac_sa_1		: 8;
	ca_uint32_t	mac_sa_2		: 8;
	ca_uint32_t	mac_sa_3		: 8;
	ca_uint32_t	mac_sa_4		: 8;
	ca_uint32_t	mac_sa_5		: 8;
	/* msk_mac_da */
	ca_uint32_t	mac_da_vld		: 1;
	ca_uint32_t	mac_da_0		: 8;
	ca_uint32_t	mac_da_1		: 8;
	ca_uint32_t	mac_da_2		: 8;
	ca_uint32_t	mac_da_3		: 8;
	ca_uint32_t	mac_da_4		: 8;
	ca_uint32_t	mac_da_5		: 8;
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_mdata_w2 */
	ca_uint32_t	mdata_w_vld_2		: 4;
	ca_uint32_t	mdata_w_2		: 16;
	/* msk_mdata_w3 */
	ca_uint32_t	mdata_w_vld_3		: 4;
	ca_uint32_t	mdata_w_3		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
} __attribute__((packed)) cls_dbg_fib_data_t;

typedef struct cls_fib_dbg_s {
	cls_dbg_fib_data_t cls_fib_data;
	ca_uint32_t cls_hit_addr	:10;
	ca_uint32_t cls_std_fib	:1;
	ca_uint32_t cls_hit		:1;
	ca_uint32_t rsv_0		:20;
	ca_uint32_t rsv_1[6];
} __attribute__((packed)) cls_fib_dbg_t;

typedef struct cls_key_s {
	ca_uint32_t spcl_pkt_hdr_mtch_msk :1;
	ca_uint32_t spcl_pkt_hdr_mtch :8;
	ca_uint32_t spcl_pkt_enc_msk :1;
	ca_uint32_t spcl_pkt_enc :6;
	ca_uint32_t msk_tcp_rdp_ctrl :1;
	ca_uint32_t tcp_rdp_ctrl :9;
	ca_uint32_t msk_l4_port :3;
	ca_uint32_t l4_dp :16;
	ca_uint32_t l4_sp :16;
	ca_uint32_t msk_l4_vld :1;
	ca_uint32_t l4_vld :1;
	ca_uint32_t msk_l3_chksum_err :1;
	ca_uint32_t l3_chksum_err :1;
	ca_uint32_t msk_spi :1;
	ca_uint32_t msk_icmp_type :1;
	ca_uint32_t icmp_type :8;
	ca_uint32_t msk_icmp_vld :1;
	ca_uint32_t icmp_vld :3;
	ca_uint32_t msk_ipv6_doh :1;
	ca_uint32_t ipv6_doh :1;
	ca_uint32_t msk_ipv6_rh :1;
	ca_uint32_t ipv6_rh :1;
	ca_uint32_t msk_ipv6_hbh :1;
	ca_uint32_t ipv6_hbh :1;
	ca_uint32_t msk_ipv6_ndp :1;
	ca_uint32_t ipv6_ndp :1;
	ca_uint32_t msk_ip_fragment :1;
	ca_uint32_t ip_fragment :1;
	ca_uint32_t msk_ip_options :1;
	ca_uint32_t ip_options :1;
	ca_uint32_t msk_ipv6_flow_lbl :1;
	ca_uint32_t ipv6_flow_lbl :20;
	ca_uint32_t ip_addr_type :1;
	ca_uint32_t msk_ip_addr_l :7;
	ca_uint32_t msk_ip_addr_h :2;
	cls_ip_addr_t ip_addr;
	ca_uint32_t msk_ip_da_sa_equal :1;
	ca_uint32_t ip_da_sa_equal :1;
	ca_uint32_t msk_ip_da_mc :1;
	ca_uint32_t ip_da_mc :1;
	ca_uint32_t msk_ip_da_enc :1;
	ca_uint32_t ip_da_enc :4;
	ca_uint32_t msk_ip_da_l :7;
	ca_uint32_t msk_ip_da_h :2;
	cls_ip_addr_t ip_da;
	ca_uint32_t msk_ip_sa_enc :1;
	ca_uint32_t ip_sa_enc :4;
	ca_uint32_t msk_ip_sa_l :7;
	ca_uint32_t msk_ip_sa_h :2;
	cls_ip_addr_t ip_sa;
	ca_uint32_t msk_ip_protocol :1;
	ca_uint32_t ip_protocol :8;
	ca_uint32_t msk_ip_ecn :1;
	ca_uint32_t ip_ecn :2;
	ca_uint32_t msk_dscp :1;
	ca_uint32_t ip_dscp :6;
	ca_uint32_t msk_ip_ver :1;
	ca_uint32_t ip_ver :1;
	ca_uint32_t msk_ip_vld :1;
	ca_uint32_t ip_vld :1;
	ca_uint32_t msk_ppp_protocol :1;
	ca_uint32_t ppp_protocol_enc :4;
	ca_uint32_t msk_pppoe_session :1;
	ca_uint32_t pppoe_session_id :16;
	ca_uint32_t msk_inner_dei :1;
	ca_uint32_t inner_dei :1;
	ca_uint32_t msk_inner_802_1p :1;
	ca_uint32_t inner_802_1p :3;
	ca_uint32_t msk_inner_vid :1;
	ca_uint32_t inner_vid :12;
	ca_uint32_t msk_inner_tpid_enc :1;
	ca_uint32_t inner_tpid_enc :3;
	ca_uint32_t msk_top_dei :1;
	ca_uint32_t top_dei :1;
	ca_uint32_t msk_top_vl_802_1p :1;
	ca_uint32_t top_802_1p :3;
	ca_uint32_t msk_top_vid_ptag :1;
	ca_uint32_t top_vid_ptag :1;
	ca_uint32_t vid_ctl :2;
	ca_uint32_t msk_top_vid :1;
	ca_uint32_t top_vid :12;
	ca_uint32_t msk_top_tpid_enc :1;
	ca_uint32_t top_tpid_enc :3;
	ca_uint32_t msk_vlan_cnt :1;
	ca_uint32_t vlan_cnt :2;
	ca_uint32_t llc_type_enc_mask :1;
	ca_uint32_t llc_type_enc :2;
	ca_uint32_t pktlen_rng_match_mask :1;
	ca_uint32_t pktlen_rng_match_vec :4;
	ca_uint32_t msk_len_encoded :1;
	ca_uint32_t len_encoded :1;
	ca_uint32_t msk_ethertype_enc :1;
	ca_uint32_t ethertype_enc :6;
	ca_uint32_t msk_mac_sa :1;
	ca_mac_addr_t mac_sa;
	ca_uint32_t msk_mac_da_rsvd :1;
	ca_uint32_t mac_da_rsvd :1;
	ca_uint32_t msk_mac_da_ip_mc :1;
	ca_uint32_t mac_da_ip_mc :1;
	ca_uint32_t msk_mac_da_mc :1;
	ca_uint32_t mac_da_mc :1;
	ca_uint32_t msk_mac_da_bc :1;
	ca_uint32_t mac_da_bc :1;
	ca_uint32_t msk_mac_da_an_hit :1;
	ca_uint32_t mac_da_an_hit :1;
	ca_uint32_t msk_mac_da_an_sel :1;
	ca_uint32_t mac_da_an_sel :4;
	ca_uint32_t msk_mac_da :1;
	ca_mac_addr_t mac_da;
	ca_uint32_t msk_dest_addr :1;
	ca_uint32_t dest_addr_type :1;
	union {
		ca_mac_addr_t dest_mac_addr;
		cls_ip_addr_t dest_ip_addr;
	} dest_addr;
	ca_uint32_t msk_mdata :16;
	cls_mdata_t mdata;
	ca_uint32_t msk_mcgid :1;
	ca_uint32_t mcgid :10;
	ca_uint32_t msk_o_lspid :1;
	ca_uint32_t o_lspid :6;
	ca_uint32_t msk_lspid :1;
	ca_uint32_t lspid :6;
} __attribute__((packed)) cls_key_t;

typedef struct cls_action_s {
	/* msk_l4_dp */
	ca_uint32_t	l4_dp_vld		: 1;
	ca_uint32_t	l4_dp			: 16;
	/* msk_l4_sp */
	ca_uint32_t	l4_sp_vld		: 1;
	ca_uint32_t	l4_sp			: 16;
	/* msk_ip_da_l */
	ca_uint32_t	ip_da_l_vld		: 1;
	ca_uint32_t	ip_da_0			: 32;
	/* msk_ip_da_h */
	ca_uint32_t	ip_da_h_vld		: 1;
	ca_uint32_t	ip_da_1			: 32;
	ca_uint32_t	ip_da_2			: 32;
	ca_uint32_t	ip_da_3			: 32;
	/* msk_ip_sa_l */
	ca_uint32_t	ip_sa_l_vld		: 1;
	ca_uint32_t	ip_sa_0			: 32;
	/* msk_ip_sa_h */
	ca_uint32_t	ip_sa_h_vld		: 1;
	ca_uint32_t	ip_sa_1			: 32;
	ca_uint32_t	ip_sa_2			: 32;
	ca_uint32_t	ip_sa_3			: 32;
	/* msk_pppoe */
	ca_uint32_t	pppoe_vld		: 1;
	ca_uint32_t	pppoe_set		: 1;
	ca_uint32_t	pppoe_session_id	: 16;
	/* msk_l3_egress_if */
	ca_uint32_t	ip_ttl_vld		: 1;
	ca_uint32_t	ip_ttl_zero_discard_en	: 1;
	ca_uint32_t	ip_ttl_cmd		: 2;
	ca_uint32_t	ip_ttl			: 8;
	ca_uint32_t	six_rd_ipda_ctrl_vld	: 1;
	ca_uint32_t	six_rd_ipda_from_v6	: 1;
	ca_uint32_t	ip_mtu_enc_vld		: 1;
	ca_uint32_t	ip_mtu_enc		: 2;
	ca_uint32_t	l2_format_vld		: 1;
	ca_uint32_t	l2_format		: 1;
	ca_uint32_t	l3_egress_if_vld	: 1;
	/* msk_vlan */
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
	/* msk_mac_sa */
	ca_uint32_t	mac_sa_vld		: 1;
	ca_uint32_t	mac_sa_0		: 8;
	ca_uint32_t	mac_sa_1		: 8;
	ca_uint32_t	mac_sa_2		: 8;
	ca_uint32_t	mac_sa_3		: 8;
	ca_uint32_t	mac_sa_4		: 8;
	ca_uint32_t	mac_sa_5		: 8;
	/* msk_mac_da */
	ca_uint32_t	mac_da_vld		: 1;
	ca_uint32_t	mac_da_0		: 8;
	ca_uint32_t	mac_da_1		: 8;
	ca_uint32_t	mac_da_2		: 8;
	ca_uint32_t	mac_da_3		: 8;
	ca_uint32_t	mac_da_4		: 8;
	ca_uint32_t	mac_da_5		: 8;
	/* msk_mdata_w0 */
	ca_uint32_t	mdata_w_vld_0		: 4;
	ca_uint32_t	mdata_w_0		: 16;
	/* msk_mdata_w1 */
	ca_uint32_t	mdata_w_vld_1		: 4;
	ca_uint32_t	mdata_w_1		: 16;
	/* msk_mdata_w2 */
	ca_uint32_t	mdata_w_vld_2		: 4;
	ca_uint32_t	mdata_w_2		: 16;
	/* msk_mdata_w3 */
	ca_uint32_t	mdata_w_vld_3		: 4;
	ca_uint32_t	mdata_w_3		: 16;
	/* msk_pol */
	ca_uint32_t	pol_vld			: 1;
	ca_uint32_t	qos_premark		: 1;
	ca_uint32_t	pol_all_bypass		: 1;
	ca_uint32_t	pol_en			: 2;
	ca_uint32_t	pol_base		: 9;
	ca_uint32_t	pol_sel			: 3;
	ca_uint32_t	pol_table_sel		: 1;
	ca_uint32_t	pol_grp_vld		: 1;
	ca_uint32_t	pol_grp_id		: 3;
	/* msk_qos */
	ca_uint32_t	ip_ecn_vld		: 1;
	ca_uint32_t	ip_ecn_en		: 1;
	ca_uint32_t	ip_tos_vld		: 1;
	ca_uint32_t	ip_tos_6		: 1;
	ca_uint32_t	ip_dscp_marked_vld	: 1;
	ca_uint32_t	ip_dscp_marked_down	: 6;
	ca_uint32_t	ip_dscp_markdown_en	: 1;
	ca_uint32_t	ip_dscp_update_en	: 2;
	ca_uint32_t	ip_dscp_sel		: 3;
	ca_uint32_t	ip_dscp			: 6;
	ca_uint32_t	dscp_table_sel		: 1;
	ca_uint32_t	inner_802_1p_sel	: 3;
	ca_uint32_t	inner_802_1p		: 3;
	ca_uint32_t	top_802_1p_sel		: 3;
	ca_uint32_t	top_802_1p		: 3;
	ca_uint32_t	qos_802_1p_table_sel	: 1;
	ca_uint32_t	cos_table_sel		: 1;
	ca_uint32_t	cos_sel			: 2;
	ca_uint32_t	cos			: 3;
	/* msk_dpid */
	ca_uint32_t	mrr_vld			: 1;
	ca_uint32_t	mrr_en			: 1;
	ca_uint32_t	no_drop_vld		: 1;
	ca_uint32_t	no_drop			: 1;
	ca_uint32_t	dpid_vld		: 1;
	ca_uint32_t	dpid_pri		: 1;
	ca_uint32_t	permit			: 1;
	ca_uint32_t	deepq			: 1;
	ca_uint32_t	mcgid			: 10;
	ca_uint32_t	mc			: 1;
	/* msk_ctrl */
	ca_uint32_t	chk_msk_ptr		: 6;
	ca_uint32_t	cache_ctrl		: 2;
	ca_uint32_t	pop_l3_vld		: 1;
	ca_uint32_t	pop_l3_chk_ecn_en	: 1;
	ca_uint32_t	pop_l3_en		: 1;
	ca_uint32_t	keep_ts_vld		: 1;
	ca_uint32_t	keep_ts_en		: 1;
	ca_uint32_t	keep_orig_pkt_vld	: 1;
	ca_uint32_t	keep_orig_pkt		: 1;
	ca_uint32_t	stage3_ctrl_vld		: 1;
	ca_uint32_t	stage3_ctrl		: 1;
	ca_uint32_t	stage2_ctrl_vld		: 1;
	ca_uint32_t	stage2_ctrl		: 1;
	ca_uint32_t	t5_ctrl_vld		: 1;
	ca_uint32_t	t5_ctrl			: 4;
	ca_uint32_t	t4_ctrl_vld		: 1;
	ca_uint32_t	t4_ctrl			: 4;
	ca_uint32_t	t3_ctrl_vld		: 1;
	ca_uint32_t	t3_ctrl			: 4;
	ca_uint32_t	t2_ctrl_vld		: 1;
	ca_uint32_t	t2_ctrl			: 4;
}__attribute__((packed)) cls_action_t;

typedef struct l3_cls_rule_s {
	/* pri: 0-15 */
	cls_rule_prio_t pri;
	/* 0: fwd type, 1: qos type */
	cls_rslt_type_t rslt_type;
	cls_key_t key;
	cls_action_t action;
} l3_cls_rule_t;



int aal_l3_cls_tbl_init(/*CODEGEN_IGNORE_TAG*/void);
void aal_l3_cls_tbl_exit(/*CODEGEN_IGNORE_TAG*/void);
ca_int32_t aal_l3_cls_key_clean(/*CODEGEN_IGNORE_TAG*/cls_key_t *key);
ca_int32_t aal_l3_cls_add(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, ca_uint32_t profile_id, l3_cls_rule_t *rule, ca_uint32_t *rslt_idx);
ca_int32_t aal_l3_cls_update(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, ca_uint32_t profile_id, l3_cls_rule_t *rule, ca_uint32_t rslt_idx);
ca_int32_t aal_l3_cls_delete(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, ca_uint32_t profile_id, ca_uint32_t rslt_idx);
ca_int32_t aal_l3_cls_default_set(/*CODEGEN_IGNORE_TAG*/ca_device_id_t device_id, ca_uint32_t profile_id, cls_rslt_type_t rslt_type, cls_action_t *action);
ca_int32_t aal_l3_spcl_pkt_init(/*CODEGEN_IGNORE_TAG*/void);
void l3fe_debug_dump_cls_fib(/*CODEGEN_IGNORE_TAG*/cls_fib_dbg_t *entry);
ca_status_t ca_cls_rule_dump(/*CODEGEN_IGNORE_TAG*/void);

/* AAL CLI debug functions */
ca_status_t aal_l3_cls_if_id_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_if_id_key_t *entry);

ca_status_t aal_l3_cls_if_id_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_if_id_key_t *entry);

ca_status_t aal_l3_cls_ipv4_short_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_ipv4_short_key_t *entry);

ca_status_t aal_l3_cls_ipv4_short_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_ipv4_short_key_t *entry);

ca_status_t aal_l3_cls_spcl_pkt_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_spcl_pkt_key_t *entry);

ca_status_t aal_l3_cls_spcl_pkt_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_spcl_pkt_key_t *entry);


ca_status_t aal_l3_cls_tunnel_id_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_ipv4_tunnel_id_key_t *entry);

ca_status_t aal_l3_cls_tunnel_id_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_ipv4_tunnel_id_key_t *entry);

ca_status_t aal_l3_cls_ipv6_short_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_ipv6_short_key_t *entry);

ca_status_t aal_l3_cls_ipv6_short_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_ipv6_short_key_t *entry);

ca_status_t aal_l3_cls_mcst_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_mcst_key_t *entry);

ca_status_t aal_l3_cls_mcst_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_mcst_key_t *entry);

ca_status_t aal_l3_cls_full_key_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cl_full_key_t *entry);

ca_status_t aal_l3_cls_full_key_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cl_full_key_t *entry);

ca_status_t aal_l3_cls_key_clean_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT l3_cls_key_type_t *type);

ca_status_t aal_l3_cls_key_clean_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l3_cls_key_type_t type);

ca_status_t aal_l3_cls_fib_0_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cls_fib_mod_0_t *entry);

ca_status_t aal_l3_cls_fib_0_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cls_fib_mod_0_t *entry);

ca_status_t aal_l3_cls_fib_1_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cls_fib_mod_1_t *entry);

ca_status_t aal_l3_cls_fib_1_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cls_fib_mod_1_t *entry);

ca_status_t aal_l3_cls_fib_2_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cls_fib_mod_2_t *entry);

ca_status_t aal_l3_cls_fib_2_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cls_fib_mod_2_t *entry);

ca_status_t aal_l3_cls_fib_3_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cls_fib_mod_3_t *entry);

ca_status_t aal_l3_cls_fib_3_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cls_fib_mod_3_t *entry);

ca_status_t aal_l3_cls_fib_4_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT cls_fib_mod_4_t *entry);

ca_status_t aal_l3_cls_fib_4_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN cls_fib_mod_4_t *entry);

ca_status_t aal_l3_cls_fib_clean_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT l3_cls_fib_mode_t *fib_mode);

ca_status_t aal_l3_cls_fib_clean_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l3_cls_fib_mode_t fib_mode);

#endif /* __AAL_CLS_H__ */

