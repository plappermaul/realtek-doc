/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 87677 $
 * $Date: 2018-05-04 15:38:37 +0800 (Fri, 04 May 2018) $
 *
 * Purpose : Definition of I2C API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) I2C control
 *           (2) I2C read/write
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_i2c.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_LUNA_G3_SERIES)
#include <linux/mutex.h>
extern struct mutex rtkI2cMutex;

#define RTK_I2C_ACCESS_LOCK      mutex_lock(&rtkI2cMutex)
#define RTK_I2C_ACCESS_UNLOCK    mutex_unlock(&rtkI2cMutex)
#else
#include <linux/spinlock.h>
extern spinlock_t rtkI2cSpinLock;
extern unsigned long rtkI2cSpinflags;
#define RTK_I2C_ACCESS_LOCK      spin_lock_irqsave(&rtkI2cSpinLock,rtkI2cSpinflags)
#define RTK_I2C_ACCESS_UNLOCK    spin_unlock_irqrestore(&rtkI2cSpinLock,rtkI2cSpinflags)
#endif
#else
#define RTK_I2C_ACCESS_LOCK
#define RTK_I2C_ACCESS_UNLOCK
#endif

/*
 * Function Declaration
 */
/* Function Name:
 *      rt_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      i2cPort     - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rt_i2c_init(rt_i2c_port_t i2cPort)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_init(rtkI2cPort);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_init */

/* Function Name:
 *      rt_i2c_enable_set
 * Description:
 *      Enable/Disable I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      enable     - enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_enable_set(rt_i2c_port_t i2cPort, rt_enable_t enable)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_enable_set(rtkI2cPort, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_enable_set */

/* Function Name:
 *      rt_i2c_enable_get
 * Description:
 *      Get I2C interface state.
 * Input:
 *      i2cPort   - I2C port interface
 *      pEnable   - the pointer of enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_enable_get(rt_i2c_port_t i2cPort, rt_enable_t *pEnable)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_enable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_enable_get(rtkI2cPort, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_enable_get */

/* Function Name:
 *      rt_i2c_write
 * Description:
 *      I2c random write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_write)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_write(rtkI2cPort, devID, regAddr, data);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_write */

/* Function Name:
 *      rt_i2c_read
 * Description:
 *      I2c random read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_read(rtkI2cPort, devID, regAddr, pData);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_read */

/* Function Name:
 *      rt_i2c_seq_write
 * Description:
 *      I2c sequential random write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - data values to be written
 *      count     - number of sequential write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_seq_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_seq_write)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_seq_write(rtkI2cPort, devID, regAddr, pData, count);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_seq_write */

/* Function Name:
 *      rt_i2c_seq_read
 * Description:
 *      I2c sequential random read data.
 * Input:
 *      i2cPort     - I2C port interface
 *      devID       - the device ID
 *      regAddr     - register address
 *      pData       - the pointer of returned data array
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_seq_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_seq_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_seq_read(rtkI2cPort, devID, regAddr, pData, count);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_i2c_seq_read */

/* Function Name:
 *      rt_i2c_clock_set
 * Description:
 *      Set the clock of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KHz
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_clock_set(rt_i2c_port_t i2cPort, uint32 clock)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_clock_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_clock_set(rtkI2cPort, clock);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_clock_set */

/* Function Name:
 *      rt_i2c_clock_get
 * Description:
 *      Get the clock of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pClock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_clock_get(rt_i2c_port_t i2cPort, uint32 *pClock)
{
    int32   ret;
    rtk_i2c_port_t rtkI2cPort;

    /* function body */
    if (NULL == RT_MAPPER->i2c_clock_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    /* Translate RT data type to RTK data type for DAL */
    switch(i2cPort)
    {
    case RT_I2C_PORT_0:
        rtkI2cPort = I2C_PORT_0;
        break;
    case RT_I2C_PORT_1:
        rtkI2cPort = I2C_PORT_1;
        break;
    case RT_I2C_PORT_END:
    default:
        rtkI2cPort = I2C_PORT_END;
        break;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_clock_get(rtkI2cPort, pClock);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_clock_get */

