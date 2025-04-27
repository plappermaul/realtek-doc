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
 * Feature : OSAL test utility (OS dependent)
 *
 */

/*
 * Include Files
 */
#include <osal/osal_test_case.h>
#ifdef CONFIG_SDK_KERNEL_LINUX
/*
#include <asm/current.h>
#include <linux/jiffies.h>
*/
#include <linux/random.h>
#endif

/*
 * Symbol Definition
 */
#ifdef CONFIG_SDK_KERNEL_LINUX
#define RAND_MAX  2147483647        /* 0x7FFFFFFF */
#endif

/*
 * Data Declaration
 */

/*
 * Macro
 */

/*
 * Function Declaration
 */
#ifdef CONFIG_SDK_KERNEL_LINUX
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

