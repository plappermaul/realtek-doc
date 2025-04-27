/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * Purpose : Define diag shell commands for flow control
 *
 * Feature : The file includes the following module and sub-modules
 *           1) flow control commands.
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/flowctrl.h>
#include <rtk/qos.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_flowctrl.h>
  #include <rtrpc/rtrpc_qos.h>
#endif

#ifdef CMD_FLOWCTRL_GET_JUMBO_MODE_STATUS
/*
 * flowctrl get jumbo-mode status
 */
cparser_result_t cparser_cmd_flowctrl_get_jumbo_mode_status(cparser_context_t *context)
{
    uint32 unit, status;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_jumboModeStatus_get(unit, &status), ret);
    diag_util_mprintf("Flow Control Jumbo Mode Status : %s\n", status ? "Jumbo Mode" : "Normal Mode");

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_JUMBO_MODE_STATE
/*
 * flowctrl get jumbo-mode state
 */
cparser_result_t cparser_cmd_flowctrl_get_jumbo_mode_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_jumboModeEnable_get(unit, &enable), ret);
    diag_util_mprintf("Flow Control Jumbo Mode State : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_JUMBO_MODE_LENGTH
/*
 * flowctrl get jumbo-mode length
 */
cparser_result_t cparser_cmd_flowctrl_get_jumbo_mode_length(cparser_context_t *context)
{
    uint32 unit, length;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_jumboModeLength_get(unit, &length), ret);
    diag_util_mprintf("Flow Control Jumbo Mode Packet Length Boundary (byte) : %d\n", length);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_SYSTEM_GUARANTEE_STATE
/*
 * flowctrl get ingress system guarantee state
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_system_guarantee_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrGuarEnable_get(unit, &enable), ret);
    diag_util_mprintf("Flow Control Ingress Guarantee Page for All Ports Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_SYSTEM_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD
/*
 * flowctrl get ingress system ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_system_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold(cparser_context_t *context)
{
    uint32 unit, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(4,4))
    {
        case 'f':
            fcon = 0;
            break;

        case 'n':
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemPauseThresh_get(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemCongestThresh_get(unit, &thresh), ret);

    diag_util_printf("Flow Control %s\n", fcon ? "On" : "Off");

    switch(TOKEN_CHAR(5,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(5,7))
                diag_util_printf("\tHigh Off Threshold : 0x%X (%u)\n", thresh.highOff, thresh.highOff);
            else
                diag_util_printf("\tHigh On Threshold  : 0x%X (%u)\n", thresh.highOn, thresh.highOn);
            break;

        case 'l':
            if('f' == TOKEN_CHAR(5,5))
                diag_util_printf("\tLow Off Threshold  : 0x%X (%u)\n", thresh.lowOff, thresh.lowOff);
            else
                diag_util_printf("\tLow On Threshold   : 0x%X (%u)\n", thresh.lowOn, thresh.lowOn);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_SYSTEM_JUMBO_MODE_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD
/*
 * flowctrl get ingress system jumbo-mode ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_system_jumbo_mode_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold(cparser_context_t *context)
{
    uint32 unit, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(5,4))
    {
        case 'f':
            fcon = 0;
            break;

        case 'n':
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemPauseThresh_get(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemCongestThresh_get(unit, &thresh), ret);

    diag_util_printf("Jumbo Mode Flow Control %s\n", fcon ? "On" : "Off");

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                diag_util_printf("\tHigh Off Threshold : 0x%X (%u)\n", thresh.highOff, thresh.highOff);
            else
                diag_util_printf("\tHigh On Threshold  : 0x%X (%u)\n", thresh.highOn, thresh.highOn);
            break;

        case 'l':
            if('f' == TOKEN_CHAR(6,5))
                diag_util_printf("\tLow Off Threshold  : 0x%X (%u)\n", thresh.lowOff, thresh.lowOff);
            else
                diag_util_printf("\tLow On Threshold   : 0x%X (%u)\n", thresh.lowOn, thresh.lowOn);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_THRESHOLD_GROUP
/*
 * flowctrl get ingress port ( <PORT_LIST:ports> | all ) threshold-group
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_threshold_group(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint32 grp_idx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Flow Control Threshold Group of Port Configuration\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portIgrPortThreshGroupSel_get(unit, port, &grp_idx), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, grp_idx);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ACTION
/*
 * flowctrl get ingress port ( <PORT_LIST:ports> | all ) pause-on-action
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_pause_on_action(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_flowctrl_pauseOnAction_t act;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Flow Control Port Pause On Action\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portPauseOnAction_get(unit, port, &act), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, act ? "DROP" : "RECEIVE");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_NUM_PKT
/*
 * flowctrl get ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-num pkt
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_pause_on_allowed_num_pkt(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port, num;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Flow Control Port Pause On Allowed Packet Number\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_pauseOnAllowedPktNum_get(unit, port, &num), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, num);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_LEN
/*
 * flowctrl get ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-len
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_pause_on_allowed_len(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port, pktLen;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Flow Control Port Pause On Allowed Total Packet Length\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_pauseOnAllowedPktLen_get(unit, port, &pktLen), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, pktLen);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_THRESHOLD_GROUP_INDEX_GUARANTEE_THRESHOLD
/*
 * flowctrl get ingress threshold-group <UINT:index> guarantee threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_threshold_group_index_guarantee_threshold(cparser_context_t *context,
    uint32_t *index_ptr)

{
    uint32 unit, grp_idx;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);

    diag_util_mprintf("Group %d Ingress Flow Control Guarantee Threshold : 0x%X (%u)\n", grp_idx, thresh.guar, thresh.guar);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_THRESHOLD_GROUP_INDEX_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_THRESHOLD
/*
 * flowctrl get ingress threshold-group <UINT:index> ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low ) threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_threshold_group_index_fc_off_threshold_fc_on_threshold_high_off_high_on_low_threshold(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;

    switch(TOKEN_CHAR(5,4))
    {
        case 'f': /* flow control OFF */
            fcon = 0;
            break;

        case 'n': /* flow control ON */
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_get(unit, grp_idx, &thresh), ret);

    diag_util_mprintf("Group %d Ingress Flow Control %s Threshold\n", grp_idx, fcon ? "On" : "Off");
    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                diag_util_printf("\tHigh Off Threshold : 0x%X (%u)\n", thresh.highOff, thresh.highOff);
            else
                diag_util_printf("\tHigh On Threshold : 0x%X (%u)\n", thresh.highOn, thresh.highOn);
            break;

        case 'l':
                diag_util_mprintf("\tLow Threshold   : 0x%X (%d)\n", thresh.lowOn, thresh.lowOn);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_QUEUE_QUEUE_ID_DROP_THRESHOLD_HIGH_LOW
/*
 * flowctrl get ingress queue <UINT:queue_id> drop-threshold ( high | low )
 */
cparser_result_t cparser_cmd_flowctrl_get_ingress_queue_queue_id_drop_threshold_high_low(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    queue_id = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_get(unit, queue_id, &thresh), ret);

    diag_util_mprintf("Ingress Queue %d Drop Threshold\n", queue_id);
    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            diag_util_printf("\tHigh Threshold : 0x%X (%u)\n", thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tLow Threshold : 0x%X (%u)\n", thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_SYSTEM_UTIL_THRESHOLD
/*
 * flowctrl get system utilization-threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_system_utilization_threshold(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrSystemUtilThresh_get(unit, &thresh), ret);

    diag_util_printf("Egress Flow Control System Utilization Threshold : 0x%X (%u)\n", thresh.highOn, thresh.highOn);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_SYSTEM_DROP_THRESHOLD
/*
 * flowctrl get system drop-threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_system_drop_threshold(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrSystemDropThresh_get(unit, &thresh), ret);

    diag_util_printf("Egress Flow Control System Drop Threshold : 0x%X (%u)\n", thresh.high, thresh.high);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_DROP_THRESHOLD
/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) drop-threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_drop_threshold(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Egress Flow Control Port Drop Threshold\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropThresh_get(unit, port, &thresh), ret);
        diag_util_printf("\tPort %2u Threshold : 0x%X (%u)\n", port, thresh.high, thresh.high);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_QUEUE_DROP_STATE
/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) queue-drop state
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_queue_drop_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Flow Control Egress Queue Drop of Port Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortQueueDropEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_IGR_CONGEST_CHECK
/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) igr-congest-check
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_igr_congest_check(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Egress Flow Control Ignore RX Port Congestion Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropRefCongestEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_REF_RX_CONGEST
/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) refer-rx-congest
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_refer_rx_congest(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Egress Flow Control Refer RX Port Congestion Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropRefCongestEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_QUEUE_ID_QID_ALL_DROP_THRESHOLD_HIGH_LOW
/*
 * flowctrl get egress queue-id ( <MASK_LIST:qid> | all ) drop-threshold ( high | low )
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_queue_id_qid_all_drop_threshold_high_low(cparser_context_t *context,
    char **qid_ptr)
{
    uint32 unit, queue;
    int32  ret = RT_ERR_FAILED;
    diag_mask_t queueMask;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Egress Flow Control Queue Drop Threshold\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 4), ret);
    DIAG_UTIL_MASK_SCAN(queueMask, queue)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThresh_get(unit, queue, &thresh), ret);

        switch(TOKEN_CHAR(6,0))
        {
            case 'h':
                diag_util_printf("\tQueue %u High Threshold : 0x%X (%u)\n", queue, thresh.high, thresh.high);
                break;

            case 'l':
                diag_util_printf("\tQueue %u Low Threshold : 0x%X (%u)\n", queue, thresh.low, thresh.low);
                break;

            default:
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_FORCE_DROP_STATE
/*
 * flowctrl get egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> force-drop state
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_queue_queue_id_force_drop_state(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    queue_id = *queue_id_ptr;

    diag_util_mprintf("Flow Control Egress Queue Force Drop Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrQueueDropForceEnable_get(unit, port, queue_id, &enable), ret);
        diag_util_mprintf("\tPort %2d Queue %2d: %s\n", port, queue_id, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_FLOWCTRL_GET_EGRESS_CPU_QUEUE_ID_QID_DROP_THRESHOLD_HIGH_LOW
/*
 * flowctrl get egress cpu queue-id <UINT:qid> drop-threshold ( high | low )
 */
cparser_result_t cparser_cmd_flowctrl_get_egress_cpu_queue_id_qid_drop_threshold_high_low(cparser_context_t *context,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("CPU Port - Egress Flow Control Queue Drop Threshold\n");

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrCpuQueueDropThresh_get(unit, *qid_ptr, &thresh), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            diag_util_printf("\tQueue %u High Threshold : 0x%X (%u)\n", *qid_ptr, thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tQueue %u Low Threshold : 0x%X (%u)\n", *qid_ptr, thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_JUMBO_MODE_STATE_DISABLE_ENABLE
/*
 * flowctrl set jumbo-mode state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_jumbo_mode_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

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

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_jumboModeEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_JUMBO_MODE_LENGTH_LENGTH
/*
 * flowctrl set jumbo-mode length <UINT:length>
 */
cparser_result_t cparser_cmd_flowctrl_set_jumbo_mode_length_length(cparser_context_t *context,
    uint32_t *length_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_jumboModeLength_set(unit, *length_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_SYSTEM_GUARANTEE_STATE_DISABLE_ENABLE
/*
 * flowctrl set ingress system guarantee state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_system_guarantee_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

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

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrGuarEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_SYSTEM_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD_THRESHOLD
/*
 * flowctrl set ingress system ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_system_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,4))
    {
        case 'f':
            fcon = 0;
            break;

        case 'n':
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemPauseThresh_get(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemCongestThresh_get(unit, &thresh), ret);

    switch(TOKEN_CHAR(5,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(5,7))
                thresh.highOff = *threshold_ptr;
            else
                thresh.highOn = *threshold_ptr;
            break;

        case 'l':
            if('f' == TOKEN_CHAR(5,5))
                thresh.lowOff = *threshold_ptr;
            else
                thresh.lowOn = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemPauseThresh_set(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrSystemCongestThresh_set(unit, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_SYSTEM_JUMBO_MODE_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD_THRESHOLD
/*
 * flowctrl set ingress system jumbo-mode ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_system_jumbo_mode_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(5,4))
    {
        case 'f':
            fcon = 0;
            break;

        case 'n':
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemPauseThresh_get(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemCongestThresh_get(unit, &thresh), ret);

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                thresh.highOff = *threshold_ptr;
            else
                thresh.highOn = *threshold_ptr;
            break;

        case 'l':
            if('f' == TOKEN_CHAR(6,5))
                thresh.lowOff = *threshold_ptr;
            else
                thresh.lowOn = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemPauseThresh_set(unit, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrJumboSystemCongestThresh_set(unit, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_THRESHOLD_GROUP_INDEX
/*
 * flowctrl set ingress port ( <PORT_LIST:ports> | all ) threshold-group <UINT:index>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_threshold_group_index(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portIgrPortThreshGroupSel_set(unit, port, *index_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ACTION_DROP_RECEIVE
/*
 * flowctrl set ingress port ( <PORT_LIST:ports> | all ) pause-on-action ( drop | receive )
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_pause_on_action_drop_receive(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_flowctrl_pauseOnAction_t act;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'r':
            act = PAUSE_ON_RECEIVE;
            break;

        case 'd':
            act = PAUSE_ON_DROP;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portPauseOnAction_set(unit, port, act), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_THRESHOLD_GROUP_INDEX_GUARANTEE_THRESHOLD_THRESHOLD
/*
 * flowctrl set ingress threshold-group <UINT:index> guarantee threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_threshold_group_index_guarantee_threshold_threshold(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    grp_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);
    thresh.guar = *threshold_ptr;
    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_set(unit, grp_idx, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_THRESHOLD_GROUP_INDEX_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_THRESHOLD_THRESHOLD
/*
 * flowctrl set ingress threshold-group <UINT:index> ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_threshold_group_index_fc_off_threshold_fc_on_threshold_high_off_high_on_low_threshold_threshold(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    grp_idx = *index_ptr;

    switch(TOKEN_CHAR(5,4))
    {
        case 'f': /* flow control OFF */
            fcon = 0;
            break;

        case 'n': /* flow control ON */
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_get(unit, grp_idx, &thresh), ret);

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                thresh.highOff = *threshold_ptr;
            else
                thresh.highOn = *threshold_ptr;
            break;

        case 'l':
            thresh.lowOn = *threshold_ptr;
            thresh.lowOff = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_set(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_set(unit, grp_idx, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_NUM_PKT_NUMBER
/*
 * flowctrl set ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-num pkt <UINT:number>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_pause_on_allowed_num_pkt_number(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *number_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_pauseOnAllowedPktNum_set(unit, port, *number_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_LEN_LENGTH
/*
 * flowctrl set ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-len <UINT:length>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_pause_on_allowed_len_length(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *length_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_pauseOnAllowedPktLen_set(unit, port, *length_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_QUEUE_QUEUE_ID_DROP_THRESHOLD_HIGH_LOW_THRESHOLD
/*
 * flowctrl set ingress queue <UINT:queue_id> drop-threshold ( high | low ) <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_ingress_queue_queue_id_drop_threshold_high_low_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    queue_id = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_get(unit, queue_id, &thresh), ret);

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_set(unit, queue_id, &thresh), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_FLOWCTRL_SET_SYSTEM_UTIL_THRESHOLD_THRESHOLD
/*
 * flowctrl set system utilization-threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_system_utilization_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    thresh.highOn = *threshold_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrSystemUtilThresh_set(unit, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_SYSTEM_DROP_THRESHOLD_THRESHOLD
/*
 * flowctrl set system drop-threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_system_drop_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    thresh.high = *threshold_ptr;
    thresh.low = *threshold_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrSystemDropThresh_set(unit, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_DROP_THRESHOLD_THRESHOLD
/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) drop-threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_drop_threshold_threshold(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        thresh.high = *threshold_ptr;
        thresh.low = *threshold_ptr;
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropThresh_set(unit, port, &thresh), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_QUEUE_DROP_STATE_DISABLE_ENABLE
/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) queue-drop state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_queue_drop_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(7,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortQueueDropEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_IGR_CONGEST_CHECK_STATE_DISABLE_ENABLE
/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) igr-congest-check state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_igr_congest_check_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(7,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropRefCongestEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_REF_RX_CONGEST_STATE_DISABLE_ENABLE
/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) refer-rx-congest state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_refer_rx_congest_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(7,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropRefCongestEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_QUEUE_ID_QID_ALL_DROP_THRESHOLD_HIGH_LOW_THRESHOLD
/*
 * flowctrl set egress queue-id ( <MASK_LIST:qid> | all ) drop-threshold ( high | low ) <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_queue_id_qid_all_drop_threshold_high_low_threshold(cparser_context_t *context,
    char **qid_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, queue;
    int32  ret = RT_ERR_FAILED;
    diag_mask_t queueMask;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 4), ret);
    DIAG_UTIL_MASK_SCAN(queueMask, queue)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThresh_get(unit, queue, &thresh), ret);

        switch(TOKEN_CHAR(6,0))
        {
            case 'h':
                thresh.high = *threshold_ptr;
                break;

            case 'l':
                thresh.low = *threshold_ptr;
                break;

            default:
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThresh_set(unit, queue, &thresh), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_FORCE_DROP_STATE_DISABLE_ENABLE
/*
 * flowctrl set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> force-drop state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_queue_queue_id_force_drop_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    queue_id = *queue_id_ptr;

    switch(TOKEN_CHAR(9,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrQueueDropForceEnable_set(unit, port, queue_id, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_CPU_QUEUE_ID_QID_DROP_THRESHOLD_HIGH_LOW_THRESHOLD
/*
 * flowctrl set egress cpu queue-id <UINT:qid> drop-threshold ( high | low ) <UINT:threshold>
 */
 cparser_result_t cparser_cmd_flowctrl_set_egress_cpu_queue_id_qid_drop_threshold_high_low_threshold(cparser_context_t *context,
    uint32_t *qid_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrCpuQueueDropThresh_get(unit, *qid_ptr, &thresh), ret);
    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrCpuQueueDropThresh_set(unit, *qid_ptr, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_NUM_PAGE
/*
* flowctrl get ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-num page
*/
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_pause_on_allowed_num_page(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port, num;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Flow Control Port Pause On Allowed Page Number\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portPauseOnAllowedPageNum_get(unit, port, &num), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, num);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_PAUSE_ON_ALLOWED_NUM_PAGE_NUMBER
/*
* flowctrl set ingress port ( <PORT_LIST:ports> | all ) pause-on-allowed-num page <UINT:number>
*/
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_pause_on_allowed_num_page_number(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *number_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portPauseOnAllowedPageNum_set(unit, port, *number_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_THRESHOLD_GROUP_INDEX_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD
/*
* flowctrl get ingress threshold-group <UINT:index> ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold
*/
cparser_result_t cparser_cmd_flowctrl_get_ingress_threshold_group_index_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;

    switch(TOKEN_CHAR(5,4))
    {
        case 'f': /* flow control OFF */
            fcon = 0;
            break;

        case 'n': /* flow control ON */
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if (fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_get(unit, grp_idx, &thresh), ret);

    diag_util_mprintf("Group %d Ingress Flow Control %s Threshold\n", grp_idx, fcon ? "On" : "Off");
    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                diag_util_printf("\tHigh Off Threshold : 0x%X (%u)\n", thresh.highOff, thresh.highOff);
            else
                diag_util_printf("\tHigh On Threshold : 0x%X (%u)\n", thresh.highOn, thresh.highOn);
            break;

        case 'l':
            if('f' == TOKEN_CHAR(6,6))
                diag_util_printf("\tLow Off Threshold : 0x%X (%u)\n", thresh.lowOff, thresh.lowOff);
            else
                diag_util_printf("\tLow On Threshold : 0x%X (%u)\n", thresh.lowOn, thresh.lowOn);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_THRESHOLD_GROUP_INDEX_FC_OFF_THRESHOLD_FC_ON_THRESHOLD_HIGH_OFF_HIGH_ON_LOW_OFF_LOW_ON_THRESHOLD_THRESHOLD
/*
* flowctrl set ingress threshold-group <UINT:index> ( fc-off-threshold | fc-on-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
*/
cparser_result_t cparser_cmd_flowctrl_set_ingress_threshold_group_index_fc_off_threshold_fc_on_threshold_high_off_high_on_low_off_low_on_threshold_threshold(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx, fcon;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    grp_idx = *index_ptr;

    switch(TOKEN_CHAR(5,4))
    {
        case 'f': /* flow control OFF */
            fcon = 0;
            break;

        case 'n': /* flow control ON */
            fcon = 1;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_get(unit, grp_idx, &thresh), ret);

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            if('f' == TOKEN_CHAR(6,7))
                thresh.highOff = *threshold_ptr;
            else
                thresh.highOn = *threshold_ptr;
            break;

        case 'l':
            if('f' == TOKEN_CHAR(6,6))
                thresh.lowOff = *threshold_ptr;
            else
                thresh.lowOn = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    if(fcon)
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_set(unit, grp_idx, &thresh), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrCongestThreshGroup_set(unit, grp_idx, &thresh), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_FLOWCTRL_GET_INGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_FC_THRESHOLD
/*
* flowctrl get ingress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) fc-threshold
*/
cparser_result_t cparser_cmd_flowctrl_get_ingress_queue_queue_id_threshold_group_index_high_low_fc_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueuePauseThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    diag_util_mprintf("Group %d Ingress Queue %d Flow Control Threshold\n", grp_idx, queue_id);
    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            diag_util_printf("\tHigh Threshold : 0x%X (%u)\n", thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tLow Threshold : 0x%X (%u)\n", thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_FC_THRESHOLD_THRESHOLD
/*
* flowctrl set ingress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) fc-threshold <UINT:threshold>
*/
cparser_result_t cparser_cmd_flowctrl_set_ingress_queue_queue_id_threshold_group_index_high_low_fc_threshold_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;


    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueuePauseThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueuePauseThreshGroup_set(unit, grp_idx, queue_id, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD
/*
* flowctrl get ingress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) drop-threshold
*/
cparser_result_t cparser_cmd_flowctrl_get_ingress_queue_queue_id_threshold_group_index_high_low_drop_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    diag_util_mprintf("Group %d Ingress Queue %d Drop Threshold\n", grp_idx, queue_id);
    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            diag_util_printf("\tHigh Threshold : 0x%X (%u)\n", thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tLow Threshold : 0x%X (%u)\n", thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD_THRESHOLD
/*
* flowctrl set ingress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) drop-threshold <UINT:threshold>
*/
cparser_result_t cparser_cmd_flowctrl_set_ingress_queue_queue_id_threshold_group_index_high_low_drop_threshold_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;


    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThreshGroup_set(unit, grp_idx, queue_id, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_IGR_QUEUE_PORT_PORTS_ALL_THRESHOLD_GROUP
/*
* flowctrl get igr-queue port ( <PORT_LIST:ports> | all ) threshold-group
*/
cparser_result_t cparser_cmd_flowctrl_get_igr_queue_port_ports_all_threshold_group(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint32 grp_idx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ingress Queue Flow Control & Drop Threshold Group of Port Configuration\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get(unit, port, &grp_idx), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, grp_idx);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_IGR_QUEUE_PORT_PORTS_ALL_THRESHOLD_GROUP_INDEX
/*
*flowctrl set igr-queue port ( <PORT_LIST:ports> | all ) threshold-group <UINT:index>
*/
cparser_result_t cparser_cmd_flowctrl_set_igr_queue_port_ports_all_threshold_group_index(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set(unit, port, *index_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD
/*
* flowctrl get egress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) drop-threshold
*/
cparser_result_t cparser_cmd_flowctrl_get_egress_queue_queue_id_threshold_group_index_high_low_drop_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    diag_util_mprintf("Group %d Egress Queue %d Drop Threshold\n", grp_idx, queue_id);
    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            diag_util_printf("\tHigh Threshold : 0x%X (%u)\n", thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tLow Threshold : 0x%X (%u)\n", thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_QUEUE_QUEUE_ID_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD_THRESHOLD
/*
* flowctrl set egress queue <UINT:queue_id> threshold-group <UINT:index> ( high | low ) drop-threshold <UINT:threshold>
*/
cparser_result_t cparser_cmd_flowctrl_set_egress_queue_queue_id_threshold_group_index_high_low_drop_threshold_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx, queue_id;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;
    queue_id = *queue_id_ptr;


    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThreshGroup_get(unit, grp_idx, queue_id, &thresh), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropThreshGroup_set(unit, grp_idx, queue_id, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD
/*
* flowctrl get egress port threshold-group <UINT:index> ( high | low ) drop-threshold
*/
cparser_result_t cparser_cmd_flowctrl_get_egress_port_threshold_group_index_high_low_drop_threshold(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit, grp_idx;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropThreshGroup_get(unit, grp_idx, &thresh), ret);

    diag_util_mprintf("Group %d Egress Port Drop Threshold\n", grp_idx);
    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            diag_util_printf("\tHigh Threshold : 0x%X (%u)\n", thresh.high, thresh.high);
            break;

        case 'l':
            diag_util_printf("\tLow Threshold : 0x%X (%u)\n", thresh.low, thresh.low);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_THRESHOLD_GROUP_INDEX_HIGH_LOW_DROP_THRESHOLD_THRESHOLD
/*
* flowctrl set egress port threshold-group <UINT:index> ( high | low ) drop-threshold <UINT:threshold>
*/
cparser_result_t cparser_cmd_flowctrl_set_egress_port_threshold_group_index_high_low_drop_threshold_threshold(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit, grp_idx;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    grp_idx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropThreshGroup_get(unit, grp_idx, &thresh), ret);

    switch(TOKEN_CHAR(6,0))
    {
        case 'h':
            thresh.high = *threshold_ptr;
            break;

        case 'l':
            thresh.low = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropThreshGroup_set(unit, grp_idx, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGR_PORT_QUEUE_PORT_PORTS_ALL_THRESHOLD_GROUP
/*
*flowctrl get egr-port-queue port ( <PORT_LIST:ports> | all ) threshold-group
*/
cparser_result_t cparser_cmd_flowctrl_get_egr_port_queue_port_ports_all_threshold_group(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint32 grp_idx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Egress Queue & Port Drop Threshold Group of Port Configuration\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropThreshGroupSel_get(unit, port, &grp_idx), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, grp_idx);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGR_PORT_QUEUE_PORT_PORTS_ALL_THRESHOLD_GROUP_INDEX
/*
*flowctrl set egr-port-queue port ( <PORT_LIST:ports> | all ) threshold-group <UINT:index>
*/
cparser_result_t cparser_cmd_flowctrl_set_egr_port_queue_port_ports_all_threshold_group_index(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portEgrDropThreshGroupSel_set(unit, port, *index_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_DROP_STATE
/*
* flowctrl get egress port ( <PORT_LIST:ports> | all ) drop state
*/
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_drop_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Flow Control Egress Port Drop Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_DROP_STATE_DISABLE_ENABLE
/*
* flowctrl set egress port ( <PORT_LIST:ports> | all ) drop state ( disable | enable )
*/
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_drop_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(7,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrPortDropEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_DROP_STATE
/*
* flowctrl get egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> drop state
*/
cparser_result_t cparser_cmd_flowctrl_get_egress_port_ports_all_queue_queue_id_drop_state(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    queue_id = *queue_id_ptr;

    diag_util_mprintf("Flow Control Egress Queue Drop Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropEnable_get(unit, port, queue_id, &enable), ret);
        diag_util_mprintf("\tPort %2d Queue %2d: %s\n", port, queue_id, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_DROP_STATE_DISABLE_ENABLE
/*
* flowctrl set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> drop state ( disable | enable )
*/
cparser_result_t cparser_cmd_flowctrl_set_egress_port_ports_all_queue_queue_id_drop_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    queue_id = *queue_id_ptr;

    switch(TOKEN_CHAR(9,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrQueueDropEnable_set(unit, port, queue_id, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_DROP_STATE
/*
*flowctrl get egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> drop state
*/
cparser_result_t cparser_cmd_flowctrl_get_ingress_port_ports_all_queue_queue_id_drop_state(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    queue_id = *queue_id_ptr;

    diag_util_mprintf("Flow Control Ingress Queue Drop Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropEnable_get(unit, port, queue_id, &enable), ret);
        diag_util_mprintf("\tPort %2d Queue %2d: %s\n", port, queue_id, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_DROP_STATE_DISABLE_ENABLE
/*
*flowctrl set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> drop state ( disable | enable )
*/
cparser_result_t cparser_cmd_flowctrl_set_ingress_port_ports_all_queue_queue_id_drop_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32 unit, port, queue_id;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    queue_id = *queue_id_ptr;

    switch(TOKEN_CHAR(9,0))
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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropEnable_set(unit, port, queue_id, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_ALGORITHM_SWRED
/*
 * flowctrl set congest-avoidance algorithm swred
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_algorithm_swred(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_qos_congAvoidAlgo_t algo;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    algo = CONG_AVOID_SWRED;

    if ((ret = rtk_qos_congAvoidAlgo_set(unit, algo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_ALGORITHM_SRED
/*
*flowctrl set congest-avoidance algorithm sred
*/
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_algorithm_sred(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_qos_congAvoidAlgo_t algo;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    algo = CONG_AVOID_SRED;

    if ((ret = rtk_qos_congAvoidAlgo_set(unit, algo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_ALGORITHM_TD
/*
 * flowctrl set congest-avoidance algorithm td
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_algorithm_td(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_qos_congAvoidAlgo_t algo;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    algo = CONG_AVOID_TD;

    if ((ret = rtk_qos_congAvoidAlgo_set(unit, algo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_CONGEST_AVOIDANCE
/*
 * flowctrl get congest-avoidance
 */
cparser_result_t cparser_cmd_flowctrl_get_congest_avoidance(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_qos_congAvoidAlgo_t algorithm;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Congestion Avoidance Configuration\n");
    if ((ret = rtk_qos_congAvoidAlgo_get(unit, &algorithm)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("\tCongestion avoidance algorithm : ");
    if (CONG_AVOID_SRED == algorithm)
    {
        diag_util_mprintf("SRED\n");
    }
    else if (CONG_AVOID_SWRED == algorithm)
    {
        diag_util_mprintf("SWRED\n");
    }
    else if (CONG_AVOID_TD == algorithm)
    {
        diag_util_mprintf("TD\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_CONGEST_AVOIDANCE_QUEUE_THRESHOLD_QUEUE_ID_QID_ALL
/*
 * flowctrl get congest-avoidance queue-threshold queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t cparser_cmd_flowctrl_get_congest_avoidance_queue_threshold_queue_id_qid_all(cparser_context_t *context,
    char **qid_ptr)
{
    uint32      unit = 0, queue;
    int32       ret = RT_ERR_FAILED;
    uint32      dp;
    diag_mask_t queueMask;
    rtk_qos_congAvoidThresh_t   threshold;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    uint32      probability;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 5), ret);

    DIAG_UTIL_MASK_SCAN(queueMask, queue)
    {
        diag_util_mprintf("Queue %u Congestion Avoidance Threshold\n", queue);

        for (dp = 0; dp <= RTK_DROP_PRECEDENCE_MAX; dp++)
        {
            diag_util_mprintf("Drop precedence %u\n", dp);
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                if (RT_ERR_OK != rtk_qos_congAvoidGlobalQueueThresh_get(unit, queue, dp, &threshold))
                {
                    diag_util_mprintf("Not support\n");
                    continue;
                }

                if (RT_ERR_OK != rtk_qos_congAvoidGlobalQueueDropProbability_get(unit, queue, dp, &probability))
                {
                    diag_util_mprintf("Not support\n");
                    continue;
                }

            }
            else
#endif
            {
                if (RT_ERR_OK != (ret=rtk_qos_congAvoidGlobalQueueConfig_get(unit, queue, dp, &threshold)))
                {
                    diag_util_mprintf("Not support, ret=0x%x\n", ret);
                    continue;
                }
            }
            diag_util_mprintf("\tMax threshold : %u\n", threshold.maxThresh);
            diag_util_mprintf("\tMin threshold : %u\n", threshold.minThresh);
            diag_util_mprintf("\tDrop Probability : %u\n", threshold.probability);
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_CONGEST_AVOIDANCE_PORT_PORTS_ALL_ALGORITHM
/*
 * flowctrl get congest-avoidance port ( <PORT_LIST:ports> | all ) algorithm
 */
cparser_result_t cparser_cmd_flowctrl_get_congest_avoidance_port_ports_all_algorithm(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0, port;
    rtk_qos_congAvoidAlgo_t algorithm;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Congestion Avoidance Algorithm of Port: \n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_qos_portCongAvoidAlgo_get(unit, port, &algorithm)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if (CONG_AVOID_SWRED == algorithm)
        {
            diag_util_mprintf("Port %2d: SWRED\n", port);
        }
        else if (CONG_AVOID_TD == algorithm)
        {
            diag_util_mprintf("Port %2d: TD\n", port);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_PORT_PORTS_ALL_ALGORITHM_SWRED
/*
 * flowctrl set congest-avoidance port ( <PORT_LIST:ports> | all ) algorithm swred
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_port_ports_all_algorithm_swred(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0, port;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portCongAvoidAlgo_set(unit, port, CONG_AVOID_SWRED), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_PORT_PORTS_ALL_ALGORITHM_TD
/*
 * flowctrl set congest-avoidance port ( <PORT_LIST:ports> | all ) algorithm td
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_port_ports_all_algorithm_td(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0, port;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_qos_portCongAvoidAlgo_set(unit, port, CONG_AVOID_TD), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_CONGEST_AVOIDANCE_SYSTEM_THRESHOLD
/*
 * flowctrl get congest-avoidance system-threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_congest_avoidance_system_threshold(cparser_context_t *context)
{
    int32       ret;
    uint32      unit = 0, probability;
    uint32      dp;
    rtk_qos_congAvoidThresh_t   threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System Congestion Avoidance Threshold\n");

    for (dp = 0; dp <= RTK_DROP_PRECEDENCE_MAX; dp++)
    {
        diag_util_mprintf("  Drop precedence %u\n", dp);
        DIAG_UTIL_ERR_CHK(rtk_qos_congAvoidSysThresh_get(unit, dp, &threshold), ret);

        diag_util_mprintf("\tMax threshold : %u\n", threshold.maxThresh);
        diag_util_mprintf("\tMin threshold : %u\n", threshold.minThresh);

        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_qos_congAvoidSysDropProbability_get(unit, dp, &probability), ret);
            diag_util_mprintf("\tDrop Probability : %u\n", probability);
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_QUEUE_THRESHOLD_QUEUE_ID_QID_ALL_DROP_PRECEDENCE_DP_MAX_THRESHOLD_MAX_THRESHOLD_MIN_THRESHOLD_MIN_THRESHOLD
/*
 * flowctrl set congest-avoidance queue-threshold queue-id ( <MASK_LIST:qid> | all ) drop-precedence <UINT:dp> max-threshold <UINT:max_threshold> min-threshold <UINT:min_threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_queue_threshold_queue_id_qid_all_drop_precedence_dp_max_threshold_max_threshold_min_threshold_min_threshold(cparser_context_t *context,
    char **qid_ptr,
    uint32_t *dp_ptr,
    uint32_t *max_threshold_ptr,
    uint32_t *min_threshold_ptr)
{
    uint32      unit = 0, queue;
    int32       ret = RT_ERR_FAILED;
    diag_mask_t queueMask;
    rtk_qos_congAvoidThresh_t   thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 5), ret);

    DIAG_UTIL_MASK_SCAN(queueMask, queue)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            thresh.maxThresh = *max_threshold_ptr;
            thresh.minThresh = *min_threshold_ptr;

            if ((ret = rtk_qos_congAvoidGlobalQueueThresh_set(unit, queue, *dp_ptr, &thresh)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_qos_congAvoidGlobalQueueConfig_get(unit, queue, *dp_ptr, &thresh)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            thresh.maxThresh = *max_threshold_ptr;
            thresh.minThresh = *min_threshold_ptr;

            if ((ret = rtk_qos_congAvoidGlobalQueueConfig_set(unit, queue, *dp_ptr, &thresh)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_QUEUE_THRESHOLD_QUEUE_ID_QID_ALL_DROP_PRECEDENCE_DP_DROP_PROBABILITY_PROBABILITY
/*
 * flowctrl set congest-avoidance queue-threshold queue-id ( <MASK_LIST:qid> | all ) drop-precedence <UINT:dp> drop-probability <UINT:probability>
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_queue_threshold_queue_id_qid_all_drop_precedence_dp_drop_probability_probability(cparser_context_t *context,
    char **qid_ptr,
    uint32_t *dp_ptr,
    uint32_t *probability_ptr)
{
    uint32      unit = 0, queue;
    int32       ret = RT_ERR_FAILED;
    diag_mask_t queueMask;
    rtk_qos_congAvoidThresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_QUEUEMASK(queueMask, 5), ret);

    DIAG_UTIL_MASK_SCAN(queueMask, queue)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_qos_congAvoidGlobalQueueDropProbability_set(unit, queue, *dp_ptr, *probability_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_qos_congAvoidGlobalQueueConfig_get(unit, queue, *dp_ptr, &thresh)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            thresh.probability = *probability_ptr;
            if ((ret = rtk_qos_congAvoidGlobalQueueConfig_set(unit, queue, *dp_ptr, &thresh)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_SYSTEM_THRESHOLD_DROP_PRECEDENCE_DP_MAX_THRESHOLD_MAX_THRESHOLD_MIN_THRESHOLD_MIN_THRESHOLD
/*
 * flowctrl set congest-avoidance system-threshold drop-precedence <UINT:dp> max-threshold <UINT:max_threshold> min-threshold <UINT:min_threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_system_threshold_drop_precedence_dp_max_threshold_max_threshold_min_threshold_min_threshold(cparser_context_t *context,
    uint32_t *dp_ptr,
    uint32_t *max_threshold_ptr,
    uint32_t *min_threshold_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qos_congAvoidThresh_t   thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    thresh.maxThresh = *max_threshold_ptr;
    thresh.minThresh = *min_threshold_ptr;

    if ((ret = rtk_qos_congAvoidSysThresh_set(unit, *dp_ptr, &thresh)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_CONGEST_AVOIDANCE_SYSTEM_THRESHOLD_DROP_PRECEDENCE_DP_DROP_PROBABILITY_PROBABILITY
/*
 * flowctrl set congest-avoidance system-threshold drop-precedence <UINT:dp> drop-probability <UINT:probability>
 */
cparser_result_t cparser_cmd_flowctrl_set_congest_avoidance_system_threshold_drop_precedence_dp_drop_probability_probability(cparser_context_t *context,
    uint32_t *drop_precedence_ptr,
    uint32_t *probability_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_qos_congAvoidSysDropProbability_set(unit, *drop_precedence_ptr, *probability_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_HOL_TRAFFIC_DROP_PORT_PORTS_ALL_STATE
/*
 * flowctrl get hol-traffic-drop port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_flowctrl_get_hol_traffic_drop_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("HOL Traffic Drop function Enable Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portHolTrafficDropEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_HOL_TRAFFIC_DROP_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * flowctrl set hol-traffic-drop port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_hol_traffic_drop_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

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

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portHolTrafficDropEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_HOL_TRAFFIC_DROP_TRAFFIC_TYPE_UNKNOWN_UCAST_L2_MCAST_IP_MCAST_BCAST_STATE
/*
 * flowctrl get hol-traffic-drop traffic-type ( unknown-ucast | l2-mcast | ip-mcast | bcast ) state
 */
cparser_result_t cparser_cmd_flowctrl_get_hol_traffic_drop_traffic_type_unknown_ucast_l2_mcast_ip_mcast_bcast_state(
    cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    rtk_flowctrl_holTrafficType_t type = HOL_TRAFFIC_TYPE_END;
    uint8  strType[20] = "";

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(4,0))
    {
        case 'u':
            type = HOL_TRAFFIC_TYPE_UNKN_UC;
            strcpy((char *)strType, "Unknown Unicast");
            break;

        case 'l':
            type = HOL_TRAFFIC_TYPE_L2_MC;
            strcpy((char *)strType, "L2 Multicast");
            break;

        case 'i':
            type = HOL_TRAFFIC_TYPE_IP_MC;
            strcpy((char *)strType, "IP Multicast");
            break;

        case 'b':
            type = HOL_TRAFFIC_TYPE_BC;
            strcpy((char *)strType, "Broadcast");
            break;
    }

    diag_util_mprintf("%s Drop Status\n", strType);
    DIAG_UTIL_ERR_CHK(rtk_flowctrl_holTrafficTypeDropEnable_get(unit, type, &enable), ret);
    diag_util_mprintf("\t%s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_HOL_TRAFFIC_DROP_TRAFFIC_TYPE_UNKNOWN_UCAST_L2_MCAST_IP_MCAST_BCAST_STATE_DISABLE_ENABLE
/*
 * flowctrl set hol-traffic-drop traffic-type ( unknown-ucast | l2-mcast | ip-mcast | bcast ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_flowctrl_set_hol_traffic_drop_traffic_type_unknown_ucast_l2_mcast_ip_mcast_bcast_state_disable_enable(
    cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    rtk_flowctrl_holTrafficType_t type = HOL_TRAFFIC_TYPE_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'u':
            type = HOL_TRAFFIC_TYPE_UNKN_UC;
            break;

        case 'l':
            type = HOL_TRAFFIC_TYPE_L2_MC;
            break;

        case 'i':
            type = HOL_TRAFFIC_TYPE_IP_MC;
            break;

        case 'b':
            type = HOL_TRAFFIC_TYPE_BC;
            break;
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


    DIAG_UTIL_ERR_CHK(rtk_flowctrl_holTrafficTypeDropEnable_set(unit, type, enable), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_FLOWCTRL_GET_E2E_REMOTE_DROP_THRESHOLD_THRESHOLD
/*
 * flowctrl get e2e remote-drop-threshold threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_e2e_remote_drop_threshold_threshold(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\tEnd-to-End FC remote Drop threshold:\n");

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eCascadePortThresh_get(unit, &thresh), ret);
    diag_util_mprintf("\tHigh:%d\n", thresh.high);
    diag_util_mprintf("\tLow:%d\n", thresh.low);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_E2E_REMOTE_DROP_THRESHOLD_HIHG_LOW_THRESHOLD_THRESHOLD
/*
 * flowctrl set e2e remote-drop-threshold ( high | low ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_e2e_remote_drop_threshold_high_low_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eCascadePortThresh_get(unit, &thresh), ret);

    if(TOKEN_CHAR(4,0)=='h')
    {
        thresh.high = *threshold_ptr;
    }
    else if(TOKEN_CHAR(4,0)=='l')
    {
        thresh.low= *threshold_ptr;
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eCascadePortThresh_set(unit, &thresh), ret);

    return CPARSER_OK;

}

#endif

#ifdef CMD_FLOWCTRL_GET_E2E_REMOTE_PORT_THRESHOLD_GROUP_INDEX_FC_OFF_FC_ON_THRESHOLD
/*
 * flowctrl get e2e remote-port-threshold-group <UINT:index> ( fc-off | fc-on ) threshold
 */
cparser_result_t cparser_cmd_flowctrl_get_e2e_remote_port_threshold_group_index_fc_off_fc_on_threshold(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(5,4)=='f')
    {
        diag_util_mprintf("\tEnd-to-End FC-OFF remote-port group %d threshold:\n", *index_ptr);
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortCongestThreshGroup_get(unit, *index_ptr, &thresh), ret);
    }
    else if(TOKEN_CHAR(5,4)=='n')
    {
        diag_util_mprintf("\tEnd-to-End FC-ON remote-port group %d threshold:\n", *index_ptr);
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortPauseThreshGroup_get(unit, *index_ptr, &thresh), ret);
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tHigh:%d\n", thresh.high);
    diag_util_mprintf("\tLow:%d\n", thresh.low);

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_E2E_REMOTE_PORT_THRESHOLD_GROUP_INDEX_FC_OFF_FC_ON_HIGH_LOW_THRESHOLD_THRESHOLD
/*
 * flowctrl set e2e remote-port-threshold-group <UINT:index> ( fc-off | fc-on ) ( high | low ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_flowctrl_set_e2e_remote_port_threshold_group_index_fc_off_fc_on_high_low_threshold_threshold(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(5,4)=='f')
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortCongestThreshGroup_get(unit, *index_ptr, &thresh), ret);
    }
    else if(TOKEN_CHAR(5,4)=='n')
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortPauseThreshGroup_get(unit, *index_ptr, &thresh), ret);
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(TOKEN_CHAR(6,0)=='h')
    {
        thresh.high = *threshold_ptr;
    }
    else if(TOKEN_CHAR(6,0)=='l')
    {
        thresh.low= *threshold_ptr;
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(TOKEN_CHAR(5,4)=='f')
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortCongestThreshGroup_set(unit, *index_ptr, &thresh), ret);
    }
    else if(TOKEN_CHAR(5,4)=='n')
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_e2eRemotePortPauseThreshGroup_set(unit, *index_ptr, &thresh), ret);
    }

    return CPARSER_OK;
}

#endif

#ifdef CMD_FLOWCTRL_GET_E2E_REMOTE_PORT_PORTS_ALL_THRESHOLD_GROUP
/*
 * flowctrl get e2e remote-port ( <PORT_LIST:ports> | all ) threshold-group
 */
cparser_result_t cparser_cmd_flowctrl_get_e2e_remote_port_ports_all_threshold_group(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("End-to-End remote-port threshold group:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portE2eRemotePortThreshGroupSel_get(unit, port, &index), ret);
        diag_util_mprintf("\tPort %2d : %d\n", port, index);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_SET_E2E_REMOTE_PORT_PORTS_ALL_THRESHOLD_GROUP_INDEX
/*
 * flowctrl set e2e remote-port ( <PORT_LIST:ports> | all ) threshold-group <UINT:index>
 */
cparser_result_t cparser_cmd_flowctrl_set_e2e_remote_port_ports_all_threshold_group_index(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_portE2eRemotePortThreshGroupSel_set(unit, port, *index_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_FLOWCTRL_GET_PAUSE_TAG_FWD_STATE
/*
 * flowctrl get pause-tag-fwd state
 */
cparser_result_t
cparser_cmd_flowctrl_get_pause_tag_fwd_state(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_tagPauseEnable_get(unit, &state), ret);

    diag_util_mprintf("Forward pause frame with tag state: ");
    if (DISABLED == state)
    {
        diag_util_mprintf("Disabled\n");
    }
    else
    {
        diag_util_mprintf("Enabled\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_flowctrl_get_pause_tag_fwd_state */
#endif

#ifdef CMD_FLOWCTRL_SET_PAUSE_TAG_FWD_STATE_DISABLE_ENABLE
/*
 * flowctrl set pause-tag-fwd state ( disable | enable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_pause_tag_fwd_state_disable_enable(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'e':
            state = ENABLED;
            break;

        case 'd':
            state = DISABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_tagPauseEnable_set(unit, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_flowctrl_set_pause_tag_fwd_state_disable_enable */
#endif

#ifdef CMD_FLOWCTRL_GET_HALF_CONSECUTIVE_RETRY_STATE
/*
 * flowctrl get half-consecutive-retry state
 */
cparser_result_t
cparser_cmd_flowctrl_get_half_consecutive_retry_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_halfConsecutiveRetryEnable_get(unit, &state), ret);
    diag_util_mprintf("\tHalf Consecutive Retry State: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_half_consecutive_retry_state */
#endif

#ifdef CMD_FLOWCTRL_SET_HALF_CONSECUTIVE_RETRY_STATE_DISABLE_ENABLE
/*
 * flowctrl set half-consecutive-retry state ( disable | enable )
 */
cparser_result_t
cparser_cmd_flowctrl_set_half_consecutive_retry_state_disable_enable(
    cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            state = DISABLED;
            break;

        case 'e':
            state = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_halfConsecutiveRetryEnable_set(unit, state), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_half_consecutive_retry_state_disable_enable */
#endif

