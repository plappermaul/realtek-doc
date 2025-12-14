/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) queue configuration (PIR/CIR/Queue schuedule type)
 *           (2) flow and queue mapping
 */

#ifndef __RTK_PONMAC_H__
#define __RTK_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */

#define GPON_MAC_MODE 1
#define TRANSCEIVER_LEN 24
#ifdef CONFIG_OE_MODULE_I2C_PORT_1
#define TRANSCEIVER_PORT 1
#else
#define TRANSCEIVER_PORT 0
#endif
#define TRANSCEIVER_A0 0x50
#define TRANSCEIVER_A2 0x51

typedef struct rtk_ponmac_queueCfg_s
{
    uint32 cir;
    uint32 pir;
    rtk_qos_queue_type_t type;
    uint32 weight;
    rtk_enable_t egrssDrop;
} rtk_ponmac_queueCfg_t;


typedef struct rtk_ponmac_queue_s
{
    uint32 schedulerId;
    uint32 queueId;/*0~31*/
} rtk_ponmac_queue_t;


typedef struct rtk_transceiver_data_s{
    uint8 buf[TRANSCEIVER_LEN];
	uint32 tx_pwr_scale;
}rtk_transceiver_data_t;

typedef enum rtk_transceiver_patameter_type_e{
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME = 0,
    RTK_TRANSCEIVER_PARA_TYPE_SN,
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM,
    RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE,
    RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE,
    RTK_TRANSCEIVER_PARA_TYPE_BIAS_CURRENT,
    RTK_TRANSCEIVER_PARA_TYPE_TX_POWER,
    RTK_TRANSCEIVER_PARA_TYPE_RX_POWER,
    RTK_TRANSCEIVER_PARA_TYPE_MAX
}rtk_transceiver_parameter_type_t;

typedef enum rtk_ponmac_mode_s
{
    PONMAC_MODE_GPON=0,
    PONMAC_MODE_EPON,
    PONMAC_MODE_FIBER_1G,
    PONMAC_MODE_FIBER_100M,
    PONMAC_MODE_FIBER_AUTO,
    PONMAC_MODE_FIBER_FORCE_1G,
    PONMAC_MODE_FIBER_FORCE_100M,
    PONMAC_MODE_SDS_SGMII_1G,
    PONMAC_MODE_END
}rtk_ponmac_mode_t;

typedef enum rtk_ponmac_polarity_s
{
    PONMAC_POLARITY_LOW=0,
    PONMAC_POLARITY_HIGH,
    PONMAC_POLARITY_END
}rtk_ponmac_polarity_t;

typedef enum rtk_ponmac_aggregate_tcont_s
{
    PONMAC_AGG_TCONT0_3=0,
    PONMAC_AGG_TCONT4_7,
    PONMAC_AGG_TCONT8_11,
    PONMAC_AGG_TCONT12_15,
    PONMAC_AGG_TCONT_END
}rtk_ponmac_aggregate_tcont_t;
/*
 * Macro Declaration
 */


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* Function Name:
 *      rtk_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_init(void);



/* Function Name:
 *      rtk_ponmac_switchBufferMode_set
 * Description:
 *      Configure PON MAC switch buffer setting 
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_switchBufferMode_set(rtk_flowctrl_patch_t patch_type);


/* Function Name:
 *      rtk_ponmac_queue_add
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
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      rtk_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      rtk_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      rtk_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue       - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      rtk_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      rtk_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *      state        - disable or enable
 * Output:
 *
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_mode_set(rtk_ponmac_mode_t mode);


/* Function Name:
 *      rtk_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *state       - point of state, enable or disable
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_ponmac_mode_get(rtk_ponmac_mode_t *pMode);

/* Function Name:
 *      rtk_ponmac_queueDrainOut_set
 * Description:
 *      Set pon queue drain out.
 * Input:
 *      pQueue - Specified which PON queue will be drain out
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue);

/* Function Name:
 *      rtk_ponmac_opticalPolarity_get
 * Description:
 *      Get the current optical output polarity
 * Input:
 *      None
 * Output:
 *      pPolarity  - the current output polarity
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity);

/* Function Name:
 *      rtk_ponmac_opticalPolarity_set
 * Description:
 *      Set optical output polarity
 * Input:
 *      polarity - the optical output polarity
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity);

/* Function Name:
 *      rtk_ponmac_losState_get
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
rtk_ponmac_losState_get(rtk_enable_t *pEnable);

/* Function Name:
*      rtk_ponmac_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pData           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK       - successful
*      others          - fail
* Note:
*
*/
extern int32
rtk_ponmac_transceiver_get(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pData);

/* Function Name:
*		rtk_ponmac_transceiver_tx_power_scale_set
* Description:
*		Set transceiver tx power scale 
* Input:
*		scale	- scale for TX output power with LSB (in unit 0.1 uW)
* Output:
*		N/A
* Return:
*		RT_ERR_OK	
*		others	
* Note:
*
*/
extern int32
rtk_ponmac_transceiver_tx_power_scale_set(uint32 scale);

/* Function Name:
 *      rtk_ponmac_serdesCdr_reset
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
rtk_ponmac_serdesCdr_reset(void);

/* Function Name:
 *      rtk_ponmac_linkState_get
 * Description:
 *      check SD and Sync state of GPON/EPON
 * Input:
 *      mode - GPON or EPON mode
 * Output:
 *      pSd     - pointer of signal detect
 *      pSync   - pointer of sync state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync);


/* Function Name:
 *      rtk_ponmac_forceBerNotify_set
 * Description:
 *      force Ber Notify set
 * Input:
 *      state: force or un-force Ber Notify
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_forceBerNotify_set(rtk_enable_t state);

/* Function Name:
 *      rtk_ponmac_bwthreshold_set
 * Description:
 *      Set PON Port threshold
 * Input:
 *      bwThreshold    - PON BW_THRESHOLD
 *      reqBwThreshold - PON REQ_BW_THRESHOLD
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_bwThreshold_set(uint32 bwThreshold, uint32 reqBwThreshold);

/* Function Name:
 *      rtk_ponmac_bwThreshold_get
 * Description:
 *      Get PON Port threshold
 * Input:
 *      N/A
 * Output:
 *      pBwThreshold    - PON BW_THRESHOLD
 *      pReqBwThreshold - PON REQ_BW_THRESHOLD
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_bwThreshold_get(uint32 *pBwThreshold, uint32 *pReqBwThreshold);

/* Function Name:
 *      rtk_ponmac_maxPktLen_set
 * Description:
 *      set pon port max packet length
 * Input:
 *      length       - max accept packet length
 *    state          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32
rtk_ponmac_maxPktLen_set(uint32 length);

/* Function Name:
 *      rtk_ponmac_sidValid_get
 * Description:
 *      Get sid valid
 * Input:
 *      sid     - sid
 * Output:
 *      pValid  - the current sid valid
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_sidValid_get(uint32 sid, uint32 *pValid);

/* Function Name:
 *      rtk_ponmac_sidValid_set
 * Description:
 *      Set sid valit
 * Input:
 *      sid     - sid
 *      valid   - valid or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_sidValid_set(uint32 sid, uint32 valid);



/* Function Name:
 *      rtk_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType);


/* Function Name:
 *      rtk_ponmac_schedulingType_set
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      queueType  - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType);


/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_get
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
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
extern int32
rtk_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate);

/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_set
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
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
extern int32
rtk_ponmac_egrBandwidthCtrlRate_set(uint32 rate);



/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_get
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      scheduleId - T-cont is or LLID index
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
extern int32
rtk_ponmac_egrScheduleIdRate_get(uint32 scheduleId,uint32 *pRate);

/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      scheduleId - T-cont is or LLID index
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
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
extern int32
rtk_ponmac_egrScheduleIdRate_set(uint32 scheduleId,uint32 rate);

/* Function Name:
 *      rtk_ponmac_txPll_relock
 * Description:
 *      Re-lock tx PLL by toggle CMU_EN_GPHY.
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
rtk_ponmac_txPll_relock(void);


/* Function Name:
 *      rtk_ponmac_synce_get
 * Description:
 *      Get the current optical synce state
 * Input:
 *      None
 * Output:
 *      enable  - the current optical synce state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_ponmac_synce_get(rtk_enable_t *enable);

/* Function Name:
 *      rtk_ponmac_synce_set
 * Description:
 *      Set the current optical synce state
 * Input:
 *      enable  - the current optical synce state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_synce_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlIncludeIfg_get
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
rtk_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude);

/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlIncludeIfg_set
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
rtk_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude);

/* Function Name:
 *      rtk_ponmac_egrScheduleIdIncludeIfg_get
 * Description:
 *      get the tcont egress bandwidth control IFG state.
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
rtk_ponmac_egrScheduleIdIncludeIfg_get(rtk_enable_t *pIfgInclude);

/* Function Name:
 *      rtk_ponmac_egrScheduleIdIncludeIfg_set
 * Description:
 *      set the tcont egress bandwidth control IFG state.
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
rtk_ponmac_egrScheduleIdIncludeIfg_set(rtk_enable_t ifgInclude);


/* Function Name:
 *      rtk_ponmac_txDisGpioId_get
 * Description:
 *      Get PON TxDis GPIO PIN id
 * Input:
 *      N/A
 * Output:
 *      gpioPinId    - gpio pin id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_txDisGpioId_get(uint32 *gpioPinId);


/* Function Name:
 *      rtk_ponmac_txPowerGpioId_get
 * Description:
 *      Get PON Tx Power GPIO PIN id
 * Input:
 *      N/A
 * Output:
 *      gpioPinId    - gpio pin id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_txPowerGpioId_get(uint32 *gpioPinId);


/* Function Name:
 *      rtk_ponmac_aggTcontState_get
 * Description:
 *      get the aggregate tcont state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pState - pointer of state of aggregate tcont 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_aggTcontState_get(rtk_ponmac_aggregate_tcont_t aggIndex, rtk_enable_t *pState);


/* Function Name:
 *      rtk_ponmac_aggTcontState_set
 * Description:
 *      set the aggregate tcont state.
 * Input:
 *      aggIndex - aggregate tcont index
 *      state - state of aggregate tcont
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_aggTcontState_set(rtk_ponmac_aggregate_tcont_t aggIndex, rtk_enable_t state);

/* Function Name:
 *      rtk_ponmac_txDisableGpio_get
 * Description:
 *      get the pon module tx disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_txDisableGpio_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_ponmac_txDisableGpio_set
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
rtk_ponmac_txDisableGpio_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_ponmac_txPowerDisableGpio_get
 * Description:
 *      get the pon module tx power disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_ponmac_txPowerDisableGpio_set
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
rtk_ponmac_txPowerDisableGpio_set(rtk_enable_t enable);
#endif /* __RTK_PONMAC_H__ */

