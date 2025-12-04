/*
 * Copyright (C) 2016 Realtek Semiconductor Corp.
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
 * Purpose : Declaration of PBO and PBO API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) PBO initialization
 *
 */

/*
 * Include Files
 */
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/rt_type.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : PBO */
/******************************************************************************/
/* PON PBO                                                                    */
/******************************************************************************/
/* Function Name:
 *      rtk_pbo_init
 * Description:
 *      Initialize pbo module.
 * Input:
 *      initParam - PBO initialization parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize pbo module before calling any pbo APIs.
 */
int32
rtk_pbo_init(rtk_pbo_initParam_t initParam)
{
    int32   ret;

    if (NULL == RT_MAPPER->pbo_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->pbo_init(initParam);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_pbo_init */

/******************************************************************************/
/* SW PBO                                                                     */
/******************************************************************************/
/* Function Name:
 *      rtk_swPbo_init
 * Description:
 *      Initialize switch PBO
 * Input:
 *      initParam - SWPBO initialization parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtk_swPbo_init(rtk_swPbo_initParam_t initParam)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_init(initParam);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portState_set
 * Description:
 *      Set switch PBO UTP port state to enable or disable
 * Input:
 *      port                - port number to perform
 *      enable              - enable / disable port switch PBO
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtk_swPbo_portState_set(
    rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portState_set(port, enable);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portState_get
 * Description:
 *      Get switch PBO UTP port state to enable or disable
 * Input:
 *      port                - port number to perform
 * Output:
 *      pEnable             - enable / disable port switch PBO
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 rtk_swPbo_portState_get(
    rtk_port_t port,
    rtk_enable_t *pEnable)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portState_get(port, pEnable);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portAutoConf_set
 * Description:
 *      Set switch PBO UTP port auto switching in ISR to enable or disable
 * Input:
 *      port                - port number to perform
 *      enable              - enable / disable port switch PBO
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 rtk_swPbo_portAutoConf_set(
    rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portAutoConf_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portAutoConf_set(port, enable);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portAutoConf_get
 * Description:
 *      Get switch PBO UTP port auto switching in ISR to enable or disable
 * Input:
 *      port                - port number to perform
 * Output:
 *      pEnable             - enable / disable port switch PBO
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 rtk_swPbo_portAutoConf_get(
    rtk_port_t port,
    rtk_enable_t *pEnable)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portAutoConf_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portAutoConf_get(port, pEnable);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portThreshold_set
 * Description:
 *      Set switch PBO UTP port page use threshold
 * Input:
 *      port                - port number to perform
 *      threshold           - page use threshold of port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 rtk_swPbo_portThreshold_set(
    rtk_port_t port,
    uint32 threshold)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portThreshold_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portThreshold_set(port, threshold);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portThreshold_get
 * Description:
 *      Get switch PBO UTP port page use threshold
 * Input:
 *      port                - port number to perform
 * Output:
 *      pThreshold          - page use threshold of port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtk_swPbo_portThreshold_get(
    rtk_port_t port,
    uint32 *pThreshold)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portThreshold_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portThreshold_get(port, pThreshold);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_queueThreshold_set
 * Description:
 *      Set switch PBO UTP queue page use threshold
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 *      threshold           - page use threshold of queue
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 * Note:
 *      None
 */
int32 rtk_swPbo_queueThreshold_set(
    rtk_port_t port,
    uint32 queue,
    uint32 threshold)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_queueThreshold_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_queueThreshold_set(port, queue, threshold);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_queueThreshold_get
 * Description:
 *      Get switch PBO UTP queue page use threshold
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      pThreshold          - page use threshold of queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtk_swPbo_queueThreshold_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pThreshold)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_queueThreshold_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_queueThreshold_get(port, queue, pThreshold);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portUsedPageCount_get
 * Description:
 *      Get switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      pCurrPage           - current used page
 *      pMaxPage            - maximum used page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtk_swPbo_portUsedPageCount_get(
    rtk_port_t port,
    uint32 *pCurrPage,
    uint32 *pMaxPage)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portUsedPageCount_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portUsedPageCount_get(port, pCurrPage, pMaxPage);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portUsedPageCount_clear
 * Description:
 *      Clear switch PBO UTP port used page maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 rtk_swPbo_portUsedPageCount_clear(
    rtk_port_t port)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portUsedPageCount_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portUsedPageCount_clear(port);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_queueUsedPageCount_get
 * Description:
 *      Get switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      pCurrPage           - current used page
 *      pMaxPage            - maximum used page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtk_swPbo_queueUsedPageCount_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pCurrPage,
    uint32 *pMaxPage)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_queueUsedPageCount_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_queueUsedPageCount_get(port, queue, pCurrPage, pMaxPage);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_queueUsedPageCount_clear
 * Description:
 *      Clear switch PBO UTP port used page current and maximum
 * Input:
 *      port                - port number to perform
 *      queue               - queue number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_NUM
 * Note:
 *      None
 */
int32 rtk_swPbo_queueUsedPageCount_clear(
    rtk_port_t port,
    uint32 queue)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_queueUsedPageCount_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_queueUsedPageCount_clear(port, queue);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portDscCount_get
 * Description:
 *      Get switch PBO UTP port descriptor current and maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      pCurrDscSram        - current dscriptor SRAM
 *      pCurrDscDram        - current dscriptor DRAM
 *      pMaxDscSram         - maximum dscriptor SRAM
 *      pMaxDscDram         - maximum dscriptor DRAM
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 rtk_swPbo_portDscCount_get(
    rtk_port_t port,
    uint32 *pCurrDscSram,
    uint32 *pCurrDscDram,
    uint32 *pMaxDscSram,
    uint32 *pMaxDscDram)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portDscCount_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portDscCount_get(port, pCurrDscSram, pCurrDscDram, pMaxDscSram, pMaxDscDram);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_swPbo_portDscCount_clear
 * Description:
 *      Clear switch PBO UTP port descriptor maximum
 * Input:
 *      port                - port number to perform
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 rtk_swPbo_portDscCount_clear(
    rtk_port_t port)
{
    int32   ret;

    if (NULL == RT_MAPPER->swPbo_portDscCount_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->swPbo_portDscCount_clear(port);
    RTK_API_UNLOCK();
    return ret;
}

