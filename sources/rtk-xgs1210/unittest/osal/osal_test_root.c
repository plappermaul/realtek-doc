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
 * Purpose : Definition of OSAL test APIs in the SDK
 *
 * Feature : SDK OSAL test APIs
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/osal_test_case.h>
#include <osal/osal_test_root.h>

/*
 * Symbol Definition
 */
#define OSAL_TEST_ACTION_CHK(action) \
    do { if ((action < SDKTEST_ACTION_OSAL_ALL) || (action >= SDKTEST_ACTION_OSAL_END)) \
            { return RT_ERR_FAILED; } } while(0)

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      osaltest_run
 * Description:
 *      Test one test case or group test cases in the OSAL for one specified device.
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
int32 osaltest_run(uint32 unit, sdktest_action_t action)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)

    uint32 osal_result = 0;
    int32 result = RT_ERR_FAILED;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Start to run OSAL test cases.... (action:%d)", __FUNCTION__, action);

    OSAL_TEST_ACTION_CHK(action);

    if ((SDKTEST_ACTION_OSAL_MEM == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_memory_cache_test()))
            osal_result |= OSALTEST_MEM_CACHE_OK;;
    }

    if ((SDKTEST_ACTION_OSAL_ISR == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_isr_test()))
            osal_result |= OSALTEST_ISR_OK;
    }

    if ((SDKTEST_ACTION_OSAL_TIME == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_time_test()))
            osal_result |= OSALTEST_TIME_OK;
    }

    if ((SDKTEST_ACTION_OSAL_THREAD == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_thread_test()))
            osal_result |= OSALTEST_THREAD_OK;
    }

    if ((SDKTEST_ACTION_OSAL_MUTEX == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_mutex_test()))
            osal_result |= OSALTEST_MUTEX_OK;
    }

    if ((SDKTEST_ACTION_OSAL_SEM == action) || (SDKTEST_ACTION_OSAL_ALL == action))
    {
        if(RT_ERR_OK == (result = osal_sem_test()))
            osal_result |= OSALTEST_SEMAPHORE_OK;
    }

    if (SDKTEST_ACTION_OSAL_ALL == action)
    {
        /* COMMON ACTION ADD HERE!! */

        /* Result of SDKTEST_ACTION_OSAL_ALL */
        result = (OSALTEST_ALL_OK == (osal_result & OSALTEST_ALL_OK)) ? RT_ERR_OK : RT_ERR_FAILED;

        /* Print Test Result */
        osal_printf("\n===============================================\n");
        osal_printf("OSAL Test Result:\n\n");
        osal_printf("OSAL Memory Cache Test........%s\n", (osal_result & OSALTEST_MEM_CACHE_OK) ? "OK" : "FAILED");
        osal_printf("OSAL ISR Test.................%s\n", (osal_result & OSALTEST_ISR_OK) ? "OK" : "FAILED");
        osal_printf("OSAL Time Test................%s\n", (osal_result & OSALTEST_TIME_OK) ? "OK" : "FAILED");
        osal_printf("OSAL Thread Test..............%s\n", (osal_result & OSALTEST_THREAD_OK) ? "OK" : "FAILED");
        osal_printf("OSAL Mutex Test...............%s\n", (osal_result & OSALTEST_MUTEX_OK) ? "OK" : "FAILED");
        osal_printf("OSAL Semaphore Test...........%s\n", (osal_result & OSALTEST_SEMAPHORE_OK) ? "OK" : "FAILED");
        osal_printf("\n===============================================\n");
    }

    return result;

#else
    return RT_ERR_OK;

#endif

}

