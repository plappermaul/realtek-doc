/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of L34  API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) L34 Networking Interface configuration
 *           (2) L34 Routing Table configuration
 *           (3) L34 ARP Table configuration
 *           (4) L34 NAPT connection configuration
 *           (5) L34 System configuration
 *           (6) L34 NAPTR configuration
 *           (7) L34 NEXT-HOP configuration
 *           (8) L34 External_Internal IP configuration
 *           (9) L34 Binding configuration
 *           (10) L34 IPv6 configuration
 *
 */

#ifndef __DAL_RTL9602C_L34_H__
#define __DAL_RTL9602C_L34_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l34.h>

/*
 * Symbol Definition
 */

#define RTL9602C_L34_HSB_WORD 10
#define RTL9602C_L34_HSA_WORD 5
#define RTL9602C_L3MCR_IDX_MAX       0X7
#define RTL9602C_IPTRANS_IDX_MAX     0Xf


/*
 * Data Declaration
 */

typedef struct rtl9602c_l34_hsb_param_s
{
    uint32 hsbWords[RTL9602C_L34_HSB_WORD];
}rtl9602c_l34_hsb_param_t;

typedef struct rtl9602c_l34_hsa_param_s
{
    uint32 hsaWords[RTL9602C_L34_HSA_WORD];
}rtl9602c_l34_hsa_param_t;


typedef enum rtl9602c_l34_hsba_mode_e
{
    RTL9602C_L34_HSBA_TEST_MODE  = 0,
    RTL9602C_L34_HSBA_NO_LOG = 1,
    RTL9602C_L34_HSBA_LOG_ALL = 2,
    RTL9602C_L34_HSBA_LOG_FIRST_DROP = 3,
    RTL9602C_L34_HSBA_LOG_FIRST_PASS = 4,
    RTL9602C_L34_HSBA_LOG_FIRST_TO_CPU = 5
}rtl9602c_l34_hsba_mode_t;

typedef enum rtl9602c_raw_l34_hsa_pppoe_act_e
{
    RTL9602C_RAW_L34_HSA_PPPOE_INACT      = 0,
    RTL9602C_RAW_L34_HSA_PPPOE_TAGGING    = 1,
    RTL9602C_RAW_L34_HSA_PPPOE_REMOVE     = 2,
    RTL9602C_RAW_L34_HSA_PPPOE_MODIFY     = 3
} rtl9602c_raw_l34_hsa_pppoe_act_t;

typedef enum rtl9602c_l34_limbc_e
{
    RTL9602C_L34_LIMBC_VLAN_BASED  = 0,
    RTL9602C_L34_LIMBC_RESERVED  = 1,
    RTL9602C_L34_LIMBC_MAC_BASED   = 2,
    RTL9602C_L34_LIMBC_RESERVED_2    = 3
} rtl9602c_l34_limbc_t;

typedef enum rtl9602c_l34_mode_e
{
    RTL9602C_L34_L3_DISABLE_L4_DISABLE  = 0,
    RTL9602C_L34_L3_ENABLE_L4_DISABLE  =  1,
    RTL9602C_L34_L3_DISABLE_L4_ENABLE  =  2,
    RTL9602C_L34_L3_ENABLE_L4_ENABLE   =  3,
    RTL9602C_L34_L3_MODE_END
} rtl9602c_l34_mode_t;

typedef enum rtl9602c_l34_pppoeKeepMode_e
{
    RTL9602C_L34_PPPOE_BY_ASIC  = 0,
    RTL9602C_L34_PPPOE_KEEP     = 1,
    RTL9602C_L34_PPPOE_END      = 3
} rtl9602c_l34_pppoeKeepMode_t;


typedef enum rtl9602c_rtk_l34_wanroute_act_e
{
    RTL9602C_L34_WAN_ROUTE_FWD         = 0,
    RTL9602C_L34_WAN_ROUTE_FWD_TO_CPU  = 1,
    RTL9602C_L34_WAN_ROUTE_DROP        = 2,
    RTL9602C_L34_WAN_ROUTE_END
} rtl9602c_rtk_l34_wanroute_act_t;

typedef enum rtl9602c_raw_l34_bindL2L3UnmatchAct_e
{
    RTL9602C_UNMATCHED_L2L3_DROP            = 0,
    RTL9602C_UNMATCHED_L2L3_TRAP            = 1,
    RTL9602C_UNMATCHED_L2L3_FORCE_L2Bridge  = 2,
    RTL9602C_UNMATCHED_L2L3_END
} rtl9602c_l34_bindL2L3UnmatchAct_t;


typedef enum rtl9602c_raw_l34_bindL2L34UnmatchAct_e
{
    RTL9602C_UNMATCHED_L2L34_DROP            = 0,
    RTL9602C_UNMATCHED_L2L34_TRAP            = 1,
    RTL9602C_UNMATCHED_L2L34_FORCE_L2BRIDGE  = 2,
    RTL9602C_UNMATCHED_L2L34_END
} rtl9602c_l34_bindL2L34UnmatchAct_t;

typedef enum rtl9602c_raw_l34_bindL3L2UnmatchAct_e
{
    RTL9602C_UNMATCHED_L3L2_DROP             = 0,
    RTL9602C_UNMATCHED_L3L2_TRAP             = 1,
    RTL9602C_UNMATCHED_L3L2_PERMIT_L2BRIDGE  = 2,
    RTL9602C_UNMATCHED_L3L2_END
} rtl9602c_l34_bindL3L2UnmatchAct_t;


typedef enum rtl9602c_raw_l34_bindL3L34UnmatchAct_e
{
    RTL9602C_UNMATCHED_L3L34_TRAP             = 0,
    RTL9602C_UNMATCHED_L3L34_FORCE_L4         = 1,
    RTL9602C_UNMATCHED_L3L34_END
} rtl9602c_l34_bindL3L34UnmatchAct_t;


typedef enum rtl9602c_raw_l34_bindL34L2UnmatchAct_e
{
    RTL9602C_UNMATCHED_L34L2_DROP             = 0,
    RTL9602C_UNMATCHED_L34L2_TRAP             = 1,
    RTL9602C_UNMATCHED_L34L2_PERMIT_L2BRIDGE  = 2,
    RTL9602C_UNMATCHED_L34L2_END
} rtl9602c_l34_bindL34L2UnmatchAct_t;


typedef enum rtl9602c_raw_l34_bindL34L3UnmatchAct_e
{
    RTL9602C_UNMATCHED_L34L3_TRAP             = 0,
    RTL9602C_UNMATCHED_L34L3_FORCE_L3         = 1,
    RTL9602C_UNMATCHED_L34L3_END
} rtl9602c_l34_bindL34L3UnmatchAct_t;



typedef enum rtl9602c_raw_l34_bindL3L3UnmatchAct_e
{
    RTL9602C_UNMATCHED_L3L3_FORCE_L3         = 0,
    RTL9602C_UNMATCHED_L3L3_TRAP             = 1,
    RTL9602C_UNMATCHED_L3L3_END
} rtl9602c_l34_bindL3L3UnmatchAct_t;



typedef enum rtl9602c_raw_l34_bindCusL2UnmatchAct_e
{
    RTL9602C_UNMATCHED_CUSL2_PERMIT_L2_BRIDGE = 0,
    RTL9602C_UNMATCHED_CUSL2_DROP             = 1,
    RTL9602C_UNMATCHED_CUSL2_TRAP             = 2,
    RTL9602C_UNMATCHED_CUSL2_END
} rtl9602c_l34_bindCusL2UnmatchAct_t;



typedef enum rtl9602c_raw_l34_bindCusL3UnmatchAct_e
{
    RTL9602C_UNMATCHED_CUSL3_FORCE_L3         = 0,
    RTL9602C_UNMATCHED_CUSL3_DROP             = 1,
    RTL9602C_UNMATCHED_CUSL3_TRAP             = 2,
    RTL9602C_UNMATCHED_CUSL3_FORCE_L4         = 3,
    RTL9602C_UNMATCHED_CUSL3_END
} rtl9602c_l34_bindCusL3UnmatchAct_t;

typedef enum rtl9602c_raw_l34_bindCusL34UnmatchAct_e
{
    RTL9602C_UNMATCHED_CUSL34_NORMAL_L34       = 0,
    RTL9602C_UNMATCHED_CUSL34_DROP             = 1,
    RTL9602C_UNMATCHED_CUSL34_TRAP             = 2,
    RTL9602C_UNMATCHED_CUSL34_FORCE_L3         = 3,
    RTL9602C_UNMATCHED_CUSL34_END
} rtl9602c_l34_bindCusL34UnmatchAct_t;

typedef enum rtl9602c_raw_l34_flowRouteType_e
{
    RTL9602C_L34_FLOWRT_TYPE_CPU     		= 0,
    RTL9602C_L34_FLOWRT_TYPE_DROP         = 1,
    RTL9602C_L34_FLOWRT_TYPE_LOCAL        = 2,
    RTL9602C_L34_FLOWRT_TYPE_GLOBAL       = 3,
    RTL9602C_L34_FLOWRT_TYPE_END
} rtl9602c_raw_l34_flowRouteType_t;

typedef enum rtl9602c_raw_l34_ipmc_pppoeAct_e
{
    RTL9602C_L34_IPMC_PPPOE_ACT_NO     = 0,
    RTL9602C_L34_IPMC_PPPOE_ACT_KEEP   = 1,
    RTL9602C_L34_IPMC_PPPOE_ACT_REMOVE = 2,
    RTL9602C_L34_IPMC_PPPOE_ACT_MODIFY = 3,
    RTL9602C_L34_IPMC_PPPOE_ACT_END
} rtl9602c_raw_l34_ipmc_pppoeAct_t;


typedef struct rtl9602c_raw_l34_hsa_s
{
	uint32  dslite_Iix;
	uint32  dslite_valid;
	uint32  is_policy;
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
	uint32  dvid;
    rtl9602c_raw_l34_hsa_pppoe_act_t  pppoe_if;
    uint32  pppid_idx;
    uint32  nexthop_mac_idx;
    uint32  l4_chksum;
    uint32  l3_chksum;
    uint32  port;
    ipaddr_t ip;
}rtl9602c_raw_l34_hsa_t;

/*
 * Symbol Definition
 */


/*
 * Function Declaration
 */



/* Module Name    : L34  */
/* Sub-module Name: Network Interface Table */

/* Function Name:
 *      dal_rtl9602c_l34_init
 * Description:
 *      Initialize l34 module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Must initialize l34  module before calling any l34  APIs.
 */
extern int32
dal_rtl9602c_l34_init(void);


/* Function Name:
 *      dal_rtl9602c_l34_netifTable_set
 * Description:
 *      Set netif table entry
 * Input:
 *      idx - index of netif table
 *      *entry -point of netif entry 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_netifTable_get
 * Description:
 *      Get netif table entry
 * Input:
 *      idx - index of netif table
 * Output:
 *      *entry -point of netif entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry);


/* Sub-module Name: ARP Table */

/* Function Name:
 *      dal_rtl9602c_l34_arpTable_set
 * Description:
 *      Set arp table entry
 * Input:
 *      idx - index of arp table
 *      *entry -point of arp entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_arpTable_get
 * Description:
 *      Get arp table entry
 * Input:
 *      idx - index of arp table
 * Output:
 *      *entry -point of arp entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_arpTable_del
 * Description:
 *      Delete arp table entry
 * Input:
 *      idx - index of arp table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_arpTable_del(uint32 idx);



/* Sub-module Name: PPPoE Table */

/* Function Name:
 *      dal_rtl9602c_l34_pppoeTable_set
 * Description:
 *      Set PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 *      *entry -point of PPPoE entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_pppoeTable_get
 * Description:
 *      Get PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 * Output:
 *      *entry -point of PPPoE entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry);



/* Sub-module Name: Routing Table */

/* Function Name:
 *      dal_rtl9602c_l34_routingTable_set
 * Description:
 *      Set Routing table entry
 * Input:
 *      idx - index of Routing table
 *      *entry -point of Routing entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_routingTable_get
 * Description:
 *      Get Routing table entry
 * Input:
 *      idx - index of Routing table
 * Output:
 *      *entry -point of Routing entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_routingTable_del
 * Description:
 *      Delete arp Routing entry
 * Input:
 *      idx - index of Routing table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_routingTable_del(uint32 idx);



/* Sub-module Name: Next-Hop Table */

/* Function Name:
 *      dal_rtl9602c_l34_nexthopTable_set
 * Description:
 *      Set  Next-Hop table entry
 * Input:
 *      idx - index of  Next-Hop table
 *      *entry -point of  Next-Hop entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_nexthopTable_get
 * Description:
 *      Get Next-Hop table entry
 * Input:
 *      idx - index of Next-Hop table
 * Output:
 *      *entry -point of Next-Hop entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry);


/* Sub-module Name: External_Internal IP Table */

/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_set
 * Description:
 *      Set  External_Internal IP table entry
 * Input:
 *      idx - index of  External_Internal IP table
 *      *entry -point of External_Internal IP entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_get
 * Description:
 *      Get External_Internal IP table entry
 * Input:
 *      idx - index of External_Internal IP table
 * Output:
 *      *entry -point of External_Internal IP entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_extIntIPTable_del
 * Description:
 *      Delete arp  External_Internal entry
 * Input:
 *      idx - index of  External_Internal table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_extIntIPTable_del(uint32 idx);


/* Sub-module Name: NAPTR  Table */

/* Function Name:
 *      dal_rtl9602c_l34_naptInboundTable_set
 * Description:
 *      Set  NAPTR table entry
 * Input:
 *      forced - force set to NAPTR table
 *      *entry - value of NAPTR table entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_naptInboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of NAPTR table
 * Output:
 *      *entry -point of NAPTR entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry);


/* Sub-module Name: NAPT  Table */

/* Function Name:
 *      dal_rtl9602c_l34_naptOutboundTable_set
 * Description:
 *      Set  napt table entry
 * Input:
 *      forced - force set to napt table
 *      *entry - value of napt table entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_naptOutboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of napt table
 * Output:
 *      *entry -point of napt entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry);


/* Sub-module Name: IPMC Transfer Table */

/* Function Name:
 *      dal_rtl9602c_l34_ipmcTransTable_set
 * Description:
 *      Set  IPMC Transfer table entry
 * Input:
 *      forced - force set to IPMC Transfer table
 *      *pEntry - value of  IPMC Transfer entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);


/* Function Name:
 *      dal_rtl9602c_l34_ipmcTransTable_get
 * Description:
 *      Get IPMC Transfer table entry
 * Input:
 *      idx - index of IPMC Transfer table
 * Output:
 *      *entry -point of IPMC Transfer entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);

/* Sub-module Name: L34 System Configure */

/* Function Name:
 *      dal_rtl9602c_l34_table_reset
 * Description:
 *      Reset a specific  L34 table entries
 * Input:
 *      type - L34 Table type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_table_reset(rtk_l34_table_type_t type);


/* Sub-module Name: Binding Table */

/* Function Name:
 *      dal_rtl9602c_l34_bindingTable_set
 * Description:
 *      Set binding table
 * Input:
 *      idx - index of binding table
 *      *entry - point of binding data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_bindingTable_get
 * Description:
 *      Get binding table
 * Input:
 *      idx - index of binding table
 * Output:
 *      *entry - point of binding data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_bindingAction_set
 * Description:
 *     Set binding action
 * Input:
 *      bindType - binding type
 *      action - binding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t action);


/* Function Name:
 *      dal_rtl9602c_l34_bindingAction_get
 * Description:
 *      Get binding action
 * Input:
 *      bindType - binding type
 * Output:
 *      *pAction - binding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *pAction);


/* Function Name:
 *      dal_rtl9602c_l34_wanTypeTable_set
 * Description:
 *      Set WAN type entry by idx.
 * Input:
 *      idx - index of wan type table for binding
 *      *entry - point of wan type table entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_wanTypeTable_get
 * Description:
 *      Get WAN type entry by idx.
 * Input:
 *      idx - index of wan type table for binding
 * Output:
 *      *entry - point of wan type table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *entry);


/* Sub-module Name: IPv6 Routing Table */


/* Function Name:
 *      dal_rtl9602c_l34_ipv6RoutingTable_set
 * Description:
 *      Set a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 *      *entry - point of ipv6 routing table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_ipv6RoutingTable_get
 * Description:
 *      Get a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 * Output:
 *      *entry - point of ipv6 routing table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipv6RoutingTable_get(uint32 idx, rtk_ipv6Routing_entry_t *entry);



/* Sub-module Name: IPv6 Neighbor Table */

/* Function Name:
 *      dal_rtl9602c_l34_ipv6NeighborTable_set
 * Description:
 *      Set neighbor table
 * Input:
 *      idx - index of neighbor table
 *      *entry - point of neighbor data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_ipv6NeighborTable_get
 * Description:
 *      Get neighbor table
 * Input:
 *      idx - index of neighbor table
 * Output:
 *      *entry - point of neighbor data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);


/* Function Name:
 *      dal_rtl9602c_l34_hsabMode_set
 * Description:
 *      Set L34 hsab mode
 * Input:
 *      hsabMode - L34 hsab 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsbaMode_set(rtk_l34_hsba_mode_t hsabMode);


/* Function Name:
 *      dal_rtl9602c_l34_hsabMode_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      *pHsabMode - point of L34 hsab 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsbaMode_get(rtk_l34_hsba_mode_t *pHsabMode);



/* Function Name:
 *      dal_rtl9602c_l34_hsaData_get
 * Description:
 *      Get L34 hsa data
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsaData_get(rtk_l34_hsa_t *pHsaData);


/* Function Name:
 *      dal_rtl9602c_l34_hsbData_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsbData_get(rtk_l34_hsb_t *pHsbData);


/* Function Name:
 *      dal_rtl9602c_l34_portWanMap_set
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      portWanMapType: port wan mapping type
 *	  portWanMapEntry: port wan mapping entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry);


/* Function Name:
 *      dal_rtl9602c_l34_portWanMap_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      portWanMapType: port wan mapping type
 * Output:
 *	  pPortWanMapEntry: point of port wan mapping entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry);

/* Sub-module Name: System configuration */

/* Function Name:
 *      dal_rtl9602c_l34_globalState_set
 * Description:
 *      get l34 global status
 * Input:
 * 	  stateType	-status type
 *	  state		- status of state type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state);


/* Function Name:
 *      dal_rtl9602c_l34_globalState_get
 * Description:
 *      set l34 global status
 * Input:
 * 	  stateType	-status type
 * Output:
 *	  pState		- status of state type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState);



/* Function Name:
 *      dal_rtl9602c_l34_lookupMode_set
 * Description:
 *      configure l34 lookup mode selection
 * Input:
 * 	  lookupMode	- mode of l34 lookup method
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode);


/* Function Name:
 *      dal_rtl9602c_l34_lookupMode_get
 * Description:
 *      get l34 lookup mode selection
 * Input:
 *      None
 * Output:
 * 	  pLookupMode	- point of mode of l34 lookup method
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode);


/* Function Name:
 *      dal_rtl9602c_l34_wanRoutMode_set
 * Description:
 *      set wan route mode 
 * Input:
 * 	  wanRouteMode	- mode of wan routed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode);


/* Function Name:
 *      dal_rtl9602c_l34_wanRoutMode_get
 * Description:
 *      get  wan route mode 
 * Input:
 *      None
 * Output:
 * 	  *pWanRouteMode	- point of mode of wan routed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode);


/* Function Name:
 *      dal_rtl9602c_l34_arpTrfIndicator_get
 * Description:
 *      get  arp entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pArpIndicator	- point of traffic indicator for arp 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator);


/* Function Name:
 *      dal_rtl9602c_l34_naptTrfIndicator_get
 * Description:
 *      get  napt entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pNaptIndicator	- point of traffic indicator for arp 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator);



/* Function Name:
 *      dal_rtl9602c_l34_pppTrfIndicator_get
 * Description:
 *      get  ppp entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pPppIndicator	- point of traffic indicator for pppoe table 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator);


/* Function Name:
 *      dal_rtl9602c_l34_neighTrfIndicator_get
 * Description:
 *      get  ipv6 neighbor entry traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pNeighIndicator	- point of traffic indicator for neighbor 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator);


/* Function Name:
 *      dal_rtl9602c_l34_hsdState_set
 * Description:
 *      Set L34 hsd state
 * Input:
 *      hsdState - L34 hsd state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsdState_set(rtk_enable_t hsdState);


/* Function Name:
 *      dal_apollo_l34_hsdState_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      phsdState - point of hsd state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_hsdState_get(rtk_enable_t *phsdState);


/* Function Name:
 *      dal_rtl9602c_l34_naptTrfIndicator_get_all
 * Description:
 *      get  napt entry traffic indicator
 * Input:
 *      None
 * Output:
 * 	  *pNaptMaps	- point of traffic indicator for mask (64*32bit mask)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_naptTrfIndicator_get_all(uint32 *pNaptMaps);


/* Function Name:
 *      dal_rtl9602c_l34_arpTrfIndicator_get_all
 * Description:
 *      get  all arp entry traffic indicator
 * Input:
 *      None
 * Output:
 * 	  *pArpMaps	- point of traffic indicator for arp 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_arpTrfIndicator_get_all(uint32 *pArpMaps);
/* Function Name:
 *      dal_rtl9602c_l34_ip6mcRoutingTransIdx_set
 * Description:
 *      Set per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 *      ipmcTransIdx - ip multicast translation table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32 dal_rtl9602c_l34_ip6mcRoutingTransIdx_set(uint32 idx, rtk_port_t port, uint32 ipmcTransIdx);

/* Function Name:
 *      dal_rtl9602c_l34_ip6mcRoutingTransIdx_get
 * Description:
 *      Get per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 * Output:
 *      pIpmcTransIdx - ip multicast translation table index to be get
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32 dal_rtl9602c_l34_ip6mcRoutingTransIdx_get(uint32 idx, rtk_port_t port, uint32 *pIpmcTransIdx);

/* Function Name:
 *		dal_rtl9602c_l34_flowRouteTable_set
 * Description:
 *		Set Flow Routing Table.
 * Input:
 *		entry  : entry content
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NOT_INIT
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_ENTRY_INDEX
 *		RT_ERR_VLAN_VID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
extern int32 
dal_rtl9602c_l34_flowRouteTable_set(rtk_l34_flowRoute_entry_t *pEntry);

/* Function Name:
 *		dal_rtl9602c_l34_flowRouteTable_get
 * Description:
 *		Get Flow Routing Table.
 * Input:
 *		None
 * Output:
 *		entry  : entry content
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NOT_INIT
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_ENTRY_INDEX
 *		RT_ERR_VLAN_VID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
extern int32 
dal_rtl9602c_l34_flowRouteTable_get(rtk_l34_flowRoute_entry_t *pEntry);

/* Function Name:
 *      dal_rtl9602c_l34_flowTrfIndicator_get
 * Description:
 *      get flow routing traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pFlowIndicator	- point of traffic indicator for flow routing traffic
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_flowTrfIndicator_get(uint32 index, rtk_enable_t *pFlowIndicator);

extern int32
dal_rtl9602c_l34_dsliteInfTable_set(rtk_l34_dsliteInf_entry_t *pEntry);
extern int32
dal_rtl9602c_l34_dsliteInfTable_get(rtk_l34_dsliteInf_entry_t *pEntry);
extern int32
dal_rtl9602c_l34_dsliteMcTable_set(rtk_l34_dsliteMc_entry_t *pEntry);
extern int32
dal_rtl9602c_l34_dsliteMcTable_get(rtk_l34_dsliteMc_entry_t *pEntry);

/* Function Name:
 *		dal_rtl9602c_l34_dsliteControl_set
 * Description:
 *		Set DS-Lite Control
 * Input:
 *		ctrlType - control type
 *		act - action
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NOT_INIT
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_ENTRY_INDEX
 *		RT_ERR_VLAN_VID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
extern int32
dal_rtl9602c_l34_dsliteControl_set(rtk_l34_dsliteCtrlType_t ctrlType, uint32 act);

/* Function Name:
 *		dal_rtl9602c_l34_dsliteControl_get
 * Description:
 *		Get DS-Lite Control
 * Input:
 *		ctrlType - control type
 * Output:
 *		pAct - action
 * Return:
 *		RT_ERR_OK
 *		RT_ERR_FAILED
 *		RT_ERR_NOT_INIT
 *		RT_ERR_NULL_POINTER
 *		RT_ERR_ENTRY_INDEX
 *		RT_ERR_VLAN_VID
 *		RT_ERR_INPUT
 * Note:
 *		None
 */
extern int32
dal_rtl9602c_l34_dsliteControl_get(rtk_l34_dsliteCtrlType_t ctrlType, uint32 *pAct);

/* Function Name:
 *      dal_rtl9602c_l34_mib_get
 * Description:
 *      Get per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      counters   - counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_mib_get(rtk_l34_mib_t *pL34Cnt);

/* Function Name:
 *      dal_rtl9602c_l34_mib_reset
 * Description:
 *      Reset per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_mib_reset(uint32 ifIndex);

/* Function Name:
 *      dal_rtl9602c_l34_lutLookupMiss_set
 * Description:
 *      set L34 MAC table lookup miss action
 * Input:
 * 	  lutMissAct	- L34 MAC table lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_lutLookupMiss_set(rtk_l34_lutMissAct_t lutMissAct);

/* Function Name:
 *      dal_rtl9602c_l34_lutLookupMiss_get
 * Description:
 *      Get L34 MAC table lookup miss action
 * Input:
 * 	    None
 * Output:
 *      lutMissAct	- L34 MAC table lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_l34_lutLookupMiss_get(rtk_l34_lutMissAct_t *pLutMissAct);
#endif /* __DAL_RTL9602C_L34_H__ */

