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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : thread relative API
 *
 */

/*
 * Include Files
 */
#include <osal/thread.h>

osal_thread_t
osal_thread_create(
    char    *pName,
    int     stack_size,
    int     thread_pri,
    void    (f)(void *),
    void    *pArg)
{
    return (osal_thread_t)0;
}
int32
osal_thread_destroy(osal_thread_t thread_id)
{
    return 0;
}

osal_thread_t
osal_thread_self(void)
{
    return (osal_thread_t)0;
}

int32
osal_thread_name(osal_thread_t thread_id, char *pThread_name, uint32 size)
{
    return 0;
}
void
osal_thread_exit(int ret_code)
{
    return;
}

void
osal_thread_sleep(int sec)
{
    return;
}

void
osal_thread_usleep(uint32 usec)
{
    return;
}

void
osal_thread_udelay(uint32 usec)
{
    return;
}

