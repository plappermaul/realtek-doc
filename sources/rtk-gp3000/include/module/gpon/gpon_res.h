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
 * Purpose : GPON MAC Driver Resource Management
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#ifndef __GMAC_RES_H__
#define __GMAC_RES_H__

int32 gpon_dev_versoin_get(gpon_dev_obj_t* obj, rtk_gpon_device_ver_t* ver);

int32 gpon_dev_initialize(gpon_dev_obj_t* obj, uint32 baseaddr);

int32 gpon_dev_deInitialize(gpon_dev_obj_t* obj);

int32 gpon_dev_sn_set(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn);

int32 gpon_dev_sn_get(gpon_dev_obj_t* obj, rtk_gpon_serialNumber_t *sn);

int32 gpon_dev_pwd_set(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd);

int32 gpon_dev_pwd_get(gpon_dev_obj_t* obj, rtk_gpon_password_t *pwd);

int32 gpon_dev_para_onu_set(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para);

int32 gpon_dev_para_onu_get(gpon_dev_obj_t* obj, rtk_gpon_onu_activation_para_t *para);

int32 gpon_dev_para_laser_set(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para);

int32 gpon_dev_para_laser_get(gpon_dev_obj_t* obj, rtk_gpon_laser_para_t *para);

int32 gpon_dev_para_dsPhy_set(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para);

int32 gpon_dev_para_dsPhy_get(gpon_dev_obj_t* obj, rtk_gpon_ds_physical_para_t *para);

int32 gpon_dev_para_dsPloam_set(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para);

int32 gpon_dev_para_dsPloam_get(gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_para_t *para);

int32 gpon_dev_para_dsBwmap_set(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para);

int32 gpon_dev_para_dsBwmap_get(gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_para_t *para);

int32 gpon_dev_para_dsGem_set(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para);

int32 gpon_dev_para_dsGem_get(gpon_dev_obj_t* obj, rtk_gpon_ds_gem_para_t *para);

int32 gpon_dev_para_dsEth_set(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para);

int32 gpon_dev_para_dsEth_get(gpon_dev_obj_t* obj, rtk_gpon_ds_eth_para_t *para);

int32 gpon_dev_para_dsOmci_set(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para);

int32 gpon_dev_para_dsOmci_get(gpon_dev_obj_t* obj, rtk_gpon_ds_omci_para_t *para);

#if 0 /* TDM PTI is removed in GPON_MAC_SWIO_r1.1 */
int32 gpon_dev_para_dsTdm_set(gpon_dev_obj_t* obj, rtk_gpon_ds_tdm_para_t *para);

int32 gpon_dev_para_dsTdm_get(gpon_dev_obj_t* obj, rtk_gpon_ds_tdm_para_t *para);
#endif

int32 gpon_dev_para_usPhy_set(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para);

int32 gpon_dev_para_usPhy_get(gpon_dev_obj_t* obj, rtk_gpon_us_physical_para_t *para);

int32 gpon_dev_para_usPloam_set(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para);

int32 gpon_dev_para_usPloam_get(gpon_dev_obj_t* obj, rtk_gpon_us_ploam_para_t *para);

int32 gpon_dev_para_usDbr_set(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para);

int32 gpon_dev_para_usDbr_get(gpon_dev_obj_t* obj, rtk_gpon_us_dbr_para_t *para);

#if 0 /* page US_FRAG is removed */
int32 gpon_dev_para_usGem_set(gpon_dev_obj_t* obj, rtk_gpon_us_gem_para_t *para);
int32 gpon_dev_para_usGem_get(gpon_dev_obj_t* obj, rtk_gpon_us_gem_para_t *para);
int32 gpon_dev_para_usEth_set(gpon_dev_obj_t* obj, rtk_gpon_us_eth_para_t *para);
int32 gpon_dev_para_usEth_get(gpon_dev_obj_t* obj, rtk_gpon_us_eth_para_t *para);
#endif
#if 0 /* GEM US omci register is removed */
int32 gpon_dev_para_usOmci_set(gpon_dev_obj_t* obj, rtk_gpon_us_omci_para_t *para);
int32 gpon_dev_para_usOmci_get(gpon_dev_obj_t* obj, rtk_gpon_us_omci_para_t *para);
#endif

#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
int32 gpon_dev_para_optPwrSav_set(gpon_dev_obj_t* obj, rtk_gpon_us_opt_pwr_sav_t *para);

int32 gpon_dev_para_optPwrSav_get(gpon_dev_obj_t* obj, rtk_gpon_us_opt_pwr_sav_t *para);
#endif

int32 gpon_dev_activate(gpon_dev_obj_t* obj, rtk_gpon_initialState_t initState);

int32 gpon_dev_deactivate(gpon_dev_obj_t* obj);

int32 gpon_dev_get_status(gpon_dev_obj_t* obj, rtk_gpon_fsm_status_t* status);

int32 gpon_dev_get_fec_status(gpon_dev_obj_t* obj, int32        *ds, int32        *us);

int32 gpon_dev_dsFec_report(gpon_dev_obj_t* obj, int32        status);

int32 gpon_dev_usFec_report(gpon_dev_obj_t* obj, int32        status);

int32 gpon_dev_upstreamOverhead_calculate(gpon_dev_obj_t* obj);

int32 gpon_dev_burstHead_preRanged_set(gpon_dev_obj_t* obj);

int32 gpon_dev_burstHead_ranged_set(gpon_dev_obj_t* obj);

int32 gpon_dev_onuid_set(gpon_dev_obj_t* obj, uint8 onuid);

int32 gpon_dev_tcont_logical_add(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);

int32 gpon_dev_tcont_logical_del(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind);

int32 gpon_dev_tcont_logical_get(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);

int32 gpon_dev_tcont_physical_add(gpon_dev_obj_t* obj, uint32 alloc,rtk_gpon_tcont_attr_t* attr);
#if 0 /* the DBA alloc-id is removed in G.984.3(2008) */
int32 gpon_dev_dba_tcont_add(gpon_dev_obj_t* obj, uint32 alloc);
#endif
int32 gpon_dev_tcont_physical_del(gpon_dev_obj_t* obj, uint32 alloc);

int32 gpon_dev_tcont_physical_get(gpon_dev_obj_t* obj, rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);

int32 gpon_dev_tcont_physical_reset(gpon_dev_obj_t* obj);

int32 gpon_dev_dsFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr);

int32 gpon_dev_dsFlow_del(gpon_dev_obj_t* obj, uint32 flow_id);

int32 gpon_dev_dsFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_dsFlow_attr_t* attr);

int32 gpon_dev_usFlow_add(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr);

int32 gpon_dev_usFlow_del(gpon_dev_obj_t* obj, uint32 flow_id);

int32 gpon_dev_usFlow_get(gpon_dev_obj_t* obj, uint32 flow_id, rtk_gpon_usFlow_attr_t* attr);

int32 gpon_dev_bcastPass_set(gpon_dev_obj_t* obj, int32 status);

int32 gpon_dev_bcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus);

int32 gpon_dev_nonMcastPass_set(gpon_dev_obj_t* obj, int32 status);

int32 gpon_dev_nonMcastPass_get(gpon_dev_obj_t* obj, int32 *pStatus);

int32 gpon_dev_mcastCheck_set(gpon_dev_obj_t* obj, uint32 ipv4_addr, uint32 ipv6_addr);

int32 gpon_dev_mcastCheck_get(gpon_dev_obj_t* obj, uint32 *ipv4_addr, uint32 *ipv6_addr);

int32 gpon_dev_wellKnownFwdAddr_set(gpon_dev_obj_t* obj, int32 status, uint32 addr);

int32 gpon_dev_wellKnownFwdAddr_get(gpon_dev_obj_t* obj, int32 *pStatus, uint32* addr);

int32 gpon_dev_macFilterMode_set(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t mode);

int32 gpon_dev_macFilterMode_get(gpon_dev_obj_t* obj, rtk_gpon_macTable_exclude_mode_t *pMode);

int32 gpon_dev_mcForceMode_set(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);

int32 gpon_dev_mcForceMode_get(gpon_dev_obj_t* obj, rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6);

int32 gpon_dev_macTableEntry_add(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry);

int32 gpon_dev_macTableEntry_del(gpon_dev_obj_t* obj, rtk_gpon_macTable_entry_t* entry);

int32 gpon_dev_macTableEntry_get(gpon_dev_obj_t* obj, uint32 index, rtk_gpon_macTable_entry_t* entry);

#if 0 /* page GEM_FCB is removed */
int32 gpon_dev_chunk_set(gpon_dev_obj_t* obj, rtk_gpon_chunkHeader_t* chunk);

int32 gpon_dev_chunk_get(gpon_dev_obj_t* obj, rtk_gpon_chunkHeader_t* chunk);

int32 gpon_dev_shaper_set(gpon_dev_obj_t* obj, rtk_gpon_shaper_t* shaper);

int32 gpon_dev_shaper_get(gpon_dev_obj_t* obj, rtk_gpon_shaper_t* shaper);

int32 gpon_dev_reqEn_set(gpon_dev_obj_t* obj, int32 enable);

int32 gpon_dev_reqEn_get(gpon_dev_obj_t* obj);
#endif /* page GEM_FCB is removed */

int32 gpon_dev_rdi_set(gpon_dev_obj_t* obj, int32 enable);

int32 gpon_dev_rdi_get(gpon_dev_obj_t* obj, int32 *pEnable);

#if 0 /* The GEM US INTF register is removed */
int32 gpon_dev_tdmEn_set(gpon_dev_obj_t* obj, int32 enable);

int32 gpon_dev_tdmEn_get(gpon_dev_obj_t* obj);
#endif

int32 gpon_dev_powerLevel_set(gpon_dev_obj_t* obj, uint8 level);

int32 gpon_dev_powerLevel_get(gpon_dev_obj_t* obj, uint8 *pLevel);

int32 gpon_dev_berInterval_set(gpon_dev_obj_t* obj, uint32 interval);
int32 gpon_dev_berInterval_get(gpon_dev_obj_t* obj, uint32 *interval);

#if 0 /* page DBRu is removed */
int32 gpon_dev_dbruPlou_send(gpon_dev_obj_t* obj, uint8 plou);

int32 gpon_dev_dbruPiggyback_send(gpon_dev_obj_t* obj, uint32 idx, rtk_gpon_piggyback_dbr_data_t* value);

int32 gpon_dev_dbruWholeDba_send(gpon_dev_obj_t* obj, uint32 len, uint8* value);
#endif /* page DBRu is removed */

extern int32 gpon_dev_auto_tcont_set(gpon_dev_obj_t *obj, int32 state);
extern int32 gpon_dev_auto_tcont_get(gpon_dev_obj_t *obj, int32 *pState);
extern int32 gpon_dev_auto_boh_set(gpon_dev_obj_t *obj, int32 state);
extern int32 gpon_dev_auto_boh_get(gpon_dev_obj_t *obj, int32 *pState);
extern int32 gpon_dev_eqd_offset_set(gpon_dev_obj_t *obj, int32 offset);
extern int32 gpon_dev_eqd_offset_get(gpon_dev_obj_t *obj, int32 *pOffset);


extern int32 gpon_dev_para_onuId_get(gpon_dev_obj_t* obj, uint8 *para);
extern int32 gpon_dev_para_drvStatus_get(gpon_drv_obj_t* obj, gpon_drv_status_t *para);

extern int32 gpon_dev_rogueOntDisTx_set(gpon_dev_obj_t* obj, rtk_enable_t txState);

extern int32 gpon_dev_signal_para_set(gpon_dev_obj_t* obj, rtk_gpon_sig_para_t *pPara);

extern int32 gpon_dev_RDIMODE_set(gpon_dev_obj_t* obj, uint32 rdiMode);

extern int32 gpon_dev_portMacForceMode_set(gpon_dev_obj_t* obj, rtk_port_linkStatus_t linkStatus);

extern int32 gpon_dev_pps_fire(gpon_dev_obj_t* obj);

#endif  /* __GMAC_RES_H__ */

