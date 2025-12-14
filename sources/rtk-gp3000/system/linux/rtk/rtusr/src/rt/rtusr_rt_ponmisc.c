/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON MISC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <rtk/rt/rt_ponmisc.h>

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
 *      rt_ponmisc_init
 * Description:
 *      Initialize pon misc interface.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rt_ponmisc_init(void)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_INIT, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_ponmisc_init */

/* Function Name:
 *      rt_ponmisc_modeSpeed_get
 * Description:
 *      get pon misc mode speed inforamtion.
 * Input:
 *      None
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
int32
rt_ponmisc_modeSpeed_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPonSpeed), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.ponSpeed, pPonSpeed, sizeof(rt_ponmisc_ponSpeed_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_MODESPEED_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPonMode, &rt_ponmisc_cfg.ponMode, sizeof(rt_ponmisc_ponMode_t));
    osal_memcpy(pPonSpeed, &rt_ponmisc_cfg.ponSpeed, sizeof(rt_ponmisc_ponSpeed_t));

    return RT_ERR_OK;
}   /* end of rt_ponmisc_modeSpeed_get */

/* Function Name:
 *      rt_ponmisc_modeSpeed_set
 * Description:
 *      set pon misc mode and speed inforamtion.
 * Input:
 *      ponMode    - PON mode
 *      ponSpeed   - PON speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      
 */
int32
rt_ponmisc_modeSpeed_set(rt_ponmisc_ponMode_t ponMode,rt_ponmisc_ponSpeed_t ponSpeed)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.ponMode, &ponMode, sizeof(rt_ponmisc_ponMode_t));
    osal_memcpy(&rt_ponmisc_cfg.ponSpeed, &ponSpeed, sizeof(rt_ponmisc_ponSpeed_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_MODESPEED_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_ponmisc_modeSpeed_set */

/* Function Name:
 *      rt_ponmisc_sid_get
 * Description:
 *      get pon misc sid inforamtion.
 * Input:
 *      sid         - stream id
 * Output:
 *      pSidInfo    - corresponding sid infotmation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT          - Invalid input parameters.
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      
 */
int32
rt_ponmisc_sid_get(uint16 sid,rt_ponmisc_sidInfo_t *pSidInfo)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSidInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.sid, &sid, sizeof(uint16));
    GETSOCKOPT(RTDRV_RT_PONMISC_SID_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pSidInfo, &rt_ponmisc_cfg.sidInfo, sizeof(rt_ponmisc_sidInfo_t));

    return RT_ERR_OK;
}   /* end of rt_ponmisc_sid_get */

/* Function Name:
 *      rt_ponmisc_sid_set
 * Description:
 *      set pon misc sid inforamtion.
 * Input:
 *      sid         - stream id
 *      sidInfo     - corresponding sid infotmation
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      
 */
int32
rt_ponmisc_sid_set(uint16 sid,rt_ponmisc_sidInfo_t sidInfo)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.sid, &sid, sizeof(uint16));
    osal_memcpy(&rt_ponmisc_cfg.sidInfo, &sidInfo, sizeof(rt_ponmisc_sidInfo_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_SID_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_ponmisc_sid_set */

/* Function Name:
*      rt_ponmisc_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pData           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_transceiver_get(rt_transceiver_parameter_type_t type, rt_transceiver_data_t *pData)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.trxDataType, &type, sizeof(rt_transceiver_parameter_type_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_TRX_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pData, &rt_ponmisc_cfg.trxData, sizeof(rt_transceiver_data_t));

    return RT_ERR_OK;
}   /* end of rt_ponmisc_transceiver_get */


/* Function Name:
*       rt_ponmisc_transceiver_tx_power_scale_set
* Description:
*       Set transceiver tx power scale 
* Input:
*       scale   - scale for TX output power with LSB (in unit 0.1 uW)
* Output:
*       N/A
* Return:
*       RT_ERR_OK   
*       others  
* Note:
*
*/
int32
rt_ponmisc_transceiver_tx_power_scale_set(uint32 scale)
{
       rtdrv_rt_ponmiscCfg_t ponmisc_cfg;
       osal_memset(&ponmisc_cfg, 0x0, sizeof(rtdrv_rt_ponmiscCfg_t));
       ponmisc_cfg.trxData.tx_pwr_scale = scale;
       SETSOCKOPT(RTDRV_RT_PONMISC_TRANSCEIVER_TX_PWR_SCALE_SET, &ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
       return RT_ERR_OK;

}

/* Function Name:
*      rt_ponmisc_burstPolarityReverse_get
* Description:
*      Get the status of PON burst polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of burst polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_burstPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.burstPolarity, pPolarity, sizeof(rt_ponmisc_polarity_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_BURSTPOLARITYREVERSE_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPolarity, &rt_ponmisc_cfg.burstPolarity, sizeof(rt_ponmisc_polarity_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_burstPolarityReverse_set
* Description:
*      Set the status of PON burst polarity reverse
* Input:
*      polarity        - the burst polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_burstPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.burstPolarity, &polarity, sizeof(rt_ponmisc_polarity_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_BURSTPOLARITYREVERSE_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_rxlosPolarityReverse_get
* Description:
*      Get the status of PON rx_los polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of rx_los polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_rxlosPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.rxlosPolarity, pPolarity, sizeof(rt_ponmisc_polarity_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_RXLOSPOLARITYREVERSE_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPolarity, &rt_ponmisc_cfg.rxlosPolarity, sizeof(rt_ponmisc_polarity_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_rxlosPolarityReverse_set
* Description:
*      Set the status of PON rx_los polarity reverse
* Input:
*      polarity        - the rx_los polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_rxlosPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;
    memset(&rt_ponmisc_cfg, 0x0, sizeof(rtdrv_rt_ponmiscCfg_t));


    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.rxlosPolarity, &polarity, sizeof(rt_ponmisc_polarity_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_RXLOSPOLARITYREVERSE_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forceLaserState_get
* Description:
*      Get Force Laser status
* Input:
*      none
* Output:
*      pStatus       - pointer of Force Laser status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forceLaserState_get(rt_ponmisc_laser_status_t *pStatus)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.laserStatus, pStatus, sizeof(rt_ponmisc_laser_status_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_FORCELASER_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pStatus, &rt_ponmisc_cfg.laserStatus, sizeof(rt_ponmisc_laser_status_t));


    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forceLaserState_set
* Description:
*      Set Force Laser status
* Input:
*      status       - Force Laser status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forceLaserState_set(rt_ponmisc_laser_status_t status)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.laserStatus, &status, sizeof(rt_ponmisc_laser_status_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_FORCELASER_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forcePRBS_get
* Description:
*      Get the PRBS config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_get(rt_ponmisc_prbs_t *pPrbsCfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.prbs, pPrbsCfg, sizeof(rt_ponmisc_prbs_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_FORCEPRBS_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPrbsCfg, &rt_ponmisc_cfg.prbs, sizeof(rt_ponmisc_prbs_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forcePRBS_set
* Description:
*      Set the PRBS config
* Input:
*      none
* Output:
*      prbsCfg       - PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_set(rt_ponmisc_prbs_t prbsCfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    osal_memcpy(&rt_ponmisc_cfg.prbs, &prbsCfg, sizeof(rt_ponmisc_prbs_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_FORCEPRBS_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forcePRBS_rx_get
* Description:
*      Get the PRBS RX config
* Input:
*      none
* Output:
*      pPrbsRxCfg       - pointer of PRBS RX config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_rx_get(rt_ponmisc_prbs_t *pPrbsRxCfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsRxCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.prbs_rx, pPrbsRxCfg, sizeof(rt_ponmisc_prbs_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_FORCEPRBS_RX_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPrbsRxCfg, &rt_ponmisc_cfg.prbs_rx, sizeof(rt_ponmisc_prbs_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_forcePRBS_rx_set
* Description:
*      Set the PRBS RX config
* Input:
*      prbsCfg       - PRBS RX config
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_forcePRBS_rx_set(rt_ponmisc_prbs_t prbsRxCfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;
    memset(&rt_ponmisc_cfg, 0x0, sizeof(rtdrv_rt_ponmiscCfg_t));


    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.prbs_rx, &prbsRxCfg, sizeof(rt_ponmisc_prbs_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_FORCEPRBS_RX_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_prbsRxStatus_get
* Description:
*      Get the PRBS RX Status
* Input:
*      none
* Output:
*      pPrbsRxStatus       - pointer of PRBS RX Status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_prbsRxStatus_get(rt_ponmisc_prbsRxStatus_t *pPrbsRxStatus)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrbsRxStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.prbs_rx_status, pPrbsRxStatus, sizeof(rt_ponmisc_prbsRxStatus_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_PRBS_RX_STATUS_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pPrbsRxStatus, &rt_ponmisc_cfg.prbs_rx_status, sizeof(rt_ponmisc_prbsRxStatus_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_selfRogue_cfg_get
* Description:
*      Get the self rogue detect config
* Input:
*      none
* Output:
*      pCfg       - pointer of self rogue detect config
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
*      RT_ERR_OUT_OF_RANGE  - value out of range
* Note:
*
*/
int32
rt_ponmisc_selfRogue_cfg_get(rtk_enable_t *pCfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.cfg, pCfg, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_SELFROGUE_CFG_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pCfg, &rt_ponmisc_cfg.cfg, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_selfRogue_cfg_set
* Description:
*      Set the self rogue detect config
* Input:
*      cfg       - self rogue detect config
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_selfRogue_cfg_set(rtk_enable_t cfg)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;
    memset(&rt_ponmisc_cfg, 0x0, sizeof(rtdrv_rt_ponmiscCfg_t));

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.cfg, &cfg, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_SELFROGUE_CFG_SET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_selfRogue_count_get
* Description:
*      Get the self rogue detect counter
* Input:
*      none
* Output:
*      pCnt       - pointer of self rogue detect counter
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
* Note:
*
*/
int32
rt_ponmisc_selfRogue_count_get(rt_ponmisc_rogue_cnt_t *pCnt)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_ponmisc_cfg.cfg, pCnt, sizeof(rt_ponmisc_rogue_cnt_t));
    GETSOCKOPT(RTDRV_RT_PONMISC_SELFROGUE_CNT_GET, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);
    osal_memcpy(pCnt, &rt_ponmisc_cfg.cnt, sizeof(rt_ponmisc_rogue_cnt_t));

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_selfRogue_count_clear
* Description:
*      Clear the self rogue detect counter
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
rt_ponmisc_selfRogue_count_clear(void)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_SELFROGUE_CNT_CLEAR, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
*      rt_ponmisc_dying_gasp_force_trigger
* Description:
*      force trigger dying gasp signal
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
* Note:
*
*/
int32
rt_ponmisc_dying_gasp_force_trigger(void)
{
    rtdrv_rt_ponmiscCfg_t rt_ponmisc_cfg;

    /* function body */
    osal_memset(&rt_ponmisc_cfg, 0, sizeof(rtdrv_rt_ponmiscCfg_t));
    SETSOCKOPT(RTDRV_RT_PONMISC_DYINGGASP_FORCE_TRIGGER, &rt_ponmisc_cfg, rtdrv_rt_ponmiscCfg_t, 1);

    return RT_ERR_OK;
}

