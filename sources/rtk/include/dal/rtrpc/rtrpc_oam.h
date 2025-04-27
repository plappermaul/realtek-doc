/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_OAM_H__
#define __RTRPC_OAM_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_oam_init                                     rtrpc_oam_init
#define rtk_oam_portDyingGaspPayload_set                 rtrpc_oam_portDyingGaspPayload_set
#define rtk_oam_dyingGaspSend_set                        rtrpc_oam_dyingGaspSend_set
#define rtk_oam_autoDyingGaspEnable_get                  rtrpc_oam_autoDyingGaspEnable_get
#define rtk_oam_autoDyingGaspEnable_set                  rtrpc_oam_autoDyingGaspEnable_set
#define rtk_oam_dyingGaspWaitTime_get                    rtrpc_oam_dyingGaspWaitTime_get
#define rtk_oam_dyingGaspWaitTime_set                    rtrpc_oam_dyingGaspWaitTime_set
#define rtk_oam_loopbackMacSwapEnable_get                rtrpc_oam_loopbackMacSwapEnable_get
#define rtk_oam_loopbackMacSwapEnable_set                rtrpc_oam_loopbackMacSwapEnable_set
#define rtk_oam_portLoopbackMuxAction_get                rtrpc_oam_portLoopbackMuxAction_get
#define rtk_oam_portLoopbackMuxAction_set                rtrpc_oam_portLoopbackMuxAction_set
#define rtk_oam_cfmCcmPcp_get                            rtrpc_oam_cfmCcmPcp_get
#define rtk_oam_cfmCcmPcp_set                            rtrpc_oam_cfmCcmPcp_set
#define rtk_oam_cfmCcmCfi_get                            rtrpc_oam_cfmCcmCfi_get
#define rtk_oam_cfmCcmCfi_set                            rtrpc_oam_cfmCcmCfi_set
#define rtk_oam_cfmCcmTpid_get                           rtrpc_oam_cfmCcmTpid_get
#define rtk_oam_cfmCcmTpid_set                           rtrpc_oam_cfmCcmTpid_set
#define rtk_oam_cfmCcmInstLifetime_get                   rtrpc_oam_cfmCcmInstLifetime_get
#define rtk_oam_cfmCcmInstLifetime_set                   rtrpc_oam_cfmCcmInstLifetime_set
#define rtk_oam_cfmCcmMepid_get                          rtrpc_oam_cfmCcmMepid_get
#define rtk_oam_cfmCcmMepid_set                          rtrpc_oam_cfmCcmMepid_set
#define rtk_oam_cfmCcmIntervalField_get                  rtrpc_oam_cfmCcmIntervalField_get
#define rtk_oam_cfmCcmIntervalField_set                  rtrpc_oam_cfmCcmIntervalField_set
#define rtk_oam_cfmCcmMdl_get                            rtrpc_oam_cfmCcmMdl_get
#define rtk_oam_cfmCcmMdl_set                            rtrpc_oam_cfmCcmMdl_set
#define rtk_oam_cfmCcmInstTagStatus_get                  rtrpc_oam_cfmCcmInstTagStatus_get
#define rtk_oam_cfmCcmInstTagStatus_set                  rtrpc_oam_cfmCcmInstTagStatus_set
#define rtk_oam_cfmCcmInstVid_get                        rtrpc_oam_cfmCcmInstVid_get
#define rtk_oam_cfmCcmInstVid_set                        rtrpc_oam_cfmCcmInstVid_set
#define rtk_oam_cfmCcmInstMaid_get                       rtrpc_oam_cfmCcmInstMaid_get
#define rtk_oam_cfmCcmInstMaid_set                       rtrpc_oam_cfmCcmInstMaid_set
#define rtk_oam_cfmCcmInstTxStatus_get                   rtrpc_oam_cfmCcmInstTxStatus_get
#define rtk_oam_cfmCcmInstTxStatus_set                   rtrpc_oam_cfmCcmInstTxStatus_set
#define rtk_oam_cfmCcmInstInterval_get                   rtrpc_oam_cfmCcmInstInterval_get
#define rtk_oam_cfmCcmInstInterval_set                   rtrpc_oam_cfmCcmInstInterval_set
#define rtk_oam_cfmCcmTxInstPort_get                     rtrpc_oam_cfmCcmTxInstPort_get
#define rtk_oam_cfmCcmTxInstPort_set                     rtrpc_oam_cfmCcmTxInstPort_set
#define rtk_oam_cfmCcmRxInstVid_get                      rtrpc_oam_cfmCcmRxInstVid_get
#define rtk_oam_cfmCcmRxInstVid_set                      rtrpc_oam_cfmCcmRxInstVid_set
#define rtk_oam_cfmCcmRxInstPort_get                     rtrpc_oam_cfmCcmRxInstPort_get
#define rtk_oam_cfmCcmRxInstPort_set                     rtrpc_oam_cfmCcmRxInstPort_set
#define rtk_oam_cfmCcmInstAliveTime_get                  rtrpc_oam_cfmCcmInstAliveTime_get
#define rtk_oam_cfmPortEthDmEnable_get                   rtrpc_oam_cfmPortEthDmEnable_get
#define rtk_oam_cfmPortEthDmEnable_set                   rtrpc_oam_cfmPortEthDmEnable_set
#define rtk_oam_cfmEthDmRxTimestamp_get                  rtrpc_oam_cfmEthDmRxTimestamp_get
#define rtk_oam_cfmEthDmTxDelay_get                      rtrpc_oam_cfmEthDmTxDelay_get
#define rtk_oam_cfmEthDmTxDelay_set                      rtrpc_oam_cfmEthDmTxDelay_set
#define rtk_oam_cfmEthDmRefTime_get                      rtrpc_oam_cfmEthDmRefTime_get
#define rtk_oam_cfmEthDmRefTime_set                      rtrpc_oam_cfmEthDmRefTime_set
#define rtk_oam_cfmEthDmRefTimeEnable_get                rtrpc_oam_cfmEthDmRefTimeEnable_get
#define rtk_oam_cfmEthDmRefTimeEnable_set                rtrpc_oam_cfmEthDmRefTimeEnable_set
#define rtk_oam_cfmEthDmRefTimeFreq_get                  rtrpc_oam_cfmEthDmRefTimeFreq_get
#define rtk_oam_cfmEthDmRefTimeFreq_set                  rtrpc_oam_cfmEthDmRefTimeFreq_set
#define rtk_oam_dyingGaspPktCnt_get                      rtrpc_oam_dyingGaspPktCnt_get
#define rtk_oam_dyingGaspPktCnt_set                      rtrpc_oam_dyingGaspPktCnt_set
#define rtk_oam_linkFaultMonEnable_set                   rtrpc_oam_linkFaultMonEnable_set
#define rtk_oam_pduLearningEnable_get                    rtrpc_oam_pduLearningEnable_get
#define rtk_oam_pduLearningEnable_set                    rtrpc_oam_pduLearningEnable_set
#define rtk_oam_cfmCcmInstTxMepid_get                    rtrpc_oam_cfmCcmInstTxMepid_get
#define rtk_oam_cfmCcmInstTxMepid_set                    rtrpc_oam_cfmCcmInstTxMepid_set
#define rtk_oam_cfmCcmInstTxIntervalField_get            rtrpc_oam_cfmCcmInstTxIntervalField_get
#define rtk_oam_cfmCcmInstTxIntervalField_set            rtrpc_oam_cfmCcmInstTxIntervalField_set
#define rtk_oam_cfmCcmInstTxMdl_get                      rtrpc_oam_cfmCcmInstTxMdl_get
#define rtk_oam_cfmCcmInstTxMdl_set                      rtrpc_oam_cfmCcmInstTxMdl_set
#define rtk_oam_cfmCcmInstTxMember_get                   rtrpc_oam_cfmCcmInstTxMember_get
#define rtk_oam_cfmCcmInstTxMember_set                   rtrpc_oam_cfmCcmInstTxMember_set
#define rtk_oam_cfmCcmInstRxMember_get                   rtrpc_oam_cfmCcmInstRxMember_get
#define rtk_oam_cfmCcmInstRxMember_set                   rtrpc_oam_cfmCcmInstRxMember_set
/*
 * Function Declaration
 */

extern int32
rtrpc_oam_init(uint32 unit);

/* Module Name    : OAM               */
/* Sub-module Name: OAM configuration */

extern int32
rtrpc_oam_portDyingGaspPayload_set(uint32 unit, rtk_port_t port, uint8 *pPayload,
    uint32 len);

extern int32
rtrpc_oam_dyingGaspSend_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_oam_autoDyingGaspEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_autoDyingGaspEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_oam_dyingGaspWaitTime_get(uint32 unit, uint32 *pTime);

extern int32
rtrpc_oam_dyingGaspWaitTime_set(uint32 unit, uint32 time);

/* Module Name    : OAM               */
/* Sub-module Name: CFM configuration */

extern int32
rtrpc_oam_loopbackMacSwapEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_loopbackMacSwapEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_oam_portLoopbackMuxAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

extern int32
rtrpc_oam_portLoopbackMuxAction_set(uint32 unit, rtk_port_t port, rtk_action_t action);

extern int32
rtrpc_oam_cfmCcmPcp_get(uint32 unit, uint32 *pPcp);

extern int32
rtrpc_oam_cfmCcmPcp_set(uint32 unit, uint32 pcp);

extern int32
rtrpc_oam_cfmCcmCfi_get(uint32 unit, uint32 *pCfi);

extern int32
rtrpc_oam_cfmCcmCfi_set(uint32 unit, uint32 cfi);

extern int32
rtrpc_oam_cfmCcmTpid_get(uint32 unit, uint32 *pTpid);

extern int32
rtrpc_oam_cfmCcmTpid_set(uint32 unit, uint32 tpid);

extern int32
rtrpc_oam_cfmCcmInstLifetime_get(uint32 unit, uint32 instance, uint32 *pLifetime);

extern int32
rtrpc_oam_cfmCcmInstLifetime_set(uint32 unit, uint32 instance, uint32 lifetime);

extern int32
rtrpc_oam_cfmCcmMepid_get(uint32 unit, uint32 *pMepid);

extern int32
rtrpc_oam_cfmCcmMepid_set(uint32 unit, uint32 mepid);

extern int32
rtrpc_oam_cfmCcmIntervalField_get(uint32 unit, uint32 *pInterval);

extern int32
rtrpc_oam_cfmCcmIntervalField_set(uint32 unit, uint32 interval);

extern int32
rtrpc_oam_cfmCcmMdl_get(uint32 unit, uint32 *pMdl);

extern int32
rtrpc_oam_cfmCcmMdl_set(uint32 unit, uint32 mdl);

extern int32
rtrpc_oam_cfmCcmInstTagStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_cfmCcmInstTagStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable);

extern int32
rtrpc_oam_cfmCcmInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid);

extern int32
rtrpc_oam_cfmCcmInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid);

extern int32
rtrpc_oam_cfmCcmInstMaid_get(uint32 unit, uint32 instance, uint32 *pMaid);

extern int32
rtrpc_oam_cfmCcmInstMaid_set(uint32 unit, uint32 instance, uint32 maid);

extern int32
rtrpc_oam_cfmCcmInstTxStatus_get(uint32 unit, uint32 instance, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_cfmCcmInstTxStatus_set(uint32 unit, uint32 instance, rtk_enable_t enable);

extern int32
rtrpc_oam_cfmCcmInstInterval_get(uint32 unit, uint32 instance, uint32 *pInterval);

extern int32
rtrpc_oam_cfmCcmInstInterval_set(uint32 unit, uint32 instance, uint32 interval);

extern int32
rtrpc_oam_cfmCcmTxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort);

extern int32
rtrpc_oam_cfmCcmTxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port);

extern int32
rtrpc_oam_cfmCcmRxInstVid_get(uint32 unit, uint32 instance, rtk_vlan_t *pVid);

extern int32
rtrpc_oam_cfmCcmRxInstVid_set(uint32 unit, uint32 instance, rtk_vlan_t vid);

extern int32
rtrpc_oam_cfmCcmRxInstPort_get(uint32 unit, uint32 instance, uint32 index, rtk_port_t *pPort);

extern int32
rtrpc_oam_cfmCcmRxInstPort_set(uint32 unit, uint32 instance, uint32 index, rtk_port_t port);

extern int32
rtrpc_oam_cfmCcmInstAliveTime_get(uint32 unit, uint32 instance, uint32 index, uint32 *pTime);

extern int32
rtrpc_oam_cfmPortEthDmEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);


extern int32
rtrpc_oam_cfmPortEthDmEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_oam_cfmEthDmRxTimestamp_get(
    uint32 unit,
    uint32 index,
    rtk_time_timeStamp_t *pTimeStamp);

extern int32
rtrpc_oam_cfmEthDmTxDelay_get(
    uint32 unit,
    rtk_oam_cfmEthDmTxDelay_t *pTxDelay);

extern int32
rtrpc_oam_cfmEthDmTxDelay_set(
    uint32                      unit,
    rtk_oam_cfmEthDmTxDelay_t txDelay);

extern int32
rtrpc_oam_cfmEthDmRefTime_get(uint32 unit, rtk_time_timeStamp_t *pTimeStamp);

extern int32
rtrpc_oam_cfmEthDmRefTime_set(uint32 unit, rtk_time_timeStamp_t timeStamp);

extern int32
rtrpc_oam_cfmEthDmRefTimeEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_cfmEthDmRefTimeEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_oam_cfmEthDmRefTimeFreq_get(uint32 unit, uint32 *pFreq);

extern int32
rtrpc_oam_cfmEthDmRefTimeFreq_set(uint32 unit, uint32 freq);


extern int32
rtrpc_oam_dyingGaspPktCnt_get(uint32 unit, uint32 *pCnt);

extern int32
rtrpc_oam_dyingGaspPktCnt_set(uint32 unit, uint32 cnt);

extern int32 rtrpc_oam_linkFaultMonEnable_set(uint32 unit, rtk_enable_t enable);

extern int32 rtrpc_oam_linkFaultMon_register(uint32 unit, rtk_oam_linkFaultMon_callback_t callback);

extern int32 rtrpc_oam_linkFaultMon_unregister(uint32 unit);

extern int32
rtrpc_oam_pduLearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_oam_pduLearningEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_oam_cfmCcmInstTxMepid_get(uint32 unit, uint32 instance, uint32 *mepid);

extern int32
rtrpc_oam_cfmCcmInstTxMepid_set(uint32 unit, uint32 instance, uint32 mepid);

extern int32
rtrpc_oam_cfmCcmInstTxIntervalField_get(uint32 unit, uint32 instance, uint32 *interval);

extern int32
rtrpc_oam_cfmCcmInstTxIntervalField_set(uint32 unit, uint32 instance, uint32 interval);

extern int32
rtrpc_oam_cfmCcmInstTxMdl_get(uint32 unit, uint32 instance, uint32 *mdl);

extern int32
rtrpc_oam_cfmCcmInstTxMdl_set(uint32 unit, uint32 instance, uint32 mdl);

extern int32
rtrpc_oam_cfmCcmInstTxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

extern int32
rtrpc_oam_cfmCcmInstTxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

extern int32
rtrpc_oam_cfmCcmInstRxMember_get(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

extern int32
rtrpc_oam_cfmCcmInstRxMember_set(uint32 unit, uint32 instance,
    rtk_oam_cfmInstMember_t *member);

extern int32
rtrpc_oamMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_OAM_H__ */
