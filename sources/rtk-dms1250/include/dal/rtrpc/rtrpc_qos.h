/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_QOS_H__
#define __RTRPC_QOS_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_qos_queueNum_get                             rtrpc_qos_queueNum_get
#define rtk_qos_queueNum_set                             rtrpc_qos_queueNum_set
#define rtk_qos_priMap_get                               rtrpc_qos_priMap_get
#define rtk_qos_priMap_set                               rtrpc_qos_priMap_set
#define rtk_qos_1pPriRemap_get                           rtrpc_qos_1pPriRemap_get
#define rtk_qos_1pPriRemap_set                           rtrpc_qos_1pPriRemap_set
#define rtk_qos_pri2QidMap_get                           rtrpc_qos_pri2QidMap_get
#define rtk_qos_pri2QidMap_set                           rtrpc_qos_pri2QidMap_set
#define rtk_qos_cpuQid2QidMap_get                        rtrpc_qos_cpuQid2QidMap_get
#define rtk_qos_cpuQid2QidMap_set                        rtrpc_qos_cpuQid2QidMap_set
#define rtk_qos_cpuQid2StackQidMap_get                   rtrpc_qos_cpuQid2StackQidMap_get
#define rtk_qos_cpuQid2StackQidMap_set                   rtrpc_qos_cpuQid2StackQidMap_set
#define rtk_qos_dpSrcSel_get                             rtrpc_qos_dpSrcSel_get
#define rtk_qos_dpSrcSel_set                             rtrpc_qos_dpSrcSel_set
#define rtk_qos_priSelGroup_get                          rtrpc_qos_priSelGroup_get
#define rtk_qos_priSelGroup_set                          rtrpc_qos_priSelGroup_set
#define rtk_qos_portPriSelGroup_get                      rtrpc_qos_portPriSelGroup_get
#define rtk_qos_portPriSelGroup_set                      rtrpc_qos_portPriSelGroup_set
#define rtk_qos_portInnerPri_get                         rtrpc_qos_portInnerPri_get
#define rtk_qos_portInnerPri_set                         rtrpc_qos_portInnerPri_set
#define rtk_qos_portOuterPri_get                         rtrpc_qos_portOuterPri_get
#define rtk_qos_portOuterPri_set                         rtrpc_qos_portOuterPri_set
#define rtk_qos_deiDpRemap_get                           rtrpc_qos_deiDpRemap_get
#define rtk_qos_deiDpRemap_set                           rtrpc_qos_deiDpRemap_set
#define rtk_qos_portDEISrcSel_get                        rtrpc_qos_portDEISrcSel_get
#define rtk_qos_portDEISrcSel_set                        rtrpc_qos_portDEISrcSel_set
#define rtk_qos_portDpSel_get                            rtrpc_qos_portDpSel_get
#define rtk_qos_portDpSel_set                            rtrpc_qos_portDpSel_set
#define rtk_qos_dscpDpRemap_get                          rtrpc_qos_dscpDpRemap_get
#define rtk_qos_dscpDpRemap_set                          rtrpc_qos_dscpDpRemap_set
#define rtk_qos_dpRemap_get                              rtrpc_qos_dpRemap_get
#define rtk_qos_dpRemap_set                              rtrpc_qos_dpRemap_set
#define rtk_qos_priRemap_get                             rtrpc_qos_priRemap_get
#define rtk_qos_priRemap_set                             rtrpc_qos_priRemap_set
#define rtk_qos_dscpPriRemap_get                         rtrpc_qos_dscpPriRemap_get
#define rtk_qos_dscpPriRemap_set                         rtrpc_qos_dscpPriRemap_set
#define rtk_qos_portPri_get                              rtrpc_qos_portPri_get
#define rtk_qos_portPri_set                              rtrpc_qos_portPri_set
#define rtk_qos_outer1pPriRemap_get                      rtrpc_qos_outer1pPriRemap_get
#define rtk_qos_outer1pPriRemap_set                      rtrpc_qos_outer1pPriRemap_set
#define rtk_qos_port1pRemarkEnable_get                   rtrpc_qos_port1pRemarkEnable_get
#define rtk_qos_port1pRemarkEnable_set                   rtrpc_qos_port1pRemarkEnable_set
#define rtk_qos_1pRemarking_get                          rtrpc_qos_1pRemarking_get
#define rtk_qos_1pRemarking_set                          rtrpc_qos_1pRemarking_set
#define rtk_qos_port1pDfltPri_get                        rtrpc_qos_port1pDfltPri_get
#define rtk_qos_port1pDfltPri_set                        rtrpc_qos_port1pDfltPri_set
#define rtk_qos_port1pDfltPriExt_get                     rtrpc_qos_port1pDfltPriExt_get
#define rtk_qos_port1pDfltPriExt_set                     rtrpc_qos_port1pDfltPriExt_set
#define rtk_qos_port1pDfltPriSrcSel_get                  rtrpc_qos_port1pDfltPriSrcSel_get
#define rtk_qos_port1pDfltPriSrcSel_set                  rtrpc_qos_port1pDfltPriSrcSel_set
#define rtk_qos_port1pDfltPriSrcSelExt_get               rtrpc_qos_port1pDfltPriSrcSelExt_get
#define rtk_qos_port1pDfltPriSrcSelExt_set               rtrpc_qos_port1pDfltPriSrcSelExt_set
#define rtk_qos_1pDfltPriCfgSrcSel_get                   rtrpc_qos_1pDfltPriCfgSrcSel_get
#define rtk_qos_1pDfltPriCfgSrcSel_set                   rtrpc_qos_1pDfltPriCfgSrcSel_set
#define rtk_qos_portOut1pRemarkEnable_get                rtrpc_qos_portOut1pRemarkEnable_get
#define rtk_qos_portOut1pRemarkEnable_set                rtrpc_qos_portOut1pRemarkEnable_set
#define rtk_qos_outer1pRemarking_get                     rtrpc_qos_outer1pRemarking_get
#define rtk_qos_outer1pRemarking_set                     rtrpc_qos_outer1pRemarking_set
#define rtk_qos_portOuter1pDfltPri_get                   rtrpc_qos_portOuter1pDfltPri_get
#define rtk_qos_portOuter1pDfltPri_set                   rtrpc_qos_portOuter1pDfltPri_set
#define rtk_qos_portOuter1pDfltPriExt_get                rtrpc_qos_portOuter1pDfltPriExt_get
#define rtk_qos_portOuter1pDfltPriExt_set                rtrpc_qos_portOuter1pDfltPriExt_set
#define rtk_qos_portDscpRemarkEnable_get                 rtrpc_qos_portDscpRemarkEnable_get
#define rtk_qos_portDscpRemarkEnable_set                 rtrpc_qos_portDscpRemarkEnable_set
#define rtk_qos_dscpRemarking_get                        rtrpc_qos_dscpRemarking_get
#define rtk_qos_dscpRemarking_set                        rtrpc_qos_dscpRemarking_set
#define rtk_qos_portDeiRemarkEnable_get                  rtrpc_qos_portDeiRemarkEnable_get
#define rtk_qos_portDeiRemarkEnable_set                  rtrpc_qos_portDeiRemarkEnable_set
#define rtk_qos_deiRemarking_get                         rtrpc_qos_deiRemarking_get
#define rtk_qos_deiRemarking_set                         rtrpc_qos_deiRemarking_set
#define rtk_qos_deiRemarkSrcSel_get                      rtrpc_qos_deiRemarkSrcSel_get
#define rtk_qos_deiRemarkSrcSel_set                      rtrpc_qos_deiRemarkSrcSel_set
#define rtk_qos_1pDfltPri_get                            rtrpc_qos_1pDfltPri_get
#define rtk_qos_1pDfltPri_set                            rtrpc_qos_1pDfltPri_set
#define rtk_qos_1pRemarkSrcSel_get                       rtrpc_qos_1pRemarkSrcSel_get
#define rtk_qos_1pRemarkSrcSel_set                       rtrpc_qos_1pRemarkSrcSel_set
#define rtk_qos_1pRemark_get                             rtrpc_qos_1pRemark_get
#define rtk_qos_1pRemark_set                             rtrpc_qos_1pRemark_set
#define rtk_qos_outer1pRemarkSrcSel_get                  rtrpc_qos_outer1pRemarkSrcSel_get
#define rtk_qos_outer1pRemarkSrcSel_set                  rtrpc_qos_outer1pRemarkSrcSel_set
#define rtk_qos_portOuter1pDfltPriSrcSel_get             rtrpc_qos_portOuter1pDfltPriSrcSel_get
#define rtk_qos_portOuter1pDfltPriSrcSel_set             rtrpc_qos_portOuter1pDfltPriSrcSel_set
#define rtk_qos_portOuter1pDfltPriSrcSelExt_get          rtrpc_qos_portOuter1pDfltPriSrcSelExt_get
#define rtk_qos_portOuter1pDfltPriSrcSelExt_set          rtrpc_qos_portOuter1pDfltPriSrcSelExt_set
#define rtk_qos_dscpRemarkSrcSel_get                     rtrpc_qos_dscpRemarkSrcSel_get
#define rtk_qos_dscpRemarkSrcSel_set                     rtrpc_qos_dscpRemarkSrcSel_set
#define rtk_qos_dscpRemark_get                           rtrpc_qos_dscpRemark_get
#define rtk_qos_dscpRemark_set                           rtrpc_qos_dscpRemark_set
#define rtk_qos_dscp2Dot1pRemark_get                     rtrpc_qos_dscp2Dot1pRemark_get
#define rtk_qos_dscp2Dot1pRemark_set                     rtrpc_qos_dscp2Dot1pRemark_set
#define rtk_qos_dscp2Outer1pRemark_get                   rtrpc_qos_dscp2Outer1pRemark_get
#define rtk_qos_dscp2Outer1pRemark_set                   rtrpc_qos_dscp2Outer1pRemark_set
#define rtk_qos_dscp2DscpRemark_get                      rtrpc_qos_dscp2DscpRemark_get
#define rtk_qos_dscp2DscpRemark_set                      rtrpc_qos_dscp2DscpRemark_set
#define rtk_qos_outer1pRemark_get                        rtrpc_qos_outer1pRemark_get
#define rtk_qos_outer1pRemark_set                        rtrpc_qos_outer1pRemark_set
#define rtk_qos_deiRemark_get                            rtrpc_qos_deiRemark_get
#define rtk_qos_deiRemark_set                            rtrpc_qos_deiRemark_set
#define rtk_qos_portDeiRemarkTagSel_get                  rtrpc_qos_portDeiRemarkTagSel_get
#define rtk_qos_portDeiRemarkTagSel_set                  rtrpc_qos_portDeiRemarkTagSel_set
#define rtk_qos_schedulingAlgorithm_get                  rtrpc_qos_schedulingAlgorithm_get
#define rtk_qos_schedulingAlgorithm_set                  rtrpc_qos_schedulingAlgorithm_set
#define rtk_qos_schedulingQueue_get                      rtrpc_qos_schedulingQueue_get
#define rtk_qos_schedulingQueue_set                      rtrpc_qos_schedulingQueue_set
#define rtk_qos_congAvoidAlgo_get                        rtrpc_qos_congAvoidAlgo_get
#define rtk_qos_congAvoidAlgo_set                        rtrpc_qos_congAvoidAlgo_set
#define rtk_qos_portCongAvoidAlgo_get                    rtrpc_qos_portCongAvoidAlgo_get
#define rtk_qos_portCongAvoidAlgo_set                    rtrpc_qos_portCongAvoidAlgo_set
#define rtk_qos_congAvoidSysThresh_get                   rtrpc_qos_congAvoidSysThresh_get
#define rtk_qos_congAvoidSysThresh_set                   rtrpc_qos_congAvoidSysThresh_set
#define rtk_qos_congAvoidSysDropProbability_get          rtrpc_qos_congAvoidSysDropProbability_get
#define rtk_qos_congAvoidSysDropProbability_set          rtrpc_qos_congAvoidSysDropProbability_set
#define rtk_qos_congAvoidGlobalQueueThresh_get           rtrpc_qos_congAvoidGlobalQueueThresh_get
#define rtk_qos_congAvoidGlobalQueueThresh_set           rtrpc_qos_congAvoidGlobalQueueThresh_set
#define rtk_qos_congAvoidGlobalQueueDropProbability_get  rtrpc_qos_congAvoidGlobalQueueDropProbability_get
#define rtk_qos_congAvoidGlobalQueueDropProbability_set  rtrpc_qos_congAvoidGlobalQueueDropProbability_set
#define rtk_qos_congAvoidGlobalQueueConfig_get           rtrpc_qos_congAvoidGlobalQueueConfig_get
#define rtk_qos_congAvoidGlobalQueueConfig_set           rtrpc_qos_congAvoidGlobalQueueConfig_set
#define rtk_qos_portAvbStreamReservationClassEnable_get  rtrpc_qos_portAvbStreamReservationClassEnable_get
#define rtk_qos_portAvbStreamReservationClassEnable_set  rtrpc_qos_portAvbStreamReservationClassEnable_set
#define rtk_qos_avbStreamReservationConfig_get           rtrpc_qos_avbStreamReservationConfig_get
#define rtk_qos_avbStreamReservationConfig_set           rtrpc_qos_avbStreamReservationConfig_set
#define rtk_qos_pkt2CpuPriRemap_get                      rtrpc_qos_pkt2CpuPriRemap_get
#define rtk_qos_pkt2CpuPriRemap_set                      rtrpc_qos_pkt2CpuPriRemap_set
#define rtk_qos_rspanPriRemap_get                        rtrpc_qos_rspanPriRemap_get
#define rtk_qos_rspanPriRemap_set                        rtrpc_qos_rspanPriRemap_set
#define rtk_qos_portPri2IgrQMap_get                      rtrpc_qos_portPri2IgrQMap_get
#define rtk_qos_portPri2IgrQMap_set                      rtrpc_qos_portPri2IgrQMap_set
#define rtk_qos_portPri2IgrQMapEnable_get                rtrpc_qos_portPri2IgrQMapEnable_get
#define rtk_qos_portPri2IgrQMapEnable_set                rtrpc_qos_portPri2IgrQMapEnable_set
#define rtk_qos_portIgrQueueWeight_get                   rtrpc_qos_portIgrQueueWeight_get
#define rtk_qos_portIgrQueueWeight_set                   rtrpc_qos_portIgrQueueWeight_set
#define rtk_qos_1pDfltPriSrcSel_get                      rtrpc_qos_1pDfltPriSrcSel_get
#define rtk_qos_1pDfltPriSrcSel_set                      rtrpc_qos_1pDfltPriSrcSel_set
#define rtk_qos_portOuter1pRemarkSrcSel_get              rtrpc_qos_portOuter1pRemarkSrcSel_get
#define rtk_qos_portOuter1pRemarkSrcSel_set              rtrpc_qos_portOuter1pRemarkSrcSel_set
#define rtk_qos_outer1pDfltPri_get                       rtrpc_qos_outer1pDfltPri_get
#define rtk_qos_outer1pDfltPri_set                       rtrpc_qos_outer1pDfltPri_set
#define rtk_qos_outer1pDfltPriCfgSrcSel_get              rtrpc_qos_outer1pDfltPriCfgSrcSel_get
#define rtk_qos_outer1pDfltPriCfgSrcSel_set              rtrpc_qos_outer1pDfltPriCfgSrcSel_set
#define rtk_qos_invldDscpVal_get                         rtrpc_qos_invldDscpVal_get
#define rtk_qos_invldDscpVal_set                         rtrpc_qos_invldDscpVal_set
#define rtk_qos_invldDscpMask_get                        rtrpc_qos_invldDscpMask_get
#define rtk_qos_invldDscpMask_set                        rtrpc_qos_invldDscpMask_set
#define rtk_qos_portInvldDscpEnable_get                  rtrpc_qos_portInvldDscpEnable_get
#define rtk_qos_portInvldDscpEnable_set                  rtrpc_qos_portInvldDscpEnable_set
#define rtk_qos_invldDscpEnable_get                      rtrpc_qos_invldDscpEnable_get
#define rtk_qos_invldDscpEnable_set                      rtrpc_qos_invldDscpEnable_set
#define rtk_qos_portPriRemapEnable_get                   rtrpc_qos_portPriRemapEnable_get
#define rtk_qos_portPriRemapEnable_set                   rtrpc_qos_portPriRemapEnable_set
#define rtk_qos_sysPortPriRemapSel_get                   rtrpc_qos_sysPortPriRemapSel_get
#define rtk_qos_sysPortPriRemapSel_set                   rtrpc_qos_sysPortPriRemapSel_set
#define rtk_qos_portPortPriRemapSel_get                  rtrpc_qos_portPortPriRemapSel_get
#define rtk_qos_portPortPriRemapSel_set                  rtrpc_qos_portPortPriRemapSel_set
#define rtk_qos_portInnerPriRemapEnable_get              rtrpc_qos_portInnerPriRemapEnable_get
#define rtk_qos_portInnerPriRemapEnable_set              rtrpc_qos_portInnerPriRemapEnable_set
#define rtk_qos_portOuterPriRemapEnable_get              rtrpc_qos_portOuterPriRemapEnable_get
#define rtk_qos_portOuterPriRemapEnable_set              rtrpc_qos_portOuterPriRemapEnable_set
#define rtk_qos_priRemapEnable_get                       rtrpc_qos_priRemapEnable_get
#define rtk_qos_priRemapEnable_set                       rtrpc_qos_priRemapEnable_set
#define rtk_qos_portQueueStrictEnable_get                rtrpc_qos_portQueueStrictEnable_get
#define rtk_qos_portQueueStrictEnable_set                rtrpc_qos_portQueueStrictEnable_set
/*
 * Function Declaration
 */

extern int32
rtrpc_qos_init(uint32 unit, uint32 queueNum);

/* Module Name    : QoS                       */
/* Sub-module Name: Ingress priority decision */

extern int32
rtrpc_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

extern int32
rtrpc_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

extern int32
rtrpc_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx);

extern int32
rtrpc_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx);

extern int32
rtrpc_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType);
/* rename rtrpc_qos_portPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType); */

extern int32
rtrpc_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type);
/* rename rtrpc_qos_portPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type); */

extern int32
rtrpc_qos_portPortPriRemapSel_get(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t *pType);

extern int32
rtrpc_qos_portPortPriRemapSel_set(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t type);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_qos_portPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pInt_pri);

extern int32
rtrpc_qos_portPri_set(uint32 unit, rtk_port_t port, rtk_pri_t int_pri);

extern int32
rtrpc_qos_portPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_qos_portInnerPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_portInnerPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);
#endif

extern int32
rtrpc_qos_dpSrcSel_get(uint32 unit, rtk_qos_dpSrc_t *pType);

extern int32
rtrpc_qos_dpSrcSel_set(uint32 unit, rtk_qos_dpSrc_t type);

extern int32
rtrpc_qos_portDEISrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

extern int32
rtrpc_qos_portDEISrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

extern int32
rtrpc_qos_portDpSel_get(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel);

extern int32
rtrpc_qos_portDpSel_set(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel);

extern int32
rtrpc_qos_deiDpRemap_get(uint32 unit, uint32 dei, uint32 *pDp);

extern int32
rtrpc_qos_deiDpRemap_set(uint32 unit, uint32 dei, uint32 dp);

extern int32
rtrpc_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 *pDp);

extern int32
rtrpc_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_qos_dscpDpRemap_get(uint32 unit, uint32 dscp, uint32 *pDp);

extern int32
rtrpc_qos_dscpDpRemap_set(uint32 unit, uint32 dscp, uint32 dp);

extern int32
rtrpc_qos_portOuterPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_portOuterPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

extern int32
rtrpc_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri);

extern int32
rtrpc_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri);

extern int32
rtrpc_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri);

extern int32
rtrpc_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri);

extern int32
rtrpc_qos_outer1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t *pInt_pri);

extern int32
rtrpc_qos_outer1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t int_pri);
#endif

extern int32
rtrpc_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri);

extern int32
rtrpc_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri);

extern int32
rtrpc_qos_queueNum_get(uint32 unit, uint32 *pQueue_num);

extern int32
rtrpc_qos_queueNum_set(uint32 unit, uint32 queue_num);

extern int32
rtrpc_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

extern int32
rtrpc_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid);

extern int32
rtrpc_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid);

extern int32
rtrpc_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid);

extern int32
rtrpc_qos_cpuQid2QidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid);

extern int32
rtrpc_qos_cpuQid2QidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid);

extern int32
rtrpc_qos_cpuQid2StackQidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid);

extern int32
rtrpc_qos_cpuQid2StackQidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid);


/* Module Name    : QoS           */
/* Sub-module Name: Egress remark */

extern int32
rtrpc_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri);

extern int32
rtrpc_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri);

extern int32
rtrpc_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType);

extern int32
rtrpc_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type);

extern int32
rtrpc_qos_port1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_port1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

extern int32
rtrpc_qos_port1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_port1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri);

extern int32
rtrpc_qos_port1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc);

extern int32
rtrpc_qos_port1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src);

extern int32
rtrpc_qos_port1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc);

extern int32
rtrpc_qos_port1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src);

extern int32
rtrpc_qos_1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_1pDfltCfgSrc_t *pCfg);

extern int32
rtrpc_qos_1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_1pDfltCfgSrc_t cfg);

extern int32
rtrpc_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t * pType);

extern int32
rtrpc_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type);

extern int32
rtrpc_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri);

extern int32
rtrpc_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

extern int32
rtrpc_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_qos_portOuter1pRemarkSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t * pType);

extern int32
rtrpc_qos_portOuter1pRemarkSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t type);

extern int32
rtrpc_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri);

extern int32
rtrpc_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri);

extern int32
rtrpc_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t *pType);

extern int32
rtrpc_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type);

extern int32
rtrpc_qos_portOuter1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_portOuter1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri);

extern int32
rtrpc_qos_portOuter1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri);

extern int32
rtrpc_qos_portOuter1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri);

extern int32
rtrpc_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType);

extern int32
rtrpc_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type);

extern int32
rtrpc_qos_portOuter1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType);

extern int32
rtrpc_qos_portOuter1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type);

extern int32
rtrpc_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel);

extern int32
rtrpc_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel);

extern int32
rtrpc_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t * pDot1p_pri);

extern int32
rtrpc_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri);

extern int32
rtrpc_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


extern int32
rtrpc_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp);

extern int32
rtrpc_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp);

extern int32
rtrpc_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType);

extern int32
rtrpc_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type);

 extern int32
rtrpc_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType);

extern int32
rtrpc_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type);

extern int32
rtrpc_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei);

extern int32
rtrpc_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei);

extern int32
rtrpc_qos_deiRemarkSrcSel_get(uint32 unit, rtk_qos_deiRmkSrc_t *pType);

extern int32
rtrpc_qos_deiRemarkSrcSel_set(uint32 unit, rtk_qos_deiRmkSrc_t type);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

extern int32
rtrpc_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);


extern int32
rtrpc_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri);

extern int32
rtrpc_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri);

extern int32
rtrpc_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp);

extern int32
rtrpc_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp);

extern int32
rtrpc_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp);

extern int32
rtrpc_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp);

extern int32
rtrpc_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

extern int32
rtrpc_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

extern int32
rtrpc_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri);

extern int32
rtrpc_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri);

extern int32
rtrpc_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei);

extern int32
rtrpc_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei);
#endif

/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

extern int32
rtrpc_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type);

extern int32
rtrpc_qos_schedulingAlgorithm_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   scheduling_type);

extern int32
rtrpc_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

extern int32
rtrpc_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Module Name    : QoS              */
/* Sub-module Name: Congestion avoidance */

extern int32
rtrpc_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo);

extern int32
rtrpc_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo);

extern int32
rtrpc_qos_portCongAvoidAlgo_get(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t *pAlgo);

extern int32
rtrpc_qos_portCongAvoidAlgo_set(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t algo);

extern int32
rtrpc_qos_congAvoidSysThresh_get(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

extern int32
rtrpc_qos_congAvoidSysThresh_set(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

extern int32
rtrpc_qos_congAvoidSysDropProbability_get(
    uint32  unit,
    uint32  dp,
    uint32  *pProbability);

extern int32
rtrpc_qos_congAvoidSysDropProbability_set(
    uint32  unit,
    uint32  dp,
    uint32  probability);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_qos_congAvoidGlobalQueueThresh_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

extern int32
rtrpc_qos_congAvoidGlobalQueueThresh_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh);

extern int32
rtrpc_qos_congAvoidGlobalQueueDropProbability_get(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      *pProbability);

extern int32
rtrpc_qos_congAvoidGlobalQueueDropProbability_set(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      probability);
#endif

extern int32
rtrpc_qos_congAvoidGlobalQueueConfig_get(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

extern int32
rtrpc_qos_congAvoidGlobalQueueConfig_set(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh);

/*
 * Component: WRED
 */

extern int32
rtrpc_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable);

extern int32
rtrpc_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable);

extern int32
rtrpc_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

extern int32
rtrpc_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf);

extern int32
rtrpc_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t intPri, rtk_pri_t *pNewPri);

extern int32
rtrpc_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t intPri, rtk_pri_t newPri);

extern int32
rtrpc_qos_rspanPriRemap_get(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t *pInt_pri);

extern int32
rtrpc_qos_rspanPriRemap_set(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t int_pri);

extern int32
rtrpc_qos_portPri2IgrQMapEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portPri2IgrQMapEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_qos_portPri2IgrQMap_get(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid);

extern int32
rtrpc_qos_portPri2IgrQMap_set(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid);

extern int32
rtrpc_qos_portIgrQueueWeight_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pQweight);

extern int32
rtrpc_qos_portIgrQueueWeight_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 qWeight);

extern int32
rtrpc_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp);

extern int32
rtrpc_qos_invldDscpVal_set(uint32 unit, uint32 dscp);

extern int32
rtrpc_qos_invldDscpMask_get(uint32 unit, uint32 *pDscpMask) ;

extern int32
rtrpc_qos_invldDscpMask_set(uint32 unit, uint32 dscpMask) ;

extern int32
rtrpc_qos_portInvldDscpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portInvldDscpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable) ;

extern int32
rtrpc_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable) ;

extern int32
rtrpc_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable) ;

extern int32
rtrpc_qos_portInnerPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portInnerPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_qos_portOuterPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portOuterPriRemapEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable);

extern int32
rtrpc_qos_portQueueStrictEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

extern int32
rtrpc_qos_portQueueStrictEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

extern int32
rtrpc_qosMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_QOS_H__ */
