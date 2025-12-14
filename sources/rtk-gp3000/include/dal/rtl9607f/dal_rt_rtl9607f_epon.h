/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
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
 * Purpose : Definition of EPON API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) EPON
 */


#ifndef __DAL_RT_RTL9607F_EPON_H__
#define __DAL_RT_RTL9607F_EPON_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_epon.h>

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
 *      dal_rt_rtl9607f_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern 
int32 dal_rt_rtl9607f_epon_init(void);

/* Function Name:
 *      dal_rt_rtl9607f_epon_deinit
 * Description:
 *      epon register level de-initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern 
int32 dal_rt_rtl9607f_epon_deinit(uint8 pon_mode);

/* Function Name:
 *      dal_rt_rtl9607f_epon_oam_tx
 * Description:
 *      Transmit OAM message.
 * Input:
 *      msgLen   - length of the OAM message to be transmitted
 *      pMsg     - pointer to the OAM message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_oam_tx(uint32 msgLen, uint8 *pMsg, uint8 llidIdx);

/* Function Name:
 *      dal_rt_rtl9607f_epon_oam_rx_callback_register
 * Description:
 *      Register OAM RX callback function.
 * Input:
 *      oamRx   - pointer to the callback function for OAM RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx);




/* Function Name:
 *      dal_rtl9607f_epon_mpcp_info_get
 * Description:
 *      Get epon mpcp information
 * Input:
 * Output:
 *      info - mpcp relative information 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *
 *   This API routine is used to get generic ONU MPCP information and can be invoked at any time. 
 *   However some of fields like laser on laser_off sync_time and olt_discinfo will be valid if ONU is not in registered
 *   state. Note that field onu_discinfo and olt_discinfo are only appropriate for 10G ONU. 
 */
extern int32 
dal_rtl9607f_epon_mpcp_info_get(rt_epon_mpcp_info_t *info);


/* Function Name:
 *      dal_rtl9607f_epon_mpcp_queue_threshold_set
 * Description:
 *      Set epon threshold report
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 *      The the report level 1 for normal report type, max value is 4.
 */
extern int32 dal_rtl9607f_epon_mpcp_queue_threshold_set(rt_epon_report_threshold_t *pThresholdRpt);

/* Function Name:
 *      dal_rtl9607f_epon_mpcp_queue_threshold_get
 * Description:
 *      Get epon threshold report setting
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 */
extern int32
dal_rtl9607f_epon_mpcp_queue_threshold_get(rt_epon_report_threshold_t *pThresholdRpt);

/* Function Name:
 *      dal_rt_rtl9607f_epon_dyinggasp_set
 * Description:
 *      Transmit dying gasp OAM message.
 * Input:
 *      msgLen   - length of the dying gasp OAM message to be transmitted
 *      pMsg     - pointer to the dying gasp OAM message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_dyinggasp_set(uint32 msgLen, uint8 *pMsg);

/* Function Name:
 *      dal_rt_rtl9607f_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_registerReq_get(rt_epon_regReq_t *pRegEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_registerReq_set(rt_epon_regReq_t *pRegEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry);

/* Function Name:
 *      dal_rt_rtl9607f_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_usFecState_get(rt_enable_t *pState);

/* Function Name:
 *      dal_rt_rtl9607f_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_usFecState_set(rt_enable_t state);

/* Function Name:
 *      dal_rtl9607f_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_dsFecState_get(rt_enable_t *pState);

/* Function Name:
 *      dal_rt_rtl9607f_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_dsFecState_set(rt_enable_t state);

/* Function Name:
 *      dal_rt_rtl9607f_epon_mibCounter_get
 * Description:
 *      Get EPON MIB Counter
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
dal_rt_rtl9607f_epon_mibCounter_get(rt_epon_counter_t *pCounter);



/* Function Name:
 *      dal_rt_rtl9607f_epon_mibCounter_clear
 * Description:
 *      Clear EPON MIB Counter
 * Input:
 *       None
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 
dal_rt_rtl9607f_epon_mibCounter_clear(void);


/* Function Name:
 *      dal_rt_rtl9607f_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rt_rtl9607f_epon_losState_get(rt_enable_t *pState);



/* Function Name:
 *      dal_rt_rtl9607f_epon_lofState_get
 * Description:
 *      Get laser lose of frame state.
 * Input:
 *      pState LOF state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 
dal_rt_rtl9607f_epon_lofState_get(rt_enable_t *pState);



/* Function Name:
 *      dal_rt_rtl9607f_epon_mpcp_gate_timer_set
 * Description:
 *      Set mpcp gate expire timer and deregister state.
 * Input:
 *      gate_timer      gate expire timer
 *      deregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 
dal_rt_rtl9607f_epon_mpcp_gate_timer_set(uint32 gateTimer,rt_enable_t deregistration);




/* Function Name:
 *      dal_rt_rtl9607f_epon_mpcp_gate_timer_get
 * Description:
 *      Get mpcp gate expire timer and deregister state.
 * Input:
 *      *pGate_timer      gate expire timer
 *      *pDeregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 
dal_rt_rtl9607f_epon_mpcp_gate_timer_get(uint32 *pGateTimer,rt_enable_t *pDeregistration);


/* Function Name:
 *      dal_rt_rtl9607f_epon_info_notify_callback_register
 * Description:
 *      set epon info notify callback
 * Input:
 *      callBack - call back function pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
extern int32 
dal_rt_rtl9607f_epon_info_notify_callback_register(rt_epon_info_notify_cb callBack);


/* Function Name:
 *      dal_rt_rt9607f_epon_ponLoopback_set
 * Description:
 *      config pon port loopback
 * Input:
 *      enable - set loopback status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 dal_rt_rt9607f_epon_ponLoopback_set(uint32 llid,rt_enable_t enable);



#endif /* __DAL_RT_RTL9607F_EPON_H__ */