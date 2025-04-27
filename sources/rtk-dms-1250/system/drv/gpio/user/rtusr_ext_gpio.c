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
 * Feature : GPIO relative API
 *
 */

/*
 * Include Files
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <common/error.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/gpio/ext_gpio.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

int32 drv_extGpio_reg_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = reg;
    ioctl(fd, RTCORE_EXTGPIO_REG_READ, &dio);
    *pData = dio.data[3];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_reg_read */

int32 drv_extGpio_reg_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = reg;
    dio.data[3] = data;
    ioctl(fd, RTCORE_EXTGPIO_REG_WRITE, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_reg_write */

/* Function Name:
 *      drv_extGpio_devReady_get
 * Description:
 *      Get external GPIO ready status
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pIsReady - the device ready status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_devReady_get(uint32 unit, uint32 dev, uint32 *pIsReady)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_DEVREADY_GET, &dio);
    *pIsReady = dio.data[2];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_devReady_get */

/* Function Name:
 *      drv_extGpio_dev_get
 * Description:
 *      Get the external GPIO settings in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 * Output:
 *      pData - initialize configuration data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Get the access mode and relatived information from driver.
 */
int32 drv_extGpio_dev_get(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_DEV_GET, &dio);
    (*pData).access_mode = dio.data[2];
    (*pData).address = dio.data[3];
    (*pData).page = dio.data[4];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_dev_get */

/* Function Name:
 *      drv_extGpio_dev_init
 * Description:
 *      Initialize the external GPIO in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 *      pData - initialize configuration data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Specified the access mode and relatived information to driver.
 */
int32 drv_extGpio_dev_init(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = pData->access_mode;
    dio.data[3] = pData->address;
    dio.data[4] = pData->page;

    ioctl(fd, RTCORE_EXTGPIO_DEV_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_dev_init */

/* Function Name:
 *      drv_extGpio_devEnable_get
 * Description:
 *      Get the external GPIO status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pEnable - the buffer pointer of the status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_devEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_DEVENABLE_GET, &dio);
    (*pEnable) = dio.data[2];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_devEnable_get */

/* Function Name:
 *      drv_extGpio_devEnable_set
 * Description:
 *      Set the external GPIO status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      enable - the status of the specified external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_devEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = enable;
    ioctl(fd, RTCORE_EXTGPIO_DEVENABLE_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_devEnable_set */

/* Function Name:
 *      drv_extGpio_syncEnable_get
 * Description:
 *      Get the external GPIO sync configuration status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pEnable - the buffer pointer of the sync configuration status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_syncEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_SYNCENABLE_GET, &dio);
    (*pEnable) = dio.data[2];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_syncEnable_get */

/* Function Name:
 *      drv_extGpio_syncEnable_set
 * Description:
 *      Set the external GPIO sync configuration status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      enable - the sync configuration status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_syncEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = enable;
    ioctl(fd, RTCORE_EXTGPIO_SYNCENABLE_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_syncEnable_set */

/* Function Name:
 *      drv_extGpio_syncStatus_get
 * Description:
 *      Get the external GPIO sync progress status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pData - the buffer pointer of the sync progress status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      The output value 1 means in progress and 0 means completed.
 */
int32 drv_extGpio_syncStatus_get(uint32 unit, uint32 dev, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_SYNCSTATUS_GET, &dio);
    (*pData) = dio.data[2];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_syncStatus_get */

/* Function Name:
 *      drv_extGpio_sync_start
 * Description:
 *      Trigger the external GPIO sync progress in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_sync_start(uint32 unit, uint32 dev)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_SYNC_START, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_sync_start */

/* Function Name:
 *      drv_extGpio_pin_get
 * Description:
 *      Get the external GPIO pin function settings in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the gpio pin configuration data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_pin_get(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    ioctl(fd, RTCORE_EXTGPIO_PIN_GET, &dio);
    (*pData).direction = dio.data[3];
    (*pData).debounce = dio.data[4];
    (*pData).inverter = dio.data[5];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_pin_get */

/* Function Name:
 *      drv_extGpio_pin_init
 * Description:
 *      Initialize the external GPIO pin function in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      pData  - the gpio pin configuration data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_pin_init(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    dio.data[3] = pData->direction;
    dio.data[4] = pData->debounce;
    dio.data[5] = pData->inverter;
    ioctl(fd, RTCORE_EXTGPIO_PIN_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_pin_init */

/* Function Name:
 *      drv_extGpio_syncEnable_get
 * Description:
 *      Get the external GPIO pin value in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the buffer pointer of the gpio pin value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_dataBit_get(uint32 unit, uint32 dev, uint32 gpioId, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    ioctl(fd, RTCORE_EXTGPIO_DATABIT_GET, &dio);
    *pData = dio.data[3];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_dataBit_get */

/* Function Name:
 *      drv_extGpio_dataBit_set
 * Description:
 *      Set the external GPIO pin value in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      data  - the gpio pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_dataBit_set(uint32 unit, uint32 dev, uint32 gpioId, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    dio.data[3] = data;
    ioctl(fd, RTCORE_EXTGPIO_DATABIT_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_dataBit_set */

/* Function Name:
 *      drv_extGpio_devRecovery_start
 * Description:
 *      Recovery the external GPIO status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 drv_extGpio_devRecovery_start(uint32 unit, uint32 dev)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    ioctl(fd, RTCORE_EXTGPIO_DEVRECOVERY_START, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_devRecovery_start */

/* Function Name:
 *      drv_extGpio_direction_get
 * Description:
 *      Get the external GPIO pin direction in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the buffer pointer of the gpio pin direction
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_extGpio_direction_get(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    ioctl(fd, RTCORE_EXTGPIO_DIRECTION_GET, &dio);
    *pData = dio.data[3];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_direction_get */

/* Function Name:
 *      drv_extGpio_direction_set
 * Description:
 *      Set the external GPIO pin direction in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      data  - the gpio pin direction
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_extGpio_direction_set(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = gpioId;
    dio.data[3] = data;
    ioctl(fd, RTCORE_EXTGPIO_DIRECTION_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_direction_set */

/* Function Name:
 *      drv_extGpio_i2c_init
 * Description:
 *      Initialize the SCK/SDA in external GPIO pin in the specified device of the unit
 * Input:
 *      unit      - unit id
 *      dev       - external GPIO dev id
 *      i2c_clock - i2c SCK pin in external GPIO device
 *      i2c_data  - i2c SDA pin in external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_extGpio_i2c_init(uint32 unit, uint32 dev, uint32 i2c_clock, uint32 i2c_data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = i2c_clock;
    dio.data[3] = i2c_data;
    ioctl(fd, RTCORE_EXTGPIO_I2C_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_i2c_init */

/* Function Name:
 *      drv_extGpio_i2c_read
 * Description:
 *      Read the value of register in external GPIO pin in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 *      reg   - register to read
 * Output:
 *      pData - buffer pointer of data value in external GPIO device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_extGpio_i2c_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = reg;
    ioctl(fd, RTCORE_EXTGPIO_I2C_READ, &dio);
    *pData = dio.data[3];

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_i2c_read */

/* Function Name:
 *      drv_extGpio_i2c_write
 * Description:
 *      Write the value of register in external GPIO pin in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 *      reg  - register to read
 *      data - data value in external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_extGpio_i2c_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = reg;
    dio.data[3] = data;
    ioctl(fd, RTCORE_EXTGPIO_I2C_WRITE, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_extGpio_i2c_write */
