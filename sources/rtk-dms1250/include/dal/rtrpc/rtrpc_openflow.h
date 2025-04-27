/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_OPENFLOW_H__
#define __RTRPC_OPENFLOW_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_of_init                                      rtrpc_of_init
#define rtk_of_classifier_get                            rtrpc_of_classifier_get
#define rtk_of_classifier_set                            rtrpc_of_classifier_set
#define rtk_of_flowMatchFieldSize_get                    rtrpc_of_flowMatchFieldSize_get
#define rtk_of_flowEntrySize_get                         rtrpc_of_flowEntrySize_get
#define rtk_of_flowEntryValidate_get                     rtrpc_of_flowEntryValidate_get
#define rtk_of_flowEntryValidate_set                     rtrpc_of_flowEntryValidate_set
#define rtk_of_flowEntryFieldList_get                    rtrpc_of_flowEntryFieldList_get
#define rtk_of_flowEntryField_check                      rtrpc_of_flowEntryField_check
#define rtk_of_flowEntrySetField_check                   rtrpc_of_flowEntrySetField_check
#define rtk_of_flowEntryField_read                       rtrpc_of_flowEntryField_read
#define rtk_of_flowEntryField_write                      rtrpc_of_flowEntryField_write
#define rtk_of_flowEntryOperation_get                    rtrpc_of_flowEntryOperation_get
#define rtk_of_flowEntryOperation_set                    rtrpc_of_flowEntryOperation_set
#define rtk_of_flowEntryInstruction_get                  rtrpc_of_flowEntryInstruction_get
#define rtk_of_flowEntryInstruction_set                  rtrpc_of_flowEntryInstruction_set
#define rtk_of_flowEntryHitSts_get                       rtrpc_of_flowEntryHitSts_get
#define rtk_of_flowEntry_del                             rtrpc_of_flowEntry_del
#define rtk_of_flowEntry_move                            rtrpc_of_flowEntry_move
#define rtk_of_ftTemplateSelector_get                    rtrpc_of_ftTemplateSelector_get
#define rtk_of_ftTemplateSelector_set                    rtrpc_of_ftTemplateSelector_set
#define rtk_of_flowCntMode_get                           rtrpc_of_flowCntMode_get
#define rtk_of_flowCntMode_set                           rtrpc_of_flowCntMode_set
#define rtk_of_flowCnt_get                               rtrpc_of_flowCnt_get
#define rtk_of_flowCnt_clear                             rtrpc_of_flowCnt_clear
#define rtk_of_flowCntThresh_get                         rtrpc_of_flowCntThresh_get
#define rtk_of_flowCntThresh_set                         rtrpc_of_flowCntThresh_set
#define rtk_of_ttlExcpt_get                              rtrpc_of_ttlExcpt_get
#define rtk_of_ttlExcpt_set                              rtrpc_of_ttlExcpt_set
#define rtk_of_maxLoopback_get                           rtrpc_of_maxLoopback_get
#define rtk_of_maxLoopback_set                           rtrpc_of_maxLoopback_set
#define rtk_of_l2FlowTblMatchField_get                   rtrpc_of_l2FlowTblMatchField_get
#define rtk_of_l2FlowTblMatchField_set                   rtrpc_of_l2FlowTblMatchField_set
#define rtk_of_l2FlowEntrySetField_check                 rtrpc_of_l2FlowEntrySetField_check
#define rtk_of_l2FlowEntry_get                           rtrpc_of_l2FlowEntry_get
#define rtk_of_l2FlowEntryNextValid_get                  rtrpc_of_l2FlowEntryNextValid_get
#define rtk_of_l2FlowEntry_add                           rtrpc_of_l2FlowEntry_add
#define rtk_of_l2FlowEntry_del                           rtrpc_of_l2FlowEntry_del
#define rtk_of_l2FlowEntry_delAll                        rtrpc_of_l2FlowEntry_delAll
#define rtk_of_l2FlowEntryHitSts_get                     rtrpc_of_l2FlowEntryHitSts_get
#define rtk_of_l2FlowTblHashAlgo_get                     rtrpc_of_l2FlowTblHashAlgo_get
#define rtk_of_l2FlowTblHashAlgo_set                     rtrpc_of_l2FlowTblHashAlgo_set
#define rtk_of_l3FlowTblPri_get                          rtrpc_of_l3FlowTblPri_get
#define rtk_of_l3FlowTblPri_set                          rtrpc_of_l3FlowTblPri_set
#define rtk_of_l3CamFlowTblMatchField_get                rtrpc_of_l3CamFlowTblMatchField_get
#define rtk_of_l3CamFlowTblMatchField_set                rtrpc_of_l3CamFlowTblMatchField_set
#define rtk_of_l3HashFlowTblMatchField_get               rtrpc_of_l3HashFlowTblMatchField_get
#define rtk_of_l3HashFlowTblMatchField_set               rtrpc_of_l3HashFlowTblMatchField_set
#define rtk_of_l3HashFlowTblHashAlgo_get                 rtrpc_of_l3HashFlowTblHashAlgo_get
#define rtk_of_l3HashFlowTblHashAlgo_set                 rtrpc_of_l3HashFlowTblHashAlgo_set
#define rtk_of_l3FlowEntrySetField_check                 rtrpc_of_l3FlowEntrySetField_check
#define rtk_of_l3CamFlowEntry_get                        rtrpc_of_l3CamFlowEntry_get
#define rtk_of_l3CamFlowEntry_add                        rtrpc_of_l3CamFlowEntry_add
#define rtk_of_l3CamFlowEntry_del                        rtrpc_of_l3CamFlowEntry_del
#define rtk_of_l3CamFlowEntry_move                       rtrpc_of_l3CamFlowEntry_move
#define rtk_of_l3CamflowEntryHitSts_get                  rtrpc_of_l3CamflowEntryHitSts_get
#define rtk_of_l3HashFlowEntry_get                       rtrpc_of_l3HashFlowEntry_get
#define rtk_of_l3HashFlowEntryNextValid_get              rtrpc_of_l3HashFlowEntryNextValid_get
#define rtk_of_l3HashFlowEntry_add                       rtrpc_of_l3HashFlowEntry_add
#define rtk_of_l3HashFlowEntry_del                       rtrpc_of_l3HashFlowEntry_del
#define rtk_of_l3HashFlowEntry_delAll                    rtrpc_of_l3HashFlowEntry_delAll
#define rtk_of_l3HashflowEntryHitSts_get                 rtrpc_of_l3HashflowEntryHitSts_get
#define rtk_of_groupEntry_get                            rtrpc_of_groupEntry_get
#define rtk_of_groupEntry_set                            rtrpc_of_groupEntry_set
#define rtk_of_groupTblHashPara_get                      rtrpc_of_groupTblHashPara_get
#define rtk_of_groupTblHashPara_set                      rtrpc_of_groupTblHashPara_set
#define rtk_of_actionBucket_get                          rtrpc_of_actionBucket_get
#define rtk_of_actionBucket_set                          rtrpc_of_actionBucket_set
#define rtk_of_trapTarget_get                            rtrpc_of_trapTarget_get
#define rtk_of_trapTarget_set                            rtrpc_of_trapTarget_set
#define rtk_of_tblMissAction_get                         rtrpc_of_tblMissAction_get
#define rtk_of_tblMissAction_set                         rtrpc_of_tblMissAction_set
#define rtk_of_flowTblCnt_get                            rtrpc_of_flowTblCnt_get
/*
 * Function Declaration
 */

/* Module Name     : OpenFlow */

extern int32
rtrpc_of_init(uint32 unit);

extern int32
rtrpc_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData);

extern int32
rtrpc_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data);

extern int32
rtrpc_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size);

extern int32
rtrpc_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size);

extern int32
rtrpc_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid);

extern int32
rtrpc_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid);

extern int32
rtrpc_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list);

extern int32
rtrpc_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type);

extern int32
rtrpc_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

extern int32
rtrpc_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

extern int32
rtrpc_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

extern int32
rtrpc_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

extern int32
rtrpc_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

extern int32
rtrpc_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

extern int32
rtrpc_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

extern int32
rtrpc_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit);

extern int32
rtrpc_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx);

extern int32
rtrpc_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData);

extern int32
rtrpc_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx);

extern int32
rtrpc_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx);

extern int32
rtrpc_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode);

extern int32
rtrpc_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode);

extern int32
rtrpc_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt);

extern int32
rtrpc_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type);

extern int32
rtrpc_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold);

extern int32
rtrpc_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold);

extern int32
rtrpc_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction);

extern int32
rtrpc_of_ttlExcpt_set(uint32 unit, rtk_action_t action);

extern int32
rtrpc_of_maxLoopback_get(uint32 unit, uint32 *pTimes);

extern int32
rtrpc_of_maxLoopback_set(uint32 unit, uint32 times);

extern int32
rtrpc_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField);

extern int32
rtrpc_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field);

extern int32
rtrpc_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

extern int32
rtrpc_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

extern int32
rtrpc_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry);

extern int32
rtrpc_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

extern int32
rtrpc_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

extern int32
rtrpc_of_l2FlowEntry_delAll(uint32 unit);

int32
rtrpc_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

extern int32
rtrpc_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

extern int32
rtrpc_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

extern int32
rtrpc_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable);

extern int32
rtrpc_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table);

extern int32
rtrpc_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField);

extern int32
rtrpc_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field);

extern int32
rtrpc_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField);

extern int32
rtrpc_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field);

extern int32
rtrpc_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

extern int32
rtrpc_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

extern int32
rtrpc_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

extern int32
rtrpc_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx);

extern int32
rtrpc_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pClrIdx);

extern int32
rtrpc_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit);

extern int32
rtrpc_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

extern int32
rtrpc_of_l3HashFlowEntry_delAll(uint32 unit);

extern int32
rtrpc_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

extern int32
rtrpc_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

extern int32
rtrpc_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

extern int32
rtrpc_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para);

extern int32
rtrpc_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para);

extern int32
rtrpc_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

extern int32
rtrpc_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

extern int32
rtrpc_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

extern int32
rtrpc_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

extern int32
rtrpc_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct);

extern int32
rtrpc_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act);

extern int32
rtrpc_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt);

extern int32
rtrpc_ofMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_OPENFLOW_H__ */

