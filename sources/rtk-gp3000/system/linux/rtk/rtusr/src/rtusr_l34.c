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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#ifdef CONFIG_XDSL_RG_DIAGSHELL
#include <rtk_rg_xdsl_struct.h>
#else
#include <rtk/l34.h>
#endif
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    SETSOCKOPT(RTDRV_L34_INIT, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == netifEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.netifEntry, netifEntry, sizeof(rtk_l34_netif_entry_t));
    SETSOCKOPT(RTDRV_L34_NETIFTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == netifEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NETIFTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(netifEntry, &l34_cfg.netifEntry, sizeof(rtk_l34_netif_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == arpEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.arpEntry, arpEntry, sizeof(rtk_l34_arp_entry_t));
    SETSOCKOPT(RTDRV_L34_ARPTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == arpEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ARPTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(arpEntry, &l34_cfg.arpEntry, sizeof(rtk_l34_arp_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_ARPTABLE_DEL, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pppEntry), RT_ERR_NULL_POINTER);
    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.ppEntry, pppEntry, sizeof(rtk_l34_pppoe_entry_t));
    SETSOCKOPT(RTDRV_L34_PPPOETABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pppEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_PPPOETABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pppEntry, &l34_cfg.ppEntry, sizeof(rtk_l34_pppoe_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == routEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.routEntry, routEntry, sizeof(rtk_l34_routing_entry_t));
    SETSOCKOPT(RTDRV_L34_ROUTINGTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == routEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ROUTINGTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(routEntry, &l34_cfg.routEntry, sizeof(rtk_l34_routing_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_ROUTINGTABLE_DEL, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == nextHopEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.nextHopEntry, nextHopEntry, sizeof(rtk_l34_nexthop_entry_t));
    SETSOCKOPT(RTDRV_L34_NEXTHOPTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == nextHopEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NEXTHOPTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(nextHopEntry, &l34_cfg.nextHopEntry, sizeof(rtk_l34_nexthop_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == extIpEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.extIpEntry, extIpEntry, sizeof(rtk_l34_ext_intip_entry_t));
    SETSOCKOPT(RTDRV_L34_EXTINTIPTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == extIpEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_EXTINTIPTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(extIpEntry, &l34_cfg.extIpEntry, sizeof(rtk_l34_ext_intip_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_EXTINTIPTABLE_DEL, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptrEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.forced, &forced, sizeof(int8));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.naptrEntry, naptrEntry, sizeof(rtk_l34_naptInbound_entry_t));
    SETSOCKOPT(RTDRV_L34_NAPTINBOUNDTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptrEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NAPTINBOUNDTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(naptrEntry, &l34_cfg.naptrEntry, sizeof(rtk_l34_naptInbound_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.forced, &forced, sizeof(int8));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.naptEntry, naptEntry, sizeof(rtk_l34_naptOutbound_entry_t));
    SETSOCKOPT(RTDRV_L34_NAPTOUTBOUNDTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NAPTOUTBOUNDTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(naptEntry, &l34_cfg.naptEntry, sizeof(rtk_l34_naptOutbound_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipmcEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.ipmcEntry, ipmcEntry, sizeof(rtk_l34_ipmcTrans_entry_t));
    SETSOCKOPT(RTDRV_L34_IPMCTRANSTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipmcEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_IPMCTRANSTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(ipmcEntry, &l34_cfg.ipmcEntry, sizeof(rtk_l34_ipmcTrans_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.type, &type, sizeof(rtk_l34_table_type_t));
    SETSOCKOPT(RTDRV_L34_TABLE_RESET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == bindEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.bindEntry, bindEntry, sizeof(rtk_binding_entry_t));
    SETSOCKOPT(RTDRV_L34_BINDINGTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == bindEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_BINDINGTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(bindEntry, &l34_cfg.bindEntry, sizeof(rtk_binding_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.bindType, &bindType, sizeof(rtk_l34_bindType_t));
    osal_memcpy(&l34_cfg.bindAction, &bindAction, sizeof(rtk_l34_bindAct_t));
    SETSOCKOPT(RTDRV_L34_BINDINGACTION_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == bindAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.bindType, &bindType, sizeof(rtk_l34_bindType_t));
    GETSOCKOPT(RTDRV_L34_BINDINGACTION_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(bindAction, &l34_cfg.bindAction, sizeof(rtk_l34_bindAct_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == wanTypeEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.wanTypeEntry, wanTypeEntry, sizeof(rtk_wanType_entry_t));
    SETSOCKOPT(RTDRV_L34_WANTYPETABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == wanTypeEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_WANTYPETABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(wanTypeEntry, &l34_cfg.wanTypeEntry, sizeof(rtk_wanType_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipv6RoutEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.ipv6RoutEntry, ipv6RoutEntry, sizeof(rtk_ipv6Routing_entry_t));
    SETSOCKOPT(RTDRV_L34_IPV6ROUTINGTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipv6RoutEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_IPV6ROUTINGTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(ipv6RoutEntry, &l34_cfg.ipv6RoutEntry, sizeof(rtk_ipv6Routing_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipv6NeighborEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.ipv6NeighborEntry, ipv6NeighborEntry, sizeof(rtk_ipv6Neighbor_entry_t));
    SETSOCKOPT(RTDRV_L34_IPV6NEIGHBORTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ipv6NeighborEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_IPV6NEIGHBORTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(ipv6NeighborEntry, &l34_cfg.ipv6NeighborEntry, sizeof(rtk_ipv6Neighbor_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.hsabMode, &hsabMode, sizeof(rtk_l34_hsba_mode_t));
    SETSOCKOPT(RTDRV_L34_HSABMODE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsabMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.hsabMode, pHsabMode, sizeof(rtk_l34_hsba_mode_t));
    GETSOCKOPT(RTDRV_L34_HSABMODE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pHsabMode, &l34_cfg.hsabMode, sizeof(rtk_l34_hsba_mode_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsaData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.hsaData, pHsaData, sizeof(rtk_l34_hsa_t));
    GETSOCKOPT(RTDRV_L34_HSADATA_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pHsaData, &l34_cfg.hsaData, sizeof(rtk_l34_hsa_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHsbData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.hsbData, pHsbData, sizeof(rtk_l34_hsb_t));
    GETSOCKOPT(RTDRV_L34_HSBDATA_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pHsbData, &l34_cfg.hsbData, sizeof(rtk_l34_hsb_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_hsbData_get */

/* Function Name:
 *      rtk_l34_portWanMap_set
 * Description:
 *      Set L34 WAN interface Port mapping
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.portWanMapType, &portWanMapType, sizeof(rtk_l34_portWanMapType_t));
    osal_memcpy(&l34_cfg.portWanMapEntry, &portWanMapEntry, sizeof(rtk_l34_portWanMap_entry_t));
    SETSOCKOPT(RTDRV_L34_PORTWANMAP_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_portWanMap_set */


/* Function Name:
 *      rtk_l34_portWanMap_get
 * Description:
 *      Get L34 WAN interface Port mapping
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortWanMapEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.portWanMapType, &portWanMapType, sizeof(rtk_l34_portWanMapType_t));
    osal_memcpy(&l34_cfg.portWanMapEntry, pPortWanMapEntry, sizeof(rtk_l34_portWanMap_entry_t));
    GETSOCKOPT(RTDRV_L34_PORTWANMAP_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pPortWanMapEntry, &l34_cfg.portWanMapEntry, sizeof(rtk_l34_portWanMap_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.stateType, &stateType, sizeof(rtk_l34_globalStateType_t));
    osal_memcpy(&l34_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L34_GLOBALSTATE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.stateType, &stateType, sizeof(rtk_l34_globalStateType_t));
    GETSOCKOPT(RTDRV_L34_GLOBALSTATE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pState, &l34_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.lookupMode, &lookupMode, sizeof(rtk_l34_lookupMode_t));
    SETSOCKOPT(RTDRV_L34_LOOKUPMODE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLookupMode), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L34_LOOKUPMODE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pLookupMode, &l34_cfg.lookupMode, sizeof(rtk_l34_lookupMode_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.portType, &portType, sizeof(rtk_l34_portType_t));
    osal_memcpy(&l34_cfg.portId, &portId, sizeof(uint32));
    osal_memcpy(&l34_cfg.wanIdx, &wanIdx, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_LOOKUPPORTMAP_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pWanIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.portType, &portType, sizeof(rtk_l34_portType_t));
    osal_memcpy(&l34_cfg.portId, &portId, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_LOOKUPPORTMAP_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pWanIdx, &l34_cfg.wanIdx, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.wanRouteMode, &wanRouteMode, sizeof(rtk_l34_wanRouteMode_t));
    SETSOCKOPT(RTDRV_L34_WANROUTMODE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pWanRouteMode), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L34_WANROUTMODE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pWanRouteMode, &l34_cfg.wanRouteMode, sizeof(rtk_l34_wanRouteMode_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ARPTRFINDICATOR_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pArpIndicator, &l34_cfg.arpIndicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pNaptIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NAPTTRFINDICATOR_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pNaptIndicator, &l34_cfg.naptIndicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPppIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_PPPTRFINDICATOR_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pPppIndicator, &l34_cfg.pppIndicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pNeighIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NEIGHTRFINDICATOR_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pNeighIndicator, &l34_cfg.neighIndicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.hsdState, &hsdState, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L34_HSDSTATE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == phsdState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.hsdState, phsdState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_L34_HSDSTATE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(phsdState, &l34_cfg.hsdState, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.l4TrfTable, &l4TrfTable, sizeof(rtk_l34_l4_trf_t));
    SETSOCKOPT(RTDRV_L34_HWL4TRFWRKTBL_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pl4TrfTable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.l4TrfTable, pl4TrfTable, sizeof(rtk_l34_l4_trf_t));
    GETSOCKOPT(RTDRV_L34_HWL4TRFWRKTBL_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pl4TrfTable, &l34_cfg.l4TrfTable, sizeof(rtk_l34_l4_trf_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.l4TrfTable, &l4TrfTable, sizeof(rtk_l34_l4_trf_t));
    osal_memcpy(&l34_cfg.l4EntryIndex, &l4EntryIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_L4TRFTB_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pIndicator, &l34_cfg.indicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.l4TrfTable, &l4TrfTable, sizeof(rtk_l34_l4_trf_t));
    GETSOCKOPT(RTDRV_L34_HWL4TRFWRKTBL_CLEAR, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.arpTrfTable, &arpTrfTable, sizeof(rtk_l34_arp_trf_t));
    SETSOCKOPT(RTDRV_L34_HWARPTRFWRKTBL_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpTrfTable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.arpTrfTable, pArpTrfTable, sizeof(rtk_l34_arp_trf_t));
    GETSOCKOPT(RTDRV_L34_HWARPTRFWRKTBL_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pArpTrfTable, &l34_cfg.arpTrfTable, sizeof(rtk_l34_arp_trf_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.arpTrfTable, &arpTrfTable, sizeof(rtk_l34_arp_trf_t));
    osal_memcpy(&l34_cfg.arpEntryIndex, &arpEntryIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ARPTRFTB_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pIndicator, &l34_cfg.indicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.arpTrfTable, &arpTrfTable, sizeof(rtk_l34_arp_trf_t));
    GETSOCKOPT(RTDRV_L34_HWARPTRFWRKTBL_CLEAR, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pNaptMaps), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&l34_cfg.naptMaps, pNaptMaps, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NAPTTRFINDICATOR_GET_ALL, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pNaptMaps, &l34_cfg.naptMaps, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pArpMaps), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&l34_cfg.arpMaps, pArpMaps, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ARPTRFINDICATOR_GET_ALL, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pArpMaps, &l34_cfg.arpMaps, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPppIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L34_PPPTRFINDICATOR_GET_ALL, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pPppIndicator, &l34_cfg.pppTrfAll, sizeof(rtk_l34_ppp_trf_all_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_pppTrfIndicator_get */


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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.portId, &port, sizeof(uint32));
    osal_memcpy(&l34_cfg.index, &ipmcTransIdx, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_IPV6MCROUTINGTRANSIDX_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcTransIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&l34_cfg.portId, &port, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_IPV6MCROUTINGTRANSIDX_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pIpmcTransIdx, &l34_cfg.index, sizeof(uint32));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_l34_ipv4FlowRouteTable_set
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.flowRouteEntry, pEntry, sizeof(rtk_l34_flowRoute_entry_t));
    SETSOCKOPT(RTDRV_L34_FLOWROUTE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_flowRouteTable_set */

/* Function Name:
 *      rtk_l34_ipv6FlowRouteTable_get
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.flowRouteEntry, pEntry, sizeof(rtk_l34_flowRoute_entry_t));
    GETSOCKOPT(RTDRV_L34_FLOWROUTE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pEntry, &l34_cfg.flowRouteEntry, sizeof(rtk_l34_flowRoute_entry_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_flowRouteTable_get */

/* Function Name:
 *      rtk_l34_flowTrfIndicator_get
 * Description:
 *      get  flow routing traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pFlowIndicator	- point of traffic indicator for flow routing 
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFlowIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_FLOWTRFINDICATOR_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pFlowIndicator, &l34_cfg.flowIndicator, sizeof(rtk_enable_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsliteInfEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.dsliteInfEntry, pDsliteInfEntry, sizeof(rtk_l34_dsliteInf_entry_t));
    SETSOCKOPT(RTDRV_L34_DSLITEINFTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsliteInfEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.dsliteInfEntry, pDsliteInfEntry, sizeof(rtk_l34_dsliteInf_entry_t));
    GETSOCKOPT(RTDRV_L34_DSLITEINFTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pDsliteInfEntry, &l34_cfg.dsliteInfEntry, sizeof(rtk_l34_dsliteInf_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsliteMcEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.dsliteMcEntry, pDsliteMcEntry, sizeof(rtk_l34_dsliteMc_entry_t));
    SETSOCKOPT(RTDRV_L34_DSLITEMCTABLE_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsliteMcEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.dsliteMcEntry, pDsliteMcEntry, sizeof(rtk_l34_dsliteMc_entry_t));
    GETSOCKOPT(RTDRV_L34_DSLITEMCTABLE_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pDsliteMcEntry, &l34_cfg.dsliteMcEntry, sizeof(rtk_l34_dsliteMc_entry_t));

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.dsliteCtrlType, &ctrlType, sizeof(rtk_l34_dsliteCtrlType_t));
	osal_memcpy(&l34_cfg.dsliteCtrlAct, &act, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_DSLITECONTROL_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    /* function body */
	osal_memcpy(&l34_cfg.dsliteCtrlType, &ctrlType, sizeof(rtk_l34_dsliteCtrlType_t));
    GETSOCKOPT(RTDRV_L34_DSLITECONTROL_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pAct, &l34_cfg.dsliteCtrlAct, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l34_dsliteState_get */

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
	rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pL34Cnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34_cfg.l34Cnts, pL34Cnt, sizeof(rtk_l34_mib_t));
    GETSOCKOPT(RTDRV_L34_MIB_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
    osal_memcpy(pL34Cnt, &l34_cfg.l34Cnts, sizeof(rtk_l34_mib_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_mib_get */

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
	rtdrv_l34Cfg_t l34_cfg;

    /* parameter check */

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.idx, &ifIndex, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_MIB_RESET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
}

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
	rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34Cfg_t));
    osal_memcpy(&l34_cfg.lutMissAct, &lutMissAct, sizeof(rtk_l34_lutMissAct_t));
    SETSOCKOPT(RTDRV_L34_LUTMISS_SET, &l34_cfg, rtdrv_l34Cfg_t, 1);

    return RT_ERR_OK;
}

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
	rtdrv_l34Cfg_t l34_cfg;

    /* function body */
    GETSOCKOPT(RTDRV_L34_LUTMISS_GET, &l34_cfg, rtdrv_l34Cfg_t, 1);
	osal_memcpy(pLutMissAct, &l34_cfg.lutMissAct, sizeof(rtk_l34_lutMissAct_t));

    return RT_ERR_OK;
}
