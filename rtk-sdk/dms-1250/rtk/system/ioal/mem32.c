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
 * $Revision: 100997 $
 * $Date: 2019-10-15 08:44:21 +0800 (Tue, 15 Oct 2019) $
 *
 * Purpose : I/O read/write APIs in the SDK.
 *
 * Feature : I/O read/write APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#ifdef CONFIG_X86_I2C
#include <x86_i2c/x86_i2c.h>
#endif
#include <common/error.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <common/debug/rt_log.h>
#include <drv/spi/spi.h>
#include <hwp/hw_profile.h>

#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif  /* CONFIG_SDK_DRIVER_I2C */

#if defined(CONFIG_SDK_RTL9310) /* remote access */
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
ioal_mem32_read_t  hra_read_cb=NULL;
ioal_mem32_write_t hra_write_cb=NULL;



/*
 * Macro Declaration
 */
#define INTERFACE_SPI(unit)     (HWP_SWCORE_ACC_METHOD(unit) == HWP_SW_ACC_SPI)
#define INTERFACE_MEM(unit)     (HWP_SWCORE_ACC_METHOD(unit) == HWP_SW_ACC_MEM)
#define INTERFACE_PCIe(unit)    (HWP_SWCORE_ACC_METHOD(unit) == HWP_SW_ACC_PCIe)
#define INTERFACE_I2C(unit)     (HWP_SWCORE_ACC_METHOD(unit) == HWP_SW_ACC_I2C)
#define INTERFACE_REMOTE(unit)  (1)


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_mem32_hraCb_register
 * Description:
 *      Register callback function of Hardware Remote Access API
 * Input:
 *      unit        - unit id
 *      readWrite   - Read function or Write function
 *      func        - callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid
 *      RT_ERR_FAILED - invalid
 * Note:
 *      None
 */
int32
ioal_mem32_hraCb_register(uint32 unit, ioal_cb_readWRite_t readWrite, void *func)
{
    if( HWP_MY_UNIT_ID() != unit)
        return RT_ERR_FAILED;

    switch(readWrite)
    {
        case IOAL_CB_READ:
            hra_read_cb = (ioal_mem32_read_t)func;
            break;
        case IOAL_CB_WRITE:
            hra_write_cb = (ioal_mem32_write_t)func;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      ioal_mem32_check
 * Description:
 *      Check the register address is valid or not for the specified chip.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid
 *      RT_ERR_FAILED - invalid
 * Note:
 *  1. The addr value should be offset address from the chip base in multiple chip.
 *  2. For some single chip solution, it maybe input physical address value.
 */
int32
ioal_mem32_check(uint32 unit, uintptr *addr)
{
    uintptr base;
    int32 ret = RT_ERR_FAILED;

    /* Upper layer have check the unit, and don't need to check again */

    RT_ERR_CHK(ioal_init_memRegion_get(unit, IOAL_MEM_SWCORE, &base), ret);
    *addr += base;

    return RT_ERR_OK;
} /* end of ioal_mem32_check */


/* Function Name:
 *      ioal_mem32_read
 * Description:
 *      Get the value from register.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
ioal_mem32_read(uint32 unit, uintptr addr, uint32 *pVal)
{
    int32 ret = RT_ERR_FAILED;

    if(HWP_NOT_INIT_YET() || HWP_USEDEFHWP() || INTERFACE_MEM(unit))
    {
        RT_ERR_CHK(ioal_mem32_check(unit, &addr), ret);
        *pVal = MEM32_READ(addr);
    }
    else if (INTERFACE_PCIe(unit))
    {
        RT_ERR_CHK(ioal_mem32_check(unit, &addr), ret);
        *pVal = PCI_MEM32_READ(addr);
    }
    else if (INTERFACE_SPI(unit))
    {
  #if defined(CONFIG_SDK_DRIVER_SPI)
        ret = drv_spi_read(unit, addr, pVal);
  #endif
    }
    else if(INTERFACE_I2C(unit))
    {
#if defined(CONFIG_X86_I2C)
        RT_ERR_CHK(x86_i2c_read(unit, (addr & 0xFFFF), pVal), ret);
#else
  #if defined(CONFIG_SDK_DRIVER_I2C)
        I2C_READ_WORD(HWP_MY_UNIT_ID(), 0, (addr & 0xFFFF), pVal, ret);
  #endif
#endif
    }
    else if (INTERFACE_REMOTE(unit))
    {
        if( NULL != hra_read_cb)
            ret = hra_read_cb(unit, addr, pVal);
    }
    else
    {
        ret = RT_ERR_FAILED;
    }

    return ret;
} /* end of ioal_mem32_read */


/* Function Name:
 *      ioal_mem32_write
 * Description:
 *      Set the value to register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32 ioal_mem32_write(uint32 unit, uintptr addr, uint32 val)
{
    int32 ret = RT_ERR_FAILED;


    if(HWP_NOT_INIT_YET() || HWP_USEDEFHWP() || INTERFACE_MEM(unit))
    {
        /* Upper layer have check the unit, and don't need to check again */
        RT_ERR_CHK(ioal_mem32_check(unit, &addr), ret);
        MEM32_WRITE(addr, val);
    }
    else if (INTERFACE_PCIe(unit))
    {
        RT_ERR_CHK(ioal_mem32_check(unit, &addr), ret);
        PCI_MEM32_WRITE(addr, val);
    }
    else if (INTERFACE_SPI(unit))
    {
  #if defined(CONFIG_SDK_DRIVER_SPI)
        ret = drv_spi_write(unit, addr, &val);
  #endif
    }
    else if(INTERFACE_I2C(unit))
    {
#if defined(CONFIG_X86_I2C)
        RT_ERR_CHK(x86_i2c_write(unit, (addr & 0xFFFF), &val), ret);
#else
  #if defined(CONFIG_SDK_DRIVER_I2C)
        I2C_WRITE_WORD(HWP_MY_UNIT_ID(), 0, (addr & 0xFFFF), &val, ret);
  #endif
#endif
    }
    else if (INTERFACE_REMOTE(unit))
    {
        if( NULL != hra_write_cb)
            ret = hra_write_cb(unit, addr, val);
    }
    else
    {
        ret = RT_ERR_FAILED;
    }

    return ret;
} /* end of ioal_mem32_write */

/* Function Name:
 *      ioal_mem32_field_read
 * Description:
 *      Read the value from the field of register.
 * Input:
 *      unit   - unit id
 *      addr   - register address
 *      offset - field offset
 *      mask   - field mask
 * Output:
 *      pVal - pointer buffer of the register field value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_mem32_field_read(uint32 unit, uintptr addr, uint32 offset, uint32 mask, uint32 *pVal)
{
    uint32  tmp;
    int32   ret = RT_ERR_FAILED;

    RT_ERR_CHK(ioal_mem32_read(unit, addr, &tmp), ret);
    *pVal = (tmp & mask) >> offset;

    return RT_ERR_OK;
} /* end of ioal_mem32_field_read */

/* Function Name:
 *      ioal_mem32_field_write
 * Description:
 *      Write the value to the field of register.
 * Input:
 *      unit   - unit id
 *      addr   - register address
 *      offset - field offset
 *      mask   - field mask
 *      val    - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_mem32_field_write(uint32 unit, uintptr addr, uint32 offset, uint32 mask, uint32 val)
{
    uint32  tmp;
    int32   ret = RT_ERR_FAILED;

    RT_ERR_CHK(ioal_mem32_read(unit, addr, &tmp), ret);
    tmp = (tmp & ~mask) | ((val << offset) & mask);
    RT_ERR_CHK(ioal_mem32_write(unit, addr, tmp), ret);

    return RT_ERR_OK;
} /* end of ioal_mem32_field_write */

/* Function Name:
 *      ioal_soc_mem32_read
 * Description:
 *      Get the value from register of SoC
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
ioal_soc_mem32_read(uint32 unit, uintptr addr, uint32 *pVal)
{
    uintptr baseAddr;

    ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &baseAddr);
    *pVal = MEM32_READ(baseAddr+addr);

    return RT_ERR_OK;
}

/* Function Name:
 *      ioal_soc_mem32_write
 * Description:
 *      Set the value to SoC related register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
ioal_soc_mem32_write(uint32 unit, uintptr addr, uint32 val)
{
    uintptr baseAddr;

    ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &baseAddr);
    MEM32_WRITE(baseAddr+addr, val);

    return RT_ERR_OK;
}
