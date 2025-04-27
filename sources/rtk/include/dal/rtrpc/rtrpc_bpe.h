/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_BPE_H__
#define __RTRPC_BPE_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_bpe_init                                     rtrpc_bpe_init
#define rtk_bpe_portFwdMode_get                          rtrpc_bpe_portFwdMode_get
#define rtk_bpe_portFwdMode_set                          rtrpc_bpe_portFwdMode_set
#define rtk_bpe_portEcidNameSpaceGroupId_get             rtrpc_bpe_portEcidNameSpaceGroupId_get
#define rtk_bpe_portEcidNameSpaceGroupId_set             rtrpc_bpe_portEcidNameSpaceGroupId_set
#define rtk_bpe_portPcid_get                             rtrpc_bpe_portPcid_get
#define rtk_bpe_portPcid_set                             rtrpc_bpe_portPcid_set
#define rtk_bpe_portPcidAct_get                          rtrpc_bpe_portPcidAct_get
#define rtk_bpe_portPcidAct_set                          rtrpc_bpe_portPcidAct_set
#define rtk_bpe_portEgrTagSts_get                        rtrpc_bpe_portEgrTagSts_get
#define rtk_bpe_portEgrTagSts_set                        rtrpc_bpe_portEgrTagSts_set
#define rtk_bpe_portEgrVlanTagSts_get                    rtrpc_bpe_portEgrVlanTagSts_get
#define rtk_bpe_portEgrVlanTagSts_set                    rtrpc_bpe_portEgrVlanTagSts_set
#define rtk_bpe_pvidEntry_add                            rtrpc_bpe_pvidEntry_add
#define rtk_bpe_pvidEntry_del                            rtrpc_bpe_pvidEntry_del
#define rtk_bpe_pvidEntry_get                            rtrpc_bpe_pvidEntry_get
#define rtk_bpe_pvidEntryNextValid_get                   rtrpc_bpe_pvidEntryNextValid_get
#define rtk_bpe_fwdEntry_add                             rtrpc_bpe_fwdEntry_add
#define rtk_bpe_fwdEntry_del                             rtrpc_bpe_fwdEntry_del
#define rtk_bpe_fwdEntry_get                             rtrpc_bpe_fwdEntry_get
#define rtk_bpe_fwdEntryNextValid_get                    rtrpc_bpe_fwdEntryNextValid_get
#define rtk_bpe_globalCtrl_get                           rtrpc_bpe_globalCtrl_get
#define rtk_bpe_globalCtrl_set                           rtrpc_bpe_globalCtrl_set
#define rtk_bpe_portCtrl_get                             rtrpc_bpe_portCtrl_get
#define rtk_bpe_portCtrl_set                             rtrpc_bpe_portCtrl_set
#define rtk_bpe_priRemarking_get                         rtrpc_bpe_priRemarking_get
#define rtk_bpe_priRemarking_set                         rtrpc_bpe_priRemarking_set
/*
 * Function Declaration
 */

extern int32
rtrpc_bpe_init(uint32 unit);

/* Module Name    : BPE (Bridge Port Extension) */
/* Sub-module Name: BPE (Bridge Port Extension) */

extern int32
rtrpc_bpe_portFwdMode_get(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t *pMode);

extern int32
rtrpc_bpe_portFwdMode_set(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t mode);

extern int32
rtrpc_bpe_portEcidNameSpaceGroupId_get(uint32 unit, rtk_port_t port, uint32 *pGroupId);

extern int32
rtrpc_bpe_portEcidNameSpaceGroupId_set(uint32 unit, rtk_port_t port, uint32 groupId);

extern int32
rtrpc_bpe_portPcid_get(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t *pPcid);

extern int32
rtrpc_bpe_portPcid_set(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t pcid);

extern int32
rtrpc_bpe_portPcidAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

extern int32
rtrpc_bpe_portPcidAct_set(uint32 unit, rtk_port_t port, rtk_action_t action);

extern int32
rtrpc_bpe_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_bpe_tagSts_t *pStatus);

extern int32
rtrpc_bpe_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_bpe_tagSts_t status);

extern int32
rtrpc_bpe_portEgrVlanTagSts_get(uint32 unit, rtk_port_t port, rtk_bpe_vlanTagSts_t *pVlan_status);

extern int32
rtrpc_bpe_portEgrVlanTagSts_set(uint32 unit, rtk_port_t port, rtk_bpe_vlanTagSts_t vlan_status);


extern int32
rtrpc_bpe_pvidEntry_add(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

extern int32
rtrpc_bpe_pvidEntry_del(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

extern int32
rtrpc_bpe_pvidEntry_get(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

extern int32
rtrpc_bpe_pvidEntry_set(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

extern int32
rtrpc_bpe_pvidEntryNextValid_get(uint32 unit, int32 *pScan_idx, rtk_bpe_pvidEntry_t *pEntry);

extern int32
rtrpc_bpe_fwdEntry_add(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

extern int32
rtrpc_bpe_fwdEntry_del(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

extern int32
rtrpc_bpe_fwdEntry_get(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

extern int32
rtrpc_bpe_fwdEntry_set(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

extern int32
rtrpc_bpe_fwdEntryNextValid_get(uint32 unit, int32 *pScan_idx, rtk_bpe_fwdEntry_t *pEntry);

extern int32
rtrpc_bpe_globalCtrl_get(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 *pArg);

extern int32
rtrpc_bpe_globalCtrl_set(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 arg);

extern int32
rtrpc_bpe_portCtrl_get(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 *pArg);

extern int32
rtrpc_bpe_portCtrl_set(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 arg);

extern int32
rtrpc_bpe_priRemarking_get(uint32 unit, rtk_bpe_priRmkSrc_t src, rtk_bpe_priRmkVal_t val, rtk_pri_t *pPri);

extern int32
rtrpc_bpe_priRemarking_set(uint32 unit, rtk_bpe_priRmkSrc_t src, rtk_bpe_priRmkVal_t val, rtk_pri_t pri);

int32
rtrpc_bpeMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_BPE_H__ */

