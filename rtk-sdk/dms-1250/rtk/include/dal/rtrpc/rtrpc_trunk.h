/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_TRUNK_H__
#define __RTRPC_TRUNK_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_trunk_mode_get                               rtrpc_trunk_mode_get
#define rtk_trunk_mode_set                               rtrpc_trunk_mode_set
#define rtk_trunk_port_get                               rtrpc_trunk_port_get
#define rtk_trunk_port_set                               rtrpc_trunk_port_set
#define rtk_trunk_localPort_get                          rtrpc_trunk_localPort_get
#define rtk_trunk_localPort_set                          rtrpc_trunk_localPort_set
#define rtk_trunk_egrPort_get                            rtrpc_trunk_egrPort_get
#define rtk_trunk_egrPort_set                            rtrpc_trunk_egrPort_set
#define rtk_trunk_tunnelHashSrc_get                      rtrpc_trunk_tunnelHashSrc_get
#define rtk_trunk_tunnelHashSrc_set                      rtrpc_trunk_tunnelHashSrc_set
#define rtk_trunk_distributionAlgorithmBind_get          rtrpc_trunk_distributionAlgorithmBind_get
#define rtk_trunk_distributionAlgorithmBind_set          rtrpc_trunk_distributionAlgorithmBind_set
#define rtk_trunk_distributionAlgorithmTypeBind_get      rtrpc_trunk_distributionAlgorithmTypeBind_get
#define rtk_trunk_distributionAlgorithmTypeBind_set      rtrpc_trunk_distributionAlgorithmTypeBind_set
#define rtk_trunk_distributionAlgorithmParam_get         rtrpc_trunk_distributionAlgorithmParam_get
#define rtk_trunk_distributionAlgorithmParam_set         rtrpc_trunk_distributionAlgorithmParam_set
#define rtk_trunk_distributionAlgorithmTypeParam_get     rtrpc_trunk_distributionAlgorithmTypeParam_get
#define rtk_trunk_distributionAlgorithmTypeParam_set     rtrpc_trunk_distributionAlgorithmTypeParam_set
#define rtk_trunk_distributionAlgorithmShift_get         rtrpc_trunk_distributionAlgorithmShift_get
#define rtk_trunk_distributionAlgorithmShift_set         rtrpc_trunk_distributionAlgorithmShift_set
#define rtk_trunk_distributionAlgorithmShiftGbl_get      rtrpc_trunk_distributionAlgorithmShiftGbl_get
#define rtk_trunk_distributionAlgorithmShiftGbl_set      rtrpc_trunk_distributionAlgorithmShiftGbl_set
#define rtk_trunk_trafficSeparate_get                    rtrpc_trunk_trafficSeparate_get
#define rtk_trunk_trafficSeparate_set                    rtrpc_trunk_trafficSeparate_set
#define rtk_trunk_trafficSeparateEnable_get              rtrpc_trunk_trafficSeparateEnable_get
#define rtk_trunk_trafficSeparateEnable_set              rtrpc_trunk_trafficSeparateEnable_set
#define rtk_trunk_trafficSeparateDivision_get            rtrpc_trunk_trafficSeparateDivision_get
#define rtk_trunk_trafficSeparateDivision_set            rtrpc_trunk_trafficSeparateDivision_set
#define rtk_trunk_stkTrkPort_get                         rtrpc_trunk_stkTrkPort_get
#define rtk_trunk_stkTrkPort_set                         rtrpc_trunk_stkTrkPort_set
#define rtk_trunk_stkTrkHash_get                         rtrpc_trunk_stkTrkHash_get
#define rtk_trunk_stkTrkHash_set                         rtrpc_trunk_stkTrkHash_set
#define rtk_trunk_stkDistributionAlgorithmTypeBind_get   rtrpc_trunk_stkDistributionAlgorithmTypeBind_get
#define rtk_trunk_stkDistributionAlgorithmTypeBind_set   rtrpc_trunk_stkDistributionAlgorithmTypeBind_set
#define rtk_trunk_localFirst_get                         rtrpc_trunk_localFirst_get
#define rtk_trunk_localFirst_set                         rtrpc_trunk_localFirst_set
#define rtk_trunk_localFirstFailOver_get                 rtrpc_trunk_localFirstFailOver_get
#define rtk_trunk_localFirstFailOver_set                 rtrpc_trunk_localFirstFailOver_set
#define rtk_trunk_srcPortMap_get                         rtrpc_trunk_srcPortMap_get
#define rtk_trunk_srcPortMap_set                         rtrpc_trunk_srcPortMap_set
/*
 * Function Declaration
 */

/* Module Name : TRUNK */

extern int32
rtrpc_trunk_init(uint32 unit);

/* Module Name    : Trunk                    */
/* Sub-module Name: User configuration trunk */

extern int32
rtrpc_trunk_mode_get (uint32 unit, rtk_trunk_mode_t *pMode);

extern int32
rtrpc_trunk_mode_set(uint32 unit, rtk_trunk_mode_t mode);

extern int32
rtrpc_trunk_port_get(uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrunk_member_portmask);

extern int32
rtrpc_trunk_port_set(uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrunk_member_portmask);

extern int32
rtrpc_trunk_localPort_get (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);


extern int32
rtrpc_trunk_localPort_set (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);


extern int32
rtrpc_trunk_egrPort_get(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports);

extern int32
rtrpc_trunk_egrPort_set(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports);

extern int32
rtrpc_trunk_tunnelHashSrc_get(uint32 unit, rtk_trunk_tunnelHashSrc_t *pTunnelHashSrc);

extern int32
rtrpc_trunk_tunnelHashSrc_set(uint32 unit, rtk_trunk_tunnelHashSrc_t tunnelHashSrc);

extern int32
rtrpc_trunk_distributionAlgorithmBind_get(uint32 unit, rtk_trk_t trk_gid, uint32 *pAlgo_idx);

extern int32
rtrpc_trunk_distributionAlgorithmBind_set(uint32 unit, rtk_trk_t trk_gid, uint32 algo_idx);

extern int32
rtrpc_trunk_distributionAlgorithmTypeBind_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx);

extern int32
rtrpc_trunk_distributionAlgorithmTypeBind_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx);

extern int32
rtrpc_trunk_distributionAlgorithmParam_get(uint32 unit, uint32 algo_idx, uint32 *pAlgo_bitmask);

extern int32
rtrpc_trunk_distributionAlgorithmParam_set(uint32 unit, uint32 algo_idx, uint32 algo_bitmask);

extern int32
rtrpc_trunk_distributionAlgorithmTypeParam_get(uint32 unit, rtk_trunk_hashParamType_t type, uint32 algo_idx, uint32 *pAlgo_bitmask);

extern int32
rtrpc_trunk_distributionAlgorithmTypeParam_set(uint32 unit, rtk_trunk_hashParamType_t type, uint32 algo_idx, uint32 algo_bitmask);

extern int32
rtrpc_trunk_distributionAlgorithmShift_get(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift);

extern int32
rtrpc_trunk_distributionAlgorithmShift_set(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift);

extern int32
rtrpc_trunk_distributionAlgorithmShiftGbl_get(uint32 unit, rtk_trunk_distAlgoShift_t *pShift);

extern int32
rtrpc_trunk_distributionAlgorithmShiftGbl_set(uint32 unit, rtk_trunk_distAlgoShift_t *pShift);


extern int32
rtrpc_trunk_trafficSeparate_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t *pSeparateType);

extern int32
rtrpc_trunk_trafficSeparate_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType);

extern int32
rtrpc_trunk_trafficSeparateEnable_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t *pEnable);


extern int32
rtrpc_trunk_trafficSeparateEnable_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t enable);

extern int32
rtrpc_trunk_trafficSeparateDivision_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_trunk_trafficSeparateDivision_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_trunk_stkTrkPort_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

extern int32
rtrpc_trunk_stkTrkPort_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

extern int32
rtrpc_trunk_stkTrkHash_get(uint32 unit, rtk_trunk_stkTrkHash_t *pStkTrkHash);


extern int32
rtrpc_trunk_stkTrkHash_set(uint32 unit, rtk_trunk_stkTrkHash_t stkTrkHash);


extern int32
rtrpc_trunk_stkDistributionAlgorithmTypeBind_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx);

extern int32
rtrpc_trunk_stkDistributionAlgorithmTypeBind_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx);

extern int32
rtrpc_trunk_localFirst_get(uint32 unit, rtk_enable_t *pLocalFirst);

extern int32
rtrpc_trunk_localFirst_set(uint32 unit, rtk_enable_t localFirst);

extern int32
rtrpc_trunk_localFirstFailOver_get(uint32 unit, rtk_enable_t *pCongstAvoid, rtk_enable_t *pLinkFailAvoid);

extern int32
rtrpc_trunk_localFirstFailOver_set(uint32 unit, rtk_enable_t congstAvoid, rtk_enable_t linkFailAvoid);

extern int32
rtrpc_trunk_srcPortMap_get(uint32 unit,  rtk_dev_port_t devPort, uint32 *pIsTrkMbr, rtk_trk_t *pTrk_gid);

extern int32
rtrpc_trunk_srcPortMap_set(uint32 unit, rtk_dev_port_t devPort, uint32 isTrkMbr, rtk_trk_t trk_gid);

extern int32
rtrpc_trunkMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_TRUNK_H__ */
