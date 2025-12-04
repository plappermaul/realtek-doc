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
 *      rtk_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      port     - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
rtk_i2c_init(rtk_i2c_port_t i2cPort)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(rtdrv_i2cCfg_t));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    SETSOCKOPT(RTDRV_I2C_INIT, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_init */

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
int32
rtk_i2c_enable_set(rtk_i2c_port_t i2cPort, rtk_enable_t enable)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_I2C_ENABLE_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_enable_set */

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
int32
rtk_i2c_enable_get(rtk_i2c_port_t i2cPort, rtk_enable_t *pEnable)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    GETSOCKOPT(RTDRV_I2C_ENABLE_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pEnable, &i2c_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_i2c_enable_get */

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
int32
rtk_i2c_width_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.width, &width, sizeof(rtk_i2c_width_t));
    SETSOCKOPT(RTDRV_I2C_WIDTH_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_width_set */

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
int32
rtk_i2c_width_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    GETSOCKOPT(RTDRV_I2C_WIDTH_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pWidth, &i2c_cfg.width, sizeof(rtk_i2c_width_t));

    return RT_ERR_OK;
}   /* end of rtk_i2c_width_get */

/* Function Name:
 *      rtk_i2c_write
 * Description:
 *      I2c write data.
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
rtk_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.devID, &devID, sizeof(uint32));
    osal_memcpy(&i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    osal_memcpy(&i2c_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_I2C_WRITE, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_write */

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
int32
rtk_i2c_noWait_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.devID, &devID, sizeof(uint32));
    osal_memcpy(&i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    osal_memcpy(&i2c_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_I2C_NOWAIT_WRITE, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_noWait_write */

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
int32
rtk_i2c_seq_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    i2c_cfg.port = i2cPort;
    i2c_cfg.devID= devID;
    i2c_cfg.regAddr = regAddr;
    i2c_cfg.pSeqData = pData;
    i2c_cfg.seqCnt = count;
    SETSOCKOPT(RTDRV_I2C_SEQ_WRITE, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_i2c_read
 * Description:
 *      I2c read data.
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
rtk_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.devID, &devID, sizeof(uint32));
    osal_memcpy(&i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    GETSOCKOPT(RTDRV_I2C_READ, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pData, &i2c_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_i2c_read */

/* Function Name:
 *      rtk_i2c_seq_read
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
rtk_i2c_seq_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData, uint32 count)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    i2c_cfg.port = i2cPort;
    i2c_cfg.devID= devID;
    i2c_cfg.regAddr = regAddr;
    i2c_cfg.pSeqData = pData;
    i2c_cfg.seqCnt = count;
    GETSOCKOPT(RTDRV_I2C_SEQ_READ, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}

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
int32
rtk_i2c_curr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.devID, &devID, sizeof(uint32));
    GETSOCKOPT(RTDRV_I2C_CURR_READ, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pData, &i2c_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_i2c_curr_read */

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
int32
rtk_i2c_seqCurr_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 *pData, uint32 count)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    i2c_cfg.port = i2cPort;
    i2c_cfg.devID= devID;
    i2c_cfg.pSeqData = pData;
    i2c_cfg.seqCnt = count;
    GETSOCKOPT(RTDRV_I2C_SEQCURR_READ, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_seqCurr_read */

/* Function Name:
 *      rtk_i2c_clock_set
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
rtk_i2c_clock_set(rtk_i2c_port_t i2cPort, uint32 clock)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.clock, &clock, sizeof(uint32));
    SETSOCKOPT(RTDRV_I2C_CLOCK_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_clock_set */

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
int32
rtk_i2c_clock_get(rtk_i2c_port_t i2cPort, uint32 *pClock)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    GETSOCKOPT(RTDRV_I2C_CLOCK_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pClock, &i2c_cfg.clock, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_i2c_clock_get */

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
int32
rtk_i2c_eepMirror_set(rtk_i2c_eeprom_mirror_t eepMirrorCfg)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.devID, &eepMirrorCfg.devId, sizeof(uint32));
    osal_memcpy(&i2c_cfg.width, &eepMirrorCfg.addrWidth, sizeof(rtk_i2c_width_t));
    osal_memcpy(&i2c_cfg.addrExtMode, &eepMirrorCfg.addrExtMode, sizeof(rtk_i2c_addr_ext_mode_t));
    SETSOCKOPT(RTDRV_I2C_EEPMIRROR_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_eepMirror_set */

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
int32
rtk_i2c_eepMirror_get(rtk_i2c_eeprom_mirror_t *pEepMirrorCfg)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEepMirrorCfg), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_I2C_EEPMIRROR_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(&pEepMirrorCfg->devId, &i2c_cfg.devID, sizeof(uint32));
    osal_memcpy(&pEepMirrorCfg->addrWidth, &i2c_cfg.width, sizeof(rtk_i2c_width_t));
    osal_memcpy(&pEepMirrorCfg->addrExtMode, &i2c_cfg.addrExtMode, sizeof(rtk_i2c_addr_ext_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_i2c_eepMirror_get */

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
int32
rtk_i2c_eepMirror_write(uint32 regAddr, uint32 data)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    osal_memcpy(&i2c_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_I2C_EEPMIRROR_WRITE, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_eepMirror_write */

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
int32
rtk_i2c_eepMirror_read(uint32 regAddr, uint32 *pData)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.regAddr, &regAddr, sizeof(uint32));
    GETSOCKOPT(RTDRV_I2C_EEPMIRROR_READ, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pData, &i2c_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_i2c_eepMirror_read */

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
int32
rtk_i2c_dataWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.width, &width, sizeof(rtk_i2c_width_t));
    SETSOCKOPT(RTDRV_I2C_DATAWIDTH_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_dataWidth_set */

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
int32
rtk_i2c_dataWidth_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    GETSOCKOPT(RTDRV_I2C_DATAWIDTH_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pWidth, &i2c_cfg.width, sizeof(rtk_i2c_width_t));

    return RT_ERR_OK;
}   /* end of rtk_i2c_dataWidth_get */



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
int32
rtk_i2c_addrWidth_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* function body */
    osal_memcpy(&i2c_cfg.port, &i2cPort, sizeof(rtk_i2c_port_t));
    osal_memcpy(&i2c_cfg.width, &width, sizeof(rtk_i2c_width_t));
    SETSOCKOPT(RTDRV_I2C_ADDRWIDTH_SET, &i2c_cfg, rtdrv_i2cCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_i2c_addrWidth_set */

/* Function Name:
 *      rtk_i2c_addrWidth_get
 * Description:
 *      Get the address width of I2C interface.
 * Input:
 *      port    - I2C port interface
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
int32
rtk_i2c_addrWidth_get(rtk_i2c_port_t port, rtk_i2c_width_t *pWidth)
{
    rtdrv_i2cCfg_t i2c_cfg;
    osal_memset(&i2c_cfg, 0x0, sizeof(i2c_cfg));

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&i2c_cfg.port, &port, sizeof(rtk_i2c_port_t));
    GETSOCKOPT(RTDRV_I2C_ADDRWIDTH_GET, &i2c_cfg, rtdrv_i2cCfg_t, 1);
    osal_memcpy(pWidth, &i2c_cfg.width, sizeof(rtk_i2c_width_t));

    return RT_ERR_OK;
}   /* end of rtk_i2c_addrWidth_get */
