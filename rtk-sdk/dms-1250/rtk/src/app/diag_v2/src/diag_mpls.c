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
 * Purpose : Definition those MPLS command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) MPLS
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/mpls.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_mpls.h>
#endif
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */


/* Module Name : MPLS */
#ifdef CMD_MPLS_GET_ENCAP_TTL_INHERIT
/*
 * mpls get encap ttl inherit
 */
cparser_result_t
cparser_cmd_mpls_get_encap_ttl_inherit(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret;
    rtk_mpls_ttlInherit_t   inherit;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_mpls_ttlInherit_get(unit, &inherit);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tMPLS TTL inherit: ");
    if (RTK_MPLS_TTL_INHERIT_UNIFORM == inherit)
        diag_util_mprintf("Uniform\n");
    else if(RTK_MPLS_TTL_INHERIT_PIPE == inherit)
        diag_util_mprintf("Pipe\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_encap_ttl_inherit */
#endif  /* CPARSER_CMD_MPLS_GET_ENCAP_TTL_INHERIT */

#ifdef CMD_MPLS_SET_ENCAP_TTL_INHERIT_UNIFORM_PIPE
/*
 * mpls set encap ttl inherit ( uniform | pipe )
 */
cparser_result_t
cparser_cmd_mpls_set_encap_ttl_inherit_uniform_pipe(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret;
    rtk_mpls_ttlInherit_t   inherit;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(5, 0))
        inherit = RTK_MPLS_TTL_INHERIT_UNIFORM;
    else if('p' == TOKEN_CHAR(5, 0))
        inherit = RTK_MPLS_TTL_INHERIT_PIPE;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_ttlInherit_set(unit, inherit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_ttl_inherit_uniform_pipe */
#endif  /* CPARSER_CMD_MPLS_SET_ENCAP_TTL_INHERIT_UNIFORM_PIPE */

#ifdef CMD_MPLS_GET_ENCAP_LIB_INDEX
/*
 * mpls get encap lib <UINT:index>
 */
cparser_result_t
cparser_cmd_mpls_get_encap_lib_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32              unit = 0;
    int32               ret;
    rtk_mpls_encap_t    info;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    info.oper = RTK_MPLS_LABEL_OPER_SINGLE;

    ret = rtk_mpls_encap_get(unit, *index_ptr, &info);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tMPLS LIB %d ", *index_ptr);
    diag_util_mprintf("inner information: \n");
    diag_util_mprintf("\t\tLable:%d \n", info.label0);
    diag_util_mprintf("\t\tExp:%d \n", info.exp0);
    diag_util_mprintf("\t\tTTL:%d \n", info.ttl0);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_encap_lib_index */
#endif  /* CPARSER_CMD_MPLS_GET_ENCAP_LIB_INDEX */

#ifdef CMD_MPLS_SET_ENCAP_LIB_INDEX_LABEL_LABEL_EXP_EXP_TTL_TTL
/*
 * mpls set encap lib <UINT:index> label <UINT:label> exp <UINT:exp> ttl <UINT:ttl>
 */
cparser_result_t
cparser_cmd_mpls_set_encap_lib_index_label_label_exp_exp_ttl_ttl(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *label_ptr,
    uint32_t *exp_ptr,
    uint32_t *ttl_ptr)
{
    uint32              unit = 0;
    int32               ret;
    rtk_mpls_encap_t    info;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    info.oper = RTK_MPLS_LABEL_OPER_SINGLE;
    info.label0 = *label_ptr;
    info.exp0   = *exp_ptr;
    info.ttl0   = *ttl_ptr;

    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, *index_ptr, &info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_lib_index_label_exp_ttl */
#endif  /* CPARSER_CMD_MPLS_SET_ENCAP_LIB_INDEX_LABEL_EXP_TTL */

#ifdef CMD_MPLS_GET_STATE
/*
 * mpls get state
 */
cparser_result_t
cparser_cmd_mpls_get_state(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_mpls_enable_get(unit, &enable);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("MPLS decapsulation state: ");
    if (ENABLED == enable)
        diag_util_mprintf("Enable\n");
    else
        diag_util_mprintf("Disable\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_state */
#endif

#ifdef CMD_MPLS_SET_STATE_ENABLE_DISABLE
/*
 * mpls set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_mpls_set_state_enable_disable(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(3, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_enable_set(unit, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_state_enable_disable */
#endif

#ifdef CMD_MPLS_GET_TRAP_TARGET
/*
 * mpls get trap-target
 */
cparser_result_t
cparser_cmd_mpls_get_trap_target(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_mpls_trapTarget_get(unit, &target);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("MPLS trap target: ");
    if (RTK_TRAP_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_trap_target */
#endif

#ifdef CMD_MPLS_SET_TRAP_TARGET_LOCAL_MASTER
/*
 * mpls set trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_mpls_set_trap_target_local_master(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(3, 0))
    {
        target = RTK_TRAP_LOCAL;
    }
    else
    {
        target = RTK_TRAP_MASTER;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_trapTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_trap_target_local_master */
#endif

#ifdef CMD_MPLS_GET_EXCEPTION_LABEL_EXCEED_LABEL_UNKNOWN_TTL_FAIL_LABEL_0_15
/*
 * mpls get exception ( label-exceed | label-unknown | ttl-fail | label-0-15 )
 */
cparser_result_t
cparser_cmd_mpls_get_exception_label_exceed_label_unknown_ttl_fail_label_0_15(
    cparser_context_t *context)
{
    rtk_mpls_exceptionType_t    type;
    rtk_action_t                action;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(3, 6))
    {
        type = RTK_MPLS_ET_LABEL_EXCEED;
    }
    else if ('u' == TOKEN_CHAR(3, 6))
    {
        type = RTK_MPLS_ET_LABEL_UNKNOWN;
    }
    else if ('0' == TOKEN_CHAR(3, 6))
    {
        type = RTK_MPLS_ET_LABEL_0_15;
    }
    else
    {
        type = RTK_MPLS_ET_TTL_FAIL;
    }

    ret = rtk_mpls_exceptionCtrl_get(unit, type, &action);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("MPLS exception type %s: ", TOKEN_STR(3));
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");
    else
        diag_util_mprintf("Forward\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_exception_label_exceed_label_unknown_ttl_fail_label_0_15 */
#endif

#ifdef CMD_MPLS_SET_EXCEPTION_LABEL_EXCEED_LABEL_UNKNOWN_TTL_FAIL_DROP_TRAP
/*
 * mpls set exception ( label-exceed | label-unknown | ttl-fail ) ( drop | trap )
 */
cparser_result_t
cparser_cmd_mpls_set_exception_label_exceed_label_unknown_ttl_fail_drop_trap(
    cparser_context_t *context)
{
    rtk_mpls_exceptionType_t    type;
    rtk_action_t                action;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(3, 6))
    {
        type = RTK_MPLS_ET_LABEL_EXCEED;
    }
    else if ('u' == TOKEN_CHAR(3, 6))
    {
        type = RTK_MPLS_ET_LABEL_UNKNOWN;
    }
    else
    {
        type = RTK_MPLS_ET_TTL_FAIL;
    }

    if ('d' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_DROP;
    }
    else
    {
        action = ACTION_TRAP2CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_exceptionCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_exception_label_exceed_label_unknown_ttl_fail_drop_trap */
#endif

#ifdef CMD_MPLS_SET_EXCEPTION_LABEL_0_15_FORWARD_TRAP
/*
 * mpls set exception label-0-15 ( forward | trap )
 */
cparser_result_t
cparser_cmd_mpls_set_exception_label_0_15_forward_trap(
    cparser_context_t *context)
{
    rtk_mpls_exceptionType_t    type;
    rtk_action_t                action;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    type = RTK_MPLS_ET_LABEL_0_15;

    if ('f' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_FORWARD;
    }
    else
    {
        action = ACTION_TRAP2CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_exceptionCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_exception_label_0_15_forward_trap */
#endif

#ifdef CMD_MPLS_CREATE_NEXTHOP_INTERFACE_INTFID_MAC_MAC_ADDR_ENCAP_ENCAPID
/*
 * mpls create nexthop interface <UINT:intfId> mac <MACADDR:mac_addr> encap <UINT:encapId>
 */
cparser_result_t
cparser_cmd_mpls_create_nexthop_interface_intfId_mac_mac_addr_encap_encapId(
    cparser_context_t *context,
    uint32_t *intfId_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *encapId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&nh, 0, sizeof(rtk_mpls_nextHop_t));
    nh.intfIdx = *intfId_ptr;
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.encapId = *encapId_ptr;

    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_create(unit, &nh, &l3Id), ret);

    diag_util_mprintf("entry path ID : %d\n", l3Id);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_nexthop_interface_intfId_mac_mac_addr_encap_encapId */
#endif

#ifdef CMD_MPLS_CREATE_NEXTHOP_INTERFACE_INTFID_MAC_MAC_ADDR_ENCAP_ENCAPID_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * mpls create nexthop interface <UINT:intfId> mac <MACADDR:mac_addr> encap <UINT:encapId> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_mpls_create_nexthop_interface_intfId_mac_mac_addr_encap_encapId_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *intfId_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *encapId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&nh, 0, sizeof(rtk_mpls_nextHop_t));
    nh.intfIdx = *intfId_ptr;
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.encapId = *encapId_ptr;
    DIAG_UTIL_PARSE_L3_ACT(10, nh.l3_act);

    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_create(unit, &nh, &l3Id), ret);

    diag_util_mprintf("entry path ID : %d\n", l3Id);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_nexthop_interface_intfId_mac_mac_addr_encap_encapId_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_MPLS_SET_NEXTHOP_PATHID_INTERFACE_INTFID
/*
 * mpls set nexthop <UINT:pathId> interface <UINT:intfId>
 */
cparser_result_t
cparser_cmd_mpls_set_nexthop_pathId_interface_intfId(
    cparser_context_t *context,
    uint32_t *pathId_ptr,
    uint32_t *intfId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_get(unit, l3Id, &nh), ret);

    nh.intfIdx = *intfId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_set(unit, l3Id, &nh), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_nexthop_pathId_interface_intfId */
#endif

#ifdef CMD_MPLS_SET_NEXTHOP_PATHID_MAC_MAC_ADDR
/*
 * mpls set nexthop <UINT:pathId> mac <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_mpls_set_nexthop_pathId_mac_mac_addr(
    cparser_context_t *context,
    uint32_t *pathId_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_get(unit, l3Id, &nh), ret);

    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_set(unit, l3Id, &nh), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_nexthop_pathId_mac_mac_addr */
#endif

#ifdef CMD_MPLS_SET_NEXTHOP_PATHID_ENCAP_ENCAPID
/*
 * mpls set nexthop <UINT:pathId> encap <UINT:encapId>
 */
cparser_result_t
cparser_cmd_mpls_set_nexthop_pathId_encap_encapId(
    cparser_context_t *context,
    uint32_t *pathId_ptr,
    uint32_t *encapId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_get(unit, l3Id, &nh), ret);

    nh.encapId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_set(unit, l3Id, &nh), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_nexthop_pathId_encap_encapId */
#endif

#ifdef CMD_MPLS_SET_NEXTHOP_PATHID_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * mpls set nexthop <UINT:pathId> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_mpls_set_nexthop_pathId_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *pathId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_get(unit, l3Id, &nh), ret);

    DIAG_UTIL_PARSE_L3_ACT(5, nh.l3_act);
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_set(unit, l3Id, &nh), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_nexthop_pathId_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_MPLS_DESTROY_NEXTHOP_PATHID
/*
 * mpls destroy nexthop <UINT:pathId>
 */
cparser_result_t
cparser_cmd_mpls_destroy_nexthop_pathId(
    cparser_context_t *context,
    uint32_t *pathId_ptr)
{
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_destroy(unit, l3Id), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_destroy_nexthop_pathId */
#endif

#ifdef CMD_MPLS_GET_NEXTHOP_PATHID
/*
 * mpls get nexthop <UINT:pathId>
 */
cparser_result_t
cparser_cmd_mpls_get_nexthop_pathId(
    cparser_context_t *context,
    uint32_t *pathId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    rtk_l3_pathId_t     l3Id;
    uint32              unit;
    int32               ret;
    char                macStr[32];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    l3Id = *pathId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_get(unit, l3Id, &nh), ret);

    diag_util_mprintf("MPLS next hop entry %d:\n", l3Id);
    diag_util_mprintf("    interface index: %d\n", nh.intfIdx);
    diag_util_mac2str(macStr, nh.mac_addr.octet);
    diag_util_mprintf("    MAC address: %s\n", macStr);
    diag_util_mprintf("    encapsulation index: %d\n", nh.encapId);
    diag_util_mprintf("    L3 action : %s\n", text_l3_action[nh.l3_act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_nexthop_pathId */
#endif

#ifdef CMD_MPLS_CREATE_ENCAP_LABEL_LABELVAL
/*
 * mpls create encap label <UINT:labelVal>
 */
cparser_result_t
cparser_cmd_mpls_create_encap_label_labelVal(
    cparser_context_t *context,
    uint32_t *labelVal_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&encap, 0, sizeof(rtk_mpls_encap_t));
    encap.label = *labelVal_ptr;
    encap.tcAct = RTK_MPLS_TC_ASSIGN;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_create(unit, &encap, &entryId), ret);

    diag_util_mprintf("MPLS encapsulation entry index: %d\n", entryId);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_encap_label_labelVal */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_OPERATION_PUSH_SWAP
/*
 * mpls set encap <UINT:encapId> operation ( push | swap )
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_operation_push_swap(
    cparser_context_t *context,
    uint32_t *encapId_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    if ('p' == TOKEN_CHAR(5, 0))
    {
        encap.labelAct = RTK_MPLS_LABELACT_PUSH;
    }
    else
    {
        encap.labelAct = RTK_MPLS_LABELACT_SWAP;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_operation_push_swap */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_NEXT_ENCAP_NEXTENCAPID_NONE
/*
 * mpls set encap <UINT:encapId> next-encap ( <UINT:nextEncapId> | none )
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_next_encap_nextEncapId_none(
    cparser_context_t *context,
    uint32_t *encapId_ptr,
    uint32_t *nextEncapId_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    if ('n' == TOKEN_CHAR(5, 0))
    {
        encap.flags &= (~RTK_MPLS_FLAG_NEXTLABEL);
    }
    else
    {
        encap.flags |= RTK_MPLS_FLAG_NEXTLABEL;
        encap.nextEntryId = *nextEncapId_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_next_encap_nextEncapId_none */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_TTL_OP_FORCE_INHERIT
/*
 * mpls set encap <UINT:encapId> ttl-op ( force | inherit )
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_ttl_op_force_inherit(
    cparser_context_t *context,
    uint32_t *encapId_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    if ('f' == TOKEN_CHAR(5, 0))
    {
        encap.flags |= RTK_MPLS_FLAG_TTL_ASSIGN;
    }
    else
    {
        encap.flags &= (~RTK_MPLS_FLAG_TTL_ASSIGN);
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_ttl_op_force_inherit */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_TTL_TTLVAL
/*
 * mpls set encap <UINT:encapId> ttl <UINT:ttlVal>
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_ttl_ttlVal(
    cparser_context_t *context,
    uint32_t *encapId_ptr,
    uint32_t *ttlVal_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    encap.ttl = *ttlVal_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_ttl_ttlVal */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_TC_OP_INHERIT_FORCE_INTERNAL_PRIORITY_REMARK
/*
 * mpls set encap <UINT:encapId> tc-op ( inherit | force | internal-priority-remark )
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_tc_op_inherit_force_internal_priority_remark(
    cparser_context_t *context,
    uint32_t *encapId_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        if ('h' == TOKEN_CHAR(5, 2))
        {
            encap.tcAct = RTK_MPLS_TC_INHERIT;
        }
        else
        {
            encap.tcAct = RTK_MPLS_TC_INTPRI;
        }
    }
    else
    {
        encap.tcAct = RTK_MPLS_TC_ASSIGN;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_tc_op_inherit_force_internal_priority_remark */
#endif

#ifdef CMD_MPLS_SET_ENCAP_ENCAPID_TC_TCVAL
/*
 * mpls set encap <UINT:encapId> tc <UINT:tcVal>
 */
cparser_result_t
cparser_cmd_mpls_set_encap_encapId_tc_tcVal(
    cparser_context_t *context,
    uint32_t *encapId_ptr,
    uint32_t *tcVal_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    encap.tc = *tcVal_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_set(unit, entryId, &encap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_encap_encapId_tc_tcVal */
#endif

#ifdef CMD_MPLS_DESTROY_ENCAP_ENCAPID
/*
 * mpls destroy encap <UINT:encapId>
 */
cparser_result_t
cparser_cmd_mpls_destroy_encap_encapId(
    cparser_context_t *context,
    uint32_t *encapId_ptr)
{
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_destroy(unit, entryId), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_destroy_encap_encapId */
#endif

#ifdef CMD_MPLS_GET_ENCAP_ENCAPID
/*
 * mpls get encap <UINT:encapId>
 */
cparser_result_t
cparser_cmd_mpls_get_encap_encapId(
    cparser_context_t *context,
    uint32_t *encapId_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    entryId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_get(unit, entryId, &encap), ret);

    diag_util_mprintf("MPLS encapsulation entry %d:\n", entryId);
    diag_util_mprintf("    Label: %d\n", encap.label);
    diag_util_mprintf("    Label action: ");
    if (RTK_MPLS_LABELACT_PUSH == encap.labelAct)
    {
        diag_util_mprintf("Push\n");
    }
    else
    {
        diag_util_mprintf("Swap\n");
    }

    /* TTL */
    diag_util_mprintf("    TTL: ");
    if (RTK_MPLS_FLAG_TTL_ASSIGN & encap.flags)
    {
        diag_util_mprintf("Assign %d\n", encap.ttl);
    }
    else
    {
        diag_util_mprintf("Inherit\n");
    }

    /* TC */
    diag_util_mprintf("    Traiff class: ");
    switch (encap.tcAct)
    {
        case RTK_MPLS_TC_ASSIGN:
            diag_util_mprintf("Assign %d\n", encap.tc);
            break;
        case RTK_MPLS_TC_INTPRI:
            diag_util_mprintf("Internal priority remark\n");
            break;
        case RTK_MPLS_TC_INHERIT:
            diag_util_mprintf("Inherit\n");
            break;
        default:
            return CPARSER_NOT_OK;
    }
    /* next entry */
    diag_util_mprintf("    Next entry: ");
    if (RTK_MPLS_FLAG_NEXTLABEL & encap.flags)
    {
        diag_util_mprintf("%d\n", encap.nextEntryId);
    }
    else
    {
        diag_util_mprintf("None\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_encap_encapId */
#endif

#ifdef CMD_MPLS_GET_ENCAP_ID_LABEL_LABELVAL
/*
 * mpls get encap-id label <UINT:labelVal>
 */
cparser_result_t
cparser_cmd_mpls_get_encap_id_label_labelVal(
    cparser_context_t *context,
    uint32_t *labelVal_ptr)
{
    rtk_mpls_encap_t    encap;
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    encap.label = *labelVal_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encapId_find(unit, &encap, &entryId), ret);

    diag_util_mprintf("MPLS encapsulation entry label %d in %d\n",
            encap.label, entryId);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_encap_id_label_labelVal */
#endif

#ifdef CMD_MPLS_GET_DECAP_HASH_ALGORITHM
/*
 * mpls get decap hash-algorithm
 */
cparser_result_t
cparser_cmd_mpls_get_decap_hash_algorithm(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    uint8   hashAlgo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mpls_hashAlgo_get(unit, &hashAlgo), ret);
    diag_util_mprintf("MPLS hash algorithm: ");
    if (RTK_MPLS_HASHALGO_0 == hashAlgo)
    {
        diag_util_mprintf("0\n");
    }
    else
    {
        diag_util_mprintf("1\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_decap_hash_algorithm */
#endif

#ifdef CMD_MPLS_SET_DECAP_HASH_ALGORITHM_ALGO0_ALGO1
/*
 * mpls set decap hash-algorithm ( algo0 | algo1 )
 */
cparser_result_t
cparser_cmd_mpls_set_decap_hash_algorithm_algo0_algo1(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    uint8   hashAlgo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('0' == TOKEN_CHAR(4, 4))
    {
        hashAlgo = RTK_MPLS_HASHALGO_0;
    }
    else
    {
        hashAlgo = RTK_MPLS_HASHALGO_1;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_hashAlgo_set(unit, hashAlgo), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_hash_algorithm_algo0_algo1 */
#endif

#ifdef CMD_MPLS_CREATE_DECAP_LABEL_LABELVAL
/*
 * mpls create decap label <UINT:labelVal>
 */
cparser_result_t
cparser_cmd_mpls_create_decap_label_labelVal(
    cparser_context_t *context,
    uint32_t *labelVal_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&decap, 0, sizeof(rtk_mpls_decapEntry_t));
    decap.label = *labelVal_ptr;
    decap.labelAct = RTK_MPLS_LABELACT_POP;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_create(unit, &decap, &entryId), ret);

    diag_util_mprintf("MPLS decapsulation entry index: %d\n", entryId);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_decap_label_labelVal */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_OPERATION_POP_SWAP_PHP
/*
 * mpls set decap <UINT:decapId> operation ( pop | swap | php )
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_operation_pop_swap_php(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    if ('o' == TOKEN_CHAR(5, 1))
    {
        decap.labelAct = RTK_MPLS_LABELACT_POP;
    }
    else if ('s' == TOKEN_CHAR(5, 0))
    {
        decap.labelAct = RTK_MPLS_LABELACT_SWAP;
    }
    else
    {
        decap.labelAct = RTK_MPLS_LABELACT_PHP;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_operation_pop_swap_php */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_TTL_INHERIT_STATE_DISABLE_ENABLE
/*
 * mpls set decap <UINT:decapId> ttl-inherit state ( disable | enable )
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_ttl_inherit_state_disable_enable(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    if ('d' == TOKEN_CHAR(6, 0))
    {
        decap.flags &= (~RTK_MPLS_FLAG_TTL_INHERIT);
    }
    else
    {
        decap.flags |= RTK_MPLS_FLAG_TTL_INHERIT;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_ttl_inherit_state_disable_enable */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_DSCP_OP_UNIFORM_PIPE
/*
 * mpls set decap <UINT:decapId> dscp-op ( uniform | pipe )
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_dscp_op_uniform_pipe(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    if ('u' == TOKEN_CHAR(5, 0))
    {
        decap.flags &= (~RTK_MPLS_FLAG_TC_INHERIT);
    }
    else
    {
        decap.flags |= RTK_MPLS_FLAG_TC_INHERIT;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_dscp_op_uniform_pipe */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_INTERNAL_PRIORITY_STATE_DISABLE_ENABLE
/*
 * mpls set decap <UINT:decapId> internal-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_internal_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    if ('d' == TOKEN_CHAR(6, 0))
    {
        decap.flags &= (~RTK_MPLS_FLAG_INTPRI_ASSIGN);
    }
    else
    {
        decap.flags |= RTK_MPLS_FLAG_INTPRI_ASSIGN;
    }

    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_internal_priority_state_disable_enable */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_INTERNAL_PRIORITY_INTPRI
/*
 * mpls set decap <UINT:decapId> internal-priority <UINT:intPri>
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_internal_priority_intPri(
    cparser_context_t *context,
    uint32_t *decapId_ptr,
    uint32_t *intPri_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    decap.intPri = *intPri_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_internal_priority_intPri */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_PRIORITY_SELECTION_PRISELID
/*
 * mpls set decap <UINT:decapId> priority-selection <UINT:priSelId>
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_priority_selection_priSelId(
    cparser_context_t *context,
    uint32_t *decapId_ptr,
    uint32_t *priSelId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    decap.priSelTblId = *priSelId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_priority_selection_priSelId */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_INTERFACE_INTFID
/*
 * mpls set decap <UINT:decapId> interface <UINT:intfId>
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_interface_intfId(
    cparser_context_t *context,
    uint32_t *decapId_ptr,
    uint32_t *intfId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    decap.intfId = *intfId_ptr;
    decap.flags &= (~RTK_MPLS_FLAG_ECMP);
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_interface_intfId */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_NEXT_HOP_NHID
/*
 * mpls set decap <UINT:decapId> next-hop <UINT:nhId>
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_next_hop_nhId(
    cparser_context_t *context,
    uint32_t *decapId_ptr,
    uint32_t *nhId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    decap.intfId = *nhId_ptr;
    decap.flags &= (~RTK_MPLS_FLAG_ECMP);
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_next_hop_nhId */
#endif

#ifdef CMD_MPLS_SET_DECAP_DECAPID_ECMP_ECMPID
/*
 * mpls set decap <UINT:decapId> ecmp <UINT:ecmpId>
 */
cparser_result_t
cparser_cmd_mpls_set_decap_decapId_ecmp_ecmpId(
    cparser_context_t *context,
    uint32_t *decapId_ptr,
    uint32_t *ecmpId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    decap.intfId = *ecmpId_ptr;
    decap.flags |= RTK_MPLS_FLAG_ECMP;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_set(unit, entryId, &decap), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_decap_decapId_ecmp_ecmpId */
#endif

#ifdef CMD_MPLS_DESTROY_DECAP_DECAPID
/*
 * mpls destroy decap <UINT:decapId>
 */
cparser_result_t
cparser_cmd_mpls_destroy_decap_decapId(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_entryId_t  entryId;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_destroy(unit, entryId), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_destroy_decap_decapId */
#endif

#ifdef CMD_MPLS_GET_DECAP_DECAPID
/*
 * mpls get decap <UINT:decapId>
 */
cparser_result_t
cparser_cmd_mpls_get_decap_decapId(
    cparser_context_t *context,
    uint32_t *decapId_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    entryId = *decapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decap_get(unit, entryId, &decap), ret);

    diag_util_mprintf("MPLS decapsulation entry:\n");
    diag_util_mprintf("    label: %d\n", decap.label);
    diag_util_mprintf("    label action: ");
    switch (decap.labelAct)
    {
        case RTK_MPLS_LABELACT_POP:
            diag_util_mprintf("Pop\n");
            break;
        case RTK_MPLS_LABELACT_SWAP:
            diag_util_mprintf("Swap\n");
            break;
        case RTK_MPLS_LABELACT_PHP:
            diag_util_mprintf("PHP\n");
            break;
        default:
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf("    TTL inherit: ");
    if (RTK_MPLS_FLAG_TTL_INHERIT & decap.flags)
    {
        diag_util_mprintf("Enable\n");
    }
    else
    {
        diag_util_mprintf("Disable\n");
    }

    /* TC */
    diag_util_mprintf("    TC inherit: ");
    if (RTK_MPLS_FLAG_TC_INHERIT & decap.flags)
    {
        diag_util_mprintf("Enable\n");
    }
    else
    {
        diag_util_mprintf("Disable\n");
    }

    /* internal priority */
    diag_util_mprintf("    Internal priority assign: ");
    if (RTK_MPLS_FLAG_INTPRI_ASSIGN & decap.flags)
    {
        diag_util_mprintf("%d\n", decap.intPri);
    }
    else
    {
        diag_util_mprintf("Disable\n");
    }

    /* priority selection group */
    diag_util_mprintf("    Priroity selection group: %d\n", decap.priSelTblId);
    /* interface/nh/ecmp */
    if (RTK_MPLS_LABELACT_POP == decap.labelAct)
    {
        diag_util_mprintf("    Interface index: %d\n", decap.intfId);
    }
    else
    {
        if (RTK_MPLS_FLAG_ECMP & decap.flags)
        {
            diag_util_mprintf("    ECMP index: %d\n", decap.intfId);
        }
        else
        {
            diag_util_mprintf("    Next hop index: %d\n", decap.intfId);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_decap_decapId */
#endif

#ifdef CMD_MPLS_GET_DECAP_ID_LABEL_LABELVAL
/*
 * mpls get decap-id label <UINT:labelVal>
 */
cparser_result_t
cparser_cmd_mpls_get_decap_id_label_labelVal(
    cparser_context_t *context,
    uint32_t *labelVal_ptr)
{
    rtk_mpls_decapEntry_t   decap;
    rtk_mpls_entryId_t      entryId;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    decap.label = *labelVal_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_decapId_find(unit, &decap, &entryId), ret);
    diag_util_mprintf("MPLS decapsulation label %d in entry index %d\n",
            decap.label, entryId);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_decap_id_label_labelVal */
#endif

#ifdef CMD_MPLS_GET_EGRESS_TC_MAP_SOURCE_DP_DP_INTERNAL_PRIORITY_PRIORITY
/*
 * mpls get egress-tc-map source dp <UINT:dp> internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_mpls_get_egress_tc_map_source_dp_dp_internal_priority_priority(
    cparser_context_t *context,
    uint32_t *dp_ptr,
    uint32_t *priority_ptr)
{
    rtk_mpls_egrTcMapSrc_t  src;
    uint32                  unit;
    int32                   ret;
    uint8                   tc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    src.dp = *dp_ptr;
    src.pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_egrTcMap_get(unit, &src, &tc), ret);
    diag_util_mprintf("DP %u internal priority %u maps egress MPLS TC %u\n",
            *dp_ptr, *priority_ptr, tc);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_get_egress_tc_map_source_dp_dp_internal_priority_priority */
#endif

#ifdef CMD_MPLS_SET_EGRESS_TC_MAP_SOURCE_DP_DP_INTERNAL_PRIORITY_PRIORITY_MAP_TC_TC
/*
 * mpls set egress-tc-map source dp <UINT:dp> internal-priority <UINT:priority> map tc <UINT:tc>
 */
cparser_result_t
cparser_cmd_mpls_set_egress_tc_map_source_dp_dp_internal_priority_priority_map_tc_tc(
    cparser_context_t *context,
    uint32_t *dp_ptr,
    uint32_t *priority_ptr,
    uint32_t *tc_ptr)
{
    rtk_mpls_egrTcMapSrc_t  src;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    src.dp = *dp_ptr;
    src.pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_egrTcMap_set(unit, &src, (uint8)*tc_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_set_egress_tc_map_source_dp_dp_internal_priority_priority_map_tc_tc */
#endif

#ifdef CMD_MPLS_CREATE_NEXTHOP_NHID_INTERFACE_INTFID_MAC_MAC_ADDR_ENCAP_ENCAPID
/*
 * mpls create nexthop <UINT:nhId> interface <UINT:intfId> mac <MACADDR:mac_addr> encap <UINT:encapId>
 */
cparser_result_t
cparser_cmd_mpls_create_nexthop_nhId_interface_intfId_mac_mac_addr_encap_encapId(
    cparser_context_t *context,
    uint32_t *nhId_ptr,
    uint32_t *intfId_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *encapId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&nh, 0, sizeof(rtk_mpls_nextHop_t));
    nh.intfIdx = *intfId_ptr;
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.encapId = *encapId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_create_id(unit, &nh, *nhId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_nexthop_nhId_interface_intfId_mac_mac_addr_encap_encapId */
#endif

#ifdef CMD_MPLS_CREATE_NEXTHOP_NHID_INTERFACE_INTFID_MAC_MAC_ADDR_ENCAP_ENCAPID_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * mpls create nexthop <UINT:nhId> interface <UINT:intfId> mac <MACADDR:mac_addr> encap <UINT:encapId> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_mpls_create_nexthop_nhId_interface_intfId_mac_mac_addr_encap_encapId_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *nhId_ptr,
    uint32_t *intfId_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *encapId_ptr)
{
    rtk_mpls_nextHop_t  nh;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&nh, 0, sizeof(rtk_mpls_nextHop_t));
    nh.intfIdx = *intfId_ptr;
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.encapId = *encapId_ptr;
    DIAG_UTIL_PARSE_L3_ACT(11, nh.l3_act);
    DIAG_UTIL_ERR_CHK(rtk_mpls_nextHop_create_id(unit, &nh, *nhId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_nexthop_nhId_interface_intfId_mac_mac_addr_encap_encapId_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_MPLS_CREATE_ENCAP_ENCAPID_LABEL_LABELVAL
/*
 * mpls create encap <UINT:encapId> label <UINT:labelVal>
 */
cparser_result_t
cparser_cmd_mpls_create_encap_encapId_label_labelVal(
    cparser_context_t *context,
    uint32_t *encapId_ptr,
    uint32_t *labelVal_ptr)
{
    rtk_mpls_encap_t    encap;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&encap, 0, sizeof(rtk_mpls_encap_t));
    encap.label = *labelVal_ptr;
    encap.tcAct = RTK_MPLS_TC_ASSIGN;
    DIAG_UTIL_ERR_CHK(rtk_mpls_encap_create_id(unit, &encap, *encapId_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mpls_create_encap_encapId_label_labelVal */
#endif

