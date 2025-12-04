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
 * Purpose : Declaration of PBO API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) PBO initialization
 *
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
 *      Initialize PBO module.
 * Input:
 *      initParam - PBO initialization parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize PBO module before calling any PBO APIs.
 */
int32
rtk_pbo_init(rtk_pbo_initParam_t initParam)
{
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.initParam = initParam;
    SETSOCKOPT(RTDRV_PBO_INIT, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_pbo_init */

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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    memcpy(&pboCfg.swPboInitParam, &initParam, sizeof(rtk_swPbo_initParam_t));
    SETSOCKOPT(RTDRV_SWPBO_INIT, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    pboCfg.enable = enable;
    SETSOCKOPT(RTDRV_SWPBO_PORTSTATE_SET, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    GETSOCKOPT(RTDRV_SWPBO_PORTSTATE_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pEnable = pboCfg.enable;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    pboCfg.enable = enable;
    SETSOCKOPT(RTDRV_SWPBO_PORTAUTOCONF_SET, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    GETSOCKOPT(RTDRV_SWPBO_PORTAUTOCONF_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pEnable = pboCfg.enable;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    pboCfg.threshold = threshold;
    SETSOCKOPT(RTDRV_SWPBO_PORTTHRESHOLD_SET, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    GETSOCKOPT(RTDRV_SWPBO_PORTTHRESHOLD_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pThreshold = pboCfg.threshold;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    pboCfg.qid = queue;
    pboCfg.threshold = threshold;
    SETSOCKOPT(RTDRV_SWPBO_QUEUETHRESHOLD_SET, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    pboCfg.qid = queue;
    GETSOCKOPT(RTDRV_SWPBO_QUEUETHRESHOLD_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pThreshold = pboCfg.threshold;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCurrPage), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMaxPage), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    GETSOCKOPT(RTDRV_SWPBO_PORTUSEDPAGE_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pCurrPage = pboCfg.currPage;
    *pMaxPage = pboCfg.maxPage;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    SETSOCKOPT(RTDRV_SWPBO_PORTUSEDPAGE_CLEAR, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCurrPage), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMaxPage), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    pboCfg.qid = queue;
    GETSOCKOPT(RTDRV_SWPBO_QUEUEUSEDPAGE_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pCurrPage = pboCfg.currPage;
    *pMaxPage = pboCfg.maxPage;

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    pboCfg.qid = queue;
    SETSOCKOPT(RTDRV_SWPBO_QUEUEUSEDPAGE_CLEAR, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_pboCfg_t pboCfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCurrDscSram), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pCurrDscDram), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMaxDscSram), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMaxDscDram), RT_ERR_NULL_POINTER);

    /* function body */
    pboCfg.port = port;
    GETSOCKOPT(RTDRV_SWPBO_PORTDSC_GET, &pboCfg, rtdrv_pboCfg_t, 1);
    *pCurrDscSram = pboCfg.currDscSram;
    *pCurrDscDram = pboCfg.currDscDram;
    *pMaxDscSram = pboCfg.maxDscSram;
    *pMaxDscDram = pboCfg.maxDscDram;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_swPbo_portDscCount_clear
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
int32 rtk_swPbo_portDscCount_clear(
    rtk_port_t port)
{
    rtdrv_pboCfg_t pboCfg;
    
    /* function body */
    osal_memset(&pboCfg, 0x0, sizeof(rtdrv_pboCfg_t));
    pboCfg.port = port;
    SETSOCKOPT(RTDRV_SWPBO_PORTDSC_CLEAR, &pboCfg, rtdrv_pboCfg_t, 1);

    return RT_ERR_OK;
}

