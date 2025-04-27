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
 * Purpose : Definition of SPI API
 *
 *
 */

/*
 * Include Files
 */
#include <drv/i2c/i2c.h>
#include <private/drv/i2c/i2c_mapper.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      drv_i2c_init
 * Description:
 *      Initialize I2C module.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Must initialize i2c module before calling any i2c APIs.
 */
int32
drv_i2c_init(uint32 unit)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;

    ioctl(fd, RTCORE_I2C_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_i2c_init */


/* Function Name:
 *      drv_i2c_dev_init
 * Description:
 *      Initialize I2C module of the specified device.
 * Input:
 *      unit                - unit id
 *      *i2c_dev         - i2c_devConf_t pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Must initialize i2c module before calling any i2c APIs.
 */
int32
drv_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = i2c_dev->device_id;
    dio.data[2] = i2c_dev->mem_addr_width;
    dio.data[3] = i2c_dev->dev_addr;
    dio.data[4] = i2c_dev->data_width;
    dio.data[5] = i2c_dev->clk_freq;
    dio.data[6] = i2c_dev->scl_delay;
    dio.data[7] = i2c_dev->scl_dev;
    dio.data[8] = i2c_dev->scl_pin_id;
    dio.data[9] = i2c_dev->sda_dev;
    dio.data[10] = i2c_dev->sda_pin_id;
    dio.data[11] = i2c_dev->i2c_interface_id;
    dio.data[12] = i2c_dev->read_type;

    ioctl(fd, RTCORE_I2C_DEV_INIT, &dio);

    close(fd);

    return dio.ret;

} /* end of drv_i2c_dev_init */


/* Function Name:
 *      drv_i2c_write
 * Description:
 *      Transmit data via I2C of the specified device.
 * Input:
 *      unit                - unit id
 *      *i2c_dev         - i2c_devConf_t pointer
 *      reg_idx           - register index
 *      *pBuff            - transfer data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *
 */
int32
drv_i2c_write(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx,uint32 *pBuff)

{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = i2c_dev_id;
    dio.data[2] = reg_idx;
    dio.data[3] = pBuff;

    ioctl(fd, RTCORE_I2C_WRITE, &dio);

    close(fd);

    return dio.ret;

} /* end of drv_i2c_write */


/* Function Name:
 *      drv_i2c_read
 * Description:
 *      Start the Receice data from I2C.
 * Input:
 *      unit                - unit id
 *      *i2c_dev         - i2c_devConf_t pointer
 *      reg_idx           - register index
 * Output:
 *      pBuff               - received data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      8390
 * Note:
 *      None
 */
int32
drv_i2c_read(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint32 *pBuff)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = i2c_dev_id;
    dio.data[2] = reg_idx;
    dio.data[3] = pBuff;

    ioctl(fd, RTCORE_I2C_READ, &dio);

    *pBuff = dio.data[2];

    close(fd);

    return dio.ret;

}

