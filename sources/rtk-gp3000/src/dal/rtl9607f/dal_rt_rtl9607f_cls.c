/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of classify API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) classify
 */

#include <common/rt_type.h>
#include <rtk/rt/rt_cls.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_cls.h>
#include <dal/rtl9607f/dal_rtl9607f_qos.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>
#include <aal_l2_classification.h>
#include <aal_l2_specpkt.h>
#include <aal_l2_vlan.h>
#include <aal_mcast.h>
#include <aal_port.h>
#include <aal_arb.h>



#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <osal/print.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "osal_common.h"
#include "osal_spinlock.h"
#endif

#define RTL9607F_CLS_RULE_DEFAULT_RESERVED_NUM 48
#define RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM 128
#define RTL9607F_CLS_RULE_NOT_IN_USED -1
#define RTL9607F_CLS_RULE_PORT_NOT_USED -1
#define RTL9607F_CLS_RULE_PORT_IGNORE -2
#define RTL9607F_CLS_DEFAULT_CTAG_TPID 0x8100
#define RTL9607F_CLS_DEFAULT_STAG_TPID 0x88a8
#define RTL9607F_CLS_XLATEVID_NOT_USED 0xFFFFFFFE
#define RTL9607F_CLS_XLATEVID_ONLY_ONE 0x00000001
#define RTL9607F_CLS_XLATEVID_TWO_MORE 0x00000002
#define RTL9607F_CLS_XLATEVID_UNTAG    0xFFFFFFFD
#define RTL9607F_CLS_XLATEVID_TRANSPATENT 0xFFFFFFFC

#define RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE 100
#define RTL9607F_CLS_DSCP_PORT_INDEX_BASE 100

#define DEFAULT_DEVICE_ID 0
#define RTL9607F_CLS_SID_MAX 256
#define RTL9607F_CLS_PORT_TYPE_PPTP 1
#define RTL9607F_CLS_PORT_TYPE_VEIP 0



typedef struct dal_9607f_cls_rule_index_s
{
    int32 index;
    rt_cls_dir_t dir;
    uint32 ruleIndex;
    uint32 rulePriority;
    int32 port;
}dal_9607f_cls_rule_index_t;

typedef struct {
    rt_cls_rule_t rule;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} dal_9607f_cls_rule_db_t;

typedef struct {
    uint32 ruleIndex;
    ca_uint16_t swMcgid;
    ca_uint16_t hwMcgid;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} dal_9607f_cls_rule_mcgid_t;


typedef struct {
    uint32 index;
    uint32 type;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} dal_9607f_cls_vlan_rule_index_t;

typedef struct
{
    unsigned int vid;
    unsigned int xlateMode;
} dal_9607f_cls_vlan_xlate_info_t;


uint8 doubleCtag=0;
int32 portRuleIndex[RTK_MAX_NUM_OF_PORTS];
uint8 portType[RTK_MAX_NUM_OF_PORTS] = {0};
uint8 portFwdPort[RTK_MAX_NUM_OF_PORTS] = {0};
ca_uint16_t sw_mcgid=CA_UINT16_INVALID,hw_mcgid=CA_UINT16_INVALID;
uint32 cls_init = INIT_NOT_COMPLETED;
uint16 currentTpid = 0;


dal_9607f_cls_rule_index_t *clsRuleUsage;
#if defined(CONFIG_SDK_KERNEL_LINUX)
struct list_head dsRuleHead;
struct list_head usRuleHead;
struct list_head vlanRuleIndexHead;
struct list_head sp2cVlanHead;
struct list_head mcgidHead;
ca_uint __dal_cf_us_db_lock = 0;
ca_uint __dal_cf_ds_db_lock = 0;
#define __CF_DS_DB_LOCK()	ca_spin_lock(__dal_cf_ds_db_lock)
#define __CF_DS_DB_UNLOCK()	ca_spin_unlock(__dal_cf_ds_db_lock)
#define __CF_US_DB_LOCK()	ca_spin_lock(__dal_cf_us_db_lock)
#define __CF_US_DB_UNLOCK()	ca_spin_unlock(__dal_cf_us_db_lock)
#else
#define __CF_DS_DB_LOCK()
#define __CF_DS_DB_UNLOCK()
#define __CF_US_DB_LOCK()
#define __CF_US_DB_UNLOCK()
#endif

uint16 ethertypeTable[AAL_L2_SPECPKT_ETHTYPE_ENTRY_ID_MAX];
rt_ponmisc_sidInfo_t clsSidInfo[RTL9607F_CLS_SID_MAX];

#if defined(CONFIG_SDK_RTL8198X)
extern int32 dal_rtl9607f_ca_specpkt_ctrl_ldpid_update (rtk_port_t port);
#endif

static void _9607f_cls_rule_adjust(void);
static int32 _9607f_cls_rule_set(rt_cls_rule_t *pClsRule);



static int32 _9607f_cls_ethertype_encode_get(uint16 ethertype,uint8 *encode)
{
    int i;
    aal_l2_specpkt_ethtype_encode_mask_t mask;
    aal_l2_specpkt_ethtype_encode_t config;

    for(i=0;i<AAL_L2_SPECPKT_ETHTYPE_ENTRY_ID_MAX;i++)
    {
        if(ethertypeTable[i] == ethertype)
        {
            *encode = i;
            return RT_ERR_OK;
        }
    }
    /*add new ethertype table */
    for(i=0;i<AAL_L2_SPECPKT_ETHTYPE_ENTRY_ID_MAX;i++)
    {
        if(ethertypeTable[i] == 0)
        {
            mask.s.ethertype = 1;
            mask.s.encoded_value = 1;
            config.ethertype = ethertype;
            config.encoded_value = i;
            if(CA_E_OK != aal_l2_specpkt_ethtype_encode_set(DEFAULT_DEVICE_ID,i,mask,&config) )
            {
                printk(KERN_ERR"Set ethertype fail, ethertype 0x%04x, entry id %d\n",ethertype, i);
                return RT_ERR_FAILED;
            }
            ethertypeTable[i] = ethertype;
            *encode = i;
            return RT_ERR_OK;
        }
    }
    printk(KERN_ERR"Can't find available ethertype table!!\n");
    return RT_ERR_FAILED;
}

static int32 _9607f_cls_rule_index_get(uint32 ruleIndex, rt_cls_dir_t dir,int *index, int32 port,uint32 rulePriority, int isReversedRule)
{
    int i=0,j=0,k,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    dal_9607f_cls_rule_index_t writeData, tempData;
    aal_l2_cf_rule_t entry;
    ca_status_t ret;
    memset(&writeData,0,sizeof(dal_9607f_cls_rule_index_t));
    memset(&tempData,0,sizeof(dal_9607f_cls_rule_index_t));

    if(NULL == clsRuleUsage)
        return RT_ERR_FAILED;

    if(isReversedRule)
    {
        min = RTL9607F_CLS_RULE_DEFAULT_RESERVED_NUM;
        max = RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM;
    }
    else
    {
        min = RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM;
        max = ruleMax;
    }

    for(i=min;i<max;i++)
    {
        if((RTL9607F_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == ruleIndex) && (clsRuleUsage[i].dir == dir) && (port == clsRuleUsage[i].port))
        {
            *index=clsRuleUsage[i].index;
            return RT_ERR_OK;
        }
    }
    /*Not exist, find an empty index*/
    for(i=min;i<max;i++)
    {
        /*High priority rule, insert the rule before low priority rule*/
        if(RTL9607F_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index && clsRuleUsage[i].rulePriority < rulePriority)
        {
            writeData.dir = dir;
            writeData.ruleIndex = ruleIndex;
            writeData.rulePriority = rulePriority;
            writeData.port = port;
            *index = i;
            for(j=i;j<max;j++)
            {
                if(RTL9607F_CLS_RULE_NOT_IN_USED == clsRuleUsage[j].index )
                {
                    clsRuleUsage[j].index = j;
                    clsRuleUsage[j].ruleIndex=writeData.ruleIndex;
                    clsRuleUsage[j].rulePriority = writeData.rulePriority;
                    clsRuleUsage[j].dir=writeData.dir;
                    clsRuleUsage[j].port=writeData.port;
                    break;
                }
                else
                {
                    tempData.dir = clsRuleUsage[j].dir;
                    tempData.ruleIndex = clsRuleUsage[j].ruleIndex;
                    tempData.rulePriority = clsRuleUsage[j].rulePriority;
                    tempData.port = clsRuleUsage[j].port;

                    clsRuleUsage[j].index = j;
                    clsRuleUsage[j].ruleIndex=writeData.ruleIndex;
                    clsRuleUsage[j].rulePriority = writeData.rulePriority;
                    clsRuleUsage[j].dir=writeData.dir;
                    clsRuleUsage[j].port=writeData.port;

                    memcpy(&writeData,&tempData,sizeof(dal_9607f_cls_rule_index_t));
                }
            }
            if(!isReversedRule)
            {
                _9607f_cls_rule_adjust();
            }
            else
            {
                for(k = j ; k > i; k--)
                {
                    memset(&entry, 0, sizeof(aal_l2_cf_rule_t));
                    ret = aal_l2_cf_rule_get(DEFAULT_DEVICE_ID, k-1, &entry);
                    if(ret != CA_E_OK)
                        return RT_ERR_FAILED;
                    ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, k, &entry);
                    if(ret != CA_E_OK)
                        return RT_ERR_FAILED;
                }
            }
            if(j == max)
               return RT_ERR_FAILED;
            else
                return RT_ERR_OK;
        }
        if(RTL9607F_CLS_RULE_NOT_IN_USED == clsRuleUsage[i].index)
        {
            clsRuleUsage[i].index = i;
            clsRuleUsage[i].ruleIndex=ruleIndex;
            clsRuleUsage[i].rulePriority=rulePriority;
            clsRuleUsage[i].dir=dir;
            clsRuleUsage[i].port=port;
            *index=clsRuleUsage[i].index;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}

static int32 _9607f_cls_rule_index_find(uint32 ruleIndex, rt_cls_dir_t dir,int *index, int32 port,int isReversedRule)
{
    int i=0,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();

    if(NULL == clsRuleUsage)
        return RT_ERR_FAILED;

    if(isReversedRule)
    {
        min = RTL9607F_CLS_RULE_DEFAULT_RESERVED_NUM;
        max = RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM;
    }
    else
    {
        min = RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM;
        max = ruleMax;
    }
    for(i=min;i<max;i++)
    {
        if((RTL9607F_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == ruleIndex) && (clsRuleUsage[i].dir == dir) )
        {
            if(port == RTL9607F_CLS_RULE_PORT_IGNORE || port == clsRuleUsage[i].port)
            {
                *index=clsRuleUsage[i].index;
                return RT_ERR_OK;
            }
        }
    }
    return RT_ERR_FAILED;
}

static int32 _9607f_cls_rule_index_del(uint32 index)
{
    int i=0,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();

    if(NULL == clsRuleUsage)
        return RT_ERR_FAILED;

    if(index >= ruleMax)
        return RT_ERR_FAILED;


    clsRuleUsage[index].index = RTL9607F_CLS_RULE_NOT_IN_USED;
    clsRuleUsage[index].port=RTL9607F_CLS_RULE_PORT_NOT_USED;
    clsRuleUsage[index].ruleIndex = 0;
    clsRuleUsage[index].rulePriority = 0;
    return RT_ERR_OK;

}

static void _9607f_cls_tpid_set(uint16 tpid)
{
    aal_l2_vlan_default_cfg_mask_t defaultCfgMask;
    aal_l2_vlan_default_cfg_t defaultCfg;

    if(tpid == currentTpid)
        return;
    if(currentTpid != 0)
        printk("Need multiple user S tpid!! current:0x%04x, new: 0x%04x\n", currentTpid, tpid);
    currentTpid = tpid;
    memset(&defaultCfg,0,sizeof(aal_l2_vlan_default_cfg_t));
    memset(&defaultCfgMask,0,sizeof(aal_l2_vlan_default_cfg_mask_t));
    defaultCfg.tx_tpid1 = currentTpid;
    defaultCfgMask.s.tx_tpid1 = 1;
    aal_l2_vlan_default_cfg_set(DEFAULT_DEVICE_ID,defaultCfgMask,&defaultCfg);
}

static void _9607f_cls_rule_adjust(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    dal_9607f_cls_rule_db_t *pEntryData= NULL;

    __CF_DS_DB_LOCK();
    list_for_each_safe(pEntry, pTmpEntry, &dsRuleHead)
    {
        pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
        _9607f_cls_rule_set(&(pEntryData->rule));
    }
    __CF_DS_DB_UNLOCK();

    __CF_US_DB_LOCK();
    list_for_each_safe(pEntry, pTmpEntry, &usRuleHead)
    {
        pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
        _9607f_cls_rule_set(&(pEntryData->rule));
    }
    __CF_US_DB_UNLOCK();
#endif
}

static int32 _9607f_cls_action_set(aal_l2_cf_rule_t *pEntry, rt_cls_rule_t *pClsRule)
{
    int8 isCsTag = 0;
    int8 filterTag = pClsRule->filterRule.outerTagIf+pClsRule->filterRule.innerTagIf;
    int8 processTag = filterTag;
    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_TRANSPARENT;
    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_TRANSPARENT;
    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;

    if(pClsRule->direction == RT_CLS_DIR_US)
    {
        if(clsSidInfo[pClsRule->flowId].enFlag == DISABLED)
        {
            printk("Unknown us flow id %d, add l2_cls rule fail\n",pClsRule->flowId);
            return RT_ERR_FAILED;
        }
        pEntry->action.flowid_act = AAL_L2_CF_ACTION_FLOW_ID_ACT_ASSIGN;
        pEntry->action.cf_flowid = clsSidInfo[pClsRule->flowId].flowId;
        pEntry->action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
        pEntry->action.cf_cos = clsSidInfo[pClsRule->flowId].pri;
        pEntry->action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
        pEntry->action.cf_fwd_id = clsSidInfo[pClsRule->flowId].dstPort;
    }


    if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
    {
        pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_ADD_CTAG;
        pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_VID_BIT)
        {
            pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_ASSIGN;
            pEntry->action.cvid = pClsRule->filterRule.innerTagVid;
        }
        else
        {
            pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
        }

        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT)
        {
            pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_ASSIGN;
            pEntry->action.cpri = pClsRule->filterRule.innerTagPri;
        }
        else
        {
            pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
        }
    }

    switch(pClsRule->outerTagAct.tagAction)
    {
        case RT_CLS_TAG_TRANSPARENT:
            break;
        case RT_CLS_TAG_MODIFY:
            processTag--;
            /*fall through*/
        case RT_CLS_TAG_TAGGING:
            isCsTag=1;
            if(pClsRule->outerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_STAG_TPID)
            {
                pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_STAG_TPID;
            }
            else
            {
                pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_TPID_B;
                _9607f_cls_tpid_set(pClsRule->outerTagAct.assignedTpid);
            }
            break;
        case RT_CLS_TAG_REMOVE:
            processTag--;
            if(pClsRule->filterRule.outerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RTL9607F_CLS_DEFAULT_CTAG_TPID)
            {
                pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_DELETE_CTAG;
            }
            else
            {
                isCsTag=1;
                pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
            }
            break;
        default:
            printk(KERN_ERR"[%s] %d Unknown outer tag action %d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, pClsRule->outerTagAct.tagAction, pClsRule->index, pClsRule->direction);
            break;
    }

    if(pClsRule->outerTagAct.tagAction != RT_CLS_TAG_TRANSPARENT && pClsRule->outerTagAct.tagAction != RT_CLS_TAG_REMOVE)
    {
        switch(pClsRule->outerTagAct.tagVidAction)
        {
            case RT_CLS_VID_ASSIGN:
                if(isCsTag)
                {
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_ASSIGN;
                    pEntry->action.cs_vid = pClsRule->outerTagAct.assignedVid;
                }
                else
                {
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_ASSIGN;
                    pEntry->action.cvid = pClsRule->outerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(isCsTag)
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                break;
            default:
                printk(KERN_ERR"[%s] %d Unknown outer vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->outerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }

        switch(pClsRule->outerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_ASSIGN;
                    pEntry->action.cs_pri = pClsRule->outerTagAct.assignedPri;
                }
                else
                {
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_ASSIGN;
                    pEntry->action.cpri = pClsRule->outerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_DSCP;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_DSCP;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                break;
            default:
                printk(KERN_ERR"[%s] %d Unknown outer priority action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->outerTagAct.tagPriAction, pClsRule->index, pClsRule->direction);
                break;
        }
    }
    isCsTag = 0;
    switch(pClsRule->innerTagAct.tagAction)
    {
        case RT_CLS_TAG_TRANSPARENT:
            break;
        case RT_CLS_TAG_TAGGING:
            if(pClsRule->innerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
            {
                if(pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
                {
                    isCsTag=1;
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_TPID_B;
                    _9607f_cls_tpid_set(pClsRule->innerTagAct.assignedTpid);
                }
                else
                {
                    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_ADD_CTAG;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_STAG_TPID;
                }
                else
                {
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_TPID_B;
                    _9607f_cls_tpid_set(pClsRule->innerTagAct.assignedTpid);
                }
            }
            break;
        case RT_CLS_TAG_MODIFY:
            if(pClsRule->innerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
            {
                if(processTag == 2)
                {
                    isCsTag=1;
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_TPID_B;
                    _9607f_cls_tpid_set(pClsRule->innerTagAct.assignedTpid);
                }
                else
                {
                    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_ADD_CTAG;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_STAG_TPID;
                }
                else
                {
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_ADD_TAG_TPID_B;
                    _9607f_cls_tpid_set(pClsRule->innerTagAct.assignedTpid);
                }
            }
            if(processTag == 1 && pClsRule->innerTagAct.assignedTpid != pClsRule->filterRule.innerTagTpid && (pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT || pClsRule->outerTagAct.tagAction == RT_CLS_TAG_REMOVE))
            {
                if(pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
                    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_DELETE_CTAG;
                else
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
            }
            if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID && pClsRule->innerTagAct.assignedTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
            break;
        case RT_CLS_TAG_REMOVE:
            if(processTag == 1)
            {
                if(pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
                {
                    if(doubleCtag == 1 && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                        pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
                    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_DELETE_CTAG;
                }
                else
                {
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
                }
            }
            else
            {
                if(pClsRule->filterRule.outerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RTL9607F_CLS_DEFAULT_CTAG_TPID)
                    pEntry->action.cact = AAL_L2_CF_ACTION_C_ACT_DELETE_CTAG;
                else
                    pEntry->action.cs_act = AAL_L2_CF_ACTION_CS_ACT_DELETE_TAG;
            }
            break;
        default:
            printk(KERN_ERR"[%s] %d Unknown inner tag action %d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagAction, pClsRule->index, pClsRule->direction);
            break;
    }

    if(pClsRule->innerTagAct.tagAction != RT_CLS_TAG_TRANSPARENT && pClsRule->innerTagAct.tagAction != RT_CLS_TAG_REMOVE)
    {
        switch(pClsRule->innerTagAct.tagVidAction)
        {
            case RT_CLS_VID_ASSIGN:
                if(isCsTag)
                {
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_ASSIGN;
                    pEntry->action.cs_vid = pClsRule->innerTagAct.assignedVid;
                }
                else
                {
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_ASSIGN;
                    pEntry->action.cvid = pClsRule->innerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                    {
                        pEntry->action.cs_vid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        pEntry->action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_1ST_TAG;
                    }
                }
                break;
            default:
                printk(KERN_ERR"[%s] %d Unknown inner vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }

        switch(pClsRule->innerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_ASSIGN;
                    pEntry->action.cs_pri = pClsRule->innerTagAct.assignedPri;
                }
                else
                {
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_ASSIGN;
                    pEntry->action.cpri = pClsRule->innerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_DSCP;
                else
                    pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_DSCP;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                    {
                        pEntry->action.cs_pri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        pEntry->action.cpri_act = AAL_L2_CF_ACTION_PRI_ACT_FROM_1ST_TAG;
                    }
                }
                break;
            default:
                printk(KERN_ERR"[%s] %d Unknown inner priority action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagPriAction, pClsRule->index, pClsRule->direction);
                break;
        }
    }


    if(pClsRule->direction == RT_CLS_DIR_DS && pClsRule->dsPriAct == RT_CLS_DS_PRI_ACT_ASSIGN && pClsRule->dsPri <= 7)
    {
        pEntry->action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
        pEntry->action.cf_cos = pClsRule->dsPri;;
    }
    return RT_ERR_OK;
}

static int32 _9607f_cls_filter_set(aal_l2_cf_rule_t *pEntry, rt_cls_rule_t *pClsRule)
{
    int16 innerVlan=0;
    int16 innerVlanMask=0;
    int ctag=0, stag=0, ctagValue = 0,stagValue = 0;
    uint8 ethertypeEncode = 0;
    int32 ret=RT_ERR_FAILED;
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;
    uint8 portIndex;
    aal_l2_vlan_default_cfg_mask_t defaultCfgMask;
    aal_l2_vlan_default_cfg_t defaultCfg;

    pEntry->key.data_bits.valid = ENABLED;
    pEntry->key.care_bits.valid = ENABLED;


    if(pClsRule->direction == RT_CLS_DIR_US)
        pEntry->key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_UPSTREAM;
    else
        pEntry->key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_DOWNSTREAM;
    pEntry->key.care_bits.direction = 1;

    /*filter*/
    if(RT_CLS_FILTER_OUTER_TAGIf_BIT & pClsRule->filterRule.filterMask && pClsRule->filterRule.outerTagIf == 1)
    {
        if(pClsRule->filterRule.outerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RTL9607F_CLS_DEFAULT_CTAG_TPID)
        {
            ctag=1;
        }
        else
        {
            stag=1;
            /*change default stag tpid*/

            if(pClsRule->filterRule.outerTagTpid != RTL9607F_CLS_DEFAULT_STAG_TPID && pClsRule->filterRule.outerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
            {
                memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
                ilpbCfgMask.s.s_tpid_match = 1;
                for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
                {
                    if(pClsRule->ingressPortMask & (1 << portIndex))
                    {
                        memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                        if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,portIndex,&ilpbCfg))
                        {
                            ilpbCfg.s_tpid_match |= 1;
                            aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,portIndex,ilpbCfgMask,&ilpbCfg);
                        }
                    }
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 1;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),ilpbCfgMask,&ilpbCfg);
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 1;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,ilpbCfgMask,&ilpbCfg);
                }

                memset(&defaultCfg,0,sizeof(aal_l2_vlan_default_cfg_t));
                memset(&defaultCfgMask,0,sizeof(aal_l2_vlan_default_cfg_mask_t));
                defaultCfg.cmp_tpid_user = pClsRule->filterRule.outerTagTpid;
                defaultCfgMask.s.cmp_tpid_user = 1;

                aal_l2_vlan_default_cfg_set(DEFAULT_DEVICE_ID,defaultCfgMask,&defaultCfg);


            }

            if(doubleCtag == 0 && pClsRule->filterRule.outerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
            {
                doubleCtag = 1;
                /*enable stag with 0x8100*/
                memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
                ilpbCfgMask.s.s_tpid_match = 1;
                for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
                {
                    if(pClsRule->ingressPortMask & (1 << portIndex))
                    {
                        memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                        if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,portIndex,&ilpbCfg))
                        {
                            ilpbCfg.s_tpid_match |= 8;
                            aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,portIndex,ilpbCfgMask,&ilpbCfg);
                        }
                    }
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 8;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),ilpbCfgMask,&ilpbCfg);
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 8;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,ilpbCfgMask,&ilpbCfg);
                }
                _9607f_cls_rule_adjust();
            }
        }
    }

    if(RT_CLS_FILTER_INNER_TAGIf_BIT & pClsRule->filterRule.filterMask && pClsRule->filterRule.innerTagIf == 1)
    {
        if(pClsRule->filterRule.innerTagTpid == RTL9607F_CLS_DEFAULT_CTAG_TPID)
        {
            ctag=1;
        }
        else
        {
            stag=1;
            /*change default stag tpid*/
            if(pClsRule->filterRule.innerTagTpid != RTL9607F_CLS_DEFAULT_STAG_TPID)
            {
                memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
                ilpbCfgMask.s.s_tpid_match = 1;
                for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
                {
                    if(pClsRule->ingressPortMask & (1 << portIndex))
                    {
                        memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                        if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,portIndex,&ilpbCfg))
                        {
                            ilpbCfg.s_tpid_match |= 1;
                            aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,portIndex,ilpbCfgMask,&ilpbCfg);
                        }
                    }
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 1;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),ilpbCfgMask,&ilpbCfg);
                }

                memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
                if(CA_E_OK == aal_port_ilpb_cfg_get(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,&ilpbCfg))
                {
                    ilpbCfg.s_tpid_match |= 1;
                    aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,RTL9607F_CLS_EXT_WAN_PORT,ilpbCfgMask,&ilpbCfg);
                }

                memset(&defaultCfg,0,sizeof(aal_l2_vlan_default_cfg_t));
                memset(&defaultCfgMask,0,sizeof(aal_l2_vlan_default_cfg_mask_t));
                defaultCfg.cmp_tpid_user = pClsRule->filterRule.outerTagTpid;
                defaultCfgMask.s.cmp_tpid_user = 1;

                aal_l2_vlan_default_cfg_set(DEFAULT_DEVICE_ID,defaultCfgMask,&defaultCfg);


            }
        }
    }

    if(RT_CLS_FILTER_OUTER_TAGIf_BIT & pClsRule->filterRule.filterMask || RT_CLS_FILTER_INNER_TAGIf_BIT & pClsRule->filterRule.filterMask)
    {
        if(doubleCtag == 1 && ctag == 1 && stag == 0)
        {
            ctagValue = 0;
            stagValue = 1;
        }
        else
        {
            ctagValue = ctag;
            stagValue = stag;
        }

        pEntry->key.data_bits.ctag = ctagValue;
        pEntry->key.care_bits.ctag = 1;

        pEntry->key.data_bits.stag = stagValue;
        pEntry->key.care_bits.stag = 1;


    }

    if(RT_CLS_FILTER_FLOWID_BIT & pClsRule->filterRule.filterMask)
    {
        pEntry->key.data_bits.pol_id = pClsRule->filterRule.flowId;
        pEntry->key.care_bits.pol_id = 0x1ff;
    }

    if(RT_CLS_FILTER_OUTER_VID_BIT & pClsRule->filterRule.filterMask)
    {
        pEntry->key.data_bits.outer_vid = pClsRule->filterRule.outerTagVid;
        pEntry->key.care_bits.outer_vid = 0xfff;
    }

    if(RT_CLS_FILTER_OUTER_PRI_BIT & pClsRule->filterRule.filterMask)
    {
        pEntry->key.data_bits.outer_pri = pClsRule->filterRule.outerTagPri;
        pEntry->key.care_bits.outer_pri = 7;
    }

    if(RT_CLS_FILTER_OUTER_DEI_BIT & pClsRule->filterRule.filterMask)
    {
        pEntry->key.data_bits.outer_dei_cfi = pClsRule->filterRule.outerTagDei;
        pEntry->key.care_bits.outer_dei_cfi = 1;
    }

    if(RT_CLS_FILTER_INNER_VID_BIT & pClsRule->filterRule.filterMask)
    {

        if(pClsRule->filterRule.outerTagIf == 1)
        {
            pEntry->key.data_bits.ctag_vid = pClsRule->filterRule.innerTagVid;
            pEntry->key.care_bits.ctag_vid = 0xfff;
        }
        else
        {
            pEntry->key.data_bits.outer_vid = pClsRule->filterRule.innerTagVid;
            pEntry->key.care_bits.outer_vid = 0xfff;
        }
    }

    if(RT_CLS_FILTER_INNER_PRI_BIT & pClsRule->filterRule.filterMask)
    {

        if(pClsRule->filterRule.outerTagIf == 1)
        {
            pEntry->key.data_bits.ctag_pri = pClsRule->filterRule.innerTagPri;
            pEntry->key.care_bits.ctag_pri = 7;
        }
        else
        {
            pEntry->key.data_bits.outer_pri = pClsRule->filterRule.innerTagPri;
            pEntry->key.care_bits.outer_pri = 7;
        }
    }

    if(RT_CLS_FILTER_INNER_DEI_BIT & pClsRule->filterRule.filterMask)
    {

        if(pClsRule->filterRule.outerTagIf == 1)
        {
            pEntry->key.data_bits.ctag_cfi = pClsRule->filterRule.innerTagDei;
            pEntry->key.care_bits.ctag_cfi = 1;
        }
        else
        {
            pEntry->key.data_bits.outer_dei_cfi = pClsRule->filterRule.innerTagDei;
            pEntry->key.care_bits.outer_dei_cfi = 1;
        }
    }

    if(RT_CLS_FILTER_ETHERTYPE & pClsRule->filterRule.filterMask)
    {
        if(RT_ERR_OK != _9607f_cls_ethertype_encode_get(pClsRule->filterRule.etherType, &ethertypeEncode))
        {
            printk(KERN_ERR"Can't get ethertype encode value, ethertype 0x%04x\n",pClsRule->filterRule.etherType);
            return RT_ERR_FAILED;
        }
        pEntry->key.data_bits.ev2pt_table = ethertypeEncode;
        pEntry->key.care_bits.ev2pt_table = 0x1f;
    }

    return RT_ERR_OK;
}

static void _9607f_cls_vlan_aggregate_cf_delAll(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32 index;
    struct list_head *cur_next = NULL;
    struct list_head *cur_tmp = NULL;
    dal_9607f_cls_vlan_rule_index_t *cur  = NULL;
    int32 ret=RT_ERR_FAILED;
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    dal_9607f_cls_rule_mcgid_t *pMcgidEntryData= NULL;

    list_for_each_safe(cur_next, cur_tmp, &vlanRuleIndexHead)
    {
        cur = list_entry(cur_next, dal_9607f_cls_vlan_rule_index_t, list);

        while(_9607f_cls_rule_index_find(cur->index+RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE, RT_CLS_DIR_DS, &index,RTL9607F_CLS_RULE_PORT_IGNORE, 1) == RT_ERR_OK)
        {
            ret = aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, index);
            if(ret != CA_E_OK)
            {
                    printk("[%s] %d Delete classify rule fail ret:%d index:%d "
                        ,__FUNCTION__, __LINE__, ret, index);
            }

            ret = _9607f_cls_rule_index_del(index);
            if(ret != RT_ERR_OK)
            {
                    printk("[%s] %d Delete vlan aggregate cls rule index fail ret:%d ruleIndex:%d"
                        ,__FUNCTION__, __LINE__, ret,cur->index);
            }
        }

        list_for_each_safe(pEntry, pTmpEntry, &mcgidHead)
        {
            pMcgidEntryData = list_entry(pEntry, dal_9607f_cls_rule_mcgid_t, list);
            if (pMcgidEntryData->ruleIndex == cur->index+RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE)
            {
                aal_hw_mcgid_free(pMcgidEntryData->swMcgid,pMcgidEntryData->hwMcgid);
                aal_sw_mcgid_free(pMcgidEntryData->swMcgid);
                list_del(&pMcgidEntryData->list);
                kfree(pMcgidEntryData);
                pMcgidEntryData = NULL;
                break;
            }
        }

        list_del(&cur->list);
        kfree(cur);
    }
#endif
}

static void _9607f_cls_vlan_aggregate_fill_outerVID(unsigned int *pOuterVID, rt_cls_rule_t *pDsVlanRule)
{
    if ((pOuterVID== NULL) || (pDsVlanRule == NULL))
        return;

    if(pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_VID_BIT)
    {
        *pOuterVID = pDsVlanRule->filterRule.outerTagVid;
    }
    else if(pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_VID_BIT)
    {
        *pOuterVID = pDsVlanRule->filterRule.innerTagVid;
    }
    else if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) &&
            (pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) &&
            pDsVlanRule->filterRule.outerTagIf == 0 &&
            pDsVlanRule->filterRule.innerTagIf == 0)
    {
        *pOuterVID = RTL9607F_CLS_XLATEVID_UNTAG;
    }
    else
    {
        *pOuterVID = RTL9607F_CLS_XLATEVID_NOT_USED;
    }
}

static void _9607f_cls_vlan_aggregate_fill_xlateVID(dal_9607f_cls_vlan_xlate_info_t *pXlateVID,
                                              rt_cls_rule_t *pDsVlanRule,
                                              unsigned int outerVID,
                                              unsigned int uniMask)
{
    uint32 xlate_cvid = RTL9607F_CLS_XLATEVID_UNTAG;
    uint32 port;

    if ((pXlateVID == NULL) || (pDsVlanRule == NULL))
        return;

    if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) && pDsVlanRule->filterRule.outerTagIf == 1)
    {
        if(pDsVlanRule->outerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
        }
        else if(pDsVlanRule->outerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->outerTagAct.assignedVid;
        }
        else
        {
            if(outerVID == RTL9607F_CLS_XLATEVID_NOT_USED)
                xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
            else
                xlate_cvid = outerVID;
        }
    }
    else if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) && pDsVlanRule->filterRule.innerTagIf == 1)
    {
        if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
        }
        else if(pDsVlanRule->innerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->innerTagAct.assignedVid;
        }
        else
        {
            if(outerVID == RTL9607F_CLS_XLATEVID_NOT_USED)
                xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
            else
                xlate_cvid = outerVID;
        }
    }
    else if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) == 0 && (pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) == 0)
    {
        if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
        }
        else if(pDsVlanRule->innerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->innerTagAct.assignedVid;
        }
        else
        {
            if(outerVID == RTL9607F_CLS_XLATEVID_NOT_USED)
                xlate_cvid = RTL9607F_CLS_XLATEVID_TRANSPATENT;
            else
                xlate_cvid = outerVID;
        }
    }
    else
    {
        xlate_cvid = RTL9607F_CLS_XLATEVID_UNTAG;
    }


    for(port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (uniMask & (0x0001 << port))
        {
            /* One VID, Mode = SP2C, more than one VID, Mode = LUT */
            if (pXlateVID[port].xlateMode == RTL9607F_CLS_XLATEVID_NOT_USED)
            {
                pXlateVID[port].xlateMode = RTL9607F_CLS_XLATEVID_ONLY_ONE;
                pXlateVID[port].vid = xlate_cvid;
            }
            else
            {
                pXlateVID[port].xlateMode = RTL9607F_CLS_XLATEVID_TWO_MORE;

                /* if more than one VID, we record untag first, than smallest VID */
                if ( (xlate_cvid == RTL9607F_CLS_XLATEVID_UNTAG) || (pXlateVID[port].vid > xlate_cvid) )
                    pXlateVID[port].vid = xlate_cvid;
            }
        }
    }
}

static int _9607f_cls_is_same_rule_by_ds_vlan_filter(
                                               rt_cls_rule_t *pBR,
                                               rt_cls_rule_t *pL2)
{
    rt_cls_filter_rule_t tmpFilterRule[2];
    rt_cls_filter_mask_t fwdAllMask = 0xffff - RT_CLS_FILTER_FLOWID_BIT - RT_CLS_FILTER_EGRESS_PORT_BIT;

    memcpy(&tmpFilterRule[0],&(pBR->filterRule),sizeof(rt_cls_filter_rule_t));
    memcpy(&tmpFilterRule[1],&(pL2->filterRule),sizeof(rt_cls_filter_rule_t));
    if((pBR->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) || (pL2->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) )
    {
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
        tmpFilterRule[0].egressPortMask = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
        tmpFilterRule[1].egressPortMask = 0;
    }

    if(memcmp(&tmpFilterRule[0], &tmpFilterRule[1], sizeof(rt_cls_filter_rule_t)) == 0)
        return TRUE;

    /* Forward all */
    if((pBR->filterRule.filterMask & fwdAllMask) == 0 || (pL2->filterRule.filterMask & fwdAllMask) == 0)
    {
        return TRUE;
    }

    if(((pBR->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) == 0 && (pBR->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) == 0)
        || ((pL2->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) == 0 && (pL2->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) == 0))
    {
        /* ignore VLAN tag/TPID filter if one of rule only filter outer VID */
        memcpy(&tmpFilterRule[0],&(pBR->filterRule),sizeof(rt_cls_filter_rule_t));
        memcpy(&tmpFilterRule[1],&(pL2->filterRule),sizeof(rt_cls_filter_rule_t));
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_OUTER_TAGIf_BIT;
        tmpFilterRule[0].outerTagIf = 0;
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_INNER_TAGIf_BIT;
        tmpFilterRule[0].innerTagIf = 0;
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_OUTER_TPID_BIT;
        tmpFilterRule[0].outerTagTpid = 0;
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_INNER_TPID_BIT;
        tmpFilterRule[0].innerTagTpid = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_OUTER_TAGIf_BIT;
        tmpFilterRule[1].outerTagIf = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_INNER_TAGIf_BIT;
        tmpFilterRule[1].innerTagIf = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_OUTER_TPID_BIT;
        tmpFilterRule[1].outerTagTpid = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_INNER_TPID_BIT;
        tmpFilterRule[1].innerTagTpid = 0;
        if((pBR->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) || (pL2->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) )
        {
            tmpFilterRule[0].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
            tmpFilterRule[0].egressPortMask = 0;
            tmpFilterRule[1].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
            tmpFilterRule[1].egressPortMask = 0;
        }
        if(memcmp(&tmpFilterRule[0], &tmpFilterRule[1], sizeof(rt_cls_filter_rule_t)) == 0)
            return TRUE;
    }
    if(((pBR->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT) == 0 && (pL2->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT) != 0)
        || ((pBR->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT) != 0 && (pL2->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT) == 0))
    {
        /* ignore inner pbit filter if one of rule only filter inner pbit */
        memcpy(&tmpFilterRule[0],&(pBR->filterRule),sizeof(rt_cls_filter_rule_t));
        memcpy(&tmpFilterRule[1],&(pL2->filterRule),sizeof(rt_cls_filter_rule_t));
        tmpFilterRule[0].filterMask |= RT_CLS_FILTER_INNER_PRI_BIT;
        tmpFilterRule[0].innerTagPri = 0;
        tmpFilterRule[1].filterMask |= RT_CLS_FILTER_INNER_PRI_BIT;
        tmpFilterRule[1].innerTagPri = 0;
        if((pBR->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) || (pL2->filterRule.filterMask & RT_CLS_FILTER_EGRESS_PORT_BIT) )
        {
            tmpFilterRule[0].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
            tmpFilterRule[0].egressPortMask = 0;
            tmpFilterRule[1].filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
            tmpFilterRule[1].egressPortMask = 0;
        }

        if(memcmp(&tmpFilterRule[0], &tmpFilterRule[1], sizeof(rt_cls_filter_rule_t)) == 0)
            return TRUE;
    }

    return FALSE;
}

static int32 _9607f_cls_vlan_aggregate_cf_set(unsigned int mode, rt_cls_rule_t *pClsRule,int portIndex, dal_9607f_cls_vlan_xlate_info_t *pXlateVID)
{
    aal_l2_cf_rule_t entry;
    int32 index=0, i;
    int32 ret=RT_ERR_FAILED;
    uint32 targetUniMask = 0x0;
    dal_9607f_cls_rule_mcgid_t *pEntryData = NULL;
    portForwardActionVlanExtend_t portAction;

    memset(&entry,0,sizeof(aal_l2_cf_rule_t));
    ret = _9607f_cls_filter_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls filter fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }
    ret = _9607f_cls_action_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls action fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }


    if( mode == RTL9607F_CLS_XLATEVID_TWO_MORE)  /* LUT */
    {
        ret = _9607f_cls_rule_index_get(pClsRule->index+RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE, pClsRule->direction, &index, portIndex, 1, 1);
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
        }

        entry.key.data_bits.ldpid = portIndex;
        entry.key.care_bits.ldpid = 0x3f;

        entry.key.data_bits.pkt_type = AAL_L2_CF_PATTEN_PKT_TYPE_UC;
        entry.key.care_bits.pkt_type = 7;

        entry.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
        entry.action.cf_fwd_id = portIndex;


        entry.action.cact = AAL_L2_CF_ACTION_C_ACT_ADD_CTAG;
        entry.action.cvid_act = AAL_L2_CF_ACTION_VID_ACT_FROM_LUT;
        entry.action.cvid = 0;
    }
    else  /* SP2C*/
    {
        ret = _9607f_cls_rule_index_get(pClsRule->index+RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE, pClsRule->direction, &index, HAL_GET_PON_PORT(), 0, 1);
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
        pEntryData = kmalloc(sizeof(dal_9607f_cls_rule_mcgid_t), GFP_KERNEL);
        if(!pEntryData)
        {
            printk(KERN_ERR"[%s] %d Alloc memory for mcgid fail  ruleIndex:%d "
                ,__FUNCTION__, __LINE__, pClsRule->index);
            return RT_ERR_FAILED;
        }
        memset(pEntryData, 0, sizeof(dal_9607f_cls_rule_mcgid_t));
        pEntryData->ruleIndex = pClsRule->index+RTL9607F_CLS_XLATEVID_INDEX_SHIFT_BASE;
        if(aal_sw_mcgid_alloc(&pEntryData->swMcgid) != RT_ERR_OK)
        {
            printk(KERN_ERR"[%s] %d Alloc SW mcgid fail  ruleIndex:%d "
                ,__FUNCTION__, __LINE__, pClsRule->index);
            return RT_ERR_FAILED;
        }
        for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
        {
            if(pXlateVID[i].xlateMode != RTL9607F_CLS_XLATEVID_NOT_USED)
            {
                if(HAL_IS_CPU_PORT(i))
                    continue;
                memset(&portAction,0,sizeof(portForwardActionVlanExtend_t));
                portAction.ldpid = i;
                if(pXlateVID[i].vid == RTL9607F_CLS_XLATEVID_UNTAG)
                {
                    portAction.cvlanId_cmd = ACTION_VLAN_UNTAG;
                }
                if(pXlateVID[i].vid <= 4095)
                {
                    portAction.cvlanId_cmd = ACTION_VLAN_MODIFY;
                    portAction.cvlanId = pXlateVID[i].vid;
                }
                aal_port_forward_action_add(pEntryData->swMcgid,&portAction);
            }
        }

        aal_hw_mcgid_new(pEntryData->swMcgid, &pEntryData->hwMcgid);
        list_add_tail(&pEntryData->list, &mcgidHead);

        /* Filter BC + UUC by mask bit */
        entry.key.data_bits.pkt_type = AAL_L2_CF_PATTEN_PKT_TYPE_BC;
        entry.key.care_bits.pkt_type = 5;

        entry.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_MCGID;
        entry.action.cf_fwd_id = pEntryData->hwMcgid;
#endif

        entry.action.cact = AAL_L2_CF_ACTION_C_ACT_TRANSPARENT;
        entry.action.cs_act = AAL_L2_CF_ACTION_CS_ACT_TRANSPARENT;
    }

    ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index, &entry);

    if(ret != CA_E_OK)
    {
       printk(KERN_ERR"[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
           ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
       return ret;
    }

    return ret;
}

static int _9607f_cls_l2_vlan_aggregate(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *cur_next = NULL;
    struct list_head *cur_tmp = NULL;
    dal_9607f_cls_rule_db_t *cur = NULL;
    struct list_head *tar_next = NULL;
    struct list_head *tar_tmp = NULL;
    dal_9607f_cls_rule_db_t *tar = NULL;
    uint32 aggrUniMask, curUniMask, tarUniMask;
    uint32 setupAggVlanCf;
    int32 ret;
    uint32 outerVID;
    dal_9607f_cls_vlan_xlate_info_t pXlateVID[RTK_MAX_NUM_OF_PORTS];
    uint32 i;
    dal_9607f_cls_vlan_rule_index_t *pVlanRuleIndexEntry;
    rt_cls_filter_mask_t fwdAllMask = 0xffff - RT_CLS_FILTER_FLOWID_BIT - RT_CLS_FILTER_EGRESS_PORT_BIT;


    /* Delete all exist L2 Aggregated VLAN CF rule & clear database*/
    _9607f_cls_vlan_aggregate_cf_delAll();
    /* Serach all service flow to find out any aggregated service */
    __CF_DS_DB_LOCK();
    list_for_each_safe(cur_next, cur_tmp, &dsRuleHead)
    {
        cur = list_entry(cur_next, dal_9607f_cls_rule_db_t, list);
        if((cur->rule.filterRule.filterMask & fwdAllMask) == 0)
            continue;
        if(cur->rule.ingressPortMask != 0)
            curUniMask = cur->rule.ingressPortMask;
        else
            curUniMask = cur->rule.filterRule.egressPortMask;
        if(curUniMask == (1 << HAL_GET_CPU_PORT()))
            continue;
        aggrUniMask = curUniMask;
        setupAggVlanCf = FALSE;


        /* fill Outer & Translation VID database by current flow */
        for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
        {
            pXlateVID[i].xlateMode = RTL9607F_CLS_XLATEVID_NOT_USED;
            pXlateVID[i].vid = 0;
        }
        _9607f_cls_vlan_aggregate_fill_outerVID(&outerVID, &cur->rule);
        _9607f_cls_vlan_aggregate_fill_xlateVID(pXlateVID, &cur->rule, outerVID, curUniMask);
        list_for_each_safe(tar_next, tar_tmp, &dsRuleHead)
        {
            tar = list_entry(tar_next, dal_9607f_cls_rule_db_t, list);
            if(tar->rule.ingressPortMask != 0)
                tarUniMask = tar->rule.ingressPortMask;
            else
                tarUniMask = tar->rule.filterRule.egressPortMask;
            if(tarUniMask == (1 << HAL_GET_CPU_PORT()))
                continue;
            /* Compare 2 flows only when they are in the same DS SID and current flow is not VLAN_OPER_MODE_FORWARD_ALL */
            if ((cur->rule.filterRule.flowId == tar->rule.filterRule.flowId) && (cur->rule.index != tar->rule.index))
            {
                if (_9607f_cls_is_same_rule_by_ds_vlan_filter( &cur->rule, &tar->rule) == TRUE)
                {
                    /* Same DS VLAN Filter */
                    /* Setup Aggregated VLAN rule only if           */
                    /* 1. Target rule is VLAN_OPER_MODE_FORWARD_ALL */
                    /* 2. Current rule index < Target rule index    */
                    if (cur->rule.index < tar->rule.index || (tar->rule.filterRule.filterMask & fwdAllMask) == 0)
                    {
                        aggrUniMask |= tarUniMask;
                        setupAggVlanCf = TRUE;
                        /* fill Translation VID database by target flow */
                        _9607f_cls_vlan_aggregate_fill_xlateVID(pXlateVID, &(tar->rule), outerVID, tarUniMask);
                    }
                    else
                    {
                        /* Always setup aggregated VLAN CF via lowest index service flow */
                        setupAggVlanCf = FALSE;
                        break;
                    }
                }
            }
        }
        if (setupAggVlanCf == TRUE)
        {
            for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
            {
                if(HAL_IS_CPU_PORT(i))
                    continue;
                if (pXlateVID[i].xlateMode == RTL9607F_CLS_XLATEVID_TWO_MORE)
                {
                    if ((ret = _9607f_cls_vlan_aggregate_cf_set(RTL9607F_CLS_XLATEVID_TWO_MORE,  &(cur->rule), i, pXlateVID)) != RT_ERR_OK)
                        printk("omci_set_vlan_aggregate_cf Fail, ret = 0x%X", ret);
                }
            }

            if ((ret = _9607f_cls_vlan_aggregate_cf_set(RTL9607F_CLS_XLATEVID_ONLY_ONE,  &(cur->rule), i, pXlateVID)) != RT_ERR_OK)
                printk("omci_set_vlan_aggregate_sp2c_cf Fail, ret = 0x%X", ret);

            pVlanRuleIndexEntry=kmalloc(sizeof(dal_9607f_cls_vlan_rule_index_t), GFP_KERNEL);
            if (!pVlanRuleIndexEntry)
                return RT_ERR_FAILED;
            memset(pVlanRuleIndexEntry, 0, sizeof(dal_9607f_cls_vlan_rule_index_t));
            pVlanRuleIndexEntry->index = cur->rule.index;
            list_add_tail(&pVlanRuleIndexEntry->list, &vlanRuleIndexHead);

        }
    }
    __CF_DS_DB_UNLOCK();
#endif
    return RT_ERR_OK;
}

static int32 _9607f_cls_rule_set(rt_cls_rule_t *pClsRule)
{
    aal_l2_cf_rule_t entry;
    int32 index=0, portIndex, dscp;
    int32 ret=RT_ERR_OK;
    dal_9607f_cls_rule_db_t *pNewDsRule=NULL;
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    dal_9607f_cls_rule_mcgid_t *pEntryData = NULL;
    portForwardActionVlanExtend_t portAction;
    ca_uint16_t hwMcgid = CA_UINT16_INVALID;
    uint32 uniMask = 0;

    memset(&entry,0,sizeof(aal_l2_cf_rule_t));
    ret = _9607f_cls_filter_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk(KERN_ERR"[%s] %d Set cls filter fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }

    ret = _9607f_cls_action_set(&entry,pClsRule);

    if(ret != RT_ERR_OK)
    {
        printk(KERN_ERR"[%s] %d Set cls action fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }

    if(pClsRule->ingressPortMask != 0)
    {
        uniMask = pClsRule->ingressPortMask;
    }
    else /* MC/BC rule */
    {
        uniMask = pClsRule->filterRule.egressPortMask;
    }

    for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
    {
        if(uniMask & (1 << portIndex))
        {
            if(HAL_IS_CPU_PORT(portIndex))
                continue;
            if(pClsRule->direction == RT_CLS_DIR_US)
            {
                entry.key.data_bits.lspid = portIndex;
                entry.key.care_bits.lspid = 0x3f;
            }
            else
            {
                entry.key.data_bits.ldpid = portIndex;
                entry.key.care_bits.ldpid = 0x3f;
                entry.key.data_bits.pkt_type = AAL_L2_CF_PATTEN_PKT_TYPE_UC;
                entry.key.care_bits.pkt_type = 7;
                entry.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
                entry.action.cf_fwd_id = portIndex;
            }
            if(RT_CLS_FILTER_DSCP_BIT & pClsRule->filterRule.filterMask)
            {
                for(dscp=0;dscp<=RTK_VALUE_OF_DSCP_MAX;dscp++)
                {
                    if(pClsRule->filterRule.dscp[dscp/8] & (1 << (dscp%8)))  
                    {
                        ret = _9607f_cls_rule_index_get(pClsRule->index, pClsRule->direction, &index, (portIndex+1)*RTL9607F_CLS_DSCP_PORT_INDEX_BASE + dscp, pClsRule->rulePriority, 0);
                        if(ret != RT_ERR_OK)
                        {
                            printk(KERN_ERR"[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                            return ret;
                        }
                        /*For rule saving, merge the DSCP filter which can be merged by carebit*/
                        if(dscp % 8 == 0 && pClsRule->filterRule.dscp[dscp/8] == 0xff)
                        {
                            entry.key.data_bits.tos_tc = dscp<<2;
                            entry.key.care_bits.tos_tc = 0xe0;
                            dscp+=7;
                        }
                        else
                        {
                            entry.key.data_bits.tos_tc = dscp<<2;
                            entry.key.care_bits.tos_tc = 0xfc;
                        }
                        ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index, &entry);
                        if(ret != CA_E_OK)
                        {
                                printk(KERN_ERR"[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                                    ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                                return ret;
                        }
                    }
                }
            }
            else
            {
                ret = _9607f_cls_rule_index_get(pClsRule->index, pClsRule->direction, &index, portIndex, pClsRule->rulePriority, 0);
                if(ret != RT_ERR_OK)
                {
                    printk(KERN_ERR"[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                        ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                    return ret;
                }

                ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index, &entry);
                if(ret != CA_E_OK)
                {
                        printk(KERN_ERR"[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                        return ret;
                }
            }
        }
    }

    /* DS UUC + BC rule */
    if(pClsRule->direction == RT_CLS_DIR_DS)
    {
        entry.key.data_bits.ldpid = 0;
        entry.key.care_bits.ldpid = 0;
        /* Filter BC + UUC by mask bit */
        entry.key.data_bits.pkt_type = AAL_L2_CF_PATTEN_PKT_TYPE_BC;
        entry.key.care_bits.pkt_type = 5;
        entry.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_MCGID;

        if(uniMask == (1 << HAL_GET_CPU_PORT()))
            return ret;

#if defined(CONFIG_SDK_KERNEL_LINUX)
        list_for_each_safe(pEntry, pTmpEntry, &mcgidHead)
        {
            pEntryData = list_entry(pEntry, dal_9607f_cls_rule_mcgid_t, list);
            if (pEntryData->ruleIndex == pClsRule->index)
            {
                aal_hw_mcgid_free(pEntryData->swMcgid,pEntryData->hwMcgid);
                aal_sw_mcgid_free(pEntryData->swMcgid);
                list_del(&pEntryData->list);
                kfree(pEntryData);
                pEntryData = NULL;
                break;
            }
        }
        pEntryData = kmalloc(sizeof(dal_9607f_cls_rule_mcgid_t), GFP_KERNEL);
        if(!pEntryData)
        {
            printk(KERN_ERR"[%s] %d Alloc memory for mcgid fail  ruleIndex:%d "
                ,__FUNCTION__, __LINE__, pClsRule->index);
            return RT_ERR_FAILED;
        }
        memset(pEntryData, 0, sizeof(dal_9607f_cls_rule_mcgid_t));
        pEntryData->ruleIndex = pClsRule->index;
        if(aal_sw_mcgid_alloc(&pEntryData->swMcgid) != RT_ERR_OK)
        {
            printk(KERN_ERR"[%s] %d Alloc SW mcgid fail  ruleIndex:%d "
                ,__FUNCTION__, __LINE__, pClsRule->index);
            return RT_ERR_FAILED;
        }
        for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
        {
            if(uniMask & (1 << portIndex))
            {
                if(HAL_IS_CPU_PORT(portIndex))
                    continue;
                memset(&portAction,0,sizeof(portForwardActionVlanExtend_t));
                portAction.ldpid = portIndex;
                aal_port_forward_action_add(pEntryData->swMcgid,&portAction);
            }
        }

        aal_hw_mcgid_new(pEntryData->swMcgid, &pEntryData->hwMcgid);
        hwMcgid = pEntryData->hwMcgid;
        list_add_tail(&pEntryData->list, &mcgidHead);
#endif
        entry.action.cf_fwd_id = hwMcgid;
        ret = _9607f_cls_rule_index_get(pClsRule->index, pClsRule->direction, &index, RTL9607F_CLS_RULE_PORT_IGNORE, pClsRule->rulePriority, 0);

        if(ret != RT_ERR_OK)
        {
            printk(KERN_ERR"[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
        }

        ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index, &entry);
        if(ret != CA_E_OK)
        {
                printk(KERN_ERR"[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                return ret;
        }
    }

    return ret;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_init
 * Description:
 *      add classification rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_init(void)
{
    aal_l2_cf_rule_t ruleCfg;
    uint32 index = RTL9607F_QOS_INGRESS_QOS_RESERVED_NUM, i;
    rtk_port_t port;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;
    aal_ple_dft_fwd_ctrl_mask_t dftMask;
    aal_ple_dft_fwd_ctrl_t dftCfg;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    clsRuleUsage=(dal_9607f_cls_rule_index_t *)kzalloc(sizeof(dal_9607f_cls_rule_index_t)*ruleMax, GFP_KERNEL);
#endif
    memset(clsRuleUsage,0,sizeof(dal_9607f_cls_rule_index_t)*ruleMax);
    for(i=0;i<ruleMax;i++)
    {
        clsRuleUsage[i].index = RTL9607F_CLS_RULE_NOT_IN_USED;
        clsRuleUsage[i].port = RTL9607F_CLS_RULE_PORT_NOT_USED;
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    INIT_LIST_HEAD(&dsRuleHead);
    INIT_LIST_HEAD(&usRuleHead);
    INIT_LIST_HEAD(&vlanRuleIndexHead);
    INIT_LIST_HEAD(&sp2cVlanHead);
    INIT_LIST_HEAD(&mcgidHead);
    if(ca_spin_lock_init(&__dal_cf_ds_db_lock, SPINLOCK_BH) != CA_OSAL_OK)
    {
        printk("[%s] %d Init CF DS DB spin lock fail",__FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
    if( ca_spin_lock_init(&__dal_cf_us_db_lock, SPINLOCK_BH) != CA_OSAL_OK)
    {
        printk("[%s] %d Init CF US DB spin lock fail",__FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
#endif
    memset(&clsSidInfo, 0, sizeof(rt_ponmisc_sidInfo_t)*RTL9607F_CLS_SID_MAX);

    for(i=0;i<RTK_MAX_NUM_OF_PORTS;i++)
    {
        portRuleIndex[i] = -1;
        portFwdPort[i] = AAL_LPORT_L3_LAN;
    }
    portFwdPort[HAL_GET_PON_PORT()] = AAL_LPORT_L3_WAN;
    portFwdPort[RTL9607F_CLS_EXT_WAN_PORT] = AAL_LPORT_L3_WAN;

    memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
    ilpbCfgMask.s.cf_sel_wan_en = 1;
    memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
    ilpbCfg.cf_sel_wan_en = 1;
    aal_port_ilpb_cfg_set(0, HAL_GET_PON_PORT(), ilpbCfgMask, &ilpbCfg);
    aal_port_ilpb_cfg_set(0, RTL9607F_CLS_EXT_WAN_PORT, ilpbCfgMask, &ilpbCfg);

    /* port vlan setting */
    memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
    memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
    ilpbCfgMask.s.top_c_tpid_enable = 1;
    ilpbCfg.top_c_tpid_enable = 1;
    ilpbCfgMask.s.top_s_tpid_enable = 1;
    ilpbCfg.top_s_tpid_enable = 1;
    ilpbCfgMask.s.inner_s_tpid_enable = 1;
    ilpbCfg.inner_s_tpid_enable = 0;
    ilpbCfgMask.s.inner_c_tpid_enable = 1;
    ilpbCfg.inner_c_tpid_enable = 1;
    ilpbCfgMask.s.s_tpid_match = 1;
    ilpbCfg.s_tpid_match = 2;
    ilpbCfgMask.s.c_tpid_match = 1;
    ilpbCfg.c_tpid_match = 8;
    HAL_SCAN_ALL_ETH_PORT(port)
    {
        aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
    }
    aal_port_ilpb_cfg_set(0, RTL9607F_CLS_EXT_WAN_PORT, ilpbCfgMask, &ilpbCfg);
    aal_port_ilpb_cfg_set(0, AAL_LPORT_MC, ilpbCfgMask, &ilpbCfg);
    for(i=AAL_LPORT_CPU_0;i<=AAL_LPORT_CPU_7;i++)
        aal_port_ilpb_cfg_set(0, i, ilpbCfgMask, &ilpbCfg);

    /* Ext WAN DFT init */
    /* UUC */
    memset(&dftCfg, 0, sizeof(aal_ple_dft_fwd_ctrl_t));
    memset(&dftMask, 0, sizeof(aal_ple_dft_fwd_ctrl_mask_t));
    dftCfg.dft_fwd_deny = 0;
    dftCfg.dft_fwd_redir_en = 0;
    dftCfg.dft_fwd_mc_group_id_valid = 0;
    dftCfg.dft_fwd_mc_group_id = 0;
    dftMask.bf.dft_fwd_deny = 1;
    dftMask.bf.dft_fwd_redir_en = 1;
    dftMask.bf.dft_fwd_mc_group_id_valid = 1;
    dftMask.bf.dft_fwd_mc_group_id = 1;
    aal_ple_dft_fwd_set(0,RTL9607F_CLS_EXT_WAN_PORT,AAL_PLE_NONKWN_TYPE_UUC,dftMask,&dftCfg);

    /* BC/MC */
    memset(&dftCfg, 0, sizeof(aal_ple_dft_fwd_ctrl_t));
    memset(&dftMask, 0, sizeof(aal_ple_dft_fwd_ctrl_mask_t));
    dftCfg.dft_fwd_deny = 0;
    dftCfg.dft_fwd_redir_en = 1;
    dftCfg.dft_fwd_mc_group_id_valid = 1;
    dftCfg.dft_fwd_mc_group_id = AAL_LPORT_L3_WAN;
    dftMask.bf.dft_fwd_deny = 1;
    dftMask.bf.dft_fwd_redir_en = 1;
    dftMask.bf.dft_fwd_mc_group_id_valid = 1;
    dftMask.bf.dft_fwd_mc_group_id = 1;
    aal_ple_dft_fwd_set(0,RTL9607F_CLS_EXT_WAN_PORT,AAL_PLE_NONKWN_TYPE_BC,dftMask,&dftCfg);
    aal_ple_dft_fwd_set(0,RTL9607F_CLS_EXT_WAN_PORT,AAL_PLE_NONKWN_TYPE_UL2MC,dftMask,&dftCfg);
    aal_ple_dft_fwd_set(0,RTL9607F_CLS_EXT_WAN_PORT,AAL_PLE_NONKWN_TYPE_UL3MC,dftMask,&dftCfg);

    cls_init = INIT_COMPLETED;

    /* Rule 16: DS to L3FE forward to L3FE WAN*/
    memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
    ruleCfg.key.care_bits.valid = 1;
    ruleCfg.key.data_bits.valid = 1;
    ruleCfg.key.care_bits.ldpid = 0x3e;
    ruleCfg.key.data_bits.ldpid = AAL_LPORT_L3_WAN;
    ruleCfg.key.care_bits.direction = 1;
    ruleCfg.key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_DOWNSTREAM;
    ruleCfg.action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
    ruleCfg.action.cf_cos = 0;
    ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
    ruleCfg.action.cf_fwd_id = AAL_LPORT_L3_WAN;
    aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index,&ruleCfg);
    index++;

    /* Rule 17: US to L3FE forward to L3FE LAN*/
    memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
    ruleCfg.key.care_bits.valid = 1;
    ruleCfg.key.data_bits.valid = 1;
    ruleCfg.key.care_bits.ldpid = 0x3e;
    ruleCfg.key.data_bits.ldpid = AAL_LPORT_L3_WAN;
    ruleCfg.key.care_bits.direction = 1;
    ruleCfg.key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_UPSTREAM;
    ruleCfg.action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
    ruleCfg.action.cf_cos = 0;
    ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
    ruleCfg.action.cf_fwd_id = AAL_LPORT_L3_LAN;
    aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index,&ruleCfg);
    index++;

    /* Rule 18: Bypass special LDPID to CPU */
    memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
    ruleCfg.key.care_bits.valid = 1;
    ruleCfg.key.data_bits.valid = 1;
    ruleCfg.key.care_bits.ldpid = 0x3f;
    ruleCfg.key.data_bits.ldpid = AAL_LPORT_SPCL;
    ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_TRAP_TO_CPU;
    ruleCfg.action.cf_fwd_id = AAL_LPORT_CPU_0;
    aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index,&ruleCfg);
    index++;

    /* Rule 19~25: LAN VEIP port default rule*/
#ifdef CONFIG_CORTINA_BOARD_FPGA
    for(port = 1;port < 4; port++)
#else
    HAL_SCAN_ALL_ETH_PORT(port)
#endif
    {
        if(HAL_IS_PON_PORT(port))
            continue;
        portRuleIndex[port] = index++;
        dal_rt_rtl9607f_cls_veipPortRule_set(port, ENABLED);
    }

    /* Rule 26/27: PON VEIP port default rule*/
    portRuleIndex[HAL_GET_PON_PORT()] = index++;
    portRuleIndex[RTL9607F_CLS_EXT_WAN_PORT] = index++;
    dal_rt_rtl9607f_cls_veipPortRule_set(HAL_GET_PON_PORT(), ENABLED);

    /* Rule 28~34: bypass LAN to LAN*/
#ifdef CONFIG_CORTINA_BOARD_FPGA
    for(port = 1;port < 4; port++)
#else
    HAL_SCAN_ALL_ETH_PORT(port)
#endif
    {
        if(HAL_IS_PON_PORT(port))
            continue;
        memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
        ruleCfg.key.care_bits.valid = 1;
        ruleCfg.key.data_bits.valid = 1;
        ruleCfg.key.care_bits.ldpid = 0x3f;
        ruleCfg.key.data_bits.ldpid = port;

        ruleCfg.key.care_bits.direction = 1;
        ruleCfg.key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_UPSTREAM;
        aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, index,&ruleCfg);
        index++;
    }

    if(index > RTL9607F_CLS_RULE_DEFAULT_RESERVED_NUM)
    {
        printk("Default reserve rule need %d rules, but only reserve %d rules\n",index-1,RTL9607F_CLS_RULE_DEFAULT_RESERVED_NUM);
    }

    ethertypeTable[RTL9607F_TRAP_LOOP_DETECT_EV2PT_ID] = RTL9607F_TRAP_LOOP_DETECT_ETHERTYPE;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_cls_veip_port_rule_set
 * Description:
 *      Enable/Disable VEIP port default rule
 * Input:
 *      port   - Port index
 *      state  - VEIP port state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_veipPortRule_set(rtk_port_t port, rtk_enable_t state)
{
    aal_l2_cf_rule_t ruleCfg;
    portForwardActionVlanExtend_t portAction;
    uint32 i;
    uint32 hasPptp = 0;
    ca_uint16_t new_hw_mcgid;

    /* check Init status */
    RT_INIT_CHK(cls_init);

    if(port >= RTK_MAX_NUM_OF_PORTS || portRuleIndex[port]  == -1)
    {
        return RT_ERR_PORT_ID;
    }
    if(state == ENABLED)
    {
        memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
        ruleCfg.key.care_bits.valid = 1;
        ruleCfg.key.data_bits.valid = 1;
        ruleCfg.key.care_bits.lspid = 0x3f;
        ruleCfg.key.data_bits.lspid = port;
        ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
        ruleCfg.action.cf_fwd_id = portFwdPort[port];
        ruleCfg.action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
        ruleCfg.action.cf_cos = 0;
        aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, portRuleIndex[port] ,&ruleCfg);
        portType[port] = RTL9607F_CLS_PORT_TYPE_VEIP;
    }
    else
    {
        aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, portRuleIndex[port]);
        portType[port] = RTL9607F_CLS_PORT_TYPE_PPTP;
    }

    if(port == HAL_GET_PON_PORT())
    {
        if(state == ENABLED)
        {
            memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
            ruleCfg.key.care_bits.valid = 1;
            ruleCfg.key.data_bits.valid = 1;
            ruleCfg.key.care_bits.lspid = 0x3f;
            ruleCfg.key.data_bits.lspid = RTL9607F_CLS_EXT_WAN_PORT;
            ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
            ruleCfg.action.cf_fwd_id = portFwdPort[RTL9607F_CLS_EXT_WAN_PORT];
            ruleCfg.action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
            ruleCfg.action.cf_cos = 0;
            aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, portRuleIndex[RTL9607F_CLS_EXT_WAN_PORT] ,&ruleCfg);
            portType[RTL9607F_CLS_EXT_WAN_PORT] = RTL9607F_CLS_PORT_TYPE_VEIP;
        }
        else
        {
            aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, portRuleIndex[RTL9607F_CLS_EXT_WAN_PORT]);
            portType[RTL9607F_CLS_EXT_WAN_PORT] = RTL9607F_CLS_PORT_TYPE_PPTP;
        }
        return RT_ERR_OK;
    }

    for(i=0;i<RTK_MAX_NUM_OF_PORTS;i++)
    {
        if(portType[i] == RTL9607F_CLS_PORT_TYPE_PPTP)
        {
            hasPptp = 1;
            break;
        }
    }

    if(hasPptp == 1)
    {
        if(sw_mcgid == CA_UINT16_INVALID)
        {
            if(state != DISABLED)
            {
                return RT_ERR_FAILED;
            }
            if(aal_sw_mcgid_alloc(&sw_mcgid) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
            memset(&portAction,0,sizeof(portForwardActionVlanExtend_t));
            portAction.ldpid = port;
            aal_port_forward_action_add(sw_mcgid,&portAction);
        }
        else
        {
            memset(&portAction,0,sizeof(portForwardActionVlanExtend_t));
            portAction.ldpid = port;
            if(state == DISABLED)
                aal_port_forward_action_add(sw_mcgid,&portAction);
            else
                aal_port_forward_action_del(sw_mcgid,&portAction);
        }
        aal_hw_mcgid_new(sw_mcgid,&new_hw_mcgid);
        if(hw_mcgid != CA_UINT16_INVALID && hw_mcgid != new_hw_mcgid)
            aal_hw_mcgid_free(sw_mcgid,hw_mcgid);
        hw_mcgid = new_hw_mcgid;

        /*Index 510: LAN to LAN UUC rule */
        memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
        ruleCfg.key.care_bits.valid = 1;
        ruleCfg.key.data_bits.valid = 1;
        ruleCfg.key.care_bits.direction = 1;
        ruleCfg.key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_UPSTREAM;
        ruleCfg.key.data_bits.pkt_type = AAL_L2_CF_PATTEN_PKT_TYPE_UUC;
        ruleCfg.key.care_bits.pkt_type = 7;
        ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_MCGID;
        ruleCfg.action.cf_fwd_id = hw_mcgid;
        aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, HAL_CLASSIFY_ENTRY_MAX()-2 ,&ruleCfg);

        /*Index 511: default drop rule */
        memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
        ruleCfg.key.care_bits.valid = 1;
        ruleCfg.key.data_bits.valid = 1;
        ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_DROP;
        aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, HAL_CLASSIFY_ENTRY_MAX()-1 ,&ruleCfg);

    }
    else
    {
        aal_hw_mcgid_free(sw_mcgid,hw_mcgid);
        aal_sw_mcgid_free(sw_mcgid);
        sw_mcgid = CA_UINT16_INVALID;
        hw_mcgid = CA_UINT16_INVALID;

        aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, HAL_CLASSIFY_ENTRY_MAX()-2);
        aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, HAL_CLASSIFY_ENTRY_MAX()-1);
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_rule_add
 * Description:
 *      add classification rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_rule_add(rt_cls_rule_t *pClsRule)
{
    int32 ret=RT_ERR_FAILED;
    dal_9607f_cls_rule_db_t *pNewDsRule=NULL;

    /* check Init status */
    RT_INIT_CHK(cls_init);
    RT_PARAM_CHK(pClsRule->flowId >= RTL9607F_CLS_SID_MAX, RT_ERR_INPUT);

    _9607f_cls_rule_set(pClsRule);

    /* VLAN aggregation */
    if(pClsRule->direction == RT_CLS_DIR_DS)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        pNewDsRule = kmalloc(sizeof(dal_9607f_cls_rule_db_t), GFP_KERNEL);
#endif
        if (!pNewDsRule)
            return RT_ERR_FAILED;
        osal_memcpy(&pNewDsRule->rule, pClsRule, sizeof(rt_cls_rule_t));
#if defined(CONFIG_SDK_KERNEL_LINUX)
        __CF_DS_DB_LOCK();
        list_add_tail(&pNewDsRule->list, &dsRuleHead);
        __CF_DS_DB_UNLOCK();
#endif
        _9607f_cls_l2_vlan_aggregate();
    }

    if(pClsRule->direction == RT_CLS_DIR_US)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        pNewDsRule = kmalloc(sizeof(dal_9607f_cls_rule_db_t), GFP_KERNEL);
#endif
        if (!pNewDsRule)
            return RT_ERR_FAILED;
        osal_memcpy(&pNewDsRule->rule, pClsRule, sizeof(rt_cls_rule_t));
#if defined(CONFIG_SDK_KERNEL_LINUX)
        __CF_US_DB_LOCK();
        list_add_tail(&pNewDsRule->list, &usRuleHead);
        __CF_US_DB_UNLOCK();
#endif
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_rule_delete
 * Description:
 *      delete classification rule
 * Input:
 *      index   - classification rule index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_rule_delete(uint32 ruleIndex)
{
    int32 index, dsChange = 0;
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    dal_9607f_cls_rule_db_t  *pEntryData= NULL;
    dal_9607f_cls_rule_mcgid_t *pMcgidEntryData= NULL;
#endif
    rtk_port_t port;
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;
    aal_l2_vlan_default_cfg_mask_t defaultCfgMask;
    aal_l2_vlan_default_cfg_t defaultCfg;

    /* check Init status */
    RT_INIT_CHK(cls_init);

    while(_9607f_cls_rule_index_find(ruleIndex, RT_CLS_DIR_DS, &index,RTL9607F_CLS_RULE_PORT_IGNORE, 0) == RT_ERR_OK)
    {
        ret = aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, index);

        if(ret != CA_E_OK)
        {
            printk("[%s] %d Delete classify rule fail ret:%d index:%d "
            ,__FUNCTION__, __LINE__, ret, index);
            return ret;
        }

        ret = _9607f_cls_rule_index_del(index);
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Delete cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, ruleIndex, RT_CLS_DIR_US);
            return ret;
        }
    }

#if defined(CONFIG_SDK_KERNEL_LINUX)
    __CF_DS_DB_LOCK();
    list_for_each_safe(pEntry, pTmpEntry, &dsRuleHead)
    {
        pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
        if (pEntryData->rule.index == ruleIndex)
        {
            dsChange = 1;
            list_del(&pEntryData->list);
            kfree(pEntryData);
        }
    }
    __CF_DS_DB_UNLOCK();
    
    list_for_each_safe(pEntry, pTmpEntry, &mcgidHead)
    {
        pMcgidEntryData = list_entry(pEntry, dal_9607f_cls_rule_mcgid_t, list);
        if (pMcgidEntryData->ruleIndex == ruleIndex)
        {
            aal_hw_mcgid_free(pMcgidEntryData->swMcgid,pMcgidEntryData->hwMcgid);
            aal_sw_mcgid_free(pMcgidEntryData->swMcgid);
            list_del(&pMcgidEntryData->list);
            kfree(pMcgidEntryData);
            pMcgidEntryData = NULL;
            break;
        }
    }
#endif

    while(_9607f_cls_rule_index_find(ruleIndex, RT_CLS_DIR_US, &index,RTL9607F_CLS_RULE_PORT_IGNORE, 0) == RT_ERR_OK)
    {
        ret = aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, index);

        if(ret != CA_E_OK)
        {
                printk("[%s] %d Delete classify rule fail ret:%d index:%d "
                    ,__FUNCTION__, __LINE__, ret, index);
                return ret;
        }

        ret = _9607f_cls_rule_index_del(index);
        if(ret != RT_ERR_OK)
        {
                printk("[%s] %d Delete cls rule index fail ret:%d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, ret, ruleIndex, RT_CLS_DIR_US);
                return ret;
        }
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    __CF_US_DB_LOCK();
    list_for_each_safe(pEntry, pTmpEntry, &usRuleHead)
    {
        pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
        if (pEntryData->rule.index == ruleIndex)
        {
            list_del(&pEntryData->list);
            kfree(pEntryData);
        }
    }

    __CF_DS_DB_LOCK();
    if(list_empty(&usRuleHead) && list_empty(&dsRuleHead))
    {
        doubleCtag = 0;
        memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
        memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
        ilpbCfgMask.s.s_tpid_match = 1;
        ilpbCfg.s_tpid_match = 2;
        HAL_SCAN_ALL_ETH_PORT(port)
        {
            aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,port,ilpbCfgMask,&ilpbCfg);
        }
        aal_port_ilpb_cfg_set(DEFAULT_DEVICE_ID,HAL_GET_PON_PORT(),ilpbCfgMask,&ilpbCfg);

        memset(&defaultCfg,0,sizeof(aal_l2_vlan_default_cfg_t));
        memset(&defaultCfgMask,0,sizeof(aal_l2_vlan_default_cfg_mask_t));
        defaultCfg.cmp_tpid_user = 0;
        defaultCfgMask.s.cmp_tpid_user = 1;
        aal_l2_vlan_default_cfg_set(DEFAULT_DEVICE_ID,defaultCfgMask,&defaultCfg);

    }
    __CF_DS_DB_UNLOCK();
    __CF_US_DB_UNLOCK();
#endif

    if(dsChange == 1)
        _9607f_cls_l2_vlan_aggregate();
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_rule_get
 * Description:
 *      Get  classify rule.
 * Input:
 *      op        - operation type of get (first/next/index)
 *      index     - index of classify rule entry.
 *      dir       - direction of classify rule
 * Output:
 *      pClsRuleInfo  - classify rule information
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
extern int32 dal_rt_rtl9607f_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo)
{
    int32   ret = RT_ERR_OK;
    uint32  ruleHwIndex=0,i;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    dal_9607f_cls_rule_db_t *pEntryData= NULL;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    struct list_head *pHead     = NULL;
#endif

    /* check Init status */
    RT_INIT_CHK(cls_init);

    if(pClsRuleInfo == NULL)
        return RT_ERR_NULL_POINTER;

    memset(pClsRuleInfo->hwIndex,0xff,sizeof(uint16)*RT_CLS_HW_INDEX_MAX);

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(dir == RT_CLS_DIR_DS)
    {
        pHead = &dsRuleHead;
        __CF_DS_DB_LOCK();
    }
    else
    {
        pHead = &usRuleHead;
        __CF_US_DB_LOCK();
    }
#endif

    if(op == RT_CLS_GET_OP_FIRST)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        pEntryData = list_first_entry_or_null(pHead, dal_9607f_cls_rule_db_t, list);
#else
        pEntryData = NULL;
#endif
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    else if(op == RT_CLS_GET_OP_NEXT)
    {
        list_for_each_safe(pEntry, pTmpEntry, pHead)
        {
            pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
            if (pEntryData->rule.index == index)
            {
                if(!list_is_last(pEntry,pHead))
                {
                    pEntryData = list_next_entry(pEntryData, list);
                }
                else
                {
                    pEntryData=NULL;
                }
                break;
            }
        }
    }
    else
    {
        list_for_each_safe(pEntry, pTmpEntry, pHead)
        {
            pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
            if (pEntryData->rule.index == index)
            {
                break;
            }
            pEntryData = NULL;
        }
    }
#endif

    if(pEntryData == NULL)
    {
        if(dir == RT_CLS_DIR_DS)
            __CF_DS_DB_UNLOCK();
        else
            __CF_US_DB_UNLOCK();
        return RT_ERR_FAILED;
    }
    else
    {
        memcpy(&(pClsRuleInfo->rule),&(pEntryData->rule),sizeof(rt_cls_rule_t));
    }

    for(i=RTL9607F_CLS_RULE_SYSTEM_RESERVED_NUM;i<ruleMax;i++)
    {
        if((RTL9607F_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == pEntryData->rule.index) && (clsRuleUsage[i].dir == dir) )
        {
            pClsRuleInfo->hwIndex[ruleHwIndex++]=i;
            if(ruleHwIndex >= RT_CLS_HW_INDEX_MAX)
                break;
        }
    }
    if(dir == RT_CLS_DIR_DS)
        __CF_DS_DB_UNLOCK();
    else
        __CF_US_DB_UNLOCK();
    return ret;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_extend_rule_add
 * Description:
 *      add classification extend rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_extend_rule_add(rt_cls_extend_rule_t *pClsRule)
{
    aal_l2_cf_rule_t entry;
    int32 ret = RT_ERR_OK;
    uint8 ethertypeCode;

    /* check Init status */
    RT_INIT_CHK(cls_init);


    if(pClsRule == NULL)
    {
        return RT_ERR_FAILED;
    }

    memset(&entry, 0, sizeof(aal_l2_cf_rule_t));

    /* Filter */

    entry.key.care_bits.valid = ENABLED;
    entry.key.data_bits.valid = ENABLED;

    entry.key.care_bits.tos_tc = pClsRule->filterRule.mask.tosTc;
    entry.key.data_bits.tos_tc = pClsRule->filterRule.data.tosTc;

    entry.key.care_bits.outer_vid = pClsRule->filterRule.mask.outerVid;
    entry.key.data_bits.outer_vid = pClsRule->filterRule.data.outerVid;

    entry.key.care_bits.outer_pri = pClsRule->filterRule.mask.outerPri;
    entry.key.data_bits.outer_pri = pClsRule->filterRule.data.outerPri;

    entry.key.care_bits.outer_dei_cfi = pClsRule->filterRule.mask.outerDei;
    entry.key.data_bits.outer_dei_cfi = pClsRule->filterRule.data.outerDei;

    entry.key.care_bits.ctag_vid = pClsRule->filterRule.mask.ctagVid;
    entry.key.data_bits.ctag_vid = pClsRule->filterRule.data.ctagVid;

    entry.key.care_bits.ctag_pri = pClsRule->filterRule.mask.ctagPri;
    entry.key.data_bits.ctag_pri = pClsRule->filterRule.data.ctagPri;

    entry.key.care_bits.ctag_cfi = pClsRule->filterRule.mask.ctagCfi;
    entry.key.data_bits.ctag_cfi = pClsRule->filterRule.data.ctagCfi;

    entry.key.care_bits.direction = pClsRule->filterRule.mask.direction;
    entry.key.data_bits.direction = pClsRule->filterRule.data.direction;

    entry.key.care_bits.pol_id = pClsRule->filterRule.mask.flowId;
    entry.key.data_bits.pol_id = pClsRule->filterRule.data.flowId;

    entry.key.care_bits.igmp_mld = pClsRule->filterRule.mask.igmpMld;
    entry.key.data_bits.igmp_mld = pClsRule->filterRule.data.igmpMld;

    entry.key.care_bits.ipv6 = pClsRule->filterRule.mask.ipv6;
    entry.key.data_bits.ipv6 = pClsRule->filterRule.data.ipv6;

    entry.key.care_bits.ipv4 = pClsRule->filterRule.mask.ipv4;
    entry.key.data_bits.ipv4 = pClsRule->filterRule.data.ipv4;

    entry.key.care_bits.pkt_type = pClsRule->filterRule.mask.pktType;
    entry.key.data_bits.pkt_type = pClsRule->filterRule.data.pktType;

    entry.key.care_bits.s_tpid = pClsRule->filterRule.mask.sTpid;
    entry.key.data_bits.s_tpid = pClsRule->filterRule.data.sTpid;

    entry.key.care_bits.ctag = pClsRule->filterRule.mask.cbit;
    entry.key.data_bits.ctag = pClsRule->filterRule.data.cbit;

    entry.key.care_bits.stag = pClsRule->filterRule.mask.sbit;
    entry.key.data_bits.stag = pClsRule->filterRule.data.sbit;

    entry.key.care_bits.lspid = pClsRule->filterRule.mask.ingressPort;
    entry.key.data_bits.lspid = pClsRule->filterRule.data.ingressPort;

    entry.key.care_bits.ldpid = pClsRule->filterRule.mask.egressPort;
    entry.key.data_bits.ldpid = pClsRule->filterRule.data.egressPort;

    if(pClsRule->filterRule.mask.ethertype != 0)
    {
        if(pClsRule->filterRule.mask.ethertype != 0xffff)
        {
            printk(KERN_ERR"Ethertype not support mask not 0xffff\n");
            return RT_ERR_FAILED;
        }
        _9607f_cls_ethertype_encode_get(pClsRule->filterRule.data.ethertype, &ethertypeCode);
        entry.key.care_bits.ev2pt_table = 0x1f;
        entry.key.data_bits.ev2pt_table = ethertypeCode;
    }

    /* FIB */

    if(pClsRule->sTagAct.vidAction == RT_CLS_EXT_VID_FROM_FDB)
    {
        printk(KERN_ERR"STag vid action not support from FDB\n");
        return RT_ERR_FAILED;
    }

    entry.action.cs_act = pClsRule->sTagAct.tagAction;
    entry.action.cs_vid = pClsRule->sTagAct.assignedVid;
    entry.action.cs_pri = pClsRule->sTagAct.assignedPri;
    entry.action.cs_pri_act = pClsRule->sTagAct.priAction;
    entry.action.cs_vid_act = pClsRule->sTagAct.vidAction;
    entry.action.cscfi_keep = pClsRule->sTagAct.deiAction;
    entry.action.grpid_act = pClsRule->grpPolAct;
    entry.action.flowid_act = pClsRule->flowAct;
    entry.action.cf_grpid = pClsRule->grpPolId;
    entry.action.cf_flowid = pClsRule->flowId;
    entry.action.fwd_act = pClsRule->fwdAct;
    entry.action.cf_fwd_id = pClsRule->fwdId;
    entry.action.cact = pClsRule->cTagAct.tagAction;
    entry.action.cvid = pClsRule->cTagAct.assignedVid;
    entry.action.cpri = pClsRule->cTagAct.assignedPri;
    entry.action.cvid_act = pClsRule->cTagAct.vidAction;
    entry.action.cpri_act = pClsRule->cTagAct.priAction;
    entry.action.ccfi_keep = pClsRule->cTagAct.deiAction;
    entry.action.cf_cos_act = pClsRule->cosAct;
    entry.action.cf_cos = pClsRule->cos;

    ret = aal_l2_cf_rule_add(DEFAULT_DEVICE_ID, pClsRule->index, &entry);
    if(ret != CA_E_OK)
    {
        printk(KERN_ERR"[%s] %d Set extend cls rule fail ret:%d ruleIndex:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_extend_rule_delete
 * Description:
 *      delete classification extend rule
 * Input:
 *      index   - classification rule index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_extend_rule_delete(uint32 ruleIndex)
{
    int32 ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(cls_init);

    ret = aal_l2_cf_rule_delete(DEFAULT_DEVICE_ID, ruleIndex);

    if(ret != CA_E_OK)
    {
        printk("[%s] %d Delete extend classify rule fail ret:%d index:%d "
        ,__FUNCTION__, __LINE__, ret, ruleIndex);
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_extend_rule_get
 * Description:
 *      Get classification extend rule
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      pClsRule  - extend classify rule
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
int32 dal_rt_rtl9607f_cls_extend_rule_get(uint32 index, rt_cls_extend_rule_t *pClsRule)
{
    aal_l2_cf_key_t key;
    aal_l2_cf_action_t fib;
    int32 ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(cls_init);

    if(pClsRule == NULL)
    {
        return RT_ERR_FAILED;
    }

    if(index >= HAL_CLASSIFY_ENTRY_MAX())
    {
        return RT_ERR_FAILED;
    }

    memset(pClsRule, 0, sizeof(rt_cls_extend_rule_t));
    pClsRule->index = index;

    /* key */
    memset(&key, 0, sizeof(aal_l2_cf_key_t));
    ret = aal_l2_cf_key_get(DEFAULT_DEVICE_ID, index, &key);

    if(ret != CA_E_OK)
    {
        printk("[%s] Get CLS key fail ret:%d index:%d " ,__FUNCTION__, __LINE__, ret, index);
        return ret;
    }

    pClsRule->valid = key.data_bits.valid;

    pClsRule->filterRule.mask.egressPort = key.care_bits.ldpid;
    pClsRule->filterRule.data.egressPort = key.data_bits.ldpid;

    pClsRule->filterRule.mask.ingressPort = key.care_bits.lspid;
    pClsRule->filterRule.data.ingressPort = key.data_bits.lspid;

    pClsRule->filterRule.mask.sbit = key.care_bits.stag;
    pClsRule->filterRule.data.sbit = key.data_bits.stag;

    pClsRule->filterRule.mask.cbit = key.care_bits.ctag;
    pClsRule->filterRule.data.cbit = key.data_bits.ctag;

    pClsRule->filterRule.mask.sTpid = key.care_bits.s_tpid;
    pClsRule->filterRule.data.sTpid = key.data_bits.s_tpid;

    pClsRule->filterRule.mask.pktType = key.care_bits.pkt_type;
    pClsRule->filterRule.data.pktType = key.data_bits.pkt_type;

    pClsRule->filterRule.mask.ipv4 = key.care_bits.ipv4;
    pClsRule->filterRule.data.ipv4 = key.data_bits.ipv4;

    pClsRule->filterRule.mask.ipv6 = key.care_bits.ipv6;
    pClsRule->filterRule.data.ipv6 = key.data_bits.ipv6;

    pClsRule->filterRule.mask.igmpMld = key.care_bits.igmp_mld;
    pClsRule->filterRule.data.igmpMld = key.data_bits.igmp_mld;

    pClsRule->filterRule.mask.flowId = key.care_bits.pol_id;
    pClsRule->filterRule.data.flowId = key.data_bits.pol_id;

    pClsRule->filterRule.mask.direction = key.care_bits.direction;
    pClsRule->filterRule.data.direction = key.data_bits.direction;

    pClsRule->filterRule.mask.ctagVid = key.care_bits.ctag_vid;
    pClsRule->filterRule.data.ctagVid = key.data_bits.ctag_vid;

    pClsRule->filterRule.mask.ctagPri = key.care_bits.ctag_pri;
    pClsRule->filterRule.data.ctagPri = key.data_bits.ctag_pri;

    pClsRule->filterRule.mask.ctagCfi = key.care_bits.ctag_cfi;
    pClsRule->filterRule.data.ctagCfi = key.data_bits.ctag_cfi;

    pClsRule->filterRule.mask.outerVid = key.care_bits.outer_vid;
    pClsRule->filterRule.data.outerVid = key.data_bits.outer_vid;

    pClsRule->filterRule.mask.outerPri = key.care_bits.outer_pri;
    pClsRule->filterRule.data.outerPri = key.data_bits.outer_pri;

    pClsRule->filterRule.mask.outerDei = key.care_bits.outer_dei_cfi;
    pClsRule->filterRule.data.outerDei = key.data_bits.outer_dei_cfi;

    pClsRule->filterRule.mask.tosTc = key.care_bits.tos_tc;
    pClsRule->filterRule.data.tosTc = key.data_bits.tos_tc;

    if(key.care_bits.ev2pt_table != 0)
    {
        pClsRule->filterRule.mask.ethertype = 0xffff;
        pClsRule->filterRule.data.ethertype = ethertypeTable[key.data_bits.ev2pt_table];
    }

    /* FIB */

    memset(&fib, 0, sizeof(aal_l2_cf_action_t));

    ret = aal_l2_cf_action_get(DEFAULT_DEVICE_ID, index, &fib);

    if(ret != CA_E_OK)
    {
        printk("[%s] Get CLS action fail ret:%d index:%d " ,__FUNCTION__, __LINE__, ret, index);
        return ret;
    }

    pClsRule->sTagAct.tagAction = fib.cs_act;
    pClsRule->sTagAct.vidAction = fib.cs_vid_act;
    pClsRule->sTagAct.priAction = fib.cs_pri_act;
    pClsRule->sTagAct.deiAction = fib.cscfi_keep;
    pClsRule->sTagAct.assignedVid = fib.cs_vid;
    pClsRule->sTagAct.assignedPri = fib.cs_pri;

    pClsRule->cTagAct.tagAction = fib.cact;
    pClsRule->cTagAct.vidAction = fib.cvid_act;
    pClsRule->cTagAct.priAction = fib.cpri_act;
    pClsRule->cTagAct.deiAction = fib.ccfi_keep;
    pClsRule->cTagAct.assignedVid = fib.cvid;
    pClsRule->cTagAct.assignedPri = fib.cpri;

    pClsRule->grpPolAct = fib.grpid_act;
    pClsRule->grpPolId = fib.cf_grpid;

    pClsRule->flowAct = fib.flowid_act;
    pClsRule->flowId = fib.cf_flowid;

    pClsRule->fwdAct = fib.fwd_act;
    pClsRule->fwdId = fib.cf_fwd_id;

    pClsRule->cosAct = fib.cf_cos_act;
    pClsRule->cos = fib.cf_cos;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_pon_sid_set
 * Description:
 *      Set pon sid info
 * Input:
 *      sid     - Stream ID
 *      sidInfo - Stream ID information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_pon_sid_set(uint16 sid,rt_ponmisc_sidInfo_t sidInfo)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    dal_9607f_cls_rule_db_t *pEntryData= NULL;
#endif
    /* check Init status */
    RT_INIT_CHK(cls_init);

    if(sid >= RTL9607F_CLS_SID_MAX)
        return RT_ERR_FAILED;
    memcpy(&clsSidInfo[sid],&sidInfo,sizeof(rt_ponmisc_sidInfo_t));

#if defined(CONFIG_SDK_KERNEL_LINUX)
    __CF_US_DB_LOCK();
    list_for_each_safe(pEntry, pTmpEntry, &usRuleHead)
    {
        pEntryData = list_entry(pEntry, dal_9607f_cls_rule_db_t, list);
        if(pEntryData->rule.flowId == sid)
            _9607f_cls_rule_set(&(pEntryData->rule));
    }
    __CF_US_DB_UNLOCK();
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rt_rtl9607f_cls_fwdPort_set
 * Description:
 *      Set veip port default forward port
 * Input:
 *      port    - port ID
 *      fwdPort - default forward port in VEIP mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_fwdPort_set(rt_port_t port, rt_port_t fwdPort)
{
    int32   ret = RT_ERR_OK;
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;

    /* check Init status */
    RT_INIT_CHK(cls_init);

    if(port >= RTK_MAX_NUM_OF_PORTS)
    {
        return RT_ERR_PORT_ID;
    }

    portFwdPort[port] = fwdPort;
    memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
    ilpbCfgMask.s.cf_sel_wan_en = 1;
    memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
    if(fwdPort == AAL_LPORT_L3_WAN)
        ilpbCfg.cf_sel_wan_en = 1;
    else
        ilpbCfg.cf_sel_wan_en = 0;
    aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
    if(portType[port] == RTL9607F_CLS_PORT_TYPE_VEIP)
        ret = dal_rt_rtl9607f_cls_veipPortRule_set(port, ENABLED);

#if defined(CONFIG_SDK_RTL8198X)
    dal_rtl9607f_ca_specpkt_ctrl_ldpid_update(port);
#endif

    return ret;
}


/* Function Name:
 *      dal_rt_rtl9607f_cls_vlan_paser_set
 * Description:
 *      Set VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 *      data     - value of vlan parser attribute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_vlan_paser_set(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 data)
{
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;
    aal_l2_vlan_default_cfg_mask_t vlanMask;
    aal_l2_vlan_default_cfg_t vlan;

    memset(&ilpbCfg, 0 ,sizeof(aal_ilpb_cfg_t));
    memset(&ilpbCfgMask, 0 ,sizeof(aal_ilpb_cfg_msk_t));
    memset(&vlan, 0 ,sizeof(aal_l2_vlan_default_cfg_t));
    memset(&vlanMask, 0 ,sizeof(aal_l2_vlan_default_cfg_mask_t));

    switch(type)
    {
        case RT_CLS_VLAN_STAG_MATCH:
            ilpbCfgMask.s.s_tpid_match = 1;
            ilpbCfg.s_tpid_match = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_CTAG_MATCH:
            ilpbCfgMask.s.c_tpid_match = 1;
            ilpbCfg.c_tpid_match = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_OUTER_STAG:
            ilpbCfgMask.s.top_s_tpid_enable = 1;
            ilpbCfg.top_s_tpid_enable = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_INNER_STAG:
            ilpbCfgMask.s.inner_s_tpid_enable = 1;
            ilpbCfg.inner_s_tpid_enable = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_OUTER_CTAG:
            ilpbCfgMask.s.top_c_tpid_enable = 1;
            ilpbCfg.top_c_tpid_enable = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_INNER_CTAG:
            ilpbCfgMask.s.inner_c_tpid_enable = 1;
            ilpbCfg.inner_c_tpid_enable = data;
            aal_port_ilpb_cfg_set(0, port, ilpbCfgMask, &ilpbCfg);
            break;
        case RT_CLS_VLAN_STAG_TPID_A:
            vlanMask.s.tx_tpid0 = 1;
            vlan.tx_tpid0 = data;
            aal_l2_vlan_default_cfg_set(0, vlanMask, &vlan);
            break;
        case RT_CLS_VLAN_STAG_TPID_B:
            vlanMask.s.tx_tpid1 = 1;
            vlan.tx_tpid1 = data;
            aal_l2_vlan_default_cfg_set(0, vlanMask, &vlan);
            break;
        case RT_CLS_VLAN_USER_STAG_TPID:
            vlanMask.s.cmp_tpid_svlan = 1;
            vlan.cmp_tpid_svlan = data;
            aal_l2_vlan_default_cfg_set(0, vlanMask, &vlan);
            break;
        case RT_CLS_VLAN_USER_CTAG_TPID:
            vlanMask.s.cmp_tpid_cvlan = 1;
            vlan.cmp_tpid_cvlan = data;
            aal_l2_vlan_default_cfg_set(0, vlanMask, &vlan);
            break;
        default:
            return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_cls_vlan_paser_get
 * Description:
 *      Get VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 * Output:
 *      pData     - value of vlan parser attribute
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

int32 dal_rt_rtl9607f_cls_vlan_paser_get(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 *pData)
{
    aal_ilpb_cfg_t ilpbCfg;
    aal_l2_vlan_default_cfg_t vlan;
    ca_status_t ret;

    memset(&ilpbCfg, 0 ,sizeof(aal_ilpb_cfg_t));
    memset(&vlan, 0 ,sizeof(aal_l2_vlan_default_cfg_t));

    ret = aal_port_ilpb_cfg_get(0, port, &ilpbCfg);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    ret = aal_l2_vlan_default_cfg_get(0, &vlan);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    switch(type)
    {
        case RT_CLS_VLAN_STAG_MATCH:
            *pData = ilpbCfg.s_tpid_match;
            break;
        case RT_CLS_VLAN_CTAG_MATCH:
            *pData = ilpbCfg.c_tpid_match;
            break;
        case RT_CLS_VLAN_OUTER_STAG:
            *pData = ilpbCfg.top_s_tpid_enable;
            break;
        case RT_CLS_VLAN_INNER_STAG:
            *pData = ilpbCfg.inner_s_tpid_enable;
            break;
        case RT_CLS_VLAN_OUTER_CTAG:
            *pData = ilpbCfg.top_c_tpid_enable;
            break;
        case RT_CLS_VLAN_INNER_CTAG:
            *pData = ilpbCfg.inner_c_tpid_enable;
            break;
        case RT_CLS_VLAN_STAG_TPID_A:
            *pData = vlan.tx_tpid0;
            break;
        case RT_CLS_VLAN_STAG_TPID_B:
            *pData = vlan.tx_tpid1;
            break;
        case RT_CLS_VLAN_USER_STAG_TPID:
            *pData = vlan.cmp_tpid_svlan;
            break;
        case RT_CLS_VLAN_USER_CTAG_TPID:
            *pData = vlan.cmp_tpid_cvlan;
            break;
        default:
            return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

