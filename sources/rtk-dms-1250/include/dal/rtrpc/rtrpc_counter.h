/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_COUNTER_H__
#define __RTRPC_COUNTER_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_stat_enable_get                              rtrpc_stat_enable_get
#define rtk_stat_enable_set                              rtrpc_stat_enable_set
#define rtk_stat_global_get                              rtrpc_stat_global_get
#define rtk_stat_global_getAll                           rtrpc_stat_global_getAll
#define rtk_stat_global_reset                            rtrpc_stat_global_reset
#define rtk_stat_port_get                                rtrpc_stat_port_get
#define rtk_stat_port_getAll                             rtrpc_stat_port_getAll
#define rtk_stat_port_reset                              rtrpc_stat_port_reset
#define rtk_stat_tagLenCntIncEnable_get                  rtrpc_stat_tagLenCntIncEnable_get
#define rtk_stat_tagLenCntIncEnable_set                  rtrpc_stat_tagLenCntIncEnable_set
#define rtk_stat_stackHdrLenCntIncEnable_get             rtrpc_stat_stackHdrLenCntIncEnable_get
#define rtk_stat_stackHdrLenCntIncEnable_set             rtrpc_stat_stackHdrLenCntIncEnable_set
#define rtk_stat_flexibleCntRange_get                    rtrpc_stat_flexibleCntRange_get
#define rtk_stat_flexibleCntRange_set                    rtrpc_stat_flexibleCntRange_set
/*
 * Function Declaration
 */

/* Module Name : STAT */

extern int32
rtrpc_stat_init(uint32 unit);

extern int32
rtrpc_stat_enable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_stat_enable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_stat_global_reset(uint32 unit);

extern int32
rtrpc_stat_port_reset(uint32 unit, rtk_port_t port);

extern int32
rtrpc_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr);

extern int32
rtrpc_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs);

extern int32
rtrpc_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr);

extern int32
rtrpc_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs);

extern int32
rtrpc_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable);

extern int32
rtrpc_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable);

extern int32
rtrpc_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable);

extern int32
rtrpc_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);

extern int32
rtrpc_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);

#endif /* __RTRPC_STAT_H__ */
