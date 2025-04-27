/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74163 $
 * $Date: 2016-12-09 11:39:52 +0800 (Fri, 09 Dec 2016) $
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
#include <rtk/vlan.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/*Trunk Id*/
#define TEST_TRUNK_ID_MIN                           0
#define TEST_TRUNK_ID_MAX(unit)                     (HAL_MAX_NUM_OF_TRUNK(unit) - 1)
/*Trunk Hash Algo Id*/
#define TEST_TRUNK_HASHALGOID_MIN                   0
#define TEST_TRUNK_HASHALGOID_MAX(unit)             (HAL_MAX_NUM_OF_TRUNK_ALGO(unit) - 1)
/*Hash Algo Shift*/
#define TEST_TRUNK_HASHALGOSHIFT_MIN                0
#define TEST_TRUNK_HASHALGOSHIFT_MAX(unit)          (HAL_TRUNK_ALGO_SHIFT_MAX(unit))
/*Traffic Separate Type*/
#define TEST_TRUNK_SEPARATETYPE_MIN                 SEPARATE_NONE
#define TEST_TRUNK_SEPARATETYPE_MAX                 (SEPARATE_END-1)
/*Hash Algo*/
#define TEST_TRUNK_HASHALGO_MIN                     0
#define TEST_TRUNK_HASHALGO_MAX(unit)               TRUNK_DISTRIBUTION_ALGO_MASKALL
/*Trunk Mode*/
#define TEST_TRUNK_FLOODMODE_MIN                    FLOOD_MODE_BY_HASH
#define TEST_TRUNK_FLOODMODE_MAX                    FLOOD_MODE_BY_CONFIG

//FIXME: compiler says "sizeof not defined."
//#if RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST >= 2
#if 1
static rtk_portmask_t trunk_memportMask[] =
{
    { .bits[0] = 0x00000001, .bits[1]=0x000f0000 },
    { .bits[0] = 0x00000100, .bits[1]=0x00000030 },
    { .bits[0] = 0x000000ff, .bits[1]=0x00000000 },
    { .bits[0] = 0x000000f0, .bits[1]=0x0000f000 },
    { .bits[0] = 0x0000fe00, .bits[1]=0x00000000 },
    { .bits[0] = 0x000001ff, .bits[1]=0x00000000 },
    { .bits[0] = 0x00f00000, .bits[1]=0x00000500 },
    { .bits[0] = 0x00c10200, .bits[1]=0x00000008 },
    { .bits[0] = 0x00000001, .bits[1]=0x000f0000 },
    { .bits[0] = 0x00000100, .bits[1]=0x00000030 },
    { .bits[0] = 0x000000ff, .bits[1]=0x00000000 },
    { .bits[0] = 0x000000f0, .bits[1]=0x0000f000 },
    { .bits[0] = 0x0000fe00, .bits[1]=0x00000000 },
    { .bits[0] = 0x000001ff, .bits[1]=0x00000000 },
    { .bits[0] = 0x00f00000, .bits[1]=0x00000500 },
    { .bits[0] = 0x00c10200, .bits[1]=0x00000008 },

};
#else
static rtk_portmask_t trunk_memportMask[] =
{
    { .bits[0] = 0x00000001 },
    { .bits[0] = 0x00000100 },
    { .bits[0] = 0x000000ff },
    { .bits[0] = 0x000000f0 },
    { .bits[0] = 0x0000fe00 },
    { .bits[0] = 0x000001ff },
    { .bits[0] = 0x00f00000 },
    { .bits[0] = 0x00c10200 },

};
#endif

int32 dal_trunk_port_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  portMask_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_port_set(unit, 0, &portMask_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_port_get(unit, 0, &portMask_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  trunk_id[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_result[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_index;
        int32  trunk_id_last;

        rtk_portmask_t portMsk;
        int32  portMsk_index;
        int32  portMsk_last = sizeof(trunk_memportMask)/sizeof(rtk_portmask_t);
        uint8           portMsk_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];

        rtk_portmask_t  result_portMsk;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_ID_MAX(unit), TEST_TRUNK_ID_MIN, trunk_id, trunk_id_result, trunk_id_last);

        for (trunk_id_index = 0; trunk_id_index <= trunk_id_last; trunk_id_index++)
        {
            inter_result2 = trunk_id_result[trunk_id_index];
            for (portMsk_index = 0; portMsk_index <= portMsk_last; portMsk_index++)
            {
                memset(&portMsk, 0 ,sizeof(rtk_portmask_t));
                portMsk.bits[0] = trunk_memportMask[portMsk_index].bits[0];
                if(HWP_8390_50_FAMILY(unit) || HWP_9310_FAMILY_ID(unit))
                    portMsk.bits[1] = trunk_memportMask[portMsk_index].bits[1];

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trunk_port_set(unit, trunk_id[trunk_id_index], &portMsk);
                if (RT_ERR_OK == ret)
                {
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_port_set (unit %u, trunk_id %u, portMsk %s)"
                                    , ret, expect_result, unit, trunk_id[trunk_id_index], portMsk_pm_str);

                    ret = rtk_trunk_port_get(unit, trunk_id[trunk_id_index], &result_portMsk);
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_port_get (unit %u, trunk_id %u, portMsk %s)"
                                     , ret, expect_result, unit, trunk_id[trunk_id_index], portMsk_pm_str);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_port_set/get value unequal (unit %u, trunk_id %u, portMsk.bits[0] 0x%x)"
                                     , result_portMsk.bits[0], portMsk.bits[0], unit, trunk_id[trunk_id_index], portMsk.bits[0]);
                    if(HWP_8390_50_FAMILY(unit))
                        RT_TEST_IS_EQUAL_INT("rtk_trunk_port_set/get value unequal (unit %u, trunk_id %u, portMsk.bits[1] 0x%x)"
                                         , result_portMsk.bits[1], portMsk.bits[1], unit, trunk_id[trunk_id_index], portMsk.bits[1]);

                }
                else
                {
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_port_set (unit %u, trunk_id %u, portMsk %s)"
                                        , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index], portMsk_pm_str);
                }
            }

            ret = rtk_trunk_port_get(unit, trunk_id[trunk_id_index], &result_portMsk);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trunk_port_set (unit %u, trunk_id %u)"
                                , ret, expect_result, unit, trunk_id[trunk_id_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_port_set (unit %u, trunk_id %u)"
                                    , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index]);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trunk_port_test*/

int32 dal_trunk_distributionAlgorithmParam_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  hashAlgoMsk_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmParam_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmParam_get(unit, 0, &hashAlgoMsk_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  hashAlgo_id[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgo_id_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgo_id_index;
        int32  hashAlgo_id_last;

        uint32 hashAlgoMsk[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoMsk_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoMsk_index;
        int32  hashAlgoMsk_last;

        int32  result_hashAlgoMsk;



        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOID_MAX(unit), TEST_TRUNK_HASHALGOID_MIN, hashAlgo_id, hashAlgo_id_result, hashAlgo_id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGO_MAX(unit), TEST_TRUNK_HASHALGO_MIN, hashAlgoMsk, hashAlgoMsk_result, hashAlgoMsk_last);

        for (hashAlgo_id_index = 0; hashAlgo_id_index <= hashAlgo_id_last; hashAlgo_id_index++)
        {
            inter_result2 = hashAlgo_id_result[hashAlgo_id_index];
            for (hashAlgoMsk_index = 0; hashAlgoMsk_index <= hashAlgoMsk_last; hashAlgoMsk_index++)
            {
                inter_result3 = hashAlgoMsk_result[hashAlgoMsk_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trunk_distributionAlgorithmParam_set(unit, hashAlgo_id[hashAlgo_id_index], hashAlgoMsk[hashAlgoMsk_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                    , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index], hashAlgoMsk[hashAlgoMsk_index]);

                    ret = rtk_trunk_distributionAlgorithmParam_get(unit, hashAlgo_id[hashAlgo_id_index], (uint32*)&result_hashAlgoMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_get (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                     , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index], hashAlgoMsk[hashAlgoMsk_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                     , result_hashAlgoMsk, hashAlgoMsk[hashAlgoMsk_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoMsk[hashAlgoMsk_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                        , ret, RT_ERR_OK, unit, hashAlgo_id[hashAlgo_id_index], hashAlgoMsk[hashAlgoMsk_index]);
                }
            }

            ret = rtk_trunk_distributionAlgorithmParam_get(unit, hashAlgo_id[hashAlgo_id_index], (uint32*)&result_hashAlgoMsk);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u)"
                                , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u)"
                                    , ret, RT_ERR_OK, unit, hashAlgo_id[hashAlgo_id_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        int32  hashAlgo_id;
        int32  hashAlgo_id_max;
        uint32  hashAlgoMsk;
        uint32  hashAlgoMsk_max;
        uint32  result_hashAlgoMsk;

        G_VALUE(TEST_TRUNK_HASHALGOID_MAX(unit), hashAlgo_id_max);
        G_VALUE(TEST_TRUNK_HASHALGO_MAX(unit), hashAlgoMsk_max);

        for (L_VALUE(TEST_TRUNK_HASHALGOID_MIN, hashAlgo_id); hashAlgo_id <= hashAlgo_id_max; hashAlgo_id++)
        {
            ASSIGN_EXPECT_RESULT(hashAlgo_id, TEST_TRUNK_HASHALGOID_MIN, TEST_TRUNK_HASHALGOID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_TRUNK_HASHALGO_MIN, hashAlgoMsk); hashAlgoMsk <= hashAlgoMsk_max; hashAlgoMsk++)
            {
                ASSIGN_EXPECT_RESULT(hashAlgoMsk, TEST_TRUNK_HASHALGO_MIN, TEST_TRUNK_HASHALGO_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_trunk_distributionAlgorithmParam_set(unit, hashAlgo_id, hashAlgoMsk);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                    , ret, expect_result, unit, hashAlgo_id, hashAlgoMsk);
                    ret = rtk_trunk_distributionAlgorithmParam_get(unit, hashAlgo_id, (uint32*)&result_hashAlgoMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_get (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                     , ret, expect_result, unit, hashAlgo_id, hashAlgoMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                     , result_hashAlgoMsk, hashAlgoMsk, unit, hashAlgo_id, hashAlgoMsk);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u, hashAlgoMsk %u)"
                                        , ret, RT_ERR_OK, unit, hashAlgo_id, hashAlgoMsk);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trunk_distributionAlgorithmParam_test*/

int32 dal_trunk_distributionAlgorithmShift_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_trunk_distAlgoShift_t  shift_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmShift_set(unit, 0, &shift_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmShift_get(unit, 0, &shift_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 hashAlgo_id[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgo_id_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgo_id_index;
        int32  hashAlgo_id_last;

        uint32 hashAlgoSpaShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSpaShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSpaShift_index;
        int32  hashAlgoSpaShift_last;

        uint32 hashAlgoSmacShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSmacShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSmacShift_index;
        int32  hashAlgoSmacShift_last;

        uint32 hashAlgoDmacShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDmacShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDmacShift_index;
        int32  hashAlgoDmacShift_last;

        uint32 hashAlgoSipShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSipShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSipShift_index;
        int32  hashAlgoSipShift_last;

        uint32 hashAlgoDipShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDipShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDipShift_index;
        int32  hashAlgoDipShift_last;

        uint32 hashAlgoSportShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSportShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoSportShift_index;
        int32  hashAlgoSportShift_last;

        uint32 hashAlgoDportShift[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDportShift_result[UNITTEST_MAX_TEST_VALUE];
        int32  hashAlgoDportShift_index;
        int32  hashAlgoDportShift_last;

        rtk_trunk_distAlgoShift_t hashAlgoShift;
        rtk_trunk_distAlgoShift_t result_hashAlgoShift;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOID_MAX(unit), TEST_TRUNK_HASHALGOID_MIN, hashAlgo_id, hashAlgo_id_result, hashAlgo_id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSpaShift, hashAlgoSpaShift_result, hashAlgoSpaShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSmacShift, hashAlgoSmacShift_result, hashAlgoSmacShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDmacShift, hashAlgoDmacShift_result, hashAlgoDmacShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSipShift, hashAlgoSipShift_result, hashAlgoSipShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDipShift, hashAlgoDipShift_result, hashAlgoDipShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSportShift, hashAlgoSportShift_result, hashAlgoSportShift_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDportShift, hashAlgoDportShift_result, hashAlgoDportShift_last);

        for (hashAlgo_id_index = 0; hashAlgo_id_index <= hashAlgo_id_last; hashAlgo_id_index++)
        {
            inter_result2 = hashAlgo_id_result[hashAlgo_id_index];
            for (hashAlgoSpaShift_index = 0; hashAlgoSpaShift_index <= hashAlgoSpaShift_last; hashAlgoSpaShift_index++)
            {
                for (hashAlgoSmacShift_index = 0; hashAlgoSmacShift_index <= hashAlgoSmacShift_last; hashAlgoSmacShift_index++)
                {
                    for (hashAlgoDmacShift_index = 0; hashAlgoDmacShift_index <= hashAlgoDmacShift_last; hashAlgoDmacShift_index++)
                    {
                        for (hashAlgoSipShift_index = 0; hashAlgoSipShift_index <= hashAlgoSipShift_last; hashAlgoSipShift_index++)
                        {
                            for (hashAlgoDipShift_index = 0; hashAlgoDipShift_index <= hashAlgoDipShift_last; hashAlgoDipShift_index++)
                            {
                                for (hashAlgoSportShift_index = 0; hashAlgoSportShift_index <= hashAlgoSportShift_last; hashAlgoSportShift_index++)
                                {
                                    for (hashAlgoDportShift_index = 0; hashAlgoDportShift_index <= hashAlgoDportShift_last; hashAlgoDportShift_index++)
                                    {
                                        inter_result3 = (hashAlgoSpaShift_result[hashAlgoSpaShift_index] && hashAlgoSmacShift_result[hashAlgoSmacShift_index] &&
                                                        hashAlgoDmacShift_result[hashAlgoDmacShift_index] && hashAlgoSipShift_result[hashAlgoSipShift_index] &&
                                                        hashAlgoDipShift_result[hashAlgoDipShift_index] && hashAlgoSportShift_result[hashAlgoSportShift_index] &&
                                                        hashAlgoDportShift_result[hashAlgoDportShift_index]);
                                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                            expect_result = RT_ERR_FAILED;
                                        else
                                            expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                                        hashAlgoShift.spa_shift = hashAlgoSpaShift[hashAlgoSpaShift_index];
                                        hashAlgoShift.smac_shift = hashAlgoSmacShift[hashAlgoSmacShift_index];
                                        hashAlgoShift.dmac_shift = hashAlgoDmacShift[hashAlgoDmacShift_index];
                                        hashAlgoShift.sip_shift = hashAlgoSipShift[hashAlgoSipShift_index];
                                        hashAlgoShift.dip_shift = hashAlgoDipShift[hashAlgoDipShift_index];
                                        hashAlgoShift.sport_shift = hashAlgoSportShift[hashAlgoSportShift_index];
                                        hashAlgoShift.dport_shift = hashAlgoDportShift[hashAlgoDportShift_index];
                                        ret = rtk_trunk_distributionAlgorithmShift_set(unit, hashAlgo_id[hashAlgo_id_index], &hashAlgoShift);
                                        if (RT_ERR_OK == expect_result)
                                        {
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmParam_set (unit %u, hashAlgo_id %u)"
                                                            , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index]);

                                            ret = rtk_trunk_distributionAlgorithmShift_get(unit, hashAlgo_id[hashAlgo_id_index], &result_hashAlgoShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_get (unit %u, hashAlgo_id %u)"
                                                             , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.spa_shift %u)"
                                                             , result_hashAlgoShift.spa_shift, hashAlgoSpaShift[hashAlgoSpaShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoSpaShift[hashAlgoSpaShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.smac_shift %u)"
                                                             , result_hashAlgoShift.smac_shift, hashAlgoSmacShift[hashAlgoSmacShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoSmacShift[hashAlgoSmacShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dmac_shift %u)"
                                                             , result_hashAlgoShift.dmac_shift, hashAlgoDmacShift[hashAlgoDmacShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoDmacShift[hashAlgoDmacShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.sip_shift %u)"
                                                             , result_hashAlgoShift.sip_shift, hashAlgoSipShift[hashAlgoSipShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoSipShift[hashAlgoSipShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dip_shift %u)"
                                                             , result_hashAlgoShift.dip_shift, hashAlgoDipShift[hashAlgoDipShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoDipShift[hashAlgoDipShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.sport_shift %u)"
                                                             , result_hashAlgoShift.sport_shift, hashAlgoSportShift[hashAlgoSportShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoSportShift[hashAlgoSportShift_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dport_shift %u)"
                                                             , result_hashAlgoShift.dport_shift, hashAlgoDportShift[hashAlgoDportShift_index], unit, hashAlgo_id[hashAlgo_id_index], hashAlgoDportShift[hashAlgoDportShift_index]);
                                        }
                                        else
                                        {
                                            RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set (unit %u, hashAlgo_id %u)"
                                                                , ret, RT_ERR_OK, unit, hashAlgo_id[hashAlgo_id_index]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ret = rtk_trunk_distributionAlgorithmShift_get(unit, hashAlgo_id[hashAlgo_id_index], &result_hashAlgoShift);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_get (unit %u, hashAlgo_id %u)"
                                , ret, expect_result, unit, hashAlgo_id[hashAlgo_id_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_get (unit %u, hashAlgo_id %u)"
                                    , ret, RT_ERR_OK, unit, hashAlgo_id[hashAlgo_id_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        int32  hashAlgo_id;
        int32  hashAlgo_id_max;
        uint32  hashAlgoSpaShift;
        uint32  hashAlgoSpaShift_max;
        uint32  hashAlgoSmacShift;
        uint32  hashAlgoSmacShift_max;
        uint32  hashAlgoDmacShift;
        uint32  hashAlgoDmacShift_max;
        uint32  hashAlgoSipShift;
        uint32  hashAlgoSipShift_max;
        uint32  hashAlgoDipShift;
        uint32  hashAlgoDipShift_max;
        uint32  hashAlgoSportShift;
        uint32  hashAlgoSportShift_max;
        uint32  hashAlgoDportShift;
        uint32  hashAlgoDportShift_max;
        rtk_trunk_distAlgoShift_t hashAlgoShift;
        rtk_trunk_distAlgoShift_t result_hashAlgoShift;
        int8  temp_inter_result3_1 = 1;
        int8  temp_inter_result3_2 = 1;
        int8  temp_inter_result3_3 = 1;
        int8  temp_inter_result3_4 = 1;
        int8  temp_inter_result3_5 = 1;
        int8  temp_inter_result3_6 = 1;
        int8  temp_inter_result3_7 = 1;

        G_VALUE(TEST_TRUNK_HASHALGOID_MAX(unit), hashAlgo_id_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoSpaShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoSmacShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoDmacShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoSipShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoDipShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoSportShift_max);
        G_VALUE(TEST_TRUNK_HASHALGOSHIFT_MAX(unit), hashAlgoDportShift_max);

        for (L_VALUE(TEST_TRUNK_HASHALGOID_MIN, hashAlgo_id); hashAlgo_id <= hashAlgo_id_max; hashAlgo_id++)
        {
            ASSIGN_EXPECT_RESULT(hashAlgo_id, TEST_TRUNK_HASHALGOID_MIN, TEST_TRUNK_HASHALGOID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSpaShift); hashAlgoSpaShift <= hashAlgoSpaShift_max; hashAlgoSpaShift++)
            {
                ASSIGN_EXPECT_RESULT(hashAlgoSpaShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_1);
                for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSmacShift); hashAlgoSmacShift <= hashAlgoSmacShift_max; hashAlgoSmacShift++)
                {
                    ASSIGN_EXPECT_RESULT(hashAlgoSmacShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_2);
                    for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDmacShift); hashAlgoDmacShift <= hashAlgoDmacShift_max; hashAlgoDmacShift++)
                    {
                        ASSIGN_EXPECT_RESULT(hashAlgoDmacShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_3);
                        for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSipShift); hashAlgoSipShift <= hashAlgoSipShift_max; hashAlgoSipShift++)
                        {
                            ASSIGN_EXPECT_RESULT(hashAlgoSipShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_4);
                            for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDipShift); hashAlgoDipShift <= hashAlgoDipShift_max; hashAlgoDipShift++)
                            {
                                ASSIGN_EXPECT_RESULT(hashAlgoDipShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_5);
                                for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoSportShift); hashAlgoSportShift <= hashAlgoSportShift_max; hashAlgoSportShift++)
                                {
                                    ASSIGN_EXPECT_RESULT(hashAlgoSportShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_6);
                                    for (L_VALUE(TEST_TRUNK_HASHALGOSHIFT_MIN, hashAlgoDportShift); hashAlgoDportShift <= hashAlgoDportShift_max; hashAlgoDportShift++)
                                    {
                                        ASSIGN_EXPECT_RESULT(hashAlgoDportShift, TEST_TRUNK_HASHALGOSHIFT_MIN, TEST_TRUNK_HASHALGOSHIFT_MAX(unit), temp_inter_result3_7);
                                        inter_result3 = (temp_inter_result3_1 && temp_inter_result3_2 && temp_inter_result3_3 && temp_inter_result3_4 &&
                                                         temp_inter_result3_5 && temp_inter_result3_6 && temp_inter_result3_7);
                                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                            expect_result = RT_ERR_FAILED;
                                        else
                                            expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                                        hashAlgoShift.spa_shift = hashAlgoSpaShift;
                                        hashAlgoShift.smac_shift = hashAlgoSmacShift;
                                        hashAlgoShift.dmac_shift = hashAlgoDmacShift;
                                        hashAlgoShift.sip_shift = hashAlgoSipShift;
                                        hashAlgoShift.dip_shift = hashAlgoDipShift;
                                        hashAlgoShift.sport_shift = hashAlgoSportShift;
                                        hashAlgoShift.dport_shift = hashAlgoDportShift;
                                        ret = rtk_trunk_distributionAlgorithmShift_set(unit, hashAlgo_id, &hashAlgoShift);
                                        if (expect_result == RT_ERR_OK)
                                        {
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set (unit %u, hashAlgo_id %u)"
                                                            , ret, expect_result, unit, hashAlgo_id);
                                            ret = rtk_trunk_distributionAlgorithmShift_get(unit, hashAlgo_id, &result_hashAlgoShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_get (unit %u, hashAlgo_id %u)"
                                                             , ret, expect_result, unit, hashAlgo_id);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.spa_shift %u)"
                                                             , result_hashAlgoShift.spa_shift, hashAlgoSpaShift, unit, hashAlgo_id, hashAlgoSpaShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.smac_shift %u)"
                                                             , result_hashAlgoShift.smac_shift, hashAlgoSmacShift, unit, hashAlgo_id, hashAlgoSmacShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dmac_shift %u)"
                                                             , result_hashAlgoShift.dmac_shift, hashAlgoDmacShift, unit, hashAlgo_id, hashAlgoDmacShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.sip_shift %u)"
                                                             , result_hashAlgoShift.sip_shift, hashAlgoSipShift, unit, hashAlgo_id, hashAlgoSipShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dip_shift %u)"
                                                             , result_hashAlgoShift.dip_shift, hashAlgoDipShift, unit, hashAlgo_id, hashAlgoDipShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.sport_shift %u)"
                                                             , result_hashAlgoShift.sport_shift, hashAlgoSportShift, unit, hashAlgo_id, hashAlgoSportShift);
                                            RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set/get value unequal (unit %u, hashAlgo_id %u, hashAlgoShift.dport_shift %u)"
                                                             , result_hashAlgoShift.dport_shift, hashAlgoDportShift, unit, hashAlgo_id, hashAlgoDportShift);
                                        }
                                        else
                                        {
                                            RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmShift_set (unit %u, hashAlgo_id %u"
                                                                , ret, RT_ERR_OK, unit, hashAlgo_id);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trunk_distributionAlgorithmShift_test*/

int32 dal_trunk_trafficSeparate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  separateType_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_trafficSeparate_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_trafficSeparate_get(unit, 0, &separateType_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  trunk_id[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_result[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_index;
        int32  trunk_id_last;

        uint32 separateType[UNITTEST_MAX_TEST_VALUE];
        int32  separateType_result[UNITTEST_MAX_TEST_VALUE];
        int32  separateType_index;
        int32  separateType_last;
        rtk_trunk_separateType_t result_separateType;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_ID_MAX(unit), TEST_TRUNK_ID_MIN, trunk_id, trunk_id_result, trunk_id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_SEPARATETYPE_MAX, TEST_TRUNK_SEPARATETYPE_MIN, separateType, separateType_result, separateType_last);

        for (trunk_id_index = 0; trunk_id_index <= trunk_id_last; trunk_id_index++)
        {
            inter_result2 = trunk_id_result[trunk_id_index];
            for (separateType_index = 0; separateType_index <= separateType_last; separateType_index++)
            {
                inter_result3 = separateType_result[separateType_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_trunk_trafficSeparate_set(unit, trunk_id[trunk_id_index], separateType[separateType_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_trafficSeparate_set (unit %u, trunk_id %u, separateType %u)"
                                    , ret, expect_result, unit, trunk_id[trunk_id_index], separateType[separateType_index]);

                    ret = rtk_trunk_trafficSeparate_get(unit, trunk_id[trunk_id_index], &result_separateType);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_trafficSeparate_get (unit %u, trunk_id %u, separateType %u)"
                                     , ret, expect_result, unit, trunk_id[trunk_id_index], separateType[separateType_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_trafficSeparate_set/get value unequal (unit %u, trunk_id %u, separateType %u)"
                                     , result_separateType, separateType[separateType_index], unit, trunk_id[trunk_id_index], separateType[separateType_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_trafficSeparate_set (unit %u, trunk_id %u, separateType %u)"
                                        , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index], separateType[separateType_index]);
                }
            }

            ret = rtk_trunk_trafficSeparate_get(unit, trunk_id[trunk_id_index], &result_separateType);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trunk_trafficSeparate_set (unit %u, trunk_id %u)"
                                , ret, expect_result, unit, trunk_id[trunk_id_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_trafficSeparate_set (unit %u, trunk_id %u)"
                                    , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index]);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trunk_trafficSeparate_test*/

int32 dal_trunk_distributionAlgorithmBind_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  algoIdx_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmBind_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_trunk_distributionAlgorithmBind_get(unit, 0, &algoIdx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  trunk_id[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_result[UNITTEST_MAX_TEST_VALUE];
        int32  trunk_id_index;
        int32  trunk_id_last;

        uint32 distributeAlgoBind[UNITTEST_MAX_TEST_VALUE];
        int32  distributeAlgoBind_result[UNITTEST_MAX_TEST_VALUE];
        int32  distributeAlgoBind_index;
        int32  distributeAlgoBind_last;
        uint32 result_distributeAlgoBind;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_ID_MAX(unit), TEST_TRUNK_ID_MIN, trunk_id, trunk_id_result, trunk_id_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TRUNK_HASHALGOID_MAX(unit), TEST_TRUNK_HASHALGOID_MIN, distributeAlgoBind, distributeAlgoBind_result, distributeAlgoBind_last);

        for (trunk_id_index = 0; trunk_id_index <= trunk_id_last; trunk_id_index++)
        {
            inter_result2 = trunk_id_result[trunk_id_index];
            for (distributeAlgoBind_index = 0; distributeAlgoBind_index <= distributeAlgoBind_last; distributeAlgoBind_index++)
            {
                inter_result3 = distributeAlgoBind_result[distributeAlgoBind_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_trunk_distributionAlgorithmBind_set(unit, trunk_id[trunk_id_index], distributeAlgoBind[distributeAlgoBind_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_set (unit %u, trunk_id %u, distributeAlgoBind %u)"
                                    , ret, expect_result, unit, trunk_id[trunk_id_index], distributeAlgoBind[distributeAlgoBind_index]);

                    ret = rtk_trunk_distributionAlgorithmBind_get(unit, trunk_id[trunk_id_index], &result_distributeAlgoBind);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_get (unit %u, trunk_id %u, distributeAlgoBind %u)"
                                     , ret, expect_result, unit, trunk_id[trunk_id_index], distributeAlgoBind[distributeAlgoBind_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_set/get value unequal (unit %u, trunk_id %u, distributeAlgoBind %u)"
                                     , result_distributeAlgoBind, distributeAlgoBind[distributeAlgoBind_index], unit, trunk_id[trunk_id_index], distributeAlgoBind[distributeAlgoBind_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_set (unit %u, trunk_id %u, distributeAlgoBind %u)"
                                        , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index], distributeAlgoBind[distributeAlgoBind_index]);
                }
            }

            ret = rtk_trunk_distributionAlgorithmBind_get(unit, trunk_id[trunk_id_index], &result_distributeAlgoBind);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_set (unit %u, trunk_id %u)"
                                , ret, expect_result, unit, trunk_id[trunk_id_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_trunk_distributionAlgorithmBind_set (unit %u, trunk_id %u)"
                                    , ret, RT_ERR_OK, unit, trunk_id[trunk_id_index]);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_trunk_distributionAlgorithmBind_test*/
