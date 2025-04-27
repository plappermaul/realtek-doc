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
#include <linux/slab.h>
#include <linux/wait.h>
#include <common/type.h>
#include <common/error.h>
#include <osal/wait.h>
#include <osal/print.h>
#include <linux/sched.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */
typedef struct osal_linux_wait_event_s {
    osal_event_t                event_id;
    wait_queue_head_t           osal_linux_wait_event_queue;
    atomic_t                    osal_linux_wait_event_intr;
    init_state_t                wait_module_init_flag;
    void                        *prev;
    void                        *next;
} osal_linux_wait_event_t;

osal_linux_wait_event_t     *osal_linux_wait_head = NULL;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      _osal_wait_module_find
 * Description:
 *      Fund out the event structure by event ID.
 * Input:
 *      None
 * Output:
 *      event_id
 * Return:
 *
 * Note:
 *
 */
osal_linux_wait_event_t *
_osal_wait_module_find(osal_event_t event_id)
{
    osal_linux_wait_event_t     *event_index_cur;

    event_index_cur = osal_linux_wait_head;

    if(event_index_cur == NULL)
        return NULL;

    while(event_index_cur->event_id != event_id)
    {
        if(event_index_cur->next == NULL)
        {
            return NULL;
        }else{
            event_index_cur = event_index_cur->next;
        }
    }
    if(event_index_cur->wait_module_init_flag == INIT_COMPLETED)
        return event_index_cur;
    else
        return NULL;

}

/* Function Name:
 *      osal_wait_module_list
 * Description:
 *      List all event structure.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
void
osal_wait_module_list(void)
{
    osal_linux_wait_event_t     *event_index_cur;
    uint32                      loop_idx = 0;

    event_index_cur = osal_linux_wait_head;
    osal_printf("\n[OSAL EVENT MODULE LIST]\n");

    while(event_index_cur != NULL)
    {
        osal_printf("( %d ) EVENT ID = %ld ",loop_idx,(unsigned long)event_index_cur->event_id);
        loop_idx++;

        if((loop_idx % 5) == 0)
            osal_printf(" <--> \n");
        else
            osal_printf(" <--> ");

        if(event_index_cur->next == NULL)
        {
            osal_printf("\n");
            return;
        }else{
            event_index_cur = event_index_cur->next;
        }
    }
}


/* Function Name:
 *      osal_wait_module_create
 * Description:
 *      Create a event structure.
 * Input:
 *      None
 * Output:
 *      event_id
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_module_create(osal_event_t *event_id)
{
    osal_linux_wait_event_t     *event_index_cur;
    osal_linux_wait_event_t     *p;
    uint32                      id = 0;

    event_index_cur = osal_linux_wait_head;

    /* First event structure */
    if(event_index_cur == NULL)
    {
        p = kmalloc(sizeof(osal_linux_wait_event_t), GFP_ATOMIC);
        if(p == NULL)
            return RT_ERR_FAILED;
        p->prev = NULL;
        p->next = NULL;
        osal_linux_wait_head = p;
        atomic_set(&p->osal_linux_wait_event_intr, 0);
        init_waitqueue_head(&p->osal_linux_wait_event_queue);
        p->event_id = id;
        p->wait_module_init_flag = INIT_COMPLETED;
        *event_id = id;
        return RT_ERR_OK;
    }

    while(event_index_cur != NULL)
    {
        id = (event_index_cur->event_id + 1);
        if(event_index_cur->next == NULL)
        {
            p = kmalloc(sizeof(osal_linux_wait_event_t), GFP_ATOMIC);
            if(p == NULL)
                return RT_ERR_FAILED;
            p->next = NULL;
            p->prev = event_index_cur;
            event_index_cur->next = p;
            atomic_set(&p->osal_linux_wait_event_intr, 0);
            init_waitqueue_head(&p->osal_linux_wait_event_queue);
            p->event_id = id;
            p->wait_module_init_flag = INIT_COMPLETED;
            *event_id = id;
            return RT_ERR_OK;
        }else{
            event_index_cur = event_index_cur->next;
        }
    }
    return RT_ERR_FAILED;
}

/* Function Name:
 *      osal_wait_module_destroy
 * Description:
 *      Destroy the existed event structure.
 * Input:
 *      event_id
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_module_destroy(osal_event_t event_id)
{
    osal_linux_wait_event_t     *p;
    osal_linux_wait_event_t     *p_prev;
    osal_linux_wait_event_t     *p_next;

    p = _osal_wait_module_find(event_id);
    if(p == NULL)
        return RT_ERR_FAILED;

    if(p->prev == NULL)         /* Head */
    {
        osal_linux_wait_head = p->next;
    }
    else if(p->next == NULL)   /* Tail */
    {
        p_prev = p->prev;
        p_prev->next = NULL;
    }
    else
    {
        p_prev = p->prev;
        p_next = p->next;
        p_prev->next = p->next;
        p_next->prev = p->prev;
    }
    kfree(p);
    return RT_ERR_OK;
}


/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wait a event that osal_wake_up() sends.
 * Input:
 *      event      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_event(osal_event_t event)
{
    uint32  ret = 0;
    osal_linux_wait_event_t     *p;

    p = _osal_wait_module_find(event);
    if(p == NULL)
        return RT_ERR_FAILED;

    if (p->osal_linux_wait_event_intr.counter <= 0)
    {
        atomic_inc(&p->osal_linux_wait_event_intr);
    }

    do
    {
        ret = wait_event_interruptible(p->osal_linux_wait_event_queue, atomic_read(&p->osal_linux_wait_event_intr) <= 0);
        if(ret != 0)
        {
            if (signal_pending(current))
            {
                 flush_signals(current);
            }
        }
    }while(ret != 0);
    /*
     * only run the interrupt handler once.
     */
    atomic_set(&p->osal_linux_wait_event_intr, 0);

    /* will implement by semaphore take */
    return RT_ERR_OK;
}


/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wake up a event queue.
 * Input:
 *      event      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wake_up(osal_event_t event)
{
    osal_linux_wait_event_t     *p;

    p = _osal_wait_module_find(event);
    if(p == NULL)
        return RT_ERR_FAILED;

    if (atomic_dec_return(&p->osal_linux_wait_event_intr) >= 0)
    {
        wake_up_interruptible(&p->osal_linux_wait_event_queue);
    }
    /* will implement by semaphore give */
    return RT_ERR_OK;
}


