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
* $Revision: 10018 $
* $Date: 2010-06-04 13:57:15 +0800 (Fri, 04 Jun 2010) $
*
* Purpose : Export the public APIs in lower layer module in the Realtek Protocols.
*
* Feature : Export the public APIs in lower layer module
*
*/

/*
* Include Files
*/
#include <igmp/inc/proto_igmp.h>


//drv header
#include "pf_vlan.h"
#include "pf_mac.h"


/*
* Symbol Definition
*/

extern uint8                    igmp_trapDataPkt_lock;
extern uint32                   igmp_hw_entry_num;
//extern igmp_group_head_entry_t  *p_group_head_db;
extern igmp_static_group_t      groupSave;
extern sys_vlanmask_t           mcastVlanCfgMsk[MULTICAST_TYPE_END];

static sys_igmp_profile_t       **pp_profile_db;
volatile int32                  igmp_thread_status = 0;

osal_mutex_t                    igmp_sem_pkt;
osal_mutex_t                    igmp_sem_db;
sys_enable_t                    igmp_packet_stop = DISABLED;
sys_enable_t                    mcast_timer_stop = DISABLED;
sys_enable_t                    mcast_groupAdd_stop = DISABLED;
uint32                          *p_igmp_tag_oper_per_port;/* transparent, strip, translation */
igmp_vlan_translation_entry_t   *p_igmp_tag_translation_table;
igmp_mc_vlan_entry_t            igspVlanMap[4096];
int32                           igmpMcVlanNum = 0;
uint32                          *p_rx_counter_current;
uint32                          *p_remain_packets;
uint32                          *p_rx_counter;
uint32                          *p_mcast_total_rx_counter;
uint32                          *p_mcast_rx_drop_counter;

struct mcast_port_info_head_t portInfoHead;
struct mcast_prof_head_t mcastProfHead;
struct mcast_allowed_preview_head_t allowedPreviewHead;



static int32 _mcast_hwEntryByVid_del(sys_vlanmask_t *pVlanMask, multicast_ipType_t ipType, mcast_group_type_t type);
static int32 _mcast_igmp_group_timer_dipsip(void);
static int32 _mcast_group_timer_dmacvid(void);
static int32 mcast_igmp_snooping_stats_init(int32 boot);
static int32 mcast_mld_snooping_stats_init(int32 boot);
static void mcast_igmp_group_head_timer(void);
static int mcast_igmpmcvlancheck(void);

sys_igmp_func_t                 igmp_lookup_mode_func[] =
{
     /*IPV4*/
    /* IGMP_DMAC_VID_MODE */
    {
        .igmp_group_timer = _mcast_group_timer_dmacvid,//TBD: current bandwidth
        .igmp_groupMbrPort_del_wrapper = mcast_groupMbrPort_del_wrapper_dmacvid,//TBD: current bandwidth
        .igmp_group_mcstData_add_wrapper = mcast_group_mcstData_add_wrapper_dmacvid,
        .igmpv3_group_mcstData_add_wrapper = mcast_group_mcstData_add_wrapper_dmacvid,
    },

    /* IGMP_DIP_SIP_MODE */
    {
        .igmp_group_timer = _mcast_igmp_group_timer_dipsip,
        .igmp_groupMbrPort_del_wrapper = mcast_groupMbrPort_del_wrapper_dipsip,
        .igmp_group_mcstData_add_wrapper = mcast_igmp_group_mcstData_add_wrapper_dipsip,
        .igmpv3_group_mcstData_add_wrapper = mcast_igmpv3_group_mcstData_add_wrapper_dipsip,
    },


    /* IGMP_DIP_VID_MODE */
    {
    	.igmp_group_timer = _mcast_igmp_group_timer_dipsip,
    	.igmp_groupMbrPort_del_wrapper = mcast_groupMbrPort_del_wrapper_dipsip,
    	.igmp_group_mcstData_add_wrapper = mcast_igmp_group_mcstData_add_wrapper_dipsip,
    	.igmpv3_group_mcstData_add_wrapper = mcast_igmpv3_group_mcstData_add_wrapper_dipsip,
    },

    /* IGMP_DIP_MODE */
    {
        .igmp_group_timer = _mcast_igmp_group_timer_dipsip,
        .igmp_groupMbrPort_del_wrapper = mcast_groupMbrPort_del_wrapper_dipsip,
        .igmp_group_mcstData_add_wrapper = mcast_igmp_group_mcstData_add_wrapper_dipsip,
        .igmpv3_group_mcstData_add_wrapper = mcast_igmpv3_group_mcstData_add_wrapper_dipsip,
    },

     /*IPV6*/
    /* IGMP_DMAC_VID_MODE */
    {
        .igmp_group_timer = _mcast_group_timer_dmacvid,
        .igmp_groupMbrPort_del_wrapper = mcast_groupMbrPort_del_wrapper_dmacvid,
        .igmp_group_mcstData_add_wrapper = mcast_group_mcstData_add_wrapper_dmacvid,
        .igmpv3_group_mcstData_add_wrapper = mcast_group_mcstData_add_wrapper_dmacvid,
    },

    /* IGMP_DIP_SIP_MODE */
    {
        .igmp_group_timer = NULL,
        .igmp_groupMbrPort_del_wrapper = NULL,
        .igmp_group_mcstData_add_wrapper = NULL,
        .igmpv3_group_mcstData_add_wrapper = NULL,
    },


    /* IGMP_DIP_VID_MODE */
    {
        .igmp_group_timer = NULL,
        .igmp_groupMbrPort_del_wrapper = NULL,
        .igmp_group_mcstData_add_wrapper = NULL,
        .igmpv3_group_mcstData_add_wrapper = NULL,
    },

    /* IGMP_DIP_MODE */
    {
        .igmp_group_timer = NULL,
        .igmp_groupMbrPort_del_wrapper = NULL,
        .igmp_group_mcstData_add_wrapper = NULL,
        .igmpv3_group_mcstData_add_wrapper = NULL,
    },

};

int32 mcast_max_life_get(igmp_group_entry_t *group_entry)
{
    int max = 0;
    sys_logic_port_t port;

    SYS_PARAM_CHK(NULL == group_entry, SYS_ERR_FAILED);     /* Note: Should return negative value */

    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, group_entry->mbr)
    {
        if (max < group_entry->p_mbr_timer[port])
            max = group_entry->p_mbr_timer[port];
    }

    return max;
}

int32 mcast_hw_mcst_mbr_remove(igmp_group_entry_t *pGroup, sys_logic_portmask_t *delPmsk)
{
    int32                   ret = 0;
    sys_ipMcastEntry_t      ipMcstEntry;
    sys_l2McastEntry_t      l2McstEntry;
    sys_logic_port_t        normalPort, port;
    sys_logic_portmask_t    routPmsk, removePmsk;
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    igmp_group_entry_t      group_entry;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK(NULL == delPmsk, SYS_ERR_NULL_POINTER);

    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));
    osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));

    osal_memset(&group_entry, 0, sizeof(igmp_group_entry_t));
    osal_memcpy(&group_entry, pGroup, sizeof(igmp_group_entry_t));

    LOGIC_PORTMASK_CLEAR_ALL(routPmsk);
    LOGIC_PORTMASK_CLEAR_ALL(removePmsk);
    LOGIC_PORTMASK_COPY(removePmsk, *delPmsk);

    mcast_routerPort_get(group_entry.vid, group_entry.ipType, &routPmsk);
    LOGIC_PORTMASK_ANDNOT(removePmsk, removePmsk, routPmsk);

    SYS_DBG(LOG_DBG_IGMP,
        "delPmk=%u, rtPmk=%u, removePmk=%u, lookupMode=%u\n",
        delPmsk->bits[0], routPmsk.bits[0], removePmsk.bits[0], p_igmp_stats->lookup_mode);

    if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
        (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
    {
        /* exist hw entry, so sw entry should exist */
        mcast_aggregate_db_get(pGroup, &pAggrEntry);
        if (pAggrEntry == NULL)
        {
            /* not found in sw aggr. db*/
            SYS_DBG(group_entry.ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP: LOG_DBG_MLD,
               "[Multicast]%s():%d An existing group which has no aggregate record!  pGroup->dip:"
               IPADDR_PRINT"\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(group_entry.dip));
            return SYS_ERR_FAILED;
        }
        /* found in sw aggr. db */
        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, removePmsk)
        {
            SYS_DBG(LOG_DBG_IGMP, "refer count=%u\n", pAggrEntry->p_port_ref_cnt[port]);
            if (pAggrEntry->p_port_ref_cnt[port] > 0)
                pAggrEntry->p_port_ref_cnt[port]--;
            if (0 != pAggrEntry->p_port_ref_cnt[port])
            {
                /* still different dip mapping to the same mac
                    so, this port cannot remove */
                LOGIC_PORTMASK_CLEAR_PORT(removePmsk, port);
            }
        }

        SYS_ERR_CHK((IS_LOGIC_PORTMASK_CLEAR(removePmsk)), SYS_ERR_FAILED);

        l2McstEntry.vid = group_entry.vid;
        l2McstEntry.care_vid = group_entry.care_vid;
        osal_memcpy(l2McstEntry.mac, group_entry.mac, MAC_ADDR_LEN);

        //update hw entry, remove port mask
        if (SYS_ERR_OK == (ret = rsd_l2McastEntry_get(&l2McstEntry)))
        {
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(normalPort, removePmsk)
            {
                LOGIC_PORTMASK_CLEAR_PORT(l2McstEntry.portmask, normalPort);
            }
            //LOGIC_PORTMASK_ANDNOT(l2McstEntry.portmask, l2McstEntry.portmask, *delPmsk);
            if (IS_LOGIC_PORTMASK_CLEAR(l2McstEntry.portmask))
                ret = mcast_hw_mcst_entry_del(&group_entry);
            else
                ret = mcast_hw_l2McastEntry_set(group_entry.ipType, &l2McstEntry);
        }

        SYS_DBG(LOG_DBG_IGMP,
            "ret:%d  dip:"IPADDR_PRINT"  sip:"IPADDR_PRINT"  vid:%d, isGrpRefB=%u\n",
            ret, IPADDR_PRINT_ARG(group_entry.dip), IPADDR_PRINT_ARG(group_entry.sip), group_entry.vid, ipMcstEntry.isGrpRefB);
    }
    else
    {
        if (IS_LOGIC_PORTMASK_CLEAR(removePmsk))
            return SYS_ERR_OK;

        if (MULTICAST_TYPE_IPV4 == group_entry.ipType)
        {
            ipMcstEntry.dip = group_entry.dip;
            ipMcstEntry.sip = group_entry.sip;
            ipMcstEntry.vid = group_entry.vid;
    		ipMcstEntry.fmode = group_entry.fmode;
    		ipMcstEntry.isGrpRefB = mcast_group_ref_get(&group_entry);

            if (SYS_ERR_OK == (ret = rsd_ipMcastEntry_get(&ipMcstEntry)))
            {
    			if (MODE_GPON == igmpCtrl.ponMode)
    				LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, group_entry.mbr);

                FOR_EACH_PORT_IN_LOGIC_PORTMASK(normalPort, removePmsk)
                {
                    LOGIC_PORTMASK_CLEAR_PORT(ipMcstEntry.portmask, normalPort);
                }
                //LOGIC_PORTMASK_ANDNOT(ipMcstEntry.portmask, ipMcstEntry.portmask, *delPmsk);
                ret = rsd_ipMcastEntry_set(&ipMcstEntry);

    			SYS_DBG(LOG_DBG_IGMP,
                    "%s() %d ret:%d  dip:"IPADDR_PRINT"  sip:"IPADDR_PRINT"  vid:%d, isGrpRefB=%u\n",
                    __FUNCTION__, __LINE__, ret, IPADDR_PRINT_ARG(group_entry.dip),
                    IPADDR_PRINT_ARG(group_entry.sip), group_entry.vid, ipMcstEntry.isGrpRefB);
            }
        }
        else if (MULTICAST_TYPE_IPV6 == group_entry.ipType)
        {
            l2McstEntry.vid = group_entry.vid;
            l2McstEntry.care_vid = group_entry.care_vid;
            osal_memcpy(l2McstEntry.mac, group_entry.mac, MAC_ADDR_LEN);

            if (SYS_ERR_OK == (ret = rsd_l2McastEntry_get(&l2McstEntry)))
            {
    			if (MODE_GPON == igmpCtrl.ponMode)
    				LOGIC_PORTMASK_COPY(l2McstEntry.portmask, group_entry.mbr);

                FOR_EACH_PORT_IN_LOGIC_PORTMASK(normalPort, removePmsk)
                {
                    LOGIC_PORTMASK_CLEAR_PORT(l2McstEntry.portmask, normalPort);
                }
                //LOGIC_PORTMASK_ANDNOT(ipMcstEntry.portmask, ipMcstEntry.portmask, *delPmsk);
                if (IS_LOGIC_PORTMASK_CLEAR(l2McstEntry.portmask))
                    ret = mcast_hw_mcst_entry_del(&group_entry);
                else
                    ret = rsd_l2McastEntry_set(&l2McstEntry);

    			SYS_DBG(LOG_DBG_IGMP,
                    "%s() %d ret:%d  dip:"IPADDRV6_PRINT"  sip:"IPADDRV6_PRINT"  vid:%d\n",
                    __FUNCTION__, __LINE__, ret, IPADDRV6_PRINT_ARG(group_entry.dipv6.addr),
                    IPADDRV6_PRINT_ARG(group_entry.sipv6.addr), group_entry.vid);
            }
        }
    }

    return ret;
}

int32 mcast_hw_mcst_entry_del(igmp_group_entry_t *group_entry)
{
    sys_ipMcastEntry_t      ipMcstEntry;
    sys_l2McastEntry_t      l2McstEntry;
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    sys_logic_portmask_t    delPmsk;
    //uint32                freeCount;

    SYS_PARAM_CHK(NULL == group_entry, SYS_ERR_NULL_POINTER);

    if ((MULTICAST_TYPE_IPV4 == group_entry->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
        (MULTICAST_TYPE_IPV6 == group_entry->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
    {
        l2McstEntry.vid = group_entry->vid;
        l2McstEntry.care_vid = group_entry->care_vid;

        osal_memcpy(l2McstEntry.mac, group_entry->mac, MAC_ADDR_LEN);

        mcast_aggregate_db_get(group_entry, &pAggrEntry);

        if (pAggrEntry == NULL)
        {
            if (MULTICAST_TYPE_IPV4 == group_entry->ipType)
            {
                SYS_DBG(LOG_DBG_IGMP,
                    "%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDR_PRINT"\n",
                    __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(group_entry->dip));
            }
            else
            {
                SYS_DBG(LOG_DBG_MLD,
                    "%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDRV6_PRINT"\n",
                    __FUNCTION__, __LINE__, IPADDRV6_PRINT_ARG(group_entry->dipv6.addr));
            }

            rsd_l2McastEntry_del(&l2McstEntry);
#if 0 //debug
            rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);
            if (freeCount != 1024 - mcast_group_num + 1)
            {
                SYS_PRINTF("Asic free count : %d,  soft free count : %d \n", freeCount, 1024 - mcast_group_num + 1);
            }
#endif
            return SYS_ERR_FAILED;
        }

        //last group ref;
        if (1 == pAggrEntry->group_ref_cnt)
        {
            pAggrEntry->group_ref_cnt--;

            mcast_aggregate_db_del(group_entry);

            return rsd_l2McastEntry_del(&l2McstEntry);
        }
        else if (pAggrEntry->group_ref_cnt > 1)
        {
            LOGIC_PORTMASK_CLEAR_ALL(delPmsk);

            LOGIC_PORTMASK_COPY(delPmsk, group_entry->mbr);

            pAggrEntry->group_ref_cnt--;

            if (!IS_LOGIC_PORTMASK_CLEAR(delPmsk))
            {
                return mcast_hw_mcst_mbr_remove(group_entry, &delPmsk);
            }
        }
    }
    else
    {
        if (MULTICAST_TYPE_IPV4 == group_entry->ipType)
        {
            ipMcstEntry.dip = group_entry->dip;
            ipMcstEntry.sip = group_entry->sip;
            ipMcstEntry.vid = group_entry->vid;
		    ipMcstEntry.fmode = group_entry->fmode;
		    ipMcstEntry.isGrpRefB = mcast_group_ref_get(group_entry);
            ipMcstEntry.care_vid = group_entry->care_vid;
            return rsd_ipMcastEntry_del(&ipMcstEntry);
        }
        else
        {
            l2McstEntry.vid = group_entry->vid;
            l2McstEntry.care_vid = group_entry->care_vid;
            osal_memcpy(l2McstEntry.mac, group_entry->mac, MAC_ADDR_LEN);
            // TBD unkown multicast vlan, using fid 0
            if (0 == group_entry->vid)
            {
                //fid 0
                SYS_DBG(LOG_DBG_IGMP, "%s %d, del by fid 0", __FUNCTION__, __LINE__);
                return rsd_l2McastEntry_del(&l2McstEntry);
            }
            else
            {

                SYS_DBG(LOG_DBG_IGMP, "%s %d, del by vid + mac", __FUNCTION__, __LINE__);
                // vid + mac
            }
        }
    }

    return SYS_ERR_FAILED;
}

int32 mcast_hw_l2McastEntry_add(multicast_ipType_t ipType, sys_l2McastEntry_t *pEntry)
{
    mcast_vlan_entry_t      mcastVlan;
    sys_logic_portmask_t    logicMbr;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);

    LOGIC_PORTMASK_CLEAR_ALL(logicMbr);
    LOGIC_PORTMASK_OR(logicMbr, logicMbr, pEntry->portmask);

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        osal_memset(&mcastVlan, 0, sizeof(mcastVlan));
        mcastVlan.vid = pEntry->vid;
        mcastVlan.ipType = ipType;

        SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlan)), SYS_ERR_FAILED);

        if (0 != mcastVlan.vid)
        {
            LOGIC_PORTMASK_OR(logicMbr, logicMbr, mcastVlan.staticMbr);
            LOGIC_PORTMASK_ANDNOT(logicMbr, logicMbr, mcastVlan.forbidMbr);
        }
    }
    LOGIC_PORTMASK_COPY(pEntry->portmask, logicMbr);

    return rsd_l2McastEntry_add(pEntry);
}

int32 mcast_hw_l2McastEntry_set(multicast_ipType_t ipType, sys_l2McastEntry_t *pEntry)
{
    mcast_vlan_entry_t    mcastVlan;
    sys_logic_portmask_t logicMbr;

    SYS_PARAM_CHK((NULL == pEntry), SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK((ipType > MULTICAST_TYPE_END) || (ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);

    LOGIC_PORTMASK_CLEAR_ALL(logicMbr);
    LOGIC_PORTMASK_OR(logicMbr, logicMbr, pEntry->portmask);

    if (MODE_GPON != igmpCtrl.ponMode)
    {
        osal_memset(&mcastVlan, 0, sizeof(mcastVlan));
        mcastVlan.vid = pEntry->vid;
        mcastVlan.ipType = ipType;

        SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_get(&mcastVlan)), SYS_ERR_FAILED);

        if (0 != mcastVlan.vid)
        {
            LOGIC_PORTMASK_OR(logicMbr, logicMbr, mcastVlan.staticMbr);
            LOGIC_PORTMASK_ANDNOT(logicMbr, logicMbr, mcastVlan.forbidMbr);
        }
    }
    LOGIC_PORTMASK_COPY(pEntry->portmask, logicMbr);

    return rsd_l2McastEntry_set(pEntry);
}

/* Function Name:
*      mcast_table_clear
* Description:
*      Clears IGMP table stored in ASIC
* Input:
*      None
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_table_clear(void)
{
    mcast_group_del_all();

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_unknMcastAction_set
* Description:
*      Sets unknow multicast action e.g. DROP/FLOOD
* Input:
*      action - Unknown multicast action, DROP/FLOOD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_unknMcastAction_set(uint8 action)
{
    uint32 oldAction = gUnknown_mcast_action;

    if (IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode)
    {
        /* During DMAC+VID mode, unknown multicast action implements by lookup miss action. */
        if (action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
        {
            //TBD: SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, FRAME_TRAP_ACTION_FLOOD_IN_VLAN));
            if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == oldAction)
            {
                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, DISABLED));  wanghuanyu

                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, DISABLED));
                mcast_group_learnByData_del();
            }
        }
        else if (action == IGMP_UNKNOWN_MCAST_ACTION_DROP)
        {
            //TBD: SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, FRAME_TRAP_ACTION_DROP));
            if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == oldAction)
            {

                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, DISABLED));   wanghuanyu
                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, DISABLED));
                mcast_group_learnByData_del();
            }
        }
        else if (action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
        {
            //TBD: SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, FRAME_TRAP_ACTION_TRAP2CPU));
            if (DISABLED == p_igmp_stats->igmpsnp_enable)
            {
                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, ENABLED));    wanghuanyu
            }
            else if (DISABLED == p_mld_stats->mldsnp_enable)
            {
                //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, ENABLED));
            }
        }

        gUnknown_mcast_action = action;
    }
    else
    {
        /*
        During DIP+SIP mode,  unknown multicast action implements by software first and l2 table
        ipmc portmask. Unknown Multicast data packets will be always trap to CPU first,
        don't change lookup miss action of DLF_TYPE_IPMC except group database has full.
        */
        gUnknown_mcast_action = action;
		//TBD: SYS_ERR_CHK(rsd_l2_lookupMissAction_set(DLF_TYPE_MCAST, gUnknown_mcast_action));
        mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_DYNAMIC);
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_portGroup_excessAct_get
* Description:
*      Gets mcast port excess max-group action
* Input:
*      ipType - ip type
*      port - logic port id
* Output:
*      pAct - excess max-group action
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_portGroup_excessAct_get(multicast_ipType_t ipType, sys_logic_port_t port, igmp_maxGroupAct_t *pAct)
{
    SYS_PARAM_CHK((NULL == pAct), SYS_ERR_NULL_POINTER);

    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        *pAct = p_igmp_stats->p_max_groups_act[port];
    }
    else
    {
        *pAct = p_mld_stats->p_max_groups_act[port];
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_portGroup_limit_get
* Description:
*      Gets igmp port max limit group
* Input:
*      port - logic port id
*      pMaxnum - max num of group
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_portGroup_limit_get(multicast_ipType_t ipType, sys_logic_port_t port, uint16 *pMaxnum)
{
    SYS_PARAM_CHK((NULL == pMaxnum), SYS_ERR_NULL_POINTER);

    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

    if (MULTICAST_TYPE_IPV4 == ipType)
    {
        *pMaxnum = p_igmp_stats->p_port_entry_limit[port];
    }
    else
    {
        *pMaxnum = p_mld_stats->p_port_entry_limit[port];
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      _mcast_hwEntryByVid_del
* Description:
*      Delete hw entry by vid
* Input:
*      pVlanMask -- vlan mask
*      ipType --ip type
*      type -- group type
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      ip type is MULTICAST_TYPE_END, not check ip type.
*/
static int32 _mcast_hwEntryByVid_del(sys_vlanmask_t *pVlanMask, multicast_ipType_t ipType, mcast_group_type_t type)
{
    uint32 i = 0;
    uint32 ret = SYS_ERR_OK;
    igmp_group_entry_t *pEntry = NULL;
    uint32 tmpNum;

    SYS_PARAM_CHK((NULL == pVlanMask), SYS_ERR_NULL_POINTER);

    mcast_group_rx_timer_stop();

    for (i = 0 ; i < mcast_group_num ; i++)
    {
        pEntry = mcast_group_get_with_index(i);

        if (!pEntry)
        {
            continue;
        }
        if (MULTICAST_TYPE_END != ipType)
        {
            if (ipType != pEntry->ipType)
                continue;
        }

        if (IGMP_GROUP_ENTRY_ALL !=  type)
        {
            if (type != pEntry->form)
                continue;
        }

        if (VLANMASK_IS_VIDSET(*pVlanMask,pEntry->vid))
        {
            ret = mcast_hw_mcst_entry_del(pEntry);

            if (SYS_ERR_OK == ret)
            {
                igmp_hw_entry_num--;
            }

            tmpNum = mcast_group_num;
            mcast_group_del(pEntry);
            if (tmpNum != mcast_group_num)
                i--;

        }
    }

    mcast_group_rx_timer_start();
    return SYS_ERR_OK;
}

/* Function Name:
*      _mcast_vlan_handler
* Description:
*      Handle Vlan create and destroy event
* Input:
*      pVlanmask - VLAN configuration
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 _mcast_vlan_handler(void* pData)
{
    sys_vlan_event_t *pVlanEvent = NULL;
    igmp_querier_entry_t *pEntry = NULL;
    sys_vlanmask_t vlanMask;
    multicast_ipType_t ipType;
    //sys_vlan_operEntry_t vlanEntry; wanghuanyu

    SYS_PARAM_CHK((NULL == pData), SYS_ERR_NULL_POINTER);

    pVlanEvent = (sys_vlan_event_t *)pData;

    //not hanler the dynamic vlan;
    #if 0
    osal_memset(&vlanEntry,0,sizeof(sys_vlan_operEntry_t));
    SYS_ERR_CHK(rsd_vlan_operEntry_get(pVlanEvent->vid, &vlanEntry));
    if (SYS_VLAN_ENTRY_TYPE_DYNAMIC ==  vlanEntry.type)
        return SYS_ERR_OK;
	#endif
    IGMP_DB_SEM_LOCK();

    if (VLAN_EVENT_CREATE == pVlanEvent->eventType)
    {
        for (ipType = MULTICAST_TYPE_IPV4; ipType < MULTICAST_TYPE_END ; ipType++)
        {
            mcast_querier_db_add(pVlanEvent->vid, ipType);
        }
    }
    else if (VLAN_EVENT_DESTROY == pVlanEvent->eventType)
    {
        for (ipType = MULTICAST_TYPE_IPV4; ipType < MULTICAST_TYPE_END; ipType++)
        {
            mcast_querier_db_get(pVlanEvent->vid, ipType, &pEntry);
            if (pEntry)
            {
                mcast_querier_db_del(pVlanEvent->vid, ipType);
            }
            else
            {
                if (MULTICAST_TYPE_IPV4 == ipType)
                    SYS_DBG(LOG_DBG_IGMP, "Warring! IGMP querier entry for VLAN-%d doesn't exist!\n", pVlanEvent->vid);
                else
                    SYS_DBG(LOG_DBG_MLD, "Warring! MLD querier entry for VLAN-%d doesn't exist!\n", pVlanEvent->vid);
            }
        }

        VLANMASK_CLEAR_ALL(vlanMask);
        VLANMASK_SET_VID(vlanMask, pVlanEvent->vid);
        _mcast_hwEntryByVid_del(&vlanMask, MULTICAST_TYPE_END, IGMP_GROUP_ENTRY_DYNAMIC);
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int ext_mcast_vlan_del(int vid)
{
	sys_vlan_event_t vlanEvent;
	uint16				sortedIdx;
	mcast_vlan_entry_t  *entry = NULL;
	igmp_router_entry_t *pEntry = NULL;
	multicast_ipType_t ipType;

	if (p_igmp_stats->igmpsnp_enable != DISABLED)
    {
        ipType = MULTICAST_TYPE_IPV4;

        mcast_vlan_array_search(vid, ipType, &sortedIdx, &entry);

        if (entry != NULL)
        {
            mcast_vlan_db_del(vid, ipType);
        }

        mcast_router_db_get(vid, ipType, &pEntry);

        if (NULL != pEntry)
        {
            mcast_router_db_del(vid, ipType);
        }
    }

    if (p_mld_stats->mldsnp_enable != DISABLED)
    {
        ipType = MULTICAST_TYPE_IPV6;
        entry  = NULL;
        pEntry = NULL;

        mcast_vlan_array_search(vid, ipType, &sortedIdx, &entry);

        if (entry != NULL)
        {
            mcast_vlan_db_del(vid, ipType);
        }

        mcast_router_db_get(vid, ipType, &pEntry);

        if (NULL != pEntry)
        {
            mcast_router_db_del(vid, ipType);
        }
    }
    vlanEvent.vid = vid;
    vlanEvent.eventType = VLAN_EVENT_DESTROY;
    return _mcast_vlan_handler((void *)&vlanEvent);
}

/* Function Name:
*      mcast_router_get
* Description:
*      Retrieves router information
* Input:
*      pRouter - Router information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_router_get(igmp_router_entry_t *pRouter)
{
    igmp_router_entry_t *tmp;

    SYS_PARAM_CHK(NULL == pRouter, SYS_ERR_NULL_POINTER);

    mcast_router_db_get(pRouter->vid, pRouter->ipType, &tmp);

    if (tmp)
    {
        osal_memcpy(pRouter, tmp, sizeof(igmp_router_entry_t));
    }
    else
        osal_memset(pRouter, 0, sizeof(igmp_router_entry_t));

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_router_set
* Description:
*      set router database
* Input:
*      pRouter - Router information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_router_set(igmp_router_entry_t *pRouter)
{
    return mcast_router_db_set(pRouter);
}

/* Function Name:
*      mcast_router_dynPortMbr_del
* Description:
*      delete dynamic router port
* Input:
*      ipType - ip Type
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_router_dynPortMbr_del(multicast_ipType_t ipType)
{
    sys_vid_t vid;
    igmp_router_entry_t  *pRouterEntry;
    sys_logic_portmask_t dynPmsk;

    sys_logic_port_t port;
    sys_vlanmask_t vlanMsk;

    SYS_PARAM_CHK((ipType< MULTICAST_TYPE_IPV4 ||ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

	osal_memset(vlanMsk, 0x00, sizeof(sys_vlanmask_t));
	//   (VOID)CFG_GetExistVlanList(vlanMsk);
	VLANMASK_SET_ALL(vlanMsk);//rsd_vlan_entryExist_get(vlanMsk);
    ForEachListVlan(vid, vlanMsk)
    {
        mcast_router_db_get(vid , ipType, &pRouterEntry);
        if (NULL != pRouterEntry)
        {
            LOGIC_PORTMASK_CLEAR_ALL(dynPmsk);
            LOGIC_PORTMASK_ANDNOT(dynPmsk, pRouterEntry->router_pmsk, pRouterEntry->router_static_pmsk);

            if (IS_LOGIC_PORTMASK_CLEAR(pRouterEntry->router_static_pmsk) &&
                IS_LOGIC_PORTMASK_CLEAR(pRouterEntry->router_forbid_pmsk))
            {
                mcast_router_db_del(vid, ipType);
            }

            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, dynPmsk)
            {
                LOGIC_PORTMASK_CLEAR_PORT(pRouterEntry->router_pmsk, port);
                pRouterEntry->p_port_timer[port] = 0;
            }

            if (IS_LOGIC_PORTMASK_CLEAR(dynPmsk))
            {
                VLANMASK_CLEAR_ALL(vlanMsk);
                VLANMASK_SET_VID(vlanMsk, vid);
                mcast_routerPortMbr_remove(ipType, &vlanMsk,  &dynPmsk);
            }
        }
    }

    return SYS_ERR_OK;
}

/* Function Name:
*       mcast_basic_static_groupEntry_set
* Description:
*        Set IGMP v3-basic  static group entry.
* Input:
*       pGroup - group entry
*       fmode - include /exclude
*       isAdd - True/False
* Output:
*        None
* Return:
*       SYS_ERR_OK
*       SYS_ERR_FAILED;
* Note:
*       pGroup->mbr  is the input portmask.
*/
int32 mcast_basic_static_groupEntry_set(igmp_group_entry_t *pGroup,  uint8 fmode, uint8 isAdd)
{
    uint8                   zeroIpv6[IPV6_ADDR_LEN], isSetAsic = TRUE;
    uint16                  sortedIdx;
    int32                   ret;
    igmp_group_entry_t      *pEntry, *groupHead;
    sys_ipMcastEntry_t      ipMcstEntry;
    sys_logic_port_t        port = 0, firstPort =0;
    sys_l2McastEntry_t      l2McstEntry;
    sys_logic_portmask_t    rtPmsk, addInMbr, addExMbr, delExMbr, delInMbr;
    igmp_aggregate_entry_t  *pAggrEntry = NULL;
    mcast_vlan_entry_t      *pMcastVlan = NULL;

    SYS_PARAM_CHK((NULL == pGroup), SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK((pGroup->ipType < MULTICAST_TYPE_IPV4 || pGroup->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));
    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));

    if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
    {
        if (!IGMP_IN_MULTICAST(ntohl(pGroup->dip)))
        {
            SYS_PRINTF("IGMP Group address Error: Group Address("IPADDR_PRINT") is not in "
                "Multicast range(224.0.0.0~239.255.255.255)\n", IPADDR_PRINT_ARG(pGroup->dip));
            return SYS_ERR_FAILED;
        }

        if ((ntohl(pGroup->dip) >= ntohl(0xE0000000L)) && (ntohl(pGroup->dip) <= ntohl(0xE00000FFL)))
        {
            SYS_PRINTF("IGMP Group address  Error: Group Address("IPADDR_PRINT") belongs to "
                "Reserved Multicast range(224.0.0.0~224.0.0.255)!\n", IPADDR_PRINT_ARG(pGroup->dip));
            return SYS_ERR_FAILED;
        }
    }
    else
    {
        osal_memset(zeroIpv6,0, IPV6_ADDR_LEN);
        if (pGroup->dipv6.addr[0] != 0xff)
        {
            SYS_PRINTF("MLD Group address Error: Group Address("IPADDRV6_PRINT") is not  "
                "Multicast  address \n", IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));
            return SYS_ERR_FAILED;
        }

        /*  rfc4541,
            MLD messages are also not sent regarding groups with addresses in the
            range FF00::/15 (which encompasses both the reserved FF00::/16 and
            node-local FF01::/16 IPv6 address spaces).  These addresses should
            never appear in packets on the link.*/
        if ((pGroup->dipv6.addr[1] == 0x0 || pGroup->dipv6.addr[1] == 0x01) &&
              (osal_memcmp(&pGroup->dipv6.addr[2], &zeroIpv6[2] ,IPV6_ADDR_LEN-2) == 0)  )
        {
            SYS_PRINTF("MLD Group address Error: Group Address("IPADDRV6_PRINT") is "
                "invalid Multicast group address !\n", IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));
            return SYS_ERR_FAILED;
        }

        pGroup->dip = IPV6_TO_IPV4(pGroup->dipv6.addr);
    }

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);

    if (groupHead)
    {
        //Delete the dynamic entry.
        if (IGMP_GROUP_ENTRY_DYNAMIC == groupHead->form)
        {
            if (FALSE == isAdd)
            {
                return SYS_ERR_IGMP_GROUP_ENTRY_NOT_EXIST;
            }
            else
                mcast_basic_static_group_clear(pGroup);
        }
        else
        {
            IGMP_DB_SEM_LOCK();

            LOGIC_PORTMASK_CLEAR_ALL(addInMbr);
            LOGIC_PORTMASK_CLEAR_ALL(addExMbr);
            LOGIC_PORTMASK_CLEAR_ALL(delExMbr);
            LOGIC_PORTMASK_CLEAR_ALL(delInMbr);

            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup->mbr)
            {
                if (TRUE == isAdd)
                {
                    //LOGIC_PORTMASK_SET_PORT(groupHead->staticMbr, port);
                    if (FILTER_MODE_INCLUDE == fmode)
                    {
                        if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
                        {
                            LOGIC_PORTMASK_SET_PORT(addInMbr, port);
                        }

                        LOGIC_PORTMASK_SET_PORT(groupHead->staticMbr, port);
                        LOGIC_PORTMASK_SET_PORT(groupHead->mbr, port);
                        LOGIC_PORTMASK_SET_PORT(groupHead->staticInMbr, port);
                    }
                    else
                    {
                        LOGIC_PORTMASK_SET_PORT(groupHead->staticMbr, port);

                        if(p_igmp_stats->oper_version <= IGMP_VERSION_V2)
                            LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);
                        else
                            LOGIC_PORTMASK_SET_PORT(groupHead->mbr, port);

                        LOGIC_PORTMASK_SET_PORT(addExMbr, port);
                    }
                }
                else
                {
                    LOGIC_PORTMASK_CLEAR_PORT(groupHead->staticMbr, port);

                    if (FILTER_MODE_INCLUDE == fmode)
                    {
                        LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);
                        LOGIC_PORTMASK_CLEAR_PORT(groupHead->staticInMbr, port);
                        LOGIC_PORTMASK_SET_PORT(delInMbr,port);
                    }
                    else
                    {
                        if(p_igmp_stats->oper_version > IGMP_VERSION_V2)
                        {
                            LOGIC_PORTMASK_CLEAR_PORT(groupHead->mbr, port);
                        }
                        LOGIC_PORTMASK_SET_PORT(delExMbr, port);
                    }
                }
            }

            if (FALSE == isAdd)
            {
                if (IS_LOGIC_PORTMASK_CLEAR(groupHead->mbr))
                {
                    //set port back, to delete
                    LOGIC_PORTMASK_OR(groupHead->mbr, groupHead->mbr, delInMbr);

                    mcast_basic_static_group_clear(groupHead);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                }
            }

            mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);

            if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
                (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
            {
                mcast_aggregate_db_get(pGroup, &pAggrEntry);
                if (pAggrEntry == NULL)
                {
                    if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                        SYS_DBG(LOG_DBG_IGMP,"[IGMP]%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDR_PRINT"\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pGroup->dip));
                    else
                        SYS_DBG(LOG_DBG_IGMP,"[MLD]%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDRV6_PRINT"\n", __FUNCTION__, __LINE__, IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));

                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_FAILED;

                }
                else
                {
                    if (TRUE == isAdd)
                    {
                        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, addInMbr)
                        {
                            pAggrEntry->p_port_ref_cnt[port]++;
                        }
                    }
                    else
                    {
                        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, delInMbr)
                        {
                            pAggrEntry->p_port_ref_cnt[port]--;
                        }
                    }
                }

                l2McstEntry.vid = pGroup->vid;
                osal_memcpy(l2McstEntry.mac, groupHead->mac, MAC_ADDR_LEN);
                if (SYS_ERR_OK == (ret = rsd_l2McastEntry_get(&l2McstEntry)))
                {
                    /* ASIC's mbr may contain router port */
                    LOGIC_PORTMASK_OR(l2McstEntry.portmask, addInMbr, l2McstEntry.portmask);

                    //if(TRUE == isAdd)
                     //   LOGIC_PORTMASK_ANDNOT(l2McstEntry.portmask, l2McstEntry.portmask, addExMbr);

                    if (FALSE == isAdd)
                    {
                        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, delInMbr)
                        {
                            if (0 == pAggrEntry->p_port_ref_cnt[port])
                            {
                                if (!IS_LOGIC_PORTMASK_PORTSET(rtPmsk, port))
                                {
                                    LOGIC_PORTMASK_CLEAR_PORT(l2McstEntry.portmask, port);
                                }
                            }
                        }
                    }

                    if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2McstEntry)))
                    {
                        if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                        else
                            SYS_DBG(LOG_DBG_MLD, "Failed writing to ASIC!  ret:%d\n", ret);
                    }
                }
            }
            else
            {
                //not support ipv6 dip+sip
                if (MULTICAST_TYPE_IPV4 != pGroup->ipType)
                {
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                }

                // v3-full, update static entry port member.
                if( p_igmp_stats->oper_version > IGMP_VERSION_V2)
                {
                    pEntry = groupHead;
                    while (pEntry)
                    {
                        //update the static filter group
                        if (IGMP_GROUP_ENTRY_STATIC == pEntry->form)
                        {
                            if (pEntry != groupHead)
                            {
                                osal_memcpy(&pEntry->mbr, &groupHead->mbr,sizeof(sys_logic_portmask_t));
                                osal_memcpy(&pEntry->staticMbr, &groupHead->staticMbr,sizeof(sys_logic_portmask_t));
                                osal_memcpy(&pEntry->staticInMbr, &groupHead->staticInMbr,sizeof(sys_logic_portmask_t));
                            }

                            if (TRUE == isAdd)
                            {
                                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, addExMbr)
                                {
                                    pEntry->v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NOT;
                                }
                            }
                            else
                            {
                                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, delExMbr)
                                {
                                    if (IGMPV3_FWD_NOT ==  pEntry->v3.p_mbr_fwd_flag[port])
                                       pEntry->v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NORMAL;
                                }
                            }

                            if ( L2TABLE_WROTE == pEntry->lookupTableFlag )
                            {
                                ipMcstEntry.vid = pGroup->vid;
                                ipMcstEntry.dip = pGroup->dip;
                                ipMcstEntry.sip = pEntry->sip;

                                mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);
                                mcast_igmpv3_fwdPmsk_get(&ipMcstEntry.portmask, pEntry);
                                LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, rtPmsk);
                                if (SYS_ERR_OK != rsd_ipMcastEntry_set(&ipMcstEntry))
                                    SYS_DBG(LOG_DBG_IGMP, "%s()@%d: Failed rsd_ipMcastEntry_set\n", __FUNCTION__, __LINE__);

                           }
                        }
                       pEntry = pEntry->next_subgroup;
                    }
                }
                else
                {
                    if (groupHead->sip == 0)     /* This group is created by only Join */
                    {
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_OK;      /* Need not write to ASIC */
                    }
                    else                        /* This group is created by Data (maybe and Join) */
                    {
                        pEntry = groupHead;
                        while (pEntry)
                        {
                            if (pEntry != groupHead)
                            {
                                if (IS_LOGIC_PORTMASK_EQUAL(pEntry->mbr, groupHead->mbr))
                                {
                                    pEntry = pEntry->next_subgroup;
                                    continue;
                                }
                                osal_memcpy(&pEntry->mbr, &groupHead->mbr, sizeof(sys_logic_portmask_t));
                            }
                            ipMcstEntry.vid = pGroup->vid;
                            ipMcstEntry.dip = pGroup->dip;
                            ipMcstEntry.sip = pEntry->sip;
                            mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);
                            LOGIC_PORTMASK_OR(ipMcstEntry.portmask, pEntry->mbr, rtPmsk);
                            ret = rsd_ipMcastEntry_set(&ipMcstEntry);

                            SYS_DBG(LOG_DBG_IGMP, "%s()@%d rsd_ipMcastEntry_set failed ret =%d\n", __FUNCTION__, __LINE__, ret);

                            pEntry = pEntry->next_subgroup;
                        }
                    }
                }
            }

            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }
     }
     else
     {
        if (FALSE == isAdd)
        {
            if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
            {
                SYS_DBG(LOG_DBG_IGMP,
                    "The VLAN %d group ip("IPADDR_PRINT")  is not exist !\n", pGroup->vid, IPADDR_PRINT_ARG(pGroup->dip));
            }
            else
            {
                SYS_DBG(LOG_DBG_MLD,
                    "The VLAN %d group ip("IPADDRV6_PRINT")  is not exist !\n", pGroup->vid, IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));
            }
            return SYS_ERR_FAILED;
        }
     }

    IGMP_DB_SEM_LOCK();
    //add new group entry
    pEntry = mcast_group_freeEntry_get(pGroup->ipType);
    if (pEntry != NULL)
    {
        //don't consider the aggregate
        pEntry->form = IGMP_GROUP_ENTRY_STATIC;
        pEntry->ipType = pGroup->ipType;
        pEntry->dip = pGroup->dip;
        pEntry->sip = 0;
        pEntry->vid = pGroup->vid;
        pEntry->sortKey =  mcast_group_sortKey_ret(pGroup);

        if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
        {
            pEntry->mac[0] = 0x01;
            pEntry->mac[1] = 0x00;
            pEntry->mac[2] = 0x5e;
            pEntry->mac[3] = (pGroup->dip >> 16) & 0x7f;
            pEntry->mac[4] = (pGroup->dip >> 8) & 0xff;
            pEntry->mac[5] = pGroup->dip & 0xff;
        }
        else
        {
            pEntry->mac[0] = 0x33;
            pEntry->mac[1] = 0x33;
            pEntry->mac[2] = (pGroup->dip >> 24) & 0xff;
            pEntry->mac[3] = (pGroup->dip >> 16) & 0xff;
            pEntry->mac[4] = (pGroup->dip >> 8) & 0xff;
            pEntry->mac[5] = pGroup->dip & 0xff;

            osal_memcpy(pEntry->dipv6.addr, pGroup->dipv6.addr,IPV6_ADDR_LEN);
        }

        //all version must copy input mbr to entry mbr.
        osal_memcpy(&pEntry->staticMbr, &pGroup->mbr, sizeof(sys_logic_portmask_t));
        osal_memcpy(&pEntry->mbr, &pGroup->mbr, sizeof(sys_logic_portmask_t));

        if (FILTER_MODE_INCLUDE == fmode)
        {
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port,pGroup->mbr)
            {
                LOGIC_PORTMASK_SET_PORT(pEntry->staticInMbr, port);
            }
            //osal_memcpy(&pEntry->staticInMbr,&pGroup->mbr,sizeof(sys_logic_portmask_t));
        }
        else
        {
            if (p_igmp_stats->oper_version > IGMP_VERSION_V2)
            {
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup->mbr)
                {
                    pEntry->v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NOT;
                }
            }
        }

        // if the mbr is NULL, do not create group.
        if (IS_LOGIC_PORTMASK_CLEAR(pEntry->mbr))
        {
            mcast_group_entry_release(pEntry);
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }


        if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            /* Handle DIP -> MAC aggregation */
            mcast_aggregate_db_get(pGroup, &pAggrEntry);
            if (pAggrEntry == NULL)
            {
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
                {
                    firstPort = port;
                    break;
                }
                if (SYS_ERR_OK != (ret = mcast_aggregate_db_add(pGroup, firstPort)))
                {
                    SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                    mcast_group_entry_release(pEntry);
                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                    //SYS_PRINTF("%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                }

                mcast_aggregate_db_get(pGroup, &pAggrEntry);
                if (pAggrEntry != NULL)
                {
                    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
                    {
                        if (firstPort != port)
                        {
                            pAggrEntry->p_port_ref_cnt[port]++;
                        }
                    }
                }
                else
                {
                     SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                        "%s():%d  Failed to add a aggregate record  ret:%d\n", __FUNCTION__, __LINE__, ret);
                }
            }
            else
            {
                pAggrEntry->group_ref_cnt++;
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
                {
                    pAggrEntry->p_port_ref_cnt[port]++;
                }
            }

            //add for igmp/mld disabled , not write to asic
            if (MULTICAST_TYPE_IPV4 == pGroup->ipType && DISABLED == p_igmp_stats->igmpsnp_enable)
            {
                isSetAsic = FALSE;
            }

            if (MULTICAST_TYPE_IPV6 == pGroup->ipType && DISABLED == p_mld_stats->mldsnp_enable)
            {
                isSetAsic = FALSE;
            }

            mcast_vlan_db_get(pGroup->vid , pGroup->ipType, &pMcastVlan);
            if (NULL == pMcastVlan)
            {
                mcast_group_entry_release(pEntry);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_OK;
            }
            else
            {
                if (DISABLED == pMcastVlan->enable)
                {
                    isSetAsic = FALSE;
                }
            }

            if (!isSetAsic)
            {
                mcast_group_num_cal(pGroup->ipType,TRUE);
                mcast_group_sortedArray_ins(sortedIdx, pEntry);
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_OK;
            }
            //LOGIC_PORTMASK_COPY(l2McstEntry.portmask, pEntry->mbr);
            /* Also add router portmask */
           // mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);
           // LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);

            l2McstEntry.vid = pGroup->vid;
            osal_memcpy(l2McstEntry.mac, pEntry->mac, MAC_ADDR_LEN);

            if (SYS_ERR_OK == rsd_l2McastEntry_get(&l2McstEntry))
            {
                LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pEntry->mbr);

                if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2McstEntry)))
                {
                        SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d  vid:%d   mac:"MAC_PRINT"\n",
                            ret, pGroup->vid, MAC_PRINT_ARG(l2McstEntry.mac));
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_OK;
                }
            }
            else
            {
                mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);
                LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);
                LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pEntry->mbr);
                if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_add(pGroup->ipType, &l2McstEntry)))
                {
                    /*The LUT entry may already be created by another group that map to the same aggregation. ex: 224.2.2.1 and 225.2.2.1 */
                    if (ret == SYS_ERR_L2_MAC_IS_EXIST)
                    {
                        mcast_group_entry_release(pEntry);
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_FAILED;
                    }
                }
                else
                {
                    /* Only when rsd_l2McastEntry_add() success should we need to increase igmp_hw_entry_num */
                    igmp_hw_entry_num++;
                }
            }
        }

        //mcast_group_num++;
        mcast_group_num_cal(pGroup->ipType, TRUE);
        mcast_group_sortedArray_ins(sortedIdx, pEntry);

        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_OK;
    }
    else
    {
        IGMP_DB_SEM_UNLOCK();
        SYS_DBG(LOG_DBG_IGMP, "Group database is full!\n");
        return SYS_ERR_MCAST_DATABASE_FULL;
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;

}

int32 mcast_group_entry_clear(igmp_group_entry_t *pGroup)
{
    int32 ret;
    uint16 sortedIdx;
    igmp_group_entry_t *groupHead;
    igmp_group_entry_t *pEntry;


    SYS_PARAM_CHK((NULL == pGroup), SYS_ERR_NULL_POINTER);

    pEntry = mcast_group_get(pGroup);

    if (pEntry == NULL)
    {
        return SYS_ERR_FAILED;
    }

    ret = mcast_hw_mcst_entry_del(pEntry);
    if (ret == SYS_ERR_OK)
    {
        igmp_hw_entry_num--;
    }

     mcast_group_del(pGroup);

    if (p_igmp_stats->oper_version == IGMP_VERSION_V3_FULL)
    {
        mcast_group_sortedArray_search(mcast_group_sortKey_ret(pEntry), &sortedIdx, &groupHead);
        if(groupHead == NULL)
        {
            mcast_igmp_group_head_del(pGroup->dip, pGroup->vid);
        }
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_basic_static_group_clear
* Description:
*      Clear IGMP static group entry for v3 basic.
* Input:
*      vid - vlan id
*      groupAddr - group Ip
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      None
*/
int32 mcast_basic_static_group_clear(igmp_group_entry_t *pGroup)
{
    uint16 sortedIdx;
    igmp_group_entry_t  *groupHead = NULL, *pEntry = NULL, *pTmp = NULL;

    SYS_PARAM_CHK((NULL == pGroup), SYS_ERR_NULL_POINTER);

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);

    if (groupHead == NULL)
        return SYS_ERR_OK;

    if (IGMP_DIP_SIP_MODE == p_igmp_stats->lookup_mode )
    {
        pEntry = groupHead;
        while(pEntry)
        {
            pTmp = pEntry->next_subgroup;
            mcast_group_entry_clear(pEntry);
            pEntry = pTmp;
        }
    }
    else
    {
        if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            mcast_group_entry_clear(groupHead);
        }
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_static_group_clear
* Description:
*      Clear IGMP static group entry.
* Input:
*     pGroup -  group entry
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      None
*/
int32 mcast_static_group_clear(igmp_group_entry_t  *pGroup)
{
    IGMP_DB_SEM_LOCK();

    SYS_PARAM_CHK((NULL == pGroup), SYS_ERR_NULL_POINTER);

    if(pGroup->sip  == 0)
    {
        mcast_basic_static_group_clear(pGroup);
    }
    else
    {
        mcast_group_entry_clear(pGroup);
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_static_group_hwRestore
* Description:
*      restore static group to HW .
* Input:
*      vlanMsk - vlan mask
*      ipType - ip type.
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      None
*/
int32 mcast_static_group_hwRestore(sys_vlanmask_t vlanMsk, multicast_ipType_t ipType)
{
    int i;
    igmp_group_entry_t *pGroup = NULL;
    sys_logic_portmask_t  rtPmsk;
    sys_l2McastEntry_t l2McstEntry;
    sys_ipMcastEntry_t ipMcstEntry;
    sys_logic_port_t port;
    uint32 freeCount = 1;
    int32 ret;
    mcast_vlan_entry_t *pMcastVlan = NULL;

    SYS_PARAM_CHK((ipType > MULTICAST_TYPE_END || ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);

    IGMP_DB_SEM_LOCK();

    for (i = 0; i < mcast_group_num; i++)
    {
        pGroup = mcast_group_get_with_index(i);

        if ( NULL == pGroup)
            continue;

        if (ipType != pGroup->ipType)
            continue;

        if (!VLANMASK_IS_VIDSET(vlanMsk, pGroup->vid))
            continue;

        if (IGMP_GROUP_ENTRY_STATIC != pGroup->form)
            continue;

        mcast_vlan_db_get(pGroup->vid, pGroup->ipType, &pMcastVlan);
        if (NULL == pMcastVlan)
        {
            continue;
        }
        else
        {
            if (DISABLED == pMcastVlan->enable)
                continue;
        }

        mcast_routerPort_get(pGroup->vid, pGroup->ipType, &rtPmsk);

        if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
            (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
        {
            osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));
            l2McstEntry.vid = pGroup->vid;
            osal_memcpy(l2McstEntry.mac, pGroup->mac,MAC_ADDR_LEN);

            /* Also add router portmask */
            LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, rtPmsk);
            LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pGroup->mbr);

            if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_add(ipType, &l2McstEntry)))
            {
                /*The LUT entry may already be created by another group that map to the same aggregation. ex: 224.2.2.1 and 225.2.2.1 */
                if (SYS_ERR_L2_MAC_IS_EXIST == ret)
                {
                    rsd_l2McastEntry_get(&l2McstEntry);
                    LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pGroup->mbr);
                    if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(ipType, &l2McstEntry)))
                    {
                        SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d  vid:%d   mac:"MAC_PRINT"\n",
                            ret, pGroup->vid, MAC_PRINT_ARG(l2McstEntry.mac));
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_OK;
                    }
                }
                else if (SYS_ERR_L2_MAC_FULL == ret)
                {
					#if 0
					SYS_LOG(LOG_SEV_WARNING, LOG_CAT_IGMP, LOG_IGMP_GROUP_L2_FULL,
                        pGroup->vid, pGroup->mac[0], pGroup->mac[1],pGroup->mac[2],
                        pGroup->mac[3], pGroup->mac[4],pGroup->mac[5]);
					#endif

                    IGMP_DB_SEM_UNLOCK();
                    return SYS_ERR_OK;
                }
            }
            else
            {
                /* Only when rsd_l2McastEntry_add() success should we need to increase igmp_hw_entry_num */
                igmp_hw_entry_num++;
            }
        }
        else
        {
            if (0 == pGroup->sip)
                continue;

            osal_memset(&ipMcstEntry, 0 ,sizeof(sys_ipMcastEntry_t));
            ipMcstEntry.vid = pGroup->vid;
            ipMcstEntry.dip = pGroup->dip;
            ipMcstEntry.sip = pGroup->sip;

            if (p_igmp_stats->oper_version < IGMP_VERSION_V3_FULL)
                LOGIC_PORTMASK_COPY(ipMcstEntry.portmask, pGroup->mbr);

            if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_add(&ipMcstEntry)))
            {
                /* Check if multicast portmask table if full. If full, we don't trap other unknown mcast data to CPU until the entry is released */
                //rsd_l2_mcastFwdIndexFreeCount_get(&freeCount);//set 1, allways is free;
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
                pGroup->lookupTableFlag = L2TABLE_WROTE;

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
    }

    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

int32 mcast_profile_get(multicast_ipType_t ipType, sys_logic_port_t port, sys_igmp_profile_t **pProfile)
{
    SYS_PARAM_CHK((ipType > MULTICAST_TYPE_END || ipType < MULTICAST_TYPE_IPV4), SYS_ERR_MCAST_IPTYPE);
    SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(port), SYS_ERR_PORT_ID);

    *pProfile = &pp_profile_db[ipType][port-1];
    return SYS_ERR_OK;
}

int32 mcast_igmp_group_vid_by_group_search(
    multicast_ipType_t ipType, mcast_groupAddr_t grpAddr, uint16 *pGrpVid, uint32 *pAclEntryId)
{
	sys_igmp_profile_t      *pProfile;
	igmp_acl_entry_list_t   *pAclEntryList = NULL;
    sys_logic_port_t        port;
	int32                   ret1 = 0, ret2 = 0;

	SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    FOR_EACH_LOGIC_PORT(port)
 	{
		if (SYS_ERR_OK != mcast_profile_get(ipType, port, &pProfile))
		{
			continue;
		}
		else
		{
			//get profile success and check acl entry
			pAclEntryList = pProfile->aclEntryList[IGMP_GROUP_ENTRY_DYNAMIC];
			while (pAclEntryList)
			{
				// check grpAddr in acl entry
				if (MULTICAST_TYPE_IPV4 == ipType)
				{
					if ((grpAddr.ipv4 >= pAclEntryList->aclEntry->dipStart.ipv4) &&
						(grpAddr.ipv4 <= pAclEntryList->aclEntry->dipEnd.ipv4))
					{
						*pGrpVid = pAclEntryList->aclEntry->aniVid;
						*pAclEntryId = pAclEntryList->aclEntry->id;
						return SYS_ERR_OK;
					}
				}
				else
				{
					ret1 = osal_memcmp(grpAddr.ipv6.addr, pAclEntryList->aclEntry->dipStart.ipv6, IPV6_ADDR_LEN);
					ret2 = osal_memcmp(grpAddr.ipv6.addr, pAclEntryList->aclEntry->dipEnd.ipv6, IPV6_ADDR_LEN);

                    if (ret2 <= 0 && ret1 >= 0)
					{
						*pGrpVid  = pAclEntryList->aclEntry->aniVid;
						*pAclEntryId = pAclEntryList->aclEntry->id;
						return SYS_ERR_OK;
					}
				}
				pAclEntryList = pAclEntryList->next;
			}
            //TBD ipv4
            // ipv6 mop without acl
            if (MULTICAST_TYPE_IPV6 == ipType || MULTICAST_TYPE_IPV4 == ipType)
            {
                 /* check exist mop profile with forward unauthorized ctrl packet but no acl rule. */
                if(SYS_ERR_OK == mcast_mop_profile_per_port_find(port, pAclEntryId))
                {
                    *pGrpVid = 0;
                    return SYS_ERR_OK;
                }
            }
		}
	}
	return SYS_ERR_FAILED;
}

static int32 _mcast_group_timer_dmacvid(void)
{
    int32                   ret;
    uint16                  i, time;
//    uint8  flag = FALSE;
    sys_logic_port_t        port;
    igmp_group_entry_t      *pGroup;
    sys_logic_portmask_t    delPmsk;
    int32                   tmpNum;
    igmp_aggregate_entry_t  *pAggrEntry;
    igmp_querier_entry_t    *pQuerEntry;
    sys_l2McastEntry_t      l2McstEntry;
    igmp_router_entry_t     *pRouterEntry;

    mcast_vlan_entry_t *pMcastVlan = NULL;
    uint8 flag;

    mcast_groupAdd_stop = ENABLED;
    osal_time_usleep(200);
    osal_memset(&l2McstEntry, 0, sizeof(sys_l2McastEntry_t));
    /* Check memberPort timer */
    IGMP_DB_SEM_LOCK();

    for (i = 0; i < group_sortedAry_entry_num; i++)
    {
        if (ENABLED == mcast_timer_stop)
        {
            IGMP_DB_SEM_UNLOCK();
            mcast_groupAdd_stop = DISABLED;
            return SYS_ERR_OK;
        }

        pGroup = pp_group_sorted_array[i];
        if (NULL == pGroup)
            continue;

        flag = FALSE;

        //the static group have no dynamic port. so if static group continue;
        if (IGMP_GROUP_ENTRY_STATIC == pGroup->form ||
			IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED == pGroup->form)
        {
            continue;
        }
        if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
        {
            time = pGroup->groupTimer;
            if (time == 0)
            {
                SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                    "%s()@%d:  Warring! an existing mbr whose life is zero\n", __FUNCTION__, __LINE__);
                //fix error group time = 0;
                #if 0 //debug;
                SYS_PRINTF("%s():%d  Warring! an existing mbr whose life is zero\n", __FUNCTION__, __LINE__);
                ret = mcast_hw_mcst_entry_del(pGroup);
                if (ret == SYS_ERR_OK)
                        igmp_hw_entry_num--;

                tmpNum = group_sortedAry_entry_num;
                if (SYS_ERR_OK != mcast_group_del(pGroup))
                    SYS_PRINTF("group delete failure !\n");
                if (tmpNum != group_sortedAry_entry_num)
                    i--;
                #endif
            }
            else
            {
                if (time >= IGMP_TEMP_PERIOD)
                {
                    pGroup->groupTimer -= PASS_SECONDS;
                }
                else if (time == 2)
                {
                    /* Add CPU port to L2 table to see if any this kind of mcst data is flowing.
                    If stream flows, we remove CPU port in funciton igmp_group_mcstData_add_wrapper().
                    If none, we will delete this L2 entry next second */

                    l2McstEntry.vid = pGroup->vid;
                    l2McstEntry.care_vid = pGroup->care_vid;
                    osal_memcpy(l2McstEntry.mac, pGroup->mac, MAC_ADDR_LEN);
                    rsd_l2McastEntry_get(&l2McstEntry);
                    l2McstEntry.crossVlan = ENABLED;
                    rsd_l2McastEntryIncCPU_set(&l2McstEntry);
                    pGroup->groupTimer -= PASS_SECONDS;
                }
                else
                {
                    SYS_DBG(LOG_DBG_IGMP, "Group Timeout of IP = "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pGroup->dip));

                    ret = mcast_hw_mcst_entry_del(pGroup);
                    if (ret == SYS_ERR_OK)
                    {
                        igmp_hw_entry_num--;
                    }
                    tmpNum = group_sortedAry_entry_num;
                    mcast_group_del(pGroup);
                    if (tmpNum != group_sortedAry_entry_num)
                        i--;
                }
            }
            continue;
        }
		if (MODE_GPON != igmpCtrl.ponMode)
		{
	        mcast_vlan_db_get(pGroup->vid, pGroup->ipType, &pMcastVlan);
	        if (NULL == pMcastVlan)
	        {
	        	 SYS_DBG(LOG_DBG_IGMP,"get group vid %d failed\n",pGroup->vid);
	            continue;
	        }

	        /* Querier */
	        mcast_querier_db_get(pGroup->vid, pGroup->ipType, &pQuerEntry);
	        if (pQuerEntry == NULL)
	        {
	           // SYS_PRINTF("[IGMP]%s():%d VLAN-%d doesn't have query DB entry!\n", __FUNCTION__, __LINE__, pGroup->vid);
	            SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
	            "%s():%d VLAN-%d doesn't have query DB entry!\n", __FUNCTION__, __LINE__, pGroup->vid);
	            IGMP_DB_SEM_UNLOCK();
	            mcast_groupAdd_stop = DISABLED;
	            return SYS_ERR_FAILED;
	        }
		}

        FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup->mbr)
        {
            time = pGroup->p_mbr_timer[port];
            if (time == 0)
            {
                 SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                    "%s():%d  Warring! an existing mbr whose life is zero\n", __FUNCTION__, __LINE__);
            }
            else
            {
                if (time > PASS_SECONDS)
                {
                    if (SYS_ERR_OK != mcast_host_tracking_update_member_time_per_port(pGroup, (uint16)port))
					{
						SYS_DBG(LOG_DBG_IGMP, " group timer is timeout need to del host entry in this port \n");
					}
                    pGroup->p_mbr_timer[port] -= PASS_SECONDS;
                }
                else
                {
                    if (MODE_GPON != igmpCtrl.ponMode && pQuerEntry->status == IGMP_QUERIER &&
                    	(pGroup->p_gsQueryCnt[port] >= 1 &&
                    	pGroup->p_gsQueryCnt[port] < pMcastVlan->operLastMmbrQueryCnt))
                    {

                        pGroup->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;
                        pGroup->p_gsQueryCnt[port]++;
                        //mcast_igmp_send_grp_specific_query(pGroup->vid, pGroup->dip, port);
                        mcast_send_gs_query(pGroup, port);
                    }
                    else
                    {
                        SYS_DBG(LOG_DBG_IGMP,
                            "Group membership Timeout on Port=%d for IP = "IPADDR_PRINT"\n", port, IPADDR_PRINT_ARG(pGroup->dip));
                        pGroup->p_mbr_timer[port] = 0;

                        LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
                        LOGIC_PORTMASK_SET_PORT(delPmsk, port);

                        if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                        {
                            if (IS_LOGIC_PORTMASK_PORTSET(pGroup->v3.isGssQueryFlag, port))
                                LOGIC_PORTMASK_CLEAR_PORT(pGroup->v3.isGssQueryFlag, port);
                        }

                        if (MODE_GPON != igmpCtrl.ponMode)
                        {
                            LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                            flag = TRUE;
                            mcast_hw_mcst_mbr_remove(pGroup, &delPmsk);

                        }
                        else
                        {
                            if (!IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                            {
                                if (SYS_ERR_OK == mcast_hw_mcst_mbr_remove(pGroup, &delPmsk))
                                {
                                    p_igmp_stats->p_port_info[port].curBw -= pGroup->imputedGrpBw;
                                    igmp_hw_entry_num--;
                                }
                                LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                                if (SYS_ERR_OK == mcast_host_tracking_del_per_port(pGroup, (uint16)port))
                                {
                                    SYS_DBG(LOG_DBG_IGMP, " group timer is timeout need to del host entry in this port \n");
                                }

                                /*if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                {
                                    SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(pGroup->vid, pGroup->ipType,
                                        pGroup->dip, pGroup->dipv6.addr, &tmpGroup)), SYS_ERR_FAILED);
                                    tmpGroup.sip = pGroup->sip;
                                }*/

                                tmpNum = group_sortedAry_entry_num;
                                if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                                {
                                    mcast_group_del(pGroup);
                                }

                                /*if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                {
                                    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroup), &sortedIdx, &groupHead);
                                    if (groupHead == NULL)
                                    {
                                        mcast_igmp_group_head_del(tmpGroup.dip, tmpGroup.vid);
                                    }
                                }*/

                                if (tmpNum != group_sortedAry_entry_num)
                                    i--;
                            }
                        }
                    }
                }
            }
        }

        if (flag)
        {
            if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
            {
                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                {
                    mcast_aggregate_db_get(pGroup, &pAggrEntry);
                    if (pAggrEntry == NULL)
                    {
                        SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP: LOG_DBG_MLD,
                           "[Multicast]%s():%d An existing group which has no aggregate record!  pGroup->dip:"IPADDR_PRINT"\n", __FUNCTION__, __LINE__, IPADDR_PRINT_ARG(pGroup->dip));
                         IGMP_DB_SEM_UNLOCK();
                         mcast_groupAdd_stop = DISABLED;
                        return SYS_ERR_OK;
                    }

                    if (pAggrEntry->group_ref_cnt == 1)
                    {
                        /* pAggrEntry->group_ref_cnt--; */      /* The last group will minus when pGroup->groupTimer == 0 */

                        pGroup->groupTimer = IGMP_TEMP_PERIOD;

                        l2McstEntry.vid = pGroup->vid;
                        l2McstEntry.care_vid = pGroup->care_vid;
                        osal_memcpy(l2McstEntry.mac, pGroup->mac, MAC_ADDR_LEN);
                        mcast_router_db_get(pGroup->vid, pGroup->ipType, &pRouterEntry);
                        if (pRouterEntry)
                            LOGIC_PORTMASK_OR(l2McstEntry.portmask, l2McstEntry.portmask, pRouterEntry->router_pmsk);
                        else
                            SYS_DBG(LOG_DBG_IGMP, "Warring! This vlan has no router port!\n");

                        if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2McstEntry)))
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Failed writing to ASIC!  ret:%d\n", ret);
                        }
                    }
                    else
                    {
                        pAggrEntry->group_ref_cnt--;
                        tmpNum = group_sortedAry_entry_num;
                        mcast_group_del(pGroup);
                        if (tmpNum != group_sortedAry_entry_num)
                            i--;        /* array entry is removed */
                    }
                }
                else
                {
                    /* 2. Delete this group from DB */
                    ret = mcast_hw_mcst_entry_del(pGroup);
                    if (ret == SYS_ERR_OK)
                    {
                        igmp_hw_entry_num--;
                    }
                    tmpNum =  group_sortedAry_entry_num;
                    mcast_group_del(pGroup);
                    if (tmpNum != group_sortedAry_entry_num)
                        i--;
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();
    mcast_groupAdd_stop = DISABLED;
    return SYS_ERR_OK;
}

void mcast_tick_thread(void *pInput)
{
    osal_usecs_t  startTime, endTime;
    int32 interval;
	prctl(PR_SET_NAME, (unsigned long)((char *)pInput));
    /* forever loop */
    for (;;)
    {
        startTime = 0;
        endTime = 0;
        interval = 0;

        osal_time_usecs_get(&startTime);

        mcast_igmp_querier_timer();

        //mcast_routerPort_timer();CTC ROUTE PORT IS FIXED PON
		if (igmp_lookup_mode_func[MULTICAST_TYPE_IPV4 * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_group_timer)
	    	igmp_lookup_mode_func[MULTICAST_TYPE_IPV4 * IGMP_LOOKUP_MODE_END + p_igmp_stats->lookup_mode].igmp_group_timer(); //maintain grp timer

        if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
            mcast_igmp_group_head_timer(); // maintain src timer

		mcast_igmpmcvlancheck();
        osal_time_usecs_get(&endTime);
        interval = endTime - startTime;

        interval = PASS_SECONDS * 1000 * 1000 - interval;

        if (interval > 0)
            osal_time_usleep(interval);  /* PASS_SECONDS Sec */

        if (igmp_thread_status)
        {
            break;
        }
    }

    igmp_thread_status = -1;

    osal_thread_exit(0);
}

/* Function Name:
 *      mcast_vlan_get
 * Description:
 *      Get mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      pEntry - Multicast VLAN information
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
int32 mcast_vlan_get(mcast_vlan_entry_t *pEntry)
{
    mcast_vlan_entry_t  *tmp = NULL;

    SYS_PARAM_CHK(IS_VID_INVALID(pEntry->vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((pEntry->ipType < MULTICAST_TYPE_IPV4 ||pEntry->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    mcast_vlan_db_get(pEntry->vid, pEntry->ipType, &tmp);

    if(tmp)
    {
        osal_memcpy(pEntry,tmp,sizeof(mcast_vlan_entry_t));
    }
    else
    {
        osal_memset(pEntry,0,sizeof(mcast_vlan_entry_t));
    }


    return SYS_ERR_OK;

}

/* Function Name:
 *      mcast_vlan_set
 * Description:
 *      Configures mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
int32 mcast_vlan_set(mcast_vlan_entry_t *pEntry)
{
    mcast_vlan_entry_t tmp;
    sys_vlanmask_t vlanMask;
    igmp_group_entry_t *pGroup = NULL;
    sys_l2McastEntry_t  l2Mcst_entry;
    sys_logic_portmask_t  delStaticPmsk, aggratePmk;
    igmp_router_entry_t router;
    int32 i, ret;

    SYS_PARAM_CHK(IS_VID_INVALID(pEntry->vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((pEntry->ipType < MULTICAST_TYPE_IPV4 ||pEntry->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    osal_memset(&tmp, 0, sizeof(mcast_vlan_entry_t));
    tmp.vid = pEntry->vid;
    tmp.ipType = pEntry->ipType;
    mcast_vlan_get(&tmp);

    VLANMASK_CLEAR_ALL(vlanMask);
    VLANMASK_SET_VID(vlanMask, pEntry->vid);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_vlan_db_set(pEntry)), SYS_ERR_FAILED);

    if (DISABLED == pEntry->enable)
    {
        if ((0 != tmp.vid) && (tmp.enable == ENABLED))
        {
            IGMP_DB_SEM_LOCK();
            mcast_static_group_save(vlanMask, pEntry->ipType);
            _mcast_hwEntryByVid_del(&vlanMask, pEntry->ipType, IGMP_GROUP_ENTRY_ALL);
            IGMP_DB_SEM_UNLOCK();
            SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_restore()), SYS_ERR_FAILED);
        }
    }
    else
    {
        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_hwRestore(vlanMask, pEntry->ipType)), SYS_ERR_FAILED);
    }

    VLANMASK_SET_VID(mcastVlanCfgMsk[pEntry->ipType], pEntry->vid);

    if (0 !=  tmp.vid)
    {
        if (IS_LOGIC_PORTMASK_EQUAL(tmp.staticMbr, pEntry->staticMbr) &&
            IS_LOGIC_PORTMASK_EQUAL(tmp.forbidMbr, pEntry->forbidMbr))
        {
            ;  //don't need update asic entry.
        }
        else
        {
            LOGIC_PORTMASK_CLEAR_ALL(delStaticPmsk);

            LOGIC_PORTMASK_ANDNOT(delStaticPmsk, tmp.staticMbr, pEntry->staticMbr);

            IGMP_DB_SEM_LOCK();
            for (i = 0; i < mcast_group_num; i++)
            {
                pGroup = mcast_group_get_with_index(i);
                if (pGroup == NULL)
                    continue;

                if (pEntry->ipType != pGroup->ipType)
                    continue;

                if (pGroup->vid != pEntry->vid)
                    continue;

                osal_memset(&l2Mcst_entry, 0, sizeof(sys_l2McastEntry_t));
                osal_memset(&router, 0, sizeof(igmp_router_entry_t));
                router.vid = pEntry->vid;
                router.ipType = pGroup->ipType;
                mcast_router_get(&router);

                if ((MULTICAST_TYPE_IPV4 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_igmp_stats->lookup_mode) ||
                    (MULTICAST_TYPE_IPV6 == pGroup->ipType && IGMP_DMAC_VID_MODE == p_mld_stats->lookup_mode))
                {
                    l2Mcst_entry.vid = pEntry->vid;
                    osal_memcpy(l2Mcst_entry.mac, pGroup->mac, MAC_ADDR_LEN);

                    if (SYS_ERR_OK != (ret = rsd_l2McastEntry_get(&l2Mcst_entry)))
                    {
                        SYS_DBG(LOG_DBG_IGMP, "Getting Mcst enrty failed! ret:%d   vid:%d\n", ret, pEntry->vid);
                    }
                    else
                    {
                        if (!IS_LOGIC_PORTMASK_CLEAR(delStaticPmsk))
                        {
                            LOGIC_PORTMASK_ANDNOT(l2Mcst_entry.portmask, l2Mcst_entry.portmask, delStaticPmsk);
                        }

                         mcast_aggregatePort_get(pGroup, &aggratePmk);
                         LOGIC_PORTMASK_OR(l2Mcst_entry.portmask, l2Mcst_entry.portmask, aggratePmk);
                         LOGIC_PORTMASK_OR(l2Mcst_entry.portmask, l2Mcst_entry.portmask, router.router_pmsk);
                         LOGIC_PORTMASK_OR(l2Mcst_entry.portmask, l2Mcst_entry.portmask, pGroup->mbr);


                        if (SYS_ERR_OK != (ret = mcast_hw_l2McastEntry_set(pGroup->ipType, &l2Mcst_entry)))
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Set L2 Mcst entry  failed!  vid:%d  \n", pEntry->vid);
                            IGMP_DB_SEM_UNLOCK();
                            return ret;
                        }
                    }
                }
            }
        }
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

/* Function Name:
 *      mcast_vlan_add
 * Description:
 *      add mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
int32 mcast_vlan_add(mcast_vlan_entry_t *pEntry)
{
    SYS_PARAM_CHK(IS_VID_INVALID(pEntry->vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((pEntry->ipType < MULTICAST_TYPE_IPV4 ||pEntry->ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

     return mcast_vlan_db_add(pEntry->vid, pEntry->ipType);
}

/* Function Name:
 *      mcast_vlan_del
 * Description:
 *      Delete mcast vlan information
 * Input:
 *      vid - VLAN ID
 *      ipType - ip type ipv4/ipv6
 * Output:
 *     None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
int32 mcast_vlan_del(sys_vid_t vid, multicast_ipType_t ipType)
{
    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 ||ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    return mcast_vlan_db_del(vid, ipType);
}

/* Function Name:
 *      mcast_vlanMskConfig_get
 * Description:
 *      Get mcast VLAN configuration entry
 * Input:
 *      None
 * Output:
 *      pBitmap - pointer to VLAN exist bitmap
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
int32 mcast_vlanMskConfig_get(multicast_ipType_t ipType, sys_vlanmask_t *pBitmap)
{
    SYS_PARAM_CHK((NULL == pBitmap), SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 ||ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    osal_memcpy(pBitmap,  &mcastVlanCfgMsk[ipType], sizeof(sys_vlanmask_t));

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_suppre_enable_set
* Description:
*      Enable IGMP suppre report function.
* Input:
*      enable - ENABLED/DISABLED MLD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_suppre_enable_set(sys_enable_t enable)
{
    p_igmp_stats->suppre_enable = enable;
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_statistics_clear
* Description:
*      Clears IGMP statistics
* Input:
*      None
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_statistics_clear(void)
{
    p_igmp_stats->total_pkt_rcvd = 0;
    p_igmp_stats->valid_pkt_rcvd = 0;
    p_igmp_stats->invalid_pkt_rcvd = 0;

    p_igmp_stats->g_query_rcvd = 0;
    p_igmp_stats->gs_query_rcvd = 0;
    p_igmp_stats->leave_rcvd = 0;
    p_igmp_stats->report_rcvd = 0;
    p_igmp_stats->other_rcvd = 0;

    p_igmp_stats->g_query_xmit = 0;
    p_igmp_stats->gs_query_xmit = 0;
    p_igmp_stats->leave_xmit = 0;
    p_igmp_stats->report_xmit = 0;

    p_igmp_stats->v3.g_queryV3_rcvd = 0;
    p_igmp_stats->v3.gs_queryV3_rcvd = 0;
    p_igmp_stats->v3.gss_queryV3_rcvd = 0;
    p_igmp_stats->v3.isIn_rcvd = 0;
    p_igmp_stats->v3.isEx_rcvd = 0;
    p_igmp_stats->v3.toIn_rcvd = 0;
    p_igmp_stats->v3.toEx_rcvd = 0;
    p_igmp_stats->v3.allow_rcvd = 0;
    p_igmp_stats->v3.block_rcvd = 0;
    p_igmp_stats->v3.g_queryV3_xmit = 0;
    p_igmp_stats->v3.gs_queryV3_xmit = 0;
    p_igmp_stats->v3.gss_queryV3_xmit = 0;

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_operVersion_get
* Description:
*      gets igmp operation version
* Input:
*      operVersion - operation version
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_operVersion_get(igmp_version_t *pVersion)
{
    SYS_PARAM_CHK((NULL == pVersion), SYS_ERR_NULL_POINTER);
    *pVersion = p_igmp_stats->oper_version;
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_operVersion_set
* Description:
*      Sets igmp operation version
* Input:
*      operVersion - operation version
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_operVersion_set(uint8 operVersion)
{
    sys_vlanmask_t vlanMsk;
    sys_vid_t vid;
    sys_igmp_querier_t qEntry;


    VLANMASK_CLEAR_ALL(vlanMsk);

    if (operVersion == p_igmp_stats->oper_version)
        return SYS_ERR_OK;

    p_igmp_stats->oper_version = operVersion;

    if(IGMP_VERSION_V3_FULL == operVersion)
    {
        if(IGMP_DIP_SIP_MODE != p_igmp_stats->lookup_mode)
        {
            mcast_igmp_lookupMode_set(IGMP_DIP_SIP_MODE);
            mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_ALL);
        }
        else
            mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_DYNAMIC);
    }
    else
    {
        if(DFLT_IGMP_LOOKUP_MODE != p_igmp_stats->lookup_mode)
        {
            mcast_igmp_lookupMode_set(DFLT_IGMP_LOOKUP_MODE);
            mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_ALL);
        }
        else
            mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_DYNAMIC);
    }


    if (IGMP_VERSION_V2 == operVersion)
    {
        VLANMASK_SET_ALL(vlanMsk);//rsd_vlan_entryExist_get(vlanMsk);
        ForEachListVlan(vid, vlanMsk)
        {
            osal_memset(&qEntry,0,sizeof(sys_igmp_querier_t));
            qEntry.vid = vid;
            qEntry.ipType = MULTICAST_TYPE_IPV4;
            mcast_igmp_querier_get(&qEntry);
            //if (ENABLED == qEntry.enabled && IGMP_NON_QUERIER != qEntry.status && IGMP_QUERY_V3 == qEntry.version)
            {
                qEntry.version = IGMP_QUERY_V2;
                mcast_igmp_querier_set(&qEntry);
            }
        }
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_info_get
* Description:
*      Retrieves IGMP information
* Input:
*      pInfo - IGMP information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_info_get(sys_igmp_info_t *pInfo)
{
    SYS_PARAM_CHK(NULL == pInfo, SYS_ERR_NULL_POINTER);

    pInfo->enable                   = p_igmp_stats->igmpsnp_enable;
    pInfo->suppreEnable             = p_igmp_stats->suppre_enable;
    pInfo->unknMcastActn            = gUnknown_mcast_action;

    pInfo->operVersion              = p_igmp_stats->oper_version;
    pInfo->totalRx                  = p_igmp_stats->total_pkt_rcvd;
    pInfo->validRx                  = p_igmp_stats->valid_pkt_rcvd;
    pInfo->invalidRx                = p_igmp_stats->invalid_pkt_rcvd;
    pInfo->generalQueryRx           = p_igmp_stats->g_query_rcvd;
    pInfo->gsQueryRx                = p_igmp_stats->gs_query_rcvd;
    pInfo->leaveRx                  = p_igmp_stats->leave_rcvd;
    pInfo->reportRx                 = p_igmp_stats->report_rcvd;
    pInfo->otherRx                  = p_igmp_stats->other_rcvd;
    pInfo->generalQueryTx           = p_igmp_stats->g_query_xmit;
    pInfo->gsQueryTx                = p_igmp_stats->gs_query_xmit;
    pInfo->leaveTx                  = p_igmp_stats->leave_xmit;
    pInfo->reportTx                 = p_igmp_stats->report_xmit;

    pInfo->v3.generalQueryV3Rx      = p_igmp_stats->v3.g_queryV3_rcvd;
    pInfo->v3.gsqueryV3Rx           = p_igmp_stats->v3.gs_queryV3_rcvd;
    pInfo->v3.gsSrcQueryV3Rx        = p_igmp_stats->v3.gss_queryV3_rcvd;

    pInfo->v3.isInRx                = p_igmp_stats->v3.isIn_rcvd;
    pInfo->v3.isExRx                = p_igmp_stats->v3.isEx_rcvd;
    pInfo->v3.toInRx                = p_igmp_stats->v3.toIn_rcvd;
    pInfo->v3.toExRx                = p_igmp_stats->v3.toEx_rcvd;
    pInfo->v3.allowRx               = p_igmp_stats->v3.allow_rcvd;
    pInfo->v3.blockRx               = p_igmp_stats->v3.block_rcvd;
    pInfo->v3.gQuerierV3Tx          = p_igmp_stats->v3.g_queryV3_xmit;
    pInfo->v3.gsQueryV3Tx           = p_igmp_stats->v3.gs_queryV3_xmit;
    pInfo->v3.gSsrcV3Tx             = p_igmp_stats->v3.gss_queryV3_xmit;

    return SYS_ERR_OK;
}


/* Function Name:
*      mcast_igmp_querier_set
* Description:
*      Configures/enables IGMP Querier in VLANs
* Input:
*      pQuerier - Querier information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_querier_set(sys_igmp_querier_t *pQuerier)
{
    igmp_querier_entry_t *pEntry;

    mcast_vlan_entry_t *pMcastVlan = NULL;
    multicast_ipType_t ipType = MULTICAST_TYPE_IPV4;

    SYS_PARAM_CHK(NULL == pQuerier, SYS_ERR_NULL_POINTER);

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(pQuerier->vid, ipType, &pMcastVlan);
    if (NULL == pMcastVlan)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    mcast_querier_db_get(pQuerier->vid, ipType, &pEntry);
    if (pEntry)
    {
        if (pEntry->enabled == pQuerier->enabled)
        {
            if (ENABLED == pEntry->enabled)
            {
                if (IGMP_NON_QUERIER == pEntry->status)
                {
                    if (pEntry->version != pQuerier->version)
                     {
                        IGMP_DB_SEM_UNLOCK();
                        return SYS_ERR_IGMP_QUERIER_CHANGE;
                     }
                }
                else
                    pEntry->version = pQuerier->version;
            }

            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_OK;
        }

        pEntry->enabled = pQuerier->enabled;
        pEntry->version = pQuerier->version;

        if (pEntry->enabled)
        {
            pEntry->status = IGMP_QUERIER;
            pEntry->timer = pMcastVlan->operQueryIntv/4;
        }
        else
        {
            pEntry->status = IGMP_NON_QUERIER;
            pEntry->ip = 0;
        }
    }
    else
    {
        SYS_DBG(LOG_DBG_IGMP, "Warring! querier entry for VLAN-%d doesn't exist!\n", pQuerier->vid);
    }
    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_querier_get
* Description:
*      Retrieves IGMP Querier information
* Input:
*      pQuerier - Querier information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_querier_get(sys_igmp_querier_t *pQuerier)
{
    sys_ipv4_addr_t         tmpIp, tmpMsk;
    igmp_querier_entry_t *pEntry;

    IGMP_DB_SEM_LOCK();
    mcast_querier_db_get(pQuerier->vid, pQuerier->ipType, &pEntry);
    if (pEntry)
    {
        pQuerier->enabled = pEntry->enabled;
        if (pEntry->enabled)
            pQuerier->status = pEntry->status;
        else
            pQuerier->status = 0;

        if (pEntry->ip == 0)
        {
            igmp_util_ipAddr_get("eth0", &tmpIp, &tmpMsk);
            pQuerier->ipAddr = tmpIp;
        }
        else
            pQuerier->ipAddr = pEntry->ip;

        pQuerier->version = pEntry->version;
    }
    else
    {
        SYS_DBG(LOG_DBG_IGMP, "Warring! querier entry for VLAN-%d doesn't exist!\n", pQuerier->vid);
        pQuerier->enabled = 0;
        pQuerier->status = 0;
    }
    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_lookupMode_set
* Description:
*      Set igmp lookup mode
* Input:
*      lookup mode
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED
* Note:
*      None
*/
int32 mcast_igmp_lookupMode_set(igmp_lookup_mode_t mode)
{
    if(mode != p_igmp_stats->lookup_mode)
    {
        if(ENABLED == p_igmp_stats->igmpsnp_enable)
        {
            igmp_packet_stop = ENABLED;

            /*Dont remove. Must delete group before setting new mode */
            mcast_group_del_all();

            /* this line must behind mcast_group_del_all(); and before below two lines */
            p_igmp_stats->lookup_mode = mode;

            //rsd_igmp_lookupMode_set(p_igmp_stats->lookup_mode);

            mcast_unknMcastAction_set(gUnknown_mcast_action);

            igmp_packet_stop = DISABLED;
        }
        else
        {
            /*Dont remove. Must delete group before setting new mode */
            mcast_group_del_all();
            p_igmp_stats->lookup_mode = mode;
        }
    }

    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_lookupMode_get
* Description:
*      Get igmp lookup mode
* Input:
*      None
* Output:
*      lookup mode
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_lookupMode_get(igmp_lookup_mode_t *mode)
{
    *mode = p_igmp_stats->lookup_mode;

    return SYS_ERR_OK;
}


int32 mcast_igmp_group_del_byType( mcast_group_type_t type)
{
    sys_vlanmask_t vlanMask;

    if (type < IGMP_GROUP_ENTRY_DYNAMIC || type > IGMP_GROUP_ENTRY_ALL)
        return SYS_ERR_MCAST_GROUP_TYPE;

    VLANMASK_SET_ALL(vlanMask);

    IGMP_DB_SEM_LOCK();
    _mcast_hwEntryByVid_del(&vlanMask, MULTICAST_TYPE_IPV4, type);
    IGMP_DB_SEM_UNLOCK();
    return  SYS_ERR_OK;
}

int32 mcast_igmp_snooping_stats_init (int32 boot)
{
    sys_enable_t        enable;

    if (boot)
    {
        osal_memset(&pp_profile_db[MULTICAST_TYPE_IPV4][0], 0, (igmpCtrl.sys_max_port_num - 1) * sizeof(sys_igmp_profile_t));

        SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_enable_set(DFLT_IGMP_EBL)), SYS_ERR_FAILED);


        enable = (MODE_GPON == igmpCtrl.ponMode ? DISABLED :
                (IS_LOGIC_PORTMASK_PORTSET(igmpCtrl.igmpMldTrap_enable, LOGIC_PORT_START) ? ENABLED : DISABLED));

        rsd_mcast_ctrl_and_data_to_cpu_enable_set(enable);
    }
    else
    {
        SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_statistics_clear()), SYS_ERR_FAILED);
    }
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_enable_set
* Description:
*      Enable IGMP module globally
* Input:
*      enable - ENABLED/DISABLED IGMP
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_igmp_enable_set(sys_enable_t enable)
{
    sys_vlanmask_t vlanMsk;

    SYS_PARAM_CHK(IS_ENABLE_INVALID(enable), SYS_ERR_INPUT);
    VLANMASK_SET_ALL(vlanMsk);

    if (ENABLED == enable && p_igmp_stats->igmpsnp_enable == DISABLED)
    {
        p_igmp_stats->igmpsnp_enable = ENABLED;
        SYS_DBG(LOG_DBG_IGMP, "Snooping = Enabled\n");

        if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action &&
            IGMP_DMAC_VID_MODE ==  p_igmp_stats->lookup_mode)
        {
          //  SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, DISABLED)); wanghuanyu
        }

        if (DISABLED == p_mld_stats->mldsnp_enable)
        {
            igmp_thread_status = 0;
            if ((osal_thread_t)NULL == (igmp_thread_create("IGMP Tick Thread", STACK_SIZE,
                SYS_THREAD_PRI_IGMP_TIMER, (void *)mcast_tick_thread, "IGMP_TICK")))
            {
                SYS_PRINTF("IGMP Tick Thread create failed\n");
                return SYS_ERR_FAILED;
            }

            //wanghuanyu
           // SYS_ERR_CHK(sys_notifier_observer_register(SYS_NOTIFIER_SUBJECT_PORT, _mcast_linkevent_handler, SYS_NOTIFIER_PRI_MEDIUM));
        }

        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_hwRestore(vlanMsk, MULTICAST_TYPE_IPV4)), SYS_ERR_FAILED);

      //  SYS_ERR_CHK(rsd_mcast_enable_set(enable));

    }
    else if (DISABLED == enable && p_igmp_stats->igmpsnp_enable == ENABLED)
    {
        //rsd_igmp_enable_set(DISABLED, p_igmp_stats->lookup_mode);
        p_igmp_stats->igmpsnp_enable = DISABLED;
        SYS_DBG(LOG_DBG_IGMP, "Snooping = Disabled\n");
        SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_snooping_stats_init(0)), SYS_ERR_FAILED);

        if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action &&
            IGMP_DMAC_VID_MODE ==  p_igmp_stats->lookup_mode)
        {
           // SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV4, ENABLED)); wanghuanyu
        }

        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_save(vlanMsk, MULTICAST_TYPE_IPV4)), SYS_ERR_FAILED);
        SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_group_del_byType(IGMP_GROUP_ENTRY_ALL)), SYS_ERR_FAILED);
        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_restore()), SYS_ERR_FAILED);

        SYS_ERR_CHK((SYS_ERR_OK != mcast_router_dynPortMbr_del(MULTICAST_TYPE_IPV4)), SYS_ERR_FAILED);

        if (DISABLED== p_mld_stats->mldsnp_enable)
        {
        //	wanghuanyu
           // SYS_ERR_CHK(sys_notifier_observer_unregister(SYS_NOTIFIER_SUBJECT_PORT, _mcast_linkevent_handler));
            //mcast_group_del_all();
            igmp_thread_status = 1;
            do { } while (igmp_thread_status >= 0);

           // SYS_ERR_CHK(rsd_mcast_enable_set(enable));wanghuanyu
        }

    }

    return SYS_ERR_OK;
}

static int32 _mcast_igmp_group_timer_dipsip(void)
{
    int32                   tmpNum = 0;
    uint16                  i, time, flag = FALSE, sortedIdx;
    sys_logic_port_t        port;
    igmp_group_entry_t      *pGroup, *pNextgroup, *groupHead, tmpGroup;
    sys_logic_portmask_t    delPmsk;
    uint32                  isClear;
    igmp_querier_entry_t    *pQuerEntry;
    sys_ipMcastEntry_t      ipMcstEntry;
    igmp_router_entry_t     *pRouterEntry;
    mcast_vlan_entry_t      *pMcastVlan = NULL;

    if (igmpCtrl.igmpMode == IGMP_MODE_CTC)
    {
    	return SYS_ERR_OK;
    }

    mcast_groupAdd_stop = ENABLED;

    osal_time_usleep(200);

    osal_memset(&ipMcstEntry, 0, sizeof(sys_ipMcastEntry_t));

    /* Check memberPort timer */
    IGMP_DB_SEM_LOCK();
    for (i = 0; i < group_sortedAry_entry_num; i++)
    {
        if (ENABLED == mcast_timer_stop)
        {
            IGMP_DB_SEM_UNLOCK();
            mcast_groupAdd_stop = DISABLED;
            return SYS_ERR_OK;
        }

        if (pp_group_sorted_array[i] == NULL)
        {
            if (i != (group_sortedAry_entry_num - 1))
                SYS_DBG(LOG_DBG_IGMP, "Warring! pp_group_sorted_array has hole!\n");

            break;
        }

        pGroup = pp_group_sorted_array[i];
        while (pGroup)
        {
            flag = FALSE;
            pNextgroup = pGroup->next_subgroup;

            /* If groupTimer == 0, should delete this entry from ASIC */
            LOGIC_PORTMASK_IS_CLEAR(pGroup->mbr, isClear);
            if (isClear)
            {
                if (IGMP_GROUP_ENTRY_STATIC == pGroup->form ||
					IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED == pGroup->form)
                {
                    pGroup = pNextgroup;
                    continue;
                }
                time = pGroup->groupTimer;
                if (time == 0)
                {
                    SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                            "%s():%d  Warring! an existing mbr whose life is zero\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    if (time > PASS_SECONDS + 1)
                    {
                        pGroup->groupTimer -= PASS_SECONDS;
                    }
                    else if (time == 2)
                    {
                        /* Add CPU port to L2 table to see if any this kind of mcst data is flowing.
                        If stream flows, we remove CPU port in funciton igmp_group_mcstData_add_wrapper().
                        If none, we will delete this L2 entry next second */
                        if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                        {
                            ipMcstEntry.vid = pGroup->vid;
                            ipMcstEntry.dip = pGroup->dip;
                            ipMcstEntry.sip = pGroup->sip;
						    ipMcstEntry.fmode = pGroup->fmode;
						    ipMcstEntry.isGrpRefB = mcast_group_ref_get(pGroup);
                            ipMcstEntry.care_vid = pGroup->care_vid;
                            if (SYS_ERR_OK != (rsd_ipMcastEntry_get(&ipMcstEntry)))
                                SYS_DBG(LOG_DBG_IGMP, "rsd_ipMcastEntry_get failed\n");

                        }
                        //rsd_ipMcastEntryIncCPU_set(&ipMcstEntry);
                        pGroup->groupTimer -= PASS_SECONDS;
                    }
                    else
                    {
                        if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Group Timeout of IP = "IPADDR_PRINT"\n", IPADDR_PRINT_ARG(pGroup->dip));

                            if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                            {
                                SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(pGroup->vid, pGroup->ipType, pGroup->dip,
                                    pGroup->dipv6.addr, &tmpGroup)), SYS_ERR_FAILED);
                                tmpGroup.sip = pGroup->sip;
                            }

                            if (SYS_ERR_OK == mcast_hw_mcst_entry_del(pGroup))
                            {
                                igmp_hw_entry_num--;
    							//TBD update current bandwidth but unknown port
                            }

                            tmpNum = group_sortedAry_entry_num;

                            mcast_group_del(pGroup);

                            if(IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                            {
                                mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroup), &sortedIdx, &groupHead);

                                if(groupHead == NULL)
                                {
                                    mcast_igmp_group_head_del(tmpGroup.dip, tmpGroup.vid);
                                }
                            }
                        }
                        else if (MULTICAST_TYPE_IPV6 == pGroup->ipType)
                        {
                            SYS_DBG(LOG_DBG_IGMP, "Group Timeout of IP = "IPADDRV6_PRINT"\n", IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));

                            /* TBD MLDv2
                            if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                            {
                                SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(pGroup->vid, pGroup->ipType, pGroup->dip,
                                    pGroup->dipv6.addr, &tmpGroup)), SYS_ERR_FAILED);
                                tmpGroup.sip = pGroup->sip;
                            }*/

                            if (SYS_ERR_OK == mcast_hw_mcst_entry_del(pGroup))
                            {
                                igmp_hw_entry_num--;
    							//TBD update current bandwidth but unknown port
                            }

                            tmpNum = group_sortedAry_entry_num;

                            mcast_group_del(pGroup);

                            /* TBD MLDv2
                            if(IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                            {
                                mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroup), &sortedIdx, &groupHead);
                                if(groupHead == NULL)
                                {
                                    mcast_igmp_group_head_del(tmpGroup.dip, tmpGroup.vid);
                                }
                            }*/
                        }
                        if (tmpNum != group_sortedAry_entry_num)
                            i--;
                    }
                }

                pGroup = pNextgroup;
                continue;
            }

			if(MODE_GPON != igmpCtrl.ponMode)
			{
	            mcast_vlan_db_get(pGroup->vid, pGroup->ipType, &pMcastVlan);

	            if (NULL == pMcastVlan)
	            {
	                pGroup = pNextgroup;
	                continue;
	            }

	            mcast_querier_db_get(pGroup->vid, pGroup->ipType, &pQuerEntry);

	            if (pQuerEntry == NULL)
	            {
	                SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
	                    "%s():%d VLAN-%d doesn't have query DB entry!\n", __FUNCTION__, __LINE__, pGroup->vid);

	                IGMP_DB_SEM_UNLOCK();

	                mcast_groupAdd_stop = DISABLED;

	                return SYS_ERR_FAILED;
	            }
			}

            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pGroup->mbr)
            {
                // if is the group entry static port , continue;
                if(IS_LOGIC_PORTMASK_PORTSET(pGroup->staticMbr, port))
                    continue;

                time = pGroup->p_mbr_timer[port];

                if (time == 0)
                {
                    SYS_DBG(pGroup->ipType == MULTICAST_TYPE_IPV4 ? LOG_DBG_IGMP : LOG_DBG_MLD,
                            "Warring! an existing mbr whose life is zero\n");
                }
                else
                {
                    if (time > PASS_SECONDS)
                    {
						if (SYS_ERR_OK != mcast_host_tracking_update_member_time_per_port(pGroup, (uint16)port))
						{
							SYS_DBG(LOG_DBG_IGMP, " group timer is timeout need to del host entry in this port \n");
						}
                        pGroup->p_mbr_timer[port] -= PASS_SECONDS;
                    }
					else
                    {
                        /*v2/v3 support grp specfic query*/
                        if (MODE_GPON != igmpCtrl.ponMode && ((pQuerEntry->status == IGMP_QUERIER &&
                        	(pGroup->p_gsQueryCnt[port] >= 1 && pGroup->p_gsQueryCnt[port] < pMcastVlan->operLastMmbrQueryCnt)) ||
                            (pQuerEntry->status == IGMP_QUERIER &&
                            (pGroup->v3.p_gss_query_cnt[port] >= 1 && pGroup->v3.p_gss_query_cnt[port] < pMcastVlan->operLastMmbrQueryCnt))))
                        {
                            if ((pQuerEntry->status == IGMP_QUERIER && (pGroup->p_gsQueryCnt[port] >= 1 &&
                                pGroup->p_gsQueryCnt[port] < pMcastVlan->operLastMmbrQueryCnt)))
                            {
                                pGroup->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;
                                pGroup->p_gsQueryCnt[port]++;
                                //mcast_igmp_send_grp_specific_query(pGroup->vid, pGroup->dip, port);
                                mcast_send_gs_query(pGroup, port);
                            }
                            else
                            {
                                if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                {
                                    if ((pQuerEntry->status == IGMP_QUERIER && (pGroup->v3.p_gss_query_cnt[port] >= 1 &&
                                        pGroup->v3.p_gss_query_cnt[port] < pMcastVlan->operLastMmbrQueryCnt)))
                                    {
                                        pGroup->p_mbr_timer[port] = pMcastVlan->operGsqueryIntv;

                                        mcast_igmp_send_grp_src_specific_query(pGroup->vid, pGroup->dip, &pGroup->sip, 1,port);
                                        pGroup->v3.p_gss_query_cnt[port] ++;
                                    }
                                }
                            }
                        }

                        else
                        {
                            if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
                            {
                                SYS_DBG(LOG_DBG_IGMP,
                                    "Group membership Timeout on Port: %d for IP = "IPADDR_PRINT"\n",
                                    port, IPADDR_PRINT_ARG(pGroup->dip));

                                pGroup->p_mbr_timer[port] = 0;

                                if (MODE_GPON != igmpCtrl.ponMode)
                                    LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                                if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                {
                                    if (IS_LOGIC_PORTMASK_PORTSET(pGroup->v3.isGssQueryFlag, port))
                                        LOGIC_PORTMASK_CLEAR_PORT(pGroup->v3.isGssQueryFlag, port);
                                }

                                if (!IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                                {
                                    LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
                                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);
                                    if (SYS_ERR_OK == mcast_hw_mcst_mbr_remove(pGroup, &delPmsk))
                                        p_igmp_stats->p_port_info[port].curBw -= pGroup->imputedGrpBw;

                                    if (igmpCtrl.ponMode)
                                        LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                                    if (SYS_ERR_OK == mcast_host_tracking_del_per_port(pGroup, (uint16)port))
                                    {
                                        SYS_DBG(LOG_DBG_IGMP, " group timer is timeout need to del host entry in this port \n");
                                    }

                                    if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                    {
                                        SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(pGroup->vid, pGroup->ipType,
                                            pGroup->dip, pGroup->dipv6.addr, &tmpGroup)), SYS_ERR_FAILED);
                                        tmpGroup.sip = pGroup->sip;
                                    }

                                    tmpNum = group_sortedAry_entry_num;
                                    if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                                    {
                                        mcast_group_del(pGroup);
                                    }

                                    if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                    {
                                        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroup), &sortedIdx, &groupHead);
                                        if (groupHead == NULL)
                                        {
                                            mcast_igmp_group_head_del(tmpGroup.dip, tmpGroup.vid);
                                        }
                                    }

                                    if (tmpNum != group_sortedAry_entry_num)
                                        i--;
                                }
                                else
                                    flag = TRUE;
                            }
                            else
                            {
                                SYS_DBG(LOG_DBG_IGMP,
                                    "Group membership Timeout on Port: %d for IPv6 = "IPADDRV6_PRINT"\n",
                                    port, IPADDRV6_PRINT_ARG(pGroup->dipv6.addr));

                                pGroup->p_mbr_timer[port] = 0;

                                if (MODE_GPON != igmpCtrl.ponMode)
                                    LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                                /* TBD MLDv2
                                if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                {
                                    if (IS_LOGIC_PORTMASK_PORTSET(pGroup->v3.isGssQueryFlag, port))
                                        LOGIC_PORTMASK_CLEAR_PORT(pGroup->v3.isGssQueryFlag, port);
                                }*/

                                if (!IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                                {
                                    LOGIC_PORTMASK_CLEAR_ALL(delPmsk);
                                    LOGIC_PORTMASK_SET_PORT(delPmsk, port);

                                    if (SYS_ERR_OK == mcast_hw_mcst_mbr_remove(pGroup, &delPmsk))
                                        p_igmp_stats->p_port_info[port].curBw -= pGroup->imputedGrpBw;

                                    if (igmpCtrl.ponMode)
                                        LOGIC_PORTMASK_CLEAR_PORT(pGroup->mbr, port);

                                    if (SYS_ERR_OK == mcast_host_tracking_del_per_port(pGroup, (uint16)port))
                                    {
                                        SYS_DBG(LOG_DBG_IGMP, " group timer is timeout need to del host entry in this port \n");
                                    }
                                    /* TBD MLD v2
                                    if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                    {
                                        SYS_ERR_CHK((SYS_ERR_OK != mcast_build_groupEntryKey(pGroup->vid, pGroup->ipType,
                                            pGroup->dip, pGroup->dipv6.addr, &tmpGroup)), SYS_ERR_FAILED);
                                        tmpGroup.sip = pGroup->sip;
                                    }*/

                                    tmpNum = group_sortedAry_entry_num;
                                    if (IS_LOGIC_PORTMASK_CLEAR(pGroup->mbr))
                                    {
                                        mcast_group_del(pGroup);
                                    }

                                    /* TBD MLDv2
                                    if (IGMP_VERSION_V3_FULL == p_igmp_stats->oper_version)
                                    {
                                        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&tmpGroup), &sortedIdx, &groupHead);
                                        if (groupHead == NULL)
                                        {
                                            mcast_igmp_group_head_del(tmpGroup.dip, tmpGroup.vid);
                                        }
                                    }*/

                                    if (tmpNum != group_sortedAry_entry_num)
                                        i--;
                                }
                                else
                                    flag = TRUE;
                            }

                        }
                    }
					//SYS_DBG(LOG_DBG_IGMP, "%s() mbrTime=%u gsQueryCnt=%u +++++\n", __FUNCTION__, pGroup->mbrTimer[port], pGroup->gsQueryCnt[port]);
                }
            }

            if (flag)
            {
                /* In SIP+DIP, if the portmask of a group which created by Mcst-Data packet becomes empty,
                we don't delete this entry from ASIC immediately */
                pGroup->groupTimer = IGMP_TEMP_PERIOD;


                /* Restore the mbr of unknown multicast */
                ipMcstEntry.dip = pGroup->dip;
                ipMcstEntry.sip = pGroup->sip;
                ipMcstEntry.vid = pGroup->vid;
				ipMcstEntry.fmode = pGroup->fmode;
				ipMcstEntry.isGrpRefB = mcast_group_ref_get(pGroup);
                ipMcstEntry.care_vid = pGroup->care_vid;
                LOGIC_PORTMASK_CLEAR_ALL(ipMcstEntry.portmask);

                if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_FLOOD)
                {
                    LOGIC_PORTMASK_SET_ALL(ipMcstEntry.portmask);
                }
                else if (gUnknown_mcast_action == IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT)
                {
                    mcast_router_db_get(pGroup->vid, pGroup->ipType, &pRouterEntry);
                    if (pRouterEntry)
                        LOGIC_PORTMASK_OR(ipMcstEntry.portmask, ipMcstEntry.portmask, pRouterEntry->router_pmsk);
                    else
                        SYS_DBG(LOG_DBG_IGMP, "Warring! This vlan has no router port!\n");
                }
                if(SYS_ERR_OK == rsd_ipMcastEntry_set(&ipMcstEntry))
					;//TBD: update current bandwidth but port should be multiple
            }
            pGroup = pNextgroup;
        }
    }
    IGMP_DB_SEM_UNLOCK();
    mcast_groupAdd_stop = DISABLED;
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmp_group_head_timer
* Description:
*      group head timer process group filter mode.
* Input:
*      None
* Output:
*      None
* Return:
*      None
* Note:
*      None
*/
void mcast_igmp_group_head_timer(void)
{
    int i;
    uint16 port, portid;
    uint16 time;
    uint16 sortedIdx;
    uint16 flag = FALSE;

    igmp_group_entry_t  *pEntry = NULL, *groupHead;
    igmp_group_head_entry_t  *pGrpHead = NULL;
    igmp_group_entry_t groupEntry;

    IGMP_DB_SEM_LOCK();

    for(i = 0; i < SYS_MCAST_MAX_GROUP_NUM; i++)
    {
        pGrpHead = &(p_group_head_db[i]);

        if(0 == pGrpHead->dip)
            continue;

        mcast_build_groupEntryKey(pGrpHead->vid, pGrpHead->ipType, pGrpHead->dip, NULL, &groupEntry);
        groupEntry.sip = pGrpHead->sip;

        FOR_EACH_LOGIC_PORT(port)
        {
            if( PORT_IS_FILTER_EXCLUDE(pGrpHead->fmode, port))  //group filter mode is EXCLUDE
            {
                time = pGrpHead->p_mbr_timer[port]; //group timer
                if (time == 0)
                    SYS_DBG(LOG_DBG_IGMP, "Warring! an existing mbr whose life is zero\n");
                else
                {
                    if (time > PASS_SECONDS)
                        pGrpHead->p_mbr_timer[port] -= PASS_SECONDS; //At least one mbr is exclude mode
                    else
                    {
                        mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);
                        pEntry = groupHead;
                        while (pEntry)
                        {
                            if (pEntry->v3.p_mbr_fwd_flag[port] == IGMPV3_FWD_CONFLICT)//include src record state
                            {
                                FOR_EACH_LOGIC_PORT(portid)
                                {
                                	//check src timer != 0 and group filter mode set to INCLUDE
                                    if (pEntry->p_mbr_timer[portid] != 0)
                                    {
                                        PORT_SET_FILTER_INCLUDE(pGrpHead->fmode, port);
 										if(MODE_GPON == igmpCtrl.ponMode)
											PORT_SET_FILTER_INCLUDE(groupHead->fmode, port);
                                        flag = TRUE;
                                        break;
                                    }
                                }
                            }
                            else if(pEntry->v3.p_mbr_fwd_flag[port] == IGMPV3_FWD_NORMAL)
                            {
                                flag = TRUE;
                                pEntry->v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_CONFLICT;
                            }
                            pEntry = pEntry->next_subgroup;
                        }

                        if (flag == FALSE)
                        {
                            PORT_SET_FILTER_INCLUDE(pGrpHead->fmode, port);
							if (MODE_GPON == igmpCtrl.ponMode)
								PORT_SET_FILTER_INCLUDE(groupHead->fmode, port);
                        }
                    }
                }
            }
        }
    }
    IGMP_DB_SEM_UNLOCK();
    return;
}

/* Function Name:
*      mcast_igmpv3_static_groupEntry_set
* Description:
*      Set IGMP static group entry.
* Input:
*      vid - vlan id
*      groupAddr - group Ip
*      sourceAddr - source Ip
*      pSpmsk - static port list
*      fmode - include /exclude
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      None
*/
int32 mcast_igmpv3_static_groupEntry_set(
    sys_vid_t vid, uint32 groupAddr, uint32 sourceAddr, sys_logic_portmask_t *pSpmsk, uint8 fmode)
{
    int32                   ret;
    uint8                   isStaticEntryExist = FALSE;
    ipAddr_t                clientIp;
    igmp_group_entry_t      *pEntry = NULL, groupEntry, newEntry;
    igmp_group_head_entry_t *group_head_entry;
    sys_logic_portmask_t    exMbr, delStaticMbr;
    sys_logic_port_t        port;
    sys_ipMcastEntry_t      ipMcst_entry;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;

	memset(&clientIp, 0, sizeof(ipAddr_t));
    memset(&exMbr, 0, sizeof(sys_logic_portmask_t));
    memset(&delStaticMbr, 0, sizeof(sys_logic_portmask_t));

    IGMP_DB_SEM_LOCK();

    if (!IGMP_IN_MULTICAST(ntohl(groupAddr)))
    {
        SYS_PRINTF("IGMP Group address Error: Group Address("IPADDR_PRINT") is not in "
            "Multicast range(224.0.0.0~239.255.255.255)\n", IPADDR_PRINT_ARG(groupAddr));
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    if ((ntohl(groupAddr) >= ntohl(0xE0000000L)) && (ntohl(groupAddr) <= ntohl(0xE00000FFL)))
    {
        SYS_PRINTF("IGMP Group address  Error: Group Address("IPADDR_PRINT") belongs to "
            "Reserved Multicast range(224.0.0.0~224.0.0.255)!\n", IPADDR_PRINT_ARG(groupAddr));
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    if((0x0 == sourceAddr) || (0xFFFFFFFF == sourceAddr))
    {
        SYS_PRINTF("IP address Error!\n");
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    // if the group head and group exist , delete it.
    mcast_igmp_group_head_get(groupAddr, vid, &group_head_entry);
    if (group_head_entry)
    {
        SYS_DBG(LOG_DBG_IGMP, "Group head is exist !\n");
        osal_memset(&groupEntry, 0, sizeof(igmp_group_entry_t));
        groupEntry.vid = vid;
        groupEntry.dip = groupAddr;
        groupEntry.sip = sourceAddr;
        groupEntry.ipType = ipType;

        pEntry = mcast_group_get(&groupEntry);
        if(pEntry != NULL)
        {
            if(IGMP_GROUP_ENTRY_DYNAMIC == pEntry->form)
            {
                IGMP_DB_SEM_UNLOCK();

                mcast_static_group_clear(&groupEntry);
                IGMP_DB_SEM_LOCK();
            }
            else
            {
                if(FILTER_MODE_INCLUDE == fmode)
                {
                    // add all set staticInMbr
                    LOGIC_PORTMASK_ANDNOT(exMbr,pEntry->staticMbr, pEntry->staticInMbr);

                    LOGIC_PORTMASK_ANDNOT(delStaticMbr,pEntry->staticInMbr, *pSpmsk);

                    osal_memcpy(&pEntry->staticInMbr, pSpmsk, sizeof(sys_logic_portmask_t));
                    LOGIC_PORTMASK_ANDNOT(pEntry->staticInMbr, pEntry->staticInMbr,delStaticMbr);
                    LOGIC_PORTMASK_ANDNOT(pEntry->staticInMbr, pEntry->staticInMbr, exMbr);


                    LOGIC_PORTMASK_OR(pEntry->staticMbr, pEntry->staticMbr, *pSpmsk);
                    LOGIC_PORTMASK_ANDNOT(pEntry->staticMbr, pEntry->staticMbr, delStaticMbr);

                    //update the entry mbr.
                    LOGIC_PORTMASK_OR(pEntry->mbr, pEntry->mbr, *pSpmsk);
                    LOGIC_PORTMASK_ANDNOT(pEntry->mbr, pEntry->mbr, delStaticMbr);
                }
                else if(FILTER_MODE_EXCLUDE == fmode)
                {

                    LOGIC_PORTMASK_ANDNOT(exMbr,pEntry->staticMbr, pEntry->staticInMbr);
                    LOGIC_PORTMASK_ANDNOT(delStaticMbr,exMbr, *pSpmsk);

                    //update the entry mbr.
                    LOGIC_PORTMASK_OR(pEntry->mbr, pEntry->mbr, *pSpmsk);
                    LOGIC_PORTMASK_ANDNOT(pEntry->mbr, pEntry->mbr, delStaticMbr);

                    //update the static mbr;
                    LOGIC_PORTMASK_OR(pEntry->staticMbr, pEntry->staticMbr, *pSpmsk);
                    LOGIC_PORTMASK_ANDNOT(pEntry->staticMbr, pEntry->staticMbr, delStaticMbr);

                    // add all set exMbr, delete the InMbr
                    LOGIC_PORTMASK_ANDNOT(pEntry->staticInMbr, pEntry->staticInMbr, *pSpmsk);

                    FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, pEntry->mbr)
                    {
                        if((!IS_LOGIC_PORTMASK_PORTSET(pEntry->staticInMbr, port)) ||
                            ((IS_LOGIC_PORTMASK_PORTSET(pEntry->mbr, port)) && (!IS_LOGIC_PORTMASK_PORTSET(pEntry->staticMbr, port))))
                        {
                            pEntry->v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NOT;
                        }
                    }
                }

                isStaticEntryExist = TRUE;
            }
        }
    }

    mcast_igmp_group_head_get(groupAddr, vid, &group_head_entry);
    if (group_head_entry == NULL)
    {
        SYS_DBG(LOG_DBG_IGMP, "Group head is not exist !\n");
        // create group head

        group_head_entry = mcast_igmp_free_group_head_get();
        if(group_head_entry != NULL)
        {
            osal_memset(&group_head_entry->fmode, 0, sizeof(sys_logic_portmask_t));
            /*set group entry filter mode*/
            group_head_entry->dip = groupAddr;
            group_head_entry->sip = 0;
            group_head_entry->vid = vid;

            group_head_entry->mac[0] = 0x01;
            group_head_entry->mac[1] = 0x00;
            group_head_entry->mac[2] = 0x5e;
            group_head_entry->mac[3] = (groupAddr >> 16) & 0xff;
            group_head_entry->mac[4] = (groupAddr >> 8) & 0xff;
            group_head_entry->mac[5] = groupAddr & 0xff;
        }
        else
        {
            SYS_DBG(LOG_DBG_IGMP, "Group head database is full!\n");
            IGMP_DB_SEM_UNLOCK();
            return SYS_ERR_FAILED;
        }
    }

    osal_memset(&newEntry, 0 , sizeof(igmp_group_entry_t));
    newEntry.v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

    if (!newEntry.v3.p_mbr_fwd_flag)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
    osal_memset(newEntry.v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

    SYS_DBG(LOG_DBG_IGMP, "Add new static group entry Group is: "IPADDR_PRINT" , sourece  ip is : "IPADDR_PRINT" \n",
        IPADDR_PRINT_ARG(groupAddr), IPADDR_PRINT_ARG(sourceAddr));

    newEntry.dip                = groupAddr;
    newEntry.sip                = sourceAddr;
    newEntry.vid                = vid;
    newEntry.ipType             = ipType;
    newEntry.lookupTableFlag    = L2TABLE_WROTE;

    if (isStaticEntryExist)
    {
        osal_memcpy(&newEntry.mbr, &pEntry->mbr,sizeof(sys_logic_portmask_t));
        osal_memcpy(&newEntry.staticMbr, &pEntry->staticMbr, sizeof(sys_logic_portmask_t));
        osal_memcpy(&newEntry.staticInMbr, &pEntry->staticInMbr, sizeof(sys_logic_portmask_t));
    }
    else
    {
        osal_memcpy(&newEntry.mbr, pSpmsk, sizeof(sys_logic_portmask_t));
        osal_memcpy(&newEntry.staticMbr, pSpmsk, sizeof(sys_logic_portmask_t));

        if (FILTER_MODE_INCLUDE == fmode)
        {
            osal_memcpy(&newEntry.staticInMbr, pSpmsk, sizeof(sys_logic_portmask_t));
        }
        else if (FILTER_MODE_EXCLUDE == fmode)
        {
            FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, newEntry.mbr)
            {
                newEntry.v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NOT;
            }
        }
    }

    newEntry.form = IGMP_GROUP_ENTRY_STATIC;
    newEntry.mac[0] = 0x01;
    newEntry.mac[1] = 0x00;
    newEntry.mac[2] = 0x5e;
    newEntry.mac[3] = (groupAddr >> 16) & 0xff;
    newEntry.mac[4] = (groupAddr >> 8) & 0xff;
    newEntry.mac[5] = groupAddr & 0xff;

    ret = mcast_group_add(&newEntry, clientIp, 0, 0);

    if (ret)
    {
        SYS_DBG(LOG_DBG_IGMP, "Failed add a static group!  ret:%d\n", ret);
        osal_free(newEntry.v3.p_mbr_fwd_flag);
        IGMP_DB_SEM_UNLOCK();
        return ret;
    }

    osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));
    ipMcst_entry.vid = vid;
    ipMcst_entry.sip = newEntry.sip;
    ipMcst_entry.dip = newEntry.dip;

    if (isStaticEntryExist ||((FILTER_MODE_INCLUDE == fmode)&&(!isStaticEntryExist)) )
    {
        mcast_igmpv3_fwdPmsk_get(&ipMcst_entry.portmask, &newEntry);
        //osal_memcpy(&ipMcst_entry.portmask, &newEntry.mbr, sizeof(sys_logic_portmask_t));
        if (SYS_ERR_OK != (ret = rsd_ipMcastEntry_set(&ipMcst_entry)))
        {
            SYS_DBG(LOG_DBG_IGMP, "Set ip Mcst entry  failed!  vid:%d  \n", vid);
            osal_free(newEntry.v3.p_mbr_fwd_flag);
            IGMP_DB_SEM_UNLOCK();
            return ret;
        }
    }
    osal_free(newEntry.v3.p_mbr_fwd_flag);
    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_igmpv3_static_filter_group_set
* Description:
*      Set IGMP static filter group entry.
* Input:
*       pGroup -- group entry
*       fmode - include/exclude
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      pGroup will inpurt vid/dip/sip/fmode
*/
int32 mcast_igmpv3_static_filter_group_set(igmp_group_entry_t *pGroup, uint8 fmode)
{
    uint16 sortedIdx;
    int32 ret;
    igmp_group_entry_t *groupHead = NULL, *pEntry = NULL;
    igmp_group_entry_t  newEntry;
    sys_logic_port_t  port;
    sys_ipMcastEntry_t  ipMcst_entry;
	ipAddr_t clientIp;

    SYS_PARAM_CHK(NULL == pGroup, SYS_ERR_NULL_POINTER);

    IGMP_DB_SEM_LOCK();

	memset(&clientIp, 0, sizeof(ipAddr_t));

    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);
    if(groupHead == NULL)
    {
        SYS_DBG(LOG_DBG_IGMP, "the Group ip ("IPADDR_PRINT") is not exist  \n", IPADDR_PRINT_ARG(pGroup->dip));
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }
    else
    {
        osal_memset(&ipMcst_entry, 0, sizeof(sys_ipMcastEntry_t));
        ipMcst_entry.vid = pGroup->vid;
        ipMcst_entry.sip = pGroup->sip;
        ipMcst_entry.dip = pGroup->dip;

#if 0
        //keep groupHead sip == 0
        if(groupHead->sip == 0)
        {
            groupHead->sip = pGroup->sip;
            groupHead->lookupTableFlag = L2TABLE_WROTE;

            mcast_igmpv3_fwdPmsk_get(&ipMcst_entry.portmask, groupHead);
        }
        else
#endif
        {
                //the group entry is exist , then delete it
            pEntry = mcast_group_get(pGroup);
            if (pEntry != NULL)
            {
                IGMP_DB_SEM_UNLOCK();
                mcast_static_group_clear(pGroup);
                IGMP_DB_SEM_LOCK();
            }

            osal_memset(&newEntry, 0 , sizeof(igmp_group_entry_t));
            newEntry.v3.p_mbr_fwd_flag = (igmp_fwdFlag_t *)osal_alloc(sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);
            if (!newEntry.v3.p_mbr_fwd_flag)
            {
                IGMP_DB_SEM_UNLOCK();
                return SYS_ERR_FAILED;
            }
            osal_memset(newEntry.v3.p_mbr_fwd_flag, 0, sizeof(igmp_fwdFlag_t) * igmpCtrl.sys_max_port_num);

            SYS_DBG(LOG_DBG_IGMP, "Add new static group entry Group is: "IPADDR_PRINT" , sourece  ip is : "IPADDR_PRINT" \n",
                IPADDR_PRINT_ARG(pGroup->dip), IPADDR_PRINT_ARG(pGroup->sip));

            newEntry.dip = pGroup->dip;
            newEntry.sip = pGroup->sip;
            newEntry.vid = pGroup->vid;
            newEntry.ipType = pGroup->ipType;

            newEntry.lookupTableFlag = L2TABLE_NOT_WROTE;

            osal_memcpy(&newEntry.staticInMbr, &groupHead->staticInMbr, sizeof(sys_logic_portmask_t));
            osal_memcpy(&newEntry.staticMbr, &groupHead->staticMbr, sizeof(sys_logic_portmask_t));
            LOGIC_PORTMASK_OR(newEntry.mbr, newEntry.mbr, newEntry.staticMbr);

            if (FILTER_MODE_EXCLUDE == fmode)
            {
                FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, newEntry.staticMbr)
                {
                    if(!IS_LOGIC_PORTMASK_PORTSET(newEntry.staticInMbr, port))
                        newEntry.v3.p_mbr_fwd_flag[port] = IGMPV3_FWD_NOT;
                }
            }

            newEntry.form = IGMP_GROUP_ENTRY_STATIC;

            newEntry.mac[0] = 0x01;
            newEntry.mac[1] = 0x00;
            newEntry.mac[2] = 0x5e;
            newEntry.mac[3] = (pGroup->dip >> 16) & 0xff;
            newEntry.mac[4] = (pGroup->dip >> 8) & 0xff;
            newEntry.mac[5] = pGroup->dip & 0xff;

            ret = mcast_group_add(&newEntry, clientIp, 0, 0);
            if (ret)
            {
                SYS_DBG(LOG_DBG_IGMP, "Failed add a static group!  ret:%d\n", ret);
                osal_free(newEntry.v3.p_mbr_fwd_flag);
                IGMP_DB_SEM_UNLOCK();
                return ret;
            }
            osal_free(newEntry.v3.p_mbr_fwd_flag);
        }
    }

    pEntry = mcast_group_get(pGroup);
    if(pEntry != NULL)
    {
        mcast_igmpv3_ipMcstEntry_update_dipsip(pGroup->vid, pGroup->dip, pGroup->sip, pEntry);
    }

    IGMP_DB_SEM_UNLOCK();
    return SYS_ERR_OK;
}

/*Below functions are move from ctc_mc.c for do igmp action in IGMP module, and have be modified */

int32 mcast_igmpMode_set(uint32 mode)
{
	if (mode >= IGMP_MODE_STOP)
	{
		return SYS_ERR_FAILED;
	}

	if (igmpCtrl.igmpMode != mode)
	{
		igmpCtrl.igmpMode = mode;
	}

	return SYS_ERR_OK;
}

uint32 mcast_fastLeaveMode_get(uint32 portId)
{
	return ((FALSE != IS_LOGIC_PORTMASK_PORTSET(igmpCtrl.leaveMode, portId)) ? IGMP_LEAVE_MODE_FAST : IGMP_LEAVE_MODE_NON_FAST_LEAVE);
}


int32 mcast_igmp_mld_ctrl_init(ponMode_t ponMode)
{

	mcast_igmpMode_set(IGMP_MODE_SNOOPING);

	if (MODE_GPON == ponMode)
    {
        LOGIC_PORTMASK_CLEAR_ALL(igmpCtrl.leaveMode);
    }
    else
    {
        //By default, epon enable fast leave
        LOGIC_PORTMASK_SET_ALL(igmpCtrl.leaveMode);
    }

    return SYS_ERR_OK;
}

mcvlan_tag_oper_mode_t mcast_igmpTagOperPerPortGet(uint32 portId)
{
    SYS_ERR_CHK((FALSE == IS_VALID_PORT_ID(portId) && 255 != portId), SYS_ERR_FAILED);

    return p_igmp_tag_oper_per_port[portId];
}

int mcast_igspVlanMapExist(uint32 portId, uint32 vlanId)
{

    if ((0 != vlanId) && (!VALID_VLAN_ID(vlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
		SYS_DBG(LOG_DBG_IGMP, "fail to %s() [%d] to vlan [%d]\n", __FUNCTION__, portId, vlanId);
        return FALSE;
    }

    if (0 != IS_LOGIC_PORTMASK_PORTSET(igspVlanMap[vlanId].portmask, portId))
    {
    	SYS_DBG(LOG_DBG_IGMP, "portid %d is in mc vlan [%d]\n", portId, vlanId);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int mcast_igmpMcVlanGet(uint32 portId, uint16 *vlanId, uint32 *num)
{
    uint32 j;
    uint8 vlanIdNum = 0;

	if (IS_VALID_PORT_ID(portId) == FALSE && portId != 255)
	{
	    return SYS_ERR_FAILED;
	}

	if (!vlanId || !num)
	{
		return SYS_ERR_FAILED;
	}

    SYS_DBG(LOG_DBG_IGMP, "get port [%d] mc vlan table\n", portId);

    for (j = 1; j < MAX_VLAN_NUMBER; j++)
    {
        if (TRUE == mcast_igspVlanMapExist(portId, j))
        {
        	*vlanId = j;
        	vlanId++;
        	vlanIdNum++;
        }
    }

    *num = vlanIdNum;

    SYS_DBG(LOG_DBG_IGMP, "get port [%d] mc vlan table number %d .\n", portId, *num);
    return SYS_ERR_OK;
}

int _mcast_igmpMcVlanDelete(uint32 portId, uint32 vlanId)
{
    int ret=0;
    uint32 uiPPort;
    rtk_portmask_t stLMask;

    if ((0 != vlanId) && (!VALID_VLAN_ID(vlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
		SYS_DBG(LOG_DBG_IGMP, "fail to %s() [%d] to vlan [%d]\n", __FUNCTION__, portId, vlanId);
        return SYS_ERR_VLAN_ID;
    }

    LOGIC_PORTMASK_CLEAR_PORT(igspVlanMap[vlanId].portmask,portId);

    SYS_DBG(LOG_DBG_IGMP, "delete port [%d] from mc vlan %d..\n", portId, vlanId);

    mcast_del_port_from_vlan(vlanId,portId);

    SYS_DBG(LOG_DBG_IGMP, "delete port [%d] from mc vlan %d\n", portId, vlanId);

    if (IS_LOGIC_PORTMASK_CLEAR(igspVlanMap[vlanId].portmask))
    {
        if (0 < igmpMcVlanNum)
        {
            igmpMcVlanNum--;
        }

        igspVlanMap[vlanId].vid=0;
        ext_mcast_vlan_del(vlanId);

        SYS_DBG(LOG_DBG_IGMP, "delete empty mc vlan %d\n",vlanId);
    }


    switch (mcast_igmpTagOperPerPortGet(portId))
    {
        case TAG_OPER_MODE_TRANSPARENT:
        	uiPPort = PortLogic2PhyID(portId);
        	RTK_PORTMASK_RESET(stLMask);
        	RTK_PORTMASK_PORT_SET(stLMask, uiPPort);
        	ret = pf_mc_vlan_member_remove(vlanId, stLMask);
        	SYS_ERR_CHK((ret != RT_ERR_OK), SYS_ERR_FAILED);
            break;
        case TAG_OPER_MODE_STRIP:
        	uiPPort = PortLogic2PhyID(portId);
        	RTK_PORTMASK_RESET(stLMask);
        	RTK_PORTMASK_PORT_SET(stLMask, uiPPort);
        	ret = pf_mc_vlan_member_remove(vlanId, stLMask);
            SYS_ERR_CHK((ret != RT_ERR_OK), SYS_ERR_FAILED);
            break;
        case TAG_OPER_MODE_TRANSLATION:
            break;
        default:
            break;
    }
    return SYS_ERR_OK;
}

int mcast_igmpMcVlanDelete(uint32 portId, uint32 vlanId)
{
	int ret = SYS_ERR_OK;

	if (FALSE == IS_VALID_PORT_ID(portId) && 255 != portId)
  	{
  		return SYS_ERR_FAILED;
  	}

	SYS_DBG(LOG_DBG_IGMP, "delete the mc vlan of port : %d\n", portId);

    if (mcast_igspVlanMapExist(portId, vlanId))
	{
		ret =_mcast_igmpMcVlanDelete(portId,vlanId);
        SYS_ERR_CHK((SYS_ERR_OK != ret), ret);
	}
  	return SYS_ERR_OK;
}

int mcast_ismvlan(int vid)
{
	if (1 == vid)
    {
	    return TRUE;
	}
    else
    {
	    return FALSE;
    }
}

int mcast_mcVlanExist(uint32 vlanId)
{

    if ((0 != vlanId) && (!VALID_VLAN_ID(vlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
		SYS_DBG(LOG_DBG_IGMP, "fail to %s() vlan [%d]\n", __FUNCTION__, vlanId);
        return FALSE;
    }

    if (0 == igspVlanMap[vlanId].vid)
    {
        return FALSE;
    }

    return TRUE;
}

int mcast_igmpmcvlancheck(void)
{
    int     delflag = 0, num = 0, i = 0, j = 0;
    uint32  portId, vlannum = 0;
    igmp_control_entry_t ctl_entry_list[MAX_MC_GROUP_NUM];
    uint16  tmpvlanId[MAX_VLAN_NUMBER];

    if (IGMP_MODE_CTC != igmpCtrl.igmpMode)
    {
    	return 0;
    }

    if ((MC_CTRL_GDA_MAC_VID == igmpCtrl.controlType)
    	|| (MC_CTRL_GDA_GDA_IP_VID == igmpCtrl.controlType))
    {
    	mcast_ctrlGrpEntry_get(ctl_entry_list, &num);
    	FOR_EACH_LOGIC_PORT(portId)
    	{
    	    mcast_igmpMcVlanGet(portId, tmpvlanId, &vlannum);

            if(0 == vlannum)
    		    continue;

    		for (j = 0; j < vlannum; j++)
    		{
    			delflag = 1;
    			for (i = 0; i < num; i++)
    			{
    				if ((ctl_entry_list[i].port_id == portId) && (ctl_entry_list[i].vlan_id == tmpvlanId[j]))
    				{
    					delflag = 0;
    					break;
    				}
    			}

    			if (1 == delflag)
    			{
    				if (SYS_ERR_OK != mcast_igmpMcVlanDelete(portId, tmpvlanId[j]))
    				{
                        SYS_DBG(LOG_DBG_IGMP, "mcast_igmpMcVlanDelete() FAIL\n");
    				}
    			}
    		}
    	}
    }
    return 0;
}

int mcast_igmpMcTagstripGet(uint32 portId, uint8 *tagOper)
{
    SYS_ERR_CHK((FALSE == IS_VALID_PORT_ID(portId) && 255 != portId), SYS_ERR_FAILED);

    SYS_ERR_CHK((!tagOper), SYS_ERR_FAILED);

    *tagOper = mcast_igmpTagOperPerPortGet(portId);

    return SYS_ERR_OK;
}

int mcast_igmpTagTranslationTableGet(uint32 portId, uint16 mcVid, uint16 *userVid)
{
    uint32 i;

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED == p_igmp_tag_translation_table[i].enable)
        {
            if (p_igmp_tag_translation_table[i].portId == portId &&
                p_igmp_tag_translation_table[i].mcVid == mcVid)
            {
                /* match */
                *userVid = p_igmp_tag_translation_table[i].userVid;
                return SYS_ERR_OK;
            }
        }
    }

    return SYS_ERR_FAILED;
}

int mcast_igmpTagTranslationTableGetbyUservid(uint32 portId, uint16 userVid, uint16 *mcVid)
{
    uint32 i;

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if (ENABLED == p_igmp_tag_translation_table[i].enable)
        {
            if (p_igmp_tag_translation_table[i].portId == portId &&
                p_igmp_tag_translation_table[i].userVid == userVid)
            {
                /* match */
                *mcVid = p_igmp_tag_translation_table[i].mcVid;
                return SYS_ERR_OK;
            }
        }
    }

    /* not match */
    return SYS_ERR_FAILED;
}

int mcast_igmpTagTranslationEntryGetPerPort(
    uint32 portId, igmp_vlan_translation_entry_vid_t *p_tranlation_entry, uint32 *num)
{
    uint32                              i, entryNum = 0;
    igmp_vlan_translation_entry_vid_t   *tmp = NULL;

    tmp = p_tranlation_entry;

    SYS_DBG(LOG_DBG_IGMP, "get port [%d] Translation table\n", portId);

    for (i = 0; i < VLAN_TRANSLATION_ENTRY; i++)
    {
        if(p_igmp_tag_translation_table[i].enable == ENABLED)
        {
            if(p_igmp_tag_translation_table[i].portId == portId)
            {
                tmp->mcVid      = p_igmp_tag_translation_table[i].mcVid;
                tmp->userVid    = p_igmp_tag_translation_table[i].userVid;
                tmp++;
                entryNum++;
                SYS_DBG(LOG_DBG_IGMP, "Translation item [%d]:mvlan %d,uservid %d\n",
                    entryNum,p_igmp_tag_translation_table[i].mcVid, p_igmp_tag_translation_table[i].userVid);
            }
        }
    }

    *num = entryNum;

	SYS_DBG(LOG_DBG_IGMP, "portid %d ,Translation number %d\n", portId, entryNum);

	return SYS_ERR_OK;
}

int igmp_rate_limit_check(uint32 portId)
{
	uint32 portidx;

	SYS_PARAM_CHK(IS_LOGIC_PORT_INVALID(portId), SYS_ERR_PORT_ID);

	IGMP_DB_SEM_LOCK();
	portidx = portId-1;

	if (0 == p_igmp_us_rate_limit[portidx])
	{
		p_mcast_total_rx_counter[portidx]++;
		IGMP_DB_SEM_UNLOCK();
		return SYS_ERR_OK;
	}

	if (p_rx_counter_current[portidx] >= p_remain_packets[portidx])
	{
		p_mcast_rx_drop_counter[portidx]++;
		IGMP_DB_SEM_UNLOCK();
		return SYS_ERR_IGMP_RATE_OVER;
	}
	else
	{
		p_mcast_total_rx_counter[portidx]++;
		p_rx_counter_current[portidx]++;
		IGMP_DB_SEM_UNLOCK();
		return SYS_ERR_OK;
	}
}

/* Function Name:
*      mcast_mld_suppre_enable_set
* Description:
*      Enable MLD suppre report function.
* Input:
*      enable - ENABLED/DISABLED MLD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/

int32 mcast_mld_suppre_enable_set(sys_enable_t enable)
{
    p_mld_stats->suppre_enable    = enable;
    return SYS_ERR_OK;
}

int32 mcast_mld_statistics_clear(void)
{
    p_mld_stats->total_pkt_rcvd   = 0;
    p_mld_stats->valid_pkt_rcvd   = 0;
    p_mld_stats->invalid_pkt_rcvd = 0;

    p_mld_stats->g_query_rcvd     = 0;
    p_mld_stats->gs_query_rcvd    = 0;
    p_mld_stats->gss_query_rcvd   = 0;
    p_mld_stats->leave_rcvd       = 0;
    p_mld_stats->report_rcvd      = 0;
    p_mld_stats->other_rcvd       = 0;

    p_mld_stats->g_query_xmit     = 0;
    p_mld_stats->gs_query_xmit    = 0;
    p_mld_stats->gss_query_xmit   = 0;
    p_mld_stats->leave_xmit       = 0;
    p_mld_stats->report_xmit      = 0;

    return SYS_ERR_OK;
}

int32 mcast_mld_group_del_byType(mcast_group_type_t type)
{
    sys_vlanmask_t vlanMask;

    if (type < IGMP_GROUP_ENTRY_DYNAMIC || type > IGMP_GROUP_ENTRY_ALL)
        return SYS_ERR_MCAST_GROUP_TYPE;

    VLANMASK_SET_ALL(vlanMask);

    IGMP_DB_SEM_LOCK();
    _mcast_hwEntryByVid_del(&vlanMask, MULTICAST_TYPE_IPV6, type);
    IGMP_DB_SEM_UNLOCK();
    return  SYS_ERR_OK;
}

int32 mcast_mld_snooping_stats_init(int32 boot)
{
    multicast_ipType_t ipType = MULTICAST_TYPE_IPV6;

    if (boot)
    {
        osal_memset(&pp_profile_db[ipType][0], 0, (igmpCtrl.sys_max_port_num - 1) * sizeof(sys_igmp_profile_t));

        SYS_ERR_CHK((SYS_ERR_OK != mcast_mld_enable_set(DFLT_MLD_EBL)), SYS_ERR_FAILED);
#if 0
        /*Config MLD form LAN port trap action*/
	    rsd_mld_ipMcastPkt2CpuEnable_set(p_mld_stats->igmpTrap_enable);

		/*mld from PON port trap to CPU*/
		if (MODE_GPON == igmpCtrl.ponMode) //based on TRAP option of GEPON config file  for 6.3.14
			rsd_igmp_ipMcastPkt2CpuEnablePerPort_set(DISABLED, GET_LOGICAL_PON_PORT());
		else
			rsd_igmp_ipMcastPkt2CpuEnablePerPort_set(ENABLED, GET_LOGICAL_PON_PORT());
#endif
    }
    else
    {
        SYS_ERR_CHK((SYS_ERR_OK != mcast_mld_statistics_clear()), SYS_ERR_FAILED);
    }
    return SYS_ERR_OK;
}

/* Function Name:
*      mcast_mld_enable_set
* Description:
*      Enable MLD module globally
* Input:
*      enable - ENABLED/DISABLED MLD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
int32 mcast_mld_enable_set(sys_enable_t enable)
{
    sys_vlanmask_t vlanMsk;

    SYS_PARAM_CHK(IS_ENABLE_INVALID(enable), SYS_ERR_INPUT);
    VLANMASK_SET_ALL(vlanMsk);

    if (ENABLED == enable && p_mld_stats->mldsnp_enable == DISABLED)
    {
        SYS_DBG(LOG_DBG_MLD, "MLD Snooping = Enabled\n");
        p_mld_stats->mldsnp_enable = ENABLED;

        if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action &&
            IGMP_DMAC_VID_MODE ==  p_igmp_stats->lookup_mode)
        {
            //SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, DISABLED)); wanghuanyu
        }

        if (DISABLED == p_igmp_stats->igmpsnp_enable)
        {
            igmp_thread_status = 0;
            if ((osal_thread_t)NULL == (igmp_thread_create("MLD Tick Thread", STACK_SIZE,
                SYS_THREAD_PRI_IGMP_TIMER, (void *)mcast_tick_thread, "MLD Tick")))
            {
                SYS_PRINTF("Mcast Tick Thread create failed\n");
                return SYS_ERR_FAILED;
            }
        }

        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_hwRestore(vlanMsk, MULTICAST_TYPE_IPV6)), SYS_ERR_FAILED);

      //  SYS_ERR_CHK(rsd_mcast_enable_set(enable));  wanghuanyu
    }
    else if (DISABLED == enable && p_mld_stats->mldsnp_enable == ENABLED)
    {
        p_mld_stats->mldsnp_enable = DISABLED;
        SYS_DBG(LOG_DBG_MLD, "MLD Snooping = Disabled\n");
        SYS_ERR_CHK((SYS_ERR_OK != mcast_mld_snooping_stats_init(0)), SYS_ERR_FAILED);

        if (IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT == gUnknown_mcast_action &&
            IGMP_DMAC_VID_MODE ==  p_igmp_stats->lookup_mode)
        {
          //  SYS_ERR_CHK(rsd_mcast_unKnown_routerPortFlood_set(MULTICAST_TYPE_IPV6, ENABLED)); wanghuanyu
        }

        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_save(vlanMsk, MULTICAST_TYPE_IPV6)), SYS_ERR_FAILED);
        SYS_ERR_CHK((SYS_ERR_OK != mcast_mld_group_del_byType(IGMP_GROUP_ENTRY_ALL)), SYS_ERR_FAILED);
        SYS_ERR_CHK((SYS_ERR_OK != mcast_static_group_restore()), SYS_ERR_FAILED);

        SYS_ERR_CHK((SYS_ERR_OK != mcast_router_dynPortMbr_del(MULTICAST_TYPE_IPV6)), SYS_ERR_FAILED);

        if (DISABLED == p_igmp_stats->igmpsnp_enable)
        {
            igmp_thread_status = 1;
            do { } while (igmp_thread_status >= 0);

         //   SYS_ERR_CHK(rsd_mcast_enable_set(enable)); wanghuanyu
        }
    }

	//SYS_ERR_CHK((SYS_ERR_OK != pf_ipMcastMode_set(enable)), SYS_ERR_FAILED);
    //SYS_ERR_CHK(rsd_mld_enable_set(enable, p_igmp_stats->lookup_mode));
    return SYS_ERR_OK;
}

int mcast_recv(unsigned char *frame, unsigned short frame_len)
{
    unsigned short len = 0;
    struct lw_l2_ctl_s l2_ctl;
    int ret = SYS_ERR_OK;
    uint8 buf[ETH_FRAME_LEN + 1000];
    sys_nic_pkt_t Pkt;
    sys_vid_t vid;
    multicast_ipType_t ipType = MULTICAST_TYPE_END;
    uint16 tagLen = 0;
    uint16 value = 0;
    uint32 portIdx;
    struct timespec recvTime;
    clock_gettime(CLOCK_MONOTONIC, &recvTime);

    osal_memset(buf, 0, sizeof(buf));
    osal_memset(&l2_ctl,0,sizeof(l2_ctl));

	if (frame_len > 60)
    {
		/*Check if LAN port over IGMP rate, if yes drop packet*/
		portIdx = frame[0];
		if ((LOGIC_PORT_START <= portIdx) && ((igmpCtrl.sys_max_port_num - 2) >= portIdx))
		{
			if (SYS_ERR_OK != igmp_rate_limit_check(portIdx))
			{
				//free skb?
				return SYS_ERR_OK;
			}
		}

		len = frame_len - 3;

		osal_memcpy(buf, &frame[3], len);

        /*
         * frame[0] is from source port
         * frame[1] ~ fraome[2] are vid which 12 bits
         */
		l2_ctl.sll_port = frame[0];
		value = frame[1];
		l2_ctl.sll_vlanid = (value << 8) | frame[2];
        if (0 == l2_ctl.sll_vlanid)
		    l2_ctl.sll_vlanid = 1;

        Pkt.length = (uint32)len;
        Pkt.data = buf;
        Pkt.tail = buf + len;
        Pkt.rx_tag.source_port = (uint8)(l2_ctl.sll_port);
        Pkt.rx_tag.inner_vid = l2_ctl.sll_vlanid;

        SYS_DBG(LOG_DBG_IGMP, "%s()@%d packet from physical port %u with vlanId=%u  \n",
            __FUNCTION__, __LINE__, (Pkt.rx_tag.source_port - 1), Pkt.rx_tag.inner_vid);

        //tag
        if (0x81 == buf[12] && 0x00 == buf[13])
        {
            Pkt.rx_tag.cvid_tagged = TRUE;
            vid = (*(sys_vid_t *)(&buf[6 + 6 + 2])) & 0xfff;
            Pkt.rx_tag.inner_vid = vid;
            Pkt.rx_tag.inner_pri = (((*(sys_vid_t *)(&buf[6 + 6 + 2])) >> 13) & 0xff);
            SYS_PRINTF("%s()@%d packet from physical port %u with vid=%u, pri=%u\n",
                __FUNCTION__, __LINE__, (Pkt.rx_tag.source_port - 1), vid, Pkt.rx_tag.inner_pri);
        }
        else if (0x88 == buf[12] && 0xa8 == buf[13])
        {
            Pkt.rx_tag.svid_tagged = TRUE;
            Pkt.rx_tag.outer_vid = (*(sys_vid_t *)(&buf[6 + 6 + 2])) & 0xfff;
            Pkt.rx_tag.outer_pri = (((*(sys_vid_t *)(&buf[6 + 6 + 2])) >> 13) & 0xff);
            if (0x81 == buf[14] && 0x00 == buf[15])
            {
                Pkt.rx_tag.cvid_tagged = TRUE;
                Pkt.rx_tag.inner_vid = (*(sys_vid_t *)(&buf[6 + 6 + 6]) & 0xfff);
                Pkt.rx_tag.inner_pri = (((*(sys_vid_t *)(&buf[6 + 6 + 6])) >> 13) & 0xff);
            }
            vid = l2_ctl.sll_vlanid;
        }
        else
        {
            Pkt.rx_tag.svid_tagged = FALSE;
            Pkt.rx_tag.cvid_tagged = FALSE;
            vid = l2_ctl.sll_vlanid;
        }

        if (Pkt.rx_tag.cvid_tagged)
            tagLen += 4;
        if (Pkt.rx_tag.svid_tagged)
            tagLen += 4;

        if (0x01 == buf[0] && 0x00 == buf[1] && 0x5e == buf[2])
        {
            if (0x08 == buf[6 + 6 + tagLen] && 0x00 == buf[6 + 6 + tagLen + 1])
            {
                ipType = MULTICAST_TYPE_IPV4;
            }
        }
        else if (buf[0] == 0x33 && buf[1] == 0x33)
        {
            SYS_DBG(LOG_DBG_IGMP, "%s()@%d packet is 0x86dd \n",
                __FUNCTION__, __LINE__);

            if (0x86 == buf[6 + 6 + tagLen] && 0xdd == buf[6 + 6 + tagLen + 1])
            {
                ipType = MULTICAST_TYPE_IPV6;
            }
        }

        if (MULTICAST_TYPE_IPV4 == ipType)
        {
            ret = mcast_igmp_rx(&Pkt, NULL, &recvTime);
        }
        else if (MULTICAST_TYPE_IPV6 == ipType)
        {
            ret = mcast_mld_rx(&Pkt, NULL, &recvTime);
        }

    }
	return ret;
}

int32 mcast_init(void)
{
    uint32 mask, i;
    //sys_vlan_event_t vlanEvent;

    /* init mcast profile db */
    pp_profile_db = (sys_igmp_profile_t **)osal_alloc(sizeof(sys_igmp_profile_t *) * MULTICAST_TYPE_END);
    SYS_ERR_CHK((!pp_profile_db), SYS_ERR_FAILED);

    for (i = 0; i < MULTICAST_TYPE_END; i++)
    {
        pp_profile_db[i] = NULL;

        if (!(pp_profile_db[i] = (sys_igmp_profile_t *)osal_alloc(sizeof(sys_igmp_profile_t) * (igmpCtrl.sys_max_port_num - 1))))
            goto alloc_failed;
    }

	// set ip-mcast-mode for l2-table
    SYS_ERR_CHK((SYS_ERR_OK != rsd_mcast_lookup_mode_set(p_igmp_stats->lookup_mode)), SYS_ERR_FAILED);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_mld_ctrl_init(igmpCtrl.ponMode)), SYS_ERR_FAILED);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_db_init()), SYS_ERR_FAILED);

    /* TBD : handle sem in proto */
    igmp_sem_pkt = osal_sem_mutex_create();
    igmp_sem_db = osal_sem_mutex_create();

    SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_snooping_stats_init(1)), SYS_ERR_FAILED);

    SYS_ERR_CHK((SYS_ERR_OK != mcast_mld_snooping_stats_init(1)), SYS_ERR_FAILED);

	//set drop when igmp trap to cpu only dmac + vid
	SYS_ERR_CHK((SYS_ERR_OK != mcast_unknMcastAction_set(gUnknown_mcast_action)), SYS_ERR_FAILED);

    osal_memset(&groupSave, 0, sizeof(igmp_static_group_t));

    SYS_ERR_CHK((SYS_ERR_OK != igmp_util_ipAddr_get("eth0", &igmpCtrl.sys_ip, &mask)), SYS_ERR_FAILED);
    SYS_ERR_CHK((SYS_ERR_OK != igmp_util_mac_addr_get("eth0", &igmpCtrl.sys_mac)), SYS_ERR_FAILED);

    /*
    vlanEvent.vid = DFLT_VLAN_VID;
    vlanEvent.eventType = VLAN_EVENT_CREATE;
    _mcast_vlan_handler((char*)&vlanEvent);
    */

	LIST_INIT(&portInfoHead);
    LIST_INIT(&previewTimerHead);

    SYS_ERR_CHK((SYS_ERR_OK != igmp_mld_cfg_msg_task_init()), SYS_ERR_FAILED);

    SYS_ERR_CHK((SYS_ERR_OK != igmp_mld_preview_timer_mgr_init()), SYS_ERR_FAILED);

	/* TBD:
	    pf_switch_version_get(&chipId, &rev, &subType);
	    if 9601b asic is accurate for us igmp rate limiting, it need to check chip id
	*/
	SYS_ERR_CHK((SYS_ERR_OK != igmp_mld_rate_limit_timer_init()), SYS_ERR_FAILED);

    SYS_ERR_CHK((SYS_ERR_OK != igmp_mld_create_pkt_dev()), SYS_ERR_FAILED);

    return SYS_ERR_OK;
alloc_failed:
    for (i = 0; i < MULTICAST_TYPE_END; i++)
    {
        if (pp_profile_db[i])
            osal_free(pp_profile_db[i]);
    }
    if (pp_profile_db)
        osal_free(pp_profile_db);
    return SYS_ERR_FAILED;
}

int32 mcast_exit(void)
{
    osal_sem_mutex_destroy(igmp_sem_pkt);
    osal_sem_mutex_destroy(igmp_sem_db);

    // SYS_ERR_CHK(sys_notifier_observer_unregister(SYS_NOTIFIER_SUBJECT_VLAN, _mcast_vlan_handler));
    return SYS_ERR_OK;
}

int32 mcast_igmp_mld_statistics_init(void)
{
    uint32 idx;
    /* igmp */
    p_igmp_stats = (igmp_stats_t *)osal_alloc(sizeof(igmp_stats_t));

    SYS_ERR_CHK((!p_igmp_stats), SYS_ERR_FAILED);

    p_igmp_stats->p_port_info = (mcast_port_stats_t *)osal_alloc(sizeof(mcast_port_stats_t) * igmpCtrl.sys_max_port_num);

    SYS_ERR_CHK((!p_igmp_stats->p_port_info), SYS_ERR_FAILED);

    p_igmp_stats->p_port_entry_limit = (uint32 *)osal_alloc(sizeof(uint32) * igmpCtrl.sys_max_port_num);

    SYS_ERR_CHK((!p_igmp_stats->p_port_entry_limit), SYS_ERR_FAILED);

    p_igmp_stats->p_max_groups_act = (uint32 *)osal_alloc(sizeof(uint32) * igmpCtrl.sys_max_port_num);

    SYS_ERR_CHK((!p_igmp_stats->p_max_groups_act), SYS_ERR_FAILED);

    /* mld */

    p_mld_stats = (mld_stats_t *)osal_alloc(sizeof(mld_stats_t));

    SYS_ERR_CHK((!p_mld_stats), SYS_ERR_FAILED);

    p_mld_stats->p_port_entry_limit = (uint32 *)osal_alloc(sizeof(uint32) * igmpCtrl.sys_max_port_num);

    SYS_ERR_CHK((!p_mld_stats->p_port_entry_limit), SYS_ERR_FAILED);

    p_mld_stats->p_max_groups_act = (uint32 *)osal_alloc(sizeof(uint32) * igmpCtrl.sys_max_port_num);

    SYS_ERR_CHK((!p_mld_stats->p_max_groups_act), SYS_ERR_FAILED);

    for (idx = 0; idx < igmpCtrl.sys_max_port_num; idx++)
    {
        /* igmp */
        osal_memset(&(p_igmp_stats->p_port_info[idx]), 0, sizeof(mcast_port_stats_t));
        LIST_INIT(&p_igmp_stats->p_port_info[idx].ipv4ActiveGrpTblHead);
        LIST_INIT(&p_igmp_stats->p_port_info[idx].ipv6ActiveGrpTblHead);
        p_igmp_stats->p_port_entry_limit[idx] = SYS_IGMP_PORT_LIMIT_ENTRY_NUM;
        p_igmp_stats->p_max_groups_act[idx] = DFLT_IGMP_MAX_GROUP_ACT;
        /* mld */
        p_mld_stats->p_port_entry_limit[idx] = SYS_MLD_PORT_LIMIT_ENTRY_NUM;
        p_mld_stats->p_max_groups_act[idx] = DFLT_MLD_MAX_GROUP_ACT;
    }

    return SYS_ERR_OK;
}
