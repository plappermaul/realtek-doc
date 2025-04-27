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
#include <rtk/rate.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */

#define TEST_MIRROR_ID_MAX                         3
#define TEST_MIRROR_ID_MIN                         0

#define TEST_MIRROR_RSPANIGR_MODE_MAX              RSPAN_IGR_HANDLE_RSPAN_TAG
#define TEST_MIRROR_RSPANIGR_MODE_MIN              RSPAN_IGR_IGNORE_RSPAN_TAG

#define TEST_MIRROR_RSPANEGR_MODE_MAX              RSPAN_EGR_NO_MODIFY
#define TEST_MIRROR_RSPANEGR_MODE_MIN              RSPAN_EGR_REMOVE_TAG

#define TEST_MIRROR_SFLOW_SMPRATE_MAX(unit)        HAL_SFLOW_RATE_MAX(unit)
#define TEST_MIRROR_SFLOW_SMPRATE_MIN              0

#define TEST_MIRROR_SFLOW_SMPCTRL_MAX              SFLOW_CTRL_EGRESS
#define TEST_MIRROR_SFLOW_SMPCTRL_MIN              SFLOW_CTRL_INGRESS

#define TEST_DOT1P_PRIORITY_MIN                    0
#define TEST_DOT1P_PRIORITY_MAX                    RTK_DOT1P_PRIORITY_MAX

#define RATE_RANDOM_RUN_TIMES                      10

int32 dal_mirror_group_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mirror_entry_t  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_group_set(unit, TEST_MIRROR_ID_MIN, &mirror_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_group_get(unit, TEST_MIRROR_ID_MIN, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32    id[UNITTEST_MAX_TEST_VALUE];
        int32  id_result[UNITTEST_MAX_TEST_VALUE];
        int32  id_index;
        int32  id_last;

        rtk_port_t    port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last = 0;

        int32  randIdx = 0;
        uint32 test_index = 0;

          rtk_mirror_entry_t  test_mirror;
        rtk_mirror_entry_t  result_mirror;
//        rtk_mirror_entry_t  failed_test_mirror;

        osal_memset(&result_mirror, 0, sizeof(rtk_mirror_entry_t));

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_ID_MAX, TEST_MIRROR_ID_MIN, id, id_result, id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (id_index = 0; id_index <= id_last; id_index++)
        {
            inter_result2 = id_result[id_index];
            /*The following loop is used to initital and clean all entries*/
            if((HWP_UNIT_VALID_LOCAL(unit)) && inter_result2)
            {
                for(test_index = 0; test_index <= TEST_MIRROR_ID_MAX; test_index++)
                {
                    osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));
                    ret = rtk_mirror_group_set(unit, test_index, &test_mirror);
                }
            }

            for (port_index = 0; port_index <= port_last; port_index++)
            {
                inter_result3 = port_result[port_index];
                for (enable_index = 0; enable_index <= enable_last; enable_index++)
                {
                    inter_result4 = enable_result[enable_index];
                    for (randIdx = 0; randIdx < 5*RATE_RANDOM_RUN_TIMES; randIdx++)
                       {
                        osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));

                        test_mirror.mirroring_port = port[port_index];
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        test_mirror.mirror_enable = enable[enable_index];
#endif
                        UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_mirror.mirrored_igrPorts);
                        UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_mirror.mirrored_egrPorts);
                        test_mirror.mirror_orginalPkt = ut_rand() & BITMASK_1B;
                        test_mirror.oper_of_igr_and_egr_ports = ut_rand() & BITMASK_1B;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        test_mirror.mirroring_port_egrMode = ut_rand() & BITMASK_1B;
#endif
                        UNITTEST_PORTMASK_PORT_REMOVE(port[port_index], test_mirror.mirrored_igrPorts);
                        UNITTEST_PORTMASK_PORT_REMOVE(port[port_index], test_mirror.mirrored_egrPorts);

                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                        /*Failed-case1:Add the Mirroring Port into Igr/Egr portmasks for ERROR test*/
                        if(((randIdx%5) == 0) && (randIdx != 0))
                        {
                                UNITTEST_PORTMASK_PORT_ADD(port[port_index], test_mirror.mirrored_igrPorts);
                                UNITTEST_PORTMASK_PORT_ADD(port[port_index], test_mirror.mirrored_egrPorts);
                                expect_result = RT_ERR_FAILED;
                        }
                        /*Failed-case2:Add the Mirroring Port into other enabled entry's Igr/Egr portmasks for ERROR test*/
/*This condition is removed from DAL*/
#if 0
                        if(((randIdx%11) == 0) && (RT_ERR_OK == expect_result) && (randIdx != 0)
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            && (ENABLED == test_mirror.mirror_enable)
#endif
                            )
                            {
                                osal_memset(&failed_test_mirror, 0, sizeof(rtk_mirror_entry_t));
                                osal_memcpy(&failed_test_mirror, &test_mirror, sizeof(rtk_mirror_entry_t));

                                UNITTEST_PORTMASK_PORT_ADD(port[port_index], failed_test_mirror.mirrored_igrPorts);
                                UNITTEST_PORTMASK_PORT_ADD(port[port_index], failed_test_mirror.mirrored_egrPorts);

                                if(port[port_index] >= TEST_PORT_ID_MAX(unit))
                                    failed_test_mirror.mirroring_port = TEST_PORT_ID_MIN(unit);
                                else
                                    UNITTEST_TEST_NEXT_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[port_index], failed_test_mirror.mirroring_port);
                                UNITTEST_PORTMASK_PORT_REMOVE(failed_test_mirror.mirroring_port, failed_test_mirror.mirrored_igrPorts);
                                UNITTEST_PORTMASK_PORT_REMOVE(failed_test_mirror.mirroring_port, failed_test_mirror.mirrored_egrPorts);

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                                failed_test_mirror.mirror_enable = ENABLED;
#endif
                                if(id[id_index] <= TEST_MIRROR_ID_MAX)
                                    test_index = (TEST_MIRROR_ID_MAX - id[id_index]);
                                else
                                    test_index = TEST_MIRROR_ID_MIN;

                                ret = rtk_mirror_group_set(unit, test_index, &failed_test_mirror);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                                RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, test_index, failed_test_mirror.mirroring_port, failed_test_mirror.mirror_enable);
#else
                                RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u)"
                                            , ret, RT_ERR_OK, unit, test_index, failed_test_mirror.mirroring_port);
#endif
                                expect_result = RT_ERR_FAILED;

                        }
#endif
                        ret = rtk_mirror_group_set(unit, id[id_index], &test_mirror);
                        if (RT_ERR_OK == expect_result)
                        {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u, enable %u)"
                                            , ret, expect_result, unit, id[id_index], test_mirror.mirroring_port, test_mirror.mirror_enable);
#else
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u)"
                                            , ret, expect_result, unit, id[id_index], test_mirror.mirroring_port);
#endif

                            osal_memset(&result_mirror, 0, sizeof(rtk_mirror_entry_t));
                            ret = rtk_mirror_group_get(unit, id[id_index], &result_mirror);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_get (unit %u, id %u, mirroring port %u, enable %u)"
                                             , ret, expect_result, unit, id[id_index], result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_get (unit %u, id %u, mirroring port %u)"
                                             , ret, expect_result, unit, id[id_index], result_mirror.mirroring_port);
#endif

                            ret = osal_memcmp(&test_mirror, &result_mirror, sizeof(rtk_mirror_entry_t));
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set/get value unequal (unit %u, id %u, mirroring port %u, enable %u)"
                                             , ret, expect_result, unit, id[id_index], result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set/get value unequal (unit %u, id %u, mirroring port %u)"
                                             , ret, expect_result, unit, id[id_index], result_mirror.mirroring_port);
#endif

                            /*Clear the test case entry*/
                            osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));
                            test_mirror.mirroring_port = TEST_PORT_ID_MIN(unit);
                            ret = rtk_mirror_group_set(unit, id[id_index], &test_mirror);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u, enable %u)"
                                        , ret, expect_result, unit, id[id_index], test_mirror.mirroring_port, test_mirror.mirror_enable);
#else
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u)"
                                        , ret, expect_result, unit, id[id_index], test_mirror.mirroring_port);
#endif
                        }
                        else
                        {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, id[id_index], test_mirror.mirroring_port, test_mirror.mirror_enable);
#else
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u)"
                                    , ret, RT_ERR_OK, unit, id[id_index], test_mirror.mirroring_port);
#endif
                            /*Clear the Failed-case2 entry*/
                            if(((randIdx%11) == 0) && (1 == inter_result4) && (1 == inter_result3)&& (1 == inter_result2)&& (randIdx != 0))
                            {
                                osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));
                                test_mirror.mirroring_port = TEST_PORT_ID_MIN(unit);
                                ret = rtk_mirror_group_set(unit, test_index, &test_mirror);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                                RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u, enable %u)"
                                                , ret, RT_ERR_OK, unit, test_index, test_mirror.mirroring_port, test_mirror.mirror_enable);
#else
                                RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, mirroring port %u)"
                                                , ret, RT_ERR_OK, unit, test_index, test_mirror.mirroring_port);
#endif
                            }
                        }
                    }
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if (RT_ERR_OK != expect_result)
                    {
                        ret = rtk_mirror_group_get(unit, id[id_index], NULL);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_get (unit %u, id %u, mirroring port %u, enable %u)"
                                , ret, RT_ERR_OK, unit, id[id_index], result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_get (unit %u, id %u, mirroring port %u)"
                                , ret, RT_ERR_OK, unit, id[id_index], result_mirror.mirroring_port);
#endif
                    }
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32              id;
        uint32              id_max;
        rtk_port_t          port = 0;
        rtk_port_t          port_max;
        rtk_enable_t        enable;
        rtk_enable_t        enable_max;
        rtk_mirror_entry_t  test_mirror;
        rtk_mirror_entry_t  result_mirror;
        uint32 randIdx = 0;
        uint32 test_index;

        G_VALUE(TEST_MIRROR_ID_MAX, id_max);
        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_MIRROR_ID_MIN, id); id <= id_max; id++)
        {
            ASSIGN_EXPECT_RESULT(id, TEST_MIRROR_ID_MIN, TEST_MIRROR_ID_MAX, inter_result2);
             /*The following loop is used to initital and clean all entries*/
            if((HWP_UNIT_VALID_LOCAL(unit)) && inter_result2)
            {
                for(test_index = 0; test_index <= TEST_MIRROR_ID_MAX; test_index++)
                {
                    osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));
                    ret = rtk_mirror_group_set(unit, test_index, &test_mirror);
                }
            }
            for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result3);
                UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);

                    osal_memset(&test_mirror, 0, sizeof(rtk_mirror_entry_t));
                    test_mirror.mirroring_port = port;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                    test_mirror.mirror_enable = enable;
#endif
                    UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_mirror.mirrored_igrPorts);
                    UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &test_mirror.mirrored_egrPorts);
                    test_mirror.mirror_orginalPkt = ut_rand() & BITMASK_1B;
                    test_mirror.oper_of_igr_and_egr_ports = ut_rand() & BITMASK_1B;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                    test_mirror.mirroring_port_egrMode = ut_rand() & BITMASK_1B;
#endif
                    UNITTEST_PORTMASK_PORT_REMOVE(port, test_mirror.mirrored_igrPorts);
                    UNITTEST_PORTMASK_PORT_REMOVE(port, test_mirror.mirrored_egrPorts);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    ret = rtk_mirror_group_set(unit, id, &test_mirror);
                    if (RT_ERR_OK == expect_result)
                    {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, port %u, enable %u)"
                                        , ret, expect_result, unit, id, test_mirror.mirroring_port, test_mirror.mirror_enable);
#else
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, port %u)"
                                        , ret, expect_result, unit, id, test_mirror.mirroring_port);
#endif

                        osal_memset(&result_mirror, 0, sizeof(rtk_mirror_entry_t));
                        ret = rtk_mirror_group_get(unit, id, &result_mirror);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_get (unit %u, port %u, port %u, enable %u)"
                                         , ret, expect_result, unit, port, result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_get (unit %u, port %u, port %u)"
                                         , ret, expect_result, unit, port, result_mirror.mirroring_port);
#endif

                        ret = osal_memcmp(&test_mirror, &result_mirror, sizeof(rtk_mirror_entry_t));
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set/get value unequal (unit %u, port %u, storm %u, enable %u)"
                                         , ret, RT_ERR_OK, unit, port, result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                        RT_TEST_IS_EQUAL_INT("rtk_mirror_group_set/get value unequal (unit %u, port %u, storm %u)"
                                         , ret, RT_ERR_OK, unit, port, result_mirror.mirroring_port);
#endif
                    }
                    else
                    {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, port %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port, result_mirror.mirroring_port, result_mirror.mirror_enable);
#else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_group_set (unit %u, id %u, port %u)"
                                            , ret, RT_ERR_OK, unit, port, result_mirror.mirroring_port);
#endif
                    }
                }
            }

        }
    }

    return RT_ERR_OK;
}/*dal_mirror_group_test*/

int32 dal_mirror_rspanIgrMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mirror_rspanIgrMode_t  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanIgrMode_set(unit, TEST_MIRROR_ID_MIN, TEST_MIRROR_RSPANIGR_MODE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanIgrMode_get(unit, TEST_MIRROR_ID_MIN, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32    id[UNITTEST_MAX_TEST_VALUE];
        int32  id_result[UNITTEST_MAX_TEST_VALUE];
        int32  id_index;
        int32  id_last;

        rtk_mirror_rspanIgrMode_t    mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_mirror_rspanIgrMode_t  result_mirror;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_ID_MAX, TEST_MIRROR_ID_MIN, id, id_result, id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_RSPANIGR_MODE_MAX, TEST_MIRROR_RSPANIGR_MODE_MIN, mode, mode_result, mode_last);

        for (id_index = 0; id_index <= id_last; id_index++)
        {
            inter_result2 = id_result[id_index];
            for (mode_index = 0; mode_index <= mode_last; mode_index++)
            {
                inter_result3 = mode_result[mode_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_rspanIgrMode_set(unit, id[id_index], mode[mode_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_set (unit %u, id %u, mode %u)"
                                    , ret, expect_result, unit, id[id_index], mode[mode_index]);


                    ret = rtk_mirror_rspanIgrMode_get(unit, id[id_index], &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_get (unit %u, id %u, mode %u)"
                                     , ret, expect_result, unit, id[id_index], result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_set/get value unequal (unit %u, id %u, SET mode %u, GET Mode %u)"
                                     , ret, expect_result, unit, id[id_index], mode[mode_index], result_mirror);
                }
                else
                {
                    //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanIgrMode_set (uunit %u, id %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, id[id_index], mode[mode_index]);
                }
            }
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (RT_ERR_OK != expect_result)
            {
                //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                ret = rtk_mirror_rspanIgrMode_get(unit, id[id_index], &result_mirror);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanIgrMode_get (unit %u, id %u, mode %u)"
                        , ret, RT_ERR_OK, unit, id[id_index], result_mirror);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32             id;
        uint32             id_max;
        rtk_mirror_rspanIgrMode_t        mode = 0;
        rtk_mirror_rspanIgrMode_t        mode_max;
        rtk_mirror_rspanIgrMode_t  result_mirror;

        G_VALUE(TEST_MIRROR_ID_MAX, id_max);
        G_VALUE(TEST_MIRROR_RSPANIGR_MODE_MAX, mode_max);

        for (L_VALUE(TEST_MIRROR_ID_MIN, id); id <= id_max; id++)
        {
            ASSIGN_EXPECT_RESULT(id, TEST_MIRROR_ID_MIN, TEST_MIRROR_ID_MAX, inter_result2);

            for (L_VALUE(TEST_MIRROR_RSPANIGR_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_MIRROR_RSPANIGR_MODE_MIN, TEST_MIRROR_RSPANIGR_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_rspanIgrMode_set(unit, id, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_set (unit %u, id %u, mode %u)"
                                    , ret, expect_result, unit, id, mode);

                    ret = rtk_mirror_rspanIgrMode_get(unit, id, &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_get (unit %u, id %u, mode %u)"
                                     , ret, expect_result, unit, id, result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanIgrMode_set/get value unequal (unit %u, id %u, mode %u)"
                                     , mode, result_mirror, unit, id, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanIgrMode_set (unit %u, id %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, id, mode);

                }

            }
        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_rspanIgrMode_test*/

int32 dal_mirror_rspanEgrMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mirror_rspanEgrMode_t  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanEgrMode_set(unit, TEST_MIRROR_ID_MIN, TEST_MIRROR_RSPANIGR_MODE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanEgrMode_get(unit, TEST_MIRROR_ID_MIN, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32    id[UNITTEST_MAX_TEST_VALUE];
        int32  id_result[UNITTEST_MAX_TEST_VALUE];
        int32  id_index;
        int32  id_last;

        rtk_mirror_rspanEgrMode_t    mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_mirror_rspanEgrMode_t  result_mirror;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_ID_MAX, TEST_MIRROR_ID_MIN, id, id_result, id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_RSPANEGR_MODE_MAX, TEST_MIRROR_RSPANEGR_MODE_MIN, mode, mode_result, mode_last);

        for (id_index = 0; id_index <= id_last; id_index++)
        {
            inter_result2 = id_result[id_index];
            for (mode_index = 0; mode_index <= mode_last; mode_index++)
            {
                inter_result3 = mode_result[mode_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_rspanEgrMode_set(unit, id[id_index], mode[mode_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_set (unit %u, id %u, mode %u)"
                                    , ret, expect_result, unit, id[id_index], mode[mode_index]);


                    ret = rtk_mirror_rspanEgrMode_get(unit, id[id_index], &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_get (unit %u, id %u, mode %u)"
                                     , ret, expect_result, unit, id[id_index], result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_set/get value unequal (unit %u, id %u, SET mode %u, GET Mode %u)"
                                     , ret, expect_result, unit, id[id_index], mode[mode_index], result_mirror);
                }
                else
                {
                    //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanEgrMode_set (uunit %u, id %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, id[id_index], mode[mode_index]);
                }
            }
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (RT_ERR_OK != expect_result)
            {
                //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                ret = rtk_mirror_rspanEgrMode_get(unit, id[id_index], &result_mirror);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanEgrMode_get (unit %u, id %u, mode %u)"
                        , ret, RT_ERR_OK, unit, id[id_index], result_mirror);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32             id;
        uint32             id_max;
        rtk_mirror_rspanEgrMode_t        mode = 0;
        rtk_mirror_rspanEgrMode_t        mode_max;

        rtk_mirror_rspanEgrMode_t  result_mirror;

        G_VALUE(TEST_MIRROR_ID_MAX, id_max);
        G_VALUE(TEST_MIRROR_RSPANEGR_MODE_MAX, mode_max);

        for (L_VALUE(TEST_MIRROR_ID_MIN, id); id <= id_max; id++)
        {
            ASSIGN_EXPECT_RESULT(id, TEST_MIRROR_ID_MIN, TEST_MIRROR_ID_MAX, inter_result2);

            for (L_VALUE(TEST_MIRROR_RSPANEGR_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_MIRROR_RSPANEGR_MODE_MIN, TEST_MIRROR_RSPANEGR_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_rspanEgrMode_set(unit, id, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_set (unit %u, id %u, mode %u)"
                                    , ret, expect_result, unit, id, mode);

                    ret = rtk_mirror_rspanEgrMode_get(unit, id, &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_get (unit %u, id %u, mode %u)"
                                     , ret, expect_result, unit, id, result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanEgrMode_set/get value unequal (unit %u, id %u, mode %u)"
                                     , mode, result_mirror, unit, id, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanEgrMode_set (unit %u, id %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, id, mode);

                }

            }
        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_rspanEgrMode_test*/

int32 dal_mirror_rspanTag_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mirror_rspanTag_t  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanTag_set(unit, TEST_MIRROR_ID_MIN, TEST_MIRROR_RSPANIGR_MODE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_rspanTag_get(unit, TEST_MIRROR_ID_MIN, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32    id[UNITTEST_MAX_TEST_VALUE];
        int32  id_result[UNITTEST_MAX_TEST_VALUE];
        int32  id_index;
        int32  id_last;

        rtk_vlan_t    vlan[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_result[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_index;
        int32  vlan_last;

        rtk_pri_t    pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;

        int32  randIdx;

        rtk_mirror_rspanTag_t  test_mirror;
        rtk_mirror_rspanTag_t  result_mirror;
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_ID_MAX, TEST_MIRROR_ID_MIN, id, id_result, id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vlan, vlan_result, vlan_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DOT1P_PRIORITY_MAX, TEST_DOT1P_PRIORITY_MIN, pri, pri_result, pri_last);

        for (id_index = 0; id_index <= id_last; id_index++)
        {
            inter_result2 = id_result[id_index];
            for (vlan_index = 0; vlan_index <= vlan_last; vlan_index++)
            {
                inter_result3 = vlan_result[vlan_index];
                for (pri_index = 0; pri_index <= pri_last; pri_index++)
                {
                    inter_result4 = pri_result[pri_index];
                    for (randIdx = 0; randIdx < 5*RATE_RANDOM_RUN_TIMES; randIdx++)
                       {
                        osal_memset(&test_mirror, 0, sizeof(rtk_mirror_rspanTag_t));

                        test_mirror.vid = vlan[vlan_index];
                        test_mirror.pri = pri[pri_index];
                        test_mirror.cfi = ut_rand() & BITMASK_1B;
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
                        if(HWP_8380_30_FAMILY(unit) || HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
                            test_mirror.tpid = ut_rand() & BITMASK_2B;
#endif
#if defined(CONFIG_SDK_RTL8390)
                        if(HWP_8390_50_FAMILY(unit))
                        {
                            test_mirror.tpidIdx = ut_rand() & BITMASK_2B;
                        }
#endif

                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                        ret = rtk_mirror_rspanTag_set(unit, id[id_index], &test_mirror);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_set (unit %u, id %u, vid %u, pri %u)"
                                            , ret, expect_result, unit, id[id_index], test_mirror.vid, test_mirror.pri);

                            osal_memset(&result_mirror, 0, sizeof(rtk_mirror_rspanTag_t));
                            ret = rtk_mirror_rspanTag_get(unit, id[id_index], &result_mirror);
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_get (unit %u, id %u, vid %u, pri %u)"
                                             , ret, expect_result, unit, id[id_index], result_mirror.vid, result_mirror.pri);

                            ret = osal_memcmp(&test_mirror, &result_mirror, sizeof(rtk_mirror_rspanTag_t));
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_set/get value unequal (unit %u, id %u, vid %u, pri %u)"
                                             , ret, RT_ERR_OK, unit, id[id_index], result_mirror.vid, result_mirror.pri);
                        }
                        else
                        {
                            //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanTag_set (unit %u, id %u, vid %u, pri %u)"
                                                , ret, RT_ERR_OK, unit, id[id_index], test_mirror.vid, test_mirror.pri);

                        }
                    }
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if (RT_ERR_OK != expect_result)
                    {
                        //osal_printf("\nr1 = %u, r2 = %u, r3 = %u, r4 = %u, randIdx = %u, enable_index = %u\n",inter_result1,inter_result2,inter_result3,inter_result4,randIdx,enable_index);
                        ret = rtk_mirror_rspanTag_get(unit, id[id_index], NULL);
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanTag_get (unit %u, id %u, vid %u, pri %u)"
                                , ret, RT_ERR_OK, unit, id[id_index], result_mirror.vid, result_mirror.pri);
                    }
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32          id;
        uint32          id_max;
        rtk_port_t      vlan = 0;
        rtk_port_t      vlan_max;
        rtk_enable_t    pri;
        rtk_enable_t    pri_max;

        int32  randIdx;

        rtk_mirror_rspanTag_t  test_mirror;
        rtk_mirror_rspanTag_t  result_mirror;

        G_VALUE(TEST_MIRROR_ID_MAX, id_max);
        G_VALUE(TEST_VLAN_ID_MAX, vlan_max);
        G_VALUE(TEST_DOT1P_PRIORITY_MAX, pri_max);

        for (L_VALUE(TEST_MIRROR_ID_MIN, id); id <= id_max; id++)
        {

            ASSIGN_EXPECT_RESULT(id, TEST_MIRROR_ID_MIN, TEST_MIRROR_ID_MAX, inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, vlan); vlan <= vlan_max; vlan++)
            {
                ASSIGN_EXPECT_RESULT(vlan, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);

                for (L_VALUE(TEST_DOT1P_PRIORITY_MIN, pri); pri <= pri_max; pri++)
                {
                    ASSIGN_EXPECT_RESULT(pri, TEST_DOT1P_PRIORITY_MIN, TEST_DOT1P_PRIORITY_MAX, inter_result4);
                    for (randIdx = 0; randIdx < 5*RATE_RANDOM_RUN_TIMES; randIdx++)
                       {
                        osal_memset(&test_mirror, 0, sizeof(rtk_mirror_rspanTag_t));

                        test_mirror.vid = vlan;
                        test_mirror.pri = pri;
                        test_mirror.cfi = ut_rand() & BITMASK_1B;
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
                        if(HWP_8380_30_FAMILY(unit) || HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
                            test_mirror.tpid = ut_rand() & BITMASK_2B;
#endif
#if defined(CONFIG_SDK_RTL8390)
                        if(HWP_8390_50_FAMILY(unit))
                        {
                            test_mirror.tpidIdx = ut_rand() & BITMASK_2B;
                        }
#endif
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                        ret = rtk_mirror_rspanTag_set(unit, id, &test_mirror);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_set (unit %u, id %u, vid %u, pri %u)"
                                            , ret, expect_result, unit, id, result_mirror.vid, result_mirror.pri);

                            osal_memset(&result_mirror, 0, sizeof(rtk_mirror_rspanTag_t));
                            ret = rtk_mirror_rspanTag_get(unit, id, &result_mirror);
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_get (unit %u, id %u, vid %u, pri %u)"
                                             , ret, expect_result, unit, id, result_mirror.vid, result_mirror.pri);

                            ret = osal_memcmp(&test_mirror, &result_mirror, sizeof(rtk_mirror_rspanTag_t));
                            RT_TEST_IS_EQUAL_INT("rtk_mirror_rspanTag_set/get value unequal (unit %u, id %u, vid %u, pri %u)"
                                             , ret, RT_ERR_OK, unit, id, result_mirror.vid, result_mirror.pri);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_rspanTag_set (unit %u, id %u, vid %u, pri %u)"
                                                , ret, RT_ERR_OK, unit, id, result_mirror.vid, result_mirror.pri);

                        }
                    }
                }
            }

        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_rspanTag_test*/

int32 dal_mirror_sflowMirrorSampleRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowMirrorSampleRate_set(unit, TEST_MIRROR_ID_MIN, TEST_MIRROR_SFLOW_SMPRATE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowMirrorSampleRate_get(unit, TEST_MIRROR_ID_MIN, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32    id[UNITTEST_MAX_TEST_VALUE];
        int32  id_result[UNITTEST_MAX_TEST_VALUE];
        int32  id_index;
        int32  id_last;

        uint32    rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;

        uint32  result_mirror;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_ID_MAX, TEST_MIRROR_ID_MIN, id, id_result, id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), TEST_MIRROR_SFLOW_SMPRATE_MIN, rate, rate_result, rate_last);

        for (id_index = 0; id_index <= id_last; id_index++)
        {
            inter_result2 = id_result[id_index];
            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowMirrorSampleRate_set(unit, id[id_index], rate[rate_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set (unit %u, id %u, rate %u)"
                                    , ret, expect_result, unit, id[id_index], rate[rate_index]);


                    ret = rtk_mirror_sflowMirrorSampleRate_get(unit, id[id_index], &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_get (unit %u, id %u, rate %u)"
                                     , ret, expect_result, unit, id[id_index], result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set/get value unequal (unit %u, id %u, rate %u)"
                                     , result_mirror, rate[rate_index], unit, id[id_index], rate[rate_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set (uunit %u, id %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, id[id_index], rate[rate_index]);
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK != expect_result)
                {
                    ret = rtk_mirror_sflowMirrorSampleRate_get(unit, id[id_index], NULL);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_get (unit %u, id %u, rate %u)"
                            , ret, RT_ERR_OK, unit, id[id_index], rate[rate_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32            id;
        uint32            id_max;
        uint32            rate = 0;
        uint32            rate_max;

        uint32  result_mirror;

        G_VALUE(TEST_MIRROR_ID_MAX, id_max);
        G_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), rate_max);

        for (L_VALUE(TEST_MIRROR_ID_MIN, id); id <= id_max; id++)
        {
            ASSIGN_EXPECT_RESULT(id, TEST_MIRROR_ID_MIN, TEST_MIRROR_ID_MAX, inter_result2);

            for (L_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MIN, rate); rate <= rate_max; rate++)
            {
                ASSIGN_EXPECT_RESULT(rate, TEST_MIRROR_SFLOW_SMPRATE_MIN, TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowMirrorSampleRate_set(unit, id, rate);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set (unit %u, id %u, mode %u)"
                                    , ret, expect_result, unit, id, rate);

                    ret = rtk_mirror_sflowMirrorSampleRate_get(unit, id, &result_mirror);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_get (unit %u, id %u, mode %u)"
                                     , ret, expect_result, unit, id, result_mirror);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set/get value unequal (unit %u, id %u, mode %u)"
                                     , rate, result_mirror, unit, id, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowMirrorSampleRate_set (unit %u, id %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, id, rate);

                }

            }
        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_sflowMirrorSampleRate_test*/

int32 dal_mirror_sflowPortIgrSampleRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  mirror_temp;
    int8    inter_result2 = 1;
    int8    inter_result3 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowPortIgrSampleRate_set(unit, TEST_PORT_ID_MIN(unit), TEST_MIRROR_SFLOW_SMPRATE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowPortIgrSampleRate_get(unit, TEST_PORT_ID_MIN(unit), &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t    port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32    rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;

        uint32  result_rate;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), TEST_MIRROR_SFLOW_SMPRATE_MIN, rate, rate_result, rate_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowPortIgrSampleRate_set(unit, port[port_index], rate[rate_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port[port_index], rate[rate_index]);


                    ret = rtk_mirror_sflowPortIgrSampleRate_get(unit, port[port_index], &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port[port_index], result_rate);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set/get value unequal (unit %u, port %u, rate %u)"
                                     , result_rate, rate[rate_index], unit, port[port_index], rate[rate_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set (uunit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK != expect_result)
                {
                    ret = rtk_mirror_sflowPortIgrSampleRate_get(unit, port[port_index], NULL);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_get (unit %u, port %u, rate %u)"
                            , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t        port;
        rtk_port_t        port_max;
        uint32            rate = 0;
        uint32            rate_max;

        uint32  result_rate;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MIN, rate); rate <= rate_max; rate++)
            {
                ASSIGN_EXPECT_RESULT(rate, TEST_MIRROR_SFLOW_SMPRATE_MIN, TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowPortIgrSampleRate_set(unit, port, rate);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port, rate);

                    ret = rtk_mirror_sflowPortIgrSampleRate_get(unit, port, &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port, result_rate);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set/get value unequal (unit %u, port %u, mode %u)"
                                     , rate, result_rate, unit, port, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port, rate);

                }

            }
        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_sflowPortIgrSampleRate_test*/

int32 dal_mirror_sflowPortEgrSampleRate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  mirror_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowPortEgrSampleRate_set(unit, TEST_PORT_ID_MIN(unit), TEST_MIRROR_SFLOW_SMPRATE_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowPortEgrSampleRate_get(unit, TEST_PORT_ID_MIN(unit), &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t    port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32    rate[UNITTEST_MAX_TEST_VALUE];
        int32  rate_result[UNITTEST_MAX_TEST_VALUE];
        int32  rate_index;
        int32  rate_last;

        uint32  result_rate;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), TEST_MIRROR_SFLOW_SMPRATE_MIN, rate, rate_result, rate_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (rate_index = 0; rate_index <= rate_last; rate_index++)
            {
                inter_result3 = rate_result[rate_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowPortEgrSampleRate_set(unit, port[port_index], rate[rate_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set (unit %u, port %u, rate %u)"
                                    , ret, expect_result, unit, port[port_index], rate[rate_index]);


                    ret = rtk_mirror_sflowPortEgrSampleRate_get(unit, port[port_index], &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_get (unit %u, port %u, rate %u)"
                                     , ret, expect_result, unit, port[port_index], result_rate);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set/get value unequal (unit %u, port %u, rate %u)"
                                     , result_rate, rate[rate_index], unit, port[port_index], rate[rate_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set (uunit %u, port %u, rate %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK != expect_result)
                {
                    ret = rtk_mirror_sflowPortEgrSampleRate_get(unit, port[port_index], NULL);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortIgrSampleRate_get (unit %u, port %u, rate %u)"
                            , ret, RT_ERR_OK, unit, port[port_index], rate[rate_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t         port;
        rtk_port_t         port_max;
        uint32            rate = 0;
        uint32            rate_max;
        uint32              result_rate;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), rate_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_MIRROR_SFLOW_SMPRATE_MIN, rate); rate <= rate_max; rate++)
            {
                ASSIGN_EXPECT_RESULT(rate, TEST_MIRROR_SFLOW_SMPRATE_MIN, TEST_MIRROR_SFLOW_SMPRATE_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowPortEgrSampleRate_set(unit, port, rate);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port, rate);

                    ret = rtk_mirror_sflowPortEgrSampleRate_get(unit, port, &result_rate);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port, result_rate);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set/get value unequal (unit %u, port %u, mode %u)"
                                     , rate, result_rate, unit, port, rate);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowPortEgrSampleRate_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port, rate);

                }

            }
        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_sflowPortEgrSampleRate_test*/

int32 dal_mirror_sflowSampleCtrl_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_sflowSampleCtrl_t  mirror_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowSampleCtrl_set(unit, TEST_MIRROR_SFLOW_SMPCTRL_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mirror_sflowSampleCtrl_get(unit, &mirror_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_sflowSampleCtrl_t    ctrl[UNITTEST_MAX_TEST_VALUE];
        int32  ctrl_result[UNITTEST_MAX_TEST_VALUE];
        int32  ctrl_index;
        int32  ctrl_last;

        rtk_sflowSampleCtrl_t  result_ctrl;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MIRROR_SFLOW_SMPCTRL_MAX, TEST_MIRROR_SFLOW_SMPCTRL_MIN, ctrl, ctrl_result, ctrl_last);

        for (ctrl_index = 0; ctrl_index <= ctrl_last; ctrl_index++)
        {
            inter_result2 = ctrl_result[ctrl_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_mirror_sflowSampleCtrl_set(unit, ctrl[ctrl_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set (unit %u, ctrl %u)"
                                , ret, expect_result, unit, ctrl[ctrl_index]);

                ret = rtk_mirror_sflowSampleCtrl_get(unit, &result_ctrl);
                RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_get (unit %u, ctrl %u)"
                                 , ret, expect_result, unit, result_ctrl);

                RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set/get value unequal (unit %u, ctrl %u)"
                                 , result_ctrl, ctrl[ctrl_index], unit, ctrl[ctrl_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set (unit %u, ctrl %u)"
                                    , ret, RT_ERR_OK, unit, ctrl[ctrl_index]);
            }
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (RT_ERR_OK != expect_result)
            {
                ret = rtk_mirror_sflowSampleCtrl_get(unit, NULL);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowSampleCtrl_get (unit %u, ctrl %u)"
                        , ret, RT_ERR_OK, unit, ctrl[ctrl_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_sflowSampleCtrl_t         ctrl;
        rtk_sflowSampleCtrl_t         ctrl_max;
        rtk_sflowSampleCtrl_t         result_ctrl;

        G_VALUE(TEST_MIRROR_SFLOW_SMPCTRL_MAX, ctrl_max);

        for (L_VALUE(TEST_MIRROR_SFLOW_SMPCTRL_MIN, ctrl); ctrl <= ctrl_max; ctrl++)
        {
            ASSIGN_EXPECT_RESULT(ctrl, TEST_MIRROR_SFLOW_SMPCTRL_MIN, TEST_MIRROR_SFLOW_SMPCTRL_MAX, inter_result2);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_mirror_sflowSampleCtrl_set(unit, ctrl);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set (unit %u, ctrl %u)"
                                    , ret, expect_result, unit, ctrl);

                    ret = rtk_mirror_sflowSampleCtrl_get(unit, &result_ctrl);
                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_get (unit %u, ctrl %u)"
                                     , ret, expect_result, unit, result_ctrl);

                    RT_TEST_IS_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set/get value unequal (unit %u, ctrl %u)"
                                     , ctrl, result_ctrl, unit, ctrl);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mirror_sflowSampleCtrl_set (unit %u, ctrl %u)"
                                        , ret, RT_ERR_OK, unit, ctrl);

                }


        }
    }

    return RT_ERR_OK;
}  /*dal_mirror_sflowSampleCtrl_test*/

