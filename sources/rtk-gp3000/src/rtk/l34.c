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


/*
 * Include Files
 */
 
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/init.h>
#include <rtk/l34.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */


/* Module Name    : L34  */
/* Sub-module Name: Network Interface Table */

/* Function Name:
 *      rtk_l34_init
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
int32
rtk_l34_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_init */


/* Function Name:
 *      rtk_l34_netifTable_set
 * Description:
 *      Set netif table entry
 * Input:
 *      idx - index of netif table
 *      *netifEntry -point of netif entry 
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
int32
rtk_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *netifEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_netifTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_netifTable_set(idx, netifEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_netifTable_set */


/* Function Name:
 *      rtk_l34_netifTable_get
 * Description:
 *      Get netif table entry
 * Input:
 *      idx - index of netif table
 * Output:
 *      *netifEntry -point of netif entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *netifEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_netifTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_netifTable_get(idx, netifEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_netifTable_get */


/* Sub-module Name: ARP Table */

/* Function Name:
 *      rtk_l34_arpTable_set
 * Description:
 *      Set arp table entry
 * Input:
 *      idx - index of arp table
 *      *arpEntry -point of arp entry
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
int32
rtk_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *arpEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTable_set(idx, arpEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTable_set */


/* Function Name:
 *      rtk_l34_arpTable_get
 * Description:
 *      Get arp table entry
 * Input:
 *      idx - index of arp table
 * Output:
 *      *arpEntry -point of arp entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *arpEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTable_get(idx, arpEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTable_get */


/* Function Name:
 *      rtk_l34_arpTable_del
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
int32
rtk_l34_arpTable_del(uint32 idx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTable_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTable_del(idx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTable_del */



/* Sub-module Name: PPPoE Table */

/* Function Name:
 *      rtk_l34_pppoeTable_set
 * Description:
 *      Set PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 *      *pppEntry -point of PPPoE entry
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
int32
rtk_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *pppEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_pppoeTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_pppoeTable_set(idx, pppEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_pppoeTable_set */


/* Function Name:
 *      rtk_l34_pppoeTable_get
 * Description:
 *      Get PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 * Output:
 *      *pppEntry -point of PPPoE entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *pppEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_pppoeTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_pppoeTable_get(idx, pppEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_pppoeTable_get */



/* Sub-module Name: Routing Table */

/* Function Name:
 *      rtk_l34_routingTable_set
 * Description:
 *      Set Routing table entry
 * Input:
 *      idx - index of Routing table
 *      *routEntry -point of Routing entry
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
int32
rtk_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *routEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_routingTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_routingTable_set(idx, routEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_routingTable_set */


/* Function Name:
 *      rtk_l34_routingTable_get
 * Description:
 *      Get Routing table entry
 * Input:
 *      idx - index of Routing table
 * Output:
 *      *routEntry -point of Routing entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *routEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_routingTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    osal_memset(routEntry, 0x0, sizeof(rtk_l34_routing_entry_t));
    ret = RT_MAPPER->l34_routingTable_get(idx, routEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_routingTable_get */


/* Function Name:
 *      rtk_l34_routingTable_del
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
int32
rtk_l34_routingTable_del(uint32 idx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_routingTable_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_routingTable_del(idx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_routingTable_del */



/* Sub-module Name: Next-Hop Table */

/* Function Name:
 *      rtk_l34_nexthopTable_set
 * Description:
 *      Set  Next-Hop table entry
 * Input:
 *      idx - index of  Next-Hop table
 *      *nextHopEntry -point of  Next-Hop entry
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
int32
rtk_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *nextHopEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_nexthopTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_nexthopTable_set(idx, nextHopEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_nexthopTable_set */


/* Function Name:
 *      rtk_l34_nexthopTable_get
 * Description:
 *      Get Next-Hop table entry
 * Input:
 *      idx - index of Next-Hop table
 * Output:
 *      *nextHopEntry -point of Next-Hop entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *nextHopEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_nexthopTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_nexthopTable_get(idx, nextHopEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_nexthopTable_get */


/* Sub-module Name: External_Internal IP Table */

/* Function Name:
 *      rtk_l34_extIntIPTable_set
 * Description:
 *      Set  External_Internal IP table entry
 * Input:
 *      idx - index of  External_Internal IP table
 *      *extIpEntry -point of External_Internal IP entry
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
int32
rtk_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *extIpEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_extIntIPTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_extIntIPTable_set(idx, extIpEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_extIntIPTable_set */


/* Function Name:
 *      rtk_l34_extIntIPTable_get
 * Description:
 *      Get External_Internal IP table entry
 * Input:
 *      idx - index of External_Internal IP table
 * Output:
 *      *extIpEntry -point of External_Internal IP entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *extIpEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_extIntIPTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_extIntIPTable_get(idx, extIpEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_extIntIPTable_get */


/* Function Name:
 *      rtk_l34_extIntIPTable_del
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
int32
rtk_l34_extIntIPTable_del(uint32 idx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_extIntIPTable_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_extIntIPTable_del(idx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_extIntIPTable_del */


/* Sub-module Name: NAPTR  Table */

/* Function Name:
 *      rtk_l34_naptInboundTable_set
 * Description:
 *      Set  NAPTR table entry
 * Input:
 *      forced - force set to NAPTR table
 *      *naptrEntry - value of NAPTR table entry
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
int32
rtk_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *naptrEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptInboundTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptInboundTable_set(forced, idx, naptrEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptInboundTable_set */


/* Function Name:
 *      rtk_l34_naptInboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of NAPTR table
 * Output:
 *      *naptrEntry -point of NAPTR entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *naptrEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptInboundTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptInboundTable_get(idx, naptrEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptInboundTable_get */


/* Sub-module Name: NAPT  Table */

/* Function Name:
 *      rtk_l34_naptOutboundTable_set
 * Description:
 *      Set  napt table entry
 * Input:
 *      forced - force set to napt table
 *      *naptEntry - value of napt table entry
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
int32
rtk_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *naptEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptOutboundTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptOutboundTable_set(forced, idx, naptEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptOutboundTable_set */


/* Function Name:
 *      rtk_l34_naptOutboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of napt table
 * Output:
 *      *naptEntry -point of napt entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *naptEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptOutboundTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptOutboundTable_get(idx, naptEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptOutboundTable_get */


/* Sub-module Name: IPMC Transfer Table */

/* Function Name:
 *      rtk_l34_ipmcTransTable_set
 * Description:
 *      Set  IPMC Transfer table entry
 * Input:
 *      forced - force set to IPMC Transfer table
 *      *ipmcEntry - value of  IPMC Transfer entry
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
int32
rtk_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *ipmcEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipmcTransTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipmcTransTable_set(idx, ipmcEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipmcTransTable_set */


/* Function Name:
 *      rtk_l34_ipmcTransTable_get
 * Description:
 *      Get IPMC Transfer table entry
 * Input:
 *      idx - index of IPMC Transfer table
 * Output:
 *      *ipmcEntry -point of IPMC Transfer entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *ipmcEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipmcTransTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipmcTransTable_get(idx, ipmcEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipmcTransTable_get */

/* Sub-module Name: L34 System Configure */

/* Function Name:
 *      rtk_l34_table_reset
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
int32
rtk_l34_table_reset(rtk_l34_table_type_t type)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_table_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_table_reset(type);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_table_reset */


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
int32
rtk_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *bindEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_bindingTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_bindingTable_set(idx, bindEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_bindingTable_set */


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
int32
rtk_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *bindEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_bindingTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_bindingTable_get(idx, bindEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_bindingTable_get */


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
int32
rtk_l34_bindingAction_set(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t bindAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_bindingAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_bindingAction_set(bindType, bindAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_bindingAction_set */


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
int32
rtk_l34_bindingAction_get(rtk_l34_bindType_t bindType, rtk_l34_bindAct_t *bindAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_bindingAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_bindingAction_get(bindType, bindAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_bindingAction_get */


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
int32
rtk_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *wanTypeEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_wanTypeTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_wanTypeTable_set(idx, wanTypeEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_wanTypeTable_set */


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
int32
rtk_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *wanTypeEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_wanTypeTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_wanTypeTable_get(idx, wanTypeEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_wanTypeTable_get */


/* Sub-module Name: IPv6 Routing Table */


/* Function Name:
 *      rtk_l34_ipv6RoutingTable_set
 * Description:
 *      Set a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 *      *ipv6RoutEntry - point of ipv6 routing table
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
int32
rtk_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *ipv6RoutEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipv6RoutingTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipv6RoutingTable_set(idx, ipv6RoutEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipv6RoutingTable_set */


/* Function Name:
 *      rtk_l34_ipv6RoutingTable_get
 * Description:
 *      Get a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 * Output:
 *      *ipv6RoutEntry - point of ipv6 routing table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_ipv6RoutingTable_get(uint32 idx, rtk_ipv6Routing_entry_t *ipv6RoutEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipv6RoutingTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipv6RoutingTable_get(idx, ipv6RoutEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipv6RoutingTable_get */



/* Sub-module Name: IPv6 Neighbor Table */

/* Function Name:
 *      rtk_l34_ipv6NeighborTable_set
 * Description:
 *      Set neighbor table
 * Input:
 *      idx - index of neighbor table
 *      *ipv6NeighborEntry - point of neighbor data
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
int32
rtk_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *ipv6NeighborEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipv6NeighborTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipv6NeighborTable_set(idx, ipv6NeighborEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipv6NeighborTable_set */


/* Function Name:
 *      rtk_l34_ipv6NeighborTable_get
 * Description:
 *      Get neighbor table
 * Input:
 *      idx - index of neighbor table
 * Output:
 *      *ipv6NeighborEntry - point of neighbor data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *ipv6NeighborEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ipv6NeighborTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ipv6NeighborTable_get(idx, ipv6NeighborEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_ipv6NeighborTable_get */

/* Function Name:
 *      rtk_l34_hsabMode_set
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
int32
rtk_l34_hsabMode_set(rtk_l34_hsba_mode_t hsabMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsabMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsabMode_set(hsabMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsabMode_set */


/* Function Name:
 *      rtk_l34_hsabMode_get
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
int32
rtk_l34_hsabMode_get(rtk_l34_hsba_mode_t *pHsabMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsabMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsabMode_get(pHsabMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsabMode_get */



/* Function Name:
 *      rtk_l34_hsaData_get
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
int32
rtk_l34_hsaData_get(rtk_l34_hsa_t *pHsaData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsaData_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsaData_get(pHsaData);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsaData_get */


/* Function Name:
 *      rtk_l34_hsbData_get
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
int32
rtk_l34_hsbData_get(rtk_l34_hsb_t *pHsbData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsbData_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsbData_get(pHsbData);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsbData_get */

/* Function Name:
 *      rtk_l34_portWanMap_set
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
int32
rtk_l34_portWanMap_set(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t portWanMapEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_portWanMap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_portWanMap_set(portWanMapType, portWanMapEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_portWanMap_set */


/* Function Name:
 *      rtk_l34_portWanMap_get
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
int32
rtk_l34_portWanMap_get(rtk_l34_portWanMapType_t portWanMapType, rtk_l34_portWanMap_entry_t *pPortWanMapEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_portWanMap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_portWanMap_get(portWanMapType, pPortWanMapEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_portWanMap_get */

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
int32
rtk_l34_globalState_set(rtk_l34_globalStateType_t stateType,rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_globalState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_globalState_set(stateType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_globalState_set */


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
int32
rtk_l34_globalState_get(rtk_l34_globalStateType_t stateType,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_globalState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_globalState_get(stateType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_globalState_get */



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
int32
rtk_l34_lookupMode_set(rtk_l34_lookupMode_t lookupMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lookupMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lookupMode_set(lookupMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_lookupMode_set */


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
int32
rtk_l34_lookupMode_get(rtk_l34_lookupMode_t *pLookupMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lookupMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lookupMode_get(pLookupMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_lookupMode_get */



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
int32
rtk_l34_lookupPortMap_set(rtk_l34_portType_t portType, uint32 portId, uint32 wanIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lookupPortMap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lookupPortMap_set(portType, portId, wanIdx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_lookupPortMap_set */


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
int32
rtk_l34_lookupPortMap_get(rtk_l34_portType_t portType, uint32 portId, uint32 *pWanIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lookupPortMap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lookupPortMap_get(portType, portId, pWanIdx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_lookupPortMap_get */


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
int32
rtk_l34_wanRoutMode_set(rtk_l34_wanRouteMode_t wanRouteMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_wanRoutMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_wanRoutMode_set(wanRouteMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_wanRoutMode_set */


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
int32
rtk_l34_wanRoutMode_get(rtk_l34_wanRouteMode_t *pWanRouteMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_wanRoutMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_wanRoutMode_get(pWanRouteMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_wanRoutMode_get */

/* Function Name:
 *      rtk_l34_arpTrfIndicator_get
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
int32
rtk_l34_arpTrfIndicator_get(uint32 index, rtk_enable_t *pArpIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTrfIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTrfIndicator_get(index, pArpIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTrfIndicator_get */


/* Function Name:
 *      rtk_l34_naptTrfIndicator_get
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
int32
rtk_l34_naptTrfIndicator_get(uint32 index, rtk_enable_t *pNaptIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptTrfIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptTrfIndicator_get(index, pNaptIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptTrfIndicator_get */



/* Function Name:
 *      rtk_l34_pppTrfIndicator_get
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
int32
rtk_l34_pppTrfIndicator_get(uint32 index, rtk_enable_t *pPppIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_pppTrfIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_pppTrfIndicator_get(index, pPppIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_pppTrfIndicator_get */


/* Function Name:
 *      rtk_l34_neighTrfIndicator_get
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
int32
rtk_l34_neighTrfIndicator_get(uint32 index, rtk_enable_t *pNeighIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_neighTrfIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_neighTrfIndicator_get(index, pNeighIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_neighTrfIndicator_get */


/* Function Name:
 *      rtk_l34_hsdState_set
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
int32
rtk_l34_hsdState_set(rtk_enable_t hsdState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsdState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsdState_set(hsdState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsdState_set */


/* Function Name:
 *      rtk_l34_hsdState_get
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
int32
rtk_l34_hsdState_get(rtk_enable_t *phsdState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hsdState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hsdState_get(phsdState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hsdState_get */


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_set
 * Description:
 *      Set HW working table id for L4 trf.
 * Input:
 *      rtk_l34_l4_trf_t l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 
rtk_l34_hwL4TrfWrkTbl_set(rtk_l34_l4_trf_t l4TrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwL4TrfWrkTbl_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwL4TrfWrkTbl_set(l4TrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwL4TrfWrkTbl_set */


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      rtk_l34_l4_trf_t *pl4TrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 
rtk_l34_hwL4TrfWrkTbl_get(rtk_l34_l4_trf_t *pl4TrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwL4TrfWrkTbl_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwL4TrfWrkTbl_get(pl4TrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwL4TrfWrkTbl_get */

/* Function Name:
 *      rtk_l34_l4TrfTb_get
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
int32 
rtk_l34_l4TrfTb_get(rtk_l34_l4_trf_t l4TrfTable,uint32 l4EntryIndex,rtk_enable_t *pIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_l4TrfTb_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_l4TrfTb_get(l4TrfTable, l4EntryIndex, pIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_l4TrfTb_get */


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_Clear
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
int32 
rtk_l34_hwL4TrfWrkTbl_Clear(rtk_l34_l4_trf_t l4TrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwL4TrfWrkTbl_Clear)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwL4TrfWrkTbl_Clear(l4TrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwL4TrfWrkTbl_Clear */


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_set
 * Description:
 *      Set HW working table id for ARP trf.
 * Input:
 *      rtk_l34_arp_trf_t arpTrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 
rtk_l34_hwArpTrfWrkTbl_set(rtk_l34_arp_trf_t arpTrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwArpTrfWrkTbl_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwArpTrfWrkTbl_set(arpTrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwArpTrfWrkTbl_set */


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      None
 * Output:
 *      rtk_l34_l4_trf_t *pArpTrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 
rtk_l34_hwArpTrfWrkTbl_get(rtk_l34_arp_trf_t *pArpTrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwArpTrfWrkTbl_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwArpTrfWrkTbl_get(pArpTrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwArpTrfWrkTbl_get */

/* Function Name:
 *      rtk_l34_arpTrfTb_get
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
int32 
rtk_l34_arpTrfTb_get(rtk_l34_arp_trf_t arpTrfTable,uint32 arpEntryIndex,rtk_enable_t *pIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTrfTb_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTrfTb_get(arpTrfTable, arpEntryIndex, pIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTrfTb_get */


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_Clear
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
int32 
rtk_l34_hwArpTrfWrkTbl_Clear(rtk_l34_arp_trf_t arpTrfTable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_hwArpTrfWrkTbl_Clear)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_hwArpTrfWrkTbl_Clear(arpTrfTable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_hwArpTrfWrkTbl_Clear */


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
int32
rtk_l34_naptTrfIndicator_get_all(uint32 *pNaptMaps)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_naptTrfIndicator_get_all)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_naptTrfIndicator_get_all(pNaptMaps);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_naptTrfIndicator_get_all */


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
int32
rtk_l34_arpTrfIndicator_get_all(uint32 *pArpMaps)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_arpTrfIndicator_get_all)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_arpTrfIndicator_get_all(pArpMaps);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_arpTrfIndicator_get_all */

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
int32
rtk_l34_pppTrfIndicator_get_all(rtk_l34_ppp_trf_all_t *pPppIndicator)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_pppTrfIndicator_get_all)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_pppTrfIndicator_get_all(pPppIndicator);
    RTK_API_UNLOCK();
    return ret;
}  /*end of rtk_l34_pppTrfIndicator_get_all*/

/* Function Name:
 *      rtk_l34_ip6mcRoutingTransIdx_set
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
int32
rtk_l34_ip6mcRoutingTransIdx_set(uint32 idx, rtk_port_t port, uint32 ipmcTransIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ip6mcRoutingTransIdx_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ip6mcRoutingTransIdx_set(idx, port, ipmcTransIdx);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l34_ip6mcRoutingTransIdx_get
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
int32
rtk_l34_ip6mcRoutingTransIdx_get(uint32 idx, rtk_port_t port, uint32 *pIpmcTransIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_ip6mcRoutingTransIdx_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_ip6mcRoutingTransIdx_get(idx, port, pIpmcTransIdx);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l34_flowRouteTable_set
 * Description:
 *      Set Ipv4 Flow Routing Table.
 * Input:
 *		entry  : entry content
 * Output:
 *		None
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
int32
rtk_l34_flowRouteTable_set(rtk_l34_flowRoute_entry_t *pEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_flowRouteTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_flowRouteTable_set(pEntry);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l34_flowRouteTable_get
 * Description:
 *      Get Ipv6 Flow Routing Table.
 * Input:
 *		None
 * Output:
 *		entry  : entry content
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
int32
rtk_l34_flowRouteTable_get(rtk_l34_flowRoute_entry_t *pEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_flowRouteTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_flowRouteTable_get(pEntry);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l34_flowTrfIndicator_get
 * Description:
 *      get  flow routing traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pArpIndicator	- point of traffic indicator for flow routing 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_flowTrfIndicator_get(uint32 index, rtk_enable_t *pFlowIndicator)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_flowTrfIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_flowTrfIndicator_get(index, pFlowIndicator);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_flowTrfIndicator_get */


/* Function Name:
 *      rtk_l34_dsliteInfTable_set
 * Description:
 *      Set DS-Lite interface table entry
 * Input:
 *      *dsliteInfEntry - point of DS-Lite interface entry 
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
int32
rtk_l34_dsliteInfTable_set(rtk_l34_dsliteInf_entry_t *pDsliteInfEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteInfTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteInfTable_set(pDsliteInfEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteInfTable_set */

/* Function Name:
 *      rtk_l34_dsliteInfTable_get
 * Description:
 *      Get DS-Lite interface table entry
 * Input:
 *      dsliteInfEntry->index - index of DS-Lite interface table
 * Output:
 *      *dsliteInfEntry - point of DS-Lite interface table entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_dsliteInfTable_get(rtk_l34_dsliteInf_entry_t *pDsliteInfEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteInfTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteInfTable_get(pDsliteInfEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteInfTable_get */

/* Function Name:
 *      rtk_l34_dsliteMcTable_set
 * Description:
 *      Set DS-Lite multicast table entry
 * Input:
 *      *dsliteMcEntry - point of DS-Lite multicast table entry 
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
int32
rtk_l34_dsliteMcTable_set(rtk_l34_dsliteMc_entry_t *pDsliteMcEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteMcTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteMcTable_set(pDsliteMcEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteMcTable_set */

/* Function Name:
 *      rtk_l34_dsliteMcTable_get
 * Description:
 *      Get DS-Lite multicast table entry
 * Input:
 *      dsliteMcEntry->index - index of DS-Lite multicast table 
 * Output:
 *      *dsliteMcEntry - point of DS-Lite multicast table entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_dsliteMcTable_get(rtk_l34_dsliteMc_entry_t *pDsliteMcEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteMcTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteMcTable_get(pDsliteMcEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteMcTable_get */

/* Function Name:
 *      rtk_l34_dsliteState_set
 * Description:
 *      Set DS-Lite state
 * Input:
 *      ctrlType  - Control type
 *      act - action
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
int32
rtk_l34_dsliteControl_set(rtk_l34_dsliteCtrlType_t ctrlType, uint32 act)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteControl_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteControl_set(ctrlType, act);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteState_set */

/* Function Name:
 *      rtk_l34_dsliteState_get
 * Description:
 *      Get DS-Lite state
 * Input:
 *      ctrlType  - Control type
 * Output:
 *      pAct - action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l34_dsliteControl_get(rtk_l34_dsliteCtrlType_t ctrlType, uint32 *pAct)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_dsliteControl_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_dsliteControl_get(ctrlType, pAct);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_l34_dsliteState_set */

/* Function Name:
 *      rtk_l34_mib_get
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
int32
rtk_l34_mib_get(rtk_l34_mib_t *pL34Cnt)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_mib_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l34_mib_get( pL34Cnt );
    RTK_API_UNLOCK();
    return ret;
}/* end of rtk_l34_mib_get */

/* Function Name:
 *      rtk_l34_mib_reset
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
int32
rtk_l34_mib_reset(uint32 ifIndex)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_mib_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l34_mib_reset( ifIndex );
    RTK_API_UNLOCK();
    return ret;
}/*end of rtk_l34_mib_reset*/

/* Function Name:
 *      rtk_l34_lutLookupMiss_set
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
int32
rtk_l34_lutLookupMiss_set(rtk_l34_lutMissAct_t lutMissAct)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lutLookupMiss_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lutLookupMiss_set(lutMissAct);
    RTK_API_UNLOCK();
    return ret;
}/*end of rtk_l34_lutLookupMiss_set*/

/* Function Name:
 *      rtk_l34_lutLookupMiss_get
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
int32
rtk_l34_lutLookupMiss_get(rtk_l34_lutMissAct_t *pLutMissAct)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->l34_lutLookupMiss_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->l34_lutLookupMiss_get(pLutMissAct);
    RTK_API_UNLOCK();
    return ret;
}/*end of rtk_l34_lutLookupMiss_get*/
