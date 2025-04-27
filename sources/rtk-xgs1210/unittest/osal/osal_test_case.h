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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of OSAL test APIs in the SDK
 *
 * Feature : SDK OSAL test APIs
 *
 */

#ifndef __OSAL_TEST_CASE_H__
#define __OSAL_TEST_CASE_H__
/*
 * Include Files
 */
#include <dev_config.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/cache.h>
#include <osal/isr.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
#define DBG_TIME                 (1)
#define DBG_THREAD               (1)
#define DBG_SEMAPHORE            (1)
#define DBG_MEMORY_CACHE         (1)
#define DBG_ISR                  (1)
#define DBG_MUTEX                (1)

#define VERI_DESTROY_TIME        (2)
#define VERI_DEST_LIMIT          (3)
#define VERI_THREAD_EXIT_TIME    (VERI_DESTROY_TIME+VERI_DEST_LIMIT+1)
#define VERI_SEMAPHORE_EXIT_TIME (4)
#define VERI_MUTEX_EXIT_TIME     (3)

#define OSAL_TIME_TEST_NUMBER    (64)
#define OSAL_RAND_USEC           (4000000)  //for 4 secs
#define OSAL_RAND_SEC            (4)        //for 4 secs
#define OSAL_TESTCASE_TIMEOUT    (60)       //for 60 secs
#define INIT_SEM_COUNTER         (2)

/* for test results */
#define OSALTEST_TIME_OK         (1<<0)
#define OSALTEST_THREAD_OK       (1<<1)
#define OSALTEST_SEMAPHORE_OK    (1<<2)
#define OSALTEST_MEM_CACHE_OK    (1<<3)
#define OSALTEST_ISR_OK          (1<<4)
#define OSALTEST_MUTEX_OK        (1<<5)

#define OSALTEST_FINAL           (1<<31)

#define OSALTEST_ALL_OK          ( OSALTEST_TIME_OK      | \
                                   OSALTEST_THREAD_OK    | \
                                   OSALTEST_SEMAPHORE_OK | \
                                   OSALTEST_MEM_CACHE_OK | \
                                   OSALTEST_ISR_OK       | \
                                   OSALTEST_MUTEX_OK     \
                                 )

#define SHOWDBG                  (0)

#ifndef REG32
  #define REG32(x)               (*((volatile unsigned int*)(x)))
#endif /* REG32 */

/*
 * Macro
 */
#ifndef SHOWMESSAGE
    #if SHOWDBG
      #define SHOWMESSAGE(msg, argv...) osal_printf("[%s][%s][%03d]" msg, __FILE__, __FUNCTION__, __LINE__, ##argv)
    #else
      #define SHOWMESSAGE(msg, argv...)
    #endif
#endif /* end of ndef SHOWMESSAGE */

#define DBG_printf(num, msg, argv...)                      \
    if( (num) ){                                           \
        SHOWMESSAGE(msg, ##argv);                          \
    }

/*
 * Function Declaration
 */

/* local utility for osal test (OS dependent) */
#ifdef CONFIG_SDK_KERNEL_LINUX
extern int rand(void);
#endif

/* osal test case (OS independent) */
extern int32 osal_memory_cache_test(uint32 caseNo, uint32 unit);
extern int32 osal_isr_test(uint32 caseNo, uint32 unit);
extern int32 osal_time_test(uint32 caseNo, uint32 unit);
extern int32 osal_thread_test(uint32 caseNo, uint32 unit);
extern int32 osal_mutex_test(uint32 caseNo, uint32 unit);
extern int32 osal_sem_test(uint32 caseNo, uint32 unit);

#endif /* end of __OSAL_TEST_CASE_H__ */

