/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_MPLS_H__
#define __RTRPC_MPLS_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_mpls_init                                    rtrpc_mpls_init
#define rtk_mpls_ttlInherit_get                          rtrpc_mpls_ttlInherit_get
#define rtk_mpls_ttlInherit_set                          rtrpc_mpls_ttlInherit_set
#define rtk_mpls_enable_get                              rtrpc_mpls_enable_get
#define rtk_mpls_enable_set                              rtrpc_mpls_enable_set
#define rtk_mpls_trapTarget_get                          rtrpc_mpls_trapTarget_get
#define rtk_mpls_trapTarget_set                          rtrpc_mpls_trapTarget_set
#define rtk_mpls_exceptionCtrl_get                       rtrpc_mpls_exceptionCtrl_get
#define rtk_mpls_exceptionCtrl_set                       rtrpc_mpls_exceptionCtrl_set
#define rtk_mpls_nextHop_create                          rtrpc_mpls_nextHop_create
#define rtk_mpls_nextHop_destroy                         rtrpc_mpls_nextHop_destroy
#define rtk_mpls_nextHop_get                             rtrpc_mpls_nextHop_get
#define rtk_mpls_nextHop_set                             rtrpc_mpls_nextHop_set
#define rtk_mpls_encap_create                            rtrpc_mpls_encap_create
#define rtk_mpls_encap_destroy                           rtrpc_mpls_encap_destroy
#define rtk_mpls_encap_get                               rtrpc_mpls_encap_get
#define rtk_mpls_encap_set                               rtrpc_mpls_encap_set
#define rtk_mpls_encapId_find                            rtrpc_mpls_encapId_find
#define rtk_mpls_hashAlgo_get                            rtrpc_mpls_hashAlgo_get
#define rtk_mpls_hashAlgo_set                            rtrpc_mpls_hashAlgo_set
#define rtk_mpls_decap_create                            rtrpc_mpls_decap_create
#define rtk_mpls_decap_destroy                           rtrpc_mpls_decap_destroy
#define rtk_mpls_decap_get                               rtrpc_mpls_decap_get
#define rtk_mpls_decap_set                               rtrpc_mpls_decap_set
#define rtk_mpls_decapId_find                            rtrpc_mpls_decapId_find
#define rtk_mpls_egrTcMap_get                            rtrpc_mpls_egrTcMap_get
#define rtk_mpls_egrTcMap_set                            rtrpc_mpls_egrTcMap_set
#define rtk_mpls_nextHop_create_id                       rtrpc_mpls_nextHop_create_id
#define rtk_mpls_encap_create_id                         rtrpc_mpls_encap_create_id
/*
 * Function Declaration
 */

/* Module Name : MPLS */

extern int32
rtrpc_mpls_init(uint32 unit);

extern int32
rtrpc_mpls_ttlInherit_get(uint32 unit, rtk_mpls_ttlInherit_t *pInherit);

extern int32
rtrpc_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit);

extern int32
rtrpc_mpls_encap_get(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo);

extern int32
rtrpc_mpls_encap_set(uint32 unit, rtk_mpls_entryId_t encapId, rtk_mpls_encap_t *pInfo);

extern int32
rtrpc_mpls_enable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_mpls_enable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_mpls_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_mpls_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

extern int32
rtrpc_mpls_exceptionCtrl_get(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t *pAction);

extern int32
rtrpc_mpls_exceptionCtrl_set(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t action);

/* next hop */
extern int32
rtrpc_mpls_nextHop_create(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t *pPathId);

extern int32
rtrpc_mpls_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId);

extern int32
rtrpc_mpls_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

extern int32
rtrpc_mpls_nextHop_set(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh);

/* encapsulation */
extern int32
rtrpc_mpls_encap_create(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

extern int32
rtrpc_mpls_encap_destroy(uint32 unit, rtk_mpls_entryId_t encapId);

extern int32
rtrpc_mpls_encapId_find(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId);

/* decapsulation */
extern int32
rtrpc_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo);

extern int32
rtrpc_mpls_hashAlgo_set(uint32 unit, uint8 hashAlgo);

extern int32
rtrpc_mpls_decap_create(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

extern int32
rtrpc_mpls_decap_destroy(uint32 unit, rtk_mpls_entryId_t decapId);

extern int32
rtrpc_mpls_decap_get(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

extern int32
rtrpc_mpls_decap_set(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap);

extern int32
rtrpc_mpls_decapId_find(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId);

extern int32
rtrpc_mpls_egrTcMap_get(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 *pTc);

extern int32
rtrpc_mpls_egrTcMap_set(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 tc);

extern int32
rtrpc_mplsMapper_init(dal_mapper_t *pMapper);


extern int32
rtrpc_mpls_nextHop_create_id(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t pathId);

extern int32
rtrpc_mpls_encap_create_id(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t encapId);

#endif /* __RTRPC_MPLS_H__ */
