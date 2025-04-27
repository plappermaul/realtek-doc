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

#ifndef __DRV_SPI_PRIVATE_H__
#define __DRV_SPI_PRIVATE_H__

/*
 * Data Declaration
 */
typedef struct spi_init_info_s{
    int gpioNum_sclk;
    int gpioNum_mosi;
    int gpioNum_miso;
    int gpioNum_ss;
    int gpioNum_reset;
    int gpioNum_int;
}spi_init_info_t;


/* Function Name:
 *      drv_spiPin_init
 * Description:
 *      Use to define GPIO pin number for SPI interface each pin
 * Input:
 *      unit                - unit id
 *      spi_init_info_t     - pin number definiton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_spiPin_init(uint32 unit, spi_init_info_t * init_info);


#endif /* __DRV_SPI_PRIVATE_H__ */

