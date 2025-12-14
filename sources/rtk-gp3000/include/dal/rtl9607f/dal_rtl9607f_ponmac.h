/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
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
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */


#ifndef __DAL_RTL9607F_PONMAC_H__
#define __DAL_RTL9607F_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>

/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */


/*
 * Function Declaration
 */
/*
 * Macro Declaration
 */
/* Function Name:
 *      dal_rtl9607f_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_init(void);

/* Function Name:
 *      dal_rtl9607f_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_losState_get(rtk_enable_t *pState);

/* Function Name:
 *      dal_rtl9607f_ponmac_serdesCdr_reset
 * Description:
 *      Serdes CDR reset
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_serdesCdr_reset(void);

/* Function Name:
 *      dal_ca9607f_ponmac_linkState_get
 * Description:
 *      check SD and Sync state of GPON/EPON
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
dal_ca9607f_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync);

/* Function Name:
 *      dal_rtl9607f_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);

/* Function Name:
 *      dal_rtl9607f_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);

/* Function Name:
 *      dal_rtl9607f_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);

/* Function Name:
 *      dal_rtl9607f_ponmac_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is Kbps.
 */
extern int32
dal_rtl9607f_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate);

/* Function Name:
 *      dal_rtl9607f_ponmac_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in rtl9607f is 1Kbps.
 */
extern int32
dal_rtl9607f_ponmac_egrBandwidthCtrlRate_set(uint32 rate);

/* Function Name:
 *      dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      get the pon port egress bandwidth control IFG state.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - pointer of state of IFG 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude);

/* Function Name:
 *      dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      set the pon port egress bandwidth control IFG state.
 * Input:
 *      ifgInclude - state of IFG 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude);

/* Function Name:
 *      dal_rtl9607f_ponmac_txDisableGpio_get
 * Description:
 *      get the pon module tx disable state.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_txDisableGpio_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9607f_ponmac_txDisableGpio_set
 * Description:
 *      set the pon module tx disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_txDisableGpio_set(rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9607f_ponmac_txPowerDisableGpio_get
 * Description:
 *      get the pon module tx power disable state.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9607f_ponmac_txPowerDisableGpio_set
 * Description:
 *      set the pon module tx power disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_txPowerDisableGpio_set(rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9607f_ponmac_trxSwitchGpio_set
 * Description:
 *      set the gpio state for switching transceiver module.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rtl9607f_ponmac_trxSwitchGpio_set(rtk_enable_t enable);

#endif /* __DAL_RTL9607F_PONMAC_H__ */
