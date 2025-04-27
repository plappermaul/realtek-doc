/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_SEC_H__
#define __RTRPC_SEC_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_sec_portAttackPrevent_get                    rtrpc_sec_portAttackPrevent_get
#define rtk_sec_portAttackPrevent_set                    rtrpc_sec_portAttackPrevent_set
#define rtk_sec_portAttackPreventEnable_get              rtrpc_sec_portAttackPreventEnable_get
#define rtk_sec_portAttackPreventEnable_set              rtrpc_sec_portAttackPreventEnable_set
#define rtk_sec_attackPreventAction_get                  rtrpc_sec_attackPreventAction_get
#define rtk_sec_attackPreventAction_set                  rtrpc_sec_attackPreventAction_set
#define rtk_sec_minIPv6FragLen_get                       rtrpc_sec_minIPv6FragLen_get
#define rtk_sec_minIPv6FragLen_set                       rtrpc_sec_minIPv6FragLen_set
#define rtk_sec_maxPingLen_get                           rtrpc_sec_maxPingLen_get
#define rtk_sec_maxPingLen_set                           rtrpc_sec_maxPingLen_set
#define rtk_sec_minTCPHdrLen_get                         rtrpc_sec_minTCPHdrLen_get
#define rtk_sec_minTCPHdrLen_set                         rtrpc_sec_minTCPHdrLen_set
#define rtk_sec_smurfNetmaskLen_get                      rtrpc_sec_smurfNetmaskLen_get
#define rtk_sec_smurfNetmaskLen_set                      rtrpc_sec_smurfNetmaskLen_set
#define rtk_sec_trapTarget_get                           rtrpc_sec_trapTarget_get
#define rtk_sec_trapTarget_set                           rtrpc_sec_trapTarget_set
#define rtk_sec_ipMacBindAction_get                      rtrpc_sec_ipMacBindAction_get
#define rtk_sec_ipMacBindAction_set                      rtrpc_sec_ipMacBindAction_set
#define rtk_sec_portIpMacBindEnable_get                  rtrpc_sec_portIpMacBindEnable_get
#define rtk_sec_portIpMacBindEnable_set                  rtrpc_sec_portIpMacBindEnable_set
#define rtk_sec_ipMacBindEntry_add                       rtrpc_sec_ipMacBindEntry_add
#define rtk_sec_ipMacBindEntry_del                       rtrpc_sec_ipMacBindEntry_del
#define rtk_sec_ipMacBindEntry_getNext                   rtrpc_sec_ipMacBindEntry_getNext
#define rtk_sec_attackPreventHit_get                     rtrpc_sec_attackPreventHit_get
/*
 * Function Declaration
 */

/* Module Name : Security */

extern int32
rtrpc_sec_init(uint32 unit);

/* Module Name    : Security          */
/* Sub-module Name: Attack prevention */

extern int32
rtrpc_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction);

extern int32
rtrpc_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

extern int32
rtrpc_sec_portAttackPreventEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_sec_portAttackPreventEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_sec_attackPreventAction_get(
    uint32                      unit,
    rtk_sec_attackType_t        attack_type,
    rtk_action_t                *pAction);

extern int32
rtrpc_sec_attackPreventAction_set(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action);

extern int32
rtrpc_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength);

extern int32
rtrpc_sec_minIPv6FragLen_set(uint32 unit, uint32 length);

extern int32
rtrpc_sec_maxPingLen_get(uint32 unit, uint32 *pLength);

extern int32
rtrpc_sec_maxPingLen_set(uint32 unit, uint32 length);

extern int32
rtrpc_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength);

extern int32
rtrpc_sec_minTCPHdrLen_set(uint32 unit, uint32 length);

extern int32
rtrpc_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength);

extern int32
rtrpc_sec_smurfNetmaskLen_set(uint32 unit, uint32 length);

extern int32
rtrpc_sec_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_sec_trapTarget_set(uint32 unit, rtk_trapTarget_t target);


extern int32
rtrpc_sec_ipMacBindAction_get(uint32 unit, rtk_action_t *pLumisAct, rtk_action_t *pMatchAct, rtk_action_t *pMismatchAct);

extern int32
rtrpc_sec_ipMacBindAction_set(uint32 unit, rtk_action_t lumisAct, rtk_action_t matchAct, rtk_action_t mismatchAct);

extern int32
rtrpc_sec_portIpMacBindEnable_get(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_sec_portIpMacBindEnable_set(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t enable);

extern int32
rtrpc_sec_ipMacBindEntry_add(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

extern int32
rtrpc_sec_ipMacBindEntry_del(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry);

extern int32
rtrpc_sec_ipMacBindEntry_getNext(uint32 unit, int32 *pBase, rtk_sec_ipMacBindEntry_t *pEntry);

extern int32
rtrpc_sec_attackPreventHit_get(uint32 unit,
    rtk_sec_attackType_t attack_type, uint32 *pHit);

extern int32
rtrpc_secMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_SEC_H__ */


