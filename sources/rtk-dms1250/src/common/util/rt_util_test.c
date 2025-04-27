/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date:
 *
 * Purpose : Define the test method utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <common/util/rt_util_test.h>
#include <common/rt_error.h>
#include <hwp/hw_profile.h>
#include <hal/mac/reg.h>
#include <rtk/stat.h>
#include <rtk/switch.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <osal/time.h>
#include <osal/print.h>
#include <common/error.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */
#define RT_TEST_DBG_PRINT   osal_printf

/*
 * Function Declaration
 */


/* --------------------
 * NIC TX test utility
 * --------------------
 */
uint8 rt_test_nicTx_pkt[] = { // random packet content
    0x00, 0x13, 0x49, 0x00, 0x00, 0x01, 0x00, 0x19, 0xcb, 0xfd, 0x78, 0x14, 0x08, 0x12, 0x45, 0x00,
    0x03, 0xea, 0x76, 0xef, 0x46, 0xaa, 0xe6, 0xaf, 0x3b, 0xaf, 0x37, 0xf0, 0xc5, 0x28, 0x94, 0xc9,
    0xc7, 0xbf, 0xff, 0x4a, 0xb2, 0x38, 0x2d, 0xc8, 0x21, 0x05, 0x32, 0x0a, 0xcf, 0xd6, 0xf1, 0xd2,
    0xc0, 0x68, 0xc1, 0x07, 0x12, 0xa8, 0xb6, 0x4d, 0x57, 0xed, 0x3e, 0x1d, 0x15, 0xd2, 0xe6, 0xdc,
    0x91, 0xe6, 0x26, 0x44, 0x1e, 0x54, 0x0c, 0x40, 0x59, 0x3f, 0x4a, 0x28, 0x15, 0x3c, 0xfb, 0xd6
    };

#define GET_MIB_COUNTER(txPort, txPortCntrs) \
    osal_memset(&txPortCntrs, 0 , sizeof(txPortCntrs)); \
    RT_ERR_CHK(rtk_stat_port_get(unit, txPort, IF_IN_UCAST_PKTS_INDEX, \
                                    &(txPortCntrs.ifInUcastPkts)), ret); \
    RT_ERR_CHK(rtk_stat_port_get(unit, txPort, IF_IN_OCTETS_INDEX, \
                                    &(txPortCntrs.ifInOctets)), ret); \
    RT_ERR_CHK(rtk_stat_port_get(unit, txPort, IF_OUT_UCAST_PKTS_CNT_INDEX, \
                                    &(txPortCntrs.ifOutUcastPkts)), ret); \
    RT_ERR_CHK(rtk_stat_port_get(unit, txPort, IF_OUT_OCTETS_INDEX, \
                                    &(txPortCntrs.ifOutOctets)), ret); \
    RT_ERR_CHK(rtk_stat_port_get(unit, txPort, ETHER_STATS_CRC_ALIGN_ERRORS_INDEX, \
                                    &(txPortCntrs.etherStatsCRCAlignErrors)), ret);


#define PRINT_MIB_COUNTER(str, txPort, txPortCntrs) \
    osal_printf("%s Port %d:InUcast=%llu/%llu, OutUcast=%llu/%llu\n",str,txPort, \
                    txPortCntrs.ifInUcastPkts, txPortCntrs.ifInOctets, \
                    txPortCntrs.ifOutUcastPkts, txPortCntrs.ifOutOctets);


typedef struct rt_test_portCntr_s
{
    /* Naming is the same with rtk_stat_port_cntr_t */
    uint64 ifInUcastPkts;
    uint64 ifInOctets;
    uint64 ifOutUcastPkts;
    uint64 ifOutOctets;
    uint64 etherStatsCRCAlignErrors; //tx+rx crc error counter
}rt_test_portCntr_t;

int32 _rt_test_nicTx_init(void *need_cast_data)
{
    int32 ret;
    uint32 unit;
    rt_test_nicTxTestData_t *data = (rt_test_nicTxTestData_t *)need_cast_data;
    rt_test_portCntr_t txPortCntrs;
    rt_test_portCntr_t rxPortCntrs;
    rtk_port_t txPort, rxPort;
    rtk_switch_devInfo_t devInfo;

    unit = data->unit;
    txPort = data->txPort;
    rxPort = data->rxPort;

    /* check */
    if (data->txPort == data->rxPort)
    {
        // if tx = rx, need to enable PHY loopback inside this function
        return RT_ERR_NOT_SUPPORTED;
    }
    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    RT_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);
    if (HWP_CHIP_FAMILY_ID(unit) != RTL9310_FAMILY_ID)
    {
        return RT_ERR_NOT_SUPPORTED;
    }

    /* Backup l2 learn mode and action */
    RT_ERR_CHK(rtk_l2_portNewMacOp_get(data->unit, data->txPort, &data->testPortLrnMode, &data->testPortAct),ret);
    RT_ERR_CHK(rtk_l2_portNewMacOp_get(data->unit, data->rxPort, &data->peerPortLrnMode, &data->peerPortAct),ret);
    /* Set l2 not learning and drop */
    RT_ERR_CHK(rtk_l2_portNewMacOp_set(data->unit, data->txPort, NOT_LEARNING, ACTION_DROP),ret);
    RT_ERR_CHK(rtk_l2_portNewMacOp_set(data->unit, data->rxPort, NOT_LEARNING, ACTION_DROP),ret);

    /* Reset MIB counter */
    RT_ERR_CHK(rtk_stat_port_reset(unit, rxPort), ret);
    RT_ERR_CHK(rtk_stat_port_reset(unit, txPort), ret);

    GET_MIB_COUNTER(txPort, txPortCntrs);
    GET_MIB_COUNTER(rxPort, rxPortCntrs);

    if (txPortCntrs.ifOutUcastPkts != 0 || txPortCntrs.ifOutOctets != 0)
    {
        //Should not enter here
        return RT_ERR_FAILED;
    }
    if (rxPortCntrs.ifInUcastPkts != 0 || txPortCntrs.ifInOctets != 0)
    {
        //Should not enter here
        return RT_ERR_FAILED;
    }
    if (rxPortCntrs.etherStatsCRCAlignErrors != 0 ||
        txPortCntrs.etherStatsCRCAlignErrors != 0)
    {
        //Should not enter here
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _rt_test_nicTx_cleanUp(void *need_cast_data)
{
    int32 ret;
    rt_test_nicTxTestData_t *data = (rt_test_nicTxTestData_t *)need_cast_data;

    /* Restore l2 learn mode and action */
    RT_ERR_CHK(rtk_l2_portNewMacOp_set(data->unit, data->txPort, data->testPortLrnMode, data->testPortAct), ret);
    RT_ERR_CHK(rtk_l2_portNewMacOp_set(data->unit, data->rxPort, data->peerPortLrnMode, data->peerPortAct), ret);

    return RT_ERR_OK;
}
int32 _rt_test_nicTx_chkErrCnt(void *need_cast_data)
{
    int ret;
    rt_test_nicTxTestData_t *data = (rt_test_nicTxTestData_t *)need_cast_data;
    uint32 unit = data->unit;
    rtk_port_t txPort= data->txPort, rxPort=data->rxPort;
    rt_test_portCntr_t txPortCntrs;
    rt_test_portCntr_t rxPortCntrs;

    /* Get MIB counter */
    GET_MIB_COUNTER(txPort, txPortCntrs);
    GET_MIB_COUNTER(rxPort, rxPortCntrs);

    if(txPortCntrs.ifOutOctets != txPortCntrs.ifOutUcastPkts * data->pktLen)
    {
        //retry once
        osal_time_mdelay(10);
        GET_MIB_COUNTER(txPort, txPortCntrs);
        GET_MIB_COUNTER(rxPort, rxPortCntrs);
    }
    if (data->testOption & RT_TEST_OPT_VERBASE)
    {
        PRINT_MIB_COUNTER("TX", txPort, txPortCntrs);
        PRINT_MIB_COUNTER("RX", rxPort, rxPortCntrs);
    }

    /* Check if TX counter equals to RX counter */
    if(txPortCntrs.ifOutUcastPkts != rxPortCntrs.ifInUcastPkts)
    {
        data->result = RT_TEST_FAIL;
        return RT_ERR_OK;
    }
    if((txPortCntrs.ifOutOctets != rxPortCntrs.ifInOctets) ||
    (txPortCntrs.ifOutOctets != txPortCntrs.ifOutUcastPkts * data->pktLen))
    {
        data->result = RT_TEST_FAIL;
        return RT_ERR_OK;
    }
    /* Check CRC error */
    if (rxPortCntrs.etherStatsCRCAlignErrors != 0 ||
        txPortCntrs.etherStatsCRCAlignErrors != 0)
    {
        data->result = RT_TEST_FAIL;
        return RT_ERR_OK;
    }
    data->result = RT_TEST_PASS;
    return RT_ERR_OK;

}
int32 _rt_test_nicTx(void *need_cast_data)
{
#ifdef CONFIG_SDK_DRIVER_NIC
#define TX_PMP_START_IDX 13 // 7 bytes, cpuTag[13]~cpuTag[19]
#define TX_PMP_END_IDX 19
#define BIT_PER_BYTE 8
    rt_test_nicTxTestData_t *data =(rt_test_nicTxTestData_t *)need_cast_data;
    /* only for 9310 */
    uint8   cpuTag[CPUTAG_SIZE] = {0x88,0x99,0x04,0x00,
                            0x00,0x00,0x02,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x01,0x00,0x00,
                            0x00,0x00,0x00,0x00};
    int32 ret;
    uint32 unit;
    rtk_portmask_t portmask;
    rtk_port_t txPort;
    //rtk_port_t txPort, rxPort;

    RTK_PORTMASK_RESET(portmask);
    unit = data->unit;
    txPort = data->txPort;
    //rxPort = data->rxPort;

    /* NIC Diag TX  */
    //Set Tx portmap
    osal_memset(&cpuTag[TX_PMP_START_IDX], 0, TX_PMP_END_IDX - TX_PMP_START_IDX + 1);
    cpuTag[TX_PMP_END_IDX - (txPort/BIT_PER_BYTE)] = (1 << (txPort%BIT_PER_BYTE));
    //Call NIC API
    RT_ERR_CHK(drv_nic_tag_set(unit, NIC_TXTAG_MANUAL, cpuTag, &portmask), ret);
    RT_ERR_CHK(drv_nic_txData_set(unit, FALSE, rt_test_nicTx_pkt, data->pktLen), ret);

    ret = drv_nic_diagPkt_send(unit, data->pktCnt);
    if (ret != RT_ERR_OK)
    {
        osal_printf("NIC diag packet send fail\n");
        return ret;
    }

    osal_time_mdelay(2); //wait the packets sent out; 1ms should be enough
    return RT_ERR_OK;
#else
    return RT_ERR_DRIVER_NOT_SUPPORTED;
#endif
}


int32 _rt_test_nicTx_iter(void *need_cast_data, const int32 const *test_run_cnt, int32 *do_next)
{
    rt_test_nicTxTestData_t *data =(rt_test_nicTxTestData_t *)need_cast_data;
    rtk_port_t tmpPort;

    RT_PARAM_CHK((test_run_cnt == NULL)||(do_next== NULL), RT_ERR_NULL_POINTER);

    if(data->testOption & RT_TEST_OPT_BIDIR)
    {
        //swap tx/rx port
        tmpPort = data->txPort;
        data->txPort = data->rxPort;
        data->rxPort = tmpPort;
    }
    // if previous result is failed, do not run the next test
    if((*test_run_cnt < 2) && (RT_TEST_PASS == data->result))
    {
        *do_next = TRUE;
    }
    else
    {
        *do_next = FALSE;
    }
    return RT_ERR_OK;
}
/* --------------------
 * Serdes test mode utility
 * --------------------
 */

int32 _rt_test_sdsTest_iter(void *need_cast_data, const int32 const *test_run_cnt, int32 *do_next)
{
    rt_test_sdsTestData_t *data = need_cast_data;
    rt_sds_t tmpSds;

    RT_PARAM_CHK((test_run_cnt == NULL)||(do_next== NULL), RT_ERR_NULL_POINTER);

    if(data->testOption & RT_TEST_OPT_BIDIR)
    {
        //swap tx/rx serdes
        tmpSds = data->txSds;
        data->txSds = data->rxSds;
        data->rxSds = tmpSds;
    }
    // if previous result is failed, do not run the next test
    if((*test_run_cnt < 2) && (RT_TEST_PASS == data->result))
    {
        *do_next = TRUE;
    }
    else
    {
        *do_next = FALSE;
    }

    return RT_ERR_OK;
}


int32 _rt_test_sdsTest_init(void *need_cast_data)
{
    int32 ret;
    rt_test_sdsTestData_t *data = need_cast_data;
    if(data->txSds.type != SERDES_IN_MAC || data->rxSds.type != SERDES_IN_MAC)
    {
        return RT_ERR_INPUT;
    }

    RT_ERR_CHK(rtk_sds_testMode_set(data->unit, data->txSds.mac_sdsId, data->mode), ret);
    if(data->testOption & RT_TEST_OPT_VERBASE)
    {
        RT_TEST_DBG_PRINT("Set mode %d on serdes id %d \n", data->mode, data->txSds.mac_sdsId);
    }


    /* Set test mode at RX */
    RT_ERR_CHK(rtk_sds_testMode_set(data->unit, data->rxSds.mac_sdsId, data->mode), ret);
    //osal_time_mdelay(200);
    if(data->testOption & RT_TEST_OPT_VERBASE)
    {
        RT_TEST_DBG_PRINT("Set mode %d on serdes id %d \n", data->mode, data->rxSds.mac_sdsId);
    }


    return RT_ERR_OK;
}

int32 _rt_test_sdsTest(void *need_cast_data)
{
#define RT_TEST_SDS_TEST_ERR_CNT_RETRY 5
    int32 ret;
    uint32 cnt=0, retryCnt = RT_TEST_SDS_TEST_ERR_CNT_RETRY;
    rt_test_sdsTestData_t *data = need_cast_data;

    /* Set test mode at RX */
    do{
        // read to clear counter
        //osal_time_mdelay(200);
        RT_ERR_CHK(rtk_sds_testModeCnt_get(data->unit, data->rxSds.mac_sdsId, &cnt), ret);
        if(data->testOption & RT_TEST_OPT_VERBASE)
        {
            RT_TEST_DBG_PRINT("serdes id %d, before test get cnt %d at %d time\n",
                              data->rxSds.mac_sdsId, cnt,
                              RT_TEST_SDS_TEST_ERR_CNT_RETRY - retryCnt);
        }
        retryCnt--;
    }while((cnt != 0) && (retryCnt > 0));

    data->cntRecord = cnt;

    osal_time_mdelay(data->msec);
    return RT_ERR_OK;
}

int32 _rt_test_sdsTest_chkPrnErr(void *need_cast_data)
{
#define RT_TEST_SDS_TEST_ERR_CNT_MAX 0xFFFF
    rt_test_sdsTestData_t *data = need_cast_data;
    uint32 cnt=0;
    int32 ret;
    /* Check if pattern error at RX side */
    RT_ERR_CHK(rtk_sds_testModeCnt_get(data->unit, data->rxSds.mac_sdsId, &cnt), ret);
    if(data->testOption & RT_TEST_OPT_VERBASE)
    {
        RT_TEST_DBG_PRINT("serdes id %d, after test chk cnt %d\n", data->rxSds.mac_sdsId, cnt);
    }

    if((data->cntRecord == cnt) && (cnt > 0) && (cnt < RT_TEST_SDS_TEST_ERR_CNT_MAX))
    {
        RT_TEST_DBG_PRINT("serdes id %d, counter val(%d) is the same before/after the test\n", data->rxSds.mac_sdsId, cnt);
        return RT_ERR_FAILED;
    }

    data->result = (cnt == 0)? RT_TEST_PASS: RT_TEST_FAIL;
    return RT_ERR_OK;
}

int32 _rt_test_sdsTest_cleanUp(void *need_cast_data)
{
    int32 ret;
    rt_test_sdsTestData_t *data = need_cast_data;
    /* Disable test mode at both TX and RX side */
    RT_ERR_CHK(rtk_sds_testMode_set(data->unit, data->txSds.mac_sdsId, RTK_SDS_TESTMODE_DISABLE), ret);
    RT_ERR_CHK(rtk_sds_testMode_set(data->unit, data->rxSds.mac_sdsId, RTK_SDS_TESTMODE_DISABLE), ret);

    return RT_ERR_OK;
}


rt_test_proc_t rt_test_proc[]=
{
    /* test name   ,        test init ,       do_test  ,    test done callback ,     test iteration cb,  test cleanup  */
    {"NIC TX test", _rt_test_nicTx_init, _rt_test_nicTx,   _rt_test_nicTx_chkErrCnt , _rt_test_nicTx_iter, _rt_test_nicTx_cleanUp},
    {"SERDES test",_rt_test_sdsTest_init, _rt_test_sdsTest, _rt_test_sdsTest_chkPrnErr, _rt_test_sdsTest_iter, _rt_test_sdsTest_cleanUp },

};


int32 rt_test_run(int32 test_id, void *data)
{
    int32 ret;
    int32 test_run_cnt=0, do_next=FALSE;
    if(test_id >= sizeof(rt_test_proc)/sizeof(rt_test_proc_t))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    if(rt_test_proc[test_id].test_init)
    {
        RT_ERR_CHK((rt_test_proc[test_id].test_init)(data), ret);
    }

    do {
        if(rt_test_proc[test_id].do_test)
        {
            test_run_cnt++;
            RT_ERR_CHK((rt_test_proc[test_id].do_test)(data), ret);
        }

        if(rt_test_proc[test_id].test_done_cb)
        {
            RT_ERR_CHK((rt_test_proc[test_id].test_done_cb)(data), ret);
        }

        if(rt_test_proc[test_id].test_iter)
        {
            RT_ERR_CHK((rt_test_proc[test_id].test_iter)(data, &test_run_cnt, &do_next), ret);
        }
    }while(TRUE == do_next);

    if(rt_test_proc[test_id].test_cleanup)
    {
        RT_ERR_CHK((rt_test_proc[test_id].test_cleanup)(data), ret);
    }
    return RT_ERR_OK;
}


