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
 * $Revision: 
 * $Date$
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


#ifndef __RTK_INTR_H__
#define __RTK_INTR_H__


/*
 * Include Files
 */

/*
 * Symbol Definition
 */
typedef enum rtk_intr_polarity_e
{
	INTR_POLAR_HIGH = 0,
	INTR_POLAR_LOW,

	INTR_POLAR_END
} rtk_intr_polarity_t;


typedef enum rtk_intr_status_e
{
	INTR_STATUS_SPEED_CHANGE = 0,
	INTR_STATUS_LINKUP,
	INTR_STATUS_LINKDOWN,
	INTR_STATUS_GPHY,

	INTR_STATUS_END
} rtk_intr_status_t;



typedef enum rtk_intr_type_e
{
	INTR_TYPE_LINK_CHANGE = 0,
	INTR_TYPE_METER_EXCEED,
	INTR_TYPE_LEARN_OVER,
	INTR_TYPE_SPEED_CHANGE,
	INTR_TYPE_SPECIAL_CONGEST,
	INTR_TYPE_LOOP_DETECTION,
	INTR_TYPE_CABLE_DIAG_FIN,
	INTR_TYPE_ACL_ACTION,
	INTR_TYPE_GPHY,
	INTR_TYPE_SERDES,
	INTR_TYPE_GPON,
	INTR_TYPE_EPON,
	INTR_TYPE_PTP,
	INTR_TYPE_DYING_GASP,
	INTR_TYPE_THERMAL,
	INTR_TYPE_ADC,
	INTR_TYPE_EEPROM_UPDATE_110OR118,
	INTR_TYPE_EEPROM_UPDATE_128TO247,
	INTR_TYPE_PKTBUFFER_ERROR,
	INTR_TYPE_DEBUG,
	INTR_TYPE_SMARTCARD,
	INTR_TYPE_SWITCH_ERROR,
	INTR_TYPE_SFP,
	INTR_TYPE_FB_EVENT,
	INTR_TYPE_TOD,
	INTR_TYPE_TOD_UPDATE,
	INTR_TYPE_LOS,
	INTR_TYPE_LOF,
	INTR_TYPE_ALL,
	/*endof rtk_intr_type*/
	INTR_TYPE_END
} rtk_intr_type_t;

#define IS_VALID_INTERRUPT_TYPE(type)       ((validIntr != NULL) ? (validIntr(type)) : 0)


/*
 * Data Declaration
 */
extern uint32 (*validIntr)(uint32);

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_intr_init
 * Description:
 *      Initialize interrupt module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize interrupt module before calling any interrupt APIs.
 */
extern int32
rtk_intr_init(void);


/* Function Name:
 *      rtk_intr_polarity_set
 * Description:
 *      Set interrupt polarity mode
 * Input:
 *      mode - Interrupt polarity mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK		- OK
 *      RT_ERR_FAILED	- Failed
 *      RT_ERR_INPUT	- Invalid input parameters.
 * Note:
 *      The API can set Interrupt polarity mode.
 *      The modes that can be set as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
extern int32 
rtk_intr_polarity_set(rtk_intr_polarity_t polar);


/* Function Name:
 *      rtk_intr_polarity_mode_get
 * Description:
 *      Get Interrupt polarity mode
 * Input:
 *      None
 * Output:
 *      pMode - Interrupt polarity mode
 * Return:
 *      RT_ERR_OK      	- OK
 *      RT_ERR_FAILED	- Failed
 *      RT_ERR_INPUT 	- Invalid input parameters.
 * Note:
 *      The API can get Interrupt polarity mode.
 *	  The modes that can be got as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
extern int32 
rtk_intr_polarity_get(rtk_intr_polarity_t *pPolar);


/* Function Name:
 *      rtk_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *	   None.	
 */
extern int32 
rtk_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable);


/* Function Name:
 *      rtk_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return  status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
 extern int32 
 rtk_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pState        - pointer of return status of mask (for SW_INTR_TYPE_ALL) or 
 * 				  enable (for specific inter type value)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
rtk_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pStatus);


/* Function Name:
 *      rtk_intr_ims_clear
 * Description:
 *      Clear interrupt status.
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
rtk_intr_ims_clear(rtk_intr_type_t intr);


/* Function Name:
 *      rtk_intr_speedChangeStatus_get
 * Description:
 *      Get interrupt status of speed change.
 * Input:
 *      pPortMask          - pointer of return port status (bitmask)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
rtk_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      rtk_intr_speedChangeStatus_clear
 * Description:
 *      Clear interrupt status of port speed change.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_speedChangeStatus_clear(void);


/* Function Name:
 *      rtk_intr_linkupStatus_get
 * Description:
 *      Get interrupt status of linkup.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
rtk_intr_linkupStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      rtk_intr_linkupStatus_clear
 * Description:
 *      Clear interrupt status of linkup.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_linkupStatus_clear(void);


/* Function Name:
 *      rtk_intr_linkupPortStatus_clear
 * Description:
 *      Clear interrupt status of linkup.
 * Input:
 *      portMask - port mask to be cleared
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_intr_linkupPortStatus_clear(rtk_portmask_t portMask);


/* Function Name:
 *      rtk_intr_sts_linkdown_get
 * Description:
 *      Get interrupt status of linkdown.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
rtk_intr_linkdownStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      rtk_intr_linkdownStatus_clear
 * Description:
 *      Clear interrupt status of linkdown.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_linkdownStatus_clear(void);


/* Function Name:
 *      rtk_intr_linkdownPortStatus_clear
 * Description:
 *      Clear interrupt status of linkdown.
 * Input:
 *      portMask - port mask to be cleared
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_intr_linkdownPortStatus_clear(rtk_portmask_t portMask);


/* Function Name:
 *      rtk_intr_sts_gphy_get
 * Description:
 *      Get interrupt status of GPHY.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
rtk_intr_gphyStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      rtk_intr_gphyStatus_clear
 * Description:
 *      Clear interrupt status of GPHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_gphyStatus_clear(void);


/* Function Name:
 *      rtk_intr_imr_restore
 * Description:
 *     set imr mask from input value
 * Input:
 *      imrValue: imr value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_imr_restore(uint32 imrValue);

/* Function Name:
 *      rtk_intr_isr_set
 * Description:
 *     set isr to register or deregister
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_isr_set(rtk_intr_type_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_intr_isr_get
 * Description:
 *     get isr register status
 * Input:
 *      intr          - interrupt type
 * Output:
 *      enable      - interrupt status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_isr_get(rtk_intr_type_t type, rtk_enable_t *enable);

/* Function Name:
 *      rtk_intr_imr_restore
 * Description:
 *     set imr mask from input value
 * Input:
 *      intr          - interrupt type
 * Output:
 *      counter      - counter of interrupt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_intr_isr_counter_dump(rtk_intr_type_t type, uint32 *counter);


#endif /* __RTK_INTR_H__ */
