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
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <rtk/qos.h>

#include <hal/common/halctrl.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>
#endif

/*
 * qos init
 */
cparser_result_t
cparser_cmd_qos_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rtk_qos_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_init */


/*
 * qos get priority-selector group-id <UINT:index>
 */
cparser_result_t
cparser_cmd_qos_get_priority_selector_group_id_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
#ifdef CONFIG_LUNA_G3_SERIES
    diag_util_mprintf("%-13s %s\n","TYPE","Weight");
    diag_util_mprintf("%-13s %-2d\n","dot1q",weight.weight_of_dot1q);
    diag_util_mprintf("%-13s %-2d\n","dscp",weight.weight_of_dscp);
#else
    diag_util_mprintf("%-13s %s\n","TYPE","Weight");
    diag_util_mprintf("%-13s %-2d\n","port",weight.weight_of_portBased);
    diag_util_mprintf("%-13s %-2d\n","dot1q",weight.weight_of_dot1q);
    diag_util_mprintf("%-13s %-2d\n","dscp",weight.weight_of_dscp);
    diag_util_mprintf("%-13s %-2d\n","acl",weight.weight_of_acl);
	diag_util_mprintf("%-13s %-2d\n", "svlan", weight.weight_of_svlanBased);
#ifndef CONFIG_SDK_RTL9607C
    diag_util_mprintf("%-13s %-2d\n","vlan",weight.weight_of_vlanBased);
    diag_util_mprintf("%-13s %-2d\n","lookup-table",weight.weight_of_lutFwd);
    diag_util_mprintf("%-13s %-2d\n","smac",weight.weight_of_saBaed);
    diag_util_mprintf("%-13s %-2d\n","l4",weight.weight_of_l4Based);
#endif
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_priority_selector_group_id_index */

/*
 * qos set priority-selector group-id <UINT:index> port <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_port_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_portBased = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_port_weight */

/*
 * qos set priority-selector group-id <UINT:index> dot1q <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_dot1q_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_dot1q = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_dot1q_weight */

/*
 * qos set priority-selector group-id <UINT:index> dscp <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_dscp_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_dscp = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_dscp_weight */

/*
 * qos set priority-selector group-id <UINT:index> acl <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_acl_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_acl = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_acl_weight */

/*
 * qos set priority-selector group-id <UINT:index> lookup-table <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_lookup_table_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_lutFwd = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_lookup_table_weight */

/*
 * qos set priority-selector group-id <UINT:index> smac <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_smac_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_saBaed = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_smac_weight */

/*
 * qos set priority-selector group-id <UINT:index> svlan <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_svlan_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_svlanBased = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_svlan_weight */

/*
 * qos set priority-selector group-id <UINT:index> vlan <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_vlan_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_vlanBased = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_vlan_weight */

/*
 * qos set priority-selector group-id <UINT:index> l4 <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_group_id_index_l4_weight(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t   weight;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(*index_ptr, &weight), ret);
    weight.weight_of_l4Based = *weight_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_set(*index_ptr, &weight), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_group_id_index_l4_weight */

/*
 * qos set priority-selector port ( <PORT_LIST:ports> | all ) group-id <UINT:index>
 */
cparser_result_t
cparser_cmd_qos_set_priority_selector_port_ports_all_group_id_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriSelGroup_set(port, *index_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_selector_port_ports_all_group_id_index */

/*
 * qos get priority-selector port ( <PORT_LIST:ports> | all ) group-id
 */
cparser_result_t
cparser_cmd_qos_get_priority_selector_port_ports_all_group_id(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
	uint32 index;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    diag_util_mprintf("Port Group\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriSelGroup_get(port, &index), ret);

        diag_util_mprintf("%-4d %d\n", port,index);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_priority_selector_port_ports_all_group_id */

/*
 * qos get priority-to-queue port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_qos_get_priority_to_queue_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32     index;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    diag_util_mprintf("Port  Index\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriMap_get(port, &index), ret);
        diag_util_mprintf("%-4d  %-4d\n",port,index);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_priority_to_queue_port_ports_all */

/*
 * qos set priority-to-queue port ( <PORT_LIST:ports> | all ) table <UINT:index>
 */
cparser_result_t
cparser_cmd_qos_set_priority_to_queue_port_ports_all_table_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriMap_set(port, *index_ptr), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_to_queue_port_ports_all_table_index */

/*
 * qos get priority-to-queue
 */
cparser_result_t
cparser_cmd_qos_get_priority_to_queue(
    cparser_context_t *context)
{
    rtk_qos_pri2queue_t pri2queue;
    uint32 priority,index;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    for(index=0 ; index<=HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() ; index++ )
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_priMap_get(index, &pri2queue), ret);

        diag_util_mprintf("Index pri_0 pri_1 pri_2 pri_3 pri_4 pri_5 pri_6 pri_7\n");
        diag_util_printf("%-5d ",index);

        for(priority=0 ;priority<=HAL_INTERNAL_PRIORITY_MAX(); priority++ )
        {
            diag_util_printf("%-5d ",pri2queue.pri2queue[priority]);
        }

        diag_util_printf("\n");
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_priority_to_queue */

/*
 * qos get priority-to-queue table <UINT:index>
 */
cparser_result_t
cparser_cmd_qos_get_priority_to_queue_table_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_qos_pri2queue_t pri2queue;
    uint32 priority;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_qos_priMap_get(*index_ptr, &pri2queue), ret);

    diag_util_mprintf("Index pri_0 pri_1 pri_2 pri_3 pri_4 pri_5 pri_6 pri_7\n");
    diag_util_printf("%-5d ",*index_ptr);

    for(priority=0 ;priority<=HAL_INTERNAL_PRIORITY_MAX(); priority++ )
    {
        diag_util_printf("%-5d ",pri2queue.pri2queue[priority]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_priority_to_queue_table_index */

/*
 * qos set priority-to-queue table <UINT:index> priority <MASK_LIST:priority> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_qos_set_priority_to_queue_table_index_priority_priority_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *priority_ptr,
    uint32_t  *qid_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_qos_pri2queue_t pri2queue;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_priMap_get(*index_ptr, &pri2queue), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 6), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        pri2queue.pri2queue[index]=*qid_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_priMap_set(*index_ptr, &pri2queue), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_priority_to_queue_table_index_priority_priority_queue_id_qid */

/*
 * qos get remapping dot1p
 */
cparser_result_t
cparser_cmd_qos_get_remapping_dot1p(
    cparser_context_t *context)
{
    uint32      grpIdx=0;
    uint32      dp=0;
    uint32      dot1pPri;
    uint32      intPri;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("1p Priority    internal priority\n");
    for(dot1pPri =0; dot1pPri <= RTK_DOT1P_PRIORITY_MAX ; dot1pPri++ )
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemapGroup_get(grpIdx,
                                                      dot1pPri,
                                                      &intPri,
                                                      &dp), ret);
        diag_util_mprintf("%-15d  %-4d\n",dot1pPri,intPri);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remapping_dot1p */

/*
 * qos set remapping dot1p dot1p-priority <UINT:dot1p_priority> internal-priority <UINT:internal_priority>
 */
cparser_result_t
cparser_cmd_qos_set_remapping_dot1p_dot1p_priority_dot1p_priority_internal_priority_internal_priority(
    cparser_context_t *context,
    uint32_t  *dot1p_priority_ptr,
    uint32_t  *internal_priority_ptr)
{
    uint32      grpIdx=0;
    uint32      dp=0;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemapGroup_set(grpIdx,
                                                  *dot1p_priority_ptr,
                                                  *internal_priority_ptr,
                                                  dp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remapping_dot1p_dot1p_priority_dot1p_priority_internal_priority_internal_priority */

/*
 * qos get remapping dscp
 */
cparser_result_t
cparser_cmd_qos_get_remapping_dscp(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32      grpIdx=0;
    uint32      dp=0;
    uint32      dscp;
    uint32      intPri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("DSCP  Priority\n");
    for(dscp =0; dscp <= RTK_VALUE_OF_DSCP_MAX ; dscp++ )
    {
        if((ret = rtk_qos_dscpPriRemapGroup_get(grpIdx,dscp,&intPri,&dp)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        diag_util_mprintf("%-4d  %-4d\n",dscp,intPri);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remapping_dscp */

/*
 * qos set remapping dscp dscp <MASK_LIST:dscp> internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_qos_set_remapping_dscp_dscp_dscp_internal_priority_priority(
    cparser_context_t *context,
    char * *dscp_ptr,
    uint32_t  *priority_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32      grpIdx=0;
    uint32      dp=0;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 5), ret);

    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_dscpPriRemapGroup_set(grpIdx, index, *priority_ptr, dp), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remapping_dscp_dscp_dscp_internal_priority_priority */

/*
 * qos get avb remapping
 */
cparser_result_t
cparser_cmd_qos_get_avb_remapping(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 pri;
    uint32 remapPri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Priority  Remap-Pri\n");

    for(pri=0;  pri<=RTK_DOT1P_PRIORITY_MAX ; pri++)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, pri, PRIf, &remapPri)), ret);
                diag_util_mprintf("%-10d  %-4d\n",pri,remapPri);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(APOLLOMP_AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_PRIf, &remapPri)), ret);
                diag_util_mprintf("%-10d  %-4d\n",pri,remapPri);

                break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_avb_remapping */

/*
 * qos get avb remapping internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_qos_get_avb_remapping_internal_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 remapPri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Priority  Remap-Pri\n");

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_array_field_read(AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, *priority_ptr, PRIf, &remapPri)), ret);
            diag_util_mprintf("%-10d  %-4d\n",*priority_ptr,remapPri);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_array_field_read(APOLLOMP_AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, *priority_ptr, APOLLOMP_PRIf, &remapPri)), ret);
            diag_util_mprintf("%-10d  %-4d\n",*priority_ptr,remapPri);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;

}    /* end of cparser_cmd_qos_get_avb_remapping_internal_priority_priority */

/*
 * qos set avb remapping internal-priority <UINT:priority> user-priority <UINT:user_priority>
 */
cparser_result_t
cparser_cmd_qos_set_avb_remapping_internal_priority_priority_user_priority_user_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *user_priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 remapPri;

    DIAG_UTIL_PARAM_CHK();
    remapPri = *user_priority_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_array_field_write(AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, *priority_ptr, PRIf, &remapPri)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_array_field_write(APOLLOMP_AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, *priority_ptr, APOLLOMP_PRIf, &remapPri)), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_avb_remapping_internal_priority_priority_user_priority_user_priority */

/*
 * qos get avb remapping port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_qos_get_avb_remapping_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Port State\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(AVB_PORT_ENr, port, REG_ARRAY_INDEX_NONE, ENf, &enable)), ret);
                diag_util_mprintf("%-4d %s\n",port,diagStr_enable[enable]);

                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(APOLLOMP_AVB_PORT_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)), ret);
                diag_util_mprintf("%-4d %s\n",port,diagStr_enable[enable]);

                break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_avb_remapping_port_ports_all_state */

/*
 * qos set avb remapping port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_qos_set_avb_remapping_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(7,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_write(AVB_PORT_ENr, port,REG_ARRAY_INDEX_NONE, ENf, &enable)), ret);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_write(APOLLOMP_AVB_PORT_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)), ret);
                break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_avb_remapping_port_ports_all_state_enable_disable */

/*
 * qos get remapping forward-to-cpu
 */
cparser_result_t
cparser_cmd_qos_get_remapping_forward_to_cpu(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 pri;
    uint32 inrPri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    for(inrPri = 0; inrPri <= HAL_INTERNAL_PRIORITY_MAX(); inrPri++)
    {
        if((ret = rtk_qos_fwd2CpuPriRemap_get(inrPri,&pri)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        diag_util_mprintf("Priority:%2d Remapping prority:%2d\n",inrPri,pri);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remapping_forward_to_cpu */

/*
 * qos get remapping forward-to-cpu internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_qos_get_remapping_forward_to_cpu_internal_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 pri;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_qos_fwd2CpuPriRemap_get(*priority_ptr,&pri)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Priority:%2d Remapping prority:%2d\n",*priority_ptr,pri);
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remapping_forward_to_cpu_internal_priority_priority */

/*
 * qos set remapping forward-to-cpu internal-priority <UINT:priority> remapping-priority <UINT:remapping_priority>
 */
cparser_result_t
cparser_cmd_qos_set_remapping_forward_to_cpu_internal_priority_priority_remapping_priority_remapping_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *remapping_priority_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_qos_fwd2CpuPriRemap_set(*priority_ptr,*remapping_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remapping_forward_to_cpu_internal_priority_priority_remapping_priority_remapping_priority */

/*
 * qos get remapping port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_qos_get_remapping_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32   pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port prioirty\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPri_get(port, &pri), ret);
        diag_util_mprintf("%-4d %-4d\n",port,pri);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remapping_port_ports_all */

/*
 * qos set remapping port ( <PORT_LIST:ports> | all ) internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_qos_set_remapping_port_ports_all_internal_priority_priority(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *priority_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPri_set(port, *priority_ptr), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remapping_port_ports_all_internal_priority_priority */



/*
 * qos get remarking port ( <PORT_LIST:ports> | all ) dscp source
 */
cparser_result_t
cparser_cmd_qos_get_remarking_port_ports_all_dscp_source(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_qos_dscpRmkSrc_t source;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("%-4s %-11s\n","Port","DSCP source");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDscpRemarkSrcSel_get(port, &source), ret);
        if(DSCP_RMK_SRC_INT_PRI == source)
            diag_util_mprintf("%-4d %-11s\n",port,"int-pri");
        else if(DSCP_RMK_SRC_DSCP == source)
            diag_util_mprintf("%-4d %-11s\n",port,"dscp");
        else
            diag_util_mprintf("%-4d %-11s\n",port,"user-pri");

    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remarking_port_ports_all_dscp_source */

/*
 * qos set remarking port ( <PORT_LIST:ports> | all ) dscp source ( internal-priority | user-priority | dscp )
 */
cparser_result_t
cparser_cmd_qos_set_remarking_port_ports_all_dscp_source_internal_priority_user_priority_dscp(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_qos_dscpRmkSrc_t source;

    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(7,0))
        source = DSCP_RMK_SRC_INT_PRI;
    else if('u'==TOKEN_CHAR(7,0))
        source = DSCP_RMK_SRC_USER_PRI;
    else
        source = DSCP_RMK_SRC_DSCP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDscpRemarkSrcSel_set(port, source), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_port_ports_all_dscp_source_internal_priority_dscp */



/*
 * qos get remarking dscp ( inter-priority | original-dscp )
 */
cparser_result_t
cparser_cmd_qos_get_remarking_dscp_inter_priority_original_dscp(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qos_dscpRmkSrc_t    source;
    uint32      grpIdx=0;
    rtk_pri_t   intPri;
    uint32      dp=0;
    uint32      oriDscp;
    uint32      dscp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    if ('i' == TOKEN_CHAR(4,0))
        source = DSCP_RMK_SRC_INT_PRI;
    else
        source = DSCP_RMK_SRC_DSCP;

    if(DSCP_RMK_SRC_INT_PRI == source)
    {
        diag_util_mprintf("%-3s  %-13s\n","Pri", "Remarking_dscp");
        for(intPri=0 ; intPri<=HAL_INTERNAL_PRIORITY_MAX() ; intPri++)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarkGroup_get(grpIdx, intPri, dp, &dscp), ret);
            diag_util_mprintf("%-3d  %-13d\n",intPri, dscp);
        }
    }
    else
    {
        diag_util_mprintf("%-4s  %-13s\n","dscp", "Remarking_dscp");
        for(oriDscp=0 ; oriDscp<=RTK_VALUE_OF_DSCP_MAX ; oriDscp++)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscp2DscpRemarkGroup_get(grpIdx, oriDscp, &dscp), ret);
            diag_util_mprintf("%-3d  %-13d\n",oriDscp, dscp);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remarking_dscp_inter_priority_original_dscp */

/*
 * qos set remarking dscp inter-priority <UINT:priority> remarking-dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_qos_set_remarking_dscp_inter_priority_priority_remarking_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *dscp_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32      grpIdx=0;
    uint32      dp=0;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarkGroup_set(grpIdx, *priority_ptr, dp, *dscp_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_dscp_inter_priority_priority_remarking_dscp_dscp */


/*
 * qos set remarking dscp original-dscp <MASK_LIST:original_dscp> remarking-dscp <UINT:remarking_dscp>
 */
cparser_result_t
cparser_cmd_qos_set_remarking_dscp_original_dscp_original_dscp_remarking_dscp_remarking_dscp(
    cparser_context_t *context,
    char * *original_dscp_ptr,
    uint32_t  *remarking_dscp_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32      grpIdx=0;
    uint32      oriDscp;
    diag_mask_t mask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK64(mask, 5), ret);

    DIAG_UTIL_MASK_SCAN(mask, oriDscp)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_dscp2DscpRemarkGroup_set(grpIdx, oriDscp, *remarking_dscp_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_dscp_original_dscp_original_dscp_remarking_dscp_remarking_dscp */


/*
 * qos get remarking ( dot1p | dscp ) port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_qos_get_remarking_dot1p_dscp_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t  enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('o' == TOKEN_CHAR(3,1))
    {
        diag_util_mprintf("\ndot1p remarking state\n");
        diag_util_mprintf("port state\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarkEnable_get(port, &enable), ret);
            diag_util_mprintf("%4d %s\n",port,diagStr_enable[enable]);
        }
    }
    else
    {
        diag_util_mprintf("\ndscp remarking state\n");
        diag_util_mprintf("port state\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarkEnable_get(port, &enable), ret);
            diag_util_mprintf("%4d %s\n",port,diagStr_enable[enable]);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remarking_dot1p_dscp_port_ports_all_state */

/*
 * qos set remarking ( dot1p | dscp ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_qos_set_remarking_dot1p_dscp_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t  enable;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('d' == TOKEN_CHAR(7,0))
        enable = DISABLED;
    else
        enable = ENABLED;

    if ('o' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarkEnable_set(port, enable), ret);
        }
    }
    else
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarkEnable_set(port, enable), ret);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_dot1p_dscp_port_ports_all_state_disable_enable */

/*
 * qos get remarking dot1p
 */
cparser_result_t
cparser_cmd_qos_get_remarking_dot1p(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 grpIdx=0;
    uint32 dp=0;
    uint32 intrPri;
    uint32 remarkPri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("%-12s %-11s \n","internal-pri","remark-pri");
    for(intrPri=0 ; intrPri<=HAL_INTERNAL_PRIORITY_MAX() ; intrPri++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarkGroup_get(grpIdx, intrPri, dp, &remarkPri), ret);
        diag_util_mprintf("%-12d %-11d \n",intrPri, remarkPri);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remarking_dot1p */

/*
 * qos set remarking dot1p user-priority <UINT:priority> dot1p-priority <UINT:dot1p_priority>
 */
cparser_result_t
cparser_cmd_qos_set_remarking_dot1p_user_priority_priority_dot1p_priority_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *dot1p_priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 grpIdx=0;
    uint32 dp=0;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarkGroup_set(grpIdx, *priority_ptr, dp, *dot1p_priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_dot1p_user_priority_priority_dot1p_priority_dot1p_priority */

/*
 * qos get scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_qos_get_scheduling_algorithm_port_ports_all_queue_id_qid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg;
#ifdef CONFIG_SDK_APOLLO
    uint32 type;
#endif
    rtk_qos_queue_weights_t weight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(3 == port)
                {
                    continue;
                }

                if(port <= 2)
                {
                    phyPort = port;
                    reg     = WFQ_TYPE_P02_CFGr;
                }
                else if(port > 3)
                {
                    phyPort = port - 4;
                    reg     = WFQ_TYPE_P46_CFGr;

                }
                DIAG_UTIL_ERR_CHK(reg_array_field_read(reg, phyPort, *qid_ptr, QUEUE_TYPEf, &type), ret);
                diag_util_mprintf("port: %d queue: %d type:%s\n",port,*qid_ptr,(type==0)?DIAG_STR_QUEUE_STRICT:DIAG_STR_QUEUE_WFQ );
                break;
#endif
            default:
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port,&weight), ret);
                diag_util_mprintf("port: %d queue: %d type:%s\n",port,*qid_ptr,(weight.weights[*qid_ptr]==0)?DIAG_STR_QUEUE_STRICT:DIAG_STR_QUEUE_WFQ );
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_scheduling_algorithm_port_ports_all_queue_id_qid */

/*
 * qos set scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> ( strict | wfq )
 */
cparser_result_t
cparser_cmd_qos_set_scheduling_algorithm_port_ports_all_queue_id_qid_strict_wfq(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg;
    uint32 type;
    rtk_qos_queue_weights_t weight;


    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(8,0))
        type = 0;  /*strict*/
    else
        type = 1;  /*wfq*/

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(3 == port)
                {
                    continue;
                }

                if(port <= 2)
                {
                    phyPort = port;
                    reg     = WFQ_TYPE_P02_CFGr;
                }
                else if(port > 3)
                {
                    phyPort = port - 4;
                    reg     = WFQ_TYPE_P46_CFGr;

                }
                DIAG_UTIL_ERR_CHK(reg_array_field_write(reg, phyPort, *qid_ptr, QUEUE_TYPEf, &type), ret);

                break;
#endif
            default:
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port, &weight), ret);
                weight.weights[*qid_ptr] = type;
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_set(port, &weight), ret);
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_scheduling_algorithm_port_ports_all_queue_id_qid_strict_wfq */

/*
 * qos get scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_qos_get_scheduling_queue_weight_port_ports_all_queue_id_qid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg,field;
    uint32 qid,phyQid;
    rtk_qos_queue_weights_t qWeight;
#ifdef CONFIG_SDK_APOLLO
    uint32 weight;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    qid = *qid_ptr;

    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(3 == port)
                {
                    continue;
                }

                if(port <= 2)
                {
                    phyPort = port;
                    if(0 == qid)
                    {
                        reg    = WFQ_P02_CFG0r;
                        field  = WEIGHT0f;
                        phyQid = REG_ARRAY_INDEX_NONE;
                    }
                    else
                    {
                        reg    = WFQ_P02_CFG1_7r;
                        field  = WEIGHT1_7f;
                        phyQid = qid;
                    }
                }
                else if(port > 3)
                {
                    phyPort = port - 4;
                    if(0 == qid)
                    {
                        reg    = WFQ_P46_CFG0r;
                        field  = WEIGHT0f;
                        phyQid = REG_ARRAY_INDEX_NONE;
                    }
                    else
                    {
                        reg    = WFQ_P46_CFG1_7r;
                        field  = WEIGHT1_7f;
                        phyQid = qid;
                    }
                }

                DIAG_UTIL_ERR_CHK(reg_array_field_read(reg, phyPort, phyQid, field, &weight), ret);
                diag_util_mprintf("port: %d queue: %d weight:%d\n",port,*qid_ptr,weight);
                break;
#endif
            default:
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port, &qWeight), ret);
                diag_util_mprintf("port: %d queue: %d weight:%d\n",port,*qid_ptr,qWeight.weights[*qid_ptr]);
                break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_scheduling_queue_weight_port_ports_all_queue_id_qid */

/*
 * qos set scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> weight <UINT:weight>
 */
cparser_result_t
cparser_cmd_qos_set_scheduling_queue_weight_port_ports_all_queue_id_qid_weight_weight(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *weight_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 phyPort;
    uint32 reg,field;
    uint32 qid,phyQid;
    rtk_qos_queue_weights_t qWeight;
    uint32 weight;

    weight = *weight_ptr;
    qid = *qid_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK((RTK_MAX_NUM_OF_QUEUE<=*qid_ptr), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(3 == port)
                {
                    continue;
                }

                if(port <= 2)
                {
                    phyPort = port;
                    if(0 == qid)
                    {
                        reg    = WFQ_P02_CFG0r;
                        field  = WEIGHT0f;
                        phyQid = REG_ARRAY_INDEX_NONE;
                    }
                    else
                    {
                        reg    = WFQ_P02_CFG1_7r;
                        field  = WEIGHT1_7f;
                        phyQid = qid;
                    }
                }
                else if(port > 3)
                {
                    phyPort = port - 4;
                    if(0 == qid)
                    {
                        reg    = WFQ_P46_CFG0r;
                        field  = WEIGHT0f;
                        phyQid = REG_ARRAY_INDEX_NONE;
                    }
                    else
                    {
                        reg    = WFQ_P46_CFG1_7r;
                        field  = WEIGHT1_7f;
                        phyQid = qid;
                    }
                }

                DIAG_UTIL_ERR_CHK(reg_array_field_write(reg, phyPort, phyQid, field, &weight), ret);

                break;
#endif
            default:
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(port, &qWeight), ret);
                qWeight.weights[qid] = weight;
                DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_set(port, &qWeight), ret);
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_scheduling_queue_weight_port_ports_all_queue_id_qid_weight_weight */

/*
 * qos set scheduling type ( wrr | wfq )
 */
cparser_result_t
cparser_cmd_qos_set_scheduling_type_wrr_wfq(
    cparser_context_t *context)
{
    rtk_qos_scheduling_type_t type;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('r' == TOKEN_CHAR(4,1))
        type = RTK_QOS_WRR;  /*wrr*/
    else
        type = RTK_QOS_WFQ;  /*wfq*/

    DIAG_UTIL_ERR_CHK(rtk_qos_schedulingType_set(type), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_scheduling_type_wrr_wfq */

/*
 * qos get scheduling type
 */
cparser_result_t
cparser_cmd_qos_get_scheduling_type(
    cparser_context_t *context)
{
    rtk_qos_scheduling_type_t type;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_qos_schedulingType_get(&type), ret);

    diag_util_mprintf("schedule type:");

    if(RTK_QOS_WRR==type)
        diag_util_mprintf("WRR\n");
    else
        diag_util_mprintf("WFQ\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_scheduling_type */

/*
 * qos get remarking port ( <PORT_LIST:ports> | all ) dot1p source
 */
cparser_result_t
cparser_cmd_qos_get_remarking_port_ports_all_dot1p_source(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_qos_do1pRmkSrc_t source;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Source\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDot1pRemarkSrcSel_get(port, &source), ret);
        diag_util_mprintf("%-4d %s\n",port,diagStr_qos1premarkSource[source]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_qos_get_remarking_port_ports_all_dot1p_source */

/*
 * qos set remarking port ( <PORT_LIST:ports> | all ) dot1p source ( original-ctag-priority | user-priority )
 */
cparser_result_t
cparser_cmd_qos_set_remarking_port_ports_all_dot1p_source_original_ctag_priority_user_priority(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_qos_dscpRmkSrc_t source;

    DIAG_UTIL_PARAM_CHK();

    if('o'==TOKEN_CHAR(7,0))
        source = DOT1P_RMK_ORI_CTAG_PRI;
    else if('u'==TOKEN_CHAR(7,0))
        source = DOT1P_RMK_SRC_USER_PRI;
    else
        return RT_ERR_FAILED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDot1pRemarkSrcSel_set(port, source), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_qos_set_remarking_port_ports_all_dot1p_source_original_ctag_priority_user_priority */


