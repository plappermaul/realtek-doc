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
uint32 (*validIntr)(uint32) = NULL;


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
int32
rtk_intr_init(void)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    SETSOCKOPT(RTDRV_INTR_INIT, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_init */


/* Function Name:
 *      rtk_intr_polarity_set
 * Description:
 *      Set interrupt polarity mode
 * Input:
 *      mode - Interrupt polarity mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - Invalid input parameters.
 * Note:
 *      The API can set Interrupt polarity mode.
 *      The modes that can be set as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
int32 
rtk_intr_polarity_set(rtk_intr_polarity_t polar)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    osal_memcpy(&intr_cfg.polar, &polar, sizeof(rtk_intr_polarity_t));
    SETSOCKOPT(RTDRV_INTR_POLARITY_SET, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_polarity_set */


/* Function Name:
 *      rtk_intr_polarity_mode_get
 * Description:
 *      Get Interrupt polarity mode
 * Input:
 *      None
 * Output:
 *      pMode - Interrupt polarity mode
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - Invalid input parameters.
 * Note:
 *      The API can get Interrupt polarity mode.
 *    The modes that can be got as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
int32 
rtk_intr_polarity_get(rtk_intr_polarity_t *pPolar)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolar), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_INTR_POLARITY_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pPolar, &intr_cfg.polar, sizeof(rtk_intr_polarity_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_polarity_get */


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
 *     None.    
 */
int32 
rtk_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    osal_memcpy(&intr_cfg.intr, &intr, sizeof(rtk_intr_type_t));
    osal_memcpy(&intr_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_INTR_IMR_SET, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_imr_set */


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
 rtk_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_INPUT);

    intr_cfg.intr = intr;
    /* function body */
    GETSOCKOPT(RTDRV_INTR_IMR_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pEnable, &intr_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_imr_get *



/* Function Name:
 *      rtk_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pState        - pointer of return status of mask (for SW_INTR_TYPE_ALL) or 
 *                enable (for specific inter type value)
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
rtk_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pStatus)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&intr_cfg.intr, &intr, sizeof(rtk_intr_type_t));
    GETSOCKOPT(RTDRV_INTR_IMS_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pStatus, &intr_cfg.status, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_ims_get */


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
int32 
rtk_intr_ims_clear(rtk_intr_type_t intr)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    osal_memcpy(&intr_cfg.intr, &intr, sizeof(rtk_intr_type_t));
    SETSOCKOPT(RTDRV_INTR_IMS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_ims_clear */


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
int32 
rtk_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_INTR_SPEEDCHANGESTATUS_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pPortMask, &intr_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_speedChangeStatus_get */


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
int32 
rtk_intr_speedChangeStatus_clear(void)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    SETSOCKOPT(RTDRV_INTR_SPEEDCHANGESTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_speedChangeStatus_clear */


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
int32 
rtk_intr_linkupStatus_get(rtk_portmask_t *pPortMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_INTR_LINKUPSTATUS_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pPortMask, &intr_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_linkupStatus_get */


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
int32 
rtk_intr_linkupStatus_clear(void)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    SETSOCKOPT(RTDRV_INTR_LINKUPSTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_linkupStatus_clear */


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
int32
rtk_intr_linkupPortStatus_clear(rtk_portmask_t portMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    intr_cfg.portMask = portMask;
    SETSOCKOPT(RTDRV_INTR_LINKUPPORTSTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_linkupPortStatus_clear */


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
int32 
rtk_intr_linkdownStatus_get(rtk_portmask_t *pPortMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_INTR_LINKDOWNSTATUS_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pPortMask, &intr_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_linkdownStatus_get */


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
int32 
rtk_intr_linkdownStatus_clear(void)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    SETSOCKOPT(RTDRV_INTR_LINKDOWNSTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_linkdownStatus_clear */


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
int32
rtk_intr_linkdownPortStatus_clear(rtk_portmask_t portMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    intr_cfg.portMask = portMask;
    SETSOCKOPT(RTDRV_INTR_LINKDOWNPORTSTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_linkdownPortStatus_clear */


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
int32 
rtk_intr_gphyStatus_get(rtk_portmask_t *pPortMask)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_INTR_GPHYSTATUS_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(pPortMask, &intr_cfg.portMask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_gphyStatus_get */


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
int32 
rtk_intr_gphyStatus_clear(void)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    SETSOCKOPT(RTDRV_INTR_GPHYSTATUS_CLEAR, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_gphyStatus_clear */


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
int32 
rtk_intr_imr_restore(uint32 imrValue)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    osal_memcpy(&intr_cfg.imrValue, &imrValue, sizeof(uint32));
    SETSOCKOPT(RTDRV_INTR_IMR_RESTORE, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_imr_restore */

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
int32 
rtk_intr_isr_set(rtk_intr_type_t type, rtk_enable_t enable)
{
    rtdrv_intrCfg_t intr_cfg;

    /* function body */
    osal_memset(&intr_cfg, 0x0, sizeof(rtdrv_intrCfg_t));
    osal_memcpy(&intr_cfg.type, &type, sizeof(rtk_intr_type_t));
    osal_memcpy(&intr_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_INTR_ISR_SET, &intr_cfg, rtdrv_intrCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_intr_isr_set */

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
int32 
rtk_intr_isr_get(rtk_intr_type_t type, rtk_enable_t *enable)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == enable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&intr_cfg.type, &type, sizeof(rtk_intr_type_t));
    GETSOCKOPT(RTDRV_INTR_ISR_GET, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(enable, &intr_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_intr_isr_get */

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
int32 
rtk_intr_isr_counter_dump(rtk_intr_type_t type, uint32 *counter)
{
    rtdrv_intrCfg_t intr_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == counter), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&intr_cfg.type, &type, sizeof(rtk_intr_type_t));
    GETSOCKOPT(RTDRV_INTR_ISR_COUNTER_DUMP, &intr_cfg, rtdrv_intrCfg_t, 1);
    osal_memcpy(counter, &intr_cfg.counter, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_intr_isr_counter_dump */


