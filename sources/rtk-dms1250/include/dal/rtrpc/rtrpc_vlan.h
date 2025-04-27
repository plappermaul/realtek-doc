/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_VLAN_H__
#define __RTRPC_VLAN_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_vlan_port_add                                rtrpc_vlan_port_add
#define rtk_vlan_port_get                                rtrpc_vlan_port_get
#define rtk_vlan_port_set                                rtrpc_vlan_port_set
#define rtk_vlan_port_del                                rtrpc_vlan_port_del
#define rtk_vlan_mcastGroup_get                          rtrpc_vlan_mcastGroup_get
#define rtk_vlan_mcastGroup_set                          rtrpc_vlan_mcastGroup_set
#define rtk_vlan_portPvid_get                            rtrpc_vlan_portPvid_get
#define rtk_vlan_portPvid_set                            rtrpc_vlan_portPvid_set
#define rtk_vlan_portPrivateVlanEnable_get               rtrpc_vlan_portPrivateVlanEnable_get
#define rtk_vlan_portPrivateVlanEnable_set               rtrpc_vlan_portPrivateVlanEnable_set
#define rtk_vlan_portAcceptFrameType_get                 rtrpc_vlan_portAcceptFrameType_get
#define rtk_vlan_portAcceptFrameType_set                 rtrpc_vlan_portAcceptFrameType_set
#define rtk_vlan_portEgrFilterEnable_get                 rtrpc_vlan_portEgrFilterEnable_get
#define rtk_vlan_portEgrFilterEnable_set                 rtrpc_vlan_portEgrFilterEnable_set
#define rtk_vlan_mcastLeakyEnable_get                    rtrpc_vlan_mcastLeakyEnable_get
#define rtk_vlan_mcastLeakyEnable_set                    rtrpc_vlan_mcastLeakyEnable_set
#define rtk_vlan_svlMode_get                             rtrpc_vlan_svlMode_get
#define rtk_vlan_svlMode_set                             rtrpc_vlan_svlMode_set
#define rtk_vlan_svlFid_get                              rtrpc_vlan_svlFid_get
#define rtk_vlan_svlFid_set                              rtrpc_vlan_svlFid_set
#define rtk_vlan_stg_get                                 rtrpc_vlan_stg_get
#define rtk_vlan_stg_set                                 rtrpc_vlan_stg_set
#define rtk_vlan_l2LookupSvlFid_get                      rtrpc_vlan_l2LookupSvlFid_get
#define rtk_vlan_l2LookupSvlFid_set                      rtrpc_vlan_l2LookupSvlFid_set
#define rtk_vlan_create                                  rtrpc_vlan_create
#define rtk_vlan_destroy                                 rtrpc_vlan_destroy
#define rtk_vlan_destroyAll                              rtrpc_vlan_destroyAll
#define rtk_vlan_protoGroup_get                          rtrpc_vlan_protoGroup_get
#define rtk_vlan_protoGroup_set                          rtrpc_vlan_protoGroup_set
#define rtk_vlan_portProtoVlan_get                       rtrpc_vlan_portProtoVlan_get
#define rtk_vlan_portProtoVlan_set                       rtrpc_vlan_portProtoVlan_set
#define rtk_vlan_portIgrTpid_get                         rtrpc_vlan_portIgrTpid_get
#define rtk_vlan_portIgrTpid_set                         rtrpc_vlan_portIgrTpid_set
#define rtk_vlan_portEgrTpid_get                         rtrpc_vlan_portEgrTpid_get
#define rtk_vlan_portEgrTpid_set                         rtrpc_vlan_portEgrTpid_set
#define rtk_vlan_portEgrTpidSrc_get                      rtrpc_vlan_portEgrTpidSrc_get
#define rtk_vlan_portEgrTpidSrc_set                      rtrpc_vlan_portEgrTpidSrc_set
#define rtk_vlan_portIgrExtraTagEnable_get               rtrpc_vlan_portIgrExtraTagEnable_get
#define rtk_vlan_portIgrExtraTagEnable_set               rtrpc_vlan_portIgrExtraTagEnable_set
#define rtk_vlan_portIgrVlanTransparentEnable_get        rtrpc_vlan_portIgrVlanTransparentEnable_get
#define rtk_vlan_portIgrVlanTransparentEnable_set        rtrpc_vlan_portIgrVlanTransparentEnable_set
#define rtk_vlan_portEgrVlanTransparentEnable_get        rtrpc_vlan_portEgrVlanTransparentEnable_get
#define rtk_vlan_portEgrVlanTransparentEnable_set        rtrpc_vlan_portEgrVlanTransparentEnable_set
#define rtk_vlan_l2LookupMode_get                        rtrpc_vlan_l2LookupMode_get
#define rtk_vlan_l2LookupMode_set                        rtrpc_vlan_l2LookupMode_set
#define rtk_vlan_groupMask_get                           rtrpc_vlan_groupMask_get
#define rtk_vlan_groupMask_set                           rtrpc_vlan_groupMask_set
#define rtk_vlan_profileIdx_get                          rtrpc_vlan_profileIdx_get
#define rtk_vlan_profileIdx_set                          rtrpc_vlan_profileIdx_set
#define rtk_vlan_profile_get                             rtrpc_vlan_profile_get
#define rtk_vlan_portFwdVlan_get                         rtrpc_vlan_portFwdVlan_get
#define rtk_vlan_portFwdVlan_set                         rtrpc_vlan_portFwdVlan_set
#define rtk_vlan_profile_set                             rtrpc_vlan_profile_set
#define rtk_vlan_portIgrFilter_get                       rtrpc_vlan_portIgrFilter_get
#define rtk_vlan_portIgrFilter_set                       rtrpc_vlan_portIgrFilter_set
#define rtk_vlan_portPvidMode_get                        rtrpc_vlan_portPvidMode_get
#define rtk_vlan_portPvidMode_set                        rtrpc_vlan_portPvidMode_set
#define rtk_vlan_macBasedVlan_get                        rtrpc_vlan_macBasedVlan_get
#define rtk_vlan_macBasedVlan_set                        rtrpc_vlan_macBasedVlan_set
#define rtk_vlan_macBasedVlanWithMsk_get                 rtrpc_vlan_macBasedVlanWithMsk_get
#define rtk_vlan_macBasedVlanWithMsk_set                 rtrpc_vlan_macBasedVlanWithMsk_set
#define rtk_vlan_macBasedVlanWithPort_get                rtrpc_vlan_macBasedVlanWithPort_get
#define rtk_vlan_macBasedVlanWithPort_set                rtrpc_vlan_macBasedVlanWithPort_set
#define rtk_vlan_portMacBasedVlanEnable_get              rtrpc_vlan_portMacBasedVlanEnable_get
#define rtk_vlan_portMacBasedVlanEnable_set              rtrpc_vlan_portMacBasedVlanEnable_set
#define rtk_vlan_macBasedVlanEntry_get                   rtrpc_vlan_macBasedVlanEntry_get
#define rtk_vlan_macBasedVlanEntry_set                   rtrpc_vlan_macBasedVlanEntry_set
#define rtk_vlan_macBasedVlanEntry_add                   rtrpc_vlan_macBasedVlanEntry_add
#define rtk_vlan_macBasedVlanEntry_del                   rtrpc_vlan_macBasedVlanEntry_del
#define rtk_vlan_ipSubnetBasedVlan_get                   rtrpc_vlan_ipSubnetBasedVlan_get
#define rtk_vlan_ipSubnetBasedVlan_set                   rtrpc_vlan_ipSubnetBasedVlan_set
#define rtk_vlan_ipSubnetBasedVlanWithPort_get           rtrpc_vlan_ipSubnetBasedVlanWithPort_get
#define rtk_vlan_ipSubnetBasedVlanWithPort_set           rtrpc_vlan_ipSubnetBasedVlanWithPort_set
#define rtk_vlan_portIpSubnetBasedVlanEnable_get         rtrpc_vlan_portIpSubnetBasedVlanEnable_get
#define rtk_vlan_portIpSubnetBasedVlanEnable_set         rtrpc_vlan_portIpSubnetBasedVlanEnable_set
#define rtk_vlan_ipSubnetBasedVlanEntry_get              rtrpc_vlan_ipSubnetBasedVlanEntry_get
#define rtk_vlan_ipSubnetBasedVlanEntry_set              rtrpc_vlan_ipSubnetBasedVlanEntry_set
#define rtk_vlan_ipSubnetBasedVlanEntry_add              rtrpc_vlan_ipSubnetBasedVlanEntry_add
#define rtk_vlan_ipSubnetBasedVlanEntry_del              rtrpc_vlan_ipSubnetBasedVlanEntry_del
#define rtk_vlan_tpidEntry_get                           rtrpc_vlan_tpidEntry_get
#define rtk_vlan_tpidEntry_set                           rtrpc_vlan_tpidEntry_set
#define rtk_vlan_portEgrTagSts_get                       rtrpc_vlan_portEgrTagSts_get
#define rtk_vlan_portEgrTagSts_set                       rtrpc_vlan_portEgrTagSts_set
#define rtk_vlan_igrVlanCnvtBlkMode_get                  rtrpc_vlan_igrVlanCnvtBlkMode_get
#define rtk_vlan_igrVlanCnvtBlkMode_set                  rtrpc_vlan_igrVlanCnvtBlkMode_set
#define rtk_vlan_igrVlanCnvtEntry_get                    rtrpc_vlan_igrVlanCnvtEntry_get
#define rtk_vlan_igrVlanCnvtEntry_set                    rtrpc_vlan_igrVlanCnvtEntry_set
#define rtk_vlan_portIgrVlanCnvtEnable_get               rtrpc_vlan_portIgrVlanCnvtEnable_get
#define rtk_vlan_portIgrVlanCnvtEnable_set               rtrpc_vlan_portIgrVlanCnvtEnable_set
#define rtk_vlan_egrVlanCnvtDblTagEnable_get             rtrpc_vlan_egrVlanCnvtDblTagEnable_get
#define rtk_vlan_egrVlanCnvtDblTagEnable_set             rtrpc_vlan_egrVlanCnvtDblTagEnable_set
#define rtk_vlan_egrVlanCnvtVidSource_get                rtrpc_vlan_egrVlanCnvtVidSource_get
#define rtk_vlan_egrVlanCnvtVidSource_set                rtrpc_vlan_egrVlanCnvtVidSource_set
#define rtk_vlan_egrVlanCnvtEntry_get                    rtrpc_vlan_egrVlanCnvtEntry_get
#define rtk_vlan_egrVlanCnvtEntry_set                    rtrpc_vlan_egrVlanCnvtEntry_set
#define rtk_vlan_portEgrVlanCnvtEnable_get               rtrpc_vlan_portEgrVlanCnvtEnable_get
#define rtk_vlan_portEgrVlanCnvtEnable_set               rtrpc_vlan_portEgrVlanCnvtEnable_set
#define rtk_vlan_aggrEnable_get                          rtrpc_vlan_aggrEnable_get
#define rtk_vlan_aggrEnable_set                          rtrpc_vlan_aggrEnable_set
#define rtk_vlan_portVlanAggrEnable_get                  rtrpc_vlan_portVlanAggrEnable_get
#define rtk_vlan_portVlanAggrEnable_set                  rtrpc_vlan_portVlanAggrEnable_set
#define rtk_vlan_portVlanAggrPriEnable_get               rtrpc_vlan_portVlanAggrPriEnable_get
#define rtk_vlan_portVlanAggrPriEnable_set               rtrpc_vlan_portVlanAggrPriEnable_set
#define rtk_vlan_leakyStpFilter_get                      rtrpc_vlan_leakyStpFilter_get
#define rtk_vlan_leakyStpFilter_set                      rtrpc_vlan_leakyStpFilter_set
#define rtk_vlan_except_get                              rtrpc_vlan_except_get
#define rtk_vlan_except_set                              rtrpc_vlan_except_set
#define rtk_vlan_portIgrCnvtDfltAct_get                  rtrpc_vlan_portIgrCnvtDfltAct_get
#define rtk_vlan_portIgrCnvtDfltAct_set                  rtrpc_vlan_portIgrCnvtDfltAct_set
#define rtk_vlan_portIgrVlanCnvtLuMisAct_get             rtrpc_vlan_portIgrVlanCnvtLuMisAct_get
#define rtk_vlan_portIgrVlanCnvtLuMisAct_set             rtrpc_vlan_portIgrVlanCnvtLuMisAct_set
#define rtk_vlan_portEgrVlanCnvtLuMisAct_get             rtrpc_vlan_portEgrVlanCnvtLuMisAct_get
#define rtk_vlan_portEgrVlanCnvtLuMisAct_set             rtrpc_vlan_portEgrVlanCnvtLuMisAct_set
#define rtk_vlan_portIgrTagKeepType_get                  rtrpc_vlan_portIgrTagKeepType_get
#define rtk_vlan_portIgrTagKeepType_set                  rtrpc_vlan_portIgrTagKeepType_set
#define rtk_vlan_portEgrTagKeepType_get                  rtrpc_vlan_portEgrTagKeepType_get
#define rtk_vlan_portEgrTagKeepType_set                  rtrpc_vlan_portEgrTagKeepType_set
#define rtk_vlan_igrVlanCnvtHitIndication_get            rtrpc_vlan_igrVlanCnvtHitIndication_get
#define rtk_vlan_egrVlanCnvtHitIndication_get            rtrpc_vlan_egrVlanCnvtHitIndication_get
#define rtk_vlan_igrVlanCnvtEntry_delAll                 rtrpc_vlan_igrVlanCnvtEntry_delAll
#define rtk_vlan_egrVlanCnvtEntry_delAll                 rtrpc_vlan_egrVlanCnvtEntry_delAll
#define rtk_vlan_portIgrVlanCnvtRangeCheckSet_get        rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_get
#define rtk_vlan_portIgrVlanCnvtRangeCheckSet_set        rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_set
#define rtk_vlan_igrVlanCnvtRangeCheckEntry_get          rtrpc_vlan_igrVlanCnvtRangeCheckEntry_get
#define rtk_vlan_igrVlanCnvtRangeCheckEntry_set          rtrpc_vlan_igrVlanCnvtRangeCheckEntry_set
#define rtk_vlan_portEgrVlanCnvtRangeCheckSet_get        rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_get
#define rtk_vlan_portEgrVlanCnvtRangeCheckSet_set        rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_set
#define rtk_vlan_egrVlanCnvtRangeCheckEntry_get          rtrpc_vlan_egrVlanCnvtRangeCheckEntry_get
#define rtk_vlan_egrVlanCnvtRangeCheckEntry_set          rtrpc_vlan_egrVlanCnvtRangeCheckEntry_set
#define rtk_vlan_portVlanAggrCtrl_get                    rtrpc_vlan_portVlanAggrCtrl_get
#define rtk_vlan_portVlanAggrCtrl_set                    rtrpc_vlan_portVlanAggrCtrl_set
#define rtk_vlan_portEgrVlanCnvtVidSource_get            rtrpc_vlan_portEgrVlanCnvtVidSource_get
#define rtk_vlan_portEgrVlanCnvtVidSource_set            rtrpc_vlan_portEgrVlanCnvtVidSource_set
#define rtk_vlan_portEgrVlanCnvtVidTarget_get            rtrpc_vlan_portEgrVlanCnvtVidTarget_get
#define rtk_vlan_portEgrVlanCnvtVidTarget_set            rtrpc_vlan_portEgrVlanCnvtVidTarget_set
#define rtk_vlan_portVlanAggrVidSource_get               rtrpc_vlan_portVlanAggrVidSource_get
#define rtk_vlan_portVlanAggrVidSource_set               rtrpc_vlan_portVlanAggrVidSource_set
#define rtk_vlan_portVlanAggrPriTagVidSource_get         rtrpc_vlan_portVlanAggrPriTagVidSource_get
#define rtk_vlan_portVlanAggrPriTagVidSource_set         rtrpc_vlan_portVlanAggrPriTagVidSource_set
#define rtk_vlan_l2UcastLookupMode_get                   rtrpc_vlan_l2UcastLookupMode_get
#define rtk_vlan_l2UcastLookupMode_set                   rtrpc_vlan_l2UcastLookupMode_set
#define rtk_vlan_l2McastLookupMode_get                   rtrpc_vlan_l2McastLookupMode_get
#define rtk_vlan_l2McastLookupMode_set                   rtrpc_vlan_l2McastLookupMode_set
#define rtk_vlan_portInnerAcceptFrameType_get            rtrpc_vlan_portInnerAcceptFrameType_get
#define rtk_vlan_portInnerAcceptFrameType_set            rtrpc_vlan_portInnerAcceptFrameType_set
#define rtk_vlan_portOuterAcceptFrameType_get            rtrpc_vlan_portOuterAcceptFrameType_get
#define rtk_vlan_portOuterAcceptFrameType_set            rtrpc_vlan_portOuterAcceptFrameType_set
#define rtk_vlan_portInnerPvidMode_get                   rtrpc_vlan_portInnerPvidMode_get
#define rtk_vlan_portInnerPvidMode_set                   rtrpc_vlan_portInnerPvidMode_set
#define rtk_vlan_portOuterPvidMode_get                   rtrpc_vlan_portOuterPvidMode_get
#define rtk_vlan_portOuterPvidMode_set                   rtrpc_vlan_portOuterPvidMode_set
#define rtk_vlan_portInnerPvid_get                       rtrpc_vlan_portInnerPvid_get
#define rtk_vlan_portInnerPvid_set                       rtrpc_vlan_portInnerPvid_set
#define rtk_vlan_portOuterPvid_get                       rtrpc_vlan_portOuterPvid_get
#define rtk_vlan_portOuterPvid_set                       rtrpc_vlan_portOuterPvid_set
#define rtk_vlan_innerTpidEntry_get                      rtrpc_vlan_innerTpidEntry_get
#define rtk_vlan_innerTpidEntry_set                      rtrpc_vlan_innerTpidEntry_set
#define rtk_vlan_outerTpidEntry_get                      rtrpc_vlan_outerTpidEntry_get
#define rtk_vlan_outerTpidEntry_set                      rtrpc_vlan_outerTpidEntry_set
#define rtk_vlan_extraTpidEntry_get                      rtrpc_vlan_extraTpidEntry_get
#define rtk_vlan_extraTpidEntry_set                      rtrpc_vlan_extraTpidEntry_set
#define rtk_vlan_portIgrInnerTpid_get                    rtrpc_vlan_portIgrInnerTpid_get
#define rtk_vlan_portIgrInnerTpid_set                    rtrpc_vlan_portIgrInnerTpid_set
#define rtk_vlan_portIgrOuterTpid_get                    rtrpc_vlan_portIgrOuterTpid_get
#define rtk_vlan_portIgrOuterTpid_set                    rtrpc_vlan_portIgrOuterTpid_set
#define rtk_vlan_portEgrInnerTpid_get                    rtrpc_vlan_portEgrInnerTpid_get
#define rtk_vlan_portEgrInnerTpid_set                    rtrpc_vlan_portEgrInnerTpid_set
#define rtk_vlan_portEgrOuterTpid_get                    rtrpc_vlan_portEgrOuterTpid_get
#define rtk_vlan_portEgrOuterTpid_set                    rtrpc_vlan_portEgrOuterTpid_set
#define rtk_vlan_portEgrInnerTagSts_get                  rtrpc_vlan_portEgrInnerTagSts_get
#define rtk_vlan_portEgrInnerTagSts_set                  rtrpc_vlan_portEgrInnerTagSts_set
#define rtk_vlan_portEgrOuterTagSts_get                  rtrpc_vlan_portEgrOuterTagSts_get
#define rtk_vlan_portEgrOuterTagSts_set                  rtrpc_vlan_portEgrOuterTagSts_set
#define rtk_vlan_portIgrTagKeepEnable_get                rtrpc_vlan_portIgrTagKeepEnable_get
#define rtk_vlan_portIgrTagKeepEnable_set                rtrpc_vlan_portIgrTagKeepEnable_set
#define rtk_vlan_portEgrTagKeepEnable_get                rtrpc_vlan_portEgrTagKeepEnable_get
#define rtk_vlan_portEgrTagKeepEnable_set                rtrpc_vlan_portEgrTagKeepEnable_set
#define rtk_vlan_portEgrVlanCnvtLookupMissAct_get        rtrpc_vlan_portEgrVlanCnvtLookupMissAct_get
#define rtk_vlan_portEgrVlanCnvtLookupMissAct_set        rtrpc_vlan_portEgrVlanCnvtLookupMissAct_set
#define rtk_vlan_egrVlanCnvtRangeCheckVid_get            rtrpc_vlan_egrVlanCnvtRangeCheckVid_get
#define rtk_vlan_egrVlanCnvtRangeCheckVid_set            rtrpc_vlan_egrVlanCnvtRangeCheckVid_set
#define rtk_vlan_ecidPmsk_add                            rtrpc_vlan_ecidPmsk_add
#define rtk_vlan_ecidPmsk_del                            rtrpc_vlan_ecidPmsk_del
#define rtk_vlan_ecidPmsk_get                            rtrpc_vlan_ecidPmsk_get
#define rtk_vlan_ecidPmskNextValid_get                   rtrpc_vlan_ecidPmskNextValid_get
#define rtk_vlan_trkVlanAggrEnable_get                   rtrpc_vlan_trkVlanAggrEnable_get
#define rtk_vlan_trkVlanAggrEnable_set                   rtrpc_vlan_trkVlanAggrEnable_set
#define rtk_vlan_trkVlanAggrPriEnable_get                rtrpc_vlan_trkVlanAggrPriEnable_get
#define rtk_vlan_trkVlanAggrPriEnable_set                rtrpc_vlan_trkVlanAggrPriEnable_set
#define rtk_vlan_trkVlanAggrCtrl_get                     rtrpc_vlan_trkVlanAggrCtrl_get
#define rtk_vlan_trkVlanAggrCtrl_set                     rtrpc_vlan_trkVlanAggrCtrl_set
#define rtk_vlan_intfId_get                              rtrpc_vlan_intfId_get
#define rtk_vlan_intfId_set                              rtrpc_vlan_intfId_set

/*
 * Function Declaration
 */

extern int32
rtrpc_vlan_init(uint32 unit);

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

extern int32
rtrpc_vlan_create(uint32 unit, rtk_vlan_t vid);

extern int32
rtrpc_vlan_destroy(uint32 unit, rtk_vlan_t vid);

extern int32
rtrpc_vlan_destroyAll(uint32 unit, uint32 restore_default_vlan);

extern int32
rtrpc_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag);

extern int32
rtrpc_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port);

extern int32
rtrpc_vlan_port_get(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask);

extern int32
rtrpc_vlan_port_set(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask);

extern int32
rtrpc_vlan_mcastGroup_get(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t *pGroupId);

extern int32
rtrpc_vlan_mcastGroup_set(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t groupId);

extern int32
rtrpc_vlan_svlMode_get(uint32 unit, rtk_vlan_svlMode_t *pMode);

extern int32
rtrpc_vlan_svlMode_set(uint32 unit, rtk_vlan_svlMode_t mode);

extern int32
rtrpc_vlan_svlFid_get(uint32 unit, rtk_vlan_t vid, rtk_fid_t *pFid);

extern int32
rtrpc_vlan_svlFid_set(uint32 unit, rtk_vlan_t vid, rtk_fid_t fid);

extern int32
rtrpc_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg);

extern int32
rtrpc_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg);

extern int32
rtrpc_vlan_l2LookupSvlFid_get(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t *pFid);

extern int32
rtrpc_vlan_l2LookupSvlFid_set(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t fid);

extern int32
rtrpc_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t *pLookupMode);

extern int32
rtrpc_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t lookupMode);

extern int32
rtrpc_vlan_groupMask_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask);

extern int32
rtrpc_vlan_groupMask_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask);

extern int32
rtrpc_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx);

extern int32
rtrpc_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx);

/* Module Name     : vlan         */
/* Sub-module Name : vlan profile */

extern int32
rtrpc_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);

extern int32
rtrpc_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);

extern int32
rtrpc_vlan_portFwdVlan_get (uint32 unit, rtk_port_t port, rtk_vlan_pktTagMode_t tagMode, rtk_vlanType_t *pVlanType);

extern int32
rtrpc_vlan_portFwdVlan_set (uint32 unit, rtk_port_t port, rtk_vlan_pktTagMode_t tagMode, rtk_vlanType_t vlanType);

/* Module Name     : vlan           */
/* Sub-module Name : vlan attribute */

/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

extern int32
rtrpc_vlan_portAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

extern int32
rtrpc_vlan_portAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

extern int32
rtrpc_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter);

extern int32
rtrpc_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter);

extern int32
rtrpc_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky);

extern int32
rtrpc_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky);

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

extern int32
rtrpc_vlan_portPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode);

extern int32
rtrpc_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode);

extern int32
rtrpc_vlan_portPvid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_t *pPvid);

extern int32
rtrpc_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_t pvid);

extern int32
rtrpc_vlan_portPrivateVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portPrivateVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_protoGroup_get(uint32 unit, uint32 protoGroup_idx, rtk_vlan_protoGroup_t *pProtoGroup);

extern int32
rtrpc_vlan_protoGroup_set(uint32 unit, uint32 protoGroup_idx, rtk_vlan_protoGroup_t *pProtoGroup);

extern int32
rtrpc_vlan_portProtoVlan_get(uint32 unit, rtk_port_t port, uint32 protoGroup_idx, rtk_vlan_protoVlanCfg_t *pVlan_cfg);

extern int32
rtrpc_vlan_portProtoVlan_set(uint32 unit, rtk_port_t port, uint32 protoGroup_idx, rtk_vlan_protoVlanCfg_t *pVlan_cfg);

extern int32
rtrpc_vlan_macBasedVlan_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

extern int32
rtrpc_vlan_macBasedVlan_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    rtk_mac_t   *pSmac,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

extern int32
rtrpc_vlan_macBasedVlanWithMsk_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

extern int32
rtrpc_vlan_macBasedVlanWithMsk_set(uint32 unit, uint32 index, uint32 valid, rtk_mac_t *pSmac, rtk_mac_t *pSmsk, rtk_vlan_t vid, rtk_pri_t priority);

extern int32
rtrpc_vlan_macBasedVlanWithPort_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_port_t  *pPort,
    rtk_port_t  *pPmsk,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

extern int32
rtrpc_vlan_macBasedVlanWithPort_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_port_t  port,
    rtk_port_t  pmsk,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

extern int32
rtrpc_vlan_portMacBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portMacBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_macBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry);

extern int32
rtrpc_vlan_macBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry);

extern int32
rtrpc_vlan_macBasedVlanEntry_add(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry);

extern int32
rtrpc_vlan_macBasedVlanEntry_del(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry);

extern int32
rtrpc_vlan_ipSubnetBasedVlan_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    ipaddr_t    *pSip,
    ipaddr_t    *pSip_mask,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

extern int32
rtrpc_vlan_ipSubnetBasedVlan_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    ipaddr_t    sip,
    ipaddr_t    sip_mask,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

extern int32
rtrpc_vlan_ipSubnetBasedVlanEntry_add(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

extern int32
rtrpc_vlan_ipSubnetBasedVlanEntry_del(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

extern int32
rtrpc_vlan_ipSubnetBasedVlanWithPort_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    ipaddr_t    *pSip,
    ipaddr_t    *pSip_mask,
    rtk_port_t  *pPort,
    rtk_port_t  *pPort_mask,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);


extern int32
rtrpc_vlan_ipSubnetBasedVlanWithPort_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    ipaddr_t    sip,
    ipaddr_t    sip_mask,
    rtk_port_t  port,
    rtk_port_t  port_mask,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

extern int32
rtrpc_vlan_portIpSubnetBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portIpSubnetBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_ipSubnetBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

extern int32
rtrpc_vlan_ipSubnetBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

/* Module Name    : Vlan               */
/* Sub-module Name: TPID configuration */

extern int32
rtrpc_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 *pTpid);

extern int32
rtrpc_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 tpid);

extern int32
rtrpc_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx_mask);

extern int32
rtrpc_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx_mask);

extern int32
rtrpc_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx);

extern int32
rtrpc_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx);

extern int32
rtrpc_vlan_portEgrTpidSrc_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t *pTpid_src);

extern int32
rtrpc_vlan_portEgrTpidSrc_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t tpid_src);

/* Module Name    : Vlan                 */
/* Sub-module Name: Ingress tag handling */

/* Module Name    : Vlan                */
/* Sub-module Name: Egress tag handling */

extern int32
rtrpc_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts);

extern int32
rtrpc_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,rtk_vlan_tagSts_t sts);

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

extern int32
rtrpc_vlan_portIgrVlanTransparentEnable_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portIgrVlanTransparentEnable_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t enable);

extern int32
rtrpc_vlan_portEgrVlanTransparentEnable_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portEgrVlanTransparentEnable_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t enable);

/*
 * MISC
 */

extern int32
rtrpc_vlan_igrVlanCnvtBlkMode_get(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t *pMode);

extern int32
rtrpc_vlan_igrVlanCnvtBlkMode_set(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t mode);

extern int32
rtrpc_vlan_igrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData);

extern int32
rtrpc_vlan_igrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData);

extern int32
rtrpc_vlan_portIgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portIgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_egrVlanCnvtDblTagEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_egrVlanCnvtDblTagEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_vlan_egrVlanCnvtVidSource_get(uint32 unit, rtk_l2_vlanMode_t *pSrc);

extern int32
rtrpc_vlan_egrVlanCnvtVidSource_set(uint32 unit, rtk_l2_vlanMode_t src);

extern int32
rtrpc_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

extern int32
rtrpc_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

extern int32
rtrpc_vlan_portEgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portEgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_aggrEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_aggrEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_portVlanAggrPriEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_portVlanAggrPriEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl);

extern int32
rtrpc_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl);
extern int32
rtrpc_vlan_leakyStpFilter_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_leakyStpFilter_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_vlan_except_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_vlan_except_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_vlan_portIgrCnvtDfltAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

extern int32
rtrpc_vlan_portIgrCnvtDfltAct_set(uint32 unit, rtk_port_t port, rtk_action_t action);

extern int32
rtrpc_vlan_portIgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t *pAction);

extern int32
rtrpc_vlan_portIgrVlanCnvtLuMisAct_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t action);

extern int32
rtrpc_vlan_portEgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t *pAction);

extern int32
rtrpc_vlan_portEgrVlanCnvtLuMisAct_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t action);

extern int32
rtrpc_vlan_portIgrTagKeepType_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  *pKeeptypeOuter,
    rtk_vlan_tagKeepType_t  *pKeeptypeInner);

extern int32
rtrpc_vlan_portIgrTagKeepType_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  keeptypeOuter,
    rtk_vlan_tagKeepType_t  keeptypeInner);

extern int32
rtrpc_vlan_portEgrTagKeepType_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  *pKeeptypeOuter,
    rtk_vlan_tagKeepType_t  *pKeeptypeInner);

extern int32
rtrpc_vlan_portEgrTagKeepType_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  keeptypeOuter,
    rtk_vlan_tagKeepType_t  keeptypeInner);


extern int32
rtrpc_vlan_portEgrVlanCnvtVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc);

extern int32
rtrpc_vlan_portEgrVlanCnvtVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src);

extern int32
rtrpc_vlan_portEgrVlanCnvtVidTarget_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pTgt);

extern int32
rtrpc_vlan_portEgrVlanCnvtVidTarget_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t tgt);

extern int32
rtrpc_vlan_igrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx, uint32 flag, uint32 *pIsHit);

extern int32
rtrpc_vlan_egrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx, uint32 flag, uint32 *pIsHit);

extern int32
rtrpc_vlan_igrVlanCnvtEntry_delAll(uint32 unit);

extern int32
rtrpc_vlan_egrVlanCnvtEntry_delAll(uint32 unit);

extern int32
rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_get(uint32 unit, rtk_port_t port, uint32 *pSetIdx);

extern int32
rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_set(uint32 unit, rtk_port_t port, uint32 setIdx);

extern int32
rtrpc_vlan_igrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

extern int32
rtrpc_vlan_igrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

extern int32
rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_get(uint32 unit, rtk_port_t port, uint32 *pSetIdx);

extern int32
rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_set(uint32 unit, rtk_port_t port, uint32 setIdx);

extern int32
rtrpc_vlan_egrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

extern int32
rtrpc_vlan_egrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
extern int32
rtrpc_vlan_portVlanAggrVidSource_get(uint32 unit, rtk_port_t port, rtk_vlanType_t *pSrc);

extern int32
rtrpc_vlan_portVlanAggrVidSource_set(uint32 unit, rtk_port_t port, rtk_vlanType_t src);

extern int32
rtrpc_vlan_portVlanAggrPriTagVidSource_get(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t *pSrc);

extern int32
rtrpc_vlan_portVlanAggrPriTagVidSource_set(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t src);

extern int32
rtrpc_vlan_portInnerAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

extern int32
rtrpc_vlan_portInnerAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

extern int32
rtrpc_vlan_portOuterAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

extern int32
rtrpc_vlan_portOuterAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

extern int32
rtrpc_vlan_l2UcastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode);

extern int32
rtrpc_vlan_l2UcastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode);

extern int32
rtrpc_vlan_l2McastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode);

extern int32
rtrpc_vlan_l2McastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode);

extern int32
rtrpc_vlan_innerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

extern int32
rtrpc_vlan_innerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

extern int32
rtrpc_vlan_outerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

extern int32
rtrpc_vlan_outerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

extern int32
rtrpc_vlan_extraTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

extern int32
rtrpc_vlan_extraTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

extern int32
rtrpc_vlan_portInnerPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode);

extern int32
rtrpc_vlan_portInnerPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode);

extern int32
rtrpc_vlan_portOuterPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode);

extern int32
rtrpc_vlan_portOuterPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode);

extern int32
rtrpc_vlan_portInnerPvid_get(uint32 unit, rtk_port_t port, rtk_vlan_t *pPvid);

extern int32
rtrpc_vlan_portInnerPvid_set(uint32 unit, rtk_port_t port, rtk_vlan_t pvid);

extern int32
rtrpc_vlan_portOuterPvid_get(uint32 unit, rtk_port_t port, rtk_vlan_t *pPvid);

extern int32
rtrpc_vlan_portOuterPvid_set(uint32 unit, rtk_port_t port, rtk_vlan_t pvid);

extern int32
rtrpc_vlan_portIgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask);
extern int32
rtrpc_vlan_portIgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask);

extern int32
rtrpc_vlan_portIgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask);

extern int32
rtrpc_vlan_portIgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask);

extern int32
rtrpc_vlan_portEgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx);

extern int32
rtrpc_vlan_portEgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx);

extern int32
rtrpc_vlan_portEgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx);

extern int32
rtrpc_vlan_portEgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx);

extern int32
rtrpc_vlan_portEgrInnerTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts);

extern int32
rtrpc_vlan_portEgrInnerTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts);

extern int32
rtrpc_vlan_portEgrOuterTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts);

extern int32
rtrpc_vlan_portEgrOuterTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts);

extern int32
rtrpc_vlan_portIgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner);

extern int32
rtrpc_vlan_portIgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner);

extern int32
rtrpc_vlan_portEgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner);

extern int32
rtrpc_vlan_portEgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner);

extern int32
rtrpc_vlan_portEgrVlanCnvtLookupMissAct_get(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t *pAct);

extern int32
rtrpc_vlan_portEgrVlanCnvtLookupMissAct_set(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t act);
#endif

extern int32
rtrpc_vlan_egrVlanCnvtRangeCheckVid_get(uint32 unit, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

extern int32
rtrpc_vlan_egrVlanCnvtRangeCheckVid_set(uint32 unit, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

extern int32
rtrpc_vlan_ecidPmsk_add(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

extern int32
rtrpc_vlan_ecidPmsk_del(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

extern int32
rtrpc_vlan_ecidPmsk_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

extern int32
rtrpc_vlan_ecidPmskNextValid_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

extern int32
rtrpc_vlan_trkVlanAggrEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_trkVlanAggrEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

extern int32
rtrpc_vlan_trkVlanAggrPriEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

extern int32
rtrpc_vlan_trkVlanAggrPriEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

extern int32
rtrpc_vlan_trkVlanAggrCtrl_get(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl);

extern int32
rtrpc_vlan_trkVlanAggrCtrl_set(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t vlanAggrCtrl);

extern int32
rtrpc_vlan_intfId_get(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t *pIntfId);

extern int32
rtrpc_vlan_intfId_set(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t intfId);

extern int32
rtrpc_vlanMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_VLAN_H__ */

