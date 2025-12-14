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
 * Purpose : Definition of common unittest utility in the SDK
 *
 * Feature : common unittest utility
 *
 */
 
/*
 * Include Files
 */
//#include <osal/osal_test_case.h> 
//#include <dev_config.h>
//#include <soc/soc.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
//#include <osal/cache.h>
//#include <osal/isr.h>
//#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
//#include <osal/thread.h>
//#include <osal/time.h>
#include <common/unittest_util.h>

#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#include <linux/random.h>
#endif

/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#define RAND_MAX  2147483647        /* 0x7FFFFFFF */
#endif


/*
 * Data Declaration
 */
int32 unit_test_mode = TEST_SCAN_MODE;

/*
 * Macro
 */

/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
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


static uint32 isGetchipId = 0;
static uint32 keepChipId = 0xFFFFFFFF;

uint32 unittest_util_chip_id_get(void)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;
    if(0 == isGetchipId)
    {
    	isGetchipId = 1;
    	if (( ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != RT_ERR_OK )
        {
            keepChipId = 0xFFFFFFFF;
            return 0xFFFFFFFF;
        }    
        keepChipId = chipId;
        return chipId;
    }
    else
    {
        return keepChipId;
    }
}
