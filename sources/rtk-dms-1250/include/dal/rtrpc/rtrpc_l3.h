/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_L3_H__
#define __RTRPC_L3_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>

#ifdef RTUSR
#define rtk_l3_intf_t_init                               rtrpc_l3_intf_t_init
#define rtk_l3_nextHop_t_init                            rtrpc_l3_nextHop_t_init
#define rtk_l3_key_t_init                                rtrpc_l3_key_t_init
#define rtk_l3_host_t_init                               rtrpc_l3_host_t_init
#define rtk_l3_route_t_init                              rtrpc_l3_route_t_init
#endif //RTUSR

#define rtk_l3_init                                      rtrpc_l3_init
#define rtk_l3_routeEntry_get                            rtrpc_l3_routeEntry_get
#define rtk_l3_routeEntry_set                            rtrpc_l3_routeEntry_set
#define rtk_l3_routeSwitchMacAddr_get                    rtrpc_l3_routeSwitchMacAddr_get
#define rtk_l3_routeSwitchMacAddr_set                    rtrpc_l3_routeSwitchMacAddr_set
#define rtk_l3_info_get                                  rtrpc_l3_info_get
#define rtk_l3_routerMacEntry_get                        rtrpc_l3_routerMacEntry_get
#define rtk_l3_routerMacEntry_set                        rtrpc_l3_routerMacEntry_set
#define rtk_l3_intf_create                               rtrpc_l3_intf_create
#define rtk_l3_intf_destroy                              rtrpc_l3_intf_destroy
#define rtk_l3_intf_destroyAll                           rtrpc_l3_intf_destroyAll
#define rtk_l3_intf_get                                  rtrpc_l3_intf_get
#define rtk_l3_intf_set                                  rtrpc_l3_intf_set
#define rtk_l3_intfStats_get                             rtrpc_l3_intfStats_get
#define rtk_l3_intfStats_reset                           rtrpc_l3_intfStats_reset
#define rtk_l3_vrrp_add                                  rtrpc_l3_vrrp_add
#define rtk_l3_vrrp_del                                  rtrpc_l3_vrrp_del
#define rtk_l3_vrrp_delAll                               rtrpc_l3_vrrp_delAll
#define rtk_l3_vrrp_get                                  rtrpc_l3_vrrp_get
#define rtk_l3_nextHop_create                            rtrpc_l3_nextHop_create
#define rtk_l3_nextHop_destroy                           rtrpc_l3_nextHop_destroy
#define rtk_l3_nextHop_get                               rtrpc_l3_nextHop_get
#define rtk_l3_nextHopPath_find                          rtrpc_l3_nextHopPath_find
#define rtk_l3_ecmp_create                               rtrpc_l3_ecmp_create
#define rtk_l3_ecmp_destroy                              rtrpc_l3_ecmp_destroy
#define rtk_l3_ecmp_get                                  rtrpc_l3_ecmp_get
#define rtk_l3_ecmp_add                                  rtrpc_l3_ecmp_add
#define rtk_l3_ecmp_del                                  rtrpc_l3_ecmp_del
#define rtk_l3_ecmp_find                                 rtrpc_l3_ecmp_find
#define rtk_l3_ecmpHashTbl_get                           rtrpc_l3_ecmpHashTbl_get
#define rtk_l3_ecmpHashTbl_set                           rtrpc_l3_ecmpHashTbl_set
#define rtk_l3_ecmpCtrl_get                              rtrpc_l3_ecmpCtrl_get
#define rtk_l3_ecmpCtrl_set                              rtrpc_l3_ecmpCtrl_set
#define rtk_l3_host_add                                  rtrpc_l3_host_add
#define rtk_l3_host_del                                  rtrpc_l3_host_del
#define rtk_l3_host_del_byNetwork                        rtrpc_l3_host_del_byNetwork
#define rtk_l3_host_del_byIntfId                         rtrpc_l3_host_del_byIntfId
#define rtk_l3_host_delAll                               rtrpc_l3_host_delAll
#define rtk_l3_host_find                                 rtrpc_l3_host_find
#define rtk_l3_hostConflict_get                          rtrpc_l3_hostConflict_get
#define rtk_l3_host_age                                  rtrpc_l3_host_age
#define rtk_l3_host_getNext                              rtrpc_l3_host_getNext
#define rtk_l3_route_add                                 rtrpc_l3_route_add
#define rtk_l3_route_del                                 rtrpc_l3_route_del
#define rtk_l3_route_get                                 rtrpc_l3_route_get
#define rtk_l3_route_del_byIntfId                        rtrpc_l3_route_del_byIntfId
#define rtk_l3_route_delAll                              rtrpc_l3_route_delAll
#define rtk_l3_route_age                                 rtrpc_l3_route_age
#define rtk_l3_route_getNext                             rtrpc_l3_route_getNext
#define rtk_l3_globalCtrl_get                            rtrpc_l3_globalCtrl_get
#define rtk_l3_globalCtrl_set                            rtrpc_l3_globalCtrl_set
#define rtk_l3_intfCtrl_get                              rtrpc_l3_intfCtrl_get
#define rtk_l3_intfCtrl_set                              rtrpc_l3_intfCtrl_set
#define rtk_l3_portCtrl_get                              rtrpc_l3_portCtrl_get
#define rtk_l3_portCtrl_set                              rtrpc_l3_portCtrl_set
/*
 * Function Declaration
 */

extern int32
rtrpc_l3_init(uint32 unit);

/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

extern int32
rtrpc_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

extern int32
rtrpc_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

extern int32
rtrpc_l3_routeSwitchMacAddr_get(uint32 unit, uint32 index, rtk_mac_t *pMac);

extern int32
rtrpc_l3_routeSwitchMacAddr_set(uint32 unit, uint32 index, rtk_mac_t *pMac);

extern int32
rtrpc_l3_info_t_init(rtk_l3_info_t *pInfo);

extern int32
rtrpc_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo);

extern int32
rtrpc_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

extern int32
rtrpc_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

extern int32
rtrpc_l3_intf_t_init(rtk_l3_intf_t *pIntf);

extern int32
rtrpc_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf);

extern int32
rtrpc_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

extern int32
rtrpc_l3_intf_destroyAll(uint32 unit);

extern int32
rtrpc_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

extern int32
rtrpc_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

extern int32
rtrpc_l3_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intf_stats_t *pStats);

extern int32
rtrpc_l3_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

extern int32
rtrpc_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

extern int32
rtrpc_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

extern int32
rtrpc_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid);

extern int32
rtrpc_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount);

extern int32
rtrpc_l3_nextHop_t_init(rtk_l3_nextHop_t *pNextHop);

extern int32
rtrpc_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId);

extern int32
rtrpc_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t nhId);

extern int32
rtrpc_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t nhId, rtk_l3_nextHop_t *pNextHop);

extern int32
rtrpc_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId);

extern int32
rtrpc_l3_ecmp_create(uint32 unit, rtk_l3_flag_t flags, uint32 nhIdCnt, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId);

extern int32
rtrpc_l3_ecmp_destroy(uint32 unit, rtk_l3_pathId_t ecmpId);

extern int32
rtrpc_l3_ecmp_get(uint32 unit, rtk_l3_pathId_t ecmpId, uint32 nhIdArraySize, rtk_l3_pathId_t *pNhIdArray, uint32 *pNhIdCount);

extern int32
rtrpc_l3_ecmp_add(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

extern int32
rtrpc_l3_ecmp_del(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

extern int32
rtrpc_l3_ecmp_find(uint32 unit, uint32 nhIdCount, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId);

extern int32
rtrpc_l3_key_t_init(rtk_l3_key_t *pKey);

extern int32
rtrpc_l3_host_t_init(rtk_l3_host_t *pHost);

extern int32
rtrpc_l3_host_add(uint32 unit, rtk_l3_host_t *pHost);

extern int32
rtrpc_l3_host_del(uint32 unit, rtk_l3_host_t *pHost);

extern int32
rtrpc_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags);

extern int32
rtrpc_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags);

extern int32
rtrpc_l3_host_find(uint32 unit, rtk_l3_host_t *pHost);

extern int32
rtrpc_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount);

extern int32
rtrpc_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie);

extern int32
rtrpc_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost);

extern int32
rtrpc_l3_route_t_init(rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId);

extern int32
rtrpc_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags);

extern int32
rtrpc_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie);

extern int32
rtrpc_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute);

extern int32
rtrpc_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg);

extern int32
rtrpc_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg);

extern int32
rtrpc_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg);

extern int32
rtrpc_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg);

extern int32
rtrpc_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg);

extern int32
rtrpc_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg);

extern int32
rtrpc_l3Mapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_L3_H__ */

