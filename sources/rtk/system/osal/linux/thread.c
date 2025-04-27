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
#include <linux/version.h>

#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
#include <linux/smp_lock.h>
#endif
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <linux/kthread.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
#include <linux/sched/signal.h>
#endif
#include <common/debug/rt_log.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/sem.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/* Data structure used to control and record the information of the generated thread.*/
typedef struct kthread_ctrl_s
{
    osal_mutex_t wait_sem;
    struct list_head list;
    struct task_struct *thread;
    struct work_struct wq;
    void (*f)(void *);
    void *pArg;
    char name[16];
    int  priority;
} kthread_ctrl_t;

kthread_ctrl_t * gpKt_ctrl;


/* list head of thread list. */
LIST_HEAD(kthread_list);


/*
 * Macro Definition
 */
#define TO_POSIX_NICE_PRIO(p) ((((p) * 39) / 255) - 20)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
#define DAEMONIZE(a) daemonize(a); \
    allow_signal(SIGKILL);
#else
#define DAEMONIZE(a)    do { \
        allow_signal(SIGKILL);  \
    } while (0)
#endif /* LINUX_VERSION_CODE */

/*
 * Function Declaration
 */

/* Function Name:
 *      search_thread_byID
 * Description:
 *      Search for the thread by thread id.
 * Input:
 *      t_id - The searched thread id.
 * Output:
 *      None
 * Return:
 *      NULL   - No thread with the given thread id.
 *      Others - the pointer pointed to kthread_ctrl_t data structure with the given thread id.
 * Note:
 *      Used for internal implementation.
 */
static kthread_ctrl_t *
search_thread_byID(osal_thread_t t_id)
{
    struct list_head *pH;
    kthread_ctrl_t *pKt_ctrl;

    /* Iterate the whole list to search the thread. */
    list_for_each(pH, &kthread_list)
    {
        pKt_ctrl = list_entry(pH, kthread_ctrl_t, list);

        if (pKt_ctrl->thread->pid == t_id)
        {
            return pKt_ctrl;
        }
    }

    return (kthread_ctrl_t *)NULL;
} /* end of search_thread_byID */

/* Function Name:
 *      thread_entry_point
 * Description:
 *      The function used to set necessary setting for the thread before runng it.
 * Input:
 *      pKt_ctrl - The pointer points to the kthread_ctrl_t data of the created thread.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      Used for internal implementation.
 */
static void
thread_entry_point(kthread_ctrl_t *pKt_ctrl)
{
    /* Daemonize the thread if needed, therefore it won't be affect by its creator. */
    DAEMONIZE(pKt_ctrl->name);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0))
    current->sighand->action[(SIGKILL)-1].sa.sa_handler = SIG_DFL;
#else
    kernel_sigaction(SIGKILL, SIG_DFL);
#endif

    /*
     * Since we can't get thread information before creating it,
     * we complete kthread_ctrl_t data before running it.
     */
    osal_strcpy(current->comm, pKt_ctrl->name);
    current->policy = SCHED_NORMAL;
    set_user_nice(current, TO_POSIX_NICE_PRIO(pKt_ctrl->priority));
    pKt_ctrl->thread = current;

    /* Add kthread_list into pKt_ctrl->list. */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
    lock_kernel();
    list_add(&pKt_ctrl->list, &kthread_list);
    unlock_kernel();
#else
    list_add(&pKt_ctrl->list, &kthread_list);
#endif

    /* Signal the completion of the thread initializaion. */
    osal_sem_give(pKt_ctrl->wait_sem);

    /* Call the real thread function. */
    pKt_ctrl->f(pKt_ctrl->pArg);

    /* Do the jobs for exiting. */
    osal_thread_exit(0);
} /* end of thread_entry_point */

/*
 * For the reason that the child thread will inherent the features of the parent,
 * we switch the thread creation to keventd kernel thread and wait for the wait_sem semaphore
 * for the thread initialization. This might avoid some bothering jobs like closing signals ...
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
    kthread_ctrl_t *pKt_ctrl;

    /* allocate memory */
    pKt_ctrl = osal_alloc(sizeof(kthread_ctrl_t));
    if (NULL == pKt_ctrl)
    {
        /* memory allocation error. */
        return (osal_thread_t)(uintptr)NULL;
    }
    gpKt_ctrl = pKt_ctrl;
    /* clear the data */
    osal_memset(pKt_ctrl, 0, sizeof(kthread_ctrl_t));

    /* fill up the field in the pKt_ctrl */
    if (NULL != pName)
    {
        if (strlen(pName) < sizeof(pKt_ctrl->name))
        {
            osal_strcpy(pKt_ctrl->name, pName);
        }
        else
        {
            osal_memcpy(pKt_ctrl->name, pName, sizeof(pKt_ctrl->name)-1);
            pKt_ctrl->name[sizeof(pKt_ctrl->name)-1] = '\0';
        }
    }
    pKt_ctrl->f = f;
    pKt_ctrl->pArg = pArg;
    pKt_ctrl->thread = NULL;
    pKt_ctrl->priority = thread_pri;
    pKt_ctrl->wait_sem = osal_sem_create(0); /* create semaphore. */
    if (0 == pKt_ctrl->wait_sem)
    {
        /* fail to create semaphore. */
        return (osal_thread_t)(uintptr)NULL;
    }

    /* deliver the creation of the thread to kevnetd. */
    pKt_ctrl->thread = kthread_create((int (*)(void *))thread_entry_point, pKt_ctrl, "%s", pKt_ctrl->name);
    if (pKt_ctrl->thread == NULL)
        printk("\ntask create FAILED !!!\n");
     wake_up_process(pKt_ctrl->thread);

    /* waiting for thread_creator_callby_kevnetd to create the thread. */
    osal_sem_take(pKt_ctrl->wait_sem, OSAL_SEM_WAIT_FOREVER);

    return (osal_thread_t)pKt_ctrl->thread->pid;
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
    kthread_ctrl_t *pTarget_thread;

    /* Find the thread control data. */
    pTarget_thread = search_thread_byID(thread_id);

    /* We found the thread control data. */
    if (NULL != pTarget_thread)
    {
        kill_pid((struct pid *)(pTarget_thread->thread->pids[PIDTYPE_PID].pid), SIGKILL, 1);
        return RT_ERR_OK;
    }
    /* We find no thread control data. */
    else
    {
        return RT_ERR_FAILED;
    }
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
    return (osal_thread_t)current->pid;
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
    kthread_ctrl_t *pTarget_thread;

    RT_INTERNAL_PARAM_CHK((NULL == pThread_name), RT_ERR_NULL_POINTER);

    pTarget_thread = search_thread_byID(thread_id);

    if (NULL != pTarget_thread)
    {
        if (osal_strlen(pTarget_thread->name) >= size)
        {
            return RT_ERR_FAILED;
        }
        else
        {
            osal_strcpy(pThread_name, pTarget_thread->name);

            return RT_ERR_OK;
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
    kthread_ctrl_t *pExit;

    pExit = search_thread_byID(current->pid);
    /* Not the thread under our management. */
    if (NULL == pExit)
    {
        return;
    }
    /* Remove the thread from the list. */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39))
    lock_kernel();
    list_del(&pExit->list);
    unlock_kernel();
#else
    list_del(&pExit->list);
#endif

    osal_sem_destroy(pExit->wait_sem);

    /* Relase the memory. */
    osal_free(pExit);

    /* Signal the parent thread. */
    kill_pid((struct pid *)(current->pids[PIDTYPE_PID].pid), SIGKILL, 1);
    complete_and_exit(NULL, ret_code);

} /* end of osal_thread_exit */

