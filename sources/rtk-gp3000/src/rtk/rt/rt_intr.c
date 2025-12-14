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




/*
 * Include Files
 */
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_intr.h>
#include <rtk/rt/rt_gpon.h>

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
 *      rt_intr_init
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
int32
rt_intr_init(void)
{
    int32  ret = RT_ERR_OK;

    if (NULL == RT_MAPPER->intr_init)
       return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_init();
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_intr_init */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *     None.
 */
int32
rt_intr_imr_set(rt_intr_type_t intr, rt_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->intr_imr_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_imr_set(intr, enable);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_intr_imr_set */


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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
int32
rt_intr_imr_get(rt_intr_type_t intr, rt_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->intr_imr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_imr_get(intr, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_intr_imr_get */

/* Function Name:
 *      rt_intr_isr_callback_register
 * Description:
 *      
 * Input:
 *      isrRx - call back function of interrupt
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
rt_intr_isr_callback_register(rt_intr_type_t intr, rt_intr_isr_rx_callback isrRx)
{
    int32   ret = RT_ERR_OK;

    if(intr >= RT_INTR_TYPE_END)
        return RT_ERR_FAILED;

    if(isrRx == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_intr_isr_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_intr_isr_callback_register(intr, isrRx);
    RTK_API_UNLOCK();
#else
    /*TBD*/
#endif
    return ret;
} /* end of rt_intr_isr_callback_register */

