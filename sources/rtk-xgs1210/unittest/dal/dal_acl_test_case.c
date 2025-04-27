/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 74798 $
 * $Date: 2016-12-29 10:12:36 +0800 (Thu, 29 Dec 2016) $
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
#include <rtk/acl.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/* ACL Block Index */
#define TEST_BLOCK_ID_MIN   0
#define TEST_BLOCK_ID_MAX(unit)   (HAL_MAX_NUM_OF_PIE_BLOCK(unit)-1)
/* Phase Value */
#define TEST_MIN_PHASE_VALUE    ACL_PHASE_IGR_ACL
#define TEST_MAX_PHASE_VALUE(unit)    (HWP_8390_50_FAMILY(unit)?(ACL_PHASE_END - 1):(ACL_PHASE_IGR_ACL))

/* ACL TEMPLATE TYPE */
#define TEST_MIN_TEMPLATE_TYPE  USER_FIELD_TEMPLATE_ID
#define TEST_MAX_TEMPLATE_TYPE  (USER_FIELD_END - 1)
/* ACL Entry Index */
#define TEST_ENTRY_ID_MIN   0
#define TEST_ENTRY_ID_MAX(unit)   (HWP_8390_50_FAMILY(unit)?(2048 + 256 - 1):(1536 - 1))

/* blockResultMode */
#define TEST_BLOCKRESULTMODE_MIN   ACL_BLOCK_RESULT_SINGLE
#define TEST_BLOCKRESULTMODE_MAX   (ACL_BLOCK_RESULT_END - 1)
/* blockAggrType */
#define TEST_BLOCKGROUPTYPE_MIN  ACL_BLOCK_GROUP_1
#define TEST_BLOCKGROUPTYPE_MAX  (ACL_BLOCK_GROUP_END-1)
/* Packet logId */
#define TEST_PKT_LOG_ID_MIN 0
#define TEST_PKT_LOG_ID_MAX(unit) (2*HAL_MAX_NUM_OF_PIE_COUNTER(unit)-1)
/* Byte logId */
#define TEST_BYTE_LOG_ID_MIN 0
#define TEST_BYTE_LOG_ID_MAX(unit) (HAL_MAX_NUM_OF_PIE_COUNTER(unit)-1)
/* range check l4 port index */
#define TEST_MIN_RC_L4PORT_INDEX    0
#define TEST_MAX_RC_L4PORT_INDEX(unit)    (HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(unit)-1)
/* range check vid index */
#define TEST_MIN_RC_VID_INDEX   0
#define TEST_MAX_RC_VID_INDEX(unit)   (HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)-1)
/* range check ip index */
#define TEST_MIN_RC_IP_INDEX    0
#define TEST_MAX_RC_IP_INDEX(unit)    (HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)-1)
/* range check source portmask index */
#define TEST_MIN_RC_SRCPORT_INDEX   0
#define TEST_MAX_RC_SRCPORT_INDEX(unit)   (HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)-1)
/* range check destination portmask index */
#define TEST_MIN_RC_DSTPORT_INDEX   0
#define TEST_MAX_RC_DSTPORT_INDEX(unit)   (HAL_MAX_NUM_OF_RANGE_CHECK_DSTPORT(unit)-1)
/* Packet Length */
#define TEST_MIN_RC_PKTLEN_INDEX    0
#define TEST_MAX_RC_PKTLEN_INDEX(unit)    (HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)-1)
/* Field Select Index */
#define TEST_MIN_FS_INDEX   0
#define TEST_MAX_FS_INDEX(unit)   (HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)-1)
/* Field Select Start Position */
#define TEST_MIN_FS_START_POS   FS_START_POS_RAW
#define TEST_MAX_FS_START_POS  (FS_START_POS_END-1)
/* Field Select Offset Value */
#define TEST_MIN_FS_OFFSET  0
#define TEST_MAX_FS_OFFSET(unit)  (HWP_8390_50_FAMILY(unit)?255:178)

/* Meter Block Index */
#define TEST_METER_BLOCK_ID_MIN   0
#define TEST_METER_BLOCK_ID_MAX   15
/* ACL meter mode */
#define TEST_METERMODE_MIN  0 /* METER_MODE_BYTE */
#define TEST_METERMODE_MAX  (METER_MODE_END-1)
/* Meter index */
#define TEST_METER_INDEX_MIN    0
#define TEST_METER_INDEX_MAX(unit)    (HAL_MAX_NUM_OF_METERING(unit)-1)
/* ACL entry length */
#define TEST_ACL_ENTRY_LENGTH   56
/* ACL template index */
#define TEST_TEMPLATE_ID_MIN    0
#define TEST_TEMPLATE_ID_MAX(unit)    (HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)-1)
/* ACL user template index */
#define TEST_USER_TEMPLATE_ID_MIN(unit) (HAL_PIE_USER_TEMPLATE_ID_MIN(unit))
#define TEST_USER_TEMPLATE_ID_MAX(unit) (HAL_PIE_USER_TEMPLATE_ID_MAX(unit))
#define TEST_METER_MODE_INDEX_MIN(unit)   (HWP_8390_50_FAMILY(unit)?TEST_METER_BLOCK_ID_MIN:TEST_METER_INDEX_MIN)
#define TEST_METER_MODE_INDEX_MAX(unit)   (HWP_8390_50_FAMILY(unit)?TEST_METER_BLOCK_ID_MAX:TEST_METER_INDEX_MAX(unit))

/* Function Declaration */
static int32 init_action_table(uint32 phase, rtk_acl_action_t *action_table, uint32 unit);

/* Function Body */
int32 dal_acl_partition_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  partition_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_partition_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_partition_get(unit, &partition_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  partition[UNITTEST_MAX_TEST_VALUE];
        int32  partition_result[UNITTEST_MAX_TEST_VALUE];
        int32  partition_index;
        int32  partition_last;

        uint32  result_partition;


        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_PIE_BLOCK(unit), 0, partition, partition_result, partition_last);

        for (partition_index = 0; partition_index <= partition_last; partition_index++)
        {
            inter_result2 = partition_result[partition_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_partition_set(unit, partition[partition_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_set (unit %u, partition %u)"
                                , ret, expect_result, unit, partition[partition_index]);

                ret = rtk_acl_partition_get(unit, &result_partition);
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_get (unit %u, partition %u)"
                                 , ret, expect_result, unit, result_partition);
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_set/get value unequal (unit %u, partition %u)"
                                 , result_partition, partition[partition_index], unit, partition[partition_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_partition_set (unit %u, partition %u)"
                                    , ret, RT_ERR_OK, unit, partition[partition_index]);
            }
        }

        ret = rtk_acl_partition_get(unit, &result_partition);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_acl_partition_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_partition_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  partition;
        uint32  partition_max;
        uint32  result_partition;

        G_VALUE(HAL_MAX_NUM_OF_PIE_BLOCK(unit), partition_max);

        for (L_VALUE(0, partition); partition <= partition_max; partition++)
        {
            ASSIGN_EXPECT_RESULT(partition, 0, HAL_MAX_NUM_OF_PIE_BLOCK(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_partition_set(unit, partition);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_set (unit %u, partition %u)"
                                , ret, expect_result, unit, partition);

                ret = rtk_acl_partition_get(unit, &result_partition);
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_get (unit %u, partition %u)"
                                 , ret, expect_result, unit, result_partition);
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_set/get value unequal (unit %u, partition %u)"
                                 , result_partition, partition, unit, partition);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_partition_set (unit %u, partition %u)"
                                    , ret, RT_ERR_OK, unit, partition);
            }
            ret = rtk_acl_partition_get(unit, &result_partition);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_partition_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_partition_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_acl_blockPwrEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockPwrEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockPwrEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blockId[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_result[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_index;
        int32  blockId_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCK_ID_MAX(unit), TEST_BLOCK_ID_MIN, blockId, blockId_result, blockId_last);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (blockId_index = 0; blockId_index <= blockId_last; blockId_index++)
        {
            inter_result2 = blockId_result[blockId_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_blockPwrEnable_set(unit, blockId[blockId_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_set (unit %u, blockId %u, enable %u)"
                                    , ret, expect_result, unit, blockId[blockId_index], enable);

                    ret = rtk_acl_blockPwrEnable_get(unit, blockId[blockId_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u, enable %u)"
                                     , ret, expect_result, unit, blockId[blockId_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_set/get value unequal (unit %u, blockId %u, enable %u)"
                                     , result_enable, enable, unit, blockId[blockId_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockPwrEnable_set (unit %u, blockId %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, blockId[blockId_index], enable);
                }
            }
            ret = rtk_acl_blockPwrEnable_get(unit, blockId[blockId_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u)"
                                , ret, expect_result, unit, blockId[blockId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u)"
                                    , ret, RT_ERR_OK, unit, blockId[blockId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 blockId;
        uint32 blockId_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_BLOCK_ID_MAX(unit), blockId_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_BLOCK_ID_MIN, blockId); blockId <= blockId_max; blockId++)
        {
            ASSIGN_EXPECT_RESULT(blockId, TEST_BLOCK_ID_MIN, TEST_BLOCK_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_blockPwrEnable_set(unit, blockId, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_set (unit %u, blockId %u, enable %u)"
                                    , ret, expect_result, unit, blockId, enable);

                    ret = rtk_acl_blockPwrEnable_get(unit, blockId, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u, enable %u)"
                                     , ret, expect_result, unit, blockId, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_set/get value unequal (unit %u, blockId %u, enable %u)"
                                     , result_enable, enable, unit, blockId, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockPwrEnable_set (unit %u, blockId %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, blockId, enable);
                }
                ret = rtk_acl_blockPwrEnable_get(unit, blockId, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u)"
                                    , ret, expect_result, unit, blockId);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockPwrEnable_get (unit %u, blockId %u)"
                                        , ret, RT_ERR_OK, unit, blockId);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_blockLookupEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_blockLookupEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_blockLookupEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blockId[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_result[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_index;
        int32  blockId_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCK_ID_MAX(unit), TEST_BLOCK_ID_MIN, blockId, blockId_result, blockId_last);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (blockId_index = 0; blockId_index <= blockId_last; blockId_index++)
        {
            inter_result2 = blockId_result[blockId_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_pie_blockLookupEnable_set(unit, blockId[blockId_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_set (unit %u, blockId %u, enable %u)"
                                    , ret, expect_result, unit, blockId[blockId_index], enable);

                    ret = rtk_pie_blockLookupEnable_get(unit, blockId[blockId_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u, enable %u)"
                                     , ret, expect_result, unit, blockId[blockId_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_set/get value unequal (unit %u, blockId %u, enable %u)"
                                     , result_enable, enable, unit, blockId[blockId_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_blockLookupEnable_set (unit %u, blockId %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, blockId[blockId_index], enable);
                }
            }
            ret = rtk_pie_blockLookupEnable_get(unit, blockId[blockId_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u)"
                                , ret, expect_result, unit, blockId[blockId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u)"
                                    , ret, RT_ERR_OK, unit, blockId[blockId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 blockId;
        uint32 blockId_max;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;

        G_VALUE(TEST_BLOCK_ID_MAX(unit), blockId_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_BLOCK_ID_MIN, blockId); blockId <= blockId_max; blockId++)
        {
            ASSIGN_EXPECT_RESULT(blockId, TEST_BLOCK_ID_MIN, TEST_BLOCK_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_pie_blockLookupEnable_set(unit, blockId, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_set (unit %u, blockId %u, enable %u)"
                                    , ret, expect_result, unit, blockId, enable);

                    ret = rtk_pie_blockLookupEnable_get(unit, blockId, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u, enable %u)"
                                     , ret, expect_result, unit, blockId, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_set/get value unequal (unit %u, blockId %u, enable %u)"
                                     , result_enable, enable, unit, blockId, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_blockLookupEnable_set (unit %u, blockId %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, blockId, enable);
                }
                ret = rtk_pie_blockLookupEnable_get(unit, blockId, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u)"
                                    , ret, expect_result, unit, blockId);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_blockLookupEnable_get (unit %u, blockId %u)"
                                        , ret, RT_ERR_OK, unit, blockId);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_ruleEntryFieldSize_test(uint32 caseNo, uint32 unit)
{
    int32  ret;
    int32  expect_result = 1;
    uint32 entry_size;
    int8   inter_result2 = 1;

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  phaseVal[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_result[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_index;
        int32   phaseVal_last;

        rtk_acl_fieldType_t     type[UNITTEST_MAX_TEST_VALUE];
        int32                   type_result[UNITTEST_MAX_TEST_VALUE];
        int32                   type_index;
        int32                   type_last;
        uint32  result_field_size = 0;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phaseVal, phaseVal_result, phaseVal_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_TEMPLATE_TYPE, TEST_MIN_TEMPLATE_TYPE, type, type_result, type_last);

        for (phaseVal_index = 0; phaseVal_index <= phaseVal_last; phaseVal_index++)
        {
            inter_result2 = phaseVal_result[phaseVal_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            /* show entry size here */
            ret = rtk_acl_ruleEntrySize_get(unit, phaseVal[phaseVal_index], &entry_size);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntrySize_get (unit %u, phase %u, entry_size %u)"
                                 , ret, expect_result, unit, phaseVal[phaseVal_index], entry_size);
                osal_printf("unit[%d] phase[%d] PIE entry size = %d\n", unit, phaseVal[phaseVal_index], entry_size);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntrySize_get (unit %u, phase %u, entry_size %u)"
                                 , ret, RT_ERR_OK, unit, phaseVal[phaseVal_index], entry_size);
                osal_printf("unit[%d] phase[%d] PIE entry size get ERROR! \n", unit, phaseVal[phaseVal_index]);
            }
        }/* end of for (phaseVal_index = 0; phaseVal_index <= phaseVal_last; phaseVal_index++) */

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            inter_result2 = type_result[type_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            /* For test chip */
            if (HWP_8390_50_FAMILY(unit) &&
                ((USER_FIELD_DROPPED == type[type_index]) || (USER_FIELD_FLOW_LABEL == type[type_index]) ||
                (USER_FIELD_IP6_MOB_HDR_EXIST == type[type_index]) ||
                (USER_FIELD_IGMP_GROUPIP == type[type_index]) || (USER_FIELD_DATYPE == type[type_index]) ||
                (type[type_index] == USER_FIELD_PPPOE) ||
                (type[type_index] >= USER_FIELD_RRCPKEY && type[type_index] <= USER_FIELD_RRCPHLNULLMAC) ||
                (USER_FIELD_FLDSEL_RANGE == type[type_index])))
                expect_result = RT_ERR_FAILED;
            if (HWP_8380_30_FAMILY(unit) &&
                ((type[type_index] >= USER_FIELD_OMPLS_LABEL && type[type_index] <= USER_FIELD_IMPLS_LABEL_EXIST) ||
                (type[type_index] == USER_FIELD_IP4_CHKSUM_ERROR) || (type[type_index] == USER_FIELD_IP_FRAGMENT_OFFSET) ||
                (type[type_index] == USER_FIELD_IP6_ESP_HDR_EXIST) || (type[type_index] == USER_FIELD_VID_RANGE1) ||
                (type[type_index] >= USER_FIELD_FIELD_SELECTOR4 && type[type_index] <= USER_FIELD_FIELD_SELECTOR11) ||
                (type[type_index] >= USER_FIELD_DPM && type[type_index] <= USER_FIELD_OVID) ||
                (type[type_index] >= USER_FIELD_IGR_ACL_DROP_HIT && type[type_index] <= USER_FIELD_IGR_ACL_ROUTING_HIT)))
                expect_result = RT_ERR_FAILED;
            ret = rtk_acl_ruleEntryFieldSize_get(unit, type[type_index], &result_field_size);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, entry_field_size %u)"
                                 , ret, expect_result, unit, type[type_index], result_field_size);
                osal_printf("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, field_size %u)\n", unit, type[type_index], result_field_size);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, entry_field_size %u)"
                                 , ret, RT_ERR_OK, unit, type[type_index], result_field_size);
                osal_printf("unit[%d] type[%d] PIE entry field size get ERROR! \n", unit, type[type_index]);
            }
        }/* end of for (type_index = 0; type_index <= type_last; type_index++) */
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_fieldType_t     type;
        rtk_acl_fieldType_t     type_max;
        uint32                  result_field_size = 0;

        G_VALUE(TEST_MAX_TEMPLATE_TYPE, type_max);

        for (L_VALUE(TEST_MIN_TEMPLATE_TYPE, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_MIN_TEMPLATE_TYPE, TEST_MAX_TEMPLATE_TYPE, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            /* For test chip */
            if ((HWP_8390_50_FAMILY(unit)) &&
                ((USER_FIELD_DROPPED == type) || (USER_FIELD_FLOW_LABEL == type) ||
                (USER_FIELD_IP6_MOB_HDR_EXIST == type) ||
                (USER_FIELD_IGMP_GROUPIP == type) || (USER_FIELD_DATYPE == type) ||
                (type == USER_FIELD_PPPOE) ||
                (type >= USER_FIELD_RRCPKEY && type <= USER_FIELD_RRCPHLNULLMAC) ||
                (USER_FIELD_FLDSEL_RANGE == type)))
                expect_result = RT_ERR_FAILED;
            if ((HWP_8380_30_FAMILY(unit)) &&
                ((type >= USER_FIELD_OMPLS_LABEL && type <= USER_FIELD_IMPLS_LABEL_EXIST) ||
                (type == USER_FIELD_IP4_CHKSUM_ERROR) || (type == USER_FIELD_IP_FRAGMENT_OFFSET) ||
                (type == USER_FIELD_IP6_ESP_HDR_EXIST) || (type == USER_FIELD_VID_RANGE1) ||
                (type >= USER_FIELD_FIELD_SELECTOR4 && type <= USER_FIELD_FIELD_SELECTOR11) ||
                (type >= USER_FIELD_DPM && type <= USER_FIELD_OVID) ||
                (type >= USER_FIELD_IGR_ACL_DROP_HIT && type <= USER_FIELD_IGR_ACL_ROUTING_HIT)))
                expect_result = RT_ERR_FAILED;
            result_field_size = 0;
            ret = rtk_acl_ruleEntryFieldSize_get(unit, type, &result_field_size);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, entry_field_size %u)"
                                 , ret, expect_result, unit, type, result_field_size);
                osal_printf("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, field_size %u)\n", unit, type, result_field_size);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntryFieldSize_get (unit %u, type %u, entry_field_size %u)"
                                 , ret, RT_ERR_OK, unit, type, result_field_size);
                osal_printf("unit[%d] type[%d] PIE entry field size get ERROR! \n", unit, type);
            }
        }/* end of for (L_VALUE(TEST_MIN_TEMPLATE_TYPE, type); type <= type_max; type++) */
    }/* end of if (IS_TEST_SCAN_MODE()) */

    return RT_ERR_OK;
}

int32 dal_acl_ruleValidate_test(uint32 caseNo, uint32 unit)
{
#if defined (CONFIG_SDK_RTL8390)

    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;
#endif
    uint32 valid_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleValidate_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleValidate_get(unit, 0, 0, &valid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
#if defined (CONFIG_SDK_RTL8390)
    if (!IS_TEST_SCAN_MODE())
    {
        uint32  phaseVal[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_result[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_index;
        int32   phaseVal_last;

        uint32  entryId[UNITTEST_MAX_TEST_VALUE];
        int32  entryId_result[UNITTEST_MAX_TEST_VALUE];
        int32  entryId_index;
        int32  entryId_last;

        uint32  valid;
        uint32  valid_max;

        uint32  result_valid;
        uint32  blockId;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phaseVal, phaseVal_result, phaseVal_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_ENTRY_ID_MIN, entryId, entryId_result, entryId_last);

        G_VALUE(1, valid_max);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (phaseVal_index = 0; phaseVal_index <= phaseVal_last; phaseVal_index++)
        {
            inter_result2 = phaseVal_result[phaseVal_index];
            if (ACL_PHASE_IGR_ACL == phaseVal[phaseVal_index])
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phaseVal[phaseVal_index])
            {
                rtk_acl_partition_set(unit, 0);
            }
            for (entryId_index = 0; entryId_index <= entryId_last; entryId_index++)
            {
#if defined(CONFIG_SDK_FPGA_PLATFORM)
                /* The FPGA board ACL only exist 7 block, per-block 4 entries */
                if (HWP_8390_50_FAMILY(unit) && (entryId[entryId_index]%128) >= 4 || (entryId[entryId_index]/128) >= 7)
                    continue;
#endif
                inter_result3 = entryId_result[entryId_index];
                for (L_VALUE(0, valid); valid <= valid_max; valid++)
                {
                    ASSIGN_EXPECT_RESULT(valid, 0, 1, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_acl_ruleValidate_set(unit, phaseVal[phaseVal_index], entryId[entryId_index], valid);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleValidate_set (unit %u, phase %u, entryId %u, valid %u)"
                                        , ret, expect_result, unit, phaseVal[phaseVal_index], entryId[entryId_index], valid);

                        ret = rtk_acl_ruleValidate_get(unit, phaseVal[phaseVal_index], entryId[entryId_index], &result_valid);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleValidate_get (unit %u, phase %u, entryId %u, valid %u)"
                                         , ret, expect_result, unit, phaseVal[phaseVal_index], entryId[entryId_index], result_valid);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleValidate_set/get value unequal (unit %u, phase %u, entryId %u, valid %u)"
                                         , result_valid, valid, unit, phaseVal[phaseVal_index], entryId[entryId_index], valid);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleValidate_set (unit %u, phase %u, entryId %u, valid %u)"
                                            , ret, RT_ERR_OK, unit, phaseVal[phaseVal_index], entryId[entryId_index], valid);
                    }
                }
                ret = rtk_acl_ruleValidate_get(unit, phaseVal[phaseVal_index], entryId[entryId_index], &result_valid);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleValidate_get (unit %u, phase %u, entryId %u)"
                                    , ret, expect_result, unit, phaseVal[phaseVal_index], entryId[entryId_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleValidate_get (unit %u, phase %u, entryId %u)"
                                        , ret, RT_ERR_OK, unit, phaseVal[phaseVal_index], entryId[entryId_index]);
                }
            }
        }
    }
#endif
    return RT_ERR_OK;
}

int32 dal_acl_blockResultMode_test(uint32 caseNo, uint32 unit)
{

    int32 ret;
    rtk_acl_blockResultMode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockResultMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockResultMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blockId[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_result[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_index;
        int32  blockId_last;

        rtk_acl_blockResultMode_t  mode;
        uint32  mode_max;

        rtk_acl_blockResultMode_t  result_mode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCK_ID_MAX(unit), TEST_BLOCK_ID_MIN, blockId, blockId_result, blockId_last);

        G_VALUE(TEST_BLOCKRESULTMODE_MAX, mode_max);

        for (blockId_index = 0; blockId_index <= blockId_last; blockId_index++)
        {
            inter_result2 = blockId_result[blockId_index];
            for (L_VALUE(TEST_BLOCKRESULTMODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_BLOCKRESULTMODE_MIN, TEST_BLOCKRESULTMODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_blockResultMode_set(unit, blockId[blockId_index], mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_set (unit %u, blockId %u, mode %u)"
                                    , ret, expect_result, unit, blockId[blockId_index], mode);

                    ret = rtk_acl_blockResultMode_get(unit, blockId[blockId_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u, mode %u)"
                                     , ret, expect_result, unit, blockId[blockId_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_set/get value unequal (unit %u, blockId %u, mode %u)"
                                     , result_mode, mode, unit, blockId[blockId_index], mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockResultMode_set (unit %u, blockId %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, blockId[blockId_index], mode);
                }
            }
            ret = rtk_acl_blockResultMode_get(unit, blockId[blockId_index], &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u)"
                                , ret, expect_result, unit, blockId[blockId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u)"
                                    , ret, RT_ERR_OK, unit, blockId[blockId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 blockId;
        uint32 blockId_max;
        rtk_acl_blockResultMode_t  mode;
        uint32  mode_max;

        rtk_acl_blockResultMode_t  result_mode;

        G_VALUE(TEST_BLOCK_ID_MAX(unit), blockId_max);
        G_VALUE(TEST_BLOCKRESULTMODE_MAX, mode_max);

        for (L_VALUE(TEST_BLOCK_ID_MIN, blockId); blockId <= blockId_max; blockId++)
        {
            ASSIGN_EXPECT_RESULT(blockId, TEST_BLOCK_ID_MIN, TEST_BLOCK_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_BLOCKRESULTMODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_BLOCKRESULTMODE_MIN, TEST_BLOCKRESULTMODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_blockResultMode_set(unit, blockId, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_set (unit %u, blockId %u, mode %u)"
                                    , ret, expect_result, unit, blockId, mode);

                    ret = rtk_acl_blockResultMode_get(unit, blockId, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u, mode %u)"
                                     , ret, expect_result, unit, blockId, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_set/get value unequal (unit %u, blockId %u, mode %u)"
                                     , result_mode, mode, unit, blockId, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockResultMode_set (unit %u, blockId %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, blockId, mode);
                }
                ret = rtk_acl_blockResultMode_get(unit, blockId, &result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u)"
                                    , ret, expect_result, unit, blockId);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockResultMode_get (unit %u, blockId %u)"
                                        , ret, RT_ERR_OK, unit, blockId);
                }
            }
        }
        }
    return RT_ERR_OK;
}

int32 dal_acl_blockGroupEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockGroupEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_blockGroupEnable_get(unit, 0, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blockId[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_result[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_index;
        int32  blockId_last;

        rtk_acl_blockGroup_t  aggrType[UNITTEST_MAX_TEST_VALUE];
        int32  aggrType_result[UNITTEST_MAX_TEST_VALUE];
        int32  aggrType_index;
        int32  aggrType_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCK_ID_MAX(unit), TEST_BLOCK_ID_MIN, blockId, blockId_result, blockId_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCKGROUPTYPE_MAX, TEST_BLOCKGROUPTYPE_MIN, aggrType, aggrType_result, aggrType_last);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (blockId_index = 0; blockId_index <= blockId_last; blockId_index++)
        {
            inter_result2 = blockId_result[blockId_index];
            for (aggrType_index = 0; aggrType_index <= aggrType_last; aggrType_index++)
            {
                inter_result3 = aggrType_result[aggrType_index];

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if (HWP_8380_30_FAMILY(unit) && (aggrType[aggrType_index] != ACL_BLOCK_GROUP_1 ||  TEST_BLOCK_ID_MAX(unit) == blockId[blockId_index]))
                    {
                        expect_result = RT_ERR_FAILED;
                    }
                    if (HWP_8390_50_FAMILY(unit) &&
                        ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_1) ||
                        ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_2) && ((blockId[blockId_index] % 2) != 0)) ||
                        ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_4) && (((blockId[blockId_index] % 4) != 0) || (blockId[blockId_index] + 3 > TEST_BLOCK_ID_MAX(unit)))) ||
                        ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_8) && (((blockId[blockId_index] % 8) != 0) || (blockId[blockId_index] + 7 > TEST_BLOCK_ID_MAX(unit)))) ||
                        ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_ALL) && (blockId[blockId_index] != 0))))
                    {
                        expect_result = RT_ERR_FAILED;
                    }
                    ret = rtk_acl_blockGroupEnable_set(unit, blockId[blockId_index], aggrType[aggrType_index], enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_set (unit %u, blockId %u, aggrType %u, enable %u)"
                                        , ret, expect_result, unit, blockId[blockId_index], aggrType[aggrType_index], enable);

                        ret = rtk_acl_blockGroupEnable_get(unit, blockId[blockId_index], aggrType[aggrType_index], &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u, enable %u)"
                                         , ret, expect_result, unit, blockId[blockId_index], aggrType[aggrType_index], result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_set/get value unequal (unit %u, blockId %u, aggrType %u, enable %u)"
                                         , result_enable, enable, unit, blockId[blockId_index], aggrType[aggrType_index], enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockAggregatorEnable_set (unit %u, blockId %u, aggrType %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, blockId[blockId_index], aggrType[aggrType_index], enable);
                    }
                }
                ret = rtk_acl_blockGroupEnable_get(unit, blockId[blockId_index], aggrType[aggrType_index], &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if (HWP_8390_50_FAMILY(unit) &&
                    ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_1) ||
                    ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_2) && ((blockId[blockId_index] % 2) != 0)) ||
                    ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_4) && (((blockId[blockId_index] % 4) != 0) || (blockId[blockId_index] + 3 > TEST_BLOCK_ID_MAX(unit)))) ||
                    ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_8) && (((blockId[blockId_index] % 8) != 0) || (blockId[blockId_index] + 7 > TEST_BLOCK_ID_MAX(unit)))) ||
                    ((aggrType[aggrType_index] == ACL_BLOCK_GROUP_ALL) && (blockId[blockId_index] != 0))))
                {
                    expect_result = RT_ERR_FAILED;
                }

                if (HWP_8380_30_FAMILY(unit) && (aggrType[aggrType_index] != ACL_BLOCK_GROUP_1 ||  TEST_BLOCK_ID_MAX(unit) == blockId[blockId_index]))
                {
                    expect_result = RT_ERR_FAILED;
                }
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u)"
                                    , ret, expect_result, unit, blockId[blockId_index], aggrType[aggrType_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u)"
                                        , ret, RT_ERR_OK, unit, blockId[blockId_index], aggrType[aggrType_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 blockId;
        uint32 blockId_max;
        rtk_acl_blockGroup_t  aggrType;
        uint32  aggrType_max;
        rtk_enable_t  enable;
        uint32  enable_max;

        rtk_enable_t  result_enable;

        G_VALUE(TEST_BLOCK_ID_MAX(unit), blockId_max);
        G_VALUE(TEST_BLOCKGROUPTYPE_MAX, aggrType_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_BLOCK_ID_MIN, blockId); blockId <= blockId_max; blockId++)
        {
            ASSIGN_EXPECT_RESULT(blockId, TEST_BLOCK_ID_MIN, TEST_BLOCK_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_BLOCKGROUPTYPE_MIN, aggrType); aggrType <= aggrType_max; aggrType++)
            {
                ASSIGN_EXPECT_RESULT(aggrType, TEST_BLOCKGROUPTYPE_MIN, TEST_BLOCKGROUPTYPE_MAX, inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if (HWP_8380_30_FAMILY(unit) && ((aggrType == ACL_BLOCK_GROUP_1) ||
                        ((aggrType == ACL_BLOCK_GROUP_2) && ((blockId % 2) != 0)) ||
                        ((aggrType == ACL_BLOCK_GROUP_4) && (((blockId % 4) != 0) || (blockId + 3 > TEST_BLOCK_ID_MAX(unit)))) ||
                        ((aggrType == ACL_BLOCK_GROUP_8) && (((blockId % 8) != 0) || (blockId + 7 > TEST_BLOCK_ID_MAX(unit)))) ||
                        ((aggrType == ACL_BLOCK_GROUP_ALL) && (blockId != 0))))
                    {
                        expect_result = RT_ERR_FAILED;
                    }

                    if (HWP_8380_30_FAMILY(unit) && (aggrType != ACL_BLOCK_GROUP_1 ||  TEST_BLOCK_ID_MAX(unit) == blockId))
                    {
                        expect_result = RT_ERR_FAILED;
                    }
                    ret = rtk_acl_blockGroupEnable_set(unit, blockId, aggrType, enable);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_set (unit %u, blockId %u, aggrType %u, enable %u)"
                                        , ret, expect_result, unit, blockId, aggrType, enable);

                        ret = rtk_acl_blockGroupEnable_get(unit, blockId, aggrType, &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u, enable %u)"
                                         , ret, expect_result, unit, blockId, aggrType, result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_set/get value unequal (unit %u, blockId %u, aggrType %u, enable %u)"
                                         , result_enable, enable, unit, blockId, aggrType, enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockGroupEnable_set (unit %u, blockId %u, aggrType %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, blockId, aggrType, enable);
                    }
                    ret = rtk_acl_blockGroupEnable_get(unit, blockId, aggrType, &result_enable);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                    if ((HWP_8380_30_FAMILY(unit)) && ((aggrType != ACL_BLOCK_GROUP_1) ||  (TEST_BLOCK_ID_MAX(unit) == blockId)))
                    {
                        expect_result = RT_ERR_FAILED;
                    }
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u)"
                                        , ret, expect_result, unit, blockId, aggrType);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_blockGroupEnable_get (unit %u, blockId %u, aggrType %u)"
                                            , ret, RT_ERR_OK, unit, blockId, aggrType);
                    }
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_statPktCnt_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pktCnt_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_statPktCnt_clear(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_statPktCnt_get(unit, 0, &pktCnt_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  logId[UNITTEST_MAX_TEST_VALUE];
        int32  logId_result[UNITTEST_MAX_TEST_VALUE];
        int32  logId_index;
        int32  logId_last;
        uint32  result_pktCnt;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PKT_LOG_ID_MAX(unit), TEST_PKT_LOG_ID_MIN, logId, logId_result, logId_last);

        for (logId_index = 0; logId_index <= logId_last; logId_index++)
        {
            inter_result2 = logId_result[logId_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_statPktCnt_clear(unit, logId[logId_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_clear (unit %u, logId %u)"
                                , ret, expect_result, unit, logId[logId_index]);

                ret = rtk_acl_statPktCnt_get(unit, logId[logId_index], &result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u, pktCnt %u)"
                                 , ret, expect_result, unit, logId[logId_index], result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get value unequal (unit %u, logId %u, pktCnt %u)"
                                 , result_pktCnt, 0, unit, logId[logId_index], result_pktCnt);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statPktCnt_clear (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId[logId_index]);
            }

            ret = rtk_acl_statPktCnt_get(unit, logId[logId_index], &result_pktCnt);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u)"
                                , ret, expect_result, unit, logId[logId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId[logId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 logId;
        uint32 logId_max;
        uint32  result_pktCnt;

        G_VALUE(TEST_PKT_LOG_ID_MAX(unit), logId_max);

        for (L_VALUE(TEST_PKT_LOG_ID_MIN, logId); logId <= logId_max; logId++)
        {
            ASSIGN_EXPECT_RESULT(logId, TEST_PKT_LOG_ID_MIN, TEST_PKT_LOG_ID_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_statPktCnt_clear(unit, logId);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_clear (unit %u, logId %u)"
                                , ret, expect_result, unit, logId);

                ret = rtk_acl_statPktCnt_get(unit, logId, &result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u, pktCnt %u)"
                                 , ret, expect_result, unit, logId, result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get value unequal (unit %u, logId %u, pktCnt %u)"
                                 , result_pktCnt, 0, unit, logId, result_pktCnt);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statPktCnt_clear (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId);
            }
            ret = rtk_acl_statPktCnt_get(unit, logId, &result_pktCnt);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u)"
                                , ret, expect_result, unit, logId);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_statByteCnt_test(uint32 caseNo, uint32 unit)
{
    uint64  byteCnt_temp;
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_statByteCnt_clear(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_statByteCnt_get(unit, 0, &byteCnt_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  logId[UNITTEST_MAX_TEST_VALUE];
        int32  logId_result[UNITTEST_MAX_TEST_VALUE];
        int32  logId_index;
        int32  logId_last;

        uint64  result_byteCnt;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_BYTE_LOG_ID_MAX(unit), TEST_BYTE_LOG_ID_MIN, logId, logId_result, logId_last);

        for (logId_index = 0; logId_index <= logId_last; logId_index++)
        {
            inter_result2 = logId_result[logId_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_statByteCnt_clear(unit, logId[logId_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_clear (unit %u, logId %u)"
                                , ret, expect_result, unit, logId[logId_index]);

                ret = rtk_acl_statByteCnt_get(unit, logId[logId_index], &result_byteCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u, byteCnt %llu)"
                                 , ret, expect_result, unit, logId[logId_index], result_byteCnt);
                RT_TEST_IS_EQUAL_INT64("rtk_acl_statByteCnt_get value unequal (unit %u, logId %u, byteCnt %llu)"
                                 , result_byteCnt, (uint64)0, unit, logId[logId_index], result_byteCnt);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statByteCnt_clear (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId[logId_index]);
            }

            ret = rtk_acl_statByteCnt_get(unit, logId[logId_index], &result_byteCnt);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u)"
                                , ret, expect_result, unit, logId[logId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId[logId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 logId;
        uint32 logId_max;
        uint64  result_byteCnt;

        G_VALUE(TEST_BYTE_LOG_ID_MAX(unit), logId_max);

        for (L_VALUE(TEST_BYTE_LOG_ID_MIN, logId); logId <= logId_max; logId++)
        {
            ASSIGN_EXPECT_RESULT(logId, TEST_BYTE_LOG_ID_MIN, TEST_BYTE_LOG_ID_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_statByteCnt_clear(unit, logId);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_clear (unit %u, logId %u)"
                                , ret, expect_result, unit, logId);

                ret = rtk_acl_statByteCnt_get(unit, logId, &result_byteCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u, byteCnt %llu)"
                                 , ret, expect_result, unit, logId, result_byteCnt);
                RT_TEST_IS_EQUAL_INT64("rtk_acl_statByteCnt_get value unequal (unit %u, logId %u, byteCnt %llu)"
                                 , result_byteCnt, (uint64)0, unit, logId, result_byteCnt);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statByteCnt_clear (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId);
            }
            ret = rtk_acl_statByteCnt_get(unit, logId, &result_byteCnt);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u)"
                                , ret, expect_result, unit, logId);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_statByteCnt_get (unit %u, logId %u)"
                                    , ret, RT_ERR_OK, unit, logId);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_statClear_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32  expect_result = 1;

    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_stat_clearAll(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32 logId;
        uint32 result_pktCnt;

        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        ret = rtk_acl_stat_clearAll(unit);
        if (RT_ERR_OK == expect_result)
        {
            RT_TEST_IS_EQUAL_INT("rtk_acl_stat_clearAll (unit %u)"
                            , ret, expect_result, unit);

            for (logId = 0; logId <= TEST_PKT_LOG_ID_MAX(unit); logId++)
            {
                ret = rtk_acl_statPktCnt_get(unit, logId, &result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get (unit %u, logId %u, pktCnt %u)"
                                 , ret, expect_result, unit, logId, result_pktCnt);
                RT_TEST_IS_EQUAL_INT("rtk_acl_statPktCnt_get value unequal (unit %u, logId %u, pktCnt %u)"
                                 , result_pktCnt, 0, unit, logId, result_pktCnt);
            }
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_stat_clearAll (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckL4Port_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_rangeCheck_l4Port_t l4port_data;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckL4Port_set(unit, 0, &l4port_data))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckL4Port_get(unit, 0, &l4port_data)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    osal_memset(&l4port_data, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));
    l4port_data.upper_bound = 100;
    l4port_data.lower_bound = 10;
#if defined (CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
        l4port_data.reverse = 1;
#endif
    l4port_data.l4port_dir = RNGCHK_L4PORT_DIRECTION_DST;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_acl_rangeCheck_l4Port_t result_l4port;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_L4PORT_INDEX(unit), TEST_MIN_RC_L4PORT_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckL4Port_set(unit, entry[entry_index], &l4port_data);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_l4port, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));
                ret = rtk_acl_rangeCheckL4Port_get(unit, entry[entry_index], &result_l4port);
                #if 0 /*debug message */
                osal_printf("result_l4port.upper_bound = %d\n", result_l4port.upper_bound);
                osal_printf("result_l4port.lower_bound = %d\n", result_l4port.lower_bound);
                if(HWP_8390_50_FAMILY(unit))
                {
                    osal_printf("result_l4port.reverse = %d\n", result_l4port.reverse);
                }
                osal_printf("result_l4port.l4port_dir = %d\n", result_l4port.l4port_dir);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&l4port_data, &result_l4port, sizeof(rtk_acl_rangeCheck_l4Port_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);

                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (upper_bound %u, result_upper_bound %u)\n", __FUNCTION__, __LINE__, l4port_data.upper_bound, result_l4port.upper_bound);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (lower_bound %u, result_lower_bound %u)\n", __FUNCTION__, __LINE__, l4port_data.lower_bound, result_l4port.lower_bound);
                    if(HWP_8390_50_FAMILY(unit))
                        osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (reverse %u, result_reverse %u)\n", __FUNCTION__, __LINE__, l4port_data.reverse, result_l4port.reverse);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (l4port_dir %u, result_l4port_dir %u)\n", __FUNCTION__, __LINE__, l4port_data.l4port_dir, result_l4port.l4port_dir);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckL4Port_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckL4Port_get(unit, entry[entry_index], &result_l4port);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        rtk_acl_rangeCheck_l4Port_t result_l4port;

        G_VALUE(TEST_MAX_RC_L4PORT_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_L4PORT_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_L4PORT_INDEX, TEST_MAX_RC_L4PORT_INDEX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckL4Port_set(unit, entry, &l4port_data);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_l4port, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));
                ret = rtk_acl_rangeCheckL4Port_get(unit, entry, &result_l4port);
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&l4port_data, &result_l4port, sizeof(rtk_acl_rangeCheck_l4Port_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (upper_bound %u, result_upper_bound %u)\n", __FUNCTION__, __LINE__, l4port_data.upper_bound, result_l4port.upper_bound);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (lower_bound %u, result_lower_bound %u)\n", __FUNCTION__, __LINE__, l4port_data.lower_bound, result_l4port.lower_bound);
#if defined (CONFIG_SDK_RTL8390)
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (reverse %u, result_reverse %u)\n", __FUNCTION__, __LINE__, l4port_data.reverse, result_l4port.reverse);
                    }
#endif
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckL4Port_set/get value unequal (l4port_dir %u, result_l4port_dir %u)\n", __FUNCTION__, __LINE__, l4port_data.l4port_dir, result_l4port.l4port_dir);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckL4Port_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckL4Port_get(unit, entry, &result_l4port);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckL4Port_get (unit %u, entry %u)"
                                 , ret, RT_ERR_OK, unit, entry);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckVid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_rangeCheck_vid_t vid_data;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckVid_set(unit, 0, &vid_data))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckVid_get(unit, 0, &vid_data)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    osal_memset(&vid_data, 0, sizeof(rtk_acl_rangeCheck_vid_t));
    vid_data.vid_upper_bound = 200;
    vid_data.vid_lower_bound = 12;
    if(HWP_8390_50_FAMILY(unit))
    {
        vid_data.reverse = 1;
    }
    vid_data.vid_type = RNGCHK_VID_TYPE_OUTER;

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;
        rtk_acl_rangeCheck_vid_t result_vid;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_VID_INDEX(unit), TEST_MIN_RC_VID_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckVid_set(unit, entry[entry_index], &vid_data);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_vid, 0, sizeof(rtk_acl_rangeCheck_vid_t));
                ret = rtk_acl_rangeCheckVid_get(unit, entry[entry_index], &result_vid);
                #if 0 /* debug message */
                osal_printf("result_vid.vid_upper_bound = %d\n", result_vid.vid_upper_bound);
                osal_printf("result_vid.vid_lower_bound = %d\n", result_vid.vid_lower_bound);
                if(HWP_8390_50_FAMILY(unit))
                {
                    osal_printf("result_vid.reverse = %d\n", result_vid.reverse);
                }
                osal_printf("result_vid.vid_type = %d\n", result_vid.vid_type);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&vid_data, &result_vid, sizeof(rtk_acl_rangeCheck_vid_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (upper_bound %u, result_upper_bound %u)\n", __FUNCTION__, __LINE__, vid_data.vid_upper_bound, result_vid.vid_upper_bound);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (vid_lower_bound %u, vid_lower_bound %u)\n", __FUNCTION__, __LINE__, vid_data.vid_lower_bound, result_vid.vid_lower_bound);
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (reverse %u, reverse %u)\n", __FUNCTION__, __LINE__, vid_data.reverse, result_vid.reverse);
                    }
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (vid_type %u, vid_type %u)\n", __FUNCTION__, __LINE__, vid_data.vid_type, result_vid.vid_type);

                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckVid_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckVid_get(unit, entry[entry_index], &result_vid);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        rtk_acl_rangeCheck_vid_t result_vid;

        G_VALUE(TEST_MAX_RC_VID_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_VID_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_VID_INDEX, TEST_MAX_RC_VID_INDEX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckVid_set(unit, entry, &vid_data);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_vid, 0, sizeof(rtk_acl_rangeCheck_vid_t));
                ret = rtk_acl_rangeCheckVid_get(unit, entry, &result_vid);
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&vid_data, &result_vid, sizeof(rtk_acl_rangeCheck_vid_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckVid_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckVid_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckVid_get(unit, entry, &result_vid);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckVid_get (unit %u, entry %u)"
                                 , ret, RT_ERR_OK, unit, entry);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckIp_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_pie_rangeCheck_ip_t temp_ipData;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_rangeCheckIp_set(unit, 0, &temp_ipData))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_rangeCheckIp_get(unit, 0, &temp_ipData)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        uint32  ip_rand_1, ip_rand_2;
        rtk_pie_rangeCheck_ip_t ipData;
        rtk_pie_rangeCheck_ip_t result_ipData;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_IP_INDEX(unit), TEST_MIN_RC_IP_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&ipData, 0, sizeof(rtk_pie_rangeCheck_ip_t));
            ip_rand_1 = ut_rand();
            ip_rand_2 = ut_rand();
            if (ip_rand_2 >= ip_rand_1)
            {
                ipData.ip_lower_bound = ip_rand_1;
                ipData.ip_upper_bound = ip_rand_2;
            }
            else
            {
                ipData.ip_lower_bound = ip_rand_2;
                ipData.ip_upper_bound = ip_rand_1;
            }
            if(HWP_8390_50_FAMILY(unit))
            {
                ipData.reverse = ut_rand()%2;
            }
            ipData.ip_type = ut_rand()%RNGCHK_IP_TYPE_END;

            ret = rtk_pie_rangeCheckIp_set(unit, entry[entry_index], &ipData);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_rangeCheckIp_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_ipData, 0, sizeof(rtk_pie_rangeCheck_ip_t));
                ret = rtk_pie_rangeCheckIp_get(unit, entry[entry_index], &result_ipData);
                #if 0 /* debug message */
                osal_printf("result_ipData.ip_lower_bound = %x\n", result_ipData.ip_lower_bound);
                osal_printf("result_ipData.ip_upper_bound = %x\n", result_ipData.ip_upper_bound);
                if(HWP_8390_50_FAMILY(unit))
                    osal_printf("result_ip.reverse = %d\n", result_ip.reverse);
                osal_printf("result_ip.ip_type = %d\n", result_ip.ip_type);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_pie_rangeCheckIp_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&ipData, &result_ipData, sizeof(rtk_pie_rangeCheck_ip_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_lower_bound %u, result_ip_lower_bound %u)\n", __FUNCTION__, __LINE__, ipData.ip_lower_bound, result_ipData.ip_lower_bound);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_upper_bound %u, result_ip_upper_bound %u)\n", __FUNCTION__, __LINE__, ipData.ip_upper_bound, result_ipData.ip_upper_bound);
                    if(HWP_8390_50_FAMILY(unit))
                        osal_printf("\t%s(%u): rtk_acl_rangeCheckIp_set/get value unequal (reverse %u, result_reverse %u)\n", __FUNCTION__, __LINE__, ipData.reverse, result_ipData.reverse);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_type %u, result_ip_type %u)\n", __FUNCTION__, __LINE__, ipData.ip_type, result_ipData.ip_type);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_rangeCheckIp_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        uint32  ip_rand_1, ip_rand_2;
        rtk_pie_rangeCheck_ip_t ipData;
        rtk_pie_rangeCheck_ip_t result_ipData;

        G_VALUE(TEST_MAX_RC_IP_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_IP_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_IP_INDEX, TEST_MAX_RC_IP_INDEX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&ipData, 0, sizeof(rtk_pie_rangeCheck_ip_t));
            ip_rand_1 = ut_rand();
            ip_rand_2 = ut_rand();
            if (ip_rand_2 >= ip_rand_1)
            {
                ipData.ip_lower_bound = ip_rand_1;
                ipData.ip_upper_bound = ip_rand_2;
            }
            else
            {
                ipData.ip_lower_bound = ip_rand_2;
                ipData.ip_upper_bound = ip_rand_1;
            }
            if(HWP_8390_50_FAMILY(unit))
                ipData.reverse = ut_rand()%2;
            ipData.ip_type = ut_rand()%RNGCHK_IP_TYPE_END;

            ret = rtk_pie_rangeCheckIp_set(unit, entry, &ipData);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_rangeCheckIp_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_ipData, 0, sizeof(rtk_pie_rangeCheck_ip_t));
                ret = rtk_pie_rangeCheckIp_get(unit, entry, &result_ipData);
                RT_TEST_IS_EQUAL_INT("rtk_pie_rangeCheckIp_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&ipData, &result_ipData, sizeof(rtk_pie_rangeCheck_ip_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_lower_bound %u, result_ip_lower_bound %u)\n", __FUNCTION__, __LINE__, ipData.ip_lower_bound, result_ipData.ip_lower_bound);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_upper_bound %u, result_ip_upper_bound %u)\n", __FUNCTION__, __LINE__, ipData.ip_upper_bound, result_ipData.ip_upper_bound);
                    if(HWP_8390_50_FAMILY(unit))
                        osal_printf("\t%s(%u): rtk_acl_rangeCheckIp_set/get value unequal (reverse %u, result_reverse %u)\n", __FUNCTION__, __LINE__, ipData.reverse, result_ipData.reverse);
                    osal_printf("\t%s(%u): rtk_pie_rangeCheckIp_set/get value unequal (ip_type %u, result_ip_type %u)\n", __FUNCTION__, __LINE__, ipData.ip_type, result_ipData.ip_type);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_rangeCheckIp_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckSrcPort_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_rangeCheck_portMask_t port_data;
    rtk_portmask_t portmask;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckSrcPort_set(unit, 0, &port_data))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckSrcPort_get(unit, 0, &port_data)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    osal_memset(&port_data, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
    port_data.port_mask.bits[0] = 0x12aabbcc;
    if(HWP_8390_50_FAMILY(unit))
    {
        port_data.port_mask.bits[1] = 0x00034567;
    }
    HWP_GET_ALL_PORTMASK(unit, portmask);
    port_data.port_mask.bits[0] &= portmask.bits[0];
    port_data.port_mask.bits[1] &= portmask.bits[1];

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_acl_rangeCheck_portMask_t result_port;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_SRCPORT_INDEX(unit), TEST_MIN_RC_SRCPORT_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckSrcPort_set(unit, entry[entry_index], &port_data);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckSrcPort_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_port, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
                ret = rtk_acl_rangeCheckSrcPort_get(unit, entry[entry_index], &result_port);
                #if 0 /* debug message */
                osal_printf("result_port.port_mask = %x\n", result_port.port_mask);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckSrcPort_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&port_data, &result_port, sizeof(rtk_acl_rangeCheck_portMask_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckSrcPort_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckSrcPort_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        rtk_acl_rangeCheck_portMask_t result_port;

        G_VALUE(TEST_MAX_RC_SRCPORT_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_SRCPORT_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_SRCPORT_INDEX, TEST_MAX_RC_SRCPORT_INDEX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckSrcPort_set(unit, entry, &port_data);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckSrcPort_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_port, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
                ret = rtk_acl_rangeCheckSrcPort_get(unit, entry, &result_port);
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckSrcPort_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&port_data, &result_port, sizeof(rtk_acl_rangeCheck_portMask_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckSrcPort_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckSrcPort_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckDstPort_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_rangeCheck_portMask_t port_data;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckDstPort_set(unit, 0, &port_data))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckDstPort_get(unit, 0, &port_data)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    osal_memset(&port_data, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
    port_data.port_mask.bits[0] = 0x12aabbcc;
    if(HWP_8390_50_FAMILY(unit))
    {
        port_data.port_mask.bits[1] = 0x00034567;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_acl_rangeCheck_portMask_t result_port;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_DSTPORT_INDEX(unit), TEST_MIN_RC_DSTPORT_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckDstPort_set(unit, entry[entry_index], &port_data);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckDstPort_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_port, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
                ret = rtk_acl_rangeCheckDstPort_get(unit, entry[entry_index], &result_port);
                #if 0 /* debug message */
                osal_printf("result_port.port_mask = %x\n", result_port.port_mask);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckDstPort_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&port_data, &result_port, sizeof(rtk_acl_rangeCheck_portMask_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckDstPort_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckDstPort_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        rtk_acl_rangeCheck_portMask_t result_port;

        G_VALUE(TEST_MAX_RC_DSTPORT_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_DSTPORT_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_DSTPORT_INDEX, TEST_MAX_RC_DSTPORT_INDEX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_acl_rangeCheckDstPort_set(unit, entry, &port_data);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckDstPort_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_port, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
                ret = rtk_acl_rangeCheckDstPort_get(unit, entry, &result_port);
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckDstPort_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&port_data, &result_port, sizeof(rtk_acl_rangeCheck_portMask_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckDstPort_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckDstPort_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_rangeCheckPacketLen_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_rangeCheck_packetLen_t temp_packetLen;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckPacketLen_set(unit, 0, &temp_packetLen))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_rangeCheckPacketLen_get(unit, 0, &temp_packetLen)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        uint32  pktLen_rand_1, pktLen_rand_2;
        rtk_acl_rangeCheck_packetLen_t packetLen;
        rtk_acl_rangeCheck_packetLen_t result_packetLen;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_RC_PKTLEN_INDEX(unit), TEST_MIN_RC_PKTLEN_INDEX, entry, entry_result, entry_last);

        for (entry_index = 0; entry_index <= entry_last; entry_index++)
        {
            inter_result2 = entry_result[entry_index];
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&packetLen, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));
            pktLen_rand_1 = ut_rand()&0x3FFF;
            pktLen_rand_2 = ut_rand()&0x3FFF;
            if (pktLen_rand_2 >= pktLen_rand_1)
            {
                packetLen.lower_bound = pktLen_rand_1;
                packetLen.upper_bound = pktLen_rand_2;
            }
            else
            {
                packetLen.lower_bound = pktLen_rand_2;
                packetLen.upper_bound = pktLen_rand_1;
            }
            if(HWP_8390_50_FAMILY(unit))
                packetLen.reverse = ut_rand()%2;
            #if 0 /* debug message */
            osal_printf("packetLen.lower_bound = %x\n", packetLen.lower_bound);
            osal_printf("packetLen.upper_bound = %x\n", packetLen.upper_bound);
            if(HWP_8390_50_FAMILY(unit))
                osal_printf("packetLen.reverse = %d\n", packetLen.reverse);
            #endif
            ret = rtk_acl_rangeCheckPacketLen_set(unit, entry[entry_index], &packetLen);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckPacketLen_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry[entry_index]);
                osal_memset(&result_packetLen, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));
                ret = rtk_acl_rangeCheckPacketLen_get(unit, entry[entry_index], &result_packetLen);
                #if 0 /* debug message */
                osal_printf("result_packetLen.lower_bound = %x\n", result_packetLen.lower_bound);
                osal_printf("result_packetLen.upper_bound = %x\n", result_packetLen.upper_bound);
                if(HWP_8390_50_FAMILY(unit))
                    osal_printf("result_packetLen.reverse = %d\n", result_packetLen.reverse);
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckPacketLen_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry[entry_index]);
                if (osal_memcmp(&packetLen, &result_packetLen, sizeof(rtk_acl_rangeCheck_packetLen_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry[entry_index]);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (lower_bound %u, result_lower_bound %u)\n", __FUNCTION__, __LINE__, packetLen.lower_bound, result_packetLen.lower_bound);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (upper_bound %u, result_upper_bound %u)\n", __FUNCTION__, __LINE__, packetLen.upper_bound, result_packetLen.upper_bound);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckPacketLen_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry[entry_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t entry;
        rtk_acl_id_t entry_max;

        uint32  pktLen_rand_1, pktLen_rand_2;
        rtk_acl_rangeCheck_packetLen_t packetLen;
        rtk_acl_rangeCheck_packetLen_t result_packetLen;

        G_VALUE(TEST_MAX_RC_PKTLEN_INDEX(unit), entry_max);

        for (L_VALUE(TEST_MIN_RC_PKTLEN_INDEX, entry); entry <= entry_max; entry++)
        {
            ASSIGN_EXPECT_RESULT(entry, TEST_MIN_RC_PKTLEN_INDEX, TEST_MAX_RC_PKTLEN_INDEX(unit), inter_result2);
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&packetLen, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));
            pktLen_rand_1 = ut_rand()&0x3FFF;
            pktLen_rand_2 = ut_rand()&0x3FFF;
            if (pktLen_rand_2 >= pktLen_rand_1)
            {
                packetLen.lower_bound = pktLen_rand_1;
                packetLen.upper_bound = pktLen_rand_2;
            }
            else
            {
                packetLen.lower_bound = pktLen_rand_2;
                packetLen.upper_bound = pktLen_rand_1;
            }
            if(HWP_8390_50_FAMILY(unit))
                packetLen.reverse = ut_rand()%2;
            ret = rtk_acl_rangeCheckPacketLen_set(unit, entry, &packetLen);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckPacketLen_set (unit %u, entry %u)"
                                , ret, expect_result, unit, entry);
                osal_memset(&result_packetLen, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));
                ret = rtk_acl_rangeCheckPacketLen_get(unit, entry, &result_packetLen);
                RT_TEST_IS_EQUAL_INT("rtk_acl_rangeCheckPacketLen_get (unit %u, entry %u)"
                                 , ret, expect_result, unit, entry);
                if (osal_memcmp(&packetLen, &result_packetLen, sizeof(rtk_acl_rangeCheck_packetLen_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (unit %u, entry %u)\n", __FUNCTION__, __LINE__, unit, entry);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (lower_bound %u, result_lower_bound %u)\n", __FUNCTION__, __LINE__, packetLen.lower_bound, result_packetLen.lower_bound);
                    osal_printf("\t%s(%u): rtk_acl_rangeCheckPacketLen_set/get value unequal (upper_bound %u, result_upper_bound %u)\n", __FUNCTION__, __LINE__, packetLen.upper_bound, result_packetLen.upper_bound);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_rangeCheckPacketLen_set (unit %u, entry %u)"
                                    , ret, RT_ERR_OK, unit, entry);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_fieldSelector_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    rtk_pie_fieldSelector_data_t temp_fsData;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_fieldSelector_set(unit, 0, &temp_fsData))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_fieldSelector_get(unit, 0, &temp_fsData)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 fsIdx[UNITTEST_MAX_TEST_VALUE];
        int32  fsIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  fsIdx_index;
        int32  fsIdx_last;

        rtk_pie_fieldSelector_startPosition_t start[UNITTEST_MAX_TEST_VALUE];
        int32  start_result[UNITTEST_MAX_TEST_VALUE];
        int32  start_index;
        int32  start_last;

        uint32 offset[UNITTEST_MAX_TEST_VALUE];
        int32  offset_result[UNITTEST_MAX_TEST_VALUE];
        int32  offset_index;
        int32  offset_last;

        rtk_pie_fieldSelector_data_t fsData;
        rtk_pie_fieldSelector_data_t result_fsData;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_FS_INDEX(unit), TEST_MIN_FS_INDEX, fsIdx, fsIdx_result, fsIdx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_FS_START_POS, TEST_MIN_FS_START_POS, start, start_result, start_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_MIN_FS_OFFSET, offset, offset_result, offset_last);

        for (fsIdx_index = 0; fsIdx_index <= fsIdx_last; fsIdx_index++)
        {
            inter_result2 = fsIdx_result[fsIdx_index];

            for (start_index = 0; start_index <= start_last; start_index++)
            {
                inter_result3 = start_result[start_index];

                for (offset_index = 0; offset_index <= offset_last; offset_index++)
                {
                    inter_result4 = offset_result[offset_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if ((HWP_8390_50_FAMILY(unit)) && (FS_START_POS_L2 == start[start_index]))
                        expect_result = RT_ERR_FAILED;
                    if ((HWP_8380_30_FAMILY(unit)) && (FS_START_POS_LLC == start[start_index] || FS_START_POS_ARP == start[start_index] ||
                        FS_START_POS_IPV4 == start[start_index] || FS_START_POS_IPV6 == start[start_index] ||
                        FS_START_POS_IP == start[start_index]))
                        expect_result = RT_ERR_FAILED;
                    osal_memset(&fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                    fsData.start = start[start_index];
                    fsData.offset = offset[offset_index];

                    ret = rtk_pie_fieldSelector_set(unit, fsIdx[fsIdx_index], &fsData);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_set (unit %u, fsIdx %u)"
                                        , ret, expect_result, unit, fsIdx[fsIdx_index]);
                        osal_memset(&result_fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                        ret = rtk_pie_fieldSelector_get(unit, fsIdx[fsIdx_index], &result_fsData);
                        #if 0 /* debug message */
                        osal_printf("result_fsData.start = %x\n", result_fsData.start);
                        osal_printf("result_fsData.offset = %x\n", result_fsData.offset);
                        #endif
                        RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                         , ret, expect_result, unit, fsIdx[fsIdx_index]);
                        if (osal_memcmp(&fsData, &result_fsData, sizeof(rtk_pie_fieldSelector_data_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_pie_fieldSelector_set/get value unequal (unit %u, fsIdx %u)\n", __FUNCTION__, __LINE__, unit, fsIdx[fsIdx_index]);
                            osal_printf("fsData.start = %x, result_fsData.start = %x\n", fsData.start, result_fsData.start);
                            osal_printf("fsData.offset = %x, result_fsData.offset = %x\n", fsData.offset, result_fsData.offset);
                            return RT_ERR_FAILED;
                        }
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_fieldSelector_set (unit %u, fsIdx %u)"
                                            , ret, RT_ERR_OK, unit, fsIdx[fsIdx_index]);
                    }
                }
            }

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_fieldSelector_get(unit, fsIdx[fsIdx_index], &result_fsData);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                 , ret, expect_result, unit, fsIdx[fsIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                 , ret, RT_ERR_OK, unit, fsIdx[fsIdx_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 fsIdx;
        uint32 fsIdx_max;
        rtk_pie_fieldSelector_startPosition_t start;
        uint32 start_max;
        uint32 offset;
        uint32 offset_max;

        rtk_pie_fieldSelector_data_t fsData;
        rtk_pie_fieldSelector_data_t result_fsData;

        G_VALUE(TEST_MAX_FS_INDEX(unit), fsIdx_max);
        G_VALUE(TEST_MAX_FS_START_POS, start_max);
        G_VALUE(TEST_ENTRY_ID_MAX(unit), offset_max);

        for (L_VALUE(TEST_MIN_FS_INDEX, fsIdx); fsIdx <= fsIdx_max; fsIdx++)
        {
            ASSIGN_EXPECT_RESULT(fsIdx, TEST_MIN_FS_INDEX, TEST_MAX_FS_INDEX(unit), inter_result2);

            for (L_VALUE(TEST_MIN_FS_START_POS, start); start <= start_max; start++)
            {
                ASSIGN_EXPECT_RESULT(start, TEST_MIN_FS_START_POS, TEST_MAX_FS_START_POS, inter_result3);

                for (L_VALUE(TEST_MIN_FS_OFFSET, offset); offset <= offset_max; offset++)
                {
                    ASSIGN_EXPECT_RESULT(offset, TEST_MIN_FS_OFFSET, TEST_ENTRY_ID_MAX(unit), inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    if ((HWP_8390_50_FAMILY(unit)) && FS_START_POS_L2 == start)
                        expect_result = RT_ERR_FAILED;

                    if ((HWP_8380_30_FAMILY(unit)) && (FS_START_POS_LLC == start || FS_START_POS_ARP == start ||
                        FS_START_POS_IPV4 == start || FS_START_POS_IPV6 == start ||
                        FS_START_POS_IP == start))
                        expect_result = RT_ERR_FAILED;
                    osal_memset(&fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                    fsData.start = start;
                    fsData.offset = offset;

                    ret = rtk_pie_fieldSelector_set(unit, fsIdx, &fsData);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_set (unit %u, fsIdx %u)"
                                        , ret, expect_result, unit, fsIdx);
                        osal_memset(&result_fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                        ret = rtk_pie_fieldSelector_get(unit, fsIdx, &result_fsData);
                        RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                         , ret, expect_result, unit, fsIdx);
                        if (osal_memcmp(&fsData, &result_fsData, sizeof(rtk_pie_fieldSelector_data_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_pie_fieldSelector_set/get value unequal (unit %u, fsIdx %u)\n", __FUNCTION__, __LINE__, unit, fsIdx);
                            return RT_ERR_FAILED;
                        }
                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_fieldSelector_set (unit %u, fsIdx %u)"
                                            , ret, RT_ERR_OK, unit, fsIdx);
                }
            }

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_fieldSelector_get(unit, fsIdx, &result_fsData);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                 , ret, expect_result, unit, fsIdx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_fieldSelector_get (unit %u, fsIdx %u)"
                                 , ret, RT_ERR_OK, unit, fsIdx);
            }
        }
    }

    return RT_ERR_OK;
}
#if 0 /*no rtk api*/
int32 dal_acl_portFieldSelector_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int32  expect_result = 1;

    rtk_pie_fieldSelector_data_t temp_fsData;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_portFieldSelector_set(unit, 0, 0, &temp_fsData))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_portFieldSelector_get(unit, 0, 0, &temp_fsData)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32 fsIdx[UNITTEST_MAX_TEST_VALUE];
        int32  fsIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  fsIdx_index;
        int32  fsIdx_last;

        rtk_pie_fieldSelector_startPosition_t start[UNITTEST_MAX_TEST_VALUE];
        int32  start_result[UNITTEST_MAX_TEST_VALUE];
        int32  start_index;
        int32  start_last;

        uint32 offset[UNITTEST_MAX_TEST_VALUE];
        int32  offset_result[UNITTEST_MAX_TEST_VALUE];
        int32  offset_index;
        int32  offset_last;

        rtk_pie_fieldSelector_data_t fsData;
        rtk_pie_fieldSelector_data_t result_fsData;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN, port, port_result, port_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_FS_INDEX(unit), TEST_MIN_FS_INDEX, fsIdx, fsIdx_result, fsIdx_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_FS_START_POS, TEST_MIN_FS_START_POS, start, start_result, start_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_MIN_FS_OFFSET, offset, offset_result, offset_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];

            for (fsIdx_index = 0; fsIdx_index <= fsIdx_last; fsIdx_index++)
            {
                inter_result3 = fsIdx_result[fsIdx_index];

                for (start_index = 0; start_index <= start_last; start_index++)
                {
                    inter_result4 = start_result[start_index];

                    for (offset_index = 0; offset_index <= offset_last; offset_index++)
                    {
                        inter_result5 = offset_result[offset_index];
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;

                        if ((HWP_8390_50_FAMILY(unit)) && FS_START_POS_L2 == start[start_index])
                            expect_result = RT_ERR_FAILED;
                        if ((HWP_8380_30_FAMILY(unit)) && (FS_START_POS_LLC == start[start_index] || FS_START_POS_ARP == start[start_index] ||
                            FS_START_POS_IPV4 == start[start_index] || FS_START_POS_IPV6 == start[start_index] ||
                            FS_START_POS_IP == start[start_index]))
                            expect_result = RT_ERR_FAILED;

                        osal_memset(&fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                        fsData.start = start[start_index];
                        fsData.offset = offset[offset_index];

                        ret = rtk_acl_portFieldSelector_set(unit, port[port_index], fsIdx[fsIdx_index], &fsData);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_set (unit %u, port %u, fsIdx %u)"
                                            , ret, expect_result, unit, port[port_index], fsIdx[fsIdx_index]);
                            osal_memset(&result_fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                            ret = rtk_acl_portFieldSelector_get(unit, port[port_index], fsIdx[fsIdx_index], &result_fsData);
                            #if 0 /* debug message */
                            osal_printf("result_fsData.start = %x\n", result_fsData.start);
                            osal_printf("result_fsData.offset = %x\n", result_fsData.offset);
                            #endif
                            RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                             , ret, expect_result, unit, port[port_index], fsIdx[fsIdx_index]);
                            if (osal_memcmp(&fsData, &result_fsData, sizeof(rtk_pie_fieldSelector_data_t)) != 0)
                            {
                                osal_printf("\t%s(%u): rtk_acl_portFieldSelector_set/get value unequal (unit %u, port %u, fsIdx %u)\n", __FUNCTION__, __LINE__, unit, port[port_index], fsIdx[fsIdx_index]);
                                return RT_ERR_FAILED;
                            }
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_portFieldSelector_set (unit %u, port %u, fsIdx %u)"
                                                , ret, RT_ERR_OK, unit, port[port_index], fsIdx[fsIdx_index]);
                        }
                    }
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_portFieldSelector_get(unit, port[port_index], fsIdx[fsIdx_index], &result_fsData);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                     , ret, expect_result, unit, port[port_index], fsIdx[fsIdx_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                     , ret, RT_ERR_OK, unit, port[port_index], fsIdx[fsIdx_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t port;
        uint32 port_max;
        uint32 fsIdx;
        uint32 fsIdx_max;
        rtk_pie_fieldSelector_startPosition_t start;
        uint32 start_max;
        uint32 offset;
        uint32 offset_max;

        rtk_pie_fieldSelector_data_t fsData;
        rtk_pie_fieldSelector_data_t result_fsData;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_MAX_FS_INDEX(unit), fsIdx_max);
        G_VALUE(TEST_MAX_FS_START_POS, start_max);
        G_VALUE(TEST_ENTRY_ID_MAX(unit), offset_max);

        for (L_VALUE(TEST_PORT_ID_MIN, port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN, TEST_PORT_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_MIN_FS_INDEX, fsIdx); fsIdx <= fsIdx_max; fsIdx++)
            {
                ASSIGN_EXPECT_RESULT(fsIdx, TEST_MIN_FS_INDEX, TEST_MAX_FS_INDEX(unit), inter_result3);

                for (L_VALUE(TEST_MIN_FS_START_POS, start); start <= start_max; start++)
                {
                    ASSIGN_EXPECT_RESULT(start, TEST_MIN_FS_START_POS, TEST_MAX_FS_START_POS, inter_result4);

                    for (L_VALUE(TEST_MIN_FS_OFFSET, offset); offset <= offset_max; offset++)
                    {
                        ASSIGN_EXPECT_RESULT(offset, TEST_MIN_FS_OFFSET, TEST_ENTRY_ID_MAX(unit), inter_result5);

                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;
                        if ((HWP_8390_50_FAMILY(unit)) && FS_START_POS_L2 == start)
                            expect_result = RT_ERR_FAILED;
                        if ((HWP_8380_30_FAMILY(unit)) && (FS_START_POS_LLC == start || FS_START_POS_ARP == start ||
                            FS_START_POS_IPV4 == start || FS_START_POS_IPV6 == start ||
                            FS_START_POS_IP == start))
                            expect_result = RT_ERR_FAILED;
                        osal_memset(&fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                        fsData.start = start;
                        fsData.offset = offset;

                        ret = rtk_acl_portFieldSelector_set(unit, port, fsIdx, &fsData);
                        if (expect_result == RT_ERR_OK)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_set (unit %u, port %u, fsIdx %u)"
                                            , ret, expect_result, unit, port, fsIdx);
                            osal_memset(&result_fsData, 0, sizeof(rtk_pie_fieldSelector_data_t));
                            ret = rtk_acl_portFieldSelector_get(unit, port, fsIdx, &result_fsData);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                             , ret, expect_result, unit, port, fsIdx);
                            if (osal_memcmp(&fsData, &result_fsData, sizeof(rtk_pie_fieldSelector_data_t)) != 0)
                            {
                                osal_printf("\t%s(%u): rtk_acl_portFieldSelector_set/get value unequal (unit %u, port %u, fsIdx %u)\n", __FUNCTION__, __LINE__, unit, port, fsIdx);
                                return RT_ERR_FAILED;
                            }
                        }
                        else
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_portFieldSelector_set (unit %u, port %u, fsIdx %u)"
                                                , ret, RT_ERR_OK, unit, port, fsIdx);
                    }
                }

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_portFieldSelector_get(unit, port, fsIdx, &result_fsData);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                     , ret, expect_result, unit, port, fsIdx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_portFieldSelector_get (unit %u, port %u, fsIdx %u)"
                                     , ret, RT_ERR_OK, unit, port, fsIdx);
                }
            }
        }
    }

    return RT_ERR_OK;
}
#endif
int32 dal_acl_meterMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pie_meterMode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_meterMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_meterMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  meterModeId[UNITTEST_MAX_TEST_VALUE];
        int32  meterModeId_result[UNITTEST_MAX_TEST_VALUE];
        int32  meterModeId_index;
        int32  meterModeId_last;

        rtk_pie_meterMode_t  mode;
        uint32  mode_max;

        rtk_pie_meterMode_t  result_mode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_METER_MODE_INDEX_MAX(unit), TEST_METER_MODE_INDEX_MIN(unit), meterModeId, meterModeId_result, meterModeId_last);

        G_VALUE(TEST_METERMODE_MAX, mode_max);

        for (meterModeId_index = 0; meterModeId_index <= meterModeId_last; meterModeId_index++)
        {
            inter_result2 = meterModeId_result[meterModeId_index];
            for (L_VALUE(TEST_METERMODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_METERMODE_MIN, TEST_METERMODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_meterMode_set(unit, meterModeId[meterModeId_index], mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_set (unit %u, meterModeId %u, mode %u)"
                                    , ret, expect_result, unit, meterModeId[meterModeId_index], mode);

                    ret = rtk_acl_meterMode_get(unit, meterModeId[meterModeId_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u, mode %u)"
                                     , ret, expect_result, unit, meterModeId[meterModeId_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_set/get value unequal (unit %u, meterModeId %u, mode %u)"
                                     , result_mode, mode, unit, meterModeId[meterModeId_index], mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterMode_set (unit %u, meterModeId %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, meterModeId[meterModeId_index], mode);
                }
            }
            ret = rtk_acl_meterMode_get(unit, meterModeId[meterModeId_index], &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u)"
                                , ret, expect_result, unit, meterModeId[meterModeId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u)"
                                    , ret, RT_ERR_OK, unit, meterModeId[meterModeId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 meterModeId;
        uint32 meterModeId_max;
        rtk_pie_meterMode_t  mode;
        uint32  mode_max;

        rtk_pie_meterMode_t  result_mode;

        G_VALUE(TEST_METER_MODE_INDEX_MAX(unit), meterModeId_max);
        G_VALUE(TEST_METERMODE_MAX, mode_max);

        for (L_VALUE(TEST_METER_MODE_INDEX_MIN(unit), meterModeId); meterModeId <= meterModeId_max; meterModeId++)
        {
            ASSIGN_EXPECT_RESULT(meterModeId, TEST_METER_MODE_INDEX_MIN(unit), TEST_METER_MODE_INDEX_MAX(unit), inter_result2);

            for (L_VALUE(TEST_METERMODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_METERMODE_MIN, TEST_METERMODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_acl_meterMode_set(unit, meterModeId, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_set (unit %u, meterModeId %u, mode %u)"
                                    , ret, expect_result, unit, meterModeId, mode);

                    ret = rtk_acl_meterMode_get(unit, meterModeId, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u, mode %u)"
                                     , ret, expect_result, unit, meterModeId, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_set/get value unequal (unit %u, meterModeId %u, mode %u)"
                                     , result_mode, mode, unit, meterModeId, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterMode_set (unit %u, meterModeId %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, meterModeId, mode);
                }
                ret = rtk_acl_meterMode_get(unit, meterModeId, &result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u)"
                                    , ret, expect_result, unit, meterModeId);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterMode_get (unit %u, meterModeId %u)"
                                        , ret, RT_ERR_OK, unit, meterModeId);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_meterIncludeIfg_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_meterIncludeIfg_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_meterIncludeIfg_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
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
            ret = rtk_pie_meterIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_pie_meterIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
        }
        ret = rtk_pie_meterIncludeIfg_get(unit, &result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u)"
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
            ret = rtk_pie_meterIncludeIfg_set(unit, enable);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);

                ret = rtk_pie_meterIncludeIfg_get(unit, &result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterIncludeIfg_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
            }
            ret = rtk_pie_meterIncludeIfg_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterIncludeIfg_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_meterBurstSize_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_acl_meterBurstSize_t temp_meterBurstSize;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_meterBurstSize_set(unit, 0, &temp_meterBurstSize))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_meterBurstSize_get(unit, 0, &temp_meterBurstSize)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pie_meterMode_t mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_acl_meterBurstSize_t meterBurstSize;
        rtk_acl_meterBurstSize_t result_meterBurstSize;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_METERMODE_MAX, TEST_METERMODE_MIN, mode, mode_result, mode_last);

        for (mode_index = 0; mode_index <= mode_last; mode_index++)
        {
            inter_result2 = mode_result[mode_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&meterBurstSize, 0, sizeof(rtk_acl_meterBurstSize_t));
#if defined (CONFIG_SDK_RTL8380)
            if (HWP_8380_30_FAMILY(unit))
            {
                meterBurstSize.slb_lb0bs = ut_rand()&0xFFFF;
                meterBurstSize.slb_lb1bs = ut_rand()&0xFFFF;
            }
#endif
#if defined (CONFIG_SDK_RTL8390)
            if (HWP_8390_50_FAMILY(unit))
            {
                meterBurstSize.dlb_lb0bs = ut_rand()&0xFFFF;
                meterBurstSize.dlb_lb1bs = ut_rand()&0xFFFF;
                meterBurstSize.srtcm_cbs = ut_rand()&0xFFFF;
                meterBurstSize.srtcm_ebs = ut_rand()&0xFFFF;
                meterBurstSize.trtcm_cbs = ut_rand()&0xFFFF;
                meterBurstSize.trtcm_pbs = ut_rand()&0xFFFF;
            }
#endif
            ret = rtk_acl_meterBurstSize_set(unit, mode[mode_index], &meterBurstSize);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_meterBurstSize_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);
                osal_memset(&result_meterBurstSize, 0, sizeof(rtk_acl_meterBurstSize_t));
                ret = rtk_acl_meterBurstSize_get(unit, mode[mode_index], &result_meterBurstSize);
                #if 0 /* debug message */
                #if defined(CONFIG_SDK_RTL8380)
                osal_printf("meterBurstSize.slb_lb0bs = %x\n", result_meterBurstSize.slb_lb0bs);
                osal_printf("meterBurstSize.slb_lb1bs = %x\n", result_meterBurstSize.slb_lb1bs);
                #endif
                #if defined(CONFIG_SDK_RTL8390)
                osal_printf("meterBurstSize.dlb_lb0bs = %x\n", result_meterBurstSize.dlb_lb0bs);
                osal_printf("meterBurstSize.dlb_lb1bs = %x\n", result_meterBurstSize.dlb_lb1bs);
                osal_printf("meterBurstSize.srtcm_cbs = %d\n", result_meterBurstSize.srtcm_cbs);
                osal_printf("meterBurstSize.srtcm_ebs = %d\n", result_meterBurstSize.srtcm_ebs);
                osal_printf("meterBurstSize.trtcm_cbs = %d\n", result_meterBurstSize.trtcm_cbs);
                osal_printf("meterBurstSize.trtcm_pbs = %d\n", result_meterBurstSize.trtcm_pbs);
                #endif

                #endif
                RT_TEST_IS_EQUAL_INT("rtk_acl_meterBurstSize_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode[mode_index]);
                if (osal_memcmp(&meterBurstSize, &result_meterBurstSize, sizeof(rtk_acl_meterBurstSize_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_acl_meterBurstSize_set/get value unequal (unit %u, mode %u)\n", __FUNCTION__, __LINE__, unit, mode[mode_index]);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterBurstSize_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pie_meterMode_t mode;
        uint32 mode_max;

        rtk_acl_meterBurstSize_t meterBurstSize;
        rtk_acl_meterBurstSize_t result_meterBurstSize;

        G_VALUE(TEST_METERMODE_MAX, mode_max);

        for (L_VALUE(TEST_METERMODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_METERMODE_MIN, TEST_METERMODE_MAX, inter_result2);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&meterBurstSize, 0, sizeof(rtk_acl_meterBurstSize_t));
#if defined (CONFIG_SDK_RTL8380)
                if (HWP_8380_30_FAMILY(unit))
                {
                    meterBurstSize.slb_lb0bs = ut_rand()&0xFFFF;
                    meterBurstSize.slb_lb1bs = ut_rand()&0xFFFF;
                }
#endif
#if defined (CONFIG_SDK_RTL8390)
                if (HWP_8390_50_FAMILY(unit))
                {
                    meterBurstSize.dlb_lb0bs = ut_rand()&0xFFFF;
                    meterBurstSize.dlb_lb1bs = ut_rand()&0xFFFF;
                    meterBurstSize.srtcm_cbs = ut_rand()&0xFFFF;
                    meterBurstSize.srtcm_ebs = ut_rand()&0xFFFF;
                    meterBurstSize.trtcm_cbs = ut_rand()&0xFFFF;
                    meterBurstSize.trtcm_pbs = ut_rand()&0xFFFF;
                }
#endif
                ret = rtk_acl_meterBurstSize_set(unit, mode, &meterBurstSize);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterBurstSize_set (unit %u, mode %u)"
                                    , ret, expect_result, unit, mode);
                    osal_memset(&result_meterBurstSize, 0, sizeof(rtk_acl_meterBurstSize_t));
                    ret = rtk_acl_meterBurstSize_get(unit, mode, &result_meterBurstSize);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_meterBurstSize_get (unit %u, mode %u)"
                                     , ret, expect_result, unit, mode);
                    if (osal_memcmp(&meterBurstSize, &result_meterBurstSize, sizeof(rtk_acl_meterBurstSize_t)) != 0)
                    {
                        osal_printf("\t%s(%u): rtk_acl_meterBurstSize_set/get value unequal (unit %u, mode %u)\n", __FUNCTION__, __LINE__, unit, mode);
                        return RT_ERR_FAILED;
                    }
                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_meterBurstSize_set (unit %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, mode);
            }
    }

    return RT_ERR_OK;
}

int32 dal_acl_meterExceed_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  isExceed_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_meterExceed_get(unit, 0, &isExceed_temp))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  meterId[UNITTEST_MAX_TEST_VALUE];
        int32  meterId_result[UNITTEST_MAX_TEST_VALUE];
        int32  meterId_index;
        int32  meterId_last;

        uint32  isExceed;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_METER_INDEX_MAX(unit), TEST_METER_INDEX_MIN, meterId, meterId_result, meterId_last);

        for (meterId_index = 0; meterId_index <= meterId_last; meterId_index++)
        {
            inter_result2 = meterId_result[meterId_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_meterExceed_get(unit, meterId[meterId_index], &isExceed);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterExceed_get (unit %u, meterId %u, exceed %u)"
                                , ret, expect_result, unit, meterId[meterId_index], isExceed);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterExceed_get (unit %u, meterId %u, exceed %u)"
                                    , ret, RT_ERR_OK, unit, meterId[meterId_index], isExceed);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 meterId;
        uint32 meterId_max;
        uint32 isExceed;

        G_VALUE(TEST_METER_INDEX_MAX(unit), meterId_max);

        for (L_VALUE(TEST_METER_INDEX_MIN, meterId); meterId <= meterId_max; meterId++)
        {
            ASSIGN_EXPECT_RESULT(meterId, TEST_METER_INDEX_MIN, TEST_METER_INDEX_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_meterExceed_get(unit, meterId, &isExceed);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterExceed_get (unit %u, meterId %u, exceed %u)"
                                , ret, expect_result, unit, meterId, isExceed);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterExceed_get (unit %u, meterId %u, exceed %u)"
                                    , ret, RT_ERR_OK, unit, meterId, isExceed);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_meterEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_pie_meterEntry_t temp_meterEntry;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_meterEntry_set(unit, 0, &temp_meterEntry))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_meterEntry_get(unit, 0, &temp_meterEntry)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 meterId[UNITTEST_MAX_TEST_VALUE];
        int32  meterId_result[UNITTEST_MAX_TEST_VALUE];
        int32  meterId_index;
        int32  meterId_last;

        rtk_pie_meterEntry_t meterEntry;
        rtk_pie_meterEntry_t result_meterEntry;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_METER_INDEX_MAX(unit), TEST_METER_INDEX_MIN, meterId, meterId_result, meterId_last);

        for (meterId_index = 0; meterId_index <= meterId_last; meterId_index++)
        {
            inter_result2 = meterId_result[meterId_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&meterEntry, 0, sizeof(rtk_pie_meterEntry_t));
#if defined (CONFIG_SDK_RTL8380)
            if(HWP_8380_30_FAMILY(unit))
            {
                meterEntry.rate = ut_rand()&0xFFFF;
                meterEntry.thr_grp = ut_rand()%2;
            }
#endif
            //if ((METER_TYPE_DLB == meterEntry.type) || (METER_TYPE_SRTCM == meterEntry.type) || (METER_TYPE_TRTCM == meterEntry.type))
            //    expect_result = RT_ERR_FAILED;
#if defined (CONFIG_SDK_RTL8390)
            if(HWP_8390_50_FAMILY(unit))
            {
                meterEntry.type = ut_rand()%5;
                meterEntry.color_aware = ut_rand()%2;
                meterEntry.lb0_rate = ut_rand()&0xFFFF;
                meterEntry.lb1_rate = ut_rand()&0xFFFF;
                if (METER_TYPE_SLB == meterEntry.type)
                    expect_result = RT_ERR_FAILED;
            }
#endif

            ret = rtk_pie_meterEntry_set(unit, meterId[meterId_index], &meterEntry);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterEntry_set (unit %u, meterId %u)"
                                , ret, expect_result, unit, meterId[meterId_index]);
                osal_memset(&result_meterEntry, 0, sizeof(rtk_pie_meterEntry_t));
                ret = rtk_pie_meterEntry_get(unit, meterId[meterId_index], &result_meterEntry);
                #if 0 /* debug message */
                osal_printf("meterEntry.type = %x\n", result_meterEntry.type);
                #if defined(CONFIG_SDK_RTL8380)
                osal_printf("meterEntry.rate = %x\n", result_meterEntry.rate);
                osal_printf("meterEntry.thr_grp = %x\n", result_meterEntry.thr_grp);
                #endif
                if(HWP_8390_50_FAMILY(unit))
                {
                    osal_printf("meterEntry.color_aware = %x\n", result_meterEntry.color_aware);
                    osal_printf("meterEntry.lb0_rate = %x\n", result_meterEntry.lb0_rate);
                    osal_printf("meterEntry.lb1_rate = %d\n", result_meterEntry.lb1_rate);
                }
                #endif
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterEntry_get (unit %u, meterId %u)"
                                 , ret, expect_result, unit, meterId[meterId_index]);
                if (osal_memcmp(&meterEntry, &result_meterEntry, sizeof(rtk_pie_meterEntry_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_pie_meterEntry_set/get value unequal (unit %u, meterId %u)\n", __FUNCTION__, __LINE__, unit, meterId[meterId_index]);
                    return RT_ERR_FAILED;
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterEntry_set (unit %u, meterId %u)"
                                    , ret, RT_ERR_OK, unit, meterId[meterId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 meterId;
        uint32 meterId_max;

        rtk_pie_meterEntry_t meterEntry;
        rtk_pie_meterEntry_t result_meterEntry;

        G_VALUE(TEST_METER_INDEX_MAX(unit), meterId_max);

        for (L_VALUE(TEST_METER_INDEX_MIN, meterId); meterId <= meterId_max; meterId++)
        {
            ASSIGN_EXPECT_RESULT(meterId, TEST_METER_INDEX_MIN, TEST_METER_INDEX_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            osal_memset(&meterEntry, 0, sizeof(rtk_pie_meterEntry_t));
#if defined (CONFIG_SDK_RTL8380)
            if(HWP_8380_30_FAMILY(unit))
            {
                meterEntry.rate = ut_rand()&0xFFFF;
                meterEntry.thr_grp = ut_rand()%2;
            }
#endif
            //if ((METER_TYPE_DLB == meterEntry.type) || (METER_TYPE_SRTCM == meterEntry.type) || (METER_TYPE_TRTCM == meterEntry.type))
            //    expect_result = RT_ERR_FAILED;
#if defined (CONFIG_SDK_RTL8390)
            if(HWP_8390_50_FAMILY(unit))
            {
                meterEntry.type = ut_rand()%5;
                meterEntry.color_aware = ut_rand()%2;
                meterEntry.lb0_rate = ut_rand()&0xFFFF;
                meterEntry.lb1_rate = ut_rand()&0xFFFF;
                if (METER_TYPE_SLB == meterEntry.type)
                    expect_result = RT_ERR_FAILED;
            }
#endif

            ret = rtk_pie_meterEntry_set(unit, meterId, &meterEntry);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterEntry_set (unit %u, meterId %u)"
                                , ret, expect_result, unit, meterId);
                osal_memset(&result_meterEntry, 0, sizeof(rtk_pie_meterEntry_t));
                ret = rtk_pie_meterEntry_get(unit, meterId, &result_meterEntry);
                RT_TEST_IS_EQUAL_INT("rtk_pie_meterEntry_get (unit %u, meterId %u)"
                                 , ret, expect_result, unit, meterId);
                if (osal_memcmp(&meterEntry, &result_meterEntry, sizeof(rtk_pie_meterEntry_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_pie_meterEntry_set/get value unequal (unit %u, meterId %u)\n", __FUNCTION__, __LINE__, unit, meterId);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_meterEntry_set (unit %u, meterId %u)"
                                    , ret, RT_ERR_OK, unit, meterId);
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_ruleEntryFieldCheck_test(uint32 caseNo, uint32 unit)
{
    int32  ret;
    int8   inter_result2 = 1;
    int8   inter_result3 = 1;
    int32  expect_result = 1;

    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleEntryField_check(unit, 0, 0))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  phaseVal[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_result[UNITTEST_MAX_TEST_VALUE];
        int32   phaseVal_index;
        int32   phaseVal_last;

        rtk_acl_fieldType_t type[UNITTEST_MAX_TEST_VALUE];
        int32               type_result[UNITTEST_MAX_TEST_VALUE];
        int32               type_index;
        int32               type_last;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phaseVal, phaseVal_result, phaseVal_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_TEMPLATE_TYPE, TEST_MIN_TEMPLATE_TYPE, type, type_result, type_last);

        for (phaseVal_index = 0; phaseVal_index <= phaseVal_last; phaseVal_index++)
        {
            inter_result2 = phaseVal_result[phaseVal_index];

            for (type_index = 0; type_index <= type_last; type_index++)
            {
                inter_result3 = type_result[type_index];

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8380_30_FAMILY(unit)) && ((type[type_index] >= USER_FIELD_OMPLS_LABEL && type[type_index] <= USER_FIELD_IMPLS_LABEL_EXIST) ||
                    (type[type_index] == USER_FIELD_IP4_CHKSUM_ERROR) || (USER_FIELD_IP_FRAGMENT_OFFSET == type[type_index]) ||
                    (type[type_index] == USER_FIELD_IP6_ESP_HDR_EXIST) || (USER_FIELD_VID_RANGE1 == type[type_index]) ||
                    (type[type_index] >= USER_FIELD_FIELD_SELECTOR4 && type[type_index] <= USER_FIELD_FIELD_SELECTOR11) ||
                    (type[type_index] >= USER_FIELD_DPM && type[type_index] <= USER_FIELD_OVID) ||
                    (type[type_index] >= USER_FIELD_IGR_ACL_DROP_HIT && type[type_index] <= USER_FIELD_IGR_ACL_ROUTING_HIT)))
                    expect_result = RT_ERR_FAILED;
                /* For test chip */
                if ((HWP_8390_50_FAMILY(unit)) && (((ACL_PHASE_IGR_ACL == phaseVal[phaseVal_index]) && (type[type_index] >= USER_FIELD_DPM)) ||
                    (USER_FIELD_DROPPED == type[type_index]) || (USER_FIELD_FLOW_LABEL == type[type_index]) ||
                    (USER_FIELD_IP6_MOB_HDR_EXIST == type[type_index]) ||
                    (USER_FIELD_IGMP_GROUPIP == type[type_index]) || (USER_FIELD_DATYPE == type[type_index]) ||
                    (type[type_index] == USER_FIELD_PPPOE) ||
                    (type[type_index] >= USER_FIELD_RRCPKEY && type[type_index] <= USER_FIELD_RRCPHLNULLMAC) ||
                    (USER_FIELD_FLDSEL_RANGE == type[type_index])))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_acl_ruleEntryField_check(unit, phaseVal[phaseVal_index], type[type_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntryField_check (unit %u, phase %u, type %u)"
                                     , ret, expect_result, unit, phaseVal[phaseVal_index], type[type_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntryField_check (unit %u, phase %u, type %u)"
                                     , ret, RT_ERR_OK, unit, phaseVal[phaseVal_index], type[type_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32                  phaseVal;
        uint32                  phaseVal_max;
        rtk_acl_fieldType_t     type;
        uint32                  type_max;

        G_VALUE(TEST_MAX_PHASE_VALUE(unit), phaseVal_max);
        G_VALUE(TEST_MAX_TEMPLATE_TYPE, type_max);

        for (L_VALUE(TEST_MIN_PHASE_VALUE, phaseVal); phaseVal <= phaseVal_max; phaseVal++)
        {
            ASSIGN_EXPECT_RESULT(phaseVal, TEST_MIN_PHASE_VALUE, TEST_MAX_PHASE_VALUE(unit), inter_result2);

            for (L_VALUE(TEST_MIN_TEMPLATE_TYPE, type); type <= type_max; type++)
            {
                ASSIGN_EXPECT_RESULT(type, TEST_MIN_TEMPLATE_TYPE, TEST_MAX_TEMPLATE_TYPE, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                if ((HWP_8380_30_FAMILY(unit)) && ((type >= USER_FIELD_OMPLS_LABEL && type <= USER_FIELD_IMPLS_LABEL_EXIST) ||
                    (type == USER_FIELD_IP4_CHKSUM_ERROR) || (USER_FIELD_IP_FRAGMENT_OFFSET == type) ||
                    (type == USER_FIELD_IP6_ESP_HDR_EXIST) || (USER_FIELD_VID_RANGE1 == type) ||
                    (type >= USER_FIELD_FIELD_SELECTOR4 && type <= USER_FIELD_FIELD_SELECTOR11) ||
                    (type >= USER_FIELD_DPM && type <= USER_FIELD_OVID) ||
                    (type >= USER_FIELD_IGR_ACL_DROP_HIT && type <= USER_FIELD_IGR_ACL_ROUTING_HIT)))
                    expect_result = RT_ERR_FAILED;
                /* For test chip */
                if ((HWP_8390_50_FAMILY(unit)) && (((ACL_PHASE_IGR_ACL == phaseVal) && (type >= USER_FIELD_DPM)) ||
                    (USER_FIELD_DROPPED == type) || (USER_FIELD_FLOW_LABEL == type) ||
                    (USER_FIELD_IP6_MOB_HDR_EXIST == type) ||
                    (USER_FIELD_IGMP_GROUPIP == type) || (USER_FIELD_DATYPE == type) ||
                    (type == USER_FIELD_PPPOE) ||
                    (type >= USER_FIELD_RRCPKEY && type <= USER_FIELD_RRCPHLNULLMAC) ||
                    (USER_FIELD_FLDSEL_RANGE == type)))
                    expect_result = RT_ERR_FAILED;
                ret = rtk_acl_ruleEntryField_check(unit, phaseVal, type);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntryField_check (unit %u, phase %u, type %u)"
                                     , ret, expect_result, unit, phaseVal, type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntryField_check (unit %u, phase %u, type %u)"
                                     , ret, RT_ERR_OK, unit, phaseVal, type);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_ruleEntry_test(uint32 caseNo, uint32 unit)
{
    uint32 blockId;
    int32  ret, index;
    int32  expect_result = 1;
    uint8  entry_buffer[TEST_ACL_ENTRY_LENGTH];
    uint8  entry_data[TEST_ACL_ENTRY_LENGTH] = {
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
        0xff, 0xff, 0xdf, /* 8380 sphy[5:0] only use 5-bits, and sphy[5] is reserved */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int8   inter_result2 = 1;
    int8   inter_result3 = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleEntry_read(unit, 0, 0, entry_buffer))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleEntry_write(unit, 0, 0, entry_buffer)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase[UNITTEST_MAX_TEST_VALUE];
        int32  phase_result[UNITTEST_MAX_TEST_VALUE];
        int32  phase_index;
        int32  phase_last;

        rtk_pie_id_t  entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;
        if(HWP_8390_50_FAMILY(unit))
            entry_data[29] = 0xff;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phase, phase_result, phase_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_ENTRY_ID_MIN, entry, entry_result, entry_last);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (phase_index = 0; phase_index <= phase_last; phase_index++)
        {
            inter_result2 = phase_result[phase_index];
            if (ACL_PHASE_IGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, 0);
            }
            for (entry_index = 0; entry_index <= entry_last; entry_index++)
            {
#if defined(CONFIG_SDK_FPGA_PLATFORM)
                /* The FPGA board ACL only exist 7 block, per-block 4 entries */
                if ((HWP_8390_50_FAMILY(unit)) && (entry[entry_index]%128) >= 4 || (entry[entry_index]/128) >= 7)
                    continue;
#endif

                inter_result3 = entry_result[entry_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memcpy(entry_buffer, entry_data, TEST_ACL_ENTRY_LENGTH);
                ret = rtk_acl_ruleEntry_write(unit, phase[phase_index], entry[entry_index], entry_data);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntry_write (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase[phase_index], entry[entry_index]);
                    osal_memset(entry_buffer, 0, TEST_ACL_ENTRY_LENGTH);
                    ret = rtk_acl_ruleEntry_read(unit, phase[phase_index], entry[entry_index], entry_buffer);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntry_read (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase[phase_index], entry[entry_index]);

                    for(index=0; index < TEST_ACL_ENTRY_LENGTH; index++)
                    {
                        osal_printf("(Get)entry_buffer[%d]=%x, (Set)entry_data[%d]=%x\n", index, entry_buffer[index], index, entry_data[index]);
                    }
                    for(index=0; index < TEST_ACL_ENTRY_LENGTH; index++)
                    {
                        if(entry_buffer[index] != entry_data[index])
                        {
                            osal_printf("entry_buffer[%d]=%x, entry_data[%d]=%x\n", index, entry_buffer[index], index, entry_data[index]);
                            osal_printf("\t%s(%u): rtk_acl_ruleEntry_write/read value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase[phase_index], entry[entry_index]);
                            return RT_ERR_FAILED;
                        }
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntry_write (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase[phase_index], entry[entry_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase;
        uint32 phase_max;
        rtk_pie_id_t  entry;
        uint32  entry_max;

        G_VALUE(TEST_MAX_PHASE_VALUE(unit), phase_max);
        G_VALUE(TEST_ENTRY_ID_MAX(unit), entry_max);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

    for (L_VALUE(TEST_MIN_PHASE_VALUE, phase); phase <= phase_max; phase++)
        {
            ASSIGN_EXPECT_RESULT(phase, TEST_MIN_PHASE_VALUE, TEST_MAX_PHASE_VALUE(unit), inter_result2);
            if (ACL_PHASE_IGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, 0);
            }

            for (L_VALUE(TEST_ENTRY_ID_MIN, entry); entry <= entry_max; entry++)
            {
#if defined(CONFIG_SDK_FPGA_PLATFORM)
                /* The FPGA board ACL only exist 7 block, per-block 4 entries */
                if ((HWP_8390_50_FAMILY(unit)) && (entry%128) >= 4 || (entry/128) >= 7)
                    continue;
#endif
                ASSIGN_EXPECT_RESULT(entry, TEST_ENTRY_ID_MIN, TEST_ENTRY_ID_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                osal_memcpy(entry_buffer, entry_data, TEST_ACL_ENTRY_LENGTH);
                ret = rtk_acl_ruleEntry_write(unit, phase, entry, entry_data);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntry_write (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase, entry);
                    osal_memset(entry_buffer, 0, TEST_ACL_ENTRY_LENGTH);
                    ret = rtk_acl_ruleEntry_read( unit, phase, entry, entry_buffer);
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleEntry_read (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase, entry);
                    if (osal_memcmp(entry_buffer, entry_data, TEST_ACL_ENTRY_LENGTH) != 0)
                    {
                        osal_printf("\t%s(%u): rtk_acl_ruleEntry_write/read value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase, entry);
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleEntry_write (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase, entry);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_ruleEntryFieldGetSet_test(uint32 caseNo, uint32 unit)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

int32 dal_acl_ruleEntryFieldReadWrite_test(uint32 caseNo, uint32 unit)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

int32 dal_acl_ruleOperation_test(uint32 caseNo, uint32 unit)
{
    uint32 blockId;
    int32  ret;
    int32  expect_result = 1;
    int8   inter_result2 = 1;
    int8   inter_result3 = 1;
    rtk_acl_operation_t  aclOper_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleOperation_get(unit, 0, 0, &aclOper_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleOperation_set(unit, 0, 0, &aclOper_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase[UNITTEST_MAX_TEST_VALUE];
        int32  phase_result[UNITTEST_MAX_TEST_VALUE];
        int32  phase_index;
        int32  phase_last;

        rtk_pie_id_t  entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_acl_operation_t  aclOper;
        rtk_acl_operation_t  result_aclOper;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phase, phase_result, phase_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_ENTRY_ID_MIN, entry, entry_result, entry_last);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (phase_index = 0; phase_index <= phase_last; phase_index++)
        {
            inter_result2 = phase_result[phase_index];
            if (ACL_PHASE_IGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, 0);
            }
            for (entry_index = 0; entry_index <= entry_last; entry_index++)
            {
                inter_result3 = entry_result[entry_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&aclOper, 0, sizeof(rtk_acl_operation_t));
                aclOper.reverse = ut_rand()%2;
                aclOper.aggr_1 = ut_rand()%2;
                aclOper.aggr_2 = ut_rand()%2;

                if ((aclOper.aggr_1 == ENABLED) && (entry[entry_index]%2 !=0))
                    expect_result = RT_ERR_FAILED;
                if ((aclOper.aggr_2 == ENABLED) && (entry[entry_index]%2 !=0 || (entry[entry_index]/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
                    expect_result = RT_ERR_FAILED;

                /* Configure the acl entry to valid, else the setting value will not keep */
                rtk_acl_ruleValidate_set(unit, phase[phase_index], entry[entry_index], 1);
                ret = rtk_acl_ruleOperation_set(unit, phase[phase_index], entry[entry_index], &aclOper);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_set (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase[phase_index], entry[entry_index]);
                    osal_memset(&result_aclOper, 0, sizeof(rtk_acl_operation_t));
                    ret = rtk_acl_ruleOperation_get(unit, phase[phase_index], entry[entry_index], &result_aclOper);
                    /* Configure the invalid value of the acl entry back */
                    rtk_acl_ruleValidate_set(unit, phase[phase_index], entry[entry_index], 0);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase[phase_index], entry[entry_index]);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.reverse=%d, result_aclOper.reverse=%d"
                                     , aclOper.reverse, result_aclOper.reverse, unit, phase[phase_index], entry[entry_index], aclOper.reverse, result_aclOper.reverse);
                    if (entry[entry_index]%2 == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.aggr_1=%d, result_aclOper.aggr_1=%d"
                                     , aclOper.aggr_1, result_aclOper.aggr_1, unit, phase[phase_index], entry[entry_index], aclOper.aggr_1, result_aclOper.aggr_1);
                    if ((entry[entry_index]%2 == 0 && (entry[entry_index]/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 == 0))
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.aggr_2=%d, result_aclOper.aggr_2=%d"
                                     , aclOper.aggr_2, result_aclOper.aggr_2, unit, phase[phase_index], entry[entry_index], aclOper.aggr_2, result_aclOper.aggr_2);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleOperation_set (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase[phase_index], entry[entry_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase;
        uint32 phase_max;
        rtk_pie_id_t  entry;
        uint32  entry_max;

        rtk_acl_operation_t  aclOper;
        rtk_acl_operation_t  result_aclOper;

        G_VALUE(TEST_MAX_PHASE_VALUE(unit), phase_max);
        G_VALUE(TEST_ENTRY_ID_MAX(unit), entry_max);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (L_VALUE(TEST_MIN_PHASE_VALUE, phase); phase <= phase_max; phase++)
        {
            ASSIGN_EXPECT_RESULT(phase, TEST_MIN_PHASE_VALUE, TEST_MAX_PHASE_VALUE(unit), inter_result2);
            if (ACL_PHASE_IGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, 0);
            }

            for (L_VALUE(TEST_ENTRY_ID_MIN, entry); entry <= entry_max; entry++)
            {
                ASSIGN_EXPECT_RESULT(entry, TEST_ENTRY_ID_MIN, TEST_ENTRY_ID_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&aclOper, 0, sizeof(rtk_acl_operation_t));
                aclOper.reverse = ut_rand()%2;
                aclOper.aggr_1 = ut_rand()%2;
                aclOper.aggr_2 = ut_rand()%2;

                if ((aclOper.aggr_1 == ENABLED) && (entry%2 !=0))
                    expect_result = RT_ERR_FAILED;
                if ((aclOper.aggr_2 == ENABLED) && (entry%2 !=0 || (entry/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 != 0))
                    expect_result = RT_ERR_FAILED;

                /* Configure the acl entry to valid, else the setting value will not keep */
                rtk_acl_ruleValidate_set(unit, phase, entry, 1);
                ret = rtk_acl_ruleOperation_set(unit, phase, entry, &aclOper);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_set (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase, entry);
                    osal_memset(&result_aclOper, 0, sizeof(rtk_acl_operation_t));
                    ret = rtk_acl_ruleOperation_get(unit, phase, entry, &result_aclOper);
                    /* Configure the invalid value of the acl entry back */
                    rtk_acl_ruleValidate_set(unit, phase, entry, 0);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase, entry);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.reverse=%d, result_aclOper.reverse=%d"
                                     , aclOper.reverse, result_aclOper.reverse, unit, phase, entry, aclOper.reverse, result_aclOper.reverse);
                    if (entry%2 == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.aggr_1=%d, result_aclOper.aggr_1=%d"
                                     , aclOper.aggr_1, result_aclOper.aggr_1, unit, phase, entry, aclOper.aggr_1, result_aclOper.aggr_1);
                    if ((entry%2 == 0 && (entry/HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))%2 == 0))
                        RT_TEST_IS_EQUAL_INT("rtk_acl_ruleOperation_get (unit %u, phase %u, entry %u): aclOper.aggr_2=%d, result_aclOper.aggr_2=%d"
                                     , aclOper.aggr_2, result_aclOper.aggr_2, unit, phase, entry, aclOper.aggr_2, result_aclOper.aggr_2);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleOperation_set (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase, entry);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_ruleAction_test(uint32 caseNo, uint32 unit)
{
    uint32 blockId;
    int32  ret;
    int32  expect_result = 1;
    int8   inter_result2 = 1;
    int8   inter_result3 = 1;
    rtk_acl_action_t  action_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleAction_get(unit, 0, 0, &action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_ruleAction_set(unit, 0, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase[UNITTEST_MAX_TEST_VALUE];
        int32  phase_result[UNITTEST_MAX_TEST_VALUE];
        int32  phase_index;
        int32  phase_last;

        rtk_pie_id_t  entry[UNITTEST_MAX_TEST_VALUE];
        int32  entry_result[UNITTEST_MAX_TEST_VALUE];
        int32  entry_index;
        int32  entry_last;

        rtk_acl_action_t  action;
        rtk_acl_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MAX_PHASE_VALUE(unit), TEST_MIN_PHASE_VALUE, phase, phase_result, phase_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENTRY_ID_MAX(unit), TEST_ENTRY_ID_MIN, entry, entry_result, entry_last);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (phase_index = 0; phase_index <= phase_last; phase_index++)
        {
            inter_result2 = phase_result[phase_index];
            if (ACL_PHASE_IGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase[phase_index])
            {
                rtk_acl_partition_set(unit, 0);
            }
            for (entry_index = 0; entry_index <= entry_last; entry_index++)
            {
#if defined(CONFIG_SDK_FPGA_PLATFORM)
                /* The FPGA board ACL only exist 7 block, per-block 4 entries */
                if ((HWP_8390_50_FAMILY(unit)) && (entry[entry_index]%128) >= 4 || (entry[entry_index]/128) >= 7)
                    continue;
#endif
                inter_result3 = entry_result[entry_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&action, 0, sizeof(rtk_acl_action_t));
                init_action_table(phase[phase_index], &action, unit);

                ret = rtk_acl_ruleAction_set(unit, phase[phase_index], entry[entry_index], &action);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleAction_set (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase[phase_index], entry[entry_index]);
                    osal_memset(&result_action, 0, sizeof(rtk_acl_action_t));
                    ret = rtk_acl_ruleAction_get(unit, phase[phase_index], entry[entry_index], &result_action);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleAction_get (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase[phase_index], entry[entry_index]);

                    if (ACL_PHASE_IGR_ACL == phase[phase_index])
                    {
                        if (osal_memcmp(&result_action.igr_acl, &action.igr_acl, sizeof(rtk_acl_igrAction_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_acl_ruleAction_set/get value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase[phase_index], entry[entry_index]);
                            return RT_ERR_FAILED;
                        }
                    }
#if defined (CONFIG_SDK_RTL8390)
                    else if ((HWP_8390_50_FAMILY(unit)) && (ACL_PHASE_EGR_ACL == phase[phase_index]))
                    {
                        if (osal_memcmp(&result_action.egr_acl, &action.egr_acl, sizeof(rtk_acl_egrAction_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_acl_ruleAction_set/get value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase[phase_index], entry[entry_index]);
                            return RT_ERR_FAILED;
                        }
                    }
#endif
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleAction_set (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase[phase_index], entry[entry_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_phase_t phase;
        uint32 phase_max;
        rtk_pie_id_t  entry;
        uint32  entry_max;

        rtk_acl_action_t  action;
        rtk_acl_action_t  result_action;

        G_VALUE(TEST_MAX_PHASE_VALUE(unit), phase_max);
        G_VALUE(TEST_ENTRY_ID_MAX(unit), entry_max);

        /* enabled all block power before testing */
        for (blockId = 0; blockId <= TEST_BLOCK_ID_MAX(unit); blockId++)
        {
            rtk_acl_blockPwrEnable_set(unit, blockId, ENABLED);
        }

        for (L_VALUE(TEST_MIN_PHASE_VALUE, phase); phase <= phase_max; phase++)
        {
            ASSIGN_EXPECT_RESULT(phase, TEST_MIN_PHASE_VALUE, TEST_MAX_PHASE_VALUE(unit), inter_result2);
            if (ACL_PHASE_IGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, HAL_MAX_NUM_OF_PIE_BLOCK(unit));
            }
            else if (ACL_PHASE_EGR_ACL == phase)
            {
                rtk_acl_partition_set(unit, 0);
            }

            for (L_VALUE(TEST_ENTRY_ID_MIN, entry); entry <= entry_max; entry++)
            {
#if defined(CONFIG_SDK_FPGA_PLATFORM)
                /* The FPGA board ACL only exist 7 block, per-block 4 entries */
                if ((HWP_8390_50_FAMILY(unit)) && (entry%128) >= 4 || (entry/128) >= 7)
                    continue;
#endif
                ASSIGN_EXPECT_RESULT(entry, TEST_ENTRY_ID_MIN, TEST_ENTRY_ID_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&action, 0, sizeof(rtk_acl_action_t));
                init_action_table(phase, &action, unit);

                ret = rtk_acl_ruleAction_set(unit, phase, entry, &action);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleAction_set (unit %u, phase %u, entry %u)"
                                    , ret, expect_result, unit, phase, entry);
                    osal_memset(&result_action, 0, sizeof(rtk_acl_action_t));
                    ret = rtk_acl_ruleAction_get(unit, phase, entry, &result_action);

                    RT_TEST_IS_EQUAL_INT("rtk_acl_ruleAction_get (unit %u, phase %u, entry %u)"
                                     , ret, expect_result, unit, phase, entry);

                    if (ACL_PHASE_IGR_ACL == phase)
                    {
                        if (osal_memcmp(&result_action.igr_acl, &action.igr_acl, sizeof(rtk_acl_igrAction_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_acl_ruleAction_set/get value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase, entry);
                            return RT_ERR_FAILED;
                        }
                    }
#if defined (CONFIG_SDK_RTL8390)

                    else if((HWP_8390_50_FAMILY(unit)) && (ACL_PHASE_EGR_ACL == phase))
                    {
                        if (osal_memcmp(&result_action.egr_acl, &action.egr_acl, sizeof(rtk_acl_egrAction_t)) != 0)
                        {
                            osal_printf("\t%s(%u): rtk_acl_ruleAction_set/get value unequal (unit %u, phase %u, entry %u)\n", __FUNCTION__, __LINE__, unit, phase, entry);
                            return RT_ERR_FAILED;
                        }
                    }
#endif
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_ruleAction_set (unit %u, phase %u, entry %u)"
                                        , ret, RT_ERR_OK, unit, phase, entry);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_templateSelector_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_acl_templateIdx_t  templateIdx_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int32 expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_templateSelector_get(unit, 0, &templateIdx_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_acl_templateSelector_set(unit, 0, templateIdx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blockId[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_result[UNITTEST_MAX_TEST_VALUE];
        int32  blockId_index;
        int32  blockId_last;

        uint32 templateIdx0[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx0_result[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx0_index;
        int32  templateIdx0_last;

        uint32 templateIdx1[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx1_result[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx1_index;
        int32  templateIdx1_last;

        uint32 templateIdx2[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx2_result[UNITTEST_MAX_TEST_VALUE];
        int32  templateIdx2_index;
        int32  templateIdx2_last;

        rtk_acl_templateIdx_t  templateIdx;
        rtk_acl_templateIdx_t  result_templateIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_BLOCK_ID_MAX(unit), TEST_BLOCK_ID_MIN, blockId, blockId_result, blockId_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TEMPLATE_ID_MAX(unit), TEST_TEMPLATE_ID_MIN, templateIdx0, templateIdx0_result, templateIdx0_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TEMPLATE_ID_MAX(unit), TEST_TEMPLATE_ID_MIN, templateIdx1, templateIdx1_result, templateIdx1_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_TEMPLATE_ID_MAX(unit), TEST_TEMPLATE_ID_MIN, templateIdx2, templateIdx2_result, templateIdx2_last);

        for (blockId_index = 0; blockId_index <= blockId_last; blockId_index++)
        {
            inter_result2 = blockId_result[blockId_index];

            for (templateIdx0_index = 0; templateIdx0_index <= templateIdx0_last; templateIdx0_index++)
            {
                inter_result3 = templateIdx0_result[templateIdx0_index];

                for (templateIdx1_index = 0; templateIdx1_index <= templateIdx1_last; templateIdx1_index++)
                {
                    inter_result4 = templateIdx1_result[templateIdx1_index];

                    for (templateIdx2_index = 0; templateIdx2_index <= templateIdx2_last; templateIdx2_index++)
                    {
                        inter_result5 = templateIdx2_result[templateIdx2_index];
                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                        {
                            if(HWP_8390_50_FAMILY(unit))
                                expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                            else if(HWP_8380_30_FAMILY(unit))
                                expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;
                        }
                        templateIdx.template_id[0] = templateIdx0[templateIdx0_index];
                        templateIdx.template_id[1] = templateIdx1[templateIdx1_index];
                        templateIdx.template_id[2] = 0;
                        if(HWP_8380_30_FAMILY(unit))
                            templateIdx.template_id[2] = templateIdx2[templateIdx2_index];
                        ret = rtk_acl_templateSelector_set(unit, blockId[blockId_index], templateIdx);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set (unit %u, blockId %u)"
                                            , ret, expect_result, unit, blockId[blockId_index]);

                            ret = rtk_acl_templateSelector_get(unit, blockId[blockId_index], &result_templateIdx);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                             , ret, expect_result, unit, blockId[blockId_index]);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[0]=%u)"
                                             , result_templateIdx.template_id[0], templateIdx.template_id[0], unit, blockId[blockId_index], templateIdx.template_id[0]);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[1]=%u)"
                                             , result_templateIdx.template_id[1], templateIdx.template_id[1], unit, blockId[blockId_index], templateIdx.template_id[1]);
                            if(HWP_8380_30_FAMILY(unit))
                                RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[2]=%u)"
                                                 , result_templateIdx.template_id[2], templateIdx.template_id[2], unit, blockId[blockId_index], templateIdx.template_id[2]);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_templateSelector_set (unit %u, blockId %u)"
                                                , ret, RT_ERR_OK, unit, blockId[blockId_index]);
                        }
                    }
                }
            }
            ret = rtk_acl_templateSelector_get(unit, blockId[blockId_index], &result_templateIdx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                , ret, expect_result, unit, blockId[blockId_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                    , ret, RT_ERR_OK, unit, blockId[blockId_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 blockId;
        uint32 blockId_max;
        uint32 templateIdx0;
        uint32 templateIdx0_max;
        uint32 templateIdx1;
        uint32 templateIdx1_max;
        uint32 templateIdx2;
        uint32 templateIdx2_max;

        rtk_acl_templateIdx_t  templateIdx;
        rtk_acl_templateIdx_t  result_templateIdx;

        G_VALUE(TEST_BLOCK_ID_MAX(unit), blockId_max);
        G_VALUE(TEST_TEMPLATE_ID_MAX(unit), templateIdx0_max);
        G_VALUE(TEST_TEMPLATE_ID_MAX(unit), templateIdx1_max);
        G_VALUE(TEST_TEMPLATE_ID_MAX(unit), templateIdx2_max);

        for (L_VALUE(TEST_BLOCK_ID_MIN, blockId); blockId <= blockId_max; blockId++)
        {
            ASSIGN_EXPECT_RESULT(blockId, TEST_BLOCK_ID_MIN, TEST_BLOCK_ID_MAX(unit), inter_result2);

            for (L_VALUE(TEST_TEMPLATE_ID_MIN, templateIdx0); templateIdx0 <= templateIdx0_max; templateIdx0++)
            {
                ASSIGN_EXPECT_RESULT(templateIdx0, TEST_TEMPLATE_ID_MIN, TEST_TEMPLATE_ID_MAX(unit), inter_result3);

                for (L_VALUE(TEST_TEMPLATE_ID_MIN, templateIdx1); templateIdx1 <= templateIdx1_max; templateIdx1++)
                {
                    ASSIGN_EXPECT_RESULT(templateIdx1, TEST_TEMPLATE_ID_MIN, TEST_TEMPLATE_ID_MAX(unit), inter_result4);
                    for (L_VALUE(TEST_TEMPLATE_ID_MIN, templateIdx2); templateIdx2 <= templateIdx2_max; templateIdx2++)
                    {
                        ASSIGN_EXPECT_RESULT(templateIdx2, TEST_TEMPLATE_ID_MIN, TEST_TEMPLATE_ID_MAX(unit), inter_result5);

                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                        {
                            if(HWP_8390_50_FAMILY(unit))
                                expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                            else if(HWP_8380_30_FAMILY(unit))
                                expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;
                        }
                        templateIdx.template_id[0] = templateIdx0;
                        templateIdx.template_id[1] = templateIdx1;
                        templateIdx.template_id[2] = 0;

                        if(HWP_8380_30_FAMILY(unit))
                            templateIdx.template_id[2] = templateIdx2;
                        ret = rtk_acl_templateSelector_set(unit, blockId, templateIdx);
                        if (RT_ERR_OK == expect_result)
                        {
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set (unit %u, blockId %u)"
                                            , ret, expect_result, unit, blockId);

                            ret = rtk_acl_templateSelector_get(unit, blockId, &result_templateIdx);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                             , ret, expect_result, unit, blockId);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[0]=%u)"
                                             , result_templateIdx.template_id[0], templateIdx.template_id[0], unit, blockId, templateIdx.template_id[0]);
                            RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[1]=%u)"
                                             , result_templateIdx.template_id[1], templateIdx.template_id[1], unit, blockId, templateIdx.template_id[1]);
                            if(HWP_8380_30_FAMILY(unit))
                                RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_set/get value unequal (unit %u, blockId %u, templateIdx.template_id[2]=%u)"
                                                 , result_templateIdx.template_id[2], templateIdx.template_id[2], unit, blockId, templateIdx.template_id[2]);
                        }
                        else
                        {
                            RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_templateSelector_set (unit %u, blockId %u)"
                                                , ret, RT_ERR_OK, unit, blockId);
                        }
                    }
                }
            }
            ret = rtk_acl_templateSelector_get(unit, blockId, &result_templateIdx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                , ret, expect_result, unit, blockId);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_acl_templateSelector_get (unit %u, blockId %u)"
                                    , ret, RT_ERR_OK, unit, blockId);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_acl_userTemplate_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    rtk_pie_template_t template, template_data;

    template_data.field[0] = TMPLTE_FIELD_DMAC0; /*5*/
    template_data.field[1] = TMPLTE_FIELD_DMAC1; /*6*/
    template_data.field[2] = TMPLTE_FIELD_DMAC2; /*7*/
    template_data.field[3] = TMPLTE_FIELD_SMAC0; /*8*/
    template_data.field[4] = TMPLTE_FIELD_SMAC1; /*9*/
    template_data.field[5] = TMPLTE_FIELD_SMAC2; /*10*/
    template_data.field[6] = TMPLTE_FIELD_ETHERTYPE; /*11*/
    template_data.field[7] = TMPLTE_FIELD_IP_TOS_PROTO;/*31*/
    template_data.field[8] = TMPLTE_FIELD_SIP0;
    template_data.field[9] = TMPLTE_FIELD_SIP1;
    template_data.field[10] = TMPLTE_FIELD_DIP0;
    template_data.field[11] = TMPLTE_FIELD_DIP1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_template_set(unit, 0, &template))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_pie_template_get(unit, 0, &template)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t template_idx[UNITTEST_MAX_TEST_VALUE];
        int32  template_idx_result[UNITTEST_MAX_TEST_VALUE];
        int32  template_idx_index;
        int32  template_idx_last;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_USER_TEMPLATE_ID_MAX(unit), TEST_USER_TEMPLATE_ID_MIN(unit), template_idx, template_idx_result, template_idx_last);
        for (template_idx_index = 0; template_idx_index <= template_idx_last; template_idx_index++)
        {
            inter_result2 = template_idx_result[template_idx_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_template_set(unit, template_idx[template_idx_index], &template_data);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_template_set (unit %u, template_idx %u)"
                                , ret, expect_result, unit, template_idx[template_idx_index]);

                ret = rtk_pie_template_get(unit, template_idx[template_idx_index], &template);
                RT_TEST_IS_EQUAL_INT("rtk_pie_template_get (unit %u, template_idx %u)"
                                 , ret, expect_result, unit, template_idx[template_idx_index]);
                if (osal_memcmp(&template.field, &template_data.field, sizeof(rtk_pie_templateFieldType_t)) != 0)
                {
                    uint32  i;

                    osal_printf("\t%s(%u): rtk_pie_template_set/get value unequal (unit %u, template_idx %u)\n", __FUNCTION__, __LINE__, unit, template_idx[template_idx_index]);
                    for (i=0; i<RTK_MAX_NUM_OF_ACL_TEMPLATE_FIELD; i++)
                        osal_printf("\t%s(%u): (template_data.field[%d]=%u, template.field[%d]=%u)\n", __FUNCTION__, __LINE__, i, template_data.field[i], i, template.field[i]);
                    return RT_ERR_FAILED;
                }

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_template_set (unit %u, template_idx %u)"
                                    , ret, RT_ERR_OK, unit, template_idx[template_idx_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_acl_id_t template_idx;
        uint32 template_idx_max;

        G_VALUE(TEST_USER_TEMPLATE_ID_MAX(unit), template_idx_max);

        for (L_VALUE(TEST_USER_TEMPLATE_ID_MIN(unit), template_idx); template_idx <= template_idx_max; template_idx++)
        {
            ASSIGN_EXPECT_RESULT(template_idx, TEST_USER_TEMPLATE_ID_MIN(unit), TEST_USER_TEMPLATE_ID_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_pie_template_set(unit, template_idx, &template_data);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_pie_template_set (unit %u, template_idx %u)"
                                , ret, expect_result, unit, template_idx);
                ret = rtk_pie_template_get(unit, template_idx, &template);
                RT_TEST_IS_EQUAL_INT("rtk_pie_template_get (unit %u, template_idx %u)"
                                 , ret, expect_result, unit, template_idx);
                if (osal_memcmp(&template.field, &template_data.field, sizeof(rtk_pie_templateFieldType_t)) != 0)
                {
                    osal_printf("\t%s(%u): rtk_pie_template_set/get value unequal (unit %u, template_idx %u)\n", __FUNCTION__, __LINE__, unit, template_idx);
                    return RT_ERR_FAILED;
                }
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_pie_template_set (unit %u, template_idx %u)"
                                    , ret, RT_ERR_OK, unit, template_idx);
        }
    }

    return RT_ERR_OK;
}
static int32 init_action_table(uint32 phase, rtk_acl_action_t *action_table, uint32 unit)
{
    if (NULL == action_table)
    {
        return RT_ERR_FAILED;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        /* ingress acl action */
        action_table->igr_acl.fwd_en = ENABLED;
        action_table->igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID;
#if defined (CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
            action_table->igr_acl.fwd_data.fwd_info = 5;
#endif
#if defined (CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
            action_table->igr_acl.fwd_data.info.copy_redirect_port.fwd_port_id = 5;
#endif
        action_table->igr_acl.stat_en = DISABLED;
        action_table->igr_acl.mirror_en = DISABLED;
        action_table->igr_acl.meter_en = DISABLED;
        action_table->igr_acl.inner_vlan_assign_en = ENABLED;
        action_table->igr_acl.inner_vlan_data.vid_assign_type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
        action_table->igr_acl.inner_vlan_data.vid_value = 10;
        action_table->igr_acl.outer_vlan_assign_en = DISABLED;
        action_table->igr_acl.pri_en = ENABLED;
        action_table->igr_acl.pri_data.pri = 3;
    }
#if defined (CONFIG_SDK_RTL8390)
    if ((HWP_8390_50_FAMILY(unit)) && ACL_PHASE_EGR_ACL == phase)
    {
        /* egress acl action */
        action_table->egr_acl.rmk_en = ENABLED;
        action_table->egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_INNER_USER_PRI;
        action_table->egr_acl.rmk_data.rmk_info = 7;
    }
#endif
    return RT_ERR_OK;
}/* end of init_action_table */
