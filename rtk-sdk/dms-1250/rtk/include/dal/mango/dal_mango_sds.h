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
 * $Revision: 81289 $
 * $Date: 2017-08-15 20:45:44 +0800 (Tue, 15 Aug 2017) $
 *
 * Purpose : The data for setting MAC Serdes parameters
 *
 * Feature : The data struct of  MAC Serdes parameters
 *
 */

#ifndef __DAL_MANGO_SDS_H__
#define __DAL_MANGO_SDS_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/sds.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef enum dal_mango_sds_cmutype_e
{
    DAL_MANGO_SDS_CMU_LC,
    DAL_MANGO_SDS_CMU_RING,
    DAL_MANGO_SDS_CMU_END
} dal_mango_sds_cmutype_t;

/*
 * Macro Declaration
 */

/* Function Name:
 *      dal_mango_sds_leq_get
 * Description:
 *      Get SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pLeq - LEQ configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_mango_sds_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

/* Function Name:
 *      dal_mango_sds_leq_set
 * Description:
 *      Set SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - user SerDes id
 *      pLeq - LEQ configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_mango_sds_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

/* Function Name:
 *      dal_mango_sds_leq_adapt
 * Description:
 *      Set SerDes LEQ adapt
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_mango_sds_leq_adapt(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_mango_sds_testModeCnt_get
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt);

/* Function Name:
 *      dal_mango_sds_testMode_set
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode);

/* Function Name:
 *      dal_mango_sdsMapper_init
 * Description:
 *      Hook SerDes module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook SerDes module before calling any port APIs.
 */
extern int32
dal_mango_sdsMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_sds_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_mango_sds_init(uint32 unit);

/* Function Name:
 *      dal_mango_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      unit      - SerDes id
 * Output:
 *      info      - symbol error count information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

/* Function Name:
 *      dal_mango_sds_symErr_clear
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sds_symErr_clear(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_mango_sds_linkSts_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

/* Function Name:
 *      dal_mango_sds_xsgNwayEn_set
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en);

/* Function Name:
 *      dal_mango_sds_cmuBand_get
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band);

/* Function Name:
 *      dal_mango_sds_cmuBand_set
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val);

/* Function Name:
 *      dal_mango_sds_eyeMonitor_start
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum);

/* Function Name:
 *      dal_mango_sds_rst
 * Description:
 *      Reset Serdes.
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      dal_mango_sds_rx_rst
 * Description:
 *      Reset Serdes Rx and original patch are kept.
 * Input:
 *      unit    - unit id
 *      sds     - serdes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_OUT_OF_RANGE - Serdes index is not support.
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rx_rst(uint32  unit, uint32 sds);

/* Function Name:
 *      dal_mango_sds_rxCali
 * Description:
 *      execute SerDes rx calibration.
 * Input:
 *      unit          - unit id
 *      sds          - SerDes id
 *      retryCnt   - retry count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt);

/* Function Name:
 *      dal_mango_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to the sds enable status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliEnable_get(uint32 unit, uint32 sds, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_sds_rxCaliEnable_set
 * Description:
 *      Set the SerDes rx cali status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable    -  status of rx calibration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_sds_rxCaliDbgEnable_set
 * Description:
 *      Set debug msg status for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliDbgEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_mango_sds_rxCaliConf_get
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf);

/* Function Name:
 *      dal_mango_sds_rxCaliConf_set
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
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf);

/* Function Name:
 *      dal_mango_sds_rxCaliStatus_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
dal_mango_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      dal_mango_sds_info_get
 * Description:
 *      Get SerDes information
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 * Output:
 *      pInfo   - SerDes information
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
extern int32
dal_mango_sds_info_get(uint32 unit, uint32 sds, rtk_sds_info_t *pInfo);

/* Function Name:
 *      dal_mango_sds_eyeMonitorInfo_get
 * Description:
 *      Get eye monitor height and width
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 *      frameNum- frame number
 * Output:
 *      pInfo   - eye monitor information
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
extern int32
dal_mango_sds_eyeMonitorInfo_get(uint32 unit, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo);

/* Function Name:
 *      dal_mango_sds_loopback_get
 * Description:
 *      Get SerDes loopback status
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 * Output:
 *      pStatus - SerDes loopback status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
extern int32
dal_mango_sds_loopback_get(uint32 unit, uint32 sds, rtk_enable_t *pStatus);

/* Function Name:
 *      dal_mango_sds_loopback_set
 * Description:
 *      Set SerDes loopback status
 * Input:
 *      unit    - unit id
 *      sds     - user SerDis id
 *      status  - SerDes loopback status
 * Output:
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *      None
 */
extern int32
dal_mango_sds_loopback_set(uint32 unit, uint32 sds, rtk_enable_t status);

/* Function Name:
 *      dal_mango_sds_swChgSerdesMode
 * Description:
 *      MAC Serdes mode is linked to PHY SerDes mode.
 * Input:
 *      unit    - unit id
 *      port    - link changed port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
dal_mango_sds_swChgSerdesMode(uint32 unit, rtk_port_t port);

/* Function Name:
 *      dal_mango_sds_ctrl_get
 * Description:
 *      Get SerDes specific settings
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      ctrlType  - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_mango_sds_ctrl_get(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 *pValue);

/* Function Name:
 *      dal_mango_sds_ctrl_set
 * Description:
 *      Set SerDes specific settings
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      ctrlType  - setting type
 *      value     - setting value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_mango_sds_ctrl_set(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 value);

/* Function Name:
 *      dal_mango_sds_pcb_adapt
 * Description:
 *      Trigger SerDes pcb adapt
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_mango_sds_pcb_adapt(uint32 unit, uint32 sds);
#endif /* __DAL_MANGO_SDS_H__ */
