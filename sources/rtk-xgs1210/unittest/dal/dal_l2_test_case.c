/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 77407 $
 * $Date: 2017-04-11 19:02:23 +0800 (Tue, 11 Apr 2017) $
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
#include <rtk/l2.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

/* Define symbol used for test input */
/* Common */
#define L2_RANDOM_RUN_TIMES        10

/* Multicast */
#define TEST_MC_FWD_TBL_ENTRY_MIN           0
#define TEST_MC_FWD_TBL_ENTRY_MAX(unit)     (HWP_8390_50_FAMILY(unit)?4095:\
                                                HWP_9300_FAMILY_ID(unit)?1023:511)

/*Mac Cnt*/
#define TEST_MAC_CNT_MIN                    (0)
#define TEST_MAC_CNT_MAX(unit)              (HWP_8390_50_FAMILY(unit) ? (16 * 1024 + 64) : (8 * 1024)) /*16k(hash) + 64(CAM)*//*8k(hash)*/
/*Action*/
#define TEST_ACT_ID_MIN                     ACTION_FORWARD

#define TEST_ACT_ID_MAX(unit)               (HWP_8380_30_FAMILY(unit)||HWP_8390_50_FAMILY(unit)?ACTION_TRAP2CPU:\
                                                HWP_9300_FAMILY_ID(unit)?ACTION_COPY2MASTERCPU : ACTION_END)

#define TEST_ADDR_ACT_ID_MAX(unit)          (HWP_8380_30_FAMILY(unit)||HWP_8390_50_FAMILY(unit)?ACTION_TRAP2CPU:\
                                                    HWP_9300_FAMILY_ID(unit)?ACTION_COPY2MASTERCPU : ACTION_END)
#define TEST_ACT_MAX(unit)                  (HWP_8380_30_FAMILY(unit)||HWP_8390_50_FAMILY(unit)?ACTION_COPY2CPU:\
                                                    HWP_9300_FAMILY_ID(unit)?ACTION_COPY2MASTERCPU : ACTION_END)

#define TEST_LIMIT_ACT_ID_MAX(unit)          (HWP_8380_30_FAMILY(unit)||HWP_8390_50_FAMILY(unit)?LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU:\
                                                    HWP_9300_FAMILY_ID(unit)?LIMIT_LEARN_CNT_ACTION_COPY_TO_MASTER_CPU : LIMIT_LEARN_CNT_ACTION_END)
#define TEST_LIMIT_ACT_ID_MIN               LIMIT_LEARN_CNT_ACTION_FORWARD

/*Vlan Learning Limitation entry*/
#define TEST_VML_ID_MIN                     0
#define TEST_VML_ID_MAX(unit)               (HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)-1)
/*vlan mode*/
#define TEST_VLAN_MODE_MIN                    BASED_ON_INNER_VLAN
#define TEST_VLAN_MODE_MAX                    BASED_ON_OUTER_VLAN
/*Learning mode*/
#define TEST_LRN_MODE_MIN                    HARDWARE_LEARNING
#define TEST_LRN_MODE_MAX                    NOT_LEARNING
/*IVL/SVL Mode*/
#define TEST_IVL_MODE_MIN                    UC_LOOKUP_ON_VID
#define TEST_IVL_MODE_MAX                    UC_LOOKUP_ON_FID
/*l2multicast mode*/
#define TEST_MCAST_MODE_MIN                    MC_LOOKUP_ON_VID
#define TEST_MCAST_MODE_MAX                    MC_LOOKUP_ON_FID
/*ip multicast mode*/
#define TEST_IPMCAST_MODE_MIN                    LOOKUP_ON_FVID_AND_MAC
#define TEST_IPMCAST_MODE_MAX                    LOOKUP_ON_DIP_AND_FVID
/*Address Table Lookup Miss Type*/
#define TEST_LKMISS_TYPE_MIN                    DLF_TYPE_IPMC
#define TEST_LKMISS_TYPE_MAX                    DLF_TYPE_MCAST
/*Address Table Lookup Miss Action*/
#define TEST_LKMISS_ACT_MIN                    ACTION_FORWARD
/*Address Table Lookup Miss Type (Port)*/
#define TEST_PORTLKMISS_TYPE_MIN                    DLF_TYPE_IPMC
#define TEST_PORTLKMISS_TYPE_MAX                    DLF_TYPE_MCAST
/*Address Table Lookup Miss Action (Port)*/
#define TEST_PORTLKMISS_ACT_MIN                    ACTION_FORWARD
#define TEST_PORTLKMISS_ACT_MAX                    ACTION_END

/*Exception SA Type*/

#define TEST_EXCEPTSA_TYPE_MIN                    SA_IS_ZERO
#define TEST_EXCEPTSA_TYPE_MAX                    SA_IS_BCAST_OR_MCAST
/*IPv6 LOOKUP ON ACTION*/
#define TEST_LOOKUP_ON_ACT_MIN                    LOOKUP_ON_FVID_AND_MAC
#define TEST_LOOKUP_ON_ACT_MAX                    LOOKUP_ON_DIP_AND_FVID
/*IPv6 CARE_BYTE TYPE*/
#define TEST_CARE_BYTE_TYPE_MIN                    L2_DIP_HASH_CARE_BYTE
#define TEST_CARE_BYTE_TYPE_MAX                    L2_SIP_HASH_CARE_BYTE

#define TEST_DLF_TYPE_CHECK(unit)                  DLF_TYPE_UCAST

/*Notification BackPressure Thresh*/
#define TEST_CARE_BP_THRESH_MIN                    0
/*Dynamic PortMoveForbid Action (Port)*/
#define TEST_DYNAMIC_PMOVE_FB_ACT_MIN              ACTION_FORWARD
#define TEST_DYNAMIC_PMOVE_FB_ACT_MAX              ACTION_COPY2MASTERCPU

static rtk_l2_ucastAddr_t  l2_ucastEntry_array[] =
{
   {    vid:10,
         mac:{octet:{0x0, 0x1,0x2,0x3,0x4,0x5}},
         port:0,
         trk_gid:1,
         flags:0x8, /*static*/
         state:0,

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
         agg_vid:10
#endif
   },
   {    vid:11,
         mac:{octet:{0xa4, 0xdc,0x55,0x44,0x32,0x19}},
         port:10,
         trk_gid:1,
         flags:1,
         state:1,

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
          agg_vid:11
#endif
   },
   {    vid:1,
         mac:{octet:{0xb4, 0xdc,0xd5,0x44,0x42,0xc9}},
         port:10,
         trk_gid:1,
         flags:2,
         state:1,

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
         agg_vid:1
#endif
   },

   {    vid:55,
         mac:{octet:{0xb0, 0xdc,0xdf,0x74,0x82,0xc9}},
         port:12,
         trk_gid:1,
         flags:0x8, /*static*/
         state:1,
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
         agg_vid:55
#endif
   },

   {    vid:126,
         mac:{octet:{0x00, 0xdf,0xef,0x94,0x82,0xca}},
         port:28,
         trk_gid:1,
         flags:2,
         state:1,
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
         agg_vid:126
#endif
   },
};

static rtk_l2_mcastAddr_t  l2_mcastEntry_array[] =
{
   {    rvid:10,
         mac:{octet:{0xff, 0x1,0x2,0x3,0x4,0x5}},
         portmask:{{0x1}},
   },
   {    rvid:11,
         mac:{octet:{0xa1, 0xdc,0x55,0x44,0x32,0x19}},
         portmask:{{0xaa}},
   },
   {    rvid:1,
         mac:{octet:{0xb5, 0xdc,0xd5,0x44,0x42,0xc9}},
         portmask:{{0x23b}},
   },

   {    rvid:55,
         mac:{octet:{0xb1, 0xdc,0xdf,0x74,0x82,0xc9}},
         portmask:{{0x123456}},
   },

   {    rvid:126,
         mac:{octet:{0x23, 0xdf,0xef,0x94,0x82,0xca}},
         portmask:{{0xabcdef}},
   },

   {    rvid:127,
         mac:{octet:{0x0f, 0xdf,0xef,0x94,0x82,0x55}},
         portmask:{{0xabcdef}},
   },
};

static rtk_l2_ip6McastAddr_t  l2_ip6McastEntry_array[] =
{
   {    rvid:10,
         dip:{octet:{0xff,0xd1,0xa2,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}},
         sip:{octet:{0x01,0x01,0x52,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}},
         portmask:{{0x1}},
    },
   {    rvid:11,
         dip:{octet:{0xff,0x91,0x62,0x63,0x64,0x95,0x66,0xa7,0xa8,0xa9,0xaa,0x09,0xa9,0x0d,0x0e,0x6f}},
         sip:{octet:{0xa7,0x71,0xc6,0x03,0xcc,0x05,0xdd,0xd7,0x08,0x09,0xc9,0x0b,0x7c,0x0d,0x0e,0x0f}},
         portmask:{{0xaa}},
    },
   {    rvid:1,
         dip:{octet:{0xff,0x01,0xb2,0x03,0xf4,0x05,0xb6,0x97,0xa8,0xb9,0x0a,0x0b,0xac,0xad,0x0e,0x0f}},
         sip:{octet:{0x0f,0x01,0xa2,0x83,0x04,0x05,0x86,0x07,0x08,0x09,0x0a,0x0b,0x8c,0x9d,0x0e,0x0f}},
         portmask:{{0x23b}},
    },

   {    rvid:55,
         dip:{octet:{0xf1,0xf4,0x02,0x73,0xc4,0x5d,0x06,0x6f,0xc8,0xf4,0xf7,0x0b,0x0c,0x0d,0x3e,0x0f}},
         sip:{octet:{0x61,0xd1,0x02,0x03,0xb4,0x05,0x06,0x07,0x08,0x0f,0x0a,0xad,0x0c,0x0d,0x0e,0x0f}},
         portmask:{{0x123456}},
    },

   {    rvid:126,
         dip:{octet:{0xff,0x61,0x82,0x03,0x04,0xf3,0xf6,0x07,0x58,0xd9,0x0a,0x0b,0x0c,0x2d,0x9e,0x0f}},
         sip:{octet:{0x71,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}},
         portmask:{{0xabcdef}},
    },

   {    rvid:127,
         dip:{octet:{0x5f,0x01,0xc2,0x23,0x04,0x4f,0x06,0xf7,0x08,0xe9,0x0a,0x0b,0x0c,0x1d,0x0e,0xef}},
         sip:{octet:{0xd1,0x01,0xd2,0x03,0x04,0x5d,0x06,0x07,0x7f,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f}},
         portmask:{{0xabcdef}},
    },
};

static rtk_l2_ipMcastAddr_t  l2_ipMcastEntry_array[] =
{
   {    rvid:10,
         dip:0xe2574567,
         sip:0x1256de34,
         portmask:{{0x1}},
   },
   {    rvid:11,
         dip:0xe2345878,
         sip:0x12acde34,
         portmask:{{0xaa}},
   },
   {    rvid:1,
         dip:0xe234be00,
         sip:0x190cde34,
         portmask:{{0x23b}},
   },

   {    rvid:55,
         dip:0xe2555678,
         sip:0x1200de34,
         portmask:{{0x123456}},
   },

   {    rvid:126,
         dip:0xe2377634,
         sip:0x12a88e34,
         portmask:{{0xabcdef}},
   },

   {    rvid:127,
         dip:0xe09456ac,
         sip:0xfeacde34,
         portmask:{{0xabcdef}},
   },
};

static int32 _dal_macAddr_clearAll(uint32 unit);

int32 dal_l2_flushLinkDownPortAddrEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_flushLinkDownPortAddrEnable_get(unit, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_flushLinkDownPortAddrEnable_set(unit, TEST_ENABLE_STATUS_MAX)))
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
            ret = rtk_l2_flushLinkDownPortAddrEnable_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_l2_flushLinkDownPortAddrEnable_get(unit, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_get (unit %u, enable %u,)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }

        }

        ret = rtk_l2_flushLinkDownPortAddrEnable_get(unit, &enable[enable_index]);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                            , ret, expect_result, unit, enable[enable_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                                , ret, RT_ERR_OK, unit, enable[enable_index]);
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
            ret = rtk_l2_flushLinkDownPortAddrEnable_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_l2_flushLinkDownPortAddrEnable_get(unit,  (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_flushLinkDownPortAddrEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    /* Restore */
    rtk_l2_flushLinkDownPortAddrEnable_set(unit, enable_temp);
    return RT_ERR_OK;
}   /*end of dal_l2_flushLinkDownPortAddrEnable_test*/

int32 dal_l2_portLimitLearningCnt_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  count_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLimitLearningCnt_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLimitLearningCnt_get(unit, 0, &count_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last = 0;

        uint32  count[UNITTEST_MAX_TEST_VALUE];
        int32  count_result[UNITTEST_MAX_TEST_VALUE];
        int32  count_index;
        int32  count_last;

        int32  result_count;

        if(HWP_8380_30_FAMILY(unit))
        {
            UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        }
        else
        {
            UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        }

        UNITTEST_TEST_VALUE_ASSIGN(HAL_L2_LEARN_LIMIT_CNT_MAX(unit), 0, count, count_result, count_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (count_index = 0; count_index <= count_last; count_index++)
            {
                inter_result3 = count_result[count_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_portLimitLearningCnt_set(unit, port[port_index], count[count_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u, count %u)"
                                    , ret, expect_result, unit, port[port_index], count[count_index]);

                    ret = rtk_l2_portLimitLearningCnt_get(unit, port[port_index], (uint32*)&result_count);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_get (unit %u, port %u, count %u)"
                                     , ret, expect_result, unit, port[port_index], count[count_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set/get value unequal (unit %u, port %u, count %u)"
                                     , result_count, count[count_index], unit, port[port_index], count[count_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u, count %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], count[count_index]);
                }
            }

            ret = rtk_l2_portLimitLearningCnt_get(unit, port[port_index], (uint32*)&result_count);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max = 0;
        uint32  count;
        uint32  count_max;
        uint32  result_count;

        if(HWP_8380_30_FAMILY(unit))
        {
            G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        }
        else
        {
            G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        }
        G_VALUE(HAL_L2_LEARN_LIMIT_CNT_MAX(unit), count_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            if(HWP_8380_30_FAMILY(unit))
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            }
            else
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            }
            /* If the feature is not support on CPU port, once port == CPU port, doesn't do this check.*/
//              if(!(port == HWP_CPU_MACID(unit))&&(port_max == TEST_PORT_ID_MAX(unit)))
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_MAC_CNT_MIN, count); count <= count_max; count++)
            {
                ASSIGN_EXPECT_RESULT(count, TEST_MAC_CNT_MIN, HAL_L2_LEARN_LIMIT_CNT_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_l2_portLimitLearningCnt_set(unit, port, count);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u, count %u)"
                                    , ret, expect_result, unit, port, count);
                    ret = rtk_l2_portLimitLearningCnt_get(unit, port, (uint32*)&result_count);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_get (unit %u, port %u, count %u)"
                                     , ret, expect_result, unit, port, count);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set/get value unequal (unit %u, port %u, count %u)"
                                     , result_count, count, unit, port, count);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, port %u, count %u)"
                                        , ret, RT_ERR_OK, unit, port, count);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_portLimitLearningCnt_test*/


int32 dal_l2_portLimitLearningCntAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_limitLearnCntAction_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLimitLearningCntAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLimitLearningCntAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last = 0;

        uint32  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_l2_limitLearnCntAction_t  result_action;


        if(HWP_8380_30_FAMILY(unit))
        {
            UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        }
        else
        {
            UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        }
        UNITTEST_TEST_VALUE_ASSIGN(TEST_LIMIT_ACT_ID_MAX(unit), TEST_LIMIT_ACT_ID_MIN, action, action_result, action_last);


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
                ret = rtk_l2_portLimitLearningCntAction_set(unit, port[port_index], (rtk_l2_limitLearnCntAction_t)action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_l2_portLimitLearningCntAction_get(unit, port[port_index], (rtk_l2_limitLearnCntAction_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_l2_portLimitLearningCntAction_get(unit, port[port_index], (rtk_l2_limitLearnCntAction_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max = 0;
        rtk_l2_limitLearnCntAction_t  action;
        rtk_l2_limitLearnCntAction_t  action_max;
        rtk_l2_limitLearnCntAction_t  result_action;

        if(HWP_8380_30_FAMILY(unit))
            G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max);
        else
            G_VALUE(TEST_PORT_ID_MAX_WO_CPU(unit), port_max); /* Exclude CPU port */
        G_VALUE(TEST_ACT_ID_MAX(unit), action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            if(HWP_8380_30_FAMILY(unit))
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX_WO_CPU(unit), inter_result2);
            }
            else
            {
                ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            }
            /* If the feature is not support on CPU port, once port == CPU port, doesn't do this check.*/
//              if(!(port == HWP_CPU_MACID(unit))&&(port_max == TEST_PORT_ID_MAX_WO_CPU(unit)))
            UNITTEST_TEST_EXISTED_PORT_CHECK_WO_CPU(unit, port, inter_result2);

            for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_LIMIT_ACT_ID_MIN, TEST_LIMIT_ACT_ID_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_portLimitLearningCntAction_set(unit, port, (rtk_l2_limitLearnCntAction_t)action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);
                    ret = rtk_l2_portLimitLearningCntAction_get(unit, port, (rtk_l2_limitLearnCntAction_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_limitLearningCntAction_test*/

int32 dal_l2_fidLimitLearningEntry_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_fidMacLimitEntry_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_fidLimitLearningEntry_get(unit, 0, &action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_fidLimitLearningEntry_set(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  entryIdx[UNITTEST_MAX_TEST_VALUE];
        int32  entryIdx_result[UNITTEST_MAX_TEST_VALUE];
        int32  entryIdx_index;
        int32  entryIdx_last;
        rtk_l2_fidMacLimitEntry_t  entry;
        rtk_l2_fidMacLimitEntry_t  result_entry;

        int32 randIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_VML_ID_MAX(unit), TEST_VML_ID_MIN, entryIdx, entryIdx_result, entryIdx_last);

        for (entryIdx_index = 0; entryIdx_index <= entryIdx_last; entryIdx_index++)
        {
            inter_result2 = entryIdx_result[entryIdx_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            for (randIdx = 0; randIdx <= L2_RANDOM_RUN_TIMES; randIdx++)
            {
                osal_memset(&entry, 0, sizeof(rtk_l2_fidMacLimitEntry_t));
                entry.fid = ut_rand()&BITMASK_12B;
                entry.maxNum = ut_rand()%TEST_MAC_CNT_MAX(unit);
                entry.port = ut_rand()%RTK_MAX_NUM_OF_PORTS;
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                if(!HWP_PORT_EXIST(unit, entry.port))
                    expect_result = RT_ERR_FAILED;
                ret = rtk_l2_fidLimitLearningEntry_set(unit, entryIdx[entryIdx_index], (rtk_l2_fidMacLimitEntry_t *)&entry);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_fidLimitLearningEntry_set (unit %u, entryIdx %u)"
                                    , ret, expect_result, unit, entryIdx[entryIdx_index]);

                    ret = rtk_l2_fidLimitLearningEntry_get(unit, entryIdx[entryIdx_index], (rtk_l2_fidMacLimitEntry_t*)&result_entry);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_fidLimitLearningEntry_get (unit %u, entryIdx %u)"
                                     , ret, expect_result, unit, entryIdx[entryIdx_index]);
                    /*Check Entry Content*/
                    ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_fidMacLimitEntry_t));
                    if (ret != 0)
                    {
                        osal_printf("entry.fid=%d, result_entry.fid=%d\n", entry.fid, result_entry.fid);
                        osal_printf("entry.maxNum=%d, result_entry.maxNum=%d\n", entry.maxNum, result_entry.maxNum);
                        if (HWP_8390_50_FAMILY(unit) || HWP_8380_30_FAMILY(unit))
                        {
                            osal_printf("entry.port=%d, result_entry.port=%d\n", entry.port, result_entry.port);
                        }
                    }
                    RT_TEST_IS_EQUAL_INT("rtk_l2_fidLimitLearningEntry_get/get value unequal (unit %u, entryIdx %u)"
                                , ret, 0, unit, entryIdx[entryIdx_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_fidLimitLearningEntry_set (unit %u, entryIdx %u)"
                                        , ret, RT_ERR_OK, unit, entryIdx[entryIdx_index]);
                }

            }

            ret = rtk_l2_fidLimitLearningEntry_get(unit, entryIdx[entryIdx_index], (rtk_l2_fidMacLimitEntry_t*)&result_entry);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLimitLearningEntry_set (unit %u, entryIdx %u)"
                                , ret, expect_result, unit, entryIdx[entryIdx_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_fidLimitLearningEntry_set (unit %u, entryIdx %u)"
                                    , ret, RT_ERR_OK, unit, entryIdx[entryIdx_index]);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_fidLimitLearningEntry_test*/

int32 dal_l2_aging_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  aging_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_aging_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_aging_get(unit, &aging_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32 aging;
        int32  aging_result;
        int32 randIdx;

        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        for (randIdx = 0; randIdx <= L2_RANDOM_RUN_TIMES; randIdx++)
        {
            aging = ut_rand()& 0xfff;
            ret = rtk_l2_aging_set(unit, aging);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_aging_set (unit %u, aging %u)"
                                , ret, expect_result, unit, aging);

                ret = rtk_l2_aging_get(unit, (uint32*)&aging_result);
                RT_TEST_IS_EQUAL_INT("rtk_l2_aging_get (unit %u, aging %u)"
                                 , ret, expect_result, unit, aging);
                if (aging != aging_result)
                {
                    /* Maybe have 1-2 unit transfer different */
                    if ((aging_result > aging+2) || (aging_result < aging-2))
                        RT_TEST_IS_EQUAL_INT("rtk_l2_aging_set/get value unequal (unit %u, aging %u)"
                                         , aging_result, aging, unit, aging);
                }
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_aging_set (unit %u, aging %u)"
                                    , ret, RT_ERR_OK, unit, aging);
            }
        }

    }

    return RT_ERR_OK;
}   /*end of dal_l2_aging_test*/

int32 dal_l2_hashAlgo_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_hashAlgo_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_hashAlgo_get(unit, &enable_temp)))
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
            ret = rtk_l2_hashAlgo_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_l2_hashAlgo_get(unit, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_get (unit %u, enable %u,)"
                                 , ret, expect_result, unit, enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }

        }

        ret = rtk_l2_hashAlgo_get(unit, &enable[enable_index]);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                            , ret, expect_result, unit, enable[enable_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                                , ret, RT_ERR_OK, unit, enable[enable_index]);
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
            ret = rtk_l2_hashAlgo_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_l2_hashAlgo_get(unit,  (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_hashAlgo_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_hashAlgo_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_hashAlgo_test*/

int32 dal_l2_vlanMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_vlanMode_t  vlan_mode_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_vlanMode_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_vlanMode_get(unit, 0, &vlan_mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_vlan_t  vlan_mode[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_mode_index;
        int32  vlan_mode_last;

        rtk_l2_vlanMode_t  result_vlan_mode;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_MODE_MAX, TEST_VLAN_MODE_MIN, vlan_mode, vlan_mode_result, vlan_mode_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (vlan_mode_index = 0; vlan_mode_index <= vlan_mode_last; vlan_mode_index++)
            {
                inter_result3 = vlan_mode_result[vlan_mode_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_vlanMode_set(unit, port[port_index], vlan_mode[vlan_mode_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u, vlan_mode %u)"
                                    , ret, expect_result, unit, port[port_index], vlan_mode[vlan_mode_index]);

                    ret = rtk_l2_vlanMode_get(unit, port[port_index], (rtk_l2_vlanMode_t*)&result_vlan_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_get (unit %u, port %u, vlan_mode %u)"
                                     , ret, expect_result, unit, port[port_index], vlan_mode[vlan_mode_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_set/get value unequal (unit %u, port %u, vlan_mode %u)"
                                     , result_vlan_mode, vlan_mode[vlan_mode_index], unit, port[port_index], vlan_mode[vlan_mode_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u, vlan_mode %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], vlan_mode[vlan_mode_index]);
                }
            }

            ret = rtk_l2_vlanMode_get(unit, port[port_index], (rtk_l2_vlanMode_t*)&result_vlan_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port;
        rtk_port_t  port_max;
        rtk_l2_vlanMode_t  vlan_mode;
        rtk_l2_vlanMode_t  vlan_mode_max;
        rtk_l2_vlanMode_t  result_vlan_mode;

        G_VALUE(TEST_PORT_ID_MAX(unit), port_max);
        G_VALUE(TEST_VLAN_MODE_MAX, vlan_mode_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_VLAN_MODE_MIN, vlan_mode); vlan_mode <= vlan_mode_max; vlan_mode++)
            {
                ASSIGN_EXPECT_RESULT(vlan_mode, TEST_VLAN_MODE_MIN, TEST_VLAN_MODE_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_vlanMode_set(unit, port, vlan_mode);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u, vlan_mode %u)"
                                    , ret, expect_result, unit, port, vlan_mode);
                    ret = rtk_l2_vlanMode_get(unit, port, &result_vlan_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_get (unit %u, port %u, vlan_mode %u)"
                                     , ret, expect_result, unit, port, vlan_mode);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_vlanMode_set/get value unequal (unit %u, port %u, vlan_mode %u)"
                                     , result_vlan_mode, vlan_mode, unit, port, vlan_mode);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_vlanMode_set (unit %u, port %u, vlan_mode %u)"
                                        , ret, RT_ERR_OK, unit, port, vlan_mode);
            }

        }
    }



    return RT_ERR_OK;
} /*end of dal_l2_vlanMode_test*/

int32 dal_l2_portNewMacOp_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_newMacLrnMode_t  mode_temp;
    rtk_action_t            fwdAction_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portNewMacOp_set(unit, 0, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portNewMacOp_get(unit, 0, &mode_temp, &fwdAction_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last = 0;

        rtk_l2_newMacLrnMode_t  mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_l2_newMacLrnMode_t  result_mode;
        rtk_action_t            result_action;



        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX_WO_CPU(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_LRN_MODE_MAX, TEST_LRN_MODE_MIN, mode, mode_result, mode_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ACT_ID_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (mode_index = 0; mode_index <= mode_last; mode_index++)
            {
                inter_result3 = mode_result[mode_index];
                 for (action_index = 0; action_index <= action_last; action_index++)
                 {
                    inter_result4 = action_result[action_index];
                    if(!(HWP_UNIT_VALID_LOCAL(unit)))
                        expect_result = RT_ERR_FAILED;
                    else
                        expect_result = (inter_result2 && inter_result3 && inter_result4)? RT_ERR_OK : RT_ERR_FAILED;
                    ret = rtk_l2_portNewMacOp_set(unit, port[port_index], mode[mode_index], action[action_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portNewMacOp_set (unit %u, port %u, mode %u, action %u)"
                                        , ret, expect_result, unit, port[port_index], mode[mode_index], action[action_index]);

                        ret = rtk_l2_portNewMacOp_get(unit, port[port_index], (rtk_l2_newMacLrnMode_t*)&result_mode, (rtk_action_t*)&result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portNewMacOp_get (unit %u, port %u, mode %u, action %u)"
                                         , ret, expect_result, unit, port[port_index], mode[mode_index], action[action_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portNewMacOp_set/get value unequal (unit %u, port %u, mode %u, action %u)"
                                         , result_mode, mode[mode_index], unit, port[port_index], mode[mode_index],  action[action_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portNewMacOp_set/get value unequal (unit %u, port %u, mode %u, action %u)"
                                         , result_action, action[action_index], unit, port[port_index], mode[mode_index], action[action_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portNewMacOp_set (unit %u, port %u, mode %u, action %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], mode[mode_index], action[action_index]);
                    }
                 }

            }

            ret = rtk_l2_portNewMacOp_get(unit, port[port_index], (rtk_l2_newMacLrnMode_t*)&result_mode, (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_portNewMacOp_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portNewMacOp_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_l2_newMacOp_test*/

int32 dal_l2_addr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;

        int32 randIdx;

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ucastAddr_t));
            entry.vid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
            entry.state = 0;
            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(entry.mac.octet[0] & BITMASK_1B)
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.vid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_addr_add(unit, (rtk_l2_ucastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ucastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get/get value unequal (unit %u)"
                            , ret, 0, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_addr_test*/

int32 dal_l2_addr_test1(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;
        int32 randIdx;

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ucastAddr_t));
            entry.vid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
            entry.state = 0;
            if (HWP_8390_50_FAMILY(unit))
                UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(entry.mac.octet[0] & BITMASK_1B)
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.vid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_addr_add(unit, (rtk_l2_ucastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ucastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_addr_test1*/

int32 dal_l2_addr_del_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;
        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ucastAddr_t));
            entry.vid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | !(RTK_L2_UCAST_FLAG_STATIC));

            entry.state = 0;
            if (HWP_8390_50_FAMILY(unit))
                UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)) || (!HWP_PORT_EXIST(unit, entry.port)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(entry.mac.octet[0] & BITMASK_1B)
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.vid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_addr_add(unit, (rtk_l2_ucastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_addr_del(unit, entry.vid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, RT_ERR_OK, unit);

                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_addr_del_test*/

int32 dal_l2_addr_set_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;
        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 1);

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ucastAddr_t));
            entry.vid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;
            entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | !(RTK_L2_UCAST_FLAG_STATIC));
            entry.state = 0;
            if (HWP_8390_50_FAMILY(unit))
                UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(entry.mac.octet[0] & BITMASK_1B)
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.vid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_addr_add(unit, (rtk_l2_ucastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));


                entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                                   RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
                entry.state = 0;
                if (HWP_8390_50_FAMILY(unit))
                    UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
                ret = rtk_l2_addr_set(unit, (rtk_l2_ucastAddr_t*)&entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                 , ret, expect_result, unit);

                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ucastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get/get value unequal (unit %u)"
                            , ret, 0, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_addr_set_test*/

int32 dal_l2_addr_getNext_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;
        rtk_portmask_t  trunk_member;

        int32 randIdx;
        int32 scan_idx;
        uint32 static_entryNum = 0;
        uint32 dynamic_entryNum = 0;

        /*Remove all Layer2 entrys in the table*/
        ret = rtk_l2_addr_delAll(unit, 1);
        for (randIdx = 0; randIdx < sizeof(l2_ucastEntry_array)/sizeof(rtk_l2_ucastAddr_t); randIdx++)
        {
            osal_memcpy(&entry, &l2_ucastEntry_array[randIdx], sizeof(rtk_l2_ucastAddr_t));

            if (entry.flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
            {
                /* Configure trunk 1 member to port 10-11 */
                osal_memset(&trunk_member, 0, sizeof(rtk_portmask_t));
                trunk_member.bits[0] = 0xc00;
                rtk_trunk_port_set(unit, 1, &trunk_member);
            }

            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);

            ret = rtk_l2_addr_add(unit, (rtk_l2_ucastAddr_t *)&entry);

            RT_TEST_IS_EQUAL_INT("rtk_l2_addr_add Failed(unit %u)"
                                    , ret, RT_ERR_OK, unit);

            if (entry.flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
            {
                /* Clear trunk 1 member */
                osal_memset(&trunk_member, 0, sizeof(rtk_portmask_t));
                rtk_trunk_port_set(unit, 1, &trunk_member);
            }
        }

        /*from start*/
        scan_idx = -1;
        do
        {
            ret = rtk_l2_nextValidAddr_get(unit, &scan_idx, 0, & result_entry); /*Get Dynamic entry*/
            if(ret == RT_ERR_OK)
                dynamic_entryNum++;
        }while(ret == RT_ERR_OK);

        RT_TEST_IS_EQUAL_INT("rtk_l2_nextValidAddr_get dynamic entrys(unit %u)"
                                    , dynamic_entryNum, 3, unit);


        scan_idx = -1;
        do
        {
            ret = rtk_l2_nextValidAddr_get(unit, &scan_idx, 1, & result_entry); /*Get Dynamic & Staic entry*/
            if(ret == RT_ERR_OK)
                static_entryNum++;
        }while(ret == RT_ERR_OK);

        RT_TEST_IS_EQUAL_INT("rtk_l2_nextValidAddr_get statuc entrys(unit %u)"
                                    , static_entryNum, 5, unit);
    }

    /*Remove all Layer2 entrys in the table*/
    ret = rtk_l2_addr_delAll(unit, 1);

    return RT_ERR_OK;
}  /*end of dal_l2_addr_getNext_test*/

int32 dal_l2_ipmcMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipmcMode_t  mode_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipmcMode_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipmcMode_get(unit, &mode_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_l2_ipmcMode_t mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_l2_ipmcMode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_IPMCAST_MODE_MAX, TEST_IPMCAST_MODE_MIN, mode, mode_result, mode_last);

        for (mode_index = 0; mode_index <= mode_last; mode_index++)
        {
            inter_result2 = mode_result[mode_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_l2_ipmcMode_set(unit, mode[mode_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);

                ret = rtk_l2_ipmcMode_get(unit, (rtk_l2_ipmcMode_t*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_get (unit %u,  mode %u)"
                                 , ret, expect_result, unit,  mode[mode_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode[mode_index], unit, mode[mode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u,  mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_ipmcMode_t  mode;
        rtk_l2_ipmcMode_t  mode_max;
        rtk_l2_ipmcMode_t  result_mode;

        G_VALUE(TEST_IPMCAST_MODE_MAX, mode_max);

        for (L_VALUE(TEST_IPMCAST_MODE_MIN, mode); mode <= mode_max; mode++)
        {
            ASSIGN_EXPECT_RESULT(mode, TEST_IPMCAST_MODE_MIN, TEST_IPMCAST_MODE_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            ret = rtk_l2_ipmcMode_set(unit, mode);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode);
                ret = rtk_l2_ipmcMode_get(unit, (rtk_l2_ipmcMode_t*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode, unit, mode);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u,mode %u)"
                                    , ret, RT_ERR_OK, unit, mode);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_ipmcMode_test*/

int32 dal_l2_legalPortMoveAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_legalPortMoveAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_legalPortMoveAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ACT_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);

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
                ret = rtk_l2_legalPortMoveAction_set(unit, port[port_index], (rtk_action_t)action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_l2_legalPortMoveAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_l2_legalPortMoveAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u)"
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
        G_VALUE(TEST_ACT_MAX(unit), action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ACT_ID_MIN, TEST_ACT_ID_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_legalPortMoveAction_set(unit, port, (rtk_action_t)action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);
                    ret = rtk_l2_legalPortMoveAction_get(unit, port, (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_legalPortMoveAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_legalPortMoveAction_test*/


int32 dal_l2_lookupMissFloodPortMask_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  port_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_lookupMissFloodPortMask_setByIndex(unit, 0,DLF_TYPE_UCAST, 0))
            || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_lookupMissFloodPortMask_get(unit, DLF_TYPE_UCAST, &port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    {

        rtk_port_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_portmask_t  portMsk;
        rtk_portmask_t  portMsk_result;
        int32           randIdx;
        uint8           portMsk_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];


        UNITTEST_TEST_VALUE_ASSIGN(TEST_LKMISS_TYPE_MAX, TEST_LKMISS_TYPE_MIN, type, type_result, type_last);

        for (type_index = 0; type_index <= type_last; type_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (type_result[type_index])? RT_ERR_OK : RT_ERR_FAILED;;
            for (randIdx = 0; randIdx <= L2_RANDOM_RUN_TIMES; randIdx++)
            {
                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &portMsk);
                ret = rtk_l2_lookupMissFloodPortMask_setByIndex(unit, type[type_index], DLF_TYPE_UCAST, &portMsk);

                if ((HWP_8390_50_FAMILY(unit)||HWP_8380_30_FAMILY(unit)) &&
                    (type[type_index] >= DLF_TYPE_END || type[type_index] == DLF_TYPE_IPMC ||
                    type[type_index] == DLF_TYPE_IP6MC || type[type_index] == DLF_TYPE_MCAST))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit)) && (type[type_index] > DLF_TYPE_BCAST || type[type_index] < DLF_TYPE_UCAST))
                    expect_result = RT_ERR_FAILED;


                if (RT_ERR_OK == expect_result)
                {
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_set (unit %u, type %u, portMsk.bit%s)"
                                    , ret, expect_result, unit, type[type_index], portMsk_pm_str);

                        ret = rtk_l2_lookupMissFloodPortMask_get(unit, type[type_index], &portMsk_result);
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_get (unit %u, type %u, portMsk.bit%s)"
                                     , ret, expect_result, unit, type[type_index], portMsk_pm_str);
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_set/get value unequal (unit %u, type %u, portMsk.bit%s)"
                                     , portMsk_result.bits[0], portMsk.bits[0], unit, type[type_index], portMsk_pm_str);
                }
                else
                {
                    RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_set (unit %u, type %u, portMsk.bit%s)"
                                        , ret, RT_ERR_OK, unit, type[type_index], portMsk_pm_str);
                }
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_lookupMissFloodPortMask_test*/

int32 dal_l2_lookupMissFloodPortMask_add_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  port_temp = 0;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_lookupMissFloodPortMask_add(unit, TEST_DLF_TYPE_CHECK(unit), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_lookupMissFloodPortMask_del(unit, TEST_DLF_TYPE_CHECK(unit), port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_port_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_portmask_t  portMsk_result;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_LKMISS_TYPE_MAX, TEST_LKMISS_TYPE_MIN, type, type_result, type_last);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            for (type_index = 0; type_index <= type_last; type_index++)
            {
                inter_result3 = type_result[type_index];
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                if ((HWP_8390_50_FAMILY(unit)||HWP_8380_30_FAMILY(unit)) &&
                    (type[type_index] >= DLF_TYPE_END || type[type_index] == DLF_TYPE_IPMC ||
                    type[type_index] == DLF_TYPE_IP6MC || type[type_index] == DLF_TYPE_MCAST))
                    expect_result = RT_ERR_FAILED;
                if ((HWP_9300_FAMILY_ID(unit)) && (type[type_index] > DLF_TYPE_BCAST || type[type_index] < DLF_TYPE_UCAST))
                    expect_result = RT_ERR_FAILED;

                /*Test Add Function*/
                ret = rtk_l2_lookupMissFloodPortMask_add(unit, type[type_index], port[port_index]);
                if (RT_ERR_OK == expect_result)
                {
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set (unit %u, type %u, port %u)"
                                        , ret, expect_result, unit, type[type_index], port[port_index]);

                        ret = rtk_l2_lookupMissFloodPortMask_get(unit, type[type_index], &portMsk_result);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_get (unit %u, type %u, port %u)"
                                         , ret, expect_result, unit, type[type_index], port[port_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set/get value unequal (unit %u, type %u, port %u)"
                                         , BITMAP_IS_SET(portMsk_result.bits, port[port_index]) ? TRUE: FALSE, TRUE, unit, type[type_index], port[port_index]);
                }
                else
                {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set (unit %u, type %u, port %u)"
                                            , ret, RT_ERR_OK, unit, type[type_index], port[port_index]);
                }

                /*Test Del Function*/
                    ret = rtk_l2_lookupMissFloodPortMask_del(unit, type[type_index], port[port_index]);
                if (RT_ERR_OK == expect_result)
                {
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set (unit %u, type %u, port %u)"
                                        , ret, expect_result, unit, type[type_index], port[port_index]);

                        ret = rtk_l2_lookupMissFloodPortMask_get(unit, type[type_index], &portMsk_result);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_get (unit %u, type %u, port %u)"
                                         , ret, expect_result, unit, type[type_index], port[port_index]);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set/get value unequal (unit %u, type %u, port %u)"
                                         , BITMAP_IS_SET(portMsk_result.bits, port[port_index])? TRUE: FALSE, FALSE, unit, type[type_index], port[port_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_lookupMissFloodPortMask_add_set (unit %u, type %u, port %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], port[port_index]);
                }
            }
        }
    }

    return RT_ERR_OK;
} /*end of dal_l2_lookupMissFloodPortMask_add_test*/

int32 dal_l2_srcPortEgrFilterMask_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  port_temp;
    int32  expect_result = 1;
    uint8   portMsk_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_srcPortEgrFilterMask_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_srcPortEgrFilterMask_get(unit, &port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_portmask_t portMsk;
        rtk_portmask_t  portMsk_result;
        int32 randIdx;
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        for (randIdx = 0; randIdx <= L2_RANDOM_RUN_TIMES; randIdx++)
        {
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &portMsk);
            ret = rtk_l2_srcPortEgrFilterMask_set(unit, &portMsk);
            if (RT_ERR_OK == expect_result)
            {
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_set (unit %u, portMsk.bit%s)"
                                , ret, expect_result, unit, portMsk_pm_str);

                ret = rtk_l2_srcPortEgrFilterMask_get(unit, &portMsk_result);
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_get (unit %u, portMsk.bit%s)"
                                 , ret, expect_result, unit, portMsk_pm_str);
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_set/get value unequal (unit %u, portMsk.bit%s)"
                                 , portMsk_result.bits[0], portMsk.bits[0], unit, portMsk_pm_str);
            }
            else
            {
                RTK_PORTMASK_SPRINTF(portMsk_pm_str, portMsk);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_set (unit %u, portMsk.bit%s)"
                                    , ret, RT_ERR_OK, unit, portMsk_pm_str);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_srcPortEgrFilterMask_test*/

int32 dal_l2_srcPortEgrFilterMask_add_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_port_t  port_temp = 0;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_srcPortEgrFilterMask_add(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_srcPortEgrFilterMask_del(unit, port_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_portmask_t  portMsk_result;
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);

        for (port_index = 0; port_index <= port_last; port_index++)
        {
            inter_result2 = port_result[port_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            /*Test Add Function*/
            ret = rtk_l2_srcPortEgrFilterMask_add(unit, port[port_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);

                ret = rtk_l2_srcPortEgrFilterMask_get(unit, &portMsk_result);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_get (unit %u, port %u,)"
                                 , ret, expect_result, unit, port[port_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set/get value unequal (unit %u, port %u)"
                                 , BITMAP_IS_SET(portMsk_result.bits, port[port_index]) ? TRUE: FALSE, TRUE, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

            /*Test Del Function*/
            ret = rtk_l2_srcPortEgrFilterMask_del(unit, port[port_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);

                ret = rtk_l2_srcPortEgrFilterMask_get(unit, &portMsk_result);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_get (unit %u, port %u,)"
                                 , ret, expect_result, unit, port[port_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set/get value unequal (unit %u, port %u)"
                                 , BITMAP_IS_SET(portMsk_result.bits, port[port_index])? TRUE: FALSE, FALSE, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_srcPortEgrFilterMask_add_set (unit %u, port %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index]);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_l2_srcPortEgrFilterMask_add_test*/

int32 dal_l2_exceptionAddrAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_exceptionAddrAction_set(unit, TEST_EXCEPTSA_TYPE_MIN, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_exceptionAddrAction_get(unit, TEST_EXCEPTSA_TYPE_MIN, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_l2_exceptionAddrType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;

        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_EXCEPTSA_TYPE_MAX, TEST_EXCEPTSA_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ADDR_ACT_ID_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);


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

                ret = rtk_l2_exceptionAddrAction_set(unit, type[type_index], action[action_index]);
                if ((RT_ERR_OK == expect_result) && (ret == RT_ERR_OK))
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type[type_index], action[action_index]);

                    ret = rtk_l2_exceptionAddrAction_get(unit, type[type_index], (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type[type_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action[action_index], unit, type[type_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type[type_index], action[action_index]);
                }
            }

            ret = rtk_l2_exceptionAddrAction_get(unit, type[type_index], (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u)"
                                , ret, expect_result, unit, type[type_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u)"
                                    , ret, RT_ERR_OK, unit, type[type_index]);
            }

        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_exceptionAddrType_t  type;
        rtk_l2_exceptionAddrType_t  type_max;
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;

        G_VALUE(TEST_EXCEPTSA_TYPE_MAX, type_max);
        G_VALUE(TEST_ACT_ID_MAX(unit), action_max);

        for (L_VALUE(TEST_EXCEPTSA_TYPE_MIN, type); type <= type_max; type++)
        {
            ASSIGN_EXPECT_RESULT(type, TEST_EXCEPTSA_TYPE_MIN, TEST_EXCEPTSA_TYPE_MAX, inter_result2);

            for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ACT_ID_MIN, TEST_ADDR_ACT_ID_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

                ret = rtk_l2_exceptionAddrAction_set(unit, type, action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u, action %u)"
                                    , ret, expect_result, unit, type, action);
                    ret = rtk_l2_exceptionAddrAction_get(unit, type, &result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_get (unit %u, type %u, action %u)"
                                     , ret, expect_result, unit, type, action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_exceptionAddrAction_set/get value unequal (unit %u, type %u, action %u)"
                                     , result_action, action, unit, type, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_exceptionAddrAction_set (unit %u, type %u, action %u)"
                                        , ret, RT_ERR_OK, unit, type, action);
            }

        }
    }

    return RT_ERR_OK;
} /*end of dal_l2_exceptionAddrAction_test*/

int32 dal_l2_mcastAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;
        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_mcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get/add value unequal (unit %u)"
                            , ret, 0, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_test*/

int32 dal_l2_mcastAddr_test1(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;
        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 1);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            entry.portmask.bits[0] = ut_rand() & BITMASK_29B;
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_mcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_test1*/


int32 dal_l2_mcastAddr_del_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;

        int32 randIdx;

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                ret = rtk_l2_mcastAddr_del(unit, entry.rvid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, RT_ERR_OK, unit);

                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_del_test*/

int32 dal_l2_mcastAddr_set_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;
        int32 randIdx;

        for (randIdx = 0; randIdx < 50*L2_RANDOM_RUN_TIMES; randIdx++)

        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

                ret = rtk_l2_mcastAddr_set(unit, (rtk_l2_mcastAddr_t*)&entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                 , ret, expect_result, unit);

                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_mcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_set_test*/

int32 dal_l2_mcastAddr_getNext_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    _dal_macAddr_clearAll(unit);

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;

        int32 randIdx;
        int32 scan_idx;
        uint32 entryNums = 0;


        for (randIdx = 0; randIdx < sizeof(l2_mcastEntry_array)/sizeof(rtk_l2_mcastAddr_t); randIdx++)
        {
            osal_memcpy(&entry, &l2_mcastEntry_array[randIdx], sizeof(rtk_l2_mcastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_add Failed(unit %u)"
                                    , ret, RT_ERR_OK, unit);
         }

        /*from start*/
        scan_idx = -1;
        do
        {
            ret = rtk_l2_nextValidMcastAddr_get(unit, &scan_idx, & result_entry);
            if(ret == RT_ERR_OK)
                entryNums++;
        }while(ret == RT_ERR_OK);

        RT_TEST_IS_EQUAL_INT("rtk_l2_nextValidAddr_get dynamic entrys(unit %u)"
                                    , entryNums, 6, unit);
        /* Clear all test entry*/
        for (randIdx = 0; randIdx < sizeof(l2_mcastEntry_array)/sizeof(rtk_l2_mcastAddr_t); randIdx++)
        {
            osal_memcpy(&entry, &l2_mcastEntry_array[randIdx], sizeof(rtk_l2_mcastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_mcastAddr_del(unit, entry.rvid, &entry.mac);

            RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_del Failed(unit %u)"
                                    , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_getNext_test*/

int32 dal_l2_ipMcastAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit,LOOKUP_ON_DIP_AND_FVID);

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand() & BITMASK_32B;



            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;

                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ipMcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_test*/

int32 dal_l2_ipMcastAddr_test1(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit,LOOKUP_ON_DIP_AND_FVID);

        ret = rtk_l2_hashAlgo_set(unit, 1);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand() & BITMASK_32B;

            entry.portmask.bits[0] = ut_rand() & BITMASK_29B;
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;

                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ipMcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_test*/

int32 dal_l2_ipMcastAddr_del_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit,LOOKUP_ON_DIP_AND_FVID);


        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand();
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);


            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;

                ret = rtk_l2_ipMcastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);

                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_del_test*/

int32 dal_l2_ipMcastAddr_set_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    _dal_macAddr_clearAll(unit);

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit, LOOKUP_ON_DIP_AND_FVID);

        for (randIdx = 0; randIdx < 50*L2_RANDOM_RUN_TIMES; randIdx++)

        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand();

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;

                ret = rtk_l2_ipMcastAddr_set(unit, (rtk_l2_ipMcastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                 , ret, expect_result, unit);

                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ipMcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_set_test*/

int32 dal_l2_ipMcastAddr_getNext_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    _dal_macAddr_clearAll(unit);

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;
        int32 scan_idx;
        uint32 entryNums = 0;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit, LOOKUP_ON_DIP_AND_FVID);

        for (randIdx = 0; randIdx < sizeof(l2_ipMcastEntry_array)/sizeof(rtk_l2_ipMcastAddr_t); randIdx++)
        {
            osal_memcpy(&entry, &l2_ipMcastEntry_array[randIdx], sizeof(rtk_l2_ipMcastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_add Failed(unit %u)"
                                    , ret, RT_ERR_OK, unit);
         }

        /*from start*/
        scan_idx = -1;
        do
        {
            ret = rtk_l2_nextValidIpMcastAddr_get(unit, &scan_idx, & result_entry);
            if(ret == RT_ERR_OK)
                entryNums++;
        }while(ret == RT_ERR_OK);

        RT_TEST_IS_EQUAL_INT("rtk_l2_nextValidAddr_get dynamic entrys(unit %u)"
                                    , entryNums, 6, unit);

        /*Clear L2 entry*/
        for (randIdx = 0; randIdx < sizeof(l2_ipMcastEntry_array)/sizeof(rtk_l2_ipMcastAddr_t); randIdx++)
        {
            osal_memcpy(&entry, &l2_ipMcastEntry_array[randIdx], sizeof(rtk_l2_ipMcastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_ipMcastAddr_del(unit, entry.sip, entry.dip, entry.rvid);

            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_del Failed(unit %u)"
                               , ret, RT_ERR_OK, unit);
         }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_getNext_test*/

static int32 _dal_macAddr_clearAll(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;
    int32   scan_idx;
    rtk_vlan_t  vid;
    rtk_mac_t   mac;
    rtk_l2_mcastAddr_t m_data;
    ipaddr_t    dip, sip;
    rtk_l2_ipMcastAddr_t ip_data;

    /* Delete all unicast mac address */
    rtk_l2_addr_delAll(unit, 1 /* include static mac */);

    scan_idx = -1;
    memset(&m_data, 0, sizeof(rtk_l2_mcastAddr_t));
    while(1)
    {
        ret = rtk_l2_nextValidMcastAddr_get(unit, &scan_idx, &m_data);

        if (ret != RT_ERR_OK)
            break;

        vid = m_data.rvid;
        memcpy(&mac, &(m_data.mac), sizeof(rtk_mac_t));
        rtk_l2_mcastAddr_del(unit, vid, &mac);
    }

    scan_idx = -1;
    memset(&ip_data, 0, sizeof(rtk_l2_ipMcastAddr_t));
    while(1)
    {
        ret = rtk_l2_nextValidIpMcastAddr_get(unit, &scan_idx, &ip_data);

        if (ret != RT_ERR_OK)
            break;

        vid = ip_data.rvid;
        dip = ip_data.dip;
        sip = ip_data.sip;
        rtk_l2_ipMcastAddr_del(unit, sip, dip, vid);
    }

    return RT_ERR_OK;
} /* end of _dal_macAddr_clearAll */

int32 dal_l2_limitLearningCnt_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    uint32  count_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_limitLearningCnt_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_limitLearningCnt_get(unit, &count_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  count[UNITTEST_MAX_TEST_VALUE];
        int32  count_result[UNITTEST_MAX_TEST_VALUE];
        int32  count_index;
        int32  count_last;
        int32  result_count;

        UNITTEST_TEST_VALUE_ASSIGN(HAL_L2_LEARN_LIMIT_CNT_MAX(unit), 0, count, count_result, count_last);

        for (count_index = 0; count_index <= count_last; count_index++)
        {
            inter_result2 = count_result[count_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_limitLearningCnt_set(unit, count[count_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                                , ret, expect_result, unit, count[count_index]);

                ret = rtk_l2_limitLearningCnt_get(unit, (uint32*)&result_count);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_get (unit %u, count %u)"
                                 , ret, expect_result, unit, count[count_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set/get value unequal (unit %u, count %u)"
                                 , result_count, count[count_index], unit, count[count_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                                  , ret, RT_ERR_OK, unit, count[count_index]);
            }
        }

        ret = rtk_l2_limitLearningCnt_get(unit, (uint32*)&result_count);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                            , ret, expect_result, unit, count[count_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                            , ret, RT_ERR_OK, unit, count[count_index]);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_port_t  count;
        rtk_port_t  count_max;
        uint32  result_count;

        G_VALUE(HAL_L2_LEARN_LIMIT_CNT_MAX(unit), count_max);

        for (L_VALUE(TEST_MAC_CNT_MIN, count); count <= count_max; count++)
        {
            ASSIGN_EXPECT_RESULT(count, TEST_MAC_CNT_MIN, HAL_L2_LEARN_LIMIT_CNT_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_limitLearningCnt_set(unit, count);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                                , ret, expect_result, unit, count);
                ret = rtk_l2_limitLearningCnt_get(unit, (uint32*)&result_count);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_get (unit %u, count %u)"
                                 , ret, expect_result, unit, count);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCnt_set/get value unequal (unit %u, count %u)"
                                 , result_count, count, unit, count);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCnt_set (unit %u, count %u)"
                                    , ret, RT_ERR_OK, unit, count);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_limitLearningCnt_test*/

int32 dal_l2_limitLearningCntAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_limitLearnCntAction_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_limitLearningCntAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_limitLearningCntAction_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_l2_limitLearnCntAction_t  result_action;

       UNITTEST_TEST_VALUE_ASSIGN(TEST_LIMIT_ACT_ID_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_limitLearningCntAction_set(unit, (rtk_l2_limitLearnCntAction_t)action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_l2_limitLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_l2_limitLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                            , ret, expect_result, unit, action[action_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                                , ret, RT_ERR_OK, unit, action[action_index]);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_limitLearnCntAction_t  action;
        rtk_l2_limitLearnCntAction_t  action_max;
        rtk_l2_limitLearnCntAction_t  result_action;

        G_VALUE(TEST_ACT_ID_MAX(unit), action_max);

        for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_LIMIT_ACT_ID_MIN, TEST_LIMIT_ACT_ID_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_limitLearningCntAction_set(unit, (rtk_l2_limitLearnCntAction_t)action);
            osal_printf("expect_result:%d action:%d \n", expect_result, action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_l2_limitLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_limitLearningCntAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_limitLearningCntAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_limitLearningCntAction_test*/

int32 dal_l2_fidLearningCntAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_limitLearnCntAction_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_fidLearningCntAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_fidLearningCntAction_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_l2_limitLearnCntAction_t  result_action;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_LIMIT_ACT_ID_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_fidLearningCntAction_set(unit, (rtk_l2_limitLearnCntAction_t)action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_l2_fidLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_l2_fidLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                            , ret, expect_result, unit, action[action_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                                , ret, RT_ERR_OK, unit, action[action_index]);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_limitLearnCntAction_t  action;
        rtk_l2_limitLearnCntAction_t  action_max;
        rtk_l2_limitLearnCntAction_t  result_action;

        G_VALUE(TEST_ACT_ID_MAX(unit), action_max);

        for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_LIMIT_ACT_ID_MIN, TEST_LIMIT_ACT_ID_MAX(unit), inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_fidLearningCntAction_set(unit, (rtk_l2_limitLearnCntAction_t)action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_l2_fidLearningCntAction_get(unit, (rtk_l2_limitLearnCntAction_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_fidLearningCntAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_fidLearningCntAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_fidLearningCntAction_test*/

int32 dal_l2_portAgingEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    rtk_enable_t  enable;
    rtk_enable_t  enable_max;
    rtk_port_t  port;
    rtk_port_t  port_max;
    rtk_enable_t  result_enable;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portAgingEnable_get(unit, 0, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portAgingEnable_set(unit, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }



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
            ret = rtk_l2_portAgingEnable_set(unit, port, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_portAgingEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_l2_portAgingEnable_get(unit, port, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_portAgingEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_portAgingEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portAgingEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    /* Restore */
//    rtk_l2_portAgingEnable_set(unit, enable_temp);
    return RT_ERR_OK;
}   /*end of dal_l2_portAgingEnable_test*/

int32 dal_l2_zeroSALearningEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    rtk_enable_t  enable;
    rtk_enable_t  enable_max;
    rtk_enable_t  result_enable;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_zeroSALearningEnable_get(unit, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_zeroSALearningEnable_set(unit, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

    for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
    {
        ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        ret = rtk_l2_zeroSALearningEnable_set(unit, enable);
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_zeroSALearningEnable_set (unit %u, enable %u)"
                         , ret, expect_result, unit, enable);
            ret = rtk_l2_zeroSALearningEnable_get(unit, (rtk_enable_t*)&result_enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_portAgingEnable_get (unit %u, enable %u)"
                             , ret, expect_result, unit, enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_zeroSALearningEnable_set/get value unequal (unit %u, enable %u)"
                             , result_enable, enable, unit, enable);

        }
        else
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_zeroSALearningEnable_set (unit %u, enable %u)"
                                , ret, RT_ERR_OK, unit, enable);
    }


    return RT_ERR_OK;
}   /*end of dal_l2_zeroSALearningEnable_test*/

int32 dal_l2_staticPortMoveAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_staticPortMoveAction_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_staticPortMoveAction_get(unit, 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        uint32  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_ACT_MAX(unit), TEST_ACT_ID_MIN, action, action_result, action_last);

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
                ret = rtk_l2_staticPortMoveAction_set(unit, port[port_index], (rtk_action_t)action[action_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port[port_index], action[action_index]);

                    ret = rtk_l2_staticPortMoveAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port[port_index], action[action_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action[action_index], unit, port[port_index], action[action_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], action[action_index]);
                }
            }

            ret = rtk_l2_staticPortMoveAction_get(unit, port[port_index], (rtk_action_t*)&result_action);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u)"
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
        G_VALUE(TEST_ACT_ID_MAX(unit), action_max);

        for (L_VALUE(TEST_PORT_ID_MIN(unit), port); port <= port_max; port++)
        {
            ASSIGN_EXPECT_RESULT(port, TEST_PORT_ID_MIN(unit), TEST_PORT_ID_MAX(unit), inter_result2);
            UNITTEST_TEST_EXISTED_PORT_CHECK(unit, port, inter_result2);

            for (L_VALUE(TEST_ACT_ID_MIN, action); action <= action_max; action++)
            {
                ASSIGN_EXPECT_RESULT(action, TEST_ACT_ID_MIN, TEST_ACT_MAX(unit), inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_staticPortMoveAction_set(unit, port, (rtk_action_t)action);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u, action %u)"
                                    , ret, expect_result, unit, port, action);
                    ret = rtk_l2_staticPortMoveAction_get(unit, port, (rtk_action_t*)&result_action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_get (unit %u, port %u, action %u)"
                                     , ret, expect_result, unit, port, action);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_staticPortMoveAction_set/get value unequal (unit %u, port %u, action %u)"
                                     , result_action, action, unit, port, action);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_staticPortMoveAction_set (unit %u, port %u, action %u)"
                                        , ret, RT_ERR_OK, unit, port, action);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_staticPortMoveAction_test*/

int32 dal_l2_legalPortMoveFlushAddrEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;
    rtk_enable_t  enable;
    rtk_enable_t  enable_max;
    rtk_port_t  port;
    rtk_port_t  port_max;
    rtk_enable_t  result_enable;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_legalPortMoveFlushAddrEnable_get(unit, 0, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_legalPortMoveFlushAddrEnable_set(unit, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

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
            ret = rtk_l2_legalPortMoveFlushAddrEnable_set(unit, port, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveFlushAddrEnable_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_l2_legalPortMoveFlushAddrEnable_get(unit, port, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveFlushAddrEnable_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_legalPortMoveFlushAddrEnable_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_legalPortMoveFlushAddrEnable_set (unit %u, enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }


    return RT_ERR_OK;
}   /*end of dal_l2_legalPortMoveFlushAddrEnable_test*/

int32 dal_l2_mcastFwdPortmask_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_portmask_t  testPortmask;
    uint32          testCrossVlan;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastFwdPortmask_get(unit, TEST_MC_FWD_TBL_ENTRY_MIN, &testPortmask, &testCrossVlan)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastFwdPortmask_set(unit, TEST_MC_FWD_TBL_ENTRY_MIN, 0, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  mcEntry[UNITTEST_MAX_TEST_VALUE];
        int32  mcEntry_result[UNITTEST_MAX_TEST_VALUE];
        int32  mcEntry_index;
        int32  mcEntry_last;

        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;

        int32 randIdx;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_MC_FWD_TBL_ENTRY_MAX(unit), TEST_MC_FWD_TBL_ENTRY_MIN, mcEntry, mcEntry_result, mcEntry_last);

        for (mcEntry_index = 0; mcEntry_index < mcEntry_last; mcEntry_index++)
        {
            inter_result2 = mcEntry_result[mcEntry_index];
            for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
            {
            //  osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));

                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_mcastFwdPortmask_set(unit, mcEntry[mcEntry_index], &entry.portmask, 0);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdPortmask_set (unit %u)"
                                    , ret, expect_result, unit);

                    osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                    ret = rtk_l2_mcastFwdPortmask_get(unit, mcEntry[mcEntry_index], &result_entry.portmask, 0);

                    RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdPortmask_get (unit %u)"
                                     , ret, expect_result, unit);

                    /*Check Entry Content*/
                    ret = osal_memcmp(&result_entry.portmask, &entry.portmask, sizeof(rtk_portmask_t));

                    RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdPortmask_set/get value unequal (unit %u)"
                                , ret, 0, unit);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastFwdPortmask_set (unit %u)"
                                        , ret, RT_ERR_OK, unit);
                }
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastFwdPortmask_test*/

int32 dal_l2_cpuMacAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_mac_t       testmac;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_cpuMacAddr_del(unit, 0, &testmac)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_cpuMacAddr_add(unit, 0, &testmac)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        uint32  vlan[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_result[UNITTEST_MAX_TEST_VALUE];
        int32  vlan_index;
        int32  vlan_last;

        rtk_l2_mcastAddr_t  entry;

        int32 randIdx;

       UNITTEST_TEST_VALUE_ASSIGN(TEST_VLAN_ID_MAX, TEST_VLAN_ID_MIN, vlan, vlan_result, vlan_last);

        for (vlan_index = 0; vlan_index < vlan_last; vlan_index++)
        {
            inter_result2 = vlan_result[vlan_index];
            for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
            {
                osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
                entry.mac.octet[0] = ut_rand()&0xfe;
                entry.mac.octet[1] = ut_rand()&0xff;
                entry.mac.octet[2] = ut_rand()&0xff;
                entry.mac.octet[3] = ut_rand()&0xff;
                entry.mac.octet[4] = ut_rand()&0xff;
                entry.mac.octet[5] = ut_rand()&0xff;

                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result =  (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_l2_cpuMacAddr_add(unit, vlan[vlan_index], &entry.mac);

                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_cpuMacAddr_add (unit %u, vlan %u)"
                                    , ret, expect_result, unit, vlan[vlan_index]);

                    ret = rtk_l2_cpuMacAddr_del(unit, vlan[vlan_index], &entry.mac);


                    RT_TEST_IS_EQUAL_INT("rtk_l2_cpuMacAddr_del (unit %u, vlan %u)"
                                     , ret, expect_result, unit, vlan[vlan_index]);

                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_cpuMacAddr_add (unit %u, vlan %u)"
                                        , ret, RT_ERR_OK, unit, vlan[vlan_index]);
                }
            }
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_cpuMacAddr_test*/

int32 dal_l2_ipMcastAddrChkEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    rtk_enable_t  enable;
    rtk_enable_t  enable_max;
    rtk_enable_t  result_enable;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddrChkEnable_get(unit, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddrChkEnable_set(unit, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

    for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
    {
        ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        ret = rtk_l2_ipMcastAddrChkEnable_set(unit, enable);
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddrChkEnable_set (unit %u, enable %u)"
                         , ret, expect_result, unit, enable);
            ret = rtk_l2_ipMcastAddrChkEnable_get(unit, (rtk_enable_t*)&result_enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddrChkEnable_get (unit %u, enable %u)"
                             , ret, expect_result, unit, enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddrChkEnable_set/get value unequal (unit %u, enable %u)"
                             , result_enable, enable, unit, enable);

        }
        else
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddrChkEnable_set (unit %u, enable %u)"
                                , ret, RT_ERR_OK, unit, enable);
    }


    return RT_ERR_OK;
}   /*end of dal_l2_ipMcastAddrChkEnable_test*/

int32 dal_l2_ipMcstFidVidCompareEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;
    rtk_enable_t  enable;
    rtk_enable_t  enable_max;
    rtk_enable_t  result_enable;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcstFidVidCompareEnable_get(unit, &enable_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcstFidVidCompareEnable_set(unit, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

    for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
    {
        ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result2);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
        ret = rtk_l2_ipMcstFidVidCompareEnable_set(unit, enable);
        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_set (unit %u, enable %u)"
                         , ret, expect_result, unit, enable);
            ret = rtk_l2_ipMcstFidVidCompareEnable_get(unit, (rtk_enable_t*)&result_enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_get (unit %u, enable %u)"
                             , ret, expect_result, unit, enable);
            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_set/get value unequal (unit %u, enable %u)"
                             , result_enable, enable, unit, enable);

        }
        else
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_set (unit %u, enable %u)"
                                , ret, RT_ERR_OK, unit, enable);
    }

    return RT_ERR_OK;
}   /*end of dal_l2_ipMcstFidVidCompareEnable_test*/

/*Added for Debug only, will remove later*/
int32 dal_l2_testPrint_rtk_l2_mcastAddr(rtk_l2_mcastAddr_t *test)
{
    uint8   pm_str[RTK_PORTMASK_PRINT_STRING_LEN];

    osal_printf("\nrvid = %u",test->rvid);
    osal_printf("\nmac = %02x:%02x:%02x:%02x:%02x:%02x",test->mac.octet[0],test->mac.octet[1],test->mac.octet[2],test->mac.octet[3],test->mac.octet[4],test->mac.octet[5]);
    RTK_PORTMASK_SPRINTF(pm_str, test->portmask);
    osal_printf("\nportMask = %s",pm_str);
    osal_printf("\nfwdIndex = %u",test->fwdIndex);
    return RT_ERR_OK;
}


int32 dal_l2_mcastAddr_addByIndex_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    action_temp.rvid = 0x5000;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_addByIndex(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;

        int32 randIdx;

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            entry.fwdIndex = ut_rand() & BITMASK_12B;
            if(HWP_8380_30_FAMILY(unit))
            {
                entry.fwdIndex = ut_rand() & BITMASK_9B;
            }

            ret = rtk_l2_mcastAddr_addByIndex(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_addByIndex (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy((rtk_mac_t *)&result_entry.mac, (rtk_mac_t *)&entry.mac, sizeof(rtk_mac_t));


                result_entry.fwdIndex = entry.fwdIndex;
                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u), entry.fwdIndex = %u, randIdx = %u"
                               , ret, RT_ERR_OK, unit, entry.fwdIndex, randIdx);


                ret = rtk_l2_mcastAddr_del(unit, entry.rvid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_del (unit %u)"
                                 , ret, RT_ERR_OK, unit);

                result_entry.fwdIndex = entry.fwdIndex;
                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_addByIndex (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

    }

    return RT_ERR_OK;

}  /*end of dal_l2_mcastAddr_addByIndex_test*/

int32 dal_l2_ipMcastAddr_addByIndex_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_addByIndex(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                        , ret, RT_ERR_OK, unit,LOOKUP_ON_DIP_AND_FVID);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand();

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            entry.fwdIndex = ut_rand() & BITMASK_12B;
            if(HWP_8380_30_FAMILY(unit))
            {
                entry.fwdIndex = ut_rand() & BITMASK_9B;
            }
            ret = rtk_l2_ipMcastAddr_addByIndex(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_addByIndex (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;

                ret = rtk_l2_ipMcastAddr_del(unit, entry.sip, entry.dip, entry.rvid);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);

                result_entry.fwdIndex = entry.fwdIndex;
                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_addByIndex (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_addByIndex_test*/

int32 dal_l2_ip6mcMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipmcMode_t  action_temp = 0;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_get(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        uint32  mode[UNITTEST_MAX_TEST_VALUE];
        int32  mode_result[UNITTEST_MAX_TEST_VALUE];
        int32  mode_index;
        int32  mode_last;

        rtk_l2_ipmcMode_t  result_mode;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_LOOKUP_ON_ACT_MAX, TEST_LOOKUP_ON_ACT_MIN, mode, mode_result, mode_last);

        for (mode_index = 0; mode_index <= mode_last; mode_index++)
        {
            inter_result2 = mode_result[mode_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_ip6mcMode_set(unit, (rtk_l2_ipmcMode_t)mode[mode_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);

                ret = rtk_l2_ip6mcMode_get(unit, (rtk_l2_ipmcMode_t*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode[mode_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode[mode_index], unit, mode[mode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
        }

            ret = rtk_l2_ip6mcMode_get(unit, (rtk_l2_ipmcMode_t*)&result_mode);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode[mode_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode[mode_index]);
            }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_ipmcMode_t  mode;
        rtk_l2_ipmcMode_t  mode_max;
        rtk_l2_ipmcMode_t  result_mode;

        G_VALUE(TEST_LOOKUP_ON_ACT_MIN, mode_max);

        for (L_VALUE(TEST_LOOKUP_ON_ACT_MIN, mode); mode <= mode_max; mode++)
        {
            ASSIGN_EXPECT_RESULT(mode, TEST_LOOKUP_ON_ACT_MIN, TEST_LOOKUP_ON_ACT_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_ip6mcMode_set(unit, (rtk_l2_ipmcMode_t)mode);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                , ret, expect_result, unit, mode);
                ret = rtk_l2_ip6mcMode_get(unit, (rtk_l2_ipmcMode_t*)&result_mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_get (unit %u, mode %u)"
                                 , ret, expect_result, unit, mode);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6mcMode_set/get value unequal (unit %u, mode %u)"
                                 , result_mode, mode, unit, mode);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6mcMode_set (unit %u, mode %u)"
                                    , ret, RT_ERR_OK, unit, mode);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6mcMode_test*/

int32 dal_l2_ip6CareByte_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_get(unit, caretype_temp, &carebyte_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_l2_ip6_careByte_type_t  careType[UNITTEST_MAX_TEST_VALUE];
        int32  careType_result[UNITTEST_MAX_TEST_VALUE];
        int32  careType_index;
        int32  careType_last;

        uint32 result_careType;
        uint32 config_careByte;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_CARE_BYTE_TYPE_MAX, TEST_CARE_BYTE_TYPE_MIN, careType, careType_result, careType_last);

        for (careType_index = 0; careType_index <= careType_last; careType_index++)
        {
            inter_result2 = careType_result[careType_index];

            config_careByte = ut_rand() & BITMASK_16B;

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_ip6CareByte_set(unit, careType[careType_index], config_careByte);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_set (unit %u, careType %u)"
                                , ret, expect_result, unit, careType[careType_index]);

                ret = rtk_l2_ip6CareByte_get(unit, (rtk_l2_ip6_careByte_type_t)careType[careType_index],(uint32*)&result_careType);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_get (unit %u, careType %u)"
                                 , ret, expect_result, unit, careType[careType_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_set/get value unequal (unit %u, careType %u)"
                                 , result_careType, config_careByte, unit, careType[careType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6CareByte_set (unit %u, careType %u)"
                                  , ret, RT_ERR_OK, unit, careType[careType_index]);

                ret = rtk_l2_ip6CareByte_get(unit, (rtk_l2_ip6_careByte_type_t)careType[careType_index],(uint32*)&result_careType);
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6CareByte_get (unit %u, careType %u)"
                                , ret, RT_ERR_OK, unit, careType[careType_index]);

            }
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_l2_ip6_careByte_type_t  careType;
        rtk_l2_ip6_careByte_type_t  careType_max;
        uint32  result_care;
        uint32  config_care;

        G_VALUE(TEST_CARE_BYTE_TYPE_MAX, careType_max);

        for (L_VALUE(TEST_CARE_BYTE_TYPE_MIN, careType); careType <= careType_max; careType++)
        {
            ASSIGN_EXPECT_RESULT(careType, TEST_CARE_BYTE_TYPE_MIN, TEST_CARE_BYTE_TYPE_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            config_care = ut_rand() & BITMASK_16B;
            ret = rtk_l2_ip6CareByte_set(unit, (rtk_l2_ip6_careByte_type_t)careType, (uint32)config_care);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_set (unit %u, careType %u)"
                                , ret, expect_result, unit, careType);
                ret = rtk_l2_ip6CareByte_get(unit, (rtk_l2_ip6_careByte_type_t)careType,(uint32*)&result_care);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_get (unit %u, careType %u)"
                                 , ret, expect_result, unit, careType);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6CareByte_set/get value unequal (unit %u, careType %u)"
                                 , result_care, config_care, unit, careType);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6CareByte_set (unit %u, careType %u)"
                                    , ret, RT_ERR_OK, unit, careType);
        }

    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6CareByte_test*/

int32 dal_l2_ip6McastAddr_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_FVID);
    ret = rtk_l2_ip6CareByte_set(unit, L2_DIP_HASH_CARE_BYTE, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, L2_SIP_HASH_CARE_BYTE, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_set(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        int32  fill_ipIndex;

        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;

        int32 randIdx;


        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
            entry.rvid = ut_rand()% (0x1000);

            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;

                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);

                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_test*/

int32 dal_l2_ip6McastAddr_del_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_FVID);
    ret = rtk_l2_ip6CareByte_set(unit, 0, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, 1, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;

        int32  fill_ipIndex;

        int32 randIdx;

       for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);


            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);

                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);

                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_del_test*/

int32 dal_l2_ip6McastAddr_set_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_FVID);
    ret = rtk_l2_ip6CareByte_set(unit, 0, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, 1, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    _dal_macAddr_clearAll(unit);

    {
        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;

        int32  fill_ipIndex;

        int32 randIdx;

        for (randIdx = 0; randIdx < 50*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);

                ret = rtk_l2_ip6McastAddr_set(unit, (rtk_l2_ip6McastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                 , ret, expect_result, unit);

                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get/get value unequal (unit %u)"
                            , ret, 0, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_set_test*/

int32 dal_l2_ip6McastAddr_addByIndex_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_SIP);
    ret = rtk_l2_ip6CareByte_set(unit, 0, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, 1, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_addByIndex(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;

        int32  fill_ipIndex;
        int32 randIdx;

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            entry.fwdIndex = ut_rand() & BITMASK_12B;
            if(HWP_8380_30_FAMILY(unit))
            {
                entry.fwdIndex = ut_rand() & BITMASK_9B;
            }
            ret = rtk_l2_ip6McastAddr_addByIndex(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);

                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);
                result_entry.fwdIndex = entry.fwdIndex;
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_addByIndex_test*/

int32 dal_l2_ip6McastAddr_getNext_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    _dal_macAddr_clearAll(unit);

    {
        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;


        int32 scan_idx;
        int32 randIdx;
        uint32 entryNums = 0;
        /*Excute some basic initial*/
        rtk_l2_ip6mcMode_set(unit, LOOKUP_ON_DIP_AND_SIP);
        ret = rtk_l2_ip6CareByte_set(unit, L2_DIP_HASH_CARE_BYTE, 0x3210);
        ret = rtk_l2_ip6CareByte_set(unit, L2_SIP_HASH_CARE_BYTE, 0x5210);

        for (randIdx = 0; randIdx < (sizeof(l2_ip6McastEntry_array)/sizeof(rtk_l2_ip6McastAddr_t)); randIdx++)
        {
            osal_memcpy(&entry, &l2_ip6McastEntry_array[randIdx], sizeof(rtk_l2_ip6McastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_add Failed(unit %u)"
                                    , ret, RT_ERR_OK, unit);
         }

        /*from start*/
        scan_idx = -1;
        do
        {
            ret = rtk_l2_nextValidIp6McastAddr_get(unit, &scan_idx, & result_entry);
            if(ret == RT_ERR_OK)
                entryNums++;
        }while(ret == RT_ERR_OK);

        RT_TEST_IS_EQUAL_INT("rtk_l2_nextValidAddr_get dynamic entrys(unit %u)"
                                    , entryNums, (sizeof(l2_ip6McastEntry_array)/sizeof(rtk_l2_ip6McastAddr_t)), unit);

        for (randIdx = 0; randIdx < (sizeof(l2_ip6McastEntry_array)/sizeof(rtk_l2_ip6McastAddr_t)); randIdx++)
        {
            osal_memcpy(&entry, &l2_ip6McastEntry_array[randIdx], sizeof(rtk_l2_ip6McastAddr_t));
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

            ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, entry.rvid);

            RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_add Failed(unit %u)"
                            , ret, RT_ERR_OK, unit);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_getNext_test*/

int32 dal_l2_portLookupMissAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int8  inter_result4 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLookupMissAction_set(unit, TEST_PORT_ID_MIN(unit), 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portLookupMissAction_get(unit, TEST_PORT_ID_MIN(unit), 0, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_port_t  port[UNITTEST_MAX_TEST_VALUE];
        int32  port_result[UNITTEST_MAX_TEST_VALUE];
        int32  port_index;
        int32  port_last;

        rtk_l2_lookupMissType_t  type[UNITTEST_MAX_TEST_VALUE];
        int32  type_result[UNITTEST_MAX_TEST_VALUE];
        int32  type_index;
        int32  type_last;


        rtk_action_t  action[UNITTEST_MAX_TEST_VALUE];
        int32  action_result[UNITTEST_MAX_TEST_VALUE];
        int32  action_index;
        int32  action_last;

        rtk_action_t  result_action;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORTLKMISS_TYPE_MAX, TEST_PORTLKMISS_TYPE_MIN, type, type_result, type_last);
        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORTLKMISS_ACT_MAX, TEST_PORTLKMISS_ACT_MIN, action, action_result, action_last);

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

                    if((type[type_index] != DLF_TYPE_BCAST)&&(HWP_9300_FAMILY_ID(unit)||HWP_9310_FAMILY_ID(unit)))
                        expect_result = RT_ERR_FAILED;
                    if((action[action_index] > ACTION_COPY2CPU) && (HWP_8390_50_FAMILY(unit)||HWP_8380_30_FAMILY(unit)))
                        expect_result = RT_ERR_FAILED;

                    ret = rtk_l2_portLookupMissAction_set(unit, port[port_index], type[type_index],(rtk_action_t)action[action_index]);
                    if (RT_ERR_OK == expect_result)
                    {
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portLookupMissAction_set (unit %u, port %u, type %u, action %u)"
                                        , ret, expect_result, unit, port[port_index], type[type_index],action[action_index]);

                        ret = rtk_l2_portLookupMissAction_get(unit, port[port_index], type[type_index], (rtk_action_t*)&result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portLookupMissAction_get (unit %u, port %u, type %u, action %u)"
                                         , ret, expect_result, unit, port[port_index], type[type_index], result_action);
                        RT_TEST_IS_EQUAL_INT("rtk_l2_portLookupMissAction_set/get value unequal (unit %u, port %u, type %u, action %u)"
                                         , result_action, action[action_index], unit, port[port_index], type[type_index], action[action_index]);
                    }
                    else
                    {
                        RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portLookupMissAction_set (unit %u, port %u, type %u, action %u)"
                                            , ret, RT_ERR_OK, unit, port[port_index], type[type_index], action[action_index]);
                    }
                }
            }
            ret = rtk_l2_portLookupMissAction_get(unit, port[port_index], type[type_index], (rtk_action_t*)&result_action);
            expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;

            if((type[type_index] != DLF_TYPE_BCAST)&&(HWP_9300_FAMILY_ID(unit)||HWP_9310_FAMILY_ID(unit)))
                expect_result = RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_portLookupMissAction_set (unit %u, port %u, type %u, action %u)"
                                , ret, expect_result, unit, port[port_index], type[type_index], action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portLookupMissAction_set (unit %u, port %u, type %u, action %u)"
                                    , ret, RT_ERR_OK, unit, port[port_index], type[type_index], action[action_index]);
            }
        }
    }
    return RT_ERR_OK;
}  /*end of dal_l2_portLookupMissAction_test*/

/**********************************************************/
/* rtk_l2_mcastFwdIndex_alloc() & rtk_l2_mcastFwdIndex_free() with */
/* index 0 test cases are included in                               */
/* rtk_l2_lookupMissFloodPortMask_setByIndex()                           */
/* So the test case cover index != 0 only                                       */
/**********************************************************/
int32 dal_l2_mcastFwdIndex_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    int32  expect_result = 1;


    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastFwdIndex_alloc(unit, 0))
            || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastFwdIndex_free(unit, 0)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        int32  index[UNITTEST_MAX_TEST_VALUE];
        int32  index_result[UNITTEST_MAX_TEST_VALUE];
        int32  index_index;
        int32  index_last;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_MC_FWD_TBL_ENTRY_MAX(unit), (TEST_MC_FWD_TBL_ENTRY_MIN+1), index, index_result, index_last);


        for (index_index = 0; index_index <= index_last; index_index++)
        {
            /*Free at first*/
            ret = rtk_l2_mcastFwdIndex_free(unit, index[index_index]);
        }
        for (index_index = 0; index_index <= index_last; index_index++)
        {
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (index_result[index_index])? RT_ERR_OK : RT_ERR_FAILED;

            if(0 == index[index_index])
                continue;

            ret = rtk_l2_mcastFwdIndex_alloc(unit, &index[index_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdIndex_alloc (unit %u, index %u)"
                                , ret, expect_result, unit, index[index_index]);

                ret = rtk_l2_mcastFwdIndex_alloc(unit, &index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdIndex_alloc (unit %u, index %u)"
                                , ret, RT_ERR_L2_MCAST_FWD_ENTRY_EXIST, unit, index[index_index]);

                ret = rtk_l2_mcastFwdIndex_free(unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdIndex_free (unit %u, index %u)"
                                , ret, expect_result, unit, index[index_index]);
                ret = rtk_l2_mcastFwdIndex_free(unit, index[index_index]);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastFwdIndex_free (unit %u, index %u)"
                                , ret, RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST, unit, index[index_index]);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastFwdIndex_alloc (unit %u, index %u)"
                                    , ret, RT_ERR_OK, unit,  index[index_index]);
            }
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_mcastFwdIndex_test*/

int32 dal_l2_addr_all_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ucastAddr_t  action_temp;
    rtk_mac_t mac_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_get(unit, &action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_add(unit, &action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_set(unit, &action_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_addr_del(unit, TEST_VLAN_ID_MIN, &mac_temp))
    )
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_ucastAddr_t  entry;
        rtk_l2_ucastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 100*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ucastAddr_t));
            entry.vid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;
            entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
            entry.state = 0;

            UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(entry.mac.octet[0] & BITMASK_1B)
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.vid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }
            /* To verify the add() API*/
            ret = rtk_l2_addr_add(unit, &entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u) port %u randIdx %u"
                                , ret, expect_result, unit, entry.port, randIdx);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                /* To verify the get() API for existed entry*/
                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, expect_result, unit);


                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
                entry.state = 0;
                UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
                /* To verify the set() API for existed entry*/
                ret = rtk_l2_addr_set(unit, (rtk_l2_ucastAddr_t*)&entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                     , ret, expect_result, unit);


                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));

                /* To verify the get() API for existed entry*/
                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                                 , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                /* To verify the del() API for existed entry*/
                ret = rtk_l2_addr_del(unit, entry.vid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_del (unit %u, vid %u)"
                                 , ret, RT_ERR_OK, unit, entry.vid);
                /* To verify the get() API for NON-existed entry*/
                ret = rtk_l2_addr_get(unit, (rtk_l2_ucastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                /* To verify the del() API for NON existed entry*/
                ret = rtk_l2_addr_del(unit, entry.vid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_del (unit %u, vid %u)"
                                 , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit, entry.vid);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ucastAddr_t));
                result_entry.vid = entry.vid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                entry.flags = ut_rand() & (RTK_L2_UCAST_FLAG_DA_BLOCK |
                                               RTK_L2_UCAST_FLAG_SA_BLOCK | RTK_L2_UCAST_FLAG_STATIC);
                entry.state = 0;

                UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, entry.port);
                /* To verify the set() API for NON existed entry*/
                ret = rtk_l2_addr_set(unit, (rtk_l2_ucastAddr_t*)&entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                     , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_addr_set (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_addr_all_test*/

int32 dal_l2_mcastAddr_all_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_mcastAddr_t  action_temp;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_mcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_l2_mcastAddr_t  entry;
        rtk_l2_mcastAddr_t  result_entry;
        uint8               entry_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];
        uint8               result_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];

        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_mcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.mac.octet[0] = ut_rand()&0xff;
            entry.mac.octet[1] = ut_rand()&0xff;
            entry.mac.octet[2] = ut_rand()&0xff;
            entry.mac.octet[3] = ut_rand()&0xff;
            entry.mac.octet[4] = ut_rand()&0xff;
            entry.mac.octet[5] = ut_rand()&0xff;
#if defined (CONFIG_SDK_RTL8380)
            if(HWP_8380_30_FAMILY(unit))
                entry.agg_vid = ut_rand()% (0x1000);
#endif
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if(!(entry.mac.octet[0] & BITMASK_1B))
            {
                expect_result = RT_ERR_FAILED;
            }
            if(entry.rvid > RTK_VLAN_ID_MAX)
            {
                expect_result = RT_ERR_FAILED;
            }

            ret = rtk_l2_mcastAddr_add(unit, (rtk_l2_mcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                /*To verify the get() for existed entry.*/
                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content for add()*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_mcastAddr_t));
                if (ret != 0)
                {
                    osal_printf("[%s:%d]entry.rvid=%d, result_entry.rvid=%d\n", __FUNCTION__, __LINE__, entry.rvid, result_entry.rvid);
                    osal_printf("[%s:%d]entry.mac=%02X-%02X-%02X-%02X-%02X-%02X, result_entry.mac=%02X-%02X-%02X-%02X-%02X-%02X\n", __FUNCTION__, __LINE__,
                        entry.mac.octet[0], entry.mac.octet[1], entry.mac.octet[2], entry.mac.octet[3], entry.mac.octet[4], entry.mac.octet[5],
                        result_entry.mac.octet[0], result_entry.mac.octet[1], result_entry.mac.octet[2], result_entry.mac.octet[3], result_entry.mac.octet[4], result_entry.mac.octet[5]);
                    RTK_PORTMASK_SPRINTF(entry_pm_str, entry.portmask);
                    RTK_PORTMASK_SPRINTF(result_pm_str, result_entry.portmask);
                    osal_printf("[%s:%d]entry.portmask.bits%s, result_entry.portmask.bits%s\n", __FUNCTION__, __LINE__, entry_pm_str, result_pm_str);
                    osal_printf("[%s:%d]entry.fwdIndex=%d, result_entry.fwdIndex=%d\n", __FUNCTION__, __LINE__, entry.fwdIndex, result_entry.fwdIndex);
                    osal_printf("[%s:%d]entry.l2_idx=%d, result_entry.l2_idx=%d\n", __FUNCTION__, __LINE__, entry.l2_idx, result_entry.l2_idx);
                    osal_printf("[%s:%d]entry.add_op_flags=%d, result_entry.add_op_flags=%d\n", __FUNCTION__, __LINE__, entry.add_op_flags, result_entry.add_op_flags);
#if defined (CONFIG_SDK_RTL8380)
                    if(HWP_8380_30_FAMILY(unit))
                        osal_printf("[%s:%d]entry.agg_vid=%d, result_entry.agg_vid=%d\n", __FUNCTION__, __LINE__, entry.agg_vid, result_entry.agg_vid);
#endif
                }

                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
#if defined (CONFIG_SDK_RTL8380)
                if(HWP_8380_30_FAMILY(unit))
                    entry.agg_vid = ut_rand()% (0x1000);
#endif
                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
                /*To verify the set() for existed entry.*/
                ret = rtk_l2_mcastAddr_set(unit, (rtk_l2_mcastAddr_t*)&entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                 , ret, expect_result, unit);


                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content for set()*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_mcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get/get value unequal (unit %u)"
                                , ret, 0, unit);

                /*To verify the del() for existed entry.*/
                ret = rtk_l2_mcastAddr_del(unit, entry.rvid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_del (unit %u)"
                                 , ret, RT_ERR_OK, unit);
                osal_memset(&result_entry, 0, sizeof(rtk_l2_mcastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(&result_entry.mac.octet[0], &entry.mac.octet[0], sizeof(rtk_mac_t));
                /*To verify the get() for NON existed entry.*/
                ret = rtk_l2_mcastAddr_get(unit, (rtk_l2_mcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);


                /*To verify the set() for NON existed entry.*/
                ret = rtk_l2_mcastAddr_set(unit, (rtk_l2_mcastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_set (unit %u)"
                                 , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);


                /*To verify the del() for NON existed entry.*/
                ret = rtk_l2_mcastAddr_del(unit, entry.rvid, &entry.mac);

                RT_TEST_IS_EQUAL_INT("rtk_l2_mcastAddr_del (unit %u)"
                                 , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);


            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_mcastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_mcastAddr_all_test*/

int32 dal_l2_ipMcastAddr_all_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t  action_temp;
    int32  expect_result = 1;

    ret = rtk_l2_ipmcMode_set(unit, LOOKUP_ON_DIP_AND_FVID);
    RT_TEST_IS_EQUAL_INT("rtk_l2_ipmcMode_set (unit %u, mode %u)"
                    , ret, RT_ERR_OK, unit,LOOKUP_ON_DIP_AND_FVID);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ipMcastAddr_get(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {

        rtk_l2_ipMcastAddr_t  entry;
        rtk_l2_ipMcastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
            entry.rvid = ut_rand()% (0x1000);
            entry.dip = ((ut_rand() & BITMASK_28B) | 0xE0000000);
            entry.sip = ut_rand() & BITMASK_32B;
            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ipMcastAddr_add(unit, (rtk_l2_ipMcastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;
                /* To verify the get() API for existed entry*/
                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ipMcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get/add value unequal (unit %u)"
                            , ret, 0, unit);

                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ipMcastAddr_t));
                result_entry.rvid = entry.rvid;
                result_entry.dip = entry.dip;
                result_entry.sip = entry.sip;
                /* To verify the set() API for existed entry*/
                ret = rtk_l2_ipMcastAddr_set(unit, (rtk_l2_ipMcastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                             , ret, expect_result, unit);

                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                /* To verify the get() API for existed entry*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ipMcastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get/set value unequal (unit %u)"
                            , ret, 0, unit);

                /* To verify the del() API for existed entry*/

                ret = rtk_l2_ipMcastAddr_del(unit, entry.sip, entry.dip, entry.rvid);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);

                /* To verify the get() API for NON existed entry*/
                ret = rtk_l2_ipMcastAddr_get(unit, (rtk_l2_ipMcastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

                /* To verify the set() API for NON existed entry*/
                ret = rtk_l2_ipMcastAddr_set(unit, (rtk_l2_ipMcastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_set (unit %u)"
                             , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);


                /* To verify the del() API for NON existed entry*/

                ret = rtk_l2_ipMcastAddr_del(unit, entry.sip, entry.dip, entry.rvid);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcastAddr_del (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ipMcastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

    }


    return RT_ERR_OK;
}  /*end of dal_l2_ipMcastAddr_all_test*/

int32 dal_l2_ip6McastAddr_DIPSIP_all_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;
    rtk_enable_t    old_state;
    uint8           entry_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];
    uint8           result_pm_str[RTK_PORTMASK_PRINT_STRING_LEN];

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_SIP);
    ret = rtk_l2_ip6CareByte_set(unit, L2_DIP_HASH_CARE_BYTE, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, L2_SIP_HASH_CARE_BYTE, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_set(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        int32  fill_ipIndex;

        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;

        int32 randIdx;

        ret = rtk_l2_ipMcstFidVidCompareEnable_get(unit, &old_state);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_get (enable %u)", ret, RT_ERR_OK, old_state);
        ret = rtk_l2_ipMcstFidVidCompareEnable_set(unit, DISABLED);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_set DISABLED failed!!", ret, RT_ERR_OK);

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
            entry.rvid = ut_rand()% (0x1000);

            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;

                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);

                /* to verify the get() for existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));
                if (ret != 0)
                {
                    osal_printf("[%s:%d]entry.rvid=%d, result_entry.rvid=%d\n", __FUNCTION__, __LINE__, entry.rvid, result_entry.rvid);
                    for (fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
                        osal_printf("[%s:%d]entry.dip.octet[%d]=0x%x, result_entry.dip.octet[%d]=0x%x\n", __FUNCTION__, __LINE__, fill_ipIndex, entry.dip.octet[fill_ipIndex], fill_ipIndex, result_entry.dip.octet[fill_ipIndex]);
                    for (fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
                        osal_printf("[%s:%d]entry.sip.octet[%d]=0x%x, result_entry.sip.octet[%d]=0x%x\n", __FUNCTION__, __LINE__, fill_ipIndex, entry.sip.octet[fill_ipIndex], fill_ipIndex, result_entry.sip.octet[fill_ipIndex]);
                    RTK_PORTMASK_SPRINTF(entry_pm_str, entry.portmask);
                    RTK_PORTMASK_SPRINTF(result_pm_str, result_entry.portmask);
                    osal_printf("[%s:%d]entry.portmask.bits%s, result_entry.portmask.bits%s\n", __FUNCTION__, __LINE__, entry_pm_str, result_pm_str);
                    osal_printf("[%s:%d]entry.fwdIndex=%d, result_entry.fwdIndex=%d\n", __FUNCTION__, __LINE__, entry.fwdIndex, result_entry.fwdIndex);
                    osal_printf("[%s:%d]entry.l2_idx=%d, result_entry.l2_idx=%d\n", __FUNCTION__, __LINE__, entry.l2_idx, result_entry.l2_idx);
                    osal_printf("[%s:%d]entry.add_op_flags=%d, result_entry.add_op_flags=%d\n", __FUNCTION__, __LINE__, entry.add_op_flags, result_entry.add_op_flags);
                }

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                entry.rvid = ut_rand()% (0x1000);
                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the set() for existed entry*/
                ret = rtk_l2_ip6McastAddr_set(unit, (rtk_l2_ip6McastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                 , ret, expect_result, unit);

                /* to verify the get() for existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set/get value unequal (unit %u)"
                            , result_entry.rvid, entry.rvid, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the del() for existed entry*/
                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);
                /* to verify the get() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = ut_rand()% (0x1000);
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the set() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_set(unit, (rtk_l2_ip6McastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                 , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the del() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                           , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

        ret = rtk_l2_ipMcstFidVidCompareEnable_set(unit, old_state);
        RT_TEST_IS_EQUAL_INT("rtk_l2_ipMcstFidVidCompareEnable_set (enable %u)", ret, RT_ERR_OK, old_state);
    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_DIPSIP_all_test*/

int32 dal_l2_ip6McastAddr_DIPFVID_all_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_l2_ip6McastAddr_t  action_temp;
    int32  expect_result = 1;

    rtk_l2_ip6_careByte_type_t caretype_temp = 0;
    uint32  carebyte_temp = 0;
    rtk_l2_ipmcMode_t  mode_temp = 0;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6mcMode_set(unit, mode_temp))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6CareByte_set(unit, caretype_temp,carebyte_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /*Excute some basic initial*/
    rtk_l2_ip6mcMode_set(unit,LOOKUP_ON_DIP_AND_FVID);
    ret = rtk_l2_ip6CareByte_set(unit, L2_DIP_HASH_CARE_BYTE, 0x3210);
    ret = rtk_l2_ip6CareByte_set(unit, L2_SIP_HASH_CARE_BYTE, 0x5210);

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_get(unit, &action_temp)) ||
        (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_ip6McastAddr_set(unit, &action_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        int32  fill_ipIndex;
        rtk_l2_ip6McastAddr_t  entry;
        rtk_l2_ip6McastAddr_t  result_entry;
        int32 randIdx;

        ret = rtk_l2_hashAlgo_set(unit, 0);

        for (randIdx = 0; randIdx < 30*L2_RANDOM_RUN_TIMES; randIdx++)
        {
            osal_memset(&entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
            entry.rvid = ut_rand()% (0x1000);

            for(fill_ipIndex = 0; fill_ipIndex < IPV6_ADDR_LEN; fill_ipIndex++)
            {
                if(0 != fill_ipIndex)
                    entry.dip.octet[fill_ipIndex] = (ut_rand() & BITMASK_8B);
                else
                    entry.dip.octet[fill_ipIndex] = (uint8)(0xFF);
                //entry.sip.octet[fill_ipIndex]= (ut_rand() & BITMASK_8B);
            }

            UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;

            ret = rtk_l2_ip6McastAddr_add(unit, (rtk_l2_ip6McastAddr_t *)&entry);

            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                , ret, expect_result, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
//                  osal_printf("vid = %u",result_entry.rvid);
//                  l2test_dump_octet(&result_entry.dip);
                /* to verify the get() for existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);

                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_add/get value unequal (unit %u)"
                            , ret, 0, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                UNITTEST_RANDOM_PORTMASK_ASSIGN(unit, randIdx, &entry.portmask);
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
//                  osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the set() for existed entry*/
                ret = rtk_l2_ip6McastAddr_set(unit, (rtk_l2_ip6McastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                 , ret, expect_result, unit);
                /* to verify the get() for existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                                 , ret, expect_result, unit);
                /*Check Entry Content*/
                ret = osal_memcmp(&result_entry, &entry, sizeof(rtk_l2_ip6McastAddr_t));
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set/get value unequal (unit %u)"
                            , result_entry.rvid, entry.rvid, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
                osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the del() for existed entry*/
                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                               , ret, RT_ERR_OK, unit);
                /* to verify the get() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_get(unit, (rtk_l2_ip6McastAddr_t*)&result_entry);

                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_get (unit %u)"
                               , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
//                  osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the set() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_set(unit, (rtk_l2_ip6McastAddr_t*)&entry);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_set (unit %u)"
                                 , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);
                osal_memset(&result_entry, 0, sizeof(rtk_l2_ip6McastAddr_t));
                result_entry.rvid = entry.rvid;
                osal_memcpy(result_entry.dip.octet, entry.dip.octet, IPV6_ADDR_LEN);
//                  osal_memcpy(result_entry.sip.octet, entry.sip.octet, IPV6_ADDR_LEN);
                /* to verify the del() for NON existed entry*/
                ret = rtk_l2_ip6McastAddr_del(unit, entry.sip, entry.dip, result_entry.rvid);
                RT_TEST_IS_EQUAL_INT("rtk_l2_ip6McastAddr_del (unit %u)"
                           , ret, RT_ERR_L2_ENTRY_NOTFOUND, unit);

            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_ip6McastAddr_add (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }

        }

    }

    return RT_ERR_OK;
}  /*end of dal_l2_ip6McastAddr_DIPFVID_all_test*/

int32 dal_l2_secureMacMode_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_secureMacMode_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_secureMacMode_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if (!IS_TEST_SCAN_MODE())
    {
        rtk_enable_t enable[UNITTEST_MAX_TEST_VALUE];
        int32  enable_result[UNITTEST_MAX_TEST_VALUE];
        int32  enable_index;
        int32  enable_last;

        int32  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_ENABLE_STATUS_MAX, TEST_ENABLE_STATUS_MIN, enable, enable_result, enable_last);


        for (enable_index = 0; enable_index <= enable_last; enable_index++)
        {
            inter_result2 = enable_result[enable_index];
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            ret = rtk_l2_secureMacMode_set(unit, enable[enable_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable[enable_index]);

                ret = rtk_l2_secureMacMode_get(unit, (uint32*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_get (unit %u,  enable %u)"
                                 , ret, expect_result, unit,  enable[enable_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable[enable_index], unit, enable[enable_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_secureMacMode_set (unit %u,  enable %u)"
                                    , ret, RT_ERR_OK, unit, enable[enable_index]);
            }
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
            ret = rtk_l2_secureMacMode_set(unit, enable);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_set (unit %u, enable %u)"
                                , ret, expect_result, unit, enable);
                ret = rtk_l2_secureMacMode_get(unit, (rtk_enable_t*)&result_enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_get (unit %u, enable %u)"
                                 , ret, expect_result, unit, enable);
                RT_TEST_IS_EQUAL_INT("rtk_l2_secureMacMode_set/get value unequal (unit %u, enable %u)"
                                 , result_enable, enable, unit, enable);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_secureMacMode_set (unit %u,enable %u)"
                                    , ret, RT_ERR_OK, unit, enable);
        }
    }

    return RT_ERR_OK;
}   /*end of dal_l2_secureMacMode_test*/

int32 dal_l2_portDynamicPortMoveForbidEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portDynamicPortMoveForbidEnable_set(unit, TEST_PORT_ID_MIN(unit), 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_portDynamicPortMoveForbidEnable_get(unit, TEST_PORT_ID_MIN(unit), &enable_temp)))
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

        rtk_enable_t  result_enable;


        UNITTEST_TEST_VALUE_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), port, port_result, port_last);
        UNITTEST_TEST_EXISTED_PORT_ASSIGN(TEST_PORT_ID_MAX(unit), TEST_PORT_ID_MIN(unit), unit, port[2]);
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
                ret = rtk_l2_portDynamicPortMoveForbidEnable_set(unit, port[port_index], enable[enable_index]);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port[port_index], enable[enable_index]);

                    ret = rtk_l2_portDynamicPortMoveForbidEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port[port_index], enable[enable_index]);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable[enable_index], unit, port[port_index], enable[enable_index]);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port[port_index], enable[enable_index]);
                }
            }

            ret = rtk_l2_portDynamicPortMoveForbidEnable_get(unit, port[port_index], (rtk_enable_t*)&result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u)"
                                , ret, expect_result, unit, port[port_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u)"
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
                ret = rtk_l2_portDynamicPortMoveForbidEnable_set(unit, port, enable);
                if (expect_result == RT_ERR_OK)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u, enable %u)"
                                    , ret, expect_result, unit, port, enable);
                    ret = rtk_l2_portDynamicPortMoveForbidEnable_get(unit, port, (rtk_enable_t*)&result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_get (unit %u, port %u, enable %u)"
                                     , ret, expect_result, unit, port, enable);
                    RT_TEST_IS_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set/get value unequal (unit %u, port %u, enable %u)"
                                     , result_enable, enable, unit, port, enable);

                }
                else
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_portDynamicPortMoveForbidEnable_set (unit %u, port %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, port, enable);
            }

        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_portDynamicPortMoveForbidEnable_test*/

int32 dal_l2_dynamicPortMoveForbidAction_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_action_t  action_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_dynamicPortMoveForbidAction_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_l2_dynamicPortMoveForbidAction_get(unit, &action_temp)))
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

        UNITTEST_TEST_VALUE_ASSIGN(TEST_DYNAMIC_PMOVE_FB_ACT_MAX, TEST_DYNAMIC_PMOVE_FB_ACT_MIN, action, action_result, action_last);

        for (action_index = 0; action_index <= action_last; action_index++)
        {
            inter_result2 = action_result[action_index];

            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;

            if ((HWP_8380_30_FAMILY(unit) || HWP_8390_50_FAMILY(unit)) && (action[action_index] > ACTION_TRAP2CPU))
                expect_result = RT_ERR_FAILED;
            ret = rtk_l2_dynamicPortMoveForbidAction_set(unit, (rtk_l2_limitLearnCntAction_t)action[action_index]);
            if (RT_ERR_OK == expect_result)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action[action_index]);

                ret = rtk_l2_dynamicPortMoveForbidAction_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action[action_index]);
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action[action_index], unit, action[action_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action[action_index]);
            }
        }

        ret = rtk_l2_dynamicPortMoveForbidAction_get(unit, (rtk_action_t*)&result_action);
        if(!(HWP_UNIT_VALID_LOCAL(unit)))
            expect_result = RT_ERR_FAILED;
        else
            expect_result = RT_ERR_OK;

        if (expect_result == RT_ERR_OK)
        {
            RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                            , ret, expect_result, unit, action[action_index]);
        }
        else
        {
            RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                                , ret, RT_ERR_OK, unit, action[action_index]);
        }
    }

    if (IS_TEST_SCAN_MODE())
    {
        rtk_action_t  action;
        rtk_action_t  action_max;
        rtk_action_t  result_action;
        G_VALUE(TEST_DYNAMIC_PMOVE_FB_ACT_MAX, action_max);

        for (L_VALUE(TEST_DYNAMIC_PMOVE_FB_ACT_MIN, action); action <= action_max; action++)
        {
            ASSIGN_EXPECT_RESULT(action, TEST_DYNAMIC_PMOVE_FB_ACT_MIN, TEST_DYNAMIC_PMOVE_FB_ACT_MAX, inter_result2);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if ((HWP_8380_30_FAMILY(unit) || HWP_8390_50_FAMILY(unit)) && (action > ACTION_TRAP2CPU))
                expect_result = RT_ERR_FAILED;
            ret = rtk_l2_dynamicPortMoveForbidAction_set(unit, (rtk_l2_limitLearnCntAction_t)action);
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                                , ret, expect_result, unit, action);
                ret = rtk_l2_dynamicPortMoveForbidAction_get(unit, (rtk_action_t*)&result_action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_get (unit %u, action %u)"
                                 , ret, expect_result, unit, action);
                RT_TEST_IS_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set/get value unequal (unit %u, action %u)"
                                 , result_action, action, unit, action);

            }
            else
                RT_TEST_IS_NOT_EQUAL_INT("rtk_l2_dynamicPortMoveForbidAction_set (unit %u, action %u)"
                                    , ret, RT_ERR_OK, unit, action);
        }
    }

    return RT_ERR_OK;
}  /*end of dal_l2_dynamicPortMoveForbidAction_test*/

