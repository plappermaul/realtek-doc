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
 * $Revision: 6401 $
 * $Date: 2009-10-14 16:03:12 +0800 (Wed, 14 Oct 2009) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : thread relative API
 *
 */

#ifndef __IGMP_THREAD_H__
#define __IGMP_THREAD_H__


/*
 * Include Files
 */
#include <sys_def.h>

/*
 * Symbol Definition
 */

/*
 * Macro Definition
 */
#define STACK_SIZE 32768
/*
 * Function Declaration
 */

/* Function Name:
 *      igmp_thread_create
 * Description:
 *      Create a thread for running the main routine
 * Input:
 *      pName      - name of thread
 *      stack_size - stack size of thread
 *      thread_pri - priority of thread
 *      f          - main routine
 *      pArg       - input argument of the main routine
 * Output:
 *      None
 * Return:
 *      thread id
 * Note:
 *      Linux - stack_size is currently using kernel default value.
 */
extern osal_thread_t
igmp_thread_create(
    char    *pName,
    int     stack_size,
    int     thread_pri,
    void    (f)(void *),
    void    *pArg);


#endif /* __IGMP_THREAD_H__ */

