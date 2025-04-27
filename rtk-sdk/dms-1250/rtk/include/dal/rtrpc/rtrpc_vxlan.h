/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_VXLAN_H__
#define __RTRPC_VXLAN_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_vxlan_init                                   rtrpc_vxlan_init
#define rtk_vxlan_vni_add                                rtrpc_vxlan_vni_add
#define rtk_vxlan_vni_del                                rtrpc_vxlan_vni_del
#define rtk_vxlan_vni_delAll                             rtrpc_vxlan_vni_delAll
#define rtk_vxlan_vni_get                                rtrpc_vxlan_vni_get
#define rtk_vxlan_vni_set                                rtrpc_vxlan_vni_set
#define rtk_vxlan_vni_getNext                            rtrpc_vxlan_vni_getNext
#define rtk_vxlan_globalCtrl_get                         rtrpc_vxlan_globalCtrl_get
#define rtk_vxlan_globalCtrl_set                         rtrpc_vxlan_globalCtrl_set
/*
 * Function Declaration
 */

extern int32
rtrpc_vxlan_init(uint32 unit);

extern int32
rtrpc_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

extern int32
rtrpc_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

extern int32
rtrpc_vxlan_vni_delAll(uint32 unit);

extern int32
rtrpc_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

extern int32
rtrpc_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

extern int32
rtrpc_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry);

extern int32
rtrpc_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg);

extern int32
rtrpc_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg);

extern int32
rtrpc_vxlanMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_VXLAN_H__ */

