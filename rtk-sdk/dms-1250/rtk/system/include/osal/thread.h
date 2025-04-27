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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : thread relative API
 *
 */

#ifndef __OSAL_THREAD_H__
#define __OSAL_THREAD_H__


/*
 * Include Files
 */
#include <common/type.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef int32 osal_thread_t;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_thread_create
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
osal_thread_create(
    char    *pName,
    int     stack_size,
    int     thread_pri,
    void    (f)(void *),
    void    *pArg);


/* Function Name:
 *      osal_thread_destroy
 * Description:
 *      Destroy the thread
 * Input:
 *      thread_id - thread id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
osal_thread_destroy(osal_thread_t thread_id);


/* Function Name:
 *      osal_thread_self
 * Description:
 *      Return thread id of caller
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      thread id
 * Note:
 *      None
 */
extern osal_thread_t
osal_thread_self(void);


/* Function Name:
 *      osal_thread_name
 * Description:
 *      Return thread name
 * Input:
 *      thread_id - thread id
 *      size      - size of thread_name buffer
 * Output:
 *      pThread_name  - buffer to return thread name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The return thread name as following:
 *      - NULL, if thread name is not available
 *      - others, if thread name is available
 */
extern int32
osal_thread_name(osal_thread_t thread_id, char *pThread_name, uint32 size);


/* Function Name:
 *      osal_thread_exit
 * Description:
 *      Exit the calling thread
 * Input:
 *      ret_code - return code from thread.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_thread_exit(int32 ret_code);

#endif /* __OSAL_THREAD_H__ */

