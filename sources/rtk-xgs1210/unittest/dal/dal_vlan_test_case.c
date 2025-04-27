/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 82367 $
 * $Date: 2017-09-21 13:59:33 +0800 (Thu, 21 Sep 2017) $
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
#define TEST_RAND_IDX_MIN                       0
#define TEST_RAND_IDX_MAX                       50

#define TEST_ACCEPT_FRAMETYPE_MAX               (ACCEPT_FRAME_TYPE_END-1)
#define TEST_ACCEPT_FRAMETYPE_MIN               ACCEPT_FRAME_TYPE_ALL

/* TPID index for inner/outter*/
#define TEST_TPID_IDX_MIN                       0
#define TEST_TPID_IDX_MAX(unit)                 HAL_TPID_ENTRY_IDX_MAX(unit)
/* TPID index for extra*/
#define TEST_EXTRA_TPID_IDX_MIN                 0
#define TEST_EXTRA_TPID_IDX_MAX(unit)           (HAL_MAX_NUM_OF_EVLAN_TPID(unit)-1)

#define TEST_TPIDIDX_MASK_MIN                   0x0
#define TEST_TPIDIDX_MASK_MAX(unit)             HAL_TPID_ENTRY_MASK_MAX(unit)

#define TEST_TPID_MIN                           0
#define TEST_TPID_MAX                           RTK_ETHERTYPE_MAX
/*protocol group index*/
#define TEST_PRTOCOGRP_IDX_MIN                  0
#define TEST_PRTOCOGRP_IDX_MAX(unit)            HAL_PROTOCOL_VLAN_IDX_MAX(unit)
/*uc lookup mode*/
#define TEST_UC_LOOKUP_MODE_MIN                 VLAN_L2_LOOKUP_MODE_VID
#define TEST_UC_LOOKUP_MODE_MAX                 (VLAN_L2_LOOKUP_MODE_END-1)
/*mc lookup mode*/
#define TEST_MC_LOOKUP_MODE_MIN                 VLAN_L2_LOOKUP_MODE_VID
#define TEST_MC_LOOKUP_MODE_MAX                 (VLAN_L2_LOOKUP_MODE_END-1)
/*vlan profile index*/
#define TEST_VLAN_PROFILE_IDX_MIN               0
#define TEST_VLAN_PROFILE_IDX_MAX(unit)         (HAL_MAX_NUM_OF_VLAN_PROFILE(unit)-1)
/*vlan ingress filter*/
#define TEST_VLAN_IGR_FILTER_MIN                INGRESS_FILTER_FWD
#define TEST_VLAN_IGR_FILTER_MAX                (INGRESS_FILTER_TRAP2MASTERCPU-1)
/*port-based pvid mode*/
#define TEST_VLAN_PBPVID_MODE_MIN               PBVLAN_MODE_UNTAG_AND_PRITAG
#define TEST_VLAN_PBPVID_MODE_MAX               (PBVLAN_MODE_END-1)
/*vlan tag status*/
#define TEST_VLAN_TAGSTATUS_MIN                 TAG_STATUS_UNTAG
#define TEST_VLAN_TAGSTATUS_MAX(unit)           ((HWP_8390_50_FAMILY(unit)||HWP_8380_30_FAMILY(unit))?((TAG_STATUS_INTERNAL-1)):((TAG_STATUS_END-1)))
/*C2SC block*/
#define TEST_C2SC_BLK_MIN   0
#define TEST_C2SC_BLK_MAX(unit)                 (HAL_MAX_NUM_OF_C2SC_BLK(unit)-1)
/*igrVlanCnvtBlk_mode*/
#define TEST_VLAN_CNVTBLK_MODE_MIN              CONVERSION_MODE_C2SC
#define TEST_VLAN_CNVTBLK_MODE_MAX              (CONVERSION_MODE_END-1)
/*vlan type*/
#define TEST_VLAN_TYPE_MIN                      INNER_VLAN
#define TEST_VLAN_TYPE_MAX                      OUTER_VLAN
/*vlan mode*/
#define TEST_VLAN_MODE_MIN                      BASED_ON_INNER_VLAN
#define TEST_VLAN_MODE_MAX                      BASED_ON_OUTER_VLAN
/*vlan exception action*/
#define TEST_VLAN_EXCEPT_ACT_MIN                ACTION_FORWARD
#define TEST_VLAN_EXCEPT_ACT_MAX                ACTION_TRAP2CPU
/*portIgrCnvtDfltAct*/
#define TEST_IGRCNVTDFLTACT_MIN                 ACTION_FORWARD
#define TEST_IGRCNVTDFLTACT_MAX                 ACTION_DROP
/*tag keep type*/
#define TEST_TAGKEEPTYPE_MIN                    TAG_KEEP_TYPE_NOKEEP
#define TEST_TAGKEEPTYPE_MAX                    (TAG_KEEP_TYPE_END-1)
/*priTagVidSrc*/
#define TEST_PRITAGVIDSRC_MIN                   LEARNING_VID_PRI
#define TEST_PRITAGVIDSRC_MAX                   (LEARNING_VID_END-1)
/*portEgrVlanCnvtLookupMissAct*/
#define TEST_EGRCNVTLOOKUPMISSACT_MIN           LOOKUPMISS_FWD
#define TEST_EGRCNVTLOOKUPMISSACT_MAX           LOOKUPMISS_DROP
/*C2SC entry index*/
#define TEST_C2SC_ENTRY_MIN                     0
#define TEST_C2SC_ENTRY_MAX(unit)               (HAL_MAX_NUM_OF_C2SC_ENTRY(unit)-1)
/*SC2C entry index*/
#define TEST_SC2C_ENTRY_MIN   0
#define TEST_SC2C_ENTRY_MAX(unit)               (HWP_8390_50_FAMILY(unit)?(HAL_MAX_NUM_OF_SC2C_ENTRY(unit)-1):(HAL_MAX_NUM_OF_SC2C_ENTRY(unit)*2-1))
/*egrVlanCnvt_RangeCheckVid entry index*/
#define TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MIN            0
#define TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MAX(unit)      (HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)-1)
/* maximum number of multicast portmask entry */
#define TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit)               HAL_MAX_NUM_OF_MCAST_ENTRY(unit)

int32 dal_vlan_table_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_create(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_destroy(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_destroyAll(unit, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;
        uint32 test_round;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            for (test_round = 0; test_round <= 2; test_round++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_create(unit, vid[vid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_create (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);
                    ret = rtk_vlan_destroy(unit, vid[vid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_destroy (unit %u, vid %u)"
                                     , ret, expect_result, unit, vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_create (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                    ret = rtk_vlan_destroy(unit, vid[vid_index]);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_destroy (unit %u, vid %u)"
                                     , ret, RT_ERR_OK, unit, vid[vid_index]);
                }
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        uint32 test_round;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);
            for (test_round = 0; test_round <= 2; test_round++)
            {
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_create(unit, vid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_create (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid);

                    ret = rtk_vlan_destroy(unit, vid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_destroy (unit %u, vid %u)"
                                     , ret, expect_result, unit, vid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_create (unit %u, vid %u)"
                                    , ret, RT_ERR_OK, unit, vid);
                }
            }
            if (expect_result == RT_ERR_OK)
            {
                ret = rtk_vlan_destroy(unit, vid);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_destroy (unit %u, vid %u)"
                                 , ret, RT_ERR_OK, unit, vid);
            }
        }
        ret = rtk_vlan_destroyAll(unit, 1);
        RT_TEST_IS_EQUAL_INT("rtk_vlan_destroyAll (unit %u, restore_vid %u)"
                                        , ret, RT_ERR_OK, 0, 1);
    }
    return RT_ERR_OK;
}

int32 dal_vlan_stg_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  stg_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_stg_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_stg_get(unit, 0, &stg_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_stg_t  stg[UNITTEST_MAX_TEST_VALUE];
        int32  stg_result[UNITTEST_MAX_TEST_VALUE];
        int32  stg_index;
        int32  stg_last;
        uint32  result_stg;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);
        UNITTEST_TEST_VALUE_ASSIGN((HAL_MAX_NUM_OF_MSTI(unit) - 1), 0, stg, stg_result, stg_last);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            for (stg_index = 0; stg_index <= stg_last; stg_index++)
            {
                inter_result3 = stg_result[stg_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Must create vlan first */
                rtk_vlan_create(unit, vid[vid_index]);
                ret = rtk_vlan_stg_set(unit, vid[vid_index], stg[stg_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_set (unit %u, vid %u, stg %u)"
                                    , ret, expect_result, unit, vid[vid_index], stg[stg_index]);

                    ret = rtk_vlan_stg_get(unit, vid[vid_index], &result_stg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u, stg %u)"
                                     , ret, expect_result, unit, vid[vid_index], result_stg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_set/get value unequal (unit %u, vid %u, stg %u)"
                                     , result_stg, stg[stg_index], unit, vid[vid_index], stg[stg_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_stg_set (unit %u, vid %u, stg %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index], stg[stg_index]);
                }
                ret = rtk_vlan_stg_get(unit, vid[vid_index], (uint32*)&result_stg);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                }
                rtk_vlan_destroy(unit, vid[vid_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_stg_t  stg;
        rtk_stg_t  stg_max;
        uint32  result_stg;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);
        G_VALUE(HAL_MAX_NUM_OF_MSTI(unit)-1, stg_max);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);

            for (L_VALUE(0, stg); stg <= stg_max; stg++)
            {
                ASSIGN_EXPECT_RESULT(stg, 0, (HAL_MAX_NUM_OF_MSTI(unit)-1), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                rtk_vlan_create(unit, vid);
                ret = rtk_vlan_stg_set(unit, vid, stg);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_set (unit %u, vid %u, stg %u)"
                                    , ret, expect_result, unit, vid, stg);

                    ret = rtk_vlan_stg_get(unit, vid, &result_stg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u, stg %u)"
                                     , ret, expect_result, unit, vid, result_stg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_set/get value unequal (unit %u, vid %u, stg %u)"
                                     , result_stg, stg, unit, vid, stg);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_stg_set (unit %u, vid %u, stg %u)"
                                        , ret, RT_ERR_OK, unit, vid, stg);
                }
            }
            ret = rtk_vlan_stg_get(unit, vid, &result_stg);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u)"
                                , ret, expect_result, unit, vid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_stg_get (unit %u, vid %u)"
                                    , ret, RT_ERR_OK, unit, vid);
            }
            rtk_vlan_destroy(unit, vid);
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_port_modify_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_create(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_destroy(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_port_add(unit, 0, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_portmask_t  portmask;
        rtk_portmask_t  nullmask;
        rtk_portmask_t  result_member_portmask;
        rtk_portmask_t  result_untag_portmask;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        RTK_PORTMASK_RESET(nullmask);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            rtk_vlan_create(unit, vid[vid_index]);
            for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result3);
                UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Add tagged member */
                RTK_PORTMASK_RESET(portmask);
                ret = rtk_vlan_port_add(unit, vid[vid_index], port, FALSE);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);

                    /* Confirm and delete member port */
                    ret = rtk_vlan_port_get(unit, vid[vid_index], &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_CLEAR(result_member_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                    ret = rtk_vlan_port_del(unit, vid[vid_index], port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, expect_result, unit, vid[vid_index], port);

                    /* SET and CLEAR port from portmask */
                    RTK_PORTMASK_PORT_SET(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid[vid_index], &portmask, &nullmask);
                    RTK_PORTMASK_PORT_CLEAR(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid[vid_index], &portmask, &nullmask);

                    /* Confirm GET result is clean */
                    ret = rtk_vlan_port_get(unit, vid[vid_index], &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_SET(result_member_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                    ret = rtk_vlan_port_del(unit, vid[vid_index], port);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, RT_ERR_OK, unit, vid[vid_index], port);
                }
                /* Add untagged member */
                RTK_PORTMASK_RESET(portmask);
                ret = rtk_vlan_port_add(unit, vid[vid_index], port, TRUE);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);

                    /* Confirm and delete member port */
                    ret = rtk_vlan_port_get(unit, vid[vid_index], &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_CLEAR(result_member_portmask, port) || RTK_PORTMASK_IS_PORT_CLEAR(result_untag_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d), unit %u, vid %u, port %u\n", __LINE__, unit, vid[vid_index], port);
                        return RT_ERR_FAILED;
                    }
                    ret = rtk_vlan_port_del(unit, vid[vid_index], port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, expect_result, unit, vid[vid_index], port);

                    /* SET and CLEAR port from portmask */
                    RTK_PORTMASK_PORT_SET(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid[vid_index], &portmask, &portmask);
                    RTK_PORTMASK_PORT_CLEAR(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid[vid_index], &portmask, &portmask);

                    /* Confirm GET result is clean */
                    ret = rtk_vlan_port_get(unit, vid[vid_index], &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_SET(result_member_portmask, port) || RTK_PORTMASK_IS_PORT_SET(result_untag_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                    ret = rtk_vlan_port_del(unit, vid[vid_index], port);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, RT_ERR_OK, unit, vid[vid_index], port);
                }
            }
            rtk_vlan_destroy(unit, vid[vid_index]);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_port_t  port;
        rtk_port_t  port_max;

        rtk_portmask_t  portmask;
        rtk_portmask_t  nullmask;
        rtk_portmask_t  result_member_portmask;
        rtk_portmask_t  result_untag_portmask;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);
        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);

        RTK_PORTMASK_RESET(nullmask);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);
            rtk_vlan_create(unit, vid);
            for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result3);
                UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Add tagged member */
                RTK_PORTMASK_RESET(portmask);
                ret = rtk_vlan_port_add(unit, vid, port, FALSE);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid);

                    /* Confirm and delete member port */
                    ret = rtk_vlan_port_get(unit, vid, &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_CLEAR(result_member_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                    ret = rtk_vlan_port_del(unit, vid, port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, expect_result, unit, vid, port);

                    /* SET and CLEAR port from portmask */
                    RTK_PORTMASK_PORT_SET(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid, &portmask, &nullmask);
                    RTK_PORTMASK_PORT_CLEAR(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid, &portmask, &nullmask);

                    /* Confirm GET result is clean */
                    ret = rtk_vlan_port_get(unit, vid, &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_SET(result_member_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid);
                    ret = rtk_vlan_port_del(unit, vid, port);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, RT_ERR_OK, unit, vid, port);
                }


                /* Add untagged member */
                RTK_PORTMASK_RESET(portmask);
                ret = rtk_vlan_port_add(unit, vid, port, TRUE);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid);

                    /* Confirm and delete member port */
                    ret = rtk_vlan_port_get(unit, vid, &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_CLEAR(result_member_portmask, port) || RTK_PORTMASK_IS_PORT_CLEAR(result_untag_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                    ret = rtk_vlan_port_del(unit, vid, port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, expect_result, unit, vid, port);

                    /* SET and CLEAR port from portmask */
                    RTK_PORTMASK_PORT_SET(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid, &portmask, &portmask);
                    RTK_PORTMASK_PORT_CLEAR(portmask, port);
                    ret = rtk_vlan_port_set(unit, vid, &portmask, &portmask);

                    /* Confirm GET result is clean */
                    ret = rtk_vlan_port_get(unit, vid, &result_member_portmask, &result_untag_portmask);
                    if (RTK_PORTMASK_IS_PORT_SET(result_member_portmask, port) || RTK_PORTMASK_IS_PORT_SET(result_untag_portmask, port))
                    {
                        osal_printf("FAIL! rtk_vlan_port_set/get value unequa(%d)l\n", __LINE__);
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_add (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid);
                    ret = rtk_vlan_port_del(unit, vid, port);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_port_del (unit %u, vid %u, port %u)"
                                     , ret, RT_ERR_OK, unit, vid, port);
                }
            }
            rtk_vlan_destroy(unit, vid);
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portInnerAcceptFrameType_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_acceptFrameType_t  frame_type_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerAcceptFrameType_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerAcceptFrameType_get(unit, 0, &frame_type_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_acceptFrameType_t  frame_type;
        rtk_vlan_acceptFrameType_t  frame_type_max;
        rtk_vlan_acceptFrameType_t  result_frame_type;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ACCEPT_FRAMETYPE_MAX, frame_type_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ACCEPT_FRAMETYPE_MIN, frame_type); frame_type <= frame_type_max; frame_type++)
            {
                ASSIGN_EXPECT_RESULT(frame_type, TEST_ACCEPT_FRAMETYPE_MIN, TEST_ACCEPT_FRAMETYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerAcceptFrameType_set(unit, port[port_index], frame_type);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                    , ret, expect_result, unit, port[port_index], frame_type);

                    ret = rtk_vlan_portInnerAcceptFrameType_get(unit, port[port_index], &result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u, frame_type %u)"
                                     , ret, expect_result, unit, port[port_index], result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set/get value unequal (unit %u, port %u, frame_type %u)"
                                     , result_frame_type, frame_type, unit, port[port_index], frame_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], frame_type);
                }
            }
            ret = rtk_vlan_portInnerAcceptFrameType_get(unit, port[port_index], &result_frame_type);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_acceptFrameType_t  frame_type;
        rtk_vlan_acceptFrameType_t  frame_type_max;
        rtk_vlan_acceptFrameType_t  result_frame_type;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ACCEPT_FRAMETYPE_MAX, frame_type_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ACCEPT_FRAMETYPE_MIN, frame_type); frame_type <= frame_type_max; frame_type++)
            {
                ASSIGN_EXPECT_RESULT(frame_type, TEST_ACCEPT_FRAMETYPE_MIN, TEST_ACCEPT_FRAMETYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerAcceptFrameType_set(unit, port, frame_type);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                    , ret, expect_result, unit, port, frame_type);

                    ret = rtk_vlan_portInnerAcceptFrameType_get(unit, port, &result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u, frame_type %u)"
                                     , ret, expect_result, unit, port, result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set/get value unequal (unit %u, port %u, frame_type %u)"
                                     , result_frame_type, frame_type, unit, port, frame_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                        , ret, RT_ERR_OK, unit, port, frame_type);
                }
                ret = rtk_vlan_portInnerAcceptFrameType_get(unit, port, &result_frame_type);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerAcceptFrameType_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrFilterEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrFilterEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrFilterEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrFilterEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_vlan_portEgrFilterEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_vlan_portEgrFilterEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
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
                ret = rtk_vlan_portEgrFilterEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_vlan_portEgrFilterEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrFilterEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_vlan_portEgrFilterEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrFilterEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrInnerTpid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_idx_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrInnerTpid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrInnerTpid_get(unit, 0, &tpid_idx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrInnerTpid_set(unit, port[port_index], tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port[port_index], tpid_idx);

                    ret = rtk_vlan_portEgrInnerTpid_get(unit, port[port_index], &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port[port_index], result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port[port_index], tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tpid_idx);
                }
            }
            ret = rtk_vlan_portEgrInnerTpid_get(unit, port[port_index], &result_tpid_idx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrInnerTpid_set(unit, port, tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port, tpid_idx);

                    ret = rtk_vlan_portEgrInnerTpid_get(unit, port, &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port, result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, tpid_idx);
                }
                ret = rtk_vlan_portEgrInnerTpid_get(unit, port, &result_tpid_idx);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTpid_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrOuterTpid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_idx_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrOuterTpid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrOuterTpid_get(unit, 0, &tpid_idx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrOuterTpid_set(unit, port[port_index], tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port[port_index], tpid_idx);

                    ret = rtk_vlan_portEgrOuterTpid_get(unit, port[port_index], &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port[port_index], result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port[port_index], tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tpid_idx);
                }
            }
            ret = rtk_vlan_portEgrOuterTpid_get(unit, port[port_index], &result_tpid_idx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrOuterTpid_set(unit, port, tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port, tpid_idx);

                    ret = rtk_vlan_portEgrOuterTpid_get(unit, port, &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port, result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, tpid_idx);
                }
                ret = rtk_vlan_portEgrOuterTpid_get(unit, port, &result_tpid_idx);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTpid_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrTagKeepEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp, enable2_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrTagKeepEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrTagKeepEnable_get(unit, 0, &enable_temp, &enable2_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;
        rtk_enable_t  enable2 = 0;
        rtk_enable_t  enable2_max;
        rtk_enable_t  result_enable2;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable2_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable2); enable2 <= enable_max; enable2++)
                {
                    ASSIGN_EXPECT_RESULT(enable2, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portEgrTagKeepEnable_set(unit, port[port_index], enable, enable2);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                        , ret, expect_result, unit, port[port_index], enable, enable2);

                        ret = rtk_vlan_portEgrTagKeepEnable_get(unit, port[port_index], &result_enable, &result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u, enable %u, enable2 %u)"
                                         , ret, expect_result, unit, port[port_index], result_enable, result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                         , result_enable, enable, unit, port[port_index], enable);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable2 %u)"
                                         , result_enable2, enable2, unit, port[port_index], enable2);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], enable, enable2);
                    }
                }
            }
            ret = rtk_vlan_portEgrTagKeepEnable_get(unit, port[port_index], &result_enable, &result_enable2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
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
        rtk_enable_t  enable2;
        rtk_enable_t  enable2_max;
        rtk_enable_t  result_enable2;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable2_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable2); enable2 <= enable2_max; enable2++)
                {
                    ASSIGN_EXPECT_RESULT(enable2, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portEgrTagKeepEnable_set(unit, port, enable, enable2);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                        , ret, expect_result, unit, port, enable, enable2);

                        ret = rtk_vlan_portEgrTagKeepEnable_get(unit, port, &result_enable, &result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u, enable %u, enable2 %u)"
                                         , ret, expect_result, unit, port, result_enable, result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                         , result_enable, enable, unit, port, enable);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable2 %u)"
                                         , result_enable2, enable2, unit, port, enable2);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                            , ret, RT_ERR_OK, unit, port, enable, enable2);
                    }
                    ret = rtk_vlan_portEgrTagKeepEnable_get(unit, port, &result_enable, &result_enable2);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u)"
                                        , ret, expect_result, unit, port);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepEnable_get (unit %u, port %u)"
                                            , ret, RT_ERR_OK, unit, port);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portIgrExtraTagEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrExtraTagEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrExtraTagEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;

        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrExtraTagEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);

                    ret = rtk_vlan_portIgrExtraTagEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_vlan_portIgrExtraTagEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
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
                ret = rtk_vlan_portIgrExtraTagEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_vlan_portIgrExtraTagEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_vlan_portIgrExtraTagEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrExtraTagEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_mcastLeakyEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  result_enable;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_mcastLeakyEnable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_mcastLeakyEnable_get(unit, &result_enable)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        UNITTEST_TEST_VALUE_ASSIGN((RTK_ENABLE_END - 1), 0, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_mcastLeakyEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_mcastLeakyEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_vlan_mcastLeakyEnable_get(unit, (rtk_enable_t *)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_mcastLeakyEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_mcastLeakyEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_mcastLeakyEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }

        ret = rtk_vlan_mcastLeakyEnable_get(unit, (rtk_enable_t*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_mcastLeakyEnable_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_mcastLeakyEnable_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }


    return RT_ERR_OK;
}

int32 dal_vlan_portIgrInnerTpid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_idx_temp;
    int8  inter_result1 = 1;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrInnerTpid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrInnerTpid_get(unit, 0, &tpid_idx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_TPIDIDX_MASK_MAX(unit), tpid_idx_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TPIDIDX_MASK_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPIDIDX_MASK_MIN, TEST_TPIDIDX_MASK_MAX(unit), inter_result3);
                expect_result = (inter_result1 && inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrInnerTpid_set(unit, port[port_index], tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port[port_index], tpid_idx);

                    ret = rtk_vlan_portIgrInnerTpid_get(unit, port[port_index], &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port[port_index], result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port[port_index], tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tpid_idx);
                }
            }
            ret = rtk_vlan_portIgrInnerTpid_get(unit, port[port_index], &result_tpid_idx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result1 && inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TPIDIDX_MASK_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TPIDIDX_MASK_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPIDIDX_MASK_MIN, TEST_TPIDIDX_MASK_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrInnerTpid_set(unit, port, tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port, tpid_idx);

                    ret = rtk_vlan_portIgrInnerTpid_get(unit, port, &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port, result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrInnerTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, tpid_idx);
                }
                ret = rtk_vlan_portIgrInnerTpid_get(unit, port, &result_tpid_idx);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrInnerTpid_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portIgrOuterTpid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_idx_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrOuterTpid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrOuterTpid_get(unit, 0, &tpid_idx_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_TPIDIDX_MASK_MAX(unit), tpid_idx_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TPIDIDX_MASK_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPIDIDX_MASK_MIN, TEST_TPIDIDX_MASK_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrOuterTpid_set(unit, port[port_index], tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port[port_index], tpid_idx);

                    ret = rtk_vlan_portIgrOuterTpid_get(unit, port[port_index], &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port[port_index], result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port[port_index], tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tpid_idx);
                }
            }
            ret = rtk_vlan_portIgrOuterTpid_get(unit, port[port_index], &result_tpid_idx);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  result_tpid_idx;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TPIDIDX_MASK_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TPIDIDX_MASK_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
            {
                ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPIDIDX_MASK_MIN, TEST_TPIDIDX_MASK_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrOuterTpid_set(unit, port, tpid_idx);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                    , ret, expect_result, unit, port, tpid_idx);

                    ret = rtk_vlan_portIgrOuterTpid_get(unit, port, &result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u, tpid_idx %u)"
                                     , ret, expect_result, unit, port, result_tpid_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set/get value unequal (unit %u, port %u, tpid_idx %u)"
                                     , result_tpid_idx, tpid_idx, unit, port, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrOuterTpid_set (unit %u, port %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, tpid_idx);
                }
                ret = rtk_vlan_portIgrOuterTpid_get(unit, port, &result_tpid_idx);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrOuterTpid_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portIgrTagKeepEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp, enable2_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrTagKeepEnable_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrTagKeepEnable_get(unit, 0, &enable_temp, &enable2_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        rtk_enable_t  enable2;
        rtk_enable_t  enable2_max;
        rtk_enable_t  result_enable2;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable2_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable2); enable2 <= enable_max; enable2++)
                {
                    ASSIGN_EXPECT_RESULT(enable2, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portIgrTagKeepEnable_set(unit, port[port_index], enable, enable2);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                        , ret, expect_result, unit, port[port_index], enable, enable2);

                        ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port[port_index], &result_enable, &result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u, enable %u, enable2 %u)"
                                         , ret, expect_result, unit, port[port_index], result_enable, result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                         , result_enable, enable, unit, port[port_index], enable);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable2 %u)"
                                         , result_enable2, enable2, unit, port[port_index], enable2);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], enable, enable2);
                    }
                }
            }
            ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port[port_index], &result_enable, &result_enable2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
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

        rtk_enable_t  enable2;
        rtk_enable_t  enable2_max;
        rtk_enable_t  result_enable2;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable2_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                 for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable2); enable2 <= enable2_max; enable2++)
                {
                    ASSIGN_EXPECT_RESULT(enable2, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portIgrTagKeepEnable_set(unit, port, enable, enable2);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                        , ret, expect_result, unit, port, enable, enable);

                        ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port, &result_enable, &result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u, enable %u, enable2 %u)"
                                         , ret, expect_result, unit, port, result_enable, result_enable2);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                         , result_enable, enable, unit, port, enable);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                         , result_enable2, enable2, unit, port, enable);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_set (unit %u, port %u, enable %u, enable2 %u)"
                                            , ret, RT_ERR_OK, unit, port, enable, enable2);
                    }
                    ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port, &result_enable, &result_enable2);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u)"
                                        , ret, expect_result, unit, port);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepEnable_get (unit %u, port %u)"
                                            , ret, RT_ERR_OK, unit, port);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portOuterAcceptFrameType_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_acceptFrameType_t  frame_type_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterAcceptFrameType_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterAcceptFrameType_get(unit, 0, &frame_type_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_acceptFrameType_t  frame_type;
        rtk_vlan_acceptFrameType_t  frame_type_max;
        rtk_vlan_acceptFrameType_t  result_frame_type;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ACCEPT_FRAMETYPE_MAX, frame_type_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ACCEPT_FRAMETYPE_MIN, frame_type); frame_type <= frame_type_max; frame_type++)
            {
                ASSIGN_EXPECT_RESULT(frame_type, TEST_ACCEPT_FRAMETYPE_MIN, TEST_ACCEPT_FRAMETYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterAcceptFrameType_set(unit, port[port_index], frame_type);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                    , ret, expect_result, unit, port[port_index], frame_type);

                    ret = rtk_vlan_portOuterAcceptFrameType_get(unit, port[port_index], &result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u, frame_type %u)"
                                     , ret, expect_result, unit, port[port_index], result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set/get value unequal (unit %u, port %u, frame_type %u)"
                                     , result_frame_type, frame_type, unit, port[port_index], frame_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], frame_type);
                }
            }
            ret = rtk_vlan_portOuterAcceptFrameType_get(unit, port[port_index], &result_frame_type);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_acceptFrameType_t  frame_type;
        rtk_vlan_acceptFrameType_t  frame_type_max;
        rtk_vlan_acceptFrameType_t  result_frame_type;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ACCEPT_FRAMETYPE_MAX, frame_type_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ACCEPT_FRAMETYPE_MIN, frame_type); frame_type <= frame_type_max; frame_type++)
            {
                ASSIGN_EXPECT_RESULT(frame_type, TEST_ACCEPT_FRAMETYPE_MIN, TEST_ACCEPT_FRAMETYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterAcceptFrameType_set(unit, port, frame_type);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                    , ret, expect_result, unit, port, frame_type);

                    ret = rtk_vlan_portOuterAcceptFrameType_get(unit, port, &result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u, frame_type %u)"
                                     , ret, expect_result, unit, port, result_frame_type);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set/get value unequal (unit %u, port %u, frame_type %u)"
                                     , result_frame_type, frame_type, unit, port, frame_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_set (unit %u, port %u, frame_type %u)"
                                        , ret, RT_ERR_OK, unit, port, frame_type);
                }
                ret = rtk_vlan_portOuterAcceptFrameType_get(unit, port, &result_frame_type);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterAcceptFrameType_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portOuterPvid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  pvid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterPvid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterPvid_get(unit, 0, &pvid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_t  pvid[UNITTEST_MAX_TEST_VALUE];
        int32  pvid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pvid_index;
        int32  pvid_last;

        int32  result_pvid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, pvid, pvid_result, pvid_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (pvid_index = 0; pvid_index <= pvid_last; pvid_index++)
            {
                inter_result3 = pvid_result[pvid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterPvid_set(unit, port[port_index], pvid[pvid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u, pvid %u)"
                                    , ret, expect_result, unit, port[port_index], pvid[pvid_index]);

                    ret = rtk_vlan_portOuterPvid_get(unit, port[port_index], (uint32*)&result_pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_get (unit %u, port %u, pvid %u)"
                                     , ret, expect_result, unit, port[port_index], pvid[pvid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_set/get value unequal (unit %u, port %u, pvid %u)"
                                     , result_pvid, pvid[pvid_index], unit, port[port_index], pvid[pvid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u, pvid %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pvid[pvid_index]);
                }
            }

            ret = rtk_vlan_portOuterPvid_get(unit, port[port_index], (uint32*)&result_pvid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_t  pvid;
        rtk_vlan_t  pvid_max;
        rtk_vlan_t  result_pvid;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_ID_MAX, pvid_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, pvid); pvid <= pvid_max; pvid++)
            {
                ASSIGN_EXPECT_RESULT(pvid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterPvid_set(unit, port, pvid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u, pvid %u)"
                                    , ret, expect_result, unit, port, pvid);
                    ret = rtk_vlan_portOuterPvid_get(unit, port, (uint32*)&result_pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_get (unit %u, port %u, pvid %u)"
                                     , ret, expect_result, unit, port, pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvid_set/get value unequal (unit %u, port %u, pvid %u)"
                                     , result_pvid, pvid, unit, port, pvid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvid_set (unit %u, port %u, pvid %u)"
                                        , ret, RT_ERR_OK, unit, port, pvid);
            }

        }
    }



    return RT_ERR_OK;
}

int32 dal_vlan_portProtoVlan_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32 ret1;
    rtk_vlan_protoVlanCfg_t  vlancfg_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portProtoVlan_set(unit, 0, 0, &vlancfg_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portProtoVlan_get(unit, 0, 0, &vlancfg_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;
        uint32  protoGroup_idx;
        uint32  protoGroup_idx_max;

        rtk_vlan_protoVlanCfg_t vlancfg;
        rtk_vlan_protoVlanCfg_t result_vlancfg;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_PRTOCOGRP_IDX_MAX(unit), protoGroup_idx_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_PRTOCOGRP_IDX_MIN, protoGroup_idx); protoGroup_idx <= protoGroup_idx_max; protoGroup_idx++)
            {
                ASSIGN_EXPECT_RESULT(protoGroup_idx, TEST_PRTOCOGRP_IDX_MIN, TEST_PRTOCOGRP_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                vlancfg.vid = ut_rand()%4096;
                vlancfg.pri = ut_rand()%8;
                ret = rtk_vlan_portProtoVlan_set(unit, port[port_index], protoGroup_idx, &vlancfg);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                    , ret, expect_result, unit, port[port_index], protoGroup_idx);

                    ret = rtk_vlan_portProtoVlan_get(unit, port[port_index], protoGroup_idx, &result_vlancfg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                     , ret, expect_result, unit, port[port_index], protoGroup_idx);
                    if((vlancfg.vid != result_vlancfg.vid) ||(vlancfg.pri != result_vlancfg.pri))
                    {
                        osal_printf("rtk_vlan_portProtoVlan_set/get value unequal (unit %u, port %u, protoGroup_idx %u)\n",
                                     unit, port[port_index], protoGroup_idx);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], protoGroup_idx);
                }
            }
#if defined (CONFIG_SDK_RTL8390)
            if(HWP_8390_50_FAMILY(unit))
                vlancfg.valid = 0;
#endif
#if defined (CONFIG_SDK_RTL9300) || defined (CONFIG_SDK_RTL9310)
            if(HWP_9310_FAMILY_ID(unit)||HWP_9300_FAMILY_ID(unit))
            {
                vlancfg.pri_assign = ut_rand()%2;
                vlancfg.vid_assign = ut_rand()%2;
                vlancfg.vlan_type = ut_rand()%2;
            }
#endif

            ret = rtk_vlan_portProtoVlan_set(unit, port[port_index], protoGroup_idx, &vlancfg);
            ret1 = rtk_vlan_portProtoVlan_get(unit, port[port_index], protoGroup_idx, &result_vlancfg);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                , ret,  expect_result, unit, port[port_index], protoGroup_idx);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                , ret1, expect_result, unit, port[port_index], protoGroup_idx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index], protoGroup_idx);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                    , ret1, RT_ERR_OK, unit, port[port_index], protoGroup_idx);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  protoGroup_idx;
        uint32  protoGroup_idx_max;

        rtk_vlan_protoVlanCfg_t vlancfg;
        rtk_vlan_protoVlanCfg_t result_vlancfg;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PRTOCOGRP_IDX_MAX(unit), protoGroup_idx_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_PRTOCOGRP_IDX_MIN, protoGroup_idx); protoGroup_idx <= protoGroup_idx_max; protoGroup_idx++)
            {
                ASSIGN_EXPECT_RESULT(protoGroup_idx, TEST_PRTOCOGRP_IDX_MIN, TEST_PRTOCOGRP_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                vlancfg.vid = ut_rand()%4096;
                vlancfg.pri = ut_rand()%8;

                ret = rtk_vlan_portProtoVlan_set(unit, port, protoGroup_idx, &vlancfg);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                    , ret, expect_result, unit, port, protoGroup_idx);

                    ret = rtk_vlan_portProtoVlan_get(unit, port, protoGroup_idx, &result_vlancfg);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                     , ret, expect_result, unit, port, protoGroup_idx);

                    if((vlancfg.vid != result_vlancfg.vid) ||(vlancfg.pri != result_vlancfg.pri))
                    {
                        osal_printf("rtk_vlan_portProtoVlan_set/get value unequal (unit %u, port %u, protoGroup_idx %u)\n",
                                     unit, port, protoGroup_idx);
                    }
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, protoGroup_idx);
                }
#if defined (CONFIG_SDK_RTL8390)
                if(HWP_8390_50_FAMILY(unit))
                    vlancfg.valid = 0;
#endif
#if defined (CONFIG_SDK_RTL9300) || defined (CONFIG_SDK_RTL9310)
                if(HWP_9310_FAMILY_ID(unit)||HWP_9300_FAMILY_ID(unit))
                {
                    vlancfg.pri_assign = ut_rand()%2;
                    vlancfg.vid_assign = ut_rand()%2;
                    vlancfg.vlan_type = ut_rand()%2;
                }
#endif

                ret = rtk_vlan_portProtoVlan_set(unit, port, protoGroup_idx, &vlancfg);
                ret1 = rtk_vlan_portProtoVlan_get(unit, port, protoGroup_idx, &result_vlancfg);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                    , ret1, expect_result, unit, port, protoGroup_idx);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                    , ret1, expect_result, unit, port, protoGroup_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_set (unit %u, port %u, protoGroup_idx %u)"
                                        , ret, RT_ERR_OK, unit, port, protoGroup_idx);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portProtoVlan_get (unit %u, port %u, protoGroup_idx %u)"
                                        , ret1, RT_ERR_OK, unit, port, protoGroup_idx);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_protoGroup_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_protoGroup_t  protogroup_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_protoGroup_set(unit, 0, &protogroup_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_protoGroup_get(unit, 0, &protogroup_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  protoGroup_idx;
        uint32  protoGroup_idx_max;

        rtk_vlan_protoGroup_t protogroup;
        rtk_vlan_protoGroup_t result_protogroup;

        G_VALUE(TEST_PRTOCOGRP_IDX_MAX(unit), protoGroup_idx_max);

        for (L_VALUE(TEST_PRTOCOGRP_IDX_MIN, protoGroup_idx); protoGroup_idx <= protoGroup_idx_max; protoGroup_idx++)
        {
            ASSIGN_EXPECT_RESULT(protoGroup_idx, TEST_PRTOCOGRP_IDX_MIN, TEST_PRTOCOGRP_IDX_MAX(unit), inter_result2);
            protogroup.frametype = ut_rand()%FRAME_TYPE_END;
            protogroup.framevalue = ut_rand()%4095;
            if((protogroup.frametype != FRAME_TYPE_SNAP8021H) && (protogroup.frametype != FRAME_TYPE_SNAPOTHER))
                inter_result3 = 1;
            else
                inter_result3 = 0;
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_protoGroup_set(unit, protoGroup_idx, &protogroup);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_set (unit %u, protoGroup_idx %u, frametype %u, framevalue %u)"
                                , ret, expect_result, unit, protoGroup_idx, protogroup.frametype, protogroup.framevalue);

                ret = rtk_vlan_protoGroup_get(unit, protoGroup_idx, &result_protogroup);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                                 , ret, expect_result, unit, protoGroup_idx);
                if((protogroup.frametype != result_protogroup.frametype) ||(protogroup.framevalue != result_protogroup.framevalue))
                {
                    osal_printf("rtk_vlan_protoGroup_set/get value unequal (%u %u unit %u, protoGroup_idx %u, frametype %u, framevalue %u)\n",
                                 result_protogroup.frametype, protogroup.framevalue, unit, protoGroup_idx, protogroup.frametype, protogroup.framevalue);
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_protoGroup_set (unit %u, protoGroup_idx %u, frametype %u, framevalue %u)"
                                , ret, RT_ERR_OK, unit, protoGroup_idx, protogroup.frametype, protogroup.framevalue);
            }
        }
        ret = rtk_vlan_protoGroup_get(unit, protoGroup_idx, &result_protogroup);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                            , ret, expect_result, unit, protoGroup_idx);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                            , ret, RT_ERR_OK, unit, protoGroup_idx);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  protoGroup_idx;
        uint32  protoGroup_idx_max;
        rtk_vlan_protoGroup_t protogroup;
        rtk_vlan_protoGroup_t result_protogroup;

        G_VALUE(TEST_PRTOCOGRP_IDX_MAX(unit), protoGroup_idx_max);

        for (L_VALUE(TEST_PRTOCOGRP_IDX_MIN, protoGroup_idx); protoGroup_idx <= protoGroup_idx_max; protoGroup_idx++)
        {
            ASSIGN_EXPECT_RESULT(protoGroup_idx, TEST_PRTOCOGRP_IDX_MIN, TEST_PRTOCOGRP_IDX_MAX(unit), inter_result2);
            protogroup.frametype = ut_rand()%FRAME_TYPE_END;
            protogroup.framevalue = ut_rand()%4095;
            if((protogroup.frametype != FRAME_TYPE_SNAP8021H) && (protogroup.frametype != FRAME_TYPE_SNAPOTHER))
                inter_result3 = 1;
            else
                inter_result3 = 0;
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_protoGroup_set(unit, protoGroup_idx, &protogroup);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_set (unit %u, protoGroup_idx %u, frametype %u, framevalue %u)"
                                , ret, expect_result, unit, protoGroup_idx, protogroup.frametype, protogroup.framevalue);

                ret = rtk_vlan_protoGroup_get(unit, protoGroup_idx, &result_protogroup);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                                 , ret, expect_result, unit, protoGroup_idx);

                if((protogroup.frametype != result_protogroup.frametype) ||(protogroup.framevalue != result_protogroup.framevalue))
                {
                    osal_printf("rtk_vlan_protoGroup_set/get value unequal (unit %u, protoGroup_idx %u)\n",
                                 unit, protoGroup_idx);
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_protoGroup_set (unit %u, protoGroup_idx %u)"
                                    , ret, RT_ERR_OK, unit, protoGroup_idx);
            }
            ret = rtk_vlan_protoGroup_get(unit, protoGroup_idx, &result_protogroup);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                                , ret, expect_result, unit, protoGroup_idx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_protoGroup_get (unit %u, protoGroup_idx %u)"
                                    , ret, RT_ERR_OK, unit, protoGroup_idx);
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portInnerPvid_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_t  pvid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerPvid_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerPvid_get(unit, 0, &pvid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_t  pvid[UNITTEST_MAX_TEST_VALUE];
        int32  pvid_result[UNITTEST_MAX_TEST_VALUE];
        int32  pvid_index;
        int32  pvid_last;

        int32  result_pvid;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, pvid, pvid_result, pvid_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (pvid_index = 0; pvid_index <= pvid_last; pvid_index++)
            {
                inter_result3 = pvid_result[pvid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerPvid_set(unit, port[port_index], pvid[pvid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u, pvid %u)"
                                    , ret, expect_result, unit, port[port_index], pvid[pvid_index]);

                    ret = rtk_vlan_portInnerPvid_get(unit, port[port_index], (uint32*)&result_pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_get (unit %u, port %u, pvid %u)"
                                     , ret, expect_result, unit, port[port_index], pvid[pvid_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_set/get value unequal (unit %u, port %u, pvid %u)"
                                     , result_pvid, pvid[pvid_index], unit, port[port_index], pvid[pvid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u, pvid %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], pvid[pvid_index]);
                }
            }

            ret = rtk_vlan_portInnerPvid_get(unit, port[port_index], (uint32*)&result_pvid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_t  pvid;
        rtk_vlan_t  pvid_max;
        rtk_vlan_t  result_pvid;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_ID_MAX, pvid_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_ID_MIN, pvid); pvid <= pvid_max; pvid++)
            {
                ASSIGN_EXPECT_RESULT(pvid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerPvid_set(unit, port, pvid);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u, pvid %u)"
                                    , ret, expect_result, unit, port, pvid);
                    ret = rtk_vlan_portInnerPvid_get(unit, port, (uint32*)&result_pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_get (unit %u, port %u, pvid %u)"
                                     , ret, expect_result, unit, port, pvid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvid_set/get value unequal (unit %u, port %u, pvid %u)"
                                     , result_pvid, pvid, unit, port, pvid);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvid_set (unit %u, port %u, pvid %u)"
                                        , ret, RT_ERR_OK, unit, port, pvid);
            }

        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_l2UcastLookupMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_l2LookupMode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_l2UcastLookupMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_l2UcastLookupMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_vlan_l2LookupMode_t  mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        uint32  result_mode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_UC_LOOKUP_MODE_MAX, TEST_UC_LOOKUP_MODE_MIN, mode, mode_result, mode_last);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            for (mode_index = 0; mode_index <= mode_last; mode_index++)
            {
                inter_result3 = mode_result[mode_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Must create vlan first */
                rtk_vlan_create(unit, vid[vid_index]);
                ret = rtk_vlan_l2UcastLookupMode_set(unit, vid[vid_index], mode[mode_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set (unit %u, vid %u, mode %u)"
                                    , ret, expect_result, unit, vid[vid_index], mode[mode_index]);

                    ret = rtk_vlan_l2UcastLookupMode_get(unit, vid[vid_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u, mode %u)"
                                     , ret, expect_result, unit, vid[vid_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set/get value unequal (unit %u, vid %u, mode %u)"
                                     , result_mode, mode[mode_index], unit, vid[vid_index], mode[mode_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set (unit %u, vid %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index], mode[mode_index]);
                }
                ret = rtk_vlan_l2UcastLookupMode_get(unit, vid[vid_index], (uint32*)&result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                }
                rtk_vlan_destroy(unit, vid[vid_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_vlan_l2LookupMode_t  mode;
        rtk_vlan_l2LookupMode_t  mode_max;

        uint32  result_mode;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);
        G_VALUE(TEST_UC_LOOKUP_MODE_MAX, mode_max);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);

            for (L_VALUE(TEST_UC_LOOKUP_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_UC_LOOKUP_MODE_MIN, TEST_UC_LOOKUP_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                rtk_vlan_create(unit, vid);
                ret = rtk_vlan_l2UcastLookupMode_set(unit, vid, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set (unit %u, vid %u, mode %u)"
                                    , ret, expect_result, unit, vid, mode);

                    ret = rtk_vlan_l2UcastLookupMode_get(unit, vid, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u, mode %u)"
                                     , ret, expect_result, unit, vid, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set/get value unequal (unit %u, vid %u, mode %u)"
                                     , result_mode, mode, unit, vid, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2UcastLookupMode_set (unit %u, vid %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, vid, mode);
                }
            }
            ret = rtk_vlan_l2UcastLookupMode_get(unit, vid, &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u)"
                                , ret, expect_result, unit, vid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2UcastLookupMode_get (unit %u, vid %u)"
                                    , ret, RT_ERR_OK, unit, vid);
            }
            rtk_vlan_destroy(unit, vid);
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_l2McastLookupMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_l2LookupMode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_l2McastLookupMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_l2McastLookupMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        rtk_vlan_l2LookupMode_t  mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;
        uint32  result_mode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_MC_LOOKUP_MODE_MAX, TEST_MC_LOOKUP_MODE_MIN, mode, mode_result, mode_last);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            for (mode_index = 0; mode_index <= mode_last; mode_index++)
            {
                inter_result3 = mode_result[mode_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Must create vlan first */
                rtk_vlan_create(unit, vid[vid_index]);
                ret = rtk_vlan_l2McastLookupMode_set(unit, vid[vid_index], mode[mode_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_set (unit %u, vid %u, mode %u)"
                                    , ret, expect_result, unit, vid[vid_index], mode[mode_index]);

                    ret = rtk_vlan_l2McastLookupMode_get(unit, vid[vid_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u, mode %u)"
                                     , ret, expect_result, unit, vid[vid_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_set/get value unequal (unit %u, vid %u, mode %u)"
                                     , result_mode, mode[mode_index], unit, vid[vid_index], mode[mode_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2McastLookupMode_set (unit %u, vid %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index], mode[mode_index]);
                }
                ret = rtk_vlan_l2McastLookupMode_get(unit, vid[vid_index], (uint32*)&result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                }
                rtk_vlan_destroy(unit, vid[vid_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        rtk_vlan_l2LookupMode_t  mode;
        rtk_vlan_l2LookupMode_t  mode_max;

        uint32  result_mode;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);
        G_VALUE(TEST_MC_LOOKUP_MODE_MAX, mode_max);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);

            for (L_VALUE(TEST_MC_LOOKUP_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_MC_LOOKUP_MODE_MIN, TEST_MC_LOOKUP_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                rtk_vlan_create(unit, vid);
                ret = rtk_vlan_l2McastLookupMode_set(unit, vid, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_set (unit %u, vid %u, mode %u)"
                                    , ret, expect_result, unit, vid, mode);

                    ret = rtk_vlan_l2McastLookupMode_get(unit, vid, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u, mode %u)"
                                     , ret, expect_result, unit, vid, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_set/get value unequal (unit %u, vid %u, mode %u)"
                                     , result_mode, mode, unit, vid, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2McastLookupMode_set (unit %u, vid %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, vid, mode);
                }
            }
            ret = rtk_vlan_l2McastLookupMode_get(unit, vid, &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u)"
                                , ret, expect_result, unit, vid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_l2McastLookupMode_get (unit %u, vid %u)"
                                    , ret, RT_ERR_OK, unit, vid);
            }
            rtk_vlan_destroy(unit, vid);
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_profileIdx_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  index_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_profileIdx_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_profileIdx_get(unit, 0, &index_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid[UNITTEST_MAX_TEST_VALUE];
        int32  vid_result[UNITTEST_MAX_TEST_VALUE];
        int32  vid_index;
        int32  vid_last;

        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last;
        uint32  result_index;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vid, vid_result, vid_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_PROFILE_IDX_MAX(unit), TEST_VLAN_PROFILE_IDX_MIN, index, index_result, index_last);

        for (vid_index = 0; vid_index <= vid_last; vid_index++)
        {
            inter_result2 = vid_result[vid_index];
            for (index_index = 0; index_index <= index_last; index_index++)
            {
                inter_result3 = index_result[index_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                /* Must create vlan first */
                rtk_vlan_create(unit, vid[vid_index]);
                ret = rtk_vlan_profileIdx_set(unit, vid[vid_index], index[index_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_set (unit %u, vid %u, index %u)"
                                    , ret, expect_result, unit, vid[vid_index], index[index_index]);

                    ret = rtk_vlan_profileIdx_get(unit, vid[vid_index], &result_index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u, index %u)"
                                     , ret, expect_result, unit, vid[vid_index], result_index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_set/get value unequal (unit %u, vid %u, index %u)"
                                     , result_index, index[index_index], unit, vid[vid_index], index[index_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profileIdx_set (unit %u, vid %u, index %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index], index[index_index]);
                }
                ret = rtk_vlan_profileIdx_get(unit, vid[vid_index], (uint32*)&result_index);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u)"
                                    , ret, expect_result, unit, vid[vid_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u)"
                                        , ret, RT_ERR_OK, unit, vid[vid_index]);
                }
                rtk_vlan_destroy(unit, vid[vid_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_vlan_t  vid;
        rtk_vlan_t  vid_max;
        uint32  index;
        uint32  index_max;
        uint32  result_index;

        G_VALUE(TEST_VLAN_ID_MAX, vid_max);
        G_VALUE(TEST_VLAN_PROFILE_IDX_MAX(unit), index_max);

        for (L_VALUE(TEST_VLAN_ID_MIN, vid); vid <= vid_max; vid++)
        {
            ASSIGN_EXPECT_RESULT(vid, TEST_VLAN_ID_MIN, TEST_VLAN_ID_MAX, inter_result2);

            for (L_VALUE(TEST_VLAN_PROFILE_IDX_MIN, index); index <= index_max; index++)
            {
                ASSIGN_EXPECT_RESULT(index, TEST_VLAN_PROFILE_IDX_MIN, TEST_VLAN_PROFILE_IDX_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                rtk_vlan_create(unit, vid);
                ret = rtk_vlan_profileIdx_set(unit, vid, index);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_set (unit %u, vid %u, index %u)"
                                    , ret, expect_result, unit, vid, index);

                    ret = rtk_vlan_profileIdx_get(unit, vid, &result_index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u, index %u)"
                                     , ret, expect_result, unit, vid, result_index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_set/get value unequal (unit %u, vid %u, index %u)"
                                     , result_index, index, unit, vid, index);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profileIdx_set (unit %u, vid %u, index %u)"
                                        , ret, RT_ERR_OK, unit, vid, index);
                }
            }
            ret = rtk_vlan_profileIdx_get(unit, vid, &result_index);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u)"
                                , ret, expect_result, unit, vid);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profileIdx_get (unit %u, vid %u)"
                                    , ret, RT_ERR_OK, unit, vid);
            }
            rtk_vlan_destroy(unit, vid);
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_profile_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_profile_t  profile_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_profile_set(unit, 0, &profile_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_profile_get(unit, 0, &profile_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  profile_idx;
        uint32  profile_idx_max;

        rtk_vlan_profile_t profile;
        rtk_vlan_profile_t result_profile;

        G_VALUE(TEST_VLAN_PROFILE_IDX_MAX(unit), profile_idx_max);

        for (L_VALUE(TEST_VLAN_PROFILE_IDX_MIN, profile_idx); profile_idx <= profile_idx_max; profile_idx++)
        {
            ASSIGN_EXPECT_RESULT(profile_idx, TEST_VLAN_PROFILE_IDX_MIN, TEST_VLAN_PROFILE_IDX_MAX(unit), inter_result2);
            profile.learn = ut_rand()%2;
            profile.l2_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            profile.ip4_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            profile.ip6_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_profile_set(unit, profile_idx, &profile);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_set (unit %u, profile_idx %u, learn %u, l2_mcast_dlf_pm_idx %u, ip4_mcast_dlf_pm_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                , ret, expect_result, unit, profile_idx, profile.learn, profile.l2_mcast_dlf_pm_idx, profile.ip4_mcast_dlf_pm_idx, profile.ip6_mcast_dlf_pm_idx);

                ret = rtk_vlan_profile_get(unit, profile_idx, &result_profile);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                                 , ret, expect_result, unit, profile_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, learn %u)"
                                 , profile.learn, result_profile.learn, unit, profile_idx, profile.learn);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, l2_mcast_dlf_pm_idx %u)"
                                 , profile.l2_mcast_dlf_pm_idx, result_profile.l2_mcast_dlf_pm_idx, unit, profile_idx, profile.l2_mcast_dlf_pm_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, ip4_mcast_dlf_pm_idx %u)"
                                 , profile.ip4_mcast_dlf_pm_idx, result_profile.ip4_mcast_dlf_pm_idx, unit, profile_idx, profile.ip4_mcast_dlf_pm_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                 , profile.ip6_mcast_dlf_pm_idx, result_profile.ip6_mcast_dlf_pm_idx, unit, profile_idx, profile.ip6_mcast_dlf_pm_idx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profile_set (unit %u, profile_idx %u, learn %u, l2_mcast_dlf_pm_idx %u, ip4_mcast_dlf_pm_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                , ret, RT_ERR_OK, unit, profile_idx, profile.learn, profile.l2_mcast_dlf_pm_idx, profile.ip4_mcast_dlf_pm_idx, profile.ip6_mcast_dlf_pm_idx);
            }
        }
        ret = rtk_vlan_profile_get(unit, profile_idx, &result_profile);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                            , ret, expect_result, unit, profile_idx);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                            , ret, RT_ERR_OK, unit, profile_idx);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  profile_idx;
        uint32  profile_idx_max;

        rtk_vlan_profile_t profile;
        rtk_vlan_profile_t result_profile;

        G_VALUE(TEST_VLAN_PROFILE_IDX_MAX(unit), profile_idx_max);

        for (L_VALUE(TEST_VLAN_PROFILE_IDX_MIN, profile_idx); profile_idx <= profile_idx_max; profile_idx++)
        {
            ASSIGN_EXPECT_RESULT(profile_idx, TEST_VLAN_PROFILE_IDX_MIN, TEST_VLAN_PROFILE_IDX_MAX(unit), inter_result2);
            profile.learn = ut_rand()%2;
            profile.l2_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            profile.ip4_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            profile.ip6_mcast_dlf_pm_idx = ut_rand()%TEST_HAL_MAX_NUM_OF_MCAST_ENTRY(unit);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_profile_set(unit, profile_idx, &profile);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_set (unit %u, profile_idx %u, learn %u, l2_mcast_dlf_pm_idx %u, ip4_mcast_dlf_pm_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                , ret, expect_result, unit, profile_idx, profile.learn, profile.l2_mcast_dlf_pm_idx, profile.ip4_mcast_dlf_pm_idx, profile.ip6_mcast_dlf_pm_idx);

                ret = rtk_vlan_profile_get(unit, profile_idx, &result_profile);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                                 , ret, expect_result, unit, profile_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, learn %u)"
                                 , profile.learn, result_profile.learn, unit, profile_idx, profile.learn);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, l2_mcast_dlf_pm_idx %u)"
                                 , profile.l2_mcast_dlf_pm_idx, result_profile.l2_mcast_dlf_pm_idx, unit, profile_idx, profile.l2_mcast_dlf_pm_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, ip4_mcast_dlf_pm_idx %u)"
                                 , profile.ip4_mcast_dlf_pm_idx, result_profile.ip4_mcast_dlf_pm_idx, unit, profile_idx, profile.ip4_mcast_dlf_pm_idx);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                 , profile.ip6_mcast_dlf_pm_idx, result_profile.ip6_mcast_dlf_pm_idx, unit, profile_idx, profile.ip6_mcast_dlf_pm_idx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profile_set (unit %u, profile_idx %u, learn %u, l2_mcast_dlf_pm_idx %u, ip4_mcast_dlf_pm_idx %u, ip6_mcast_dlf_pm_idx %u)"
                                , ret, RT_ERR_OK, unit, profile_idx, profile.learn, profile.l2_mcast_dlf_pm_idx, profile.ip4_mcast_dlf_pm_idx, profile.ip6_mcast_dlf_pm_idx);
            }
            ret = rtk_vlan_profile_get(unit, profile_idx, &result_profile);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                                , ret, expect_result, unit, profile_idx);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_profile_get (unit %u, profile_idx %u)"
                                    , ret, RT_ERR_OK, unit, profile_idx);
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portIgrFilter_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_ifilter_t  igrFilter_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrFilter_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrFilter_get(unit, 0, &igrFilter_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_ifilter_t  igrFilter;
        rtk_vlan_ifilter_t  igrFilter_max;
        rtk_vlan_ifilter_t  result_igrFilter;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_IGR_FILTER_MAX, igrFilter_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_IGR_FILTER_MIN, igrFilter); igrFilter <= igrFilter_max; igrFilter++)
            {
                ASSIGN_EXPECT_RESULT(igrFilter, TEST_VLAN_IGR_FILTER_MIN, TEST_VLAN_IGR_FILTER_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                if ((igrFilter != INGRESS_FILTER_FWD)
                    && (igrFilter != INGRESS_FILTER_DROP)
                    && (igrFilter != INGRESS_FILTER_TRAP)
                    && (igrFilter != INGRESS_FILTER_TRAP2MASTERCPU))
                        expect_result = RT_ERR_FAILED;
                ret = rtk_vlan_portIgrFilter_set(unit, port[port_index], igrFilter);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_set (unit %u, port %u, igrFilter %u)"
                                    , ret, expect_result, unit, port[port_index], igrFilter);

                    ret = rtk_vlan_portIgrFilter_get(unit, port[port_index], &result_igrFilter);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u, igrFilter %u)"
                                     , ret, expect_result, unit, port[port_index], result_igrFilter);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_set/get value unequal (unit %u, port %u, igrFilter %u)"
                                     , result_igrFilter, igrFilter, unit, port[port_index], igrFilter);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrFilter_set (unit %u, port %u, igrFilter %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], igrFilter);
                }
            }
            ret = rtk_vlan_portIgrFilter_get(unit, port[port_index], &result_igrFilter);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_ifilter_t  igrFilter;
        rtk_vlan_ifilter_t  igrFilter_max;
        rtk_vlan_ifilter_t  result_igrFilter;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_IGR_FILTER_MAX, igrFilter_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_IGR_FILTER_MIN, igrFilter); igrFilter <= igrFilter_max; igrFilter++)
            {
                ASSIGN_EXPECT_RESULT(igrFilter, TEST_VLAN_IGR_FILTER_MIN, TEST_VLAN_IGR_FILTER_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((igrFilter != INGRESS_FILTER_FWD)
                    && (igrFilter != INGRESS_FILTER_DROP)
                    && (igrFilter != INGRESS_FILTER_TRAP)
                    && (igrFilter != INGRESS_FILTER_TRAP2MASTERCPU))
                        expect_result = RT_ERR_FAILED;

                ret = rtk_vlan_portIgrFilter_set(unit, port, igrFilter);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_set (unit %u, port %u, igrFilter %u)"
                                    , ret, expect_result, unit, port, igrFilter);

                    ret = rtk_vlan_portIgrFilter_get(unit, port, &result_igrFilter);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u, igrFilter %u)"
                                     , ret, expect_result, unit, port, result_igrFilter);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_set/get value unequal (unit %u, port %u, igrFilter %u)"
                                     , result_igrFilter, igrFilter, unit, port, igrFilter);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrFilter_set (unit %u, port %u, igrFilter %u)"
                                        , ret, RT_ERR_OK, unit, port, igrFilter);
                }
                ret = rtk_vlan_portIgrFilter_get(unit, port, &result_igrFilter);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrFilter_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portInnerPvidMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_pbVlan_mode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerPvidMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portInnerPvidMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_pbVlan_mode_t  mode;
        rtk_vlan_pbVlan_mode_t  mode_max;
        rtk_vlan_pbVlan_mode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_PBPVID_MODE_MAX, mode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_PBPVID_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_PBPVID_MODE_MIN, TEST_VLAN_PBPVID_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerPvidMode_set(unit, port[port_index], mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port[port_index], mode);

                    ret = rtk_vlan_portInnerPvidMode_get(unit, port[port_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port[port_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_set/get value unequal (unit %u, port %u, mode %u)"
                                     , result_mode, mode, unit, port[port_index], mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvidMode_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], mode);
                }
            }
            ret = rtk_vlan_portInnerPvidMode_get(unit, port[port_index], &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_pbVlan_mode_t  mode;
        rtk_vlan_pbVlan_mode_t  mode_max;
        rtk_vlan_pbVlan_mode_t  result_mode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_PBPVID_MODE_MAX, mode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_PBPVID_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_PBPVID_MODE_MIN, TEST_VLAN_PBPVID_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portInnerPvidMode_set(unit, port, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port, mode);

                    ret = rtk_vlan_portInnerPvidMode_get(unit, port, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_set/get value unequal (unit %u, port %u, mode %u)"
                                     , result_mode, mode, unit, port, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvidMode_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port, mode);
                }
                ret = rtk_vlan_portInnerPvidMode_get(unit, port, &result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portInnerPvidMode_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portOuterPvidMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_pbVlan_mode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterPvidMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portOuterPvidMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_pbVlan_mode_t  mode;
        rtk_vlan_pbVlan_mode_t  mode_max;
        rtk_vlan_pbVlan_mode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_PBPVID_MODE_MAX, mode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_PBPVID_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_PBPVID_MODE_MIN, TEST_VLAN_PBPVID_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterPvidMode_set(unit, port[port_index], mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port[port_index], mode);

                    ret = rtk_vlan_portOuterPvidMode_get(unit, port[port_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port[port_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_set/get value unequal (unit %u, port %u, mode %u)"
                                     , result_mode, mode, unit, port[port_index], mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvidMode_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], mode);
                }
            }
            ret = rtk_vlan_portOuterPvidMode_get(unit, port[port_index], &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_pbVlan_mode_t  mode;
        rtk_vlan_pbVlan_mode_t  mode_max;
        rtk_vlan_pbVlan_mode_t  result_mode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_PBPVID_MODE_MAX, mode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_PBPVID_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_PBPVID_MODE_MIN, TEST_VLAN_PBPVID_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portOuterPvidMode_set(unit, port, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_set (unit %u, port %u, mode %u)"
                                    , ret, expect_result, unit, port, mode);

                    ret = rtk_vlan_portOuterPvidMode_get(unit, port, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u, mode %u)"
                                     , ret, expect_result, unit, port, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_set/get value unequal (unit %u, port %u, mode %u)"
                                     , result_mode, mode, unit, port, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvidMode_set (unit %u, port %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, port, mode);
                }
                ret = rtk_vlan_portOuterPvidMode_get(unit, port, &result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portOuterPvidMode_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_innerTpidEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_innerTpidEntry_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_innerTpidEntry_get(unit, 0, &tpid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;

        uint32  tpid[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_result[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_index;
        int32  tpid_last;
        uint32  result_tpid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TPID_MAX, TEST_TPID_MIN, tpid, tpid_result, tpid_last);

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result2);
            for (tpid_index = 0; tpid_index <= tpid_last; tpid_index++)
            {
                inter_result3 = tpid_result[tpid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_innerTpidEntry_set(unit, tpid_idx, tpid[tpid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid[tpid_index]);

                    ret = rtk_vlan_innerTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid[tpid_index], unit, tpid_idx, result_tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_innerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid[tpid_index]);
                }
            }
        }
        ret = rtk_vlan_innerTpidEntry_get(unit, tpid_idx, &result_tpid);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, expect_result, unit, tpid_idx);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, RT_ERR_OK, unit, tpid_idx);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  tpid;
        uint32  tpid_max;
        uint32  result_tpid;

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);
        G_VALUE(TEST_TPID_MAX, tpid_max);

        for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result2);
            for (L_VALUE(TEST_TPID_MAX, tpid); tpid <= tpid_max; tpid++)
            {
                ASSIGN_EXPECT_RESULT(tpid, TEST_TPID_MIN, TEST_TPID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_innerTpidEntry_set(unit, tpid_idx, tpid);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid);

                    ret = rtk_vlan_innerTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid, unit, tpid_idx, tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_innerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid);
                }
                ret = rtk_vlan_innerTpidEntry_get(unit, tpid_idx, &result_tpid);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u)"
                                    , ret, expect_result, unit, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_innerTpidEntry_get (unit %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, tpid_idx);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_outerTpidEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_outerTpidEntry_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_outerTpidEntry_get(unit, 0, &tpid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;

        uint32  tpid[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_result[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_index;
        int32  tpid_last;
        uint32  result_tpid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TPID_MAX, TEST_TPID_MIN, tpid, tpid_result, tpid_last);

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result2);
            for (tpid_index = 0; tpid_index <= tpid_last; tpid_index++)
            {
                inter_result3 = tpid_result[tpid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_outerTpidEntry_set(unit, tpid_idx, tpid[tpid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid[tpid_index]);

                    ret = rtk_vlan_outerTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid[tpid_index], unit, tpid_idx, result_tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_outerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid[tpid_index]);
                }
            }
        }
        ret = rtk_vlan_outerTpidEntry_get(unit, tpid_idx, &result_tpid);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, expect_result, unit, tpid_idx);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, RT_ERR_OK, unit, tpid_idx);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  tpid;
        uint32  tpid_max;
        uint32  result_tpid;

        G_VALUE(TEST_TPID_IDX_MAX(unit), tpid_idx_max);
        G_VALUE(TEST_TPID_MAX, tpid_max);

        for (L_VALUE(TEST_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_TPID_IDX_MIN, TEST_TPID_IDX_MAX(unit), inter_result2);
            for (L_VALUE(TEST_TPID_MAX, tpid); tpid <= tpid_max; tpid++)
            {
                ASSIGN_EXPECT_RESULT(tpid, TEST_TPID_MIN, TEST_TPID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_outerTpidEntry_set(unit, tpid_idx, tpid);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid);

                    ret = rtk_vlan_outerTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid, unit, tpid_idx, tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_outerTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid);
                }
                ret = rtk_vlan_outerTpidEntry_get(unit, tpid_idx, &result_tpid);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u)"
                                    , ret, expect_result, unit, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_outerTpidEntry_get (unit %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, tpid_idx);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_extraTpidEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  tpid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_extraTpidEntry_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_extraTpidEntry_get(unit, 0, &tpid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;

        uint32  tpid[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_result[UNITTEST_MAX_TEST_VALUE];
        int32  tpid_index;
        int32  tpid_last;
        uint32  result_tpid;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TPID_MAX, TEST_TPID_MIN, tpid, tpid_result, tpid_last);

        G_VALUE(TEST_EXTRA_TPID_IDX_MAX(unit), tpid_idx_max);

        for (L_VALUE(TEST_EXTRA_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_EXTRA_TPID_IDX_MIN, TEST_EXTRA_TPID_IDX_MAX(unit), inter_result2);
            for (tpid_index = 0; tpid_index <= tpid_last; tpid_index++)
            {
                inter_result3 = tpid_result[tpid_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_extraTpidEntry_set(unit, tpid_idx, tpid[tpid_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid[tpid_index]);

                    ret = rtk_vlan_extraTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid[tpid_index], unit, tpid_idx, result_tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_extraTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid[tpid_index]);
                }
            }
        }
        ret = rtk_vlan_extraTpidEntry_get(unit, tpid_idx, &result_tpid);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, expect_result, unit, tpid_idx);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u)"
                            , ret, RT_ERR_OK, unit, tpid_idx);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  tpid_idx;
        uint32  tpid_idx_max;
        uint32  tpid;
        uint32  tpid_max;
        uint32  result_tpid;

        G_VALUE(TEST_EXTRA_TPID_IDX_MAX(unit), tpid_idx_max);
        G_VALUE(TEST_TPID_MAX, tpid_max);

        for (L_VALUE(TEST_EXTRA_TPID_IDX_MIN, tpid_idx); tpid_idx <= tpid_idx_max; tpid_idx++)
        {
            ASSIGN_EXPECT_RESULT(tpid_idx, TEST_EXTRA_TPID_IDX_MIN, TEST_EXTRA_TPID_IDX_MAX(unit), inter_result2);
            for (L_VALUE(TEST_TPID_MAX, tpid); tpid <= tpid_max; tpid++)
            {
                ASSIGN_EXPECT_RESULT(tpid, TEST_TPID_MIN, TEST_TPID_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_extraTpidEntry_set(unit, tpid_idx, tpid);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, expect_result, unit, tpid_idx, tpid);

                    ret = rtk_vlan_extraTpidEntry_get(unit, tpid_idx, &result_tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u, tpid %u)"
                                     , ret, expect_result, unit, tpid_idx, tpid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_set/get value unequal (unit %u, tpid_idx %u, tpid %u)"
                                     , result_tpid, tpid, unit, tpid_idx, tpid);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_extraTpidEntry_set (unit %u, tpid_idx %u, tpid %u)"
                                    , ret, RT_ERR_OK, unit, tpid_idx, tpid);
                }
                ret = rtk_vlan_extraTpidEntry_get(unit, tpid_idx, &result_tpid);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u)"
                                    , ret, expect_result, unit, tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_extraTpidEntry_get (unit %u, tpid_idx %u)"
                                        , ret, RT_ERR_OK, unit, tpid_idx);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrInnerTagSts_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_tagSts_t  tagStatus_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrInnerTagSts_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrInnerTagSts_get(unit, 0, &tagStatus_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_tagSts_t  tagStatus;
        rtk_vlan_tagSts_t  tagStatus_max;
        rtk_vlan_tagSts_t  result_tagStatus;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_TAGSTATUS_MAX(unit), tagStatus_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_TAGSTATUS_MIN, tagStatus); tagStatus <= tagStatus_max; tagStatus++)
            {
                ASSIGN_EXPECT_RESULT(tagStatus, TEST_VLAN_TAGSTATUS_MIN, TEST_VLAN_TAGSTATUS_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrInnerTagSts_set(unit, port[port_index], tagStatus);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set (unit %u, port %u, tagStatus %u)"
                                    , ret, expect_result, unit, port[port_index], tagStatus);

                    ret = rtk_vlan_portEgrInnerTagSts_get(unit, port[port_index], &result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u, tagStatus %u)"
                                     , ret, expect_result, unit, port[port_index], result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set/get value unequal (unit %u, port %u, tagStatus %u)"
                                     , result_tagStatus, tagStatus, unit, port[port_index], tagStatus);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set (unit %u, port %u, tagStatus %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tagStatus);
                }
            }
            ret = rtk_vlan_portEgrInnerTagSts_get(unit, port[port_index], &result_tagStatus);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_tagSts_t  tagStatus;
        rtk_vlan_tagSts_t  tagStatus_max;
        rtk_vlan_tagSts_t  result_tagStatus;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_TAGSTATUS_MAX(unit), tagStatus_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_TAGSTATUS_MIN, tagStatus); tagStatus <= tagStatus_max; tagStatus++)
            {
                ASSIGN_EXPECT_RESULT(tagStatus, TEST_VLAN_TAGSTATUS_MIN, TEST_VLAN_TAGSTATUS_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrInnerTagSts_set(unit, port, tagStatus);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set (unit %u, port %u, tagStatus %u)"
                                    , ret, expect_result, unit, port, tagStatus);

                    ret = rtk_vlan_portEgrInnerTagSts_get(unit, port, &result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u, tagStatus %u)"
                                     , ret, expect_result, unit, port, result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set/get value unequal (unit %u, port %u, tagStatus %u)"
                                     , result_tagStatus, tagStatus, unit, port, tagStatus);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_set (unit %u, port %u, tagStatus %u)"
                                        , ret, RT_ERR_OK, unit, port, tagStatus);
                }
                ret = rtk_vlan_portEgrInnerTagSts_get(unit, port, &result_tagStatus);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrInnerTagSts_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrOuterTagSts_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_tagSts_t  tagStatus_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrOuterTagSts_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrOuterTagSts_get(unit, 0, &tagStatus_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_tagSts_t  tagStatus;
        rtk_vlan_tagSts_t  tagStatus_max;
        rtk_vlan_tagSts_t  result_tagStatus;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_TAGSTATUS_MAX(unit), tagStatus_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_TAGSTATUS_MIN, tagStatus); tagStatus <= tagStatus_max; tagStatus++)
            {
                ASSIGN_EXPECT_RESULT(tagStatus, TEST_VLAN_TAGSTATUS_MIN, TEST_VLAN_TAGSTATUS_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrOuterTagSts_set(unit, port[port_index], tagStatus);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set (unit %u, port %u, tagStatus %u)"
                                    , ret, expect_result, unit, port[port_index], tagStatus);

                    ret = rtk_vlan_portEgrOuterTagSts_get(unit, port[port_index], &result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u, tagStatus %u)"
                                     , ret, expect_result, unit, port[port_index], result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set/get value unequal (unit %u, port %u, tagStatus %u)"
                                     , result_tagStatus, tagStatus, unit, port[port_index], tagStatus);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set (unit %u, port %u, tagStatus %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], tagStatus);
                }
            }
            ret = rtk_vlan_portEgrOuterTagSts_get(unit, port[port_index], &result_tagStatus);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        uint32  tagStatus;
        uint32  tagStatus_max;
        uint32  result_tagStatus;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_TAGSTATUS_MAX(unit), tagStatus_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_TAGSTATUS_MIN, tagStatus); tagStatus <= tagStatus_max; tagStatus++)
            {
                ASSIGN_EXPECT_RESULT(tagStatus, TEST_VLAN_TAGSTATUS_MIN, (uint32)TEST_VLAN_TAGSTATUS_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrOuterTagSts_set(unit, port, (rtk_vlan_tagSts_t)tagStatus);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set (unit %u, port %u, tagStatus %u)"
                                    , ret, expect_result, unit, port, tagStatus);

                    ret = rtk_vlan_portEgrOuterTagSts_get(unit, port, (rtk_vlan_tagSts_t*)&result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u, tagStatus %u)"
                                     , ret, expect_result, unit, port, result_tagStatus);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set/get value unequal (unit %u, port %u, tagStatus %u)"
                                     , result_tagStatus, tagStatus, unit, port, tagStatus);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_set (unit %u, port %u, tagStatus %u)"
                                        , ret, RT_ERR_OK, unit, port, tagStatus);
                }
                ret = rtk_vlan_portEgrOuterTagSts_get(unit, port, &result_tagStatus);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrOuterTagSts_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_igrVlanCnvtBlkMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_igrVlanCnvtBlk_mode_t  mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_igrVlanCnvtBlkMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_igrVlanCnvtBlkMode_get(unit, 0, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  blkIdx[UNITTEST_MAX_TEST_VALUE];
        int32  blkIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  blkIdx_index;
        int32  blkIdx_last;

        rtk_vlan_igrVlanCnvtBlk_mode_t  mode;
        rtk_vlan_igrVlanCnvtBlk_mode_t  mode_max;
        rtk_vlan_igrVlanCnvtBlk_mode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_C2SC_BLK_MAX(unit), TEST_C2SC_BLK_MIN, blkIdx, blkIdx_result, blkIdx_last);

        G_VALUE(TEST_VLAN_CNVTBLK_MODE_MAX, mode_max);

        for (blkIdx_index = 0; blkIdx_index <= blkIdx_last; blkIdx_index++)
        {
            inter_result2 = blkIdx_result[blkIdx_index];
            for (L_VALUE(TEST_VLAN_CNVTBLK_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_CNVTBLK_MODE_MIN, TEST_VLAN_CNVTBLK_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_igrVlanCnvtBlkMode_set(unit, blkIdx[blkIdx_index], mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set (unit %u, blkIdx %u, mode %u)"
                                    , ret, expect_result, unit, blkIdx[blkIdx_index], mode);

                    ret = rtk_vlan_igrVlanCnvtBlkMode_get(unit, blkIdx[blkIdx_index], &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u, mode %u)"
                                     , ret, expect_result, unit, blkIdx[blkIdx_index], result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set/get value unequal (unit %u, blkIdx %u, mode %u)"
                                     , result_mode, mode, unit, blkIdx[blkIdx_index], mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set (unit %u, blkIdx %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, blkIdx[blkIdx_index], mode);
                }
            }
            ret = rtk_vlan_igrVlanCnvtBlkMode_get(unit, blkIdx[blkIdx_index], &result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u)"
                                , ret, expect_result, unit, blkIdx[blkIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u)"
                                    , ret, RT_ERR_OK, unit, blkIdx[blkIdx_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32  blkIdx;
        uint32  blkIdx_max;
        rtk_vlan_igrVlanCnvtBlk_mode_t  mode;
        rtk_vlan_igrVlanCnvtBlk_mode_t  mode_max;
        rtk_vlan_igrVlanCnvtBlk_mode_t  result_mode;

        G_VALUE(TEST_C2SC_BLK_MAX(unit), blkIdx_max);
        G_VALUE(TEST_VLAN_CNVTBLK_MODE_MAX, mode_max);

        for (L_VALUE(TEST_C2SC_BLK_MIN, blkIdx); blkIdx <= blkIdx_max; blkIdx++)
        {
            ASSIGN_EXPECT_RESULT(blkIdx, TEST_C2SC_BLK_MIN, TEST_C2SC_BLK_MAX(unit), inter_result2);

            for (L_VALUE(TEST_VLAN_CNVTBLK_MODE_MIN, mode); mode <= mode_max; mode++)
            {
                ASSIGN_EXPECT_RESULT(mode, TEST_VLAN_CNVTBLK_MODE_MIN, TEST_VLAN_CNVTBLK_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_igrVlanCnvtBlkMode_set(unit, blkIdx, mode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set (unit %u, blkIdx %u, mode %u)"
                                    , ret, expect_result, unit, blkIdx, mode);

                    ret = rtk_vlan_igrVlanCnvtBlkMode_get(unit, blkIdx, &result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u, mode %u)"
                                     , ret, expect_result, unit, blkIdx, result_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set/get value unequal (unit %u, blkIdx %u, mode %u)"
                                     , result_mode, mode, unit, blkIdx, mode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_set (unit %u, blkIdx %u, mode %u)"
                                        , ret, RT_ERR_OK, unit, blkIdx, mode);
                }
                ret = rtk_vlan_igrVlanCnvtBlkMode_get(unit, blkIdx, &result_mode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u)"
                                    , ret, expect_result, unit, blkIdx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtBlkMode_get (unit %u, blkIdx %u)"
                                        , ret, RT_ERR_OK, unit, blkIdx);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_egrVlanCnvtDblTagEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtDblTagEnable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtDblTagEnable_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, 0, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_egrVlanCnvtDblTagEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_vlan_egrVlanCnvtDblTagEnable_get(unit, (uint32*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }

        ret = rtk_vlan_egrVlanCnvtDblTagEnable_get(unit, (uint32*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtDblTagEnable_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_egrVlanCnvtVidSource_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_vlanMode_t  result_vlanMode;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtVidSource_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtVidSource_get(unit, &result_vlanMode)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_vlanMode_t  vlanMode[UNITTEST_MAX_TEST_VALUE];
        int32  vlanMode_result[UNITTEST_MAX_TEST_VALUE];
        int32  vlanMode_index;
        int32  vlanMode_last;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_MODE_MAX, TEST_VLAN_MODE_MIN, vlanMode, vlanMode_result, vlanMode_last);

        for (vlanMode_index = 0; vlanMode_index <= vlanMode_last; vlanMode_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (vlanMode_result[vlanMode_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_egrVlanCnvtVidSource_set(unit, vlanMode[vlanMode_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_set (unit %u, vlanMode %u)"
                                , ret, expect_result, unit, vlanMode[vlanMode_index]);

                ret = rtk_vlan_egrVlanCnvtVidSource_get(unit, (rtk_l2_vlanMode_t *)&result_vlanMode);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_get (unit %u, vlanMode %u)"
                                 , ret, expect_result, unit, vlanMode[vlanMode_index]);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_set/get value unequal (unit %u, vlanMode %u)"
                                 , result_vlanMode, vlanMode[vlanMode_index], unit, vlanMode[vlanMode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_set (unit %u, vlanMode %u)"
                                    , ret, RT_ERR_OK, unit, vlanMode[vlanMode_index]);
            }
        }

        ret = rtk_vlan_egrVlanCnvtVidSource_get(unit, (rtk_l2_vlanMode_t *)&result_vlanMode);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtVidSource_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portVlanAggrEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
     rtk_enable_t  enable_temp;
     int8  inter_result2 = 1;
     int8  inter_result3 = 1;
     int32  expect_result = 1;

     if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_ETHER_PORT_ID_MAX(unit), TEST_ETHER_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrEnable_set(unit, port[port_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable);
                     ret = rtk_vlan_portVlanAggrEnable_get(unit, port[port_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port[port_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable);
                }
            }
            ret = rtk_vlan_portVlanAggrEnable_get(unit, port[port_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }
    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max = 0;
        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        G_VALUE(TEST_ETHER_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (L_VALUE(TEST_ETHER_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_ETHER_PORT_ID_MIN(unit), TEST_ETHER_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrEnable_set(unit, port, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);

                    ret = rtk_vlan_portVlanAggrEnable_get(unit, port, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
                }
                ret = rtk_vlan_portVlanAggrEnable_get(unit, port, &result_enable);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrEnable_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_leakyStpFilter_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_leakyStpFilter_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_leakyStpFilter_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_enable_t  enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;
        int32  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, 0, enable, enable_result, enable_last);

        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (enable_result[enable_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_leakyStpFilter_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_leakyStpFilter_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_vlan_leakyStpFilter_get(unit, (uint32*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_leakyStpFilter_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_leakyStpFilter_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_leakyStpFilter_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
        }

        ret = rtk_vlan_leakyStpFilter_get(unit, (uint32*)&result_enable);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_leakyStpFilter_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_leakyStpFilter_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_except_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_except_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_except_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;
        int32  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_EXCEPT_ACT_MAX, TEST_VLAN_EXCEPT_ACT_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (action_result[action_index])? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_vlan_except_set(unit, action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_except_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_vlan_except_get(unit, (uint32*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_except_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_vlan_except_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_except_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_vlan_except_get(unit, (uint32*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_except_get (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_except_get (unit %u)"
                                , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_portIgrCnvtDfltAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrCnvtDfltAct_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrCnvtDfltAct_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_IGRCNVTDFLTACT_MAX, action_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_IGRCNVTDFLTACT_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_IGRCNVTDFLTACT_MIN, TEST_IGRCNVTDFLTACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrCnvtDfltAct_set(unit, port[port_index], action);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action);

                    ret = rtk_vlan_portIgrCnvtDfltAct_get(unit, port[port_index], &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port[port_index], action);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action);
                }
            }
            ret = rtk_vlan_portIgrCnvtDfltAct_get(unit, port[port_index], &result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_IGRCNVTDFLTACT_MAX, action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_IGRCNVTDFLTACT_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_IGRCNVTDFLTACT_MIN, TEST_IGRCNVTDFLTACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portIgrCnvtDfltAct_set(unit, port, action);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);

                    ret = rtk_vlan_portIgrCnvtDfltAct_get(unit, port, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
                }
                ret = rtk_vlan_portIgrCnvtDfltAct_get(unit, port, &result_action);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrCnvtDfltAct_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portIgrTagKeepType_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_tagKeepType_t  keepTypeOuter_temp, keepTypeInner_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrTagKeepType_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portIgrTagKeepType_get(unit, 0, &keepTypeOuter_temp, &keepTypeInner_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_tagKeepType_t  keepTypeOuter;
        rtk_vlan_tagKeepType_t  keepTypeOuter_max;
        rtk_vlan_tagKeepType_t  result_keepTypeOuter;

        rtk_vlan_tagKeepType_t  keepTypeInner;
        rtk_vlan_tagKeepType_t  keepTypeInner_max;
        rtk_vlan_tagKeepType_t  result_keepTypeInner;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeOuter_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeInner_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeOuter); keepTypeOuter <= keepTypeOuter_max; keepTypeOuter++)
            {
                ASSIGN_EXPECT_RESULT(keepTypeOuter, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result3);
                for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeInner); keepTypeInner <= keepTypeInner_max; keepTypeInner++)
                {
                    ASSIGN_EXPECT_RESULT(keepTypeInner, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portIgrTagKeepType_set(unit, port[port_index], keepTypeOuter, keepTypeInner);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                        , ret, expect_result, unit, port[port_index], keepTypeOuter, keepTypeInner);

                        ret = rtk_vlan_portIgrTagKeepType_get(unit, port[port_index], &result_keepTypeOuter, &result_keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , ret, expect_result, unit, port[port_index], result_keepTypeOuter, keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeOuter %u)"
                                         , result_keepTypeOuter, keepTypeOuter, unit, port[port_index], keepTypeOuter);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeInner %u)"
                                         , result_keepTypeInner, keepTypeInner, unit, port[port_index], keepTypeInner);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], keepTypeOuter, keepTypeInner);
                    }
                }
            }
            ret = rtk_vlan_portIgrTagKeepType_get(unit, port[port_index], &result_keepTypeOuter, &result_keepTypeInner);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_tagKeepType_t  keepTypeOuter;
        rtk_vlan_tagKeepType_t  keepTypeOuter_max;
        rtk_vlan_tagKeepType_t  keepTypeInner;
        rtk_vlan_tagKeepType_t  keepTypeInner_max;
        rtk_vlan_tagKeepType_t  result_keepTypeOuter;
        rtk_vlan_tagKeepType_t  result_keepTypeInner;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeOuter_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeInner_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeOuter); keepTypeOuter <= keepTypeOuter_max; keepTypeOuter++)
            {
                ASSIGN_EXPECT_RESULT(keepTypeOuter, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result3);

                for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeInner); keepTypeInner <= keepTypeInner_max; keepTypeInner++)
                {
                    ASSIGN_EXPECT_RESULT(keepTypeInner, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portIgrTagKeepType_set(unit, port, keepTypeOuter, keepTypeInner);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                        , ret, expect_result, unit, port, keepTypeOuter, keepTypeInner);

                        ret = rtk_vlan_portIgrTagKeepType_get(unit, port, &result_keepTypeOuter, &result_keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , ret, expect_result, unit, port, result_keepTypeOuter, keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , result_keepTypeOuter, keepTypeOuter, unit, port, keepTypeOuter, keepTypeInner);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                            , ret, RT_ERR_OK, unit, port, keepTypeOuter, keepTypeInner);
                    }
                    ret = rtk_vlan_portIgrTagKeepType_get(unit, port, &result_keepTypeOuter, &result_keepTypeInner);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u)"
                                        , ret, expect_result, unit, port);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portIgrTagKeepType_get (unit %u, port %u)"
                                            , ret, RT_ERR_OK, unit, port);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrTagKeepType_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_tagKeepType_t  keepTypeOuter_temp, keepTypeInner_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrTagKeepType_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrTagKeepType_get(unit, 0, &keepTypeOuter_temp, &keepTypeInner_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_tagKeepType_t  keepTypeOuter;
        rtk_vlan_tagKeepType_t  keepTypeOuter_max;
        rtk_vlan_tagKeepType_t  result_keepTypeOuter;

        rtk_vlan_tagKeepType_t  keepTypeInner;
        rtk_vlan_tagKeepType_t  keepTypeInner_max;
        rtk_vlan_tagKeepType_t  result_keepTypeInner;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeOuter_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeInner_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeOuter); keepTypeOuter <= keepTypeOuter_max; keepTypeOuter++)
            {
                ASSIGN_EXPECT_RESULT(keepTypeOuter, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result3);
                for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeInner); keepTypeInner <= keepTypeInner_max; keepTypeInner++)
                {
                    ASSIGN_EXPECT_RESULT(keepTypeInner, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result4);

                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portEgrTagKeepType_set(unit, port[port_index], keepTypeOuter, keepTypeInner);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                        , ret, expect_result, unit, port[port_index], keepTypeOuter, keepTypeInner);

                        ret = rtk_vlan_portEgrTagKeepType_get(unit, port[port_index], &result_keepTypeOuter, &result_keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , ret, expect_result, unit, port[port_index], result_keepTypeOuter, keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeOuter %u)"
                                         , result_keepTypeOuter, keepTypeOuter, unit, port[port_index], keepTypeOuter);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeInner %u)"
                                         , result_keepTypeInner, keepTypeInner, unit, port[port_index], keepTypeInner);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], keepTypeOuter, keepTypeInner);
                    }
                }
            }
            ret = rtk_vlan_portEgrTagKeepType_get(unit, port[port_index], &result_keepTypeOuter, &result_keepTypeInner);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_tagKeepType_t  keepTypeOuter;
        rtk_vlan_tagKeepType_t  keepTypeOuter_max;
        rtk_vlan_tagKeepType_t  keepTypeInner;
        rtk_vlan_tagKeepType_t  keepTypeInner_max;

        rtk_vlan_tagKeepType_t  result_keepTypeOuter;
        rtk_vlan_tagKeepType_t  result_keepTypeInner;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeOuter_max);
        G_VALUE(TEST_TAGKEEPTYPE_MAX, keepTypeInner_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeOuter); keepTypeOuter <= keepTypeOuter_max; keepTypeOuter++)
            {
                ASSIGN_EXPECT_RESULT(keepTypeOuter, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result3);

                for (L_VALUE(TEST_TAGKEEPTYPE_MIN, keepTypeInner); keepTypeInner <= keepTypeInner_max; keepTypeInner++)
                {
                    ASSIGN_EXPECT_RESULT(keepTypeInner, TEST_TAGKEEPTYPE_MIN, TEST_TAGKEEPTYPE_MAX, inter_result4);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_vlan_portEgrTagKeepType_set(unit, port, keepTypeOuter, keepTypeInner);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                        , ret, expect_result, unit, port, keepTypeOuter, keepTypeInner);

                        ret = rtk_vlan_portEgrTagKeepType_get(unit, port, &result_keepTypeOuter, &result_keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , ret, expect_result, unit, port, result_keepTypeOuter, keepTypeInner);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set/get value unequal (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                         , result_keepTypeOuter, keepTypeOuter, unit, port, keepTypeOuter, keepTypeInner);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepType_set (unit %u, port %u, keepTypeOuter %u, keepTypeInner %u)"
                                            , ret, RT_ERR_OK, unit, port, keepTypeOuter, keepTypeInner);
                    }
                    ret = rtk_vlan_portEgrTagKeepType_get(unit, port, &result_keepTypeOuter, &result_keepTypeInner);
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                    if (expect_result == RT_ERR_OK)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u)"
                                        , ret, expect_result, unit, port);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrTagKeepType_get (unit %u, port %u)"
                                            , ret, RT_ERR_OK, unit, port);
                    }
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portVlanAggrVidSource_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlanType_t  vlanMode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrVidSource_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrVidSource_get(unit, 0, &vlanMode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlanType_t  vlanMode;
        rtk_vlanType_t  vlanMode_max;
        rtk_vlanType_t  result_vlanMode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_TYPE_MIN, TEST_VLAN_TYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrVidSource_set(unit, port[port_index], vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port[port_index], vlanMode);

                    ret = rtk_vlan_portVlanAggrVidSource_get(unit, port[port_index], &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port[port_index], result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port[port_index], vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], vlanMode);
                }
            }
            ret = rtk_vlan_portVlanAggrVidSource_get(unit, port[port_index], &result_vlanMode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlanType_t  vlanMode;
        rtk_vlanType_t  vlanMode_max;
        rtk_vlanType_t  result_vlanMode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_TYPE_MIN, TEST_VLAN_TYPE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrVidSource_set(unit, port, vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port, vlanMode);

                    ret = rtk_vlan_portVlanAggrVidSource_get(unit, port, &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port, result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port, vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port, vlanMode);
                }
                ret = rtk_vlan_portVlanAggrVidSource_get(unit, port, &result_vlanMode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrVidSource_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portVlanAggrPriTagVidSource_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_priTagVidSrc_t  priTagVidSrc_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrPriTagVidSource_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portVlanAggrPriTagVidSource_get(unit, 0, &priTagVidSrc_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_priTagVidSrc_t  priTagVidSrc;
        rtk_vlan_priTagVidSrc_t  priTagVidSrc_max;
        rtk_vlan_priTagVidSrc_t  result_priTagVidSrc;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_PRITAGVIDSRC_MAX, priTagVidSrc_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_PRITAGVIDSRC_MIN, priTagVidSrc); priTagVidSrc <= priTagVidSrc_max; priTagVidSrc++)
            {
                ASSIGN_EXPECT_RESULT(priTagVidSrc, TEST_PRITAGVIDSRC_MIN, TEST_PRITAGVIDSRC_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrPriTagVidSource_set(unit, port[port_index], priTagVidSrc);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set (unit %u, port %u, priTagVidSrc %u)"
                                    , ret, expect_result, unit, port[port_index], priTagVidSrc);

                    ret = rtk_vlan_portVlanAggrPriTagVidSource_get(unit, port[port_index], &result_priTagVidSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u, priTagVidSrc %u)"
                                     , ret, expect_result, unit, port[port_index], result_priTagVidSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set/get value unequal (unit %u, port %u, priTagVidSrc %u)"
                                     , result_priTagVidSrc, priTagVidSrc, unit, port[port_index], priTagVidSrc);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set (unit %u, port %u, priTagVidSrc %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], priTagVidSrc);
                }
            }
            ret = rtk_vlan_portVlanAggrPriTagVidSource_get(unit, port[port_index], &result_priTagVidSrc);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_priTagVidSrc_t  priTagVidSrc;
        rtk_vlan_priTagVidSrc_t  priTagVidSrc_max;
        rtk_vlan_priTagVidSrc_t  result_priTagVidSrc;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_PRITAGVIDSRC_MAX, priTagVidSrc_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_PRITAGVIDSRC_MIN, priTagVidSrc); priTagVidSrc <= priTagVidSrc_max; priTagVidSrc++)
            {
                ASSIGN_EXPECT_RESULT(priTagVidSrc, TEST_PRITAGVIDSRC_MIN, TEST_PRITAGVIDSRC_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portVlanAggrPriTagVidSource_set(unit, port, priTagVidSrc);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set (unit %u, port %u, priTagVidSrc %u)"
                                    , ret, expect_result, unit, port, priTagVidSrc);

                    ret = rtk_vlan_portVlanAggrPriTagVidSource_get(unit, port, &result_priTagVidSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u, priTagVidSrc %u)"
                                     , ret, expect_result, unit, port, result_priTagVidSrc);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set/get value unequal (unit %u, port %u, priTagVidSrc %u)"
                                     , result_priTagVidSrc, priTagVidSrc, unit, port, priTagVidSrc);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_set (unit %u, port %u, priTagVidSrc %u)"
                                        , ret, RT_ERR_OK, unit, port, priTagVidSrc);
                }
                ret = rtk_vlan_portVlanAggrPriTagVidSource_get(unit, port, &result_priTagVidSrc);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portVlanAggrPriTagVidSource_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrVlanCnvtVidSource_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_vlanMode_t  vlanMode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtVidSource_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtVidSource_get(unit, 0, &vlanMode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_l2_vlanMode_t  vlanMode;
        rtk_l2_vlanMode_t  vlanMode_max;
        rtk_l2_vlanMode_t  result_vlanMode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_MODE_MIN, TEST_VLAN_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtVidSource_set(unit, port[port_index], vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port[port_index], vlanMode);

                    ret = rtk_vlan_portEgrVlanCnvtVidSource_get(unit, port[port_index], &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port[port_index], result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port[port_index], vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], vlanMode);
                }
            }
            ret = rtk_vlan_portEgrVlanCnvtVidSource_get(unit, port[port_index], &result_vlanMode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_l2_vlanMode_t  vlanMode;
        rtk_l2_vlanMode_t  vlanMode_max;
        rtk_l2_vlanMode_t  result_vlanMode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_MODE_MIN, TEST_VLAN_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtVidSource_set(unit, port, vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port, vlanMode);

                    ret = rtk_vlan_portEgrVlanCnvtVidSource_get(unit, port, &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port, result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port, vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port, vlanMode);
                }
                ret = rtk_vlan_portEgrVlanCnvtVidSource_get(unit, port, &result_vlanMode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidSource_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrVlanCnvtVidTarget_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_vlanMode_t  vlanMode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtVidTarget_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, 0, &vlanMode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_l2_vlanMode_t  vlanMode;
        rtk_l2_vlanMode_t  vlanMode_max;
        rtk_l2_vlanMode_t  result_vlanMode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_MODE_MIN, TEST_VLAN_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtVidTarget_set(unit, port[port_index], vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port[port_index], vlanMode);

                    ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, port[port_index], &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port[port_index], result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port[port_index], vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], vlanMode);
                }
            }
            ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, port[port_index], &result_vlanMode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_l2_vlanMode_t  vlanMode;
        rtk_l2_vlanMode_t  vlanMode_max;
        rtk_l2_vlanMode_t  result_vlanMode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_MODE_MAX, vlanMode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_MODE_MIN, vlanMode); vlanMode <= vlanMode_max; vlanMode++)
            {
                ASSIGN_EXPECT_RESULT(vlanMode, TEST_VLAN_MODE_MIN, TEST_VLAN_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtVidTarget_set(unit, port, vlanMode);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set (unit %u, port %u, vlanMode %u)"
                                    , ret, expect_result, unit, port, vlanMode);

                    ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, port, &result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u, vlanMode %u)"
                                     , ret, expect_result, unit, port, result_vlanMode);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set/get value unequal (unit %u, port %u, vlanMode %u)"
                                     , result_vlanMode, vlanMode, unit, port, vlanMode);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_set (unit %u, port %u, vlanMode %u)"
                                        , ret, RT_ERR_OK, unit, port, vlanMode);
                }
                ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, port, &result_vlanMode);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtVidTarget_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_portEgrVlanCnvtLookupMissAct_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_vlan_lookupMissAct_t  act_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtLookupMissAct_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, 0, &act_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_lookupMissAct_t  act;
        rtk_vlan_lookupMissAct_t  act_max;
        rtk_vlan_lookupMissAct_t  result_act;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), TEST_MIN_UNIT_ID, port[2]);

        G_VALUE(TEST_EGRCNVTLOOKUPMISSACT_MAX, act_max);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (L_VALUE(TEST_EGRCNVTLOOKUPMISSACT_MIN, act); act <= act_max; act++)
            {
                ASSIGN_EXPECT_RESULT(act, TEST_EGRCNVTLOOKUPMISSACT_MIN, TEST_EGRCNVTLOOKUPMISSACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3 )? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_set(unit, port[port_index], act);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set (unit %u, port %u, act %u)"
                                    , ret, expect_result, unit, port[port_index], act);

                    ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port[port_index], &result_act);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u, act %u)"
                                     , ret, expect_result, unit, port[port_index], result_act);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set/get value unequal (unit %u, port %u, act %u)"
                                     , result_act, act, unit, port[port_index], act);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set (unit %u, port %u, act %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], act);
                }
            }
            ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port[port_index], &result_act);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_vlan_lookupMissAct_t  act;
        rtk_vlan_lookupMissAct_t  act_max;
        rtk_vlan_lookupMissAct_t  result_act;


        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_EGRCNVTLOOKUPMISSACT_MAX, act_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_EGRCNVTLOOKUPMISSACT_MIN, act); act <= act_max; act++)
            {
                ASSIGN_EXPECT_RESULT(act, TEST_EGRCNVTLOOKUPMISSACT_MIN, TEST_EGRCNVTLOOKUPMISSACT_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_set(unit, port, act);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set (unit %u, port %u, act %u)"
                                    , ret, expect_result, unit, port, act);

                    ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port, &result_act);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u, act %u)"
                                     , ret, expect_result, unit, port, result_act);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set/get value unequal (unit %u, port %u, act %u)"
                                     , result_act, act, unit, port, act);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_set (unit %u, port %u, act %u)"
                                        , ret, RT_ERR_OK, unit, port, act);
                }
                ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port, &result_act);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u)"
                                    , ret, expect_result, unit, port);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_portEgrVlanCnvtLookupMissAct_get (unit %u, port %u)"
                                        , ret, RT_ERR_OK, unit, port);
                }
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_macBasedVlan_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32 ret1;
    uint32      valid_temp = 0;
    rtk_mac_t   smac_temp;
    rtk_vlan_t  vid_temp = 0;
    rtk_pri_t   pri_temp = 0;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_macBasedVlan_set(unit, 0, valid_temp, &smac_temp, vid_temp, pri_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_macBasedVlan_get(unit, 0, &valid_temp, &smac_temp, &vid_temp, &pri_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last = 0;

        uint32  rand_index;
        uint32  c2sc_entry_num_per_blk = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        uint32  valid, result_valid;
        rtk_mac_t smac, result_smac;
        rtk_vlan_t vid, result_vid;
        rtk_pri_t pri, result_pri;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_C2SC_ENTRY_MAX(unit), TEST_C2SC_ENTRY_MIN, index, index_result, index_last);
        for (index_index = 0; index_index <= index_last; index_index++)
        {
            inter_result2 = index_result[index_index];
            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                valid = ut_rand()%2;
                smac.octet[0] = ut_rand()&0xff;
                smac.octet[1] = ut_rand()&0xff;
                smac.octet[2] = ut_rand()&0xff;
                smac.octet[3] = ut_rand()&0xff;
                smac.octet[4] = ut_rand()&0xff;
                smac.octet[5] = ut_rand()&0xff;
                vid = ut_rand()%4096;
                pri = ut_rand()%8;

                rtk_vlan_igrVlanCnvtBlkMode_set(unit, index[index_index]/c2sc_entry_num_per_blk, CONVERSION_MODE_MAC_BASED);
                ret = rtk_vlan_macBasedVlan_set(unit, index[index_index], valid, &smac, vid, pri);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index[index_index], valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);

                    ret = rtk_vlan_macBasedVlan_get(unit, index[index_index], &result_valid, &result_smac, &result_vid, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index[index_index], valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, valid %u)"
                                    , valid, result_valid, unit, index[index_index], valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[0] %02x)"
                                    , smac.octet[0], result_smac.octet[0], unit, index[index_index], smac.octet[0]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[1] %02x)"
                                    , smac.octet[1], result_smac.octet[1], unit, index[index_index], smac.octet[1]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[2] %02x)"
                                    , smac.octet[2], result_smac.octet[2], unit, index[index_index], smac.octet[2]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[3] %02x)"
                                    , smac.octet[3], result_smac.octet[3], unit, index[index_index], smac.octet[3]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[4] %02x)"
                                    , smac.octet[4], result_smac.octet[4], unit, index[index_index], smac.octet[4]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[5] %02x)"
                                    , smac.octet[5], result_smac.octet[5], unit, index[index_index], smac.octet[5]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, vid %u)"
                                    , vid, result_vid, unit, index[index_index], vid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, pri %u)"
                                    , pri, result_pri, unit, index[index_index], pri);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index[index_index], valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);
                }
            }
            valid = 0;
            osal_memset(&smac, 0, sizeof(rtk_mac_t));
            vid = 0;
            pri = 0;
            ret = rtk_vlan_macBasedVlan_set(unit, index[index_index], valid, &smac, vid, pri);
            ret1 = rtk_vlan_macBasedVlan_get(unit, index[index_index], &result_valid, &result_smac, &result_vid, &result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index[index_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index[index_index]);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index[index_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 index;
        uint32 index_max;
        uint32  rand_index;
        uint32  c2sc_entry_num_per_blk = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        uint32  valid, result_valid;
        rtk_mac_t smac, result_smac;
        rtk_vlan_t vid, result_vid;
        rtk_pri_t pri, result_pri;

        G_VALUE(TEST_C2SC_ENTRY_MAX(unit), index_max);

        for (L_VALUE(TEST_C2SC_ENTRY_MIN, index); index <= index_max; index++)
        {
            ASSIGN_EXPECT_RESULT(index, TEST_C2SC_ENTRY_MIN, TEST_C2SC_ENTRY_MAX(unit), inter_result2);

            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                valid = ut_rand()%2;
                smac.octet[0] = ut_rand()&0xff;
                smac.octet[1] = ut_rand()&0xff;
                smac.octet[2] = ut_rand()&0xff;
                smac.octet[3] = ut_rand()&0xff;
                smac.octet[4] = ut_rand()&0xff;
                smac.octet[5] = ut_rand()&0xff;
                vid = ut_rand()%4096;
                pri = ut_rand()%8;

                rtk_vlan_igrVlanCnvtBlkMode_set(unit, index/c2sc_entry_num_per_blk, CONVERSION_MODE_MAC_BASED);
                ret = rtk_vlan_macBasedVlan_set(unit, index, valid, &smac, vid, pri);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index, valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);

                    ret = rtk_vlan_macBasedVlan_get(unit, index, &result_valid, &result_smac, &result_vid, &result_pri);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index, valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, valid %u)"
                                    , valid, result_valid, unit, index, valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[0] %02x)"
                                    , smac.octet[0], result_smac.octet[0], unit, index, smac.octet[0]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[1] %02x)"
                                    , smac.octet[1], result_smac.octet[1], unit, index, smac.octet[1]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[2] %02x)"
                                    , smac.octet[2], result_smac.octet[2], unit, index, smac.octet[2]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[3] %02x)"
                                    , smac.octet[3], result_smac.octet[3], unit, index, smac.octet[3]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[4] %02x)"
                                    , smac.octet[4], result_smac.octet[4], unit, index, smac.octet[4]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, smac[5] %02x)"
                                    , smac.octet[5], result_smac.octet[5], unit, index, smac.octet[5]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, vid %u)"
                                    , vid, result_vid, unit, index, vid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u, pri %u)"
                                    , pri, result_pri, unit, index, pri);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u, valid %u, smac %02x-%02x-%02x-%02x-%02x-%02x, vid %u, pri %u)"
                                    , ret, expect_result, unit, index, valid, smac.octet[0], smac.octet[1]
                                    , smac.octet[2], smac.octet[3], smac.octet[4], smac.octet[5], vid, pri);
                }
            }
            valid = 0;
            osal_memset(&smac, 0, sizeof(rtk_mac_t));
            vid = 0;
            pri = 0;
            ret = rtk_vlan_macBasedVlan_set(unit, index, valid, &smac, vid, pri);
            ret1 = rtk_vlan_macBasedVlan_get(unit, index, &result_valid, &result_smac, &result_vid, &result_pri);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_macBasedVlan_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_igrVlanCnvtEntry_test(uint32 caseNo, uint32 unit)
{

    rtk_vlan_igrVlanCnvtEntry_t igrVlanCnvtEntry_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    int32 ret, ret1;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_igrVlanCnvtEntry_set(unit, 0, &igrVlanCnvtEntry_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_igrVlanCnvtEntry_get(unit, 0, &igrVlanCnvtEntry_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    if (!IS_TEST_SCAN_MODE())
    {
        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last = 0;

        uint32  rand_index;
        uint32  c2sc_entry_num_per_blk = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        rtk_vlan_igrVlanCnvtEntry_t igrVlanCnvtEntry;
        rtk_vlan_igrVlanCnvtEntry_t result_igrVlanCnvtEntry;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_C2SC_ENTRY_MAX(unit), TEST_C2SC_ENTRY_MIN, index, index_result, index_last);

        for (index_index = 0; index_index <= index_last; index_index++)
        {
            inter_result2 = index_result[index_index];
            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                igrVlanCnvtEntry.valid = ut_rand()%2;
#if defined (CONFIG_SDK_RTL8390) || defined (CONFIG_SDK_RTL9310) || defined (CONFIG_SDK_RTL9300)
                if(!HWP_8380_FAMILY_ID(unit))
                {
                    igrVlanCnvtEntry.vid_ignore = ut_rand()%2;
                    igrVlanCnvtEntry.vid = ut_rand()%4096;
                    igrVlanCnvtEntry.rngchk_result = ut_rand();
                    igrVlanCnvtEntry.rngchk_result_mask = ut_rand();
                    igrVlanCnvtEntry.priority_ignore = ut_rand()%2;
                    igrVlanCnvtEntry.priority = ut_rand()%8;
                    igrVlanCnvtEntry.port_ignore = ut_rand()%2;
                    igrVlanCnvtEntry.port = ut_rand()%TEST_ETHER_PORT_ID_MAX(unit);
                    igrVlanCnvtEntry.vid_cnvt_sel = ut_rand()%2;
                    igrVlanCnvtEntry.vid_shift = ut_rand()%2;
                }
#endif
                igrVlanCnvtEntry.vid_shift_sel = ut_rand()%2;
                igrVlanCnvtEntry.vid_new = ut_rand()%4096;
                igrVlanCnvtEntry.pri_assign = ut_rand()%2;
                igrVlanCnvtEntry.pri_new = ut_rand()%8;
                igrVlanCnvtEntry.inner_tag_sts = ut_rand()%2;
                igrVlanCnvtEntry.outer_tag_sts = ut_rand()%2;
                igrVlanCnvtEntry.tpid_assign = ut_rand()%2;
                igrVlanCnvtEntry.tpid_idx = ut_rand()%4;

                if (igrVlanCnvtEntry.port_ignore != 1 && !HWP_ETHER_PORT(unit, igrVlanCnvtEntry.port))
                    expect_result = RT_ERR_FAILED;

                rtk_vlan_igrVlanCnvtBlkMode_set(unit, index[index_index]/c2sc_entry_num_per_blk, CONVERSION_MODE_C2SC);
                ret = rtk_vlan_igrVlanCnvtEntry_set(unit, index[index_index], &igrVlanCnvtEntry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);

                    ret = rtk_vlan_igrVlanCnvtEntry_get(unit, index[index_index], &result_igrVlanCnvtEntry);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, valid %u)"
                                    , igrVlanCnvtEntry.valid, result_igrVlanCnvtEntry.valid, unit, index[index_index], igrVlanCnvtEntry.valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_ignore %u)"
                                    , igrVlanCnvtEntry.vid_ignore, result_igrVlanCnvtEntry.vid_ignore, unit, index[index_index], igrVlanCnvtEntry.vid_ignore);
                    if (igrVlanCnvtEntry.vid_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid %u)"
                                    , igrVlanCnvtEntry.vid, result_igrVlanCnvtEntry.vid, unit, index[index_index], igrVlanCnvtEntry.vid);
#if defined(CONFIG_VIRTUAL_ARRAY_ONLY)
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , igrVlanCnvtEntry.rngchk_result, result_igrVlanCnvtEntry.rngchk_result, unit, index[index_index], igrVlanCnvtEntry.rngchk_result);
#else
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , (igrVlanCnvtEntry.rngchk_result&igrVlanCnvtEntry.rngchk_result_mask), result_igrVlanCnvtEntry.rngchk_result, unit, index[index_index], igrVlanCnvtEntry.rngchk_result);
#endif
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result_mask %u)"
                                    , igrVlanCnvtEntry.rngchk_result_mask, result_igrVlanCnvtEntry.rngchk_result_mask, unit, index[index_index], igrVlanCnvtEntry.rngchk_result_mask);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, priority_ignore %u)"
                                    , igrVlanCnvtEntry.priority_ignore, result_igrVlanCnvtEntry.priority_ignore, unit, index[index_index], igrVlanCnvtEntry.priority_ignore);
                    if (igrVlanCnvtEntry.priority_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, priority %u)"
                                    , igrVlanCnvtEntry.priority, result_igrVlanCnvtEntry.priority, unit, index[index_index], igrVlanCnvtEntry.priority);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, port_ignore %u)"
                                    , igrVlanCnvtEntry.port_ignore, result_igrVlanCnvtEntry.port_ignore, unit, index[index_index], igrVlanCnvtEntry.port_ignore);
                    if (igrVlanCnvtEntry.port_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, port %u)"
                                    , igrVlanCnvtEntry.port, result_igrVlanCnvtEntry.port, unit, index[index_index], igrVlanCnvtEntry.port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_cnvt_sel %u)"
                                    , igrVlanCnvtEntry.vid_cnvt_sel, result_igrVlanCnvtEntry.vid_cnvt_sel, unit, index[index_index], igrVlanCnvtEntry.vid_cnvt_sel);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_shift %u)"
                                    , igrVlanCnvtEntry.vid_shift, result_igrVlanCnvtEntry.vid_shift, unit, index[index_index], igrVlanCnvtEntry.vid_shift);
//                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_shift_sel %u)"
//                                        , igrVlanCnvtEntry.vid_shift_sel, result_igrVlanCnvtEntry.vid_shift_sel, unit, index[index_index], igrVlanCnvtEntry.vid_shift_sel);
                    if (igrVlanCnvtEntry.vid_shift_sel == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , igrVlanCnvtEntry.vid_new, result_igrVlanCnvtEntry.vid_new, unit, index[index_index], igrVlanCnvtEntry.vid_new);
                    else
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , igrVlanCnvtEntry.vid_new, (4096 - result_igrVlanCnvtEntry.vid_new)%4096, unit, index[index_index], igrVlanCnvtEntry.vid_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, pri_assign %u)"
                                    , igrVlanCnvtEntry.pri_assign, result_igrVlanCnvtEntry.pri_assign, unit, index[index_index], igrVlanCnvtEntry.pri_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, pri_new %u)"
                                    , igrVlanCnvtEntry.pri_new, result_igrVlanCnvtEntry.pri_new, unit, index[index_index], igrVlanCnvtEntry.pri_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, inner_tag_sts %u)"
                                    , igrVlanCnvtEntry.inner_tag_sts, result_igrVlanCnvtEntry.inner_tag_sts, unit, index[index_index], igrVlanCnvtEntry.inner_tag_sts);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, outer_tag_sts %u)"
                                    , igrVlanCnvtEntry.outer_tag_sts, result_igrVlanCnvtEntry.outer_tag_sts, unit, index[index_index], igrVlanCnvtEntry.outer_tag_sts);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, tpid_assign %u)"
                                    , igrVlanCnvtEntry.tpid_assign, result_igrVlanCnvtEntry.tpid_assign, unit, index[index_index], igrVlanCnvtEntry.tpid_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, tpid_idx %u)"
                                    , igrVlanCnvtEntry.tpid_idx, result_igrVlanCnvtEntry.tpid_idx, unit, index[index_index], igrVlanCnvtEntry.tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                }
            }
            osal_memset(&igrVlanCnvtEntry, 0, sizeof(rtk_vlan_igrVlanCnvtEntry_t));
            ret = rtk_vlan_igrVlanCnvtEntry_set(unit, index[index_index], &igrVlanCnvtEntry);
            ret1 = rtk_vlan_igrVlanCnvtEntry_get(unit, index[index_index], &result_igrVlanCnvtEntry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index[index_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index[index_index]);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index[index_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 index;
        uint32 index_max;
        uint32  rand_index;
        uint32  c2sc_entry_num_per_blk = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        rtk_vlan_igrVlanCnvtEntry_t igrVlanCnvtEntry;
        rtk_vlan_igrVlanCnvtEntry_t result_igrVlanCnvtEntry;

        G_VALUE(TEST_C2SC_ENTRY_MAX(unit), index_max);

        for (L_VALUE(TEST_C2SC_ENTRY_MIN, index); index <= index_max; index++)
        {
            ASSIGN_EXPECT_RESULT(index, TEST_C2SC_ENTRY_MIN, TEST_C2SC_ENTRY_MAX(unit), inter_result2);

            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                igrVlanCnvtEntry.valid = ut_rand()%2;
                igrVlanCnvtEntry.vid_ignore = ut_rand()%2;
                igrVlanCnvtEntry.vid = ut_rand()%4096;
                igrVlanCnvtEntry.rngchk_result = ut_rand();
                igrVlanCnvtEntry.rngchk_result_mask = ut_rand();
                igrVlanCnvtEntry.priority_ignore = ut_rand()%2;
                igrVlanCnvtEntry.priority = ut_rand()%8;
                igrVlanCnvtEntry.port_ignore = ut_rand()%2;
                igrVlanCnvtEntry.port = ut_rand()%TEST_ETHER_PORT_ID_MAX(unit);
                igrVlanCnvtEntry.vid_cnvt_sel = ut_rand()%2;
                igrVlanCnvtEntry.vid_shift = ut_rand()%2;
                igrVlanCnvtEntry.vid_shift_sel = ut_rand()%2;
                igrVlanCnvtEntry.vid_new = ut_rand()%4096;
                igrVlanCnvtEntry.pri_assign = ut_rand()%2;
                igrVlanCnvtEntry.pri_new = ut_rand()%8;
                igrVlanCnvtEntry.inner_tag_sts = ut_rand()%2;
                igrVlanCnvtEntry.outer_tag_sts = ut_rand()%2;
                igrVlanCnvtEntry.tpid_assign = ut_rand()%2;
                igrVlanCnvtEntry.tpid_idx = ut_rand()%4;

                if (igrVlanCnvtEntry.port_ignore != 1 && !HWP_ETHER_PORT(TEST_MIN_UNIT_ID, igrVlanCnvtEntry.port))
                    expect_result = RT_ERR_FAILED;

                rtk_vlan_igrVlanCnvtBlkMode_set(unit, index/c2sc_entry_num_per_blk, CONVERSION_MODE_C2SC);
                ret = rtk_vlan_igrVlanCnvtEntry_set(unit, index, &igrVlanCnvtEntry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);

                    ret = rtk_vlan_igrVlanCnvtEntry_get(unit, index, &result_igrVlanCnvtEntry);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, valid %u)"
                                    , igrVlanCnvtEntry.valid, result_igrVlanCnvtEntry.valid, unit, index, igrVlanCnvtEntry.valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_ignore %u)"
                                    , igrVlanCnvtEntry.vid_ignore, result_igrVlanCnvtEntry.vid_ignore, unit, index, igrVlanCnvtEntry.vid_ignore);
                    if (igrVlanCnvtEntry.vid_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid %u)"
                                    , igrVlanCnvtEntry.vid, result_igrVlanCnvtEntry.vid, unit, index, igrVlanCnvtEntry.vid);
#if defined(CONFIG_VIRTUAL_ARRAY_ONLY)
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , igrVlanCnvtEntry.rngchk_result, result_igrVlanCnvtEntry.rngchk_result, unit, index, igrVlanCnvtEntry.rngchk_result);
#else
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , (igrVlanCnvtEntry.rngchk_result&igrVlanCnvtEntry.rngchk_result_mask), result_igrVlanCnvtEntry.rngchk_result, unit, index, igrVlanCnvtEntry.rngchk_result);
#endif
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, rngchk_result_mask %u)"
                                    , igrVlanCnvtEntry.rngchk_result_mask, result_igrVlanCnvtEntry.rngchk_result_mask, unit, index, igrVlanCnvtEntry.rngchk_result_mask);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, priority_ignore %u)"
                                    , igrVlanCnvtEntry.priority_ignore, result_igrVlanCnvtEntry.priority_ignore, unit, index, igrVlanCnvtEntry.priority_ignore);
                    if (igrVlanCnvtEntry.priority_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, priority %u)"
                                    , igrVlanCnvtEntry.priority, result_igrVlanCnvtEntry.priority, unit, index, igrVlanCnvtEntry.priority);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, port_ignore %u)"
                                    , igrVlanCnvtEntry.port_ignore, result_igrVlanCnvtEntry.port_ignore, unit, index, igrVlanCnvtEntry.port_ignore);
                    if (igrVlanCnvtEntry.port_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, port %u)"
                                    , igrVlanCnvtEntry.port, result_igrVlanCnvtEntry.port, unit, index, igrVlanCnvtEntry.port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_cnvt_sel %u)"
                                    , igrVlanCnvtEntry.vid_cnvt_sel, result_igrVlanCnvtEntry.vid_cnvt_sel, unit, index, igrVlanCnvtEntry.vid_cnvt_sel);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_shift %u)"
                                    , igrVlanCnvtEntry.vid_shift, result_igrVlanCnvtEntry.vid_shift, unit, index, igrVlanCnvtEntry.vid_shift);
//                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_shift_sel %u)"
//                                        , igrVlanCnvtEntry.vid_shift_sel, result_igrVlanCnvtEntry.vid_shift_sel, unit, index, igrVlanCnvtEntry.vid_shift_sel);
                    if (igrVlanCnvtEntry.vid_shift_sel == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , igrVlanCnvtEntry.vid_new, result_igrVlanCnvtEntry.vid_new, unit, index, igrVlanCnvtEntry.vid_new);
                    else
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , igrVlanCnvtEntry.vid_new, (4096 - result_igrVlanCnvtEntry.vid_new)%4096, unit, index, igrVlanCnvtEntry.vid_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, pri_assign %u)"
                                    , igrVlanCnvtEntry.pri_assign, result_igrVlanCnvtEntry.pri_assign, unit, index, igrVlanCnvtEntry.pri_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, pri_new %u)"
                                    , igrVlanCnvtEntry.pri_new, result_igrVlanCnvtEntry.pri_new, unit, index, igrVlanCnvtEntry.pri_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, inner_tag_sts %u)"
                                    , igrVlanCnvtEntry.inner_tag_sts, result_igrVlanCnvtEntry.inner_tag_sts, unit, index, igrVlanCnvtEntry.inner_tag_sts);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, outer_tag_sts %u)"
                                    , igrVlanCnvtEntry.outer_tag_sts, result_igrVlanCnvtEntry.outer_tag_sts, unit, index, igrVlanCnvtEntry.outer_tag_sts);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, tpid_assign %u)"
                                    , igrVlanCnvtEntry.tpid_assign, result_igrVlanCnvtEntry.tpid_assign, unit, index, igrVlanCnvtEntry.tpid_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u, tpid_idx %u)"
                                    , igrVlanCnvtEntry.tpid_idx, result_igrVlanCnvtEntry.tpid_idx, unit, index, igrVlanCnvtEntry.tpid_idx);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                }
            }
            osal_memset(&igrVlanCnvtEntry, 0, sizeof(rtk_vlan_igrVlanCnvtEntry_t));
            ret = rtk_vlan_igrVlanCnvtEntry_set(unit, index, &igrVlanCnvtEntry);
            ret1 = rtk_vlan_igrVlanCnvtEntry_get(unit, index, &result_igrVlanCnvtEntry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index);
            }
        }
    }
    return RT_ERR_OK;
}

int32 dal_vlan_egrVlanCnvtEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret, ret1;
    rtk_vlan_egrVlanCnvtEntry_t egrVlanCnvtEntry_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtEntry_set(unit, 0, &egrVlanCnvtEntry_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtEntry_get(unit, 0, &egrVlanCnvtEntry_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last = 0;

        uint32  rand_index;
        rtk_vlan_egrVlanCnvtEntry_t egrVlanCnvtEntry;
        rtk_vlan_egrVlanCnvtEntry_t result_egrVlanCnvtEntry;
        UNITTEST_TEST_VALUE_ASSIGN(TEST_SC2C_ENTRY_MAX(unit), TEST_SC2C_ENTRY_MIN, index, index_result, index_last);

        for (index_index = 0; index_index <= index_last; index_index++)
        {
            inter_result2 = index_result[index_index];
            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                egrVlanCnvtEntry.valid = ut_rand()%2;
                egrVlanCnvtEntry.vid_ignore = ut_rand()%2;
                egrVlanCnvtEntry.vid = ut_rand()%4096;
                egrVlanCnvtEntry.orgpri_ignore = ut_rand()%2;
                egrVlanCnvtEntry.orgpri = ut_rand()%8;
                egrVlanCnvtEntry.rngchk_result = ut_rand();
                egrVlanCnvtEntry.rngchk_result_mask = ut_rand();
                egrVlanCnvtEntry.port_ignore = ut_rand()%2;
                egrVlanCnvtEntry.port = ut_rand()%TEST_ETHER_PORT_ID_MAX(unit);
                egrVlanCnvtEntry.vid_shift = ut_rand()%2;
                egrVlanCnvtEntry.vid_new = ut_rand()%4096;
                egrVlanCnvtEntry.pri_assign = ut_rand()%2;
                egrVlanCnvtEntry.pri_new = ut_rand()%8;
                if(HWP_8390_50_FAMILY(unit))
                {
                    egrVlanCnvtEntry.itpid_assign = ut_rand()%2;
                    egrVlanCnvtEntry.itpid_idx = ut_rand()%4;
                }

                if (egrVlanCnvtEntry.port_ignore != 1 && !HWP_ETHER_PORT(TEST_MIN_UNIT_ID, egrVlanCnvtEntry.port))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_vlan_egrVlanCnvtEntry_set(unit, index[index_index], &egrVlanCnvtEntry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);

                    ret = rtk_vlan_egrVlanCnvtEntry_get(unit, index[index_index], &result_egrVlanCnvtEntry);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, valid %u)"
                                    , egrVlanCnvtEntry.valid, result_egrVlanCnvtEntry.valid, unit, index[index_index], egrVlanCnvtEntry.valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_ignore %u)"
                                    , egrVlanCnvtEntry.vid_ignore, result_egrVlanCnvtEntry.vid_ignore, unit, index[index_index], egrVlanCnvtEntry.vid_ignore);
                    if (egrVlanCnvtEntry.vid_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid %u)"
                                    , egrVlanCnvtEntry.vid, result_egrVlanCnvtEntry.vid, unit, index[index_index], egrVlanCnvtEntry.vid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, orgpri_ignore %u)"
                                    , egrVlanCnvtEntry.orgpri_ignore, result_egrVlanCnvtEntry.orgpri_ignore, unit, index[index_index], egrVlanCnvtEntry.orgpri_ignore);
                    if (egrVlanCnvtEntry.orgpri_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, orgpri %u)"
                                    , egrVlanCnvtEntry.orgpri, result_egrVlanCnvtEntry.orgpri, unit, index[index_index], egrVlanCnvtEntry.orgpri);
#if defined(CONFIG_VIRTUAL_ARRAY_ONLY)
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , egrVlanCnvtEntry.rngchk_result, result_egrVlanCnvtEntry.rngchk_result, unit, index[index_index], egrVlanCnvtEntry.rngchk_result);
#else
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , (egrVlanCnvtEntry.rngchk_result&egrVlanCnvtEntry.rngchk_result_mask), result_egrVlanCnvtEntry.rngchk_result, unit, index[index_index], egrVlanCnvtEntry.rngchk_result);
#endif
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result_mask %u)"
                                    , egrVlanCnvtEntry.rngchk_result_mask, result_egrVlanCnvtEntry.rngchk_result_mask, unit, index[index_index], egrVlanCnvtEntry.rngchk_result_mask);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, port_ignore %u)"
                                    , egrVlanCnvtEntry.port_ignore, result_egrVlanCnvtEntry.port_ignore, unit, index[index_index], egrVlanCnvtEntry.port_ignore);
                    if (egrVlanCnvtEntry.port_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, port %u)"
                                    , egrVlanCnvtEntry.port, result_egrVlanCnvtEntry.port, unit, index[index_index], egrVlanCnvtEntry.port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_shift %u)"
                                    , egrVlanCnvtEntry.vid_shift, result_egrVlanCnvtEntry.vid_shift, unit, index[index_index], egrVlanCnvtEntry.vid_shift);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , egrVlanCnvtEntry.vid_new, result_egrVlanCnvtEntry.vid_new, unit, index[index_index], egrVlanCnvtEntry.vid_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, pri_assign %u)"
                                    , egrVlanCnvtEntry.pri_assign, result_egrVlanCnvtEntry.pri_assign, unit, index[index_index], egrVlanCnvtEntry.pri_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, pri_new %u)"
                                    , egrVlanCnvtEntry.pri_new, result_egrVlanCnvtEntry.pri_new, unit, index[index_index], egrVlanCnvtEntry.pri_new);
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, itpid_assign %u)"
                                        , egrVlanCnvtEntry.itpid_assign, result_egrVlanCnvtEntry.itpid_assign, unit, index[index_index], egrVlanCnvtEntry.itpid_assign);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, itpid_idx %u)"
                                        , egrVlanCnvtEntry.itpid_idx, result_egrVlanCnvtEntry.itpid_idx, unit, index[index_index], egrVlanCnvtEntry.itpid_idx);
                    }

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                }
            }
            osal_memset(&egrVlanCnvtEntry, 0, sizeof(rtk_vlan_egrVlanCnvtEntry_t));
            ret = rtk_vlan_egrVlanCnvtEntry_set(unit, index[index_index], &egrVlanCnvtEntry);
            ret1 = rtk_vlan_egrVlanCnvtEntry_get(unit, index[index_index], &result_egrVlanCnvtEntry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index[index_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index[index_index]);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index[index_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 index;
        uint32 index_max;
        uint32  rand_index;
        rtk_vlan_egrVlanCnvtEntry_t egrVlanCnvtEntry;
        rtk_vlan_egrVlanCnvtEntry_t result_egrVlanCnvtEntry;

        G_VALUE(TEST_SC2C_ENTRY_MAX(unit), index_max);

        for (L_VALUE(TEST_SC2C_ENTRY_MIN, index); index <= index_max; index++)
        {
            ASSIGN_EXPECT_RESULT(index, TEST_SC2C_ENTRY_MIN, TEST_SC2C_ENTRY_MAX(unit), inter_result2);

            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                egrVlanCnvtEntry.valid = ut_rand()%2;
                egrVlanCnvtEntry.vid_ignore = ut_rand()%2;
                egrVlanCnvtEntry.vid = ut_rand()%4096;
                egrVlanCnvtEntry.orgpri_ignore = ut_rand()%2;
                egrVlanCnvtEntry.orgpri = ut_rand()%8;
                egrVlanCnvtEntry.rngchk_result = ut_rand();
                egrVlanCnvtEntry.rngchk_result_mask = ut_rand();
                egrVlanCnvtEntry.port_ignore = ut_rand()%2;
                egrVlanCnvtEntry.port = ut_rand()%TEST_ETHER_PORT_ID_MAX(unit);
                egrVlanCnvtEntry.vid_shift = ut_rand()%2;
                egrVlanCnvtEntry.vid_new = ut_rand()%4096;
                egrVlanCnvtEntry.pri_assign = ut_rand()%2;
                egrVlanCnvtEntry.pri_new = ut_rand()%8;
                if(HWP_8390_50_FAMILY(unit))
                {
                    egrVlanCnvtEntry.itpid_assign = ut_rand()%2;
                    egrVlanCnvtEntry.itpid_idx = ut_rand()%4;
                }

                if (egrVlanCnvtEntry.port_ignore != 1 && !HWP_ETHER_PORT(TEST_MIN_UNIT_ID, egrVlanCnvtEntry.port))
                    expect_result = RT_ERR_FAILED;

                ret = rtk_vlan_egrVlanCnvtEntry_set(unit, index, &egrVlanCnvtEntry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);

                    ret = rtk_vlan_egrVlanCnvtEntry_get(unit, index, &result_egrVlanCnvtEntry);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, valid %u)"
                                    , egrVlanCnvtEntry.valid, result_egrVlanCnvtEntry.valid, unit, index, egrVlanCnvtEntry.valid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_ignore %u)"
                                    , egrVlanCnvtEntry.vid_ignore, result_egrVlanCnvtEntry.vid_ignore, unit, index, egrVlanCnvtEntry.vid_ignore);
                    if (egrVlanCnvtEntry.vid_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid %u)"
                                    , egrVlanCnvtEntry.vid, result_egrVlanCnvtEntry.vid, unit, index, egrVlanCnvtEntry.vid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, orgpri_ignore %u)"
                                    , egrVlanCnvtEntry.orgpri_ignore, result_egrVlanCnvtEntry.orgpri_ignore, unit, index, egrVlanCnvtEntry.orgpri_ignore);
                    if (egrVlanCnvtEntry.orgpri_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, orgpri %u)"
                                    , egrVlanCnvtEntry.orgpri, result_egrVlanCnvtEntry.orgpri, unit, index, egrVlanCnvtEntry.orgpri);
#if defined(CONFIG_VIRTUAL_ARRAY_ONLY)
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , egrVlanCnvtEntry.rngchk_result, result_egrVlanCnvtEntry.rngchk_result, unit, index, egrVlanCnvtEntry.rngchk_result);
#else
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result %u)"
                                    , (egrVlanCnvtEntry.rngchk_result&egrVlanCnvtEntry.rngchk_result_mask), result_egrVlanCnvtEntry.rngchk_result, unit, index, egrVlanCnvtEntry.rngchk_result);
#endif
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, rngchk_result_mask %u)"
                                    , egrVlanCnvtEntry.rngchk_result_mask, result_egrVlanCnvtEntry.rngchk_result_mask, unit, index, egrVlanCnvtEntry.rngchk_result_mask);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, port_ignore %u)"
                                    , egrVlanCnvtEntry.port_ignore, result_egrVlanCnvtEntry.port_ignore, unit, index, egrVlanCnvtEntry.port_ignore);
                    if (egrVlanCnvtEntry.port_ignore == 0)
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, port %u)"
                                    , egrVlanCnvtEntry.port, result_egrVlanCnvtEntry.port, unit, index, egrVlanCnvtEntry.port);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_shift %u)"
                                    , egrVlanCnvtEntry.vid_shift, result_egrVlanCnvtEntry.vid_shift, unit, index, egrVlanCnvtEntry.vid_shift);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, vid_new %u)"
                                    , egrVlanCnvtEntry.vid_new, result_egrVlanCnvtEntry.vid_new, unit, index, egrVlanCnvtEntry.vid_new);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, pri_assign %u)"
                                    , egrVlanCnvtEntry.pri_assign, result_egrVlanCnvtEntry.pri_assign, unit, index, egrVlanCnvtEntry.pri_assign);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, pri_new %u)"
                                    , egrVlanCnvtEntry.pri_new, result_egrVlanCnvtEntry.pri_new, unit, index, egrVlanCnvtEntry.pri_new);
                    if(HWP_8390_50_FAMILY(unit))
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, itpid_assign %u)"
                                        , egrVlanCnvtEntry.itpid_assign, result_egrVlanCnvtEntry.itpid_assign, unit, index, egrVlanCnvtEntry.itpid_assign);
                        RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u, itpid_idx %u)"
                                        , egrVlanCnvtEntry.itpid_idx, result_egrVlanCnvtEntry.itpid_idx, unit, index, egrVlanCnvtEntry.itpid_idx);
                    }

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                }
            }
            osal_memset(&egrVlanCnvtEntry, 0, sizeof(rtk_vlan_egrVlanCnvtEntry_t));
            ret = rtk_vlan_egrVlanCnvtEntry_set(unit, index, &egrVlanCnvtEntry);
            ret1 = rtk_vlan_egrVlanCnvtEntry_get(unit, index, &result_egrVlanCnvtEntry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_igrVlanCnvtEntry_delAll_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32  expect_result = 1;

    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_igrVlanCnvtEntry_delAll(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        ret = rtk_vlan_igrVlanCnvtEntry_delAll(unit);
        if (RT_ERR_OK == expect_result)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_delAll (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_igrVlanCnvtEntry_delAll (unit %u)"
                            , ret, expect_result, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_egrVlanCnvtEntry_delAll_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32  expect_result = 1;

    if (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtEntry_delAll(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        ret = rtk_vlan_egrVlanCnvtEntry_delAll(unit);
        if (RT_ERR_OK == expect_result)
        {
            RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_delAll (unit %u)"
                            , ret, expect_result, unit);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtEntry_delAll (unit %u)"
                            , ret, expect_result, unit);
        }
    }

    return RT_ERR_OK;
}

int32 dal_vlan_egrVlanCnvtRangeCheckVid_test(uint32 caseNo, uint32 unit)
{
    int32 ret, ret1;
    rtk_vlan_vlanCnvtRangeCheck_vid_t egrVlanCnvtRangeCheckVid_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, 0, &egrVlanCnvtRangeCheckVid_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, 0, &egrVlanCnvtRangeCheckVid_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last;

        uint32  rand_index;
        uint32  vid_rand_1, vid_rand_2;
        rtk_vlan_vlanCnvtRangeCheck_vid_t egrVlanCnvtRangeCheckVid;
        rtk_vlan_vlanCnvtRangeCheck_vid_t result_egrVlanCnvtRangeCheckVid;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MAX(unit), TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MIN, index, index_result, index_last);

        for (index_index = 0; index_index <= index_last; index_index++)
        {
            inter_result2 = index_result[index_index];
            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&egrVlanCnvtRangeCheckVid, 0, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
                vid_rand_1 = ut_rand()%4096;
                vid_rand_2 = ut_rand()%4096;
                if (vid_rand_2 >= vid_rand_1)
                {
                    egrVlanCnvtRangeCheckVid.vid_lower_bound = vid_rand_1;
                    egrVlanCnvtRangeCheckVid.vid_upper_bound = vid_rand_2;
                }
                else
                {
                    egrVlanCnvtRangeCheckVid.vid_lower_bound = vid_rand_2;
                    egrVlanCnvtRangeCheckVid.vid_upper_bound = vid_rand_1;
                }
                egrVlanCnvtRangeCheckVid.vid_type = ut_rand()%2;

                ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, index[index_index], &egrVlanCnvtRangeCheckVid);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);

                    ret = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, index[index_index], &result_egrVlanCnvtRangeCheckVid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_upper_bound %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_upper_bound, result_egrVlanCnvtRangeCheckVid.vid_upper_bound, unit, index[index_index], egrVlanCnvtRangeCheckVid.vid_upper_bound);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_lower_bound %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_lower_bound, result_egrVlanCnvtRangeCheckVid.vid_lower_bound, unit, index[index_index], egrVlanCnvtRangeCheckVid.vid_lower_bound);
//                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, reverse %u)"
//                                    , egrVlanCnvtRangeCheckVid.reverse, result_egrVlanCnvtRangeCheckVid.reverse, unit, index[index_index], egrVlanCnvtRangeCheckVid.reverse);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_type %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_type, result_egrVlanCnvtRangeCheckVid.vid_type, unit, index[index_index], egrVlanCnvtRangeCheckVid.vid_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index[index_index]);
                }
            }
            osal_memset(&egrVlanCnvtRangeCheckVid, 0, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, index[index_index], &egrVlanCnvtRangeCheckVid);
            ret1 = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, index[index_index], &result_egrVlanCnvtRangeCheckVid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index[index_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index[index_index]);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index[index_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        uint32 index;
        uint32 index_max;
        uint32  rand_index;
        uint32  vid_rand_1, vid_rand_2;
        rtk_vlan_vlanCnvtRangeCheck_vid_t egrVlanCnvtRangeCheckVid;
        rtk_vlan_vlanCnvtRangeCheck_vid_t result_egrVlanCnvtRangeCheckVid;

        G_VALUE(TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MAX(unit), index_max);

        for (L_VALUE(TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MIN, index); index <= index_max; index++)
        {
            ASSIGN_EXPECT_RESULT(index, TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MIN, TEST_EGRVLANCNVT_RANGECHECKVID_ENTRY_MAX(unit), inter_result2);

            for (rand_index = TEST_RAND_IDX_MIN; rand_index <= TEST_RAND_IDX_MAX; rand_index++)
            {
                ASSIGN_EXPECT_RESULT(rand_index, TEST_RAND_IDX_MIN, TEST_RAND_IDX_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                osal_memset(&egrVlanCnvtRangeCheckVid, 0, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
                vid_rand_1 = ut_rand()%4096;
                vid_rand_2 = ut_rand()%4096;
                if (vid_rand_2 >= vid_rand_1)
                {
                    egrVlanCnvtRangeCheckVid.vid_lower_bound = vid_rand_1;
                    egrVlanCnvtRangeCheckVid.vid_upper_bound = vid_rand_2;
                }
                else
                {
                    egrVlanCnvtRangeCheckVid.vid_lower_bound = vid_rand_2;
                    egrVlanCnvtRangeCheckVid.vid_upper_bound = vid_rand_1;
                }
                egrVlanCnvtRangeCheckVid.vid_type = ut_rand()%2;

                ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, index, &egrVlanCnvtRangeCheckVid);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);

                    ret = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, index, &result_egrVlanCnvtRangeCheckVid);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_upper_bound %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_upper_bound, result_egrVlanCnvtRangeCheckVid.vid_upper_bound, unit, index, egrVlanCnvtRangeCheckVid.vid_upper_bound);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_lower_bound %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_lower_bound, result_egrVlanCnvtRangeCheckVid.vid_lower_bound, unit, index, egrVlanCnvtRangeCheckVid.vid_lower_bound);
                    //RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, reverse %u)"
                //                    , egrVlanCnvtRangeCheckVid.reverse, result_egrVlanCnvtRangeCheckVid.reverse, unit, index, egrVlanCnvtRangeCheckVid.reverse);
                    RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u, vid_type %u)"
                                    , egrVlanCnvtRangeCheckVid.vid_type, result_egrVlanCnvtRangeCheckVid.vid_type, unit, index, egrVlanCnvtRangeCheckVid.vid_type);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, expect_result, unit, index);
                }
            }
            osal_memset(&egrVlanCnvtRangeCheckVid, 0, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
            ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, index, &egrVlanCnvtRangeCheckVid);
            ret1 = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, index, &result_egrVlanCnvtRangeCheckVid);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                , ret,  expect_result, unit, index);

                RT_TEST_IS_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                , ret1, expect_result, unit, index);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_set (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit, index);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_vlan_egrVlanCnvtRangeCheckVid_get (unit %u, index %u)"
                                    , ret1, RT_ERR_OK, unit, index);
            }
        }
    }

    return RT_ERR_OK;
}
