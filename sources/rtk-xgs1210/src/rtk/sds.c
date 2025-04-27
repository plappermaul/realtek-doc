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

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/sds.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
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
int32
rtk_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_symErr_get(unit, sds, info);
}   /* end of rtk_sds_symErr_get */

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
int32
rtk_sds_symErr_clear(uint32 unit, uint32 sds)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_symErr_clear(unit, sds);
}   /* end of rtk_sds_symErr_clear */

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
int32
rtk_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_linkSts_get(unit, sds, info);
}   /* end of rtk_sds_linkSts_get */

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
int32
rtk_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_testModeCnt_get(unit, sds, cnt);
}   /* end of rtk_sds_testModeCnt_get */

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
int32
rtk_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_testMode_set(unit, sds, mode);
}   /* end of rtk_sds_testMode_set */

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
int32
rtk_sds_rx_rst(uint32  unit, uint32 sds)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_rx_rst(unit, sds);
}   /* end of rtk_sds_rx_rst */

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
int32
rtk_sds_leq_adapt(uint32  unit, uint32 sds)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_leq_adapt(unit, sds);
}   /* end of rtk_sds_leq_adapt */

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
int32
rtk_sds_leq_get(uint32 unit, uint32 sds, uint32 *leq)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_leq_get(unit, sds, leq);
}   /* end of rtk_sds_leq_get */

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
int32
rtk_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_xsgNwayEn_set(unit, sds, en);
}   /* end of rtk_sds_xsgNwayEn_set */

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
int32
rtk_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_cmuBand_get(unit, sds, band);
}   /* end of rtk_sds_cmuBand_get */

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
int32
rtk_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_cmuBand_set(unit, sds, en, val);
}   /* end of rtk_sds_cmuBand_set */

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
int32
rtk_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_eyeMonitor_start(unit, sds, frameNum);
}   /* end of rtk_sds_eyeMonitor_start */

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
int32
rtk_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_eyeParam_get(unit, sds, pParam);
}   /* end of rtk_sds_eyeParam_get */

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
int32
rtk_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_eyeParam_set(unit, sds, param);
}   /* end of rtk_sds_eyeParam_set */

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
int32
rtk_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_rxCaliConf_get(unit, sds, pConf);
}   /* end of rtk_sds_rxCaliConf_get */

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
int32
rtk_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->sds_rxCaliConf_set(unit, sds, conf);
}   /* end of rtk_sds_rxCaliConf_set */


