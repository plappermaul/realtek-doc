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
 * Purpose : Definition those BPE command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Bridge Port Extension
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/bpe.h>
#include <osal/memory.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_bpe.h>
#endif

const char text_bpe_fwdMode[RTK_BPE_FWDMODE_END+1][32] =
{
    "CB",
    "PE",
    "RTK_BPE_FWDMODE_END , remove it",
};

const char text_bpe_tagSts[RTK_BPE_TAGSTS_END+1][32] =
{
    "Untag",
    "Tag",
    "Mcast Untag",
    "Mcast Tag",
    "RTK_BPE_TAGSTS_END , remove it",
};

const char text_bpe_vlanTagSts[RTK_BPE_VTAGSTS_END+1][32] =
{
    "Normal",
    "Mcast Tag",
    "RTK_BPE_VTAGSTS_END , remove it",
};


#ifdef CMD_BPE_GET_PE_ETAG_TPID
/*
 * bpe get pe-etag tpid
 */
cparser_result_t
cparser_cmd_bpe_get_pe_etag_tpid(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32  tpid = BPE_ETAG_TPID_DEFAULT;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_get(unit, RTK_BPE_GCT_ETAG_TPID, (int32 *)&tpid), ret);
    DIAG_UTIL_MPRINTF("get tpid: 0x%x\n", tpid);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_pe_etag_tpid */
#endif

#ifdef CMD_BPE_SET_PE_ETAG_TPID_TPID
/*
 * bpe set pe-etag tpid <UINT:tpid>
 */
cparser_result_t
cparser_cmd_bpe_set_pe_etag_tpid_tpid(
    cparser_context_t *context,
    uint32_t *tpid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_set(unit, RTK_BPE_GCT_ETAG_TPID, *tpid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_pe_etag_tpid_tpid */
#endif

#ifdef CMD_BPE_GET_PE_ETAG_CPU_KEEP_STATE
/*
 * bpe get pe-etag cpu-keep state
 */
cparser_result_t
cparser_cmd_bpe_get_pe_etag_cpu_keep_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_get(unit, RTK_BPE_GCT_ETAG_CPU_KEEP, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("CPU keep state: %s\n", text_state[state]);


    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_pe_etag_cpu_keep_state */
#endif

#ifdef CMD_BPE_SET_PE_ETAG_CPU_KEEP_STATE_ENABLE_DISABLE
/*
 * bpe set pe-etag cpu-keep state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_pe_etag_cpu_keep_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(5, state);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_set(unit, RTK_BPE_GCT_ETAG_CPU_KEEP, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_pe_etag_cpu_keep_state_enable_disable */
#endif

#ifdef CMD_BPE_GET_ECID_RPF_FAIL_ACTION
/*
 * bpe get ecid-rpf-fail action
 */
cparser_result_t
cparser_cmd_bpe_get_ecid_rpf_fail_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_FORWARD;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_get(unit, RTK_BPE_GCT_ECID_RPF_FAIL_ACT, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("RPF fail action: %s\n", text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_ecid_rpf_fail_action */
#endif

#ifdef CMD_BPE_SET_ECID_RPF_FAIL_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * bpe set ecid-rpf-fail action ( forward | drop | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_bpe_set_ecid_rpf_fail_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_FORWARD;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(4, action);


    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_set(unit, RTK_BPE_GCT_ECID_RPF_FAIL_ACT, action), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_bpe_set_ecid_rpf_fail_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_BPE_GET_PE_MISS_FILTER_STATE
/*
 * bpe get pe-miss-filter state
 */
cparser_result_t
cparser_cmd_bpe_get_pe_miss_filter_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_get(unit, RTK_BPE_GCT_PE_LM_FTLR, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("PE lookup miss filter state: %s\n", text_state[state]);


    return CPARSER_OK;

}   /* end of cparser_cmd_bpe_get_pe_miss_filter_state */
#endif

#ifdef CMD_BPE_SET_PE_MISS_FILTER_STATE_ENABLE_DISABLE
/*
 * bpe set pe-miss-filter state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_pe_miss_filter_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(4, state);
    DIAG_UTIL_ERR_CHK(rtk_bpe_globalCtrl_set(unit, RTK_BPE_GCT_PE_LM_FTLR, state), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_bpe_set_pe_miss_filter_state_enable_disable */
#endif

#ifdef CMD_BPE_DUMP_PORT_PORTS_ALL
/*
 * bpe dump port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bpe_dump_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    uint32  val = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_fwdMode_t mode = RTK_BPE_FWDMODE_END;
    rtk_bpe_pcidCfgType_t type = RTK_BPE_PCFG_EXT_AND_BASE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("\nE-CID Configuration:\n");
    DIAG_UTIL_MPRINTF("PORT  EXT  BASE  NSG\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("%4u", port);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portPcid_get(unit, port, type, (rtk_bpe_ecid_t *)&val), ret);
        DIAG_UTIL_MPRINTF(" 0x%02x 0x%03x", BPE_ECID_EXT(val), BPE_ECID_BASE(val));

        DIAG_UTIL_ERR_CHK(rtk_bpe_portEcidNameSpaceGroupId_get(unit, port, &val), ret);
        DIAG_UTIL_MPRINTF(" 0x%02x",val);

        DIAG_UTIL_MPRINTF("\n");
    }


    DIAG_UTIL_MPRINTF("\nControl Configuration:\n");
    DIAG_UTIL_MPRINTF("PORT   ETAG-EN  FWD-MODE   USE-DEF   BASE-PE PCID-MATCH\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("%4u", port);
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_PETAG_PARSE_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portFwdMode_get(unit, port, &mode), ret);
        DIAG_UTIL_MPRINTF("%10s",text_bpe_fwdMode[mode]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_USE_DEFAULT, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_BASE_PE, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portPcidAct_get(unit, port, (rtk_action_t *)&val), ret);
        DIAG_UTIL_MPRINTF(" %s",text_action[val]);

        DIAG_UTIL_MPRINTF("\n");
    }

    DIAG_UTIL_MPRINTF("\nIngress Configuration:\n");
    DIAG_UTIL_MPRINTF("PORT RSVD-FLTR   MC-FLRT   RPF-CHK\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("%4u", port);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_IGR_RSVD_ECID_FLTR_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_IGR_MC_ECID_FLTR_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_IGR_ECID_RPF_CHK_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_MPRINTF("\n");
    }

    DIAG_UTIL_MPRINTF("\nEgress Configuration:\n");
    DIAG_UTIL_MPRINTF("PORT  PRI-KEEP   DEI-RMK  SRC-FLTR  ETAG-STS  VTAG-STS\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("%4u", port);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_EGR_TAG_PRI_KEEP_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_EGR_TAG_DEI_RMK_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_get(unit, port, RTK_BPE_PCT_EGR_SRC_PORT_FLTR_EN, (int32 *)&state), ret);
        DIAG_UTIL_MPRINTF("%10s",text_state[state]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portEgrTagSts_get(unit, port, (rtk_bpe_tagSts_t *)&val), ret);
        DIAG_UTIL_MPRINTF("%10s",text_bpe_tagSts[val]);

        DIAG_UTIL_ERR_CHK(rtk_bpe_portEgrVlanTagSts_get(unit, port, (rtk_bpe_vlanTagSts_t *)&val), ret);
        DIAG_UTIL_MPRINTF("%10s",text_bpe_vlanTagSts[val]);

        DIAG_UTIL_MPRINTF("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_dump_port_ports_all */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_CONTROL_PE_ETAG_PARSE_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) control pe-etag-parse state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_control_pe_etag_parse_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_PETAG_PARSE_EN;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_control_pe_etag_parse_state_enable_disable */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_CONTROL_FWD_MODE_CONTROL_BRIDGE_PORT_EXTENDER
/*
 * bpe set port ( <PORT_LIST:ports> | all ) control fwd-mode ( control-bridge | port-extender )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_control_fwd_mode_control_bridge_port_extender(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_fwdMode_t mode = RTK_BPE_FWDMODE_END;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('c' == TOKEN_CHAR(6, 0))
        mode = RTK_BPE_FWDMODE_CTRL_BRIDGE;
    else if ('p' == TOKEN_CHAR(6, 0))
        mode = RTK_BPE_FWDMODE_PORT_EXTENDER;


    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portFwdMode_set(unit, port, mode), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_control_fwd_mode_control_bridge_port_extender */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_NSG_NSG
/*
 * bpe set port ( <PORT_LIST:ports> | all ) nsg <UINT:nsg>
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_nsg_nsg(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *nsg_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portEcidNameSpaceGroupId_set(unit, port, *nsg_ptr), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_nsg_nsg */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_PCID_EXTENSION_PCID_EXT
/*
 * bpe set port ( <PORT_LIST:ports> | all ) pcid extension <UINT:pcid_ext>
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_pcid_extension_pcid_ext(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pcid_ext_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_bpe_ecid_t pcid;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    osal_memset(&pcid, 0, sizeof(pcid));
    pcid = BPE_ECID_PCID(*pcid_ext_ptr, 0);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portPcid_set(unit, port, RTK_BPE_PCFG_EXT_ONLY, pcid), ret);
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_pcid_extension_pcid_ext */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_PCID_BASE_PCID_BASE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) pcid base <UINT:pcid_base>
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_pcid_base_pcid_base(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pcid_base_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_bpe_ecid_t pcid;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    osal_memset(&pcid, 0, sizeof(pcid));
    pcid = BPE_ECID_PCID(0, *pcid_base_ptr);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portPcid_set(unit, port, RTK_BPE_PCFG_BASE_ONLY, pcid), ret);
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_pcid_base_pcid_base */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_CONTROL_PCID_USE_DEFAULT_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) control pcid use-default state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_control_pcid_use_default_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_USE_DEFAULT;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_control_pcid_use_default_state_enable_disable */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_CONTROL_PCID_BASE_PE_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) control pcid base-pe state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_control_pcid_base_pe_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_BASE_PE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_control_pcid_base_pe_state_enable_disable */
#endif


#ifdef CMD_BPE_SET_PORT_PORTS_ALL_CONTROL_PCID_MATCH_ECID_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU
/*
 * bpe set port ( <PORT_LIST:ports> | all ) control pcid match-ecid action ( forward | drop | trap-to-cpu | copy-to-cpu )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_control_pcid_match_ecid_action_forward_drop_trap_to_cpu_copy_to_cpu(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_FORWARD;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(8, action);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portPcidAct_set(unit, port, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_control_pcid_match_ecid_action_forward_drop_trap_to_cpu_copy_to_cpu */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_INGRESS_MULTICAST_ECID_FILTER_RESERVED_ECID_FILTER_ECID_RPF_CHECK_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) ingress ( multicast-ecid-filter | reserved-ecid-filter | ecid-rpf-check ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_ingress_multicast_ecid_filter_reserved_ecid_filter_ecid_rpf_check_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('m' == TOKEN_CHAR(5, 0))
        type = RTK_BPE_PCT_IGR_MC_ECID_FLTR_EN;
    else if ('r' == TOKEN_CHAR(5, 0))
        type = RTK_BPE_PCT_IGR_RSVD_ECID_FLTR_EN;
    else if ('e' == TOKEN_CHAR(5, 0))
        type = RTK_BPE_PCT_IGR_ECID_RPF_CHK_EN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_ingress_multicast_ecid_filter_reserved_ecid_filter_ecid_rpf_check_state_enable_disable */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_EGRESS_PE_ETAG_PRI_KEEP_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) egress pe-etag pri-keep state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_pri_keep_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_EGR_TAG_PRI_KEEP_EN;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_pri_keep_state_enable_disable */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_EGRESS_PE_ETAG_DEI_RMK_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) egress pe-etag dei-rmk state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_dei_rmk_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_EGR_TAG_DEI_RMK_EN;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }


    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_dei_rmk_state_enable_disable */
#endif

#ifdef CMD_BPE_SET_PORT_PORTS_ALL_EGRESS_PE_ETAG_TAG_STATUS_UNTAG_TAG_MCAST_UNTAG_MCAST_TAG
/*
 * bpe set port ( <PORT_LIST:ports> | all ) egress pe-etag tag-status ( untag | tag | mcast-untag | mcast-tag )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_tag_status_untag_tag_mcast_untag_mcast_tag(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_tagSts_t status = RTK_BPE_TAGSTS_UNTAGGED;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(7, 0))
    {
        status = RTK_BPE_TAGSTS_UNTAGGED;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        status = RTK_BPE_TAGSTS_TAGGED;
    }
    else if ('m' == TOKEN_CHAR(7, 0))
    {
        if ('u' == TOKEN_CHAR(7, 6))
            status = RTK_BPE_TAGSTS_PCID_CHK_MCAST_UNTAGGED;
        else if ('t' == TOKEN_CHAR(7, 6))
            status = RTK_BPE_TAGSTS_PCID_CHK_MCAST_TAGGED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portEgrTagSts_set(unit, port, status), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_egress_pe_etag_tag_status_untag_tag_mcast_untag_mcast_tag */
#endif


#ifdef CMD_BPE_SET_PORT_PORTS_ALL_EGRESS_VLAN_TAG_TAG_STATUS_NORMAL_MCAST_TAG
/*
 * bpe set port ( <PORT_LIST:ports> | all ) egress vlan-tag tag-status ( normal | mcast-tag )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_egress_vlan_tag_tag_status_normal_mcast_tag(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_vlanTagSts_t status = RTK_BPE_VTAGSTS_NORMAL;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('n' == TOKEN_CHAR(7, 0))
    {
        status = RTK_BPE_VTAGSTS_NORMAL;
    }
    else if ('m' == TOKEN_CHAR(7, 0))
    {
        status = RTK_BPE_VTAGSTS_MCAST_TAGGED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portEgrVlanTagSts_set(unit, port, status), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_egress_vlan_tag_tag_status_normal_mcast_tag */
#endif


#ifdef CMD_BPE_SET_PORT_PORTS_ALL_EGRESS_SRC_PORT_FILTER_STATE_ENABLE_DISABLE
/*
 * bpe set port ( <PORT_LIST:ports> | all ) egress src-port-filter state ( enable | disable )
 */
cparser_result_t
cparser_cmd_bpe_set_port_ports_all_egress_src_port_filter_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_bpe_portCtrlType_t type = RTK_BPE_PCT_EGR_SRC_PORT_FLTR_EN;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_bpe_portCtrl_set(unit, port, type, state), ret);
    }


    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_port_ports_all_egress_src_port_filter_state_enable_disable */
#endif

#ifdef CMD_BPE_GET_REMARKING_PE_ETAG_SYSTEM_INTERNAL_PRIORITY
/*
 * bpe get remarking pe-etag system internal-priority
 */
cparser_result_t
cparser_cmd_bpe_get_remarking_pe_etag_system_internal_priority(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint8   int_pri = 0;
    rtk_pri_t  pe_pri = 0;
    rtk_bpe_priRmkVal_t rmk_src;

    DIAG_UTIL_FUNC_INIT(unit);

    for (int_pri = 0; int_pri <= 7; int_pri++)
    {
        osal_memset(&rmk_src, 0, sizeof(rmk_src));
        rmk_src.pri.val = (rtk_pri_t)int_pri;
        DIAG_UTIL_ERR_CHK(rtk_bpe_priRemarking_get(unit, PETAG_PRI_RMK_SRC_INT_PRI, rmk_src, &pe_pri), ret);
        DIAG_UTIL_MPRINTF("internal prioirty %u remark pe priority: %u\n", int_pri, pe_pri);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_remarking_pe_etag_system_internal_priority */
#endif

#ifdef CMD_BPE_SET_REMARKING_PE_ETAG_SYSTEM_INTERNAL_PRIORITY_INTERNAL_PRIORITY_REMARK_PE_PRIORITY_REMARK_PE_PRIORITY
/*
 * bpe set remarking pe-etag system internal-priority <UINT:internal_priority> remark-pe-priority <UINT:remark_pe_priority>
 */
cparser_result_t
cparser_cmd_bpe_set_remarking_pe_etag_system_internal_priority_internal_priority_remark_pe_priority_remark_pe_priority(
    cparser_context_t *context,
    uint32_t *internal_priority_ptr,
    uint32_t *remark_pe_priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_priRmkVal_t rmk_src;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&rmk_src, 0, sizeof(rmk_src));
    rmk_src.pri.val = (rtk_pri_t)(*internal_priority_ptr);

    DIAG_UTIL_ERR_CHK(rtk_bpe_priRemarking_set(unit, PETAG_PRI_RMK_SRC_INT_PRI, rmk_src, (rtk_pri_t)(*remark_pe_priority_ptr)), ret);


    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_set_remarking_pe_etag_system_internal_priority_internal_priority_remark_pe_priority_remark_pe_priority */
#endif

#ifdef CMD_BPE_ADD_PVID_TABLE_NSG_NSG_ECID_ECID_VID_VID
/*
 * bpe add pvid-table nsg <UINT:nsg> ecid <UINT:ecid> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_bpe_add_pvid_table_nsg_nsg_ecid_ecid_vid_vid(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_pvidEntry_t pvid_entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&pvid_entry, 0, sizeof(pvid_entry));
    pvid_entry.nsg_id= (uint32)*nsg_ptr;
    pvid_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    pvid_entry.pvid = (rtk_vlan_t)*vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_bpe_pvidEntry_add(unit, &pvid_entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_add_pvid_table_nsg_nsg_ecid_ecid_vid_vid */
#endif

#ifdef CMD_BPE_ADD_PVID_TABLE_NSG_NSG_ECID_ECID_VID_VID_INGRESS_PORT_TRUNK_ID
/*
 * bpe add pvid-table nsg <UINT:nsg> ecid <UINT:ecid> vid <UINT:vid> ingress ( port | trunk ) <UINT:id>
 */
cparser_result_t
cparser_cmd_bpe_add_pvid_table_nsg_nsg_ecid_ecid_vid_vid_ingress_port_trunk_id(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr,
    uint32_t *vid_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_pvidEntry_t pvid_entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&pvid_entry, 0, sizeof(pvid_entry));
    pvid_entry.nsg_id= (uint32)*nsg_ptr;
    pvid_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    pvid_entry.pvid = (rtk_vlan_t)*vid_ptr;

    if ('p' == TOKEN_CHAR(10, 0))
    {
        pvid_entry.port_id = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(10, 0))
    {
        pvid_entry.flags |= RTK_BPE_PVID_FLAG_TRUNK_PORT;
        pvid_entry.trunk_id= *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }


    DIAG_UTIL_ERR_CHK(rtk_bpe_pvidEntry_add(unit, &pvid_entry), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_bpe_add_pvid_table_nsg_nsg_ecid_ecid_vid_vid_ingress_port_trunk_id */
#endif


#ifdef CMD_BPE_DEL_PVID_TABLE_NSG_NSG_ECID_ECID
/*
 * bpe del pvid-table nsg <UINT:nsg> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_bpe_del_pvid_table_nsg_nsg_ecid_ecid(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32 ret = RT_ERR_FAILED;
    rtk_bpe_pvidEntry_t pvid_entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&pvid_entry, 0, sizeof(pvid_entry));
    pvid_entry.nsg_id= (uint32)*nsg_ptr;
    pvid_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_bpe_pvidEntry_del(unit, &pvid_entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_del_pvid_table_nsg_nsg_ecid_ecid */
#endif

#ifdef CMD_BPE_GET_PVID_TABLE_NSG_NSG_ECID_ECID
/*
 * bpe get pvid-table nsg <UINT:nsg> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_bpe_get_pvid_table_nsg_nsg_ecid_ecid(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32 ret = RT_ERR_FAILED;
    rtk_bpe_pvidEntry_t pvid_entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&pvid_entry, 0, sizeof(pvid_entry));
    pvid_entry.nsg_id = (uint32)*nsg_ptr;
    pvid_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;

    DIAG_UTIL_MPRINTF("index  nsg    ecid  vid  is_trk   id\n");
    DIAG_UTIL_MPRINTF("----- ---- ------- ---- ------- ----\n");

    DIAG_UTIL_ERR_CHK(rtk_bpe_pvidEntry_get(unit, &pvid_entry), ret);

    DIAG_UTIL_MPRINTF("%5u 0x%02x 0x%05x %4u %7u %4u\n",
        pvid_entry.entry_idx,
        pvid_entry.nsg_id,
        pvid_entry.ecid,
        pvid_entry.pvid,
        (pvid_entry.flags&RTK_BPE_PVID_FLAG_TRUNK_PORT) ? 1 : 0,
        (pvid_entry.flags&RTK_BPE_PVID_FLAG_TRUNK_PORT) ? pvid_entry.trunk_id : pvid_entry.port_id);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_pvid_table_nsg_nsg_ecid_ecid */
#endif

#ifdef CMD_BPE_DUMP_PVID_TABLE
/*
 * bpe dump pvid-table
 */
cparser_result_t
cparser_cmd_bpe_dump_pvid_table(
    cparser_context_t *context)
{
    uint32 unit;
    int32 i = -1;
    int32 ret = RT_ERR_FAILED;
    uint32 total_entry = 0;
    rtk_bpe_pvidEntry_t pvid_entry;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_MPRINTF("index  nsg    ecid  vid  is_trk   id\n");
    DIAG_UTIL_MPRINTF("----- ---- ------- ---- ------- ----\n");

    do
    {
        osal_memset(&pvid_entry, 0 , sizeof(pvid_entry));
        if ((ret = rtk_bpe_pvidEntryNextValid_get(unit, &i, &pvid_entry)) != RT_ERR_OK)
            break;

        total_entry++;

        DIAG_UTIL_MPRINTF("%5u 0x%02x 0x%05x %4u %7u %4u\n",
            pvid_entry.entry_idx,
            pvid_entry.nsg_id,
            pvid_entry.ecid,
            pvid_entry.pvid,
            (pvid_entry.flags&RTK_BPE_PVID_FLAG_TRUNK_PORT) ? 1 : 0,
            (pvid_entry.flags&RTK_BPE_PVID_FLAG_TRUNK_PORT) ? pvid_entry.trunk_id : pvid_entry.port_id);

    } while (i != -1);

    diag_util_mprintf("\nTotal Number Of Entries : %d\n", total_entry);
    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_dump_pvid_table */
#endif


#ifdef CMD_BPE_ADD_FWD_TABLE_NSG_NSG_ECID_ECID_PORT_PORTS_ALL
/*
 * bpe add fwd-table nsg <UINT:nsg> ecid <UINT:ecid> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bpe_add_fwd_table_nsg_nsg_ecid_ecid_port_ports_all(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_fwdEntry_t  fwd_entry;
    diag_portlist_t     portlist;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&fwd_entry, 0, sizeof(fwd_entry));
    fwd_entry.nsg_id= (uint32)*nsg_ptr;
    fwd_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    fwd_entry.portmask = portlist.portmask;

    DIAG_UTIL_ERR_CHK(rtk_bpe_fwdEntry_add(unit, &fwd_entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_add_fwd_table_nsg_nsg_ecid_ecid_port_ports_all */
#endif

#ifdef CMD_BPE_DEL_FWD_TABLE_NSG_NSG_ECID_ECID
/*
 * bpe del fwd-table nsg <UINT:nsg> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_bpe_del_fwd_table_nsg_nsg_ecid_ecid(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32 ret = RT_ERR_FAILED;
    rtk_bpe_fwdEntry_t  fwd_entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&fwd_entry, 0, sizeof(fwd_entry));
    fwd_entry.nsg_id= (uint32)*nsg_ptr;
    fwd_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_bpe_fwdEntry_del(unit, &fwd_entry), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_bpe_del_fwd_table_nsg_nsg_ecid_ecid */
#endif

#ifdef CMD_BPE_GET_FWD_TABLE_NSG_NSG_ECID_ECID
/*
 * bpe get fwd-table nsg <UINT:nsg> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_bpe_get_fwd_table_nsg_nsg_ecid_ecid(
    cparser_context_t *context,
    uint32_t *nsg_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_bpe_fwdEntry_t fwd_entry;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];


    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&fwd_entry, 0, sizeof(fwd_entry));
    fwd_entry.nsg_id = (uint32)*nsg_ptr;
    fwd_entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_bpe_fwdEntry_get(unit, &fwd_entry), ret);

    osal_memset(port_list, 0, sizeof(port_list));
    diag_util_lPortMask2str(port_list, &fwd_entry.portmask);

    DIAG_UTIL_MPRINTF("nsg 0x%x ecid 0x%x: %s\n", fwd_entry.nsg_id, fwd_entry.ecid, port_list);

    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_get_fwd_table_nsg_nsg_ecid_ecid */
#endif

#ifdef CMD_BPE_DUMP_FWD_TABLE
/*
 * bpe dump fwd-table
 */
cparser_result_t
cparser_cmd_bpe_dump_fwd_table(
    cparser_context_t *context)
{
    uint32  unit;
    int32   i = -1;
    int32   ret = RT_ERR_FAILED;
    uint32  total_entry = 0;
    rtk_bpe_fwdEntry_t fwd_entry;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];


    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_MPRINTF("index  nsg    ecid portmask\n");
    DIAG_UTIL_MPRINTF("----- ---- ------- ----------\n");

    do
    {
        osal_memset(&fwd_entry, 0 , sizeof(fwd_entry));
        osal_memset(port_list, 0 , sizeof(port_list));

        if ((ret = rtk_bpe_fwdEntryNextValid_get(unit, &i, &fwd_entry)) != RT_ERR_OK)
            break;

        total_entry++;

        diag_util_lPortMask2str(port_list, &fwd_entry.portmask);

        DIAG_UTIL_MPRINTF("%5u 0x%02x 0x%05x %s\n",
            fwd_entry.entry_idx,
            fwd_entry.nsg_id,
            fwd_entry.ecid,
            port_list);

    } while (i != -1);

    diag_util_mprintf("\nTotal Number Of Entries : %d\n", total_entry);
    return CPARSER_OK;
}   /* end of cparser_cmd_bpe_dump_fwd_table */
#endif


