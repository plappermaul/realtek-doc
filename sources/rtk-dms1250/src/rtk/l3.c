/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public Layer3 routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Layer3 routing error handling
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/l3.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_l3_init
 * Description:
 *      Initialize L3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 * Changes:
 *      None
 */
int32
rtk_l3_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_init(unit);
} /* end of rtk_l3_init */

/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

/* Function Name:
 *      rtk_l3_routeEntry_get
 * Description:
 *      Get L3 routing entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 * Output:
 *      pEntry - L3 route entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The route entry is indexed by L2 NextHop entry.
 *          Valid index range:
 *          - 0~2047 in 8390
 *          - 0~511 in 8380
 *      (2) L3 chip architecture be changed for 9300/9310, should reference other API
 * Changes:
 *      None
 */
int32
rtk_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routeEntry_get(unit, index, pEntry);
}

/* Function Name:
 *      rtk_l3_routeEntry_set
 * Description:
 *      Set L3 routing entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 *      pEntry - L3 route entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The route entry is indexed by L2 NextHop entry.
 *          Valid index range:
 *          - 0~2047 in 8390
 *          - 0~511 in 8380
 *      (2) L3 chip architecture be changed for 9300/9310, should reference other API
 * Changes:
 *      None
 */
int32
rtk_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routeEntry_set(unit, index, pEntry);
}

/* Function Name:
 *      rtk_l3_routeSwitchMacAddr_get
 * Description:
 *      Get routing switch MAC address.
 * Input:
 *      unit  - unit id
 *      index - index of switch MAC address
 * Output:
 *      pMac  - pointer to switch MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Multiple switch MAC addresses are supported for unicast routing.
 *          Valid index value is 0~15 in 8390.
 *      (2) L3 chip architecture be changed for 9300/9310, The API implement should use 'rtk_l3_intf_get'
 * Changes:
 *      None
 */
int32
rtk_l3_routeSwitchMacAddr_get(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routeSwitchMacAddr_get(unit, index, pMac);
} /* end of rtk_l3_routeSwitchMacAddr_get */

/* Function Name:
 *      rtk_l3_routeSwitchMacAddr_set
 * Description:
 *      Set routing switch MAC address.
 * Input:
 *      unit  - unit id
 *      index - index of switch MAC address
 *      pMac  - pointer to switch MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Multiple switch MAC addresses are supported for unicast routing.
 *          Valid index value is 0~15 in 8390.
 *      (2) L3 chip architecture be changed for 9300/9310,
 *           The API implement should use 'rtk_l3_intf_create' or 'rtk_l3_intf_set'
 * Changes:
 *      None
 */
int32
rtk_l3_routeSwitchMacAddr_set(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routeSwitchMacAddr_set(unit, index, pMac);
} /* end of rtk_l3_routeSwitchMacAddr_set */


/* Function Name:
 *      rtk_l3_info_t_init
 * Description:
 *      Initialize a rtk_l3_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to L3 information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_info_t_init(rtk_l3_info_t *pInfo)
{
    /* function body */

    return RT_ERR_OK;
}   /* end of rtk_l3_info_t_init */


/* Function Name:
 *      rtk_l3_info_get
 * Description:
 *      Get L3-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to L3 information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_info_get(unit, pInfo);
}   /* end of rtk_l3_info_get */


/* Function Name:
 *      rtk_l3_routerMacEntry_get
 * Description:
 *      Get an router MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - index of router MAC address
 * Output:
 *      pEntry - router MAC entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routerMacEntry_get(unit, index, pEntry);
}   /* end of rtk_l3_routerMacEntry_get */

/* Function Name:
 *      rtk_l3_routerMacEntry_set
 * Description:
 *      Set an router MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - index of router MAC address
 *      pEntry - router MAC entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) This API is used to manage Router MAC table by caller.
 *          While using this API to maintain Router MAC table,
 *          to create an L3 interface by calling rtk_l3_intf_create()
 *          SHOULD set with RTK_L3_INTF_FLAG_MAC_MANUAL flag and
 *          DO NOT use rtk_l3_vrrp_add() simultaneously.
 *      (2) MAC entry usage in rtk_l3_info_t which can get by rtk_l3_info_get()
 *          is NOT counted while using this API to add an entry.
 * Changes:
 *      None
 */
int32
rtk_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_routerMacEntry_set(unit, index, pEntry);
}   /* end of rtk_l3_routerMacEntry_set */

/* Function Name:
 *      rtk_l3_intf_t_init
 * Description:
 *      Initialize a rtk_l3_intf_t_init structure
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to L3 interface
 * Output:
 *      pIntf - pointer to L3 interface (initialized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_t_init(rtk_l3_intf_t *pIntf)
{
    /* function body */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    osal_memset(pIntf, 0, sizeof(rtk_l3_intf_t));
    pIntf->intf_id = -1;    /* Not assigned */
    pIntf->vrf_id = RTK_DEFAULT_L3_INTF_VRF_ID;
    pIntf->vid = RTK_DEFAULT_L3_INTF_VID;
    pIntf->mtu = RTK_DEFAULT_L3_INTF_MTU;
    pIntf->ipv6_mtu = RTK_DEFAULT_L3_INTF_MTU;
    pIntf->ttl = RTK_DEFAULT_L3_INTF_TTL_SCOPE;

    return RT_ERR_OK;
}   /* end of rtk_l3_intf_t_init */


/* Function Name:
 *      rtk_l3_intf_create
 * Description:
 *      Create a new L3 interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to L3 interface containing the input parameters
 * Output:
 *      pIntf - pointer to L3 interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_VLAN_VID                 - invalid VLAN ID
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 *      RT_ERR_MTU_EXCEED               - interface MTU is too big (over the maximum)
 *      RT_ERR_TTL_EXCEED               - interface TTL is over the maximum
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - vlan entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pIntf as input:
 *          mac_addr, vid. (VID=0 means not associated with a VLAN)
 *      (2) Optional: to create an L3 interface with specified interface ID
 *          call with RTK_L3_INTF_FLAG_WITH_ID set and intf_id will be refered.
 *      (3) Optional: to create an L3 interface without allocating a Router MAC entry.
 *          call with RTK_L3_INTF_FLAG_MAC_MANUTL set and
 *          using rtk_l3_routerMac_set() API to manage Router MAC entry for MAC terminate.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intf_create(unit, pIntf);
}   /* end of rtk_l3_intf_create */


/* Function Name:
 *      rtk_l3_intf_destroy
 * Description:
 *      Destroy an L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intf_destroy(unit, intfId);
}   /* end of rtk_l3_intf_destroy */


/* Function Name:
 *      rtk_l3_intf_destroyAll
 * Description:
 *      Destroy all L3 interfaces
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_destroyAll(uint32 unit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intf_destroyAll(unit);
}   /* end of rtk_l3_intf_destroyAll */


/* Function Name:
 *      rtk_l3_intf_get
 * Description:
 *      Get an L3 interface by interface ID/VID/MAC+VID.
 * Input:
 *      unit  - unit id
 *      type  - search key type
 *      pIntf - pointer to L3 interface (interface id, mac_addr, and vid)
 * Output:
 *      pIntf - pointer to L3 interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intf_get(unit, type, pIntf);
}   /* end of rtk_l3_intf_get */


/* Function Name:
 *      rtk_l3_intf_set
 * Description:
 *      Set an L3 interface by interface ID/VID/MAC+VID.
 * Input:
 *      unit  - unit id
 *      type  - search key type
 *      pIntf - pointer to L3 interface (interface id, mac_addr, and vid)
 * Output:
 *      pIntf - pointer to L3 interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 *      RT_ERR_MTU_EXCEED               - interface MTU is over the maximum
 *      RT_ERR_TTL_EXCEED               - interface TTL is over the maximum
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - vlan entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intf_set(unit, type, pIntf);
}   /* end of rtk_l3_intf_set */

/* Function Name:
 *      rtk_l3_intfStats_get
 * Description:
 *      Get statistic counters of the specified L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - interface id
 * Output:
 *      pStats - pointer to the statistic data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
rtk_l3_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intf_stats_t *pStats)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intfStats_get(unit, intfId, pStats);
}   /* end of rtk_l3_intfStats_get */

/* Function Name:
 *      rtk_l3_intfStats_reset
 * Description:
 *      Reset statistic counters of the specified L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
rtk_l3_intfStats_reset(uint32 unit, rtk_intf_id_t intfId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intfStats_reset(unit, intfId);
}   /* end of rtk_l3_intfStats_reset */


/* Function Name:
 *      rtk_l3_vrrp_add
 * Description:
 *      Add a VRRP MAC address to the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      vrid - VRRP ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_EXCEEDS_CAPACITY   - exceed the hardware capacity
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) VRRP MAC address is build as { 00:00:5E:00, flags, VRID }.
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
int32
rtk_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_vrrp_add(unit, flags, vid, vrId);
}   /* end of rtk_l3_vrrp_add */

/* Function Name:
 *      rtk_l3_vrrp_del
 * Description:
 *      Delete a VRRP MAC address from the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      vrid - VRRP ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
int32
rtk_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_vrrp_del(unit, flags, vid, vrId);
}   /* end of rtk_l3_vrrp_del */

/* Function Name:
 *      rtk_l3_vrrp_delAll
 * Description:
 *      Delete all VRRP MAC addresses of the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
int32
rtk_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_vrrp_delAll(unit, flags, vid);
}   /* end of rtk_l3_vrrp_delAll */

/* Function Name:
 *      rtk_l3_vrrp_get
 * Description:
 *      Get all VRIDs of the specified VLAN
 * Input:
 *      unit          - unit id
 *      vid           - VLAN ID
 *      vrIdArraySize - size of allocated entries in pVrIdArray
 * Output:
 *      pVrIdArray    - array of VRIDs
 *      pVrIdCount    - number of entries of VRID actually filled in.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
int32
rtk_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_vrrp_get(unit, flags, vid, vrIdArraySize, pVrIdArray, pVrIdCount);
}   /* end of rtk_l3_vrrp_get */


/* Function Name:
 *      rtk_l3_nextHop_t_init
 * Description:
 *      Initialize a rtk_l3_nextHop_t structure
 * Input:
 *      unit     - unit id
 *      pNextHop - pointer to L3 nexthop info (uninitilized)
 * Output:
 *      pNextHop - pointer to L3 nexthop info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_nextHop_t_init(rtk_l3_nextHop_t *pNextHop)
{
    /* function body */
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(pNextHop, 0, sizeof(rtk_l3_nextHop_t));

    return RT_ERR_OK;
}   /* end of rtk_l3_nextHop_t_init */


/* Function Name:
 *      rtk_l3_nextHop_create
 * Description:
 *      Create an L3 nexthop and get the returned path ID
 * Input:
 *      unit     - unit id
 *      flags    - optional flags (RTK_L3_FLAG_XXX)
 *      pNextHop - pointer to nexthop
 * Output:
 *      pPathId  - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_REPLACE
 *          RTK_L3_FLAG_WITH_ID
 *          RTK_L3_FLAG_WITH_NH_DMAC
 *      (2) If the flag RTK_L3_FLAG_REPLACE is set, then replace the existing entry
 *          with the new info based on the input path ID (nhId).
 *          Otherwise, SDK will allocate a path ID for this new nexthop entry.
 *      (3) RTK_L3_FLAG_WITH_NH_DMAC is only supported in 9300. If the flag RTK_L3_FLAG_WITH_NH_DMAC is not set, pNextHop->mac_addr for adding l2 entry and dmac entry;
 *          If the flag RTK_L3_FLAG_WITH_NH_DMAC is set, pNextHop->mac_addr for adding l2 entry,  pNextHop->nh_mac_addr for dmac entry.
 *          In microsoft NLB, Routed packet with unicast DMAC need forward to multiport.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_nextHop_create(unit, flags, pNextHop, pNhId);
}   /* end of rtk_l3_nextHop_create */


/* Function Name:
 *      rtk_l3_nextHop_destroy
 * Description:
 *      Destroy an L3 Next-Hop
 * Input:
 *      unit   - unit id
 *      pathId - pointer to L3 path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t nhId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_nextHop_destroy(unit, nhId);
}   /* end of rtk_l3_nextHop_destroy */


/* Function Name:
 *      rtk_l3_nextHop_get
 * Description:
 *      Get an L3 Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - L3 path ID
 * Output:
 *      pNextHop - pointer to nexthop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t nhId,  rtk_l3_nextHop_t *pNextHop)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_nextHop_get(unit, nhId, pNextHop);
}   /* end of rtk_l3_nextHop_get */


/* Function Name:
 *      rtk_l3_nextHopPath_find
 * Description:
 *      Find an path ID pointing to a nexthop
 * Input:
 *      unit     - unit id
 *      pNextHop - pointer to nexthop
 * Output:
 *      pPathId  - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_nextHopPath_find(unit, pNextHop, pNhId);
}   /* end of rtk_l3_nextHopPath_find */


/* Function Name:
 *      rtk_l3_ecmp_create
 * Description:
 *      Create an ECMP path (contains one or more nexthop paths)
 * Input:
 *      unit       - unit id
 *      flags      - optional flags (REPLACE flag for updating)
 *      pathCnt    - size of the allocated array
 *      pIntfArray - pointer to the path ID array
 *      pEcmpIntf  - pointer to the ECMP path ID
 * Output:
 *      pEcmpIntf  - pointer to the ECMP path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL        - table is full
 *      RT_ERR_EXCEED_CAPACITY - exceed the hardware capacity
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_create(uint32 unit, rtk_l3_flag_t flags, uint32 nhIdCnt, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_create(unit, flags, nhIdCnt, pNhIdArray, pEcmpId);
}   /* end of rtk_l3_ecmp_create */


/* Function Name:
 *      rtk_l3_ecmp_destroy
 * Description:
 *      Destroy an ECMP path
 * Input:
 *      unit   - unit id
 *      ecmpId - ECMP path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_destroy(unit, pathId);
}   /* end of rtk_l3_ecmp_destroy */


/* Function Name:
 *      rtk_l3_ecmp_get
 * Description:
 *      Get all path IDs of an ECMP path
 * Input:
 *      unit          - unit id
 *      ecmpPathId    - ECMP path ID
 *      nhIdArraySize - size of allocated entries in pIntf_array
 * Output:
 *      pNhIdArray    - array of ECMP path IDs
 *      pNhIdCount    - number of entries of intf_count actually filled in.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_get(uint32 unit, rtk_l3_pathId_t ecmpId, uint32 nhIdArraySize, rtk_l3_pathId_t *pNhIdArray, uint32 *pNhIdCount)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_get(unit, ecmpId, nhIdArraySize, pNhIdArray, pNhIdCount);
}   /* end of rtk_l3_ecmp_get */


/* Function Name:
 *      rtk_l3_ecmp_add
 * Description:
 *      Add a nexthop in an ECMP entry
 * Input:
 *      unit       - unit id
 *      ecmpPathId - ECMP path ID
 *      pathId     - nexthop path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_EXCEED_HW_CAPACITY - exceed the hardware capacity
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_add(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_add(unit, ecmpId, nhId);
}   /* end of rtk_l3_ecmp_add */


/* Function Name:
 *      rtk_l3_ecmp_del
 * Description:
 *      Delete a nexthop from an ECMP entry
 * Input:
 *      unit   - unit id
 *      ecmpId - ECMP path ID
 *      nhId   - nexthop path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_del(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_del(unit, ecmpId, nhId);
}   /* end of rtk_l3_ecmp_del */


/* Function Name:
 *      rtk_l3_ecmp_find
 * Description:
 *      Find a nexthop pointing to a ECMP path
 * Input:
 *      unit       - unit id
 *      nhIdCount  - number of path IDs in pIntf_array
 *      pNhIdArray - pointer to the path IDs
 * Output:
 *      pEcmpId    - ECMP path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_ecmp_find(uint32 unit, uint32 nhIdCount, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_ecmp_find(unit, nhIdCount, pNhIdArray, pEcmpId);
}   /* end of rtk_l3_ecmp_find */


/* Function Name:
 *      rtk_l3_key_t_init
 * Description:
 *      Initialize a rtk_l3_key_t_init structure
 * Input:
 *      unit - unit id
 *      pKey - pointer to L3 key (uninitialized)
 * Output:
 *      pKey - pointer to L3 key (initialized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_key_t_init(rtk_l3_key_t *pKey)
{
    /* function body */
    RT_PARAM_CHK((NULL == pKey), RT_ERR_NULL_POINTER);

    osal_memset(pKey, 0, sizeof(rtk_l3_key_t));

    return RT_ERR_OK;
}   /* end of rtk_l3_key_t_init */


/* Function Name:
 *      rtk_l3_host_t_init
 * Description:
 *      Initialize a rtk_l3_host_t structure
 * Input:
 *      pHost - pointer to host entry info (uninitilized)
 * Output:
 *      pHost - pointer to host entry info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_t_init(rtk_l3_host_t *pHost)
{
    /* function body */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);

    osal_memset(pHost, 0, sizeof(rtk_l3_host_t));

    return RT_ERR_OK;
}   /* end of rtk_l3_host_t_init */


/* Function Name:
 *      rtk_l3_host_add
 * Description:
 *      Add an entry into the L3 host routing table
 * Input:
 *      unit  - unit id
 *      pHost - pointer to rtk_l3_host_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 *      RT_ERR_TBL_FULL           - table is full
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6            - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP    - assign path with nexthop entry
 *      (3) When pHost->path_id = 0, the pHost->fwd_act can only be RTK_L3_HOST_ACT_DROP
 *          or RTK_L3_HOST_ACT_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_add(uint32 unit, rtk_l3_host_t *pHost)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_add(unit, pHost);
}   /* end of rtk_l3_host_add */


/* Function Name:
 *      rtk_l3_host_del
 * Description:
 *      Delete an entry from the L3 host routing table
 * Input:
 *      unit  - unit id
 *      pHost - pointer to rtk_l3_host_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_del(uint32 unit, rtk_l3_host_t *pHost)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_del(unit, pHost);
}   /* end of rtk_l3_host_del */


/* Function Name:
 *      rtk_l3_host_del_byNetwork
 * Description:
 *      Delete L3 host entries based on IP prefix (network)
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to the structure containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_del_byNetwork(unit, pRoute);
}   /* end of rtk_l3_host_del_byNetwork */


/* Function Name:
 *      rtk_l3_host_del_byIntfId
 * Description:
 *      Delete L3 host entries that match or do not match a specified L3 interface number
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type host entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_del_byIntfId(unit, intfId, flags);
}   /* end of rtk_l3_host_del_byIntfId */


/* Function Name:
 *      rtk_l3_host_delAll
 * Description:
 *      Delete all L3 host table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_delAll(unit, flags);
}   /* end of rtk_l3_host_delAll */


/* Function Name:
 *      rtk_l3_host_find
 * Description:
 *      Find an L3 host entry based on IP address
 * Input:
 *      unit  - unit id
 *      pHost - pointer to the structure containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set, if the bit set the flag RTK_L3_FLAG_READ_HIT_IGNORE should be clear
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_find(uint32 unit, rtk_l3_host_t *pHost)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_find(unit, pHost);
}   /* end of rtk_l3_host_find */


/* Function Name:
 *      rtk_l3_hostConflict_get
 * Description:
 *      Return list of conflicts in the L3 table
 * Input:
 *      unit       - unit id
 *      pKey       - IP address to test conflict condition
 *      pHostArray - pointer to the array of conflicting addresses
 *      maxHost    - maximum number of conflicts that may be returned
 * Output:
 *      pHostArray - pointer to the array of conflicting addresses returned
 *      pHostCount - actual number of conflicting addresses returned
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (3) pHostArray is input buffer, user should alloc memory firstly,
 *          maxHost is the max entry number can use rtk_switch_deviceInfo_get() to get max_num_of_l3_conflict_host
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_hostConflict_get(unit, pKey, pHostArray, maxHost, pHostCount);
}   /* end of rtk_l3_hostConflict_get */


/* Function Name:
 *      rtk_l3_host_age
 * Description:
 *      Run L3 host table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags (RTK_L3_FLAG_XXX)
 *      fAge    - callback function, NULL if none
 *      pCookie - user data to be passed to callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_age(unit, flags, fAge, pCookie);
}   /* end of rtk_l3_host_age */


/* Function Name:
 *      rtk_l3_host_getNext
 * Description:
 *      Get the next valid L3 host entry (based on the base index)
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 *      pBase - pointer to the start number to be searched
 * Output:
 *      pBase - pointer to the index of the returned entry (-1 means not found)
 *      pHost - L3 host entry (if found)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_host_getNext(unit, flags, pBase, pHost);
}   /* end of rtk_l3_host_getNext */


/* Function Name:
 *      rtk_l3_route_t_init
 * Description:
 *      Initialize a rtk_l3_route_t structure
 * Input:
 *      pRoute - pointer to L3 route entry info (uninitilized)
 * Output:
 *      pRoute - pointer to L3 route entry info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_t_init(rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);

    osal_memset(pRoute, 0, sizeof(rtk_l3_route_t));

    return RT_ERR_OK;
}   /* end of rtk_l3_route_t_init */


/* Function Name:
 *      rtk_l3_route_add
 * Description:
 *      Add an IP route to the L3 route table
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6            - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP    - assign path with nexthop entry
 *      (3) When pRoute->path_id = 0, the pRoute->fwd_act can only be RTK_L3_ROUTE_ACT_DROP
 *          or RTK_L3_ROUTE_ACT_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_add(unit, pRoute);
}   /* end of rtk_l3_route_add */


/* Function Name:
 *      rtk_l3_route_del
 * Description:
 *      Delete an IP route from the L3 route table
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_del(unit, pRoute);
}   /* end of rtk_l3_route_del */


/* Function Name:
 *      rtk_l3_route_get
 * Description:
 *      Look up a route given the network and netmask
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t specifying the basic inputs
 * Output:
 *      pRoute - pointer to returned rtk_l3_route_t info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set, if the bit set the flag RTK_L3_FLAG_READ_HIT_IGNORE should be clear
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_get(unit, pRoute);
}   /* end of rtk_l3_route_get */


/* Function Name:
 *      rtk_l3_route_del_byIntfId
 * Description:
 *      Delete routes based on matching or non-matching L3 interface ID
 * Input:
 *      unit   - unit id
 *      flags  - control flags (RTK_L3_FLAG_XXX)
 *      intfId - L3 interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type route entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_del_byIntfId(unit, flags, intfId);
}   /* end of rtk_l3_route_del_byIntfId */


/* Function Name:
 *      rtk_l3_route_delAll
 * Description:
 *      Delete all L3 route table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_delAll(unit, flags);
}   /* end of rtk_l3_route_delAll */


/* Function Name:
 *      rtk_l3_route_age
 * Description:
 *      Run L3 route table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags (RTK_L3_FLAG_XXX)
 *      fAge    - callback function, NULL if none
 *      pCookie - user data to be passed to callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_age(unit, flags, fAge, pCookie);
}   /* end of rtk_l3_route_age */


/* Function Name:
 *      rtk_l3_route_getNext
 * Description:
 *      Get the next valid L3 route entry (based on the base index)
 * Input:
 *      unit   - unit id
 *      flags  - control flags (RTK_L3_FLAG_XXX)
 *      pBase  - pointer to the starting index number to be searched
 * Output:
 *      pBase  - pointer to the index of the returned entry (-1 means not found)
 *      pRoute - L3 route entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of L3 route table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_route_getNext(unit, flags, pBase, pRoute);
}   /* end of rtk_l3_route_getNext */

/* Function Name:
 *      rtk_l3_globalCtrl_get
 * Description:
 *      Get the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_globalCtrl_get(unit, type, pArg);
}   /* end of rtk_l3_globalCtrl_get */

/* Function Name:
 *      rtk_l3_globalCtrl_set
 * Description:
 *      Set the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_globalCtrl_set(unit, type, arg);
}   /* end of rtk_l3_globalCtrl_set */

/* Function Name:
 *      rtk_l3_intfCtrl_get
 * Description:
 *      Get the configuration of the specified control type and interface ID
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface id
 *      type   - control type
 * Output:
 *      pArg   - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intfCtrl_get(unit, intfId, type, pArg);
}   /* end of rtk_l3_intfCtrl_get */

/* Function Name:
 *      rtk_l3_intfCtrl_set
 * Description:
 *      Set the configuration of the specified control type and interface ID
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface id
 *      type   - control type
 *      arg    - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_intfCtrl_set(unit, intfId, type, arg);
}   /* end of rtk_l3_intfCtrl_set */

/* Function Name:
 *      rtk_l3_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_portCtrl_get(unit, port, type, pArg);
}   /* end of rtk_l3_portCtrl_get */

/* Function Name:
 *      rtk_l3_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l3_portCtrl_set(unit, port, type, arg);
}   /* end of rtk_l3_portCtrl_set */

