/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74642 $
 * $Date: 2016-12-23 16:27:24 +0800 (Fri, 23 Dec 2016) $
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
#include <rtk/led.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */

/* Port media type */
#define TEST_PORT_MEDIA_TYPE_MIN    PORT_MEDIA_COPPER
#define TEST_PORT_MEDIA_TYPE_MAX    PORT_MEDIA_FIBER
/* System LED type */
#define TEST_SYSLED_TYPE_MIN    LED_TYPE_SYS
#define TEST_SYSLED_TYPE_MAX(unit)    (HWP_8390_50_FAMILY(unit)?LED_TYPE_SYS:(RTK_LED_TYPE_END-1))
/* led_swCtrl_mode */
#define TEST_LED_SWCTRL_MODE_MIN  RTK_LED_SWCTRL_MODE_OFF
#define TEST_LED_SWCTRL_MODE_MAX  (RTK_LED_SWCTRL_MODE_BLINKING_END-1)

int32 dal_led_sysEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_sysEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_sysEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_led_type_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_SYSLED_TYPE_MAX(unit), TEST_SYSLED_TYPE_MIN, type, type_result, type_last);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (LED_TYPE_SYS != type[type_index])
                    expect_result = RT_ERR_FAILED;
                ret = rtk_led_sysEnable_set(unit, type[type_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_led_sysEnable_set (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type[type_index], enable);

                    ret = rtk_led_sysEnable_get(unit, type[type_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_led_sysEnable_get (unit %u, type %u, enable %u)"
                                     , ret, expect_result, unit, type[type_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_led_sysEnable_set/get value unequal (unit %u, type %u, enable %u)"
                                     , result_enable, enable, unit, type[type_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_led_sysEnable_set (unit %u, type %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], enable);
                }
            }
            ret = rtk_led_sysEnable_get(unit, type[type_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (LED_TYPE_SYS != type[type_index])
                expect_result = RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_led_sysEnable_get (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_led_sysEnable_get (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_led_portLedEntitySwCtrlEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_portLedEntitySwCtrlEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_portLedEntitySwCtrlEnable_get(unit, 0, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  entity[UNITTEST_MAX_TEST_VALUE];
        int32  entity_result[UNITTEST_MAX_TEST_VALUE];
        int32  entity_index;
        int32  entity_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, 0, entity, entity_result, entity_last);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (entity_index = 0; entity_index <= entity_last; entity_index++)
            {
                inter_result3 = entity_result[entity_index];
                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_led_portLedEntitySwCtrlEnable_set(unit, port[port_index], entity[entity_index], enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set (unit %u, port %u, entity %u, enable %u)"
                                        , ret, expect_result, unit, port[port_index], entity[entity_index], enable);

                        ret = rtk_led_portLedEntitySwCtrlEnable_get(unit, port[port_index], entity[entity_index], &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u, enable %u)"
                                         , ret, expect_result, unit, port[port_index], entity[entity_index], result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set/get value unequal (unit %u, port %u, entity %u, enable %u)"
                                         , result_enable, enable, unit, port[port_index], entity[entity_index], enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set (unit %u, port %u, entity %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], entity[entity_index], enable);
                    }
                }
                ret = rtk_led_portLedEntitySwCtrlEnable_get(unit, port[port_index], entity[entity_index], &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u)"
                                    , ret, expect_result, unit, port[port_index], entity[entity_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], entity[entity_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  entity;
        uint32  entity_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;

        G_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), port_max);
        G_VALUE(HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, entity_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(0, entity); entity <= entity_max; entity++)
            {
                ASSIGN_EXPECT_RESULT(entity, 0, HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_led_portLedEntitySwCtrlEnable_set(unit, port, entity, enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set (unit %u, port %u, entity %u, enable %u)"
                                        , ret, expect_result, unit, port, entity, enable);

                        ret = rtk_led_portLedEntitySwCtrlEnable_get(unit, port, entity, &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u, enable %u)"
                                         , ret, expect_result, unit, port, entity, result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set/get value unequal (unit %u, port %u, entity %u, enable %u)"
                                         , result_enable, enable, unit, port, entity, enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_set (unit %u, port %u, entity %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port, entity, enable);
                    }
                    ret = rtk_led_portLedEntitySwCtrlEnable_get(unit, port, entity, &result_enable);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u)"
                                        , ret, expect_result, unit, port, entity);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlEnable_get (unit %u, port %u, entity %u)"
                                            , ret, RT_ERR_OK, unit, port, entity);
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_led_portLedEntitySwCtrlMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_led_swCtrl_mode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_portLedEntitySwCtrlMode_set(unit, 0, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_portLedEntitySwCtrlMode_get(unit, 0, 0, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  entity[UNITTEST_MAX_TEST_VALUE];
        int32  entity_result[UNITTEST_MAX_TEST_VALUE];
        int32  entity_index;
        int32  entity_last;

        uint32  media[UNITTEST_MAX_TEST_VALUE];
        int32  media_result[UNITTEST_MAX_TEST_VALUE];
        int32  media_index;
        int32  media_last;

        rtk_led_swCtrl_mode_t  mode;
        uint32  mode_max;

        rtk_led_swCtrl_mode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, 0, entity, entity_result, entity_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_MEDIA_TYPE_MAX, TEST_PORT_MEDIA_TYPE_MIN, media, media_result, media_last);
        G_VALUE(TEST_LED_SWCTRL_MODE_MAX, mode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (entity_index = 0; entity_index <= entity_last; entity_index++)
            {
                inter_result3 = entity_result[entity_index];
                for (media_index = 0; media_index <= media_last; media_index++)
                {
                    inter_result4 = media_result[media_index];
                    for (L_VALUE(TEST_LED_SWCTRL_MODE_MIN, mode); mode <= mode_max; mode++)
                    {
                        ASSIGN_EXPECT_RESULT(mode, TEST_LED_SWCTRL_MODE_MIN, TEST_LED_SWCTRL_MODE_MAX, inter_result5);
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;
                        if((HWP_COPPER_PORT(unit, port[port_index]) && media[media_index] == PORT_MEDIA_FIBER) || (HWP_FIBER_PORT(unit, port[port_index]) && media[media_index] == PORT_MEDIA_COPPER))
                                expect_result = RT_ERR_FAILED;

                        ret = rtk_led_portLedEntitySwCtrlMode_set(unit, port[port_index], entity[entity_index], media[media_index], mode);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set (unit %u, port %u, entity %u, media %u, mode %u)"
                                            , ret, expect_result, unit, port[port_index], entity[entity_index], media[media_index], mode);

                            ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port[port_index], entity[entity_index], media[media_index], &result_mode);
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u, mode %u)"
                                             , ret, expect_result, unit, port[port_index], entity[entity_index], media[media_index], result_mode);
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set/get value unequal (unit %u, port %u, entity %u, media %u, mode %u)"
                                             , result_mode, mode, unit, port[port_index], entity[entity_index], media[media_index], mode);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set (unit %u, port %u, entity %u, media %u, mode %u)"
                                                , ret, RT_ERR_OK, unit, port[port_index], entity[entity_index], media[media_index], mode);
                        }
                    }
                    ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port[port_index], entity[entity_index], media[media_index], &result_mode);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u)"
                                        , ret, expect_result, unit, port[port_index], entity[entity_index], media[media_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], entity[entity_index], media[media_index]);
                    }
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  entity;
        uint32  entity_max;
        uint32  media;
        uint32  media_max;
        rtk_led_swCtrl_mode_t  mode;
        uint32  mode_max;

        rtk_led_swCtrl_mode_t  result_mode;

        G_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), port_max);
        G_VALUE(HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, entity_max);
        G_VALUE(TEST_PORT_MEDIA_TYPE_MAX, media_max);
        G_VALUE(TEST_LED_SWCTRL_MODE_MAX, mode_max);

        for (L_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(0, entity); entity <= entity_max; entity++)
            {
                ASSIGN_EXPECT_RESULT(entity, 0, HAL_MAX_NUM_OF_LED_ENTITY(unit)-1, inter_result3);

                for (L_VALUE(TEST_PORT_MEDIA_TYPE_MIN, media); media <= media_max; media++)
                {
                    ASSIGN_EXPECT_RESULT(media, TEST_PORT_MEDIA_TYPE_MIN, TEST_PORT_MEDIA_TYPE_MAX, inter_result4);

                    for (L_VALUE(TEST_LED_SWCTRL_MODE_MIN, mode); mode <= mode_max; mode++)
                    {
                        ASSIGN_EXPECT_RESULT(mode, TEST_LED_SWCTRL_MODE_MIN, TEST_LED_SWCTRL_MODE_MAX, inter_result5);
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;

                        if ((HWP_8380_30_FAMILY(unit)) && ((RTK_LED_SWCTRL_MODE_BLINKING_256MS == mode) || (RTK_LED_SWCTRL_MODE_BLINKING_1024MS == mode)))
                            expect_result = RT_ERR_FAILED;

                        ret = rtk_led_portLedEntitySwCtrlMode_set(unit, port, entity, media, mode);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set (unit %u, port %u, entity %u, media %u, mode %u)"
                                            , ret, expect_result, unit, port, entity, media, mode);

                            ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port, entity, media, &result_mode);
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u, mode %u)"
                                             , ret, expect_result, unit, port, entity, media, result_mode);
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set/get value unequal (unit %u, port %u, entity %u, media %u, mode %u)"
                                             , result_mode, mode, unit, port, entity, media, result_mode);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_set (unit %u, port %u, entity %u, media %u, mode %u)"
                                                , ret, RT_ERR_OK, unit, port, entity, media, result_mode);
                        }
                        ret = rtk_led_portLedEntitySwCtrlMode_get(unit, port, entity, media, &result_mode);
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                        if (expect_result == RT_ERR_OK)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u)"
                                            , ret, expect_result, unit, port, entity, media);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_led_portLedEntitySwCtrlMode_get (unit %u, port %u, entity %u, media %u)"
                                                , ret, RT_ERR_OK, unit, port, entity, media);
                        }
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_led_sysMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_led_swCtrl_mode_t  mode_temp;
    int8    inter_result2 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_sysMode_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_led_sysMode_get(unit, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_led_swCtrl_mode_t  mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_led_swCtrl_mode_t  result_mode;

       UNITTEST_TEST_VALUE_ASSIGN(TEST_LED_SWCTRL_MODE_MAX, TEST_LED_SWCTRL_MODE_MIN, mode, mode_result, mode_last);


        for (mode_index = 0; mode_index <= mode_last; mode_index++)
        {
            inter_result2 = mode_result[mode_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if(HWP_8390_50_FAMILY(unit)||HWP_8380_30_FAMILY(unit))
            {
                if (RTK_LED_SWCTRL_MODE_OFF != mode[mode_index] && RTK_LED_SWCTRL_MODE_BLINKING_64MS != mode[mode_index] &&
                    RTK_LED_SWCTRL_MODE_BLINKING_1024MS != mode[mode_index] && RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE != mode[mode_index])
                    expect_result = RT_ERR_FAILED;
            }
            ret = rtk_led_sysMode_set(unit, mode[mode_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_led_sysMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);

                ret = rtk_led_sysMode_get(unit, &result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_led_sysMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode[mode_index]);
                RT_TEST_IS_EQUAL_INT("rtk_led_sysMode_get (unit %u, mode %u)"
                                 , result_mode, mode[mode_index], unit, mode[mode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_led_sysMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
        }

        ret = rtk_led_sysMode_get(unit, &result_mode);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_led_sysMode_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_led_sysMode_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}
