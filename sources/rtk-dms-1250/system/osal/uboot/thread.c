/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 86145 $
 * $Date: 2018-02-09 16:05:06 +0800 (Fri, 09 Feb 2018) $
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

#include <common/debug/rt_log.h>
#include <osal/thread.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
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
osal_thread_t
osal_thread_create(char *pName, int stack_size, int thread_pri, void (f)(void *), void *pArg)
{
    return RT_ERR_OK;
} /* end of osal_thread_create */


/* Function Name:
 *      osal_thread_destroy
 * Description:
 *      Destroy the thread
 * Input:
 *      thread - thread id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
osal_thread_destroy(osal_thread_t thread_id)
{
    return RT_ERR_OK;
} /* end of osal_thread_destroy */

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
void
osal_thread_exit(int32 ret_code)
{

    return;

} /* end of osal_thread_exit */

