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
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
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

#ifndef __BOOTLOADER__
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) && defined(CONFIG_SDK_BSP_SPI_DRV)
extern int drv_register_linux_spi_resource(uint32 unit);
extern int drv_spi_dev_customer_setup(uint32 unit);
#endif
#endif

int32
drv_spiPin_init(uint32 unit, spi_init_info_t * init_info)
{
    int32 ret=RT_ERR_OK;
    RT_PARAM_CHK((unit != HWP_MY_UNIT_ID()), RT_ERR_UNIT_ID);

#if defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2SpiPin_init(unit,init_info);
#endif

    return ret;
}

/* Function Name:
 *      drv_spi_init
 * Description:
 *      Initialize spi module of the specified device.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize spi module before calling any spi APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_spi_init(uint32 unit)
{
    int32 ret=RT_ERR_OK;
#ifndef __BOOTLOADER__
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) && defined(CONFIG_SDK_BSP_SPI_DRV)
    int   check;
#endif
#endif

    RT_INIT_MSG("  SPI init (unit %u) ",unit);

    RT_PARAM_CHK(!HWP_UNIT_VALID_LOCAL(unit), RT_ERR_UNIT_ID);

#if defined(CONFIG_SDK_SPI_DEV_1)
    RT_INIT_MSG("(type1)\n");
    ret = spi_dev1_init(unit);
#elif defined(CONFIG_SDK_SPI_DEV_2)
    RT_INIT_MSG("(type2)\n");
    ret = spi_dev2_init(unit);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    RT_INIT_MSG("(type3)\n");
    ret = spi_dev3_init(unit);
#elif defined(CONFIG_SDK_SPI_DEV_4)
    RT_INIT_MSG("(type4)\n");
    ret = spi_dev4_init(unit);
#elif defined(CONFIG_SDK_SPI_DEV_5)
    RT_INIT_MSG("(type5)\n");
    ret = spi_dev5_init(unit);
#elif defined(CONFIG_SDK_SPI_DEV_NONE)
    RT_INIT_MSG("\n");
#endif

#ifndef __BOOTLOADER__
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) && defined(CONFIG_SDK_BSP_SPI_DRV)
    check = drv_register_linux_spi_resource(unit);
    if(check)
        osal_printf("\n[%s][%d] FAILED!!!\n",__FUNCTION__,__LINE__);
    check = drv_spi_dev_customer_setup(unit);
#endif
#endif
    return ret;

}


/* Function Name:
 *      drv_spi_write
 * Description:
 *      Transmit data via SPI of the specified device.
 * Input:
 *      unit                - unit id
 *      mAddrs              - SPI address
 *      pBuff               - transfer data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_spi_write(uint32 unit, uint32 mAddrs, uint32 *pBuff)
{
    int32 ret=RT_ERR_OK;

#if defined(CONFIG_SDK_SPI_DEV_1)
    spi_cs_t cs = SPI_CS_PIN1;
    ret = spi_dev1Reg_write(unit, cs, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_write(unit, mAddrs, (uint8 *)pBuff +3);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = spi_dev3Reg_write(unit, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = spi_dev4Reg_write(unit, mAddrs, (uint8 *)pBuff +3);
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = spi_dev5Reg_write(unit, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_NONE)
#endif

    return ret;

}


/* Function Name:
 *      drv_spi_read
 * Description:
 *      Start the Receice data from SPI.
 * Input:
 *      unit                - unit id
 *      mAddrs              - SPI address
 * Output:
 *      pBuff               - received data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_spi_read(uint32 unit, uint32 mAddrs, uint32 *pBuff)
{
    int32 ret=RT_ERR_OK;

    *pBuff = 0;

#if defined(CONFIG_SDK_SPI_DEV_1)
    ret = spi_dev1Reg_read(unit, SPI_CS_PIN1, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_read(unit, mAddrs, (uint8 *)pBuff +3);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = spi_dev3Reg_read(unit, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = spi_dev4Reg_read(unit, mAddrs, (uint8 *)pBuff +3);
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = spi_dev5Reg_read(unit, mAddrs, pBuff);
#elif defined(CONFIG_SDK_SPI_DEV_NONE)

#endif

    return ret;
}

/* Function Name:
 *      drv_spi_transfer_start
 * Description:
 *      Start a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8390
 * Note:
 *      Must be called before drv_spi_raw_read/drv_spi_raw_write APIs.
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
drv_spi_transfer_start(uint32 unit)
{
    int32 ret=RT_ERR_OK;

    RT_PARAM_CHK(!HWP_UNIT_VALID_LOCAL(unit), RT_ERR_UNIT_ID);

#if defined(CONFIG_SDK_SPI_DEV_1)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_transfer_start(unit);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_NONE)
#endif
    return ret;

}

/* Function Name:
 *      drv_spi_transfer_stop
 * Description:
 *      Stop a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8390
 * Note:
 *      Must be called after drv_spi_raw_read/drv_spi_raw_write APIs.
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
drv_spi_transfer_stop(uint32 unit)
{
    int32 ret=RT_ERR_OK;

    RT_PARAM_CHK(!HWP_UNIT_VALID_LOCAL(unit), RT_ERR_UNIT_ID);

#if defined(CONFIG_SDK_SPI_DEV_1)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_transfer_stop(unit);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_NONE)
#endif
    return ret;

}

/* Function Name:
 *      drv_spi_raw_read
 * Description:
 *      Process the SPI RAW read process.
 * Input:
 *      unit                - unit id
 *      pBuff               - Store read data
 *      len                 - Read data length
 * Output:
 *      pBuff               - received data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
drv_spi_raw_read(uint32 unit, uint8 *pBuff, uint32 len)
{
    int32 ret=RT_ERR_OK;

#if defined(CONFIG_SDK_SPI_DEV_1)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_raw_read(unit, pBuff, len);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_NONE)

#endif

    return ret;
}

/* Function Name:
 *      drv_spi_raw_write
 * Description:
 *      Process the SPI RAW write process.
 * Input:
 *      unit                - unit id
 *      pBuff               - Write data buffer
 *      len                 - Write data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
drv_spi_raw_write(uint32 unit, uint8 *pBuff, uint32 len)
{
    int32 ret=RT_ERR_OK;

#if defined(CONFIG_SDK_SPI_DEV_1)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_2)
    ret = spi_dev2Reg_raw_write(unit, pBuff, len);
#elif defined(CONFIG_SDK_SPI_DEV_3)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_4)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_5)
    ret = RT_ERR_FAILED;
#elif defined(CONFIG_SDK_SPI_DEV_NONE)

#endif

    return ret;
}

