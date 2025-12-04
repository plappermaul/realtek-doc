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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking]
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
#include <rtk/port.h>
#include <rtk/vlan.h>


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

/* Module Name : SVLAN */

/* Function Name:
 *      rtk_svlan_init
 * Description:
 *      Initialize svlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize svlan module before calling any svlan APIs.
 */
int32
rtk_svlan_init(void)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_SVLAN_INIT, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_init */

/* Function Name:
 *      rtk_svlan_create
 * Description:
 *      Create the svlan.
 * Input:
 *      svid - svlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_EXIST - SVLAN entry is exist
 * Note:
 *      None
 */
int32
rtk_svlan_create(rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_CREATE, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_create */


/* Function Name:
 *      rtk_svlan_destroy
 * Description:
 *      Destroy the svlan.
 * Input:
 *      svid - svlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      None
 */
int32
rtk_svlan_destroy(rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_DESTROY, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_destroy */

/* Function Name:
 *      rtk_svlan_portSvid_get
 * Description:
 *      Get port default svlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pSvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_PORTSVID_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_portSvid_get */


/* Function Name:
 *      rtk_svlan_portSvid_set
 * Description:
 *      Set port default svlan id.
 * Input:
 *      port - port id
 *      svid - port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
rtk_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_PORTSVID_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_portSvid_set */


/* Function Name:
 *      rtk_svlan_servicePort_get
 * Description:
 *      Get service ports from the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - status of service port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_SERVICEPORT_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_servicePort_get */


/* Function Name:
 *      rtk_svlan_servicePort_set
 * Description:
 *      Set service ports to the specified device.
 * Input:
 *      port       - port id
 *      enable     - status of service port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_SERVICEPORT_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_servicePort_set */


/* Function Name:
 *      rtk_svlan_memberPort_set
 * Description:
 *      Replace the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
rtk_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlanPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvlanUntagPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.svlanPortmask, pSvlanPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&svlan_cfg.svlanUntagPortmask, pSvlanUntagPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_SVLAN_MEMBERPORT_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_memberPort_set */



/* Function Name:
 *      rtk_svlan_memberPort_get
 * Description:
 *      Get the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
rtk_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlanPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvlanUntagPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_MEMBERPORT_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvlanPortmask, &svlan_cfg.svlanPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(pSvlanUntagPortmask, &svlan_cfg.svlanUntagPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_memberPort_get */


/* Function Name:
 *      rtk_svlan_tpidEntry_get
 * Description:
 *      Get the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 * Output:
 *      pSvlanTagId - pointer buffer of svlan TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only support pSvlanTagId 0 in Apollo.
 */
int32
rtk_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlanTagId), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svlanIndex, &svlanIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_SVLAN_TPIDENTRY_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvlanTagId, &svlan_cfg.svlanTagId, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_svlan_tpidEntry_get */


/* Function Name:
 *      rtk_svlan_tpidEntry_set
 * Description:
 *      Set the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 *      svlan_tag_id - svlan TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only support pSvlan_tag_id 0 in Apollo.
 */
int32
rtk_svlan_tpidEntry_set(uint32 svlanIndex, uint32 svlan_tag_id)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svlanIndex, &svlanIndex, sizeof(uint32));
    osal_memcpy(&svlan_cfg.svlan_tag_id, &svlan_tag_id, sizeof(uint32));
    SETSOCKOPT(RTDRV_SVLAN_TPIDENTRY_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_tpidEntry_set */


/* Function Name:
 *      rtk_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
 *      ref - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
rtk_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.ref, &ref, sizeof(rtk_svlan_pri_ref_t));
    SETSOCKOPT(RTDRV_SVLAN_PRIORITYREF_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_priorityRef_set */

/* Function Name:
 *      rtk_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
 *      None
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
rtk_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pRef), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_PRIORITYREF_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pRef, &svlan_cfg.ref, sizeof(rtk_svlan_pri_ref_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_priorityRef_get */

/* Function Name:
 *      rtk_svlan_memberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 * Input:
 *      psvlan_cfg - SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameter.
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_SVLAN_TABLE_FULL - SVLAN configuration is full.
 * Note:
 *      The API can set system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped by default setup.
 *      - rtk_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
int32
rtk_svlan_memberPortEntry_set(rtk_svlan_memberCfg_t *pSvlan_cfg)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlan_cfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svlan_cfg, pSvlan_cfg, sizeof(rtk_svlan_memberCfg_t));
    SETSOCKOPT(RTDRV_SVLAN_MEMBERPORTENTRY_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_memberPortEntry_set */


/* Function Name:
 *      rtk_svlan_memberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 * Input:
 *      pSvlan_cfg - SVLAN member configuration
 * Output:
 *      pSvlan_cfg - SVLAN member configuration
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can get system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped.
 */
int32
rtk_svlan_memberPortEntry_get(rtk_svlan_memberCfg_t *pSvlan_cfg)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlan_cfg), RT_ERR_NULL_POINTER);

    /* function body */


    osal_memcpy(&svlan_cfg.svlan_cfg, pSvlan_cfg, sizeof(rtk_svlan_memberCfg_t));
    GETSOCKOPT(RTDRV_SVLAN_MEMBERPORTENTRY_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvlan_cfg, &svlan_cfg.svlan_cfg, sizeof(rtk_svlan_memberCfg_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_memberPortEntry_get */


/* Function Name:
 *      rtk_svlan_ipmc2s_add
 * Description:
 *      add ip multicast address to SVLAN
 * Input:
 *      ipmc    - ip multicast address
 *      ipmcMsk - ip multicast address mask
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can set IP mutlicast to SVID configuration. If upstream packet is IPv4 multicast
 *      packet and DIP is matched MC2S configuration, ASIC will assign egress SVID to the packet.
 *      There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_ipmc2s_add(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.ipmc, &ipmc, sizeof(ipaddr_t));
    osal_memcpy(&svlan_cfg.ipmcMsk, &ipmcMsk, sizeof(ipaddr_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_IPMC2S_ADD, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_ipmc2s_add */

/* Function Name:
 *      rtk_svlan_ipmc2s_del
 * Description:
 *      delete ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete IP mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_ipmc2s_del(ipaddr_t ipmc, ipaddr_t ipmcMsk)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.ipmc, &ipmc, sizeof(ipaddr_t));
    osal_memcpy(&svlan_cfg.ipmcMsk, &ipmcMsk, sizeof(ipaddr_t));
    SETSOCKOPT(RTDRV_SVLAN_IPMC2S_DEL, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_ipmc2s_del */


/* Function Name:
 *      rtk_svlan_ipmc2s_get
 * Description:
 *      Get ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can get IP mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_ipmc2s_get(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.ipmc, &ipmc, sizeof(ipaddr_t));
    osal_memcpy(&svlan_cfg.ipmcMsk, &ipmcMsk, sizeof(ipaddr_t));
    GETSOCKOPT(RTDRV_SVLAN_IPMC2S_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_ipmc2s_get */


/* Function Name:
 *      rtk_svlan_l2mc2s_add
 * Description:
 *      Add L2 multicast address to SVLAN
 * Input:
 *      mac - L2 multicast address
 *      macMsk  - L2 multicast address
 *      svid - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can set L2 Mutlicast to SVID configuration. If upstream packet is L2 multicast
 *      packet and DMAC is matched, ASIC will assign egress SVID to the packet. There are 32
 *      SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_l2mc2s_add(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.mac, &mac, sizeof(rtk_mac_t));
    osal_memcpy(&svlan_cfg.macMsk, &macMsk, sizeof(rtk_mac_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_L2MC2S_ADD, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_l2mc2s_add */


/* Function Name:
 *      rtk_svlan_l2mc2s_del
 * Description:
 *      delete L2 multicast address to SVLAN
 * Input:
 *      mac     - L2 multicast address
 *      macMsk  - L2 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete Mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_l2mc2s_del(rtk_mac_t mac, rtk_mac_t macMsk)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.mac, &mac, sizeof(rtk_mac_t));
    osal_memcpy(&svlan_cfg.macMsk, &macMsk, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_SVLAN_L2MC2S_DEL, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_l2mc2s_del */


/* Function Name:
 *      rtk_svlan_l2mc2s_get
 * Description:
 *      Get L2 multicast address to SVLAN
 * Input:
 *      mac - L2 multicast address
 *      macMsk  - L2 multicast address
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can get L2 mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
rtk_svlan_l2mc2s_get(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.mac, &mac, sizeof(rtk_mac_t));
    osal_memcpy(&svlan_cfg.macMsk, &macMsk, sizeof(rtk_mac_t));
    GETSOCKOPT(RTDRV_SVLAN_L2MC2S_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_l2mc2s_get */


/* Function Name:
 *      rtk_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
int32
rtk_svlan_sp2c_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t cvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.dstPort, &dstPort, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.cvid, &cvid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_SP2C_ADD, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_sp2c_add */


/* Function Name:
 *      rtk_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid 	    - SVLAN VID
 *      dst_port 	- Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pCvid - VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
rtk_svlan_sp2c_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t *pCvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pCvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.dstPort, &dstPort, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_SP2C_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pCvid, &svlan_cfg.cvid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_sp2c_get */

/* Function Name:
 *      rtk_svlan_sp2cPriority_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 *      priority    - Priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
int32
rtk_svlan_sp2cPriority_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t priority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.dstPort, &dstPort, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.priority, &priority, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_SVLAN_SP2CPRIORITY_ADD, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_svlan_sp2cPriority_add */

/* Function Name:
 *      rtk_svlan_sp2cPriority_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid 	    - SVLAN VID
 *      dst_port 	- Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pPriority 	- Priority
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration.
 */
int32
rtk_svlan_sp2cPriority_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t *pPriority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.dstPort, &dstPort, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_SP2CPRIORITY_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pPriority, &svlan_cfg.priority, sizeof(rtk_pri_t));

    return RT_ERR_OK;
} /* end of rtk_svlan_sp2cPriority_get */

/* Function Name:
 *      rtk_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can delete SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
rtk_svlan_sp2c_del(rtk_vlan_t svid, rtk_port_t dstPort)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.dstPort, &dstPort, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_SVLAN_SP2C_DEL, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_sp2c_del */

/* Function Name:
 *      rtk_svlan_dmacVidSelState_set
 * Description:
 *      Set DMAC CVID selection status
 * Input:
 *      port    - Port
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection state
 */
int32
rtk_svlan_dmacVidSelState_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_DMACVIDSELSTATE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_dmacVidSelState_set */

/* Function Name:
 *      rtk_svlan_dmacVidSelState_get
 * Description:
 *      Get DMAC CVID selection status
 * Input:
 *      port    - Port
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can get DMAC CVID Selection state
 */
int32
rtk_svlan_dmacVidSelState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_DMACVIDSELSTATE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_dmacVidSelState_get */

/* Function Name:
 *      rtk_svlan_unmatchAction_set
 * Description:
 *      Configure Action of downstream Unmatch packet
 * Input:
 *      action  - Action for Unmatch
 *      svid    - The SVID assigned to Unmatch packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also supported by this API. The parameter add svid is
 *      only refernced when the action is set to UNMATCH_ASSIGN
 */
int32
rtk_svlan_unmatchAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.action, &action, sizeof(rtk_svlan_action_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_UNMATCHACTION_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_unmatchAction_set */


/* Function Name:
 *      rtk_svlan_unmatchAction_get
 * Description:
 *      Get Action of downstream Unmatch packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for Unmatch
 *      pSvid    - The SVID assigned to Unmatch packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNMATCH_ASSIGN
 */
int32
rtk_svlan_unmatchAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_UNMATCHACTION_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pAction, &svlan_cfg.action, sizeof(rtk_svlan_action_t));
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_unmatchAction_get */


/* Function Name:
 *      rtk_svlan_untagAction_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
 *      action  - Action for UnStag
 *      svid    - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
int32
rtk_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.action, &action, sizeof(rtk_svlan_action_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_UNTAGACTION_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_untagAction_set */


/* Function Name:
 *      rtk_svlan_untagAction_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
int32
rtk_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_UNTAGACTION_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pAction, &svlan_cfg.action, sizeof(rtk_svlan_action_t));
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_untagAction_get */

/* Function Name:
 *      rtk_svlan_c2s_add
 * Description:
 *      add CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can set upstream packet CVID and ingress port to SVID configuration.
 *      There are 128 SVLAN configurations for CVID and ingress port.
 *      If CVID and SVID of configured entry are matched with configuration parameter, then
 *      different ingress port will share the same configuration entry.
 */
int32
rtk_svlan_c2s_add(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.cvid, &cvid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_C2S_ADD, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_c2s_add */

/* Function Name:
 *      rtk_svlan_c2s_del
 * Description:
 *      delete CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration.
 */
int32
rtk_svlan_c2s_del(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.cvid, &cvid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    SETSOCKOPT(RTDRV_SVLAN_C2S_DEL, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_c2s_del */


/* Function Name:
 *      rtk_svlan_c2s_get
 * Description:
 *      Get CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration.
 */
int32
rtk_svlan_c2s_get(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t *pSvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.cvid, &cvid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_SVLAN_C2S_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pSvid, &svlan_cfg.svid, sizeof(rtk_vlan_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_c2s_get */

/* Function Name:
 *      rtk_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
 *      None
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_trapPri_get(rtk_pri_t *pPriority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_TRAPPRI_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pPriority, &svlan_cfg.priority, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_trapPri_get */


/* Function Name:
 *      rtk_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
 *      priority - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY
 * Note:
 *      None
 */
int32
rtk_svlan_trapPri_set(rtk_pri_t priority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.priority, &priority, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_SVLAN_TRAPPRI_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_trapPri_set */

/* Function Name:
 *      rtk_svlan_deiKeepState_get
 * Description:
 *      Get svlan dei keep state
 * Input:
 *      None
 * Output:
 *      pEnable - state of keep dei
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_deiKeepState_get(rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_DEIKEEPSTATE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_deiKeepState_get */

/* Function Name:
 *      rtk_svlan_deiKeepState_set
 * Description:
 *      Set svlan dei keep state
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_svlan_deiKeepState_set(rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_DEIKEEPSTATE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_deiKeepState_set */

/* Function Name:
 *      rtk_svlan_lookupType_get
 * Description:
 *      Get lookup type of SVLAN
 * Input:
 *      None
 * Output:
 *      pType 		- lookup type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
rtk_svlan_lookupType_get(rtk_svlan_lookupType_t *pType)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_LOOKUPTYPE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pType, &svlan_cfg.type, sizeof(rtk_svlan_lookupType_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_lookupType_get */

/* Function Name:
 *      rtk_svlan_lookupType_set
 * Description:
 *      Set lookup type of SVLAN
 * Input:
 *      type 		- lookup type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      Must call this API after rtl_svlan_init. Otherwise, there will be some unexpected switch behaviors.
 *      Set lookup type to SVLAN_LOOKUP_C4KVLAN and must create vlan by using API rtk_vlan_create. In the
 *		SVLAN_LOOKUP_C4KVLAN config, rtk_svlan_create and rtk_svlan_destroy will return failed.
 */
int32
rtk_svlan_lookupType_set(rtk_svlan_lookupType_t type)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.type, &type, sizeof(rtk_svlan_lookupType_t));
    SETSOCKOPT(RTDRV_SVLAN_LOOKUPTYPE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_lookupType_set */

/* Function Name:
 *      rtk_svlan_sp2cUnmatchCtagging_get
 * Description:
 *      Get unmatch sp2c egress ctagging state
 * Input:
 *      None
 * Output:
 *      pState     - unmatch cvlan tagging state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_svlan_sp2cUnmatchCtagging_get(rtk_enable_t *pState)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_SVLAN_SP2CUNMATCHCTAGGING_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pState, &svlan_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_sp2cUnmatchCtagging_get */

/* Function Name:
 *      rtk_svlan_sp2cUnmatchCtagging_set
 * Description:
 *      Set unmatch sp2c egress ctagging state
 * Input:
 *      state      - unmatch cvlan tagging state
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_svlan_sp2cUnmatchCtagging_set(rtk_enable_t state)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_SP2CUNMATCHCTAGGING_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_sp2cUnmatchCtagging_set */

/* Function Name:
 *      rtk_svlan_priority_get
 * Description:
 *      Get SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pPriority - priority assigned for the SVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *     None
 */
int32
rtk_svlan_priority_get(rtk_vlan_t svid, rtk_pri_t *pPriority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_PRIORITY_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pPriority, &svlan_cfg.priority, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_priority_get */

/* Function Name:
 *      rtk_svlan_priority_set
 * Description:
 *      Set SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 *      priority - priority assigned for the SVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      This API is used to set priority per SVLAN.
 */
int32
rtk_svlan_priority_set(rtk_vlan_t svid, rtk_pri_t priority)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.priority, &priority, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_SVLAN_PRIORITY_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_priority_set */


/* Function Name:
 *      rtk_svlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_fid_get(rtk_vlan_t svid, rtk_fid_t *pFid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_FID_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pFid, &svlan_cfg.fid, sizeof(rtk_fid_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_fid_get */

/* Function Name:
 *      rtk_svlan_fid_set
 * Description:
 *      Set the filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The FID is effective only in VLAN SVL mode.
 */
int32
rtk_svlan_fid_set(rtk_vlan_t svid, rtk_fid_t fid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.fid, &fid, sizeof(rtk_fid_t));
    SETSOCKOPT(RTDRV_SVLAN_FID_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_fid_set */

/* Function Name:
 *      rtk_svlan_fidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based fid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_fidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_FIDENABLE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_fidEnable_get */


/* Function Name:
 *      rtk_svlan_fidEnable_set
 * Description:
 *      Set svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based fid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
rtk_svlan_fidEnable_set(rtk_vlan_t svid, rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_FIDENABLE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_fidEnable_set */

/* Function Name:
 *      rtk_svlan_enhancedFid_get
 * Description:
 *      Get the enhanced filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEfid - pointer buffer of enhanced filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_enhancedFid_get(rtk_vlan_t svid, rtk_efid_t *pEfid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_ENHANCEDFID_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEfid, &svlan_cfg.efid, sizeof(rtk_efid_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_enhancedFid_get */

/* Function Name:
 *      rtk_svlan_enhancedFid_set
 * Description:
 *      Set the enhanced filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      efid  - enhanced filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The EFID is effective only in VLAN SVL mode.
 */
int32
rtk_svlan_enhancedFid_set(rtk_vlan_t svid, rtk_efid_t efid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.efid, &efid, sizeof(rtk_efid_t));
    SETSOCKOPT(RTDRV_SVLAN_ENHANCEDFID_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_enhancedFid_set */

/* Function Name:
 *      rtk_svlan_enhancedFidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based efid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_enhancedFidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_SVLAN_ENHANCEDFIDENABLE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_enhancedFidEnable_get */


/* Function Name:
 *      rtk_svlan_enhancedFidEnable_set
 * Description:
 *      Set svlan based efid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based efid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
rtk_svlan_enhancedFidEnable_set(rtk_vlan_t svid, rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svid, &svid, sizeof(rtk_vlan_t));
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_ENHANCEDFIDENABLE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_enhancedFidEnable_set */

/* Function Name:
 *      rtk_svlan_dmacVidSelForcedState_set
 * Description:
 *      Set DMAC CVID selection status
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection state
 */
int32
rtk_svlan_dmacVidSelForcedState_set(rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_DMACVIDSELFORCEDSTATE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_dmacVidSelForcedState_set */

/* Function Name:
 *      rtk_svlan_dmacVidSelForcedState_get
 * Description:
 *      Get DMAC CVID selection status
 * Input:
 *      None
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_NULL_POINTER         	- input parameter may be null pointer
 * Note:
 *      This API can get DMAC CVID Selection state
 */
int32
rtk_svlan_dmacVidSelForcedState_get(rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_SVLAN_DMACVIDSELFORCEDSTATE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_dmacVidSelForcedState_get */

/* Function Name:
 *      rtk_svlan_svlanFunctionEnable_get
 * Description:
 *      Get the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_svlan_svlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.enable, pEnable, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_SVLAN_SVLANFUNCTIONENABLE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_svlanFunctionEnable_get */


/* Function Name:
 *      rtk_svlan_svlanFunctionEnable_set
 * Description:
 *      Set the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_svlan_svlanFunctionEnable_set(rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_SVLANFUNCTIONENABLE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_svlanFunctionEnable_set */


/* Function Name:
 *      rtk_svlan_tpidEnable_get
 * Description:
 *      Get the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 * Output:
 *      pEnable - pointer to svlan tpid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rtk_svlan_tpidEnable_get(uint32 svlanIndex, rtk_enable_t *pEnable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.svlanIndex, &svlanIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_SVLAN_TPIDENABLE_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pEnable, &svlan_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_svlan_tpidEnable_get */


/* Function Name:
 *      rtk_svlan_tpidEnable_set
 * Description:
 *      Set the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 *      enable - svlan tpid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 */
int32
rtk_svlan_tpidEnable_set(uint32 svlanIndex, rtk_enable_t enable)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.svlanIndex, &svlanIndex, sizeof(uint32));
    osal_memcpy(&svlan_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SVLAN_TPIDENABLE_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_tpidEnable_set */

/* Function Name:
 *      rtk_svlan_extPortPvid_get
 * Description:
 *      Get extension port default svlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_svlan_extPortPvid_get(uint32 extPort, uint32 *pPvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&svlan_cfg.extPort, &extPort, sizeof(uint32));
    GETSOCKOPT(RTDRV_SVLAN_EXTPORTPVID_GET, &svlan_cfg, rtdrv_svlanCfg_t, 1);
    osal_memcpy(pPvid, &svlan_cfg.pvid, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_svlan_extPortPvid_get */


/* Function Name:
 *      rtk_svlan_extPortPvid_set
 * Description:
 *      Set extension port default svlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
rtk_svlan_extPortPvid_set(uint32 extPort, uint32 pvid)
{
    rtdrv_svlanCfg_t svlan_cfg;
    osal_memset(&svlan_cfg, 0x0, sizeof(rtdrv_svlanCfg_t));

    /* function body */
    osal_memcpy(&svlan_cfg.extPort, &extPort, sizeof(uint32));
    osal_memcpy(&svlan_cfg.pvid, &pvid, sizeof(uint32));
    SETSOCKOPT(RTDRV_SVLAN_EXTPORTPVID_SET, &svlan_cfg, rtdrv_svlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_svlan_extPortPvid_set */





