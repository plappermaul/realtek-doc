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
 * Purpose : Definition those OAM command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
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
#include <rtk/oam.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_oam.h>
#endif
#define UTIL_STRING_BUFFER_LENGTH   (30)

#define MIN_PKTLEN         64
#define MAX_PKTLEN      (9*1024) /*9k*/

#ifdef CMD_OAM_GET_ASIC_AUTO_DYING_GASP_PORT_PORTS_ALL_STATE
/*
 * oam get asic-auto-dying-gasp port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_oam_get_asic_auto_dying_gasp_port_ports_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    rtk_enable_t            enable = DISABLED;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\tPort %u:\n", port);

        ret = rtk_oam_autoDyingGaspEnable_get(unit, port, &enable);
        if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else
        {
            if(ENABLED == enable)
                diag_util_mprintf("\t\tAsic Auto Dying Gasp  : ENABLE\n");
            else
                diag_util_mprintf("\t\tAsic Auto Dying Gasp  : DISABLE\n");
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_DYING_GASP_WAITING_TIME
/*
 * oam get dying-gasp waiting-time
 */
cparser_result_t cparser_cmd_oam_get_dying_gasp_waiting_time(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    uint32                  value = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


    ret = rtk_oam_dyingGaspWaitTime_get(unit, &value);
    if ( (ret!= RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tDying Gasp Waiting Time : %u\n", value);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_OAM_SET_ASIC_AUTO_DYING_GASP_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * oam set asic-auto-dying-gasp port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_oam_set_asic_auto_dying_gasp_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    rtk_enable_t            enable;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_autoDyingGaspEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_DYING_GASP_PORT_PORTS_ALL_PAYLOAD_PAYLOAD
/* convert Integer from string to number */
int32
diag_oam_str2IntArray(uint8 *int_array, uint8 *str, uint32 size)
{
    uint32 str_idx, array_idx, hi_bits;
    uint8  value = 0;

    if (!(int_array && str))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
        return  RT_ERR_FAILED;
    }

    if (!((strlen((char *)str) > 2) && ('0' == str[0]) && ('x' == str[1])))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
        return  RT_ERR_FAILED;
    }

    value = 0;
    array_idx = size - 1;
    hi_bits = 0;

    /* exclude 0x head */
    for (str_idx = (strlen((char *)str) - 1); str_idx >= 2; str_idx--)
    {
        if (('0' <= str[str_idx]) && ('9' >= str[str_idx]))
        {
            value = str[str_idx] - '0';
        }
        else if (('a' <= str[str_idx]) && ('f' >= str[str_idx]))
        {
            value = str[str_idx] - 'a' + 10;
        }
        else if (('A' <= str[str_idx]) && ('F' >= str[str_idx]))
        {
            value = str[str_idx] - 'A' + 10;
        }
        else
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
            return  RT_ERR_FAILED;
        }

        if(hi_bits == 1)
        {
            int_array[array_idx] = int_array[array_idx] + (value  << 4);
            hi_bits = 0;
            array_idx--;
        }
        else
        {
            int_array[array_idx] = value;
            hi_bits = 1;
        }
    }

    return RT_ERR_OK;
}   /* end of diag_oam_str2IntArray */

/*
 * oam set dying-gasp port ( <PORT_LIST:ports> | all ) payload <STRING:payload>
 */
cparser_result_t cparser_cmd_oam_set_dying_gasp_port_ports_all_payload_payload(cparser_context_t *context,
    char **port_ptr,
    char **payload_ptr)
{
    uint32                  unit = 0, size;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint8                   data[RTK_OAM_DYINGGASPPAYLOAD_MAX];

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    size = strlen(*payload_ptr);
    if (size > RTK_OAM_DYINGGASPPAYLOAD_MAX)
    {
        diag_util_printf("The length of payload is too long!\n");
        return CPARSER_NOT_OK;
    }

    size = ((size - 2) + 1) / 2;
    if (diag_oam_str2IntArray(data, (uint8 *)*payload_ptr, size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_portDyingGaspPayload_set(unit, port, data, size), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_DYING_GASP_SEND_ENABLE
/*
 * oam set dying-gasp send enable
 */
cparser_result_t cparser_cmd_oam_set_dying_gasp_send_enable(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_dyingGaspSend_set(unit, ENABLED), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_DYING_GASP_WAITING_TIME_TIME
/*
 * oam set dying-gasp waiting-time <UINT:time>
 */
cparser_result_t cparser_cmd_oam_set_dying_gasp_waiting_time_time(cparser_context_t *context,
    uint32_t *time_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_oam_dyingGaspWaitTime_set(unit, *time_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_DYING_GASP_PACKET_COUNT
/*
 * oam get dying-gasp packet-count
 */
cparser_result_t
cparser_cmd_oam_get_dying_gasp_packet_count(
    cparser_context_t *context)
{
    uint32  unit;
    uint32  cnt;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_dyingGaspPktCnt_get(unit, &cnt);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("The dying gasp packet count: %d\n", cnt);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_DYING_GASP_PACKET_COUNT_COUNT
/*
 * oam set dying-gasp packet-count <UINT:count>
 */
cparser_result_t
cparser_cmd_oam_set_dying_gasp_packet_count_count(
    cparser_context_t *context,
    uint32_t *count_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_oam_dyingGaspPktCnt_set(unit, *count_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_LOOPBACK_CTRL_MAC_SWAP_STATE
/*
 * oam get loopback-ctrl mac-swap state
 */
cparser_result_t
cparser_cmd_oam_get_loopback_ctrl_mac_swap_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_loopbackMacSwapEnable_get(unit, &enable);
    if ((ret!= RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if( DISABLED == enable )
        diag_util_mprintf("OAM MAC swap state: Disable\n");
    else if( ENABLED == enable )
        diag_util_mprintf("OAM MAC swap state: Enable\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_LOOPBACK_CTRL_PORT_PORTS_ALL_MUX
/*
 * oam get loopback-ctrl port ( <PORT_LIST:ports> | all ) mux
 */
cparser_result_t
cparser_cmd_oam_get_loopback_ctrl_port_ports_all_mux(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_oam_portLoopbackMuxAction_get(unit, port, &action);
        if ((ret!= RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\tPort %u:\n", port);
        if( ACTION_DROP == action )
            diag_util_mprintf("\t\tOAM multiplexer action: Drop\n");
        else if( ACTION_FORWARD == action )
            diag_util_mprintf("\t\tOAM multiplexer action: Forward\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_LOOPBACK_CTRL_MAC_SWAP_STATE_DISABLE_ENABLE
/*
 * oam set loopback-ctrl mac-swap state ( disable | enable )
 */
cparser_result_t
cparser_cmd_oam_set_loopback_ctrl_mac_swap_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_oam_loopbackMacSwapEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_LOOPBACK_CTRL_PORT_PORTS_ALL_MUX_ACTION_DROP_FORWARD
/*
 * oam set loopback-ctrl port ( <PORT_LIST:ports> | all ) mux action ( drop | forward )
 */
cparser_result_t
cparser_cmd_oam_set_loopback_ctrl_port_ports_all_mux_action_drop_forward(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    muxAction;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(7, 0))
        muxAction = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        muxAction = ACTION_FORWARD;
    else
    {
        diag_util_printf("User config: muxAction Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_portLoopbackMuxAction_set(unit, port, muxAction), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_PCP
/* oam get cfm ccm pcp */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_pcp(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  pcp;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmPcp_get(unit, &pcp);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tPriority Code Point value for CCM tx frame: %d\n", pcp);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_cfm_ccm_pcp */
#endif

#ifdef CMD_OAM_SET_CFM_CCM_PCP_VALUE
/* oam set cfm ccm pcp <UINT:value> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_pcp_value(cparser_context_t *context,
                                      uint32_t *value_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((value_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmPcp_set(unit, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_CFI
/* oam get cfm ccm cfi */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_cfi(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  cfi;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmCfi_get(unit, &cfi);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCanonical Format Identifier value for CCM tx frame: %d\n", cfi);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_CFI_VALUE
/* oam set cfm ccm cfi <UINT:value> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_cfi_value(cparser_context_t *context,
                                      uint32_t *value_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((value_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmCfi_set(unit, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_TPID
/* oam get cfm ccm tpid */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_tpid(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  tpid;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmTpid_get(unit, &tpid);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tTPID value for CCM tx frame: 0x%04x\n", tpid);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_TPID_VALUE
/* oam set cfm ccm tpid <HEX:value> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_tpid_value(cparser_context_t *context,
                                       uint32_t *value_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmTpid_set(unit, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_LIFETIME
/* oam get cfm ccm <UINT:instance> lifetime */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_lifetime(cparser_context_t *context,
                                                    uint32_t *instance_ptr)
{
    uint32  unit = 0;
    uint32  lifetime;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstLifetime_get(unit, *instance_ptr, &lifetime);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM instance %d reset lifetime: %d\n",
                      *instance_ptr, lifetime);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_LIFETIME_VALUE
/* oam set cfm ccm <UINT:instance> lifetime <UINT:value> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_lifetime_value(cparser_context_t *context,
                                                          uint32_t *instance_ptr,
                                                          uint32_t *value_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstLifetime_set(unit, *instance_ptr, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_MEPID
/* oam get cfm ccm mepid */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_mepid(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  mepid;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmMepid_get(unit, &mepid);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM mepid: %d\n", mepid);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_MEPID_MEPID
/* oam set cfm ccm mepid <UINT:mepid> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_mepid_mepid(cparser_context_t *context,
                                        uint32_t *mepid_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmMepid_set(unit, *mepid_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_LIFETIME
/* oam get cfm ccm lifetime */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_lifetime(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  lifetime;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmIntervalField_get(unit, &lifetime);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM lifetime: %d\n", lifetime);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_LIFETIME_LIFETIME
/* oam set cfm ccm lifetime <UINT:lifetime> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_lifetime_lifetime(cparser_context_t *context,
                                              uint32_t *lifetime_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmIntervalField_set(unit, *lifetime_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_MDL
/* oam get cfm ccm mdl */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_mdl(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  level;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmMdl_get(unit, &level);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM MD level %d\n", level);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_MDL_MDL
/* oam set cfm ccm mdl <UINT:mdl> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_mdl_mdl(cparser_context_t *context,
                                    uint32_t *mdl_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmMdl_set(unit, *mdl_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_TAG_STATE
/* oam get cfm ccm <UINT:instance> tag-state */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_tag_state(cparser_context_t *context,
                                                uint32_t *instance_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstTagStatus_get(unit, *instance_ptr, &enable);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("CFM CCM instance %d tag status: ", *instance_ptr);
    if( DISABLED == enable )
        diag_util_mprintf("Disable\n");
    else if( ENABLED == enable )
        diag_util_mprintf("Enable\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_TAG_STATE_DISABLE_ENABLE
/* oam set cfm ccm <UINT:instance> tag-state ( disable | enable ) */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_tag_state_disable_enable(cparser_context_t *context,
                                                               uint32_t *instance_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTagStatus_set(unit, *instance_ptr, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_VID
/* oam get cfm ccm <UINT:instance> vid */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_vid(cparser_context_t *context,
                                         uint32_t *instance_ptr)
{
    uint32      unit = 0;
    int32       ret;
    rtk_vlan_t  vid;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstVid_get(unit, *instance_ptr, &vid);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM instance %d vid: %d\n", *instance_ptr, vid);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_VID_VID
/* oam set cfm ccm <UINT:instance> vid <UINT:vid> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_vid_vid(cparser_context_t *context,
                                             uint32_t *instance_ptr,
                                             uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstVid_set(unit, *instance_ptr, *vid_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_MAID
/* oam get cfm ccm <UINT:instance> maid */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_maid(cparser_context_t *context,
                                          uint32_t *instance_ptr)
{
    uint32  unit = 0;
    uint32  maid;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstMaid_get(unit, *instance_ptr, &maid);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM instance %d MAID: %d\n", *instance_ptr, maid);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_MAID_MAID
/* oam set cfm ccm <UINT:instance> maid <UINT:maid> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_maid_maid(cparser_context_t *context,
                                               uint32_t *instance_ptr,
                                               uint32_t *maid_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstMaid_set(unit, *instance_ptr, *maid_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_TX_STATE
/* oam get cfm ccm <UINT:instance> tx-state */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_tx_state(cparser_context_t *context,
                                               uint32_t *instance_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstTxStatus_get(unit, *instance_ptr, &enable);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("CFM CCM instance %d tx status: ", *instance_ptr);
    if( DISABLED == enable )
        diag_util_mprintf("Disable\n");
    else if( ENABLED == enable )
        diag_util_mprintf("Enable\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_TX_STATE_DISABLE_ENABLE
/* oam set cfm ccm <UINT:instance> tx-state ( disable | enable ) */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_tx_state_disable_enable(cparser_context_t *context,
                                                              uint32_t *instance_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxStatus_set(unit, *instance_ptr, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_INTERVAL
/* oam get cfm ccm <UINT:instance> interval */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_interval(cparser_context_t *context,
                                              uint32_t *instance_ptr)
{
    uint32  unit = 0;
    uint32  interval;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstInterval_get(unit, *instance_ptr, &interval);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM instance %d interval: %d\n", *instance_ptr, interval);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_INTERVAL_INTERVAL
/* oam set cfm ccm <UINT:instance> interval <UINT:interval> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_interval_interval(cparser_context_t *context,
                                                       uint32_t *instance_ptr,
                                                       uint32_t *interval_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstInterval_set(unit, *instance_ptr, *interval_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_TX_PORT_INDEX_PORT
/* oam get cfm ccm <UINT:instance> tx <UINT:port_index> port */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_tx_port_index_port(cparser_context_t *context,
                                                   uint32_t *instance_ptr,
                                                   uint32_t *port_index_ptr)
{
    rtk_oam_cfmInstMember_t member;
    uint32                  unit = 0;
    int32                   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&member, 0, sizeof(rtk_oam_cfmInstMember_t));

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (0 == *port_index_ptr)
            DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmTxInstPort_get(unit, *instance_ptr,
                    *port_index_ptr, &member.member0_port), ret);
        else
            DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmTxInstPort_get(unit, *instance_ptr,
                    *port_index_ptr, &member.member1_port), ret);
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMember_get(unit, *instance_ptr, &member), ret);
    }


    diag_util_mprintf("CFM CCM TX instance %d index %d ", *instance_ptr, *port_index_ptr);

    if (0 == *port_index_ptr)
    {
        if (0 == member.member0_truk_present)
            diag_util_mprintf("Port %d\n", member.member0_port);
        else
            diag_util_mprintf("Trunk %d\n", member.member0_port);
    }
    else
    {
        if (0 == member.member1_truk_present)
            diag_util_mprintf("Port %d\n", member.member1_port);
        else
            diag_util_mprintf("Trunk %d\n", member.member1_port);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_TX_PORT_INDEX_PORT_PORT
/* oam set cfm ccm <UINT:instance> tx <UINT:port_index> port <UINT:port> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_tx_port_index_port_port(cparser_context_t *context,
                                                        uint32_t *instance_ptr,
                                                        uint32_t *port_index_ptr,
                                                        uint32_t *port_ptr)
{
    rtk_oam_cfmInstMember_t member;
    uint32                  unit = 0;
    int32                   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMember_get(unit, *instance_ptr, &member), ret);
    }


    if (0 == *port_index_ptr)
    {
        member.member0_truk_present = 0;
        member.member0_port = *port_ptr;
    }
    else
    {
        member.member1_truk_present = 0;
        member.member1_port = *port_ptr;
    }

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmTxInstPort_set(unit, *instance_ptr,
                *port_index_ptr, *port_ptr), ret);
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMember_set(unit, *instance_ptr,
                &member), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_RX_VID
/* oam get cfm ccm <UINT:instance> rx vid */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_rx_vid(cparser_context_t *context,
                                            uint32_t *instance_ptr)
{
    uint32      unit = 0;
    int32       ret;
    rtk_vlan_t  vid;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmRxInstVid_get(unit, *instance_ptr, &vid);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM instance %d rx vid: %d\n", *instance_ptr, vid);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_RX_VID_VID
/* oam set cfm ccm <UINT:instance> rx vid <UINT:vid> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_rx_vid_vid(cparser_context_t *context,
                                                uint32_t *instance_ptr,
                                                uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((instance_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((vid_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmRxInstVid_set(unit, *instance_ptr, *vid_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_RX_PORT_INDEX_PORT
/* oam get cfm ccm <UINT:instance> rx <UINT:port_index> port */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_rx_port_index_port(cparser_context_t *context,
                                                   uint32_t *instance_ptr,
                                                   uint32_t *port_index_ptr)
{
    rtk_oam_cfmInstMember_t member;
    uint32                  unit = 0;
    int32                   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&member, 0, sizeof(rtk_oam_cfmInstMember_t));

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (0 == *port_index_ptr)
            DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmRxInstPort_get(unit, *instance_ptr,
                    *port_index_ptr, &member.member0_port), ret);
        else
            DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmRxInstPort_get(unit, *instance_ptr,
                    *port_index_ptr, &member.member1_port), ret);
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstRxMember_get(unit, *instance_ptr, &member), ret);
    }

    diag_util_mprintf("\tCFM CCM RX instance %d index %d ",
                      *instance_ptr, *port_index_ptr);
    if (0 == *port_index_ptr)
    {
        if (0 == member.member0_truk_present)
            diag_util_mprintf("Port %d\n", member.member0_port);
        else
            diag_util_mprintf("Trunk %d\n", member.member0_port);
    }
    else
    {
        if (0 == member.member1_truk_present)
            diag_util_mprintf("Port %d\n", member.member1_port);
        else
            diag_util_mprintf("Trunk %d\n", member.member1_port);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_RX_PORT_INDEX_PORT_PORT
/* oam set cfm ccm <UINT:instance> rx <UINT:port_index> port <UINT:port> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_rx_port_index_port_port(cparser_context_t *context,
                                                        uint32_t *instance_ptr,
                                                        uint32_t *port_index_ptr,
                                                        uint32_t *port_ptr)
{
    rtk_oam_cfmInstMember_t member;
    uint32                  unit = 0;
    int32                   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstRxMember_get(unit, *instance_ptr, &member), ret);
    }

    if (0 == *port_index_ptr)
    {
        member.member0_truk_present = 0;
        member.member0_port = *port_ptr;
    }
    else
    {
        member.member1_truk_present = 0;
        member.member1_port = *port_ptr;
    }

    #if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmRxInstPort_set(unit, *instance_ptr,
                *port_index_ptr, *port_ptr), ret);
    }
    else
    #endif  /* defined(CONFIG_SDK_RTL8390) */
    {
        DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstRxMember_set(unit, *instance_ptr,
                &member), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_RX_PORT_INDEX_TRUNK_TRUNK
/* oam set cfm ccm <UINT:instance> rx <UINT:port_index> trunk <UINT:trunk> */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_rx_port_index_trunk_trunk(cparser_context_t *context,
                                                        uint32_t *instance_ptr,
                                                        uint32_t *port_index_ptr,
                                                        uint32_t *trunk_ptr)
{
    rtk_oam_cfmInstMember_t member;
    uint32                  unit = 0;
    int32                   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstRxMember_get(unit, *instance_ptr, &member), ret);

    if (0 == *port_index_ptr)
    {
        member.member0_truk_present = 1;
        member.member0_port = *trunk_ptr;
    }
    else
    {
        member.member1_truk_present = 1;
        member.member1_port = *trunk_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstRxMember_set(unit, *instance_ptr,
            &member), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_INDEX_KEEPALIVE_COUNTER
/* oam get cfm ccm <UINT:instance> <UINT:index> keepalive-counter */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_index_keepalive_counter(cparser_context_t *context,
                                                     uint32_t *instance_ptr,
                                                     uint32_t *index_ptr)
{
    uint32  unit = 0;
    uint32  keepalive;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmCcmInstAliveTime_get(unit, *instance_ptr, *index_ptr, &keepalive);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM RX instance %d index %d keep alive: %d\n",
                      *instance_ptr, *index_ptr, keepalive);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_PORT_PORT_ALL_STATE
/* oam get cfm eth-dm port ( <PORT_LIST:port> | all ) state */
cparser_result_t
cparser_cmd_oam_get_cfm_eth_dm_port_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\tPort %u:\n", port);
        diag_util_mprintf("\tCFM ETH-DM state: ");

        ret = rtk_oam_cfmPortEthDmEnable_get(unit, port, &enable);
        if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else
        {
            if(ENABLED == enable)
                diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_ETH_DM_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/* oam set cfm eth-dm port ( <PORT_LIST:port> | all ) state ( disable | enable ) */
cparser_result_t
cparser_cmd_oam_set_cfm_eth_dm_port_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_oam_cfmPortEthDmEnable_set(unit, port, enable);
        if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_RX_TIMESTAMP_INDEX_INDEX
/*
 * oam get cfm eth-dm rx-timestamp index <UINT:index>
 */
cparser_result_t
cparser_cmd_oam_get_cfm_eth_dm_rx_timestamp_index_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_time_timeStamp_t timeStamp;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmEthDmRxTimestamp_get(unit, *index_ptr, &timeStamp), ret);
    diag_util_mprintf("\tETH-DM Rx Timestamp [%u]: %llu.%09u\n", *index_ptr, timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_cfm_eth_dm_rx_timestamp_index_index */
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_SPEED_10M_100M_1G_10G_TX_ADJUST
/*oam get cfm eth-dm speed ( 10m | 100m | 1g | 10g ) tx-adjust*/
cparser_result_t cparser_cmd_oam_get_cfm_eth_dm_speed_10m_100m_1g_10g_tx_adjust(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_oam_cfmEthDmTxDelay_t txDelay;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmEthDmTxDelay_get(unit, &txDelay);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('g' == TOKEN_CHAR(5, 1))
    {
        diag_util_mprintf("\tCFM ETH-DM TX Adjustmemt (500M/1G/2.5G) : %u ns\n", txDelay.delay_1G * 8);
    }
    else if('m' == TOKEN_CHAR(5, 2))
    {
        diag_util_mprintf("\tCFM ETH-DM TX Adjustmemt (10M) : %u ns\n", txDelay.delay_10M  * 8);
    }
    else if('g' == TOKEN_CHAR(5, 2))
    {
        diag_util_mprintf("\tCFM ETH-DM TX Adjustmemt (10G) : %u ns\n", txDelay.delay_10G  * 8);
    }
    else if('m' == TOKEN_CHAR(5, 3))
    {
        diag_util_mprintf("\tCFM ETH-DM TX Adjustmemt (100M) : %u ns\n", txDelay.delay_100M  * 8);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_REFERENCE_TIME
/*oam get cfm eth-dm reference-time*/
cparser_result_t cparser_cmd_oam_get_cfm_eth_dm_reference_time(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_time_timeStamp_t    timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("The time-stamp of reference time \n");
    if ((ret = rtk_oam_cfmEthDmRefTime_get(unit, &timeStamp)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    diag_util_mprintf("\tRef-Time : %llu.%09u\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_REFERENCE_TIME_STATE
/*oam get cfm eth-dm reference-time state*/
cparser_result_t cparser_cmd_oam_get_cfm_eth_dm_reference_time_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_oam_cfmEthDmRefTimeEnable_get(unit, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tRef-Time Clock : %s\n", (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_CFM_ETH_DM_REFERENCE_TIME_FREQUENCY
/*oam get cfm eth-dm reference-time frequency*/
cparser_result_t cparser_cmd_oam_get_cfm_eth_dm_reference_time_frequency(cparser_context_t *context)
{
    uint32     unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32     freq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("The frequency of reference time \n");
    if ((ret = rtk_oam_cfmEthDmRefTimeFreq_get(unit, &freq)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    diag_util_mprintf("\tFrequency Of Ref-Time : %x\n", freq);

    return CPARSER_OK;
}
#endif


#ifdef CMD_OAM_SET_CFM_ETH_DM_SPEED_10M_100M_1G_10G_TX_ADJUST_TX_DELAY
cparser_result_t cparser_cmd_oam_set_cfm_eth_dm_speed_10m_100m_1g_10g_tx_adjust_tx_delay(cparser_context_t *context,
    uint32_t *tx_delay_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_oam_cfmEthDmTxDelay_t txDelay;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_oam_cfmEthDmTxDelay_get(unit, &txDelay);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

     if('g' == TOKEN_CHAR(5, 1))
    {
        txDelay.delay_1G = *tx_delay_ptr;
    }
    else if('m' == TOKEN_CHAR(5, 2))
    {
        txDelay.delay_10M = *tx_delay_ptr;
    }
    else if('g' == TOKEN_CHAR(5, 2))
    {
        txDelay.delay_10G = *tx_delay_ptr;
    }
    else if('m' == TOKEN_CHAR(5, 3))
    {
        txDelay.delay_100M = *tx_delay_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_oam_cfmEthDmTxDelay_set(unit, txDelay);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_ETH_DM_REFERENCE_TIME_SECOND_SECOND_NANOSECOND_NANOSECOND
/*oam set cfm eth-dm reference-time second <UINT:second> nanosecond <UINT:nanosecond>*/
cparser_result_t cparser_cmd_oam_set_cfm_eth_dm_reference_time_second_second_nanosecond_nanosecond(cparser_context_t *context,
    uint32_t *second_ptr,
    uint32_t *nanosecond_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_time_timeStamp_t    timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    if ((ret = rtk_oam_cfmEthDmRefTime_set(unit, timeStamp)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_OAM_SET_CFM_ETH_DM_REFERENCE_TIME_STATE_ENABLE_DISABLE
/*oam set cfm eth-dm reference-time state ( enable | disable )*/
cparser_result_t cparser_cmd_oam_set_cfm_eth_dm_reference_time_state_enable_disable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    enable = ('e' == TOKEN_CHAR(6, 0))? ENABLED : DISABLED;

    if ((ret = rtk_oam_cfmEthDmRefTimeEnable_set(unit, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_SET_CFM_ETH_DM_REFERENCE_TIME_FREQUENCY_FREQUENCYVAL
/*oam set cfm eth-dm reference-time frequency <UINT:frequencyVal>*/
cparser_result_t cparser_cmd_oam_set_cfm_eth_dm_reference_time_frequency_frequencyVal(cparser_context_t *context,
    uint32_t *frequencyVal_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_oam_cfmEthDmRefTimeFreq_set(unit, *frequencyVal_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;

}
#endif


#ifdef CMD_OAM_SET_LINK_FAULT_MONITOR_STATE_DISABLE_ENABLE
/* oam set link-fault monitor state ( disable | enable ) */
cparser_result_t
cparser_cmd_oam_set_link_fault_monitor_state_disable_enable(cparser_context_t *context)
{
    uint32          unit;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    ret = rtk_oam_linkFaultMonEnable_set(0, enable);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_OAM_GET_PDU_SA_LEARN
/*
 * oam get pdu sa-learn
 */
cparser_result_t
cparser_cmd_oam_get_pdu_sa_learn(
    cparser_context_t *context)
{
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_oam_pduLearningEnable_get(unit, &enable), ret);

    diag_util_printf("OAM PDU SA learning: ");
    if(ENABLED == enable)
        diag_util_printf("%s\n",DIAG_STR_ENABLE);
    else
        diag_util_printf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_pdu_sa_learn */
#endif

#ifdef CMD_OAM_SET_PDU_SA_LEARN_DISABLE_ENABLE
/*
 * oam set pdu sa-learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_oam_set_pdu_sa_learn_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_oam_pduLearningEnable_set(unit, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_set_pdu_sa_learn_disable_enable */
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_MEPID
/*
 * oam get cfm ccm <UINT:instance> mepid
 */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_mepid(
    cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32  unit;
    uint32  mepid;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMepid_get(unit, *instance_ptr, &mepid), ret);

    diag_util_mprintf("\tCFM CCM intance %d mepid: %d\n", *instance_ptr, mepid);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_cfm_ccm_instance_mepid */
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_MEPID_MEPID
/*
 * oam set cfm ccm <UINT:instance> mepid <UINT:mepid>
 */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_mepid_mepid(
    cparser_context_t *context,
    uint32_t *instance_ptr,
    uint32_t *mepid_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMepid_set(unit, *instance_ptr, *mepid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_set_cfm_ccm_instance_mepid_mepid */
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_TX_INTERVAL
/*
 * oam get cfm ccm <UINT:instance> tx interval
 */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_tx_interval(
    cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32  unit;
    uint32  interval;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxIntervalField_get(unit, *instance_ptr,
            &interval), ret);

    diag_util_mprintf("\tCFM CCM intance %d interval: %d\n", *instance_ptr, interval);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_cfm_ccm_instance_tx_interval */
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_TX_INTERVAL_INTERVAL
/*
 * oam set cfm ccm <UINT:instance> tx interval <UINT:interval>
 */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_tx_interval_interval(
    cparser_context_t *context,
    uint32_t *instance_ptr,
    uint32_t *interval_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxIntervalField_set(unit, *instance_ptr,
            *interval_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_set_cfm_ccm_instance_tx_interval_interval */
#endif

#ifdef CMD_OAM_GET_CFM_CCM_INSTANCE_MDL
/*
 * oam get cfm ccm <UINT:instance> mdl
 */
cparser_result_t
cparser_cmd_oam_get_cfm_ccm_instance_mdl(
    cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32  unit;
    uint32  mdl;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMdl_get(unit, *instance_ptr,
            &mdl), ret);

    diag_util_mprintf("\tCFM CCM intance %d mdl: %d\n", *instance_ptr, mdl);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_get_cfm_ccm_instance_mdl */
#endif

#ifdef CMD_OAM_SET_CFM_CCM_INSTANCE_MDL_MDL
/*
 * oam set cfm ccm <UINT:instance> mdl <UINT:mdl>
 */
cparser_result_t
cparser_cmd_oam_set_cfm_ccm_instance_mdl_mdl(
    cparser_context_t *context,
    uint32_t *instance_ptr,
    uint32_t *mdl_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_oam_cfmCcmInstTxMdl_set(unit, *instance_ptr,
            *mdl_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_oam_set_cfm_ccm_instance_mdl_mdl */
#endif


