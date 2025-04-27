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
 * Purpose : Definition those rate command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) ingress bandwidth
 *           2) egress bandwidth
 *           3) storm control
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
#include <rtk/rate.h>
#include <rtk/flowctrl.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_rate.h>
  #include <rtrpc/rtrpc_flowctrl.h>
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_PORT_PORTS_ALL
/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_port_ports_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0, rate;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32 burst;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d : \n", port);
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlEnable_get(unit, port, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("\tEgress Bandwidth : Enable\n");
        }
        else
        {
            diag_util_mprintf("\tEgress Bandwidth : Disable\n");
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlRate_get(unit, port, &rate), ret);
        diag_util_mprintf("\tRate : %d (0x%x)\n", rate, rate);

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlBurstSize_get(unit, port, &burst), ret);
            diag_util_mprintf("\tBurst Size : %d (0x%x)\n", burst, burst);
        }
#endif
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_egress_port_all */
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_IFG
/*
 * bandwidth get egress ifg
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_ifg(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    ifg_include = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* show all egr-bandwidth info */
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &ifg_include), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_get(unit, RATE_MODULE_EGR, &ifg_include), ret);
    }
    if (ENABLED == ifg_include)
    {
        diag_util_mprintf("Egress Bandwidth : Include IFG\n");
    }
    else
    {
        diag_util_mprintf("Egress Bandwidth : Exclude IFG\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_egress_ifg */
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID
/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id>
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_port_ports_all_queue_queue_id(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr)
{
    uint32          unit = 0, rate;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32 burst;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port, *queue_id_ptr, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("\tEgress Bandwidth (Queue %d) : Enable\n", *queue_id_ptr);
        }
        else
        {
            diag_util_mprintf("\tEgress Bandwidth (Queue %d) : Disable\n", *queue_id_ptr);
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlRate_get(unit, port, *queue_id_ptr, &rate), ret);
        diag_util_mprintf("\tRate (Queue %d) : %d (0x%x)\n", *queue_id_ptr, rate, rate);
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlBurstSize_get(unit, port, *queue_id_ptr, &burst), ret);
            diag_util_mprintf("\tBurst Size (Queue %d) : %d (0x%x)\n", *queue_id_ptr, burst, burst);
        }
#endif
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_egress_port_all_queue_id */
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_CPU_COUNTING_MODE
/*
 * bandwidth get egress cpu-counting-mode
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_cpu_counting_mode(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_rate_rateMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_cpuEgrBandwidthCtrlRateMode_get(unit, &mode), ret);
    if (RATE_MODE_BYTE == mode)
    {
        diag_util_mprintf("CPU Egress Bandwidth Control Counting Mode : Byte\n");
    }
    else
    {
        diag_util_mprintf("CPU Egress Bandwidth Control Counting Mode : Packet\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_egress_ifg_port_all */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_IFG_EXCLUDE_INCLUDE
/*
 * bandwidth set egress ifg ( exclude | include )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_ifg_exclude_include(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    /* set IFG include */
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_set(unit, enable), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_set(unit, RATE_MODULE_EGR, enable), ret);
    }
    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_ifg_exclude_include */
#endif


#ifdef CMD_BANDWIDTH_GET_EGRESS_BURST_SIZE
/*
 * bandwidth get egress burst-size
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_burst_size(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          burst_size;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_egrPortBwCtrlBurstSize_get(unit, &burst_size), ret);
    diag_util_mprintf("Egress Port Burst Size(Bytes) : %d\n", burst_size);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_CPU_COUNTING_MODE_BYTE_PACKET
/*
 * bandwidth set egress cpu-counting-mode ( byte | packet )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_cpu_counting_mode_byte_packet(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_rate_rateMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(4,0))
    {
        mode = RATE_MODE_BYTE;
    }
    else if ('p' == TOKEN_CHAR(4,0))
    {
        mode = RATE_MODE_PKT;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_cpuEgrBandwidthCtrlRateMode_set(unit, mode), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_ifg_port_all_exclude_include */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_port_all_state_disable_enable */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_RATE_RATE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlRate_set(unit, port, *rate_ptr), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_port_all_rate_rate */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_BURST_SIZE_SIZE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_burst_size_size(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBwCtrlBurstSize_set(unit, port, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_STATE_DISABLE_ENABLE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_queue_queue_id_state_disable_enable(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t    enable = DISABLED;
    rtk_qid_t       queue_id = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    queue_id = *queue_id_ptr;

    if ('e' == TOKEN_CHAR(8,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlEnable_set(unit, port, queue_id, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_port_all_queue_queue_id_state_disable_enable */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_RATE_RATE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> rate <UINT:rate>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_queue_queue_id_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlRate_set(unit, port, *queue_id_ptr, *rate_ptr), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_egress_port_all_queue_queue_id_rate_rate */
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_BURST_SIZE_SIZE
/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_port_ports_all_queue_queue_id_burst_size_size(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueBwCtrlBurstSize_set(unit, port, *queue_id_ptr, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_BURST_SIZE_SIZE
/*
 * bandwidth set egress burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_burst_size_size(cparser_context_t *context,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_rate_egrPortBwCtrlBurstSize_set(unit, *size_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_FIXED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_STATE
/*
 * bandwidth get egress fixed-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> state
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_fixed_bandwidth_port_ports_all_queue_queue_id_state(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *qid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
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
        diag_util_mprintf("Egress Fixed Bandwidth Ability\n");

        DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueFixedBandwidthEnable_get(unit, port, *qid_ptr, &enable), ret);

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

#ifdef CMD_BANDWIDTH_SET_EGRESS_FIXED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_STATE_DISABLE_ENABLE
/*
 * bandwidth set egress fixed-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_fixed_bandwidth_port_ports_all_queue_queue_id_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *qid_ptr)
{
    uint32      unit = 0;
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    int32       ret = RT_ERR_FAILED;
#endif
    rtk_port_t  port;
    rtk_enable_t    enable;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(9,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(9,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            if ((ret = rtk_rate_egrQueueFixedBandwidthEnable_set(unit, port, *qid_ptr, enable)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
#endif
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_ASSURED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID
/*
 * bandwidth get egress assured-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id>
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_assured_bandwidth_port_ports_all_queue_queue_id(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    rtk_rate_assuredMode_t mode = ASSURED_MODE_SHARE;
    diag_portlist_t portlist;
    uint32          rate, burst;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlEnable_get(unit, port, *queue_id_ptr, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("\tAssured Bandwidth (Queue %d) : Enable\n", *queue_id_ptr);
        }
        else
        {
            diag_util_mprintf("\tAssured Bandwidth (Queue %d) : Disable\n", *queue_id_ptr);
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlRate_get(unit, port, *queue_id_ptr, &rate), ret);
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get(unit, port, *queue_id_ptr, &burst), ret);
        diag_util_mprintf("\tRate (Queue %d) : %d (0x%x)\n", *queue_id_ptr, rate, rate);
        diag_util_mprintf("\tBurst Size (Queue %d) : %d (0x%x)\n", *queue_id_ptr, burst, burst);
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlMode_get(unit, port, *queue_id_ptr, &mode), ret);
        if (ASSURED_MODE_SHARE == mode)
        {
            diag_util_mprintf("\tMode (Queue %d) : Shared Remain Bandwidth\n", *queue_id_ptr);
        }
        else
        {
            diag_util_mprintf("\tMode (Queue %d) : Fixed Bandwidth\n", *queue_id_ptr);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_ASSURED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_STATE_DISABLE_ENABLE
/*
 * bandwidth set egress assured-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_assured_bandwidth_port_ports_all_queue_queue_id_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    rtk_enable_t    enable;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(9,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(9,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(unit, port, *queue_id_ptr, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_ASSURED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_RATE_RATE
/*
 * bandwidth set egress assured-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> rate <UINT:rate>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_assured_bandwidth_port_ports_all_queue_queue_id_rate_rate(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlRate_set(unit, port, *queue_id_ptr, *rate_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_ASSURED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_BURST_SIZE_SIZE
/*
 * bandwidth set egress assured-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_assured_bandwidth_port_ports_all_queue_queue_id_burst_size_size(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set(unit, port, *queue_id_ptr, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_ASSURED_BANDWIDTH_PORT_PORTS_ALL_QUEUE_QUEUE_ID_MODE_SHARE_BANDWIDTH_FIX_BANDWIDTH
/*
 * bandwidth set egress assured-bandwidth port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> mode ( shared-bandwidth | fixed-bandwidth )
 */
 cparser_result_t cparser_cmd_bandwidth_set_egress_assured_bandwidth_port_ports_all_queue_queue_id_mode_shared_bandwidth_fixed_bandwidth(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    rtk_rate_assuredMode_t mode;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('s' == TOKEN_CHAR(9,0))
    {
        mode = ASSURED_MODE_SHARE;
    }
    else if ('f' == TOKEN_CHAR(9,0))
    {
        mode = ASSURED_MODE_FIX;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_set(unit, port, *queue_id_ptr, mode)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_IFG
/*
 * bandwidth get ingress ifg
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_ifg(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    ifg_include = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* show all igr-bandwidth info */
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &ifg_include), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_get(unit , RATE_MODULE_IGR, &ifg_include), ret);
    }
    if (ENABLED == ifg_include)
    {
        diag_util_mprintf("Ingress Bandwidth : Include IFG\n");
    }
    else
    {
        diag_util_mprintf("Ingress Bandwidth : Exclude IFG\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_ingress_ifg */
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_BYPASS_PACKET_ARP_REQUEST_RMA_BPDU_RTK_CTRL_PKT_IGMP_STATE
/*
 * bandwidth get ingress bypass-packet ( arp-request | rma | bpdu | rtk-ctrl-pkt | igmp ) state
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_bypass_packet_arp_request_rma_bpdu_rtk_ctrl_pkt_igmp_state(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_RMA, &enable), ret);
        diag_util_mprintf("RMA Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('p' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_BPDU, &enable), ret);
        diag_util_mprintf("BPDU Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('t' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_RTKPKT, &enable), ret);
        diag_util_mprintf("RTK Control Packet Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('g' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_IGMP, &enable), ret);
        diag_util_mprintf("IGMP Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('r' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_ARPREQ, &enable), ret);
        diag_util_mprintf("ARP Request Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_BYPASS_PACKET_RIP_DHCP_STATE
/*
 * bandwidth get ingress bypass-packet ( rip | dhcp ) state
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_bypass_packet_rip_dhcp_state(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_RIP, &enable), ret);
        diag_util_mprintf("RIP Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('h' == TOKEN_CHAR(4,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_get(unit, IGR_BYPASS_DHCP, &enable), ret);
        diag_util_mprintf("DHCP Bypass Ingress Bandwidth Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_BURST_SIZE
/*
 * bandwidth get ingress burst size
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_burst_size(cparser_context_t *context)
{
    uint32      unit = 0, burst_size;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrPortBwCtrlBurstSize_get(unit, &burst_size), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBwCtrlBurstSize_get(unit, &burst_size), ret);
    }

    diag_util_mprintf("Ingress Bandwidth Burst Size : %d (0x%x)\n", burst_size, burst_size);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_FLOW_CONTROL_PORT_PORTS_ALL_STATE
/*
 * bandwidth get ingress flow-control port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_flow_control_port_ports_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("Flow Control Status of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwFlowctrlEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("Port %2d : %s\n", port, (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_ingress_flow_control_port_all */
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_FLOW_CONTROL_QUEUE_QUEUE_ID_DROP_THRESHOLD_HIGH_LOW
/*
 * bandwidth get ingress flow-control queue <UINT:queue_id> drop-threshold ( high | low )
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_flow_control_queue_queue_id_drop_threshold_high_low(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_get(unit, *queue_id_ptr, &thresh), ret);
    diag_util_mprintf("Ingress Queue %d Flow Control Threshold\n", *queue_id_ptr);
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

#ifdef CMD_BANDWIDTH_GET_INGRESS_FLOW_CONTROL_LOW_THRESHOLD
/*
 * bandwidth get ingress flow-control low-threshold
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_flow_control_low_threshold(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthLowThresh_get(unit, &thresh), ret);
    diag_util_mprintf("Flow Control Turn ON/OFF Low Threshold of System : %d (0x%x)\n", thresh, thresh);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_FLOW_CONTROL_HIGH_THRESHOLD_PORT_PORTS_ALL
/*
 * bandwidth get ingress flow-control high-threshold port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_flow_control_high_threshold_port_ports_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    uint32          thresh;
    diag_portlist_t portlist;
    rtk_rate_igrBwBurst_cfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Flow Control Turn ON/OFF High Threshold of Ports \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if(IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthHighThresh_get(unit, port, &thresh), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg), ret);
        thresh = cfg.burst_high;

    }
        diag_util_mprintf("Port %2d : %d (0x%x)\n", port, thresh, thresh);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_PORT_PORTS_ALL
/*
 * bandwidth get ingress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_port_ports_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          isExceed, rate;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;
    rtk_rate_igrBwBurst_cfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d : \n", port);
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlEnable_get(unit, port, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("\tIngress Bandwidth : Enable\n");
        }
        else
        {
            diag_util_mprintf("\tIngress Bandwidth : Disable\n");
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlRate_get(unit, port, &rate), ret);

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg), ret);
        }

        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_igrPortBwCtrlBurstSize_get(unit, &cfg.burst_high), ret);
        }


        diag_util_mprintf("\tRate : %d (0x%x)\n", rate, rate);

        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID))
        {
            diag_util_mprintf("\tBurst Size : %d (0x%x)\n", cfg.burst_high, cfg.burst_high);
        }

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf("\tBurst Size low: %d (0x%x)\n", cfg.burst_low, cfg.burst_low);
            diag_util_mprintf("\tBurst Size high: %d (0x%x)\n", cfg.burst_high, cfg.burst_high);
        }


        if (RT_ERR_OK == rtk_rate_portIgrBandwidthCtrlExceed_get(unit, port, &isExceed))
        {
            diag_util_mprintf("\tExceed Flag : %s\n", (TRUE == isExceed)?"  Exceed  ":" Not-exceed ");
        }
        else
        {
            diag_util_mprintf("\tNot Support \n");
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_get_ingress_port_all */
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_FLOW_CONTROL_LOW_THRESHOLD_THRESH
/*
 * bandwidth set ingress flow-control low-threshold <UINT:thresh>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_flow_control_low_threshold_thresh(cparser_context_t *context,
    uint32_t *thresh_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthLowThresh_set(unit, *thresh_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_FLOW_CONTROL_HIGH_THRESHOLD_PORT_PORTS_ALL_THRESH
/*
 * bandwidth set ingress flow-control high-threshold port ( <PORT_LIST:ports> | all ) <UINT:thresh>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_flow_control_high_threshold_port_ports_all_thresh(cparser_context_t *context,
    char **port_ptr,
    uint32_t *thresh_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_igrBwBurst_cfg_t  Cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if(IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthHighThresh_set(unit, port, *thresh_ptr), ret);
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_get(unit, port, &Cfg), ret);
            Cfg.burst_high = *thresh_ptr;
            DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_set(unit, port, &Cfg), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_FLOW_CONTROL_QUEUE_QUEUE_ID_DROP_THRESHOLD_HIGH_LOW_THRESHOLD
/*
 * bandwidth set ingress flow-control queue <UINT:queue_id> drop-threshold ( high | low ) <UINT:threshold>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_flow_control_queue_queue_id_drop_threshold_high_low_threshold(cparser_context_t *context,
    uint32_t *queue_id_ptr,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_get(unit, *queue_id_ptr, &thresh), ret);

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

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_igrQueueDropThresh_set(unit, *queue_id_ptr, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_FLOW_CONTROL_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * bandwidth set ingress flow-control port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_flow_control_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

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

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwFlowctrlEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_ingress_flow_control_port_all_state_disable_enable */
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_IFG_EXCLUDE_INCLUDE
/*
 * bandwidth set ingress ifg ( exclude | include )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_ifg_exclude_include(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    /* set IFG include */
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlIncludeIfg_set(unit, enable), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_set(unit, RATE_MODULE_IGR, enable), ret);
    }
    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_ingress_ifg_exclude_include */
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_ingress_port_all_state_disable_enable */
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_RATE_RATE
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlRate_set(unit, port, *rate_ptr), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_bandwidth_set_ingress_port_all_rate_rate */
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_BURST_SIZE_SIZE
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_burst_size_size(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_igrBwBurst_cfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg), ret);
        cfg.burst_high = *size_ptr;
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_LOW_BURST_SIZE_LOW_HIGH_BURST_SIZE_HIGH
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) low-burst-size <UINT:low> high-burst-size <UINT:high>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_low_burst_size_low_high_burst_size_high(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *low_ptr,
    uint32_t *high_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_igrBwBurst_cfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        cfg.burst_low = *low_ptr;
        cfg.burst_high = *high_ptr;
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_BYPASS_PACKET_ARP_REQUEST_RMA_BPDU_RTK_CTRL_PKT_IGMP_STATE_DISABLE_ENABLE
/*
 * bandwidth set ingress bypass-packet ( arp-request | rma | bpdu | rtk-ctrl-pkt | igmp ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_bypass_packet_arp_request_rma_bpdu_rtk_ctrl_pkt_igmp_state_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;
    rtk_rate_igr_bypassType_t bypassType = IGR_BYPASS_RMA;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_RMA;
    }
    else if ('p' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_BPDU;
    }
    else if ('t' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_RTKPKT;
    }
    else if ('g' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_IGMP;
    }
    else if ('r' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_ARPREQ;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_set(unit, bypassType, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_BYPASS_PACKET_RIP_DHCP_STATE_DISABLE_ENABLE
/*
 * bandwidth set ingress bypass-packet ( rip | dhcp ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_bypass_packet_rip_dhcp_state_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;
    rtk_rate_igr_bypassType_t bypassType = IGR_BYPASS_RMA;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_RIP;
    }
    else if ('h' == TOKEN_CHAR(4,1))
    {
        bypassType = IGR_BYPASS_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_igrBandwidthCtrlBypass_set(unit, bypassType, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_BURST_SIZE_SIZE
/*
 * bandwidth set ingress burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_burst_size_size(cparser_context_t *context,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrPortBwCtrlBurstSize_set(unit, *size_ptr), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_igrBwCtrlBurstSize_set(unit, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_RESET_INGRESS_METER_EXCEED_PORT_PORTS_ALL
/*
 * bandwidth reset ingress meter-exceed port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_bandwidth_reset_ingress_meter_exceed_port_ports_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlExceed_reset(unit, port), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID
/*
 * bandwidth get ingress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id>
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_port_ports_all_queue_queue_id(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_enable_t    enable = DISABLED;
    diag_portlist_t portlist;
    uint32 exceed, rate;
#if defined(CONFIG_SDK_RTL9310)
    uint32 burst;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlEnable_get(unit, port, *queue_id_ptr, &enable), ret);
        if (ENABLED == enable)
        {
            diag_util_mprintf("\tIngress Bandwidth (Queue %d) : Enable\n", *queue_id_ptr);
        }
        else
        {
            diag_util_mprintf("\tIngress Bandwidth (Queue %d) : Disable\n", *queue_id_ptr);
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlRate_get(unit, port, *queue_id_ptr, &rate), ret);
        diag_util_mprintf("\tRate (Queue %d) : %d (0x%x)\n", *queue_id_ptr, rate, rate);

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlBurstSize_get(unit, port, *queue_id_ptr, &burst), ret);
            diag_util_mprintf("\tBurst Size (Queue %d) : %d (0x%x)\n", *queue_id_ptr, burst, burst);
        }
#endif

        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlExceed_get(unit, port, *queue_id_ptr, &exceed), ret);
        diag_util_mprintf("\tExceed Flag(Queue %d) : %s\n", *queue_id_ptr, (TRUE == exceed)?"  Exceed  ":" Not-exceed ");
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_STATE_DISABLE_ENABLE
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_queue_queue_id_state_disable_enable(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t    enable = DISABLED;
    rtk_qid_t       queue_id = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    queue_id = *queue_id_ptr;

    if ('e' == TOKEN_CHAR(8,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlEnable_set(unit, port, queue_id, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_RATE_RATE
/*
* bandwidth set ingress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> rate <UINT:rate>
*/
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_queue_queue_id_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlRate_set(unit, port, *queue_id_ptr, *rate_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_PORT_PORTS_ALL_QUEUE_QUEUE_ID_RATE_RATE
/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id> burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_port_ports_all_queue_queue_id_burst_size_size(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *queue_id_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlBurstSize_set(unit, port, *queue_id_ptr, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_RESET_INGRESS_METER_EXCEED_PORT_PORTS_ALL_QUEUE_QUEUE_ID
/*
* bandwidth reset ingress meter-exceed port ( <PORT_LIST:ports> | all ) queue <UINT:queue_id>
*/
cparser_result_t cparser_cmd_bandwidth_reset_ingress_meter_exceed_port_ports_all_queue_queue_id(cparser_context_t *context,
    char **port_ptr,
    uint32_t *queue_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrQueueBwCtrlExceed_reset(unit, port, *queue_id_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_IFG
/*
 * storm-control get ifg
 */
cparser_result_t cparser_cmd_storm_control_get_ifg(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    ifg_include = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlIncludeIfg_get(unit, &ifg_include), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_get(unit, RATE_MODULE_STORM, &ifg_include), ret);
    }
    if (ENABLED == ifg_include)
    {
        diag_util_mprintf("Storm Control Include IFG\n");
    }
    else
    {
        diag_util_mprintf("Storm Control Exclude IFG\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_storm_control_get_ifg */
#endif

#ifdef CMD_STORM_CONTROL_SET_PROTO_STORM_ARP_REQUEST_DHCP_VLAN_CONSTRT_STATE_DISABLE_ENABLE
/*
 * storm-control set proto-storm ( arp-request | dhcp ) vlan-constrt state ( disable | enable )
 */
cparser_result_t cparser_cmd_storm_control_set_proto_storm_arp_request_dhcp_vlan_constrt_state_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_enable_t enable;
    int32       ret = RT_ERR_FAILED;
    rtk_rate_storm_proto_group_t      type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }
    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_stormCtrlProtoVlanConstrtEnable_set(unit, type, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_PROTO_STORM_ARP_REQUEST_DHCP_VLAN_CONSTRT_STATE
/*
 * storm-control get proto-storm ( arp-request | dhcp ) vlan-constrt state
 */
cparser_result_t cparser_cmd_storm_control_get_proto_storm_arp_request_dhcp_vlan_constrt_state(cparser_context_t *context)
{
    uint32      unit = 0;
    rtk_enable_t enable;
    int32       ret = RT_ERR_FAILED;
    rtk_rate_storm_proto_group_t      type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_stormCtrlProtoVlanConstrtEnable_get(unit, type, &enable), ret);
    if (STORM_PROTO_GROUP_ARP == type)
    {
        diag_util_mprintf("ARP-request Storm Control VLAN Constraint State : %s\n", (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if (STORM_PROTO_GROUP_DHCP == type)
    {
        diag_util_mprintf("DHCP Storm Control VLAN Constraint State : %s\n", (enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_IFG_EXCLUDE_INCLUDE
/*
 * storm-control set ifg ( exclude | include )
 */
cparser_result_t cparser_cmd_storm_control_set_ifg_exclude_include(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(3,0))
    {
        enable = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(3,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    /* set IFG include */
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlIncludeIfg_set(unit, enable), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_includeIfg_set(unit, RATE_MODULE_STORM, enable), ret);
    }
    return CPARSER_OK;
} /* end of cparser_cmd_storm_control_set_ifg_exclude_include */
#endif

#ifdef CMD_STORM_CONTROL_GET_PORT_PORT_ALL_COUNTING_MODE
/*
    storm-control get port ( <PORT_LIST:port> | all ) counting-mode
*/
cparser_result_t cparser_cmd_storm_control_get_port_port_all_counting_mode(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_storm_rateMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlRateMode_get(unit, port, &mode), ret);

        diag_util_mprintf("Port  %2d:  ", port);

        if (BASED_ON_BYTE == mode)
        {
            diag_util_mprintf("Storm Control Counting Mode : Byte\n");
        }
        else
        {
            diag_util_mprintf("Storm Control Counting Mode : Packet\n");
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_PORT_PORT_ALL_COUNTING_MODE_BYTE_PACKET
/*
storm-control set port ( <PORT_LIST:port> | all ) counting-mode ( byte | packet )
*/
cparser_result_t cparser_cmd_storm_control_set_port_port_all_counting_mode_byte_packet(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_storm_rateMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('b' == TOKEN_CHAR(5,0))
    {
        mode = BASED_ON_BYTE;
    }
    else if ('p' == TOKEN_CHAR(5,0))
    {
        mode = BASED_ON_PKT;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlRateMode_set(unit, port, mode), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_BROADCAST_MULTICAST_UNICAST_PORT_PORT_ALL
/*
storm-control get ( broadcast | multicast | unicast ) port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_storm_control_get_broadcast_multicast_unicast_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    uint32      rate = 0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32      burst =0;
#endif
#if defined(CONFIG_SDK_RTL8380)
    uint32      globalBurst = 0;
#endif
    uint32      isExceed = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_storm_group_t      type;
    rtk_rate_storm_sel_t storm_sel;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('b' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else if ('u' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_UNICAST;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }


    diag_util_mprintf(" Port |  State  |  Type  |    Rate    | Burst Size | Meter Exceed\n");
    diag_util_mprintf("=================================================================\n");

#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBurstSize_get(unit, type, &globalBurst), ret);
        }
#endif

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("  %2d  ", port);

        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlEnable_get(unit, port, type, &enable), ret);
        diag_util_mprintf("  %s ", ((enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));

        if (type == STORM_GROUP_BROADCAST)
            diag_util_mprintf("   all  ");
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlTypeSel_get(unit, port, type, &storm_sel), ret);
            diag_util_mprintf("  %s ", (STORM_SEL_UNKNOWN == storm_sel)?"unknown":"    all");
        }

        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlRate_get(unit, port, type, &rate), ret);
        diag_util_mprintf("  %10d ", rate);

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlBurstSize_get(unit, port, type, &burst), ret);
            diag_util_mprintf("  %10d ", burst);
        }
#endif

#if defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("  %10d ", globalBurst);
        }
#endif

        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlExceed_get(unit, port, type, &isExceed), ret);
        diag_util_mprintf("  %s ", (TRUE == isExceed)?"  Exceed  ":" Not-exceed ");

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_BROADCAST_MULTICAST_UNICAST_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/*
storm-control set ( broadcast | multicast | unicast ) port ( <PORT_LIST:port> | all ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_storm_control_set_broadcast_multicast_unicast_port_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_group_t  type;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('b' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else
    {
        type = STORM_GROUP_UNICAST;
    }

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlEnable_set(unit, port, type, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_BROADCAST_MULTICAST_UNICAST_PORT_PORT_ALL_RATE_RATE
/*
storm-control set ( broadcast | multicast | unicast ) port ( <PORT_LIST:port> | all ) rate <UINT:rate>
*/
cparser_result_t cparser_cmd_storm_control_set_broadcast_multicast_unicast_port_port_all_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_group_t  type;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('b' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else
    {
        type = STORM_GROUP_UNICAST;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlRate_set(unit, port, type, *rate_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_BROADCAST_MULTICAST_UNICAST_PORT_PORT_ALL_BURST_SIZE_SIZE
/*
storm-control set ( broadcast | multicast | unicast ) port ( <PORT_LIST:port> | all ) burst-size <UINT:size>
*/
cparser_result_t cparser_cmd_storm_control_set_broadcast_multicast_unicast_port_port_all_burst_size_size(cparser_context_t *context,
    char **port_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_group_t  type;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('b' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else
    {
        type = STORM_GROUP_UNICAST;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlBurstSize_set(unit, port, type, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_RESET_METER_EXCEED_BROADCAST_MULTICAST_UNICAST_PORT_PORT_ALL
/*
storm-control reset meter-exceed ( broadcast | multicast | unicast ) port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_storm_control_reset_meter_exceed_broadcast_multicast_unicast_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_storm_group_t      type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('b' == TOKEN_CHAR(3,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(3,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
        type = STORM_GROUP_UNICAST;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlExceed_reset(unit, port, type), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_UNICAST_MULTICAST_PORT_PORT_ALL_TYPE_UNKNOWN_ONLY_BOTH
/*
storm-control set ( unicast | multicast ) port ( <PORT_LIST:port> | all ) type ( unknown-only | both )
*/
cparser_result_t cparser_cmd_storm_control_set_unicast_multicast_port_port_all_type_unknown_only_both(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_group_t  type;
    rtk_rate_storm_sel_t    sel;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('u' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_UNICAST;
    }
    else
    {
        type = STORM_GROUP_MULTICAST;
    }

    if ('b' == TOKEN_CHAR(6,0))
    {
        sel = STORM_SEL_UNKNOWN_AND_KNOWN;
    }
    else
    {
        sel = STORM_SEL_UNKNOWN;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlTypeSel_set(unit, port, type, sel), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_BYPASS_PACKET_ARP_REQUEST_BPDU_IGMP_RMA_RTK_CTRL_PKT_DHCP_RIP_OSPF_STATE
/*
storm-control get bypass-packet ( arp-request | bpdu | igmp | rma | rtk-ctrl-pkt | dhcp | rip-ospf ) state
*/
cparser_result_t cparser_cmd_storm_control_get_bypass_packet_arp_request_bpdu_igmp_rma_rtk_ctrl_pkt_dhcp_rip_ospf_state(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_RMA, &enable), ret);
        diag_util_mprintf("RMA Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('r' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_ARP, &enable), ret);
        diag_util_mprintf("ARP Request Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('p' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_BPDU, &enable), ret);
        diag_util_mprintf("BPDU Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('t' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_RTKPKT, &enable), ret);
        diag_util_mprintf("RTK Control Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('g' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_IGMP, &enable), ret);
        diag_util_mprintf("IGMP Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('h' == TOKEN_CHAR(3,1))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Packet Bypass Storm Control State : %s\n", "Not Support");
            return CPARSER_OK;
        }
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_DHCP, &enable), ret);
        diag_util_mprintf("DHCP Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('i' == TOKEN_CHAR(3,1))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("RIP-OSPF Packet Bypass Storm Control State : %s\n", "Not Support");
            return CPARSER_OK;
        }
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_get(unit, STORM_BYPASS_RIP, &enable), ret);
        diag_util_mprintf("RIP-OSPF Packet Bypass Storm Control State : %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_BYPASS_PACKET_ARP_REQUEST_BPDU_IGMP_RMA_RTK_CTRL_PKT_DHCP_RIP_OSPF_STATE_DISABLE_ENABLE
/*
storm-control set bypass-packet ( arp-request | bpdu | igmp | rma | rtk-ctrl-pkt | dhcp | rip-ospf ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_storm_control_set_bypass_packet_arp_request_bpdu_igmp_rma_rtk_ctrl_pkt_dhcp_rip_ospf_state_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;
    rtk_rate_storm_bypassType_t bypassType = IGR_BYPASS_RMA;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(3,1))
    {
        bypassType = STORM_BYPASS_RMA;
    }
    else if ('r' == TOKEN_CHAR(3,1))
    {
        bypassType = STORM_BYPASS_ARP;
    }
    else if ('p' == TOKEN_CHAR(3,1))
    {
        bypassType = STORM_BYPASS_BPDU;
    }
    else if ('t' == TOKEN_CHAR(3,1))
    {
        bypassType = STORM_BYPASS_RTKPKT;
    }
    else if ('g' == TOKEN_CHAR(3,1))
    {
        bypassType = STORM_BYPASS_IGMP;
    }
    else if ('h' == TOKEN_CHAR(3,1))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Packet Bypass Storm Control State : %s\n", "Not Support");
            return CPARSER_OK;
        }
        bypassType = STORM_BYPASS_DHCP;
    }
    else if ('i' == TOKEN_CHAR(3,1))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("RIP-OSPF Packet Bypass Storm Control State : %s\n", "Not Support");
            return CPARSER_OK;
        }
        bypassType = STORM_BYPASS_RIP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

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
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBypass_set(unit, bypassType, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_PROTO_STORM_ARP_REQUEST_BPDU_IGMP_DHCP_PORT_PORT_ALL
/*
storm-control get proto-storm ( arp-request | bpdu | igmp | dhcp ) port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_storm_control_get_proto_storm_arp_request_bpdu_igmp_dhcp_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    uint32      rate = 0;
    uint32      isExceed = 0;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32      burst = 0;
    rtk_enable_t enable;
#endif
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_rate_storm_proto_group_t      type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_BPDU;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_IGMP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Protocol Storm Control : %s\n", "Not Support");
            return CPARSER_OK;
        }
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf(" Port |  State  |  Rate      | Burst Size | Meter Exceed\n");
        diag_util_mprintf("============================================================\n");

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoEnable_get(unit, port, type, &enable), ret);
            diag_util_mprintf("  %2d  ", port);
            diag_util_mprintf("  %s ", ((enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));

            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoRate_get(unit, port, type, &rate), ret);
            diag_util_mprintf("  %10d ", rate);

            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoBurstSize_get(unit, port, type, &burst), ret);
            diag_util_mprintf("  %10d ", burst);

            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoExceed_get(unit, port, type, &isExceed), ret);
            diag_util_mprintf("  %s ", (TRUE == isExceed)?"  Exceed  ":" Not-exceed ");

            diag_util_mprintf("\n");
        }
    }
#endif /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf(" Port |   Rate   | Meter Exceed\n");
        diag_util_mprintf("================================\n");

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoRate_get(unit, port, type, &rate), ret);
            diag_util_mprintf("  %2d  ", port);
            diag_util_mprintf("  %8d ", rate);

            DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoExceed_get(unit, port, type, &isExceed), ret);
            diag_util_mprintf("  %s ", (TRUE == isExceed)?"  Exceed  ":" Not-exceed ");

            diag_util_mprintf("\n");
        }
   }
#endif /* defined(CONFIG_SDK_RTL8390) */

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_PROTO_STORM_ARP_REQUEST_BPDU_IGMP_DHCP_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/*
storm-control set proto-storm ( arp-request | bpdu | igmp | dhcp ) port ( <PORT_LIST:port> | all ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_storm_control_set_proto_storm_arp_request_bpdu_igmp_dhcp_port_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_proto_group_t      type;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_BPDU;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_IGMP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoEnable_set(unit, port, type, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_PROTO_STORM_ARP_REQUEST_BPDU_IGMP_DHCP_PORT_PORT_ALL_RATE_RATE
/*
storm-control set proto-storm ( arp-request | bpdu | igmp | dhcp ) port ( <PORT_LIST:port> | all ) rate <UINT:rate>
*/
cparser_result_t cparser_cmd_storm_control_set_proto_storm_arp_request_bpdu_igmp_dhcp_port_port_all_rate_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *rate_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_proto_group_t  type;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_BPDU;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_IGMP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Protocol Storm Control : %s\n", "Not Support");
            return CPARSER_OK;
        }
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoRate_set(unit, port, type, *rate_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_SET_PROTO_STORM_ARP_REQUEST_BPDU_IGMP_DHCP_PORT_PORT_ALL_BURST_SIZE_SIZE
/*
storm-control set proto-storm ( arp-request | bpdu | igmp | dhcp ) port ( <PORT_LIST:port> | all ) burst-size <UINT:size>
*/
cparser_result_t cparser_cmd_storm_control_set_proto_storm_arp_request_bpdu_igmp_dhcp_port_port_all_burst_size_size(cparser_context_t *context,
    char **port_ptr,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_proto_group_t  type;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('a' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_BPDU;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        type = STORM_PROTO_GROUP_IGMP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Protocol Storm Control : %s\n", "Not Support");
            return CPARSER_OK;
        }
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoBurstSize_set(unit, port, type, *size_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_RESET_PROTO_STORM_METER_EXCEED_ARP_REQUEST_BPDU_IGMP_DHCP_PORT_PORT_ALL
/*
storm-control reset proto-storm meter-exceed ( arp-request | bpdu | igmp | dhcp ) port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_storm_control_reset_proto_storm_meter_exceed_arp_request_bpdu_igmp_dhcp_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_rate_storm_proto_group_t  type;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if ('a' == TOKEN_CHAR(4,0))
    {
        type = STORM_PROTO_GROUP_ARP;
    }
    else if ('b' == TOKEN_CHAR(4,0))
    {
        type = STORM_PROTO_GROUP_BPDU;
    }
    else if ('i' == TOKEN_CHAR(4,0))
    {
        type = STORM_PROTO_GROUP_IGMP;
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("DHCP Protocol Storm Control : %s\n", "Not Support");
            return CPARSER_OK;
        }
        type = STORM_PROTO_GROUP_DHCP;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_portStormCtrlProtoExceed_reset(unit, port, type), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_STORM_CONTROL_GET_COUNTING_MODE
/*
 * storm-control get counting-mode
 */
cparser_result_t cparser_cmd_storm_control_get_counting_mode(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_rate_storm_rateMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlRateMode_get(unit, &mode), ret);
    if (BASED_ON_BYTE == mode)
    {
        diag_util_mprintf("Storm Control Counting Mode : Byte\n");
    }
    else
    {
        diag_util_mprintf("Storm Control Counting Mode : Packet\n");
    }

    return CPARSER_OK;
}/* end of cparser_cmd_storm_control_get_counting_mode */
#endif

#ifdef CMD_STORM_CONTROL_SET_COUNTING_MODE_BYTE_PACKET
/*
 * storm-control set counting-mode ( byte | packet )
 */
cparser_result_t cparser_cmd_storm_control_set_counting_mode_byte_packet(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_rate_storm_rateMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(3,0))
    {
        mode = BASED_ON_BYTE;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        mode = BASED_ON_PKT;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    /* set storm counting mode */
    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlRateMode_set(unit, mode), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_storm_control_set_counting_mode_byte_packet */
#endif

#ifdef CMD_STORM_CONTROL_SET_BROADCAST_MULTICAST_UNICAST_BURST_SIZE_SIZE
/*
*storm-control set ( broadcast | multicast | unicast ) burst-size <UINT:size>
*/
cparser_result_t cparser_cmd_storm_control_set_broadcast_multicast_unicast_burst_size_size(cparser_context_t *context,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    uint32      rate = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_rate_storm_group_t type = STORM_GROUP_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_BROADCAST;
    }
    else if ('m' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_MULTICAST;
    }
    else if ('u' == TOKEN_CHAR(2,0))
    {
        type = STORM_GROUP_UNICAST;
    }

    rate = *size_ptr;
    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlBurstSize_set(unit, type, rate), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_GET_EGRESS_QUEUE_BURST_SIZE
/*
 * bandwidth get egress-queue burst-size
 */
cparser_result_t cparser_cmd_bandwidth_get_egress_queue_burst_size(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          burst_size;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlBurstSize_get(unit, &burst_size), ret);
    diag_util_mprintf("Egress Queue Burst Size(Bytes) : %d\n", burst_size);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_EGRESS_QUEUE_BURST_SIZE_SIZE
/*
 * bandwidth set egress-queue burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_egress_queue_burst_size_size(cparser_context_t *context,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlBurstSize_set(unit, *size_ptr), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_BANDWIDTH_GET_INGRESS_QUEUE_BURST_SIZE
/*
 * bandwidth get ingress-queue burst-size
 */
cparser_result_t cparser_cmd_bandwidth_get_ingress_queue_burst_size(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          burst_size;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_igrQueueBwCtrlBurstSize_get(unit, &burst_size), ret);
    diag_util_mprintf("Ingress Queue Burst Size(Bytes) : %d\n", burst_size);

    return CPARSER_OK;
}
#endif

#ifdef CMD_BANDWIDTH_SET_INGRESS_QUEUE_BURST_SIZE_SIZE
/*
 * bandwidth set ingress-queue burst-size <UINT:size>
 */
cparser_result_t cparser_cmd_bandwidth_set_ingress_queue_burst_size_size(cparser_context_t *context,
    uint32_t *size_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_rate_igrQueueBwCtrlBurstSize_set(unit, *size_ptr), ret);

    return CPARSER_OK;
}
#endif






