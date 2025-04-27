/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DRV_TC_MAPPER_H__
#define __DRV_TC_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>
#include <drv/tc/tc.h>

/*
 * Symbol Definition
 */
typedef enum tc_controller_e
{
#if (defined(CONFIG_SDK_RTL8390))
    TC_R8390,
#endif
#if (defined(CONFIG_SDK_RTL8380))
    TC_R8380,
#endif
#if (defined(CONFIG_SDK_RTL9300))
    TC_R9300,
#endif
#if (defined(CONFIG_SDK_RTL9310))
    TC_R9310,
#endif
    TC_CTRL_END
} tc_controller_t;

typedef struct tc_mapper_operation_s
{
    int32 (*init)(uint32);
    int32 (*tc_enable_set)(uint32,drv_tc_id_t, rtk_enable_t);
    int32 (*tcMode_set)(uint32, drv_tc_id_t, drv_tc_mode_t);
    int32 (*tcDivFactor_set)(uint32, drv_tc_id_t,uint32);
    int32 (*tcDataInitValue_set)(uint32, drv_tc_id_t, uint32);
    int32 (*tcIntEnable_set)(uint32, drv_tc_id_t, rtk_enable_t);
    int32 (*tcIntState_get)(uint32, drv_tc_id_t, drv_tc_intState_t *);
    int32 (*tcIntState_clear)(uint32, drv_tc_id_t);
    int32 (*tcCounterValue_get)(uint32, drv_tc_id_t, uint32 *);
} tc_mapper_operation_t;

extern tc_mapper_operation_t tc_ops[];
extern uint32 tc_if[];
#define TC_CTRL(unit)      tc_ops[tc_if[unit]]
#define TC_CHK(unit)       (tc_if[unit] != CID_GROUP_NONE)


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      tc_probe
 * Description:
 *      Probe tc module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
tc_probe(uint32 unit);


/* Function Name:
 *      drv_tc_init
 * Description:
 *      Initialize TC module.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_init(uint32 unit);


#endif /* __DRV_TC_MAPPER_H__ */

