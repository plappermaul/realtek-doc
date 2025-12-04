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
 * $Revision:
 * $Date:
 *
 * Purpose :
 *          (1) Logical port convert to physical port
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <igmp/inc/proto_igmp.h>

#include "pf_mac.h"

int32 rsd_mcast_ctrl_and_data_to_cpu_enable_set(sys_enable_t enable)
{
    mac_address_t           pMcastMac;
    rtk_port_t              port;
    mcast_unknow_act_t      unknow_act;
    rsv_mac_pass_action_t   ctrl_act;
    rtk_portmask_t          etherPm;
    mcast_dir_port_info_t   dir_port;

    SYS_PARAM_CHK(IS_ENABLE_INVALID(enable), SYS_ERR_INPUT);

    pMcastMac[0] = 0x01;
    pMcastMac[1] = 0x00;
    pMcastMac[2] = 0x5e;
    pMcastMac[3] = 0x00;
    pMcastMac[4] = 0x00;
    pMcastMac[5] = 0x01;

    if (igmpCtrl.ponMacReplaceL2_enable && ENABLED == enable)
    {
        /* default set gpon multicast-filter-mode is include mode = 0 */
        SYS_ERR_CHK((RT_ERR_OK != pf_gmac_filter_mode_set(0)), SYS_ERR_FAILED);
        /* add mac address of GQ in gpon mac entry */
        SYS_ERR_CHK((SYS_ERR_OK != pf_gpon_macEntry_add(pMcastMac)), SYS_ERR_FAILED);
    }

    ctrl_act = (ENABLED == enable ? PASS_TYPE_CPU_ONLY : PASS_TYPE_ALLPORTS);

    HAL_SCAN_ALL_PORT(port)
    {
        if (HAL_IS_CPU_PORT(port))
            continue;

        pf_igmp_packet_action_set(ctrl_act, port);
    }

    unknow_act = (ENABLED == enable ? MCAST_ACT_DROP : MCAST_ACT_FORWARD);
    osal_memset(&dir_port, 0, sizeof(dir_port));
    dir_port.dir = MCAST_DIR_PORT_END;
    pf_unknow_mcast_action_set(dir_port, DLF_TYPE_IPMC, unknow_act);
    pf_unknow_mcast_action_set(dir_port, DLF_TYPE_IP6MC, unknow_act);
    pf_unknow_mcast_action_set(dir_port, DLF_TYPE_MCAST, unknow_act);

    if (MCAST_ACT_FORWARD == unknow_act)
    {
        RTK_PORTMASK_RESET(etherPm);
        HAL_GET_ETHER_PORTMASK(etherPm); /* include pon portmask  */
		pf_lookupmiss_flood_portmask_set(DLF_TYPE_MCAST, &etherPm);
		pf_lookupmiss_flood_portmask_set(DLF_TYPE_IPMC, &etherPm);
    }
    return SYS_ERR_OK;
}


int32 rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(sys_enable_t enable, uint32 port)
{
    uint32                  phyPortId;
    uint16                  idx;
    mac_address_t           pMcastMac;
    mcast_unknow_act_t      unknow_act_per_port;
    rsv_mac_pass_action_t   ctrl_act_per_port;
    rtk_portmask_t          portMask;
    sys_enable_t            is_lan_all_trap = ENABLED;
    mcast_dir_port_info_t   dir_port, dir_port_spec;

    SYS_PARAM_CHK(IS_ENABLE_INVALID(enable), SYS_ERR_INPUT);

    pMcastMac[0] = 0x01;
    pMcastMac[1] = 0x00;
    pMcastMac[2] = 0x5e;
    pMcastMac[3] = 0x00;
    pMcastMac[4] = 0x00;
    pMcastMac[5] = 0x01;

    if (igmpCtrl.ponMacReplaceL2_enable && ENABLED == enable && (port == HAL_GET_PON_PORT() + 1))
    {
        //default set gpon multicast-filter-mode is include mode = 0 */
        SYS_ERR_CHK((RT_ERR_OK != pf_gmac_filter_mode_set(0)), SYS_ERR_FAILED);
        /* add mac address of GQ in gpon mac entry */
        SYS_ERR_CHK((SYS_ERR_OK != pf_gpon_macEntry_add(pMcastMac)), SYS_ERR_FAILED);
    }

    ctrl_act_per_port = (ENABLED == enable ? PASS_TYPE_CPU_ONLY : PASS_TYPE_ALLPORTS);
    unknow_act_per_port = (ENABLED == enable ? MCAST_ACT_DROP : MCAST_ACT_FORWARD);

    osal_memset(&dir_port, 0, sizeof(dir_port));

    if (INVALID_PORT != (phyPortId = PortLogic2PhyID(port)))
    {
        pf_igmp_packet_action_set(ctrl_act_per_port, phyPortId);

        // if all lan ports or pon port trap, set mcast action, drop for PON and LAN ports
        // if one of all lan ports does not trap, set mcast action forward on PON port.

        FOR_EACH_LOGIC_PORT(idx)
        {
            if (idx == (HAL_GET_PON_PORT() + 1) || idx == (HAL_GET_RGMII_PORT() +1))
                continue;

            if (IS_LOGIC_PORTMASK_PORTSET(igmpCtrl.igmpMldTrap_enable, idx))
                continue;
            else
            {
                is_lan_all_trap = DISABLED;
                break;
            }
        }

        RTK_PORTMASK_RESET(portMask);
        pf_lookupmiss_flood_portmask_get(DLF_TYPE_IPMC, &portMask);
        if (MCAST_ACT_DROP == unknow_act_per_port)
        {
            RTK_PORTMASK_PORT_CLEAR(portMask, phyPortId);
        }
        else if (MCAST_ACT_FORWARD == unknow_act_per_port)
        {
            RTK_PORTMASK_PORT_SET(portMask, phyPortId);
        }
        RTK_PORTMASK_PORT_CLEAR(portMask, HAL_GET_CPU_PORT());
        RTK_PORTMASK_PORT_SET(portMask, HAL_GET_PON_PORT());
        pf_lookupmiss_flood_portmask_set(DLF_TYPE_IPMC, &portMask);
        if (MCAST_ACT_FORWARD == unknow_act_per_port || MCAST_ACT_DROP == unknow_act_per_port)
        {
            osal_memset(&dir_port_spec, 0, sizeof(dir_port_spec));
            dir_port_spec.dir = MCAST_DIR_PORT_SPECLAN;
            RTK_PORTMASK_PORT_SET(dir_port_spec.port_mask, phyPortId);
            pf_unknow_mcast_action_set(dir_port_spec, DLF_TYPE_IPMC, unknow_act_per_port);
        }
        RTK_PORTMASK_RESET(portMask);
        pf_lookupmiss_flood_portmask_get(DLF_TYPE_MCAST, &portMask);
        if (MCAST_ACT_DROP == unknow_act_per_port)
        {
            RTK_PORTMASK_PORT_CLEAR(portMask, phyPortId);
        }
        else if (MCAST_ACT_FORWARD == unknow_act_per_port)
        {
            RTK_PORTMASK_PORT_SET(portMask, phyPortId);
        }
        RTK_PORTMASK_PORT_CLEAR(portMask, HAL_GET_CPU_PORT());
        RTK_PORTMASK_PORT_SET(portMask, HAL_GET_PON_PORT());
        pf_lookupmiss_flood_portmask_set(DLF_TYPE_MCAST, &portMask);
        if (MCAST_ACT_FORWARD == unknow_act_per_port || MCAST_ACT_DROP == unknow_act_per_port)
        {
            osal_memset(&dir_port_spec, 0, sizeof(dir_port_spec));
            dir_port_spec.dir = MCAST_DIR_PORT_SPECLAN;
            RTK_PORTMASK_PORT_SET(dir_port_spec.port_mask, phyPortId);
            pf_unknow_mcast_action_set(dir_port_spec, DLF_TYPE_MCAST, unknow_act_per_port);
        }

        dir_port.dir = MCAST_DIR_PORT_UPLINK;
        // one of all ports does not trap,
        pf_unknow_mcast_action_set(dir_port, DLF_TYPE_IPMC, (!is_lan_all_trap ? MCAST_ACT_FORWARD : MCAST_ACT_DROP));
        pf_unknow_mcast_action_set(dir_port, DLF_TYPE_IP6MC, (!is_lan_all_trap ? MCAST_ACT_FORWARD : MCAST_ACT_DROP));
        pf_unknow_mcast_action_set(dir_port, DLF_TYPE_MCAST, (!is_lan_all_trap ? MCAST_ACT_FORWARD : MCAST_ACT_DROP));
    }
    return SYS_ERR_OK;
}

int32 rsd_mcast_lookup_mode_set(igmp_lookup_mode_t mode)
{
	SYS_ERR_CHK((SYS_ERR_OK != pf_ipMcastMode_set(mode)), SYS_ERR_FAILED);

    return SYS_ERR_OK;
}

int32 rsd_l2McastEntry_add(sys_l2McastEntry_t *pEntry)
{
    mac_mcast_t         stMacMcast;
    sys_logic_port_t    port = 0;
    uint32              uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    osal_memset(&stMacMcast, 0, sizeof(stMacMcast));
    osal_memcpy(stMacMcast.mac_addr, pEntry->mac, MAC_ADDR_LEN);

    stMacMcast.tdVid    = pEntry->vid;
    stMacMcast.care_vid = pEntry->care_vid;

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stMacMcast.port_mask, uiPPort);
    }

    SYS_DBG(LOG_DBG_IGMP,
        "%s() :vid %d mac"MAC_PRINT", care_vid=%u\n",
        __FUNCTION__, stMacMcast.tdVid, MAC_PRINT_ARG(stMacMcast.mac_addr), stMacMcast.care_vid);

    if (RT_ERR_OK == pf_mac_mcast_set_by_mac(stMacMcast))
    {
        return SYS_ERR_OK;
    }
    return SYS_ERR_FAILED;
}

int32 rsd_l2McastEntry_del(sys_l2McastEntry_t *pEntry)
{
    mac_mcast_t         stMacMcast;
    sys_logic_port_t    port = 0;
    uint32              uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    osal_memset(&stMacMcast, 0, sizeof(stMacMcast));
    osal_memcpy(stMacMcast.mac_addr, pEntry->mac, MAC_ADDR_LEN);

    stMacMcast.tdVid    = pEntry->vid;
    stMacMcast.care_vid = pEntry->care_vid;

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stMacMcast.port_mask, uiPPort);
    }

    SYS_DBG(LOG_DBG_IGMP,
        "%s() :vid %d mac"MAC_PRINT", care-vid=%u\n",
        __FUNCTION__, stMacMcast.tdVid, MAC_PRINT_ARG(stMacMcast.mac_addr), stMacMcast.care_vid);

    if (RT_ERR_OK == pf_mac_mcast_del_by_mac(stMacMcast))
    {
        return SYS_ERR_OK;
    }
    return SYS_ERR_FAILED;
}

int32 rsd_l2McastEntry_get(sys_l2McastEntry_t *pEntry)
{
 	mac_mcast_t         stMacMcast;
	mac_mcast_t         retMacMcast;
    uint32              port_no, uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

	osal_memset(&stMacMcast, 0, sizeof(stMacMcast));
	osal_memcpy(stMacMcast.mac_addr, pEntry->mac, MAC_ADDR_LEN);
	stMacMcast.tdVid    = pEntry->vid;
    stMacMcast.care_vid = pEntry->care_vid;

	SYS_DBG(LOG_DBG_IGMP, "%s() :vid %d mac"MAC_PRINT"\n",
	    __FUNCTION__, pEntry->vid, MAC_PRINT_ARG(stMacMcast.mac_addr));

    if (RT_ERR_OK == pf_mac_mcast_get_by_mac(stMacMcast, &retMacMcast))
    {
        LOGIC_PORTMASK_CLEAR_ALL(pEntry->portmask);
		LgcPortFor(port_no)
        {
        	uiPPort = PortLogic2PhyID(port_no);
        	if (RTK_PORTMASK_IS_PORT_SET(retMacMcast.port_mask, uiPPort))
        	{
                LOGIC_PORTMASK_SET_PORT(pEntry->portmask, port_no);
        	}
        }
        return SYS_ERR_OK;
    }
    return SYS_ERR_FAILED;
}

int32 rsd_l2McastEntry_set(sys_l2McastEntry_t *pEntry)
{
    int32              ret;
    sys_logic_port_t   port = 0;
    mac_mcast_t        stMacMcast;
    uint32             uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    osal_memset(&stMacMcast, 0, sizeof(mac_mcast_t));
    osal_memcpy(stMacMcast.mac_addr, pEntry->mac, MAC_ADDR_LEN);

    stMacMcast.tdVid    = pEntry->vid;
    stMacMcast.care_vid = pEntry->care_vid;

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stMacMcast.port_mask, uiPPort);
    }
    /* MLDv2
    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->fmode)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stIpMcast.filter_mode_mask, uiPPort);
    }
    */
    if (IS_LOGIC_PORTMASK_CLEAR(stMacMcast.port_mask))
    	ret = pf_mac_mcast_del_by_mac(stMacMcast);
    else
    	ret = pf_mac_mcast_set_by_mac(stMacMcast);

    ret = (RT_ERR_OK == ret ? SYS_ERR_OK : SYS_ERR_FAILED);

    SYS_DBG(LOG_DBG_IGMP,
        "ret = %d, portMask = %x\n", ret, *(stMacMcast.port_mask.bits));

    return ret;

}

int32 rsd_ipMcGroup_get(void)
{
    int32               ret;
	ip_mcast_t          stIpMcast;

	osal_memset(&stIpMcast, 0, sizeof(stIpMcast));

    ret = pf_ipMcGroup_get(stIpMcast);

    ret = (RT_ERR_DRIVER_NOT_FOUND == ret ? SYS_ERR_FAILED : SYS_ERR_OK);

    return ret;
}

int32 rsd_ipMcastEntry_add(sys_ipMcastEntry_t *pEntry)
{
    int32               ret;
	ip_mcast_t          stIpMcast;
	sys_logic_port_t    port = 0;
	uint32              uiPPort;

	SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

	osal_memset(&stIpMcast, 0, sizeof(stIpMcast));

	stIpMcast.vid = pEntry->vid;
	stIpMcast.dip = pEntry->dip;
	stIpMcast.sip = pEntry->sip;
    stIpMcast.care_vid = pEntry->care_vid;

	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
	{
		uiPPort = PortLogic2PhyID(port);
		RTK_PORTMASK_PORT_SET(stIpMcast.port_mask, uiPPort);
	}

	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->fmode)
	{
		uiPPort = PortLogic2PhyID(port);
		RTK_PORTMASK_PORT_SET(stIpMcast.filter_mode_mask, uiPPort);
	}

    ret = pf_ipMcastEntry_add(stIpMcast);
    ret = (RT_ERR_OK == ret ? SYS_ERR_OK : SYS_ERR_FAILED);

    SYS_DBG(LOG_DBG_IGMP,
        "ret=%d,  rsd_ipMcastEntry_add: dip="IPADDR_PRINT" sip="IPADDR_PRINT" vid=%u\n",
        ret, IPADDR_PRINT_ARG(stIpMcast.dip), IPADDR_PRINT_ARG(stIpMcast.sip), stIpMcast.vid);

    return ret;
}
int32 rsd_ipMcastEntry_del(sys_ipMcastEntry_t *pEntry)
{
    int32               ret;
	ip_mcast_t          stIpMcast;
	sys_logic_port_t    port = 0;
	uint32              uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);
	osal_memset(&stIpMcast, 0, sizeof(stIpMcast));

	stIpMcast.vid       = pEntry->vid;
	stIpMcast.dip       = pEntry->dip;
	stIpMcast.sip       = pEntry->sip;
	stIpMcast.isGrpRefB = pEntry->isGrpRefB;
    stIpMcast.care_vid  = pEntry->care_vid;

	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
	{
		uiPPort = PortLogic2PhyID(port);
		RTK_PORTMASK_PORT_SET(stIpMcast.port_mask, uiPPort);
	}

	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->fmode)
	{
		uiPPort = PortLogic2PhyID(port);
		RTK_PORTMASK_PORT_SET(stIpMcast.filter_mode_mask, uiPPort);
	}

    ret = pf_ipMcastEntry_del(stIpMcast);
    ret = (RT_ERR_OK == ret ? SYS_ERR_OK : SYS_ERR_FAILED);

	SYS_DBG(LOG_DBG_IGMP,
        "ret =%d, rsd_ipMcastEntry_del: dip="IPADDR_PRINT " sip ="IPADDR_PRINT"\n",
        ret, IPADDR_PRINT_ARG(stIpMcast.dip) , IPADDR_PRINT_ARG(stIpMcast.sip));
    return ret;
}

int32 rsd_ipMcastEntry_get(sys_ipMcastEntry_t *pEntry)
{
	ip_mcast_t          stIpMcast;
    int32               ret;
    uint32              port_no, uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

	osal_memset(&stIpMcast, 0, sizeof(stIpMcast));

    stIpMcast.vid       = pEntry->vid;
	stIpMcast.dip       = pEntry->dip;
	stIpMcast.sip       = pEntry->sip;
	stIpMcast.isGrpRefB = pEntry->isGrpRefB;
    stIpMcast.care_vid  = pEntry->care_vid;

	ret = pf_ipMcastEntry_get(&stIpMcast);
    if (RT_ERR_OK == ret)
    {
        LOGIC_PORTMASK_CLEAR_ALL(pEntry->portmask);
        LOGIC_PORTMASK_CLEAR_ALL(pEntry->fmode);
		LgcPortFor(port_no)
        {
        	uiPPort = PortLogic2PhyID(port_no);
        	if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.port_mask, uiPPort))
        	{
	            LOGIC_PORTMASK_SET_PORT(pEntry->portmask, port_no);
        	}

            if (0 == stIpMcast.sip)
            {
                if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.filter_mode_mask, uiPPort))
                {
                    LOGIC_PORTMASK_SET_PORT(pEntry->fmode, port_no);
                }
            }
            else
            {
                if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.filter_mode_mask, uiPPort))
                {
                    LOGIC_PORTMASK_CLEAR_PORT(pEntry->fmode, port_no);
                }
            }
        }
    }
    ret = (RT_ERR_OK == ret ? SYS_ERR_OK : SYS_ERR_FAILED);

	SYS_DBG(LOG_DBG_IGMP,
        "ret %d rsd_ipMcastEntry_get: dip="IPADDR_PRINT " sip ="IPADDR_PRINT" fmode=%x\n",
        ret, IPADDR_PRINT_ARG(stIpMcast.dip) , IPADDR_PRINT_ARG(stIpMcast.sip), *(pEntry->fmode.bits));

	return ret;
}

int32 rsd_ipMcastEntry_set(sys_ipMcastEntry_t *pEntry)
{
    int32              ret;
    sys_logic_port_t   port = 0;
    ip_mcast_t         stIpMcast;
    uint32             uiPPort;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    osal_memset(&stIpMcast, 0, sizeof(stIpMcast));
    stIpMcast.vid       = pEntry->vid;
    stIpMcast.dip       = pEntry->dip;
    stIpMcast.sip       = pEntry->sip;
    stIpMcast.isGrpRefB = pEntry->isGrpRefB;
    stIpMcast.care_vid  = pEntry->care_vid;

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->portmask)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stIpMcast.port_mask, uiPPort);
    }

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->fmode)
    {
        uiPPort = PortLogic2PhyID(port);
        RTK_PORTMASK_PORT_SET(stIpMcast.filter_mode_mask, uiPPort);
    }

    if (IS_LOGIC_PORTMASK_CLEAR(stIpMcast.port_mask))
    	ret = pf_ipMcastEntry_del(stIpMcast);
    else
    	ret = pf_ipMcastEntry_set(stIpMcast);

    ret = (RT_ERR_OK == ret ? SYS_ERR_OK : SYS_ERR_FAILED);

    SYS_DBG(LOG_DBG_IGMP,
        "ret %d, rsd_ipMcastEntry_set: dip="IPADDR_PRINT " sip ="
        IPADDR_PRINT" portMask=%x, filter_mode_mask=%x, isGrpRefB=%u\n",
        ret, IPADDR_PRINT_ARG(stIpMcast.dip), IPADDR_PRINT_ARG(stIpMcast.sip),
        *(stIpMcast.port_mask.bits), *(stIpMcast.filter_mode_mask.bits),
        stIpMcast.isGrpRefB);

    return ret;

}

int32 rsd_l2McastEntryIncCPU_set(sys_l2McastEntry_t *pEntry)
{
    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    rsd_l2McastEntry_get(pEntry);
    LOGIC_PORTMASK_SET_PORT(pEntry->portmask, (HAL_GET_CPU_PORT() + 1));
    rsd_l2McastEntry_set(pEntry);

	return SYS_ERR_OK;
}

