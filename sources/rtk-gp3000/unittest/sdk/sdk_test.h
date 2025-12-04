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

#define GRP_NONE                    0x8000000000000000LL
#define GRP_ALL                     0x0000000000000001LL
#define GRP_HAL                     0x0000000000000002LL
#define GRP_DOT1X                   0x0000000000000004LL
#define GRP_EEE                     0x0000000000000008LL
#define GRP_FLOWCTRL                0x0000000000000010LL
#define GRP_L2                      0x0000000000000020LL
#define GRP_L3                      0x0000000000000040LL
#define GRP_OAM                     0x0000000000000080LL
#define GRP_PIE                     0x0000000000000100LL
#define GRP_PORT                    0x0000000000000200LL
#define GRP_QOS                     0x0000000000000400LL
#define GRP_RATE                    0x0000000000000800LL
#define GRP_SEC                     0x0000000000001000LL
#define GRP_STP                     0x0000000000002000LL
#define GRP_SWITCH                  0x0000000000004000LL
#define GRP_TRAP                    0x0000000000008000LL
#define GRP_TRUNK                   0x0000000000010000LL
#define GRP_VLAN                    0x0000000000020000LL
#define GRP_SVLAN                   0x0000000000040000LL
#define GRP_FILTER                  0x0000000000080000LL
#define GRP_NIC                     0x0000000000100000LL
#define GRP_OSAL                    0x0000000000200000LL
#define GRP_STATS                   0x0000000000400000LL
#define GRP_GPON                    0x0000000000800000LL
#define GRP_L34                     0x0000000001000000LL
#define GRP_CLASSF                  0x0000000002000000LL
#define GRP_CPU                     0x0000000004000000LL
#define GRP_INTR                    0x0000000008000000LL
#define GRP_MIRROR                  0x0000000010000000LL
#define GRP_AFB                     0x0000000020000000LL
#define GRP_PONMAC                  0x0000000040000000LL
#define GRP_LED                     0x0000000080000000LL
#define GRP_RLDP                    0x0000000100000000LL


#define GRP_DOT1X_BASE           (1000)
#define GRP_EEE_BASE             (2000)
#define GRP_FLOWCTRL_BASE        (4000)
#define GRP_L2_BASE              (5000)
#define GRP_L3_BASE              (6000)
#define GRP_OAM_BASE             (9000)
#define GRP_PIE_BASE             (10000)
#define GRP_PORT_BASE            (11000)
#define GRP_QOS_BASE             (12000)
#define GRP_RATE_BASE            (13000)
#define GRP_SEC_BASE             (14000)
#define GRP_STP_BASE             (15000)
#define GRP_CLASSF_BASE          (16000)
#define GRP_SWITCH_BASE          (19000)
#define GRP_TRAP_BASE            (20000)
#define GRP_TRUNK_BASE           (21000)
#define GRP_VLAN_BASE            (22000)
#define GRP_SVLAN_BASE           (23000)
#define GRP_FILTER_BASE          (24000)
#define GRP_NIC_BASE             (25000)
#define GRP_OSAL_BASE            (26000)
#define GRP_STATISTIC_BASE       (27000)
#define GRP_GPON_BASE            (28000)
#define GRP_L34_BASE             (29000)
#define GRP_EPON_BASE            (30000)
#define GRP_CPU_BASE             (31000)
#define GRP_INTR_BASE            (32000)
#define GRP_MIRROR_BASE          (33000)
#define GRP_AFB_BASE             (34000)
#define GRP_PONMAC_BASE          (35000)
#define GRP_LED_BASE             (36000)
#define GRP_RLDP_BASE            (37000)





typedef struct unit_test_case_s
{
    int32 no;
    char *name;
     int32(*fp) (uint32);
    uint64 group;
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
