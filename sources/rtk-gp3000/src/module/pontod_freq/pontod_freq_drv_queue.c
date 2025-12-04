/*
 * Copyright (C) 2017 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PON ToD tune Time Frequency kernel drivers
 *
 * Feature : This module install PON ToD tune Time Frequency kernel callbacks and other 
 * kernel functions
 */

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#include "rtk/time.h"

#include "pontod_freq_drv_main.h"
#include "pontod_freq_drv_queue.h"

Queue_t* qCreate(void)
{
    Queue_t *q;

    q = (Queue_t *) kmalloc(sizeof(Queue_t), GFP_ATOMIC);
    if (! q)
    {
        printk("[%s %d]:qCreate:kmalloc failed\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    INIT_LIST_HEAD(&(q->list));

    spin_lock_init(&(q->lock));

    q->cnt=0;

    return q;
}

// Queue must be empty!
void qDelete(Queue_t *q)
{

    kfree((void *) q);

    return;
}

void qEnqueue(Queue_t *q, void* item)
{
    QueueItem_t *qi;

    if(q == NULL)
        return;

    qi = (QueueItem_t *) kmalloc(sizeof(QueueItem_t), GFP_ATOMIC);
    if (! qi)
    {
        printk("[%s %d]:qEnqueue:kmalloc failed\n",__FUNCTION__,__LINE__);
        return;
    }

    qi->item = item;

    qLock(q);
    list_add_tail(&(qi->list), &(q->list));
    q->cnt++;
    qUnlock(q);

    return;
}

void* qDequeue(Queue_t *q)
{
    QueueItem_t *qi;
    void * item;

    if(q == NULL)
        return NULL;

    if (qIsEmpty(q))
    {
        return NULL;
    }

    qLock(q);

    qi = list_first_entry(&(q->list), QueueItem_t, list);

    item = qi->item;

    list_del(&(qi->list));

    kfree((void *) qi);

    q->cnt--;

    qUnlock(q);

    return item;
}

int qIsEmpty(Queue_t *q)
{
    int ret;

    if(q == NULL)
        return 1;

    qLock(q);
    ret = list_empty(&(q->list));
    qUnlock(q);

    return ret;
}

uint64 qNum(Queue_t *q)
{
    uint64 tmp=0;

    if(q == NULL)
        return 0;

    qLock(q);
    tmp=q->cnt;
    qUnlock(q);

    return tmp;
}

void qLock(Queue_t *q)
{
    if(q == NULL)
        return;

    spin_lock(&(q->lock));

    return;
}

void qUnlock(Queue_t *q)
{

    if(q == NULL)
        return;

    spin_unlock(&(q->lock));

    return;
}



