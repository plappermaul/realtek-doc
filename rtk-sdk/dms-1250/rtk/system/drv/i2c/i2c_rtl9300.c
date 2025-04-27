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
#include <hal/chipdef/chip.h>
#include <ioal/ioal_init.h>
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <private/drv/i2c/i2c_rtl9300.h>
#include <private/drv/i2c/i2c_software_drv.h>
#include <drv/gpio/generalCtrl_gpio.h>

extern i2c_devInfo_t gI2C_dev[RTK_MAX_NUM_OF_UNIT][I2C_DEV_ID_END];
static uint8         i2c_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};


static int32 r9300_data_write(uint32 unit, i2c_devConf_t *i2c_dev, uint8 *pBuff)
{
    uint32  data_width, reg_offset;
    uint32  reg_data;
    uint32  reg_addr;
    int32   ret = RT_ERR_OK;

    data_width = gI2C_dev[unit][i2c_dev->device_id].conf.data_width;

    switch(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id){
        case I2C_INTF_CONTROLLER_ID0:
            reg_addr = RTL9300_I2C_MST1_DATA_START_ADDR;
            break;
        case I2C_INTF_CONTROLLER_ID1:
            reg_addr = RTL9300_I2C_MST2_DATA_START_ADDR;
            break;
        default:
            return RT_ERR_FAILED;
    }

    while(data_width)
    {
        reg_data = 0;
        for(reg_offset = 0; reg_offset < RTL9300_INT_I2C_DATA_REG_BYTE_COUNT; reg_offset++)
        {
            reg_data |= ((*(pBuff)) << (reg_offset*RTL9300_INT_I2C_DATA_REG_BYTE_SHIFT));
            pBuff++;
            data_width --;
            if(data_width == 0)
                break;
        }
        ret = ioal_mem32_write(unit, reg_addr, reg_data);
        reg_addr += RTL9300_INT_I2C_DATA_REG_OFFSET;
    }

    return ret;
}



static int32 r9300_data_read(uint32 unit, i2c_devConf_t *i2c_dev, uint8 *pBuff)
{
    uint32  data_width, reg_offset;
    uint32  reg_data;
    uint32  reg_addr;
    int32   ret = RT_ERR_OK;

    data_width = gI2C_dev[unit][i2c_dev->device_id].conf.data_width;

    switch(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id){
        case I2C_INTF_CONTROLLER_ID0:
            reg_addr = RTL9300_I2C_MST1_DATA_START_ADDR;
            break;
        case I2C_INTF_CONTROLLER_ID1:
            reg_addr = RTL9300_I2C_MST2_DATA_START_ADDR;
            break;
        default:
            return RT_ERR_FAILED;
    }
    while(data_width)
    {
       ret = ioal_mem32_read(unit, reg_addr, &reg_data);

       for(reg_offset = 0; reg_offset < RTL9300_INT_I2C_DATA_REG_BYTE_COUNT; reg_offset++)
       {
            *(pBuff) = (reg_data & RTL9300_INT_I2C_DATA_REG_BYTE_MASK);
            reg_data = (reg_data >> (RTL9300_INT_I2C_DATA_REG_BYTE_SHIFT));
            pBuff++;
            data_width --;
            if(data_width == 0)
                break;
       }
       reg_addr += RTL9300_INT_I2C_DATA_REG_OFFSET;
    }
     return ret;
}

int32 r9300_i2c_execution(uint32 unit, uint32 intf_id)
{
    uint32  reg_data;
    int32   ret = RT_ERR_OK;

    switch(intf_id){
        case I2C_INTF_CONTROLLER_ID0:
            /* Set Trig */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
            reg_data |= RTL9300_I2C_MST1_CTRL1_I2C_TRIG_MASK;
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST1_CTRL1_ADDR, reg_data);
            /* Check Complete or Not */
            while(1)
            {
                ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
                if((reg_data & RTL9300_I2C_MST1_CTRL1_I2C_TRIG_MASK) == 0)
                    break;
            }
            /* Check Failed or Not*/
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
            if((reg_data & RTL9300_I2C_MST1_CTRL1_I2C_FAIL_MASK) != 0)
            {
                return RT_ERR_FAILED;
            }

            break;
        case I2C_INTF_CONTROLLER_ID1:
            /* Set Trig */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
            reg_data |= RTL9300_I2C_MST2_CTRL1_I2C_TRIG_MASK;
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST2_CTRL1_ADDR, reg_data);
            /* Check Complete or Not */
            while(1)
            {
                ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
                if((reg_data & RTL9300_I2C_MST2_CTRL1_I2C_TRIG_MASK) == 0)
                    break;
            }
            /* Check Failed or Not*/
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
            if((reg_data & RTL9300_I2C_MST2_CTRL1_I2C_FAIL_MASK) != 0)
                return RT_ERR_FAILED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}

int32 r9300_i2c_init(uint32 unit)
{
    int32 ret;

    RT_INIT_REENTRY_CHK(i2c_init[unit]);
    i2c_init[unit] = INIT_COMPLETED;

    ret = drv_software_i2c_module_init(unit);
    if(ret != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

int32 r9300_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev)
{
    uint32  reg_data;
    uint32  mapping_freg;
    int32   ret;
    uint32  i2c_addrwidth, i2c_datawidth;

    if((gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id >= I2C_9300_INTF_CONTROLLER_SDA_END)&&(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id != I2C_INTF_SOFTWARE_DRV_ID))
        return RT_ERR_FAILED;

    switch(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id){
        case I2C_INTF_CONTROLLER_ID0:
            /* Set SCL pin */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
            reg_data |= RTL9300_I2C_MST1_CTRL1_GPIO8_SCL_SEL_MASK;
            /* Set SDA pin*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
            {
                reg_data &= ~RTL9300_INT_I2C_MST1_CTRL1_SDA_OUT_SEL_MASK;
                reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id << RTL9300_INT_I2C_MST1_CTRL1_SDA_OUT_SEL_OFFSET);

            }else{
                reg_data &= ~RTL9300_I2C_MST1_CTRL1_SDA_OUT_SEL_MASK;
                reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id << RTL9300_I2C_MST1_CTRL1_SDA_OUT_SEL_OFFSET);
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST1_CTRL1_ADDR, reg_data);

            /* Set Slave Device Address*/
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL2_ADDR, &reg_data);
            reg_data &= ~RTL9300_I2C_MST1_CTRL2_DEV_ADDR_MASK;
            reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.dev_addr << RTL9300_I2C_MST1_CTRL2_DEV_ADDR_OFFSET);
            /* Set Data Width */
            reg_data &= ~RTL9300_I2C_MST1_CTRL2_DATA_WIDTH_MASK;
            reg_data |= ((gI2C_dev[unit][i2c_dev->device_id].conf.data_width - 1) << RTL9300_I2C_MST1_CTRL2_DATA_WIDTH_OFFSET);
            /* Set Memory Address Width */
            reg_data &= ~RTL9300_I2C_MST1_CTRL2_MEM_ADDR_WIDTH_MASK;
            reg_data |= ((gI2C_dev[unit][i2c_dev->device_id].conf.mem_addr_width) << RTL9300_I2C_MST1_CTRL2_MEM_ADDR_WIDTH_OFFSET);
            /* Set SCL Freq */
            switch(gI2C_dev[unit][i2c_dev->device_id].conf.clk_freq){
                case I2C_CLK_STD_MODE:
                    mapping_freg = 0;
                    break;
                case I2C_CLK_FAST_MODE:
                    mapping_freg = 1;
                    break;
                case I2C_CLK_RT_50K:
                    mapping_freg = 3;
                    break;
                case I2C_CLK_RT_2P5M:
                    mapping_freg = 2;
                    break;
                default:
                    return RT_ERR_FAILED;
            }
            reg_data &= ~RTL9300_I2C_MST1_CTRL2_SCL_FREQ_MASK;
            reg_data |= ((mapping_freg) << RTL9300_I2C_MST1_CTRL2_SCL_FREQ_OFFSET);
            /* Set RD MODE */
            switch(gI2C_dev[unit][i2c_dev->device_id].conf.read_type){
                case I2C_INTF_READ_TYPE_RANDOM:
                    reg_data &= ~RTL9300_INT_I2C_MST1_CTRL2_RD_MODE_MASK;
                    break;
                case I2C_INTF_READ_TYPE_SEQUENTIAL:
                    reg_data |= RTL9300_INT_I2C_MST1_CTRL2_RD_MODE_MASK;
                    break;
                default:
                    return RT_ERR_FAILED;
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST1_CTRL2_ADDR, reg_data);
            break;
        case I2C_INTF_CONTROLLER_ID1:
            /* Set SCL pin */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
            reg_data |= RTL9300_I2C_MST2_CTRL1_GPIO17_SCL_SEL_MASK;
            /* Set SDA pin*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
            {
                reg_data &= ~RTL9300_INT_I2C_MST2_CTRL1_SDA_OUT_SEL_MASK;
                reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id << RTL9300_INT_I2C_MST2_CTRL1_SDA_OUT_SEL_OFFSET);
            }else{
                reg_data &= ~RTL9300_I2C_MST2_CTRL1_SDA_OUT_SEL_MASK;
                reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id << RTL9300_I2C_MST2_CTRL1_SDA_OUT_SEL_OFFSET);
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST2_CTRL1_ADDR, reg_data);
            /* Set Slave Device Address*/
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL2_ADDR, &reg_data);
            reg_data &= ~RTL9300_I2C_MST2_CTRL2_DEV_ADDR_MASK;
            reg_data |= (gI2C_dev[unit][i2c_dev->device_id].conf.dev_addr << RTL9300_I2C_MST2_CTRL2_DEV_ADDR_OFFSET);
            /* Set Data Width */
            reg_data &= ~RTL9300_I2C_MST2_CTRL2_DATA_WIDTH_MASK;
            reg_data |= ((gI2C_dev[unit][i2c_dev->device_id].conf.data_width - 1) << RTL9300_I2C_MST2_CTRL2_DATA_WIDTH_OFFSET);
            /* Set Memory Address Width */
            reg_data &= ~RTL9300_I2C_MST2_CTRL2_MEM_ADDR_WIDTH_MASK;
            reg_data |= ((gI2C_dev[unit][i2c_dev->device_id].conf.mem_addr_width) << RTL9300_I2C_MST2_CTRL2_MEM_ADDR_WIDTH_OFFSET);
            /* Set SCL Freq */
            switch(gI2C_dev[unit][i2c_dev->device_id].conf.clk_freq){
                case I2C_CLK_STD_MODE:
                    mapping_freg = 0;
                    break;
                case I2C_CLK_FAST_MODE:
                    mapping_freg = 1;
                    break;
                case I2C_CLK_RT_50K:
                    mapping_freg = 3;
                    break;
                case I2C_CLK_RT_2P5M:
                    mapping_freg = 2;
                    break;
                default:
                    return RT_ERR_FAILED;
            }
            reg_data &= ~RTL9300_I2C_MST2_CTRL2_SCL_FREQ_MASK;
            reg_data |= ((mapping_freg) << RTL9300_I2C_MST2_CTRL2_SCL_FREQ_OFFSET);
            /* Set RD MODE */
            switch(gI2C_dev[unit][i2c_dev->device_id].conf.read_type){
                case I2C_INTF_READ_TYPE_RANDOM:
                    reg_data &= ~RTL9300_INT_I2C_MST1_CTRL2_RD_MODE_MASK;
                    break;
                case I2C_INTF_READ_TYPE_SEQUENTIAL:
                    reg_data |= RTL9300_INT_I2C_MST1_CTRL2_RD_MODE_MASK;
                    break;
                default:
                    return RT_ERR_FAILED;
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST2_CTRL2_ADDR, reg_data);
            break;
        case I2C_INTF_SOFTWARE_DRV_ID:
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
            break;
        default:
            return RT_ERR_FAILED;
    }

    /* Set Related SDA pin to I2C function*/
    ret = ioal_mem32_read(unit, RTL9300_I2C_MST_GLB_CTRL_ADDR, &reg_data);
    reg_data |= (1 << gI2C_dev[unit][i2c_dev->device_id].conf.sda_pin_id);
    ret = ioal_mem32_write(unit, RTL9300_I2C_MST_GLB_CTRL_ADDR, reg_data);

    return RT_ERR_OK;
}


int32 r9300_i2c_read(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idex, uint8 *pBuff)
{
    uint32  reg_data;
    int32   ret;
    i2c_devConf_t read_dev;

    osal_memcpy(&read_dev, &gI2C_dev[unit][i2c_dev->device_id].conf, sizeof(i2c_devConf_t));
    r9300_i2c_dev_init(unit, &read_dev);

    switch(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id){
        case I2C_INTF_CONTROLLER_ID0:
            /* Set Memory Address */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
            /* Set Read OP*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
            {
                reg_data &= ~RTL9300_INT_I2C_MST1_CTRL1_MEM_ADDR_MASK;
                reg_data |= (reg_idex << RTL9300_INT_I2C_MST1_CTRL1_MEM_ADDR_OFFSET);
                reg_data |= RTL9300_I2C_MST1_CTRL1_RWOP_MASK;
            }else{
                reg_data &= ~RTL9300_I2C_MST1_CTRL1_MEM_ADDR_MASK;
                reg_data |= (reg_idex << RTL9300_I2C_MST1_CTRL1_MEM_ADDR_OFFSET);
                reg_data &= ~RTL9300_I2C_MST1_CTRL1_RWOP_MASK;
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST1_CTRL1_ADDR, reg_data);
            break;
        case I2C_INTF_CONTROLLER_ID1:
            /* Set Memory Address */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
            /* Set Read OP*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
            {
                reg_data &= ~RTL9300_INT_I2C_MST2_CTRL1_MEM_ADDR_MASK;
                reg_data |= (reg_idex << RTL9300_INT_I2C_MST2_CTRL1_MEM_ADDR_OFFSET);
                reg_data |= RTL9300_I2C_MST2_CTRL1_RWOP_MASK;
            }else{
                reg_data &= ~RTL9300_I2C_MST2_CTRL1_MEM_ADDR_MASK;
                reg_data |= (reg_idex << RTL9300_I2C_MST2_CTRL1_MEM_ADDR_OFFSET);
                reg_data &= ~RTL9300_I2C_MST2_CTRL1_RWOP_MASK;
            }
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST2_CTRL1_ADDR, reg_data);
            break;
        case I2C_INTF_SOFTWARE_DRV_ID:
            if((ret = drv_software_i2c_read(reg_idex, pBuff, i2c_dev->device_id)) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
            return RT_ERR_OK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    ret = r9300_i2c_execution(unit, gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id);
    if(ret == RT_ERR_FAILED)
        return ret;

    ret = r9300_data_read(unit, i2c_dev, pBuff);;
    return ret;
}

int32 r9300_i2c_write(uint32 unit, i2c_devConf_t *i2c_dev, uint32 reg_idex, uint8 *pBuff)
{
    uint32  reg_data;
    int32   ret;
    i2c_devConf_t write_dev;

    osal_memcpy(&write_dev, &gI2C_dev[unit][i2c_dev->device_id].conf, sizeof(i2c_devConf_t));
    r9300_i2c_dev_init(unit, &write_dev);

    switch(gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id){
        case I2C_INTF_CONTROLLER_ID0:
            /* Set Memory Address */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST1_CTRL1_ADDR, &reg_data);
            reg_data &= ~RTL9300_I2C_MST1_CTRL1_MEM_ADDR_MASK;
            reg_data |= (reg_idex << RTL9300_I2C_MST1_CTRL1_MEM_ADDR_OFFSET);
            /* Set Write OP*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
                reg_data &= ~RTL9300_I2C_MST1_CTRL1_RWOP_MASK;
            else
                reg_data |= RTL9300_I2C_MST1_CTRL1_RWOP_MASK;
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST1_CTRL1_ADDR, reg_data);
            break;
        case I2C_INTF_CONTROLLER_ID1:
            /* Set Memory Address */
            ret = ioal_mem32_read(unit, RTL9300_I2C_MST2_CTRL1_ADDR, &reg_data);
            reg_data &= ~RTL9300_I2C_MST2_CTRL1_MEM_ADDR_MASK;
            reg_data |= (reg_idex << RTL9300_I2C_MST2_CTRL1_MEM_ADDR_OFFSET);
            /* Set Write OP*/
            if((HWP_CHIP_REV(unit) < 3) && HWP_9300_FAMILY_ID(unit) && IF_CHIP_TYPE_1(unit))
                reg_data &= ~RTL9300_I2C_MST2_CTRL1_RWOP_MASK;
            else
                reg_data |= RTL9300_I2C_MST2_CTRL1_RWOP_MASK;
            ret = ioal_mem32_write(unit, RTL9300_I2C_MST2_CTRL1_ADDR, reg_data);
            break;
        case I2C_INTF_SOFTWARE_DRV_ID:
            if((ret = drv_software_i2c_write(reg_idex, pBuff, i2c_dev->device_id)) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
            return RT_ERR_OK;
            break;
        default:
            return RT_ERR_FAILED;
    }

    ret = r9300_data_write(unit, i2c_dev, pBuff);
    if(ret == RT_ERR_FAILED)
        return ret;

    ret = r9300_i2c_execution(unit, gI2C_dev[unit][i2c_dev->device_id].conf.i2c_interface_id);

    return ret;
}

