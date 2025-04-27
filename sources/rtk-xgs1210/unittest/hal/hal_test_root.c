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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of HAL test APIs in the SDK
 *
 * Feature : SDK HAL test APIs
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <hal/hal_test_case.h>
#include <hal/hal_test_root.h>

/*
 * Symbol Definition
 */
#define HAL_TEST_ACTION_CHK(action) \
    do { if ((action < SDKTEST_ACTION_HAL_ALL) || (action >= SDKTEST_ACTION_HAL_END)) \
            { return RT_ERR_FAILED; } } while(0)

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      haltest_run
 * Description:
 *      Test one test case or group test cases in the HAL for one specified device.
 * Input:
 *      unit   - unit id
 *      action - action id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32 haltest_run(uint32 unit, sdktest_action_t action)
{
    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Start to run HAL test cases.... (action:%d)\n", __FUNCTION__, action);

    HAL_TEST_ACTION_CHK(action);

    if ((SDKTEST_ACTION_HAL_COMBO_SET1 == action) || (SDKTEST_ACTION_HAL_ALL == action))
    {
        testComboMedia_1(0x00020004);
    }

    if ((SDKTEST_ACTION_HAL_COMBO_SET2 == action) || (SDKTEST_ACTION_HAL_ALL == action))
    {
        testComboMedia_2(0x00020005);
    }

    if ((SDKTEST_ACTION_HAL_COMBO_GET == action) || (SDKTEST_ACTION_HAL_ALL == action))
    {
        testComboMedia_3(0x00020006);
    }

    if ((SDKTEST_ACTION_HAL_REG_SCAN == action) || (SDKTEST_ACTION_HAL_ALL == action))
    {
        testRegField_check(0x00020007);
    }

    if ((SDKTEST_ACTION_HAL_REG_DEFAULT == action) || (SDKTEST_ACTION_HAL_ALL == action))
    {
        testRegDefaultVal_check(0x00020008);
    }

    if (SDKTEST_ACTION_HAL_ALL == action)
    {
        testPort_1(0x00020001);
        testHal_1(0x00020002);
        testHal_2(0x00020003);
    }

    return RT_ERR_OK;
}
