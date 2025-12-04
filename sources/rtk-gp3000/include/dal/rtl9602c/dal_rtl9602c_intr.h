/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
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
 * $Date: 2013-05-03 17:35:27 +0800 $
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


#ifndef __DAL_RTL9602C_INTR_H__
#define __DAL_RTL9602C_INTR_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/intr.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#define INTR_PORT_MASK 0x7f
#define INTR_IMR_MASK  0x7ffff


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9602c_intr_init
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
dal_rtl9602c_intr_init(void);


/* Function Name:
 *      dal_rtl9602c_intr_polarity_set
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
dal_rtl9602c_intr_polarity_set(rtk_intr_polarity_t polar);


/* Function Name:
 *      dal_rtl9602c_intr_polarity_get
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
dal_rtl9602c_intr_polarity_get(rtk_intr_polarity_t *pPolar);


/* Function Name:
 *      dal_rtl9602c_intr_imr_set
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
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *	   None.	
 */
extern int32 
dal_rtl9602c_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl9602c_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return  state 
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
 extern int32 
dal_rtl9602c_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_rtl9602c_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pState        - pointer of return status 
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9602c_intr_ims_clear
 * Description:
 *      Clear interrupt status.
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_intr_ims_clear(rtk_intr_type_t intr);


/* Function Name:
 *      dal_rtl9602c_intr_speedChangeStatus_get
 * Description:
 *      Get interrupt status of speed change.
 * Input:
 *      pPortMask        - pointer of return status
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
dal_rtl9602c_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_rtl9602c_intr_speedChangeStatus_clear
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
dal_rtl9602c_intr_speedChangeStatus_clear(void);


/* Function Name:
 *      dal_rtl9602c_intr_linkupStatus_get
 * Description:
 *      Get interrupt status of linkup.
 * Input:
 *      pPortMask      - pointer of return status
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
dal_rtl9602c_intr_linkupStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_rtl9602c_intr_linkupStatus_clear
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
dal_rtl9602c_intr_linkupStatus_clear(void);


/* Function Name:
 *      dal_rtl9602c_intr_linkupPortStatus_clear
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
dal_rtl9602c_intr_linkupPortStatus_clear(rtk_portmask_t portMask);


/* Function Name:
 *      dal_rtl9602c_intr_linkdownStatus_get
 * Description:
 *      Get interrupt status of linkdown.
 * Input:
 *      pPortMask        - pointer of return status
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
dal_rtl9602c_intr_linkdownStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_rtl9602c_intr_linkdownStatus_clear
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
dal_rtl9602c_intr_linkdownStatus_clear(void);


/* Function Name:
 *      dal_rtl9602c_intr_linkdownPortStatus_clear
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
dal_rtl9602c_intr_linkdownPortStatus_clear(rtk_portmask_t portMask);


/* Function Name:
 *      dal_rtl9602c_intr_gphyStatus_get
 * Description:
 *      Get interrupt status of GPHY.
 * Input:
 *      pPortMask      - pointer of return status
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
dal_rtl9602c_intr_gphyStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_rtl9602c_intr_gphyStatus_clear
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
dal_rtl9602c_intr_gphyStatus_clear(void);


/* Function Name:
 *      dal_rtl9602c_intr_imr_restore
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
dal_rtl9602c_intr_imr_restore(uint32 imrValue);

/* Function Name:
 *      dal_rtl9602c_intr_isr_set
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
dal_rtl9602c_intr_isr_set(rtk_intr_type_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_intr_isr_get
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
dal_rtl9602c_intr_isr_get(rtk_intr_type_t type, rtk_enable_t *enable);

/* Function Name:
 *      dal_rtl9602c_intr_imr_restore
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
dal_rtl9602c_intr_isr_counter_dump(rtk_intr_type_t type, uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_meter_exceed_isr_entry
 * Description:
 *      Meter Exceed isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_meter_exceed_isr_entry(void);

/* Function Name:
 *      dal_rtl9602c_meter_exceed_isr_counter_get
 * Description:
 *      Get Meter Exceed isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_meter_exceed_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_learn_over_isr_entry
 * Description:
 *      Learn Over isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_learn_over_isr_entry(void);

/* Function Name:
 *      dal_rtl9602c_learn_over_isr_counter_get
 * Description:
 *      Get Learn Over isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_learn_over_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_speed_change_isr_entry
 * Description:
 *      Speed Change isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_speed_change_isr_entry(void);

/* Function Name:
 *      dal_rtl9602c_speed_change_isr_counter_get
 * Description:
 *      Get Speed Change isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_speed_change_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_special_congest_isr_entry
 * Description:
 *      Specail Congest isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_special_congest_isr_entry(void);


/* Function Name:
 *      dal_rtl9602c_special_congest_isr_counter_get
 * Description:
 *      Get Special Congest isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_special_congest_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_gphy_isr_entry
 * Description:
 *      Thermal isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_gphy_Isr_entry ( void );
/* Function Name:
 *      dal_rtl9602c_gphy_isr_counter_get
 * Description:
 *      Get Gphy isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_gphy_isr_counter_get ( uint32* counter );

/* Function Name:
 *      dal_rtl9602c_thermal_isr_entry
 * Description:
 *      Thermal isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_thermal_isr_entry(void);


/* Function Name:
 *      dal_rtl9602c_thermal_isr_counter_get
 * Description:
 *      Get Thermal isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_thermal_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_debug_isr_entry
 * Description:
 *      Debug isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_debug_isr_entry(void);


/* Function Name:
 *      dal_rtl9602c_debug_isr_counter_get
 * Description:
 *      Get Debug isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_debug_isr_counter_get(uint32 *counter);

/* Function Name:
 *      dal_rtl9602c_crash_isr_entry
 * Description:
 *      Crash isr entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_rtl9602c_crash_isr_entry(void);


/* Function Name:
 *      dal_rtl9602c_crash_isr_counter_get
 * Description:
 *      Get Crash isr counter
 * Input:
 *      None
 * Output:
 *      counter - isr counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rtl9602c_crash_isr_counter_get(uint32 *counter);


#endif /* __DAL_RTL9602C_INTR_H__ */

