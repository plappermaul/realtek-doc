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
#ifndef __AAL_L2_SPECPKT_H__
#define __AAL_L2_SPECPKT_H__
#include "ca_types.h"

#define AAL_L2_SPECPKT_ETHTYPE_ENTRY_ID_MAX  31
#define AAL_L2_SPECPKT_L7_OFFSET_MAX         127
#define AAL_L2_SPECPKT_TBL_IDX_MAX           7
#define AAL_L2_SPECPKT_ETHTYPE_MIN           0x600
#define AAL_L2_SPECPKT_ETHTYPE_ENCODE_MAX    31

#define AAL_L2_SPECPKT_HTTPLINK_TYPE_BOTH_TCP_UDP 0
#define AAL_L2_SPECPKT_HTTPLINK_TYPE_BOTH_TCP     1
#define AAL_L2_SPECPKT_HTTPLINK_TYPE_BOTH_UDP     2

#define AAL_L2_SPECPKT_HTTPLINK_L4_PORT_TYPE_SRC  0
#define AAL_L2_SPECPKT_HTTPLINK_L4_PORT_TYPE_DST  1

typedef enum {
    AAL_L2_SPECPKT_TYPE_NORMAL = 0x0,
    AAL_L2_SPECPKT_TYPE_IEEE_RSVD_A = 0x1,
    AAL_L2_SPECPKT_TYPE_IEEE_RSVD_B = 0x2,
    AAL_L2_SPECPKT_TYPE_IEEE_RSVD_C = 0x3,
    AAL_L2_SPECPKT_TYPE_IEEE_RSVD_D = 0x4,
    AAL_L2_SPECPKT_TYPE_IEEE_RSVD_GRP = 0x5,
    AAL_L2_SPECPKT_TYPE_HELLO = 0x6,
    AAL_L2_SPECPKT_TYPE_MEF_OAM = 0x7,
    AAL_L2_SPECPKT_TYPE_802_DOT1X = 0x8,
    AAL_L2_SPECPKT_TYPE_EFM_OAM = 0x9,
    AAL_L2_SPECPKT_TYPE_MPCP = 0xA,
    AAL_L2_SPECPKT_TYPE_BPDU = 0xB,
    AAL_L2_SPECPKT_TYPE_MYMAC = 0xC,
    AAL_L2_SPECPKT_TYPE_IGMP_MLD = 0xD,
    AAL_L2_SPECPKT_TYPE_ARP = 0xE,
    AAL_L2_SPECPKT_TYPE_DHCP = 0xF,
    /* Rsvd 0x10, pause frame, debug purpose */
    AAL_L2_SPECPKT_TYPE_PPPOE_DISC = 0x11,
    AAL_L2_SPECPKT_TYPE_PPPOE_SESS = 0x12,
    AAL_L2_SPECPKT_TYPE_NDP = 0x13,
    /* Rsvd 0x14 */
    AAL_L2_SPECPKT_TYPE_UDF_1 = 0x15,
    AAL_L2_SPECPKT_TYPE_UDF_0 = 0x16,
    AAL_L2_SPECPKT_TYPE_SNAP_OTHER = 0x17,
    AAL_L2_SPECPKT_TYPE_LLC_OTHER = 0x18,
    AAL_L2_SPECPKT_TYPE_HTTP_LINK0 = 0x19,
    AAL_L2_SPECPKT_TYPE_HTTP_LINK1 = 0x1A,
    AAL_L2_SPECPKT_TYPE_L2_PTP = 0x1B,
    AAL_L2_SPECPKT_TYPE_PTP_UDP = 0x1C,
    AAL_L2_SPECPKT_TYPE_JUMBO = 0x1D,
    AAL_L2_SPECPKT_TYPE_HTTP_LINK2 = 0x1E,
    AAL_L2_SPECPKT_TYPE_ICMP = 0x1F,

    AAL_L2_SPECPKT_TYPE_MAX = AAL_L2_SPECPKT_TYPE_ICMP,

    /* Must be last one */
    AAL_L2_SPECPKT_TYPE_INVALID,
} aal_l2_specpkt_type_t;


typedef union {
    ca_uint32_t u32;
    
    struct {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t ethertype     : 1;
        ca_uint32_t encoded_value : 1;
        ca_uint32_t rsvd          : 30;
#else /* CS_LITTLE_ENDIAN */
        ca_uint32_t rsvd          : 30;
        ca_uint32_t encoded_value : 1;
        ca_uint32_t ethertype     : 1;
#endif
    }s;
    
} aal_l2_specpkt_ethtype_encode_mask_t;


typedef struct {
    ca_uint16_t ethertype;    /* Ethernet Type, not less than 0x600 */
    ca_uint16_t encoded_value;/* ASIC encoding for above Ethernet Type, 0~31 */
} aal_l2_specpkt_ethtype_encode_t;

typedef union {
    ca_uint32_t u32;
    
    struct {
#ifdef CS_BIG_ENDIAN 
    ca_uint32_t  pkt_no_drop       : 1;
    ca_uint32_t  pkt_no_edit       : 1;
    ca_uint32_t  keep_ts           : 1;
    ca_uint32_t  bypass_sa_lrn_chk : 1;
    ca_uint32_t  limit_fwd_permit  : 1;
    ca_uint32_t  learn_dis         : 1;
    ca_uint32_t  dscp_mark_down    : 1;
    ca_uint32_t  spcl_fwd          : 1;
    ca_uint32_t  rate_limit_bypass : 1;
    ca_uint32_t  flow_en           : 1;
    ca_uint32_t  cos               : 1;
    ca_uint32_t  ldpid             : 1;
    ca_uint32_t  flowid            : 1;
    ca_uint32_t  rsvd              : 19;
    
#else /* CS_LITTLE_ENDIAN */
    
    ca_uint32_t  rsvd              : 19;
    ca_uint32_t  flowid            : 1;
    ca_uint32_t  ldpid             : 1;
    ca_uint32_t  cos               : 1;
    ca_uint32_t  flow_en           : 1;
    ca_uint32_t  rate_limit_bypass : 1;
    ca_uint32_t  spcl_fwd          : 1;
    ca_uint32_t  dscp_mark_down    : 1;
    ca_uint32_t  learn_dis         : 1;
    ca_uint32_t  limit_fwd_permit  : 1;
    ca_uint32_t  bypass_sa_lrn_chk : 1;
    ca_uint32_t  keep_ts           : 1;
    ca_uint32_t  pkt_no_edit       : 1;
    ca_uint32_t  pkt_no_drop       : 1;

#endif
    }s;
    
} aal_l2_specpkt_ctrl_mask_t;


typedef struct {
    ca_boolean_t pkt_no_drop      ; /* when set,the packet would be taken highest priority in L2TM while buffer is almost out */
    ca_boolean_t pkt_no_edit      ; /* when set,the packet will not be modified in L2FE */
    ca_boolean_t keep_ts          ; /* when set,the timestamp for this special packet will be keep */
    ca_boolean_t bypass_sa_lrn_chk;
    ca_boolean_t limit_fwd_permit ;
    ca_boolean_t learn_dis        ;
    ca_boolean_t dscp_mark_down   ;
    ca_boolean_t spcl_fwd         ;
    ca_boolean_t rate_limit_bypass;
    ca_boolean_t flow_en          ;
    ca_uint8_t   cos              ;
    ca_uint8_t   ldpid            ;
    ca_uint16_t  flowid           ;
} aal_l2_specpkt_ctrl_t;

typedef union
{
    ca_uint32_t        u32[2];
    struct 
    {
#ifdef CS_BIG_ENDIAN 
        ca_uint32_t hello_mac : 1;
        ca_uint32_t hello_mac_mask : 1;
        ca_uint32_t udf0_detect_enable : 1;
        ca_uint32_t udf0_mac : 1;
        ca_uint32_t udf0_mac_mask : 1;
        ca_uint32_t udf0_ethtype : 1;
        ca_uint32_t udf0_ethtype_enable : 1;
        ca_uint32_t udf1_detect_enable : 1;
        ca_uint32_t udf1_mac : 1;
        ca_uint32_t udf1_mac_mask : 1;
        ca_uint32_t udf1_vlan : 1;
        ca_uint32_t udf1_vlan_enable : 1;
        ca_uint32_t my_mac : 1;
        ca_uint32_t my_mac_mask : 1;
        ca_uint32_t my_mac1 : 1;
        ca_uint32_t my_mac1_mask : 1;
        ca_uint32_t rsvd_mc_mac : 1;
        ca_uint32_t rsvd_mc_mac_mask : 1;
        ca_uint32_t rsvd_grp_mac : 1;
        ca_uint32_t rsvd_grp_mac_mask : 1;
        ca_uint32_t mef0_mac : 1;
        ca_uint32_t mef0_ethtype : 1;
        ca_uint32_t mef1_mac : 1;
        ca_uint32_t mef1_ethtype : 1;
        ca_uint32_t l7_offset : 1;
        ca_uint32_t efm_oam_subtype_detect_enable : 1;
        ca_uint32_t efm_oam_subtype : 1;
        ca_uint32_t ipv4_checksum_check_enable : 1;
        ca_uint32_t egr_ptp_pkt_no_drop_set : 1;
        ca_uint32_t l3fe_to_l2fe_lrn_ctrl : 1;
        ca_uint32_t use_ip_addr_detect_mc : 1; 
        ca_uint32_t http_link0 : 1;
        ca_uint32_t http_link1 : 1;
        ca_uint32_t http_link2 : 1;
        ca_uint32_t rsvd : 30;
#else 
        ca_uint32_t rsvd : 30;
        ca_uint32_t http_link2 : 1;
        ca_uint32_t http_link1 : 1;
        ca_uint32_t http_link0 : 1;
        ca_uint32_t use_ip_addr_detect_mc : 1;
        ca_uint32_t l3fe_to_l2fe_lrn_ctrl : 1;
        ca_uint32_t egr_ptp_pkt_no_drop_set : 1;
        ca_uint32_t ipv4_checksum_check_enable : 1;
        ca_uint32_t efm_oam_subtype : 1;
        ca_uint32_t efm_oam_subtype_detect_enable : 1;
        ca_uint32_t l7_offset : 1;
        ca_uint32_t mef1_ethtype : 1;
        ca_uint32_t mef1_mac : 1;
        ca_uint32_t mef0_ethtype : 1;
        ca_uint32_t mef0_mac : 1;
        ca_uint32_t rsvd_grp_mac_mask : 1;
        ca_uint32_t rsvd_grp_mac : 1;
        ca_uint32_t rsvd_mc_mac_mask : 1;
        ca_uint32_t rsvd_mc_mac : 1;
        ca_uint32_t my_mac_mask : 1;
        ca_uint32_t my_mac : 1;
        ca_uint32_t my_mac1_mask : 1;
        ca_uint32_t my_mac1 : 1;
        ca_uint32_t udf1_vlan_enable : 1;
        ca_uint32_t udf1_vlan : 1;
        ca_uint32_t udf1_mac_mask : 1;
        ca_uint32_t udf1_mac : 1;
        ca_uint32_t udf1_detect_enable : 1;
        ca_uint32_t udf0_ethtype_enable : 1;
        ca_uint32_t udf0_ethtype : 1;
        ca_uint32_t udf0_mac_mask : 1;
        ca_uint32_t udf0_mac : 1;
        ca_uint32_t udf0_detect_enable : 1;
        ca_uint32_t hello_mac_mask : 1;
        ca_uint32_t hello_mac : 1;
#endif 
    }s;
    
}aal_l2_specpkt_detect_mask_t;

typedef struct {
    ca_mac_addr_t hello_mac;
    ca_mac_addr_t hello_mac_mask;
    ca_boolean_t  udf0_detect_enable; /* Enable the ethernet type detection for User defined packet type 0 */
    ca_mac_addr_t udf0_mac;
    ca_mac_addr_t udf0_mac_mask;
    ca_uint16_t   udf0_ethtype;
    ca_boolean_t  udf0_ethtype_enable;
    ca_boolean_t  udf1_detect_enable; /* Enable User defined packet type 1 detection */
    ca_mac_addr_t udf1_mac;
    ca_mac_addr_t udf1_mac_mask;
    ca_uint16_t   udf1_vlan;
    ca_boolean_t  udf1_vlan_enable; /* Enable the VLAN detection for User defined packet type 1 */
    ca_mac_addr_t my_mac; /* My MAC#0 */
    ca_mac_addr_t my_mac_mask;
    ca_mac_addr_t my_mac1; /* My MAC#1*/
    ca_mac_addr_t my_mac1_mask;
    ca_mac_addr_t rsvd_mc_mac;
    ca_mac_addr_t rsvd_mc_mac_mask;
    ca_mac_addr_t rsvd_grp_mac;
    ca_mac_addr_t rsvd_grp_mac_mask;
    ca_mac_addr_t mef0_mac;
    ca_mac_addr_t mef0_mac_mask;
    ca_uint16_t   mef0_ethtype;
    ca_mac_addr_t mef1_mac;
    ca_mac_addr_t mef1_mac_mask;
    ca_uint16_t   mef1_ethtype;
    ca_uint8_t    l7_offset; /* start offset used to exact 4 bytes data from packet, counting from the start of L2 DA */
    ca_uint8_t    efm_oam_subtype; /* OAM Subtype */
    ca_boolean_t  efm_oam_subtype_detect_enable; /* enable detect OAM packet need to match the efm_oam_subtype */
    ca_boolean_t  ipv4_checksum_check_enable;/* enable ipv4_checksum_check for NI to FE incoming traffic */
    ca_boolean_t  egr_ptp_pkt_no_drop_set; /* enable set no_drop indicator for egress PTP bypass packet  */
    ca_boolean_t  l3fe_to_l2fe_lrn_ctrl; /* enable the learning control from L3FE to L2FE's HDR_A */
    ca_boolean_t  use_ip_addr_detect_mc; /* enable to use the IP_ADDR to determine the IP MC */   

    ca_boolean_t  http_link0_detect_enable; 
    ca_boolean_t  http_link0_lspid_valid; /* 1 means match source port */ 
    ca_uint8_t    http_link0_lspid; 
    ca_uint8_t    http_link0_l4_type; /* 0 means TCP and UDP, 1 means TCP, 2 means UDP */
    ca_uint8_t    http_link0_l4_port_type; /* 1 means L4 dest port, 0 means L4 src port */
    ca_uint16_t   http_link0_l4_port;

    ca_boolean_t  http_link1_detect_enable; 
    ca_boolean_t  http_link1_lspid_valid; /* 1 means match source port */ 
    ca_uint8_t    http_link1_lspid; 
    ca_uint8_t    http_link1_l4_type; /* 0 means TCP and UDP, 1 means TCP, 2 means UDP */
    ca_uint8_t    http_link1_l4_port_type; /* 1 means L4 dest port, 0 means L4 src port */
    ca_uint16_t   http_link1_l4_port;

    ca_boolean_t  http_link2_detect_enable; 
    ca_boolean_t  http_link2_lspid_valid; /* 1 means match source port */ 
    ca_uint8_t    http_link2_lspid; 
    ca_uint8_t    http_link2_l4_type; /* 0 means TCP and UDP, 1 means TCP, 2 means UDP */
    ca_uint8_t    http_link2_l4_port_type; /* 1 means L4 dest port, 0 means L4 src port */
    ca_uint16_t   http_link2_l4_port;
} aal_l2_specpkt_detect_t;


ca_status_t  aal_l2_specpkt_ethtype_encode_set(
    CA_IN ca_device_id_t                            device_id,
    CA_IN ca_uint32_t                            entry_id,
    CA_IN aal_l2_specpkt_ethtype_encode_mask_t      mask,
    CA_IN aal_l2_specpkt_ethtype_encode_t          *config
);

ca_status_t  aal_l2_specpkt_ethtype_encode_get(
    CA_IN  ca_device_id_t                      device_id,
    CA_IN  ca_uint32_t                      entry_id,
    CA_OUT aal_l2_specpkt_ethtype_encode_t    *config
);

ca_status_t  aal_l2_specpkt_current_capture_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_l2_specpkt_type_t      *pkt_type
);

ca_status_t  aal_l2_specpkt_detect_set(
    CA_IN ca_device_id_t                       device_id,
    CA_IN aal_l2_specpkt_detect_mask_t      mask,
    CA_IN aal_l2_specpkt_detect_t          *config
);

ca_status_t  aal_l2_specpkt_detect_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_l2_specpkt_detect_t    *config
);

ca_status_t  aal_l2_specpkt_ctrl_set(
    CA_IN ca_device_id_t                   device_id,
    CA_IN ca_uint32_t                   spb_idx, 
    CA_IN aal_l2_specpkt_type_t         pkt_type,
    CA_IN aal_l2_specpkt_ctrl_mask_t    mask,
    CA_IN aal_l2_specpkt_ctrl_t        *config
);

ca_status_t  aal_l2_specpkt_ctrl_get(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                spb_idx, 
    CA_IN  aal_l2_specpkt_type_t      pkt_type,
    CA_OUT aal_l2_specpkt_ctrl_t     *config
);

ca_status_t  aal_l2_specpkt_init(/*AUTO-CLI gen ignore */
    CA_IN ca_device_id_t device_id);

ca_status_t aal_l2_specpkt_match_hello_packet( /*AUTO-CLI gen ignore */
   CA_IN ca_mac_addr_t da_mac_addr);

#endif

