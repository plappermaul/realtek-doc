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
 * Feature : SMI relative API
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
#include <common/debug/rt_log.h>
#include <drv/gpio/gpio.h>
#include <drv/watchdog/watchdog.h>

/*
 * Symbol Definition
 */

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
 *      drv_watchdog_init
 * Description:
 *      Init the watchdog module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_watchdog_init(uint32 unit)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    ioctl(fd, RTCORE_WATCHDOG_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_init */

/* Function Name:
 *      drv_watchdog_scale_set
 * Description:
 *      Set watchdog expired period
 * Input:
 *      unit  - unit id
 *      scale - period scale
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - succeed in setting watchdog expired period.
 *      RT_ERR_FAILED - fail to set watchdog expired period.
 * Note:
 *      None
 */
int32
drv_watchdog_scale_set(uint32 unit, drv_watchdog_scale_t scale)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = (uint32)scale;
    ioctl(fd, RTCORE_WATCHDOG_SCALE_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_scale_set */

/* Function Name:
 *      drv_watchdog_scale_get
 * Description:
 *      Get watchdog expired period scale
 * Input:
 *      unit   - unit id
 * Output:
 *      pScale - period scale
 * Return:
 *      RT_ERR_OK           - get watchdog expired period scale successfully.
 *      RT_ERR_FAILED       - fail to get get watchdog expired period scale.
 *      RT_ERR_NULL_POINTER - pScale is a null pointer.
 * Note:
 *      None
 */
int32
drv_watchdog_scale_get(uint32 unit, drv_watchdog_scale_t *pScale)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    ioctl(fd, RTCORE_WATCHDOG_SCALE_GET, &dio);
    *pScale = (drv_watchdog_scale_t)dio.data[1];

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_scale_get */

/* Function Name:
 *      drv_watchdog_enable_set
 * Description:
 *      Set watchdog enable/disable
 * Input:
 *      unit   - unit id
 *      enable - enable or disable request
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - enable watchdog successfully.
 *      RT_ERR_FAILED - fail to enable watchdog.
 * Note:
 *      None
 */
int32
drv_watchdog_enable_set(uint32 unit, uint32 enable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = (uint32)enable;
    ioctl(fd, RTCORE_WATCHDOG_ENABLE_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_enable_set */

/* Function Name:
 *      drv_watchdog_enable_get
 * Description:
 *      Get watchdog enable/disable status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - watchdog enable/disable status
 * Return:
 *      RT_ERR_OK           - get watchdog enable/disable status successfully.
 *      RT_ERR_NULL_POINTER - pEnable is a null pointer.
 * Note:
 *      None
 */
int32
drv_watchdog_enable_get(uint32 unit, uint32 *pEnable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    ioctl(fd, RTCORE_WATCHDOG_ENABLE_GET, &dio);
    *pEnable = (uint32)dio.data[1];

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_enable_get */

/* Function Name:
 *      drv_watchdog_kick
 * Description:
 *      Kick watchdog
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - kick watchdog successfully.
 * Note:
 *      None
 */
int32
drv_watchdog_kick(uint32 unit)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    ioctl(fd, RTCORE_WATCHDOG_KICK, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_kick */

/* Function Name:
 *      drv_watchdog_threshold_set
 * Description:
 *      Set watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 *      pThreshold - watchdog threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 * Note:
 *      None
 */
int32
drv_watchdog_threshold_set(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = pThreshold->phase_1_threshold;
    dio.data[2] = pThreshold->phase_2_threshold;
    ioctl(fd, RTCORE_WATCHDOG_THRESHOLD_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_threshold_set */

/* Function Name:
 *      drv_watchdog_threshold_get
 * Description:
 *      Get watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 * Output:
 *      pThreshold - watchdog threshold
 * Return:
 *      RT_ERR_OK - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 * Note:
 *      None
 */
int32
drv_watchdog_threshold_get(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    ioctl(fd, RTCORE_WATCHDOG_THRESHOLD_GET, &dio);
    pThreshold->phase_1_threshold = dio.data[1];
    pThreshold->phase_2_threshold = dio.data[2];

    close(fd);

    return dio.ret;
} /* end of drv_watchdog_threshold_get */



