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
#include <sdk/sdk_test.h>
#include <common/unittest_util.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
#endif
#ifdef CMD_SDK_TEST_GROUP_ITEM
/*
 * sdk test group <STRING:item>
 */
cparser_result_t cparser_cmd_sdk_test_group_item(cparser_context_t *context,
    char **item_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(sdktest_run(unit, (uint8 *)*item_ptr), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_sdk_test_group_item */
#endif

#ifdef CMD_SDK_TEST_CASE_ID_START_END
/*
 * sdk test case_id <UINT:start> { <UINT:end> }
 */
cparser_result_t cparser_cmd_sdk_test_case_id_start_end(cparser_context_t *context,
    uint32_t *start_ptr, uint32_t *end_ptr)
{
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

    DIAG_UTIL_ERR_CHK(sdktest_run_id(unit, start, end), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_sdk_test_case_id_start_end */
#endif

#ifdef CMD_SDK_SET_TEST_MODE_NORMAL_SCAN
/*
 * sdk set test-mode ( normal | scan )
 */
cparser_result_t cparser_cmd_sdk_set_test_mode_normal_scan(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('n' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(sdktest_mode_set(TEST_NORMAL_MODE), ret);
    }
    else if ('s' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(sdktest_mode_set(TEST_SCAN_MODE), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
} /* end of cparser_cmd_sdk_set_test_mode_normal_scan */
#endif

#ifdef CMD_SDK_GET_TEST_MODE
/*
 * sdk get test-mode
 */
cparser_result_t cparser_cmd_sdk_get_test_mode(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    int32   mode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(sdktest_mode_get(&mode), ret);

    diag_util_mprintf("SDK Unit-test Mode: %s\n", (TEST_NORMAL_MODE == mode)?"Normal":"Scan");
    return CPARSER_OK;
} /* end of cparser_cmd_sdk_get_test_mode */
#endif
