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

#ifndef __NIC_TEST_ROOT_H__
#define __NIC_TEST_ROOT_H__

/*
 * Include Files
 */
#include <sdk/sdk_test.h>

/*
 * Symbol Definition
 */

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
extern int32
nictest_run(uint32 unit, sdktest_action_t action);

#endif  /* __NIC_TEST_ROOT_H__ */

