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
 * $Revision: 60215 $
 * $Date: 2017-03-23 16:58:52 +0800 (Thur, 23 Mar 2017) $
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

#include <common/rt_type.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_i2c.h>
#include <rtk/i2c.h>

#include <peri.h>

/*
 * Symbol Definition
 */
#define MAX_I2C_BUS 1
#define MAX_I2C_SEQ_RW  256

/*
 * Data Declaration
 */
static uint32 i2c_init[MAX_I2C_BUS] = {INIT_NOT_COMPLETED};
static rtk_enable_t GBL_I2C_ENABLE[MAX_I2C_BUS] = {DISABLED};
static uint16 gbl_clock = 100; //100kHZ

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9607f_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      i2cPort    - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
dal_rtl9607f_i2c_init (
    rtk_i2c_port_t i2cPort )
{
    int32 ret=RT_ERR_OK;
    ca_uint8_t data;
    
    /* parameter check */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);

    /* Init status */
    i2c_init[i2cPort] = INIT_COMPLETED;

    if((ret = cap_i2c_enable_set(0,1,gbl_clock)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        i2c_init[i2cPort] = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }
    GBL_I2C_ENABLE[i2cPort] = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_init */

/* Function Name:
 *      dal_rtl9607f_i2c_enable_get
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
dal_rtl9607f_i2c_enable_get (
    rtk_i2c_port_t i2cPort,
    rtk_enable_t* pEnable )
{
    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    *pEnable = GBL_I2C_ENABLE[i2cPort];

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_enable_get */

/* Function Name:
 *      dal_rtl9607f_i2c_enable_set
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
dal_rtl9607f_i2c_enable_set (
    rtk_i2c_port_t i2cPort,
    rtk_enable_t enable )
{
    ca_uint32_t ret = CA_E_OK;
    ca_boolean ca_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "enable=%d", enable);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(enable == ENABLED)
        ca_enable = 1;
    else
        ca_enable = 0;

    if((ret = cap_i2c_enable_set(0,ca_enable,gbl_clock)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    GBL_I2C_ENABLE[i2cPort] = enable;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_enable_set */

/* Function Name:
 *      dal_rtl9607f_i2c_write
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
dal_rtl9607f_i2c_write (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32 data )
{
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t slave_addr;
    ca_uint8_t slave_offset;
    ca_uint8_t tx_data;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d data=0x%x", devID, regAddr ,data);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    if(GBL_I2C_ENABLE[i2cPort] == DISABLED)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_NOT_ALLOWED;
    }

    /* function body */
    /* CA API require the 8-bits address directly, so shift 1 bit left here */
    slave_addr = (devID << 1);
    slave_offset = regAddr;
    tx_data = data;

    if((ret = cap_i2c_write(0,slave_addr,slave_offset,1,&tx_data)) != CA_E_OK)
    {
        if(ret == CA_E_BUSY)
        {
            cap_i2c_enable_set(0,1,gbl_clock);
        }
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_CHIP_NOT_FOUND;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_write */

/* Function Name:
 *      dal_rtl9607f_i2c_read
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
dal_rtl9607f_i2c_read (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32* pData )
{
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t slave_addr;
    ca_uint8_t slave_offset;
    ca_uint8_t rx_data;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d", devID, regAddr);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if(GBL_I2C_ENABLE[i2cPort] == DISABLED)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_NOT_ALLOWED;
    }

    /* function body */
    /* CA API require the 8-bits address directly, so shift 1 bit left here */
    slave_addr = (devID << 1);
    slave_offset = regAddr; 

    if((ret = cap_i2c_read(0,slave_addr,slave_offset,1,&rx_data)) != CA_E_OK)
    {
        if(ret == CA_E_BUSY)
        {
            cap_i2c_enable_set(0,1,gbl_clock);
        }

        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_CHIP_NOT_FOUND;
    }

    *pData = rx_data;
    
    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_read */

/* Function Name:
 *      dal_rtl9607f_i2c_clock_get
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
dal_rtl9607f_i2c_clock_get (
    rtk_i2c_port_t i2cPort,
    uint32* pClock )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "i2cPort=%d", i2cPort);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    *pClock = gbl_clock;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_clock_get */

/* Function Name:
 *      dal_rtl9607f_i2c_clock_set
 * Description:
 *      Set the clock of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KHz, 10-bit value
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
dal_rtl9607f_i2c_clock_set (
    rtk_i2c_port_t i2cPort,
    uint32 clock )
{
    ca_uint32_t ret = CA_E_OK;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "i2cPort=%d", i2cPort);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((clock == 0), RT_ERR_INPUT);
    RT_PARAM_CHK((clock > 1000), RT_ERR_INPUT);

    /* function body */
    if((ret = cap_i2c_enable_set(0,GBL_I2C_ENABLE[i2cPort] == ENABLED ? 1 : 0,clock)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    gbl_clock = clock;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_i2c_clock_set */

/* Function Name:
 *      dal_rtl9607f_i2c_seq_read
 * Description:
 *      I2c read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *
 */
int32
dal_rtl9607f_i2c_seq_read (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32 *pData,
    uint32 count)
{
    uint32 i;
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t slave_addr;
    ca_uint8_t slave_offset;
    ca_uint8_t rData[MAX_I2C_SEQ_RW];
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d", devID, regAddr);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_INIT_CHK(i2c_init[i2cPort]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((MAX_I2C_SEQ_RW < count), RT_ERR_INPUT);

    if(GBL_I2C_ENABLE[i2cPort] == DISABLED)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_NOT_ALLOWED;
    }

    /* function body */
    /* CA API require the 8-bits address directly, so shift 1 bit left here */
    slave_addr = (devID << 1);
    slave_offset = regAddr; 

    if((ret = cap_i2c_read(0,slave_addr,slave_offset,count,rData)) != CA_E_OK)
    {
        if(ret == CA_E_BUSY)
        {
            cap_i2c_enable_set(0,1,gbl_clock);
        }

        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_CHIP_NOT_FOUND;
    }

    for(i = 0; i < count ; i ++)
    {
        pData[i] = rData[i];
    }
    return RT_ERR_OK;
} /* end of dal_rtl9607f_i2c_seq_read */


/* Function Name:
 *      dal_rtl9607f_i2c_seq_write
 * Description:
 *      I2c read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 *      count       - number of sequential read
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *
 */
int32
dal_rtl9607f_i2c_seq_write (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32 *pData,
    uint32 count)
{
    uint32 i;
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t slave_addr;
    ca_uint8_t slave_offset;
    ca_uint8_t wData[MAX_I2C_SEQ_RW];
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d pData=0x%x", devID, regAddr ,pData);

    /* check Init status */
    RT_PARAM_CHK((i2cPort >= MAX_I2C_BUS), RT_ERR_PORT_ID);
    RT_PARAM_CHK((MAX_I2C_SEQ_RW < count), RT_ERR_INPUT);
    RT_INIT_CHK(i2c_init[i2cPort]);

    if(GBL_I2C_ENABLE[i2cPort] == DISABLED)
    {
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_NOT_ALLOWED;
    }

    /* function body */
    /* CA API require the 8-bits address directly, so shift 1 bit left here */
    slave_addr = (devID << 1);
    slave_offset = regAddr;
    for(i = 0; i < count ; i ++)
    {
        wData[i] = pData[i];
    }
    
    if((ret = cap_i2c_write(0,slave_addr,slave_offset,count,wData)) != CA_E_OK)
    {
        if(ret == CA_E_BUSY)
        {
            cap_i2c_enable_set(0,1,gbl_clock);
        }
        RT_ERR(ret, (MOD_HWMISC | MOD_DAL), "");
        return RT_ERR_CHIP_NOT_FOUND;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_i2c_seq_write */

