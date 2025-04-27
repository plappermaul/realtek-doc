/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
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
 * Purpose : Define the serdes utility in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <common/util/rt_util_serdes.h>
#include <common/util/rt_util_test.h>
#include <common/rt_error.h>
#include <rtk/port.h>
#include <osal/time.h>
#include <hal/mac/mac_debug.h>
#include <hal/common/miim_debug.h>
#include <hwp/hw_profile.h>
#include <osal/print.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
int32 utilSdsDebugLog = FALSE;

/*
 * Macro Definition
 */
#define _RT_UTIL_SDS_DBG_MSG(fmt, args...)                  \
    do {                                                        \
        if (utilSdsDebugLog) {\
            osal_printf(fmt, ##args);                           \
        }                                                       \
    } while(0)

#define _RT_UTIL_SDS_DBG_MSG_SDS(sds, fmt, args...)    \
    do {  \
        if (utilSdsDebugLog) {\
            if (sds.type == SERDES_IN_MAC){ \
                osal_printf("MAC serdes id %d: ",sds.mac_sdsId); \
            } \
            else if (sds.type == SERDES_IN_PHY) \
            {  \
                osal_printf("PHY(%d) serdes id %d: ",sds.base_port, sds.phy_sdsId); \
            }\
            osal_printf(fmt, ##args);                           \
        }                                                       \
    } while(0)


/*
 * Function Declaration
 */
/* MAC/PHY Serdes API */
int32 _rt_util_sds_eye_param_get(uint32 unit, rt_sds_t sds, rtk_sds_eyeParam_t *eyeParam)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(rtk_sds_eyeParam_get(unit, sds.mac_sdsId, eyeParam), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(rtk_port_phySdsEyeParam_get(unit, sds.base_port, sds.phy_sdsId, eyeParam), ret);
    }
    return RT_ERR_OK;
}

int32 _rt_util_sds_eye_param_set(uint32 unit, rt_sds_t sds, rtk_sds_eyeParam_t eyeParam)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(rtk_sds_eyeParam_set(unit, sds.mac_sdsId, eyeParam), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(rtk_port_phySdsEyeParam_set(unit, sds.base_port , sds.phy_sdsId, &eyeParam), ret);
    }
    _RT_UTIL_SDS_DBG_MSG_SDS(sds, "set amp(%d/%d/%d)\n", eyeParam.pre_amp, eyeParam.main_amp, eyeParam.post_amp);
    return RT_ERR_OK;
}
int32 _rt_util_sds_rx_cali(uint32 unit, rt_sds_t sds)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(mac_debug_sds_rxCali(unit, sds.mac_sdsId, 0), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(miim_debug_sdsRxCali_start(unit, sds.base_port, sds.phy_sdsId), ret);
    }
    _RT_UTIL_SDS_DBG_MSG_SDS(sds, "do calibration\n");
    return RT_ERR_OK;
}

int32 _rt_util_sds_leq_set(uint32 unit, rt_sds_t sds, rtk_sds_leq_t *leq)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(rtk_sds_leq_set(unit, sds.mac_sdsId, leq), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(rtk_port_phySdsLeq_set(unit, sds.base_port, sds.phy_sdsId, leq->manual, leq->val), ret);
    }
    _RT_UTIL_SDS_DBG_MSG_SDS(sds, "set leq %d\n", leq->val);
    return RT_ERR_OK;
}

int32 _rt_util_sds_leq_get(uint32 unit, rt_sds_t sds, rtk_sds_leq_t *leq)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(rtk_sds_leq_get(unit, sds.mac_sdsId, leq), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(rtk_port_phySdsLeq_get(unit, sds.base_port, sds.phy_sdsId, &(leq->manual), &(leq->val)), ret);
    }
    return RT_ERR_OK;
}


int32 _rt_util_sds_eyeMonitorInfo_get(uint32 unit, rt_sds_t sds,uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo)
{
    int32 ret;
    if (sds.type == SERDES_IN_MAC){
        RT_ERR_CHK(rtk_sds_eyeMonitorInfo_get(unit, sds.mac_sdsId, frameNum, pInfo), ret);
    }
    else if (sds.type == SERDES_IN_PHY)
    {
        RT_ERR_CHK(rtk_port_phyEyeMonitorInfo_get(unit, sds.base_port, sds.phy_sdsId, frameNum, pInfo), ret);
    }
    return RT_ERR_OK;
}

int32
_rt_util_serdesTxScanLink_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *sts)
{
    int32 retryLink=20, ret = RT_ERR_FAILED;
    rtk_port_linkStatus_t   linkSts = PORT_LINKDOWN;
    rtk_port_media_t        media;

    while(retryLink >= 0 && linkSts == PORT_LINKDOWN)
    {
        RT_ERR_CHK(rtk_port_linkMedia_get(unit, port, &linkSts, &media), ret);
        retryLink--;
        osal_time_mdelay(200);
    }
    *sts = linkSts;
    return RT_ERR_OK;
}

int32
_rt_util_serdesTxScanAMP_strengthen(uint32 unit, rt_sdsTxScanParam_t *txScanParam)
{
    int32 ret;
    rtk_port_linkStatus_t linkSts = PORT_LINKDOWN;
    rtk_sds_eyeParam_t eyeParam;

    osal_printf("Tx-scan strengthen signal...");
    /*Set main AMP=21 , let the signal to be detected again */
    osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
    RT_ERR_CHK(_rt_util_sds_eye_param_get(unit, txScanParam->testSerdes, &eyeParam), ret);
    eyeParam.main_amp = 21;
    RT_ERR_CHK(_rt_util_sds_eye_param_set(unit, txScanParam->testSerdes, eyeParam), ret);

    if (txScanParam->option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        RT_ERR_CHK(_rt_util_sds_eye_param_get(unit, txScanParam->peerSerdes, &eyeParam), ret);
        eyeParam.main_amp = 21;
        RT_ERR_CHK(_rt_util_sds_eye_param_set(unit, txScanParam->peerSerdes, eyeParam), ret);
    }
    /* delay 150ms */
    osal_time_mdelay(150);

    /* Check if link up */
    RT_ERR_CHK(_rt_util_serdesTxScanLink_get(unit, txScanParam->testPort, &linkSts), ret);
    if (PORT_LINKDOWN == linkSts)
    {
         osal_printf("failed\n");
         return RT_ERR_FAILED;
    }
    osal_printf("success\n");

    return RT_ERR_OK;
}
/* Function Name:
*      rt_util_serdesTxEyeParam_scan
* Description:
*      Perform the serdes TX scan procedure and get the result back
*      The procedure is as follows:
*      1. Foreach pre/post AMP param, do
*      2.1 Set serdes TX eye param (pre/main/post AMP)
*      2.2 Do the serdes RX side calibration or set leq
*      2.3 Perform one of three test methods: cpu-tx, serdes test mode, eye-monitor
*      2.4 Get the result
* Input:
*      unit          - unit id
*      txScanParam   - param needed by serdes TX scan procedure
* Output:
*      scanResult    - TX scan result
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_UNIT_ID - invalid unit id
* Applicable:
*      9310
* Note:
*      None
* Changes:
*      None
*/
int32
rt_util_serdesTxEyeParam_scan(uint32 unit,
                              rt_sdsTxScanParam_t *txScanParam,
                              rt_sdsTxScanChart_t *scanResult)
{
    uint32 preAmp,postAmp;
    int32   ret = RT_ERR_OK, ret1=0, result = 0;
    rtk_sds_eyeParam_t param;
    rtk_port_linkStatus_t   linkSts;
    /* test method data */
    rt_test_nicTxTestData_t nicTxTestData;
    rt_test_sdsTestData_t sdsTestData;
    rtk_sds_eyeMonInfo_t eyeMonInfo;
    rtk_sds_leq_t leq;

    RT_PARAM_CHK((NULL == txScanParam), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == scanResult), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((SERDES_IN_MAC != txScanParam->testSerdes.type), RT_ERR_INPUT);
    RT_PARAM_CHK((txScanParam->txScanMethod != RT_UTIL_TXSCAN_NIC_TX) &&
                 (txScanParam->txScanMethod != RT_UTIL_TXSCAN_SERDES_TEST) &&
                 (txScanParam->txScanMethod != RT_UTIL_TXSCAN_EYE_MON), RT_ERR_INPUT);

    txScanParam->testSerdes.mac_sdsId = HWP_PORT_SDSID(unit, txScanParam->testPort);
    txScanParam->peerSerdes.mac_sdsId = HWP_PORT_SDSID(unit, txScanParam->peerPort);

    scanResult->testSerdes = txScanParam->testSerdes;
    scanResult->peerSerdes = txScanParam->peerSerdes;

    utilSdsDebugLog = (txScanParam->option & RT_SDS_TX_SCAN_OPT_VERBOSE)? TRUE: FALSE;
    /* Need to disable watchdog when doing tx-scan to avoid config is overwitten */
    RT_ERR_CHK(hal_setWatchdogMonitorEnable(DISABLED), ret);
    for(postAmp = txScanParam->postAmpRangeStep.start;
        postAmp <= txScanParam->postAmpRangeStep.end;
        postAmp += txScanParam->postAmpRangeStep.step)
    {
        for(preAmp = txScanParam->preAmpRangeStep.start;
            preAmp <= txScanParam->preAmpRangeStep.end;
            preAmp += txScanParam->preAmpRangeStep.step)
        {
            /* Set eye param to serdes */
            RT_ERR_HDL(_rt_util_sds_eye_param_get(unit, txScanParam->testSerdes, &param), exit, ret);
            param.main_amp = txScanParam->mainAmp;
            param.pre_amp = preAmp;
            param.post_amp = postAmp;
            RT_ERR_HDL(_rt_util_sds_eye_param_set(unit, txScanParam->testSerdes, param), exit, ret);

            /* delay 150ms */
            osal_time_mdelay(150);

            if(txScanParam->option & RT_SDS_TX_SCAN_OPT_RX_CALI)
            {
                ret = _rt_util_sds_rx_cali(unit, txScanParam->peerSerdes);
                if(ret != RT_ERR_OK)
                {
                    osal_printf("serdes %d calibration fail %d (pre/main/post=%d/%d/%d)\n",
                                 txScanParam->peerSerdes.mac_sdsId,ret,
                                 param.pre_amp, param.main_amp, param.post_amp);
                    scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_CALI;
                    continue;
                }
                if(txScanParam->option & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ)
                {
                    ret = _rt_util_sds_leq_set(unit, txScanParam->peerSerdes, &(txScanParam->leqValue));

                    if(ret != RT_ERR_OK)
                    {
                        osal_printf("serdes %d set manual leq fail %d (pre/main/post=%d/%d/%d)\n",
                                    txScanParam->peerSerdes.mac_sdsId,ret,
                                    param.pre_amp, param.main_amp, param.post_amp);
                        scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_LEQ;
                        continue;
                    }
                 }
                 /* delay 1s for calibration */
                 osal_time_mdelay(1000);
             }
             /* Reverse TX/RX do TX amp setting, calibration, leq */
             if(txScanParam->option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
             {
                 RT_ERR_HDL(_rt_util_sds_eye_param_get(unit, txScanParam->peerSerdes, &param), exit, ret);
                 param.main_amp = txScanParam->mainAmp;
                 param.pre_amp = preAmp;
                 param.post_amp = postAmp;
                 RT_ERR_HDL(_rt_util_sds_eye_param_set(unit, txScanParam->peerSerdes, param), exit, ret);
                 /* delay 150ms */
                 osal_time_mdelay(150);

                if(txScanParam->option & RT_SDS_TX_SCAN_OPT_RX_CALI)
                {
                     ret = _rt_util_sds_rx_cali(unit, txScanParam->testSerdes);
                     if(ret != RT_ERR_OK)
                     {
                         osal_printf("serdes %d calibration fail %d (pre/main/post=%d/%d/%d)\n",
                                     txScanParam->testSerdes.mac_sdsId,ret,
                                     param.pre_amp, param.main_amp, param.post_amp);
                         scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_CALI;
                         continue;
                     }
                     if(txScanParam->option & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ)
                     {
                         ret = _rt_util_sds_leq_set(unit, txScanParam->testSerdes, &(txScanParam->leqValue));
                         if(ret != RT_ERR_OK)
                         {
                             osal_printf("serdes %d set manual leq fail %d (pre/main/post=%d/%d/%d)\n",
                                         txScanParam->testSerdes.mac_sdsId,ret,
                                         param.pre_amp, param.main_amp, param.post_amp);
                             scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_LEQ;
                             continue;
                         }
                     }
                     /* delay 1s for calibration */
                     osal_time_mdelay(1000);
                 }
             }

             /* Check if link is up */
             RT_ERR_HDL(_rt_util_serdesTxScanLink_get(unit, txScanParam->testPort, &linkSts),exit, ret);
             if(PORT_LINKDOWN == linkSts)
             {
                 osal_printf("Port %d linkdown (pre/main/post=%d/%d/%d)\n",txScanParam->testPort, param.pre_amp, param.main_amp, param.post_amp);
                 scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_LINK;
                 ret = _rt_util_serdesTxScanAMP_strengthen(unit, txScanParam);
                 if(ret != RT_ERR_OK)
                 {
                    osal_printf("Can not recover to link up state, stop the tx-scan process\n");
                    goto exit;
                 }
                 continue;
             }

             RT_ERR_HDL(_rt_util_serdesTxScanLink_get(unit, txScanParam->peerPort, &linkSts),exit, ret);
             if(PORT_LINKDOWN == linkSts)
             {
                 osal_printf("Port %d linkdown (pre/main/post=%d/%d/%d)\n",txScanParam->peerPort, param.pre_amp, param.main_amp, param.post_amp);
                 scanResult->rMap[preAmp][postAmp] = RT_SDS_TXSCAN_FAIL_LINK;
                 ret = _rt_util_serdesTxScanAMP_strengthen(unit, txScanParam);
                 if(ret != RT_ERR_OK)
                 {
                    osal_printf("Can not recover to link up state, stop the tx-scan process\n");
                    goto exit;
                 }
                 continue;
             }

             if(txScanParam->txScanMethod == RT_UTIL_TXSCAN_NIC_TX)
             {
                 osal_memset(&nicTxTestData, 0, sizeof(rt_test_nicTxTestData_t));
                 nicTxTestData.unit = unit;
                 nicTxTestData.txPort = txScanParam->testPort;
                 nicTxTestData.rxPort = txScanParam->peerPort;
                 nicTxTestData.testOption |= (txScanParam->option & RT_SDS_TX_SCAN_OPT_PAIR_TST)? RT_TEST_OPT_BIDIR:0;
                 nicTxTestData.testOption |= (txScanParam->option & RT_SDS_TX_SCAN_OPT_VERBOSE)? RT_TEST_OPT_VERBASE:0;
                 nicTxTestData.pktCnt = txScanParam->pktCnt;
                 nicTxTestData.pktLen = txScanParam->pktLen;
                 RT_ERR_HDL(rt_test_run(RT_TEST_NIC_TX, (void*)&nicTxTestData), exit, ret);
                 result = nicTxTestData.result;
                 scanResult->rMap[preAmp][postAmp] = (result == RT_TEST_PASS)? RT_SDS_TXSCAN_PASS : RT_SDS_TXSCAN_FAIL;
             }
             else if(txScanParam->txScanMethod == RT_UTIL_TXSCAN_SERDES_TEST)
             {
                 osal_memset(&sdsTestData, 0, sizeof(rt_test_sdsTestData_t));
                 sdsTestData.unit = unit;
                 sdsTestData.txSds = txScanParam->testSerdes;
                 sdsTestData.rxSds = txScanParam->peerSerdes;
                 sdsTestData.testOption |= (txScanParam->option & RT_SDS_TX_SCAN_OPT_PAIR_TST)? RT_TEST_OPT_BIDIR:0;
                 sdsTestData.testOption |= (txScanParam->option & RT_SDS_TX_SCAN_OPT_VERBOSE)? RT_TEST_OPT_VERBASE:0;
                 sdsTestData.mode = txScanParam->sdsTestMode;
                 sdsTestData.msec = txScanParam->sdsTestTime * 1000; //sec to ms
                 RT_ERR_HDL(rt_test_run(RT_TEST_SERDES_MODE, (void*)&sdsTestData), exit, ret);
                 result = sdsTestData.result;
                 scanResult->rMap[preAmp][postAmp] = (result == RT_TEST_PASS)? RT_SDS_TXSCAN_PASS : RT_SDS_TXSCAN_FAIL;
             }
             else if(txScanParam->txScanMethod == RT_UTIL_TXSCAN_EYE_MON)
             {
                /* get eye info from RX side, this does not support OPT_PAIR_TST */
                RT_ERR_HDL(_rt_util_sds_eyeMonitorInfo_get(unit, txScanParam->peerSerdes, txScanParam->frameNum, &eyeMonInfo),exit,ret);

                scanResult->eyeHeight[preAmp][postAmp] = eyeMonInfo.height;
                scanResult->eyeWidth[preAmp][postAmp] = eyeMonInfo.width;
                RT_ERR_HDL(_rt_util_sds_leq_get(unit, txScanParam->peerSerdes, &leq), exit, ret);
                scanResult->rMap[preAmp][postAmp] = leq.val;
             }
        }
    }

exit:
    /* Resume watchdog when tx-scan stops */
    ret1 = hal_setWatchdogMonitorEnable(ENABLED);
    if(ret1 != RT_ERR_OK)
    {
	    osal_printf("Resume watchdog failed.\n");
    }
    utilSdsDebugLog = FALSE;
    return ret;
}


