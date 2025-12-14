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

#ifndef __DAL_RTL9601B_PONMAC_H__
#define __DAL_RTL9601B_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */
/* Define Serdes index */
#define RTL9601B_SDS_IDX_LAN                0x00
#define RTL9601B_SDS_IDX_PON                0x01

/* Define Serdes pages */
/* Digital Serdes */
#define RTL9601B_SDS_PAGE_SDS               0x00
#define RTL9601B_SDS_PAGE_SDS_EXT           0x01
#define RTL9601B_SDS_PAGE_FIB               0x02
#define RTL9601B_SDS_PAGE_FIB_EXT           0x03
#define RTL9601B_SDS_PAGE_TGR_STD_0         0x04
#define RTL9601B_SDS_PAGE_TGR_STD_1         0x05
#define RTL9601B_SDS_PAGE_TGR_PRO_0         0x06
#define RTL9601B_SDS_PAGE_TGR_PRO_1         0x07
#define RTL9601B_SDS_PAGE_TGX_STD_0         0x08
#define RTL9601B_SDS_PAGE_TGX_STD_1         0x09
#define RTL9601B_SDS_PAGE_TGX_PRO_0         0x0A
#define RTL9601B_SDS_PAGE_TGX_PRO_1         0x0B
#define RTL9601B_SDS_PAGE_WDIG              0x1F
/* Analog Serdes */
#define RTL9601B_SDS_PAGE_MISC              0x20
#define RTL9601B_SDS_PAGE_COMMON            0x21
#define RTL9601B_SDS_PAGE_SPEED             0x22
#define RTL9601B_SDS_PAGE_SPEED_EXT         0x23
#define RTL9601B_SDS_PAGE_125G_SPEED        0x24    /* 1.25G Speed */
#define RTL9601B_SDS_PAGE_125G_SPEED_EXT    0x25    /* 1.25G Speed Ext */
#define RTL9601B_SDS_PAGE_25G_SPEED         0x26    /* 2.5G Speed */
#define RTL9601B_SDS_PAGE_25G_SPEED_EXT     0x27    /* 2.5G Speed Ext */
#define RTL9601B_SDS_PAGE_3125G_SPEED       0x28    /* 3.125G Speed */
#define RTL9601B_SDS_PAGE_3125G_SPEED_EXT   0x29    /* 3.125G Speed Ext */
#define RTL9601B_SDS_PAGE_5G_SPEED          0x2A    /* 5G Speed */
#define RTL9601B_SDS_PAGE_5G_SPEED_EXT      0x2B    /* 5G Speed Ext */
#define RTL9601B_SDS_PAGE_625G_SPEED        0x2C    /* 6.25G Speed */
#define RTL9601B_SDS_PAGE_625G_SPEED_EXT    0x2D    /* 6.25G Speed Ext */
#define RTL9601B_SDS_PAGE_10G_SPEED         0x2E    /* 10G Speed */
#define RTL9601B_SDS_PAGE_10G_SPEED_EXT     0x2F    /* 10G Speed Ext */
#define RTL9601B_SDS_PAGE_GPON_SPEED        0x30    /* GPON Speed */
#define RTL9601B_SDS_PAGE_GPON_SPEED_EXT    0x31    /* GPON Speed Ext */
#define RTL9601B_SDS_PAGE_EPON_GSPEED       0x32    /* EPON Speed */
#define RTL9601B_SDS_PAGE_EPON_SPEED_EXT    0x33    /* EPON Speed Ext */

#define RTL9601B_SDS_ACCESS_TIMEOUT         0x0010

typedef struct rtl9601b_sds_acc_s {
    uint32 reserved:16;
    uint32 index:5;
    uint32 page:6;
    uint32 regaddr:5;
}rtl9601b_sds_acc_t;

typedef enum rtl9601b_ponmac_draintOutState_e
{
	RTL9601B_PONMAC_DRAINOUT_FIN = 0,
	RTL9601B_PONMAC_DRAINOUT_DOING,
	RTL9601B_PONMAC_DRAINOUT_END
}rtl9601b_ponmac_draintOutState_t;

/*
 * Macro Declaration
 */

/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* Function Name:
 *      dal_rtl9601b_ponmac_init
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
dal_rtl9601b_ponmac_init(void);


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_add
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
dal_rtl9601b_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);



/* Function Name:
 *      dal_rtl9601b_ponmac_queue_get
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
dal_rtl9601b_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_del
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
dal_rtl9601b_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_set
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
dal_rtl9601b_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_get
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
dal_rtl9601b_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_mode_set
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
dal_rtl9601b_ponmac_mode_set(rtk_ponmac_mode_t mode);

/* Function Name:
 *      dal_rtl9601b_ponmac_mode_get
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
dal_rtl9601b_ponmac_mode_get(rtk_ponmac_mode_t *pMode);


/* Function Name:
 *      dal_rtl9601b_ponmac_queueDrainOut_set
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
dal_rtl9601b_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_get
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
dal_rtl9601b_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity);


/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_set
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
dal_rtl9601b_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity);


/* Function Name:
 *      dal_rtl9601b_ponmac_losState_get
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
dal_rtl9601b_ponmac_losState_get(rtk_enable_t *pState);

extern int _rtl9601b_serdes_ind_write(rtl9601b_sds_acc_t sds_acc, uint16 data);
extern int _rtl9601b_serdes_ind_read(rtl9601b_sds_acc_t sds_acc, uint16 *data);

/* Function Name:
 *      dal_rtl9601b_ponmac_serdesCdr_reset
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
dal_rtl9601b_ponmac_serdesCdr_reset(void);

/* Function Name:
 *      dal_rtl9601b_ponmac_linkState_get
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
dal_rtl9601b_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync);

/* Function Name:
 *      dal_rtl9601b_ponmac_forceBerNotify_set
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
dal_rtl9601b_ponmac_forceBerNotify_set(rtk_enable_t state);

/* Function Name:
 *      dal_rtl9601b_ponmac_maxPktLen_set
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
dal_rtl9601b_ponmac_maxPktLen_set(uint32 length);

/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
dal_rtl9601b_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType);


/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_set
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      queueType  - the WFQ schedule type 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
dal_rtl9601b_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType);


/* Function Name:
 *      dal_rtl9601b_ponmac_egrScheduleIdRate_get
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
dal_rtl9601b_ponmac_egrScheduleIdRate_get(uint32 scheduleId,uint32 *pRate);





/* Function Name:
 *      dal_rtl9601b_ponmac_egrScheduleIdRate_set
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
dal_rtl9601b_ponmac_egrScheduleIdRate_set(uint32 scheduleId,uint32 rate);


#endif /* __DAL_RTL9601B_PONMAC_H__ */

