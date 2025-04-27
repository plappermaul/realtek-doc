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

#ifndef __DAL_LONGAN_SDS_H__
#define __DAL_LONGAN_SDS_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/sds.h>
#include <dal/dal_mapper.h>

typedef enum dal_longan_sds_rxCaliProcessSts_e
{
     DAL_LONGAN_SDS_RXCALI_PROCESS_STS_IDLE = 0,
     DAL_LONGAN_SDS_RXCALI_PROCESS_STS_PROCESS,
     DAL_LONGAN_SDS_RXCALI_PROCESS_STS_END
} dal_longan_sds_rxCaliProcessSts_t;

/* Function Name:
 *      dal_longan_sds_reg_get
 * Description:
 *      Get SerDes register through logic sds id
 * Input:
 *      unit    - unit id
 *      sds     - logic SerDes index
 *      page    - page
 *      reg     - register index/address
 * Output:
 *      pData   - register value
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
extern int32
dal_longan_sds_reg_get(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 *pData);

/* Function Name:
 *      dal_longan_sds_reg_set
 * Description:
 *      Set SerDes register through logic sds id
 * Input:
 *      unit    - unit id
 *      sds     - logic SerDes index
 *      page    - page
 *      reg     - register index/address
 *      data    - register value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support
 * Note:
 *      None
 */
extern int32
dal_longan_sds_reg_set(uint32 unit, uint32 sds, uint32 page, uint32 reg, uint32 data);

/* Function Name:
 *      dal_longan_sds_field_read
 * Description:
 *      Read SerDes field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 * Output:
 *      data - SerDes Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_longan_sds_field_read(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 *data);

/* Function Name:
 *      dal_longan_sds_field_write
 * Description:
 *      Write SerDes field through logic sds id
 * Input:
 *      unit - unit id
 *      sds  - logic SerDes id
 *      page - page number
 *      reg  - register
 *      endBit    - end bit of configure field
 *      startBit  - start bit of configure field
 *      data - SerDes Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_longan_sds_field_write(uint32 unit, uint32 sds, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 data);

/* Function Name:
 *      dal_longan_sds_leq_get
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
dal_longan_sds_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

/* Function Name:
 *      dal_longan_sds_leq_set
 * Description:
 *      Set SerDes LEQ
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      pLeq  - LEQ configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_longan_sds_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *leq);

/* Function Name:
 *      dal_longan_sds_leq_adapt
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
dal_longan_sds_leq_adapt(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_longan_sds_xsgNwayEn_set
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
dal_longan_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en);

/* Function Name:
 *      dal_longan_sds_cmuBand_get
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
dal_longan_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band);

/* Function Name:
 *      dal_longan_sds_cmuBand_set
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
dal_longan_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val);

/* Function Name:
 *      dal_longan_sds_testMode_set
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
dal_longan_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode);

/* Function Name:
 *      dal_longan_sds_eyeParam_get
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
 * Note:
 *      None
 */
extern int32
dal_longan_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam);

/* Function Name:
 *      dal_longan_sds_eyeParam_set
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
 * Note:
 *      None
 */
extern int32
dal_longan_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param);

/* Function Name:
 *      dal_longan_sds_linkSts_get
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
dal_longan_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

/* Function Name:
 *      dal_longan_sds_symErr_clear
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
dal_longan_sds_symErr_clear(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_longan_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
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
dal_longan_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

/* Function Name:
 *      dal_longan_sds_rx_rst
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
dal_longan_sds_rx_rst(uint32 unit, uint32 sdsId);

/* Function Name:
 *      dal_longan_sds_10gRxIdle_get
 * Description:
 *      Get the RxIdle for 10G Sds.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pRxIdle      - rxIdle information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_sds_10gRxIdle_get(uint32 unit, uint32 sds, uint32 *pRxIdle);

/* Function Name:
 *      dal_longan_sds_10gBerNotify_get
 * Description:
 *      Get the BerNotify for 10G Sds.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pBerNotify      - rxIdle information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_sds_10gBerNotify_get(uint32 unit, uint32 sds, uint32 *pBerNotify);

/* Function Name:
 *      dal_longan_sds_10gEyeParam_load
 * Description:
 *      Load 10g SerDes eye parameter
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
 dal_longan_sds_10gEyeParam_load(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_longan_sdsMapper_init
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
dal_longan_sdsMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_sds_init
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
dal_longan_sds_init(uint32 unit);

/* Function Name:
 *      dal_longan_sds_rxCaliRegDefault_init
 * Description:
 *      Initialize rx calibration registers to default value
 * Input:
 *      unit - unit ID
 *      sdsId - PHY SerDes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_longan_sds_rxCaliRegDefault_init(uint32 unit, uint32 sds);

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_sds_rxCaliStatus_clear(uint32 unit, uint32 sdsId);

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
 extern int32
dal_longan_sds_rxCaliStatus_get(uint32 unit, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      dal_longan_rxCaliProcessSts_get
 * Description:
 *      Get the SerDes rx calibration process status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  process status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_rxCaliProcessSts_get(uint32 unit, uint32 sdsId,dal_longan_sds_rxCaliProcessSts_t *pStatus);

/* Function Name:
 *      dal_longan_rxCaliProcessSts_set
 * Description:
 *      Set the SerDes rx calibration process status.
 * Input:
 *      unit      - unit id
 *      sds      - SerDes id
 *      sts       - rx calibration process status
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
dal_longan_rxCaliProcessSts_set(uint32 unit, uint32 sdsId, dal_longan_sds_rxCaliProcessSts_t status);

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_get
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
dal_longan_sds_rxCaliEnable_get(uint32 unit, uint32 sdsId, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_sds_rxCaliEnable_set
 * Description:
 *      Set the SerDes rx cali enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable  - enable status of rx calibration
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
dal_longan_sds_rxCaliEnable_set(uint32 unit, uint32 sdsId, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_sds_rxCaliConf_get
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
dal_longan_sds_rxCaliConf_get(uint32 unit, uint32 sdsId, rtk_sds_rxCaliConf_t *pConf);

/* Function Name:
 *      dal_longan_sds_rxCaliConf_set
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
dal_longan_sds_rxCaliConf_set(uint32 unit, uint32 sdsId, rtk_sds_rxCaliConf_t conf);

/* Function Name:
 *      dal_longan_sds_rxCali
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
dal_longan_sds_rxCali(uint32 unit, uint32 sdsId, uint32 retryCnt);

/* Function Name:
 *      dal_longan_sds_rxCali_debugEnable_set
 * Description:
 *      Enable debug msg for SerDes rx calibration
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
dal_longan_sds_rxCali_debugEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_longan_sds_clk_routine
 * Description:
 *      execute SerDes clock routine.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_longan_sds_clk_routine(uint32 unit);

/* Function Name:
 *      dal_longan_sds_linkFault_check
 * Description:
 *      Check the serdes link fault problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For 8G Serdes mode.
 */
extern int32
dal_longan_sds_linkFault_check(uint32 unit);

/* Function Name:
 *      dal_longan_sds_mode_get
 * Description:
 *      Get SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 * Output:
 *      pMode - pointer to serdes mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_longan_sds_mode_get(uint32 unit, uint32 sds, rt_serdesMode_t* pMode);

/* Function Name:
 *      dal_longan_sds_mode_set
 * Description:
 *      Set SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      mode - Serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
dal_longan_sds_mode_set(uint32 unit, uint32 sds, rt_serdesMode_t mode);

/* Function Name:
 *      _dal_longan_sds_10gSdsMode_force
 * Description:
 *      Force 10g SerDes mode
 * Input:
 *      unit - unit id
 *      sds  - SerDes id
 *      mode - Serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
extern int32
_dal_longan_sds_10gSdsMode_force(uint32 unit, uint32 sds, rtk_serdesMode_t mode);

/* Function Name:
 *      dal_longan_sds_macSds_set
 * Description:
 *      Set mac SerDes mode.
 * Input:
 *      unit    - unit id
 *      sds     - user SerDes id
 *      mode    - SerDes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only used for loader.
 */
extern int32
dal_longan_sds_macSds_set(uint32 unit, uint32 sds, rt_serdesMode_t mode);

/* Function Name:
 *      dal_longan_sds_loopback_get
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
dal_longan_sds_loopback_get(uint32 unit, uint32 sds, rtk_enable_t *pStatus);

/* Function Name:
 *      dal_longan_sds_loopback_set
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
dal_longan_sds_loopback_set(uint32 unit, uint32 sds, rtk_enable_t status);

/* Function Name:
 *      dal_longan_sds_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific serdes
 * Input:
 *      unit           - unit id
 *      sds            - SerDes id
 * Output:
 *      pEnable        - pointer to the enable status of fiber unidirection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_sds_fiberUnidirEnable_get(uint32 unit, uint32 sds, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_sds_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific serdes
 * Input:
 *      unit           - unit id
 *      sds            - SerDes id
 *      enable         - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_sds_fiberUnidirEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_sds_ctrl_get
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
dal_longan_sds_ctrl_get(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 *pValue);

/* Function Name:
 *      dal_longan_sds_ctrl_set
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
dal_longan_sds_ctrl_set(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 value);

#endif /* __DAL_LONGAN_SDS_H__ */
