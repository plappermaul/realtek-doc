#ifndef _APOLLOMP_RAW_L34_H_
#define _APOLLOMP_RAW_L34_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
#include <rtk/l34.h>
/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef enum apollomp_raw_l34_hsb_type_e
{
    APOLLOMP_RAW_L34_HSB_ETHERNET      = 0,
    APOLLOMP_RAW_L34_HSB_RESERVED_PPTP = 1,
    APOLLOMP_RAW_L34_HSB_IPV4          = 2,
    APOLLOMP_RAW_L34_HSB_ICMP          = 3,
    APOLLOMP_RAW_L34_HSB_IGMP          = 4,
    APOLLOMP_RAW_L34_HSB_TCP           = 5,
    APOLLOMP_RAW_L34_HSB_UDP           = 6,
    APOLLOMP_RAW_L34_HSB_IPV6          = 7
} apollomp_raw_l34_hsb_type_t;


typedef enum apollomp_raw_l34_hsa_pppoe_act_e
{
    APOLLOMP_RAW_L34_HSA_PPPOE_INACT      = 0,
    APOLLOMP_RAW_L34_HSA_PPPOE_TAGGING    = 1,
    APOLLOMP_RAW_L34_HSA_PPPOE_REMOVE     = 2,
    APOLLOMP_RAW_L34_HSA_PPPOE_MODIFY     = 3
} apollomp_raw_l34_hsa_pppoe_act_t;


typedef enum apollomp_raw_l34_hsba_mode_e
{
    APOLLOMP_RAW_L34_HSBA_BOTH_LOG         = 0,
    APOLLOMP_RAW_L34_HSBA_NO_LOG           = 1,
    APOLLOMP_RAW_L34_HSBA_LOG_ALL          = 2,
    APOLLOMP_RAW_L34_HSBA_LOG_FIRST_DROP   = 3,
    APOLLOMP_RAW_L34_HSBA_LOG_FIRST_PASS   = 4,
    APOLLOMP_RAW_L34_HSBA_LOG_FIRST_TO_CPU = 5
} apollomp_raw_l34_hsba_mode_t;






typedef enum apollomp_raw_l34_limbc_e
{
    APOLLOMP_RAW_L34_LIMBC_VLAN_BASED  = 0,
    APOLLOMP_RAW_L34_LIMBC_PORT_BASED  = 1,
    APOLLOMP_RAW_L34_LIMBC_MAC_BASED   = 2,
    APOLLOMP_RAW_L34_LIMBC_RESERVED    = 3
} apollomp_raw_l34_limbc_t;





typedef enum apollomp_raw_l34_pppoeKeepMode_e
{
    APOLLOMP_RAW_L34_PPPOE_BY_ASIC  = 0,
    APOLLOMP_RAW_L34_PPPOE_KEEP     = 1,
    APOLLOMP_RAW_L34_PPPOE_END      = 3
} apollomp_raw_l34_pppoeKeepMode_t;



typedef enum apollomp_raw_l34_mode_e
{
    APOLLOMP_RAW_L34_L3_DISABLE_L4_DISABLE  = 0,
    APOLLOMP_RAW_L34_L3_ENABLE_L4_DISABLE  =  1,
    APOLLOMP_RAW_L34_L3_DISABLE_L4_ENABLE  =  2,
    APOLLOMP_RAW_L34_L3_ENABLE_L4_ENABLE   =  3,
    APOLLOMP_RAW_L34_L3_MODE_END
} apollomp_raw_l34_mode_t;


typedef enum apollomp_raw_l34_l4_trf_e
{
    APOLLOMP_RAW_L34_L4TRF_TABLE0  = 0,
    APOLLOMP_RAW_L34_L4TRF_TABLE1  = 1
} apollomp_raw_l34_l4_trf_t;


typedef enum apollomp_raw_l34_arp_trf_e
{
    APOLLOMP_RAW_L34_ARPTRF_TABLE0  = 0,
    APOLLOMP_RAW_L34_ARPTRF_TABLE1  = 1
} apollomp_raw_l34_arp_trf_t;


typedef enum apollomp_raw_l34_bindL2L3UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L2L3_DROP            = 0,
    APOLLOMP_UNMATCHED_L2L3_TRAP            = 1,
    APOLLOMP_UNMATCHED_L2L3_FORCE_L2Bridge  = 2,
    APOLLOMP_UNMATCHED_L2L3_END
} apollomp_l34_bindL2L3UnmatchAct_t;


typedef enum apollomp_raw_l34_bindL2L34UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L2L34_DROP            = 0,
    APOLLOMP_UNMATCHED_L2L34_TRAP            = 1,
    APOLLOMP_UNMATCHED_L2L34_FORCE_L2BRIDGE  = 2,
    APOLLOMP_UNMATCHED_L2L34_END
} apollomp_l34_bindL2L34UnmatchAct_t;

typedef enum apollomp_raw_l34_bindL3L2UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L3L2_DROP             = 0,
    APOLLOMP_UNMATCHED_L3L2_TRAP             = 1,
    APOLLOMP_UNMATCHED_L3L2_PERMIT_L2BRIDGE  = 2,
    APOLLOMP_UNMATCHED_L3L2_END
} apollomp_l34_bindL3L2UnmatchAct_t;


typedef enum apollomp_raw_l34_bindL3L34UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L3L34_TRAP             = 0,
    APOLLOMP_UNMATCHED_L3L34_FORCE_L4         = 1,
    APOLLOMP_UNMATCHED_L3L34_END
} apollomp_l34_bindL3L34UnmatchAct_t;


typedef enum apollomp_raw_l34_bindL34L2UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L34L2_DROP             = 0,
    APOLLOMP_UNMATCHED_L34L2_TRAP             = 1,
    APOLLOMP_UNMATCHED_L34L2_PERMIT_L2BRIDGE  = 2,
    APOLLOMP_UNMATCHED_L34L2_END
} apollomp_l34_bindL34L2UnmatchAct_t;


typedef enum apollomp_raw_l34_bindL34L3UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L34L3_TRAP             = 0,
    APOLLOMP_UNMATCHED_L34L3_FORCE_L3         = 1,
    APOLLOMP_UNMATCHED_L34L3_END
} apollomp_l34_bindL34L3UnmatchAct_t;



typedef enum apollomp_raw_l34_bindL3L3UnmatchAct_e
{
    APOLLOMP_UNMATCHED_L3L3_FORCE_L3         = 0,
    APOLLOMP_UNMATCHED_L3L3_TRAP             = 1,
    APOLLOMP_UNMATCHED_L3L3_END
} apollomp_l34_bindL3L3UnmatchAct_t;



typedef enum apollomp_raw_l34_bindCusL2UnmatchAct_e
{
    APOLLOMP_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE = 0,
    APOLLOMP_UNMATCHED_CUSL2_DROP             = 1,
    APOLLOMP_UNMATCHED_CUSL2_TRAP             = 2,
    APOLLOMP_UNMATCHED_CUSL2_END
} apollomp_l34_bindCusL2UnmatchAct_t;



typedef enum apollomp_raw_l34_bindCusL3UnmatchAct_e
{
    APOLLOMP_UNMATCHED_CUSL3_FORCE_L3         = 0,
    APOLLOMP_UNMATCHED_CUSL3_DROP             = 1,
    APOLLOMP_UNMATCHED_CUSL3_TRAP             = 2,
    APOLLOMP_UNMATCHED_CUSL3_FORCE_L4         = 3,
    APOLLOMP_UNMATCHED_CUSL3_END
} apollomp_l34_bindCusL3UnmatchAct_t;

typedef enum apollomp_raw_l34_bindCusL34UnmatchAct_e
{
    APOLLOMP_UNMATCHED_CUSL34_NORMAL_L34       = 0,
    APOLLOMP_UNMATCHED_CUSL34_DROP             = 1,
    APOLLOMP_UNMATCHED_CUSL34_TRAP             = 2,
    APOLLOMP_UNMATCHED_CUSL34_FORCE_L3         = 3,
    APOLLOMP_UNMATCHED_CUSL34_END
} apollomp_l34_bindCusL34UnmatchAct_t;



typedef enum apollomp_l34_bindType_e
{
    APOLLOMP_L34_BIND_UNMATCHED_L2L3  = 0,
    APOLLOMP_L34_BIND_UNMATCHED_L2L34 = 1,
    APOLLOMP_L34_BIND_UNMATCHED_L3L2  = 2,
    APOLLOMP_L34_BIND_UNMATCHED_L3L34 = 3,
    APOLLOMP_L34_BIND_UNMATCHED_L34L2 = 4,
    APOLLOMP_L34_BIND_UNMATCHED_L34L3 = 5,
    APOLLOMP_L34_BIND_UNMATCHED_L3L3  = 6,
    APOLLOMP_L34_BIND_CUSTOMIZED_L2   = 7,
    APOLLOMP_L34_BIND_CUSTOMIZED_L3   = 8,
    APOLLOMP_L34_BIND_CUSTOMIZED_L34  = 9,
    APOLLOMP_L34_BIND_TYPE_END
} apollomp_l34_bindType_t;



typedef struct apollomp_raw_l34_hsb_s
{
    uint32  isFromWan;
    uint32  l2bridge;
    uint32  ipfrag_s;
    uint32  ipmf;
    uint32  l4_chksum_ok;
    uint32  l3_chksum_ok;
    uint32  cpu_direct_tx;
    uint32  udp_no_chksum;
    uint32  parse_fail;
    uint32  pppoe_if;
    uint32  svlan_if;
    uint32  ttls;   /*0b00 TTL=0, 0b01 TTL=1, 0b10 TTL>1 */
    apollomp_raw_l34_hsb_type_t  type;
    uint32   tcp_flag;
    uint32  cvlan_if;
    uint32   spa;
    uint32  cvid;
    uint32  len;
    uint32  dport_l4chksum;
    uint32  pppoe_id;
    rtk_ipv6_addr_t dip;
    ipaddr_t sip;
    uint32  sport_icmpid_chksum;
    rtk_mac_t  dmac;
}apollomp_raw_l34_hsb_t;


typedef struct apollomp_raw_l34_hsa_s
{
    uint32  pppoeKeep;
    uint32  bindVidTrans;
    uint32  interVlanIf;
    uint32  reason;
    uint32  l34trans;
    uint32  l2trans;
    uint32  action;
    uint32  l4_pri_valid;
    uint32  l4_pri_sel;
    uint32  frag;
    uint32  difid;
    apollomp_raw_l34_hsa_pppoe_act_t  pppoe_if;
    uint32  pppid_idx;
    uint32  nexthop_mac_idx;
    uint32  l4_chksum;
    uint32  l3_chksum;
    uint32  dvid;
    uint32  port;
    ipaddr_t ip;
}apollomp_raw_l34_hsa_t;



typedef struct apollomp_raw_l34_ipmcTransEntry_s
{
    uint32  index;
    uint32  netifIdx;
    uint32  sipTransEnable;
    uint32  extipIdx;
    uint32  pppoeAct;
    uint32  pppoeIdx;
}apollomp_raw_l34_ipmcTransEntry_t;


extern int32 apollomp_raw_l34_hsba_mode_get(apollomp_raw_l34_hsba_mode_t *mode);
extern int32 apollomp_raw_l34_hsba_mode_set(apollomp_raw_l34_hsba_mode_t mode);
extern int32 apollomp_raw_l34_hsbaBusyFlag_get(uint32 *flag);
extern int32 apollomp_raw_l34_hsbaActiveFlag_set(uint32 flag);
extern int32 apollomp_raw_l34_hsbaActiveFlag_get(uint32 *flag);

extern int32 apollomp_raw_l34_hsaData_set(apollomp_raw_l34_hsa_t *hsaData);
extern int32 apollomp_raw_l34_hsaData_get(apollomp_raw_l34_hsa_t *hsaData);
extern int32 apollomp_raw_l34_hsbData_set(apollomp_raw_l34_hsb_t *hsbData);
extern int32 apollomp_raw_l34_hsbData_get(apollomp_raw_l34_hsb_t *hsbData);

extern int32 apollomp_raw_l34_pppoeTrf_get(uint32 *pppoeTrf);

extern int32 apollomp_raw_l34_portIntfIdx_get(rtk_port_t port,uint8 *idx);
extern int32 apollomp_raw_l34_portIntfIdx_set(rtk_port_t port,uint8 idx);
extern int32 apollomp_raw_l34_extPortIntfIdx_get(rtk_port_t port,uint8 *idx);
extern int32 apollomp_raw_l34_extPortIntfIdx_set(rtk_port_t port,uint8 idx);
extern int32 apollomp_raw_l34_vcPortIntfIdx_get(rtk_port_t port,uint8 *idx);
extern int32 apollomp_raw_l34_vcPortIntfIdx_set(rtk_port_t port,uint8 idx);

extern int32 apollomp_raw_l34_debugMonitor_set(rtk_enable_t enable);
extern int32 apollomp_raw_l34_debugMonitor_get(rtk_enable_t *enable);
extern int32 apollomp_raw_l34_natAttack2CPU_set(rtk_enable_t enable);
extern int32 apollomp_raw_l34_natAttack2CPU_get(rtk_enable_t *enable);
extern int32 apollomp_raw_l34_wanRouteAct_set(rtk_l34_wanroute_act_t act);
extern int32 apollomp_raw_l34_wanRouteAct_get(rtk_l34_wanroute_act_t *act);
extern int32 apollomp_raw_l34_LanIntfMDBC_set(apollomp_raw_l34_limbc_t mode);
extern int32 apollomp_raw_l34_LanIntfMDBC_get(apollomp_raw_l34_limbc_t *mode);
extern int32 apollomp_raw_l34_mode_set(apollomp_raw_l34_mode_t mode);
extern int32 apollomp_raw_l34_mode_get(apollomp_raw_l34_mode_t *mode);
extern int32 apollomp_raw_l34_TtlOperationMode_set(rtk_enable_t mode);
extern int32 apollomp_raw_l34_TtlOperationMode_get(rtk_enable_t *mode);
extern int32 apollomp_raw_l34_L3chksumErrAllow_set(rtk_enable_t mode);
extern int32 apollomp_raw_l34_L3chksumErrAllow_get(rtk_enable_t *mode);
extern int32 apollomp_raw_l34_L4chksumErrAllow_set(rtk_enable_t mode);
extern int32 apollomp_raw_l34_L4chksumErrAllow_get(rtk_enable_t *mode);
extern int32 apollomp_raw_l34_FragPkt2Cpu_set(rtk_enable_t mode);
extern int32 apollomp_raw_l34_FragPkt2Cpu_get(rtk_enable_t *mode);

extern int32 apollomp_raw_l34_hwL4TrfWrkTbl_set(apollomp_raw_l34_l4_trf_t table);
extern int32 apollomp_raw_l34_hwL4TrfWrkTbl_get(apollomp_raw_l34_l4_trf_t *table);
extern int32 apollomp_raw_l34_hwArpTrfWrkTbl_set(apollomp_raw_l34_arp_trf_t table);
extern int32 apollomp_raw_l34_hwArpTrfWrkTbl_get(apollomp_raw_l34_arp_trf_t *table);
extern int32 apollomp_raw_l34_hwArpTrfWrkTbl_Clear(apollomp_raw_l34_arp_trf_t table);
extern int32 apollomp_raw_l34_hwL4TrfWrkTbl_Clear(apollomp_raw_l34_l4_trf_t table);

extern int32 apollomp_raw_l34_portToWanAction_set(rtk_port_t port, uint32 wanIdx, rtk_action_t action);
extern int32 apollomp_raw_l34_portToWanAction_get(rtk_port_t port, uint32 wanIdx, rtk_action_t *action);
extern int32 apollomp_raw_l34_extPortToWanAction_set(rtk_port_t extPort, uint32 wanIdx, rtk_action_t action);
extern int32 apollomp_raw_l34_extPortToWanAction_get(rtk_port_t extPort, uint32 wanIdx, rtk_action_t *action);
extern int32 apollomp_raw_l34_wanToPortAction_set(uint32 wanIdx, rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l34_wanToPortAction_get(uint32 wanIdx, rtk_port_t port, rtk_action_t *action);
extern int32 apollomp_raw_l34_wanToExtPortAction_set(uint32 wanIdx, rtk_port_t extPort, rtk_action_t action);
extern int32 apollomp_raw_l34_wanToExtPortAction_get(uint32 wanIdx, rtk_port_t extPort, rtk_action_t *action);

extern int32 apollomp_raw_l34_ipmcTransEntry_set(apollomp_raw_l34_ipmcTransEntry_t *entry);
extern int32 apollomp_raw_l34_ipmcTransEntry_get(apollomp_raw_l34_ipmcTransEntry_t *entry);

extern int32 apollomp_raw_l34_ipmcTtlFunction_set(rtk_enable_t enable);
extern int32 apollomp_raw_l34_ipmcTtlFunction_get(rtk_enable_t *enable);


extern int32 apollomp_raw_l34_bindFunction_set(rtk_enable_t enable);
extern int32 apollomp_raw_l34_bindFunction_get(rtk_enable_t *enable);


extern int32 apollomp_raw_l34_bindingAction_set(apollomp_l34_bindType_t type, uint32 action);
extern int32 apollomp_raw_l34_bindingAction_get(apollomp_l34_bindType_t type, uint32 *action);

extern int32
apollomp_raw_l34_globalFunction_get(rtk_enable_t *enable);

extern int32
apollomp_raw_l34_globalFunction_set(rtk_enable_t enable);

extern int32
apollomp_raw_l34_pppoeKeep_get(apollomp_raw_l34_pppoeKeepMode_t *mode);

extern int32 
apollomp_raw_l34_pppoeKeep_set(apollomp_raw_l34_pppoeKeepMode_t mode);

extern int32
apollomp_raw_l34_l4TrfTb_get(apollomp_raw_l34_l4_trf_t table,uint32 index,rtk_enable_t *pIndicator);

extern int32 
apollomp_raw_l34_arpTrfTb_get(apollomp_raw_l34_arp_trf_t table,uint32 index,rtk_enable_t *pIndicator);

#endif /*#ifndef _APOLLOMP_RAW_L34_H_*/

