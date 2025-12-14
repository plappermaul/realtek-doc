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

static igmp_router_db_t         router_db[MULTICAST_TYPE_END];
static igmp_router_entry_t      *p_tmp_router_array;
static igmp_aggregate_db_t      aggregate_db;
static igmp_aggregate_entry_t   *p_tmp_aggr_array;
static int32                    igmp_group_num;
static int32                    mld_group_num;
static igmp_group_entry_t       *p_group_db;
static igmp_group_entry_t       **pp_tmp_group_sorted_array;
static igmp_group_head_t        igmpGroupFreeHead;
static mcast_vlan_db_t          vlan_db[MULTICAST_TYPE_END];
static mcast_vlan_entry_t       *p_tmp_vlan_array;

uint8                           igmp_trapDataPkt_lock;
int32                           group_sortedAry_entry_num;
uint32                          igmp_hw_entry_num; /* The number of entries that have writen into L2 table */
igmp_group_entry_t              **pp_group_sorted_array;
igmp_group_head_entry_t         *p_group_head_db;
igmp_static_group_t             groupSave;
igmp_acl_entry_list_t           *globalAclEntryListHead[MULTICAST_TYPE_END][IGMP_GROUP_ENTRY_ALL];
multicast_dip_entry_list_t      *globalMcDipEntryListHead[MULTICAST_TYPE_END];
sys_vlanmask_t                  mcastVlanCfgMsk[MULTICAST_TYPE_END];
uint32                          mcast_group_num;
uint32                          *p_vlan_ref_cnt;

static int32 mcast_vlan_db_init(void);
static int32 mcast_router_db_init(void);
static int32 mcast_aggregate_db_init(void);
static int32 mcast_group_db_init(void);
static int32 mcast_igmp_group_head_db_init(void);
static int32 mcast_igmp_group_head_ins(uint32 groupAddr, uint16 vid, uint16 portid, uint16 rcdType, igmp_group_head_entry_t **ppGrpHead, uint32 timer);
static uint64 mcast_aggregate_sortKey_ret(multicast_ipType_t ipType, uint32 dip, sys_vid_t vid);
static int32 mcast_port_entry_get(multicast_ipType_t ipType, sys_logic_port_t port, uint32 *pEntryNum);
static int32 mcast_maxGroupAct_process(multicast_ipType_t ipType, sys_vid_t vid, uint32 dip, uint32 sip,sys_logic_port_t port);
static int32 mcast_is_last_host_per_port(igmp_group_entry_t *pGroupHead, uint16 portId);
static int32 mcast_host_tracking_find(uint16 portId, igmp_group_entry_t *pGroupHead, ipAddr_t clientIp);
static int32 mcast_host_tracking_add(igmp_group_entry_t *pGroupHead, ipAddr_t clientIp, uint16 portId);
static int32 mcast_host_tracking_del(igmp_group_entry_t *pGroupHead, ipAddr_t clientIp, uint16 portId);
static int32 mcast_port_stats_update(op_code_t op, uint16 portId, igmp_group_entry_t *pGroupHead, ipAddr_t clientIp);
static int32 mcast_igmp_subgroup_sip_search(igmp_group_entry_t *gipHead, uint32 key, igmp_group_entry_t **subgroup, igmp_group_entry_t **previous);

void mcast_group_rx_timer_stop(void)
{
    mcast_timer_stop = ENABLED;
    igmp_packet_stop = ENABLED;
    osal_time_usleep(5 * 1000);
}

void mcast_group_rx_timer_start(void)
{
    mcast_timer_stop = DISABLED;
    igmp_packet_stop = DISABLED;
}

void mcast_group_num_cal(multicast_ipType_t  ipType, uint8 isInc)
{
    sys_logic_port_t port;
    if (ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END)
        return ;

    if (TRUE == isInc)
    {
        mcast_group_num++;

        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            igmp_group_num++;
        }
        else
        {
            mld_group_num++;
        }

        if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action)
            {
                if (SYS_MCAST_MAX_GROUP_NUM == mcast_group_num)
                {
                    //rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, ENABLED);  wanghuanyu
                    //rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, ENABLED);
                    igmp_trapDataPkt_lock = TRUE;
                }
            }
        }
    }
    else
    {
        mcast_group_num--;
        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            igmp_group_num--;
        }
        else
        {
            mld_group_num--;
        }

        if (igmp_trapDataPkt_lock && mcast_group_num < SYS_MCAST_MAX_GROUP_NUM)
        {
            if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
                (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
            {
                if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action)
                {
                    if (ENABLED == p_igmp_stats->igmpsnp_enable)
                    {
                        //rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, DISABLED);wanghuanyu
                    }
                    if (ENABLED == p_mld_stats->mldsnp_enable)
                    {
                        //rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, DISABLED);
                    }
                }
            }
            else
            {
                LgcPortFor(port)
            	{
            	    /*All LAN ports*/
            	    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(ENABLED, port);
            	}
            }
            igmp_trapDataPkt_lock = FALSE;
        }

    }

}

int32 mcast_vlan_array_search(uint16 search, multicast_ipType_t ipType,uint16 *idx, mcast_vlan_entry_t **entry)
{
    int low = 0;
    int mid;

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    int high = vlan_db[ipType].entry_num - 1;
    mcast_vlan_entry_t *sortedArray = vlan_db[ipType].p_vlan_entry;

    SYS_ERR_CHK((idx == NULL || entry == NULL), SYS_ERR_FAILED);

    while (low <= high)
    {
        mid = (low + high) / 2;

        if (sortedArray[mid].vid == 0)   /* this case occurs when sorted array is empty */
        {
            *entry = NULL;
            *idx = mid;
            return SYS_ERR_OK;
        }

        if (sortedArray[mid].vid == search)
        {
            *entry = &sortedArray[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        else if (sortedArray[mid].vid > search)
        {
            high = mid - 1;
        }
        else if (sortedArray[mid].vid < search)
        {
            low = mid + 1;
        }
    }

    *entry = NULL;
    *idx = low;
    return SYS_ERR_OK;
}

int32 mcast_vlan_array_ins(uint16 sortedIdx, multicast_ipType_t ipType, mcast_vlan_entry_t *entry)
{
    SYS_ERR_CHK((!entry), SYS_ERR_FAILED);

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx > vlan_db[ipType].entry_num), SYS_ERR_FAILED);

    SYS_ERR_CHK((vlan_db[ipType].entry_num >= MAX_ROUTER_VLAN), SYS_ERR_FAILED);

    /* Move data */
    osal_memcpy(p_tmp_vlan_array, vlan_db[ipType].p_vlan_entry + sortedIdx, sizeof(mcast_vlan_entry_t) * (vlan_db[ipType].entry_num - sortedIdx));
    osal_memcpy(vlan_db[ipType].p_vlan_entry + sortedIdx, entry, sizeof(mcast_vlan_entry_t));
    osal_memcpy(vlan_db[ipType].p_vlan_entry + sortedIdx + 1, p_tmp_vlan_array, sizeof(mcast_vlan_entry_t) * (vlan_db[ipType].entry_num - sortedIdx));
    vlan_db[ipType].entry_num++;

    return SYS_ERR_OK;
}

int32 mcast_vlan_array_remove(uint16 sortedIdx, multicast_ipType_t ipType)
{

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx >= vlan_db[ipType].entry_num), SYS_ERR_FAILED);

    osal_memcpy(p_tmp_vlan_array, vlan_db[ipType].p_vlan_entry + sortedIdx + 1, sizeof(mcast_vlan_entry_t) * (vlan_db[ipType].entry_num - sortedIdx - 1));
    osal_memcpy(vlan_db[ipType].p_vlan_entry + sortedIdx, p_tmp_vlan_array, sizeof(mcast_vlan_entry_t) * (vlan_db[ipType].entry_num - sortedIdx - 1));
    vlan_db[ipType].entry_num--;
    osal_memset(vlan_db[ipType].p_vlan_entry + vlan_db[ipType].entry_num, 0, sizeof(mcast_vlan_entry_t));

    return SYS_ERR_OK;
}

int32 mcast_vlan_db_init(void)
{
    uint8 i;

    for (i = 0; i < MULTICAST_TYPE_END; i++)
    {
        osal_memset(&mcastVlanCfgMsk[i], 0, sizeof(mcastVlanCfgMsk[i]));

        osal_memset(&vlan_db[i], 0, sizeof(mcast_vlan_db_t));

        vlan_db[i].p_vlan_entry = (mcast_vlan_entry_t *)osal_alloc(sizeof(mcast_vlan_entry_t) * MAX_ROUTER_VLAN);

        SYS_ERR_CHK((!vlan_db[i].p_vlan_entry), SYS_ERR_FAILED);

        osal_memset(vlan_db[i].p_vlan_entry, 0, sizeof(mcast_vlan_entry_t) * MAX_ROUTER_VLAN);
    }

    p_tmp_vlan_array = (mcast_vlan_entry_t *)osal_alloc(sizeof(mcast_vlan_entry_t) * (MAX_ROUTER_VLAN - 1));

    SYS_ERR_CHK((!p_tmp_vlan_array), SYS_ERR_FAILED);

    osal_memset(p_tmp_vlan_array, 0, sizeof(mcast_vlan_entry_t) * (MAX_ROUTER_VLAN - 1));

    // TBD EPON?
    p_vlan_ref_cnt = (uint32 *)osal_alloc(sizeof(uint32) * 4096);
    if (p_vlan_ref_cnt)
    {
        osal_memset(p_vlan_ref_cnt, 0, sizeof(uint32) * 4096);
    }

    /* igmp tag per port init */
    if (!(p_igmp_tag_oper_per_port = (uint32 *)osal_alloc(sizeof(uint32) * (igmpCtrl.sys_max_port_num - 1))))
        goto alloc_failed;
    osal_memset(p_igmp_tag_oper_per_port, 0, sizeof(uint32) * (igmpCtrl.sys_max_port_num - 1));

    if (!(p_igmp_tag_translation_table = (igmp_vlan_translation_entry_t *)osal_alloc(sizeof(igmp_vlan_translation_entry_t) * VLAN_TRANSLATION_ENTRY)))
        goto alloc_failed;
    osal_memset(p_igmp_tag_translation_table, 0, sizeof(igmp_vlan_translation_entry_t) * VLAN_TRANSLATION_ENTRY);

    return SYS_ERR_OK;

alloc_failed:
    if (p_igmp_tag_oper_per_port)
        osal_free(p_igmp_tag_oper_per_port);

    if (p_igmp_tag_translation_table)
        osal_free(p_igmp_tag_translation_table);

    return SYS_ERR_FAILED;
}

int32 mcast_vlan_db_add(uint16 vid, multicast_ipType_t ipType)
{
    uint16              sortedIdx;
    mcast_vlan_entry_t *entry = NULL;
    mcast_vlan_entry_t newEntry;

    mcast_vlan_array_search(vid,ipType, &sortedIdx, &entry);
    if (entry)
    {
        // vlan db entry is exist.
        return SYS_ERR_OK;
    }
    else  /* not found */
    {
        osal_memset(&newEntry, 0, sizeof(mcast_vlan_entry_t));

        //add default setting
        newEntry.vid = vid;
        newEntry.ipType = ipType;
        LOGIC_PORTMASK_CLEAR_ALL(newEntry.staticMbr);
        LOGIC_PORTMASK_CLEAR_ALL(newEntry.forbidMbr);

        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            newEntry.enable = DFLT_IGMP_VLAN_EBL;
            newEntry.fastleave_enable = DFLT_IGMP_FAST_LEAVE;
            newEntry.pimDvmrpLearn = DFLT_IGMP_ROUTER_PIMDVMRP_LEARN;

            newEntry.router_port_timeout =  DFLT_IGMP_ROUTER_TIMEOUT;
            newEntry.grpMembershipIntv = DFLT_IGMP_GROUP_MEMBERSHIP_INTV;
            newEntry.robustnessVar = DFLT_IGMP_ROBUSTNESS_VAR;
            newEntry.operRobNessVar = newEntry.robustnessVar;
            newEntry.queryIntv = DFLT_IGMP_QUERY_INTV;
            newEntry.operQueryIntv = newEntry.queryIntv;
            newEntry.lastMmbrQueryCnt = DFLT_IGMP_ROBUSTNESS_VAR;
            newEntry.operLastMmbrQueryCnt = newEntry.lastMmbrQueryCnt;
            newEntry.grpSpecificQueryIntv = DFLT_IGMP_GS_QUERY_RESP_INTV;
            newEntry.operGsqueryIntv =  newEntry.grpSpecificQueryIntv;
            newEntry.responseTime = DFLT_IGMP_QUERY_RESPONSE_INTV;
            newEntry.operRespTime = newEntry.responseTime;

            newEntry.otherQuerierPresentInterval = (DFLT_IGMP_ROBUSTNESS_VAR * DFLT_IGMP_QUERY_INTV) + DFLT_IGMP_QUERY_RESPONSE_INTV/2;
        }
        else
        {
            newEntry.enable = DFLT_MLD_VLAN_EBL;
            newEntry.fastleave_enable = DFLT_MLD_FAST_LEAVE;
            newEntry.pimDvmrpLearn = DFLT_MLD_ROUTER_PIMDVMRP_LEARN;

            newEntry.router_port_timeout =  DFLT_MLD_ROUTER_TIMEOUT;
            newEntry.grpMembershipIntv = DFLT_MLD_GROUP_MEMBERSHIP_INTV;
            newEntry.robustnessVar = DFLT_MLD_ROBUSTNESS_VAR;
            newEntry.operRobNessVar = newEntry.robustnessVar;
            newEntry.queryIntv = DFLT_MLD_QUERY_INTV;
            newEntry.operQueryIntv = newEntry.queryIntv;
            newEntry.lastMmbrQueryCnt = DFLT_MLD_ROBUSTNESS_VAR;
            newEntry.operLastMmbrQueryCnt = newEntry.lastMmbrQueryCnt;
            newEntry.grpSpecificQueryIntv = DFLT_MLD_GS_QUERY_RESP_INTV;
            newEntry.operGsqueryIntv =  newEntry.grpSpecificQueryIntv;
            newEntry.responseTime = DFLT_MLD_QUERY_RESPONSE_INTV;
            newEntry.operRespTime = newEntry.responseTime;

            newEntry.otherQuerierPresentInterval = (DFLT_MLD_ROBUSTNESS_VAR * DFLT_MLD_QUERY_INTV) + DFLT_MLD_QUERY_RESPONSE_INTV/2;
        }
        return mcast_vlan_array_ins(sortedIdx, ipType,&newEntry);
    }

    return SYS_ERR_OK;
}

int32 mcast_vlan_db_get(uint16 vid, multicast_ipType_t ipType, mcast_vlan_entry_t **ppEntry)
{
    uint16              sortedIdx;

    SYS_PARAM_CHK(NULL == ppEntry, SYS_ERR_NULL_POINTER);

    mcast_vlan_array_search(vid, ipType, &sortedIdx, ppEntry);

    return SYS_ERR_OK;
}

int32 mcast_vlan_db_set(mcast_vlan_entry_t *pEntry)
{
    uint16              sortedIdx;
    mcast_vlan_entry_t *entry = NULL;

    SYS_PARAM_CHK(NULL == pEntry, SYS_ERR_NULL_POINTER);

    mcast_vlan_array_search(pEntry->vid, pEntry->ipType, &sortedIdx, &entry);
    if(entry != NULL)
    {
        entry->enable = pEntry->enable;
        entry->fastleave_enable = pEntry->fastleave_enable;
        entry->pimDvmrpLearn = pEntry->pimDvmrpLearn;
        LOGIC_PORTMASK_COPY(entry->staticMbr, pEntry->staticMbr);
        LOGIC_PORTMASK_COPY(entry->forbidMbr, pEntry->forbidMbr);

        entry->robustnessVar = pEntry->robustnessVar;
        entry->operRobNessVar = pEntry->operRobNessVar;
        entry->queryIntv = pEntry->queryIntv;
        entry->operQueryIntv = pEntry->operQueryIntv;
        entry->responseTime = pEntry->responseTime;
        entry->operRespTime = pEntry->operRespTime;
        entry->lastMmbrQueryCnt = pEntry->lastMmbrQueryCnt;
        entry->operLastMmbrQueryCnt = pEntry->operLastMmbrQueryCnt;
        entry->grpSpecificQueryIntv = pEntry->grpSpecificQueryIntv;
        entry->operGsqueryIntv =  pEntry->operGsqueryIntv;

        entry->otherQuerierPresentInterval = (pEntry->operRobNessVar * pEntry->operQueryIntv) + pEntry->operRespTime/2;
        entry->router_port_timeout = (pEntry->operRobNessVar * pEntry->operQueryIntv) + pEntry->operRespTime;
        entry->grpMembershipIntv = (pEntry->operRobNessVar * pEntry->operQueryIntv) + pEntry->operRespTime;
    }
    else
    {
        SYS_PARAM_CHK(IS_VID_INVALID(pEntry->vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
        SYS_PARAM_CHK((pEntry->ipType < MULTICAST_TYPE_IPV4 ||pEntry->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

        mcast_vlan_db_add(pEntry->vid, pEntry->ipType);
    }

    return SYS_ERR_OK;
}

int32 mcast_vlan_db_del(uint16 vid, multicast_ipType_t  ipType)
{
    uint16          sortedIdx;
    mcast_vlan_entry_t   *entry = NULL;

    mcast_vlan_array_search(vid, ipType,&sortedIdx, &entry);
    if (entry)
    {
        return mcast_vlan_array_remove(sortedIdx, ipType);
    }

    return SYS_ERR_FAILED;
}

int32 mcast_router_array_search(uint16 search, multicast_ipType_t ipType, uint16 *idx, igmp_router_entry_t **entry)
{
    int low = 0;
    int mid;

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    int high = router_db[ipType].entry_num - 1;
    igmp_router_entry_t *sortedArray = router_db[ipType].p_rt_entry;

    SYS_ERR_CHK( (idx == NULL || entry == NULL), SYS_ERR_FAILED);

    while (low <= high)
    {
        mid = (low + high) / 2;

        if (sortedArray[mid].vid == 0)   /* this case occurs when sorted array is empty */
        {
            *entry = NULL;
            *idx = mid;
            return SYS_ERR_OK;
        }

        if (sortedArray[mid].vid == search)
        {
            *entry = &sortedArray[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        else if (sortedArray[mid].vid > search)
        {
            high = mid - 1;
        }
        else if (sortedArray[mid].vid < search)
        {
            low = mid + 1;
        }
    }

    *entry = NULL;
    *idx = low;
    return SYS_ERR_OK;
}

int32 mcast_router_array_ins(uint16 sortedIdx, multicast_ipType_t ipType, igmp_router_entry_t *entry)
{
    igmp_router_entry_t      *ptr = NULL, *ptr_tmp = NULL;
    uint16                    idx, offset;

    SYS_ERR_CHK((!entry), SYS_ERR_FAILED);

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx > router_db[ipType].entry_num), SYS_ERR_FAILED);

    SYS_ERR_CHK((router_db[ipType].entry_num >= MAX_ROUTER_VLAN), SYS_ERR_FAILED);

    /* Move data */
    //osal_memcpy(p_tmp_router_array, router_db[ipType].p_rt_entry + sortedIdx, sizeof(igmp_router_entry_t) * (router_db[ipType].entry_num - sortedIdx));
    //osal_memcpy(router_db[ipType].p_rt_entry + sortedIdx, entry, sizeof(igmp_router_entry_t));
    //osal_memcpy(router_db[ipType].p_rt_entry + sortedIdx + 1, p_tmp_router_array, sizeof(igmp_router_entry_t) * (router_db[ipType].entry_num - sortedIdx));

    offset = 0;
    for(idx = sortedIdx; idx < router_db[ipType].entry_num; idx++)
    {
        //src
        ptr = router_db[ipType].p_rt_entry + idx;
        //dst
        ptr_tmp = p_tmp_router_array + offset;
        ptr_tmp->ipType = ptr->ipType;
        ptr_tmp->vid = ptr->vid;
        ptr_tmp->rtVersion = ptr->rtVersion;
        osal_memcpy(ptr_tmp->p_port_timer, ptr->p_port_timer, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        osal_memcpy(&(ptr_tmp->router_pmsk), &(ptr->router_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr_tmp->router_static_pmsk), &(ptr->router_static_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr_tmp->router_forbid_pmsk), &(ptr->router_forbid_pmsk), sizeof(sys_logic_portmask_t));
        offset++;
    }
    ptr = router_db[ipType].p_rt_entry + sortedIdx;
    ptr->ipType = entry->ipType;
    ptr->vid = entry->vid;
    ptr->rtVersion = entry->rtVersion;
    osal_memcpy(ptr->p_port_timer, entry->p_port_timer, sizeof(uint16) * igmpCtrl.sys_max_port_num);
    osal_memcpy(&(ptr->router_pmsk), &(entry->router_pmsk), sizeof(sys_logic_portmask_t));
    osal_memcpy(&(ptr->router_static_pmsk), &(entry->router_static_pmsk), sizeof(sys_logic_portmask_t));
    osal_memcpy(&(ptr->router_forbid_pmsk), &(entry->router_forbid_pmsk), sizeof(sys_logic_portmask_t));

    offset = 0;
    for(idx = sortedIdx; idx < router_db[ipType].entry_num; idx++)
    {
        //src
        ptr_tmp = router_db[ipType].p_rt_entry + offset;
        //dst
        ptr = p_tmp_router_array + idx + 1;
        ptr->ipType = ptr_tmp->ipType;
        ptr->vid = ptr_tmp->vid;
        ptr->rtVersion = ptr_tmp->rtVersion;
        osal_memcpy(ptr->p_port_timer, ptr_tmp->p_port_timer, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        osal_memcpy(&(ptr->router_pmsk), &(ptr_tmp->router_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr->router_static_pmsk), &(ptr_tmp->router_static_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr->router_forbid_pmsk), &(ptr_tmp->router_forbid_pmsk), sizeof(sys_logic_portmask_t));
        offset++;
    }

    router_db[ipType].entry_num++;

    return SYS_ERR_OK;
}

int32 mcast_router_array_remove(uint16 sortedIdx, multicast_ipType_t ipType)
{
    uint16 idx, offset;
    igmp_router_entry_t *ptr = NULL, *ptr_tmp = NULL;

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx >= router_db[ipType].entry_num), SYS_ERR_FAILED);

    //osal_memcpy(p_tmp_router_array, router_db[ipType].p_rt_entry + sortedIdx + 1, sizeof(igmp_router_entry_t) * (router_db[ipType].entry_num - sortedIdx - 1));
    //osal_memcpy(router_db[ipType].p_rt_entry + sortedIdx, p_tmp_router_array, sizeof(igmp_router_entry_t) * (router_db[ipType].entry_num - sortedIdx - 1));

    offset = 0;
    for (idx = sortedIdx + 1; idx < router_db[ipType].entry_num; idx++)
    {
        //src
        ptr = router_db[ipType].p_rt_entry + idx;
        //dst
        ptr_tmp = p_tmp_router_array + offset;
        ptr_tmp->ipType = ptr->ipType;
        ptr_tmp->vid = ptr->vid;
        ptr_tmp->rtVersion = ptr->rtVersion;
        osal_memcpy(ptr_tmp->p_port_timer, ptr->p_port_timer, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        osal_memcpy(&(ptr_tmp->router_pmsk), &(ptr->router_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr_tmp->router_static_pmsk), &(ptr->router_static_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr_tmp->router_forbid_pmsk), &(ptr->router_forbid_pmsk), sizeof(sys_logic_portmask_t));
        offset++;
    }

    offset = 0;
    for(idx = sortedIdx + 1; idx < aggregate_db.entry_num; idx++)
    {
        //src
        ptr_tmp = p_tmp_router_array + offset;
        //dst
        ptr = router_db[ipType].p_rt_entry + idx - 1;
        ptr->ipType = ptr_tmp->ipType;
        ptr->vid = ptr_tmp->vid;
        ptr->rtVersion = ptr_tmp->rtVersion;
        osal_memcpy(ptr->p_port_timer, ptr_tmp->p_port_timer, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        osal_memcpy(&(ptr->router_pmsk), &(ptr_tmp->router_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr->router_static_pmsk), &(ptr_tmp->router_static_pmsk), sizeof(sys_logic_portmask_t));
        osal_memcpy(&(ptr->router_forbid_pmsk), &(ptr_tmp->router_forbid_pmsk), sizeof(sys_logic_portmask_t));
        offset++;
    }

    router_db[ipType].entry_num--;
    //osal_memset(router_db[ipType].p_rt_entry + router_db[ipType].entry_num, 0, sizeof(igmp_router_entry_t));
    ptr = router_db[ipType].p_rt_entry + router_db[ipType].entry_num;
    ptr->ipType = 0;
    ptr->vid = 0;
    ptr->rtVersion = 0;
    osal_memset(ptr->p_port_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
    osal_memset(&(ptr->router_pmsk), 0, sizeof(sys_logic_portmask_t));
    osal_memset(&(ptr->router_static_pmsk), 0, sizeof(sys_logic_portmask_t));
    osal_memset(&(ptr->router_forbid_pmsk), 0, sizeof(sys_logic_portmask_t));

    return SYS_ERR_OK;

}

int32 mcast_db_init(void)
{
    int32 ret = SYS_ERR_OK;

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_vlan_db_init())), ret);

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_router_db_init())), ret);

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_aggregate_db_init())), ret);

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_group_db_init())), ret);

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_querier_db_init())), ret);

    SYS_ERR_CHK((SYS_ERR_OK != (ret = mcast_igmp_group_head_db_init())), ret);

    return ret;
}

int32 mcast_router_db_init(void)
{
    multicast_ipType_t ipType;
    uint32 vidIdx;

    osal_memset(router_db, 0, sizeof(igmp_router_db_t) * MULTICAST_TYPE_END);

    p_tmp_router_array = (igmp_router_entry_t *)osal_alloc(sizeof(igmp_router_entry_t) * (MAX_ROUTER_VLAN - 1));

    SYS_ERR_CHK((!p_tmp_router_array), SYS_ERR_FAILED);

    osal_memset(p_tmp_router_array, 0, sizeof(igmp_router_entry_t) * (MAX_ROUTER_VLAN - 1));

    for (ipType = MULTICAST_TYPE_IPV4; ipType < MULTICAST_TYPE_END; ipType++)
    {
        router_db[ipType].p_rt_entry = (igmp_router_entry_t *)osal_alloc(sizeof(igmp_router_entry_t) * MAX_ROUTER_VLAN);

        SYS_ERR_CHK((!router_db[ipType].p_rt_entry), SYS_ERR_FAILED);

        osal_memset(router_db[ipType].p_rt_entry, 0, sizeof(igmp_router_entry_t) * MAX_ROUTER_VLAN);

        for (vidIdx = 0; vidIdx < MAX_ROUTER_VLAN; vidIdx++)
        {
            router_db[ipType].p_rt_entry[vidIdx].p_port_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);

            SYS_ERR_CHK((!router_db[ipType].p_rt_entry[vidIdx].p_port_timer), SYS_ERR_FAILED);

            osal_memset(router_db[ipType].p_rt_entry[vidIdx].p_port_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        }
    }

    for (vidIdx = 0; vidIdx < MAX_ROUTER_VLAN - 1; vidIdx++)
    {
        p_tmp_router_array[vidIdx].p_port_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);

        SYS_ERR_CHK((!p_tmp_router_array[vidIdx].p_port_timer), SYS_ERR_FAILED);

        osal_memset(p_tmp_router_array[vidIdx].p_port_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
    }

    return SYS_ERR_OK;
}

int32 mcast_router_db_add(uint16 vid, multicast_ipType_t ipType, sys_logic_portmask_t pmsk, igmp_router_port_type_t portType)
{
    uint16              i, sortedIdx;
    int32               ret;
    igmp_router_entry_t *entry = NULL;
    igmp_router_entry_t newEntry;

    mcast_vlan_entry_t *pMcastVlan = NULL;

    if (IGMP_ROUTER_PORT_DYNAMIC == portType)
    {
        mcast_vlan_db_get(vid, ipType, &pMcastVlan);
        SYS_ERR_CHK((!pMcastVlan), SYS_ERR_FAILED);
    }

    mcast_router_array_search(vid, ipType,&sortedIdx, &entry);
    if (entry)
    {
        switch(portType)
        {
            case IGMP_ROUTER_PORT_STATIC:
                osal_memcpy(&entry->router_static_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                break;

            case IGMP_ROUTER_PORT_FORBID:
                osal_memcpy(&entry->router_forbid_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                break;

            case IGMP_ROUTER_PORT_DYNAMIC:
            default:
                osal_memcpy(&entry->router_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pmsk)
                {
                    entry->p_port_timer[i] = pMcastVlan->router_port_timeout;
                }
                break;
        }
    }
    else  /* not found */
    {
        osal_memset(&newEntry, 0, sizeof(igmp_router_entry_t));
        newEntry.p_port_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!newEntry.p_port_timer), SYS_ERR_FAILED);
        newEntry.vid = vid;

        switch(portType)
        {
            case IGMP_ROUTER_PORT_STATIC:
                osal_memcpy(&newEntry.router_static_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                break;

            case IGMP_ROUTER_PORT_FORBID:
                osal_memcpy(&newEntry.router_forbid_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                break;

            case IGMP_ROUTER_PORT_DYNAMIC:
            default:
                osal_memcpy(&newEntry.router_pmsk, &pmsk, sizeof(sys_logic_portmask_t));
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pmsk)
                {
                    newEntry.p_port_timer[i] = pMcastVlan->router_port_timeout;
                }
                break;
        }
        ret = mcast_router_array_ins(sortedIdx, ipType, &newEntry);
        osal_free(newEntry.p_port_timer);
        return ret;
    }

    return SYS_ERR_OK;
}

int32 mcast_router_db_get(uint16 vid,multicast_ipType_t ipType, igmp_router_entry_t **ppEntry)
{
    uint16              sortedIdx;

    SYS_PARAM_CHK(NULL == ppEntry, SYS_ERR_NULL_POINTER);

    mcast_router_array_search(vid, ipType, &sortedIdx, ppEntry);

    return SYS_ERR_OK;
}

int32 mcast_router_db_set(igmp_router_entry_t *pEntry)
{
    uint16              sortedIdx;
    igmp_router_entry_t *entry = NULL;
    sys_logic_port_t port;

    SYS_PARAM_CHK(NULL == pEntry, SYS_ERR_NULL_POINTER);

    mcast_router_array_search(pEntry->vid, pEntry->ipType, &sortedIdx, &entry);
    if (entry != NULL)
    {
        FOR_EACH_LOGIC_PORT(port)
        {
            entry->p_port_timer[port] = pEntry->p_port_timer[port];
        }
        osal_memcpy(&entry->router_pmsk, &pEntry->router_pmsk, sizeof(sys_logic_portmask_t));
        osal_memcpy(&entry->router_static_pmsk, &pEntry->router_static_pmsk, sizeof(sys_logic_portmask_t));
        osal_memcpy(&entry->router_forbid_pmsk, &pEntry->router_forbid_pmsk, sizeof(sys_logic_portmask_t));
    }
    return SYS_ERR_OK;
}

int32 mcast_router_db_del(uint16 vid, multicast_ipType_t ipType)
{
    uint16          sortedIdx;
    igmp_router_entry_t   *entry;

    mcast_router_array_search(vid, ipType, &sortedIdx, &entry);
    if (entry)
    {
        return mcast_router_array_remove(sortedIdx, ipType);
    }

    return SYS_ERR_FAILED;
}

int32 mcast_routerPort_add_wrapper(uint16 vid, multicast_ipType_t ipType, sys_logic_portmask_t pmsk)
{
    int32 i;
    int32 port = -1;
    int32 ret;
    igmp_router_entry_t *pEntry = NULL;
    igmp_group_entry_t *pGroup = NULL;
    sys_l2McastEntry_t  l2Mcst_entry;
    sys_ipMcastEntry_t  ipMcst_entry;

    mcast_vlan_entry_t *pMcastVlan = NULL;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    osal_memset(&l2Mcst_entry, 0, sizeof(sys_l2McastEntry_t));
    osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));

    /* Find port in portmask */
    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pmsk)
    {
        break;
    }

    if(IS_LOGIC_PORT_INVALID(port))
        return SYS_ERR_PORT_ID;

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
    if (NULL == pMcastVlan)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    if (DISABLED == pMcastVlan->pimDvmrpLearn)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    if (DISABLED == pMcastVlan->enable)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_OK;
    }

    mcast_router_db_get(vid, ipType, &pEntry);
    if (pEntry)
    {
        if(IS_LOGIC_PORTMASK_PORTSET(pEntry->router_forbid_pmsk, port))
        {
            SYS_DBG(LOG_DBG_IGMP, "vid:%d port:%d is forbidden router port \n", vid, port);
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }

        if(IS_LOGIC_PORTMASK_PORTSET(pEntry->router_static_pmsk, port))
        {
            SYS_DBG(LOG_DBG_IGMP, "vid:%d port:%d is static router port \n", vid, port);
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }

        pEntry->p_port_timer[port] = pMcastVlan->router_port_timeout;
        LOGIC_PORTMASK_SET_PORT(pEntry->router_pmsk, port);
    }
    else
    {
        mcast_router_db_add(vid, ipType, pmsk,IGMP_ROUTER_PORT_DYNAMIC);
    }


    /* Add this port to all groups' ASIC mbr port of this vlan */
    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup = mcast_group_get_with_index(i);

        if ( NULL == pGroup)
            continue;

        if (pGroup->vid == vid && pGroup->ipType == ipType)
        {
            if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
                (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
            {
                l2Mcst_entry.vid = vid;
                osal_memcpy(l2Mcst_entry.mac, pGroup->mac, MAC_ADDR_LEN);

                if (SYS_ERR_OK == rsd_l2McastEntry_get(&l2Mcst_entry))
                {
                    if (!IS_LOGIC_PORTMASK_PORTSET(l2Mcst_entry.portmask, port))
                    {
                        LOGIC_PORTMASK_SET_PORT(l2Mcst_entry.portmask, port);

                        if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2Mcst_entry)))
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Adding router port failed!  vid:%d port:%d\n", vid, port);
                            IGMP_DB_SEM_UNLOCK();
                            return ret;
                        }
                        else
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Added Router Port-%d to above entry\n", port);
                        }
                    }
                }
            }
            else
            {
                if (pGroup->sip != 0)
                {
                    ipMcst_entry.vid = vid;
                    ipMcst_entry.sip = pGroup->sip;
                    ipMcst_entry.dip = pGroup->dip;

                    if (SYS_ERR_OK == rsd_ipMcastEntry_get(&ipMcst_entry))
                    {
                        if (!IS_LOGIC_PORTMASK_PORTSET(ipMcst_entry.portmask, port))
                        {
                            LOGIC_PORTMASK_SET_PORT(ipMcst_entry.portmask, port);

                            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcst_entry)))
                            {
                                SYS_DBG(LOG_DBG_IGMP, "Adding router port failed!  vid:%d port:%d\n", vid, port);
                                IGMP_DB_SEM_UNLOCK();
                                return ret;
                            }
                            else
                            {
                                SYS_DBG(LOG_DBG_IGMP, "Added Router Port-%d to above entry\n", port);
                            }
                        }
                    }
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_routerPort_get(uint16 vid, multicast_ipType_t ipType, sys_logic_portmask_t *portmask)
{
    igmp_router_entry_t *rt_enrty;

    mcast_router_db_get(vid, ipType, &rt_enrty);
    if (rt_enrty)
        osal_memcpy(portmask, &rt_enrty->router_pmsk, sizeof(sys_logic_portmask_t));
    else
    {
        osal_memset(portmask, 0, sizeof(sys_logic_portmask_t));
    }

    return SYS_ERR_OK;
}

int32 mcast_aggregate_array_search(uint64 search, uint16 *idx, igmp_aggregate_entry_t **entry)
{
    int low = 0;
    int mid;
    int high = aggregate_db.entry_num - 1;
    igmp_aggregate_entry_t *sortedArray = aggregate_db.p_aggregate_entry;

    SYS_ERR_CHK(((idx == NULL || entry == NULL)), SYS_ERR_FAILED);

    while (low <= high)
    {
        mid = (low + high) / 2;

        if (sortedArray[mid].sortKey == 0)   /* this case occurs when sorted array is empty */
        {
            *entry = NULL;
            *idx = mid;
            return SYS_ERR_OK;
        }

        if (sortedArray[mid].sortKey == search)
        {
            *entry = &sortedArray[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        else if (sortedArray[mid].sortKey > search)
        {
            high = mid - 1;
        }
        else if (sortedArray[mid].sortKey < search)
        {
            low = mid + 1;
        }
    }

    *entry = NULL;
    *idx = low;
    return SYS_ERR_OK;
}

int32 mcast_aggregate_array_ins(uint16 sortedIdx, igmp_aggregate_entry_t *entry)
{
    uint16 idx, offset;
    igmp_aggregate_entry_t *ptr = NULL, *ptr_tmp = NULL;

    SYS_ERR_CHK((!entry), SYS_ERR_FAILED);
    SYS_ERR_CHK((sortedIdx > aggregate_db.entry_num), SYS_ERR_FAILED);
    SYS_ERR_CHK((aggregate_db.entry_num >= SYS_MCAST_MAX_GROUP_NUM), SYS_ERR_FAILED);

    /* Move data */
    if (sortedIdx ==  aggregate_db.entry_num)
    {
        //osal_memcpy(aggregate_db.p_aggregate_entry + sortedIdx, entry, sizeof(igmp_aggregate_entry_t));
        ptr = aggregate_db.p_aggregate_entry + sortedIdx;
        ptr->group_ref_cnt = entry->group_ref_cnt;
        ptr->sortKey = entry->sortKey;
        osal_memcpy(ptr->p_port_ref_cnt, entry->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);
    }
    else
    {
        //osal_memcpy(p_tmp_aggr_array, aggregate_db.p_aggregate_entry + sortedIdx, sizeof(igmp_aggregate_entry_t) * (aggregate_db.entry_num - sortedIdx));
        //osal_memcpy(aggregate_db.p_aggregate_entry + sortedIdx, entry, sizeof(igmp_aggregate_entry_t));
        //osal_memcpy(aggregate_db.p_aggregate_entry + sortedIdx + 1, p_tmp_aggr_array, sizeof(igmp_aggregate_entry_t) * (aggregate_db.entry_num - sortedIdx));

        offset = 0;
        for(idx = sortedIdx; idx < aggregate_db.entry_num; idx++)
        {
            //src
            ptr = aggregate_db.p_aggregate_entry + idx;
            //dst
            ptr_tmp = p_tmp_aggr_array + offset;
            ptr_tmp->group_ref_cnt = ptr->group_ref_cnt;
            ptr_tmp->sortKey = ptr->sortKey;
            osal_memcpy(ptr_tmp->p_port_ref_cnt, ptr->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            offset++;
        }

        ptr = aggregate_db.p_aggregate_entry + sortedIdx;
        ptr->group_ref_cnt = entry->group_ref_cnt;
        ptr->sortKey = entry->sortKey;
        osal_memcpy(ptr->p_port_ref_cnt, entry->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);

        offset = 0;
        for(idx = sortedIdx; idx < aggregate_db.entry_num; idx++)
        {
            //src
            ptr_tmp = p_tmp_aggr_array + offset;
            //dst
            ptr = aggregate_db.p_aggregate_entry + idx + 1;
            ptr->group_ref_cnt = ptr_tmp->group_ref_cnt;
            ptr->sortKey = ptr_tmp->sortKey;
            osal_memcpy(ptr->p_port_ref_cnt, ptr_tmp->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            offset++;
        }
    }
    aggregate_db.entry_num++;

    return SYS_ERR_OK;
}

int32 mcast_aggregate_array_remove(uint16 sortedIdx)
{
    uint16 idx, offset;
    igmp_aggregate_entry_t *ptr = NULL, *ptr_tmp = NULL;

    SYS_ERR_CHK((sortedIdx >= aggregate_db.entry_num), SYS_ERR_FAILED);

    if (aggregate_db.entry_num - sortedIdx > 1)
    {
        //osal_memcpy(p_tmp_aggr_array, aggregate_db.p_aggregate_entry + sortedIdx + 1, sizeof(igmp_aggregate_entry_t) * (aggregate_db.entry_num - sortedIdx - 1));
        //osal_memcpy(aggregate_db.p_aggregate_entry + sortedIdx, p_tmp_aggr_array, sizeof(igmp_aggregate_entry_t) * (aggregate_db.entry_num - sortedIdx - 1));

        offset = 0;
        for(idx = sortedIdx + 1; idx < aggregate_db.entry_num; idx++)
        {
            //src
            ptr = aggregate_db.p_aggregate_entry + idx;
            //dst
            ptr_tmp = p_tmp_aggr_array + offset;
            ptr_tmp->group_ref_cnt = ptr->group_ref_cnt;
            ptr_tmp->sortKey = ptr->sortKey;
            osal_memcpy(ptr_tmp->p_port_ref_cnt, ptr->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            offset++;
        }


        offset = 0;
        for(idx = sortedIdx + 1; idx < aggregate_db.entry_num; idx++)
        {
            //src
            ptr_tmp = p_tmp_aggr_array + offset;
            //dst
            ptr = aggregate_db.p_aggregate_entry + idx - 1;
            ptr->group_ref_cnt = ptr_tmp->group_ref_cnt;
            ptr->sortKey = ptr_tmp->sortKey;
            osal_memcpy(ptr->p_port_ref_cnt, ptr_tmp->p_port_ref_cnt, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            offset++;
        }
    }
    aggregate_db.entry_num--;

    //osal_memset(aggregate_db.p_aggregate_entry + aggregate_db.entry_num, 0, sizeof(igmp_aggregate_entry_t));

    ptr = aggregate_db.p_aggregate_entry + aggregate_db.entry_num;
    ptr->group_ref_cnt = 0;
    ptr->sortKey = 0;
    osal_memset(ptr->p_port_ref_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);

    return SYS_ERR_OK;
}

int32 mcast_aggregate_db_init(void)
{
    uint32 grpIdx;
    osal_memset(&aggregate_db, 0, sizeof(igmp_aggregate_db_t));

    aggregate_db.p_aggregate_entry = (igmp_aggregate_entry_t *)osal_alloc(sizeof(igmp_aggregate_entry_t) * SYS_MCAST_MAX_GROUP_NUM);

    SYS_ERR_CHK((!aggregate_db.p_aggregate_entry), SYS_ERR_FAILED);

    osal_memset(aggregate_db.p_aggregate_entry, 0, sizeof(igmp_aggregate_entry_t) * SYS_MCAST_MAX_GROUP_NUM);

    for (grpIdx = 0; grpIdx < SYS_MCAST_MAX_GROUP_NUM; grpIdx++)
    {
        aggregate_db.p_aggregate_entry[grpIdx].p_port_ref_cnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);

        SYS_ERR_CHK((!aggregate_db.p_aggregate_entry[grpIdx].p_port_ref_cnt), SYS_ERR_FAILED);

        osal_memset(aggregate_db.p_aggregate_entry[grpIdx].p_port_ref_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
    }

    p_tmp_aggr_array = (igmp_aggregate_entry_t *)osal_alloc(sizeof(igmp_aggregate_entry_t) * (SYS_MCAST_MAX_GROUP_NUM - 1));

    SYS_ERR_CHK((!p_tmp_aggr_array), SYS_ERR_FAILED);

    osal_memset(p_tmp_aggr_array, 0, sizeof(igmp_aggregate_entry_t) * (SYS_MCAST_MAX_GROUP_NUM - 1));


    for (grpIdx = 0; grpIdx < SYS_MCAST_MAX_GROUP_NUM - 1; grpIdx++)
    {
        p_tmp_aggr_array[grpIdx].p_port_ref_cnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);

        SYS_ERR_CHK((!p_tmp_aggr_array[grpIdx].p_port_ref_cnt), SYS_ERR_FAILED);

        osal_memset(p_tmp_aggr_array[grpIdx].p_port_ref_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
    }

    return SYS_ERR_OK;
}

int32 mcast_aggregate_db_add(igmp_group_entry_t *pGroup, sys_logic_port_t port)
{
    uint16                  sortedIdx;
    igmp_aggregate_entry_t  *entry = NULL;
    igmp_aggregate_entry_t  newEntry;
    uint64 sortKey;
    int32 ret;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);

    sortKey = mcast_aggregate_sortKey_ret(pGroup->ipType, pGroup->dip, pGroup->vid);

    SYS_DBG(LOG_DBG_IGMP, "%s()@%d: sortKey=%llu ipType=%u dip="IPADDR_PRINT"vid=%u\n",
            __FUNCTION__, __LINE__, sortKey, pGroup->ipType, IPADDR_PRINT_ARG(pGroup->dip), pGroup->vid);

    SYS_ERR_CHK((sortKey == 0), SYS_ERR_FAILED);

    mcast_aggregate_array_search(sortKey, &sortedIdx, &entry);

    if (NULL == entry)
    {
        osal_memset(&newEntry, 0, sizeof(igmp_aggregate_entry_t));
        newEntry.p_port_ref_cnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!newEntry.p_port_ref_cnt), SYS_ERR_FAILED);
        newEntry.sortKey = sortKey;
        newEntry.p_port_ref_cnt[port] = 1;
        newEntry.group_ref_cnt = 1;
        ret = mcast_aggregate_array_ins(sortedIdx, &newEntry);
        osal_free(newEntry.p_port_ref_cnt);

        SYS_DBG(LOG_DBG_IGMP, "%s()@%d: sortKey=%llu sortedIdx=%u, aggrEntry=%p\n",
                __FUNCTION__, __LINE__, sortKey, sortedIdx, &newEntry);
        return ret;
    }

    return SYS_ERR_FAILED;
}

int32 mcast_aggregate_db_get(igmp_group_entry_t *pGroup, igmp_aggregate_entry_t **ppEntry)
{
    uint16              sortedIdx;

    SYS_PARAM_CHK(NULL == ppEntry, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);

   // sortKey = mcast_aggregate_sortKey_ret(pGroup->ipType, pGroup->dip, pGroup->vid);
    mcast_aggregate_array_search(mcast_aggregate_sortKey_ret(pGroup->ipType, pGroup->dip, pGroup->vid), &sortedIdx, ppEntry);

    SYS_DBG(LOG_DBG_IGMP, "%s()@%d: sortedIdx=%u, aggrEntry=%p\n",
            __FUNCTION__, __LINE__, sortedIdx, *ppEntry);

    return SYS_ERR_OK;
}

int32 mcast_aggregate_db_del(igmp_group_entry_t *pGroup)
{
    uint16                  sortedIdx;
    igmp_aggregate_entry_t  *entry;
    uint64                  sortKey;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);

    sortKey =  mcast_aggregate_sortKey_ret(pGroup->ipType, pGroup->dip, pGroup->vid);

    mcast_aggregate_array_search(sortKey, &sortedIdx, &entry);

    if (entry)
    {
        return mcast_aggregate_array_remove(sortedIdx);
    }

    return SYS_ERR_FAILED;
}

int32 mcast_aggregatePort_get(igmp_group_entry_t *pGroup, sys_logic_portmask_t *portmask)
{
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    sys_logic_port_t port;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(NULL == portmask, SYS_ERR_NULL_POINTER);

    LOGIC_PORTMASK_CLEAR_ALL(*portmask);

    mcast_aggregate_db_get(pGroup, &pAggrEntry);

    if (NULL != pAggrEntry)
    {
        FOR_EACH_LOGIC_PORT(port)
        {
            if (0 != pAggrEntry->p_port_ref_cnt[port])
            {
                LOGIC_PORTMASK_SET_PORT(*portmask, port);
            }
        }
    }
    else
    {
        SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
            "%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDR_PRINT"\n",
            __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pGroup->dip));
        return SYS_ERR_FAILED;
    }

    return SYS_ERR_OK;
}

int32 mcast_group_db_init(void)
{
    int i, j;
    igmp_group_entry_t  *iter;
    sys_logic_port_t port;

    pp_group_sorted_array = (igmp_group_entry_t **)osal_alloc(sizeof(igmp_group_entry_t *) * SYS_MCAST_MAX_GROUP_NUM);
    SYS_ERR_CHK((!pp_group_sorted_array), SYS_ERR_FAILED);

    pp_tmp_group_sorted_array = (igmp_group_entry_t **)osal_alloc(sizeof(igmp_group_entry_t *) * (SYS_MCAST_MAX_GROUP_NUM - 1));
    SYS_ERR_CHK((!pp_tmp_group_sorted_array), SYS_ERR_FAILED);

    p_group_db = (igmp_group_entry_t *)osal_alloc(sizeof(igmp_group_entry_t) * SYS_MCAST_MAX_GROUP_NUM);
    SYS_ERR_CHK((!p_group_db), SYS_ERR_FAILED);

    for (i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        osal_memset(&p_group_db[i], 0, sizeof(igmp_group_entry_t));

        p_group_db[i].p_mbr_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_db[i].p_mbr_timer), SYS_ERR_FAILED);
        osal_memset(p_group_db[i].p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);

        p_group_db[i].p_gsQueryCnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_db[i].p_gsQueryCnt), SYS_ERR_FAILED);
        osal_memset(p_group_db[i].p_gsQueryCnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);

        p_group_db[i].v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_db[i].v3.p_mbr_fwd_flag), SYS_ERR_FAILED);
        osal_memset(p_group_db[i].v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

        p_group_db[i].v3.p_gss_query_cnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_db[i].v3.p_gss_query_cnt), SYS_ERR_FAILED);
        osal_memset(p_group_db[i].v3.p_gss_query_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);

        p_group_db[i].p_hostListHead = (struct hostHead *)osal_alloc(sizeof(struct hostHead) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_db[i].p_hostListHead), SYS_ERR_FAILED);

		FOR_EACH_LOGIC_PORT(j)
        {
        	LIST_INIT(&p_group_db[i].p_hostListHead[j]);
        }
    }

    igmpGroupFreeHead.freeListNumber = SYS_MCAST_MAX_GROUP_NUM;
    iter = igmpGroupFreeHead.item = &p_group_db[0];

    for (i = 1; i < SYS_MCAST_MAX_GROUP_NUM; iter = iter->next_free, i++)
    {
        iter->next_free = &p_group_db[i];
    }
    iter->next_free = NULL;

    group_sortedAry_entry_num = 0;
    igmp_group_num = 0;
    mld_group_num = 0;
    mcast_group_num = 0;

    if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
    {
        if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
            SYS_PRINTF("TBD \n");//TBD: SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, FRAME_TRAP_ACTION_TRAP2CPU));
    }
    else
    {
        if (igmp_trapDataPkt_lock)
		{
            LgcPortFor(port)
        	{
        	    /*All LAN ports*/
        	    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(ENABLED, port);
        	}
        }
    }

    igmp_hw_entry_num = 0;
    igmp_trapDataPkt_lock = FALSE;

    return SYS_ERR_OK;

}

int32 mcast_group_sortedArray_search(uint64 search, uint16 *idx, igmp_group_entry_t **groupHead)
{
    int low = 0;
    int mid;
    int high = group_sortedAry_entry_num - 1;

    SYS_PARAM_CHK(NULL == idx, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(NULL == groupHead, SYS_ERR_NULL_POINTER);

    while (low <= high)
    {
        mid = (low + high) / 2;

        if (pp_group_sorted_array[mid] == NULL)   /* this case occurs when sorted array is empty */
        {
            *groupHead = NULL;
            *idx = mid;
            return SYS_ERR_OK;
        }

        if (pp_group_sorted_array[mid]->sortKey == search)
        {
            *groupHead = pp_group_sorted_array[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        /*For Group-specify query without source IP list, ignore source IP compare*/
        else if((IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version) && ((pp_group_sorted_array[mid]->sortKey | 0xffffffff) == search))
        {
            *groupHead = pp_group_sorted_array[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        else if (pp_group_sorted_array[mid]->sortKey > search)
        {
            high = mid - 1;
        }
        else if (pp_group_sorted_array[mid]->sortKey < search)
        {
            low = mid + 1;
        }
    }

    *groupHead = NULL;
    *idx = low;
    return SYS_ERR_OK;

}

int32 mcast_group_sortedArray_ins(uint16 sortedIdx, igmp_group_entry_t *entry)
{
    SYS_ERR_CHK((!entry), SYS_ERR_FAILED);

    SYS_ERR_CHK((sortedIdx > group_sortedAry_entry_num), SYS_ERR_FAILED);

    /* Move data */
    if (group_sortedAry_entry_num == sortedIdx)
    {
        pp_group_sorted_array[sortedIdx] = entry;
    }
    else
    {
        osal_memcpy(pp_tmp_group_sorted_array, pp_group_sorted_array + sortedIdx,
                    sizeof(igmp_group_entry_t *) * (group_sortedAry_entry_num - sortedIdx));
        pp_group_sorted_array[sortedIdx] = entry;

        osal_memcpy(pp_group_sorted_array + sortedIdx + 1, pp_tmp_group_sorted_array,
                    sizeof(igmp_group_entry_t *) * (group_sortedAry_entry_num - sortedIdx));
    }
    group_sortedAry_entry_num++;

	SYS_DBG(LOG_DBG_IGMP, "%s() group fmode=%x, sortKey=%llu\n",
        __FUNCTION__,  *((pp_group_sorted_array[sortedIdx])->fmode.bits), (pp_group_sorted_array[sortedIdx])->sortKey);

    return SYS_ERR_OK;
}

int32 mcast_group_sortedArray_del(uint16 sortedIdx)
{
    SYS_ERR_CHK((sortedIdx >= group_sortedAry_entry_num), SYS_ERR_FAILED);

    if (group_sortedAry_entry_num - sortedIdx  > 1)
    {
        osal_memcpy(pp_tmp_group_sorted_array, pp_group_sorted_array + sortedIdx + 1,
            sizeof(igmp_group_entry_t*) * (group_sortedAry_entry_num - sortedIdx - 1));

        osal_memcpy(pp_group_sorted_array + sortedIdx, pp_tmp_group_sorted_array,
            sizeof(igmp_group_entry_t*) * (group_sortedAry_entry_num - sortedIdx - 1));
    }
    group_sortedAry_entry_num--;
    pp_group_sorted_array[group_sortedAry_entry_num] = NULL;

    return SYS_ERR_OK;
}

igmp_group_entry_t* mcast_group_freeEntry_get(multicast_ipType_t ipType)
{
    igmp_group_entry_t *entry;

    if (igmpGroupFreeHead.freeListNumber == 0)
    {
        return NULL;
    }

    if (mcast_group_num >= SYS_MCAST_MAX_GROUP_NUM)
        return NULL;

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        if (igmp_group_num >= SYS_IGMP_MAX_GROUP_NUM)
        {
            return NULL;
        }
    }
    else
    {
        if (mld_group_num >= SYS_MLD_MAX_GROUP_NUM)
        {
            return NULL;
        }
    }

    entry = igmpGroupFreeHead.item;
    if (NULL == entry)
        return NULL;

    igmpGroupFreeHead.item = igmpGroupFreeHead.item->next_free;
    igmpGroupFreeHead.freeListNumber--;
    return entry;
}

int32 mcast_group_entry_release(igmp_group_entry_t *entry)
{
//  uint32 freeCount;
//  multicast_ipType_t ipType;
    SYS_ERR_CHK((!entry), SYS_ERR_FAILED);
//  ipType = entry->ipType;
#if 1
    uint32 portId;
    entry->ipType = 0;
    entry->dip = 0;
    entry->ipType = 0;
    osal_memset(&(entry->dipv6), 0, sizeof(ipv6_addr_t));
    osal_memset(&(entry->sipv6), 0, sizeof(ipv6_addr_t));
    entry->vid = 0;
    osal_memset(&(entry->mac), 0, MAC_ADDR_LEN);
    entry->sortKey = 0;
    osal_memset(&(entry->mbr), 0, sizeof(sys_logic_portmask_t));
    osal_memset(&(entry->fmode), 0, sizeof(sys_logic_portmask_t));
    osal_memset(entry->p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
    entry->groupTimer = 0;
    osal_memset(entry->p_gsQueryCnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
    entry->lookupTableFlag = 0;
    entry->suppreFlag = 0;
    osal_memset(&(entry->v3.isGssQueryFlag), 0, sizeof(sys_logic_portmask_t));
    osal_memset(entry->v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
    osal_memset(entry->v3.p_gss_query_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
    entry->v3.pGroupHead = NULL;
    entry->form = 0;
    osal_memset(&(entry->staticMbr), 0, sizeof(sys_logic_portmask_t));
    osal_memset(&(entry->staticInMbr), 0, sizeof(sys_logic_portmask_t));
    entry->next_subgroup = NULL;
    entry->next_free = igmpGroupFreeHead.item;
    entry->imputedGrpBw = 0;
    entry->care_vid = 0;
    FOR_EACH_LOGIC_PORT(portId)
    {
    	LIST_INIT(&(entry->p_hostListHead[portId]));
    }

    igmpGroupFreeHead.item = entry;
    igmpGroupFreeHead.freeListNumber++;

#else
    osal_memset(entry, 0, sizeof(igmp_group_entry_t));
    entry->next_subgroup = NULL;
    entry->next_free = igmpGroupFreeHead.item;
    igmpGroupFreeHead.item = entry;
    igmpGroupFreeHead.freeListNumber++;
#endif

#if 0
    if (igmp_trapDataPkt_lock)
    {
        rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);
        if (igmpGroupFreeHead.freeListNumber >= 1 && freeCount >= 1)
        {
            igmp_trapDataPkt_lock = FALSE;

            if(IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                 {
                    SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(ipType, DISABLED));
                    //SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, ACTION_TRAP2CPU));
                 }
            }
            else
            {
                SYS_ERR_CHK(rsd_igmp_ipMcastPkt2CpuEnable_set(ENABLED));
            }
        }
    }
#endif

    return SYS_ERR_OK;
}

int32 mcast_build_groupEntryKey(sys_vid_t vid, multicast_ipType_t ipType, uint32 dip, uint8 *ipv6Addr, igmp_group_entry_t *pGroup)
{
    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK((0 == dip), SYS_ERR_IPV4_ADDR);

    osal_memset(pGroup, 0, sizeof(igmp_group_entry_t));
    pGroup->vid = vid;
    pGroup->ipType = ipType;
    pGroup->dip = dip;

    if (MULTICAST_TYPE_IPV4 != ipType)
    {
        SYS_PARAM_CHK(NULL == ipv6Addr, SYS_ERR_NULL_POINTER);
        osal_memcpy(pGroup->dipv6.addr, ipv6Addr, IPV6_ADDR_LEN);
    }

    return SYS_ERR_OK;
}

uint64 mcast_group_sortKey_ret(igmp_group_entry_t *pGroup)
{
    uint64  ret = 0;

    if (!pGroup)
        return ret;

    if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
    {
		if (MODE_GPON == igmpCtrl.ponMode)
		{
			if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
			{
				ret = (((uint64)((pGroup->dip) & 0xffffffff)) << 32) | (((uint64)(pGroup->sip) & 0xffffffff));
			}
			else
			{
				ret = ((uint64)((pGroup->dip) & 0xffffffff) << 12)  | ((uint64)(pGroup->vid & 0xfff));
			}
		}
		else
		{
			ret = ((uint64)((pGroup->dip) & 0xffffffff) << 12)  | ((uint64)(pGroup->vid & 0xfff));
		}
        //ret = (((uint64)(pGroup->ipType & 0x0f) << 52) | ((uint64)((pGroup->dip) & 0xffffffff) << 12)  | ((uint64)(pGroup->vid & 0xfff)) ) ;
    }
    else
    {
        ret = (((uint64)(pGroup->dip) & 0xffffffff) << 12)  | (pGroup->vid & 0xfff);
        ret = ((uint64)((pGroup->dipv6.addr[1]) & 0xff) << 44) | ret;
        ret = ((uint64)0x01 << 52) | ret;
//      ret = (((uint64)(pGroup->ipType & 0x0f) << 52) | ((uint64)((pGroup->dipv6.addr[1]) & 0xff) << 44)  | ( ((uint64)(pGroup->dip) & 0xffffffff) << 12)  | (pGroup->vid & 0xfff) ) ;
    }
    return ret;
}

uint64 mcast_aggregate_sortKey_ret(multicast_ipType_t ipType, uint32 dip, sys_vid_t vid)
{
    uint64  ret = 0;
    uint32  groupIp = 0;

    if (ipType < MULTICAST_TYPE_IPV4 ||ipType >= MULTICAST_TYPE_END)
        return ret;

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        groupIp =  (dip & 0x7fffff) | 0x5e000000;
        ret = ((uint64)(groupIp & 0xffffffff) << 12) | ((uint64)(vid & 0xfff));
        ret = ((uint64)0x0100 << 44)  | ret;
        //ret = (((uint64)(ipType & 0xf) << 60)  | ((uint64)0x0100 << 44) | ((uint64)(groupIp & 0xffffffff) << 12) | ((uint64)(vid & 0xfff)));
    }
    else
    {
        ret = ((uint64)(dip & 0xffffffff) << 12) | ((uint64)(vid & 0xfff));
        ret = ((uint64)0x3333 << 44) | ret;
        ret = ((uint64)0x01 << 60) | ret;
        //ret = (((uint64)(ipType & 0xf) << 60)  | ((uint64)0x3333 << 44) | ((uint64)(dip & 0xffffffff) << 12) | ((uint64)(vid & 0xfff)));
    }
   return ret;
}

int32 mcast_group_add(igmp_group_entry_t *pNewEntry, ipAddr_t clientIp, uint16 portId, uint32 timer)
{
    uint16                  i, sortedIdx, vid;
    uint32                  dip, sip;
    igmp_group_entry_t      *tmpEntry, *subgroup, *entry, *groupHead;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    multicast_ipType_t      ipType;
    sys_ipMcastEntry_t      ipMcstEntry;
	sys_logic_portmask_t    rtPmsk;

    SYS_PARAM_CHK(NULL == pNewEntry, SYS_ERR_NULL_POINTER);

    vid = pNewEntry->vid;
    dip = pNewEntry->dip;
    sip = pNewEntry->sip;
    ipType = pNewEntry->ipType;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    SYS_ERR_CHK( (!pMcastVlan), SYS_ERR_FAILED);
	}
    osal_memset(&ipMcstEntry, 0, sizeof(ipMcstEntry));

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pNewEntry), &sortedIdx, &groupHead);

    /* further, compare dip + sip*/
    if (groupHead)
    {
    	SYS_DBG(LOG_DBG_IGMP, "%s() %d exist groupHead \n", __FUNCTION__, __LINE__);

        if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            osal_memcpy(groupHead->mac, pNewEntry->mac, MAC_ADDR_LEN);
            osal_memcpy(&groupHead->mbr, &pNewEntry->mbr, sizeof(sys_logic_portmask_t));
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pNewEntry->mbr)
            {
            	if (MODE_GPON != igmpCtrl.ponMode)
					groupHead->p_mbr_timer[i] = pMcastVlan->grpMembershipIntv;
				else
					groupHead->p_mbr_timer[i] = timer;
            }
        }
        else
        {
            mcast_igmp_subgroup_sip_search(groupHead, sip, &subgroup, NULL);
            if (subgroup == NULL)
            {
                SYS_DBG(LOG_DBG_IGMP, "subgroup NOT existed! i.e. the same group with different group_src\n");
                entry = mcast_group_freeEntry_get(ipType);
                if (entry != NULL)
                {
                    entry->dip = dip;
                    entry->sip = sip;
                    entry->vid = vid;
                    entry->sortKey = mcast_group_sortKey_ret(pNewEntry);
                    entry->groupTimer = pNewEntry->groupTimer;
                    osal_memcpy(entry->mac, pNewEntry->mac, MAC_ADDR_LEN);
                    osal_memcpy(&entry->mbr, &pNewEntry->mbr, sizeof(sys_logic_portmask_t));

                    entry->form = pNewEntry->form;
                    entry->lookupTableFlag = pNewEntry->lookupTableFlag;
                    osal_memcpy(&entry->staticMbr, &pNewEntry->staticMbr,sizeof(sys_logic_portmask_t));
                    osal_memcpy(&entry->staticInMbr, &pNewEntry->staticInMbr,sizeof(sys_logic_portmask_t));

                    FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pNewEntry->mbr)
                    {
                        switch(pNewEntry->form)
                        {
                        	case IGMP_GROUP_ENTRY_DYNAMIC:
							case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
							case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
								if (MODE_GPON != igmpCtrl.ponMode)
									entry->p_mbr_timer[i] = pMcastVlan->grpMembershipIntv;
								else
									entry->p_mbr_timer[i] = timer;//DFLT_IGMP_GROUP_MEMBERSHIP_INTV
								break;
							default:
								SYS_DBG(LOG_DBG_IGMP, "Not support group type\n");
                        }

                        /*Add support igmpv3*/
                        entry->v3.p_mbr_fwd_flag[i] = pNewEntry->v3.p_mbr_fwd_flag[i] ;
                    }
					/* add asic since the same group but new group_src */
					if (MODE_GPON == igmpCtrl.ponMode)
					{
						ipMcstEntry.vid = entry->vid;
						ipMcstEntry.sip = entry->sip;
						ipMcstEntry.dip = entry->dip;
						LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, entry->mbr);

						LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, ipMcstEntry.portmask);
						FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, ipMcstEntry.portmask)
						{
							//update group filter mode in groupHead when group filter mode is include
							if (IS_LOGIC_PORTMASK_CLEAR(groupHead->fmode))
							{
								if(IGMPV3_FWD_NOT == entry->v3.p_mbr_fwd_flag[i])
								{
									LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, i);
									LOGIC_PORTMASK_SET_PORT(groupHead->fmode, i);
									SYS_DBG(LOG_DBG_IGMP, "change group filter mode to exclude %02x \n", *(groupHead->fmode.bits));
								}
								else if(IGMPV3_FWD_CONFLICT == entry->v3.p_mbr_fwd_flag[i])
								{
									LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, i);
								}
							}
						}

						//new group_src need to add l2-table
						if (SYS_ERR_OK != rsd_ipMcastEntry_add(&ipMcstEntry))
						{
							SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
							return SYS_ERR_FAILED;
						}
					}
					SYS_DBG(LOG_DBG_IGMP, "%s() %d fmode=%02x \n", __FUNCTION__, __LINE__, *(groupHead->fmode.bits));

                    //mcast_group_num++ only for VID_MAC mode;
                    mcast_group_num_cal(ipType,TRUE);
                    tmpEntry = pp_group_sorted_array[sortedIdx]->next_subgroup;
                    pp_group_sorted_array[sortedIdx]->next_subgroup = entry;
                    entry->next_subgroup = tmpEntry;
                    SYS_DBG(LOG_DBG_IGMP, "subgroup add  successfull !\n");

					switch(pNewEntry->form)
					{
						case IGMP_GROUP_ENTRY_DYNAMIC:
						case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
						case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
							if(!mcast_host_tracking_find(portId, groupHead, clientIp))
							{
								SYS_DBG(LOG_DBG_IGMP, "can not find clinet, so insert it !\n");
								//insert clientIp to clientIp List
								if(SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, portId))
								{
									SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
								}
							}
							break;
						default:
							SYS_DBG(LOG_DBG_IGMP, "Not support group type\n");
					}
					SYS_DBG(LOG_DBG_IGMP, "%s() %d fmode=%02x \n", __FUNCTION__, __LINE__, *(groupHead->fmode.bits));
                    return SYS_ERR_OK;
                }
                else
                {
                    /* no free entry  */
                    return SYS_ERR_MCAST_DATABASE_FULL;
                }
            }
            else
            {
                SYS_DBG(LOG_DBG_IGMP, "subgroup has existed! i.e. the same group with the same group_src\n");
                /* subgroup has existed */
                osal_memcpy(&subgroup->mbr, &pNewEntry->mbr, sizeof(sys_logic_portmask_t));
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pNewEntry->mbr)
                {
                	if (MODE_GPON != igmpCtrl.ponMode)
                    	subgroup->p_mbr_timer[i] = pMcastVlan->grpMembershipIntv;
                    else
						subgroup->p_mbr_timer[i] = timer;//DFLT_IGMP_GROUP_MEMBERSHIP_INTV
                }

				/* update asic since the same group_dip & group_src */
				if (MODE_GPON ==igmpCtrl.ponMode)
				{
					ipMcstEntry.vid = subgroup->vid;
					ipMcstEntry.sip = subgroup->sip;
					ipMcstEntry.dip = subgroup->dip;
					ipMcstEntry.isGrpRefB = mcast_group_ref_get(subgroup);
					LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, subgroup->mbr);

					LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, ipMcstEntry.portmask);
					FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, ipMcstEntry.portmask)
					{
						//update group filter mode in groupHead when group filter mode is include
						if (IS_LOGIC_PORTMASK_CLEAR(groupHead->fmode))
						{
							if(IGMPV3_FWD_NOT == subgroup->v3.p_mbr_fwd_flag[i])
							{
								LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, i);
								LOGIC_PORTMASK_SET_PORT(groupHead->fmode, i);
								SYS_DBG(LOG_DBG_IGMP, "change group filter mode to exclude %02x \n", *(groupHead->fmode.bits));
							}
							else if(IGMPV3_FWD_CONFLICT == subgroup->v3.p_mbr_fwd_flag[i])
							{
								LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, i);
							}
						}
					}

					//update asic
					if (SYS_ERR_OK != rsd_ipMcastEntry_set(&ipMcstEntry))
					{
						SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
						return SYS_ERR_FAILED;
					}
				}
				switch(pNewEntry->form)
				{
					case IGMP_GROUP_ENTRY_DYNAMIC:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
						if(!mcast_host_tracking_find(portId, groupHead, clientIp))
						{
							//insert clientIp to clientIp List
							if(SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, portId))
							{
								SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
							}
						}
						break;
					default:
						SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
				}

            }
        }
    }
    else    /* new group, insert into sorted array */
    {
    	SYS_DBG(LOG_DBG_IGMP, "%s() %d new group NOT exit groupHead \n", __FUNCTION__, __LINE__);
        entry = mcast_group_freeEntry_get(ipType);
        if (entry != NULL)
        {
            #if 0
            osal_memset(entry, 0, sizeof(igmp_group_entry_t));
            #else
            entry->ipType = 0;
            entry->dip = 0;
            entry->ipType = 0;
            osal_memset(&(entry->dipv6), 0, sizeof(ipv6_addr_t));
            osal_memset(&(entry->sipv6), 0, sizeof(ipv6_addr_t));
            entry->vid = 0;
            osal_memset(&(entry->mac), 0, MAC_ADDR_LEN);
            entry->sortKey = 0;
            osal_memset(&(entry->mbr), 0, sizeof(sys_logic_portmask_t));
            osal_memset(&(entry->fmode), 0, sizeof(sys_logic_portmask_t));
            osal_memset(entry->p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
            entry->groupTimer = 0;
            osal_memset(entry->p_gsQueryCnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            entry->lookupTableFlag = 0;
            entry->suppreFlag = 0;
            osal_memset(&(entry->v3.isGssQueryFlag), 0, sizeof(sys_logic_portmask_t));
            osal_memset(entry->v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
            osal_memset(entry->v3.p_gss_query_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
            entry->v3.pGroupHead = NULL;
            entry->form = 0;
            osal_memset(&(entry->staticMbr), 0, sizeof(sys_logic_portmask_t));
            osal_memset(&(entry->staticInMbr), 0, sizeof(sys_logic_portmask_t));
            entry->next_subgroup = NULL;
            //entry->next_free = igmpGroupFreeHead.item;
            entry->imputedGrpBw = 0;
            entry->care_vid = 0;
            FOR_EACH_LOGIC_PORT(portId)
            {
                LIST_INIT(&(entry->p_hostListHead[portId]));
            }
            #endif
            entry->dip = dip;
            entry->sip = sip;
            entry->vid = vid;
            entry->sortKey = mcast_group_sortKey_ret(pNewEntry);

            entry->form = pNewEntry->form;
            entry->lookupTableFlag = pNewEntry->lookupTableFlag;
            osal_memcpy(&entry->staticMbr, &pNewEntry->staticMbr,sizeof(sys_logic_portmask_t));
            osal_memcpy(&entry->staticInMbr, &pNewEntry->staticInMbr,sizeof(sys_logic_portmask_t));

            osal_memcpy(entry->mac, pNewEntry->mac, MAC_ADDR_LEN);
            osal_memcpy(entry->dipv6.addr, pNewEntry->dipv6.addr, IPV6_ADDR_LEN);
            osal_memcpy(&entry->mbr, &pNewEntry->mbr, sizeof(sys_logic_portmask_t));

            FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, pNewEntry->mbr)
            {
            	if (MODE_GPON != igmpCtrl.ponMode)
                	entry->p_mbr_timer[i] = pMcastVlan->grpMembershipIntv;
                else
					entry->p_mbr_timer[i] = timer;//DFLT_IGMP_GROUP_MEMBERSHIP_INTV
                entry->v3.p_mbr_fwd_flag[i] = pNewEntry->v3.p_mbr_fwd_flag[i] ;
            }
            if(MODE_GPON == igmpCtrl.ponMode)
            {
	            ipMcstEntry.vid = entry->vid;
	            ipMcstEntry.sip = entry->sip;
	            ipMcstEntry.dip = entry->dip;
	        	LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, entry->mbr);

	            /* Also add router portmask */
	            mcast_routerPort_get(vid, ipType, &rtPmsk);
	            LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, rtPmsk);

	            FOR_EACH_PORT_IN_LOGIC_PORTMASK(i, ipMcstEntry.portmask)
	            {
					if(IGMPV3_FWD_NOT == entry->v3.p_mbr_fwd_flag[i])
					{
						LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, i);
						// update group filter mode to software
						LOGIC_PORTMASK_SET_PORT(entry->fmode, i);
					}
					else if(IGMPV3_FWD_CONFLICT == entry->v3.p_mbr_fwd_flag[i])
					{
						LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, i);
						// update group filter mode to software
						LOGIC_PORTMASK_CLEAR_PORT(entry->fmode, i);
					}
	            }

	            if (SYS_ERR_OK != rsd_ipMcastEntry_add(&ipMcstEntry))
	            {
	                SYS_DBG(LOG_DBG_IGMP, "Failed from ASIC!  \n");
	                return SYS_ERR_FAILED;
	            }
            }
           // mcast_group_num++;
            mcast_group_num_cal(ipType,TRUE);

            SYS_DBG(LOG_DBG_IGMP, "%s() %d sip="IPADDR_PRINT"\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(entry->sip));
            mcast_group_sortedArray_ins(sortedIdx, entry);

			mcast_group_sortedArray_search(entry->sortKey, &sortedIdx, &groupHead);

			//update group filter mode in groupHead when group filter mode is include
			LOGIC_PORTMASK_COPY(groupHead->fmode, entry->fmode);

			SYS_DBG(LOG_DBG_IGMP, "%s() %d sortedId=%u; groupHead->fmode=%02x \n", __FUNCTION__, __LINE__, sortedIdx, *(groupHead->fmode.bits));

			switch(entry->form)
			{
				case IGMP_GROUP_ENTRY_DYNAMIC:
				case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
				case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
					//find clientIp list
					if(!mcast_host_tracking_find(portId, groupHead, clientIp))
					{
						//insert clientIp to clientIp List
						if(SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, portId))
						{
							SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
						}
					}
					break;
				default:
					SYS_DBG(LOG_DBG_IGMP, "Not support group type\n");
			}

            return SYS_ERR_OK;
        }
        else
            return SYS_ERR_MCAST_DATABASE_FULL;
    }
    return SYS_ERR_FAILED;
}

/*
must have the field : dip, sip,vid, ipType, ipv6 must have dipv6
*/
igmp_group_entry_t* mcast_group_get(igmp_group_entry_t *pGroup)
{
    uint16              sortedIdx;
    igmp_group_entry_t  *groupHead;
    igmp_group_entry_t  *subgroup;

    SYS_PARAM_CHK(NULL == pGroup, NULL);

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);

    if (groupHead)    /* further, compare dip + sip*/
    {
        if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            return groupHead;
        }
        else
        {
            mcast_igmp_subgroup_sip_search(groupHead, pGroup->sip, &subgroup, NULL);
            if (subgroup)
                return subgroup;
            else
                return NULL;
        }
    }

    return NULL;
}


igmp_group_entry_t* mcast_group_get_with_index(uint32 index)
{
    uint16              i, j = 0;
    igmp_group_entry_t  *subgroup;

    for (i = 0; i < group_sortedAry_entry_num; i++)
    {
        if (pp_group_sorted_array[i] == NULL)
            break;

        subgroup = pp_group_sorted_array[i];
        while (subgroup)
        {
            if (j == index)
                return subgroup;

            subgroup = subgroup->next_subgroup;
            j++;
        }
    }

    return NULL;
}

/*
must have the field : dip, sip,vid, ipType, ipv6 must have dipv6
*/
int32 mcast_group_del(igmp_group_entry_t *pGroup)
{
    uint16          sortedIdx;
    igmp_group_entry_t  *prevNode;
    igmp_group_entry_t  *subgroup;
    igmp_group_entry_t  *groupHead;
    igmp_group_entry_t   delGroup;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(IS_VID_INVALID(pGroup->vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((pGroup->ipType < MULTICAST_TYPE_IPV4 ||pGroup->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);
    osal_memset(&delGroup, 0, sizeof(igmp_group_entry_t));
    osal_memcpy(&delGroup, pGroup, sizeof(igmp_group_entry_t));

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);

    if (groupHead)
    {
        if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            if (SYS_ERR_OK == mcast_group_sortedArray_del(sortedIdx))
            {
                mcast_group_num_cal(delGroup.ipType, FALSE);
                mcast_group_entry_release(groupHead);
                return SYS_ERR_OK;
            }
        }
        else
        {
            prevNode = NULL;
            mcast_igmp_subgroup_sip_search(groupHead, delGroup.sip, &subgroup, &prevNode);

            if (subgroup)
            {
                if (subgroup == pp_group_sorted_array[sortedIdx])   /* target group is group-head */
                {
                    pp_group_sorted_array[sortedIdx] = subgroup->next_subgroup;
                    if (pp_group_sorted_array[sortedIdx] == NULL)
                        mcast_group_sortedArray_del(sortedIdx);
                    mcast_group_entry_release(subgroup);
                    //mcast_group_num--;
                    mcast_group_num_cal(delGroup.ipType, FALSE);
                }
                else
                {
                    prevNode->next_subgroup = subgroup->next_subgroup;
                    mcast_group_entry_release(subgroup);
                    //mcast_group_num--;
                    mcast_group_num_cal(delGroup.ipType, FALSE);
                }
                return SYS_ERR_OK;
            }
        }

      //  osal_time_udelay(10);
    }

    return SYS_ERR_FAILED;
}

int32 mcast_group_subgroup_hw_del_all(igmp_group_entry_t  *entry)
{
    igmp_group_entry_t  *subgroup;
    sys_l2McastEntry_t  l2Mcst_entry;
    sys_ipMcastEntry_t  ipMcst_entry;

    osal_memset(&l2Mcst_entry, 0, sizeof(sys_l2McastEntry_t));
    osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));

    subgroup = entry;
    while (subgroup)
    {
        if ((MULTICAST_TYPE_IPV4 == entry->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == entry->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            l2Mcst_entry.vid = subgroup->vid;
            osal_memcpy(l2Mcst_entry.mac, subgroup->mac, MAC_ADDR_LEN);
            if (SYS_ERR_OK != rsd_l2McastEntry_del(&l2Mcst_entry))
            {
                SYS_DBG(LOG_DBG_IGMP, "Failed to delete entry from H/W !!\n");
            }
        }
        else
        {
            if (0 != subgroup->sip)
            {
                ipMcst_entry.vid = subgroup->vid;
                ipMcst_entry.sip = subgroup->sip;
                ipMcst_entry.dip = subgroup->dip;
				ipMcst_entry.isGrpRefB = mcast_group_ref_get(subgroup);
                if (SYS_ERR_OK != rsd_ipMcastEntry_del(&ipMcst_entry))
                {
                    SYS_DBG(LOG_DBG_IGMP, "Failed to delete entry from H/W !! sip %x\n", ipMcst_entry.sip);
                }
            }
        }
        subgroup = subgroup->next_subgroup;
    }
    return SYS_ERR_OK;
}

int32 mcast_group_del_all(void)
{
    int i;
    sys_vlanmask_t  vlanMsk;

    /* To avoid rx packet interfere group database processing*/
    igmp_packet_stop = ENABLED;

    VLANMASK_SET_ALL(vlanMsk);
    mcast_static_group_save(vlanMsk, MULTICAST_TYPE_END);

    for (i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        if (pp_group_sorted_array[i] == NULL)
            break;

        mcast_group_subgroup_hw_del_all(pp_group_sorted_array[i]);
    }

    mcast_igmp_group_head_db_init();
    mcast_group_db_init();

    if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
    {
        mcast_aggregate_db_init();
    }

    mcast_static_group_restore();

    /* restore to original status */
    igmp_packet_stop = DISABLED;

    return SYS_ERR_OK;
}


int32 mcast_suppre_flag_check(igmp_group_entry_t *pEntry, uint8 *pSendFlag)
{
    SYS_PARAM_CHK((NULL == pSendFlag), SYS_ERR_NULL_POINTER);

    if (pEntry)
    {
        if (pEntry->suppreFlag)
        {
            *pSendFlag = 0;
        }
        else
        {
            if (MODE_GPON != igmpCtrl.ponMode)
            {
                pEntry->suppreFlag = 1;
            }
            *pSendFlag = 1;
        }
    }
    else
        *pSendFlag = 1;

    return SYS_ERR_OK;
}

int32 treatmentTag(uint8 tagType, uint16 vid, sys_nic_pkt_t *pPkt, igmp_tag_t outTag)
{
    uint16 outer_vid = 0xFFFF;
    SYS_DBG(LOG_DBG_IGMP, "%s() original vid=%u\n", __FUNCTION__, vid);

	if ((IGMP_ADD_VID | IGMP_ADD_PRI) & outTag.tagOp.tagAct)
	{
		memmove(&(pPkt->data[16]), &(pPkt->data[12]), pPkt->length - 12);
		osal_memcpy(&(pPkt->data[12]), &(outTag.tpid), sizeof(uint16));
		osal_memcpy(&(pPkt->data[14]), &(outTag.tci.val), sizeof(uint16));
		pPkt->length += 4;
		pPkt->tail += 4;//length+=4;

		if (CTAG == tagType)
			outer_vid = outTag.tci.bit.vid;

		SYS_DBG(LOG_DBG_IGMP, "%s() Add vid =%u, pri=%u, len=%u, outer_vid=%u\n",
            __FUNCTION__, outTag.tci.bit.vid, outTag.tci.bit.pri, pPkt->length, outer_vid);

	}
	else if ((IGMP_MODIFY_VID & outTag.tagOp.tagAct) || (IGMP_MODIFY_PRI & outTag.tagOp.tagAct))
	{
		if (IGMP_MODIFY_VID & outTag.tagOp.tagAct)
		{
			pPkt->data[14] &= 0xF0;
			pPkt->data[14] |= ((uint8)(outTag.tci.bit.vid >> 8) & 0xF) ;
			pPkt->data[15] = outTag.tci.bit.vid & 0xFF;
			if(CTAG == tagType)
				outer_vid = outTag.tci.bit.vid;

			SYS_DBG(LOG_DBG_IGMP, "%s() modify vid =%u, outer_vid=%u\n", __FUNCTION__, outTag.tci.bit.vid, outer_vid);
		}
		if (IGMP_MODIFY_PRI & outTag.tagOp.tagAct)
		{
			pPkt->data[14] &= 0x1F;
			pPkt->data[14] |= (((uint8)(outTag.tci.bit.pri & 0x7)) << 5);

			SYS_DBG(LOG_DBG_IGMP, "%s() modify pri=%u, outer_vid=%u\n", __FUNCTION__, outTag.tci.bit.pri, outer_vid);
		}

        if (IGMP_MODIFY_TPID & outTag.tagOp.tagAct)
        {
            osal_memcpy(&(pPkt->data[12]), &(outTag.tpid), sizeof(uint16));
        }
	}
	else if (IGMP_REMOVE_VLAN & outTag.tagOp.tagAct)
	{
		memmove(&(pPkt->data[12]), &(pPkt->data[16]), (pPkt->length - 12 - 4));
		pPkt->length -= 4;
		pPkt->tail -= 4;//length-=4;
		pPkt->rx_tag.cvid_tagged = FALSE;
		outer_vid = 0;

		SYS_DBG(LOG_DBG_IGMP, "%s() remove vlan, outer_vid=%u\n", __FUNCTION__, outer_vid);
	}

	//TBD
	return SYS_ERR_OK;
}

/* only handle tag packet */
int32 filterTag(igmp_tag_t inTag, uint16 vid, uint16 pri)
{
	int32 ret = SYS_ERR_FAILED;
	switch(inTag.tagOp.tagFilter)
	{
		case IGMP_FILTER_PRI: /* filter pri */
			if (inTag.tci.bit.pri == pri)
				ret = SYS_ERR_OK;
			break;
		case IGMP_FILTER_VID: /* filter vid */
			if (inTag.tci.bit.vid == vid)
				ret = SYS_ERR_OK;
			break;
		case IGMP_NO_FILTER:
			ret = SYS_ERR_OK;
			break;
		case IGMP_FILTER_ETYPE:
			printf("Not support yet, always PASS\n");
			ret = SYS_ERR_OK;
			break;
		default:
			if (IGMP_FILTER_ETYPE & inTag.tagOp.tagFilter)
			{
				/* filter etype + X */
				if ((IGMP_FILTER_VID & inTag.tagOp.tagFilter) &&
					(IGMP_FILTER_PRI & inTag.tagOp.tagFilter))
				{
					//TBD: check ETYPE is correct
					/* filter Etype + VID + PRI */
					if (inTag.tci.bit.pri == pri && inTag.tci.bit.vid == vid)
						ret = SYS_ERR_OK;
				}
			}
			else
			{
				/* no care etype + X */
				if(inTag.tci.bit.pri == pri && inTag.tci.bit.vid == vid)
					ret = SYS_ERR_OK;
			}
			break;
	}
	return ret;
}

int32 treatQueryTag(sys_nic_pkt_t *pPacket, igmp_tag_t tag)
{
    igmp_tag_filter_mode_e  tagFilter = tag.tagOp.tagFilter;
    uint16                  vid       = tag.tci.bit.vid;
    uint16                  pri       = tag.tci.bit.pri;
    uint16                  tpid      = tag.tpid;

	switch(tagFilter)
	{
		case IGMP_FILTER_PRI: /* filter pri */
			pPacket->data[14] &= 0x1F;
			pPacket->data[14] |= (((uint8)(pri & 0x7)) << 5);
			break;
		case IGMP_FILTER_VID: /* filter vid */
			pPacket->data[14] &= 0xF0;
			pPacket->data[14] |= (uint8)((vid >> 8) & 0xF) ;
			pPacket->data[15] = vid & 0xFF;
			break;
		default:
			if ((IGMP_FILTER_VID & tagFilter) && (IGMP_FILTER_PRI & tagFilter))
			{
				pPacket->data[14] &= 0xF0;
				pPacket->data[14] |= (uint8)((vid >> 8) & 0xF) ;
				pPacket->data[15] = vid & 0xFF;

				pPacket->data[14] &= 0x1F;
				pPacket->data[14] |= (((uint8)(pri & 0x7)) << 5);
			}
			break;
	}

    if (IGMP_FILTER_TPID & tagFilter)
        memcpy(&(pPacket->data[12]), &tpid, sizeof(uint16));

    return SYS_ERR_OK;
}

int32 treatmentQuery(sys_nic_pkt_t *pPacket, mcast_port_info_t entry)
{
	if (((IGMP_FILTER_UNTAG & entry.inTag1.tagOp.tagFilter) &&
		(IGMP_FILTER_UNTAG & entry.inTag2.tagOp.tagFilter)) ||
		((IGMP_NO_FILTER & entry.inTag1.tagOp.tagFilter) &&
		(IGMP_NO_FILTER & entry.inTag2.tagOp.tagFilter)))
	{
		//treat to untag
		SYS_DBG(LOG_DBG_IGMP, "%s() %d treat to untag \n", __FUNCTION__, __LINE__);
		if (TRUE == pPacket->rx_tag.svid_tagged)
		{
			memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
			pPacket->length -= 4;
			//pPacket->tail -= 4;
			pPacket->rx_tag.svid_tagged = FALSE;
		}
		if(TRUE == pPacket->rx_tag.cvid_tagged)
		{
			memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
			pPacket->length -= 4;
			//acket->tail -= 4;
			pPacket->rx_tag.cvid_tagged = FALSE;
		}
	}else if (((IGMP_FILTER_UNTAG & entry.inTag2.tagOp.tagFilter) ||
			(IGMP_NO_FILTER & entry.inTag2.tagOp.tagFilter)) &&
			(!(IGMP_FILTER_UNTAG & entry.inTag1.tagOp.tagFilter) &&
			!(IGMP_NO_FILTER & entry.inTag1.tagOp.tagFilter)))
	{
		//treat to one stag
		SYS_DBG(LOG_DBG_IGMP, "%s() %d treat to one stag \n", __FUNCTION__, __LINE__);

		if (TRUE == pPacket->rx_tag.svid_tagged && TRUE == pPacket->rx_tag.cvid_tagged)
		{
			//modify s
			treatQueryTag(pPacket, entry.inTag1);
			//remove c
			memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
			pPacket->length -= 4;
			//acket->tail -= 4;
			pPacket->rx_tag.cvid_tagged = FALSE;
		}
		else
		{
			// 1to1(s2s)
			if (FALSE == pPacket->rx_tag.cvid_tagged)
			{
				//DS with Stag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d replace stag to stag' \n", __FUNCTION__, __LINE__);
				treatQueryTag(pPacket, entry.inTag1);
			}
		}
	}
	else if ((!(IGMP_FILTER_UNTAG & entry.inTag2.tagOp.tagFilter) &&
			!(IGMP_NO_FILTER & entry.inTag2.tagOp.tagFilter)) &&
			((IGMP_FILTER_UNTAG & entry.inTag1.tagOp.tagFilter) ||
			(IGMP_NO_FILTER & entry.inTag1.tagOp.tagFilter)))
	{
		//treat to one ctag
		SYS_DBG(LOG_DBG_IGMP, "%s() %d treat to one ctag \n", __FUNCTION__, __LINE__);
		if (TRUE == pPacket->rx_tag.svid_tagged && TRUE == pPacket->rx_tag.cvid_tagged)
		{
			//remove s
			memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
			pPacket->length -= 4;
			//acket->tail -= 4;
			pPacket->rx_tag.svid_tagged = FALSE;
			//modify c
			treatQueryTag(pPacket, entry.inTag2);
		}
		else
		{
			// 1to1(c2c)
			if (FALSE == pPacket->rx_tag.svid_tagged)
			{
				//DS with Ctag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d replace --> ctag to ctag \n", __FUNCTION__, __LINE__);
				treatQueryTag(pPacket, entry.inTag2);
			}
			// 1to1(s2c)
			if (FALSE == pPacket->rx_tag.cvid_tagged)
			{
				uint16 tpid = htons(0x88a8);
				osal_memcpy(&(pPacket->data[12]), &tpid, sizeof(tpid));
				//DS with Stag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d replace --> stag to ctag \n", __FUNCTION__, __LINE__);
				treatQueryTag(pPacket, entry.inTag2);
			}
		}
	}
	else
	{
		//modify s
		SYS_DBG(LOG_DBG_IGMP, "%s() %d replace stag and ctag\n", __FUNCTION__, __LINE__);
		treatQueryTag(pPacket, entry.inTag1);
		//modify c
		treatQueryTag(pPacket, entry.inTag2);
	}

	return SYS_ERR_OK;
}

int32 isSameAniWithUnTag(mcast_port_info_t item)
{
	mcast_port_info_entry_t *ptr = NULL;
    LIST_FOREACH(ptr, &portInfoHead, entries)
	{
		if (!osal_memcmp(&(ptr->entry), &item, sizeof(mcast_port_info_t)))
		{
			//the smae excactly
			continue;
		}
		else
		{
			//check ani tag
			if (ptr->entry.outTag1.tci.val == item.outTag1.tci.val &&
				ptr->entry.outTag1.tpid == item.outTag1.tpid &&
				ptr->entry.outTag2.tci.val == item.outTag2.tci.val &&
				ptr->entry.outTag2.tpid == item.outTag2.tpid)
			{
				if(IGMP_FILTER_UNTAG == ptr->entry.inTag2.tagOp.tagFilter &&
					IGMP_FILTER_UNTAG == ptr->entry.inTag1.tagOp.tagFilter)
				{
					return TRUE;
				}
			}
		}
    }
	return FALSE;
}

int32 IsHandleByExtVlan(uint16 mopId)
{
	mcast_prof_entry_t *ptr = NULL;

	IGMP_DB_SEM_LOCK();
	LIST_FOREACH(ptr, &mcastProfHead, entries)
	{
		if (ptr->entry.mopId == mopId &&
			0 == ptr->entry.dsIgmpTci[0])
		{
			IGMP_DB_SEM_UNLOCK();
			return TRUE;
		}
	}
	IGMP_DB_SEM_UNLOCK();

	return FALSE;
}

int32 mcast_viduni_get(unsigned int port, uint16 mopId, uint16 *pVidUni)
{
	mcast_port_info_entry_t *ptr = NULL;

    SYS_PARAM_CHK((!pVidUni), SYS_ERR_INPUT);

	LIST_FOREACH(ptr, &portInfoHead, entries)
	{
		if (ptr->entry.uniPort == port && mopId == ptr->entry.mopId)
		{
			*pVidUni = ptr->entry.vidUni;
			return SYS_ERR_OK;
		}
	}
	return SYS_ERR_FAILED;
}

int32 treatQueryByMcastProf(sys_nic_pkt_t *pPacket, uint16 mopId, uint16 vidUni)
{
	mcast_prof_entry_t *ptr = NULL;
	uint16 vid;
	IGMP_DB_SEM_LOCK();

	LIST_FOREACH(ptr, &mcastProfHead, entries)
	{
		if (ptr->entry.mopId == mopId) //first match, no matter one port with multiple mcast profile
		{
			switch (ptr->entry.dsIgmpTci[0])
			{
				case IGMP_DS_TAG_ACTION_STRIP_TAG:
					if (TRUE == pPacket->rx_tag.svid_tagged)
					{
						memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
						pPacket->length -= 4;
						pPacket->rx_tag.svid_tagged = FALSE;
					}
					if (TRUE == pPacket->rx_tag.cvid_tagged)
					{
						memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
						pPacket->length -= 4;
						pPacket->rx_tag.cvid_tagged = FALSE;
					}
					break;
				case IGMP_DS_TAG_ACTION_ADD_TAG:
					memmove(&(pPacket->data[16]), &(pPacket->data[12]), pPacket->length - 12);
					pPacket->data[12] = 0x81;
					pPacket->data[13] = 0x00;
					pPacket->data[14] = ptr->entry.dsIgmpTci[1];
					pPacket->data[15] = ptr->entry.dsIgmpTci[2];
					pPacket->length += 4;
					pPacket->rx_tag.cvid_tagged = TRUE;
					break;
				case IGMP_DS_TAG_ACTION_REPLACE_TAG:
					pPacket->data[14] = ptr->entry.dsIgmpTci[1];
					pPacket->data[15] = ptr->entry.dsIgmpTci[2];
					break;
				case IGMP_DS_TAG_ACTION_REPLACE_VID:
					vid = (((ptr->entry.dsIgmpTci[1] & 0x0F) << 8) | ptr->entry.dsIgmpTci[2]);
					pPacket->data[14] &= 0xF0;
					pPacket->data[14] |= (uint8)((vid >> 8) & 0xF) ;
					pPacket->data[15] = vid & 0xFF;
					break;
				case IGMP_DS_TAG_ACTION_ADD_VIDUNI_TAG:
					memmove(&(pPacket->data[16]), &(pPacket->data[12]), pPacket->length - 12);
					pPacket->data[12] = 0x81;
					pPacket->data[13] = 0x00;
					if (USHRT_MAX == vidUni || 4096 == vidUni)
					{
						pPacket->data[14] = ptr->entry.dsIgmpTci[1];
						pPacket->data[15] = ptr->entry.dsIgmpTci[2];
					}
					else
					{
						vidUni = htons(vidUni);
						osal_memcpy(&pPacket->data[14], &vidUni, sizeof(uint16));
					}
					pPacket->length += 4;
					pPacket->rx_tag.cvid_tagged = TRUE;
					break;
				case IGMP_DS_TAG_ACTION_REPLACE_VIDUNI_TAG:
					if (USHRT_MAX == vidUni)
					{
						//replace to dsigmpTci
						pPacket->data[14] = ptr->entry.dsIgmpTci[1];
						pPacket->data[15] = ptr->entry.dsIgmpTci[2];
					}
					else if (4096 == vidUni)
					{
						//strip outer
						if (TRUE == pPacket->rx_tag.svid_tagged)
						{
							memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
							pPacket->length -= 4;
							pPacket->rx_tag.svid_tagged = FALSE;
						}
						else if (TRUE == pPacket->rx_tag.cvid_tagged)
						{
							memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
							pPacket->length -= 4;
							pPacket->rx_tag.cvid_tagged = FALSE;
						}
					}
					else
					{
						vidUni = htons(vidUni);
						osal_memcpy(&pPacket->data[14], &vidUni, sizeof(uint16));
					}
					break;
				case IGMP_DS_TAG_ACTION_REPLACE_VID2VIDUNI:
					if (USHRT_MAX == vidUni)
					{
						//replace vid to dsIgmpTci
						vid = (((ptr->entry.dsIgmpTci[1] & 0x0F) << 8) | ptr->entry.dsIgmpTci[2]);
						pPacket->data[14] &= 0xF0;
						pPacket->data[14] |= (uint8)((vid >> 8) & 0xF) ;
						pPacket->data[15] = vid & 0xFF;
					}
					else if(4096 == vidUni)
					{
						//strip outer
						if (TRUE == pPacket->rx_tag.svid_tagged)
						{
							memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
							pPacket->length -= 4;
							pPacket->rx_tag.svid_tagged = FALSE;
						}
						else if(TRUE == pPacket->rx_tag.cvid_tagged)
						{
							memmove(&(pPacket->data[12]), &(pPacket->data[16]), (pPacket->length - 12 - 4));
							pPacket->length -= 4;
							pPacket->rx_tag.cvid_tagged = FALSE;
						}
					}
					else
					{
						pPacket->data[14] &= 0xF0;
						pPacket->data[14] |= (uint8)((vidUni >> 8) & 0xF) ;
						pPacket->data[15] = vidUni & 0xFF;
					}
					break;
				default:
					SYS_DBG(LOG_DBG_IGMP, "%s() %d the others are NOT support \n", __FUNCTION__, __LINE__);
					break;
			}
		}
	}
	IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

int32 is_handle_query(sys_nic_pkt_t *pPacket, mcast_port_info_entry_t *ptr)
{
	int32 ret = FALSE;
	uint16 vidUni = USHRT_MAX;
	if (IsHandleByExtVlan(ptr->entry.mopId))
	{
		if (!igmpCtrl.igmpTagBehavior_enable)
		{
			//first hit
			SYS_DBG(LOG_DBG_IGMP, "%s() %d first hit \n", __FUNCTION__, __LINE__);
			treatmentQuery(pPacket, ptr->entry);
			ret = TRUE;
		}
		else
		{
			//untag rule is absolute priority
			if (IGMP_FILTER_UNTAG == ptr->entry.inTag2.tagOp.tagFilter)
			{
				SYS_DBG(LOG_DBG_IGMP, "%s() %d ungtag handle\n", __FUNCTION__, __LINE__);
				treatmentQuery(pPacket, ptr->entry);
				ret = TRUE;
			}
			else
			{
				SYS_DBG(LOG_DBG_IGMP, "%s() %d check ingress is untag rule is in next entry or not \n", __FUNCTION__, __LINE__);
				//check ingress is untag rule is in next entry or not
				if (!isSameAniWithUnTag(ptr->entry))
				{
					SYS_DBG(LOG_DBG_IGMP, "%s() %d check ingress is untag rule is in next entry or not\n", __FUNCTION__, __LINE__);
					treatmentQuery(pPacket, ptr->entry);
					ret = TRUE;
				}
			}
		}
	}
	else
	{
		if (SYS_ERR_OK != mcast_viduni_get(ptr->entry.uniPort, ptr->entry.mopId, &vidUni))
		{
			SYS_DBG(LOG_DBG_IGMP, "%s() %d mcast_viduni_get FAILED \n", __FUNCTION__, __LINE__);
		}
		treatQueryByMcastProf(pPacket, ptr->entry.mopId, vidUni);
		ret = TRUE;
	}
	SYS_DBG(LOG_DBG_IGMP, "%s() %d tagBehavior=%u ret=%d\n", __FUNCTION__, __LINE__, igmpCtrl.igmpTagBehavior_enable, ret);
	return ret;
}

static int32 mcast_query_outer_vid_check(sys_nic_pkt_t *pPacket, mcast_port_info_entry_t *p)
{
    int32 ret = FALSE;
    if (!p || !pPacket)
        return ret;

    if (0 == p->entry.outTag1.tagOp.tagAct ||
        IGMP_REMOVE_VLAN & p->entry.outTag1.tagOp.tagAct)
    {
        // use outTag2 rule
        if ((IGMP_ADD_TPID & p->entry.outTag2.tagOp.tagAct ||
            IGMP_MODIFY_TPID & p->entry.outTag2.tagOp.tagAct) &&
            (0x88A8 == p->entry.outTag2.tpid) &&
            (pPacket->rx_tag.outer_vid == p->entry.outTag2.tci.bit.vid))
        {
            ret = TRUE;
        }
        else if (pPacket->rx_tag.outer_vid == p->entry.outTag2.tci.bit.vid)
        {
            ret = TRUE;
        }
        //TBD check other act TPID
    }
    else
    {
        // use outTag1 rule
        if ((IGMP_ADD_TPID & p->entry.outTag1.tagOp.tagAct ||
            IGMP_MODIFY_TPID & p->entry.outTag1.tagOp.tagAct) &&
            (0x88A8 == p->entry.outTag1.tpid) &&
            (pPacket->rx_tag.outer_vid == p->entry.outTag1.tci.bit.vid))
        {
            ret = TRUE;
        }
        else if (pPacket->rx_tag.outer_vid == p->entry.outTag1.tci.bit.vid)
        {
            ret = TRUE;
        }
        //TBD check other act TPID
    }
    return ret;
}

int32 mcast_query_treatment(sys_nic_pkt_t *pPacket, sys_logic_port_t port)
{
	mcast_port_info_entry_t *ptr = NULL;
	//uint8 matchFilterB = FALSE;

	SYS_DBG(LOG_DBG_IGMP, "%s() %d port =%u\n", __FUNCTION__, __LINE__, port);
	SYS_PARAM_CHK((NULL == pPacket), SYS_ERR_NULL_POINTER);

    LIST_FOREACH(ptr, &portInfoHead, entries)
	{
		if (ptr->entry.uniPort == port)
		{
			if (TRUE == pPacket->rx_tag.svid_tagged &&
				(TRUE == igmpCtrl.igmpVlanTag_type ||
				(pPacket->rx_tag.outer_vid == ptr->entry.outTag1.tci.bit.vid &&
				TRUE == pPacket->rx_tag.cvid_tagged &&
				pPacket->rx_tag.inner_vid == ptr->entry.outTag2.tci.bit.vid)))
			{
				//downstream with double tag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d DS is double tag \n", __FUNCTION__, __LINE__);
				if (is_handle_query(pPacket, ptr))
					break;
			}
			else if (TRUE == pPacket->rx_tag.svid_tagged &&
				(TRUE == igmpCtrl.igmpVlanTag_type ||
				(TRUE == mcast_query_outer_vid_check(pPacket, ptr))))
			{
				//downstream with stag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d DS is 1 stag \n", __FUNCTION__, __LINE__);
				if (is_handle_query(pPacket, ptr))
					break;
			}
			else if (TRUE == pPacket->rx_tag.cvid_tagged &&
				(TRUE == igmpCtrl.igmpVlanTag_type ||
				pPacket->rx_tag.inner_vid == ptr->entry.outTag2.tci.bit.vid))
			{
				//downstream with ctag
				SYS_DBG(LOG_DBG_IGMP, "%s() %d DS is 1 ctag \n", __FUNCTION__, __LINE__);
				if (is_handle_query(pPacket, ptr))
					break;
			}
		}
	}
	return SYS_ERR_OK;
}

int32 mcast_filter_treatment(sys_nic_pkt_t *pPkt, uint16 vid, sys_logic_port_t port, uint32 aclEntryId, mcast_preview_info_t previewInfo)
{
	uint16 msbId = aclEntryId >> 16;
	mcast_port_info_entry_t *ptr = NULL;
	uint8 matchFilterB = FALSE;

	SYS_DBG(LOG_DBG_IGMP, "%s() msbId=%u, port=%u\n", __FUNCTION__, msbId, port);

	LIST_FOREACH(ptr, &portInfoHead, entries)
	{
		if (ptr->entry.uniPort == port &&
			((MCAST_ALLOWED_PREVIEW != previewInfo.previewType && ptr->entry.mopId == msbId) ||
			(MCAST_ALLOWED_PREVIEW == previewInfo.previewType && ptr->entry.mopId == msbId)))
		{
			SYS_DBG(LOG_DBG_IGMP," %s() %d  port and msbId are matched stag=%u, ctag=%u \n", __FUNCTION__, __LINE__,
				pPkt->rx_tag.svid_tagged, pPkt->rx_tag.cvid_tagged);
			if(FALSE == pPkt->rx_tag.svid_tagged &&
				FALSE == pPkt->rx_tag.cvid_tagged &&
				(4096 == ptr->entry.vidUni || USHRT_MAX == ptr->entry.vidUni) &&
				(MCAST_ALLOWED_PREVIEW != previewInfo.previewType ||
				(MCAST_ALLOWED_PREVIEW == previewInfo.previewType && 0 == previewInfo.allowedPreviewUniVid)))
			{
				//filter untag
				if(((IGMP_FILTER_UNTAG & ptr->entry.inTag1.tagOp.tagFilter) &&
					(IGMP_FILTER_UNTAG & ptr->entry.inTag2.tagOp.tagFilter)) ||
					((IGMP_NO_FILTER & ptr->entry.inTag1.tagOp.tagFilter) &&
					(IGMP_NO_FILTER & ptr->entry.inTag2.tagOp.tagFilter)))
				{
					SYS_DBG(LOG_DBG_IGMP, "%s() pkt is untag/no filter any thing. Treat it\n", __FUNCTION__);
					treatmentTag(CTAG, vid, pPkt, ptr->entry.outTag2);
					treatmentTag(STAG, vid, pPkt, ptr->entry.outTag1);
					pPkt->tx_tag.dst_port_mask = ptr->entry.usFlowId;
					return SYS_ERR_OK;
				}
				else
				{
					continue;
				}
			}
			else
			{
				//single or double tag
				SYS_DBG(LOG_DBG_IGMP, "%s() pkt is double or single tag\n", __FUNCTION__);
				if (FALSE == pPkt->rx_tag.cvid_tagged)
				{
					if ((IGMP_FILTER_UNTAG & ptr->entry.inTag2.tagOp.tagFilter) ||
						(IGMP_NO_FILTER & ptr->entry.inTag2.tagOp.tagFilter))
					{
						SYS_DBG(LOG_DBG_IGMP, "%s() pkt is only one stag \n", __FUNCTION__);
						/* filter outer */
						if (SYS_ERR_OK == filterTag(ptr->entry.inTag1, pPkt->rx_tag.outer_vid, pPkt->rx_tag.outer_pri) &&
							((MCAST_ALLOWED_PREVIEW != previewInfo.previewType) ||
							(MCAST_ALLOWED_PREVIEW == previewInfo.previewType &&
							pPkt->rx_tag.outer_vid == previewInfo.allowedPreviewUniVid)))
						{
							/* match filter */
							matchFilterB = TRUE;
						}
						else
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}
				else
				{
					if (SYS_ERR_OK == filterTag(ptr->entry.inTag2, pPkt->rx_tag.inner_vid, pPkt->rx_tag.inner_pri))
					{
						/* filter outer */
						if (FALSE == pPkt->rx_tag.svid_tagged)
						{
							if (((IGMP_FILTER_UNTAG & ptr->entry.inTag1.tagOp.tagFilter) ||
								(IGMP_NO_FILTER & ptr->entry.inTag1.tagOp.tagFilter)) &&
								((MCAST_ALLOWED_PREVIEW != previewInfo.previewType) ||
								(MCAST_ALLOWED_PREVIEW == previewInfo.previewType &&
								pPkt->rx_tag.inner_vid == previewInfo.allowedPreviewUniVid)))
							{
								SYS_DBG(LOG_DBG_IGMP, "%s() pkt is only one ctag \n", __FUNCTION__);
								matchFilterB = TRUE;
							}
							else
							{
								continue;
							}
						}
						else
						{
							if (SYS_ERR_OK == filterTag(ptr->entry.inTag1, pPkt->rx_tag.outer_vid, pPkt->rx_tag.outer_pri) &&
								((MCAST_ALLOWED_PREVIEW != previewInfo.previewType) ||
								(MCAST_ALLOWED_PREVIEW == previewInfo.previewType &&
								pPkt->rx_tag.outer_vid == previewInfo.allowedPreviewUniVid)))
							{
								/* match filter */
								SYS_DBG(LOG_DBG_IGMP, "%s() pkt is double tag  stag +  ctag \n", __FUNCTION__);
								matchFilterB = TRUE;
							}
							else
							{
								continue;
							}
						}
					}
					else
					{
						continue;
					}
				}
				if (matchFilterB)
				{
					if (USHRT_MAX == ptr->entry.vidUni ||
						(4097 == ptr->entry.vidUni && (TRUE == pPkt->rx_tag.svid_tagged || TRUE == pPkt->rx_tag.cvid_tagged)) ||
						(TRUE == pPkt->rx_tag.svid_tagged && ptr->entry.vidUni == pPkt->rx_tag.outer_vid) ||
						(FALSE == pPkt->rx_tag.svid_tagged && TRUE == pPkt->rx_tag.cvid_tagged && ptr->entry.vidUni == pPkt->rx_tag.inner_vid))
					{
						treatmentTag(CTAG, vid, pPkt, ptr->entry.outTag2);
						treatmentTag(STAG, vid, pPkt, ptr->entry.outTag1);
						pPkt->tx_tag.dst_port_mask = ptr->entry.usFlowId;
						return SYS_ERR_OK;
					}
				}
			}
		}
	}
	return SYS_ERR_FAILED;
}

int32 mcast_groupMbrPort_add_wrapper(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, uint32 groupSrcIp, mcast_groupAddr_t *pAddr,
    sys_logic_port_t port,  uint8 *pSendFlag, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    uint16                  sortedIdx = 0;
    uint32                  dip = 0;
    igmp_group_entry_t      groupKeyEntry;
    igmp_group_entry_t      *pEntry = NULL;
    igmp_group_entry_t      *groupHead = NULL;
    sys_ipMcastEntry_t      ipMcstEntry;
    igmp_group_head_entry_t *pGroup_head_entry = NULL;
    int32                   ret = SYS_ERR_OK;
    sys_l2McastEntry_t      l2McstEntry;
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    sys_logic_portmask_t    rtPmsk;
    uint32                  portEntryNum = 0;
    uint16                  limitNum = 0, grpVid = 0;
	uint32					aclEntryId = UINT_MAX, imputedGrpBw = 0;
	uint32					grpMbrTimerIntv = DFLT_IGMP_GROUP_MEMBERSHIP_INTV;
	mcast_preview_info_t	previewInfo;
    mcast_vlan_entry_t *pMcastVlan = NULL;

    osal_memset(&ipMcstEntry, 0, sizeof(ipMcstEntry));
    osal_memset(&l2McstEntry, 0, sizeof(l2McstEntry));
	osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));
    LOGIC_PORTMASK_CLEAR_ALL(rtPmsk);

	SYS_DBG(LOG_DBG_IGMP,
        "%s()@%d: vid=%d group="IPADDR_PRINT" port=%d\n", __FUNCTION__, __LINE__, vid, IPADDR_PRINT_ARG(pAddr->ipv4), port);

    dip = (MULTICAST_TYPE_IPV4 == ipType ? (pAddr->ipv4) : (IPV6_TO_IPV4(pAddr->ipv6.addr)));

    //igmp filter
    SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_check(
        pRecvTime, ipType, port, groupSrcIp, pAddr, &aclEntryId, &grpVid, &imputedGrpBw, &previewInfo)), SYS_ERR_FAILED);

	if (MODE_GPON == igmpCtrl.ponMode)
	{
		if (pBuf)
		{
			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_treatment(pBuf, vid, port, aclEntryId, previewInfo)), SYS_ERR_FAILED);

			SYS_DBG(LOG_DBG_IGMP, "%s()@%d: aclEntryId=%x sid=%u imputedGrpBw=%u\n", __FUNCTION__, __LINE__,
                aclEntryId, pBuf->tx_tag.dst_port_mask, imputedGrpBw);
		}
		vid = grpVid;
	}

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pAddr->ipv6.addr, &groupKeyEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

	mcast_profile_time_intv_get(INTV_TYPE_GROUP_MBR, aclEntryId, &previewInfo, &grpMbrTimerIntv);
    mcast_port_entry_get(ipType, port, &portEntryNum);
    mcast_portGroup_limit_get(ipType, port, &limitNum);

    /* search group entry by shortKey and output sorted arrary index for this group entry */
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupKeyEntry), &sortedIdx, &groupHead);
    /* check proxy reporting */
    mcast_suppre_flag_check(groupHead, pSendFlag);

    SYS_DBG(LOG_DBG_IGMP, "%s %d, shortKey=%llu\n", __FUNCTION__, __LINE__, mcast_group_sortKey_ret(&groupKeyEntry));

    if (groupHead)
    {
    	SYS_DBG(LOG_DBG_IGMP, " %s() %d groupHead is NOT NULL \n", __FUNCTION__, __LINE__);

        if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
        {
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }

        if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
        {
        	if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
        	{
	            if (portEntryNum >= limitNum)
	            {
	                SYS_DBG(LOG_DBG_IGMP, "The Port %d group entry is full \n", port);
	                if (SYS_ERR_FAILED == mcast_maxGroupAct_process(groupHead->ipType, groupHead->vid, groupHead->dip, 0, port))
	                {
	                    IGMP_DB_SEM_UNLOCK();
	                    return (MODE_GPON == igmpCtrl.ponMode ? SYS_ERR_FAILED : SYS_ERR_OK);
	                }
	            }
        	}
        }

		if (MODE_GPON != igmpCtrl.ponMode)
		{
	        mcast_vlan_db_get(vid, groupHead->ipType, &pMcastVlan);
	        if (NULL == pMcastVlan)
	        {
	            IGMP_DB_SEM_UNLOCK();
	            return SYS_ERR_FAILED;
	        }
		}

        if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            // multiple ip address mapping to the same mac address, ip should aggregate to mac
            /* Update aggregate database */
            mcast_aggregate_db_get(groupHead, &pAggrEntry);
            if (pAggrEntry == NULL)
            {
                SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d An existing group which has no aggregate record!\n", __FUNCTION__, __LINE__);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            groupHead->groupTimer = 0;
			if (MODE_GPON != igmpCtrl.ponMode)
            {
				groupHead->p_mbr_timer[port] = pMcastVlan->grpMembershipIntv;
			}
            else
			{
				if (MCAST_ALLOWED_PREVIEW != previewInfo.previewType || (0 == groupHead->p_mbr_timer[port]))
					groupHead->p_mbr_timer[port] = grpMbrTimerIntv;
			}
            groupHead->p_gsQueryCnt[port] = 0;

            if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
            {
            	if (0 != p_igmp_stats->p_port_info[port].maxBw &&
					p_igmp_stats->p_port_info[port].curBw >= p_igmp_stats->p_port_info[port].maxBw)
            	{
            	    //check max bw
            	    p_igmp_stats->p_port_info[port].bwExcdCount =
            	        ((UINT_MAX == p_igmp_stats->p_port_info[port].bwExcdCount) ? 0 : p_igmp_stats->p_port_info[port].bwExcdCount + 1);

                    SYS_DBG(LOG_DBG_IGMP, "%s() %d  exceed bw count :%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].bwExcdCount);

                    if (TRUE == p_igmp_stats->p_port_info[port].bwEnforceB)
            		{
            			IGMP_DB_SEM_UNLOCK();
	            		return SYS_ERR_FAILED;
            		}
            	}

                LOGIC_PORTMASK_SET_PORT(groupHead->mbr, port);

                if (0 == pAggrEntry->p_port_ref_cnt[port])
                {
                    l2McstEntry.vid = vid;
                    l2McstEntry.care_vid = groupHead->care_vid;
                    osal_memcpy(l2McstEntry.mac, groupHead->mac, MAC_ADDR_LEN);
                    if (SYS_ERR_OK == (ret = rsd_l2McastEntry_get(&l2McstEntry)))
                    {
                        /* ASIC's mbr may contain router port */
                        LOGIC_PORTMASK_OR(l2McstEntry.portmask, groupHead->mbr, l2McstEntry.portmask);
                        if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(ipType, &l2McstEntry)))
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
							IGMP_DB_SEM_UNLOCK();
							return SYS_ERR_FAILED;
                        }
						else
							p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
                    }
                }
                pAggrEntry->p_port_ref_cnt[port]++;
            }

			switch(groupHead->form)
			{
				case IGMP_GROUP_ENTRY_DYNAMIC:
				case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
				case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
					if(!mcast_host_tracking_find(port, groupHead, clientIp))
					{
						//insert clientIp to clientIp List
						if (SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, port))
						{
							SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
						}
					}
					break;
				default:
					SYS_DBG(LOG_DBG_IGMP, "Not support group type");
			}

        }
        /*IGMP_DMAC_VID_MODE != p_igmp_stats->lookup_mode and exist Group Entry */
        else
        {
        	SYS_DBG(LOG_DBG_IGMP, " %s() %d lookup mode is SIP+DIP \n", __FUNCTION__, __LINE__);
            /*IGMP_DMAC_VID_MODE != p_igmp_stats->lookup_mode and exist Group Entry and group sip = 0 */
            if (groupHead->sip == 0)     /* This group is created by only Join */
            {
				SYS_DBG(LOG_DBG_IGMP, " %s() %d exist group with sip 0.0.0.0 \n", __FUNCTION__, __LINE__);
            	if (MODE_GPON !=igmpCtrl.ponMode)
					groupHead->p_mbr_timer[port] = pMcastVlan->grpMembershipIntv;
				else
				{
					if (MCAST_ALLOWED_PREVIEW != previewInfo.previewType || (0 == groupHead->p_mbr_timer[port]))
                		groupHead->p_mbr_timer[port] = grpMbrTimerIntv;
				}
				if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
				{
					if (0 != p_igmp_stats->p_port_info[port].maxBw &&
						p_igmp_stats->p_port_info[port].curBw >= p_igmp_stats->p_port_info[port].maxBw)
					{
					    p_igmp_stats->p_port_info[port].bwExcdCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].bwExcdCount) ?
                                                                    0 : p_igmp_stats->p_port_info[port].bwExcdCount + 1);
						SYS_DBG(LOG_DBG_IGMP,
                            "%s() %d    exceed bw count :%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].bwExcdCount);

						if (TRUE == p_igmp_stats->p_port_info[port].bwEnforceB)
						{
							IGMP_DB_SEM_UNLOCK();
							return SYS_ERR_FAILED;
						}
					}
					LOGIC_PORTMASK_SET_PORT(groupHead->mbr, port);

					ipMcstEntry.dip = groupHead->dip;
					ipMcstEntry.vid = groupHead->vid;
					ipMcstEntry.sip = groupHead->sip;
                    ipMcstEntry.care_vid = groupHead->care_vid;
                    ipMcstEntry.isGrpRefB = mcast_group_ref_get(groupHead);

                    if (SYS_ERR_OK != rsd_ipMcastEntry_get(&ipMcstEntry))
                    {
                        //for check filter mode per port
					    LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, ipMcstEntry.portmask);
                    }

					//since sip = 0, don't care fmode
					LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, port);

					LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, groupHead->mbr);
					/* Also add router portmask */
					mcast_routerPort_get(vid, ipType, &rtPmsk);
					LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, rtPmsk);

					if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcstEntry)))
					{
						IGMP_DB_SEM_UNLOCK();
						return SYS_ERR_FAILED;
					}
					else
						p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
				}

				switch(groupHead->form)
				{
					case IGMP_GROUP_ENTRY_DYNAMIC:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
						if (!mcast_host_tracking_find(port, groupHead, clientIp))
						{
							//insert clientIp to clientIp List
							if (SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, port))
							{
								SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
							}
						}
						break;
					default:
						SYS_DBG(LOG_DBG_IGMP, "Not support group type");
				}

				IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_OK;      /* Need not write to ASIC */
            }
            /*IGMP_DMAC_VID_MODE != p_igmp_stats->lookup_mode and exist Group Entry and group sip != 0 */
            else                        /* This group is created by Data (maybe and Join) */
            {
				SYS_DBG(LOG_DBG_IGMP, " %s() %d exist group with multiple group source ip \n", __FUNCTION__, __LINE__);
                pEntry = groupHead;
                while (pEntry)
                {
                    //this group will be set 1.
                    pEntry->suppreFlag = groupHead->suppreFlag;
					if (MODE_GPON != igmpCtrl.ponMode)
						pEntry->p_mbr_timer[port] = pMcastVlan->grpMembershipIntv;
					else
					{
						if (MCAST_ALLOWED_PREVIEW != previewInfo.previewType ||
							(0 == pEntry->p_mbr_timer[port]))
						pEntry->p_mbr_timer[port] = grpMbrTimerIntv;
					}
                    pEntry->p_gsQueryCnt[port] = 0;

                    if (!IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, port))
                    {
                    	if (0 != p_igmp_stats->p_port_info[port].maxBw &&
							p_igmp_stats->p_port_info[port].curBw >= p_igmp_stats->p_port_info[port].maxBw)
						{
						    p_igmp_stats->p_port_info[port].bwExcdCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].bwExcdCount) ?
                                                                        0 : p_igmp_stats->p_port_info[port].bwExcdCount + 1);
							SYS_DBG(LOG_DBG_IGMP,
                                "%s() %d    exceed bw count:%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].bwExcdCount);

							if (TRUE == p_igmp_stats->p_port_info[port].bwEnforceB)
							{
								IGMP_DB_SEM_UNLOCK();
								return SYS_ERR_FAILED;
							}
						}
                        LOGIC_PORTMASK_SET_PORT(pEntry->mbr, port);
                        ipMcstEntry.vid = vid;
                        ipMcstEntry.dip = dip;
                        ipMcstEntry.sip = pEntry->sip;
						ipMcstEntry.isGrpRefB = mcast_group_ref_get(pEntry);
                        ipMcstEntry.care_vid = pEntry->care_vid;
						//modify:
						//ipMcstEntry.fmode = pEntry->fmode;
                        mcast_routerPort_get(vid, ipType, &rtPmsk);
                        LOGIC_PORTMASK_OR(ipMcstEntry.portmask, pEntry->mbr, rtPmsk);
						//for check filter mode per port
						LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, ipMcstEntry.portmask);
						//since sip != 0, care fmode
						if (IS_LOGIC_PORTMASK_PORTSET(pEntry->fmode, port))
						{
							/* exclude */
							LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, port);
						}
						else
						{
							/* include */
							LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, port);
						}
                        if (SYS_ERR_OK == rsd_ipMcastEntry_set(&ipMcstEntry))
							p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
                    }

                    switch(pEntry->form)
    				{
    					case IGMP_GROUP_ENTRY_DYNAMIC:
    					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
    					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
    						if (!mcast_host_tracking_find(port, pEntry, clientIp))
    						{
    							//insert clientIp to clientIp List
    							if (SYS_ERR_FAILED == mcast_host_tracking_add(pEntry, clientIp, port))
    							{
    								SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
    							}
    						}
    						break;
    					default:
    						SYS_DBG(LOG_DBG_IGMP, "Not support group type");
    				}

                    pEntry = pEntry->next_subgroup;
                }
            }
        }
    }
    /* new Group */
    else
    {
        if (igmp_packet_stop)
        {
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }
        // check max group per port
        if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
        {
	        if (portEntryNum >= limitNum)
	        {
	            SYS_DBG(LOG_DBG_IGMP, "The Port %d group entry is full \n", port);

	            if (SYS_ERR_FAILED == mcast_maxGroupAct_process(ipType, vid, dip, 0, port))
	            {
	            	SYS_DBG(LOG_DBG_IGMP, "%s() %d mcast_maxGroupAct_process FAILED\n", __FUNCTION__, __LINE__);
	                IGMP_DB_SEM_UNLOCK();
					return (MODE_GPON == igmpCtrl.ponMode ? SYS_ERR_FAILED : SYS_ERR_OK);
	            }
	            else // get the sortedIdx again
	            {
	                mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupKeyEntry), &sortedIdx, &groupHead);
	            }
	        }
        }
        pEntry = mcast_group_freeEntry_get(ipType);
        if (!pEntry)
        {
            SYS_DBG(LOG_DBG_IGMP, "Group database is full!\n");
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_MCAST_DATABASE_FULL;
        }

		if (MODE_GPON != igmpCtrl.ponMode)
		{
	        mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	        if (NULL == pMcastVlan)
	        {
	            mcast_group_entry_release(pEntry);
				SYS_DBG(LOG_DBG_IGMP, "vid %d does not  exist!\n",vid);
	            IGMP_DB_SEM_UNLOCK();
	            return SYS_ERR_FAILED;
	        }
		}
        pEntry->ipType = ipType;
		pEntry->imputedGrpBw = imputedGrpBw;

        if (MODE_GPON == igmpCtrl.ponMode)
        {
            pEntry->care_vid = (((aclEntryId & 0xFFFF) == 0xFFFF) ? DISABLED : ENABLED);
        }
        else
        {
            // For EPON mode
            // if control multicast, pEntry->care_vid = ENABLED;
            // if non-control multicast, pEntry->care_vid = DISABLED;
            // For normal case
            // According to the tag of report
        }

		switch (previewInfo.previewType)
		{
			case MCAST_NON_PREVIEW:
			case MCAST_PREVIEW:
				pEntry->form = IGMP_GROUP_ENTRY_DYNAMIC;
				break;
			case MCAST_ALLOWED_PREVIEW:
				pEntry->form = (previewInfo.length == 0 ? IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED : IGMP_GROUP_ENTRY_ALLOWED_PREVIEW);
				break;
			default:
				printf("Not support preview Type\n");
		}
        pEntry->dip = dip;
        pEntry->sip = 0;
        pEntry->vid = vid;
        pEntry->sortKey = mcast_group_sortKey_ret(&groupKeyEntry);

		SYS_DBG(LOG_DBG_IGMP,
            "%s %d, shortKey=%llu, ori=%llu\n", __FUNCTION__, __LINE__, pEntry->sortKey, mcast_group_sortKey_ret(&groupKeyEntry));

        pEntry->p_mbr_timer[port] = ((MODE_GPON != igmpCtrl.ponMode) ? pMcastVlan->grpMembershipIntv : grpMbrTimerIntv);

        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            pEntry->mac[0] = 0x01;
            pEntry->mac[1] = 0x00;
            pEntry->mac[2] = 0x5e;
            pEntry->mac[3] = (uint8)((dip >> 16) & 0x7f);
            pEntry->mac[4] = (uint8)((dip >> 8) & 0xff);
            pEntry->mac[5] = (uint8)(dip & 0xff);
        }
        else
        {
            osal_memcpy(pEntry->dipv6.addr, pAddr->ipv6.addr, IPV6_ADDR_LEN);
            pEntry->mac[0] = 0x33;
            pEntry->mac[1] = 0x33;
            pEntry->mac[2] = (uint8)((dip >> 24) & 0xff);
            pEntry->mac[3] = (uint8)((dip >> 16) & 0xff);
            pEntry->mac[4] = (uint8)((dip >> 8) & 0xff);
            pEntry->mac[5] = (uint8)(dip & 0xff);
        }

        LOGIC_PORTMASK_CLEAR_ALL(pEntry->mbr);
        LOGIC_PORTMASK_SET_PORT(pEntry->mbr, port);

        //new group. will send to router.
        if (MODE_GPON == igmpCtrl.ponMode)
        {
            pEntry->suppreFlag = (IGMP_MODE_SPR == igmpCtrl.igmpMode ? 1 : 0);
        }
        else
        {
            pEntry->suppreFlag = 1;
        }

        /* Group entry is new and  IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode */
        if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            LOGIC_PORTMASK_COPY(l2McstEntry.portmask, pEntry->mbr);
            /* Also add router portmask */
            mcast_routerPort_get(vid, ipType, &rtPmsk);
            LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);

            l2McstEntry.vid = vid;
            l2McstEntry.care_vid = pEntry->care_vid;
            osal_memcpy(l2McstEntry.mac, pEntry->mac, MAC_ADDR_LEN);

            /* Handle DIP -> MAC aggregation */
            mcast_aggregate_db_get(pEntry, &pAggrEntry);

            if (0 != p_igmp_stats->p_port_info[port].maxBw &&
				p_igmp_stats->p_port_info[port].curBw >= p_igmp_stats->p_port_info[port].maxBw)
			{
			    //check max bw per port
			    p_igmp_stats->p_port_info[port].bwExcdCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].bwExcdCount) ?
			                                                0 : p_igmp_stats->p_port_info[port].bwExcdCount + 1);
				SYS_DBG(LOG_DBG_IGMP,
                    "%s()@%d: exceed bw count :%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].bwExcdCount);

				if(TRUE == p_igmp_stats->p_port_info[port].bwEnforceB)
				{
					IGMP_DB_SEM_UNLOCK();
					return SYS_ERR_FAILED;
				}
			}

            if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_add(ipType, &l2McstEntry)))
            {
                /*The LUT entry may already be created by another group that map to the same aggregation. ex: 224.2.2.1 and 225.2.2.1 */
                if (SYS_ERR_L2_MAC_IS_EXIST == ret)
                {
                    if (NULL == pAggrEntry)
                    {
                        /* group entry in aggregation db is not found */
                        SYS_DBG((ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD),
                            "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                        /* conflict since entry in hw entry is found but it in sw aggr. db is not found.
                            In order to sw and hw sync delete hw entry */
                        if(SYS_ERR_OK == rsd_l2McastEntry_del(&l2McstEntry))
							p_igmp_stats->p_port_info[port].curBw -= imputedGrpBw;

                        mcast_group_entry_release(pEntry);
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                    }
                    // sw aggr. db is found, update hw entry
                    rsd_l2McastEntry_get(&l2McstEntry);
                    LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pEntry->mbr);
                    if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(ipType, &l2McstEntry)))
                    {
                        // update hw entry failed.
                        SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d  vid:%d   mac:"MAC_PRINT"\n",
                            ret, vid, MAC_PRINT_ARG(l2McstEntry.mac));
                        mcast_group_entry_release(pEntry);
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                    }
                    else
                    {
                        // update hw entry success.
                    	p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
                        if (SYS_ERR_OK == mcast_group_sortedArray_ins(sortedIdx, pEntry))
                        {
                            pAggrEntry->group_ref_cnt++;
                            pAggrEntry->p_port_ref_cnt[port]++;
                            mcast_group_num_cal(ipType, TRUE);

							mcast_group_sortedArray_search(pEntry->sortKey, &sortedIdx, &groupHead);
							switch(pEntry->form)
							{
								case IGMP_GROUP_ENTRY_DYNAMIC:
								case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
								case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
									if(!mcast_host_tracking_find(port, groupHead, clientIp))
									{
										//insert clientIp to clientIp List
										if(SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, port))
										{
											SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
										}
									}
									break;
								default:
									SYS_DBG(LOG_DBG_IGMP, "Not support group type");
							}
                        }
                    }
                }
                else if (SYS_ERR_L2_MAC_FULL == ret)
                {
                    SYS_DBG(LOG_DBG_IGMP,
                            "Failed writing to ASIC!  ret:%d  vid:%d   mac:"MAC_PRINT"\n", ret, vid, MAC_PRINT_ARG(pEntry->mac));

                    mcast_group_entry_release(pEntry);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                }
            }
            else
            {
                // add hw entry success, so increase igmp_hw_entry_num.
                if (pAggrEntry == NULL)
                {
                    SYS_DBG(LOG_DBG_IGMP, "%s() %d  pAggrEntry is NULL \n", __FUNCTION__, __LINE__);
                    /* group entry in aggregation db is not found, so add sw aggr. db entry */
                    if (SYS_ERR_OK != mcast_aggregate_db_add(pEntry, port))
                    {
                        // entry add failed in sw aggr. db.
                        SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                                "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                        // In order to sw and hw sync, del hw entry.
                        ret = rsd_l2McastEntry_del(&l2McstEntry);

                        if (SYS_ERR_OK != ret)
                            SYS_DBG(LOG_DBG_IGMP, "Failed to delete entry from H/W !!\n");
                        else
                            igmp_hw_entry_num--;
                        mcast_group_entry_release(pEntry);
                    }
                    else
                    {
                        // entry add success in sw aggr. db, so also add it in group_db entry
                        if (SYS_ERR_OK == mcast_group_sortedArray_ins(sortedIdx, pEntry))
                        {
                            if (MULTICAST_TYPE_IPV6 == ipType)
                            {
                                mcast_aggregate_db_get(pEntry, &pAggrEntry);
                                if (pAggrEntry)
                                    SYS_DBG(LOG_DBG_IGMP, "grp_ref_cnt=%u", pAggrEntry->group_ref_cnt);
                                SYS_DBG(LOG_DBG_IGMP, "%s()@%d", __FUNCTION__, __LINE__ );
                            }
                            // add success in group_db
                            p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
                            mcast_group_num_cal(ipType, TRUE);
                            igmp_hw_entry_num++;

                            mcast_group_sortedArray_search(pEntry->sortKey, &sortedIdx, &groupHead);

                            switch (pEntry->form)
                            {
                            	case IGMP_GROUP_ENTRY_DYNAMIC:
                                case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
                                case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
                                    if (!mcast_host_tracking_find(port, groupHead, clientIp))
                                    {
                                    	//insert clientIp to clientIp List
                                    	if (SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, port))
                                    	{
                                    		SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
                                    	}
                                    }
                                    break;
                                default:
                            	    SYS_DBG(LOG_DBG_IGMP, "Not support group type");
                            }

                        }
                        else
                        {
                            SYS_DBG(LOG_DBG_IGMP, "%s() %d  mcast_group_sortedArray insert FAILED \n", __FUNCTION__, __LINE__);
                            // add fail in group_db, so del hw entry and sw aggr. db etnry
                            rsd_l2McastEntry_del(&l2McstEntry);
                            mcast_group_entry_release(pEntry);
                            mcast_aggregate_db_del(pEntry);
                        }
                    }
                }
                else
                {
                    SYS_DBG(LOG_DBG_IGMP, "%s() %d  pAggrEntry is NOT null \n", __FUNCTION__, __LINE__);
                    /* entry add hw success and group entry in aggregation db is found, as conflict.
                       In order to sw and hw sync,  so del hw entry and sw aggr. db entry and group db entry */
                    rsd_l2McastEntry_del(&l2McstEntry);
                    mcast_group_entry_release(pEntry);
                    mcast_aggregate_db_del(pEntry);
                }
            }
        }
        /* Group Entry is new and IGMP_DMAC_VID_MODE != p_igmp_stats->lookup_mode */
        else
        {
        	if (MODE_GPON == igmpCtrl.ponMode)
        	{
				if (0 != p_igmp_stats->p_port_info[port].maxBw &&
					p_igmp_stats->p_port_info[port].curBw >= p_igmp_stats->p_port_info[port].maxBw)
				{
				    // check max bw limit
				    p_igmp_stats->p_port_info[port].bwExcdCount = ((UINT_MAX == p_igmp_stats->p_port_info[port].bwExcdCount) ?
				                                                0 : p_igmp_stats->p_port_info[port].bwExcdCount + 1);

					SYS_DBG(LOG_DBG_IGMP,
                        "%s() %d    exceed bw count :%u\n", __FUNCTION__, __LINE__, p_igmp_stats->p_port_info[port].bwExcdCount);

					if (TRUE == p_igmp_stats->p_port_info[port].bwEnforceB)
					{
						IGMP_DB_SEM_UNLOCK();
						return SYS_ERR_FAILED;
					}
				}

                switch (ipType)
                {
                    case MULTICAST_TYPE_IPV4:
                        LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, pEntry->mbr);
                        /* Also add router portmask */
                        mcast_routerPort_get(vid, ipType, &rtPmsk);
                        LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, rtPmsk);

                        ipMcstEntry.dip = pEntry->dip;
                        ipMcstEntry.vid = pEntry->vid;
                        ipMcstEntry.sip = pEntry->sip;
                        ipMcstEntry.care_vid = pEntry->care_vid;

                        LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, ipMcstEntry.portmask);
                        LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, port);
        	            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
        	            {
        	            	SYS_DBG(LOG_DBG_IGMP, "rsd_ipMcastEntry_add failed\n");
        					IGMP_DB_SEM_UNLOCK();
        					return SYS_ERR_FAILED;
        	            }
        				else
        					p_igmp_stats->p_port_info[port].curBw += imputedGrpBw;
                        break;
                    case MULTICAST_TYPE_IPV6:
                        LOGIC_PORTMASK_COPY(l2McstEntry.portmask, pEntry->mbr);
                        /* Also add router portmask */
                        mcast_routerPort_get(vid, ipType, &rtPmsk);
                        LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);

                        l2McstEntry.vid = pEntry->vid;
                        l2McstEntry.care_vid = pEntry->care_vid;
                        osal_memcpy(l2McstEntry.mac, pEntry->mac, MAC_ADDR_LEN);

                        if (SYS_ERR_OK != (ret = rsd_l2McastEntry_add(&l2McstEntry)))
        	            {
        	            	SYS_DBG(LOG_DBG_IGMP, "rsd_ipMcastEntry_add failed\n");
        					IGMP_DB_SEM_UNLOCK();
        					return SYS_ERR_FAILED;
        	            }
                        //TBD current Bandwidth
                        break;
                    default:
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                }
        	}

            if (SYS_ERR_OK == mcast_group_sortedArray_ins(sortedIdx, pEntry))
            {
                if (MULTICAST_TYPE_IPV6 == ipType)
                {
                    mcast_aggregate_db_get(pEntry, &pAggrEntry);
                    if (pAggrEntry)
                        SYS_DBG(LOG_DBG_IGMP, "grp_ref_cnt=%u", pAggrEntry->group_ref_cnt);
                    SYS_DBG(LOG_DBG_IGMP, "%s()@%d", __FUNCTION__, __LINE__ );
                }
                mcast_group_num_cal(ipType, TRUE);

                if (MODE_GPON == igmpCtrl.ponMode)
					igmp_hw_entry_num++;

				SYS_DBG(LOG_DBG_IGMP, "software insert OK sortKey=%llu sortIdx=%u, version=%d\n",
                    pEntry->sortKey, sortedIdx, p_igmp_stats->oper_version);

				mcast_group_sortedArray_search(pEntry->sortKey, &sortedIdx, &groupHead);

				switch(pEntry->form)
				{
					case IGMP_GROUP_ENTRY_DYNAMIC:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
					case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
						if (!mcast_host_tracking_find(port, groupHead, clientIp))
						{
							//insert clientIp to clientIp List
							if (SYS_ERR_FAILED == mcast_host_tracking_add(groupHead, clientIp, port))
							{
								SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
							}
						}
						break;
					default:
						SYS_DBG(LOG_DBG_IGMP, "Not support group type");
				}
            }

            if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                mcast_igmp_group_head_ins(dip, vid, port, MODE_IS_INCLUDE, &pGroup_head_entry, grpMbrTimerIntv);
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_groupMbrPort_del_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pAddr, sys_logic_port_t port, ipAddr_t clientIp)
{
    uint16                  sortedIdx = 0, grpVid = 0;
    uint32                  dip;
    igmp_group_entry_t      *groupHead;
    igmp_group_entry_t      *pEntry;
    igmp_group_entry_t      groupEntry;
    sys_logic_portmask_t    delPmsk;
    igmp_querier_entry_t    *pQuerEntry;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
	int                     fastleave=0;
	mcast_preview_info_t	previewInfo;
	uint32					aclEntryId = UINT_MAX, imputedGrpBw = 0;

	osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));

    dip = (MULTICAST_TYPE_IPV4 == ipType ? (pAddr->ipv4) : (IPV6_TO_IPV4(pAddr->ipv6.addr)));

    //igmp filter
    SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_check(
        NULL, ipType, port, 0, pAddr, &aclEntryId, &grpVid, &imputedGrpBw, &previewInfo)), SYS_ERR_FAILED);

	SYS_DBG(LOG_DBG_IGMP, " %s() %d  aclEntryId=%x\n", __FUNCTION__, __LINE__, aclEntryId);

	if (MODE_GPON == igmpCtrl.ponMode)
	{
		if(pBuf)
		{
			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_treatment(pBuf, vid, port, aclEntryId, previewInfo)), SYS_ERR_FAILED);
			SYS_DBG(LOG_DBG_IGMP, " %s() %d  sid=%u\n", __FUNCTION__, __LINE__, pBuf->tx_tag.dst_port_mask);
		}
		vid = grpVid;
	}

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pAddr->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);

	    if (!pMcastVlan)
	    {
	    	IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }

	    mcast_querier_db_get(vid, ipType, &pQuerEntry);

	    if (pQuerEntry == NULL)
	    {
	        SYS_PRINTF("[IGMP]%s():%d VLAN-%d doesn't have query DB entry!\n", __FUNCTION__, __LINE__, vid);
	        IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }
	}

    fastleave = mcast_fastLeaveMode_get(port);

    /* fast leave enable */
    if (IGMP_LEAVE_MODE_FAST == fastleave)
    {
        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

        SYS_DBG(LOG_DBG_IGMP, "fast leave enable, sortedIdx=%u, shortKey=%llu\n",
            sortedIdx, mcast_group_sortKey_ret(&groupEntry));

        if (groupHead)
        {
            /* group entry exist in sw db */
			SYS_DBG(LOG_DBG_IGMP, "%s() %d group group Head exist\n", __FUNCTION__, __LINE__);

            if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_OK;
            }

            if (MODE_GPON != igmpCtrl.ponMode)
            {
	            if (IGMP_QUERIER == pQuerEntry->status)
	            {
	                mcast_send_gs_query(groupHead, port);
	            }
            }

            if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
            {
                IGMP_DB_SEM_UNLOCK();
				return SYS_ERR_OK;
            }
            /* This group is created by only Join */
            /* group src ip = 0 */
            if (groupHead->sip == 0)
            {
            	if (SYS_ERR_OK == mcast_host_tracking_del(groupHead, clientIp, (uint16)port))
            	{
	            	if (mcast_is_last_host_per_port(groupHead, (uint16)port))
	            	{
		                groupHead->p_mbr_timer[port] = 0;
		                osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
		                LOGIC_PORTMASK_SET_PORT(delPmsk, port);

		                if (SYS_ERR_OK == mcast_hw_mcst_mbr_remove(groupHead, &delPmsk))
							p_igmp_stats->p_port_info[port].curBw -= imputedGrpBw;

                        LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);

                        /* sip == 0, this group doesn't have corresponding ASIC entry, just delete it */
                        if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
		                {
		                    mcast_group_del(groupHead);
							if (p_igmp_stats->oper_version == IGMP_VERSION_V3_FULL)
                            {
								mcast_igmp_group_head_del(dip, vid);
                            }
		                }
	            	}
            	}
            }
            /* group src ip != 0 */
            else
            {
                /* This group is created by Data (maybe and Join) */
                pEntry = groupHead;
                while (pEntry)
                {
					if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, (uint16)port))
					{
						if (mcast_is_last_host_per_port(pEntry, (uint16)port))
						{
		                    pEntry->p_mbr_timer[port] = 0;
		                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
		                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);

                            if (SYS_ERR_OK  == mcast_hw_mcst_mbr_remove(pEntry, &delPmsk))
								p_igmp_stats->p_port_info[port].curBw -= imputedGrpBw;

                            LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, port);

		                    /* In SIP+DIP, although the portmask becomes empty,
		                        don't delete this entry from ASIC immediately */
		                    if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
		                    {
		                        pEntry->groupTimer = IGMP_TEMP_PERIOD;
		                    }

		                    pEntry = pEntry->next_subgroup;
						}
					}
                }
            }
        }
    }
    /* fast leave disable */
    else
    {
    	SYS_DBG(LOG_DBG_IGMP, "fast leave disable\n");
        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

        if (groupHead)
        {
			SYS_DBG(LOG_DBG_IGMP, "group is found\n");
            if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port) && groupHead->p_gsQueryCnt[port] == 0)
            {
            	 if (MODE_GPON != igmpCtrl.ponMode)
            	 {
                    if (IGMP_QUERIER == pQuerEntry->status)
                    {
                        mcast_send_gs_query(groupHead, port);
                    }
            	 }
            }
            /* This group is created by only Join */
            if (groupHead->sip == 0)
            {
                if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port) && groupHead->p_gsQueryCnt[port] == 0)
                {
                	 if (MODE_GPON != igmpCtrl.ponMode)
                	 {
	                    if (pQuerEntry->status == IGMP_QUERIER)
	                    {
	                        if (IGMP_GROUP_ENTRY_STATIC != groupHead->form)
	                        {
	                            groupHead->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;
	                            groupHead->p_gsQueryCnt[port]++;
	                        }
	                        //mcast_igmp_send_grp_specific_query(vid, dip, port);
	                    }
                	 }
                }
            }
            else
            {
            	/* v3 */
                pEntry = groupHead;
                while(pEntry)
                {
                    if (IGMP_GROUP_ENTRY_STATIC == pEntry->form)
                    {
                        pEntry = pEntry->next_subgroup;
                        continue;
                    }

                    if (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, port) && pEntry->p_gsQueryCnt[port] == 0)
                    {
                    	if (MODE_GPON != igmpCtrl.ponMode)
                    	{
	                        if (pQuerEntry->status == IGMP_QUERIER)
	                        {
	                            pEntry->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;
	                            pEntry->p_gsQueryCnt[port]++;
	                        }
                    	}
                    }
                    pEntry = pEntry->next_subgroup;
                }
            }
        }
    }
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

//TBD: count current bandwidth
int32 mcast_groupMbrPort_del_wrapper_dmacvid(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pAddr,  sys_logic_port_t port, ipAddr_t clientIp)
{
    int32                   ret;
    uint16                  sortedIdx, grpVid = 0;
    uint32                  dip;
    igmp_group_entry_t      *groupHead;
    igmp_aggregate_entry_t  *pAggrEntry;
    sys_logic_portmask_t    delPmsk;
    igmp_querier_entry_t    *pQuerEntry;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    igmp_group_entry_t      groupEntry;
    sys_l2McastEntry_t      l2McstEntry;
	int                     fastleave = 0;
	mcast_preview_info_t	previewInfo;
	uint32					aclEntryId = UINT_MAX, imputedGrpBw = 0;

	SYS_DBG(LOG_DBG_IGMP,
        "mcast_groupMbrPort_del_wrapper_dmacvid:vid %d group"IPADDR_PRINT" port %d\n", vid, IPADDR_PRINT_ARG(pAddr->ipv4), port);

	osal_memset(&previewInfo, 0, sizeof(mcast_preview_info_t));

    dip = (MULTICAST_TYPE_IPV4 == ipType ? (pAddr->ipv4) : (IPV6_TO_IPV4(pAddr->ipv6.addr)));

    //igmp filter
    SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_check(
        NULL, ipType, port, 0, pAddr, &aclEntryId, &grpVid, &imputedGrpBw, &previewInfo)), SYS_ERR_FAILED);

	SYS_DBG(LOG_DBG_IGMP, " %s() %d  aclEntryId=%x\n", __FUNCTION__, __LINE__, aclEntryId);

	if (MODE_GPON == igmpCtrl.ponMode)
	{
		if(pBuf)
		{
			SYS_ERR_CHK((SYS_ERR_OK != mcast_filter_treatment(pBuf, vid, port, aclEntryId, previewInfo)), SYS_ERR_FAILED);
			SYS_DBG(LOG_DBG_IGMP, " %s() %d  sid=%u\n", __FUNCTION__, __LINE__, pBuf->tx_tag.dst_port_mask);
		}
		vid = grpVid;
	}

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pAddr->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        mcast_vlan_db_get(vid, ipType, &pMcastVlan);

        if (!pMcastVlan)
    	{
    		IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_FAILED;
    	}

        mcast_querier_db_get(vid, ipType, &pQuerEntry);

        if (!pQuerEntry)
        {
            SYS_PRINTF("[IGMP]%s():%d VLAN-%d doesn't have query DB entry!\n", __FUNCTION__, __LINE__, vid);
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_FAILED;
        }
    }
    fastleave = mcast_fastLeaveMode_get(port);

    /* fast leave enable */
    if (IGMP_LEAVE_MODE_FAST == fastleave)
    {
        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

        SYS_DBG(LOG_DBG_IGMP,
            "fast leave enable, sortedIdx=%u, shortKey=%llu\n", sortedIdx, mcast_group_sortKey_ret(&groupEntry));

        if (groupHead)
        {
            /* group entry exist in sw db */
			SYS_DBG(LOG_DBG_IGMP, "%s() %d group group Head exist\n", __FUNCTION__, __LINE__);

            /* multiple dip aggregate to single mac */
            mcast_aggregate_db_get(groupHead, &pAggrEntry);
            if (!pAggrEntry)
            {
                /* conflict: dip entry exist, but mac entry not exist */
                SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d An existing group which has no aggregate record!\n", __FUNCTION__, __LINE__);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
            {
                if (MODE_GPON != igmpCtrl.ponMode)
                {
                    if (IGMP_QUERIER == pQuerEntry->status)
                    {
                        mcast_send_gs_query(groupHead, port);
                    }
                }

                if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
                {
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                }
                if (SYS_ERR_OK == mcast_host_tracking_del(groupHead, clientIp, (uint16)port))
                {
                	if (mcast_is_last_host_per_port(groupHead, (uint16)port))
                	{
		                /* Group */
		                groupHead->p_mbr_timer[port] = 0;
		                LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);
                        /* only remain this port */
		                if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
		                {
		                    if (gUnknown_mcast_action != IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
		                    {
		                        pAggrEntry->group_ref_cnt--;

		                        if (pAggrEntry->group_ref_cnt == 0)
		                        {
		                            /* no dip refer to this mac, so delete it */
		                            osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));
		                            l2McstEntry.vid = vid;
                                    l2McstEntry.care_vid = groupHead->care_vid;
		                            osal_memcpy(l2McstEntry.mac, groupHead->mac, MAC_ADDR_LEN);

		                            if (SYS_ERR_OK != (ret = rsd_l2McastEntry_del(&l2McstEntry)))
		                            {
		                                SYS_DBG(LOG_DBG_IGMP, "Deleting Mcst enrty failed!  ret:%d\n", ret);
		                            }
		                            else
		                            {
		                                p_igmp_stats->p_port_info[port].curBw -= imputedGrpBw;
		                                igmp_hw_entry_num--;
		                            }
		                            mcast_aggregate_db_del(groupHead);
		                        }
		                        else
		                        {
		                            /* still exist other dip refer to this mac */
		                            LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
		                            LOGIC_PORTMASK_SET_PORT(delPmsk, port);
		                            mcast_hw_mcst_mbr_remove(groupHead, &delPmsk);
		                        }
		                         mcast_group_del(groupHead);
		                    }
		                    else
		                    {
		                        groupHead->groupTimer = IGMP_TEMP_PERIOD;
		                    }
		                }
		                else
		                {
		                    /* still exist other port */
		                    LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
		                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);
		                    mcast_hw_mcst_mbr_remove(groupHead, &delPmsk);
		                }
                	}
                }
            }
        }
    }
    /* fast leave disable */
    else
    {
        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

        if (groupHead)
        {
            mcast_aggregate_db_get(groupHead, &pAggrEntry);
            /* dip entry exist in group_db, so mac entry should be exist in sw aggr. db */
            if (!pAggrEntry)
            {
                SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d An existing group which has no aggregate record!\n", __FUNCTION__, __LINE__);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            if (MODE_GPON != igmpCtrl.ponMode)
            {
                if (IGMP_QUERIER == pQuerEntry->status)
                {
                    if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
                        mcast_send_gs_query(groupHead, port);
                    else
                    {
                        if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port) && groupHead->p_gsQueryCnt[port] == 0)
                        {
                            groupHead->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;
                            groupHead->p_gsQueryCnt[port]++;
                            mcast_send_gs_query(groupHead, port);
                        }
                    }
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_port_entry_get(multicast_ipType_t ipType, sys_logic_port_t port, uint32 *pEntryNum)
{
    int32 i;
    igmp_group_entry_t  *pGroup_entry;
    uint32 entryCnt = 0;
    uint32 lastGroup = 0, lastVid = 0;
    uint16 lastIpv6Pre16 = 0;

    SYS_PARAM_CHK(NULL == pEntryNum, SYS_ERR_NULL_POINTER);

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup_entry = mcast_group_get_with_index(i);

        if (pGroup_entry)
        {
            if (ipType != pGroup_entry->ipType)
                continue;

            if (p_igmp_stats->oper_version < IGMP_VERSION_V3_FULL)
            {
                if (MULTICAST_TYPE_IPV4 == ipType)
                {
                    if (pGroup_entry->dip == lastGroup && lastVid == pGroup_entry->vid)
                        continue;
                }
                else
                {
                    if (pGroup_entry->dip == lastGroup && lastVid == pGroup_entry->vid &&
                         (lastIpv6Pre16 == *(uint16 *)(&pGroup_entry->dipv6.addr[0])))
                         continue;
                }
            }

            lastGroup = pGroup_entry->dip;
            lastVid = pGroup_entry->vid;
            if (MULTICAST_TYPE_IPV4 != ipType)
            {
                lastIpv6Pre16 = *(uint16 *)(&pGroup_entry->dipv6.addr[0]);
            }

            if (IGMP_GROUP_ENTRY_STATIC == pGroup_entry->form)
                continue;

            if (IS_LOGIC_PORTMASK_PORTSET(pGroup_entry->mbr, port) &&
                pGroup_entry->v3.p_mbr_fwd_flag[port] != IGMPV3_FWD_NOT)
            {
                entryCnt++;
            }
        }
    }

    *pEntryNum = entryCnt;

    return SYS_ERR_OK;

}

int32 mcast_suppreFlag_clear(sys_vid_t vid, multicast_ipType_t ipType)
{
    int32 i;
    igmp_group_entry_t  *pGroup_entry;

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup_entry = mcast_group_get_with_index(i);

        if (pGroup_entry)
        {
            if (ipType == pGroup_entry->ipType && vid == pGroup_entry->vid)
                pGroup_entry->suppreFlag = 0;
        }
    }
    return SYS_ERR_OK;
}

int32 mcast_group_portTime_update(
    multicast_ipType_t ipType , sys_vid_t vid, mcast_groupAddr_t *pAddr, uint16 srcNum, uint32 *pSrcList, uint32 lastMbrIntv)
{
    sys_logic_port_t port;
    mcast_vlan_entry_t *pMcastVlan = NULL;
    igmp_group_entry_t  *pGroupHead = NULL, *pEntry = NULL,  groupEntry, tmpGroupEntry;
    uint16 sortedIdx, i;
	uint32 lastMbrQueryCnt;
	uint32 gsQueryIntv;

    SYS_PARAM_CHK((NULL == pAddr), SYS_ERR_NULL_POINTER);

	SYS_DBG(LOG_DBG_IGMP, " %s() %d handle group member timer update \n", __FUNCTION__, __LINE__);

	if (MODE_GPON != igmpCtrl.ponMode)
	{
    	mcast_vlan_db_get(vid, ipType, &pMcastVlan);
    	SYS_ERR_CHK((NULL == pMcastVlan), SYS_ERR_OK);
		lastMbrQueryCnt = pMcastVlan->operLastMmbrQueryCnt;
		gsQueryIntv = pMcastVlan->operGsqueryIntv;
	}
	else
	{
		lastMbrQueryCnt = 1;
		gsQueryIntv = lastMbrIntv;
	}

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, pAddr->ipv4, pAddr->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);
    memcpy(&tmpGroupEntry,&groupEntry,sizeof(igmp_group_entry_t));
    /*For Group-specify query without source IP list, set sip to 0xffffffff for ignore source IP compare*/
    if(srcNum == 0)
        tmpGroupEntry.sip = 0xffffffff;
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroupEntry), &sortedIdx, &pGroupHead);

    SYS_ERR_CHK((NULL == pGroupHead && srcNum == 0), SYS_ERR_OK);

    if (IGMP_DIP_SIP_MODE == p_igmp_stats->lookup_mode || IGMP_DIP_VID_MODE == p_igmp_stats->lookup_mode)
    {
    	if (srcNum)
    	{
    		SYS_DBG(LOG_DBG_IGMP,
                " %s() %d handle GSQ, lastCnt=%u,  gsQueryIntv=%u,lastMbrIntv=%u \n",
                __FUNCTION__, __LINE__, lastMbrQueryCnt, gsQueryIntv, lastMbrIntv);

    		for (i = 0; i < srcNum; i++)
    		{
    			groupEntry.sip = *pSrcList;
                pEntry = mcast_group_get(&groupEntry);
				if (pEntry)
				{
					/* update src timer for actived group with group src per port*/
					FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
		            {
		            	if(IGMP_GROUP_ENTRY_DYNAMIC == pEntry->form)
	                    	pEntry->p_mbr_timer[port] = lastMbrQueryCnt * gsQueryIntv;
		            }
				}
				pSrcList++;
    		}
    	}
		else
		{
			SYS_DBG(LOG_DBG_IGMP,
                " %s() %d handle SQ of v2 or v3 lastCnt=%u,  gsQueryIntv=%u,lastMbrIntv=%u \n",
                __FUNCTION__, __LINE__, lastMbrQueryCnt, gsQueryIntv, lastMbrIntv);
			pEntry = pGroupHead;
			/* update group timer  for active group per port */
			igmp_group_head_entry_t *group_head_entry = NULL;
			mcast_igmp_group_head_get(pEntry->dip, pEntry->vid, &group_head_entry);

            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
            {
            	if (group_head_entry)
                	group_head_entry->p_mbr_timer[port] =  lastMbrQueryCnt * gsQueryIntv;
				if (IGMP_GROUP_ENTRY_DYNAMIC == pEntry->form)
					pEntry->p_mbr_timer[port] = lastMbrQueryCnt * gsQueryIntv;
            }
		}
    }
    else
   {
        if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
        {
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroupHead->mbr)
            {
                //if (pEntry->mbrTimer[port] > pMcastVlan->operLastMmbrQueryCnt * pMcastVlan->operGsqueryIntv)
                if(IGMP_GROUP_ENTRY_DYNAMIC == pGroupHead->form)
					pGroupHead->p_mbr_timer[port] =  lastMbrQueryCnt * gsQueryIntv;
            }
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_groupPort_remove(igmp_group_entry_t *pGroup, sys_logic_port_t port)
{
    int32 ret;
    sys_logic_portmask_t delPmsk, rtPmsk;

    SYS_PARAM_CHK((NULL == pGroup), SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

    LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
    LOGIC_PORTMASK_SET_PORT(delPmsk, port);

    LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);
    pGroup->p_mbr_timer[port] = 0;

    if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
    {
        ret = mcast_hw_mcst_entry_del(pGroup);

        if (ret == SYS_ERR_OK)
        {
            igmp_hw_entry_num--;
        }

		if (p_igmp_stats->oper_version == IGMP_VERSION_V3_FULL)
        	mcast_igmp_group_head_del(pGroup->dip, pGroup->vid);
        mcast_group_del(pGroup);
    }
    else
    {
        mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);

		if (SYS_ERR_OK != (ret = mcast_hw_mcst_mbr_remove(pGroup, &delPmsk)))
        {
            SYS_DBG(LOG_DBG_IGMP, "Deleting Mcst mbr failed!  ret:%d\n", ret);
        }

        if (IS_LOGIC_PORTMASK_PORTSET(rtPmsk, port))
        {
            LOGIC_PORTMASK_CLEAR_PORT(delPmsk, port);
        }

    }

    return SYS_ERR_OK;
}


int32 mcast_routerPortMbr_remove(multicast_ipType_t ipType, sys_vlanmask_t *pVlanMsk, sys_logic_portmask_t *pdelMbr)
{
    int32 i, ret;
    igmp_group_entry_t *pGroup = NULL;
    sys_l2McastEntry_t  l2Mcst_entry;
    sys_ipMcastEntry_t  ipMcst_entry;
    sys_logic_portmask_t  removePmsk;
    igmp_aggregate_entry_t *pAggrEntry = NULL;
    sys_logic_port_t port;


    if (IS_LOGIC_PORTMASK_CLEAR((*pdelMbr)))
        return SYS_ERR_OK;

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup = mcast_group_get_with_index(i);
        if ( NULL == pGroup)
            continue;

        if (!VLANMASK_IS_VIDSET(*pVlanMsk, pGroup->vid))
            continue;

        if (MULTICAST_TYPE_END != ipType)
        {
            if ( ipType != pGroup->ipType)
                continue;
        }

        osal_memset(&l2Mcst_entry, 0, sizeof(sys_l2McastEntry_t));
        osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));

        if ((MULTICAST_TYPE_IPV4 == ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            l2Mcst_entry.vid = pGroup->vid;
            l2Mcst_entry.care_vid = pGroup->care_vid;
            osal_memcpy(l2Mcst_entry.mac, pGroup->mac, MAC_ADDR_LEN);
            if (SYS_ERR_OK == rsd_l2McastEntry_get(&l2Mcst_entry))
            {
                //LOGIC_PORTMASK_ANDNOT(delPmsk, delPmsk, pGroup->mbr);   /* Make sure that router port is not belong to member port */
                LOGIC_PORTMASK_CLEAR_ALL(removePmsk);
                LOGIC_PORTMASK_COPY(removePmsk, *pdelMbr);

                mcast_aggregate_db_get(pGroup, &pAggrEntry);

                if (pAggrEntry == NULL)
                {
                    if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                    {
                       SYS_DBG(LOG_DBG_IGMP,
                            "%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDR_PRINT"\n",
                            __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pGroup->dip));
                    }
                    else
                    {
                       SYS_DBG(LOG_DBG_MLD,
                            "[Multicast]%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDRV6_PRINT"\n",
                            __FUNCTION__, __LINE__, IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));
                    }
                    return SYS_ERR_FAILED;
                }

                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, removePmsk)
                {
                    if (0 != pAggrEntry->p_port_ref_cnt[port])
                    {
                        LOGIC_PORTMASK_CLEAR_PORT(removePmsk, port);
                    }
                }

                LOGIC_PORTMASK_ANDNOT(l2Mcst_entry.portmask, l2Mcst_entry.portmask, removePmsk);
                 /* Make sure that router port is not belong to member port */
                LOGIC_PORTMASK_OR(l2Mcst_entry.portmask, l2Mcst_entry.portmask, pGroup->mbr);

                if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2Mcst_entry)))
                {
                    SYS_DBG(LOG_DBG_IGMP, "Delete router port, Set l2 mulitcast  hw failed!\n");
                    return ret;
                }
            }
        }
        else
        {
            if (pGroup->sip != 0 )
            {
                ipMcst_entry.vid = pGroup->vid;
                ipMcst_entry.sip = pGroup->sip;
                ipMcst_entry.dip = pGroup->dip;

                if (SYS_ERR_OK == rsd_ipMcastEntry_get(&ipMcst_entry))
                {
                    // LOGIC_PORTMASK_ANDNOT(delPmsk, delPmsk, pGroup->mbr);   /* Make sure that router port is not belong to member port */
                    LOGIC_PORTMASK_ANDNOT(ipMcst_entry.portmask, ipMcst_entry.portmask, *pdelMbr);
                    /* Make sure that router port is not belong to member port */
                    LOGIC_PORTMASK_OR(ipMcst_entry.portmask, ipMcst_entry.portmask, pGroup->mbr);

                    if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcst_entry)))
                    {
                        SYS_DBG(LOG_DBG_IGMP, "Delete router port, Set ip mulitcast  hw failed! \n");
                        return ret;
                    }
                }
            }
        }

    }

    return SYS_ERR_OK;
}

int32 mcast_group_learnByData_del(void)
{
    uint32 i = 0;
    uint32 ret = SYS_ERR_OK;
    igmp_group_entry_t *pEntry = NULL;
    uint32 tmpNum, dbSize;

    IGMP_DB_SEM_LOCK();
    dbSize = mcast_group_num;

    if (0 == dbSize)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_OK;
    }
    mcast_group_rx_timer_stop();

    for (i = dbSize ; i > 0 ; i--)
    {
        pEntry = mcast_group_get_with_index(i-1);

        if (NULL == pEntry)
            continue;

        if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
        {
            ret = mcast_hw_mcst_entry_del(pEntry);
            if (SYS_ERR_OK == ret)
            {
                igmp_hw_entry_num--;
            }
            tmpNum = mcast_group_num;
            mcast_group_del(pEntry);
            if (tmpNum != mcast_group_num)
                i++;
        }
    }

    mcast_group_rx_timer_start();
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_maxGroupAct_process(multicast_ipType_t ipType, sys_vid_t vid, uint32 dip, uint32 sip, sys_logic_port_t port)
{
    int32 i;
    igmp_group_entry_t  *pEntry;
    igmp_maxGroupAct_t act;
    uint16  limitNum;

    // if act is deny, do not process
    mcast_portGroup_excessAct_get(ipType, port, &act);

    if (IGMP_ACT_DENY == act)
    {
        if (MULTICAST_TYPE_IPV4 == ipType)
            SYS_DBG(LOG_DBG_IGMP, "The Port %d group is arrived max-group number.\n", port);
        else
            SYS_DBG(LOG_DBG_MLD, "The Port %d group is arrived max-group number.\n", port);

        return SYS_ERR_FAILED;
    }

    mcast_portGroup_limit_get(ipType,port, &limitNum);

    if (0 == limitNum)
        return SYS_ERR_FAILED;

    for (i = 0; i < mcast_group_num; i++)
    {
        pEntry = mcast_group_get_with_index(i);
        if (pEntry)
        {
            if (IGMP_GROUP_ENTRY_STATIC == pEntry->form)
                continue;

            if (ipType != pEntry->ipType)
                continue;

            if (pEntry->vid == vid && pEntry->dip == dip)
                continue;

            if (p_igmp_stats->oper_version < IGMP_VERSION_V3_FULL)
            {
                if (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, port))
                {
                    SYS_ERR_CHK((SYS_ERR_OK != mcast_groupPort_remove(pEntry, port)), SYS_ERR_FAILED);
                    break;
                }
            }
        }
    }

    return SYS_ERR_OK;

}


int32 mcast_igmp_subgroup_sip_search(igmp_group_entry_t *gipHead, uint32 key, igmp_group_entry_t **subgroup, igmp_group_entry_t **previous)
{
    igmp_group_entry_t  *entry;

    if (subgroup == NULL)
        return SYS_ERR_FAILED;

    entry = gipHead;
    while (entry)
    {
        if (entry->sip == key)
        {
            *subgroup = entry;
            return SYS_ERR_OK;
        }

        if (previous != NULL)
            *previous = entry;
        entry = entry->next_subgroup;
    }
    *subgroup = NULL;

    return SYS_ERR_OK;
}

int32 mcast_igmp_group_mcstData_add_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip, sys_logic_port_t port, ipAddr_t clientIp)
{
    int32                   ret;
    uint16                  sortedIdx, limitNum;
    uint32                  freeCount = 1, portEntryNum, dip, sip = 0;
    igmp_group_entry_t      *pEntry, *groupHead, newEntry, groupEntry;
    igmp_router_entry_t     *pRouterEntry;
    sys_ipMcastEntry_t      ipMcstEntry;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    sys_logic_port_t        lanPortIdx;

    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));
    osal_memset(&newEntry, 0, sizeof(igmp_group_entry_t));

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        dip = pGroupDip->ipv4;
        sip = pSip->ipv4;
    }
    else
    {
        dip = IPV6_TO_IPV4(pGroupDip->ipv6.addr);
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pGroupDip->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    if (groupHead)
    {
        /* This group is created by only Join */
        if (groupHead->sip == 0)
        {
            groupHead->sip = sip;

            /* Now, we can write to ASIC */
            mcast_router_db_get(vid, ipType, &pRouterEntry);
            if (pRouterEntry)
            {
                LOGIC_PORTMASK_OR(ipMcstEntry.portmask, pRouterEntry->router_pmsk, groupHead->mbr);
            }
            else
            {
                SYS_DBG(LOG_DBG_IGMP, "Warring! This vlan has no router port!\n");
                LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, groupHead->mbr);
            }
            ipMcstEntry.vid = vid;
            ipMcstEntry.dip = dip;
            ipMcstEntry.sip = sip;
            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until any one entry is released */
                //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                if (freeCount == 0)
                {
                    LgcPortFor(lanPortIdx)
                	{
                	    /*All LAN ports*/
                	    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                	}
                    igmp_trapDataPkt_lock = TRUE;
                }

                SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
            }
            else
            {
                groupHead->lookupTableFlag = L2TABLE_WROTE;
                igmp_hw_entry_num++;
                if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                {
                    LgcPortFor(lanPortIdx)
                	{
                	    /*All LAN ports*/
                	    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                	}
                    igmp_trapDataPkt_lock = TRUE;
                }
            }
        }
        else
        {
            /* This group has already created by Data (maybe and Join) */
            /* Check if this dip+sip group exists, if not, clone all subgroup's portmask */
            mcast_build_groupEntryKey(vid, ipType, dip, pGroupDip->ipv6.addr, &groupEntry);
            groupEntry.sip = sip;
            pEntry = mcast_group_get(&groupEntry);

            if (pEntry == NULL)
            {
                mcast_port_entry_get(ipType, port, &portEntryNum);
                mcast_portGroup_limit_get(ipType, port, &limitNum);
				if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
				{
	                if (portEntryNum >= limitNum)
	                {
	                    if (SYS_ERR_FAILED == mcast_maxGroupAct_process(ipType, vid, dip,0, port))
	                    {
	                        IGMP_DB_SEM_UNLOCK();
	                        return SYS_ERR_OK;
	                    }
	                }
				}
                newEntry.dip = dip;
                newEntry.sip = sip;
                newEntry.vid = vid;
                newEntry.ipType = ipType;

                if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
                    newEntry.form = IGMP_GROUP_ENTRY_STATIC;
                else
                	newEntry.form = IGMP_GROUP_ENTRY_DYNAMIC;

                if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
                    newEntry.groupTimer = pMcastVlan->grpMembershipIntv;

                osal_memcpy(&newEntry.mbr, &groupHead->mbr, sizeof(sys_logic_portmask_t));
                osal_memcpy(newEntry.mac, groupHead->mac, MAC_ADDR_LEN);

                ret = mcast_group_add(&newEntry, clientIp, (uint16)port, 0);

                if (ret)
                {
                    SYS_DBG(LOG_DBG_IGMP, "Failed add a group!  ret:%d\n", ret);
                }

                ipMcstEntry.vid = vid;
                ipMcstEntry.dip = dip;
                ipMcstEntry.sip = sip;

                osal_memcpy(&ipMcstEntry.portmask, &groupHead->mbr, sizeof(sys_logic_portmask_t));

                if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
                {
                    /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                    //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount); //set 1, allways is free;
                    if (freeCount == 0)
                    {
                        LgcPortFor(lanPortIdx)
                        {
                            /*All LAN ports*/
                            rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                        }
                        igmp_trapDataPkt_lock = TRUE;
                    }

                    SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                }
                else
                {
                    pEntry = mcast_group_get(&groupEntry);
                    if(pEntry != NULL)
                        pEntry->lookupTableFlag = L2TABLE_WROTE;

                    igmp_hw_entry_num++;
                    if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                    {
                        LgcPortFor(lanPortIdx)
                        {
                            /*All LAN ports*/
                            rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                        }
                        igmp_trapDataPkt_lock = TRUE;
                    }
                }
            }
        }
    }
    else
    {
        pEntry = mcast_group_freeEntry_get(ipType);
        if (pEntry != NULL)
        {
            pEntry->ipType = ipType;
            pEntry->form = IGMP_GROUP_ENTRY_DYNAMIC;
            pEntry->dip = dip;
            pEntry->sip = sip;
            pEntry->vid = vid;
            pEntry->sortKey = mcast_group_sortKey_ret(&groupEntry);
            pEntry->groupTimer = pMcastVlan->grpMembershipIntv;
            pEntry->mac[0] = 0x01;
            pEntry->mac[1] = 0x00;
            pEntry->mac[2] = 0x5e;
            pEntry->mac[3] = (dip >> 16) & 0x7f;
            pEntry->mac[4] = (dip >> 8) & 0xff;
            pEntry->mac[5] = dip & 0xff;
            //mcast_group_num++;


            ipMcstEntry.vid = vid;
            ipMcstEntry.dip = dip;
            ipMcstEntry.sip = sip;


            if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
            {
                LOGIC_PORTMASK_SET_ALL(ipMcstEntry.portmask);
            }
            else if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
            {
                mcast_router_db_get(vid, ipType, &pRouterEntry);
                if (pRouterEntry)
                    LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, pRouterEntry->router_pmsk);
            }

            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                if (freeCount == 0)
                {
                    LgcPortFor(lanPortIdx)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                }

                SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                mcast_group_entry_release(pEntry);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }
            else
            {
                pEntry->lookupTableFlag = L2TABLE_WROTE;

                igmp_hw_entry_num++;
                if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                {
                    LgcPortFor(lanPortIdx)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, lanPortIdx);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                }
            }

            mcast_group_num_cal(ipType,TRUE);
            mcast_group_sortedArray_ins(sortedIdx, pEntry);
        }
        else
        {
            IGMP_DB_SEM_UNLOCK();
            SYS_DBG(LOG_DBG_IGMP, "Group database is full!\n");
            return SYS_ERR_MCAST_DATABASE_FULL;
        }
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_group_mcstData_add_wrapper_dmacvid(
    multicast_ipType_t ipType,uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip, sys_logic_port_t port, ipAddr_t clientIp)
{
    uint16                  sortedIdx;
    int32                   ret;
//    uint32                  freeCount;
    igmp_group_entry_t      *pEntry, *groupHead, groupEntry;
    igmp_aggregate_entry_t  *pAggrEntry;
    sys_l2McastEntry_t      l2McstEntry;
    sys_logic_portmask_t    rtPmsk;
    uint32 portEntryNum;
    uint16 limitNum;
    uint32 dip;

    mcast_vlan_entry_t *pMcastVlan = NULL;

    osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));

    dip = ((MULTICAST_TYPE_IPV4 == ipType) ? pGroupDip->ipv4 : IPV6_TO_IPV4(pGroupDip->ipv6.addr));

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pGroupDip->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);

    if (NULL == pMcastVlan)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    if (groupHead)
    {
        if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
        {
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }
        l2McstEntry.vid = groupHead->vid;
        osal_memcpy(l2McstEntry.mac, groupHead->mac, MAC_ADDR_LEN);
        /* The repeated Mcst data */
        /* Read and write again seem nonsense, but indeed, it  clear the CPU port from portmask,
        cause logical portmask doesn't conatins CPU port */
        if (SYS_ERR_OK != rsd_l2McastEntry_get(&l2McstEntry))
            SYS_DBG(LOG_DBG_IGMP, "%s():%d  Failed to rsd_l2McastEntry_get\n", __FUNCTION__, __LINE__);

        if (SYS_ERR_OK != mcast_hw_l2McastEntry_set(ipType, &l2McstEntry))
            SYS_DBG(LOG_DBG_IGMP, "%s():%d  Failed to mcast_hw_l2McastEntry_set\n", __FUNCTION__, __LINE__);

        groupHead->groupTimer = pMcastVlan->grpMembershipIntv;
    }
    else
    {
        mcast_port_entry_get(ipType, port, &portEntryNum);
        mcast_portGroup_limit_get(ipType, port, &limitNum);

		if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
		{
	        if (portEntryNum >= limitNum)
	        {
	            if (SYS_ERR_FAILED == mcast_maxGroupAct_process(ipType,vid, dip,0, port))
	            {
	                IGMP_DB_SEM_UNLOCK();
	                return SYS_ERR_OK;
	            }
	        }
		}
        pEntry = mcast_group_freeEntry_get(ipType);
        if (pEntry != NULL)
        {
            /* Handle DIP -> MAC aggregation */
            mcast_aggregate_db_get(&groupEntry, &pAggrEntry);
            if (pAggrEntry == NULL)
            {
                if (SYS_ERR_OK != (ret = mcast_aggregate_db_add(&groupEntry, port)))
                {
                    SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ?  LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                    mcast_group_entry_release(pEntry);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_FAILED;
                }

                //the port must --;
                mcast_aggregate_db_get(&groupEntry, &pAggrEntry);

                if (NULL != pAggrEntry)
                {
                    pAggrEntry->p_port_ref_cnt[port]--;
                }
                else
                {
                    mcast_group_entry_release(pEntry);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_FAILED;
                }
            }
            else
            {
                /* This happens when another group with the same aggregation has existd */
                //pAggrEntry->group_ref_cnt++;
            }

            pEntry->ipType = ipType;
            pEntry->form = IGMP_GROUP_ENTRY_DYNAMIC;
            pEntry->dip = dip;
            pEntry->sip = 0;
            pEntry->vid = vid;
            pEntry->sortKey = mcast_group_sortKey_ret(&groupEntry);
            pEntry->groupTimer = pMcastVlan->grpMembershipIntv;

            if (MULTICAST_TYPE_IPV4 == ipType)
            {
                pEntry->mac[0] = 0x01;
                pEntry->mac[1] = 0x00;
                pEntry->mac[2] = 0x5e;
                pEntry->mac[3] = (dip >> 16) & 0x7f;
                pEntry->mac[4] = (dip >> 8) & 0xff;
                pEntry->mac[5] = dip & 0xff;
            }
            else
            {
                pEntry->mac[0] = 0x33;
                pEntry->mac[1] = 0x33;
                pEntry->mac[2] = (dip >> 24) & 0xff;
                pEntry->mac[3] = (dip >> 16) & 0xff;
                pEntry->mac[4] = (dip >> 8) & 0xff;
                pEntry->mac[5] = dip & 0xff;
                osal_memcpy(pEntry->dipv6.addr, pGroupDip->ipv6.addr, IPV6_ADDR_LEN);
            }

            /* Also add router portmask */
            mcast_routerPort_get(vid, ipType, &rtPmsk);
            LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);

            l2McstEntry.vid = vid;
            osal_memcpy(l2McstEntry.mac, pEntry->mac, MAC_ADDR_LEN);
            if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_add(ipType, &l2McstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
#if 0          //no need, check .
                rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);
                if (freeCount == 0)
                {
                    if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                        rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, ACTION_TRAP2CPU);
                    igmp_trapDataPkt_lock = TRUE;
                }
#endif

                if (SYS_ERR_L2_MAC_IS_EXIST == ret)
                {
                    if (NULL == pAggrEntry)
                    {
                        SYS_DBG(ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                            "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                        goto release;
                    }

                    if (SYS_ERR_OK != rsd_l2McastEntry_get(&l2McstEntry))
                        SYS_DBG(LOG_DBG_IGMP, "%s():%d  Failed to rsd_l2McastEntry_get\n", __FUNCTION__, __LINE__);

                    LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pEntry->mbr);

                    if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(ipType, &l2McstEntry)))
                    {
                        SYS_DBG(LOG_DBG_IGMP,
                            "Failed writing to ASIC!  ret:%d  vid:%d   mac:"MAC_PRINT"\n", ret, vid, MAC_PRINT_ARG(l2McstEntry.mac));
                        goto release;
                    }
                    else
                    {
                        if (SYS_ERR_OK == mcast_group_sortedArray_ins(sortedIdx, pEntry))
                        {
                            pAggrEntry->group_ref_cnt++;
                            pAggrEntry->p_port_ref_cnt[port]++;
                            mcast_group_num_cal(ipType,TRUE);
                        }
                        else
                        {
                            goto release;
                        }
                    }
                }
                else  //if (SYS_ERR_L2_MAC_FULL == ret)
                {
                    SYS_DBG(LOG_DBG_IGMP,
                            "Failed writing to ASIC! ret=%d, vid=%d, mac:"MAC_PRINT"\n", ret, vid, MAC_PRINT_ARG(pEntry->mac));
                   goto release;
                }
            }
            else
            {
                igmp_hw_entry_num++;
                mcast_group_num_cal(ipType,TRUE);
                mcast_group_sortedArray_ins(sortedIdx, pEntry);
            }

            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }
        else
        {
            IGMP_DB_SEM_UNLOCK();
            SYS_DBG(LOG_DBG_IGMP, "Group database is full!\n");
            return SYS_ERR_MCAST_DATABASE_FULL;
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;

release:
    mcast_group_entry_release(pEntry);
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_FAILED;

}

int32 mcast_igmpv3_ipMcstEntry_update_dipsip(uint16 vid, uint32 dip, uint32 sip, igmp_group_entry_t *pEntry)
{
    int32 ret;
    uint32                  freeCount = 1;
    igmp_router_entry_t     *pRouterEntry;
    sys_ipMcastEntry_t      ipMcstEntry;
    sys_logic_portmask_t   setMsk;
    sys_logic_port_t port;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;

    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));

    ipMcstEntry.vid = vid;
    ipMcstEntry.dip = dip;
    ipMcstEntry.sip = sip;
	ipMcstEntry.isGrpRefB = mcast_group_ref_get(pEntry);

    mcast_igmpv3_fwdPmsk_get(&setMsk, pEntry);

    mcast_router_db_get(vid, ipType, &pRouterEntry);
    if (pRouterEntry)
    {
        LOGIC_PORTMASK_OR(setMsk, pRouterEntry->router_pmsk, setMsk);
    }

    if (L2TABLE_NOT_WROTE == pEntry->lookupTableFlag)
    {
        if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_get(&ipMcstEntry)))
        {
            SYS_DBG(LOG_DBG_IGMP, "Failed read from ASIC!  ret:%d\n", ret);
        }

        LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, setMsk);

        if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
        {
            /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
            //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1,  allways is free;
            if (freeCount == 0)
            {
                LgcPortFor(port)
                {
                    /*All LAN ports*/
                    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, port);
                }
                igmp_trapDataPkt_lock = TRUE;
            }

            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
        }
        else
        {
            pEntry->lookupTableFlag = L2TABLE_WROTE;
            igmp_hw_entry_num++;
            if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
            {
                LgcPortFor(port)
                {
                    /*All LAN ports*/
                    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, port);
                }
                igmp_trapDataPkt_lock = TRUE;
            }
        }
    }
    else
    {
        if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcstEntry)))
        {
            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_group_mcstData_add_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip, sys_logic_port_t port, ipAddr_t clientIp)
{
    int32                   ret;
    uint32                  freeCount = 1, portEntryNum, dip, sip;
    uint16                  sortedIdx, limitNum;
    igmp_group_entry_t      *pEntry = NULL, *groupHead, newEntry, groupEntry;
    igmp_router_entry_t     *pRouterEntry;
    sys_ipMcastEntry_t      ipMcstEntry;
    sys_logic_port_t        portid;
    igmp_group_head_entry_t *group_head_entry;
    mcast_vlan_entry_t      *pMcastVlan = NULL;

    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));
    osal_memset(&newEntry, 0, sizeof(igmp_group_entry_t));

    newEntry.v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!newEntry.v3.p_mbr_fwd_flag), SYS_ERR_FAILED);
    osal_memset(newEntry.v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        dip = pGroupDip->ipv4;
        sip = pSip->ipv4;
    }
    else
    {
        // not suppport;
        osal_free(newEntry.v3.p_mbr_fwd_flag);
        return SYS_ERR_OK;
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, dip, pGroupDip->ipv6.addr, &groupEntry)), SYS_ERR_FAILED);
    groupEntry.sip = sip;

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
    if (NULL == pMcastVlan)
    {
        osal_free(newEntry.v3.p_mbr_fwd_flag);
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    if (groupHead)
    {
        if (groupHead->sip == 0)     /* This group is created by only Join */
        {
            mcast_router_db_get(vid, ipType, &pRouterEntry);
            if (pRouterEntry)
            {
                LOGIC_PORTMASK_OR(ipMcstEntry.portmask, pRouterEntry->router_pmsk, ipMcstEntry.portmask);
            }
            else
            {
                SYS_DBG(LOG_DBG_IGMP, "Warring! This vlan has no router port!\n");
            }

            if (IGMP_GROUP_ENTRY_DYNAMIC == groupHead->form)
            {
                groupHead->sip = sip;
                mcast_igmpv3_fwdPmsk_get(&ipMcstEntry.portmask,groupHead);
                            /* Now, we can write to ASIC */
            }
            else
            {
                pEntry = mcast_group_get(&groupEntry);
                if (pEntry == NULL)
                {
                    newEntry.ipType = ipType;
                    newEntry.form = IGMP_GROUP_ENTRY_STATIC;
                    newEntry.dip = dip;
                    newEntry.sip = sip;
                    newEntry.vid = vid;
                    newEntry.ipType = ipType;
                    newEntry.groupTimer = pMcastVlan->grpMembershipIntv;
                    osal_memcpy(newEntry.mac, groupHead->mac, MAC_ADDR_LEN);
                    osal_memcpy(&newEntry.mbr, &groupHead->mbr,sizeof(sys_logic_portmask_t));
                    ret = mcast_group_add(&newEntry, clientIp, (uint16)port, 0);
                    if (ret)
                        SYS_DBG(LOG_DBG_IGMP, "mcast_group_add FAIL  ret:%d\n", ret);

                    LOGIC_PORTMASK_OR(ipMcstEntry.portmask,ipMcstEntry.portmask,groupHead->mbr);
                }
            }

            ipMcstEntry.vid = vid;
            ipMcstEntry.dip = dip;
            ipMcstEntry.sip = sip;

            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until any one entry is released */
                //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                if (freeCount == 0)
                {
                    LgcPortFor(portid)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                }

                SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
            }
            else
            {
                if (IGMP_GROUP_ENTRY_DYNAMIC == groupHead->form)
                {
                    groupHead->lookupTableFlag = L2TABLE_WROTE;
                }
                else
                {
                    pEntry = mcast_group_get(&groupEntry);
                    if(pEntry != NULL)
                        pEntry->lookupTableFlag = L2TABLE_WROTE;
                }
                igmp_hw_entry_num++;
                if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                {
                    LgcPortFor(portid)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                }
            }
        }
        else                        /* This group has already created by Data (maybe and Join) */
        {
            /* Check if this dip+sip group exists, if not, clone all subgroup's portmask */
            pEntry = mcast_group_get(&groupEntry);

            if (IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
            {
                if (pEntry == NULL)
                {
                    mcast_port_entry_get(ipType, port, &portEntryNum);
                    mcast_portGroup_limit_get(ipType, port, &limitNum);

					if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
					{
	                    if (portEntryNum >= limitNum)
	                    {
	                        if (SYS_ERR_FAILED == mcast_maxGroupAct_process(ipType, vid, dip,0, port))
	                        {
                                osal_free(newEntry.v3.p_mbr_fwd_flag);
	                            IGMP_DB_SEM_UNLOCK();
	                            return SYS_ERR_OK;
	                        }
	                    }
					}
                    newEntry.dip = dip;
                    newEntry.sip = sip;
                    newEntry.vid = vid;
                    newEntry.ipType = ipType;

                    if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
                    {
                        newEntry.form = IGMP_GROUP_ENTRY_STATIC;
                        osal_memcpy(&newEntry.staticInMbr, &groupHead->staticInMbr, sizeof(sys_logic_portmask_t));
                        osal_memcpy(&newEntry.staticMbr, &groupHead->staticMbr, sizeof(sys_logic_portmask_t));
                    }
                    else
                    {
                        newEntry.form = IGMP_GROUP_ENTRY_DYNAMIC;
                        newEntry.groupTimer = pMcastVlan->grpMembershipIntv;
                    }

                    osal_memcpy(&newEntry.mbr, &groupHead->mbr, sizeof(sys_logic_portmask_t));

                    osal_memcpy(newEntry.mac, groupHead->mac, MAC_ADDR_LEN);

                    ret = mcast_group_add(&newEntry, clientIp, (uint16)port, 0);

                    if (ret)
                    {
                        SYS_DBG(LOG_DBG_IGMP, "Failed add a group!  ret:%d\n", ret);
                    }

                    ipMcstEntry.vid = vid;
                    ipMcstEntry.dip = dip;
                    ipMcstEntry.sip = sip;

                    osal_memcpy(&ipMcstEntry.portmask, &groupHead->mbr, sizeof(sys_logic_portmask_t));

                    if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
                    {
                        /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                        //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                        if (freeCount == 0)
                        {
                            LgcPortFor(portid)
                            {
                                /*All LAN ports*/
                                rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                            }
                            igmp_trapDataPkt_lock = TRUE;
                        }

                        SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                    }
                    else
                    {
                        pEntry = mcast_group_get(&groupEntry);
                        if (pEntry != NULL)
                            pEntry->lookupTableFlag = L2TABLE_WROTE;

                        igmp_hw_entry_num++;
                        if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                        {
                            LgcPortFor(portid)
                            {
                                /*All LAN ports*/
                                rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                            }
                            igmp_trapDataPkt_lock = TRUE;
                        }
                    }
                }
                else    // SYS_PRINTF("The port is include mode !\n");
                {
                    SYS_DBG(LOG_DBG_IGMP, "The entry is not null!\n");
                    //fix this in v3 full;
                    //mcast_igmpv3_ipMcstEntry_update_dipsip(vid,dip,sip,pEntry);
                }
            }
            else
            {
                mcast_igmp_group_head_get(dip, vid, &group_head_entry);
                if (group_head_entry != NULL)
                {
                    if (pEntry == NULL)
                    {
                        //All port is include mode port;
                        newEntry.ipType = ipType;
                        newEntry.dip = dip;
                        newEntry.sip = sip;
                        newEntry.vid = vid;
                        newEntry.form = IGMP_GROUP_ENTRY_DYNAMIC;

                        newEntry.groupTimer = pMcastVlan->grpMembershipIntv;
                        osal_memcpy(newEntry.mac, groupHead->mac, MAC_ADDR_LEN);

                                                    //All port is include mode port;
                        if(IS_LOGIC_PORTMASK_CLEAR(group_head_entry->fmode))
                        {
                            LOGIC_PORTMASK_CLEAR_ALL(ipMcstEntry.portmask);
                        }
                        else
                        {
                            //exclude mode port,
                            osal_memcpy(&ipMcstEntry.portmask, &group_head_entry->fmode, sizeof(sys_logic_portmask_t));

                            FOR_EACH_PORT_IN_LOGIC_PORTMASK(portid, group_head_entry->fmode)
                            {
                                LOGIC_PORTMASK_SET_PORT(newEntry.mbr, portid);
                                newEntry.v3.p_mbr_fwd_flag[portid] = IGMPV3_FWD_NORMAL;
                            }
                        }

                        ret = mcast_group_add(&newEntry, clientIp, (uint16)port, 0);
                        if (ret)
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Failed add a group!  ret:%d\n", ret);
                        }

                        ipMcstEntry.vid = vid;
                        ipMcstEntry.dip = dip;
                        ipMcstEntry.sip = sip;

                        if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
                        {
                            /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                            //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                            if (freeCount == 0)
                            {
                                LgcPortFor(portid)
                                {
                                    /*All LAN ports*/
                                    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                                }
                                igmp_trapDataPkt_lock = TRUE;
                            }

                            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                        }
                        else
                        {
                            pEntry = mcast_group_get(&groupEntry);
                            if(pEntry != NULL)
                                pEntry->lookupTableFlag = L2TABLE_WROTE;

                            igmp_hw_entry_num++;
                            if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                            {
                                LgcPortFor(portid)
                                {
                                    /*All LAN ports*/
                                    rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                                }
                                igmp_trapDataPkt_lock = TRUE;
                            }
                        }
                    }
                    else
                    {
                        mcast_igmpv3_ipMcstEntry_update_dipsip(vid,dip,sip,pEntry);
                    }
                }

            }
        }
    }
    else
    {
        pEntry = mcast_group_freeEntry_get(ipType);
        if (pEntry != NULL)
        {
            pEntry->ipType = ipType;
            pEntry->form = IGMP_GROUP_ENTRY_DYNAMIC;
            pEntry->dip = dip;
            pEntry->sip = sip;
            pEntry->vid = vid;
            pEntry->sortKey = mcast_group_sortKey_ret(&groupEntry);
            pEntry->groupTimer = pMcastVlan->grpMembershipIntv;
            pEntry->mac[0] = 0x01;
            pEntry->mac[1] = 0x00;
            pEntry->mac[2] = 0x5e;
            pEntry->mac[3] = (dip >> 16) & 0x7f;
            pEntry->mac[4] = (dip >> 8) & 0xff;
            pEntry->mac[5] = dip & 0xff;

            ipMcstEntry.vid = vid;
            ipMcstEntry.dip = dip;
            ipMcstEntry.sip = sip;

            LOGIC_PORTMASK_CLEAR_ALL(ipMcstEntry.portmask);

            if(IGMP_VERSION_V3_BASIC == p_igmp_stats->oper_version)
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
                {
                    LOGIC_PORTMASK_SET_ALL(ipMcstEntry.portmask);
                }
                else if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                {
                    mcast_router_db_get(vid, ipType, &pRouterEntry);
                    if (pRouterEntry)
                        LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, pRouterEntry->router_pmsk);
                }
            }
            else if(IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
            {
                mcast_igmp_group_head_ins(dip, vid, port, MODE_IS_INCLUDE,  &group_head_entry, 0);
            }

            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
                if (freeCount == 0)
                {
                    LgcPortFor(portid)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                    mcast_group_entry_release(pEntry);
                }
                SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
            }
            else
            {
                pEntry->lookupTableFlag = L2TABLE_WROTE;
                        //mcast_group_num++;
                mcast_group_num_cal(ipType,TRUE);
                mcast_group_sortedArray_ins(sortedIdx, pEntry);

                igmp_hw_entry_num++;
                if (igmp_hw_entry_num == SYS_MCAST_MAX_GROUP_NUM)
                {
                    LgcPortFor(portid)
                    {
                        /*All LAN ports*/
                        rsd_mcast_ctrl_and_data_to_cpu_enable_per_port_set(DISABLED, portid);
                    }
                    igmp_trapDataPkt_lock = TRUE;
                }
            }
        }
        else
        {
            osal_free(newEntry.v3.p_mbr_fwd_flag);
            IGMP_DB_SEM_UNLOCK();
            SYS_DBG(LOG_DBG_IGMP, "Group database is full!\n");
            return SYS_ERR_MCAST_DATABASE_FULL;
        }
    }
    osal_free(newEntry.v3.p_mbr_fwd_flag);

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_igmp_group_head_db_init(void)
{
    uint32 head_idx;
    p_group_head_db = (igmp_group_head_entry_t *)osal_alloc(sizeof(igmp_group_head_entry_t) * SYS_MCAST_MAX_GROUP_NUM);
    SYS_ERR_CHK((!p_group_head_db), SYS_ERR_FAILED);
    osal_memset(p_group_head_db, 0, sizeof(igmp_group_head_entry_t) * SYS_MCAST_MAX_GROUP_NUM);

    for (head_idx = 0; head_idx < SYS_MCAST_MAX_GROUP_NUM; head_idx++)
    {
        p_group_head_db[head_idx].p_mbr_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!p_group_head_db[head_idx].p_mbr_timer), SYS_ERR_FAILED);
        osal_memset(p_group_head_db[head_idx].p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
    }
    return SYS_ERR_OK;
}


int32 mcast_igmp_group_head_ins(uint32 groupAddr, uint16 vid, uint16 portid, uint16 rcdType, igmp_group_head_entry_t **ppGrpHead, uint32 timer)
{
    igmp_group_head_entry_t *pEntry = NULL;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t *pMcastVlan = NULL;

	if (MODE_GPON != igmpCtrl.ponMode)
	{
		mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    if (NULL == pMcastVlan)
	    {
	        return SYS_ERR_FAILED;
	    }
	}

    mcast_igmp_group_head_get(groupAddr, vid, &pEntry);
    if (pEntry)
        return SYS_ERR_OK;

    pEntry = mcast_igmp_free_group_head_get();
    if (pEntry)
    {
        osal_memset(&pEntry->fmode, 0, sizeof(sys_logic_portmask_t));
        /*set group entry filter mode*/
        if(rcdType == CHANGE_TO_EXCLUDE_MODE || rcdType == MODE_IS_EXCLUDE)
            PORT_SET_FILTER_EXCLUDE(pEntry->fmode, portid);
        else if(rcdType == CHANGE_TO_INCLUDE_MODE || rcdType == MODE_IS_INCLUDE)
            PORT_SET_FILTER_INCLUDE(pEntry->fmode, portid);

        pEntry->ipType = ipType;
        pEntry->dip = groupAddr;
        pEntry->sip = 0;
        pEntry->vid = vid;

        pEntry->mac[0] = 0x01;
        pEntry->mac[1] = 0x00;
        pEntry->mac[2] = 0x5e;
        pEntry->mac[3] = (groupAddr >> 16) & 0xff;
        pEntry->mac[4] = (groupAddr >> 8) & 0xff;
        pEntry->mac[5] = groupAddr & 0xff;

		if (MODE_GPON != igmpCtrl.ponMode)
			pEntry->p_mbr_timer[portid] = pMcastVlan->grpMembershipIntv;
		else
			pEntry->p_mbr_timer[portid] = timer;//DFLT_IGMP_GROUP_MEMBERSHIP_INTV

        *ppGrpHead = pEntry;
    }
    else
    {
        SYS_DBG(LOG_DBG_IGMP, "Group head database is full!\n");
        *ppGrpHead = NULL;
        return SYS_ERR_FAILED;
    }

    return SYS_ERR_OK;
}

igmp_group_head_entry_t *mcast_igmp_group_head_get(uint32 groupAddr, uint16 vid, igmp_group_head_entry_t **pGrpHead)
{
    int i;
    igmp_group_head_entry_t  *pEntry = NULL;

    for (i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        if (p_group_head_db[i].dip == groupAddr && (MODE_GPON == igmpCtrl.ponMode || p_group_head_db[i].vid == vid))
        {
            pEntry = &p_group_head_db[i];
            break;
        }
    }
    *pGrpHead = pEntry;
    return pEntry;
}


int32 mcast_igmp_group_head_del(uint32 groupAddr,uint16 vid)
{
    int i;

    for (i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        if (p_group_head_db[i].dip == groupAddr && (MODE_GPON == igmpCtrl.ponMode || p_group_head_db[i].vid == vid))
        {
            p_group_head_db[i].dip = 0;
            p_group_head_db[i].vid = 0;
            osal_memset(p_group_head_db[i].p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
            break;
        }
    }

    return SYS_ERR_OK;
}

igmp_group_head_entry_t *mcast_igmp_free_group_head_get(void)
{
    int i;
    igmp_group_head_entry_t  *pEntry = NULL;

    for (i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        if (p_group_head_db[i].dip == 0)
        {
            pEntry = &p_group_head_db[i];
            break;
        }
    }

    return pEntry;
}

int32 mcast_igmpv3_source_add_wrapper(
    ipAddr_t clientIp, uint32 groupAddr, uint32 sourceAddr, uint16 vid, uint16 portid,
    uint32 timer, uint16 fwdFlag,  mcast_preview_info_t	previewInfo)
{
    uint32 ret;
    sys_ipMcastEntry_t      ipMcstEntry;
    igmp_group_entry_t    newEntry, *pEntry = NULL;
    uint8  portFlag = FALSE;
    uint32 portEntryNum;
    uint16 limitNum;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;

    mcast_port_entry_get( ipType, portid, &portEntryNum);
    mcast_portGroup_limit_get(ipType, portid, &limitNum);

    osal_memset(&ipMcstEntry, 0 , sizeof(sys_ipMcastEntry_t));
    osal_memset(&newEntry, 0 , sizeof(igmp_group_entry_t));
    newEntry.dip = groupAddr;
    newEntry.sip = sourceAddr;
    newEntry.vid = vid;
    newEntry.ipType = ipType;

    pEntry = mcast_group_get(&newEntry);//group_dip and group_sip need match

    if (pEntry == NULL)
    {
    	if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
    	{
	        if (portEntryNum >= limitNum)
	        {
	            SYS_DBG(LOG_DBG_IGMP, "The Port %d group entry is full \n", portid);
	            return SYS_ERR_OK;
	        }
    	}
        osal_memset(&newEntry, 0 , sizeof(igmp_group_entry_t));
        SYS_DBG(LOG_DBG_IGMP, "add new group entry  ip is : "IPADDR_PRINT" \n",  IPADDR_PRINT_ARG(sourceAddr));
        newEntry.dip = groupAddr;
        newEntry.sip = sourceAddr;
        newEntry.vid = vid;
        newEntry.ipType = ipType;
        newEntry.lookupTableFlag = ((MODE_GPON == igmpCtrl.ponMode) ? L2TABLE_WROTE : L2TABLE_NOT_WROTE);

        LOGIC_PORTMASK_SET_PORT(newEntry.mbr, portid);

        newEntry.mac[0] = 0x01;
        newEntry.mac[1] = 0x00;
        newEntry.mac[2] = 0x5e;
        newEntry.mac[3] = (groupAddr >> 16) & 0xff;
        newEntry.mac[4] = (groupAddr >> 8) & 0xff;
        newEntry.mac[5] = groupAddr & 0xff;

        newEntry.v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
        SYS_ERR_CHK((!newEntry.v3.p_mbr_fwd_flag), SYS_ERR_FAILED);
        osal_memset(newEntry.v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

        newEntry.v3.p_mbr_fwd_flag[portid] = fwdFlag;

		switch(previewInfo.previewType)
		{
			case MCAST_NON_PREVIEW:
			case MCAST_PREVIEW:
				newEntry.form = IGMP_GROUP_ENTRY_DYNAMIC;
				break;
			case MCAST_ALLOWED_PREVIEW:
				newEntry.form = (previewInfo.length == 0 ? IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED : IGMP_GROUP_ENTRY_ALLOWED_PREVIEW);
				break;
			default:
				printf("Not support preview Type\n");
		}

        ret = mcast_group_add(&newEntry, clientIp, portid, timer);
        if (ret)
        {
            SYS_DBG(LOG_DBG_IGMP, "Failed add a group!  ret:%d\n", ret);
            osal_free(newEntry.v3.p_mbr_fwd_flag);
            return ret;
        }
        osal_free(newEntry.v3.p_mbr_fwd_flag);
    }
    else /*the entry is exist*/
    {
        SYS_DBG(LOG_DBG_IGMP, "the group entry is exist !\n");

        if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
        {
            return SYS_ERR_OK;
        }

        if (!IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
        {
            if ((limitNum != 0 &&  MODE_GPON == igmpCtrl.ponMode) || MODE_GPON != igmpCtrl.ponMode)
            {
                SYS_DBG(LOG_DBG_IGMP, "The Port %d group entry is full \n", portid);
                return SYS_ERR_OK;
            }
            LOGIC_PORTMASK_SET_PORT(pEntry->mbr, portid);
            portFlag = TRUE;
        }
		if (MCAST_ALLOWED_PREVIEW != previewInfo.previewType || (0 == pEntry->p_mbr_timer[portid]))
        	pEntry->p_mbr_timer[portid] =  timer;
        /*Delete the the port if the group entry is not FWD_NOT.*/
        /*save the info for the FWD_NOT port*/
        if (PORT_IS_SRC_QUERY(pEntry->v3.isGssQueryFlag,portid))
        {
            PORT_SET_SRC_NOT_QUERY(pEntry->v3.isGssQueryFlag,portid);
        }

        pEntry->v3.p_mbr_fwd_flag[portid] =  fwdFlag;

		switch (pEntry->form)
		{
			case IGMP_GROUP_ENTRY_DYNAMIC:
			case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW:
			case IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED:
				if (!mcast_host_tracking_find(portid, pEntry, clientIp))
				{
					//insert clientIp to clientIp List
					if(SYS_ERR_FAILED == mcast_host_tracking_add(pEntry, clientIp, portid))
					{
						SYS_DBG(LOG_DBG_IGMP, "add client ip failed\n");
					}
				}
				portFlag = TRUE;
				break;
			default:
				SYS_DBG(LOG_DBG_IGMP, "Not support group type\n");
		}

        if (pEntry->lookupTableFlag == L2TABLE_WROTE && portFlag)
        {
            ipMcstEntry.vid = vid;
            ipMcstEntry.sip = pEntry->sip;
            ipMcstEntry.dip = pEntry->dip;
			ipMcstEntry.isGrpRefB = mcast_group_ref_get(pEntry);
			if (MODE_GPON == igmpCtrl.ponMode)
			{
				LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, pEntry->mbr);
	        	LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, pEntry->fmode);
				if(IGMPV3_FWD_NOT == fwdFlag)
					LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, portid);	//igmp_TB set mbr but L2_TB mbr is none
				else if(IGMPV3_FWD_CONFLICT == fwdFlag)
					LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, portid);//igmp_TB mbr is none but L2_TB set mbr
			}
			else
			{
				mcast_igmpv3_fwdPmsk_get(&ipMcstEntry.portmask, pEntry);
			}
            if(SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcstEntry)))
            {
                SYS_DBG(LOG_DBG_IGMP, "Failed get from ASIC!  ret:%d\n", ret);
                return SYS_ERR_FAILED;
            }
        }

    }

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_source_is_inPkt(igmp_group_entry_t *pEntry, uint16 portid, uint32 *pSrcArray, uint32 srcNum)
{
    uint32 i;
    uint32 *ptr = pSrcArray;

    SYS_PARAM_CHK(NULL == pEntry, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(NULL == pSrcArray, SYS_ERR_NULL_POINTER);

    for (i = 0; i < srcNum; i++)
    {
        if ((pEntry->sip == *ptr) && (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid)))
            return TRUE;
        ptr++;
    }

    return FALSE;
}

int32 mcast_igmpv3_fwdPmsk_get(sys_logic_portmask_t *pPortMsk,  igmp_group_entry_t *group)
{
    sys_logic_portmask_t pmsk;
    uint16 port;

    SYS_PARAM_CHK((pPortMsk == NULL || group == NULL), SYS_ERR_FAILED);

    osal_memset(&pmsk, 0, sizeof(sys_logic_portmask_t));

    FOR_EACH_LOGIC_PORT(port)
    {
        if (IS_LOGIC_PORTMASK_PORTSET(group->mbr, port) && group->v3.p_mbr_fwd_flag[port] != IGMPV3_FWD_NOT)
        {
            LOGIC_PORTMASK_SET_PORT(pmsk, port);
        }
    }

    osal_memcpy(pPortMsk, &pmsk, sizeof(sys_logic_portmask_t));
    return SYS_ERR_OK;
}

int32 mcast_igmpv3_groupMbrPort_isIn_proc(
    igmpv3_grec_t *pRcd, uint16 vid,  uint16 portid, ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t previewInfo)
{
    uint32 groupAddr = 0, dip;
    uint16 numSrc = 0, i;
    uint32 *sourceAddr = NULL;
	uint32 grpIntv;

    igmp_group_head_entry_t *group_head_entry;
    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t *pMcastVlan = NULL;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    groupAddr = ntohl(pRcd->grec_mca);
    numSrc = ((!pRcd) ? 0 : ntohl(pRcd->grec_nsrcs));
    sourceAddr =  pRcd->grec_src;
    dip = groupAddr;

    IGMP_DB_SEM_LOCK();

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    if (NULL == pMcastVlan)
	    {
	        IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }
		grpIntv = pMcastVlan->grpMembershipIntv;
	}
	else
	{
		grpIntv = grpMbrTimerIntv;
	}

    if (numSrc != 0)
    {
        mcast_igmp_group_head_get(groupAddr, vid, &group_head_entry);

        if (group_head_entry)
        {
			if (IS_LOGIC_PORTMASK_CLEAR(group_head_entry->fmode))
            {
                SYS_DBG(LOG_DBG_IGMP, "%s() %d is include mode \n", __FUNCTION__, __LINE__);
                /*process the new report source*/
                for (i = 0; i < numSrc; i++)
                {
                    mcast_igmpv3_source_add_wrapper(clientIp, dip,*sourceAddr, vid, portid, grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
                    sourceAddr++;
                }
            }
            else
            {
                SYS_DBG(LOG_DBG_IGMP, "%s() %d is exlude mode \n", __FUNCTION__, __LINE__);
                /*process the exclude mode,  EXCLUDE(X+A, Y-A)*/
                SYS_DBG(LOG_DBG_IGMP, "proc X+A, and A = GMI \n");

				if (MODE_GPON != igmpCtrl.ponMode)
					group_head_entry->p_mbr_timer[portid] = grpIntv;

                for(i = 0; i < numSrc; i++)
                {
                    mcast_igmpv3_source_add_wrapper(clientIp, dip,*sourceAddr, vid,portid,
                        grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
                    sourceAddr++;
                }
            }
        }
        else  /*the group head is not exist*/
        {
            SYS_DBG(LOG_DBG_IGMP, "Group head is not exist !\n");
            mcast_igmp_group_head_ins(groupAddr, vid, portid, MODE_IS_INCLUDE, &group_head_entry, grpIntv);

            if(group_head_entry != NULL)
            {
                for(i = 0; i < numSrc; i++)
                {
                    SYS_DBG(LOG_DBG_IGMP, "the source ip is : "IPADDR_PRINT" \n", IPADDR_PRINT_ARG(*sourceAddr));
                    mcast_igmpv3_source_add_wrapper(clientIp, groupAddr, *sourceAddr, vid, portid,
                        grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
                    sourceAddr++;
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_igmpv3_groupMbrPort_isEx_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, uint16 rcdType,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t	previewInfo)
{
    uint32 grpAddr = 0;
    uint16 srcNum = 0, i;
    uint16 sortedIdx;
    uint32  *pSourceAddr = NULL, *sourceArray = NULL;
    uint32  sipArray[32], numSip = 0;
	uint32 grpIntv;
	int32 tmpNum;


    igmp_group_entry_t  *pEntry = NULL, *groupHead, groupEntry;
    igmp_querier_entry_t *pQueryEntry = NULL;
    igmp_group_head_entry_t  *group_head_entry = NULL;

    sys_logic_portmask_t delPmsk;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t *pMcastVlan = NULL;


    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    grpAddr = ntohl(pRcd->grec_mca);
    srcNum = ((!pRcd) ? 0 : ntohl(pRcd->grec_nsrcs));
    pSourceAddr =  pRcd->grec_src;
    sourceArray = pSourceAddr;
	SYS_DBG(LOG_DBG_IGMP, "%s() %d  srcNum=%u\n", __FUNCTION__, __LINE__, srcNum);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, grpAddr, NULL, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    if (NULL == pMcastVlan)
	    {
	        IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }
		grpIntv = pMcastVlan->grpMembershipIntv;
	}
	else
	{
		grpIntv = grpMbrTimerIntv;
	}

    osal_memset(sipArray,0,sizeof(sipArray));

    mcast_igmp_group_head_get(grpAddr,vid, &group_head_entry);

	SYS_DBG(LOG_DBG_IGMP, "%s() %d  \n", __FUNCTION__, __LINE__);

    if (srcNum == 0)
    {
        //if(CHANGE_TO_EXCLUDE_MODE == rcdType)
        {
            SYS_DBG(LOG_DBG_IGMP, "no pkt src, message join \n");
			#if 1 // v2 and v3 doesn't exist together, RTK_L2_IPMCAST_FLAG_DIP_ONLY can snoop more entries; the related setting in pf_mac.c
            if (SYS_ERR_OK != mcast_igmp_group_head_ins(grpAddr, vid, portid, MODE_IS_INCLUDE, &group_head_entry, grpIntv))
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }
            if (group_head_entry != NULL)
            {
                mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, 0, vid, portid,
                   	grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
            }
			#else
			/* v3: {EX_NULL} <=>  v2; but this method snoop less entries */
            if(SYS_ERR_OK != mcast_igmp_group_head_ins(grpAddr, vid, portid, MODE_IS_EXCLUDE, &group_head_entry))
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }
            if(group_head_entry != NULL)
            {
                mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, 0, vid, portid,
                   	grpIntv,
                    IGMPV3_FWD_NOT);
            }
			#endif
        }
    }
    else
    {
        if (group_head_entry == NULL)
        {
            SYS_DBG(LOG_DBG_IGMP, "the group head not exist \n");

            if (SYS_ERR_OK != mcast_igmp_group_head_ins(grpAddr, vid, portid, MODE_IS_EXCLUDE, &group_head_entry, grpIntv))
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            if (group_head_entry != NULL)
            {
                for (i = 0; i < srcNum; i++)
                {
                    mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
                        grpIntv, IGMPV3_FWD_NOT, previewInfo);
                    pSourceAddr++;
                }
            }
        }
        else
        {
            SYS_DBG(LOG_DBG_IGMP, "the group head exist \n");

            group_head_entry->p_mbr_timer[portid] = grpIntv;

            mcast_querier_db_get(vid, ipType, &pQueryEntry);

            mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

            if (IS_LOGIC_PORTMASK_CLEAR(group_head_entry->fmode))
            {
                /*include mode*/
                SYS_DBG(LOG_DBG_IGMP, "group is include mode \n");
                /*change to exclude mode*/
                PORT_SET_FILTER_EXCLUDE(group_head_entry->fmode, portid);

                SYS_DBG(LOG_DBG_IGMP, "handle src in database, not pkt  --(A-B)\n");

				for (i = 0; i < group_sortedAry_entry_num; i++)
				{
	                pEntry = pp_group_sorted_array[i];
	                while (pEntry != NULL)
	                {
	                	if (pEntry->dip != grpAddr)
	                	{
	                		pEntry = pEntry->next_subgroup;
							continue;
	                	}

	                    if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
	                    {
	                        pEntry = pEntry->next_subgroup;
	                        continue;
	                    }
	                   	PORT_SET_FILTER_EXCLUDE(pEntry->fmode, portid);
						//check srcList of database(pEntry) are in receive v3 report with multiple group_src or not
						//if fast leave enable and srcList of DB are not in new src list, delete ASIC entry based on srclist of DB
	                    if (mcast_igmpv3_source_is_inPkt(pEntry, portid, sourceArray, srcNum) == TRUE)
	                    {
	                    	/*(A*B) update GMI  */
							SYS_DBG(LOG_DBG_IGMP,
							    "%s() %d check srcList of database(pEntry) are in receive v3 report ! grp_src="IPADDR_PRINT"!\n",
							    __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
							if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
								(MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
                            {
                            	if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
                            	{
									SYS_DBG(LOG_DBG_IGMP, "mcast_host_tracking_del ok \n");

                                    if (mcast_is_last_host_per_port(pEntry, portid))
                            		{
										SYS_DBG(LOG_DBG_IGMP, "mcast_is_last_host_per_port \n");

                                        pEntry->p_mbr_timer[portid] = 0;

		                                if (pEntry->lookupTableFlag == L2TABLE_WROTE)
		                                {
		                                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
		                                    LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
		                                    mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
											LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
		                                }

		                                if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
		                                {
		                                    if (MODE_GPON == igmpCtrl.ponMode)
			                            	{
												SYS_DBG(LOG_DBG_IGMP,
                                                    "%s() %d sortKey=%llu\n", __FUNCTION__, __LINE__, mcast_group_sortKey_ret(pEntry));
												SYS_DBG(LOG_DBG_IGMP,
                                                    "%s() %d grp_src="IPADDR_PRINT"!\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
												tmpNum =  group_sortedAry_entry_num;
												mcast_group_del(pEntry);
												if (tmpNum != group_sortedAry_entry_num)
													i--;
			                            	}
											else
											{
			                                	pEntry->groupTimer = grpIntv;
											}
		                                }
                            		}
									else
									{
										if ((pEntry->v3.p_mbr_fwd_flag[portid] != IGMPV3_FWD_NORMAL ) &&
				                            (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid)))
				                        {
				                            pEntry->p_mbr_timer[portid] = grpIntv;
				                            pEntry->v3.p_mbr_fwd_flag[portid] = IGMPV3_FWD_NORMAL;
											SYS_DBG(LOG_DBG_IGMP, "mcast_igmpv3_source_is_inPkt == FALSE  && IGMPV3_FWD_NORMAL\n");
				                        }
									}
                            	}
                            }
							else
							{
		                        if ((pEntry->v3.p_mbr_fwd_flag[portid] != IGMPV3_FWD_NORMAL ) &&
		                            (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid)))
		                        {
		                            pEntry->p_mbr_timer[portid] = grpIntv;
		                            pEntry->v3.p_mbr_fwd_flag[portid] = IGMPV3_FWD_NORMAL;
									SYS_DBG(LOG_DBG_IGMP, "mcast_igmpv3_source_is_inPkt == FALSE  && IGMPV3_FWD_NORMAL\n");
		                        }
							}
	                    }
	                    else
	                    {
	                    	/*(A-B) delete entry */
							SYS_DBG(LOG_DBG_IGMP,
							    "%s() %d check srcList of database(pEntry) are NOT in receive v3 report ! grp_src="IPADDR_PRINT"!\n",
							    __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
	                        pEntry->p_mbr_timer[portid] = grpIntv;
	                        pEntry->v3.p_mbr_fwd_flag[portid] = IGMPV3_FWD_CONFLICT;

	                        if (rcdType == CHANGE_TO_EXCLUDE_MODE)
	                        {
	                            if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
									(MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
	                            {
	                            	if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
	                            	{
										SYS_DBG(LOG_DBG_IGMP, "mcast_host_tracking_del ok \n");
	                            		if (mcast_is_last_host_per_port(pEntry, portid))
	                            		{
											SYS_DBG(LOG_DBG_IGMP, "mcast_is_last_host_per_port \n");
	                         	            pEntry->p_mbr_timer[portid] = 0;

			                                if(pEntry->lookupTableFlag == L2TABLE_WROTE)
			                                {
			                                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
			                                    LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
			                                    mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
												LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
			                                }

			                                if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
			                                {
			                                    if (MODE_GPON == igmpCtrl.ponMode)
				                            	{
													SYS_DBG(LOG_DBG_IGMP, "%s() %d sortKey=%llu\n", __FUNCTION__, __LINE__,
                                                        mcast_group_sortKey_ret(pEntry));
													SYS_DBG(LOG_DBG_IGMP, "%s() %d grp_src="IPADDR_PRINT"!\n", __FUNCTION__,
                                                        __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
													tmpNum =  group_sortedAry_entry_num;
													mcast_group_del(pEntry);
													if (tmpNum != group_sortedAry_entry_num)
														i--;
				                            	}
												else
												{
				                                	pEntry->groupTimer = grpIntv;
												}
			                                }
	                            		}
	                            	}
	                            }
	                            else
	                            {
	                                sipArray[numSip++] = pEntry->sip;
	                                PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
	                            }
	                        }
	                    }
	                    pEntry = pEntry->next_subgroup;
	                }
				}

                SYS_DBG(LOG_DBG_IGMP, "handle pkt src, not database --(B-A) \n");
                //(B-A) block
                for (i = 0; i < srcNum; i++)
                {
                    groupEntry.sip = *pSourceAddr;
                    pEntry = mcast_group_get(&groupEntry);
                    if (pEntry == NULL)
                        mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
                        	grpIntv, IGMPV3_FWD_NOT, previewInfo);
                    else
                    {
						SYS_DBG(LOG_DBG_IGMP, "mcast_group_get entry exist \n");
                        if (!IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
                            mcast_igmpv3_source_add_wrapper(clientIp, pEntry->dip, *pSourceAddr, vid, portid,
                            grpIntv, IGMPV3_FWD_NOT, previewInfo);

                        if (pEntry->v3.p_mbr_fwd_flag[portid] == IGMPV3_FWD_NORMAL)
                            pEntry->v3.p_mbr_fwd_flag[portid] = IGMPV3_FWD_CONFLICT;
                    }

                    pSourceAddr++;
                }
            }
            else
            {
                /*exclude mode*/
                SYS_DBG(LOG_DBG_IGMP, "group is exclude mode \n");
                //PORT_SET_FILTER_EXCLUDE(group_head_entry->fmode, portid);
                /*Process EXCLUDE(A-Y, Y*A),  (A-X-Y) = GMI)*/
                //pEntry = groupHead;
                /*updata group head timer*/

                group_head_entry->p_mbr_timer[portid] =  grpIntv;

				SYS_DBG(LOG_DBG_IGMP, "%s() %d Delete(X-A), (Y-A) \n", __FUNCTION__, __LINE__);
				/*Delete(X-A), (Y-A)*/
				for (i = 0; i < group_sortedAry_entry_num; i++)
				{
					pEntry = pp_group_sorted_array[i];
					//pEntry = groupHead;
					while (pEntry != NULL)
					{
						if (pEntry->dip != grpAddr)
						{
							pEntry = pEntry->next_subgroup;
							continue;
						}
						if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
						{
							pEntry = pEntry->next_subgroup;
							continue;
						}
						if (mcast_igmpv3_source_is_inPkt(pEntry, portid, sourceArray,srcNum) == FALSE)
						{
        					#if 0
								if((pEntry->v3.mbrFwdFlag[portid] != IGMPV3_FWD_NORMAL ) &&
									(IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid)))
								{
									SYS_DBG(LOG_DBG_IGMP, "Group Address of IP = "IPADDR_PRINT", Src IP = "IPADDR_PRINT"\n",
										IPADDR_PRINT_ARG(pEntry->dip),
										IPADDR_PRINT_ARG(pEntry->sip));

									pEntry->mbrTimer[portid] = grpIntv;
									pEntry->v3.mbrFwdFlag[portid] = IGMPV3_FWD_NORMAL;
								}
							#else
								if (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
								{
									SYS_DBG(LOG_DBG_IGMP, "%s() %d port %u is in mbr port \n", __FUNCTION__, __LINE__, portid);
									if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
									{
										if (mcast_is_last_host_per_port(pEntry, portid))
										{
											pEntry->p_mbr_timer[portid] = 0;


											if (pEntry->lookupTableFlag == L2TABLE_WROTE)
											{
												osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
												LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
												mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
												LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
											}

											if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
											{
												SYS_DBG(LOG_DBG_IGMP,
                                                    "%s() %d sortKey=%llu\n", __FUNCTION__, __LINE__, mcast_group_sortKey_ret(pEntry));
												SYS_DBG(LOG_DBG_IGMP,
                                                    "%s() %d grp_src="IPADDR_PRINT"!\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
												tmpNum =  group_sortedAry_entry_num;
												mcast_group_del(pEntry);
												if (tmpNum != group_sortedAry_entry_num)
													i--;
											}
										}
									}
									else
									{
										sipArray[numSip++] = pEntry->sip;
										PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
									}
								}
								else
								{
									SYS_DBG(LOG_DBG_IGMP, "%s() %d port %u is NOT in mbr port \n", __FUNCTION__, __LINE__, portid);
									sys_ipMcastEntry_t      ipMcstEntry;
                                    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));
									if (pEntry->lookupTableFlag == L2TABLE_WROTE)
									{
										ipMcstEntry.vid = vid;
										ipMcstEntry.sip = pEntry->sip;
										ipMcstEntry.dip = pEntry->dip;
										ipMcstEntry.isGrpRefB = mcast_group_ref_get(pEntry);
										LOGIC_PORTMASK_SET_PORT(ipMcstEntry.portmask, portid);
										LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, pEntry->fmode);
										LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.fmode, portid);
										if (SYS_ERR_OK != rsd_ipMcastEntry_set(&ipMcstEntry))
							            {
                                            IGMP_DB_SEM_UNLOCK();
							                SYS_DBG(LOG_DBG_IGMP, "Failed get from ASIC! \n");
							                return SYS_ERR_FAILED;
							            }
									}
								}
							#endif
						}
						pEntry = pEntry->next_subgroup;
					}
				}

				SYS_DBG(LOG_DBG_IGMP, "%s() %d A-X-Y \n", __FUNCTION__, __LINE__);
				uint16 fwdFlag;
                for (i = 0; i < srcNum; i++)
                {
                    groupEntry.sip = *pSourceAddr;
					mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);
					pEntry = groupHead;
                    //pEntry = mcast_group_get(&groupEntry);
                    if (pEntry == NULL)
                    {
                        SYS_DBG(LOG_DBG_IGMP, "%s() %d group Head entry is NULL rcdType=%u\n", __FUNCTION__, __LINE__, rcdType);
                        /*A-X-Y*/
						if (IS_LOGIC_PORTMASK_PORTSET(group_head_entry->fmode, portid))
							fwdFlag = IGMPV3_FWD_NOT;
						else
							fwdFlag = IGMPV3_FWD_CONFLICT;
                        mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
                            grpIntv, fwdFlag, previewInfo);

						#if 1
                        if (rcdType == CHANGE_TO_EXCLUDE_MODE)
                        {
                            pEntry = mcast_group_get(&groupEntry);
                            if(pEntry != NULL)
                            {
                            	SYS_DBG(LOG_DBG_IGMP, "%s() %d update entry  \n", __FUNCTION__, __LINE__);
                                if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
                                    continue;
								if (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
								{
									SYS_DBG(LOG_DBG_IGMP, "%s() %d port %u is in mbr port \n", __FUNCTION__, __LINE__, portid);
									#if 0
	                                if((IS_GPON_MODE() && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid))
										|| (!IS_GPON_MODE() && pMcastVlan->fastleave_enable))
	                                {
										if(SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
										{
											if(mcast_is_last_host_per_port(pEntry, portid))
											{
			                                    pEntry->mbrTimer[portid] = 0;
			                                    LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);

			                                    if(pEntry->lookupTableFlag == L2TABLE_WROTE)
			                                    {
			                                        osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
			                                        LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
			                                        mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
			                                    }

			                                    if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
			                                    {
			                                        pEntry->groupTimer = grpIntv;
			                                    }
											}
										}
	                                }
	                                else
	                                {
	                                    sipArray[numSip++] = *pSourceAddr;
	                                    PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
	                                }
									#endif
								}
								else
								{
									SYS_DBG(LOG_DBG_IGMP, "%s() %d port %u is NOT in mbr port \n", __FUNCTION__, __LINE__, portid);
									sys_ipMcastEntry_t      ipMcstEntry;
                                    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));
									LOGIC_PORTMASK_SET_PORT(pEntry->mbr, portid);
									if (pEntry->lookupTableFlag == L2TABLE_WROTE)
									{
										ipMcstEntry.vid = vid;
										ipMcstEntry.sip = pEntry->sip;
										ipMcstEntry.dip = pEntry->dip;
										LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, pEntry->mbr);
										LOGIC_PORTMASK_COPY(ipMcstEntry.fmode, pEntry->fmode);
										LOGIC_PORTMASK_SET_PORT(ipMcstEntry.fmode, portid);
										if (SYS_ERR_OK != rsd_ipMcastEntry_add(&ipMcstEntry))
							            {
                                            IGMP_DB_SEM_UNLOCK();
							                SYS_DBG(LOG_DBG_IGMP, "Failed get from ASIC! \n");
							                return SYS_ERR_FAILED;
							            }
									}
								}
                            }
                        }
						#endif
                    }
                    else
                    {
                        SYS_DBG(LOG_DBG_IGMP, "%s() %d group Head entry is NOT NULL rcdType=%u\n", __FUNCTION__, __LINE__, rcdType);
                        /*Process A*X && A*Y */

                        /*(A-Y) = A*X + (A-X-Y)*/
                        /*A*X is as follow*/

                        if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
                            continue;

                        /*updata the port*/
                        if (!IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
                            mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr,vid, portid,
                            grpIntv, IGMPV3_FWD_NOT, previewInfo);

                        if (pEntry->v3.p_mbr_fwd_flag[portid] == IGMPV3_FWD_CONFLICT)
                        {
                            if (rcdType == CHANGE_TO_EXCLUDE_MODE)
                            {
                                if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
									(MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
                                {

									if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
									{
										if (mcast_is_last_host_per_port(pEntry, portid))
										{
		                                    pEntry->p_mbr_timer[portid] = 0;


		                                    if (pEntry->lookupTableFlag == L2TABLE_WROTE)
		                                    {
		                                        osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
		                                        LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
		                                        mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
												LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
		                                    }

		                                    if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
		                                    {
		                                        pEntry->groupTimer = grpIntv;
		                                    }
										}
									}
                                }
                                else
                                {
                                    sipArray[numSip++] = *pSourceAddr;
                                    PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
                                }
                            }

                        }
                        /*do nothing*/
                        /*A*Y,  this can do nothing, because the Exclude(X,Y), the Y-A is delete , The Y*A is not fwd status not change*/
                    }
                    pSourceAddr++;
                }
            }

            /*IF querier, send grp &src spec query*/
            if ((rcdType == CHANGE_TO_EXCLUDE_MODE) &&
				((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_NON_FAST_LEAVE== mcast_fastLeaveMode_get(portid)) ||
				(MODE_GPON != igmpCtrl.ponMode && !pMcastVlan->fastleave_enable)))
            {
                if (pQueryEntry == NULL && MODE_GPON != igmpCtrl.ponMode)
                {
                    SYS_DBG(LOG_DBG_IGMP, "[IGMP]VLAN-%d doesn't have query DB entry!\n", vid);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_FAILED;
                }

                if (MODE_GPON != igmpCtrl.ponMode && pQueryEntry->status == IGMP_QUERIER)
                {
                    if (numSip > 0)
                    {
                        for (i = 0; i < numSip; i++)
                        {
                            groupEntry.sip = sipArray[i];
                            pEntry = mcast_group_get(&groupEntry);
                            if (pEntry)
                            {
                                pEntry->v3.p_gss_query_cnt[portid]++;
                                mcast_igmp_send_grp_src_specific_query(vid,grpAddr, &sipArray[i],1, portid);
                            }
                        }
                    }
                }

            }
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;

}

int32 mcast_igmpv3_groupMbrPort_toIn_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t previewInfo)
{
    uint32 grpAddr = 0;
    uint16 srcNum = 0, i;
    uint16 sortedIdx;
    uint32  *pSourceAddr = NULL;
    uint32  sipArray[32], numSip = 0;
	int32 tmpNum;

    igmp_group_entry_t  *pEntry,  *groupHead = NULL, groupEntry;
    igmp_group_head_entry_t   *group_head_entry = NULL;
    igmp_querier_entry_t *pQueryEntry = NULL;

    sys_logic_portmask_t delPmsk;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t *pMcastVlan = NULL;
	uint32 grpIntv;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    grpAddr = ntohl(pRcd->grec_mca);
    srcNum = ((!pRcd) ? 0 : ntohl(pRcd->grec_nsrcs));
    pSourceAddr =  pRcd->grec_src;

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, grpAddr, NULL, &groupEntry)), SYS_ERR_FAILED);

	SYS_DBG(LOG_DBG_IGMP, "%s() %d	srcNum=%u\n", __FUNCTION__, __LINE__, srcNum);
    IGMP_DB_SEM_LOCK();

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    if (NULL == pMcastVlan)
	    {
	        IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }
		grpIntv = pMcastVlan->grpMembershipIntv;
	}
	else
	{
		grpIntv = grpMbrTimerIntv;
	}

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    mcast_igmp_group_head_get( grpAddr, vid, &group_head_entry);

    if (srcNum == 0)  /*process as specfic group query*/
    {
		if (MODE_GPON != igmpCtrl.ponMode)
		{
			if (!groupHead)
	        {
	            IGMP_DB_SEM_UNLOCK();
	            return SYS_ERR_OK;
	        }
	        else
	        {
	            SYS_DBG(LOG_DBG_IGMP, "no pkt src, message leave\n");
	            while (groupHead)
	            {
	                groupHead->p_mbr_timer[portid] = 1;     /*delete the port */
	                groupHead = groupHead->next_subgroup;
	            }
	        }
		}
		else
		{
	        SYS_DBG(LOG_DBG_IGMP, "no pkt src, message leave\n");
			if (!group_head_entry)
	        {
	            IGMP_DB_SEM_UNLOCK();
	            return SYS_ERR_OK;
	        }

			for (i = 0; i < group_sortedAry_entry_num; i++)
			{
				pEntry = pp_group_sorted_array[i];
				while (pEntry)
				{
					if ((pEntry->dip != grpAddr) ||
						IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
					{
						pEntry = pEntry->next_subgroup;
						continue;
					}

					if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
						(MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
					{
						SYS_DBG(LOG_DBG_IGMP, "%s() %d fast leave  for %u  !\n", __FUNCTION__, __LINE__, portid);
						if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
						{
							if (mcast_is_last_host_per_port(pEntry, portid))
							{
								pEntry->p_mbr_timer[portid] = 0;

								if (pEntry->lookupTableFlag == L2TABLE_WROTE)
								{
									osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
									LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
									mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
									LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
								}

								if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
								{
									tmpNum =  group_sortedAry_entry_num;
									mcast_group_del(pEntry);
									if (tmpNum != group_sortedAry_entry_num)
										i--;
									if(p_igmp_stats->oper_version == IGMP_VERSION_V3_FULL)
										mcast_igmp_group_head_del(grpAddr, vid);
								}
							}
						}
					}
					pEntry = pEntry->next_subgroup;
				}
			}
		}
    }
    else
    {
        if (group_head_entry == NULL)
        {
            SYS_DBG(LOG_DBG_IGMP, "Group not exist !\n");

            if (SYS_ERR_OK != mcast_igmp_group_head_ins(grpAddr, vid, portid, MODE_IS_INCLUDE, &group_head_entry, grpIntv))
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            if (group_head_entry != NULL)
            {
                for (i = 0; i < srcNum; i++)
                {
                    mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
                        grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
                    pSourceAddr++;
                }
            }
        }
        else
        {
            SYS_DBG(LOG_DBG_IGMP, "Group  exist !\n");
            if (IS_LOGIC_PORTMASK_CLEAR(group_head_entry->fmode))
            {
            	SYS_DBG(LOG_DBG_IGMP, "group_head_entry  fmode is include !\n");

            	if (MODE_GPON == igmpCtrl.ponMode)
					PORT_SET_FILTER_INCLUDE(group_head_entry->fmode, portid);
				else
                	PORT_SET_FILTER_EXCLUDE(group_head_entry->fmode, portid);

                group_head_entry->p_mbr_timer[portid] = grpIntv;

				for (i = 0; i < group_sortedAry_entry_num; i++)
				{
					pEntry = pp_group_sorted_array[i];
	                while (pEntry != NULL)
	                {
	                	if (pEntry->dip != grpAddr)
	                	{
	                		pEntry = pEntry->next_subgroup;
							continue;
	                	}
						SYS_DBG(LOG_DBG_IGMP, "%s() %d, i=%u entryNum=%u grp_src="IPADDR_PRINT"!\n", __FUNCTION__, __LINE__,
                            i, group_sortedAry_entry_num, IPADDR_PRINT_ARG(pEntry->sip));
	                    if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
	                    {
	                        pEntry = pEntry->next_subgroup;
	                        continue;
	                    }
						//check srcList of database(pEntry) are in receive v3 report with multiple group_src or not
						//if fast leave enable and srcList of DB are not in new src list, delete ASIC entry based on srclist of DB
	                    if (mcast_igmpv3_source_is_inPkt(pEntry, portid, pSourceAddr, srcNum) == FALSE)
	                    {
	                    	SYS_DBG(LOG_DBG_IGMP, "%s() %d receive v3 report with multi group_src are not in pEntry! grp_src="IPADDR_PRINT"!\n",
                                __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pEntry->sip));
	                        if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
								(MODE_GPON != igmpCtrl.ponMode  && pMcastVlan->fastleave_enable))
	                        {
	                        	if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
	                        	{
	                        		if (mcast_is_last_host_per_port(pEntry, portid))
	                        		{
			                            pEntry->p_mbr_timer[portid] = 0;


			                            if (L2TABLE_WROTE == pEntry->lookupTableFlag)
			                            {
			                                osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
			                                LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
			                                mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
											LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
			                            }

			                            if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
			                            {
			                            	if (MODE_GPON == igmpCtrl.ponMode)
			                            	{
												SYS_DBG(LOG_DBG_IGMP, "%s() %d sortKey=%llu\n", __FUNCTION__, __LINE__,
                                                    mcast_group_sortKey_ret(pEntry));
												SYS_DBG(LOG_DBG_IGMP, "%s() %d grp_src="IPADDR_PRINT"!\n", __FUNCTION__, __LINE__,
                                                    IPADDR_PRINT_ARG(pEntry->sip));
												tmpNum =  group_sortedAry_entry_num;
												mcast_group_del(pEntry);
												if (tmpNum != group_sortedAry_entry_num)
													i--;
			                            	}
											else
											{
			                                	pEntry->groupTimer = grpIntv;
											}
			                            }
	                        		}
	                        	}
	                        }
	                        else
	                        {
	                            sipArray[numSip++] = pEntry->sip;
	                            PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
	                        }
	                    }
	                    pEntry = pEntry->next_subgroup;
               		}
				}

				SYS_DBG(LOG_DBG_IGMP, "%s() %d srcNum=%u, pkt grp_src="IPADDR_PRINT"!\n", __FUNCTION__, __LINE__,
					srcNum, IPADDR_PRINT_ARG(pSourceAddr[0]));
                for (i =0; i < srcNum; i++)
                {
					//check receive v3 report with multiple group_src are in database(pEntry) or not
					//if new srcList is not in DB, add new srcList in ASIC
                    mcast_igmpv3_source_add_wrapper(clientIp, group_head_entry->dip,*pSourceAddr,group_head_entry->vid, portid,
                        grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
                    pSourceAddr++;
                }

				if ((MODE_GPON != igmpCtrl.ponMode && !pMcastVlan->fastleave_enable))
                {
                    mcast_querier_db_get(vid, ipType, &pQueryEntry);
                    if (!pQueryEntry)
                    {
                        SYS_DBG(LOG_DBG_IGMP, "[IGMP] VLAN-%d doesn't have query DB entry!\n", vid);
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                    }

                    if (IGMP_QUERIER == pQueryEntry->status)
                    {
                        for (i = 0; i < numSip; i++)
                        {
                            groupEntry.sip = sipArray[i];
                            pEntry = mcast_group_get(&groupEntry);
                            if(pEntry != NULL)
                            {
                                pEntry->v3.p_gss_query_cnt[portid]++;
                                mcast_igmp_send_grp_src_specific_query(vid,grpAddr, &sipArray[i],1, portid);
                            }
                        }
                    }

                }
            }
            else
            {
            	SYS_DBG(LOG_DBG_IGMP, "group_head_entry  fmode is exclude !\n");
                /*process the group mode is exclude*/
                /*process the exclude mode,  EXCLUDE(X+A, Y-A)*/
                SYS_DBG(LOG_DBG_IGMP, "proc X+A, and A = GMI \n");

                group_head_entry->p_mbr_timer[portid] = grpIntv;
                osal_memset(sipArray,0,sizeof(sipArray));

				//uint8 mbrFwdFlag = (IS_GPON_MODE() == TRUE ?  IGMPV3_FWD_NOT : IGMPV3_FWD_CONFLICT);
				for (i = 0; i < group_sortedAry_entry_num; i++)
				{
	                /*X-A*/
	                pEntry = pp_group_sorted_array[i];
	                while (pEntry != NULL)
	                {
	                	if (pEntry->dip != grpAddr)
	                	{
	                		pEntry = pEntry->next_subgroup;
							continue;
	                	}
	                    if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
	                    {
	                        pEntry = pEntry->next_subgroup;
	                        continue;
	                    }

	                    if (pEntry->v3.p_mbr_fwd_flag[portid] == IGMPV3_FWD_CONFLICT)
	                    {
							SYS_DBG(LOG_DBG_IGMP, "%s() %d pEntry for %u is include !\n", __FUNCTION__, __LINE__, portid);
	                        if (mcast_igmpv3_source_is_inPkt(pEntry,portid, pSourceAddr, srcNum) == FALSE)
	                        {
								SYS_DBG(LOG_DBG_IGMP, "%s() %d receive v3 report with multi group_src are not in pEntry!\n", __FUNCTION__, __LINE__);
								if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid)) ||
									(MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
	                            {
									if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
									{
										if (mcast_is_last_host_per_port(pEntry, portid))
										{
			                                pEntry->p_mbr_timer[portid] = 0;

			                                if (pEntry->lookupTableFlag == L2TABLE_WROTE)
			                                {
			                                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
			                                    LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
			                                    mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
											 	LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
			                                }

											if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
											{
												if (MODE_GPON == igmpCtrl.ponMode)
												{
													/*if(p_igmp_stats->oper_version == IGMP_VERSION_V3_FULL)
														mcast_igmp_group_head_del(pEntry->dip, pEntry->vid);*/
													tmpNum =  group_sortedAry_entry_num;
													mcast_group_del(pEntry);
													if (tmpNum != group_sortedAry_entry_num)
														i--;
												}
												else
												{
													pEntry->groupTimer = grpIntv;
												}
											}
										}
									}
	                            }
	                            else
	                            {
	                                sipArray[numSip++] = pEntry->sip;
	                                PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
	                            }
	                        }
	                    }
	                    pEntry = pEntry->next_subgroup;
	                }
				}

				//(A-X)
				for (i =0; i < srcNum; i++)
				{
					//rcv srcList had been in DB, update ASIC; rcv srcList hadn't been in DB, Add ASIC
					mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
						grpIntv, IGMPV3_FWD_CONFLICT, previewInfo);
					pSourceAddr++;
				}

				if ((MODE_GPON != igmpCtrl.ponMode && !pMcastVlan->fastleave_enable))
                {
                    mcast_querier_db_get(vid,  ipType, &pQueryEntry);
                    if (!pQueryEntry)
                    {
                        SYS_DBG(LOG_DBG_IGMP, "[IGMP]VLAN-%d doesn't have query DB entry!\n", vid);
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                    }

                    if (IGMP_QUERIER == pQueryEntry->status)
                    {
                        if (numSip > 0)
                        {
                            for (i = 0; i < numSip; i++)
                            {
                                groupEntry.sip = sipArray[i];
                                pEntry = mcast_group_get(&groupEntry);
                                if (pEntry)
                                {
                                    pEntry->v3.p_gss_query_cnt[portid]++;
                                    mcast_igmp_send_grp_src_specific_query(vid,grpAddr, &sipArray[i],1, portid);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_igmpv3_groupMbrPort_block_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid,
	ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t previewInfo)
{
    uint32 grpAddr = 0;
    uint16 srcNum = 0, i;
    uint16 sortedIdx;
    uint32  *pSourceAddr = NULL;
    uint32  sipArray[32];  /*a record have max source list num is 128*/
    uint16  numSip = 0, fwdFlag;

    igmp_group_entry_t  *pEntry = NULL,  *groupHead = NULL, groupEntry;
    igmp_group_head_entry_t  *group_head_entry = NULL;

    sys_logic_portmask_t  delPmsk;
    igmp_querier_entry_t  *pQueryEntry = NULL;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t *pMcastVlan = NULL;

	uint32 grpIntv;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    grpAddr = ntohl(pRcd->grec_mca);
    srcNum = ((!pRcd) ? 0 : ntohl(pRcd->grec_nsrcs));
    pSourceAddr =  pRcd->grec_src;

    if (srcNum == 0)
    {
        SYS_DBG(LOG_DBG_IGMP, "no pkt src in the group \n");
        return SYS_ERR_OK;
    }

    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(vid, ipType, grpAddr, NULL, &groupEntry)), SYS_ERR_FAILED);

    IGMP_DB_SEM_LOCK();

	if (MODE_GPON != igmpCtrl.ponMode)
	{
	    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
	    if (NULL == pMcastVlan)
	    {
	        IGMP_DB_SEM_UNLOCK();
	        return SYS_ERR_FAILED;
	    }
		grpIntv = pMcastVlan->grpMembershipIntv;
	}
	else
	{
		grpIntv = grpMbrTimerIntv;
	}
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);

    mcast_igmp_group_head_get(grpAddr, vid,  &group_head_entry);

    if (group_head_entry == NULL)
    {
        SYS_DBG(LOG_DBG_IGMP, "the group not exist \n");
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_OK;
    }

    if (IS_LOGIC_PORTMASK_CLEAR(group_head_entry->fmode))
    {
        /*include mode*/
		if (groupHead && groupHead->sip == 0)
		{

			if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid))
				|| (MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
			{
				if (SYS_ERR_OK == mcast_host_tracking_del(groupHead, clientIp, portid))
				{
					if (mcast_is_last_host_per_port(groupHead, portid))
					{
						if (!groupHead->next_subgroup)
						{
							mcast_group_del(groupHead);
							mcast_igmp_group_head_del(grpAddr, vid);
						}
					}
				}
			}
			else
			{
				if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, portid) && groupHead->p_gsQueryCnt[portid] == 0)
                {
                    /* Querier */
                    mcast_querier_db_get(groupHead->vid, groupHead->ipType, &pQueryEntry);
                    if (pQueryEntry && IGMP_NON_QUERIER == pQueryEntry->status)
                    {
                        ;
                    }
                    else
                    {
                        groupHead->p_mbr_timer[portid] = grpIntv;
                        groupHead->p_gsQueryCnt[portid]++;
                        //mcast_igmp_send_grp_specific_query(vid, grpAddr, portid);
                        mcast_send_gs_query(groupHead, portid);
                    }
                }
			}
		}
		else
		{
			for (i = 0; i < srcNum; i++)
			{
				groupEntry.sip = *pSourceAddr;
				pEntry = mcast_group_get(&groupEntry);
				if (pEntry)
				{
					SYS_DBG(LOG_DBG_IGMP, "the new srcList is in DB \n");
					if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
						continue;
					if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
					{
						SYS_DBG(LOG_DBG_IGMP, "del host tracking by client \n");
					}
					if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid))
						|| (MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
			        {
		                if (IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, portid))
		                {
							if (mcast_is_last_host_per_port(pEntry, portid))
							{
	                            pEntry->p_mbr_timer[portid] = 0; //set srcTimer = 0;

	                            if (L2TABLE_WROTE == pEntry->lookupTableFlag)
	                            {
	                                osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
	                                LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
	                                mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
									LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
	                            }
								mcast_group_del(pEntry);
								//TBD: del host tracking per port ?
	                            if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
	                            {
	                                pEntry->groupTimer = grpIntv;
	                            }
							}
		                }
			        }
				}
				pSourceAddr++;
			}
		}
    }
    else
    {
        /*exclude mode*/
        /*proc EXCLUDE(X + (A-Y), Y),  (A-X-Y) = GMI*/
        /*A-Y = (A*X) + (A-X-Y)*/
        osal_memset(sipArray, 0, sizeof(sipArray));

        for (i = 0; i < srcNum; i++)
        {
            groupEntry.sip = *pSourceAddr;
            pEntry = mcast_group_get(&groupEntry);
            if (pEntry)  /*A*X*/
            {
            	SYS_DBG(LOG_DBG_IGMP, "the new srcList is in DB \n");
                if (IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, portid))
                    continue;
				if (SYS_ERR_OK == mcast_host_tracking_del(pEntry, clientIp, portid))
				{
					SYS_DBG(LOG_DBG_IGMP, "del host tracking by client \n");
				}
                if (pEntry->v3.p_mbr_fwd_flag[portid] == IGMPV3_FWD_CONFLICT)
                {
                    if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_FAST == mcast_fastLeaveMode_get(portid))
						|| (MODE_GPON != igmpCtrl.ponMode && pMcastVlan->fastleave_enable))
                    {
                		if (mcast_is_last_host_per_port(pEntry, portid))
                		{
	                        pEntry->p_mbr_timer[portid] = 0; //set src timer  = 0


	                        if (L2TABLE_WROTE == pEntry->lookupTableFlag)
	                        {
	                            osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
	                            LOGIC_PORTMASK_SET_PORT(delPmsk, portid);
	                            mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);
								LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, portid);
	                        }
							mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, pEntry->sip, vid, portid,
               	 				grpIntv, IGMPV3_FWD_NOT, previewInfo);
							//TBD: del host tracking per port ?
	                        if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
	                        {
	                            pEntry->groupTimer = grpIntv;
	                        }
                		}
                    }
                    else
                    {
                        sipArray[numSip++] = *pSourceAddr;
                        PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
                    }
                }
                else if (IGMPV3_FWD_NORMAL == pEntry->v3.p_mbr_fwd_flag[portid])
                {
                    sipArray[numSip++] = *pSourceAddr;
                    PORT_SET_SRC_QUERY(pEntry->v3.isGssQueryFlag, portid);
                }
            }
			else
			{
				SYS_DBG(LOG_DBG_IGMP, "the new srcList is NOT in DB \n");
				if (IS_LOGIC_PORTMASK_PORTSET(group_head_entry->fmode, portid))
					fwdFlag = IGMPV3_FWD_NOT;
				else
					fwdFlag = IGMPV3_FWD_CONFLICT;
                mcast_igmpv3_source_add_wrapper(clientIp, grpAddr, *pSourceAddr, vid, portid,
               	 grpIntv, fwdFlag, previewInfo);
			}
            pSourceAddr++;
        }

        /*Send grp&src query*/
        if ((MODE_GPON == igmpCtrl.ponMode && IGMP_LEAVE_MODE_NON_FAST_LEAVE == mcast_fastLeaveMode_get(portid))
			|| (MODE_GPON != igmpCtrl.ponMode && !pMcastVlan->fastleave_enable))
        {
            mcast_querier_db_get(vid, ipType, &pQueryEntry);
            if (pQueryEntry == NULL)
            {
                SYS_DBG(LOG_DBG_IGMP, "[IGMP]VLAN-%d doesn't have query DB entry!\n", vid);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }

            if (IGMP_QUERIER == pQueryEntry->status)
            {
                if (numSip > 0)
                {
                    for (i = 0; i < numSip; i++)
                    {
                        groupEntry.sip = sipArray[i];
                        pEntry = mcast_group_get(&groupEntry);
                        if (pEntry)
                        {
                            pEntry->v3.p_gss_query_cnt[portid]++;
                            mcast_igmp_send_grp_src_specific_query(vid,grpAddr, &sipArray[i],1, portid);
                            SYS_DBG(LOG_DBG_IGMP,
                                "send gss query sip : ("IPADDR_PRINT") , dip : ("IPADDR_PRINT")  \n\n",
                                IPADDR_PRINT_ARG(sipArray[i]), IPADDR_PRINT_ARG(grpAddr));
                        }
                    }
                }
            }

        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

int32 mcast_igmpv3_basic_groupMbrPort_isIn_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    uint32 dip, grpSrcIp;
    //uint16 numSrc = 0;
    mcast_groupAddr_t groupAddr;
    uint8  sendFlag = 0;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

	//TBD: mutiple group src ip
	grpSrcIp = ntohl(pRcd->grec_src[0]);
    dip = ntohl(pRcd->grec_mca);
    //numSrc = ntohl(pRcd->grec_nsrcs);

    groupAddr.ipv4 = dip;

    mcast_groupMbrPort_add_wrapper(ipType, vid, NULL, grpSrcIp, &groupAddr, portid, &sendFlag, clientIp, pRecvTime);

    return SYS_ERR_OK;
}

int32 mcast_igmpv3_basic_groupMbrPort_isEx_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    mcast_igmpv3_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    return SYS_ERR_OK;
}


int32 mcast_igmpv3_basic_groupMbrPort_toEx_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    mcast_igmpv3_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    return SYS_ERR_OK;
}

int32 mcast_igmpv3_basic_groupMbrPort_toIn_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    uint32 dip;
    uint16 numSrc = 0;
    mcast_groupAddr_t groupAddr;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    dip = ntohl(pRcd->grec_mca);

    numSrc = ((!pRcd) ? 0 : ntohl(pRcd->grec_nsrcs));

    groupAddr.ipv4 = dip;

    if(0 == numSrc)
    {
        if (IGMP_DIP_SIP_MODE == p_igmp_stats->lookup_mode || IGMP_DIP_VID_MODE == p_igmp_stats->lookup_mode)
        {
             mcast_groupMbrPort_del_wrapper_dipsip(ipType, vid,NULL,&groupAddr, portid, clientIp);
        }
        else if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
        {
             mcast_groupMbrPort_del_wrapper_dmacvid(ipType, vid,NULL,&groupAddr,portid, clientIp);
        }
    }
    else
    {
         mcast_igmpv3_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    }


    return SYS_ERR_OK;
}

int32 mcast_igmpv3_basic_groupMbrPort_block_proc(igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp)
{
    return SYS_ERR_OK;
}

int32 mcast_mldv2_basic_groupMbrPort_isIn_proc(mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    //uint16 numSrc = 0;
    mcast_groupAddr_t groupAddr;
    uint8   sendFlag = 0;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV6;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    //numSrc = ntohl(pRcd->grec_nsrcs);

    osal_memcpy(groupAddr.ipv6.addr, pRcd->grec_ipv6Mca, IPV6_ADDR_LEN);
	//TBD: not support Ipv6 source filtering
    mcast_groupMbrPort_add_wrapper(ipType, vid, NULL, 0, &groupAddr, portid, &sendFlag, clientIp, pRecvTime);

    return SYS_ERR_OK;
}

int32 mcast_mldv2_basic_groupMbrPort_isEx_proc(mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    mcast_mldv2_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    return SYS_ERR_OK;
}


int32 mcast_mldv2_basic_groupMbrPort_toEx_proc(mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    mcast_mldv2_basic_groupMbrPort_isIn_proc(pRcd,vid,portid, clientIp, pRecvTime);
    return SYS_ERR_OK;
}

int32 mcast_mldv2_basic_groupMbrPort_toIn_proc(mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    uint16 numSrc = 0;
    mcast_groupAddr_t groupAddr;
    uint8 sendFlag = 0;

    multicast_ipType_t ipType = MULTICAST_TYPE_IPV6;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    numSrc = ntohl(pRcd->grec_nsrcs);
    osal_memcpy(groupAddr.ipv6.addr, pRcd->grec_ipv6Mca, IPV6_ADDR_LEN);

    if(0 == numSrc)
    {
        if (IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode)
        {
             mcast_groupMbrPort_del_wrapper_dmacvid(ipType, vid,NULL,&groupAddr,portid, clientIp);
        }
    }
    else
    {
    	//TBD: not support ipv6 source filtering
        mcast_groupMbrPort_add_wrapper(ipType, vid, NULL, 0, &groupAddr, portid, &sendFlag, clientIp, pRecvTime);
    }

    return SYS_ERR_OK;
}

int32 mcast_mldv2_basic_groupMbrPort_block_proc(mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime)
{
    return SYS_ERR_OK;
}

int32 mcast_static_group_save(sys_vlanmask_t vlanMsk, multicast_ipType_t ipType)
{
    uint32 i;
    igmp_group_entry_t  *pGroup_entry;

    osal_memset(&groupSave, 0, sizeof(igmp_static_group_t));

    if (mcast_group_num == 0)
        return SYS_ERR_OK;

    groupSave.pGroup = (igmp_group_entry_t *)osal_alloc(sizeof(igmp_group_entry_t) * mcast_group_num);

    if (groupSave.pGroup == NULL)
    {
        SYS_DBG(LOG_DBG_IGMP, "Alloc memory failure !\n");
        return SYS_ERR_FAILED;
    }

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup_entry = mcast_group_get_with_index(i);

        if (pGroup_entry)
        {
            if (IGMP_GROUP_ENTRY_STATIC != pGroup_entry->form)
                continue;

            if (MULTICAST_TYPE_END != ipType)
            {
                if (ipType != pGroup_entry->ipType)
                    continue;
            }

            if (!VLANMASK_IS_VIDSET(vlanMsk, pGroup_entry->vid))
                continue;

            if ((pGroup_entry->dip != 0) && (pGroup_entry->vid != 0))
            {
                groupSave.pGroup[groupSave.groupCnt].vid = pGroup_entry->vid;
                groupSave.pGroup[groupSave.groupCnt].dip = pGroup_entry->dip;
                groupSave.pGroup[groupSave.groupCnt].form = pGroup_entry->form;
                groupSave.pGroup[groupSave.groupCnt].ipType = pGroup_entry->ipType;
                if (MULTICAST_TYPE_IPV4 != pGroup_entry->ipType)
                {
                    osal_memcpy(groupSave.pGroup[groupSave.groupCnt].dipv6.addr, pGroup_entry->dipv6.addr, IPV6_ADDR_LEN);
                }
                osal_memcpy(&(groupSave.pGroup[groupSave.groupCnt].staticMbr), &pGroup_entry->staticMbr, sizeof(sys_logic_portmask_t));
                osal_memcpy(&(groupSave.pGroup[groupSave.groupCnt].staticInMbr), &pGroup_entry->staticInMbr, sizeof(sys_logic_portmask_t));
                groupSave.groupCnt++;
            }
        }
    }

    return SYS_ERR_OK;
}

int32 mcast_static_group_restore(void)
{
    uint32 i, j;
    igmp_group_entry_t  group_entry;

    group_entry.p_mbr_timer = (uint16 *)osal_alloc(sizeof(uint16) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!group_entry.p_mbr_timer), SYS_ERR_FAILED);

    group_entry.p_gsQueryCnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!group_entry.p_gsQueryCnt), SYS_ERR_FAILED);

    group_entry.p_hostListHead = (struct hostHead *)osal_alloc(sizeof(struct hostHead) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!group_entry.p_hostListHead), SYS_ERR_FAILED);

    group_entry.v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!group_entry.v3.p_mbr_fwd_flag), SYS_ERR_FAILED);

    group_entry.v3.p_gss_query_cnt = (uint8 *)osal_alloc(sizeof(uint8) * igmpCtrl.sys_max_port_num);
    SYS_ERR_CHK((!group_entry.v3.p_gss_query_cnt), SYS_ERR_FAILED);


    for(i = 0; i < groupSave.groupCnt; i++)
    {
        osal_memset(&group_entry, 0, sizeof(igmp_group_entry_t));
        if(group_entry.p_mbr_timer != NULL)
            osal_memset(group_entry.p_mbr_timer, 0, sizeof(uint16) * igmpCtrl.sys_max_port_num);
        if(group_entry.p_gsQueryCnt != NULL)
            osal_memset(group_entry.p_gsQueryCnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);
        FOR_EACH_LOGIC_PORT(j)
        {
        	LIST_INIT(&group_entry.p_hostListHead[j]);
        }
        if(group_entry.v3.p_mbr_fwd_flag != NULL)
            osal_memset(group_entry.v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
        if(group_entry.v3.p_gss_query_cnt != NULL)
            osal_memset(group_entry.v3.p_gss_query_cnt, 0, sizeof(uint8) * igmpCtrl.sys_max_port_num);

        osal_memcpy(&group_entry, &(groupSave.pGroup[i]), sizeof(igmp_group_entry_t));
        //now only save the InMbr.
        osal_memcpy(&group_entry.mbr, &(groupSave.pGroup[i].staticInMbr), sizeof(sys_logic_portmask_t));

        mcast_basic_static_groupEntry_set(&group_entry, FILTER_MODE_INCLUDE, TRUE);
    }

    if (NULL != groupSave.pGroup)
    {
        osal_free(groupSave.pGroup);
        groupSave.pGroup = NULL;
    }
    osal_free(group_entry.p_mbr_timer);
    osal_free(group_entry.p_gsQueryCnt);
    osal_free(group_entry.p_hostListHead);
    osal_free(group_entry.v3.p_mbr_fwd_flag);
    osal_free(group_entry.v3.p_gss_query_cnt);

    return SYS_ERR_OK;
}


int32 mcast_unauthorized_check(uint32 aclEntryId)
{
	mcast_prof_entry_t *ptr = NULL;
	uint16 mopId = aclEntryId >> 16;
	uint16 keyId = aclEntryId & 0xFFFF;

	if (1023 == keyId || 0xFFFF == keyId)
	{
		// is default rule
		LIST_FOREACH(ptr, &mcastProfHead, entries)
		{
			if (ptr->entry.mopId == mopId)
			{
				if(TRUE == ptr->entry.unAuthJoinRqtBhvr)
					return SYS_ERR_OK;
				else
					return SYS_ERR_FAILED;
			}
		}
		SYS_DBG(LOG_DBG_IGMP, "%s() %d match default rule, mopId=%u, keyId=%u \n", __FUNCTION__, __LINE__, mopId, keyId);
	}
	else
	{
		// not default rule
		SYS_DBG(LOG_DBG_IGMP, "%s() %d match non-default rule, mopId=%u, keyId=%u \n", __FUNCTION__, __LINE__, mopId, keyId);
		return SYS_ERR_OK;
	}
	return SYS_ERR_FAILED;
}

int32 mcast_preview_check(igmp_acl_entry_t *pAclEntry, struct timespec *pJoinTime, mcast_preview_info_t *pPreviewInfo)
{
	igmp_preview_timer_entry_t *ptr = NULL;
	timer_arg_t *pArg = NULL;
	long long nosec;
	//IGMP_DB_SEM_LOCK();
	nosec = (long long)(pJoinTime->tv_sec) * 1000 * 1000 * 1000 + pJoinTime->tv_nsec;
	LIST_FOREACH(ptr, &previewTimerHead, entries)
	{
		pArg = (timer_arg_t *)(ptr->pEntry->arg);
		if(pAclEntry->id == pArg->id)
		{
			if(nosec >= ((long long)(pArg->nextPreviewTime.tv_sec) * 1000 * 1000 * 1000 + pArg->nextPreviewTime.tv_nsec))
			{
				if((0 == pAclEntry->previewRepeatCnt) ||
					(pArg->watchTimes <= pAclEntry->previewRepeatCnt))
				{
					pArg->watchTimes++;
					//update nextPreviewTime
					nosec = (long long)((pJoinTime->tv_sec)+ pAclEntry->previewLen + pAclEntry->previewRepeatTime) * 1000 * 1000 * 1000 + pJoinTime->tv_nsec;
    				pArg->nextPreviewTime.tv_sec = nosec / (1000 * 1000 * 1000);
    				pArg->nextPreviewTime.tv_nsec = nosec % (1000 * 1000 * 1000);
					pPreviewInfo->previewType = MCAST_PREVIEW;
					pPreviewInfo->length = pAclEntry->previewLen;
					return SYS_ERR_OK;
				}
				else
				{
					printf("watch times is over than limitation!!\n");
				}
			}
			else
			{
				printf("the next preview is not arrive!!\n");
			}
		}
	}
	//IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_FAILED;
}

int32 mcast_allowed_preview_check(multicast_ipType_t ipType, sys_logic_port_t port, uint32 groupSrcIp,
	mcast_groupAddr_t *pGroupAddr, uint32 *pAclEntryId, uint16 *pGrpVid, mcast_preview_info_t *pPreviewInfo)
{
	mcast_allowed_preview_row_entry_t *ptr = NULL;
	uint32 ipv4IpAddr, isIgmpv2B = FALSE, res[3] = {FALSE, FALSE, FALSE}, result = FALSE;
	//IGMP_DB_SEM_LOCK();
	LIST_FOREACH(ptr, &allowedPreviewHead, entries)
	{
		//TBD ipv6
		if((ptr->entry.id >> 16) == port)
		{
			switch(ipType)
			{
				case MULTICAST_TYPE_IPV4: /* IPV4 */
					//check igmp version
					isIgmpv2B = ((0 == groupSrcIp) ?  TRUE : FALSE);
                    if (isIgmpv2B == TRUE)
                    {
                        IS_ZERO_RANGE((ptr->entry.dstIpAddr), 0, 11, (&res[0]));
						IS_ZERO_RANGE((ptr->entry.srcIpAddr), 0, 15, (&res[1]));
						osal_memcpy(&ipv4IpAddr, (ptr->entry.dstIpAddr + 12), sizeof(uint32));
						res[2] = ((ipv4IpAddr == pGroupAddr->ipv4) ? TRUE : FALSE);
						//TBD check UNI tag
						if(res[0] && res[1] && res[2])
							result = TRUE;
                    }
                    else
                    {
                        osal_memcpy(&ipv4IpAddr, (ptr->entry.dstIpAddr + 12), sizeof(uint32));
						res[0] = ((ipv4IpAddr == pGroupAddr->ipv4) ? TRUE : FALSE);
						osal_memcpy(&ipv4IpAddr, (ptr->entry.srcIpAddr + 12), sizeof(uint32));
						res[1] = ((ipv4IpAddr == groupSrcIp) ? TRUE : FALSE);
						//TBD check UNI tag
						if(res[0] && res[1])
							result = TRUE;
                    }

					break;
				case MULTICAST_TYPE_IPV6:
					//TBD
					printf("Not support IPv6\n");
					break;
				default:
					printf("Not support \n");
			}
			if(TRUE == result)
			{
				*pAclEntryId = ptr->entry.id;
				*pGrpVid = ptr->entry.aniVid;
				pPreviewInfo->previewType = MCAST_ALLOWED_PREVIEW;
				pPreviewInfo->allowedPreviewUniVid = ptr->entry.uniVid;
				pPreviewInfo->length = ptr->entry.duration;
				return SYS_ERR_OK;
			}
		}
	}
	//IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_FAILED;
}

uint32 mcast_acl_entry_group_num(multicast_ipType_t ipType, uint8 *pStart, uint8 *pEnd)
{
	uint32 num = 1, idx, rentOneB = FALSE;
    uint8 temp;
	switch(ipType)
	{
		case MULTICAST_TYPE_IPV4:
			num = (*((uint32 *)pEnd)) - (*((uint32 *)pStart));
			break;
		case MULTICAST_TYPE_IPV6:
			for (idx = IPV6_ADDR_LEN - 1; idx >= 12; idx--)
	        {
	            if (pEnd[idx] >= pStart[idx])
	            {
	                temp = pEnd[idx] - pStart[idx];
	                if (rentOneB)
	                {
	                    if (temp > 0)
	                    {
	                        temp -= 1;
	                        rentOneB = FALSE;
	                    }
	                    else
	                    {
	                        temp = 0xFF;
	                        rentOneB = TRUE;
	                    }
	                }
	                num += temp << 8 * (IPV6_ADDR_LEN - idx -1);
	            }
	            else
	            {
	                rentOneB = TRUE;
	                temp = 0xFF - pStart[idx] + pEnd[idx];
	                num += temp << 8 * (IPV6_ADDR_LEN - idx -1);
	            }
	        }
			break;
		default:
			SYS_DBG(LOG_DBG_IGMP, "%s() %d Not Support ipType=%u \n", __FUNCTION__, __LINE__, ipType);
	}

	return ((0 == num) ? 1 : num);
}

int32 mcast_mop_profile_per_port_find(sys_logic_port_t port, uint32 *pAclEntryId)
{
	mcast_port_info_entry_t *ptr = NULL;

	if (!pAclEntryId)
		return SYS_ERR_INPUT;

	LIST_FOREACH(ptr, &portInfoHead, entries)
	{
		if (ptr->entry.uniPort == port)
		{
			*pAclEntryId = (ptr->entry.mopId << 16) | 0xFFFF;
			return SYS_ERR_OK;
		}
	}
	return SYS_ERR_FAILED;
}

/*check groupAddr in port's acl entry of profile*/
int32 mcast_filter_check(struct timespec *pJoinTime, multicast_ipType_t ipType, sys_logic_port_t port, uint32 groupSrcIp,
	mcast_groupAddr_t *pGroupAddr, uint32 *pAclEntryId, uint16 *pGrpVid, uint32 *pImputedGrpBw, mcast_preview_info_t *pPreviewInfo)
{
    sys_igmp_profile_t      *profile;
	igmp_acl_entry_list_t   *pAclEntryList;
    int32                   ret1 = 0, ret2 = 0;

    SYS_PARAM_CHK((NULL == pGroupAddr), SYS_ERR_NULL_POINTER);

    if (SYS_ERR_OK != mcast_profile_get(ipType, port, &profile))
	{
		printf("Can't find profile, ipType = %d, port =%d\n", ipType, port);
		return SYS_ERR_FAILED;
	}

	pAclEntryList = profile->aclEntryList[IGMP_GROUP_ENTRY_DYNAMIC];

    if (MULTICAST_TYPE_IPV4 != ipType)
    {
    	while (pAclEntryList)
    	{
    		//TBD: mldv2 srcip check for ipv6
        	ret1 = osal_memcmp(pGroupAddr->ipv6.addr, pAclEntryList->aclEntry->dipStart.ipv6, IPV6_ADDR_LEN);
        	ret2 = osal_memcmp(pGroupAddr->ipv6.addr, pAclEntryList->aclEntry->dipEnd.ipv6, IPV6_ADDR_LEN);

			if (ret2 <= 0 && ret1 >= 0)
			{
				*pAclEntryId = pAclEntryList->aclEntry->id;
				*pGrpVid  = pAclEntryList->aclEntry->aniVid;
				*pImputedGrpBw = (pAclEntryList->aclEntry->imputedGrpBW) /
					mcast_acl_entry_group_num(ipType, pAclEntryList->aclEntry->dipStart.ipv6,
					                            pAclEntryList->aclEntry->dipEnd.ipv6);

	        	return mcast_unauthorized_check(pAclEntryList->aclEntry->id);
			}
			pAclEntryList = pAclEntryList->next;
		}
        /* check exist mop profile with forward unauthorized ctrl packet but no acl rule. */
        SYS_ERR_CHK((SYS_ERR_FAILED == mcast_mop_profile_per_port_find(port, pAclEntryId)), SYS_ERR_FAILED);
        return mcast_unauthorized_check(*pAclEntryId);
    }
    else if (MULTICAST_TYPE_IPV4 == ipType)
	{
		while (pAclEntryList)
    	{
		    if (pGroupAddr->ipv4 <= pAclEntryList->aclEntry->dipEnd.ipv4
				&& pGroupAddr->ipv4 >= pAclEntryList->aclEntry->dipStart.ipv4
				&& (0 == groupSrcIp || 0 == pAclEntryList->aclEntry->sip.ipv4
				|| groupSrcIp == pAclEntryList->aclEntry->sip.ipv4))
		    {
		    	*pAclEntryId = pAclEntryList->aclEntry->id;
				*pGrpVid  = pAclEntryList->aclEntry->aniVid;
				*pImputedGrpBw = (pAclEntryList->aclEntry->imputedGrpBW) /
					mcast_acl_entry_group_num(ipType, (uint8 *)(&pAclEntryList->aclEntry->dipStart.ipv4), (uint8 *)(&pAclEntryList->aclEntry->dipEnd.ipv4));
				if(pAclEntryList->aclEntry->previewLen)
				{
					//TBD mcast_allowed_preview_check if hit return
					if (SYS_ERR_OK == mcast_allowed_preview_check(ipType, port, groupSrcIp, pGroupAddr, pAclEntryId, pGrpVid, pPreviewInfo))
						return SYS_ERR_OK;
					if (pJoinTime && (SYS_ERR_FAILED == mcast_preview_check(pAclEntryList->aclEntry, pJoinTime, pPreviewInfo)))
					{
						printf("%s() %d mcast_preview_check failed\n", __FUNCTION__, __LINE__);
						return SYS_ERR_FAILED;
					}
				}
		        return mcast_unauthorized_check(pAclEntryList->aclEntry->id);
		    }
			pAclEntryList = pAclEntryList->next;
		}
        if (pAclEntryList)
        {
            SYS_DBG(LOG_DBG_IGMP, "%s() %d No acl entry list \n", __FUNCTION__, __LINE__);
		    //TBD mcast_allowed_preview_check even if full authorized group not match
		    return  mcast_allowed_preview_check(ipType, port, groupSrcIp, pGroupAddr, pAclEntryId, pGrpVid, pPreviewInfo);
        }
        else
        {
            /* check exist mop profile with forward unauthorized ctrl packet but no acl rule. */
            SYS_ERR_CHK((SYS_ERR_FAILED == mcast_mop_profile_per_port_find(port, pAclEntryId)), SYS_ERR_FAILED);
            SYS_DBG(LOG_DBG_IGMP, "%s() %d only exist mop profile \n", __FUNCTION__, __LINE__);
            return mcast_unauthorized_check(*pAclEntryId);
        }
	}

    SYS_DBG(LOG_DBG_IGMP, "The group ip is filter!\n");

    return SYS_ERR_FAILED;
}

#include <sys/sysinfo.h>

static long uptime(void)
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}

int32 mcast_groupMbrPort_DelFromOneVlan_dmacvid(uint16 vid, sys_logic_port_t port,
                                                igmp_del_port_type_t delportType)
{
    int32                  i;
    int32                   ret;
    igmp_group_entry_t      *groupHead, tmpEntry;
    igmp_aggregate_entry_t  *pAggrEntry;
    sys_logic_portmask_t    delPmsk;
    int8  iRet = 0;

    IGMP_DB_SEM_LOCK();

    for(i = 0; i < mcast_group_num; i++)
    {
    	// printf("delete port [%d] from mc vlan %d..\n",port,vid);
        groupHead = mcast_group_get_with_index(i);
        if (groupHead)
        {

            if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
            {
                continue;
            }

            if (((IGMP_DEL_PORT_FROM_ONE_VLAN == delportType) && (vid != groupHead->vid)) ||
                ((IGMP_DEL_PORT_EXCLUDE_ONE_VLAN == delportType) && (vid == groupHead->vid)))
            {
                continue;
            }

            if (IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
            {


                iRet = mcast_aggregate_db_get(groupHead,&pAggrEntry);
                if (pAggrEntry == NULL || SYS_ERR_OK != iRet)
                {
                   // SYS_PRINTF("[IGMP]%s():%d An existing group which has no aggregate record!\n", __FUNCTION__, __LINE__);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_FAILED;
                }
                /* Group */
                groupHead->p_mbr_timer[port] = uptime();
                LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);

                /* groupHead may be delete later, backup its value */
                osal_memset(&tmpEntry, 0, sizeof(tmpEntry));
                tmpEntry.vid = groupHead->vid;
                osal_memcpy(tmpEntry.mac, groupHead->mac, MAC_ADDR_LEN);
                tmpEntry.ipType = groupHead->ipType;
                LOGIC_PORTMASK_COPY(tmpEntry.mbr, groupHead->mbr);
              	osal_memcpy(&(tmpEntry.dip),&(groupHead->dip),sizeof(groupHead->dip));
				osal_memcpy(&(tmpEntry.dipv6),&(groupHead->dipv6),sizeof(groupHead->dipv6));


                pAggrEntry->p_port_ref_cnt[port]--;

                /* ASIC */
                if (pAggrEntry->p_port_ref_cnt[port] == 0)
                {
                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);
                    mcast_hw_mcst_mbr_remove(&tmpEntry, &delPmsk);
                }

				if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
                {
                	 if(pAggrEntry->group_ref_cnt==1)
                    {
                    	if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
	                    {
	                        groupHead->groupTimer = IGMP_TEMP_PERIOD;
	                    }
						else
						{
	                        if (SYS_ERR_OK != (ret = mcast_hw_mcst_entry_del(&tmpEntry)))
	                        {
	                            //SYS_DBG(IGMP_DEBUG_ABNORMAL | MLD_DEBUG_ABNORMAL, "Deleting Mcst enrty failed!  ret:%d\n", ret);
	                        }
	                        else
	                        {
	                            igmp_hw_entry_num--;
	                        }
	                        mcast_aggregate_db_del(groupHead);
                    	}

                    }
                    else
                	{
                		pAggrEntry->group_ref_cnt--;
                	}
                    if (gUnknown_mcast_action != IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                    {
                         mcast_group_del(groupHead);
                         i--;
                    }


                }

            }
        }
        else
		{
            SYS_PRINTF("mcast_groupMbrPort_DelFromOneVlan_dmacvid...failed %d !\n", i);
        }
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_groupMbrPort_DelFromOneVlan_dipsip(uint16 vid, sys_logic_port_t port,
                                               igmp_del_port_type_t delportType)
{
    uint32                  i;
    igmp_group_entry_t      *groupHead;
    igmp_group_entry_t      *pEntry;
    sys_logic_portmask_t    delPmsk;

    IGMP_DB_SEM_LOCK();

    for(i = 0; i < mcast_group_num; i++)
    {
        groupHead = mcast_group_get_with_index(i);

        if (groupHead)
        {
            if (IGMP_GROUP_ENTRY_STATIC == groupHead->form)
            {
                continue;
            }

            if (((IGMP_DEL_PORT_FROM_ONE_VLAN == delportType) && (vid != groupHead->vid)) ||
                ((IGMP_DEL_PORT_EXCLUDE_ONE_VLAN == delportType) && (vid == groupHead->vid)))
            {
                continue;
            }

            /* This group is created by only Join */
            if (groupHead->sip == 0)
            {
                groupHead->p_mbr_timer[port] = uptime();
                LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);
                osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
                LOGIC_PORTMASK_SET_PORT(delPmsk, port);
                mcast_hw_mcst_mbr_remove(groupHead, &delPmsk);
                /* sip == 0, this group doesn't have corresponding ASIC entry, just delete it */
                if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
                {
                    mcast_group_del(groupHead);
                    i--;
                }
            }
            else
            {
                /* This group is created by Data (maybe and Join) */
                pEntry = groupHead;
                while(pEntry)
                {
                    pEntry->p_mbr_timer[port] = uptime();
                    LOGIC_PORTMASK_CLEAR_PORT(pEntry->mbr, port);
                    osal_memset(&delPmsk, 0, sizeof(sys_logic_portmask_t));
                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);
                    mcast_hw_mcst_mbr_remove(pEntry, &delPmsk);

                    /* In SIP+DIP, although the portmask becomes empty, don't delete this entry from ASIC immediately */
                    if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
                    {
                        pEntry->groupTimer = IGMP_TEMP_PERIOD;
                    }

                    pEntry = pEntry->next_subgroup;
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_del_port_from_vlan(sys_vid_t vid, sys_logic_port_t port)
{
    if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode || IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode)
    {
        return mcast_groupMbrPort_DelFromOneVlan_dmacvid(vid, port, IGMP_DEL_PORT_FROM_ONE_VLAN);
    }
    else
    {
        return mcast_groupMbrPort_DelFromOneVlan_dipsip(vid, port, IGMP_DEL_PORT_FROM_ONE_VLAN);
    }
}

int mcast_ctrlGrpEntry_get(igmp_control_entry_t *ctl_entry_list, int *num)
{
	int                     i = 0, number = 0, port = 0;
	igmp_control_entry_t    *ptr_entry=NULL;

    SYS_ERR_CHK(((!ctl_entry_list || !num)), SYS_ERR_FAILED);

	ptr_entry = ctl_entry_list;
	IGMP_DB_SEM_LOCK();

	for (i = 0; i < group_sortedAry_entry_num; i++)
	{
		if (pp_group_sorted_array[i])
		{
			FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pp_group_sorted_array[i]->mbr)
			{
				osal_memcpy(ptr_entry->mac, pp_group_sorted_array[i]->mac, MAC_ADDR_LEN);
				ptr_entry->port_id = port;
				ptr_entry->vlan_id = pp_group_sorted_array[i]->vid;
				number++;
				ptr_entry++;
			}
		}

		if (number >= MAX_MC_GROUP_NUM)
		{
			break;
		}
	}
	*num = number > MAX_MC_GROUP_NUM ? MAX_MC_GROUP_NUM : number;
	IGMP_DB_SEM_UNLOCK();
	return SYS_ERR_OK;
}

int32 mcast_is_last_host_per_port(igmp_group_entry_t *pGroupHead, uint16 portId)
{
	igmp_host_list_t *ptr = NULL;

	LIST_FOREACH(ptr, &pGroupHead->p_hostListHead[portId], entries)
	{
		return FALSE;
	}
	/* this port is no client */
	return TRUE;
}

int32 mcast_host_tracking_find(uint16 portId, igmp_group_entry_t *pGroupHead, ipAddr_t clientIp)
{
	igmp_host_list_t *ptr = NULL;

	LIST_FOREACH(ptr, &pGroupHead->p_hostListHead[portId], entries)
	{
		if (!osal_memcmp(&(ptr->tableEntry.hostIp), &clientIp, sizeof(ipAddr_t)))
		{
			/* found : */
			//update last join time
			mcast_port_stats_update(SYS_OP_UPDATE, portId, pGroupHead, clientIp);
			return TRUE;
		}
	}
	/* NOT found */
	return FALSE;
}

int32 mcast_port_stats_update(op_code_t op, uint16 portId,
                                igmp_group_entry_t *pGroupHead, ipAddr_t clientIp)
{
	ipv4ActiveGrpTblEntry_t *pIpv4ActiveGrp     = NULL;
	ipv4ActiveGrpTblEntry_t *pIpv4ActiveGrpTmp  = NULL;
    ipv6ActiveGrpTblEntry_t *pIpv6ActiveGrp     = NULL;
    ipv6ActiveGrpTblEntry_t *pIpv6ActiveGrpTmp  = NULL;
	ipv4ActiveGroup_t       tmp;
    ipv6ActiveGroup_t       tmpV6;
	osal_usecs_t            joinTime;

	switch (op)
	{
		case SYS_OP_ADD:
            if (!clientIp.isIpv6B)
            {
    			pIpv4ActiveGrp = (ipv4ActiveGrpTblEntry_t *)osal_alloc(sizeof(ipv4ActiveGrpTblEntry_t));
    			pIpv4ActiveGrp->tableEntry.vlanId = pGroupHead->vid;
    			pIpv4ActiveGrp->tableEntry.srcIpAddr = pGroupHead->sip;
    			pIpv4ActiveGrp->tableEntry.dstIpAddr = pGroupHead->dip;
    			pIpv4ActiveGrp->tableEntry.actualBw = pGroupHead->imputedGrpBw;
   				pIpv4ActiveGrp->tableEntry.clientIpAddr = clientIp.ipAddr.ipv4Addr;
    			osal_time_usecs_get(&joinTime);
    			pIpv4ActiveGrp->tableEntry.recentJoinTime = joinTime / 1000000;
    			pIpv4ActiveGrp->tableEntry.reserved = 0;
    			LIST_INSERT_HEAD(&p_igmp_stats->p_port_info[portId].ipv4ActiveGrpTblHead, pIpv4ActiveGrp, entries);
    			p_igmp_stats->p_port_info[portId].ipv4ActiveGrpCount++;
            }
            else
            {
    			pIpv6ActiveGrp = (ipv6ActiveGrpTblEntry_t *)osal_alloc(sizeof(ipv6ActiveGrpTblEntry_t));
    			pIpv6ActiveGrp->tableEntry.vlanId = pGroupHead->vid;
    			osal_memcpy(pIpv6ActiveGrp->tableEntry.srcIpAddr, pGroupHead->sipv6.addr, IPV6_ADDR_LEN);
    			osal_memcpy(pIpv6ActiveGrp->tableEntry.dstIpAddr, pGroupHead->dipv6.addr, IPV6_ADDR_LEN);
    			pIpv6ActiveGrp->tableEntry.actualBw = pGroupHead->imputedGrpBw;
   				osal_memcpy(pIpv6ActiveGrp->tableEntry.clientIpAddr, clientIp.ipAddr.ipv6Addr.addr, IPV6_ADDR_LEN);
    			osal_time_usecs_get(&joinTime);
    			pIpv6ActiveGrp->tableEntry.recentJoinTime = joinTime / 1000000;
    			LIST_INSERT_HEAD(&p_igmp_stats->p_port_info[portId].ipv6ActiveGrpTblHead, pIpv6ActiveGrp, entries);
    			p_igmp_stats->p_port_info[portId].ipv6ActiveGrpCount++;
            }
            break;
		case SYS_OP_DEL:
            if (!clientIp.isIpv6B)
            {
    			memset(&tmp, 0, sizeof(ipv4ActiveGroup_t));
    			tmp.vlanId = pGroupHead->vid;
    			tmp.srcIpAddr = pGroupHead->sip;
    			tmp.dstIpAddr = pGroupHead->dip;
    			tmp.actualBw = pGroupHead->imputedGrpBw;
   				tmp.clientIpAddr = clientIp.ipAddr.ipv4Addr;
                for (pIpv4ActiveGrp = LIST_FIRST(&(p_igmp_stats->p_port_info[portId].ipv4ActiveGrpTblHead)); pIpv4ActiveGrp != NULL; )
                {
    				if (0 == osal_memcmp(&(pIpv4ActiveGrp->tableEntry), &tmp, 18))
    				{
    				    pIpv4ActiveGrpTmp = pIpv4ActiveGrp;
                        pIpv4ActiveGrp = LIST_NEXT(pIpv4ActiveGrp, entries);
    					LIST_REMOVE(pIpv4ActiveGrpTmp, entries);
    					osal_free(pIpv4ActiveGrpTmp);
    					p_igmp_stats->p_port_info[portId].ipv4ActiveGrpCount--;
    				}
                    else
                        pIpv4ActiveGrp = LIST_NEXT(pIpv4ActiveGrp, entries);
    			}
            }
            else
            {
    			memset(&tmpV6, 0, sizeof(ipv6ActiveGroup_t));
    			tmpV6.vlanId = pGroupHead->vid;
    			osal_memcpy(tmpV6.srcIpAddr, pGroupHead->sipv6.addr, IPV6_ADDR_LEN);
    			osal_memcpy(tmpV6.dstIpAddr, pGroupHead->dipv6.addr, IPV6_ADDR_LEN);
    			tmpV6.actualBw = pGroupHead->imputedGrpBw;
                osal_memcpy(tmpV6.clientIpAddr, clientIp.ipAddr.ipv6Addr.addr, IPV6_ADDR_LEN);
                for (pIpv6ActiveGrp = LIST_FIRST(&(p_igmp_stats->p_port_info[portId].ipv6ActiveGrpTblHead)); pIpv6ActiveGrp != NULL; )
                {
    				if (0 == osal_memcmp(&(pIpv6ActiveGrp->tableEntry), &tmpV6, 18))
    				{
    				    pIpv6ActiveGrpTmp = pIpv6ActiveGrp;
                        pIpv6ActiveGrp = LIST_NEXT(pIpv6ActiveGrp, entries);
    					LIST_REMOVE(pIpv6ActiveGrpTmp, entries);
    					osal_free(pIpv6ActiveGrpTmp);
    					p_igmp_stats->p_port_info[portId].ipv6ActiveGrpCount--;
    				}
                    else
                        pIpv6ActiveGrp = LIST_NEXT(pIpv6ActiveGrp, entries);
    			}
            }
			break;
		case SYS_OP_UPDATE:
            if (!clientIp.isIpv6B)
            {
                memset(&tmp, 0, sizeof(ipv4ActiveGroup_t));
                tmp.vlanId = pGroupHead->vid;
                tmp.srcIpAddr = pGroupHead->sip;
                tmp.dstIpAddr = pGroupHead->dip;
                tmp.actualBw = pGroupHead->imputedGrpBw;
                tmp.clientIpAddr = clientIp.ipAddr.ipv4Addr;
    			LIST_FOREACH(pIpv4ActiveGrp, &p_igmp_stats->p_port_info[portId].ipv4ActiveGrpTblHead, entries)
    			{
    				if (0 == osal_memcmp(&(pIpv4ActiveGrp->tableEntry), &tmp, 18))
    				{
    					osal_time_usecs_get(&joinTime);
    					pIpv4ActiveGrp->tableEntry.recentJoinTime = joinTime / 1000000;
    				}
    			}
            }
            else
            {
    			memset(&tmpV6, 0, sizeof(ipv6ActiveGroup_t));
    			tmpV6.vlanId = pGroupHead->vid;
    			osal_memcpy(tmpV6.srcIpAddr, pGroupHead->sipv6.addr, IPV6_ADDR_LEN);
    			osal_memcpy(tmpV6.dstIpAddr, pGroupHead->dipv6.addr, IPV6_ADDR_LEN);
    			tmpV6.actualBw = pGroupHead->imputedGrpBw;
                osal_memcpy(tmpV6.clientIpAddr, clientIp.ipAddr.ipv6Addr.addr, IPV6_ADDR_LEN);

    			LIST_FOREACH(pIpv6ActiveGrp, &p_igmp_stats->p_port_info[portId].ipv6ActiveGrpTblHead, entries)
    			{
    			    if (0 == osal_memcmp(&(pIpv6ActiveGrp->tableEntry), &tmpV6, 18))
    				{
    					osal_time_usecs_get(&joinTime);
    					pIpv6ActiveGrp->tableEntry.recentJoinTime = joinTime / 1000000;
    				}
    			}
            }
			break;
		default:
			SYS_DBG(LOG_DBG_IGMP, "%s() %d Not Support op=%u \n", __FUNCTION__, __LINE__, op);
	}
	return SYS_ERR_OK;
}

int32 mcast_host_tracking_add(igmp_group_entry_t *pGroupHead, ipAddr_t clientIp, uint16 portId)
{
	igmp_host_list_t *pNew = NULL;

	pNew = (igmp_host_list_t *)osal_alloc(sizeof(igmp_host_list_t));
	if (!pNew)
		return SYS_ERR_FAILED;

	osal_memcpy(&(pNew->tableEntry.hostIp), &clientIp, sizeof(ipAddr_t));
	pNew->tableEntry.mbrTimer = pGroupHead->p_mbr_timer[portId];

	LIST_INSERT_HEAD(&pGroupHead->p_hostListHead[portId], pNew, entries);

	//should update active group in p_igmp_stats per port after
	mcast_port_stats_update(SYS_OP_ADD, portId, pGroupHead, clientIp);
	return SYS_ERR_OK;
}

int32 mcast_host_tracking_del(igmp_group_entry_t *pGroupHead, ipAddr_t clientIp, uint16 portId)
{
    igmp_host_list_t *ptr  = NULL;
    igmp_host_list_t *pTmp = NULL;

    if (clientIp.isIpv6B)
    {
        SYS_DBG(LOG_DBG_MLD, "client ip"IPADDRV6_PRINT" port %d\n",
            IPADDRV6_PRINT_ARG(clientIp.ipAddr.ipv6Addr.addr), portId);
    }
    else
    {
	    SYS_DBG(LOG_DBG_IGMP, "client ip"IPADDR_PRINT" port %d\n",
            IPADDR_PRINT_ARG(clientIp.ipAddr.ipv4Addr), portId);
    }

    for (ptr = LIST_FIRST(&(pGroupHead->p_hostListHead[portId])); ptr != NULL; )
    {
		if (!osal_memcmp(&(ptr->tableEntry.hostIp), &clientIp, sizeof(ipAddr_t)))
		{
			//should update active group in igmp_stats per port first
			mcast_port_stats_update(SYS_OP_DEL, portId, pGroupHead, ptr->tableEntry.hostIp);
			SYS_DBG(LOG_DBG_IGMP,"del entry\n");
            pTmp = ptr;
            ptr = LIST_NEXT(ptr, entries);
			LIST_REMOVE(pTmp, entries);
			osal_free(pTmp);
		}
        else
        {
            ptr = LIST_NEXT(ptr, entries);
        }
	}

	return SYS_ERR_OK;
}

int32 mcast_host_tracking_del_per_port(igmp_group_entry_t *pGroupHead, uint16 portId)
{
	igmp_host_list_t *ptr       = NULL;
    igmp_host_list_t *ptrTmp    = NULL;

	for (ptr = LIST_FIRST(&(pGroupHead->p_hostListHead[portId])); ptr != NULL; )
	{
		//should update active group in p_igmp_stats per port first
		mcast_port_stats_update(SYS_OP_DEL, portId, pGroupHead, ptr->tableEntry.hostIp);
        ptrTmp = ptr;
        ptr = LIST_NEXT(ptr, entries);
		LIST_REMOVE(ptrTmp, entries);
		osal_free(ptrTmp);
	}
	return SYS_ERR_OK;
}

int32 mcast_host_tracking_update_member_time_per_port(igmp_group_entry_t *pGroupHead, uint16 portId)
{
	igmp_host_list_t *ptr = NULL;

	LIST_FOREACH(ptr, &pGroupHead->p_hostListHead[portId], entries)
	{
		if (ptr->tableEntry.mbrTimer >= 1)
			ptr->tableEntry.mbrTimer -= PASS_SECONDS;
	}
	return SYS_ERR_OK;
}

int32 mcast_group_ref_get(igmp_group_entry_t *pGroup_entry)
{
	igmp_group_entry_t *ptr = NULL;
	uint16              i, port = LOGIC_PORT_START;
	int32               ret = FALSE;
    uint32              mbrCnt = 0;

    if (!igmpCtrl.sys_has_ip_mc_tbl)
        return ret;

    for (i = 0; i < group_sortedAry_entry_num; i++)
    {
        ptr = pp_group_sorted_array[i];
        while (ptr)
        {
            if (IGMP_GROUP_ENTRY_STATIC == ptr->form ||
                ptr->dip != pGroup_entry->dip)
            {
                ptr = ptr->next_subgroup;;
                continue;
            }
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup_entry->mbr)
            {
                if (IS_LOGIC_PORTMASK_PORTSET(ptr->mbr, port) && ptr->p_mbr_timer[port] != 0)
                {
                    mbrCnt++;
                    SYS_DBG(LOG_DBG_IGMP,"%s(), mbrCnt=%d, mbrTimer=%u\n",
                        __FUNCTION__, mbrCnt, ptr->p_mbr_timer[port]);
                }
            }
            ptr = ptr->next_subgroup;
        }
    }

    ret = (mbrCnt == 0 ? FALSE : TRUE);

	return ret;
}

int32 mcast_profile_time_intv_get(
    intv_type_t type, uint32 aclEntryId, mcast_preview_info_t *pPreviewInfo, uint32 *pIntv)
{
	uint32              id          = aclEntryId >> 16;
	mcast_prof_entry_t  *ptr        = NULL;
	uint8               robustness  = DFLT_IGMP_ROBUSTNESS_VAR;

	LIST_FOREACH(ptr, &mcastProfHead, entries)
	{
		if (ptr->entry.mopId == id)
		{
			if (INTV_TYPE_GROUP_MBR == type)
			{
				robustness = ((0 == ptr->entry.robustness) ? DFLT_IGMP_ROBUSTNESS_VAR : ptr->entry.robustness);
				*pIntv = ((pPreviewInfo && MCAST_NON_PREVIEW !=
					pPreviewInfo->previewType) ? ((MCAST_ALLOWED_PREVIEW == pPreviewInfo->previewType &&
					0 == pPreviewInfo->length) ? UINT_MAX : pPreviewInfo->length) : (((robustness) *
					(ptr->entry.queryIntval)) + ptr->entry.queryMaxRspTime/10));
			}
			else
			{
				*pIntv = (ptr->entry.lastMbrQueryIntval) / 10;

				if (0 == *pIntv)
					*pIntv = DFLT_IGMP_GS_QUERY_RESP_INTV;
			}
			return SYS_ERR_OK;
		}
	}
	return SYS_ERR_OK;
}

