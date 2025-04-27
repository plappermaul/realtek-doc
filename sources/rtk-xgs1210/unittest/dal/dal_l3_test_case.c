/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <osal/time.h>
#include <dal/dal_mgmt.h>
#include <rtk/vlan.h>
#include <rtk/l3.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* HOST MAC Index*/
#define TEST_HOST_MAC_INDEX_MIN               0
#define TEST_HOST_MAC_INDEX_MAX(unit)         HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)
/* SWITCH MAC Index*/
#define TEST_SWITCH_MAC_INDEX_MIN             0
#define TEST_SWITCH_MAC_INDEX_MAX(unit)       (HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit)-1)
#define L3_RANDOM_RUN_TIMES        10

int32 dal_l3_routeSwitchMacAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mac_t  mac_temp;
    int8  inter_result1 = 1;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ( (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l3_routeSwitchMacAddr_set(unit, 0,&mac_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l3_routeSwitchMacAddr_get(unit, 0,&mac_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last;
        int32 randIdx;
        rtk_mac_t  test_mac;
        rtk_mac_t  result_mac;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_SWITCH_MAC_INDEX_MAX(unit), TEST_SWITCH_MAC_INDEX_MIN, index, index_result, index_last);

        for (index_index = 0; index_index < index_last; index_index++)
        {
            inter_result2 = index_result[index_index];
            for (randIdx = 0; randIdx < 100*L3_RANDOM_RUN_TIMES; randIdx++)
            {
                osal_memset(&test_mac, 0, sizeof(rtk_mac_t));

                test_mac.octet[0] = ut_rand()&0xff;
                test_mac.octet[1] = ut_rand()&0xff;
                test_mac.octet[2] = ut_rand()&0xff;
                test_mac.octet[3] = ut_rand()&0xff;
                test_mac.octet[4] = ut_rand()&0xff;
                test_mac.octet[5] = ut_rand()&0xff;

                expect_result =  (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_l3_routeSwitchMacAddr_set(unit, index[index_index], (rtk_mac_t *)&test_mac);

                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l3_routeSwitchMacAddr_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);

                    osal_memset(&result_mac, 0, sizeof(rtk_mac_t));

                    ret = rtk_l3_routeSwitchMacAddr_get(unit, index[index_index], (rtk_mac_t *)&result_mac);

                    RT_TEST_IS_EQUAL_INT("rtk_l3_routeSwitchMacAddr_get (unit %u, index %u)"
                                 , ret, expect_result, index[index_index], unit);

                    /*Check Entry Content*/
                    ret = osal_memcmp(&result_mac, &test_mac, sizeof(rtk_mac_t));

                    RT_TEST_IS_EQUAL_INT("rtk_l3_routeSwitchMacAddr_set/get value unequal (unit %u, index %u)"
                                , ret, RT_ERR_OK, unit, index[index_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l3_routeSwitchMacAddr_set (unit %u, index %u)"
                                        , ret, RT_ERR_OK, unit, index[index_index]);
                }

            }
            expect_result =  (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            /*ret = rtk_l3_routeHostMacAddr_get(unit, index[index_index], (rtk_mac_t *)&result_mac);*/
               if (RT_ERR_OK == expect_result)
               {
                   RT_TEST_IS_EQUAL_INT("rtk_l3_routeSwitchMacAddr_get (unit %u, index %u)"
                                , ret, expect_result, unit, index[index_index]);
               }
            else
            {
                  RT_TEST_IS_NOT_EQUAL_INT("rtk_l3_routeSwitchMacAddr_get (unit %u, index %u)"
                                   , ret, RT_ERR_OK, unit, index[index_index]);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l3_routeSwitchMacAddr_test*/

