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
 * Purpose : Definition of NIC test APIs in the SDK
 *
 * Feature : SDK NIC test APIs
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <nic/nic_test_case.h>
#include <nic/nic_test_root.h>

/*
 * Symbol Definition
 */
#define NIC_TEST_ACTION_CHK(action) \
    do { if ((action < SDKTEST_ACTION_NIC_ALL) || (action >= SDKTEST_ACTION_NIC_END)) \
            { return RT_ERR_FAILED; } } while(0)

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      nictest_run
 * Description:
 *      Test one test case or group test cases in the NIC for one specified device.
 * Input:
 *      unit   - unit id
 *      action - action id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
int32 nictest_run(uint32 unit, sdktest_action_t action)
{
    RT_LOG(LOG_FUNC_ENTER, MOD_UNITTEST, "[%s] Start to run NIC test cases.... (action:%d)\n", __FUNCTION__, action);

    NIC_TEST_ACTION_CHK(action);

    if ((SDKTEST_ACTION_NIC_DUMP_CNTR == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_9(0x00030009);      /* Dump Tx/Rx counter */
    }

    if ((SDKTEST_ACTION_NIC_DUMP_BUFFER_STATUS == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_10(0x0003000a);     /* Dump buffer status */
    }

    if ((SDKTEST_ACTION_NIC_CLEAR_CNTR == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_12(0x0003000c);     /* Clear Tx/Rx counter */
    }

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    if ((SDKTEST_ACTION_NIC_TX_TEST1 == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_3(0x00030003);      /* Tx function testing [port 2] */
    }

    if ((SDKTEST_ACTION_NIC_TX_TEST2 == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_13(0x0003000d);     /* Tx function testing [port 24] */
    }
#endif

    if ((SDKTEST_ACTION_NIC_RX_START == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_4(0x00030004);      /* Rx start */
    }

    if ((SDKTEST_ACTION_NIC_RX_STOP == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_5(0x00030005);      /* Rx stop */
    }

    if ((SDKTEST_ACTION_NIC_DEBUG_TX == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_6(0x00030006);      /* Enable Nic Tx debug */
    }

    if ((SDKTEST_ACTION_NIC_DEBUG_RX == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_7(0x00030007);      /* Enable Nic Rx debug */
    }

    if ((SDKTEST_ACTION_NIC_DEBUG_TXRX == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_8(0x00030008);      /* Enable Nic Tx/Rx debug */
    }

    if ((SDKTEST_ACTION_NIC_DEBUG_OFF == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_11(0x0003000b);     /* Disable Nic Tx/Rx debug */
    }

    if ((SDKTEST_ACTION_NIC_DUMP == action) || (SDKTEST_ACTION_NIC_ALL == action))
    {
        testNic_2(0x00030002);      /* Dump information */
    }

    if (SDKTEST_ACTION_NIC_ALL == action)
    {
        /* COMMON ACTION ADD HERE!! */
    }

     return RT_ERR_OK;
}
