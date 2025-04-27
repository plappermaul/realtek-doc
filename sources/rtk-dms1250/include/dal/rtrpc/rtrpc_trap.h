/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_TRAP_H__
#define __RTRPC_TRAP_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_trap_rmaAction_get                           rtrpc_trap_rmaAction_get
#define rtk_trap_rmaAction_set                           rtrpc_trap_rmaAction_set
#define rtk_trap_bypassStp_get                           rtrpc_trap_bypassStp_get
#define rtk_trap_bypassStp_set                           rtrpc_trap_bypassStp_set
#define rtk_trap_bypassVlan_get                          rtrpc_trap_bypassVlan_get
#define rtk_trap_bypassVlan_set                          rtrpc_trap_bypassVlan_set
#define rtk_trap_userDefineRma_get                       rtrpc_trap_userDefineRma_get
#define rtk_trap_userDefineRma_set                       rtrpc_trap_userDefineRma_set
#define rtk_trap_userDefineRmaEnable_get                 rtrpc_trap_userDefineRmaEnable_get
#define rtk_trap_userDefineRmaEnable_set                 rtrpc_trap_userDefineRmaEnable_set
#define rtk_trap_userDefineRmaAction_get                 rtrpc_trap_userDefineRmaAction_get
#define rtk_trap_userDefineRmaAction_set                 rtrpc_trap_userDefineRmaAction_set
#define rtk_trap_mgmtFrameAction_get                     rtrpc_trap_mgmtFrameAction_get
#define rtk_trap_mgmtFrameAction_set                     rtrpc_trap_mgmtFrameAction_set
#define rtk_trap_mgmtFramePri_get                        rtrpc_trap_mgmtFramePri_get
#define rtk_trap_mgmtFramePri_set                        rtrpc_trap_mgmtFramePri_set
#define rtk_trap_mgmtFrameQueue_get                      rtrpc_trap_mgmtFrameQueue_get
#define rtk_trap_mgmtFrameQueue_set                      rtrpc_trap_mgmtFrameQueue_set
#define rtk_trap_portMgmtFrameAction_get                 rtrpc_trap_portMgmtFrameAction_get
#define rtk_trap_portMgmtFrameAction_set                 rtrpc_trap_portMgmtFrameAction_set
#define rtk_trap_pktWithCFIAction_get                    rtrpc_trap_pktWithCFIAction_get
#define rtk_trap_pktWithCFIAction_set                    rtrpc_trap_pktWithCFIAction_set
#define rtk_trap_pktWithOuterCFIAction_get               rtrpc_trap_pktWithOuterCFIAction_get
#define rtk_trap_pktWithOuterCFIAction_set               rtrpc_trap_pktWithOuterCFIAction_set
#define rtk_trap_pktWithCFIPri_get                       rtrpc_trap_pktWithCFIPri_get
#define rtk_trap_pktWithCFIPri_set                       rtrpc_trap_pktWithCFIPri_set
#define rtk_trap_cfmFrameTrapPri_get                     rtrpc_trap_cfmFrameTrapPri_get
#define rtk_trap_cfmFrameTrapPri_set                     rtrpc_trap_cfmFrameTrapPri_set
#define rtk_trap_oamPDUAction_get                        rtrpc_trap_oamPDUAction_get
#define rtk_trap_oamPDUAction_set                        rtrpc_trap_oamPDUAction_set
#define rtk_trap_oamPDUPri_get                           rtrpc_trap_oamPDUPri_get
#define rtk_trap_oamPDUPri_set                           rtrpc_trap_oamPDUPri_set
#define rtk_trap_cfmUnknownFrameAct_get                  rtrpc_trap_cfmUnknownFrameAct_get
#define rtk_trap_cfmUnknownFrameAct_set                  rtrpc_trap_cfmUnknownFrameAct_set
#define rtk_trap_cfmLoopbackLinkTraceAct_get             rtrpc_trap_cfmLoopbackLinkTraceAct_get
#define rtk_trap_cfmLoopbackLinkTraceAct_set             rtrpc_trap_cfmLoopbackLinkTraceAct_set
#define rtk_trap_cfmCcmAct_get                           rtrpc_trap_cfmCcmAct_get
#define rtk_trap_cfmCcmAct_set                           rtrpc_trap_cfmCcmAct_set
#define rtk_trap_cfmEthDmAct_get                         rtrpc_trap_cfmEthDmAct_get
#define rtk_trap_cfmEthDmAct_set                         rtrpc_trap_cfmEthDmAct_set
#define rtk_trap_portOamLoopbackParAction_get            rtrpc_trap_portOamLoopbackParAction_get
#define rtk_trap_portOamLoopbackParAction_set            rtrpc_trap_portOamLoopbackParAction_set
#define rtk_trap_routeExceptionAction_get                rtrpc_trap_routeExceptionAction_get
#define rtk_trap_routeExceptionAction_set                rtrpc_trap_routeExceptionAction_set
#define rtk_trap_routeExceptionPri_get                   rtrpc_trap_routeExceptionPri_get
#define rtk_trap_routeExceptionPri_set                   rtrpc_trap_routeExceptionPri_set
#define rtk_trap_userDefineRmaLearningEnable_get         rtrpc_trap_userDefineRmaLearningEnable_get
#define rtk_trap_userDefineRmaLearningEnable_set         rtrpc_trap_userDefineRmaLearningEnable_set
#define rtk_trap_rmaLearningEnable_get                   rtrpc_trap_rmaLearningEnable_get
#define rtk_trap_rmaLearningEnable_set                   rtrpc_trap_rmaLearningEnable_set
#define rtk_trap_mgmtFrameLearningEnable_get             rtrpc_trap_mgmtFrameLearningEnable_get
#define rtk_trap_mgmtFrameLearningEnable_set             rtrpc_trap_mgmtFrameLearningEnable_set
#define rtk_trap_mgmtFrameMgmtVlanEnable_get             rtrpc_trap_mgmtFrameMgmtVlanEnable_get
#define rtk_trap_mgmtFrameMgmtVlanEnable_set             rtrpc_trap_mgmtFrameMgmtVlanEnable_set
#define rtk_trap_bpduFloodPortmask_get                   rtrpc_trap_bpduFloodPortmask_get
#define rtk_trap_bpduFloodPortmask_set                   rtrpc_trap_bpduFloodPortmask_set
#define rtk_trap_eapolFloodPortmask_get                  rtrpc_trap_eapolFloodPortmask_get
#define rtk_trap_eapolFloodPortmask_set                  rtrpc_trap_eapolFloodPortmask_set
#define rtk_trap_lldpFloodPortmask_get                   rtrpc_trap_lldpFloodPortmask_get
#define rtk_trap_lldpFloodPortmask_set                   rtrpc_trap_lldpFloodPortmask_set
#define rtk_trap_userDefineFloodPortmask_get             rtrpc_trap_userDefineFloodPortmask_get
#define rtk_trap_userDefineFloodPortmask_set             rtrpc_trap_userDefineFloodPortmask_set
#define rtk_trap_rmaFloodPortmask_get                    rtrpc_trap_rmaFloodPortmask_get
#define rtk_trap_rmaFloodPortmask_set                    rtrpc_trap_rmaFloodPortmask_set
#define rtk_trap_rmaCancelMirror_get                     rtrpc_trap_rmaCancelMirror_get
#define rtk_trap_rmaCancelMirror_set                     rtrpc_trap_rmaCancelMirror_set
#define rtk_trap_rmaGroupAction_get                      rtrpc_trap_rmaGroupAction_get
#define rtk_trap_rmaGroupAction_set                      rtrpc_trap_rmaGroupAction_set
#define rtk_trap_rmaGroupLearningEnable_get              rtrpc_trap_rmaGroupLearningEnable_get
#define rtk_trap_rmaGroupLearningEnable_set              rtrpc_trap_rmaGroupLearningEnable_set
#define rtk_trap_mgmtFrameSelfARPEnable_get              rtrpc_trap_mgmtFrameSelfARPEnable_get
#define rtk_trap_mgmtFrameSelfARPEnable_set              rtrpc_trap_mgmtFrameSelfARPEnable_set
#define rtk_trap_rmaLookupMissActionEnable_get           rtrpc_trap_rmaLookupMissActionEnable_get
#define rtk_trap_rmaLookupMissActionEnable_set           rtrpc_trap_rmaLookupMissActionEnable_set
#define rtk_trap_cfmAct_get                              rtrpc_trap_cfmAct_get
#define rtk_trap_cfmAct_set                              rtrpc_trap_cfmAct_set
#define rtk_trap_cfmTarget_get                           rtrpc_trap_cfmTarget_get
#define rtk_trap_cfmTarget_set                           rtrpc_trap_cfmTarget_set
#define rtk_trap_oamTarget_get                           rtrpc_trap_oamTarget_get
#define rtk_trap_oamTarget_set                           rtrpc_trap_oamTarget_set
#define rtk_trap_mgmtFrameTarget_get                     rtrpc_trap_mgmtFrameTarget_get
#define rtk_trap_mgmtFrameTarget_set                     rtrpc_trap_mgmtFrameTarget_set
#define rtk_trap_capwapInvldHdr_get                      rtrpc_trap_capwapInvldHdr_get
#define rtk_trap_capwapInvldHdr_set                      rtrpc_trap_capwapInvldHdr_set

/*
 * Function Declaration
 */

extern int32
rtrpc_trap_init(uint32 unit);

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */

/* Module Name    : Trap     */
/* Sub-module Name: RMA      */

extern int32
rtrpc_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action);

extern int32
rtrpc_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action);

extern int32
rtrpc_trap_rmaGroupAction_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t *pRma_action);

extern int32
rtrpc_trap_rmaGroupAction_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t rma_action);

extern int32
rtrpc_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

extern int32
rtrpc_trap_rmaGroupLearningEnable_get(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_rmaGroupLearningEnable_set(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t enable);

extern int32
rtrpc_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable);

extern int32
rtrpc_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable);

/* Module Name    : Trap     */
/* Sub-module Name: User defined RMA */

extern int32
rtrpc_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

extern int32
rtrpc_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

extern int32
rtrpc_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

extern int32
rtrpc_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction);

extern int32
rtrpc_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action);

extern int32
rtrpc_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Module Name    : Trap                         */
/* Sub-module Name: System-wise management frame */

extern int32
rtrpc_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

extern int32
rtrpc_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

extern int32
rtrpc_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority);

extern int32
rtrpc_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority);

extern int32
rtrpc_trap_mgmtFrameQueue_get(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t *pQid);

extern int32
rtrpc_trap_mgmtFrameQueue_set(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t qid);

extern int32
rtrpc_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable);

/* Module Name    : Trap                                   */
/* Sub-module Name: Per port user defined management frame */

extern int32
rtrpc_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

extern int32
rtrpc_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Module Name    : Trap                               */
/* Sub-module Name: Packet with special flag or option */

extern int32
rtrpc_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority);

extern int32
rtrpc_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority);

/* Module Name    : Trap       */
/* Sub-module Name: CFM and OAM packet */

extern int32
rtrpc_trap_cfmFrameTrapPri_get(uint32 unit, rtk_pri_t *pPriority);

extern int32
rtrpc_trap_cfmFrameTrapPri_set(uint32 unit, rtk_pri_t priority);

extern int32
rtrpc_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_trap_oamPDUAction_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_trap_oamPDUPri_get(uint32 unit, rtk_pri_t *pPriority);

extern int32
rtrpc_trap_oamPDUPri_set(uint32 unit, rtk_pri_t priority);
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
extern int32
rtrpc_trap_cfmUnknownFrameAct_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_trap_cfmUnknownFrameAct_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_trap_cfmLoopbackLinkTraceAct_get(uint32 unit, uint32 level, rtk_action_t *pAction);

extern int32
rtrpc_trap_cfmLoopbackLinkTraceAct_set(uint32 unit, uint32 level, rtk_action_t action);

extern int32
rtrpc_trap_cfmCcmAct_get(uint32 unit, uint32 level,rtk_trap_oam_action_t *pAction);

extern int32
rtrpc_trap_cfmCcmAct_set(uint32 unit, uint32 level, rtk_trap_oam_action_t action);

extern int32
rtrpc_trap_cfmEthDmAct_get(uint32 unit, uint32 mdl, rtk_action_t *pAction);

extern int32
rtrpc_trap_cfmEthDmAct_set(uint32 unit, uint32 mdl, rtk_action_t action);
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
extern int32
rtrpc_trap_portOamLoopbackParAction_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_trap_oam_action_t   *pAction);

extern int32
rtrpc_trap_portOamLoopbackParAction_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_trap_oam_action_t   action);

extern int32
rtrpc_trap_routeExceptionAction_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    *pAction);

extern int32
rtrpc_trap_routeExceptionAction_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    action);

extern int32
rtrpc_trap_routeExceptionPri_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_pri_t                       *pPriority);

extern int32
rtrpc_trap_routeExceptionPri_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_pri_t                       priority);

extern int32
rtrpc_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_trap_bpduFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_bpduFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_eapolFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_eapolFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_lldpFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_lldpFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_userDefineFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_userDefineFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_rmaFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_rmaFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

extern int32
rtrpc_trap_rmaCancelMirror_get(uint32 unit,  rtk_enable_t *pEnable);

extern int32
rtrpc_trap_rmaCancelMirror_set(uint32 unit,  rtk_enable_t enable);

extern int32
rtrpc_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable);


extern int32
rtrpc_trap_cfmAct_get(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t *pAction);

extern int32
rtrpc_trap_cfmAct_set(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t action);

extern int32
rtrpc_trap_cfmTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_trap_cfmTarget_set(uint32 unit, rtk_trapTarget_t target);

extern int32
rtrpc_trap_oamTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_trap_oamTarget_set(uint32 unit, rtk_trapTarget_t target);

extern int32
rtrpc_trap_mgmtFrameTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_trap_mgmtFrameTarget_set(uint32 unit, rtk_trapTarget_t target);

extern int32
rtrpc_trap_capwapInvldHdr_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_trap_capwapInvldHdr_set(uint32 unit, rtk_enable_t enable);


extern int32
rtrpc_trapMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_TRAP_H__ */

