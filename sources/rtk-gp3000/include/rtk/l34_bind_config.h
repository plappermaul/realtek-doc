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
 *           (1) L34 Binding configuration
 *           (2) L34 Port & WAN mappaing
 *           (3) L34 System Configuration
 *           (4) L34 Traffic Indicator
 *
 */

#ifndef __RTK_L34_BINDING_CONF_H__
#define __RTK_L34_BINDING_CONF_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


typedef enum rtk_l34_wanroute_act_e
{
    L34_WAN_ROUTE_FWD         = ACTION_FORWARD,
    L34_WAN_ROUTE_FWD_TO_CPU  = ACTION_TRAP2CPU,
    L34_WAN_ROUTE_DROP        = ACTION_DROP,
    L34_WAN_ROUTE_END         = ACTION_END
} rtk_l34_wanroute_act_t;



typedef enum rtk_l34_bindProto_e
{
    L34_BIND_PROTO_NOT_IPV4_IPV6 = 0,  /*for other ether type*/
    L34_BIND_PROTO_NOT_IPV6      = 1,  /*for IPv4 and other*/
    L34_BIND_PROTO_NOT_IPV4      = 2,  /*for IPv6 and other*/
    L34_BIND_PROTO_ALL           = 3,  /*for IPv4, IPv6 and other*/
    L34_BIND_PROTO_TYPE_END
} rtk_l34_bindProto_t;


/*binding table*/
typedef struct rtk_binding_entry_s {
    uint32	        wanTypeIdx;
    uint32	        vidLan;   /*VID=0: Port based binding  Others: Port-and-VLAN based binding*/
    rtk_portmask_t  portMask;
    rtk_portmask_t  extPortMask;
    rtk_l34_bindProto_t  bindProto;
} rtk_binding_entry_t;



/*WAN type table*/
typedef enum rtk_l34_wanType_e
{
    L34_WAN_TYPE_L2_BRIDGE         = 0,
    L34_WAN_TYPE_L3_ROUTE          = 1,
    L34_WAN_TYPE_L34NAT_ROUTE      = 2,
    L34_WAN_TYPE_L34_CUSTOMIZED    = 3,
    L34_WAN_TYPE_END
} rtk_l34_wanType_t;

typedef struct rtk_wanType_entry_s {
    uint32       	    nhIdx;
    rtk_l34_wanType_t	wanType;
} rtk_wanType_entry_t;




typedef enum rtk_l34_bindType_e
{
    L34_BIND_UNMATCHED_L2L3  = 0,
    L34_BIND_UNMATCHED_L2L34,
    L34_BIND_UNMATCHED_L3L2,
    L34_BIND_UNMATCHED_L3L34,
    L34_BIND_UNMATCHED_L34L2,
    L34_BIND_UNMATCHED_L34L3,
    L34_BIND_UNMATCHED_L3L3,
    L34_BIND_CUSTOMIZED_L2,
    L34_BIND_CUSTOMIZED_L3,
    L34_BIND_CUSTOMIZED_L34,
    L34_BIND_TYPE_END
} rtk_l34_bindType_t;


typedef enum rtk_l34_bindAct_e
{
	L34_BIND_ACT_DROP = 0,
	L34_BIND_ACT_TRAP,
	L34_BIND_ACT_FORCE_L2BRIDGE,
	L34_BIND_ACT_PERMIT_L2BRIDGE,
	L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP,
	L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4,
	L34_BIND_ACT_FORCE_BINDL3,
	L34_BIND_ACT_NORMAL_LOOKUPL34,
	L34_BIND_ACT_END
}rtk_l34_bindAct_t;


typedef enum rtk_l34_portWanMapAct_e{
	L34_PORTMAP_ACT_START=0,
	L34_PORTMAP_ACT_DROP = L34_PORTMAP_ACT_START,
	L34_PORTMAP_ACT_PERMIT,
	L34_PORTMAP_ACT_END
}rtk_l34_portWanMapAct_t;


typedef enum rtk_l34_portWanMapType_e{
	L34_PORTMAP_TYPE_START=0,
	L34_PORTMAP_TYPE_PORT2WAN = L34_PORTMAP_TYPE_START,
	L34_PORTMAP_TYPE_EXT2WAN,
	L34_PORTMAP_TYPE_WAN2PORT,
	L34_PORTMAP_TYPE_WAN2EXT,
	L34_PORTMAP_TYPE_END
}rtk_l34_portWanMapType_t;

typedef struct rtk_l34_portWanMap_entry_s {
	uint32 	 wanIdx;
	rtk_port_t port;
	rtk_l34_portWanMapAct_t act;
}rtk_l34_portWanMap_entry_t;



typedef enum rtk_l34_lookupMode_e{
	L34_LOOKUP_MODE_START=0,
	L34_LOOKUP_VLAN_BASE=L34_LOOKUP_MODE_START,
	L34_LOOKUP_PORT_BASE,
	L34_LOOKUP_MAC_BASE,
	L34_LOOKUP_MODE_END
}rtk_l34_lookupMode_t;

typedef enum rtk_l34_wanRouteMode_e{
	L34_WANROUTE_START=0,
	L34_WANROUTE_FORWARD=L34_WANROUTE_START,
	L34_WANROUTE_FORWARD2CPU,
	L34_WANROUTE_DROP,
	L34_WANROUTE_END
}rtk_l34_wanRouteMode_t;



typedef enum rtk_l34_portType_e{
	L34_PORT_TYPE_START=0,
	L34_PORT_MAC=L34_PORT_TYPE_START,
	L34_PORT_EXTENSION,
	L34_PORT_DSLVC,
	L34_PORT_TYPE_END
}rtk_l34_portType_t;


typedef enum rtk_l34_globalStateType_e{
	L34_GLOBAL_STATE_START=0,
	L34_GLOBAL_L34_STATE=L34_GLOBAL_STATE_START,
	L34_GLOBAL_L3NAT_STATE,
	L34_GLOBAL_L4NAT_STATE,
	L34_GLOBAL_L3CHKSERRALLOW_STATE,
	L34_GLOBAL_L4CHKSERRALLOW_STATE,
	L34_GLOBAL_NAT2LOG_STATE,
	L34_GLOBAL_TTLMINUS_STATE,
	L34_GLOBAL_FRAG2CPU_STATE,
	L34_GLOBAL_BIND_STATE,
	L34_GLOBAL_PPPKEEP_STATE,
	L34_GLOBAL_KEEP_ORG_STATE,
	L34_GLOBAL_ADV_PPPOE_MTU_CAL_STATE,
	L34_GLOBAL_V4FLOW_RT_STATE,
	L34_GLOBAL_V6FLOW_RT_STATE,
	L34_GLOBAL_IPMC_TTLMINUS_STATE,
	L34_GLOBAL_SIP_ARP_TRF_STATE,
	L34_GLOBAL_DIP_ARP_TRF_STATE,
	L34_GLOBAL_STATE_END
}rtk_l34_globalStateType_t;


typedef enum rtk_l34_dsliteCtrlType_e{
	L34_DSLITE_CTRL_START = 0,
	L34_DSLITE_CTRL_DSLITE_STATE = L34_DSLITE_CTRL_START,
	L34_DSLITE_CTRL_MC_PREFIX_UNMATCH,
	L34_DSLITE_CTRL_DS_UNMATCH_ACT,
	L34_DSLITE_CTRL_IP6_NH_ACTION,
	L34_DSLITE_CTRL_IP6_FRAGMENT_ACTION,
	L34_DSLITE_CTRL_IP4_FRAGMENT_ACTION,
	L34_DSLITE_CTRL_END
}rtk_l34_dsliteCtrlType_t;

typedef enum rtk_l34_lutMissAct_e{
	L34_LUTMISS_ACT_START = 0,
	L34_LUTMISS_ACT_DROP = L34_LUTMISS_ACT_START,
	L34_LUTMISS_ACT_TRAP,
	L34_LUTMISS_ACT_END
}rtk_l34_lutMissAct_t;

#define PPP_TRF_MAX_OF_WORD 1

typedef struct rtk_l34_ppp_trf_all_s
{
	uint32 trf_state[PPP_TRF_MAX_OF_WORD];
}rtk_l34_ppp_trf_all_t;
/*
 * Function Declaration
 */


/* Sub-module Name: Binding Table */

/* Function Name:
 *      rtk_l34_bindingTable_set
 * Description:
 *      Set binding table
 * Input:
 *      idx - index of binding table
 *      *bindEntry - point of binding data
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
rtk_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *bindEntry);


/* Function Name:
 *      rtk_l34_bindingTable_get
 * Description:
 *      Get binding table
 * Input:
 *      idx - index of binding table
 * Output:
 *      *bindEntry - point of binding data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *bindEntry);


/* Function Name:
 *      rtk_l34_bindingAction_set
 * Description:
 *     Set binding action
 * Input:
 *      bindType - binding type
 *      bindAction - binding action
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
rtk_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t bindAction);


/* Function Name:
 *      rtk_l34_bindingAction_get
 * Description:
 *      Get binding action
 * Input:
 *      bindType - binding type
 * Output:
 *      *bindAction - binding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *bindAction);


/* Function Name:
 *      rtk_l34_wanTypeTable_set
 * Description:
 *      Set WAN type entry by idx.
 * Input:
 *      idx - index of wan type table for binding
 *      *wanTypeEntry - point of wan type table entry
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
rtk_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *wanTypeEntry);


/* Function Name:
 *      rtk_l34_wanTypeTable_get
 * Description:
 *      Get WAN type entry by idx.
 * Input:
 *      idx - index of wan type table for binding
 * Output:
 *      *wanTypeEntry - point of wan type table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *wanTypeEntry);



/* Sub-module Name: Port WAN Mapping */

/* Function Name:
 *      rtk_l34_portWanMap_set
 * Description:
 *      Set L34 WAN interface port mapping
 * Input:
 *      portWanMapType - port wan mapping type
 *	  portWanMapEntry - port wan mapping entry
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
rtk_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry);


/* Function Name:
 *      rtk_l34_portWanMap_get
 * Description:
 *      Get L34 WAN interface port mapping
 * Input:
 *      portWanMapType -  port wan mapping type
 * Output:
 *	  pPortWanMapEntry -  point of port wan mapping entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry);



/* Sub-module Name: System configuration */

/* Function Name:
 *      rtk_l34_globalState_set
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
rtk_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state);


/* Function Name:
 *      rtk_l34_globalState_get
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
rtk_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState);



/* Function Name:
 *      rtk_l34_lookupMode_set
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
rtk_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode);


/* Function Name:
 *      rtk_l34_lookupMode_get
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
rtk_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode);



/* Function Name:
 *      rtk_l34_lookupPortMap_set
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
rtk_l34_lookupPortMap_set(rtk_l34_portType_t portType, uint32 portId, uint32 wanIdx);


/* Function Name:
 *      rtk_l34_lookupPortMap_get
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
rtk_l34_lookupPortMap_get(rtk_l34_portType_t portType, uint32 portId, uint32 *pWanIdx);


/* Function Name:
 *      rtk_l34_wanRoutMode_set
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
rtk_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode);


/* Function Name:
 *      rtk_l34_wanRoutMode_get
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
rtk_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode);


/* Sub-module Name: Traffic Indicator */


/* Function Name:
 *      rtk_l34_arpTrfIndicator_get
 * Description:
 *      get  arp entry traffic indicator by index
 * Input:
 *      index - traffic table index
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
rtk_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator);


/* Function Name:
 *      rtk_l34_naptTrfIndicator_get
 * Description:
 *      get  napt entry traffic indicator by index
 * Input:
 *      index - traffic table index
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
rtk_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator);



/* Function Name:
 *      rtk_l34_pppTrfIndicator_get
 * Description:
 *      get  ppp entry traffic indicator by index
 * Input:
 *      index - traffic table index
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
rtk_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator);


/* Function Name:
 *      rtk_l34_neighTrfIndicator_get
 * Description:
 *      get  ipv6 neighbor entry traffic indicator by index
 * Input:
 *      index - traffic table index
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
rtk_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator);


/* Function Name:
 *      rtk_l34_naptTrfIndicator_get_all
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
rtk_l34_naptTrfIndicator_get_all(uint32 *pNaptMaps);


/* Function Name:
 *      rtk_l34_arpTrfIndicator_get_all
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
rtk_l34_arpTrfIndicator_get_all(uint32 *pArpMaps);

/* Function Name:
 *      rtk_l34_pppTrfIndicator_get_all
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
rtk_l34_pppTrfIndicator_get_all(rtk_l34_ppp_trf_all_t *pPppIndicator);

#endif /* __RTK_L34_BINDING_CONF_H__ */
