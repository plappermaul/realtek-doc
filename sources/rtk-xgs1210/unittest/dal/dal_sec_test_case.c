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
#include <rtk/sec.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/* Common */
#define TEST_PORT_ID_MAX_WO_CPU(unit)   (TEST_ETHER_PORT_ID_MAX(unit))

/* ATTACK */
#define TEST_ATTACK_TYPE_MIN                    DAEQSA_DENY
#define TEST_ATTACK_TYPE_MAX(unit)              ((HWP_9310_FAMILY_ID(unit)) ? IPV4_INVALID_HDR : \
                                                 (HWP_9300_FAMILY_ID(unit)) ? IPV4_INVALID_LEN : \
                                                 (HWP_8390_50_FAMILY(unit)) ? GRATUITOUS_ARP : GRATUITOUS_ARP)


#define TEST_ATTACK_PREVENT_TYPE_MIN            DAEQSA_DENY
#define TEST_ATTACK_PREVENT_TYPE_MAX(unit)      ((HWP_9310_FAMILY_ID(unit)) ? IPV4_INVALID_HDR : \
                                                 (HWP_9300_FAMILY_ID(unit)) ? IPV4_INVALID_LEN : \
                                                 (HWP_8390_50_FAMILY(unit)) ? TCP_FRAG_OFF_MIN_CHECK : TCP_FRAG_OFF_MIN_CHECK)

#define TEST_PORT_ATTACK_ACTION_MIN             ACTION_FORWARD
#define TEST_PORT_ATTACK_ACTION_MAX(unit)       ((HWP_9310_FAMILY_ID(unit)|| HWP_9300_FAMILY_ID(unit) || HWP_8390_50_FAMILY(unit))\
                                                ? ACTION_TRAP2CPU : ACTION_COPY2CPU)


#define TEST_ATTACK_ACTION_MIN                  ACTION_FORWARD
#define TEST_ATTACK_ACTION_MAX(unit)            ((HWP_9310_FAMILY_ID(unit)|| HWP_9300_FAMILY_ID(unit) || HWP_8390_50_FAMILY(unit)) ? ACTION_TRAP2CPU : ACTION_TRAP2CPU)

#define TEST_PING_LENGTH_MIN    0
#define TEST_PING_LENGTH_MAX(unit)    HAL_SEC_MAXPINGLEN_MAX(unit)

#define TEST_V6FRAG_LENGTH_MIN  0
#define TEST_V6FRAG_LENGTH_MAX(unit)  HAL_SEC_MINIPV6FRAGLEN_MAX(unit)

#define TEST_TCPHDR_LENGTH_MIN  0
#define TEST_TCPHDR_LENGTH_MAX  0x1f

#define TEST_SMURFMASK_LENGTH_MIN  0
#define TEST_SMURFMASK_LENGTH_MAX  32

int32 dal_sec_portAttackPrevent_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_portAttackPrevent_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_portAttackPrevent_get(unit, 0, 0, &action_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_sec_attackType_t  attack_type[UNITTEST_MAX_TEST_VALUE];
        int32  attack_type_result[UNITTEST_MAX_TEST_VALUE];
        int32  attack_type_index;
        int32  attack_type_last;

        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_WO_CPU_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ATTACK_TYPE_MAX(unit), TEST_ATTACK_PREVENT_TYPE_MIN, attack_type, attack_type_result, attack_type_last);

        G_VALUE(TEST_ATTACK_ACTION_MAX(unit), action_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (attack_type_index = 0; attack_type_index <= attack_type_last; attack_type_index++)
            {
                inter_result3 = attack_type_result[attack_type_index];
                for (L_VALUE(TEST_PORT_ATTACK_ACTION_MIN, action); action <= action_max; action++)
                {
                    ASSIGN_EXPECT_RESULT(action, TEST_PORT_ATTACK_ACTION_MIN, TEST_PORT_ATTACK_ACTION_MAX(unit), inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    if (HWP_8380_30_FAMILY(unit)|| HWP_8390_50_FAMILY(unit))
                    {
                        if (action == ACTION_COPY2CPU && attack_type[attack_type_index] == ARP_INVALID)
                            expect_result = RT_ERR_FAILED;
                        if ((attack_type[attack_type_index] != ARP_INVALID) && (attack_type[attack_type_index] != GRATUITOUS_ARP))
                            expect_result = RT_ERR_FAILED;
                    }
                    if (HWP_8390_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                    {
                        if (action != ACTION_FORWARD || action != ACTION_DROP || action != ACTION_TRAP2CPU)
                            expect_result = RT_ERR_FAILED;
                    }

                    ret = rtk_sec_portAttackPrevent_set(unit, port[port_index], attack_type[attack_type_index], action);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_set (unit %u, port %u, attack_type %u, action %u)"
                                        , ret, expect_result, unit, port[port_index], attack_type[attack_type_index], action);

                        ret = rtk_sec_portAttackPrevent_get(unit, port[port_index], attack_type[attack_type_index], &result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_get (unit %u, port %u, attack_type %u, action %u)"
                                         , ret, expect_result, unit, port[port_index], attack_type[attack_type_index], result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_set/get value unequal (unit %u, port %u, attack_type %u, action %u)"
                                         , result_action, action, unit, port[port_index], attack_type[attack_type_index], action);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPrevent_set (unit %u, port %u, attack_type %u, action %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], attack_type[attack_type_index], action);
                    }
                }
                ret = rtk_sec_portAttackPrevent_get(unit, port[port_index], attack_type[attack_type_index], (rtk_action_t*)&result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (HWP_8380_30_FAMILY(unit)|| HWP_8390_50_FAMILY(unit))
                {
                    if (action == ACTION_COPY2CPU && attack_type[attack_type_index] == ARP_INVALID)
                        expect_result = RT_ERR_FAILED;
                    if ((attack_type[attack_type_index] != ARP_INVALID) && (attack_type[attack_type_index] != GRATUITOUS_ARP))
                        expect_result = RT_ERR_FAILED;
                }
                if (HWP_8390_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                {
                    if (action != ACTION_FORWARD || action != ACTION_DROP || action != ACTION_TRAP2CPU)
                        expect_result = RT_ERR_FAILED;
                }

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_get (unit %u, port %u, attack_type %u)"
                                    , ret, expect_result, unit, port[port_index], attack_type[attack_type_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPrevent_get (unit %u, port %u, attack_type %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], attack_type[attack_type_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_sec_attackType_t  attack_type;
        rtk_sec_attackType_t  attack_type_max;
        rtk_action_t  action;
        rtk_action_t  action_max;

        rtk_action_t  result_action;

        G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        G_VALUE(TEST_ATTACK_TYPE_MAX(unit), attack_type_max);
        G_VALUE(TEST_PORT_ATTACK_ACTION_MAX(unit), action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ATTACK_PREVENT_TYPE_MIN, attack_type); attack_type <= attack_type_max; attack_type++)
            {
                ASSIGN_EXPECT_RESULT(attack_type, TEST_ATTACK_PREVENT_TYPE_MIN, TEST_ATTACK_TYPE_MAX(unit), inter_result3);

                for (L_VALUE(TEST_PORT_ATTACK_ACTION_MIN, action); action <= action_max; action++)
                {
                    ASSIGN_EXPECT_RESULT(action, TEST_PORT_ATTACK_ACTION_MIN, TEST_PORT_ATTACK_ACTION_MAX(unit), inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    if (HWP_8380_30_FAMILY(unit)|| HWP_8390_50_FAMILY(unit))
                    {
                        if (action == ACTION_COPY2CPU && attack_type == ARP_INVALID)
                            expect_result = RT_ERR_FAILED;
                        if ((attack_type != ARP_INVALID) && (attack_type != GRATUITOUS_ARP))
                            expect_result = RT_ERR_FAILED;
                    }

                    if (HWP_8390_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
                    {
                        if (action != ACTION_FORWARD || action != ACTION_DROP || action != ACTION_TRAP2CPU)
                            expect_result = RT_ERR_FAILED;
                    }

                    ret = rtk_sec_portAttackPrevent_set(unit, port, attack_type, action);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_set (unit %u, port %u, attack_type %u, action %u)"
                                        , ret, expect_result, unit, port, attack_type, action);

                        ret = rtk_sec_portAttackPrevent_get(unit, port, attack_type, &result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_get (unit %u, port %u, attack_type %u, action %u)"
                                         , ret, expect_result, unit, port, attack_type, result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_set/get value unequal (unit %u, port %u, attack_type %u, action %u)"
                                         , result_action, action, unit, port, attack_type, action);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPrevent_set (unit %u, port %u, attack_type %u, action %u)"
                                            , ret, RT_ERR_OK, unit, port, attack_type, action);
                    }
                    ret = rtk_sec_portAttackPrevent_get(unit, port, attack_type, &result_action);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                    if (HWP_8380_30_FAMILY(unit)|| HWP_8390_50_FAMILY(unit))
                    {
                        if (action == ACTION_COPY2CPU && attack_type == ARP_INVALID)
                            expect_result = RT_ERR_FAILED;
                        if ((attack_type != ARP_INVALID) && (attack_type != GRATUITOUS_ARP))
                            expect_result = RT_ERR_FAILED;
                    }

                    if (HWP_8390_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
                    {
                        if (action != ACTION_FORWARD || action != ACTION_DROP || action != ACTION_TRAP2CPU)
                            expect_result = RT_ERR_FAILED;
                    }

                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPrevent_get (unit %u, port %u, attack_type %u)"
                                        , ret, expect_result, unit, port, attack_type);
                    }
                }
            }

        }
    }
    return RT_ERR_OK;
}

int32 dal_sec_portAttackPreventEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_portAttackPreventEnable_set(unit, TEST_PORT_ID_MIN(unit), TEST_ENABLE_STATUS_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_portAttackPreventEnable_get(unit, TEST_PORT_ID_MIN(unit), &enable_temp)))
    {
        return RT_ERR_OK;
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
                ret = rtk_sec_portAttackPreventEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_sec_portAttackPreventEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPreventEnable_set (unit %u, port %u, enable %u)"
                                        , ret, expect_result, unit, port[port_index], enable[enable_index]);
                }
                ret = rtk_sec_portAttackPreventEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index]);
                }

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
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_sec_portAttackPreventEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_sec_portAttackPreventEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPreventEnable_set (unit %u, port %u, enable %u)"
                                        , ret, expect_result, unit, port, enable);
                }
                ret = rtk_sec_portAttackPreventEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_portAttackPreventEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }

        }
    }
    return RT_ERR_OK;
}/* dal_sec_portAttackPreventEnable_test() */

int32 dal_sec_attackPreventAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_attackPreventAction_set(unit, TEST_ATTACK_TYPE_MIN, TEST_ATTACK_ACTION_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_attackPreventAction_get(unit, TEST_ATTACK_TYPE_MIN, &action_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_sec_attackType_t  attackType[UNITTEST_MAX_TEST_VALUE];
        int32  attackType_result[UNITTEST_MAX_TEST_VALUE];
        int32  attackType_index;
        int32  attackType_last;

        rtk_action_t action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ATTACK_PREVENT_TYPE_MAX(unit), TEST_ATTACK_PREVENT_TYPE_MIN, attackType, attackType_result, attackType_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ATTACK_ACTION_MAX(unit), TEST_ATTACK_ACTION_MIN, action, action_result, action_last);

        for (attackType_index = 0; attackType_index <= attackType_last; attackType_index++)
        {
            inter_result2 = attackType_result[attackType_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_sec_attackPreventAction_set(unit, attackType[attackType_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_set (unit %u, attackType %u, action %u)"
                                    , ret, expect_result, unit, attackType[attackType_index], action[action_index]);

                    ret = rtk_sec_attackPreventAction_get(unit, attackType[attackType_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_get (unit %u, attackType %u, action %u)"
                                     , ret, expect_result, unit, attackType[attackType_index], result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_set/get value unequal (unit %u, attackType %u, action %u)"
                                     , result_action, action[action_index], unit, attackType[attackType_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_attackPreventAction_set (unit %u, attackType %u, action %u)"
                                        , ret, RT_ERR_OK, unit, attackType[attackType_index], action[action_index]);
                }
                ret = rtk_sec_attackPreventAction_get(unit, attackType[attackType_index], (rtk_action_t*)&result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_get (unit %u, attackType %u)"
                                    , ret, expect_result, unit, attackType[attackType_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_attackPreventAction_get (unit %u, attackType %u)"
                                        , ret, RT_ERR_OK, unit, attackType[attackType_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_sec_attackType_t  attackType;
        rtk_sec_attackType_t  attackType_max;
        rtk_action_t  action;
        rtk_action_t  action_max;

        rtk_action_t  result_action;

        G_VALUE(TEST_ATTACK_TYPE_MAX(unit), attackType_max);
        G_VALUE(TEST_ATTACK_ACTION_MAX(unit), action_max);

        for (L_VALUE(TEST_ATTACK_TYPE_MIN, attackType); attackType <= attackType_max; attackType++)
        {
            ASSIGN_EXPECT_RESULT(attackType, TEST_ATTACK_PREVENT_TYPE_MIN, TEST_ATTACK_PREVENT_TYPE_MAX(unit), inter_result2);

            for (L_VALUE(TEST_ATTACK_ACTION_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ATTACK_ACTION_MIN, TEST_ATTACK_ACTION_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_sec_attackPreventAction_set(unit, attackType, action);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_set (unit %u, attackType %u, action %u)"
                                    , ret, expect_result, unit, attackType, action);

                    ret = rtk_sec_attackPreventAction_get(unit, attackType, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_get (unit %u, attackType %u, action %u)"
                                     , ret, expect_result, unit, attackType, result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_set/get value unequal (unit %u, attackType %u, action %u)"
                                     , result_action, action, unit, attackType, action);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_attackPreventAction_set (unit %u, attackType %u, action %u)"
                                        , ret, RT_ERR_OK, unit, attackType, action);
                }
                ret = rtk_sec_attackPreventAction_get(unit, attackType, &result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_attackPreventAction_get (unit %u, attackType %u)"
                                    , ret, expect_result, unit, attackType);
                }
            }

        }
    }
    return RT_ERR_OK;
}/* dal_sec_attackPreventAction_test() */




int32 dal_sec_minIPv6FragLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_minIPv6FragLen_set(unit, TEST_V6FRAG_LENGTH_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_minIPv6FragLen_get(unit, &length_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;

        uint32  result_length;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_V6FRAG_LENGTH_MAX(unit), TEST_V6FRAG_LENGTH_MIN, length, length_result, length_last);


        for (length_index = 0; length_index <= length_last; length_index++)
        {
            inter_result2 = length_result[length_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_minIPv6FragLen_set(unit, length[length_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);

                ret = rtk_sec_minIPv6FragLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length[length_index], unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minIPv6FragLen_set (unit %u, attackType %ulength)"
                                    , ret, expect_result, unit, length[length_index]);
            }
            ret = rtk_sec_minIPv6FragLen_get(unit, (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length[length_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  length;
        uint32  length_max;

        uint32  result_length;

        G_VALUE(TEST_V6FRAG_LENGTH_MAX(unit), length_max);

        for (L_VALUE(TEST_V6FRAG_LENGTH_MIN, length); length <= length_max; length++)
        {
            ASSIGN_EXPECT_RESULT(length, TEST_V6FRAG_LENGTH_MIN, TEST_V6FRAG_LENGTH_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_minIPv6FragLen_set(unit, length);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length);

                ret = rtk_sec_minIPv6FragLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minIPv6FragLen_set (unit %u, length %u)"
                                    , ret, expect_result, unit, length);
            }
            ret = rtk_sec_minIPv6FragLen_get(unit, &result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                               , ret, expect_result, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minIPv6FragLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length);
            }
        }
    }
    return RT_ERR_OK;
}/* dal_sec_minIPv6FragLen_test() */

int32 dal_sec_maxPingLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_maxPingLen_set(unit, TEST_PING_LENGTH_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_maxPingLen_get(unit, &length_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;
        uint32  result_length;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PING_LENGTH_MAX(unit), TEST_PING_LENGTH_MIN, length, length_result, length_last);

        for (length_index = 0; length_index <= length_last; length_index++)
        {
            inter_result2 = length_result[length_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_maxPingLen_set(unit, length[length_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);

                ret = rtk_sec_maxPingLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length[length_index], unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_maxPingLen_set (unit %u, attackType %ulength)"
                                    , ret, expect_result, unit, length[length_index]);
            }
            ret = rtk_sec_maxPingLen_get(unit, (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length[length_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  length;
        uint32  length_max;
        uint32  result_length;

        G_VALUE(TEST_PING_LENGTH_MAX(unit), length_max);

        for (L_VALUE(TEST_PING_LENGTH_MIN, length); length <= length_max; length++)
        {
            ASSIGN_EXPECT_RESULT(length, TEST_PING_LENGTH_MIN, TEST_PING_LENGTH_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_maxPingLen_set(unit, length);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length);

                ret = rtk_sec_maxPingLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_maxPingLen_set (unit %u, length %u)"
                                    , ret, expect_result, unit, length);
            }
            ret = rtk_sec_maxPingLen_get(unit, &result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                               , ret, expect_result, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_maxPingLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length);
            }
        }
    }
    return RT_ERR_OK;
}/* dal_sec_maxPingLen_test() */

int32 dal_sec_minTCPHdrLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_minTCPHdrLen_set(unit, TEST_TCPHDR_LENGTH_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_minTCPHdrLen_get(unit, &length_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;
        uint32  result_length;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TCPHDR_LENGTH_MAX, TEST_TCPHDR_LENGTH_MIN, length, length_result, length_last);

        for (length_index = 0; length_index <= length_last; length_index++)
        {
            inter_result2 = length_result[length_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_minTCPHdrLen_set(unit, length[length_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);

                ret = rtk_sec_minTCPHdrLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length[length_index], unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minTCPHdrLen_set (unit %u, attackType %ulength)"
                                    , ret, expect_result, unit, length[length_index]);
            }
            ret = rtk_sec_minTCPHdrLen_get(unit, (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length[length_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  length;
        uint32  length_max;
        uint32  result_length;

        G_VALUE(TEST_PING_LENGTH_MAX(unit), length_max);

        for (L_VALUE(TEST_TCPHDR_LENGTH_MIN, length); length <= length_max; length++)
            {
                ASSIGN_EXPECT_RESULT(length, TEST_TCPHDR_LENGTH_MIN, TEST_TCPHDR_LENGTH_MAX, inter_result2);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_sec_minTCPHdrLen_set(unit, length);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_set (unit %u, length %u)"
                                    , ret, expect_result, unit, length);

                    ret = rtk_sec_minTCPHdrLen_get(unit, &result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                     , ret, expect_result, unit, result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_set/get value unequal (unit %u, length %u)"
                                     , result_length, length, unit, length);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minTCPHdrLen_set (unit %u, length %u)"
                                        , ret, expect_result, unit, length);
                }
                ret = rtk_sec_minTCPHdrLen_get(unit, &result_length);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = RT_ERR_OK;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                   , ret, expect_result, unit, length);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_minTCPHdrLen_get (unit %u, length %u)"
                                        , ret, RT_ERR_OK, unit, length);
                }
            }
    }
    return RT_ERR_OK;
}/* dal_sec_minTCPHdrLen_test() */

int32 dal_sec_smurfNetmaskLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_smurfNetmaskLen_set(unit, TEST_SMURFMASK_LENGTH_MIN))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_sec_smurfNetmaskLen_get(unit, &length_temp)))
    {
        return RT_ERR_OK;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;
        uint32  result_length;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_SMURFMASK_LENGTH_MAX, TEST_SMURFMASK_LENGTH_MIN, length, length_result, length_last);

        for (length_index = 0; length_index <= length_last; length_index++)
        {
            inter_result2 = length_result[length_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_smurfNetmaskLen_set(unit, length[length_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);

                ret = rtk_sec_smurfNetmaskLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length[length_index], unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_smurfNetmaskLen_set (unit %u, attackType %ulength)"
                                    , ret, expect_result, unit, length[length_index]);
            }
            ret = rtk_sec_smurfNetmaskLen_get(unit, (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                                , ret, expect_result, unit, length[length_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length[length_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  length;
        uint32  length_max;
        uint32  result_length;

        G_VALUE(TEST_SMURFMASK_LENGTH_MAX, length_max);

        for (L_VALUE(TEST_SMURFMASK_LENGTH_MIN, length); length <= length_max; length++)
        {
            ASSIGN_EXPECT_RESULT(length, TEST_SMURFMASK_LENGTH_MIN, TEST_SMURFMASK_LENGTH_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_sec_smurfNetmaskLen_set(unit, length);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_set (unit %u, length %u)"
                                , ret, expect_result, unit, length);

                ret = rtk_sec_smurfNetmaskLen_get(unit, &result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                                 , ret, expect_result, unit, result_length);
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_set/get value unequal (unit %u, length %u)"
                                 , result_length, length, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_smurfNetmaskLen_set (unit %u, length %u)"
                                    , ret, expect_result, unit, length);
            }
            ret = rtk_sec_smurfNetmaskLen_get(unit, &result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                               , ret, expect_result, unit, length);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_sec_smurfNetmaskLen_get (unit %u, length %u)"
                                    , ret, RT_ERR_OK, unit, length);
            }
        }
    }
    return RT_ERR_OK;
}/* dal_sec_smurfNetmaskLen_test() */

