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
#define THREAD_1_FIRST_SLEEP     (0)            /*  sec */
#define THREAD_1_TAKE_SLEEP      (1)            /*  sec */
#define THREAD_1_GIVE_SLEEP      (2)            /*  sec */
#define THREAD_1_RETRY_SLEEP     (1)            /*  sec */
#define THREAD_1_TAKE_WAIT       (1000)         /* usec */

#define THREAD_2_FIRST_SLEEP     (1)            /*  sec */
#define THREAD_2_TAKE_SLEEP      (2)            /*  sec */
#define THREAD_2_GIVE_SLEEP      (3)            /*  sec */
#define THREAD_2_RETRY_SLEEP     (1)            /*  sec */
#define THREAD_2_TAKE_WAIT       (1000)         /* usec */

#define THREAD_3_FIRST_SLEEP     (2)            /*  sec */
#define THREAD_3_TAKE_SLEEP      (3)            /*  sec */
#define THREAD_3_GIVE_SLEEP      (1)            /*  sec */
#define THREAD_3_RETRY_SLEEP     (1)            /*  sec */
#define THREAD_3_TAKE_WAIT       (1000)         /* usec */

#define THREAD_3_DESTORY_WAIT     (VERI_MUTEX_EXIT_TIME*(THREAD_1_TAKE_SLEEP+THREAD_1_GIVE_SLEEP+\
                     THREAD_2_TAKE_SLEEP+THREAD_2_GIVE_SLEEP))

/*
 * Data Declaration
 */
static volatile int take_num_1 = 0;
static volatile int take_num_2 = 0;
static volatile int take_num_3 = 0;
static volatile int even_fail  = 0;
static unsigned char  sem[3];
static osal_mutex_t   mutex;
static int osal_result = 0;

/*
 * Macro
 */

/*
 * Function Declaration
 */
void mutex_test_1(void *data){
    osal_thread_t thread_id;
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;
    unsigned int even_retry = 0;
    char name[32];

    take_num_1 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
        osal_thread_exit(1);
    }

    osal_time_sleep(THREAD_1_FIRST_SLEEP);

#if DBG_MUTEX
    do{
        /* Save current time in take_time*/
        osal_time_usecs_get(&take_time);

        /* Try Take the mutex, wait THREAD_1_TAKE_WAIT */
        if_take = osal_sem_mutex_take(mutex, THREAD_1_TAKE_WAIT);

        /* osal_sem_mutex_take return, save current time in return_time */
        osal_time_usecs_get(&return_time);

        /* Check do take or not */
        if( RT_ERR_OK==if_take ){
            /* Set thread_1 take mutex */
            sem[0] = 1;

            /* Check Max Mutex */
            if( 1<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }

            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);

            /* Take Sleep */
            osal_time_sleep(THREAD_1_TAKE_SLEEP);

            /* Give Mutex */
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            sem[0] = 0;

            osal_sem_mutex_give(mutex);
            take_num_1++;

            /* Next Try Take Time */
            osal_time_sleep(THREAD_1_GIVE_SLEEP);
        }else{
            even_retry++;
            SHOWMESSAGE("[%s timeout, take_time=%08d, return_time=%08d,"\
                    " eclapsed=%08d, sem=[%d, %d, %d]]\n", \
                    name, take_time, return_time, return_time-take_time, \
                    sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_1_RETRY_SLEEP);
        }

        /* Thread Exit */
        if( VERI_MUTEX_EXIT_TIME==take_num_1 ){
            SHOWMESSAGE("[%s exit]\n", name);
            if( 0==even_retry ){
                even_fail++;
            }
            osal_thread_exit(1);
        }
    }while(1);
#else /* else of DBG_MUTEX */
    osal_thread_exit(1);
#endif /*end of else of DBG_MUTEX */

}

void mutex_test_2(void *data){
    osal_thread_t thread_id;
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;
    unsigned int even_retry = 0;
    char name[32];

    take_num_2 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

#if DBG_MUTEX
    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
        osal_thread_exit(1);
    }
    osal_time_sleep(THREAD_2_FIRST_SLEEP);
    do{
        /* Save current time in take_time*/
        osal_time_usecs_get(&take_time);

        /* Try Take the mutex, wait THREAD_2_TAKE_WAIT */
        if_take = osal_sem_mutex_take(mutex, THREAD_2_TAKE_WAIT);

        /* osal_sem_mutex_take return, save current time in return_time */
        osal_time_usecs_get(&return_time);

        /* Check do take or not */
        if( RT_ERR_OK==if_take ){
            /* Set thread_2 take mutex */
            sem[1] = 1;

            /* Check Max Mutex */
            if( 1<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }

            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);

            /* Take Sleep */
            osal_time_sleep(THREAD_2_TAKE_SLEEP);

            /* Give Mutex */
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            sem[1] = 0;
            osal_sem_mutex_give(mutex);
            take_num_2++;

            /* Next Try Take Time */
            osal_time_sleep(THREAD_2_GIVE_SLEEP);
        }else{
            even_retry++;
            SHOWMESSAGE("[%s timeout, take_time=%08d, return_time=%08d,"\
                    " eclapsed=%08d, sem=[%d, %d, %d]]\n", \
                    name, take_time, return_time, return_time-take_time, \
                    sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_2_RETRY_SLEEP);
        }

        /* Thread Exit */
        if( VERI_MUTEX_EXIT_TIME==take_num_2 ){
            SHOWMESSAGE("[%s exit]\n", name);
            if( 0==even_retry ){
                even_fail++;
            }
            osal_thread_exit(1);
        }
    }while(1);
#else /* else of DBG_MUTEX */
    osal_thread_exit(1);
#endif /*end of else of DBG_MUTEX */
}
void mutex_test_3(void *data){
    osal_thread_t thread_id;
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;
    unsigned int even_retry = 0;
    char name[32];

    take_num_3 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
    }
    osal_time_sleep(THREAD_3_FIRST_SLEEP);
    do{
        /* Save current time in take_time*/
        osal_time_usecs_get(&take_time);

        /* Try Take the mutex, wait THREAD_3_TAKE_WAIT */
        if_take = osal_sem_mutex_take(mutex, THREAD_3_TAKE_WAIT);

        /* osal_sem_mutex_take return, save current time in return_time */
        osal_time_usecs_get(&return_time);
        if( RT_ERR_OK==if_take ){
            /* Set thread_3 take mutex */
            sem[2] = 1;

            /* Check Max Mutex */
            if( 1<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }

            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            /* Take Sleep */
            osal_time_sleep(THREAD_3_TAKE_SLEEP);

            /* Give Mutex */
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            sem[2] = 0;
            osal_sem_mutex_give(mutex);
            take_num_3++;

            /* Next Try Take Time */
            osal_time_sleep(THREAD_3_GIVE_SLEEP);
        }else{
            even_retry++;
            SHOWMESSAGE("[%s timeout, take_time=%08d, return_time=%08d,"\
                    " eclapsed=%08d, sem=[%d, %d, %d]]\n", \
                    name, take_time, return_time, return_time-take_time, \
                    sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_3_RETRY_SLEEP);
        }

        if( VERI_MUTEX_EXIT_TIME==take_num_3 || 0==DBG_MUTEX ){
            SHOWMESSAGE("[%s exit]\n", name);
#if DBG_MUTEX
            while( VERI_MUTEX_EXIT_TIME!=take_num_1 || \
                    VERI_MUTEX_EXIT_TIME!=take_num_2 || \
                    VERI_MUTEX_EXIT_TIME!=take_num_3    \
                 );
            while( 0!=sem[0] || 0!=sem[1] || 0!=sem[2] );
#endif /* end of DBG_MUTEX */
            if_take = osal_sem_mutex_take(mutex, OSAL_SEM_WAIT_FOREVER);
            if( RT_ERR_OK!=if_take ){
                even_fail++;
            }else{
                osal_sem_mutex_give(mutex);
                osal_time_sleep(THREAD_3_DESTORY_WAIT);
                osal_sem_mutex_destroy(mutex);
            }
            if( (0==even_fail) && (0!=even_retry) ){
                osal_result |= OSALTEST_MUTEX_OK;
                osal_printf("[OSAL Mutex Test Pass]\n");
            }

            osal_result |= OSALTEST_FINAL;
            osal_thread_exit(1);
        }
    }while(1);
}

int32 osal_mutex_test(uint32 caseNo, uint32 unit)
{
    osal_thread_t osal_threadA, osal_threadB, osal_threadC;
    uint32 timeout = 0;

    /* initialize global variables */
    take_num_1 = 0;
    take_num_2 = 0;
    take_num_3 = 0;
    even_fail  = 0;
    osal_result = 0;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running MUTEX test cases", __FUNCTION__);

    /* Create Mutex */
    mutex = osal_sem_mutex_create();
    if( NULL==(void*)mutex ){
        osal_printf("[OSAL Create Mutex Fail]");
        return RT_ERR_FAILED;
    }

    /* Clear Mutex Signals */
    sem[0] = sem[1] = sem[2] = 0;

    /* Create the OSAL Mutex Test Thread */
    osal_threadA = (osal_thread_t) osal_thread_create( \
                    "Mutex A",
                    4096,
                    4,
                    mutex_test_1,
                    (void *)0
                    );
    osal_threadB = (osal_thread_t) osal_thread_create( \
                    "Mutex B",
                    4096,
                    4,
                    mutex_test_2,
                    (void *)1
                    );
    osal_threadC = (osal_thread_t) osal_thread_create( \
                    "Mutex C",
                    4096,
                    4,
                    mutex_test_3,
                    (void *)2
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

            osal_printf("[OSAL Test Case Timeout]\n");
            break;
        }
    }

    if(OSALTEST_MUTEX_OK != (osal_result & OSALTEST_MUTEX_OK))
    {
        osal_printf("[OSAL Mutex Test Fail]\n");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
