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
 * aal_l2_cls.h: Hardware Abstraction Layer for vlan driver to access hardware regsiters
 *
 */
#ifndef __AAL_L2_CLS_H__
#define __AAL_L2_CLS_H__
#include "ca_types.h"
#include "aal_l2_vlan.h"

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define __INGRESS_CLS_ENTRY_NUM_PER_PORT 10
#define __EGRESS_CLS_ENTRY_NUM_PER_PORT  10
#elif defined CONFIG_ARCH_CORTINA_SATURN_SFU
#define __INGRESS_CLS_ENTRY_NUM_PER_PORT 20
#define __EGRESS_CLS_ENTRY_NUM_PER_PORT  20
#else
#define __INGRESS_CLS_ENTRY_NUM_PER_PORT 16
#define __EGRESS_CLS_ENTRY_NUM_PER_PORT  16
#endif

#if defined(CONFIG_ARCH_CORTINA_G3LITE)
#define AAL_L2_CLS_VLAN_ENTRY_NUMBER        4
#define AAL_L2_CLS_MAC_FILTER_ENTRY_NUMBER  3
#define AAL_L2_CLS_RESERVE_ENTRY_NUMBER     3

#elif defined CONFIG_ARCH_CORTINA_SATURN_SFU
#define AAL_L2_CLS_VLAN_ENTRY_NUMBER        6
#define AAL_L2_CLS_MAC_FILTER_ENTRY_NUMBER  4
#define AAL_L2_CLS_RESERVE_ENTRY_NUMBER     10
#else

#define AAL_L2_CLS_VLAN_ENTRY_NUMBER        6
#define AAL_L2_CLS_MAC_FILTER_ENTRY_NUMBER  4
#define AAL_L2_CLS_RESERVE_ENTRY_NUMBER     6

#endif

typedef enum {
    AAL_L2_CLS_MAC_FIELD_CTRL_ALL_MATCH = 0,
    AAL_L2_CLS_MAC_FIELD_CTRL_DA_RANGE = 1,
    AAL_L2_CLS_MAC_FIELD_CTRL_SA_RANGE = 2,
    AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC = 3,
    AAL_L2_CLS_MAC_FIELD_CTRL_MAX = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC
} aal_l2_cls_mac_field_ctrl_t;


typedef enum {
    AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY = 0,
    AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY = 1,
    AAL_L2_CLS_KEY_ENTRY_TYPE_16RULE1ENTRY = 2,
    AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE2ENTRY = 3,
    AAL_L2_CLS_KEY_ENTRY_TYPE_MAX = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE2ENTRY
} aal_l2_cls_key_entry_type_t;

typedef enum {
    AAL_L2_CLS_KEY_2RULE_TYPE_INVALID = 0,
    AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE = 1,
    AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE = 2,
    AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE = 3,
    AAL_L2_CLS_KEY_2RULE_TYPE_MAX = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE
} aal_l2_cls_key_2rule_type_t;


typedef enum{
    AAL_L2_CLS_RULE_TYPE_IPV4           = 0,
    AAL_L2_CLS_RULE_TYPE_IPV6_SHORT     = 1,
    AAL_L2_CLS_RULE_TYPE_IPV6_LONG      = 2,
    AAL_L2_CLS_RULE_TYPE_L2RL2R         = 3,
    AAL_L2_CLS_RULE_TYPE_L2RL3R         = 4,
    AAL_L2_CLS_RULE_TYPE_L2RCMR         = 5,
    AAL_L2_CLS_RULE_TYPE_L3RL2R         = 6,
    AAL_L2_CLS_RULE_TYPE_L3RL3R         = 7,
    AAL_L2_CLS_RULE_TYPE_L3RCMR         = 8,
    AAL_L2_CLS_RULE_TYPE_CMRL2R         = 9,
    AAL_L2_CLS_RULE_TYPE_CMRL3R         = 10,
    AAL_L2_CLS_RULE_TYPE_CMRCMR         = 11,
    AAL_L2_CLS_RULE_TYPE_L2R1           = 12,
    AAL_L2_CLS_RULE_TYPE_L2R2           = 13,
    AAL_L2_CLS_RULE_TYPE_L3R1           = 14,
    AAL_L2_CLS_RULE_TYPE_L3R2           = 15,
    AAL_L2_CLS_RULE_TYPE_CMR1           = 16,
    AAL_L2_CLS_RULE_TYPE_CMR2           = 17,
    AAL_L2_CLS_RULE_TYPE_MAX            = AAL_L2_CLS_RULE_TYPE_CMR2
}aal_l2_cls_rule_type_t;

typedef enum{
    AAL_L2_CLS_IP_CTRL_ALL_MATCH       = 0 ,
    AAL_L2_CLS_IP_CTRL_DIP_RANGE       = 1 ,
    AAL_L2_CLS_IP_CTRL_SIP_RANGE       = 2 ,
    AAL_L2_CLS_IP_CTRL_MASK_IP_ADDR    = 3 ,
    AAL_L2_CLS_IP_CTRL_COMPARE_DIP_SIP = 4 ,
    AAL_L2_CLS_IP_CTRL_COMPARE_DIP     = 5 ,
    AAL_L2_CLS_IP_CTRL_MAX             = AAL_L2_CLS_IP_CTRL_COMPARE_DIP
}aal_l2_cls_ip_ctrl_t;

typedef enum{
    AAL_L2_CLS_L4_DST_PORT_FIELD_MASK      = 0 ,
    AAL_L2_CLS_L4_DST_PORT_FIELD_USE_HIGH  = 1 ,
    AAL_L2_CLS_L4_DST_PORT_FIELD_IS_RANGE  = 2 ,
    AAL_L2_CLS_L4_DST_PORT_FIELD_INVALID   = 3 ,
    AAL_L2_CLS_L4_DST_PORT_FIELD_MAX             = AAL_L2_CLS_L4_DST_PORT_FIELD_INVALID
}aal_l2_cls_l4_dst_port_field_ctrl_t;

typedef enum{
    AAL_L2_CLS_L4_SRC_PORT_FIELD_MASK      = 0 ,
    AAL_L2_CLS_L4_SRC_PORT_FIELD_USE_HIGH  = 1 ,
    AAL_L2_CLS_L4_SRC_PORT_FIELD_IS_RANGE  = 2 ,
    AAL_L2_CLS_L4_SRC_PORT_FIELD_INVALID   = 3 ,
    AAL_L2_CLS_L4_SRC_PORT_FIELD_MAX             = AAL_L2_CLS_L4_DST_PORT_FIELD_INVALID
}aal_l2_cls_l4_src_port_field_ctrl_t;

typedef enum{
    AAL_L2_CLS_L7_CTRL_CMPR_AFTER_TCPUDP_HEADER     = 0 ,
    AAL_L2_CLS_L7_CTRL_CMPR_AFTER_IP_HEADER         = 1 ,
    AAL_L2_CLS_L7_CTRL_CMPR_AFTER_PROGRAMABLE_4B    = 2 ,
    AAL_L2_CLS_L7_CTRL_MASK                         = 3 ,
    AAL_L2_CLS_L7_CTRL_MAX                          = AAL_L2_CLS_L7_CTRL_MASK
}aal_l2_cls_l7_ctrl_t;

typedef enum {
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_ALL_MATCH = 0,
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_DP_RANGE = 1,
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_SP_RANGE = 2,
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT = 3,
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_MAX =
    AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT
} aal_l2_cls_l4_port_field_ctrl_t;


                                                                                 
typedef enum {                                                                   
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_ALL_MATCH = 0,                             
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_DP_RANGE = 1,                              
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_SP_RANGE = 2,                              
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_MASK_L4PORT = 3,                           
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_4B_NEXT_ETHTYPE = 4,
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_4B_NEXT_IPHEAD  = 5,
     AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_MAX =                                      
    AAL_L2_CLS_CMR_L4_PORT_FIELD_CTRL_4B_NEXT_IPHEAD                                    
} aal_l2_cls_cmr_l4_port_field_ctrl_t;                                           

typedef enum {
    AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH = 0,
    AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE = 1,
    AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE = 2,
    AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN = 3,
    AAL_L2_CLS_VLAN_FIELD_CTRL_MAX = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN
} aal_l2_cls_vlan_field_ctrl_t;

typedef enum {
    AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH = 0,
    AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE = 1,
    AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE = 2,
    AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P = 3,
    AAL_L2_CLS_DOT1P_FIELD_CTRL_MAX = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P
} aal_l2_cls_dot1p_field_ctrl_t;

typedef enum {
    AAL_L2_CLS_VLAN_NUM_UNTAG = 0,
    AAL_L2_CLS_VLAN_NUM_SINGLE_TAG = 1,
    AAL_L2_CLS_VLAN_NUM_DOUBLE_TAG = 2,
    AAL_L2_CLS_VLAN_NUM_TWO_MORE_TAG = 3,
    AAL_L2_CLS_VLAN_NUM_MAX = AAL_L2_CLS_VLAN_NUM_TWO_MORE_TAG
} aal_l2_cls_vlan_number_t;

typedef struct{
    ca_boolean_t debug_flag;
}aal_l2_cls_debug_flag_t;

typedef union{
    struct{
        ca_uint32_t    valid        : 1;
        ca_uint32_t    entry_type   : 1;
        ca_uint32_t    is_ipv6_rule : 1;
        ca_uint32_t    rule_0       : 1;
        ca_uint32_t    rule_1       : 1;   
    }s;
    ca_uint32_t u32; 
}aal_l2_cls_entry_status_mask_t;


typedef struct{
    ca_boolean_t                    valid       ;
    aal_l2_cls_key_entry_type_t     entry_type  ;
    ca_boolean_t                    is_ipv6_rule;
    aal_l2_cls_key_2rule_type_t     rule_0      ;
    aal_l2_cls_key_2rule_type_t     rule_1      ;    
}aal_l2_cls_entry_status_t;


typedef struct{   
    ca_boolean_t                        valid;
    ca_boolean_t                        is_ipv4_ipv6_key;
    ca_uint32_t                          ipv4_ecn;
    ca_boolean_t                        ipv4_ecn_vld;
    ca_mac_addr_t                       mac_da;                  
    ca_mac_addr_t                       mac_sa;                  
    aal_l2_cls_mac_field_ctrl_t         mac_field_ctrl;          
    ca_uint32_t                         top_vlan_id;             
    ca_boolean_t                        top_is_svlan;            
    ca_uint32_t                         inner_vlan_id;           
    ca_boolean_t                        inner_is_svlan;          
    aal_l2_cls_vlan_field_ctrl_t        vlan_field_ctrl;         
    ca_uint32_t                          top_dot1p;               
    ca_uint32_t                          inner_dot1p;             
    aal_l2_cls_dot1p_field_ctrl_t       dot1p_ctrl;              
    aal_l2_cls_vlan_number_t            vlan_number;             
    ca_boolean_t                        vlan_number_vld;         
    ca_uint32_t                          ethernet_type;           
    ca_boolean_t                        ethernet_type_vld;       
    ca_uint32_t                         ip_da;                   
    ca_uint32_t                          ip_da_mask;              
    ca_uint32_t                         ip_sa;                   
    ca_uint32_t                          ip_sa_mask;              
    aal_l2_cls_ip_ctrl_t                ip_ctrl;                 
    ca_uint32_t                          ip_pt;                   
    ca_boolean_t                        ip_pt_valid;             
    ca_uint32_t                          dscp;                    
    ca_boolean_t                        dscp_valid;              
    ca_boolean_t                        ip_option;               
    ca_boolean_t                        ip_option_valid;         
    ca_boolean_t                        ip_fragment;             
    ca_boolean_t                        ip_fragment_valid;       
    ca_uint32_t                         l4_dst_port_hi;          
    ca_uint32_t                         l4_dst_port_lo;          
    aal_l2_cls_l4_dst_port_field_ctrl_t l4_dst_ctrl;             
    ca_uint32_t                         l4_src_port_hi;          
    ca_uint32_t                         l4_src_port_lo;          
    aal_l2_cls_l4_src_port_field_ctrl_t l4_src_ctrl;
    ca_uint32_t                            l4_flag;
    ca_boolean_t                        l4_flag_valid;
    ca_uint32_t                          special_pkt_code;        
    ca_boolean_t                        special_pkt_code_valid;  
    ca_uint32_t                         global_key;              
    ca_uint32_t                          global_key_mask;              
    aal_l2_cls_l7_ctrl_t                l7_ctrl;                 
    ca_boolean_t                        fields_is_or;            
    ca_uint32_t                          precedence;              
} aal_l2_cls_ipv4_key_t;                                    

typedef union{                                   
struct{   
        ca_uint64_t valid                  : 1;  
        ca_uint64_t is_ipv4_ipv6_key       : 1;
        ca_uint64_t ipv4_ecn               : 1;  
        ca_uint64_t ipv4_ecn_vld           : 1;  
        ca_uint64_t mac_da                 : 1;  
        ca_uint64_t mac_sa                 : 1;  
        ca_uint64_t mac_field_ctrl         : 1;  
        ca_uint64_t top_vlan_id            : 1;  
        ca_uint64_t top_is_svlan           : 1;  
        ca_uint64_t inner_vlan_id          : 1;  
        ca_uint64_t inner_is_svlan         : 1;  
        ca_uint64_t vlan_field_ctrl        : 1;  
        ca_uint64_t top_dot1p              : 1;  
        ca_uint64_t inner_dot1p            : 1;  
        ca_uint64_t dot1p_ctrl             : 1;  
        ca_uint64_t vlan_number            : 1;  
        ca_uint64_t vlan_number_vld        : 1;  
        ca_uint64_t ethernet_type          : 1;  
        ca_uint64_t ethernet_type_vld      : 1;  
        ca_uint64_t ip_da                  : 1;  
        ca_uint64_t ip_da_mask             : 1;  
        ca_uint64_t ip_sa                  : 1;  
        ca_uint64_t ip_sa_mask             : 1;  
        ca_uint64_t ip_ctrl                : 1;  
        ca_uint64_t ip_pt                  : 1;  
        ca_uint64_t ip_pt_valid            : 1;  
        ca_uint64_t dscp                   : 1;  
        ca_uint64_t dscp_valid             : 1;  
        ca_uint64_t ip_option              : 1;  
        ca_uint64_t ip_option_valid        : 1;  
        ca_uint64_t ip_fragment            : 1;  
        ca_uint64_t ip_fragment_valid      : 1;  
        ca_uint64_t l4_dst_port_hi         : 1;  
        ca_uint64_t l4_dst_port_lo         : 1;  
        ca_uint64_t l4_dst_ctrl            : 1;  
        ca_uint64_t l4_src_port_hi         : 1;  
        ca_uint64_t l4_src_port_lo         : 1;  
        ca_uint64_t l4_src_ctrl            : 1; 
        ca_uint64_t l4_flag                : 1;  
        ca_uint64_t l4_flag_valid          : 1;         
        ca_uint64_t special_pkt_code       : 1;  
        ca_uint64_t special_pkt_code_valid : 1;  
        ca_uint64_t global_key             : 1;  
        ca_uint64_t global_key_mask        : 1;  
        ca_uint64_t l7_ctrl                : 1;  
        ca_uint64_t fields_is_or           : 1;  
        ca_uint64_t precedence             : 1;  
    }s;                                          
    ca_uint64_t u64;                             
} aal_l2_cls_ipv4_key_mask_t;  

typedef enum{
    AAL_L2_CLS_IPV6_CTRL_ALL_MATCH       = 0 ,
    AAL_L2_CLS_IPV6_CTRL_DIP_RANGE       = 1 ,
    AAL_L2_CLS_IPV6_CTRL_SIP_RANGE       = 2 ,
    AAL_L2_CLS_IPV6_CTRL_MASK_IP_ADDR    = 3 ,
    AAL_L2_CLS_IPV6_CTRL_MAX               = AAL_L2_CLS_IPV6_CTRL_MASK_IP_ADDR
}aal_l2_cls_ipv6_ctrl_t;


typedef struct{   
    ca_boolean_t                         valid    ;
    ca_boolean_t                        is_ipv4_ipv6_key;
    ca_uint32_t                           ipv6_ecn;
    ca_boolean_t                         ipv6_ecn_valid;
    ca_uint32_t                         ipv6_da[4];                  
    ca_uint32_t                         ipv6_sa[4];
    ca_uint32_t                         ipv6_addr_mask;
    aal_l2_cls_ipv6_ctrl_t              ipv6_ctrl;   
    ca_uint32_t                          ipv6_pt;
    ca_boolean_t                        ipv6_pt_valid;
    ca_uint32_t                          dscp;    
    ca_boolean_t                        dscp_valid;              
    ca_uint32_t                          l4_flag;
    ca_boolean_t                        l4_flag_valid;
    ca_uint32_t                          special_pkt_code;        
    ca_boolean_t                        special_pkt_code_valid;   
    ca_uint32_t                         top_vlan_id;             
    ca_boolean_t                        top_is_svlan; 
    ca_boolean_t                        top_vlan_valid; 
    ca_uint32_t                          top_dot1p;   
    ca_boolean_t                        top_dot1p_valid; 
    aal_l2_cls_vlan_number_t            vlan_number;             
    ca_boolean_t                        vlan_number_vld;    
    ca_uint32_t                         l4_dst_port;
    ca_uint32_t                         l4_src_port;    
    aal_l2_cls_l4_port_field_ctrl_t     l4_ctrl;
    ca_uint32_t                         flow_lab;
    ca_boolean_t                        flow_lab_valid;
    ca_boolean_t                        fields_is_or;            
    ca_uint32_t                          precedence;              
} aal_l2_cls_ipv6_short_key_t;  


typedef union{                                             
    struct{                                                
        ca_uint32_t      valid                  : 1; 
        ca_uint32_t     is_ipv4_ipv6_key        : 1;
        ca_uint32_t     ipv6_ecn                : 1;
        ca_uint32_t     ipv6_ecn_valid          : 1;
        ca_uint32_t     ipv6_da                 : 1;       
        ca_uint32_t     ipv6_sa                 : 1;       
        ca_uint32_t     ipv6_addr_mask          : 1;       
        ca_uint32_t     ipv6_ctrl               : 1;       
        ca_uint32_t     ipv6_pt                 : 1;       
        ca_uint32_t     ipv6_pt_valid           : 1;       
        ca_uint32_t     dscp                    : 1;       
        ca_uint32_t     dscp_valid              : 1;       
        ca_uint32_t     l4_flag                 : 1;       
        ca_uint32_t     l4_flag_valid           : 1;       
        ca_uint32_t     special_pkt_code        : 1;       
        ca_uint32_t     special_pkt_code_valid  : 1;       
        ca_uint32_t     top_vlan_id             : 1;       
        ca_uint32_t     top_is_svlan            : 1;       
        ca_uint32_t     top_vlan_valid          : 1;       
        ca_uint32_t     top_dot1p               : 1;       
        ca_uint32_t     top_dot1p_valid         : 1;       
        ca_uint32_t     vlan_number             : 1;       
        ca_uint32_t     vlan_number_vld         : 1;       
        ca_uint32_t     l4_dst_port             : 1;       
        ca_uint32_t     l4_src_port             : 1;       
        ca_uint32_t     l4_ctrl                 : 1;       
        ca_uint32_t     flow_lab                : 1;       
        ca_uint32_t     flow_lab_valid          : 1;       
        ca_uint32_t     fields_is_or            : 1;       
        ca_uint32_t     precedence              : 1;       
    }s;                                                    
    ca_uint32_t                   u32;                     
} aal_l2_cls_ipv6_short_key_mask_t;   


typedef struct {
    aal_l2_cls_ipv4_key_t  entry_0;
    aal_l2_cls_ipv6_short_key_t  entry_1;
}aal_l2_cls_ipv6_long_key_t;

typedef struct {
    aal_l2_cls_ipv4_key_mask_t  entry_0;
    aal_l2_cls_ipv6_short_key_mask_t  entry_1;
}aal_l2_cls_ipv6_long_key_mask_t;

typedef union{
    struct{
         ca_uint32_t  mac_da             : 1  ;
         ca_uint32_t  mac_sa             : 1  ;
         ca_uint32_t  mac_field_ctrl     : 1  ;
         ca_uint32_t  top_vlan_id        : 1  ;
         ca_uint32_t  top_is_svlan       : 1  ;
         ca_uint32_t  inner_vlan_id      : 1  ;
         ca_uint32_t  inner_is_svlan     : 1  ;
         ca_uint32_t  vlan_field_ctrl    : 1  ;
         ca_uint32_t  top_dot1p          : 1  ;
         ca_uint32_t  inner_dot1p        : 1  ;
         ca_uint32_t  dot1p_ctrl         : 1  ;
         ca_uint32_t  vlan_number        : 1  ;
         ca_uint32_t  vlan_number_vld    : 1  ;
         ca_uint32_t  ethernet_type      : 1  ;
         ca_uint32_t  ethernet_type_vld  : 1  ;
         ca_uint32_t  l4_src_port        : 1  ;
         ca_uint32_t  l4_dst_port        : 1  ;
         ca_uint32_t  l4_port_field_ctrl : 1  ;
         ca_uint32_t  field_is_or        : 1  ;
         ca_uint32_t  precedence         : 1  ;

    }s;
    ca_uint32_t u32;
}aal_l2_cls_l2r_key_mask_t;


typedef struct {
    ca_mac_addr_t                   mac_da;
    ca_mac_addr_t                   mac_sa;
    aal_l2_cls_mac_field_ctrl_t     mac_field_ctrl;
    ca_uint32_t                     top_vlan_id;
    ca_boolean_t                    top_is_svlan;
    ca_uint32_t                     inner_vlan_id;
    ca_boolean_t                    inner_is_svlan;
    aal_l2_cls_vlan_field_ctrl_t    vlan_field_ctrl;
    ca_uint32_t                      top_dot1p;
    ca_uint32_t                      inner_dot1p;
    aal_l2_cls_dot1p_field_ctrl_t   dot1p_ctrl;
    aal_l2_cls_vlan_number_t        vlan_number;
    ca_boolean_t                    vlan_number_vld;
    ca_uint32_t                      ethernet_type;
    ca_boolean_t                    ethernet_type_vld;
    ca_uint32_t                     l4_src_port;
    ca_uint32_t                     l4_dst_port;
    aal_l2_cls_l4_port_field_ctrl_t l4_port_field_ctrl;
    ca_boolean_t                    field_is_or;
    ca_uint32_t                      precedence;
} __attribute__((packed)) aal_l2_cls_l2r_key_t;

typedef union{
    struct {                                                              
        ca_uint32_t                     ecn  : 1;             
        ca_uint32_t               ecn_valid  : 1;             
        ca_uint32_t                mac_addr  : 1;             
        ca_uint32_t          mac_addr_is_sa  : 1;             
        ca_uint32_t          mac_addr_valid  : 1;             
        ca_uint32_t                 ip_addr  : 1;             
        ca_uint32_t           ip_addr_is_sa  : 1;             
        ca_uint32_t           ip_addr_valid  : 1;             
        ca_uint32_t                   ip_pt  : 1;             
        ca_uint32_t             ip_pt_valid  : 1;             
        ca_uint32_t                    dscp  : 1;             
        ca_uint32_t              dscp_valid  : 1;             
        ca_uint32_t                has_ipop  : 1;             
        ca_uint32_t          has_ipop_valid  : 1;             
        ca_uint32_t                 l4_flag  : 1;             
        ca_uint32_t           l4_flag_valid  : 1;             
        ca_uint32_t           ipv4_fragment  : 1;             
        ca_uint32_t     ipv4_fragment_valid  : 1;             
        ca_uint32_t        special_pkt_code  : 1;             
        ca_uint32_t  special_pkt_code_valid  : 1;             
        ca_uint32_t             top_vlan_id  : 1;             
        ca_uint32_t            top_is_svlan  : 1;             
        ca_uint32_t        vlan_field_valid  : 1;             
        ca_uint32_t               top_dot1p  : 1;             
        ca_uint32_t             dot1p_valid  : 1;             
        ca_uint32_t             vlan_number  : 1;             
        ca_uint32_t         vlan_number_vld  : 1;             
        ca_uint32_t           ethernet_type  : 1;             
        ca_uint32_t       ethernet_type_vld  : 1;             
        ca_uint32_t             l4_src_port  : 1;             
        ca_uint32_t             l4_dst_port  : 1;             
        ca_uint32_t      l4_port_field_ctrl  : 1;             
        ca_uint32_t             field_is_or  : 1;             
        ca_uint32_t              precedence  : 1;             
    }s;
    ca_uint32_t u32;
} aal_l2_cls_cmr_key_mask_t;                                 
 

typedef struct {
    ca_uint8_t                                              ecn ;
    ca_boolean_t                                      ecn_valid ;
    ca_mac_addr_t                                     mac_addr  ;             
    ca_boolean_t                                mac_addr_is_sa  ;             
    ca_boolean_t                                mac_addr_valid  ;             
    ca_uint32_t                                        ip_addr  ;             
    ca_boolean_t                                 ip_addr_is_sa  ;             
    ca_boolean_t                                 ip_addr_valid  ;             
    ca_uint32_t                                           ip_pt  ;             
    ca_boolean_t                                   ip_pt_valid  ;             
    ca_uint32_t                                            dscp  ;             
    ca_boolean_t                                    dscp_valid  ;             
    ca_boolean_t                                      has_ipop  ;             
    ca_boolean_t                                has_ipop_valid  ;             
    ca_boolean_t                                       l4_flag  ;             
    ca_boolean_t                                 l4_flag_valid  ;             
    ca_boolean_t                                 ipv4_fragment  ;             
    ca_boolean_t                           ipv4_fragment_valid  ;             
    ca_uint32_t                                special_pkt_code  ;             
    ca_boolean_t                        special_pkt_code_valid  ;             
    ca_uint32_t                                    top_vlan_id  ;             
    ca_boolean_t                                  top_is_svlan  ;             
    ca_boolean_t                              vlan_field_valid  ;             
    ca_uint32_t                                       top_dot1p  ;             
    ca_boolean_t                                   dot1p_valid  ;             
    aal_l2_cls_vlan_number_t                       vlan_number  ;             
    ca_boolean_t                               vlan_number_vld  ;             
    ca_uint32_t                                   ethernet_type  ;             
    ca_boolean_t                             ethernet_type_vld  ;             
    ca_uint32_t                                    l4_src_port  ;             
    ca_uint32_t                                    l4_dst_port  ;             
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl  ;             
    ca_boolean_t                                   field_is_or  ;             
    ca_uint32_t                                      precedence  ;             
} __attribute__((packed)) aal_l2_cls_cmr_key_t;                                                       
                                                                                                                                                           
typedef union {
    struct{
    ca_uint32_t                     ecn  : 1;             
    ca_uint32_t               ecn_valid  : 1;
    ca_uint32_t                  ip_da  : 1;                    
    ca_uint32_t                  ip_sa  : 1;                    
    ca_uint32_t                ip_ctrl  : 1;                    
    ca_uint32_t                  ip_pt  : 1;                    
    ca_uint32_t            ip_pt_valid  : 1;                    
    ca_uint32_t                   dscp  : 1;                    
    ca_uint32_t             dscp_valid  : 1;                    
    ca_uint32_t               has_ipop  : 1;                    
    ca_uint32_t         has_ipop_valid  : 1; 
    ca_uint32_t          ipv4_fragment  : 1;
    ca_uint32_t    ipv4_fragment_valid  : 1;
    ca_uint32_t                l4_flag  : 1;                    
    ca_uint32_t          l4_flag_valid  : 1;                    
    ca_uint32_t       special_pkt_code  : 1;                    
    ca_uint32_t special_pkt_code_valid  : 1;                    
    ca_uint32_t            top_vlan_id  : 1;                    
    ca_uint32_t           top_is_svlan  : 1;                    
    ca_uint32_t          inner_vlan_id  : 1;                    
    ca_uint32_t         inner_is_svlan  : 1;                    
    ca_uint32_t        vlan_field_ctrl  : 1;                    
    ca_uint32_t              top_dot1p  : 1;                    
    ca_uint32_t            inner_dot1p  : 1;                    
    ca_uint32_t             dot1p_ctrl  : 1;                    
    ca_uint32_t            vlan_number  : 1;                    
    ca_uint32_t        vlan_number_vld  : 1;                    
    ca_uint32_t          ethernet_type  : 1;                    
    ca_uint32_t      ethernet_type_vld  : 1;                    
    ca_uint32_t            l4_src_port  : 1;                    
    ca_uint32_t            l4_dst_port  : 1;                    
    ca_uint32_t     l4_port_field_ctrl  : 1;                    
    ca_uint32_t            field_is_or  : 1;                    
    ca_uint32_t             precedence  : 1; 
    }s;
    ca_uint32_t u32;
} aal_l2_cls_l3r_key_mask_t;               
                                                                              
typedef struct {                                                                                     
    ca_uint8_t                                          ecn   ;
    ca_boolean_t                                  ecn_valid   ;
    ca_uint32_t                                       ip_da    ;                                     
    ca_uint32_t                                       ip_sa    ;                                     
    aal_l2_cls_ip_ctrl_t                            ip_ctrl    ;                                     
    ca_uint32_t                                        ip_pt    ;                                     
    ca_boolean_t                                ip_pt_valid    ;                                     
    ca_uint32_t                                         dscp    ;                                     
    ca_boolean_t                                 dscp_valid    ;                                     
    ca_boolean_t                                   has_ipop    ;                                     
    ca_boolean_t                             has_ipop_valid    ; 
    ca_boolean_t                              ipv4_fragment    ;
    ca_boolean_t                        ipv4_fragment_valid    ;
    ca_boolean_t                                    l4_flag    ;                                     
    ca_boolean_t                              l4_flag_valid    ;                                     
    ca_uint32_t                             special_pkt_code    ;                                     
    ca_boolean_t                     special_pkt_code_valid    ;                                     
    ca_uint32_t                                 top_vlan_id    ;                                     
    ca_boolean_t                               top_is_svlan    ;                                     
    ca_uint32_t                               inner_vlan_id    ;                                     
    ca_boolean_t                             inner_is_svlan    ;                                     
    aal_l2_cls_vlan_field_ctrl_t            vlan_field_ctrl    ;                                     
    ca_uint32_t                                    top_dot1p    ;                                     
    ca_uint32_t                                  inner_dot1p    ;                                     
    aal_l2_cls_dot1p_field_ctrl_t                dot1p_ctrl    ;                                     
    aal_l2_cls_vlan_number_t                    vlan_number    ;                                     
    ca_boolean_t                            vlan_number_vld    ;                                     
    ca_uint32_t                                ethernet_type    ;                                     
    ca_boolean_t                          ethernet_type_vld    ;                                     
    ca_uint32_t                                 l4_src_port    ;                                     
    ca_uint32_t                                 l4_dst_port    ;                                     
    aal_l2_cls_l4_port_field_ctrl_t      l4_port_field_ctrl    ;                                     
    ca_boolean_t                                field_is_or    ;                                     
    ca_uint32_t                                   precedence    ;                                     
} __attribute__((packed))aal_l2_cls_l3r_key_t;                                                                              
                                                                                                     
                                                                                                     
                                                                                                     




typedef union {
    struct {
ca_uint64_t   valid                :
        1 ;
ca_uint64_t   mac_da_0             :
        1 ;
ca_uint64_t   mac_sa_0             :
        1 ;
ca_uint64_t   mac_field_ctrl_0     :
        1 ;
ca_uint64_t   top_vlan_id_0        :
        1 ;
ca_uint64_t   top_is_svlan_0       :
        1 ;
ca_uint64_t   inner_vlan_id_0      :
        1 ;
ca_uint64_t   inner_is_svlan_0     :
        1 ;
ca_uint64_t   vlan_field_ctrl_0    :
        1 ;
ca_uint64_t   top_dot1p_0          :
        1 ;
ca_uint64_t   inner_dot1p_0        :
        1 ;
ca_uint64_t   dot1p_ctrl_0         :
        1 ;
ca_uint64_t   vlan_number_0        :
        1 ;
ca_uint64_t   vlan_number_vld_0    :
        1 ;
ca_uint64_t   ethernet_type_0      :
        1 ;
ca_uint64_t   ethernet_type_vld_0  :
        1 ;
ca_uint64_t   l4_src_port_0        :
        1 ;
ca_uint64_t   l4_dst_port_0        :
        1 ;
ca_uint64_t   l4_port_field_ctrl_0 :
        1 ;
ca_uint64_t   field_is_or_0        :
        1 ;
ca_uint64_t   precedence_0         :
        1 ;
ca_uint64_t   mac_da_1             :
        1 ;
ca_uint64_t   mac_sa_1             :
        1 ;
ca_uint64_t   mac_field_ctrl_1     :
        1 ;
ca_uint64_t   top_vlan_id_1        :
        1 ;
ca_uint64_t   top_is_svlan_1       :
        1 ;
ca_uint64_t   inner_vlan_id_1      :
        1 ;
ca_uint64_t   inner_is_svlan_1     :
        1 ;
ca_uint64_t   vlan_field_ctrl_1    :
        1 ;
ca_uint64_t   top_dot1p_1          :
        1 ;
ca_uint64_t   inner_dot1p_1        :
        1 ;
ca_uint64_t   dot1p_ctrl_1         :
        1 ;
ca_uint64_t   vlan_number_1        :
        1 ;
ca_uint64_t   vlan_number_vld_1    :
        1 ;
ca_uint64_t   ethernet_type_1      :
        1 ;
ca_uint64_t   ethernet_type_vld_1  :
        1 ;
ca_uint64_t   l4_src_port_1        :
        1 ;
ca_uint64_t   l4_dst_port_1        :
        1 ;
ca_uint64_t   l4_port_field_ctrl_1 :
        1 ;
ca_uint64_t   field_is_or_1        :
        1 ;
ca_uint64_t   precedence_1         :
        1 ;
ca_uint64_t   f_rsvd               :
        26;
    } s;
    ca_uint64_t u64;
} aal_l2_cls_l2rl2r_key_mask_t;





typedef struct {
    ca_boolean_t                      valid;
    ca_mac_addr_t                     mac_da_0;
    ca_mac_addr_t                     mac_sa_0;
    aal_l2_cls_mac_field_ctrl_t       mac_field_ctrl_0;
    ca_uint32_t                      top_vlan_id_0;
    ca_boolean_t                      top_is_svlan_0;
    ca_uint32_t                      inner_vlan_id_0;
    ca_boolean_t                      inner_is_svlan_0;
    aal_l2_cls_vlan_field_ctrl_t      vlan_field_ctrl_0;
    ca_uint32_t                        top_dot1p_0;
    ca_uint32_t                        inner_dot1p_0;
    aal_l2_cls_dot1p_field_ctrl_t     dot1p_ctrl_0;
    aal_l2_cls_vlan_number_t          vlan_number_0;
    ca_boolean_t                      vlan_number_vld_0;
    ca_uint32_t                        ethernet_type_0;
    ca_boolean_t                      ethernet_type_vld_0;
    ca_uint32_t                       l4_src_port_0;
    ca_uint32_t                       l4_dst_port_0;
    aal_l2_cls_l4_port_field_ctrl_t   l4_port_field_ctrl_0;
    ca_boolean_t                      field_is_or_0;
    ca_uint32_t                        precedence_0;
    ca_mac_addr_t                     mac_da_1;
    ca_mac_addr_t                     mac_sa_1;
    aal_l2_cls_mac_field_ctrl_t       mac_field_ctrl_1;
    ca_uint32_t                      top_vlan_id_1;
    ca_boolean_t                      top_is_svlan_1;
    ca_uint32_t                      inner_vlan_id_1;
    ca_boolean_t                      inner_is_svlan_1;
    aal_l2_cls_vlan_field_ctrl_t      vlan_field_ctrl_1;
    ca_uint32_t                        top_dot1p_1;
    ca_uint32_t                        inner_dot1p_1;
    aal_l2_cls_dot1p_field_ctrl_t     dot1p_ctrl_1;
    aal_l2_cls_vlan_number_t          vlan_number_1;
    ca_boolean_t                      vlan_number_vld_1;
    ca_uint32_t                        ethernet_type_1;
    ca_boolean_t                      ethernet_type_vld_1;
    ca_uint32_t                       l4_src_port_1;
    ca_uint32_t                       l4_dst_port_1;
    aal_l2_cls_l4_port_field_ctrl_t   l4_port_field_ctrl_1;
    ca_boolean_t                      field_is_or_1;
    ca_uint32_t                        precedence_1;
}__attribute__((packed)) aal_l2_cls_l2rl2r_key_t;

typedef union{                                          
    struct {                                            
        ca_uint64_t                      valid  : 1;    
        ca_uint64_t                   mac_da_0  : 1;    
        ca_uint64_t                   mac_sa_0  : 1;    
        ca_uint64_t           mac_field_ctrl_0  : 1;    
        ca_uint64_t              top_vlan_id_0  : 1;    
        ca_uint64_t             top_is_svlan_0  : 1;    
        ca_uint64_t            inner_vlan_id_0  : 1;    
        ca_uint64_t           inner_is_svlan_0  : 1;    
        ca_uint64_t          vlan_field_ctrl_0  : 1;    
        ca_uint64_t                top_dot1p_0  : 1;    
        ca_uint64_t              inner_dot1p_0  : 1;    
        ca_uint64_t               dot1p_ctrl_0  : 1;    
        ca_uint64_t              vlan_number_0  : 1;    
        ca_uint64_t          vlan_number_vld_0  : 1;    
        ca_uint64_t            ethernet_type_0  : 1;    
        ca_uint64_t        ethernet_type_vld_0  : 1;    
        ca_uint64_t              l4_src_port_0  : 1;    
        ca_uint64_t              l4_dst_port_0  : 1;    
        ca_uint64_t       l4_port_field_ctrl_0  : 1;    
        ca_uint64_t              field_is_or_0  : 1;    
        ca_uint64_t               precedence_0  : 1; 
        ca_uint64_t                      ecn_1  : 1;             
        ca_uint64_t                ecn_valid_1  : 1;
        ca_uint64_t                    ip_da_1  : 1;    
        ca_uint64_t                    ip_sa_1  : 1;    
        ca_uint64_t                  ip_ctrl_1  : 1;    
        ca_uint64_t                    ip_pt_1  : 1;    
        ca_uint64_t              ip_pt_valid_1  : 1;    
        ca_uint64_t                     dscp_1  : 1;    
        ca_uint64_t               dscp_valid_1  : 1;    
        ca_uint64_t                 has_ipop_1  : 1;    
        ca_uint64_t           has_ipop_valid_1  : 1;        
        ca_uint64_t            ipv4_fragment_1  : 1;
        ca_uint64_t      ipv4_fragment_valid_1  : 1;
        ca_uint64_t                  l4_flag_1  : 1;    
        ca_uint64_t            l4_flag_valid_1  : 1;    
        ca_uint64_t         special_pkt_code_1  : 1;    
        ca_uint64_t   special_pkt_code_valid_1  : 1;    
        ca_uint64_t              top_vlan_id_1  : 1;    
        ca_uint64_t             top_is_svlan_1  : 1;    
        ca_uint64_t            inner_vlan_id_1  : 1;    
        ca_uint64_t           inner_is_svlan_1  : 1;    
        ca_uint64_t          vlan_field_ctrl_1  : 1;    
        ca_uint64_t                top_dot1p_1  : 1;    
        ca_uint64_t              inner_dot1p_1  : 1;    
        ca_uint64_t               dot1p_ctrl_1  : 1;    
        ca_uint64_t              vlan_number_1  : 1;    
        ca_uint64_t          vlan_number_vld_1  : 1;    
        ca_uint64_t            ethernet_type_1  : 1;    
        ca_uint64_t        ethernet_type_vld_1  : 1;    
        ca_uint64_t              l4_src_port_1  : 1;    
        ca_uint64_t              l4_dst_port_1  : 1;    
        ca_uint64_t       l4_port_field_ctrl_1  : 1;    
        ca_uint64_t              field_is_or_1  : 1;    
        ca_uint64_t               precedence_1  : 1;    
    }s;                                                 
    ca_uint64_t u64;                                    
}aal_l2_cls_l2rl3r_key_mask_t;                          
                                                        

    


typedef struct {                                                      
    ca_boolean_t                                           valid  ;   
    ca_mac_addr_t                                       mac_da_0  ;   
    ca_mac_addr_t                                       mac_sa_0  ;   
    aal_l2_cls_mac_field_ctrl_t                 mac_field_ctrl_0  ;   
    ca_uint32_t                                    top_vlan_id_0  ;   
    ca_boolean_t                                  top_is_svlan_0  ;   
    ca_uint32_t                                  inner_vlan_id_0  ;   
    ca_boolean_t                                inner_is_svlan_0  ;   
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_0  ;   
    ca_uint32_t                                       top_dot1p_0  ;   
    ca_uint32_t                                     inner_dot1p_0  ;   
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_0  ;   
    aal_l2_cls_vlan_number_t                       vlan_number_0  ;   
    ca_boolean_t                               vlan_number_vld_0  ;   
    ca_uint32_t                                   ethernet_type_0  ;   
    ca_boolean_t                             ethernet_type_vld_0  ;   
    ca_uint32_t                                    l4_src_port_0  ;   
    ca_uint32_t                                    l4_dst_port_0  ;   
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_0  ;   
    ca_boolean_t                                   field_is_or_0  ;   
    ca_uint32_t                                      precedence_0  ; 
    ca_uint8_t                                         ecn_1      ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_uint32_t                                          ip_da_1  ;   
    ca_uint32_t                                          ip_sa_1  ;   
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_1  ;   
    ca_uint32_t                                           ip_pt_1  ;   
    ca_boolean_t                                   ip_pt_valid_1  ;   
    ca_uint32_t                                            dscp_1  ;   
    ca_boolean_t                                    dscp_valid_1  ;   
    ca_boolean_t                                      has_ipop_1  ;   
    ca_boolean_t                                has_ipop_valid_1  ;     
    ca_boolean_t                                 ipv4_fragment_1  ;
    ca_boolean_t                           ipv4_fragment_valid_1  ;
    ca_boolean_t                                       l4_flag_1  ;   
    ca_boolean_t                                 l4_flag_valid_1  ;   
    ca_uint32_t                                special_pkt_code_1  ;   
    ca_boolean_t                        special_pkt_code_valid_1  ;   
    ca_uint32_t                                    top_vlan_id_1  ;   
    ca_boolean_t                                  top_is_svlan_1  ;   
    ca_uint32_t                                  inner_vlan_id_1  ;   
    ca_boolean_t                                inner_is_svlan_1  ;   
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_1  ;   
    ca_uint32_t                                       top_dot1p_1  ;   
    ca_uint32_t                                     inner_dot1p_1  ;   
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_1  ;   
    aal_l2_cls_vlan_number_t                       vlan_number_1  ;   
    ca_boolean_t                               vlan_number_vld_1  ;   
    ca_uint32_t                                   ethernet_type_1  ;   
    ca_boolean_t                             ethernet_type_vld_1  ;   
    ca_uint32_t                                    l4_src_port_1  ;   
    ca_uint32_t                                    l4_dst_port_1  ;   
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_1  ;   
    ca_boolean_t                                   field_is_or_1  ;   
    ca_uint32_t                                      precedence_1  ;   
}__attribute__((packed))aal_l2_cls_l2rl3r_key_t;                                             
                                                                      
typedef union{                                                          
    struct {                                                            
        ca_uint64_t                      valid  : 1 ;                   
        ca_uint64_t                   mac_da_0  : 1 ;                   
        ca_uint64_t                   mac_sa_0  : 1 ;                   
        ca_uint64_t           mac_field_ctrl_0  : 1 ;                   
        ca_uint64_t              top_vlan_id_0  : 1 ;                   
        ca_uint64_t             top_is_svlan_0  : 1 ;                   
        ca_uint64_t            inner_vlan_id_0  : 1 ;                   
        ca_uint64_t           inner_is_svlan_0  : 1 ;                   
        ca_uint64_t          vlan_field_ctrl_0  : 1 ;                   
        ca_uint64_t                top_dot1p_0  : 1 ;                   
        ca_uint64_t              inner_dot1p_0  : 1 ;                   
        ca_uint64_t               dot1p_ctrl_0  : 1 ;                   
        ca_uint64_t              vlan_number_0  : 1 ;                   
        ca_uint64_t          vlan_number_vld_0  : 1 ;                   
        ca_uint64_t            ethernet_type_0  : 1 ;                   
        ca_uint64_t        ethernet_type_vld_0  : 1 ;                   
        ca_uint64_t              l4_src_port_0  : 1 ;                   
        ca_uint64_t              l4_dst_port_0  : 1 ;                   
        ca_uint64_t       l4_port_field_ctrl_0  : 1 ;                   
        ca_uint64_t              field_is_or_0  : 1 ;                   
        ca_uint64_t               precedence_0  : 1 ;                   
        ca_uint64_t                      ecn_1  : 1;             
        ca_uint64_t                ecn_valid_1  : 1;
        ca_uint64_t                 mac_addr_1  : 1 ;                   
        ca_uint64_t           mac_addr_is_sa_1  : 1 ;                   
        ca_uint64_t           mac_addr_valid_1  : 1 ;                   
        ca_uint64_t                  ip_addr_1  : 1 ;                   
        ca_uint64_t            ip_addr_is_sa_1  : 1 ;                   
        ca_uint64_t            ip_addr_valid_1  : 1 ;                   
        ca_uint64_t                    ip_pt_1  : 1 ;                   
        ca_uint64_t              ip_pt_valid_1  : 1 ;                   
        ca_uint64_t                     dscp_1  : 1 ;                   
        ca_uint64_t               dscp_valid_1  : 1 ;                   
        ca_uint64_t                 has_ipop_1  : 1 ;                   
        ca_uint64_t           has_ipop_valid_1  : 1 ;                   
        ca_uint64_t                  l4_flag_1  : 1 ;                   
        ca_uint64_t            l4_flag_valid_1  : 1 ;                   
        ca_uint64_t            ipv4_fragment_1  : 1 ;                   
        ca_uint64_t      ipv4_fragment_valid_1  : 1 ;                   
        ca_uint64_t         special_pkt_code_1  : 1 ;                   
        ca_uint64_t   special_pkt_code_valid_1  : 1 ;                   
        ca_uint64_t              top_vlan_id_1  : 1 ;                   
        ca_uint64_t             top_is_svlan_1  : 1 ;                   
        ca_uint64_t         vlan_field_valid_1  : 1 ;                   
        ca_uint64_t                top_dot1p_1  : 1 ;                   
        ca_uint64_t              dot1p_valid_1  : 1 ;                   
        ca_uint64_t              vlan_number_1  : 1 ;                   
        ca_uint64_t          vlan_number_vld_1  : 1 ;                   
        ca_uint64_t            ethernet_type_1  : 1 ;                   
        ca_uint64_t        ethernet_type_vld_1  : 1 ;                   
        ca_uint64_t              l4_src_port_1  : 1 ;                   
        ca_uint64_t              l4_dst_port_1  : 1 ;                   
        ca_uint64_t       l4_port_field_ctrl_1  : 1 ;                   
        ca_uint64_t              field_is_or_1  : 1 ;                   
        ca_uint64_t               precedence_1  : 1 ;                   
    }s;                                                                 
    ca_uint64_t u64;                                                    
}aal_l2_cls_l2rcmr_key_mask_t;                                          
                                                                                                                                                                                                                                                                                                                                                                                                                                       
typedef struct {                                                      
    ca_boolean_t                                           valid  ;   
    ca_mac_addr_t                                       mac_da_0  ;   
    ca_mac_addr_t                                       mac_sa_0  ;   
    aal_l2_cls_mac_field_ctrl_t                 mac_field_ctrl_0  ;   
    ca_uint32_t                                    top_vlan_id_0  ;   
    ca_boolean_t                                  top_is_svlan_0  ;   
    ca_uint32_t                                  inner_vlan_id_0  ;   
    ca_boolean_t                                inner_is_svlan_0  ;   
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_0  ;   
    ca_uint32_t                                       top_dot1p_0  ;   
    ca_uint32_t                                     inner_dot1p_0  ;   
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_0  ;   
    aal_l2_cls_vlan_number_t                       vlan_number_0  ;   
    ca_boolean_t                               vlan_number_vld_0  ;   
    ca_uint32_t                                   ethernet_type_0  ;   
    ca_boolean_t                             ethernet_type_vld_0  ;   
    ca_uint32_t                                    l4_src_port_0  ;   
    ca_uint32_t                                    l4_dst_port_0  ;   
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_0  ;   
    ca_boolean_t                                   field_is_or_0  ;   
    ca_uint32_t                                      precedence_0  ;  
    ca_uint8_t                                         ecn_1      ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_mac_addr_t                                     mac_addr_1  ; 
    ca_boolean_t                                mac_addr_is_sa_1  ; 
    ca_boolean_t                                mac_addr_valid_1  ; 
    ca_uint32_t                                        ip_addr_1  ; 
    ca_boolean_t                                 ip_addr_is_sa_1  ; 
    ca_boolean_t                                 ip_addr_valid_1  ; 
    ca_uint32_t                                           ip_pt_1  ; 
    ca_boolean_t                                   ip_pt_valid_1  ; 
    ca_uint32_t                                            dscp_1  ; 
    ca_boolean_t                                    dscp_valid_1  ; 
    ca_boolean_t                                      has_ipop_1  ; 
    ca_boolean_t                                has_ipop_valid_1  ; 
    ca_boolean_t                                       l4_flag_1  ; 
    ca_boolean_t                                 l4_flag_valid_1  ; 
    ca_boolean_t                                 ipv4_fragment_1  ; 
    ca_boolean_t                           ipv4_fragment_valid_1  ; 
    ca_uint32_t                                special_pkt_code_1  ; 
    ca_boolean_t                        special_pkt_code_valid_1  ; 
    ca_uint32_t                                    top_vlan_id_1  ; 
    ca_boolean_t                                  top_is_svlan_1  ; 
    ca_boolean_t                              vlan_field_valid_1  ; 
    ca_uint32_t                                       top_dot1p_1  ; 
    ca_boolean_t                                   dot1p_valid_1  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_1  ; 
    ca_boolean_t                               vlan_number_vld_1  ; 
    ca_uint32_t                                   ethernet_type_1  ; 
    ca_boolean_t                             ethernet_type_vld_1  ; 
    ca_uint32_t                                    l4_src_port_1  ; 
    ca_uint32_t                                    l4_dst_port_1  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_1  ; 
    ca_boolean_t                                   field_is_or_1  ; 
    ca_uint32_t                                      precedence_1  ;  
}__attribute__((packed))aal_l2_cls_l2rcmr_key_t;                                             

typedef union{
    struct { 
        ca_uint64_t                       valid : 1 ;
        ca_uint64_t                      ecn_0  : 1;             
        ca_uint64_t                ecn_valid_0  : 1;
        ca_uint64_t                     ip_da_0 : 1 ;                   
        ca_uint64_t                     ip_sa_0 : 1 ;                   
        ca_uint64_t                   ip_ctrl_0 : 1 ;                   
        ca_uint64_t                     ip_pt_0 : 1 ;                   
        ca_uint64_t               ip_pt_valid_0 : 1 ;                   
        ca_uint64_t                      dscp_0 : 1 ;                   
        ca_uint64_t                dscp_valid_0 : 1 ;                   
        ca_uint64_t                  has_ipop_0 : 1 ;                   
        ca_uint64_t            has_ipop_valid_0 : 1 ;           
        ca_uint64_t             ipv4_fragment_0 : 1 ;
        ca_uint64_t       ipv4_fragment_valid_0 : 1 ;
        ca_uint64_t                   l4_flag_0 : 1 ;                   
        ca_uint64_t             l4_flag_valid_0 : 1 ;                   
        ca_uint64_t          special_pkt_code_0 : 1 ;                   
        ca_uint64_t    special_pkt_code_valid_0 : 1 ;                   
        ca_uint64_t               top_vlan_id_0 : 1 ;                   
        ca_uint64_t              top_is_svlan_0 : 1 ;                   
        ca_uint64_t             inner_vlan_id_0 : 1 ;                   
        ca_uint64_t            inner_is_svlan_0 : 1 ;                   
        ca_uint64_t           vlan_field_ctrl_0 : 1 ;                   
        ca_uint64_t                 top_dot1p_0 : 1 ;                   
        ca_uint64_t               inner_dot1p_0 : 1 ;                   
        ca_uint64_t                dot1p_ctrl_0 : 1 ;                   
        ca_uint64_t               vlan_number_0 : 1 ;                   
        ca_uint64_t           vlan_number_vld_0 : 1 ;                   
        ca_uint64_t             ethernet_type_0 : 1 ;                   
        ca_uint64_t         ethernet_type_vld_0 : 1 ;                   
        ca_uint64_t               l4_src_port_0 : 1 ;                   
        ca_uint64_t               l4_dst_port_0 : 1 ;                   
        ca_uint64_t        l4_port_field_ctrl_0 : 1 ;                   
        ca_uint64_t               field_is_or_0 : 1 ;                   
        ca_uint64_t                precedence_0 : 1 ; 
        ca_uint64_t                      ecn_1  : 1;             
        ca_uint64_t                ecn_valid_1  : 1;
        ca_uint64_t                     ip_da_1 : 1 ;
        ca_uint64_t                     ip_sa_1 : 1 ;
        ca_uint64_t                   ip_ctrl_1 : 1 ;
        ca_uint64_t                     ip_pt_1 : 1 ;
        ca_uint64_t               ip_pt_valid_1 : 1 ;
        ca_uint64_t                      dscp_1 : 1 ;
        ca_uint64_t                dscp_valid_1 : 1 ;
        ca_uint64_t                  has_ipop_1 : 1 ;
        ca_uint64_t            has_ipop_valid_1 : 1 ;
        ca_uint64_t             ipv4_fragment_1 : 1 ;
        ca_uint64_t       ipv4_fragment_valid_1 : 1 ;
        ca_uint64_t                   l4_flag_1 : 1 ;
        ca_uint64_t             l4_flag_valid_1 : 1 ;
        ca_uint64_t          special_pkt_code_1 : 1 ;
        ca_uint64_t    special_pkt_code_valid_1 : 1 ;
        ca_uint64_t               top_vlan_id_1 : 1 ;
        ca_uint64_t              top_is_svlan_1 : 1 ;
        ca_uint64_t             inner_vlan_id_1 : 1 ;
        ca_uint64_t            inner_is_svlan_1 : 1 ;
        ca_uint64_t           vlan_field_ctrl_1 : 1 ;
        ca_uint64_t                 top_dot1p_1 : 1 ;
        ca_uint64_t               inner_dot1p_1 : 1 ;
        ca_uint64_t                dot1p_ctrl_1 : 1 ;
        ca_uint64_t               vlan_number_1 : 1 ;
        ca_uint64_t           vlan_number_vld_1 : 1 ;
        ca_uint64_t             ethernet_type_1 : 1 ;
        ca_uint64_t         ethernet_type_vld_1 : 1 ;
        ca_uint64_t               l4_src_port_1 : 1 ;
        ca_uint64_t               l4_dst_port_1 : 1 ;
        ca_uint64_t        l4_port_field_ctrl_1 : 1 ;
        ca_uint64_t               field_is_or_1 : 1 ;
        ca_uint64_t                precedence_1 : 1 ;                
    }s;
    ca_uint64_t u64;
} aal_l2_cls_l3rl3r_key_mask_t;    

typedef struct {          
    ca_boolean_t                                           valid ;
    ca_uint8_t                                         ecn_0      ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_uint32_t                                          ip_da_0  ;  
    ca_uint32_t                                          ip_sa_0  ;  
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_0  ;  
    ca_uint32_t                                           ip_pt_0  ;  
    ca_boolean_t                                   ip_pt_valid_0  ;  
    ca_uint32_t                                            dscp_0  ;  
    ca_boolean_t                                    dscp_valid_0  ;  
    ca_boolean_t                                      has_ipop_0  ;  
    ca_boolean_t                                has_ipop_valid_0  ; 
    ca_boolean_t                                 ipv4_fragment_0  ;
    ca_boolean_t                           ipv4_fragment_valid_0  ;
    ca_boolean_t                                       l4_flag_0  ;  
    ca_boolean_t                                 l4_flag_valid_0  ;  
    ca_uint32_t                                special_pkt_code_0  ;  
    ca_boolean_t                        special_pkt_code_valid_0  ;  
    ca_uint32_t                                    top_vlan_id_0  ;  
    ca_boolean_t                                  top_is_svlan_0  ;  
    ca_uint32_t                                  inner_vlan_id_0  ;  
    ca_boolean_t                                inner_is_svlan_0  ;  
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_0  ;  
    ca_uint32_t                                       top_dot1p_0  ;  
    ca_uint32_t                                     inner_dot1p_0  ;  
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_0  ;  
    aal_l2_cls_vlan_number_t                       vlan_number_0  ;  
    ca_boolean_t                               vlan_number_vld_0  ;  
    ca_uint32_t                                   ethernet_type_0  ;  
    ca_boolean_t                             ethernet_type_vld_0  ;  
    ca_uint32_t                                    l4_src_port_0  ;  
    ca_uint32_t                                    l4_dst_port_0  ;  
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_0  ;  
    ca_boolean_t                                   field_is_or_0  ;  
    ca_uint32_t                                      precedence_0  ;  
    ca_uint8_t                                         ecn_1      ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_uint32_t                                          ip_da_1  ;  
    ca_uint32_t                                          ip_sa_1  ;  
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_1  ;  
    ca_uint32_t                                           ip_pt_1  ;  
    ca_boolean_t                                   ip_pt_valid_1  ;  
    ca_uint32_t                                            dscp_1  ;  
    ca_boolean_t                                    dscp_valid_1  ;  
    ca_boolean_t                                      has_ipop_1  ;  
    ca_boolean_t                                has_ipop_valid_1  ; 
    ca_boolean_t                                 ipv4_fragment_1  ;
    ca_boolean_t                           ipv4_fragment_valid_1  ;
    ca_boolean_t                                       l4_flag_1  ;  
    ca_boolean_t                                 l4_flag_valid_1  ;  
    ca_uint32_t                                special_pkt_code_1  ;  
    ca_boolean_t                        special_pkt_code_valid_1  ;  
    ca_uint32_t                                    top_vlan_id_1  ;  
    ca_boolean_t                                  top_is_svlan_1  ;  
    ca_uint32_t                                  inner_vlan_id_1  ;  
    ca_boolean_t                                inner_is_svlan_1  ;  
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_1  ;  
    ca_uint32_t                                       top_dot1p_1  ;  
    ca_uint32_t                                     inner_dot1p_1  ;  
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_1  ;  
    aal_l2_cls_vlan_number_t                       vlan_number_1  ;  
    ca_boolean_t                               vlan_number_vld_1  ;  
    ca_uint32_t                                   ethernet_type_1  ;  
    ca_boolean_t                             ethernet_type_vld_1  ;  
    ca_uint32_t                                    l4_src_port_1  ;  
    ca_uint32_t                                    l4_dst_port_1  ;  
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_1  ;  
    ca_boolean_t                                   field_is_or_1  ;  
    ca_uint32_t                                      precedence_1  ;  
} __attribute__((packed))aal_l2_cls_l3rl3r_key_t;                                           

typedef union{
    struct { 
        ca_uint64_t                        valid : 1 ; 
        ca_uint64_t                      ecn_0  : 1;             
        ca_uint64_t                ecn_valid_0  : 1;
        ca_uint64_t                      ip_da_0 : 1 ; 
        ca_uint64_t                      ip_sa_0 : 1 ; 
        ca_uint64_t                    ip_ctrl_0 : 1 ; 
        ca_uint64_t                      ip_pt_0 : 1 ; 
        ca_uint64_t                ip_pt_valid_0 : 1 ; 
        ca_uint64_t                       dscp_0 : 1 ; 
        ca_uint64_t                 dscp_valid_0 : 1 ; 
        ca_uint64_t                   has_ipop_0 : 1 ; 
        ca_uint64_t             has_ipop_valid_0 : 1 ;           
        ca_uint64_t              ipv4_fragment_0 : 1 ;
        ca_uint64_t        ipv4_fragment_valid_0 : 1 ;
        ca_uint64_t                    l4_flag_0 : 1 ; 
        ca_uint64_t              l4_flag_valid_0 : 1 ; 
        ca_uint64_t           special_pkt_code_0 : 1 ; 
        ca_uint64_t     special_pkt_code_valid_0 : 1 ; 
        ca_uint64_t                top_vlan_id_0 : 1 ; 
        ca_uint64_t               top_is_svlan_0 : 1 ; 
        ca_uint64_t              inner_vlan_id_0 : 1 ; 
        ca_uint64_t             inner_is_svlan_0 : 1 ; 
        ca_uint64_t            vlan_field_ctrl_0 : 1 ; 
        ca_uint64_t                  top_dot1p_0 : 1 ; 
        ca_uint64_t                inner_dot1p_0 : 1 ; 
        ca_uint64_t                 dot1p_ctrl_0 : 1 ; 
        ca_uint64_t                vlan_number_0 : 1 ; 
        ca_uint64_t            vlan_number_vld_0 : 1 ; 
        ca_uint64_t              ethernet_type_0 : 1 ; 
        ca_uint64_t          ethernet_type_vld_0 : 1 ; 
        ca_uint64_t                l4_src_port_0 : 1 ; 
        ca_uint64_t                l4_dst_port_0 : 1 ; 
        ca_uint64_t         l4_port_field_ctrl_0 : 1 ; 
        ca_uint64_t                field_is_or_0 : 1 ; 
        ca_uint64_t                 precedence_0 : 1 ; 
        ca_uint64_t                     mac_da_1 : 1 ; 
        ca_uint64_t                     mac_sa_1 : 1 ; 
        ca_uint64_t             mac_field_ctrl_1 : 1 ; 
        ca_uint64_t                top_vlan_id_1 : 1 ; 
        ca_uint64_t               top_is_svlan_1 : 1 ; 
        ca_uint64_t              inner_vlan_id_1 : 1 ; 
        ca_uint64_t             inner_is_svlan_1 : 1 ; 
        ca_uint64_t            vlan_field_ctrl_1 : 1 ; 
        ca_uint64_t                  top_dot1p_1 : 1 ; 
        ca_uint64_t                inner_dot1p_1 : 1 ; 
        ca_uint64_t                 dot1p_ctrl_1 : 1 ; 
        ca_uint64_t                vlan_number_1 : 1 ; 
        ca_uint64_t            vlan_number_vld_1 : 1 ; 
        ca_uint64_t              ethernet_type_1 : 1 ; 
        ca_uint64_t          ethernet_type_vld_1 : 1 ; 
        ca_uint64_t                l4_src_port_1 : 1 ; 
        ca_uint64_t                l4_dst_port_1 : 1 ; 
        ca_uint64_t         l4_port_field_ctrl_1 : 1 ; 
        ca_uint64_t                field_is_or_1 : 1 ; 
        ca_uint64_t                 precedence_1 : 1 ;             
    }s;
    ca_uint64_t u64;
} aal_l2_cls_l3rl2r_key_mask_t; 


typedef struct {                                                       
    ca_boolean_t                                           valid  ;   
    ca_uint8_t                                         ecn_0      ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_uint32_t                                          ip_da_0  ;    
    ca_uint32_t                                          ip_sa_0  ;    
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_0  ;    
    ca_uint32_t                                           ip_pt_0  ;    
    ca_boolean_t                                   ip_pt_valid_0  ;    
    ca_uint32_t                                            dscp_0  ;    
    ca_boolean_t                                    dscp_valid_0  ;    
    ca_boolean_t                                      has_ipop_0  ;    
    ca_boolean_t                                has_ipop_valid_0  ;    
    ca_boolean_t                                 ipv4_fragment_0  ;
    ca_boolean_t                           ipv4_fragment_valid_0  ;
    ca_boolean_t                                       l4_flag_0  ;    
    ca_boolean_t                                 l4_flag_valid_0  ;    
    ca_uint32_t                                special_pkt_code_0  ;    
    ca_boolean_t                        special_pkt_code_valid_0  ;    
    ca_uint32_t                                    top_vlan_id_0  ;    
    ca_boolean_t                                  top_is_svlan_0  ;    
    ca_uint32_t                                  inner_vlan_id_0  ;    
    ca_boolean_t                                inner_is_svlan_0  ;    
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_0  ;    
    ca_uint32_t                                       top_dot1p_0  ;    
    ca_uint32_t                                     inner_dot1p_0  ;    
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_0  ;    
    aal_l2_cls_vlan_number_t                       vlan_number_0  ;    
    ca_boolean_t                               vlan_number_vld_0  ;    
    ca_uint32_t                                   ethernet_type_0  ;    
    ca_boolean_t                             ethernet_type_vld_0  ;    
    ca_uint32_t                                    l4_src_port_0  ;    
    ca_uint32_t                                    l4_dst_port_0  ;    
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_0  ;    
    ca_boolean_t                                   field_is_or_0  ;    
    ca_uint32_t                                      precedence_0  ;    
    ca_mac_addr_t                                       mac_da_1  ;    
    ca_mac_addr_t                                       mac_sa_1  ;    
    aal_l2_cls_mac_field_ctrl_t                 mac_field_ctrl_1  ;    
    ca_uint32_t                                    top_vlan_id_1  ;    
    ca_boolean_t                                  top_is_svlan_1  ;    
    ca_uint32_t                                  inner_vlan_id_1  ;    
    ca_boolean_t                                inner_is_svlan_1  ;    
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_1  ;    
    ca_uint32_t                                       top_dot1p_1  ;    
    ca_uint32_t                                     inner_dot1p_1  ;    
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_1  ;    
    aal_l2_cls_vlan_number_t                       vlan_number_1  ;    
    ca_boolean_t                               vlan_number_vld_1  ;    
    ca_uint32_t                                   ethernet_type_1  ;    
    ca_boolean_t                             ethernet_type_vld_1  ;    
    ca_uint32_t                                    l4_src_port_1  ;    
    ca_uint32_t                                    l4_dst_port_1  ;    
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_1  ;    
    ca_boolean_t                                   field_is_or_1  ;    
    ca_uint32_t                                      precedence_1  ;    
} __attribute__((packed))aal_l2_cls_l3rl2r_key_t;                                             

typedef union{
    struct { 
        ca_uint64_t                        valid : 1 ; 
        ca_uint64_t                      ecn_0  : 1;             
        ca_uint64_t                ecn_valid_0  : 1;
        ca_uint64_t                      ip_da_0 : 1 ; 
        ca_uint64_t                      ip_sa_0 : 1 ; 
        ca_uint64_t                    ip_ctrl_0 : 1 ; 
        ca_uint64_t                      ip_pt_0 : 1 ; 
        ca_uint64_t                ip_pt_valid_0 : 1 ; 
        ca_uint64_t                       dscp_0 : 1 ; 
        ca_uint64_t                 dscp_valid_0 : 1 ; 
        ca_uint64_t                   has_ipop_0 : 1 ; 
        ca_uint64_t             has_ipop_valid_0 : 1 ;           
        ca_uint64_t              ipv4_fragment_0 : 1 ;
        ca_uint64_t        ipv4_fragment_valid_0 : 1 ;
        ca_uint64_t                    l4_flag_0 : 1 ; 
        ca_uint64_t              l4_flag_valid_0 : 1 ; 
        ca_uint64_t           special_pkt_code_0 : 1 ; 
        ca_uint64_t     special_pkt_code_valid_0 : 1 ; 
        ca_uint64_t                top_vlan_id_0 : 1 ; 
        ca_uint64_t               top_is_svlan_0 : 1 ; 
        ca_uint64_t              inner_vlan_id_0 : 1 ; 
        ca_uint64_t             inner_is_svlan_0 : 1 ; 
        ca_uint64_t            vlan_field_ctrl_0 : 1 ; 
        ca_uint64_t                  top_dot1p_0 : 1 ; 
        ca_uint64_t                inner_dot1p_0 : 1 ; 
        ca_uint64_t                 dot1p_ctrl_0 : 1 ; 
        ca_uint64_t                vlan_number_0 : 1 ; 
        ca_uint64_t            vlan_number_vld_0 : 1 ; 
        ca_uint64_t              ethernet_type_0 : 1 ; 
        ca_uint64_t          ethernet_type_vld_0 : 1 ; 
        ca_uint64_t                l4_src_port_0 : 1 ; 
        ca_uint64_t                l4_dst_port_0 : 1 ; 
        ca_uint64_t         l4_port_field_ctrl_0 : 1 ; 
        ca_uint64_t                field_is_or_0 : 1 ; 
        ca_uint64_t                 precedence_0 : 1 ; 
        ca_uint64_t                      ecn_1  : 1;             
        ca_uint64_t                ecn_valid_1  : 1;
        ca_uint64_t                  mac_addr_1  : 1 ; 
        ca_uint64_t            mac_addr_is_sa_1  : 1 ; 
        ca_uint64_t            mac_addr_valid_1  : 1 ; 
        ca_uint64_t                   ip_addr_1  : 1 ; 
        ca_uint64_t             ip_addr_is_sa_1  : 1 ; 
        ca_uint64_t             ip_addr_valid_1  : 1 ; 
        ca_uint64_t                     ip_pt_1  : 1 ; 
        ca_uint64_t               ip_pt_valid_1  : 1 ; 
        ca_uint64_t                      dscp_1  : 1 ; 
        ca_uint64_t                dscp_valid_1  : 1 ; 
        ca_uint64_t                  has_ipop_1  : 1 ; 
        ca_uint64_t            has_ipop_valid_1  : 1 ; 
        ca_uint64_t                   l4_flag_1  : 1 ; 
        ca_uint64_t             l4_flag_valid_1  : 1 ; 
        ca_uint64_t             ipv4_fragment_1  : 1 ; 
        ca_uint64_t       ipv4_fragment_valid_1  : 1 ; 
        ca_uint64_t          special_pkt_code_1  : 1 ; 
        ca_uint64_t    special_pkt_code_valid_1  : 1 ; 
        ca_uint64_t               top_vlan_id_1  : 1 ; 
        ca_uint64_t              top_is_svlan_1  : 1 ; 
        ca_uint64_t          vlan_field_valid_1  : 1 ; 
        ca_uint64_t                 top_dot1p_1  : 1 ; 
        ca_uint64_t               dot1p_valid_1  : 1 ; 
        ca_uint64_t               vlan_number_1  : 1 ; 
        ca_uint64_t           vlan_number_vld_1  : 1 ; 
        ca_uint64_t             ethernet_type_1  : 1 ; 
        ca_uint64_t         ethernet_type_vld_1  : 1 ; 
        ca_uint64_t               l4_src_port_1  : 1 ; 
        ca_uint64_t               l4_dst_port_1  : 1 ; 
        ca_uint64_t        l4_port_field_ctrl_1  : 1 ; 
        ca_uint64_t               field_is_or_1  : 1 ; 
        ca_uint64_t                precedence_1  : 1 ; 
    }s;
    ca_uint64_t u64;
} aal_l2_cls_l3rcmr_key_mask_t; 


typedef struct {                                                       
    ca_boolean_t                                           valid  ;
    ca_uint8_t                                              ecn_0 ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_uint32_t                                          ip_da_0  ;    
    ca_uint32_t                                          ip_sa_0  ;    
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_0  ;    
    ca_uint32_t                                           ip_pt_0  ;    
    ca_boolean_t                                   ip_pt_valid_0  ;    
    ca_uint32_t                                            dscp_0  ;    
    ca_boolean_t                                    dscp_valid_0  ;    
    ca_boolean_t                                      has_ipop_0  ;    
    ca_boolean_t                                has_ipop_valid_0  ;    
    ca_boolean_t                                 ipv4_fragment_0  ;
    ca_boolean_t                           ipv4_fragment_valid_0  ;
    ca_boolean_t                                       l4_flag_0  ;    
    ca_boolean_t                                 l4_flag_valid_0  ;    
    ca_uint32_t                                special_pkt_code_0  ;    
    ca_boolean_t                        special_pkt_code_valid_0  ;    
    ca_uint32_t                                    top_vlan_id_0  ;    
    ca_boolean_t                                  top_is_svlan_0  ;    
    ca_uint32_t                                  inner_vlan_id_0  ;    
    ca_boolean_t                                inner_is_svlan_0  ;    
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_0  ;    
    ca_uint32_t                                       top_dot1p_0  ;    
    ca_uint32_t                                     inner_dot1p_0  ;    
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_0  ;    
    aal_l2_cls_vlan_number_t                       vlan_number_0  ;    
    ca_boolean_t                               vlan_number_vld_0  ;    
    ca_uint32_t                                   ethernet_type_0  ;    
    ca_boolean_t                             ethernet_type_vld_0  ;    
    ca_uint32_t                                    l4_src_port_0  ;    
    ca_uint32_t                                    l4_dst_port_0  ;    
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_0  ;    
    ca_boolean_t                                   field_is_or_0  ;    
    ca_uint32_t                                      precedence_0  ;  
    ca_uint8_t                                         ecn_1      ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_mac_addr_t                                     mac_addr_1  ; 
    ca_boolean_t                                mac_addr_is_sa_1  ; 
    ca_boolean_t                                mac_addr_valid_1  ; 
    ca_uint32_t                                        ip_addr_1  ; 
    ca_boolean_t                                 ip_addr_is_sa_1  ; 
    ca_boolean_t                                 ip_addr_valid_1  ; 
    ca_uint32_t                                           ip_pt_1  ; 
    ca_boolean_t                                   ip_pt_valid_1  ; 
    ca_uint32_t                                            dscp_1  ; 
    ca_boolean_t                                    dscp_valid_1  ; 
    ca_boolean_t                                      has_ipop_1  ; 
    ca_boolean_t                                has_ipop_valid_1  ; 
    ca_boolean_t                                       l4_flag_1  ; 
    ca_boolean_t                                 l4_flag_valid_1  ; 
    ca_boolean_t                                 ipv4_fragment_1  ; 
    ca_boolean_t                           ipv4_fragment_valid_1  ; 
    ca_uint32_t                                special_pkt_code_1  ; 
    ca_boolean_t                        special_pkt_code_valid_1  ; 
    ca_uint32_t                                    top_vlan_id_1  ; 
    ca_boolean_t                                  top_is_svlan_1  ; 
    ca_boolean_t                              vlan_field_valid_1  ; 
    ca_uint32_t                                       top_dot1p_1  ; 
    ca_boolean_t                                   dot1p_valid_1  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_1  ; 
    ca_boolean_t                               vlan_number_vld_1  ; 
    ca_uint32_t                                   ethernet_type_1  ; 
    ca_boolean_t                             ethernet_type_vld_1  ; 
    ca_uint32_t                                    l4_src_port_1  ; 
    ca_uint32_t                                    l4_dst_port_1  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_1  ; 
    ca_boolean_t                                   field_is_or_1  ; 
    ca_uint32_t                                      precedence_1  ;  
} __attribute__((packed))aal_l2_cls_l3rcmr_key_t;                                             

typedef union{
    struct {                                                                
        ca_uint64_t                     valid  : 1 ;  
        ca_uint64_t                      ecn_0  : 1;             
        ca_uint64_t                ecn_valid_0  : 1;
        ca_uint64_t                mac_addr_0  : 1 ;  
        ca_uint64_t          mac_addr_is_sa_0  : 1 ;  
        ca_uint64_t          mac_addr_valid_0  : 1 ;  
        ca_uint64_t                 ip_addr_0  : 1 ;  
        ca_uint64_t           ip_addr_is_sa_0  : 1 ;  
        ca_uint64_t           ip_addr_valid_0  : 1 ;  
        ca_uint64_t                   ip_pt_0  : 1 ;  
        ca_uint64_t             ip_pt_valid_0  : 1 ;  
        ca_uint64_t                    dscp_0  : 1 ;  
        ca_uint64_t              dscp_valid_0  : 1 ;  
        ca_uint64_t                has_ipop_0  : 1 ;  
        ca_uint64_t          has_ipop_valid_0  : 1 ;  
        ca_uint64_t                 l4_flag_0  : 1 ;  
        ca_uint64_t           l4_flag_valid_0  : 1 ;  
        ca_uint64_t           ipv4_fragment_0  : 1 ;  
        ca_uint64_t     ipv4_fragment_valid_0  : 1 ;  
        ca_uint64_t        special_pkt_code_0  : 1 ;  
        ca_uint64_t  special_pkt_code_valid_0  : 1 ;  
        ca_uint64_t             top_vlan_id_0  : 1 ;  
        ca_uint64_t            top_is_svlan_0  : 1 ;  
        ca_uint64_t        vlan_field_valid_0  : 1 ;  
        ca_uint64_t               top_dot1p_0  : 1 ;  
        ca_uint64_t             dot1p_valid_0  : 1 ;  
        ca_uint64_t             vlan_number_0  : 1 ;  
        ca_uint64_t         vlan_number_vld_0  : 1 ;  
        ca_uint64_t           ethernet_type_0  : 1 ;  
        ca_uint64_t       ethernet_type_vld_0  : 1 ;  
        ca_uint64_t             l4_src_port_0  : 1 ;  
        ca_uint64_t             l4_dst_port_0  : 1 ;  
        ca_uint64_t      l4_port_field_ctrl_0  : 1 ;  
        ca_uint64_t             field_is_or_0  : 1 ;  
        ca_uint64_t              precedence_0  : 1 ;  
        ca_uint64_t                      ecn_1  : 1;             
        ca_uint64_t                ecn_valid_1  : 1;
        ca_uint64_t                mac_addr_1  : 1 ;  
        ca_uint64_t          mac_addr_is_sa_1  : 1 ;  
        ca_uint64_t          mac_addr_valid_1  : 1 ;  
        ca_uint64_t                 ip_addr_1  : 1 ;  
        ca_uint64_t           ip_addr_is_sa_1  : 1 ;  
        ca_uint64_t           ip_addr_valid_1  : 1 ;  
        ca_uint64_t                   ip_pt_1  : 1 ;  
        ca_uint64_t             ip_pt_valid_1  : 1 ;  
        ca_uint64_t                    dscp_1  : 1 ;  
        ca_uint64_t              dscp_valid_1  : 1 ;  
        ca_uint64_t                has_ipop_1  : 1 ;  
        ca_uint64_t          has_ipop_valid_1  : 1 ;  
        ca_uint64_t                 l4_flag_1  : 1 ;  
        ca_uint64_t           l4_flag_valid_1  : 1 ;  
        ca_uint64_t           ipv4_fragment_1  : 1 ;  
        ca_uint64_t     ipv4_fragment_valid_1  : 1 ;  
        ca_uint64_t        special_pkt_code_1  : 1 ;  
        ca_uint64_t  special_pkt_code_valid_1  : 1 ;  
        ca_uint64_t             top_vlan_id_1  : 1 ;  
        ca_uint64_t            top_is_svlan_1  : 1 ;  
        ca_uint64_t        vlan_field_valid_1  : 1 ;  
        ca_uint64_t               top_dot1p_1  : 1 ;  
        ca_uint64_t             dot1p_valid_1  : 1 ;  
        ca_uint64_t             vlan_number_1  : 1 ;  
        ca_uint64_t         vlan_number_vld_1  : 1 ;  
        ca_uint64_t           ethernet_type_1  : 1 ;  
        ca_uint64_t       ethernet_type_vld_1  : 1 ;  
        ca_uint64_t             l4_src_port_1  : 1 ;  
        ca_uint64_t             l4_dst_port_1  : 1 ;  
        ca_uint64_t      l4_port_field_ctrl_1  : 1 ;  
        ca_uint64_t             field_is_or_1  : 1 ;  
        ca_uint64_t              precedence_1  : 1 ;                      
    }s;
    ca_uint64_t u64;
}aal_l2_cls_cmrcmr_key_mask_t;                                                                            


typedef struct {      
    ca_boolean_t                                           valid  ; 
        ca_uint8_t                                         ecn_0      ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_mac_addr_t                                     mac_addr_0  ; 
    ca_boolean_t                                mac_addr_is_sa_0  ; 
    ca_boolean_t                                mac_addr_valid_0  ; 
    ca_uint32_t                                        ip_addr_0  ; 
    ca_boolean_t                                 ip_addr_is_sa_0  ; 
    ca_boolean_t                                 ip_addr_valid_0  ; 
    ca_uint32_t                                           ip_pt_0  ; 
    ca_boolean_t                                   ip_pt_valid_0  ; 
    ca_uint32_t                                            dscp_0  ; 
    ca_boolean_t                                    dscp_valid_0  ; 
    ca_boolean_t                                      has_ipop_0  ; 
    ca_boolean_t                                has_ipop_valid_0  ; 
    ca_boolean_t                                       l4_flag_0  ; 
    ca_boolean_t                                 l4_flag_valid_0  ; 
    ca_boolean_t                                 ipv4_fragment_0  ; 
    ca_boolean_t                           ipv4_fragment_valid_0  ; 
    ca_uint32_t                                special_pkt_code_0  ; 
    ca_boolean_t                        special_pkt_code_valid_0  ; 
    ca_uint32_t                                    top_vlan_id_0  ; 
    ca_boolean_t                                  top_is_svlan_0  ; 
    ca_boolean_t                              vlan_field_valid_0  ; 
    ca_uint32_t                                       top_dot1p_0  ; 
    ca_boolean_t                                   dot1p_valid_0  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_0  ; 
    ca_boolean_t                               vlan_number_vld_0  ; 
    ca_uint32_t                                   ethernet_type_0  ; 
    ca_boolean_t                             ethernet_type_vld_0  ; 
    ca_uint32_t                                    l4_src_port_0  ; 
    ca_uint32_t                                    l4_dst_port_0  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_0  ; 
    ca_boolean_t                                   field_is_or_0  ; 
    ca_uint32_t                                      precedence_0  ; 
        ca_uint8_t                                         ecn_1   ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_mac_addr_t                                     mac_addr_1  ; 
    ca_boolean_t                                mac_addr_is_sa_1  ; 
    ca_boolean_t                                mac_addr_valid_1  ; 
    ca_uint32_t                                        ip_addr_1  ; 
    ca_boolean_t                                 ip_addr_is_sa_1  ; 
    ca_boolean_t                                 ip_addr_valid_1  ; 
    ca_uint32_t                                           ip_pt_1  ; 
    ca_boolean_t                                   ip_pt_valid_1  ; 
    ca_uint32_t                                            dscp_1  ; 
    ca_boolean_t                                    dscp_valid_1  ; 
    ca_boolean_t                                      has_ipop_1  ; 
    ca_boolean_t                                has_ipop_valid_1  ; 
    ca_boolean_t                                       l4_flag_1  ; 
    ca_boolean_t                                 l4_flag_valid_1  ; 
    ca_boolean_t                                 ipv4_fragment_1  ; 
    ca_boolean_t                           ipv4_fragment_valid_1  ; 
    ca_uint32_t                                special_pkt_code_1  ; 
    ca_boolean_t                        special_pkt_code_valid_1  ; 
    ca_uint32_t                                    top_vlan_id_1  ; 
    ca_boolean_t                                  top_is_svlan_1  ; 
    ca_boolean_t                              vlan_field_valid_1  ; 
    ca_uint32_t                                       top_dot1p_1  ; 
    ca_boolean_t                                   dot1p_valid_1  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_1  ; 
    ca_boolean_t                               vlan_number_vld_1  ; 
    ca_uint32_t                                   ethernet_type_1  ; 
    ca_boolean_t                             ethernet_type_vld_1  ; 
    ca_uint32_t                                    l4_src_port_1  ; 
    ca_uint32_t                                    l4_dst_port_1  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_1  ; 
    ca_boolean_t                                   field_is_or_1  ; 
    ca_uint32_t                                      precedence_1  ;                                    
} __attribute__((packed))aal_l2_cls_cmrcmr_key_t;                                                                            
                                                                                                      

typedef union{                                                  
    struct {                                                    
        ca_uint64_t                        valid  : 1 ;    
        ca_uint64_t                      ecn_0  : 1;             
        ca_uint64_t                ecn_valid_0  : 1;
        ca_uint64_t                   mac_addr_0  : 1 ;    
        ca_uint64_t             mac_addr_is_sa_0  : 1 ;    
        ca_uint64_t             mac_addr_valid_0  : 1 ;    
        ca_uint64_t                    ip_addr_0  : 1 ;    
        ca_uint64_t              ip_addr_is_sa_0  : 1 ;    
        ca_uint64_t              ip_addr_valid_0  : 1 ;    
        ca_uint64_t                      ip_pt_0  : 1 ;    
        ca_uint64_t                ip_pt_valid_0  : 1 ;    
        ca_uint64_t                       dscp_0  : 1 ;    
        ca_uint64_t                 dscp_valid_0  : 1 ;    
        ca_uint64_t                   has_ipop_0  : 1 ;    
        ca_uint64_t             has_ipop_valid_0  : 1 ;    
        ca_uint64_t                    l4_flag_0  : 1 ;    
        ca_uint64_t              l4_flag_valid_0  : 1 ;    
        ca_uint64_t              ipv4_fragment_0  : 1 ;    
        ca_uint64_t        ipv4_fragment_valid_0  : 1 ;    
        ca_uint64_t           special_pkt_code_0  : 1 ;    
        ca_uint64_t     special_pkt_code_valid_0  : 1 ;    
        ca_uint64_t                top_vlan_id_0  : 1 ;    
        ca_uint64_t               top_is_svlan_0  : 1 ;    
        ca_uint64_t           vlan_field_valid_0  : 1 ;    
        ca_uint64_t                  top_dot1p_0  : 1 ;    
        ca_uint64_t                dot1p_valid_0  : 1 ;    
        ca_uint64_t                vlan_number_0  : 1 ;    
        ca_uint64_t            vlan_number_vld_0  : 1 ;    
        ca_uint64_t              ethernet_type_0  : 1 ;    
        ca_uint64_t          ethernet_type_vld_0  : 1 ;    
        ca_uint64_t                l4_src_port_0  : 1 ;    
        ca_uint64_t                l4_dst_port_0  : 1 ;    
        ca_uint64_t         l4_port_field_ctrl_0  : 1 ;    
        ca_uint64_t                field_is_or_0  : 1 ;    
        ca_uint64_t                 precedence_0  : 1 ;    
        ca_uint64_t                     mac_da_1  : 1 ;    
        ca_uint64_t                     mac_sa_1  : 1 ;    
        ca_uint64_t             mac_field_ctrl_1  : 1 ;    
        ca_uint64_t                top_vlan_id_1  : 1 ;    
        ca_uint64_t               top_is_svlan_1  : 1 ;    
        ca_uint64_t              inner_vlan_id_1  : 1 ;    
        ca_uint64_t             inner_is_svlan_1  : 1 ;    
        ca_uint64_t            vlan_field_ctrl_1  : 1 ;    
        ca_uint64_t                  top_dot1p_1  : 1 ;    
        ca_uint64_t                inner_dot1p_1  : 1 ;    
        ca_uint64_t                 dot1p_ctrl_1  : 1 ;    
        ca_uint64_t                vlan_number_1  : 1 ;    
        ca_uint64_t            vlan_number_vld_1  : 1 ;    
        ca_uint64_t              ethernet_type_1  : 1 ;    
        ca_uint64_t          ethernet_type_vld_1  : 1 ;    
        ca_uint64_t                l4_src_port_1  : 1 ;    
        ca_uint64_t                l4_dst_port_1  : 1 ;    
        ca_uint64_t         l4_port_field_ctrl_1  : 1 ;    
        ca_uint64_t                field_is_or_1  : 1 ;    
        ca_uint64_t                 precedence_1  : 1 ;           
    }s;                                                         
    ca_uint64_t u64;                                            
}aal_l2_cls_cmrl2r_key_mask_t;                                  
                                                                
                                                                       


typedef struct {      
    ca_boolean_t                                           valid  ; 
        ca_uint8_t                                         ecn_0      ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_mac_addr_t                                     mac_addr_0  ; 
    ca_boolean_t                                mac_addr_is_sa_0  ; 
    ca_boolean_t                                mac_addr_valid_0  ; 
    ca_uint32_t                                        ip_addr_0  ; 
    ca_boolean_t                                 ip_addr_is_sa_0  ; 
    ca_boolean_t                                 ip_addr_valid_0  ; 
    ca_uint32_t                                           ip_pt_0  ; 
    ca_boolean_t                                   ip_pt_valid_0  ; 
    ca_uint32_t                                            dscp_0  ; 
    ca_boolean_t                                    dscp_valid_0  ; 
    ca_boolean_t                                      has_ipop_0  ; 
    ca_boolean_t                                has_ipop_valid_0  ; 
    ca_boolean_t                                       l4_flag_0  ; 
    ca_boolean_t                                 l4_flag_valid_0  ; 
    ca_boolean_t                                 ipv4_fragment_0  ; 
    ca_boolean_t                           ipv4_fragment_valid_0  ; 
    ca_uint32_t                                special_pkt_code_0  ; 
    ca_boolean_t                        special_pkt_code_valid_0  ; 
    ca_uint32_t                                    top_vlan_id_0  ; 
    ca_boolean_t                                  top_is_svlan_0  ; 
    ca_boolean_t                              vlan_field_valid_0  ; 
    ca_uint32_t                                       top_dot1p_0  ; 
    ca_boolean_t                                   dot1p_valid_0  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_0  ; 
    ca_boolean_t                               vlan_number_vld_0  ; 
    ca_uint32_t                                   ethernet_type_0  ; 
    ca_boolean_t                             ethernet_type_vld_0  ; 
    ca_uint32_t                                    l4_src_port_0  ; 
    ca_uint32_t                                    l4_dst_port_0  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_0  ; 
    ca_boolean_t                                   field_is_or_0  ; 
    ca_uint32_t                                      precedence_0  ; 
    ca_mac_addr_t                                       mac_da_1  ;    
    ca_mac_addr_t                                       mac_sa_1  ;    
    aal_l2_cls_mac_field_ctrl_t                 mac_field_ctrl_1  ;    
    ca_uint32_t                                    top_vlan_id_1  ;    
    ca_boolean_t                                  top_is_svlan_1  ;    
    ca_uint32_t                                  inner_vlan_id_1  ;    
    ca_boolean_t                                inner_is_svlan_1  ;    
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_1  ;    
    ca_uint32_t                                       top_dot1p_1  ;    
    ca_uint32_t                                     inner_dot1p_1  ;    
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_1  ;    
    aal_l2_cls_vlan_number_t                       vlan_number_1  ;    
    ca_boolean_t                               vlan_number_vld_1  ;    
    ca_uint32_t                                   ethernet_type_1  ;    
    ca_boolean_t                             ethernet_type_vld_1  ;    
    ca_uint32_t                                    l4_src_port_1  ;    
    ca_uint32_t                                    l4_dst_port_1  ;    
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_1  ;    
    ca_boolean_t                                   field_is_or_1  ;    
    ca_uint32_t                                      precedence_1  ;                                      
} __attribute__((packed))aal_l2_cls_cmrl2r_key_t;                                                                            

 typedef union{                                                  
     struct {                                                    
         ca_uint64_t                       valid  : 1 ;          
         ca_uint64_t                      ecn_0  : 1;             
         ca_uint64_t                ecn_valid_0  : 1;
         ca_uint64_t                  mac_addr_0  : 1 ;          
         ca_uint64_t            mac_addr_is_sa_0  : 1 ;          
         ca_uint64_t            mac_addr_valid_0  : 1 ;          
         ca_uint64_t                   ip_addr_0  : 1 ;          
         ca_uint64_t             ip_addr_is_sa_0  : 1 ;          
         ca_uint64_t             ip_addr_valid_0  : 1 ;          
         ca_uint64_t                     ip_pt_0  : 1 ;          
         ca_uint64_t               ip_pt_valid_0  : 1 ;          
         ca_uint64_t                      dscp_0  : 1 ;          
         ca_uint64_t                dscp_valid_0  : 1 ;          
         ca_uint64_t                  has_ipop_0  : 1 ;          
         ca_uint64_t            has_ipop_valid_0  : 1 ;          
         ca_uint64_t                   l4_flag_0  : 1 ;          
         ca_uint64_t             l4_flag_valid_0  : 1 ;          
         ca_uint64_t             ipv4_fragment_0  : 1 ;          
         ca_uint64_t       ipv4_fragment_valid_0  : 1 ;          
         ca_uint64_t          special_pkt_code_0  : 1 ;          
         ca_uint64_t    special_pkt_code_valid_0  : 1 ;          
         ca_uint64_t               top_vlan_id_0  : 1 ;          
         ca_uint64_t              top_is_svlan_0  : 1 ;          
         ca_uint64_t          vlan_field_valid_0  : 1 ;          
         ca_uint64_t                 top_dot1p_0  : 1 ;          
         ca_uint64_t               dot1p_valid_0  : 1 ;          
         ca_uint64_t               vlan_number_0  : 1 ;          
         ca_uint64_t           vlan_number_vld_0  : 1 ;          
         ca_uint64_t             ethernet_type_0  : 1 ;          
         ca_uint64_t         ethernet_type_vld_0  : 1 ;          
         ca_uint64_t               l4_src_port_0  : 1 ;          
         ca_uint64_t               l4_dst_port_0  : 1 ;          
         ca_uint64_t        l4_port_field_ctrl_0  : 1 ;          
         ca_uint64_t               field_is_or_0  : 1 ;          
         ca_uint64_t                precedence_0  : 1 ;          
         ca_uint64_t                      ecn_1  : 1;             
         ca_uint64_t                ecn_valid_1  : 1;
         ca_uint64_t                     ip_da_1  : 1 ;
         ca_uint64_t                     ip_sa_1  : 1 ;
         ca_uint64_t                   ip_ctrl_1  : 1 ;
         ca_uint64_t                     ip_pt_1  : 1 ;
         ca_uint64_t               ip_pt_valid_1  : 1 ;
         ca_uint64_t                      dscp_1  : 1 ;
         ca_uint64_t                dscp_valid_1  : 1 ;
         ca_uint64_t                  has_ipop_1  : 1 ;
         ca_uint64_t            has_ipop_valid_1  : 1 ;
         ca_uint64_t             ipv4_fragment_1  : 1 ;
         ca_uint64_t       ipv4_fragment_valid_1  : 1 ;
         ca_uint64_t                   l4_flag_1  : 1 ;
         ca_uint64_t             l4_flag_valid_1  : 1 ;
         ca_uint64_t          special_pkt_code_1  : 1 ;
         ca_uint64_t    special_pkt_code_valid_1  : 1 ;
         ca_uint64_t               top_vlan_id_1  : 1 ;
         ca_uint64_t              top_is_svlan_1  : 1 ;
         ca_uint64_t             inner_vlan_id_1  : 1 ;
         ca_uint64_t            inner_is_svlan_1  : 1 ;
         ca_uint64_t           vlan_field_ctrl_1  : 1 ;
         ca_uint64_t                 top_dot1p_1  : 1 ;
         ca_uint64_t               inner_dot1p_1  : 1 ;
         ca_uint64_t                dot1p_ctrl_1  : 1 ;
         ca_uint64_t               vlan_number_1  : 1 ;
         ca_uint64_t           vlan_number_vld_1  : 1 ;
         ca_uint64_t             ethernet_type_1  : 1 ;
         ca_uint64_t         ethernet_type_vld_1  : 1 ;
         ca_uint64_t               l4_src_port_1  : 1 ;
         ca_uint64_t               l4_dst_port_1  : 1 ;
         ca_uint64_t        l4_port_field_ctrl_1  : 1 ;
         ca_uint64_t               field_is_or_1  : 1 ;
         ca_uint64_t                precedence_1  : 1 ;
               
     }s;                                                         
     ca_uint64_t u64;                                            
 }aal_l2_cls_cmrl3r_key_mask_t;                                  
                                                                 
                                                                        
 
 
typedef struct {      
    ca_boolean_t                                           valid  ; 
        ca_uint8_t                                         ecn_0      ;
    ca_boolean_t                                      ecn_valid_0 ;
    ca_mac_addr_t                                     mac_addr_0  ; 
    ca_boolean_t                                mac_addr_is_sa_0  ; 
    ca_boolean_t                                mac_addr_valid_0  ; 
    ca_uint32_t                                        ip_addr_0  ; 
    ca_boolean_t                                 ip_addr_is_sa_0  ; 
    ca_boolean_t                                 ip_addr_valid_0  ; 
    ca_uint32_t                                           ip_pt_0  ; 
    ca_boolean_t                                   ip_pt_valid_0  ; 
    ca_uint32_t                                            dscp_0  ; 
    ca_boolean_t                                    dscp_valid_0  ; 
    ca_boolean_t                                      has_ipop_0  ; 
    ca_boolean_t                                has_ipop_valid_0  ; 
    ca_boolean_t                                       l4_flag_0  ; 
    ca_boolean_t                                 l4_flag_valid_0  ; 
    ca_boolean_t                                 ipv4_fragment_0  ; 
    ca_boolean_t                           ipv4_fragment_valid_0  ; 
    ca_uint32_t                                special_pkt_code_0  ; 
    ca_boolean_t                        special_pkt_code_valid_0  ; 
    ca_uint32_t                                    top_vlan_id_0  ; 
    ca_boolean_t                                  top_is_svlan_0  ; 
    ca_boolean_t                              vlan_field_valid_0  ; 
    ca_uint32_t                                       top_dot1p_0  ; 
    ca_boolean_t                                   dot1p_valid_0  ; 
    aal_l2_cls_vlan_number_t                       vlan_number_0  ; 
    ca_boolean_t                               vlan_number_vld_0  ; 
    ca_uint32_t                                   ethernet_type_0  ; 
    ca_boolean_t                             ethernet_type_vld_0  ; 
    ca_uint32_t                                    l4_src_port_0  ; 
    ca_uint32_t                                    l4_dst_port_0  ; 
    aal_l2_cls_cmr_l4_port_field_ctrl_t     l4_port_field_ctrl_0  ; 
    ca_boolean_t                                   field_is_or_0  ; 
    ca_uint32_t                                      precedence_0  ; 
        ca_uint8_t                                         ecn_1      ;
    ca_boolean_t                                      ecn_valid_1 ;
    ca_uint32_t                                          ip_da_1  ;  
    ca_uint32_t                                          ip_sa_1  ;  
    aal_l2_cls_ip_ctrl_t                               ip_ctrl_1  ;  
    ca_uint32_t                                           ip_pt_1  ;  
    ca_boolean_t                                   ip_pt_valid_1  ;  
    ca_uint32_t                                            dscp_1  ;  
    ca_boolean_t                                    dscp_valid_1  ;  
    ca_boolean_t                                      has_ipop_1  ;  
    ca_boolean_t                                has_ipop_valid_1  ;  
    ca_boolean_t                                 ipv4_fragment_1  ;
    ca_boolean_t                           ipv4_fragment_valid_1  ;
    ca_boolean_t                                       l4_flag_1  ;  
    ca_boolean_t                                 l4_flag_valid_1  ;  
    ca_uint32_t                                special_pkt_code_1  ;  
    ca_boolean_t                        special_pkt_code_valid_1  ;  
    ca_uint32_t                                    top_vlan_id_1  ;  
    ca_boolean_t                                  top_is_svlan_1  ;  
    ca_uint32_t                                  inner_vlan_id_1  ;  
    ca_boolean_t                                inner_is_svlan_1  ;  
    aal_l2_cls_vlan_field_ctrl_t               vlan_field_ctrl_1  ;  
    ca_uint32_t                                       top_dot1p_1  ;  
    ca_uint32_t                                     inner_dot1p_1  ;  
    aal_l2_cls_dot1p_field_ctrl_t                   dot1p_ctrl_1  ;  
    aal_l2_cls_vlan_number_t                       vlan_number_1  ;  
    ca_boolean_t                               vlan_number_vld_1  ;  
    ca_uint32_t                                   ethernet_type_1  ;  
    ca_boolean_t                             ethernet_type_vld_1  ;  
    ca_uint32_t                                    l4_src_port_1  ;  
    ca_uint32_t                                    l4_dst_port_1  ;  
    aal_l2_cls_l4_port_field_ctrl_t         l4_port_field_ctrl_1  ;  
    ca_boolean_t                                   field_is_or_1  ;  
    ca_uint32_t                                      precedence_1  ;                                
} __attribute__((packed))aal_l2_cls_cmrl3r_key_t; 


typedef union{
    aal_l2_cls_ipv4_key_mask_t       ipv4_key;
    aal_l2_cls_ipv6_short_key_mask_t  ipv6_short_key;
    aal_l2_cls_ipv6_long_key_mask_t  ipv6_long_key;
    aal_l2_cls_l2rl2r_key_mask_t      l2rl2r_key;
    aal_l2_cls_l2rl3r_key_mask_t      l2rl3r_key;
    aal_l2_cls_l2rcmr_key_mask_t     l2rcmr_key;
    aal_l2_cls_l3rl2r_key_mask_t      l3rl2r_key;
    aal_l2_cls_l3rl3r_key_mask_t      l3rl3r_key;
    aal_l2_cls_l3rcmr_key_mask_t     l3rcmr_key;
    aal_l2_cls_cmrl2r_key_mask_t      cmrl2r_key;
    aal_l2_cls_cmrl3r_key_mask_t      cmrl3r_key;
    aal_l2_cls_cmrcmr_key_mask_t     cmrcmr_key;
} aal_l2_cls_key_mask_t;


typedef union{
    aal_l2_cls_ipv4_key_t       ipv4_key;
    aal_l2_cls_ipv6_short_key_t  ipv6_short_key;
    aal_l2_cls_ipv6_long_key_t  ipv6_long_key;
    aal_l2_cls_l2rl2r_key_t      l2rl2r_key;
    aal_l2_cls_l2rl3r_key_t      l2rl3r_key;
    aal_l2_cls_l2rcmr_key_t     l2rcmr_key;
    aal_l2_cls_l3rl2r_key_t      l3rl2r_key;
    aal_l2_cls_l3rl3r_key_t      l3rl3r_key;
    aal_l2_cls_l3rcmr_key_t     l3rcmr_key;
    aal_l2_cls_cmrl2r_key_t      cmrl2r_key;
    aal_l2_cls_cmrl3r_key_t      cmrl3r_key;
    aal_l2_cls_cmrcmr_key_t     cmrcmr_key;
} aal_l2_cls_key_t;


/*
typedef enum
{
  AAL_L2_CLS_VLAN_IVL = 0,
  AAL_L2_CLS_VLAN_SVL = 1,
  AAL_L2_CLS_VLAN_SI_MAX = AAL_L2_CLS_VLAN_SVL
} aal_l2_cls_vlan_si_t;
*/
typedef enum {
    AAL_L2_CLS_VLAN_SC_CVLAN = 0,
    AAL_L2_CLS_VLAN_SC_SVLAN = 1,
    AAL_L2_CLS_VLAN_SC_MAX = AAL_L2_CLS_VLAN_SC_SVLAN
} aal_l2_cls_vlan_sc_t;

typedef enum {
    AAL_L2_CLS_SPT_MODE_INVALID = 0,
    AAL_L2_CLS_SPT_MODE_BLOCK_LEARN = 1,
    AAL_L2_CLS_SPT_MODE_FWD_NOLEARN = 2,
    AAL_L2_CLS_SPT_MODE_FWD_LEARN = 3,
    AAL_L2_CLS_SPT_MODE_MAX
} aal_l2_cls_spt_mode_t;

typedef struct {
    ca_boolean_t          permit;
    ca_boolean_t          permit_valid;
    ca_boolean_t          premarked;
    aal_l2_cls_vlan_sc_t  top_sc_ind;
    ca_uint32_t           top_vlan_cmd;
    ca_boolean_t          top_cmd_valid;
    ca_uint32_t           top_vlanid;
    aal_l2_cls_vlan_sc_t  inner_sc_ind;
    ca_uint32_t           inner_vlan_cmd;
    ca_boolean_t          inner_cmd_valid;
    ca_uint32_t           inner_vlanid;
    ca_uint32_t            dot1p;
    ca_boolean_t          dot1p_valid;
    ca_uint32_t            dscp;
    ca_boolean_t          dscp_valid;
    ca_uint32_t            cos;
    ca_boolean_t          cos_valid;
    ca_uint32_t           dp_value;
    ca_boolean_t          dp_valid;
    ca_uint32_t           flowid;
    ca_boolean_t          mark_ena;
    ca_uint32_t            dscp_mark_down;
    aal_l2_cls_spt_mode_t spt_mode;
    ca_boolean_t          no_drop;
} __attribute__((packed))aal_l2_cls_one_fib_t;

typedef union {
    struct {
        ca_uint64_t   permit_0          : 1;
        ca_uint64_t   permit_valid_0    : 1;
        ca_uint64_t   premarked_0       : 1;
        ca_uint64_t   top_sc_ind_0      : 1;
        ca_uint64_t   top_vlan_cmd_0    : 1;
        ca_uint64_t   top_cmd_valid_0   : 1;
        ca_uint64_t   top_vlanid_0      : 1;
        ca_uint64_t   inner_sc_ind_0    : 1;
        ca_uint64_t   inner_vlan_cmd_0  : 1;
        ca_uint64_t   inner_cmd_valid_0 : 1;
        ca_uint64_t   inner_vlanid_0    : 1;
        ca_uint64_t   dot1p_0           : 1;
        ca_uint64_t   dot1p_valid_0     : 1;
        ca_uint64_t   dscp_0            : 1;
        ca_uint64_t   dscp_valid_0      : 1;
        ca_uint64_t   cos_0             : 1;
        ca_uint64_t   cos_valid_0       : 1;
        ca_uint64_t   dp_value_0        : 1;
        ca_uint64_t   dp_valid_0        : 1;
        ca_uint64_t   flowid_0          : 1;
        ca_uint64_t   mark_ena_0        : 1;
        ca_uint64_t   dscp_mark_down_0  : 1;
        ca_uint64_t   spt_mode_0        : 1;
        ca_uint64_t   no_drop_0         : 1;
        ca_uint64_t   permit_1          : 1;
        ca_uint64_t   permit_valid_1    : 1;
        ca_uint64_t   premarked_1       : 1;
        ca_uint64_t   top_sc_ind_1      : 1;
        ca_uint64_t   top_vlan_cmd_1    : 1;
        ca_uint64_t   top_cmd_valid_1   : 1;
        ca_uint64_t   top_vlanid_1      : 1;
        ca_uint64_t   inner_sc_ind_1    : 1;
        ca_uint64_t   inner_vlan_cmd_1  : 1;
        ca_uint64_t   inner_cmd_valid_1 : 1;
        ca_uint64_t   inner_vlanid_1    : 1;
        ca_uint64_t   dot1p_1           : 1;
        ca_uint64_t   dot1p_valid_1     : 1;
        ca_uint64_t   dscp_1            : 1;
        ca_uint64_t   dscp_valid_1      : 1;
        ca_uint64_t   cos_1             : 1;
        ca_uint64_t   cos_valid_1       : 1;
        ca_uint64_t   dp_value_1        : 1;
        ca_uint64_t   dp_valid_1        : 1;
        ca_uint64_t   flowid_1          : 1;
        ca_uint64_t   mark_ena_1        : 1;
        ca_uint64_t   dscp_mark_down_1  : 1;
        ca_uint64_t   spt_mode_1        : 1;
        ca_uint64_t   no_drop_1         : 1;        
        ca_uint64_t   reserve           :16;
    } s;
    ca_uint64_t u64;
}aal_l2_cls_fib_mask_t;


typedef struct {
    ca_boolean_t            permit_0;
    ca_boolean_t            permit_valid_0;
    ca_boolean_t            premarked_0;
    aal_l2_cls_vlan_sc_t    top_sc_ind_0;
    ca_uint32_t             top_vlan_cmd_0;
    ca_boolean_t            top_cmd_valid_0;
    ca_uint32_t             top_vlanid_0;
    aal_l2_cls_vlan_sc_t    inner_sc_ind_0;
    ca_uint32_t             inner_vlan_cmd_0;
    ca_boolean_t            inner_cmd_valid_0;
    ca_uint32_t             inner_vlanid_0;
    ca_uint32_t              dot1p_0;
    ca_boolean_t            dot1p_valid_0;
    ca_uint32_t              dscp_0;
    ca_boolean_t            dscp_valid_0;
    ca_uint32_t              cos_0;
    ca_boolean_t            cos_valid_0;
    ca_uint32_t             dp_value_0;
    ca_boolean_t            dp_valid_0;
    ca_uint32_t             flowid_0;
    ca_boolean_t            mark_ena_0;
    ca_uint32_t              dscp_mark_down_0;
    aal_l2_cls_spt_mode_t   spt_mode_0;
    ca_boolean_t            no_drop_0;
    ca_boolean_t            permit_1;
    ca_boolean_t            permit_valid_1;
    ca_boolean_t            premarked_1;
    aal_l2_cls_vlan_sc_t    top_sc_ind_1;
    ca_uint32_t             top_vlan_cmd_1;
    ca_boolean_t            top_cmd_valid_1;
    ca_uint32_t             top_vlanid_1;
    aal_l2_cls_vlan_sc_t    inner_sc_ind_1;
    ca_uint32_t             inner_vlan_cmd_1;
    ca_boolean_t            inner_cmd_valid_1;
    ca_uint32_t             inner_vlanid_1;
    ca_uint32_t              dot1p_1;
    ca_boolean_t            dot1p_valid_1;
    ca_uint32_t              dscp_1;
    ca_boolean_t            dscp_valid_1;
    ca_uint32_t              cos_1;
    ca_boolean_t            cos_valid_1;
    ca_uint32_t             dp_value_1;
    ca_boolean_t            dp_valid_1;
    ca_uint32_t             flowid_1;
    ca_boolean_t            mark_ena_1;
    ca_uint32_t              dscp_mark_down_1;
    aal_l2_cls_spt_mode_t   spt_mode_1;
    ca_boolean_t            no_drop_1;

} __attribute__((packed))aal_l2_cls_fib_t;

ca_status_t aal_l2_cls_ipv4_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv4_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv4_key_t * cfg);

ca_status_t aal_l2_cls_ipv4_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv4_key_t * cfg);

ca_status_t aal_l2_cls_ipv6_short_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv6_short_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv6_short_key_t * cfg);

ca_status_t aal_l2_cls_ipv6_short_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv6_short_key_t * cfg);

/*ca_status_t aal_l2_cls_ipv6_long_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv6_long_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv6_long_key_t * cfg);

ca_status_t aal_l2_cls_ipv6_long_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv6_long_key_t * cfg);*/

ca_status_t aal_l2_cls_l2rl2r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l2rl2r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l2rl2r_key_t * cfg);

ca_status_t aal_l2_cls_l2rl2r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l2rl2r_key_t * cfg);

ca_status_t aal_l2_cls_l2rl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l2rl3r_key_mask_t mask,
        CA_IN aal_l2_cls_l2rl3r_key_t *cfg);

ca_status_t aal_l2_cls_l2rl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l2rl3r_key_t *cfg);

ca_status_t aal_l2_cls_l2rcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l2rcmr_key_mask_t mask,
        CA_IN aal_l2_cls_l2rcmr_key_t *cfg);

ca_status_t aal_l2_cls_l2rcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l2rcmr_key_t *cfg);

ca_status_t aal_l2_cls_l3rl2r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rl2r_key_mask_t mask,
        CA_IN aal_l2_cls_l3rl2r_key_t *cfg);

ca_status_t aal_l2_cls_l3rl2r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rl2r_key_t *cfg);

ca_status_t aal_l2_cls_l3rl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rl3r_key_mask_t mask,
        CA_IN aal_l2_cls_l3rl3r_key_t *cfg);

ca_status_t aal_l2_cls_l3rl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rl3r_key_t *cfg);

ca_status_t aal_l2_cls_l3rcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rcmr_key_mask_t mask,
        CA_IN aal_l2_cls_l3rcmr_key_t *cfg);

ca_status_t aal_l2_cls_l3rcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rcmr_key_t *cfg);

ca_status_t aal_l2_cls_cmrl2r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrl2r_key_mask_t mask,
        CA_IN aal_l2_cls_cmrl2r_key_t *cfg);

ca_status_t aal_l2_cls_cmrl2r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrl2r_key_t *cfg);

ca_status_t aal_l2_cls_cmrl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrl3r_key_mask_t mask,
        CA_IN aal_l2_cls_cmrl3r_key_t *cfg);

ca_status_t aal_l2_cls_cmrl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrl3r_key_t *cfg);

ca_status_t aal_l2_cls_cmrcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrcmr_key_mask_t mask,
        CA_IN aal_l2_cls_cmrcmr_key_t *cfg);

ca_status_t aal_l2_cls_cmrcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrcmr_key_t *cfg);


ca_status_t aal_l2_cls_fib_set(CA_IN ca_device_id_t device_id,
                               CA_IN ca_uint32_t fib_id,
                               CA_IN aal_l2_cls_fib_mask_t mask,
                               CA_IN aal_l2_cls_fib_t * cfg);

ca_status_t aal_l2_cls_fib_get(CA_IN ca_device_id_t device_id,
                               CA_IN ca_uint32_t fib_id,
                               CA_OUT aal_l2_cls_fib_t * cfg);

ca_status_t aal_l2_cls_egr_l2rl2r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l2rl2r_key_mask_t mask,
        CA_IN aal_l2_cls_l2rl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l2rl2r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l2rl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_ipv4_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv4_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv4_key_t * cfg);

ca_status_t aal_l2_cls_egr_ipv4_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv4_key_t * cfg);


ca_status_t aal_l2_cls_egr_ipv6_short_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv6_short_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv6_short_key_t * cfg);

ca_status_t aal_l2_cls_egr_ipv6_short_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv6_short_key_t * cfg);

ca_status_t aal_l2_cls_egr_l2rl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l2rl3r_key_mask_t mask,
        CA_IN aal_l2_cls_l2rl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l2rl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l2rl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l2rcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l2rcmr_key_mask_t mask,
        CA_IN aal_l2_cls_l2rcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_l2rcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l2rcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rl2r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rl2r_key_mask_t mask,
        CA_IN aal_l2_cls_l3rl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rl2r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rl3r_key_mask_t mask,
        CA_IN aal_l2_cls_l3rl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_l3rcmr_key_mask_t mask,
        CA_IN aal_l2_cls_l3rcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_l3rcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_l3rcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrl2r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrl2r_key_mask_t mask,
        CA_IN aal_l2_cls_cmrl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrl2r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrl2r_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrl3r_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrl3r_key_mask_t mask,
        CA_IN aal_l2_cls_cmrl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrl3r_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrl3r_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrcmr_key_set(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_IN aal_l2_cls_cmrcmr_key_mask_t mask,
        CA_IN aal_l2_cls_cmrcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_cmrcmr_key_get(CA_IN ca_device_id_t device_id,
        CA_IN ca_uint32_t entry_id,
        CA_OUT aal_l2_cls_cmrcmr_key_t *cfg);

ca_status_t aal_l2_cls_egr_fib_set(CA_IN ca_device_id_t device_id,
                                   CA_IN ca_uint32_t fib_id,
                                   CA_IN aal_l2_cls_fib_mask_t mask,
                                   CA_IN aal_l2_cls_fib_t * cfg);

ca_status_t aal_l2_cls_egr_fib_get(CA_IN ca_device_id_t device_id,
                                   CA_IN ca_uint32_t fib_id,
                                   CA_OUT aal_l2_cls_fib_t * cfg);

ca_status_t aal_l2_cls_entry_status_set(
    CA_IN ca_device_id_t device_id, 
    CA_IN ca_uint32_t entry_id, 
    CA_IN aal_l2_cls_entry_status_mask_t mask,
    CA_IN aal_l2_cls_entry_status_t *key_type);


ca_status_t aal_l2_cls_entry_status_get(
    CA_IN ca_device_id_t device_id, 
    CA_IN ca_uint32_t entry_id, 
    CA_OUT aal_l2_cls_entry_status_t *key_type);


ca_status_t
aal_l2_cls_l2r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg);
ca_status_t
aal_l2_cls_l2r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg);

                          

ca_status_t
aal_l2_cls_l2r_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg);
ca_status_t
aal_l2_cls_l2r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg);

ca_status_t
aal_l2_cls_l3r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l3r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l3r_key_t        * cfg);
ca_status_t
aal_l2_cls_l3r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l3r_key_t        * cfg);

ca_status_t
aal_l2_cls_l3r_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l3r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l3r_key_t        * cfg);
ca_status_t
aal_l2_cls_l3r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l3r_key_t        * cfg);

ca_status_t
aal_l2_cls_cmr_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_cmr_key_mask_t   mask,
                          CA_IN aal_l2_cls_cmr_key_t        * cfg);
ca_status_t
aal_l2_cls_cmr_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_cmr_key_t        * cfg);

ca_status_t
aal_l2_cls_cmr_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_cmr_key_mask_t   mask,
                          CA_IN aal_l2_cls_cmr_key_t        * cfg);
ca_status_t
aal_l2_cls_cmr_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_cmr_key_t        * cfg);

ca_status_t
aal_l2_cls_egr_l2r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_l2r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg);

                          

ca_status_t
aal_l2_cls_egr_l2r_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_l2r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg);

ca_status_t
aal_l2_cls_egr_l3r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l3r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l3r_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_l3r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l3r_key_t        * cfg);

ca_status_t
aal_l2_cls_egr_l3r_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l3r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l3r_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_l3r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l3r_key_t        * cfg);

ca_status_t
aal_l2_cls_egr_cmr_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_cmr_key_mask_t   mask,
                          CA_IN aal_l2_cls_cmr_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_cmr_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_cmr_key_t        * cfg);

ca_status_t
aal_l2_cls_egr_cmr_2nd_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_cmr_key_mask_t   mask,
                          CA_IN aal_l2_cls_cmr_key_t        * cfg);
ca_status_t
aal_l2_cls_egr_cmr_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_cmr_key_t        * cfg);

ca_status_t aal_l2_cls_egr_entry_status_set(
    CA_IN ca_device_id_t device_id, 
    CA_IN ca_uint32_t entry_id, 
    CA_IN aal_l2_cls_entry_status_mask_t mask,
    CA_IN aal_l2_cls_entry_status_t *key_type);


ca_status_t aal_l2_cls_egr_entry_status_get(
    CA_IN ca_device_id_t device_id, 
    CA_IN ca_uint32_t entry_id, 
    CA_OUT aal_l2_cls_entry_status_t *key_type);


ca_status_t aal_l2_cls_debug_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_cls_debug_flag_t   *debug_flag);

ca_status_t aal_l2_cls_debug_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_cls_debug_flag_t   *debug_flag);

ca_status_t aal_l2_cls_init(ca_device_id_t device_id);

#endif
