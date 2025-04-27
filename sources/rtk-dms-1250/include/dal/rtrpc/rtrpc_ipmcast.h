/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_IPMCAST_H__
#define __RTRPC_IPMCAST_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>

#ifdef RTUSR
  #define rtk_ipmc_addr_t_init                             rtrpc_ipmc_addr_t_init
#endif //#ifdef RTUSR
#define rtk_ipmc_init                                    rtrpc_ipmc_init
#define rtk_ipmc_addr_add                                rtrpc_ipmc_addr_add
#define rtk_ipmc_addr_find                               rtrpc_ipmc_addr_find
#define rtk_ipmc_addr_del                                rtrpc_ipmc_addr_del
#define rtk_ipmc_addr_delAll                             rtrpc_ipmc_addr_delAll
#define rtk_ipmc_nextValidAddr_get                       rtrpc_ipmc_nextValidAddr_get
#define rtk_ipmc_statMont_create                         rtrpc_ipmc_statMont_create
#define rtk_ipmc_statMont_destroy                        rtrpc_ipmc_statMont_destroy
#define rtk_ipmc_statCntr_reset                          rtrpc_ipmc_statCntr_reset
#define rtk_ipmc_statCntr_get                            rtrpc_ipmc_statCntr_get
#define rtk_ipmc_globalCtrl_set                          rtrpc_ipmc_globalCtrl_set
#define rtk_ipmc_globalCtrl_get                          rtrpc_ipmc_globalCtrl_get
/*
 * Function Declaration
 */

/* Module Name : L3 Multicast */

extern int32
rtrpc_ipmc_init(uint32 unit);

extern int32
rtrpc_ipmc_addr_t_init(rtk_ipmc_addr_t *pAddr);

extern int32
rtrpc_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pAddr);

extern int32
rtrpc_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t *pAddr);

extern int32
rtrpc_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pAddr);

extern int32
rtrpc_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flag);

extern int32
rtrpc_ipmc_nextValidAddr_get(uint32 unit, uint32 flags, int32 *pBase, rtk_ipmc_addr_t *pAddr);

extern int32
rtrpc_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont);

extern int32
rtrpc_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont);

extern int32
rtrpc_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pKey);

extern int32
rtrpc_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pKey, rtk_ipmc_statCntr_t *pCntr);

extern int32
rtrpc_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg);

extern int32
rtrpc_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg);

extern int32
rtrpc_ipmcMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_IPMCAST_H__ */

