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
 * Feature : The file includes the following modules and sub-modules
 *           (1) SPI driver

 */

#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

/*
 * Include Files
 */
#if defined(CONFIG_SDK_SPI_DEV_1)
  #include <private/drv/spi/spi_dev1.h>
#elif defined(CONFIG_SDK_SPI_DEV_2)
  #include <private/drv/spi/spi_private.h>
  #include <private/drv/spi/spi_dev2.h>
#elif defined(CONFIG_SDK_SPI_DEV_3)
  #include <private/drv/spi/spi_dev3.h>
#elif defined(CONFIG_SDK_SPI_DEV_4)
  #include <private/drv/spi/spi_dev4.h>
#elif defined(CONFIG_SDK_SPI_DEV_5)
  #include <private/drv/spi/spi_dev5.h>
#elif defined(CONFIG_SDK_SPI_DEV_NONE)

#endif


/*
 * Symbol Definition
 */
#define RT_SPI_USE_FLASH_SEMA   1       /* 1: Using same semaphore with FLASH driver; 0: no */


/*
 * Data Declaration
 */
typedef enum spi_cs_e{
    SPI_CS_PIN0 = 0,    /* chip select 0 */
    SPI_CS_PIN1,        /* chip select 0 */
    SPI_CS_PIN2,        /* chip select 0 */
    SPI_CS_PIN3,        /* chip select 0 */
    SPI_CS_TEST,        /* for debug purpose */
}spi_cs_t;

typedef enum spi_clk_e{
    SPI_CLK_1M = 0,         /* 1     MHz */
    SPI_CLK_1_25M,          /* 1.25  MHz */
    SPI_CLK_1_875M,         /* 1.875 MHz */
    SPI_CLK_2M,             /* 2     MHz */
    SPI_CLK_4_69M,          /* 4.69  MHz */
    SPI_CLK_9_325M,         /* 9.325 MHz */
    SPI_CLK_12_5M,          /* 12.5  MHz */
    SPI_CLK_18_75M,         /* 18.75 MHz */
    SPI_CLK_25M,            /* 25    MHz */
    SPI_CLK_37_5M,          /* 37.5  MHz */
    SPI_CLK_40M,            /* 40    MHz */
    SPI_CLK_50M,            /* 50    MHz */
    SPI_CLK_66_7M,          /* 66.7  MHz */
}spi_clk_t;

typedef struct spi_para_s{
    uint32 spi_clk;
    uint32 system_clk;
    uint32 extra_mdelay;
    uint32 clk_regVal;
}spi_para_t;
/*
 * Function Declaration
 */


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
drv_spi_init(uint32 unit);


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
drv_spi_write(uint32 unit, uint32 mAddrs, uint32 *pBuff);


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
drv_spi_read(uint32 unit, uint32 mAddrs, uint32 *pBuff);

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
drv_spi_transfer_start(uint32 unit);

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
drv_spi_transfer_stop(uint32 unit);

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
drv_spi_raw_read(uint32 unit, uint8 *pBuff, uint32 len);

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
drv_spi_raw_write(uint32 unit, uint8 *pBuff, uint32 len);

#endif /* __DRV_SPI_H__ */

