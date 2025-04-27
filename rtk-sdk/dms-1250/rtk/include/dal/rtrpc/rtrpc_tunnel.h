/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_TUNNEL_H__
#define __RTRPC_TUNNEL_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>

#ifdef RTUSR
  #define rtk_tunnel_info_t_init                           rtrpc_tunnel_info_t_init
  #define rtk_tunnel_intf_t_init                           rtrpc_tunnel_intf_t_init
#endif //RTUSR

#define rtk_tunnel_init                                  rtrpc_tunnel_init
#define rtk_tunnel_info_get                              rtrpc_tunnel_info_get
#define rtk_tunnel_intf_create                           rtrpc_tunnel_intf_create
#define rtk_tunnel_intf_destroy                          rtrpc_tunnel_intf_destroy
#define rtk_tunnel_intf_destroyAll                       rtrpc_tunnel_intf_destroyAll
#define rtk_tunnel_intf_get                              rtrpc_tunnel_intf_get
#define rtk_tunnel_intf_set                              rtrpc_tunnel_intf_set
#define rtk_tunnel_intfPathId_get                        rtrpc_tunnel_intfPathId_get
#define rtk_tunnel_intfPathId_set                        rtrpc_tunnel_intfPathId_set
#define rtk_tunnel_intfPath_get                          rtrpc_tunnel_intfPath_get
#define rtk_tunnel_intfPath_set                          rtrpc_tunnel_intfPath_set
#define rtk_tunnel_intfStats_get                         rtrpc_tunnel_intfStats_get
#define rtk_tunnel_intfStats_reset                       rtrpc_tunnel_intfStats_reset
#define rtk_tunnel_qosProfile_get                        rtrpc_tunnel_qosProfile_get
#define rtk_tunnel_qosProfile_set                        rtrpc_tunnel_qosProfile_set
#define rtk_tunnel_globalCtrl_get                        rtrpc_tunnel_globalCtrl_get
#define rtk_tunnel_globalCtrl_set                        rtrpc_tunnel_globalCtrl_set
#define rtk_tunnel_intfCtrl_get                          rtrpc_tunnel_intfCtrl_get
#define rtk_tunnel_intfCtrl_set                          rtrpc_tunnel_intfCtrl_set
/*
 * Function Declaration
 */

extern int32
rtrpc_tunnel_init(uint32 unit);

/* Module Name    : Tunneling                */
/* Sub-module Name: Tunneling error handling */

extern int32
rtrpc_tunnel_info_t_init(rtk_tunnel_info_t *pInfo);

extern int32
rtrpc_tunnel_info_get(uint32 unit, rtk_tunnel_info_t *pInfo);

extern int32
rtrpc_tunnel_intf_t_init(rtk_tunnel_intf_t *pIntf);

extern int32
rtrpc_tunnel_intf_create(uint32 unit, rtk_tunnel_intf_t *pIntf);

extern int32
rtrpc_tunnel_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

extern int32
rtrpc_tunnel_intf_destroyAll(uint32 unit);

extern int32
rtrpc_tunnel_intf_get(uint32 unit, rtk_tunnel_intf_t *pIntf);

extern int32
rtrpc_tunnel_intf_set(uint32 unit, rtk_tunnel_intf_t *pIntf);

extern int32
rtrpc_tunnel_intfPathId_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t *pPathId);

extern int32
rtrpc_tunnel_intfPathId_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t pathId);

extern int32
rtrpc_tunnel_intfPath_get(uint32 unit, rtk_intf_id_t intfId, uint32 *pNhDmacIdx, uint32 *pL3EgrIntfIdx);

extern int32
rtrpc_tunnel_intfPath_set(uint32 unit, rtk_intf_id_t intfId, uint32 nhDmacIdx, uint32 l3EgrIntfIdx);

extern int32
rtrpc_tunnel_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_tunnel_intf_stats_t *pStats);

extern int32
rtrpc_tunnel_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

extern int32
rtrpc_tunnel_qosProfile_get(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t *pProfile);

extern int32
rtrpc_tunnel_qosProfile_set(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t profile);

extern int32
rtrpc_tunnel_globalCtrl_get(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 *pArg);

extern int32
rtrpc_tunnel_globalCtrl_set(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 arg);

extern int32
rtrpc_tunnelMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_TUNNEL_H__ */

