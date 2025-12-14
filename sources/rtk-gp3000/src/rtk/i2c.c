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
 * $Revision$
 * $Date$
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
#include <rtk/i2c.h>
#include <dal/dal_mgmt.h>

/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_LUNA_G3_SERIES)
#include <linux/mutex.h>
DEFINE_MUTEX(rtkI2cMutex);

#define RTK_I2C_ACCESS_LOCK      mutex_lock(&rtkI2cMutex)
#define RTK_I2C_ACCESS_UNLOCK    mutex_unlock(&rtkI2cMutex)
#else
#include <linux/spinlock.h>
#if defined(CONFIG_KERNEL_2_6_30)
spinlock_t rtkI2cSpinLock = SPIN_LOCK_UNLOCKED;
#else
DEFINE_SPINLOCK(rtkI2cSpinLock);
#endif
unsigned long rtkI2cSpinflags;

#define RTK_I2C_ACCESS_LOCK      spin_lock_irqsave(&rtkI2cSpinLock,rtkI2cSpinflags)
#define RTK_I2C_ACCESS_UNLOCK    spin_unlock_irqrestore(&rtkI2cSpinLock,rtkI2cSpinflags)
#endif
#else
#define RTK_I2C_ACCESS_LOCK
#define RTK_I2C_ACCESS_UNLOCK
#endif

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_i2c_init
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
rtk_i2c_init(rtk_i2c_port_t i2cPort)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_init(i2cPort);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rtk_i2c_init */

/* Function Name:
 *      rtk_i2c_enable_set
 * Description:
 *      Enable/Disable I2C interface.
 * Input:
 *      i2cPort     - I2C port interface
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
rtk_i2c_enable_set(rtk_i2c_port_t i2cPort, rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_enable_set(i2cPort, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_enable_set */

/* Function Name:
 *      rtk_i2c_enable_get
 * Description:
 *      Get I2C interface state.
 * Input:
 *      i2cPort     - I2C port interface
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
rtk_i2c_enable_get(rtk_i2c_port_t i2cPort, rtk_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_enable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_enable_get(i2cPort, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_enable_get */

/* Function Name:
 *      rtk_i2c_width_set
 * Description:
 *      Set the data and address width of I2C interface.
 * Input:
 *      i2cPort     - I2C port interface
 *      width     - 8-bit or 16-bit
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
rtk_i2c_width_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_width_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_width_set(i2cPort, width);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_width_set */

/* Function Name:
 *      rtk_i2c_width_get
 * Description:
 *      Get the data and address width of I2C interface.
 * Input:
 *      i2cPort     - I2C port interface
 *      pWidth     - the pointer of width
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
rtk_i2c_width_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_width_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_width_get(i2cPort, pWidth);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_width_get */

/* Function Name:
 *      rtk_i2c_write
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
rtk_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_write(i2cPort, devID, regAddr, data);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_write */

/* Function Name:
 *      rtk_i2c_seq_write
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
rtk_i2c_seq_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_seq_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_seq_write(i2cPort, devID, regAddr, pData, count);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_seq_write */

/* Function Name:
 *      rtk_i2c_noWait_write
 * Description:
 *      I2c random write data without write complete check. (Won't return write error)
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
 *      This write API is for some special applications which are time senstive.
 * For most of the application, use rtk_i2c_write instead. Only valid for some
 * ASIC (RTL9601B)
 */
int32
rtk_i2c_noWait_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_noWait_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_noWait_write(i2cPort, devID, regAddr, data);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_noWait_write */

/* Function Name:
 *      rtk_i2c_read
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
rtk_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_read(i2cPort, devID, regAddr, pData);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_read */

/* Function Name:
 *      rtk_i2c_seq_read
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
rtk_i2c_seq_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_seq_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_seq_read(i2cPort, devID, regAddr, pData, count);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_seq_read */

/* Function Name:
 *      rtk_i2c_curr_read
 * Description:
 *      I2c current read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
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
rtk_i2c_curr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_curr_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_curr_read(i2cPort, devID, pData);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_curr_read */

/* Function Name:
 *      rtk_i2c_seqCurr_read
 * Description:
 *      I2c sequential current read data.
 * Input:
 *      i2cPort     - I2C port interface
 *      devID       - the device ID
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
rtk_i2c_seqCurr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData, uint32 count)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_seqCurr_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_seqCurr_read(i2cPort, devID, pData, count);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_seqCurr_read */

/* Function Name:
 *      rtk_i2c_clock_set
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
rtk_i2c_clock_set(rtk_i2c_port_t i2cPort, uint32 clock)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_clock_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_clock_set(i2cPort, clock);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_clock_set */

/* Function Name:
 *      rtk_i2c_clock_get
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
rtk_i2c_clock_get(rtk_i2c_port_t i2cPort, uint32 *pClock)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_clock_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_clock_get(i2cPort, pClock);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_clock_get */

/* Function Name:
 *      rtk_i2c_eepMirror_set
 * Description:
 *      Configure EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0¡A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rtk_i2c_eepMirror_set(rtk_i2c_eeprom_mirror_t eepMirrorCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_eepMirror_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_eepMirror_set(eepMirrorCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_eepMirror_set */

/* Function Name:
 *      rtk_i2c_eepMirror_get
 * Description:
 *      Configure EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0¡A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rtk_i2c_eepMirror_get(rtk_i2c_eeprom_mirror_t *pEepMirrorCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_eepMirror_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_eepMirror_get(pEepMirrorCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_eepMirror_get */

/* Function Name:
 *      rtk_i2c_eepMirror_write
 * Description:
 *      I2c EEPROM mirror write data.
 * Input:
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
rtk_i2c_eepMirror_write(uint32 regAddr, uint32 data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_eepMirror_write)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_eepMirror_write(regAddr, data);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_eepMirror_write */

/* Function Name:
 *      rtk_i2c_eepMirror_read
 * Description:
 *      I2c read data from EEPROM mirror.
 * Input:
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
rtk_i2c_eepMirror_read(uint32 regAddr, uint32 *pData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_eepMirror_read)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    RTK_I2C_ACCESS_LOCK;
    ret = RT_MAPPER->i2c_eepMirror_read(regAddr, pData);
    RTK_I2C_ACCESS_UNLOCK;
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_eepMirror_read */

/* Function Name:
 *      rtk_i2c_dataWidth_set
 * Description:
 *      Set the data width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
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
rtk_i2c_dataWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_dataWidth_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_dataWidth_set(i2cPort, width);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_dataWidth_set */

/* Function Name:
 *      rtk_i2c_dataWidth_get
 * Description:
 *      Get the data width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
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
rtk_i2c_dataWidth_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_dataWidth_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_dataWidth_get(i2cPort, pWidth);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_dataWidth_get */



/* Function Name:
 *      rtk_i2c_addrWidth_set
 * Description:
 *      Set the address width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
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
rtk_i2c_addrWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_addrWidth_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_addrWidth_set(i2cPort, width);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_addrWidth_set */

/* Function Name:
 *      rtk_i2c_addrWidth_get
 * Description:
 *      Get the address width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
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
rtk_i2c_addrWidth_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->i2c_addrWidth_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_addrWidth_get(i2cPort, pWidth);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_i2c_addrWidth_get */
