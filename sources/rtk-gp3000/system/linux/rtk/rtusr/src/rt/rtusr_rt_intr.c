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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>

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
    rtdrv_rt_intrCfg_t rt_intr_cfg;

    /* function body */
    osal_memset(&rt_intr_cfg, 0x0, sizeof(rtdrv_rt_intrCfg_t));
    SETSOCKOPT(RTDRV_RT_INTR_INIT, &rt_intr_cfg, rtdrv_rt_intrCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_rt_intrCfg_t rt_intr_cfg;

    /* function body */
    osal_memcpy(&rt_intr_cfg.intr, &intr, sizeof(rt_intr_type_t));
    osal_memcpy(&rt_intr_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_INTR_IMR_SET, &rt_intr_cfg, rtdrv_rt_intrCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_rt_intrCfg_t rt_intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_intr_cfg.intr, &intr, sizeof(rt_intr_type_t));
    GETSOCKOPT(RTDRV_RT_INTR_IMR_GET, &rt_intr_cfg, rtdrv_rt_intrCfg_t, 1);
    osal_memcpy(pEnable, &rt_intr_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_intr_imr_get */

