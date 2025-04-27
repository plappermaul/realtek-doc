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
 * Purpose : Definition those NIC command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) NIC
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
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_util.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>


#ifdef CMD_L2_NTFY_GET_STATE
/*
  *  l2ntfy get state
  */
cparser_result_t cparser_cmd_l2_ntfy_get_state(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_enable_get(unit, &enable), ret);
    diag_util_mprintf("L2 Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_GET_BACK_PRESSURE_THRESHOLD
/*
  *  l2ntfy get back-pressure threshold
  */
cparser_result_t cparser_cmd_l2_ntfy_get_back_pressure_threshold(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    uint32                          value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_backPressureThresh_get(unit, &value), ret);
    diag_util_mprintf("L2-Notification Back-Pressure Threshold : %d\n", value);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_GET_TYPE_LINK_DOWN_FLUSH_SUSPEND_STATE
/*
 * l2ntfy get type ( link-down-flush | suspend ) state
 */
cparser_result_t
cparser_cmd_l2_ntfy_get_type_link_down_flush_suspend_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('l' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_LINKDOWNFLUSH, &enable), ret);
        diag_util_mprintf("L2 Suspend Learn Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if('s' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_SUSPEND, &enable), ret);
        diag_util_mprintf("Link-down-flush Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_get_notification_type_suspend_state */
#endif

#ifdef CMD_L2_NTFY_GET_DEBUG_FLAG
/*
 * l2ntfy get debug-flag
 */
cparser_result_t cparser_cmd_l2_ntfy_get_debug_flag(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_debug_get(unit, &value), ret);
    diag_util_mprintf("L2-Notification debug flag : %d\n", value);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_STATE_ENABLE_DISABLE
/*
 * l2ntfy set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l2_ntfy_set_state_enable_disable(
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

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_enable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_BACK_PRESSURE_THRESHOLD_THRESHOLD
/*
  *  l2ntfy set back-pressure threshold <UINT:threshold>
  */
cparser_result_t cparser_cmd_l2_ntfy_set_back_pressure_threshold_threshold(cparser_context_t *context, uint32_t *threshold_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_backPressureThresh_set(unit, *threshold_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_TYPE_LINK_DOWN_FLUSH_SUSPEND_STATE_DISABLE_ENABLE
/*
 * l2ntfy set type ( link-down-flush | suspend ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_ntfy_set_type_link_down_flush_suspend_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(5, 0))
    {
        if ('l' == TOKEN_CHAR(3, 0))
            DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_set(unit, L2NTFY_EVENT_LINKDOWNFLUSH, ENABLED), ret);
        else if('s' == TOKEN_CHAR(3, 0))
            DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_set(unit, L2NTFY_EVENT_SUSPEND, ENABLED), ret);
        else
        {
            diag_util_mprintf("Not Support\n");
            return CPARSER_OK;
        }
    }
    else if('d' == TOKEN_CHAR(5, 0))
    {
        if ('l' == TOKEN_CHAR(3, 0))
            DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_set(unit, L2NTFY_EVENT_LINKDOWNFLUSH, DISABLED), ret);
        else if('s' == TOKEN_CHAR(3, 0))
            DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_set(unit, L2NTFY_EVENT_SUSPEND, DISABLED), ret);
        else
        {
            diag_util_mprintf("Not Support\n");
            return CPARSER_OK;
        }
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }


    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_notification_type_suspend_state_disable_enable */
#endif

#ifdef CMD_L2_NTFY_RESET_DUMP_COUNTER
/*
 * l2ntfy ( reset | dump ) counter
 */
cparser_result_t cparser_cmd_l2_ntfy_reset_dump_counter(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('r' == TOKEN_CHAR(1,0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_counter_clear(unit), ret);
    }
    else if ('d' == TOKEN_CHAR(1,0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_counter_dump(unit), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_DEBUG_FLAG_BITMASK
/*
 * l2ntfy set debug-flag <UINT:bitmask>
 */
cparser_result_t cparser_cmd_l2_ntfy_set_debug_flag_bitmask(cparser_context_t *context, uint32_t *bitmask)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_debug_set(unit, *bitmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_GET_DST
/*
 * l2-ntfy get dst
 */
cparser_result_t
cparser_cmd_l2_ntfy_get_dst(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_l2ntfy_dst_t dst;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_dst_get(unit, &dst), ret);
    diag_util_mprintf("L2 Notification Destination :   ");
    switch(dst)
    {
        case L2NTFY_DST_PKT_TO_LOCAL:
            diag_util_mprintf("%s", "packet to local");break;
        case L2NTFY_DST_PKT_TO_MASTER:
            diag_util_mprintf("%s", "packet to master");break;
        case L2NTFY_DST_NIC:
            diag_util_mprintf("%s", "NIC");break;
        default:
            diag_util_mprintf("Not Support\n");return CPARSER_OK;
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_ntfy_get_dst */
#endif

#ifdef CMD_L2_NTFY_SET_DST_SEND_TO_LOCAL_SEND_TO_MASTER_NIC
/*
 * l2-ntfy set dst ( send-to-local | send-to-master | nic )
 */
cparser_result_t
cparser_cmd_l2_ntfy_set_dst_send_to_local_send_to_master_nic(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_l2ntfy_dst_t dst;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('n' == TOKEN_CHAR(3, 0))
    {
        dst = L2NTFY_DST_NIC;
    }
    else if ('l' == TOKEN_CHAR(3, 8))
    {
        dst = L2NTFY_DST_PKT_TO_LOCAL;
    }
    else if ('m' == TOKEN_CHAR(3, 8))
    {
        dst = L2NTFY_DST_PKT_TO_MASTER;
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_dst_set(unit, dst), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_l2_ntfy_set_dst_send_to_local_send_to_master_nic */
#endif

#ifdef CMD_L2_NTFY_GET_TYPE_TAG_STS_DABLK_SABLK_STATIC_DYNAMIC_HASH_FULL_STATE
/*
 * l2-ntfy get type ( tag-sts | dablk-sablk | static | dynamic | hash-full ) state
 */
cparser_result_t
cparser_cmd_l2_ntfy_get_type_tag_sts_dablk_sablk_static_dynamic_hash_full_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('t' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_TAG, &enable), ret);
        diag_util_mprintf("Tag Status Change Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if('d' == TOKEN_CHAR(3, 0) && 'a' == TOKEN_CHAR(3, 1))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_DASABLK, &enable), ret);
        diag_util_mprintf("DA/SA block Entry Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if('s' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_STTC, &enable), ret);
        diag_util_mprintf("Static Entry Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if('d' == TOKEN_CHAR(3, 0) && 'y' == TOKEN_CHAR(3, 1))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_DYN, &enable), ret);
        diag_util_mprintf("Dynamic Entry Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if('h' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_get(unit, L2NTFY_EVENT_HASHFULL, &enable), ret);
        diag_util_mprintf("Hash-Full Notification Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_ntfy_get_type_tag_sts_dablk_sablk_static_dynamic_state */
#endif

#ifdef CMD_L2_NTFY_SET_TYPE_TAG_STS_DABLK_SABLK_STATIC_DYNAMIC_HASH_FULL_STATE_DISABLE_ENABLE
/*
 * l2-ntfy set type ( tag-sts | dablk-sablk | static | dynamic | hash-full ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_ntfy_set_type_tag_sts_dablk_sablk_static_dynamic_hash_full_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t    enable;
    rtk_l2ntfy_event_t event;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if('t' == TOKEN_CHAR(3, 0))
    {
        event = L2NTFY_EVENT_TAG;
    }
    else if('d' == TOKEN_CHAR(3, 0) && 'a' == TOKEN_CHAR(3, 1))
    {
        event = L2NTFY_EVENT_DASABLK;
    }
    else if('s' == TOKEN_CHAR(3, 0))
    {
        event = L2NTFY_EVENT_STTC;
    }
    else if('d' == TOKEN_CHAR(3, 0) && 'y' == TOKEN_CHAR(3, 1))
    {
        event = L2NTFY_EVENT_DYN;
    }
    else if('h' == TOKEN_CHAR(3, 0))
    {
        event = L2NTFY_EVENT_HASHFULL;
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_notificationEventEnable_set(unit, event, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_ntfy_set_type_tag_sts_dablk_sablk_static_dynamic_state_disable_enable */
#endif

#ifdef CMD_L2_NTFY_GET_DMAC_SMAC
/*
 * l2-ntfy get ( dmac | smac )
 */
cparser_result_t
cparser_cmd_l2_ntfy_get_dmac_smac(
    cparser_context_t *context)
{
    uint32      unit;
    int32       ret;
    rtk_mac_t   mac;
    char        macStr[18];
    rtk_l2ntfy_addrType_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(2, 0))
    {
        type = L2NTFY_ADDR_DMAC;
    }
    else if ('s' == TOKEN_CHAR(2, 0))
    {
        type = L2NTFY_ADDR_SMAC;
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_macAddr_get(unit, type, &mac), ret);
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, mac.octet), ret);
    if (L2NTFY_ADDR_DMAC ==type)
        diag_util_printf("DMAC address : %s\n", macStr);
    else
        diag_util_printf("SMAC address : %s\n", macStr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_DMAC_SMAC_MAC
/*
 * l2-ntfy set ( dmac | smac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_ntfy_set_dmac_smac_mac(
    cparser_context_t *context, cparser_macaddr_t *mac_ptr)
{
    uint32      unit;
    int32       ret;
    rtk_mac_t   mac;
    rtk_l2ntfy_addrType_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(diag_util_str2mac(mac.octet, TOKEN_STR(3)), ret);

    if ('d' == TOKEN_CHAR(2, 0))
    {
        type = L2NTFY_ADDR_DMAC;
    }
    else if ('s' == TOKEN_CHAR(2, 0))
    {
        type = L2NTFY_ADDR_SMAC;
    }
    else
    {
        diag_util_mprintf("Not Support\n");
        return CPARSER_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_macAddr_set(unit, type, &mac), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_GET_MAGIC_NUM
/*
 * l2ntfy get magic-num
 */
cparser_result_t cparser_cmd_l2_ntfy_get_magic_num(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      magicNum;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_magicNum_get(unit, &magicNum), ret);
    diag_util_printf("Magic number : %#x\n", magicNum);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_NTFY_SET_MAGIC_NUM_NUM
/*
 * l2ntfy set magic-num <UINT:num>
 */
cparser_result_t cparser_cmd_l2_ntfy_set_magic_num_num(cparser_context_t *context, uint32_t *num_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_l2ntfy_magicNum_set(unit, *num_ptr), ret);

    return CPARSER_OK;
}
#endif

