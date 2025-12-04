/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * $Revision: 37178 $
 * $Date: 2013-02-25 17:08:01 +0800 (Mon, 25 Feb 2013) $
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

#include "igmp_thread.h"

/*
 * Symbol Definition
 */
typedef struct uthread_ctrl_s
{
    void                    *(*f)(void *);
    char                    name[16];
    pthread_t               tid;
    void                    *pArg;
    struct uthread_ctrl_s   *next;
    int                     sem;
} uthread_ctrl_t;

/*
 * Data Declaration
 */
static uthread_ctrl_t	*igmp_thread_head;

/* Function Name:
 *     igmp_thread_create
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
 *      Linux - stack_size is currently using default value.
 */
osal_thread_t
igmp_thread_create(char *pName, int stack_size, int thread_pri, void (f)(void *), void *pArg)
{
    pthread_attr_t	attr;
    uthread_ctrl_t	*pUt_ctrl;
    pthread_t		id;
    struct sched_param sched;

    if (0 != pthread_attr_init(&attr))
    {
        /* thread property configuration error */
        return (osal_thread_t)NULL;
    }

    pthread_attr_setstacksize(&attr, stack_size);

    if (!(pUt_ctrl = (uthread_ctrl_t *)osal_alloc(sizeof (*pUt_ctrl))))
    {
        return (osal_thread_t)NULL;
    }

    osal_memset(pUt_ctrl, 0, sizeof(uthread_ctrl_t));

    /* fill up the field in the pUt_ctrl */
    if (NULL != pName)
    {
        if (strlen(pName) < sizeof(pUt_ctrl->name))
        {
            osal_strcpy(pUt_ctrl->name, pName);
        }
        else
        {
            osal_memcpy(pUt_ctrl->name, pName, sizeof(pUt_ctrl->name) - 1);
            pUt_ctrl->name[sizeof(pUt_ctrl->name) - 1] = '\0';
        }
    }

    pUt_ctrl->f         = (void *(*)(void *))f;
    pUt_ctrl->pArg      = pArg;
    pUt_ctrl->tid       = (pthread_t)0;
    pUt_ctrl->next      = igmp_thread_head;
    pUt_ctrl->sem       = (int)osal_sem_create(0);
    igmp_thread_head    = pUt_ctrl;

    if (pthread_create(&id, NULL, (void *)f, (void *)pArg))
    {
    	igmp_thread_head = igmp_thread_head->next;
    	osal_free(pUt_ctrl);
    	return (osal_thread_t)NULL;
    }

    /*set priority*/
    sched.sched_priority = thread_pri;
    if (0 != pthread_setschedparam(id, SCHED_FIFO, &sched))
    {
    	igmp_thread_head = igmp_thread_head->next;
    	osal_free(pUt_ctrl);
		return (osal_thread_t)NULL;
    }

	pUt_ctrl->tid = id;

    return ((osal_thread_t)pUt_ctrl->tid);
} /* end of igmp_thread_create */

