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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Declaration of PBO API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) PBO initialization
 *
 */


#ifndef __RTK_PBO_H__
#define __RTK_PBO_H__


/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Symbol Definition
 */
typedef enum rtk_pbo_mode_s
{
	PBO_MODE_GPON=0,
    PBO_MODE_EPON,
	PBO_MODE_END
} rtk_pbo_mode_t;

typedef enum rtk_pbo_pageSize_s
{
	PBO_PAGE_SIZE_128  = 128,
    PBO_PAGE_SIZE_256  = 256,
	PBO_PAGE_SIZE_512  = 512,
	PBO_PAGE_SIZE_1024 = 1024,
	PBO_PAGE_SIZE_END
} rtk_pbo_pageSize_t;

typedef enum rtk_pbo_pageCount_s
{
	PBO_PAGE_COUNT_512  = 512,
    PBO_PAGE_COUNT_1024 = 1024,
	PBO_PAGE_COUNT_2048 = 2048,
	PBO_PAGE_COUNT_4096 = 4096,
	PBO_PAGE_COUNT_END
} rtk_pbo_pageCount_t;

typedef struct rtk_pbo_initParm_s
{
	rtk_pbo_mode_t mode;
    rtk_pbo_pageSize_t usPageSize;
    rtk_pbo_pageSize_t dsPageSize;
} rtk_pbo_initParam_t;

typedef struct rtk_swPbo_initParam_s
{
    rtk_pbo_pageSize_t pageSize;
    rtk_pbo_pageCount_t pageCount;
} rtk_swPbo_initParam_t;

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
extern int32
rtk_pbo_init(rtk_pbo_initParam_t initParam);

/******************************************************************************/
/* SW PBO                                                                     */
/******************************************************************************/
/* Function Name:
 *      rtk_swPbo_init
 * Description:
 *      initParam - SWPBO initialization parameters
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32 rtk_swPbo_init(rtk_swPbo_initParam_t initParam);

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
extern int32 rtk_swPbo_portState_set(
    rtk_port_t port,
    rtk_enable_t enable);

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
extern int32 rtk_swPbo_portState_get(
    rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_swPbo_portAutoConfig_set
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
extern int32 rtk_swPbo_portAutoConf_set(
    rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_swPbo_portAutoConfig_get
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
extern int32 rtk_swPbo_portAutoConf_get(
    rtk_port_t port,
    rtk_enable_t *pEnable);

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
extern int32 rtk_swPbo_portThreshold_set(
    rtk_port_t port,
    uint32 threshold);

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
extern int32 rtk_swPbo_portThreshold_get(
    rtk_port_t port,
    uint32 *pThreshold);

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
extern int32 rtk_swPbo_queueThreshold_set(
    rtk_port_t port,
    uint32 queue,
    uint32 threshold);

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
extern int32 rtk_swPbo_queueThreshold_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pThreshold);

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
extern int32 rtk_swPbo_portUsedPageCount_get(
    rtk_port_t port,
    uint32 *pCurrPage,
    uint32 *pMaxPage);

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
extern int32 rtk_swPbo_portUsedPageCount_clear(
    rtk_port_t port);

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
extern int32 rtk_swPbo_queueUsedPageCount_get(
    rtk_port_t port,
    uint32 queue,
    uint32 *pCurrPage,
    uint32 *pMaxPage);

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
extern int32 rtk_swPbo_queueUsedPageCount_clear(
    rtk_port_t port,
    uint32 queue);

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
extern int32 rtk_swPbo_portDscCount_get(
    rtk_port_t port,
    uint32 *pCurrDscSram,
    uint32 *pCurrDscDram,
    uint32 *pMaxDscSram,
    uint32 *pMaxDscDram);

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
extern int32 rtk_swPbo_portDscCount_clear(
    rtk_port_t port);

#endif /* __RTK_PBO_H__ */

