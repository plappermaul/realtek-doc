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
 * Purpose : Definition of GPON API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) GPON
 */


#ifndef __DAL_RT_RTL8277C_GPON_H__
#define __DAL_RT_RTL8277C_GPON_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_gpon.h>

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
 *      dal_rt_rtl8277c_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_init(void);

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rt_rtl8277c_gpon_deinit
 * Description:
 *      gpon register level de-initial function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_deinit(uint8 pon_mode);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_serialNumber_set
 * Description:
 *      Set GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_serialNumber_get
 * Description:
 *      Get GPON serial number
 * Input:
 *      None
 * Output:
 *      pSN   - pointer to the GPON serial number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_serialNumber_get(rt_gpon_serialNumber_t *pSN);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_registrationId_set
 * Description:
 *      Set GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_registrationId_get
 * Description:
 *      Get GPON registration ID (password in 984.3).
 * Input:
 *      None
 * Output:
 *      pRegId   - pointer to the GPON registration id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_registrationId_get(rt_gpon_registrationId_t *pRegId);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState   - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      The GPON MAC is starting to work now.
 */
extern int32 dal_rt_rtl8277c_gpon_activate(rt_gpon_initialState_t initState);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_deactivate
 * Description:
 *      GPON MAC Deactivate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The GPON MAC is out of work now.
*/
extern int32 dal_rt_rtl8277c_gpon_deactivate(void);

/* Function Name:
 *      dal_rtl8277c_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_onuState_get(rt_gpon_onuState_t  *pOnuState);



/* Function Name:
 *      dal_rt_rtl8277c_gpon_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      msgLen   - length of the OMCI message to be transmitted
 *      pMsg     - pointer to the OMCI message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_omci_tx(uint32 msgLen, uint8 *pMsg);

/* Function Name:
 *      rt_gpon_omci_rx_callback_register
 * Description:
 *      Register OMCI RX callback function.
 * Input:
 *      omciRx   - pointer to the callback function for OMCI RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_tcont_set
 * Description:
 *      Set GPON tcont
 * Input:
 *      tcontId   - tcont index
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Failed
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_tcont_set(uint32 *pTcontId, uint32 allocId);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_tcont_get
 * Description:
 *      Get GPON allocation id
 * Input:
 *      tcontId   - tcont index
 * Output:
 *      pAllocId  - pointer to the allocation id 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_tcont_del
 * Description:
 *      Delete GPON tcont
 * Input:
 *      tcontId   - tcont index
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int dal_rt_rtl8277c_gpon_tcont_del(uint32 tcontId);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_usFlow_set
 * Description:
 *      Set GPON upstream flow
 * Input:
 *      usFlowId   - upstream flow id
 *      pUsFlow    - the pointer of upstream flow config
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_usFlow_set(uint32 usFlowId, const rt_gpon_usFlow_t *pUsFlow);


 /* Function Name:
 *      dal_rt_rtl8277c_gpon_usFlow_get
 * Description:
 *      Get the upstream flow
 * Input:
 *      usFlowId  - upstream flow id
 * Output:
 *      pUsFlow   - the pointer of upstream flow config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_usFlow_del
 * Description:
 *      Delete GPON upstream flow
 * Input:
 *      usFlowId   - upstream flow id
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_usFlow_del(uint32 usFlowId);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_usFlow_delAll
 * Description:
 *      Delete all GPON upstream flow
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_usFlow_delAll(void);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_dsFlow_set
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 *      pDsFlow    - the pointer of downstream flow config
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_dsFlow_set(uint32 dsFlowId, const rt_gpon_dsFlow_t *pDsFlow);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_dsFlow_get
 * Description:
 *      Get GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 * Output:
 *      pDsFlow    - the pointer of downstream flow config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_dsFlow_del
 * Description:
 *      Delete GPON downstream flow
 * Input:
 *      dsFlowId   - donwstream flow id
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_dsFlow_del(uint32 dsFlowId);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_dsFlow_delAll
 * Description:
 *      Delete all GPON downstream flow
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_dsFlow_delAll(void);

/* Function Name:
 *      dal_rt_rtl8277c_loop_gemport_set
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      gemPortId   - GEM port ID
 *      enable    - Enable/disble loop GEM port function
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
extern int32 dal_rt_rtl8277c_loop_gemPort_set(uint32 gemPortId, rt_enable_t enable);

/* Function Name:
 *      dal_rt_rtl8277c_loop_gemPort_get
 * Description:
 *      Set GPON downstream flow
 * Input:
 *      gemPortId   - GEM port ID
 *
 * Output:
 *      pEnable    - Enable/disble loop GEM port function
 *
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
extern int32 dal_rt_rtl8277c_loop_gemPort_get(uint32 gemPortId, rt_enable_t *pEnable);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      tcontId    - tcont index
 *      tcQueueId  - tcont queue index
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.       
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_ponQueue_set(
    uint32 tcontId, 
    uint32 tcQueueId,  
    const rt_gpon_queueCfg_t *pQueuecfg);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      tcontId    - tcont index
 *      tcQueueId  - tcont queue index
 * Output:
 *      pQueuecfg  - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.        
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 dal_rt_rtl8277c_gpon_ponQueue_get(
    uint32 tcontId, 
    uint32 tcQueueId,  
    rt_gpon_queueCfg_t *pQueuecfg);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      tcontId     - tcont index
 *      tcQueueId   - tcont queue index
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_ponQueue_del(    
    uint32 tcontId, 
    uint32 tcQueueId);



/* Function Name:
 *      dal_rt_rtl8277c_gpon_scheInfo_get
 * Description:
 *      Get scheduling information
 * Input:
 *      pScheInfo  - pointer to the scheduling information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_flowCounter_get
 * Description:
 *      Get the flow counters.
 * Input:
 *      flowId   - logical flow ID
 *      flowCntType     - counter type
 * Output:
 *      pFlowCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_pmCounter_get
 * Description:
 *      Get the pm counters.
 * Input:
 *      pmCntType     - counter type
 * Output:
 *      pPmCnt -  pointer to the pm counters
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ponTag_get
 * Description:
 *      Get the PON Tag.
 * Input:
 *      None
 * Output:
 *      pPonTag       - pointer to the PON TAG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_msk_set
 * Description:
 *      Set the Master Session Key.
 * Input:
 *      pMsk - pointer to the Master Session Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_msk_set(rt_gpon_msk_t *pMsk);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_omci_mic_generate
 * Description:
 *      Generate the OMCI MIC value according to the inputs.
 * Input:
 *      dir     - the direction of the OMCI msg, 0x01 for Downstream and 0x02 for Upstream
 *      pMsg    - pointer to the OMCI message data
 *      msgLen  - length of the OMCI message, not include MIC
 * Output:
 *      mic     - pointer to the generated MIC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl8277c_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_mcKey_set
 * Description:
 *      Set the Multicast Key.
 * Input:
 *      pMcKey - pointer to the Multicast Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 dal_rt_rtl8277c_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_attribute_set
 * Description:
 *      Set the gpon attribute.
 * Input:
 *      attributeType -  attribute type
 *      pAttributeValue - pointer to the attribute value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_attribute_set(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_attribute_get
 * Description:
 *      Get the gpon attribute.
 * Input:
 *      attributeType -  attribute type
 * Output:
 *      pAttributeValue - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_attribute_get(rt_gpon_attribute_type_t attributeType, rt_gpon_attribute_t *pAttributeValue);

 /* Function Name:
 *      dal_rt_rtl8277c_gpon_usFlow_phyData_get
 * Description:
 *      Get the physical data of upstream flow
 * Input:
 *      usFlowId    - upstream flow id
 *      pTcontId    - pointer to the physical T-CONT ID value
 *      pTcQueueId  - pointer to the physical T-CONT Queue ID value
 *      pGemIdx     - pointer to the physical GEM index value
 * Output:
 *      pUsFlow   - the pointer of upstream flow config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_usFlow_phyData_get(uint32 usFlowId, uint32 *pTcontId, uint32 *pTcQueueId, uint32 *pGemIdx);


/* Function Name:
 *      dal_rt_rtl8277c_gpon_omcc_get
 * Description:
 *      get omcc information.
 * Input:
 *      
 * Output:
 *      pOmcc                 -   omcc information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_omcc_get(rt_gpon_omcc_t *pOmcc);




/* Function Name:
 *      dal_rt_rtl8277c_gpon_omci_mirror_set
 * Description:
 *      set gpon omci mirror function.
 * Input:
 *      enable          - enable mirrior function
 *      mirroringPort   - Select      
 * Output:
 *      pOmcc                 -   omcc information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort);




/* Function Name:
 *      dal_rt_rtl8277c_gpon_fec_get
 * Description:
 *      get gpon fec status function.
 * Input:
 *
 * Output:
 *      pFecStatus               -   fec status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl8277c_gpon_fec_get(rt_gpon_fec_status_t *pFecStatus);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ploam_rx_register
 * Description:
 *      register ploam rx callback
 * Input:
 *      func            - callback func
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - null callback func
 * Note:
 */
extern int32 
dal_rt_rtl8277c_gpon_ploam_rx_register(rt_gpon_ploam_rx_callback func);

/* Function Name:
 *      dal_rt_rtl8277c_gpon_omci_rx_callback_register
 * Description:
 *      Register OMCI RX callback function.
 * Input:
 *      omciRx   - pointer to the callback function for OMCI RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_rt_rtl8277c_gpon_fttr_omci_rx_callback_register(rt_gpon_omci_rx_callback fttrOmciRx);

#endif /* __DAL_RT_RTL8277C_GPON_H__ */
