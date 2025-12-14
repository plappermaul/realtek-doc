/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
#include <sdk/sdk_test.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <flag/cmd_flag_apollo.h>
/*
 * sdk test group <STRING:item>
 */
cparser_result_t cparser_cmd_sdk_test_group_item(cparser_context_t *context,
    char **item_ptr)
{
#if defined(CONFIG_LUNA_G3_SERIES)
    //LUNA G3 not support sdktest_run
    return CPARSER_OK;
#else
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_CHIP_ID(unit);

    DIAG_UTIL_ERR_CHK(sdktest_run(unit, *item_ptr), ret);
    return CPARSER_OK;
#endif
} /* end of cparser_cmd_sdk_test_group_item */

/*
 * sdk test case_id <UINT:start> { <UINT:end> }
 */
cparser_result_t cparser_cmd_sdk_test_case_id_start_end(cparser_context_t *context,
    uint32_t *start_ptr, uint32_t *end_ptr)
{
#if defined(CONFIG_LUNA_G3_SERIES)
    //LUNA G3 not support sdktest_run_id
    return CPARSER_OK;
#else
    uint32  unit = 0;
    uint32  start = 0, end = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_CHIP_ID(unit);

    if (5 == TOKEN_NUM())
    {
        start = *start_ptr;
        end = *end_ptr;
    }
    else if (4 == TOKEN_NUM())
    {
        start = *start_ptr;
        end = start;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(sdktest_run_id(unit, start, end), ret);
    return CPARSER_OK;
#endif
} /* end of cparser_cmd_sdk_test_case_id_start_end */
