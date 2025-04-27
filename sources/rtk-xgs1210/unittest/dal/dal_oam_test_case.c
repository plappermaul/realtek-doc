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
#include <rtk/oam.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define OAM_RANDOM_RUN_TIMES          100
/*CFM Entry*/
#define TEST_CFMENTRY_ID_MIN           0
#define TEST_CFMENTRY_ID_MAX           1
/*Interface Status*/
#define TEST_INTERFACE_STATUS_MIN      1
#define TEST_INTERFACE_STATUS_MAX      7
/*Port Status*/
#define TEST_PORT_STATUS_MIN           1
#define TEST_PORT_STATUS_MAX           2
/* ACTION*/
#define TEST_ACTION_ID_MIN             ACTION_FORWARD
#define TEST_ACTION_ID_MAX             ACTION_DROP
/* PCP*/
#define TEST_PCP_ID_MIN                0
#define TEST_PCP_ID_MAX                RTK_DOT1P_PRIORITY_MAX
/* CFI*/
#define TEST_CFI_ID_MIN                0
#define TEST_CFI_ID_MAX                RTK_DOT1P_CFI_MAX
/* TPID*/
#define TEST_TPID_ID_MIN               0
#define TEST_TPID_ID_MAX               RTK_TPID_MAX
/* Tx Instance*/
#define TEST_TX_INSTANCE_MIN           0
#define TEST_TX_INSTANCE_MAX           RTK_CFM_TX_INSTANCE_MAX-1
/* Reset Lifetime*/
#define TEST_RESET_LIFETIME_MIN        0
#define TEST_RESET_LIFETIME_MAX        RTK_CFM_RESET_LIFETIME_MAX
/* MEPIF*/
#define TEST_MEPID_MIN                 0
#define TEST_MEPID_MAX                 RTK_CFM_MEPID_MAX
/* Lifetime*/
#define TEST_LIFETIME_MIN              0
#define TEST_LIFETIME_MAX              RTK_CFM_LIFETIME_MAX-1
/* Mdl*/
#define TEST_MDL_ID_MIN                0
#define TEST_MDL_ID_MAX                RTK_CFM_MDL_MAX-1
/* Maid*/
#define TEST_MAID_ID_MIN               0
#define TEST_MAID_ID_MAX               RTK_CFM_MAID_MAX-1
/* Tx Interval*/
#define TEST_TX_INTERVAL_MIN           0
#define TEST_TX_INTERVAL_MAX           RTK_CFM_TX_INTERVAL
/* CCM Port*/
#define TEST_CCM_PORT_IDX_MIN          0
#define TEST_CCM_PORT_IDX_MAX          RTK_CFM_CCM_PORT_MAX-1
/* Keepalive*/
#define TEST_KEEPALIVE_MIN             0
#define TEST_KEEPALIVE_MAX             0x3FF

int32 dal_oam_autoDyingGaspEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_autoDyingGaspEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_autoDyingGaspEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32 enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
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
                ret = rtk_oam_autoDyingGaspEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_oam_autoDyingGaspEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_oam_autoDyingGaspEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        uint32  enable;
        uint32  enable_max;
        uint32  result_enable;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);


        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(ACTION_FORWARD, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_autoDyingGaspEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_oam_autoDyingGaspEnable_get(unit, port, (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_autoDyingGaspEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }


    return RT_ERR_OK;
}   /*end of dal_oam_autoDyingGaspEnable_test*/

int32 dal_oam_dyingGaspWaitTime_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  waitTime_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_dyingGaspWaitTime_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_dyingGaspWaitTime_get(unit, &waitTime_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 waitTime;
        uint32 waitTime_max;
        int32  waitTime_result;
        G_VALUE(HAL_DYING_GASP_SUSTAIN_TIME_MAX(unit), waitTime_max);

        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        for (L_VALUE(0, waitTime); waitTime < waitTime_max; waitTime++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            ret = rtk_oam_dyingGaspWaitTime_set(unit, waitTime);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_dyingGaspWaitTime_set (unit %u, waitTime %u)"
                                , ret, expect_result, unit, waitTime);

                ret = rtk_oam_dyingGaspWaitTime_get(unit, (uint32*)&waitTime_result);
                RT_TEST_IS_EQUAL_INT("rtk_oam_dyingGaspWaitTime_get (unit %u, waitTime %u)"
                                 , ret, expect_result, unit, waitTime);
                RT_TEST_IS_EQUAL_INT("rtk_oam_dyingGaspWaitTime_set/get value unequal (unit %u, waitTime %u)"
                                 , waitTime_result, waitTime, unit, waitTime);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_dyingGaspWaitTime_set (unit %u, waitTime %u)"
                                    , ret, RT_ERR_OK, unit, waitTime);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_dyingGaspWaitTime_test*/

int32 dal_oam_loopbackMacSwapEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_loopbackMacSwapEnable_set(unit, TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_loopbackMacSwapEnable_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            inter_result2 = enable_result[enable_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_loopbackMacSwapEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_oam_loopbackMacSwapEnable_get(unit, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }

        ret = rtk_oam_loopbackMacSwapEnable_get(unit, (rtk_enable_t*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

       for (L_VALUE(ACTION_FORWARD, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_loopbackMacSwapEnable_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_oam_loopbackMacSwapEnable_get(unit, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_loopbackMacSwapEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_loopbackMacSwapEnable_test*/

int32 dal_oam_portLoopbackMuxAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_portLoopbackMuxAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_portLoopbackMuxAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_action_t action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ACTION_ID_MAX, TEST_ACTION_ID_MIN, action, action_result, action_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_portLoopbackMuxAction_set(unit, port[port_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_oam_portLoopbackMuxAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_oam_portLoopbackMuxAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_portLoopbackMuxAction_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ACTION_ID_MAX, action_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ACTION_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ACTION_ID_MIN, TEST_ACTION_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_portLoopbackMuxAction_set(unit, port, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);
                    ret = rtk_oam_portLoopbackMuxAction_get(unit, port, (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, action);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_portLoopbackMuxAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_portLoopbackMuxAction_test*/

int32 dal_oam_cfmCcmPcp_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pcp_temp;
    int8  inter_result1 = 1;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmPcp_set(unit, TEST_PCP_ID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmPcp_get(unit, &pcp_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  pcp[UNITTEST_MAX_TEST_VALUE];
        int32  pcp_result[UNITTEST_MAX_TEST_VALUE];
        int32  pcp_index;
        int32  pcp_last;

        uint32  result_pcp;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PCP_ID_MAX, TEST_PCP_ID_MIN, pcp, pcp_result, pcp_last);

        for (pcp_index = 0; pcp_index <= pcp_last; pcp_index++)
        {
            inter_result2 = pcp_result[pcp_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmPcp_set(unit, pcp[pcp_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_set (unit %u, pcp %u)"
                                , ret, expect_result, unit, pcp[pcp_index]);

                ret = rtk_oam_cfmCcmPcp_get(unit, &result_pcp);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_get (unit %u, pcp %u)"
                                 , ret, expect_result, unit, result_pcp);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_set/get value unequal (unit %u, pcp %u)"
                                 , result_pcp, pcp[pcp_index], unit, pcp[pcp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmPcp_set (unit %u, pcp %u)"
                                    , ret, RT_ERR_OK, unit, pcp[pcp_index]);
            }

            ret = rtk_oam_cfmCcmPcp_get(unit, &result_pcp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result1)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_get (unit %u, pcp %u)"
                                , ret, expect_result, unit, result_pcp);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmPcp_get (unit %u, pcp %u)"
                                    , ret, RT_ERR_OK, unit, result_pcp);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  pcp;
        uint32  pcp_max;
        uint32  result_pcp;

        G_VALUE(TEST_PCP_ID_MAX, pcp_max);

        for (L_VALUE(TEST_PCP_ID_MIN, pcp); pcp <= pcp_max; pcp++)
            {
                ASSIGN_EXPECT_RESULT(pcp, TEST_PCP_ID_MIN, TEST_PCP_ID_MAX, inter_result2);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmPcp_set(unit, pcp);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_set (unit %u, pcp %u)"
                                    , ret, expect_result, unit, pcp);
                    ret = rtk_oam_cfmCcmPcp_get(unit, &result_pcp);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_get (unit %u, pcp %u)"
                                     , ret, expect_result, unit, pcp);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmPcp_set/get value unequal (unit %u, pcp %u)"
                                     , result_pcp, pcp, unit, pcp);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmPcp_set (unit %u, pcp %u)"
                                        , ret, RT_ERR_OK, unit, pcp);

            }
        }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmPcp_test */

int32 dal_oam_cfmCcmCfi_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  cfi_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmCfi_set(unit, TEST_CFI_ID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmCfi_get(unit, &cfi_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  cfi[UNITTEST_MAX_TEST_VALUE];
        int32  cfi_result[UNITTEST_MAX_TEST_VALUE];
        int32  cfi_index;
        int32  cfi_last;

        uint32  result_cfi;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_CFI_ID_MAX, TEST_CFI_ID_MIN, cfi, cfi_result, cfi_last);

        for (cfi_index = 0; cfi_index <= cfi_last; cfi_index++)
        {
            inter_result2 = cfi_result[cfi_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmCfi_set(unit, cfi[cfi_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_set (unit %u, cfi %u)"
                                , ret, expect_result, unit, cfi[cfi_index]);

                ret = rtk_oam_cfmCcmCfi_get(unit, &result_cfi);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_get (unit %u, cfi %u)"
                                 , ret, expect_result, unit, result_cfi);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_set/get value unequal (unit %u, cfi %u)"
                                 , result_cfi, cfi[cfi_index], unit, cfi[cfi_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmCfi_set (unit %u, cfi %u)"
                                    , ret, RT_ERR_OK, unit, cfi[cfi_index]);
            }

            ret = rtk_oam_cfmCcmCfi_get(unit, &result_cfi);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_get (unit %u, cfi %u)"
                                , ret, expect_result, unit, result_cfi);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmCfi_get (unit %u, cfi %u)"
                                    , ret, RT_ERR_OK, unit, result_cfi);
            }
        }
}

    if (IS_TEST_SCAN_MODE())
    {
        uint32  cfi;
        uint32  cfi_max;

        uint32  result_cfi;

        G_VALUE(TEST_CFI_ID_MAX, cfi_max);

        for (L_VALUE(TEST_CFI_ID_MIN, cfi); cfi <= cfi_max; cfi++)
        {
            ASSIGN_EXPECT_RESULT(cfi, TEST_CFI_ID_MIN, TEST_CFI_ID_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmCfi_set(unit, cfi);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_set (unit %u, cfi %u)"
                                , ret, expect_result, unit, cfi);
                ret = rtk_oam_cfmCcmCfi_get(unit, &result_cfi);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_get (unit %u, cfi %u)"
                                 , ret, expect_result, unit, cfi);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmCfi_set/get value unequal (unit %u, cfi %u)"
                                 , result_cfi, cfi, unit, cfi);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmCfi_set (unit %u, cfi %u)"
                                    , ret, RT_ERR_OK, unit, cfi);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmCfi_test */

int32 dal_oam_cfmCcmTpid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmTpid_set(unit, TEST_CFI_ID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmTpid_get(unit, &tpid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  tpid[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_result[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_index;
        int32  tpid_last;

        uint32  result_tpid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TPID_ID_MAX, TEST_TPID_ID_MIN, tpid, tpid_result, tpid_last);

        for (tpid_index = 0; tpid_index <= tpid_last; tpid_index++)
        {
            inter_result2 = tpid_result[tpid_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmTpid_set(unit, tpid[tpid_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_set (unit %u, tpid %u)"
                                , ret, expect_result, unit, tpid[tpid_index]);

                ret = rtk_oam_cfmCcmTpid_get(unit, &result_tpid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_get (unit %u, tpid %u)"
                                 , ret, expect_result, unit, result_tpid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_set/get value unequal (unit %u, tpid %u)"
                                 , result_tpid, tpid[tpid_index], unit, tpid[tpid_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTpid_set (unit %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid[tpid_index]);
            }

            ret = rtk_oam_cfmCcmTpid_get(unit, &result_tpid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_get (unit %u, tpid %u)"
                                , ret, expect_result, unit, result_tpid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTpid_get (unit %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, result_tpid);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  tpid;
        uint32  tpid_max;
        uint32  result_tpid;

        G_VALUE(TEST_TPID_ID_MAX, tpid_max);

        for (L_VALUE(TEST_TPID_ID_MIN, tpid); tpid <= tpid_max; tpid++)
        {
            ASSIGN_EXPECT_RESULT(tpid, TEST_TPID_ID_MIN, TEST_TPID_ID_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmTpid_set(unit, tpid);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_set (unit %u, tpid %u)"
                                , ret, expect_result, unit, tpid);
                ret = rtk_oam_cfmCcmTpid_get(unit, &result_tpid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_get (unit %u, tpid %u)"
                                 , ret, expect_result, unit, tpid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTpid_set/get value unequal (unit %u, tpid %u)"
                                 , result_tpid, tpid, unit, tpid);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTpid_set (unit %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmTpid_test */

int32 dal_oam_cfmCcmInstLifetime_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  lifetime_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstLifetime_set(unit, TEST_TX_INSTANCE_MIN, TEST_RESET_LIFETIME_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstLifetime_get(unit, TEST_TX_INSTANCE_MIN, &lifetime_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32  lifetime[UNITTEST_MAX_TEST_VALUE];
        int32  lifetime_result[UNITTEST_MAX_TEST_VALUE];
        int32  lifetime_index;
        int32  lifetime_last;

        uint32  result_lifetime;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RESET_LIFETIME_MAX, TEST_RESET_LIFETIME_MIN, lifetime, lifetime_result, lifetime_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (lifetime_index = 0; lifetime_index <= lifetime_last; lifetime_index++)
            {
                inter_result3 = lifetime_result[lifetime_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstLifetime_set(unit, instance[instance_index], lifetime[lifetime_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set (unit %u, instance %u, lifetime %u)"
                                    , ret, expect_result, unit, instance[instance_index], lifetime[lifetime_index]);

                    ret = rtk_oam_cfmCcmInstLifetime_get(unit, instance[instance_index], (uint32*)&result_lifetime);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_get (unit %u, instance %u, lifetime %u)"
                                     , ret, expect_result, unit, instance[instance_index], lifetime[lifetime_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set/get value unequal (unit %u, instance %u, lifetime %u)"
                                     , result_lifetime, lifetime[lifetime_index], unit, instance[instance_index], lifetime[lifetime_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set (unit %u, instance %u, lifetime %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], lifetime[lifetime_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstLifetime_get(unit, instance[instance_index], (uint32*)&result_lifetime);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  lifetime;
        uint32  lifetime_max;
        uint32  result_lifetime;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_RESET_LIFETIME_MAX, lifetime_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_RESET_LIFETIME_MIN, lifetime); lifetime <= lifetime_max; lifetime++)
            {
                ASSIGN_EXPECT_RESULT(lifetime, TEST_RESET_LIFETIME_MIN, TEST_RESET_LIFETIME_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstLifetime_set(unit, instance, lifetime);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set (unit %u, instance %u, lifetime %u)"
                                    , ret, expect_result, unit, instance, lifetime);
                    ret = rtk_oam_cfmCcmInstLifetime_get(unit, instance, (uint32*)&result_lifetime);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_get (unit %u, instance %u, lifetime %u)"
                                     , ret, expect_result, unit, instance, lifetime);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set/get value unequal (unit %u, instance %u, lifetime %u)"
                                     , result_lifetime, lifetime, unit, instance, lifetime);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstLifetime_set (unit %u, instance %u, lifetime %u)"
                                        , ret, RT_ERR_OK, unit, instance, lifetime);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmResetLifetime_test */

int32 dal_oam_cfmCcmMepid_test(uint32 caseNo, uint32 unit)
{

    int32 ret;
    uint32  mepid_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmMepid_set(unit, TEST_MEPID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmMepid_get(unit, &mepid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  mepid[UNITTEST_MAX_TEST_VALUE];
        int32  mepid_result[UNITTEST_MAX_TEST_VALUE];
        int32  mepid_index;
        int32  mepid_last;

        uint32  result_mepid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MEPID_MAX, TEST_MEPID_MIN, mepid, mepid_result, mepid_last);

        for (mepid_index = 0; mepid_index <= mepid_last; mepid_index++)
        {
            inter_result2 = mepid_result[mepid_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmMepid_set(unit, mepid[mepid_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_set (unit %u, mepid %u)"
                                , ret, expect_result, unit, mepid[mepid_index]);

                ret = rtk_oam_cfmCcmMepid_get(unit, &result_mepid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_get (unit %u, mepid %u)"
                                 , ret, expect_result, unit, result_mepid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_set/get value unequal (unit %u, mepid %u)"
                                 , result_mepid, mepid[mepid_index], unit, mepid[mepid_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMepid_set (unit %u, mepid %u)"
                                    , ret, RT_ERR_OK, unit, mepid[mepid_index]);
            }

            ret = rtk_oam_cfmCcmMepid_get(unit, &result_mepid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_get (unit %u, mepid %u)"
                                , ret, expect_result, unit, result_mepid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMepid_get (unit %u, mepid %u)"
                                    , ret, RT_ERR_OK, unit, result_mepid);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  mepid;
        uint32  mepid_max;

        uint32  result_mepid;

        G_VALUE(TEST_MEPID_MAX, mepid_max);

        for (L_VALUE(TEST_MEPID_MIN, mepid); mepid <= mepid_max; mepid++)
        {
            ASSIGN_EXPECT_RESULT(mepid, TEST_MEPID_MIN, TEST_MEPID_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmMepid_set(unit, mepid);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_set (unit %u, mepid %u)"
                                , ret, expect_result, unit, mepid);
                ret = rtk_oam_cfmCcmMepid_get(unit, &result_mepid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_get (unit %u, mepid %u)"
                                 , ret, expect_result, unit, mepid);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMepid_set/get value unequal (unit %u, mepid %u)"
                                 , result_mepid, mepid, unit, mepid);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMepid_set (unit %u, mepid %u)"
                                    , ret, RT_ERR_OK, unit, mepid);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmMepid_test */

int32 dal_oam_cfmCcmIntervalField_test(uint32 caseNo, uint32 unit)
{

    uint32  interval_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    int32 ret;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmIntervalField_set(unit, TEST_LIFETIME_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmIntervalField_get(unit, &interval_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  interval[UNITTEST_MAX_TEST_VALUE];
        int32  interval_result[UNITTEST_MAX_TEST_VALUE];
        int32  interval_index;
        int32  interval_last;

        uint32  result_interval;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_LIFETIME_MAX, TEST_LIFETIME_MIN, interval, interval_result, interval_last);

        for (interval_index = 0; interval_index <= interval_last; interval_index++)
        {
            inter_result2 = interval_result[interval_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = ( inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmIntervalField_set(unit, interval[interval_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set (unit %u, interval %u)"
                                , ret, expect_result, unit, interval[interval_index]);

                ret = rtk_oam_cfmCcmIntervalField_get(unit, &result_interval);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_get (unit %u, interval %u)"
                                 , ret, expect_result, unit, result_interval);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set/get value unequal (unit %u, interval %u)"
                                 , result_interval, interval[interval_index], unit, interval[interval_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set (unit %u, interval %u)"
                                    , ret, RT_ERR_OK, unit, interval[interval_index]);
            }

            ret = rtk_oam_cfmCcmIntervalField_get(unit, &result_interval);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_get (unit %u, interval %u)"
                                , ret, expect_result, unit, result_interval);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmIntervalField_get (unit %u, interval %u)"
                                    , ret, RT_ERR_OK, unit, result_interval);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  interval;
        uint32  interval_max;
        uint32  result_interval;

        G_VALUE(TEST_LIFETIME_MAX, interval_max);

        for (L_VALUE(TEST_LIFETIME_MIN, interval); interval <= interval_max; interval++)
        {
            ASSIGN_EXPECT_RESULT(interval, TEST_LIFETIME_MIN, TEST_LIFETIME_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmIntervalField_set(unit, interval);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set (unit %u, interval %u)"
                                , ret, expect_result, unit, interval);
                ret = rtk_oam_cfmCcmIntervalField_get(unit, &result_interval);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_get (unit %u, interval %u)"
                                 , ret, expect_result, unit, interval);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set/get value unequal (unit %u, interval %u)"
                                 , result_interval, interval, unit, interval);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmIntervalField_set (unit %u, interval %u)"
                                    , ret, RT_ERR_OK, unit, interval);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmIntervalField_test */

int32 dal_oam_cfmCcmMdl_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  mdl_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmMdl_set(unit, TEST_MDL_ID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmMdl_get(unit, &mdl_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  mdl[UNITTEST_MAX_TEST_VALUE];
        int32  mdl_result[UNITTEST_MAX_TEST_VALUE];
        int32  mdl_index;
        int32  mdl_last;

        uint32  result_mdl;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MDL_ID_MAX, TEST_MDL_ID_MIN, mdl, mdl_result, mdl_last);

        for (mdl_index = 0; mdl_index <= mdl_last; mdl_index++)
        {
            inter_result2 = mdl_result[mdl_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmMdl_set(unit, mdl[mdl_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_set (unit %u, mdl %u)"
                                , ret, expect_result, unit, mdl[mdl_index]);

                ret = rtk_oam_cfmCcmMdl_get(unit, &result_mdl);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_get (unit %u, mdl %u)"
                                 , ret, expect_result, unit, result_mdl);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_set/get value unequal (unit %u, mdl %u)"
                                 , result_mdl, mdl[mdl_index], unit, mdl[mdl_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMdl_set (unit %u, mdl %u)"
                                    , ret, RT_ERR_OK, unit, mdl[mdl_index]);
            }

            ret = rtk_oam_cfmCcmMdl_get(unit, &result_mdl);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result =RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_get (unit %u, mdl %u)"
                                , ret, expect_result, unit, result_mdl);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMdl_get (unit %u, mdl %u)"
                                    , ret, RT_ERR_OK, unit, result_mdl);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  mdl;
        uint32  mdl_max;

        uint32  result_mdl;

        G_VALUE(TEST_MDL_ID_MAX, mdl_max);

        for (L_VALUE(TEST_MDL_ID_MIN, mdl); mdl <= mdl_max; mdl++)
        {
            ASSIGN_EXPECT_RESULT(mdl, TEST_MDL_ID_MIN, TEST_MDL_ID_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_cfmCcmMdl_set(unit, mdl);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_set (unit %u, mdl %u)"
                                , ret, expect_result, unit, mdl);
                ret = rtk_oam_cfmCcmMdl_get(unit, &result_mdl);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_get (unit %u, mdl %u)"
                                 , ret, expect_result, unit, mdl);
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmMdl_set/get value unequal (unit %u, mdl %u)"
                                 , result_mdl, mdl, unit, mdl);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmMdl_set (unit %u, mdl %u)"
                                    , ret, RT_ERR_OK, unit, mdl);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmMdl_test */

int32 dal_oam_cfmCcmInstTagStatus_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstTagStatus_set(unit, TEST_TX_INSTANCE_MIN, TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstTagStatus_get(unit, TEST_TX_INSTANCE_MIN, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstTagStatus_set(unit, instance[instance_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set (unit %u, instance %u, enable %u)"
                                    , ret, expect_result, unit, instance[instance_index], enable[enable_index]);

                    ret = rtk_oam_cfmCcmInstTagStatus_get(unit, instance[instance_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_get (unit %u, instance %u, enable %u)"
                                     , ret, expect_result, unit, instance[instance_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set/get value unequal (unit %u, instance %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, instance[instance_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set (unit %u, instance %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], enable[enable_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstTagStatus_get(unit, instance[instance_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstTagStatus_set(unit, instance, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set (unit %u, instance %u, enable %u)"
                                    , ret, expect_result, unit, instance, enable);
                    ret = rtk_oam_cfmCcmInstTagStatus_get(unit, instance, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_get (unit %u, instance %u, enable %u)"
                                     , ret, expect_result, unit, instance, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set/get value unequal (unit %u, instance %u, enable %u)"
                                     , result_enable, enable, unit, instance, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTagStatus_set (unit %u, instance %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, instance, enable);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstTagStatus_test */

int32 dal_oam_cfmCcmInstVid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  vid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstVid_set(unit, TEST_TX_INSTANCE_MIN, TEST_RESET_LIFETIME_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstVid_get(unit, TEST_TX_INSTANCE_MIN, &vid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_vlan_t  result_vid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (vid_index = 0; vid_index <= vid_last; vid_index++)
            {
                inter_result3 = vid_result[vid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstVid_set(unit, instance[instance_index], vid[vid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_set (unit %u, instance %u, vid %u)"
                                    , ret, expect_result, unit, instance[instance_index], vid[vid_index]);

                    ret = rtk_oam_cfmCcmInstVid_get(unit, instance[instance_index], (uint32*)&result_vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_get (unit %u, instance %u, vid %u)"
                                     , ret, expect_result, unit, instance[instance_index], vid[vid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_set/get value unequal (unit %u, instance %u, vid %u)"
                                     , result_vid, vid[vid_index], unit, instance[instance_index], vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstVid_set (unit %u, instance %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], vid[vid_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstVid_get(unit, instance[instance_index], &result_vid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstVid_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_vlan_t  result_vid;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_VLAN_ID_MAX, vid_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
            {
                ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstVid_set(unit, instance, vid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_set (unit %u, instance %u, vid %u)"
                                    , ret, expect_result, unit, instance, vid);
                    ret = rtk_oam_cfmCcmInstVid_get(unit, instance, &result_vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_get (unit %u, instance %u, vid %u)"
                                     , ret, expect_result, unit, instance, vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstVid_set/get value unequal (unit %u, instance %u, vid %u)"
                                     , result_vid, vid, unit, instance, vid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstVid_set (unit %u, instance %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, instance, vid);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstVid_test */

int32 dal_oam_cfmCcmInstMaid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  maid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstMaid_set(unit, TEST_TX_INSTANCE_MIN, TEST_MAID_ID_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstMaid_get(unit, TEST_TX_INSTANCE_MIN, &maid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32  maid[UNITTEST_MAX_TEST_VALUE];
        int32  maid_result[UNITTEST_MAX_TEST_VALUE];
        int32  maid_index;
        int32  maid_last;

        uint32  result_maid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAID_ID_MAX, TEST_MAID_ID_MIN, maid, maid_result, maid_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (maid_index = 0; maid_index <= maid_last; maid_index++)
            {
                inter_result3 = maid_result[maid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstMaid_set(unit, instance[instance_index], maid[maid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set (unit %u, instance %u, maid %u)"
                                    , ret, expect_result, unit, instance[instance_index], maid[maid_index]);

                    ret = rtk_oam_cfmCcmInstMaid_get(unit, instance[instance_index], (uint32*)&result_maid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_get (unit %u, instance %u, maid %u)"
                                     , ret, expect_result, unit, instance[instance_index], maid[maid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set/get value unequal (unit %u, instance %u, maid %u)"
                                     , result_maid, maid[maid_index], unit, instance[instance_index], maid[maid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set (unit %u, instance %u, maid %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], maid[maid_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstMaid_get(unit, instance[instance_index], &result_maid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstMaid_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  maid;
        uint32  maid_max;
        uint32  result_maid;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_MAID_ID_MAX, maid_max);

       for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_MAID_ID_MIN, maid); maid <= maid_max; maid++)
            {
                ASSIGN_EXPECT_RESULT(maid, TEST_MAID_ID_MIN, TEST_MAID_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstMaid_set(unit, instance, maid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set (unit %u, instance %u, maid %u)"
                                    , ret, expect_result, unit, instance, maid);
                    ret = rtk_oam_cfmCcmInstMaid_get(unit, instance, &result_maid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_get (unit %u, instance %u, maid %u)"
                                     , ret, expect_result, unit, instance, maid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set/get value unequal (unit %u, instance %u, maid %u)"
                                     , result_maid, maid, unit, instance, maid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstMaid_set (unit %u, instance %u, maid %u)"
                                        , ret, RT_ERR_OK, unit, instance, maid);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstMaid_test */

int32 dal_oam_cfmCcmInstTxStatus_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstTxStatus_set(unit, TEST_TX_INSTANCE_MIN, TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstTxStatus_get(unit, TEST_TX_INSTANCE_MIN, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

       UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstTxStatus_set(unit, instance[instance_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set (unit %u, instance %u, enable %u)"
                                    , ret, expect_result, unit, instance[instance_index], enable[enable_index]);

                    ret = rtk_oam_cfmCcmInstTxStatus_get(unit, instance[instance_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_get (unit %u, instance %u, enable %u)"
                                     , ret, expect_result, unit, instance[instance_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set/get value unequal (unit %u, instance %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, instance[instance_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set (unit %u, instance %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], enable[enable_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstTxStatus_get(unit, instance[instance_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  enable;
        uint32  enable_max;
        uint32  result_enable;


        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstTxStatus_set(unit, instance, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set (unit %u, instance %u, enable %u)"
                                    , ret, expect_result, unit, instance, enable);
                    ret = rtk_oam_cfmCcmInstTxStatus_get(unit, instance, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_get (unit %u, instance %u, enable %u)"
                                     , ret, expect_result, unit, instance, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set/get value unequal (unit %u, instance %u, enable %u)"
                                     , result_enable, enable, unit, instance, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstTxStatus_set (unit %u, instance %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, instance, enable);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstTxStatus_test */

int32 dal_oam_cfmCcmInstInterval_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  interval_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstInterval_set(unit, TEST_TX_INSTANCE_MIN, TEST_TX_INTERVAL_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstInterval_get(unit, TEST_TX_INSTANCE_MIN, &interval_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32  interval[UNITTEST_MAX_TEST_VALUE];
        int32  interval_result[UNITTEST_MAX_TEST_VALUE];
        int32  interval_index;
        int32  interval_last;

        uint32  result_interval;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INTERVAL_MAX, TEST_TX_INTERVAL_MIN, interval, interval_result, interval_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (interval_index = 0; interval_index <= interval_last; interval_index++)
            {
                inter_result3 = interval_result[interval_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstInterval_set(unit, instance[instance_index], interval[interval_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set (unit %u, instance %u, interval %u)"
                                    , ret, expect_result, unit, instance[instance_index], interval[interval_index]);

                    ret = rtk_oam_cfmCcmInstInterval_get(unit, instance[instance_index], (uint32*)&result_interval);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_get (unit %u, instance %u, interval %u)"
                                     , ret, expect_result, unit, instance[instance_index], interval[interval_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set/get value unequal (unit %u, instance %u, interval %u)"
                                     , result_interval, interval[interval_index], unit, instance[instance_index], interval[interval_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set (unit %u, instance %u, interval %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], interval[interval_index]);
                }
            }

            ret = rtk_oam_cfmCcmInstInterval_get(unit, instance[instance_index], &result_interval);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstInterval_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  interval;
        uint32  interval_max;
        uint32  result_interval;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_TX_INTERVAL_MAX, interval_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_TX_INTERVAL_MIN, interval); interval <= interval_max; interval++)
            {
                ASSIGN_EXPECT_RESULT(interval, TEST_TX_INTERVAL_MIN, TEST_TX_INTERVAL_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmInstInterval_set(unit, instance, interval);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set (unit %u, instance %u, interval %u)"
                                    , ret, expect_result, unit, instance, interval);
                    ret = rtk_oam_cfmCcmInstInterval_get(unit, instance, &result_interval);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_get (unit %u, instance %u, interval %u)"
                                     , ret, expect_result, unit, instance, interval);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set/get value unequal (unit %u, instance %u, interval %u)"
                                     , result_interval, interval, unit, instance, interval);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstInterval_set (unit %u, instance %u, interval %u)"
                                        , ret, RT_ERR_OK, unit, instance, interval);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstInterval_test */

int32 dal_oam_cfmCcmRxInstVid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  vid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmRxInstVid_set(unit, TEST_TX_INSTANCE_MIN, TEST_RESET_LIFETIME_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmRxInstVid_get(unit, TEST_TX_INSTANCE_MIN, &vid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_vlan_t  result_vid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (vid_index = 0; vid_index <= vid_last; vid_index++)
            {
                inter_result3 = vid_result[vid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmRxInstVid_set(unit, instance[instance_index], vid[vid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set (unit %u, instance %u, vid %u)"
                                    , ret, expect_result, unit, instance[instance_index], vid[vid_index]);

                    ret = rtk_oam_cfmCcmRxInstVid_get(unit, instance[instance_index], (uint32*)&result_vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_get (unit %u, instance %u, vid %u)"
                                     , ret, expect_result, unit, instance[instance_index], vid[vid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set/get value unequal (unit %u, instance %u, vid %u)"
                                     , result_vid, vid[vid_index], unit, instance[instance_index], vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set (unit %u, instance %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index], vid[vid_index]);
                }
            }

            ret = rtk_oam_cfmCcmRxInstVid_get(unit, instance[instance_index], &result_vid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_get (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_get (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_vlan_t  result_vid;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_VLAN_ID_MAX, vid_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
            {
                ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmCcmRxInstVid_set(unit, instance, vid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set (unit %u, instance %u, vid %u)"
                                    , ret, expect_result, unit, instance, vid);
                    ret = rtk_oam_cfmCcmRxInstVid_get(unit, instance, &result_vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_get (unit %u, instance %u, vid %u)"
                                     , ret, expect_result, unit, instance, vid);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set/get value unequal (unit %u, instance %u, vid %u)"
                                     , result_vid, vid, unit, instance, vid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstVid_set (unit %u, instance %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, instance, vid);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmRxInstVid_test */

int32 dal_oam_cfmCcmTxInstPort_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  port_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmTxInstPort_set(unit, TEST_TX_INSTANCE_MIN, TEST_CCM_PORT_IDX_MIN,TEST_ETHER_PORT_ID_MIN(unit)))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmTxInstPort_get(unit, TEST_TX_INSTANCE_MIN, TEST_CCM_PORT_IDX_MIN,&port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32 idx[UNITTEST_MAX_TEST_VALUE];
        int32  idx_result[UNITTEST_MAX_TEST_VALUE];
        int32  idx_index;
        int32  idx_last;

        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_t  result_port;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CCM_PORT_IDX_MAX, TEST_CCM_PORT_IDX_MIN, idx, idx_result, idx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

       for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (idx_index = 0; idx_index <= idx_last; idx_index++)
            {
                inter_result3 = idx_result[idx_index];
                for (port_index = 0; port_index <= port_last; port_index++)
                {
                    inter_result4 = port_result[port_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_oam_cfmCcmTxInstPort_set(unit, instance[instance_index], idx[idx_index], port[port_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                        , ret, expect_result, unit, instance[instance_index], idx[idx_index], port[port_index]);

                        ret = rtk_oam_cfmCcmTxInstPort_get(unit, instance[instance_index],  idx[idx_index], &result_port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_get (unit %u, instance %u, idx %u, port %u)"
                                         , ret, expect_result, unit, instance[instance_index],  idx[idx_index],port[port_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set/get value unequal (unit %u, instance %u, idx %u, port %u)"
                                         , result_port, port[port_index], unit, instance[instance_index],  idx[idx_index], port[port_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                            , ret, RT_ERR_OK, unit, instance[instance_index],  idx[idx_index], port[port_index]);
                    }
                }
            }
            ret = rtk_oam_cfmCcmTxInstPort_get(unit, instance[instance_index],   idx[idx_index], &result_port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_get (unit %u, instance %u, idx %u)"
                                , ret, expect_result, unit, instance[instance_index], idx[idx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_get (unit %u, instance %u, idx %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index], idx[idx_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  idx;
        uint32  idx_max;
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_port_t  result_port;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_CCM_PORT_IDX_MAX, idx_max);
        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);
            for (L_VALUE(TEST_CCM_PORT_IDX_MIN, idx); idx <= idx_max; idx++)
            {
                ASSIGN_EXPECT_RESULT(idx, TEST_CCM_PORT_IDX_MIN, TEST_CCM_PORT_IDX_MAX, inter_result3);
                for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
                {
                    ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result4);
                    UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_oam_cfmCcmTxInstPort_set(unit, instance, idx, port);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                        , ret, expect_result, unit, instance, idx, port);
                        ret = rtk_oam_cfmCcmTxInstPort_get(unit, instance, idx, &result_port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_get (unit %u, instance %u, idx %u, port %u)"
                                         , ret, expect_result, unit, instance, idx, port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set/get value unequal (unit %u, instance %u, idx %u, port %u)"
                                         , result_port, port, unit, instance, idx, port);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmTxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                            , ret, RT_ERR_OK, unit, instance, idx, port);
                }
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmTxInstPort_test */

int32 dal_oam_cfmCcmRxInstPort_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  port_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmRxInstPort_set(unit, TEST_TX_INSTANCE_MIN, TEST_CCM_PORT_IDX_MIN,TEST_ETHER_PORT_ID_MIN(unit)))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmRxInstPort_get(unit, TEST_TX_INSTANCE_MIN, TEST_CCM_PORT_IDX_MIN,&port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32 idx[UNITTEST_MAX_TEST_VALUE];
        int32  idx_result[UNITTEST_MAX_TEST_VALUE];
        int32  idx_index;
        int32  idx_last;

        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_t  result_port;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CCM_PORT_IDX_MAX, TEST_CCM_PORT_IDX_MIN, idx, idx_result, idx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (idx_index = 0; idx_index <= idx_last; idx_index++)
            {
                inter_result3 = idx_result[idx_index];
                for (port_index = 0; port_index <= port_last; port_index++)
                {
                    inter_result4 = port_result[port_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_oam_cfmCcmRxInstPort_set(unit, instance[instance_index], idx[idx_index], port[port_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                        , ret, expect_result, unit, instance[instance_index], idx[idx_index], port[port_index]);

                        ret = rtk_oam_cfmCcmRxInstPort_get(unit, instance[instance_index],  idx[idx_index], &result_port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_get (unit %u, instance %u, idx %u, port %u)"
                                         , ret, expect_result, unit, instance[instance_index],  idx[idx_index],port[port_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set/get value unequal (unit %u, instance %u, idx %u, port %u)"
                                         , result_port, port[port_index], unit, instance[instance_index],  idx[idx_index], port[port_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                            , ret, RT_ERR_OK, unit, instance[instance_index],  idx[idx_index], port[port_index]);
                    }
                }
            }
            ret = rtk_oam_cfmCcmRxInstPort_get(unit, instance[instance_index],   idx[idx_index], &result_port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_get (unit %u, instance %u, idx %u)"
                                , ret, expect_result, unit, instance[instance_index], idx[idx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_get (unit %u, instance %u, idx %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index], idx[idx_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  idx;
        uint32  idx_max;
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_port_t  result_port;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_CCM_PORT_IDX_MAX, idx_max);
        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);

        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);
            for (L_VALUE(TEST_CCM_PORT_IDX_MIN, idx); idx <= idx_max; idx++)
            {
                ASSIGN_EXPECT_RESULT(idx, TEST_CCM_PORT_IDX_MIN, TEST_CCM_PORT_IDX_MAX, inter_result3);
                for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
                {
                    ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result4);
                    UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_oam_cfmCcmRxInstPort_set(unit, instance, idx, port);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                        , ret, expect_result, unit, instance, idx, port);
                        ret = rtk_oam_cfmCcmRxInstPort_get(unit, instance, idx, &result_port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_get (unit %u, instance %u, idx %u, port %u)"
                                         , ret, expect_result, unit, instance, idx, port);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set/get value unequal (unit %u, instance %u, idx %u, port %u)"
                                         , result_port, port, unit, instance, idx, port);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmRxInstPort_set (unit %u, instance %u, idx %u, port %u)"
                                            , ret, RT_ERR_OK, unit, instance, idx, port);
                }
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmRxInstPort_test */

int32 dal_oam_cfmPortEthDmEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmPortEthDmEnable_set(unit, TEST_ETHER_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmPortEthDmEnable_get(unit, TEST_ETHER_PORT_ID_MIN(unit), &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
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
                ret = rtk_oam_cfmPortEthDmEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_oam_cfmPortEthDmEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_oam_cfmPortEthDmEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_get (unit %u, port %u)"
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

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_oam_cfmPortEthDmEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_oam_cfmPortEthDmEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmPortEthDmEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmPortEthDmEnable_test*/

int32 dal_oam_cfmCcmInstAliveTime_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  keepalive_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_cfmCcmInstAliveTime_get(unit, TEST_TX_INSTANCE_MIN, TEST_CCM_PORT_IDX_MIN,&keepalive_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;

        uint32 idx[UNITTEST_MAX_TEST_VALUE];
        int32  idx_result[UNITTEST_MAX_TEST_VALUE];
        int32  idx_index;
        int32  idx_last;

        uint32  result_keepslive;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TX_INSTANCE_MAX, TEST_TX_INSTANCE_MIN, instance, instance_result, instance_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CCM_PORT_IDX_MAX, TEST_CCM_PORT_IDX_MIN, idx, idx_result, idx_last);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            for (idx_index = 0; idx_index <= idx_last; idx_index++)
            {
                inter_result3 = idx_result[idx_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                   if (RT_ERR_OK == expect_result)
                   {
                    ret = rtk_oam_cfmCcmInstAliveTime_get(unit, instance[instance_index],  idx[idx_index], &result_keepslive);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get (unit %u, instance %u, idx %u, keepslive %u)"
                                        , ret, expect_result, unit, instance[instance_index],  idx[idx_index], result_keepslive);

                    ret = (TEST_KEEPALIVE_MAX >= result_keepslive)? RT_ERR_OK : RT_ERR_FAILED;
                       RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get value out of range (unit %u, instance %u, idx %u, keepslive %u)"
                                        , ret, RT_ERR_OK, unit, instance[instance_index],  idx[idx_index], result_keepslive);
                }
                    else
                   {
                    ret = rtk_oam_cfmCcmInstAliveTime_get(unit, instance[instance_index],  idx[idx_index], &result_keepslive);
                       RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get (unit %u, instance %u, idx %u, keepslive %u)"
                                           , ret, RT_ERR_OK, unit, instance[instance_index],  idx[idx_index], result_keepslive);
                   }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  idx;
        uint32  idx_max;

        uint32  result_keepslive;

        G_VALUE(TEST_TX_INSTANCE_MAX, instance_max);
        G_VALUE(TEST_CCM_PORT_IDX_MAX, idx_max);
        for (L_VALUE(TEST_TX_INSTANCE_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_TX_INSTANCE_MIN, TEST_TX_INSTANCE_MAX, inter_result2);
            for (L_VALUE(TEST_CCM_PORT_IDX_MIN, idx); idx <= idx_max; idx++)
            {
                ASSIGN_EXPECT_RESULT(idx, TEST_CCM_PORT_IDX_MIN, TEST_CCM_PORT_IDX_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                       expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                   if (expect_result == RT_ERR_OK)
                {
                    ret = rtk_oam_cfmCcmInstAliveTime_get(unit, instance,  idx, &result_keepslive);
                        RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get (unit %u, instance %u, idx %u, keepslive %u)"
                                        , ret, expect_result, unit, instance,  idx, result_keepslive);

                    ret = (TEST_KEEPALIVE_MAX >= result_keepslive)? RT_ERR_OK : RT_ERR_FAILED;
                       RT_TEST_IS_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get value out of range (unit %u, instance %u, idx %u, keepslive %u)"
                                        , ret, RT_ERR_OK, unit, instance,  idx, result_keepslive);


                   }
                   else
                   {
                    ret = rtk_oam_cfmCcmInstAliveTime_get(unit, instance,  idx, &result_keepslive);
                       RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_cfmCcmInstAliveTime_get (unit %u, instance %u, idx %u, keepslive %u)"
                                           , ret, RT_ERR_OK, unit, instance,  idx, result_keepslive);

                   }
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_cfmCcmInstAliveTime_test */

int32 dal_oam_dyingGaspSend_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_oam_dyingGaspSend_set(unit, TEST_ENABLE_STATUS_MIN)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            inter_result2 = enable_result[enable_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_dyingGaspSend_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_dyingGaspSend_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_dyingGaspSend_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_oam_dyingGaspSend_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_oam_dyingGaspSend_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_oam_dyingGaspSend_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);

        }
    }

    return RT_ERR_OK;
}   /*end of dal_oam_dyingGaspSend_test */

