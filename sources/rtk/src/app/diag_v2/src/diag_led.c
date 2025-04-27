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
 * Purpose : Definition those LED command and APIs in the SDK diagnostic shell.
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
#include <rtk/led.h>
#include <rtk/switch.h>
#include <diag_util.h>
#include <diag_str.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_led.h>
  #include <rtrpc/rtrpc_switch.h>
#endif
#ifdef CMD_LED_GET_SYSTEM_SYS_STATE
/*
 * led get system sys state
 */
cparser_result_t cparser_cmd_led_get_system_sys_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_led_type_t  type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    type = LED_TYPE_SYS;
    DIAG_UTIL_ERR_CHK(rtk_led_sysEnable_get(unit, type, &enable), ret);
    diag_util_mprintf("System LED: %s\n", (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_LED_SET_SYSTEM_SYS_STATE_DISABLE_ENABLE
/*
 * led set system sys state ( disable | enable )
 */
cparser_result_t cparser_cmd_led_set_system_sys_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_led_type_t  type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    type = LED_TYPE_SYS;

    if ('d' == TOKEN_CHAR(5, 0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5, 0))
    {
        enable = ENABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_led_sysEnable_set(unit, type, enable), ret);
    return CPARSER_OK;
}
#endif

rtk_led_swCtrl_mode_t diag_led_strToMode(char *str)
{
    if ('o' == str[0])
        return RTK_LED_SWCTRL_MODE_OFF;
    else if ('c' == str[0])
        return RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE;
    else if ('b' == str[0])
    {
        if (strlen("blink-1024") == strlen(str))
            return RTK_LED_SWCTRL_MODE_BLINKING_1024MS;

        switch (str[6])
        {
            case '3':
                return RTK_LED_SWCTRL_MODE_BLINKING_32MS;
            case '6':
                return RTK_LED_SWCTRL_MODE_BLINKING_64MS;
            case '1':
                return RTK_LED_SWCTRL_MODE_BLINKING_128MS;
            case '2':
                return RTK_LED_SWCTRL_MODE_BLINKING_256MS;
            case '5':
                return RTK_LED_SWCTRL_MODE_BLINKING_512MS;
            default:
                return RTK_LED_SWCTRL_MODE_BLINKING_END;
        }
    }

    return RTK_LED_SWCTRL_MODE_BLINKING_END;
}

char* diag_led_modeToStr(rtk_led_swCtrl_mode_t mode)
{
    switch (mode)
    {
        case RTK_LED_SWCTRL_MODE_OFF:
            return "off";
        case RTK_LED_SWCTRL_MODE_BLINKING_32MS:
            return "blinking per 32ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_64MS:
            return "blinking per 64ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_128MS:
            return "blinking per 128ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_256MS:
            return "blinking per 256ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_512MS:
            return "blinking per 512ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_1024MS:
            return "blinking per 1024ms";
        case RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE:
            return "continue light";
        default:
            break;
    }

    return "";
}

#ifdef CMD_LED_GET_SOFTWARE_CONTROL_PORT_ALL_MODE
/*
 * led get software-control ( <PORT_LIST:port> | all ) mode
 */
cparser_result_t cparser_cmd_led_get_software_control_port_all_mode(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit;
    uint32                  entity;
    int32                   ret;
    rtk_led_swCtrl_mode_t   mode;
    rtk_port_t              port;
    diag_portlist_t         portlist;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    diag_util_mprintf("LED Softward Control Mode Configuration:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %u:\n", port);
        for (entity = 0; entity < devInfo.capacityInfo.max_num_of_led_entity; ++entity)
        {
            diag_util_mprintf("\tLED entity %u:\n", entity);

            ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port, entity,
                    PORT_MEDIA_COPPER, &mode);
            if (RT_ERR_OK != ret)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("\t\tCopper mode: %s\n", diag_led_modeToStr(mode));

            ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port, entity,
                    PORT_MEDIA_FIBER, &mode);
            if (RT_ERR_OK != ret)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("\t\t Fiber mode: %s\n", diag_led_modeToStr(mode));
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_get_software_control_port_all_mode */
#endif

#ifdef CMD_LED_GET_SOFTWARE_CONTROL_PORT_ALL_STATE
/*
 * led get software-control ( <PORT_LIST:port> | all ) state
 */
cparser_result_t cparser_cmd_led_get_software_control_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit;
    uint32          entity;
    int32           ret;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("LED Softward Control State Configuration:\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %u:\n", port);
        for (entity = 0; entity < devInfo.capacityInfo.max_num_of_led_entity; ++entity)
        {
            diag_util_mprintf("\tLED entity %u: ", entity);

            ret = rtk_led_portLedEntitySwCtrlEnable_get(unit, port, entity,
                    &enable);
            if (RT_ERR_OK != ret)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("%s\n", (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_get_software_control_port_all_state */
#endif

#ifdef CMD_LED_GET_SYSTEM_SYS_MODE
/*
 * led get system sys mode
 */
cparser_result_t
cparser_cmd_led_get_system_sys_mode(
    cparser_context_t *context)
{
    uint32                  unit;
    int32                   ret;
    rtk_led_swCtrl_mode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_led_sysMode_get(unit, &mode);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("System LED mode: %s\n", diag_led_modeToStr(mode));

    return CPARSER_OK;
}   /* end of cparser_cmd_led_get_system_sys_mode */
#endif

#ifdef CMD_LED_GET_BLINK_TIME
/*
 * led get blink-time
 */
cparser_result_t
cparser_cmd_led_get_blink_time(
    cparser_context_t *context)
{
    uint32              unit;
    int32               ret;
    rtk_led_blinkTime_t time;
    uint32              ms;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ((ret = rtk_led_blinkTime_get(unit, &time)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (time)
    {
    case RTK_LED_BLINKTIME_32MS:
        ms = 32;
        break;

    case RTK_LED_BLINKTIME_64MS:
        ms = 64;
        break;

    case RTK_LED_BLINKTIME_128MS:
        ms = 128;
        break;

    case RTK_LED_BLINKTIME_256MS:
        ms = 256;
        break;

    case RTK_LED_BLINKTIME_512MS:
        ms = 512;
        break;

    case RTK_LED_BLINKTIME_1024MS:
        ms = 1024;
        break;

    default:
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Blinking Cycle Time: %dms\n", ms);

    return CPARSER_OK;
}   /* end of cparser_cmd_led_get_blink_time */
#endif

#ifdef CMD_LED_SET_SOFTWARE_CONTROL_PORT_ALL_ENTITY_ENTITY_MEDIA_COPPER_FIBER_MODE_OFF_BLINK_32_BLINK_64_BLINK_128_BLINK_256_BLINK_512_BLINK_1024_CONTINUE
/*
 * led set software-control ( <PORT_LIST:port> | all ) entity <UINT:entity> media ( copper | fiber ) mode ( off | blink-32 | blink-64 | blink-128 | blink-256 | blink-512 | blink-1024 | continue )
 */
cparser_result_t
cparser_cmd_led_set_software_control_port_all_entity_entity_media_copper_fiber_mode_off_blink_32_blink_64_blink_128_blink_256_blink_512_blink_1024_continue(cparser_context_t *context,
    char **port_ptr,
    uint32_t *entity_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_led_swCtrl_mode_t   mode;
    rtk_port_media_t        media;
    rtk_port_t              port;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('c' == TOKEN_CHAR(7, 0))
    {
        media = PORT_MEDIA_COPPER;
    }
    else if ('f' == TOKEN_CHAR(7, 0))
    {
        media = PORT_MEDIA_FIBER;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    mode = diag_led_strToMode(TOKEN_STR(9));
    if (RTK_LED_SWCTRL_MODE_BLINKING_END == mode)
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_led_portLedEntitySwCtrlMode_set(unit, port, *entity_ptr,
                    media, mode);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_set_software_control_port_all_entity_entity_media_copper_fiber_mode_off_blink_32_blink_64_blink_128_blink_256_blink_512_blink_1024_continue */
#endif

#ifdef CMD_LED_SET_SOFTWARE_CONTROL_PORT_ALL_ENTITY_ENTITY_STATE_DISABLE_ENABLE
/*
 * led set software-control ( <PORT_LIST:port> | all ) entity <UINT:entity> state ( disable | enable )
 */
cparser_result_t cparser_cmd_led_set_software_control_port_all_entity_entity_state_disable_enable(
    cparser_context_t *context,
    char **port_ptr,
    uint32_t *entity_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('d' == TOKEN_CHAR(7, 0))
        {
            enable = DISABLED;
        }
        else if ('e' == TOKEN_CHAR(7, 0))
        {
            enable = ENABLED;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        ret = rtk_led_portLedEntitySwCtrlEnable_set(unit, port, *entity_ptr,
                    enable);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_set_software_control_port_all_entity_entity_state_disable_enable */
#endif

#ifdef CMD_LED_SET_SOFTWARE_CONTROL_START
/*
 * led set software-control start
 */
cparser_result_t
cparser_cmd_led_set_software_control_start(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_led_swCtrl_start(unit);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_set_software_control_start */
#endif

#ifdef CMD_LED_SET_SYSTEM_SYS_MODE_OFF_BLINK_64_BLINK_1024_CONTINUE
/*
 * led set system sys mode ( off | blink-64 | blink-1024 | continue )
 */
cparser_result_t
cparser_cmd_led_set_system_sys_mode_off_blink_64_blink_1024_continue(
    cparser_context_t *context)
{
    uint32                  unit;
    int32                   ret;
    rtk_led_swCtrl_mode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    mode = diag_led_strToMode(TOKEN_STR(5));
    if (RTK_LED_SWCTRL_MODE_BLINKING_END == mode)
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_led_sysMode_set(unit, mode);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_set_system_sys_mode_off_blink_64_blink_1024_continue */
#endif

#ifdef CMD_LED_SET_BLINK_TIME_PER_32MS_PER_64MS_PER_128MS_PER_256MS_PER_512MS_PER_1024MS
/*
 * led set blink-time ( per-32ms | per-64ms | per-128ms | per-256ms | per-512ms | per-1024ms )
 */
cparser_result_t
cparser_cmd_led_set_blink_time_per_32ms_per_64ms_per_128ms_per_256ms_per_512ms_per_1024ms(
    cparser_context_t *context)
{
    uint32              unit;
    int32               ret;
    rtk_led_blinkTime_t time;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (0 == osal_strcmp(TOKEN_STR(3), "per-32ms"))
    {
        time = RTK_LED_BLINKTIME_32MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "per-64ms"))
    {
        time = RTK_LED_BLINKTIME_64MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "per-128ms"))
    {
        time = RTK_LED_BLINKTIME_128MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "per-256ms"))
    {
        time = RTK_LED_BLINKTIME_256MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "per-512ms"))
    {
        time = RTK_LED_BLINKTIME_512MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "per-1024ms"))
    {
        time = RTK_LED_BLINKTIME_1024MS;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_led_blinkTime_set(unit, time)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_led_set_blink_time_per_32ms_per_64ms_per_128ms_per_256ms_per_512ms_per_1024ms */
#endif

