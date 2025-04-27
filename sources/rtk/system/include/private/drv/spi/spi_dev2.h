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

 *
 * Purpose : For APIs which used to access registers of MAX3421E
 *
 * Feature : Inital/Read/Write
 *
 */
#ifndef __DRV_SPI_DEV2_H__
#define __DRV_SPI_DEV2_H__

/* define MAX read/write direction */
#define SPI_DEV2_DIR_WRITE                  (0x2)
#define SPI_DEV2_DIR_READ                   (0x0)

/* Function Name:
 *      spi_dev2Reg_write
 * Description:
 *      Write data to MAX3421E through SPI interface.
 * Input:
 *      unit                - unit id
 *      regAddr             - address of register
 *      pBuff               - data to write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
spi_dev2Reg_write(uint32 unit, uint32 regAddr, uint8 *buff);


/* Function Name:
 *      spi_dev2Reg_read
 * Description:
 *      Read data from MAX3421E through SPI interface.
 * Input:
 *      unit                - unit id
 *      regAddr             - address of register
 *      pBuff               - to store data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
spi_dev2Reg_read(uint32 unit, uint32 regAddr, uint8 *buff);


/* Function Name:
 *      spi_dev2_init
 * Description:
 *      Inital SPI interface which used to access registers of MAX3421E
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
spi_dev2_init(uint32 unit);

int32
spi_dev2SpiPin_init(uint32 unit, spi_init_info_t *init_info);


/* Function Name:
 *      spi_dev2Reg_transfer_start
 * Description:
 *      Start a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_transfer_start(uint32 unit);

/* Function Name:
 *      spi_dev2Reg_transfer_stop
 * Description:
 *      Stop a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_transfer_stop(uint32 unit);

/* Function Name:
 *      spi_dev2Reg_raw_read
 * Description:
 *      Read raw data from device through SPI interface.
 * Input:
 *      unit                - unit id
 *      pBuff               - to store data
 *      len                 - get back data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_raw_read(uint32 unit, uint8 *buff, uint32 len);

/* Function Name:
 *      spi_dev2Reg_raw_write
 * Description:
 *      Write raw data buffer to SPI interface.
 * Input:
 *      unit                - unit id
 *      pBuff               - transfer data
 *      len                 - transfer data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_raw_write(uint32 unit, uint8 *buff, uint32 len);



#endif //#define __DRV_SPI_DEV2_H__


