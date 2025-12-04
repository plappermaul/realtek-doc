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
 * $Revision: 87677 $
 * $Date: 2018-05-04 15:38:37 +0800 (Fri, 04 May 2018) $
 *
 * Purpose : Definition of I2C API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) I2C control
 *           (2) I2C read/write
 */

#ifndef __RT_I2C_H__
#define __RT_I2C_H__


/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
typedef enum rt_i2c_port_e
{
    RT_I2C_PORT_0 = 0,  /* I2C port 0 of the chip */
    RT_I2C_PORT_1,      /* I2C port 1 of the chip */

    RT_I2C_PORT_END
} rt_i2c_port_t;

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
 *      rt_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      i2cPort     - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 * Note:
 *      Must initialize before calling any other APIs.
 */
extern int32
rt_i2c_init(rt_i2c_port_t i2cPort);

/* Function Name:
 *      rt_i2c_enable_set
 * Description:
 *      Enable/Disable I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      enable     - enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 *      RT_ERR_INPUT    - invalid input parameters
 * Note:
 *
 */
extern int32
rt_i2c_enable_set(rt_i2c_port_t i2cPort, rt_enable_t enable);

/* Function Name:
 *      rt_i2c_enable_get
 * Description:
 *      Get I2C interface state.
 * Input:
 *      i2cPort   - I2C port interface
 *      pEnable   - the pointer of enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rt_i2c_enable_get(rt_i2c_port_t i2cPort, rt_enable_t *pEnable);

/* Function Name:
 *      rt_i2c_write
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
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 *      RT_ERR_INPUT    - invalid input parameters
 * Note:
 *
 */
extern int32
rt_i2c_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data);

/* Function Name:
 *      rt_i2c_read
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
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_INPUT        - invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rt_i2c_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData);

/* Function Name:
 *      rt_i2c_seq_write
 * Description:
 *      I2c sequential write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - data values to be written
 *      count     - number of sequential write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 *      RT_ERR_INPUT    - invalid input parameters
 * Note:
 *
 */
extern int32
rt_i2c_seq_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count);

/* Function Name:
 *      rt_i2c_seq_read
 * Description:
 *      I2c sequential read data.
 * Input:
 *      i2cPort     - I2C port interface
 *      devID       - the device ID
 *      regAddr     - register address
 *      pData       - the pointer of returned data array
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_INPUT        - invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rt_i2c_seq_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count);

/* Function Name:
 *      rt_i2c_clock_set
 * Description:
 *      Set the clock frequency of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KHz
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - failed
 *      RT_ERR_INPUT    - invalid input parameters
 * Note:
 *
 */
extern int32
rt_i2c_clock_set(rt_i2c_port_t i2cPort, uint32 clock);

/* Function Name:
 *      rt_i2c_clock_get
 * Description:
 *      Get the clock frequency of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pClock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
rt_i2c_clock_get(rt_i2c_port_t i2cPort, uint32 *pClock);

#endif /*#ifndef __RT_I2C_H__*/

