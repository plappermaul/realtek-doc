/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74489 $
 * $Date: 2016-12-21 11:48:26 +0800 (Wed, 21 Dec 2016) $
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
#include <rtk/time.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define TEST_RAND_IDX_MIN   0
#define TEST_RAND_IDX_MAX   50

/* PTP Port ID */
#define TEST_PTP_PORT_ID_MIN(unit)  (HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit))
#define TEST_PTP_PORT_ID_MAX(unit)  (HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit))

int32 dal_time_refTime_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  rand_index;
    rtk_time_timeStamp_t  timeStamp_temp;
    int8    inter_result2 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_time_refTime_get(unit, &timeStamp_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_time_refTime_set(unit, timeStamp_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_time_timeStamp_t  timeStamp;
        rtk_time_timeStamp_t  result_timeStamp;

        /* test case 1: ref time mechanism is disabled */
        rtk_time_refTimeEnable_set(unit, DISABLED);

        for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
        {
            ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            timeStamp.sec = ut_rand();
            timeStamp.nsec = ut_rand()%0x3b9aca00;/* 0x40000000 = 10^9 */
            ret = rtk_time_refTime_set(unit, timeStamp);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                , ret, expect_result, unit, (uint32)timeStamp.sec, timeStamp.nsec);

                ret = rtk_time_refTime_get(unit, &result_timeStamp);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u, time %u sec %u nsec)"
                                 , ret, expect_result, unit, (uint32)result_timeStamp.sec, result_timeStamp.nsec);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, timeStamp.sec %u)"
                                 , (uint32)result_timeStamp.sec, (uint32)timeStamp.sec, unit, (uint32)timeStamp.sec);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, timeStamp.nsec %u)"
                                 , (result_timeStamp.nsec>>3), (timeStamp.nsec>>3), unit, timeStamp.nsec);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                    , ret, RT_ERR_OK, unit, (uint32)timeStamp.sec, timeStamp.nsec);
            }
        }

        ret = rtk_time_refTime_get(unit, &result_timeStamp);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }


        /* test case 1: ref time mechanism is enabled */
        rtk_time_refTimeEnable_set(unit, ENABLED);

        for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
        {
            ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            timeStamp.sec = ut_rand();
            timeStamp.nsec = ut_rand()%0x3b9aca00;/* 0x3b9aca00 = 10^9 */
            ret = rtk_time_refTime_set(unit, timeStamp);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                , ret, expect_result, unit, (uint32)timeStamp.sec, timeStamp.nsec);

                ret = rtk_time_refTime_get(unit, &result_timeStamp);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u, time %u sec %u nsec)"
                                 , ret, expect_result, unit, (uint32)result_timeStamp.sec, result_timeStamp.nsec);
                if (!((result_timeStamp.sec > timeStamp.sec) ||
                    ((result_timeStamp.sec == timeStamp.sec) && (result_timeStamp.nsec > timeStamp.nsec))))
                {
                    osal_printf("ref time is not increased in rand_index=%d.\n\r", rand_index);
                    osal_printf("result_timeStamp.sec time is 0x%x, timeStamp.sec time is 0x%x.\n\r", (uint32)result_timeStamp.sec, (uint32)timeStamp.sec);
                    osal_printf("result_timeStamp.nsec time is 0x%x, timeStamp.nsec time is 0x%x.\n\r", result_timeStamp.nsec, timeStamp.nsec);
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                    , ret, RT_ERR_OK, unit, (uint32)timeStamp.sec, timeStamp.nsec);
            }
        }

        ret = rtk_time_refTime_get(unit, &result_timeStamp);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }


    return RT_ERR_OK;
}

int32 dal_time_refTimeAdjust_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  rand_index, sign;
    rtk_time_timeStamp_t  timeStamp_temp;
    int8    inter_result2 = 1;
    int32   expect_result = 1;

    timeStamp_temp.nsec = 0;
    timeStamp_temp.sec = 0;
    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_time_refTimeAdjust_set(unit, 0, timeStamp_temp))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_time_timeStamp_t  timeStamp, delta_timeStamp;
        rtk_time_timeStamp_t  result_timeStamp;

        for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
        {
            ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            timeStamp.sec = ut_rand();
            timeStamp.nsec = ut_rand()%0x40000000;
            ret = rtk_time_refTime_set(unit, timeStamp);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                , ret, expect_result, unit, (uint32)timeStamp.sec, timeStamp.nsec);

                ret = rtk_time_refTime_get(unit, &result_timeStamp);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u, time %u sec %u nsec)"
                                 , ret, expect_result, unit, (uint32)result_timeStamp.sec, result_timeStamp.nsec);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, timeStamp.sec %u)"
                                 , (uint32)result_timeStamp.sec, (uint32)timeStamp.sec, unit, (uint32)timeStamp.sec);
                delta_timeStamp.sec = 0;
                delta_timeStamp.nsec = 0x00000200;
                if (timeStamp.nsec >= 0x20000000)
                {
                    sign = 1;
                    ret = rtk_time_refTimeAdjust_set(unit, sign, delta_timeStamp);
                    RT_TEST_IS_EQUAL_INT("rtk_time_refTimeAdjust_set (unit %u, decrease %u sec %u nsec)"
                                 , ret, RT_ERR_OK, unit, (uint32)delta_timeStamp.sec, delta_timeStamp.nsec);
                }
                else
                {
                    sign = 0;
                    ret = rtk_time_refTimeAdjust_set(unit, sign, delta_timeStamp);
                    RT_TEST_IS_EQUAL_INT("rtk_time_refTimeAdjust_set (unit %u, increase %u sec %u nsec)"
                                 , ret, RT_ERR_OK, unit, (uint32)delta_timeStamp.sec, delta_timeStamp.nsec);
                }
                ret = rtk_time_refTime_get(unit, &result_timeStamp);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u, time %u sec %u nsec)"
                                 , ret, expect_result, unit, (uint32)result_timeStamp.sec, result_timeStamp.nsec);
                RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, timeStamp.sec %u)"
                                 , (uint32)result_timeStamp.sec, (uint32)timeStamp.sec, unit, (uint32)timeStamp.sec);
                if (sign == 1)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, decrease %u sec %u nsec)"
                                 , ((result_timeStamp.nsec+delta_timeStamp.nsec)>>3), (timeStamp.nsec>>3), unit, (uint32)delta_timeStamp.sec, delta_timeStamp.nsec);
                }
                else
                {
                    RT_TEST_IS_EQUAL_INT("rtk_time_refTime_set/get value unequal (unit %u, increase %u sec %u nsec)"
                                 , ((result_timeStamp.nsec-delta_timeStamp.nsec)>>3), (timeStamp.nsec>>3), unit, (uint32)delta_timeStamp.sec, delta_timeStamp.nsec);
                 }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_set (unit %u, time %u sec %u nsec)"
                                    , ret, RT_ERR_OK, unit, (uint32)timeStamp.sec, timeStamp.nsec);
            }
        }

        ret = rtk_time_refTime_get(unit, &result_timeStamp);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_time_refTime_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

/* The test case is for 8390 port 24 & 36 and RTL8218B series PHY */
int32 dal_time_portPtpEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    hal_control_t   *pHalCtrl;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_time_portPtpEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_time_portPtpEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PTP_PORT_ID_MAX(unit), TEST_PTP_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PTP_PORT_ID_MAX(unit), TEST_PTP_PORT_ID_MIN(unit), unit, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
            return RT_ERR_FAILED;

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port[port_index], inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                //if (port[port_index] != 24 && port[port_index] != 36)
                //    expect_result = RT_ERR_FAILED;
                ret = rtk_time_portPtpEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_time_portPtpEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    if ((pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218B) ||
                        (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB) ||
                        (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB_MP) ||
                        (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC) ||
                        (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC_MP) ||
                        (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8380_INT_GE))
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_time_portPtpEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_time_portPtpEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            //if (port[port_index] != 24 && port[port_index] != 36)
            //    expect_result = RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                if ((pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218B) ||
                    (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB) ||
                    (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB_MP) ||
                    (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC) ||
                    (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC_MP) ||
                    (pHalCtrl->pPhy_ctrl[port[port_index]]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8380_INT_GE))
                {
                    RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u)"
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

        G_VALUE(TEST_PTP_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PTP_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PTP_PORT_ID_MIN(unit), TEST_PTP_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                //if (port != 24 && port != 36)
                //    expect_result = RT_ERR_FAILED;
                ret = rtk_time_portPtpEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_time_portPtpEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    if ((pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218B) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB_MP) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC_MP) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8380_INT_GE))
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_time_portPtpEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_time_portPtpEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                //if (port != 24 && port != 36)
                //    expect_result = RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    if ((pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218B) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8218FB_MP) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8214FC_MP) ||
                        (pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index == RT_PHYDRV_RTL8380_INT_GE))
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_time_portPtpEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }

    return RT_ERR_OK;
}
