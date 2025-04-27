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
 * $Revision: 85579 $
 * $Date: 2018-01-19 19:16:52 +0800 (Fri, 19 Jan 2018) $
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
#include <linux/version.h>
#include <common/type.h>
#include <linux/interrupt.h>


/*
 * Symbol Definition
 */
typedef struct work_struct  osal_work_struct_t;
typedef struct list_head    osal_list_head_t;


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
typedef void  osal_work_struct_arg_t;
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
typedef struct work_struct  osal_work_struct_arg_t;
#endif

/*
 * Function Declaration
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
#define OSAL_INIT_WORK(_work, _func, _data)     INIT_WORK(_work, _func, _data)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
#define OSAL_INIT_WORK(_work, _func, _data)     INIT_WORK(_work, _func)
#endif
#define OSAL_INIT_LIST_HEAD(_list_head)         INIT_LIST_HEAD(_list_head)
#define osal_list_for_each_entry_safe(pos, n, head, member) list_for_each_entry_safe(pos, n, head, member)
#define osal_list_del(entry)            list_del(entry)
#define osal_list_add_tail(new, head)   list_add_tail(new, head)
#define osal_schedule_work(work)        schedule_work(work)

#endif /* __OSAL_WORKQUEUE_H__ */
