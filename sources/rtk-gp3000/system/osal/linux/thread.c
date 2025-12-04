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
#if defined(CONFIG_KERNEL_2_6_30 )
#include <linux/smp_lock.h>
#else
#include <linux/mutex.h>
#include <linux/kthread.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
#include <linux/sched/signal.h>
#endif
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <common/debug/rt_log.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/sem.h>

/*
 * Symbol Definition
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
  #define MY_INIT_WORK(_t, _f, _d)	INIT_WORK((_t), (_f), (_d))
#else
  #define MY_INIT_WORK(_t, _f, _d)	INIT_WORK((_t), (_f))
#endif

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

/* list head of thread list. */
LIST_HEAD(kthread_list);


/*
 * Macro Definition
 */
#define TO_POSIX_NICE_PRIO(p) ((((p) * 39) / 255) - 20)

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
    /* daemonize the thread, therefore it won't be affect by its creator. */
#if defined(CONFIG_KERNEL_2_6_30)
    daemonize(pKt_ctrl->name);
#endif

    /* kernel thread disable all signals by default. enable SIGKILL signal. */
    allow_signal(SIGKILL);

    /*
     * Since we can't get thread information before creating it,
     * we complete kthread_ctrl_t data before running it.
     */
    osal_strcpy(current->comm, pKt_ctrl->name);
    current->policy = SCHED_NORMAL;
    set_user_nice(current, TO_POSIX_NICE_PRIO(pKt_ctrl->priority));
    pKt_ctrl->thread = current;

    /* Add kthread_list into pKt_ctrl->list. */
#if defined(CONFIG_KERNEL_2_6_30)
    lock_kernel();
#endif
    list_add(&pKt_ctrl->list, &kthread_list);
#if defined(CONFIG_KERNEL_2_6_30)
    unlock_kernel();
#endif

    /* Signal the completion of the thread initializaion. */
    osal_sem_give(pKt_ctrl->wait_sem);

    /* Call the real thread function. */
    pKt_ctrl->f(pKt_ctrl->pArg);

    /* Do the jobs for exiting. */
    osal_thread_exit(0);
} /* end of thread_entry_point */


/* Function Name:
 *      thread_creator_callby_kevnetd
 * Description:
 *      called by kevnetd thread in the linux kernel to create the thread.
 * Input:
 *      data - The private data which is passed to the created thread.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      Used for internal implementation.
 */
#if defined(CONFIG_KERNEL_2_6_30) 
static void
thread_creator_callby_kevnetd(void *pData)
{
    kernel_thread((int (*)(void *))thread_entry_point, pData, 0);
} /* end of thread_creator_callby_kevnetd */
#else
static void
thread_creator_callby_kevnetd(kthread_ctrl_t *pData)
{
	kthread_run((int (*)(void *))thread_entry_point, pData, pData->name);

} /* end of thread_creator_callby_kevnetd */
#endif

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
    if(NULL == pKt_ctrl)
    {
        /* memory allocation error. */
        return (osal_thread_t)NULL;
    }

    /* clear the data */
    osal_memset(pKt_ctrl, 0, sizeof(kthread_ctrl_t));

    /* fill up the field in the pKt_ctrl */
    if (NULL != pName)
    {
        if(strlen(pName) < sizeof(pKt_ctrl->name))
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
	osal_free(pKt_ctrl);
        return (osal_thread_t)NULL;
    }

    /* deliver the creation of the thread to kevnetd. */
#if 0
    INIT_WORK(&pKt_ctrl->wq, thread_creator_callby_kevnetd, pKt_ctrl);
#else
    MY_INIT_WORK(&pKt_ctrl->wq, thread_creator_callby_kevnetd, pKt_ctrl);
#endif
    schedule_work(&pKt_ctrl->wq);

    /* waiting for thread_creator_callby_kevnetd to create the thread. */
    osal_sem_take(pKt_ctrl->wait_sem, OSAL_SEM_WAIT_FOREVER);

    if(NULL != pKt_ctrl->thread)
    {
        return (osal_thread_t)pKt_ctrl->thread->pid;
    }
    else
    {
        return 0;
    }

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
#if 1
    struct pid *pid;
#endif
    /* Find the thread control data. */
    pTarget_thread = search_thread_byID(thread_id);

    /* We found the thread control data. */
    if (NULL != pTarget_thread)
    {
#if 0
        kill_proc(((struct task_struct *)(pTarget_thread->thread))->pid, SIGKILL, 1);
#else
        rcu_read_lock();
		pid = get_pid(find_vpid(((struct task_struct *)(pTarget_thread->thread))->pid));
		rcu_read_unlock();
		kill_pid(pid, SIGKILL, 1);
#endif
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
#if 1
    struct pid *pid;
#endif

    pExit = search_thread_byID(current->pid);
    /* Not the thread under our management. */
    if (NULL == pExit)
    {
        return;
    }
    /* Remove the thread from the list. */
#if defined(CONFIG_KERNEL_2_6_30)
    lock_kernel();
#endif
    list_del(&pExit->list);
#if defined(CONFIG_KERNEL_2_6_30)
    unlock_kernel();
#endif
    /* Relase the memory. */
    osal_free(pExit);

    /* Signal the parent thread. */
#if 0
    kill_proc(2, SIGCHLD, 1);
#else
    rcu_read_lock();
	pid = get_pid(find_vpid(current->pid));
	rcu_read_unlock();
	kill_pid(pid, SIGCHLD, 1);
#endif

    complete_and_exit(NULL, ret_code);
} /* end of osal_thread_exit */

