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
#ifndef __AAL_L3_SPECPKT_H__
#define __AAL_L3_SPECPKT_H__

#include "ca_types.h"

#define AAL_L3_SPECPKT_SEL_MAX  1
#define AAL_L3_SPECPKT_SEL_NUM  2

#define AAL_L3_SPECPKT_T0_CTRL_L3_LPB_PROC_ENABLE  0
#define AAL_L3_SPECPKT_T0_CTRL_L3_LPB_PROC_DISABLE 1

#define AAL_L3_SPECPKT_T1_CTRL_PROFILE_0    0
#define AAL_L3_SPECPKT_T1_CTRL_PROFILE_1    1
#define AAL_L3_SPECPKT_T1_CTRL_PROFILE_2    2
#define AAL_L3_SPECPKT_T1_CTRL_PROFILE_3    3
#define AAL_L3_SPECPKT_T1_CTRL_BYPASS_MODE  0xF

#define AAL_L3_SPECPKT_T2_CTRL_BYPASS_HASH  0xF

#define AAL_L3_SPECPKT_T3_CTRL_ENABLE_LPM_SEARCH   1
#define AAL_L3_SPECPKT_T3_CTRL_BYPASS              0xF

#define AAL_L3_SPECPKT_T4_CTRL_USE_LPM_AS_ACT_IDX  0xE
#define AAL_L3_SPECPKT_T4_CTRL_BYPASS              0xF

#define AAL_L3_SPECPKT_T5_CTRL_ENABLE_L2_LKUP      0
#define AAL_L3_SPECPKT_T5_CTRL_BYPASS              0xF

#define AAL_L3_SPECPKT_ERR_FWD_CTRL_DROP        0 /* drop */
#define AAL_L3_SPECPKT_ERR_FWD_CTRL_TRAP        1 /* forward to the trap destination */
#define AAL_L3_SPECPKT_ERR_FWD_CTRL_NORMAL_FWD  2 /* forward as normal */

#define AAL_L3_SPECPKT_HTTPLINK_TYPE_BOTH_TCP_UDP 0
#define AAL_L3_SPECPKT_HTTPLINK_TYPE_BOTH_TCP     1
#define AAL_L3_SPECPKT_HTTPLINK_TYPE_BOTH_UDP     2

#define AAL_L3_SPECPKT_HTTPLINK_L4_PORT_TYPE_SRC  0
#define AAL_L3_SPECPKT_HTTPLINK_L4_PORT_TYPE_DST  1

typedef enum {
    AAL_L3_SPECPKT_TYPE_NORMAL = 0x0,
    AAL_L3_SPECPKT_TYPE_BPDU = 0x1,
    AAL_L3_SPECPKT_TYPE_802_DOT1X = 0x2,
    AAL_L3_SPECPKT_TYPE_IEEE_RSVD_GRP = 0x3,
    AAL_L3_SPECPKT_TYPE_MYMAC = 0x4,
    AAL_L3_SPECPKT_TYPE_IEEE_RSVD_A = 0x5,
    AAL_L3_SPECPKT_TYPE_IEEE_RSVD_B = 0x6,
    AAL_L3_SPECPKT_TYPE_IEEE_RSVD_C = 0x7,
    AAL_L3_SPECPKT_TYPE_IGMP = 0x8,
    AAL_L3_SPECPKT_TYPE_ARP = 0x9,
    AAL_L3_SPECPKT_TYPE_EFM_OAM = 0xA,
    AAL_L3_SPECPKT_TYPE_MPCP = 0xB,
    AAL_L3_SPECPKT_TYPE_DHCPV4 = 0xC,
    AAL_L3_SPECPKT_TYPE_PPPOE_DISC = 0xD,
    AAL_L3_SPECPKT_TYPE_PPPOE_SESS = 0xE,
    AAL_L3_SPECPKT_TYPE_HELLO = 0xF,
    AAL_L3_SPECPKT_TYPE_IPV6_NDP = 0x10,
    AAL_L3_SPECPKT_TYPE_IPV4_ICMP = 0x11,
    AAL_L3_SPECPKT_TYPE_IPV6_ICMP = 0x12,
    AAL_L3_SPECPKT_TYPE_CFM_LBM = 0x13,
    AAL_L3_SPECPKT_TYPE_CFM_LMM = 0x14,
    AAL_L3_SPECPKT_TYPE_CFM_1DM_DMM = 0x15,
    AAL_L3_SPECPKT_TYPE_CFM_DMR = 0x16,
    AAL_L3_SPECPKT_TYPE_L2_PTP = 0x17,
    AAL_L3_SPECPKT_TYPE_L4_PTP = 0x18,
    AAL_L3_SPECPKT_TYPE_MLD = 0x19,
    AAL_L3_SPECPKT_TYPE_DHCPV6 = 0x1A,
    AAL_L3_SPECPKT_TYPE_DNS = 0x1B,
    AAL_L3_SPECPKT_TYPE_RIP = 0x1C,
    AAL_L3_SPECPKT_TYPE_SSDP = 0x1D,
    AAL_L3_SPECPKT_TYPE_USER_DEF_0 = 0x1E,
    AAL_L3_SPECPKT_TYPE_USER_DEF_1 = 0x1F,
    AAL_L3_SPECPKT_TYPE_USER_DEF_2 = 0x20,
    AAL_L3_SPECPKT_TYPE_USER_DEF_3 = 0x21,

    AAL_L3_SPECPKT_TYPE_CFM_CCM = 0x22,
    AAL_L3_SPECPKT_TYPE_CFM_LBR = 0x23,
    AAL_L3_SPECPKT_TYPE_CFM_LTR = 0x24,
    AAL_L3_SPECPKT_TYPE_CFM_LTM = 0x25,

    AAL_L3_SPECPKT_TYPE_HTTP_LINK0 = 0x26,
    AAL_L3_SPECPKT_TYPE_HTTP_LINK1 = 0x27,
    AAL_L3_SPECPKT_TYPE_HTTP_LINK2 = 0x28,
    AAL_L3_SPECPKT_TYPE_HTTP_LINK3 = 0x29,

    /* reserved 0x2A ~ 0x2F */

    AAL_L3_SPECPKT_TYPE_L4_MSG_0 = 0x30,
    AAL_L3_SPECPKT_TYPE_L4_MSG_1 = 0x31,
    AAL_L3_SPECPKT_TYPE_L4_MSG_2 = 0x32,
    AAL_L3_SPECPKT_TYPE_L4_MSG_3 = 0x33,
    AAL_L3_SPECPKT_TYPE_L4_MSG_4 = 0x34,
    AAL_L3_SPECPKT_TYPE_L4_MSG_5 = 0x35,
    AAL_L3_SPECPKT_TYPE_L4_MSG_6 = 0x36,
    AAL_L3_SPECPKT_TYPE_L4_MSG_7 = 0x37,
    AAL_L3_SPECPKT_TYPE_L4_MSG_8 = 0x38,
    AAL_L3_SPECPKT_TYPE_L4_MSG_9 = 0x39,
    AAL_L3_SPECPKT_TYPE_L4_MSG_10 = 0x3A,
    AAL_L3_SPECPKT_TYPE_L4_MSG_11 = 0x3B,
    AAL_L3_SPECPKT_TYPE_L4_MSG_12 = 0x3C,
    AAL_L3_SPECPKT_TYPE_L4_MSG_13 = 0x3D,
    AAL_L3_SPECPKT_TYPE_L4_MSG_14 = 0x3E,
    AAL_L3_SPECPKT_TYPE_L4_MSG_15 = 0x3F,

    AAL_L3_SPECPKT_TYPE_HTTP_GET = AAL_L3_SPECPKT_TYPE_L4_MSG_0,
    AAL_L3_SPECPKT_TYPE_HTTP_PUT = AAL_L3_SPECPKT_TYPE_L4_MSG_1,
    AAL_L3_SPECPKT_TYPE_HTTP_HEAD = AAL_L3_SPECPKT_TYPE_L4_MSG_2,
    AAL_L3_SPECPKT_TYPE_HTTP_POST = AAL_L3_SPECPKT_TYPE_L4_MSG_3,
    AAL_L3_SPECPKT_TYPE_HTTP_OPTIONS = AAL_L3_SPECPKT_TYPE_L4_MSG_4,
    AAL_L3_SPECPKT_TYPE_HTTP_DELETE = AAL_L3_SPECPKT_TYPE_L4_MSG_5,
    AAL_L3_SPECPKT_TYPE_HTTP_TRACE = AAL_L3_SPECPKT_TYPE_L4_MSG_6,
    AAL_L3_SPECPKT_TYPE_HTTP_CONNECT = AAL_L3_SPECPKT_TYPE_L4_MSG_7,
    AAL_L3_SPECPKT_TYPE_HTTP_UNLINK = AAL_L3_SPECPKT_TYPE_L4_MSG_8,
    AAL_L3_SPECPKT_TYPE_HTTP_PATCH = AAL_L3_SPECPKT_TYPE_L4_MSG_9,

    /* Other L4 Msgs */
    AAL_L3_SPECPKT_TYPE_MAX = AAL_L3_SPECPKT_TYPE_L4_MSG_15,

    /* Must be last one */
    AAL_L3_SPECPKT_TYPE_INVALID,
} aal_l3_specpkt_type_t;


typedef union {
    ca_uint32_t u32;
    
    struct {
#ifdef CS_BIG_ENDIAN 
    ca_uint32_t  pol_id       : 1;         
    ca_uint32_t  grp_pol_id   : 1; 
    ca_uint32_t  ldpid        : 1; 
    ca_uint32_t  cos          : 1; 
    ca_uint32_t  pol_en_sel   : 1; 
    ca_uint32_t  mrr_en       : 1; 
    ca_uint32_t  mrr_en_valid : 1; 
    ca_uint32_t  no_drop      : 1;
    ca_uint32_t  keep_ts      : 1; 
    ca_uint32_t  spcl_en      : 1; 
    ca_uint32_t  rsvd         : 22;
    
#else /* CS_LITTLE_ENDIAN */
    
    ca_uint32_t  rsvd         : 22;
    ca_uint32_t  spcl_en      : 1; 
    ca_uint32_t  keep_ts      : 1; 
    ca_uint32_t  no_drop      : 1;
    ca_uint32_t  mrr_en_valid : 1; 
    ca_uint32_t  mrr_en       : 1; 
    ca_uint32_t  pol_en_sel   : 1; 
    ca_uint32_t  cos          : 1; 
    ca_uint32_t  ldpid        : 1; 
    ca_uint32_t  grp_pol_id   : 1; 
    ca_uint32_t  pol_id       : 1;         

#endif
    }s;
    
} aal_l3_specpkt_ctrl_mask_t;

typedef enum {
    AAL_L3_SPECPKT_POL_DISABLE = 0,
    AAL_L3_SPECPKT_POL_ENABLE_L2 = 1,
    AAL_L3_SPECPKT_POL_ENABLE_L3 = 2,
    AAL_L3_SPECPKT_POL_ENABLE_L2_L3 = 3,
    AAL_L3_SPECPKT_POL_MAX = AAL_L3_SPECPKT_POL_ENABLE_L2_L3
} aal_l3_specpkt_pol_en_sel_t;

typedef struct {
    ca_boolean_t spcl_en          ;
    ca_uint8_t   ldpid            ;
    ca_uint8_t   cos              ;
    ca_uint8_t   pol_en_sel       ;
    ca_uint16_t  pol_id           ;
    ca_uint8_t   grp_pol_id       ;
    ca_boolean_t keep_ts          ;
    ca_boolean_t no_drop          ;
    ca_boolean_t mrr_en           ;
    ca_boolean_t mrr_en_valid     ;
} aal_l3_specpkt_ctrl_t;

typedef union
{
    ca_uint32_t        u32[2]; /* array */
    struct 
    {
#ifdef CS_BIG_ENDIAN 
    ca_uint32_t  hello_mac  : 1;
    ca_uint32_t  my_mac  : 1;
    ca_uint32_t  rsvd_mc_mac_hi_range  : 1;
    ca_uint32_t  rsvd_mc_mac_low_range  : 1;
    ca_uint32_t  user_def_0_mac  : 1;
    ca_uint32_t  user_def_0_mac_mask  : 1;
    ca_uint32_t  user_def_1_mac  : 1;
    ca_uint32_t  user_def_1_mac_mask  : 1;
    ca_uint32_t  user_def_ethtype_0  : 1;
    ca_uint32_t  user_def_ethtype_1  : 1;
    ca_uint32_t  user_def_ethtype_2  : 1;
    ca_uint32_t  user_def_ethtype_3  : 1;
    ca_uint32_t  user_def_spcl_3_enable  : 1;  
    ca_uint32_t  user_def_spcl_2_enable  : 1;  
    ca_uint32_t  user_def_spcl_1_enable  : 1;  
    ca_uint32_t  user_def_spcl_0_enable  : 1;  
    ca_uint32_t  user_def_1_ethtype_detect_enable  : 1;
    ca_uint32_t  user_def_0_ethtype_detect_enable  : 1;
    ca_uint32_t  detect_depth : 1;
    ca_uint32_t  l7_offset  : 1; 
    ca_uint32_t  efm_oam_subtype  : 1;
    ca_uint32_t  cfm_md_level  : 1; 
    ca_uint32_t  efm_oam_subtype_detect_enable  : 1;
    ca_uint32_t  bpdu_mac_08_enable  : 1;
    ca_uint32_t  l4_chksum_rh_chk_enable  : 1;
    ca_uint32_t  l4_chksum_chk_enable  : 1;
    ca_uint32_t  ipv4_chksum_chk_enable  : 1;
    ca_uint32_t  rdp_v1_detection_enable  : 1;
    ca_uint32_t  l4_spcl_hdr_detct_enable  : 1; 
    ca_uint32_t  l3_spcl_hdr_detct_enable  : 1;
    ca_uint32_t  l2_spcl_hdr_detct_enable  : 1; 
    ca_uint32_t  my_mac_detct_enable  : 1; 
    ca_uint32_t  mpcp_opcode_chk_enable  : 1;  
    ca_uint32_t  check_arp_target_ip_enable  : 1; 
    ca_uint32_t  udp_lite_detct_enable  : 1;
    ca_uint32_t  arp_target_ip        : 1;
    ca_uint32_t  user_define_hi_pri   : 1; 
    ca_uint32_t  dhcpv4_detct_enable  : 1;
    ca_uint32_t  dhcpv6_detct_enable  : 1;
    ca_uint32_t  l4ptp_detct_enable   : 1;
    ca_uint32_t  dns_detct_enable     : 1;
    ca_uint32_t  rip_detct_enable     : 1;
    ca_uint32_t  ssdp_detct_enable    : 1;
    ca_uint32_t  l4_msgs_detct_enable : 1;
    ca_uint32_t  http_link3           : 1;
    ca_uint32_t  http_link2           : 1;
    ca_uint32_t  http_link1           : 1;
    ca_uint32_t  http_link0           : 1;
    ca_uint32_t  rsvd                 : 1;
#else 
    ca_uint32_t  rsvd                 : 1;
    ca_uint32_t  http_link3           : 1;
    ca_uint32_t  http_link2           : 1;
    ca_uint32_t  http_link1           : 1;
    ca_uint32_t  http_link0           : 1;
    ca_uint32_t  l4_msgs_detct_enable : 1;
    ca_uint32_t  ssdp_detct_enable    : 1;
    ca_uint32_t  rip_detct_enable     : 1;
    ca_uint32_t  dns_detct_enable     : 1;
    ca_uint32_t  l4ptp_detct_enable   : 1;
    ca_uint32_t  dhcpv6_detct_enable  : 1;
    ca_uint32_t  dhcpv4_detct_enable  : 1;
    ca_uint32_t  user_define_hi_pri   : 1;  
    ca_uint32_t  arp_target_ip        : 1;
    ca_uint32_t  udp_lite_detct_enable  : 1;
    ca_uint32_t  check_arp_target_ip_enable  : 1; 
    ca_uint32_t  mpcp_opcode_chk_enable  : 1;  
    ca_uint32_t  my_mac_detct_enable  : 1; 
    ca_uint32_t  l2_spcl_hdr_detct_enable  : 1; 
    ca_uint32_t  l3_spcl_hdr_detct_enable  : 1;
    ca_uint32_t  l4_spcl_hdr_detct_enable  : 1; 
    ca_uint32_t  rdp_v1_detection_enable  : 1;
    ca_uint32_t  ipv4_chksum_chk_enable  : 1;
    ca_uint32_t  l4_chksum_chk_enable  : 1;
    ca_uint32_t  l4_chksum_rh_chk_enable  : 1;
    ca_uint32_t  bpdu_mac_08_enable  : 1;
    ca_uint32_t  efm_oam_subtype_detect_enable  : 1;
    ca_uint32_t  cfm_md_level  : 1; 
    ca_uint32_t  efm_oam_subtype  : 1;
    ca_uint32_t  l7_offset  : 1; 
    ca_uint32_t  detect_depth : 1;
    ca_uint32_t  user_def_0_ethtype_detect_enable  : 1;
    ca_uint32_t  user_def_1_ethtype_detect_enable  : 1;
    ca_uint32_t  user_def_spcl_0_enable  : 1;  
    ca_uint32_t  user_def_spcl_1_enable  : 1;  
    ca_uint32_t  user_def_spcl_2_enable  : 1;  
    ca_uint32_t  user_def_spcl_3_enable  : 1;  
    ca_uint32_t  user_def_ethtype_3  : 1;
    ca_uint32_t  user_def_ethtype_2  : 1;
    ca_uint32_t  user_def_ethtype_1  : 1;
    ca_uint32_t  user_def_ethtype_0  : 1;
    ca_uint32_t  user_def_1_mac_mask  : 1;
    ca_uint32_t  user_def_1_mac  : 1;
    ca_uint32_t  user_def_0_mac_mask  : 1;
    ca_uint32_t  user_def_0_mac  : 1;
    ca_uint32_t  rsvd_mc_mac_low_range  : 1;
    ca_uint32_t  rsvd_mc_mac_hi_range  : 1;
    ca_uint32_t  my_mac  : 1;
    ca_uint32_t  hello_mac  : 1;

#endif 
    }s;
    
}aal_l3_specpkt_detect_mask_t;



typedef union {
    ca_uint32_t u32;
    
    struct {
#ifdef CS_BIG_ENDIAN 
    ca_uint32_t  rsvd                          : 12;
    ca_uint32_t  ipv4_hdr_cs_err               : 1;
    ca_uint32_t  pkt_l2_size_err               : 1;
    ca_uint32_t  pkt_l3_total_len_err          : 1;
    ca_uint32_t  ip_hdr_err                    : 1;
    ca_uint32_t  ipv4_total_len_err            : 1;
    ca_uint32_t  tcp_data_offset_err           : 1;
    ca_uint32_t  udp_total_len_err             : 1;
    ca_uint32_t  udp_lite_cs_cvrg_err          : 1;
    ca_uint32_t  rdp_hdr_len_err               : 1;
    ca_uint32_t  offset_l3_excd_255_err        : 1;
    ca_uint32_t  offset_l4_excd_255_err        : 1;
	ca_uint32_t  dsl_inner_ip_hdr_cs_err       : 1;
    ca_uint32_t  pkt_inner_l3_total_len_err    : 1;
    ca_uint32_t  inner_ip_hdr_err              : 1;
    ca_uint32_t  dsl_inner_ipv4_total_len_err  : 1;
    ca_uint32_t  offset_l3_inner_excd_255_err  : 1;
    ca_uint32_t  udp_zero_chksum_err           : 1;
    ca_uint32_t  more_than_2_vlan_tags_flg     : 1;
    ca_uint32_t  l2tp_hdr_err                  : 1;
    ca_uint32_t  rdp_ver_err                   : 1;
    
#else /* CS_LITTLE_ENDIAN */
    ca_uint32_t  rsvd                          : 12;
    ca_uint32_t  rdp_ver_err                   : 1;    
    ca_uint32_t  l2tp_hdr_err                  : 1;
    ca_uint32_t  more_than_2_vlan_tags_flg     : 1;
    ca_uint32_t  udp_zero_chksum_err           : 1;
    ca_uint32_t  offset_l3_inner_excd_255_err  : 1;
    ca_uint32_t  dsl_inner_ipv4_total_len_err  : 1;
    ca_uint32_t  inner_ip_hdr_err              : 1;
    ca_uint32_t  pkt_inner_l3_total_len_err    : 1;
	ca_uint32_t  dsl_inner_ip_hdr_cs_err       : 1;
    ca_uint32_t  offset_l4_excd_255_err        : 1;
    ca_uint32_t  offset_l3_excd_255_err        : 1;
    ca_uint32_t  rdp_hdr_len_err               : 1;
    ca_uint32_t  udp_lite_cs_cvrg_err          : 1;
    ca_uint32_t  udp_total_len_err             : 1;
    ca_uint32_t  tcp_data_offset_err           : 1;
    ca_uint32_t  ipv4_total_len_err            : 1;
    ca_uint32_t  ip_hdr_err                    : 1;
    ca_uint32_t  pkt_l3_total_len_err          : 1;
    ca_uint32_t  pkt_l2_size_err               : 1;
    ca_uint32_t  ipv4_hdr_cs_err               : 1;

#endif
    }s;
    
} aal_l3_specpkt_err_fwd_ctrl_mask_t;

typedef struct {
    ca_uint8_t ipv4_hdr_cs_err              ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t pkt_l2_size_err              ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t pkt_l3_total_len_err         ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t ip_hdr_err                   ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t ipv4_total_len_err           ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t tcp_data_offset_err          ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t udp_total_len_err            ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t udp_lite_cs_cvrg_err         ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t rdp_hdr_len_err              ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t offset_l3_excd_255_err       ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t offset_l4_excd_255_err       ; /* 0: drop, 1: trap, 2: normal forward */
	ca_uint8_t dsl_inner_ip_hdr_cs_err      ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t pkt_inner_l3_total_len_err   ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t inner_ip_hdr_err             ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t dsl_inner_ipv4_total_len_err ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t offset_l3_inner_excd_255_err ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t udp_zero_chksum_err          ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t more_than_2_vlan_tags_flg    ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t l2tp_hdr_err                 ; /* 0: drop, 1: trap, 2: normal forward */
    ca_uint8_t rdp_ver_err                  ; /* 0: drop, 1: trap, 2: normal forward */
} aal_l3_specpkt_err_fwd_ctrl_t;


typedef struct {
    ca_mac_addr_t hello_mac;
    ca_mac_addr_t my_mac;
    ca_mac_addr_t rsvd_mc_mac_hi_range;
    ca_mac_addr_t rsvd_mc_mac_low_range;
    
    ca_mac_addr_t user_def_0_mac;
    ca_mac_addr_t user_def_0_mac_mask;
    ca_mac_addr_t user_def_1_mac;
    ca_mac_addr_t user_def_1_mac_mask;
    ca_uint16_t   user_def_ethtype_0;
    ca_uint16_t   user_def_ethtype_1;
    ca_uint16_t   user_def_ethtype_2;
    ca_uint16_t   user_def_ethtype_3;

    ca_boolean_t  user_def_spcl_3_enable;  
    ca_boolean_t  user_def_spcl_2_enable;  
    ca_boolean_t  user_def_spcl_1_enable;  
    ca_boolean_t  user_def_spcl_0_enable;  
    ca_boolean_t  user_def_1_ethtype_detect_enable;
    ca_boolean_t  user_def_0_ethtype_detect_enable;

    ca_uint8_t    detect_depth; /* MAX packet header parsing depth started from L2 DA */
    ca_uint8_t    l7_offset; /* 0 ~ 0x3F */
    ca_uint8_t    efm_oam_subtype;
    ca_uint8_t    cfm_md_level; /* 0 ~7 */

    ca_boolean_t  efm_oam_subtype_detect_enable;
    ca_boolean_t  bpdu_mac_08_enable;
    ca_boolean_t  l4_chksum_rh_chk_enable;
    ca_boolean_t  l4_chksum_chk_enable;
    ca_boolean_t  ipv4_chksum_chk_enable;
    ca_boolean_t  rdp_v1_detection_enable;
    ca_boolean_t  l4_spcl_hdr_detct_enable; 
    ca_boolean_t  l3_spcl_hdr_detct_enable;
    ca_boolean_t  l2_spcl_hdr_detct_enable; 
    ca_boolean_t  my_mac_detct_enable; 
    
    ca_boolean_t  mpcp_opcode_chk_enable;  
    ca_boolean_t  check_arp_target_ip_enable; 
    ca_boolean_t  udp_lite_detct_enable;
    
    ca_uint32_t   arp_target_ip;
    ca_boolean_t  user_define_hi_pri; 
    
    ca_boolean_t  dhcpv4_detct_enable ;
    ca_boolean_t  dhcpv6_detct_enable ;
    ca_boolean_t  l4ptp_detct_enable  ;
    ca_boolean_t  dns_detct_enable    ;
    ca_boolean_t  rip_detct_enable    ;
    ca_boolean_t  ssdp_detct_enable   ;
    ca_boolean_t  l4_msgs_detct_enable;

    ca_boolean_t  http_link0_detect_enable; 
    ca_boolean_t  http_link0_lspid_valid; /* match source port or not */ 
    ca_uint8_t    http_link0_lspid; /* source port */
    ca_uint8_t    http_link0_l4_type; /* 0 means TCP and UDP, 1 means TCP, 2 means UDP */
    ca_uint8_t    http_link0_l4_port_type; /* 1 means L4 dest port, 0 means L4 src port */
    ca_uint16_t   http_link0_l4_port; /* L4 port */

    ca_boolean_t  http_link1_detect_enable; 
    ca_boolean_t  http_link1_lspid_valid;
    ca_uint8_t    http_link1_lspid; 
    ca_uint8_t    http_link1_l4_type; 
    ca_uint8_t    http_link1_l4_port_type; 
    ca_uint16_t   http_link1_l4_port;

    ca_boolean_t  http_link2_detect_enable; 
    ca_boolean_t  http_link2_lspid_valid;
    ca_uint8_t    http_link2_lspid; 
    ca_uint8_t    http_link2_l4_type; 
    ca_uint8_t    http_link2_l4_port_type;
    ca_uint16_t   http_link2_l4_port;

    ca_boolean_t  http_link3_detect_enable; 
    ca_boolean_t  http_link3_lspid_valid;
    ca_uint8_t    http_link3_lspid; 
    ca_uint8_t    http_link3_l4_type;
    ca_uint8_t    http_link3_l4_port_type;
    ca_uint16_t   http_link3_l4_port;

} aal_l3_specpkt_detect_t;

typedef struct {
    ca_boolean_t pingpong_fifo_overflow;
    ca_boolean_t ni_intf_sop_eop_mismatch;
    ca_boolean_t ni_intf_runt_l2_pkt;
    ca_boolean_t ni_intf_fifo_overflow;

    ca_boolean_t tcp_chksum_err;
    ca_boolean_t udp_chksum_err;
    ca_boolean_t udp_lite_chksum_err;
    ca_boolean_t rdpv1_chksum_err;
    ca_boolean_t rdpv2_chksum_err;

    ca_boolean_t ipv4_hdr_chksum_err;
    ca_boolean_t dsl_inner_ip_hdr_chksum_err;
    ca_boolean_t pkt_l2_size_err;
    ca_boolean_t pkt_l3_total_len_err;
	ca_boolean_t ip_hdr_err;
    ca_boolean_t ipv4_total_len_err;
    ca_boolean_t tcp_data_offset_err;
    ca_boolean_t udp_total_len_err;
    ca_boolean_t udp_lite_cs_cvrg_err;
    ca_boolean_t rdp_hdr_len_err;
    ca_boolean_t pkt_inner_l3_total_len_err;
    ca_boolean_t inner_ip_hdr_err;
    ca_boolean_t dsl_inner_ipv4_total_len_err;
    ca_boolean_t offset_l3_excd_255_err;
    ca_boolean_t offset_l3_inner_excd_255_err;
    ca_boolean_t offset_l4_excd_255_err;
    ca_boolean_t udp_zero_chksum_err;
    ca_boolean_t more_than_2_vlan_tags_flg;
    ca_boolean_t l2tp_header_detection_err;
    ca_boolean_t rdp_ver_err;

} aal_l3_specpkt_paser_status_t;


typedef union {
    ca_uint32_t u32;
    
    struct {
#ifdef CS_BIG_ENDIAN 
   
    ca_uint32_t   keep_orig_pkt : 1;         
    ca_uint32_t   rdir_ldpid    : 1; 
    ca_uint32_t   rdir_cos      : 1; 
    ca_uint32_t   t0_ctrl       : 1; 
    ca_uint32_t   t1_ctrl       : 1; 
    ca_uint32_t   t2_ctrl       : 1; 
    ca_uint32_t   t3_ctrl       : 1; 
    ca_uint32_t   t4_ctrl       : 1; 
    ca_uint32_t   t5_ctrl       : 1; 
    ca_uint32_t   rsvd          : 23;
    
#else /* CS_LITTLE_ENDIAN */
    
    ca_uint32_t   rsvd          : 23;
    ca_uint32_t   t5_ctrl       : 1; 
    ca_uint32_t   t4_ctrl       : 1; 
    ca_uint32_t   t3_ctrl       : 1; 
    ca_uint32_t   t2_ctrl       : 1;
    ca_uint32_t   t1_ctrl       : 1; 
    ca_uint32_t   t0_ctrl       : 1; 
    ca_uint32_t   rdir_cos      : 1; 
    ca_uint32_t   rdir_ldpid    : 1; 
    ca_uint32_t   keep_orig_pkt : 1;          

#endif
    }s;
}aal_l3_specpkt_err_trap_cfg_mask_t;

typedef struct {
    ca_boolean_t keep_orig_pkt;
    ca_uint8_t   rdir_ldpid   ;
    ca_uint8_t   rdir_cos     ;
    ca_uint8_t   t0_ctrl      ;
    ca_uint8_t   t1_ctrl      ;
    ca_uint8_t   t2_ctrl      ;
    ca_uint8_t   t3_ctrl      ;
    ca_uint8_t   t4_ctrl      ;
    ca_uint8_t   t5_ctrl      ;
} aal_l3_specpkt_err_trap_cfg_t;

ca_status_t aal_l3_specpkt_parser_status_get(
    CA_IN  ca_device_id_t device_id,
    CA_OUT aal_l3_specpkt_paser_status_t *stat
);

ca_status_t  aal_l3_specpkt_err_trap_cfg_set(
    CA_IN ca_device_id_t                             device_id,
    CA_IN aal_l3_specpkt_err_trap_cfg_mask_t      mask,
    CA_IN aal_l3_specpkt_err_trap_cfg_t          *config
);

ca_status_t  aal_l3_specpkt_err_trap_cfg_get(
    CA_IN  ca_device_id_t                       device_id,
    CA_OUT aal_l3_specpkt_err_trap_cfg_t    *config
);

ca_status_t  aal_l3_specpkt_err_fwd_ctrl_set(
    CA_IN ca_device_id_t                             device_id,
    CA_IN aal_l3_specpkt_err_fwd_ctrl_mask_t      mask,
    CA_IN aal_l3_specpkt_err_fwd_ctrl_t          *config
);

ca_status_t  aal_l3_specpkt_err_fwd_ctrl_get(
    CA_IN  ca_device_id_t                       device_id,
    CA_OUT aal_l3_specpkt_err_fwd_ctrl_t    *config
);

ca_status_t  aal_l3_specpkt_ctrl_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint32_t                   specpkt_sel, /*LPB special packet select, 0~1 */
    CA_IN aal_l3_specpkt_type_t         pkt_type, /* special packet type, 0~57*/
    CA_IN aal_l3_specpkt_ctrl_mask_t    mask,
    CA_IN aal_l3_specpkt_ctrl_t        *config
);

ca_status_t  aal_l3_specpkt_ctrl_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_uint32_t                 specpkt_sel,/*LPB special packet select, 0~1 */
    CA_IN  aal_l3_specpkt_type_t       pkt_type, /* special packet type, 0~57*/
    CA_OUT aal_l3_specpkt_ctrl_t      *config
);

ca_status_t  aal_l3_specpkt_detect_set(
    CA_IN ca_device_id_t                       device_id,
    CA_IN aal_l3_specpkt_detect_mask_t      mask,
    CA_IN aal_l3_specpkt_detect_t          *config
);

ca_status_t  aal_l3_specpkt_detect_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_l3_specpkt_detect_t    *config
);

ca_status_t  aal_l3_specpkt_init(/*AUTO-CLI gen ignore */
    CA_IN ca_device_id_t device_id);

#endif

