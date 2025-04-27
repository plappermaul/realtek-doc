/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_MIRROR_H__
#define __RTRPC_MIRROR_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_mirror_group_init                            rtrpc_mirror_group_init
#define rtk_mirror_group_get                             rtrpc_mirror_group_get
#define rtk_mirror_group_set                             rtrpc_mirror_group_set
#define rtk_mirror_rspanIgrMode_get                      rtrpc_mirror_rspanIgrMode_get
#define rtk_mirror_rspanIgrMode_set                      rtrpc_mirror_rspanIgrMode_set
#define rtk_mirror_rspanEgrMode_get                      rtrpc_mirror_rspanEgrMode_get
#define rtk_mirror_rspanEgrMode_set                      rtrpc_mirror_rspanEgrMode_set
#define rtk_mirror_rspanTag_get                          rtrpc_mirror_rspanTag_get
#define rtk_mirror_rspanTag_set                          rtrpc_mirror_rspanTag_set
#define rtk_mirror_sflowMirrorSampleRate_get             rtrpc_mirror_sflowMirrorSampleRate_get
#define rtk_mirror_sflowMirrorSampleRate_set             rtrpc_mirror_sflowMirrorSampleRate_set
#define rtk_mirror_egrQueue_get                          rtrpc_mirror_egrQueue_get
#define rtk_mirror_egrQueue_set                          rtrpc_mirror_egrQueue_set
#define rtk_mirror_sflowPortIgrSampleRate_get            rtrpc_mirror_sflowPortIgrSampleRate_get
#define rtk_mirror_sflowPortIgrSampleRate_set            rtrpc_mirror_sflowPortIgrSampleRate_set
#define rtk_mirror_sflowPortEgrSampleRate_get            rtrpc_mirror_sflowPortEgrSampleRate_get
#define rtk_mirror_sflowPortEgrSampleRate_set            rtrpc_mirror_sflowPortEgrSampleRate_set
#define rtk_mirror_sflowSampleCtrl_get                   rtrpc_mirror_sflowSampleCtrl_get
#define rtk_mirror_sflowSampleCtrl_set                   rtrpc_mirror_sflowSampleCtrl_set
#define rtk_mirror_sflowSampleTarget_get                 rtrpc_mirror_sflowSampleTarget_get
#define rtk_mirror_sflowSampleTarget_set                 rtrpc_mirror_sflowSampleTarget_set
/*
 * Function Declaration
 */

/* Module Name : Mirror */

extern int32
rtrpc_mirror_init(uint32 unit);

/* Module Name    : Mirror             */
/* Sub-module Name: Group-based mirror */

extern int32
rtrpc_mirror_group_init(
    uint32              unit,
    rtk_mirror_entry_t  *pMirrorEntry);

extern int32
rtrpc_mirror_group_get(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry);

extern int32
rtrpc_mirror_group_set(
    uint32              unit,
    uint32              mirror_id,
    rtk_mirror_entry_t  *pMirrorEntry);

extern int32
rtrpc_mirror_rspanIgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t *pIgrMode);

extern int32
rtrpc_mirror_rspanIgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanIgrMode_t igrMode);

extern int32
rtrpc_mirror_rspanEgrMode_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t *pEgrMode);

extern int32
rtrpc_mirror_rspanEgrMode_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanEgrMode_t egrMode);


/* Module Name    : Mirror */
/* Sub-module Name: RSPAN  */

extern int32
rtrpc_mirror_rspanTag_get(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag);

extern int32
rtrpc_mirror_rspanTag_set(uint32 unit, uint32 mirror_id, rtk_mirror_rspanTag_t *pTag);

/* Module Name    : Mirror             */
/* Sub-module Name: Mirror-based SFLOW */

extern int32
rtrpc_mirror_sflowMirrorSampleRate_get(uint32 unit, uint32 mirror_id, uint32 *pRate);

extern int32
rtrpc_mirror_sflowMirrorSampleRate_set(uint32 unit, uint32 mirror_id, uint32 rate);


extern int32
rtrpc_mirror_egrQueue_get(uint32 unit, rtk_enable_t *pEnable, rtk_qid_t *pQid);

int32
rtrpc_mirror_egrQueue_set(uint32 unit, rtk_enable_t enable, rtk_qid_t qid);

extern int32
rtrpc_mirror_sflowPortIgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

extern int32
rtrpc_mirror_sflowPortIgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate);

extern int32
rtrpc_mirror_sflowPortEgrSampleRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

extern int32
rtrpc_mirror_sflowPortEgrSampleRate_set(uint32 unit, rtk_port_t port, uint32 rate);

extern int32
rtrpc_mirror_sflowSampleCtrl_get(uint32 unit, rtk_sflowSampleCtrl_t *pCtrl);

extern int32
rtrpc_mirror_sflowSampleCtrl_set(uint32 unit, rtk_sflowSampleCtrl_t ctrl);

extern int32
rtrpc_mirror_sflowSampleTarget_get(uint32 unit, rtk_sflow_sampleTarget_t *pTarget);

extern int32
rtrpc_mirror_sflowSampleTarget_set(uint32 unit, rtk_sflow_sampleTarget_t target);

int32
rtrpc_mirrorMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_MIRROR_H__ */

