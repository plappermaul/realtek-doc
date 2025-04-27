/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_L2_H__
#define __RTRPC_L2_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_l2_init                                      rtrpc_l2_init
#define rtk_l2_flushLinkDownPortAddrEnable_get           rtrpc_l2_flushLinkDownPortAddrEnable_get
#define rtk_l2_flushLinkDownPortAddrEnable_set           rtrpc_l2_flushLinkDownPortAddrEnable_set
#define rtk_l2_ucastAddr_flush                           rtrpc_l2_ucastAddr_flush
#define rtk_l2_learningCnt_get                           rtrpc_l2_learningCnt_get
#define rtk_l2_portLearningCnt_get                       rtrpc_l2_portLearningCnt_get
#define rtk_l2_fidLearningCnt_get                        rtrpc_l2_fidLearningCnt_get
#define rtk_l2_macLearningCnt_get                        rtrpc_l2_macLearningCnt_get
#define rtk_l2_limitLearningCnt_get                      rtrpc_l2_limitLearningCnt_get
#define rtk_l2_limitLearningCnt_set                      rtrpc_l2_limitLearningCnt_set
#define rtk_l2_portLimitLearningCnt_get                  rtrpc_l2_portLimitLearningCnt_get
#define rtk_l2_portLimitLearningCnt_set                  rtrpc_l2_portLimitLearningCnt_set
#define rtk_l2_limitLearningNum_get                      rtrpc_l2_limitLearningNum_get
#define rtk_l2_limitLearningNum_set                      rtrpc_l2_limitLearningNum_set
#define rtk_l2_limitLearningCntAction_get                rtrpc_l2_limitLearningCntAction_get
#define rtk_l2_limitLearningCntAction_set                rtrpc_l2_limitLearningCntAction_set
#define rtk_l2_portLimitLearningCntAction_get            rtrpc_l2_portLimitLearningCntAction_get
#define rtk_l2_portLimitLearningCntAction_set            rtrpc_l2_portLimitLearningCntAction_set
#define rtk_l2_fidLearningCntAction_get                  rtrpc_l2_fidLearningCntAction_get
#define rtk_l2_fidLearningCntAction_set                  rtrpc_l2_fidLearningCntAction_set
#define rtk_l2_limitLearningAction_get                   rtrpc_l2_limitLearningAction_get
#define rtk_l2_limitLearningAction_set                   rtrpc_l2_limitLearningAction_set
#define rtk_l2_fidLimitLearningEntry_get                 rtrpc_l2_fidLimitLearningEntry_get
#define rtk_l2_fidLimitLearningEntry_set                 rtrpc_l2_fidLimitLearningEntry_set
#define rtk_l2_fidLearningCnt_reset                      rtrpc_l2_fidLearningCnt_reset
#define rtk_l2_aging_get                                 rtrpc_l2_aging_get
#define rtk_l2_aging_set                                 rtrpc_l2_aging_set
#define rtk_l2_agingTime_get                             rtrpc_l2_agingTime_get
#define rtk_l2_agingTime_set                             rtrpc_l2_agingTime_set
#define rtk_l2_portAgingEnable_get                       rtrpc_l2_portAgingEnable_get
#define rtk_l2_portAgingEnable_set                       rtrpc_l2_portAgingEnable_set
#define rtk_l2_trkAgingEnable_get                        rtrpc_l2_trkAgingEnable_get
#define rtk_l2_trkAgingEnable_set                        rtrpc_l2_trkAgingEnable_set
#define rtk_l2_hashAlgo_get                              rtrpc_l2_hashAlgo_get
#define rtk_l2_hashAlgo_set                              rtrpc_l2_hashAlgo_set
#define rtk_l2_bucketHashAlgo_get                        rtrpc_l2_bucketHashAlgo_get
#define rtk_l2_bucketHashAlgo_set                        rtrpc_l2_bucketHashAlgo_set
#define rtk_l2_vlanMode_get                              rtrpc_l2_vlanMode_get
#define rtk_l2_vlanMode_set                              rtrpc_l2_vlanMode_set
#define rtk_l2_learningFullAction_get                    rtrpc_l2_learningFullAction_get
#define rtk_l2_learningFullAction_set                    rtrpc_l2_learningFullAction_set
#define rtk_l2_portNewMacOp_get                          rtrpc_l2_portNewMacOp_get
#define rtk_l2_portNewMacOp_set                          rtrpc_l2_portNewMacOp_set
#define rtk_l2_addr_init                                 rtrpc_l2_addr_init
#define rtk_l2_addr_add                                  rtrpc_l2_addr_add
#define rtk_l2_addr_del                                  rtrpc_l2_addr_del
#define rtk_l2_addr_get                                  rtrpc_l2_addr_get
#define rtk_l2_addr_set                                  rtrpc_l2_addr_set
#define rtk_l2_addr_delAll                               rtrpc_l2_addr_delAll
#define rtk_l2_nextValidAddr_get                         rtrpc_l2_nextValidAddr_get
#define rtk_l2_mcastAddr_init                            rtrpc_l2_mcastAddr_init
#define rtk_l2_mcastAddr_add                             rtrpc_l2_mcastAddr_add
#define rtk_l2_mcastAddr_del                             rtrpc_l2_mcastAddr_del
#define rtk_l2_mcastAddr_get                             rtrpc_l2_mcastAddr_get
#define rtk_l2_mcastAddr_set                             rtrpc_l2_mcastAddr_set
#define rtk_l2_mcastAddr_addByIndex                      rtrpc_l2_mcastAddr_addByIndex
#define rtk_l2_mcastAddr_setByIndex                 rtrpc_l2_mcastAddr_setByIndex
#define rtk_l2_mcastAddr_delIgnoreIndex         rtrpc_l2_mcastAddr_delIgnoreIndex
#define rtk_l2_nextValidMcastAddr_get                    rtrpc_l2_nextValidMcastAddr_get
#define rtk_l2_ipmcMode_get                              rtrpc_l2_ipmcMode_get
#define rtk_l2_ipmcMode_set                              rtrpc_l2_ipmcMode_set
#define rtk_l2_ipMcastAddrExt_init                       rtrpc_l2_ipMcastAddrExt_init
#define rtk_l2_ipMcastAddr_add                           rtrpc_l2_ipMcastAddr_add
#define rtk_l2_ipMcastAddr_del                           rtrpc_l2_ipMcastAddr_del
#define rtk_l2_ipMcastAddr_get                           rtrpc_l2_ipMcastAddr_get
#define rtk_l2_ipMcastAddr_set                           rtrpc_l2_ipMcastAddr_set
#define rtk_l2_ipMcastAddr_addByIndex                    rtrpc_l2_ipMcastAddr_addByIndex
#define rtk_l2_ipMcastAddr_setByIndex               rtrpc_l2_ipMcastAddr_setByIndex
#define rtk_l2_ipMcastAddr_delIgnoreIndex       rtrpc_l2_ipMcastAddr_delIgnoreIndex
#define rtk_l2_nextValidIpMcastAddr_get                  rtrpc_l2_nextValidIpMcastAddr_get
#define rtk_l2_ipMcastAddrChkEnable_get                  rtrpc_l2_ipMcastAddrChkEnable_get
#define rtk_l2_ipMcastAddrChkEnable_set                  rtrpc_l2_ipMcastAddrChkEnable_set
#define rtk_l2_ipMcstFidVidCompareEnable_get             rtrpc_l2_ipMcstFidVidCompareEnable_get
#define rtk_l2_ipMcstFidVidCompareEnable_set             rtrpc_l2_ipMcstFidVidCompareEnable_set
#define rtk_l2_ip6mcMode_get                             rtrpc_l2_ip6mcMode_get
#define rtk_l2_ip6mcMode_set                             rtrpc_l2_ip6mcMode_set
#define rtk_l2_ip6CareByte_get                           rtrpc_l2_ip6CareByte_get
#define rtk_l2_ip6CareByte_set                           rtrpc_l2_ip6CareByte_set
#define rtk_l2_ip6McastAddrExt_init                      rtrpc_l2_ip6McastAddrExt_init
#define rtk_l2_ip6McastAddr_add                          rtrpc_l2_ip6McastAddr_add
#define rtk_l2_ip6McastAddr_del                          rtrpc_l2_ip6McastAddr_del
#define rtk_l2_ip6McastAddr_get                          rtrpc_l2_ip6McastAddr_get
#define rtk_l2_ip6McastAddr_set                          rtrpc_l2_ip6McastAddr_set
#define rtk_l2_ip6McastAddr_addByIndex                   rtrpc_l2_ip6McastAddr_addByIndex
#define rtk_l2_ip6McastAddr_setByIndex          rtrpc_l2_ip6McastAddr_setByIndex
#define rtk_l2_ip6McastAddr_delIgnoreIndex      rtrpc_l2_ip6McastAddr_delIgnoreIndex
#define rtk_l2_nextValidIp6McastAddr_get                 rtrpc_l2_nextValidIp6McastAddr_get
#define rtk_l2_mcastFwdIndex_alloc                       rtrpc_l2_mcastFwdIndex_alloc
#define rtk_l2_mcastFwdIndex_free                        rtrpc_l2_mcastFwdIndex_free
#define rtk_l2_mcastFwdIndexFreeCount_get                rtrpc_l2_mcastFwdIndexFreeCount_get
#define rtk_l2_mcastFwdPortmask_get                      rtrpc_l2_mcastFwdPortmask_get
#define rtk_l2_mcastFwdPortmask_set                      rtrpc_l2_mcastFwdPortmask_set
#define rtk_l2_mcastFwdPortmaskEntry_get                 rtrpc_l2_mcastFwdPortmaskEntry_get
#define rtk_l2_mcastFwdPortmaskEntry_set                 rtrpc_l2_mcastFwdPortmaskEntry_set
#define rtk_l2_cpuMacAddr_add                            rtrpc_l2_cpuMacAddr_add
#define rtk_l2_cpuMacAddr_del                            rtrpc_l2_cpuMacAddr_del
#define rtk_l2_legalPortMoveAction_get                   rtrpc_l2_legalPortMoveAction_get
#define rtk_l2_legalPortMoveAction_set                   rtrpc_l2_legalPortMoveAction_set
#define rtk_l2_dynamicPortMoveForbidAction_get           rtrpc_l2_dynamicPortMoveForbidAction_get
#define rtk_l2_dynamicPortMoveForbidAction_set           rtrpc_l2_dynamicPortMoveForbidAction_set
#define rtk_l2_portMoveAction_get                        rtrpc_l2_portMoveAction_get
#define rtk_l2_portMoveAction_set                        rtrpc_l2_portMoveAction_set
#define rtk_l2_portMoveLearn_get                         rtrpc_l2_portMoveLearn_get
#define rtk_l2_portMoveLearn_set                         rtrpc_l2_portMoveLearn_set
#define rtk_l2_legalPortMoveFlushAddrEnable_get          rtrpc_l2_legalPortMoveFlushAddrEnable_get
#define rtk_l2_legalPortMoveFlushAddrEnable_set          rtrpc_l2_legalPortMoveFlushAddrEnable_set
#define rtk_l2_staticPortMoveAction_get                  rtrpc_l2_staticPortMoveAction_get
#define rtk_l2_staticPortMoveAction_set                  rtrpc_l2_staticPortMoveAction_set
#define rtk_l2_lookupMissFloodPortMask_get               rtrpc_l2_lookupMissFloodPortMask_get
#define rtk_l2_lookupMissFloodPortMask_set               rtrpc_l2_lookupMissFloodPortMask_set
#define rtk_l2_lookupMissFloodPortMask_add               rtrpc_l2_lookupMissFloodPortMask_add
#define rtk_l2_lookupMissFloodPortMask_del               rtrpc_l2_lookupMissFloodPortMask_del
#define rtk_l2_lookupMissFloodPortMask_setByIndex        rtrpc_l2_lookupMissFloodPortMask_setByIndex
#define rtk_l2_lookupMissFloodPortMaskIdx_get            rtrpc_l2_lookupMissFloodPortMaskIdx_get
#define rtk_l2_lookupMissFloodPortMaskIdx_set            rtrpc_l2_lookupMissFloodPortMaskIdx_set
#define rtk_l2_portLookupMissAction_get                  rtrpc_l2_portLookupMissAction_get
#define rtk_l2_portLookupMissAction_set                  rtrpc_l2_portLookupMissAction_set
#define rtk_l2_portUcastLookupMissAction_get             rtrpc_l2_portUcastLookupMissAction_get
#define rtk_l2_portUcastLookupMissAction_set             rtrpc_l2_portUcastLookupMissAction_set
#define rtk_l2_srcPortEgrFilterMask_get                  rtrpc_l2_srcPortEgrFilterMask_get
#define rtk_l2_srcPortEgrFilterMask_set                  rtrpc_l2_srcPortEgrFilterMask_set
#define rtk_l2_srcPortEgrFilterMask_add                  rtrpc_l2_srcPortEgrFilterMask_add
#define rtk_l2_srcPortEgrFilterMask_del                  rtrpc_l2_srcPortEgrFilterMask_del
#define rtk_l2_exceptionAddrAction_get                   rtrpc_l2_exceptionAddrAction_get
#define rtk_l2_exceptionAddrAction_set                   rtrpc_l2_exceptionAddrAction_set
#define rtk_l2_addrEntry_get                             rtrpc_l2_addrEntry_get
#define rtk_l2_conflictAddr_get                          rtrpc_l2_conflictAddr_get
#define rtk_l2_zeroSALearningEnable_get                  rtrpc_l2_zeroSALearningEnable_get
#define rtk_l2_zeroSALearningEnable_set                  rtrpc_l2_zeroSALearningEnable_set
#define rtk_l2_secureMacMode_get                         rtrpc_l2_secureMacMode_get
#define rtk_l2_secureMacMode_set                         rtrpc_l2_secureMacMode_set
#define rtk_l2_portDynamicPortMoveForbidEnable_get       rtrpc_l2_portDynamicPortMoveForbidEnable_get
#define rtk_l2_portDynamicPortMoveForbidEnable_set       rtrpc_l2_portDynamicPortMoveForbidEnable_set
#define rtk_l2_trkDynamicPortMoveForbidEnable_get        rtrpc_l2_trkDynamicPortMoveForbidEnable_get
#define rtk_l2_trkDynamicPortMoveForbidEnable_set        rtrpc_l2_trkDynamicPortMoveForbidEnable_set
#define rtk_l2_portMacFilterEnable_get                   rtrpc_l2_portMacFilterEnable_get
#define rtk_l2_portMacFilterEnable_set                   rtrpc_l2_portMacFilterEnable_set
#define rtk_l2_hwNextValidAddr_get                       rtrpc_l2_hwNextValidAddr_get
#define rtk_l2_portCtrl_get                              rtrpc_l2_portCtrl_get
#define rtk_l2_portCtrl_set                              rtrpc_l2_portCtrl_set
#define rtk_l2_status_get                                rtrpc_l2_status_get
#define rtk_l2_stkLearningEnable_get                     rtrpc_l2_stkLearningEnable_get
#define rtk_l2_stkLearningEnable_set                     rtrpc_l2_stkLearningEnable_set
#define rtk_l2_stkKeepUcastEntryValid_get                rtrpc_l2_stkKeepUcastEntryValid_get
#define rtk_l2_stkKeepUcastEntryValid_set                rtrpc_l2_stkKeepUcastEntryValid_set
#define rtk_l2_entryCnt_get                              rtrpc_l2_entryCnt_get
#define rtk_l2_hashIdx_get                               rtrpc_l2_hashIdx_get
#define rtk_l2_addr_delByMac                             rtrpc_l2_addr_delByMac
/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */

extern int32
rtrpc_l2_init(uint32 unit);

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

extern int32
rtrpc_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig);

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_learningCnt_get(uint32 unit, uint32 *pMac_cnt);

extern int32
rtrpc_l2_portLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt);

extern int32
rtrpc_l2_fidLearningCnt_get(uint32 unit, uint32 fid_macLimit_idx, uint32 *pNum);
#endif

extern int32
rtrpc_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_limitLearningCnt_get(uint32 unit, uint32 *pMac_cnt);

extern int32
rtrpc_l2_limitLearningCnt_set(uint32 unit, uint32 mac_cnt);

extern int32
rtrpc_l2_portLimitLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt);

extern int32
rtrpc_l2_portLimitLearningCnt_set(uint32 unit, rtk_port_t port, uint32 mac_cnt);
#endif

extern int32
rtrpc_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);


extern int32
rtrpc_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_limitLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction);

extern int32
rtrpc_l2_limitLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action);

extern int32
rtrpc_l2_portLimitLearningCntAction_get(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t *pAction);

extern int32
rtrpc_l2_portLimitLearningCntAction_set(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t action);

extern int32
rtrpc_l2_fidLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction);

extern int32
rtrpc_l2_fidLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action);
#endif

extern int32
rtrpc_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

extern int32
rtrpc_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

extern int32
rtrpc_l2_fidLimitLearningEntry_get(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

extern int32
rtrpc_l2_fidLimitLearningEntry_set(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

extern int32
rtrpc_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx);

/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_aging_get(uint32 unit, uint32 *pAging_time);

extern int32
rtrpc_l2_aging_set(uint32 unit, uint32 aging_time);
#endif

extern int32
rtrpc_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time);

extern int32
rtrpc_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time);

extern int32
rtrpc_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo);

extern int32
rtrpc_l2_hashAlgo_set(uint32 unit, uint32 hash_algo);
#endif

extern int32
rtrpc_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo);

extern int32
rtrpc_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo);

extern int32
rtrpc_l2_vlanMode_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pVlanMode);

extern int32
rtrpc_l2_vlanMode_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t vlanMode);

/* Module Name    : L2      */
/* Sub-module Name: Unicast */

extern int32
rtrpc_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_l2_learningFullAction_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_l2_portNewMacOp_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction);

extern int32
rtrpc_l2_portNewMacOp_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction);

/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

extern int32
rtrpc_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr);

extern int32
rtrpc_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

extern int32
rtrpc_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

extern int32
rtrpc_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

extern int32
rtrpc_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

extern int32
rtrpc_l2_addr_delAll(uint32 unit, uint32 include_static);

extern int32
rtrpc_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_data);

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

extern int32
rtrpc_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

extern int32
rtrpc_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_setByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

extern int32
rtrpc_l2_mcastAddr_delIgnoreIndex(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

extern int32
rtrpc_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pL2_data);

/* Module Name    : L2           */
/* Sub-module Name: IP multicast */

extern int32
rtrpc_l2_ipmcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode);

extern int32
rtrpc_l2_ipmcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode);

extern int32
rtrpc_l2_ipMcastAddrExt_init(uint32 unit, rtk_l2_ipMcastHashKey_t *pIpMcast_hashKey, rtk_l2_ipMcastAddr_t *pIpMcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_add(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_del(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid);

extern int32
rtrpc_l2_ipMcastAddr_get(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_set(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_addByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpMcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_setByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ipMcastAddr_delIgnoreIndex(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid);

extern int32
rtrpc_l2_nextValidIpMcastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ipMcastAddr_t    *pL2_data);

extern int32
rtrpc_l2_ipMcastAddrChkEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_ipMcastAddrChkEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_ipMcstFidVidCompareEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_ipMcstFidVidCompareEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_ip6mcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode);

extern int32
rtrpc_l2_ip6mcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode);

extern int32
rtrpc_l2_ip6CareByte_get(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 *pCareByte);

extern int32
rtrpc_l2_ip6CareByte_set(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 careByte);

extern int32
rtrpc_l2_ip6McastAddrExt_init(uint32 unit, rtk_l2_ip6McastHashKey_t *pIp6Mcast_hashKey, rtk_l2_ip6McastAddr_t *pIp6Mcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_add(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_del(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid);

extern int32
rtrpc_l2_ip6McastAddr_get(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_set(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_addByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpMcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_setByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr);

extern int32
rtrpc_l2_ip6McastAddr_delIgnoreIndex(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid);

extern int32
rtrpc_l2_nextValidIp6McastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ip6McastAddr_t    *pL2_data);

/* Module Name    : L2                         */
/* Sub-module Name: Multicast forwarding table */

extern int32
rtrpc_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex);

extern int32
rtrpc_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index);

extern int32
rtrpc_l2_mcastFwdIndexFreeCount_get(uint32 unit, uint32 *pFreeCount);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_mcastFwdPortmask_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          *pCrossVlan);

extern int32
rtrpc_l2_mcastFwdPortmask_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          crossVlan);
#endif

extern int32
rtrpc_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

extern int32
rtrpc_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

/* Module Name    : L2              */
/* Sub-module Name: CPU MAC address */

extern int32
rtrpc_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);


extern int32
rtrpc_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Module Name    : L2        */
/* Sub-module Name: Port move */
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_l2_legalPortMoveAction_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        *pFwdAction);

extern int32
rtrpc_l2_legalPortMoveAction_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        fwdAction);

extern int32
rtrpc_l2_dynamicPortMoveForbidAction_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_l2_dynamicPortMoveForbidAction_set(uint32 unit, rtk_action_t action);
#endif

extern int32
rtrpc_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

extern int32
rtrpc_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

extern int32
rtrpc_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn);

extern int32
rtrpc_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn);

extern int32
rtrpc_l2_legalPortMoveFlushAddrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_legalPortMoveFlushAddrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_l2_staticPortMoveAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pFwdAction);

extern int32
rtrpc_l2_staticPortMoveAction_set(uint32 unit, rtk_port_t port, rtk_action_t fwdAction);

/* Module Name    : L2                        */
/* Sub-module Name: Parameter for lookup miss */

extern int32
rtrpc_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

extern int32
rtrpc_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

extern int32
rtrpc_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

extern int32
rtrpc_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

extern int32
rtrpc_l2_lookupMissFloodPortMask_setByIndex(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx, rtk_portmask_t *pFlood_portmask);

extern int32
rtrpc_l2_lookupMissFloodPortMaskIdx_get(uint32 unit, rtk_l2_lookupMissType_t type, uint32 *pIdx);

extern int32
rtrpc_l2_lookupMissFloodPortMaskIdx_set(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx);

extern int32
rtrpc_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction);

extern int32
rtrpc_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action);

extern int32
rtrpc_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

extern int32
rtrpc_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action);

/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */

extern int32
rtrpc_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask);

extern int32
rtrpc_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask);

extern int32
rtrpc_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port);

extern int32
rtrpc_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port);

/*
 * MISC
 */

extern int32
rtrpc_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction);

extern int32
rtrpc_l2_exceptionAddrAction_set(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    action);

extern int32
rtrpc_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry);

extern int32
rtrpc_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt);

extern int32
rtrpc_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_secureMacMode_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_secureMacMode_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

extern int32
rtrpc_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable);

extern int32
rtrpc_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry);

extern int32
rtrpc_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg);

extern int32
rtrpc_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg);

extern int32
rtrpc_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg);

extern int32
rtrpc_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt);

extern int32
rtrpc_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx);

extern int32
rtrpc_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac);

extern int32
rtrpc_l2Mapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_L2_H__ */

