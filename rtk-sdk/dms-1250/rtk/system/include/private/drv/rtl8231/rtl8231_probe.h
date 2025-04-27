/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public RTL8231 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RTL8231 probe
 *
 */
#ifndef __DRV_RTL8231_PROBE_H__
#define __DRV_RTL8231_PROBE_H__

/*
 * Include Files
 */
#include <private/drv/rtl8231/rtl8231.h>

/*
 * Symbol Definition
 */
typedef enum rtl8231_controller_e
{
#if defined(CONFIG_SDK_RTL9310)
    RTL8231_R9310,
#endif
#if defined(CONFIG_SDK_RTL9300)
    RTL8231_R9300,
#endif
#if defined(CONFIG_SDK_RTL8390)
    RTL8231_R8390,
#endif
#if defined(CONFIG_SDK_RTL8380)
    RTL8231_R8380,
#endif
    RTL8231_CTRL_END
} rtl8231_controller_t;

typedef struct rtl8231_mapper_operation_s
{
    /* RTL8231 */
    int32   (*i2c_read)(uint32 , uint32 , uint32 , uint32 *);
    int32   (*i2c_write)(uint32 , uint32 , uint32 , uint32 );
    int32   (*mdc_read)(uint32 , uint32 , uint32 , uint32 , uint32 *);
    int32   (*mdc_write)(uint32 , uint32 , uint32 , uint32 , uint32 );
    int32   (*init)(uint32 );
    int32   (*mdcSem_register)(uint32 , drv_rtl8231_mdcSem_cb_f);
    int32   (*mdcSem_unregister)(uint32 , drv_rtl8231_mdcSem_cb_f);
    int32   (*intrStatus_get)(uint32 , uint32 , uint32, uint32 *);
    int32   (*intrStatus_clear)(uint32 , uint32 , uint32);
    int32   (*extGPIOIntrStatus_get)(uint32 , uint32 , uint32 *);
    int32   (*pinIntrEnable_get)(uint32 , uint32 , uint32, rtk_enable_t *);
    int32   (*pinIntrEnable_set)(uint32 , uint32 , uint32, rtk_enable_t);
    int32   (*pinIntrMode_get)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t *);
    int32   (*pinIntrMode_set)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t);
    int32   (*directAccess_set)(uint32 , uint32 , drv_extGpio_directAccessMode_t, rtk_enable_t);
} rtl8231_mapper_operation_t;


#define RTL8231_CTRL(unit)      rtl8231_ops[rtl8231_if[unit]]
#define RTL8231_CHK(unit)       (rtl8231_if[unit] != CID_GROUP_NONE)

/*
 * Data Declaration
 */
extern rtl8231_mapper_operation_t rtl8231_ops[RTL8231_CTRL_END];
extern uint32 rtl8231_if[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtl8231_probe
 * Description:
 *      Probe rtl8231 module of the specified device.
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
 *      None
 */
extern int32
rtl8231_probe(uint32 unit);


#endif /* __DRV_RTL8231_PROBE_H__ */

