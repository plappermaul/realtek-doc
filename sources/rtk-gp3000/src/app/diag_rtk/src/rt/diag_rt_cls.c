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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition those RT cls command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_cls.h>

rt_cls_rule_t entry;
rt_cls_extend_rule_t extEntry;

char* cs_act_show_str[5]={"Transparent","Delete","Stag_TPID","TPID_A","TPID_B"};
char* ctag_act_show_str[3]={"Transparent","Delete","Tagging"};
char* pri_act_show_str[4]={"Assign","Copy from outer","Copy from inner","Copy from DSCP"};
char* vid_act_show_str[4]={"Assign","Copy from outer","Copy from inner","Copy from FDB"};
char* cfi_act_show_str[2]={"Always 0","Keep from ingress"};
char* flowid_act_show_str[2]={"Nop","Assign"};
char* grpid_act_show_str[2]={"Nop","Assign"};
char* fwd_act_show_str[5]={"Nop","Drop","Trap to CPU","LDPID","MCGID"};
char* cos_act_show_str[2]={"Nop","Assign"};
char* pkttype_show_str[5]={"Unicast","Broadcast","Multicast","Unknown Unicast","Unknown Multicast"};

static uint32 _cls_rule_show(rt_cls_rule_t *pRule)
{
    int i;
    char ingressPort[64];
    rt_portmask_t portmask;
    rtk_portmask_t tmpPortmask;

    diag_util_printf("Index: %d\n", pRule->index);
    diag_util_printf("FlowID: %d\n", pRule->flowId);
    if(pRule->direction == RT_CLS_DIR_DS)
        diag_util_printf("Direction: Downstream\n");
    else
        diag_util_printf("Direction: Upstream\n");

    portmask.bits[0] = pRule->ingressPortMask;
    memcpy(tmpPortmask.bits,portmask.bits,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST*sizeof(uint32));
    diag_util_lPortMask2str(ingressPort,&tmpPortmask);
    diag_util_printf("IngressPortMask: %s\n",ingressPort);

    /*Filter*/
    diag_util_printf("Filter:\n");
    if(RT_CLS_FILTER_FLOWID_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("      FlowId: %d\n", pRule->filterRule.flowId);
    }

    if(RT_CLS_FILTER_OUTER_TAGIf_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    OuterTag: %d\n", pRule->filterRule.outerTagIf);
    }

    if(RT_CLS_FILTER_OUTER_TPID_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("   OuterTpid: 0x%04x\n", pRule->filterRule.outerTagTpid);
    }

    if(RT_CLS_FILTER_OUTER_VID_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    OuterVid: %d\n", pRule->filterRule.outerTagVid);
    }

    if(RT_CLS_FILTER_OUTER_PRI_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    OuterPri: %d\n", pRule->filterRule.outerTagPri);
    }

    if(RT_CLS_FILTER_OUTER_DEI_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    OuterDei: %d\n", pRule->filterRule.outerTagDei);
    }

    if(RT_CLS_FILTER_INNER_TAGIf_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    InnerTag: %d\n", pRule->filterRule.innerTagIf);
    }

    if(RT_CLS_FILTER_INNER_TPID_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("   InnerTpid: 0x%04x\n", pRule->filterRule.innerTagTpid);
    }

    if(RT_CLS_FILTER_INNER_VID_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    InnerVid: %d\n", pRule->filterRule.innerTagVid);
    }

    if(RT_CLS_FILTER_INNER_PRI_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    InnerPri: %d\n", pRule->filterRule.innerTagPri);
    }

    if(RT_CLS_FILTER_INNER_DEI_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("    InnerDei: %d\n", pRule->filterRule.innerTagDei);
    }

    if(RT_CLS_FILTER_ETHERTYPE & pRule->filterRule.filterMask)
    {
        diag_util_printf("   Ethertype: 0x%04x\n", pRule->filterRule.etherType);
    }

    if(RT_CLS_FILTER_DSCP_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("        DSCP: 0x");
        for(i=0;i<8;i++)
            diag_util_printf("%02x",pRule->filterRule.dscp[i]);
        diag_util_printf("\n");
    }

    if(RT_CLS_FILTER_EGRESS_PORT_BIT & pRule->filterRule.filterMask)
    {
        diag_util_printf("  EgressPort: 0x%x\n", pRule->filterRule.egressPortMask);
    }

    diag_util_printf("Action:\n");

    switch(pRule->outerTagAct.tagAction)
    {
        case RT_CLS_TAG_TRANSPARENT:
            diag_util_printf("    OuterTag: Transparent\n");
            break;
        case RT_CLS_TAG_MODIFY:
            diag_util_printf("    OuterTag: Modify\n");
            break;
        case RT_CLS_TAG_TAGGING:
            diag_util_printf("    OuterTag: Tagging\n");
            break;
        case RT_CLS_TAG_REMOVE:
            diag_util_printf("    OuterTag: Remove\n");
            break;
        default:
            diag_util_printf("    OuterTag: Unknown action\n");
            break;
    }

    if(pRule->outerTagAct.tagAction != RT_CLS_TAG_TRANSPARENT && pRule->outerTagAct.tagAction != RT_CLS_TAG_REMOVE)
    {
        switch(pRule->outerTagAct.tagTpidAction)
        {
            case RT_CLS_TPID_ASSIGN:
                diag_util_printf("   OuterTpid: Assign 0x%04x\n",pRule->outerTagAct.assignedTpid);
                break;
            case RT_CLS_TPID_COPY_FROM_1ST_TAG:
            diag_util_printf("   OuterTpid: Copy from 1st tag\n");
                break;
            case RT_CLS_TPID_COPY_FROM_2ND_TAG:
            diag_util_printf("   OuterTpid: Copy from 2nd tag\n");
                break;
            default:
            diag_util_printf("   OuterTpid: Unknown tpid action\n");
                break;
        }

        switch(pRule->outerTagAct.tagVidAction)
        {
            case RT_CLS_VID_ASSIGN:
                diag_util_printf("    OuterVid: Assign %d\n",pRule->outerTagAct.assignedVid);
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                diag_util_printf("    OuterVid: Copy from 1st tag\n");
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                diag_util_printf("    OuterVid: Copy from 2nd tag\n");
                break;
            case RT_CLS_VID_TRANSPARENT:
                diag_util_printf("    OuterVid: Transparent\n");
                break;
            default:
                diag_util_printf("    OuterVid: Unknown vid action\n");
                break;
        }

        switch(pRule->outerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                diag_util_printf("    OuterPri: Assign %d\n",pRule->outerTagAct.assignedPri);
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                diag_util_printf("    OuterPri: Copy from 1st tag\n");
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                diag_util_printf("    OuterPri: Copy from 2nd tag\n");
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                diag_util_printf("    OuterPri: DSCP remapping\n");
                break;
            case RT_CLS_PRI_TRANSPARENT:
                diag_util_printf("    OuterPri: Transparent\n");
                break;
            default:
                diag_util_printf("    OuterPri: Unknown pri action\n");
                break;
        }

        switch(pRule->outerTagAct.tagDeiAction)
        {
            case RT_CLS_DEI_ASSIGN:
                diag_util_printf("    OuterDei: Assign %d\n",pRule->outerTagAct.assignedDei);
                break;
            case RT_CLS_DEI_COPY_FROM_1ST_TAG:
                diag_util_printf("    OuterDei: Copy from 1st tag\n");
                break;
            case RT_CLS_DEI_COPY_FROM_2ND_TAG:
                diag_util_printf("    OuterDei: Copy from 2nd tag\n");
                break;
            default:
                diag_util_printf("    OuterDei: Unknown dei action\n");
                break;
        }
    }

    switch(pRule->innerTagAct.tagAction)
    {
        case RT_CLS_TAG_TRANSPARENT:
            diag_util_printf("    InnerTag: Transparent\n");
            break;
        case RT_CLS_TAG_MODIFY:
            diag_util_printf("    InnerTag: Modify\n");
            break;
        case RT_CLS_TAG_TAGGING:
            diag_util_printf("    InnerTag: Tagging\n");
            break;
        case RT_CLS_TAG_REMOVE:
            diag_util_printf("    InnerTag: Remove\n");
            break;
        default:
            diag_util_printf("    InnerTag: Unknown action\n");
            break;
    }

    if(pRule->innerTagAct.tagAction != RT_CLS_TAG_TRANSPARENT && pRule->innerTagAct.tagAction != RT_CLS_TAG_REMOVE)
    {
        switch(pRule->innerTagAct.tagTpidAction)
        {
            case RT_CLS_TPID_ASSIGN:
                diag_util_printf("   InnerTpid: Assign 0x%04x\n",pRule->innerTagAct.assignedTpid);
                break;
            case RT_CLS_TPID_COPY_FROM_1ST_TAG:
            diag_util_printf("   InnerTpid: Copy from 1st tag\n");
                break;
            case RT_CLS_TPID_COPY_FROM_2ND_TAG:
            diag_util_printf("   InnerTpid: Copy from 2nd tag\n");
                break;
            default:
            diag_util_printf("   InnerTpid: Unknown tpid action\n");
                break;
        }

        switch(pRule->innerTagAct.tagVidAction)
        {
            case RT_CLS_VID_ASSIGN:
                diag_util_printf("    InnerVid: Assign %d\n",pRule->innerTagAct.assignedVid);
                break;
            case RT_CLS_VID_COPY_FROM_1ST_TAG:
                diag_util_printf("    InnerVid: Copy from 1st tag\n");
                break;
            case RT_CLS_VID_COPY_FROM_2ND_TAG:
                diag_util_printf("    InnerVid: Copy from 2nd tag\n");
                break;
            case RT_CLS_VID_TRANSPARENT:
                diag_util_printf("    InnerVid: Transparent\n");
                break;
            default:
                diag_util_printf("    InnerVid: Unknown vid action\n");
                break;
        }

        switch(pRule->innerTagAct.tagPriAction)
        {
            case RT_CLS_PRI_ASSIGN:
                diag_util_printf("    InnerPri: Assign %d\n",pRule->innerTagAct.assignedPri);
                break;
            case RT_CLS_PRI_COPY_FROM_1ST_TAG:
                diag_util_printf("    InnerPri: Copy from 1st tag\n");
                break;
            case RT_CLS_PRI_COPY_FROM_2ND_TAG:
                diag_util_printf("    InnerPri: Copy from 2nd tag\n");
                break;
            case RT_CLS_PRI_COPY_FROM_DSCP_REMAP:
                diag_util_printf("    InnerPri: DSCP remapping\n");
                break;
            case RT_CLS_PRI_TRANSPARENT:
                diag_util_printf("    InnerPri: Transparent\n");
                break;
            default:
                diag_util_printf("    InnerPri: Unknown pri action\n");
                break;
        }

        switch(pRule->innerTagAct.tagDeiAction)
        {
            case RT_CLS_DEI_ASSIGN:
                diag_util_printf("    InnerDei: Assign %d\n",pRule->innerTagAct.assignedDei);
                break;
            case RT_CLS_DEI_COPY_FROM_1ST_TAG:
                diag_util_printf("    InnerDei: Copy from 1st tag\n");
                break;
            case RT_CLS_DEI_COPY_FROM_2ND_TAG:
                diag_util_printf("    InnerDei: Copy from 2nd tag\n");
                break;
            default:
                diag_util_printf("    InnerDei: Unknown dei action\n");
                break;
        }
    }

    if(pRule->direction == RT_CLS_DIR_DS)
    {
        if(pRule->dsPriAct == RT_CLS_DS_PRI_ACT_NOP)
        {
            diag_util_printf("    DsPriAct: No action\n");
        }
        else
        {
            diag_util_printf("    DsPriAct: Assign priority %d\n",pRule->dsPri);
        }
    }
    return CPARSER_OK;
}

static uint32 _cls_extend_rule_show(rt_cls_extend_rule_t *pExtRule)
{
    diag_util_mprintf("***************************************\n");
    diag_util_printf("Index: %d\n", pExtRule->index);
    diag_util_printf("Valid: %d\n", pExtRule->valid);
    diag_util_printf("Filter:\n");
    if(pExtRule->filterRule.mask.direction != 0)
    {
        diag_util_printf("    Direction: %s\n",pExtRule->filterRule.data.direction == 1 ? "Downstream" : "Upstream");
    }
    if(pExtRule->filterRule.mask.egressPort != 0)
    {
        diag_util_printf("    Egress Port: (%d,0x%x)\n",pExtRule->filterRule.data.egressPort,pExtRule->filterRule.mask.egressPort);
    }
    if(pExtRule->filterRule.mask.ingressPort != 0)
    {
        diag_util_printf("    Ingress Port: (%d,0x%x)\n",pExtRule->filterRule.data.ingressPort,pExtRule->filterRule.mask.ingressPort);
    }
    if(pExtRule->filterRule.mask.sbit != 0)
    {
        diag_util_printf("    S Bit: (%d,0x%x)\n",pExtRule->filterRule.data.sbit,pExtRule->filterRule.mask.sbit);
    }
    if(pExtRule->filterRule.mask.cbit != 0)
    {
        diag_util_printf("    C Bit: (%d,0x%x)\n",pExtRule->filterRule.data.cbit,pExtRule->filterRule.mask.cbit);
    }
    if(pExtRule->filterRule.mask.sTpid != 0)
    {
        diag_util_printf("    S TPID: (%d,0x%x)\n",pExtRule->filterRule.data.sTpid,pExtRule->filterRule.mask.sTpid);
    }
    if(pExtRule->filterRule.mask.pktType != 0)
    {
        diag_util_printf("    Packet Type: %s\n",pkttype_show_str[pExtRule->filterRule.data.pktType]);
    }
    if(pExtRule->filterRule.mask.ipv4 != 0)
    {
        diag_util_printf("    IPV4: (%d,0x%x)\n",pExtRule->filterRule.data.ipv4,pExtRule->filterRule.mask.ipv4);
    }
    if(pExtRule->filterRule.mask.ipv6 != 0)
    {
        diag_util_printf("    IPV6: (%d,0x%x)\n",pExtRule->filterRule.data.ipv6,pExtRule->filterRule.mask.ipv6);
    }
    if(pExtRule->filterRule.mask.igmpMld != 0)
    {
        diag_util_printf("    IGMP/MLD: (%d,0x%x)\n",pExtRule->filterRule.data.igmpMld,pExtRule->filterRule.mask.igmpMld);
    }
    if(pExtRule->filterRule.mask.flowId != 0)
    {
        diag_util_printf("    Flow ID: (%d,0x%x)\n",pExtRule->filterRule.data.flowId,pExtRule->filterRule.mask.flowId);
    }
    if(pExtRule->filterRule.mask.ctagVid != 0)
    {
        diag_util_printf("    Ctag VID: (%d,0x%x)\n",pExtRule->filterRule.data.ctagVid,pExtRule->filterRule.mask.ctagVid);
    }
    if(pExtRule->filterRule.mask.ctagPri != 0)
    {
        diag_util_printf("    Ctag PRI: (%d,0x%x)\n",pExtRule->filterRule.data.ctagPri,pExtRule->filterRule.mask.ctagPri);
    }
    if(pExtRule->filterRule.mask.ctagCfi != 0)
    {
        diag_util_printf("    Ctag DEI: (%d,0x%x)\n",pExtRule->filterRule.data.ctagCfi,pExtRule->filterRule.mask.ctagCfi);
    }
    if(pExtRule->filterRule.mask.outerVid != 0)
    {
        diag_util_printf("    Outer VID: (%d,0x%x)\n",pExtRule->filterRule.data.outerVid,pExtRule->filterRule.mask.outerVid);
    }
    if(pExtRule->filterRule.mask.outerPri != 0)
    {
        diag_util_printf("    Outer PRI: (%d,0x%x)\n",pExtRule->filterRule.data.outerPri,pExtRule->filterRule.mask.outerPri);
    }
    if(pExtRule->filterRule.mask.outerDei != 0)
    {
        diag_util_printf("    Outer DEI: (%d,0x%x)\n",pExtRule->filterRule.data.outerDei,pExtRule->filterRule.mask.outerDei);
    }
    if(pExtRule->filterRule.mask.tosTc != 0)
    {
        diag_util_printf("    TOS/TC: (%d,0x%x)\n",pExtRule->filterRule.data.tosTc,pExtRule->filterRule.mask.tosTc);
    }
    if(pExtRule->filterRule.mask.ethertype != 0)
    {
        diag_util_printf("    Ethertype: (0x%04x,0x%04x)\n",pExtRule->filterRule.data.ethertype,pExtRule->filterRule.mask.ethertype);
    }
    
    diag_util_printf("Action:\n");
    
    diag_util_printf("    Stag Action: %s\n",cs_act_show_str[pExtRule->sTagAct.tagAction]);
    if(pExtRule->sTagAct.tagAction != RT_CLS_EXT_STAG_TRANSPARENT && pExtRule->sTagAct.tagAction != RT_CLS_EXT_STAG_REMOVE)
    {
        diag_util_printf("    Stag VID Action: %s\n",vid_act_show_str[pExtRule->sTagAct.vidAction]);
        diag_util_printf("    Stag VID: %u\n",pExtRule->sTagAct.assignedVid);
        diag_util_printf("    Stag PRI Action: %s\n",pri_act_show_str[pExtRule->sTagAct.priAction]);
        diag_util_printf("    Stag PRI: %u\n",pExtRule->sTagAct.assignedPri);
        diag_util_printf("    Stag CFI Action: %s\n",cfi_act_show_str[pExtRule->sTagAct.deiAction]);
    }
    diag_util_printf("    Ctag Action: %s\n",ctag_act_show_str[pExtRule->cTagAct.tagAction]);
    if(pExtRule->cTagAct.tagAction != RT_CLS_EXT_CTAG_TRANSPARENT && pExtRule->cTagAct.tagAction != RT_CLS_EXT_CTAG_REMOVE)
    {
        diag_util_printf("    Ctag VID Action: %s\n",vid_act_show_str[pExtRule->cTagAct.vidAction]);
        diag_util_printf("    Ctag VID: %u\n",pExtRule->cTagAct.assignedVid);
        diag_util_printf("    Ctag PRI Action: %s\n",pri_act_show_str[pExtRule->cTagAct.priAction]);
        diag_util_printf("    Ctag PRI: %u\n",pExtRule->cTagAct.assignedPri);
        diag_util_printf("    Ctag CFI Action: %s\n",cfi_act_show_str[pExtRule->cTagAct.deiAction]);
    }

    if(pExtRule->flowAct != RT_CLS_EXT_NOP)
        diag_util_printf("    Flow ID action  : %s %u\n",flowid_act_show_str[pExtRule->flowAct], pExtRule->flowId);
    else
        diag_util_printf("    Flow ID action  : %s\n",flowid_act_show_str[pExtRule->flowAct]);

    if(pExtRule->grpPolAct != RT_CLS_EXT_NOP)
        diag_util_printf("    Group ID action : %s %u\n",grpid_act_show_str[pExtRule->grpPolAct], pExtRule->grpPolId);
    else
        diag_util_printf("    Group ID action : %s\n",grpid_act_show_str[pExtRule->grpPolAct]);

    if(pExtRule->fwdAct != RT_CLS_EXT_FWD_NOP)
        diag_util_printf("    Forward action  : %s 0x%x\n",fwd_act_show_str[pExtRule->fwdAct], pExtRule->fwdId);
    else
        diag_util_printf("    Forward action  : %s\n",fwd_act_show_str[pExtRule->fwdAct]);

    if(pExtRule->cosAct != RT_CLS_EXT_NOP)
        diag_util_printf("    COS action      : %s %u\n",cos_act_show_str[pExtRule->cosAct], pExtRule->cos);
    else
        diag_util_printf("    COS action      : %s\n",cos_act_show_str[pExtRule->cosAct]);
    return CPARSER_OK;
}

static uint32 _cls_hwIndex_show(uint16_t *pHwINDEX)
{
    char str[256]={0};
    char str2[512]={0};
    int i,first=0;
    snprintf(str,sizeof(str),"HW Index:");
    for(i=0;i<RT_CLS_HW_INDEX_MAX;i++)
    {
        if(pHwINDEX[i] != 0xffff)
        {
            snprintf(str2,sizeof(str2),"%s%s %d",str,first ? "," : "" ,pHwINDEX[i]);
            strncpy(str, str2, sizeof(str));
            first=1;
        }
    }
    diag_util_printf("%s\n",str);
    return CPARSER_OK;
}
/*
 * rt_cls clear rule
 */
cparser_result_t
cparser_cmd_rt_cls_clear_rule(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    memset(&entry, 0, sizeof(rt_cls_rule_t));
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_clear_rule */

/*
 * rt_cls set rule direction ( upstream | downstream )
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_direction_upstream_downstream(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if ('u' == TOKEN_CHAR(4,0))
        entry.direction = RT_CLS_DIR_US;
    else
        entry.direction = RT_CLS_DIR_DS;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_direction_upstream_downstream */

/*
 * rt_cls set rule connection-index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_connection_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    entry.index = *index_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_connection_index_index */

/*
 * rt_cls set rule flow-id <UINT:flowId>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_flow_id_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
    entry.flowId = *flowId_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_flow_id_flowid */

/*
 * rt_cls set rule ingress-port-mask ( <PORT_LIST:ingressports> | none )
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_ingress_port_mask_ingressports_none(
    cparser_context_t *context,
    char * *ingressports_ptr)
{
    diag_portlist_t portlist;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    entry.ingressPortMask = portlist.portmask.bits[0];
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_ingress_port_mask_ingressports_none */

/*
 * rt_cls set rule filter flow-id <UINT:flowId>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_flow_id_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_FLOWID_BIT;
    entry.filterRule.flowId = *flowId_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_flow_id_flowid */

/*
 * rt_cls set rule filter outer-tag <UINT:outerTag> { <UINT:tpid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_outer_tag_outerTag_tpid(
    cparser_context_t *context,
    uint32_t  *outerTag_ptr,
    uint32_t  *tpid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*outerTag_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_OUTER_TAGIf_BIT;
    entry.filterRule.outerTagIf = *outerTag_ptr;
    if(*outerTag_ptr && tpid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*tpid_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
        entry.filterRule.filterMask |= RT_CLS_FILTER_OUTER_TPID_BIT;
        entry.filterRule.outerTagTpid = *tpid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_outer_tag_outertag_tpid */

/*
 * rt_cls set rule filter inner-tag <UINT:innerTag> { <UINT:tpid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_inner_tag_innerTag_tpid(
    cparser_context_t *context,
    uint32_t  *innerTag_ptr,
    uint32_t  *tpid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*innerTag_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_INNER_TAGIf_BIT;
    entry.filterRule.innerTagIf = *innerTag_ptr;
    if(*innerTag_ptr && tpid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*tpid_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
        entry.filterRule.filterMask |= RT_CLS_FILTER_INNER_TPID_BIT;
        entry.filterRule.innerTagTpid = *tpid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_inner_tag_innertag_tpid */

/*
 * rt_cls set rule filter outer-vid <UINT:outerVid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_outer_vid_outerVid(
    cparser_context_t *context,
    uint32_t  *outerVid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*outerVid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_OUTER_VID_BIT;
    entry.filterRule.outerTagVid = *outerVid_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_outer_vid_outervid */

/*
 * rt_cls set rule filter outer-pri <UINT:outerPri>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_outer_pri_outerPri(
    cparser_context_t *context,
    uint32_t  *outerPri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*outerPri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_OUTER_PRI_BIT;
    entry.filterRule.outerTagPri = *outerPri_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_outer_pri_outerpri */

/*
 * rt_cls set rule filter outer-dei <UINT:outerDei>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_outer_dei_outerDei(
    cparser_context_t *context,
    uint32_t  *outerDei_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*outerDei_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_OUTER_DEI_BIT;
    entry.filterRule.outerTagDei = *outerDei_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_outer_dei_outerdei */

/*
 * rt_cls set rule filter inner-vid <UINT:innerVid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_inner_vid_innerVid(
    cparser_context_t *context,
    uint32_t  *innerVid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*innerVid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_INNER_VID_BIT;
    entry.filterRule.innerTagVid = *innerVid_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_inner_vid_innervid */

/*
 * rt_cls set rule filter inner-pri <UINT:innerPri>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_inner_pri_innerPri(
    cparser_context_t *context,
    uint32_t  *innerPri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*innerPri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_INNER_PRI_BIT;
    entry.filterRule.innerTagPri = *innerPri_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_inner_pri_innerpri */

/*
 * rt_cls set rule filter inner-dei <UINT:innerDei>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_inner_dei_innerDei(
    cparser_context_t *context,
    uint32_t  *innerDei_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*innerDei_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_INNER_DEI_BIT;
    entry.filterRule.innerTagDei = *innerDei_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_inner_dei_innerdei */

/*
 * rt_cls set rule filter ethertype <UINT:ethertype>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_ethertype_ethertype(
    cparser_context_t *context,
    uint32_t  *ethertype_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*ethertype_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_ETHERTYPE;
    entry.filterRule.etherType = *ethertype_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_ethertype_ethertype */

/*
 * rt_cls set rule filter dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > 63), CPARSER_ERR_INVALID_PARAMS);
    entry.filterRule.filterMask |= RT_CLS_FILTER_DSCP_BIT;
    entry.filterRule.dscp[*dscp_ptr/8] = (1 << (7-(*dscp_ptr%8)));
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_dscp_dscp */

/*
 * rt_cls set rule filter egress-port <UINT:egressPort>
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_filter_egress_port_egressPort(
    cparser_context_t *context,
    uint32_t  *egressPort_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    entry.filterRule.filterMask |= RT_CLS_FILTER_EGRESS_PORT_BIT;
    entry.filterRule.egressPortMask = *egressPort_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_filter_egress_port_egressport */

/*
 * rt_cls set rule action outer-tag ( transparent | tagging | modify | remove )
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_outer_tag_transparent_tagging_modify_remove(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if(0 == osal_strcmp("transparent", TOKEN_STR(5)))
        entry.outerTagAct.tagAction = RT_CLS_TAG_TRANSPARENT;
    else if(0 == osal_strcmp("tagging", TOKEN_STR(5)))
        entry.outerTagAct.tagAction = RT_CLS_TAG_TAGGING;
    else if('m' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagAction = RT_CLS_TAG_MODIFY;
    else if('r' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagAction = RT_CLS_TAG_REMOVE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_outer_tag_transparent_tagging_modify_remove */

/*
 * rt_cls set rule action outer-tpid ( assign | copy-outer | copy-inner ) { <UINT:tpid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_outer_tpid_assign_copy_outer_copy_inner_tpid(
    cparser_context_t *context,
    uint32_t  *tpid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagTpidAction = RT_CLS_TPID_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.outerTagAct.tagTpidAction = RT_CLS_TPID_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.outerTagAct.tagTpidAction = RT_CLS_TPID_COPY_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(tpid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*tpid_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
        entry.outerTagAct.assignedTpid = *tpid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_outer_tpid_assign_copy_outer_copy_inner_tpid */

/*
 * rt_cls set rule action outer-vid ( assign | copy-outer | copy-inner | transparent ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_outer_vid_assign_copy_outer_copy_inner_transparent_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagVidAction = RT_CLS_VID_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.outerTagAct.tagVidAction = RT_CLS_VID_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.outerTagAct.tagVidAction = RT_CLS_VID_COPY_FROM_2ND_TAG;
    else if('t' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagVidAction = RT_CLS_VID_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(vid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
        entry.outerTagAct.assignedVid = *vid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_outer_vid_assign_copy_outer_copy_inner_transparent_vid */


/*
 * rt_cls set rule action outer-pri ( assign | copy-outer | copy-inner | dscp-remapping | transparent ) { <UINT:pri> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_outer_pri_assign_copy_outer_copy_inner_dscp_remapping_transparent_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagPriAction = RT_CLS_PRI_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.outerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.outerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_2ND_TAG;
    else if('d' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_DSCP_REMAP;
    else if('t' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagPriAction = RT_CLS_PRI_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        entry.outerTagAct.assignedPri = *pri_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_outer_pri_assign_copy_outer_copy_inner_dscp_remapping_transparent_pri */

/*
 * rt_cls set rule action outer-dei ( assign | copy-outer | copy-inner ) { <UINT:dei> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_outer_dei_assign_copy_outer_copy_inner_dei(
    cparser_context_t *context,
    uint32_t  *dei_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.outerTagAct.tagDeiAction = RT_CLS_DEI_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.outerTagAct.tagDeiAction = RT_CLS_DEI_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.outerTagAct.tagDeiAction = RT_CLS_DEI_COPY_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(dei_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*dei_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
        entry.outerTagAct.assignedDei = *dei_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_outer_dei_assign_copy_outer_copy_inner_dei */

/*
 * rt_cls set rule action inner-tag ( transparent | tagging | modify | remove )
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_inner_tag_transparent_tagging_modify_remove(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if(0 == osal_strcmp("transparent", TOKEN_STR(5)))
        entry.innerTagAct.tagAction = RT_CLS_TAG_TRANSPARENT;
    else if(0 == osal_strcmp("tagging", TOKEN_STR(5)))
        entry.innerTagAct.tagAction = RT_CLS_TAG_TAGGING;
    else if('m' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagAction = RT_CLS_TAG_MODIFY;
    else if('r' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagAction = RT_CLS_TAG_REMOVE;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_inner_tag_transparent_tagging_modify_remove */

/*
 * rt_cls set rule action inner-tpid ( assign | copy-outer | copy-inner ) { <UINT:tpid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_inner_tpid_assign_copy_outer_copy_inner_tpid(
    cparser_context_t *context,
    uint32_t  *tpid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagTpidAction = RT_CLS_TPID_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.innerTagAct.tagTpidAction = RT_CLS_TPID_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.innerTagAct.tagTpidAction = RT_CLS_TPID_COPY_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(tpid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*tpid_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
        entry.innerTagAct.assignedTpid = *tpid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_inner_tpid_assign_copy_outer_copy_inner_tpid */

/*
 * rt_cls set rule action inner-vid ( assign | copy-outer | copy-inner | transparent ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_inner_vid_assign_copy_outer_copy_inner_transparent_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagVidAction = RT_CLS_VID_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.innerTagAct.tagVidAction = RT_CLS_VID_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.innerTagAct.tagVidAction = RT_CLS_VID_COPY_FROM_2ND_TAG;
    else if('t' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagVidAction = RT_CLS_VID_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(vid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
        entry.innerTagAct.assignedVid = *vid_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_inner_vid_assign_copy_outer_copy_inner_transparent_vid */

/*
 * rt_cls set rule action inner-pri ( assign | copy-outer | copy-inner | dscp-remapping | transparent ) { <UINT:pri> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_inner_pri_assign_copy_outer_copy_inner_dscp_remapping_transparent_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagPriAction = RT_CLS_PRI_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.innerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.innerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_2ND_TAG;
    else if('d' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagPriAction = RT_CLS_PRI_COPY_FROM_DSCP_REMAP;
    else if('t' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagPriAction = RT_CLS_PRI_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        entry.innerTagAct.assignedPri = *pri_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_inner_pri_assign_copy_outer_copy_inner_dscp_remapping_transparent_pri */

/*
 * rt_cls set rule action inner-dei ( assign | copy-outer | copy-inner ) { <UINT:dei> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_inner_dei_assign_copy_outer_copy_inner_dei(
    cparser_context_t *context,
    uint32_t  *dei_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.innerTagAct.tagDeiAction = RT_CLS_DEI_ASSIGN;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(5)))
        entry.innerTagAct.tagDeiAction = RT_CLS_DEI_COPY_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(5)))
        entry.innerTagAct.tagDeiAction = RT_CLS_DEI_COPY_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(dei_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*dei_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
        entry.innerTagAct.assignedDei = *dei_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_inner_dei_assign_copy_outer_copy_inner_dei */

/*
 * rt_cls set rule action downstream-pri-act ( assign | nop ) { <UINT:pri> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_rule_action_downstream_pri_act_assign_nop_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if('a' == TOKEN_CHAR(5,0))
        entry.dsPriAct =  RT_CLS_DS_PRI_ACT_ASSIGN;
    else if('n' == TOKEN_CHAR(5,0))
        entry.dsPriAct = RT_CLS_DS_PRI_ACT_NOP;
    else
        return CPARSER_ERR_INVALID_PARAMS;
    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        entry.dsPri = *pri_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_rule_action_downstream_pri_act_assign_nop_pri */

/*
 * rt_cls init
 */
cparser_result_t
cparser_cmd_rt_cls_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_cls_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_init */

/*
 * rt_cls add rule
 */
cparser_result_t
cparser_cmd_rt_cls_add_rule(
    cparser_context_t *context)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_cls_rule_add(&entry), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_add_rule */

/*
 * rt_cls delete rule connection-index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_cls_delete_rule_connection_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_cls_rule_delete(*index_ptr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_delete_rule_connection_index_index */

/*
 * rt_cls show rule */
cparser_result_t
cparser_cmd_rt_cls_show_rule(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    _cls_rule_show(&entry);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_show_rule */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) fwdPort <UINT:fwdPort> */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_fwdPort_fwdPort(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fwdPort_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_fwdPort_set(port,*fwdPort_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_fwdport_fwdport */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) stag-match <UINT:sMatch>
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_stag_match_sMatch(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *sMatch_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_STAG_MATCH, *sMatch_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_stag_match_smatch */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) ctag-match <UINT:cMatch>
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_ctag_match_cMatch(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *cMatch_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_CTAG_MATCH, *cMatch_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_ctag_match_cmatch */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) outer-stag ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_outer_stag_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    if('e' == TOKEN_CHAR(5,0))
        data = 1;
    else
        data = 0;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_OUTER_STAG, data), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_outer_stag_enable_disable */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) inner-stag ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_inner_stag_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    if('e' == TOKEN_CHAR(5,0))
        data = 1;
    else
        data = 0;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_INNER_STAG, data), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_inner_stag_enable_disable */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) outer-ctag ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_outer_ctag_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    if('e' == TOKEN_CHAR(5,0))
        data = 1;
    else
        data = 0;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_OUTER_CTAG, data), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_outer_ctag_enable_disable */

/*
 * rt_cls set port ( <PORT_LIST:ports> | all ) inner-ctag ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_cls_set_port_ports_all_inner_ctag_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    
    if('e' == TOKEN_CHAR(5,0))
        data = 1;
    else
        data = 0;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(port, RT_CLS_VLAN_INNER_CTAG, data), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_port_ports_all_inner_ctag_enable_disable */

/*
 * rt_cls set tpid s-tpid-a <UINT:sTpid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_tpid_s_tpid_a_sTpid(
    cparser_context_t *context,
    uint32_t  *sTpid_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(0, RT_CLS_VLAN_STAG_TPID_A, *sTpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_tpid_s_tpid_a_stpid */

/*
 * rt_cls set tpid s-tpid-b <UINT:sTpid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_tpid_s_tpid_b_sTpid(
    cparser_context_t *context,
    uint32_t  *sTpid_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(0, RT_CLS_VLAN_STAG_TPID_B, *sTpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_tpid_s_tpid_b_stpid */

/*
 * rt_cls set tpid user-stpid <UINT:sTpid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_tpid_user_stpid_sTpid(
    cparser_context_t *context,
    uint32_t  *sTpid_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(0, RT_CLS_VLAN_USER_STAG_TPID, *sTpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_tpid_user_stpid_stpid */

/*
 * rt_cls set tpid user-ctpid <UINT:cTpid>
 */
cparser_result_t
cparser_cmd_rt_cls_set_tpid_user_ctpid_cTpid(
    cparser_context_t *context,
    uint32_t  *cTpid_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_set(0, RT_CLS_VLAN_USER_CTAG_TPID, *cTpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_tpid_user_ctpid_ctpid */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) stag-match
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_stag_match(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_STAG_MATCH, &data), ret);
        diag_util_mprintf("Port %d: 0x%x\n",port , data);
    }

    

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_stag_match */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) ctag-match
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_ctag_match(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_CTAG_MATCH, &data), ret);
        diag_util_mprintf("Port %d: 0x%x\n",port , data);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_ctag_match */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) outer-stag
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_outer_stag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_OUTER_STAG, &data), ret);
        diag_util_mprintf("Port %d: %s\n",port , data == 1 ? "Enable" : "Disable" );
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_outer_stag */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) inner-stag
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_inner_stag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_INNER_STAG, &data), ret);
        diag_util_mprintf("Port %d: %s\n",port , data == 1 ? "Enable" : "Disable" );
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_inner_stag */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) outer-ctag
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_outer_ctag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_OUTER_CTAG, &data), ret);
        diag_util_mprintf("Port %d: %s\n",port , data == 1 ? "Enable" : "Disable" );
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_outer_ctag */

/*
 * rt_cls get port ( <PORT_LIST:ports> | all ) inner-ctag
 */
cparser_result_t
cparser_cmd_rt_cls_get_port_ports_all_inner_ctag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32_t data=0;
    rt_port_t port;
    diag_portlist_t portlist;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(port, RT_CLS_VLAN_INNER_CTAG, &data), ret);
        diag_util_mprintf("Port %d: %s\n",port , data == 1 ? "Enable" : "Disable" );
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_port_ports_all_inner_ctag */

/*
 * rt_cls get tpid s-tpid-a
 */
cparser_result_t
cparser_cmd_rt_cls_get_tpid_s_tpid_a(
    cparser_context_t *context)
{
    int32 ret;
    uint32_t data=0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(0, RT_CLS_VLAN_STAG_TPID_A, &data), ret);
    diag_util_mprintf("STAG TPID A: 0x%04x\n", data );

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_tpid_s_tpid_a */

/*
 * rt_cls get tpid s-tpid-b
 */
cparser_result_t
cparser_cmd_rt_cls_get_tpid_s_tpid_b(
    cparser_context_t *context)
{
    int32 ret;
    uint32_t data=0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(0, RT_CLS_VLAN_STAG_TPID_B, &data), ret);
    diag_util_mprintf("STAG TPID B: 0x%04x\n", data );

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_tpid_s_tpid_b */

/*
 * rt_cls get tpid user-stpid
 */
cparser_result_t
cparser_cmd_rt_cls_get_tpid_user_stpid(
    cparser_context_t *context)
{
    int32 ret;
    uint32_t data=0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(0, RT_CLS_VLAN_USER_STAG_TPID, &data), ret);
    diag_util_mprintf("User STAG TPID: 0x%04x\n", data );

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_tpid_user_stpid */

/*
 * rt_cls get tpid user-ctpid
 */
cparser_result_t
cparser_cmd_rt_cls_get_tpid_user_ctpid(
    cparser_context_t *context)
{
    int32 ret;
    uint32_t data=0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_cls_vlan_paser_get(0, RT_CLS_VLAN_USER_CTAG_TPID, &data), ret);
    diag_util_mprintf("User CTAG TPID: 0x%04x\n", data );

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_tpid_user_ctpid */

/*
 * rt_cls get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_cls_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    int32 ret,found=0;
    rt_cls_rule_info_t ruleInfo;
    uint32_t index;

    memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
    memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
    ret = rt_cls_rule_get(RT_CLS_GET_OP_INDEX, *index_ptr, RT_CLS_DIR_DS, &ruleInfo);
    if(ret == RT_ERR_OK)
    {
        found=1;
        diag_util_mprintf("***************************************\n");
        _cls_rule_show(&ruleInfo.rule);
        _cls_hwIndex_show(ruleInfo.hwIndex);
    }
    
    memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
    memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
    ret = rt_cls_rule_get(RT_CLS_GET_OP_INDEX, *index_ptr, RT_CLS_DIR_US, &ruleInfo);
    if(ret == RT_ERR_OK)
    {
        found=1;
        diag_util_mprintf("***************************************\n");
        _cls_rule_show(&ruleInfo.rule);
        _cls_hwIndex_show(ruleInfo.hwIndex);
    }
    
    if(found == 0)
        diag_util_mprintf("rt_cls rule not found\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_entry_index */

/*
 * rt_cls get entry valid
 */
cparser_result_t
cparser_cmd_rt_cls_get_entry_valid(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    int32 ret,found=0;
    rt_cls_rule_info_t ruleInfo;
    uint32_t index;

    memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
    memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
    //_cls_hwIndex_show(ruleInfo.hwIndex);
    ret = rt_cls_rule_get(RT_CLS_GET_OP_FIRST, 0, RT_CLS_DIR_DS, &ruleInfo);
    while(ret == RT_ERR_OK)
    {
        found=1;
        diag_util_mprintf("***************************************\n");
        _cls_rule_show(&ruleInfo.rule);
        _cls_hwIndex_show(ruleInfo.hwIndex);
        index = ruleInfo.rule.index;
        memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
        memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
        ret = rt_cls_rule_get(RT_CLS_GET_OP_NEXT, index, RT_CLS_DIR_DS, &ruleInfo);
    }

    memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
    memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
    ret = rt_cls_rule_get(RT_CLS_GET_OP_FIRST, 0, RT_CLS_DIR_US, &ruleInfo);
    while(ret == RT_ERR_OK)
    {
        found=1;
        diag_util_mprintf("***************************************\n");
        _cls_rule_show(&ruleInfo.rule);
        _cls_hwIndex_show(ruleInfo.hwIndex);
        index = ruleInfo.rule.index;
        memset(&ruleInfo,0,sizeof(rt_cls_rule_info_t));
        memset(ruleInfo.hwIndex,0xff,sizeof(uint16_t)*RT_CLS_HW_INDEX_MAX);
        ret = rt_cls_rule_get(RT_CLS_GET_OP_NEXT, index, RT_CLS_DIR_US, &ruleInfo);
    }

    if(found == 0)
        diag_util_mprintf("No any rt_cls rule\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_entry_valid */

/*
 * rt_cls clear ext-rule
 */
cparser_result_t
cparser_cmd_rt_cls_clear_ext_rule(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    memset(&extEntry, 0, sizeof(rt_cls_extend_rule_t));
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_clear_ext_rule */

/*
 * rt_cls set ext-rule filter tos-tc data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_tos_tc_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    extEntry.filterRule.mask.tosTc = *mask_ptr;
    extEntry.filterRule.data.tosTc = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_tos_tc_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter egrPort data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_egrPort_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.egressPort = *mask_ptr;
    extEntry.filterRule.data.egressPort = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_egrport_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter igrPort data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_igrPort_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ingressPort = *mask_ptr;
    extEntry.filterRule.data.ingressPort = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_igrport_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter sbit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_sbit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.sbit = *mask_ptr;
    extEntry.filterRule.data.sbit = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_sbit_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter cbit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_cbit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.cbit = *mask_ptr;
    extEntry.filterRule.data.cbit = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_cbit_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter s-tpid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_s_tpid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.sTpid = *mask_ptr;
    extEntry.filterRule.data.sTpid = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_s_tpid_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter pkt-type data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_pkt_type_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.pktType = *mask_ptr;
    extEntry.filterRule.data.pktType = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_pkt_type_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter ipv4 data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_ipv4_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ipv4 = *mask_ptr;
    extEntry.filterRule.data.ipv4 = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_ipv4_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter ipv6 data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_ipv6_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ipv6 = *mask_ptr;
    extEntry.filterRule.data.ipv6 = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_ipv6_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter igmp-mld data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_igmp_mld_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.igmpMld = *mask_ptr;
    extEntry.filterRule.data.igmpMld = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_igmp_mld_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter flowId data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_flowId_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.flowId = *mask_ptr;
    extEntry.filterRule.data.flowId = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_flowid_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter direction data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_direction_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.direction = *mask_ptr;
    extEntry.filterRule.data.direction = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_direction_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter c-vid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_c_vid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ctagVid = *mask_ptr;
    extEntry.filterRule.data.ctagVid = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_c_vid_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter c-pri data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_c_pri_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ctagPri = *mask_ptr;
    extEntry.filterRule.data.ctagPri = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_c_pri_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter c-dei data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_c_dei_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ctagCfi = *mask_ptr;
    extEntry.filterRule.data.ctagCfi = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_c_dei_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter outer-vid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_outer_vid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.outerVid = *mask_ptr;
    extEntry.filterRule.data.outerVid = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_outer_vid_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter outer-pri data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_outer_pri_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.outerPri = *mask_ptr;
    extEntry.filterRule.data.outerPri = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_outer_pri_data_data_mask_mask */

/*
 * rt_cls set ext-rule filter outer-dei data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_outer_dei_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.outerDei = *mask_ptr;
    extEntry.filterRule.data.outerDei = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_outer_dei_data_data_mask_mask */


/*
 * rt_cls set ext-rule filter ethertype data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_filter_ethertype_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    extEntry.filterRule.mask.ethertype = *mask_ptr;
    extEntry.filterRule.data.ethertype = *data_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_filter_ethertype_data_data_mask_mask */

/*
 * rt_cls set ext-rule action svlan-act ( transparent | delete | tpid_ori | tpid_a | tpid_b )
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_svlan_act_transparent_delete_tpid_ori_tpid_a_tpid_b(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    
    if('r' == TOKEN_CHAR(5,1))
        extEntry.sTagAct.tagAction = RT_CLS_EXT_STAG_TRANSPARENT;
    else if ('d' == TOKEN_CHAR(5,0))
        extEntry.sTagAct.tagAction = RT_CLS_EXT_STAG_REMOVE;
    else if ('o' == TOKEN_CHAR(5,5))
        extEntry.sTagAct.tagAction = RT_CLS_EXT_STAG_TPID_ORI;
    else if ('a' == TOKEN_CHAR(5,5))
        extEntry.sTagAct.tagAction = RT_CLS_EXT_STAG_TPID_A;
    else
        extEntry.sTagAct.tagAction = RT_CLS_EXT_STAG_TPID_B;
    

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_svlan_act_transparent_delete_tpid_ori_tpid_a_tpid_b */

/*
 * rt_cls set ext-rule action s-vid ( assign | copy-outer | copy-inner ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_s_vid_assign_copy_outer_copy_inner_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    if('a' == TOKEN_CHAR(5,0))
        extEntry.sTagAct.vidAction = RT_CLS_EXT_VID_ASSIGN;
    else if ('o' == TOKEN_CHAR(5,5))
        extEntry.sTagAct.vidAction = RT_CLS_EXT_VID_COPY_FROM_1ST_TAG;
    else
        extEntry.sTagAct.vidAction = RT_CLS_EXT_VID_COPY_FROM_2ND_TAG;

    if(vid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
        extEntry.sTagAct.assignedVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_s_vid_assign_copy_outer_copy_inner_vid */

/*
 * rt_cls set ext-rule action s-pri ( assign | copy-outer | copy-inner | from-dscp ) { <UINT:pri> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_s_pri_assign_copy_outer_copy_inner_from_dscp_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(5,0))
        extEntry.sTagAct.priAction = RT_CLS_EXT_PRI_ASSIGN;
    else if ('o' == TOKEN_CHAR(5,5))
        extEntry.sTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_1ST_TAG;
    else if ('i' == TOKEN_CHAR(5,5))
        extEntry.sTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_2ND_TAG;
    else
        extEntry.sTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_DSCP_REMAP;

    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        extEntry.sTagAct.assignedPri = *pri_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_s_pri_assign_copy_outer_copy_inner_from_dscp_pri */

/*
 * rt_cls set ext-rule action s-dei ( assign-0 | keep )
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_s_dei_assign_0_keep(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(5,0))
        extEntry.sTagAct.deiAction = RT_CLS_EXT_DEI_ASSIGN_0;
    else
        extEntry.sTagAct.deiAction = RT_CLS_EXT_DEI_KEEP;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_s_dei_assign_0_keep */

/*
 * rt_cls set ext-rule action cvlan-act ( transparent | delete | tagging )
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_cvlan_act_transparent_delete_tagging(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('r' == TOKEN_CHAR(5,1))
        extEntry.cTagAct.tagAction = RT_CLS_EXT_CTAG_TRANSPARENT;
    else if('e' == TOKEN_CHAR(5,1))
        extEntry.cTagAct.tagAction = RT_CLS_EXT_CTAG_REMOVE;
    else
        extEntry.cTagAct.tagAction = RT_CLS_EXT_CTAG_TAGING;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_cvlan_act_transparent_delete_tagging */

/*
 * rt_cls set ext-rule action c-vid ( assign | copy-outer | copy-inner | from-FDB ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_c_vid_assign_copy_outer_copy_inner_from_FDB_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(5,0))
        extEntry.cTagAct.vidAction = RT_CLS_EXT_VID_ASSIGN;
    else if('f' == TOKEN_CHAR(5,0))
        extEntry.cTagAct.vidAction = RT_CLS_EXT_VID_FROM_FDB;
    else if('o' == TOKEN_CHAR(5,5))
        extEntry.cTagAct.vidAction = RT_CLS_EXT_VID_COPY_FROM_1ST_TAG;
    else
        extEntry.cTagAct.vidAction = RT_CLS_EXT_VID_COPY_FROM_2ND_TAG;

    if(vid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
        extEntry.cTagAct.assignedVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_c_vid_assign_copy_outer_copy_inner_from_fdb_vid */

/*
 * rt_cls set ext-rule action c-pri ( assign | copy-outer | copy-inner | from-dscp ) { <UINT:pri> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_c_pri_assign_copy_outer_copy_inner_from_dscp_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(5,0))
        extEntry.cTagAct.priAction = RT_CLS_EXT_PRI_ASSIGN;
    else if('f' == TOKEN_CHAR(5,0))
        extEntry.cTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_DSCP_REMAP;
    else if('o' == TOKEN_CHAR(5,5))
        extEntry.cTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_1ST_TAG;
    else
        extEntry.cTagAct.priAction = RT_CLS_EXT_PRI_COPY_FROM_2ND_TAG;

    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        extEntry.cTagAct.assignedPri = *pri_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_c_pri_assign_copy_outer_copy_inner_from_dscp_pri */

/*
 * rt_cls set ext-rule action c-dei ( assign-0 | keep )
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_c_dei_assign_0_keep(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(5,0))
        extEntry.cTagAct.deiAction = RT_CLS_EXT_DEI_ASSIGN_0;
    else
        extEntry.cTagAct.deiAction = RT_CLS_EXT_DEI_KEEP;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_c_dei_assign_0_keep */

/*
 * rt_cls set ext-rule action grp-pol ( nop | assign ) { <UINT:grpPol> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_grp_pol_nop_assign_grpPol(
    cparser_context_t *context,
    uint32_t  *grpPol_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(5,0))
        extEntry.grpPolAct = RT_CLS_EXT_NOP;
    else
        extEntry.grpPolAct = RT_CLS_EXT_ASSIGN;

    if(grpPol_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*grpPol_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        extEntry.grpPolId = *grpPol_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_grp_pol_nop_assign_grppol */

/*
 * rt_cls set ext-rule action flow-id ( nop | assign ) { <UINT:flowId> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_flow_id_nop_assign_flowId(
    cparser_context_t *context,
    uint32_t  *flowId_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(5,0))
        extEntry.flowAct = RT_CLS_EXT_NOP;
    else
        extEntry.flowAct = RT_CLS_EXT_ASSIGN;

    if(flowId_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*flowId_ptr > 511), CPARSER_ERR_INVALID_PARAMS);
        extEntry.flowId = *flowId_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_flow_id_nop_assign_flowid */

/*
 * rt_cls set ext-rule action fwd ( nop | drop | trap | ldpid | mcgid ) { <UINT:fwdId> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_fwd_nop_drop_trap_ldpid_mcgid_fwdId(
    cparser_context_t *context,
    uint32_t  *fwdId_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(5,0))
        extEntry.fwdAct = RT_CLS_EXT_FWD_NOP;
    else if('d' == TOKEN_CHAR(5,0))
        extEntry.fwdAct = RT_CLS_EXT_FWD_DROP;
    else if('t' == TOKEN_CHAR(5,0))
        extEntry.fwdAct = RT_CLS_EXT_FWD_TRAP;
    else if('l' == TOKEN_CHAR(5,0))
        extEntry.fwdAct = RT_CLS_EXT_FWD_LDPID;
    else
        extEntry.fwdAct = RT_CLS_EXT_FWD_MCGID;

    if(fwdId_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*fwdId_ptr > 255), CPARSER_ERR_INVALID_PARAMS);
        extEntry.fwdId = *fwdId_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_fwd_nop_drop_trap_ldpid_mcgid_fwdid */

/*
 * rt_cls set ext-rule action cos ( nop | assign ) { <UINT:cos> }
 */
cparser_result_t
cparser_cmd_rt_cls_set_ext_rule_action_cos_nop_assign_cos(
    cparser_context_t *context,
    uint32_t  *cos_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(5,0))
        extEntry.cosAct = RT_CLS_EXT_NOP;
    else
        extEntry.cosAct = RT_CLS_EXT_ASSIGN;

    if(cos_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*cos_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        extEntry.cos = *cos_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_set_ext_rule_action_cos_nop_assign_cos */

/*
 * rt_cls add ext-rule index { <UINT:index> }
 */
cparser_result_t
cparser_cmd_rt_cls_add_ext_rule_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    if(index_ptr == NULL)
        return CPARSER_ERR_INVALID_PARAMS;

    extEntry.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rt_cls_extend_rule_add(&extEntry), ret);

    return CPARSER_OK;
}     /* end of cparser_cmd_rt_cls_add_ext_rule_index_index */

/*
 * rt_cls delete ext-rule index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_cls_delete_ext_rule_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_cls_extend_rule_delete(*index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_delete_ext_rule_index_index */

/*
 * rt_cls show ext-rule
 */
cparser_result_t
cparser_cmd_rt_cls_show_ext_rule(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    
    _cls_extend_rule_show(&extEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_show_ext_rule */

/*
 * rt_cls get ext-rule index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_cls_get_ext_rule_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rt_cls_extend_rule_t extRule;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&extRule, 0, sizeof(rt_cls_extend_rule_t));

    DIAG_UTIL_ERR_CHK(rt_cls_extend_rule_get(*index_ptr, &extRule), ret);

    _cls_extend_rule_show(&extRule);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_ext_rule_index_index */

/*
 * rt_cls get ext-rule index valid
 */
cparser_result_t
cparser_cmd_rt_cls_get_ext_rule_index_valid(
    cparser_context_t *context)
{
    rt_cls_extend_rule_t extRule;
    int32 ret;
    uint32 index = 0;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    memset(&extRule, 0, sizeof(rt_cls_extend_rule_t));
    while (RT_ERR_OK == (rt_cls_extend_rule_get(index, &extRule)))
    {
        if(extRule.valid)
        {
            _cls_extend_rule_show(&extRule);
        }
        index++;
        memset(&extRule, 0, sizeof(rt_cls_extend_rule_t));
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_cls_get_ext_rule_index_valid */

