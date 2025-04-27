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
 * Purpose : Implementation of the RTL8231 driver
 *
 * Feature : RTL8231 driver
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
#include <hal/chipdef/chip.h>
#include <private/drv/rtl8231/rtl8231.h>
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

/* Function Name:
 *      drv_rtl8231_i2c_read
 * Description:
 *      Read rtl8231 register via MAC indirect access mechanism. (I2C)
 * Input:
 *      unit       - unit id
 *      slave_addr - I2C slave address
 *      reg_addr   - 8231 register address
 * Output:
 *      pData      - pointer buffer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - pData is a null pointer.
 * Note:
 *      None
 */
int32
drv_rtl8231_i2c_read(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = slave_addr;
    dio.data[2] = reg_addr;
    ioctl(fd, RTCORE_RTL8231_I2C_READ, &dio);
    (*pData) = dio.data[3];

    close(fd);

    return dio.ret;
} /* end of drv_rtl8231_i2c_read */

/* Function Name:
 *      drv_rtl8231_i2c_write
 * Description:
 *      Write rtl8231 register via MAC indirect access mechanism. (I2C)
 * Input:
 *      unit       - unit id
 *      slave_addr - I2C slave address
 *      reg_addr   - 8231 register address
 *      data       - configure data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_rtl8231_i2c_write(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = slave_addr;
    dio.data[2] = reg_addr;
    dio.data[3] = data;
    ioctl(fd, RTCORE_RTL8231_I2C_WRITE, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_rtl8231_i2c_write */

/* Function Name:
 *      drv_rtl8231_mdc_read
 * Description:
 *      Read rtl8231 register via MAC indirect access mechanism. (MDC/MDIO)
 * Input:
 *      unit     - unit id
 *      phy_id   - PHY id
 *      page     - PHY page
 *      reg_addr - 8231 register address
 * Output:
 *      pData    - pointer buffer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - pData is a null pointer.
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
int32
drv_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = phy_id;
    dio.data[2] = page;
    dio.data[3] = reg_addr;
    ioctl(fd, RTCORE_RTL8231_MDC_READ, &dio);
    (*pData) = dio.data[4];

    close(fd);

    return dio.ret;
} /* end of drv_rtl8231_mdc_read */

/* Function Name:
 *      drv_rtl8231_mdc_write
 * Description:
 *      Write rtl8231 register via MAC indirect access mechanism. (MDC/MDIO)
 * Input:
 *      unit     - unit id
 *      phy_id   - PHY id
 *      page     - PHY page
 *      reg_addr - 8231 register address
 *      data     - configure data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) valid page as following:
 *      - 0x1D is internal register page
 *      - 0x1E is system register page (default)
 */
int32
drv_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = phy_id;
    dio.data[2] = page;
    dio.data[3] = reg_addr;
    dio.data[4] = data;
    ioctl(fd, RTCORE_RTL8231_MDC_WRITE, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_rtl8231_mdc_write */
