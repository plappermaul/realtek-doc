/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Define diag shell functions for dot1x.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) dot1x diag shell.
 */


#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/qos.h>
#include <rtk/switch.h>
#include <rtk/port.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_qos.h>
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
#endif
#ifdef CMD_QOS_SET_REMAPPING_FORWARD_TO_CPU_INTERNAL_PRIORITY_INTERNAL_PRIORITY_REMAPPING_PRIORITY_REMAPPING_PRIORITY
/*
 * qos set remapping forward-to-cpu internal-priority <UINT:internal_priority> remapping-priority <UINT:remapping_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_forward_to_cpu_internal_priority_internal_priority_remapping_priority_remapping_priority(cparser_context_t *context,
    uint32_t *internal_priority_ptr,
    uint32_t *remapping_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_pkt2CpuPriRemap_set(unit, *internal_priority_ptr, *remapping_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DROP_PRECEDENCE_SELECTOR_PORT_PORTS_ALL
/*
 * qos get drop-precedence-selector port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_drop_precedence_selector_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32 ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_dpSelWeight_t weightOfDpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDpSel_get(unit, port, &weightOfDpSel), ret);

        diag_util_mprintf("Port %2d Drop Precedence Selector\n", port);
        diag_util_mprintf("\tDSCP-based      : %u\n", weightOfDpSel.weight_of_dscp);
        diag_util_mprintf("\tInner-tag-based : %u\n", weightOfDpSel.weight_of_innerTag);
        diag_util_mprintf("\tOuter-tag-based : %u\n", weightOfDpSel.weight_of_outerTag);
#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf("\tMPLS-based      : %u\n", weightOfDpSel.weight_of_mpls);
        }
#endif
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DROP_PRECEDENCE_SELECTOR_PORT_PORTS_ALL_DSCP_WEIGHT
/*
 * qos set drop-precedence-selector port ( <PORT_LIST:ports> | all ) dscp <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_drop_precedence_selector_port_ports_all_dscp_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *weight_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_dpSelWeight_t weightOfDpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDpSel_get(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        weightOfDpSel.weight_of_dscp = *weight_ptr;

        if ((ret = rtk_qos_portDpSel_set(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DROP_PRECEDENCE_SELECTOR_PORT_PORTS_ALL_INNER_TAG_WEIGHT
/*
 * qos set drop-precedence-selector port ( <PORT_LIST:ports> | all ) inner-tag <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_drop_precedence_selector_port_ports_all_inner_tag_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *weight_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_dpSelWeight_t weightOfDpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDpSel_get(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        weightOfDpSel.weight_of_innerTag = *weight_ptr;

        if ((ret = rtk_qos_portDpSel_set(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DROP_PRECEDENCE_SELECTOR_PORT_PORTS_ALL_OUTER_TAG_WEIGHT
/*
 * qos set drop-precedence-selector port ( <PORT_LIST:ports> | all ) outer-tag <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_drop_precedence_selector_port_ports_all_outer_tag_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *weight_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_dpSelWeight_t weightOfDpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDpSel_get(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        weightOfDpSel.weight_of_outerTag = *weight_ptr;

        if ((ret = rtk_qos_portDpSel_set(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DROP_PRECEDENCE_SELECTOR_PORT_PORTS_ALL_MPLS_WEIGHT
/*
 * qos set drop-precedence-selector port ( <PORT_LIST:ports> | all ) mpls <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_drop_precedence_selector_port_ports_all_mpls_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *weight_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_dpSelWeight_t weightOfDpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDpSel_get(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        weightOfDpSel.weight_of_mpls = *weight_ptr;

        if ((ret = rtk_qos_portDpSel_set(unit, port, &weightOfDpSel)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_PORT_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> port <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_port_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    priSelWeight.weight_of_portBased = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_PORT_INNER_PRIORITY_WEIGHT
/*
 *qos set priority-selector group-id <UINT:index> port-inner-priority <UINT:weight>
*/
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_port_inner_priority_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    priSelWeight.weight_of_portBasedIpri = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_PORT_OUTER_PRIORITY_WEIGHT
/*
 *qos set priority-selector group-id <UINT:index> port-outer-priority <UINT:weight>
*/
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_port_outer_priority_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    priSelWeight.weight_of_portBasedOpri = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_DSCP_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> dscp <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_dscp_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    priSelWeight.weight_of_dscp = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_INNER_TAG_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> inner-tag <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_inner_tag_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    priSelWeight.weight_of_innerTag = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_OUTER_TAG_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> outer-tag <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_outer_tag_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_outerTag = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_PE_TAG_WEIGHT
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_pe_tag_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_peTag = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_INGRESS_ACL_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> ingress-acl <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_ingress_acl_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_inAcl = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_VLAN_ACL_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> vlan-acl <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_vlan_acl_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_vlanAcl = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_MAC_BASED_VLAN_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> mac-based-vlan <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_mac_based_vlan_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_macVlan = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_PROTOCOL_BASED_VLAN_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> protocol-based-vlan <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_protocol_based_vlan_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_protoVlan = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_ROUTING_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> routing <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_routing_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_routing = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_DOT11E_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> dot11e <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_dot11e_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_80211e = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_TUNNEL_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> tunnel <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_tunnel_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_tunnel = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_GROUP_ID_INDEX_MPLS_WEIGHT
/*
 * qos set priority-selector group-id <UINT:index> mpls <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_group_id_index_mpls_weight(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSelWeight_t  priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priSelGroup_get(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    priSelWeight.weight_of_mpls = *weight_ptr;

    if ((ret = rtk_qos_priSelGroup_set(unit, *index_ptr, &priSelWeight)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_AVB_PORT_PORTS_ALL_SR_CLASS_A_SR_CLASS_B_STATE_ENABLE_DISABLE
/*
 * qos set avb port ( <PORT_LIST:ports> | all ) ( sr-class-a | sr-class-b ) state ( enable | disable )
 */
cparser_result_t cparser_cmd_qos_set_avb_port_ports_all_sr_class_a_sr_class_b_state_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_avbSrClass_t    srClass;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    srClass = ('a' == TOKEN_CHAR(5, 9))? AVB_SR_CLASS_A : AVB_SR_CLASS_B;
    enable = ('e' == TOKEN_CHAR(7, 0))? ENABLED : DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portAvbStreamReservationClassEnable_set(unit, port, srClass, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_AVB_SR_CLASS_A_SR_CLASS_B_PRIORITY_PRIORITY
/*
 * qos set avb ( sr-class-a | sr-class-b ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_avb_sr_class_a_sr_class_b_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('a' == TOKEN_CHAR(3, 9))
    {
        srClassConf.class_a_priority = (rtk_pri_t)*priority_ptr;
    }
    else
    {
        srClassConf.class_b_priority = (rtk_pri_t)*priority_ptr;
    }

    if ((ret = rtk_qos_avbStreamReservationConfig_set(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_AVB_SR_CLASS_A_SR_CLASS_B_QUEUE_ID_QUEUE_ID
/*
 * qos set avb ( sr-class-a | sr-class-b ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_qos_set_avb_sr_class_a_sr_class_b_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('a' == TOKEN_CHAR(3, 9))
    {
        srClassConf.class_a_queue_id = (rtk_qid_t)*queue_id_ptr;
    }
    else
    {
        srClassConf.class_b_queue_id = (rtk_qid_t)*queue_id_ptr;
    }

    if ((ret = rtk_qos_avbStreamReservationConfig_set(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_AVB_NON_SR_CLASS_A_NON_SR_CLASS_B_REDIRECT_QUEUE_ID_QUEUE_ID
/*
 * qos set avb ( non-sr-class-a | non-sr-class-b ) redirect-queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_qos_set_avb_non_sr_class_a_non_sr_class_b_redirect_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('a' == TOKEN_CHAR(3, 13))
    {
        srClassConf.class_non_a_redirect_queue_id = (rtk_qid_t)*queue_id_ptr;
    }
    else
    {
        srClassConf.class_non_b_redirect_queue_id = (rtk_qid_t)*queue_id_ptr;
    }

    if ((ret = rtk_qos_avbStreamReservationConfig_set(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_AVB_NON_SR_CLASS_A_NON_SR_CLASS_B_REMARK_PRIORITY_PRIORITY
/*
 * qos set avb ( non-sr-class-a | non-sr-class-b ) remark-priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_avb_non_sr_class_a_non_sr_class_b_remark_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('a' == TOKEN_CHAR(3, 13))
    {
        srClassConf.class_non_a_remark_priority = (rtk_qid_t)*priority_ptr;
    }
    else
    {
        srClassConf.class_non_b_remark_priority = (rtk_qid_t)*priority_ptr;
    }

    if ((ret = rtk_qos_avbStreamReservationConfig_set(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_DROP_PRECEDENCE_SOURCE_DEI_DSCP
/*
 * qos set remapping drop-precedence source ( dei | dscp )
 */
cparser_result_t cparser_cmd_qos_set_remapping_drop_precedence_source_dei_dscp(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dpSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(5,1))
    {
        type = DP_SRC_DEI_BASED;
    }
    else if ('s' == TOKEN_CHAR(5,1))
    {
        type = DP_SRC_DSCP_BASED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_qos_dpSrcSel_set(unit, type)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_SELECTOR_PORT_PORTS_ALL_GROUP_ID_INDEX
/*
 * qos set priority-selector port ( <PORT_LIST:ports> | all ) group-id <UINT:index>
 */
cparser_result_t cparser_cmd_qos_set_priority_selector_port_ports_all_group_id_index(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portPriSelGroup_set(unit, port, *index_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_QUEUE_NUMBER_QUEUE_NUM
/*
 * qos set queue number <UINT:queue_num>
 */
cparser_result_t cparser_cmd_qos_set_queue_number_queue_num(cparser_context_t *context,
    uint32_t *queue_num_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_queueNum_set(unit, *queue_num_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_TO_QUEUE_QUEUE_NUMBER_QUEUE_NUM_PRIORITY_PRIORITY_QUEUE_ID_QID
/*
 * qos set priority-to-queue queue-number <UINT:queue_num> priority <UINT:priority> queue-id <UINT:qid>
 */
cparser_result_t cparser_cmd_qos_set_priority_to_queue_queue_number_queue_num_priority_priority_queue_id_qid(cparser_context_t *context,
    uint32_t *queue_num_ptr,
    uint32_t *priority_ptr,
    uint32_t *qid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_pri2queue_t pri2qid;
    uint32 priority_max;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_priMap_get(unit, *queue_num_ptr, &pri2qid)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    DIAG_OM_GET_CHIP_CAPACITY(unit, priority_max, internal_priority_max);

    if (*priority_ptr > priority_max)
    {
        DIAG_ERR_PRINT(RT_ERR_QOS_INT_PRIORITY);
        return CPARSER_NOT_OK;
    }
    pri2qid.pri2queue[*priority_ptr] = *qid_ptr;

    if ((ret = rtk_qos_priMap_set(unit, *queue_num_ptr, &pri2qid)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_PRIORITY_TO_QUEUE_PRIORITY_PRIORITY_QUEUE_ID_QID
/*
 * qos set priority-to-queue priority <UINT:priority> queue-id <UINT:qid>
 */
cparser_result_t cparser_cmd_qos_set_priority_to_queue_priority_priority_queue_id_qid(cparser_context_t *context,
    uint32_t *priority_ptr,
    uint32_t *qid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_pri2QidMap_set(unit, *priority_ptr, *qid_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_REMAPPING_PORT_PORTS_ALL_INTERNAL_PRIORITY_PRIORITY
/*
 * qos set remapping port ( <PORT_LIST:ports> | all ) internal-priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_ports_all_internal_priority_priority(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        srcVal.src.port = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_qos_portPri_set(unit, port, *priority_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_PB_PRI, srcVal, *priority_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PORT_PORTS_ALL_INNER_TAG_PRIORITY_PRIORITY
/*
 * qos set remapping port ( <PORT_LIST:ports> | all ) inner-tag priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_ports_all_inner_tag_priority_priority(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portInnerPri_set(unit, port, *priority_ptr), ret);
        }
        else
#endif
        {
            srcVal.src.port = port;
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_PB_INNER_PRI, srcVal, *priority_ptr), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PORT_PORTS_ALL_OUTER_TAG_PRIORITY_PRIORITY
/*
 * qos set remapping port ( <PORT_LIST:ports> | all ) outer-tag priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_ports_all_outer_tag_priority_priority(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portOuterPri_set(unit, port, *priority_ptr), ret);
        }
        else
#endif
        {
            srcVal.src.port = port;
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_PB_OUTER_PRI, srcVal, *priority_ptr), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_DEI_SYSTEM_DEI_DEI_DROP_PRECEDENCE_DROP_PRECEDENCE
/*
 * qos set remapping dei system dei <UINT:dei> drop-precedence <UINT:drop_precedence>
 */
cparser_result_t cparser_cmd_qos_set_remapping_dei_system_dei_dei_drop_precedence_drop_precedence(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *drop_precedence_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_deiDpRemap_set(unit, *dei_ptr, *drop_precedence_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_DEI_TAG_SELECTOR_PORT_PORTS_ALL_INNER_TAG_OUTER_TAG
/*
 * qos set remapping dei tag-selector port ( <PORT_LIST:ports> | all ) ( inner-tag | outer-tag )
 */
cparser_result_t cparser_cmd_qos_set_remapping_dei_tag_selector_port_ports_all_inner_tag_outer_tag(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_deiSel_t type;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('i' == TOKEN_CHAR(7,0))
    {
        type = DEI_SEL_INNER_TAG;
    }
    else if ('o' == TOKEN_CHAR(7,0))
    {
        type = DEI_SEL_OUTER_TAG;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDEISrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_INNER_TAG_SYSTEM_INNER_PRIORITY_INNER_PRIORITY_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping inner-tag system inner-priority <UINT:inner_priority> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_inner_tag_system_inner_priority_inner_priority_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *inner_priority_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_1pPriRemap_set(unit, *inner_priority_ptr, *internal_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        srcVal.src.tag.pri = *inner_priority_ptr;
        srcVal.src.tag.dei = 0;
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_set(unit, PRI_SRC_INNER_USER_PRI, srcVal, *internal_priority_ptr), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_DSCP_SYSTEM_DSCP_DSCP_DROP_PRECEDENCE_DROP_PRECEDENCE
/*
 * qos set remapping dscp system dscp <MASK_LIST:dscp> drop-precedence <UINT:drop_precedence>
 */
cparser_result_t cparser_cmd_qos_set_remapping_dscp_system_dscp_dscp_drop_precedence_drop_precedence(cparser_context_t *context,
    char **dscp_ptr,
    uint32_t *drop_precedence_ptr)
{
    uint32      unit = 0, dscp;
    int32       ret = RT_ERR_FAILED;
    diag_mask_t dscpMask;
    rtk_qos_dpSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(dscpMask, 6, DIAG_MASKTYPE_DSCP), ret);
    DIAG_UTIL_MASK_SCAN(dscpMask, dscp)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_qos_dscpDpRemap_set(unit, dscp, *drop_precedence_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            srcVal.src.dscp = dscp;
            if ((ret = rtk_qos_dpRemap_set(unit, DP_SRC_DSCP_BASED, srcVal, *drop_precedence_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_DSCP_SYSTEM_DSCP_DSCP_INTERNAL_PRIORITY_PRIORITY
/*
 * qos set remapping dscp system dscp <MASK_LIST:dscp> internal-priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_dscp_system_dscp_dscp_internal_priority_priority(cparser_context_t *context,
    char **dscp_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0, dscp;
    int32       ret = RT_ERR_FAILED;
    diag_mask_t dscpMask;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(dscpMask, 6, DIAG_MASKTYPE_DSCP), ret);
    DIAG_UTIL_MASK_SCAN(dscpMask, dscp)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_qos_dscpPriRemap_set(unit, dscp, *priority_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            srcVal.src.dscp = dscp;
            if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_DSCP, srcVal, *priority_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_INNER_TAG_SYSTEM_DEI_DEI_PRIORITY_PRIORITY_DROP_PRECEDENCE_DROP_PRECEDENCE
/*
 * qos set remapping inner-tag system dei <UINT:dei> priority <UINT:priority> drop-precedence <UINT:drop_precedence>
 */
cparser_result_t cparser_cmd_qos_set_remapping_inner_tag_system_dei_dei_priority_priority_drop_precedence_drop_precedence(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *priority_ptr,
    uint32_t *drop_precedence_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dpSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tag.dei = *dei_ptr;
    srcVal.src.tag.pri = *priority_ptr;
    if ((ret = rtk_qos_dpRemap_set(unit, DP_SRC_INNER_PRI_BASED, srcVal, *drop_precedence_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_INNER_TAG_SYSTEM_DEI_DEI_PRIORITY_PRIORITY_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping inner-tag system dei <UINT:dei> priority <UINT:priority> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_inner_tag_system_dei_dei_priority_priority_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *priority_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tag.dei = *dei_ptr;
    srcVal.src.tag.pri = *priority_ptr;
    if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_INNER_USER_PRI, srcVal, *internal_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_OUTER_TAG_SYSTEM_DEI_DEI_PRIORITY_PRIORITY_DROP_PRECEDENCE_DROP_PRECEDENCE
/*
 * qos set remapping outer-tag system dei <UINT:dei> priority <UINT:priority> drop-precedence <UINT:drop_precedence>
 */
cparser_result_t cparser_cmd_qos_set_remapping_outer_tag_system_dei_dei_priority_priority_drop_precedence_drop_precedence(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *priority_ptr,
    uint32_t *drop_precedence_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dpSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tag.dei = *dei_ptr;
    srcVal.src.tag.pri = *priority_ptr;
    if ((ret = rtk_qos_dpRemap_set(unit, DP_SRC_OUTER_PRI_BASED, srcVal, *drop_precedence_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_OUTER_TAG_SYSTEM_DEI_DEI_PRIORITY_PRIORITY_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping outer-tag system dei <UINT:dei> priority <UINT:priority> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_outer_tag_system_dei_dei_priority_priority_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *priority_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_outer1pPriRemap_set(unit, *priority_ptr, *dei_ptr, *internal_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        srcVal.src.tag.dei = *dei_ptr;
        srcVal.src.tag.pri = *priority_ptr;
        if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_OUTER_USER_PRI, srcVal, *internal_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PE_TAG_SYSTEM_DEI_DEI_PRIORITY_PRIORITY_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping pe-tag system dei <UINT:dei> priority <UINT:priority> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_pe_tag_system_dei_dei_priority_priority_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *dei_ptr,
    uint32_t *priority_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tag.dei = *dei_ptr;
    srcVal.src.tag.pri = *priority_ptr;
    if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_PE_PRI, srcVal, *internal_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_80211E_SYSTEM_TID_TID_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping 80211e system tid <UINT:tid> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_80211e_system_tid_tid_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *tid_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tid = *tid_ptr;
    if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_80211E, srcVal, *internal_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_MPLS_SYSTEM_TC_TC_INTERNAL_PRIORITY_INTERNAL_PRIORITY
/*
 * qos set remapping mpls system tc <UINT:tc> internal-priority <UINT:internal_priority>
 */
cparser_result_t cparser_cmd_qos_set_remapping_mpls_system_tc_tc_internal_priority_internal_priority(cparser_context_t *context,
    uint32_t *tc_ptr,
    uint32_t *internal_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tc = *tc_ptr;
    if ((ret = rtk_qos_priRemap_set(unit, PRI_SRC_MPLS, srcVal, *internal_priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_INNER_TAG_OUTER_TAG_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * qos set remarking ( dscp | inner-tag | outer-tag ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_inner_tag_outer_tag_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    int32   (*fp)(uint32, rtk_port_t, rtk_enable_t);
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('i' == TOKEN_CHAR(3,0))
        {
            fp = rtk_qos_port1pRemarkEnable_set;
        }
        else if ('d' == TOKEN_CHAR(3,0))
        {
            fp = rtk_qos_portDscpRemarkEnable_set;
        }
        else if ('o' == TOKEN_CHAR(3,0))
        {
            fp = rtk_qos_portOut1pRemarkEnable_set;
        }
        else
        {
            diag_util_printf("User config : Error!\n");
            return CPARSER_NOT_OK;
        }

        if ('e' == TOKEN_CHAR(7,0))
        {
            enable = ENABLED;
        }
        else if ('d' == TOKEN_CHAR(7,0))
        {
            enable = DISABLED;
        }
        else
        {
            diag_util_printf("User config : Error!\n");
            return CPARSER_NOT_OK;
        }

        if ((ret = fp(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DEI_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * qos set remarking dei port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(7,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDeiRemarkEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DEI_SYSTEM_DROP_PRECEDENCE_DROP_PRECEDENCE_DEI_DEI
/*
 * qos set remarking dei system drop-precedence <UINT:drop_precedence> dei <UINT:dei>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_system_drop_precedence_drop_precedence_dei_dei(cparser_context_t *context,
    uint32_t *drop_precedence_ptr,
    uint32_t *dei_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_qos_deiRmkVal_t     rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_deiRemark_set(unit, *drop_precedence_ptr, *dei_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.dp.val = *drop_precedence_ptr;
        if ((ret = rtk_qos_deiRemarking_set(unit, DEI_RMK_SRC_DP, rmkVal, *dei_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DEI_SYSTEM_INTERNAL_PRIORITY_PRIORITY_DEI_DEI
/*
 * qos set remarking dei system internal_priority <UINT:priority> dei <UINT:dei>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_system_internal_priority_priority_dei_dei(cparser_context_t *context,
    uint32_t *priority_ptr,
    uint32_t *dei_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_qos_deiRmkVal_t     rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    rmkVal.pri.val = *priority_ptr;
    if ((ret = rtk_qos_deiRemarking_set(unit, DEI_RMK_SRC_INT_PRI, rmkVal, *dei_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_REMARKING_DEI_TAG_SELECTOR_PORT_PORTS_ALL_INNER_TAG_OUTER_TAG
/*
 * qos set remarking dei tag-selector port ( <PORT_LIST:ports> | all ) ( inner-tag | outer-tag )
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_tag_selector_port_ports_all_inner_tag_outer_tag(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_deiSel_t type;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(7,0))
    {
        type = DEI_SEL_INNER_TAG;
    }
    else if ('o' == TOKEN_CHAR(7,0))
    {
        type = DEI_SEL_OUTER_TAG;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portDeiRemarkTagSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_DSCP_DSCP_REMARK_INNER_PRIORITY_REMARK_INNER_PRIORITY
/*
 * qos set remarking inner-tag system dscp <UINT:dscp> remark-inner-priority <UINT:remark_inner_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_dscp_dscp_remark_inner_priority_remark_inner_priority(cparser_context_t *context,
    uint32_t *dscp_ptr,
    uint32_t *remark_inner_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_dscp2Dot1pRemark_set(unit, *dscp_ptr, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.dscp.val = *dscp_ptr;
        if ((ret = rtk_qos_1pRemarking_set(unit, DOT_1P_RMK_SRC_DSCP, rmkVal, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_INNER_PRIORITY_INNER_PRIORITY_REMARK_INNER_PRIORITY_REMARK_INNER_PRIORITY
/*
 * qos set remarking inner-tag system inner-priority <UINT:inner_priority> remark-inner-priority <UINT:remark_inner_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_inner_priority_inner_priority_remark_inner_priority_remark_inner_priority(cparser_context_t *context,
    uint32_t *inner_priority_ptr,
    uint32_t *remark_inner_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_1pRemark_set(unit, *inner_priority_ptr, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *inner_priority_ptr;
        if ((ret = rtk_qos_1pRemarking_set(unit, DOT_1P_RMK_SRC_USER_PRI, rmkVal, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_OUTER_PRIORITY_OUTER_PRIORITY_REMARK_INNER_PRIORITY_REMARK_INNER_PRIORITY
/*
 * qos set remarking inner-tag system outer-priority <UINT:outer_priority> remark-inner-priority <UINT:remark_inner_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_outer_priority_outer_priority_remark_inner_priority_remark_inner_priority(cparser_context_t *context,
    uint32_t *outer_priority_ptr,
    uint32_t *remark_inner_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_1pRemark_set(unit, *outer_priority_ptr, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *outer_priority_ptr;
        if ((ret = rtk_qos_1pRemarking_set(unit, DOT_1P_RMK_SRC_OUTER_USER_PRI, rmkVal, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_INTERNAL_PRIORITY_INTERNAL_PRIORITY_REMARK_INNER_PRIORITY_REMARK_INNER_PRIORITY
/*
 * qos set remarking inner-tag system internal-priority <UINT:internal_priority> remark-inner-priority <UINT:remark_inner_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_internal_priority_internal_priority_remark_inner_priority_remark_inner_priority(cparser_context_t *context,
    uint32_t *internal_priority_ptr,
    uint32_t *remark_inner_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_1pRemark_set(unit, *internal_priority_ptr, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *internal_priority_ptr;
        if ((ret = rtk_qos_1pRemarking_set(unit, DOT_1P_RMK_SRC_INT_PRI, rmkVal, *remark_inner_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_SOURCE_INTERNAL_PRIORITY
/*
 * qos set remarking inner-tag system source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_source_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_INT_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_SOURCE_INNER_PRIORITY
/*
 * qos set remarking inner-tag system source inner-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_source_inner_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_INNER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_SOURCE_OUTER_PRIORITY
/*
 * qos set remarking inner-tag system source outer-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_source_outer_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_OUTER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_INNER_TAG_SYSTEM_SOURCE_DSCP
/*
 * qos set remarking inner-tag system source dscp
 */
cparser_result_t cparser_cmd_qos_set_remarking_inner_tag_system_source_dscp(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_1pRemarkSrcSel_set(unit, PRI_SRC_DSCP)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_PRIORITY_PRIORITY
/*
 * qos set default-priority inner-tag system priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   dot1p_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    dot1p_pri = *priority_ptr;
    if ((ret = rtk_qos_1pDfltPri_set(unit, dot1p_pri)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_SOURCE_DSCP
/*
 * qos set remarking outer-tag system source dscp
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_source_dscp(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_outer1pRemarkSrcSel_set(unit, PRI_SRC_DSCP)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_SOURCE_INNER_PRIORITY
/*
 * qos set remarking outer-tag system source inner-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_source_inner_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_outer1pRemarkSrcSel_set(unit, PRI_SRC_INNER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_SOURCE_INTERNAL_PRIORITY
/*
 * qos set remarking outer-tag system source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_source_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_outer1pRemarkSrcSel_set(unit, PRI_SRC_INT_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_SOURCE_OUTER_PRIORITY
/*
 * qos set remarking outer-tag system source outer-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_source_outer_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_outer1pRemarkSrcSel_set(unit, PRI_SRC_OUTER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_DSCP_DSCP_REMARK_OUTER_PRIORITY_REMARK_OUTER_PRIORITY
/*
 * qos set remarking outer-tag system dscp <UINT:dscp> remark-outer-priority <UINT:remark_outer_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_dscp_dscp_remark_outer_priority_remark_outer_priority(cparser_context_t *context,
    uint32_t *dscp_ptr,
    uint32_t *remark_outer_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_dscp2Outer1pRemark_set(unit, *dscp_ptr, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.dscp.val = *dscp_ptr;
        if ((ret = rtk_qos_outer1pRemarking_set(unit, OUTER_1P_RMK_SRC_DSCP, rmkVal, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_INNER_PRIORITY_INNER_PRIORITY_REMARK_OUTER_PRIORITY_REMARK_OUTER_PRIORITY
/*
 * qos set remarking outer-tag system inner-priority <UINT:inner_priority> remark-outer-priority <UINT:remark_outer_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_inner_priority_inner_priority_remark_outer_priority_remark_outer_priority(cparser_context_t *context,
    uint32_t *inner_priority_ptr,
    uint32_t *remark_outer_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_outer1pRemark_set(unit, *inner_priority_ptr, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *inner_priority_ptr;
        if ((ret = rtk_qos_outer1pRemarking_set(unit, OUTER_1P_RMK_SRC_INNER_USER_PRI, rmkVal, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_OUTER_PRIORITY_OUTER_PRIORITY_REMARK_OUTER_PRIORITY_REMARK_OUTER_PRIORITY
/*
 * qos set remarking outer-tag system outer-priority <UINT:outer_priority> remark-outer-priority <UINT:remark_outer_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_outer_priority_outer_priority_remark_outer_priority_remark_outer_priority(cparser_context_t *context,
    uint32_t *outer_priority_ptr,
    uint32_t *remark_outer_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_outer1pRemark_set(unit, *outer_priority_ptr, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *outer_priority_ptr;
        if ((ret = rtk_qos_outer1pRemarking_set(unit, OUTER_1P_RMK_SRC_USER_PRI, rmkVal, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_SYSTEM_INTERNAL_PRIORITY_INTERNAL_PRIORITY_REMARK_OUTER_PRIORITY_REMARK_OUTER_PRIORITY
/*
 * qos set remarking outer-tag system internal-priority <UINT:internal_priority> remark-outer-priority <UINT:remark_outer_priority>
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_system_internal_priority_internal_priority_remark_outer_priority_remark_outer_priority(cparser_context_t *context,
    uint32_t *internal_priority_ptr,
    uint32_t *remark_outer_priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_outer1pRemark_set(unit, *internal_priority_ptr, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *internal_priority_ptr;
        if ((ret = rtk_qos_outer1pRemarking_set(unit, OUTER_1P_RMK_SRC_INT_PRI, rmkVal, *remark_outer_priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_INTERNAL_PRIORITY_PRIORITY_REMARK_DSCP_REMARK_DSCP
/*
 * qos set remarking dscp system internal-priority <UINT:priority> remark-dscp <UINT:remark_dscp>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_internal_priority_priority_remark_dscp_remark_dscp(cparser_context_t *context,
    uint32_t *priority_ptr,
    uint32_t *remark_dscp_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_dscpRemark_set(unit, *priority_ptr, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *priority_ptr;
        if ((ret = rtk_qos_dscpRemarking_set(unit, DSCP_RMK_SRC_INT_PRI, rmkVal, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_INNER_PRIORITY_PRIORITY_REMARK_DSCP_REMARK_DSCP
/*
 * qos set remarking dscp system inner-priority <UINT:priority> remark-dscp <UINT:remark_dscp>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_inner_priority_priority_remark_dscp_remark_dscp(cparser_context_t *context,
    uint32_t *priority_ptr,
    uint32_t *remark_dscp_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_dscpRemark_set(unit, *priority_ptr, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *priority_ptr;
        if ((ret = rtk_qos_dscpRemarking_set(unit, DSCP_RMK_SRC_USER_PRI, rmkVal, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_OUTER_PRIORITY_PRIORITY_REMARK_DSCP_REMARK_DSCP
/*
 * qos set remarking dscp system outer-priority <UINT:priority> remark-dscp <UINT:remark_dscp>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_outer_priority_priority_remark_dscp_remark_dscp(cparser_context_t *context,
    uint32_t *priority_ptr,
    uint32_t *remark_dscp_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_qos_dscpRemark_set(unit, *priority_ptr, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        rmkVal.pri.val = *priority_ptr;
        if ((ret = rtk_qos_dscpRemarking_set(unit, DSCP_RMK_SRC_OUTER_USER_PRI, rmkVal, *remark_dscp_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_DSCP_DSCP_REMARK_DSCP_REMARK_DSCP
/*
 * qos set remarking dscp system dscp <MASK_LIST:dscp> remark-dscp <UINT:remark_dscp>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_dscp_dscp_remark_dscp_remark_dscp(cparser_context_t *context,
    char **dscp_ptr,
    uint32_t *remark_dscp_ptr)
{
    uint32      unit = 0, dscp;
    int32       ret = RT_ERR_FAILED;
    diag_mask_t dscpMask;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(dscpMask, 6, DIAG_MASKTYPE_DSCP), ret);
    DIAG_UTIL_MASK_SCAN(dscpMask, dscp)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_qos_dscp2DscpRemark_set(unit, dscp, *remark_dscp_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            rmkVal.dscp.val = dscp;
            if ((ret = rtk_qos_dscpRemarking_set(unit, DSCP_RMK_SRC_DSCP, rmkVal, *remark_dscp_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_DROP_PRECEDENCE_PRECEDENCE_INTERNAL_PRIORITY_PRIORITY_REMARK_DSCP_REMARK_DSCP
/*
 * qos set remarking dscp system drop-precedence <UINT:precedence> internal-priority <UINT:priority> remark-dscp <UINT:remark_dscp>
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_drop_precedence_precedence_internal_priority_priority_remark_dscp_remark_dscp(cparser_context_t *context,
    uint32_t *precedence_ptr,
    uint32_t *priority_ptr,
    uint32_t *remark_dscp_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    rmkVal.intPriDp.dp = *precedence_ptr;
    rmkVal.intPriDp.pri = *priority_ptr;
    if ((ret = rtk_qos_dscpRemarking_set(unit, DSCP_RMK_SRC_DP_INT_PRI, rmkVal, *remark_dscp_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_INNER_PRIORITY
/*
 * qos set remarking dscp system source inner-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_inner_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_INNER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_INTERNAL_PRIORITY
/*
 * qos set remarking dscp system source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_INT_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_OUTER_PRIORITY
/*
 * qos set remarking dscp system source outer-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_outer_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_OUTER_USER_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_DSCP
/*
 * qos set remarking dscp system source dscp
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_dscp(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_DSCP)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_DROP_PRECEDENCE
/*
 * qos set remarking dscp system source drop-precedence
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_drop_precedence(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_DP)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DSCP_SYSTEM_SOURCE_DROP_PRECEDENCE_AND_INTERNAL_PRIORITY
/*
 * qos set remarking dscp system source drop-precedence-and-internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_dscp_system_source_drop_precedence_and_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dscpRemarkSrcSel_set(unit, PRI_SRC_DP_INT_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DEI_SYSTEM_SOURCE_INTERNAL_PRIORITY
/*
 * qos set remarking dei system source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_system_source_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_deiRemarkSrcSel_set(unit, DEI_RMK_SRC_INT_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_DEI_SYSTEM_SOURCE_DROP_PRECEDENCE
/*
 * qos set remarking dei system source drop-precedence
 */
cparser_result_t cparser_cmd_qos_set_remarking_dei_system_source_drop_precedence(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_deiRemarkSrcSel_set(unit, DEI_RMK_SRC_DP)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE_COPY_INNER_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) source copy-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_source_copy_inner_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[7].buf, "copy-inner-priority"))
    {
        type = OUTER_1P_DFLT_SRC_USER_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE_COPY_INTERNAL_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) source copy-internal-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_source_copy_internal_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, OUTER_1P_DFLT_SRC_INT_PRI)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DEI_SYSTEM_DROP_PRECEDENCE
/*
 * qos get remarking dei system drop-precedence
 */
cparser_result_t cparser_cmd_qos_get_remarking_dei_system_drop_precedence(cparser_context_t *context)
{
    int32     ret;
    uint32      unit = 0;
    uint32      dp;
    uint32      dei;
    uint32      dp_max;
    rtk_qos_deiRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, dp_max, drop_precedence_max);

    diag_util_mprintf("Drop Precedence\t  DEI Value\n");
    for (dp = 0; dp <= dp_max; dp++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_deiRemark_get(unit, dp, &dei),ret);
        }
        else
#endif
        {
            rmkVal.dp.val = dp;
            DIAG_UTIL_ERR_CHK(rtk_qos_deiRemarking_get(unit, DEI_RMK_SRC_DP, rmkVal, &dei),ret);
        }

        diag_util_mprintf("DP %u : %u\n", dp, dei);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DEI_SYSTEM_INTERNAL_PRIORITY
/*
 * qos get remarking dei system internal_priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_dei_system_internal_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    uint32      int_pri;
    uint32      dei;
    rtk_pri_t   pri_max;
    rtk_qos_deiRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("Internal Priority\t  DEI Value\n");
    for (int_pri = 0; int_pri <= pri_max; int_pri++)
    {
        rmkVal.pri.val = int_pri;
        DIAG_UTIL_ERR_CHK(rtk_qos_deiRemarking_get(unit, DEI_RMK_SRC_INT_PRI, rmkVal, &dei),ret);

        diag_util_mprintf("INT_PRI %u : %u\n", int_pri, dei);
    }

    return CPARSER_OK;
}
#endif



#ifdef CMD_QOS_GET_REMARKING_DEI_TAG_SELECTOR_PORT_PORTS_ALL
/*
 * qos get remarking dei tag-selector port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_remarking_dei_tag_selector_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_deiSel_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("DEI Remarking Tag Selection of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDeiRemarkTagSel_get(unit, port, &type),ret);
        if (DEI_SEL_INNER_TAG == type)
        {
            diag_util_mprintf("Port %2d : Inner-tag\n", port);
        }
        else
        {
            diag_util_mprintf("Port %2d : Outer-tag\n", port);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_INNER_TAG_SYSTEM_INNER_PRIORITY
/*
 * qos get remarking inner-tag system inner-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_inner_tag_system_inner_priority(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_1pRmkVal_t  rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("Inner Priority\t  TX Inner Priority\n");
    for (int_pri = 0; int_pri <= pri_max; int_pri++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemark_get(unit, int_pri, &dot1p_pri),ret);
        }
        else
#endif
        {
            rmkVal.pri.val = int_pri;
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarking_get(unit, DOT_1P_RMK_SRC_USER_PRI, rmkVal, &dot1p_pri),ret);
        }

        diag_util_mprintf("Priority %u : \t\t%u\n", int_pri, dot1p_pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_INNER_TAG_SYSTEM_INTERNAL_PRIORITY
/*
 * qos get remarking inner-tag system internal-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_inner_tag_system_internal_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_1pRmkVal_t  rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("Internal Priority\t  Inner Priority\n");
    for (int_pri = 0; int_pri <= pri_max; int_pri++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemark_get(unit, int_pri, &dot1p_pri),ret);
        }
        else
#endif
        {
            rmkVal.pri.val = int_pri;
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarking_get(unit, DOT_1P_RMK_SRC_INT_PRI, rmkVal, &dot1p_pri),ret);
        }

       diag_util_mprintf("Priority %u : \t\t%u\n", int_pri, dot1p_pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_INNER_TAG_SYSTEM_OUTER_PRIORITY
/*
 * qos get remarking inner-tag system outer-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_inner_tag_system_outer_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_1pRmkVal_t  rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("Internal Priority\t  Outer Priority\n");
    for (int_pri = 0; int_pri <= pri_max; int_pri++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemark_get(unit, int_pri, &dot1p_pri),ret);
        }
        else
#endif
        {
            rmkVal.pri.val = int_pri;
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarking_get(unit, DOT_1P_RMK_SRC_OUTER_USER_PRI, rmkVal, &dot1p_pri),ret);
        }

        diag_util_mprintf("Priority %u : %u\n", int_pri, dot1p_pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_INNER_TAG_SYSTEM_DSCP
/*
 * qos get remarking inner-tag system dscp
 */
cparser_result_t cparser_cmd_qos_get_remarking_inner_tag_system_dscp(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0, dscp;
    rtk_pri_t   dot1p_pri;
    rtk_qos_1pRmkVal_t  rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("DSCP\t\t  Inner Priority\n");
    for (dscp = 0; dscp <= RTK_VALUE_OF_DSCP_MAX; dscp++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscp2Dot1pRemark_get(unit, dscp, &dot1p_pri), ret);
        }
        else
#endif
        {
            rmkVal.dscp.val = dscp;
            DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarking_get(unit, DOT_1P_RMK_SRC_DSCP, rmkVal, &dot1p_pri),ret);
        }

       diag_util_mprintf("DSCP %2d : %u\n", dscp, dot1p_pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_INNER_TAG_SYSTEM_SOURCE
/*
 * qos get remarking inner-tag system source
 */
cparser_result_t cparser_cmd_qos_get_remarking_inner_tag_system_source(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_qos_1pRmkSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_1pRemarkSrcSel_get(unit, &type),ret);

    diag_util_mprintf("Remarking Source : ");
    if (type == DOT_1P_RMK_SRC_INT_PRI)
        diag_util_mprintf("%s\n", "Internal priority");
    else if (type == DOT_1P_RMK_SRC_USER_PRI)
        diag_util_mprintf("%s\n", "Original inner-tag priority");
    else if (type == DOT_1P_RMK_SRC_OUTER_USER_PRI)
        diag_util_mprintf("%s\n", "Original outer-tag priority");
    else if (type == DOT_1P_RMK_SRC_DSCP)
        diag_util_mprintf("%s\n", "DSCP");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_PRIORITY
/*
 * qos get default-priority inner-tag system priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_system_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Default Inner Priority Configuration of System \n");
    DIAG_UTIL_ERR_CHK(rtk_qos_1pDfltPri_get(unit, &dot1p_pri),ret);
    diag_util_mprintf("Default Priority : %d\n", dot1p_pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_INNER_PRIORITY
/*
 * qos get remarking dscp system inner-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_inner_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    uint32      dscp;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("System DSCP Remarking Configuration \n");

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Inner Priority\t  DSCP Value\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemark_get(unit, int_pri, &dscp),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarking_get(unit, DSCP_RMK_SRC_USER_PRI, rmkVal, &dscp),ret);
            }
            diag_util_mprintf("Priority %u : %2d\n", int_pri, dscp);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Not Support\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_INTERNAL_PRIORITY
/*
 * qos get remarking dscp system internal-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_internal_priority(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      dscp;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("System DSCP Remarking Configuration \n");

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Internal Priority\t  DSCP Value\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemark_get(unit, int_pri, &dscp),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarking_get(unit, DSCP_RMK_SRC_INT_PRI, rmkVal, &dscp),ret);
            }
            diag_util_mprintf("Priority %u :\t\t %2d\n", int_pri, dscp);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_OUTER_PRIORITY
/*
 * qos get remarking dscp system outer-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_outer_priority(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      dscp;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("System DSCP Remarking Configuration \n");

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Outer Priority\t  DSCP Value\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemark_get(unit, int_pri, &dscp),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarking_get(unit, DSCP_RMK_SRC_OUTER_USER_PRI, rmkVal, &dscp),ret);
            }
            diag_util_mprintf("Priority %u : %2d\n", int_pri, dscp);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Not Support\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_DSCP
/*
 * qos get remarking dscp system dscp
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_dscp(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    uint32      orgDscp, dscp;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System DSCP Remarking Configuration \n");

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Original DSCP\t  DSCP Value\n");
        for (orgDscp = 0; orgDscp <= RTK_VALUE_OF_DSCP_MAX; orgDscp++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscp2DscpRemark_get(unit, orgDscp, &dscp),ret);
            }
            else
#endif
            {
                rmkVal.dscp.val = orgDscp;
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarking_get(unit, DSCP_RMK_SRC_DSCP, rmkVal, &dscp),ret);
            }
            diag_util_mprintf("DSCP %2d :\t\t %2d\n", orgDscp, dscp);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_DROP_PRECEDENCE_AND_INTERNAL_PRIORITY
/*
 * qos get remarking dscp system drop-precedence-and-internal-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_drop_precedence_and_internal_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    uint32      int_pri, dp, dscp;
    rtk_qos_dscpRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System DSCP Remarking Configuration \n");

    diag_util_mprintf("Drop precedenct\t  Internal Priority\t  DSCP Value\n");
    for (dp = 0; dp <= RTK_DROP_PRECEDENCE_MAX; dp++)
    {
        for (int_pri = 0; int_pri <= RTK_DOT1P_PRIORITY_MAX; int_pri++)
        {
            rmkVal.intPriDp.dp = dp;
            rmkVal.intPriDp.pri = int_pri;
            DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarking_get(unit, DSCP_RMK_SRC_DP_INT_PRI, rmkVal, &dscp),ret);
            diag_util_mprintf("DP %2d\tINT PRI %2d\t:\t\t %2d\n", dp, int_pri, dscp);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_SYSTEM_SOURCE
/*
 * qos get remarking dscp system source
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_system_source(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_dscpRmkSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_dscpRemarkSrcSel_get(unit, &type),ret);

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if (DSCP_RMK_SRC_INT_PRI == type)
            diag_util_mprintf("Remarking Source : Internal priority\n");
        else if (DSCP_RMK_SRC_DSCP == type)
            diag_util_mprintf("Remarking Source : DSCP value\n");
    }

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (DSCP_RMK_SRC_INT_PRI == type)
            diag_util_mprintf("Remarking Source : Internal priority\n");
        else if (DSCP_RMK_SRC_DSCP == type)
            diag_util_mprintf("Remarking Source : DSCP value\n");
        else if (DSCP_RMK_SRC_DP == type)
            diag_util_mprintf("Remarking Source : Drop precedence\n");
        else if (DSCP_RMK_SRC_USER_PRI == type)
            diag_util_mprintf("Remarking Source : Original inner-tag priority\n");
        else if (DSCP_RMK_SRC_OUTER_USER_PRI == type)
            diag_util_mprintf("Remarking Source : Original outer-tag priority\n");

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            if (DSCP_RMK_SRC_DP_INT_PRI == type)
                diag_util_mprintf("Remarking Source : Drop precedence and Internal priority\n");
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DEI_SYSTEM_SOURCE
/*
 * qos get remarking dei system source
 */
cparser_result_t cparser_cmd_qos_get_remarking_dei_system_source(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_deiRmkSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_deiRemarkSrcSel_get(unit, &type),ret);

    if (DEI_RMK_SRC_INT_PRI == type)
        diag_util_mprintf("Remarking Source : Internal priority\n");
    else if (DEI_RMK_SRC_DP == type)
        diag_util_mprintf("Remarking Source : Drop precedence\n");

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE
/*
 * qos get default-priority outer-tag port ( <PORT_LIST:ports> | all ) source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_port_ports_all_source(cparser_context_t *context,
    char **ports_ptr)
{
    int32        ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Outer Priority Configuration of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port, &type),ret);

            if (type == OUTER_1P_DFLT_SRC_INT_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
            else if (type == OUTER_1P_DFLT_SRC_USER_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy inner priority");
            else if (type == OUTER_1P_DFLT_SRC_PB_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy port-based priority");
            else if (type == OUTER_1P_DFLT_SRC_PB_OUTER_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy port-based outer priority");
            else
                diag_util_mprintf("Port %2d : %s\n", port, "Copy default outer priority");
        }
#endif

#if defined(CONFIG_SDK_RTL8390)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port, &type),ret);

            if (type == OUTER_1P_DFLT_SRC_INT_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
            else if (type == OUTER_1P_DFLT_SRC_USER_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy inner priority");
            else if (type == OUTER_1P_DFLT_SRC_PB_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy port-based priority");
            else
                diag_util_mprintf("Port %2d : %s\n", port, "Copy default outer priority");
        }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port, &type),ret);

            if (type == OUTER_1P_DFLT_SRC_INT_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
            else if (type == OUTER_1P_DFLT_SRC_USER_PRI)
                diag_util_mprintf("Port %2d : %s\n", port, "Copy inner priority");
            else
                diag_util_mprintf("Port %2d : %s\n", port, "Copy default outer priority");
        }
#endif
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_PRIORITY
/*
 * qos get default-priority outer-tag port ( <PORT_LIST:ports> | all ) priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_port_ports_all_priority(cparser_context_t *context,
    char **ports_ptr)
{
    int32        ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_pri_t                   pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Outer Priority Value of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPri_get(unit, port, &pri),ret);
         diag_util_mprintf("Port %2d : %d\n", port, pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE
/*
 * qos get default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_dev_devID_port_ports_all_source(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t    src;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Outer Priority Configuration of Ports in Source Device %2d\n", *devID_ptr);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPriSrcSelExt_get(unit, *devID_ptr, port, &src),ret);

        if (src == OUTER_1P_DFLT_SRC_INT_PRI)
            diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
        else if (src == OUTER_1P_DFLT_SRC_USER_PRI)
            diag_util_mprintf("Port %2d : %s\n", port, "Copy inner priority");
        else
            diag_util_mprintf("Port %2d : %s\n", port, "Copy default outer priority");

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_PRIORITY
/*
 * qos get default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_dev_devID_port_ports_all_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_pri_t                   pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Outer Priority Value of Ports in Source Device %2d\n", *devID_ptr);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pDfltPriExt_get(unit, *devID_ptr, port, &pri),ret);
        diag_util_mprintf("Port %2d : %d\n", port, pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_SYSTEM_SOURCE
/*
 * qos get remarking outer-tag system source
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_system_source(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_qos_outer1pRmkSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemarkSrcSel_get(unit, &type),ret);

     diag_util_mprintf("Remarking Source : ");
    if (type == OUTER_1P_RMK_SRC_INT_PRI)
        diag_util_mprintf("%s\n", "Internal priority");
    else if (type == OUTER_1P_RMK_SRC_INNER_USER_PRI)
        diag_util_mprintf("%s\n", "Original inner-tag priority");
    else if (type == OUTER_1P_RMK_SRC_USER_PRI)
        diag_util_mprintf("%s\n", "Original outer-tag priority");
    else if (type == OUTER_1P_RMK_SRC_DSCP)
        diag_util_mprintf("%s\n", "DSCP");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_SYSTEM_INNER_PRIORITY
/*
 * qos get remarking outer-tag system inner-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_system_inner_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Inner Priority\t  Outer Priority\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
            diag_util_printf("Priority %2u : ", int_pri);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemark_get(unit, int_pri, &dot1p_pri),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemarking_get(unit, OUTER_1P_RMK_SRC_INNER_USER_PRI, rmkVal, &dot1p_pri),ret);
            }
            diag_util_mprintf("%u\n", dot1p_pri);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Not Support\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_SYSTEM_INTERNAL_PRIORITY
/*
 * qos get remarking outer-tag system internal-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_system_internal_priority(cparser_context_t *context)
{
    int32        ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Internal Priority\t  Outer Priority\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
            diag_util_printf("Priority %2u : \t\t", int_pri);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemark_get(unit, int_pri, &dot1p_pri),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemarking_get(unit, OUTER_1P_RMK_SRC_INT_PRI, rmkVal, &dot1p_pri),ret);
            }
            diag_util_mprintf("%u\n", dot1p_pri);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_SYSTEM_OUTER_PRIORITY
/*
 * qos get remarking outer-tag system outer-priority
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_system_outer_priority(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Outer Priority\t  Outer Priority\n");
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
        {
            diag_util_printf("Priority %2u : \t\t", int_pri);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemark_get(unit, int_pri, &dot1p_pri),ret);
            }
            else
#endif
            {
                rmkVal.pri.val = int_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemarking_get(unit, OUTER_1P_RMK_SRC_USER_PRI, rmkVal, &dot1p_pri),ret);
            }
            diag_util_mprintf("%u\n", dot1p_pri);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_SYSTEM_DSCP
/*
 * qos get remarking outer-tag system dscp
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_system_dscp(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0, dscp;
    rtk_pri_t   dot1p_pri;
    rtk_qos_outer1pRmkVal_t rmkVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("DSCP\t\t  Outer Priority\n");
        for (dscp = 0; dscp <= RTK_VALUE_OF_DSCP_MAX; dscp++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscp2Outer1pRemark_get(unit, dscp, &dot1p_pri),ret);
            }
            else
#endif
            {
                rmkVal.dscp.val = dscp;
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pRemarking_get(unit, OUTER_1P_RMK_SRC_DSCP, rmkVal, &dot1p_pri),ret);
            }
            diag_util_mprintf("DSCP %2d : %u\n", dscp, dot1p_pri);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Not Support\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DSCP_INNER_TAG_OUTER_TAG_PORT_PORTS_ALL_STATE
/*
 * qos get remarking ( dscp | inner-tag | outer-tag ) port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_qos_get_remarking_dscp_inner_tag_outer_tag_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(3,0))
    {
        diag_util_mprintf("Inner-tag Priority Remarking Ability of Ports \n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_port1pRemarkEnable_get(unit, port, &enable),ret);
            if (ENABLED == enable)
            {
                diag_util_printf("Port %2u : Enable\n", port);
            }
            else
            {
                diag_util_printf("Port %2u : Disable\n", port);
            }
        }
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        diag_util_mprintf("DSCP Remarking Ability of Ports \n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
           DIAG_UTIL_ERR_CHK(rtk_qos_portDscpRemarkEnable_get(unit, port, &enable),ret);
            if (ENABLED == enable)
            {
                diag_util_printf("Port %2u : Enable\n", port);
            }
            else
            {
                diag_util_printf("Port %2u : Disable\n", port);
            }
        }
    }
    else if ('o' == TOKEN_CHAR(3,0))
    {
        diag_util_mprintf("Outer-tag Priority Remarking Ability of Ports \n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portOut1pRemarkEnable_get(unit, port, &enable),ret);
            if (ENABLED == enable)
            {
                diag_util_printf("Port %2u : Enable\n", port);
            }
            else
            {
                diag_util_printf("Port %2u : Disable\n", port);
            }
        }
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMARKING_DEI_PORT_PORTS_ALL_STATE
/*
 * qos get remarking dei port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_qos_get_remarking_dei_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("DEI Remarking Ability of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDeiRemarkEnable_get(unit, port, &enable),ret);
        if (ENABLED == enable)
        {
            diag_util_printf("Port %2u : Enable\n", port);
        }
        else
        {
            diag_util_printf("Port %2u : Disable\n", port);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_SYSTEM_PRIORITY
/*
 * qos get default-priority outer-tag system priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_system_priority(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_pri_t   out_dot1p_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Default Outer Priority Configuration of System \n");
    DIAG_UTIL_ERR_CHK(rtk_qos_outer1pDfltPri_get(unit, &out_dot1p_pri),ret);
    diag_util_mprintf("Default Priority : %d\n", out_dot1p_pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_SYSTEM_PRIORITY_PRIORITY
/*
 * qos set default-priority outer-tag system priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_system_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   out_dot1p_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    out_dot1p_pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_qos_outer1pDfltPri_set(unit, out_dot1p_pri),ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_OUTER_TAG_SYSTEM_CONFIG_SOURCE
/*
 * qos get default-priority outer-tag system config-source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_outer_tag_system_config_source(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_outer1pDfltCfgSrc_t dflt_sel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_outer1pDfltPriCfgSrcSel_get(unit, &dflt_sel),ret);

    diag_util_mprintf("Default Outer Priority Configured Direction of System : %s\n",
                (dflt_sel == OUTER_1P_DFLT_CFG_SRC_INGRESS) ? "Ingress" : "Egress");


    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_SYSTEM_CONFIG_SOURCE_EGRESS_INGRESS
/*
 * qos set default-priority outer-tag system config-source ( egress | ingress )
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_system_config_source_egress_ingress(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_outer1pDfltCfgSrc_t dflt_sel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(6,0))
    {
        dflt_sel = OUTER_1P_DFLT_CFG_SRC_EGRESS;
    }
    else if ('i' == TOKEN_CHAR(6,0))
    {
        dflt_sel = OUTER_1P_DFLT_CFG_SRC_INGRESS;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_outer1pDfltPriCfgSrcSel_set(unit, dflt_sel),ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_DEV_DEVID_PORT_PORTS_ALL_PRIORITY
/*
 * qos get default-priority inner-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_dev_devID_port_ports_all_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_pri_t                 pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Inner Priority Value of Ports in Source Device %2d\n", *devID_ptr);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_port1pDfltPriExt_get(unit, *devID_ptr, port, &pri),ret);
        diag_util_mprintf("Port %2d : %d\n", port, pri);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_DEV_DEVID_PORT_PORTS_ALL_PRIORITY_PRIORITY
/*
 * qos set default-priority inner-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_dev_devID_port_ports_all_priority_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPriExt_set(unit, *devID_ptr, port, *priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE
/*
 * qos get default-priority inner-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_dev_devID_port_ports_all_source(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t    src;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Inner Priority Configuration of Ports in Source Device %2d\n", *devID_ptr);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_port1pDfltPriSrcSelExt_get(unit, *devID_ptr, port, &src),ret);

        if (src == INNER_1P_DFLT_SRC_INT_PRI)
            diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
        else
            diag_util_mprintf("Port %2d : %s\n", port, "Copy default inner priority");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE_COPY_DEFAULT_INNER_PRIORITY
/*
 * qos set default-priority inner-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source copy-default-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_dev_devID_port_ports_all_source_copy_default_inner_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[9].buf, "copy-default-inner-priority"))
    {
        type = INNER_1P_DFLT_SRC_DFLT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPriSrcSelExt_set(unit, *devID_ptr, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE_COPY_INTERNAL_PRIORITY
/*
 * qos set default-priority inner-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source copy-internal-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_dev_devID_port_ports_all_source_copy_internal_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[9].buf, "copy-internal-priority"))
    {
        type = INNER_1P_DFLT_SRC_INT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPriSrcSelExt_set(unit, *devID_ptr, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_PORT_PORTS_ALL_SOURCE
/*
 * qos get default-priority inner-tag port ( <PORT_LIST:ports> | all ) source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_port_ports_all_source(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t    src;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Inner Priority Configuration of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_port1pDfltPriSrcSel_get(unit, port, &src),ret);

        if (src == INNER_1P_DFLT_SRC_INT_PRI)
            diag_util_mprintf("Port %2d : %s\n", port, "Copy internal priority");
        else
            diag_util_mprintf("Port %2d : %s\n", port, "Copy default inner priority");

    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_PORT_PORTS_ALL_PRIORITY
/*
 * qos get default-priority inner-tag port ( <PORT_LIST:ports> | all ) priority
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_port_ports_all_priority(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_pri_t                 pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Default Inner Priority Value of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_port1pDfltPri_get(unit, port, &pri),ret);
        diag_util_mprintf("Port %2d : %d\n", port, pri);

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_CONFIG_SOURCE
/*
 * qos get default-priority inner-tag system config-source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_system_config_source(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_1pDfltCfgSrc_t dflt_sel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

   DIAG_UTIL_ERR_CHK(rtk_qos_1pDfltPriCfgSrcSel_get(unit, &dflt_sel),ret);

    diag_util_mprintf("Default Inner Priority Configured Direction of System : %s\n",
        (dflt_sel == INNER_1P_DFLT_CFG_SRC_INGRESS) ? "Ingress" : "Egress");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_PORT_PORTS_ALL_PRIORITY_PRIORITY
/*
 * qos set default-priority inner-tag port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_port_ports_all_priority_priority(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPri_set(unit, port, *priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_PORT_PORTS_ALL_SOURCE_COPY_DEFAULT_INNER_PRIORITY
/*
 * qos set default-priority inner-tag port ( <PORT_LIST:ports> | all ) source copy-default-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_port_ports_all_source_copy_default_inner_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[7].buf, "copy-default-inner-priority"))
    {
        type = INNER_1P_DFLT_SRC_DFLT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPriSrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_PORT_PORTS_ALL_SOURCE_COPY_INTERNAL_PRIORITY
/*
 * qos set default-priority inner-tag port ( <PORT_LIST:ports> | all ) source copy-internal-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_port_ports_all_source_copy_internal_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[7].buf, "copy-internal-priority"))
    {
        type = INNER_1P_DFLT_SRC_INT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_port1pDfltPriSrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_CONFIG_SOURCE_EGRESS_INGRESS
/*
 * qos set default-priority inner-tag system config-source ( egress | ingress )
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_config_source_egress_ingress(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pDfltCfgSrc_t dflt_sel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(6,0))
    {
        dflt_sel = INNER_1P_DFLT_CFG_SRC_EGRESS;
    }
    else if ('i' == TOKEN_CHAR(6,0))
    {
        dflt_sel = INNER_1P_DFLT_CFG_SRC_INGRESS;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_qos_1pDfltPriCfgSrcSel_set(unit, dflt_sel)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_GET_REMARKING_OUTER_TAG_PORT_PORTS_ALL_SOURCE
/*
 * qos get remarking outer-tag port ( <PORT_LIST:ports> | all ) source
 */
cparser_result_t cparser_cmd_qos_get_remarking_outer_tag_port_ports_all_source(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_outer1pRmkSrc_t type;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
      DIAG_UTIL_ERR_CHK(rtk_qos_portOuter1pRemarkSrcSel_get(unit, port, &type),ret);
        diag_util_mprintf("Port %2d outer-tag Remarking Source : %s\n", port, (type == OUTER_1P_RMK_SRC_INT_PRI) ? "Internal priority" : "Original outer-tag priority");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_PORT_PORTS_ALL_SOURCE_INTERNAL_PRIORITY
/*
 * qos set remarking outer-tag port ( <PORT_LIST:ports> | all ) source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_port_ports_all_source_internal_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pRemarkSrcSel_set(unit, port, PRI_SRC_INT_PRI)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMARKING_OUTER_TAG_PORT_PORTS_ALL_SOURCE_OUTER_PRIORITY
/*
 * qos set remarking outer-tag port ( <PORT_LIST:ports> | all ) source outer-priority
 */
cparser_result_t cparser_cmd_qos_set_remarking_outer_tag_port_ports_all_source_outer_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pRemarkSrcSel_set(unit, port, PRI_SRC_OUTER_USER_PRI)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_SOURCE
/*
 * qos get default-priority inner-tag system source
 */
cparser_result_t cparser_cmd_qos_get_default_priority_inner_tag_system_source(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Default Inner Priority Source Configuration\n");

   DIAG_UTIL_ERR_CHK(rtk_qos_1pDfltPriSrcSel_get(unit, &type),ret);

    if (type == INNER_1P_DFLT_SRC_INT_PRI)
        diag_util_mprintf("%s\n", "Copy internal priority");
    else if (type == INNER_1P_DFLT_SRC_DFLT_PRI)
        diag_util_mprintf("%s\n", "Copy default inner priority");
    else if (type == INNER_1P_DFLT_SRC_PB_INNER_PRI)
        diag_util_mprintf("%s\n", "Copy port-based inner priority");
    else if (type == INNER_1P_DFLT_SRC_PB_PRI)
        diag_util_mprintf("%s\n", "Copy port-based priority");
    else
        diag_util_mprintf("Not Support\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_SOURCE_PORT_BASED_INNER_PRIORITY
/*
 * qos set default-priority inner-tag system source port-based-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_source_port_based_inner_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (0 == strcmp(context->parser->tokens[6].buf, "port-based-inner-priority"))
    {
        type = INNER_1P_DFLT_SRC_PB_INNER_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    if ((ret = rtk_qos_1pDfltPriSrcSel_set(unit, type)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_SOURCE_PORT_BASED_PRIORITY
/*
 * qos set default-priority inner-tag system source port-based-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_source_port_based_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_1pDfltPriSrcSel_set(unit, INNER_1P_DFLT_SRC_PB_PRI)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_SOURCE_DEFAULT_INNER_PRIORITY
/*
 * qos set default-priority inner-tag system source default-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_source_default_inner_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (0 == strcmp(context->parser->tokens[6].buf, "default-inner-priority"))
    {
        type = OUTER_1P_DFLT_SRC_DFLT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    if ((ret = rtk_qos_1pDfltPriSrcSel_set(unit, type)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_INNER_TAG_SYSTEM_SOURCE_INTERNAL_PRIORITY
/*
 * qos set default-priority inner-tag system source internal-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_inner_tag_system_source_internal_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_1pDfltPriSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (0 == strcmp(context->parser->tokens[6].buf, "internal-priority"))
    {
        type = OUTER_1P_DFLT_SRC_INT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    if ((ret = rtk_qos_1pDfltPriSrcSel_set(unit, type)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE_COPY_PORT_BASED_OUTER_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) source copy-port-based-outer-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_source_copy_port_based_outer_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type = OUTER_1P_DFLT_SRC_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[7].buf, "copy-port-based-outer-priority"))
    {
#if defined(CONFIG_SDK_RTL8390)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            type = OUTER_1P_DFLT_SRC_PB_PRI;
        }
#endif

#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            type = OUTER_1P_DFLT_SRC_PB_OUTER_PRI;
        }
#endif
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE_COPY_PORT_BASED_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) source copy-port-based-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_source_copy_port_based_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, OUTER_1P_DFLT_SRC_PB_PRI)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_SOURCE_COPY_DEFAULT_OUTER_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) source copy-default-outer-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_source_copy_default_outer_priority(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[7].buf, "copy-default-outer-priority"))
    {
        type = OUTER_1P_DFLT_SRC_DFLT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_PORT_PORTS_ALL_PRIORITY_PRIORITY
/*
 * qos set default-priority outer-tag port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_port_ports_all_priority_priority(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPri_set(unit, port, *priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE_COPY_INNER_PRIORITY
/*
 * qos set default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source copy-inner-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_dev_devID_port_ports_all_source_copy_inner_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[9].buf, "copy-inner-priority"))
    {
        type = OUTER_1P_DFLT_SRC_USER_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSelExt_set(unit, *devID_ptr, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE_COPY_INTERNAL_PRIORITY
/*
 * qos set default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source copy-internal-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_dev_devID_port_ports_all_source_copy_internal_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[9].buf, "copy-internal-priority"))
    {
        type = OUTER_1P_DFLT_SRC_INT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSelExt_set(unit, *devID_ptr, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_SOURCE_COPY_DEFAULT_OUTER_PRIORITY
/*
 * qos set default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) source copy-default-outer-priority
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_dev_devID_port_ports_all_source_copy_default_outer_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_outer1pDfltSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[9].buf, "copy-default-outer-priority"))
    {
        type = OUTER_1P_DFLT_SRC_DFLT_PRI;
    }
    else
    {
        type = CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriSrcSelExt_set(unit, *devID_ptr, port, type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_DEFAULT_PRIORITY_OUTER_TAG_DEV_DEVID_PORT_PORTS_ALL_PRIORITY_PRIORITY
/*
 * qos set default-priority outer-tag dev <UINT:devID> port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_set_default_priority_outer_tag_dev_devID_port_ports_all_priority_priority(cparser_context_t *context,
    uint32_t *devID_ptr,
    char **ports_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portOuter1pDfltPriExt_set(unit, *devID_ptr, port, *priority_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_SCHEDULING_ALGORITHM_PORT_PORTS_ALL_WRR_WFQ
/*
 * qos set scheduling algorithm port ( <PORT_LIST:ports> | all ) ( wrr | wfq )
 */
cparser_result_t cparser_cmd_qos_set_scheduling_algorithm_port_ports_all_wrr_wfq(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_scheduling_type_t   algo;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('r' == TOKEN_CHAR(6,1))
        {
            algo = WRR;
        }
        else if ('f' == TOKEN_CHAR(6,1))
        {
            algo = WFQ;
        }
        else
        {
            return CPARSER_NOT_OK;
        }

        if ((ret = rtk_qos_schedulingAlgorithm_set(unit, port, algo)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_SCHEDULING_QUEUE_WEIGHT_PORT_PORTS_ALL_QUEUE_ID_QID_WEIGHT_WEIGHT
/*
 * qos set scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> weight <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_scheduling_queue_weight_port_ports_all_queue_id_qid_weight_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *qid_ptr,
    uint32_t *weight_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port, cpuPort;
    diag_portlist_t  portlist;
    rtk_qos_queue_weights_t queue_weight;
    rtk_qid_t qid_max = 0;
    rtk_qid_t nor_qid_max;
    rtk_qid_t stk_qid_max;
    rtk_qid_t cpu_qid_max;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID)
        || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        DIAG_OM_GET_CHIP_CAPACITY(unit, nor_qid_max, max_num_of_queue);
        qid_max = nor_qid_max;
        if(*qid_ptr >= qid_max)
        {
            diag_util_mprintf("Queue id must < %d\n", qid_max);
            DIAG_ERR_PRINT(RT_ERR_QUEUE_ID);
            return CPARSER_NOT_OK;
        }
    }

    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_OM_GET_CHIP_CAPACITY(unit, nor_qid_max, max_num_of_queue);
        DIAG_OM_GET_CHIP_CAPACITY(unit, stk_qid_max, max_num_of_stackQueue);
        DIAG_OM_GET_CHIP_CAPACITY(unit, cpu_qid_max, max_num_of_cpuQueue);
        DIAG_UTIL_ERR_CHK(rtk_port_cpuPortId_get(unit, &cpuPort), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {

            if ((RTK_PORTMASK_IS_PORT_SET(devInfo.stack.portmask, port)) &&
                (RTK_PORTMASK_IS_PORT_SET(devInfo.uplink.portmask, port)))
                qid_max = stk_qid_max;
            else if (port == cpuPort)
                qid_max = cpu_qid_max;
            else
                qid_max = nor_qid_max;

            if(*qid_ptr >= qid_max)
            {
                diag_util_mprintf("The number of queue of port %d is only %d\n", port, qid_max);
                DIAG_ERR_PRINT(RT_ERR_QUEUE_ID);
                return CPARSER_NOT_OK;
            }
        }
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_schedulingQueue_get(unit, port, &queue_weight)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        queue_weight.weights[*qid_ptr] = *weight_ptr;

        if ((ret = rtk_qos_schedulingQueue_set(unit, port, &queue_weight)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_FORWARD_TO_CPU_INTERNAL_PRIORITY_PRIORITY
/*
 * qos get remapping forward-to-cpu internal-priority <UINT:priority>
 */
cparser_result_t cparser_cmd_qos_get_remapping_forward_to_cpu_internal_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   int_pri = *priority_ptr;
    rtk_pri_t   new_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_pkt2CpuPriRemap_get(unit, int_pri, &new_pri), ret);
    diag_util_mprintf("Packet to CPU Priority Remapping Table\n");
    diag_util_mprintf("\tInternal Priority %u => %u\n", int_pri, new_pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_AVB_PORT_PORTS_ALL_SR_CLASS_A_SR_CLASS_B_STATE
/*
 * qos get avb port ( <PORT_LIST:ports> | all ) ( sr-class-a | sr-class-b ) state
 */
cparser_result_t cparser_cmd_qos_get_avb_port_ports_all_sr_class_a_sr_class_b_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_avbSrClass_t    srClass;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    srClass = ('a' == TOKEN_CHAR(5, 9))? AVB_SR_CLASS_A : AVB_SR_CLASS_B;

    diag_util_mprintf("State of AVB SR-Class-%c of Ports \n", (srClass == AVB_SR_CLASS_A)? 'A' : 'B');
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portAvbStreamReservationClassEnable_get(unit, port, srClass, &enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\tPort %2d State : %s\n", port, (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_AVB_SR_CLASS_A_SR_CLASS_B_PRIORITY
/*
 * qos get avb ( sr-class-a | sr-class-b ) priority
 */
cparser_result_t cparser_cmd_qos_get_avb_sr_class_a_sr_class_b_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tPriority of AVB SR-Class-%c : %u\n", \
        ('a' == TOKEN_CHAR(3, 9))? 'A' : 'B', \
        ('a' == TOKEN_CHAR(3, 9))? srClassConf.class_a_priority : srClassConf.class_b_priority);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_AVB_SR_CLASS_A_SR_CLASS_B_QUEUE_ID
/*
 * qos get avb ( sr-class-a | sr-class-b ) queue-id
 */
cparser_result_t cparser_cmd_qos_get_avb_sr_class_a_sr_class_b_queue_id(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tQueue-ID of AVB SR-Class-%c : %u\n", \
        ('a' == TOKEN_CHAR(3, 9))? 'A' : 'B', \
        ('a' == TOKEN_CHAR(3, 9))? srClassConf.class_a_queue_id : srClassConf.class_b_queue_id);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_AVB_NON_SR_CLASS_A_NON_SR_CLASS_B_REDIRECT_QUEUE_ID
/*
 * qos get avb ( non-sr-class-a | non-sr-class-b ) redirect-queue-id
 */
cparser_result_t cparser_cmd_qos_get_avb_non_sr_class_a_non_sr_class_b_redirect_queue_id(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tRedirect Queue-ID of AVB Non-SR-Class-%c : %u\n", \
        ('a' == TOKEN_CHAR(3, 13))? 'A' : 'B', \
        ('a' == TOKEN_CHAR(3, 13))? \
            srClassConf.class_non_a_redirect_queue_id : \
            srClassConf.class_non_b_redirect_queue_id);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_AVB_NON_SR_CLASS_A_NON_SR_CLASS_B_REMARK_PRIORITY
/*
 * qos get avb ( non-sr-class-a | non-sr-class-b ) remark-priority
 */
cparser_result_t cparser_cmd_qos_get_avb_non_sr_class_a_non_sr_class_b_remark_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_avbSrConf_t srClassConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_avbStreamReservationConfig_get(unit, &srClassConf)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tRemarking Priority of AVB Non-SR-Class-%c : %u\n", \
        ('a' == TOKEN_CHAR(3, 13))? 'A' : 'B', \
        ('a' == TOKEN_CHAR(3, 13))? \
            srClassConf.class_non_a_remark_priority : \
            srClassConf.class_non_b_remark_priority);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_DROP_PRECEDENCE_SOURCE
/*
 * qos get remapping drop-precedence source
 */
cparser_result_t cparser_cmd_qos_get_remapping_drop_precedence_source(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dpSrc_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_dpSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (DP_SRC_DEI_BASED == type)
    {
        diag_util_mprintf("Drop Precedence Source : DEI-based\n");
    }
    else if (DP_SRC_DSCP_BASED == type)
    {
        diag_util_mprintf("Drop Precedence Source : DSCP-based\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_PRIORITY_SELECTOR_GROUP_ID_INDEX
/*
 * qos get priority-selector group-id { <UINT:index> }
 */
cparser_result_t cparser_cmd_qos_get_priority_selector_group_id_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      group_id;
    uint32      group_id_max;
    rtk_qos_priSelWeight_t   priSelWeight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (5 == context->parser->cmd_tokens )
    {
        group_id        = *index_ptr;
        group_id_max    = *index_ptr;
    }
    else
    {
        group_id        = 0;
        DIAG_OM_GET_CHIP_CAPACITY(unit, group_id_max, pri_sel_group_index_max);
    }

    for (;group_id <= group_id_max; group_id++)
    {
        diag_util_mprintf("Group %u Priority Selector\n", group_id);
        DIAG_UTIL_ERR_CHK(rtk_qos_priSelGroup_get(unit, group_id, &priSelWeight), ret);

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf("\tPort-based               : %u\n", priSelWeight.weight_of_portBased);
            diag_util_mprintf("\tDSCP-based               : %u\n", priSelWeight.weight_of_dscp);
            diag_util_mprintf("\tVLAN-ACL-based           : %u\n", priSelWeight.weight_of_vlanAcl);
            diag_util_mprintf("\tInner-tag-based          : %u\n", priSelWeight.weight_of_innerTag);
            diag_util_mprintf("\tOuter-tag-based          : %u\n", priSelWeight.weight_of_outerTag);
            diag_util_mprintf("\tPE-tag-based             : %u\n", priSelWeight.weight_of_peTag);
            diag_util_mprintf("\tMAC-VLAN/IP-subnet-based : %u\n", priSelWeight.weight_of_macVlan);
            diag_util_mprintf("\tProtocol-and-port-based  : %u\n", priSelWeight.weight_of_protoVlan);
            diag_util_mprintf("\tRouting-based            : %u\n", priSelWeight.weight_of_routing);
            diag_util_mprintf("\t802.11e-based            : %u\n", priSelWeight.weight_of_80211e);
            diag_util_mprintf("\tTunnel-based             : %u\n", priSelWeight.weight_of_tunnel);
            diag_util_mprintf("\tMPLS-based               : %u\n", priSelWeight.weight_of_mpls);
        }
#endif
#if defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf("\tPort-based               : %u\n", priSelWeight.weight_of_portBased);
            diag_util_mprintf("\tDSCP-based               : %u\n", priSelWeight.weight_of_dscp);
            diag_util_mprintf("\tVLAN-ACL-based        : %u\n", priSelWeight.weight_of_vlanAcl);
            diag_util_mprintf("\tInner-tag-based          : %u\n", priSelWeight.weight_of_innerTag);
            diag_util_mprintf("\tOuter-tag-based          : %u\n", priSelWeight.weight_of_outerTag);
            diag_util_mprintf("\tMAC-VLAN/IP-subnet-based : %u\n", priSelWeight.weight_of_macVlan);
            diag_util_mprintf("\tProtocol-and-port-based  : %u\n", priSelWeight.weight_of_protoVlan);
            diag_util_mprintf("\tRouting-based            : %u\n", priSelWeight.weight_of_routing);
        }
#endif
#if defined(CONFIG_SDK_RTL8390)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("\tPort-based               : %u\n", priSelWeight.weight_of_portBased);
            diag_util_mprintf("\tDSCP-based               : %u\n", priSelWeight.weight_of_dscp);
            diag_util_mprintf("\tIngress-ACL-based        : %u\n", priSelWeight.weight_of_inAcl);
            diag_util_mprintf("\tInner-tag-based          : %u\n", priSelWeight.weight_of_innerTag);
            diag_util_mprintf("\tOuter-tag-based          : %u\n", priSelWeight.weight_of_outerTag);
            diag_util_mprintf("\tMAC-VLAN/IP-subnet-based : %u\n", priSelWeight.weight_of_macVlan);
            diag_util_mprintf("\tProtocol-and-port-based  : %u\n", priSelWeight.weight_of_protoVlan);
        }
#endif
#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("\tDSCP based   \t: %u\n", priSelWeight.weight_of_dscp);
            diag_util_mprintf("\tInner tag    \t: %u\n", priSelWeight.weight_of_innerTag);
            diag_util_mprintf("\tOuter tag    \t: %u\n", priSelWeight.weight_of_outerTag);
            diag_util_mprintf("\tPortBaseInner tag    \t: %u\n", priSelWeight.weight_of_portBasedIpri);
            diag_util_mprintf("\tPortBaseOuter tag    \t: %u\n", priSelWeight.weight_of_portBasedOpri);

        }
#endif

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_PRIORITY_SELECTOR_PORT_PORTS_ALL_GROUP_ID
/*
 * qos get priority-selector port ( <PORT_LIST:ports> | all ) group-id
 */
cparser_result_t cparser_cmd_qos_get_priority_selector_port_ports_all_group_id(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      group_id;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Priority Selector Group of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portPriSelGroup_get(unit, port, &group_id)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\tPort %2d Group : %u\n", port, group_id);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_QUEUE_NUMBER
/*
 * qos get queue number
 */
cparser_result_t cparser_cmd_qos_get_queue_number(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      queue_num;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_queueNum_get(unit, &queue_num)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Qos Queue Number : %u \n", queue_num);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_PRIORITY_TO_QUEUE_QUEUE_NUMBER_QUEUE_NUM
/*
 * qos get priority-to-queue queue-number { <UINT:queue_num> }
 */
cparser_result_t cparser_cmd_qos_get_priority_to_queue_queue_number_queue_num(cparser_context_t *context,
    uint32_t *queue_num_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri;
    rtk_pri_t   pri_max;
    uint32      queue_num;
    uint32      queue_num_max;
    rtk_qos_pri2queue_t   pri2Qid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (5 == context->parser->cmd_tokens )
    {
        queue_num        = *queue_num_ptr;
        queue_num_max    = *queue_num_ptr;
    }
    else
    {
        queue_num        = 1;
        DIAG_OM_GET_CHIP_CAPACITY(unit, queue_num_max, max_num_of_queue);
    }

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    for (;queue_num <= queue_num_max; queue_num++)
    {
        diag_util_mprintf("Queue Number %u Priority Queue Mapping\n", queue_num);
        diag_util_mprintf("Internal Priority\t  Queue ID\n");
        DIAG_UTIL_ERR_CHK(rtk_qos_priMap_get(unit, queue_num, &pri2Qid), ret);

        for (pri = 0; pri <= pri_max; pri++)
        {
            diag_util_mprintf("Priority %u : %u\n", pri, pri2Qid.pri2queue[pri]);
        }

        diag_util_mprintf("\n");
    }


    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_PRIORITY_TO_QUEUE
/*
 * qos get priority-to-queue
 */
cparser_result_t cparser_cmd_qos_get_priority_to_queue(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri;
    rtk_pri_t   pri_max;
    rtk_qid_t   qid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    diag_util_mprintf("Internal Priority\t  Queue ID\n");
    for (pri = 0; pri <= pri_max; pri++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_pri2QidMap_get(unit, pri, &qid), ret);

        diag_util_mprintf("Priority %u : %u\n", pri, qid);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_DEI_SYSTEM
/*
 * qos get remapping dei system
 */
cparser_result_t cparser_cmd_qos_get_remapping_dei_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      dei, dp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System DEI Remapping Configuration \n");

    diag_util_mprintf("DEI Value\t  Drop Precedence\n");
    for (dei = RTK_VALUE_OF_DEI_MIN; dei <= RTK_VALUE_OF_DEI_MAX; dei++)
    {
        diag_util_printf("DEI %u : ", dei);
        DIAG_UTIL_ERR_CHK(rtk_qos_deiDpRemap_get(unit, dei, &dp), ret);

        diag_util_mprintf("%u\n", dp);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_DEI_TAG_SELECTOR_PORT_PORTS_ALL
/*
 * qos get remapping dei tag-selector port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_remapping_dei_tag_selector_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_qos_deiSel_t type;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("DEI Remapping Tag Selector Configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portDEISrcSel_get(unit, port, &type), ret);
        diag_util_mprintf("Port %2d : %s\n", port, (type == DEI_SEL_INNER_TAG) ? "Inner-tag" : "Outer-tag");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_PORT_PORTS_ALL
/*
 * qos get remapping port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_remapping_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32 ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_priSrcRemap_t srcVal;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_pri_t   int_pri;
#endif

#if defined(CONFIG_SDK_RTL8380)
    rtk_pri_t   inner_pri, outer_pri;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Port Based Priority Configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        srcVal.src.port = port;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_portPri_get(unit, port, &int_pri), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_PB_PRI, srcVal, &int_pri), ret);
            }
            diag_util_mprintf("Port %2d : %u\n", port, int_pri);
        }
#endif

#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_printf("\tPriority of inner tag\t: ");
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_portInnerPri_get(unit, port, &inner_pri), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_PB_INNER_PRI, srcVal, &inner_pri), ret);
            }

            diag_util_mprintf("Port %2d : %u\n", port, inner_pri);

            diag_util_printf("\tPriority of outer tag\t: ");
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_portOuterPri_get(unit, port, &outer_pri), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_PB_OUTER_PRI, srcVal, &outer_pri), ret);
            }

            diag_util_mprintf("Port %2d : %u\n", port, outer_pri);
        }
#endif
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_INNER_TAG_SYSTEM
/*
 * qos get remapping inner-tag system
 */
cparser_result_t cparser_cmd_qos_get_remapping_inner_tag_system(cparser_context_t *context)
{
    int32   ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    uint32      dei;
    rtk_qos_priSrcRemap_t srcVal;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_qos_dpSrcRemap_t dpSrcVal;
    uint32      dp;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    for (dei = 0; dei <= RTK_DOT1P_DEI_MAX; dei++)
    {
        diag_util_printf("DEI %u Inner Priority Remapping Configuration\n", dei);

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            diag_util_mprintf("Inner Priority\t  Internal Priority\t  Drop Precedence\n");
        else
            diag_util_mprintf("Inner Priority\t  Internal Priority\n");
        for (dot1p_pri = 0; dot1p_pri <= RTK_DOT1P_PRIORITY_MAX; dot1p_pri++)
        {
            diag_util_printf("Priority %u : ", dot1p_pri);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_1pPriRemap_get(unit, dot1p_pri, &int_pri), ret);
            }
            else
#endif
            {
                srcVal.src.tag.dei = dei;
                srcVal.src.tag.pri = dot1p_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_INNER_USER_PRI, srcVal, &int_pri), ret);
            }
            diag_util_mprintf("\t\t%u\t\t", int_pri);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
            if ((DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID)))
            {
                dpSrcVal.src.tag.dei = dei;
                dpSrcVal.src.tag.pri = dot1p_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_dpRemap_get(unit, DP_SRC_INNER_PRI_BASED, dpSrcVal, &dp), ret);
                diag_util_mprintf("%u", dp);
            }
#endif
            diag_util_mprintf("\n");
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_DSCP_SYSTEM
/*
 * qos get remapping dscp system
 */
cparser_result_t cparser_cmd_qos_get_remapping_dscp_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      dscp, dp;
    rtk_pri_t   int_pri;
    rtk_qos_priSrcRemap_t srcVal;
    rtk_qos_dpSrcRemap_t dpSrcRemap;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System DSCP Priority Remapping Configuration \n");

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        diag_util_mprintf("DSCP priority\t  Internal Priority\t  Drop Precedence\n");
    else
        diag_util_mprintf("DSCP priority\t  Internal Priority\n");
    for (dscp = RTK_VALUE_OF_DSCP_MIN; dscp <= RTK_VALUE_OF_DSCP_MAX; dscp++)
    {
        diag_util_mprintf("DSCP \t%2d: ", dscp);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_dscpPriRemap_get(unit, dscp, &int_pri), ret);
        }
        else
#endif
        {
            srcVal.src.dscp = dscp;
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_DSCP, srcVal, &int_pri), ret);
        }
        diag_util_mprintf("\t\t%u\t\t", int_pri);

        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_dscpDpRemap_get(unit, dscp, &dp), ret);
            }
            else
#endif
            {
                dpSrcRemap.src.dscp = dscp;
                DIAG_UTIL_ERR_CHK(rtk_qos_dpRemap_get(unit, DP_SRC_DSCP_BASED, dpSrcRemap, &dp), ret);
            }
            diag_util_mprintf("%u\n", dp);
        }
        else
            diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_OUTER_TAG_SYSTEM
/*
 * qos get remapping outer-tag system
 */
cparser_result_t cparser_cmd_qos_get_remapping_outer_tag_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    uint32      dei;
    rtk_qos_priSrcRemap_t srcVal;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_qos_dpSrcRemap_t dpSrcVal;
    uint32      dp;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    for (dei = 0; dei <= RTK_DOT1P_DEI_MAX; dei++)
    {
        diag_util_printf("DEI %u Outer Priority Remapping Configuration\n", dei);

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            diag_util_mprintf("Outer Priority\t  Internal Priority\t  Drop Precedence\n");
        else
            diag_util_mprintf("Outer Priority\t  Internal Priority\n");
        for (dot1p_pri = 0; dot1p_pri <= RTK_DOT1P_PRIORITY_MAX; dot1p_pri++)
        {
            diag_util_printf("Priority %u : ", dot1p_pri);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_qos_outer1pPriRemap_get(unit, dot1p_pri, dei, &int_pri), ret);
            }
            else
#endif
            {
                srcVal.src.tag.dei = dei;
                srcVal.src.tag.pri = dot1p_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_OUTER_USER_PRI, srcVal, &int_pri), ret);
            }
            diag_util_mprintf("\t\t%u\t\t", int_pri);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
            if ((DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID)))
            {
                dpSrcVal.src.tag.dei = dei;
                dpSrcVal.src.tag.pri = dot1p_pri;
                DIAG_UTIL_ERR_CHK(rtk_qos_dpRemap_get(unit, DP_SRC_OUTER_PRI_BASED, dpSrcVal, &dp), ret);
                diag_util_mprintf("%u", dp);
            }
#endif
            diag_util_mprintf("\n");
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_PE_TAG_SYSTEM
/*
 * qos get remapping pe-tag system
 */
cparser_result_t cparser_cmd_qos_get_remapping_pe_tag_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    rtk_pri_t   dot1p_pri;
    rtk_pri_t   int_pri;
    uint32      dei;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    for (dei = 0; dei <= RTK_DOT1P_DEI_MAX; dei++)
    {
        diag_util_printf("DEI %u - Port Extension Tag Priority Remapping Configuration\n", dei);

        diag_util_mprintf("PE-tag Priority\t  Internal Priority\n");
        for (dot1p_pri = 0; dot1p_pri <= RTK_DOT1P_PRIORITY_MAX; dot1p_pri++)
        {
            diag_util_printf("Priority %u : ", dot1p_pri);
            srcVal.src.tag.dei = dei;
            srcVal.src.tag.pri = dot1p_pri;
            DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_PE_PRI, srcVal, &int_pri), ret);
            diag_util_mprintf("\t\t%u\t\t", int_pri);

            diag_util_mprintf("\n");
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_80211E_SYSTEM
/*
 * qos get remapping 80211e system
 */
cparser_result_t cparser_cmd_qos_get_remapping_80211e_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      tid;
    rtk_pri_t   int_pri;
    rtk_qos_priSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("802.11e TID Remapping Configuration\n");
    diag_util_mprintf("TID Value\t  Internal Priority\n");
    for (tid = 0; tid <= 15; tid++) /* [FIX ME] */
    {
        diag_util_printf("TID %u : ", tid);
        srcVal.src.tid = tid;
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_80211E, srcVal, &int_pri), ret);
        diag_util_mprintf("\t%u\n", int_pri);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_MPLS_SYSTEM
/*
 * qos get remapping mpls system
 */
cparser_result_t cparser_cmd_qos_get_remapping_mpls_system(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0;
    uint32      tc, dp;
    rtk_pri_t   int_pri;
    rtk_qos_priSrcRemap_t srcVal;
    rtk_qos_dpSrcRemap_t dpSrcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("MPLS TC Remapping Configuration\n");

    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        diag_util_mprintf("TC Value\t  Internal Priority\t  Drop Precedence\n");
    else
        diag_util_mprintf("TC Value\t  Internal Priority\n");
    for (tc = 0; tc <= 7; tc++) /* [FIX ME] */
    {
        diag_util_printf("TC %u : ", tc);
        srcVal.src.tc = tc;
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemap_get(unit, PRI_SRC_MPLS, srcVal, &int_pri), ret);
        diag_util_mprintf("\t\t\t %u \t\t\t", int_pri);

        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            dpSrcVal.src.tc = tc;

            DIAG_UTIL_ERR_CHK(rtk_qos_dpRemap_get(unit, DP_SRC_MPLS_BASED, dpSrcVal, &dp), ret);
            diag_util_mprintf("%u\n", dp);
        }
        else
            diag_util_mprintf("\n");
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_SCHEDULING_ALGORITHM_PORT_PORTS_ALL
/*
 * qos get scheduling algorithm port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_scheduling_algorithm_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32   ret;
    uint32      unit = 0;
    rtk_qos_scheduling_type_t   algorithm;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Scheduling Algorithm of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d : ", port);

        DIAG_UTIL_ERR_CHK(rtk_qos_schedulingAlgorithm_get(unit, port, &algorithm), ret);

        if (WFQ == algorithm)
        {
            diag_util_mprintf("WFQ\n");
        }
        else
        {
            diag_util_mprintf("WRR\n");
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_SCHEDULING_QUEUE_WEIGHT_PORT_PORTS_ALL
/*
 * qos get scheduling queue-weight port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_qos_get_scheduling_queue_weight_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port, cpuPort;
    rtk_qid_t   queue;
    rtk_qid_t   qid_max=0;
    diag_portlist_t  portlist;
    rtk_qos_queue_weights_t     queue_weight;
    rtk_switch_devInfo_t    devInfo;

    rtk_qid_t nor_qid_max;
    rtk_qid_t stk_qid_max;
    rtk_qid_t cpu_qid_max;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&queue_weight, 0, sizeof(rtk_qos_queue_weights_t));
    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_OM_GET_CHIP_CAPACITY(unit, nor_qid_max, max_num_of_queue);
    DIAG_OM_GET_CHIP_CAPACITY(unit, stk_qid_max, max_num_of_stackQueue);
    DIAG_OM_GET_CHIP_CAPACITY(unit, cpu_qid_max, max_num_of_cpuQueue);
    DIAG_UTIL_ERR_CHK(rtk_port_cpuPortId_get(unit, &cpuPort), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID)
            || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            qid_max = nor_qid_max;
        }

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            if ((RTK_PORTMASK_IS_PORT_SET(devInfo.stack.portmask, port)) &&
                (RTK_PORTMASK_IS_PORT_SET(devInfo.uplink.portmask, port)))
                qid_max = stk_qid_max;
            else if (port == cpuPort)
                qid_max = cpu_qid_max;
            else
                qid_max = nor_qid_max;
        }

        diag_util_mprintf("Port %2d Queue Weight\n", port);

        DIAG_UTIL_ERR_CHK(rtk_qos_schedulingQueue_get(unit, port, &queue_weight), ret);

        for (queue = 0; queue < qid_max; queue++)
        {
            diag_util_mprintf("\tQueue %u : %u\n", queue, queue_weight.weights[queue]);
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INGRESS_QUEUE_MAPPING_PORT_PORTS_ALL
/*
*qos get ingress queue mapping port ( <PORT_LIST:ports> | all )
*/
cparser_result_t cparser_cmd_qos_get_ingress_queue_mapping_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32 ret;
    uint32      unit = 0;
    rtk_pri_t   int_pri;
    rtk_pri_t   pri_max = 7;
    rtk_port_t  port = 0;
    diag_portlist_t  portlist;
    rtk_qos_pri2queue_t pri2qid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_OM_GET_CHIP_CAPACITY(unit, pri_max, internal_priority_max);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\tPort %u Ingress Queue Mapping Configuration : \n", port);

        DIAG_UTIL_ERR_CHK(rtk_qos_portPri2IgrQMap_get(unit, port, &pri2qid), ret);
        for (int_pri = 0; int_pri <= pri_max; int_pri++)
            diag_util_printf("\t  Priority %u :  Queue %u\n", int_pri, pri2qid.pri2queue[int_pri]);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_INGRESS_QUEUE_MAPPING_PORT_PORTS_ALL_PRIORITY_QUEUE_ID
/*
*qos set ingress queue mapping port ( <PORT_LIST:ports> | all ) <UINT:priority> <UINT:queue_id>
*/
cparser_result_t cparser_cmd_qos_set_ingress_queue_mapping_port_ports_all_priority_queue_id(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *priority_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0, ret;
    rtk_port_t  port;
    diag_portlist_t  portlist;
    rtk_qos_pri2queue_t pri2qid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if(*priority_ptr >= RTK_MAX_NUM_OF_PRIORITY)
    {
        diag_util_printf("Error: Invalid 1p priority\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPri2IgrQMap_get(unit, port, &pri2qid), ret);
        pri2qid.pri2queue[*priority_ptr] = *queue_id_ptr;

        DIAG_UTIL_ERR_CHK(rtk_qos_portPri2IgrQMap_set(unit, port, &pri2qid), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INGRESS_QUEUE_MAPPING_PORT_PORTS_ALL_STATE
/*
*qos get ingress queue mapping port ( <PORT_LIST:ports> | all ) state
*/
cparser_result_t cparser_cmd_qos_get_ingress_queue_mapping_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPri2IgrQMapEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_SET_INGRESS_QUEUE_MAPPING_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
*qos set ingress queue mapping port ( <PORT_LIST:ports> | all ) state ( enable | disable )
*/
cparser_result_t cparser_cmd_qos_set_ingress_queue_mapping_port_ports_all_state_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(8,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPri2IgrQMapEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_SCHEDULING_INGRESS_QUEUE_WEIGHT_PORT_PORTS_ALL
/*
*qos get scheduling ingress queue-weight port ( <PORT_LIST:ports> | all )
*/
cparser_result_t cparser_cmd_qos_get_scheduling_ingress_queue_weight_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    rtk_qid_t   queue;
    rtk_qid_t   qid_max;
    diag_portlist_t  portlist;
    uint32    queue_weight;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_OM_GET_CHIP_CAPACITY(unit, qid_max, max_num_of_igrQueue);
#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        qid_max = qid_max - 1;
    }
#endif

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d Ingress Queue Weight\n", port);

        for (queue = 0; queue < qid_max; queue++)
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_portIgrQueueWeight_get(unit, port, queue, &queue_weight), ret);
            diag_util_mprintf("\tQueue %u : %u\n", queue, queue_weight);
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_SCHEDULING_INGRESS_QUEUE_WEIGHT_PORT_PORTS_ALL_QUEUE_ID_WEIGHT
/*
 * qos set scheduling ingress queue-weight port ( <PORT_LIST:ports> | all ) <UINT:queue_id> <UINT:weight>
 */
cparser_result_t cparser_cmd_qos_set_scheduling_ingress_queue_weight_port_ports_all_queue_id_weight(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *weight_ptr)
{
    int32       ret;
    uint32      unit = 0;
    rtk_port_t  port;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portIgrQueueWeight_set(unit, port, *queue_id_ptr, *weight_ptr)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INVALID_DSCP_VALUE
/*
*qos get invalid-dscp value
*/
cparser_result_t cparser_cmd_qos_get_invalid_dscp_value(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      dscp =0;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpVal_get(unit, &dscp), ret);
    diag_util_mprintf("Invalid DSCP:\t  %d\n", dscp);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_INVALID_DSCP_VALUE_DSCP
/*
*qos set invalid-dscp value <UINT:dscp>
*/
cparser_result_t cparser_cmd_qos_set_invalid_dscp_value_dscp(cparser_context_t *context,
    uint32_t *dscp_ptr)
{
    uint32      unit = 0;
    uint32      dscp =*dscp_ptr;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpVal_set(unit, dscp), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INVALID_DSCP_MASK
/*
*qos get invalid-dscp mask
*/
cparser_result_t cparser_cmd_qos_get_invalid_dscp_mask(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      dscpMask =0;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpMask_get(unit, &dscpMask), ret);
    diag_util_mprintf("Invalid DSCP mask:\t  %d\n", dscpMask);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_INVALID_DSCP_MASK_DSCP_MASK
/*
*qos set invalid-dscp mask <UINT:dscp_mask>
*/
cparser_result_t cparser_cmd_qos_set_invalid_dscp_mask_dscp_mask(cparser_context_t *context,
    uint32_t *dscp_mask_ptr)
{
    uint32      unit = 0;
    uint32      dscpMask = *dscp_mask_ptr;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpMask_set(unit, dscpMask), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INVALID_DSCP_PORT_PORTS_ALL_STATE
/*
*qos get invalid-dscp port ( <PORT_LIST:ports> | all ) state
*/
cparser_result_t cparser_cmd_qos_get_invalid_dscp_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portInvldDscpEnable_get(unit, port, &enable), ret);
        diag_util_printf("\tPort %u \t%s\n", port, (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_INVALID_DSCP_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
*qos set invalid-dscp port ( <PORT_LIST:ports> | all ) state ( enable | disable )
*/
cparser_result_t cparser_cmd_qos_set_invalid_dscp_port_ports_all_state_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portInvldDscpEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_INVALID_DSCP_STATE
/*
*qos get invalid-dscp state
*/
cparser_result_t cparser_cmd_qos_get_invalid_dscp_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpEnable_get(unit, &enable), ret);
    diag_util_mprintf("Invalid DSCP configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_INVALID_DSCP_STATE_ENABLE_DISABLE
/*
*qos set invalid-dscp state ( enable | disable )
*/
cparser_result_t cparser_cmd_qos_set_invalid_dscp_state_enable_disable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_invldDscpEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_PORT_PRIORITY_REMAP_PORT_PORTS_ALL_TABLE_SELECTOR
/*
 * qos get remapping port-priority-remap port ( <PORT_LIST:ports> | all ) table-selector
 */
cparser_result_t cparser_cmd_qos_get_remapping_port_priority_remap_port_ports_all_table_selector(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_qos_portPriRemapSel_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    diag_util_mprintf("Port priority remapping table \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPortPriRemapSel_get(unit, port, &type), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, (type == P_PRI_REMAP_INNER_PRI_CFI0_TBL) ? "Inner-tag DEI0 priority remapping table" : "Outer-tag DEI0 priority remapping table");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_MPLS_SYSTEM_TC_TC_DROP_PRECEDENCE_DROP_PRECEDENCE
cparser_result_t cparser_cmd_qos_set_remapping_mpls_system_tc_tc_drop_precedence_drop_precedence(cparser_context_t *context,
    uint32_t *tc_ptr,
    uint32_t *drop_precedence_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_dpSrcRemap_t srcVal;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    srcVal.src.tc = *tc_ptr;
    if ((ret = rtk_qos_dpRemap_set(unit, DP_SRC_MPLS_BASED, srcVal, *drop_precedence_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PORT_PRIORITY_REMAP_PORT_PORTS_ALL_TABLE_SELECTOR_INNER_PRIORITY_REMAP_TABLE_OUTER_PRIORITY_REMAP_TABLE
/*
 * qos set remapping port-priority-remap port ( <PORT_LIST:ports> | all ) table-selector ( inner-priority-remap-table | outer-priority-remap-table )
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_priority_remap_port_ports_all_table_selector_inner_priority_remap_table_outer_priority_remap_table(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_qos_portPriRemapSel_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(7,0))
    {
        case 'i':
            type = P_PRI_REMAP_INNER_PRI_CFI0_TBL;
            break;

        case 'o':
            type = P_PRI_REMAP_OUTER_PRI_DEI0_TBL;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPortPriRemapSel_set(unit, port, type), ret);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_QOS_GET_REMAPPING_PORT_PRIORITY_REMAP_SYSTEM_STATE
/*
 * qos get remapping port-priority-remap system state
 */
cparser_result_t cparser_cmd_qos_get_remapping_port_priority_remap_system_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriRemapEnable_get(unit, &enable), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_get(unit, PRI_SRC_PB_PRI, &enable), ret);
    }
    diag_util_mprintf("Port priority remapping configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_MAC_BASED_VLAN_PRIORITY_REMAP_SYSTEM_STATE
/*
 * qos get remapping mac-based-vlan-priority-remap system state
 */
cparser_result_t cparser_cmd_qos_get_remapping_mac_based_vlan_priority_remap_system_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_get(unit, PRI_SRC_MAC_VLAN, &enable), ret);
    diag_util_mprintf("MAC-based-VLAN priority remapping configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_PROTOCOL_BASED_VLAN_PRIORITY_REMAP_SYSTEM_STATE
/*
 * qos get remapping protocol-based-vlan-priority-remap system state
 */
cparser_result_t cparser_cmd_qos_get_remapping_protocol_based_vlan_priority_remap_system_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_get(unit, PRI_SRC_PROTO_VLAN, &enable), ret);
    diag_util_mprintf("Protocol-based-VLAN priority remapping configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PORT_PRIORITY_REMAP_SYSTEM_STATE_DISABLE_ENABLE
/*
 * qos set remapping port-priority-remap system state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_priority_remap_system_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portPriRemapEnable_set(unit, enable), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_set(unit, PRI_SRC_PB_PRI, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_MAC_BASED_VLAN_PRIORITY_REMAP_SYSTEM_STATE_DISABLE_ENABLE
/*
 * qos set remapping mac-based-vlan-priority-remap system state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_remapping_mac_based_vlan_priority_remap_system_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_set(unit, PRI_SRC_MAC_VLAN, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PROTOCOL_BASED_VLAN_PRIORITY_REMAP_SYSTEM_STATE_DISABLE_ENABLE
/*
 * qos set remapping protocol-based-vlan-priority-remap system state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_remapping_protocol_based_vlan_priority_remap_system_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_priRemapEnable_set(unit, PRI_SRC_PROTO_VLAN, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_PORT_PRIORITY_REMAP_SYSTEM_TABLE_SELECTOR
/*
 * qos get remapping port-priority-remap system table-selector
 */
cparser_result_t cparser_cmd_qos_get_remapping_port_priority_remap_system_table_selector(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_qos_portPriRemapSel_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_sysPortPriRemapSel_get(unit, &type), ret);
    if(type == P_PRI_REMAP_INNER_PRI_TBL || type == P_PRI_REMAP_INNER_PRI_CFI0_TBL)
        diag_util_mprintf("Port priority remapping table: %s\n", "Inner-tag DEI0 priority remapping table");
    else if(type == P_PRI_REMAP_OUTER_PRI_DEI0_TBL)
        diag_util_mprintf("Port priority remapping table: %s\n", "Outer-tag DEI0 priority remapping table");
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_PORT_PRIORITY_REMAP_SYSTEM_TABLE_SELECTOR_INNER_PRIORITY_DEI0_REMAP_TABLE_OUTER_PRIORITY_DEI0_REMAP_TABLE
/*
 * qos set remapping port-priority-remap system table-selector ( inner-priority-dei0-remap-table | outer-priority-dei0-remap-table )
 */
cparser_result_t cparser_cmd_qos_set_remapping_port_priority_remap_system_table_selector_inner_priority_dei0_remap_table_outer_priority_dei0_remap_table(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_qos_portPriRemapSel_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(6,0))
    {
        case 'i':
            if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                type = P_PRI_REMAP_INNER_PRI_TBL;
            }
            else
                type = P_PRI_REMAP_INNER_PRI_CFI0_TBL;
            break;

        case 'o':
            type = P_PRI_REMAP_OUTER_PRI_DEI0_TBL;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_sysPortPriRemapSel_set(unit, type), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_INNER_PORTBASE_PRIORITY_STATE
/*
 * qos get remapping inner-portBase-priority state
 * qos set remapping port-inner-priority-remap system state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_get_remapping_inner_portbase_priority_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_portInnerPriRemapEnable_get(unit, &enable), ret);
    diag_util_mprintf("Inner portBase priority remapping configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_INNER_PORTBASE_PRIORITY_STATE_ENABLE_DISABLE
/*
*qos set remapping inner-portbase-priority state ( enable | disable )
*/
cparser_result_t cparser_cmd_qos_set_remapping_inner_portbase_priority_state_enable_disable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(5,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_portInnerPriRemapEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_REMAPPING_OUTER_PORTBASE_PRIORITY_STATE
/*
*qos get remapping outer-portBase-priority state
*/
cparser_result_t cparser_cmd_qos_get_remapping_outer_portbase_priority_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_portOuterPriRemapEnable_get(unit, &enable), ret);
    diag_util_mprintf("Outer portBase priority remapping configuration: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_REMAPPING_OUTER_PORTBASE_PRIORITY_STATE_ENABLE_DISABLE
/*
*qos set remapping outer-portbase-priority state ( enable | disable )
*/
cparser_result_t cparser_cmd_qos_set_remapping_outer_portbase_priority_state_enable_disable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(5,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_qos_portOuterPriRemapEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_SCHEDULING_EGRESS_STRICT_PRIORITY_PORT_PORTS_ALL_QUEUE_ID_QID_STATE
/*
*qos get scheduling egress strict-priority port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> state
*/
cparser_result_t cparser_cmd_qos_get_scheduling_egress_strict_priority_port_ports_all_queue_id_qid_state(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *qid_ptr)
{
    int32           ret;
    uint32          unit = 0;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Egress Strict-priority Ability\n");

        DIAG_UTIL_ERR_CHK(rtk_qos_portQueueStrictEnable_get(unit, port, *qid_ptr, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("Port %u Queue %u : Enable\n", port, *qid_ptr);
        }
        else
        {
            diag_util_mprintf("Port %u Queue %u : Disable\n", port, *qid_ptr);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_SET_SCHEDULING_EGRESS_STRICT_PRIORITY_PORT_PORTS_ALL_QUEUE_ID_QID_STATE_DISABLE_ENABLE
/*
 * qos set scheduling egress strict-priority port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_qos_set_scheduling_egress_strict_priority_port_ports_all_queue_id_qid_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *qid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    rtk_enable_t    enable;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(10,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(10,0))
    {
        enable = DISABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portQueueStrictEnable_set(unit, port, *qid_ptr, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_QOS_GET_CPUQ_TO_STACKQ
/*
 * qos get cpu-queue-to-stack-queue
 */
cparser_result_t
cparser_cmd_qos_get_cpu_queue_to_stack_queue(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_pri_t   qid;
    rtk_pri_t   cpuQidMax;
    rtk_qid_t   stackQid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_OM_GET_CHIP_CAPACITY(unit, cpuQidMax, max_num_of_cpuQueue);

    diag_util_mprintf("CPU Queue ID\t  Stacking Queue ID\n");
    for (qid = 0; qid <cpuQidMax; qid++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_cpuQid2StackQidMap_get(unit, qid, &stackQid), ret);
        diag_util_mprintf("CPU Queue ID %2u : %2u\n", qid, stackQid);
    }

    diag_util_mprintf("\n");


    return CPARSER_OK;
}   /* end of cparser_cmd_qos_get_cpuQ_to_stackQ */
#endif

#ifdef CMD_QOS_SET_CPUQ_TO_STACKQ_CPU_QUEUE_CPU_QID_QUEUE_ID_STK_QID
/*
 * qos set cpu-queue-to-stack-queue cpu-queue <UINT:cpu_qid> queue-id <UINT:stack_qid>
 */
cparser_result_t
cparser_cmd_qos_set_cpu_queue_to_stack_queue_cpu_queue_id_cpu_qid_queue_id_stack_qid(
    cparser_context_t *context,
    uint32_t *cpu_Qid_ptr,
    uint32_t *stk_qid_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_cpuQid2StackQidMap_set(unit, *cpu_Qid_ptr, *stk_qid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_qos_set_cpuQ_to_stackQ_cpu_queue_cpu_Qid_queue_id_stk_qid */
#endif

#ifdef CMD_QOS_GET_CPUQ_TO_NORMALQ
/*
 * qos get cpu-queue-to-normal-queue
 */
cparser_result_t
cparser_cmd_qos_get_cpu_queue_to_normal_queue(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_pri_t   qid;
    rtk_pri_t   cpuQidMax;
    rtk_qid_t   normalQid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_OM_GET_CHIP_CAPACITY(unit, cpuQidMax, max_num_of_cpuQueue);

    diag_util_mprintf("CPU Queue ID\t  Normal Queue ID\n");
    for (qid = 0; qid <cpuQidMax; qid++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_cpuQid2QidMap_get(unit, qid, &normalQid), ret);
        diag_util_mprintf("CPU Queue ID %2u : %u\n", qid, normalQid);
    }

    diag_util_mprintf("\n");


    return CPARSER_OK;
}   /* end of cparser_cmd_qos_get_cpuQ_to_normalQ */
#endif

#ifdef CMD_QOS_SET_CPUQ_TO_NORMALQ_CPU_QUEUE_CPU_QID_QUEUE_ID_NRML_QID
/*
 * qos set cpu-queue-to-normal-queue cpu-queue-id <UINT:cpu_qid> queue-id <UINT:normal_qid>
 */
cparser_result_t
cparser_cmd_qos_set_cpu_queue_to_normal_queue_cpu_queue_id_cpu_qid_queue_id_normal_qid(
    cparser_context_t *context,
    uint32_t *cpu_Qid_ptr,
    uint32_t *nrml_qid_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_cpuQid2QidMap_set(unit, *cpu_Qid_ptr, *nrml_qid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_qos_set_cpuQ_to_normalQ_cpu_queue_cpu_Qid_queue_id_nrml_qid */
#endif

#ifdef CMD_QOS_GET_RSPAN_PRI_MAPPING
/*
 * qos get rspan-pri-mapping
 */
cparser_result_t
cparser_cmd_qos_get_rspan_pri_mapping(cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_pri_t   pri;
    rtk_pri_t   int_pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("rspan priority\t  Internal priority\n");
    for (pri = 0; pri <8; pri++)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_rspanPriRemap_get(unit, pri, &int_pri), ret);
        diag_util_mprintf(" %2u :\t %u\n", pri, int_pri);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}

#endif

#ifdef CMD_QOS_SET_RSPAN_PRI_MAPPING_PRI_INT_PRI
/*
 * qos set rspan-pri-mapping <UINT:pri> <UINT:int_pri>
 */
cparser_result_t
cparser_cmd_qos_set_rspan_pri_mapping_pri_int_pri(cparser_context_t *context,
    uint32_t *pri_ptr,
    uint32_t *int_pri_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_qos_rspanPriRemap_set(unit, *pri_ptr, *int_pri_ptr), ret);
    return CPARSER_OK;
}

#endif


