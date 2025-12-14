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
 * Purpose : GPON MAC Driver Debug Functions
 *
 * Feature : GPON MAC Driver Debug Functions
 *
 */

#ifndef __GPON_DEBUG_H__
#define __GPON_DEBUG_H__
int32 gpon_dbg_para_laser_get(rtk_gpon_laser_para_t *pPara);
int32 gpon_dbg_para_dsPhy_get(rtk_gpon_ds_physical_para_t *para);
int32 gpon_dbg_para_dsPloam_get(rtk_gpon_ds_ploam_para_t *para);
int32 gpon_dbg_para_dsBwmap_get(rtk_gpon_ds_bwMap_para_t *para);
int32 gpon_dbg_para_dsGem_get(rtk_gpon_ds_gem_para_t *para);
int32 gpon_dbg_para_dsEth_get(rtk_gpon_ds_eth_para_t *para);
int32 gpon_dbg_para_dsOmci_get(rtk_gpon_ds_omci_para_t *para);
int32 gpon_dbg_para_usPhy_get(rtk_gpon_us_physical_para_t *para);
int32 gpon_dbg_para_usPloam_get(rtk_gpon_us_ploam_para_t *para);
int32 gpon_dbg_para_usDbr_get(rtk_gpon_us_dbr_para_t *para);
#if 0 /* opt powersave is removed in GPON_MAC_SWIO_r1.1 */
int32 gpon_dbg_para_optPwrSav_get(rtk_gpon_us_opt_pwr_sav_t *para);
#endif
int32 gpon_dbg_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *attr);
int32 gpon_dbg_usFlow_get(uint32 flowId, uint32 *gem_port);
int32 gpon_dbg_multicastAddrCheck_get(uint32 *ipv4_pattern, uint32 *ipv6_pattern);
#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
int32 gpon_dbg_wellKnownAddr_get(int32* mode, uint32 *addr);
#endif
int32 gpon_dbg_broadcastPass_get(int32 *mode);
int32 gpon_dbg_nonMcastPass_get(int32 *mode);
int32 gpon_dbg_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *mode);
int32 gpon_dbg_mcForceMode_get(rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6);
int32 gpon_dbg_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry);
int32 gpon_dbg_rdi_get(int32 *pEnable);
#if 0 /* gemloop is removed in GPON_MAC_SWIO_v1.1 */
int32 gpon_dbg_gemLoop_set(gpon_dev_obj_t* obj, int32 enable);

int32 gpon_dbg_gemLoop_get(gpon_dev_obj_t* obj, int32 *pEnable);
#endif
#if 0 /* page GEM FCB is removed */
rtk_gpon_laser_status_t GMac_Dbg_get_gemport_drop_his(gpon_dev_obj_t* obj, rtk_gpon_dropHisStatus_t* history);
#endif

int32 gpon_dbg_forceLaser_set(gpon_dev_obj_t* obj, rtk_gpon_laser_status_t enable);

int32 gpon_dbg_forceLaser_get(gpon_dev_obj_t* obj, rtk_gpon_laser_status_t *pEnable);

int32 gpon_dbg_forceIdle_set(gpon_dev_obj_t* obj, int32 enable);

int32 gpon_dbg_forceIdle_get(gpon_dev_obj_t* obj, int32 *pEnable);

int32 gpon_dbg_forcePRBS_set(gpon_dev_obj_t* obj, rtk_gpon_prbs_t prbsCfg);

int32 gpon_dbg_forcePRBS_get(gpon_dev_obj_t* obj, rtk_gpon_prbs_t *pPrbsCfg);

int32 gpon_dbg_dsFec_get(gpon_dev_obj_t* obj, int32 *pEnable);

const char* gpon_dbg_drv_status_str(gpon_drv_status_t status);

const char* gpon_dbg_fsm_status_str(rtk_gpon_fsm_status_t status);

const char* gpon_dbg_fsm_event_str(rtk_gpon_fsm_event_t event);

const char* gpon_dbg_mac_force_mode_str(rtk_gpon_mc_force_mode_t status);

const char* gpon_dbg_mac_force_laser_str(rtk_gpon_laser_status_t status);

const char* gpon_dbg_mac_filter_mode_str(rtk_gpon_macTable_exclude_mode_t status);

const char* gpon_dbg_alarm_str(rtk_gpon_alarm_type_t status);

const char* gpon_dbg_flow_type_str(rtk_gpon_flowType_t type);

#endif  /* __GPON_DEBUG_H__ */

