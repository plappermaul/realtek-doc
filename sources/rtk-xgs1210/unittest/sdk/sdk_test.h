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
 * Purpose : Definition of SDK test APIs in the SDK
 *
 * Feature : SDK test APIs
 *
 */

#ifndef __SDK_TEST_H__
#define __SDK_TEST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

#define SDKTEST_GRP_NONE                    0x80000000
#define SDKTEST_GRP_ALL                     0x00000001
#define SDKTEST_GRP_HAL                     0x00000002
#define SDKTEST_GRP_DOT1X                   0x00000004
#define SDKTEST_GRP_EEE                     0x00000008
#define SDKTEST_GRP_FLOWCTRL                0x00000010
#define SDKTEST_GRP_L2                      0x00000020
#define SDKTEST_GRP_L3                      0x00000040
#define SDKTEST_GRP_OAM                     0x00000080
#define SDKTEST_GRP_PIE                     0x00000100
#define SDKTEST_GRP_PORT                    0x00000200
#define SDKTEST_GRP_QOS                     0x00000400
#define SDKTEST_GRP_RATE                    0x00000800
#define SDKTEST_GRP_SEC                     0x00001000
#define SDKTEST_GRP_STP                     0x00002000
#define SDKTEST_GRP_SWITCH                  0x00004000
#define SDKTEST_GRP_TRAP                    0x00008000
#define SDKTEST_GRP_TRUNK                   0x00010000
#define SDKTEST_GRP_VLAN                    0x00020000
#define SDKTEST_GRP_SVLAN                   0x00040000
#define SDKTEST_GRP_FILTER                  0x00080000
#define SDKTEST_GRP_NIC                     0x00100000
#define SDKTEST_GRP_OSAL                    0x00200000
#define SDKTEST_GRP_STATS                   0x00400000
#define SDKTEST_GRP_MIRROR                  0x00800000
#define SDKTEST_GRP_MPLS                    0x01000000
#define SDKTEST_GRP_TIME                    0x02000000
#define SDKTEST_GRP_LED                     0x04000000
#define SDKTEST_GRP_ACL                     0x08000000

typedef struct unit_test_case_s
{
    int32 no;
    char *name;
     int32(*fp) (uint32, uint32);
    uint32 group;
} unit_test_case_t;

#define UNIT_TEST_CASE( case_no, func, group_mask ) \
    { \
        no: case_no, \
        name: #func, \
        fp: func, \
        group: group_mask, \
    }
/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      sdktest_run
 * Description:
 *      Test one test case or group test cases in the SDK for one specified device.
 * Input:
 *      unit  - unit id
 *      *pStr - string context
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
sdktest_run(uint32 unit, uint8 *pStr);

/* Function Name:
 *      sdktest_run_id
 * Description:
 *      Test some test cases from start to end case index in the SDK for one specified device.
 * Input:
 *      unit  - unit id
 *      start - start test case number
 *      end   - end test case number
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
sdktest_run_id(uint32 unit, uint32 start, uint32 end);

#endif  /* __SDK_TEST_H__ */
