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
 * Purpose : Definition those EEE command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) EEE enable/disable
 *           2) EEE wake up and sleep mode select
 *           3) Parameter for wake up and sleep mode
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
#include <rtk/switch.h>
#include <rtk/port.h>
#include <rtk/eee.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_eee.h>
#endif

#ifdef CMD_EEE_GET_PORT_PORTS_ALL_STATE
/*
 * eee get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_eee_get_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_enable_t        enabled = DISABLED;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    /* show port info */
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d EEE:\n", port);
        diag_util_mprintf("    Configure : ");
        DIAG_UTIL_ERR_CHK(rtk_eee_portEnable_get(unit, port, &enabled), ret);
        if (ENABLED == enabled)
        {
            diag_util_mprintf("enable, ");
        }
        else
        {
            diag_util_mprintf("disable, ");
        }
#ifndef PHY_ONLY
        diag_util_mprintf("    Status : ");
        DIAG_UTIL_ERR_CHK(rtk_eee_portState_get(unit, port, &enabled), ret);
        if (ENABLED == enabled)
        {
            diag_util_mprintf("enable\n");
        }
        else
        {
            diag_util_mprintf("disable\n");
        }
#endif
    }

    return CPARSER_OK;
} /* end of cparser_cmd_eee_get_port_ports_all_state */
#endif /* CMD_EEE_GET_PORT_PORTS_ALL_STATE */

#ifdef CMD_EEE_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * eee set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_eee_set_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_eee_portEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_eee_set_port_ports_all_state_disable_enable */
#endif /* CMD_EEE_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE */

#ifdef CMD_EEE_GET_PORT_PORTS_ALL_DIRECTION_INGRESS_EGRESS_POWER_STATE
/*
 * eee get port ( <PORT_LIST:ports> | all ) direction ( ingress | egress ) power-state
 */
cparser_result_t
cparser_cmd_eee_get_port_ports_all_direction_ingress_egress_power_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_eee_direction_t        direction;
    rtk_eee_power_state_t        state;
    diag_portlist_t     portlist;
    char dirStr[16];
    char stateStr[16];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('i' == TOKEN_CHAR(5,0))
    {
        direction = EEE_DIRECTION_IGR;
        osal_strcpy(dirStr, "Ingress");
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
       direction = EEE_DIRECTION_EGR;
       osal_strcpy(dirStr, "Egress");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    /* show port info */
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_eee_portPowerState_get(unit, port, direction, &state), ret);
        if(EEE_POWER_WAKE_STATE == state)
            osal_strcpy(stateStr, "Wake");
        else
            osal_strcpy(stateStr, "Sleep");
        diag_util_mprintf("Port %d %s power state: %s\n", port, dirStr, stateStr);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_eee_get_port_ports_all_direction_ingress_egress_power_state */
#endif

#ifdef CMD_EEEP_GET_PORT_PORTS_ALL_STATE
/*
 * eeep get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_eeep_get_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_enable_t        enabled = DISABLED;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    /* show port info */
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d:\n", port);
        diag_util_printf("    EEEP : ");
        DIAG_UTIL_ERR_CHK(rtk_eeep_portEnable_get(unit, port, &enabled), ret);
        if (ENABLED == enabled)
        {
            diag_util_mprintf("enable\n");
        }
        else
        {
            diag_util_mprintf("disable\n");
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_eeep_get_port_ports_all_state */
#endif /* CMD_EEEP_GET_PORT_PORTS_ALL_STATE */

#ifdef CMD_EEEP_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * eeep set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_eeep_set_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_eeep_portEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_eeep_set_port_ports_all_state_disable_enable */
#endif /* CMD_EEEP_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE */

