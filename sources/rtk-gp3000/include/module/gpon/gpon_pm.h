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
 * Purpose : GPON MAC Driver PM implement
 *
 * Feature : Provide the PM conunters
 *
 */

#ifndef __GPON_PM_H__
#define __GPON_PM_H__

int32 gpon_pm_onuActivation_get (gpon_dev_obj_t* obj, rtk_gpon_onu_activation_counter_t* counter);
int32 gpon_pm_dsPhy_get (gpon_dev_obj_t* obj, rtk_gpon_ds_physical_counter_t* counter);
int32 gpon_pm_dsPloam_get (gpon_dev_obj_t* obj, rtk_gpon_ds_ploam_counter_t* counter);
int32 gpon_pm_dsBwMap_get (gpon_dev_obj_t* obj, rtk_gpon_ds_bwMap_counter_t* counter);
int32 gpon_pm_dsGem_get (gpon_dev_obj_t* obj, rtk_gpon_ds_gem_counter_t* counter);
int32 gpon_pm_dsEth_get (gpon_dev_obj_t* obj, rtk_gpon_ds_eth_counter_t* counter);
int32 gpon_pm_dsOmci_get (gpon_dev_obj_t* obj, rtk_gpon_ds_omci_counter_t* counter);
int32 gpon_pm_usPhy_get (gpon_dev_obj_t* obj, rtk_gpon_us_physical_counter_t* counter);
int32 gpon_pm_usDbr_get (gpon_dev_obj_t* obj, rtk_gpon_us_dbr_counter_t* counter);
int32 gpon_pm_usPloam_get (gpon_dev_obj_t* obj, rtk_gpon_us_ploam_counter_t* counter);
int32 gpon_pm_usGem_get (gpon_dev_obj_t* obj, rtk_gpon_us_gem_counter_t* counter);
int32 gpon_pm_usEth_get (gpon_dev_obj_t* obj, rtk_gpon_us_eth_counter_t* counter);
int32 gpon_pm_usOmci_get (gpon_dev_obj_t* obj, rtk_gpon_us_omci_counter_t* counter);

int32 gpon_pm_tcontGem_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_gem_counter_t* counter);
int32 gpon_pm_tcontEth_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_eth_counter_t* counter);
int32 gpon_pm_tcontIdle_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_idle_counter_t* counter);
int32 gpon_pm_dsFlowGem_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_dsFlow_gem_counter_t* counter);
int32 gpon_pm_dsFlowEth_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_dsFlow_eth_counter_t* counter);
int32 gpon_pm_usFlowGem_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_usFlow_gem_counter_t* counter);
int32 gpon_pm_usFlowEth_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_usFlow_eth_counter_t* counter );

#endif  /* __GPON_PM_H__ */

