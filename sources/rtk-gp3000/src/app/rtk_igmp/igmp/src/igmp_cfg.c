/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Main function of SFU IGMP Application
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IGMP configuration APIs
 */

/*
* Include Files
*/

#include <igmp/inc/proto_igmp.h>

//drv header
#include "pf_vlan.h"
#include "pf_mac.h"

static int32    cSockfd;

static int32 mcast_vlan_enable(sys_vid_t stVid)
{
    sys_enable_t        uiIgspEn = ENABLED, uiQueryEn = ENABLED;
    sys_igmp_querier_t  stQuerier;
    mcast_vlan_entry_t  mcastVlan;

    osal_memset(&mcastVlan, 0, sizeof(mcast_vlan_entry_t));
    mcastVlan.vid       = stVid;
    mcastVlan.ipType    = MULTICAST_TYPE_IPV4;

    if (stVid == mcastVlan.vid && SYS_ERR_OK == mcast_vlan_get(&mcastVlan))
    {
        if (mcastVlan.enable != uiIgspEn)
        {
            mcastVlan.enable = uiIgspEn;
            (void)mcast_vlan_set(&mcastVlan);
        }
    }
    stQuerier.vid       = stVid;
    stQuerier.ipType    = MULTICAST_TYPE_IPV4;
	stQuerier.enabled   = ENABLED; //ctc not enable gs querier
    mcast_igmp_querier_get(&stQuerier);

    if (uiQueryEn != stQuerier.enabled)
    {
        stQuerier.enabled = (uint8)uiQueryEn;
        mcast_igmp_querier_set(&stQuerier);
    }
    return SYS_ERR_OK;
}

static int ext_mcast_vlan_add(int vid)
{
    sys_vlan_event_t    vlanEvent;
    int                 iRet = 0;

    if (DISABLED != p_igmp_stats->igmpsnp_enable)
    {
        iRet = mcast_vlan_db_add(vid, MULTICAST_TYPE_IPV4);

        if (SYS_ERR_OK != iRet)
        {
            return iRet;
        }
    }

    if (DISABLED != p_mld_stats->mldsnp_enable)
    {
        iRet = mcast_vlan_db_add(vid, MULTICAST_TYPE_IPV6);

        if (SYS_ERR_OK != iRet)
        {
           return iRet;
        }
    }

    vlanEvent.vid       = vid;
    vlanEvent.eventType = VLAN_EVENT_CREATE;

    return _mcast_vlan_handler((void *)&vlanEvent);
}

static int32 mcast_addport2mvlan(uint32 uiLPort, uint32 uiMcVid, mcvlan_tag_oper_mode_t enTagMode)
{
    rtk_portmask_t  stLMask, stUntagLMsk;

	RTK_PORTMASK_RESET(stLMask);
	RTK_PORTMASK_RESET(stUntagLMsk);

    SYS_ERR_CHK((RT_ERR_OK != pf_mc_vlan_mem_get(uiMcVid, &stLMask, &stUntagLMsk)), SYS_ERR_FAILED);

    switch (enTagMode)
    {
        case TAG_OPER_MODE_TRANSPARENT:

			if ((RTK_PORTMASK_IS_PORT_SET(stLMask, uiLPort)) && (!RTK_PORTMASK_IS_PORT_SET(stUntagLMsk, uiLPort)))
            {
                return SYS_ERR_OK;
            }
            else
            {
				RTK_PORTMASK_PORT_SET(stLMask, uiLPort);
				RTK_PORTMASK_PORT_CLEAR(stUntagLMsk, uiLPort);
            }
			break;
		case TAG_OPER_MODE_STRIP:

            if ((RTK_PORTMASK_IS_PORT_SET(stLMask, uiLPort)) && (RTK_PORTMASK_IS_PORT_SET(stUntagLMsk, uiLPort)))
			{
				return SYS_ERR_OK;
			}
			else
			{
				RTK_PORTMASK_PORT_SET(stLMask, uiLPort);
				RTK_PORTMASK_PORT_SET(stUntagLMsk, uiLPort);
			}
			break;
		default:
			return SYS_ERR_FAILED;
	}

    SYS_ERR_CHK((RT_ERR_OK != pf_mc_vlan_member_add(uiMcVid, stLMask, stUntagLMsk)), SYS_ERR_FAILED);
	return SYS_ERR_OK;
}

static int _mcast_igmpMcVlanAdd(uint32 portId, uint32 vlanId)
{
	sys_logic_portmask_t blockPm;

    LOGIC_PORTMASK_CLEAR_ALL(blockPm);

    SYS_DBG(LOG_DBG_IGMP, "add port [%d] to mc vlan %d\n", portId,vlanId);

    if ((0 != vlanId) && (!VALID_VLAN_ID(vlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
		SYS_DBG(LOG_DBG_IGMP, "fail to %s() [%d] to vlan [%d]\n", __FUNCTION__, portId, vlanId);
        return SYS_ERR_VLAN_ID;
    }

	if (TRUE == mcast_igspVlanMapExist(portId, vlanId))
	{
		SYS_DBG(LOG_DBG_IGMP, "fail to mcast_igspVlanMapExist [%d] to vlan [%d]\n", portId,vlanId);
    	return SYS_ERR_OK;
  	}

	switch (mcast_igmpTagOperPerPortGet(portId))
	{
	    case TAG_OPER_MODE_TRANSPARENT:
            /* add multicast vlan entry */
            SYS_ERR_CHK((0 != mcast_addport2mvlan(portId, vlanId, TAG_OPER_MODE_TRANSPARENT)), SYS_ERR_FAILED);
            break;
	    case TAG_OPER_MODE_STRIP:
            /* add multicast vlan entry */
            SYS_ERR_CHK((0 != mcast_addport2mvlan(portId, vlanId, TAG_OPER_MODE_STRIP)), SYS_ERR_FAILED);
            break;
	    case TAG_OPER_MODE_TRANSLATION:
            break;
	    default:
            /* do nothing */
            break;
  	}

  	if (0 == igspVlanMap[vlanId].vid)
	{
		igmpMcVlanNum++;
		igspVlanMap[vlanId].vid = vlanId;
		ext_mcast_vlan_add(vlanId);
		mcast_vlan_enable(vlanId);
		LOGIC_PORTMASK_SET_PORT(blockPm, (HAL_GET_PON_PORT() + 1));

        SYS_DBG(LOG_DBG_IGMP, "create mc vlan [%d]\n", vlanId);

        mcast_routerPort_add_wrapper(vlanId, MULTICAST_TYPE_IPV4, blockPm);
	}

    LOGIC_PORTMASK_SET_PORT(igspVlanMap[vlanId].portmask,portId);

    return SYS_ERR_OK;
}

static int mcast_igmpMcVlanAdd(uint32 portId, uint32 vlanId)
{
    int i = 0;

    if (FALSE == IS_VALID_PORT_ID(portId) && 255 != portId)
    {
        return SYS_ERR_FAILED;
    }

    SYS_DBG(LOG_DBG_IGMP, "portId %d vlanId %d",portId,vlanId);

    if (255 == portId)
    {
        for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
        {
            _mcast_igmpMcVlanAdd(i, vlanId);
        }
    }
    else
    {
        _mcast_igmpMcVlanAdd(portId, vlanId);
    }
    return SYS_ERR_OK;
}

static int mcast_igmpMcVlanClear(uint32 portId)
{
    int i = 0, j = 0;
    //int ret = 0;

    if (FALSE == IS_VALID_PORT_ID(portId) && 255 != portId)
    {
        return SYS_ERR_FAILED;
    }

    if (portId==255)
    {
        SYS_DBG(LOG_DBG_IGMP, "clear all of the port");

        for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
        {
            for (j = 1; j < MAX_VLAN_NUMBER; j++)
            {
                if (mcast_igspVlanMapExist(i, j)==TRUE)
                {
                    /* ret = */ _mcast_igmpMcVlanDelete(i, j);
                }
            }
        }
    }
    else
    {
    	SYS_DBG(LOG_DBG_IGMP, "clear  the port %d", portId);

    	for (j = 1; j < MAX_VLAN_NUMBER; j++)
    	{
            if (mcast_igspVlanMapExist(portId, j)==TRUE)
    		{
              /* ret = */ _mcast_igmpMcVlanDelete(portId, j);
            }
        }
    }
    return SYS_ERR_OK;
}

static int mcast_igspTransparentstripModeClear(uint32 portId)
{
    uint32          j, uiPPort;
    rtk_portmask_t  stLMask;

    for (j = 1; j < MAX_VLAN_NUMBER; j++)
    {
        if (mcast_igspVlanMapExist(portId, j))
        {
        	uiPPort = PortLogic2PhyID(portId);
            RTK_PORTMASK_RESET(stLMask);
            RTK_PORTMASK_PORT_SET(stLMask, uiPPort);
            if (RT_ERR_OK != pf_mc_vlan_member_remove(j, stLMask))
            {
            	SYS_DBG(LOG_DBG_IGMP, "failed to rmv the vlan member of port [%d]\n", portId);
            	return SYS_ERR_FAILED;
            }
        }
    }
    return SYS_ERR_OK;
}

static int mcast_igmpTagTranslationTableClear(uint32 portId)
{
    uint32            i, j = 0;
    int               used = 0;
    sys_vlanmask_t    vlanlist;

    VLANMASK_CLEAR_ALL(vlanlist);

    SYS_DBG(LOG_DBG_IGMP, "clear the mc translation vlan table of port[%d]\n", portId);

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED == p_igmp_tag_translation_table[i].enable)
        {
            if (p_igmp_tag_translation_table[i].portId == portId)
            {
                /* match */
                memset(&p_igmp_tag_translation_table[i], 0, sizeof(igmp_vlan_translation_entry_t));
                VLANMASK_SET_VID(vlanlist, p_igmp_tag_translation_table[i].userVid);
            }
        }
    }
    for (j = 0; j < 4096; j++)
    {
        used = 0;
        for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
        {
            if (ENABLED == p_igmp_tag_translation_table[i].enable)
            {
                if (p_igmp_tag_translation_table[i].userVid == j)
                {
                    used=1;
                    break;
                }
            }
        }
        if (0 == used)
        {
            if (VLANMASK_IS_VIDSET(vlanlist, j))
            {
                SYS_DBG(LOG_DBG_IGMP, "delete the empty user vlan [%d]\n", j);
                ext_mcast_vlan_del(j);
            }
        }
    }
    return SYS_ERR_OK;
}

static int mcast_igmpMcTagTranslationClear(uint32 portId)
{
    int             ret = SYS_ERR_OK;
    char            portIdStr[256], mcVidStr[256];
    uint32          i, uiPPort;
    rtk_portmask_t  stLMask;

    memset(portIdStr, 0, sizeof(portIdStr));
    memset(mcVidStr, 0, sizeof(mcVidStr));

    if (TAG_OPER_MODE_TRANSLATION != mcast_igmpTagOperPerPortGet(portId))
    {
        return SYS_ERR_FAILED;
    }
    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED == p_igmp_tag_translation_table[i].enable)
        {
            if (p_igmp_tag_translation_table[i].portId == portId)
            {
            	ret = mcast_del_port_from_vlan(p_igmp_tag_translation_table[i].userVid,portId);

                SYS_ERR_CHK((SYS_ERR_OK != ret), ret);

            	if (RT_ERR_OK != pf_port_sp2c_entry_delete(portId, p_igmp_tag_translation_table[i].mcVid))
            	{
            		SYS_DBG(LOG_DBG_IGMP, "failed to delete s2c of port [%d]\n",portId);
            		return SYS_ERR_FAILED;
            	}

            	if (RT_ERR_OK != pf_port_c2s_entry_delete(
                    portId, p_igmp_tag_translation_table[i].userVid, p_igmp_tag_translation_table[i].mcVid))
            	{
            		SYS_DBG(LOG_DBG_IGMP, "failed to delete c2s entry of port [%d]\n",portId);
            		return SYS_ERR_FAILED;
            	}

            	uiPPort = PortLogic2PhyID(portId);
            	RTK_PORTMASK_RESET(stLMask);
            	RTK_PORTMASK_PORT_SET(stLMask, uiPPort);

            	if (RT_ERR_OK != pf_mc_translation_vlan_member_remove(
                    p_igmp_tag_translation_table[i].userVid, p_igmp_tag_translation_table[i].mcVid, stLMask))
            	{
            		SYS_DBG(LOG_DBG_IGMP, "failed to rmv the vlan member of port [%d]\n",portId);
            		return SYS_ERR_FAILED;
            	}
            }
        }
    }
    ret = mcast_igmpTagTranslationTableClear(portId);

    SYS_ERR_CHK((SYS_ERR_OK != ret), ret);

    return SYS_ERR_OK;
}

static int mcast_igspVlanStripModeClear(uint32 portId, mcvlan_tag_oper_mode_t tagOper)
{
    int ret = SYS_ERR_OK;

    switch(tagOper)
    {
        case TAG_OPER_MODE_TRANSPARENT:
        case TAG_OPER_MODE_STRIP:
            ret = mcast_igspTransparentstripModeClear(portId);
            SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
            break;
        case TAG_OPER_MODE_TRANSLATION:
            ret = mcast_igmpMcTagTranslationClear(portId);
            SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
            break;
        default:
            /* do nothing */
            break;
    }
    return SYS_ERR_OK;
}

static int mcast_igspTransparentModeSetAction(uint32 portId)
{
    uint32 j;

    for (j = 1; j < MAX_VLAN_NUMBER; j++)
    {
        if (mcast_igspVlanMapExist(portId, j))
        {
        	if (0 != mcast_addport2mvlan(portId, j, TAG_OPER_MODE_TRANSPARENT))
        	{
                return SYS_ERR_FAILED;
        	}
        }
    }
    return SYS_ERR_OK;
}

static int mcast_igspStripModeSetAction(uint32 portId)
{
    uint32 j;

    for(j = 1; j < MAX_VLAN_NUMBER; j++)
    {
        if (mcast_igspVlanMapExist(portId, j))
        {
            if (0 != mcast_addport2mvlan(portId, j, TAG_OPER_MODE_STRIP))
            {
                return SYS_ERR_FAILED;
            }
        }
    }
    return SYS_ERR_OK;
}

static int mcast_igspVlantagModeSetAction(uint32 portId, mcvlan_tag_oper_mode_t tagOper)
{
    int ret = 0;

    switch (tagOper)
    {
        case TAG_OPER_MODE_TRANSPARENT:
            ret = mcast_igspTransparentModeSetAction(portId);
            SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
            break;
        case TAG_OPER_MODE_STRIP:
            ret = mcast_igspStripModeSetAction(portId);
            SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
            break;
        case TAG_OPER_MODE_TRANSLATION:
            break;
        default:
            break;
    }
    return SYS_ERR_OK;
}


static int mcast_igmpMcTagOperSet(uint32 portId, mcvlan_tag_oper_mode_t tagOper)
{
	if (tagOper == mcast_igmpTagOperPerPortGet(portId))
	{
    	return SYS_ERR_OK;
  	}

	mcast_igspVlanStripModeClear(portId,mcast_igmpTagOperPerPortGet(portId));
	mcast_igspVlantagModeSetAction(portId,tagOper);
	p_igmp_tag_oper_per_port[portId] = tagOper;

    SYS_DBG(LOG_DBG_IGMP, "set port [%d] mc vlan tag mode %d ok\n", portId, tagOper);

    return SYS_ERR_OK;
}

static int mcast_igmpMcTagstripSet(uint32 portId, uint32 tagOper)
{
	int i = 0;

	if (FALSE == IS_VALID_PORT_ID(portId) && 255 != portId)
  	{
  		return SYS_ERR_FAILED;
  	}

	if (255 == portId)
	{
        for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
        {
            mcast_igmpMcTagOperSet(i, tagOper);
        }
  	}
    else
	{
	    mcast_igmpMcTagOperSet(portId, tagOper);
    }

	return SYS_ERR_OK;
}


static int mcast_igmpTagTranslationTableAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
    uint32 i;

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED == p_igmp_tag_translation_table[i].enable)
        {
            if (p_igmp_tag_translation_table[i].portId == portId
                && p_igmp_tag_translation_table[i].mcVid == mcVid
                && p_igmp_tag_translation_table[i].userVid == userVid)
            {
                return SYS_ERR_OK;
            }
        }
    }

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED != p_igmp_tag_translation_table[i].enable)
        {
            p_igmp_tag_translation_table[i].enable = ENABLED;
            p_igmp_tag_translation_table[i].portId = portId;
            p_igmp_tag_translation_table[i].mcVid = mcVid;
            p_igmp_tag_translation_table[i].userVid = userVid;

            return SYS_ERR_OK;
        }
    }

    return SYS_ERR_FAILED;
}

static int mcast_mcTagTranslationAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
	rtk_portmask_t  stLMask, stUntagLMsk, stSvlanUntagLMsk;
	uint32          uiPPort;

    if ((!IS_VALID_PORT_ID(portId))   ||
        (!VALID_VLAN_ID(mcVid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))) ||
        (!VALID_VLAN_ID(userVid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return SYS_ERR_FAILED;
    }

	RTK_PORTMASK_RESET(stLMask);
	RTK_PORTMASK_RESET(stUntagLMsk);
	RTK_PORTMASK_RESET(stSvlanUntagLMsk);

    if (RT_ERR_OK != pf_mc_vlan_mem_get(mcVid, &stLMask, &stUntagLMsk))
    {
        ;
    }

 	uiPPort = PortLogic2PhyID(portId);

    if (RTK_PORTMASK_IS_PORT_SET(stLMask, uiPPort))
    {
        return SYS_ERR_OK;
    }
    else
    {
		RTK_PORTMASK_PORT_SET(stLMask, uiPPort);
		RTK_PORTMASK_PORT_SET(stSvlanUntagLMsk, uiPPort);
		RTK_PORTMASK_PORT_CLEAR(stUntagLMsk, uiPPort);
    }

    SYS_ERR_CHK((RT_ERR_OK != pf_mc_translation_vlan_member_add(userVid, mcVid, stLMask,
        stUntagLMsk, stSvlanUntagLMsk)), SYS_ERR_FAILED);

	SYS_ERR_CHK((RT_ERR_OK != pf_port_sp2c_entry_add(portId, mcVid, userVid)), SYS_ERR_FAILED);

    SYS_ERR_CHK((RT_ERR_OK != pf_port_c2s_entry_add(portId, userVid, mcVid)), SYS_ERR_FAILED);

    return SYS_ERR_OK;
}

static int mcast_igmpMcTagTranslationSet(uint32 portId, uint32 mcVid, uint32 userVid)
{
    int ret = 0;

    SYS_ERR_CHK((TAG_OPER_MODE_TRANSLATION != mcast_igmpTagOperPerPortGet(portId)), SYS_ERR_FAILED);

    SYS_DBG(LOG_DBG_IGMP,
        "set port[%d] mc translation vlan table %d:%d", portId, mcVid, userVid);

    ret = mcast_mcTagTranslationAdd(portId, mcVid, userVid);
    SYS_ERR_CHK((SYS_ERR_OK != ret), ret);

    ext_mcast_vlan_add(userVid);
    mcast_vlan_enable(userVid);

    ret = mcast_igmpTagTranslationTableAdd(portId, mcVid, userVid);
    SYS_ERR_CHK((SYS_ERR_OK != ret), ret);

    return SYS_ERR_OK;
}

static int mcast_igmpMcTagTranslationAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
	int i = 0, ret = 0;

	if (portId == 255)
	{
	    for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
		{
            mcast_igmpMcTagTranslationSet(i, mcVid, userVid);
    	}
  	}
 	else
	{
    	ret = mcast_igmpMcTagTranslationSet(portId, mcVid, userVid);
        SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
  	}

	return SYS_ERR_OK;
}

static int32 mcast_fastLeaveMode_set(uint32 enable, uint32 portId)
{
	sys_logic_portmask_t mask;

    LOGIC_PORTMASK_CLEAR_ALL(mask);
	LOGIC_PORTMASK_SET_PORT(mask, portId);

    if (ENABLED == enable)
    {
		LOGIC_PORTMASK_OR(igmpCtrl.leaveMode, igmpCtrl.leaveMode, mask);
	}
    else
    {
		LOGIC_PORTMASK_ANDNOT(igmpCtrl.leaveMode, igmpCtrl.leaveMode, mask);
    }
    return SYS_ERR_OK;
}

static int32 mcast_group_sortedArray_copy2shm(char *shm)
{
	char *pTemp;
    int32 idx;

	IGMP_DB_SEM_LOCK();

	pTemp = shm;

	/* Copy group_sortedAry_entry_num to share memory first 32 bit */
	osal_memcpy(pTemp, &group_sortedAry_entry_num, sizeof(group_sortedAry_entry_num));
	pTemp += sizeof(group_sortedAry_entry_num);

    for (idx = 0; idx < group_sortedAry_entry_num; idx++)
    {
        osal_memcpy(pTemp, pp_group_sorted_array[idx], sizeof(igmp_group_entry_t));
        pTemp += sizeof(igmp_group_entry_t);
    }

	IGMP_DB_SEM_UNLOCK();

	return SYS_ERR_OK;
}

static int32 mcast_static_group_entry_set(mcast_msgType_t op, multicast_ipType_t ipType,
	sys_logic_port_t port, igmp_acl_entry_t *pAclEntry)
{
	sys_ipMcastEntry_t  ipMcst_entry;
    sys_l2McastEntry_t  l2McstEntry;
	uint32              ipAddr;
	int32               ret;
	uint8               isClearB;
    sys_logic_port_t    unSnpPortId;
    sys_logic_portmask_t snpPm, unsnpPm;

	SYS_PARAM_CHK((op != MCAST_MSGTYPE_PROFILE_SET && op != MCAST_MSGTYPE_PROFILE_DEL), SYS_ERR_INPUT);
	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);
    SYS_PARAM_CHK(NULL == pAclEntry, SYS_ERR_NULL_POINTER);

	//Get entry is exist and old port mask OR/AND new (~)port mask By op
	//update etnry in asic
	/* Note:
		(1)test chip or normal chip behavior(ex: no src filtering) is determine at drv layer
		(2)static etnry group is different dynamic entry group
	*/
	LOGIC_PORTMASK_CLEAR_ALL(snpPm);
    LOGIC_PORTMASK_COPY(snpPm, igmpCtrl.igmpMldTrap_enable);
    LOGIC_PORTMASK_SET_ALL(unsnpPm);
    LOGIC_PORTMASK_ANDNOT(unsnpPm, unsnpPm, igmpCtrl.igmpMldTrap_enable);

	switch(ipType)
	{
		case MULTICAST_TYPE_IPV4:
			ipAddr =  pAclEntry->dipStart.ipv4;
			SYS_DBG(LOG_DBG_IGMP, "startip=%u, endip=%u\n", pAclEntry->dipStart.ipv4, pAclEntry->dipEnd.ipv4);
			while (ipAddr <= pAclEntry->dipEnd.ipv4)
			{
			    if (IGMP_DMAC_VID_MODE != p_igmp_stats->lookup_mode)
                {
    				osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));
    				ipMcst_entry.vid = pAclEntry->aniVid;
                    ipMcst_entry.care_vid = TRUE;
    				ipMcst_entry.sip = pAclEntry->sip.ipv4;
    				ipMcst_entry.dip = ipAddr;

    				ret = rsd_ipMcastEntry_get(&ipMcst_entry);

    				switch (op)
    				{
    					case MCAST_MSGTYPE_PROFILE_SET:
    						if (SYS_ERR_OK == ret)
    						{
    							//update,
    							if (!IS_LOGIC_PORTMASK_PORTSET(ipMcst_entry.portmask, port))
    							{
    							    LOGIC_PORTMASK_SET_PORT(ipMcst_entry.portmask, port);
                                    //for check filter mode per port
                                    ///LOGIC_PORTMASK_COPY(ipMcst_entry.fmode, ipMcst_entry.portmask);
                                    //set include mode, since sip = 0, don't care fmode
                                    LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.fmode, port);

    							    if (SYS_ERR_OK != rsd_ipMcastEntry_set(&ipMcst_entry))
    							    {
    							        SYS_DBG(LOG_DBG_IGMP, "set static entry port mask failed! vid:%d port:%d\n",
    										ipMcst_entry.vid, port);
    							        return SYS_ERR_FAILED;
    							    }
    							    else
    							    {
    							        SYS_DBG(LOG_DBG_IGMP, "update static entry port (%d) \n", port);
    							    }
    							}
    						}
    						else
    						{
    							//add
    							LOGIC_PORTMASK_SET_PORT(ipMcst_entry.portmask, port);
                                //add un-snooping port mask
                                LOGIC_PORTMASK_OR(ipMcst_entry.portmask, ipMcst_entry.portmask, unsnpPm);
                                //for check filter mode per port
                                ///LOGIC_PORTMASK_COPY(ipMcst_entry.fmode, ipMcst_entry.portmask);
                                //set include mode, since sip = 0, don't care fmode
                                LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.fmode, port);
                                FOR_EACH_PORT_IN_LOGIC_PORTMASK(unSnpPortId, unsnpPm)
                                {
                                     //LOGIC_PORTMASK_ANDNOT(ipMcst_entry.fmode, ipMcst_entry.fmode, unsnpPm);
                                    LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.fmode, unSnpPortId);
                                }


    							if (SYS_ERR_OK != rsd_ipMcastEntry_add(&ipMcst_entry))
    				            {
    				                SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
    				                return SYS_ERR_FAILED;
    				            }
    						}
    						break;
    					case MCAST_MSGTYPE_PROFILE_DEL:
    						if (SYS_ERR_OK == ret)
    						{
    							if (IS_LOGIC_PORTMASK_PORTSET(ipMcst_entry.portmask, port))
    							{
    								LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.portmask, port);

                                    // clear un-snoop port mask first to get real port mask is clear or not
                                    FOR_EACH_PORT_IN_LOGIC_PORTMASK(unSnpPortId, unsnpPm)
                                    {
                                        LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.portmask, unSnpPortId);
                                    }

    								LOGIC_PORTMASK_IS_CLEAR(ipMcst_entry.portmask, isClearB);
                                    //change to  exclude mode
                                    LOGIC_PORTMASK_SET_PORT(ipMcst_entry.fmode, port);

                                    if (!isClearB)
                                    {
                                        // l2 entry's mbr port is not clear, so should be added into un-snoop port mask
                                        FOR_EACH_PORT_IN_LOGIC_PORTMASK(unSnpPortId, unsnpPm)
                                        {
                                            LOGIC_PORTMASK_SET_PORT(ipMcst_entry.portmask, unSnpPortId);
                                        }
                                        // change to include mode
                                        FOR_EACH_PORT_IN_LOGIC_PORTMASK(unSnpPortId, unsnpPm)
                                        {
                                            LOGIC_PORTMASK_CLEAR_PORT(ipMcst_entry.fmode, unSnpPortId);
                                        }
                                    }

                                    SYS_DBG(LOG_DBG_IGMP, "MCAST_MSGTYPE_PROFILE_DEL fmode=%x  \n", *(ipMcst_entry.fmode.bits));

    								ipMcst_entry.isGrpRefB = ((TRUE == isClearB) ? FALSE : TRUE);


                                    if (isClearB)
                                    {
                                        if (SYS_ERR_OK != rsd_ipMcastEntry_del(&ipMcst_entry))
                                        {
                                            SYS_DBG(LOG_DBG_IGMP, "del asic entry FAILED");
        									return SYS_ERR_FAILED;
                                        }
                                    }
                                    else
                                    {
                                        if (SYS_ERR_OK != rsd_ipMcastEntry_set(&ipMcst_entry))
        								{
        									SYS_DBG(LOG_DBG_IGMP, "del asic entry FAILED");
        									return SYS_ERR_FAILED;
        								}
                                    }
    							}
    						}
    						else
    						{
    							SYS_DBG(LOG_DBG_IGMP, "Cannot find entry from asic");
    							return SYS_ERR_FAILED;
    						}
    						break;
    					default:
    						SYS_DBG(LOG_DBG_IGMP, "Not support operation for static acl rule");
    						return SYS_ERR_FAILED;
    				}
                }
                else
                {
                    osal_memset(&l2McstEntry, 0, sizeof(l2McstEntry));
                    l2McstEntry.vid = pAclEntry->aniVid;
    				l2McstEntry.care_vid = TRUE;
                    l2McstEntry.mac[0] = 0x01;
                    l2McstEntry.mac[1] = 0x00;
                    l2McstEntry.mac[2] = 0x5e;
                    l2McstEntry.mac[3] = (ipAddr >> 16) & 0xff;
                    l2McstEntry.mac[4] = (ipAddr >> 8) & 0xff;
                    l2McstEntry.mac[5] = ipAddr & 0xff;
    				//osal_memcpy(l2McstEntry.mac, pEntry->mac, MAC_ADDR_LEN);

                    ret = rsd_l2McastEntry_get(&l2McstEntry);
    				switch (op)
    				{
    					case MCAST_MSGTYPE_PROFILE_SET:
    						if (SYS_ERR_OK == ret)
    						{
    							//update,
    							if (!IS_LOGIC_PORTMASK_PORTSET(l2McstEntry.portmask, port))
    							{
    							    LOGIC_PORTMASK_SET_PORT(l2McstEntry.portmask, port);

    							    if (SYS_ERR_OK != rsd_l2McastEntry_set(&l2McstEntry))
    							    {
    							        SYS_DBG(LOG_DBG_IGMP, "set static entry port mask failed! vid:%d port:%d\n",
    										ipMcst_entry.vid, port);
    							        return SYS_ERR_FAILED;
    							    }
    							    else
    							    {
    							        SYS_DBG(LOG_DBG_IGMP, "update static entry port (%d) \n", port);
    							    }
    							}
    						}
    						else
    						{
    							//add
    							LOGIC_PORTMASK_SET_PORT(l2McstEntry.portmask, port);

                                //add un-snooping port mask
                                LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, unsnpPm);

    							if (SYS_ERR_OK != rsd_l2McastEntry_add(&l2McstEntry))
    				            {
    				                SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
    				                return SYS_ERR_FAILED;
    				            }
    						}
    						break;
    					case MCAST_MSGTYPE_PROFILE_DEL:
    						if (SYS_ERR_OK == ret)
    						{
    							if (IS_LOGIC_PORTMASK_PORTSET(l2McstEntry.portmask, port))
    							{
    								LOGIC_PORTMASK_CLEAR_PORT(l2McstEntry.portmask, port);
    								LOGIC_PORTMASK_IS_CLEAR(l2McstEntry.portmask, isClearB);

    								if (isClearB && (SYS_ERR_OK != rsd_l2McastEntry_del(&l2McstEntry)))
    								{
    									SYS_DBG(LOG_DBG_IGMP, "del asic entry FAILED");
    									return SYS_ERR_FAILED;
    								}
    							}
    						}
    						else
    						{
    							SYS_DBG(LOG_DBG_IGMP, "Cannot find entry from asic");
    							return SYS_ERR_FAILED;
    						}
    						break;
    					default:
    						SYS_DBG(LOG_DBG_IGMP, "Not support operation for static acl rule");
    						return SYS_ERR_FAILED;
    				}
                }
				SYS_DBG(LOG_DBG_IGMP, "ipAddr=%u, endip=%u\n", ipAddr, pAclEntry->dipEnd.ipv4);
				ipAddr++;
			}
			break;
		case MULTICAST_TYPE_IPV6:
			//TBD: rsd struct for ipv6
			break;
		default:
			SYS_DBG(LOG_DBG_IGMP, "Not suuport ipType");
			return SYS_ERR_FAILED;
	}

	return SYS_ERR_OK;
}

static int32 mcast_igmp_static_entry_hw_update(mcast_msgType_t op, multicast_ipType_t ipType,
    igmp_acl_entry_t *pAclEntry, sys_logic_portmask_t portMask)
{
    int port;
    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, portMask)
    {
        if (SYS_ERR_OK != mcast_static_group_entry_set(op, ipType, port, pAclEntry))
        {
            printf("delete static group entry to asic FAILED \n");
            return SYS_ERR_FAILED;
        }
    }
    return SYS_ERR_OK;
}

static int32 mcast_preview_timer_entry_set(uint32 op, igmp_acl_entry_t *pAclEntry)
{
	igmp_preview_timer_entry_t  *pNew = NULL, *ptr = NULL;
	igmp_preview_timer_entry_t  *ptrTmp = NULL;
	timer_arg_t                 *pArg = NULL;
	uint8                       isNewB = TRUE;
	long                        resetIntv;
	Timer_t                     pTimer = NULL;
	int32                       ret;

	if (pAclEntry)
	{
		if (MCAST_MSGTYPE_ACLENTRY_SET == op)
		{
			LIST_FOREACH(ptr, &previewTimerHead, entries)
			{
				pArg = (timer_arg_t *)(ptr->pEntry->arg);
				if (pArg->id == pAclEntry->id)
				{
					isNewB = FALSE;
					//TBD, if exist preview channel, should be update different value based on preveiw length or preview Reate time
					//pArg->nextPreviewTime.tv_sec = 0 + pAclEntry->previewLen + pAclEntry->previewRepeatTime;//last_start_time + preview_len + preview_repeat_time
					pArg->previewReset = pAclEntry->previewReset;
					if (0 == pAclEntry->previewReset)
					{
						resetIntv = LONG_MAX; //cannot reset preview repeat counter
					}
					else if (1 >= pAclEntry->previewReset && 24 <= pAclEntry->previewReset)
					{
						//TBD case 1: ONU has TOD clock

						//case 2: ONU does not have TOD clock, preview repeat counter is reset every 24 hours(=5184000 secords)
						resetIntv = 5184000;
					}
					else if (255 == pAclEntry->previewReset)
					{
						pArg->watchTimes = 0;
						//TBD OLT control reset
						resetIntv = LONG_MAX;
					}
					else
					{
						//TBD reserve
						resetIntv = LONG_MAX;
					}
			        struct itimerspec new_value;
			        int sec = resetIntv / 1000;
			        int ms = resetIntv % 1000;
			        new_value.it_value.tv_sec = sec;
			        new_value.it_value.tv_nsec = ms * 1000 * 1000;
			        new_value.it_interval.tv_sec = sec;
			        new_value.it_interval.tv_nsec = ms * 1000 * 1000;
			        timerfd_settime(ptr->pEntry->fd, TFD_TIMER_ABSTIME, &new_value, 0);
				}
			}

			if (isNewB)
			{
				pNew = (igmp_preview_timer_entry_t *)osal_alloc(sizeof(igmp_preview_timer_entry_t));
				if (pNew)
				{
					pArg = (timer_arg_t *)osal_alloc(sizeof(timer_arg_t));
                    if (!pArg)
                    {
                        osal_free(pNew);
                        pNew = NULL;
                        SYS_DBG(LOG_DBG_IGMP, "alloc arg failed\n");
                        return SYS_ERR_FAILED;
                    }
					pArg->id = pAclEntry->id;
					pArg->watchTimes = 0;
					//next_preview_time = last_start_time + preview_len + preview_repeat_time
					pArg->nextPreviewTime.tv_sec = 0;
					pArg->nextPreviewTime.tv_nsec = 0;
					pArg->previewReset = pAclEntry->previewReset;

                    if (0 == pAclEntry->previewReset)
					{
						resetIntv = LONG_MAX; //cannot reset preview repeat counter
					}
					else if (1 <= pAclEntry->previewReset && 24 >= pAclEntry->previewReset)
					{
						//TBD case 1: ONU has TOD clock
						#if 0 //just for debug
						resetIntv =  (pAclEntry->previewReset)*1000;
						//case 2: ONU does not have TOD clock, preview repeat counter is reset every 24 hours(=5184000 secords)
						#else
						resetIntv = 5184000;
						#endif
					}
					else
					{
						//TBD reserve
						resetIntv = LONG_MAX;
					}

					SYS_DBG(LOG_DBG_IGMP, "reset interval %ld\n", resetIntv);

                    pTimer = DEFAULT_TIMER(resetIntv, previewTimerCB, (void*)pArg);
                    if (!pTimer)
                    {
                        osal_free(pArg);
                        osal_free(pNew);
                        pArg = NULL;
                        pNew = NULL;
                        SYS_DBG(LOG_DBG_IGMP, "create timer failed\n");
                        return SYS_ERR_FAILED;
                    }
					pNew->pEntry = pTimer;
					pNew->pEntry->arg = (void *)pArg;

                    if (0 != AddTimer(tMgr, pNew->pEntry))
                    {
                        osal_free(pArg);
                        osal_free(pNew);
                        pArg = NULL;
                        pNew = NULL;
                        SYS_DBG(LOG_DBG_IGMP, "add timer failed\n");
                        return SYS_ERR_FAILED;
                    }
					LIST_INSERT_HEAD(&previewTimerHead, pNew, entries);

				}
			}
		}
		else if(MCAST_MSGTYPE_ACLENTRY_DEL == op)
		{
            for (ptr = LIST_FIRST(&previewTimerHead); ptr != NULL; )
            {
				pArg = (timer_arg_t *)(ptr->pEntry->arg);
				if (pArg->id == pAclEntry->id)
				{
					ret = RemoveTimer(tMgr, ptr->pEntry);
                    SYS_ERR_CHK((0 != ret), SYS_ERR_FAILED);
					pTimer = ptr->pEntry;
					DestroyTimer(&pTimer);
                    ptrTmp = ptr;
                    ptr = LIST_NEXT(ptr, entries);
					LIST_REMOVE(ptrTmp, entries);
					osal_free(ptrTmp);
				}
                else
                    ptr = LIST_NEXT(ptr, entries);
			}
		}
	}
	return SYS_ERR_OK;
}


/*
	return-
		0: Not exist in profile
		1: Exist in profile
*/
static int32 mcast_igmp_acl2profile_check(multicast_ipType_t ipType, mcast_group_type_t aclType,
	uint32 aclEntryId, sys_logic_portmask_t *pPortMask)
{
	sys_igmp_profile_t      *pProfile = NULL;
	igmp_acl_entry_list_t   *pAclEntryList = NULL;
	int                     port;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((IGMP_GROUP_ENTRY_STATIC != aclType), SYS_ERR_INPUT);

	LOGIC_PORTMASK_CLEAR_ALL((*pPortMask));
	FOR_EACH_LOGIC_PORT((port))
	{
		if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
			continue;

		pAclEntryList = pProfile->aclEntryList[aclType];

        while (pAclEntryList)
		{
		    if (pAclEntryList->aclEntry->id == aclEntryId)
			{
				LOGIC_PORTMASK_SET_PORT((*pPortMask), port);
			}
			pAclEntryList = pAclEntryList->next;
		}
	}
	return (!IS_LOGIC_PORTMASK_CLEAR((*pPortMask)));
}

static int32 mcast_acl_entry_set(multicast_ipType_t ipType, mcast_group_type_t aclType, igmp_acl_entry_t *aclEntry)
{
	igmp_acl_entry_list_t   *pAclEntryList;
	igmp_acl_entry_t        *pAclEntryTemp;
	sys_logic_portmask_t    portMask;
	int32                   isExistB, ret;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((aclType > IGMP_GROUP_ENTRY_ALL || aclType < IGMP_GROUP_ENTRY_DYNAMIC), SYS_ERR_INPUT);

	IGMP_DB_SEM_LOCK();
	pAclEntryList = globalAclEntryListHead[ipType][aclType];

	/*If entry existed, replace the entry*/
	while (pAclEntryList != NULL)
	{
		isExistB = FALSE;
		if (pAclEntryList->aclEntry->id == aclEntry->id)
		{
			/* For static acl rule exist in profile, update asic entry before update software entry */
 			if (IGMP_GROUP_ENTRY_STATIC == aclType &&
				(TRUE == (isExistB = mcast_igmp_acl2profile_check(ipType, aclType, aclEntry->id, &portMask))) &&
				(SYS_ERR_OK != mcast_igmp_static_entry_hw_update(MCAST_MSGTYPE_PROFILE_DEL,
				ipType, pAclEntryList->aclEntry, portMask)))
 			{
 				printf("mcast_igmp_static_entry_hw_update failed for delete "IPADDR_PRINT"\n",
                    IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipStart.ipv4));
 			}

			osal_memcpy(pAclEntryList->aclEntry, aclEntry, sizeof(igmp_acl_entry_t));

			if (IGMP_GROUP_ENTRY_STATIC == aclType && isExistB &&
				(SYS_ERR_OK != mcast_igmp_static_entry_hw_update(MCAST_MSGTYPE_PROFILE_SET,
				ipType, aclEntry, portMask)))
			{
				printf("mcast_igmp_static_entry_hw_update failed for set \n");
			}

			if (aclEntry->previewLen)
            {
				ret = mcast_preview_timer_entry_set(MCAST_MSGTYPE_ACLENTRY_SET, aclEntry);
			}
            else
            {
				ret = SYS_ERR_OK;
			}
            IGMP_DB_SEM_UNLOCK();
			return ret;
		}
		pAclEntryList = pAclEntryList->next;
	}

	/*entry not existed, create a new acl entry and add to global list*/
	pAclEntryTemp = osal_alloc(sizeof(igmp_acl_entry_t));
    if (!pAclEntryTemp)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
	osal_memcpy(pAclEntryTemp, aclEntry, sizeof(igmp_acl_entry_t));

	pAclEntryList = osal_alloc(sizeof(igmp_acl_entry_list_t));
    if (!pAclEntryList)
    {
        osal_free(pAclEntryTemp);
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
	osal_memset(pAclEntryList, 0, sizeof(igmp_acl_entry_list_t));
	pAclEntryList->aclEntry = pAclEntryTemp;
	pAclEntryList->next = globalAclEntryListHead[ipType][aclType];
	globalAclEntryListHead[ipType][aclType] = pAclEntryList;

	if (pAclEntryTemp->previewLen)
    {
		ret = mcast_preview_timer_entry_set(MCAST_MSGTYPE_ACLENTRY_SET, pAclEntryTemp);
	}
    else
    {
		ret = SYS_ERR_OK;
	}
    IGMP_DB_SEM_UNLOCK();
	return ret;

}

static int32 mcast_acl_entry_del(multicast_ipType_t ipType, mcast_group_type_t aclType, uint32 aclEntryId)
{
	igmp_acl_entry_list_t *pAclEntryList, *pAclEntryListPrev;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((aclType > IGMP_GROUP_ENTRY_ALL || aclType < IGMP_GROUP_ENTRY_DYNAMIC), SYS_ERR_INPUT);

	pAclEntryList       = globalAclEntryListHead[ipType][aclType];
	pAclEntryListPrev   = NULL;

	/*If entry existed, delete the entry*/
	while (pAclEntryList != NULL)
	{
		if (pAclEntryList->aclEntry->id == aclEntryId)
		{
			mcast_preview_timer_entry_set(MCAST_MSGTYPE_ACLENTRY_DEL, pAclEntryList->aclEntry);
			if (pAclEntryListPrev)
			{
				pAclEntryListPrev->next = pAclEntryList->next;
			}
            else
            {
				globalAclEntryListHead[ipType][aclType] = pAclEntryList->next;
			}

			osal_free(pAclEntryList->aclEntry);
			osal_free(pAclEntryList);

			return SYS_ERR_OK;
		}
		pAclEntryListPrev = pAclEntryList;
		pAclEntryList  = pAclEntryList->next;
	}
	printf("Can't find entry\n");
	return SYS_ERR_FAILED;
}

static int32 mcast_port_filter_set(multicast_ipType_t ipType, sys_logic_port_t port)
{
    uint32                  i, aclEntryId = UINT_MAX, imputedGrpBw;
    int32                   tmpCnt;
	uint16                  grpVid = 0;
    igmp_group_entry_t      *pGroup = NULL;
    mcast_groupAddr_t       groupAddr;
	mcast_preview_info_t	previewInfo;

    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));
    IGMP_DB_SEM_LOCK();

    mcast_group_rx_timer_stop();
    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup = mcast_group_get_with_index(i);
        if (!pGroup)
            continue;

        if (IGMP_GROUP_ENTRY_STATIC == pGroup->form)
            continue;

        if (ipType != pGroup->ipType)
            continue;

        if (IS_LOGIC_PORTMASK_PORTSET(pGroup->mbr, port))
        {
            osal_memset(&groupAddr, 0, sizeof(groupAddr));

            if (MULTICAST_TYPE_IPV4 == ipType)
            {
                groupAddr.ipv4 = pGroup->dip;
            }
            else
            {
                osal_memcpy(groupAddr.ipv6.addr, pGroup->dipv6.addr, IPV6_ADDR_LEN);
            }
            if (SYS_ERR_OK == mcast_filter_check(NULL, ipType, port, pGroup->sip, &groupAddr,
                &aclEntryId, &grpVid, &imputedGrpBw, &previewInfo))
            {
                continue;
            }

            tmpCnt = mcast_group_num;
            mcast_groupPort_remove(pGroup, port);

            if (mcast_group_num != tmpCnt)
            {
                i--;
            }
        }
    }

    mcast_group_rx_timer_start();
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_acl_entry_get(
    multicast_ipType_t ipType, mcast_group_type_t aclType, uint32 aclEntryId, igmp_acl_entry_t **pAclEntry)
{
	igmp_acl_entry_list_t *pAclEntryList;

	SYS_PARAM_CHK((ipType >= MULTICAST_TYPE_END || ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((aclType > IGMP_GROUP_ENTRY_ALL || aclType < IGMP_GROUP_ENTRY_DYNAMIC), SYS_ERR_INPUT);

	pAclEntryList = globalAclEntryListHead[ipType][aclType];

	while (pAclEntryList != NULL)
	{
		if (pAclEntryList->aclEntry->id == aclEntryId)
		{
			*pAclEntry = pAclEntryList->aclEntry;
			return SYS_ERR_OK;
		}
		pAclEntryList = pAclEntryList->next;
	}
	/*Can't find*/
	printf("Can't find entry\n");
	*pAclEntry = NULL;

	return SYS_ERR_FAILED;
}

static int32 mcast_igmp_acl2profile_find(
    multicast_ipType_t ipType, mcast_group_type_t aclType, sys_logic_port_t port, uint32 aclEntryId)
{
	sys_igmp_profile_t      *pProfile = NULL;
	igmp_acl_entry_list_t   *pAclEntryList = NULL;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK((aclType < IGMP_GROUP_ENTRY_DYNAMIC || aclType >= IGMP_GROUP_ENTRY_END), SYS_ERR_MCAST_GROUP_TYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

	if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return FALSE;
	}
	pAclEntryList = pProfile->aclEntryList[aclType];

	while (pAclEntryList)
	{
		if (pAclEntryList->aclEntry->id == aclEntryId)
		{
			return TRUE;
		}
		pAclEntryList = pAclEntryList->next;
	}
	return FALSE;
}


static int32 mcast_igmp_acl2profile_add(
    multicast_ipType_t ipType, mcast_group_type_t aclType, sys_logic_port_t port, uint32 aclEntryId)
{
	sys_igmp_profile_t      *pProfile;
	igmp_acl_entry_t        *pAclEntry = NULL;
	igmp_acl_entry_list_t   *pAclEntryList;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK((aclType < IGMP_GROUP_ENTRY_DYNAMIC || aclType >= IGMP_GROUP_ENTRY_END), SYS_ERR_MCAST_GROUP_TYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

	if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return SYS_ERR_FAILED;
	}

	mcast_acl_entry_get(ipType, aclType, aclEntryId, &pAclEntry);

    SYS_ERR_CHK((!pAclEntry), SYS_ERR_FAILED);

	if (!mcast_igmp_acl2profile_find(ipType, aclType, port, aclEntryId))
	{
		/*insert acl entry to the profile*/
		pAclEntryList = osal_alloc(sizeof(igmp_acl_entry_list_t));
        SYS_ERR_CHK((!pAclEntryList), SYS_ERR_FAILED);
		osal_memset(pAclEntryList, 0, sizeof(igmp_acl_entry_list_t));

		pAclEntryList->aclEntry         = pAclEntry;
		pAclEntryList->next             = pProfile->aclEntryList[aclType];
		pProfile->aclEntryList[aclType] = pAclEntryList;
		/* set l2 table or gpon mac filter table for static acl rule */
		if (IGMP_GROUP_ENTRY_STATIC == aclType &&
			(SYS_ERR_OK != mcast_static_group_entry_set(MCAST_MSGTYPE_PROFILE_SET, ipType, port, pAclEntry)))
		{
			printf("set static group entry to asic FAILED \n");
			return SYS_ERR_FAILED;
		}
	}

	/*check HW table*/
	mcast_port_filter_set(ipType, port);
	return SYS_ERR_OK;
}


static int32 mcast_igmp_acl2profile_del(
    multicast_ipType_t ipType, mcast_group_type_t aclType, sys_logic_port_t port, uint32 aclEntryId)
{
	sys_igmp_profile_t *pProfile;
	igmp_acl_entry_list_t *pAclEntryList, *pAclEntryListPrev;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 ||ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK((aclType < IGMP_GROUP_ENTRY_DYNAMIC || aclType >= IGMP_GROUP_ENTRY_END), SYS_ERR_MCAST_GROUP_TYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

	if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return SYS_ERR_FAILED;
	}
	pAclEntryList = pProfile->aclEntryList[aclType];
	pAclEntryListPrev = NULL;

	while (pAclEntryList)
	{
		if (pAclEntryList->aclEntry->id == aclEntryId)
		{
			if (IGMP_GROUP_ENTRY_STATIC == aclType &&
				(SYS_ERR_OK != mcast_static_group_entry_set(MCAST_MSGTYPE_PROFILE_DEL, ipType, port, pAclEntryList->aclEntry)))
			{
				printf("del static group entry to asic FAILED \n");
				return SYS_ERR_FAILED;
			}

			if (pAclEntryListPrev)
			{
				pAclEntryListPrev->next = pAclEntryList->next;
			}
            else
            {
				pProfile->aclEntryList[aclType] = pAclEntryList->next;
			}

            osal_free(pAclEntryList);
			/*check HW table*/
			mcast_port_filter_set(ipType, port);

			return SYS_ERR_OK;
		}
		pAclEntryListPrev = pAclEntryList;
		pAclEntryList = pAclEntryList->next;
	}

	printf("Can't find match acl entry\n");
	return SYS_ERR_FAILED;
}
static int32
mcast_filter_mc_on_pon_entry_print(multicast_ipType_t ipType)
{
	multicast_dip_entry_list_t   *pMcDipEntryList   = NULL;
	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	pMcDipEntryList = globalMcDipEntryListHead[ipType];

	while (pMcDipEntryList)
	{
		printf("\ngpon mac id: %x\n", pMcDipEntryList->dipEntry->id);
		if (ipType == MULTICAST_TYPE_IPV4)
		{
			printf("\tdipStart: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pMcDipEntryList->dipEntry->dipStart.ipv4));
			printf("\tdipEnd: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pMcDipEntryList->dipEntry->dipEnd.ipv4));
		}
		else
		{
			printf("\tdipStart: "IPADDRV6_PRINT"\n", IPADDRV6_PRINT_ARG(pMcDipEntryList->dipEntry->dipStart.ipv6));
			printf("\tdipEnd: "IPADDRV6_PRINT"\n", IPADDRV6_PRINT_ARG(pMcDipEntryList->dipEntry->dipEnd.ipv6));
		}
		pMcDipEntryList  = pMcDipEntryList->next;
	}
    return SYS_ERR_OK;
}


static int32 mcast_acl_entry_print_all(multicast_ipType_t ipType, mcast_group_type_t aclType)
{
	igmp_acl_entry_list_t *pAclEntryList;

	SYS_PARAM_CHK((ipType >= MULTICAST_TYPE_END || ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((aclType > IGMP_GROUP_ENTRY_ALL || aclType < IGMP_GROUP_ENTRY_DYNAMIC), SYS_ERR_INPUT);

	pAclEntryList = globalAclEntryListHead[ipType][aclType];

	while (pAclEntryList != NULL)
	{
		printf("ACL id: %x\n", pAclEntryList->aclEntry->id);
		if (MULTICAST_TYPE_IPV4 == ipType)
		{
			printf("aniVid: %u\n", pAclEntryList->aclEntry->aniVid);
			printf("sip: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->sip.ipv4));
			printf("dipStart: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipStart.ipv4));
			printf("dipEnd: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipEnd.ipv4));
			printf("imputed group Bandwidth: %u\n", pAclEntryList->aclEntry->imputedGrpBW);
			printf("preview length: %u\n", pAclEntryList->aclEntry->previewLen);
			printf("preveiw repeat time: %u\n", pAclEntryList->aclEntry->previewRepeatTime);
			printf("preview repeat count: %u\n", pAclEntryList->aclEntry->previewRepeatCnt);
			printf("preview reset: %u\n", pAclEntryList->aclEntry->previewReset);
			printf("======================\n");
		}
		else if (MULTICAST_TYPE_IPV6 == ipType)
        {
			printf("not support now!!\n");
		}
		pAclEntryList = pAclEntryList->next;
	}

	return SYS_ERR_OK;
}

static int32
mcast_igmp_profile_aclEntry_print(multicast_ipType_t ipType, mcast_group_type_t aclType, sys_logic_port_t port)
{
	sys_igmp_profile_t      *pProfile;
	igmp_acl_entry_list_t   *pAclEntryList;
	uint32                  i;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK((aclType < IGMP_GROUP_ENTRY_DYNAMIC || aclType >= IGMP_GROUP_ENTRY_END), SYS_ERR_MCAST_GROUP_TYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

	if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return SYS_ERR_FAILED;
	}
	pAclEntryList = pProfile->aclEntryList[aclType];

	for (i = 0; pAclEntryList != NULL; i++)
	{
		printf("==index %x==\n", i);
		printf("ACL id: %x\n", pAclEntryList->aclEntry->id);
		printf("aniVid: %u\n", pAclEntryList->aclEntry->aniVid);
		printf("sip: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->sip.ipv4));
		if (ipType == MULTICAST_TYPE_IPV4)
		{
			printf("dipStart: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipStart.ipv4));
			printf("dipEnd: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipEnd.ipv4));
		}
		else
		{
			printf("dipStart: "IPADDRV6_PRINT"\n", IPADDRV6_PRINT_ARG(pAclEntryList->aclEntry->dipStart.ipv6));
			printf("dipEnd: "IPADDRV6_PRINT"\n", IPADDRV6_PRINT_ARG(pAclEntryList->aclEntry->dipEnd.ipv6));
		}
		pAclEntryList = pAclEntryList->next;
	}

	return SYS_ERR_OK;
}


static int32 mcast_port_cfg_print(unsigned int port)
{
	mcast_port_info_entry_t *ptr = NULL;
    printf("%s\n", __FUNCTION__);

	if (port >= LOGIC_PORT_START && port < igmpCtrl.sys_max_port_num)
	{
		printf("%s", PRINT_LINE);
		printf("port [%u] cfg\n", port);
		printf("%s", PRINT_LINE);
		printf("max group number        : %u\n", p_igmp_stats->p_port_entry_limit[port]);
		printf("max bandwidth           : %u\n", p_igmp_stats->p_port_info[port].maxBw);
		printf("bandwidth enforcement   : %u\n", p_igmp_stats->p_port_info[port].bwEnforceB);
		printf("igmp immediate leave    : %u\n", mcast_fastLeaveMode_get(port));
		printf("igmp us rate            : %u\n\n", p_igmp_us_rate_limit[port - 1]);
		printf("igmp/mld ctrl packet information:\n");
        printf("Filter: [0x0001: NoFilter]\n");
        printf("        [0x0002: Untag]\n");
        printf("        [0x0004: Care Tag]\n");
        printf("        [0x0008: Priority]\n");
        printf("        [0x0010: VID]\n");
        printf("        [0x0020: TPID]\n");
        printf("        [0x0040: EtherType]\n");
        printf("Action: [0x0001: Transparent]\n");
        printf("        [0x0002: AddVID]\n");
        printf("        [0x0004: AddPriority]\n");
        printf("        [0x0008: AddTPID]\n");
        printf("        [0x0010: CopyInnerVID]\n");
        printf("        [0x0020: CopyInnerPriority]\n");
        printf("        [0x0040: CopyInnerTPID]\n");
        printf("        [0x0080: CopyOuterVID]\n");
        printf("        [0x0100: CopyOuterPriority]\n");
        printf("        [0x0200: CopyOuterTPID]\n");
        printf("        [0x0400: RemoveTag]\n");
        printf("        [0x0800: ModifyVID]\n");
        printf("        [0x1000: ModifyPriority]\n");
        printf("        [0x2000: ModifyTPID]\n");
		IGMP_DB_SEM_LOCK();
		LIST_FOREACH(ptr, &portInfoHead, entries)
		{
			if (ptr->entry.uniPort == port)
			{
                printf("%s", PRINT_LINE);
				printf("mopId               : %#x\n", ptr->entry.mopId);
				printf("    vidUni          : %#x\n", ptr->entry.vidUni);
				printf("    flowId          : %d\n", ptr->entry.usFlowId);

                printf("    [INGRESS]%45s[EGRESS]\n", " ");
				printf("    %4sC_Filter : 0x%02X%40sC_ACT : 0x%02X\n",
                    " ", ptr->entry.inTag2.tagOp.tagFilter, " ", ptr->entry.outTag2.tagOp.tagAct);
				printf("    %4sS_Filter : 0x%02X%40sS_ACT : 0x%02X\n",
                    " ", ptr->entry.inTag1.tagOp.tagFilter, " ", ptr->entry.outTag1.tagOp.tagAct);
                printf("    %16sTPID%6sVAL%5sVID%4sDEI%3sPRI%17sTPID%6sVAL%5sVID%4sDEI%3sPRI\n",
                    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
                printf("    %15s------%3s------%3s-----%3s---%3s---%16s------%3s------%3s-----%3s---%3s---\n",
                    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
                printf("    %4sCTAG     : 0x%04X%3s0x%04X%3s0x%03X%3s0x%X%3s0x%X%16s0x%04X%3s0x%04X%3s0x%03X%3s0x%X%3s0x%X\n",
                    " ", ptr->entry.inTag2.tpid, " ", ptr->entry.inTag2.tci.val, " ", ptr->entry.inTag2.tci.bit.vid,
                    " ", ptr->entry.inTag2.tci.bit.dei, " ", ptr->entry.inTag2.tci.bit.pri, " ",
                    ptr->entry.outTag2.tpid, " ", ptr->entry.outTag2.tci.val, " ", ptr->entry.outTag2.tci.bit.vid,
                    " ", ptr->entry.outTag2.tci.bit.dei, " ", ptr->entry.outTag2.tci.bit.pri);
                printf("    %4sSTAG     : 0x%04X%3s0x%04X%3s0x%03X%3s0x%X%3s0x%X%16s0x%04X%3s0x%04X%3s0x%03X%3s0x%X%3s0x%X\n",
                    " ", ptr->entry.inTag1.tpid, " ", ptr->entry.inTag1.tci.val, " ", ptr->entry.inTag1.tci.bit.vid,
                    " ", ptr->entry.inTag1.tci.bit.dei, " ", ptr->entry.inTag1.tci.bit.pri, " ",
                    ptr->entry.outTag1.tpid, " ", ptr->entry.outTag1.tci.val, " ", ptr->entry.outTag1.tci.bit.vid,
                    " ", ptr->entry.outTag1.tci.bit.dei, " ", ptr->entry.outTag1.tci.bit.pri);
			}
		}
		IGMP_DB_SEM_UNLOCK();
		printf("%s", PRINT_LINE);
	}
	else
	{
		printf("Input ERROR port ID range is start from 1\n");
	}
	return SYS_ERR_OK;
}

static int32 mcast_port_flow_info_add(mcast_port_info_t *pIgmpPortInfo)
{
	mcast_port_info_entry_t *pNew = NULL, *ptr = NULL;
	uint8                   isNewB = TRUE;

	if (pIgmpPortInfo)
	{
		IGMP_DB_SEM_LOCK();
		LIST_FOREACH(ptr, &portInfoHead, entries)
		{
			if (ptr->entry.usFlowId == pIgmpPortInfo->usFlowId && !osal_memcmp(&(ptr->entry), pIgmpPortInfo,
                sizeof(mcast_port_info_t) - sizeof(igmp_tag_t) * 2 - sizeof(int)))
			{
				isNewB = FALSE;
                //update
                osal_memcpy(&(ptr->entry), pIgmpPortInfo, sizeof(mcast_port_info_t));
				break;
			}
		}

		if (isNewB)
		{
			pNew = (mcast_port_info_entry_t *)osal_alloc(sizeof(mcast_port_info_entry_t));
			if(pNew)
			{
				osal_memcpy(&(pNew->entry), pIgmpPortInfo, sizeof(mcast_port_info_t));
				LIST_INSERT_HEAD(&portInfoHead, pNew, entries);
			}
		}
		IGMP_DB_SEM_UNLOCK();
	}
	return SYS_ERR_OK;
}


static int32 mcast_port_flow_info_del(mcast_port_info_t *pIgmpPortInfo)
{
	mcast_port_info_entry_t *ptr = NULL;

	mcast_port_info_entry_t *ptr_tmp = NULL;

	IGMP_DB_SEM_LOCK();

    for (ptr = LIST_FIRST(&portInfoHead); ptr != NULL; )
    {
		if (!osal_memcmp(&(ptr->entry), pIgmpPortInfo, sizeof(mcast_port_info_t)))
		{
		    ptr_tmp = ptr;
            ptr = LIST_NEXT(ptr, entries);
			LIST_REMOVE(ptr_tmp, entries);
			osal_free(ptr_tmp);
		}
        else
            ptr = LIST_NEXT(ptr, entries);
	}
	IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

static int mcast_us_igmp_counter_get(uint32 portId, uint32 *pIgmpUsRx, uint32 *pIgmpUsRxDrop)
{
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(portId), SYS_ERR_PORT_ID);
	*pIgmpUsRx = p_mcast_total_rx_counter[portId - 1];
	*pIgmpUsRxDrop = p_mcast_rx_drop_counter[portId - 1];

	return SYS_ERR_OK;
}

static int32 mcast_acl_entry_del_all(multicast_ipType_t ipType, mcast_group_type_t aclType)
{
	igmp_acl_entry_list_t   *pAclEntryList = NULL;
    sys_logic_portmask_t    portMask;

	SYS_PARAM_CHK((ipType >= MULTICAST_TYPE_END || ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK((aclType > IGMP_GROUP_ENTRY_ALL || aclType < IGMP_GROUP_ENTRY_DYNAMIC), SYS_ERR_INPUT);

	pAclEntryList = globalAclEntryListHead[ipType][aclType];

	while (pAclEntryList)
	{
		globalAclEntryListHead[ipType][aclType] = pAclEntryList->next;

		if (IGMP_GROUP_ENTRY_STATIC == aclType &&
            (TRUE == mcast_igmp_acl2profile_check(ipType, aclType, pAclEntryList->aclEntry->id, &portMask)))
        {
            if (SYS_ERR_OK != mcast_igmp_static_entry_hw_update(
                MCAST_MSGTYPE_PROFILE_DEL, ipType, pAclEntryList->aclEntry, portMask))
            {
                printf("mcast_igmp_static_entry_hw_update failed for delete "IPADDR_PRINT"\n",
                    IPADDR_PRINT_ARG(pAclEntryList->aclEntry->dipStart.ipv4));
                return SYS_ERR_FAILED;
            }
		}
		mcast_preview_timer_entry_set(MCAST_MSGTYPE_ACLENTRY_DEL, pAclEntryList->aclEntry);
		osal_free(pAclEntryList->aclEntry);
		osal_free(pAclEntryList);
		pAclEntryList = globalAclEntryListHead[ipType][aclType];
	}
	return SYS_ERR_OK;
}

static int32 mcast_igmp_profile_del_all(multicast_ipType_t ipType, sys_logic_port_t port)
{
	sys_igmp_profile_t      *pProfile;
	igmp_acl_entry_list_t   *pAclEntryList = NULL;
	mcast_group_type_t      i;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

	if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return SYS_ERR_FAILED;
	}

	for (i = IGMP_GROUP_ENTRY_DYNAMIC; i < IGMP_GROUP_ENTRY_ALL; i++)
	{
		pAclEntryList = pProfile->aclEntryList[i];
		while (pAclEntryList)
		{
			pProfile->aclEntryList[i] = pAclEntryList->next;
			osal_free(pAclEntryList);
			pAclEntryList = pProfile->aclEntryList[i];
		}
	}
	return SYS_ERR_OK;
}

static int32 mcast_filter_mc_on_pon_entry_hw_oper(
    op_code_t op,
    multicast_ipType_t ipType,
    multicast_dip_entry_t *pEntry)
{
    #define PON_FILTER_MC_MAX 256
    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        uint32  ipv4, cnt;
        for (ipv4 = pEntry->dipStart.ipv4, cnt = 0; (ipv4 <= pEntry->dipEnd.ipv4 && cnt < PON_FILTER_MC_MAX); ipv4++)
        {
            mac_address_t pMcastMac;

            mcast_ipv4_to_mcast_mac(ipv4, pMcastMac);

            if (SYS_OP_ADD == op)
            {
                if (RT_ERR_OK != pf_gpon_macEntry_add(pMcastMac))
                {
                    printf("%s() %d: "IPADDR_PRINT"  add gpon mac entry ["MAC_PRINT"] failed \n",
                        __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(ipv4), MAC_PRINT_ARG(pMcastMac));
                }
            }
            else
            {
                if (RT_ERR_OK != pf_gpon_macEntry_del(pMcastMac))
                {
                    printf("%s() %d: "IPADDR_PRINT"  del gpon mac entry ["MAC_PRINT"] failed \n",
                        __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(ipv4), MAC_PRINT_ARG(pMcastMac));
                }
            }
            cnt++;
        }
    }
    else
    {
        //
        // the layer 3 ipv6 multicast ip address (128bits - 16bytes) is FF02:0000:0000:0000:0000:0001:ff1E:8329
        // the layer 2 ipv6 mac address includes prefix (16 bits) 33-33 and the last 32 bits of ipv6 ip address FF-1E-83-29
        // 33-33-FF-1E-83-29 <=> FF02:0000:0000:0000:0000:0001:ff1E:8329
        //
        #if 1

        uint8 startLsb[4], endLsb[4];
        uint32 from, to, i, cnt;
        memcpy(startLsb, (pEntry->dipStart.ipv6) + 12, sizeof(uint32));
        memcpy(endLsb, (pEntry->dipEnd.ipv6) + 12, sizeof(uint32));
        from = *((uint32 *)startLsb);
        to = *((uint32 *)endLsb);
        //printf("startLsb=%x, startLsb=%u\n", *((unsigned int *)startLsb), *((unsigned int *)startLsb));
        //printf("ipv6[0]=%x, startLsb[0]=%x\n", pEntry->dipStart.ipv6[0], startLsb[0]);

        for (i = from, cnt = 0; (i <= to && cnt < PON_FILTER_MC_MAX); i++)
        {
            mac_address_t pMcastMac;
            memset(pMcastMac, 0, sizeof(mac_address_t));
            pMcastMac[0] = 0x33;
            pMcastMac[1] = 0x33;
            pMcastMac[2] = (i >> 24) & 0xff;
            pMcastMac[2] = (i >> 16) & 0xff;
            pMcastMac[4] = (i >> 8) & 0xff;
            pMcastMac[5] = (i & 0xff);

            printf("%s() %d: pMcastMac="MAC_PRINT"\n",
                __FUNCTION__, __LINE__, MAC_PRINT_ARG(pMcastMac));

            if (SYS_OP_ADD == op)
            {
                if (RT_ERR_OK != pf_gpon_macEntry_add(pMcastMac))
                {
                    printf("%s() %d: startLsb=%02x:%02x:%02x:%02x (%u)," \
                        "endLsb=%02x:%02x:%02x:%02x (%u)," \
                        "i=%u, add gpon mac entry ["MAC_PRINT"] failed \n",
                        __FUNCTION__, __LINE__,
                        startLsb[0], startLsb[1], startLsb[2], startLsb[3], from,
                        endLsb[0], endLsb[1], endLsb[2], endLsb[3], to,
                        i, MAC_PRINT_ARG(pMcastMac));
                }
            }
            else
            {
                if (RT_ERR_OK != pf_gpon_macEntry_del(pMcastMac))
                {
                    printf("%s() %d: startLsb=%02x:%02x:%02x:%02x (%u)," \
                        "endLsb=%02x:%02x:%02x:%02x (%u)," \
                        "i=%u, del gpon mac entry ["MAC_PRINT"] failed \n",
                        __FUNCTION__, __LINE__,
                        startLsb[0], startLsb[1], startLsb[2], startLsb[3], from,
                        endLsb[0], endLsb[1], endLsb[2], endLsb[3], to,
                        i, MAC_PRINT_ARG(pMcastMac));
                }
            }
        }
        #endif

    }

    return SYS_ERR_OK;
}

static int32 mcast_filter_mc_on_pon_entry_del_all(multicast_ipType_t ipType)
{
	multicast_dip_entry_list_t   *pMcDipEntryList   = NULL;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	pMcDipEntryList = globalMcDipEntryListHead[ipType];

	while (pMcDipEntryList)
	{

        globalMcDipEntryListHead[ipType] = pMcDipEntryList->next;
		mcast_filter_mc_on_pon_entry_hw_oper(SYS_OP_DEL, ipType, pMcDipEntryList->dipEntry);
		osal_free(pMcDipEntryList->dipEntry);
		osal_free(pMcDipEntryList);

		pMcDipEntryList  = globalMcDipEntryListHead[ipType];

	}

    if (RT_ERR_OK != pf_gmac_filter_mode_set(1))
        printf("pf_gmac_filter_mode_set failed\n");

	return SYS_ERR_OK;
}

static int mcast_igmp_rate_limit_set(uint32 portId, uint32 rate)
{

	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(portId), SYS_ERR_PORT_ID);
	//TBD rate limit should be done by HW when 9601B asic meter is precise
	/*APOLLOMP do NOT have HW meter with packet count, so need handle by software*/
	if (MODE_GPON == igmpCtrl.ponMode)
	{
		IGMP_DB_SEM_LOCK();
		p_igmp_us_rate_limit[portId - 1] = rate;

		/*Reset counters of this port*/
		p_rx_counter_current[portId - 1] = 0;
		p_remain_packets[portId - 1] = 0;
		p_rx_counter[portId - 1] = 0;
		osal_memset(&(pp_rx_counter_per_interval[portId - 1][0]), 0x0, sizeof(uint32) * RATE_CHECKTIME_NUM);
		IGMP_DB_SEM_UNLOCK();
	}
	else
	{
		/*TBD*/
		//rsd_igmp_rate_limit_set();
	}

	return SYS_ERR_OK;
}

/* Function Name:
*      mcast_portGroup_limit_set
* Description:
*      Sets igmp port max limit group
* Input:
*      ipType - ipType
*      port - logic port id
*      maxnum - max num of group
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
static int32 mcast_portGroup_limit_set(multicast_ipType_t ipType, sys_logic_port_t port, uint16 maxnum)
{
    uint16                  oldMaxnum;
    uint32                  i, cnt = 0;
    int32                   tmpCnt;
    igmp_group_entry_t      *pGroup;
    sys_logic_portmask_t    delPmsk;

    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

    IGMP_DB_SEM_LOCK();

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        oldMaxnum = p_igmp_stats->p_port_entry_limit[port];
        p_igmp_stats->p_port_entry_limit[port] = maxnum;
    }
    else
    {
        oldMaxnum = p_mld_stats->p_port_entry_limit[port];
        p_mld_stats->p_port_entry_limit[port] = maxnum;
    }

    if (oldMaxnum <= maxnum)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_OK;
    }

    LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
    LOGIC_PORTMASK_SET_PORT(delPmsk, port);

    mcast_group_rx_timer_stop();

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup = mcast_group_get_with_index(i);
        if (!pGroup )
            continue;
        //static group is not conuter
        if (IGMP_GROUP_ENTRY_STATIC == pGroup->form)
            continue;

        if (ipType != pGroup->ipType)
            continue;

        if (IS_LOGIC_PORTMASK_PORTSET(pGroup->mbr, port))
        {
            cnt++;
			if ((maxnum != 0 && MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
			{
				if (cnt <= maxnum)
					continue;

				tmpCnt = mcast_group_num;
				mcast_groupPort_remove(pGroup, port);
				if (mcast_group_num != tmpCnt)
					i--;
			}
        }
    }

    mcast_group_rx_timer_start();
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portStats_reset(sys_logic_port_t port)
{
	ipv4ActiveGrpTblEntry_t *ptr        = NULL;

    ipv4ActiveGrpTblEntry_t *ptr_tmp    = NULL;

	ipv6ActiveGrpTblEntry_t *ptr_v6     = NULL;

	ipv6ActiveGrpTblEntry_t *ptr_v6_tmp = NULL;

	SYS_PARAM_CHK((port < LOGIC_PORT_START || port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

    p_igmp_stats->p_port_info[port].maxBw = 0;
	p_igmp_stats->p_port_info[port].bwEnforceB = 0;
	p_igmp_stats->p_port_info[port].curBw = 0;
	p_igmp_stats->p_port_info[port].bwExcdCount = 0;
	p_igmp_stats->p_port_info[port].sJoinCount = 0;
	p_igmp_stats->p_port_info[port].usJoinCount = 0;
	p_igmp_stats->p_port_info[port].ipv4ActiveGrpCount = 0;
	p_igmp_stats->p_port_info[port].ipv6ActiveGrpCount = 0;

	//remove ipv4 entry
	for (ptr = LIST_FIRST(&p_igmp_stats->p_port_info[port].ipv4ActiveGrpTblHead); ptr != NULL; )
	{
	    ptr_tmp = ptr;
        ptr = LIST_NEXT(ptr, entries);
		LIST_REMOVE(ptr_tmp, entries);
		osal_free(ptr_tmp);
	}
	LIST_INIT(&p_igmp_stats->p_port_info[port].ipv4ActiveGrpTblHead);
    //remove ipv6 entry
    for (ptr_v6 = LIST_FIRST(&p_igmp_stats->p_port_info[port].ipv6ActiveGrpTblHead); ptr_v6 != NULL; )
    {
        ptr_v6_tmp = ptr_v6;
        ptr_v6 = LIST_NEXT(ptr_v6, entries);
        LIST_REMOVE(ptr_v6_tmp, entries);
        osal_free(ptr_v6_tmp);
    }
	LIST_INIT(&p_igmp_stats->p_port_info[port].ipv6ActiveGrpTblHead);

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_port_flow_info_clear()
{
	mcast_port_info_entry_t *ptr = NULL;

	IGMP_DB_SEM_LOCK();
	ptr = LIST_FIRST(&portInfoHead);
	while (NULL != ptr)
	{
        LIST_REMOVE(ptr, entries);
        osal_free(ptr);
		ptr = LIST_FIRST(&portInfoHead);
    }
	LIST_INIT(&portInfoHead);
	IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

static int32 mcast_profile_cfg_clear()
{
    mcast_prof_entry_t *ptr = NULL;

    IGMP_DB_SEM_LOCK();
    ptr = LIST_FIRST(&mcastProfHead);
    while (ptr)
    {
        LIST_REMOVE(ptr, entries);
        osal_free(ptr);
        ptr = LIST_FIRST(&mcastProfHead);
    }
    LIST_INIT(&mcastProfHead);
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int mcast_reset_all_config(void)
{
	uint32              i;
    sys_enable_t        enable;

	/*rate limit conters and fast leave*/
	mcast_igmp_mld_ctrl_init(igmpCtrl.ponMode);

    enable = (MODE_GPON == igmpCtrl.ponMode ? DISABLED :
            (IS_LOGIC_PORTMASK_PORTSET(igmpCtrl.igmpMldTrap_enable, LOGIC_PORT_START) ? ENABLED : DISABLED));

    rsd_mcast_ctrl_and_data_to_cpu_enable_set(enable);

    if (DISABLED == enable)
        LOGIC_PORTMASK_CLEAR_ALL(igmpCtrl.igmpMldTrap_enable);
    else
        LOGIC_PORTMASK_SET_ALL(igmpCtrl.igmpMldTrap_enable);

	//reset default if enable gmac filter multicast function
    if (igmpCtrl.ponMacReplaceL2_enable)
    {
		mac_address_t pMcastMac;
		pMcastMac[0] = 0x01;
		pMcastMac[1] = 0x00;
		pMcastMac[2] = 0x5e;
		pMcastMac[3] = 0x00;
		pMcastMac[4] = 0x00;
		pMcastMac[5] = 0x01;

		if (RT_ERR_OK == pf_gpon_macEntry_get(pMcastMac))
		{
			/* del mac address of GQ in gpon mac entry */
			if (RT_ERR_OK != pf_gpon_macEntry_del(pMcastMac))
			{
				printf("pf_gpon_macEntry_add failed\n");
			}
		}

		if (RT_ERR_OK != pf_gmac_filter_mode_set(1))
            printf("pf_gmac_filter_mode_set failed\n");


	}
	/*vlan*/

	/*acl*/
	mcast_acl_entry_del_all(MULTICAST_TYPE_IPV4, IGMP_GROUP_ENTRY_DYNAMIC);
	mcast_acl_entry_del_all(MULTICAST_TYPE_IPV4, IGMP_GROUP_ENTRY_STATIC);
    mcast_filter_mc_on_pon_entry_del_all(MULTICAST_TYPE_IPV4);

	for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
	{
		/*profile*/
		mcast_igmp_profile_del_all(MULTICAST_TYPE_IPV4, i);

		/*rate limit*/
		mcast_igmp_rate_limit_set(i, 0);

		/*max group num*/
		mcast_portGroup_limit_set(MULTICAST_TYPE_IPV4, i, 0);

		/* stats */
		mcast_portStats_reset(i);
	}

	mcast_port_flow_info_clear();

	mcast_profile_cfg_clear();

	return SYS_ERR_OK;
}

static int32 mcast_show_host_per_port(igmp_group_entry_t *pGroupHead, uint16 portId)
{
	igmp_host_list_t *ptr = NULL;

    printf("    [Host Tracking Table]\n");
    printf("    %24s%27s\n", "CLIENT IP", "MBR TIME");
    printf("    ---------------------------------------%3s----------\n", " ");
	LIST_FOREACH(ptr, &pGroupHead->p_hostListHead[portId], entries)
	{
		if (!ptr->tableEntry.hostIp.isIpv6B)
		{
			/*ipv4*/
			printf("    %4s"IPADDR_PRINT, " ",
			    IPADDR_PRINT_ARG(ptr->tableEntry.hostIp.ipAddr.ipv4Addr));

			printf("%27s%u\n", " ", ptr->tableEntry.mbrTimer);
		}
		else
		{
			/*ipv6*/
			printf("    "IPADDRV6_PRINT,
			    IPADDRV6_PRINT_ARG(ptr->tableEntry.hostIp.ipAddr.ipv6Addr.addr));
			printf("%10s%u\n", " ", ptr->tableEntry.mbrTimer);
		}
	}
	return SYS_ERR_OK;
}

static int32 mcast_group_print_all(void)
{
	igmp_group_entry_t		*pGroup = NULL, *pNextgroup = NULL;
	uint16					i, port = LOGIC_PORT_START;

    printf("\n");
	//IGMP_DB_SEM_LOCK();
	for (i = 0; i < group_sortedAry_entry_num; i++)
	{
		pGroup = pp_group_sorted_array[i];
		while (pGroup)
		{
			pNextgroup = pGroup->next_subgroup;
			if (IGMP_GROUP_ENTRY_STATIC == pGroup->form)
			{
				pGroup = pNextgroup;
				continue;
			}
			printf("NO  <%u>\n", i);
			printf("    sortKey         : %llu\n", pGroup->sortKey);
			printf("    suppreFlag      : %u\n", pGroup->suppreFlag);
            printf("    fmode           : %02x\n", *(pGroup->fmode.bits));
            printf("    lookupFlag      : %u\n", pGroup->lookupTableFlag);
			//printf("    grpTimer        : %u\n", pGroup->groupTimer);
			printf("    %11s%13s%23s%43s%26s\n", "MAC", "VID", "DIP", "SIP", "MBR");
            printf("    -----------------%3s-----%3s---------------------------------------%3s"\
                "---------------------------------------%3s-----\n", " ", " ", " ", " ");

			printf("    "MAC_PRINT, MAC_PRINT_ARG(pGroup->mac));
			printf("%8u", pGroup->vid);
            if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
            {
                printf("%3s"IPADDR_PRINT, " ", IPADDR_PRINT_ARG(pGroup->dip));
                printf("%27s"IPADDR_PRINT, " ", IPADDR_PRINT_ARG(pGroup->sip));
                printf("%27s%#x\n", " ", *(pGroup->mbr.bits));
            }
            else if (MULTICAST_TYPE_IPV6 == pGroup->ipType)
            {
                printf("%3s"IPADDRV6_PRINT, " ", IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));
                printf("%3s"IPADDRV6_PRINT, " ", IPADDRV6_PRINT_ARG(pGroup->sipv6.addr));
                printf("%4s%#x\n", " ", *(pGroup->mbr.bits));
            }

			printf("    %5s%9s%14s%7s%12s\n", "PORT", "TIME", "V3FWDMODE", "ST", "GRPMODE");
            printf("    ------%3s------%3s-----------%3s----%3s---------\n",
                " ", " ", " ", " ");
			FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup->mbr)
			{
				printf("    %2s%u", " ", port);
				printf("%7s%3u", " ", pGroup->p_mbr_timer[port]);
				printf("%10s%u", " ", pGroup->v3.p_mbr_fwd_flag[port]);

				igmp_group_head_entry_t *group_head_entry = NULL;//v3 group entry
				mcast_igmp_group_head_get(pGroup->dip, pGroup->vid, &group_head_entry);
				if (group_head_entry)
				{
					printf("%8s%u", " ", group_head_entry->p_mbr_timer[port]);//src timer
					printf("%8s%02x\n", " ", *(group_head_entry->fmode.bits));
				}
                else
                {
					printf("%8s -", " ");//src timer
					printf("%8s -\n", " ");
                }
                mcast_show_host_per_port(pGroup, port);
			}
			pGroup = pNextgroup;
		}
        printf("\n\n");
	}
	//IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

static int32 mcast_profile_set(mcast_msgType_t type, mcast_prof_t *pMcastProf)
{
	mcast_prof_entry_t *pNew = NULL, *ptr = NULL;

	mcast_prof_entry_t *ptrTmp = NULL;

	uint8 isNewB = TRUE;

	if (pMcastProf)
	{
		IGMP_DB_SEM_LOCK();
		if (MCAST_MSGTYPE_MCASTPROF_DEL != type)
		{
			LIST_FOREACH(ptr, &mcastProfHead, entries)
			{
				if (ptr->entry.mopId == pMcastProf->mopId)
				{
					//exist;  update member
					switch (type)
					{
						case MCAST_MSGTYPE_ROBUSTNESS_SET:
							ptr->entry.robustness = pMcastProf->robustness;
							break;
						case MCAST_MSGTYPE_QUERIERIP_SET:
							ptr->entry.querierIpAddr = pMcastProf->querierIpAddr;
							break;
						case MCAST_MSGTYPE_QUERYINTVAL_SET:
							ptr->entry.queryIntval = pMcastProf->queryIntval;
							break;
						case MCAST_MSGTYPE_QUERYMAXRSPTIME_SET:
							ptr->entry.queryMaxRspTime = pMcastProf->queryMaxRspTime;
							break;
						case MCAST_MSGTYPE_LASTMBRQUERYINTVAL_SET:
							ptr->entry.lastMbrQueryIntval = pMcastProf->lastMbrQueryIntval;
							break;
						case MCAST_MSGTYPE_DSIGMPMCASTTCI_SET:
							osal_memcpy(ptr->entry.dsIgmpTci, pMcastProf->dsIgmpTci, 3);
							break;
						case MCAST_MSGTYPE_UNAUTHJOINBHVR_SET:
							ptr->entry.unAuthJoinRqtBhvr = pMcastProf->unAuthJoinRqtBhvr;
							break;
						default:
							break;
					}
					isNewB = FALSE;
					break;
				}
			}

			if (isNewB)
			{
				pNew = (mcast_prof_entry_t *)osal_alloc(sizeof(mcast_prof_entry_t));
				if (!pNew)
				{
				    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_NULL_POINTER;
				}
                osal_memcpy(&(pNew->entry), pMcastProf, sizeof(mcast_prof_t));
                LIST_INSERT_HEAD(&mcastProfHead, pNew, entries);
			}
		}
		else
		{
            for (ptr = LIST_FIRST(&mcastProfHead); ptr != NULL; )
            {
				if (ptr->entry.mopId == pMcastProf->mopId)
				{
				    ptrTmp = ptr;
                    ptr = LIST_NEXT(ptr, entries);
					LIST_REMOVE(ptrTmp, entries);
					osal_free(ptrTmp);
				}
                else
                    ptr = LIST_NEXT(ptr, entries);
			}
		}
		IGMP_DB_SEM_UNLOCK();
	}
	return SYS_ERR_OK;
}

static int32 mcast_profile_print_all(void)
{
	mcast_prof_entry_t *ptr = NULL;

	IGMP_DB_SEM_LOCK();
	LIST_FOREACH(ptr, &mcastProfHead, entries)
	{
        printf("%s", PRINT_LINE);
		printf("mopId              : %x\n", ptr->entry.mopId);
		printf("robustness         : %u\n", ptr->entry.robustness);
		printf("unAuthJoinRqtBhvr  : %u\n", ptr->entry.unAuthJoinRqtBhvr);
		printf("querierIpAddr      : %x\n", ptr->entry.querierIpAddr);
		printf("queryIntval        : %u\n", ptr->entry.queryIntval);
		printf("queryMaxRspTime    : %u\n", ptr->entry.queryMaxRspTime);
		printf("lastMbrQueryIntval : %u\n", ptr->entry.lastMbrQueryIntval);
		printf("dsIgmpTci          : %02x%02x%02x\n",
            ptr->entry.dsIgmpTci[0],
            ptr->entry.dsIgmpTci[1], ptr->entry.dsIgmpTci[2]);
        printf("%s", PRINT_LINE);
	}
	IGMP_DB_SEM_UNLOCK();

	return SYS_ERR_OK;
}

static int32 mcast_preview_timer_print_all(void)
{
	igmp_preview_timer_entry_t  *ptr = NULL;
	timer_arg_t                 *pArg = NULL;
	//IGMP_DB_SEM_LOCK();
	LIST_FOREACH(ptr, &previewTimerHead, entries)
	{
        printf("%s", PRINT_LINE);
		printf("Timer Fd Id         : %d\n", ptr->pEntry->fd);
		pArg = (timer_arg_t *)(ptr->pEntry->arg);
		printf("Preview Acl Entry Id: %x\n", pArg->id);
		printf("Preview Reset       : %u\n", pArg->previewReset);
		printf("Preview watchTimes  : %lu\n", pArg->watchTimes);
        printf("%s", PRINT_LINE);
		//TBD: pArg->nextPreviewTime;
	}
	//IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

/*
	For allowed preview:
	(first pay and olt set allowed preview entry to onu. After onu join the group, start to count down the mbr time.
	When mbr time become to zero, onu should be delete allowed preview entry from mib. In order to avoid, client would
	join this channel again in next time. Note: duration is unlimited, means pay one time and watch channel forever )
	Current status:
	if joined group is allowed preview channel and the port has been join this group, no update mbr timer for this port
	if leaved group is allowed preview channel, delete SW/HW etnry when port is non-fast leave)
	if group entry is allowed preview channel and its duration is unlimited, group timer check function no count down mbr timer
	if receive specific query, only update mbr time for group form is DYNAMIC type
	TBD:
	1. OLT get time left, igmp should be update the remained mbr timer to OMCI mib
	2. when mbr timer is count down to zero, delete entry from table in OMCI mib.[HIGH PRI]
	3. Handle to non_fast leave behavior when leave group is allowed preview channel and port is enabled fast leave function.
	4. Not support allowed preview group for v3
	5. check report/leave behavior.
 */
static int32 mcast_allowed_preview_entry_set(mcast_msgType_t op, mcast_allowed_preview_entry_t *pAllowPreview)
{
	uint8                               isNewB;
	mcast_allowed_preview_row_entry_t   *pNew = NULL, *ptr = NULL;
	mcast_allowed_preview_row_entry_t   *ptrTmp = NULL;

	SYS_PARAM_CHK((op != MCAST_MSGTYPE_ALLOWED_PREVIEW_SET && op != MCAST_MSGTYPE_ALLOWED_PREVIEW_DEL), SYS_ERR_INPUT);
	SYS_PARAM_CHK((NULL == pAllowPreview), SYS_ERR_NULL_POINTER);

	IGMP_DB_SEM_LOCK();

    if (MCAST_MSGTYPE_ALLOWED_PREVIEW_SET == op)
    {
        isNewB = TRUE;

		LIST_FOREACH(ptr, &allowedPreviewHead, entries)
		{
			if (ptr->entry.id == pAllowPreview->id)
			{
				isNewB = FALSE;
				if (ptr->entry.duration != pAllowPreview->duration)
				{
					//TBD update time left(mbrtime)
				}
				//update
				osal_memcpy(&(ptr->entry), pAllowPreview, sizeof(mcast_allowed_preview_entry_t));
			}
		}

		if (isNewB)
		{
			pNew = (mcast_allowed_preview_row_entry_t *)osal_alloc(sizeof(mcast_allowed_preview_row_entry_t));
            if (!pNew)
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_NULL_POINTER;
            }
			osal_memcpy(&(pNew->entry), pAllowPreview, sizeof(mcast_allowed_preview_entry_t));
			LIST_INSERT_HEAD(&allowedPreviewHead, pNew, entries);
		}
    }
    else if (MCAST_MSGTYPE_ALLOWED_PREVIEW_DEL == op)
    {
        for (ptr = LIST_FIRST(&allowedPreviewHead); ptr != NULL; )
		{
			if (!osal_memcmp(&(ptr->entry), pAllowPreview, sizeof(mcast_allowed_preview_entry_t)))
			{
			    ptrTmp = ptr;
                ptr = LIST_NEXT(ptr, entries);
				LIST_REMOVE(ptrTmp, entries);
				osal_free(ptrTmp);
			}
            else
                ptr = LIST_NEXT(ptr, entries);
		}
    }

	IGMP_DB_SEM_UNLOCK();

	return SYS_ERR_OK;
}

static int32 mcast_allowed_preview_entry_print(unsigned int port)
{
    mcast_allowed_preview_row_entry_t *ptr = NULL;

    if (port >= LOGIC_PORT_START && port < igmpCtrl.sys_max_port_num)
    {
        printf("%s", PRINT_LINE);
        printf("port [%u] preview cfg:\n", port);
        IGMP_DB_SEM_LOCK();
        LIST_FOREACH(ptr, &allowedPreviewHead, entries)
        {
            if ((ptr->entry.id >> 16) == port)
            {
                printf("Index     : %u\n", (ptr->entry.id & 0x3FF));
                printf("Src IP    : "IPADDRV6_PRINT"\n",
                    IPADDRV6_PRINT_ARG(ptr->entry.srcIpAddr));
                printf("Dst IP    : "IPADDRV6_PRINT"\n",
                    IPADDRV6_PRINT_ARG(ptr->entry.dstIpAddr));
                printf("ANI VID   : %u\n", ptr->entry.aniVid);
                printf("UNI_VID   : %u\n", ptr->entry.uniVid);
                printf("Duration  : %u\n", ptr->entry.duration);
                printf("time_left : %u\n\n", ptr->entry.timeLeft);
            }
        }
        IGMP_DB_SEM_UNLOCK();
        printf("%s", PRINT_LINE);
    }
    else
    {
        printf("Input ERROR port ID range is start from 1\n");
    }
    return SYS_ERR_OK;
}

static int32 mcast_portMaxBw_set(sys_logic_port_t port, uint32 maxBw)
{
	SYS_PARAM_CHK((port < LOGIC_PORT_START || port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

	p_igmp_stats->p_port_info[port].maxBw = maxBw;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portBwEnforce_set(sys_logic_port_t port, uint32 bwEnforceB)
{

	SYS_PARAM_CHK((port < LOGIC_PORT_START || port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

	p_igmp_stats->p_port_info[port].bwEnforceB = bwEnforceB;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portCurrBw_get(mcast_portStat_t *pPortStat)
{
    SYS_PARAM_CHK((pPortStat->port < LOGIC_PORT_START || pPortStat->port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

    pPortStat->currBw = p_igmp_stats->p_port_info[pPortStat->port].curBw;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portJoinCnt_get(mcast_portStat_t *pPortStat)
{
	SYS_PARAM_CHK((pPortStat->port < LOGIC_PORT_START || pPortStat->port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

	pPortStat->joinCount = p_igmp_stats->p_port_info[pPortStat->port].sJoinCount;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portBwExcdCnt_get(mcast_portStat_t *pPortStat)
{
    SYS_PARAM_CHK((pPortStat->port < LOGIC_PORT_START || pPortStat->port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

    pPortStat->bwExcdCount = p_igmp_stats->p_port_info[pPortStat->port].bwExcdCount;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_active_group_copy2shm(mcast_msgType_t type, uint32 port, void *p)
{
    ipv4ActiveGrpTblEntry_t *ptr = NULL;
    SYS_PARAM_CHK((port < LOGIC_PORT_START || port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

	switch (type)
	{
		case MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET:

			IGMP_DB_SEM_LOCK();
			LIST_FOREACH(ptr, &(p_igmp_stats->p_port_info[port].ipv4ActiveGrpTblHead), entries)
			{
				osal_memcpy(p, &(ptr->tableEntry), sizeof(ipv4ActiveGroup_t));
				p += sizeof(ipv4ActiveGroup_t);
			}
			IGMP_DB_SEM_UNLOCK();
			break;
		case MCAST_MSGTYPE_PORT_IPV6_ACTIVE_GRP_COUNT_GET:
			break;
		default:
			SYS_DBG(LOG_DBG_IGMP,"%s(), Not support type = %u\n", __FUNCTION__, type);
	}
	return SYS_ERR_OK;
}

static int32 mcast_shm_create(mcast_msgType_t msgType, void **shm)
{
	int     shmId;
	key_t   shmKey;
	size_t  shmSize;

	switch (msgType)
	{
		case MCAST_MSGTYPE_GROUPENTRY_GET:
			shmKey = SHM_KEY_GROUP_ARRAY;
			shmSize = SHM_SIZE_2048;
		    break;
		case MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET:
			shmKey = SHM_KEY_GROUP_ARRAY + MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET;
			shmSize = SHM_MAX_SIZE;
		    break;
		default:
			printf("not supported\n");
			return SYS_ERR_FAILED;
	}

	if ((shmId = shmget(shmKey, shmSize, IPC_CREAT | 0666)) < 0)
    {
        printf("shmget failed\n");
  		return SYS_ERR_FAILED;
    }

    if ((*shm = shmat(shmId, (void *)0, 0)) == (void *)-1)
    {
        printf("shmat failed\n");
  		return SYS_ERR_FAILED;
    }

	return SYS_ERR_OK;

}

static int32 mcast_portIpv4ActiveGrpCnt_get(mcast_portStat_t *pPortStat)
{
	SYS_PARAM_CHK((pPortStat->port < LOGIC_PORT_START || pPortStat->port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

	pPortStat->ipv4ActiveGrpNum = p_igmp_stats->p_port_info[pPortStat->port].ipv4ActiveGrpCount;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_portIpv6ActiveGrpCnt_get(mcast_portStat_t *pPortStat)
{
	SYS_PARAM_CHK((pPortStat->port < LOGIC_PORT_START || pPortStat->port > igmpCtrl.sys_max_port_num), SYS_ERR_INPUT);

    IGMP_DB_SEM_LOCK();

	pPortStat->ipv6ActiveGrpNum = p_igmp_stats->p_port_info[pPortStat->port].ipv6ActiveGrpCount;

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

static int32 mcast_port_stat_print(unsigned int port)
{
    ipv4ActiveGrpTblEntry_t *ptr = NULL;

    if (port >= LOGIC_PORT_START && port < igmpCtrl.sys_max_port_num)
    {
        printf("%s", PRINT_LINE);
        printf("port [%u] statictics\n", port);
        printf("%s", PRINT_LINE);
        printf("current groups             : %u\n", mcast_group_num);
        printf("current bandwidth          : %u\n", p_igmp_stats->p_port_info[port].curBw);
        printf("join counter               : %u\n", p_igmp_stats->p_port_info[port].sJoinCount);
        printf("bw exceed counter          : %u\n", p_igmp_stats->p_port_info[port].bwExcdCount);
        printf("%s", PRINT_LINE);
        printf("\t[ipv4 active group Table]\n");
        printf("\t%s", PRINT_LINE);
        LIST_FOREACH(ptr, &p_igmp_stats->p_port_info[port].ipv4ActiveGrpTblHead, entries)
        {
            printf("\tvlan id                : %u\n", ptr->tableEntry.vlanId);
            printf("\tsource IP address      : "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(ptr->tableEntry.srcIpAddr));
            printf("\tdestination IP address : "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(ptr->tableEntry.dstIpAddr));
            printf("\tactual bandwidth       : %u\n", ptr->tableEntry.actualBw);
            printf("\tclient IP address      : "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(ptr->tableEntry.clientIpAddr));
            printf("\trecentJoinTime         : %u\n", ptr->tableEntry.recentJoinTime);
            printf("\t%s", PRINT_LINE);
        }
        printf("%s", PRINT_LINE);
    }
    else
    {
        printf("Input ERROR port ID range is start from 1\n");
    }
    return SYS_ERR_OK;
}

static int32 mcast_ctrl_pkt_trap_set(mcast_igmpTrapAct_t *p)
{
    sys_logic_portmask_t tmp;

    if (ENABLED == p->enable)
    {
        if (!IS_LOGIC_PORTMASK_PORTSET(igmpCtrl.igmpMldTrap_enable, p->port))
        {
            IGMP_DB_SEM_LOCK();
            LOGIC_PORTMASK_SET_PORT(igmpCtrl.igmpMldTrap_enable, p->port);
            IGMP_DB_SEM_UNLOCK();

            rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(p->enable, p->port);
        }
    }
    else
    {
        LOGIC_PORTMASK_COPY(tmp, igmpCtrl.igmpMldTrap_enable);
        LOGIC_PORTMASK_CLEAR_PORT(tmp, p->port);
        if (!IS_LOGIC_PORTMASK_EQUAL(igmpCtrl.igmpMldTrap_enable, tmp))
        {
            IGMP_DB_SEM_UNLOCK();
            LOGIC_PORTMASK_CLEAR_PORT(igmpCtrl.igmpMldTrap_enable, p->port);
            IGMP_DB_SEM_UNLOCK();
            rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(p->enable, p->port);
        }

    }
    return SYS_ERR_OK;
}


static int32 mcast_debug_info_print()
{
    uint16                  i, j;
    igmp_group_entry_t      *pGroup     = NULL;
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    igmp_group_head_entry_t *pGrpHead   = NULL;

    //printf("igmp_hw_entry_num:%u\t", igmp_hw_entry_num);
    printf("%s", PRINT_LINE);
    printf("group_sortedAry_entry_num:\t%d\n", group_sortedAry_entry_num);
    printf("mcast_group_num:\t\t%u\n", mcast_group_num);

    for (i = 0; i < group_sortedAry_entry_num; i++)
    {
        pGroup = pp_group_sorted_array[i];

        if (!pGroup)
            continue;

        if (IGMP_GROUP_ENTRY_STATIC == pGroup->form || IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED == pGroup->form)
            continue;

        mcast_aggregate_db_get(pGroup, &pAggrEntry);

        if (!pAggrEntry)
        {
            printf("warring, if group entry exist, then aggr. entry should be exist \n");
            continue;
        }
        else
        {
            printf("%s", PRINT_LINE);
            printf("sortKey:\t%llu\n", pAggrEntry->sortKey);
            // different group ip mapping to the same mac from differnt port
            printf("group reference number:\t%u\n", pAggrEntry->group_ref_cnt);
            for (j = LOGIC_PORT_START; j < igmpCtrl.sys_max_port_num - 1; j++)
            {
                // different group ip mapping to the same mac from the same port
                printf("port reference number:\t%u\n", pAggrEntry->p_port_ref_cnt[j]);
            }
            printf("%s", PRINT_LINE);
        }
    }

    printf("%s", PRINT_LINE);
    printf("Group Head info:\t\t\n");


    for(i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        pGrpHead = &(p_group_head_db[i]);

        if(0 == pGrpHead->dip)
            continue;

        printf("\tdip: "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pGrpHead->dip));

    }


    return SYS_ERR_OK;
}


static int32 mcast_filter_mc_on_pon_entry_set(
    multicast_ipType_t ipType, multicast_dip_entry_t *pDipEntry)
{
	multicast_dip_entry_list_t   *pMcDipEntryList   = NULL;
	multicast_dip_entry_t        *pMcDipEntryTemp   = NULL;
	int32                        ret                = SYS_ERR_OK;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	IGMP_DB_SEM_LOCK();
	pMcDipEntryList = globalMcDipEntryListHead[ipType];

	/*If entry existed, replace the entry*/
	while (pMcDipEntryList != NULL)
	{
		if (pMcDipEntryList->dipEntry->id == pDipEntry->id)
		{
			//
			// remove old
			//
			mcast_filter_mc_on_pon_entry_hw_oper(SYS_OP_DEL, ipType, pMcDipEntryList->dipEntry);

            //
            // replace old to new
            //
            mcast_filter_mc_on_pon_entry_hw_oper(SYS_OP_ADD, ipType, pDipEntry);

			osal_memcpy(pMcDipEntryList->dipEntry, pDipEntry, sizeof(multicast_dip_entry_t));


            IGMP_DB_SEM_UNLOCK();
			return ret;
		}
		pMcDipEntryList = pMcDipEntryList->next;
	}

	/*entry not existed, create a new acl entry and add to global list*/

    //
    // configure hardware rule for new one
    //
    mcast_filter_mc_on_pon_entry_hw_oper(SYS_OP_ADD, ipType, pDipEntry);

    //
    // maintain software database
    //
    pMcDipEntryTemp = osal_alloc(sizeof(multicast_dip_entry_t));
    if (!pMcDipEntryTemp)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
	osal_memcpy(pMcDipEntryTemp, pDipEntry, sizeof(multicast_dip_entry_t));

	pMcDipEntryList = osal_alloc(sizeof(multicast_dip_entry_list_t));
    if (!pMcDipEntryList)
    {
        osal_free(pMcDipEntryTemp);
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
	osal_memset(pMcDipEntryList, 0, sizeof(multicast_dip_entry_list_t));
	pMcDipEntryList->dipEntry = pMcDipEntryTemp;
	pMcDipEntryList->next = globalMcDipEntryListHead[ipType];
	globalMcDipEntryListHead[ipType] = pMcDipEntryList;

    IGMP_DB_SEM_UNLOCK();
	return ret;
}

static int32 mcast_filter_mc_on_pon_entry_del(
    multicast_ipType_t ipType, multicast_dip_entry_t *pDipEntry)
{
	multicast_dip_entry_list_t   *pMcDipEntryList   = NULL, *pMcDipEntryListPrev = NULL;
	//int32                        ret                = SYS_ERR_OK;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType >= MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	pMcDipEntryList = globalMcDipEntryListHead[ipType];

	while (pMcDipEntryList)
	{
	    if (pMcDipEntryList->dipEntry->id == pDipEntry->id)
		{
    		mcast_filter_mc_on_pon_entry_hw_oper(SYS_OP_DEL, ipType, pMcDipEntryList->dipEntry);
            if (pMcDipEntryListPrev)
			{
				pMcDipEntryListPrev->next = pMcDipEntryList->next;
			}
            else
            {
				globalMcDipEntryListHead[ipType] = pMcDipEntryList->next;
			}
    		osal_free(pMcDipEntryList->dipEntry);
    		osal_free(pMcDipEntryList);
            return SYS_ERR_OK;
        }

		pMcDipEntryListPrev = pMcDipEntryList;
		pMcDipEntryList  = pMcDipEntryList->next;

	}
	return SYS_ERR_OK;
}

void mcast_config_handler(ipcMsg_t *pMsg)
{
    if (NULL == pMsg)
        return;

	switch (pMsg->msgType)
	{
		case MCAST_MSGTYPE_PORT_GROUP_LIMIT_SET:
		{
			mcast_portGroupLimit_t *portGroupLimit;
			int32 i;

			portGroupLimit = (mcast_portGroupLimit_t *)pMsg->data;
			if (255 == portGroupLimit->port)
            {
	   			for (i = LOGIC_PORT_START; i < HAL_GET_CPU_PORT(); i++)
	   			{
		 			mcast_portGroup_limit_set(portGroupLimit->ipType, i, portGroupLimit->maxnum);
	   			}
	 		}
			else
	 		{
	   			mcast_portGroup_limit_set(portGroupLimit->ipType, portGroupLimit->port, portGroupLimit->maxnum);
	 		}
			break;
		}
		case MCAST_MSGTYPE_PORT_GROUP_LIMIT_GET:
		{
			mcast_portGroupLimit_t *portGroupLimit;
			uint16 maxnum;

			portGroupLimit = (mcast_portGroupLimit_t *)pMsg->data;
			mcast_portGroup_limit_get(portGroupLimit->ipType, portGroupLimit->port, &maxnum);
			portGroupLimit->maxnum = maxnum;

			if ( -1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

            break;
		}
		case MCAST_MSGTYPE_MCAST_VLAN_ADD:
		{
			mcast_mcVlan_t *mcVlan;

			mcVlan = (mcast_mcVlan_t *)pMsg->data;
			mcast_igmpMcVlanAdd(mcVlan->portId, mcVlan->vlanId);
			break;
		}
		case MCAST_MSGTYPE_MCAST_VLAN_DEL:
		{
			mcast_mcVlan_t *mcVlan;

			mcVlan = (mcast_mcVlan_t *)pMsg->data;
			mcast_igmpMcVlanDelete(mcVlan->portId, mcVlan->vlanId);
			break;
		}
		case MCAST_MSGTYPE_MCAST_VLAN_CLEAR:
		{
			uint32 portId;

			osal_memcpy(&portId, pMsg->data, sizeof(uint32));
			mcast_igmpMcVlanClear(portId);
			break;
		}
		case MCAST_MSGTYPE_MCTAG_OPER_SET:
		{
			mcast_mcTagOper_t *mcTagOper;

			mcTagOper = (mcast_mcTagOper_t *)pMsg->data;
			mcast_igmpMcTagstripSet(mcTagOper->portId, mcTagOper->tagOper);
			break;
		}
		case MCAST_MSGTYPE_MCTAG_TRANSLATION_ADD:
		{
			mcast_mcTagTranslation_t *mcTagTranslation;

			mcTagTranslation = (mcast_mcTagTranslation_t *)pMsg->data;
			mcast_igmpMcTagTranslationAdd(mcTagTranslation->portId, mcTagTranslation->mcVid, mcTagTranslation->userVid);
			break;
		}
		case MCAST_MSGTYPE_MCAST_GROUP_DEL_BY_TYPE:
		{
			mcast_group_type_t type;

			osal_memcpy(&type, pMsg->data, sizeof(mcast_group_type_t));
			mcast_igmp_group_del_byType(type);
			break;
		}
		case MCAST_MSGTYPE_GROUPMBRPORT_ADD:
		{
			mcast_groupInfo_t *groupMbrPort;

			groupMbrPort = (mcast_groupInfo_t *)pMsg->data;
			//currently only suppport igmp v2, TBD:  source filteriing , check later
			mcast_groupMbrPort_add_wrapper(
			    groupMbrPort->ipType, groupMbrPort->vid, &groupMbrPort->buf, 0,
			    &groupMbrPort->groupDip, groupMbrPort->port,  &groupMbrPort->sendFlag,
			    groupMbrPort->clientIp, NULL);

			/*return sendFlag*/
			if ( -1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

            break;
		}
		case MCAST_MSGTYPE_GROUPMBRPORT_DEL:
		{
			mcast_groupInfo_t *groupMbrPort;

			groupMbrPort = (mcast_groupInfo_t *)pMsg->data;

            if (MULTICAST_TYPE_IPV4 > groupMbrPort->ipType ||
                MULTICAST_TYPE_END <= groupMbrPort->ipType ||
                IGMP_LOOKUP_MODE_END <= p_igmp_stats->lookup_mode)
                return;

			igmp_lookup_mode_func[groupMbrPort->ipType * IGMP_LOOKUP_MODE_END +
                p_igmp_stats->lookup_mode].igmp_groupMbrPort_del_wrapper(groupMbrPort->ipType,
                groupMbrPort->vid, &groupMbrPort->buf, &groupMbrPort->groupDip,
                groupMbrPort->port, groupMbrPort->clientIp);
			break;
		}
		case MCAST_MSGTYPE_GROUPMCASTDATA_ADD:
		{
			mcast_groupInfo_t *groupMcastData;

			groupMcastData = (mcast_groupInfo_t *)pMsg->data;

            if (MULTICAST_TYPE_IPV4 > groupMcastData->ipType ||
                MULTICAST_TYPE_END <= groupMcastData->ipType ||
                IGMP_LOOKUP_MODE_END <= p_igmp_stats->lookup_mode)
                return;

			igmp_lookup_mode_func[groupMcastData->ipType * IGMP_LOOKUP_MODE_END +
                p_igmp_stats->lookup_mode].igmp_group_mcstData_add_wrapper(groupMcastData->ipType,
                groupMcastData->vid, &groupMcastData->buf, &groupMcastData->groupDip,
                &groupMcastData->sip, groupMcastData->port, groupMcastData->clientIp);
			break;
		}
		case MCAST_MSGTYPE_V3_GROUPMCASTDATA_ADD:
		{
			mcast_groupInfo_t *groupMcastData;

			groupMcastData = (mcast_groupInfo_t *)pMsg->data;

            if (MULTICAST_TYPE_IPV4 > groupMcastData->ipType ||
                MULTICAST_TYPE_END <= groupMcastData->ipType ||
                IGMP_LOOKUP_MODE_END <= p_igmp_stats->lookup_mode)
                return;

			igmp_lookup_mode_func[groupMcastData->ipType * IGMP_LOOKUP_MODE_END +
                p_igmp_stats->lookup_mode].igmpv3_group_mcstData_add_wrapper(groupMcastData->ipType,
                groupMcastData->vid, &groupMcastData->buf, &groupMcastData->groupDip,
                &groupMcastData->sip, groupMcastData->port, groupMcastData->clientIp);
			break;
		}
		case MCAST_MSGTYPE_IGMP_MODE_SET:
		{
			mcast_portIgmpMode_t *igmpMode;

			igmpMode = (mcast_portIgmpMode_t *)pMsg->data;
			mcast_igmpMode_set(igmpMode->mode);
			break;
		}
		case MCAST_MSGTYPE_FASTLEAVE_MODE_SET:
		{
			mcast_portFastLeaveMode_t *fastLeaveMode;

			fastLeaveMode = (mcast_portFastLeaveMode_t *)pMsg->data;
			mcast_fastLeaveMode_set(fastLeaveMode->enable, fastLeaveMode->port);
			break;
		}
		case MCAST_MSGTYPE_GROUPENTRY_GET:
		{
			int32 ret;
			void *shm;

			mcast_shm_create(MCAST_MSGTYPE_GROUPENTRY_GET, &shm);
			ret = mcast_group_sortedArray_copy2shm((char *)shm);
			/*Send return value, for let getting process waiting until shm copy complete*/
			*(int32 *)pMsg->data = ret;
			if (-1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));
			
			if (-1 == shmdt(shm))
			{
				printf("shmdt failed, %s()\n", __FUNCTION__);	
			}
			break;
		}
		case MCAST_MSGTYPE_ACLENTRY_SET:
		{
			mcast_aclEntry_t *aclEntry;
			aclEntry = (mcast_aclEntry_t *)pMsg->data;
			mcast_acl_entry_set(aclEntry->ipType, aclEntry->aclType, &aclEntry->aclEntry);
			break;
		}
		case MCAST_MSGTYPE_ACLENTRY_DEL:
		{
			mcast_aclEntry_t *aclEntry;

			aclEntry = (mcast_aclEntry_t *)pMsg->data;

			mcast_acl_entry_del(aclEntry->ipType, aclEntry->aclType, aclEntry->aclEntry.id);
			break;
		}
		case MCAST_MSGTYPE_PROFILE_SET:
		{
			mcast_mcProfile_t *mcProfile;
			mcProfile = (mcast_mcProfile_t *)pMsg->data;
			mcast_igmp_acl2profile_add(mcProfile->ipType, mcProfile->aclType, mcProfile->port, mcProfile->aclEntryId);
			break;
		}
		case MCAST_MSGTYPE_PROFILE_DEL:
		{
			mcast_mcProfile_t *mcProfile;

			mcProfile = (mcast_mcProfile_t *)pMsg->data;

			mcast_igmp_acl2profile_del(mcProfile->ipType, mcProfile->aclType, mcProfile->port, mcProfile->aclEntryId);
			break;
		}
		case MCAST_MSGTYPE_ACLENTRY_PRINT:
		{
			mcast_aclEntry_t *aclEntry;

			aclEntry = (mcast_aclEntry_t *)pMsg->data;

			mcast_acl_entry_print_all(aclEntry->ipType, aclEntry->aclType);
			break;
		}
		case MCAST_MSGTYPE_PROFILE_PRINT:
		{
			mcast_mcProfile_t *mcProfile;

			mcProfile = (mcast_mcProfile_t *)pMsg->data;

			mcast_igmp_profile_aclEntry_print(mcProfile->ipType, mcProfile->aclType, mcProfile->port);
			break;
		}
		case MACST_MSGTYPE_IGMPTRAP_SET:
		{
			mcast_igmpTrapAct_t *igmpTrapAct;

			igmpTrapAct = (mcast_igmpTrapAct_t *)pMsg->data;

            mcast_ctrl_pkt_trap_set(igmpTrapAct);

			break;
		}
		case MCAST_MSGTYPE_PORT_CFG_PRINT:
		{
			unsigned int *pPort = (unsigned int *)pMsg->data;
			mcast_port_cfg_print(*pPort);
			break;
		}
		case MCAST_MSGTYPE_PORT_FLOW_INFO_ADD:
		{
			mcast_port_info_t *pIgmpPortInfo = NULL;

			pIgmpPortInfo = (mcast_port_info_t *)pMsg->data;
			mcast_port_flow_info_add(pIgmpPortInfo);
			break;
		}
		case MCAST_MSGTYPE_PORT_FLOW_INFO_DEL:
		{
			mcast_port_info_t *pIgmpPortInfo;

			pIgmpPortInfo = (mcast_port_info_t *)pMsg->data;
			mcast_port_flow_info_del(pIgmpPortInfo);
			break;
		}
		case MCAST_MSGTYPE_IGMP_RATELIMIT_SET:
		{
			mcast_igmpRateLimit_t *igmpRateLimit;

			igmpRateLimit = (mcast_igmpRateLimit_t *)pMsg->data;
			mcast_igmp_rate_limit_set(igmpRateLimit->port, igmpRateLimit->packetRate);
			break;
		}
		case MCAST_MSGTYPE_COUNTER_GET:
		{
			mcast_counter_t *counter;

			counter = (mcast_counter_t *)pMsg->data;

			mcast_us_igmp_counter_get(counter->port, &counter->igmpUsRx, &counter->igmpUsRxDrop);

			if ( -1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

            break;
		}
		case MCAST_MSGTYPE_RESET_ALL:
		{
			/*reset all igmp config*/
			mcast_reset_all_config();
			break;
		}
		case MCAST_MSGTYPE_GLB_CFG_PRINT:
		{
            sys_logic_port_t port;
			printf("igmp function: %u\n", igmpCtrl.igmpMode);
            switch (igmpCtrl.ponMode)
            {
                case MODE_FIBER:
                    printf("ponMode: fiber\n");
                    break;
                case MODE_GPON:
                    printf("ponMode: gpon\n");
                    break;
                case MODE_EPON:
                    printf("ponMode: epon\n");
                    break;
                default:
                    printf("ponMode: unkown\n");
            }
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, igmpCtrl.igmpMldTrap_enable)
            {
                printf("Port %u trap igmp/mld\n", port);
            }
            printf("sys has ip mc tbl: %u\n", igmpCtrl.sys_has_ip_mc_tbl);
            if (SDK_MODE_LUNA == igmpCtrl.sdkMode)
            {
                printf("sdk type: luna\n");
            }
            else if (SDK_MODE_OPENWRT == igmpCtrl.sdkMode)
            {
                printf("sdk type: openwrt\n");
            }
            else
                printf("sdk type: unknown\n");
			break;
		}
		case MCAST_MSGTYPE_ACTIVE_GRP_SHOW:
		{
			mcast_group_print_all();
			break;
		}
		case MCAST_MSGTYPE_ROBUSTNESS_SET:
		case MCAST_MSGTYPE_QUERIERIP_SET:
		case MCAST_MSGTYPE_QUERYINTVAL_SET:
		case MCAST_MSGTYPE_QUERYMAXRSPTIME_SET:
		case MCAST_MSGTYPE_LASTMBRQUERYINTVAL_SET:
		case MCAST_MSGTYPE_DSIGMPMCASTTCI_SET:
		case MCAST_MSGTYPE_UNAUTHJOINBHVR_SET:
		case MCAST_MSGTYPE_MCASTPROF_DEL:
		{
			mcast_prof_t *pMcastProf = NULL;

			pMcastProf = (mcast_prof_t *)pMsg->data;
			mcast_profile_set(pMsg->msgType, pMcastProf);
			break;
		}
		case MCAST_MSGTYPE_MCASTPROF_PRINT:
		{
			mcast_profile_print_all();
			break;
		}
		case MCAST_MSGTYPE_PREVIEW_TIMER_SHOW:
		{
			mcast_preview_timer_print_all();
			break;
		}
		case MCAST_MSGTYPE_ALLOWED_PREVIEW_SET:
		case MCAST_MSGTYPE_ALLOWED_PREVIEW_DEL:
		{
			mcast_allowed_preview_entry_t *pAllowPreview = NULL;

			pAllowPreview = (mcast_allowed_preview_entry_t *)pMsg->data;
			mcast_allowed_preview_entry_set(pMsg->msgType, pAllowPreview);
			break;
		}
		case MCAST_MSGTYPE_ALLOWED_PREVIEW_PRINT:
		{
			unsigned int *pPort = (unsigned int *)pMsg->data;
			mcast_allowed_preview_entry_print(*pPort);
			break;
		}
		case MCAST_MSGTYPE_PORT_MAX_MCAST_BW_SET:
		{
			mcast_portBwInfo_t *pPortBwInfo;

			pPortBwInfo = (mcast_portBwInfo_t *)pMsg->data;
			mcast_portMaxBw_set(pPortBwInfo->port, pPortBwInfo->maxBw);
			break;
		}
		case MCAST_MSGTYPE_PORT_BW_ENFORCE_SET:
		{
			mcast_portBwInfo_t *pPortBwInfo;

			pPortBwInfo = (mcast_portBwInfo_t *)pMsg->data;
			mcast_portBwEnforce_set(pPortBwInfo->port, pPortBwInfo->bwEnforceB);
			break;
		}
		case MCAST_MSGTYPE_PORT_CURR_BW_GET:
		{
			mcast_portStat_t *pPortStat;

			pPortStat = (mcast_portStat_t *)pMsg->data;
			mcast_portCurrBw_get(pPortStat);
			if (-1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));
			break;
		}
		case MCAST_MSGTYPE_PORT_JOIN_COUNT_GET:
		{
			mcast_portStat_t *pPortStat;

			pPortStat = (mcast_portStat_t *)pMsg->data;
			mcast_portJoinCnt_get(pPortStat);
			if (-1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

            break;
		}
		case MCAST_MSGTYPE_PORT_BW_EXCEED_COUNT_GET:
		{
			mcast_portStat_t *pPortStat;

			pPortStat = (mcast_portStat_t *)pMsg->data;
			mcast_portBwExcdCnt_get(pPortStat);
			if (-1 ==(send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

			break;
		}
		case MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET:
		{
			mcast_portStat_t    *pPortStat;
			void                *shm, *p;

			pPortStat = (mcast_portStat_t *)pMsg->data;
			mcast_portIpv4ActiveGrpCnt_get(pPortStat);
			mcast_shm_create(MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET, &shm);
			p = shm;
			mcast_active_group_copy2shm(MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET, pPortStat->port, p);
			if ( -1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
                printf("send error: %s\n", strerror(errno));

            if (-1 == shmdt(shm))
			{
				printf("shmdt failed, %s()\n", __FUNCTION__);
			}
			break;
		}
		case MCAST_MSGTYPE_PORT_IPV6_ACTIVE_GRP_COUNT_GET:
		{
			mcast_portStat_t *pPortStat;

			pPortStat = (mcast_portStat_t *)pMsg->data;
			mcast_portIpv6ActiveGrpCnt_get(pPortStat);
			if ( -1 == (send(cSockfd, pMsg, sizeof(ipcMsg_t), 0)))
            {
                printf("send error: %s\n", strerror(errno));
            }
			break;
		}
		case MCAST_MSGTYPE_PORT_STAT_PRINT:
		{
			unsigned int *pPort = (unsigned int *)pMsg->data;
			mcast_port_stat_print(*pPort);
			break;
		}
		case MCAST_MSGTYPE_IGMP_LOG_SET:
		{
			uint32 *pVal = (uint32 *)pMsg->data;
			igmpCtrl.logEnable = *pVal;
			break;
		}
		case MCAST_MSGTYPE_INFO_DEBUG:
		{
			mcast_debug_info_print();
			break;
		}
		case MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_SET:
		{
			mcast_filter_mc_on_pon_entry_t *pFilterMcOnPonEntry;

			pFilterMcOnPonEntry = (mcast_filter_mc_on_pon_entry_t *)pMsg->data;

            mcast_filter_mc_on_pon_entry_set(pFilterMcOnPonEntry->ipType, &pFilterMcOnPonEntry->dipEntry);
			break;
		}
		case MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_DEL:
		{
			mcast_filter_mc_on_pon_entry_t *pFilterMcOnPonEntry;

			pFilterMcOnPonEntry = (mcast_filter_mc_on_pon_entry_t *)pMsg->data;

			mcast_filter_mc_on_pon_entry_del(pFilterMcOnPonEntry->ipType, &pFilterMcOnPonEntry->dipEntry);
			break;
		}
        case MCAST_MSGTYPE_FILTER_MC_ON_PON_MODE_SET:
        {
			uint32 *pMode = (uint32 *)pMsg->data;

            if (RT_ERR_OK != pf_gmac_filter_mode_set(*pMode))
            {
                printf("%s() %d: mode=%u failed \n",
                    __FUNCTION__, __LINE__, *pMode);
            }
            break;
        }
        case MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_PRINT:
        {
            mcast_filter_mc_on_pon_entry_t *pFilterMcEntry;

			pFilterMcEntry = (mcast_filter_mc_on_pon_entry_t *)pMsg->data;

            mcast_filter_mc_on_pon_entry_print(pFilterMcEntry->ipType);
            break;
        }
		default:
			break;
	}
}

static void mcast_config_rxThread(void *arg)
{
	int                 sockfd, sin_size;
	struct sockaddr_un  my_addr;
	struct sockaddr_un  their_addr;
	ipcMsg_t            pMsg;

	prctl(PR_SET_NAME, (unsigned long)((char *)arg));

	osal_memset(&pMsg, 0 , sizeof(ipcMsg_t));

	if (-1 == (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)))
	{
		printf("socket create error: %s\n", strerror(errno));
		return;
	}

	my_addr.sun_family = AF_UNIX;
	strcpy(my_addr.sun_path, SOCKET_PATH);

	if (-1 == bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr_un)))
	{
		printf("bind error: %s\n", strerror(errno));
		close(sockfd);
		return;
	}

	if (-1 == listen(sockfd, 20))
	{
		printf("listen error: %s\n", strerror(errno));
		close(sockfd);
		return;
	}
	printf("listening\n");

	while (1)
	{
		if (-1 == (cSockfd = accept(sockfd, (struct sockaddr*)&their_addr, (socklen_t *)&sin_size)))
		{
			printf("accept error: %s\n", strerror(errno));
			close(sockfd);
			return;
		}

		if ( -1 == (recv(cSockfd, &pMsg, sizeof(ipcMsg_t), 0)))
        {
			printf("recv error: %s\n", strerror(errno));
        }

		mcast_config_handler(&pMsg);

		close(cSockfd);
	}
	close(sockfd);
    return;
}

int32 igmp_mld_cfg_msg_task_init(void)
{
    if ((osal_thread_t)NULL == (igmp_thread_create("IGMP CFG Thread", STACK_SIZE,
        SYS_THREAD_PRI_IGMP_CFG, (void *)mcast_config_rxThread, "IGMP_CFG")))
	{
		printf("IGMP Tick Thread create failed\n");
		return SYS_ERR_FAILED;
	}
    return SYS_ERR_OK;
}
