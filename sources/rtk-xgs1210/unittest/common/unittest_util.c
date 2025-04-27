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
 * $Revision: 80167 $
 * $Date: 2017-06-28 16:18:46 +0800 (Wed, 28 Jun 2017) $
 *
 * Purpose : Definition of common unittest utility in the SDK
 *
 * Feature : common unittest utility
 *
 */

/*
 * Include Files
 */
//#include <osal/osal_test_case.h>
#include <dev_config.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/cache.h>
#include <osal/isr.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <common/unittest_util.h>
#include <hal/common/halctrl.h>


#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
#include <linux/random.h>
#endif

/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
#define RAND_MAX  2147483647        /* 0x7FFFFFFF */
#endif


/*
 * Data Declaration
 */
int32 unit_test_mode = TEST_SCAN_MODE; /* TEST_SCAN_MODE, TEST_NORMAL_MODE */

/*
 * Macro
 */

/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
int rand(void)
{
    /* implementation 1: get a random word but it looks like kernel doesn't export this symbol */
    // return (secure_ip_id(current->pid + jiffies) & RAND_MAX);

    /* implementation 2: use exported symbol in kernel module */
    int x;
    get_random_bytes(&x, sizeof(x));
    return (x & RAND_MAX);
}
#endif

int32 sdktest_mode_get(int32 *pMode)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    *pMode = unit_test_mode;
    return RT_ERR_OK;
} /* end of sdktest_mode_get */

int32 sdktest_mode_set(int32 mode)
{
    /* parameter check */
    RT_PARAM_CHK((TEST_SCAN_MODE != mode) && (TEST_NORMAL_MODE != mode), RT_ERR_INPUT);

    unit_test_mode = mode;
    return RT_ERR_OK;
} /* end of sdktest_mode_set */

void
UNITTEST_RANDOM_PORTMASK_ASSIGN(uint32 unit,int32 index, rtk_portmask_t *portmask)
{
    osal_memset(portmask, 0, sizeof(rtk_portmask_t));
    HWP_GET_ALL_PORTMASK(unit, *portmask);

    if(HWP_8390_50_FAMILY(unit))
    {

        if((index % 2) == 0)
            portmask->bits[0] = (portmask->bits[0]) & (ut_rand()& BITMASK_32B);
        else
            portmask->bits[1] = (portmask->bits[1]) & (ut_rand()& BITMASK_21B);
    }
    else if(HWP_8380_30_FAMILY(unit))
    {
        portmask->bits[0] = (portmask->bits[0]) & (ut_rand() & BITMASK_29B);
    }
}
