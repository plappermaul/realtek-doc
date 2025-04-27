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
 * Feature : OSAL test APIs
 *
 */

/*
 * Include Files
 */
#include <osal/osal_test_case.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static int thread_1 = 0;
static int thread_2 = 0;
static int thread_3 = 0;
static int even_fail = 0;
static int osal_result = 0;

osal_thread_t osal_thread_Destroy;
osal_mutex_t *cliblock;     /* and a mutex to protect calls to the C library */

/*
 * Macro
 */

/*
 * Function Declaration
 */

/* this is a simple program which runs in a thread */
void thread_test_1(void *data)
{
    osal_thread_t thread_id;
    osal_time_t delay;
    osal_time_t current_time;
    int message = (int) data;
    int if_exit = 0;
    char name[32];

    /* Get thread handler */
    thread_id = osal_thread_self();
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("osal_thread_name Fail\n");
        osal_thread_exit(1);
    }

    osal_printf("Beginning execution; thread data is %d, name=%s\n", message, name);

    osal_time_sleep(2); //sleep 2 sec

    for (;;) {
        delay = 2 + (rand() % 4);
        osal_time_seconds_get(&current_time);

        /* note: printf() must be protected by a
           call to osal_sem_mutex_take() */
        osal_sem_mutex_take((osal_mutex_t)cliblock, OSAL_SEM_WAIT_FOREVER); {
            SHOWMESSAGE("Thread %d(%s): delay %08d usec, current %d sec\n",
                    message, name, delay*1000000, current_time);
        }
        osal_sem_mutex_give((osal_mutex_t)cliblock);

        osal_time_sleep(delay);

        /* Thread Exit */
        if( VERI_THREAD_EXIT_TIME==if_exit ){
            SHOWMESSAGE("[%s exit]\n", name);
            thread_1 = if_exit;
            osal_thread_exit(1);
        }
        if_exit++;
    }
}
/* this is a simple program which runs in a thread */
void thread_test_2(void *data)
{
    osal_thread_t thread_id;
    osal_time_t delay;
    osal_time_t current_time;
    int message = (int) data;
    int if_del = 0;
    int status;
    char name[32];

    /* Get thread handler */
    thread_id = osal_thread_self();
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("osal_thread_name Fail\n");
        osal_thread_exit(1);
    }

    osal_printf("Beginning execution; thread data is %d, name=%s\n", message, name);

    osal_time_usleep(2000000); //sleep 2 sec

    for (;;) {
        delay = 2000000 + (rand() % 4000000);
        osal_time_seconds_get(&current_time);

        /* note: osal_printf() must be protected by a
           call to osal_sem_mutex_take() */
        osal_sem_mutex_take((osal_mutex_t)cliblock, OSAL_SEM_WAIT_FOREVER); {
            SHOWMESSAGE("Thread %d(%s): delay %08d usec, current %d sec\n",
                    message, name, delay, current_time);
        }
#if DBG_THREAD
        /* OSAL Thread Destroy Test */
        if( (2==message) &&  (VERI_DESTROY_TIME==if_del) ){
            status = osal_thread_destroy(osal_thread_Destroy);
        }
#endif /* end of DBG_THREAD */
        osal_sem_mutex_give((osal_mutex_t)cliblock);

        osal_time_usleep(delay);

        if_del++;

        /* Thread Exit & Create Next Test Thread */
        if( VERI_THREAD_EXIT_TIME==if_del || 0==DBG_THREAD ){
            /* Set Thread Test Result */
            thread_2 = if_del;
#if DBG_THREAD
            while( VERI_THREAD_EXIT_TIME!=thread_1 ){
                osal_time_sleep(1);
            }
#endif /* end of DBG_THREAD */
            /* Check the therad status */
            if( VERI_THREAD_EXIT_TIME==thread_1 && \
                VERI_THREAD_EXIT_TIME==thread_2 && \
                VERI_THREAD_EXIT_TIME!=thread_3 && \
                                    0==even_fail   \
              ){
                osal_result |= OSALTEST_THREAD_OK;
                osal_printf("[OSAL Thread Test Pass]\n");
            }

            SHOWMESSAGE("[%s exit]\n", name);
            osal_result |= OSALTEST_FINAL;
            osal_thread_exit(1);
        }
    }
}

/* This thread will be destroyed */
void thread_test_3(void *data)
{
    osal_thread_t thread_id;
    osal_time_t delay;
    osal_time_t current_time;
    int message = (int) data;
    int destroy = 0;
    char name[32];

    /* Get thread handler */
    thread_id = osal_thread_self();
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
        osal_thread_exit(1);
    }

    osal_printf("Beginning execution; thread data is %d, name=%s\n", message, name);

    osal_time_usleep(2000000); /* sleep 2 sec */

    for (;;) {
        delay = 2000000 + (rand() % 4000000);
        osal_time_seconds_get(&current_time);

        /* note: osal_printf() must be protected by a
           call to osal_sem_mutex_take() */
        osal_sem_mutex_take((osal_mutex_t)cliblock, OSAL_SEM_WAIT_FOREVER); {
            SHOWMESSAGE("Thread %d(%s): delay %08d usec, current %d sec\n",
                    message, name, delay, current_time);
        }
        osal_sem_mutex_give((osal_mutex_t)cliblock);
        osal_time_usleep(delay);

        destroy++;
        thread_3 = destroy;

        /* Destroy Fail, the destroy thread is running */
        if( destroy>=(VERI_DESTROY_TIME+VERI_DEST_LIMIT) ){
            SHOWMESSAGE("OSAL Destroy Test Fail\n");
            even_fail++;
        }

        /* Destroy Fail, the destroy thread exits by himself */
        if( VERI_THREAD_EXIT_TIME==destroy ){
            SHOWMESSAGE("OSAL Destroy Test Fail\n");
            SHOWMESSAGE("[%s exit]\n", name);
            osal_result |= OSALTEST_FINAL;
            osal_thread_exit(1);
        }
    }
}

int32 osal_thread_test(uint32 caseNo, uint32 unit)
{
    osal_thread_t osal_threadA, osal_threadB, osal_threadC;
    uint32 timeout = 0;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running THREAD test cases", __FUNCTION__);

    cliblock = (osal_mutex_t *) osal_sem_mutex_create();

    osal_threadA = (osal_thread_t) osal_thread_create("Thread A",            \
                                                      4096,                  \
                                                      4,                     \
                                                      (void *)thread_test_1, \
                                                      (void *)1              \
                                                     );

    osal_threadB = (osal_thread_t) osal_thread_create("Thread B",            \
                                                      4096,                  \
                                                      4,                     \
                                                      (void *)thread_test_2, \
                                                      (void *)2              \
                                                     );

    osal_threadC = (osal_thread_t) osal_thread_create("Thread C",            \
                                                      4096 ,                 \
                                                      4,                     \
                                                      (void *)thread_test_1, \
                                                      (void *)3              \
                                                     );

    osal_thread_Destroy = (osal_thread_t) osal_thread_create("Destroy ",     \
                                                      4096,                  \
                                                      4,                     \
                                                      (void *)thread_test_3, \
                                                      (void *)4              \
                                                     );

    while(!(osal_result & OSALTEST_FINAL))
    {
        osal_time_sleep(1);
        if(++timeout > OSAL_TESTCASE_TIMEOUT)
        {
            /* force to destroy all created thread */
            osal_thread_destroy(osal_threadA);
            osal_thread_destroy(osal_threadB);
            osal_thread_destroy(osal_threadC);
            osal_thread_destroy(osal_thread_Destroy);

            osal_printf("[OSAL Test Case Timeout]\n");
            break;
        }
    }

    if(OSALTEST_THREAD_OK != (osal_result & OSALTEST_THREAD_OK))
    {
        osal_printf("[OSAL Thread Test Fail]\n");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
