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
#ifndef __DRV_SOFTWARE_I2C_H__
#define __DRV_SOFTWARE_I2C_H__

/*
* Include Files
*/
#include <common/rt_error.h>

/*
* Symbol Definition
*/
#if defined(__BOOTLOADER__)
#define SOFTWARE_I2C_MAX_DEV_NAME            16
#endif /* defined(__BOOTLOADER__) */


/* SOFTWARE I2C devices which are accessed through generalCtrl GPIO */
enum SOFTWARE_I2C_DEVICE
{
    SOFTWARE_I2C_DEVICE_SFP1 = 0,
    SOFTWARE_I2C_DEVICE_SFP2,
    SOFTWARE_I2C_DEVICE_SFP3,
    SOFTWARE_I2C_DEVICE_SFP4,
    SOFTWARE_I2C_DEVICE_SFP5,
    SOFTWARE_I2C_DEVICE_NONE6,
    SOFTWARE_I2C_DEVICE_NONE7,
    SOFTWARE_I2C_DEVICE_NONE8,
    SOFTWARE_I2C_DEVICE_NONE9,
    SOFTWARE_I2C_DEVICE_NONE10,
    SOFTWARE_I2C_DEVICE_NONE11,
    SOFTWARE_I2C_DEVICE_NONE12,
    SOFTWARE_I2C_DEVICE_NONE13,
    SOFTWARE_I2C_DEVICE_NONE14,
    SOFTWARE_I2C_DEVICE_NONE15,
    SOFTWARE_I2C_DEVICE_MAX
};

enum SOFTWARE_I2C_TYPE
{
    SOFTWARE_I2C_TYPE_8BITS_DEV =0,
    SOFTWARE_I2C_TYPE_16BITS_DEV,
    SOFTWARE_I2C_TYPE_MAX
};

enum SOFTWARE_I2C_ADDRWIDTH
{
    SOFTWARE_I2C_ADDR_WIDTH_BYTE_0 = 0,
    SOFTWARE_I2C_ADDR_WIDTH_BYTE_1,
    SOFTWARE_I2C_ADDR_WIDTH_BYTE_2,
    SOFTWARE_I2C_ADDR_WIDTH_BYTE_3,
    SOFTWARE_I2C_ADDR_WIDTH_BYTE_END
};

enum SOFTWARE_I2C_DATAWIDTH
{
    SOFTWARE_I2C_DATA_WIDTH_BYTE_1 = 0,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_2,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_3,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_4,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_5,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_6,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_7,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_8,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_9,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_10,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_11,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_12,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_13,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_14,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_15,
    SOFTWARE_I2C_DATA_WIDTH_BYTE_END
};

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define SOFTWARE_I2C_DEVICE_CHK(dev)     (dev < SOFTWARE_I2C_DEVICE_MAX)

/*
 * Function Declaration
 */


/* Function Name:
 *      drv_software_i2c_type_set
 * Description:
 *      SOFTWARE I2C device type set function
 * Input:
 *      software_i2c_addrWidth  - device address field width
 *      software_i2c_dataWidth  - device data field width
 *      chipid                  - chip id
 *      delay                   - delay
 *      dev                     - device id
 *      name                    - dev name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_INPUT
 * Note:
 *      None
 */

int32 drv_software_i2c_type_set(uint32 software_i2c_addrWidth, uint32 software_i2c_dataWidth, uint32 chipid, uint32 delay, uint32 dev);
/* Function Name:
 *      drv_software_i2c_type_get
 * Description:
 *      SOFTWARE_I2C init function
 * Input:
 *      dev         - device id
 * Output:
 *      paddrWidth  - device address field width
 *      pdataWidth  - device data field width
 *      pchipid     - chip id
 *      pdelay      - delay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 drv_software_i2c_type_get(uint32 * paddrWidth, uint32 * pdataWidth, uint32 * pchipid, uint32 * pdelay, uint32 dev);


/* Function Name:
 *      drv_software_i2c_group_get
 * Description:
 *      SOFTWARE_I2C configuration get function
 * Input:
 *      dev     - dev id
 * Output:
 *      pDevSCK - SCK device id
 *      pinSCK  - SCK pin
 *      pDevDA - SDA device id
 *      pinSDA  - SDA pin
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 drv_software_i2c_group_get(uint32 * pDevSCK, uint32 * pPinSCK, uint32 * pDevSDA, uint32 * pPinSDA, uint32 dev);

/* Function Name:
 *      drv_software_i2c_init
 * Description:
 *      SOFTWARE I2C init function
 * Input:
 *      devSCK - SCK GPIO dev id
 *      pinSCK  - SCK pin
 *      devSDA - SDA GPIO dev id
 *      pinSDA  - SDA pin
 *      dev     - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 drv_software_i2c_init(uint32 devSCK, uint32 pinSCK, uint32 devSDA, uint32 pinSDA, uint32 dev);

/* Function Name:
 *      drv_software_i2c_write
 * Description:
 *      SOFTWARE I2C write wrapper function
 * Input:
 *      mAddrs  - address
 *      pWdata  - data write
 *      dev     - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 drv_software_i2c_write(uint32 mAddrs, uint8 *pWdata, uint32 dev);

/* Function Name:
 *      drv_software_i2c_read
 * Description:
 *      SOFTWARE I2C random read wrapper function
 * Input:
 *      mAddrs  - address
 *      dev     - dev id
 * Output:
 *      pRdata  - data read
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 drv_software_i2c_read(uint32 mAddrs, uint8 *pRdata, uint32 dev);

/* Function Name:
 *      drv_software_i2c_sequential_read
 * Description:
 *      SOFTWARE I2C sequential read wrapper function
 * Input:
 *      mAddrs  - address
 *      dev       - dev id
 * Output:
 *      pRdata  - data read
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 drv_software_i2c_sequential_read(uint32 mAddrs, uint8 *pRdata, uint32 dev);


/* Function Name:
 *      drv_software_i2c_module_init
 * Description:
 *      SOFTWARE I2C init function
 * Input:
 *      unit	- unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 drv_software_i2c_module_init(uint32 unit);

#if defined(__BOOTLOADER__)
/* Function Name:
 *      drv_software_i2c_list
 * Description:
 *      List all initialed devices
 * Input:
 *		None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void drv_software_i2c_list(void);
#endif /* defined(__BOOTLOADER__) */

#endif /* __DRV_SOFTWARE_I2C_H__ */

