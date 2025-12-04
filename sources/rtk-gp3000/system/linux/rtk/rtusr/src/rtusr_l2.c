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
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
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
#include <rtk/port.h>

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

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      rtk_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
rtk_l2_init(void)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_L2_INIT, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_init */

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_FLUSHLINKDOWNPORTADDRENABLE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pEnable, &l2_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L2_FLUSHLINKDOWNPORTADDRENABLE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_flushLinkDownPortAddrEnable_set */



/* Function Name:
 *      rtk_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_ucastAddr_flush(rtk_l2_flushCfg_t *pConfig)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.config, pConfig, sizeof(rtk_l2_flushCfg_t));
    GETSOCKOPT(RTDRV_L2_UCASTADDR_FLUSH, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pConfig, &l2_cfg.config, sizeof(rtk_l2_flushCfg_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ucastAddr_flush */

/* Function Name:
 *      rtk_l2_table_clear
 * Description:
 *      Clear entire L2 table.
 *      All the entries (static and dynamic) (L2 and L3) will be deleted.
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
rtk_l2_table_clear(void)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_L2_TABLE_CLEAR, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_table_clear */


/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_get
 * Description:
 *      Get the system learning over status
 * Input:
 *      None.
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtk_l2_limitLearningOverStatus_get(uint32 *pStatus)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LIMITLEARNINGOVERSTATUS_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pStatus, &l2_cfg.status, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningOverStatus_get */

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_clear
 * Description:
 *      Clear the system learning over status
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
rtk_l2_limitLearningOverStatus_clear(void)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_L2_LIMITLEARNINGOVERSTATUS_CLEAR, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningOverStatus_clear */

/* Function Name:
 *      rtk_l2_learningCnt_get
 * Description:
 *      Get the total mac learning counts of the whole specified device.
 * Input:
 *      None.
 * Output:
 *      pMacCnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
rtk_l2_learningCnt_get(uint32 *pMacCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LEARNINGCNT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMacCnt, &l2_cfg.macCnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_learningCnt_get */

/* Function Name:
 *      rtk_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rtk_l2_limitLearningCnt_get(uint32 *pMacCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LIMITLEARNINGCNT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMacCnt, &l2_cfg.macCnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCnt_get */

/* Function Name:
 *      rtk_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      macCnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the system.
 */
int32
rtk_l2_limitLearningCnt_set(uint32 macCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.macCnt, &macCnt, sizeof(uint32));
    SETSOCKOPT(RTDRV_L2_LIMITLEARNINGCNT_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCnt_set */

/* Function Name:
 *      rtk_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
rtk_l2_limitLearningCntAction_get(rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LIMITLEARNINGCNTACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLearningAction, &l2_cfg.learningAction, sizeof(rtk_l2_limitLearnCntAction_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCntAction_get */

/* Function Name:
 *      rtk_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the specified device.
 * Input:
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
rtk_l2_limitLearningCntAction_set(rtk_l2_limitLearnCntAction_t learningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.learningAction, &learningAction, sizeof(rtk_l2_limitLearnCntAction_t));
    SETSOCKOPT(RTDRV_L2_LIMITLEARNINGCNTACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCntAction_set */

/* Function Name:
 *      rtk_l2_limitLearningEntryAction_get
 * Description:
 *      Get the action when over learning the same hash result entry.
 * Input:
 *      None
 * Output:
 *      pLearningAction - pointer buffer of action when over learning the same hash result entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_ENTRY_ACTION_FORWARD
 *          - LIMIT_LEARN_ENTRY_ACTION_TO_CPU
 */
int32
rtk_l2_limitLearningEntryAction_get(rtk_l2_limitLearnEntryAction_t *pLearningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LIMITLEARNINGENTRYACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLearningAction, &l2_cfg.learningEntryAction, sizeof(rtk_l2_limitLearnEntryAction_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCntAction_get */

/* Function Name:
 *      rtk_l2_limitLearningEntryAction_set
 * Description:
 *      Set the action when over learning the same hash result entry.
 * Input:
 *      learningAction - action when over learning the same hash result entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_ENTRY_ACTION_FORWARD
 *          - LIMIT_LEARN_ENTRY_ACTION_TO_CPU
 */
int32
rtk_l2_limitLearningEntryAction_set(rtk_l2_limitLearnEntryAction_t learningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.learningEntryAction, &learningAction, sizeof(rtk_l2_limitLearnEntryAction_t));
    SETSOCKOPT(RTDRV_L2_LIMITLEARNINGENTRYACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_limitLearningCntAction_set */

/* Function Name:
 *      rtk_l2_limitLearningPortMask_get
 * Description:
 *      Get the port mask that indicate which port(s) are counted into
 *      system learning count
 * Input:
 *      None
 * Output:
 *      pPortmask - configure the port mask which counted into system counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rtk_l2_limitLearningPortMask_get(rtk_portmask_t *pPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_LIMITLEARNINGPORTMASK_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pPortmask, &l2_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
} /* end of rtk_l2_limitLearningPortMask_get */

/* Function Name:
 *      rtk_l2_limitLearningPortMask_set
 * Description:
 *      Set the port mask that indicate which port(s) are counted into
 *      system learning count
 * Input:
 *      portmask - configure the port mask which counted into system counter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_MASK
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rtk_l2_limitLearningPortMask_set(rtk_portmask_t portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    //RT_PARAM_CHK((!HAL_IS_PORTMASK_VALID(portmask)), RT_ERR_PORT_MASK);

    /* function body */
    osal_memcpy(&l2_cfg.portmask, &portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_LIMITLEARNINGPORTMASK_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_limitLearningPortMask_set */

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_get
 * Description:
 *      Get the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtk_l2_portLimitLearningOverStatus_get(rtk_port_t port, uint32 *pStatus)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGOVERSTATUS_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pStatus, &l2_cfg.status, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningOverStatus_get */

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_clear
 * Description:
 *      Clear the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
rtk_l2_portLimitLearningOverStatus_clear(rtk_port_t port)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGOVERSTATUS_CLEAR, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningOverStatus_clear */

/* Function Name:
 *      rtk_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt  - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
rtk_l2_portLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTLEARNINGCNT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMacCnt, &l2_cfg.macCnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLearningCnt_get */

/* Function Name:
 *      rtk_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rtk_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGCNT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMacCnt, &l2_cfg.macCnt, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningCnt_get */


/* Function Name:
 *      rtk_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
rtk_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.macCnt, &macCnt, sizeof(uint32));
    SETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGCNT_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningCnt_set */


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
rtk_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGCNTACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLearningAction, &l2_cfg.learningAction, sizeof(rtk_l2_limitLearnCntAction_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
rtk_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.learningAction, &learningAction, sizeof(rtk_l2_limitLearnCntAction_t));
    SETSOCKOPT(RTDRV_L2_PORTLIMITLEARNINGCNTACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningCntAction_set */



/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */

/* Function Name:
 *      rtk_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
int32
rtk_l2_aging_get(uint32 *pAgingTime)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_AGING_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAgingTime, &l2_cfg.agingTime, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_l2_aging_get */


/* Function Name:
 *      rtk_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
int32
rtk_l2_aging_set(uint32 agingTime)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.agingTime, &agingTime, sizeof(uint32));
    SETSOCKOPT(RTDRV_L2_AGING_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_aging_set */


/* Function Name:
 *      rtk_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTAGINGENABLE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pEnable, &l2_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_portAgingEnable_get */

/* Function Name:
 *      rtk_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L2_PORTAGINGENABLE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      rtk_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
rtk_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    GETSOCKOPT(RTDRV_L2_LOOKUPMISSACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAction, &l2_cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissAction_get */

/* Function Name:
 *      rtk_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU (only for DLF_TYPE_UCAST)
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
rtk_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    osal_memcpy(&l2_cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_LOOKUPMISSACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissAction_set */

/* Function Name:
 *      rtk_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    GETSOCKOPT(RTDRV_L2_PORTLOOKUPMISSACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAction, &l2_cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLookupMissAction_get */

/* Function Name:
 *      rtk_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    osal_memcpy(&l2_cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_PORTLOOKUPMISSACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portLookupMissAction_set */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type   - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_get(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    GETSOCKOPT(RTDRV_L2_LOOKUPMISSFLOODPORTMASK_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFlood_portmask, &l2_cfg.flood_portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_set(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    osal_memcpy(&l2_cfg.flood_portmask, pFlood_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_LOOKUPMISSFLOODPORTMASK_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_add(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    osal_memcpy(&l2_cfg.flood_port, &flood_port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_L2_LOOKUPMISSFLOODPORTMASK_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissFloodPortMask_add */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration..
 */
int32
rtk_l2_lookupMissFloodPortMask_del(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.type, &type, sizeof(rtk_l2_lookupMissType_t));
    osal_memcpy(&l2_cfg.flood_port, &flood_port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_L2_LOOKUPMISSFLOODPORTMASK_DEL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_lookupMissFloodPortMask_del */

/* Module Name    : L2      */
/* Sub-module Name: Unicast */
/* Function Name:
 *      rtk_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
rtk_l2_newMacOp_get(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pLrnMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_NEWMACOP_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pLrnMode, &l2_cfg.lrnMode, sizeof(rtk_l2_newMacLrnMode_t));
    osal_memcpy(pFwdAction, &l2_cfg.fwdAction, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_newMacOp_get */

/* Function Name:
 *      rtk_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
rtk_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.lrnMode, &lrnMode, sizeof(rtk_l2_newMacLrnMode_t));
    osal_memcpy(&l2_cfg.fwdAction, &fwdAction, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_NEWMACOP_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_newMacOp_set */


/* Module Name    : L2              */
/* Sub-module Name: Get next entry */

/* Function Name:
 *      rtk_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx       - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2UcastData   - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);

    /* function body */
	osal_memcpy(&l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    osal_memcpy(&l2_cfg.l2UcastData, pL2UcastData, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXTVALIDADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2UcastData, &l2_cfg.l2UcastData, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_nextValidAddr_get */


/* Function Name:
 *      rtk_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2UcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    osal_memcpy(&l2_cfg.l2UcastData, pL2UcastData, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXTVALIDADDRONPORT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2UcastData, &l2_cfg.l2UcastData, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_nextValidAddrOnPort_get */


/* Function Name:
 *      rtk_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2McastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          reply next valid L2 multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidMcastAddr_get(
    int32               *pScanIdx,
    rtk_l2_mcastAddr_t  *pL2McastData)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2McastData), RT_ERR_NULL_POINTER);

    /* function body */
	osal_memcpy(&l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    osal_memcpy(&l2_cfg.l2McastData, pL2McastData, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXTVALIDMCASTADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2McastData, &l2_cfg.l2McastData, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_nextValidMcastAddr_get */

/* Function Name:
 *      rtk_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pIpMcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          reply next valid L2 ip multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidIpMcastAddr_get(
    int32                   *pScanIdx,
    rtk_l2_ipMcastAddr_t    *pIpMcastData)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpMcastData), RT_ERR_NULL_POINTER);

    /* function body */
	osal_memcpy(&l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    osal_memcpy(&l2_cfg.ipMcastData, pIpMcastData, sizeof(rtk_l2_ipMcastAddr_t));
    GETSOCKOPT(RTDRV_L2_NEXTVALIDIPMCASTADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pIpMcastData, &l2_cfg.ipMcastData, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_nextValidIpMcastAddr_get */



/* Function Name:
 *      rtk_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
int32
rtk_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Entry), RT_ERR_NULL_POINTER);

    /* function body */
	osal_memcpy(&l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    osal_memcpy(&l2_cfg.l2Entry, pL2Entry, sizeof(rtk_l2_addr_table_t));
    GETSOCKOPT(RTDRV_L2_NEXTVALIDENTRY_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2Entry, &l2_cfg.l2Entry, sizeof(rtk_l2_addr_table_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_nextValidEntry_get */



/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      rtk_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rtk_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.l2Addr, pL2Addr, sizeof(rtk_l2_ucastAddr_t));
    SETSOCKOPT(RTDRV_L2_ADDR_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2Addr, &l2_cfg.l2Addr, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_addr_add */

/* Function Name:
 *      rtk_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rtk_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.l2Addr, pL2Addr, sizeof(rtk_l2_ucastAddr_t));
    SETSOCKOPT(RTDRV_L2_ADDR_DEL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_addr_del */


/* Function Name:
 *      rtk_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rtk_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.l2Addr, pL2Addr, sizeof(rtk_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_L2_ADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pL2Addr, &l2_cfg.l2Addr, sizeof(rtk_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_addr_get */


/* Function Name:
 *      rtk_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l2_addr_delAll(uint32 includeStatic)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.includeStatic, &includeStatic, sizeof(uint32));
    SETSOCKOPT(RTDRV_L2_ADDR_DELALL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_addr_delAll */


/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      rtk_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
int32
rtk_l2_mcastAddr_add(rtk_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mcastAddr, pMcastAddr, sizeof(rtk_l2_mcastAddr_t));
    SETSOCKOPT(RTDRV_L2_MCASTADDR_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcastAddr, &l2_cfg.mcastAddr, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_mcastAddr_add */

/* Function Name:
 *      rtk_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
int32
rtk_l2_mcastAddr_del(rtk_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mcastAddr, pMcastAddr, sizeof(rtk_l2_mcastAddr_t));
    SETSOCKOPT(RTDRV_L2_MCASTADDR_DEL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_mcastAddr_del */

/* Function Name:
 *      rtk_l2_mcastAddr_get
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_mcastAddr_get(rtk_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mcastAddr, pMcastAddr, sizeof(rtk_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_L2_MCASTADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMcastAddr, &l2_cfg.mcastAddr, sizeof(rtk_l2_mcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_mcastAddr_get */

/* Function Name:
 *      rtk_l2_extMemberConfig_get
 * Description:
 *      Get the extension port member config
 * Input:
 *      cfgIndex  - extension port member config index
 * Output:
 *      pPortmask - extension port mask for specific index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
rtk_l2_extMemberConfig_get(uint32 cfgIndex, rtk_portmask_t *pPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.index, &cfgIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_L2_EXTMEMBERCONFIG_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pPortmask, &l2_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_l2_extMemberConfig_set
 * Description:
 *      Set the extension port member config
 * Input:
 *      cfgIndex  - extension port member config index
 *      portmask  - extension port mask for specific index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
rtk_l2_extMemberConfig_set(uint32 cfgIndex, rtk_portmask_t portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.index, &cfgIndex, sizeof(uint32));
    osal_memcpy(&l2_cfg.portmask, &portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_EXTMEMBERCONFIG_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_l2_vidUnmatchAction_get
 * Description:
 *      Get forwarding action when vid learning unmatch happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rtk_l2_vidUnmatchAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_VIDUNMATCHACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFwdAction, &l2_cfg.fwdAction, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_vidUnmatchAction_get */

/* Function Name:
 *      rtk_l2_vidUnmatchAction_set
 * Description:
 *      Set forwarding action when vid learning unmatch happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
rtk_l2_vidUnmatchAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.fwdAction, &fwdAction, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_VIDUNMATCHACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_vidUnmatchAction_set */


/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      rtk_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rtk_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_ILLEGALPORTMOVEACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFwdAction, &l2_cfg.fwdAction, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_illegalPortMoveAction_get */

/* Function Name:
 *      rtk_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rtk_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.fwdAction, &fwdAction, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_ILLEGALPORTMOVEACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_illegalPortMoveAction_set */


/* Module Name    : L2           */
/* Sub-module Name: IP multicast */


/* Function Name:
 *      rtk_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP_AND_VID
 */
int32
rtk_l2_ipmcMode_get(rtk_l2_ipmcMode_t *pMode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mode, pMode, sizeof(rtk_l2_ipmcMode_t));
    GETSOCKOPT(RTDRV_L2_IPMCMODE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMode, &l2_cfg.mode, sizeof(rtk_l2_ipmcMode_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcMode_get */

/* Function Name:
 *      rtk_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP_AND_VID
 */
int32
rtk_l2_ipmcMode_set(rtk_l2_ipmcMode_t mode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.mode, &mode, sizeof(rtk_l2_ipmcMode_t));
    SETSOCKOPT(RTDRV_L2_IPMCMODE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcMode_set */

/* Function Name:
 *      rtk_l2_ipmcVlanMode_get
 * Description:
 *      Get lookup VLAN mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup VLAN mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_VLAN_BY_IVL_SVL
 *      - LOOKUP_VLAN_FORCE_NO_VLAN
 *      - LOOKUP_VLAN_FORCE_VLAN
 */
int32
rtk_l2_ipmcVlanMode_get(rtk_l2_ipmcVlanMode_t *pMode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mode, pMode, sizeof(rtk_l2_ipmcVlanMode_t));
    GETSOCKOPT(RTDRV_L2_IPMCVLANMODE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMode, &l2_cfg.mode, sizeof(rtk_l2_ipmcVlanMode_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcMode_get */

/* Function Name:
 *      rtk_l2_ipmcVlanMode_set
 * Description:
 *      Set lookup VLAN mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup VLAN mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_VLAN_BY_IVL_SVL
 *      - LOOKUP_VLAN_FORCE_NO_VLAN
 *      - LOOKUP_VLAN_FORCE_VLAN
 */
int32
rtk_l2_ipmcVlanMode_set(rtk_l2_ipmcVlanMode_t mode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.mode, &mode, sizeof(rtk_l2_ipmcVlanMode_t));
    SETSOCKOPT(RTDRV_L2_IPMCVLANMODE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcMode_set */

/* Function Name:
 *      rtk_l2_ipv6mcMode_get
 * Description:
 *      Get lookup mode of layer2 ipv6 multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ipv6 multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ipv6 multicast switching is as following
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP
 */
int32
rtk_l2_ipv6mcMode_get(rtk_l2_ipmcMode_t *pMode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.mode, pMode, sizeof(rtk_l2_ipmcMode_t));
    GETSOCKOPT(RTDRV_L2_IPV6MCMODE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pMode, &l2_cfg.mode, sizeof(rtk_l2_ipmcMode_t));

    return RT_ERR_OK;
} /* end of rtk_l2_ipv6mcMode_get */

/* Function Name:
 *      rtk_l2_ipv6mcMode_set
 * Description:
 *      Set lookup mode of layer2 ipv6 multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ipv6 multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ipv6 multicast switching is as following
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DIP
 */
int32
rtk_l2_ipv6mcMode_set(rtk_l2_ipmcMode_t mode)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.mode, &mode, sizeof(rtk_l2_ipmcMode_t));
    SETSOCKOPT(RTDRV_L2_IPV6MCMODE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_ipv6mcMode_set */

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_get
 * Description:
 *      Get Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      None
 * Output:
 *      pIpmcHash - pointer of Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
rtk_l2_ipmcGroupLookupMissHash_get(rtk_l2_ipmcHashOp_t *pIpmcHash)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcHash), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_IPMCGROUPLOOKUPMISSHASH_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pIpmcHash, &l2_cfg.ipmcHash, sizeof(rtk_l2_ipmcHashOp_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcGroupLookupMissHash_get */

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_set
 * Description:
 *      Set Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      ipmcHash - Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
rtk_l2_ipmcGroupLookupMissHash_set(rtk_l2_ipmcHashOp_t ipmcHash)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.ipmcHash, &ipmcHash, sizeof(rtk_l2_ipmcHashOp_t));
    SETSOCKOPT(RTDRV_L2_IPMCGROUPLOOKUPMISSHASH_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcGroupLookupMissHash_set */

/* Function Name:
 *      rtk_l2_ipmcGroup_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip         - Group IP
 *      pPortmask   - Group member port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_add(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    osal_memcpy(&l2_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_IPMCGROUP_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcGroup_add */

/* Function Name:
 *      rtk_l2_ipmcGroupExtPortmask_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip             - Group IP
 *      pExtPortmask    - Group member extension port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroupExtPortmask_add(ipaddr_t gip, rtk_portmask_t *pExtPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    osal_memcpy(&l2_cfg.portmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_IPMCGROUPEXT_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcGroup_add */

/* Function Name:
 *      rtk_l2_ipmcGroupExtMemberIdx_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip             - Group IP
 *      index           - Group member extension port mask index configured by rtk_l2_extMemberConfig_set
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroupExtMemberIdx_add(ipaddr_t gip, uint32 index)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    osal_memcpy(&l2_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_L2_IPMCGROUPEXTIDX_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcGroupExtMemberIdx_add */

/* Function Name:
 *      rtk_l2_ipmcGroup_del
 * Description:
 *      Delete an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_del(ipaddr_t gip)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    SETSOCKOPT(RTDRV_L2_IPMCGROUP_DEL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcGroup_del */

/* Function Name:
 *      rtk_l2_ipmcGroup_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      pPortmask   - Group member port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_get(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    GETSOCKOPT(RTDRV_L2_IPMCGROUP_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pPortmask, &l2_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipmcGroup_get */

/* Function Name:
 *      rtk_l2_ipmcGroupExtPortmask_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip             - Group IP
 * Output:
 *      pExtPortmask    - Group member extension port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroupExtPortmask_get(ipaddr_t gip, rtk_portmask_t *pExtPortmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    GETSOCKOPT(RTDRV_L2_IPMCGROUPEXT_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pExtPortmask, &l2_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcGroupExtPortmask_get */

/* Function Name:
 *      rtk_l2_ipmcGroupExtMemberIdx_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip             - Group IP
 * Output:
 *      pIndex          - Group member extension port mask index configured by rtk_l2_extMemberConfig_set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroupExtMemberIdx_get(ipaddr_t gip, uint32 *pIndex)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ip, &gip, sizeof(ipaddr_t));
    GETSOCKOPT(RTDRV_L2_IPMCGROUPEXTIDX_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pIndex, &l2_cfg.index, sizeof(uint32));

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcGroupExtMemberIdx_get */

/* Function Name:
 *      rtk_l2_ipmcSipFilter_set
 * Description:
 *      Set an entry to IPMC SIP filter Table by index
 * Input:
 *      index       - table index
 *      sip         - SIP to be filtered
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      
 * Note:
 *      None
 */
int32
rtk_l2_ipmcSipFilter_set(uint32 index, ipaddr_t sip)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */

    /* function body */
    l2_cfg.index = index;
    osal_memcpy(&l2_cfg.ip, &sip, sizeof(ipaddr_t));
    SETSOCKOPT(RTDRV_L2_IPMCSIPFILTER_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcSipFilter_set */

/* Function Name:
 *      rtk_l2_ipmcSipFilter_get
 * Description:
 *      Get an entry from IPMC SIP filter Table by index
 * Input:
 *      index       - table index
 * Output:
 *      pSip        - SIP buffer to store the retrieved data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      
 * Note:
 *      None
 */
int32
rtk_l2_ipmcSipFilter_get(uint32 index, ipaddr_t *pSip)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSip), RT_ERR_NULL_POINTER);

    /* function body */
    l2_cfg.index = index;
    GETSOCKOPT(RTDRV_L2_IPMCSIPFILTER_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pSip, &l2_cfg.ip, sizeof(ipaddr_t));

    return RT_ERR_OK;
} /* end of rtk_l2_ipmcSipFilter_get */

/* Function Name:
 *      rtk_l2_portIpmcAction_get
 * Description:
 *      Get the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 * Output:
 *      pAction     - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l2_portIpmcAction_get(rtk_port_t port, rtk_action_t *pAction)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_L2_PORTIPMCACTION_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pAction, &l2_cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_portIpmcAction_get */

/* Function Name:
 *      rtk_l2_portIpmcAction_set
 * Description:
 *      Set the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 *      action      - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_portIpmcAction_set(rtk_port_t port, rtk_action_t action)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&l2_cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_L2_PORTIPMCACTION_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_portIpmcAction_set */

/* Function Name:
 *      rtk_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
rtk_l2_ipMcastAddr_add(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ipmcastAddr, pIpmcastAddr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IPMCASTADDR_ADD, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pIpmcastAddr, &l2_cfg.ipmcastAddr, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipMcastAddr_add */

/* Function Name:
 *      rtk_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      pIpmcastAddr  - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *          input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid will be care.
 */
int32
rtk_l2_ipMcastAddr_del(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ipmcastAddr, pIpmcastAddr, sizeof(rtk_l2_ipMcastAddr_t));
    SETSOCKOPT(RTDRV_L2_IPMCASTADDR_DEL, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ipMcastAddr_del */

/* Function Name:
 *      rtk_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
rtk_l2_ipMcastAddr_get(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ipmcastAddr, pIpmcastAddr, sizeof(rtk_l2_ipMcastAddr_t));
    GETSOCKOPT(RTDRV_L2_IPMCASTADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pIpmcastAddr, &l2_cfg.ipmcastAddr, sizeof(rtk_l2_ipMcastAddr_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_ipMcastAddr_get */


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */


/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
rtk_l2_srcPortEgrFilterMask_get(rtk_portmask_t *pFilter_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_SRCPORTEGRFILTERMASK_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pFilter_portmask, &l2_cfg.filter_portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_srcPortEgrFilterMask_get */

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_srcPortEgrFilterMask_set(rtk_portmask_t *pFilter_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.filter_portmask, pFilter_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_SRCPORTEGRFILTERMASK_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_srcPortEgrFilterMask_set */

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_get
 * Description:
 *      Get extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
rtk_l2_extPortEgrFilterMask_get(rtk_portmask_t *pExt_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_L2_EXTPORTEGRFILTERMASK_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pExt_portmask, &l2_cfg.ext_portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_extPortEgrFilterMask_get */

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_set
 * Description:
 *      Set extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_extPortEgrFilterMask_set(rtk_portmask_t *pExt_portmask)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.ext_portmask, pExt_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_L2_EXTPORTEGRFILTERMASK_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_extPortEgrFilterMask_set */


/* Function Name:
 *      rtk_l2_camState_set
 * Description:
 *      Set LUT cam state 
 * Input:
 *      camState - enable or disable cam state 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_camState_set(rtk_enable_t camState)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((camState >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.enable, &camState, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L2_CAM_STATE_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_l2_camState_set */


/* Function Name:
 *      rtk_l2_camState_get
 * Description:
 *      Set LUT cam state 
 * Input:
 *      camState - enable or disable cam state 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_camState_get(rtk_enable_t *pCamState)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((pCamState == NULL), RT_ERR_NULL_POINTER);

    /* function body */   
   	GETSOCKOPT(RTDRV_L2_CAM_STATE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pCamState, &l2_cfg.enable, sizeof(rtk_enable_t));
   
    return RT_ERR_OK;
} /* end of rtk_l2_camState_get */

/* Function Name:
 *      rtk_l2_ip6mcReservedAddrEnable_get
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      type    - ip6 mc reserved address type 
 * Output:
 *      pEnable - treat as reserved addresss state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l2_ip6mcReservedAddrEnable_get(rtk_l2_ip6McReservedAddr_t type, rtk_enable_t *pEnable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l2_cfg.addrRsvIp6Mc, &type, sizeof(rtk_l2_ip6McReservedAddr_t));
    GETSOCKOPT(RTDRV_L2_IP6MCRSVADDR_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pEnable, &l2_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_l2_portLimitLearningCnt_get */


/* Function Name:
 *      rtk_l2_ip6mcReservedAddrEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      type    - ip6 mc reserved address type 
 *      enable  - treat as reserved state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l2_ip6mcReservedAddrEnable_set(rtk_l2_ip6McReservedAddr_t type, rtk_enable_t enable)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.addrRsvIp6Mc, &type, sizeof(rtk_l2_ip6McReservedAddr_t));
    osal_memcpy(&l2_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L2_IP6MCRSVADDR_SET, &l2_cfg, rtdrv_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l2_ip6mcReservedAddrEnable_set */

/* Function Name:
 *      rtk_l2_hashValue_get
 * Description:
 *      Calculate the hash value for specific hash type
 * Input:
 *      type        - hash type
 *      hashData    - input data to be hashed
 * Output:
 *      pHashValue  - hash result of specified type with data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
rtk_l2_hashValue_get(rtk_l2_hashType_t type, rtk_l2_hashData_t hashData, uint32 *pHashValue)
{
    rtdrv_l2Cfg_t l2_cfg;
    osal_memset(&l2_cfg, 0x0, sizeof(rtdrv_l2Cfg_t));

    /* function body */
    osal_memcpy(&l2_cfg.hashType, &type, sizeof(rtk_l2_hashType_t));
    osal_memcpy(&l2_cfg.hashData, &hashData, sizeof(rtk_l2_hashData_t));
    GETSOCKOPT(RTDRV_L2_HASHVALUE_GET, &l2_cfg, rtdrv_l2Cfg_t, 1);
    osal_memcpy(pHashValue, &l2_cfg.hashValue, sizeof(uint32));

    return RT_ERR_OK;
}

