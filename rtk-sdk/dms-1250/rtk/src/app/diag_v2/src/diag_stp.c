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
 * Purpose : Definition those STP command and APIs in the SDK diagnostic shell.
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
#include <rtk/stp.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_stp.h>
#endif
#ifdef CMD_STP_CREATE_INSTANCE
/*
  * stp create <UINT:instance>
  */
cparser_result_t cparser_cmd_stp_create_instance(cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*instance_ptr >= devInfo.capacityInfo.max_num_of_msti), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_stp_mstpInstance_create(unit, *instance_ptr), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_STP_DESTROY_INSTANCE
/*
  * stp destroy <UINT:instance>
  */
cparser_result_t cparser_cmd_stp_destroy_instance(cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*instance_ptr >= devInfo.capacityInfo.max_num_of_msti), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_stp_mstpInstance_destroy(unit, *instance_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_STP_DUMP_INSTANCE_PORT_ALL
/*
 * stp dump <UINT:instance> ( <PORT_LIST:port> | all )
 */
cparser_result_t cparser_cmd_stp_dump_instance_port_all(cparser_context_t *context,
    uint32_t *instance_ptr,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_stp_state_t stp_state = 0;
    rtk_switch_devInfo_t devInfo;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*instance_ptr >= devInfo.capacityInfo.max_num_of_msti), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("MSTI %d Status:\n", *instance_ptr);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stp_mstpState_get(unit, *instance_ptr, port, &stp_state), ret);
        diag_util_mprintf("\tPort %2d: ", port);

        if (STP_STATE_DISABLED == stp_state)
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        else if (STP_STATE_BLOCKING == stp_state)
        {
            diag_util_mprintf("BLOCKING\n");
        }
        else if (STP_STATE_LEARNING == stp_state)
        {
            diag_util_mprintf("LEARNING\n");
        }
        else if (STP_STATE_FORWARDING == stp_state)
        {
            diag_util_mprintf("FORWARDING\n");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_STP_GET_INSTANCE
/*
 * stp get <UINT:instance>
 */
cparser_result_t
cparser_cmd_stp_get_instance(
    cparser_context_t *context,
    uint32_t *instance_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          instance_state = 0;
    rtk_switch_devInfo_t devInfo;

    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*instance_ptr >= devInfo.capacityInfo.max_num_of_msti), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("MSTI %d is:\n", *instance_ptr);

    DIAG_UTIL_ERR_CHK(rtk_stp_isMstpInstanceExist_get(unit, *instance_ptr, &instance_state), ret);
    diag_util_mprintf("%s\n", (instance_state)?"Exist":"Not exist");

    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_stp_get_instance */
#endif

#ifdef CMD_STP_SET_INSTANCE_PORT_ALL_BLOCKING_DISABLE_FORWARDING_LEARNING
/*
 * stp set <UINT:instance> ( <PORT_LIST:port> | all )  ( blocking | disable | forwarding | learning )
 */
cparser_result_t cparser_cmd_stp_set_instance_port_all_blocking_disable_forwarding_learning(cparser_context_t *context,
    uint32_t *instance_ptr,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_stp_state_t stp_state = 0;
    rtk_switch_devInfo_t devInfo;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*instance_ptr >= devInfo.capacityInfo.max_num_of_msti), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(4, 0))
    {
        stp_state = STP_STATE_DISABLED;
    }
    else if ('b' == TOKEN_CHAR(4, 0))
    {
        stp_state = STP_STATE_BLOCKING;
    }
    else if ('l' == TOKEN_CHAR(4, 0))
    {
        stp_state = STP_STATE_LEARNING;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        stp_state = STP_STATE_FORWARDING;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stp_mstpState_set(unit, *instance_ptr, port, stp_state), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_STP_GET_MSTI_MODE
/*
 * stp get msti_mode
 */
cparser_result_t cparser_cmd_stp_get_msti_mode(cparser_context_t *context)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    uint32 msti_mode = STP_MSTI_MODE_NORMAL;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stp_mstpInstanceMode_get(unit, &msti_mode), ret);

    diag_util_mprintf("MSTI mode: %s\n", (STP_MSTI_MODE_NORMAL == msti_mode)? "Normal":"CIST");

    return CPARSER_OK;
}
#endif

#ifdef CMD_STP_SET_MSTI_MODE_NORMAL_CIST
/*
 * stp set msti_mode ( normal | cist )
 */
cparser_result_t cparser_cmd_stp_set_msti_mode_normal_cist(cparser_context_t *context)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    uint32 msti_mode = STP_MSTI_MODE_NORMAL;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('n' == TOKEN_CHAR(3, 0))
    {
        msti_mode = STP_MSTI_MODE_NORMAL;
    }
    else
    {
        msti_mode = STP_MSTI_MODE_CIST;
    }

    DIAG_UTIL_ERR_CHK(rtk_stp_mstpInstanceMode_set(unit, msti_mode), ret);

    return CPARSER_OK;
}
#endif
