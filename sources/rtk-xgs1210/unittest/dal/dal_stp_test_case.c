/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74011 $
 * $Date: 2016-12-06 10:24:49 +0800 (Tue, 06 Dec 2016) $
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
#include <rtk/switch.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define STP_RANDOM_RUN_TIMES        100
/*MSTI*/
#define TEST_MSTI_ID_MIN    0
#define TEST_MSTI_ID_MAX(unit)    (HAL_MAX_NUM_OF_MSTI(unit) - 1)
/*MSTP Port State*/
#define TEST_MSTI_PORTSTATE_MIN     STP_STATE_DISABLED
#define TEST_MSTI_PORTSTATE_MAX    STP_STATE_FORWARDING


int32 dal_stp_mstpInstance_create_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  instance_temp = 0;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpInstance_create(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpInstance_destroy(unit, instance_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  instance[UNITTEST_MAX_TEST_VALUE];
        int32  instance_result[UNITTEST_MAX_TEST_VALUE];
        int32  instance_index;
        int32  instance_last;
        uint32  exist_instance;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MSTI_ID_MAX(unit), TEST_MSTI_ID_MIN, instance, instance_result, instance_last);

         /*Remove All The MSTIInstance*/
        for (instance_index = 0; instance_index <= instance_last; instance_index++)
            ret = rtk_stp_mstpInstance_destroy(unit, instance[instance_index]);

        for (instance_index = 0; instance_index <= instance_last; instance_index++)
        {
            inter_result2 = instance_result[instance_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_stp_mstpInstance_create(unit, instance[instance_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_stp_mstpInstance_create (unit %u, instance %u)"
                                , ret, expect_result, unit, instance[instance_index]);

                ret = rtk_stp_isMstpInstanceExist_get(unit, instance[instance_index], &exist_instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get (unit %u, instance %u,)"
                                 , ret, expect_result, unit, instance[instance_index]);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get instance unequal (unit %u, instance %u)"
                                 , exist_instance, TRUE, unit, instance[instance_index]);

                ret = rtk_stp_mstpInstance_destroy(unit, instance[instance_index]);

                ret = rtk_stp_isMstpInstanceExist_get(unit, instance[instance_index], &exist_instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get (unit %u, instance %u,)"
                                 , ret, expect_result, unit, instance[instance_index]);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get instance unequal (unit %u, instance %u)"
                                 , exist_instance, FALSE, unit, instance[instance_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpInstance_create_set (unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance[instance_index]);
            }

        }


    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  instance;
        uint32  instance_max;
        uint32  exist_instance;

        G_VALUE(TEST_MSTI_ID_MAX(unit), instance_max);

        for (L_VALUE(TEST_MSTI_ID_MIN, instance); instance <= instance_max; instance++)
        {
            ASSIGN_EXPECT_RESULT(instance, TEST_MSTI_ID_MIN, TEST_MSTI_ID_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_stp_mstpInstance_create(unit, instance);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_stp_mstpInstance_create (unit %u, instance %u)"
                                , ret, expect_result, unit, instance);
                ret = rtk_stp_isMstpInstanceExist_get(unit, instance, &exist_instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get  (unit %u, instance %u)"
                                 , ret, expect_result, unit, instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get instance unequal (unit %u, instance %u)"
                                 , exist_instance, TRUE, unit, instance);

                ret = rtk_stp_mstpInstance_destroy(unit, instance);

                ret = rtk_stp_isMstpInstanceExist_get(unit, instance, &exist_instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get  (unit %u, instance %u)"
                                 , ret, expect_result, unit, instance);
                RT_TEST_IS_EQUAL_INT("rtk_stp_isMstpInstanceExist_get instance unequal (unit %u, instance %u)"
                                 , exist_instance, FALSE, unit, instance);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpInstance_create(unit %u, instance %u)"
                                    , ret, RT_ERR_OK, unit, instance);
        }
        ret = rtk_stp_mstpInstance_destroy(unit, 0);
    }

    return RT_ERR_OK;
}   /*end of dal_stp_mstpInstance_create_test*/

int32 dal_stp_mstpState_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_stp_state_t  portState_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpState_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpState_get(unit, 0, 0, &portState_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  msti[UNITTEST_MAX_TEST_VALUE];
        int32  msti_result[UNITTEST_MAX_TEST_VALUE];
        int32  msti_index;
        int32  msti_last;

        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_stp_state_t portState[UNITTEST_MAX_TEST_VALUE];
        int32  portState_result[UNITTEST_MAX_TEST_VALUE];
        int32  portState_index;
        int32  portState_last;

        rtk_stp_state_t  result_portState;
        uint32 exist_instance;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MSTI_ID_MAX(unit), TEST_MSTI_ID_MIN, msti, msti_result, msti_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MSTI_PORTSTATE_MAX, TEST_MSTI_PORTSTATE_MIN, portState, portState_result, portState_last);

        for (msti_index = 0; msti_index <= msti_last; msti_index++)
        {
            inter_result2 = msti_result[msti_index];
            for (port_index = 0; port_index <= port_last; port_index++)
            {
                inter_result3 = port_result[port_index];
                for (portState_index = 0; portState_index <= portState_last; portState_index++)
                {
                    inter_result4 = portState_result[portState_index];
                    ret = rtk_stp_isMstpInstanceExist_get(unit, msti[msti_index], &exist_instance);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_stp_mstpState_set(unit, msti[msti_index], port[port_index], portState[portState_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                        , ret, expect_result, unit, msti[msti_index], port[port_index], portState[portState_index]);

                        ret = rtk_stp_mstpState_get(unit, msti[msti_index], port[port_index], &result_portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_get (unit %u, msti %u, port %d, portState %u)"
                                         , ret, expect_result, unit, msti[msti_index], port[port_index], portState[portState_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set/get value unequal (unit %u, msti %u, port %d, portState %u)"
                                         , result_portState, portState[portState_index], unit, msti[msti_index], port[port_index], portState[portState_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                            , ret, RT_ERR_OK, unit, msti[msti_index], port[port_index], portState[portState_index]);
                    }
                }

                ret = rtk_stp_isMstpInstanceExist_get(unit, msti[msti_index], &exist_instance);
                ret = rtk_stp_mstpState_get(unit, msti[msti_index], port[port_index], &result_portState);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, )"
                                    , ret, expect_result, unit, msti[msti_index], port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, )"
                                        , ret, RT_ERR_OK, unit, msti[msti_index], port[port_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  msti;
        uint32  msti_max;
        uint32  port;
        uint32  port_max;
        rtk_stp_state_t  portState;
        rtk_stp_state_t  portState_max;
        rtk_stp_state_t  result_portState;
        uint32 exist_instance;

        G_VALUE(TEST_MSTI_ID_MAX(unit), msti_max);
        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_MSTI_PORTSTATE_MAX, portState_max);

        for (L_VALUE(TEST_MSTI_ID_MIN, msti); msti <= msti_max; msti++)
        {
            ASSIGN_EXPECT_RESULT(msti, TEST_MSTI_ID_MIN, TEST_MSTI_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result3);
                UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result3);

                for (L_VALUE(TEST_MSTI_PORTSTATE_MIN, portState); portState <= portState_max; portState++)
                {
                    ASSIGN_EXPECT_RESULT(portState, TEST_MSTI_PORTSTATE_MIN, TEST_MSTI_PORTSTATE_MAX, inter_result4);
                    ret = rtk_stp_isMstpInstanceExist_get(unit, msti, &exist_instance);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_stp_mstpState_set(unit, msti, port, portState);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                        , ret, expect_result, unit, msti, port, portState);
                        ret = rtk_stp_mstpState_get(unit, msti, port, &result_portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_get (unit %u, msti %u, port %d, portState %u)"
                                         , ret, expect_result, unit, msti, port, portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set/get value unequal (unit %u, msti %u, port %d, portState %u)"
                                         , result_portState, portState, unit, msti, port, portState);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                            , ret, RT_ERR_OK, unit, msti, port, portState);
                }

            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_stp_mstpState_test*/

int32 dal_stp_mstpState_test1(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_stp_state_t  portState_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpState_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_stp_mstpState_get(unit, 0, 0, &portState_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  msti[UNITTEST_MAX_TEST_VALUE];
        int32  msti_result[UNITTEST_MAX_TEST_VALUE];
        int32  msti_index;
        int32  msti_last;

        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_stp_state_t portState[UNITTEST_MAX_TEST_VALUE];
        int32  portState_result[UNITTEST_MAX_TEST_VALUE];
        int32  portState_index;
        int32  portState_last;

        rtk_stp_state_t  result_portState;
        uint32 exist_instance;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MSTI_ID_MAX(unit), TEST_MSTI_ID_MIN, msti, msti_result, msti_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MSTI_PORTSTATE_MAX, TEST_MSTI_PORTSTATE_MIN, portState, portState_result, portState_last);


        /*Add MSTI Instance Here*/
        for (msti_index = 0; msti_index <= TEST_MSTI_ID_MAX(unit); msti_index++)
        {
            ret = rtk_stp_mstpInstance_create(unit, msti_index);
        }

        for (msti_index = 0; msti_index <= msti_last; msti_index++)
        {
            inter_result2 = msti_result[msti_index];
            for (port_index = 0; port_index <= port_last; port_index++)
            {
                inter_result3 = port_result[port_index];
                for (portState_index = 0; portState_index <= portState_last; portState_index++)
                {
                    inter_result4 = portState_result[portState_index];
                    ret = rtk_stp_isMstpInstanceExist_get(unit, msti[msti_index], &exist_instance);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_stp_mstpState_set(unit, msti[msti_index], port[port_index], portState[portState_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                        , ret, expect_result, unit, msti[msti_index], port[port_index], portState[portState_index]);

                        ret = rtk_stp_mstpState_get(unit, msti[msti_index], port[port_index], &result_portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_get (unit %u, msti %u, port %d, portState %u)"
                                         , ret, expect_result, unit, msti[msti_index], port[port_index], portState[portState_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set/get value unequal (unit %u, msti %u, port %d, portState %u)"
                                         , result_portState, portState[portState_index], unit, msti[msti_index], port[port_index], portState[portState_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                            , ret, RT_ERR_OK, unit, msti[msti_index], port[port_index], portState[portState_index]);
                    }
                }

                ret = rtk_stp_isMstpInstanceExist_get(unit, msti[msti_index], &exist_instance);
                ret = rtk_stp_mstpState_get(unit, msti[msti_index], port[port_index], &result_portState);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, )"
                                    , ret, expect_result, unit, msti[msti_index], port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, )"
                                        , ret, RT_ERR_OK, unit, msti[msti_index], port[port_index]);
                }

            }
        }

        /*Destroy MSTI Instance Here*/
        for (msti_index = 0; msti_index <= TEST_MSTI_ID_MAX(unit); msti_index++)
        {
            ret = rtk_stp_mstpInstance_destroy(unit, msti_index);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  msti;
        uint32  msti_max;
        uint32  port;
        uint32  port_max;
        rtk_stp_state_t  portState;
        rtk_stp_state_t  portState_max;
        rtk_stp_state_t  result_portState;
        uint32 exist_instance;

        G_VALUE(TEST_MSTI_ID_MAX(unit), msti_max);
        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_MSTI_PORTSTATE_MAX, portState_max);

        /*Add MSTI Instance Here*/
        for (msti = 0; msti <= TEST_MSTI_ID_MAX(unit); msti++)
        {
            ret = rtk_stp_mstpInstance_create(unit, msti);
        }

        for (L_VALUE(TEST_MSTI_ID_MIN, msti); msti <= msti_max; msti++)
        {
            ASSIGN_EXPECT_RESULT(msti, TEST_MSTI_ID_MIN, TEST_MSTI_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result3);
                UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result3);

                for (L_VALUE(TEST_MSTI_PORTSTATE_MIN, portState); portState <= portState_max; portState++)
                {
                    ASSIGN_EXPECT_RESULT(portState, TEST_MSTI_PORTSTATE_MIN, TEST_MSTI_PORTSTATE_MAX, inter_result4);
                    ret = rtk_stp_isMstpInstanceExist_get(unit, msti, &exist_instance);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4 && exist_instance)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_stp_mstpState_set(unit, msti, port, portState);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                        , ret, expect_result, unit, msti, port, portState);
                        ret = rtk_stp_mstpState_get(unit, msti, port, &result_portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_get (unit %u, msti %u, port %d, portState %u)"
                                         , ret, expect_result, unit, msti, port, portState);
                        RT_TEST_IS_EQUAL_INT("rtk_stp_mstpState_set/get value unequal (unit %u, msti %u, port %d, portState %u)"
                                         , result_portState, portState, unit, msti, port, portState);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_stp_mstpState_set (unit %u, msti %u, port %d, portState %u)"
                                            , ret, RT_ERR_OK, unit, msti, port, portState);
                }

            }
        }

        /*Destroy MSTI Instance Here*/
        for (msti = 0; msti <= TEST_MSTI_ID_MAX(unit); msti++)
        {
            ret = rtk_stp_mstpInstance_destroy(unit, msti);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_stp_mstpState_test*/
