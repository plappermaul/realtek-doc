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
#define THREAD_2_TAKE_WAIT       (5*1000000)    /* usec */

#define THREAD_3_FIRST_SLEEP     (2)            /*  sec */
#define THREAD_3_TAKE_SLEEP      (3)            /*  sec */
#define THREAD_3_GIVE_SLEEP      (1)            /*  sec */
#define THREAD_3_RETRY_SLEEP     (1)            /*  sec */
#define THREAD_3_TAKE_WAIT       (100000/2)     /* usec */
#define THREAD_3_DESTORY_WAIT    (VERI_MUTEX_EXIT_TIME*(THREAD_1_TAKE_SLEEP+THREAD_1_GIVE_SLEEP+\
                                     THREAD_2_TAKE_SLEEP+THREAD_2_GIVE_SLEEP))

/*
 * Data Declaration
 */
static volatile int take_num_1 = 0;
static volatile int take_num_2 = 0;
static volatile int take_num_3 = 0;
static volatile int even_fail  = 0;
static unsigned char sem[3];
static osal_sem_t semaphore;
static int osal_result = 0;

/*
 * Function Declaration
 */
void sem_test_1(void *data){
    osal_thread_t thread_id;
    char name[32];
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;
    unsigned int even_retry = 0;

    take_num_1 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
        osal_thread_exit(1);
    }
    osal_time_sleep(THREAD_1_FIRST_SLEEP);
#if DBG_SEMAPHORE
    do{
        osal_time_usecs_get(&take_time);
        if_take = osal_sem_take(semaphore, THREAD_1_TAKE_WAIT);
        osal_time_usecs_get(&return_time);
        if( RT_ERR_OK==if_take ){
            sem[0] = 1;

            /* Check Max Semaphore */
            if( INIT_SEM_COUNTER<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }
            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_1_TAKE_SLEEP);
            sem[0] = 0;
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_sem_give(semaphore);
            take_num_1++;
            osal_time_sleep(THREAD_1_GIVE_SLEEP);
        }else{
            even_retry++;
            SHOWMESSAGE("[%s take_time=%08d, return_time=%08d, eclapsed=%08d]\n", \
                    name, take_time, return_time, return_time-take_time);
            osal_time_sleep(THREAD_1_RETRY_SLEEP);
        }
        if( VERI_SEMAPHORE_EXIT_TIME==take_num_1 ){
            SHOWMESSAGE("[%s exit]\n", name);
            if( 0==even_retry ){
                even_fail++;
            }
            osal_thread_exit(1);
        }
    }while(1);
#else /* else of DBG_SEMAPHORE */
    osal_thread_exit(1);
#endif /*end of else of DBG_SEMAPHORE */

}
void sem_test_2(void *data){
    osal_thread_t thread_id;
    char name[32];
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;
    unsigned int even_retry = 0;

    take_num_2 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

#if DBG_SEMAPHORE
    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
        osal_thread_exit(1);
    }
    osal_time_sleep(THREAD_2_FIRST_SLEEP);
    do{
        osal_time_usecs_get(&take_time);
        if_take = osal_sem_take(semaphore, THREAD_2_TAKE_WAIT);
        osal_time_usecs_get(&return_time);
        if( RT_ERR_OK==if_take ){
            sem[1] = 1;

            /* Check Max Semaphore */
            if( INIT_SEM_COUNTER<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }
            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_2_TAKE_SLEEP);
            sem[1] = 0;
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_sem_give(semaphore);
            take_num_2++;
            osal_time_sleep(THREAD_2_GIVE_SLEEP);
        }else{
            if( 0!=even_retry ){
                even_fail++;
            }
            SHOWMESSAGE("[%s take_time=%08d, return_time=%08d, eclapsed=%08d]\n", \
                    name, take_time, return_time, return_time-take_time);
            osal_time_sleep(THREAD_2_RETRY_SLEEP);
        }
        if( VERI_SEMAPHORE_EXIT_TIME==take_num_2 ){
            SHOWMESSAGE("[%s exit]\n", name);
            osal_thread_exit(1);
        }
    }while(1);
#else /* else of DBG_SEMAPHORE */
    osal_thread_exit(1);
#endif /*end of else of DBG_SEMAPHORE */
}
void sem_test_3(void *data){
    osal_thread_t thread_id;
    char name[32];
    osal_time_t take_time;
    osal_time_t return_time;
    unsigned int if_take = RT_ERR_FAILED;

    take_num_3 = 0;

    /* Get thread handler */
    thread_id = osal_thread_self();

    /* Get thread name */
    if( RT_ERR_OK!=osal_thread_name(thread_id, name, sizeof(name)) ){
        osal_printf("[%s] Get osal_thread_name Fail\n", __FUNCTION__);
    }
    osal_time_sleep(THREAD_3_FIRST_SLEEP);
    do{
        osal_time_usecs_get(&take_time);
        if_take = osal_sem_take(semaphore, THREAD_3_TAKE_WAIT);
        osal_time_usecs_get(&return_time);
        if( RT_ERR_OK==if_take ){
            sem[2] = 1;

            /* Check Max Semaphore */
            if( INIT_SEM_COUNTER<(sem[0]+sem[1]+sem[2]) ){
                even_fail++;
            }

            SHOWMESSAGE("[%s Take, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_time_sleep(THREAD_3_TAKE_SLEEP);
            sem[2] = 0;
            SHOWMESSAGE("[%s Give, sem: %d, %d, %d]\n", name, sem[0], sem[1], sem[2]);
            osal_sem_give(semaphore);
            take_num_3++;
            osal_time_sleep(THREAD_3_GIVE_SLEEP);
        }else{
            SHOWMESSAGE("[%s timeout, take_time=%08d, return_time=%08d,"\
                    " eclapsed=%08d]\n", name, take_time, return_time, \
                    return_time-take_time);
            osal_time_sleep(THREAD_3_RETRY_SLEEP);
        }
        if( VERI_SEMAPHORE_EXIT_TIME==take_num_3 || 0==DBG_SEMAPHORE ){
            SHOWMESSAGE("[%s exit]\n", name);
#if DBG_SEMAPHORE
            SHOWMESSAGE("[take_num:%d, %d, %d]\n", \
                    take_num_1, take_num_2, take_num_3);
            while( VERI_SEMAPHORE_EXIT_TIME!=take_num_1 || \
                   VERI_SEMAPHORE_EXIT_TIME!=take_num_2 || \
                   VERI_SEMAPHORE_EXIT_TIME!=take_num_3    \
                 );
            SHOWMESSAGE("\n");
            while( (0!=sem[0]) || (0!=sem[1]) || (0!=sem[2]) );
            SHOWMESSAGE("\n");
#endif /* end of DBG_SEMAPHORE */
            if_take = osal_sem_take(semaphore, OSAL_SEM_WAIT_FOREVER);
            SHOWMESSAGE("\n");
            if( RT_ERR_OK!=if_take ){
                even_fail++;
            }else{
                osal_sem_give(semaphore);
                osal_time_sleep(THREAD_3_DESTORY_WAIT);
                osal_sem_destroy(semaphore);
            }
            if( 0==even_fail ){
                osal_result |= OSALTEST_SEMAPHORE_OK;
                osal_printf("[OSAL Semaphore Test Pass]\n");
            }

            osal_result |= OSALTEST_FINAL;
            osal_thread_exit(1);
        }
    }while(1);
}

int32 osal_sem_test(uint32 caseNo, uint32 unit)
{
    osal_thread_t osal_threadA, osal_threadB, osal_threadC;
    uint32 timeout = 0;

    /* initialize global variableis */
    take_num_1 = 0;
    take_num_2 = 0;
    take_num_3 = 0;
    even_fail  = 0;
    osal_result = 0;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running SEMAPHORE test cases", __FUNCTION__);

    /* Create Next Test Thread - Semaphore Test */
    sem[0] = sem[1] = sem[2] = 0;
    semaphore = osal_sem_create(INIT_SEM_COUNTER);

    if( NULL==(void *)semaphore ){
        osal_printf("[OSAL Create Semaphore Fail]");
        return RT_ERR_FAILED;
    }

    osal_threadA = (osal_thread_t) osal_thread_create( \
                    "Semaphore A",
                    4096,
                    4,
                    sem_test_1,
                    (void *)0
                    );
    osal_threadB = (osal_thread_t) osal_thread_create( \
                    "Semaphore B",
                    4096,
                    4,
                    sem_test_2,
                    (void *)1
                    );
    osal_threadC = (osal_thread_t) osal_thread_create( \
                    "Semaphore C",
                    4096,
                    4,
                    sem_test_3,
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

    if(OSALTEST_SEMAPHORE_OK != (osal_result & OSALTEST_SEMAPHORE_OK))
    {
        osal_printf("[OSAL Semaphore Test Fail]\n");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
