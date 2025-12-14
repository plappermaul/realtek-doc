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
#include <rtk/port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/stat.h>
#include <dal/dal_mgmt.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rtk_stat_init
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
rtk_stat_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_init */


/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED

 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_reset(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_global_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_global_reset();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_global_reset */


/* Function Name:
 *      rtk_stat_port_reset
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
rtk_stat_port_reset(rtk_port_t port)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_port_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_reset( port);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_port_reset */

/* Function Name:
 *      rtk_stat_log_reset
 * Description:
 *      Reset the specified ACL log counters.
 * Input:
 *      index - log index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_log_reset(uint32 index)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_log_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_log_reset( index);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_log_reset */

/* Function Name:
 *      rtk_stat_rstCntValue_set
 * Description:
 *      Set the counter value after reset
 * Input:
 *      None
 * Output:
 *      rstValue  - the counter value after reset
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_rstCntValue_set(rtk_mib_rst_value_t rstValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_rst_cnt_value_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_rst_cnt_value_set( rstValue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_rst_cnt_value_set */

/* Function Name:
 *      rtk_stat_rstCntValue_get
 * Description:
 *      Get the counter value after reset
 * Input:
 *      None
 * Output:
 *      pRstValue  - pointer buffer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_rstCntValue_get(rtk_mib_rst_value_t *pRstValue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_rst_cnt_value_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_rst_cnt_value_get( pRstValue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_rst_cnt_value_get */

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      cntrIdx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_get(rtk_stat_global_type_t cntrIdx, uint64 *pCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_global_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = RT_MAPPER->stat_global_get( cntrIdx, pCntr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_global_get */


/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      None
 * Output:
 *      pGlobalCntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_getAll(rtk_stat_global_cntr_t *pGlobalCntrs)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_global_getAll)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_global_getAll( pGlobalCntrs);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_global_getAll */


/* Function Name:
 *      rtk_stat_port_get
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
rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_port_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_get( port, cntrIdx, pCntr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_port_get */


/* Function Name:
 *      rtk_stat_port_getAll
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
rtk_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_port_getAll)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_port_getAll( port, pPortCntrs);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_port_getAll */

/* Function Name:
 *      rtk_stat_log_get
 * Description:
 *      Get ACL logging counter.
 * Input:
 *      cntrIdx  - logging index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_log_get(uint32 index, uint64 *pCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_log_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_log_get( index, pCntr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_log_get */

/* Function Name:
 *      rtk_stat_logCtrl_set
 * Description:
 *      Set the acl log counters mode for 32-bits or 64-bits counter, and
 *      set the acl log counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL log counter
 *      ctrl 		- log conter control setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_logCtrl_set(uint32 index, rtk_stat_log_ctrl_t ctrl)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_log_ctrl_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_log_ctrl_set( index, ctrl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_log_ctrl_set */

/* Function Name:
 *      rtk_stat_logCtrl_get
 * Description:
 *      Get the acl log counters mode for 32-bits or 64-bits counter, and
 *      get the acl log counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL log counter
 * Output:
 *      pCtrl 		- log counter control setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_logCtrl_get(uint32 index, rtk_stat_log_ctrl_t *pCtrl)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_log_ctrl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_log_ctrl_get( index, pCtrl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_logCtrl_get */

/* Function Name:
 *      rtk_stat_mibCntMode_get
 * Description:
 *      Get the MIB data update mode
 * Input:
 *      None
 * Output:
 *      pCnt_mode   - pointer buffer of MIB data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibCntMode_get(rtk_mib_count_mode_t *pCnt_mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_cnt_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_cnt_mode_get( pCnt_mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibCntMode_get */

/* Function Name:
 *      rtk_stat_mibCntMode_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      cnt_mode        - MIB counter update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibCntMode_set(rtk_mib_count_mode_t cnt_mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_cnt_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_cnt_mode_set( cnt_mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibCntMode_set */

/* Function Name:
 *      rtk_stat_mibLatchTimer_get
 * Description:
 *      Get the MIB latch timer
 * Input:
 *      None
 * Output:
 *      pTimer   - pointer buffer of MIB latch timer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibLatchTimer_get(uint32 *pTimer)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_latch_timer_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_latch_timer_get( pTimer);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibLatchTimer_get */

/* Function Name:
 *      rtk_stat_mibLatchTimer_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      timer        - MIB latch timer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibLatchTimer_set(uint32 timer)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_latch_timer_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_latch_timer_set( timer);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibLatchTimer_set */

/* Function Name:
 *      rtk_stat_mibSyncMode_get
 * Description:
 *      Get the MIB register data update mode
 * Input:
 *      None
 * Output:
 *      pSync_mode   - pointer buffer of MIB register data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibSyncMode_get(rtk_mib_sync_mode_t *pSync_mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_sync_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_sync_mode_get( pSync_mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibSyncMode_get */

/* Function Name:
 *      rtk_stat_mibSyncMode_set
 * Description:
 *      Set MIB register data update mode
 * Input:
 *      sync_mode        - MIB register data update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibSyncMode_set(rtk_mib_sync_mode_t sync_mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_sync_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_sync_mode_set( sync_mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibSyncMode_set */

/* Function Name:
 *      rtk_stat_mibCntTagLen_get
 * Description:
 *      Get counting Tag length state in tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 * Output:
 *      pState   - pointer buffer of count tx/rx tag length state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibCntTagLen_get(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_count_tag_length_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_count_tag_length_get( direction, pState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibCntTagLen_get */

/* Function Name:
 *      rtk_stat_mibCntTagLen_set
 * Description:
 *      Set counting length including Ctag length or excluding Ctag length for tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 *      enable    - count tag length state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibCntTagLen_set(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_mib_count_tag_length_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_mib_count_tag_length_set( direction, state);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_mibCntTagLen_set */

/* Function Name:
 *      rtk_stat_pktInfo_get
 * Description:
 *      Get the newest packet trap/drop reason
 * Input:
 *      port - port index
 * Output:
 *      pCode   - the newest packet trap/drop reason code
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
rtk_stat_pktInfo_get(rtk_port_t port, uint32 *pCode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_pktInfo_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_pktInfo_get( port, pCode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_pktInfo_get */

/* Function Name:
 *      rtk_stat_hostCnt_reset
 * Description:
 *      Reset the specified host statistic counters.
 * Input:
 *      index - host index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_hostCnt_reset(uint32 index)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_hostCnt_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_hostCnt_reset( index);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_hostCnt_reset */

/* Function Name:
 *     rtk_stat_hostCnt_get
 * Description:
 *      Get host statistic counters.
 * Input:
 *      index  - host index
 *      cntrIdx - specified host counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostCnt_get(uint32 index, rtk_stat_host_cnt_type_t cntrIdx, uint64 *pCntr)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_hostCnt_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_hostCnt_get(index, cntrIdx, pCntr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_hostCnt_get */

/* Function Name:
 *      rtk_rtl9602c_stat_hostState_get
 * Description:
 *      Get enable status of host statistic counting function
 * Input:
 *      index	- host index
 * Output:
 *      pState	-statistic counting state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial 
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostState_get(
    uint32 index,
    rtk_enable_t    *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->stat_hostState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->stat_hostState_get(index, pState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stat_hostState_get */

/* Function Name:
 *      rtk_stat_hostState_set
 * Description:
 *      Set state of host statistic counting function
 * Input:
 *      index	- host index
 *      state	- statistic counting state.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial 
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostState_set(
    uint32 index,
    rtk_enable_t    state)
{
		int32	ret;
	
		/* function body */
		if (NULL == RT_MAPPER->stat_hostState_set)
			return RT_ERR_DRIVER_NOT_FOUND;
	
		RTK_API_LOCK();
		ret = RT_MAPPER->stat_hostState_set(index, state);
		RTK_API_UNLOCK();
		return ret;
	} /* end of rtk_stat_hostState_set */



