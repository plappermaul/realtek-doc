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
 * Purpose : Definition those sdk test command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) sdk test
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <drv/tc/tc.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_MODEL_MODE
#include <virtualmac/vmac_target.h>
#endif

/*
 * tc test case_id <UINT:start> { <UINT:end> }
 */
cparser_result_t cparser_cmd_tc_test_case_id_start_end(cparser_context_t *context,
    uint32_t *start_ptr, uint32_t *end_ptr)
{
#ifdef CONFIG_SDK_MODEL_MODE
    uint32  unit = 0;
    uint32  start = 0, end = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (5 == TOKEN_NUM)
    {
        start = *start_ptr;
        end = *end_ptr;
    }
    else if (4 == TOKEN_NUM)
    {
        start = *start_ptr;
        end = start;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(tc_exec(start, end), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_tc_test_case_id_start_end */

/*
 * var get ICType
 */
cparser_result_t cparser_cmd_var_get_ICType(cparser_context_t *context)
{
#ifdef CONFIG_SDK_MODEL_MODE
    enum IC_TYPE type = 0;
    vmac_getTarget(&type);
    diag_util_printf("gCurICType: %d\n", type);
#endif
    return CPARSER_OK;
}
/*
 * var set ICType <UINT:value>
 */
cparser_result_t cparser_cmd_var_get_RegType(cparser_context_t *context)
{
#ifdef CONFIG_SDK_MODEL_MODE
    enum REG_ACCESS_TYPE type = 0;
    vmac_getRegAccessType(&type);
    diag_util_printf("gCurRegAccessType: %d\n", type);
#endif
    return CPARSER_OK;
}
/*
 * var get RegType
 */
cparser_result_t cparser_cmd_var_set_ICType_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
#ifdef CONFIG_SDK_MODEL_MODE
    enum IC_TYPE type;
    type = (enum IC_TYPE)(*value_ptr);
    vmac_setTarget(type);
#endif
    return CPARSER_OK;
}
/*
 * var set RegType <UINT:value>
 */
cparser_result_t cparser_cmd_var_set_RegType_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
#ifdef CONFIG_SDK_MODEL_MODE
    enum REG_ACCESS_TYPE type;
    type = (enum REG_ACCESS_TYPE)(*value_ptr);
    vmac_setRegAccessType(type);
#endif
    return CPARSER_OK;
}





