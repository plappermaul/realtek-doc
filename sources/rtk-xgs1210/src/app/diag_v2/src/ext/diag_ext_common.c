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
 * $Revision: 96893 $
 * $Date: 2019-05-15 14:59:04 +0800 (Wed, 15 May 2019) $
 *
 * Purpose : Definition those extension command and APIs in the SDK diagnostic shell.
 *
 */

/*
 * Include Files
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_MODEL_MODE
#include <virtualmac/vmac_target.h>
#endif  /* CONFIG_SDK_MODEL_MODE */

#ifdef CONFIG_SDK_RTL9310
#include <rtdrv/ext/rtdrv_netfilter_ext_9310.h>
#endif  /* CONFIG_SDK_RTL9310 */

#ifdef CONFIG_SDK_RTL9300
#include <rtdrv/ext/rtdrv_netfilter_ext_9300.h>
//#include <tc.h>
#endif  /* CONFIG_SDK_RTL9300 */


/*
 * model ( ic-only | model-only | both ) <UINT:start> <UINT:end>
 */
cparser_result_t
cparser_cmd_model_ic_only_model_only_both_start_end(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr)
{
#ifdef CONFIG_SDK_MODEL_MODE
    rtdrv_ext_modelCfg_t  modelTest_cfg;
    uint32  master_view_unit;

    DIAG_UTIL_FUNC_INIT(master_view_unit);

    modelTest_cfg.startID = *start_ptr;
    modelTest_cfg.endID = *end_ptr;

    if ('i' == TOKEN_CHAR(1, 0))
        modelTest_cfg.caredType = CARE_TYPE_REAL;
    else if ('m' == TOKEN_CHAR(1, 0))
        modelTest_cfg.caredType = CARE_TYPE_MODEL;
    else
        modelTest_cfg.caredType = CARE_TYPE_BOTH;

    SETSOCKOPT(RTDRV_EXT_MODEL_TEST_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);
#endif
    return CPARSER_OK;

}


#if defined(CONFIG_SDK_RTL9310)
/*
 * model ( ic-only | model-only | both ) <UINT:start> <UINT:end> <UINT:unit>
 */
cparser_result_t
cparser_cmd_model_ic_only_model_only_both_start_end_unit(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr,
    uint32_t  *unit_ptr)
{

#ifdef CONFIG_SDK_MODEL_MODE
    rtdrv_ext_modelCfg_t  modelTest_cfg;
    uint32  master_view_unit;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    modelTest_cfg.startID = *start_ptr;
    modelTest_cfg.endID = *end_ptr;
    modelTest_cfg.unit = *unit_ptr;

    if ('i' == TOKEN_CHAR(1, 0))
        modelTest_cfg.caredType = CARE_TYPE_REAL;
    else if ('m' == TOKEN_CHAR(1, 0))
        modelTest_cfg.caredType = CARE_TYPE_MODEL;
    else
        modelTest_cfg.caredType = CARE_TYPE_BOTH;

    SETSOCKOPT(RTDRV_EXT_MODEL_TEST_UNIT_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);
#endif
    return CPARSER_OK;

}
#endif

#if defined(CONFIG_SDK_RTL9310)
cparser_result_t
cparser_cmd_model_get_dev(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_modelCfg_t  modelTest_cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    modelTest_cfg.unit = unit;
    if (unit == 0)
    {
        diag_util_mprintf("Not support this command in host system.\n Please use 'unit' command to switch to guest system\n");
        return CPARSER_OK;
    }

    GETSOCKOPT(RTDRV_EXT_MODEL_DEV_GET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);
    osal_printf("current model devId:%d\n", modelTest_cfg.devId);

    return CPARSER_OK;

}

/*
 * model set dev <UINT:unit>
 */
cparser_result_t
cparser_cmd_model_set_dev_unit(cparser_context_t *context, uint32_t *unit_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_modelCfg_t  modelTest_cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    modelTest_cfg.unit = unit;
    modelTest_cfg.devId = *unit_ptr;
    if (unit == 0)
    {
        diag_util_mprintf("Not support this command in host system.\n Please use 'unit' command to switch to guest system\n");
        return CPARSER_OK;
    }

    SETSOCKOPT(RTDRV_EXT_MODEL_DEV_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);

    return CPARSER_OK;

}

/*
 * model set topology port-map port <PORT_LIST:ports> mdev <UINT:devID> mport <PORT_LIST:mports>
 */
cparser_result_t
cparser_cmd_model_set_topology_port_map_port_ports_mdev_devID_mport_mports(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *devID_ptr,
    char **mport_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_modelCfg_t  modelTest_cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    modelTest_cfg.unit = unit;
    modelTest_cfg.devId = *devID_ptr;
    if (unit == 0)
    {
        diag_util_mprintf("Not support this command in host system.\n Please use 'unit' command to switch to guest system\n");
        return CPARSER_OK;
    }
    if (*devID_ptr > 2)
    {
        diag_util_mprintf("Invalid input\n");
        return CPARSER_OK;
    }

    diag_util_str2LPortMask(*ports_ptr, &modelTest_cfg.ports);
    diag_util_str2LPortMask(*mport_ptr, &modelTest_cfg.mPorts);
    SETSOCKOPT(RTDRV_EXT_MODEL_PORTMAP_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);

    return CPARSER_OK;

}

/*
 * model set topology device-connect src-dev <UINT:sdevID> port <PORT_LIST:sports> dst-dev <UINT:ddevID> port <PORT_LIST:dports>
 */
cparser_result_t
cparser_cmd_model_set_topology_device_connect_src_dev_sdevID_port_sports_dst_dev_ddevID_port_dports(
    cparser_context_t *context,
    uint32_t *sdevID_ptr,
    char **sports_ptr,
    uint32_t *ddevID_ptr,
    char **dport_ptr)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_modelCfg_t  modelTest_cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    modelTest_cfg.unit  = unit;
    modelTest_cfg.devId = *sdevID_ptr;
    if (unit == 0)
    {
        diag_util_mprintf("Not support this command in host system.\n Please use 'unit' command to switch to guest system\n");
        return CPARSER_OK;
    }
    if ((*sdevID_ptr > 2) || (*ddevID_ptr > 2))
    {
        diag_util_mprintf("Invalid input\n");
        return CPARSER_OK;
    }

    modelTest_cfg.devId = *sdevID_ptr;
    diag_util_str2LPortMask(*sports_ptr, &modelTest_cfg.ports);
    modelTest_cfg.dstDevId = *ddevID_ptr;
    diag_util_str2LPortMask(*dport_ptr, &modelTest_cfg.mPorts);
    SETSOCKOPT(RTDRV_EXT_MODEL_DEV_CONNECT_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);

    return CPARSER_OK;

}

/*
 * model dump topology
 */
cparser_result_t
cparser_cmd_model_dump_topology(cparser_context_t *context)
{
    uint32 unit;
    uint32 master_view_unit;
    rtdrv_ext_modelCfg_t  modelTest_cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);
    DIAG_UTIL_OUTPUT_INIT();

    modelTest_cfg.unit = unit;
    if (unit == 0)
    {
        diag_util_mprintf("Not support this command in host system.\n Please use 'unit' command to switch to guest system\n");
        return CPARSER_OK;
    }

    GETSOCKOPT(RTDRV_EXT_MODEL_TOPO_DUMP, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);

    return CPARSER_OK;

}
#endif

/*
 *  register dump
 */
cparser_result_t cparser_cmd_register_dump(cparser_context_t *context)
{
#if defined(CONFIG_SDK_RTL9300) ||defined(CONFIG_SDK_RTL9310)
    uint32  unit = 0;
    uint32 master_view_unit;
    rtdrv_ext_sdkCfg_t cfg;

    DIAG_UTIL_FUNC_INIT(master_view_unit);
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_REG_DUMP_SET, &cfg, rtdrv_ext_sdkCfg_t, 1);
#endif

    return CPARSER_OK;
}

