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



/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>

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
 *      rt_i2c_init
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
int32
rt_i2c_init(rt_i2c_port_t i2cPort)
{
    rtdrv_rt_i2cCfg_t rt_i2c_cfg;
    memset(&rt_i2c_cfg, 0x0, sizeof(rtdrv_rt_i2cCfg_t));

    /* function body */
    osal_memcpy(&rt_i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    SETSOCKOPT(RTDRV_RT_I2C_INIT, &rt_i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_i2c_init */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_enable_set(rt_i2c_port_t i2cPort, rt_enable_t enable)
{
    rtdrv_rt_i2cCfg_t rt_i2c_cfg;
    memset(&rt_i2c_cfg, 0x0, sizeof(rtdrv_rt_i2cCfg_t));

    /* function body */
    osal_memcpy(&rt_i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    osal_memcpy(&rt_i2c_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_I2C_ENABLE_SET, &rt_i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_i2c_enable_set */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_enable_get(rt_i2c_port_t i2cPort, rt_enable_t *pEnable)
{
    rtdrv_rt_i2cCfg_t rt_i2c_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    GETSOCKOPT(RTDRV_RT_I2C_ENABLE_GET, &rt_i2c_cfg, rtdrv_rt_i2cCfg_t, 1);
    osal_memcpy(pEnable, &rt_i2c_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_i2c_enable_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    rtdrv_rt_i2cCfg_t rt_i2c_cfg;
    memset(&rt_i2c_cfg, 0x0, sizeof(rtdrv_rt_i2cCfg_t));

    /* function body */
    osal_memcpy(&rt_i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    osal_memcpy(&rt_i2c_cfg.devID, &devID, sizeof(uint32));
    osal_memcpy(&rt_i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    osal_memcpy(&rt_i2c_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_I2C_WRITE, &rt_i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_i2c_write */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    rtdrv_rt_i2cCfg_t rt_i2c_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    osal_memcpy(&rt_i2c_cfg.devID, &devID, sizeof(uint32));
    osal_memcpy(&rt_i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_I2C_READ, &rt_i2c_cfg, rtdrv_rt_i2cCfg_t, 1);
    osal_memcpy(pData, &rt_i2c_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_i2c_read */

/* Function Name:
 *      rt_i2c_seq_write
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
int32
rt_i2c_seq_write(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    rtdrv_rt_i2cCfg_t i2c_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    memset(&i2c_cfg, 0x0, sizeof(rtdrv_rt_i2cCfg_t));

    /* function body */
    i2c_cfg.i2cPort = i2cPort;
    i2c_cfg.devID= devID;
    i2c_cfg.regAddr = regAddr;
    i2c_cfg.pSeqData = pData;
    i2c_cfg.seqCnt = count;
    SETSOCKOPT(RTDRV_RT_I2C_SEQ_WRITE, &i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
rt_i2c_seq_read(rt_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    rtdrv_rt_i2cCfg_t i2c_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    i2c_cfg.i2cPort = i2cPort;
    i2c_cfg.devID= devID;
    i2c_cfg.regAddr = regAddr;
    i2c_cfg.pSeqData = pData;
    i2c_cfg.seqCnt = count;
    GETSOCKOPT(RTDRV_RT_I2C_SEQ_READ, &i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_i2c_clock_set
 * Description:
 *      Set the clock of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
rt_i2c_clock_set(rt_i2c_port_t i2cPort, uint32 clock)
{
    rtdrv_rt_i2cCfg_t i2c_cfg;
    memset(&i2c_cfg, 0x0, sizeof(rtdrv_rt_i2cCfg_t));

    /* function body */
    osal_memcpy(&i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    osal_memcpy(&i2c_cfg.clock, &clock, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_I2C_CLOCK_SET, &i2c_cfg, rtdrv_rt_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_i2c_clock_set */

/* Function Name:
 *      rt_i2c_clock_get
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
int32
rt_i2c_clock_get(rt_i2c_port_t i2cPort, uint32 *pClock)
{
    rtdrv_rt_i2cCfg_t i2c_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.i2cPort, &i2cPort, sizeof(rt_i2c_port_t));
    GETSOCKOPT(RTDRV_RT_I2C_CLOCK_GET, &i2c_cfg, rtdrv_rt_i2cCfg_t, 1);
    osal_memcpy(pClock, &i2c_cfg.clock, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_i2c_clock_get */
