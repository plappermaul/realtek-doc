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
 * Feature : This module install PON ToD tune Time Frequency callbacks and other 
 * kernel functions
 */

#ifndef __PONTOD_FREQ_DRV_QUEUE_H__
#define __PONTOD_FREQ_DRV_QUEUE_H__

#if defined(CONFIG_SDK_KERNEL_LINUX)
typedef struct QueueItem_s
{
    struct list_head list; // Linked List
    void *item; // Item of Data
} QueueItem_t;

typedef struct Queue_s
{
    struct list_head list; // List Head
    spinlock_t lock; // Lock
    uint64 cnt;
} Queue_t;

// Prototypes

Queue_t* qCreate(void);
void qDelete(Queue_t *q);
void qEnqueue(Queue_t *q, void *item);
void* qDequeue(Queue_t *q);
int qIsEmpty(Queue_t *q);
uint64 qNum(Queue_t *q);
void qLock(Queue_t *q);
void qUnlock(Queue_t *q);
#endif

#endif
