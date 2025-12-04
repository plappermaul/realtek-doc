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
 * $Revision$
 * $Date$
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

#ifndef __DAL_APOLLOMP_L34_H__
#define __DAL_APOLLOMP_L34_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l34.h>

/*
 * Symbol Definition
 */
#define APOLLOMP_L34_HSB_WORD 10
#define APOLLOMP_L34_HSA_WORD 5

/*
 * Data Declaration
 */

typedef struct apollomp_l34_hsb_param_s
{
    uint32 hsbWords[APOLLOMP_L34_HSB_WORD];
}apollomp_l34_hsb_param_t;

typedef struct apollomp_l34_hsa_param_s
{
    uint32 hsaWords[APOLLOMP_L34_HSA_WORD];
}apollomp_l34_hsa_param_t;


typedef enum apollomp_l34_hsab_mode_s
{
    APOLLOMP_L34_HSBA_TEST_MODE  = 0,
    APOLLOMP_L34_HSBA_NO_LOG = 1,
    APOLLOMP_L34_HSBA_LOG_ALL = 2,
    APOLLOMP_L34_HSBA_LOG_FIRST_DROP = 3,
    APOLLOMP_L34_HSBA_LOG_FIRS_PASS = 4,
    APOLLOMP_L34_HSBA_LOG_FIRS_TO_CPU = 5
}apollomp_l34_hsab_mode_t;


/*
 * Symbol Definition
 */


/*
 * Function Declaration
 */



/* Module Name    : L34  */
/* Sub-module Name: Network Interface Table */

/* Function Name:
 *      dal_apollomp_l34_init
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
dal_apollomp_l34_init(void);


/* Function Name:
 *      dal_apollomp_l34_netifTable_set
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
dal_apollomp_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_netifTable_get
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
dal_apollomp_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry);


/* Sub-module Name: ARP Table */

/* Function Name:
 *      dal_apollomp_l34_arpTable_set
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
dal_apollomp_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_arpTable_get
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
dal_apollomp_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_arpTable_del
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
dal_apollomp_l34_arpTable_del(uint32 idx);



/* Sub-module Name: PPPoE Table */

/* Function Name:
 *      dal_apollomp_l34_pppoeTable_set
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
dal_apollomp_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_pppoeTable_get
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
dal_apollomp_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry);



/* Sub-module Name: Routing Table */

/* Function Name:
 *      dal_apollomp_l34_routingTable_set
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
dal_apollomp_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_routingTable_get
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
dal_apollomp_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_routingTable_del
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
dal_apollomp_l34_routingTable_del(uint32 idx);



/* Sub-module Name: Next-Hop Table */

/* Function Name:
 *      dal_apollomp_l34_nexthopTable_set
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
dal_apollomp_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_nexthopTable_get
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
dal_apollomp_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry);


/* Sub-module Name: External_Internal IP Table */

/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_set
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
dal_apollomp_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_get
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
dal_apollomp_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_del
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
dal_apollomp_l34_extIntIPTable_del(uint32 idx);


/* Sub-module Name: NAPTR  Table */

/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_set
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
dal_apollomp_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_get
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
dal_apollomp_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry);


/* Sub-module Name: NAPT  Table */

/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_set
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
dal_apollomp_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_get
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
dal_apollomp_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry);


/* Sub-module Name: IPMC Transfer Table */

/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_set
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
dal_apollomp_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);


/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_get
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
dal_apollomp_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);

/* Sub-module Name: L34 System Configure */

/* Function Name:
 *      dal_apollomp_l34_table_reset
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
dal_apollomp_l34_table_reset(rtk_l34_table_type_t type);


/* Sub-module Name: Binding Table */

/* Function Name:
 *      dal_apollomp_l34_bindingTable_set
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
dal_apollomp_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_bindingTable_get
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
dal_apollomp_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_bindingAction_set
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
dal_apollomp_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t action);


/* Function Name:
 *      dal_apollomp_l34_bindingAction_get
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
dal_apollomp_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *pAction);


/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_set
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
dal_apollomp_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_get
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
dal_apollomp_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *entry);


/* Sub-module Name: IPv6 Routing Table */


/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_set
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
dal_apollomp_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_get
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
dal_apollomp_l34_ipv6RoutingTable_get(uint32 idx, rtk_ipv6Routing_entry_t *entry);



/* Sub-module Name: IPv6 Neighbor Table */

/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_set
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
dal_apollomp_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_get
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
dal_apollomp_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);


/* Function Name:
 *      dal_apollomp_l34_hsabMode_set
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
dal_apollomp_l34_hsabMode_set(rtk_l34_hsba_mode_t hsabMode);


/* Function Name:
 *      dal_apollomp_l34_hsabMode_get
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
dal_apollomp_l34_hsabMode_get(rtk_l34_hsba_mode_t *pHsabMode);



/* Function Name:
 *      dal_apollomp_l34_hsaData_get
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
dal_apollomp_l34_hsaData_get(rtk_l34_hsa_t *pHsaData);


/* Function Name:
 *      dal_apollomp_l34_hsbData_get
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
dal_apollomp_l34_hsbData_get(rtk_l34_hsb_t *pHsbData);


/* Function Name:
 *      dal_apollomp_l34_portWanMap_set
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
dal_apollomp_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry);


/* Function Name:
 *      dal_apollomp_l34_portWanMap_get
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
dal_apollomp_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry);

/* Sub-module Name: System configuration */

/* Function Name:
 *      dal_apollomp_l34_globalState_set
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
dal_apollomp_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state);


/* Function Name:
 *      dal_apollomp_l34_globalState_get
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
dal_apollomp_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState);



/* Function Name:
 *      dal_apollomp_l34_lookupMode_set
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
dal_apollomp_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode);


/* Function Name:
 *      dal_apollomp_l34_lookupMode_get
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
dal_apollomp_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode);



/* Function Name:
 *      dal_apollomp_l34_lookupPortMap_set
 * Description:
 *      configure l34 port base mapping 
 * Input:
 * 	 portType	- port type, mac port/ext port/vc port
 *     portId	- port identity
 *	 wanIdx	- port based to wanIdx
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
dal_apollomp_l34_lookupPortMap_set(rtk_l34_portType_t portType, uint32 portId, uint32 wanIdx);


/* Function Name:
 *      dal_apollomp_l34_lookupPortMap_get
 * Description:
 *      configure l34 port base mapping 
 * Input:
 * 	 portType	- port type, mac port/ext port/vc port
 *     portId	- port identity
 * Output:
 *	 *pWanIdx	- port based to wanIdx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_l34_lookupPortMap_get(rtk_l34_portType_t portType, uint32 portId, uint32 *pWanIdx);


/* Function Name:
 *      dal_apollomp_l34_wanRoutMode_set
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
dal_apollomp_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode);


/* Function Name:
 *      dal_apollomp_l34_wanRoutMode_get
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
dal_apollomp_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode);


/* Function Name:
 *      dal_apollomp_l34_arpTrfIndicator_get
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
dal_apollomp_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator);


/* Function Name:
 *      dal_apollomp_l34_naptTrfIndicator_get
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
dal_apollomp_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator);



/* Function Name:
 *      dal_apollomp_l34_pppTrfIndicator_get
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
dal_apollomp_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator);


/* Function Name:
 *      dal_apollomp_l34_neighTrfIndicator_get
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
dal_apollomp_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator);


/* Function Name:
 *      dal_apollomp_l34_hsdState_set
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
dal_apollomp_l34_hsdState_set(rtk_enable_t hsdState);


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
dal_apollomp_l34_hsdState_get(rtk_enable_t *phsdState);


/* Function Name:
 *      dal_apollo_l34_hwL4TrfWrkTbl_set
 * Description:
 *      Set HW working table id for L4 trf.
 * Input:
 *      dal_apollo_l34_l4_trf_t l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwL4TrfWrkTbl_set(rtk_l34_l4_trf_t l4TrfTable);


/* Function Name:
 *      dal_apollo_l34_hwL4TrfWrkTbl_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      dal_apollo_l34_l4_trf_t *pl4TrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwL4TrfWrkTbl_get(rtk_l34_l4_trf_t *pl4TrfTable);

/* Function Name:
 *      dal_apollo_l34_l4TrfTb_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      l4TrfTable - table index
 *      l4EntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_l4TrfTb_get(rtk_l34_l4_trf_t l4TrfTable,uint32 l4EntryIndex,rtk_enable_t *pIndicator);


/* Function Name:
 *      dal_apollo_l34_hwL4TrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwL4TrfWrkTbl_Clear(rtk_l34_l4_trf_t l4TrfTable);


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_set
 * Description:
 *      Set HW working table id for ARP trf.
 * Input:
 *      dal_apollo_l34_arp_trf_t arpTrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwArpTrfWrkTbl_set(rtk_l34_arp_trf_t arpTrfTable);


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      None
 * Output:
 *      dal_apollo_l34_l4_trf_t *pArpTrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwArpTrfWrkTbl_get(rtk_l34_arp_trf_t *pArpTrfTable);

/* Function Name:
 *      dal_apollo_l34_arpTrfTb_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      arpTrfTable - table index
 *      arpEntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_arpTrfTb_get(rtk_l34_arp_trf_t arpTrfTable,uint32 arpEntryIndex,rtk_enable_t *pIndicator);


/* Function Name:
 *      dal_apollo_l34_hwArpTrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
dal_apollomp_l34_hwArpTrfWrkTbl_Clear(rtk_l34_arp_trf_t arpTrfTable);


/* Function Name:
 *      dal_apollomp_l34_naptTrfIndicator_get_all
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
dal_apollomp_l34_naptTrfIndicator_get_all(uint32 *pNaptMaps);


/* Function Name:
 *      dal_apollomp_l34_arpTrfIndicator_get_all
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
dal_apollomp_l34_arpTrfIndicator_get_all(uint32 *pArpMaps);

/* Function Name:
 *      dal_apollomp_l34_pppTrfIndicator_get_all
 * Description:
 *      get  ppp entry traffic indicator by index
 * Input:
 *      None
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
dal_apollomp_l34_pppTrfIndicator_get_all(rtk_l34_ppp_trf_all_t *pPppIndicator);

#endif /* __DAL_APOLLOMP_L34_H__ */

