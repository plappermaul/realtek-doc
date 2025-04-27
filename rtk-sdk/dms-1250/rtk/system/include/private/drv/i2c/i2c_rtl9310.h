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
 * Purpose : DRV APIs definition.
 *
 * Feature : I2C relative API
 *
 */
#ifndef __I2C_R9310_H__
#define __I2C_R9310_H__

/*
* Include Files
*/
//#include <common/rt_error.h>

/*
* Symbol Definition
*/
typedef enum drv_i2c_9310_intf_sda_e
{
    I2C_9310_INTF_CONTROLLER_SDA0 = 0,
    I2C_9310_INTF_CONTROLLER_SDA1,
    I2C_9310_INTF_CONTROLLER_SDA2,
    I2C_9310_INTF_CONTROLLER_SDA3,
    I2C_9310_INTF_CONTROLLER_SDA4,
    I2C_9310_INTF_CONTROLLER_SDA5,
    I2C_9310_INTF_CONTROLLER_SDA6,
    I2C_9310_INTF_CONTROLLER_SDA7,
    I2C_9310_INTF_CONTROLLER_SDA8,
    I2C_9310_INTF_CONTROLLER_SDA9,
    I2C_9310_INTF_CONTROLLER_SDA10,
    I2C_9310_INTF_CONTROLLER_SDA11,
    I2C_9310_INTF_CONTROLLER_SDA_END
} drv_i2c_9310_intf_sda_t;

#define RTL9310_INT_I2C_DATA_REG_BYTE_MASK                         (0xFF)


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
extern int32 r9310_i2c_init(uint32 unit);
extern int32 r9310_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev);
extern int32 r9310_i2c_read(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idx, uint8 *pBuff);
extern int32 r9310_i2c_write(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idx, uint8 *pBuff);
extern int32 r9310_i2c_type_set(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 i2c_type, uint32 chipid, uint32 delay);
extern int32 r9310_i2c_type_get(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 * ptype, uint32 * pchipid, uint32 * pdelay);

#endif /* __I2C_R9310_H__ */

