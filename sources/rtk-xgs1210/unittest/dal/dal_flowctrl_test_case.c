/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74086 $
 * $Date: 2016-12-07 15:28:30 +0800 (Wed, 07 Dec 2016) $
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
#include <rtk/flowctrl.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>


/* Define symbol used for test input */

int32 dal_flowctrl_portPauseOnAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portPauseOnAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portPauseOnAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_flowctrl_pauseOnAction_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        int32  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN((PAUSE_ON_END - 1), 0, action, action_result, action_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && action_result[action_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_portPauseOnAction_set(unit, port[port_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_flowctrl_portPauseOnAction_get(unit, port[port_index], (uint32*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portPauseOnAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_flowctrl_portPauseOnAction_get(unit, port[port_index], (uint32*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAction_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portPauseOnAction_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }

    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_portPauseOnAllowedPageNum_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pageNum_temp;
    int32  expect_result = 1;
    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portPauseOnAllowedPageNum_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portPauseOnAllowedPageNum_get(unit, 0, &pageNum_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  pageNum[UNITTEST_MAX_TEST_VALUE];
        int32  pageNum_result[UNITTEST_MAX_TEST_VALUE];
        int32  pageNum_index;
        int32  pageNum_last;

        int32  result_pageNum;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit), 0, pageNum, pageNum_result, pageNum_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (pageNum_index = 0; pageNum_index <= pageNum_last; pageNum_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && pageNum_result[pageNum_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_portPauseOnAllowedPageNum_set(unit, port[port_index], pageNum[pageNum_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_set (unit %u, port %u, pageNum %u)"
                                    , ret, expect_result, unit, port[port_index], pageNum[pageNum_index]);

                    ret = rtk_flowctrl_portPauseOnAllowedPageNum_get(unit, port[port_index], (uint32*)&result_pageNum);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_get (unit %u, port %u, pageNum %u)"
                                     , ret, expect_result, unit, port[port_index], pageNum[pageNum_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_set/get value unequal (unit %u, port %u, pageNum %u)"
                                     , result_pageNum, pageNum[pageNum_index], unit, port[port_index], pageNum[pageNum_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_set (unit %u, port %u, pageNum %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pageNum[pageNum_index]);
                }
            }

            ret = rtk_flowctrl_portPauseOnAllowedPageNum_get(unit, port[port_index], (uint32*)&result_pageNum);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portPauseOnAllowedPageNum_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }

    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_pauseOnAllowedPktNum_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pktNum_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_pauseOnAllowedPktNum_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_pauseOnAllowedPktNum_get(unit, 0, &pktNum_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  pktNum[UNITTEST_MAX_TEST_VALUE];
        int32  pktNum_result[UNITTEST_MAX_TEST_VALUE];
        int32  pktNum_index;
        int32  pktNum_last;

        int32  result_pktNum;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit), 0, pktNum, pktNum_result, pktNum_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (pktNum_index = 0; pktNum_index <= pktNum_last; pktNum_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && pktNum_result[pktNum_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_pauseOnAllowedPktNum_set(unit, port[port_index], pktNum[pktNum_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_set (unit %u, port %u, pktNum %u)"
                                    , ret, expect_result, unit, port[port_index], pktNum[pktNum_index]);

                    ret = rtk_flowctrl_pauseOnAllowedPktNum_get(unit, port[port_index], (uint32*)&result_pktNum);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_get (unit %u, port %u, pktNum %u)"
                                     , ret, expect_result, unit, port[port_index], pktNum[pktNum_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_set/get value unequal (unit %u, port %u, pktNum %u)"
                                     , result_pktNum, pktNum[pktNum_index], unit, port[port_index], pktNum[pktNum_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_set (unit %u, port %u, pktNum %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pktNum[pktNum_index]);
                }
            }

            ret = rtk_flowctrl_pauseOnAllowedPktNum_get(unit, port[port_index], (uint32*)&result_pktNum);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktNum_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }

    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_igrSystemPauseThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_thresh_t  thresh, thresh_result;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrSystemPauseThresh_set(unit, &thresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrSystemPauseThresh_get(unit, &thresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  highOn[UNITTEST_MAX_TEST_VALUE];
        int32  highOn_result[UNITTEST_MAX_TEST_VALUE];
        int32  highOn_index;
        int32  highOn_last;

        uint32  highOff[UNITTEST_MAX_TEST_VALUE];
        int32  highOff_result[UNITTEST_MAX_TEST_VALUE];
        int32  highOff_index;
        int32  highOff_last;

        uint32  lowOn[UNITTEST_MAX_TEST_VALUE];
        int32  lowOn_result[UNITTEST_MAX_TEST_VALUE];
        int32  lowOn_index;
        int32  lowOn_last;

        uint32  lowOff[UNITTEST_MAX_TEST_VALUE];
        int32  lowOff_result[UNITTEST_MAX_TEST_VALUE];
        int32  lowOff_index;
        int32  lowOff_last;


        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, highOn, highOn_result, highOn_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, highOff, highOff_result, highOff_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, lowOn, lowOn_result, lowOn_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, lowOff, lowOff_result, lowOff_last);

        for (highOn_index = 0; highOn_index <= highOn_last; highOn_index++)
        {
            for (highOff_index = 0; highOff_index <= highOff_last; highOff_index++)
            {
                for (lowOn_index = 0; lowOn_index <= lowOn_last; lowOn_index++)
                {
                    for (lowOff_index = 0; lowOff_index <= lowOff_last; lowOff_index++)
                    {
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (highOn_result[highOn_index] && highOff_result[highOff_index] && lowOn_result[lowOn_index] && lowOff_result[lowOff_index])? RT_ERR_OK : RT_ERR_FAILED;
                        thresh.highOn = highOn[highOn_index];
                        thresh.highOff = highOff[highOff_index];
                        thresh.lowOn = lowOn[lowOn_index];
                        thresh.lowOff = lowOff[lowOff_index];
                        ret = rtk_flowctrl_igrSystemPauseThresh_set(unit, &thresh);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                            , ret, expect_result, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);

                            ret = rtk_flowctrl_igrSystemPauseThresh_get(unit, &thresh_result);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_get (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                             , ret, expect_result, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set/get value unequal (unit %u, highOn %u)"
                                             , thresh_result.highOn, highOn[highOn_index], unit, highOn[highOn_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set/get value unequal (unit %u, highOff %u)"
                                             , thresh_result.highOff, highOff[highOff_index], unit, highOff[highOff_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set/get value unequal (unit %u, lowOn %u)"
                                             , thresh_result.lowOn, lowOn[lowOn_index], unit, lowOn[lowOn_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set/get value unequal (unit %u, lowOff %u)"
                                             , thresh_result.lowOff, lowOff[lowOff_index], unit, lowOff[lowOff_index]);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_set (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                                , ret, RT_ERR_OK, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);
                        }
                    }
                }
            }
        }

        ret = rtk_flowctrl_igrSystemPauseThresh_get(unit, &thresh_result);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrSystemPauseThresh_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }


    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_igrSystemCongestThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_thresh_t  thresh, thresh_result;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrSystemCongestThresh_set(unit, &thresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrSystemCongestThresh_get(unit, &thresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  highOn[UNITTEST_MAX_TEST_VALUE];
        int32  highOn_result[UNITTEST_MAX_TEST_VALUE];
        int32  highOn_index;
        int32  highOn_last;

        uint32  highOff[UNITTEST_MAX_TEST_VALUE];
        int32  highOff_result[UNITTEST_MAX_TEST_VALUE];
        int32  highOff_index;
        int32  highOff_last;

        uint32  lowOn[UNITTEST_MAX_TEST_VALUE];
        int32  lowOn_result[UNITTEST_MAX_TEST_VALUE];
        int32  lowOn_index;
        int32  lowOn_last;

        uint32  lowOff[UNITTEST_MAX_TEST_VALUE];
        int32  lowOff_result[UNITTEST_MAX_TEST_VALUE];
        int32  lowOff_index;
        int32  lowOff_last;


        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, highOn, highOn_result, highOn_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, highOff, highOff_result, highOff_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, lowOn, lowOn_result, lowOn_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, lowOff, lowOff_result, lowOff_last);


        for (highOn_index = 0; highOn_index <= highOn_last; highOn_index++)
        {
            for (highOff_index = 0; highOff_index <= highOff_last; highOff_index++)
            {
                for (lowOn_index = 0; lowOn_index <= lowOn_last; lowOn_index++)
                {
                    for (lowOff_index = 0; lowOff_index <= lowOff_last; lowOff_index++)
                    {
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (highOn_result[highOn_index] && highOff_result[highOff_index] && lowOn_result[lowOn_index] && lowOff_result[lowOff_index])? RT_ERR_OK : RT_ERR_FAILED;
                        thresh.highOn = highOn[highOn_index];
                        thresh.highOff = highOff[highOff_index];
                        thresh.lowOn = lowOn[lowOn_index];
                        thresh.lowOff = lowOff[lowOff_index];
                        ret = rtk_flowctrl_igrSystemCongestThresh_set(unit, &thresh);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                            , ret, expect_result, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);

                            ret = rtk_flowctrl_igrSystemCongestThresh_get(unit, &thresh_result);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_get (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                             , ret, expect_result, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set/get value unequal (unit %u, highOn %u)"
                                             , thresh_result.highOn, highOn[highOn_index], unit, highOn[highOn_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set/get value unequal (unit %u, highOff %u)"
                                             , thresh_result.highOff, highOff[highOff_index], unit, highOff[highOff_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set/get value unequal (unit %u, lowOn %u)"
                                             , thresh_result.lowOn, lowOn[lowOn_index], unit, lowOn[lowOn_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set/get value unequal (unit %u, lowOff %u)"
                                             , thresh_result.lowOff, lowOff[lowOff_index], unit, lowOff[lowOff_index]);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_set (unit %u, highOn %u, highOff %u, lowOn %u, lowOff %u)"
                                                , ret, RT_ERR_OK, unit, highOn[highOn_index], highOff[highOff_index], lowOn[lowOn_index], lowOff[lowOff_index]);
                        }
                    }
                }
            }
        }

        ret = rtk_flowctrl_igrSystemCongestThresh_get(unit, &thresh_result);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrSystemCongestThresh_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_egrSystemDropThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_drop_thresh_t  thresh, thresh_result;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrSystemDropThresh_set(unit, &thresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrSystemDropThresh_get(unit, &thresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  high[UNITTEST_MAX_TEST_VALUE];
        int32  high_result[UNITTEST_MAX_TEST_VALUE];
        int32  high_index;
        int32  high_last;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, high, high_result, high_last);

        for (high_index = 0; high_index <= high_last; high_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (high_result[high_index])? RT_ERR_OK : RT_ERR_FAILED;
            thresh.high = high[high_index];
            thresh.low = high[high_index];
            ret = rtk_flowctrl_egrSystemDropThresh_set(unit, &thresh);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_set (unit %u, high %u)"
                                , ret, expect_result, unit, high[high_index]);
                ret = rtk_flowctrl_egrSystemDropThresh_get(unit, &thresh_result);
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_get (unit %u, high %u)"
                                 , ret, expect_result, unit, high[high_index]);
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_set/get value unequal (unit %u, high %u)"
                                 , thresh_result.high, high[high_index], unit, high[high_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_set (unit %u, high %u)"
                                    , ret, RT_ERR_OK, unit, high[high_index]);
            }
        }

        ret = rtk_flowctrl_egrSystemDropThresh_get(unit, &thresh_result);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrSystemDropThresh_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }


    return RT_ERR_OK;
}

int32 dal_flowctrl_egrPortDropThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_drop_thresh_t  thresh, thresh_result;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrPortDropThresh_set(unit, 0, &thresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrPortDropThresh_get(unit, 0, &thresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  high[UNITTEST_MAX_TEST_VALUE];
        int32  high_result[UNITTEST_MAX_TEST_VALUE];
        int32  high_index;
        int32  high_last;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, high, high_result, high_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (high_index = 0; high_index <= high_last; high_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && high_result[high_index])? RT_ERR_OK : RT_ERR_FAILED;
                thresh.high = high[high_index];
                thresh.low = high[high_index];
                ret = rtk_flowctrl_egrPortDropThresh_set(unit, port[port_index], &thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_set (unit %u, port %u, high %u)"
                                    , ret, expect_result, unit, port[port_index], high[high_index]);

                    ret = rtk_flowctrl_egrPortDropThresh_get(unit, port[port_index], &thresh_result);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_get (unit %u, port %u, high %u)"
                                     , ret, expect_result, unit, port[port_index], high[high_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_set/get value unequal (unit %u, port %u, high %u)"
                                     , thresh_result.high, high[high_index], unit, port[port_index], high[high_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_set (unit %u, port %u, high %u, low %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], high[high_index], high[high_index]);
                }
            }

            ret = rtk_flowctrl_egrPortDropThresh_get(unit, port[port_index], &thresh_result);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrPortDropThresh_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_portEgrDropRefCongestEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portEgrDropRefCongestEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portEgrDropRefCongestEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(ENABLED, 0, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_flowctrl_portEgrDropRefCongestEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_flowctrl_portEgrDropRefCongestEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_flowctrl_portEgrDropRefCongestEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portEgrDropRefCongestEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
}

int32 dal_flowctrl_pauseOnAllowedPktLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pktLen_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_pauseOnAllowedPktLen_set(unit, TEST_PORT_ID_MIN(0), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_pauseOnAllowedPktLen_get(unit, TEST_PORT_ID_MIN(0), &pktLen_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  pktLen[UNITTEST_MAX_TEST_VALUE];
        int32  pktLen_result[UNITTEST_MAX_TEST_VALUE];
        int32  pktLen_index;
        int32  pktLen_last;

        int32  result_pktLen;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_LEN_MAX(unit), 0, pktLen, pktLen_result, pktLen_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (pktLen_index = 0; pktLen_index <= pktLen_last; pktLen_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && pktLen_result[pktLen_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_pauseOnAllowedPktLen_set(unit, port[port_index], pktLen[pktLen_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_set (unit %u, port %u, pktLen %u)"
                                    , ret, expect_result, unit, port[port_index], pktLen[pktLen_index]);

                    ret = rtk_flowctrl_pauseOnAllowedPktLen_get(unit, port[port_index], (uint32*)&result_pktLen);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_get (unit %u, port %u, pktLen %u)"
                                     , ret, expect_result, unit, port[port_index], pktLen[pktLen_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_set/get value unequal (unit %u, port %u, pktLen %u)"
                                     , result_pktLen, pktLen[pktLen_index], unit, port[port_index], pktLen[pktLen_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_set (unit %u, port %u, pktLen %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pktLen[pktLen_index]);
                }
            }

            ret = rtk_flowctrl_pauseOnAllowedPktLen_get(unit, port[port_index], (uint32*)&result_pktLen);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_pauseOnAllowedPktLen_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_pauseOnAllowedPktLen_test */

int32 dal_flowctrl_igrPauseThreshGroup_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_thresh_t  thresh_temp;
    int32  expect_result = 1;

    osal_memset(&thresh_temp, 0, sizeof(rtk_flowctrl_thresh_t));

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrPauseThreshGroup_set(unit, TEST_PORT_ID_MIN(0), &thresh_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrPauseThreshGroup_get(unit, TEST_PORT_ID_MIN(0), &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  group[UNITTEST_MAX_TEST_VALUE];
        int32  group_result[UNITTEST_MAX_TEST_VALUE];
        int32  group_index;
        int32  group_last;

        uint32  threshSize[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_result[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_index;
        int32  threshSize_last;

        rtk_flowctrl_thresh_t  test_thresh;
        rtk_flowctrl_thresh_t  result_thresh;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit), 0, group, group_result, group_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, threshSize, threshSize_result, threshSize_last);

        for (group_index = 0; group_index <= group_last; group_index++)
        {
            for (threshSize_index = 0; threshSize_index <= threshSize_last; threshSize_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (group_result[group_index] && threshSize_result[threshSize_index])? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&test_thresh, 0, sizeof(rtk_flowctrl_thresh_t));
                test_thresh.highOff = threshSize[threshSize_index];
                test_thresh.highOn = threshSize[threshSize_index];
                test_thresh.lowOn = threshSize[threshSize_index];
                //test_thresh.lowOff = threshSize[threshSize_index];

                ret = rtk_flowctrl_igrPauseThreshGroup_set(unit, group[group_index], &test_thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_set (unit %u, group %u, threshSize %u)"
                                    , ret, expect_result, unit, group[group_index], threshSize[threshSize_index]);

                    osal_memset(&result_thresh, 0, sizeof(rtk_flowctrl_thresh_t));
                    ret = rtk_flowctrl_igrPauseThreshGroup_get(unit, group[group_index], &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_get (unit %u, group %u, threshSize %u)"
                                     , ret, expect_result, unit, group[group_index], threshSize[threshSize_index]);

                    test_thresh.lowOff = 0;    /*This structure member is no used NOW*/
                    ret = osal_memcmp(&result_thresh, &test_thresh, sizeof(rtk_flowctrl_thresh_t));

                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_set/get value unequal (unit %u, group %u, threshSize %u)"
                                     , ret, RT_ERR_OK, unit, group[group_index], threshSize[threshSize_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_set (unit %u, group %u, threshSize %u)"
                                        , ret, RT_ERR_OK, unit, group[group_index], threshSize[threshSize_index]);
                }
            }

            ret = rtk_flowctrl_igrPauseThreshGroup_get(unit, group[group_index], &result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (group_result[group_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_get (unit %u, group %u)"
                                , ret, expect_result, unit, group[group_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrPauseThreshGroup_get (unit %u, group %u)"
                                    , ret, RT_ERR_OK, unit, group[group_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_igrPauseThreshGroup_test */

int32 dal_flowctrl_portIgrPortThreshGroupSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pktLen_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portIgrPortThreshGroupSel_set(unit, TEST_PORT_ID_MIN(0), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_portIgrPortThreshGroupSel_get(unit, TEST_PORT_ID_MIN(0), &pktLen_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  groupID[UNITTEST_MAX_TEST_VALUE];
        int32  groupID_result[UNITTEST_MAX_TEST_VALUE];
        int32  groupID_index;
        int32  groupID_last;

        uint32  result_groupID;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit), 0, groupID, groupID_result, groupID_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (groupID_index = 0; groupID_index <= groupID_last; groupID_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && groupID_result[groupID_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_portIgrPortThreshGroupSel_set(unit, port[port_index], groupID[groupID_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portIgrPortThreshGroupSel_set (unit %u, port %u, groupID %u)"
                                    , ret, expect_result, unit, port[port_index], groupID[groupID_index]);

                    ret = rtk_flowctrl_portIgrPortThreshGroupSel_get(unit, port[port_index], (uint32*)&result_groupID);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portIgrPortThreshGroupSel_get (unit %u, port %u, groupID %u)"
                                     , ret, expect_result, unit, port[port_index], groupID[groupID_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portIgrPortThreshGroupSel_set/get value unequal (unit %u, port %u, groupID %u)"
                                     , result_groupID, groupID[groupID_index], unit, port[port_index], groupID[groupID_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_portIgrPortThreshGroupSel_set (unit %u, port %u, groupID %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], groupID[groupID_index]);
                }
            }

            ret = rtk_flowctrl_portIgrPortThreshGroupSel_get(unit, port[port_index], (uint32*)&result_groupID);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_portIgrPortThreshGroupSel_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_portIgrPortThreshGroupSel_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_portIgrPortThreshGroupSel_test */

int32 dal_flowctrl_igrCongestThreshGroup_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_thresh_t  thresh_temp;
    int32  expect_result = 1;

    osal_memset(&thresh_temp, 0, sizeof(rtk_flowctrl_thresh_t));

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrCongestThreshGroup_set(unit, TEST_PORT_ID_MIN(unit), &thresh_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_igrCongestThreshGroup_get(unit, TEST_PORT_ID_MIN(unit), &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  group[UNITTEST_MAX_TEST_VALUE];
        int32  group_result[UNITTEST_MAX_TEST_VALUE];
        int32  group_index;
        int32  group_last;

        uint32  threshSize[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_result[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_index;
        int32  threshSize_last;

        rtk_flowctrl_thresh_t  test_thresh;
        rtk_flowctrl_thresh_t  result_thresh;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit), 0, group, group_result, group_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, threshSize, threshSize_result, threshSize_last);

        for (group_index = 0; group_index <= group_last; group_index++)
        {
            for (threshSize_index = 0; threshSize_index <= threshSize_last; threshSize_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (group_result[group_index] && threshSize_result[threshSize_index])? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&test_thresh, 0, sizeof(rtk_flowctrl_thresh_t));
                test_thresh.highOff = threshSize[threshSize_index];
                test_thresh.highOn = threshSize[threshSize_index];
                test_thresh.lowOn = threshSize[threshSize_index];
                test_thresh.lowOff = threshSize[threshSize_index];

                ret = rtk_flowctrl_igrCongestThreshGroup_set(unit, group[group_index], &test_thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_set (unit %u, group %u, threshSize %u)"
                                    , ret, expect_result, unit, group[group_index], threshSize[threshSize_index]);

                    osal_memset(&result_thresh, 0, sizeof(rtk_flowctrl_thresh_t));
                    ret = rtk_flowctrl_igrCongestThreshGroup_get(unit, group[group_index], &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_get (unit %u, group %u, threshSize %u)"
                                     , ret, expect_result, unit, group[group_index], threshSize[threshSize_index]);

                    ret = osal_memcmp(&result_thresh, &test_thresh, sizeof(rtk_flowctrl_thresh_t));

                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_set/get value unequal (unit %u, group %u, threshSize %u)"
                                     , ret, RT_ERR_OK, unit, group[group_index], threshSize[threshSize_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_set (unit %u, group %u, threshSize %u)"
                                        , ret, RT_ERR_OK, unit, group[group_index], threshSize[threshSize_index]);
                }
            }

            ret = rtk_flowctrl_igrCongestThreshGroup_get(unit, group[group_index], &result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (group_result[group_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_get (unit %u, group %u)"
                                , ret, expect_result, unit, group[group_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_igrCongestThreshGroup_get (unit %u, group %u)"
                                    , ret, RT_ERR_OK, unit, group[group_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_igrCongestThreshGroup_test */

int32 dal_flowctrl_egrQueueDropThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_drop_thresh_t  thresh_temp;
    int32  expect_result = 1;

    osal_memset(&thresh_temp, 0, sizeof(rtk_flowctrl_drop_thresh_t));

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrQueueDropThresh_set(unit, 0, &thresh_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrQueueDropThresh_get(unit, 0, &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_qid_t  queueId[UNITTEST_MAX_TEST_VALUE];
        int32  queueId_result[UNITTEST_MAX_TEST_VALUE];
        int32  queueId_index;
        int32  queueId_last;

        uint32  threshSize[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_result[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_index;
        int32  threshSize_last;

        rtk_flowctrl_drop_thresh_t  test_thresh;
        rtk_flowctrl_drop_thresh_t  result_thresh;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_QUEUE(unit) - 1, 0, queueId, queueId_result, queueId_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, threshSize, threshSize_result, threshSize_last);

        for (queueId_index = 0; queueId_index <= queueId_last; queueId_index++)
        {
            for (threshSize_index = 0; threshSize_index <= threshSize_last; threshSize_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (queueId_result[queueId_index] && threshSize_result[threshSize_index])? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&test_thresh, 0, sizeof(rtk_flowctrl_drop_thresh_t));
                test_thresh.high = threshSize[threshSize_index];
                test_thresh.low = threshSize[threshSize_index];

                ret = rtk_flowctrl_egrQueueDropThresh_set(unit, queueId[queueId_index], &test_thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_set (unit %u, queueId %u, threshSize %u)"
                                    , ret, expect_result, unit, queueId[queueId_index], threshSize[threshSize_index]);

                    osal_memset(&result_thresh, 0, sizeof(rtk_flowctrl_drop_thresh_t));
                    ret = rtk_flowctrl_egrQueueDropThresh_get(unit, queueId[queueId_index], &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_get (unit %u, queueId %u, threshSize %u)"
                                     , ret, expect_result, unit, queueId[queueId_index], threshSize[threshSize_index]);

                    ret = osal_memcmp(&result_thresh, &test_thresh, sizeof(rtk_flowctrl_drop_thresh_t));

                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_set/get value unequal (unit %u, queueId %u, threshSize %u)"
                                     , ret, RT_ERR_OK, unit, queueId[queueId_index], threshSize[threshSize_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_set (unit %u, queueId %u, threshSize %u)"
                                        , ret, RT_ERR_OK, unit, queueId[queueId_index], threshSize[threshSize_index]);
                }
            }

            ret = rtk_flowctrl_egrQueueDropThresh_get(unit, queueId[queueId_index], &result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (queueId_result[queueId_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_get (unit %u, queueId %u)"
                                , ret, expect_result, unit, queueId[queueId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrQueueDropThresh_get (unit %u, queueId %u)"
                                    , ret, RT_ERR_OK, unit, queueId[queueId_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_egrQueueDropThresh_test */

int32 dal_flowctrl_egrCpuQueueDropThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_flowctrl_drop_thresh_t  thresh_temp;
    int32  expect_result = 1;

    osal_memset(&thresh_temp, 0, sizeof(rtk_flowctrl_drop_thresh_t));

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrCpuQueueDropThresh_set(unit, 0, &thresh_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrCpuQueueDropThresh_get(unit, 0, &thresh_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_qid_t  queueId[UNITTEST_MAX_TEST_VALUE];
        int32  queueId_result[UNITTEST_MAX_TEST_VALUE];
        int32  queueId_index;
        int32  queueId_last;

        uint32  threshSize[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_result[UNITTEST_MAX_TEST_VALUE];
        int32  threshSize_index;
        int32  threshSize_last;

        rtk_flowctrl_drop_thresh_t  test_thresh;
        rtk_flowctrl_drop_thresh_t  result_thresh;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_QUEUE(unit) - 1, 0, queueId, queueId_result, queueId_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_FLOWCTRL_THRESH_MAX(unit), 0, threshSize, threshSize_result, threshSize_last);

        for (queueId_index = 0; queueId_index <= queueId_last; queueId_index++)
        {
            for (threshSize_index = 0; threshSize_index <= threshSize_last; threshSize_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (queueId_result[queueId_index] && threshSize_result[threshSize_index])? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&test_thresh, 0, sizeof(rtk_flowctrl_drop_thresh_t));
                test_thresh.high = threshSize[threshSize_index];
                test_thresh.low = threshSize[threshSize_index];

                ret = rtk_flowctrl_egrCpuQueueDropThresh_set(unit, queueId[queueId_index], &test_thresh);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_set (unit %u, queueId %u, threshSize %u)"
                                    , ret, expect_result, unit, queueId[queueId_index], threshSize[threshSize_index]);

                    osal_memset(&result_thresh, 0, sizeof(rtk_flowctrl_drop_thresh_t));
                    ret = rtk_flowctrl_egrCpuQueueDropThresh_get(unit, queueId[queueId_index], &result_thresh);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_get (unit %u, queueId %u, threshSize %u)"
                                     , ret, expect_result, unit, queueId[queueId_index], threshSize[threshSize_index]);

                    ret = osal_memcmp(&result_thresh, &test_thresh, sizeof(rtk_flowctrl_drop_thresh_t));

                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_set/get value unequal (unit %u, queueId %u, threshSize %u)"
                                     , ret, RT_ERR_OK, unit, queueId[queueId_index], threshSize[threshSize_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_set (unit %u, queueId %u, threshSize %u)"
                                        , ret, RT_ERR_OK, unit, queueId[queueId_index], threshSize[threshSize_index]);
                }
            }

            ret = rtk_flowctrl_egrCpuQueueDropThresh_get(unit, queueId[queueId_index], &result_thresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (queueId_result[queueId_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_get (unit %u, queueId %u)"
                                , ret, expect_result, unit, queueId[queueId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrCpuQueueDropThresh_get (unit %u, queueId %u)"
                                    , ret, RT_ERR_OK, unit, queueId[queueId_index]);
            }

        }
    }

    return RT_ERR_OK;
}/* dal_flowctrl_egrCpuQueueDropThresh_test */

int32 dal_flowctrl_egrPortQueueDropEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrPortQueueDropEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_flowctrl_egrPortQueueDropEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(ENABLED, DISABLED, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_flowctrl_egrPortQueueDropEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_flowctrl_egrPortQueueDropEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_flowctrl_egrPortQueueDropEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_flowctrl_egrPortQueueDropEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
} /* dal_flowctrl_egrPortQueueDropEnable_test */

