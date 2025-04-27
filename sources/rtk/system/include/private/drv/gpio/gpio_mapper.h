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
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DRV_GPIO_INIT_H__
#define __DRV_GPIO_INIT_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>

/*
 * Symbol Definition
 */
typedef enum gpio_controller_e
{
#if (defined(CONFIG_SDK_RTL9310))
    GPIO_R9310,
#endif
#if (defined(CONFIG_SDK_RTL9300))
    GPIO_R9300,
#endif
#if (defined(CONFIG_SDK_RTL8390))
    GPIO_R8390,
#endif
#if (defined(CONFIG_SDK_RTL8380))
    GPIO_R8380,
#endif
    GPIO_CTRL_END
} gpio_controller_t;

typedef struct gpio_mapper_operation_s
{
    int32   (*init)(uint32);
    int32   (*direction_set)(uint32, drv_gpio_port_t, uint32, drv_gpio_direction_t);
    int32   (*control_set)(uint32, drv_gpio_port_t, uint32, drv_gpio_control_t);
    int32   (*intrMode_set)(uint32, drv_gpio_port_t, uint32, drv_gpio_interruptType_t);
    int32   (*dataBit_init)(uint32, drv_gpio_port_t, uint32, uint32);
    int32   (*dataBit_get)(uint32, drv_gpio_port_t, uint32, uint32 *);
    int32   (*dataBit_set)(uint32, drv_gpio_port_t, uint32, uint32);
    int32   (*intrStatus_get)(uint32, drv_gpio_port_t, uint32, uint32 *);
    int32   (*intrStatus_clear)(uint32, drv_gpio_port_t, uint32);
    int32   (*portRange_get)(uint32, int32 *, int32 *);
} gpio_mapper_operation_t;

#define GPIO_CTRL(unit)         gpio_ops[gpio_if[unit]]
#define GPIO_CHK(unit)          (gpio_if[unit] != CID_GROUP_NONE)

/*
 * Data Declaration
 */
extern gpio_mapper_operation_t gpio_ops[];
extern uint32 gpio_if[];


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


#endif /* __DRV_GPIO_INIT_H __ */

