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
 * Purpose : Define diag shell functions for vxlan.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) vxlan diag shell.
 */


#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <rtk/vxlan.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_vxlan.h>
#endif

static const char text_trap_target[RTK_TRAP_END][8] =
{
    "Local",
    "Master",
};

static const char text_ovid_cmd[RTK_VXLAN_OVID_CMD_END][32] =
{
    "untag and pri tag",
    "untag",
    "all",
};

static const char text_ivid_cmd[RTK_VXLAN_IVID_CMD_END][32] =
{
    "untag and pri tag",
    "untag",
    "all",
};

static const char text_vlan_type[VLAN_TYPE_END][16] =
{
    "inner vlan",
    "outer vlan",
};



#ifdef CMD_VXLAN_GET_TRAP_TARGET
/*
 * vxlan get trap-target
 */
cparser_result_t
cparser_cmd_vxlan_get_trap_target(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_trapTarget_t target = RTK_TRAP_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, RTK_VXLAN_GCT_VXLAN_TRAP_TARGET, (int32 *)&target), ret);
    DIAG_UTIL_MPRINTF("VXLAN trap taget: %s\n", text_trap_target[target]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_trap_target */
#endif

#ifdef CMD_VXLAN_SET_TRAP_TARGET_LOCAL_MASTER
/*
 * vxlan set trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_vxlan_set_trap_target_local_master(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_trapTarget_t target = RTK_TRAP_END;

    DIAG_UTIL_FUNC_INIT(unit);
    if ('l' == TOKEN_CHAR(3, 0))
        target = RTK_TRAP_LOCAL;
    else if ('m' == TOKEN_CHAR(3, 0))
        target = RTK_TRAP_MASTER;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, RTK_VXLAN_GCT_VXLAN_TRAP_TARGET, (int32)target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_trap_target_local_master */
#endif

#ifdef CMD_VXLAN_GET_INVALID_HEADER_ACTION
/*
 * vxlan get invalid-header action
 */
cparser_result_t
cparser_cmd_vxlan_get_invalid_header_action(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, RTK_VXLAN_GCT_VXLAN_INVALID_HDR_ACT, (int32 *)&act), ret);
    DIAG_UTIL_MPRINTF("VXLAN invalid-header frame action: %s\n", text_action[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_invalid_header_action */
#endif

#ifdef CMD_VXLAN_SET_INVALID_HEADER_ACTION_DROP_TRAP_TO_CPU
/*
 * vxlan set invalid-header action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_vxlan_set_invalid_header_action_drop_trap_to_cpu(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(4, act);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, RTK_VXLAN_GCT_VXLAN_INVALID_HDR_ACT, (int32)act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_invalid_header_action_drop_trap_to_cpu */
#endif

#ifdef CMD_VXLAN_GET_CONTROL_FRAME_ACTION
/*
 * vxlan get control-frame action
 */
cparser_result_t
cparser_cmd_vxlan_get_control_frame_action(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, RTK_VXLAN_GCT_VXLAN_CTRL_FRAME_ACT, (int32 *)&act), ret);
    DIAG_UTIL_MPRINTF("VXLAN control frame action: %s\n", text_action[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_control_frame_action */
#endif

#ifdef CMD_VXLAN_SET_CONTROL_FRAME_ACTION_FORWARD_TRAP_TO_CPU
/*
 * vxlan set control-frame action ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_vxlan_set_control_frame_action_forward_trap_to_cpu(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(4, act);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, RTK_VXLAN_GCT_VXLAN_CTRL_FRAME_ACT, (int32)act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_control_frame_action_forward_trap_to_cpu */
#endif

#ifdef CMD_VXLAN_GET_VXLAN_VXLAN_GPE_EXCEPTION_FLAGS
/*
 * vxlan get ( vxlan | vxlan-gpe ) exception-flags
 */
cparser_result_t
cparser_cmd_vxlan_get_vxlan_vxlan_gpe_exception_flags(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_globalCtrlType_t type;
    uint32 flags;

    DIAG_UTIL_FUNC_INIT(unit);

    type = (osal_strcmp(TOKEN_STR(2), "vxlan") == 0)?
        RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS :
        RTK_VXLAN_GCT_VXLAN_GPE_EXCEPT_FLAGS;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, type, (int32 *)&flags), ret);
    if (type == RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS)
    {
        DIAG_UTIL_MPRINTF("VXLAN exception flags: 0x%02X\n", flags);
    } else {
        DIAG_UTIL_MPRINTF("VXLAN-GPE exception flags: 0x%02X\n", flags);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_vxlan_vxlan_gpe_exception_flags */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_VXLAN_GPE_EXCEPTION_FLAGS_FLAGS
/*
 * vxlan set ( vxlan | vxlan-gpe ) exception-flags <UINT:flags>
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_vxlan_gpe_exception_flags_flags(
    cparser_context_t *context,
    uint32_t *flags_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_globalCtrlType_t type;
    uint32 flags = *flags_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    type = (osal_strcmp(TOKEN_STR(2), "vxlan") == 0)?
        RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS :
        RTK_VXLAN_GCT_VXLAN_GPE_EXCEPT_FLAGS;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, type, (int32)flags), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_vxlan_gpe_exception_flags_flags */
#endif

#ifdef CMD_VXLAN_GET_EXCEPTION_FRAME_ACTION
/*
 * vxlan get exception-frame action
 */
cparser_result_t
cparser_cmd_vxlan_get_exception_frame_action(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS_ACT, (int32 *)&act), ret);
    DIAG_UTIL_MPRINTF("VXLAN exception flags action: %s\n", text_action[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_exception_frame_action */
#endif

#ifdef CMD_VXLAN_SET_EXCEPTION_FRAME_ACTION_FORWARD_DROP_COPY_TO_CPU_TRAP_TO_CPU
/*
 * vxlan set exception-frame action ( forward | drop | copy-to-cpu | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_vxlan_set_exception_frame_action_forward_drop_copy_to_cpu_trap_to_cpu(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(4, act);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS_ACT, (int32)act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_exception_frame_action_forward_drop_copy_to_cpu_trap_to_cpu */
#endif

#ifdef CMD_VXLAN_GET_VXLAN_ENTRY_LOOKUP_MISS_ACTION
/*
 * vxlan get vxlan-entry lookup-miss action
 */
cparser_result_t
cparser_cmd_vxlan_get_vxlan_entry_lookup_miss_action(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_get(unit, RTK_VXLAN_GCT_VXLAN_LU_MIS_ACT, (int32 *)&act), ret);
    DIAG_UTIL_MPRINTF("VXLAN lookup-miss action: %s\n", text_action[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_vxlan_entry_lookup_miss_action */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_LOOKUP_MISS_ACTION_DROP_TRAP_TO_CPU
/*
 * vxlan set vxlan-entry lookup-miss action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_lookup_miss_action_drop_trap_to_cpu(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(5, act);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_globalCtrl_set(unit, RTK_VXLAN_GCT_VXLAN_LU_MIS_ACT, (int32)act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_lookup_miss_action_drop_trap_to_cpu */
#endif

#ifdef CMD_VXLAN_DUMP_VXLAN_ENTRY_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * vxlan dump vxlan-entry from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_vxlan_dump_vxlan_entry_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32  unit;
    int32 base = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;
    uint32  entryNum = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('b' == TOKEN_CHAR(4, 0))
    {
        base = -1;  /* from beginning */
    }
    else
    {
        /* including the start number */
        base = ((*begin_index_ptr) > 0) ? ((*begin_index_ptr) - 1) : (-1);
    }

    DIAG_UTIL_MPRINTF("  Index |    Intf ID | VNI      \n");
    DIAG_UTIL_MPRINTF("--------+------------+----------\n");
    do
    {
        DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_getNext(unit, &base, &entry), ret);
        if (('e' != TOKEN_CHAR(6, 0)) && (base > (*end_index_ptr)))
        {
            break;
        }
        else if (base >= 0)
        {
            entryNum++;
            DIAG_UTIL_MPRINTF(" %6d | 0x%08X | %8d\n", entry.entry_idx, entry.intf_id, entry.vni);
        }
    } while (base >= 0);

    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", entryNum);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_dump_vxlan_entry_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_VXLAN_ADD_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI
/*
 * vxlan add vxlan-entry intf <UINT:intf_id> vni <UINT:vni>
 */
cparser_result_t
cparser_cmd_vxlan_add_vxlan_entry_intf_intf_id_vni_vni(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_add(unit, &entry), ret);

    DIAG_UTIL_MPRINTF("  VXLAN entry index : %u\n", entry.entry_idx);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_add_vxlan_entry_intf_intf_id_vni_vni */
#endif

#ifdef CMD_VXLAN_DEL_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI
/*
 * vxlan del vxlan-entry intf <UINT:intf_id> vni <UINT:vni>
 */
cparser_result_t
cparser_cmd_vxlan_del_vxlan_entry_intf_intf_id_vni_vni(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_del_vxlan_entry_intf_intf_id_vni_vni */
#endif

#ifdef CMD_VXLAN_DEL_VXLAN_ENTRY_ALL
/*
 * vxlan del vxlan-entry all
 */
cparser_result_t
cparser_cmd_vxlan_del_vxlan_entry_all(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_delAll(unit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_del_vxlan_entry_all */
#endif

#ifdef CMD_VXLAN_GET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI
/*
 * vxlan get vxlan-entry intf <UINT:intf_id> vni <UINT:vni>
 */
cparser_result_t
cparser_cmd_vxlan_get_vxlan_entry_intf_intf_id_vni_vni(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    if (RT_ERR_OK != (ret = rtk_vxlan_vni_get(unit, &entry)))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("  VXLAN entry (intf-id = 0x%X, VNI = %u)\n", entry.intf_id, entry.vni);
    DIAG_UTIL_MPRINTF("--------------------------------------------------\n");
    DIAG_UTIL_MPRINTF("  Outer VLAN command : %s (OVID: %u)\n", text_ovid_cmd[entry.ovid_cmd], entry.ovid);
    DIAG_UTIL_MPRINTF("  Inner VLAN command : %s (IVID: %u)\n", text_ivid_cmd[entry.ovid_cmd], entry.ivid);
    DIAG_UTIL_MPRINTF("     Forwarding VLAN : %s\n", text_vlan_type[entry.fwd_vlan]);
    DIAG_UTIL_MPRINTF("   internal priority : %d\n", entry.int_pri);
    DIAG_UTIL_MPRINTF("  priority group idx : %d\n", entry.priGrp_idx);
    DIAG_UTIL_MPRINTF("   QoS profile index : %d\n", entry.qosPro_idx);
    DIAG_UTIL_MPRINTF("         entry index : %d\n\n", entry.entry_idx);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_get_vxlan_entry_intf_intf_id_vni_vni */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI_INNER_TAG_OUTER_TAG_UNTAG_UNTAG_PRI_ALL_VID
/*
 * vxlan set vxlan-entry intf <UINT:intf_id> vni <UINT:vni> ( inner-tag | outer-tag ) ( untag | untag-pri | all ) <UINT:vid>
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_inner_tag_outer_tag_untag_untag_pri_all_vid(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr,
    uint32_t *vid_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_get(unit, &entry), ret);

    if ('i' == TOKEN_CHAR(7,0))
    {
        if (osal_strcmp(TOKEN_STR(8), "untag") == 0)
            entry.ivid_cmd = RTK_VXLAN_IVID_CMD_UNTAG;
        else if (osal_strcmp(TOKEN_STR(8), "untag-pri") == 0)
            entry.ivid_cmd = RTK_VXLAN_IVID_CMD_UNTAG_PRITAG;
        else if (osal_strcmp(TOKEN_STR(8), "all") == 0)
            entry.ivid_cmd = RTK_VXLAN_IVID_CMD_ALL;
        else
            entry.ivid_cmd = RTK_VXLAN_IVID_CMD_END;    /* error */

        entry.ivid = *vid_ptr;
    }
    else if ('o' == TOKEN_CHAR(7,0))
    {
        if (osal_strcmp(TOKEN_STR(8), "untag") == 0)
            entry.ovid_cmd = RTK_VXLAN_OVID_CMD_UNTAG;
        else if (osal_strcmp(TOKEN_STR(8), "untag-pri") == 0)
            entry.ovid_cmd = RTK_VXLAN_OVID_CMD_UNTAG_PRITAG;
        else if (osal_strcmp(TOKEN_STR(8), "all") == 0)
            entry.ovid_cmd = RTK_VXLAN_OVID_CMD_ALL;
        else
            entry.ovid_cmd = RTK_VXLAN_OVID_CMD_END;    /* error */

        entry.ovid = *vid_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_set(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_inner_tag_outer_tag_untag_untag_pri_all_vid */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI_FORWARD_VLAN_INNER_OUTER
/*
 * vxlan set vxlan-entry intf <UINT:intf_id> vni <UINT:vni> forward-vlan ( inner | outer )
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_forward_vlan_inner_outer(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_get(unit, &entry), ret);

    entry.fwd_vlan = ('i' == TOKEN_CHAR(8,0))? INNER_VLAN : OUTER_VLAN;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_set(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_forward_vlan_inner_outer */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI_PRIORITY_GROUP_GROUP
/*
 * vxlan set vxlan-entry intf <UINT:intf_id> vni <UINT:vni> priority-group <UINT:group>
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_priority_group_group(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr,
    uint32_t *group_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_get(unit, &entry), ret);

    entry.priGrp_idx = *group_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_set(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_priority_group_group */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI_INTERNAL_PRIORITY_PRIORITY
/*
 * vxlan set vxlan-entry intf <UINT:intf_id> vni <UINT:vni> internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_internal_priority_priority(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr,
    uint32_t *priority_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_get(unit, &entry), ret);

    entry.int_pri = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_set(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_internal_priority_priority */
#endif

#ifdef CMD_VXLAN_SET_VXLAN_ENTRY_INTF_INTF_ID_VNI_VNI_QOS_PROFILE_INDEX
/*
 * vxlan set vxlan-entry intf <UINT:intf_id> vni <UINT:vni> qos-profile <UINT:index>
 */
cparser_result_t
cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_qos_profile_index(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_vxlan_vniEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0x00, sizeof(rtk_vxlan_vniEntry_t));
    entry.intf_id = *intf_id_ptr;
    entry.vni = *vni_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_get(unit, &entry), ret);

    entry.qosPro_idx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vxlan_vni_set(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vxlan_set_vxlan_entry_intf_intf_id_vni_vni_qos_profile_index */
#endif


