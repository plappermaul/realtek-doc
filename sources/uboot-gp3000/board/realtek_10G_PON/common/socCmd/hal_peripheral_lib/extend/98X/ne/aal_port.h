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
 * aal_ni.h: Hardware Abstraction Layer for NI driver to access hardware regsiters
 *
 */
#ifndef __AAL_PORT_H__
#define __AAL_PORT_H__

#include "ca_types.h"


#if defined(CONFIG_ARCH_CORTINA_G3)|| defined(CONFIG_ARCH_CORTINA_G3HGU) /*CODEGEN_IGNORE_TAG*/
#define AAL_PORT_NUM_FOR_CLS        8
#define AAL_PORT_START_FOR_CLS      0
#define AAL_PORT_START_FOR_CLS_CPU  64
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define AAL_PORT_NUM_FOR_CLS        4
#define AAL_PORT_START_FOR_CLS      3
#define AAL_PORT_START_FOR_CLS_CPU  80
#elif defined(CONFIG_ARCH_CORTINA_G3LITE)
#define AAL_PORT_NUM_FOR_CLS   		5
#define AAL_PORT_START_FOR_CLS      0
#define AAL_PORT_START_FOR_CLS_CPU  50
#endif/*CODEGEN_IGNORE_TAG*/

//#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
#if 0
#define AAL_PORT_NUM_FIRST_ETH		3
#else//for 98F uboot
#define AAL_PORT_NUM_FIRST_ETH		0
#endif

#define AAL_PORT_NUM_L2RP			2
#define AAL_PORT_NUM_DEEPQ			8
#define AAL_PORT_NUM_CPU			8
#define AAL_PORT_NUM_TRUNK			2

#define AAL_PORT_NUM_PHYSICAL			16
#define AAL_PORT_NUM_LOGICAL			64
#define AAL_PORT_LLID_TYPE_START        32

#define AAL_PORT_NUM_TRUNK_MEMBER		16

#define AAL_PORT_ADDR_MAX			63

/* G3_header, PORT_ID definition */

/**************************************************************************************************
	* Physical Port ID(PPID) definition *
***************************************************************************************************/
#define AAL_PPORT_NI0			0x00
#define AAL_PPORT_NI1			0x01
#define AAL_PPORT_NI2			0x02
#define AAL_PPORT_NI3			0x03
#define AAL_PPORT_NI4			0x04
#define AAL_PPORT_NI5			0x05
#define AAL_PPORT_NI6			0x06
#define AAL_PPORT_NI7			0x07

#define AAL_PPORT_QM			0x08	/* PSPID of L3_L2 Connection and PDPID of L2_L3 Connection in L2 Engine */
#define AAL_PPORT_CPU			0x09	/* PDPID of CPU in L2 Engine */
#define AAL_PPORT_L3_WAN		0x0a
#define AAL_PPORT_MC			0x0b
#define AAL_PPORT_OAM			0x0c
#define AAL_PPORT_L3_LAN		0x0d

#define AAL_PPORT_BLACKHOLE		0x0f

#define AAL_PPORT_MIRROR0		0x0c
#define AAL_PPORT_MIRROR1		0x0d
#define AAL_PPORT_MIRROR2		0x0e
#define AAL_PPORT_MIRROR3		0x0f

#define AAL_PPORT_INVALID		0xff


/**************************************************************************************************
	* Logical Port ID(LPID) definition *
***************************************************************************************************/
extern ca_uint8_t wan_port_id;

#define AAL_LPORT_ETH_NI0		0x00
#define AAL_LPORT_ETH_NI1		0x01
#define AAL_LPORT_ETH_NI2		0x02
#define AAL_LPORT_ETH_NI3		0x03
#define AAL_LPORT_ETH_NI4		0x04
#define AAL_LPORT_ETH_NI5		0x05
#define AAL_LPORT_ETH_NI6		0x06
#define AAL_LPORT_ETH_NI7		0x07

#define /*CODEGEN_IGNORE_TAG*/  AAL_LPORT_ETH_WAN		wan_port_id

#define AAL_LPORT_DEEPQ_0		0x0	/* LSPID of DeepQ in L2 Engine */
#define AAL_LPORT_DEEPQ_7		0x7

#define AAL_LPORT_9QUEUE_NI0	0x08	/* 9th queue  */
#define AAL_LPORT_9QUEUE_NI7	0x0f

#define AAL_LPORT_CPU_0			0x10
#define AAL_LPORT_CPU_1			0x11
#define AAL_LPORT_CPU_2			0x12
#define AAL_LPORT_CPU_3			0x13
#define AAL_LPORT_CPU_4			0x14
#define AAL_LPORT_CPU_5			0x15
#define AAL_LPORT_CPU_6			0x16
#define AAL_LPORT_CPU_7			0x17

#define AAL_LPORT_PE_0			AAL_LPORT_CPU_6		/* PE0 is for decryption or downstream */
#define AAL_LPORT_PE_1			AAL_LPORT_CPU_7		/* PE1 is for encryption or upstream */

#define AAL_LPORT_L3_WAN		0x18	/* LSPID of LAN_L3 and LDPID of L3_LAN in L3 Engine */
#define AAL_LPORT_L3_LAN		0x19	/* LSPID of WAN_L3 and LDPID of L3_WAN in L3 Engine */

#define AAL_LPORT_OAM			0x1a
#define AAL_LPORT_MC			0x1b

#define AAL_LPORT_CPU_Q			0x1d
#define AAL_LPORT_SPCL			0x1e
#define AAL_LPORT_BLACKHOLE		0x1f

#define AAL_LPORT_CPU_MQ_0		0x20
#define AAL_LPORT_CPU_MQ_31		0x3f

//#define AAL_LPORT_TRUNK_0		0x30
//#define AAL_LPORT_TRUNK_1		0x31

#define AAL_LPORT_INVALID		0xff


/**************************************************************************************************
***************************************************************************************************/

/* classifiction rule allocation per port */
#define AAL_ILPB_CLS_START		0

#if defined(CONFIG_ARCH_CORTINA_G3LITE)/*CODEGEN_IGNORE_TAG*/

#define AAL_ILPB_CLS_LENGTH             10
#define AAL_ILPB_CLS_LENGTH_CPU         3
#define AAL_ELPB_CLS_LENGTH             10

#elif defined CONFIG_ARCH_CORTINA_SATURN_SFU
#define AAL_ILPB_CLS_LENGTH             20
#define AAL_ILPB_CLS_LENGTH_CPU         6
#define AAL_ELPB_CLS_LENGTH             20

#else
#define AAL_ILPB_CLS_LENGTH             16
#define AAL_ILPB_CLS_LENGTH_CPU         6
#define AAL_ELPB_CLS_LENGTH             16


#endif/*CODEGEN_IGNORE_TAG*/



#define AAL_ELPB_CLS_START              0


typedef enum {
	AAL_VLAN_CMD_NOP = 0,
	AAL_VLAN_CMD_PUSH = 1,
	AAL_VLAN_CMD_POP = 2,
	AAL_VLAN_CMD_SWAP = 3
} aal_vlan_cmd_t;

typedef enum {
	AAL_STP_STATE_DROP = 0,
	AAL_STP_STATE_LEARN_NO_FWD = 1,
	AAL_STP_STATE_FWD_NO_LEARN = 2,
	AAL_STP_STATE_FWD_AND_LEARN = 3
} aal_stp_state_t;

typedef enum {
	AAL_VLAN_TPID_INDEX_USER_DEFINE = 0,
	AAL_VLAN_TPID_INDEX_88A8 = 1,
	AAL_VLAN_TPID_INDEX_9100 = 2,
	AAL_VLAN_TPID_INDEX_8100 = 3,
} aal_vlan_tpid_index_t;

typedef union {
	struct {
		ca_uint32_t lspid:1;
	} s;
	ca_uint32_t u32[1];
} aal_ippb_cfg_msk_t;

typedef struct ippb {
	ca_uint8_t lspid;
} aal_ippb_cfg_t;

typedef union {
	struct {
		ca_uint32_t wan_ind:1;
		ca_uint32_t igr_port_l2uc_redir_en:1;
		ca_uint32_t igr_port_l2uc_ldpid:1;
		ca_uint32_t igr_port_l2uc_cos:1;
		ca_uint32_t igr_port_l2mc_redir_en:1;
		ca_uint32_t igr_port_l2mc_ldpid:1;
		ca_uint32_t igr_port_l2mc_cos:1;
		ca_uint32_t igr_port_l2bc_redir_en:1;
		ca_uint32_t igr_port_l2bc_ldpid:1;
		ca_uint32_t igr_port_l2bc_cos:1;
		ca_uint32_t default_vlanid:1;
		ca_uint32_t sc_ind:1;
        ca_uint32_t unkwn_pol_idx:1;
		ca_uint32_t vlan_cmd_sel_bm:1;
		ca_uint32_t default_top_vlan_cmd:1;
		ca_uint32_t default_untag_cmd:1;
		ca_uint32_t inner_vid_sel_bm:1;
		ca_uint32_t inner_vlan_cmd:1;
		ca_uint32_t default_noninnertag_cmd:1;
		ca_uint32_t drop_stag_packet:1;
		ca_uint32_t drop_ctag_packet:1;
		ca_uint32_t drop_untagged_packet:1;
		ca_uint32_t drop_pritagged_packet:1;
		ca_uint32_t drop_doubletagged_packet:1;
        ca_uint32_t drop_singletagged_packet:1;
		ca_uint32_t s_tpid_match:1;
		ca_uint32_t c_tpid_match:1;
		ca_uint32_t top_s_tpid_enable:1;
		ca_uint32_t top_c_tpid_enable:1;
		ca_uint32_t inner_s_tpid_enable:1;
		ca_uint32_t inner_c_tpid_enable:1;
		ca_uint32_t other_tpid_match:1;
		ca_uint32_t stp_mode:1;
		ca_uint32_t station_move_en:1;
		ca_uint32_t sa_move_dis_fwd:1;
		ca_uint32_t loop_enable:1;
		ca_uint32_t da_sa_swap_en:1;
		ca_uint32_t spcl_pkt_idx:1;
		ca_uint32_t igr_cls_lookup_en:1;
		ca_uint32_t cls_start:1;
		ca_uint32_t cls_length:1;
		ca_uint32_t flowid_sel_bm:1;
		ca_uint32_t top_802_1p_mode_sel:1;
		ca_uint32_t change_1p_if_pop:1;
		ca_uint32_t dot1p_mark_control_bm:1;
		ca_uint32_t inner_802_1p_mode_sel:1;
		ca_uint32_t default_802_1p:1;
		ca_uint32_t dscp_markdown_en:1;
		ca_uint32_t dscp_mode_sel:1;
		ca_uint32_t dscp_mark_control_bm:1;
		ca_uint32_t default_dscp:1;
		ca_uint32_t cos_mode_sel:1;
		ca_uint32_t cos_control_bm:1;
		ca_uint32_t default_cos:1;
		ca_uint32_t ing_fltr_ena:1;
		ca_uint32_t color_src_sel:1;
		ca_uint32_t dflt_premark:1;
	} s;
	ca_uint32_t u32[2];
} aal_ilpb_cfg_msk_t;

typedef enum{
	AAL_ILPB_VLAN_CMD_SEL_ILPB_TABLE    = 0,
	AAL_ILPB_VLAN_CMD_SEL_VLAN_TABLE    = 1,
	AAL_ILPB_VLAN_CMD_SEL_CLS_TABLE     = 2,
	AAL_ILPB_VLAN_CMD_SEL_BOTH_TABLE    = 3,
	AAL_ILPB_VLAN_CMD_SEL_MAX = AAL_ILPB_VLAN_CMD_SEL_BOTH_TABLE
}aal_ilpb_vlan_cmd_sel_t;

typedef struct ilpb {
	ca_boolean_t wan_ind;

	ca_boolean_t igr_port_l2uc_redir_en;
	ca_uint8_t igr_port_l2uc_ldpid;
	ca_uint8_t igr_port_l2uc_cos;

	ca_boolean_t igr_port_l2mc_redir_en;
	ca_uint8_t igr_port_l2mc_ldpid;
	ca_uint8_t igr_port_l2mc_cos;

	ca_boolean_t igr_port_l2bc_redir_en;
	ca_uint8_t igr_port_l2bc_ldpid;
	ca_uint8_t igr_port_l2bc_cos;

	ca_uint16_t default_vlanid;
	ca_boolean_t sc_ind;
    ca_uint8_t unkwn_pol_idx;

	ca_uint8_t vlan_cmd_sel_bm;
	aal_vlan_cmd_t default_top_vlan_cmd;

	aal_vlan_cmd_t default_untag_cmd;

	ca_uint8_t inner_vid_sel_bm;
	aal_vlan_cmd_t inner_vlan_cmd;

	aal_vlan_cmd_t default_noninnertag_cmd;

	ca_boolean_t drop_stag_packet;
	ca_boolean_t drop_ctag_packet;
	ca_boolean_t drop_untagged_packet;
	ca_boolean_t drop_pritagged_packet;
	ca_boolean_t drop_doubletagged_packet;
    ca_boolean_t drop_singletagged_packet;

	ca_uint8_t s_tpid_match;
	ca_uint8_t c_tpid_match;
	ca_boolean_t top_s_tpid_enable;
	ca_boolean_t top_c_tpid_enable;
	ca_boolean_t inner_s_tpid_enable;
	ca_boolean_t inner_c_tpid_enable;
	ca_uint8_t other_tpid_match;

	aal_stp_state_t stp_mode;

	ca_boolean_t station_move_en;
	ca_boolean_t sa_move_dis_fwd;
    ca_boolean_t loop_enable;

	ca_boolean_t da_sa_swap_en;
	ca_uint8_t spcl_pkt_idx;

	ca_boolean_t igr_cls_lookup_en;
	ca_uint8_t cls_start;
	ca_uint8_t cls_length;

	ca_uint8_t flowid_sel_bm;

	ca_uint8_t top_802_1p_mode_sel;
	ca_boolean_t change_1p_if_pop;
	ca_uint8_t dot1p_mark_control_bm;
	ca_uint8_t inner_802_1p_mode_sel;
	ca_uint8_t default_802_1p;

	ca_boolean_t dscp_markdown_en;
	ca_uint8_t dscp_mode_sel;
	ca_uint8_t dscp_mark_control_bm;
	ca_uint8_t default_dscp;

	ca_uint8_t cos_mode_sel;
	ca_uint8_t cos_control_bm;
	ca_uint8_t default_cos;

	ca_boolean_t ing_fltr_ena;
	ca_uint8_t color_src_sel;
	ca_boolean_t dflt_premark;

} aal_ilpb_cfg_t;

typedef union {
	struct {
		ca_uint32 bypass:1;
		ca_uint32 ldpid:1;
		ca_uint32 cos:1;

	} s;
	ca_uint32_t u32[1];
} aal_dpb_cfg_msk_t;

typedef struct dpb {
	ca_boolean_t bypass;
	ca_uint8_t ldpid;
	ca_uint8_t cos;
} aal_dpb_cfg_t;

typedef enum {
	AAL_VLAN_TPID_C = 0,
	AAL_VLAN_TPID_D = 1,
	AAL_VLAN_TPID_OLD_C = 2,
	AAL_VLAN_TPID_OLD_D = 3
} aal_egr_cvlan_tpid_sel_t;

typedef enum {
	AAL_VLAN_TPID_A = 0,
	AAL_VLAN_TPID_B = 1,
	AAL_VLAN_TPID_OLD_A = 2,
	AAL_VLAN_TPID_OLD_B = 3
} aal_egr_svlan_tpid_sel_t;

typedef union {
	struct {
		ca_uint32_t egr_vlan_aware_mode:1;
		ca_uint32_t egr_port_stp_status:1;
		ca_uint32_t egr_ve_srch_en:1;
		ca_uint32_t egr_dest_wan:1;
		ca_uint32_t egr_cls_length:1;
		ca_uint32_t egr_cls_start:1;
		ca_uint32_t egr_cls_lookup_en:1;
		ca_uint32_t egr_cls_flow_id_ena:1;
		ca_uint32_t egr_cls_cos_ena:1;

	} s;
	ca_uint32_t u32[1];
} aal_elpb_cfg_msk_t;

typedef struct elpb {
	ca_boolean_t egr_vlan_aware_mode;
	ca_boolean_t egr_port_stp_status;
	ca_boolean_t egr_ve_srch_en;
	ca_boolean_t egr_dest_wan;
	ca_uint8_t egr_cls_length;
	ca_uint8_t egr_cls_start;
	ca_boolean_t egr_cls_lookup_en;
	ca_boolean_t egr_cls_flow_id_ena;
	ca_boolean_t egr_cls_cos_ena;
} aal_elpb_cfg_t;

typedef struct trunk {
	ca_uint32 field_sel_cos:1;
	ca_uint32 field_sel_l4_dp:1;
	ca_uint32 field_sel_l4_sp:1;
	ca_uint32 field_sel_ip_da:1;
	ca_uint32 field_sel_ip_sa:1;
	ca_uint32 field_sel_tx_top_vid:1;
	ca_uint32 field_sel_mac_da:1;
	ca_uint32 field_sel_mac_sa:1;
} aal_trunk_hash_key_t;

#define AAL_PPID_CHECK(ppid)/*CODEGEN_IGNORE_TAG*/             if((ppid) > AAL_PORT_NUM_PHYSICAL-1){  return CA_E_PARAM;  }

#define AAL_LPID_CHECK(lpid)/*CODEGEN_IGNORE_TAG*/             if((lpid) > AAL_PORT_NUM_LOGICAL-1) {  return CA_E_PARAM;  }

#define AAL_TRUNK_CHECK(gid)/*CODEGEN_IGNORE_TAG*/             if((gid) > AAL_PORT_NUM_TRUNK-1) {  return CA_E_PARAM;  }

ca_status_t aal_port_ippb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t index,
				CA_IN aal_ippb_cfg_msk_t msk,
				CA_IN aal_ippb_cfg_t * cfg);

ca_status_t aal_port_ippb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t index,
				CA_OUT aal_ippb_cfg_t * cfg);

ca_status_t aal_port_ilpb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_IN aal_ilpb_cfg_msk_t msk,
				CA_IN aal_ilpb_cfg_t * cfg);

ca_status_t aal_port_ilpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT aal_ilpb_cfg_t * cfg);

ca_status_t aal_port_dpb_ctrl_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint8_t ldpid_pattern);

ca_status_t aal_port_dpb_ctrl_get(CA_IN ca_device_id_t dev,
				CA_OUT ca_uint8_t * ldpid_pattern);

ca_status_t aal_port_dpb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid_cos,
				CA_IN aal_dpb_cfg_msk_t msk,
				CA_IN aal_dpb_cfg_t * cfg);

ca_status_t aal_port_dpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid_cos,
				CA_OUT aal_dpb_cfg_t * cfg);

ca_status_t aal_port_elpb_cfg_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_IN aal_elpb_cfg_msk_t msk,
				CA_IN aal_elpb_cfg_t * cfg);

ca_status_t aal_port_elpb_cfg_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT aal_elpb_cfg_t * cfg);

ca_status_t aal_port_mmshp_check_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_IN ca_uint64_t bmp);

ca_status_t aal_port_mmshp_check_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t lpid,
				CA_OUT ca_uint64_t * bmp);

ca_status_t aal_port_trunk_group_hash_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_IN aal_trunk_hash_key_t key);

ca_status_t aal_port_trunk_group_hash_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_OUT aal_trunk_hash_key_t * key);

ca_status_t aal_port_trunk_group_member_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_IN ca_uint64_t bmp);

ca_status_t aal_port_trunk_group_member_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t group_id,
				CA_OUT ca_uint64_t * bmp);

ca_status_t aal_port_trunk_dest_map_set(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t trunk_idx,
				CA_IN ca_uint32_t ldpid);

ca_status_t aal_port_trunk_dest_map_get(CA_IN ca_device_id_t dev,
				CA_IN ca_uint32_t trunk_idx,
				CA_OUT ca_uint32_t * ldpid);

ca_status_t aal_port_arb_ldpid_pdpid_map_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t  my_mac_flag,
    CA_IN ca_uint32_t  dbuf_flag,
    CA_IN ca_uint32_t ldpid,
    CA_IN ca_uint32_t pdpid
);

ca_status_t aal_port_arb_ldpid_pdpid_map_get(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t  my_mac_flag,
    CA_IN ca_uint32_t  dbuf_flag,
    CA_IN ca_uint32_t ldpid,
    CA_OUT ca_uint32_t *pdpid
);
ca_status_t aal_port_init(CA_IN ca_device_id_t dev);	/*CODEGEN_IGNORE_TAG*/

#endif
