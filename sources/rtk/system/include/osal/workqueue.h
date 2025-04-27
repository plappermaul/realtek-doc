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
 * Feature : Work queue relative API
 *
 */

#ifndef __OSAL_WORKQUEUE_H__
#define __OSAL_WORKQUEUE_H__

/*
 * Include Files
 */

#include <common/type.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/version.h>
#include <linux/interrupt.h>
#endif

/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
typedef struct work_struct  osal_work_struct_t;
typedef struct list_head    osal_list_head_t;


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20))
typedef void  osal_work_struct_arg_t;
#else
typedef struct work_struct  osal_work_struct_arg_t;
#endif
#elif defined(CONFIG_SDK_OS_KERNEL_OTHER)
struct list_head {
	struct list_head *next, *prev;
};
struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);
struct work_struct {
	void    *data;
	struct list_head entry;
	work_func_t func;
};

typedef struct work_struct  osal_work_struct_t;
typedef struct work_struct  osal_work_struct_arg_t;
typedef struct list_head    osal_list_head_t;
#else

#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
/*
 * Function Declaration
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20))
#define OSAL_INIT_WORK(_work, _func, _data)     INIT_WORK(_work, _func, _data)
#else
#define OSAL_INIT_WORK(_work, _func, _data)     INIT_WORK(_work, _func)
#endif
#define OSAL_INIT_LIST_HEAD(_list_head)         INIT_LIST_HEAD(_list_head)
#define osal_list_for_each_entry_safe(pos, n, head, member) list_for_each_entry_safe(pos, n, head, member)
#define osal_list_del(entry)            list_del(entry)
#define osal_list_add_tail(new, head)   list_add_tail(new, head)
#define osal_schedule_work(work)        schedule_work(work)

#elif defined(CONFIG_SDK_OS_KERNEL_OTHER)

#define osal_offsetof(type, field) ((unsigned long)(&((type *)0)->field))

#define list_entry(ptr, type, member) ({			\
   const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
   (type *)((char *)__mptr - osal_offsetof(type,member));})

#define list_next_entry(pos, member) \
   list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_first_entry(ptr, type, member) \
   list_entry((ptr)->next, type, member)

/**
 * osal_list_for_each_entry_safe - iterate over list of given
 * type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */

#define osal_list_for_each_entry_safe(pos, n, head, member)	\
for (pos = list_first_entry(head, typeof(*pos), member),    \
     n = list_next_entry(pos, member);			            \
     &pos->member != (head); 					            \
     pos = n, n = list_next_entry(n, member))


#define OSAL_INIT_WORK(_work, _func, _data)     osal_init_work(_work, _func)

static inline void OSAL_INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

/* Function Name:
 *      osal_init_work
 * Description:
 *      init workqueue
 * Input:
 *      work - work structure
 *      func - work function
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void osal_init_work(osal_work_struct_t *work, work_func_t func);

/* Function Name:
 *      osal_list_del
 * Description:
 *      deletes entry from list
 * Input:
 *      entry - the element to delete from the list
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void osal_list_del(osal_list_head_t *entry);

/* Function Name:
 *      osal_list_add_tail
 * Description:
 *      add a new entry
 * Input:
 *      new - new entry to be added
 *      head - list head to add it before
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void osal_list_add_tail(osal_list_head_t *new, osal_list_head_t *head);

/* Function Name:
 *      osal_schedule_work
 * Description:
 *      put work task to scheduler
 * Input:
 *      work - job to be done
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern int osal_schedule_work(osal_work_struct_t *work);
#else

#endif

#endif /* __OSAL_WORKQUEUE_H__ */
