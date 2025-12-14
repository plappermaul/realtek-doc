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
 * Purpose : each chip behavor could be different.
 *
 * Feature :
 *
 */

/*
 * Include Files
 */

#include <util/inc/igmp_common_util.h>
#include "pf_mac.h"
#include "pf_vlan.h"

int32 mcast_ipv4_to_mcast_mac(ipaddr_t ip, mac_address_t mac)
{
	mac[0] = 0x01;
	mac[1] = 0x00;
	mac[2] = 0x5e;
	mac[3] = (ip >> 16) & 0x7f;
	mac[4] = (ip >> 8) & 0xff;
	mac[5] = ip & 0xff;

	return RT_ERR_OK;
}

void pf_flush_mc_vlan_mac_by_vid(int32 vid)
{
	//mac_mcast_t pstMacMcast;
	//rtk_l2_mcastAddr_t stMcastAddr;
	//uint32 uiFid=0;
//TBD, search active group software table, get all mac address with searched vid
//      delete hw entry and sw entry
    #if 0
    while (RT_ERR_OK == pf_mac_mcast_get_by_vid(vid, &pstMacMcast))
	{
		pf_fid_get_by_vid(vid, &uiFid);

		osal_memset(&stMcastAddr, 0, sizeof(stMcastAddr));
   	    osal_memcpy(stMcastAddr.mac.octet, pstMacMcast.mac_addr, sizeof(rtk_mac_t));
		stMcastAddr.fid = uiFid;
		rtk_l2_mcastAddr_del(&stMcastAddr);
    	pf_mac_mcast_delete(&pstMacMcast);
	}
    #endif
	return;
}



int32 pf_mc_vlan_mem_get(uint32 uiVid, rtk_portmask_t *pstPhyMask, rtk_portmask_t *pstPhyMaskUntag)
{
    //uint32 i;
    //uint32 uiIndex = FID_INVALID_ID;

    if (!VALID_VLAN_ID(uiVid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)) ||
        (NULL == pstPhyMask) || (NULL == pstPhyMaskUntag))
    {
        return RT_ERR_INPUT;
    }

    osal_memset(pstPhyMask, 0, sizeof(rtk_portmask_t));
    osal_memset(pstPhyMaskUntag, 0, sizeof(rtk_portmask_t));

    /* TBD: get membr port mask by rtk API */
    SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_get(uiVid, pstPhyMask, pstPhyMaskUntag)), RT_ERR_FAILED);

    /* TBD: get membr port mask by rtk API

    for (i = 0; i <= HAL_VLAN_FID_MAX(); i++)
    {
        if (TRUE == m_astVlanFid[i].bValid)
        {
            if (uiVid == m_astVlanFid[i].uiVid)
            {
                uiIndex = i;
                break;
            }
        }
    }

    if (FID_INVALID_ID != uiIndex)
    {
        osal_memcpy(pstPhyMask, &(m_astVlanFid[uiIndex].stPhyMask), sizeof(rtk_portmask_t));
        osal_memcpy(pstPhyMaskUntag, &(m_astVlanFid[uiIndex].stPhyMaskUntag), sizeof(rtk_portmask_t));
    }
    */

    return RT_ERR_OK;
}

int32 pf_mac_mcast_set_by_mac(mac_mcast_t stMacMcast)
{
	rtk_l2_mcastAddr_t  stMcastAddr;
	rtk_l2_ipmcMode_t   l2mode;

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

    /* TBD Check same multicast mac entry.*/
    osal_memset(&stMcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));

    switch (l2mode)
    {
        case LOOKUP_ON_MAC_AND_VID_FID:
            if (stMacMcast.care_vid)
            {
                stMcastAddr.vid = stMacMcast.tdVid;
                (void)pf_vlan_entry_create((uint32)stMcastAddr.vid);
                stMcastAddr.flags |= RTK_L2_MCAST_FLAG_IVL;
            }
            break;
        default:
            stMcastAddr.fid = 0;
    }

    osal_memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));
    osal_memcpy(&stMcastAddr.portmask, &(stMacMcast.port_mask), sizeof(rtk_portmask_t));

    SYS_ERR_CHK((RT_ERR_OK != rtk_l2_mcastAddr_add(&stMcastAddr)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_mac_mcast_del_by_mac(mac_mcast_t stMacMcast)
{
	rtk_l2_mcastAddr_t  stMcastAddr;
	rtk_l2_ipmcMode_t   l2mode;

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

    /* TBD Check same multicast mac entry.*/
    osal_memset(&stMcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));

    switch (l2mode)
    {
        case LOOKUP_ON_MAC_AND_VID_FID:
            if (stMacMcast.care_vid)
            {
                stMcastAddr.vid = stMacMcast.tdVid;
                (void)pf_vlan_entry_delete((uint32)stMcastAddr.vid);
                stMcastAddr.flags |= RTK_L2_MCAST_FLAG_IVL;
            }
            break;
        default:
            stMcastAddr.fid = 0;
    }

    osal_memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));

    SYS_ERR_CHK((RT_ERR_OK != rtk_l2_mcastAddr_del(&stMcastAddr)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_mac_mcast_get_by_mac(mac_mcast_t stMacMcast, mac_mcast_t *pStMacMcast)
{
	rtk_l2_mcastAddr_t  stMcastAddr;
	rtk_l2_ipmcMode_t   l2mode;

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

    /* TBD Check same multicast mac entry.*/
    osal_memset(&stMcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));

    switch (l2mode)
    {
        case LOOKUP_ON_MAC_AND_VID_FID:
            if (stMacMcast.care_vid)
            {
                stMcastAddr.flags |= RTK_L2_MCAST_FLAG_IVL;
                stMcastAddr.vid = stMacMcast.tdVid;
            }
            break;
        default:
            stMcastAddr.fid = 0;
    }

    osal_memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));

    SYS_ERR_CHK((RT_ERR_OK != rtk_l2_mcastAddr_get(&stMcastAddr)), RT_ERR_FAILED);

    osal_memcpy(&(pStMacMcast->port_mask), &(stMcastAddr.portmask), sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32 pf_unknow_mcast_action_set(mcast_dir_port_info_t dir_port, uint32 type, mcast_unknow_act_t enActMode)
{
    rtk_port_t      port;
    rtk_action_t    stAction;

    SYS_ERR_CHK((MCAST_DIR_PORT_END < dir_port.dir), RT_ERR_INPUT);

    switch (enActMode)
    {
        case MCAST_ACT_FORWARD:
            stAction = ACTION_FORWARD;
            break;
        case MCAST_ACT_DROP:
            stAction = ACTION_DROP;
            break;
        case MCAST_ACT_TRAP2CPU:
            stAction = ACTION_TRAP2CPU;
            break;
        case MCAST_ACT_ROUTER_PORT:
            stAction = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        default :
            return RT_ERR_INPUT;
    }


    if (dir_port.dir & MCAST_DIR_PORT_SPECLAN)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            if (RTK_PORTMASK_IS_PORT_SET(dir_port.port_mask, port))
            {
                if (RT_ERR_OK != rtk_l2_portLookupMissAction_set(port, (rtk_l2_lookupMissType_t)type, stAction))
                    continue;
            }
        }
    }
    // all ports exclude pon port
    if (dir_port.dir & MCAST_DIR_PORT_ALLLAN)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            if (HAL_IS_PON_PORT(port))
                continue;

            if (RT_ERR_OK != rtk_l2_portLookupMissAction_set(port, (rtk_l2_lookupMissType_t)type, stAction))
                continue;
        }
    }
    if (dir_port.dir & MCAST_DIR_PORT_UPLINK)
    {
    	if (MODE_GPON == igmpCtrl.ponMode)
    	{
    		SYS_ERR_CHK((RT_ERR_OK != rtk_l2_portLookupMissAction_set(HAL_GET_PON_PORT(), (rtk_l2_lookupMissType_t)type, stAction)), RT_ERR_FAILED);
    		//only for 9601b test chip since l2	table cannot configure ; should be remove if normal chip
    		if (igmpCtrl.ponMacReplaceL2_enable && stAction == ACTION_DROP)
    		{
    			SYS_ERR_CHK((RT_ERR_OK != rtk_l2_portLookupMissAction_set(HAL_GET_PON_PORT(), (rtk_l2_lookupMissType_t)type, ACTION_FORWARD)), RT_ERR_FAILED);
    		}
    	}
    }
    return RT_ERR_OK;
}

int32 pf_igmp_packet_action_set(uint32 ulAction, uint32 port)
{
    rtk_action_t stAction;

    switch (ulAction)
	{
	    case PASS_TYPE_CPU_ONLY:
			 stAction = ACTION_TRAP2CPU;
			 break;
		case PASS_TYPE_ALLPORTS:
			 stAction = ACTION_FORWARD;
			 break;
		case PASS_TYPE_DROP:
			 stAction = ACTION_DROP;
			 break;
		case PASS_TYPE_CPU_EXCLUDE:
			 stAction = ACTION_FORWARD_EXCLUDE_CPU;
			 break;
		default:
			 return RT_ERR_INPUT;
	}

	if (RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(port, IGMPMLD_TYPE_IGMPV2, stAction) ||
	    RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(port, IGMPMLD_TYPE_IGMPV3, stAction) ||
	    RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(port, IGMPMLD_TYPE_MLDV1, stAction))
    {
        return RT_ERR_FAILED;
    }

	/*if IGMPv1 Drop enable, then drop v1 when action is trap igmp*/
    stAction = ((ENABLED == igmpCtrl.igmpv1Drop_enable) ? ACTION_DROP : stAction);

	SYS_ERR_CHK((RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(port, IGMPMLD_TYPE_IGMPV1, stAction)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_lookupmiss_flood_portmask_set(uint32 type, rtk_portmask_t *pFlood_portmask)
{
    SYS_ERR_CHK((!pFlood_portmask), RT_ERR_FAILED);

    SYS_ERR_CHK((RT_ERR_OK != rtk_l2_lookupMissFloodPortMask_set(type, pFlood_portmask)), RT_ERR_FAILED);

	return RT_ERR_OK;
}

int32 pf_lookupmiss_flood_portmask_get(uint32 type, rtk_portmask_t *pFlood_portmask)
{
    SYS_ERR_CHK((!pFlood_portmask), RT_ERR_FAILED);

    SYS_ERR_CHK((RT_ERR_OK != rtk_l2_lookupMissFloodPortMask_get(type, pFlood_portmask)), RT_ERR_FAILED);

	return RT_ERR_OK;
}

int32 pf_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    SYS_ERR_CHK((!pPortMask), RT_ERR_FAILED);

    if (RT_ERR_OK != rtk_switch_port2PortMask_set(pPortMask, portName))
    {
        SYS_DBG(LOG_DBG_IGMP, "port name is not valid \n");
        return RT_ERR_FAILED;
    }

	return RT_ERR_OK;
}

int32 pf_ipMcastMode_set(uint32 mode)
{
	uint32 rtkMode;

	rtkMode = mode;

/*Transfer to rtk value*/
#if defined(CONFIG_RTL9602C_SERIES)
	switch(mode)
	{
		case 3:
		case 2: /*DIP_VID*/
			rtkMode = LOOKUP_ON_DIP_AND_VID_FID;
			break;
		case 0: /*DMAC_VID*/
		default:
			break;
	}
#endif

	return rtk_l2_ipmcMode_set(rtkMode);
}

int32 pf_ipMcGroup_get(ip_mcast_t stIpMcast)
{
	return rtk_l2_ipmcGroup_get((ipaddr_t)(stIpMcast.dip), &stIpMcast.port_mask);
}


int32 pf_ipMcastEntry_add(ip_mcast_t stIpMcast)
{
	rtk_l2_ipMcastAddr_t    ipmcAddr;
    rtk_port_t              port;
	rtk_portmask_t          isInMask, isExMask, maskI, maskE, retMask;
	rtk_l2_ipmcMode_t       l2mode;
	mac_address_t           pMacAddr;

	RTK_PORTMASK_RESET(isInMask);
    RTK_PORTMASK_RESET(isExMask);
	RTK_PORTMASK_RESET(retMask);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(stIpMcast.dip);
    ipmcAddr.sip = (ipaddr_t)(stIpMcast.sip);

	osal_memcpy(&(ipmcAddr.portmask), &(stIpMcast.port_mask), sizeof(rtk_portmask_t));

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);
	//TBD: for the same group for v2 and v3. How to configure l2 entry
	/* l2 mode default is DIP_AND_SIP. if the recieved report is v2 and v2 and
		v3 doesn't co-exist on the same group, set l2 entry is DIP_Only .
		In order to snoop more entries
	*/
	if ((0 == stIpMcast.sip && l2mode == LOOKUP_ON_DIP_AND_SIP) || l2mode == LOOKUP_ON_DIP)
		ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    else
        ipmcAddr.vid = stIpMcast.vid;

    HAL_SCAN_ALL_PORT(port)
    {
		if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.port_mask, port))
		{
        	if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.filter_mode_mask, port))
        	{
        		//exclude
				RTK_PORTMASK_RESET(maskE);
				RTK_PORTMASK_PORT_SET(maskE, port);
				RTK_PORTMASK_OR(isExMask, maskE);
        	}
			else
			{
				//include
				RTK_PORTMASK_RESET(maskI);
				RTK_PORTMASK_PORT_SET(maskI, port);
				RTK_PORTMASK_OR(isInMask, maskI);
			}
		}
    }
    SYS_PRINTF("%s() %d isInMask=%x, isExMask=%x\n", __FUNCTION__, __LINE__, *(isInMask.bits), *(isExMask.bits));

	//set igmp table
	if (igmpCtrl.sys_has_ip_mc_tbl && !(ipmcAddr.flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY))
	{
	    //if (l2mode == LOOKUP_ON_DIP_AND_SIP && 0 != stIpMcast.sip)
	    if (LOOKUP_ON_DIP != l2mode)
        {
    		if (RT_ERR_OK != rtk_l2_ipmcGroup_get(ipmcAddr.dip, &retMask))
    		{
    			// entry NOT exist, add new one
    			if (RT_ERR_OK != rtk_l2_ipmcGroup_add(ipmcAddr.dip, &isExMask))
    			{
    				SYS_PRINTF("igmp add table failed\n");
    				//return RT_ERR_FAILED;
    			}
    		}
    		else
    		{
    			//update portMask
    			RTK_PORTMASK_OR(isExMask, retMask);
    			if (RT_ERR_OK != rtk_l2_ipmcGroup_add(ipmcAddr.dip, &isExMask))
    			{
    				SYS_PRINTF("igmp add table failed\n");
    				//return RT_ERR_FAILED;
    			}
    		}
            //update l2 table portmask
            osal_memcpy(&(ipmcAddr.portmask), &(isInMask), sizeof(rtk_portmask_t));
        }
        else
        {
            // l2 table use hash by dip
            RTK_PORTMASK_OR(ipmcAddr.portmask, stIpMcast.port_mask);
        }
	}
    else
    {
        RTK_PORTMASK_OR(ipmcAddr.portmask, stIpMcast.port_mask);
    }

	if (igmpCtrl.ponMacReplaceL2_enable)
	{
		mcast_ipv4_to_mcast_mac(ipmcAddr.dip, pMacAddr);

		SYS_ERR_CHK((RT_ERR_OK != pf_gpon_macEntry_add(pMacAddr)), RT_ERR_FAILED);

        return RT_ERR_OK;
	}
	else
	{
    	return rtk_l2_ipMcastAddr_add(&ipmcAddr);
	}
}

int32 pf_ipMcastEntry_del(ip_mcast_t stIpMcast)
{
	rtk_l2_ipMcastAddr_t    ipmcAddr;
	rtk_l2_ipmcMode_t       l2mode;
	mac_address_t           pMacAddr;

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

    SYS_DBG(LOG_DBG_IGMP, "%s() %d \n", __FUNCTION__, __LINE__);

    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;

	if ((0 == stIpMcast.sip && l2mode == LOOKUP_ON_DIP_AND_SIP) || l2mode == LOOKUP_ON_DIP)
		ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    else
        ipmcAddr.vid = stIpMcast.vid;

    ipmcAddr.dip = (ipaddr_t)(stIpMcast.dip);

	if (l2mode == LOOKUP_ON_DIP_AND_SIP)
    	ipmcAddr.sip = (ipaddr_t)(stIpMcast.sip);

	osal_memcpy(&(ipmcAddr.portmask), &(stIpMcast.port_mask), sizeof(rtk_portmask_t));

	if (igmpCtrl.ponMacReplaceL2_enable)
	{
		mcast_ipv4_to_mcast_mac(ipmcAddr.dip, pMacAddr);
		if (RT_ERR_OK != pf_gpon_macEntry_del(pMacAddr))
		{
			SYS_PRINTF("pf_gpon_macEntry_add failed\n");
		}
	}
	else
	{
		SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipMcastAddr_del(&ipmcAddr)), RT_ERR_FAILED);
	}

    //if (l2mode == LOOKUP_ON_DIP_AND_SIP && 0 != stIpMcast.sip)
    if (igmpCtrl.sys_has_ip_mc_tbl && !(ipmcAddr.flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY))
	{
		if (!stIpMcast.isGrpRefB &&
			(FALSE == (ipmcAddr.flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY)))
		{
			if (RT_ERR_OK != rtk_l2_ipmcGroup_del(ipmcAddr.dip))
				return RT_ERR_FAILED;
		}
	}

	return RT_ERR_OK;
}

int32 pf_ipMcastEntry_get(ip_mcast_t *pStIpMcast)
{
	rtk_l2_ipMcastAddr_t    ipmcAddr;
	rtk_l2_ipmcMode_t       l2mode;
	mac_address_t           pMacAddr;
	rtk_portmask_t          etherPm;

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;

	if ((0 == pStIpMcast->sip && l2mode == LOOKUP_ON_DIP_AND_SIP) || l2mode == LOOKUP_ON_DIP)
		ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    else
        ipmcAddr.vid = pStIpMcast->vid;

    ipmcAddr.dip = (ipaddr_t)(pStIpMcast->dip);

	if (l2mode == LOOKUP_ON_DIP_AND_SIP)
    	ipmcAddr.sip = (ipaddr_t)(pStIpMcast->sip);

	if (igmpCtrl.ponMacReplaceL2_enable)
	{
		mcast_ipv4_to_mcast_mac(ipmcAddr.dip, pMacAddr);
		SYS_ERR_CHK((RT_ERR_OK != pf_gpon_macEntry_get(pMacAddr)), RT_ERR_FAILED);
		HAL_GET_ETHER_PORTMASK(etherPm);
		RTK_PORTMASK_PORT_CLEAR(etherPm, HAL_GET_PON_PORT());
	    osal_memcpy(&(pStIpMcast->port_mask), &etherPm, sizeof(rtk_portmask_t));
	}
	else
	{
	    if (RT_ERR_OK != rtk_l2_ipMcastAddr_get(&ipmcAddr))
	    {
	    	//pf_ipMcastEntry_del(*pStIpMcast);
	    	SYS_PRINTF("%s() %d pf_ipMcastEntry_del failed\n", __FUNCTION__, __LINE__);
	    	return RT_ERR_FAILED;
	    }
	    osal_memcpy(&(pStIpMcast->port_mask), &(ipmcAddr.portmask), sizeof(rtk_portmask_t));
        if (RTK_L2_IPMCAST_FLAG_DIP_ONLY & ipmcAddr.flags)
            osal_memcpy(&(pStIpMcast->filter_mode_mask), &(ipmcAddr.portmask), sizeof(rtk_portmask_t));
        else
        {
            //TBD
        }
	}
	return RT_ERR_OK;
}

int32 pf_ipMcastEntry_set(ip_mcast_t stIpMcast)
{
	rtk_l2_ipMcastAddr_t    ipmcAddr;
	rtk_l2_ipmcMode_t       l2mode = IPMC_MODE_END;
	mac_address_t           pMacAddr;
	rtk_port_t              port;
	rtk_portmask_t          isInMask, isExMask, dstMask, maskI, maskE, retMask;

	RTK_PORTMASK_RESET(isInMask);
    RTK_PORTMASK_RESET(isExMask);
    RTK_PORTMASK_RESET(dstMask);
	RTK_PORTMASK_RESET(maskE);
	RTK_PORTMASK_RESET(maskI);
	RTK_PORTMASK_RESET(retMask);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(stIpMcast.dip);
    ipmcAddr.sip = (ipaddr_t)(stIpMcast.sip);

	osal_memcpy(&(ipmcAddr.portmask), &(stIpMcast.port_mask), sizeof(rtk_portmask_t));

	SYS_ERR_CHK((RT_ERR_OK != rtk_l2_ipmcMode_get(&l2mode)), RT_ERR_FAILED);

	if ((0 == stIpMcast.sip && l2mode == LOOKUP_ON_DIP_AND_SIP) || l2mode == LOOKUP_ON_DIP)
		ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    else
        ipmcAddr.vid = stIpMcast.vid;

    HAL_SCAN_ALL_PORT(port)
    {
		if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.port_mask, port))
		{
        	if (RTK_PORTMASK_IS_PORT_SET(stIpMcast.filter_mode_mask, port))
        	{
        		//exclude
				RTK_PORTMASK_PORT_SET(maskE, port);
				RTK_PORTMASK_OR(isExMask, maskE);
        	}
			else
			{
				//include
				RTK_PORTMASK_PORT_SET(maskI, port);
				RTK_PORTMASK_OR(isInMask, maskI);
			}
		}
    }
    SYS_PRINTF("%s() %d isInMask=%x, isExMask=%x\n", __FUNCTION__, __LINE__, *(isInMask.bits), *(isExMask.bits));

	//set igmp table
	//if (l2mode == LOOKUP_ON_DIP_AND_SIP && 0 != stIpMcast.sip)
	if (igmpCtrl.sys_has_ip_mc_tbl && !(ipmcAddr.flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY))
	{
	    //if (l2mode == LOOKUP_ON_DIP_AND_SIP && 0 != stIpMcast.sip)
	    if (LOOKUP_ON_DIP != l2mode)
        {
        	if (RT_ERR_OK != rtk_l2_ipmcGroup_get(ipmcAddr.dip, &retMask))
        	{
        		// entry NOT exist, add new one
        		if (RT_ERR_OK != rtk_l2_ipmcGroup_add(ipmcAddr.dip, &isExMask))
        		{
        			SYS_PRINTF("igmp add table failed\n");
        			//return RT_ERR_FAILED;
        		}
        	}
        	else
        	{
        		//update portMask
        		RTK_PORTMASK_OR(isExMask, retMask);
        		if (RT_ERR_OK != rtk_l2_ipmcGroup_add(ipmcAddr.dip, &isExMask))
        		{
        			SYS_PRINTF("igmp add table failed\n");
        			//return RT_ERR_FAILED;
        		}
        	}
            osal_memcpy(&dstMask, &(isInMask), sizeof(rtk_portmask_t));
        }
        else
        {
            // l2 table use hash by dip
            osal_memcpy(&dstMask, &(stIpMcast.port_mask), sizeof(rtk_portmask_t));
        }
	}
    else
    {
        osal_memcpy(&dstMask, &(stIpMcast.port_mask), sizeof(rtk_portmask_t));
    }

    //update l2 table portmask
    if (RT_ERR_OK != rtk_l2_ipMcastAddr_get(&ipmcAddr))
    {
	    osal_memcpy(&(ipmcAddr.portmask), &(dstMask), sizeof(rtk_portmask_t));
    }
    else
    {

        SYS_PRINTF("%s() %d dstMask=%x, portMask=%x, srcPM=%x\n", __FUNCTION__, __LINE__, *(dstMask.bits), *(ipmcAddr.portmask.bits), *(stIpMcast.port_mask.bits));
        //update
	    RTK_PORTMASK_ASSIGN(ipmcAddr.portmask, dstMask);
    }


	if (igmpCtrl.ponMacReplaceL2_enable)
	{
		mcast_ipv4_to_mcast_mac(ipmcAddr.dip, pMacAddr);
		if (RT_ERR_OK != pf_gpon_macEntry_add(pMacAddr))
		{
			SYS_PRINTF("pf_gpon_macEntry_add failed\n");
		}
	}
	else
	{
    	return rtk_l2_ipMcastAddr_add(&ipmcAddr);
	}
	return RT_ERR_OK;
}

/* GPON MAC */
int32 pf_gmac_filter_mode_set(uint32 mode)
{
	return  rtk_gpon_macFilterMode_set((rtk_gpon_macTable_exclude_mode_t)mode);
}

int32 pf_gpon_macEntry_add(mac_address_t mac)
{
	rtk_gpon_macTable_entry_t entry;

	osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));

	osal_memcpy(entry.mac_addr, mac, RTK_GPON_MACTABLE_ADDR_LEN);

	return rtk_gpon_macEntry_add(&entry);
}

int32 pf_gpon_macEntry_del(mac_address_t mac)
{
	rtk_gpon_macTable_entry_t	entry;
	osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));

	osal_memcpy(entry.mac_addr, mac, RTK_GPON_MACTABLE_ADDR_LEN);

	return rtk_gpon_macEntry_del(&entry);
}

int32 pf_gpon_macEntry_get(mac_address_t mac)
{
	uint32 index = 0, i;
	rtk_gpon_macTable_entry_t entry;
    rtk_portmask_t mask, care_mask;

	RTK_PORTMASK_RESET(mask);
	RTK_PORTMASK_RESET(care_mask);
	osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));

	//TBD 64 need to get from HAL or other API
	while (index < GPON_DEV_MAX_MACTBL_NUM)
	{
		if (RT_ERR_OK == rtk_gpon_macEntry_get(index, &entry))
		{
			for (i = 0; i < RTK_GPON_MACTABLE_ADDR_LEN; i++)
			{
				RTK_PORTMASK_PORT_SET(care_mask, i);
				if (mac[i] == entry.mac_addr[i])
				{
					RTK_PORTMASK_PORT_SET(mask, i);
				}
			}
			if (!RTK_PORTMASK_COMPARE(mask, care_mask))
			{
				return RT_ERR_OK;
			}
			osal_memset(&entry, 0, sizeof(rtk_gpon_macTable_entry_t));
		}
		index++;
	}
	return RT_ERR_FAILED;
}

