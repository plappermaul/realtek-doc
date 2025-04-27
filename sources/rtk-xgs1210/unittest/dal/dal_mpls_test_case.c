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
#include <rtk/mpls.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/* MPLS TTL inherit type */
#define TEST_MIN_MPLS_TTLINHERIT    0 /* RTK_MPLS_TTL_INHERIT_UNIFORM */
#define TEST_MAX_MPLS_TTLINHERIT    (RTK_MPLS_TTL_INHERIT_MAX-1)
/* MPLS lib index */
#define TEST_MIN_MPLS_LIB_INDEX 0
#define TEST_MAX_MPLS_LIB_INDEX_FOR_DOUBLE(unit) (HAL_MAX_NUM_OF_MPLS_LIB(unit)-1)
#define TEST_MAX_MPLS_LIB_INDEX_FOR_SINGLE(unit) (2*HAL_MAX_NUM_OF_MPLS_LIB(unit)-1)
/* MPLS label operation */
#define TEST_MIN_MPLS_LABEL_OPER    RTK_MPLS_LABEL_OPER_SINGLE
#define TEST_MAX_MPLS_LABEL_OPER    RTK_MPLS_LABEL_OPER_DOUBEL

int32 dal_mpls_ttlInherit_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  ttlInheritType_temp;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_ttlInherit_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_ttlInherit_get(unit, &ttlInheritType_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_mpls_ttlInherit_t  ttlInheritType[UNITTEST_MAX_TEST_VALUE];
        int32  ttlInheritType_result[UNITTEST_MAX_TEST_VALUE];
        int32  ttlInheritType_index;
        int32  ttlInheritType_last;

        int32  result_ttlInheritType;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_MPLS_TTLINHERIT, TEST_MIN_MPLS_TTLINHERIT, ttlInheritType, ttlInheritType_result, ttlInheritType_last);

        for (ttlInheritType_index = 0; ttlInheritType_index <= ttlInheritType_last; ttlInheritType_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (ttlInheritType_result[ttlInheritType_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_mpls_ttlInherit_set(unit, ttlInheritType[ttlInheritType_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_mpls_ttlInherit_set (unit %u, ttlInheritType %u)"
                                , ret, expect_result, unit, ttlInheritType[ttlInheritType_index]);

                ret = rtk_mpls_ttlInherit_get(unit, (uint32*)&result_ttlInheritType);
                RT_TEST_IS_EQUAL_INT("rtk_mpls_ttlInherit_get (unit %u, ttlInheritType %u)"
                                 , ret, expect_result, unit, ttlInheritType[ttlInheritType_index]);
                RT_TEST_IS_EQUAL_INT("rtk_mpls_ttlInherit_set/get value unequal (unit %u, ttlInheritType %u)"
                                 , result_ttlInheritType, ttlInheritType[ttlInheritType_index], unit, ttlInheritType[ttlInheritType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_ttlInherit_set (unit %u, ttlInheritType %u)"
                                    , ret, RT_ERR_OK, unit, ttlInheritType[ttlInheritType_index]);
            }
        }

        ret = rtk_mpls_ttlInherit_get(unit, (uint32*)&result_ttlInheritType);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_mpls_ttlInherit_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_ttlInherit_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_mpls_encap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mpls_encap_t  encap_temp;
    int32  expect_result = 1;
    uint32 label_idx = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_encap_set(unit, 0, &encap_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_encap_get(unit, 0, &encap_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  libIdx[UNITTEST_MAX_TEST_VALUE];
        int32  libIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  libIdx_index;
        int32  libIdx_last;

        uint32  labelOper[UNITTEST_MAX_TEST_VALUE];
        int32  labelOper_result[UNITTEST_MAX_TEST_VALUE];
        int32  labelOper_index;
        int32  labelOper_last;

        rtk_mpls_encap_t  encap;
        rtk_mpls_encap_t  result_encap;

       //UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_MPLS_LIB_INDEX, TEST_MIN_MPLS_LIB_INDEX, libIdx, libIdx_result, libIdx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_MPLS_LABEL_OPER, TEST_MIN_MPLS_LABEL_OPER, labelOper, labelOper_result, labelOper_last);

        for (labelOper_index = 0; labelOper_index <= labelOper_last; labelOper_index++)
        {
            if (RTK_MPLS_LABEL_OPER_SINGLE == labelOper[labelOper_index])
                UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_MPLS_LIB_INDEX_FOR_SINGLE(unit), TEST_MIN_MPLS_LIB_INDEX, libIdx, libIdx_result, libIdx_last);
            else
                UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_MPLS_LIB_INDEX_FOR_DOUBLE(unit), TEST_MIN_MPLS_LIB_INDEX, libIdx, libIdx_result, libIdx_last);
            for (libIdx_index = 0; libIdx_index <= libIdx_last; libIdx_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (libIdx_result[libIdx_index] && labelOper_result[labelOper_index])? RT_ERR_OK : RT_ERR_FAILED;
                osal_memset(&encap, 0, sizeof(encap));
                osal_memset(&result_encap, 0, sizeof(result_encap));

                encap.oper = labelOper[labelOper_index];
                encap.label0 = ut_rand()%0x100000;
                encap.label1 = ut_rand()%0x100000;
                encap.exp0 = ut_rand()%8;
                encap.exp1 = ut_rand()%8;
                encap.ttl0 = ut_rand()%256;
                encap.ttl1 = ut_rand()%256;

                ret = rtk_mpls_encap_set(unit, libIdx[libIdx_index], &encap);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set (unit %u, libIdx %u)"
                                    , ret, expect_result, unit, libIdx[libIdx_index]);

                    result_encap.oper = labelOper[labelOper_index];
                    ret = rtk_mpls_encap_get(unit, libIdx[libIdx_index], &result_encap);
                    RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_get (unit %u, libIdx %u)"
                                     , ret, expect_result, unit, libIdx[libIdx_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                     , result_encap.oper, encap.oper, unit, libIdx[libIdx_index], encap.oper);
                    label_idx = 0;
                    if (RTK_MPLS_LABEL_OPER_SINGLE == encap.oper)
                        label_idx = libIdx[libIdx_index] & 0x1;
                    if (0 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == encap.oper)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.label0, encap.label0, unit, libIdx[libIdx_index], encap.label0);
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.exp0, encap.exp0, unit, libIdx[libIdx_index], encap.exp0);
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.ttl0, encap.ttl0, unit, libIdx[libIdx_index], encap.ttl0);
                    }
                    if (1 == label_idx || RTK_MPLS_LABEL_OPER_DOUBEL == encap.oper)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.label1, encap.label1, unit, libIdx[libIdx_index], encap.label1);
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.exp1, encap.exp1, unit, libIdx[libIdx_index], encap.exp1);
                        RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_set/get value unequal (unit %u, libIdx %u, encap.oper %u)"
                                         , result_encap.ttl1, encap.ttl1, unit, libIdx[libIdx_index], encap.ttl1);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_encap_set (unit %u, libIdx %u)"
                                        , ret, RT_ERR_OK, unit, libIdx[libIdx_index]);
                }

                osal_memset(&result_encap, 0, sizeof(result_encap));
                result_encap.oper = labelOper[labelOper_index];
                ret = rtk_mpls_encap_get(unit, libIdx[libIdx_index], &result_encap);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (libIdx_result[libIdx_index] && labelOper_result[labelOper_index])? RT_ERR_OK : RT_ERR_FAILED;

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_mpls_encap_get (unit %u, libIdx %u, labelOper %u)"
                                    , ret, expect_result, unit, libIdx[libIdx_index], labelOper_result[labelOper_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_encap_get (unit %u, libIdx %u, labelOper %u)"
                                        , ret, RT_ERR_OK, unit, libIdx[libIdx_index], labelOper_result[labelOper_index]);
                }
            }
        }
    }

    return RT_ERR_OK;
}
int32 dal_mpls_enable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  enable_temp;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_enable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_mpls_enable_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_mpls_enable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_mpls_enable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_mpls_enable_get(unit, (uint32*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_mpls_enable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_mpls_enable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_enable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }

        ret = rtk_mpls_enable_get(unit, (uint32*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_mpls_enable_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_mpls_enable_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}
