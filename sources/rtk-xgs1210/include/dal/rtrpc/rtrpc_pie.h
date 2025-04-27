/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_PIE_H__
#define __RTRPC_PIE_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_pie_meterIncludeIfg_get                      rtrpc_pie_meterIncludeIfg_get
#define rtk_pie_meterIncludeIfg_set                      rtrpc_pie_meterIncludeIfg_set
#define rtk_pie_meterExceed_get                          rtrpc_pie_meterExceed_get
#define rtk_pie_meterExceedAggregation_get               rtrpc_pie_meterExceedAggregation_get
#define rtk_pie_meterEntry_get                           rtrpc_pie_meterEntry_get
#define rtk_pie_meterEntry_set                           rtrpc_pie_meterEntry_set
#define rtk_pie_blockLookupEnable_get                    rtrpc_pie_blockLookupEnable_get
#define rtk_pie_blockLookupEnable_set                    rtrpc_pie_blockLookupEnable_set
#define rtk_pie_blockGrouping_get                        rtrpc_pie_blockGrouping_get
#define rtk_pie_blockGrouping_set                        rtrpc_pie_blockGrouping_set
#define rtk_pie_template_get                             rtrpc_pie_template_get
#define rtk_pie_template_set                             rtrpc_pie_template_set
#define rtk_pie_templateField_check                      rtrpc_pie_templateField_check
#define rtk_pie_rangeCheckIp_get                         rtrpc_pie_rangeCheckIp_get
#define rtk_pie_rangeCheckIp_set                         rtrpc_pie_rangeCheckIp_set
#define rtk_pie_rangeCheck_get                           rtrpc_pie_rangeCheck_get
#define rtk_pie_rangeCheck_set                           rtrpc_pie_rangeCheck_set
#define rtk_pie_fieldSelector_get                        rtrpc_pie_fieldSelector_get
#define rtk_pie_fieldSelector_set                        rtrpc_pie_fieldSelector_set
#define rtk_pie_phase_get                                rtrpc_pie_phase_get
#define rtk_pie_phase_set                                rtrpc_pie_phase_set
#define rtk_pie_templateVlanSel_get                      rtrpc_pie_templateVlanSel_get
#define rtk_pie_templateVlanSel_set                      rtrpc_pie_templateVlanSel_set
#define rtk_pie_meterDpSel_get                           rtrpc_pie_meterDpSel_get
#define rtk_pie_meterDpSel_set                           rtrpc_pie_meterDpSel_set
#define rtk_pie_arpMacSel_get                            rtrpc_pie_arpMacSel_get
#define rtk_pie_arpMacSel_set                            rtrpc_pie_arpMacSel_set
#define rtk_pie_intfSel_get                              rtrpc_pie_intfSel_get
#define rtk_pie_intfSel_set                              rtrpc_pie_intfSel_set
#define rtk_pie_templateVlanFmtSel_get                   rtrpc_pie_templateVlanFmtSel_get
#define rtk_pie_templateVlanFmtSel_set                   rtrpc_pie_templateVlanFmtSel_set
#define rtk_pie_meterTrtcmType_get                       rtrpc_pie_meterTrtcmType_get
#define rtk_pie_meterTrtcmType_set                       rtrpc_pie_meterTrtcmType_set
#define rtk_pie_filter1BR_get                            rtrpc_pie_filter1BR_get
#define rtk_pie_filter1BR_set                            rtrpc_pie_filter1BR_set
/*
 * Function Declaration
 */

/* Module Name     : PIE */

extern int32
rtrpc_pie_init(uint32 unit);

extern int32
rtrpc_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase);

extern int32
rtrpc_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase);

extern int32
rtrpc_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

extern int32
rtrpc_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

extern int32
rtrpc_pie_blockGrouping_get(uint32 unit, uint32 block_idx, uint32 *group_id,
    uint32 *logic_id);

extern int32
rtrpc_pie_blockGrouping_set(uint32 unit, uint32 block_idx, uint32 group_id,
    uint32 logic_id);

extern int32
rtrpc_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate);

extern int32
rtrpc_pie_template_set(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate);

extern int32
rtrpc_pie_templateVlanSel_get(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   *pVlanSel);

extern int32
rtrpc_pie_templateVlanSel_set(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   vlanSel);

extern int32
rtrpc_pie_templateField_check(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    rtk_pie_templateFieldType_t type);

/* Sub-module Name :
 *  Range Check
 */

extern int32
rtrpc_pie_rangeCheckIp_get(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

extern int32
rtrpc_pie_rangeCheckIp_set(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

extern int32
rtrpc_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

extern int32
rtrpc_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Sub-module Name :
 *  Field Selector
 */

extern int32
rtrpc_pie_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

extern int32
rtrpc_pie_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

extern int32
rtrpc_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

extern int32
rtrpc_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

extern int32
rtrpc_pie_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask);

extern int32
rtrpc_pie_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed);

extern int32
rtrpc_pie_meterEntry_get(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry);

extern int32
rtrpc_pie_meterEntry_set(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry);

extern int32
rtrpc_pie_meterDpSel_get(
    uint32                  unit,
    rtk_pie_meterDpSel_t    *pDpSel);

extern int32
rtrpc_pie_meterDpSel_set(
    uint32                  unit,
    rtk_pie_meterDpSel_t    dpSel);

extern int32
rtrpc_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel);

extern int32
rtrpc_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel);

extern int32
rtrpc_pie_intfSel_get(uint32 unit, rtk_pie_intfSel_t *intfSel);

extern int32
rtrpc_pie_intfSel_set(uint32 unit, rtk_pie_intfSel_t intfSel);

extern int32
rtrpc_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel);

extern int32
rtrpc_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel);

extern int32
rtrpc_pie_meterTrtcmType_get(uint32 unit, rtk_pie_meterTrtcmType_t *type);

extern int32
rtrpc_pie_meterTrtcmType_set(uint32 unit, rtk_pie_meterTrtcmType_t type);

extern int32
rtrpc_pie_filter1BR_get(uint32 unit, rtk_enable_t *en);

extern int32
rtrpc_pie_filter1BR_set(uint32 unit, rtk_enable_t en);

extern int32
rtrpc_pieMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_PIE_H__ */

