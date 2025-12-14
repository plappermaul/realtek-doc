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
 * $Date: 2017-03-01 16:03:53 +0800 (Wed, 01 Mar 2017) $
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


#ifndef __RT_INTR_H__
#define __RT_INTR_H__


/*
 * Include Files
 */

/*
 * Symbol Definition
 */

typedef enum rt_intr_type_e
{
    RT_INTR_TYPE_LINK_CHANGE = 0,
    RT_INTR_TYPE_METER_EXCEED,
    RT_INTR_TYPE_LEARN_OVER,
    RT_INTR_TYPE_SPEED_CHANGE,
    RT_INTR_TYPE_SPECIAL_CONGEST,
    RT_INTR_TYPE_LOOP_DETECTION,
    RT_INTR_TYPE_CABLE_DIAG_FIN,
    RT_INTR_TYPE_ACL_ACTION,
    RT_INTR_TYPE_GPHY,
    RT_INTR_TYPE_SERDES,
    RT_INTR_TYPE_GPON,
    RT_INTR_TYPE_EPON,
    RT_INTR_TYPE_PTP,
    RT_INTR_TYPE_DYING_GASP,
    RT_INTR_TYPE_THERMAL,
    RT_INTR_TYPE_ADC,
    RT_INTR_TYPE_EEPROM_UPDATE_110OR118,
    RT_INTR_TYPE_EEPROM_UPDATE_128TO247,
    RT_INTR_TYPE_PKTBUFFER_ERROR,
    RT_INTR_TYPE_DEBUG,
    RT_INTR_TYPE_SMARTCARD,
    RT_INTR_TYPE_SWITCH_ERROR,
    RT_INTR_TYPE_SFP,
    RT_INTR_TYPE_FB_EVENT,
    RT_INTR_TYPE_TOD,
    RT_INTR_TYPE_TOD_UPDATE,
    RT_INTR_TYPE_LOS,
    RT_INTR_TYPE_LOF,
    RT_INTR_TYPE_ALL,
    /*end of rt_intr_type*/
    RT_INTR_TYPE_END
} rt_intr_type_t;

typedef enum rt_intr_link_change_status_e
{
    RT_INTR_LINKCHG_LINKDOWN,
    RT_INTR_LINKCHG_LINKUP,
    RT_INTR_LINKCHG_END
} rt_intr_link_change_status_t;

typedef struct rt_intr_link_change_s
{
    rt_port_t port;
    rt_intr_link_change_status_t status;
}rt_intr_link_change_t;

typedef void (*rt_intr_isr_rx_callback)(void *cookie);

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rt_intr_init
 * Description:
 *      Initialize interrupt module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED  - Failed
 * Note:
 *      Must initialize interrupt module before calling any interrupt APIs.
 */
extern int32
rt_intr_init(void);

/* Function Name:
 *      rt_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_OUT_OF_RANGE  - Interrupt type out of range
 * Note:
 *     None.
 */
extern int32
rt_intr_imr_set(rt_intr_type_t intr, rt_enable_t enable);


/* Function Name:
 *      rt_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return  status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_NULL_POINTER  - pEnable is NULL
 *      RT_ERR_OUT_OF_RANGE  - Interrupt type out of range
 * Note:
 *      None.
 */
extern int32
rt_intr_imr_get(rt_intr_type_t intr, rt_enable_t *pEnable);

/* Function Name:
 *      rt_intr_isr_callback_register
 * Description:
 *      
 * Input:
 *      isrRx - call back function of interrupt
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 * Note:
 *      None
 */
extern int32 
rt_intr_isr_callback_register(rt_intr_type_t intr, rt_intr_isr_rx_callback isrRx);

#endif /* __RT_INTR_H__ */
