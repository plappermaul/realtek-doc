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
 * $Revision$
 * $Date$
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
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <common/error.h>
#include <common/debug/rt_log.h>
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <osal/time.h>
#include <osal/memory.h>

/*
 * Symbol Definition
 */
typedef struct uthread_ctrl_s {
    void		*(*f)(void *);
    char		name[16];
    pthread_t	tid;
    void		*pArg;
    struct uthread_ctrl_s *next;
    osal_sem_t  sem;
} uthread_ctrl_t;

/*
 * Data Declaration
 */
static uthread_ctrl_t	*thread_head;


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
static uthread_ctrl_t *_search_thread_byID(osal_thread_t t_id);
#if 0
static void *          _thread_entry_point(uthread_ctrl_t  *pUt_ctrl);
#endif

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
 *      Linux - stack_size is currently using default value.
 */
osal_thread_t
osal_thread_create(char *pName, int stack_size, int thread_pri, void (f)(void *), void *pArg)
{
    pthread_attr_t	attr;
    uthread_ctrl_t	*pUt_ctrl;
    pthread_t		id;

    if (0 != pthread_attr_init(&attr))
    {
        /* thread property configuration error */
        return (osal_thread_t)(uintptr)NULL;
    }

    pthread_attr_setstacksize(&attr, stack_size);


    /* allocate memory */
    if ((pUt_ctrl = (uthread_ctrl_t *)osal_alloc(sizeof (*pUt_ctrl))) == NULL)
    {
        /* memory allocation error. */
        return (osal_thread_t)(uintptr)NULL;
    }
    /* clear the data */
    osal_memset(pUt_ctrl, 0, sizeof(uthread_ctrl_t));

    /* fill up the field in the pUt_ctrl */
    if (NULL != pName)
    {
        if(strlen(pName) < sizeof(pUt_ctrl->name))
        {
            osal_strcpy(pUt_ctrl->name, pName);
        }
        else
        {
            osal_memcpy(pUt_ctrl->name, pName, sizeof(pUt_ctrl->name)-1);
            pUt_ctrl->name[sizeof(pUt_ctrl->name)-1] = '\0';
        }
    }

    pUt_ctrl->f = (void *(*)(void *))f;
    pUt_ctrl->pArg = pArg;
    pUt_ctrl->tid = (pthread_t)0;
    pUt_ctrl->next = thread_head;
    pUt_ctrl->sem = osal_sem_create(0);
    thread_head = pUt_ctrl;

    if (pthread_create(&id, NULL, (void *)f, (void *)pArg))
    {
        thread_head = thread_head->next;
        osal_free(pUt_ctrl);
        return (osal_thread_t)(uintptr)NULL;
    }

    pUt_ctrl->tid = id;

    return ((osal_thread_t)pUt_ctrl->tid);
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
    uthread_ctrl_t	*pUt_ctrl, **ppUt_ctrl;

    if (pthread_kill(thread_id, 0))
    {

        return RT_ERR_FAILED;
    }

    for (ppUt_ctrl = &thread_head; (*ppUt_ctrl) != NULL; ppUt_ctrl = &(*ppUt_ctrl)->next)
    {
        if ((*ppUt_ctrl)->tid == thread_id)
        {
            pUt_ctrl = (*ppUt_ctrl);
            (*ppUt_ctrl) = (*ppUt_ctrl)->next;
            osal_free(pUt_ctrl);
            break;
        }
    }

    return RT_ERR_OK;
} /* end of osal_thread_destroy */


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
osal_thread_t
osal_thread_self(void)
{
    return (osal_thread_t)pthread_self();
} /* end of osal_thread_self */


/* Function Name:
 *      osal_thread_name
 * Description:
 *      Return thread name
 * Input:
 *      thread - thread id
 *      size   - size of thread_name buffer
 * Output:
 *      pThread_name  - buffer to return thread name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
osal_thread_name(osal_thread_t thread_id, char *pThread_name, uint32 size)
{
    uthread_ctrl_t	*pUt_ctrl;

    RT_INTERNAL_PARAM_CHK((NULL == pThread_name), RT_ERR_NULL_POINTER);

    pUt_ctrl = _search_thread_byID(thread_id);

    if (NULL != pUt_ctrl)
    {
        if (strlen(pUt_ctrl->name) >= size)
        {
            return RT_ERR_FAILED;
        }
        else
        {
            osal_strcpy(pThread_name, pUt_ctrl->name);
        }

    }
    else
    {
        pThread_name[0] = '\0';
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of osal_thread_name */


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
    uthread_ctrl_t  *pUt_ctrl, **ppUt_ctrl;
    pthread_t   thread_id;

    thread_id = pthread_self();
    pUt_ctrl = _search_thread_byID(thread_id);

    if (NULL != pUt_ctrl)
    {
        ppUt_ctrl = &pUt_ctrl;
        (*ppUt_ctrl) = (*ppUt_ctrl)->next;
        osal_free(pUt_ctrl);
    }
    else
    {
        return;
    }

    pthread_exit((void *)(uintptr)ret_code);
} /* end of osal_thread_exit */

/* Function Name:
 *      _search_thread_byID
 * Description:
 *      Search for the thread by thread id.
 * Input:
 *      t_id - The searched thread id.
 * Output:
 *      None
 * Return:
 *      NULL   - No thread with the given thread id.
 *      Others - the pointer pointed to uthread_ctrl_t data structure with the given thread id.
 * Note:
 *      Used for internal implementation.
 */
static uthread_ctrl_t *_search_thread_byID(osal_thread_t t_id)
{
    uthread_ctrl_t *pUt_ctrl;

    /* Iterate the whole list to search the thread. */
    for (pUt_ctrl = thread_head; pUt_ctrl != NULL; pUt_ctrl = pUt_ctrl->next)
    {
        if (pUt_ctrl->tid == (pthread_t)t_id)
        {
            return  pUt_ctrl;
        }
    }

    return (uthread_ctrl_t *)NULL;
} /* end of _search_thread_byID */

#if 0
/* Function Name:
 *      _thread_entry_point
 * Description:
 *      The function used to set necessary setting for the thread before runng it.
 * Input:
 *      pUt_ctrl - The pointer points to the uthread_ctrl_t data of the created thread.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      Used for internal implementation.
 */
static void *
_thread_entry_point(uthread_ctrl_t  *pUt_ctrl)
{
    sigset_t    mask, ori_mask;
    void		*(*f)(void *);
    void		*pArg;

    osal_printf("_thread_entry_point\n");
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &ori_mask);

    pthread_detach(pthread_self());

    f = pUt_ctrl->f;
    pArg = pUt_ctrl->pArg;

    /* Setting ti->id notifies parent to continue */

    pUt_ctrl->tid = pthread_self();
    osal_printf("pUt_ctrl->tid=%d, pUt_ctrl=%x\n",pUt_ctrl->tid, pUt_ctrl);
    osal_sem_give(pUt_ctrl->sem);
    return (*f)(pArg);
} /* end of _thread_entry_point */
#endif
