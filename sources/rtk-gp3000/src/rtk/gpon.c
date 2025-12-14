/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/gpon.h>
#include <rtk/classify.h>
#include <hal/common/halctrl.h>


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name    : GPON     */


/* Function Name:
 *      rtk_gpon_driver_initialize
 * Description:
 *      GPON MAC Drv Initialization.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The first function call for GPON MAC Drv.
 */
int32
rtk_gpon_driver_initialize(void)
{
    return RT_MAPPER->gpon_driver_initialize();
}

/* Function Name:
 *      rtk_gpon_driver_deInitialize
 * Description:
 *      To Stop the GPON MAC Drv. The last function call for GPON MAC Drv.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The Device should be de-initialized before this function call.
 */
int32 rtk_gpon_driver_deInitialize(void)
{
    return RT_MAPPER->gpon_driver_deInitialize();
}

/* Function Name:
 *      rtk_gpon_device_initialize
 * Description:
 *      GPON MAC Device Initialization. To start the device of the GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      This function should be called after the Drv Initialization and before any other operation.
 *      It should be called after Device is de-initialized but the Drv is not be de-initialized.
 */
int32 rtk_gpon_device_initialize(void)
{
#if defined(CONFIG_EPON_FEATURE)
    int32 ret;
#endif
    rtk_port_t pon = HAL_GET_PON_PORT();
    /*force change cf port to pon and enable it*/
    rtk_classify_cfSel_set(pon,CLASSIFY_CF_SEL_ENABLE);
    #if defined(CONFIG_EPON_FEATURE)
    if((ret = rtk_oam_multiplexerAction_set(pon,OAM_MULTIPLEXER_ACTION_FORWARD)) != RT_ERR_OK){
		return RT_ERR_FAILED;
    }
    #endif

    return RT_MAPPER->gpon_device_initialize();
}

/* Function Name:
 *      rtk_gpon_device_deInitialize
 * Description:
 *      GPON MAC Device De-Initialization. To stop the device of the GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the Drv is de-initialized and the GPON MAC is not activated.
 */
int32 rtk_gpon_device_deInitialize(void)
{
    return RT_MAPPER->gpon_device_deInitialize();
}

#if 0 /* move to k_gpon.c, scott */
/* Function Name:
 *      rtk_gpon_eventHandler_stateChange_reg
 * Description:
 *      This function is called to register the callback function of the State Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_stateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_stateChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_dsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the D/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_dsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_dsFecChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_usFecChange_reg
 * Description:
 *      This function is called to register the callback function of the U/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_usFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_usFecChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_usPloamUrgEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM urgent queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_usPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    return RT_MAPPER->gpon_eventHandler_usPloamUrgEmpty_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_usPloamNrmEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM normal queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_usPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    return RT_MAPPER->gpon_eventHandler_usPloamNrmEmpty_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_ploam_reg
 * Description:
 *      This function is called to register the event handler of the Rx PLOAM.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_ploam_reg(rtk_gpon_eventHandleFunc_ploam_t func)
{
    return RT_MAPPER->gpon_eventHandler_ploam_reg(func);
}

/* Function Name:
 *      rtk_gpon_callback_queryAesKey_reg
 * Description:
 *      This function is called to register the callback function of the AES Key Query.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_callback_queryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func)
{
    return RT_MAPPER->gpon_callback_queryAesKey_reg(func);
}

/* Function Name:
 *      rtk_gpon_eventHandler_alarm_reg
 * Description:
 *      This function is called to register the alarm event handler of the alarm.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_eventHandler_alarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func)
{
    return RT_MAPPER->gpon_eventHandler_alarm_reg(alarmType, func);
}

/* Function Name:
 *      rtk_gpon_serialNumber_set
 * Description:
 *      GPON MAC Set Serial Number.
 * Input:
 *      pSN             - the pointer of Serial Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_serialNumber_set(rtk_gpon_serialNumber_t *pSN)
{
    return RT_MAPPER->gpon_serialNumber_set(pSN);
}

/* Function Name:
 *      rtk_gpon_serialNumber_get
 * Description:
 *      GPON MAC get Serial Number.
 * Input:
 *      pSN             - the pointer of Serial Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_serialNumber_get(rtk_gpon_serialNumber_t *pSN)
{
    return RT_MAPPER->gpon_serialNumber_get(pSN);
}

/* Function Name:
 *      rtk_gpon_password_set
 * Description:
 *      GPON MAC set Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_password_set(rtk_gpon_password_t *pPwd)
{
    return RT_MAPPER->gpon_password_set(pPwd);
}

/* Function Name:
 *      rtk_gpon_password_get
 * Description:
 *      GPON MAC get Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_password_get(rtk_gpon_password_t *pPwd)
{
    return RT_MAPPER->gpon_password_get(pPwd);
}

/* Function Name:
 *      rtk_gpon_parameter_set
 * Description:
 *      GPON MAC set parameters.
 * Input:
 *      type            - the parameter type
 *      pPwd            - the pointer of Parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara)
{
    return RT_MAPPER->gpon_parameter_set(type, pPara);
}

/* Function Name:
 *      rtk_gpon_parameter_get
 * Description:
 *      GPON MAC get parameters, which is set by rtk_gpon_parameter_set.
 * Input:
 *      type            - the parameter type
 *      pPwd            - the pointer of Parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara)
{
    return RT_MAPPER->gpon_parameter_get(type, pPara);
}

/* Function Name:
 *      rtk_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState       - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is working now.
 */
int32 rtk_gpon_activate(rtk_gpon_initialState_t initState)
{
    return RT_MAPPER->gpon_activate(initState);
}

/* Function Name:
 *      rtk_gpon_deActivate
 * Description:
 *      GPON MAC de-Activate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is out of work now.
 */
int32 rtk_gpon_deActivate(void)
{
    return RT_MAPPER->gpon_deActivate();
}

/* Function Name:
 *      rtk_gpon_ponStatus_get
 * Description:
 *      GPON MAC Get PON Status.
 * Input:
 *      pStatus         - pointer of status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
int32 rtk_gpon_ponStatus_get(rtk_gpon_fsm_status_t* pStatus)
{
    return RT_MAPPER->gpon_ponStatus_get(pStatus);
}

/* Function Name:
 *      rtk_gpon_isr_entry
 * Description:
 *      GPON MAC ISR entry
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Should be called in interrupt process or a polling thread
 */
void rtk_gpon_isr_entry(void)
{
    return RT_MAPPER->gpon_isr_entry();
}

/* Function Name:
 *      rtk_gpon_tcont_create
 * Description:
 *      GPON MAC Create a TCont by assigning an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A TCont ID is returned in pAttr.
 */
int32 rtk_gpon_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    return RT_MAPPER->gpon_tcont_create(pInd, pAttr);
}

/* Function Name:
 *      rtk_gpon_tcont_destroy
 * Description:
 *      GPON MAC Remove a TCont.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_tcont_destroy(rtk_gpon_tcont_ind_t *pInd)
{
    return RT_MAPPER->gpon_tcont_destroy(pInd);
}

/* Function Name:
 *      rtk_gpon_tcont_destroy
 * Description:
 *      GPON MAC Get a TCont with an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The TCont ID is returned in pAttr.
 */
int32 rtk_gpon_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    return RT_MAPPER->gpon_tcont_get(pInd, pAttr);
}

/* Function Name:
 *      rtk_gpon_dsFlow_set
 * Description:
 *      GPON MAC set a D/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_dsFlow_set(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_dsFlow_get
 * Description:
 *      GPON MAC get a D/S flow.
 * Input:
 *      flowId          - the flow id
 * Output:
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_dsFlow_get(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_usFlow_set
 * Description:
 *      GPON MAC set a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *aAttr)
{
    return RT_MAPPER->gpon_usFlow_set(flowId, aAttr);
}

/* Function Name:
 *      rtk_gpon_usFlow_set
 * Description:
 *      GPON MAC get a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *aAttr)
{
    return RT_MAPPER->gpon_usFlow_get(flowId, aAttr);
}

/* Function Name:
 *      rtk_gpon_ploam_send
 * Description:
 *      GPON MAC Send a PLOAM in upstream.
 * Input:
 *      urgent          - specify it is a urgent(1) or normal(0) PLOAM message
 *      pPloam          - the pointer of PLOAM message
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A error is returned if the PLOAM is not sent.
 */
int32 rtk_gpon_ploam_send(int32 urgent, rtk_gpon_ploam_t *pPloam)
{
    return RT_MAPPER->gpon_ploam_send(urgent, pPloam);
}

/* Function Name:
 *      rtk_gpon_broadcastPass_set
 * Description:
 *      GPON MAC set the broadcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the broadcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_broadcastPass_set(int32 mode)
{
    return RT_MAPPER->gpon_broadcastPass_set(mode);
}

/* Function Name:
 *      rtk_gpon_broadcastPass_get
 * Description:
 *      GPON MAC get the broadcast pass mode.
 * Input:
 *      pMode           - the pointer of broadcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_broadcastPass_get(int32 *pMode)
{
    return RT_MAPPER->gpon_broadcastPass_get(pMode);
}

/* Function Name:
 *      rtk_gpon_nonMcastPass_set
 * Description:
 *      GPON MAC set the non-multidcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the non-multidcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_nonMcastPass_set(int32 mode)
{
    return RT_MAPPER->gpon_nonMcastPass_set(mode);
}

/* Function Name:
 *      rtk_gpon_nonMcastPass_get
 * Description:
 *      GPON MAC get the non-multidcast pass mode.
 * Input:
 *      pMode           - the pointer of non-multidcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_nonMcastPass_get(int32 *pMode)
{
    return RT_MAPPER->gpon_nonMcastPass_get(pMode);
}

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_set
 * Description:
 *      GPON MAC set the address pattern.
 * Input:
 *      ipv4_pattern    - Address pattern of DA[47:24] for IPv4 packets.
 *      ipv6_pattern    - Address pattern of DA[47:32] for IPv6 packets.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern)
{
    return RT_MAPPER->gpon_multicastAddrCheck_set(ipv4_pattern, ipv6_pattern);
}

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_get
 * Description:
 *      GPON MAC get the address pattern.
 * Input:
 *      pIpv4_pattern    - Address pattern of DA[47:24] for IPv4 packets..
 *      pIpv6_pattern    - Address pattern of DA[47:24] for IPv6 packets..
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern)
{
    return RT_MAPPER->gpon_multicastAddrCheck_get(pIpv4_Pattern, pIpv6_Pattern);
}

/* Function Name:
 *      rtk_gpon_macFilterMode_set
 * Description:
 *      GPON MAC set the mac filter mode.
 * Input:
 *      mode            - MAC table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode)
{
    return RT_MAPPER->gpon_macFilterMode_set(mode);
}

/* Function Name:
 *      rtk_gpon_macFilterMode_get
 * Description:
 *      GPON MAC get the mac filter mode.
 * Input:
 *      pMode           - pointer of MAC filter table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode)
{
    return RT_MAPPER->gpon_macFilterMode_get(pMode);
}

/* Function Name:
 *      rtk_gpon_mcForceMode_set
 * Description:
 *      GPON MAC set the multicast force mode.
 * Input:
 *      ipv4            - IPv4 multicast force mode.
 *      ipv6            - IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    return RT_MAPPER->gpon_mcForceMode_set(ipv4,ipv6);
}

/* Function Name:
 *      rtk_gpon_mcForceMode_get
 * Description:
 *      GPON MAC get the multicast force mode.
 * Input:
 *      pIpv4           - The pointer of IPv4 multicast force mode.
 *      pIv6            - The pointer of IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6)
{
    return RT_MAPPER->gpon_mcForceMode_get(pIpv4,pIpv6);
}

/* Function Name:
 *      rtk_gpon_macEntry_add
 * Description:
 *      GPON MAC Add a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_add(rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_add(pEntry);
}

/* Function Name:
 *      rtk_gpon_macEntry_del
 * Description:
 *      GPON MAC Remove a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_del(rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_del(pEntry);
}

/* Function Name:
 *      rtk_gpon_macEntry_get
 * Description:
 *      GPON MAC Get a MAC entry by the table index.
 * Input:
 *      index           - index of MAC filter table entry.
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_get(index, pEntry);
}

/* Function Name:
 *      rtk_gpon_rdi_set
 * Description:
 *      GPON MAC set the RDI indicator in upstream.
 * Input:
 *      enable          - specify to turn on/off RDI.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_rdi_set(int32 enable)
{
    return RT_MAPPER->gpon_rdi_set(enable);
}

/* Function Name:
 *      rtk_gpon_rdi_get
 * Description:
 *      GPON MAC get the RDI indicator in upstream.
 * Input:
 *      pEnable         - the pointer of RDI indicator.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_rdi_get(int32 *pEnable)
{
    return RT_MAPPER->gpon_rdi_get(pEnable);
}

/* Function Name:
 *      rtk_gpon_powerLevel_set
 * Description:
 *      GPON MAC set ONU power level, it will update the TT field of
 *      Serial_Number_ONU PLOAMu message.
 * Input:
 *      level           - the power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_powerLevel_set(uint8 level)
{
    return RT_MAPPER->gpon_powerLevel_set(level);
}

/* Function Name:
 *      rtk_gpon_powerLevel_set
 * Description:
 *      GPON MAC get ONU power level.
 * Input:
 *      pLevel          - the pointer of power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_powerLevel_get(uint8 *pLevel)
{
    return RT_MAPPER->gpon_powerLevel_get(pLevel);
}

/* Function Name:
 *      rtk_gpon_alarmStatus_get
 * Description:
 *      GPON MAC get the alarm status.
 * Input:
 *      alarm           - the alarm type.
 *      pStatus         - the pointer of alarm status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus)
{
    return RT_MAPPER->gpon_alarmStatus_get(alarm, pStatus);
}

/* Function Name:
 *      rtk_gpon_globalCounter_get
 * Description:
 *      GPON MAC get global performance counter.
 * Input:
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara)
{
    return RT_MAPPER->gpon_globalCounter_get(type, pPara);
}

/* Function Name:
 *      rtk_gpon_tcontCounter_get
 * Description:
 *      GPON MAC get Tcont performance counter.
 * Input:
 *      tcontId         - the TCont id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara)
{
    return RT_MAPPER->gpon_tcontCounter_get(tcontId, type, pPara);
}

/* Function Name:
 *      rtk_gpon_flowCounter_get
 * Description:
 *      GPON MAC get Flow performance counter.
 * Input:
 *      flowId          - the flow id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara)
{
    return RT_MAPPER->gpon_flowCounter_get(flowId, type, pPara);
}

/* Function Name:
 *      rtk_gpon_version_get
 * Description:
 *      GPON MAC get the version infomation for debug.
 * Input:
 *      pHver           - the pointer of Hardware versiotn
 *      pSver           - the pointer of Software versiotn
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_version_get(rtk_gpon_device_ver_t *pHver, rtk_gpon_driver_ver_t *pSver)
{
    return RT_MAPPER->gpon_version_get(pHver, pSver);
}

/* Function Name:
 *      rtk_gpon_register_write
 * Description:
 *      GPON MAC write a register.
 * Input:
 *      pWrite          - the pointer of write register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_register_write(rtk_gpon_register_t *pWrite)
{
    return RT_MAPPER->gpon_register_write(pWrite);
}

/* Function Name:
 *      rtk_gpon_register_read
 * Description:
 *      GPON MAC read a register.
 * Input:
 *      pWrite          - the pointer of read register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_register_read(rtk_gpon_register_t *pRead)
{
    return RT_MAPPER->gpon_register_read(pRead);
}

/* Function Name:
 *      rtk_gpon_txForceLaser_set
 * Description:
 *      GPON MAC set the Laser status.
 * Input:
 *      status          - specify to force turn on/off laser
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceLaser_set(rtk_gpon_laser_status_t status)
{
    return RT_MAPPER->gpon_txForceLaser_set(status);
}

/* Function Name:
 *      rtk_gpon_txForceLaser_get
 * Description:
 *      GPON MAC get the Laser status.
 * Input:
 *      pStatus         - pointer of force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus)
{
    return RT_MAPPER->gpon_txForceLaser_get(pStatus);
}

/* Function Name:
 *      rtk_gpon_txForceIdle_set
 * Description:
 *      GPON MAC set to force insert the idle in upstream.
 * Input:
 *      pStatus         - specify to force send Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceIdle_set(int32 on)
{
    return RT_MAPPER->gpon_txForceIdle_set(on);
}

/* Function Name:
 *      rtk_gpon_txForceIdle_get
 * Description:
 *      GPON MAC get the status to force insert the idle in upstream.
 * Input:
 *      pStatus         - pointer of force Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceIdle_get(int32 *pOn)
{
    return RT_MAPPER->gpon_txForceIdle_get(pOn);
}

/* Function Name:
 *      rtk_gpon_txForcePRBS_get
 * Description:
 *      GPON MAC set to force insert the PRBS in upstream.
 * Input:
 *      pStatus         - specify to force send PRBS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForcePRBS_set(int32 on)
{
    return RT_MAPPER->gpon_txForcePRBS_set(on);
}

/* Function Name:
 *      rtk_gpon_txForcePRBS_get
 * Description:
 *      GPON MAC get the status to force insert the PRBS in upstream.
 * Input:
 *      pStatus         - pointer of force PRBS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForcePRBS_get(int32* pOn)
{
    return RT_MAPPER->gpon_txForcePRBS_get(pOn);
}

/* Function Name:
 *      rtk_gpon_txForcePRBS_get
 * Description:
 *      GPON MAC get the status to FEC in downstream from Ident field.
 * Input:
 *      pStatus         - pointer of D/S FEC status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFecSts_get(int32* pEn)
{
    return RT_MAPPER->gpon_dsFecSts_get(pEn);
}

/* Function Name:
 *      rtk_gpon_version_show
 * Description:
 *      GPON MAC show version infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_version_show(void)
{
    return RT_MAPPER->gpon_version_show();
}

/* Function Name:
 *      rtk_gpon_devInfo_show
 * Description:
 *      GPON MAC show the whole driver infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_devInfo_show(void)
{
    return RT_MAPPER->gpon_devInfo_show();
}

/* Function Name:
 *      rtk_gpon_gtc_show
 * Description:
 *      GPON MAC show the whole GTC infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_gtc_show(void)
{
    return RT_MAPPER->gpon_gtc_show();
}

/* Function Name:
 *      rtk_gpon_tcont_show
 * Description:
 *      GPON MAC show the TCont infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_tcont_show(uint32 tcont)
{
    return RT_MAPPER->gpon_tcont_show(tcont);
}

/* Function Name:
 *      rtk_gpon_dsFlow_show
 * Description:
 *      GPON MAC show the D/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_dsFlow_show(uint32 flow)
{
    return RT_MAPPER->gpon_dsFlow_show(flow);
}

/* Function Name:
 *      rtk_gpon_usFlow_show
 * Description:
 *      GPON MAC show the U/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_usFlow_show(uint32 flow)
{
    return RT_MAPPER->gpon_usFlow_show(flow);
}

/* Function Name:
 *      rtk_gpon_macTable_show
 * Description:
 *      GPON MAC show Ethernet Mac Table in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_macTable_show(void)
{
    return RT_MAPPER->gpon_macTable_show();
}

/* Function Name:
 *      rtk_gpon_globalCounter_show
 * Description:
 *      GPON MAC show Global Counter in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type)
{
    return RT_MAPPER->gpon_globalCounter_show(type);
}

/* Function Name:
 *      rtk_gpon_tcontCounter_show
 * Description:
 *      GPON MAC show TCont Counter in COM port.
 *      idx         - TCont index
 *      type        - counter type
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type)
{
    return RT_MAPPER->gpon_tcontCounter_show(idx, type);
}

/*
 * GPON MAC show Flow Counter in COM port.
 */
/* Function Name:
 *      rtk_gpon_flowCounter_show
 * Description:
 *      GPON MAC show Flow Counter in COM port.
 *      idx         - Flow index
 *      type        - counter type
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtk_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type)
{
    return RT_MAPPER->gpon_flowCounter_show(idx, type);
}
#endif /* #if 0 move to k_gpon.c, scott */
