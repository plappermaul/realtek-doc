/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_STP_H__
#define __RTRPC_STP_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_stp_mstpState_get                            rtrpc_stp_mstpState_get
#define rtk_stp_mstpState_set                            rtrpc_stp_mstpState_set
#define rtk_stp_mstpInstance_create                      rtrpc_stp_mstpInstance_create
#define rtk_stp_mstpInstance_destroy                     rtrpc_stp_mstpInstance_destroy
#define rtk_stp_isMstpInstanceExist_get                  rtrpc_stp_isMstpInstanceExist_get
#define rtk_stp_mstpInstanceMode_get                     rtrpc_stp_mstpInstanceMode_get
#define rtk_stp_mstpInstanceMode_set                     rtrpc_stp_mstpInstanceMode_set
/*
 * Function Declaration
 */

/* Module Name : STP */

extern int32
rtrpc_stp_init(uint32 unit);

extern int32
rtrpc_stp_mstpInstance_create(uint32 unit, uint32 msti);

extern int32
rtrpc_stp_mstpInstance_destroy(uint32 unit, uint32 msti);

extern int32
rtrpc_stp_isMstpInstanceExist_get(uint32 unit, uint32 msti, uint32 *pMsti_exist);

extern int32
rtrpc_stp_mstpState_get(
    uint32          unit,
    uint32          msti,
    rtk_port_t      port,
    rtk_stp_state_t *pStp_state);

extern int32
rtrpc_stp_mstpState_set(
    uint32          unit,
    uint32          msti,
    rtk_port_t      port,
    rtk_stp_state_t stp_state);

extern int32
rtrpc_stp_mstpInstanceMode_get(uint32 unit, rtk_stp_mstiMode_t *pMsti_mode);

extern int32
rtrpc_stp_mstpInstanceMode_set(uint32 unit, rtk_stp_mstiMode_t msti_mode);

extern int32
rtrpc_stpMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_STP_H__ */

