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
 * Purpose : I2C master driver.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) i2c read and write
 *
 */
#include <common/rt_autoconf.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <drv/i2c/i2c.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <hal/chipdef/chip.h>
#include <ioal/ioal_init.h>
#include <hwp/hw_profile.h>
#include <private/drv/i2c/i2c_software_drv.h>
#include <drv/gpio/generalCtrl_gpio.h>

static uint8    i2c_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

int32 r8390_i2c_init(uint32 unit)
{
    int32 ret;

    RT_INIT_REENTRY_CHK(i2c_init[unit]);
    i2c_init[unit] = INIT_COMPLETED;

    ret = drv_software_i2c_module_init(unit);
    if(ret != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

int32 r8390_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev)
{
    int32     ret;
    uint32    i2c_addrwidth, i2c_datawidth;

    if((ret = drv_software_i2c_init(i2c_dev->scl_dev, i2c_dev->scl_pin_id, i2c_dev->sda_dev, i2c_dev->sda_pin_id, i2c_dev->device_id)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    if(((i2c_dev->mem_addr_width) >= I2C_ADDR_WIDTH_BYTE_END) || ((i2c_dev->data_width) >= I2C_DATA_WIDTH_BYTE_END))
    {
        return RT_ERR_FAILED;
    }

    i2c_addrwidth = i2c_dev->mem_addr_width;
    i2c_datawidth = i2c_dev->data_width;

    drv_software_i2c_type_set(i2c_addrwidth, i2c_datawidth, i2c_dev->dev_addr, i2c_dev->scl_delay, i2c_dev->device_id);
    return RT_ERR_OK;
}


int32 r8390_i2c_read(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idex, uint8 *pBuff)
{
    int32    ret;

    if(i2c_dev->read_type == I2C_INTF_READ_TYPE_RANDOM)
    {
        if((ret = drv_software_i2c_read(reg_idex, pBuff, i2c_dev->device_id)) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }
    else if(i2c_dev->read_type == I2C_INTF_READ_TYPE_SEQUENTIAL)
    {
        if((ret = drv_software_i2c_sequential_read(reg_idex, pBuff, i2c_dev->device_id)) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }

    }
    else
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 r8390_i2c_write(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idex, uint8 *pBuff)
{
    int32    ret;

    if((ret = drv_software_i2c_write(reg_idex, pBuff, i2c_dev->device_id)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 r8390_i2c_type_set(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 i2c_addrWidth, uint32 i2c_dataWidth, uint32 chipid, uint32 delay)
{
    int32    ret;

    if((ret = drv_software_i2c_type_set(i2c_addrWidth, i2c_dataWidth, chipid, delay, i2c_dev_id)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}

int32 r8390_i2c_type_get(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 * i2c_addrWidth, uint32 * i2c_dataWidth, uint32 * pchipid, uint32 * pdelay)
{
    int32    ret;

    if((ret = drv_software_i2c_type_get(i2c_addrWidth, i2c_dataWidth, pchipid, pdelay, i2c_dev_id)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

