/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Feature : The file includes the following modules and sub-modules
 *           (1) I2C control
 *           (2) I2C read/write
 */

#ifndef __RTK_I2C_H__
#define __RTK_I2C_H__


/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
typedef enum rtk_i2c_width_e
{
    I2C_WIDTH_8bit = 0,
    I2C_WIDTH_16bit,
    I2C_WIDTH_24bit,
    I2C_WIDTH_32bit,

    I2C_WIDTH_END
} rtk_i2c_width_t;

typedef enum rtk_i2c_port_e
{
    I2C_PORT_0 = 0,
    I2C_PORT_1,

    I2C_PORT_END
} rtk_i2c_port_t;

typedef enum rtk_i2c_addr_ext_mode_e
{
    I2C_ADDR_EXT_MODE_0 = 0,
    I2C_ADDR_EXT_MODE_1,
    I2C_ADDR_EXT_MODE_2,
    I2C_ADDR_EXT_MODE_3,

    I2C_ADDR_EXT_MODE_END
} rtk_i2c_addr_ext_mode_t;

typedef struct rtk_i2c_eeprom_mirror_s
{
    uint32 devId;
    rtk_i2c_width_t addrWidth;
    rtk_i2c_addr_ext_mode_t addrExtMode;

} rtk_i2c_eeprom_mirror_t;

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
 *      rtk_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      i2cPort     - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
extern int32
rtk_i2c_init(rtk_i2c_port_t i2cPort);

/* Function Name:
 *      rtk_i2c_enable_set
 * Description:
 *      Enable/Disable I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      enable     - enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_enable_set(rtk_i2c_port_t i2cPort, rtk_enable_t enable);

/* Function Name:
 *      rtk_i2c_enable_get
 * Description:
 *      Get I2C interface state.
 * Input:
 *      i2cPort   - I2C port interface
 *      pEnable   - the pointer of enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_enable_get(rtk_i2c_port_t i2cPort, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_i2c_width_set
 * Description:
 *      Set the data and address width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_width_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width);

/* Function Name:
 *      rtk_i2c_width_get
 * Description:
 *      Get the data and address width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_width_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth);

/* Function Name:
 *      rtk_i2c_write
 * Description:
 *      I2c random write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data);

/* Function Name:
 *      rtk_i2c_seq_write
 * Description:
 *      I2c sequential random write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - data values to be written
 *      count     - number of sequential write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_seq_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count);

/* Function Name:
 *      rtk_i2c_noWait_write
 * Description:
 *      I2c random write data without write complete check. (Won't return write error)
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      This write API is for some special applications which are time senstive.
 * For most of the application, use rtk_i2c_write instead. Only valid for some
 * ASIC (RTL9601B)
 */
extern int32
rtk_i2c_noWait_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data);

/* Function Name:
 *      rtk_i2c_read
 * Description:
 *      I2c random read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData);

/* Function Name:
 *      rtk_i2c_seq_read
 * Description:
 *      I2c sequential random read data.
 * Input:
 *      i2cPort     - I2C port interface
 *      devID       - the device ID
 *      regAddr     - register address
 *      pData       - the pointer of returned data array
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_seq_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count);

/* Function Name:
 *      rtk_i2c_curr_read
 * Description:
 *      I2c current read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_curr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData);

/* Function Name:
 *      rtk_i2c_seqCurr_read
 * Description:
 *      I2c sequential current read data.
 * Input:
 *      i2cPort     - I2C port interface
 *      devID       - the device ID
 *      pData       - the pointer of returned data array
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_seqCurr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData, uint32 count);

/* Function Name:
 *      rtk_i2c_clock_set
 * Description:
 *      Set the clock of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KHz
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_clock_set(rtk_i2c_port_t i2cPort, uint32 clock);


/* Function Name:
 *      rtk_i2c_clock_get
 * Description:
 *      Get the clock of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pClock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_clock_get(rtk_i2c_port_t i2cPort, uint32 *pClock);

/* Function Name:
 *      rtk_i2c_eepMirror_set
 * Description:
 *      Configure EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0¡A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
extern int32
rtk_i2c_eepMirror_set(rtk_i2c_eeprom_mirror_t eepMirrorCfg);

/* Function Name:
 *      rtk_i2c_eepMirror_get
 * Description:
 *      Configure EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0¡A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
extern int32
rtk_i2c_eepMirror_get(rtk_i2c_eeprom_mirror_t *pEepMirrorCfg);

/* Function Name:
 *      rtk_i2c_eepMirror_write
 * Description:
 *      I2c EEPROM mirror write data.
 * Input:
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_eepMirror_write(uint32 regAddr, uint32 data);

/* Function Name:
 *      rtk_i2c_eepMirror_read
 * Description:
 *      I2c read data from EEPROM mirror.
 * Input:
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_eepMirror_read(uint32 regAddr, uint32 *pData);

/* Function Name:
 *      rtk_i2c_dataWidth_set
 * Description:
 *      Set the data width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_dataWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width);

/* Function Name:
 *      rtk_i2c_dataWidth_get
 * Description:
 *      Get the data width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_dataWidth_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth);



/* Function Name:
 *      rtk_i2c_addrWidth_set
 * Description:
 *      Set the address width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
extern int32
rtk_i2c_addrWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width);

/* Function Name:
 *      rtk_i2c_addrWidth_get
 * Description:
 *      Get the address width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
extern int32
rtk_i2c_addrWidth_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth);


#endif /*#ifndef __RTK_I2C_H__*/

