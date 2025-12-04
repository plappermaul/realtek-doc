/*
 * Copyright (C) 2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : I2C slave access driver
 *
 * Feature : This module provide the I2C slave access behavior to 
 * allow external device access switch/SoC register through I2C
 * interface
 *
 */

#ifdef CONFIG_I2C_SLAVE_ACCESS
/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <hal/mac/reg.h>
#include <rtk/intr.h>
#include <rtk/irq.h>
#include <rtk/i2c.h>

#include <dal/rtl9602c/dal_rtl9602c.h>
#include <ioal/ioal.h>

#define I2C_SLAVE_DEBUG                 (0)

#define I2C_SLAVE_OPERATION_READ        (1<<0)
#define I2C_SLAVE_OPERATION_WRITE       (1<<1)

#if I2C_SLAVE_DEBUG
#define I2C_DEBUG_PRINT printk    
#else
#define I2C_DEBUG_PRINT
#endif

static void i2c_isr_sfp(void)
{
    int ret;
    uint32 value;
    uint32 addr, data;
    uint32 operation;
    
    I2C_DEBUG_PRINT("%s:%d\n", __FILE__, __LINE__);
    reg_read(RTL9602C_SFP_REG_CTRL0r, &value);
    I2C_DEBUG_PRINT("addr: 0x%08x\n", value);
    reg_read(RTL9602C_SFP_REG_CTRL1r, &value);
    I2C_DEBUG_PRINT("wData: 0x%08x\n", value);
    reg_read(RTL9602C_SFP_REG_CTRL2r, &value);
    I2C_DEBUG_PRINT("rData: 0x%08x\n", value);
    reg_field_read(RTL9602C_SFP_REG_CTRL3r, RTL9602C_SFP_REG_IMRf, &value);
    I2C_DEBUG_PRINT("imr: 0x%08x\n", value);
    reg_field_read(RTL9602C_SFP_REG_CTRL3r, RTL9602C_SFP_REG_STSf, &value);
    I2C_DEBUG_PRINT("ims: 0x%08x\n", value);

    operation = value;

    switch(operation)
    {
    case I2C_SLAVE_OPERATION_READ:
        reg_read(RTL9602C_SFP_REG_CTRL0r, &addr);
        I2C_DEBUG_PRINT("rAddr: 0x%08x\n", addr);
        if(addr % 4 == 0)
        {
            data = IOAL_MEM32_READ(addr);
        }
        else
        {
            data = 0x0bad0add;
        }
        I2C_DEBUG_PRINT("rData: 0x%08x\n", data);
        reg_write(RTL9602C_SFP_REG_CTRL2r, &data);
        break;
    case I2C_SLAVE_OPERATION_WRITE:
        reg_read(RTL9602C_SFP_REG_CTRL0r, &addr);
        I2C_DEBUG_PRINT("wAddr: 0x%08x\n", addr);
        if(addr % 4 == 0)
        {
            reg_read(RTL9602C_SFP_REG_CTRL1r, &data);
            I2C_DEBUG_PRINT("wrData: 0x%08x\n", data);
            IOAL_MEM32_WRITE(addr, data);
        }
        break;
    default:
        break;
    }

    /* Clear Command */
    reg_field_write(RTL9602C_SFP_REG_CTRL3r, RTL9602C_SFP_REG_STSf, &value);
    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_SFP)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }
}

int __init i2c_slave_drv_init(void)
{
    int ret;
    uint32 value;

    /* SFP interrupt*/
    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_SFP, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    if (RT_ERR_OK != (ret = rtk_irq_isr_register(INTR_TYPE_SFP, i2c_isr_sfp)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_SFP)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_SFP, ENABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    /* I2C slave configuration */
    value = 1;
    if((ret = reg_field_write(RTL9602C_SFP_REG_CTRL4r, RTL9602C_IICSLV_ENDIANf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    value = 1;
    if((ret = reg_field_write(RTL9602C_SFP_REG_CTRL4r, RTL9602C_IICSLV_STAND_MODEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    value = 1;
    if((ret = reg_field_write(RTL9602C_SFP_REG_CTRL4r, RTL9602C_IICSLV_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    value = 1;
    if((ret = reg_field_write(RTL9602C_IO_MODE_ENr, RTL9602C_I2C_SLV_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    value = 1;
    if((ret = reg_field_write(RTL9602C_SFP_REG_CTRL3r, RTL9602C_SFP_REG_IMRf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    return 0;
}

void __exit i2c_slave_drv_exit(void)
{
    int ret;

    /* SFP interrupt*/
    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_SFP, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ;
    }
    if (RT_ERR_OK != (ret = rtk_irq_isr_unregister(INTR_TYPE_SFP)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ;
    }
    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_SFP)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ;
    }
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek I2C slave access drive module");
MODULE_AUTHOR("Realtek");
module_init(i2c_slave_drv_init);
module_exit(i2c_slave_drv_exit);
#endif

