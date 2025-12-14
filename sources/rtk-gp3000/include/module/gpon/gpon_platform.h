/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
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
 * *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON Mac Device Driver platform layer
 *
 * Feature : Provide the GPON Mac Device Driver platform API
 *
 */

#ifndef __GPON_PLATFORM_H__
#define __GPON_PLATFORM_H__

#include <osal/memory.h>
#include <osal/sem.h>
#include <osal/time.h>
#include <common/debug/rt_log.h>


/*
 * The OS Platform Abstract
 */
#define GPON_OS_Malloc(__size__)        osal_alloc(__size__)
#define GPON_OS_Free(__ptr__)           osal_free(__ptr__)

typedef osal_mutex_t                    gpon_os_lock_t;
#define GPON_OS_CreateLock()            osal_sem_mutex_create()
#define GPON_OS_DestroyLock(__sem__)    osal_sem_mutex_destroy(__sem__)
#define GPON_OS_Lock(__sem__)           osal_sem_mutex_take(__sem__, OSAL_SEM_WAIT_FOREVER)
#define GPON_OS_Unlock(__sem__)         osal_sem_mutex_give(__sem__)

#define GPON_OS_StartTimer(__interval__,__repeat__,__arg__,__func__)  osal_add_timer(__interval__,__arg__,__func__)
#define GPON_OS_StopTimer(__id__)       osal_del_timer(__id__)


#define GPON_LOG_LEVEL_ALL      0
#define GPON_LOG_LEVEL_ERROR    1
#define GPON_LOG_LEVEL_WARNING  2
#define GPON_LOG_LEVEL_NORMAL   3
#define GPON_LOG_LEVEL_INFO     4
#define GPON_LOG_LEVEL_PLOAM    5
#define GPON_LOG_LEVEL_OMCI     6
#define GPON_LOG_LEVEL_DEBUG    7

extern void gpon_dbg_enable(int32 enable, uint32 level);

extern int32 gpon_dbg_print(const uint32 level, const char *format, ...);
//#define GPON_OS_Log(__level__,__file__,__line__,__fmt__,args...)  rt_log(__level__,MOD_GPON,"%s#%d "__fmt__,__file__,__line__, ## args)
#define GPON_OS_Log(__level__,__fmt__,args...)  gpon_dbg_print(__level__,__fmt__, ## args)

#endif  /* __GPON_PLATFORM_H__ */

