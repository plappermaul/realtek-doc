/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_MCAST_H__
#define __RTRPC_MCAST_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_mcast_init                                   rtrpc_mcast_init
#define rtk_mcast_group_create                           rtrpc_mcast_group_create
#define rtk_mcast_group_destroy                          rtrpc_mcast_group_destroy
#define rtk_mcast_group_getNext                          rtrpc_mcast_group_getNext
#define rtk_mcast_egrIf_get                              rtrpc_mcast_egrIf_get
#define rtk_mcast_egrIf_add                              rtrpc_mcast_egrIf_add
#define rtk_mcast_egrIf_del                              rtrpc_mcast_egrIf_del
#define rtk_mcast_egrIf_delAll                           rtrpc_mcast_egrIf_delAll
/*
 * Function Declaration
 */

/* Module Name : Multicast Group */

extern int32
rtrpc_mcast_init(uint32 unit);

extern int32
rtrpc_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup);

extern int32
rtrpc_mcast_group_destroy(uint32 unit, rtk_mcast_group_t group);

extern int32
rtrpc_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup);

extern int32
rtrpc_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t group, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count);

extern int32
rtrpc_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);

extern int32
rtrpc_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf);

extern int32
rtrpc_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t group);

extern int32
rtrpc_mcastMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_MCAST_H__ */

