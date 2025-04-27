/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 75751 $
 * $Date: 2017-02-10 11:10:15 +0800 (Fri, 10 Feb 2017) $
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
#include <rtk/rate.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define TEST_RATE_LIMIT_MIN                   0
#define TEST_RATE_LIMIT_MAX(unit, port)       HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)

#define TEST_STORM_RATE_LIMIT_MIN             0x0
#define TEST_STORM_RATE_LIMIT_MAX             HAL_BURST_OF_STORM_CONTROL_MAX
#define TEST_QUEUE_ID_MIN(unit)               (HAL_MIN_NUM_OF_QUEUE(unit)-1)
#define TEST_QUEUE_ID_MAX(unit)               (HAL_MAX_NUM_OF_QUEUE(unit)-1)

#define TEST_STORM_TYPE_MAX                   STORM_GROUP_UNICAST
#define TEST_STORM_TYPE_MIN(unit)            STORM_GROUP_MULTICAST
//#define TEST_STORM_TYPE_MIN(unit)             (HWP_8390_50_FAMILY(unit) ? STORM_GROUP_MULTICAST : STORM_GROUP_UNKNOWN_UNICAST)
#define TEST_STORM_PROTOTYPE_MIN              STORM_PROTO_GROUP_BPDU
#define TEST_STORM_PROTOTYPE_MAX(unit)        (HWP_9300_FAMILY_ID(unit) ? STORM_PROTO_GROUP_DHCP : STORM_PROTO_GROUP_ARP)
#define TEST_STORM_PROTORATE_LIMIT_MIN        0x0
#define TEST_STORM_PROTORATE_LIMIT_MAX(unit)  HAL_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)

#define TEST_BURST_SIZE_MIN(unit)             HAL_BURST_RATE_OF_STORM_CONTROL_MIN(unit)
#define TEST_STORM_BURST_10G_MAX(unit)        HAL_BURST_RATE_OF_10G_STORM_CONTROL_MAX(unit)
#define TEST_STORM_BURST_1G_MAX(unit)         HAL_BURST_RATE_OF_STORM_CONTROL_MAX(unit)


#define TEST_RATE_MODE_MIN                    BASED_ON_PKT
#define TEST_RATE_MODE_MAX                    BASED_ON_BYTE

#define TEST_FPENTRY_ID_MIN                   0
#define TEST_FPENTRY_ID_MAX                   2

#define TEST_RATE_THRESHOLD_MIN(unit)         HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN(unit)
#define TEST_RATE_THRESHOLD_MAX(unit)         HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX(unit)

#define TEST_RATE_IGR_BYPASS_TYPE_MIN         IGR_BYPASS_RMA
#define TEST_RATE_IGR_BYPASS_TYPE_MAX(unit)  ((HWP_9300_FAMILY_ID(unit)||HWP_9310_FAMILY_ID(unit)) ? IGR_BYPASS_RIP : IGR_BYPASS_ARPREQ)

#define TEST_RATE_EGR_PORT_QUEUE_ID_MIN       0
#define TEST_RATE_EGR_PORT_QUEUE_ID_MAX       RTK_MAX_NUM_OF_QUEUE-1

#define TEST_RATE_STORM_BYPASS_TYPE_MIN       STORM_BYPASS_RMA
#define TEST_RATE_STORM_BYPASS_TYPE_MAX(unit) ((HWP_9300_FAMILY_ID(unit)||HWP_9310_FAMILY_ID(unit)) ? STORM_BYPASS_RIP : STORM_BYPASS_ARP)

#define TEST_RATE_STORM_SEL_MIN               STORM_SEL_UNKNOWN
#define TEST_RATE_STORM_SEL_MAX               STORM_SEL_UNKNOWN_AND_KNOWN

#define RATE_RANDOM_RUN_TIMES                 10

#define EXPECTED_PORT_TYPE                    1
#define UNEXPECTED_PORT_TYPE                  0

extern int32 dal_rate_portEgrBwCtrlEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrBwCtrlEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrBwCtrlEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portEgrBwCtrlEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_rate_portEgrBwCtrlEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_rate_portEgrBwCtrlEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;


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
                ret = rtk_rate_portEgrBwCtrlEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_rate_portEgrBwCtrlEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_rate_portEgrBwCtrlEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}


extern int32 dal_rate_portEgrBwCtrlRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret = 0;
    rtk_vlan_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrBwCtrlRate_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrBwCtrlRate_get(unit, 0, &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last = 0;
        uint32  result_rate;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            if((1 == inter_result2) && (HWP_UNIT_VALID_LOCAL(unit)))
            {
                UNITTEST_TEST_VALUE_ASSIGN(HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port[port_index]), TEST_RATE_LIMIT_MIN, rate, rate_result, rate_last);
                ret = rtk_rate_portEgrBwCtrlEnable_set(unit, port[port_index], ENABLED);
                RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                , ret, RT_ERR_OK, unit, port[port_index], ENABLED);
            }
            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portEgrBwCtrlRate_set(unit, port[port_index], rate[rate_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port[port_index], rate[rate_index]);

                    ret = rtk_rate_portEgrBwCtrlRate_get(unit, port[port_index], &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port[port_index], result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u result %u)"
                                     , result_rate, rate[rate_index], unit, port[port_index], rate[rate_index], result_rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }
                ret = rtk_rate_portEgrBwCtrlRate_get(unit, port[port_index], (uint32*)&result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  rate;
        uint32  rate_max = 0;
        uint32  result_rate;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);
            if((1 == inter_result2) && (HWP_UNIT_VALID_LOCAL(unit)))
                G_VALUE(HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port), rate_max);

            for (L_VALUE(TEST_RATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
            {
                if((1 == inter_result2) && (HWP_UNIT_VALID_LOCAL(unit)))
                {
                    ASSIGN_EXPECT_RESULT(rate, TEST_RATE_LIMIT_MIN, HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port), inter_result3);
                    ret = rtk_rate_portEgrBwCtrlEnable_set(unit, port, ENABLED);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, port, ENABLED);
                }
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portEgrBwCtrlRate_set(unit, port, rate);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port, rate);

                    ret = rtk_rate_portEgrBwCtrlRate_get(unit, port, &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port, result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u)"
                                     , result_rate, rate, unit, port, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port, rate);
                }
                ret = rtk_rate_portEgrBwCtrlRate_get(unit, port, &result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrBwCtrlRate_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }

        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portEgrQueueBwCtrlEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrQueueBwCtrlEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrQueueBwCtrlEnable_get(unit, 0, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qid_t  queue[UNITTEST_MAX_TEST_VALUE];
        int32  queue_result[UNITTEST_MAX_TEST_VALUE];
        int32  queue_index;
        int32  queue_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), queue, queue_result, queue_last);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (queue_index = 0; queue_index <= queue_last; queue_index++)
            {
                inter_result3 = queue_result[queue_index];
                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portEgrQueueBwCtrlEnable_set(unit, port[port_index], queue[queue_index], enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set (unit %u, port %u, queue %u, enable %u)"
                                        , ret, expect_result, unit, port[port_index], queue[queue_index], enable);

                        ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port[port_index], queue[queue_index], &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u, enable %u)"
                                         , ret, expect_result, unit, port[port_index], queue[queue_index], result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set/get value unequal (unit %u, port %u, queue %u, enable %u)"
                                         , result_enable, enable, unit, port[port_index], queue[queue_index], enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set (unit %u, port %u, queue %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], queue[queue_index], enable);
                    }
                }
                ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port[port_index], queue[queue_index], (rtk_enable_t *)&result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u)"
                                    , ret, expect_result, unit, port[port_index], queue[queue_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], queue[queue_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_qid_t  queue;
        rtk_qid_t  queue_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), queue_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_QUEUE_ID_MIN(unit), queue); queue <= queue_max; queue++)
            {
                ASSIGN_EXPECT_RESULT(queue, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    ret = rtk_rate_portEgrQueueBwCtrlEnable_set(unit, port, queue, enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set (unit %u, port %u, queue %u, enable %u)"
                                        , ret, expect_result, unit, port, queue, enable);

                        ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port, queue, &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u, enable %u)"
                                         , ret, expect_result, unit, port, queue, result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set/get value unequal (unit %u, port %u, queue %u, enable %u)"
                                         , result_enable, enable, unit, port, queue, enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_set (unit %u, port %u, queue %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port, queue, enable);
                    }
                    ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port, queue, &result_enable);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u)"
                                        , ret, expect_result, unit, port, queue);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlEnable_get (unit %u, port %u, queue %u)"
                                            , ret, RT_ERR_OK, unit, port, queue);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portEgrQueueBwCtrlRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrQueueBwCtrlRate_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portEgrQueueBwCtrlRate_get(unit, 0, 0, &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qid_t  queue[UNITTEST_MAX_TEST_VALUE];
        int32  queue_result[UNITTEST_MAX_TEST_VALUE];
        int32  queue_index;
        int32  queue_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index = 0;
        int32  rate_last = 0;

        uint32  result_rate = 0;;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), queue, queue_result, queue_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (queue_index = 0; queue_index <= queue_last; queue_index++)
            {
                inter_result3 = queue_result[queue_index];
                if((1 == inter_result2) && (HWP_UNIT_VALID_LOCAL(unit)))
                {
                      UNITTEST_TEST_VALUE_ASSIGN(HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port[port_index]), TEST_RATE_LIMIT_MIN, rate, rate_result, rate_last);

                    for (rate_index = 0; rate_index <= rate_last; rate_index++)
                    {
                        inter_result4 = rate_result[rate_index];
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                        ret = rtk_rate_portEgrQueueBwCtrlRate_set(unit, port[port_index], queue[queue_index], rate[rate_index]);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set (unit %u, port %u, queue %u, rate %u)"
                                            , ret, expect_result, unit, port[port_index], queue[queue_index], rate[rate_index]);

                            ret = rtk_rate_portEgrQueueBwCtrlRate_get(unit, port[port_index], queue[queue_index], &result_rate);
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u, rate %u)"
                                             , ret, expect_result, unit, port[port_index], queue[queue_index], result_rate);
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set/get value unequal (unit %u, port %u, queue %u, rate %u)"
                                             , result_rate, rate[rate_index], unit, port[port_index], queue[queue_index], rate[rate_index]);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set (unit %u, port %u, queue %u, rate %u)"
                                                , ret, RT_ERR_OK, unit, port[port_index], queue[queue_index], rate[rate_index]);
                        }
                    }
                }
                ret = rtk_rate_portEgrQueueBwCtrlRate_get(unit, port[port_index], queue[queue_index], (uint32*)&result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u)"
                                    , ret, expect_result, unit, port[port_index], queue[queue_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], queue[queue_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_qid_t  queue;
        rtk_qid_t  queue_max;
        uint32  rate;
        uint32  rate_max;

        uint32  result_rate;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), queue_max);


        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            G_VALUE(TEST_RATE_LIMIT_MAX(unit, port), rate_max);
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_QUEUE_ID_MIN(unit), queue); queue <= queue_max; queue++)
            {
                ASSIGN_EXPECT_RESULT(queue, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result3);

                for (L_VALUE(TEST_RATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
                {

                    if((1 == inter_result2) && (HWP_UNIT_VALID_LOCAL(unit)))
                    {
                        ASSIGN_EXPECT_RESULT(rate, TEST_RATE_LIMIT_MIN, HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port), inter_result4);
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                        ret = rtk_rate_portEgrQueueBwCtrlRate_set(unit, port, queue, rate);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set (unit %u, port %u, queue %u, rate %u)"
                                            , ret, expect_result, unit, port, queue, rate);

                            ret = rtk_rate_portEgrQueueBwCtrlRate_get(unit, port, queue, &result_rate);
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u, rate %u)"
                                             , ret, expect_result, unit, port, queue, result_rate);
                            RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set/get value unequal (unit %u, port %u, queue %u, rate %u)"
                                             , result_rate, rate, unit, port, queue, rate);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_set (unit %u, port %u, queue %u, rate %u)"
                                                , ret, RT_ERR_OK, unit, port, queue, rate);
                            osal_printf("inter_result2 %d inter_result3 %d inter_result4 %d\n", inter_result2, inter_result3, inter_result4);
                        }
                    }
                    ret = rtk_rate_portEgrQueueBwCtrlRate_get(unit, port, queue, &result_rate);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u)"
                                        , ret, expect_result, unit, port, queue);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portEgrQueueBwCtrlRate_get (unit %u, port %u, queue %u)"
                                            , ret, RT_ERR_OK, unit, port, queue);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portIgrBwCtrlEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last = 0;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port[port_index]))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBwCtrlEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_rate_portIgrBwCtrlEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_rate_portIgrBwCtrlEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port[port_index]))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max = 0;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBwCtrlEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_rate_portIgrBwCtrlEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_rate_portIgrBwCtrlEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)) || !HWP_PORT_EXIST(unit, port))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portIgrBwCtrlRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    uint32 isGEFE_port = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlRate_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlRate_get(unit, 0, &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;
        uint32  result_rate;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_RATE_OF_BANDWIDTH_MAX(unit), TEST_RATE_LIMIT_MIN, rate, rate_result, rate_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            isGEFE_port = UNEXPECTED_PORT_TYPE;
            inter_result2 = port_result[port_index];
            if((HWP_UNIT_VALID_LOCAL(unit)) && (1 == inter_result2))
            {
                if(!HWP_10GE_PORT(unit, port[port_index]))
                {
                    isGEFE_port = EXPECTED_PORT_TYPE;
                }else{
                    isGEFE_port = UNEXPECTED_PORT_TYPE;
                    inter_result2 = 0;
                }
            }
            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if(EXPECTED_PORT_TYPE == isGEFE_port)
                ret = rtk_rate_portIgrBwCtrlRate_set(unit, port[port_index], rate[rate_index]);
                else
                    ret = RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port[port_index], rate[rate_index]);

                    ret = rtk_rate_portIgrBwCtrlRate_get(unit, port[port_index], &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port[port_index], result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u result %u)"
                                     , result_rate, rate[rate_index], unit, port[port_index], rate[rate_index], result_rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }
                if(EXPECTED_PORT_TYPE == isGEFE_port)
                ret = rtk_rate_portIgrBwCtrlRate_get(unit, port[port_index], (uint32*)&result_rate);
                else
                    ret = RT_ERR_FAILED;
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  rate;
        uint32  rate_max;
        uint32  result_rate;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(HAL_RATE_OF_BANDWIDTH_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            isGEFE_port = UNEXPECTED_PORT_TYPE;
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            if((HWP_UNIT_VALID_LOCAL(unit)) && (1 == inter_result2))
            {
                if(!HWP_10GE_PORT(unit, port))
                {
                    isGEFE_port = EXPECTED_PORT_TYPE;
                }else{
                    isGEFE_port = UNEXPECTED_PORT_TYPE;
                    inter_result2 = 0;
                }
            }
            for (L_VALUE(TEST_RATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
            {
                ASSIGN_EXPECT_RESULT(rate, TEST_RATE_LIMIT_MIN, HAL_RATE_OF_BANDWIDTH_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if(EXPECTED_PORT_TYPE == isGEFE_port)
                    ret = rtk_rate_portIgrBwCtrlRate_set(unit, port, rate);
                else
                    ret = RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port, rate);

                    ret = rtk_rate_portIgrBwCtrlRate_get(unit, port, &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port, result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u)"
                                     , result_rate, rate, unit, port, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port, rate);
                }
                if(EXPECTED_PORT_TYPE == isGEFE_port)
                ret = rtk_rate_portIgrBwCtrlRate_get(unit, port, &result_rate);
                else
                    ret = RT_ERR_FAILED;
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }

        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portIgrBwFlowctrlEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwFlowctrlEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwFlowctrlEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBwFlowctrlEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_rate_portIgrBwFlowctrlEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_rate_portIgrBwFlowctrlEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBwFlowctrlEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_rate_portIgrBwFlowctrlEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_rate_portIgrBwFlowctrlEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwFlowctrlEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_igrBandwidthCtrlIncludeIfg_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthCtrlIncludeIfg_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_igrBandwidthCtrlIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
        }
        ret = rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_igrBandwidthCtrlIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
            ret = rtk_rate_igrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlIncludeIfg_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_egrBandwidthCtrlIncludeIfg_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_egrBandwidthCtrlIncludeIfg_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
        }
        ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_egrBandwidthCtrlIncludeIfg_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portStormCtrlBurstSize_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlBurstSize_set(unit, 0, TEST_STORM_TYPE_MIN(unit), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlBurstSize_get(unit, 0, TEST_STORM_TYPE_MIN(unit), &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_rate_storm_group_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last = 0;
        uint32  result_rate = 0;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_TYPE_MAX, TEST_STORM_TYPE_MIN(unit), storm, storm_result, storm_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            if((HWP_UNIT_VALID_LOCAL(unit)) && inter_result2 && (HWP_10GE_PORT(unit, port[port_index])))
            {
                UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_BURST_10G_MAX(unit), TEST_BURST_SIZE_MIN(unit), rate, rate_result, rate_last);
            }
            else
            {
                UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_BURST_1G_MAX(unit), TEST_BURST_SIZE_MIN(unit), rate, rate_result, rate_last);
            }
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];
                for (rate_index = 0; rate_index <= rate_last; rate_index++)
                {
                    inter_result4 = rate_result[rate_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlBurstSize_set(unit, port[port_index], storm[storm_index], rate[rate_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port[port_index], storm[storm_index], rate[rate_index]);

                        ret = rtk_rate_portStormCtrlBurstSize_get(unit, port[port_index], storm[storm_index], &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port[port_index], storm[storm_index], result_rate);

                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate[rate_index], unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                    else
                    {
                        if(TEST_STORM_BURST_10G_MAX(unit) < rate[rate_index])
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set (unit %u, port %u, storm %u, rate %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                }
                ret = rtk_rate_portStormCtrlBurstSize_get(unit, port[port_index], storm[storm_index], (uint32*)&result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_group_t  storm;
        rtk_rate_storm_group_t  storm_max;
        uint32  rate;
        uint32  rate_max;
        uint32  result_rate;


        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_TYPE_MAX, storm_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {


            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_STORM_TYPE_MIN(unit), storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_TYPE_MIN(unit), TEST_STORM_TYPE_MAX, inter_result3);

                if((HWP_UNIT_VALID_LOCAL(unit)) && inter_result2 && HWP_10GE_PORT(unit, port)){
                    G_VALUE(TEST_STORM_BURST_10G_MAX(unit), rate_max);
                 }else{
                    G_VALUE(TEST_STORM_BURST_1G_MAX(unit), rate_max);
                }

                for (L_VALUE(TEST_BURST_SIZE_MIN(unit), rate); rate <= rate_max; rate++)
                {
                    if((HWP_UNIT_VALID_LOCAL(unit)) && inter_result2 && HWP_10GE_PORT(unit, port))
                    {
                        ASSIGN_EXPECT_RESULT(rate, TEST_BURST_SIZE_MIN(unit), TEST_STORM_BURST_10G_MAX(unit), inter_result4);

                    }
                    else
                    {
                        ASSIGN_EXPECT_RESULT(rate, TEST_BURST_SIZE_MIN(unit), TEST_STORM_BURST_1G_MAX(unit), inter_result4);
                    }

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlBurstSize_set(unit, port, storm, rate);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port, storm, rate);

                        ret = rtk_rate_portStormCtrlBurstSize_get(unit, port, storm, &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port, storm, result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate, unit, port, storm, rate);
                    }
                    else
                    {
                        if(TEST_STORM_BURST_10G_MAX(unit) < rate)
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_set (unit %u, port %u, storm %u, rate %u)"
                                                , ret, RT_ERR_OK, unit, port, storm, rate);
                    }
                    ret = rtk_rate_portStormCtrlBurstSize_get(unit, port, storm, &result_rate);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port, storm);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlBurstSize_get (unit %u, port %u, storm %u)"
                                            , ret, RT_ERR_OK, unit, port, storm);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portStormCtrlExceed_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  exceed_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlExceed_get(unit, 0, TEST_STORM_TYPE_MIN(unit), &exceed_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qid_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;
        uint32  exceed;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_TYPE_MAX, TEST_STORM_TYPE_MIN(unit), storm, storm_result, storm_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portStormCtrlExceed_get(unit, port[port_index], storm[storm_index], &exceed);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index], exceed);
                    ret = rtk_rate_portStormCtrlExceed_get(unit, port[port_index], storm[storm_index], NULL);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_NULL_POINTER, unit, port[port_index], storm[storm_index]);

                    ret = rtk_rate_portStormCtrlExceed_reset(unit, port[port_index], storm[storm_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlExceed_reset (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], exceed);
                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port[port_index], storm[storm_index]);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlExceed_reset (unit %u, port %u, storm %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_group_t  storm;
        rtk_rate_storm_group_t  storm_max;
        uint32  exceed;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_TYPE_MAX, storm_max);

    for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);
            for (L_VALUE(TEST_STORM_TYPE_MIN(unit), storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_TYPE_MIN(unit), TEST_STORM_TYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portStormCtrlExceed_get(unit, port, storm, &exceed);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                    , ret, expect_result, unit, port, storm, exceed);
                    ret = rtk_rate_portStormCtrlExceed_get(unit, port, storm, NULL);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_NULL_POINTER, unit, port, storm);

                    ret = rtk_rate_portStormCtrlExceed_reset(unit, port, storm);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlExceed_reset (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port, storm);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                        , ret, RT_ERR_OK, unit, port, storm, exceed);

                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port, storm);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlExceed_reset (unit %u, port %u, storm %u)"
                                    , ret, RT_ERR_OK, unit, port, storm);
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_stormControlIncludeIfg_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_stormControlIncludeIfg_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_stormControlIncludeIfg_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_stormControlIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_rate_stormControlIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
            ret = rtk_rate_stormControlIncludeIfg_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlIncludeIfg_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlIncludeIfg_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portStormCtrlRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlRate_set(unit, 0, TEST_STORM_TYPE_MIN(unit), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlRate_get(unit, 0, TEST_STORM_TYPE_MIN(unit), &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qid_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;

        uint32  result_rate;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_TYPE_MAX, TEST_STORM_TYPE_MIN(unit), storm, storm_result, storm_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_RATE_OF_STORM_CONTROL_MAX(unit), TEST_STORM_RATE_LIMIT_MIN, rate, rate_result, rate_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];
                for (rate_index = 0; rate_index <= rate_last; rate_index++)
                {
                    inter_result4 = rate_result[rate_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlRate_set(unit, port[port_index], storm[storm_index], rate[rate_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port[port_index], storm[storm_index], rate[rate_index]);

                        ret = rtk_rate_portStormCtrlRate_get(unit, port[port_index], storm[storm_index], &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port[port_index], storm[storm_index], result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate[rate_index], unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlRate_set (unit %u, port %u, storm %u, rate %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                }
                ret = rtk_rate_portStormCtrlRate_get(unit, port[port_index], storm[storm_index], (uint32*)&result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_group_t  storm;
        rtk_rate_storm_group_t  storm_max;
        uint32  rate;
        uint32  rate_max;
        uint32  result_rate;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_TYPE_MAX, storm_max);
        G_VALUE(HAL_RATE_OF_STORM_CONTROL_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_STORM_TYPE_MIN(unit), storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_TYPE_MIN(unit), TEST_STORM_TYPE_MAX, inter_result3);

                for (L_VALUE(TEST_STORM_RATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
                {
                    ASSIGN_EXPECT_RESULT(rate, TEST_STORM_RATE_LIMIT_MIN, HAL_RATE_OF_STORM_CONTROL_MAX(unit), inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlRate_set(unit, port, storm, rate);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port, storm, rate);

                        ret = rtk_rate_portStormCtrlRate_get(unit, port, storm, &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port, storm, result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate, unit, port, storm, rate);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlRate_set (unit %u, port %u, storm %u, rate %u)"
                                            , ret, RT_ERR_OK, unit, port, storm, rate);
                    }
                    ret = rtk_rate_portStormCtrlRate_get(unit, port, storm, &result_rate);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port, storm);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlRate_get (unit %u, port %u, storm %u)"
                                            , ret, RT_ERR_OK, unit, port, storm);
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

extern int32 dal_rate_igrBandwidthCtrlBypass_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthCtrlBypass_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthCtrlBypass_get(unit, 0,&enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_rate_igr_bypassType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_RATE_IGR_BYPASS_TYPE_MAX(unit), TEST_RATE_IGR_BYPASS_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_igrBandwidthCtrlBypass_set(unit, type[type_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set (unit %u, type %u, enable %u)"
                                   , ret, expect_result, unit, type[type_index],  enable[enable_index]);

                    ret = rtk_rate_igrBandwidthCtrlBypass_get(unit, type[type_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u, result %u)"
                                    , ret, expect_result, unit, type[type_index], enable[enable_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set/get value unequal (unit %u, type %u, enable %u, result_enable %u)"
                                         , result_enable, enable[enable_index], unit, type[type_index], enable[enable_index], result_enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set (unit %u, type %u, enable %u)"
                                       , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index]);
                }
                ret = rtk_rate_igrBandwidthCtrlBypass_get(unit, type[type_index], &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u, result %u)"
                                   , ret, expect_result, unit, type[type_index], enable[enable_index], result_enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u, result %u)"
                                           , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index], result_enable);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_rate_igr_bypassType_t  type;
        rtk_rate_igr_bypassType_t  type_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_RATE_IGR_BYPASS_TYPE_MAX(unit), type_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_RATE_IGR_BYPASS_TYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_RATE_IGR_BYPASS_TYPE_MIN, TEST_RATE_IGR_BYPASS_TYPE_MAX(unit), inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_igrBandwidthCtrlBypass_set(unit, type, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set (unit %u, type %u, enable %u)"
                                   , ret, expect_result, unit, type, enable);

                    ret = rtk_rate_igrBandwidthCtrlBypass_get(unit, type ,&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set/get value unequal (unit %u, type %u, enable %u)"
                                    , result_enable, enable, unit, type, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_set (unit %u, type %u, enable %u)"
                                       , ret, RT_ERR_OK, unit, type, enable);
                }
                ret = rtk_rate_igrBandwidthCtrlBypass_get(unit, type,&result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u)"
                                   , ret, expect_result, unit, type, result_enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthCtrlBypass_get (unit %u, type %u, enable %u)"
                                       , ret, RT_ERR_OK, unit, type, result_enable);
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_igrBandwidthLowThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  thresh_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthLowThresh_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_igrBandwidthLowThresh_get(unit, &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  thresh[UNITTEST_MAX_TEST_VALUE];
        int32  thresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  thresh_index;
        int32  thresh_last;
        uint32  result_thresh;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_RATE_THRESHOLD_MAX(unit), TEST_RATE_THRESHOLD_MIN(unit), thresh, thresh_result, thresh_last);

        for (thresh_index = 0; thresh_index <= thresh_last; thresh_index++)
        {
            inter_result2 = thresh_result[thresh_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_igrBandwidthLowThresh_set(unit, thresh[thresh_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set (unit %u, thresh %u)"
                                , ret, expect_result, unit, thresh[thresh_index]);

                ret = rtk_rate_igrBandwidthLowThresh_get(unit, &result_thresh);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_get (unit %u, rate %u)"
                                 , ret, expect_result, unit, result_thresh);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set/get value unequal (unit %u, thresh %u)"
                                 , result_thresh, thresh[thresh_index], unit, thresh[thresh_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set (unit %u, rate %u)"
                                    , ret, RT_ERR_OK, unit, thresh[thresh_index]);
            }
            ret = rtk_rate_igrBandwidthLowThresh_get(unit,(uint32*)&result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_get (unit %u, thresh %u)"
                                , ret, expect_result, unit, thresh[thresh_index]);
            }
            else
            {
              /*This is get value API only, so failed case is meaningless over here.*/
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  thresh;
        uint32  thresh_max;
        uint32  result_thresh;


        G_VALUE(TEST_RATE_THRESHOLD_MAX(unit), thresh_max);

        for (L_VALUE(TEST_RATE_THRESHOLD_MIN(unit), thresh); thresh <= thresh_max; thresh++)
        {
            ASSIGN_EXPECT_RESULT(thresh, TEST_RATE_THRESHOLD_MIN(unit), TEST_RATE_THRESHOLD_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_rate_igrBandwidthLowThresh_set(unit, thresh);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set (unit %u, thresh %u)"
                                , ret, expect_result, unit, thresh);

                ret = rtk_rate_igrBandwidthLowThresh_get(unit, &result_thresh);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_get (unit %u, thresh %u)"
                                 , ret, expect_result, unit, result_thresh);
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set/get value unequal (unit %u, thresh %u)"
                                 , result_thresh, thresh, unit, thresh);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_set (unit %u, thresh %u)"
                                    , ret, RT_ERR_OK, unit, thresh);
            }
            ret = rtk_rate_igrBandwidthLowThresh_get(unit, &result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_get (unit %u, thresh %u)"
                                , ret, expect_result, unit, thresh);
            }
            else
            {
               /*This is get value API only, so failed case is meaningless over here.*/
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portIgrBandwidthHighThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  thresh_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBandwidthHighThresh_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBandwidthHighThresh_get(unit, 0, &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  thresh[UNITTEST_MAX_TEST_VALUE];
        int32  thresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  thresh_index;
        int32  thresh_last;

        uint32  result_thresh;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RATE_THRESHOLD_MAX(unit), TEST_RATE_THRESHOLD_MIN(unit), thresh, thresh_result, thresh_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (thresh_index = 0; thresh_index <= thresh_last; thresh_index++)
            {
                inter_result3 = thresh_result[thresh_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBandwidthHighThresh_set(unit, port[port_index], thresh[thresh_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set (unit %u, port %u, thresh %u)"
                                    , ret, expect_result, unit, port[port_index], thresh[thresh_index]);

                    ret = rtk_rate_portIgrBandwidthHighThresh_get(unit, port[port_index], &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_get (unit %u, port %u, thresh %u)"
                                     , ret, expect_result, unit, port[port_index], result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set/get value unequal (unit %u, port %u, thresh %u)"
                                     , result_thresh, thresh[thresh_index], unit, port[port_index], thresh[thresh_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set (unit %u, port %u, thresh %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], thresh[thresh_index]);
                }
                ret = rtk_rate_portIgrBandwidthHighThresh_get(unit, port[port_index], (uint32*)&result_thresh);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_get (unit %u, port %u, thresh %u)"
                                    , ret, expect_result, unit, port[port_index], thresh[thresh_index]);
                }
                else
                {
                  /*This is get value API only, so failed case is meaningless over here.*/
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  thresh;
        uint32  thresh_max;
        uint32  result_thresh;

        G_VALUE(TEST_RATE_THRESHOLD_MAX(unit), thresh_max);
        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);
            for (L_VALUE(TEST_RATE_THRESHOLD_MIN(unit), thresh); thresh <= thresh_max; thresh++)
            {
                ASSIGN_EXPECT_RESULT(thresh, TEST_RATE_THRESHOLD_MIN(unit), TEST_RATE_THRESHOLD_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portIgrBandwidthHighThresh_set(unit, port, thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set (unit %u, port %u, thresh %u)"
                                    , ret, expect_result, unit, port, thresh);

                    ret = rtk_rate_portIgrBandwidthHighThresh_get(unit, port, &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_get (unit %u, port %u, thresh %u)"
                                     , ret, expect_result, unit, port, result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set/get value unequal (unit %u, port %u, thresh %u)"
                                     , result_thresh, thresh, unit, port, thresh);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBandwidthHighThresh_set (unit %u, port %u, thresh %u)"
                                        , ret, RT_ERR_OK, unit, port, thresh);
                }
                ret = rtk_rate_portIgrBandwidthHighThresh_get(unit, port, &result_thresh);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_igrBandwidthLowThresh_get (unit %u, port %u, thresh %u)"
                                    , ret, expect_result, unit, port, thresh);
                }
                else
                {
                   /*This is get value API only, so failed case is meaningless over here.*/
                }
            }
        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_portStormCtrlProtoRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlProtoRate_set(unit, 0, TEST_STORM_PROTOTYPE_MIN, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlProtoRate_get(unit, 0, TEST_STORM_PROTOTYPE_MIN, &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_rate_storm_proto_group_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;

        uint32  result_rate;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_PROTOTYPE_MAX(unit), TEST_STORM_PROTOTYPE_MIN, storm, storm_result, storm_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_PROTORATE_LIMIT_MAX(unit), TEST_STORM_PROTORATE_LIMIT_MIN, rate, rate_result, rate_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];
                for (rate_index = 0; rate_index <= rate_last; rate_index++)
                {
                    inter_result4 = rate_result[rate_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlProtoRate_set(unit, port[port_index], storm[storm_index], rate[rate_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port[port_index], storm[storm_index], rate[rate_index]);

                        ret = rtk_rate_portStormCtrlProtoRate_get(unit, port[port_index], storm[storm_index], &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port[port_index], storm[storm_index], result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate[rate_index], unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set (unit %u, port %u, storm %u, rate %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], rate[rate_index]);
                    }
                }
                ret = rtk_rate_portStormCtrlProtoRate_get(unit, port[port_index], storm[storm_index], (uint32*)&result_rate);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_proto_group_t  storm;
        rtk_rate_storm_proto_group_t  storm_max;
        uint32  rate;
        uint32  rate_max;

        uint32  result_rate;


        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_PROTOTYPE_MAX(unit), storm_max);
        G_VALUE(TEST_STORM_PROTORATE_LIMIT_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_STORM_PROTOTYPE_MIN, storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_PROTOTYPE_MIN, TEST_STORM_PROTOTYPE_MAX(unit), inter_result3);

                for (L_VALUE(TEST_STORM_PROTORATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
                {
                    ASSIGN_EXPECT_RESULT(rate, TEST_STORM_PROTORATE_LIMIT_MIN, TEST_STORM_PROTORATE_LIMIT_MAX(unit), inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlProtoRate_set(unit, port, storm, rate);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set (unit %u, port %u, storm %u, rate %u)"
                                        , ret, expect_result, unit, port, storm, rate);

                        ret = rtk_rate_portStormCtrlProtoRate_get(unit, port, storm, &result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u, rate %u)"
                                         , ret, expect_result, unit, port, storm, result_rate);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set/get value unequal (unit %u, port %u, storm %u, rate %u)"
                                         , result_rate, rate, unit, port, storm, rate);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_set (unit %u, port %u, storm %u, rate %u)"
                                            , ret, RT_ERR_OK, unit, port, storm, rate);
                    }
                    ret = rtk_rate_portStormCtrlProtoRate_get(unit, port, storm, &result_rate);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port, storm);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoRate_get (unit %u, port %u, storm %u)"
                                            , ret, RT_ERR_OK, unit, port, storm);
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

extern int32 dal_rate_stormControlProtoExceed_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  exceed_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlProtoExceed_get(unit, 0, TEST_STORM_PROTOTYPE_MIN, &exceed_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_rate_storm_proto_group_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;

        uint32  exceed;



        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_PROTOTYPE_MAX(unit), TEST_STORM_PROTOTYPE_MIN, storm, storm_result, storm_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portStormCtrlProtoExceed_get(unit, port[port_index], storm[storm_index], &exceed);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index], exceed);
                    ret = rtk_rate_portStormCtrlProtoExceed_get(unit, port[port_index], storm[storm_index], NULL);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_NULL_POINTER, unit, port[port_index], storm[storm_index]);

                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port[port_index], storm[storm_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlProtoExceed_reset (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], exceed);

                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port[port_index], storm[storm_index]);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlProtoExceed_reset (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_proto_group_t  storm;
        rtk_rate_storm_proto_group_t  storm_max;

        uint32  exceed;


        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_PROTOTYPE_MAX(unit), storm_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_STORM_PROTOTYPE_MIN, storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_PROTOTYPE_MIN, TEST_STORM_PROTOTYPE_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_portStormCtrlProtoExceed_get(unit, port, storm, &exceed);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                    , ret, expect_result, unit, port, storm, exceed);
                    ret = rtk_rate_portStormCtrlProtoExceed_get(unit, port, storm, NULL);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_NULL_POINTER, unit, port, storm);

                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port, storm);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlProtoExceed_reset (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port, storm);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlProtoExceed_get (unit %u, port %u, storm %u, exceed %u)"
                                        , ret, RT_ERR_OK, unit, port, storm, exceed);

                    ret = rtk_rate_portStormCtrlProtoExceed_reset(unit, port, storm);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlProtoExceed_reset (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port, storm);
                }
            }

        }
    }
    return RT_ERR_OK;
}

extern int32 dal_rate_stormControlBypass_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_stormControlBypass_set(unit, TEST_RATE_STORM_BYPASS_TYPE_MIN, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_stormControlBypass_get(unit, TEST_RATE_STORM_BYPASS_TYPE_MIN,&enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_rate_storm_bypassType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_RATE_STORM_BYPASS_TYPE_MAX(unit), TEST_RATE_STORM_BYPASS_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (type_index = 0; type_index <= type_last; type_index++)
           {
            inter_result2 = type_result[type_index];
                for (enable_index = 0; enable_index <= enable_last; enable_index++)
               {
                   inter_result3 = enable_result[enable_index];
                   if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_rate_stormControlBypass_set(unit, type[type_index], enable[enable_index]);
                    if (RT_ERR_OK == expect_result)
                   {
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_set (unit %u, type %u, enable %u)"
                                       , ret, expect_result, unit, type[type_index],  enable[enable_index]);

                       ret = rtk_rate_stormControlBypass_get(unit, type[type_index], &result_enable);
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u, result %u)"
                                        , ret, expect_result, unit, type[type_index], enable[enable_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_set/get value unequal (unit %u, type %u, enable %u, result_enable %u)"
                                         , result_enable, enable[enable_index], unit, type[type_index], enable[enable_index], result_enable);
                   }
                   else
                   {
                       RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlBypass_set (unit %u, type %u, enable %u)"
                                           , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index]);
                }
                   ret = rtk_rate_stormControlBypass_get(unit, type[type_index], &result_enable);
                   if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                {
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u, result %u)"
                                       , ret, expect_result, unit, type[type_index], enable[enable_index], result_enable);
                   }
                   else
                    {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u, result %u)"
                                           , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index], result_enable);
                   }

               }
           }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_rate_storm_bypassType_t  type;
        rtk_rate_storm_bypassType_t  type_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;


        G_VALUE(TEST_RATE_STORM_BYPASS_TYPE_MAX(unit), type_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_RATE_STORM_BYPASS_TYPE_MIN, type); type <= type_max; type++)
           {
            ASSIGN_EXPECT_RESULT(type, TEST_RATE_STORM_BYPASS_TYPE_MIN, TEST_RATE_STORM_BYPASS_TYPE_MAX(unit), inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                   ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                   if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                   ret = rtk_rate_stormControlBypass_set(unit, type, enable);
                   if (RT_ERR_OK == expect_result)
                {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_set (unit %u, type %u, enable %u)"
                                       , ret, expect_result, unit, type, enable);

                       ret = rtk_rate_stormControlBypass_get(unit, type ,&result_enable);
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u)"
                                        , ret, expect_result, unit, type, result_enable);
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_set/get value unequal (unit %u, type %u, enable %u)"
                                        , result_enable, enable, unit, type, enable);
                }
                    else
                   {
                       RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlBypass_set (unit %u, type %u, enable %u)"
                                           , ret, RT_ERR_OK, unit, type, enable);
                   }
                ret = rtk_rate_stormControlBypass_get(unit, type,&result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                   if (expect_result == RT_ERR_OK)
                   {
                       RT_TEST_IS_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u)"
                                       , ret, expect_result, unit, type, result_enable);
                }
                    else
                   {
                       RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_stormControlBypass_get (unit %u, type %u, enable %u)"
                                           , ret, RT_ERR_OK, unit, type, result_enable);
                   }
               }
           }
    }
    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
#define STORM_GROUP_NUM                        STORM_GROUP_END
static rt_error_common_t stromGroup[STORM_GROUP_NUM+1] =
{RT_ERR_OK, RT_ERR_FAILED, RT_ERR_OK, RT_ERR_FAILED};
#endif

extern int32 dal_rate_portStormCtrlTypeSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_rate_storm_sel_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlTypeSel_set(unit, 0, TEST_STORM_TYPE_MIN(unit), TEST_RATE_STORM_SEL_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portStormCtrlTypeSel_get(unit, 0, TEST_STORM_TYPE_MIN(unit), &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_rate_storm_group_t  storm[UNITTEST_MAX_TEST_VALUE];
        int32  storm_result[UNITTEST_MAX_TEST_VALUE];
        int32  storm_index;
        int32  storm_last;

        rtk_rate_storm_sel_t  sel[UNITTEST_MAX_TEST_VALUE];
        int32  sel_result[UNITTEST_MAX_TEST_VALUE];
        int32  sel_index;
        int32  sel_last = 0;
        rtk_rate_storm_sel_t  result_sel = 0;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_STORM_TYPE_MAX, TEST_STORM_TYPE_MIN(unit), storm, storm_result, storm_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RATE_STORM_SEL_MAX, TEST_RATE_STORM_SEL_MIN, sel, sel_result, sel_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (storm_index = 0; storm_index <= storm_last; storm_index++)
            {
                inter_result3 = storm_result[storm_index];
                if((RT_ERR_OK == stromGroup[storm[storm_index]]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(STORM_GROUP_BROADCAST == storm[storm_index])
                    inter_result3 = 0; /*The STORM_GROUP_BROADCAST is NOT support by RTL8390*/
                for (sel_index = 0; sel_index <= sel_last; sel_index++)
                {
                    inter_result4 = sel_result[sel_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlTypeSel_set(unit, port[port_index], storm[storm_index], sel[sel_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set (unit %u, port %u, storm %u, sel %u)"
                                        , ret, expect_result, unit, port[port_index], storm[storm_index], sel[sel_index]);

                        ret = rtk_rate_portStormCtrlTypeSel_get(unit, port[port_index], storm[storm_index], &result_sel);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u, sel %u)"
                                         , ret, expect_result, unit, port[port_index], storm[storm_index], result_sel);

                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set/get value unequal (unit %u, port %u, storm %u, sel %u)"
                                         , result_sel, sel[sel_index], unit, port[port_index], storm[storm_index], sel[sel_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set (unit %u, port %u, storm %u, sel %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index], sel[sel_index]);
                    }
                }
                ret = rtk_rate_portStormCtrlTypeSel_get(unit, port[port_index], storm[storm_index], (rtk_rate_storm_sel_t *)&result_sel);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u)"
                                    , ret, expect_result, unit, port[port_index], storm[storm_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], storm[storm_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_rate_storm_group_t  storm;
        rtk_rate_storm_group_t  storm_max;
        rtk_rate_storm_sel_t  sel;
        rtk_rate_storm_sel_t  sel_max;
        rtk_rate_storm_sel_t  result_sel;


        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_STORM_TYPE_MAX, storm_max);
        G_VALUE(TEST_RATE_STORM_SEL_MAX, sel_max);


        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_STORM_TYPE_MIN(unit), storm); storm <= storm_max; storm++)
            {
                ASSIGN_EXPECT_RESULT(storm, TEST_STORM_TYPE_MIN(unit), TEST_STORM_TYPE_MAX, inter_result3);

                if((RT_ERR_OK == stromGroup[storm]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(STORM_GROUP_BROADCAST == storm)
                    inter_result3 = 0; /*The STORM_GROUP_BROADCAST is NOT support by RTL8390*/
                for (L_VALUE(TEST_RATE_STORM_SEL_MIN, sel); sel <= sel_max; sel++)
                {
                    ASSIGN_EXPECT_RESULT(sel, TEST_RATE_STORM_SEL_MIN, TEST_RATE_STORM_SEL_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_rate_portStormCtrlTypeSel_set(unit, port, storm, sel);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set (unit %u, port %u, storm %u, sel %u)"
                                        , ret, expect_result, unit, port, storm, sel);

                        ret = rtk_rate_portStormCtrlTypeSel_get(unit, port, storm, &result_sel);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u, sel %u)"
                                         , ret, expect_result, unit, port, storm, result_sel);
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set/get value unequal (unit %u, port %u, storm %u, sel %u)"
                                         , result_sel, sel, unit, port, storm, sel);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_set (unit %u, port %u, storm %u, sel %u)"
                                            , ret, RT_ERR_OK, unit, port, storm, sel);

                    }
                    ret = rtk_rate_portStormCtrlTypeSel_get(unit, port, storm, &result_sel);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u)"
                                        , ret, expect_result, unit, port, storm);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portStormCtrlTypeSel_get (unit %u, port %u, storm %u)"
                                            , ret, RT_ERR_OK, unit, port, storm);
                    }
                }
            }

        }
    }
    return RT_ERR_OK;
}
extern int32 dal_rate_portIgrBwCtrlRate_10G_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  rate_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    uint32  is10G_port = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlRate_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_rate_portIgrBwCtrlRate_get(unit, 0, &rate_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;
        uint32  result_rate;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_RATE_OF_10G_BANDWIDTH_MAX(unit), TEST_RATE_LIMIT_MIN, rate, rate_result, rate_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            is10G_port = UNEXPECTED_PORT_TYPE;
            inter_result2 = port_result[port_index];
            if((HWP_UNIT_VALID_LOCAL(unit)) && (1 == inter_result2))
            {
                if(HWP_10GE_PORT(unit, port[port_index]))
                {
                    is10G_port = EXPECTED_PORT_TYPE;
                }else{
                    is10G_port = UNEXPECTED_PORT_TYPE;
                    inter_result2 = 0;
                }
            }

            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(TEST_MAX_UNIT_ID < unit || TEST_MIN_UNIT_ID > unit)
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if(EXPECTED_PORT_TYPE == is10G_port)
                       ret = rtk_rate_portIgrBwCtrlRate_set(unit, port[port_index], rate[rate_index]);
                else
                    ret = RT_ERR_FAILED;

                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port[port_index], rate[rate_index]);

                    ret = rtk_rate_portIgrBwCtrlRate_get(unit, port[port_index], &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port[port_index], result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u result %u)"
                                     , result_rate, rate[rate_index], unit, port[port_index], rate[rate_index], result_rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  rate;
        uint32  rate_max;
        uint32  result_rate;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(HAL_RATE_OF_10G_BANDWIDTH_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            is10G_port = UNEXPECTED_PORT_TYPE;
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);
            if((HWP_UNIT_VALID_LOCAL(unit)) && (1 == inter_result2))
            {
                if(HWP_10GE_PORT(unit, port))
                {
                    is10G_port = EXPECTED_PORT_TYPE;
                }else{
                    is10G_port = UNEXPECTED_PORT_TYPE;
                    inter_result2 = 0;
                }
            }
            for (L_VALUE(TEST_RATE_LIMIT_MIN, rate); rate <= rate_max; rate++)
            {
                ASSIGN_EXPECT_RESULT(rate, TEST_RATE_LIMIT_MIN, HAL_RATE_OF_10G_BANDWIDTH_MAX(unit), inter_result3);
                if(TEST_MAX_UNIT_ID < unit || TEST_MIN_UNIT_ID > unit)
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if(EXPECTED_PORT_TYPE == is10G_port)
                    ret = rtk_rate_portIgrBwCtrlRate_set(unit, port, rate);
                else
                    ret = RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port, rate);

                    ret = rtk_rate_portIgrBwCtrlRate_get(unit, port, &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port, result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set/get value unequal (unit %u, port %u, rate %u)"
                                     , result_rate, rate, unit, port, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_rate_portIgrBwCtrlRate_set (unit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port, rate);
                }
            }
        }
    }
    return RT_ERR_OK;
} /* dal_rate_portIgrBwCtrlRate_10G_test() */




