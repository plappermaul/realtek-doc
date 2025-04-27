/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_EEE_H__
#define __RTRPC_EEE_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_eee_portEnable_get                           rtrpc_eee_portEnable_get
#define rtk_eee_portEnable_set                           rtrpc_eee_portEnable_set
#define rtk_eee_portState_get                            rtrpc_eee_portState_get
#define rtk_eeep_portEnable_get                          rtrpc_eeep_portEnable_get
#define rtk_eee_portPowerState_get                       rtrpc_eee_portPowerState_get
#define rtk_eeep_portEnable_set                          rtrpc_eeep_portEnable_set
/*
 * Function Declaration
 */

extern int32
rtrpc_eee_init(uint32 unit);


/* Module Name    : EEE                */
/* Sub-module Name: EEE enable/disable */

extern int32
rtrpc_eee_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_eee_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_eee_portState_get(
    uint32 unit,
    rtk_port_t port,
    rtk_enable_t *pState);

extern int32
rtrpc_eee_portPowerState_get(
    uint32 unit,
    rtk_port_t port,
    rtk_eee_direction_t direction,
    rtk_eee_power_state_t *pState);

extern int32
rtrpc_eeep_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_eeep_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_eeeMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_EEE_H__ */
