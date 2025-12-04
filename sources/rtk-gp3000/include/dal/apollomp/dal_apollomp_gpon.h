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
 * Purpose : GMac Driver API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#ifndef __DAL_APOLLOMP_GPON_H__
#define __DAL_APOLLOMP_GPON_H__

/*
 * GMac Drv Initialization. To start the GMac Drv. The first function call for GMac Drv.
 */
int32 dal_apollomp_gpon_driver_initialize(void);

/*
 * GMac Drv De-Initialization. To Stop the GMac Drv. The last function call for GMac Drv.
 * The Device should be de-initialized before this function call.
 */
int32 dal_apollomp_gpon_driver_deInitialize(void);

/*
 * GMac Device Initialization. To start the device of the GMac.
 * This function should be called after the Drv Initialization and before any other operation.
 * It should be called after Device is de-initialized but the Drv is not be de-initialized.
 */
int32 dal_apollomp_gpon_device_initialize(void);

/*
 * GMac Device De-Initialization. To stop the device of the GMac.
 * It should be called before the Drv is de-initialized and the GMac is not activated.
 */
int32 dal_apollomp_gpon_device_deInitialize(void);

/*
 * This function is called to register the callback function of the State Change.
 */
int32 dal_apollomp_gpon_eventHandler_stateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func);

/*
 * This function is called to register the callback function of the D/S FEC Change.
 */
int32 dal_apollomp_gpon_eventHandler_dsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);
/*
 * This function is called to register the callback function of the U/S FEC Change.
 */
int32 dal_apollomp_gpon_eventHandler_usFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);

/*
 * This function is called to register the callback function of the U/S PLOAM urgent queue is empty.
 */
int32 dal_apollomp_gpon_eventHandler_usPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);
/*
 * This function is called to register the callback function of the U/S PLOAM normal queue is empty.
 */
int32 dal_apollomp_gpon_eventHandler_usPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);

/*
 * This function is called to register the event handler of the Rx PLOAM.
 */
int32 dal_apollomp_gpon_eventHandler_ploam_reg(rtk_gpon_eventHandleFunc_ploam_t func);

/*
 * This function is called to register the event handler of the Rx OMCI.
 */
int32 dal_apollomp_gpon_eventHandler_omci_reg(rtk_gpon_eventHandleFunc_omci_t func);

/*
 * This function is called to register the callback function of the AES Key Query.
 */
int32 dal_apollomp_gpon_callback_queryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func);

/*
 * This function is called to register the alarm event handler of the alarm.
 */
int32 dal_apollomp_gpon_eventHandler_alarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func);

/*
 * GPON MAC Set Serial Number.
 * It should be called before the GMac is activated.
 */
int32 dal_apollomp_gpon_serialNumber_set(rtk_gpon_serialNumber_t *sn);

/*
 * GPON MAC Get Serial Number.
 */
int32 dal_apollomp_gpon_serialNumber_get(rtk_gpon_serialNumber_t *sn);

/*
 * GPON MAC set Password.
 * It should be called before the GMac is activated.
 */
int32 dal_apollomp_gpon_password_set(rtk_gpon_password_t *pwd);

/*
 * GPON MAC get Password.
 */
int32 dal_apollomp_gpon_password_get(rtk_gpon_password_t *pwd);

/*
 * GPON MAC set parameters.
 * It should be called before the GMac is activated.
 */
int32 dal_apollomp_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara);
/*
 * GPON MAC get parameters, which is set by dal_apollomp_gpon_parameter_set.
 */
int32 dal_apollomp_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara);

/*
 * GPON MAC Activating.
 * The GPON MAC is working now.
 */
int32 dal_apollomp_gpon_activate(rtk_gpon_initialState_t initState);
/*
 * GPON MAC de-Activate.
 * The GPON MAC is out of work now.
 */
int32 dal_apollomp_gpon_deActivate(void);

/*
 * GPON MAC Get PON Status.
 * If the device is not activated, an unknown status is returned.
 */
int32 dal_apollomp_gpon_ponStatus_get(rtk_gpon_fsm_status_t* status);

/*
 * GPON MAC ISR entry
 * Should be called in interrupt process or a polling thread
 */
void dal_apollomp_gpon_isr_entry(void);

/*
 * GPON MAC Create a TCont by assigning an alloc id.
 * A TCont ID is returned.
 */
int32 dal_apollomp_gpon_tcont_create(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);
/*
 * GPON MAC Remove a TCont.
 * The TCont ID and the alloc ID is necessary.
 */
int32 dal_apollomp_gpon_tcont_destroy(rtk_gpon_tcont_ind_t* ind);
/*
 * GPON MAC Get a TCont with an alloc id.
 * The TCont ID is returned.
 */
int32 dal_apollomp_gpon_tcont_get(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);

/*
 * GPON MAC set a D/S flow.
 */
int32 dal_apollomp_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr);

/*
 * GPON MAC get a D/S flow.
 */
int32 dal_apollomp_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr);

/*
 * GPON MAC set a U/S flow.
 */
int32 dal_apollomp_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t* attr);

/*
 * GPON MAC get a U/S flow.
 */
int32 dal_apollomp_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t* attr);

/*
 * GPON MAC Send a PLOAM in upstream.
 * A error is returned if the PLOAM is not sent.
 */
int32 dal_apollomp_gpon_ploam_send(int32 urgent, rtk_gpon_ploam_t* ploam);

/*
 * GPON MAC set the broadcast pass mode.
 */
int32 dal_apollomp_gpon_broadcastPass_set(int32 mode);
/*
 * GPON MAC get the broadcast pass mode.
 */
int32 dal_apollomp_gpon_broadcastPass_get(int32* mode);

/*
 * GPON MAC set the non-multicast pass mode.
 */
int32 dal_apollomp_gpon_nonMcastPass_set(int32 mode);
/*
 * GPON MAC get the non-multicast pass mode.
 */
int32 dal_apollomp_gpon_nonMcastPass_get(int32* mode);

#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
/*
 * GPON MAC set the well known address forwarding mode and the well known address.
 */
int32 dal_apollomp_gpon_wellKnownAddr_set(int32 mode, uint32 addr);
/*
 * GPON MAC get the well known address forwarding mode and the well known address.
 */
int32 dal_apollomp_gpon_wellKnownAddr_get(int32* mode, uint32* addr);
#endif

/*
 * GPON MAC set the multicast address check mode and the address pattern.
 */
int32 dal_apollomp_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern);
/*
 * GPON MAC get the multicast address check mode and the address pattern.
 */
int32 dal_apollomp_gpon_multicastAddrCheck_get(uint32* ipv4_pattern, uint32* ipv6_pattern);

/*
 * GPON MAC set the mac filter mode.
 */
int32 dal_apollomp_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode);

/*
 * GPON MAC get the mac filter mode.
 */
int32 dal_apollomp_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t* mode);

/*
 * GPON MAC set the multicast force mode.
 */
int32 dal_apollomp_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);

/*
 * GPON MAC get the multicast force mode.
 */
int32 dal_apollomp_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6);

/*
 * GPON MAC Add a MAC entry by the MAC Address.
 */
int32 dal_apollomp_gpon_macEntry_add(rtk_gpon_macTable_entry_t* entry);

/*
 * GPON MAC Remove a MAC entry by the MAC Address.
 */
int32 dal_apollomp_gpon_macEntry_del(rtk_gpon_macTable_entry_t* entry);

/*
 * GPON MAC Get a MAC entry by the table index.
 */
int32 dal_apollomp_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t* entry);

/*
 * GPON MAC set the RDI indicator in upstream.
 */
int32 dal_apollomp_gpon_rdi_set(int32 enable);

/*
 * GPON MAC get the RDI indicator in upstream.
 */
int32 dal_apollomp_gpon_rdi_get(int32* enable);

/*
 * GPON MAC set ONU power level.
 */
int32 dal_apollomp_gpon_powerLevel_set(uint8 level);

/*
 * GPON MAC get ONU power level.
 */
int32 dal_apollomp_gpon_powerLevel_get(uint8* level);

/*
 * GPON MAC get the alarm status.
 */
int32 dal_apollomp_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *status);

/*
 * GPON MAC get global performance counter.
 */
int32 dal_apollomp_gpon_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t* para);

/*
 * GPON MAC get Tcont performance counter.
 */
int32 dal_apollomp_gpon_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara);

/*
 * GPON MAC get Flow performance counter.
 */
int32 dal_apollomp_gpon_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara);

/*
 * GPON MAC get the version infomation for debug.
 */
int32 dal_apollomp_gpon_version_get(rtk_gpon_device_ver_t* hver, rtk_gpon_driver_ver_t* sver);

#if 0 /* gemloop is removed in GPON_MAC_SWIO_v1.1 */
/*
 * GPON MAC set the GEM Loop.
 */
int32 dal_apollomp_gpon_gemLoop_set(int32 loop);

/*
 * GPON MAC get the GEM Loop status.
 */
int32 dal_apollomp_gpon_gemLoop_get(int32 *pLoop);
#endif

/*
 * GPON MAC set the Laser status.
 */
int32 dal_apollomp_gpon_txForceLaser_set(rtk_gpon_laser_status_t status);

/*
 * GPON MAC get the Laser status.
 */
int32 dal_apollomp_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus);

/*
 * GPON MAC set to force insert the idle in upstream.
 */
int32 dal_apollomp_gpon_txForceIdle_set(int32 on);

/*
 * GPON MAC get the status to force insert the idle in upstream.
 */
int32 dal_apollomp_gpon_txForceIdle_get(int32 *pOn);

#if 0
/*
 * GPON MAC set to force insert the PRBS in upstream.
 */
int32 dal_apollomp_gpon_txForcePRBS_set(int32 on);

/*
 * GPON MAC get the status to force insert the PRBS in upstream.
 */
int32 dal_apollomp_gpon_txForcePRBS_get(int32* pOn);
#endif

/*
 * GPON MAC get the status to FEC in downstream from Ident field
 */
int32 dal_apollomp_gpon_dsFecSts_get(int32* en);

/*
 * GPON MAC show version infomation in COM port.
 */
void dal_apollomp_gpon_version_show(void);

/*
 * GPON MAC show the whole driver infomation in COM port.
 */
void dal_apollomp_gpon_devInfo_show(void);

/*
 * GPON MAC show the whole GTC infomation in COM port.
 */
void dal_apollomp_gpon_gtc_show(void);

/*
 * GPON MAC show the TCont infomation in COM port.
 */
void dal_apollomp_gpon_tcont_show(uint32 tcont); /* 0xFFFF means show all tcont */

/*
 * GPON MAC show the D/S flow infomation in COM port.
 */
void dal_apollomp_gpon_dsFlow_show(uint32 flow);/* 0xFFFF means show all ds flow */

/*
 * GPON MAC show the U/S flow infomation in COM port.
 */
void dal_apollomp_gpon_usFlow_show(uint32 flow);/* 0xFFFF means show all us flow */

/*
 * GPON MAC show Ethernet Mac Table in COM port.
 */
void dal_apollomp_gpon_macTable_show(void);

/*
 * GPON MAC show Global Counter in COM port.
 */
void dal_apollomp_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type);

/*
 * GPON MAC show TCont Counter in COM port.
 */
void dal_apollomp_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type);

/*
 * GPON MAC show Flow Counter in COM port.
 */
void dal_apollomp_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type);

int32 dal_apollomp_gpon_omci_tx(rtk_gpon_omci_msg_t* omci);
int32 dal_apollomp_gpon_omci_rx(rtk_gpon_omci_msg_t* omci);

int32 dal_apollomp_gpon_auto_tcont_set(int32 state);
int32 dal_apollomp_gpon_auto_tcont_get(int32 *pState);
int32 dal_apollomp_gpon_auto_boh_set(int32 state);
int32 dal_apollomp_gpon_auto_boh_get(int32 *pState);
int32 dal_apollomp_gpon_eqd_offset_set(int32 offset);
int32 dal_apollomp_gpon_eqd_offset_get(int32 *pOffset);
int32 dal_apollomp_gpon_autoDisTx_set(rtk_enable_t state);

#endif  /* __DAL_APOLLOMP_GPON_H__ */

