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

#ifdef CONFIG_SDK_RTL9300
#include <rtdrv/ext/rtdrv_netfilter_ext_9300.h>
//#include <tc.h>
#endif  /* CONFIG_SDK_RTL9300 */

#ifdef CONFIG_SDK_RTL9310
#include <rtdrv/ext/rtdrv_netfilter_ext_9310.h>
#endif  /* CONFIG_SDK_RTL9310 */


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

