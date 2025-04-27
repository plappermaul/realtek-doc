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
#include <rtk/qos.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/* Priority Selection Group ID */
#define TEST_PRISEL_GID_MIN     0
#define TEST_PRISEL_GID_MAX(unit)     (HAL_PRI_SEL_GROUP_INDEX_MAX(unit))
/* Priority ID */
#define TEST_PRI_MIN            0
#define TEST_PRI_MAX(unit)            (HAL_INTERNAL_PRIORITY_MAX(unit))
/* Drop-Precedence Source Selection */
#define TEST_DPSRCSEL_MIN       DP_SRC_DEI_BASED
#define TEST_DPSRCSEL_MAX       DP_SRC_DSCP_BASED
/* DEI Source Selection */
#define TEST_DEISRCSEL_MIN      DEI_SEL_INNER_TAG
#define TEST_DEISRCSEL_MAX      DEI_SEL_OUTER_TAG
/* DEI */
#define TEST_DEI_MIN            0
#define TEST_DEI_MAX            1
/* Drop-Precedence */
#define TEST_DP_MIN             0
#define TEST_DP_MAX             RTK_DROP_PRECEDENCE_MAX
/* DSCP */
#define TEST_DSCP_MIN           0
#define TEST_DSCP_MAX           63
/* Dot1p Remark Source */
#define TEST_DOT1PRMKSRC_MIN    DOT_1P_RMK_SRC_INT_PRI
#define TEST_DOT1PRMKSRC_MAX(unit)    (HWP_8380_30_FAMILY(unit) ? DOT_1P_RMK_SRC_USER_PRI:(DOT_1P_RMK_SRC_END - 1))
/* Outer 1p Remark Source */
#define TEST_OUT1PRMKSRC_MIN    OUTER_1P_RMK_SRC_INT_PRI
#define TEST_OUT1PRMKSRC_MAX    (OUTER_1P_RMK_SRC_END - 1)
/* DSCP Remark Source */
#define TEST_DSCPRMKSRC_MIN     DSCP_RMK_SRC_INT_PRI
#define TEST_DSCPRMKSRC_MAX     (DSCP_RMK_SRC_END - 1)
/* Queue ID */
#define TEST_QUEUE_ID_MIN(unit) (HAL_MIN_NUM_OF_QUEUE(unit)-1)
#define TEST_QUEUE_ID_MAX(unit) (HAL_MAX_NUM_OF_QUEUE(unit)-1)
/* AVB SR Class */
#define TEST_AVB_SR_CLASS_MIN   (AVB_SR_CLASS_A)
#define TEST_AVB_SR_CLASS_MAX   (AVB_SR_CLASS_END - 1)
/* Queue Number */
#define TEST_QUEUE_NUM_MIN      1
#define TEST_QUEUE_NUM_MAX(unit)      (HAL_MAX_NUM_OF_QUEUE(unit))
/* Queue Weight */
#define TEST_QUEUE_WEIGHT_MIN   0
#define TEST_QUEUE_WEIGHT_MAX(unit)   (HAL_QUEUE_WEIGHT_MAX(unit))
/* DEI Remark Tag Selection */
#define TEST_DEIREMARKTAGSEL_MIN    DEI_SEL_INNER_TAG /* 0 */
#define TEST_DEIREMARKTAGSEL_MAX    (DEI_SEL_END - 1)
/* Outer 1p Default Priority Source */
#define TEST_OUT1PDFLTPRISRC_MIN    OUTER_1P_DFLT_SRC_INT_PRI
#define TEST_OUT1PDFLTPRISRC_MAX(unit)    (HWP_8390_50_FAMILY(unit)?OUTER_1P_DFLT_SRC_USER_PRI:(OUTER_1P_DFLT_SRC_END - 1))
/* Flow Control Threshold */
#define TEST_FLOWCTRL_THRESH_MIN    0
#define TEST_FLOWCTRL_THRESH_MAX(unit)    (HAL_FLOWCTRL_THRESH_MAX(unit))
/* Scheduling Type */
#define TEST_SCHEDULING_TYPE_MIN    0
#define TEST_SCHEDULING_TYPE_MAX    (SCHEDULING_TYPE_END - 1)
/* Congestion Avoid Algorithm */
#define TEST_CONG_AVOID_ALGO_MIN    CONG_AVOID_SRED /* 0 */
#define TEST_CONG_AVOID_ALGO_MAX    (CONG_AVOID_END - 1)
/* WRED Drop Probability */
#define TEST_WRED_DROP_PROBABILITY_MIN    0
#define TEST_WRED_DROP_PROBABILITY_MAX(unit)    (HAL_WRED_DROP_PROBABILITY_MAX(unit))

/* 1p Default Priority Source */
#define TEST_1PDFLTPRISRC_MIN    INNER_1P_DFLT_SRC_INT_PRI
#define TEST_1PDFLTPRISRC_MAX    (INNER_1P_DFLT_SRC_END - 1)
#define CONGAVOID_ALGO_NUM              CONG_AVOID_END

int32 dal_qos_portPriSelGroup_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  priSelGrp_idx;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portPriSelGroup_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portPriSelGroup_get(unit, 0, &priSelGrp_idx)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  priSelGrp[UNITTEST_MAX_TEST_VALUE];
        int32  priSelGrp_result[UNITTEST_MAX_TEST_VALUE];
        int32  priSelGrp_index;
        int32  priSelGrp_last;

        int32  result_priSelGrp;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_SEL_GROUP_INDEX_MAX(unit), 0, priSelGrp, priSelGrp_result, priSelGrp_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (priSelGrp_index = 0; priSelGrp_index <= priSelGrp_last; priSelGrp_index++)
            {
                inter_result3 = priSelGrp_result[priSelGrp_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portPriSelGroup_set(unit, port[port_index], priSelGrp[priSelGrp_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_set (unit %u, port %u, priSelGrp %u)"
                                    , ret, expect_result, unit, port[port_index], priSelGrp[priSelGrp_index]);

                    ret = rtk_qos_portPriSelGroup_get(unit, port[port_index], (uint32*)&result_priSelGrp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_get (unit %u, port %u, priSelGrp %u)"
                                     , ret, expect_result, unit, port[port_index], priSelGrp[priSelGrp_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_set/get value unequal (unit %u, port %u, priSelGrp %u)"
                                     , result_priSelGrp, priSelGrp[priSelGrp_index], unit, port[port_index], priSelGrp[priSelGrp_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portpriSelGrp_set (unit %u, port %u, priSelGrp %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], priSelGrp[priSelGrp_index]);
                }
            }

            ret = rtk_qos_portPriSelGroup_get(unit, port[port_index], (uint32*)&result_priSelGrp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portPriSelGroup_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  priSel_gid;
        uint32  priSel_gid_max;
        uint32  result_priSel_gid;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PRISEL_GID_MAX(unit), priSel_gid_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_PRISEL_GID_MIN, priSel_gid); priSel_gid <= priSel_gid_max; priSel_gid++)
            {
                ASSIGN_EXPECT_RESULT(priSel_gid, TEST_PRISEL_GID_MIN, TEST_PRISEL_GID_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portPriSelGroup_set(unit, port, priSel_gid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_set (unit %u, port %u, priSel_gid %u)"
                                    , ret, expect_result, unit, port, priSel_gid);
                    ret = rtk_qos_portPriSelGroup_get(unit,  port, &result_priSel_gid);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_get (unit %u, port %u, priSel_gid %u)"
                                     , ret, expect_result, unit, port, priSel_gid);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPriSelGroup_set/get value unequal (unit %u, port %u, priSel_gid %u)"
                                     , result_priSel_gid, priSel_gid, unit, port, priSel_gid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portPriSelGroup_set (unit %u, port %u, priSel_gid %u)"
                                        , ret, RT_ERR_OK, unit, port, priSel_gid);
            }
        }
    }

    return RT_ERR_OK;
}


int32 dal_qos_portPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  pri_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portPri_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portPri_get(unit, 0, &pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;

        int32  result_pri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_INTERNAL_PRIORITY_MAX(unit), 0, pri, pri_result, pri_last);


        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (pri_index = 0; pri_index <= pri_last; pri_index++)
            {
                inter_result3 = pri_result[pri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portPri_set(unit, port[port_index], pri[pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_set (unit %u, port %u, pri %u)"
                                    , ret, expect_result, unit, port[port_index], pri[pri_index]);

                    ret = rtk_qos_portPri_get(unit, port[port_index], (uint32*)&result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_get (unit %u, port %u, pri %u)"
                                     , ret, expect_result, unit, port[port_index], pri[pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_set/get value unequal (unit %u, port %u, pri %u)"
                                     , result_pri, pri[pri_index], unit, port[port_index], pri[pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portPri_set (unit %u, port %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pri[pri_index]);
                }
            }

            ret = rtk_qos_portPri_get(unit, port[port_index], (uint32*)&result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portPri_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_pri_t   pri;
        rtk_pri_t   pri_max;
        rtk_pri_t   result_pri;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PRI_MAX(unit), pri_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_PRI_MIN, pri); pri <= pri_max; pri++)
            {
                ASSIGN_EXPECT_RESULT(pri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portPri_set(unit, port, pri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_set (unit %u, port %u, pri %u)"
                                    , ret, expect_result, unit, port, pri);
                    ret = rtk_qos_portPri_get(unit,  port, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_get (unit %u, port %u, pri %u)"
                                     , ret, expect_result, unit, port, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portPri_set/get value unequal (unit %u, port %u, pri %u)"
                                     , result_pri, pri, unit, port, pri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portPri_set (unit %u, port %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, port, pri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_portInnerPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  innerPri_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portInnerPri_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portInnerPri_get(unit, 0, &innerPri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_t  innerPri[UNITTEST_MAX_TEST_VALUE];
        int32  innerPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  innerPri_index;
        int32  innerPri_last;

        int32  result_innerPri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(RTK_DOT1P_PRIORITY_MAX, 0, innerPri, innerPri_result, innerPri_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (innerPri_index = 0; innerPri_index <= innerPri_last; innerPri_index++)
            {
                inter_result3 = innerPri_result[innerPri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portInnerPri_set(unit, port[port_index], innerPri[innerPri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portInnerPri_set (unit %u, port %u, innerPri %u)"
                                    , ret, expect_result, unit, port[port_index], innerPri[innerPri_index]);

                    ret = rtk_qos_portInnerPri_get(unit, port[port_index], (uint32*)&result_innerPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portInnerPri_get (unit %u, port %u, innerPri %u)"
                                     , ret, expect_result, unit, port[port_index], innerPri[innerPri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portInnerPri_set/get value unequal (unit %u, port %u, innerPri %u)"
                                     , result_innerPri, innerPri[innerPri_index], unit, port[port_index], innerPri[innerPri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portInnerPri_set (unit %u, port %u, innerPri %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], innerPri[innerPri_index]);
                }
            }

            ret = rtk_qos_portInnerPri_get(unit, port[port_index], (uint32*)&result_innerPri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portInnerPri_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portInnerPri_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }


    return RT_ERR_OK;
}

int32 dal_qos_portOuterPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  outerPri_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuterPri_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuterPri_get(unit, 0, &outerPri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_pri_t  outerPri[UNITTEST_MAX_TEST_VALUE];
        int32  outerPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  outerPri_index;
        int32  outerPri_last;

        int32  result_outerPri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(RTK_DOT1P_PRIORITY_MAX, 0, outerPri, outerPri_result, outerPri_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (outerPri_index = 0; outerPri_index <= outerPri_last; outerPri_index++)
            {
                inter_result3 = outerPri_result[outerPri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOuterPri_set(unit, port[port_index], outerPri[outerPri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuterPri_set (unit %u, port %u, outerPri %u)"
                                    , ret, expect_result, unit, port[port_index], outerPri[outerPri_index]);

                    ret = rtk_qos_portOuterPri_get(unit, port[port_index], (uint32*)&result_outerPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuterPri_get (unit %u, port %u, outerPri %u)"
                                     , ret, expect_result, unit, port[port_index], outerPri[outerPri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuterPri_set/get value unequal (unit %u, port %u, outerPri %u)"
                                     , result_outerPri, outerPri[outerPri_index], unit, port[port_index], outerPri[outerPri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuterPri_set (unit %u, port %u, outerPri %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], outerPri[outerPri_index]);
                }
            }

            ret = rtk_qos_portOuterPri_get(unit, port[port_index], (uint32*)&result_outerPri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portOuterPri_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuterPri_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }


    return RT_ERR_OK;
}

int32 dal_qos_dscpPriRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    rtk_pri_t  result_int_pri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpPriRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpPriRemap_get(unit, 0, &result_int_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  dscp[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_index;
        int32  dscp_last;

        rtk_pri_t  int_pri[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_index;
        int32  int_pri_last;

        int32  result_int_pri;


        UNITTEST_TEST_VALUE_ASSIGN(RTK_VALUE_OF_DSCP_MAX, 0, dscp, dscp_result, dscp_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_INTERNAL_PRIORITY_MAX(unit), 0, int_pri, int_pri_result, int_pri_last);

        for (dscp_index = 0; dscp_index <= dscp_last; dscp_index++)
        {
            for (int_pri_index = 0; int_pri_index <= int_pri_last; int_pri_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (dscp_result[dscp_index] && int_pri_result[int_pri_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscpPriRemap_set(unit, dscp[dscp_index], int_pri[int_pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_set (unit %u, dscp %u, int_pri %u)"
                                    , ret, expect_result, unit, dscp[dscp_index], int_pri[int_pri_index]);

                    ret = rtk_qos_dscpPriRemap_get(unit, dscp[dscp_index], (uint32*)&result_int_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_get (unit %u, dscp %u, int_pri %u)"
                                     , ret, expect_result, unit, dscp[dscp_index], int_pri[int_pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_set/get value unequal (unit %u, dscp %u, int_pri %u)"
                                     , result_int_pri, int_pri[int_pri_index], unit, dscp[dscp_index], int_pri[int_pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpPriRemap_set (unit %u, dscp %u, int_pri %u)"
                                        , ret, RT_ERR_OK, unit, dscp[dscp_index], int_pri[int_pri_index]);
                }
            }

            ret = rtk_qos_dscpPriRemap_get(unit, dscp[dscp_index], (uint32*)&result_int_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (dscp_result[dscp_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_get (unit %u, dscp %u)"
                                , ret, expect_result, unit, dscp[dscp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpPriRemap_get (unit %u, dscp %u)"
                                    , ret, RT_ERR_OK, unit, dscp[dscp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 dscp;
        uint32 dscp_max;
        rtk_pri_t   pri;
        rtk_pri_t   pri_max;
        rtk_pri_t   result_pri;

        G_VALUE(RTK_VALUE_OF_DSCP_MAX, dscp_max);
        G_VALUE(TEST_PRI_MAX(unit), pri_max);

        for (L_VALUE(RTK_VALUE_OF_DSCP_MIN, dscp); dscp <= dscp_max; dscp++)
        {
            ASSIGN_EXPECT_RESULT(dscp, RTK_VALUE_OF_DSCP_MIN, RTK_VALUE_OF_DSCP_MAX, inter_result2);

            for (L_VALUE(TEST_PRI_MIN, pri); pri <= pri_max; pri++)
            {
                ASSIGN_EXPECT_RESULT(pri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscpPriRemap_set(unit, dscp, pri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_set (unit %u, dscp %u, pri %u)"
                                    , ret, expect_result, unit, dscp, pri);
                    ret = rtk_qos_dscpPriRemap_get(unit,  dscp, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_get (unit %u, dscp %u, pri %u)"
                                     , ret, expect_result, unit, dscp, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpPriRemap_set/get value unequal (unit %u, dscp %u, pri %u)"
                                     , result_pri, pri, unit, dscp, pri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpPriRemap_set (unit %u, dscp %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, dscp, pri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_1pPriRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    rtk_pri_t  result_int_pri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pPriRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pPriRemap_get(unit, 0, &result_int_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  dot1p_pri[UNITTEST_MAX_TEST_VALUE];
        uint32  dot1p_pri_result[UNITTEST_MAX_TEST_VALUE];
        uint32  dot1p_pri_index;
        uint32  dot1p_pri_last;

        rtk_pri_t  int_pri[UNITTEST_MAX_TEST_VALUE];
        uint32  int_pri_result[UNITTEST_MAX_TEST_VALUE];
        uint32  int_pri_index;
        uint32  int_pri_last;
        uint32  result_int_pri;



        UNITTEST_TEST_VALUE_ASSIGN(RTK_DOT1P_PRIORITY_MAX, 0, dot1p_pri, dot1p_pri_result, dot1p_pri_last);
        UNITTEST_TEST_VALUE_ASSIGN(HAL_INTERNAL_PRIORITY_MAX(unit), 0, int_pri, int_pri_result, int_pri_last);

        for (dot1p_pri_index = 0; dot1p_pri_index <= dot1p_pri_last; dot1p_pri_index++)
        {
            for (int_pri_index = 0; int_pri_index <= int_pri_last; int_pri_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (dot1p_pri_result[dot1p_pri_index] && int_pri_result[int_pri_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_1pPriRemap_set(unit, dot1p_pri[dot1p_pri_index], int_pri[int_pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_set (unit %u, dot1p_pri %u, int_pri %u)"
                                    , ret, expect_result, unit, dot1p_pri[dot1p_pri_index], int_pri[int_pri_index]);

                    ret = rtk_qos_1pPriRemap_get(unit, dot1p_pri[dot1p_pri_index], (uint32*)&result_int_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_get (unit %u, dot1p_pri %u, int_pri %u)"
                                     , ret, expect_result, unit, dot1p_pri[dot1p_pri_index], int_pri[int_pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_set/get value unequal (unit %u, dot1p_pri %u, int_pri %u)"
                                     , result_int_pri, int_pri[int_pri_index], unit, dot1p_pri[dot1p_pri_index], int_pri[int_pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pPriRemap_set (unit %u, dot1p_pri %u, int_pri %u)"
                                        , ret, RT_ERR_OK, unit, dot1p_pri[dot1p_pri_index], int_pri[int_pri_index]);
                }
            }

            ret = rtk_qos_1pPriRemap_get(unit, dot1p_pri[dot1p_pri_index], &result_int_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (dot1p_pri_result[dot1p_pri_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_get (unit %u, dot1p_pri %u)"
                                , ret, expect_result, unit, dot1p_pri[dot1p_pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pPriRemap_get (unit %u, dot1p_pri %u)"
                                    , ret, RT_ERR_OK, unit, dot1p_pri[dot1p_pri_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t   dot1pPri;
        rtk_pri_t   dot1pPri_max;
        rtk_pri_t   intPri;
        rtk_pri_t   intPri_max;
        rtk_pri_t   result_intPri;


        G_VALUE(TEST_PRI_MAX(unit), dot1pPri_max);
        G_VALUE(TEST_PRI_MAX(unit), intPri_max);

        for (L_VALUE(TEST_PRI_MIN, dot1pPri); dot1pPri <= dot1pPri_max; dot1pPri++)
        {
            ASSIGN_EXPECT_RESULT(dot1pPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
            {
                ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_1pPriRemap_set(unit, dot1pPri, intPri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_set (unit %u, dot1pPri %u, intPri %u)"
                                    , ret, expect_result, unit, dot1pPri, intPri);
                    ret = rtk_qos_1pPriRemap_get(unit,  dot1pPri, &result_intPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_get (unit %u, dot1pPri %u, intPri %u)"
                                     , ret, expect_result, unit, dot1pPri, intPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pPriRemap_set/get value unequal (unit %u, dot1pPri %u, intPri %u)"
                                     , result_intPri, intPri, unit, dot1pPri, intPri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pPriRemap_set (unit %u, dot1pPri %u, intPri %u)"
                                        , ret, RT_ERR_OK, unit, dot1pPri, intPri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_queueNum_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32 result_queueNum;
    uint8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_queueNum_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_queueNum_get(unit, &result_queueNum)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  queueNum[UNITTEST_MAX_TEST_VALUE];
        int32  queueNum_result[UNITTEST_MAX_TEST_VALUE];
        int32  queueNum_index;
        int32  queueNum_last;


        UNITTEST_TEST_VALUE_ASSIGN(HAL_MAX_NUM_OF_QUEUE(unit), HAL_MIN_NUM_OF_QUEUE(unit), queueNum, queueNum_result, queueNum_last);

        for (queueNum_index = 0; queueNum_index <= queueNum_last; queueNum_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (queueNum_result[queueNum_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_queueNum_set(unit, queueNum[queueNum_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_set (unit %u, queueNum %u)"
                                , ret, expect_result, unit, queueNum[queueNum_index]);

                ret = rtk_qos_queueNum_get(unit, (uint32*)&result_queueNum);
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_get (unit %u, queueNum %u)"
                                 , ret, expect_result, unit, queueNum[queueNum_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_set/get value unequal (unit %u, queueNum %u)"
                                 , result_queueNum, queueNum[queueNum_index], unit, queueNum[queueNum_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_queueNum_set (unit %u, queueNum %u)"
                                    , ret, RT_ERR_OK, unit, queueNum[queueNum_index]);
            }
        }

        ret = rtk_qos_queueNum_get(unit, (uint32*)&result_queueNum);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_queueNum_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 queueNum;
        uint32 queueNum_max;
        uint32 result_queueNum;

        G_VALUE(HAL_MAX_NUM_OF_QUEUE(unit), queueNum_max);

        for (L_VALUE(HAL_MIN_NUM_OF_QUEUE(unit), queueNum); queueNum <= queueNum_max; queueNum++)
        {
            ASSIGN_EXPECT_RESULT(queueNum, HAL_MIN_NUM_OF_QUEUE(unit), HAL_MAX_NUM_OF_QUEUE(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_queueNum_set(unit, queueNum);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_set (unit %u, queueNum %u)"
                                , ret, expect_result, unit, queueNum);
                ret = rtk_qos_queueNum_get(unit, &result_queueNum);
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_get (unit %u, queueNum %u)"
                                 , ret, expect_result, unit, queueNum);
                RT_TEST_IS_EQUAL_INT("rtk_qos_queueNum_set/get value unequal (unit %u, queueNum %u)"
                                 , result_queueNum, queueNum, unit, queueNum);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_queueNum_set (unit %u, queueNum %u)"
                                    , ret, RT_ERR_OK, unit, queueNum);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_1pRemarkEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_port1pRemarkEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_port1pRemarkEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
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
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_port1pRemarkEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_qos_port1pRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_port1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_qos_port1pRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_port1pRemarkEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        int32  enable_max;

        rtk_enable_t  result_enable;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_port1pRemarkEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_qos_port1pRemarkEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_port1pRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_port1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_1pRemark_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    rtk_pri_t  result_dot1p_pri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pRemark_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pRemark_get(unit, 0, &result_dot1p_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  int_pri[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_index;
        int32  int_pri_last;

        rtk_pri_t  dot1p_pri[UNITTEST_MAX_TEST_VALUE];
        int32  dot1p_pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  dot1p_pri_index;
        int32  dot1p_pri_last;


        UNITTEST_TEST_VALUE_ASSIGN(HAL_INTERNAL_PRIORITY_MAX(unit), 0, int_pri, int_pri_result, int_pri_last);
        UNITTEST_TEST_VALUE_ASSIGN(RTK_DOT1P_PRIORITY_MAX, 0, dot1p_pri, dot1p_pri_result, dot1p_pri_last);


        for (int_pri_index = 0; int_pri_index <= int_pri_last; int_pri_index++)
        {
            for (dot1p_pri_index = 0; dot1p_pri_index <= dot1p_pri_last; dot1p_pri_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (int_pri_result[int_pri_index] && dot1p_pri_result[dot1p_pri_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_1pRemark_set(unit, int_pri[int_pri_index], dot1p_pri[dot1p_pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_set (unit %u, int_pri %u, dot1p_pri %u)"
                                    , ret, expect_result, unit, int_pri[int_pri_index], dot1p_pri[dot1p_pri_index]);

                    ret = rtk_qos_1pRemark_get(unit, int_pri[int_pri_index], (uint32*)&result_dot1p_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_get (unit %u, int_pri %u, dot1p_pri %u)"
                                     , ret, expect_result, unit, int_pri[int_pri_index], dot1p_pri[dot1p_pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_set/get value unequal (unit %u, int_pri %u, dot1p_pri %u)"
                                     , result_dot1p_pri, dot1p_pri[dot1p_pri_index], unit, int_pri[int_pri_index], dot1p_pri[dot1p_pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemark_set (unit %u, int_pri %u, dot1p_pri %u)"
                                        , ret, RT_ERR_OK, unit, int_pri[int_pri_index], dot1p_pri[dot1p_pri_index]);
                }
            }

            ret = rtk_qos_1pRemark_get(unit, int_pri[int_pri_index], (uint32*)&result_dot1p_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (int_pri_result[int_pri_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_get (unit %u, int_pri %u)"
                                , ret, expect_result, unit, int_pri[int_pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemark_get (unit %u, int_pri %u)"
                                    , ret, RT_ERR_OK, unit, int_pri[int_pri_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  intPri;
        uint32  intPri_max;
        uint32  dot1pPri;
        uint32  dot1pPri_max;
        uint32  result_dot1pPri;


        G_VALUE(TEST_PRI_MAX(unit), intPri_max);
        G_VALUE(TEST_PRI_MAX(unit), dot1pPri_max);

        for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
        {
            ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PRI_MIN, dot1pPri); dot1pPri <= dot1pPri_max; dot1pPri++)
            {
                ASSIGN_EXPECT_RESULT(dot1pPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_1pRemark_set(unit, intPri, dot1pPri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_set (unit %u, intPri %u, dot1pPri %u)"
                                    , ret, expect_result, unit, intPri, dot1pPri);
                    ret = rtk_qos_1pRemark_get(unit,  intPri, &result_dot1pPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_get (unit %u, intPri %u, dot1pPri %u)"
                                     , ret, expect_result, unit, intPri, dot1pPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemark_set/get value unequal (unit %u, intPri %u, dot1pPri %u)"
                                     , result_dot1pPri, dot1pPri, unit, intPri, dot1pPri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemark_set (unit %u, intPri %u, dot1pPri %u)"
                                        , ret, RT_ERR_OK, unit, intPri, dot1pPri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_out1pRemarkEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOut1pRemarkEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOut1pRemarkEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
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
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);
        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOut1pRemarkEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_qos_portOut1pRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_qos_portOut1pRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }

    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        int32  enable_max;

        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);

                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOut1pRemarkEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_qos_portOut1pRemarkEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dscpRemarkEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDscpRemarkEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDscpRemarkEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
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
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDscpRemarkEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_qos_portDscpRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_qos_portDscpRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDscpRemarkEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        int32  enable_max;

        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDscpRemarkEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_qos_portDscpRemarkEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDscpRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dscpRemark_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  result_dscp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpRemark_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpRemark_get(unit, 0, &result_dscp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_pri_t  int_pri[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  int_pri_index;
        int32  int_pri_last;

        uint32  dscp[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_index;
        int32  dscp_last;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_INTERNAL_PRIORITY_MAX(unit), 0, int_pri, int_pri_result, int_pri_last);
        UNITTEST_TEST_VALUE_ASSIGN(RTK_VALUE_OF_DSCP_MAX, 0, dscp, dscp_result, dscp_last);

        for (int_pri_index = 0; int_pri_index <= int_pri_last; int_pri_index++)
        {
            for (dscp_index = 0; dscp_index <= dscp_last; dscp_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (int_pri_result[int_pri_index] && dscp_result[dscp_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscpRemark_set(unit, int_pri[int_pri_index], dscp[dscp_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemark_set (unit %u, int_pri %u, dscp %u)"
                                    , ret, expect_result, unit, int_pri[int_pri_index], dscp[dscp_index]);

                    ret = rtk_qos_dscpRemark_get(unit, int_pri[int_pri_index], (uint32*)&result_dscp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemark_get (unit %u, int_pri %u, dscp %u)"
                                     , ret, expect_result, unit, int_pri[int_pri_index], dscp[dscp_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemark_set/get value unequal (unit %u, int_pri %u, dscp %u)"
                                     , result_dscp, dscp[dscp_index], unit, int_pri[int_pri_index], dscp[dscp_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpRemark_set (unit %u, int_pri %u, dscp %u)"
                                        , ret, RT_ERR_OK, unit, int_pri[int_pri_index], dscp[dscp_index]);
                }
            }

            ret = rtk_qos_dscpRemark_get(unit, int_pri[int_pri_index], (uint32*)&result_dscp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (int_pri_result[int_pri_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemark_get (unit %u, int_pri %u)"
                                , ret, expect_result, unit, int_pri[int_pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpRemark_get (unit %u, int_pri %u)"
                                    , ret, RT_ERR_OK, unit, int_pri[int_pri_index]);
            }

        }
    }


    return RT_ERR_OK;
}

int32 dal_qos_schedulingAlgorithm_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    rtk_qos_scheduling_type_t  scheduling_type_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_schedulingAlgorithm_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_schedulingAlgorithm_get(unit, 0, &scheduling_type_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_scheduling_type_t scheduling_type[UNITTEST_MAX_TEST_VALUE];
        int32  scheduling_type_result[UNITTEST_MAX_TEST_VALUE];
        int32  scheduling_type_index;
        int32  scheduling_type_last;
        int32  result_scheduling_type;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_SCHEDULING_TYPE_MAX, TEST_SCHEDULING_TYPE_MIN, scheduling_type, scheduling_type_result, scheduling_type_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (scheduling_type_index = 0; scheduling_type_index <= scheduling_type_last; scheduling_type_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && scheduling_type_result[scheduling_type_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_schedulingAlgorithm_set(unit, port[port_index], scheduling_type[scheduling_type_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_set (unit %u, port %u, scheduling_type %u)"
                                    , ret, expect_result, unit, port[port_index], scheduling_type[scheduling_type_index]);

                    ret = rtk_qos_schedulingAlgorithm_get(unit, port[port_index], (uint32*)&result_scheduling_type);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_get (unit %u, port %u, scheduling_type %u)"
                                     , ret, expect_result, unit, port[port_index], scheduling_type[scheduling_type_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_set/get value unequal (unit %u, port %u, scheduling_type %u)"
                                     , result_scheduling_type, scheduling_type[scheduling_type_index], unit, port[port_index], scheduling_type[scheduling_type_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingAlgorithm_set (unit %u, port %u, scheduling_type %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], scheduling_type[scheduling_type_index]);
                }
            }

            ret = rtk_qos_schedulingAlgorithm_get(unit, port[port_index], (uint32*)&result_scheduling_type);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingAlgorithm_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_qos_scheduling_type_t  scheduling_type;
        uint32  scheduling_type_max;

        rtk_qos_scheduling_type_t  result_scheduling_type;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_SCHEDULING_TYPE_MAX, scheduling_type_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_SCHEDULING_TYPE_MIN, scheduling_type); scheduling_type <= scheduling_type_max; scheduling_type++)
            {
                ASSIGN_EXPECT_RESULT(scheduling_type, TEST_SCHEDULING_TYPE_MIN, TEST_SCHEDULING_TYPE_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_schedulingAlgorithm_set(unit, port, scheduling_type);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_set (unit %u, port %u, scheduling_type %u)"
                                    , ret, expect_result, unit, port, scheduling_type);
                    ret = rtk_qos_schedulingAlgorithm_get(unit,  port, &result_scheduling_type);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_get (unit %u, port %u, scheduling_type %u)"
                                     , ret, expect_result, unit, port, scheduling_type);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingAlgorithm_set/get value unequal (unit %u, port %u, scheduling_type %u)"
                                     , result_scheduling_type, scheduling_type, unit, port, scheduling_type);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingAlgorithm_set (unit %u, port %u, scheduling_type %u)"
                                        , ret, RT_ERR_OK, unit, port, scheduling_type);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_congAvoidAlgo_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_congAvoidAlgo_t  algo_temp;
    uint8  inter_result2 = 1;
    int32  expect_result = 1;
    static rt_error_common_t congAvoidAlgo[CONGAVOID_ALGO_NUM+1];
    if(HWP_8390_50_FAMILY(unit)|| HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
    {
        congAvoidAlgo[0]=RT_ERR_FAILED;
        congAvoidAlgo[1]=RT_ERR_OK;
        congAvoidAlgo[2]=RT_ERR_OK;
        congAvoidAlgo[3]=RT_ERR_FAILED;
    }
    else if(HWP_8380_30_FAMILY(unit))
    {
        congAvoidAlgo[0]=RT_ERR_OK;
        congAvoidAlgo[1]=RT_ERR_FAILED;
        congAvoidAlgo[2]=RT_ERR_OK;
        congAvoidAlgo[3]=RT_ERR_FAILED;
    }


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidAlgo_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidAlgo_get(unit, &algo_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_qos_congAvoidAlgo_t  algo[UNITTEST_MAX_TEST_VALUE];
        int32  algo_result[UNITTEST_MAX_TEST_VALUE];
        int32  algo_index;
        int32  algo_last;
        int32  result_algo;

        UNITTEST_TEST_VALUE_ASSIGN((CONG_AVOID_END - 1), 0, algo, algo_result, algo_last);

        for (algo_index = 0; algo_index <= algo_last; algo_index++)
        {
            if((RT_ERR_OK == congAvoidAlgo[algo[algo_index]]) && (algo_result[algo_index] == 1))
                algo_result[algo_index] = 1;
            else
                algo_result[algo_index] = 0;
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (algo_result[algo_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_congAvoidAlgo_set(unit, algo[algo_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_set (unit %u, algo %u)"
                                , ret, expect_result, unit, algo[algo_index]);

                ret = rtk_qos_congAvoidAlgo_get(unit, (uint32*)&result_algo);
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_get (unit %u, algo %u)"
                                 , ret, expect_result, unit, algo[algo_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_set/get value unequal (unit %u, algo %u)"
                                 , result_algo, algo[algo_index], unit, algo[algo_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidAlgo_set (unit %u, algo %u)"
                                    , ret, RT_ERR_OK, unit, algo[algo_index]);
            }
        }

        ret = rtk_qos_congAvoidAlgo_get(unit, (uint32*)&result_algo);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidAlgo_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_qos_congAvoidAlgo_t algo;
        uint32 algo_max;
        rtk_qos_congAvoidAlgo_t result_algo;


        G_VALUE(TEST_CONG_AVOID_ALGO_MAX, algo_max);

        for (L_VALUE(TEST_CONG_AVOID_ALGO_MIN, algo); algo <= algo_max; algo++)
        {
            ASSIGN_EXPECT_RESULT(algo, TEST_CONG_AVOID_ALGO_MIN, TEST_CONG_AVOID_ALGO_MAX, inter_result2);
            if((RT_ERR_OK == congAvoidAlgo[algo]) && (inter_result2 == 1))
                inter_result2 = 1;
            else
                inter_result2 = 0;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_congAvoidAlgo_set(unit, algo);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_set (unit %u, algo %u)"
                                , ret, expect_result, unit, algo);
                ret = rtk_qos_congAvoidAlgo_get(unit, &result_algo);
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_get (unit %u, algo %u)"
                                 , ret, expect_result, unit, algo);
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidAlgo_set/get value unequal (unit %u, algo %u)"
                                 , result_algo, algo, unit, algo);
            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidAlgo_set (unit %u, algo %u)"
                                    , ret, RT_ERR_OK, unit, algo);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_congAvoidSysThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_congAvoidThresh_t  congAvoidThresh, congAvoidThresh_result;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    uint8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidSysThresh_set(unit, 0, &congAvoidThresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidSysThresh_get(unit, 0, &congAvoidThresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;

        uint32  maxThresh[UNITTEST_MAX_TEST_VALUE];
        int32  maxThresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  maxThresh_index;
        int32  maxThresh_last;

        uint32  minThresh[UNITTEST_MAX_TEST_VALUE];
        int32  minThresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  minThresh_index;
        int32  minThresh_last;


        UNITTEST_TEST_VALUE_ASSIGN(RTK_DROP_PRECEDENCE_MAX, 0, dp, dp_result, dp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_FLOWCTRL_THRESH_MAX(unit), TEST_FLOWCTRL_THRESH_MIN, maxThresh, maxThresh_result, maxThresh_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_FLOWCTRL_THRESH_MAX(unit), TEST_FLOWCTRL_THRESH_MIN, minThresh, minThresh_result, minThresh_last);

        for (dp_index = 0; dp_index <= dp_last; dp_index++)
        {
            for (maxThresh_index = 0; maxThresh_index <= maxThresh_last; maxThresh_index++)
            {
                for (minThresh_index = 0; minThresh_index <= minThresh_last; minThresh_index++)
                {
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (dp_result[dp_index] && maxThresh_result[maxThresh_index] && minThresh_result[minThresh_index])? RT_ERR_OK : RT_ERR_FAILED;
                    congAvoidThresh.maxThresh = maxThresh[maxThresh_index];
                    congAvoidThresh.minThresh = minThresh[minThresh_index];
                    ret = rtk_qos_congAvoidSysThresh_set(unit, dp[dp_index], &congAvoidThresh);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set (unit %u, dp %u, maxThresh %u, minThresh %u)"
                                        , ret, expect_result, unit, dp[dp_index], maxThresh[maxThresh_index], minThresh[minThresh_index]);

                        ret = rtk_qos_congAvoidSysThresh_get(unit, dp[dp_index], &congAvoidThresh_result);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_get (unit %u, dp %u, maxThresh %u, minThresh %u)"
                                         , ret, expect_result, unit, dp[dp_index], maxThresh[maxThresh_index], minThresh[minThresh_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set/get value unequal (unit %u, dp %u, maxThresh %u)"
                                         , congAvoidThresh_result.maxThresh, maxThresh[maxThresh_index], unit, dp[dp_index], maxThresh[maxThresh_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set/get value unequal (unit %u, dp %u, minThresh %u)"
                                         , congAvoidThresh_result.minThresh, minThresh[minThresh_index], unit, dp[dp_index], minThresh[minThresh_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysThresh_set (unit %u, dp %u, maxThresh %u, minThresh %u)"
                                            , ret, RT_ERR_OK, unit, dp[dp_index], maxThresh[maxThresh_index], minThresh[minThresh_index]);
                    }
                }
            }

            ret = rtk_qos_congAvoidSysThresh_get(unit, dp[dp_index], &congAvoidThresh_result);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (dp_result[dp_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_get (unit %u, dp %u)"
                                , ret, expect_result, unit, dp[dp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysThresh_get (unit %u, dp %u)"
                                    , ret, RT_ERR_OK, unit, dp[dp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dp;
        uint32  dp_max;
        uint32  maxThresh;
        uint32  maxThresh_max;
        uint32  minThresh;
        uint32  minThresh_max;
        rtk_qos_congAvoidThresh_t congAvoidThresh;
        rtk_qos_congAvoidThresh_t result_congAvoidThresh;


        G_VALUE(TEST_DP_MAX, dp_max);
        G_VALUE(TEST_FLOWCTRL_THRESH_MAX(unit), maxThresh_max);
        G_VALUE(TEST_FLOWCTRL_THRESH_MAX(unit), minThresh_max);

        for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
        {
            ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result2);

            for (L_VALUE(TEST_FLOWCTRL_THRESH_MIN, maxThresh); maxThresh <= maxThresh_max; maxThresh++)
            {
                ASSIGN_EXPECT_RESULT(maxThresh, TEST_FLOWCTRL_THRESH_MIN, TEST_FLOWCTRL_THRESH_MAX(unit), inter_result3);

                for (L_VALUE(TEST_FLOWCTRL_THRESH_MIN, minThresh); minThresh <= minThresh_max; minThresh++)
                {
                    ASSIGN_EXPECT_RESULT(minThresh, TEST_FLOWCTRL_THRESH_MIN, TEST_FLOWCTRL_THRESH_MAX(unit), inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;

                    congAvoidThresh.maxThresh = maxThresh;
                    congAvoidThresh.minThresh = minThresh;
                    ret = rtk_qos_congAvoidSysThresh_set(unit, dp, &congAvoidThresh);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set (unit %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                        , ret, expect_result, unit, dp, maxThresh, minThresh);

                        ret = rtk_qos_congAvoidSysThresh_get(unit, dp, &result_congAvoidThresh);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_get (unit %u, dp %u)"
                                         , ret, expect_result, unit, dp);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set/get maxThresh value unequal (unit %u, dp %u)"
                                         , result_congAvoidThresh.maxThresh, maxThresh, unit, dp);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysThresh_set/get minThresh value unequal (unit %u, dp %u)"
                                         , result_congAvoidThresh.minThresh, minThresh, unit, dp);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysThresh_set (unit %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                            , ret, RT_ERR_OK, unit, dp, maxThresh, minThresh);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_priSelGroup_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_priSelWeight_t  weightOfPriSel, weightOfPriSel_result;
    int32  expect_result = 1;
    uint32 weightDSCP[UNITTEST_MAX_TEST_VALUE];
    int32  weightDSCP_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightDSCP_index;
    int32  weightDSCP_last;

    uint32 weightITAG[UNITTEST_MAX_TEST_VALUE];
    int32  weightITAG_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightITAG_index;
    int32  weightITAG_last;

    uint32 weightOTAG[UNITTEST_MAX_TEST_VALUE];
    int32  weightOTAG_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightOTAG_index;
    int32  weightOTAG_last;

    rtk_port_t  grpIdx[UNITTEST_MAX_TEST_VALUE];
    int32  grpIdx_result[UNITTEST_MAX_TEST_VALUE];
    int32  grpIdx_index;
    int32  grpIdx_last;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)

    uint32 weightPBP[UNITTEST_MAX_TEST_VALUE];
    int32  weightPBP_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightPBP_index;
    int32  weightPBP_last;

    uint32 weightInAcl[UNITTEST_MAX_TEST_VALUE];
    int32  weightInAcl_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightInAcl_index;
    int32  weightInAcl_last;

    uint32 weightMacVlan[UNITTEST_MAX_TEST_VALUE];
    int32  weightMacVlan_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightMacVlan_index;
    int32  weightMacVlan_last;

    uint32 weightProtoVlan[UNITTEST_MAX_TEST_VALUE];
    int32  weightProtoVlan_result[UNITTEST_MAX_TEST_VALUE];
    int32  weightProtoVlan_index;
    int32  weightProtoVlan_last;

    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightPBP, weightPBP_result, weightPBP_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightInAcl, weightInAcl_result, weightInAcl_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightMacVlan, weightMacVlan_result, weightMacVlan_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightProtoVlan, weightProtoVlan_result, weightProtoVlan_last);

#endif

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_priSelGroup_set(unit, 0, &weightOfPriSel))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_priSelGroup_get(unit, 0, &weightOfPriSel)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_SEL_GROUP_INDEX_MAX(unit), 0, grpIdx, grpIdx_result, grpIdx_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightDSCP, weightDSCP_result, weightDSCP_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightITAG, weightITAG_result, weightITAG_last);
    UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightOTAG, weightOTAG_result, weightOTAG_last);
    for (grpIdx_index = 0; grpIdx_index <= grpIdx_last; grpIdx_index++)
    {
        for (weightDSCP_index = 0; weightDSCP_index <= weightDSCP_last; weightDSCP_index++)
        {
            for (weightITAG_index = 0; weightITAG_index <= weightITAG_last; weightITAG_index++)
            {
                for (weightOTAG_index = 0; weightOTAG_index <= weightOTAG_last; weightOTAG_index++)
                {

#if defined(CONFIG_SDK_RTL8380)
                    if (HWP_8380_30_FAMILY(unit))
                    {
                        uint32 weightPBIpri[UNITTEST_MAX_TEST_VALUE];
                        int32  weightPBIpri_result[UNITTEST_MAX_TEST_VALUE];
                        int32  weightPBIpri_index;
                        int32  weightPBIpri_last;

                        uint32 weightPBOpri[UNITTEST_MAX_TEST_VALUE];
                        int32  weightPBOpri_result[UNITTEST_MAX_TEST_VALUE];
                        int32  weightPBOpri_index;
                        int32  weightPBOpri_last;

                        UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightPBIpri, weightPBIpri_result, weightPBIpri_last);
                        UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightPBOpri, weightPBOpri_result, weightPBOpri_last);


                        for (weightPBIpri_index = 0; weightPBIpri_index <= weightPBIpri_last; weightPBIpri_index++)
                        {
                            for (weightPBOpri_index = 0; weightPBOpri_index <= weightPBOpri_last; weightPBOpri_index++)
                            {

                                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                    expect_result = RT_ERR_FAILED;
                                else
                                    expect_result = (grpIdx_result[grpIdx_index]
                                                && weightDSCP_result[weightDSCP_index] && weightITAG_result[weightITAG_index]
                                                && weightOTAG_result[weightOTAG_index] && weightPBIpri_result[weightPBIpri_index]
                                                && weightPBOpri_result[weightPBOpri_index])? RT_ERR_OK : RT_ERR_FAILED;

                                weightOfPriSel.weight_of_dscp = weightDSCP[weightDSCP_index];
                                weightOfPriSel.weight_of_innerTag = weightITAG[weightITAG_index];
                                weightOfPriSel.weight_of_outerTag = weightOTAG[weightOTAG_index];
                                weightOfPriSel.weight_of_portBasedIpri = weightPBIpri[weightPBIpri_index];
                                weightOfPriSel.weight_of_portBasedOpri = weightPBOpri[weightPBOpri_index];

                                ret = rtk_qos_priSelGroup_set(unit, grpIdx[grpIdx_index], &weightOfPriSel);
                                if (RT_ERR_OK == expect_result)
                                {
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_set (unit %u, grpIdx %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightPBIpri %u, weightPBOpri %u)"
                                                    , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                    , weightDSCP[weightDSCP_index], weightITAG[weightITAG_index]
                                                    , weightOTAG[weightOTAG_index], weightPBIpri[weightPBIpri_index]
                                                    , weightPBOpri[weightPBOpri_index]);

                                    ret = rtk_qos_priSelGroup_get(unit, grpIdx[grpIdx_index], &weightOfPriSel_result);

                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightPBIpri %u, weightPBOpri %u)"
                                                    , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                    , weightDSCP[weightDSCP_index], weightITAG[weightITAG_index]
                                                    , weightOTAG[weightOTAG_index], weightPBIpri[weightPBIpri_index]
                                                    , weightPBOpri[weightPBOpri_index]);
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u)"
                                                    , weightOfPriSel_result.weight_of_dscp, weightDSCP[weightDSCP_index], unit, grpIdx[grpIdx_index]
                                                    , weightDSCP[weightDSCP_index]);
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightITAG %u)"
                                                    , weightOfPriSel_result.weight_of_innerTag, weightITAG[weightITAG_index], unit, grpIdx[grpIdx_index]
                                                    , weightITAG[weightITAG_index]);
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightOTAG %u)"
                                                    , weightOfPriSel_result.weight_of_outerTag, weightOTAG[weightOTAG_index], unit, grpIdx[grpIdx_index]
                                                    , weightOTAG[weightOTAG_index]);
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBIpri %u)"
                                                    , weightOfPriSel_result.weight_of_portBasedIpri, weightPBIpri[weightPBIpri_index], unit, grpIdx[grpIdx_index]
                                                    , weightPBIpri[weightPBIpri_index]);
                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBOpri %u)"
                                                    , weightOfPriSel_result.weight_of_portBasedOpri, weightPBOpri[weightPBOpri_index], unit, grpIdx[grpIdx_index]
                                                    , weightPBOpri[weightPBOpri_index]);

                                }
                                else
                                {

                                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightPBIpri %u, weightPBOpri %u)"
                                                    , ret, RT_ERR_OK, unit, grpIdx[grpIdx_index]
                                                    , weightDSCP[weightDSCP_index], weightITAG[weightITAG_index]
                                                    , weightOTAG[weightOTAG_index], weightPBIpri[weightPBIpri_index]
                                                    , weightPBOpri[weightPBOpri_index]);
                                }
                            }
                        }
                    }
#endif
#if defined(CONFIG_SDK_RTL8390)
                    if (HWP_8390_50_FAMILY(unit))
                    {
                        for (weightPBP_index = 0; weightPBP_index <= weightPBP_last; weightPBP_index++)
                        {
                            for (weightInAcl_index = 0; weightInAcl_index <= weightInAcl_last; weightInAcl_index++)
                            {
                                for (weightMacVlan_index = 0; weightMacVlan_index <= weightMacVlan_last; weightMacVlan_index++)
                                {
                                    for (weightProtoVlan_index = 0; weightProtoVlan_index <= weightProtoVlan_last; weightProtoVlan_index++)
                                    {
                                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                            expect_result = RT_ERR_FAILED;
                                        else
                                            expect_result = (grpIdx_result[grpIdx_index] && weightPBP_result[weightPBP_index]
                                                        && weightDSCP_result[weightDSCP_index] && weightITAG_result[weightITAG_index]
                                                        && weightOTAG_result[weightOTAG_index] && weightInAcl_result[weightInAcl_index]
                                                        && weightMacVlan_result[weightMacVlan_index] && weightProtoVlan_result[weightProtoVlan_index])? RT_ERR_OK : RT_ERR_FAILED;


                                        weightOfPriSel.weight_of_portBased = weightPBP[weightPBP_index];
                                        weightOfPriSel.weight_of_dscp = weightDSCP[weightDSCP_index];
                                        weightOfPriSel.weight_of_innerTag = weightITAG[weightITAG_index];
                                        weightOfPriSel.weight_of_outerTag = weightOTAG[weightOTAG_index];
                                        weightOfPriSel.weight_of_inAcl = weightInAcl[weightInAcl_index];
                                        weightOfPriSel.weight_of_macVlan = weightMacVlan[weightMacVlan_index];
                                        weightOfPriSel.weight_of_protoVlan = weightProtoVlan[weightProtoVlan_index];
                                        ret = rtk_qos_priSelGroup_set(unit, grpIdx[grpIdx_index], &weightOfPriSel);
                                        if (RT_ERR_OK == expect_result)
                                        {
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_set (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u)"
                                                            , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                            , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                            , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                            , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                            , weightProtoVlan[weightProtoVlan_index]);

                                            ret = rtk_qos_priSelGroup_get(unit, grpIdx[grpIdx_index], &weightOfPriSel_result);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u)"
                                                            , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                            , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                            , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                            , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                            , weightProtoVlan[weightProtoVlan_index]);

                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u)"
                                                            , weightOfPriSel_result.weight_of_portBased, weightPBP[weightPBP_index], unit, grpIdx[grpIdx_index]
                                                            , weightPBP[weightPBP_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u)"
                                                            , weightOfPriSel_result.weight_of_dscp, weightDSCP[weightDSCP_index], unit, grpIdx[grpIdx_index]
                                                            , weightDSCP[weightDSCP_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightITAG %u)"
                                                            , weightOfPriSel_result.weight_of_innerTag, weightITAG[weightITAG_index], unit, grpIdx[grpIdx_index]
                                                            , weightITAG[weightITAG_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightOTAG %u)"
                                                            , weightOfPriSel_result.weight_of_outerTag, weightOTAG[weightOTAG_index], unit, grpIdx[grpIdx_index]
                                                            , weightOTAG[weightOTAG_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightInAcl %u)"
                                                            , weightOfPriSel_result.weight_of_inAcl, weightInAcl[weightInAcl_index], unit, grpIdx[grpIdx_index]
                                                            , weightInAcl[weightInAcl_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightMacVlan %u)"
                                                            , weightOfPriSel_result.weight_of_macVlan, weightMacVlan[weightMacVlan_index], unit, grpIdx[grpIdx_index]
                                                            , weightMacVlan[weightMacVlan_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightProtoVlan %u)"
                                                            , weightOfPriSel_result.weight_of_protoVlan, weightProtoVlan[weightProtoVlan_index], unit, grpIdx[grpIdx_index]
                                                            , weightProtoVlan[weightProtoVlan_index]);
                                        }
                                        else
                                        {

                                            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u)"
                                                            , ret, RT_ERR_OK, unit, grpIdx[grpIdx_index]
                                                            , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                            , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                            , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                            , weightProtoVlan[weightProtoVlan_index]);
                                        }
                                    }
                                }
                            }
                        }
                    }
#endif
#if defined(CONFIG_SDK_RTL9300)
                    if(HWP_9300_FAMILY_ID(unit))
                    {
                        uint32 weightRouting[UNITTEST_MAX_TEST_VALUE];
                        int32  weightRouting_result[UNITTEST_MAX_TEST_VALUE];
                        int32  weightRouting_index;
                        int32  weightRouting_last;

                        UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightRouting, weightRouting_result, weightRouting_last);

                            for (weightPBP_index = 0; weightPBP_index <= weightPBP_last; weightPBP_index++)
                            {
                                for (weightInAcl_index = 0; weightInAcl_index <= weightInAcl_last; weightInAcl_index++)
                                {
                                    for (weightMacVlan_index = 0; weightMacVlan_index <= weightMacVlan_last; weightMacVlan_index++)
                                    {
                                        for (weightProtoVlan_index = 0; weightProtoVlan_index <= weightProtoVlan_last; weightProtoVlan_index++)
                                        {
                                            for (weightRouting_index = 0; weightRouting_index <= weightRouting_last; weightRouting_index++)
                                            {
                                                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                                    expect_result = RT_ERR_FAILED;
                                                else
                                                    expect_result = (grpIdx_result[grpIdx_index] && weightPBP_result[weightPBP_index]
                                                                && weightDSCP_result[weightDSCP_index] && weightITAG_result[weightITAG_index]
                                                                && weightOTAG_result[weightOTAG_index] && weightInAcl_result[weightInAcl_index]
                                                                && weightMacVlan_result[weightMacVlan_index] && weightProtoVlan_result[weightProtoVlan_index]
                                                                && weightRouting_result[weightRouting_index])? RT_ERR_OK : RT_ERR_FAILED;


                                                weightOfPriSel.weight_of_portBased = weightPBP[weightPBP_index];
                                                weightOfPriSel.weight_of_dscp = weightDSCP[weightDSCP_index];
                                                weightOfPriSel.weight_of_innerTag = weightITAG[weightITAG_index];
                                                weightOfPriSel.weight_of_outerTag = weightOTAG[weightOTAG_index];
                                                weightOfPriSel.weight_of_vlanAcl = weightInAcl[weightInAcl_index];
                                                weightOfPriSel.weight_of_macVlan = weightMacVlan[weightMacVlan_index];
                                                weightOfPriSel.weight_of_protoVlan = weightProtoVlan[weightProtoVlan_index];
                                                weightOfPriSel.weight_of_routing = weightRouting[weightRouting_index];
                                                ret = rtk_qos_priSelGroup_set(unit, grpIdx[grpIdx_index], &weightOfPriSel);
                                                if (RT_ERR_OK == expect_result)
                                                {
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_set (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u)"
                                                                    , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                                    , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                    , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                    , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                    , weightProtoVlan[weightProtoVlan_index], weightRouting[weightRouting_index]);

                                                    ret = rtk_qos_priSelGroup_get(unit, grpIdx[grpIdx_index], &weightOfPriSel_result);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u)"
                                                                    , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                                    , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                    , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                    , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                    , weightProtoVlan[weightProtoVlan_index], weightRouting[weightRouting_index]);

                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u)"
                                                                    , weightOfPriSel_result.weight_of_portBased, weightPBP[weightPBP_index], unit, grpIdx[grpIdx_index]
                                                                    , weightPBP[weightPBP_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u)"
                                                                    , weightOfPriSel_result.weight_of_dscp, weightDSCP[weightDSCP_index], unit, grpIdx[grpIdx_index]
                                                                    , weightDSCP[weightDSCP_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightITAG %u)"
                                                                    , weightOfPriSel_result.weight_of_innerTag, weightITAG[weightITAG_index], unit, grpIdx[grpIdx_index]
                                                                    , weightITAG[weightITAG_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightOTAG %u)"
                                                                    , weightOfPriSel_result.weight_of_outerTag, weightOTAG[weightOTAG_index], unit, grpIdx[grpIdx_index]
                                                                    , weightOTAG[weightOTAG_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightInAcl %u)"
                                                                    , weightOfPriSel_result.weight_of_vlanAcl, weightInAcl[weightInAcl_index], unit, grpIdx[grpIdx_index]
                                                                    , weightInAcl[weightInAcl_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightMacVlan %u)"
                                                                    , weightOfPriSel_result.weight_of_macVlan, weightMacVlan[weightMacVlan_index], unit, grpIdx[grpIdx_index]
                                                                    , weightMacVlan[weightMacVlan_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightProtoVlan %u)"
                                                                    , weightOfPriSel_result.weight_of_protoVlan, weightProtoVlan[weightProtoVlan_index], unit, grpIdx[grpIdx_index]
                                                                    , weightProtoVlan[weightProtoVlan_index]);
                                                    RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightRouting %u)"
                                                                    , weightOfPriSel_result.weight_of_routing, weightRouting[weightRouting_index], unit, grpIdx[grpIdx_index]
                                                                    , weightProtoVlan[weightRouting_index]);
                                                }
                                                else
                                                {

                                                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u)"
                                                                    , ret, RT_ERR_OK, unit, grpIdx[grpIdx_index]
                                                                    , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                    , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                    , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                    , weightProtoVlan[weightProtoVlan_index], weightProtoVlan[weightRouting_index]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
#endif
#if defined(CONFIG_SDK_RTL9310)
                        if (HWP_9310_FAMILY_ID(unit))
                        {
                            uint32 weightRouting[UNITTEST_MAX_TEST_VALUE];
                            int32  weightRouting_result[UNITTEST_MAX_TEST_VALUE];
                            int32  weightRouting_index;
                            int32  weightRouting_last;

                            uint32 weight80211e[UNITTEST_MAX_TEST_VALUE];
                            int32  weight80211e_result[UNITTEST_MAX_TEST_VALUE];
                            int32  weight80211e_index;
                            int32  weight80211e_last;

                            uint32 weightTunnel;
                            int32  weightTunnel_result[UNITTEST_MAX_TEST_VALUE];
                            int32  weightTunnel_index;
                            int32  weightTunnel_last;

                            uint32 weightMpls;
                            int32  weightMpls_result[UNITTEST_MAX_TEST_VALUE];
                            int32  weightMpls_index;
                            int32  weightMpls_last;

                            UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightRouting, weightRouting_result, weightRouting_last);
                            UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weight80211e, weight80211e_result, weight80211e_last);
                            UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightTunnel, weightTunnel_result, weightTunnel_last);
                            UNITTEST_TEST_VALUE_ASSIGN(HAL_PRI_OF_SELECTION_MAX(unit), HAL_PRI_OF_SELECTION_MIN(unit), weightMpls, weightMpls_result, weightMpls_last);

                            for (weightPBP_index = 0; weightPBP_index <= weightPBP_last; weightPBP_index++)
                            {
                                for (weightInAcl_index = 0; weightInAcl_index <= weightInAcl_last; weightInAcl_index++)
                                {
                                    for (weightMacVlan_index = 0; weightMacVlan_index <= weightMacVlan_last; weightMacVlan_index++)
                                    {
                                        for (weightProtoVlan_index = 0; weightProtoVlan_index <= weightProtoVlan_last; weightProtoVlan_index++)
                                        {
                                            for (weightRouting_index = 0; weightRouting_index <= weightRouting_last; weightRouting_index++)
                                            {
                                                for (weight80211e_index = 0; weight80211e_index <= weight80211e_last; weight80211e_index++)
                                                {
                                                    for (weightTunnel_index = 0; weightTunnel_index <= weightTunnel_last; weightTunnel_index++)
                                                    {
                                                        for (weightMpls_index = 0; weightMpls_index <= weightMpls_last; weightMpls_index++)
                                                        {
                                                            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                                                expect_result = RT_ERR_FAILED;
                                                            else
                                                                expect_result = (grpIdx_result[grpIdx_index] && weightPBP_result[weightPBP_index]
                                                                            && weightDSCP_result[weightDSCP_index] && weightITAG_result[weightITAG_index]
                                                                            && weightOTAG_result[weightOTAG_index] && weightInAcl_result[weightInAcl_index]
                                                                            && weightMacVlan_result[weightMacVlan_index] && weightProtoVlan_result[weightProtoVlan_index]
                                                                            && weightRouting_result[weightRouting_index]&& weight80211e_result
                                                                            && weightTunnel_result
                                                                            && weightMpls_result)? RT_ERR_OK : RT_ERR_FAILED;

                                                            weightOfPriSel.weight_of_portBased = weightPBP[weightPBP_index];
                                                            weightOfPriSel.weight_of_dscp = weightDSCP[weightDSCP_index];
                                                            weightOfPriSel.weight_of_innerTag = weightITAG[weightITAG_index];
                                                            weightOfPriSel.weight_of_outerTag = weightOTAG[weightOTAG_index];
                                                            weightOfPriSel.weight_of_vlanAcl = weightInAcl[weightInAcl_index];
                                                            weightOfPriSel.weight_of_macVlan = weightMacVlan[weightMacVlan_index];
                                                            weightOfPriSel.weight_of_protoVlan = weightProtoVlan[weightProtoVlan_index];
                                                            weightOfPriSel.weight_of_routing = weightRouting[weightRouting_index];
                                                            weightOfPriSel.weight_of_80211e = weight80211e[weight80211e_index];
                                                            weightOfPriSel.weight_of_tunnel = weightTunnel[weightTunnel_index];
                                                            weightOfPriSel.weight_of_mpls = weightMpls[weightMpls_index];
                                                            ret = rtk_qos_priSelGroup_set(unit, grpIdx[grpIdx_index], &weightOfPriSel);
                                                            if (RT_ERR_OK == expect_result)
                                                            {
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_set (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u, weight80211e %u, weightTunnel %u, weight_of_mpls %u)"
                                                                                , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                                                , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                                , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                                , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                                , weightProtoVlan[weightProtoVlan_index], weightRouting[weightRouting_index]
                                                                                , weight80211e[weight80211e_index]
                                                                                , weightTunnel[weightTunnel_index]
                                                                                , weightMpls[weightMpls_index]);

                                                                ret = rtk_qos_priSelGroup_get(unit, grpIdx[grpIdx_index], &weightOfPriSel_result);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_set (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u, weight80211e %u, weightTunnel %u, weight_of_mpls %u)"
                                                                                , ret, expect_result, unit, grpIdx[grpIdx_index]
                                                                                , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                                , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                                , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                                , weightProtoVlan[weightProtoVlan_index], weightRouting[weightRouting_index]
                                                                                , weight80211e[weight80211e_index]
                                                                                , weightTunnel[weightTunnel_index]
                                                                                , weightMpls[weightMpls_index]);

                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u)"
                                                                                , weightOfPriSel_result.weight_of_portBased, weightPBP[weightPBP_index], unit, grpIdx[grpIdx_index]
                                                                                , weightPBP[weightPBP_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightDSCP %u)"
                                                                                , weightOfPriSel_result.weight_of_dscp, weightDSCP[weightDSCP_index], unit, grpIdx[grpIdx_index]
                                                                                , weightDSCP[weightDSCP_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightITAG %u)"
                                                                                , weightOfPriSel_result.weight_of_innerTag, weightITAG[weightITAG_index], unit, grpIdx[grpIdx_index]
                                                                                , weightITAG[weightITAG_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightOTAG %u)"
                                                                                , weightOfPriSel_result.weight_of_outerTag, weightOTAG[weightOTAG_index], unit, grpIdx[grpIdx_index]
                                                                                , weightOTAG[weightOTAG_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightInAcl %u)"
                                                                                , weightOfPriSel_result.weight_of_vlanAcl, weightInAcl[weightInAcl_index], unit, grpIdx[grpIdx_index]
                                                                                , weightInAcl[weightInAcl_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightMacVlan %u)"
                                                                                , weightOfPriSel_result.weight_of_macVlan, weightMacVlan[weightMacVlan_index], unit, grpIdx[grpIdx_index]
                                                                                , weightMacVlan[weightMacVlan_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightProtoVlan %u)"
                                                                                , weightOfPriSel_result.weight_of_protoVlan, weightProtoVlan[weightProtoVlan_index], unit, grpIdx[grpIdx_index]
                                                                                , weightProtoVlan[weightProtoVlan_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightRouting %u)"
                                                                                , weightOfPriSel_result.weight_of_routing, weightRouting[weightRouting_index], unit, grpIdx[grpIdx_index]
                                                                                , weightProtoVlan[weightRouting_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weight80211e %u)"
                                                                                , weightOfPriSel_result.weight_of_80211e, weight80211e[weight80211e_index], unit, grpIdx[grpIdx_index]
                                                                                , weightProtoVlan[weightRouting_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightTunnel %u)"
                                                                                , weightOfPriSel_result.weight_of_tunnel, weightTunnel[weightTunnel_index], unit, grpIdx[grpIdx_index]
                                                                                , weightProtoVlan[weightRouting_index]);
                                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightMpls %u)"
                                                                                , weightOfPriSel_result.weight_of_mpls, weightMpls[weightMpls_index], unit, grpIdx[grpIdx_index]
                                                                                , weightProtoVlan[weightRouting_index]);
                                                            }
                                                            else
                                                            {

                                                                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u, weightPBP %u, weightDSCP %u, weightITAG %u, weightOTAG %u, weightInAcl %u, weightMacVlan %u, weightProtoVlan %u, weightRouting %u, weight80211e %u, weightTunnel %u, weight_of_mpls %u)"
                                                                                , ret, RT_ERR_OK, unit, grpIdx[grpIdx_index]
                                                                                , weightPBP[weightPBP_index], weightDSCP[weightDSCP_index]
                                                                                , weightITAG[weightITAG_index], weightOTAG[weightOTAG_index]
                                                                                , weightInAcl[weightInAcl_index], weightMacVlan[weightMacVlan_index]
                                                                                , weightProtoVlan[weightProtoVlan_index], weightProtoVlan[weightRouting_index]
                                                                                , weight80211e[weight80211e_index]
                                                                                , weightTunnel[weightTunnel_index]
                                                                                , weightMpls[weightMpls_index]);
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
#endif
                    }
                }
            }
            ret = rtk_qos_priSelGroup_get(unit, grpIdx[grpIdx_index], &weightOfPriSel_result);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (grpIdx_result[grpIdx_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u)"
                                , ret, expect_result, unit, grpIdx[grpIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priSelGroup_get (unit %u, grpIdx %u)"
                                    , ret, RT_ERR_OK, unit, grpIdx[grpIdx_index]);
            }
        }

    return RT_ERR_OK;
}

int32 dal_qos_schedulingQueue_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    rtk_qid_t   queue, queue_max;
    rtk_qos_queue_weights_t  queue_weight_result;
    uint8  inter_result2 = 1;
    uint8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_schedulingQueue_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_schedulingQueue_get(unit, 0, &queue_weight_result)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  weight[UNITTEST_MAX_TEST_VALUE];
        int32  weight_result[UNITTEST_MAX_TEST_VALUE];
        int32  weight_index;
        int32  weight_last;

        rtk_qos_queue_weights_t queue_weight;
        rtk_qos_queue_weights_t result_queue_weight;

        queue_max = HAL_MAX_NUM_OF_QUEUE(unit);

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_WEIGHT_MAX(unit), TEST_QUEUE_WEIGHT_MIN, weight, weight_result, weight_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (weight_index = 0; weight_index <= weight_last; weight_index++)
            {
                /* fill all weight to same value */
                for (queue = 0; queue <= queue_max; queue++)
                {
                    queue_weight.weights[queue] = weight[weight_index];
                }
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && weight_result[weight_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_schedulingQueue_set(unit, port[port_index], &queue_weight);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_set (unit %u, port %u, weight %u)"
                                    , ret, expect_result, unit, port[port_index], weight[weight_index]);

                    ret = rtk_qos_schedulingQueue_get(unit, port[port_index], &result_queue_weight);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_get (unit %u, port %u, weight %u)"
                                     , ret, expect_result, unit, port[port_index], weight[weight_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_set/get value unequal (unit %u, port %u, weight %u)"
                                     , result_queue_weight.weights[0], weight[weight_index], unit, port[port_index], weight[weight_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingQueue_set (unit %u, port %u, weight %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], weight[weight_index]);
                }
            }

            ret = rtk_qos_schedulingQueue_get(unit, port[port_index], &result_queue_weight);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingQueue_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  weight;
        uint32  weight_max;

        rtk_qos_queue_weights_t queue_weight;
        rtk_qos_queue_weights_t result_queue_weight;

        queue_max = HAL_MAX_NUM_OF_QUEUE(unit);

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_QUEUE_WEIGHT_MAX(unit), weight_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_QUEUE_WEIGHT_MIN, weight); weight <= weight_max; weight++)
            {
                ASSIGN_EXPECT_RESULT(weight, TEST_QUEUE_WEIGHT_MIN, TEST_QUEUE_WEIGHT_MAX(unit), inter_result3);

                /* fill all weight to same value */
                for (queue = 0; queue <= queue_max; queue++)
                {
                    queue_weight.weights[queue] = weight;
                }
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_schedulingQueue_set(unit, port, &queue_weight);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_set (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                    ret = rtk_qos_schedulingQueue_get(unit,  port, &result_queue_weight);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_get (unit %u, port %u)"
                                     , ret, expect_result, unit, port);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_schedulingQueue_set/get value unequal (unit %u, port %u, weights %u)"
                                     , result_queue_weight.weights[0], weight, unit, port, weight);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_schedulingQueue_set (unit %u, port %u, weight %u)"
                                        , ret, RT_ERR_OK, unit, port, weight);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dpSrcSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32 result_dpSrcType;
    uint8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dpSrcSel_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dpSrcSel_get(unit, &result_dpSrcType)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  dpSrcType[UNITTEST_MAX_TEST_VALUE];
        int32  dpSrcType_result[UNITTEST_MAX_TEST_VALUE];
        int32  dpSrcType_index;
        int32  dpSrcType_last;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DPSRCSEL_MAX, TEST_DPSRCSEL_MIN, dpSrcType, dpSrcType_result, dpSrcType_last);

        for (dpSrcType_index = 0; dpSrcType_index <= dpSrcType_last; dpSrcType_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (dpSrcType_result[dpSrcType_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_dpSrcSel_set(unit, dpSrcType[dpSrcType_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_set (unit %u, dpSrcType %u)"
                                , ret, expect_result, unit, dpSrcType[dpSrcType_index]);

                ret = rtk_qos_dpSrcSel_get(unit, (uint32*)&result_dpSrcType);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_get (unit %u, dpSrcType %u)"
                                 , ret, expect_result, unit, dpSrcType[dpSrcType_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_set/get value unequal (unit %u, dpSrcType %u)"
                                 , result_dpSrcType, dpSrcType[dpSrcType_index], unit, dpSrcType[dpSrcType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dpSrcSel_set (unit %u, dpSrcType %u)"
                                    , ret, RT_ERR_OK, unit, dpSrcType[dpSrcType_index]);
            }
        }

        ret = rtk_qos_dpSrcSel_get(unit, (uint32*)&result_dpSrcType);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dpSrcSel_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 dpSrcType;
        uint32 dpSrcType_max;
        uint32 result_dpSrcType;


        G_VALUE(TEST_DPSRCSEL_MAX, dpSrcType_max);

        for (L_VALUE(TEST_DPSRCSEL_MIN, dpSrcType); dpSrcType <= dpSrcType_max; dpSrcType++)
        {
            ASSIGN_EXPECT_RESULT(dpSrcType, TEST_DPSRCSEL_MIN, TEST_DPSRCSEL_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_dpSrcSel_set(unit, dpSrcType);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_set (unit %u, dpSrcType %u)"
                                , ret, expect_result, unit, dpSrcType);
                ret = rtk_qos_dpSrcSel_get(unit, &result_dpSrcType);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_get (unit %u, dpSrcType %u)"
                                 , ret, expect_result, unit, dpSrcType);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dpSrcSel_set/get value unequal (unit %u, dpSrcType %u)"
                                 , result_dpSrcType, dpSrcType, unit, dpSrcType);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dpSrcSel_set (unit %u, dpSrcType %u)"
                                    , ret, RT_ERR_OK, unit, dpSrcType);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_portDEISrcSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;

    uint32  temp_portDEISrcSel;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDEISrcSel_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDEISrcSel_get(unit, 0, &temp_portDEISrcSel)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_deiSel_t  deiSrcSel[UNITTEST_MAX_TEST_VALUE];
        int32  deiSrcSel_result[UNITTEST_MAX_TEST_VALUE];
        int32  deiSrcSel_index;
        int32  deiSrcSel_last;
        rtk_qos_deiSel_t  result_deiSrcSel;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DEISRCSEL_MAX, TEST_DEISRCSEL_MIN, deiSrcSel, deiSrcSel_result, deiSrcSel_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (deiSrcSel_index = 0; deiSrcSel_index <= deiSrcSel_last; deiSrcSel_index++)
            {
                inter_result3 = deiSrcSel_result[deiSrcSel_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDEISrcSel_set(unit, port[port_index], deiSrcSel[deiSrcSel_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_set (unit %u, port %u, deiSrcSel %u)"
                                    , ret, expect_result, unit, port[port_index], deiSrcSel[deiSrcSel_index]);

                    ret = rtk_qos_portDEISrcSel_get(unit, port[port_index], &result_deiSrcSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_get (unit %u, port %u, deiSrcSel %u)"
                                     , ret, expect_result, unit, port[port_index], deiSrcSel[deiSrcSel_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_set/get value unequal (unit %u, port %u, deiSrcSel %u)"
                                     , result_deiSrcSel, deiSrcSel[deiSrcSel_index], unit, port[port_index], deiSrcSel[deiSrcSel_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDEISrcSel_set (unit %u, port %u, deiSrcSel %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], deiSrcSel[deiSrcSel_index]);
                }
            }

            ret = rtk_qos_portDEISrcSel_get(unit, port[port_index], &result_deiSrcSel);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDEISrcSel_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  deiSrcSel;
        uint32  deiSrcSel_max;
        uint32  result_deiSrcSel;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_DEISRCSEL_MAX, deiSrcSel_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_DEISRCSEL_MIN, deiSrcSel); deiSrcSel <= deiSrcSel_max; deiSrcSel++)
            {
                ASSIGN_EXPECT_RESULT(deiSrcSel, TEST_DEISRCSEL_MIN, TEST_DEISRCSEL_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDEISrcSel_set(unit, port, deiSrcSel);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_set (unit %u, port %u, deiSrcSel %u)"
                                    , ret, expect_result, unit, port, deiSrcSel);
                    ret = rtk_qos_portDEISrcSel_get(unit,  port, &result_deiSrcSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_get (unit %u, port %u, deiSrcSel %u)"
                                     , ret, expect_result, unit, port, deiSrcSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDEISrcSel_set/get value unequal (unit %u, port %u, deiSrcSel %u)"
                                     , result_deiSrcSel, deiSrcSel, unit, port, deiSrcSel);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDEISrcSel_set (unit %u, port %u, deiSrcSel %u)"
                                        , ret, RT_ERR_OK, unit, port, deiSrcSel);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_deiDpRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_dp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_deiDpRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_deiDpRemap_get(unit, 0, &temp_dp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dei[UNITTEST_MAX_TEST_VALUE];
        int32  dei_result[UNITTEST_MAX_TEST_VALUE];
        int32  dei_index;
        int32  dei_last;

        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;
        uint32  result_dp;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DEI_MAX, TEST_DEI_MIN, dei, dei_result, dei_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);

        for (dei_index = 0; dei_index <= dei_last; dei_index++)
        {
            inter_result2 = dei_result[dei_index];
            for (dp_index = 0; dp_index <= dp_last; dp_index++)
            {
                inter_result3 = dp_result[dp_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_deiDpRemap_set(unit, dei[dei_index], dp[dp_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_set (unit %u, dei %u, dp %u)"
                                    , ret, expect_result, unit, dei[dei_index], dp[dp_index]);

                    ret = rtk_qos_deiDpRemap_get(unit, dei[dei_index], &result_dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_get (unit %u, dei %u, dp %u)"
                                     , ret, expect_result, unit, dei[dei_index], dp[dp_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_set/get value unequal (unit %u, dei %u, dp %u)"
                                     , result_dp, dp[dp_index], unit, dei[dei_index], dp[dp_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiDpRemap_set (unit %u, dei %u, dp %u)"
                                        , ret, RT_ERR_OK, unit, dei[dei_index], dp[dp_index]);
                }
            }

            ret = rtk_qos_deiDpRemap_get(unit, dei[dei_index], &result_dp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_get (unit %u, dei %u)"
                                , ret, expect_result, unit, dei[dei_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiDpRemap_get (unit %u, dei %u)"
                                    , ret, RT_ERR_OK, unit, dei[dei_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dei;
        uint32  dei_max;
        uint32  dp;
        uint32  dp_max;
        uint32  result_dp;


        G_VALUE(TEST_DEI_MAX, dei_max);
        G_VALUE(TEST_DP_MAX, dp_max);

        for (L_VALUE(TEST_DEI_MIN, dei); dei <= dei_max; dei++)
        {
            ASSIGN_EXPECT_RESULT(dei, TEST_DEI_MIN, TEST_DEI_MAX, inter_result2);

            for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
            {
                ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_deiDpRemap_set(unit, dei, dp);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_set (unit %u, dei %u, dp %u)"
                                    , ret, expect_result, unit, dei, dp);
                    ret = rtk_qos_deiDpRemap_get(unit,  dei, &result_dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_get (unit %u, dei %u, dp %u)"
                                     , ret, expect_result, unit, dei, dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiDpRemap_set/get value unequal (unit %u, dei %u, dp %u)"
                                     , result_dp, dp, unit, dei, dp);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiDpRemap_set (unit %u, dei %u, dp %u)"
                                        , ret, RT_ERR_OK, unit, dei, dp);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dscpDpRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_dp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpDpRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpDpRemap_get(unit, 0, &temp_dp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dscp[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_index;
        int32  dscp_last;

        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;
        uint32  result_dp;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DSCP_MAX, TEST_DSCP_MIN, dscp, dscp_result, dscp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);

        for (dscp_index = 0; dscp_index <= dscp_last; dscp_index++)
        {
            inter_result2 = dscp_result[dscp_index];
            for (dp_index = 0; dp_index <= dp_last; dp_index++)
            {
                inter_result3 = dp_result[dp_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscpDpRemap_set(unit, dscp[dscp_index], dp[dp_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_set (unit %u, dscp %u, dp %u)"
                                    , ret, expect_result, unit, dscp[dscp_index], dp[dp_index]);

                    ret = rtk_qos_dscpDpRemap_get(unit, dscp[dscp_index], &result_dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_get (unit %u, dscp %u, dp %u)"
                                     , ret, expect_result, unit, dscp[dscp_index], dp[dp_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_set/get value unequal (unit %u, dscp %u, dp %u)"
                                     , result_dp, dp[dp_index], unit, dscp[dscp_index], dp[dp_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpDpRemap_set (unit %u, dscp %u, dp %u)"
                                        , ret, RT_ERR_OK, unit, dscp[dscp_index], dp[dp_index]);
                }
            }

            ret = rtk_qos_dscpDpRemap_get(unit, dscp[dscp_index], &result_dp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_get (unit %u, dscp %u)"
                                , ret, expect_result, unit, dscp[dscp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpDpRemap_get (unit %u, dscp %u)"
                                    , ret, RT_ERR_OK, unit, dscp[dscp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dscp;
        uint32  dscp_max;
        uint32  dp;
        uint32  dp_max;
        uint32  result_dp;


        G_VALUE(TEST_DSCP_MAX, dscp_max);
        G_VALUE(TEST_DP_MAX, dp_max);

        for (L_VALUE(TEST_DSCP_MIN, dscp); dscp <= dscp_max; dscp++)
        {
            ASSIGN_EXPECT_RESULT(dscp, TEST_DSCP_MIN, TEST_DSCP_MAX, inter_result2);

            for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
            {
                ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscpDpRemap_set(unit, dscp, dp);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_set (unit %u, dscp %u, dp %u)"
                                    , ret, expect_result, unit, dscp, dp);
                    ret = rtk_qos_dscpDpRemap_get(unit,  dscp, &result_dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_get (unit %u, dscp %u, dp %u)"
                                     , ret, expect_result, unit, dscp, dp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscpDpRemap_set/get value unequal (unit %u, dscp %u, dp %u)"
                                     , result_dp, dp, unit, dscp, dp);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpDpRemap_set (unit %u, dscp %u, dp %u)"
                                        , ret, RT_ERR_OK, unit, dscp, dp);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_outer1pPriRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_intPri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pPriRemap_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pPriRemap_get(unit, 0, 0, &temp_intPri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dot1pPri[UNITTEST_MAX_TEST_VALUE];
        int32  dot1pPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  dot1pPri_index;
        int32  dot1pPri_last;

        uint32 dei[UNITTEST_MAX_TEST_VALUE];
        int32  dei_result[UNITTEST_MAX_TEST_VALUE];
        int32  dei_index;
        int32  dei_last;

        uint32 intPri[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_index;
        int32  intPri_last;
        uint32  result_intPri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, dot1pPri, dot1pPri_result, dot1pPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DEI_MAX, TEST_DEI_MIN, dei, dei_result, dei_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, intPri, intPri_result, intPri_last);

        for (dot1pPri_index = 0; dot1pPri_index <= dot1pPri_last; dot1pPri_index++)
        {
            inter_result2 = dot1pPri_result[dot1pPri_index];
            for (dei_index = 0; dei_index <= dei_last; dei_index++)
            {
                inter_result3 = dei_result[dei_index];
                for (intPri_index = 0; intPri_index <= intPri_last; intPri_index++)
                {
                    inter_result4 = intPri_result[intPri_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_qos_outer1pPriRemap_set(unit, dot1pPri[dot1pPri_index], dei[dei_index], intPri[intPri_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_set (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                        , ret, expect_result, unit, dot1pPri[dot1pPri_index], dei[dei_index], intPri[intPri_index]);

                        ret = rtk_qos_outer1pPriRemap_get(unit, dot1pPri[dot1pPri_index], dei[dei_index], &result_intPri);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_get (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                         , ret, expect_result, unit, dot1pPri[dot1pPri_index], dei[dei_index], intPri[intPri_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_set/get value unequal (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                         , result_intPri, intPri[intPri_index], unit, dot1pPri[dot1pPri_index], dei[dei_index], intPri[intPri_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pPriRemap_set (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                            , ret, RT_ERR_OK, unit, dot1pPri[dot1pPri_index], dei[dei_index], intPri[intPri_index]);
                    }
                }

                ret = rtk_qos_outer1pPriRemap_get(unit, dot1pPri[dot1pPri_index], dei[dei_index], &result_intPri);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_get (unit %u, dot1pPri %u, dei %u)"
                                    , ret, expect_result, unit, dot1pPri[dot1pPri_index], dei[dei_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pPriRemap_get (unit %u, dot1pPri %u, dei %u)"
                                        , ret, RT_ERR_OK, unit, dot1pPri[dot1pPri_index], dei[dei_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dot1pPri;
        uint32  dot1pPri_max;
        uint32  dei;
        uint32  dei_max;
        uint32  intPri;
        uint32  intPri_max;
        uint32  result_intPri;


        G_VALUE(TEST_PRI_MAX(unit), dot1pPri_max);
        G_VALUE(TEST_DEI_MAX, dei_max);
        G_VALUE(TEST_PRI_MAX(unit), intPri_max);

        for (L_VALUE(TEST_PRI_MIN, dot1pPri); dot1pPri <= dot1pPri_max; dot1pPri++)
        {
            ASSIGN_EXPECT_RESULT(dot1pPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_DEI_MIN, dei); dei <= dei_max; dei++)
            {
                ASSIGN_EXPECT_RESULT(dei, TEST_DEI_MIN, TEST_DEI_MAX, inter_result3);

                for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
                {
                    ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_qos_outer1pPriRemap_set(unit, dot1pPri, dei, intPri);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_set (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                        , ret, expect_result, unit, dot1pPri, dei, intPri);
                        ret = rtk_qos_outer1pPriRemap_get(unit,  dot1pPri, dei, &result_intPri);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_get (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                         , ret, expect_result, unit, dot1pPri, dei, intPri);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pPriRemap_set/get value unequal (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                         , result_intPri, intPri, unit, dot1pPri, dei, intPri);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pPriRemap_set (unit %u, dot1pPri %u, dei %u, intPri %u)"
                                            , ret, RT_ERR_OK, unit, dot1pPri, dei, intPri);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_priMap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_pri2queue_t  temp_pri2qid;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int8  inter_result6 = 1;
    int8  inter_result7 = 1;
    int8  inter_result8 = 1;
    int8  inter_result9 = 1;
    int8  inter_result10 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_priMap_set(unit, 8, &temp_pri2qid))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_priMap_get(unit, 8, &temp_pri2qid)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 queue_num[UNITTEST_MAX_TEST_VALUE];
        int32  queue_num_result[UNITTEST_MAX_TEST_VALUE];
        int32  queue_num_index;
        int32  queue_num_last;

        uint32 pri0qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri0qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri0qid_index;
        int32  pri0qid_last;

        uint32 pri1qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri1qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri1qid_index;
        int32  pri1qid_last;

        uint32 pri2qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri2qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri2qid_index;
        int32  pri2qid_last;

        uint32 pri3qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri3qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri3qid_index;
        int32  pri3qid_last;

        uint32 pri4qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri4qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri4qid_index;
        int32  pri4qid_last;

        uint32 pri5qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri5qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri5qid_index;
        int32  pri5qid_last;

        uint32 pri6qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri6qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri6qid_index;
        int32  pri6qid_last;

        uint32 pri7qid[UNITTEST_MAX_TEST_VALUE];
        int32  pri7qid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri7qid_index;
        int32  pri7qid_last;

        rtk_qos_pri2queue_t pri2qid_entry;
        rtk_qos_pri2queue_t result_pri2qid_entry;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_NUM_MAX(unit), TEST_QUEUE_NUM_MIN, queue_num, queue_num_result, queue_num_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri0qid, pri0qid_result, pri0qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri1qid, pri1qid_result, pri1qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri2qid, pri2qid_result, pri2qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri3qid, pri3qid_result, pri3qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri4qid, pri4qid_result, pri4qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri5qid, pri5qid_result, pri5qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri6qid, pri6qid_result, pri6qid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), pri7qid, pri7qid_result, pri7qid_last);

        for (queue_num_index = 0; queue_num_index <= queue_num_last; queue_num_index++)
        {
            inter_result2 = queue_num_result[queue_num_index];
            for (pri0qid_index = 0; pri0qid_index <= pri0qid_last; pri0qid_index++)
            {
                inter_result3 = pri0qid_result[pri0qid_index];
                for (pri1qid_index = 0; pri1qid_index <= pri1qid_last; pri1qid_index++)
                {
                    inter_result4 = pri1qid_result[pri1qid_index];
                    for (pri2qid_index = 0; pri2qid_index <= pri2qid_last; pri2qid_index++)
                    {
                        inter_result5 = pri2qid_result[pri2qid_index];
                        for (pri3qid_index = 0; pri3qid_index <= pri3qid_last; pri3qid_index++)
                        {
                            inter_result6 = pri3qid_result[pri3qid_index];
                            for (pri4qid_index = 0; pri4qid_index <= pri4qid_last; pri4qid_index++)
                            {
                                inter_result7 = pri4qid_result[pri4qid_index];
                                for (pri5qid_index = 0; pri5qid_index <= pri5qid_last; pri5qid_index++)
                                {
                                    inter_result8 = pri5qid_result[pri5qid_index];
                                    for (pri6qid_index = 0; pri6qid_index <= pri6qid_last; pri6qid_index++)
                                    {
                                        inter_result9 = pri6qid_result[pri6qid_index];
                                        for (pri7qid_index = 0; pri7qid_index <= pri7qid_last; pri7qid_index++)
                                        {
                                            inter_result10 = pri7qid_result[pri7qid_index];
                                            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                                expect_result = RT_ERR_FAILED;
                                            else
                                                expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5 && inter_result6 && inter_result7 && inter_result8 && inter_result9 && inter_result10)? RT_ERR_OK : RT_ERR_FAILED;

                                            pri2qid_entry.pri2queue[0] = pri0qid[pri0qid_index];
                                            pri2qid_entry.pri2queue[1] = pri1qid[pri1qid_index];
                                            pri2qid_entry.pri2queue[2] = pri2qid[pri2qid_index];
                                            pri2qid_entry.pri2queue[3] = pri3qid[pri3qid_index];
                                            pri2qid_entry.pri2queue[4] = pri4qid[pri4qid_index];
                                            pri2qid_entry.pri2queue[5] = pri5qid[pri5qid_index];
                                            pri2qid_entry.pri2queue[6] = pri6qid[pri6qid_index];
                                            pri2qid_entry.pri2queue[7] = pri7qid[pri7qid_index];
                                            //osal_printf("unit=%u, qnum=%u, %u, %u, %u, %u, %u, %u, %u, %u\n\r",
                                            //    unit, queue_num[queue_num_index],
                                            //    pri0qid[pri0qid_index], pri1qid[pri1qid_index], pri2qid[pri2qid_index], pri3qid[pri3qid_index],
                                            //    pri4qid[pri4qid_index], pri5qid[pri5qid_index], pri6qid[pri6qid_index], pri7qid[pri7qid_index]);

                                            ret = rtk_qos_priMap_set(unit, queue_num[queue_num_index], &pri2qid_entry);
                                            if (RT_ERR_OK == expect_result)
                                            {
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set (unit %u, queue_num %u)"
                                                                , ret, expect_result, unit, queue_num[queue_num_index]);

                                                ret = rtk_qos_priMap_get(unit, queue_num[queue_num_index], &result_pri2qid_entry);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_get (unit %u, queue_num %u)"
                                                                 , ret, expect_result, unit, queue_num[queue_num_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[0] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[0] %u)"
                                                                 , result_pri2qid_entry.pri2queue[0], pri0qid[pri0qid_index], unit, queue_num[queue_num_index], pri0qid[pri0qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[1] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[1] %u)"
                                                                 , result_pri2qid_entry.pri2queue[1], pri1qid[pri1qid_index], unit, queue_num[queue_num_index], pri1qid[pri1qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[2] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[2] %u)"
                                                                 , result_pri2qid_entry.pri2queue[2], pri2qid[pri2qid_index], unit, queue_num[queue_num_index], pri2qid[pri2qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[3] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[3] %u)"
                                                                 , result_pri2qid_entry.pri2queue[3], pri3qid[pri3qid_index], unit, queue_num[queue_num_index], pri3qid[pri3qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[4] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[4] %u)"
                                                                 , result_pri2qid_entry.pri2queue[4], pri4qid[pri4qid_index], unit, queue_num[queue_num_index], pri4qid[pri4qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[5] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[5] %u)"
                                                                 , result_pri2qid_entry.pri2queue[5], pri5qid[pri5qid_index], unit, queue_num[queue_num_index], pri5qid[pri5qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[6] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[6] %u)"
                                                                 , result_pri2qid_entry.pri2queue[6], pri6qid[pri6qid_index], unit, queue_num[queue_num_index], pri6qid[pri6qid_index]);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[7] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[7] %u)"
                                                                 , result_pri2qid_entry.pri2queue[7], pri7qid[pri7qid_index], unit, queue_num[queue_num_index], pri7qid[pri7qid_index]);
                                            }
                                            else
                                            {
                                                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priMap_set (unit %u, queue_num %u, pri2qid_entry.pri2queue[0] %u, pri2qid_entry.pri2queue[1] %u, pri2qid_entry.pri2queue[2] %u, pri2qid_entry.pri2queue[3] %u, pri2qid_entry.pri2queue[4] %u, pri2qid_entry.pri2queue[5] %u, pri2qid_entry.pri2queue[6] %u, , pri2qid_entry.pri2queue[7] %u)", ret, RT_ERR_OK, unit, queue_num[queue_num_index],
                                                                          pri0qid[pri0qid_index], pri1qid[pri1qid_index], pri2qid[pri2qid_index], pri3qid[pri3qid_index],
                                                                          pri4qid[pri4qid_index], pri5qid[pri5qid_index],
                                                                          pri6qid[pri6qid_index], pri7qid[pri7qid_index]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ret = rtk_qos_priMap_get(unit, queue_num[queue_num_index], &result_pri2qid_entry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_get (unit %u, queue_num %u)"
                                , ret, expect_result, unit, queue_num[queue_num_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priMap_get (unit %u, queue_num %u)"
                                    , ret, RT_ERR_OK, unit, queue_num[queue_num_index]);
            }
        }
    }


//#if 0 /* run too long time */
    if (IS_TEST_SCAN_MODE())
    {
        uint32  queue_num;
        uint32  queue_num_max;
        uint32  pri0qid;
        uint32  pri0qid_max;
        uint32  pri1qid;
        uint32  pri1qid_max;
        uint32  pri2qid;
        uint32  pri2qid_max;
        uint32  pri3qid;
        uint32  pri3qid_max;
        uint32  pri4qid;
        uint32  pri4qid_max;
        uint32  pri5qid;
        uint32  pri5qid_max;
        uint32  pri6qid;
        uint32  pri6qid_max;
        uint32  pri7qid;
        uint32  pri7qid_max;
        rtk_qos_pri2queue_t pri2qid_entry;
        rtk_qos_pri2queue_t result_pri2qid_entry;


        G_VALUE(TEST_QUEUE_NUM_MAX(unit), queue_num_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri0qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri1qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri2qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri3qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri4qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri5qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri6qid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), pri7qid_max);

        for (L_VALUE(TEST_QUEUE_NUM_MIN, queue_num); queue_num <= queue_num_max; queue_num++)
        {
            ASSIGN_EXPECT_RESULT(queue_num, TEST_QUEUE_NUM_MIN, TEST_QUEUE_NUM_MAX(unit), inter_result2);
            for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri0qid); pri0qid <= pri0qid_max; pri0qid++)
            {
                ASSIGN_EXPECT_RESULT(pri0qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result3);
                for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri1qid); pri1qid <= pri1qid_max; pri1qid++)
                {
                    ASSIGN_EXPECT_RESULT(pri1qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result4);
                    for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri2qid); pri2qid <= pri2qid_max; pri2qid++)
                    {
                        ASSIGN_EXPECT_RESULT(pri2qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result5);
                        for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri3qid); pri3qid <= pri3qid_max; pri3qid++)
                        {
                            ASSIGN_EXPECT_RESULT(pri3qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result6);
                            for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri4qid); pri4qid <= pri4qid_max; pri4qid++)
                            {
                                ASSIGN_EXPECT_RESULT(pri4qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result7);
                                for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri5qid); pri5qid <= pri5qid_max; pri5qid++)
                                {
                                    ASSIGN_EXPECT_RESULT(pri5qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result8);
                                    for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri6qid); pri6qid <= pri6qid_max; pri6qid++)
                                    {
                                        ASSIGN_EXPECT_RESULT(pri6qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result9);
                                        for (L_VALUE(TEST_QUEUE_ID_MIN(unit), pri7qid); pri7qid <= pri7qid_max; pri7qid++)
                                        {
                                            ASSIGN_EXPECT_RESULT(pri7qid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result10);

                                            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                                expect_result = RT_ERR_FAILED;
                                            else
                                                expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5 && inter_result6 && inter_result7 && inter_result8 && inter_result9 && inter_result10)? RT_ERR_OK : RT_ERR_FAILED;

                                            pri2qid_entry.pri2queue[0] = pri0qid;
                                            pri2qid_entry.pri2queue[1] = pri1qid;
                                            pri2qid_entry.pri2queue[2] = pri2qid;
                                            pri2qid_entry.pri2queue[3] = pri3qid;
                                            pri2qid_entry.pri2queue[4] = pri4qid;
                                            pri2qid_entry.pri2queue[5] = pri5qid;
                                            pri2qid_entry.pri2queue[6] = pri6qid;
                                            pri2qid_entry.pri2queue[7] = pri7qid;
                                            ret = rtk_qos_priMap_set(unit, queue_num, &pri2qid_entry);
                                            if (expect_result == RT_ERR_OK)
                                            {
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set (unit %u, queue_num %u)"
                                                                , ret, expect_result, unit, queue_num);

                                                ret = rtk_qos_priMap_get(unit, queue_num, &result_pri2qid_entry);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_get (unit %u, queue_num %u)"
                                                                 , ret, expect_result, unit, queue_num);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[0] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[0] %u)"
                                                                 , result_pri2qid_entry.pri2queue[0], pri0qid, unit, queue_num, pri0qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[1] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[1] %u)"
                                                                 , result_pri2qid_entry.pri2queue[1], pri1qid, unit, queue_num, pri1qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[2] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[2] %u)"
                                                                 , result_pri2qid_entry.pri2queue[2], pri2qid, unit, queue_num, pri2qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[3] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[3] %u)"
                                                                 , result_pri2qid_entry.pri2queue[3], pri3qid, unit, queue_num, pri3qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[4] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[4] %u)"
                                                                 , result_pri2qid_entry.pri2queue[4], pri4qid, unit, queue_num, pri4qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[5] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[5] %u)"
                                                                 , result_pri2qid_entry.pri2queue[5], pri5qid, unit, queue_num, pri5qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[6] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[6] %u)"
                                                                 , result_pri2qid_entry.pri2queue[6], pri6qid, unit, queue_num, pri6qid);
                                                RT_TEST_IS_EQUAL_INT("rtk_qos_priMap_set/get pri2queue[7] value unequal (unit %u, queue_num %u, pri2qid_entry.pri2queue[7] %u)"
                                                                 , result_pri2qid_entry.pri2queue[7], pri7qid, unit, queue_num, pri7qid);
                                            }
                                            else
                                                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_priMap_set (unit %u, queue_num %u, pri2qid_entry.pri2queue[0] %u, pri2qid_entry.pri2queue[1] %u, pri2qid_entry.pri2queue[2] %u, pri2qid_entry.pri2queue[3] %u, pri2qid_entry.pri2queue[4] %u, pri2qid_entry.pri2queue[5] %u, pri2qid_entry.pri2queue[6] %u, , pri2qid_entry.pri2queue[7] %u)", ret, RT_ERR_OK, unit, queue_num,
                                                                          pri0qid, pri1qid, pri2qid, pri3qid, pri4qid, pri5qid, pri6qid, pri7qid);
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
//#endif

    return RT_ERR_OK;
}

int32 dal_qos_1pDfltPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_pri;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pDfltPri_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pDfltPri_get(unit, &temp_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;
        uint32  result_pri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, pri, pri_result, pri_last);

        for (pri_index = 0; pri_index <= pri_last; pri_index++)
        {
            inter_result2 = pri_result[pri_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_1pDfltPri_set(unit, pri[pri_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri[pri_index]);

                ret = rtk_qos_1pDfltPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri[pri_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri[pri_index], unit, pri[pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pDfltPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri[pri_index]);
            }
        }

        ret = rtk_qos_1pDfltPri_get(unit, &result_pri);

        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pDfltPri_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t   pri;
        rtk_pri_t   pri_max;
        rtk_pri_t   result_pri;


        G_VALUE(TEST_PRI_MAX(unit), pri_max);

        for (L_VALUE(TEST_PRI_MIN, pri); pri <= pri_max; pri++)
        {
            ASSIGN_EXPECT_RESULT(pri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_1pDfltPri_set(unit, pri);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri);
                ret = rtk_qos_1pDfltPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri, unit, pri);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pDfltPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_1pRemarkSrcSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_1pRmkSrc_t  temp_1pRmkSrc;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pRemarkSrcSel_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pRemarkSrcSel_get(unit, &temp_1pRmkSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_qos_1pRmkSrc_t  dot1pRmkSrc[UNITTEST_MAX_TEST_VALUE];
        int32  dot1pRmkSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  dot1pRmkSrc_index;
        int32  dot1pRmkSrc_last;
        rtk_qos_1pRmkSrc_t  result_dot1pRmkSrc;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_DOT1PRMKSRC_MAX(unit), TEST_DOT1PRMKSRC_MIN, dot1pRmkSrc, dot1pRmkSrc_result, dot1pRmkSrc_last);

        for (dot1pRmkSrc_index = 0; dot1pRmkSrc_index <= dot1pRmkSrc_last; dot1pRmkSrc_index++)
        {
            inter_result2 = dot1pRmkSrc_result[dot1pRmkSrc_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_1pRemarkSrcSel_set(unit, dot1pRmkSrc[dot1pRmkSrc_index]);

            if ((HWP_8380_30_FAMILY(unit))
                && (dot1pRmkSrc[dot1pRmkSrc_index] != DOT_1P_RMK_SRC_INT_PRI)
                && (dot1pRmkSrc[dot1pRmkSrc_index] != DOT_1P_RMK_SRC_USER_PRI))
                expect_result = RT_ERR_FAILED;
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set (unit %u, dot1pRmkSrc %u)"
                                , ret, expect_result, unit, dot1pRmkSrc[dot1pRmkSrc_index]);

                ret = rtk_qos_1pRemarkSrcSel_get(unit, &result_dot1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_get (unit %u, dot1pRmkSrc %u)"
                                 , ret, expect_result, unit, dot1pRmkSrc[dot1pRmkSrc_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set/get value unequal (unit %u, dot1pRmkSrc %u)"
                                 , result_dot1pRmkSrc, dot1pRmkSrc[dot1pRmkSrc_index], unit, dot1pRmkSrc[dot1pRmkSrc_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set (unit %u, dot1pRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, dot1pRmkSrc[dot1pRmkSrc_index]);
            }
        }

        ret = rtk_qos_1pRemarkSrcSel_get(unit, &result_dot1pRmkSrc);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemarkSrcSel_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_qos_1pRmkSrc_t  dot1pRmkSrc;
        uint32  dot1pRmkSrc_max;
        rtk_qos_1pRmkSrc_t  result_dot1pRmkSrc;


        G_VALUE(TEST_DOT1PRMKSRC_MAX(unit), dot1pRmkSrc_max);

        for (L_VALUE(TEST_DOT1PRMKSRC_MIN, dot1pRmkSrc); dot1pRmkSrc <= dot1pRmkSrc_max; dot1pRmkSrc++)
        {
            ASSIGN_EXPECT_RESULT(dot1pRmkSrc, TEST_DOT1PRMKSRC_MIN, TEST_DOT1PRMKSRC_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((HWP_8380_30_FAMILY(unit))
                && (dot1pRmkSrc != DOT_1P_RMK_SRC_INT_PRI)
                && (dot1pRmkSrc != DOT_1P_RMK_SRC_USER_PRI))
                expect_result = RT_ERR_FAILED;
            ret = rtk_qos_1pRemarkSrcSel_set(unit, dot1pRmkSrc);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set (unit %u, dot1pRmkSrc %u)"
                                , ret, expect_result, unit, dot1pRmkSrc);
                ret = rtk_qos_1pRemarkSrcSel_get(unit, &result_dot1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_get (unit %u, dot1pRmkSrc %u)"
                                 , ret, expect_result, unit, dot1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set/get value unequal (unit %u, dot1pRmkSrc %u)"
                                 , result_dot1pRmkSrc, dot1pRmkSrc, unit, dot1pRmkSrc);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pRemarkSrcSel_set (unit %u, dot1pRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, dot1pRmkSrc);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_outer1pRemark_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_out1pPri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pRemark_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pRemark_get(unit, 0, &temp_out1pPri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 intPri[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_index;
        int32  intPri_last;

        rtk_pri_t out1pPri[UNITTEST_MAX_TEST_VALUE];
        int32  out1pPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  out1pPri_index;
        int32  out1pPri_last;

        rtk_pri_t  result_out1pPri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, intPri, intPri_result, intPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, out1pPri, out1pPri_result, out1pPri_last);

        for (intPri_index = 0; intPri_index <= intPri_last; intPri_index++)
        {
            inter_result2 = intPri_result[intPri_index];
            for (out1pPri_index = 0; out1pPri_index <= out1pPri_last; out1pPri_index++)
            {
                inter_result3 = out1pPri_result[out1pPri_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_outer1pRemark_set(unit, intPri[intPri_index], out1pPri[out1pPri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_set (unit %u, intPri %u, out1pPri %u)"
                                    , ret, expect_result, unit, intPri[intPri_index], out1pPri[out1pPri_index]);

                    ret = rtk_qos_outer1pRemark_get(unit, intPri[intPri_index], &result_out1pPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_get (unit %u, intPri %u, out1pPri %u)"
                                     , ret, expect_result, unit, intPri[intPri_index], out1pPri[out1pPri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_set/get value unequal (unit %u, intPri %u, out1pPri %u)"
                                     , result_out1pPri, out1pPri[out1pPri_index], unit, intPri[intPri_index], out1pPri[out1pPri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemark_set (unit %u intPri %u, out1pPri %u,)"
                                        , ret, RT_ERR_OK, unit, intPri[intPri_index], out1pPri[out1pPri_index]);
                }
            }

            ret = rtk_qos_outer1pRemark_get(unit, intPri[intPri_index], &result_out1pPri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_get (unit %u, intPri %u)"
                                , ret, expect_result, unit, intPri[intPri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemark_get (unit %u, intPri %u)"
                                    , ret, RT_ERR_OK, unit, intPri[intPri_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  intPri;
        uint32  intPri_max;
        uint32  out1pPri;
        uint32  out1pPri_max;
        uint32  result_out1pPri;


        G_VALUE(TEST_PRI_MAX(unit), intPri_max);
        G_VALUE(TEST_PRI_MAX(unit), out1pPri_max);

        for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
        {
            ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PRI_MIN, out1pPri); out1pPri <= out1pPri_max; out1pPri++)
            {
                ASSIGN_EXPECT_RESULT(out1pPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                            expect_result = RT_ERR_FAILED;
                        else
                            expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_outer1pRemark_set(unit, intPri, out1pPri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_set (unit %u, intPri %u, out1pPri %u)"
                                    , ret, expect_result, unit, intPri, out1pPri);
                    ret = rtk_qos_outer1pRemark_get(unit,  intPri, &result_out1pPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_get (unit %u, intPri %u, out1pPri %u)"
                                     , ret, expect_result, unit, intPri, out1pPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemark_set/get value unequal (unit %u, intPri %u, out1pPri %u)"
                                     , result_out1pPri, out1pPri, unit, intPri, out1pPri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemark_set (unit %u, intPri %u, out1pPri %u)"
                                        , ret, RT_ERR_OK, unit, intPri, out1pPri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_outer1pRemarkSrcSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_outer1pRmkSrc_t  temp_out1pRmkSrc;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pRemarkSrcSel_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pRemarkSrcSel_get(unit, &temp_out1pRmkSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_qos_outer1pRmkSrc_t  out1pRmkSrc[UNITTEST_MAX_TEST_VALUE];
        int32  out1pRmkSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  out1pRmkSrc_index;
        int32  out1pRmkSrc_last;

        rtk_qos_outer1pRmkSrc_t  result_out1pRmkSrc;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_OUT1PRMKSRC_MAX, TEST_OUT1PRMKSRC_MIN, out1pRmkSrc, out1pRmkSrc_result, out1pRmkSrc_last);

        for (out1pRmkSrc_index = 0; out1pRmkSrc_index <= out1pRmkSrc_last; out1pRmkSrc_index++)
        {
            inter_result2 = out1pRmkSrc_result[out1pRmkSrc_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_qos_outer1pRemarkSrcSel_set(unit, out1pRmkSrc[out1pRmkSrc_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set (unit %u, out1pRmkSrc %u)"
                                , ret, expect_result, unit, out1pRmkSrc[out1pRmkSrc_index]);

                ret = rtk_qos_outer1pRemarkSrcSel_get(unit, &result_out1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_get (unit %u, out1pRmkSrc %u)"
                                 , ret, expect_result, unit, out1pRmkSrc[out1pRmkSrc_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set/get value unequal (unit %u, out1pRmkSrc %u)"
                                 , result_out1pRmkSrc, out1pRmkSrc[out1pRmkSrc_index], unit, out1pRmkSrc[out1pRmkSrc_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set (unit %u, out1pRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, out1pRmkSrc[out1pRmkSrc_index]);
            }
        }

        ret = rtk_qos_outer1pRemarkSrcSel_get(unit, &result_out1pRmkSrc);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_qos_outer1pRmkSrc_t  out1pRmkSrc;
        uint32  out1pRmkSrc_max;
        rtk_qos_outer1pRmkSrc_t  result_out1pRmkSrc;


        G_VALUE(TEST_OUT1PRMKSRC_MAX, out1pRmkSrc_max);

        for (L_VALUE(TEST_OUT1PRMKSRC_MIN, out1pRmkSrc); out1pRmkSrc <= out1pRmkSrc_max; out1pRmkSrc++)
        {
            ASSIGN_EXPECT_RESULT(out1pRmkSrc, TEST_OUT1PRMKSRC_MIN, TEST_OUT1PRMKSRC_MAX, inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_qos_outer1pRemarkSrcSel_set(unit, out1pRmkSrc);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set (unit %u, out1pRmkSrc %u)"
                                , ret, expect_result, unit, out1pRmkSrc);
                ret = rtk_qos_outer1pRemarkSrcSel_get(unit, &result_out1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_get (unit %u, out1pRmkSrc %u)"
                                 , ret, expect_result, unit, out1pRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set/get value unequal (unit %u, out1pRmkSrc %u)"
                                 , result_out1pRmkSrc, out1pRmkSrc, unit, out1pRmkSrc);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pRemarkSrcSel_set (unit %u, out1pRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, out1pRmkSrc);
        }

    }

    return RT_ERR_OK;
}

int32 dal_qos_portOuter1pDfltPriSrcSel_test(uint32 caseNo, uint32 unit)
{
    rt_error_common_t outer1pDflt_PriSrcSel[PRI_SRC_END+1]={RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,
                                                            RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,
                                                            RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED};
    int32 ret;

    rtk_qos_outer1pDfltSrc_t  temp_out1pDfltPriSrc;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuter1pDfltPriSrcSel_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuter1pDfltPriSrcSel_get(unit, 0, &temp_out1pDfltPriSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    if (HWP_8380_30_FAMILY(unit))
    {
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_USER_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_PB_OUTER_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
    }
    if (HWP_8390_50_FAMILY(unit))
    {
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_USER_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_PB_PRI] = RT_ERR_OK;

    }
    if (HWP_9300_FAMILY_ID(unit))
    {
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
        outer1pDflt_PriSrcSel[OUTER_1P_DFLT_SRC_USER_PRI] = RT_ERR_OK;
    }
    if (HWP_9310_FAMILY_ID(unit))
    {

    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_outer1pDfltSrc_t  out1pDfltPriSrc[UNITTEST_MAX_TEST_VALUE];
        int32  out1pDfltPriSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  out1pDfltPriSrc_index;
        int32  out1pDfltPriSrc_last;

        rtk_qos_outer1pDfltSrc_t  result_out1pDfltPriSrc;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_OUT1PDFLTPRISRC_MAX(unit), TEST_OUT1PDFLTPRISRC_MIN, out1pDfltPriSrc, out1pDfltPriSrc_result, out1pDfltPriSrc_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (out1pDfltPriSrc_index = 0; out1pDfltPriSrc_index <= out1pDfltPriSrc_last; out1pDfltPriSrc_index++)
            {
                inter_result3 = out1pDfltPriSrc_result[out1pDfltPriSrc_index];
                if((RT_ERR_OK == outer1pDflt_PriSrcSel[out1pDfltPriSrc[out1pDfltPriSrc_index]]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port[port_index], out1pDfltPriSrc[out1pDfltPriSrc_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, port %u, out1pDfltPriSrc %u)"
                                    , ret, expect_result, unit, port[port_index], out1pDfltPriSrc[out1pDfltPriSrc_index]);

                    ret = rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port[port_index], &result_out1pDfltPriSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_get (unit %u, port %u, out1pDfltPriSrc %u)"
                                     , ret, expect_result, unit, port[port_index], out1pDfltPriSrc[out1pDfltPriSrc_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set/get value unequal (unit %u, port %u, out1pDfltPriSrc %u)"
                                     , result_out1pDfltPriSrc, out1pDfltPriSrc[out1pDfltPriSrc_index], unit, port[port_index], out1pDfltPriSrc[out1pDfltPriSrc_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, port %u, out1pDfltPriSrc %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], out1pDfltPriSrc[out1pDfltPriSrc_index]);
                }
            }

            ret = rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port[port_index], &result_out1pDfltPriSrc);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        uint32  port_max;
        rtk_qos_outer1pDfltSrc_t  out1pDfltPriSrc;
        uint32  out1pDfltPriSrc_max;
        rtk_qos_outer1pDfltSrc_t  result_out1pDfltPriSrc;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_OUT1PDFLTPRISRC_MAX(unit), out1pDfltPriSrc_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_OUT1PDFLTPRISRC_MIN, out1pDfltPriSrc); out1pDfltPriSrc <= out1pDfltPriSrc_max; out1pDfltPriSrc++)
            {
                ASSIGN_EXPECT_RESULT(out1pDfltPriSrc, TEST_OUT1PDFLTPRISRC_MIN, TEST_OUT1PDFLTPRISRC_MAX(unit), inter_result3);

                if((RT_ERR_OK == outer1pDflt_PriSrcSel[out1pDfltPriSrc]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOuter1pDfltPriSrcSel_set(unit, port, out1pDfltPriSrc);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, port %u, out1pDfltPriSrc %u)"
                                    , ret, expect_result, unit, port, out1pDfltPriSrc);
                    ret = rtk_qos_portOuter1pDfltPriSrcSel_get(unit, port, &result_out1pDfltPriSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_get (unit %u, port %u, out1pDfltPriSrc %u)"
                                     , ret, expect_result, unit, port, out1pDfltPriSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set/get value unequal (unit %u, port %u, out1pDfltPriSrc %u)"
                                     , result_out1pDfltPriSrc, out1pDfltPriSrc, unit, port, out1pDfltPriSrc);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, port %u, out1pDfltPriSrc %u)"
                                        , ret, RT_ERR_OK, unit, port, out1pDfltPriSrc);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dscp2DscpRemark_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_dscp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscp2DscpRemark_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscp2DscpRemark_get(unit, 0, &temp_dscp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dscp[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dscp_index;
        int32  dscp_last;

        uint32 remarkDscp[UNITTEST_MAX_TEST_VALUE];
        int32  remarkDscp_result[UNITTEST_MAX_TEST_VALUE];
        int32  remarkDscp_index;
        int32  remarkDscp_last;
        uint32  result_remarkDscp;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DSCP_MAX, TEST_DSCP_MIN, dscp, dscp_result, dscp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DSCP_MAX, TEST_DSCP_MIN, remarkDscp, remarkDscp_result, remarkDscp_last);

        for (dscp_index = 0; dscp_index <= dscp_last; dscp_index++)
        {
            inter_result2 = dscp_result[dscp_index];
            for (remarkDscp_index = 0; remarkDscp_index <= remarkDscp_last; remarkDscp_index++)
            {
                inter_result3 = remarkDscp_result[remarkDscp_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscp2DscpRemark_set(unit, dscp[dscp_index], remarkDscp[remarkDscp_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_set (unit %u, dscp %u, remarkDscp %u)"
                                    , ret, expect_result, unit, dscp[dscp_index], remarkDscp[remarkDscp_index]);

                    ret = rtk_qos_dscp2DscpRemark_get(unit, dscp[dscp_index], &result_remarkDscp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_get (unit %u, dscp %u, remarkDscp %u)"
                                     , ret, expect_result, unit, dscp[dscp_index], remarkDscp[remarkDscp_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_set/get value unequal (unit %u, dscp %u, remarkDscp %u)"
                                     , result_remarkDscp, remarkDscp[remarkDscp_index], unit, dscp[dscp_index], remarkDscp[remarkDscp_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscp2DscpRemark_set (unit %u, dscp %u, remarkDscp %u)"
                                        , ret, RT_ERR_OK, unit, dscp[dscp_index], remarkDscp[remarkDscp_index]);
                }
            }

            ret = rtk_qos_dscp2DscpRemark_get(unit, dscp[dscp_index], &result_remarkDscp);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_get (unit %u, dscp %u)"
                                , ret, expect_result, unit, dscp[dscp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscp2DscpRemark_get (unit %u, dscp %u)"
                                    , ret, RT_ERR_OK, unit, dscp[dscp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dscp;
        uint32  dscp_max;
        uint32  remarkDscp;
        uint32  remarkDscp_max;
        uint32  result_remarkDscp;


        G_VALUE(TEST_DSCP_MAX, dscp_max);
        G_VALUE(TEST_DSCP_MAX, remarkDscp_max);

        for (L_VALUE(TEST_DSCP_MIN, dscp); dscp <= dscp_max; dscp++)
        {
            ASSIGN_EXPECT_RESULT(dscp, TEST_DSCP_MIN, TEST_DSCP_MAX, inter_result2);

            for (L_VALUE(TEST_DSCP_MIN, remarkDscp); remarkDscp <= remarkDscp_max; remarkDscp++)
            {
                ASSIGN_EXPECT_RESULT(remarkDscp, TEST_DSCP_MIN, TEST_DSCP_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_dscp2DscpRemark_set(unit, dscp, remarkDscp);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_set (unit %u, dscp %u, remarkDscp %u)"
                                    , ret, expect_result, unit, dscp, remarkDscp);
                    ret = rtk_qos_dscp2DscpRemark_get(unit,  dscp, &result_remarkDscp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_get (unit %u, dscp %u, remarkDscp %u)"
                                     , ret, expect_result, unit, dscp, remarkDscp);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_dscp2DscpRemark_set/get value unequal (unit %u, dscp %u, remarkDscp %u)"
                                     , result_remarkDscp, remarkDscp, unit, dscp, remarkDscp);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscp2DscpRemark_set (unit %u, dscp %u, remarkDscp %u)"
                                        , ret, RT_ERR_OK, unit, dscp, remarkDscp);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_dscpRemarkSrcSel_test(uint32 caseNo, uint32 unit)
{

    rt_error_common_t dscp_RMKSrcSel[PRI_SRC_END+1] = {RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,
                                                       RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,
                                                       RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED,RT_ERR_FAILED};

    int32 ret;
    rtk_qos_dscpRmkSrc_t  temp_dscpRmkSrc;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    if (HWP_8380_30_FAMILY(unit))
    {
        dscp_RMKSrcSel[DSCP_RMK_SRC_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DSCP] = RT_ERR_OK;
    }
    if (HWP_8390_50_FAMILY(unit))
    {
        dscp_RMKSrcSel[DSCP_RMK_SRC_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_OUTER_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DSCP] = RT_ERR_OK;

    }
    if (HWP_9300_FAMILY_ID(unit))
    {
        dscp_RMKSrcSel[DSCP_RMK_SRC_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_OUTER_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DP_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DSCP] = RT_ERR_OK;
    }
    if (HWP_9310_FAMILY_ID(unit))
    {
        dscp_RMKSrcSel[DSCP_RMK_SRC_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_OUTER_USER_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DP_INT_PRI] = RT_ERR_OK;
        dscp_RMKSrcSel[DSCP_RMK_SRC_DSCP] = RT_ERR_OK;

    }

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpRemarkSrcSel_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_dscpRemarkSrcSel_get(unit, &temp_dscpRmkSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_qos_dscpRmkSrc_t  dscpRmkSrc[UNITTEST_MAX_TEST_VALUE];
        int32  dscpRmkSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  dscpRmkSrc_index;
        int32  dscpRmkSrc_last;

        rtk_qos_dscpRmkSrc_t  result_dscpRmkSrc;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DSCPRMKSRC_MAX, TEST_DSCPRMKSRC_MIN, dscpRmkSrc, dscpRmkSrc_result, dscpRmkSrc_last);

        for (dscpRmkSrc_index = 0; dscpRmkSrc_index <= dscpRmkSrc_last; dscpRmkSrc_index++)
        {
            inter_result2 = dscpRmkSrc_result[dscpRmkSrc_index];


            if(RT_ERR_OK == dscp_RMKSrcSel[dscpRmkSrc[dscpRmkSrc_index]])
                inter_result2 = 1;
            else
                inter_result2 = 0;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((dscpRmkSrc[dscpRmkSrc_index] != DSCP_RMK_SRC_INT_PRI) && (dscpRmkSrc[dscpRmkSrc_index] != DSCP_RMK_SRC_DSCP) &&
                (dscpRmkSrc[dscpRmkSrc_index] != DSCP_RMK_SRC_DP) && (dscpRmkSrc[dscpRmkSrc_index] != DSCP_RMK_SRC_USER_PRI) &&
                (dscpRmkSrc[dscpRmkSrc_index] != DSCP_RMK_SRC_OUTER_USER_PRI))
            {
                expect_result = RT_ERR_FAILED;
            }
            ret = rtk_qos_dscpRemarkSrcSel_set(unit, dscpRmkSrc[dscpRmkSrc_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set (unit %u, dscpRmkSrc %u)"
                                , ret, expect_result, unit, dscpRmkSrc[dscpRmkSrc_index]);

                ret = rtk_qos_dscpRemarkSrcSel_get(unit, &result_dscpRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_get (unit %u, dscpRmkSrc %u)"
                                 , ret, expect_result, unit, dscpRmkSrc[dscpRmkSrc_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set/get value unequal (unit %u, dscpRmkSrc %u)"
                                 , result_dscpRmkSrc, dscpRmkSrc[dscpRmkSrc_index], unit, dscpRmkSrc[dscpRmkSrc_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set (unit %u, dscpRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, dscpRmkSrc[dscpRmkSrc_index]);
            }
        }

        ret = rtk_qos_dscpRemarkSrcSel_get(unit, &result_dscpRmkSrc);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_qos_dscpRmkSrc_t  dscpRmkSrc;
        uint32  dscpRmkSrc_max;
        rtk_qos_dscpRmkSrc_t  result_dscpRmkSrc;


        G_VALUE(TEST_DSCPRMKSRC_MAX, dscpRmkSrc_max);

        for (L_VALUE(TEST_DSCPRMKSRC_MIN, dscpRmkSrc); dscpRmkSrc <= dscpRmkSrc_max; dscpRmkSrc++)
        {
            ASSIGN_EXPECT_RESULT(dscpRmkSrc, TEST_DSCPRMKSRC_MIN, TEST_DSCPRMKSRC_MAX, inter_result2);

            if(RT_ERR_OK == dscp_RMKSrcSel[dscpRmkSrc])
                inter_result2 = 1;
            else
                inter_result2 = 0;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((dscpRmkSrc != DSCP_RMK_SRC_INT_PRI) && (dscpRmkSrc != DSCP_RMK_SRC_DSCP) &&
                (dscpRmkSrc != DSCP_RMK_SRC_DP) && (dscpRmkSrc != DSCP_RMK_SRC_USER_PRI) &&
                (dscpRmkSrc != DSCP_RMK_SRC_OUTER_USER_PRI))
            {
                expect_result = RT_ERR_FAILED;
            }
            ret = rtk_qos_dscpRemarkSrcSel_set(unit, dscpRmkSrc);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set (unit %u, dscpRmkSrc %u)"
                                , ret, expect_result, unit, dscpRmkSrc);
                ret = rtk_qos_dscpRemarkSrcSel_get(unit, &result_dscpRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_get (unit %u, dscpRmkSrc %u)"
                                 , ret, expect_result, unit, dscpRmkSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set/get value unequal (unit %u, dscpRmkSrc %u)"
                                 , result_dscpRmkSrc, dscpRmkSrc, unit, dscpRmkSrc);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_dscpRemarkSrcSel_set (unit %u, dscpRmkSrc %u)"
                                    , ret, RT_ERR_OK, unit, dscpRmkSrc);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_deiRemark_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_dp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_deiRemark_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_deiRemark_get(unit, 0, &temp_dp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;

        uint32 dei[UNITTEST_MAX_TEST_VALUE];
        int32  dei_result[UNITTEST_MAX_TEST_VALUE];
        int32  dei_index;
        int32  dei_last;
        uint32  result_dei;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DEI_MAX, TEST_DEI_MIN, dei, dei_result, dei_last);

        for (dp_index = 0; dp_index <= dp_last; dp_index++)
        {
            inter_result2 = dp_result[dp_index];
            for (dei_index = 0; dei_index <= dei_last; dei_index++)
            {
                inter_result3 = dei_result[dei_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_deiRemark_set(unit, dp[dp_index], dei[dei_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_set (unit %u, dp %u, dei %u)"
                                    , ret, expect_result, unit, dp[dp_index], dei[dei_index]);

                    ret = rtk_qos_deiRemark_get(unit, dp[dp_index], &result_dei);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_get (unit %u, dp %u, dei %u)"
                                     , ret, expect_result, unit, dp[dp_index], dei[dei_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_set/get value unequal (unit %u, dp %u, dei %u)"
                                     , result_dei, dei[dei_index], unit, dp[dp_index], dei[dei_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiRemark_set (unit %u, dp %u, dei %u)"
                                        , ret, RT_ERR_OK, unit, dp[dp_index], dei[dei_index]);
                }
            }

            ret = rtk_qos_deiRemark_get(unit, dp[dp_index], &result_dei);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_get (unit %u, dp %u)"
                                , ret, expect_result, unit, dp[dp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiRemark_get (unit %u, dp %u)"
                                    , ret, RT_ERR_OK, unit, dp[dp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dp;
        uint32  dp_max;
        uint32  dei;
        uint32  dei_max;
        uint32  result_dei;


        G_VALUE(TEST_DP_MAX, dp_max);
        G_VALUE(TEST_DEI_MAX, dei_max);

        for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
        {
            ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result2);

            for (L_VALUE(TEST_DEI_MIN, dei); dei <= dei_max; dei++)
            {
                ASSIGN_EXPECT_RESULT(dei, TEST_DEI_MIN, TEST_DEI_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_deiRemark_set(unit, dp, dei);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_set (unit %u, dp %u, dei %u)"
                                    , ret, expect_result, unit, dp, dei);
                    ret = rtk_qos_deiRemark_get(unit, dp, &result_dei);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_get (unit %u, dp %u, dei %u)"
                                     , ret, expect_result, unit, dp, dei);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_deiRemark_set/get value unequal (unit %u, dp %u, dei %u)"
                                     , result_dei, dei, unit, dp, dei);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_deiRemark_set (unit %u, dp %u, dei %u)"
                                        , ret, RT_ERR_OK, unit, dp, dei);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_deiRemarkEnable_test(uint32 caseNo, uint32 unit)
{
    int32   ret;
    uint32  enable_temp;
    int8    inter_result2 = 1;
    int8    inter_result3 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDeiRemarkEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOut1pRemarkEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
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
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (enable_index = 0; enable_index <= enable_last; enable_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDeiRemarkEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_qos_portDeiRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_qos_portOut1pRemarkEnable_get(unit, port[port_index], (uint32*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOut1pRemarkEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_enable_t  enable;
        int32  enable_max;

        rtk_enable_t  result_enable;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDeiRemarkEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_qos_portDeiRemarkEnable_get(unit,  port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDeiRemarkEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_portDeiRemarkTagSel_test(uint32 caseNo, uint32 unit)
{
    int32   ret;
    rtk_qos_deiSel_t  temp_deiSel;
    int8    inter_result2 = 1;
    int8    inter_result3 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDeiRemarkTagSel_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portDeiRemarkTagSel_get(unit, 0, &temp_deiSel)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_deiSel_t  deiSel[UNITTEST_MAX_TEST_VALUE];
        int32  deiSel_result[UNITTEST_MAX_TEST_VALUE];
        int32  deiSel_index;
        int32  deiSel_last;
        rtk_qos_deiSel_t  result_deiSel;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_DEIREMARKTAGSEL_MAX, TEST_DEIREMARKTAGSEL_MIN, deiSel, deiSel_result, deiSel_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (deiSel_index = 0; deiSel_index <= deiSel_last; deiSel_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && deiSel_result[deiSel_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDeiRemarkTagSel_set(unit, port[port_index], deiSel[deiSel_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set (unit %u, port %u, deiSel %u)"
                                    , ret, expect_result, unit, port[port_index], deiSel[deiSel_index]);

                    ret = rtk_qos_portDeiRemarkTagSel_get(unit, port[port_index], &result_deiSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_get (unit %u, port %u, deiSel %u)"
                                     , ret, expect_result, unit, port[port_index], deiSel[deiSel_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set/get value unequal (unit %u, port %u, deiSel %u)"
                                     , result_deiSel, deiSel[deiSel_index], unit, port[port_index], deiSel[deiSel_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set (unit %u, port %u, deiSel %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], deiSel[deiSel_index]);
                }
            }

            ret = rtk_qos_portDeiRemarkTagSel_get(unit, port[port_index], &result_deiSel);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (port_result[port_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_qos_deiSel_t  deiSel;
        int32  deiSel_max;

        rtk_qos_deiSel_t  result_deiSel;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_DEIREMARKTAGSEL_MAX, deiSel_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_DEIREMARKTAGSEL_MIN, deiSel); deiSel <= deiSel_max; deiSel++)
            {
                ASSIGN_EXPECT_RESULT(deiSel, TEST_DEIREMARKTAGSEL_MIN, TEST_DEIREMARKTAGSEL_MAX, inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portDeiRemarkTagSel_set(unit, port, deiSel);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set (unit %u, port %u, deiSel %u)"
                                    , ret, expect_result, unit, port, deiSel);
                    ret = rtk_qos_portDeiRemarkTagSel_get(unit,  port, &result_deiSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_get (unit %u, port %u, deiSel %u)"
                                     , ret, expect_result, unit, port, deiSel);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set/get value unequal (unit %u, port %u, deiSel %u)"
                                     , result_deiSel, deiSel, unit, port, deiSel);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portDeiRemarkTagSel_set (unit %u, port %u, deiSel %u)"
                                        , ret, RT_ERR_OK, unit, port, deiSel);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_congAvoidSysDropProbability_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_probability;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidSysDropProbability_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidSysDropProbability_get(unit, 0, &temp_probability)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;

        uint32 probability[UNITTEST_MAX_TEST_VALUE];
        int32  probability_result[UNITTEST_MAX_TEST_VALUE];
        int32  probability_index;
        int32  probability_last;
        uint32  result_probability;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_WRED_DROP_PROBABILITY_MAX(unit), TEST_WRED_DROP_PROBABILITY_MIN, probability, probability_result, probability_last);

        for (dp_index = 0; dp_index <= dp_last; dp_index++)
        {
            inter_result2 = dp_result[dp_index];
            for (probability_index = 0; probability_index <= probability_last; probability_index++)
            {
                inter_result3 = probability_result[probability_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_congAvoidSysDropProbability_set(unit, dp[dp_index], probability[probability_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set (unit %u, dp %u, probability %u)"
                                    , ret, expect_result, unit, dp[dp_index], probability[probability_index]);

                    ret = rtk_qos_congAvoidSysDropProbability_get(unit, dp[dp_index], &result_probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_get (unit %u, dp %u, probability %u)"
                                     , ret, expect_result, unit, dp[dp_index], probability[probability_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set/get value unequal (unit %u, dp %u, probability %u)"
                                     , result_probability, probability[probability_index], unit, dp[dp_index], probability[probability_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set (unit %u, dp %u, probability %u)"
                                        , ret, RT_ERR_OK, unit, dp[dp_index], probability[probability_index]);
                }
            }

            ret = rtk_qos_congAvoidSysDropProbability_get(unit, dp[dp_index], &result_probability);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_get (unit %u, dp %u)"
                                , ret, expect_result, unit, dp[dp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_get (unit %u, dp %u)"
                                    , ret, RT_ERR_OK, unit, dp[dp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  dp;
        uint32  dp_max;
        uint32  probability;
        uint32  probability_max;
        uint32  result_probability;


        G_VALUE(TEST_DP_MAX, dp_max);
        G_VALUE(TEST_WRED_DROP_PROBABILITY_MAX(unit), probability_max);

        for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
        {
            ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result2);

            for (L_VALUE(TEST_WRED_DROP_PROBABILITY_MIN, probability); probability <= probability_max; probability++)
            {
                ASSIGN_EXPECT_RESULT(probability, TEST_WRED_DROP_PROBABILITY_MIN, TEST_WRED_DROP_PROBABILITY_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_congAvoidSysDropProbability_set(unit, dp, probability);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set (unit %u, dp %u, probability %u)"
                                    , ret, expect_result, unit, dp, probability);
                    ret = rtk_qos_congAvoidSysDropProbability_get(unit, dp, &result_probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_get (unit %u, dp %u, probability %u)"
                                     , ret, expect_result, unit, dp, probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set/get value unequal (unit %u, dp %u, probability %u)"
                                     , result_probability, probability, unit, dp, probability);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidSysDropProbability_set (unit %u, dp %u, probability %u)"
                                        , ret, RT_ERR_OK, unit, dp, probability);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_congAvoidGlobalQueueThresh_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_congAvoidThresh_t  temp_congAvoidThresh;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidGlobalQueueThresh_set(unit, 0, 0, &temp_congAvoidThresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidGlobalQueueThresh_get(unit, 0, 0, &temp_congAvoidThresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;

        uint32 maxThresh[UNITTEST_MAX_TEST_VALUE];
        int32  maxThresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  maxThresh_index;
        int32  maxThresh_last;

        uint32 minThresh[UNITTEST_MAX_TEST_VALUE];
        int32  minThresh_result[UNITTEST_MAX_TEST_VALUE];
        int32  minThresh_index;
        int32  minThresh_last;

        rtk_qos_congAvoidThresh_t congAvoidThresh;
        rtk_qos_congAvoidThresh_t result_congAvoidThresh;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_FLOWCTRL_THRESH_MAX(unit), TEST_FLOWCTRL_THRESH_MIN, maxThresh, maxThresh_result, maxThresh_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_FLOWCTRL_THRESH_MAX(unit), TEST_FLOWCTRL_THRESH_MIN, minThresh, minThresh_result, minThresh_last);

        for (dp_index = 0; dp_index <= dp_last; dp_index++)
        {
            inter_result3 = dp_result[dp_index];
            for (maxThresh_index = 0; maxThresh_index <= maxThresh_last; maxThresh_index++)
            {
                inter_result4 = maxThresh_result[maxThresh_index];
                for (minThresh_index = 0; minThresh_index <= minThresh_last; minThresh_index++)
                {
                    inter_result5 = minThresh_result[minThresh_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;

                    congAvoidThresh.maxThresh = maxThresh[maxThresh_index];
                    congAvoidThresh.minThresh = minThresh[minThresh_index];
                    ret = rtk_qos_congAvoidGlobalQueueThresh_set(unit, 0, dp[dp_index], &congAvoidThresh);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set (unit %u, queue %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                        , ret, expect_result, unit, 0, dp[dp_index], maxThresh[maxThresh_index], minThresh[minThresh_index]);

                        ret = rtk_qos_congAvoidGlobalQueueThresh_get(unit, 0, dp[dp_index], &result_congAvoidThresh);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_get (unit %u, queue %u, dp %u)"
                                         , ret, expect_result, unit, 0, dp[dp_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set/get maxThresh value unequal (unit %u, queue %u, dp %u)"
                                         , result_congAvoidThresh.maxThresh, maxThresh[maxThresh_index], unit, 0, dp[dp_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set/get minThresh value unequal (unit %u, queue %u, dp %u)"
                                         , result_congAvoidThresh.minThresh, minThresh[minThresh_index], unit, 0, dp[dp_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set (unit %u, queue %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                            , ret, RT_ERR_OK, unit, 0, dp[dp_index], maxThresh[maxThresh_index], minThresh[minThresh_index]);
                    }
                }
            }

            ret = rtk_qos_congAvoidGlobalQueueThresh_get(unit, 0, dp[dp_index], &result_congAvoidThresh);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_get (unit %u, queue %u, dp %u)"
                                , ret, expect_result, unit, 0, dp[dp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_get (unit %u, queue %u, dp %u)"
                                    , ret, RT_ERR_OK, unit, 0, dp[dp_index]);
            }

        }
    }

//#if 0 /* run too long time */
    if (IS_TEST_SCAN_MODE())
    {
        uint32  dp;
        uint32  dp_max;
        uint32  maxThresh;
        uint32  maxThresh_max;
        uint32  minThresh;
        uint32  minThresh_max;
        rtk_qos_congAvoidThresh_t congAvoidThresh;
        rtk_qos_congAvoidThresh_t result_congAvoidThresh;

        G_VALUE(TEST_DP_MAX, dp_max);
        G_VALUE(TEST_FLOWCTRL_THRESH_MAX(unit), maxThresh_max);
        G_VALUE(TEST_FLOWCTRL_THRESH_MAX(unit), minThresh_max);

        for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
        {
            ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result3);

            for (L_VALUE(TEST_FLOWCTRL_THRESH_MIN, maxThresh); maxThresh <= maxThresh_max; maxThresh++)
            {
                ASSIGN_EXPECT_RESULT(maxThresh, TEST_FLOWCTRL_THRESH_MIN, TEST_FLOWCTRL_THRESH_MAX(unit), inter_result4);

                for (L_VALUE(TEST_FLOWCTRL_THRESH_MIN, minThresh); minThresh <= minThresh_max; minThresh++)
                {
                    ASSIGN_EXPECT_RESULT(minThresh, TEST_FLOWCTRL_THRESH_MIN, TEST_FLOWCTRL_THRESH_MAX(unit), inter_result5);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result3 && inter_result4 && inter_result5)? RT_ERR_OK : RT_ERR_FAILED;

                    congAvoidThresh.maxThresh = maxThresh;
                    congAvoidThresh.minThresh = minThresh;
                    ret = rtk_qos_congAvoidGlobalQueueThresh_set(unit, 0, dp, &congAvoidThresh);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set (unit %u, queue %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                        , ret, expect_result, unit, 0, dp, maxThresh, minThresh);

                        ret = rtk_qos_congAvoidGlobalQueueThresh_get(unit, 0, dp, &result_congAvoidThresh);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_get (unit %u, queue %u, dp %u)"
                                         , ret, expect_result, unit, 0, dp);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set/get maxThresh value unequal (unit %u, queue %u, dp %u)"
                                         , result_congAvoidThresh.maxThresh, maxThresh, unit, 0, dp);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set/get minThresh value unequal (unit %u, queue %u, dp %u)"
                                         , result_congAvoidThresh.minThresh, minThresh, unit, 0, dp);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueThresh_set (unit %u, queue %u, dp %u, congAvoidThresh.maxThresh %u, congAvoidThresh.minThresh %u)"
                                            , ret, RT_ERR_OK, unit, 0, dp, maxThresh, minThresh);
                }
            }
        }
    }
//#endif

    return RT_ERR_OK;
}

int32 dal_qos_congAvoidGlobalQueueDropProbability_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_probability;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidGlobalQueueDropProbability_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_congAvoidGlobalQueueDropProbability_get(unit, 0, 0, &temp_probability)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32 dp[UNITTEST_MAX_TEST_VALUE];
        int32  dp_result[UNITTEST_MAX_TEST_VALUE];
        int32  dp_index;
        int32  dp_last;

        uint32 probability[UNITTEST_MAX_TEST_VALUE];
        int32  probability_result[UNITTEST_MAX_TEST_VALUE];
        int32  probability_index;
        int32  probability_last;
        uint32  result_probability;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_DP_MAX, TEST_DP_MIN, dp, dp_result, dp_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_WRED_DROP_PROBABILITY_MAX(unit), TEST_WRED_DROP_PROBABILITY_MIN, probability, probability_result, probability_last);

        for (dp_index = 0; dp_index <= dp_last; dp_index++)
        {
            inter_result3 = dp_result[dp_index];
            for (probability_index = 0; probability_index <= probability_last; probability_index++)
            {
                inter_result4 = probability_result[probability_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_congAvoidGlobalQueueDropProbability_set(unit, 0, dp[dp_index], probability[probability_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set (unit %u, queue %u, dp %u, probability %u)"
                                    , ret, expect_result, unit, 0, dp[dp_index], probability[probability_index]);

                    ret = rtk_qos_congAvoidGlobalQueueDropProbability_get(unit, 0, dp[dp_index], &result_probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_get (unit %u, queue %u, dp %u, probability %u)"
                                     , ret, expect_result, unit, 0, dp[dp_index], probability[probability_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set/get value unequal (unit %u, queue %u, dp %u, probability %u)"
                                     , result_probability, probability[probability_index], unit, 0, dp[dp_index], probability[probability_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set (unit %u, queue %u, dp %u, probability %u)"
                                        , ret, RT_ERR_OK, unit, 0, dp[dp_index], probability[probability_index]);
                }
            }

            ret = rtk_qos_congAvoidGlobalQueueDropProbability_get(unit, 0, dp[dp_index], &result_probability);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_get (unit %u, queue %u, dp %u)"
                                , ret, expect_result, unit, 0, dp[dp_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_get (unit %u, queue %u, dp %u)"
                                    , ret, RT_ERR_OK, unit, 0, dp[dp_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {

        uint32  dp;
        uint32  dp_max;
        uint32  probability;
        uint32  probability_max;
        uint32  result_probability;

        G_VALUE(TEST_DP_MAX, dp_max);
        G_VALUE(TEST_WRED_DROP_PROBABILITY_MAX(unit), probability_max);

        for (L_VALUE(TEST_DP_MIN, dp); dp <= dp_max; dp++)
        {
            ASSIGN_EXPECT_RESULT(dp, TEST_DP_MIN, TEST_DP_MAX, inter_result3);

            for (L_VALUE(TEST_WRED_DROP_PROBABILITY_MIN, probability); probability <= probability_max; probability++)
            {
                ASSIGN_EXPECT_RESULT(probability, TEST_WRED_DROP_PROBABILITY_MIN, TEST_WRED_DROP_PROBABILITY_MAX(unit), inter_result4);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_congAvoidGlobalQueueDropProbability_set(unit, 0, dp, probability);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set (unit %u, queue %u, dp %u, probability %u)"
                                    , ret, expect_result, unit, 0, dp, probability);
                    ret = rtk_qos_congAvoidGlobalQueueDropProbability_get(unit, 0, dp, &result_probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_get (unit %u, queue %u, dp %u, probability %u)"
                                     , ret, expect_result, unit, 0, dp, probability);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set/get value unequal (unit %u, queue %u, dp %u, probability %u)"
                                     , result_probability, probability, unit, 0, dp, probability);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_congAvoidGlobalQueueDropProbability_set (unit %u, queue %u, dp %u, probability %u)"
                                        , ret, RT_ERR_OK, unit, 0, dp, probability);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_portAvbStreamReservationClassEnable_test(uint32 caseNo, uint32 unit)
{
    int32   ret;
    uint32  enable_temp;
    int8    inter_result2 = 1;
    int8    inter_result3 = 1;
    int8    inter_result4 = 1;
    int32   expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portAvbStreamReservationClassEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portAvbStreamReservationClassEnable_get(unit, 0, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_avbSrClass_t  avbSrClass[UNITTEST_MAX_TEST_VALUE];
        int32  avbSrClass_result[UNITTEST_MAX_TEST_VALUE];
        int32  avbSrClass_index;
        int32  avbSrClass_last;

        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_AVB_SR_CLASS_MAX, TEST_AVB_SR_CLASS_MIN, avbSrClass, avbSrClass_result, avbSrClass_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            for (avbSrClass_index = 0; avbSrClass_index <= avbSrClass_last; avbSrClass_index++)
            {
                for (enable_index = 0; enable_index <= enable_last; enable_index++)
                {
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (port_result[port_index] && avbSrClass_result[avbSrClass_index] && enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_qos_portAvbStreamReservationClassEnable_set(unit, port[port_index], avbSrClass[avbSrClass_index], enable[enable_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set (unit %u, port %u, avbSrClass %u, enable %u)"
                                        , ret, expect_result, unit, port[port_index], avbSrClass[avbSrClass_index], enable[enable_index]);

                        ret = rtk_qos_portAvbStreamReservationClassEnable_get(unit, port[port_index], avbSrClass[avbSrClass_index], &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_get (unit %u, port %u, avbSrClass %u, enable %u)"
                                         , ret, expect_result, unit, port[port_index], avbSrClass[avbSrClass_index], enable[enable_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set/get value unequal (unit %u, port %u, avbSrClass %u, enable %u)"
                                         , result_enable, enable[enable_index], unit, port[port_index], avbSrClass[avbSrClass_index], enable[enable_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set (unit %u, port %u, avbSrClass %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], avbSrClass[avbSrClass_index], enable[enable_index]);
                    }
                }

                ret = rtk_qos_portAvbStreamReservationClassEnable_get(unit, port[port_index], avbSrClass[avbSrClass_index], &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (port_result[port_index] && avbSrClass_result[avbSrClass_index])? RT_ERR_OK : RT_ERR_FAILED;

                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_get (unit %u, port %u, avbSrClass %u)"
                                    , ret, expect_result, unit, port[port_index], avbSrClass[avbSrClass_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_get (unit %u, port %u, avbSrClass %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], avbSrClass[avbSrClass_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        uint32  port_max;
        rtk_qos_avbSrClass_t  avbSrClass;
        uint32      avbSrClass_max;
        rtk_enable_t  enable;
        int32  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_AVB_SR_CLASS_MAX, avbSrClass_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, HWP_UNITTEST_ETHER_PORT_MACID_MIN(unit), HWP_UNITTEST_ETHER_PORT_MACID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_AVB_SR_CLASS_MIN, avbSrClass); avbSrClass <= avbSrClass_max; avbSrClass++)
            {
                ASSIGN_EXPECT_RESULT(avbSrClass, TEST_AVB_SR_CLASS_MIN, TEST_AVB_SR_CLASS_MAX, inter_result3);

                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
                {
                    ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_qos_portAvbStreamReservationClassEnable_set(unit, port, avbSrClass, enable);
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set (unit %u, port %u, avbSrClass %u, enable %u)"
                                        , ret, expect_result, unit, port, avbSrClass, enable);
                        ret = rtk_qos_portAvbStreamReservationClassEnable_get(unit,  port, avbSrClass, &result_enable);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_get (unit %u, port %u, avbSrClass %u, enable %u)"
                                         , ret, expect_result, unit, port, avbSrClass, enable);
                        RT_TEST_IS_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set/get value unequal (unit %u, port %u, avbSrClass %u, enable %u)"
                                         , result_enable, enable, unit, port, avbSrClass, enable);

                    }
                    else
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portAvbStreamReservationClassEnable_set (unit %u, port %u, avbSrClass %u, enable %u)"
                                            , ret, RT_ERR_OK, unit, port, avbSrClass, enable);
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_avbStreamReservationConfig_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_qos_avbSrConf_t  temp_congAvoidThresh;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int8  inter_result5 = 1;
    int8  inter_result6 = 1;
    int8  inter_result7 = 1;
    int8  inter_result8 = 1;
    int8  inter_result9 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_avbStreamReservationConfig_set(unit, &temp_congAvoidThresh))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_avbStreamReservationConfig_get(unit, &temp_congAvoidThresh)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t classAPri[UNITTEST_MAX_TEST_VALUE];
        int32  classAPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  classAPri_index;
        int32  classAPri_last;

        rtk_pri_t classBPri[UNITTEST_MAX_TEST_VALUE];
        int32  classBPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  classBPri_index;
        int32  classBPri_last;

        rtk_qid_t classAQid[UNITTEST_MAX_TEST_VALUE];
        int32  classAQid_result[UNITTEST_MAX_TEST_VALUE];
        int32  classAQid_index;
        int32  classAQid_last;

        rtk_qid_t classBQid[UNITTEST_MAX_TEST_VALUE];
        int32  classBQid_result[UNITTEST_MAX_TEST_VALUE];
        int32  classBQid_index;
        int32  classBQid_last;

        rtk_qid_t classNoARedirectQid[UNITTEST_MAX_TEST_VALUE];
        int32  classNoARedirectQid_result[UNITTEST_MAX_TEST_VALUE];
        int32  classNoARedirectQid_index;
        int32  classNoARedirectQid_last;

        rtk_qid_t classNoBRedirectQid[UNITTEST_MAX_TEST_VALUE];
        int32  classNoBRedirectQid_result[UNITTEST_MAX_TEST_VALUE];
        int32  classNoBRedirectQid_index;
        int32  classNoBRedirectQid_last;

        rtk_pri_t classNoARemarkPri[UNITTEST_MAX_TEST_VALUE];
        int32  classNoARemarkPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  classNoARemarkPri_index;
        int32  classNoARemarkPri_last;

        rtk_pri_t classNoBRemarkPri[UNITTEST_MAX_TEST_VALUE];
        int32  classNoBRemarkPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  classNoBRemarkPri_index;
        int32  classNoBRemarkPri_last;

        rtk_qos_avbSrConf_t avbSrConf;
        rtk_qos_avbSrConf_t result_avbSrConf;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, classAPri, classAPri_result, classAPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, classBPri, classBPri_result, classBPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), classAQid, classAQid_result, classAQid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), classBQid, classBQid_result, classBQid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), classNoARedirectQid, classNoARedirectQid_result, classNoARedirectQid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_QUEUE_ID_MAX(unit), TEST_QUEUE_ID_MIN(unit), classNoBRedirectQid, classNoBRedirectQid_result, classNoBRedirectQid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, classNoARemarkPri, classNoARemarkPri_result, classNoARemarkPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, classNoBRemarkPri, classNoBRemarkPri_result, classNoBRemarkPri_last);

        for (classAPri_index = 0; classAPri_index <= classAPri_last; classAPri_index++)
        {
            inter_result2 = classAPri_result[classAPri_index];
            for (classBPri_index = 0; classBPri_index <= classBPri_last; classBPri_index++)
            {
                inter_result3 = classBPri_result[classBPri_index];
                for (classAQid_index = 0; classAQid_index <= classAQid_last; classAQid_index++)
                {
                    inter_result4 = classAQid_result[classAQid_index];
                    for (classBQid_index = 0; classBQid_index <= classBQid_last; classBQid_index++)
                    {
                        inter_result5 = classBQid_result[classBQid_index];
                        for (classNoARedirectQid_index = 0; classNoARedirectQid_index <= classNoARedirectQid_last; classNoARedirectQid_index++)
                        {
                            inter_result6 = classNoARedirectQid_result[classNoARedirectQid_index];
                            for (classNoBRedirectQid_index = 0; classNoBRedirectQid_index <= classNoBRedirectQid_last; classNoBRedirectQid_index++)
                            {
                                inter_result7 = classNoBRedirectQid_result[classNoBRedirectQid_index];
                                for (classNoARemarkPri_index = 0; classNoARemarkPri_index <= classNoARemarkPri_last; classNoARemarkPri_index++)
                                {
                                    inter_result8 = classNoARemarkPri_result[classNoARemarkPri_index];
                                    for (classNoBRemarkPri_index = 0; classNoBRemarkPri_index <= classNoBRemarkPri_last; classNoBRemarkPri_index++)
                                    {
                                        inter_result9 = classNoBRemarkPri_result[classNoBRemarkPri_index];
                                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                            expect_result = RT_ERR_FAILED;
                                        else
                                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5 && inter_result6 && inter_result7 && inter_result8 && inter_result9)? RT_ERR_OK : RT_ERR_FAILED;

                                        avbSrConf.class_a_priority = classAPri[classAPri_index];
                                        avbSrConf.class_b_priority = classBPri[classBPri_index];
                                        avbSrConf.class_a_queue_id = classAQid[classAQid_index];
                                        avbSrConf.class_b_queue_id = classBQid[classBQid_index];
                                        avbSrConf.class_non_a_redirect_queue_id = classNoARedirectQid[classNoARedirectQid_index];
                                        avbSrConf.class_non_b_redirect_queue_id = classNoBRedirectQid[classNoBRedirectQid_index];
                                        avbSrConf.class_non_a_remark_priority = classNoARemarkPri[classNoARemarkPri_index];
                                        avbSrConf.class_non_b_remark_priority = classNoBRemarkPri[classNoBRemarkPri_index];

                                        ret = rtk_qos_avbStreamReservationConfig_set(unit, &avbSrConf);
                                        if (RT_ERR_OK == expect_result)
                                        {
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set (unit %u)"
                                                            , ret, expect_result, unit);

                                            ret = rtk_qos_avbStreamReservationConfig_get(unit, &result_avbSrConf);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_get (unit %u)"
                                                             , ret, expect_result, unit);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classAPri value unequal (unit %u, avbSrConf.classAPri %u)"
                                                             , result_avbSrConf.class_a_priority, classAPri[classAPri_index], unit, classAPri[classAPri_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classBPri value unequal (unit %u, avbSrConf.classBPri %u)"
                                                             , result_avbSrConf.class_b_priority, classBPri[classBPri_index], unit, classBPri[classBPri_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classAQid value unequal (unit %u, avbSrConf.classAQid %u)"
                                                             , result_avbSrConf.class_a_queue_id, classAQid[classAQid_index], unit, classAQid[classAQid_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classBQid value unequal (unit %u, avbSrConf.classBQid %u)"
                                                             , result_avbSrConf.class_b_queue_id, classBQid[classBQid_index], unit, classBQid[classBQid_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoARedirectQid value unequal (unit %u, avbSrConf.classNoARedirectQid %u)"
                                                             , result_avbSrConf.class_non_a_redirect_queue_id, classNoARedirectQid[classNoARedirectQid_index], unit, classNoARedirectQid[classNoARedirectQid_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoBRedirectQid value unequal (unit %u, avbSrConf.classNoBRedirectQid %u)"
                                                             , result_avbSrConf.class_non_b_redirect_queue_id, classNoBRedirectQid[classNoBRedirectQid_index], unit, classNoBRedirectQid[classNoBRedirectQid_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoARemarkPri value unequal (unit %u, avbSrConf.classNoARemarkPri %u)"
                                                             , result_avbSrConf.class_non_a_remark_priority, classNoARemarkPri[classNoARemarkPri_index], unit, classNoARemarkPri[classNoARemarkPri_index]);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoBRemarkPri value unequal (unit %u, avbSrConf.classNoBRemarkPri %u)"
                                                             , result_avbSrConf.class_non_b_remark_priority, classNoBRemarkPri[classNoBRemarkPri_index], unit, classNoBRemarkPri[classNoBRemarkPri_index]);
                                        }
                                        else
                                        {
                                            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set (unit %u, avbSrConf.classAPri %u, avbSrConf.classBPri %u, avbSrConf.classAQid %u, avbSrConf.classBQid %u, avbSrConf.classNoARedirectQid %u, avbSrConf.classNoBRedirectQid %u, avbSrConf.classNoARemarkPri %u, , avbSrConf.classNoBRemarkPri %u)", ret, RT_ERR_OK, unit,
                                                                      classAPri[classAPri_index], classBPri[classBPri_index], classAQid[classAQid_index], classBQid[classBQid_index],
                                                                      classNoARedirectQid[classNoARedirectQid_index], classNoBRedirectQid[classNoBRedirectQid_index],
                                                                      classNoARemarkPri[classNoARemarkPri_index], classNoBRemarkPri[classNoBRemarkPri_index]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ret = rtk_qos_avbStreamReservationConfig_get(unit, &result_avbSrConf);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_avbStreamReservationConfig_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t classAPri;
        uint32  classAPri_max;
        rtk_pri_t classBPri;
        uint32  classBPri_max;
        rtk_qid_t classAQid;
        uint32  classAQid_max;
        rtk_qid_t classBQid;
        uint32  classBQid_max;
        rtk_qid_t classNoARedirectQid;
        uint32  classNoARedirectQid_max;
        rtk_qid_t classNoBRedirectQid;
        uint32  classNoBRedirectQid_max;
        rtk_pri_t classNoARemarkPri;
        uint32  classNoARemarkPri_max;
        rtk_pri_t classNoBRemarkPri;
        uint32  classNoBRemarkPri_max;

        rtk_qos_avbSrConf_t avbSrConf;
        rtk_qos_avbSrConf_t result_avbSrConf;


        G_VALUE(TEST_PRI_MAX(unit), classAPri_max);
        G_VALUE(TEST_PRI_MAX(unit), classBPri_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), classAQid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), classBQid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), classNoARedirectQid_max);
        G_VALUE(TEST_QUEUE_ID_MAX(unit), classNoBRedirectQid_max);
        G_VALUE(TEST_PRI_MAX(unit), classNoARemarkPri_max);
        G_VALUE(TEST_PRI_MAX(unit), classNoBRemarkPri_max);

        for (L_VALUE(TEST_PRI_MAX(unit), classAPri); classAPri <= classAPri_max; classAPri++)
        {
            ASSIGN_EXPECT_RESULT(classAPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);
            for (L_VALUE(TEST_PRI_MAX(unit), classBPri); classBPri <= classBPri_max; classBPri++)
            {
                ASSIGN_EXPECT_RESULT(classBPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);
                for (L_VALUE(TEST_QUEUE_ID_MIN(unit), classAQid); classAQid <= classAQid_max; classAQid++)
                {
                    ASSIGN_EXPECT_RESULT(classAQid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result4);
                    for (L_VALUE(TEST_QUEUE_ID_MIN(unit), classBQid); classBQid <= classBQid_max; classBQid++)
                    {
                        ASSIGN_EXPECT_RESULT(classBQid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result5);
                        for (L_VALUE(TEST_QUEUE_ID_MIN(unit), classNoARedirectQid); classNoARedirectQid <= classNoARedirectQid_max; classNoARedirectQid++)
                        {
                            ASSIGN_EXPECT_RESULT(classNoARedirectQid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result6);
                            for (L_VALUE(TEST_QUEUE_ID_MIN(unit), classNoBRedirectQid); classNoBRedirectQid <= classNoBRedirectQid_max; classNoBRedirectQid++)
                            {
                                ASSIGN_EXPECT_RESULT(classNoBRedirectQid, TEST_QUEUE_ID_MIN(unit), TEST_QUEUE_ID_MAX(unit), inter_result7);
                                for (L_VALUE(TEST_PRI_MIN, classNoARemarkPri); classNoARemarkPri <= classNoARemarkPri_max; classNoARemarkPri++)
                                {
                                    ASSIGN_EXPECT_RESULT(classNoARemarkPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result8);
                                    for (L_VALUE(TEST_PRI_MIN, classNoBRemarkPri); classNoBRemarkPri <= classNoBRemarkPri_max; classNoBRemarkPri++)
                                    {
                                        ASSIGN_EXPECT_RESULT(classNoBRemarkPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result9);

                                        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                                            expect_result = RT_ERR_FAILED;
                                        else
                                            expect_result = (inter_result2 && inter_result3 && inter_result4 && inter_result5 && inter_result6 && inter_result7 && inter_result8 && inter_result9)? RT_ERR_OK : RT_ERR_FAILED;
                                        avbSrConf.class_a_priority = classAPri;
                                        avbSrConf.class_b_priority = classBPri;
                                        avbSrConf.class_a_queue_id = classAQid;
                                        avbSrConf.class_b_queue_id = classBQid;
                                        avbSrConf.class_non_a_redirect_queue_id = classNoARedirectQid;
                                        avbSrConf.class_non_b_redirect_queue_id = classNoBRedirectQid;
                                        avbSrConf.class_non_a_remark_priority = classNoARemarkPri;
                                        avbSrConf.class_non_b_remark_priority = classNoBRemarkPri;

                                        ret = rtk_qos_avbStreamReservationConfig_set(unit, &avbSrConf);
                                        if (expect_result == RT_ERR_OK)
                                        {
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set (unit %u)"
                                                            , ret, expect_result, unit);

                                            ret = rtk_qos_avbStreamReservationConfig_get(unit, &result_avbSrConf);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_get (unit %u)"
                                                             , ret, expect_result, unit);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classAPri value unequal (unit %u, avbSrConf.classAPri %u)"
                                                             , result_avbSrConf.class_a_priority, classAPri, unit, classAPri);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classBPri value unequal (unit %u, avbSrConf.classBPri %u)"
                                                             , result_avbSrConf.class_b_priority, classBPri, unit, classBPri);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classAQid value unequal (unit %u, avbSrConf.classAQid %u)"
                                                             , result_avbSrConf.class_a_queue_id, classAQid, unit, classAQid);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classBQid value unequal (unit %u, avbSrConf.classBQid %u)"
                                                             , result_avbSrConf.class_b_queue_id, classBQid, unit, classBQid);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoARedirectQid value unequal (unit %u, avbSrConf.classNoARedirectQid %u)"
                                                             , result_avbSrConf.class_non_a_redirect_queue_id, classNoARedirectQid, unit, classNoARedirectQid);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoBRedirectQid value unequal (unit %u, avbSrConf.classNoBRedirectQid %u)"
                                                             , result_avbSrConf.class_non_b_redirect_queue_id, classNoBRedirectQid, unit, classNoBRedirectQid);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoARemarkPri value unequal (unit %u, avbSrConf.classNoARemarkPri %u)"
                                                             , result_avbSrConf.class_non_a_remark_priority, classNoARemarkPri, unit, classNoARemarkPri);
                                            RT_TEST_IS_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set/get classNoBRemarkPri value unequal (unit %u, avbSrConf.classNoBRemarkPri %u)"
                                                             , result_avbSrConf.class_non_b_remark_priority, classNoBRemarkPri, unit, classNoBRemarkPri);
                                        }
                                        else
                                            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_avbStreamReservationConfig_set (unit %u, avbSrConf.classAPri %u, avbSrConf.classBPri %u, avbSrConf.classAQid %u, avbSrConf.classBQid %u, avbSrConf.classNoARedirectQid %u, avbSrConf.classNoBRedirectQid %u, avbSrConf.classNoARemarkPri %u, , avbSrConf.classNoBRemarkPri %u)", ret, RT_ERR_OK, unit,
                                                                      classAPri, classBPri, classAQid, classBQid,
                                                                      classNoARedirectQid, classNoBRedirectQid,
                                                                      classNoARemarkPri, classNoBRemarkPri);
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
}

int32 dal_qos_pkt2CpuPriRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  temp_pri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_pkt2CpuPriRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_pkt2CpuPriRemap_get(unit, 0, &temp_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  intPri[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_index;
        int32  intPri_last;

        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;

        rtk_pri_t  result_pri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, intPri, intPri_result, intPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, pri, pri_result, pri_last);

        for (intPri_index = 0; intPri_index <= intPri_last; intPri_index++)
        {
            for (pri_index = 0; pri_index <= pri_last; pri_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (intPri_result[intPri_index] && pri_result[pri_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_pkt2CpuPriRemap_set(unit, intPri[intPri_index], pri[pri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set (unit %u, intPri %u, pri %u)"
                                    , ret, expect_result, unit, intPri[intPri_index], pri[pri_index]);

                    ret = rtk_qos_pkt2CpuPriRemap_get(unit, intPri[intPri_index], &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_get (unit %u, intPri %u, pri %u)"
                                     , ret, expect_result, unit, intPri[intPri_index], pri[pri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set/get value unequal (unit %u, intPri %u, pri %u)"
                                     , result_pri, pri[pri_index], unit, intPri[intPri_index], pri[pri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set (unit %u, intPri %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, intPri[intPri_index], pri[pri_index]);
                }
            }

            ret = rtk_qos_pkt2CpuPriRemap_get(unit, intPri[intPri_index], (uint32*)&result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (intPri_result[intPri_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_get (unit %u, intPri %u)"
                                , ret, expect_result, unit, intPri[intPri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_get (unit %u, intPri %u)"
                                    , ret, RT_ERR_OK, unit, intPri[intPri_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  intPri;
        uint32  intPri_max;
        rtk_pri_t  pri;
        uint32  pri_max;
        uint32  result_pri;

        G_VALUE(TEST_PRI_MAX(unit), intPri_max);
        G_VALUE(TEST_PRI_MAX(unit), pri_max);

        for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
        {
            ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PRI_MIN, pri); pri <= pri_max; pri++)
            {
                ASSIGN_EXPECT_RESULT(pri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_pkt2CpuPriRemap_set(unit, intPri, pri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set (unit %u, intPri %u, pri %u)"
                                    , ret, expect_result, unit, intPri, pri);
                    ret = rtk_qos_pkt2CpuPriRemap_get(unit,  intPri, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_get (unit %u, intPri %u, pri %u)"
                                     , ret, expect_result, unit, intPri, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set/get value unequal (unit %u, intPri %u, pri %u)"
                                     , result_pri, pri, unit, intPri, pri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_pkt2CpuPriRemap_set (unit %u, intPri %u, pri %u)"
                                        , ret, RT_ERR_OK, unit, intPri, pri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_rspanPriRemap_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_pri_t  temp_intPri;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_rspanPriRemap_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_rspanPriRemap_get(unit, 0, &temp_intPri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  rspanPri[UNITTEST_MAX_TEST_VALUE];
        int32  rspanPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  rspanPri_index;
        int32  rspanPri_last;

        rtk_pri_t  intPri[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_result[UNITTEST_MAX_TEST_VALUE];
        int32  intPri_index;
        int32  intPri_last;

        rtk_pri_t  result_intPri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, rspanPri, rspanPri_result, rspanPri_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, intPri, intPri_result, intPri_last);

        for (rspanPri_index = 0; rspanPri_index <= rspanPri_last; rspanPri_index++)
        {
            for (intPri_index = 0; intPri_index <= intPri_last; intPri_index++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (rspanPri_result[rspanPri_index] && intPri_result[intPri_index])? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_rspanPriRemap_set(unit, rspanPri[rspanPri_index], intPri[intPri_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_set (unit %u, rspanPri %u, intPri %u)"
                                    , ret, expect_result, unit, rspanPri[rspanPri_index], intPri[intPri_index]);

                    ret = rtk_qos_rspanPriRemap_get(unit, rspanPri[rspanPri_index], &result_intPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_get (unit %u, rspanPri %u, intPri %u)"
                                     , ret, expect_result, unit, rspanPri[rspanPri_index], intPri[intPri_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_set/get value unequal (unit %u, rspanPri %u, intPri %u)"
                                     , result_intPri, intPri[intPri_index], unit, rspanPri[rspanPri_index], intPri[intPri_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_rspanPriRemap_set (unit %u, rspanPri %u, intPri %u)"
                                        , ret, RT_ERR_OK, unit, rspanPri[rspanPri_index], intPri[intPri_index]);
                }
            }

            ret = rtk_qos_rspanPriRemap_get(unit, rspanPri[rspanPri_index], &result_intPri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (rspanPri_result[rspanPri_index])? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_get (unit %u, rspanPri %u)"
                                , ret, expect_result, unit, rspanPri[rspanPri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_rspanPriRemap_get (unit %u, rspanPri %u)"
                                    , ret, RT_ERR_OK, unit, rspanPri[rspanPri_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  rspanPri;
        uint32  rspanPri_max;
        rtk_pri_t  intPri;
        uint32  intPri_max;
        rtk_pri_t  result_intPri;


        G_VALUE(TEST_PRI_MAX(unit), rspanPri_max);
        G_VALUE(TEST_PRI_MAX(unit), intPri_max);

        for (L_VALUE(TEST_PRI_MIN, rspanPri); rspanPri <= rspanPri_max; rspanPri++)
        {
            ASSIGN_EXPECT_RESULT(rspanPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            for (L_VALUE(TEST_PRI_MIN, intPri); intPri <= intPri_max; intPri++)
            {
                ASSIGN_EXPECT_RESULT(intPri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result3);

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_rspanPriRemap_set(unit, rspanPri, intPri);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_set (unit %u, rspanPri %u, intPri %u)"
                                    , ret, expect_result, unit, rspanPri, intPri);
                    ret = rtk_qos_rspanPriRemap_get(unit,  rspanPri, &result_intPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_get (unit %u, rspanPri %u, intPri %u)"
                                     , ret, expect_result, unit, rspanPri, intPri);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_rspanPriRemap_set/get value unequal (unit %u, rspanPri %u, intPri %u)"
                                     , result_intPri, intPri, unit, rspanPri, intPri);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_rspanPriRemap_set (unit %u, rspanPri %u, intPri %u)"
                                        , ret, RT_ERR_OK, unit, rspanPri, intPri);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_1pDfltPriSrcSel_test(uint32 caseNo, uint32 unit)
{
    static rt_error_common_t onepDflt_PriSrcSel[PRI_SRC_END+1] =
    {RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED,
     RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED,
     RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED};

    int32 ret;
    rtk_qos_1pDfltPriSrc_t  temp_1pDfltPriSrc;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pDfltPriSrcSel_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_1pDfltPriSrcSel_get(unit, &temp_1pDfltPriSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    if (HWP_8380_30_FAMILY(unit))
    {
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_PB_INNER_PRI] = RT_ERR_OK;
    }
    if (HWP_8390_50_FAMILY(unit))
    {
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_PB_PRI] = RT_ERR_OK;

    }
    if (HWP_9300_FAMILY_ID(unit))
    {
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;
    }
    if (HWP_9310_FAMILY_ID(unit))
    {
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_INT_PRI] = RT_ERR_OK;
        onepDflt_PriSrcSel[INNER_1P_DFLT_SRC_DFLT_PRI] = RT_ERR_OK;

    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_qos_1pDfltPriSrc_t  onepDfltPriSrc[UNITTEST_MAX_TEST_VALUE];
        int32  onepDfltPriSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  onepDfltPriSrc_index;
        int32  onepDfltPriSrc_last;
        rtk_qos_1pDfltPriSrc_t  result_onepDfltPriSrc;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_1PDFLTPRISRC_MAX, TEST_1PDFLTPRISRC_MIN, onepDfltPriSrc, onepDfltPriSrc_result, onepDfltPriSrc_last);

        for (onepDfltPriSrc_index = 0; onepDfltPriSrc_index <= onepDfltPriSrc_last; onepDfltPriSrc_index++)
        {
            inter_result2 = onepDfltPriSrc_result[onepDfltPriSrc_index];

            if((RT_ERR_OK == onepDflt_PriSrcSel[onepDfltPriSrc[onepDfltPriSrc_index]]) && (inter_result2 == 1))
                inter_result2 = 1;
            else
                inter_result2 = 0;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

             ret = rtk_qos_1pDfltPriSrcSel_set(unit, onepDfltPriSrc[onepDfltPriSrc_index]);
             if (RT_ERR_OK == expect_result)
             {
                  RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_set (unit %u, onepDfltPriSrc %u)"
                                  , ret, expect_result, unit,  onepDfltPriSrc[onepDfltPriSrc_index]);

                  ret = rtk_qos_1pDfltPriSrcSel_get(unit, &result_onepDfltPriSrc);
                  RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_get (unit %u, onepDfltPriSrc %u)"
                                   , ret, expect_result, unit, onepDfltPriSrc[onepDfltPriSrc_index]);
                  RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_set/get value unequal (unit %u, onepDfltPriSrc %u)"
                                   , result_onepDfltPriSrc, onepDfltPriSrc[onepDfltPriSrc_index], unit, onepDfltPriSrc[onepDfltPriSrc_index]);
              }
              else
              {
                  RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_set (unit %u, onepDfltPriSrc %u)"
                                      , ret, RT_ERR_OK, unit, onepDfltPriSrc[onepDfltPriSrc_index]);
              }
          }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_qos_1pDfltPriSrc_t  onepDfltPriSrc;
        uint32  onepDfltPriSrc_max;
        rtk_qos_1pDfltPriSrc_t  result_onepDfltPriSrc;


        G_VALUE(TEST_1PDFLTPRISRC_MAX, onepDfltPriSrc_max);

        for (L_VALUE(TEST_1PDFLTPRISRC_MIN, onepDfltPriSrc); onepDfltPriSrc <= onepDfltPriSrc_max; onepDfltPriSrc++)
        {
            ASSIGN_EXPECT_RESULT(onepDfltPriSrc, TEST_1PDFLTPRISRC_MIN, TEST_1PDFLTPRISRC_MAX, inter_result2);

            if((RT_ERR_OK == onepDflt_PriSrcSel[onepDfltPriSrc]) && (inter_result2 == 1))
                inter_result2 = 1;
            else
                inter_result2 = 0;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_qos_1pDfltPriSrcSel_set(unit, onepDfltPriSrc);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_set (unit %u, onepDfltPriSrc %u)"
                                , ret, expect_result, unit, onepDfltPriSrc);
                ret = rtk_qos_1pDfltPriSrcSel_get(unit, &result_onepDfltPriSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_get (unit %u, onepDfltPriSrc %u)"
                                 , ret, expect_result, unit, onepDfltPriSrc);
                RT_TEST_IS_EQUAL_INT("rtk_qos_1pDfltPriSrcSel_set/get value unequal (unit %u, onepDfltPriSrc %u)"
                                 , result_onepDfltPriSrc, onepDfltPriSrc, unit, onepDfltPriSrc);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, out1pDfltPriSrc %u)"
                                        , ret, RT_ERR_OK, unit, onepDfltPriSrc);
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_portOuter1pRemarkSrcSel_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    static rt_error_common_t outer1pRemark_SrcSel[PRI_SRC_END + 1] =
    {RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED,
     RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED,
     RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED, RT_ERR_FAILED};

    rtk_qos_outer1pRmkSrc_t  temp_out1pDfltPriSrc;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    if (HWP_8380_30_FAMILY(unit))
    {
        outer1pRemark_SrcSel[OUTER_1P_RMK_SRC_INT_PRI] = RT_ERR_OK;
        outer1pRemark_SrcSel[OUTER_1P_RMK_SRC_USER_PRI] = RT_ERR_OK;
    }

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuter1pRemarkSrcSel_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_portOuter1pRemarkSrcSel_get(unit, 0, &temp_out1pDfltPriSrc)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_qos_outer1pRmkSrc_t  out1pRemarkSrc[UNITTEST_MAX_TEST_VALUE];
        int32  out1pRemarkSrc_result[UNITTEST_MAX_TEST_VALUE];
        int32  out1pRemarkSrc_index;
        int32  out1pRemarkSrc_last;
        rtk_qos_outer1pRmkSrc_t  result_out1pRemarkSrc;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_OUT1PRMKSRC_MAX, TEST_OUT1PRMKSRC_MIN, out1pRemarkSrc, out1pRemarkSrc_result, out1pRemarkSrc_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (out1pRemarkSrc_index = 0; out1pRemarkSrc_index <= out1pRemarkSrc_last; out1pRemarkSrc_index++)
            {
                inter_result3 = out1pRemarkSrc_result[out1pRemarkSrc_index];
                if((RT_ERR_OK == outer1pRemark_SrcSel[out1pRemarkSrc[out1pRemarkSrc_index]]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOuter1pRemarkSrcSel_set(unit, port[port_index], out1pRemarkSrc[out1pRemarkSrc_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_set (unit %u, port %u, out1pRemarkSrc %u)"
                                    , ret, expect_result, unit, port[port_index], out1pRemarkSrc[out1pRemarkSrc_index]);

                    ret = rtk_qos_portOuter1pRemarkSrcSel_get(unit, port[port_index], &result_out1pRemarkSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_get (unit %u, port %u, out1pRemarkSrc %u)"
                                     , ret, expect_result, unit, port[port_index], out1pRemarkSrc[out1pRemarkSrc_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_set/get value unequal (unit %u, port %u, out1pRemarkSrc %u)"
                                     , result_out1pRemarkSrc, out1pRemarkSrc[out1pRemarkSrc_index], unit, port[port_index], out1pRemarkSrc[out1pRemarkSrc_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pDfltPriSrcSel_set (unit %u, port %u, out1pRemarkSrc %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], out1pRemarkSrc[out1pRemarkSrc_index]);
                }
            }

            ret = rtk_qos_portOuter1pRemarkSrcSel_get(unit, port[port_index], &result_out1pRemarkSrc);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        uint32  port_max;
        rtk_qos_outer1pRmkSrc_t  out1pRemarkSrc;
        uint32  out1pRemarkSrc_max;
        rtk_qos_outer1pRmkSrc_t  result_out1pRemarkSrc;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_OUT1PRMKSRC_MAX, out1pRemarkSrc_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_OUT1PRMKSRC_MIN, out1pRemarkSrc); out1pRemarkSrc <= out1pRemarkSrc_max; out1pRemarkSrc++)
            {
                ASSIGN_EXPECT_RESULT(out1pRemarkSrc, TEST_OUT1PRMKSRC_MIN, TEST_OUT1PRMKSRC_MAX, inter_result3);

                if((RT_ERR_OK == outer1pRemark_SrcSel[out1pRemarkSrc]) && (inter_result3 == 1))
                    inter_result3 = 1;
                else
                    inter_result3 = 0;

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_qos_portOuter1pRemarkSrcSel_set(unit, port, out1pRemarkSrc);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_set (unit %u, port %u, out1pRemarkSrc %u)"
                                    , ret, expect_result, unit, port, out1pRemarkSrc);
                    ret = rtk_qos_portOuter1pRemarkSrcSel_get(unit, port, &result_out1pRemarkSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_get (unit %u, port %u, out1pRemarkSrc %u)"
                                     , ret, expect_result, unit, port, out1pRemarkSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_set/get value unequal (unit %u, port %u, out1pRemarkSrc %u)"
                                     , result_out1pRemarkSrc, out1pRemarkSrc, unit, port, out1pRemarkSrc);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_portOuter1pRemarkSrcSel_set (unit %u, port %u, out1pDfltPriSrc %u)"
                                        , ret, RT_ERR_OK, unit, port, out1pRemarkSrc);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_qos_outer1pDfltPri_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  temp_pri;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pDfltPri_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_qos_outer1pDfltPri_get(unit, &temp_pri)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_pri_t  pri[UNITTEST_MAX_TEST_VALUE];
        int32  pri_result[UNITTEST_MAX_TEST_VALUE];
        int32  pri_index;
        int32  pri_last;
        uint32  result_pri;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PRI_MAX(unit), TEST_PRI_MIN, pri, pri_result, pri_last);

        for (pri_index = 0; pri_index <= pri_last; pri_index++)
        {
            inter_result2 = pri_result[pri_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_outer1pDfltPri_set(unit, pri[pri_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri[pri_index]);

                ret = rtk_qos_outer1pDfltPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri[pri_index]);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri[pri_index], unit, pri[pri_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pDfltPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri[pri_index]);
            }
        }

        ret = rtk_qos_outer1pDfltPri_get(unit, &result_pri);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pDfltPri_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_pri_t   pri;
        rtk_pri_t   pri_max;
        rtk_pri_t   result_pri;

        G_VALUE(TEST_PRI_MAX(unit), pri_max);

        for (L_VALUE(TEST_PRI_MIN, pri); pri <= pri_max; pri++)
        {
            ASSIGN_EXPECT_RESULT(pri, TEST_PRI_MIN, TEST_PRI_MAX(unit), inter_result2);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_qos_outer1pDfltPri_set(unit, pri);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_set (unit %u, pri %u)"
                                , ret, expect_result, unit, pri);
                ret = rtk_qos_outer1pDfltPri_get(unit, &result_pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_get (unit %u, pri %u)"
                                 , ret, expect_result, unit, pri);
                RT_TEST_IS_EQUAL_INT("rtk_qos_outer1pDfltPri_set/get value unequal (unit %u, pri %u)"
                                 , result_pri, pri, unit, pri);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_qos_outer1pDfltPri_set (unit %u, pri %u)"
                                    , ret, RT_ERR_OK, unit, pri);
        }
    }

    return RT_ERR_OK;
}


