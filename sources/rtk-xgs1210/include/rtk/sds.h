/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 81532 $
 * $Date: 2017-08-18 16:50:57 +0800 (Fri, 18 Aug 2017) $
 *
 * Purpose : Definition of SerDes API
 *
 * Feature : SerDes configuration
 */

#ifndef __RTK_SDS_H__
#define __RTK_SDS_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chipdef.h>

/*
 * Symbol Definition
 */
#define RTK_SDS_SYMERR_ALL_MAX      2
#define RTK_SDS_SYMERR_CHANNEL_MAX  8

/*
 * Data Declaration
 */
typedef enum rtk_sds_testMode_e
{
    RTK_SDS_TESTMODE_DISABLE,
    RTK_SDS_TESTMODE_PRBS9,
    RTK_SDS_TESTMODE_PRBS31,
    RTK_SDS_TESTMODE_SQUARE8,
    RTK_SDS_TESTMODE_END
} rtk_sds_testMode_t;

typedef struct rtk_sds_linkSts_s
{
    uint32  sts;
    uint32  sts1;
    uint32  latch_sts;
    uint32  latch_sts1;
} rtk_sds_linkSts_t;

typedef struct rtk_sds_symErr_s
{
    uint32  all[RTK_SDS_SYMERR_ALL_MAX];
    uint32  ch[RTK_SDS_SYMERR_CHANNEL_MAX];
    uint32  latch_blk_lock;
    uint32  latch_hiber;
    uint32  ber;
    uint32  blk_err;
} rtk_sds_symErr_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390SDS, 9300SDS, 9310SDS
 * Note:
 *      info      - symbol error count information
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

/* Function Name:
 *      rtk_sds_symErr_clear
 * Description:
 *      Clear the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_symErr_clear(uint32 unit, uint32 sds);

/* Function Name:
 *      rtk_sds_linkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - link status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390SDS, 9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

/* Function Name:
 *      rtk_sds_testModeCnt_get
 * Description:
 *      Get SerDes test mode counter
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      cnt       - SerDes test mode counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8390SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt);

/* Function Name:
 *      rtk_sds_testMode_set
 * Description:
 *      Set SerDes test mode.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      mode      - test mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8390SDS, 9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode);

/* Function Name:
 *      rtk_sds_rx_rst
 * Description:
 *      Reset Serdes
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Applicable:
 *      8390SDS, 9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_rx_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      rtk_sds_leq_adapt
 * Description:
 *      Adapt SerDes LEQ
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_leq_adapt(uint32  unit, uint32 sds);

/* Function Name:
 *      rtk_sds_leq_get
 * Description:
 *      Get SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      leq - LEQ value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_leq_get(uint32 unit, uint32 sds, uint32 *leq);

/* Function Name:
 *      rtk_sds_xsgNwayEn_set
 * Description:
 *      Set SerDes XSG N-way state
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      en   - Configure XSG N-way state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en);

/* Function Name:
 *      rtk_sds_cmuBand_get
 * Description:
 *      Get SerDes CMU band
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      band - CMU band value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band);

/* Function Name:
 *      rtk_sds_cmuBand_set
 * Description:
 *      Set SerDes CMU band state and value
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      en   - Configure CMU band state
 *      val  - CMU band
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val);

/* Function Name:
 *      rtk_sds_eyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      sds     - SerDis id
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum);

/* Function Name:
 *      rtk_sds_eyeParam_get
 * Description:
 *      Get SerDes eye parameter
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pParam - eye parameter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam);

/* Function Name:
 *      rtk_sds_eyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      param  - eye parameter value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Applicable:
 *      9300SDS
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param);

/* Function Name:
 *      rtk_sds_rxCaliConf_get
 * Description:
 *      Get the SerDes rx calibration configration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pConf      - pointer to the sds rx calibration configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
extern int32
rtk_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf);

/* Function Name:
 *      rtk_sds_rxCaliConf_set
 * Description:
 *      Config the SerDes rx calibration.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      conf      - rx calibration conf
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
extern int32
rtk_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf);

#endif /* __RTK_SDS_H__ */
