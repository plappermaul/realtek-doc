/*
* Copyright (C) 2010 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
*
* $Revision: 10455 $
* $Date: 2010-06-25 18:27:53 +0800 (Fri, 25 Jun 2010) $
*
* Purpose : Export the public APIs in lower layer module
*
* Feature : Export the public APIs in lower layer module
*
*/

/*
* Include Files
*/

#include <igmp/inc/proto_igmp.h>
#include <igmp/inc/proto_igmp_tx.h>

const ip_addr_t proxy_query                    = {{0, 0, 0, 0}};
const ip_addr_t dvmrp_routing                  = {{224, 0, 0, 4}};
const ip_addr_t ospf_all_routing               = {{224, 0, 0, 5}};
const ip_addr_t ospf_designated_routing        = {{224, 0, 0, 6}};
const ip_addr_t pimv2_routing                  = {{224, 0, 0, 13}};

const ipv6_addr_t ipv6_dip_query1              = {{0xff, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01}};
const ipv6_addr_t ipv6_dip_query2              = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01}};
const ipv6_addr_t ipv6_dip_leave1              = {{0xff, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x02}};
const ipv6_addr_t ipv6_dip_leave2              = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x02}};
const ipv6_addr_t ipv6_dip_leave3              = {{0xff, 0x05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x02}};

const ipv6_addr_t ipv6_dvmrp_routing           = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x04}};
const ipv6_addr_t ipv6_ospf_all_routing        = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x05}};
const ipv6_addr_t ipv6_ospf_designated_routing = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x06}};
const ipv6_addr_t ipv6_pimv2_routing           = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0d}};

const ipv6_addr_t ipv6_mldv2_report            = {{0xff, 0x02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x16}};
const ipv6_addr_t ipv6_proxy_query             = {{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}};

//TBD:  rsd_nic_isPortSTPForwad_ret(sys_logic_port_t port, sys_vid_t vlan)
#define IS_PORT_STP_FWD     TRUE

void mcast_process_delay(multicast_ipType_t ipType)
{
    if (mcast_groupAdd_stop)
    {
        osal_time_usleep(50 * 1000);
    }
    else
    {
        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            osal_time_usleep(20 * 1000);
        }
        else
        {
            osal_time_usleep(40 * 1000);
        }
    }
    return;
}

int32 mcast_vlan_portIgrFilter_check(sys_vid_t vid, sys_logic_port_t port, multicast_ipType_t ipType)
{
    sys_enable_t    igrFilterEnable = DISABLED;
    sys_vlanmask_t  vlanExist;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 ||ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    //SYS_ERR_CHK(rsd_vlan_portIgrFilterEnable_get(port, &igrFilterEnable));

    if (igrFilterEnable == ENABLED)
    {
        VLANMASK_SET_ALL(vlanExist);//SYS_ERR_CHK(rsd_vlan_entryExist_get(vlanExist));
        if (!VLANMASK_IS_VIDSET(vlanExist, vid))
        {
            if (MULTICAST_TYPE_IPV4 == ipType)
            {
                SYS_DBG(LOG_DBG_IGMP, "VLAN %u not exist !\n", vid);
            }
            else
            {
                SYS_DBG(LOG_DBG_MLD, "VLAN %u not exist !\n", vid);
            }
            return SYS_ERR_FAILED;
        }
    }

    return SYS_ERR_OK;
}

static uint8  mcast_igmp_dmac_check(uint8 *pMac, uint32 ipAddr)
{
    uint32 dip;

    SYS_PARAM_CHK((!pMac), FALSE);

    dip = *((uint32 *)(pMac + 2));

    if ((dip & 0x7fffff) == (ipAddr & 0x7fffff))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static int32 mcast_igmp_check_header(uint8 *pMac, iphdr_t *pIpHdr)
{
    uint16          i, size1, size2, igmplen, numOfRec = 0;
    uint32          group;
    igmp_hdr_t      *pIgmpHdr = NULL;
    igmpv3_report_t *report = NULL;
    igmpv3_grec_t   *grec = NULL, *ptr;
    uint16 csum = 0;

    SYS_PARAM_CHK((!pMac), SYS_ERR_NULL_POINTER);

    if (pIpHdr->ihl < 5 || pIpHdr->version != 4)
    {
        SYS_DBG(LOG_DBG_IGMP, "IP Header Error("IPADDR_PRINT"): Version or Length Incorrect pIpHdr->ihl %02x pIpHdr->version %02x!\n",
            IPADDR_PRINT_ARG(pIpHdr->dip),pIpHdr->ihl,pIpHdr->version);
        return ERRONEOUS_PACKET;
    }

    if (!IGMP_IN_MULTICAST(ntohl(pIpHdr->dip)))
    {
        SYS_DBG(LOG_DBG_IGMP, "IP Header Error: Dst IP("IPADDR_PRINT") is not in "
            "Multicast range(224.0.0.0 ~ 239.255.255.255)\n", IPADDR_PRINT_ARG(pIpHdr->dip));
        return ERRONEOUS_PACKET;
    }

    if (!mcast_igmp_dmac_check(pMac, pIpHdr->dip))
    {
        SYS_DBG(LOG_DBG_IGMP, "IP Header Error: Dst IP("IPADDR_PRINT") is not match "
            "The Mac address "MAC_PRINT" \n", IPADDR_PRINT_ARG(pIpHdr->dip), MAC_PRINT_ARG(pMac));
        return ERRONEOUS_PACKET;
    }

    /* Routing protocol packet */
#if 0
    if (IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, dvmrp_routing.addr) ||
        IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, ospf_all_routing.addr) ||
        IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, ospf_designated_routing.addr) ||
        IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, pimv2_routing.addr))
    {
        return ROUTER_ROUTING_PACKET;
    }
#endif

    if (IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, dvmrp_routing.addr))
    {
        if (DVMRP_PROTO_ID == pIpHdr->protocol)
            return ROUTER_ROUTING_PACKET;
        else
            return ERRONEOUS_PACKET;
    }

    if (IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, ospf_all_routing.addr) ||
        IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, ospf_designated_routing.addr) )
    {
        if (OSPF_PROTO_ID == pIpHdr->protocol)
            return ROUTER_ROUTING_PACKET;
        else
            return ERRONEOUS_PACKET;
    }

    if (IS_IPADDR_EQUAL_UINT32(pIpHdr->dip, pimv2_routing.addr))
    {
        if (PIM_PROTO_ID == pIpHdr->protocol)
            return ROUTER_ROUTING_PACKET;
        else
            return ERRONEOUS_PACKET;
    }

    /* IPPROTO_IGMP */
    if (2 != pIpHdr->protocol)
    {
        /* IPPROTO_RAW */
        if (255 == pIpHdr->protocol)
        {
            /* This is a multicast data packet */
            return MULTICAST_DATA_PACKET;
        }

        return VALID_OTHER_PACKET;
    }

    pIgmpHdr = (igmp_hdr_t *)((char *)pIpHdr + pIpHdr->ihl * 4);

    /* Routing protocol packet */
    if (IGMP_PIMV1_ROUTING_PKT == pIgmpHdr->type)
        return ROUTER_ROUTING_PACKET;

    if (pIgmpHdr->type != IGMP_HOST_MEMBERSHIP_QUERY &&
        pIgmpHdr->type != IGMP_HOST_MEMBERSHIP_REPORT &&
        pIgmpHdr->type != IGMP_HOST_NEW_MEMBERSHIP_REPORT &&
        pIgmpHdr->type != IGMP_HOST_LEAVE_MESSAGE &&
        pIgmpHdr->type != IGMPv3_HOST_MEMBERSHIP_REPORT)
    {
        SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Unsupported IGMP version, type = 0x%X!\n", pIgmpHdr->type);
        return UNSUPPORTED_IGMP_CONTROL_PACKET;
    }

    /* IGMPv3 header length */
    igmplen = pIpHdr->tot_len - (pIpHdr->ihl * 4);

    if (IGMPv3_HOST_MEMBERSHIP_REPORT == pIgmpHdr->type)
    {
        if (ntohl(0xE0000016L) != ntohl(pIpHdr->dip))
        {
            SYS_DBG(LOG_DBG_IGMP, "Dst IP("IPADDR_PRINT") in IGMPv3 Report is not 224.0.0.22\n",
                IPADDR_PRINT_ARG(pIpHdr->dip));
            return ERRONEOUS_PACKET;
        }

        report = (igmpv3_report_t*)((char*)pIpHdr + pIpHdr->ihl * 4);

        numOfRec = (NULL == (&(report->ngrec)) ? 0 : report->ngrec);

        igmp_util_checksum_get((uint16 *)report, igmplen, &csum);
        if (0 != csum)
        {
            SYS_DBG(LOG_DBG_IGMP, "IGMPv3 Header Error: Inorrect Checksum!\n");
            return ERRONEOUS_PACKET;
        }

        if (0 == numOfRec)
        {
            return SUPPORTED_IGMP_CONTROL_PACKET;
        }

        grec = report->grec;

        size1 = sizeof(igmpv3_grec_t);

        for (i = 0; i < numOfRec; i++)
        {
            group = ntohl(grec->grec_mca);
            size2 = ((!grec) ? 0 : ntohs(grec->grec_nsrcs) * 4);
            ptr = (igmpv3_grec_t*)((char*) grec + size1 + size2);
            grec = ptr;

            if (!IGMP_IN_MULTICAST(ntohl(group)))
            {
                SYS_DBG(LOG_DBG_IGMP, "IGMPv3 Header Error: Group Address("IPADDR_PRINT") is not in "
                    "Multicast range(224.0.0.0~239.255.255.255)\n", IPADDR_PRINT_ARG(group));
                return ERRONEOUS_PACKET;
            }

            if ((ntohl(group) >= ntohl(0xE0000000L)) && (ntohl(group) <= ntohl(0xE00000FFL)))
            {
                SYS_DBG(LOG_DBG_IGMP, "IGMPv3 Header Error: Group Address("IPADDR_PRINT") belongs to "
                    "Reserved Multicast range(224.0.0.0~224.0.0.255)!\n", IPADDR_PRINT_ARG(group));
                return ERRONEOUS_PACKET;
            }
        }

        return SUPPORTED_IGMP_CONTROL_PACKET;
    }

    igmp_util_checksum_get((uint16 *)pIgmpHdr, igmplen, &csum);

    if (0 != csum)
    {
        SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Inorrect Checksum!\n");
        return ERRONEOUS_PACKET;
    }

    if ((IGMP_HOST_MEMBERSHIP_QUERY == pIgmpHdr->type) && (0 == pIgmpHdr->groupAddr))
    {
        ;
    }
    else
    {
        if (!IGMP_IN_MULTICAST(ntohl(pIgmpHdr->groupAddr)))
        {
            SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Group Address("IPADDR_PRINT") is not in "
                "Multicast range(224.0.0.0~239.255.255.255)\n", IPADDR_PRINT_ARG(pIgmpHdr->groupAddr));
            return ERRONEOUS_PACKET;
        }

        if ((ntohl(pIgmpHdr->groupAddr) >= ntohl(0xE0000000L)) && (ntohl(pIgmpHdr->groupAddr) <= ntohl(0xE00000FFL)))
        {
            SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Group Address("IPADDR_PRINT") belongs to "
                "Reserved Multicast range(224.0.0.0~224.0.0.255)!\n", IPADDR_PRINT_ARG(pIgmpHdr->groupAddr));
            return ERRONEOUS_PACKET;
        }
    }

    if (IGMP_HOST_MEMBERSHIP_QUERY == pIgmpHdr->type)
    {
        if ((0 == pIgmpHdr->groupAddr) && (ntohl(0xE0000001L) != ntohl(pIpHdr->dip)))
        {
            /* General Query */
            SYS_DBG(LOG_DBG_IGMP, "IP Header Error: Dst IP("IPADDR_PRINT") of IGMP GENERAL-QUERY "
                "packet is incorrect!\n", IPADDR_PRINT_ARG(pIpHdr->dip));
            return ERRONEOUS_PACKET;
        }

        if ((0 != pIgmpHdr->groupAddr) && (ntohl(0xE0000001L) == ntohl(pIpHdr->dip)))
        {
            /*Error General Query */
            SYS_DBG(LOG_DBG_IGMP, "Igmp Header Error: group IP("IPADDR_PRINT") of IGMP GENERAL-QUERY "
                "packet is incorrect!\n", IPADDR_PRINT_ARG(pIgmpHdr->groupAddr));
            return ERRONEOUS_PACKET;
        }

        if ((0 != pIgmpHdr->groupAddr) && (pIgmpHdr->groupAddr != pIpHdr->dip))
        {
            /* GS-Specific Query */
            SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Group Address("IPADDR_PRINT") is not same "
                "with the Dst IP("IPADDR_PRINT") of IGMP GS-Specific QUERY packet!\n",
                IPADDR_PRINT_ARG(pIgmpHdr->groupAddr), IPADDR_PRINT_ARG(pIpHdr->dip));
            return ERRONEOUS_PACKET;
        }
    }

    if (((pIgmpHdr->type == IGMP_HOST_MEMBERSHIP_REPORT) ||
        (pIgmpHdr->type == IGMP_HOST_NEW_MEMBERSHIP_REPORT)) &&
        (pIgmpHdr->groupAddr != pIpHdr->dip))
    {
        SYS_DBG(LOG_DBG_IGMP, "IGMP Header Error: Group Address("IPADDR_PRINT") is not same "
            "with the Dst IP("IPADDR_PRINT") of IGMP REPORT packet!\n",
            IPADDR_PRINT_ARG(pIgmpHdr->groupAddr), IPADDR_PRINT_ARG(pIpHdr->dip));
        return ERRONEOUS_PACKET;
    }

    if ((pIgmpHdr->type == IGMP_HOST_LEAVE_MESSAGE) && (ntohl(0xE0000002L) != ntohl(pIpHdr->dip)))
    {
        SYS_DBG(LOG_DBG_IGMP, "IP Header Error: Dst IP("IPADDR_PRINT") of IGMP LEAVE packet is incorrect!\n", IPADDR_PRINT_ARG(pIpHdr->dip));
        return ERRONEOUS_PACKET;
    }

    return SUPPORTED_IGMP_CONTROL_PACKET;
}

int32 mcast_igmp_up_handle_membership_query(sys_nic_pkt_t *pBuf, uint16 vid, iphdr_t *pIpHdr, igmp_hdr_t *pIgmpHdr)
{
    uint32                  t, qqic = 0, group, *pSourceAddr = NULL, aclEntryId = UINT_MAX, lastIntvl = 0;
	uint32                  lastMbrIntv = DFLT_IGMP_GS_QUERY_RESP_INTV;
    uint8                   robussVar = 0, query_version = IGMP_QUERY_V2, srcList[512], isSpecQuery = TRUE;
    uint8                   *pSrcList = srcList;
    uint16                  i, grpVid, srcNum = 0, sortedIdx;
    igmp_router_entry_t     *pEntry = NULL;
    igmp_group_entry_t      *pGroup = NULL, groupEntry, tmpGroupEntry;
    sys_logic_port_t        port;
    sys_logic_portmask_t    blockPm, txPm, eachPm;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      *pMcastVlan = NULL, mcastVlanEntry;
    mcast_groupAddr_t       groupAddr;


    osal_memset(&groupAddr,0, sizeof(mcast_groupAddr_t));
    LOGIC_PORTMASK_CLEAR_ALL(blockPm);
    LOGIC_PORTMASK_SET_PORT(blockPm, pBuf->rx_tag.source_port);
    LOGIC_PORTMASK_CLEAR_ALL(txPm);

    group = ntohl (pIgmpHdr->groupAddr);
    groupAddr.ipv4 = group;

	SYS_DBG(LOG_DBG_IGMP,
        "%s() %d source_port=%u, logical pon port=%u\n",
        __FUNCTION__, __LINE__, pBuf->rx_tag.source_port, (HAL_GET_PON_PORT() + 1));

	if (pBuf->rx_tag.source_port != (HAL_GET_PON_PORT() + 1))
	{
		return SYS_ERR_FAILED;
	}

	if (MODE_GPON != igmpCtrl.ponMode)
	{
		FOR_EACH_LOGIC_PORT(port)
	 	{
	 		if (FALSE == mcast_igspVlanMapExist(port, vid))
	 		{
				LOGIC_PORTMASK_SET_PORT(blockPm, port);
			}
		}
	}
	else
	{
		if (0 != group)
		{
			/* change to mcastVid if igmp ctrl packet with dataVid */
			if (SYS_ERR_OK != mcast_igmp_group_vid_by_group_search(ipType, groupAddr, &grpVid, &aclEntryId))
			{
				SYS_DBG(LOG_DBG_IGMP, "%s() %d search FAILED gprVid=%u\n", __FUNCTION__, __LINE__, grpVid);
				return SYS_ERR_FAILED;
			}
			mcast_profile_time_intv_get(INTV_TYPE_SPECIFIC_RSP, aclEntryId, NULL, &lastMbrIntv);

			if (!igmpCtrl.igmpVlanTag_type)
				vid = grpVid;
            if (igmpCtrl.igmpVlanTag_type && 0 == grpVid)
                vid = grpVid;
		}
	}

	if (MODE_GPON != igmpCtrl.ponMode)
	{
    	mcast_vlan_db_get(vid, ipType, &pMcastVlan);
    	if (NULL == pMcastVlan)
        	return SYS_ERR_FAILED;
        //received query. than clear the suppre flag.
        mcast_suppreFlag_clear(vid, ipType);
	}

    /* Dont handle packet that has the same source ip address with device self. */
    /* To do: level up this concept to rsd_nic layer or acl layer */
    if(pIpHdr->sip == igmpCtrl.sys_ip)
    {
        SYS_DBG(LOG_DBG_IGMP, "The source ip is the same to the device ip, free the packet! \n");
        return SYS_ERR_OK;
    }

    // 8 == (sizeof(igmp_hdr_t) - sizeof(pIgmpHdr->v3)
    if ( (pIpHdr->tot_len - 4 * pIpHdr->ihl ) > 8)
    {
        query_version = IGMP_QUERY_V3;
    }

    if ((IGMP_QUERY_V3 == query_version) && (p_igmp_stats->oper_version < IGMP_VERSION_V3_BASIC))
    {
        sys_logic_portmask_t txPmsk;

        LOGIC_PORTMASK_SET_ALL(txPmsk);
        LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

    	SYS_DBG(LOG_DBG_IGMP, "%s() %d txPmsk=%x\n", __FUNCTION__, __LINE__, *(txPmsk.bits));
        mcast_snooping_tx(pBuf, vid, pBuf->length, &txPmsk);

        p_igmp_stats->g_query_xmit++;

        return SYS_ERR_OK;
    }

    if (IGMP_QUERY_V3 != query_version)
    {
        if (group == 0)     /* general query */
        {
			t = pIgmpHdr->maxRespTime / 10 ?
                pIgmpHdr->maxRespTime / 10 :
                (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->responseTime : 10);

            p_igmp_stats->g_query_rcvd++;

            isSpecQuery = FALSE;

            SYS_DBG(LOG_DBG_IGMP,
                "A IGMP General QUERY received on Vid = %d, Port-%d from "IPADDR_PRINT". (Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDR_PRINT_ARG(pIpHdr->sip), t);
        }
        else                /* v2 group specific query */
        {
           	t = pIgmpHdr->maxRespTime / 10 ?
                pIgmpHdr->maxRespTime / 10 :
                (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->responseTime : 10);

			if (pIgmpHdr->maxRespTime / 10)
            {
                lastIntvl = pIgmpHdr->maxRespTime / 10;
            }

            p_igmp_stats->gs_query_rcvd++;

            SYS_DBG(LOG_DBG_IGMP,
                "A IGMP Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDR_PRINT". (Group:"
                IPADDR_PRINT" Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDR_PRINT_ARG(pIpHdr->sip), IPADDR_PRINT_ARG(group), t);
        }
    }
    else
    {
        srcNum = ntohl(pIgmpHdr->v3.numOfSrc);

        if (pIgmpHdr->maxRespTime & 0x80)
        {
            t = ((pIgmpHdr->maxRespTime & 0x0F) | 0x10) << (((pIgmpHdr->maxRespTime & 0x70) >> 4) + 3);
            t = t / 10;
        }
        else
        {
            t = pIgmpHdr->maxRespTime/10;
        }

        if (t)
          lastIntvl = t;

        if (pIgmpHdr->v3.qqic & 0x80)
        {
            qqic = ((pIgmpHdr->v3.qqic & 0x0F) | 0x10) << (((pIgmpHdr->v3.qqic & 0x70) >> 4) + 3);
        }
        else
        {
            qqic = pIgmpHdr->v3.qqic;
        }

        robussVar =  pIgmpHdr->v3.rsq & 0x07;

        /* general query */
        if (group == 0)
        {
			t = t  ?  t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->responseTime : 10);

			p_igmp_stats->v3.g_queryV3_rcvd++;

            isSpecQuery = FALSE;

            SYS_DBG(LOG_DBG_IGMP, "A IGMP General QUERY v3 received on Vid = %d, Port-%d from "IPADDR_PRINT
                ". (Interval:%d sec)\n",
                vid, pBuf->rx_tag.source_port, IPADDR_PRINT_ARG(pIpHdr->sip), t);
        } /*v3 group specific query*/
        else if (0 == srcNum)
        {
           	t = t ? t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->grpSpecificQueryIntv : 1);

            p_igmp_stats->v3.gs_queryV3_rcvd++;

            SYS_DBG(LOG_DBG_IGMP,
                "A IGMP v3 Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDR_PRINT". (Group:"
                IPADDR_PRINT" Interval:%d.%d)\n",
                vid, pBuf->rx_tag.source_port, IPADDR_PRINT_ARG(pIpHdr->sip), IPADDR_PRINT_ARG(group), t/10, t%10);
        }
        else  /*v3 group and src specific query*/
        {
            osal_memset(srcList,0,sizeof(srcList));
			pSourceAddr = &(pIgmpHdr->v3.srcList[0]);
            uint16 i_max = (srcNum > 20) ? 20 : srcNum;

            for(i = 0; i < i_max; i++)
            {
                pSrcList += sprintf((char *)pSrcList, IPADDR_PRINT, IPADDR_PRINT_ARG(pIgmpHdr->v3.srcList[i]));
                pSrcList += sprintf((char *)pSrcList, "  ");
            }

			t = t ? t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->grpSpecificQueryIntv : 1);

            p_igmp_stats->v3.gss_queryV3_rcvd++;

            SYS_DBG(LOG_DBG_IGMP,
                "A IGMP Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDR_PRINT". (Group:"IPADDR_PRINT
                " Source List : %s, Interval:%d.%d)\n",
                vid, pBuf->rx_tag.source_port, IPADDR_PRINT_ARG(pIpHdr->sip), IPADDR_PRINT_ARG(group), srcList, t / 10, t % 10);
        }
    }

    if (!IS_IPADDR_EQUAL_UINT32(pIpHdr->sip, proxy_query.addr))
    {
        /* Add Source Port to query port */
		#if 0//#ifndef ONU_STYLE
        SYS_ERR_CHK((SYS_ERR_OK != mcast_routerPort_add_wrapper(vid, ipType, blockPm)), SYS_ERR_FAILED);
		#endif
    }

	SYS_DBG(LOG_DBG_IGMP, "%s() %d\n", __FUNCTION__, __LINE__);

    osal_memset(&mcastVlanEntry,0, sizeof(mcastVlanEntry));
    mcastVlanEntry.vid = vid;
    mcastVlanEntry.ipType = ipType;
    SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlanEntry)), SYS_ERR_FAILED);

    if (isSpecQuery)
    {
        mcastVlanEntry.operGsqueryIntv = lastIntvl ? lastIntvl : mcastVlanEntry.grpSpecificQueryIntv;
        if (IGMP_QUERY_V3 == query_version)
        {
            mcastVlanEntry.operLastMmbrQueryCnt = robussVar ? robussVar : mcastVlanEntry.operRobNessVar;
            mcastVlanEntry.operRobNessVar = robussVar ? robussVar : mcastVlanEntry.robustnessVar;
        }
        else
        {
             //update operGsquery interval
            mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.operRobNessVar;
            mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar;
        }
    }
    else
    {
        mcastVlanEntry.operRespTime = t;

        if (IGMP_QUERY_V3 == query_version)
        {
            mcastVlanEntry.operQueryIntv = qqic ? qqic : mcastVlanEntry.queryIntv;
            mcastVlanEntry.operRobNessVar = robussVar ? robussVar : mcastVlanEntry.robustnessVar;
        }
        else
        {
            mcastVlanEntry.operQueryIntv = mcastVlanEntry.queryIntv * 1;
            mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar * 1;
        }
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_set(&mcastVlanEntry)), SYS_ERR_FAILED);

	/* Only specific query need to update software timer */
    if (isSpecQuery)
    {
        SYS_ERR_CHK(SYS_ERR_OK != (mcast_group_portTime_update(ipType, vid, &groupAddr, srcNum, pSourceAddr,
            lastMbrIntv)), SYS_ERR_FAILED);
    }

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    /* Handle Querier election */
	    mcast_igmp_querier_check(vid, pIpHdr->sip, query_version);

	    mcast_router_db_get(vid, ipType, &pEntry);

	    if (pEntry != NULL)
	    {
			SYS_DBG(LOG_DBG_IGMP, ": %s() %d find entry in router db \n", __FUNCTION__, __LINE__);

	        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, blockPm)
	        {
	            if (IS_LOGIC_PORTMASK_PORTSET(pEntry->router_forbid_pmsk, port))
	            {
	                SYS_DBG(LOG_DBG_IGMP, "port %d is forbidden router port \n", port);
	                return SYS_ERR_OK;
	            }
	        }

	       LOGIC_PORTMASK_OR(txPm, txPm, pEntry->router_pmsk);
	    }
	}

	/* query action: tag handle and fwd port  */

	SYS_DBG(LOG_DBG_IGMP, "%s() %d\n", __FUNCTION__, __LINE__);

    if (group == 0)
    {
		SYS_DBG(LOG_DBG_IGMP, "%s() %d group is zero \n", __FUNCTION__, __LINE__);
		sys_logic_portmask_t txPmsk;

		LOGIC_PORTMASK_SET_ALL(txPmsk);
		LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

		FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, txPmsk)
		{
			sys_nic_pkt_t oldBuf;

            osal_memcpy(&oldBuf, pBuf, sizeof(sys_nic_pkt_t));

            uint8 *pData = (uint8 *)osal_alloc(sizeof(uint8) * (pBuf->length));

            if (pData)
			{
				osal_memcpy(pData, pBuf->data, pBuf->length);
				oldBuf.data = pData;
			}

			mcast_query_treatment(&oldBuf, port);
			LOGIC_PORTMASK_CLEAR_ALL(eachPm);
			LOGIC_PORTMASK_SET_PORT(eachPm, port);
			mcast_snooping_tx(&oldBuf, vid, oldBuf.length, &eachPm);
			osal_free(pData);
			pData = NULL;
		}

        if (query_version == IGMP_QUERY_V3)
        {
            p_igmp_stats->v3.g_queryV3_xmit++;      /* General query */
        }
        else
        {
            p_igmp_stats->g_query_xmit++;      /* General query */
        }
    }
    else
    {
        mcast_build_groupEntryKey(vid, ipType, group, NULL, &groupEntry);

        if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
        {
            srcNum = ((!pIgmpHdr) ? 0 : ntohl(pIgmpHdr->v3.numOfSrc));

            for (i = 0; i < srcNum; i++)
            {
                groupEntry.sip = pIgmpHdr->v3.srcList[i];
                pGroup = mcast_group_get(&groupEntry);

                if (NULL != pGroup)
                {
                    LOGIC_PORTMASK_OR(txPm, txPm, pGroup->mbr);
                }
            }
			if (MODE_GPON == igmpCtrl.ponMode && !srcNum)
			{
				/*For Group-specify query without source IP list, set sip to 0xffffffff for ignore source IP compare*/
				memcpy(&tmpGroupEntry,&groupEntry,sizeof(igmp_group_entry_t));
				tmpGroupEntry.sip=0xffffffff;
				mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroupEntry), &sortedIdx, &pGroup);

                if (NULL != pGroup)
				{
					LOGIC_PORTMASK_OR(txPm, txPm,pGroup->mbr);
				}
			}
        }
        else
        {
            mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &pGroup);

            if (NULL != pGroup)
            {
                LOGIC_PORTMASK_OR(txPm, txPm,pGroup->mbr);
            }

        }

        //delete rx port.
        LOGIC_PORTMASK_CLEAR_PORT(txPm, pBuf->rx_tag.source_port);

		SYS_DBG(LOG_DBG_IGMP, "%s() %d  igmp would be start to send txPm=%02x\n", __FUNCTION__, __LINE__, *(txPm.bits));
        if (!IS_LOGIC_PORTMASK_CLEAR(txPm))
        {
        	if (MODE_GPON == igmpCtrl.ponMode)
        	{
	        	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, txPm)
	        	{
					sys_nic_pkt_t oldBuf;
					osal_memcpy(&oldBuf, pBuf, sizeof(sys_nic_pkt_t));
					uint8 *pData = (uint8 *)osal_alloc(sizeof(uint8) * (oldBuf.length));
					osal_memcpy(pData, pBuf->data, pBuf->length);

                    if (pData)
						oldBuf.data = pData;

                    mcast_query_treatment(&oldBuf, port);
					LOGIC_PORTMASK_CLEAR_ALL(eachPm);
					LOGIC_PORTMASK_SET_PORT(eachPm, port);
					mcast_snooping_tx(&oldBuf, vid, oldBuf.length, &eachPm);
					osal_free(pData);
					pData = NULL;
	        	}
        	}
			else
			{
				SYS_DBG(LOG_DBG_IGMP, "%s() %d	!IS_LOGIC_PORTMASK_CLEAR \n", __FUNCTION__, __LINE__);
            	mcast_snooping_tx(pBuf, vid, pBuf->length, &txPm);
			}

            if (query_version == IGMP_QUERY_V3)
            {
                srcNum = ntohl(pIgmpHdr->v3.numOfSrc);

                if(srcNum == 0)
                    p_igmp_stats->v3.gs_queryV3_xmit++;
                else
                    p_igmp_stats->v3.gss_queryV3_xmit ++;
            }
            else
            {
                /* v2 group specific query */
                p_igmp_stats->gs_query_xmit++;
            }
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv12_up_handle_membership_report(
    sys_nic_pkt_t *pBuf, uint16 vid, iphdr_t *pIpHdr, igmp_hdr_t *pIgmpHdr, struct timespec *pRecvTime)
{
    uint8                               sendFlag = 0;
    uint16                              tmpvlanId[MAX_VLAN_NUMBER];
    uint32                              num = 0, i = 0, port;
    sys_logic_portmask_t                rt_pmsk;
    multicast_ipType_t                  ipType = MULTICAST_TYPE_IPV4;
    mcast_groupAddr_t                   groupAddr;
	igmp_vlan_translation_entry_vid_t   translation_entry[16];
	ipAddr_t                            clientIp;

    osal_memset(&rt_pmsk, 0, sizeof(sys_logic_portmask_t));
	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
    clientIp.isIpv6B = 0;
    clientIp.ipAddr.ipv4Addr = pIpHdr->sip;

    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_IGMP, "A IGMPv1/2 REPORT received on Vid = %d, Port = %d.\n",
        vid, pBuf->rx_tag.source_port);

    groupAddr.ipv4 = pIgmpHdr->groupAddr;

	if (IGMP_MODE_CTC != igmpCtrl.igmpMode)
	{
		SYS_DBG(LOG_DBG_IGMP, "A IGMPv1/2 REPORT received on Vid = %d, Port = %d , cvid_tagged = %d.\n",
            vid, pBuf->rx_tag.source_port, pBuf->rx_tag.cvid_tagged);

		if (MODE_GPON != igmpCtrl.ponMode &&
			(TRUE == mcast_ismvlan(vid) || FALSE == pBuf->rx_tag.cvid_tagged))
		{

			if (TAG_OPER_MODE_TRANSLATION != mcast_igmpTagOperPerPortGet(port))
			{
				mcast_igmpMcVlanGet(port, tmpvlanId, &num);

				for (i = 0; i < num; i++)
				{
					//SYS_DBG(LOG_DBG_IGMP,"mcast_igmpv12_up_handle_membership_report:vid %d group"IPADDR_PRINT" port %d\n",tmpvlanId[i],IPADDR_PRINT_ARG(groupAddr.ipv4),port);
		    		mcast_groupMbrPort_add_wrapper(
		    		    ipType, tmpvlanId[i], pBuf, 0, &groupAddr, port, &sendFlag, clientIp, pRecvTime);
				}
			}
			else
			{
				mcast_igmpTagTranslationEntryGetPerPort(port, translation_entry, &num);

				for (i = 0; i < num; i++)
				{
					//SYS_DBG(LOG_DBG_IGMP,"mcast_igmpv12_up_handle_membership_report:mcast_groupMbrPort_add_wrapper:vid %d group"IPADDR_PRINT" port %d\n",translation_entry[i].iptv_vlan,IPADDR_PRINT_ARG(groupAddr.ipv4),port);
		    		mcast_groupMbrPort_add_wrapper(
		    		    ipType, translation_entry[i].userVid, pBuf, 0, &groupAddr, port, &sendFlag, clientIp, pRecvTime);
				}
			}
		}
		else
		{
		    SYS_ERR_CHK((SYS_ERR_OK != mcast_groupMbrPort_add_wrapper(
                ipType, vid, pBuf, 0, &groupAddr, port, &sendFlag, clientIp, pRecvTime)), SYS_ERR_FAILED);
		}

	    if (ENABLED == p_igmp_stats->suppre_enable && !sendFlag)
	        return SYS_ERR_OK;
	}

	LOGIC_PORTMASK_SET_PORT(rt_pmsk, (HAL_GET_PON_PORT() + 1));

    if (!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
    {
        SYS_DBG(LOG_DBG_IGMP, "%s() %d len=%u\n", __FUNCTION__, __LINE__, pBuf->length);
        if (SYS_ERR_OK == mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk))
        {
        	//Not support mld and without snooping case
        	p_igmp_stats->p_port_info[port].sJoinCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].sJoinCount) ?
        	                                            0 : p_igmp_stats->p_port_info[port].sJoinCount + 1);
			SYS_DBG(LOG_DBG_IGMP,
                "%s() %d sucessfully  join count:%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].sJoinCount);
		}
		else
		{
		    p_igmp_stats->p_port_info[port].usJoinCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].usJoinCount) ?
                                                        0 : p_igmp_stats->p_port_info[port].usJoinCount + 1);
            SYS_DBG(LOG_DBG_IGMP,
                "%s() %d un-sucessfully join count:%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].usJoinCount);
		}
        p_igmp_stats->report_xmit++;

		SYS_DBG(LOG_DBG_IGMP, "%s() %d report xmit count = %u \n", __FUNCTION__, __LINE__, p_igmp_stats->report_xmit);
    }


    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_isIn(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, struct timespec *pRecvTime, mcast_preview_info_t	previewInfo)
{
    if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
    {
        mcast_igmpv3_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        mcast_igmpv3_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, grpMbrTimerIntv, previewInfo);
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_isEx(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, struct timespec *pRecvTime, mcast_preview_info_t	previewInfo)
{
    if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
    {
        mcast_igmpv3_basic_groupMbrPort_isEx_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        mcast_igmpv3_groupMbrPort_isEx_proc(pRcd,vid,portid, MODE_IS_EXCLUDE, clientIp, grpMbrTimerIntv, previewInfo);
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_toIn(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, struct timespec *pRecvTime, mcast_preview_info_t	previewInfo)
{
    if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
    {
        mcast_igmpv3_basic_groupMbrPort_toIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        mcast_igmpv3_groupMbrPort_toIn_proc(pRcd,vid,portid, clientIp, grpMbrTimerIntv, previewInfo);
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_toEx(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, struct timespec *pRecvTime, mcast_preview_info_t	previewInfo)
{
    if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
    {
        mcast_igmpv3_basic_groupMbrPort_toEx_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        mcast_igmpv3_groupMbrPort_isEx_proc(pRcd,vid,portid, CHANGE_TO_EXCLUDE_MODE, clientIp, grpMbrTimerIntv, previewInfo);
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_allow(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, struct timespec *pRecvTime, mcast_preview_info_t	previewInfo)
{
    mcast_igmpv3_handle_isIn(pRcd, vid, portid, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_handle_block(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t	previewInfo)
{
    if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
    {
        mcast_igmpv3_basic_groupMbrPort_block_proc(pRcd,vid,portid, clientIp);
    }
    else
    {
        mcast_igmpv3_groupMbrPort_block_proc(pRcd,vid,portid, clientIp, grpMbrTimerIntv, previewInfo);
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_up_handle_membership_report(
    sys_nic_pkt_t *pBuf, uint16 vid, iphdr_t *pIpHdr,  igmpv3_report_t *pIgmpRpt, struct timespec *pRecvTime)
{
    uint16 numofRcds = 0;
    uint8 *groupRcds = NULL;
    uint16  i = 0;
    uint8   rcdType = 0xff;
    sys_logic_port_t port;
    uint16 numofsrc, grpVid = 0;
    uint8 auxlen;
	uint32 aclEntryId = UINT_MAX, imputedGrpBw, grpSrcIp = 0;
    sys_logic_portmask_t rt_pmsk;
	uint32 originalLen = pBuf->length, newLen = 0;

    multicast_ipType_t  ipType = MULTICAST_TYPE_IPV4;
    mcast_groupAddr_t groupAddr;
	ipAddr_t clientIp;
	uint32 grpMbrTimerIntv = DFLT_IGMP_GROUP_MEMBERSHIP_INTV;
	mcast_preview_info_t	previewInfo;

    osal_memset(&rt_pmsk, 0, sizeof(sys_logic_portmask_t));
    memset(&clientIp, 0, sizeof(ipAddr_t));

	if (ipType == MULTICAST_TYPE_IPV4)
	{
		clientIp.isIpv6B = 0;
		clientIp.ipAddr.ipv4Addr = pIpHdr->sip;
	}

    numofRcds = ((!pIgmpRpt) ? 0 : ntohs(pIgmpRpt->ngrec));
    //rsd_trunk_representPort_get(&representedId, pBuf->rx_tag.source_port);
    /* Get trunk logic port if srcPort is belong to the trunk */
    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_IGMP, "Num of records: %d \n", numofRcds);

    if (numofRcds != 0)
    {
        groupRcds = (uint8 *)(&(pIgmpRpt->grec));
    }

    for (i = 0; i < numofRcds; i++)
    {
        // igmp filter
        osal_memset(&groupAddr, 0 , sizeof(groupAddr));
		osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));
        groupAddr.ipv4 = ntohl(((igmpv3_grec_t *)groupRcds)->grec_mca);
		// TBD: not support multiple grpSrcIp
		if (ntohl(((igmpv3_grec_t *)groupRcds)->grec_nsrcs))
			grpSrcIp = ntohl(((igmpv3_grec_t *)groupRcds)->grec_src[0]);

		SYS_DBG(LOG_DBG_IGMP, "GROUP ("IPADDR_PRINT")\n", IPADDR_PRINT_ARG(groupAddr.ipv4));

		/* TBD: if exist a group range is invaild in IGMPv3 report, currently we drop this packet */
		if (MODE_GPON == igmpCtrl.ponMode)
		{
			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_check(pRecvTime, ipType, port, grpSrcIp, &groupAddr, &aclEntryId,
                &grpVid, &imputedGrpBw, &previewInfo)), SYS_ERR_FAILED);

			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_treatment(pBuf, vid, port, aclEntryId, previewInfo)), SYS_ERR_FAILED);

			newLen = pBuf->length;
			if (newLen > originalLen)
			{
				groupRcds += (newLen - originalLen);
			}
			else if (originalLen > newLen)
			{
				groupRcds -= (originalLen - newLen);
			}
			vid  = grpVid;
		}
		else
		{
			SYS_ERR_CONTINUE(mcast_filter_check(pRecvTime, ipType, port, grpSrcIp, &groupAddr, &aclEntryId, &grpVid,
                &imputedGrpBw, &previewInfo));
		}
		SYS_DBG(LOG_DBG_IGMP, " %s() %d  aclEntryId=%x, vid=%u\n", __FUNCTION__, __LINE__, aclEntryId, vid);
		mcast_profile_time_intv_get(INTV_TYPE_GROUP_MBR, aclEntryId, &previewInfo, &grpMbrTimerIntv);

        rcdType = ((igmpv3_grec_t *)groupRcds)->grec_type;

        switch (rcdType)
        {
            case MODE_IS_INCLUDE:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  MODE_IS_INCLUDE\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.isIn_rcvd++;
                mcast_igmpv3_handle_isIn((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);
                break;
            case MODE_IS_EXCLUDE:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  MODE_IS_EXCLUDE\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.isEx_rcvd++;
                mcast_igmpv3_handle_isEx((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);
                break;
            case CHANGE_TO_INCLUDE_MODE:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  CHANGE_TO_INCLUDE_MODE\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.toIn_rcvd++;
                mcast_igmpv3_handle_toIn((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);
                break;
            case CHANGE_TO_EXCLUDE_MODE:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  CHANGE_TO_EXCLUDE_MODE\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.toEx_rcvd++;
                mcast_igmpv3_handle_toEx((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);
                break;
            case ALLOW_NEW_SOURCES:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  ALLOW_NEW_SOURCES\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.allow_rcvd++;
                mcast_igmpv3_handle_allow((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, pRecvTime, previewInfo);
                break;
            case BLOCK_OLD_SOURCES:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  BLOCK_OLD_SOURCES\n", __FUNCTION__, __LINE__);
                if(i == 0)
                    p_igmp_stats->v3.block_rcvd++;
                mcast_igmpv3_handle_block((igmpv3_grec_t *)groupRcds, vid, port, clientIp, grpMbrTimerIntv, previewInfo);
                break;
            default:
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d  group type is unkown \n", __FUNCTION__, __LINE__);
                break;
        }

        numofsrc = ((igmpv3_grec_t *)groupRcds)->grec_nsrcs;
        auxlen = ((igmpv3_grec_t *)groupRcds)->grec_auxwords;
        groupRcds = groupRcds + 8 + numofsrc*4 + auxlen*4;
    }

	if (MODE_GPON == igmpCtrl.ponMode)
	{
		SYS_DBG(LOG_DBG_IGMP, "%s() %d   \n", __FUNCTION__, __LINE__);
		LOGIC_PORTMASK_SET_PORT(rt_pmsk, (HAL_GET_PON_PORT() + 1));
	}
	else
	{
	    mcast_routerPort_get(vid, ipType, &rt_pmsk);
	    /*this port may be is router port ,so need to clear*/
	    LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);
	}
    if (!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
    {
		SYS_DBG(LOG_DBG_IGMP, " %s() %d  sid=%u\n", __FUNCTION__, __LINE__, pBuf->tx_tag.dst_port_mask);
        if (SYS_ERR_OK == mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk))
        {
            p_igmp_stats->p_port_info[port].sJoinCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].sJoinCount) ? 0 : p_igmp_stats->p_port_info[port].sJoinCount + 1);
			SYS_DBG(LOG_DBG_IGMP, "%s() %d sucessfully  join count:%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].sJoinCount);
		}
		else
		{
		    p_igmp_stats->p_port_info[port].usJoinCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].usJoinCount) ? 0 : p_igmp_stats->p_port_info[port].usJoinCount + 1);
			SYS_DBG(LOG_DBG_IGMP,
                "%s() %d un-sucessfully join count:%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].usJoinCount);
		}
        p_igmp_stats->report_xmit++;
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv2_up_handle_membership_leave(sys_nic_pkt_t *pBuf, uint16 vid, iphdr_t *pIpHdr, igmp_hdr_t *pIgmpHdr)
{
    uint32                              port;
    sys_logic_portmask_t                rt_pmsk;
    multicast_ipType_t                  ipType = MULTICAST_TYPE_IPV4;
    mcast_groupAddr_t                   groupAddr;
    mcast_vlan_entry_t                  mcastVlanEntry;
    igmp_querier_entry_t                *pQuerier = NULL;
    //uint16                            sortedIdx;
    //igmp_group_entry_t                *groupHead = NULL;//, groupEntry;
    //uint8                             isExistGroup = TRUE;
    uint16                              tmpvlanId[MAX_VLAN_NUMBER];
	uint32                              num = 0, i = 0;
	igmp_vlan_translation_entry_vid_t   translation_entry[16];
	ipAddr_t                            clientIp;

    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_IGMP,
        "IGMP Leave("IPADDR_PRINT") received on port = %d\n", IPADDR_PRINT_ARG(pIgmpHdr->groupAddr), port);

	osal_memset(&rt_pmsk, 0, sizeof(sys_logic_portmask_t));
	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 0;
	clientIp.ipAddr.ipv4Addr = pIpHdr->sip;

    osal_memset(tmpvlanId, 0, MAX_VLAN_NUMBER*sizeof(uint16));

    mcast_querier_db_get(vid, ipType, &pQuerier);
    if (NULL != pQuerier)
    {
        if (ENABLED == pQuerier->status)
        {
            osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
            mcastVlanEntry.vid = vid;
            mcastVlanEntry.ipType = ipType;
            SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlanEntry)), SYS_ERR_FAILED);
            if (vid == mcastVlanEntry.vid)
            {
                mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.lastMmbrQueryCnt;
                SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_set(&mcastVlanEntry)), SYS_ERR_FAILED);
            }
        }
    }

    groupAddr.ipv4 = pIgmpHdr->groupAddr;
	#if 0
    SYS_ERR_CHK(mcast_build_groupEntryKey(vid, ipType, groupAddr.ipv4, NULL, &groupEntry));
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    if (NULL == groupHead)
    {
        SYS_DBG(LOG_DBG_IGMP, "Leave Group("IPADDR_PRINT") not exsit\n", IPADDR_PRINT_ARG(pIgmpHdr->groupAddr));
        return SYS_ERR_OK;
    }
    else
    {
        isExistGroup = TRUE;
        //if static vlan, and querier, must send special query.
        if (IGMP_GROUP_ENTRY_DYNAMIC == groupHead->form)
        {
            if ( !IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
            {
                SYS_DBG(LOG_DBG_IGMP, "port %d is not Group("IPADDR_PRINT") member\n",  LP2UP(port),IPADDR_PRINT_ARG(pIgmpHdr->groupAddr));
                return SYS_ERR_OK;
            }
        }
    }
	#endif

	if (MODE_GPON != igmpCtrl.ponMode && (TRUE == mcast_ismvlan(vid) || FALSE == pBuf->rx_tag.cvid_tagged))
	{
		mcast_igmpMcVlanGet(port, tmpvlanId, &num);
		for (i = 0; i < num; i++)
        {
    	    igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_groupMbrPort_del_wrapper(
                ipType, tmpvlanId[i], pBuf, &groupAddr, port, clientIp);
        }
		mcast_igmpTagTranslationEntryGetPerPort(port, translation_entry, &num);
		for (i = 0; i < num; i++)
        {
    	    igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_groupMbrPort_del_wrapper(
                ipType, translation_entry[i].userVid, pBuf, &groupAddr, port, clientIp);
        }
	}
	else
	{
		igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_groupMbrPort_del_wrapper(
            ipType, tmpvlanId[i], pBuf, &groupAddr, port, clientIp);
	}

    //if (isExistGroup)
    {
        //mcast_routerPort_get(vid, ipType,  &rt_pmsk);
        //LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);
    	LOGIC_PORTMASK_SET_PORT(rt_pmsk, (HAL_GET_PON_PORT() + 1));
        if (!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
        {
            mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);
            p_igmp_stats->leave_xmit++;
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_igmp_up_handle_mcst_data(sys_nic_pkt_t *pBuf, uint16 vid, iphdr_t *pIpHdr)
{
    uint8                   port;
    sys_logic_portmask_t    rt_pmsk;
    igmp_group_entry_t      *pEntry, groupEntry;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_groupAddr_t       groupAddr;
    mcast_groupAddr_t       sipAddr;
	ipAddr_t                clientIp;

    SYS_DBG(LOG_DBG_IGMP, "A mcst data packet received.  dip:"IPADDR_PRINT"  SA:"IPADDR_PRINT"\n",
          IPADDR_PRINT_ARG(pIpHdr->dip), IPADDR_PRINT_ARG(pIpHdr->sip));

	memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 0;
	clientIp.ipAddr.ipv4Addr = pIpHdr->sip;

    LOGIC_PORTMASK_CLEAR_ALL(rt_pmsk);

    port = pBuf->rx_tag.source_port;
    groupAddr.ipv4 = pIpHdr->dip;
    sipAddr.ipv4 = pIpHdr->sip;

    if (p_igmp_stats->oper_version > IGMP_VERSION_V2)
    {
        igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmpv3_group_mcstData_add_wrapper(
            ipType, vid, pBuf, &groupAddr, &sipAddr, port, clientIp);
    }
    else
    {
        igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_group_mcstData_add_wrapper(
            ipType, vid, pBuf, &groupAddr, &sipAddr, port, clientIp);
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, pIpHdr->dip, NULL, &groupEntry)), SYS_ERR_FAILED);

    groupEntry.sip = pIpHdr->sip;
    pEntry = mcast_group_get(&groupEntry);
    if (pEntry)
    {
        if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
        {
            //if v3 full, don't only send the pkt to the member port
            if(IGMP_VERSION_V3_FULL != p_igmp_stats->oper_version)
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
                    LOGIC_PORTMASK_SET_ALL(rt_pmsk);
                else if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                    mcast_routerPort_get(vid, ipType, &rt_pmsk);
                else
                    LOGIC_PORTMASK_CLEAR_ALL(rt_pmsk);
            }
        }
        else        /* If this mcst group has been registered, we should forward it to group mbr */
        {
            LOGIC_PORTMASK_COPY(rt_pmsk, pEntry->mbr);
        }
    }

    LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);
    mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);

    return SYS_ERR_OK;
}

int32 mcast_igmp_rx(sys_nic_pkt_t *pPkt, void *pCookie, struct timespec *pRecvTime)
{
    sys_vid_t               vid;
    //uint16                  pl_len;             /* payload length */
    uint16                  tagLen      = 0;  /*ctag / otag length*/
    int32                   result      = 0;
    iphdr_t                 *pIpHdr     = NULL;
    igmp_hdr_t              *pIgmpHdr   = NULL;
    sys_logic_portmask_t    blockPm, txPmsk;
    igmpv3_report_t         *pIgmpv3Rpt = NULL;
    multicast_ipType_t      ipType      = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    uint8                   dmac[MAC_ADDR_LEN];

    SYS_DBG(LOG_DBG_IGMP, "Received IGMP packet  DA:"MAC_PRINT"  SA:"MAC_PRINT"\n",
        MAC_PRINT_ARG(pPkt->data), MAC_PRINT_ARG(pPkt->data + 6));

    /* Check whether the packet has vlan tag or not */
    if (TRUE == pPkt->rx_tag.cvid_tagged)
        tagLen = tagLen + 4;

    if (TRUE == pPkt->rx_tag.svid_tagged)
        tagLen = tagLen + 4;

    vid = ntohs(pPkt->rx_tag.inner_vid);

    if (FALSE == IS_PORT_STP_FWD)
    {
        SYS_DBG(LOG_DBG_IGMP, "Drop packet, Port-%d is not in STP Forwarding State\n",
            pPkt->rx_tag.source_port);
        goto igmp_up_exit;
    }

    LOGIC_PORTMASK_CLEAR_ALL(blockPm);
    LOGIC_PORTMASK_SET_PORT(blockPm, pPkt->rx_tag.source_port);
    LOGIC_PORTMASK_SET_ALL(txPmsk);
    LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

    if (DISABLED == p_igmp_stats->igmpsnp_enable)
    {
        SYS_DBG(LOG_DBG_IGMP, "%s() %d Flood IGMP packet txPmsk=%x becasue IGMP is disabled\n",
            __FUNCTION__, __LINE__, *(txPmsk.bits));
        mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);
        return SYS_ERR_OK;
    }

    if (ENABLED == igmp_packet_stop)
    {
        SYS_DBG(LOG_DBG_IGMP, "Free IGMP packet becasue packet stop sign is enabled\n");
        goto igmp_up_exit;
    }

    pIpHdr = (iphdr_t*)(&pPkt->data[6 + 6 + 2 + tagLen]);

    osal_memcpy(dmac, pPkt->data, MAC_ADDR_LEN);

    pIgmpHdr = (igmp_hdr_t*)((char*)pIpHdr + pIpHdr->ihl * 4);

    if(224 == ((pIgmpHdr->groupAddr >> 24) & 0xff) &&
        0 == ((pIgmpHdr->groupAddr >> 16) & 0xff) &&
        0 == ((pIgmpHdr->groupAddr >> 8) & 0xff))
    {
        SYS_DBG(LOG_DBG_IGMP, "Drop packet, Group is reserved!\n");
        goto igmp_up_exit;
    }

    /* Because 28 use rma to trap igmp control packet so that vlan ingress filter couldn't take effect.
        Need to check vlan ingress filter by software. The process as following.
        1. check whether vlan ingress filter is enable on port
        2. If enable, check wheather rx port is a member port of rx vlan
    */
    if (SYS_ERR_OK != mcast_vlan_portIgrFilter_check(vid, pPkt->rx_tag.source_port, ipType))
    {
        SYS_DBG(LOG_DBG_IGMP, "Drop packet, Port-%d is not in VLAN-%d\n", pPkt->rx_tag.source_port, vid);
        goto igmp_up_exit;
    }

    p_igmp_stats->total_pkt_rcvd++;
    result = mcast_igmp_check_header(dmac, pIpHdr);
    SYS_DBG(LOG_DBG_IGMP, "igmp check header result=%d\n", result);
    switch (result)
    {
        case SUPPORTED_IGMP_CONTROL_PACKET:
            break;

        case UNSUPPORTED_IGMP_CONTROL_PACKET:
            p_igmp_stats->valid_pkt_rcvd++;
            p_igmp_stats->other_rcvd++;

            SYS_DBG(LOG_DBG_IGMP, "Unsupported IGMP Pkt: Port = %d, Vid = %d\n", pPkt->rx_tag.source_port, vid);
            //rsd_nic_pkt_flood(pPkt, vid, blockPm, SYS_CPU_PORT_PRIORITY_IGMP, TRUE);
            goto igmp_up_exit;

        case ERRONEOUS_PACKET:
            SYS_DBG(LOG_DBG_IGMP, "ERRONEOUS_PACKET drop.\n");
            p_igmp_stats->invalid_pkt_rcvd++;
            goto igmp_up_exit;

        case VALID_OTHER_PACKET:
        case MULTICAST_DATA_PACKET:
			p_igmp_stats->invalid_pkt_rcvd++;
			goto igmp_up_exit;

        case ROUTER_ROUTING_PACKET:
            SYS_DBG(LOG_DBG_IGMP, "A Routing Pkt: Port = %d, Vid = %d\n", pPkt->rx_tag.source_port, vid);
			if(pPkt->rx_tag.source_port != (HAL_GET_PON_PORT() + 1))
			{
				goto igmp_up_exit;
			}

			#if 0 //#ifndef ONU_STYLE
            mcast_routerPort_add_wrapper(vid, ipType, blockPm); //why mc not process the router packet,route port is fixed == pon port
            #endif

            //rsd_nic_pkt_flood(pPkt, vid, blockPm, SYS_CPU_PORT_PRIORITY_IGMP, TRUE);
            goto igmp_up_exit;

        default:
            SYS_DBG(LOG_DBG_IGMP, "An unrecognized packet received.\n");
            goto igmp_up_exit;
    }

    p_igmp_stats->valid_pkt_rcvd++;

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        mcast_vlan_db_get(vid, ipType, &pMcastVlan);
        if (NULL == pMcastVlan)
        {
            return SYS_ERR_FAILED;
        }

        /* Check if IGMP Snooping is enabled in this VLAN or not */
        if (ENABLED != pMcastVlan->enable)
        {
            SYS_DBG(LOG_DBG_IGMP, "%s() %d IGMP Snooping is DISABLED txPmsk=%x in VLAN=%d, flood control packets in vlan \n",
                __FUNCTION__, __LINE__, *(txPmsk.bits), vid);
            mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);
            return SYS_ERR_OK;
        }

        /*disable below delay in GPON mode for IGMP rate limit, and why need to delay? check later*/
        mcast_process_delay(ipType);
    }

    switch (pIgmpHdr->type)
    {
        case IGMP_TYPE_MEMBERSHIP_QUERY:
            //pl_len = pIpHdr->tot_len - (pIpHdr->ihl * 4);
            mcast_igmp_up_handle_membership_query(pPkt, vid, pIpHdr, pIgmpHdr);
            break;

        case IGMPV1_TYPE_MEMBERSHIP_REPORT:
        case IGMPV2_TYPE_MEMBERSHIP_REPORT:
            p_igmp_stats->report_rcvd++;
            mcast_igmpv12_up_handle_membership_report(pPkt, vid, pIpHdr, pIgmpHdr, pRecvTime);
            break;

        case IGMPV2_TYPE_MEMBERSHIP_LEAVE:
    		if (MODE_GPON != igmpCtrl.ponMode)
    		{
    			if (p_igmp_stats->oper_version != IGMP_VERSION_V3_FULL)
    	        {
    	            p_igmp_stats->leave_rcvd++;
    	            mcast_igmpv2_up_handle_membership_leave(pPkt, vid, pIpHdr, pIgmpHdr);
    	        }
    		}
    		else
    		{
    			p_igmp_stats->leave_rcvd++;
                mcast_igmpv2_up_handle_membership_leave(pPkt, vid, pIpHdr, pIgmpHdr);
    		}
            break;

        case IGMPV3_TYPE_MEMBERSHIP_REPORT:
            p_igmp_stats->report_rcvd++;
            if (p_igmp_stats->oper_version >= IGMP_VERSION_V3_BASIC)
            {
                pIgmpv3Rpt = (igmpv3_report_t*)((char*)pIpHdr + pIpHdr->ihl * 4);
                mcast_igmpv3_up_handle_membership_report(pPkt, vid, pIpHdr, pIgmpv3Rpt, pRecvTime);
            }
            else
            {
                 mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);
                 //rsd_nic_pkt_flood(pPkt, vid, blockPm, SYS_CPU_PORT_PRIORITY_IGMP, TRUE);
                 p_igmp_stats->report_xmit++;
            }
            break;

        default:
            SYS_DBG(LOG_DBG_IGMP, "An unrecognized IGMP message received.\n");
            p_igmp_stats->other_rcvd++;
            break;
    }

igmp_up_exit:
    //rsd_nic_pkt_free(pPkt);
    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_isIn(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    if (MLD_VERSION_V2_BASIC == p_mld_stats->oper_version)
    {
        mcast_mldv2_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        //not support
        ;
    }

    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_isEx(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    if (MLD_VERSION_V2_BASIC == p_mld_stats->oper_version)
    {
        mcast_mldv2_basic_groupMbrPort_isEx_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        //not support
        ;
    }

    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_toIn(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    if (MLD_VERSION_V2_BASIC == p_mld_stats->oper_version)
    {
        mcast_mldv2_basic_groupMbrPort_toIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        //not support
        ;
    }

    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_toEx(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    if (MLD_VERSION_V2_BASIC == p_mld_stats->oper_version)
    {
        mcast_mldv2_basic_groupMbrPort_toEx_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        //not support
        ;
    }

    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_allow(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    mcast_mldv2_handle_isIn(pRcd, vid, portid, clientIp, pRecvTime);

    return SYS_ERR_OK;
}

int32 mcast_mldv2_handle_block(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    if (MLD_VERSION_V2_BASIC == p_mld_stats->oper_version)
    {
        mcast_mldv2_basic_groupMbrPort_block_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }
    else
    {
        //not support
        ;
    }

    return SYS_ERR_OK;
}

static uint8  mcast_mld_dmac_check(uint8 *pMac, uint32 ipAddr)
{
    uint32 dip;

    if (NULL == pMac)
        return FALSE;

    dip = *((uint32 *)(pMac + 2));

    if (dip != ipAddr)
        return FALSE;

    return TRUE;
}


uint8 is_ipv6Addr_equel(const uint8 *pSrc1Addr, const uint8 *pSrc2Addr)
{
    if (pSrc1Addr == NULL || pSrc2Addr == NULL)
        return FALSE;

    if (osal_memcmp(pSrc1Addr, pSrc2Addr, IPV6_ADDR_LEN) == 0)
        return TRUE;

    return FALSE;
}

static int32 mcast_mld_check_header(uint8 *pMac, ipv6hdr_t *pIpv6Hdr, uint32 optionLen)
{
    uint16          i, size1, size2, numOfRec;
#if 0
    uint16          mldLen;
#endif
    mld_hdr_t       *pMldHdr = NULL;
    mldv2_report_t  *report = NULL;
    mldv2_grec_t    *grec, *ptr;
    uint8           *pGroupIpv6 = NULL;
    uint16          csum = 0;
    uint32          dip;

    if (6 != pIpv6Hdr->version)
    {
        SYS_DBG(LOG_DBG_MLD, "IP Header Error: Version  Incorrect!\n");
        return ERRONEOUS_PACKET;
    }

    dip = IPV6_TO_IPV4(pIpv6Hdr->dipv6);
    if (!mcast_mld_dmac_check(pMac, dip))
    {
         SYS_DBG(LOG_DBG_MLD, "IPv6 Header Error: Dst IPv6("IPADDRV6_PRINT") is not match "
            "The Mac address "MAC_PRINT" \n", IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6), MAC_PRINT_ARG(pMac));
        return ERRONEOUS_PACKET;
    }

    /* Routing protocol packet */
#if 0
    if ((osal_memcmp(pIpv6Hdr->dipv6, ipv6_dvmrp_routing.addr, IPV6_ADDR_LEN) == 0) ||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_ospf_all_routing.addr, IPV6_ADDR_LEN) == 0) ||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_ospf_designated_routing.addr, IPV6_ADDR_LEN) == 0) ||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_pimv2_routing.addr, IPV6_ADDR_LEN) == 0))
#endif
    if (is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_dvmrp_routing.addr))
    {
        return ROUTER_ROUTING_PACKET;
    }

    if ((is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_ospf_all_routing.addr)) ||
        (is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_ospf_designated_routing.addr)))
    {
        return ROUTER_ROUTING_PACKET;
    }

    if (is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_pimv2_routing.addr))
    {
        return ROUTER_ROUTING_PACKET;
    }


    if (pIpv6Hdr->nextHead == NO_NEXT_HEADER  /* Ipv6 data */)
    {
        return MULTICAST_DATA_PACKET;
    }

    pMldHdr = (mld_hdr_t *)((uint8 *)pIpv6Hdr + optionLen + MLD_IPV6_HDR_LEN);

    /* Routing protocol packet */
    if (pMldHdr->type >= MLD_ROUTER_SOLICITATION && pMldHdr->type <= MLD_REDIRECT)
        return ROUTER_ROUTING_PACKET;

    if (pMldHdr->type == MLD_TYPE_MEMBERSHIP_QUERY ||
        pMldHdr->type == MLD_TYPE_MEMBERSHIP_REPORT ||
        pMldHdr->type == MLD_TYPE_MEMBERSHIP_DONE ||
        pMldHdr->type == MLDV2_TYPE_MEMBERSHIP_REPORT)
    {
        SYS_DBG(LOG_DBG_MLD, "MLD type = 0x%X!\n", pMldHdr->type);
    }
    else
    {
        SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Unsupported mld type, type = 0x%X!\n", pMldHdr->type);
        return VALID_OTHER_PACKET;
    }

#if 0
    /* MLDv2  header length */
    mldLen = pIpv6Hdr->tot_len - optionLen;
#endif

    if (pMldHdr->type == MLDV2_TYPE_MEMBERSHIP_REPORT)
    {
        if (!is_ipv6Addr_equel ( pIpv6Hdr->dipv6, ipv6_mldv2_report.addr))
        {
            SYS_DBG(LOG_DBG_MLD, "Dst IPv6("IPADDRV6_PRINT") in MLDv2 Report is not  FF02::16\n",
                IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
            return ERRONEOUS_PACKET;
        }

        report = (mldv2_report_t*)((char*)pMldHdr);

        csum = csum;
#if 0
        rsd_nic_checksum_get((uint16 *)report, mldLen, &csum);
        if (csum != 0)
        {
            SYS_DBG(LOG_DBG_MLD, "MLDv2 Header Error: Inorrect Checksum!\n");
            SYS_PRINTF("%s: %d: %s\n",__FILE__,__LINE__,__FUNCTION__);
            return ERRONEOUS_PACKET;
        }
#endif
        numOfRec = (NULL == (&(report->numRcd)) ? 0 : report->numRcd);

        if (0 == numOfRec)
        {
            return SUPPORTED_IGMP_CONTROL_PACKET;
        }

        grec = report->grec;

        size1 = sizeof(mldv2_grec_t);

        for (i = 0; i < numOfRec; i++)
        {
            pGroupIpv6 = grec->grec_ipv6Mca;
            size2 = grec->grec_nsrcs * IPV6_ADDR_LEN;
            ptr = (mldv2_grec_t*)((char*) grec + size1 + size2);
            grec = ptr;
            //igmp_debug("Group IP = "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(group));

            if (pGroupIpv6[0] != 0xff)
            {
                SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is not  "
                    "Multicast  address \n", IPADDRV6_PRINT_ARG(pGroupIpv6));
                return ERRONEOUS_PACKET;
            }

            /*  rfc4541,
                MLD messages are also not sent regarding groups with addresses in the
                range FF00::/15 (which encompasses both the reserved FF00::/16 and
                node-local FF01::/16 IPv6 address spaces).  These addresses should
                never appear in packets on the link.*/
            if ((pGroupIpv6[0] == 0xff) && ( (pGroupIpv6[1]  & 0xf0) == 0x0) &&
                  (osal_memcmp(&pGroupIpv6[2], &ipv6_proxy_query.addr[2] ,IPV6_ADDR_LEN-2) == 0)  )
            {
                SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is "
                    "invalid Multicast group address !\n", IPADDRV6_PRINT_ARG(pGroupIpv6));

                return ERRONEOUS_PACKET;
            }
        }

        return SUPPORTED_IGMP_CONTROL_PACKET;
    }

#if 0
    tmp = (uint8 *)pMldHdr;
    for(i = 0; i < 24; i++)
    {
        SYS_PRINTF("%2x:", *(tmp+i));
    }
    SYS_PRINTF("\n\n");

    rsd_nic_checksum_get((uint16 *)pMldHdr, mldLen, &csum);
    SYS_PRINTF("chechk sum = %x \n", csum);
    if (csum != 0)
    {
        SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Inorrect Checksum!\n");
        SYS_PRINTF("%s: %d: %s\n",__FILE__,__LINE__,__FUNCTION__);
        return ERRONEOUS_PACKET;
    }
#endif

    if ((pMldHdr->type == MLD_TYPE_MEMBERSHIP_QUERY) &&
        (is_ipv6Addr_equel(pMldHdr->groupIpv6, ipv6_proxy_query.addr)))
    {
        ;
    }
    else
    {
        if (pMldHdr->groupIpv6[0] != 0xff)
        {
            SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is not  "
                "Multicast  address \n", IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
                return ERRONEOUS_PACKET;
        }
        /*  rfc4541,
            MLD messages are also not sent regarding groups with addresses in the
            range FF00::/15 (which encompasses both the reserved FF00::/16 and
            node-local FF01::/16 IPv6 address spaces).  These addresses should
            never appear in packets on the link.*/

        if ((pMldHdr->groupIpv6[0] == 0xff) && ((pMldHdr->groupIpv6[1] & 0xf0) == 0x0) &&
              (osal_memcmp(&pMldHdr->groupIpv6[2], &ipv6_proxy_query.addr[2], IPV6_ADDR_LEN -2) == 0))
        {
            SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is "
                "invalid Multicast group address !\n", IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
            return ERRONEOUS_PACKET;
        }
    }

    if (pMldHdr->type == MLD_TYPE_MEMBERSHIP_QUERY)
    {
        if (is_ipv6Addr_equel(pMldHdr->groupIpv6, ipv6_proxy_query.addr))
        {
             /* General Query */
            if ((!is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_dip_query1.addr)) &&
                  (!is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_dip_query2.addr)))
            {
                SYS_DBG(LOG_DBG_MLD, "IP Header Error: Dst IP("IPADDRV6_PRINT") of MLD GENERAL-QUERY "
                    "packet is incorrect!\n", IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6));
                return ERRONEOUS_PACKET;
            }
        }
        else
        {
             /*Error General Query */
            if ((is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_dip_query1.addr)) ||
                 (is_ipv6Addr_equel(pIpv6Hdr->dipv6, ipv6_dip_query2.addr)))
            {
                SYS_DBG(LOG_DBG_MLD, "Mld Header Error: group IP("IPADDRV6_PRINT") of MLD GENERAL-QUERY "
                    "packet is incorrect!\n", IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
                return ERRONEOUS_PACKET;
            }

            /* GS-Specific Query */
            if (!is_ipv6Addr_equel(pMldHdr->groupIpv6, pIpv6Hdr->dipv6))
            {
                SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is not same "
                    "with the Dst IPV6("IPADDRV6_PRINT") of MLD GS-Specific QUERY packet!\n",
                    IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6), IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6));
                return ERRONEOUS_PACKET;
            }
        }
    }

    if (pMldHdr->type == MLD_TYPE_MEMBERSHIP_REPORT)
    {
        /* RFC 2710: 8.  Message destinations */
        if  (!is_ipv6Addr_equel(pMldHdr->groupIpv6, pIpv6Hdr->dipv6))
        {
            SYS_DBG(LOG_DBG_MLD, "MLD Header Error: Group Address("IPADDRV6_PRINT") is not same "
                "with the Dst IPV6("IPADDRV6_PRINT") of MLD REPORT packet!\n",
                IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6), IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6));
            return ERRONEOUS_PACKET;
        }
    }
    /* RFC 2710: 8. Message destinations */
    if (pMldHdr->type == MLD_TYPE_MEMBERSHIP_DONE)
    {
        if (!(is_ipv6Addr_equel (pIpv6Hdr->dipv6, ipv6_dip_leave2.addr)))
        {
            SYS_DBG(LOG_DBG_MLD, "IP Header Error: Dst IP("IPADDRV6_PRINT") of MLD LEAVE packet is incorrect!\n",
                IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6));
            return ERRONEOUS_PACKET;
        }
    }

    return SUPPORTED_IGMP_CONTROL_PACKET;
}

int32 mcast_mld_up_handle_membership_query(
    sys_nic_pkt_t *pBuf, uint16 vid, ipv6hdr_t *pIpv6Hdr, uint16 optionLen)
{
    uint32                  t, qqic = 0, group, lastIntvl = 0, lastMbrIntv = DFLT_MLD_GS_QUERY_RESP_INTV;
    uint32                  aclEntryId = 0;
    uint16                  i, srcNum = 0, sortedIdx, grpVid = 0;
    uint8                   robussVar = 0, query_version = MLD_QUERY_V1;
    uint8                   srcList[512], *pSrcList, isSpecQuery = TRUE;
    sys_ipv6_addr_t         groupIpv6;
    sys_logic_portmask_t    blockPm, txPm, eachPm;
    sys_logic_port_t        port;
    igmp_router_entry_t     *pEntry     = NULL;
    igmp_group_entry_t      *pGroup     = NULL, groupEntry;
    mld_hdr_t               *pMldHdr    = NULL;
    mldv2_qryhdr_t          *pMldv2Hdr  = NULL;
    multicast_ipType_t      ipType      = MULTICAST_TYPE_IPV6;
    mcast_vlan_entry_t      *pMcastVlan = NULL, mcastVlanEntry;
    mcast_groupAddr_t       groupAddr;

    osal_memset(&groupAddr,0, sizeof(mcast_groupAddr_t));
    LOGIC_PORTMASK_CLEAR_ALL(blockPm);
    LOGIC_PORTMASK_SET_PORT(blockPm, pBuf->rx_tag.source_port);

    LOGIC_PORTMASK_CLEAR_ALL(txPm);

	if (MODE_GPON != igmpCtrl.ponMode)
	{
        mcast_vlan_db_get(vid, ipType, &pMcastVlan);

        SYS_ERR_CHK((!pMcastVlan), SYS_ERR_FAILED);

        //received query. than clear the suppre flag.
        mcast_suppreFlag_clear(vid, ipType);
    }



    /*mlv2  query packet length is larger than sizeof(mld_hdr_t)*/
    if (pIpv6Hdr->tot_len - optionLen - sizeof(mld_hdr_t) > 0)
    {
        query_version = MLD_QUERY_V2;
        pMldv2Hdr = (mldv2_qryhdr_t *)((uint8 *)pIpv6Hdr + optionLen + MLD_IPV6_HDR_LEN);
        osal_memcpy(groupIpv6.ipv6_addr, pMldv2Hdr->groupIpv6, IPV6_ADDR_LEN);
    }
    else
    {
        pMldHdr = (mld_hdr_t *)((uint8 *)pIpv6Hdr + optionLen + MLD_IPV6_HDR_LEN);
        osal_memcpy(groupIpv6.ipv6_addr, pMldHdr->groupIpv6, IPV6_ADDR_LEN);
    }

    SYS_DBG(LOG_DBG_MLD,
        "%s()@%d:  A MLD QUERY received on Port: %u. its query version is %u\n",
        __FUNCTION__, __LINE__, pBuf->rx_tag.source_port, query_version);

    group           = IPV6_TO_IPV4(groupIpv6.ipv6_addr);
    groupAddr.ipv4  = group;
    osal_memcpy(groupAddr.ipv6.addr, groupIpv6.ipv6_addr, IPV6_ADDR_LEN);

    if (MODE_GPON == igmpCtrl.ponMode)
    {
        if (0 != group)
		{
			if (SYS_ERR_OK != mcast_igmp_group_vid_by_group_search(ipType, groupAddr, &grpVid, &aclEntryId))
			{
				SYS_DBG(LOG_DBG_IGMP, "%s() %d search FAILED gprVid=%u\n", __FUNCTION__, __LINE__, grpVid);

                return SYS_ERR_FAILED;
			}

			mcast_profile_time_intv_get(INTV_TYPE_SPECIFIC_RSP, aclEntryId, NULL, &lastMbrIntv);

            vid = ((!igmpCtrl.igmpVlanTag_type) ? grpVid : vid);
		}
    }

    if ((MLD_QUERY_V2 == query_version) && (p_mld_stats->oper_version < MLD_VERSION_V2_BASIC))
    {
        sys_logic_portmask_t txPmsk;
        LOGIC_PORTMASK_SET_ALL(txPmsk);
        LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

        SYS_DBG(LOG_DBG_IGMP, "%s() %d txPmsk=%x\n", __FUNCTION__, __LINE__, *(txPmsk.bits));
        mcast_snooping_tx(pBuf, vid, pBuf->length, &txPmsk);
        //rsd_nic_pkt_flood(pBuf, vid, blockPm, SYS_CPU_PORT_PRIORITY_IGMP, TRUE);
        p_mld_stats->g_query_xmit++;

        return SYS_ERR_OK;
    }

    if (MLD_QUERY_V2 != query_version)
    {
        if (0 == osal_memcmp(groupIpv6.ipv6_addr, ipv6_proxy_query.addr, IPV6_ADDR_LEN))     /* general query */
        {
            if (MODE_GPON != igmpCtrl.ponMode)
            {
                t = pMldHdr->responseDelay / 1000 ? pMldHdr->responseDelay / 1000 : pMcastVlan->responseTime;
            }
            else
            {
                t = pMldHdr->responseDelay / 1000 ? pMldHdr->responseDelay / 1000 : DFLT_MLD_QUERY_RESPONSE_INTV;
            }

            p_mld_stats->g_query_rcvd++;
            isSpecQuery = FALSE;

            SYS_DBG(LOG_DBG_MLD,
                "A MLD General QUERY received on Vid = %d, Port: %u from "IPADDRV6_PRINT". (Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6), t);
        }
        else
        {
            /* v2 group specific query */
            if (MODE_GPON != igmpCtrl.ponMode)
            {
                t = pMldHdr->responseDelay /1000 ? pMldHdr->responseDelay / 1000 : pMcastVlan->responseTime;
            }
            else
            {
                t = pMldHdr->responseDelay / 1000 ? pMldHdr->responseDelay / 1000 : DFLT_MLD_GS_QUERY_RESP_INTV;
            }

            if (pMldHdr->responseDelay /1000)
                lastIntvl = pMldHdr->responseDelay /1000;

            p_mld_stats->gs_query_rcvd++;

            SYS_DBG(LOG_DBG_MLD,
                "A MLD Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDRV6_PRINT
                ". (Group:"IPADDRV6_PRINT" Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6), IPADDRV6_PRINT_ARG(groupIpv6.ipv6_addr), t);
        }
    }
    else
    {
        pSrcList = srcList;

        srcNum = (NULL == (&(pMldv2Hdr->numOfSrc)) ? 0 : ntohl(pMldv2Hdr->numOfSrc));
        //t = ((pMldv2Hdr->responseDelay & 0x0F) | 0x10) << ((pMldv2Hdr->responseDelay & 0x70)+3);
        t = pMldv2Hdr->responseDelay / 1000;

        if (t)
          lastIntvl = pMldv2Hdr->responseDelay / 1000;

        if (pMldv2Hdr->qqic & 0x80)
        {
            qqic = ((pMldv2Hdr->qqic & 0x0F) | 0x10) << (((pMldv2Hdr->qqic & 0x70) >> 4) + 3);
        }
        else
        {
            qqic = pMldv2Hdr->qqic;
        }

        robussVar =  pMldv2Hdr->rsq & 0x07;

        /* general query */
        if (0 == osal_memcmp(groupIpv6.ipv6_addr, ipv6_proxy_query.addr, IPV6_ADDR_LEN))
        {
            t = t ? t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->responseTime : DFLT_MLD_QUERY_RESPONSE_INTV);

            p_mld_stats->g_query_rcvd++;
            isSpecQuery = FALSE;

            SYS_DBG(LOG_DBG_MLD,
                "A MLDv2 General QUERY  received on Vid = %d, Port-%d from "IPADDRV6_PRINT". (Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6), t);
        }/*mldv2  group specific query*/
        else if (srcNum == 0)
        {
            t = t ? t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->responseTime : DFLT_MLD_GS_QUERY_RESP_INTV);

            p_mld_stats->gs_query_rcvd++;

            SYS_DBG(LOG_DBG_MLD,
                "A MLDv2 Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDRV6_PRINT
                ". (Group:"IPADDRV6_PRINT" Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6), IPADDRV6_PRINT_ARG(groupIpv6.ipv6_addr), t);
        }/*mld v2 group and src specific query*/
        else
        {
            osal_memset(srcList,0,sizeof(srcList));
            for (i = 0; i < srcNum; i++)
            {
                pSrcList += sprintf((char *)pSrcList, IPADDRV6_PRINT, IPADDRV6_PRINT_ARG(pMldv2Hdr->srcList[i].addr));
                pSrcList += sprintf((char *)pSrcList, "  ");

                // record 10 source ipv6 address.
                if (i == 10)
                    break;
            }

            t = t ? t : (MODE_GPON != igmpCtrl.ponMode ? pMcastVlan->grpSpecificQueryIntv : DFLT_MLD_GS_QUERY_RESP_INTV);

            p_mld_stats->gss_query_rcvd++;
            SYS_DBG(LOG_DBG_MLD,
                "A MLD Group-Specific QUERY received on Vid = %d, Port-%d from "IPADDRV6_PRINT
                ". (Group:"IPADDRV6_PRINT" Source List : %s, Interval:%d s)\n",
                vid, pBuf->rx_tag.source_port, IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6), IPADDRV6_PRINT_ARG(groupIpv6.ipv6_addr), srcList, t);
        }
    }

    if (osal_memcmp(pIpv6Hdr->sipv6, ipv6_proxy_query.addr, IPV6_ADDR_LEN))
    {
        /* TBD for ONU. Add Source Port to query port, if the sip is not zero
        SYS_ERR_CHK((SYS_ERR_OK != mcast_routerPort_add_wrapper(vid, ipType, blockPm)), SYS_ERR_FAILED);
        */
    }

    osal_memset(&mcastVlanEntry,0, sizeof(mcastVlanEntry));
    mcastVlanEntry.vid = vid;
    mcastVlanEntry.ipType = ipType;
    SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlanEntry)), SYS_ERR_FAILED);

    if (isSpecQuery)
    {
        mcastVlanEntry.operGsqueryIntv = lastIntvl ? lastIntvl : mcastVlanEntry.grpSpecificQueryIntv;

        if (MLD_QUERY_V2 == query_version)
        {
            mcastVlanEntry.operLastMmbrQueryCnt = robussVar ? robussVar :mcastVlanEntry.lastMmbrQueryCnt;
        }
        else
        {
             //update operGsquery interval
            mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.operRobNessVar;
            mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar;
        }
    }
    else
    {
        mcastVlanEntry.operRespTime = t;
        if (MLD_QUERY_V2 == query_version)
        {
            mcastVlanEntry.operQueryIntv = qqic ? qqic : mcastVlanEntry.queryIntv;
            mcastVlanEntry.operRobNessVar = robussVar ? robussVar : mcastVlanEntry.robustnessVar;
        }
        else
        {
            mcastVlanEntry.operQueryIntv = mcastVlanEntry.queryIntv * 1;
            mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar * 1;
        }
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_set(&mcastVlanEntry)), SYS_ERR_FAILED);

    if (isSpecQuery)
    {
        SYS_ERR_CHK((SYS_ERR_OK != mcast_group_portTime_update(ipType, vid, &groupAddr, srcNum,
            NULL, lastMbrIntv)), SYS_ERR_FAILED); //TBD: srcList for IPv6
    }

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        mcast_router_db_get(vid, ipType, &pEntry);

        if (pEntry)
        {
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, blockPm)
            {
                if (IS_LOGIC_PORTMASK_PORTSET(pEntry->router_forbid_pmsk, port))
                {
                    SYS_DBG(LOG_DBG_MLD, "port %d is forbidden router port \n", port);
                    return SYS_ERR_OK;
                }
            }
            LOGIC_PORTMASK_OR(txPm, txPm, pEntry->router_pmsk);
        }
    }

    SYS_DBG(LOG_DBG_IGMP, "%s() %d\n", __FUNCTION__, __LINE__);

     /* General query */
    if (0 == osal_memcmp(groupIpv6.ipv6_addr, ipv6_proxy_query.addr, IPV6_ADDR_LEN))
    {
        // TBD: rsd_nic_pkt_flood(pBuf, vid, blockPm, SYS_CPU_PORT_PRIORITY_IGMP, TRUE);
        sys_logic_portmask_t txPmsk;
    	LOGIC_PORTMASK_SET_ALL(txPmsk);
    	LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

    	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, txPmsk)
    	{
    		sys_nic_pkt_t oldBuf;
    		osal_memcpy(&oldBuf, pBuf, sizeof(sys_nic_pkt_t));
    		uint8 *pData = (uint8 *)osal_alloc(sizeof(uint8) * (pBuf->length));
            SYS_ERR_CHK((!pData), SYS_ERR_FAILED);
            osal_memcpy(pData, pBuf->data, pBuf->length);
    		oldBuf.data = pData;
			mcast_query_treatment(&oldBuf, port);
			LOGIC_PORTMASK_CLEAR_ALL(eachPm);
			LOGIC_PORTMASK_SET_PORT(eachPm, port);
			mcast_snooping_tx(&oldBuf, vid, oldBuf.length, &eachPm);
    		osal_free(pData);
    		pData = NULL;
    	}
        p_mld_stats->g_query_xmit++;
    }/* Specific query  */
    else
    {

        if (MLD_VERSION_V2_FULL == p_mld_stats->oper_version)
        {
           // now not support;
            ;
        }
        else
        {
            SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, group,groupIpv6.ipv6_addr, &groupEntry)), SYS_ERR_FAILED);
            /* Only compare dip */
            mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &pGroup);
            if (pGroup)
            {
                LOGIC_PORTMASK_OR(txPm, txPm, pGroup->mbr);
            }
        }

        //delete rx port.
        LOGIC_PORTMASK_CLEAR_PORT(txPm, pBuf->rx_tag.source_port);

        SYS_DBG(LOG_DBG_IGMP,
            "%s() %d  igmp would be start to send txPm=%02x\n", __FUNCTION__, __LINE__, *(txPm.bits));

        if (!IS_LOGIC_PORTMASK_CLEAR(txPm))
        {
            if (MODE_GPON == igmpCtrl.ponMode)
        	{
            	FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, txPm)
            	{
    				sys_nic_pkt_t oldBuf;
    				osal_memcpy(&oldBuf, pBuf, sizeof(sys_nic_pkt_t));
    				uint8 *pData = (uint8 *)osal_alloc(sizeof(uint8) * (oldBuf.length));
                    SYS_ERR_CHK((!pData), SYS_ERR_FAILED);
    				osal_memcpy(pData, pBuf->data, pBuf->length);
				    oldBuf.data = pData;
        			mcast_query_treatment(&oldBuf, port);
					LOGIC_PORTMASK_CLEAR_ALL(eachPm);
					LOGIC_PORTMASK_SET_PORT(eachPm, port);
					mcast_snooping_tx(&oldBuf, vid, oldBuf.length, &eachPm);
    				osal_free(pData);
    				pData = NULL;
            	}
        	}
    		else
    		{
    			SYS_DBG(LOG_DBG_IGMP, "%s() %d	!IS_LOGIC_PORTMASK_CLEAR \n", __FUNCTION__, __LINE__);
            	mcast_snooping_tx(pBuf, vid, pBuf->length, &txPm);
    		}

            if (query_version == MLD_QUERY_V2)
            {
                if (srcNum == 0)
                    p_mld_stats->gs_query_xmit++;
                else
                    p_mld_stats->gss_query_xmit++;
            }
            else
            {
                /* v2 group specific query */
                p_mld_stats->gs_query_xmit++;
            }
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_mld_up_handle_membership_report(
    sys_nic_pkt_t *pBuf, uint16 vid, ipv6hdr_t *pIpv6Hdr, mld_hdr_t *pMldHdr, struct timespec *pRecvTime)
{
    uint8                   port, sendFlag = 0;
    sys_logic_portmask_t    rt_pmsk;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV6;
    mcast_groupAddr_t       groupAddr;
	ipAddr_t                clientIp;

	osal_memset(&rt_pmsk, 0, sizeof(sys_logic_portmask_t));
	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 1;
	osal_memcpy(clientIp.ipAddr.ipv6Addr.addr, pIpv6Hdr->sipv6, sizeof(IPV6_ADDR_LEN));

    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_MLD,
        "A MLD REPORT received on Vid = %d, Port=%d.\n", vid, pBuf->rx_tag.source_port);

    osal_memcpy(groupAddr.ipv6.addr, pMldHdr->groupIpv6, IPV6_ADDR_LEN);
    mcast_groupMbrPort_add_wrapper(ipType, vid, pBuf, 0, &groupAddr, port, &sendFlag, clientIp, pRecvTime);

    /*TBD
    if (ENABLED == p_mld_stats->suppre_enable && !sendFlag)
        return SYS_ERR_OK; */
    LOGIC_PORTMASK_SET_PORT(rt_pmsk, (HAL_GET_PON_PORT() + 1));

    /*this port may be router port ,so need to clear*/
    //mcast_routerPort_get(vid, ipType, &rt_pmsk);

    if (!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
    {
        mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);
        p_mld_stats->report_xmit++;
    }

    return SYS_ERR_OK;
}


int32 mcast_mldv2_up_handle_membership_report(
    sys_nic_pkt_t *pBuf, uint16 vid, ipv6hdr_t *pIpv6Hdr,  mldv2_report_t *pMldRpt, struct timespec *pRecvTime)
{
    uint16 numofRcds = 0;
    uint8 *groupRcds = NULL;
    uint16  i = 0;
    uint8   rcdType = 0xff;
    uint16 port, grpVid = 0;
    uint16 numofsrc;
    uint8 auxlen;
	uint32 aclEntryId = UINT_MAX, imputedGrpBw;
    sys_logic_portmask_t rt_pmsk;
    multicast_ipType_t  ipType = MULTICAST_TYPE_IPV6;
    mcast_groupAddr_t groupAddr;
	ipAddr_t clientIp;
	mcast_preview_info_t	previewInfo;

	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 1;
	osal_memcpy(clientIp.ipAddr.ipv6Addr.addr, pIpv6Hdr->sipv6, sizeof(IPV6_ADDR_LEN));

    numofRcds = ((!pMldRpt) ? 0 : ntohs(pMldRpt->numRcd));
    /* Get trunk logic port if srcPort is belong to the trunk */
    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_MLD, "Num of records: %d \n", numofRcds);

    if (numofRcds != 0)
    {
        groupRcds = (uint8 *)(&(pMldRpt->grec));
    }

    for (i = 0; i < numofRcds; i++)
    {
        // mld filter
        osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));
        osal_memset(&groupAddr, 0, sizeof(groupAddr));
        osal_memcpy(groupAddr.ipv6.addr, ((mldv2_grec_t *)groupRcds)->grec_ipv6Mca, IPV6_ADDR_LEN);
        //SYS_ERR_CONTINUE(mcast_filter_check(ipType, port, ntohl( *((uint32 *)&((mldv2_grec_t *)groupRcds)->grec_ipv6Mca[12]))));
        //TBD: not support ipv6 grpSrcIp
        if (MODE_GPON == igmpCtrl.ponMode)
        {
        	vid =  grpVid;
			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_check(pRecvTime, ipType, port, 0, &groupAddr, &aclEntryId,
                &grpVid, &imputedGrpBw, &previewInfo)), SYS_ERR_FAILED);
        }
		else
		{
        	SYS_ERR_CONTINUE(mcast_filter_check(pRecvTime, ipType, port, 0, &groupAddr, &aclEntryId, &grpVid,
                &imputedGrpBw, &previewInfo));
		}

        rcdType = ((mldv2_grec_t *)groupRcds)->grec_type;

        switch (rcdType)
        {
            case MODE_IS_INCLUDE:
                mcast_mldv2_handle_isIn((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            case MODE_IS_EXCLUDE:
                mcast_mldv2_handle_isEx((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            case CHANGE_TO_INCLUDE_MODE:
                mcast_mldv2_handle_toIn((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            case CHANGE_TO_EXCLUDE_MODE:
                mcast_mldv2_handle_toEx((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            case ALLOW_NEW_SOURCES:
                mcast_mldv2_handle_allow((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            case BLOCK_OLD_SOURCES:
                mcast_mldv2_handle_block((mldv2_grec_t *)groupRcds, vid, port, clientIp, pRecvTime);
                break;
            default:
                break;
        }

        numofsrc = ((mldv2_grec_t *)groupRcds)->grec_nsrcs;
        //this field is allway = 0;
        auxlen = ((mldv2_grec_t *)groupRcds)->grec_auxwords;
        groupRcds = groupRcds + sizeof(mldv2_grec_t)  + numofsrc* IPV6_ADDR_LEN + auxlen*4;
    }

    mcast_routerPort_get(vid, ipType, &rt_pmsk);

    /*this port may be is router port ,so need to clear*/
    LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);

    if(!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
    {
        mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);
        p_mld_stats->report_xmit++;
    }

    return SYS_ERR_OK;
}


int32 mcast_mld_up_handle_membership_leave(
    sys_nic_pkt_t *pBuf, uint16 vid, ipv6hdr_t *pIpv6Hdr, mld_hdr_t *pMldHdr)
{
    uint8                   port;//, isGroupExist = FALSE;
    sys_logic_portmask_t    rt_pmsk;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV6;
    mcast_groupAddr_t       groupAddr;
    mcast_vlan_entry_t      mcastVlanEntry;
    igmp_querier_entry_t    *pQuerier = NULL;
    //uint16                  sortedIdx;
    igmp_group_entry_t      groupEntry;//*groupHead = NULL,
    ipAddr_t                clientIp;

	osal_memset(&rt_pmsk, 0, sizeof(sys_logic_portmask_t));
	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 1;
	osal_memcpy(clientIp.ipAddr.ipv6Addr.addr, pIpv6Hdr->sipv6, sizeof(IPV6_ADDR_LEN));

    port = pBuf->rx_tag.source_port;

    SYS_DBG(LOG_DBG_MLD,
        "MLD Leave("IPADDRV6_PRINT") received on port = %d\n", IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6), port);

    mcast_querier_db_get(vid, ipType, &pQuerier);

    if (NULL != pQuerier)
    {
        if (ENABLED == pQuerier->status)
        {
            osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
            mcastVlanEntry.vid      = vid;
            mcastVlanEntry.ipType   = ipType;
            SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlanEntry)), SYS_ERR_FAILED);
            if (vid == mcastVlanEntry.vid)
            {
                mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.lastMmbrQueryCnt;
                SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_set(&mcastVlanEntry)), SYS_ERR_FAILED);
            }
        }
    }

    osal_memcpy(groupAddr.ipv6.addr, pMldHdr->groupIpv6, IPV6_ADDR_LEN);
    groupAddr.ipv4 = IPV6_TO_IPV4(groupAddr.ipv6.addr);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, groupAddr.ipv4,
        groupAddr.ipv6.addr, &groupEntry)), SYS_ERR_FAILED);
#if 0

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    if (!groupHead)
    {
        SYS_DBG(LOG_DBG_MLD, "Leave Group("IPADDRV6_PRINT")  is not exist\n",
            IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
        return SYS_ERR_OK;
    }
    else
    {
        isGroupExist = TRUE;
        if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
        {
            SYS_DBG(LOG_DBG_MLD, "port %d is not ipv6 Group("IPADDRV6_PRINT") member\n",
                port,IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6));
            return SYS_ERR_OK;
        }
    }
#endif
    igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_mld_stats->lookup_mode].igmp_groupMbrPort_del_wrapper(ipType, vid, pBuf, &groupAddr, port, clientIp);

    //if (isGroupExist)
    {
        //mcast_routerPort_get(vid, ipType,  &rt_pmsk);
        //LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);
        LOGIC_PORTMASK_SET_PORT(rt_pmsk, (HAL_GET_PON_PORT() + 1));
        if(!IS_LOGIC_PORTMASK_CLEAR(rt_pmsk))
        {
            mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);
            p_mld_stats->leave_xmit++;
        }
    }

    return SYS_ERR_OK;
}


int32 mcast_mld_up_handle_mcst_data(sys_nic_pkt_t *pBuf, uint16 vid, ipv6hdr_t *pIpv6Hdr)
{
    sys_logic_port_t           port;
    sys_logic_portmask_t    rt_pmsk;
    igmp_group_entry_t      *pEntry, groupEntry;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV6;
    mcast_groupAddr_t groupAddr;
    mcast_groupAddr_t sipAddr;
	ipAddr_t clientIp;

	osal_memset(&clientIp, 0, sizeof(ipAddr_t));
	clientIp.isIpv6B = 1;
	osal_memcpy(clientIp.ipAddr.ipv6Addr.addr, pIpv6Hdr->sipv6, sizeof(IPV6_ADDR_LEN));

    LOGIC_PORTMASK_CLEAR_ALL(rt_pmsk);

    SYS_DBG(LOG_DBG_MLD,
        "A mcst data packet received.  dipv6:"IPADDRV6_PRINT"  SA:"IPADDRV6_PRINT"\n",
        IPADDRV6_PRINT_ARG(pIpv6Hdr->dipv6), IPADDRV6_PRINT_ARG(pIpv6Hdr->sipv6));

    port = pBuf->rx_tag.source_port;

    groupAddr.ipv4 = IPV6_TO_IPV4(pIpv6Hdr->dipv6);
    sipAddr.ipv4 = IPV6_TO_IPV4(pIpv6Hdr->sipv6);

    osal_memcpy(groupAddr.ipv6.addr, pIpv6Hdr->dipv6, IPV6_ADDR_LEN);

    if (p_mld_stats->oper_version <= MLD_VERSION_V2_BASIC)
        igmp_lookup_mode_func[ipType * IGMP_LOOKUP_MODE_END + p_mld_stats->lookup_mode].igmp_group_mcstData_add_wrapper(ipType, vid, pBuf, &groupAddr, &sipAddr, port, clientIp);

    osal_memset(&groupEntry, 0, sizeof(igmp_group_entry_t));
    groupEntry.vid = vid;
    groupEntry.ipType = ipType;
    groupEntry.dip = groupAddr.ipv4;
    groupEntry.sip = sipAddr.ipv4;
    pEntry = mcast_group_get(&groupEntry);
    if (pEntry)
    {
        if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
        {
            //if mldv2 full, don't only send the pkt to the member port
            if (MLD_VERSION_V2_FULL != p_mld_stats->oper_version)
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
                    LOGIC_PORTMASK_SET_ALL(rt_pmsk);
                else if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                    mcast_routerPort_get(vid, ipType, &rt_pmsk);
                else
                    LOGIC_PORTMASK_CLEAR_ALL(rt_pmsk);
            }
        }
        else        /* If this mcst group has been registered, we should forward it to group mbr */
        {
            mcast_routerPort_get(vid, ipType, &rt_pmsk);
            LOGIC_PORTMASK_OR(rt_pmsk, rt_pmsk, pEntry->mbr);
        }
    }

    LOGIC_PORTMASK_CLEAR_PORT(rt_pmsk, port);
    mcast_snooping_tx(pBuf, vid, pBuf->tail - pBuf->data, &rt_pmsk);

    return SYS_ERR_OK;
}

#if 0
l2g_act_t  mcast_mld_preCheck(sys_nic_pkt_t *pPkt, void *pCookie)
{
    uint16                  tagLen = 0;  /*ctag / otag length*/
    ipv6hdr_t                 *pIpv6Hdr = NULL;
    uint8        nextHeader = NO_NEXT_HEADER;
    uint8        *ptr;
    uint16      extLen;
    mld_hdr_t              *pMldHdr = NULL;
    uint8       isPotocal = FALSE;

    SYS_DBG(LOG_DBG_MLD, "Received MLD packet  DA:"MAC_PRINT"  SA:"MAC_PRINT"\n", MAC_PRINT_ARG(pPkt->data), MAC_PRINT_ARG(pPkt->data + 6));

    //acl forward
    if (DISABLED == p_mld_stats->mldsnp_enable && DISABLED == p_igmp_stats->igmpsnp_enable)
        return L2G_ACT_NONE;

    /* Check whether the packet has vlan tag or not */
    if (TRUE == pPkt->rx_tag.cvid_tagged)
        tagLen = tagLen + 4;

    if (TRUE == pPkt->rx_tag.svid_tagged)
        tagLen = tagLen + 4;


    pIpv6Hdr = (ipv6hdr_t*)(&pPkt->data[6 + 6 + 2 + tagLen]);
    nextHeader = pIpv6Hdr->nextHead;

    if ((osal_memcmp(pIpv6Hdr->dipv6, ipv6_dvmrp_routing.addr, IPV6_ADDR_LEN) == 0) ||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_ospf_all_routing.addr, IPV6_ADDR_LEN) == 0) ||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_ospf_designated_routing.addr, IPV6_ADDR_LEN) == 0)||
        (osal_memcmp(pIpv6Hdr->dipv6, ipv6_pimv2_routing.addr, IPV6_ADDR_LEN) == 0))
    {
        if (ENABLED == p_mld_stats->mldsnp_enable)
            return L2G_ACT_TRAP;
    }


    if (pIpv6Hdr->nextHead == NO_NEXT_HEADER  /* Ipv6 data */)
    {
         return L2G_ACT_TRAP;
    }

    if ((pIpv6Hdr->dipv6[1] == 0x01) || (pIpv6Hdr->dipv6[1] == 0x02) || (pIpv6Hdr->dipv6[1] == 0x05) )
        isPotocal = TRUE;

    ptr = (uint8 *)pIpv6Hdr;
    ptr += MLD_IPV6_HDR_LEN;

    while (NO_NEXT_HEADER != nextHeader)
    {
        switch(nextHeader)
        {
            case HOP_BY_HOP_OPTIONS_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case ROUTING_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case FRAGMENT_HEADER:
                nextHeader = ptr[0];
                ptr += 8;
                break;

            case AUTHENTICATION_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 2) * 4;
                ptr+= extLen;
                break;

            case DESTINATION_OPTION_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case MLD_NH_ICMPV6:
                nextHeader = NO_NEXT_HEADER;
                break;
            case NO_NEXT_HEADER:
                return L2G_ACT_TRAP;
                break;

            default:
                if (!isPotocal)
                    return L2G_ACT_TRAP;
                else
                return L2G_ACT_NONE;
                break;
        }
    }

    pMldHdr = (mld_hdr_t *)ptr;

    if (MLD_TYPE_MEMBERSHIP_QUERY == pMldHdr->type ||
        MLD_TYPE_MEMBERSHIP_REPORT  == pMldHdr->type ||
        MLD_TYPE_MEMBERSHIP_DONE  == pMldHdr->type ||
        MLDV2_TYPE_MEMBERSHIP_REPORT == pMldHdr->type )
    {
        return L2G_ACT_TRAP;
    }

    return L2G_ACT_NONE;
}
#endif

int32 mcast_mld_rx(sys_nic_pkt_t *pPkt, void *pCookie, struct timespec *pRecvTime)
{
    sys_vid_t               vid;
    uint16                  extLen, tagLen      = 0;  /*ctag / otag length*/
    int32                   result              = 0, optionLen = 0;
    ipv6hdr_t               *pIpv6Hdr           = NULL;
    multicast_ipType_t      ipType              = MULTICAST_TYPE_IPV6;
    mld_hdr_t               *pMldHdr            = NULL;
    mldv2_report_t          *pMldv2Rpt;
    uint8                   *ptr, nextHeader    = NO_NEXT_HEADER, dmac[MAC_ADDR_LEN], isData = TRUE;
    sys_logic_portmask_t    blockPm, txPmsk;
    mcast_vlan_entry_t      *pMcastVlan = NULL;

    SYS_DBG(LOG_DBG_MLD,
        "Received MLD packet  DA:"MAC_PRINT"  SA:"MAC_PRINT"\n", MAC_PRINT_ARG(pPkt->data), MAC_PRINT_ARG(pPkt->data + 6));

    /* Check whether the packet has vlan tag or not */
    if (TRUE == pPkt->rx_tag.cvid_tagged)
        tagLen = tagLen + 4;

    if (TRUE == pPkt->rx_tag.svid_tagged)
        tagLen = tagLen + 4;

    vid = ntohs(pPkt->rx_tag.inner_vid);
    /*if (SYS_ERR_OK != rsd_nic_pktVid_get(pPkt, &vid))
    {
        SYS_DBG(LOG_DBG_MLD, "Error: Could not retrieve VID from the frame received !!\n");
    }*/

    if (FALSE == IS_PORT_STP_FWD)
    {
        SYS_DBG(LOG_DBG_MLD,
            "Drop packet, Port-%d is not in STP Forwarding State\n", pPkt->rx_tag.source_port);
        goto mld_up_exit;
    }

    LOGIC_PORTMASK_CLEAR_ALL(blockPm);
    LOGIC_PORTMASK_SET_PORT(blockPm, pPkt->rx_tag.source_port);
    LOGIC_PORTMASK_SET_ALL(txPmsk);
    LOGIC_PORTMASK_ANDNOT(txPmsk, txPmsk, blockPm);

    if (DISABLED == p_mld_stats->mldsnp_enable)
    {
        SYS_DBG(LOG_DBG_MLD,
            "%s() %d Flood MLD packet txPmsk=%x becasue MLD is disabled\n", __FUNCTION__, __LINE__, *(txPmsk.bits));
        mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);
        return SYS_ERR_OK;
    }

    if (ENABLED == igmp_packet_stop)
    {
        SYS_DBG(LOG_DBG_MLD, "Free MLD packet becasue packet stop sign is enabled\n");
        goto mld_up_exit;
    }


    /* Because 28 use rma to trap igmp control packet so that vlan ingress filter couldn't take effect.
        Need to check vlan ingress filter by software. The process as following.
        1. check whether vlan ingress filter is enable on port
        2. If enable, check wheather rx port is a member port of rx vlan
    */
    if (SYS_ERR_OK != mcast_vlan_portIgrFilter_check(vid, pPkt->rx_tag.source_port, ipType))
    {
        goto mld_up_exit;
    }

    p_mld_stats->total_pkt_rcvd++;

    osal_memcpy(dmac, pPkt->data, MAC_ADDR_LEN);
    pIpv6Hdr = (ipv6hdr_t*)(&pPkt->data[6 + 6 + 2 + tagLen]);
    nextHeader = pIpv6Hdr->nextHead;

    ptr = (uint8 *)pIpv6Hdr;
    ptr += MLD_IPV6_HDR_LEN;

    while (NO_NEXT_HEADER != nextHeader)
    {
        switch (nextHeader)
        {
            case HOP_BY_HOP_OPTIONS_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case ROUTING_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case FRAGMENT_HEADER:
                nextHeader = ptr[0];
                ptr += 8;
                break;

            case AUTHENTICATION_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 2) * 4;
                ptr+= extLen;
                break;

            case DESTINATION_OPTION_HEADER:
                nextHeader = ptr[0];
                extLen = (((uint16)ptr[1]) + 1) * 8;
                ptr += extLen;
                break;

            case MLD_NH_ICMPV6:
                nextHeader = NO_NEXT_HEADER;
                isData = FALSE;
                break;

            default:
                isData = FALSE;
                break;
        }

        if (!isData)
            break;
    }

    optionLen = ptr - (uint8 *)pIpv6Hdr - MLD_IPV6_HDR_LEN;

    pMldHdr = (mld_hdr_t *)ptr;

    result = mcast_mld_check_header(dmac, pIpv6Hdr, optionLen);

    SYS_DBG(LOG_DBG_MLD,
        "A report received group from "IPADDRV6_PRINT" ret=%d\n", IPADDRV6_PRINT_ARG(pMldHdr->groupIpv6), result);

    if (ROUTER_ROUTING_PACKET != result)
    {
        if (isData)
            result = MULTICAST_DATA_PACKET;
    }

    switch (result)
    {
        case SUPPORTED_IGMP_CONTROL_PACKET:
            break;

        case UNSUPPORTED_IGMP_CONTROL_PACKET:
            p_mld_stats->valid_pkt_rcvd++;
            p_mld_stats->other_rcvd++;
            SYS_DBG(LOG_DBG_MLD, "Unsupported MLD Pkt: Port = %d, Vid = %d\n", pPkt->rx_tag.source_port, vid);
            //rsd_nic_pkt_flood(pPkt, vid, blockPm, SYS_CPU_PORT_PRIORITY_MLD, TRUE);
            goto mld_up_exit;

        case ERRONEOUS_PACKET:
            SYS_DBG(LOG_DBG_MLD, "ERRONEOUS_PACKET drop.\n");
            p_mld_stats->invalid_pkt_rcvd++;
            goto mld_up_exit;

        case VALID_OTHER_PACKET:
        case MULTICAST_DATA_PACKET:
            if (IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode)
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                {
                    mcast_mld_up_handle_mcst_data(pPkt, vid, pIpv6Hdr);
                }
            }
            else
            {
                SYS_DBG(LOG_DBG_MLD,"Not support DIP+SIP lookup mode!\n");
            }
            goto mld_up_exit;

        case ROUTER_ROUTING_PACKET:
            SYS_DBG(LOG_DBG_MLD, "A Routing Pkt: Port = %d, Vid = %d\n", pPkt->rx_tag.source_port, vid);

            /* Add Source Port to query port */
            mcast_routerPort_add_wrapper(vid, ipType, blockPm);
            //rsd_nic_pkt_flood(pPkt, vid, blockPm, SYS_CPU_PORT_PRIORITY_MLD, TRUE);
            goto mld_up_exit;

        default:
            SYS_DBG(LOG_DBG_MLD, "An unrecognized packet received.\n");
            goto mld_up_exit;
    }

    p_mld_stats->valid_pkt_rcvd++;

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        mcast_vlan_db_get(vid, ipType, &pMcastVlan);

        if (!pMcastVlan)
        {
            SYS_DBG(LOG_DBG_MLD, "Error: Could not get MLD vlan information!!\n");
            goto mld_up_exit;
        }

        /* TODO : consider MLD data that comes from Snooping disabled vlan */

        /* Check if MLD Snooping is enabled in this VLAN or not */
        if (ENABLED != pMcastVlan->enable)
        {
            SYS_DBG(LOG_DBG_MLD,
                "%s() %d Flood MLD packet txPmsk=%x in VLAN=%d becasue MLD is disabled\n", __FUNCTION__, __LINE__, *(txPmsk.bits), vid);

            mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);

            return SYS_ERR_OK;
        }
        mcast_process_delay(ipType);
    }

    switch (pMldHdr->type)
    {
        case MLD_TYPE_MEMBERSHIP_QUERY:
            mcast_mld_up_handle_membership_query(pPkt, vid, pIpv6Hdr, optionLen);
            break;

        case MLD_TYPE_MEMBERSHIP_REPORT:
            p_mld_stats->report_rcvd++;
            mcast_mld_up_handle_membership_report(pPkt, vid, pIpv6Hdr, pMldHdr, pRecvTime);
            break;

        case MLD_TYPE_MEMBERSHIP_DONE:
            if (p_mld_stats->oper_version != MLD_VERSION_V2_FULL)
            {
                p_mld_stats->leave_rcvd++;
                mcast_mld_up_handle_membership_leave(pPkt, vid, pIpv6Hdr, pMldHdr);
            }
            break;

        case MLDV2_TYPE_MEMBERSHIP_REPORT:
            p_mld_stats->report_rcvd++;
            if (p_mld_stats->oper_version >= MLD_VERSION_V2_BASIC)
            {
                pMldv2Rpt = (mldv2_report_t*)((char *)pMldHdr);
                mcast_mldv2_up_handle_membership_report(pPkt, vid, pIpv6Hdr, pMldv2Rpt, pRecvTime);
            }
            else
            {
                 mcast_snooping_tx(pPkt, vid, pPkt->length, &txPmsk);
                 p_mld_stats->report_xmit++;
            }
            break;

        default:
            SYS_DBG(LOG_DBG_MLD, "An unrecognized MLD message received.\n");
            p_mld_stats->other_rcvd++;
            break;

    }


mld_up_exit:
    //rsd_nic_pkt_free(pPkt);
    //if (NULL != pPkt->data)
    //    osal_free(pPkt->data);
    //osal_free(pPkt);

    return SYS_ERR_OK;
}

