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
 * $Revision: 61949 $
 * $Date: 2015-09-15 20:10:29 +0800 (Tue, 15 Sep 2015) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
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
#include <rtk/rt/rt_port.h>


/*
 * Symbol Definition
 */

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rt_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
int32
rt_stat_init(void)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_STAT_INIT, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_stat_init */

/* Function Name:
 *      rt_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rt_stat_port_reset(rt_port_t port)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* function body */
    osal_memcpy(&rt_stat_cfg.port, &port, sizeof(rt_port_t));
    SETSOCKOPT(RTDRV_RT_STAT_PORT_RESET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_stat_port_reset */

/* Function Name:
 *      rt_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rt_stat_port_get(rt_port_t port, rt_stat_port_type_t cntrIdx, uint64 *pCntr)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_stat_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_stat_cfg.cntrIdx, &cntrIdx, sizeof(rt_stat_port_type_t));
    GETSOCKOPT(RTDRV_RT_STAT_PORT_GET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);
    osal_memcpy(pCntr, &rt_stat_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rt_stat_port_get */


/* Function Name:
 *      rt_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rt_stat_port_getAll(rt_port_t port, rt_stat_port_cntr_t *pPortCntrs)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_stat_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_STAT_PORT_GETALL, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);
    osal_memcpy(pPortCntrs, &rt_stat_cfg.portCntrs, sizeof(rt_stat_port_cntr_t));

    return RT_ERR_OK;
}   /* end of rt_stat_port_getAll */


/* Function Name:
 *      rt_stat_vlanCnt_filter_set
 * Description:
 *      Set vlan counter filter 
 * Input:
 *      index        - filter index
 *      filter       - vlan counter filer configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_filter_set(uint32 index, rt_stat_vlanCnt_filter_t filter)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* function body */
    osal_memcpy(&rt_stat_cfg.index, &index, sizeof(uint32));
    osal_memcpy(&rt_stat_cfg.filter, &filter, sizeof(rt_stat_vlanCnt_filter_t));
    SETSOCKOPT(RTDRV_RT_STAT_VLANCNT_FILTER_SET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_stat_vlanCnt_filter_set */


/* Function Name:
 *      rt_stat_vlanCnt_filter_get
 * Description:
 *      Get vlan counter filter 
 * Input:
 *      index        - filter index
 * Output:
 *      pFilter       - vlan counter filer configure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_filter_get(uint32 index, rt_stat_vlanCnt_filter_t *pFilter)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_stat_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_STAT_VLANCNT_FILTER_GET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);
    osal_memcpy(pFilter, &rt_stat_cfg.filter, sizeof(rt_stat_vlanCnt_filter_t));

    return RT_ERR_OK;
}   /* end of rt_stat_vlanCnt_filter_get */

/* Function Name:
 *      rt_stat_vlanCnt_mib_get
 * Description:
 *      Get vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_mib_get(uint32 index, rt_stat_port_cntr_t *pPortCntrs)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_stat_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_STAT_VLANCNT_MIB_GET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);
    osal_memcpy(pPortCntrs, &rt_stat_cfg.portCntrs, sizeof(rt_stat_port_cntr_t));

    return RT_ERR_OK;
}   /* end of rt_stat_vlanCnt_mib_get */

/* Function Name:
 *      rt_stat_vlanCnt_mib_reset
 * Description:
 *      Reset vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_mib_reset(uint32 index)
{
    rtdrv_rt_statCfg_t rt_stat_cfg;
    
    memset(&rt_stat_cfg, 0x0, sizeof(rtdrv_rt_statCfg_t));

    /* function body */
    osal_memcpy(&rt_stat_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_STAT_VLANCNT_MIB_RESET, &rt_stat_cfg, rtdrv_rt_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_stat_vlanCnt_mib_reset */