/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39157 $
 * $Date: 2013-05-06 17:36:30 +0800 (Mon, 06 May 2013) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#ifndef __DAL_RTL9607C_STAT_H__
#define __DAL_RTL9607C_STAT_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define RTL9607C_STAT_RST_TIMEOUT       1000

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_rtl9607c_stat_init
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
extern int32
dal_rtl9607c_stat_init(void);


/* Function Name:
 *      dal_rtl9607c_stat_global_reset
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
extern int32
dal_rtl9607c_stat_global_reset(void);


/* Function Name:
 *      dal_rtl9607c_stat_port_reset
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
extern int32
dal_rtl9607c_stat_port_reset(rtk_port_t port);

/* Function Name:
 *      dal_rtl9607c_stat_log_reset
 * Description:
 *      Reset the specified ACL logging counters.
 * Input:
 *      index - logging index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
dal_rtl9607c_stat_log_reset(uint32 index);

/* Function Name:
 *      dal_rtl9607c_stat_rst_cnt_value_set
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
extern int32
dal_rtl9607c_stat_rst_cnt_value_set(rtk_mib_rst_value_t rstValue);

/* Function Name:
 *      dal_rtl9607c_stat_rst_cnt_value_get
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
extern int32
dal_rtl9607c_stat_rst_cnt_value_get(rtk_mib_rst_value_t *pRstValue);

/* Function Name:
 *      dal_rtl9607c_stat_global_get
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
extern int32
dal_rtl9607c_stat_global_get(rtk_stat_global_type_t cntrIdx, uint64 *pCntr);


/* Function Name:
 *      dal_rtl9607c_stat_global_getAll
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
extern int32
dal_rtl9607c_stat_global_getAll(rtk_stat_global_cntr_t *pGlobalCntrs);


/* Function Name:
 *      dal_rtl9607c_stat_port_get
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
extern int32
dal_rtl9607c_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr);


/* Function Name:
 *      dal_rtl9607c_stat_port_getAll
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
extern int32
dal_rtl9607c_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs);

/* Function Name:
 *      dal_rtl9607c_stat_log_get
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
extern int32
dal_rtl9607c_stat_log_get(uint32 index, uint64 *pCntr);

/* Function Name:
 *      dal_rtl9607c_stat_log_ctrl_set
 * Description:
 *      Set the acl log counters mode for 32-bits or 64-bits counter, and
 *      set the acl log counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL log counter
 *      ctrl 		- log counter control setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
dal_rtl9607c_stat_log_ctrl_set(uint32 index, rtk_stat_log_ctrl_t ctrl);

/* Function Name:
 *      dal_rtl9607c_stat_log_ctrl_get
 * Description:
 *      Get the acl counters mode for 32-bits or 64-bits counter, and
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
extern int32
dal_rtl9607c_stat_log_ctrl_get(uint32 index, rtk_stat_log_ctrl_t *pCtrl);

/* Function Name:
 *      dal_rtl9607c_stat_mib_cnt_mode_get
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
extern int32
dal_rtl9607c_stat_mib_cnt_mode_get(rtk_mib_count_mode_t *pCnt_mode);

/* Function Name:
 *      dal_rtl9607c_stat_mib_cnt_mode_set
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
extern int32
dal_rtl9607c_stat_mib_cnt_mode_set(rtk_mib_count_mode_t cnt_mode);

/* Function Name:
 *      dal_rtl9607c_stat_mib_latch_timer_get
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
extern int32
dal_rtl9607c_stat_mib_latch_timer_get(uint32 *pTimer);

/* Function Name:
 *      dal_rtl9607c_stat_mib_latch_timer_set
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
extern int32
dal_rtl9607c_stat_mib_latch_timer_set(uint32 timer);

/* Function Name:
 *      dal_rtl9607c_stat_mib_sync_mode_get
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
extern int32
dal_rtl9607c_stat_mib_sync_mode_get(rtk_mib_sync_mode_t *pSync_mode);

/* Function Name:
 *      dal_rtl9607c_stat_mib_sync_mode_set
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
extern int32
dal_rtl9607c_stat_mib_sync_mode_set(rtk_mib_sync_mode_t sync_mode);

/* Function Name:
 *      dal_rtl9607c_stat_pktInfo_get
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
extern int32
dal_rtl9607c_stat_pktInfo_get(rtk_port_t port, uint32 *pCode);


/* Function Name:
 *      dal_rtl9607c_stat_hostCnt_reset
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
extern int32
dal_rtl9607c_stat_hostCnt_reset(uint32 index);

/* Function Name:
 *      dal_rtl9607c_stat_hostCnt_get
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
extern int32
dal_rtl9607c_stat_hostCnt_get(uint32 index, rtk_stat_host_cnt_type_t cntrIdx, uint64 *pCntr);

/* Function Name:
 *      dal_rtl9607c_stat_hostState_set
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
extern int32
dal_rtl9607c_stat_hostState_set(
    uint32 index,
    rtk_enable_t    state);

/* Function Name:
 *      dal_rtl9607c_stat_hostState_get
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
extern int32
dal_rtl9607c_stat_hostState_get(
    uint32 index,
    rtk_enable_t    *pState);

#endif /* __DAL_RTL9607C_STAT_H__ */
