/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74485 $
 * $Date: 2016-12-21 11:03:26 +0800 (Wed, 21 Dec 2016) $
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
#include <rtk/trap.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
#define TRAP_RANDOM_RUN_TIMES                   100
/*User Defined Layer2 RMA Entry*/
#define TEST_UDL2RMA_ID_MIN                     0
#define TEST_UDL2RMA_ID_MAX(unit)               (HWP_8380_30_FAMILY(unit)?5:1)
/*User Defined Layer2 RMA Entry*/
#define TEST_UDL34RMA_ID_MIN                    0
#define TEST_UDL34RMA_ID_MAX                    1
/*RMA Action*/
#define TEST_RMAACT_ID_MIN                      ACTION_FORWARD
#define TEST_RMAACT_ID_MAX                      (ACTION_END-1)
/*MGMT Action*/
#define TEST_MGMT_ACT_ID_MIN                    MGMT_ACTION_FORWARD
#define TEST_MGMT_ACT_ID_MAX                   (MGMT_ACTION_END-1)
/*RTK RMA Action*/
#define TEST_RTK_ACT_ID_MIN                     ACTION_FORWARD
#define TEST_RTK_ACT_ID_MAX                     (ACTION_END-1)
/*Manage Protocol Type*/
#define TEST_MGMTYPE_ID_MIN                     MGMT_TYPE_ARP
#define TEST_MGMTYPE_ID_MAX                     (MGMT_TYPE_END - 1)
/*PortManage Protocol Type*/
#define TEST_PMGMTYPE_ID_MIN                    MGMT_TYPE_BPDU
#define TEST_PMGMTYPE_ID_MAX                    MGMT_TYPE_LLDP
/*Md level*/
#define TEST_MDLEVEL_ID_MIN                     0
#define TEST_MDLEVEL_ID_MAX                     7
/*RMA mac[5]*/
#define TEST_RMA_MAC5_MIN                       0
#define TEST_RMA_MAC5_MAX                       0x2F
/*By Pass Stp Type*/
#define TEST_BYPASSSTPTYPE_MIN                  BYPASS_STP_TYPE_USER_DEF_0
#define TEST_BYPASSSTPTYPE_MAX                  (BYPASS_STP_TYPE_END-1)
/*By Pass Vlan Type*/
#define TEST_BYPASSVLANTYPE_MIN                 BYPASS_VLAN_TYPE_USER_DEF_0
#define TEST_BYPASSVLANTYPE_MAX                 (BYPASS_VLAN_TYPE_END-1)
/*cfmUnknownFrameAct*/
#define TEST_CFMUNKNOWNFRAMEACT_MIN             ACTION_FORWARD
#define TEST_CFMUNKNOWNFRAMEACT_MAX             ACTION_TRAP2CPU
/*cfmLoopbackAct*/
#define TEST_CFMLOOPBACKACT_MIN                 ACTION_FORWARD
#define TEST_CFMLOOPBACKACT_MAX                 ACTION_TRAP2CPU
/*cfmCcmAct*/
#define TEST_OAM_ACTION_MIN                     TRAP_OAM_ACTION_FORWARD
#define TEST_OAM_ACTION_MAX                     (TRAP_OAM_ACTION_END-1)
/*cfmEthDmAct*/
#define TEST_CFMETHDMACT_MIN                    ACTION_FORWARD
#define TEST_CFMETHDMACT_MAX                    ACTION_TRAP2CPU
/*routeExceptionType*/
#define TEST_ROUTE_EXCEPTION_TYPE_MIN           ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED
#define TEST_ROUTE_EXCEPTION_TYPE_MAX           (ROUTE_EXCEPTION_TYPE_END-1)
/*routeExceptionAction*/
#define TEST_ROUTE_EXCEPTION_ACT_MIN            ACTION_FORWARD
#define TEST_ROUTE_EXCEPTION_ACT_MAX            ACTION_TRAP2CPU
/* rmaGroup_frameType */
#define TEST_RMAGROUP_FRAMETYPE_MIN             RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM
#define TEST_RMAGROUP_FRAMETYPE_MAX             (RMA_GROUP_TYPE_END-1)

int32 dal_trap_rmaAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mac_t  macAddr_temp;
    rtk_mgmt_action_t  action_temp = ACTION_FORWARD;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaAction_set(unit, &macAddr_temp, action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaAction_get(unit, &macAddr_temp, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_mac_t macAddr;
        rtk_mgmt_action_t  action;
        rtk_mgmt_action_t  action_result;
        int32 randIdx;

        for (randIdx = 0; randIdx <=TRAP_RANDOM_RUN_TIMES; randIdx++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            macAddr.octet[0]= 1;
            macAddr.octet[1]= 0x80;
            macAddr.octet[2]= 0xc2;
            macAddr.octet[3]= 0;
            macAddr.octet[4]= 0;
            macAddr.octet[5]= ut_rand() % (0x2f + 10);

            if(macAddr.octet[5] > 0x2f)
                expect_result = RT_ERR_FAILED;

            if((macAddr.octet[5] == 0) || (macAddr.octet[5] == 3))
                continue;
            action = ut_rand() % (ACTION_END + 1);
            if((HWP_8390_50_FAMILY(unit))
                && (action != MGMT_ACTION_FORWARD
                || action != MGMT_ACTION_DROP
                || action != MGMT_ACTION_TRAP2CPU))
                expect_result = RT_ERR_FAILED;
            if((HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit))
                && (action != MGMT_ACTION_FORWARD
                || action != MGMT_ACTION_DROP
                || action != MGMT_ACTION_TRAP2CPU
                || action != MGMT_ACTION_TRAP2MASTERCPU))
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_rmaAction_set(unit, &macAddr, action);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaAction_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, action %u)"
                                , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                    macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], action);

                ret = rtk_trap_rmaAction_get(unit, &macAddr, &action_result);
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaAction_get (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, action %u)"
                                 , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                    macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaAction_set/get value unequal (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, action %u)"
                                 , action_result, action, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                    macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], action);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaAction_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, action %u)"
                                    , ret, RT_ERR_OK, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                        macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], action);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trap_rmaAction_test*/

int32 dal_trap_userDefineRma_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_trap_userDefinedRma_t  entryContent_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRma_set(unit, 0, &entryContent_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRma_get(unit, 0, &entryContent_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  entryIdx[UNITTEST_MAX_TEST_VALUE];
        int32  entryIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  entryIdx_index;
        int32  entryIdx_last;

        rtk_trap_userDefinedRma_t  entry;
        rtk_trap_userDefinedRma_t  result_entry;

        int32 randIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_UDL2RMA_ID_MAX(unit), TEST_UDL2RMA_ID_MIN, entryIdx, entryIdx_result, entryIdx_last);

        for (entryIdx_index = 0; entryIdx_index <= entryIdx_last; entryIdx_index++)
        {
            inter_result2 = entryIdx_result[entryIdx_index];

            for (randIdx = 0; randIdx <= TRAP_RANDOM_RUN_TIMES; randIdx++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&entry, 0, sizeof(rtk_trap_userDefinedRma_t));

                entry.mac.octet[0] = ut_rand() & BITMASK_8B;
                entry.mac.octet[1] = ut_rand() & BITMASK_8B;
                entry.mac.octet[2] = ut_rand() & BITMASK_8B;
                entry.mac.octet[3] = ut_rand() & BITMASK_8B;
                entry.mac.octet[4] = ut_rand() & BITMASK_8B;
                entry.mac.octet[5] = ut_rand() & BITMASK_8B;

                if ((HWP_8380_30_FAMILY(unit)) && entryIdx[entryIdx_index] >= 2)
                {
                    entry.mac.octet[0] = 0x01;
                    entry.mac.octet[1] = 0x80;
                    entry.mac.octet[2] = 0xC2;
                    entry.mac.octet[3] = 0x00;
                    entry.mac.octet[4] = 0x00;
                }


                ret = rtk_trap_userDefineRma_set(unit, entryIdx[entryIdx_index], (rtk_trap_userDefinedRma_t *)&entry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRma_set (unit %u, entryIdx %u)"
                                    , ret, expect_result, unit, entryIdx[entryIdx_index]);

                    ret = rtk_trap_userDefineRma_get(unit, entryIdx[entryIdx_index], (rtk_trap_userDefinedRma_t*)&result_entry);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRma_get (unit %u, entryIdx %u)"
                                     , ret, expect_result, unit, entryIdx[entryIdx_index]);

                    ret = osal_memcmp(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRma_get/get mac unequal (unit %u, entryIdx %u)"
                               , ret, 0, unit, entryIdx[entryIdx_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRma_set (unit %u, entryIdx %u)"
                                        , ret, RT_ERR_OK, unit, entryIdx[entryIdx_index]);
                }

            }

            ret = rtk_trap_userDefineRma_get(unit, entryIdx[entryIdx_index], (rtk_trap_userDefinedRma_t*)&result_entry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRma_set (unit %u, entryIdx %u)"
                                , ret, expect_result, unit, entryIdx[entryIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRma_set (unit %u, entryIdx %u)"
                                    , ret, RT_ERR_OK, unit, entryIdx[entryIdx_index]);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_trap_userDefineRma_test*/

int32 dal_trap_userDefineRmaAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mgmt_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_mgmt_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_mgmt_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_UDL2RMA_ID_MAX(unit), TEST_UDL2RMA_ID_MIN, entry, entry_result, entry_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if ((HWP_8390_50_FAMILY(unit) || HWP_8380_30_FAMILY(unit)) &&
                    (action[action_index] == MGMT_ACTION_COPY2CPU
                    || action[action_index] == MGMT_ACTION_TRAP2MASTERCPU
                    || action[action_index] == MGMT_ACTION_COPY2MASTERCPU) )
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit)) && action[action_index] == MGMT_ACTION_COPY2MASTERCPU)
                    expect_result = RT_ERR_FAILED;
                ret = rtk_trap_userDefineRmaAction_set(unit, entry[entry_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u, action %u)"
                                    , ret, expect_result, unit, entry[entry_index], action[action_index]);

                    ret = rtk_trap_userDefineRmaAction_get(unit, entry[entry_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_get (unit %u, entry %u, action %u)"
                                     , ret, expect_result, unit, entry[entry_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_set/get value unequal (unit %u, entry %u, action %u)"
                                     , result_action, action[action_index], unit, entry[entry_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u, action %u)"
                                        , ret, RT_ERR_OK, unit, entry[entry_index], action[action_index]);
                }
            }

            ret = rtk_trap_userDefineRmaAction_get(unit, entry[entry_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  entry;
        uint32  entry_max;
        rtk_mgmt_action_t  action;
        rtk_mgmt_action_t  action_max;
        rtk_mgmt_action_t  result_action;

        G_VALUE(TEST_UDL2RMA_ID_MAX(unit), entry_max);
        G_VALUE(TEST_RMAACT_ID_MAX, action_max);

        for (L_VALUE(TEST_UDL2RMA_ID_MIN, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_UDL2RMA_ID_MIN, TEST_UDL2RMA_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_MGMT_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_MGMT_ACT_ID_MIN, TEST_MGMT_ACT_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if ((HWP_8390_50_FAMILY(unit) || HWP_8380_30_FAMILY(unit)) &&
                    (action == MGMT_ACTION_COPY2CPU
                    || action == MGMT_ACTION_TRAP2MASTERCPU
                    || action == MGMT_ACTION_COPY2MASTERCPU) )
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9310_FAMILY_ID(unit) || HWP_9300_FAMILY_ID(unit)) && action == MGMT_ACTION_COPY2MASTERCPU)
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_userDefineRmaAction_set(unit, entry, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u, action %u)"
                                    , ret, expect_result, unit, entry, action);
                    ret = rtk_trap_userDefineRmaAction_get(unit, entry, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_get (unit %u, entry %u, action %u)"
                                     , ret, expect_result, unit, entry, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaAction_set/get value unequal (unit %u, entry %u, action %u)"
                                     , result_action, action, unit, entry, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaAction_set (unit %u, entry %u, action %u)"
                                        , ret, RT_ERR_OK, unit, entry, action);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_userDefineRmaAction_test*/

int32 dal_trap_mgmtFrameAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mgmt_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    /* Use MGMT_TYPE_ARP to check due to each chip have supported it!! */
    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameAction_set(unit, TEST_MGMTYPE_ID_MIN, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameAction_get(unit, TEST_MGMTYPE_ID_MIN, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_mgmt_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_mgmt_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MGMTYPE_ID_MAX, TEST_MGMTYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if(HWP_8390_50_FAMILY(unit)){
                    if (action[action_index] > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_MLD) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IGMP) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_EAPOL) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_ARP) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6ND) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_SELFMAC) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_DROP) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6_HOP_POS_ERR) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_L2_CRC_ERR) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_DROP) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IP4_CHKSUM_ERR) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_DROP) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] != MGMT_TYPE_MLD) && (type[type_index] != MGMT_TYPE_IGMP) &&
                        (type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_ARP) &&
                        (type[type_index] != MGMT_TYPE_IPV6ND) && (type[type_index] != MGMT_TYPE_SELFMAC) &&
                        (type[type_index] != MGMT_TYPE_IPV6_HOP_POS_ERR) && (type[type_index] != MGMT_TYPE_IPV6_HDR_UNKWN) &&
                        (type[type_index] != MGMT_TYPE_L2_CRC_ERR) && (type[type_index] != MGMT_TYPE_IP4_CHKSUM_ERR))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_8380_30_FAMILY(unit))
                {
                    if (action[action_index] > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_MLD) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IGMP) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_EAPOL) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_ARP) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6ND) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6_HOP_POS_ERR) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_SELFMAC) && ((action[action_index] != MGMT_ACTION_FORWARD) && (action[action_index] != MGMT_ACTION_DROP) && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] != MGMT_TYPE_MLD) && (type[type_index] != MGMT_TYPE_IGMP) &&
                        (type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_ARP) &&
                        (type[type_index] != MGMT_TYPE_IPV6ND) && (type[type_index] != MGMT_TYPE_IPV6_HOP_POS_ERR) &&
                        (type[type_index] != MGMT_TYPE_IPV6_HDR_UNKWN) && (type[type_index] != MGMT_TYPE_SELFMAC))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_9300_FAMILY_ID(unit))
                {
                    if (action[action_index] > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_L2_CRC_ERR || type[type_index] == MGMT_TYPE_IP4_CHKSUM_ERR)
                        && ((action[action_index] != MGMT_ACTION_FORWARD)
                        && (action[action_index] != MGMT_ACTION_DROP)
                        && (action[action_index] != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] != MGMT_TYPE_L2_CRC_ERR || type[type_index] != MGMT_TYPE_IP4_CHKSUM_ERR)
                        && ((action[action_index] != MGMT_ACTION_FORWARD)
                        && (action[action_index] != MGMT_ACTION_TRAP2CPU)
                        && (action[action_index] != MGMT_ACTION_TRAP2MASTERCPU)))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_9310_FAMILY_ID(unit))
                {
                    if (action[action_index] > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] == MGMT_TYPE_L2_CRC_ERR)
                        && ((action[action_index] != MGMT_ACTION_FORWARD)
                        || (action[action_index] != MGMT_ACTION_TRAP2CPU)
                        || (action[action_index] != MGMT_ACTION_DROP)))
                        expect_result = RT_ERR_FAILED;
                    if ((type[type_index] != MGMT_TYPE_L2_CRC_ERR)
                        && ((action[action_index] == MGMT_ACTION_DROP)
                        || (action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT)))
                        expect_result = RT_ERR_FAILED;

                }
                ret = rtk_trap_mgmtFrameAction_set(unit, type[type_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type[type_index], action[action_index]);

                    ret = rtk_trap_mgmtFrameAction_get(unit, type[type_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type[type_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action[action_index], unit, type[type_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], action[action_index]);
                }
            }

            ret = rtk_trap_mgmtFrameAction_get(unit, type[type_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((HWP_8390_50_FAMILY(unit)) &&
                (type[type_index] != MGMT_TYPE_MLD) && (type[type_index] != MGMT_TYPE_IGMP) &&
                (type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_ARP) &&
                (type[type_index] != MGMT_TYPE_IPV6ND) && (type[type_index] != MGMT_TYPE_SELFMAC) &&
                (type[type_index] != MGMT_TYPE_IPV6_HOP_POS_ERR) && (type[type_index] != MGMT_TYPE_IPV6_HDR_UNKWN) &&
                (type[type_index] != MGMT_TYPE_L2_CRC_ERR) && (type[type_index] != MGMT_TYPE_IP4_CHKSUM_ERR))
                expect_result = RT_ERR_FAILED;
            if ((HWP_8380_30_FAMILY(unit)) &&
                (type[type_index] != MGMT_TYPE_MLD) && (type[type_index] != MGMT_TYPE_IGMP) &&
                (type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_ARP) &&
                (type[type_index] != MGMT_TYPE_IPV6ND) && (type[type_index] != MGMT_TYPE_IPV6_HOP_POS_ERR) &&
                (type[type_index] != MGMT_TYPE_IPV6_HDR_UNKWN) && (type[type_index] != MGMT_TYPE_SELFMAC))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type;
        rtk_trap_mgmtType_t  type_max;
        rtk_mgmt_action_t  action;
        rtk_mgmt_action_t  action_max;
        rtk_mgmt_action_t  result_action;

        G_VALUE(TEST_MGMTYPE_ID_MAX, type_max);
        G_VALUE(TEST_RMAACT_ID_MAX, action_max);

        for (L_VALUE(TEST_MGMTYPE_ID_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_MGMTYPE_ID_MIN, TEST_MGMTYPE_ID_MAX, inter_result2);

            for (L_VALUE(TEST_MGMT_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_MGMT_ACT_ID_MIN, TEST_MGMT_ACT_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (action > MGMT_ACTION_COPY2CPU)
                    expect_result = RT_ERR_FAILED;
                if ((type == MGMT_TYPE_MLD) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                    expect_result = RT_ERR_FAILED;
                if ((type == MGMT_TYPE_IGMP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                    expect_result = RT_ERR_FAILED;
                if ((type == MGMT_TYPE_EAPOL) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                    expect_result = RT_ERR_FAILED;
                if ((type == MGMT_TYPE_ARP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                    expect_result = RT_ERR_FAILED;
                if ((type == MGMT_TYPE_IPV6ND) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                    expect_result = RT_ERR_FAILED;
                               if(HWP_8390_50_FAMILY(unit)){
                    if (action > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_MLD) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IGMP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_EAPOL) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_ARP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6ND) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_SELFMAC) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6_HOP_POS_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_L2_CRC_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IP4_CHKSUM_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type != MGMT_TYPE_MLD) && (type != MGMT_TYPE_IGMP) &&
                        (type != MGMT_TYPE_EAPOL) && (type != MGMT_TYPE_ARP) &&
                        (type != MGMT_TYPE_IPV6ND) && (type != MGMT_TYPE_SELFMAC) &&
                        (type != MGMT_TYPE_IPV6_HOP_POS_ERR) && (type != MGMT_TYPE_IPV6_HDR_UNKWN) &&
                        (type != MGMT_TYPE_L2_CRC_ERR) && (type != MGMT_TYPE_IP4_CHKSUM_ERR))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_8380_30_FAMILY(unit))
                {
                    if (action > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_MLD) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IGMP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_EAPOL) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_ARP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6ND) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6_HOP_POS_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_SELFMAC) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type != MGMT_TYPE_MLD) && (type != MGMT_TYPE_IGMP) &&
                        (type != MGMT_TYPE_EAPOL) && (type != MGMT_TYPE_ARP) &&
                        (type != MGMT_TYPE_IPV6ND) && (type != MGMT_TYPE_IPV6_HOP_POS_ERR) &&
                        (type != MGMT_TYPE_IPV6_HDR_UNKWN) && (type != MGMT_TYPE_SELFMAC))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_9300_FAMILY_ID(unit))
                {
                    if (action > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_L2_CRC_ERR || type == MGMT_TYPE_IP4_CHKSUM_ERR)
                        && ((action != MGMT_ACTION_FORWARD)
                        && (action != MGMT_ACTION_DROP)
                        && (action != MGMT_ACTION_TRAP2CPU)))
                        expect_result = RT_ERR_FAILED;
                    if ((type != MGMT_TYPE_L2_CRC_ERR || type != MGMT_TYPE_IP4_CHKSUM_ERR)
                        && ((action != MGMT_ACTION_FORWARD)
                        && (action != MGMT_ACTION_TRAP2CPU)
                        && (action != MGMT_ACTION_TRAP2MASTERCPU)))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_9310_FAMILY_ID(unit))
                {
                    if (action > MGMT_ACTION_COPY2CPU)
                        expect_result = RT_ERR_FAILED;
                    if ((type == MGMT_TYPE_L2_CRC_ERR)
                        && ((action != MGMT_ACTION_FORWARD)
                        || (action != MGMT_ACTION_TRAP2CPU)
                        || (action != MGMT_ACTION_DROP)))
                        expect_result = RT_ERR_FAILED;
                    if ((type != MGMT_TYPE_L2_CRC_ERR)
                        && ((action == MGMT_ACTION_DROP)
                        || (action == MGMT_ACTION_FLOOD_TO_ALL_PORT)))
                        expect_result = RT_ERR_FAILED;

                }
                ret = rtk_trap_mgmtFrameAction_set(unit, type, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type, action);
                    ret = rtk_trap_mgmtFrameAction_get(unit, type, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action, unit, type, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type, action);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_mgmtFrameAction_test*/

int32 dal_trap_mgmtFramePri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  pri_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFramePri_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFramePri_get(unit, 0, &pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;

        int32  result_pri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MGMTYPE_ID_MAX, TEST_MGMTYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_ID_MAX, TEST_PRI_ID_MIN, pri, pri_result, pri_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (pri_index = 0; pri_index <= pri_last; pri_index++)
            {
                inter_result3 = pri_result[pri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_trap_mgmtFramePri_set(unit, type[type_index], pri[pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u, pri %u)"
                                    , ret, expect_result, unit, type[type_index], pri[pri_index]);

                    ret = rtk_trap_mgmtFramePri_get(unit, type[type_index], (rtk_pri_t*)&result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_get (unit %u, type %u, pri %u)"
                                     , ret, expect_result, unit, type[type_index], pri[pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_set/get value unequal (unit %u, type %u, pri %u)"
                                     , result_pri, pri[pri_index], unit, type[type_index], pri[pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], pri[pri_index]);
                }
            }

            ret = rtk_trap_mgmtFramePri_get(unit, type[type_index], (rtk_pri_t*)&result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (HWP_8380_30_FAMILY(unit))
                {
                    if((type[type_index] != MGMT_TYPE_OTHER)
                        && (type[type_index] != MGMT_TYPE_IGR_VLAN_FLTR)
                        && (type[type_index] != MGMT_TYPE_CFI)
                        && (type[type_index] != MGMT_TYPE_RLDP_RLPP)
                        && (type[type_index] != MGMT_TYPE_MAC_CST_SYS)
                        && (type[type_index] != MGMT_TYPE_MAC_CST_PORT)
                        && (type[type_index] != MGMT_TYPE_MAC_CST_VLAN)
                        && (type[type_index] != MGMT_TYPE_RMA)
                        && (type[type_index] != MGMT_TYPE_SELFMAC)
                        && (type[type_index] != MGMT_TYPE_UNKNOWN_DA)
                        && (type[type_index] != MGMT_TYPE_SPECIAL_COPY)
                        && (type[type_index] != MGMT_TYPE_SPECIAL_TRAP)
                        && (type[type_index] != MGMT_TYPE_ROUT_EXCEPT))
                            expect_result = RT_ERR_FAILED;
                }

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type;
        rtk_trap_mgmtType_t  type_max;
        rtk_pri_t  pri;
        rtk_pri_t  pri_max;
        rtk_pri_t  result_pri;

        G_VALUE(TEST_MGMTYPE_ID_MAX, type_max);
        G_VALUE(TEST_PRI_ID_MAX, pri_max);

        for (L_VALUE(TEST_MGMTYPE_ID_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_MGMTYPE_ID_MIN, TEST_MGMTYPE_ID_MAX, inter_result2);

            for (L_VALUE(TEST_PRI_ID_MIN, pri); pri <= pri_max; pri++)
            {
                ASSIGN_EXPECT_RESULT(pri, TEST_PRI_ID_MIN, TEST_PRI_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_trap_mgmtFramePri_set(unit, type, pri);
                if (HWP_8380_30_FAMILY(unit))
                    {
                        if((type != MGMT_TYPE_OTHER)
                            && (type != MGMT_TYPE_IGR_VLAN_FLTR)
                            && (type != MGMT_TYPE_CFI)
                            && (type != MGMT_TYPE_RLDP_RLPP)
                            && (type != MGMT_TYPE_MAC_CST_SYS)
                            && (type != MGMT_TYPE_MAC_CST_PORT)
                            && (type != MGMT_TYPE_MAC_CST_VLAN)
                            && (type != MGMT_TYPE_RMA)
                            && (type != MGMT_TYPE_SELFMAC)
                            && (type != MGMT_TYPE_UNKNOWN_DA)
                            && (type != MGMT_TYPE_SPECIAL_COPY)
                            && (type != MGMT_TYPE_SPECIAL_TRAP)
                            && (type != MGMT_TYPE_ROUT_EXCEPT))
                                expect_result = RT_ERR_FAILED;
                    }
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u, pri %u)"
                                    , ret, expect_result, unit, type, pri);
                    ret = rtk_trap_mgmtFramePri_get(unit, type, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_get (unit %u, type %u, pri %u)"
                                     , ret, expect_result, unit, type, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFramePri_set/get value unequal (unit %u, type %u, pri %u)"
                                     , result_pri, pri, unit, type, pri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFramePri_set (unit %u, type %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, type, pri);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_mgmtFramePri_test*/

int32 dal_trap_portMgmtFrameAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_trap_mgmtType_t  type_temp = 0;
    rtk_mgmt_action_t    fwdAction_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_portMgmtFrameAction_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_portMgmtFrameAction_get(unit, 0, type_temp, &fwdAction_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_trap_mgmtType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_mgmt_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_mgmt_action_t         result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PMGMTYPE_ID_MAX, TEST_PMGMTYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RTK_ACT_ID_MAX, TEST_RTK_ACT_ID_MIN, action, action_result, action_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (type_index = 0; type_index <= type_last; type_index++)
            {
                inter_result3 = type_result[type_index];

                 for (action_index = 0; action_index <= action_last; action_index++)
                 {
                    inter_result4 = action_result[action_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        if(action[action_index] >= MGMT_ACTION_COPY2CPU && action[action_index] != MGMT_ACTION_FLOOD_TO_ALL_PORT)
                            expect_result = RT_ERR_FAILED;
                        if(action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT && type[type_index] == MGMT_TYPE_PTP)
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] != MGMT_TYPE_PTP) && (type[type_index] != MGMT_TYPE_LLDP) && (type[type_index] != MGMT_TYPE_BPDU))
                            expect_result = RT_ERR_FAILED;
                    }
                    if(HWP_8380_30_FAMILY(unit))
                    {
                        if(action[action_index] >= MGMT_ACTION_END)
                            expect_result = RT_ERR_FAILED;
                        if((action[action_index] > MGMT_ACTION_TRAP2CPU) && (action[action_index] != MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] != MGMT_TYPE_PTP) && (type[type_index] != MGMT_TYPE_LLDP) && (type[type_index] != MGMT_TYPE_BPDU))
                            expect_result = RT_ERR_FAILED;
                    }
                    if(HWP_9310_FAMILY_ID(unit))
                    {
                        if((type[type_index] == MGMT_TYPE_PTP || type[type_index] == MGMT_TYPE_PTP_UDP || type[type_index] == MGMT_TYPE_PTP_ETH2)
                            &&(action[action_index] == MGMT_ACTION_COPY2CPU
                            || action[action_index] == MGMT_ACTION_COPY2MASTERCPU
                            || action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] == MGMT_TYPE_BPDU || type[type_index] == MGMT_TYPE_LLDP || type[type_index] == MGMT_TYPE_EAPOL)
                            &&(action[action_index] == MGMT_ACTION_COPY2CPU
                            || action[action_index] == MGMT_ACTION_COPY2MASTERCPU))
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] == MGMT_GRATUITOUS_ARP)
                            &&(action[action_index] == MGMT_ACTION_COPY2MASTERCPU
                            || action[action_index] == MGMT_ACTION_TRAP2MASTERCPU
                            || action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                    }
                    if(HWP_9300_FAMILY_ID(unit))
                    {
                        if((type[type_index] == MGMT_TYPE_PTP)&&(action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] == MGMT_TYPE_PTP_UDP)&&(action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                        if((type[type_index] == MGMT_TYPE_PTP_ETH2)&&(action[action_index] == MGMT_ACTION_FLOOD_TO_ALL_PORT))
                            expect_result = RT_ERR_FAILED;
                        if((action[action_index] == MGMT_ACTION_COPY2MASTERCPU))
                            expect_result = RT_ERR_FAILED;

                    }
                    ret = rtk_trap_portMgmtFrameAction_set(unit, port[port_index], type[type_index], action[action_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_trap_portMgmtFrameAction_set (unit %u, port %u, type %u, action %u)"
                                        , ret, expect_result, unit, port[port_index], type[type_index], action[action_index]);

                        ret = rtk_trap_portMgmtFrameAction_get(unit, port[port_index], type[type_index],&result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_trap_portMgmtFrameAction_get (unit %u, port %u, type %u, action %u)"
                                         , ret, expect_result, unit, port[port_index], type[type_index], action[action_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_trap_portMgmtFrameAction_set/get value unequal (unit %u, port %u, type %u, action %u)"
                                         , result_action, action[action_index], unit, port[port_index], type[type_index], action[action_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_portMgmtFrameAction_set (unit %u, port %u, type %u, action %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], type[type_index], action[action_index]);
                    }
                }

                ret = rtk_trap_portMgmtFrameAction_get(unit, port[port_index], type[type_index], &result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if(HWP_8390_50_FAMILY(unit) || HWP_8380_30_FAMILY(unit))
                {
                    if((type[type_index] != MGMT_TYPE_PTP) && (type[type_index] != MGMT_TYPE_LLDP) && (type[type_index] != MGMT_TYPE_BPDU))
                        expect_result = RT_ERR_FAILED;
                }
                if(HWP_9310_FAMILY_ID(unit))
                {
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
                    if((type[type_index] != MGMT_TYPE_PTP) && (type[type_index] != MGMT_TYPE_PTP_UDP) && (type[type_index] != MGMT_TYPE_BPDU))
                        expect_result = RT_ERR_FAILED;
#endif
                    if((type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_LLDP) && (type[type_index] != MGMT_TYPE_PTP_ETH2))
                        expect_result = RT_ERR_FAILED;

                }
                if(HWP_9300_FAMILY_ID(unit))
                {
                    if((type[type_index] != MGMT_TYPE_EAPOL) && (type[type_index] != MGMT_TYPE_PTP_UDP) && (type[type_index] != MGMT_TYPE_BPDU)
                        && (type[type_index] != MGMT_TYPE_PTP) && (type[type_index] != MGMT_GRATUITOUS_ARP))
                        expect_result = RT_ERR_FAILED;

                }

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portMgmtFrameAction_get (unit %u, port %u, type %d)"
                                    , ret, expect_result, unit, port[port_index], type[type_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_portMgmtFrameAction_get (unit %u, port %u, type %d)"
                                        , ret, RT_ERR_OK, unit, port[port_index], type[type_index]);
                }

            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_portMgmtFrameAction_test*/

int32 dal_trap_pktWithCFIAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithCFIAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithCFIAction_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if(action[action_index] > ACTION_TRAP2CPU)
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_pktWithCFIAction_set(unit, action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_trap_pktWithCFIAction_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_trap_pktWithCFIAction_get(unit, (rtk_action_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_RMAACT_ID_MAX, action_max);

        for (L_VALUE(TEST_RMAACT_ID_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_RMAACT_ID_MIN, TEST_RMAACT_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if(action > ACTION_TRAP2CPU)
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_pktWithCFIAction_set(unit, action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_trap_pktWithCFIAction_get(unit, &result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_pktWithCFIAction_test*/


int32 dal_trap_pktWithCFIPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  pri_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithCFIPri_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithCFIPri_get(unit, &pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;
        int32  result_pri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_ID_MAX, TEST_PRI_ID_MIN, pri, pri_result, pri_last);

        for (pri_index = 0; pri_index <= pri_last; pri_index++)
        {
            inter_result2 = pri_result[pri_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_pktWithCFIPri_set(unit, pri[pri_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri[pri_index]);

                ret = rtk_trap_pktWithCFIPri_get(unit, (rtk_pri_t*)&result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri[pri_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri[pri_index], unit, pri[pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri[pri_index]);
            }
        }

        ret = rtk_trap_pktWithCFIPri_get(unit, (rtk_pri_t*)&result_pri);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri;
        rtk_pri_t  pri_max;
        rtk_pri_t  result_pri;

        G_VALUE(TEST_PRI_ID_MAX, pri_max);

        for (L_VALUE(TEST_PRI_ID_MIN, pri); pri <= pri_max; pri++)
        {
            ASSIGN_EXPECT_RESULT(pri, TEST_PRI_ID_MIN, TEST_PRI_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_pktWithCFIPri_set(unit, pri);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri);
                ret = rtk_trap_pktWithCFIPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithCFIPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri, unit, pri);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithCFIPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_pktWithCFIPri_test*/

int32 dal_trap_cfmFrameTrapPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  pri_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmFrameTrapPri_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmFrameTrapPri_get(unit,&pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;
        int32  result_pri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_ID_MAX, TEST_PRI_ID_MIN, pri, pri_result, pri_last);

        for (pri_index = 0; pri_index <= pri_last; pri_index++)
        {
            inter_result2 = pri_result[pri_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 )? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_cfmFrameTrapPri_set(unit, pri[pri_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri[pri_index]);

                ret = rtk_trap_cfmFrameTrapPri_get(unit, (rtk_pri_t*)&result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri[pri_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri[pri_index], unit, pri[pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri[pri_index]);
            }
        }

        ret = rtk_trap_cfmFrameTrapPri_get(unit, (rtk_pri_t*)&result_pri);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri;
        rtk_pri_t  pri_max;
        rtk_pri_t  result_pri;


        G_VALUE(TEST_PRI_ID_MAX, pri_max);

        for (L_VALUE(TEST_PRI_ID_MIN, pri); pri <= pri_max; pri++)
        {
            ASSIGN_EXPECT_RESULT(pri, TEST_PRI_ID_MIN, TEST_PRI_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = ( inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_cfmFrameTrapPri_set(unit, pri);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri);
                ret = rtk_trap_cfmFrameTrapPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri, unit, pri);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmFrameTrapPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_cfmFrameTrapPri_test*/

int32 dal_trap_oamPDUAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_oamPDUAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_oamPDUAction_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX - 1, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((action[action_index] != ACTION_FORWARD) && (action[action_index] != ACTION_DROP) && (action[action_index] != ACTION_TRAP2CPU))
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_oamPDUAction_set(unit, action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_trap_oamPDUAction_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_trap_oamPDUAction_get(unit, (rtk_action_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_RMAACT_ID_MAX - 1, action_max);

        for (L_VALUE(TEST_RMAACT_ID_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_RMAACT_ID_MIN, TEST_RMAACT_ID_MAX - 1, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((action != ACTION_FORWARD) && (action != ACTION_DROP) && (action != ACTION_TRAP2CPU))
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_oamPDUAction_set(unit, action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_trap_oamPDUAction_get(unit, &result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_oamPDUAction_test*/

int32 dal_trap_oamPDUPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  pri_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_oamPDUPri_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_oamPDUPri_get(unit, &pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;
        int32  result_pri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_ID_MAX, TEST_PRI_ID_MIN, pri, pri_result, pri_last);

        for (pri_index = 0; pri_index <= pri_last; pri_index++)
        {
            inter_result2 = pri_result[pri_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_oamPDUPri_set(unit, pri[pri_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri[pri_index]);

                ret = rtk_trap_oamPDUPri_get(unit, (rtk_pri_t*)&result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri[pri_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri[pri_index], unit, pri[pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri[pri_index]);
            }
        }

        ret = rtk_trap_oamPDUPri_get(unit, (rtk_pri_t*)&result_pri);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri;
        rtk_pri_t  pri_max;
        rtk_pri_t  result_pri;

        G_VALUE(TEST_PRI_ID_MAX, pri_max);

        for (L_VALUE(TEST_PRI_ID_MIN, pri); pri <= pri_max; pri++)
        {
            ASSIGN_EXPECT_RESULT(pri, TEST_PRI_ID_MIN, TEST_PRI_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_oamPDUPri_set(unit, pri);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri);
                ret = rtk_trap_oamPDUPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri);
                RT_TEST_IS_EQUAL_INT("rtk_trap_oamPDUPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri, unit, pri);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_oamPDUPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_oamPDUPri_test*/

int32 dal_trap_rmaGroupAction_test(uint32 caseNo, uint32 unit)
{   /* Applicable to 8380 */
    int32 ret;
    rtk_mgmt_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaGroupAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaGroupAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_rmaGroup_frameType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_mgmt_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_mgmt_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAGROUP_FRAMETYPE_MAX, TEST_RMAGROUP_FRAMETYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8380_30_FAMILY(unit)) && (action[action_index] > MGMT_ACTION_TRAP2CPU) && (action[action_index] != MGMT_ACTION_FLOOD_TO_ALL_PORT))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_rmaGroupAction_set(unit, type[type_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type[type_index], action[action_index]);

                    ret = rtk_trap_rmaGroupAction_get(unit, type[type_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type[type_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action[action_index], unit, type[type_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], action[action_index]);
                }
            }

            ret = rtk_trap_rmaGroupAction_get(unit, type[type_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_rmaGroup_frameType_t  type;
        rtk_trap_rmaGroup_frameType_t  type_max;
        rtk_mgmt_action_t  action;
        rtk_mgmt_action_t  action_max;
        rtk_mgmt_action_t  result_action;

        G_VALUE(TEST_RMAGROUP_FRAMETYPE_MAX, type_max);
        G_VALUE(TEST_RMAACT_ID_MAX, action_max);

        for (L_VALUE(TEST_RMAGROUP_FRAMETYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_RMAGROUP_FRAMETYPE_MIN, TEST_RMAGROUP_FRAMETYPE_MAX, inter_result2);

            for (L_VALUE(TEST_RMAACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_MGMT_ACT_ID_MIN, TEST_MGMT_ACT_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8380_30_FAMILY(unit)) && (action > MGMT_ACTION_TRAP2CPU) && (action != MGMT_ACTION_FLOOD_TO_ALL_PORT))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_rmaGroupAction_set(unit, type, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type, action);
                    ret = rtk_trap_rmaGroupAction_get(unit, type, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action, unit, type, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type, action);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_rmaGroupAction_test*/

int32 dal_trap_rmaLearningEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mac_t  macAddr_temp;
    rtk_enable_t  enable_temp;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaLearningEnable_set(unit, &macAddr_temp, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaLearningEnable_get(unit, &macAddr_temp, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  mac5[UNITTEST_MAX_TEST_VALUE];
        int32  mac5_result[UNITTEST_MAX_TEST_VALUE];
        int32  mac5_index;
        int32  mac5_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t result_enable;
        rtk_mac_t macAddr;

        UNITTEST_TEST_VALUE_ASSIGN(0x2f, 0, mac5, mac5_result, mac5_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (mac5_index = 0; mac5_index <= mac5_last; mac5_index++)
        {
            macAddr.octet[0]= 1;
            macAddr.octet[1]= 0x80;
            macAddr.octet[2]= 0xc2;
            macAddr.octet[3]= 0;
            macAddr.octet[4]= 0;
            macAddr.octet[5]= mac5[mac5_index];

            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (mac5_result[mac5_index] && enable_result[enable_index]) ? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_rmaLearningEnable_set(unit, &macAddr, enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                    , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                        macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable[enable_index]);

                    ret = rtk_trap_rmaLearningEnable_get(unit, &macAddr, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_get (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                     , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                        macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_set/get value unequal (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                     , result_enable, enable[enable_index], unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                        macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                        , ret, RT_ERR_OK, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                            macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable[enable_index]);
                }
            }

            ret = rtk_trap_rmaLearningEnable_get(unit, &macAddr, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (mac5_result[mac5_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x)"
                                , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                            macAddr.octet[3], macAddr.octet[4], macAddr.octet[5]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x)"
                                    , ret, RT_ERR_OK, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                            macAddr.octet[3], macAddr.octet[4], macAddr.octet[5]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  mac5;
        uint32  mac5_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;
        rtk_mac_t macAddr;

        G_VALUE(TEST_RMA_MAC5_MAX, mac5_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_RMA_MAC5_MIN, mac5); mac5 <= mac5_max; mac5++)
        {
            ASSIGN_EXPECT_RESULT(mac5, TEST_RMA_MAC5_MIN, TEST_RMA_MAC5_MAX, inter_result2);
            macAddr.octet[0]= 1;
            macAddr.octet[1]= 0x80;
            macAddr.octet[2]= 0xc2;
            macAddr.octet[3]= 0;
            macAddr.octet[4]= 0;
            macAddr.octet[5]= mac5;

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_rmaLearningEnable_set(unit, &macAddr, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                    , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                    macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable);
                    ret = rtk_trap_rmaLearningEnable_get(unit, &macAddr, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_get (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                     , ret, expect_result, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                     macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaLearningEnable_set/get value unequal (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                     , result_enable, enable, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                     macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaLearningEnable_set (unit %u, macAddr 0x%02x:%02x:%02x:%02x:%02x:%02x, enable %u)"
                                        , ret, RT_ERR_OK, unit, macAddr.octet[0], macAddr.octet[1], macAddr.octet[2],
                                        macAddr.octet[3], macAddr.octet[4], macAddr.octet[5], enable);
            }

        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_rmaGroupLearningEnable_test(uint32 caseNo, uint32 unit)
{
    /* Applicable to 8380 */
    int32 ret;
    rtk_enable_t    enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaGroupLearningEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_rmaGroupLearningEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_rmaGroup_frameType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAGROUP_FRAMETYPE_MAX, TEST_RMAGROUP_FRAMETYPE_MIN, type, type_result, type_last);
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
                ret = rtk_trap_rmaGroupLearningEnable_set(unit, type[type_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type[type_index], enable[enable_index]);

                    ret = rtk_trap_rmaGroupLearningEnable_get(unit, type[type_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_get (unit %u, type %u, enable %u)"
                                     , ret, expect_result, unit, type[type_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set/get value unequal (unit %u, type %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, type[type_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_rmaGroupLearningEnable_get(unit, type[type_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_rmaGroup_frameType_t  type;
        rtk_trap_rmaGroup_frameType_t  type_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_RMAGROUP_FRAMETYPE_MAX, type_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_RMAGROUP_FRAMETYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_RMAGROUP_FRAMETYPE_MIN, TEST_RMAGROUP_FRAMETYPE_MAX, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_rmaGroupLearningEnable_set(unit, type, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type, enable);
                    ret = rtk_trap_rmaGroupLearningEnable_get(unit, type, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_get (unit %u, type %u, enable %u)"
                                     , ret, expect_result, unit, type, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set/get value unequal (unit %u, type %u, enable %u)"
                                     , result_enable, enable, unit, type, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_rmaGroupLearningEnable_set (unit %u, type %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, type, enable);
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_trap_bypassStp_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_bypassStp_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_bypassStp_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_bypassStpType_t  bypassStpType[UNITTEST_MAX_TEST_VALUE];
        int32  bypassStpType_result[UNITTEST_MAX_TEST_VALUE];
        int32  bypassStpType_index;
        int32  bypassStpType_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_BYPASSSTPTYPE_MAX, TEST_BYPASSSTPTYPE_MIN, bypassStpType, bypassStpType_result, bypassStpType_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (bypassStpType_index = 0; bypassStpType_index <= bypassStpType_last; bypassStpType_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (bypassStpType_result[bypassStpType_index] && enable_result[enable_index]) ? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)) && ((bypassStpType[bypassStpType_index] >= BYPASS_STP_TYPE_USER_DEF_2 && bypassStpType[bypassStpType_index] <= BYPASS_STP_TYPE_USER_DEF_5) ||
                    (bypassStpType[bypassStpType_index] == BYPASS_STP_TYPE_RMA_03)))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                    && (bypassStpType[bypassStpType_index] >= BYPASS_STP_TYPE_USER_DEF_4 && bypassStpType[bypassStpType_index] <= BYPASS_STP_TYPE_LLDP))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_bypassStp_set(unit, bypassStpType[bypassStpType_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u, enable %u)"
                                    , ret, expect_result, unit, bypassStpType[bypassStpType_index], enable[enable_index]);

                    ret = rtk_trap_bypassStp_get(unit, bypassStpType[bypassStpType_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_get (unit %u, bypassStpType %u, enable %u)"
                                     , ret, expect_result, unit, bypassStpType[bypassStpType_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_set/get value unequal (unit %u, bypassStpType %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, bypassStpType[bypassStpType_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, bypassStpType[bypassStpType_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_bypassStp_get(unit, bypassStpType[bypassStpType_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (bypassStpType_result[bypassStpType_index])? RT_ERR_OK : RT_ERR_FAILED;
            if ((HWP_8390_50_FAMILY(unit)) && ((bypassStpType[bypassStpType_index] >= BYPASS_STP_TYPE_USER_DEF_2 && bypassStpType[bypassStpType_index] <= BYPASS_STP_TYPE_USER_DEF_5) ||
                (bypassStpType[bypassStpType_index] == BYPASS_STP_TYPE_RMA_03)))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u)"
                                , ret, expect_result, unit, bypassStpType[bypassStpType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u)"
                                    , ret, RT_ERR_OK, unit, bypassStpType[bypassStpType_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  bypassStpType;
        uint32  bypassStpType_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_BYPASSSTPTYPE_MAX, bypassStpType_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_BYPASSSTPTYPE_MIN, bypassStpType); bypassStpType <= bypassStpType_max; bypassStpType++)
        {
            ASSIGN_EXPECT_RESULT(bypassStpType, TEST_BYPASSSTPTYPE_MIN, TEST_BYPASSSTPTYPE_MAX, inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)) && ((bypassStpType >= BYPASS_STP_TYPE_USER_DEF_2 && bypassStpType <= BYPASS_STP_TYPE_USER_DEF_5) ||
                    (bypassStpType == BYPASS_STP_TYPE_RMA_03)))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                    && (bypassStpType >= BYPASS_STP_TYPE_USER_DEF_4 && bypassStpType <= BYPASS_STP_TYPE_LLDP))
                    expect_result = RT_ERR_FAILED;


                ret = rtk_trap_bypassStp_set(unit, bypassStpType, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u, enable %u)"
                                    , ret, expect_result, unit, bypassStpType, enable);
                    ret = rtk_trap_bypassStp_get(unit, bypassStpType, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_get (unit %u, bypassStpType %u, enable %u)"
                                     , ret, expect_result, unit, bypassStpType, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassStp_set/get value unequal (unit %u, bypassStpType %u, enable %u)"
                                     , result_enable, enable, unit, bypassStpType, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassStp_set (unit %u, bypassStpType %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, bypassStpType, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_bypassVlan_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_bypassVlan_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_bypassVlan_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_bypassVlanType_t  bypassVlanType[UNITTEST_MAX_TEST_VALUE];
        int32  bypassVlanType_result[UNITTEST_MAX_TEST_VALUE];
        int32  bypassVlanType_index;
        int32  bypassVlanType_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_BYPASSVLANTYPE_MAX, TEST_BYPASSVLANTYPE_MIN, bypassVlanType, bypassVlanType_result, bypassVlanType_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (bypassVlanType_index = 0; bypassVlanType_index <= bypassVlanType_last; bypassVlanType_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (bypassVlanType_result[bypassVlanType_index] && enable_result[enable_index]) ? RT_ERR_OK : RT_ERR_FAILED;
                if ((HWP_8390_50_FAMILY(unit)) && ((bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_USER_DEF_2 && bypassVlanType[bypassVlanType_index] <= BYPASS_VLAN_TYPE_USER_DEF_5) ||
                    (bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_OAM && bypassVlanType[bypassVlanType_index] <= BYPASS_VLAN_TYPE_GVRP) ||
                    (bypassVlanType[bypassVlanType_index] == BYPASS_VLAN_TYPE_RMA_03)))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                    && (bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_USER_DEF_5))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_bypassVlan_set(unit, bypassVlanType[bypassVlanType_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u, enable %u)"
                                    , ret, expect_result, unit, bypassVlanType[bypassVlanType_index], enable[enable_index]);

                    ret = rtk_trap_bypassVlan_get(unit, bypassVlanType[bypassVlanType_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_get (unit %u, bypassVlanType %u, enable %u)"
                                     , ret, expect_result, unit, bypassVlanType[bypassVlanType_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_set/get value unequal (unit %u, bypassVlanType %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, bypassVlanType[bypassVlanType_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, bypassVlanType[bypassVlanType_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_bypassVlan_get(unit, bypassVlanType[bypassVlanType_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (bypassVlanType_result[bypassVlanType_index])? RT_ERR_OK : RT_ERR_FAILED;

            if ((HWP_8390_50_FAMILY(unit)) && ((bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_USER_DEF_2 && bypassVlanType[bypassVlanType_index] <= BYPASS_VLAN_TYPE_USER_DEF_5) ||
                (bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_OAM && bypassVlanType[bypassVlanType_index] <= BYPASS_VLAN_TYPE_GVRP) ||
                (bypassVlanType[bypassVlanType_index] == BYPASS_VLAN_TYPE_RMA_03)))
                expect_result = RT_ERR_FAILED;
            if ((HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                && (bypassVlanType[bypassVlanType_index] >= BYPASS_VLAN_TYPE_USER_DEF_5))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u)"
                                , ret, expect_result, unit, bypassVlanType[bypassVlanType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u)"
                                    , ret, RT_ERR_OK, unit, bypassVlanType[bypassVlanType_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  bypassVlanType;
        uint32  bypassVlanType_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_BYPASSVLANTYPE_MAX, bypassVlanType_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_BYPASSVLANTYPE_MIN, bypassVlanType); bypassVlanType <= bypassVlanType_max; bypassVlanType++)
        {
            ASSIGN_EXPECT_RESULT(bypassVlanType, TEST_BYPASSVLANTYPE_MIN, TEST_BYPASSVLANTYPE_MAX, inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)) && ((bypassVlanType >= BYPASS_VLAN_TYPE_USER_DEF_2 && bypassVlanType <= BYPASS_VLAN_TYPE_USER_DEF_5) ||
                    (bypassVlanType >= BYPASS_VLAN_TYPE_OAM && bypassVlanType <= BYPASS_VLAN_TYPE_GVRP) ||
                    (bypassVlanType == BYPASS_VLAN_TYPE_RMA_03)))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
                    && (bypassVlanType >= BYPASS_VLAN_TYPE_USER_DEF_5))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_bypassVlan_set(unit, bypassVlanType, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u, enable %u)"
                                    , ret, expect_result, unit, bypassVlanType, enable);
                    ret = rtk_trap_bypassVlan_get(unit, bypassVlanType, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_get (unit %u, bypassVlanType %u, enable %u)"
                                     , ret, expect_result, unit, bypassVlanType, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_bypassVlan_set/get value unequal (unit %u, bypassVlanType %u, enable %u)"
                                     , result_enable, enable, unit, bypassVlanType, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_bypassVlan_set (unit %u, bypassVlanType %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, bypassVlanType, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_userDefineRmaEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  userDefineIdx[UNITTEST_MAX_TEST_VALUE];
        int32  userDefineIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  userDefineIdx_index;
        int32  userDefineIdx_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_UDL2RMA_ID_MAX(unit), TEST_UDL2RMA_ID_MIN, userDefineIdx, userDefineIdx_result, userDefineIdx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (userDefineIdx_index = 0; userDefineIdx_index <= userDefineIdx_last; userDefineIdx_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (userDefineIdx_result[userDefineIdx_index] && enable_result[enable_index]) ? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_userDefineRmaEnable_set(unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                    , ret, expect_result, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);

                    ret = rtk_trap_userDefineRmaEnable_get(unit, userDefineIdx[userDefineIdx_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_get (unit %u, userDefineIdx %u, enable %u)"
                                     , ret, expect_result, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_set/get value unequal (unit %u, userDefineIdx %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_userDefineRmaEnable_get(unit, userDefineIdx[userDefineIdx_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (userDefineIdx_result[userDefineIdx_index])? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u)"
                                , ret, expect_result, unit, userDefineIdx[userDefineIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u)"
                                    , ret, RT_ERR_OK, unit, userDefineIdx[userDefineIdx_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  userDefineIdx;
        uint32  userDefineIdx_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_UDL2RMA_ID_MAX(unit), userDefineIdx_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_UDL2RMA_ID_MIN, userDefineIdx); userDefineIdx <= userDefineIdx_max; userDefineIdx++)
        {
            ASSIGN_EXPECT_RESULT(userDefineIdx, TEST_UDL2RMA_ID_MIN, TEST_UDL2RMA_ID_MAX(unit), inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_userDefineRmaEnable_set(unit, userDefineIdx, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                    , ret, expect_result, unit, userDefineIdx, enable);
                    ret = rtk_trap_userDefineRmaEnable_get(unit, userDefineIdx, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_get (unit %u, userDefineIdx %u, enable %u)"
                                     , ret, expect_result, unit, userDefineIdx, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaEnable_set/get value unequal (unit %u, userDefineIdx %u, enable %u)"
                                     , result_enable, enable, unit, userDefineIdx, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, userDefineIdx, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_userDefineRmaLearningEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    uint8  inter_result1 = 1;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaLearningEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_userDefineRmaLearningEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  userDefineIdx[UNITTEST_MAX_TEST_VALUE];
        int32  userDefineIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  userDefineIdx_index;
        int32  userDefineIdx_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_UDL2RMA_ID_MAX(unit), TEST_UDL2RMA_ID_MIN, userDefineIdx, userDefineIdx_result, userDefineIdx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (userDefineIdx_index = 0; userDefineIdx_index <= userDefineIdx_last; userDefineIdx_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (userDefineIdx_result[userDefineIdx_index] && enable_result[enable_index]) ? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_userDefineRmaLearningEnable_set(unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                    , ret, expect_result, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);

                    ret = rtk_trap_userDefineRmaLearningEnable_get(unit, userDefineIdx[userDefineIdx_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_get (unit %u, userDefineIdx %u, enable %u)"
                                     , ret, expect_result, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set/get value unequal (unit %u, userDefineIdx %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, userDefineIdx[userDefineIdx_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_userDefineRmaLearningEnable_get(unit, userDefineIdx[userDefineIdx_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (userDefineIdx_result[userDefineIdx_index])? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u)"
                                , ret, expect_result, unit, userDefineIdx[userDefineIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u)"
                                    , ret, RT_ERR_OK, unit, userDefineIdx[userDefineIdx_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  userDefineIdx;
        uint32  userDefineIdx_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_UDL2RMA_ID_MAX(unit), userDefineIdx_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_UDL2RMA_ID_MIN, userDefineIdx); userDefineIdx <= userDefineIdx_max; userDefineIdx++)
        {
            ASSIGN_EXPECT_RESULT(userDefineIdx, TEST_UDL2RMA_ID_MIN, TEST_UDL2RMA_ID_MAX(unit), inter_result2);
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result1 && inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_userDefineRmaLearningEnable_set(unit, userDefineIdx, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                    , ret, expect_result, unit, userDefineIdx, enable);
                    ret = rtk_trap_userDefineRmaLearningEnable_get(unit, userDefineIdx, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_get (unit %u, userDefineIdx %u, enable %u)"
                                     , ret, expect_result, unit, userDefineIdx, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set/get value unequal (unit %u, userDefineIdx %u, enable %u)"
                                     , result_enable, enable, unit, userDefineIdx, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_userDefineRmaLearningEnable_set (unit %u, userDefineIdx %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, userDefineIdx, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_trap_mgmtFrameLearningEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameLearningEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameLearningEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MGMTYPE_ID_MAX, TEST_MGMTYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];

            if((HWP_8390_50_FAMILY(unit)) && type[type_index] != MGMT_TYPE_PTP && type[type_index] != MGMT_TYPE_LLDP)
                inter_result2 = 0;
            if((HWP_8380_30_FAMILY(unit)) && type[type_index] != MGMT_TYPE_PTP && type[type_index] != MGMT_TYPE_LLDP && type[type_index] != MGMT_TYPE_BPDU)
                inter_result2 = 0;
            if((HWP_9310_FAMILY_ID(unit)) && type[type_index] != MGMT_TYPE_PTP && type[type_index] != MGMT_TYPE_LLDP && type[type_index] != MGMT_TYPE_EAPOL)
                inter_result2 = 0;
            if((HWP_9300_FAMILY_ID(unit))
                && type[type_index] != MGMT_TYPE_PTP
                && type[type_index] != MGMT_TYPE_PTP_UDP
                && type[type_index] != MGMT_TYPE_PTP_ETH2
                && type[type_index] != MGMT_TYPE_LLDP
                && type[type_index] != MGMT_TYPE_EAPOL)
                inter_result2 = 0;
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                inter_result3 = enable_result[enable_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_mgmtFrameLearningEnable_set(unit, type[type_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type[type_index], enable[enable_index]);

                    ret = rtk_trap_mgmtFrameLearningEnable_get(unit, type[type_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_get (unit %u, type %u, enable %u)"
                                     , ret, expect_result, unit, type[type_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set/get value unequal (unit %u, type %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, type[type_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], enable[enable_index]);
                }
            }

            ret = rtk_trap_mgmtFrameLearningEnable_get(unit, type[type_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_mgmtType_t  type;
        rtk_trap_mgmtType_t  type_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_MGMTYPE_ID_MAX, type_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_MGMTYPE_ID_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_MGMTYPE_ID_MIN, TEST_MGMTYPE_ID_MAX, inter_result2);

            if((HWP_8390_50_FAMILY(unit)) && type != MGMT_TYPE_PTP && type != MGMT_TYPE_LLDP)
                inter_result2 = 0;
            if((HWP_8380_30_FAMILY(unit)) && type != MGMT_TYPE_PTP && type != MGMT_TYPE_LLDP && type != MGMT_TYPE_BPDU)
                inter_result2 = 0;
            if((HWP_9310_FAMILY_ID(unit)) && type != MGMT_TYPE_PTP && type != MGMT_TYPE_LLDP && type != MGMT_TYPE_EAPOL)
                inter_result2 = 0;
            if((HWP_9300_FAMILY_ID(unit))
                && type != MGMT_TYPE_PTP
                && type != MGMT_TYPE_PTP_UDP
                && type != MGMT_TYPE_PTP_ETH2
                && type != MGMT_TYPE_LLDP
                && type != MGMT_TYPE_EAPOL)

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_mgmtFrameLearningEnable_set(unit, type, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u, enable %u)"
                                    , ret, expect_result, unit, type, enable);
                    ret = rtk_trap_mgmtFrameLearningEnable_get(unit, type, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_get (unit %u, type %u, enable %u)"
                                     , ret, expect_result, unit, type, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set/get value unequal (unit %u, type %u, enable %u)"
                                     , result_enable, enable, unit, type, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameLearningEnable_set (unit %u, type %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, type, enable);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_mgmtFrameLearningEnable_test*/

int32 dal_trap_pktWithOuterCFIAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithOuterCFIAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_pktWithOuterCFIAction_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_RMAACT_ID_MAX, TEST_RMAACT_ID_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if(action[action_index] > ACTION_TRAP2CPU)
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_pktWithOuterCFIAction_set(unit, action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_trap_pktWithOuterCFIAction_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_trap_pktWithOuterCFIAction_get(unit, (rtk_action_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_RMAACT_ID_MAX, action_max);

        for (L_VALUE(TEST_RMAACT_ID_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_RMAACT_ID_MIN, TEST_RMAACT_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if(action > ACTION_TRAP2CPU)
                expect_result = RT_ERR_FAILED;
            ret = rtk_trap_pktWithOuterCFIAction_set(unit, action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_trap_pktWithOuterCFIAction_get(unit, &result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_pktWithOuterCFIAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_pktWithOuterCFIAction_test*/

int32 dal_trap_cfmUnknownFrameAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmUnknownFrameAct_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmUnknownFrameAct_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_CFMUNKNOWNFRAMEACT_MIN, TEST_CFMUNKNOWNFRAMEACT_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            if((action[action_index] == ACTION_COPY2CPU) || (action[action_index] == ACTION_DROP))
                continue;
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_cfmUnknownFrameAct_set(unit, action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_trap_cfmUnknownFrameAct_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_trap_cfmUnknownFrameAct_get(unit, (rtk_action_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_CFMUNKNOWNFRAMEACT_MAX, action_max);

        for (L_VALUE(TEST_CFMUNKNOWNFRAMEACT_MIN, action); action <= action_max; action++)
        {
            if((action == ACTION_COPY2CPU) || (action == ACTION_DROP))
                continue;

            ASSIGN_EXPECT_RESULT(action, TEST_CFMUNKNOWNFRAMEACT_MIN, TEST_CFMUNKNOWNFRAMEACT_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_cfmUnknownFrameAct_set(unit, action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_trap_cfmUnknownFrameAct_get(unit, &result_action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmUnknownFrameAct_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_cfmUnknownFrameAct_test*/

int32 dal_trap_cfmLoopbackLinkTraceAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmLoopbackLinkTraceAct_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmLoopbackLinkTraceAct_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  md_level[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_result[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_index;
        int32  md_level_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MDLEVEL_ID_MAX, TEST_MDLEVEL_ID_MIN, md_level, md_level_result, md_level_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CFMLOOPBACKACT_MAX, TEST_CFMLOOPBACKACT_MIN, action, action_result, action_last);

        for (md_level_index = 0; md_level_index <= md_level_last; md_level_index++)
        {
            inter_result2 = md_level_result[md_level_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_cfmLoopbackLinkTraceAct_set(unit, md_level[md_level_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level[md_level_index], action[action_index]);

                    ret = rtk_trap_cfmLoopbackLinkTraceAct_get(unit, md_level[md_level_index], (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level[md_level_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action[action_index], unit, md_level[md_level_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level[md_level_index], action[action_index]);
                }
            }

            ret = rtk_trap_cfmLoopbackLinkTraceAct_get(unit, md_level[md_level_index], (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u)"
                                , ret, expect_result, unit, md_level[md_level_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u)"
                                    , ret, RT_ERR_OK, unit, md_level[md_level_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  md_level;
        uint32  md_level_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_MDLEVEL_ID_MAX, md_level_max);
        G_VALUE(TEST_CFMLOOPBACKACT_MAX, action_max);

        for (L_VALUE(TEST_MDLEVEL_ID_MIN, md_level); md_level <= md_level_max; md_level++)
        {
            ASSIGN_EXPECT_RESULT(md_level, TEST_MDLEVEL_ID_MIN, TEST_MDLEVEL_ID_MAX, inter_result2);

            for (L_VALUE(TEST_CFMLOOPBACKACT_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_CFMLOOPBACKACT_MIN, TEST_CFMLOOPBACKACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_cfmLoopbackLinkTraceAct_set(unit, md_level, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level, action);
                    ret = rtk_trap_cfmLoopbackLinkTraceAct_get(unit, md_level, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action, unit, md_level, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmLoopbackLinkTraceAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level, action);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_cfmLoopbackLinkTraceAct_test*/

int32 dal_trap_cfmCcmAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_trap_oam_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmCcmAct_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmCcmAct_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  md_level[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_result[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_index;
        int32  md_level_last;

        rtk_trap_oam_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_trap_oam_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MDLEVEL_ID_MAX, TEST_MDLEVEL_ID_MIN, md_level, md_level_result, md_level_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_OAM_ACTION_MAX, TEST_OAM_ACTION_MIN, action, action_result, action_last);

        for (md_level_index = 0; md_level_index <= md_level_last; md_level_index++)
        {
            inter_result2 = md_level_result[md_level_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)) && action[action_index] > TRAP_OAM_ACTION_TRAP2CPU && action[action_index] != TRAP_OAM_ACTION_LINK_FAULT_DETECT)
                    expect_result = RT_ERR_FAILED;


                ret = rtk_trap_cfmCcmAct_set(unit, md_level[md_level_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level[md_level_index], action[action_index]);

                    ret = rtk_trap_cfmCcmAct_get(unit, md_level[md_level_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level[md_level_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action[action_index], unit, md_level[md_level_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level[md_level_index], action[action_index]);
                }
            }

            ret = rtk_trap_cfmCcmAct_get(unit, md_level[md_level_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u)"
                                , ret, expect_result, unit, md_level[md_level_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u)"
                                    , ret, RT_ERR_OK, unit, md_level[md_level_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  md_level;
        uint32  md_level_max;
        rtk_trap_oam_action_t  action;
        rtk_trap_oam_action_t  action_max;
        rtk_trap_oam_action_t  result_action;

        G_VALUE(TEST_MDLEVEL_ID_MAX, md_level_max);
        G_VALUE(TEST_OAM_ACTION_MAX, action_max);

        for (L_VALUE(TEST_MDLEVEL_ID_MIN, md_level); md_level <= md_level_max; md_level++)
        {
            ASSIGN_EXPECT_RESULT(md_level, TEST_MDLEVEL_ID_MIN, TEST_MDLEVEL_ID_MAX, inter_result2);

            for (L_VALUE(TEST_OAM_ACTION_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_OAM_ACTION_MIN, TEST_OAM_ACTION_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if ((HWP_8390_50_FAMILY(unit)) && action > TRAP_OAM_ACTION_TRAP2CPU && action != TRAP_OAM_ACTION_LINK_FAULT_DETECT)
                    expect_result = RT_ERR_FAILED;
                ret = rtk_trap_cfmCcmAct_set(unit, md_level, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level, action);
                    ret = rtk_trap_cfmCcmAct_get(unit, md_level, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmCcmAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action, unit, md_level, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmCcmAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level, action);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_cfmCcmAct_test*/

int32 dal_trap_cfmEthDmAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmEthDmAct_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_cfmEthDmAct_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  md_level[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_result[UNITTEST_MAX_TEST_VALUE];
        int32  md_level_index;
        int32  md_level_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MDLEVEL_ID_MAX, TEST_MDLEVEL_ID_MIN, md_level, md_level_result, md_level_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CFMETHDMACT_MAX, TEST_CFMETHDMACT_MIN, action, action_result, action_last);

        for (md_level_index = 0; md_level_index <= md_level_last; md_level_index++)
        {
            inter_result2 = md_level_result[md_level_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_cfmEthDmAct_set(unit, md_level[md_level_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level[md_level_index], action[action_index]);

                    ret = rtk_trap_cfmEthDmAct_get(unit, md_level[md_level_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level[md_level_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action[action_index], unit, md_level[md_level_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level[md_level_index], action[action_index]);
                }
            }

            ret = rtk_trap_cfmEthDmAct_get(unit, md_level[md_level_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u)"
                                , ret, expect_result, unit, md_level[md_level_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u)"
                                    , ret, RT_ERR_OK, unit, md_level[md_level_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  md_level;
        uint32  md_level_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_MDLEVEL_ID_MAX, md_level_max);
        G_VALUE(TEST_CFMETHDMACT_MAX, action_max);

        for (L_VALUE(TEST_MDLEVEL_ID_MIN, md_level); md_level <= md_level_max; md_level++)
        {
            ASSIGN_EXPECT_RESULT(md_level, TEST_MDLEVEL_ID_MIN, TEST_MDLEVEL_ID_MAX, inter_result2);

            for (L_VALUE(TEST_CFMETHDMACT_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_CFMETHDMACT_MIN, TEST_CFMETHDMACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_cfmEthDmAct_set(unit, md_level, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u, action %u)"
                                    , ret, expect_result, unit, md_level, action);
                    ret = rtk_trap_cfmEthDmAct_get(unit, md_level, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_get (unit %u, md_level %u, action %u)"
                                     , ret, expect_result, unit, md_level, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_cfmEthDmAct_set/get value unequal (unit %u, md_level %u, action %u)"
                                     , result_action, action, unit, md_level, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_cfmEthDmAct_set (unit %u, md_level %u, action %u)"
                                        , ret, RT_ERR_OK, unit, md_level, action);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_cfmEthDmAct_test*/

int32 dal_trap_portOamLoopbackParAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_trap_oam_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_portOamLoopbackParAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_portOamLoopbackParAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_trap_oam_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_trap_oam_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_OAM_ACTION_MAX, TEST_OAM_ACTION_MIN, action, action_result, action_last);

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
                if ((HWP_8390_50_FAMILY(unit)) && action[action_index] > TRAP_OAM_ACTION_TRAP2CPU && action[action_index] != TRAP_OAM_ACTION_LOOPBACK)
                    expect_result = RT_ERR_FAILED;

                if ((HWP_9310_FAMILY_ID(unit)) && action[action_index] > TRAP_OAM_ACTION_LOOPBACK)
                    expect_result = RT_ERR_FAILED;
                ret = rtk_trap_portOamLoopbackParAction_set(unit, port[port_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_trap_portOamLoopbackParAction_get(unit, port[port_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_trap_portOamLoopbackParAction_get(unit, port[port_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        uint32  port_max;
        rtk_trap_oam_action_t  action;
        rtk_trap_oam_action_t  action_max;
        rtk_trap_oam_action_t  result_action;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_OAM_ACTION_MAX, action_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_OAM_ACTION_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_OAM_ACTION_MIN, TEST_OAM_ACTION_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)) && action > TRAP_OAM_ACTION_TRAP2CPU && action != TRAP_OAM_ACTION_LOOPBACK)
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9310_FAMILY_ID(unit)) && action > TRAP_OAM_ACTION_LOOPBACK)
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_portOamLoopbackParAction_set(unit, port, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);
                    ret = rtk_trap_portOamLoopbackParAction_get(unit, port, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_portOamLoopbackParAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_portOamLoopbackParAction_test*/

int32 dal_trap_routeExceptionAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_routeExceptionAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_routeExceptionAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_routeExceptionType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_ROUTE_EXCEPTION_TYPE_MAX, TEST_ROUTE_EXCEPTION_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ROUTE_EXCEPTION_ACT_MAX, TEST_ROUTE_EXCEPTION_ACT_MIN, action, action_result, action_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (action_index = 0; action_index <= action_last; action_index++)
            {
                inter_result3 = action_result[action_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8380_30_FAMILY(unit)) && ((ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR == type[type_index]) || (ROUTE_EXCEPTION_TYPE_HDR_ERR == type[type_index])))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_routeExceptionAction_set(unit, type[type_index], action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type[type_index], action[action_index]);

                    ret = rtk_trap_routeExceptionAction_get(unit, type[type_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type[type_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action[action_index], unit, type[type_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], action[action_index]);
                }
            }

            ret = rtk_trap_routeExceptionAction_get(unit, type[type_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((HWP_8380_30_FAMILY(unit)) && ((ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR == type[type_index]) || (ROUTE_EXCEPTION_TYPE_HDR_ERR == type[type_index])))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_routeExceptionType_t  type;
        uint32  type_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;


        G_VALUE(TEST_ROUTE_EXCEPTION_TYPE_MAX, type_max);
        G_VALUE(TEST_ROUTE_EXCEPTION_ACT_MAX, action_max);

        for (L_VALUE(TEST_ROUTE_EXCEPTION_TYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_ROUTE_EXCEPTION_TYPE_MIN, TEST_ROUTE_EXCEPTION_TYPE_MAX, inter_result2);

            for (L_VALUE(TEST_ROUTE_EXCEPTION_ACT_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ROUTE_EXCEPTION_ACT_MIN, TEST_ROUTE_EXCEPTION_ACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
                else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8380_30_FAMILY(unit)) && ((ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR == type) || (ROUTE_EXCEPTION_TYPE_HDR_ERR == type)))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_trap_routeExceptionAction_set(unit, type, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type, action);
                    ret = rtk_trap_routeExceptionAction_get(unit, type, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type, action);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action, unit, type, action);
                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type, action);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_routeExceptionAction_test*/

int32 dal_trap_routeExceptionPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_routeExceptionPri_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_routeExceptionPri_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_trap_routeExceptionType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;

        rtk_pri_t  result_pri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_ROUTE_EXCEPTION_TYPE_MAX, TEST_ROUTE_EXCEPTION_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_ID_MAX, TEST_PRI_ID_MIN, pri, pri_result, pri_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (pri_index = 0; pri_index <= pri_last; pri_index++)
            {
                inter_result3 = pri_result[pri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_routeExceptionPri_set(unit, type[type_index], pri[pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u, pri %u)"
                                    , ret, expect_result, unit, type[type_index], pri[pri_index]);

                    ret = rtk_trap_routeExceptionPri_get(unit, type[type_index], &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_get (unit %u, type %u, pri %u)"
                                     , ret, expect_result, unit, type[type_index], pri[pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_set/get value unequal (unit %u, type %u, pri %u)"
                                     , result_pri, pri[pri_index], unit, type[type_index], pri[pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], pri[pri_index]);
                }
            }

            ret = rtk_trap_routeExceptionPri_get(unit, type[type_index], &result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_trap_routeExceptionType_t  type;
        uint32  type_max;
        rtk_pri_t  pri;
        rtk_pri_t  pri_max;
        rtk_pri_t  result_pri;

        G_VALUE(TEST_ROUTE_EXCEPTION_TYPE_MAX, type_max);
        G_VALUE(TEST_PRI_ID_MAX, pri_max);

        for (L_VALUE(TEST_ROUTE_EXCEPTION_TYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_ROUTE_EXCEPTION_TYPE_MIN, TEST_ROUTE_EXCEPTION_TYPE_MAX, inter_result2);

            for (L_VALUE(TEST_PRI_ID_MIN, pri); pri <= pri_max; pri++)
            {
                ASSIGN_EXPECT_RESULT(pri, TEST_PRI_ID_MIN, TEST_PRI_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trap_routeExceptionPri_set(unit, type, pri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u, pri %u)"
                                    , ret, expect_result, unit, type, pri);
                    ret = rtk_trap_routeExceptionPri_get(unit, type, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_get (unit %u, type %u, pri %u)"
                                     , ret, expect_result, unit, type, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_trap_routeExceptionPri_set/get value unequal (unit %u, type %u, pri %u)"
                                     , result_pri, pri, unit, type, pri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_routeExceptionPri_set (unit %u, type %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, type, pri);
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_routeExceptionPri_test*/

int32 dal_trap_mgmtFrameMgmtVlanEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameMgmtVlanEnable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trap_mgmtFrameMgmtVlanEnable_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
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
            ret = rtk_trap_mgmtFrameMgmtVlanEnable_set(unit,enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit,enable[enable_index]);

                ret = rtk_trap_mgmtFrameMgmtVlanEnable_get(unit,(rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit,enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit,enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit,enable[enable_index]);
            }
        }

        ret = rtk_trap_mgmtFrameMgmtVlanEnable_get(unit,(rtk_enable_t*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u)"
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
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_trap_mgmtFrameMgmtVlanEnable_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_trap_mgmtFrameMgmtVlanEnable_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trap_mgmtFrameMgmtVlanEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    return RT_ERR_OK;
} /*end of dal_trap_mgmtFrameMgmtVlanEnable_test*/
