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
//#include <linux/jiffies.h>
/*
 * Symbol Definition
 */
#define OSAL_TIME_SEC_TOLERANT              (2)            /* 2 secs */
#define OSAL_TIME_USEC_TOLERANT             (1*1000000)    /* 1 secs */
#define OSAL_TIME_TICK_TOLERANT             (5)            /* 5 ticks */
#define TICK_TOLERANT_IN_USEC               (OSAL_TIME_TICK_TOLERANT*TICK_USEC)

/*
 * Data Declaration
 */

/*
 * Macro
 */

/*
 * Function Declaration
 */

/*
    Test Function:
        Thread:osal_time_udelay
        Time  :osal_usec2Ticks, osal_seconds_get, osal_time_usecs_get
 */
int32 osal_time_test(uint32 caseNo, uint32 unit)
{
    unsigned int usec, sec;
    unsigned int expected_ticks, pTicks;
    unsigned int sec_begin, usec_begin, current_sec, current_usec;
    unsigned int total_usec_delay = 0, expected_usec, expected_sec;
    unsigned int total_sec_delay = 0;
    int i;
    int retval = 0;

    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Running TIME test cases", __FUNCTION__);

    /* osal_usec2Ticks Test */
    for(i=0;i<OSAL_TIME_TEST_NUMBER;i++){
        /* Get random usec */
        usec = rand()%OSAL_RAND_USEC;

        /* Expected ticks */
        expected_ticks = (usec + TICK_USEC - 1)/TICK_USEC;

        /* Get ticks via osal_time_usec2Ticks_get */
        osal_time_usec2Ticks_get(usec, &pTicks);

        /* Compare the result */
        if( expected_ticks!=pTicks ){
            DBG_printf(DBG_TIME, "Fail: pTicks=%d, usec=%d, expected_ticks=%d\n",\
                    pTicks, usec, expected_ticks);
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }
    }

    /* osal_seconds_get
       osal_time_usecs_get
       osal_time_udelay
     */
    /* Get the current time in sec */
    if( RT_ERR_OK!=osal_time_seconds_get(&sec_begin) ){
        osal_printf("OSAL Time osal_time_seconds_get Test Fail\n");
        retval = RT_ERR_FAILED;
        goto time_test_result;
    }

    /* Get the current time in usec */
    if( RT_ERR_OK!=osal_time_usecs_get(&usec_begin) ){
        osal_printf("OSAL Time osal_time_usecs_get Test Fail\n");
        retval = RT_ERR_FAILED;
        goto time_test_result;
    }

    /* OSAL Time osal_time_udelay, osal_time_seconds_get
     * osal_time_usecs_get Test
     */
    for(i=0;i<OSAL_TIME_TEST_NUMBER;i++){
        /* Get random usec */
        usec = rand()%OSAL_RAND_USEC;
        total_usec_delay += usec;

        /* Delay usec */
        osal_time_udelay(usec);

        /* Get the current time in sec */
        if( RT_ERR_OK!=osal_time_seconds_get(&current_sec) ){
            osal_printf("OSAL Time osal_time_seconds_get Test Fail\n");
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }

        /* Get the current time in usec*/
        if( RT_ERR_OK!=osal_time_usecs_get(&current_usec) ){
            osal_printf("OSAL Time osal_time_usecs_get Test Fail\n");
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }

        /*
         * Compare the current time with usec delay
         */
        expected_usec = usec_begin + total_usec_delay;
        expected_sec = sec_begin + (total_usec_delay/1000000);

        DBG_printf(DBG_TIME, "[%03d]expected_sec=%d, current_sec=%d\n", \
                i, expected_sec, current_sec);

        /* Compare the current time in usec */
        if( ( (expected_usec+OSAL_TIME_USEC_TOLERANT) < current_usec) ||  \
            ( (current_usec+TICK_TOLERANT_IN_USEC) < (expected_usec) ) ){
            osal_printf("Time Fail:expected_usec=%d, current_usec=%d\n", \
                    expected_usec, current_usec);
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }

        /* Compare the current time in sec */
        if( (((expected_sec+OSAL_TIME_SEC_TOLERANT)<current_sec) || \
                    ((current_sec+1)<(expected_sec)))  ){
            osal_printf("[1]Time Fail:expected_sec=%d, current_sec=%d\n", \
                    expected_sec, current_sec);
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }
    }

    /* osal_time_sleep */
    if( RT_ERR_OK!=osal_time_seconds_get(&sec_begin) ){
        osal_printf("OSAL Time osal_time_seconds_get Test Fail\n");
        retval = RT_ERR_FAILED;
        goto time_test_result;
    }
    for(i=0;i<OSAL_TIME_TEST_NUMBER;i++){
        sec = rand()%OSAL_RAND_SEC;
        total_sec_delay += sec;
        osal_time_sleep(sec);
        if( RT_ERR_OK!=osal_time_seconds_get(&current_sec) ){
            osal_printf("OSAL Time osal_time_seconds_get Test Fail\n");
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }
        expected_sec = sec_begin + total_sec_delay;

        DBG_printf(DBG_TIME, "[%03d]expected_sec=%d, current_sec=%d\n", \
                i, expected_sec, current_sec);
        if( (((expected_sec+OSAL_TIME_SEC_TOLERANT)<current_sec) || \
            ((current_sec+1)<(expected_sec))) ){
            osal_printf("[2]Time Fail:expected_sec=%d, current_sec=%d\n", \
                    expected_sec, current_sec);
            retval = RT_ERR_FAILED;
            goto time_test_result;
        }
    }

time_test_result:
    if (RT_ERR_FAILED == retval)
    {
        osal_printf("[OSAL Time Test Fail]\n");
        return RT_ERR_FAILED;
    }
    else
    {
        osal_printf("[OSAL Time Test Pass]\n");
        return RT_ERR_OK;
    }
}
