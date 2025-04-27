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

#ifndef __RT_UTIL_SERDES_H__
#define __RT_UTIL_SERDES_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util.h>
#include <rtk/sds.h>

/*
 * Symbol Definition
 */


/*
 * Data Type Declaration
 */

/* Seres in MAC/PHY */
typedef enum serdes_type_e
{
    SERDES_IN_MAC = 0,
    SERDES_IN_PHY = 1,
    SERDES_IN_END
} rt_serdes_type_t;

typedef struct rt_sds_s
{
    rt_serdes_type_t type;
    /* for type SERDES_IN_MAC */
    int mac_sdsId;
    /* for type SERDES_IN_PHY */
    rtk_port_t base_port;
    int phy_sdsId;
}rt_sds_t;

/* Serdes TX scan option */
#define RT_SDS_TX_SCAN_OPT_PAIR_TST  0x1
#define RT_SDS_TX_SCAN_OPT_RX_CALI   0x2
#define RT_SDS_TX_SCAN_OPT_MANUAL_LEQ   0x4
#define RT_SDS_TX_SCAN_OPT_VERBOSE  0x8

/* Serdes TX scan result */
#define RT_SDS_TXSCAN_PASS 'o'
#define RT_SDS_TXSCAN_FAIL 'x'
#define RT_SDS_TXSCAN_FAIL_CALI 'c'
#define RT_SDS_TXSCAN_FAIL_LEQ  'q'
#define RT_SDS_TXSCAN_FAIL_LINK 'd'

/* Serdes TX scan test method */
typedef enum rt_util_txScanMethod_e
{
    RT_UTIL_TXSCAN_NIC_TX = 0,
    RT_UTIL_TXSCAN_SERDES_TEST,
    RT_UTIL_TXSCAN_EYE_MON,
    RT_UTIL_TXSCAN_END
} rt_util_txScanMethod_t;

typedef struct rtk_sdsTxScanParam_s
{
    /* Specify the test/peer port number and serdes */
    rtk_port_t testPort;
    rtk_port_t peerPort;
    rt_sds_t testSerdes;
    rt_sds_t peerSerdes;
    uint32 option;
    rtk_sds_leq_t leqValue; //manual Leq value; if leq option is enabled
    rt_util_txScanMethod_t txScanMethod;
    /* for method NIC_TX */
    uint32 pktCnt;
    uint32 pktLen;
    /* for method SERDES_TEST */
    rtk_sds_testMode_t sdsTestMode;
    uint32 sdsTestTime; //second
    /* for method eye monitor */
    uint32 frameNum;
    /* Scan TX AMP range and step */
    uint32 mainAmp;
    rt_valRangeStep_t preAmpRangeStep; // 1 dimension
    rt_valRangeStep_t postAmpRangeStep; // 2 dimension
}rt_sdsTxScanParam_t;

#define RT_SDS_AMP_RANGE 32
typedef struct rt_sdsTxScanChart_s
{
    rt_sds_t testSerdes;
    rt_sds_t peerSerdes;
    uint8 rMap[RT_SDS_AMP_RANGE][RT_SDS_AMP_RANGE];
    uint8 eyeHeight[RT_SDS_AMP_RANGE][RT_SDS_AMP_RANGE];
    uint8 eyeWidth[RT_SDS_AMP_RANGE][RT_SDS_AMP_RANGE];
}rt_sdsTxScanChart_t;


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
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
*      8380, 8390, 9300, 9310
* Note:
*      None
* Changes:
*      None
*/
extern int32
rt_util_serdesTxEyeParam_scan(uint32 unit,rt_sdsTxScanParam_t * txScanParam,
                                      rt_sdsTxScanChart_t * scanResult);

#endif /* __RT_UTIL_SERDES_H__ */

