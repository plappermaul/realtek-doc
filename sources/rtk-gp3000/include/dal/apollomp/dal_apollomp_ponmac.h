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

#ifndef __DAL_APOLLOMP_PONMAC_H__
#define __DAL_APOLLOMP_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* Function Name:
 *      dal_apollomp_ponmac_init
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
dal_apollomp_ponmac_init(void);


/* Function Name:
 *      dal_apollomp_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);



/* Function Name:
 *      dal_apollomp_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      dal_apollomp_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue        - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_apollomp_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *	  state 	     - enable or disable
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_mode_set(rtk_ponmac_mode_t mode);

/* Function Name:
 *      dal_apollomp_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *pState       - mode state
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_mode_get(rtk_ponmac_mode_t *pMode);

/* Function Name:
 *      dal_apollomp_ponmac_queueDrainOut_set
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
dal_apollomp_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue);

/* Function Name:
 *      dal_apollomp_ponmac_serdesCdr_reset
 * Description:
 *      Serdes CDR reset by change polarity of RXSD
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
dal_apollomp_ponmac_serdesCdr_reset(void);

/* Function Name:
 *      dal_apollomp_ponmac_linkState_get
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
dal_apollomp_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync);

/* Function Name:
 *      dal_apollomp_ponmac_bwThreshold_set
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
dal_apollomp_ponmac_bwThreshold_set(uint32 bwThreshold, uint32 reqBwThreshold);

/* Function Name:
 *      dal_apollomp_ponmac_bwThreshold_get
 * Description:
 *      Get PON Port threshold
 * Input:
 *      N/A
 * Output:
 *		pBwThreshold    - PON BW_THRESHOLD
 *      pReqBwThreshold - PON REQ_BW_THRESHOLD
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_apollomp_ponmac_bwThreshold_get(uint32 *pBwThreshold, uint32 *pReqBwThreshold);

/* Function Name:
 *      dal_apollomp_ponmac_maxPktLen_set
 * Description:
 *      set pon port max packet length
 * Input:
 *      length         - max accept packet length
 *	  state 	     - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_maxPktLen_set(uint32 length);

/* Function Name:
 *      dal_apollomp_ponmac_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_apollomp_ponmac_losState_get(rtk_enable_t *pState);

/* Function Name:
 *      dal_apollomp_rtk_ponmac_txPll_relock
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
 *      None
 */
extern int32
dal_apollomp_rtk_ponmac_txPll_relock(void);
#endif /* __DAL_APOLLOMP_PONMAC_H__ */

