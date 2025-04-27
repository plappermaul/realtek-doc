/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_ACL_H__
#define __RTRPC_ACL_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_acl_ruleEntryFieldSize_get                   rtrpc_acl_ruleEntryFieldSize_get
#define rtk_acl_ruleEntrySize_get                        rtrpc_acl_ruleEntrySize_get
#define rtk_acl_ruleEntry_read                           rtrpc_acl_ruleEntry_read
#define rtk_acl_ruleEntryField_write                     rtrpc_acl_ruleEntryField_write
#define rtk_acl_portLookupEnable_get                     rtrpc_acl_portLookupEnable_get
#define rtk_acl_portLookupEnable_set                     rtrpc_acl_portLookupEnable_set
#define rtk_acl_lookupMissAct_get                        rtrpc_acl_lookupMissAct_get
#define rtk_acl_lookupMissAct_set                        rtrpc_acl_lookupMissAct_set
#define rtk_acl_rangeCheckFieldSelector_get              rtrpc_acl_rangeCheckFieldSelector_get
#define rtk_acl_rangeCheckFieldSelector_set              rtrpc_acl_rangeCheckFieldSelector_set
#define rtk_acl_ruleValidate_get                         rtrpc_acl_ruleValidate_get
#define rtk_acl_ruleValidate_set                         rtrpc_acl_ruleValidate_set
#define rtk_acl_ruleEntry_write                          rtrpc_acl_ruleEntry_write
#define rtk_acl_ruleEntryField_get                       rtrpc_acl_ruleEntryField_get
#define rtk_acl_ruleEntryField_set                       rtrpc_acl_ruleEntryField_set
#define rtk_acl_ruleEntryField_read                      rtrpc_acl_ruleEntryField_read
#define rtk_acl_ruleEntryField_check                     rtrpc_acl_ruleEntryField_check
#define rtk_acl_ruleOperation_get                        rtrpc_acl_ruleOperation_get
#define rtk_acl_ruleOperation_set                        rtrpc_acl_ruleOperation_set
#define rtk_acl_ruleAction_get                           rtrpc_acl_ruleAction_get
#define rtk_acl_ruleAction_set                           rtrpc_acl_ruleAction_set
#define rtk_acl_blockPwrEnable_get                       rtrpc_acl_blockPwrEnable_get
#define rtk_acl_blockPwrEnable_set                       rtrpc_acl_blockPwrEnable_set
#define rtk_acl_blockGroupEnable_get                     rtrpc_acl_blockGroupEnable_get
#define rtk_acl_blockGroupEnable_set                     rtrpc_acl_blockGroupEnable_set
#define rtk_acl_statPktCnt_get                           rtrpc_acl_statPktCnt_get
#define rtk_acl_statPktCnt_clear                         rtrpc_acl_statPktCnt_clear
#define rtk_acl_statByteCnt_get                          rtrpc_acl_statByteCnt_get
#define rtk_acl_statByteCnt_clear                        rtrpc_acl_statByteCnt_clear
#define rtk_acl_stat_clearAll                            rtrpc_acl_stat_clearAll
#define rtk_acl_rangeCheckL4Port_get                     rtrpc_acl_rangeCheckL4Port_get
#define rtk_acl_rangeCheckL4Port_set                     rtrpc_acl_rangeCheckL4Port_set
#define rtk_acl_rangeCheckVid_get                        rtrpc_acl_rangeCheckVid_get
#define rtk_acl_rangeCheckVid_set                        rtrpc_acl_rangeCheckVid_set
#define rtk_acl_rangeCheckSrcPort_get                    rtrpc_acl_rangeCheckSrcPort_get
#define rtk_acl_rangeCheckSrcPort_set                    rtrpc_acl_rangeCheckSrcPort_set
#define rtk_acl_rangeCheckPacketLen_get                  rtrpc_acl_rangeCheckPacketLen_get
#define rtk_acl_rangeCheckPacketLen_set                  rtrpc_acl_rangeCheckPacketLen_set
#define rtk_acl_meterBurstSize_get                       rtrpc_acl_meterBurstSize_get
#define rtk_acl_meterBurstSize_set                       rtrpc_acl_meterBurstSize_set
#define rtk_acl_meterMode_get                            rtrpc_acl_meterMode_get
#define rtk_acl_meterMode_set                            rtrpc_acl_meterMode_set
#define rtk_acl_loopBackEnable_get                       rtrpc_acl_loopBackEnable_get
#define rtk_acl_loopBackEnable_set                       rtrpc_acl_loopBackEnable_set
#define rtk_acl_limitLoopbackTimes_get                   rtrpc_acl_limitLoopbackTimes_get
#define rtk_acl_limitLoopbackTimes_set                   rtrpc_acl_limitLoopbackTimes_set
#define rtk_acl_portPhaseLookupEnable_get                rtrpc_acl_portPhaseLookupEnable_get
#define rtk_acl_portPhaseLookupEnable_set                rtrpc_acl_portPhaseLookupEnable_set
#define rtk_acl_partition_get                            rtrpc_acl_partition_get
#define rtk_acl_partition_set                            rtrpc_acl_partition_set
#define rtk_acl_templateFieldIntentVlanTag_get           rtrpc_acl_templateFieldIntentVlanTag_get
#define rtk_acl_templateFieldIntentVlanTag_set           rtrpc_acl_templateFieldIntentVlanTag_set
#define rtk_acl_blockResultMode_get                      rtrpc_acl_blockResultMode_get
#define rtk_acl_blockResultMode_set                      rtrpc_acl_blockResultMode_set
#define rtk_acl_rangeCheckDstPort_get                    rtrpc_acl_rangeCheckDstPort_get
#define rtk_acl_rangeCheckDstPort_set                    rtrpc_acl_rangeCheckDstPort_set
#define rtk_acl_templateSelector_get                     rtrpc_acl_templateSelector_get
#define rtk_acl_templateSelector_set                     rtrpc_acl_templateSelector_set
#define rtk_acl_statCnt_get                              rtrpc_acl_statCnt_get
#define rtk_acl_statCnt_clear                            rtrpc_acl_statCnt_clear
#define rtk_acl_ruleHitIndication_get                    rtrpc_acl_ruleHitIndication_get
#define rtk_acl_ruleHitIndicationMask_get                rtrpc_acl_ruleHitIndicationMask_get
#define rtk_acl_rule_del                                 rtrpc_acl_rule_del
#define rtk_acl_rule_move                                rtrpc_acl_rule_move
#define rtk_acl_ruleEntryField_validate                  rtrpc_acl_ruleEntryField_validate
#define rtk_acl_fieldUsr2Template_get                    rtrpc_acl_fieldUsr2Template_get
#define rtk_acl_templateId_get                           rtrpc_acl_templateId_get
/*
 * Function Declaration
 */

/* Module Name     : ACL */

extern int32
rtrpc_acl_init(uint32 unit);

extern int32
rtrpc_acl_portLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_acl_portLookupEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_acl_lookupMissAct_get(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t *pLmAct);

extern int32
rtrpc_acl_lookupMissAct_set(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t lmAct);
extern int32
rtrpc_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size);

extern int32
rtrpc_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size);

extern int32
rtrpc_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid);

extern int32
rtrpc_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid);

extern int32
rtrpc_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

extern int32
rtrpc_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

extern int32
rtrpc_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

extern int32
rtrpc_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

extern int32
rtrpc_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

extern int32
rtrpc_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

extern int32
rtrpc_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

extern int32
rtrpc_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

extern int32
rtrpc_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);

extern int32
rtrpc_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);

extern int32
rtrpc_acl_blockGroupEnable_get(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               *pEnable);

extern int32
rtrpc_acl_blockGroupEnable_set(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               enable);

extern int32
rtrpc_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt);

extern int32
rtrpc_acl_statPktCnt_clear(uint32 unit, uint32 log_id);

extern int32
rtrpc_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt);

extern int32
rtrpc_acl_statByteCnt_clear(uint32 unit, uint32 log_id);

extern int32
rtrpc_acl_stat_clearAll(uint32 unit);

extern int32
rtrpc_acl_ruleEntryField_check(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_fieldType_t type);

extern int32
rtrpc_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

extern int32
rtrpc_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

extern int32
rtrpc_acl_meterMode_get(
    uint32              unit,
    uint32              idx,
    rtk_acl_meterMode_t *pMeterMode);

extern int32
rtrpc_acl_meterMode_set(
    uint32              unit,
    uint32              idx,
    rtk_acl_meterMode_t meterMode);

extern int32
rtrpc_acl_meterBurstSize_get(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize);

extern int32
rtrpc_acl_meterBurstSize_set(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize);

extern int32
rtrpc_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData);

extern int32
rtrpc_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData);

extern int32
rtrpc_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData);

extern int32
rtrpc_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData);

extern int32
rtrpc_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

extern int32
rtrpc_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

extern int32
rtrpc_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData);

extern int32
rtrpc_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData);

extern int32
rtrpc_acl_rangeCheckFieldSelector_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData);

extern int32
rtrpc_acl_rangeCheckFieldSelector_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData);

extern int32
rtrpc_acl_partition_get(uint32 unit, uint32 *pPartition);

extern int32
rtrpc_acl_partition_set(uint32 unit, uint32 partition);

extern int32
rtrpc_acl_blockResultMode_get(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t *pMode);

extern int32
rtrpc_acl_blockResultMode_set(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t mode);

extern int32
rtrpc_acl_templateFieldIntentVlanTag_get(uint32 unit, rtk_vlan_tagType_t *tagType);

extern int32
rtrpc_acl_templateFieldIntentVlanTag_set(uint32 unit, rtk_vlan_tagType_t tagType);

extern int32
rtrpc_acl_rangeCheckDstPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

extern int32
rtrpc_acl_rangeCheckDstPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

extern int32
rtrpc_acl_loopBackEnable_get(uint32 unit, uint32 *pEnable);


extern int32
rtrpc_acl_loopBackEnable_set(uint32 unit, uint32 enable);

extern int32
rtrpc_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times);


extern int32
rtrpc_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times);

extern int32
rtrpc_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable);

extern int32
rtrpc_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable);

extern int32
rtrpc_acl_templateSelector_get(uint32 unit, uint32 block_idx,
    rtk_acl_templateIdx_t *pTemplate_idx);

extern int32
rtrpc_acl_templateSelector_set(uint32 unit, uint32 block_idx,
    rtk_acl_templateIdx_t template_idx);

extern int32
rtrpc_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt);

extern int32
rtrpc_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode);

extern int32
rtrpc_acl_ruleHitIndication_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 reset, uint32 *pIsHit);

extern int32
rtrpc_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
        uint32 reset, rtk_acl_hitMask_t *pHitMask);

extern int32
rtrpc_acl_rule_del(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_clear_t *pClrIdx);

extern int32
rtrpc_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData);

extern int32
rtrpc_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type);

extern int32
rtrpc_aclMapper_init(dal_mapper_t *pMapper);

extern int32
rtrpc_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info);

extern int32
rtrpc_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id);
#endif /* __RTRPC_ACL_H__ */

