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
 * aal_l2_vlan.h: Hardware Abstraction Layer for vlan driver to access hardware regsiters
 *
 */
#ifndef __AAL_L2_VLAN_H__
#define __AAL_L2_VLAN_H__
#include "ca_types.h"

#define MC_FIB_COS_CMD           1  

				/*
				   #define G3_REG_BASE     0x2c300000
				   #define __REGISTER_AREA  1

				   
				   #define CA_REG_READ(REG_ADDR)        *(ca_uint32*)REG_ADDR

				   #define CA_REG_WRITE(REG_ADDR, REG_DATA)    do{\
				   *(volatile ca_uint32*)(REG_ADDR) = (REG_DATA);\
				   }while(0)
				 */
typedef enum
{
  AAL_L2_VLAN_CMD_NOP = 0,
  AAL_L2_VLAN_CMD_PUSH = 1,
  AAL_L2_VLAN_CMD_POP = 2,
  AAL_L2_VLAN_CMD_SWAP = 3,
  AAL_L2_VLAN_CMD_MAX = AAL_L2_VLAN_CMD_SWAP
} aal_l2_vlan_cmd_t;

typedef enum
{
  AAL_L2_VLAN_ID_SEL_VID_FIELD = 0,
  AAL_L2_VLAN_ID_SEL_L2_ENGINE = 1,
  AAL_L2_VLAN_ID_SEL_MAX = AAL_L2_VLAN_ID_SEL_L2_ENGINE
} aal_l2_vlan_id_sel_t;

typedef enum
{
  AAL_L2_VLAN_STP_STAT_BLOCKING = 0,
  AAL_L2_VLAN_STP_STAT_LEARN_NOT_FORWARD = 1,
  AAL_L2_VLAN_STP_STAT_FORWARD_NOT_LEARN = 2,
  AAL_L2_VLAN_STP_STAT_FORWARD_AND_LEARN = 3,
  AAL_L2_VLAN_STP_STAT_MAX = AAL_L2_VLAN_CMD_SWAP
} aal_l2_vlan_stp_stat_t;

typedef struct
{
  ca_boolean_t inner_vid_lookup_ena;
  ca_boolean_t dpid_field_vld;
  ca_uint16_t  dpid;
  ca_uint16_t vlan_id;
  ca_boolean_t is_share_vlan;
  ca_boolean_t is_svlan;
  aal_l2_vlan_cmd_t top_vlan_cmd;
  aal_l2_vlan_id_sel_t top_vlan_id_sel;
  ca_boolean_t top_vlan_cmd_field_vld;
  aal_l2_vlan_cmd_t inner_vlan_cmd;
  aal_l2_vlan_id_sel_t inner_vlan_id_sel;
  ca_boolean_t inner_vlan_cmd_field_vld;
  aal_l2_vlan_stp_stat_t stp_stat;
  ca_boolean_t is_mc_vlan;
  ca_boolean_t key_is_share_vlan;
  ca_uint8_t dot1p;
  ca_uint8_t dscp;
  ca_uint8_t cos;
  ca_boolean_t dot1p_field_vld;
  ca_boolean_t dscp_field_vld;
  ca_boolean_t cos_field_vld;
  ca_uint16_t flow_id;
  ca_boolean_t flow_field_vld;
  ca_boolean_t permit;
  ca_boolean_t da_sa_swap_ena;
  ca_uint16_t vlan_group_id;
  ca_uint16_t memship_bm;
  ca_uint16_t untag_bm;
} aal_l2_vlan_fib_t;


typedef union
{
  struct
  {
    ca_uint32_t inner_vid_lookup_ena:1;
    ca_uint32_t dpid_field_vld:1;
    ca_uint32_t dpid:1;
    ca_uint32_t vlan_id:1;
    ca_uint32_t is_share_vlan:1;
    ca_uint32_t is_svlan:1;
    ca_uint32_t top_vlan_cmd:1;
    ca_uint32_t top_vlan_id_sel:1;
    ca_uint32_t top_vlan_cmd_field_vld:1;
    ca_uint32_t inner_vlan_cmd:1;
    ca_uint32_t inner_vlan_id_sel:1;
    ca_uint32_t inner_vlan_cmd_field_vld:1;
    ca_uint32_t stp_stat:1;
    ca_uint32_t is_mc_vlan:1;
    ca_uint32_t key_is_share_vlan:1;
    ca_uint32_t dot1p:1;
    ca_uint32_t dscp:1;
    ca_uint32_t cos:1;
    ca_uint32_t dot1p_field_vld:1;
    ca_uint32_t dscp_field_vld:1;
    ca_uint32_t cos_field_vld:1;
    ca_uint32_t flow_id:1;
    ca_uint32_t flow_field_vld:1;
    ca_uint32_t permit:1;
    ca_uint32_t da_sa_swap_ena:1;
    ca_uint32_t vlan_group_id:1;
    ca_uint32_t memship_bm:1;
    ca_uint32_t untag_bm:1;
    ca_uint32_t reserve:4;
  } s;
  ca_uint32_t u32;
} aal_l2_vlan_fib_mask_t;

typedef enum
{
  AAL_L2_VLAN_TYPE_WAN_SVLAN = 0,
  AAL_L2_VLAN_TYPE_WAN_CVLAN = 1,
  AAL_L2_VLAN_TYPE_LAN_VLAN = 2,
  AAL_L2_VLAN_TYPE_MAX = AAL_L2_VLAN_TYPE_LAN_VLAN
} aal_l2_vlan_type_t;

typedef enum
{
  AAL_L2_VLAN_MODE_IVL = 0,
  AAL_L2_VLAN_MODE_SVL = 1,
  AAL_L2_VLAN_MODE_MAX = AAL_L2_VLAN_MODE_SVL
} aal_l2_vlan_mode_t;

typedef struct
{
  aal_l2_vlan_mode_t vlan_mode;
  ca_uint16_t cmp_tpid_svlan;
  ca_uint16_t cmp_tpid_cvlan;
  ca_uint16_t cmp_tpid_other;
  ca_uint16_t tx_tpid0;
  ca_uint16_t tx_tpid1;
  ca_uint16_t tx_tpid2;
  ca_uint16_t tx_tpid3;
  ca_boolean_t wan_memship_check_ena;
  ca_boolean_t lan_memship_check_ena;
  ca_uint16_t rx_unknown_vlan_memship_check_bmp;
  ca_uint16_t rx_unknown_vlan_grp_id;
  ca_uint16_t tx_unknown_vlan_memship_check_bmp;
  ca_uint16_t tx_unknown_vlan_untag_bmp;
  ca_boolean_t tx_unknown_vlan_stp_mode_forward_ena;
  ca_boolean_t unknown_vlan_fwd_ena;
  ca_boolean_t vlan_4095_fwd_ena;
} aal_l2_vlan_default_cfg_t;

typedef union
{
  struct
  {
    ca_uint32_t vlan_mode:1;
    ca_uint32_t cmp_tpid_svlan:1;
    ca_uint32_t cmp_tpid_cvlan:1;
    ca_uint32_t cmp_tpid_other:1;
    ca_uint32_t tx_tpid0:1;
    ca_uint32_t tx_tpid1:1;
    ca_uint32_t tx_tpid2:1;
    ca_uint32_t tx_tpid3:1;
    ca_uint32_t wan_memship_check_ena:1;
    ca_uint32_t lan_memship_check_ena:1;
    ca_uint32_t rx_unknown_vlan_memship_check_bmp:1;
    ca_uint32_t rx_unknown_vlan_grp_id:1;
    ca_uint32_t tx_unknown_vlan_memship_check_bmp:1;
    ca_uint32_t tx_unknown_vlan_untag_bmp:1;
    ca_uint32_t tx_unknown_vlan_stp_mode_forward_ena:1;
    ca_uint32_t unknown_vlan_fwd_ena:1;
    ca_uint32_t vlan_4095_fwd_ena:1;
    ca_uint32_t reserve:15;
  } s;
  ca_uint32_t u32;
} aal_l2_vlan_default_cfg_mask_t;

typedef struct
{
  ca_uint32_t fib_id;
  ca_boolean_t valid;
} aal_l2_vlan_fib_map_t;

typedef union
{
  struct
  {
    ca_uint32_t fib_id:1;
    ca_uint32_t valid:1;
    ca_uint32_t reserve:30;
  } s;
  ca_uint32_t u32;
} aal_l2_vlan_fib_map_mask_t;

typedef struct{
    ca_uint32_t heada_hi;
    ca_uint32_t heada_mid;
    ca_uint32_t heada_low;
    ca_uint8_t  cpu_flag;
    ca_uint8_t  deep_q;
    ca_uint8_t  policy_group_id;
    ca_uint16_t policy_id;
    ca_uint8_t  policy_ena;
    ca_uint8_t  marked;
    ca_uint8_t  mirror;
    ca_uint8_t  no_drop;
    ca_uint8_t  rx_packet_type;
    ca_uint8_t  drop_code;
    ca_uint16_t mc_group_id;
    ca_uint8_t  header_type;
    ca_uint8_t  fe_bypass;
    ca_uint16_t packet_size;
    ca_uint8_t  logic_spid;
    ca_uint8_t  logic_dpid;
    ca_uint8_t  cos;
    
}aal_l2_fe_heada_t;

typedef struct{
    ca_uint16_t sop_eop_mismatch;
    ca_uint16_t runt_l2_packet;
    ca_uint16_t sob_eob_mismatch;
    ca_uint16_t sop_eop_gap;
    ca_uint8_t  pp_pingpong_overflow;
    ca_uint8_t  ready;
    ca_uint16_t drop_cnt;
    ca_uint16_t sop_cnt;
    ca_uint16_t eop_cnt;
    
}aal_l2_fe_ni_status_t;

typedef struct{
    ca_uint32_t drop_source_cnt_00;
    ca_uint32_t ipv4_cs_chk_drp_01;
    ca_uint32_t dpid_blackhole_drp_02;
    ca_uint32_t ilpb_stp_chk_drp_03;
    ca_uint32_t vlan_stp_chk_drp_04;
    ca_uint32_t ilpb_vlan_type_drp_05;
    ca_uint32_t igr_cls_pmt_stp_drp_06;
    ca_uint32_t vid4095_drp_07;
    ca_uint32_t unknown_vlan_drp_08;
    ca_uint32_t ve_deny_mc_flag_drp_09;
    ca_uint32_t l2e_da_deny_drp_10;
    ca_uint32_t non_std_sa_drp_11;
    ca_uint32_t sw_learn_err_drp_12;
    ca_uint32_t l2e_sa_deny_drp_13;
    ca_uint32_t sa_mis_or_sa_limit_drp_14;
    ca_uint32_t uuc_umc_bc_drp_15;
    ca_uint32_t spid_dpid_not_in_rx_vlan_mem_drp_16;
    ca_uint32_t dpid_not_in_port_mem_drp_17;
    ca_uint32_t spid_dpid_not_in_tx_vlan_mem_drp_18;
    ca_uint32_t tx_vlan_stp_drp_19;
    ca_uint32_t egr_cls_deny_drp_20;
    ca_uint32_t loopback_drp_21;
    ca_uint32_t elpb_stp_drp_22;
    ca_uint32_t drop_source_cnt_23;
    ca_uint32_t drop_source_cnt_24;
    ca_uint32_t mcfib_vlan_drp_25;
    ca_uint32_t drop_source_cnt_26;
    ca_uint32_t mcfib_dpid_equal_spid_drp_27;
    ca_uint32_t drop_source_cnt_28;
    ca_uint32_t drop_source_cnt_29;
    ca_uint32_t drop_source_cnt_30;
    ca_uint32_t drop_source_cnt_31;
}aal_l2_fe_drop_source_t;
typedef enum{
    AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_A  = 0, 
    AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_B  = 1, 
    AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A   = 2, 
    AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_B   = 3,     
    AAL_L2_VLAN_TX_TPID_SEL_MAX         = AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_B, 
}aal_l2_vlan_tx_tpid_sel_t;

typedef struct{
    aal_l2_vlan_tx_tpid_sel_t svlan;
    aal_l2_vlan_tx_tpid_sel_t cvlan;
    aal_l2_vlan_tx_tpid_sel_t inner_svlan;
    aal_l2_vlan_tx_tpid_sel_t inner_cvlan;
}aal_l2_vlan_port_tx_tpid_sel_t;

typedef union
{
    struct{
      ca_uint32_t svlan :1;
      ca_uint32_t cvlan :1;
      ca_uint32_t inner_svlan :1;
      ca_uint32_t inner_cvlan :1;
      ca_uint32_t reserve:28;
    }s;
    ca_uint32_t u32;
}aal_l2_vlan_port_tx_tpid_sel_mask_t;

typedef union{
    struct{
    ca_uint32_t tm2fe_ready;
    ca_uint32_t  sop_cnt;
    ca_uint32_t  eop_cnt;
    }s;
    ca_uint32_t u32;
}ca_pe2tm_cnt_mask_t;


typedef struct{
    ca_boolean_t tm2fe_ready;
    ca_uint16_t  sop_cnt;
    ca_uint16_t  eop_cnt;
}ca_pe2tm_cnt_t;


typedef struct{
    ca_uint16_t  drp_cnt;
    ca_uint16_t  sop_cnt;
    ca_uint16_t  eop_cnt;
}ca_ni2pp_cnt_t;

typedef struct{
    ca_uint32_t tag_num;
}aal_l2_vlan_insert_tag_num_t;

ca_status_t aal_l2_vlan_action_cfg_set (CA_IN ca_device_id_t device_id,
					CA_IN aal_l2_vlan_type_t vlan_type,
					CA_IN ca_uint8_t fib_id,
					CA_IN aal_l2_vlan_fib_mask_t mask,
					CA_IN aal_l2_vlan_fib_t * cfg);

ca_status_t aal_l2_vlan_action_cfg_get (CA_IN ca_device_id_t device_id,
					CA_IN aal_l2_vlan_type_t vlan_type,
					CA_IN ca_uint8_t fib_id,
					CA_OUT aal_l2_vlan_fib_t * cfg);

ca_status_t aal_l2_vlan_fib_map_set (CA_IN ca_device_id_t device_id,
				     CA_IN aal_l2_vlan_type_t vlan_type,
				     CA_IN ca_uint16_t vlan_id,
				     CA_IN aal_l2_vlan_fib_map_mask_t mask,
				     CA_IN aal_l2_vlan_fib_map_t * cfg);

ca_status_t aal_l2_vlan_fib_map_get (CA_IN ca_device_id_t device_id,
				     CA_IN aal_l2_vlan_type_t vlan_type,
				     CA_IN ca_uint16_t vlan_id,
				     CA_OUT aal_l2_vlan_fib_map_t * cfg);

ca_status_t aal_l2_vlan_default_cfg_set (CA_IN ca_device_id_t device_id,
					 CA_IN aal_l2_vlan_default_cfg_mask_t
					 mask,
					 CA_IN aal_l2_vlan_default_cfg_t *
					 cfg);

ca_status_t aal_l2_vlan_default_cfg_get (CA_IN ca_device_id_t device_id,
					 CA_OUT aal_l2_vlan_default_cfg_t *
					 cfg);

ca_status_t aal_l2_fe_pp_heada_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_heada_t *heada);

ca_status_t aal_l2_fe_pp_nierr_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_ni_status_t *err);

ca_status_t aal_l2_fe_pe_heada_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_heada_t *heada);

ca_status_t aal_l2_fe_drop_source_cnt_get(CA_IN  ca_device_id_t device_id,
                                   CA_OUT aal_l2_fe_drop_source_t *drop_source_cnt);

ca_status_t aal_l2_pe2tm_pkt_cnt_set(CA_IN  ca_device_id_t device_id,
                                      CA_IN ca_pe2tm_cnt_mask_t mask,
                                      CA_IN ca_pe2tm_cnt_t *cnt);


ca_status_t aal_l2_pe2tm_pkt_cnt_get(CA_IN  ca_device_id_t device_id,
                                      CA_OUT ca_pe2tm_cnt_t *cnt);

ca_status_t aal_l2_ni2pp_pkt_cnt_set(CA_IN  ca_device_id_t device_id,
                                      CA_IN ca_ni2pp_cnt_t *cnt);

ca_status_t aal_l2_ni2pp_pkt_cnt_get(CA_IN  ca_device_id_t device_id,
                                      CA_OUT ca_ni2pp_cnt_t *cnt);

ca_status_t aal_l2_vlan_port_memship_map_set(CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t logic_port, CA_IN ca_uint32_t mapped_port);

ca_status_t aal_l2_vlan_port_memship_map_get(CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t logic_port, CA_OUT ca_uint32_t *mapped_port);

ca_status_t aal_l2_vlan_tx_tpid_sel_set(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_IN aal_l2_vlan_port_tx_tpid_sel_mask_t mask,
                                        CA_IN aal_l2_vlan_port_tx_tpid_sel_t  *cfg);

ca_status_t aal_l2_vlan_tx_tpid_sel_get(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_OUT aal_l2_vlan_port_tx_tpid_sel_t  *cfg);

ca_status_t aal_l2_vlan_insert_tag_num_set(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_IN aal_l2_vlan_insert_tag_num_t *cfg);

ca_status_t aal_l2_vlan_insert_tag_num_get(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_OUT aal_l2_vlan_insert_tag_num_t *cfg);
ca_status_t aal_l2_unknown_vlan_mc_fib_bmp_get(
    CA_IN ca_device_id_t device_id,
    CA_OUT ca_uint64_t *bmp);

ca_status_t aal_l2_vlan_debug_set(
                CA_IN ca_device_id_t device_id,
                CA_IN ca_boolean_t add_del,
                CA_IN ca_uint16_t vlan,
                CA_IN ca_uint16_t lan_port);

ca_status_t aal_l2_vlan_debug_get(
                CA_IN ca_device_id_t device_id,
                CA_IN ca_boolean_t add_del,
                CA_IN ca_uint16_t vlan,
                CA_OUT ca_uint16_t *lan_port);

#endif
