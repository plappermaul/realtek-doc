/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 
 * $Date:
 *
 * Purpose : Define the test method utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

#ifndef __RT_UTIL_TEST_H__
#define __RT_UTIL_TEST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util.h>
#include <common/util/rt_util_serdes.h>
#include <rtk/l2.h>

/*
 * Symbol Definition
 */



/*
 * Data Type Declaration
 */

typedef enum rt_test_result_e
{
    RT_TEST_PASS = 0,
    RT_TEST_FAIL = -1,
}rt_test_result_t;

#define RT_TEST_OPT_VERBASE 0x1
#define RT_TEST_OPT_BIDIR  0x2  //bidirectinal test

/* RT_TEST_NIC_TX */
typedef struct rt_test_nicTxTestData_s
{
    /* param */
    uint32 unit;
    rtk_port_t txPort;
    rtk_port_t rxPort;
    uint32 pktCnt;
    uint32 pktLen;
    uint32 testOption;
    /* backup config */
    rtk_l2_newMacLrnMode_t testPortLrnMode, peerPortLrnMode;
    rtk_action_t  testPortAct, peerPortAct;
    /* result */
    rt_test_result_t result;
}rt_test_nicTxTestData_t;

/* RT_TEST_SERDES_MODE */
typedef struct rt_test_sdsTestData_s
{
    /* param */
    uint32 unit;
    rt_sds_t txSds;
    rt_sds_t rxSds;
    rtk_sds_testMode_t mode;
    uint32 msec;
    uint32 testOption;
    uint32 cntRecord;
    /* result */
    rt_test_result_t result;
}rt_test_sdsTestData_t;

typedef enum rt_test_testId_e
{
    RT_TEST_NIC_TX = 0,
    RT_TEST_SERDES_MODE,
    RT_TEST_END
}rt_test_testId_t;


typedef struct rt_test_proc_s
{
    char *testName;
    int32 (*test_init)(void *);
    int32 (*do_test)(void *);
    int32 (*test_done_cb)(void *);
    int32 (*test_iter)(void *, const int32 const *, int32 *);
    int32 (*test_cleanup)(void *);
}rt_test_proc_t;

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_test_run
 * Description:
 *      Run RTK test
 * Input:
 *      test_id   - test id
 *      data  - includes test param and the output of the test
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
extern int32 rt_test_run(int32 test_id, void *data);


#endif /* __RT_UTIL_LED_H__ */

