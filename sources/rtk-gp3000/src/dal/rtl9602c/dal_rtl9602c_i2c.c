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
 * $Date: 2015-07-15 16:58:52 +0800 (Wed, 15 Jul 2015) $
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
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_i2c.h>
#include <rtk/i2c.h>


/*
 * Symbol Definition
 */
#define PART_NUMBER_LEN     16

/*
 * Data Declaration
 */
static uint32    i2c_init = INIT_NOT_COMPLETED;
const char* lowSpeedOe[] = {
    "CT-1225TGT-IH5CD",
    "CT-1212TET-HB5CD",
    "CT-1212TET-HF5CD",
    NULL
};

/*
 * Macro Declaration
 */
#define IS_I2C_IN_BUSY(v) (v & 0x4)
#define IS_I2C_IN_NO_ACK(v) (v & 0x8)


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9602c_i2c_init
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
dal_rtl9602c_i2c_init (
    rtk_i2c_port_t i2cPort )
{
    int i;
    uint32 data;
    uint32 value;
    uint32 ret;
    char isLowSpeed = 0;
    char partNo [PART_NUMBER_LEN + 1];

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_read (RTL9602C_I2C_CONFIGr, REG_ARRAY_INDEX_NONE, i2cPort, &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set address width to 8 bit */
    value = 0;
    ret = reg_field_set(
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_AWf, 
                &value, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set data width to 8 bit */
    value = 0;
    ret = reg_field_set (
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_DWf, 
                &value, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write (RTL9602C_I2C_CONFIGr, REG_ARRAY_INDEX_NONE, i2cPort, &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    /* Cortek OE module CT-1225TGT-IH5CD, CT-1212TET-HB5CD and CT-1212TET-HF5CD can't use 99KHz
     * To prevent problem, set to 65 KHz to read transcevier part number first
     * If no Cortek's OE present, set to 99 KHz
     */

    /* set clock to 65KHz bit */
#if 0
    value = 0x5F;
    data = 0x0;
    ret = reg_field_write (
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_CLOCK_DIVf, 
                &value);
#else
    ret = dal_rtl9602c_i2c_clock_set(i2cPort, 65);
#endif
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set enable */
    ret = reg_field_read(
                RTL9602C_IO_MODE_ENr,
                RTL9602C_I2C_ENf, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    value |= 1 << i2cPort;
    ret = reg_field_write(
                RTL9602C_IO_MODE_ENr,
                RTL9602C_I2C_ENf, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* Init status */
    i2c_init = INIT_COMPLETED;

    for (i = 0; i < PART_NUMBER_LEN; i++) {
        ret = dal_rtl9602c_i2c_read(
                    i2cPort, 
                    0x50, 
                    i + 40, 
                    &data);
        if (ret != RT_ERR_OK) {
            /* It is possible that there is no OE module, then the access will fail */
            break;
        }
        partNo[i] = (char)data;
    }

    partNo[PART_NUMBER_LEN] = '\0';

    i = 0;
    while (lowSpeedOe[i] != NULL) {
        if (!osal_strcmp(lowSpeedOe[i], partNo)) {
            isLowSpeed = 1;
            break;
        }
        i++;
    }

    if (!isLowSpeed) {
        /* set disable */
        ret = reg_field_read(
                    RTL9602C_IO_MODE_ENr, 
                    RTL9602C_I2C_ENf, 
                    &value);
        if (ret != RT_ERR_OK) {
            return (ret);
        }
        value = value & ~(1 << i2cPort);
        ret = reg_field_write(
                    RTL9602C_IO_MODE_ENr, 
                    RTL9602C_I2C_ENf, 
                    &value);
        if (ret != RT_ERR_OK) {
            return (ret);
        }

        /* set clock to 99KHz bit */
#if 0
        value = 0x3E;
        data = 0x0;
        ret = reg_field_write(
                    RTL9602C_I2C_CONFIGr, 
                    RTL9602C_I2C_CLOCK_DIVf, 
                    &value);
#else
    ret = dal_rtl9602c_i2c_clock_set(i2cPort, 99);
#endif
        if (ret != RT_ERR_OK) {
            return (ret);
        }

        /* set enable */
        ret = reg_field_read(
                    RTL9602C_IO_MODE_ENr, 
                    RTL9602C_I2C_ENf, 
                    &value);
        if (ret != RT_ERR_OK) {
            return (ret);
        }
        value |= (1 << i2cPort);
        ret = reg_field_write(
                    RTL9602C_IO_MODE_ENr, 
                    RTL9602C_I2C_ENf, 
                    &value);
        if (ret != RT_ERR_OK) {
            return (ret);
        }
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_init */

/* Function Name:
 *      dal_rtl9602c_i2c_enable_set
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
dal_rtl9602c_i2c_enable_set ( 
    rtk_i2c_port_t i2cPort, 
    rtk_enable_t enable )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "enable=%d", enable);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_field_read(
                RTL9602C_IO_MODE_ENr, 
                RTL9602C_I2C_ENf, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    if (ENABLED == enable)
        data |= (1 << i2cPort);
    else
        data &= ~(1 << i2cPort);

    ret = reg_field_write(
                RTL9602C_IO_MODE_ENr, 
                RTL9602C_I2C_ENf, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_enable_set */

/* Function Name:
 *      dal_rtl9602c_i2c_enable_get
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
dal_rtl9602c_i2c_enable_get ( 
    rtk_i2c_port_t i2cPort, 
    rtk_enable_t* pEnable )
{
    uint32 val;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_field_read(
                RTL9602C_IO_MODE_ENr, 
                RTL9602C_I2C_ENf, &val);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    if (val & (1 << i2cPort)) {
        *pEnable = ENABLED;
    } else {
        *pEnable = DISABLED;
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_enable_get */

/* Function Name:
 *      dal_rtl9602c_i2c_width_set
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
dal_rtl9602c_i2c_width_set ( 
    rtk_i2c_port_t i2cPort, 
    rtk_i2c_width_t width )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "width=%d", width);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((I2C_WIDTH_END <= width), RT_ERR_INPUT);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    if (I2C_WIDTH_24bit == width) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_read (RTL9602C_I2C_CONFIGr, REG_ARRAY_INDEX_NONE, i2cPort, &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set address width */
    ret = reg_field_set(
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_AWf, 
                &width, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set data width */
    ret = reg_field_set(
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_DWf, 
                &width, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_width_set */

/* Function Name:
 *      dal_rtl9602c_i2c_width_get
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
dal_rtl9602c_i2c_width_get ( 
    rtk_i2c_port_t   i2cPort, 
    rtk_i2c_width_t* pWidth )
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_field_read (
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_AWf, 
                pWidth);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_width_get */

/* Function Name:
 *      dal_rtl9602c_i2c_write
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
dal_rtl9602c_i2c_write ( 
    rtk_i2c_port_t i2cPort, 
    uint32 devID, 
    uint32 regAddr, 
    uint32 data )
{
    uint32 value;
    uint32 en;
    uint32 ret;
    uint8 no_ack_flag = 0;

    RT_DBG(
        LOG_DEBUG, 
        (MOD_DAL | MOD_HWMISC), 
        "devID=%d,regAddr=%d,data=%d", 
        devID, 
        regAddr, 
        data);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    /* set device ID */
    ret = reg_array_field_write(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_DEV_IDf, 
                &devID);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    /* set write data */
    ret = reg_array_field_write(
            RTL9602C_I2C_IND_WDr, 
            REG_ARRAY_INDEX_NONE,
            i2cPort,
            RTL9602C_WR_DATf, 
            &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    /* set regAddr */
    ret = reg_array_field_write(
                RTL9602C_I2C_IND_ADRr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_ADRf, 
                &regAddr);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set write */
    value = 0;
    en = 1;
    ret = reg_field_set(
                RTL9602C_I2C_IND_CMDr, 
                RTL9602C_WRENf, 
                &en, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    /* set cmd en */
    en = 1;
    ret = reg_field_set(
                RTL9602C_I2C_IND_CMDr, 
                RTL9602C_CMD_ENf, 
                &en, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write(
                RTL9602C_I2C_IND_CMDr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* check busy */
    do {
        ret = reg_array_read(
                  RTL9602C_I2C_IND_CMDr,
                  REG_ARRAY_INDEX_NONE,
                  i2cPort,
                  &value); 
        if (ret != RT_ERR_OK) {
            return (ret);
        }
        if (!no_ack_flag) {
            no_ack_flag = IS_I2C_IN_NO_ACK(value);
        }
    } while (IS_I2C_IN_BUSY(value));

    if (no_ack_flag) {
        return (RT_ERR_CHIP_NOT_FOUND);
    }
#if 0
    /* check NACK */
    if ((ret = reg_field_read(RTL9602C_EEPROM_DOWNLOADr, RTL9602C_NACKf, &value)) != RT_ERR_OK)
        return (ret);

    if (1 == value) {
        value = 1;
        if ((ret = reg_field_write(RTL9602C_EEPROM_DOWNLOADr, RTL9602C_NACKf, &value)) != RT_ERR_OK)
            return (ret);
        return (RT_ERR_EEPROM_NACK);
    }
#endif
    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_write */

/* Function Name:
 *      dal_rtl9602c_i2c_seq_write
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
 *      The ASIC doesn't support sequential write opearation
 * Use multiple random write to simulate the behavior
 */
int32
dal_rtl9602c_i2c_seq_write (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32 *pData,
    uint32 count)
{
    int i;
    int32 ret;
    uint32 addrBits, accessAddr;
    rtk_i2c_width_t addrWidth;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d", devID, regAddr);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if((ret = dal_rtl9602c_i2c_addrWidth_get(i2cPort, &addrWidth)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(addrWidth)
    {
    case I2C_WIDTH_8bit:
        addrBits = 8;
        break;
    case I2C_WIDTH_16bit:
        addrBits = 16;
        break;
    case I2C_WIDTH_24bit:
        addrBits = 24;
        break;
    case I2C_WIDTH_32bit:
        addrBits = 32;
        break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    accessAddr = regAddr;
    for(i = 0 ; i < count ; i++)
    {
        if((ret = dal_rtl9602c_i2c_write(i2cPort, devID, accessAddr, pData[i])) != RT_ERR_OK)
        {
            return ret;
        }
        if(accessAddr == ((((uint64)1)<<addrBits) - 1))
        {
            accessAddr = 0;
        }
        else
        {
            accessAddr ++;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_i2c_read
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
dal_rtl9602c_i2c_read ( 
    rtk_i2c_port_t i2cPort, 
    uint32 devID, 
    uint32 regAddr, 
    uint32* pData )
{
    uint32 value;
    uint32 en;
    uint32 ret;
    uint8 no_ack_flag = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d", devID, regAddr);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    /* set device ID */
    ret = reg_array_field_write(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_DEV_IDf, 
                &devID);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set regAddr */
    ret = reg_array_field_write(
                RTL9602C_I2C_IND_ADRr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_ADRf, 
                &regAddr);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set read */
    value = 0;
    en = 0;
    ret = reg_field_set(
                RTL9602C_I2C_IND_CMDr, 
                RTL9602C_WRENf, 
                &en, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    /* set cmd en */
    en = 1;
    ret = reg_field_set(
                RTL9602C_I2C_IND_CMDr, 
                RTL9602C_CMD_ENf, 
                &en, 
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write(
                RTL9602C_I2C_IND_CMDr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &value);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    do {
        ret = reg_array_read (
                    RTL9602C_I2C_IND_CMDr,
                    REG_ARRAY_INDEX_NONE,
                    i2cPort,
                    &value);
        if (ret != RT_ERR_OK) {
            return (ret);
        }
        if (!no_ack_flag) {
            no_ack_flag = IS_I2C_IN_NO_ACK(value);
        }
    } while (IS_I2C_IN_BUSY(value));

    if (no_ack_flag) {
        *pData = 0xFFFFFFFF;
        return (RT_ERR_CHIP_NOT_FOUND);
    }
#if 0
    /* check NACK */
    if ((ret = reg_field_read(RTL9602C_EEPROM_DOWNLOADr, RTL9602C_NACKf, &value)) != RT_ERR_OK)
        return (ret);

    if (1 == value) {
        value = 1;
        if ((ret = reg_field_write(RTL9602C_EEPROM_DOWNLOADr, RTL9602C_NACKf, &value)) != RT_ERR_OK)
            return (ret);
        return (RT_ERR_EEPROM_NACK);
    }
#endif
    ret = reg_array_field_read(
                RTL9602C_I2C_IND_RDr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_RD_DATf, 
                pData);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_read */

/* Function Name:
 *      dal_rtl9602c_i2c_read
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
 *      The ASIC doesn't support sequential read opearation
 * Use multiple random read to simulate the behavior
 */
int32
dal_rtl9602c_i2c_seq_read (
    rtk_i2c_port_t i2cPort,
    uint32 devID,
    uint32 regAddr,
    uint32 *pData,
    uint32 count)
{
    int i;
    int32 ret;
    uint32 addrBits, accessAddr;
    rtk_i2c_width_t addrWidth;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "devID=%d,regAddr=%d", devID, regAddr);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if((ret = dal_rtl9602c_i2c_addrWidth_get(i2cPort, &addrWidth)) != RT_ERR_OK)
    {
        return ret;
    }
    switch(addrWidth)
    {
    case I2C_WIDTH_8bit:
        addrBits = 8;
        break;
    case I2C_WIDTH_16bit:
        addrBits = 16;
        break;
    case I2C_WIDTH_24bit:
        addrBits = 24;
        break;
    case I2C_WIDTH_32bit:
        addrBits = 32;
        break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    accessAddr = regAddr;
    for(i = 0 ; i < count ; i++)
    {
        if((ret = dal_rtl9602c_i2c_read(i2cPort, devID, accessAddr, &pData[i])) != RT_ERR_OK)
        {
            return ret;
        }
        if(accessAddr == ((((uint64)1)<<addrBits) - 1))
        {
            accessAddr = 0;
        }
        else
        {
            accessAddr ++;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_i2c_clock_set
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
dal_rtl9602c_i2c_clock_set ( 
    rtk_i2c_port_t i2cPort, 
    uint32 clock )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "clock=%d", clock);

    /* parameter check */
    RT_PARAM_CHK((0x800 < clock), RT_ERR_INPUT);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    data = (31250 / clock) - 1;

    /* set data width */
    ret = reg_array_field_write(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_CLOCK_DIVf, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_clock_set */

/* Function Name:
 *      dal_rtl9602c_i2c_clock_get
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
dal_rtl9602c_i2c_clock_get ( rtk_i2c_port_t i2cPort, uint32* pClock )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_field_read(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_CLOCK_DIVf, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    *pClock = 125000 / ((data * 2 + 2) * 2);

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_width_get */
#if 0
/* Function Name:
 *      dal_rtl9602c_i2c_eepMirror_set
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
dal_rtl9602c_i2c_eepMirror_set ( rtk_i2c_eeprom_mirror_t eepMirrorCfg )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((0x7f < eepMirrorCfg.devId), RT_ERR_INPUT);
    RT_PARAM_CHK((I2C_WIDTH_END <= eepMirrorCfg.addrWidth), RT_ERR_INPUT);
    RT_PARAM_CHK((I2C_ADDR_EXT_MODE_END <= eepMirrorCfg.addrExtMode), RT_ERR_INPUT);


    /* function body */
//    if ((ret = reg_read(RTL9602C_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* set device id */
//    if ((ret = reg_field_set(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_DEV_IDf, &eepMirrorCfg.devId, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* set address width */
//    if ((ret = reg_field_set(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_AWf, &eepMirrorCfg.addrWidth, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* set address ext mode */
//    if ((ret = reg_field_set(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_AW_EXTf, &eepMirrorCfg.addrExtMode, &data)) != RT_ERR_OK)
//        return (ret);
//
//    if ((ret = reg_write(RTL9602C_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
//        return (ret);

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_eepMirror_set */

/* Function Name:
 *      dal_rtl9602c_i2c_eepMirror_get
 * Description:
 *      Get EEPROM mirror setting.
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
dal_rtl9602c_i2c_eepMirror_get ( rtk_i2c_eeprom_mirror_t* pEepMirrorCfg )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pEepMirrorCfg), RT_ERR_NULL_POINTER);


    /* function body */
//    if ((ret = reg_read(RTL9602C_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* device id */
//    if ((ret = reg_field_get(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_DEV_IDf, &pEepMirrorCfg->devId, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* address width */
//    if ((ret = reg_field_get(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_AWf, &pEepMirrorCfg->addrWidth, &data)) != RT_ERR_OK)
//        return (ret);
//
//    /* address ext mode */
//    if ((ret = reg_field_get(RTL9602C_I2C_IND_WIDTHr, RTL9602C_EEPROM_AW_EXTf, &pEepMirrorCfg->addrExtMode, &data)) != RT_ERR_OK)
//        return (ret);

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_eepMirror_get */
#endif

/* Function Name:
 *      dal_rtl9602c_i2c_eepMirror_write
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
dal_rtl9602c_i2c_eepMirror_write ( uint32 regAddr, uint32 data )
{
    uint32 value;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "regAddr=%d", regAddr);

    /* parameter check */
    RT_PARAM_CHK((regAddr > 0x1ff), RT_ERR_INPUT);

    /* function body */

    /* set regAddr */
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_ADRf, &regAddr)) != RT_ERR_OK)
        return (ret);

    /* set write */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_WRENf, &value)) != RT_ERR_OK)
        return (ret);
    /* write data */
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_WR_DATf, &data)) != RT_ERR_OK)
        return (ret);
    /* set cmd en */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_CMD_ENf, &value)) != RT_ERR_OK)
        return (ret);

    /* SRAM operation, no need to wait complete */

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_eepMirror_write */

/* Function Name:
 *      dal_rtl9602c_i2c_eepMirror_read
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
dal_rtl9602c_i2c_eepMirror_read ( uint32 regAddr, uint32* pData )
{
    uint32 value;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "regAddr=%d", regAddr);

    /* parameter check */
    RT_PARAM_CHK((regAddr > 0x1ff), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */

    /* set regAddr */
    value = regAddr & 0x1ff;
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_ADRf, &value)) != RT_ERR_OK)
        return (ret);

    /* set read */
    value = 0;
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_WRENf, &value)) != RT_ERR_OK)
        return (ret);
    /* set cmd en */
    value = 1;
    if ((ret = reg_field_write(RTL9602C_SFP_IND_ACCESSr, RTL9602C_CMD_ENf, &value)) != RT_ERR_OK)
        return (ret);

    /* SRAM operation, no need to wait complete */

    /* read data */
    if ((ret = reg_field_read(RTL9602C_SFP_IND_ACCESSr, RTL9602C_RD_DATf, pData)) != RT_ERR_OK)
        return (ret);

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_eepMirror_read */

/* Function Name:
 *      dal_rtl9602c_i2c_dataWidth_set
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
dal_rtl9602c_i2c_dataWidth_set ( rtk_i2c_port_t i2cPort, rtk_i2c_width_t width )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "width=%d", width);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((I2C_WIDTH_END <= width), RT_ERR_INPUT);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    if (I2C_WIDTH_24bit == width) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_read(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set data width */
    ret = reg_field_set(
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_DWf, 
                &width, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_dataWidth_set */

/* Function Name:
 *      dal_rtl9602c_i2c_dataWidth_get
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
dal_rtl9602c_i2c_dataWidth_get ( 
    rtk_i2c_port_t i2cPort, 
    rtk_i2c_width_t* pWidth )
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_field_read(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_DWf, 
                pWidth);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_dataWidth_get */



/* Function Name:
 *      dal_rtl9602c_i2c_addrWidth_set
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
dal_rtl9602c_i2c_addrWidth_set ( rtk_i2c_port_t i2cPort, rtk_i2c_width_t width )
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "width=%d", width);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((I2C_WIDTH_END <= width), RT_ERR_INPUT);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    if (I2C_WIDTH_24bit == width) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_read(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    /* set address width */
    ret = reg_field_set(
                RTL9602C_I2C_CONFIGr, 
                RTL9602C_I2C_AWf, 
                &width, 
                &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    ret = reg_array_write (RTL9602C_I2C_CONFIGr, REG_ARRAY_INDEX_NONE, i2cPort, &data);
    if (ret != RT_ERR_OK) {
        return (ret);
    }

    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_addrWidth_set */

/* Function Name:
 *      dal_rtl9602c_i2c_addrWidth_get
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
int32
dal_rtl9602c_i2c_addrWidth_get (
    rtk_i2c_port_t   i2cPort,
    rtk_i2c_width_t* pWidth )
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    if ((I2C_PORT_0 != i2cPort) && (I2C_PORT_1 != i2cPort)) {
        return (RT_ERR_CHIP_NOT_SUPPORTED);
    }

    ret = reg_array_field_read(
                RTL9602C_I2C_CONFIGr, 
                REG_ARRAY_INDEX_NONE,
                i2cPort,
                RTL9602C_I2C_AWf, 
                pWidth);
    if (ret != RT_ERR_OK) {
        return (ret);
    }
    
    return (RT_ERR_OK);
}   /* end of dal_rtl9602c_i2c_addrWidth_get */

