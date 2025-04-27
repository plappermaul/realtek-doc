/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 82366 $
 * $Date: 2017-09-21 13:19:23 +0800 (Thu, 21 Sep 2017) $
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
#include <drv/nic/nic.h>
#include <dal/dal_mgmt.h>
#include <rtk/stat.h>
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <common/unittest_util.h>
#include <common/unittest_def.h>

#define PKTBUF_ALLOC(size)    osal_alloc(size)
#define PKTBUF_FREE(pktbuf)   osal_free(pktbuf)

static int32 _nic_pauseOnPkt_send(uint32 unit, uint32 portmask);
static int32 _nic_pauseOffPkt_send(uint32 unit, uint32 portmask);

/* tagCntType */
#define TEST_TAGCNTTYPE_MIN    TAG_CNT_TYPE_RX
#define TEST_TAGCNTTYPE_MAX    (TAG_CNT_TYPE_END-1)

/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
static void rtl83xx_nic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);

static void
rtl83xx_nic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (pPacket == NULL)
    {
        goto _exit;
    }
    osal_free(pCookie);
    PKTBUF_FREE(pPacket);

_exit:
    return;
}
#endif

/* Define symbol used for test input */
int32 dal_stat_readPerformance_test(uint32 caseNo, uint32 unit)
{
    uint64  counter;
    uint32  value, i, j;
    int32   ret = RT_ERR_FAILED;
    osal_usecs_t    usec_before = 0, usec_after = 0;

    value = 100;
    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            ret = rtk_stat_port_get(unit, 0, IF_IN_OCTETS_INDEX, &counter);
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access Port Counter Number: %u\n", value);
    osal_printf("Access one port counter nanosecond (rtk_stat_port_get) = %u\n", (usec_after-usec_before)/value);

    return ret;
}

/* Define symbol used for test input */
int32 dal_stat_readPerformance2_test(uint32 caseNo, uint32 unit)
{
    uint64  counter_b, counter_a;
    uint32  value, i, j, yes = 0;
    int32   ret = RT_ERR_FAILED;
    osal_usecs_t    usec_before = 0, usec_after = 0;
    uint64  threshold = 0x2000;
    rtk_portmask_t portmask;

    value = 28;
    memset(&portmask, 0, sizeof(rtk_portmask_t));
    portmask.bits[0] = 0x0fffffff;
    ret = osal_time_usecs_get(&usec_before);
    for (j= 0; j<1000; j++)
    {
        //for (i = 0; i < value; i++)
        //{
        _nic_pauseOnPkt_send(unit, 0x0fffffff);
        //}
        osal_time_udelay(16);
        for (i = 0; i < value; i++)
        {
            ret = rtk_stat_port_get(unit, i, IF_IN_OCTETS_INDEX, &counter_b);
            ret = rtk_stat_port_get(unit, i, IF_IN_OCTETS_INDEX, &counter_a);
            if ((counter_a - counter_b) < threshold)
                yes++;
        }
        ret = phy_reg_portmask_set(unit, portmask, 0, 0, 0x1940);
        ret = phy_reg_portmask_set(unit, portmask, 0, 0, 0x1140);
        _nic_pauseOffPkt_send(unit, 0x0fffffff);
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Access Port Counter Number: %u\n", value);
    osal_printf("Access all port one-cycle nanosecond (rtk_stat_port_get) = %u\n", (usec_after-usec_before));

    return ret;
}

static int32 _nic_pauseOnPkt_send(uint32 unit, uint32 portmask)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint8 pkt1[] = { /* Pause frame */
        0x01, 0x80, 0xc2, 0x00, 0x00, 0x01, 0x00, 0x1A, 0x1B, 0x33, 0x44, 0x55, 0x88, 0x08, 0x00, 0x01,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        /* out of memory */
        return RT_ERR_FAILED;   /* Failed */
    }

    tx_buff = osal_alloc(1600);
    if (tx_buff == NULL)
    {
        PKTBUF_FREE(pPacket);
        return RT_ERR_FAILED;
    }

    {
        pPacket->as_txtag = 1;

        pPacket->tx_tag.dst_port_mask       = portmask; /* (0x1 << port); */
        pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
        pPacket->tx_tag.as_priority         = 1;
#ifdef CONFIG_SDK_FPGA_PLATFORM
        /* only 4 TX queues (0,1,2,7) on FPGA platform */
        pPacket->tx_tag.priority            = 7;
#else
        pPacket->tx_tag.priority            = 7;
#endif
        pPacket->tx_tag.l2_learning         = 0;
    }

    /* raw packet */
    pPacket->buf_id = (void *)NULL;
    pPacket->head = tx_buff;
    pPacket->data = tx_buff+8;
    pPacket->tail = tx_buff+8+64;
    pPacket->end = tx_buff+1600;
    pPacket->length = 60;
    pPacket->next = NULL;

    memcpy(pPacket->data, pkt1, 60);

    if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
    {

    }
    else
    {
        PKTBUF_FREE(pPacket);
        osal_free(tx_buff);
        return RT_ERR_FAILED;   /* Failed */
    }
#endif

    return RT_ERR_OK;   /* Success */
}

static int32 _nic_pauseOffPkt_send(uint32 unit, uint32 portmask)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint8 pkt1[] = { /* Pause frame */
        0x01, 0x80, 0xc2, 0x00, 0x00, 0x01, 0x00, 0x1A, 0x1B, 0x33, 0x44, 0x55, 0x88, 0x08, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        /* out of memory */
        return RT_ERR_FAILED;   /* Failed */
    }

    tx_buff = osal_alloc(1600);
    if (tx_buff == NULL)
    {
        PKTBUF_FREE(pPacket);
        return RT_ERR_FAILED;
    }

    {
        pPacket->as_txtag = 1;

        pPacket->tx_tag.dst_port_mask       = portmask; /* (0x1 << port); */
        pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
        pPacket->tx_tag.as_priority         = 1;
#if defined(CONFIG_SDK_FPGA_PLATFORM)
        /* only 4 TX queues (0,1,2,7) on FPGA platform */
        pPacket->tx_tag.priority            = 7;
#else
        pPacket->tx_tag.priority            = 7;
#endif
        pPacket->tx_tag.l2_learning         = 0;
    }

    /* raw packet */
    pPacket->buf_id = (void *)NULL;
    pPacket->head = tx_buff;
    pPacket->data = tx_buff+8;
    pPacket->tail = tx_buff+8+64;
    pPacket->end = tx_buff+1600;
    pPacket->length = 60;
    pPacket->next = NULL;

    memcpy(pPacket->data, pkt1, 60);

    if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
    {

    }
    else
    {
        PKTBUF_FREE(pPacket);
        osal_free(tx_buff);
        return RT_ERR_FAILED;   /* Failed */
    }
#endif

    return RT_ERR_OK;   /* Success */
}

int32 dal_stat_tagLenCntIncEnable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int8  inter_result3 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_stat_tagLenCntIncEnable_set(unit, 0, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_stat_tagLenCntIncEnable_get(unit, 0, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    {
        rtk_stat_tagCnt_type_t  tagCntType[UNITTEST_MAX_TEST_VALUE];
        int32  tagCntType_result[UNITTEST_MAX_TEST_VALUE];
        int32  tagCntType_index;
        int32  tagCntType_last;

        rtk_enable_t  enable;
        rtk_enable_t  enable_max;
        rtk_enable_t  result_enable;

        UNITTEST_TEST_VALUE_ASSIGN(TEST_TAGCNTTYPE_MAX, TEST_TAGCNTTYPE_MIN, tagCntType, tagCntType_result, tagCntType_last);
        G_VALUE(TEST_ENABLE_STATUS_MAX, enable_max);

        for (tagCntType_index = 0; tagCntType_index <= tagCntType_last; tagCntType_index++)
        {
            inter_result2 = tagCntType_result[tagCntType_index];
            for (L_VALUE(TEST_ENABLE_STATUS_MIN, enable); enable <= enable_max; enable++)
            {
                ASSIGN_EXPECT_RESULT(enable, TEST_ENABLE_STATUS_MIN, TEST_ENABLE_STATUS_MAX, inter_result3);
                if(!(HWP_UNIT_VALID_LOCAL(unit)))
                    expect_result = RT_ERR_FAILED;
                else
                    expect_result = (inter_result2 && inter_result3)? RT_ERR_OK : RT_ERR_FAILED;
                ret = rtk_stat_tagLenCntIncEnable_set(unit, tagCntType[tagCntType_index], enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_stat_tagLenCntIncEnable_set (unit %u, tagCntType %u, enable %u)"
                                    , ret, expect_result, unit, tagCntType[tagCntType_index], enable);

                    ret = rtk_stat_tagLenCntIncEnable_get(unit, tagCntType[tagCntType_index], &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_stat_tagLenCntIncEnable_get (unit %u, tagCntType %u, enable %u)"
                                     , ret, expect_result, unit, tagCntType[tagCntType_index], result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_stat_tagLenCntIncEnable_set/get value unequal (unit %u, tagCntType %u, enable %u)"
                                     , result_enable, enable, unit, tagCntType[tagCntType_index], enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_stat_tagLenCntIncEnable_set (unit %u, tagCntType %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, tagCntType[tagCntType_index], enable);
                }
            }
            ret = rtk_stat_tagLenCntIncEnable_get(unit, tagCntType[tagCntType_index], &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = (inter_result2)? RT_ERR_OK : RT_ERR_FAILED;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_stat_tagLenCntIncEnable_get (unit %u, tagCntType %u)"
                                , ret, expect_result, unit, tagCntType[tagCntType_index]);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_stat_tagLenCntIncEnable_get (unit %u, tagCntType %u)"
                                    , ret, RT_ERR_OK, unit, tagCntType[tagCntType_index]);
            }
        }
    }

    return RT_ERR_OK;
}

int32 dal_stat_enable_test(uint32 caseNo, uint32 unit)
{
    int32 ret;
    rtk_enable_t  enable_temp;
    int8  inter_result2 = 1;
    int32  expect_result = 1;

    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_stat_enable_set(unit, 0))
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_stat_enable_get(unit, &enable_temp)))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

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
                ret = rtk_stat_enable_set(unit, enable);
                if (RT_ERR_OK == expect_result)
                {
                    RT_TEST_IS_EQUAL_INT("rtk_stat_enable_set (unit %u, enable %u)"
                                    , ret, expect_result, unit, enable);

                    ret = rtk_stat_enable_get(unit, &result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_stat_enable_get (unit %u, enable %u)"
                                     , ret, expect_result, unit, result_enable);
                    RT_TEST_IS_EQUAL_INT("rtk_stat_enable_set/get value unequal (unit %u, enable %u)"
                                     , result_enable, enable, unit, enable);
                }
                else
                {
                    RT_TEST_IS_NOT_EQUAL_INT("rtk_stat_enable_set (unit %u, enable %u)"
                                        , ret, RT_ERR_OK, unit, enable);
                }
            }
            ret = rtk_stat_enable_get(unit, &result_enable);
            if(!(HWP_UNIT_VALID_LOCAL(unit)))
                expect_result = RT_ERR_FAILED;
            else
                expect_result = RT_ERR_OK;
            if (expect_result == RT_ERR_OK)
            {
                RT_TEST_IS_EQUAL_INT("rtk_stat_enable_get (unit %u)"
                                , ret, expect_result, unit);
            }
            else
            {
                RT_TEST_IS_NOT_EQUAL_INT("rtk_stat_enable_get (unit %u)"
                                    , ret, RT_ERR_OK, unit);
            }
    }

    return RT_ERR_OK;
}
