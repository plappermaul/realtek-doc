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
#ifndef __DRV_I2C_MAPPER_H__
#define __DRV_I2C_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/error.h>
#include <drv/i2c/i2c.h>

/*
 * Symbol Definition
 */
typedef enum i2c_controller_e
{
#if (defined(CONFIG_SDK_DRIVER_I2C_R8390))
    I2C_R8390,
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R8380))
    I2C_R8380,
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9300))
    I2C_R9300,
#endif
#if (defined(CONFIG_SDK_DRIVER_I2C_R9310))
    I2C_R9310,
#endif
    I2C_CTRL_END
} i2c_controller_t;

typedef struct i2c_mapper_operation_s
{
    /* I2C */
    int32   (*init)(uint32);
    int32   (*i2c_dev_init)(uint32, i2c_devConf_t *);
    int32   (*i2c_write)(uint32, i2c_devConf_t *, uint32, uint8 *);
    int32   (*i2c_read)(uint32, i2c_devConf_t *, uint32, uint8 *);
    int32   (*i2c_type_set)(uint32, drv_i2c_devId_t, uint32, uint32, uint32);
    int32   (*i2c_type_get)(uint32, drv_i2c_devId_t, uint32 *, uint32 *, uint32 *);
} i2c_mapper_operation_t;

#define I2C_CTRL(unit)      i2c_ops[i2c_if[unit]]
#define I2C_CHK(unit)       (i2c_if[unit] != CID_GROUP_NONE)


/*
 * Data Declaration
 */
extern i2c_mapper_operation_t i2c_ops[I2C_CTRL_END];
extern uint32 i2c_if[RTK_MAX_NUM_OF_UNIT];


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


#endif /* __DRV_I2C_MAPPER_H __ */

