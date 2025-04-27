/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 83908 $
 * $Date: 2017-12-01 17:06:12 +0800 (Fri, 01 Dec 2017) $
 *
 * Purpose : Definition of DAL test APIs in the SDK
 *
 * Feature : DAL test APIs
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>
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
#include <rtk/port.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define PORT_RANDOM_RUN_TIMES                 100
#define TEST_LINKMON_RATE_MIN                 RTK_LINKMON_SCAN_INTERVAL_MIN

#define TEST_VLAN_PORT_ISO_ENTRY_MIN          0
#define TEST_VLAN_PORT_ISO_ENTRY_MAX(unit)    HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit)-1

#define TEST_VLAN_PORT_ISO_SRC_MIN            VLAN_ISOLATION_SRC_INNER
#define TEST_VLAN_PORT_ISO_SRC_MAX            VLAN_ISOLATION_SRC_FORWARD

#define TEST_SPEC_CONGEST_TIME_MIN            0
#define TEST_SPEC_CONGEST_TIME_MAX            RTK_PORT_SPEC_CONGEST_TIME_MAX

#define TEST_PORT_LINK_STATUS_ID_MIN          PORT_LINKDOWN
#define TEST_PORT_LINK_STATUS_ID_MAX          PORT_LINKUP

#define TEST_PORT_SPEED_ID_MIN                PORT_SPEED_10M
#define TEST_PORT_SPEED_ID_MAX                PORT_SPEED_10G

#define TEST_PORT_DUPLEX_ID_MIN               PORT_HALF_DUPLEX
#define TEST_PORT_DUPLEX_ID_MAX               PORT_FULL_DUPLEX

#define TEST_PORT_MASTERSLAVE_ID_MIN          PORT_AUTO_MODE
#define TEST_PORT_MASTERSLAVE_ID_MAX          PORT_MASTER_MODE

#define TEST_PORT_MEDIA_ID_MIN                PORT_MEDIA_COPPER
#define TEST_PORT_MEDIA_ID_MAX                PORT_MEDIA_FIBER /* PORT_MEDIA_FIBER */

#define TEST_LINKMON_SCAN_INTERVAL_MIN        10000

int32 dal_port_isolation_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  portMask_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_isolation_set(unit, 0, &portMask_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_isolation_get(unit, 0, &portMask_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_portmask_t  portMask;
        rtk_portmask_t  result_portMask;

        int32 randIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            for(randIdx = 0; randIdx < PORT_RANDOM_RUN_TIMES; randIdx++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                HWP_GET_ALL_PORTMASK(unit, portMask);
                portMask.bits[0] = (portMask.bits[0]) & (ut_rand() & BITMASK_29B);

                ret = rtk_port_isolation_set(unit, port[port_index], &portMask);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_set (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);

                    ret = rtk_port_isolation_get(unit, port[port_index], &result_portMask);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u,)"
                                     , ret, expect_result, unit, port[port_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_set/get value unequal (unit %u, port %u)"
                                     , result_portMask.bits[0], portMask.bits[0], unit, port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_isolation_set (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }
            }


        }
    }
    return RT_ERR_OK;
}   /*end of dal_port_isolation_test*/

int32 dal_port_isolation_add_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  por_temp = 0;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_isolation_add(unit, 0, por_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_isolation_del(unit, 0, por_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_portmask_t  portMask;
        rtk_portmask_t  result_portMask;

        int32 randIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            for(randIdx = 0; randIdx < PORT_RANDOM_RUN_TIMES; randIdx++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                HWP_GET_ALL_PORTMASK(unit, portMask);
                portMask.bits[0] = (portMask.bits[0]) & (ut_rand() & BITMASK_29B);

                ret = rtk_port_isolation_set(unit, port[port_index], &portMask);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_set (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);

                    ret = rtk_port_isolation_get(unit, port[port_index], &result_portMask);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u,)"
                                     , ret, expect_result, unit, port[port_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_set/get value unequal (unit %u, port %u)"
                                     , result_portMask.bits[0], portMask.bits[0], unit, port[port_index]);

                    /*Add rand port*/
                    UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, por_temp);
                    ret = rtk_port_isolation_add(unit, port[port_index], por_temp);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u, iso_port %u)"
                                     , ret, expect_result, unit, port[port_index], por_temp);

                    ret = rtk_port_isolation_get(unit, port[port_index], &result_portMask);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u,)"
                                     , ret, expect_result, unit, port[port_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_add port unequal (unit %u, port %u)"
                                     , RTK_PORTMASK_IS_PORT_SET(result_portMask, por_temp)? TRUE : FALSE
                                     , TRUE, unit, port[port_index]);

                    ret = rtk_port_isolation_del(unit, port[port_index], por_temp);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u,)"
                                     , ret, expect_result, unit, port[port_index]);

                    ret = rtk_port_isolation_get(unit, port[port_index], &result_portMask);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_get (unit %u, port %u,)"
                                     , ret, expect_result, unit, port[port_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_isolation_add port unequal (unit %u, port %u)"
                                     , RTK_PORTMASK_IS_PORT_CLEAR(result_portMask, por_temp)? TRUE : FALSE
                                     , TRUE, unit, port[port_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_isolation_set (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }
            }

        }
    }
    return RT_ERR_OK;
}   /*end of dal_port_isolation_test*/

int32 dal_port_backpressureEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_backpressureEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_backpressureEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_backpressureEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_port_backpressureEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }
            ret = rtk_port_backpressureEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_backpressureEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_port_backpressureEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_backpressureEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_backpressureEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_backpressureEnable_test*/

int32 dal_port_adminEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_adminEnable_set(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_adminEnable_get(unit, TEST_PORT_ID_MIN(unit), &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_adminEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_port_adminEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_adminEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_port_adminEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_adminEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MIN, enable_max);
        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_adminEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_port_adminEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_adminEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_adminEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_adminEnable_test*/

int32 dal_port_linkMon_swScanPorts_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  port_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_linkMon_swScanPorts_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_linkMon_swScanPorts_get(unit, &port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_portmask_t portMsk;
        rtk_portmask_t  portMsk_result;
        int32 randIdx;
        uint8   portMsk_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];


        for (randIdx = 0; randIdx <= PORT_RANDOM_RUN_TIMES; randIdx++)
        {
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &portMsk);
            ret = rtk_port_linkMon_swScanPorts_set(unit, &portMsk);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
            else
                    expect_result = RT_ERR_OK;
            if (RT_ERR_OK == expect_result)
            {
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_swScanPorts_set (unit %u, portMsk.bit%s)"
                                , ret, expect_result, unit, portMsk_pm_str);

                ret = rtk_port_linkMon_swScanPorts_get(unit, &portMsk_result);
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_swScanPorts_get (unit %u, portMsk.bit%s)"
                                 , ret, expect_result, unit, portMsk_pm_str);
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_swScanPorts_set/get value unequal (unit %u, portMsk.bit%s)"
                                 , portMsk_result.bits[0], portMsk.bits[0], unit, portMsk_pm_str);
            }
            else
            {
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_linkMon_swScanPorts_set (unit %u, portMsk.bit%s)"
                                    , ret, RT_ERR_OK, unit, portMsk_pm_str);
            }
        }
        if(expect_result != RT_ERR_OK){
            ret = rtk_port_linkMon_swScanPorts_get(unit, &portMsk_result);
            RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_linkMon_swScanPorts_set (unit %u, portMsk.bit%s)"
                          , ret, RT_ERR_OK, unit, portMsk_pm_str);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_port_linkMon_swScanPorts_test*/

extern int32 dal_port_vlanBasedIsolationEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_vlanIsolationEntry_t  entry_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_vlanBasedIsolationEntry_set(unit, TEST_VLAN_PORT_ISO_ENTRY_MIN, &entry_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_vlanBasedIsolationEntry_get(unit, TEST_VLAN_PORT_ISO_ENTRY_MIN,&entry_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_vlan_t  vlan[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_result[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_index;
        int32  vlan_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32 randIdx;

        rtk_port_vlanIsolationEntry_t  test_entry;
        rtk_port_vlanIsolationEntry_t  result_entry;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_PORT_ISO_ENTRY_MAX(unit), TEST_VLAN_PORT_ISO_ENTRY_MIN, entry, entry_result, entry_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vlan, vlan_result, vlan_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            for (vlan_index = 0; vlan_index <= vlan_last; vlan_index++)
            {
                inter_result3 = vlan_result[vlan_index];
                for (enable_index = 0; enable_index <= enable_last; enable_index++)
                {
                    inter_result4 = enable_result[enable_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    for (randIdx = 0; randIdx <= PORT_RANDOM_RUN_TIMES; randIdx++)
                    {
                        osal_memset(&test_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                        test_entry.enable = enable[enable_index];
                        test_entry.vid = vlan[vlan_index];
                        UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_entry.portmask);

                        ret = rtk_port_vlanBasedIsolationEntry_set(unit, entry[entry_index], &test_entry);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                            , ret, expect_result, unit, entry[entry_index], test_entry.vid, test_entry.enable);

                            osal_memset(&result_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                            ret = rtk_port_vlanBasedIsolationEntry_get(unit, entry[entry_index], &result_entry);
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_get (unit %u, entry %u, vid %u, enable %u)"
                                             , ret, expect_result, unit, entry[entry_index], result_entry.vid, result_entry.enable);

                            ret = osal_memcmp(&result_entry, &test_entry, sizeof(rtk_port_vlanIsolationEntry_t));
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set/get value unequal (unit %u, entry %u, vid %u, enable %u)"
                                             , ret, RT_ERR_OK, unit, entry[entry_index], result_entry.vid, result_entry.enable);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                                , ret, RT_ERR_OK, unit, entry[entry_index], test_entry.vid, test_entry.enable);
                        }
                    }
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result != RT_ERR_OK)
                {
                    ret = rtk_port_vlanBasedIsolationEntry_get(unit, entry[entry_index], &result_entry);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_get (unit %u, entry %u, vid %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, entry[entry_index], result_entry.vid, result_entry.enable);
                }

            }
            if((entry[entry_index] <= TEST_VLAN_PORT_ISO_ENTRY_MAX(unit)) && (unit <= TEST_MAX_UNIT_ID))
            {
                osal_memset(&test_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                test_entry.enable = TEST_ENABLE_STATUS_MIN;
                test_entry.vid = TEST_VLAN_ID_MIN; /*InValid this entry*/
                ret = rtk_port_vlanBasedIsolationEntry_set(unit, entry[entry_index], &test_entry);
                RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                            , ret, expect_result, unit, entry[entry_index], test_entry.vid, test_entry.enable);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  entry;
        uint32  entry_max;
        rtk_vlan_t  vlan;
        rtk_vlan_t  vlan_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        int32 randIdx;

        rtk_port_vlanIsolationEntry_t  test_entry;
        rtk_port_vlanIsolationEntry_t  result_entry;

        G_VALUE(TEST_VLAN_PORT_ISO_ENTRY_MAX(unit), entry_max);
        G_VALUE(TEST_VLAN_ID_MAX, vlan_max);
        G_VALUE(TEST_ENABLE_STATUS_MIN, enable_max);

        for (L_VALUE(TEST_VLAN_PORT_ISO_ENTRY_MIN, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_VLAN_PORT_ISO_ENTRY_MIN, TEST_VLAN_PORT_ISO_ENTRY_MAX(unit), inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, vlan); vlan <= vlan_max; vlan++)
            {
                ASSIGN_EXPECT_RESULT(vlan, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    for (randIdx = 0; randIdx <= PORT_RANDOM_RUN_TIMES; randIdx++)
                    {
                        osal_memset(&test_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                        test_entry.enable = enable;
                        test_entry.vid = vlan;
                        UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_entry.portmask);

                        ret = rtk_port_vlanBasedIsolationEntry_set(unit, entry, &test_entry);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                            , ret, expect_result, unit, entry, test_entry.vid, test_entry.enable);

                            osal_memset(&result_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                            ret = rtk_port_vlanBasedIsolationEntry_get(unit, entry, &result_entry);
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_get (unit %u, entry %u, vid %u, enable %u)"
                                             , ret, expect_result, unit, entry, result_entry.vid, result_entry.enable);

                            ret = osal_memcmp(&result_entry, &test_entry, sizeof(rtk_port_vlanIsolationEntry_t));
                            RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set/get value unequal (unit %u, entry %u, vid %u, enable %u)"
                                             , ret, RT_ERR_OK, unit, entry, result_entry.vid, result_entry.enable);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                                , ret, RT_ERR_OK, unit, entry, test_entry.vid, test_entry.enable);
                        }
                    }
                   }
            }
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result != RT_ERR_OK)
            {
                ret = rtk_port_vlanBasedIsolationEntry_get(unit, entry, &result_entry);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_get (unit %u, entry %u, vid %u, enable %u)"
                    , ret, RT_ERR_OK, unit, entry,result_entry.vid, result_entry.enable);
            }
            if((entry <= TEST_VLAN_PORT_ISO_ENTRY_MAX(unit)) && (unit <= TEST_MAX_UNIT_ID))
            {
                osal_memset(&test_entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
                test_entry.enable = TEST_ENABLE_STATUS_MIN;
                test_entry.vid = TEST_VLAN_ID_MIN; /*InValid this entry*/
                ret = rtk_port_vlanBasedIsolationEntry_set(unit, entry, &test_entry);
                RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolationEntry_set (unit %u, entry %u, vid %u, enable %u)"
                                            , ret, expect_result, unit, entry, test_entry.vid, test_entry.enable);
            }
        }
    }

    return RT_ERR_OK;
}/* dal_port_vlanBasedIsolationEntry_test() */

extern int32 dal_port_vlanBasedIsolation_vlanSource_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_vlanIsolationSrc_t  src_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_vlanBasedIsolation_vlanSource_set(unit, TEST_VLAN_PORT_ISO_SRC_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_vlanBasedIsolation_vlanSource_get(unit, &src_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_vlanIsolationSrc_t    src[UNITTEST_MAX_TEST_VALUE];
        int32  src_result[UNITTEST_MAX_TEST_VALUE];
        int32  src_index;
        int32  src_last;

        rtk_port_vlanIsolationSrc_t  result_src;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_PORT_ISO_SRC_MAX, TEST_VLAN_PORT_ISO_SRC_MIN, src, src_result, src_last);

        for (src_index = 0; src_index <= src_last; src_index++)
        {
            inter_result2 = src_result[src_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 )? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_port_vlanBasedIsolation_vlanSource_set(unit, src[src_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set (unit %u, src %u)"
                                , ret, expect_result, unit, src[src_index]);

                ret = rtk_port_vlanBasedIsolation_vlanSource_get(unit, &result_src);
                RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_get (unit %u, src %u)"
                                 , ret, expect_result, unit, result_src);

                RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set/get value unequal (unit %u, src %u)"
                                 , result_src, src[src_index], unit, src[src_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set (unit %u, src %u)"
                                    , ret, RT_ERR_OK, unit, src[src_index]);
            }
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (RT_ERR_OK != expect_result)
            {
                ret = rtk_port_vlanBasedIsolation_vlanSource_get(unit, NULL);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_get (unit %u, src %u)"
                        , ret, RT_ERR_OK, unit, src[src_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_vlanIsolationSrc_t     src;
        rtk_port_vlanIsolationSrc_t     src_max;

        rtk_port_vlanIsolationSrc_t     result_src;

        G_VALUE(TEST_VLAN_PORT_ISO_SRC_MAX, src_max);

        for (L_VALUE(TEST_VLAN_PORT_ISO_SRC_MIN, src); src <= src_max; src++)
        {
            ASSIGN_EXPECT_RESULT(src, TEST_VLAN_PORT_ISO_SRC_MIN, TEST_VLAN_PORT_ISO_SRC_MAX, inter_result2);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_port_vlanBasedIsolation_vlanSource_set(unit, src);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set (unit %u, src %u)"
                                    , ret, expect_result, unit, src);

                    ret = rtk_port_vlanBasedIsolation_vlanSource_get(unit, &result_src);
                    RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_get (unit %u, src %u)"
                                     , ret, expect_result, unit, result_src);

                    RT_TEST_IS_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set/get value unequal (unit %u, src %u)"
                                     , src, result_src, unit, src);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_vlanBasedIsolation_vlanSource_set (unit %u, src %u)"
                                        , ret, RT_ERR_OK, unit, src);

                }


        }
    }

    return RT_ERR_OK;
}  /*dal_port_vlanBasedIsolation_vlanSource_test*/

int32 dal_port_rxEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_rxEnable_set(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_rxEnable_get(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_rxEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_rxEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_port_rxEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_rxEnable_get (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], result_enable);

                    RT_TEST_IS_EQUAL_INT("rtk_port_rxEnable_set/get isn't equal (unit %u, port %u, enable %u)"
                                    , enable[enable_index], result_enable, unit, port[port_index], enable[enable_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_rxEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_rxEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_rxEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_rxEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_rxEnable_test*/

extern int32 dal_port_specialCongest_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_specialCongest_set(unit, TEST_PORT_ID_MIN(unit), TEST_SPEC_CONGEST_TIME_MIN)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  second[UNITTEST_MAX_TEST_VALUE];
        int32  second_result[UNITTEST_MAX_TEST_VALUE];
        int32  second_index;
        int32  second_last;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_SPEC_CONGEST_TIME_MAX, TEST_SPEC_CONGEST_TIME_MIN, second, second_result, second_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            for (second_index = 0; second_index <= second_last; second_index++)
            {
                inter_result3 = second_result[second_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_specialCongest_set(unit, port[port_index], second[second_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_specialCongest_set (unit %u, port %u, second %u)"
                                    , ret, expect_result, unit, port[port_index], second[second_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_specialCongest_set (unit %u, port %u, second %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], second[second_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  second;
        uint32  second_max;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_SPEC_CONGEST_TIME_MAX, second_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_SPEC_CONGEST_TIME_MIN, second); second <= second_max; second++)
            {
                ASSIGN_EXPECT_RESULT(second, TEST_SPEC_CONGEST_TIME_MIN, TEST_SPEC_CONGEST_TIME_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_specialCongest_set(unit, port, second);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_specialCongest_set (unit %u, port %u, second %u)"
                                    , ret, expect_result, unit, port, second);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_specialCongest_set (unit %u, port %u, second %u)"
                                        , ret, RT_ERR_OK, unit, port, second);
                }
            }
        }
    }

    return RT_ERR_OK;
}/*end of dal_port_specialCongest_test*/

extern int32 dal_port_link_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_linkStatus_t linkSt_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_link_get(unit, TEST_PORT_ID_MIN(unit), &linkSt_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_linkStatus_t result_linkSt;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_link_get(unit, port[port_index], &result_linkSt);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_link_get (unit %u, port %u, result_linkSt %u)"
                                , ret, expect_result, unit, port[port_index], result_linkSt);
                ret = (TEST_PORT_LINK_STATUS_ID_MAX >= result_linkSt)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_link_get result failed(unit %u, port %u, result_linkSt %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], result_linkSt);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_link_get (unit %u, port %u, result_linkSt %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], result_linkSt);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;

        rtk_port_linkStatus_t result_linkSt;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_link_get(unit, port, &result_linkSt);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_link_get (unit %u, port %u, result_linkSt %u)"
                                , ret, expect_result, unit, port, result_linkSt);

                ret = (TEST_PORT_LINK_STATUS_ID_MAX >= result_linkSt)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_link_get result failed(unit %u, port %u, result_linkSt %u)"
                                , ret, RT_ERR_OK, unit, port, result_linkSt);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_link_get (unit %u, port %u, result_linkSt %u)"
                                , ret, RT_ERR_OK, unit, port, result_linkSt);
            }
        }
    }

    return RT_ERR_OK;
}/*end of dal_port_link_test*/

extern int32 dal_port_speedDuplex_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_speed_t  speed_temp;
    rtk_port_duplex_t duplex_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_speedDuplex_get(unit, TEST_PORT_ID_MIN(unit), &speed_temp, &duplex_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_speed_t result_speed;
        rtk_port_duplex_t result_duplex;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_speedDuplex_get(unit, port[port_index], &result_speed, &result_duplex);
            /*Once this port is LINKDOWN, then this API will not result real value.*/
            if ((RT_ERR_OK == expect_result) && (RT_ERR_PORT_LINKDOWN != ret))
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get (unit %u, port %u, result_speed %u, result_duplex %u)"
                                , ret, expect_result, unit, port[port_index], result_speed, result_duplex);

                ret = (TEST_PORT_SPEED_ID_MAX >= result_speed)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get result_speed failed(unit %u, port %u, result_speed %u, result_duplex %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index], result_speed, result_duplex);

                ret = (TEST_PORT_DUPLEX_ID_MAX >= result_duplex)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get result)duplex failed(unit %u, port %u, result_speed %u, result_duplex %u)"
                                   , ret, RT_ERR_OK, unit, port[port_index], result_speed, result_duplex);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_link_get (unit %u, port %u, result_speed %u, result_duplex %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], result_speed, result_duplex);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;

        rtk_port_speed_t result_speed;
        rtk_port_duplex_t result_duplex;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_speedDuplex_get(unit, port, &result_speed, &result_duplex);
            /*Once this port is LINKDOWN, then this API will not result real value.*/
            if ((RT_ERR_OK == expect_result) && (RT_ERR_PORT_LINKDOWN != ret))
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get (unit %u, port %u, result_speed %u, result_duplex %u)"
                                , ret, expect_result, unit, port, result_speed, result_duplex);
                ret = (TEST_PORT_SPEED_ID_MAX >= result_speed)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get result_speed failed(unit %u, port %u, result_speed %u, result_duplex %u)"
                                    , ret, RT_ERR_OK, unit, port, result_speed, result_duplex);

                ret = (TEST_PORT_DUPLEX_ID_MAX >= result_duplex)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_speedDuplex_get result_duplex failed(unit %u, port %u, result_speed %u, result_duplex %u)"
                                   , ret, RT_ERR_OK, unit, port, result_speed, result_duplex);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_speedDuplex_get (unit %u, port %u, result_speed %u, result_duplex %u)"
                                , ret, RT_ERR_OK, unit, port, result_speed, result_duplex);
            }
        }
    }

    return RT_ERR_OK;
}/*end of dal_port_speedDuplex_test*/

extern int32 dal_port_flowctrl_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  txStatus_temp;
    uint32  rxStatus_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_flowctrl_get(unit, TEST_PORT_ID_MIN(unit), &txStatus_temp, &rxStatus_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32 result_txStatus;
        uint32 result_rxStatus;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_flowctrl_get(unit, port[port_index], &result_txStatus, &result_rxStatus);
            /*Once this port is LINKDOWN, then this API will not result real value.*/
            if ((RT_ERR_OK == expect_result) && (RT_ERR_PORT_LINKDOWN != ret))
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get (unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                , ret, expect_result, unit, port[port_index], result_txStatus, result_rxStatus);
                ret = (TEST_ENABLE_STATUS_MAX >= result_txStatus)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get result_txStatus failed(unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                   , ret, RT_ERR_OK, unit, port[port_index], result_txStatus, result_rxStatus);

                ret = (TEST_ENABLE_STATUS_MAX >= result_rxStatus)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get result_rxStatus failed(unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], result_txStatus, result_rxStatus);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_flowctrl_get (unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], result_txStatus, result_rxStatus);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;

        uint32 result_txStatus;
        uint32 result_rxStatus;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_flowctrl_get(unit, port, &result_txStatus, &result_rxStatus);
            /*Once this port is LINKDOWN, then this API will not result real value.*/
            if ((RT_ERR_OK == expect_result) && (RT_ERR_PORT_LINKDOWN != ret))
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get (unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                , ret, expect_result, unit, port, result_txStatus, result_rxStatus);
                ret = (TEST_ENABLE_STATUS_MAX >= result_txStatus)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get result_txStatus failed(unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                   , ret, RT_ERR_OK, unit, port, result_txStatus, result_rxStatus);

                ret = (TEST_ENABLE_STATUS_MAX >= result_rxStatus)? RT_ERR_OK : RT_ERR_FAILED;
                RT_TEST_IS_EQUAL_INT("rtk_port_flowctrl_get result_rxStatus failed(unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                   , ret, RT_ERR_OK, unit, port, result_txStatus, result_rxStatus);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_flowctrl_get (unit %u, port %u, result_txStatus %u, result_rxStatus %u)"
                                , ret, RT_ERR_OK, unit, port, result_txStatus, result_rxStatus);
            }
        }
    }

    return RT_ERR_OK;
}/*end of dal_port_flowctrl_test*/

extern int32 dal_port_cpuPortId_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  port_temp;
    int32  expect_result = 1;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_cpuPortId_get(unit, &port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t result_port;

        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        ret = rtk_port_cpuPortId_get(unit, &result_port);
        if (RT_ERR_OK == expect_result)
        {
            RT_TEST_IS_EQUAL_INT("rtk_port_cpuPortId_get (unit %u, CPU port %u)"
                            , ret, expect_result, unit, result_port);
            ret = (TEST_PORT_ID_MAX(unit) >= result_port)? RT_ERR_OK : RT_ERR_FAILED;
            RT_TEST_IS_EQUAL_INT("rtk_port_cpuPortId_get result_ID failed(unit %u, CPU port %u)"
                               , ret, RT_ERR_OK, unit, result_port);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_cpuPortId_get (unit %u, CPU port %u)"
                            , ret, RT_ERR_OK, unit, result_port);
        }
    }
    return RT_ERR_OK;
}/*end of dal_port_cpuPortId_test*/

int32 dal_port_phyAutoNegoEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoEnable_set(unit, port[port_index], TEST_ENABLE_STATUS_MAX))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoEnable_get(unit, port[port_index], &enable_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            inter_result2 = port_result[port_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port[port_index]))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_phyAutoNegoEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_port_phyAutoNegoEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_port_phyAutoNegoEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoEnable_set(unit, port, TEST_ENABLE_STATUS_MAX))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoEnable_get(unit, port, &enable_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_phyAutoNegoEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_port_phyAutoNegoEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_phyAutoNegoEnable_test*/

int32 dal_port_phyAutoNegoAbility_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_phy_ability_t  phy_ability_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        int32  randIdx;
        rtk_port_phy_ability_t  test_phy_ability;
        rtk_port_phy_ability_t  result_phy_ability;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoAbility_set(unit, port[port_index], &phy_ability_temp))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoAbility_get(unit, port[port_index], &phy_ability_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            inter_result2 = port_result[port_index];
            for (randIdx = 0; randIdx <= PORT_RANDOM_RUN_TIMES; randIdx++)
            {
                osal_memset(&test_phy_ability, 0, sizeof(rtk_port_phy_ability_t));

                test_phy_ability.AsyFC     = (ut_rand() & BITMASK_1B);
                test_phy_ability.FC = (ut_rand() & BITMASK_1B);
                test_phy_ability.Half_10 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Full_10 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Half_100 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Full_100 = (ut_rand() & BITMASK_1B);
                if (HWP_GE_PORT(unit, port[port_index]))
                {
                    test_phy_ability.Half_1000 = 0;
                    test_phy_ability.Full_1000 = (ut_rand() & BITMASK_1B);
                }
                else
                {
                    test_phy_ability.Half_1000 = 0;
                    test_phy_ability.Full_1000 = 0;
                }
#if defined (CONFIG_SDK_RTL8390)
                if (HWP_8390_50_FAMILY(unit))
                {
                    if (HWP_10GE_PORT(unit, port[port_index]))
                    {
                        test_phy_ability.Half_10G = (ut_rand() & BITMASK_1B);
                        test_phy_ability.Full_10G = (ut_rand() & BITMASK_1B);
                    }
                    else
                    {
                        test_phy_ability.Half_10G = 0;
                        test_phy_ability.Full_10G = 0;
                    }
                }
#endif

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_phyAutoNegoAbility_set(unit, port[port_index], &test_phy_ability);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_set (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);

                    osal_memset(&result_phy_ability, 0, sizeof(rtk_port_phy_ability_t));
                    ret = rtk_port_phyAutoNegoAbility_get(unit, port[port_index], &result_phy_ability);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_get (unit %u, port %u)"
                                     , ret, expect_result, unit, port[port_index]);

                    ret = osal_memcmp(&test_phy_ability, &result_phy_ability, sizeof(rtk_port_phy_ability_t));
                    if (ret != 0)
                    {
                        osal_printf("test_phy_ability.Half_10 = %d, result_phy_ability.Half_10 = %d\n\r", test_phy_ability.Half_10, result_phy_ability.Half_10);
                        osal_printf("test_phy_ability.Full_10 = %d, result_phy_ability.Full_10 = %d\n\r", test_phy_ability.Full_10, result_phy_ability.Full_10);
                        osal_printf("test_phy_ability.Half_100 = %d, result_phy_ability.Half_100 = %d\n\r", test_phy_ability.Half_100, result_phy_ability.Half_100);
                        osal_printf("test_phy_ability.Full_100 = %d, result_phy_ability.Full_100 = %d\n\r", test_phy_ability.Full_100, result_phy_ability.Full_100);
                        osal_printf("test_phy_ability.Half_1000 = %d, result_phy_ability.Half_1000 = %d\n\r", test_phy_ability.Half_1000, result_phy_ability.Half_1000);
                        osal_printf("test_phy_ability.Full_1000 = %d, result_phy_ability.Full_1000 = %d\n\r", test_phy_ability.Full_1000, result_phy_ability.Full_1000);
#if defined (CONFIG_SDK_RTL8390)
                        if(HWP_8390_50_FAMILY(unit))
                        {
                            osal_printf("test_phy_ability.Half_10G = %d, result_phy_ability.Half_10G = %d\n\r", test_phy_ability.Half_10G, result_phy_ability.Half_10G);
                            osal_printf("test_phy_ability.Full_10G = %d, result_phy_ability.Full_10G = %d\n\r", test_phy_ability.Full_10G, result_phy_ability.Full_10G);
                        }
#endif
                        osal_printf("test_phy_ability.FC = %d, result_phy_ability.FC = %d\n\r", test_phy_ability.FC, result_phy_ability.FC);
                        osal_printf("test_phy_ability.AsyFC = %d, result_phy_ability.AsyFC = %d\n\r", test_phy_ability.AsyFC, result_phy_ability.AsyFC);
                    }
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_set/get value unequal (unit %u, port %u)"
                                     , ret, RT_ERR_OK, unit, port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoAbility_set (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }
            }

            ret = rtk_port_phyAutoNegoAbility_get(unit, port[port_index], &result_phy_ability);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoAbility_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;

        int32  randIdx;

        rtk_port_phy_ability_t  test_phy_ability;
        rtk_port_phy_ability_t  result_phy_ability;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoAbility_set(unit, port, &phy_ability_temp))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyAutoNegoAbility_get(unit, port, &phy_ability_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);

            for (randIdx = 0; randIdx <= PORT_RANDOM_RUN_TIMES; randIdx++)
            {
                osal_memset(&test_phy_ability, 0, sizeof(rtk_port_phy_ability_t));

                test_phy_ability.AsyFC     = (ut_rand() & BITMASK_1B);
                test_phy_ability.FC = (ut_rand() & BITMASK_1B);
                test_phy_ability.Half_10 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Full_10 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Half_100 = (ut_rand() & BITMASK_1B);
                test_phy_ability.Full_100 = (ut_rand() & BITMASK_1B);
                if (HWP_GE_PORT(unit, port))
                {
                    test_phy_ability.Half_1000 = 0;
                    test_phy_ability.Full_1000 = (ut_rand() & BITMASK_1B);
                }
                else
                {
                    test_phy_ability.Half_1000 = 0;
                    test_phy_ability.Full_1000 = 0;
                }
#if defined(CONFIG_SDK_RTL8390)
                if (HWP_8390_50_FAMILY(unit))
                {
                    if (HWP_10GE_PORT(unit, port))
                    {
                        test_phy_ability.Half_10G = (ut_rand() & BITMASK_1B);
                        test_phy_ability.Full_10G = (ut_rand() & BITMASK_1B);
                    }
                    else
                    {
                        test_phy_ability.Half_10G = 0;
                        test_phy_ability.Full_10G = 0;
                    }
                }
#endif
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_port_phyAutoNegoAbility_set(unit, port, &test_phy_ability);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_set (unit %u, port %u)"
                                    , ret, expect_result, unit, port);

                    osal_memset(&result_phy_ability, 0, sizeof(rtk_port_phy_ability_t));
                    ret = rtk_port_phyAutoNegoAbility_get(unit, port, &result_phy_ability);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_get (unit %u, port %u)"
                                     , ret, expect_result, unit, port);

                    ret = osal_memcmp(&test_phy_ability, &result_phy_ability, sizeof(rtk_port_phy_ability_t));
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyAutoNegoAbility_set/get value unequal (unit %u, port %u)"
                                     , ret, RT_ERR_OK, unit, port);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyAutoNegoAbility_set (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_port_phyAutoNegoAbility_test*/

int32 dal_port_phyMasterSlave_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_masterSlave_t  msModeCfg_temp;
    rtk_port_masterSlave_t  msModeActual_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_masterSlave_t  msModeCfg[UNITTEST_MAX_TEST_VALUE];
        int32  msModeCfg_result[UNITTEST_MAX_TEST_VALUE];
        int32  msModeCfg_index;
        int32  msModeCfg_last;

        rtk_port_masterSlave_t  result_msModeCfg, result_msModeActual;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_MASTERSLAVE_ID_MAX, TEST_PORT_MASTERSLAVE_ID_MIN, msModeCfg, msModeCfg_result, msModeCfg_last);

       for (port_index = 0; port_index <= port_last; port_index++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyMasterSlave_set(unit, port[port_index], TEST_PORT_MASTERSLAVE_ID_MIN))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyMasterSlave_get(unit, port[port_index], &msModeCfg_temp, &msModeActual_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            inter_result2 = port_result[port_index];
            for (msModeCfg_index = 0; msModeCfg_index <= msModeCfg_last; msModeCfg_index++)
            {
                inter_result3 = msModeCfg_result[msModeCfg_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (HWP_FE_PORT(unit, port[port_index]))
                    expect_result = RT_ERR_FAILED;
                ret = rtk_port_phyMasterSlave_set(unit, port[port_index], msModeCfg[msModeCfg_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_set (unit %u, port %u, msModeCfg %u)"
                                    , ret, expect_result, unit, port[port_index], msModeCfg[msModeCfg_index]);

                    ret = rtk_port_phyMasterSlave_get(unit, port[port_index], &result_msModeCfg, &result_msModeActual);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_get (unit %u, port %u, msModeCfg %u)"
                                     , ret, expect_result, unit, port[port_index], msModeCfg[msModeCfg_index]);

                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_set/get value unequal (unit %u, port %u, msModeCfg %u)"
                                     , result_msModeCfg, msModeCfg[msModeCfg_index], unit, port[port_index], msModeCfg[msModeCfg_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyMasterSlave_set (unit %u, port %u, msModeCfg %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], msModeCfg[msModeCfg_index]);
                }
            }

            ret = rtk_port_phyMasterSlave_get(unit, port[port_index], &result_msModeCfg, &result_msModeActual);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (HWP_FE_PORT(unit, port[port_index]))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyMasterSlave_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_port_masterSlave_t  msModeCfg;
        rtk_port_masterSlave_t  msModeCfg_max;
        rtk_port_masterSlave_t  result_msModeCfg, result_msModeActual;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PORT_MASTERSLAVE_ID_MAX, msModeCfg_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyMasterSlave_set(unit, port, TEST_PORT_MASTERSLAVE_ID_MIN))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyMasterSlave_get(unit, port, &msModeCfg_temp, &msModeActual_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PORT_MASTERSLAVE_ID_MIN, msModeCfg); msModeCfg <= msModeCfg_max; msModeCfg++)
            {
                ASSIGN_EXPECT_RESULT(msModeCfg, TEST_PORT_MASTERSLAVE_ID_MIN, TEST_PORT_MASTERSLAVE_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (HWP_FE_PORT(unit, port) || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_COPPER);
                ret = rtk_port_phyMasterSlave_set(unit, port, msModeCfg);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_set (unit %u, port %u, msModeCfg %u)"
                                    , ret, expect_result, unit, port, msModeCfg);
                    ret = rtk_port_phyMasterSlave_get(unit, port, &result_msModeCfg, &result_msModeActual);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_get (unit %u, port %u, msModeCfg %u)"
                                     , ret, expect_result, unit, port, result_msModeCfg);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyMasterSlave_set/get value unequal (unit %u, port %u, msModeCfg %u)"
                                     , result_msModeCfg, msModeCfg, unit, port, result_msModeCfg);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyMasterSlave_set (unit %u, port %u, msModeCfg %u)"
                                        , ret, RT_ERR_OK, unit, port, msModeCfg);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_phyMasterSlave_test*/

int32 dal_port_phyForceModeAbility_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_speed_t  speed_temp;
    rtk_port_duplex_t duplex_temp;
    rtk_enable_t       flowctrl_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int32  expect_result = 1;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_speed_t  speed[UNITTEST_MAX_TEST_VALUE];
        int32  speed_result[UNITTEST_MAX_TEST_VALUE];
        int32  speed_index;
        int32  speed_last;

        rtk_port_duplex_t  duplex[UNITTEST_MAX_TEST_VALUE];
        int32  duplex_result[UNITTEST_MAX_TEST_VALUE];
        int32  duplex_index;
        int32  duplex_last;

        rtk_enable_t  flowctrl[UNITTEST_MAX_TEST_VALUE];
        int32  flowctrl_result[UNITTEST_MAX_TEST_VALUE];
        int32  flowctrl_index;
        int32  flowctrl_last;

        rtk_port_speed_t  result_speed;
        rtk_port_duplex_t  result_duplex;
        rtk_enable_t  result_flowctrl;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_SPEED_ID_MAX, TEST_PORT_SPEED_ID_MIN, speed, speed_result, speed_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_DUPLEX_ID_MAX, TEST_PORT_DUPLEX_ID_MIN, duplex, duplex_result, duplex_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, flowctrl, flowctrl_result, flowctrl_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyForceModeAbility_set(unit, port[port_index], TEST_PORT_SPEED_ID_MIN, TEST_PORT_DUPLEX_ID_MIN, TEST_ENABLE_STATUS_MAX))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyForceModeAbility_get(unit, port[port_index], &speed_temp, &duplex_temp, &flowctrl_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            inter_result2 = port_result[port_index];
            for (speed_index = 0; speed_index <= speed_last; speed_index++)
            {
                inter_result3 = speed_result[speed_index];

                for (duplex_index = 0; duplex_index <= duplex_last; duplex_index++)
                {
                    inter_result4 = duplex_result[duplex_index];

                    for (flowctrl_index = 0; flowctrl_index <= flowctrl_last; flowctrl_index++)
                    {
                        inter_result5 = flowctrl_result[flowctrl_index];

                        if(!(HWP_UNIT_VALID_LOCAL(unit)) || (!HWP_PORT_EXIST(unit, port[port_index])))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;
                        ret = rtk_port_phyForceModeAbility_set(unit, port[port_index], speed[speed_index], duplex[duplex_index], flowctrl[flowctrl_index]);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                            , ret, expect_result, unit, port[port_index], speed[speed_index], duplex[duplex_index], flowctrl[flowctrl_index]);

                            ret = rtk_port_phyForceModeAbility_get(unit, port[port_index], &result_speed, &result_duplex, &result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_get (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , ret, RT_ERR_OK, unit, port[port_index], result_speed, result_duplex, result_flowctrl);

                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get speed Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_speed, speed[speed_index], unit, port[port_index], result_speed, result_duplex, result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get duplex Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_duplex, duplex[duplex_index], unit, port[port_index], result_speed, result_duplex, result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get flowctrl Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_flowctrl, flowctrl[flowctrl_index], unit, port[port_index], result_speed, result_duplex, result_flowctrl);

                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyForceModeAbility_set (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                                , ret, RT_ERR_OK, unit, port[port_index], speed[speed_index], duplex[duplex_index], flowctrl[flowctrl_index]);
                        }
                    }
                }
            }

            ret = rtk_port_phyForceModeAbility_get(unit, port[port_index], &result_speed, &result_duplex, &result_flowctrl);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyForceModeAbility_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_port_speed_t  speed;
        rtk_port_speed_t  speed_max;
        rtk_port_duplex_t  duplex;
        rtk_port_duplex_t  duplex_max;
        rtk_enable_t  flowctrl;
        rtk_enable_t  flowctrl_max;

        rtk_port_speed_t  result_speed;
        rtk_port_duplex_t  result_duplex;
        rtk_enable_t  result_flowctrl;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PORT_SPEED_ID_MAX, speed_max);
        G_VALUE(TEST_PORT_DUPLEX_ID_MAX, duplex_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, flowctrl_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyForceModeAbility_set(unit, port, TEST_PORT_SPEED_ID_MIN, TEST_PORT_DUPLEX_ID_MIN, TEST_ENABLE_STATUS_MAX))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyForceModeAbility_get(unit, port, &speed_temp, &duplex_temp, &flowctrl_temp)))
            {
                return RT_ERR_PORT_NOT_SUPPORTED;
            }
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PORT_SPEED_ID_MIN, speed); speed <= speed_max; speed++)
            {
                ASSIGN_EXPECT_RESULT(speed, TEST_PORT_SPEED_ID_MIN, TEST_PORT_SPEED_ID_MAX, inter_result3);

                for (L_VALUE(TEST_PORT_DUPLEX_ID_MIN, duplex); duplex <= duplex_max; duplex++)
                {
                    ASSIGN_EXPECT_RESULT(duplex, TEST_PORT_DUPLEX_ID_MIN, TEST_PORT_DUPLEX_ID_MAX, inter_result4);
                    for (L_VALUE(TEST_ENABLE_STATUS_MIN, flowctrl); flowctrl <= flowctrl_max; flowctrl++)
                    {
                        ASSIGN_EXPECT_RESULT(flowctrl, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result5);
                        if (HWP_FE_PORT(unit, port) && (speed > PORT_SPEED_100M))
                            inter_result3 = 0;
                        if(!(HWP_UNIT_VALID_LOCAL(unit)) || (!HWP_PORT_EXIST(unit, port)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;

                        ret = rtk_port_phyForceModeAbility_set(unit, port, speed, duplex, flowctrl);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                            , ret, expect_result, unit, port, speed, duplex, flowctrl);

                            ret = rtk_port_phyForceModeAbility_get(unit, port, &result_speed, &result_duplex, &result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_get (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , ret, RT_ERR_OK, unit, port, result_speed, result_duplex, result_flowctrl);

                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get speed Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_speed, speed, unit, port, result_speed, result_duplex, result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get duplex Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_duplex, duplex, unit, port, result_speed, result_duplex, result_flowctrl);
                            RT_TEST_IS_EQUAL_INT("rtk_port_phyForceModeAbility_set/get flowctrl Failed (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                             , result_flowctrl, flowctrl, unit, port, result_speed, result_duplex, result_flowctrl);

                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyForceModeAbility_set (unit %u, port %u, speed %u, duplex %u, flowctrl %u)"
                                                , ret, RT_ERR_OK, unit, port, speed, duplex, flowctrl);
                        }

                    }
                }
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_port_phyForceModeAbility_test*/

int32 dal_port_phyComboPortMedia_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_media_t  portMedia_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;



    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_media_t  media[UNITTEST_MAX_TEST_VALUE];
        int32  media_result[UNITTEST_MAX_TEST_VALUE];
        int32  media_index;
        int32  media_last;

        rtk_port_media_t  result_media;
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_MEDIA_ID_MAX, TEST_PORT_MEDIA_ID_MIN, media, media_result, media_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyComboPortMedia_set(unit, port[port_index], TEST_PORT_MEDIA_ID_MIN))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyComboPortMedia_get(unit, port[port_index], &portMedia_temp)))
            {
                continue;
            }
            inter_result2 = port_result[port_index];
            for (media_index = 0; media_index <= media_last; media_index++)
            {
                inter_result3 = media_result[media_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (!HWP_COMBO_PORT(unit, port[port_index]) || media[media_index] >= PORT_MEDIA_END || !HWP_PORT_EXIST(unit, port[port_index]))
                    expect_result = RT_ERR_FAILED;
                ret = rtk_port_phyComboPortMedia_set(unit, port[port_index], media[media_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port[port_index], media[media_index]);

                    ret = rtk_port_phyComboPortMedia_get(unit, port[port_index], &result_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u, media %u)"
                                     , ret, expect_result, unit, port[port_index], result_media);

                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set/get value unequal (unit %u, port %u, media %u)"
                                     , result_media, media[media_index], unit, port[port_index], media[media_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], media[media_index]);
                }
            }

            ret = rtk_port_phyComboPortMedia_get(unit, port[port_index], &result_media);
            if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port[port_index]))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_port_media_t  media;
        rtk_port_media_t  media_max;
        rtk_port_media_t  result_media;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PORT_MEDIA_ID_MAX, media_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);

            if ((RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyComboPortMedia_set(unit, port, TEST_PORT_MEDIA_ID_MIN))
                || (RT_ERR_PORT_NOT_SUPPORTED == rtk_port_phyComboPortMedia_get(unit, port, &portMedia_temp)))
            {
                continue;
            }

            for (L_VALUE(TEST_PORT_MEDIA_ID_MIN, media); media <= media_max; media++)
            {
                ASSIGN_EXPECT_RESULT(media, TEST_PORT_MEDIA_ID_MIN, TEST_PORT_MEDIA_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (!HWP_COMBO_PORT(unit, port) || media >= PORT_MEDIA_END || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                ret = rtk_port_phyComboPortMedia_set(unit, port, media);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port, media);
                    ret = rtk_port_phyComboPortMedia_get(unit, port, &result_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u, media %u)"
                                     , ret, expect_result, unit, port, result_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set/get value unequal (unit %u, port %u, media %u)"
                                     , result_media, media, unit, port, result_media);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, RT_ERR_OK, unit, port, media);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_phyComboPortMedia_test*/

int32 dal_port_txEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_txEnable_set(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_txEnable_get(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_txEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_txEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_port_txEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_port_txEnable_get (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], result_enable);

                    RT_TEST_IS_EQUAL_INT("rtk_port_txEnable_set/get isn't equal (unit %u, port %u, enable %u)"
                                    , enable[enable_index], result_enable, unit, port[port_index], enable[enable_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_txEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_port_txEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_port_txEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_port_txEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_port_txEnable_test*/
#if 0//support when linkmon sync from sdk-2nchips
int32 dal_port_linkMon_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result1 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_linkMon_enable(unit, TEST_LINKMON_SCAN_INTERVAL_MIN)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_port_linkMon_disable(unit)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  interval[UNITTEST_MAX_TEST_VALUE];
        int32  interval_result[UNITTEST_MAX_TEST_VALUE];
        int32  interval_index;
        int32  interval_last;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_LINKMON_SCAN_INTERVAL_MIN, TEST_LINKMON_SCAN_INTERVAL_MIN, interval, interval_result, interval_last);
        interval_result[2] = 1; /*This API only concern MIN*/

        for (interval_index = 0; interval_index <= interval_last; interval_index++)
        {
            ret = rtk_port_linkMon_disable(unit);
            RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_disable (void)"
                   , ret, RT_ERR_OK);

            inter_result1 = interval_result[interval_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result1)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_port_linkMon_enable(unit, interval[interval_index]);
            if (RT_ERR_OK == expect_result)
            {
                 RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_enable (interval %u)"
                         , ret, expect_result, interval[interval_index]);

                 ret = rtk_port_linkMon_disable(unit);
                 RT_TEST_IS_EQUAL_INT("rtk_port_linkMon_disable (void)"
                         , ret, expect_result);

            }
            else
            {
                 RT_TEST_IS_NOT_EQUAL_INT("rtk_port_linkMon_enable (interval %u)"
                         , ret, RT_ERR_OK, interval[interval_index]);
            }
        }
    }
    return RT_ERR_OK;
} /*end of dal_port_linkMon_test*/

#endif

