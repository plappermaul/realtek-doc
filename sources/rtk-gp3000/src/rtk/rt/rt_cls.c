/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : Definition of classify rule global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) classify rule add/delete
 *
 */
 

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <hal/chipdef/chip.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_cls.h>
#include <rtk/rt/rt_qos.h>
#include <rtk/classify.h>
#include <rtk/svlan.h>

#if defined(CONFIG_SDK_KERNEL_LINUX)
#ifndef CONFIG_LUNA_G3_SERIES
#include <osal/print.h>
#include <linux/list.h>
#include <linux/slab.h>
#endif
#endif
/*
 * Symbol Definition
 */
#ifndef CONFIG_LUNA_G3_SERIES
#define RT_CLS_RULE_RESERVED_NUM 64
#define RT_CLS_RULE_NOT_IN_USED -1
#define RT_CLS_RULE_PORT_NOT_USED -1
#define RT_CLS_RULE_PORT_IGNORE -2
#define RT_CLS_DEFAULT_CTAG_TPID 0x8100
#define RT_CLS_DEFAULT_STAG_TPID 0x88a8
#define RT_CLS_IPV4_ETHERTYPE 0x0800
#define RT_CLS_IPV6_ETHERTYPE 0x86DD
#define OMCI_XLATEVID_NOT_USED 0x00000000
#define OMCI_XLATEVID_ONLY_ONE 0x00000001
#define OMCI_XLATEVID_TWO_MORE 0x00000002
#define OMCI_XLATEVID_UNTAG    0xFFFFFFFD
#define OMCI_XLATEVID_RULE_SP2C 1
#define OMCI_XLATEVID_RULE_LUT 2
#define OMCI_XLATEVID_INDEX_SHIFT_BASE 100

#ifndef CONFIG_SDK_KERNEL_LINUX
#define printk(fmt, arg...) ;
#endif
 
typedef struct rt_cls_rule_index_s
{
    int32 index;
    rt_cls_dir_t dir;
    uint32 ruleIndex;
    uint32 rulePriority;
    int32 port;
}rt_cls_rule_index_t;

typedef struct {
    rt_cls_rule_t rule;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} rt_cls_rule_db_t;

typedef struct {
    uint32 index;
    uint32 type;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} rt_cls_vlan_rule_index_t;

typedef struct {
    uint8 port;
    uint32 vlan;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head list;
#endif
} rt_cls_sp2c_vlan_t;

typedef struct
{
    unsigned int vid;
    unsigned int xlateMode;
} rt_cls_vlan_xlate_info_t;

uint8 initial=0;
uint8 doubleCtag=0;
rt_cls_rule_index_t *clsRuleUsage;
#if defined(CONFIG_SDK_KERNEL_LINUX)
struct list_head dsRuleHead;
struct list_head usRuleHead;
struct list_head vlanRuleIndexHead;
struct list_head sp2cVlanHead;
#endif
rtk_classify_field_t flowField,cTagField,sTagField,ethertypeField,interPriField;
rtk_classify_field_t vidField,priField,deiField,innerVlanField,uniField,ipv4Field,ipv6Field;

static int32 rt_rtk_cls_rule_add(rt_cls_rule_t *pClsRule,int first);
static void rt_rtk_cls_rule_adjust(void);

/*
 * Function Declaration
 */

/* Module Name    : Classify rule     */
/* Sub-module Name: Add/delete classify rule */

static void rt_cls_initial(void)
{
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    int i=0;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    clsRuleUsage=(rt_cls_rule_index_t *)kzalloc(sizeof(rt_cls_rule_index_t)*ruleMax, GFP_KERNEL);
#endif
    memset(clsRuleUsage,0,sizeof(rt_cls_rule_index_t)*ruleMax);
    for(i=0;i<ruleMax;i++)
    {
        clsRuleUsage[i].index=RT_CLS_RULE_NOT_IN_USED;
        clsRuleUsage[i].port=RT_CLS_RULE_PORT_NOT_USED;
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    INIT_LIST_HEAD(&dsRuleHead);
    INIT_LIST_HEAD(&usRuleHead);
    INIT_LIST_HEAD(&vlanRuleIndexHead);
    INIT_LIST_HEAD(&sp2cVlanHead);
#endif
    initial=1;
}

static int32 rt_cls_rule_index_get(uint32 ruleIndex, rt_cls_dir_t dir,int *index, int32 port,uint32 rulePriority, int isReversedRule)
{
    int i=0,j=0,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    rt_cls_rule_index_t writeData, tempData;
    memset(&writeData,0,sizeof(rt_cls_rule_index_t));
    memset(&tempData,0,sizeof(rt_cls_rule_index_t));

    if(0 == initial)
        rt_cls_initial();
    
    if(isReversedRule)
    {
        min = 0;
        max = RT_CLS_RULE_RESERVED_NUM;
    }
    else
    {
        min = RT_CLS_RULE_RESERVED_NUM;
        max = ruleMax;
    }
    
    for(i=min;i<max;i++)
    {
        if((RT_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == ruleIndex) && (clsRuleUsage[i].dir == dir) && (port == clsRuleUsage[i].port))
        {
            *index=clsRuleUsage[i].index;
            return RT_ERR_OK;
        }
    }
    /*Not exist, find an empty index*/
    for(i=min;i<max;i++)
    {
        /*High priority rule, insert the rule before low priority rule*/
        if(RT_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index && clsRuleUsage[i].rulePriority < rulePriority)
        {
            writeData.dir = dir;
            writeData.ruleIndex = ruleIndex;
            writeData.rulePriority = rulePriority;
            writeData.port = port;
            *index = i;
            for(j=i;j<max;j++)
            {
                if(RT_CLS_RULE_NOT_IN_USED == clsRuleUsage[j].index )
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

                    memcpy(&writeData,&tempData,sizeof(rt_cls_rule_index_t));
                }
            }
            rt_rtk_cls_rule_adjust();
            if(j == max)
               return RT_ERR_FAILED;
            else
                return RT_ERR_OK;
        }
        if(RT_CLS_RULE_NOT_IN_USED == clsRuleUsage[i].index)
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

static int32 rt_cls_rule_index_find(uint32 ruleIndex, rt_cls_dir_t dir,int *index, int32 port,int isReversedRule)
{
    int i=0,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();

    if(0 == initial)
        rt_cls_initial();
    if(isReversedRule)
    {
        min = 0;
        max = RT_CLS_RULE_RESERVED_NUM;
    }
    else
    {
        min = RT_CLS_RULE_RESERVED_NUM;
        max = ruleMax;
    }
    for(i=min;i<max;i++)
    {
        if((RT_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == ruleIndex) && (clsRuleUsage[i].dir == dir) )
        {
            if(port == RT_CLS_RULE_PORT_IGNORE || port == clsRuleUsage[i].port)
            {
                *index=clsRuleUsage[i].index;
                return RT_ERR_OK;
            }
        }
    }
    return RT_ERR_FAILED;
}

static int32 rt_cls_rule_index_del(uint32 ruleIndex, rt_cls_dir_t dir,int isReversedRule)
{
    int i=0,min,max;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();

    if(0 == initial)
        rt_cls_initial();
    if(isReversedRule)
    {
        min = 0;
        max = RT_CLS_RULE_RESERVED_NUM;
    }
    else
    {
        min = RT_CLS_RULE_RESERVED_NUM;
        max = ruleMax;
    }
    for(i=min;i<max;i++)
    {
        if((RT_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == ruleIndex) && (clsRuleUsage[i].dir == dir))
        {
            clsRuleUsage[i].index = RT_CLS_RULE_NOT_IN_USED;
            clsRuleUsage[i].port=RT_CLS_RULE_PORT_NOT_USED;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}

static void rt_rtk_cls_rule_adjust(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    rt_cls_rule_db_t *pEntryData= NULL;
    rtk_classify_field_t flowFieldTmp,cTagFieldTmp,sTagFieldTmp,ethertypeFieldTmp,interPriFieldTmp;
    rtk_classify_field_t vidFieldTmp,priFieldTmp,deiFieldTmp,innerVlanFieldTmp,uniFieldTmp,ipv4FieldTmp,ipv6FieldTmp;

    /*Backup filter field setting*/
    memcpy(&flowFieldTmp,&flowField,sizeof(rtk_classify_field_t));
    memcpy(&cTagFieldTmp,&cTagField,sizeof(rtk_classify_field_t));
    memcpy(&sTagFieldTmp,&sTagField,sizeof(rtk_classify_field_t));
    memcpy(&ethertypeFieldTmp,&ethertypeField,sizeof(rtk_classify_field_t));
    memcpy(&interPriFieldTmp,&interPriField,sizeof(rtk_classify_field_t));
    memcpy(&vidFieldTmp,&vidField,sizeof(rtk_classify_field_t));
    memcpy(&priFieldTmp,&priField,sizeof(rtk_classify_field_t));
    memcpy(&deiFieldTmp,&deiField,sizeof(rtk_classify_field_t));
    memcpy(&innerVlanFieldTmp,&innerVlanField,sizeof(rtk_classify_field_t));
    memcpy(&uniFieldTmp,&uniField,sizeof(rtk_classify_field_t));
    memcpy(&ipv4FieldTmp,&ipv4Field,sizeof(rtk_classify_field_t));
    memcpy(&ipv6FieldTmp,&ipv6Field,sizeof(rtk_classify_field_t));

    list_for_each_safe(pEntry, pTmpEntry, &dsRuleHead)
    {
        pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
        rt_rtk_cls_rule_add(&(pEntryData->rule), 0);
    }

    list_for_each_safe(pEntry, pTmpEntry, &usRuleHead)
    {
        pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
        rt_rtk_cls_rule_add(&(pEntryData->rule), 0);
    }

    /*Recovery filter field setting*/
    memcpy(&flowField,&flowFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&cTagField,&cTagFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&sTagField,&sTagFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&ethertypeField,&ethertypeFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&interPriField,&interPriFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&vidField,&vidFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&priField,&priFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&deiField,&deiFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&innerVlanField,&innerVlanFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&uniField,&uniFieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&ipv4Field,&ipv4FieldTmp,sizeof(rtk_classify_field_t));
    memcpy(&ipv6Field,&ipv6FieldTmp,sizeof(rtk_classify_field_t));
#endif
}

static int32 rt_rtk_cls_us_action_set(rtk_classify_cfg_t *pEntry, rt_cls_rule_t *pClsRule)
{
    int8 isCsTag = 0;
    int8 filterTag = pClsRule->filterRule.outerTagIf+pClsRule->filterRule.innerTagIf;
    int8 processTag = filterTag;
    
    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_TRANSPARENT;
    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_TRANSPARENT;
    pEntry->act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
    pEntry->act.usAct.sidQid = pClsRule->flowId;

    if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
    {
        pEntry->act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
        pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_VID_BIT)
        {
            pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
            pEntry->act.usAct.cTagVid = pClsRule->filterRule.innerTagVid;
        }
        else
        {
            pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
        }
        
        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT)
        {
            pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
            pEntry->act.usAct.cTagPri = pClsRule->filterRule.innerTagPri;
        }
        else
        {
            pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
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
            if(pClsRule->outerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    isCsTag=1;
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->outerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID;
                else
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
            }
            break;
        case RT_CLS_TAG_REMOVE:
            processTag--;
            if(pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_CTAG_TPID)
            {
                pEntry->act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
            }
            else
            {
                isCsTag=1;
                pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
            }
            break;
        default:
            printk("[%s] %d Unknown outer tag action %d ruleIndex:%d dir:%d"
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
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
                    pEntry->act.usAct.sTagVid = pClsRule->outerTagAct.assignedVid;
                }
                else
                {
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
                    pEntry->act.usAct.cTagVid = pClsRule->outerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                break;
            default:
                printk("[%s] %d Unknown outer vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->outerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }
        
        switch(pClsRule->outerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
                    pEntry->act.usAct.sTagPri = pClsRule->outerTagAct.assignedPri;
                }
                else
                {
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
                    pEntry->act.usAct.cTagPri = pClsRule->outerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                break;
            default:
                printk("[%s] %d Unknown outer priority action %d ruleIndex:%d dir:%d"
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
            if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    isCsTag=1;
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID;
                }
                else
                {
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
                }
            }
            break;
        case RT_CLS_TAG_MODIFY:
            if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(processTag == 2)
                {
                    isCsTag=1;
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID;
                }
                else
                {
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
                }
            }
            if(processTag == 1 && pClsRule->innerTagAct.assignedTpid != pClsRule->filterRule.innerTagTpid && (pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT || pClsRule->outerTagAct.tagAction == RT_CLS_TAG_REMOVE))
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
                else
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
            }
            if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
            break;
        case RT_CLS_TAG_REMOVE:
            if(processTag == 1)
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    if(doubleCtag == 1 && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                        pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
                }
                else
                {
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
                }
            }
            else
            {
                if(pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_CTAG_TPID)
                    pEntry->act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
                else
                    pEntry->act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
            }
            break;
        default:
            printk("[%s] %d Unknown inner tag action %d ruleIndex:%d dir:%d"
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
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
                    pEntry->act.usAct.sTagVid = pClsRule->innerTagAct.assignedVid;
                }
                else
                {
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
                    pEntry->act.usAct.cTagVid = pClsRule->innerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(isCsTag)
                    pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_NOP;
                else
                    pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_NOP;
                
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                        pEntry->act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                    else
                        pEntry->act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
                }
                break;
            default:
                printk("[%s] %d Unknown inner vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }
        
        switch(pClsRule->innerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
                    pEntry->act.usAct.sTagPri = pClsRule->innerTagAct.assignedPri;
                }
                else
                {
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
                    pEntry->act.usAct.cTagPri = pClsRule->innerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
                else
                    pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                        pEntry->act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                    else
                        pEntry->act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
                }
                break;
            default:
                printk("[%s] %d Unknown inner priority action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagPriAction, pClsRule->index, pClsRule->direction);
                break;
        }
    }
    
    return RT_ERR_OK;
}

static int32 rt_rtk_cls_ds_action_set(rtk_classify_cfg_t *pEntry, rt_cls_rule_t *pClsRule)
{
    int8 isCsTag = 0;
    int8 filterTag = pClsRule->filterRule.outerTagIf+pClsRule->filterRule.innerTagIf;
    int8 processTag = filterTag;
    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_TRANSPARENT;
    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_TRANSPARENT;
    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    
    if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
    {
        pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
        pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_VID_BIT)
        {
            pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
            pEntry->act.dsAct.cTagVid = pClsRule->filterRule.innerTagVid;
        }
        else
        {
            pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
        }
        
        if(pClsRule->filterRule.filterMask & RT_CLS_FILTER_INNER_PRI_BIT)
        {
            pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
            pEntry->act.dsAct.cTagPri = pClsRule->filterRule.innerTagPri;
        }
        else
        {
            pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
        }
    }
    
    if(pClsRule->ingressPortMask != 0)
    {
        pEntry->act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
        pEntry->act.dsAct.uniMask.bits[0] = pClsRule->ingressPortMask;
    }

    switch(pClsRule->outerTagAct.tagAction)
    {
        case RT_CLS_TAG_TRANSPARENT:
            break;
        case RT_CLS_TAG_MODIFY:
            processTag--;
            /*fall through*/
        case RT_CLS_TAG_TAGGING:
            if(pClsRule->outerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    isCsTag=1;
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->outerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
                else
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
            }
            break;
        case RT_CLS_TAG_REMOVE:
            processTag--;
            if(pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_CTAG_TPID)
            {
                pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
            }
            else
            {
                isCsTag=1;
                pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
            }
            break;
        default:
            printk("[%s] %d Unknown outer tag action %d ruleIndex:%d dir:%d"
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
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
                    pEntry->act.dsAct.sTagVid = pClsRule->outerTagAct.assignedVid;
                }
                else
                {
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
                    pEntry->act.dsAct.cTagVid = pClsRule->outerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(isCsTag)
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                break;
            default:
                printk("[%s] %d Unknown outer vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->outerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }
        
        switch(pClsRule->outerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
                    pEntry->act.dsAct.sTagPri = pClsRule->outerTagAct.assignedPri;
                }
                else
                {
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
                    pEntry->act.dsAct.cTagPri = pClsRule->outerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                break;
            default:
                printk("[%s] %d Unknown outer priority action %d ruleIndex:%d dir:%d"
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
            if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    isCsTag=1;
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
                }
                else
                {
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
                }
            }
            break;
        case RT_CLS_TAG_MODIFY:
            if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                if(processTag == 2)
                {
                    isCsTag=1;
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
                }
                else
                {
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
                }
            }
            else
            {
                isCsTag=1;
                if(pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_STAG_TPID)
                {
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
                }
                else
                {
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
                }
            }
            if(processTag == 1 && pClsRule->innerTagAct.assignedTpid != pClsRule->filterRule.innerTagTpid && (pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT || pClsRule->outerTagAct.tagAction == RT_CLS_TAG_REMOVE))
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
                else
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
            }
            if(doubleCtag == 1 && filterTag == 1 && pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->innerTagAct.assignedTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
            break;
        case RT_CLS_TAG_REMOVE:
            if(processTag == 1)
            {
                if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
                {
                    if(doubleCtag == 1 && pClsRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
                        pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
                }
                else
                {
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
                }
            }
            else
            {
                if(pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_CTAG_TPID)
                    pEntry->act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
                else
                    pEntry->act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
            }
            break;
        default:
            printk("[%s] %d Unknown inner tag action %d ruleIndex:%d dir:%d"
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
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
                    pEntry->act.dsAct.sTagVid = pClsRule->innerTagAct.assignedVid;
                }
                else
                {
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
                    pEntry->act.dsAct.cTagVid = pClsRule->innerTagAct.assignedVid;
                }
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_VID_TRANSPARENT:
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                        pEntry->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                    else
                        pEntry->act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
                }
                break;
            default:
                printk("[%s] %d Unknown inner vlan action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagVidAction, pClsRule->index, pClsRule->direction);
                break;
        }
        
        switch(pClsRule->innerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                if(isCsTag)
                {
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
                    pEntry->act.dsAct.sTagPri = pClsRule->innerTagAct.assignedPri;
                }
                else
                {
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
                    pEntry->act.dsAct.cTagPri = pClsRule->innerTagAct.assignedPri;
                }
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                if(isCsTag)
                    pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
                else
                    pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
                break;
            case RT_CLS_PRI_TRANSPARENT:
                if(processTag == 1)
                {
                    if(isCsTag)
                    {
                        if(filterTag == 2)
                            pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                    }
                    else
                    {
                        if(filterTag == 2)
                            pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
                        else
                            pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                    }
                }
                else
                {
                    if(isCsTag)
                        pEntry->act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                    else
                        pEntry->act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
                }
                break;
            default:
                printk("[%s] %d Unknown inner priority action %d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, pClsRule->innerTagAct.tagPriAction, pClsRule->index, pClsRule->direction);
                break;
        }
    }

    if(pClsRule->dsPriAct == RT_CLS_DS_PRI_ACT_ASSIGN && pClsRule->dsPri <= 7)
    {
        pEntry->act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
        pEntry->act.dsAct.cfPri = (uint8)pClsRule->dsPri;
    }
    
    return RT_ERR_OK;
}

static int32 rt_rtk_cls_filter_set(rtk_classify_cfg_t *pEntry, rt_cls_rule_t *pClsRule)
{
    uint16 innerVlan=0;
    uint16 innerVlanMask=0;
    int ctag=0, stag=0, ctagValue = 0,stagValue = 0, i;
    int32 ret=RT_ERR_FAILED;
    rt_qos_dscp2Pbit_t dscp2pbit;
    uint32 interPri = 0;

    if(pClsRule->direction == RT_CLS_DIR_US)
        pEntry->direction = CLASSIFY_DIRECTION_US;
    else
        pEntry->direction = CLASSIFY_DIRECTION_DS;
    pEntry->valid = ENABLED;
    pEntry->templateIdx = 2;

    /*filter*/
    if(RT_CLS_FILTER_OUTER_TAGIf_BIT & pClsRule->filterRule.filterMask && pClsRule->filterRule.outerTagIf == 1)
    {
        if(pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID && pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_CTAG_TPID)
        {
            ctag=1;
        }
        else
        {
            stag=1;
            /*change default stag tpid*/
            if(pClsRule->filterRule.outerTagTpid != RT_CLS_DEFAULT_STAG_TPID)
            {
                rtk_svlan_tpidEnable_set(1,ENABLED);
                rtk_svlan_tpidEntry_set(1,pClsRule->filterRule.outerTagTpid);
            }
            if(doubleCtag == 0 && pClsRule->filterRule.outerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
            {
                doubleCtag = 1;
                rt_rtk_cls_rule_adjust();
            }
        }
    }
    
    if(RT_CLS_FILTER_INNER_TAGIf_BIT & pClsRule->filterRule.filterMask && pClsRule->filterRule.innerTagIf == 1)
    {
        if(pClsRule->filterRule.innerTagTpid == RT_CLS_DEFAULT_CTAG_TPID)
        {
            ctag=1;
        }
        else
        {
            stag=1;
            /*change default stag tpid*/
            if(pClsRule->filterRule.innerTagTpid != RT_CLS_DEFAULT_STAG_TPID)
            {
                rtk_svlan_tpidEnable_set(1,ENABLED);
                rtk_svlan_tpidEntry_set(1,pClsRule->filterRule.innerTagTpid);
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

        memset(&cTagField,0,sizeof(rtk_classify_field_t));
        cTagField.fieldType = CLASSIFY_FIELD_IS_CTAG;
        cTagField.classify_pattern.isCtag.value = ctagValue;
        cTagField.classify_pattern.isCtag.mask = 1;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &cTagField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;

        memset(&sTagField,0,sizeof(rtk_classify_field_t));
        sTagField.fieldType = CLASSIFY_FIELD_IS_STAG;
        sTagField.classify_pattern.isStag.value = stagValue;
        sTagField.classify_pattern.isStag.mask = 1;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &sTagField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;

    }

    if(RT_CLS_FILTER_FLOWID_BIT & pClsRule->filterRule.filterMask)
    {
        memset(&flowField,0,sizeof(rtk_classify_field_t));
        flowField.fieldType = CLASSIFY_FIELD_TOS_DSIDX;
        flowField.classify_pattern.tosDsidx.value = pClsRule->filterRule.flowId;
        flowField.classify_pattern.tosDsidx.mask = 0x7f;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &flowField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    
    if(RT_CLS_FILTER_OUTER_VID_BIT & pClsRule->filterRule.filterMask)
    {
        memset(&vidField,0,sizeof(rtk_classify_field_t));
        vidField.fieldType = CLASSIFY_FIELD_TAG_VID;
        vidField.classify_pattern.tagVid.value = pClsRule->filterRule.outerTagVid;
        vidField.classify_pattern.tagVid.mask = 0xfff;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &vidField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    
    if(RT_CLS_FILTER_OUTER_PRI_BIT & pClsRule->filterRule.filterMask)
    {
        memset(&priField,0,sizeof(rtk_classify_field_t));
        priField.fieldType = CLASSIFY_FIELD_TAG_PRI;
        priField.classify_pattern.tagPri.value = pClsRule->filterRule.outerTagPri;
        priField.classify_pattern.tagPri.mask = 7;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &priField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    
    if(RT_CLS_FILTER_OUTER_DEI_BIT & pClsRule->filterRule.filterMask)
    {
        memset(&deiField,0,sizeof(rtk_classify_field_t));
        deiField.fieldType = CLASSIFY_FIELD_DEI;
        deiField.classify_pattern.dei.value = pClsRule->filterRule.outerTagDei;
        deiField.classify_pattern.dei.mask = 1;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &deiField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    
    if(RT_CLS_FILTER_INNER_VID_BIT & pClsRule->filterRule.filterMask)
    {
        
        if(pClsRule->filterRule.outerTagIf == 1)
        {
            innerVlan = pClsRule->filterRule.innerTagVid;
            innerVlanMask = 0xfff;
        }
        else
        {
            memset(&vidField,0,sizeof(rtk_classify_field_t));
            vidField.fieldType = CLASSIFY_FIELD_TAG_VID;
            vidField.classify_pattern.tagVid.value = pClsRule->filterRule.innerTagVid;
            vidField.classify_pattern.tagVid.mask = 0xfff;
            if (NULL == RT_MAPPER->classify_field_add)
               return RT_ERR_DRIVER_NOT_FOUND;
            RTK_API_LOCK();
            ret = RT_MAPPER->classify_field_add(pEntry, &vidField);
            RTK_API_UNLOCK();
            if(ret != RT_ERR_OK)
                return ret;
        }
    }
    
    if(RT_CLS_FILTER_INNER_PRI_BIT & pClsRule->filterRule.filterMask)
    {
        
        if(pClsRule->filterRule.outerTagIf == 1)
        {
            innerVlan |= (pClsRule->filterRule.innerTagPri << 13);
            innerVlanMask |= (0x7 << 13);
        }
        else
        {
            memset(&priField,0,sizeof(rtk_classify_field_t));
            priField.fieldType = CLASSIFY_FIELD_TAG_PRI;
            priField.classify_pattern.tagPri.value = pClsRule->filterRule.innerTagPri;
            priField.classify_pattern.tagPri.mask = 7;
            if (NULL == RT_MAPPER->classify_field_add)
               return RT_ERR_DRIVER_NOT_FOUND;
            RTK_API_LOCK();
            ret = RT_MAPPER->classify_field_add(pEntry, &priField);
            RTK_API_UNLOCK();
            if(ret != RT_ERR_OK)
                return ret;
        }
    }
    
    if(RT_CLS_FILTER_INNER_DEI_BIT & pClsRule->filterRule.filterMask)
    {
        
        if(pClsRule->filterRule.outerTagIf == 1)
        {
            innerVlan |= (pClsRule->filterRule.innerTagDei << 12);
            innerVlanMask |= (0x1 << 12);
        }
        else
        {
            memset(&deiField,0,sizeof(rtk_classify_field_t));
            deiField.fieldType = CLASSIFY_FIELD_DEI;
            deiField.classify_pattern.dei.value = pClsRule->filterRule.innerTagDei;
            deiField.classify_pattern.dei.mask = 1;
            if (NULL == RT_MAPPER->classify_field_add)
               return RT_ERR_DRIVER_NOT_FOUND;
            RTK_API_LOCK();
            ret = RT_MAPPER->classify_field_add(pEntry, &deiField);
            RTK_API_UNLOCK();
            if(ret != RT_ERR_OK)
                return ret;
        }
    }
    
    if(innerVlanMask != 0)
    {
        memset(&innerVlanField,0,sizeof(rtk_classify_field_t));
        innerVlanField.fieldType = CLASSIFY_FIELD_INNER_VLAN;
        innerVlanField.classify_pattern.innerVlan.value = innerVlan;
        innerVlanField.classify_pattern.innerVlan.mask = innerVlanMask;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &innerVlanField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    
    if(RT_CLS_FILTER_ETHERTYPE & pClsRule->filterRule.filterMask && !(RT_CLS_FILTER_DSCP_BIT & pClsRule->filterRule.filterMask))
    {
        if(innerVlanMask != 0)
        {
            printk("[%s] %d Not support filter both ethertype and inner vlan, ignore inner vlan filter"
                ,__FUNCTION__, __LINE__);
        }
        pEntry->templateIdx = 1;
        memset(&ethertypeField,0,sizeof(rtk_classify_field_t));
        ethertypeField.fieldType = CLASSIFY_FIELD_ETHERTYPE;
        ethertypeField.classify_pattern.etherType.value = pClsRule->filterRule.etherType;
        ethertypeField.classify_pattern.etherType.mask = 0xffff;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &ethertypeField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }

    if(RT_CLS_FILTER_DSCP_BIT & pClsRule->filterRule.filterMask)
    {
        if(innerVlanMask != 0)
        {
            printk("[%s] %d Not support filter both DSCP and inner vlan, ignore inner vlan filter"
                ,__FUNCTION__, __LINE__);
        }
        if(RT_CLS_FILTER_ETHERTYPE & pClsRule->filterRule.filterMask)
        {
            if(pClsRule->filterRule.etherType == RT_CLS_IPV4_ETHERTYPE)
            {
                memset(&ipv4Field,0,sizeof(rtk_classify_field_t));
                ipv4Field.fieldType = CLASSIFY_FIELD_IPV4;
                ipv4Field.classify_pattern.ipv4.value = 1;
                ipv4Field.classify_pattern.ipv4.mask = 1;
                if (NULL == RT_MAPPER->classify_field_add)
                   return RT_ERR_DRIVER_NOT_FOUND;
                RTK_API_LOCK();
                ret = RT_MAPPER->classify_field_add(pEntry, &ipv4Field);
                RTK_API_UNLOCK();
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else if(pClsRule->filterRule.etherType == RT_CLS_IPV6_ETHERTYPE)
            {
                memset(&ipv6Field,0,sizeof(rtk_classify_field_t));
                ipv6Field.fieldType = CLASSIFY_FIELD_IPV6;
                ipv6Field.classify_pattern.ipv6.value = 1;
                ipv6Field.classify_pattern.ipv6.mask = 1;
                if (NULL == RT_MAPPER->classify_field_add)
                   return RT_ERR_DRIVER_NOT_FOUND;
                RTK_API_LOCK();
                ret = RT_MAPPER->classify_field_add(pEntry, &ipv6Field);
                RTK_API_UNLOCK();
                if(ret != RT_ERR_OK)
                    return ret;
            }
            else
            {
                printk("[%s] %d Not support filter both DSCP and ethertype 0x%04x, ignore ethertype filter"
                ,__FUNCTION__, __LINE__,pClsRule->filterRule.etherType);
            }
        }
        memset(&dscp2pbit,0,sizeof(rt_qos_dscp2Pbit_t));
        rt_qos_dscp2pbit_get(&dscp2pbit);
        for(i=0;i<RT_MAX_DCSP_NUM;i++)
        {
            if((pClsRule->filterRule.dscp[i/8] >> i%8) & 0x1)
            {
                interPri=dscp2pbit.dscp[i];
                break;
            }
        }
        pEntry->templateIdx = 0;
        memset(&interPriField,0,sizeof(rtk_classify_field_t));
        interPriField.fieldType = CLASSIFY_FIELD_INTER_PRI;
        interPriField.classify_pattern.interPri.value = interPri;
        interPriField.classify_pattern.interPri.mask = 0x7;
        if (NULL == RT_MAPPER->classify_field_add)
           return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_MAPPER->classify_field_add(pEntry, &interPriField);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
            return ret;
    }
    return RT_ERR_OK;
}

static void vlan_aggregate_sp2c_delAll(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *cur_next = NULL;
    struct list_head *cur_tmp = NULL;
    rt_cls_sp2c_vlan_t *cur = NULL;
    int32 ret;
    list_for_each_safe(cur_next, cur_tmp, &sp2cVlanHead)
    {
        cur = list_entry(cur_next, rt_cls_sp2c_vlan_t, list);
        ret = rtk_svlan_sp2c_del(cur->vlan, cur->port);
        if(ret != RT_ERR_OK)
            printk("[%s]Delete sp2c vlan fail, port %d, vlan %d\n",__FUNCTION__,cur->port,cur->vlan);
        list_del(&cur->list);
        kfree(cur);
    }
#endif
    
}

static void vlan_aggregate_cf_delAll(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32 index;
    struct list_head *cur_next = NULL;
    struct list_head *cur_tmp = NULL;
    rt_cls_vlan_rule_index_t *cur  = NULL;
    int32 ret=RT_ERR_FAILED;
    
    list_for_each_safe(cur_next, cur_tmp, &vlanRuleIndexHead)
    {
        cur = list_entry(cur_next, rt_cls_vlan_rule_index_t, list);
        
        if(rt_cls_rule_index_find(cur->index+OMCI_XLATEVID_INDEX_SHIFT_BASE*cur->type, RT_CLS_DIR_DS, &index,RT_CLS_RULE_PORT_IGNORE, 1) == RT_ERR_OK)
        {
            ret = rtk_classify_cfgEntry_del(index);
            if(ret != RT_ERR_OK)
            {
                    printk("[%s] %d Delete classify rule fail ret:%d index:%d "
                        ,__FUNCTION__, __LINE__, ret, index);
            }
            
            ret = rt_cls_rule_index_del(cur->index+OMCI_XLATEVID_INDEX_SHIFT_BASE*cur->type, RT_CLS_DIR_DS, 1);
            if(ret != RT_ERR_OK)
            {
                    printk("[%s] %d Delete vlan aggregate cls rule index fail ret:%d ruleIndex:%d type:%d"
                        ,__FUNCTION__, __LINE__, ret,cur->index, cur->type);
            }
        }
        list_del(&cur->list);
        kfree(cur);
    }
#endif
}

static void vlan_aggregate_fill_outerVID(unsigned int *pOuterVID, rt_cls_rule_t *pDsVlanRule)
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
        *pOuterVID = OMCI_XLATEVID_UNTAG;
    }
    else
    {
        *pOuterVID = OMCI_XLATEVID_NOT_USED;
    }
}

static void vlan_aggregate_fill_xlateVID(rt_cls_vlan_xlate_info_t *pXlateVID,
                                              rt_cls_rule_t *pDsVlanRule,
                                              unsigned int outerVID,
                                              unsigned int uniMask)
{
    uint32 xlate_cvid = OMCI_XLATEVID_UNTAG;
    uint32 port;

    if ((pXlateVID == NULL) || (pDsVlanRule == NULL))
        return;

    if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) && pDsVlanRule->filterRule.outerTagIf == 1)
    {
        if(pDsVlanRule->outerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = OMCI_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->outerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = outerVID;
        }
        else if(pDsVlanRule->outerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->outerTagAct.assignedVid;
        }
        else
        {
            xlate_cvid = outerVID;
        }
    }
    else if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) && pDsVlanRule->filterRule.innerTagIf == 1)
    {
        if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = OMCI_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = outerVID;
        }
        else if(pDsVlanRule->innerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->innerTagAct.assignedVid;
        }
        else
        {
            xlate_cvid = outerVID;
        }
    }
    else if((pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_INNER_TAGIf_BIT) == 0 && (pDsVlanRule->filterRule.filterMask & RT_CLS_FILTER_OUTER_TAGIf_BIT) == 0)
    {
        if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_REMOVE)
        {
            xlate_cvid = OMCI_XLATEVID_UNTAG;
        }
        else if(pDsVlanRule->innerTagAct.tagAction == RT_CLS_TAG_TRANSPARENT)
        {
            xlate_cvid = outerVID;
        }
        else if(pDsVlanRule->innerTagAct.tagVidAction == RT_CLS_VID_ASSIGN)
        {
            xlate_cvid = pDsVlanRule->innerTagAct.assignedVid;
        }
        else
        {
            xlate_cvid = outerVID;
        }
    }
    else
    {
        xlate_cvid = OMCI_XLATEVID_UNTAG;
    }
   

    for(port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (uniMask & (0x0001 << port))
        {
            /* One VID, Mode = SP2C, more than one VID, Mode = LUT */
            if (pXlateVID[port].xlateMode == OMCI_XLATEVID_NOT_USED)
            {
                pXlateVID[port].xlateMode = OMCI_XLATEVID_ONLY_ONE;
                pXlateVID[port].vid = xlate_cvid;
            }
            else
            {
                pXlateVID[port].xlateMode = OMCI_XLATEVID_TWO_MORE;

                /* if more than one VID, we record untag first, than smallest VID */
                if ( (xlate_cvid == OMCI_XLATEVID_UNTAG) || (pXlateVID[port].vid > xlate_cvid) )
                    pXlateVID[port].vid = xlate_cvid;
            }
        }
    }
}

static int is_same_rule_by_ds_vlan_filter(
                                               rt_cls_rule_t *pBR,
                                               rt_cls_rule_t *pL2)
{
    rt_cls_filter_rule_t tmpFilterRule[2];

    if(memcmp(&(pBR->filterRule), &(pL2->filterRule), sizeof(rt_cls_filter_rule_t)) == 0)
        return TRUE;

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
        if(memcmp(&tmpFilterRule[0], &tmpFilterRule[1], sizeof(rt_cls_filter_rule_t)) == 0)
            return TRUE;
    }
    return FALSE;
}

static int32 vlan_aggregate_cf_set(int type,rt_cls_rule_t *pClsRule,rt_cls_vlan_xlate_info_t *pXlateVID,int aggrUniMask)
{
    rtk_classify_cfg_t entry;
    int32 index=0, i;
    int32 ret=RT_ERR_FAILED;
    uint32 targetUniMask = 0x0;

    memset(&entry,0,sizeof(rtk_classify_cfg_t));
    ret = rt_rtk_cls_filter_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls filter fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }
    ret = rt_rtk_cls_ds_action_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls action fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }
    ret = rt_cls_rule_index_get(pClsRule->index+OMCI_XLATEVID_INDEX_SHIFT_BASE*type, pClsRule->direction, &index, 0, 0, 1);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }

    if(type == OMCI_XLATEVID_RULE_SP2C)
    {
        /* Calculate SP2C UNI mask */
        for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
        {
            if (pXlateVID[i].xlateMode != OMCI_XLATEVID_NOT_USED)
                targetUniMask |= (0x0001 << i);
        }
        entry.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
        entry.act.dsAct.uniMask.bits[0] = targetUniMask;
        
        entry.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
        entry.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C;
        entry.act.dsAct.cTagVid = 0;
    }
    if(type == OMCI_XLATEVID_RULE_LUT)
    {
        /* Don't need to do force forward because LUT CF rule already filter destination port */
        entry.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_NOP;
        entry.act.dsAct.uniMask.bits[0] = 0x0;

        /* Filter destination UNI */
        memset(&uniField,0,sizeof(rtk_classify_field_t));
        uniField.fieldType = CLASSIFY_FIELD_UNI;
        uniField.classify_pattern.uni.value = aggrUniMask;
        uniField.classify_pattern.uni.mask = 0xf;
        if( (ret = rtk_classify_field_add(&entry, &uniField)) != RT_ERR_OK)
        {
            return ret;
        }
        entry.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
        entry.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
        entry.act.dsAct.cTagVid = 0;
    }

    entry.index=index;
    ret = rtk_classify_cfgEntry_add(&entry);
    if(ret != RT_ERR_OK)
    {
            printk("[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
    }

    return ret;
}

static int l2_vlan_aggregate(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *cur_next = NULL;
    struct list_head *cur_tmp = NULL;
    rt_cls_rule_db_t *cur = NULL;
    struct list_head *tar_next = NULL;
    struct list_head *tar_tmp = NULL;
    rt_cls_rule_db_t *tar = NULL;
    uint32 aggrUniMask;
    uint32 setupAggVlanCf;
    int32 ret;
    uint32 outerVID;
    rt_cls_vlan_xlate_info_t pXlateVID[RTK_MAX_NUM_OF_PORTS];
    uint32 i;
    rt_cls_vlan_rule_index_t *pVlanRuleIndexEntry;
    rt_cls_sp2c_vlan_t *pSp2cVlanEntry;

    /* Delete all exist SP2C entry */
    vlan_aggregate_sp2c_delAll();

    /* Delete all exist L2 Aggregated VLAN CF rule & clear database*/
    vlan_aggregate_cf_delAll();
    /* Serach all service flow to find out any aggregated service */
    list_for_each_safe(cur_next, cur_tmp, &dsRuleHead)
    {
        cur = list_entry(cur_next, rt_cls_rule_db_t, list);
        aggrUniMask = cur->rule.ingressPortMask;
        setupAggVlanCf = FALSE;
        

        /* fill Outer & Translation VID database by current flow */
        for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
        {
            pXlateVID[i].xlateMode = OMCI_XLATEVID_NOT_USED;
            pXlateVID[i].vid = 0;
        }
        vlan_aggregate_fill_outerVID(&outerVID, &cur->rule);
        vlan_aggregate_fill_xlateVID(pXlateVID, &cur->rule, outerVID, cur->rule.ingressPortMask);
        list_for_each_safe(tar_next, tar_tmp, &dsRuleHead)
        {
            tar = list_entry(tar_next, rt_cls_rule_db_t, list);
            /* Compare 2 flows only when they are in the same DS SID and current flow is not VLAN_OPER_MODE_FORWARD_ALL */
            if ((cur->rule.flowId == tar->rule.flowId) && (cur->rule.index != tar->rule.index))
            {
                if (is_same_rule_by_ds_vlan_filter( &cur->rule, &tar->rule) == TRUE)
                {
                    /* Same DS VLAN Filter */
                    /* Setup Aggregated VLAN rule only if           */
                    /* 1. Target rule is VLAN_OPER_MODE_FORWARD_ALL */
                    /* 2. Current rule index < Target rule index    */
                    if (cur->rule.index < tar->rule.index )
                    {
                        aggrUniMask |= tar->rule.ingressPortMask;
                        setupAggVlanCf = TRUE;
                        /* fill Translation VID database by target flow */
                        vlan_aggregate_fill_xlateVID(pXlateVID, &(tar->rule), outerVID, tar->rule.ingressPortMask);
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
            /* Setup Aggregated VLAN LUT CF */
            for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
            {
                if(HAL_IS_CPU_PORT(i))
                    continue;
                if (pXlateVID[i].xlateMode == OMCI_XLATEVID_TWO_MORE)
                {
                    if ((ret = vlan_aggregate_cf_set(OMCI_XLATEVID_RULE_LUT,  &(cur->rule), pXlateVID, i)) != RT_ERR_OK)
                        printk("omci_set_vlan_aggregate_cf Fail, ret = 0x%X", ret);
                    pVlanRuleIndexEntry=kzalloc(sizeof(rt_cls_vlan_rule_index_t), GFP_KERNEL);
                    if (!pVlanRuleIndexEntry)
                        return RT_ERR_FAILED;
                    pVlanRuleIndexEntry->index = cur->rule.index;
                    pVlanRuleIndexEntry->type = OMCI_XLATEVID_RULE_LUT;

                    list_add_tail(&pVlanRuleIndexEntry->list, &vlanRuleIndexHead);
                }
            }

            /* Setup Aggregated VLAN CF */
            for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
            {
                if (pXlateVID[i].xlateMode == OMCI_XLATEVID_ONLY_ONE)
                {
                    if ((ret = vlan_aggregate_cf_set(OMCI_XLATEVID_RULE_SP2C, &(cur->rule), pXlateVID, aggrUniMask)) != RT_ERR_OK)
                       printk("omci_set_vlan_aggregate_cf Fail, ret = 0x%X", ret);
                    pVlanRuleIndexEntry=kzalloc(sizeof(rt_cls_vlan_rule_index_t), GFP_KERNEL);
                    if (!pVlanRuleIndexEntry)
                        return RT_ERR_FAILED;
                    pVlanRuleIndexEntry->index = cur->rule.index;
                    pVlanRuleIndexEntry->type = OMCI_XLATEVID_RULE_SP2C;
                    list_add_tail(&pVlanRuleIndexEntry->list, &vlanRuleIndexHead);
                    break;
                }
                    
            }
            /* Setup SP2C VLAN table */
            for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
            {
                if (pXlateVID[i].xlateMode == OMCI_XLATEVID_ONLY_ONE && pXlateVID[i].vid != OMCI_XLATEVID_UNTAG)
                {
                    vlan_aggregate_fill_outerVID(&outerVID, &cur->rule);
                    ret=rtk_svlan_sp2c_add(outerVID, i, pXlateVID[i].vid);
                    if(ret != RT_ERR_OK)
                        printk("[%s]Add sp2c vlan fail, port %d, vlan %d\n",__FUNCTION__,i,outerVID);
                    pSp2cVlanEntry=kzalloc(sizeof(rt_cls_sp2c_vlan_t), GFP_KERNEL);
                    if (!pSp2cVlanEntry)
                        return RT_ERR_FAILED;
                    pSp2cVlanEntry->port = i;
                    pSp2cVlanEntry->vlan = outerVID;
                    list_add_tail(&pSp2cVlanEntry->list, &sp2cVlanHead);
                }
            }
        }
    }
#endif
    return RT_ERR_OK;
}

static int32 rt_rtk_cls_rule_add(rt_cls_rule_t *pClsRule,int first)
{
    rtk_classify_cfg_t entry;
    int32 index=0, portIndex;
    int32 ret=RT_ERR_FAILED;
    rt_cls_rule_db_t *pNewDsRule=NULL;


    memset(&entry,0,sizeof(rtk_classify_cfg_t));

    ret = rt_rtk_cls_filter_set(&entry,pClsRule);
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls filter fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }
    if(pClsRule->direction == RT_CLS_DIR_US)
        ret = rt_rtk_cls_us_action_set(&entry,pClsRule);
    else
        ret = rt_rtk_cls_ds_action_set(&entry,pClsRule);
    
    if(ret != RT_ERR_OK)
    {
        printk("[%s] %d Set cls action fail ret:%d ruleIndex:%d dir:%d"
            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
        return ret;
    }
    if(pClsRule->ingressPortMask != 0 && pClsRule->direction == RT_CLS_DIR_US)
    {
        memset(&uniField,0,sizeof(rtk_classify_field_t));
        uniField.fieldType = CLASSIFY_FIELD_UNI;
        uniField.classify_pattern.uni.mask = 0xf;
        ret = rtk_classify_field_add(&entry, &uniField);
        if(ret != RT_ERR_OK)
            return ret;
        for(portIndex = 0; portIndex < RTK_MAX_NUM_OF_PORTS; portIndex++)
        {
            if(pClsRule->ingressPortMask & (1 << portIndex))
            {
                uniField.classify_pattern.uni.value = portIndex;
                ret = rt_cls_rule_index_get(pClsRule->index, pClsRule->direction, &index, portIndex, pClsRule->rulePriority, 0);
                if(ret != RT_ERR_OK)
                {
                    printk("[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                        ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                    return ret;
                }
                entry.index=index;
                if (NULL == RT_MAPPER->classify_cfgEntry_add)
                    return RT_ERR_DRIVER_NOT_FOUND;

                RTK_API_LOCK();
                ret = RT_MAPPER->classify_cfgEntry_add(&entry);
                RTK_API_UNLOCK();
                if(ret != RT_ERR_OK)
                {
                        printk("[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                            ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
                        return ret;
                }
            }
        }
    }
    else
    {
        ret = rt_cls_rule_index_get(pClsRule->index, pClsRule->direction, &index, RT_CLS_RULE_PORT_NOT_USED, pClsRule->rulePriority, 0);
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Get cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
        }
        entry.index=index;
        if (NULL == RT_MAPPER->classify_cfgEntry_add)
            return RT_ERR_DRIVER_NOT_FOUND;

        RTK_API_LOCK();
        ret = RT_MAPPER->classify_cfgEntry_add(&entry);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Set cls rule fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, pClsRule->index, pClsRule->direction);
            return ret;
        }
    }

    if(first == 1)
    {
        /* VLAN aggregation */
        if(pClsRule->direction == RT_CLS_DIR_DS)
        {
#if defined(CONFIG_SDK_KERNEL_LINUX)
            pNewDsRule = kzalloc(sizeof(rt_cls_rule_db_t), GFP_KERNEL);
#endif
            if (!pNewDsRule)
                return RT_ERR_FAILED;
            osal_memcpy(&pNewDsRule->rule, pClsRule, sizeof(rt_cls_rule_t));
#if defined(CONFIG_SDK_KERNEL_LINUX)
            list_add_tail(&pNewDsRule->list, &dsRuleHead);
#endif
            l2_vlan_aggregate();
        }

        if(pClsRule->direction == RT_CLS_DIR_US)
        {
#if defined(CONFIG_SDK_KERNEL_LINUX)
            pNewDsRule = kzalloc(sizeof(rt_cls_rule_db_t), GFP_KERNEL);
#endif
            if (!pNewDsRule)
                return RT_ERR_FAILED;
            osal_memcpy(&pNewDsRule->rule, pClsRule, sizeof(rt_cls_rule_t));
#if defined(CONFIG_SDK_KERNEL_LINUX)
            list_add_tail(&pNewDsRule->list, &usRuleHead);
#endif
        }
    }
    return ret;
}


static int32 rt_rtk_cls_rule_delete(uint32 ruleIndex)
{
    int32 index;
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    rt_cls_rule_db_t  *pEntryData= NULL;
#endif
    
    ret = rt_cls_rule_index_find(ruleIndex, RT_CLS_DIR_DS, &index, RT_CLS_RULE_PORT_NOT_USED, 0);
    if(ret == RT_ERR_OK)
    {
        if (NULL == RT_MAPPER->classify_cfgEntry_del)
           return RT_ERR_DRIVER_NOT_FOUND;

        RTK_API_LOCK();
        ret = RT_MAPPER->classify_cfgEntry_del(index);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Delete classify rule fail ret:%d index:%d "
            ,__FUNCTION__, __LINE__, ret, index);
            return ret;
        }
    
        ret = rt_cls_rule_index_del(ruleIndex, RT_CLS_DIR_DS, 0);
        if(ret != RT_ERR_OK)
        {
            printk("[%s] %d Delete cls rule index fail ret:%d ruleIndex:%d dir:%d"
                ,__FUNCTION__, __LINE__, ret, ruleIndex, RT_CLS_DIR_US);
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
        list_for_each_safe(pEntry, pTmpEntry, &dsRuleHead)
        {
            pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
            if (pEntryData->rule.index == ruleIndex)
            {
                list_del(&pEntryData->list);
                kfree(pEntryData);
            }
        }
#endif
    }

    while(rt_cls_rule_index_find(ruleIndex, RT_CLS_DIR_US, &index,RT_CLS_RULE_PORT_IGNORE, 0) == RT_ERR_OK)
    {
        if (NULL == RT_MAPPER->classify_cfgEntry_del)
           return RT_ERR_DRIVER_NOT_FOUND;

        RTK_API_LOCK();
        ret = RT_MAPPER->classify_cfgEntry_del(index);
        RTK_API_UNLOCK();
        if(ret != RT_ERR_OK)
        {
                printk("[%s] %d Delete classify rule fail ret:%d index:%d "
                    ,__FUNCTION__, __LINE__, ret, index);
                return ret;
        }
        
        ret = rt_cls_rule_index_del(ruleIndex, RT_CLS_DIR_US, 0);
        if(ret != RT_ERR_OK)
        {
                printk("[%s] %d Delete cls rule index fail ret:%d ruleIndex:%d dir:%d"
                    ,__FUNCTION__, __LINE__, ret, ruleIndex, RT_CLS_DIR_US);
                return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
        list_for_each_safe(pEntry, pTmpEntry, &usRuleHead)
        {
            pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
            if (pEntryData->rule.index == ruleIndex)
            {
                list_del(&pEntryData->list);
                kfree(pEntryData);
            }
        }
#endif
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(list_empty(&usRuleHead) && list_empty(&dsRuleHead))
    {
        doubleCtag = 0;
        rtk_svlan_tpidEntry_set(1,0x8888);
        rtk_svlan_tpidEnable_set(1,DISABLED);
    }
#endif
    return RT_ERR_OK;
}

static int32 rt_rtk_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo)
{
    int32   ret = RT_ERR_OK;
    uint32  ruleHwIndex=0, i;
    int ruleMax = HAL_CLASSIFY_ENTRY_MAX();
    rt_cls_rule_db_t *pEntryData= NULL;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    struct list_head *pHead     = NULL;
#endif

    if(pClsRuleInfo == NULL)
        return RT_ERR_NULL_POINTER;

    memset(pClsRuleInfo->hwIndex,0xff,sizeof(uint16)*RT_CLS_HW_INDEX_MAX);

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(dir == RT_CLS_DIR_DS)
        pHead = &dsRuleHead;
    else
        pHead = &usRuleHead;
#endif

    if(op == RT_CLS_GET_OP_FIRST)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)
        pEntryData = list_first_entry_or_null(pHead, rt_cls_rule_db_t, list);
#else
        if(index == 0)
            pEntryData = NULL;
#endif
    }
#if defined(CONFIG_SDK_KERNEL_LINUX)
    else if(op == RT_CLS_GET_OP_NEXT)
    {
        list_for_each_safe(pEntry, pTmpEntry, pHead)
        {
            pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
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
            pEntryData = list_entry(pEntry, rt_cls_rule_db_t, list);
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
        return RT_ERR_FAILED;
    }
    else
    {
        memcpy(&(pClsRuleInfo->rule),&(pEntryData->rule),sizeof(rt_cls_rule_t));
    }

    for(i=RT_CLS_RULE_RESERVED_NUM;i<ruleMax;i++)
    {
        if((RT_CLS_RULE_NOT_IN_USED != clsRuleUsage[i].index) && (clsRuleUsage[i].ruleIndex == pEntryData->rule.index) && (clsRuleUsage[i].dir == dir) )
        {
            pClsRuleInfo->hwIndex[ruleHwIndex++]=i;
            if(ruleHwIndex >= RT_CLS_HW_INDEX_MAX)
                break;
        }
    }

    return ret;
}

#endif

/* Function Name:
 *      rt_cls_init
 * Description:
 *      Initialize cls setting
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
int32 rt_cls_init(void)
{
    int32   ret = RT_ERR_OK;
#ifdef CONFIG_LUNA_G3_SERIES
#if (!defined(CONFIG_CA8279_SERIES) && !defined(CONFIG_CA8277B_SERIES))
    if (NULL == RT_MAPPER->rt_cls_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_init();
    RTK_API_UNLOCK();
#endif
#else
    rt_cls_initial();
#endif
    return ret;
}


/* Function Name:
 *      rt_cls_rule_add
 * Description:
 *      Add  classify rule and apply to ASIC.
 * Input:
 *      pClsRule - The classify rule configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32 rt_cls_rule_add(rt_cls_rule_t *pClsRule)
{
    int32   ret = RT_ERR_OK;
#ifdef CONFIG_LUNA_G3_SERIES
    /* function body */
    if (NULL == RT_MAPPER->rt_cls_rule_add)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_rule_add(pClsRule);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_cls_rule_add(pClsRule, 1);
#endif
    return ret;
}   /* end of rt_cls_rule_add */

/* Function Name:
 *      rt_cls_rule_delete
 * Description:
 *      Delete  classify rule.
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
int32 rt_cls_rule_delete(uint32 index)
{
    int32   ret = RT_ERR_OK;
#ifdef CONFIG_LUNA_G3_SERIES
    /* function body */
    if (NULL == RT_MAPPER->rt_cls_rule_delete)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_rule_delete(index);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_cls_rule_delete(index);
#endif
    return ret;
}   /* end of rt_cls_rule_delete */

/* Function Name:
 *      rt_cls_rule_get
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
extern int32 rt_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo)
{
    int32   ret;

#ifdef CONFIG_LUNA_G3_SERIES
    /* function body */
    if (NULL == RT_MAPPER->rt_cls_rule_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_rule_get(op, index, dir, pClsRuleInfo);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_cls_rule_get(op, index, dir, pClsRuleInfo);
#endif
    return ret;
}   /* end of rt_cls_rule_get */

/* Function Name:
 *      rt_cls_extend_rule_add
 * Description:
 *      Add extend classify rule to ASIC.
 * Input:
 *      pClsRule - The extend classify rule configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32 rt_cls_extend_rule_add(rt_cls_extend_rule_t *pClsRule)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_extend_rule_add)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_extend_rule_add(pClsRule);
    RTK_API_UNLOCK();

    return ret;
} /* end of rt_cls_extend_rule_add */

/* Function Name:
 *      rt_cls_extend_rule_delete
 * Description:
 *      Delete extend classify rule.
 * Input:
 *      index     - index of extend classify rule.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
extern int32 rt_cls_extend_rule_delete(uint32 index)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_extend_rule_delete)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_extend_rule_delete(index);
    RTK_API_UNLOCK();

    return ret;
} /* end of rt_cls_extend_rule_delete */

/* Function Name:
 *      rt_cls_extend_rule_get
 * Description:
 *      Get extend classify rule.
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      pClsRule  - extend classify rule information
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
extern int32 rt_cls_extend_rule_get(uint32 index, rt_cls_extend_rule_t *pClsRule)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_extend_rule_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_extend_rule_get(index, pClsRule);
    RTK_API_UNLOCK();

    return ret;
} /* end of rt_cls_extend_rule_get */



/* Function Name:
 *      rt_cls_fwdPort_set
 * Description:
 *      Set VEIP port default forward port.
 * Input:
 *      port     - port index.
 *      fwdPort  - defalt forward port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None.
 */
int32 rt_cls_fwdPort_set(rt_port_t port, rt_port_t fwdPort)
{
    int32   ret = RT_ERR_OK;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_fwdPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_fwdPort_set(port, fwdPort);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_cls_fwdPort_set */

/* Function Name:
 *      rt_cls_vlan_paser_set
 * Description:
 *      Set VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 *      data     - value of vlan parser attribute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32 rt_cls_vlan_paser_set(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 data)
{
    int32   ret = RT_ERR_OK;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_fwdPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_vlan_paser_set(port, type, data);
    RTK_API_UNLOCK();

    return ret;
}


/* Function Name:
 *      rt_cls_vlan_paser_get
 * Description:
 *      Get VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 * Output:
 *      pData     - value of vlan parser attribute
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32 rt_cls_vlan_paser_get(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 *pData)
{
    int32   ret = RT_ERR_OK;

    /* function body */
    if (NULL == RT_MAPPER->rt_cls_fwdPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_cls_vlan_paser_get(port, type, pData);
    RTK_API_UNLOCK();

    return ret;
}



