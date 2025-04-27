/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 75091 $
 * $Date: 2017-01-06 15:58:28 +0800 (Fri, 06 Jan 2017) $
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
#define SWITCH_RANDOM_RUN_TIMES                 100

/* Common */
/* Length */
#define TEST_PKTLEN_ID_MIN                      0
#define TEST_PKTLEN_ID_MAX(unit)                (HAL_MAX_ACCEPT_FRAME_LEN(unit))
/* Snap Mode */
#define TEST_SNAPMODE_ID_MIN                    SNAP_MODE_AAAA03000000
#define TEST_SNAPMODE_ID_MAX                    SNAP_MODE_AAAA03
/* checksum Faile Type */
#define TEST_CKSERRFAIL_TYPE_ID_MIN             LAYER2_CHKSUM_FAIL
#define TEST_CKSERRFAIL_TYPE_ID_MAX(unit)       LAYER2_CHKSUM_FAIL
/* checksum Faile Action */
#define TEST_CKSERRFAIL_ACT_ID_MIN              ACTION_FORWARD
#define TEST_CKSERRFAIL_ACT_ID_MAX              ACTION_DROP
/*Link Speed*/
#define TEST_LINK_SPEED_MIN                     MAXPKTLEN_LINK_SPEED_FE /*0*/
#define TEST_LINK_SPEED_MAX                     (MAXPKTLEN_LINK_SPEED_END - 1)
/*CPU Maximum Packet Direction*/
#define TEST_CPU_MAXPKTDIR_MIN                  PKTDIR_RX
#define TEST_CPU_MAXPKTDIR_MAX                  PKTDIR_TX
/* Packet to CPU Format */
#define TEST_PKT2CPU_FORMAT_MIN                 ORIGINAL_PACKET
#define TEST_PKT2CPU_FORMAT_MAX                 (PKT_FORMAT_END - 1)
/* Packet to CPU Type */
#define TEST_PKT2CPU_TYPE_ID_MIN                PKT2CPU_TYPE_FORWARD
#define TEST_PKT2CPU_TYPE_ID_MAX                PKT2CPU_TYPE_TRAP
/* Packet Format */
#define TEST_PKT_FORMAT_MIN                     ORIGINAL_PACKET
#define TEST_PKT_FORMAT_MAX                     MODIFIED_PACKET

int32 dal_switch_chksumFailAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_chksumFailAction_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_chksumFailAction_get(unit, 0, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_switch_chksum_fail_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_action_t action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CKSERRFAIL_TYPE_ID_MAX(unit), TEST_CKSERRFAIL_TYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_CKSERRFAIL_ACT_ID_MAX, TEST_CKSERRFAIL_ACT_ID_MIN, action, action_result, action_last);

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

                    ret = rtk_switch_chksumFailAction_set(unit, port[port_index], type[type_index], action[action_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, action %u)"
                                        , ret, expect_result, unit, port[port_index], type[type_index], action[action_index]);

                        ret = rtk_switch_chksumFailAction_get(unit, port[port_index], type[type_index], (rtk_action_t*)&result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_get (unit %u, port %u, type %d, action %u)"
                                         , ret, expect_result, unit, port[port_index], type[type_index], action[action_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_set/get value unequal (unit %u, port %u, type %d, action %u)"
                                         , result_action, action[action_index], unit, port[port_index], type[type_index], action[action_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, action %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], type[type_index], action[action_index]);
                    }
                }

                ret = rtk_switch_chksumFailAction_get(unit, port[port_index], type[type_index], (rtk_action_t*)&result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                    if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, )"
                                    , ret, expect_result, unit, port[port_index], type[type_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, )"
                                        , ret, RT_ERR_OK, unit, port[port_index], type[type_index]);
                }

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  port;
        uint32  port_max;
        rtk_switch_chksum_fail_t  type;
        uint32  type_max;
        rtk_action_t  action;
        uint32  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_CKSERRFAIL_TYPE_ID_MAX(unit), type_max);
        G_VALUE(TEST_CKSERRFAIL_ACT_ID_MAX, action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_CKSERRFAIL_TYPE_ID_MIN, type); type <= type_max; type++)
            {
                ASSIGN_EXPECT_RESULT(type, TEST_CKSERRFAIL_TYPE_ID_MIN, TEST_CKSERRFAIL_TYPE_ID_MAX(unit), inter_result3);

                for (L_VALUE(TEST_CKSERRFAIL_ACT_ID_MIN, action); action <= action_max; action++)
                {
                    ASSIGN_EXPECT_RESULT(action, TEST_CKSERRFAIL_ACT_ID_MIN, TEST_CKSERRFAIL_ACT_ID_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    ret = rtk_switch_chksumFailAction_set(unit, port, type, action);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, action %u)"
                                        , ret, expect_result, unit, port, type, action);
                        ret = rtk_switch_chksumFailAction_get(unit, port, type, (rtk_action_t*)&result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_get (unit %u, port %u, type %d, action %u)"
                                         , ret, expect_result, unit, port, type, action);
                        RT_TEST_IS_EQUAL_INT("rtk_switch_chksumFailAction_set/get value unequal (unit %u, port %u, type %d, action %u)"
                                         , result_action, action, unit, port, type, action);
                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_chksumFailAction_set (unit %u, port %u, type %d, action %u)"
                                            , ret, RT_ERR_OK, unit, port, type, action);
                }

            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_chksumFailAction_test*/

int32 dal_switch_recalcCRCEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_recalcCRCEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_recalcCRCEnable_get(unit, 0, &enable_temp)))
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


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
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
                ret = rtk_switch_recalcCRCEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_switch_recalcCRCEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_switch_recalcCRCEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u)"
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

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(ACTION_FORWARD, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_recalcCRCEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_switch_recalcCRCEnable_get(unit, port, (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_recalcCRCEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_recalcCRCEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_recalcCRCEnable_test*/

int32 dal_switch_mgmtMacAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mac_t  mac_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_mgmtMacAddr_set(unit, &mac_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_mgmtMacAddr_get(unit, &mac_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_mac_t mac;
        rtk_mac_t  mac_result;
        int32 randIdx;

        for (randIdx = 0; randIdx <= SWITCH_RANDOM_RUN_TIMES; randIdx++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            mac.octet[0]= ut_rand()& BITMASK_8B;
            mac.octet[1]= ut_rand()& BITMASK_8B;
            mac.octet[2]= ut_rand()& BITMASK_8B;
            mac.octet[3]= ut_rand()& BITMASK_8B;
            mac.octet[4]= ut_rand()& BITMASK_8B;
            mac.octet[5]= ut_rand()& BITMASK_8B;

            if((mac.octet[0] & BITMASK_1B) == 1)
                expect_result = RT_ERR_FAILED;

            ret = rtk_switch_mgmtMacAddr_set(unit, &mac);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_mgmtMacAddr_set (unit %u, mac %02x:%02x:%02x:%02x:%02x:%02x)"
                                ,ret, expect_result, unit, mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],
                                 mac.octet[5]);

                ret = rtk_switch_mgmtMacAddr_get(unit, &mac_result);
                RT_TEST_IS_EQUAL_INT("rtk_switch_mgmtMacAddr_get (unit %u, mac %02x:%02x:%02x:%02x:%02x:%02x)"
                                 , ret, expect_result, unit, mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],
                                 mac.octet[5]);
                ret = osal_memcmp(&mac_result.octet[0], &mac.octet[0], sizeof(rtk_mac_t));
                RT_TEST_IS_EQUAL_INT("rtk_switch_mgmtMacAddr_set/get mac address (unit %u, mac %02x:%02x:%02x:%02x:%02x:%02x)"
                                 ,ret, 0, unit, mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],
                                 mac.octet[5]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_mgmtMacAddr_set (unit %u, mac %02x:%02x:%02x:%02x:%02x:%02x)"
                                    , ret, RT_ERR_OK, unit, mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],
                                 mac.octet[5]);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_mgmtMacAddr_test*/

int32 dal_switch_IPv4Addr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  ipAddr_temp = 0;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_IPv4Addr_set(unit, ipAddr_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_IPv4Addr_get(unit, &ipAddr_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32 ipAddr;
        uint32  ipAddr_result;
        int32 randIdx;
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        for (randIdx = 0; randIdx <= SWITCH_RANDOM_RUN_TIMES; randIdx++)
        {
            ipAddr = ut_rand();

            ret = rtk_switch_IPv4Addr_set(unit, ipAddr);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_IPv4Addr_set (unit %u, ipAddr %u)"
                                ,ret, expect_result, unit, ipAddr);

                ret = rtk_switch_IPv4Addr_get(unit, &ipAddr_result);
                RT_TEST_IS_EQUAL_INT("rtk_switch_IPv4Addr_get (unit %u, ipAddr %u)"
                                 , ret, expect_result, unit, ipAddr);
                RT_TEST_IS_EQUAL_INT("rtk_switch_IPv4Addr_set/get ipAddr address (unit %u, ipAddr %u)"
                                 , ipAddr, ipAddr_result, unit, ipAddr);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_IPv4Addr_set (unit %u, ipAddr %u)"
                                    , ret, RT_ERR_OK, unit, ipAddr);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_IPv4Addr_test*/

int32 dal_switch_maxPktLenLinkSpeed_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenLinkSpeed_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenLinkSpeed_get(unit, 0, &length_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_switch_maxPktLen_linkSpeed_t  linkSpeed[UNITTEST_MAX_TEST_VALUE];
        int32  linkSpeed_result[UNITTEST_MAX_TEST_VALUE];
        int32  linkSpeed_index;
        int32  linkSpeed_last;

        uint32 length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;

        int32  result_length;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_LINK_SPEED_MAX, TEST_LINK_SPEED_MIN, linkSpeed, linkSpeed_result, linkSpeed_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PKTLEN_ID_MAX(unit), TEST_PKTLEN_ID_MIN, length, length_result, length_last);

        for (linkSpeed_index = 0; linkSpeed_index <= linkSpeed_last; linkSpeed_index++)
        {
            inter_result2 = linkSpeed_result[linkSpeed_index];
            for (length_index = 0; length_index <= length_last; length_index++)
            {
                inter_result3 = length_result[length_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_maxPktLenLinkSpeed_set(unit, linkSpeed[linkSpeed_index], length[length_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u, length %u)"
                                    , ret, expect_result, unit, linkSpeed[linkSpeed_index], length[length_index]);

                    ret = rtk_switch_maxPktLenLinkSpeed_get(unit, linkSpeed[linkSpeed_index], (uint32*)&result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_get (unit %u, linkSpeed %u, length %u)"
                                     , ret, expect_result, unit, linkSpeed[linkSpeed_index], length[length_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set/get value unequal (unit %u, linkSpeed %u, length %u)"
                                     , result_length, length[length_index], unit, linkSpeed[linkSpeed_index], length[length_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u, length %u)"
                                        , ret, RT_ERR_OK, unit, linkSpeed[linkSpeed_index], length[length_index]);
                }
            }

            ret = rtk_switch_maxPktLenLinkSpeed_get(unit, linkSpeed[linkSpeed_index], (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u)"
                                , ret, expect_result, unit, linkSpeed[linkSpeed_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u)"
                                    , ret, RT_ERR_OK, unit, linkSpeed[linkSpeed_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  linkSpeed;
        uint32  linkSpeed_max;
        uint32  length;
        uint32  length_max;
        uint32  result_length;

        G_VALUE(TEST_LINK_SPEED_MAX, linkSpeed_max);
        G_VALUE(TEST_PKTLEN_ID_MAX(unit), length_max);

        for (L_VALUE(TEST_LINK_SPEED_MIN, linkSpeed); linkSpeed <= linkSpeed_max; linkSpeed++)
        {
            ASSIGN_EXPECT_RESULT(linkSpeed, TEST_LINK_SPEED_MIN, TEST_LINK_SPEED_MAX, inter_result2);

            for (L_VALUE(TEST_PKTLEN_ID_MIN, length); length <= length_max; length++)
            {
                ASSIGN_EXPECT_RESULT(length, TEST_PKTLEN_ID_MIN, TEST_PKTLEN_ID_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_maxPktLenLinkSpeed_set(unit, linkSpeed, length);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u, length %u)"
                                    , ret, expect_result, unit, linkSpeed, length);
                    ret = rtk_switch_maxPktLenLinkSpeed_get(unit, linkSpeed, (uint32*)&result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_get (unit %u, linkSpeed %u, length %u)"
                                     , ret, expect_result, unit, linkSpeed, length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set/get value unequal (unit %u, linkSpeed %u, length %u)"
                                     , result_length, length, unit, linkSpeed, length);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenLinkSpeed_set (unit %u, linkSpeed %u, length %u)"
                                        , ret, RT_ERR_OK, unit, linkSpeed, length);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_maxPktLenLinkSpeed_test*/

int32 dal_switch_maxPktLenTagLenCntIncEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenTagLenCntIncEnable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenTagLenCntIncEnable_get(unit, &enable_temp)))
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
            ret = rtk_switch_maxPktLenTagLenCntIncEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_switch_maxPktLenTagLenCntIncEnable_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }

            ret = rtk_switch_maxPktLenTagLenCntIncEnable_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  enable;
        uint32  enable_max;
        uint32  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_switch_maxPktLenTagLenCntIncEnable_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_switch_maxPktLenTagLenCntIncEnable_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_maxPktLenTagLenCntIncEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_maxPktLenTagLenCntIncEnable_test*/

int32 dal_switch_snapMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  mode_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_snapMode_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_snapMode_get(unit, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;
        int32  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_SNAPMODE_ID_MAX, TEST_SNAPMODE_ID_MIN, mode, mode_result, mode_last);

        for (mode_index = 0; mode_index <= mode_last; mode_index++)
        {
            inter_result2 = mode_result[mode_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_switch_snapMode_set(unit, mode[mode_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);

                ret = rtk_switch_snapMode_get(unit, (uint32*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode[mode_index]);
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode[mode_index], unit, mode[mode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_snapMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
        }

        ret = rtk_switch_snapMode_get(unit, (uint32*)&result_mode);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_set (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_snapMode_set (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  mode;
        uint32  mode_max;
        uint32  result_mode;

        G_VALUE(TEST_SNAPMODE_ID_MAX, mode_max);

        for (L_VALUE(TEST_SNAPMODE_ID_MIN, mode); mode <= mode_max; mode++)
        {
            ASSIGN_EXPECT_RESULT(mode, TEST_SNAPMODE_ID_MIN, TEST_SNAPMODE_ID_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_switch_snapMode_set(unit, mode);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode);
                ret = rtk_switch_snapMode_get(unit, (uint32*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode);
                RT_TEST_IS_EQUAL_INT("rtk_switch_snapMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode, unit, mode);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_snapMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_snapMode_test*/

int32 dal_switch_cpuMaxPktLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  length_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_cpuMaxPktLen_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_cpuMaxPktLen_get(unit, 0, &length_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_switch_pktDir_t  pktDir[UNITTEST_MAX_TEST_VALUE];
        int32  pktDir_result[UNITTEST_MAX_TEST_VALUE];
        int32  pktDir_index;
        int32  pktDir_last;

        uint32 length[UNITTEST_MAX_TEST_VALUE];
        int32  length_result[UNITTEST_MAX_TEST_VALUE];
        int32  length_index;
        int32  length_last;

        int32  result_length;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_CPU_MAXPKTDIR_MAX, TEST_CPU_MAXPKTDIR_MIN, pktDir, pktDir_result, pktDir_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PKTLEN_ID_MAX(unit), TEST_PKTLEN_ID_MIN, length, length_result, length_last);

        for (pktDir_index = 0; pktDir_index <= pktDir_last; pktDir_index++)
        {
            inter_result2 = pktDir_result[pktDir_index];
            for (length_index = 0; length_index <= length_last; length_index++)
            {
                inter_result3 = length_result[length_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_cpuMaxPktLen_set(unit, pktDir[pktDir_index], length[length_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u, length %u)"
                                    , ret, expect_result, unit, pktDir[pktDir_index], length[length_index]);

                    ret = rtk_switch_cpuMaxPktLen_get(unit, pktDir[pktDir_index], (uint32*)&result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_get (unit %u, pktDir %u, length %u)"
                                     , ret, expect_result, unit, pktDir[pktDir_index], length[length_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_set/get value unequal (unit %u, pktDir %u, length %u)"
                                     , result_length, length[length_index], unit, pktDir[pktDir_index], length[length_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u, length %u)"
                                        , ret, RT_ERR_OK, unit, pktDir[pktDir_index], length[length_index]);
                }
            }

            ret = rtk_switch_cpuMaxPktLen_get(unit, pktDir[pktDir_index], (uint32*)&result_length);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u)"
                                , ret, expect_result, unit, pktDir[pktDir_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u)"
                                    , ret, RT_ERR_OK, unit, pktDir[pktDir_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  pktDir;
        uint32  pktDir_max;
        uint32  length;
        uint32  length_max;
        uint32  result_length;

        G_VALUE(TEST_CPU_MAXPKTDIR_MAX, pktDir_max);
        G_VALUE(TEST_PKTLEN_ID_MAX(unit), length_max);

        for (L_VALUE(TEST_CPU_MAXPKTDIR_MIN, pktDir); pktDir <= pktDir_max; pktDir++)
        {
            ASSIGN_EXPECT_RESULT(pktDir, TEST_CPU_MAXPKTDIR_MIN, TEST_CPU_MAXPKTDIR_MAX, inter_result2);

            for (L_VALUE(TEST_PKTLEN_ID_MIN, length); length <= length_max; length++)
            {
                ASSIGN_EXPECT_RESULT(length, TEST_PKTLEN_ID_MIN, TEST_PKTLEN_ID_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_cpuMaxPktLen_set(unit, pktDir, length);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u, length %u)"
                                    , ret, expect_result, unit, pktDir, length);
                    ret = rtk_switch_cpuMaxPktLen_get(unit, pktDir, (uint32*)&result_length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_get (unit %u, pktDir %u, length %u)"
                                     , ret, expect_result, unit, pktDir, length);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_cpuMaxPktLen_set/get value unequal (unit %u, pktDir %u, length %u)"
                                     , result_length, length, unit, pktDir, length);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_cpuMaxPktLen_set (unit %u, pktDir %u, length %u)"
                                        , ret, RT_ERR_OK, unit, pktDir, length);
            }

        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_cpuMaxPktLen_test*/

int32 dal_switch_pppoePassthrough_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_pppoeIpParseEnable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_pppoeIpParseEnable_get(unit, &enable_temp)))
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

            ret = rtk_switch_pppoeIpParseEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_switch_pppoeIpParseEnable_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  enable;
        uint32  enable_max;
        uint32  result_enable;

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
        {
            ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_switch_pppoeIpParseEnable_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_switch_pppoeIpParseEnable_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_pppoeIpParseEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }


    return RT_ERR_OK;
}   /*end of dal_switch_pppoePassthrough_test*/

int32 dal_switch_deviceInfo_test(uint32 caseNo)
{
    rtk_switch_devInfo_t  devInfo_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_deviceInfo_get(0, &devInfo_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}   /*end of dal_switch_deviceInfo_test*/

int32 dal_switch_pkt2CpuTypeFormat_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pktFormat_t  pktFormat_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_pkt2CpuTypeFormat_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_pkt2CpuTypeFormat_get(unit, 0, &pktFormat_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_switch_pkt2CpuType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_pktFormat_t format[UNITTEST_MAX_TEST_VALUE];
        int32  format_result[UNITTEST_MAX_TEST_VALUE];
        int32  format_index;
        int32  format_last;

        rtk_pktFormat_t  result_format;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PKT2CPU_TYPE_ID_MAX, TEST_PKT2CPU_TYPE_ID_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PKT_FORMAT_MAX, TEST_PKT_FORMAT_MIN, format, format_result, format_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];
            for (format_index = 0; format_index <= format_last; format_index++)
            {
                inter_result3 = format_result[format_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_pkt2CpuTypeFormat_set(unit, type[type_index], format[format_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d, format %u)"
                                    , ret, expect_result, unit, type[type_index], format[format_index]);

                    ret = rtk_switch_pkt2CpuTypeFormat_get(unit, type[type_index], &result_format);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_get (unit %u, type %d, format %u)"
                                     , ret, expect_result, unit, type[type_index], format[format_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set/get value unequal (unit %u, type %d, format %u)"
                                     , result_format, format[format_index], unit, type[type_index], format[format_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d, format %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], format[format_index]);
                }
            }

            ret = rtk_switch_pkt2CpuTypeFormat_get(unit, type[type_index], &result_format);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_switch_pkt2CpuType_t  type;
        uint32  type_max;
        rtk_pktFormat_t  format;
        uint32  format_max;
        uint32  result_format;

        G_VALUE(TEST_PKT2CPU_TYPE_ID_MAX, type_max);
        G_VALUE(TEST_PKT_FORMAT_MAX, format_max);

        for (L_VALUE(TEST_PKT2CPU_TYPE_ID_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_PKT2CPU_TYPE_ID_MIN, TEST_PKT2CPU_TYPE_ID_MAX, inter_result2);

            for (L_VALUE(TEST_PKT_FORMAT_MIN, format); format <= format_max; format++)
            {
                ASSIGN_EXPECT_RESULT(format, TEST_PKT_FORMAT_MIN, TEST_PKT_FORMAT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_switch_pkt2CpuTypeFormat_set(unit, type, format);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d, format %u)"
                                    , ret, expect_result, unit, type, format);
                    ret = rtk_switch_pkt2CpuTypeFormat_get(unit, type, &result_format);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_get (unit %u, type %d, format %u)"
                                     , ret, expect_result, unit, type, format);
                    RT_TEST_IS_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set/get value unequal (unit %u, type %d, format %u)"
                                     , result_format, format, unit, type, format);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_switch_pkt2CpuTypeFormat_set (unit %u, type %d, format %u)"
                                        , ret, RT_ERR_OK, unit, type, format);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_switch_pkt2CpuTypeFormat_test*/


