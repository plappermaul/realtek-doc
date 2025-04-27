/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 73144 $
 * $Date: 2016-11-09 11:32:34 +0800 (Wed, 09 Nov 2016) $
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
#include <dal/dal_mgmt.h>
#include <rtk/eee.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>


/* Define symbol used for test input */
/* Unit ID */

/* Define symbol used for test input */
int32 dal_eee_portEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  enable_temp;
    rtk_port_media_t    restore_media;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_eee_portEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_eee_portEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE()){
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(ENABLED, 0, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    ret = rtk_port_phyComboPortMedia_get(unit, port[port_index], &restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);
                    if (restore_media != PORT_MEDIA_COPPER)
                    {
                        ret = rtk_port_phyComboPortMedia_set(unit, port[port_index], PORT_MEDIA_COPPER);
                        RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, expect_result, unit, port[port_index], PORT_MEDIA_COPPER);
                    }
                }
                ret = rtk_eee_portEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_eee_portEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_eee_portEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }

                if (RT_ERR_OK == expect_result && restore_media != PORT_MEDIA_COPPER)
                {
                    ret = rtk_port_phyComboPortMedia_set(unit, port[port_index], restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port[port_index], restore_media);
                }
            }

            ret = rtk_eee_portEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
            else
            expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_eee_portEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        uint32  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), port_max);
        G_VALUE(ENABLED, enable_max);

        for (L_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(0, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, 0, (ENABLED), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    ret = rtk_port_phyComboPortMedia_get(unit, port, &restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                    if (restore_media != PORT_MEDIA_COPPER)
                    {
                        ret = rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_COPPER);
                        RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, expect_result, unit, port, PORT_MEDIA_COPPER);
                    }
                }
                ret = rtk_eee_portEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_eee_portEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eee_portEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_eee_portEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                if (RT_ERR_OK == expect_result && restore_media != PORT_MEDIA_COPPER)
                {
                    ret = rtk_port_phyComboPortMedia_set(unit, port, restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port, restore_media);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_eeep_portEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  enable_temp;
    rtk_port_media_t    restore_media;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_eeep_portEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_eeep_portEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE()){
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(ENABLED, 0, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    ret = rtk_port_phyComboPortMedia_get(unit, port[port_index], &restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);
                    if (restore_media != PORT_MEDIA_COPPER)
                    {
                        ret = rtk_port_phyComboPortMedia_set(unit, port[port_index], PORT_MEDIA_COPPER);
                        RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, expect_result, unit, port[port_index], PORT_MEDIA_COPPER);
                    }
                }

                ret = rtk_eeep_portEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_eeep_portEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_eeep_portEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
                if (RT_ERR_OK == expect_result && restore_media != PORT_MEDIA_COPPER)
                {
                    ret = rtk_port_phyComboPortMedia_set(unit, port[port_index], restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port[port_index], restore_media);
                }
            }

            ret = rtk_eeep_portEnable_get(unit, port[port_index],&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_eeep_portEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        uint32  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), port_max);
        G_VALUE(ENABLED, enable_max);

        for (L_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(0, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, DISABLED, ENABLED, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (RT_ERR_OK == expect_result)
                {
                    ret = rtk_port_phyComboPortMedia_get(unit, port, &restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                    if (restore_media != PORT_MEDIA_COPPER)
                    {
                        ret = rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_COPPER);
                        RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                        , ret, expect_result, unit, port, PORT_MEDIA_COPPER);
                    }
                }
                ret = rtk_eeep_portEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_eeep_portEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_eeep_portEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_eeep_portEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                if (RT_ERR_OK == expect_result && restore_media != PORT_MEDIA_COPPER)
                {
                    ret = rtk_port_phyComboPortMedia_set(unit, port, restore_media);
                    RT_TEST_IS_EQUAL_INT("rtk_port_phyComboPortMedia_set (unit %u, port %u, media %u)"
                                    , ret, expect_result, unit, port, restore_media);
                }
            }
        }
    }

    return RT_ERR_OK;
}
