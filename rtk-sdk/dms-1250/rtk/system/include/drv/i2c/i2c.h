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
 * Purpose : Definition of I2C API
 *
 * Feature : Init, set and get I2C devices
 *
 */

#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

/*
 * Include Files
 */
#include <drv/gpio/generalCtrl_gpio.h>

/*
 * Symbol Definition
 */

#define I2C_DATA_WIDTH_MAX_LEN      16

typedef enum drv_i2c_devId_e
{
    I2C_DEV_ID0 = 0,
    I2C_DEV_ID1,
    I2C_DEV_ID2,
    I2C_DEV_ID3,
    I2C_DEV_ID4,
    I2C_DEV_ID5,
    I2C_DEV_ID6,
    I2C_DEV_ID7,
    I2C_DEV_ID8,
    I2C_DEV_ID9,
    I2C_DEV_ID10,
    I2C_DEV_ID11,
    I2C_DEV_ID12,
    I2C_DEV_ID13,
    I2C_DEV_ID14,
    I2C_DEV_ID15,
    I2C_DEV_ID_END
} drv_i2c_devId_t;


typedef enum drv_i2c_clk_e
{
    I2C_CLK_STD_MODE = 0,           /* 100KHZ */
    I2C_CLK_FAST_MODE,              /* 400KHZ */
    I2C_CLK_RT_50K,                 /* 50KHZ */
    I2C_CLK_RT_2P5M,                /* 2.5MHZ */
    I2C_CLK_END
} drv_i2c_clk_t;


typedef enum drv_i2c_addrWidth_e
{
    I2C_ADDR_WIDTH_BYTE_0 = 0,
    I2C_ADDR_WIDTH_BYTE_1,
    I2C_ADDR_WIDTH_BYTE_2,
    I2C_ADDR_WIDTH_BYTE_3,
    I2C_ADDR_WIDTH_BYTE_END
} drv_i2c_addrWidth_t;

typedef enum drv_i2c_dataWidth_e
{
    I2C_DATA_WIDTH_BYTE_0 = 0,
    I2C_DATA_WIDTH_BYTE_1,
    I2C_DATA_WIDTH_BYTE_2,
    I2C_DATA_WIDTH_BYTE_3,
    I2C_DATA_WIDTH_BYTE_4,
    I2C_DATA_WIDTH_BYTE_5,
    I2C_DATA_WIDTH_BYTE_6,
    I2C_DATA_WIDTH_BYTE_7,
    I2C_DATA_WIDTH_BYTE_8,
    I2C_DATA_WIDTH_BYTE_9,
    I2C_DATA_WIDTH_BYTE_10,
    I2C_DATA_WIDTH_BYTE_11,
    I2C_DATA_WIDTH_BYTE_12,
    I2C_DATA_WIDTH_BYTE_13,
    I2C_DATA_WIDTH_BYTE_14,
    I2C_DATA_WIDTH_BYTE_15,
    I2C_DATA_WIDTH_BYTE_16,
    I2C_DATA_WIDTH_BYTE_END
} drv_i2c_dataWidth_t;


typedef enum drv_i2c_interface_e
{
    I2C_INTF_CONTROLLER_ID0 = 0,
    I2C_INTF_CONTROLLER_ID1,
    I2C_INTF_SOFTWARE_DRV_ID,           /* GPIO simulates the I2C interface */
    I2C_INTF_END
} drv_i2c_interfaceId_t;


typedef enum drv_i2c_read_type_e
{
    I2C_INTF_READ_TYPE_RANDOM = 0,      /* Use Random Read */
    I2C_INTF_READ_TYPE_SEQUENTIAL,      /* Use Sequential Read */
    I2C_INTF_READ_TYPE_END
} drv_i2c_read_type_t;


typedef struct i2c_devConf_s
{
    /* I2C */
    drv_i2c_devId_t             device_id;                  /* I2C driver device structure ID */
                                                                /* The I2C driver device structure is a software structure*/
                                                                /* For RTL839x/8x, this is mapped to SMI */

    drv_i2c_interfaceId_t       i2c_interface_id;           /* Chip I2C master interface ID */
    uint32                      dev_addr;                   /* device address of the slave device*/
    drv_i2c_clk_t               clk_freq;                   /* Serial clock frequency */
    uint32                      scl_delay;                  /* For RTL839x/8x software simulation of I2C clock delay time */
    drv_i2c_addrWidth_t         mem_addr_width;             /* slave memory address/Regitser index field width */
    drv_i2c_dataWidth_t         data_width;                 /* slave data field width */
    drv_i2c_read_type_t         read_type;                  /* Select Read Type : 0(Random Read) / 1(Sequential Read) */

    uint32                      scl_pin_id;                 /* SCL pin ID */
                                                                /* For Hardware I2C master inferface, this argument is useless */
                                                                /* For Software simulation of I2C, this is GPIO ID */
    uint32                      sda_pin_id;                 /* SDA pin ID */
                                                                /* For Hardware I2C master inferface, this argument is SDAX in specification */
                                                                /* For Software simulation of I2C, this is GPIO ID */

    drv_generalCtrlGpio_devId_t scl_dev;                    /* Only for Software simulation of I2C usage */
    drv_generalCtrlGpio_devId_t sda_dev;                    /* Only for Software simulation of I2C usage */

} i2c_devConf_t;

/*
 * Data Declaration
 */

typedef struct i2c_devInfo_s
{
    init_state_t        init_flag;
    i2c_devConf_t       conf;
}i2c_devInfo_t;


/*
 * Macro Definition
 */
#define I2C_READ_WORD(unit, i2c_dev_id, addr, pVal, ret) \
            do {\
                uint8 buf[4]; \
                ret = drv_i2c_read(unit, i2c_dev_id, addr, buf); \
                *pVal = (uint32)(buf[0]); \
                *pVal |= (uint32)(buf[1] << 8); \
                *pVal |= (uint32)(buf[2] << 16); \
                *pVal |= (uint32)(buf[3] << 24); \
            } while(0)

#define I2C_WRITE_WORD(unit, i2c_dev_id, addr, pVal, ret) \
            do {\
                uint8 buf[4]; \
                buf[0] = (uint8)(*pVal); \
                buf[1] = (uint8)((*pVal) >> 8); \
                buf[2] = (uint8)((*pVal) >> 16); \
                buf[3] = (uint8)((*pVal) >> 24); \
                ret = drv_i2c_write(unit, i2c_dev_id, addr, buf); \
               } while(0)

/*
 * Function Declaration
 */

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
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize i2c module before calling any i2c APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev);


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
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_i2c_write(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff);


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
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_i2c_read(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff);


/* Function Name:
 *      i2c_probe
 * Description:
 *      Probe i2c module of the specified device.
 * Input:
 *      unit - unit id
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
extern int32
i2c_probe(uint32 unit);


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
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize i2c module before calling any i2c APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_i2c_init(uint32 unit);


#endif /* __DRV_I2C_H__ */


