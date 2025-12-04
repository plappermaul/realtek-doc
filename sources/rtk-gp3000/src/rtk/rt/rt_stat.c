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
#include <common/rt_type.h>
#include <rtk/rt/rt_port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_stat.h>

#ifdef CONFIG_EXTERNAL_SWITCH
#include <dal/dal_ext_switch_mapper.h>
#endif


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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stat_init();
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

#if defined(CONFIG_EXTERNAL_SWITCH)
    if(IS_EXT_SWITCH_PORT(port))
    {
        /* function body */
        if (NULL == RT_EXT_SWITCH_MAPPER || NULL == RT_EXT_SWITCH_MAPPER->stat_port_reset)
            return RT_ERR_DRIVER_NOT_FOUND;      
        RTK_API_LOCK();
        ret = RT_EXT_SWITCH_MAPPER->stat_port_reset(port);
        RTK_API_UNLOCK();
        return ret;
    }
#endif

    /* function body */
    if (NULL == RT_MAPPER->stat_port_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_reset(port);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_port_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_get(port, cntrIdx, pCntr);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

#if defined(CONFIG_EXTERNAL_SWITCH)
    if(IS_EXT_SWITCH_PORT(port))
    {
        /* function body */
        if (NULL == RT_EXT_SWITCH_MAPPER || NULL == RT_EXT_SWITCH_MAPPER->stat_port_getAll)
            return RT_ERR_DRIVER_NOT_FOUND;      
        RTK_API_LOCK();
        ret = RT_EXT_SWITCH_MAPPER->stat_port_getAll(port, (rtk_stat_port_cntr_t *)pPortCntrs);
        RTK_API_UNLOCK();
        return ret;
    }
#endif

    /* function body */
    if (NULL == RT_MAPPER->stat_port_getAll)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_getAll(port, (rtk_stat_port_cntr_t *)pPortCntrs);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_stat_vlanCnt_filter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_stat_vlanCnt_filter_set(index, filter);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_stat_vlanCnt_filter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_stat_vlanCnt_filter_get(index, pFilter);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_stat_vlanCnt_filter_get */

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
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_mib_get(uint32 index, rt_stat_port_cntr_t *pPortCntrs)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_stat_vlanCnt_mib_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_stat_vlanCnt_mib_get(index, pPortCntrs);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_stat_vlanCnt_mib_get */

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
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
int32
rt_stat_vlanCnt_mib_reset(uint32 index)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_stat_vlanCnt_mib_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_stat_vlanCnt_mib_reset(index);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_stat_vlanCnt_mib_reset */


