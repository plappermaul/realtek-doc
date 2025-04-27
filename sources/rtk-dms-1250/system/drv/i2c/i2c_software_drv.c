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

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/time.h>
#include <private/drv/i2c/i2c_software_drv.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */
#define ACK_TIMER                       5
#define DELAY_4000                      4000 /*100KHz*/
#define SFP_CHIPID                      0x50
#define SFP_TYPE                        SOFTWARE_I2C_TYPE_8BITS_DEV

/*
 * Data Declaration
 */
static uint32 software_i2c_addr_width[SOFTWARE_I2C_DEVICE_MAX];         /* SOFTWARE I2C waveform address field width */
static uint32 software_i2c_data_width[SOFTWARE_I2C_DEVICE_MAX];         /* SOFTWARE I2C waveform data field width */
static uint32 software_i2c_CHIPID[SOFTWARE_I2C_DEVICE_MAX];
static uint32 software_i2c_DELAY[SOFTWARE_I2C_DEVICE_MAX];
#if defined(__BOOTLOADER__)
  static uint8  software_i2c_NAME[SOFTWARE_I2C_DEVICE_MAX][SOFTWARE_I2C_MAX_DEV_NAME];
#endif /*defined(__BOOTLOADER__) */
static init_state_t software_i2c_INIT_FLAG[SOFTWARE_I2C_DEVICE_MAX];    /* SOFTWARE I2C device initial flag */

static drv_generalCtrlGpio_gpioId_t general_software_i2c_SCK[SOFTWARE_I2C_DEVICE_MAX];
static drv_generalCtrlGpio_gpioId_t general_software_i2c_SDA[SOFTWARE_I2C_DEVICE_MAX];

/*
 * Macro Declaration
 */

#define CLK_DURATION(clk)               osal_time_udelay(clk)


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
int32 drv_software_i2c_type_set(uint32 software_i2c_addrWidth, uint32 software_i2c_dataWidth, uint32 chipid, uint32 delay, uint32 dev)
{
    software_i2c_addr_width[dev] = software_i2c_addrWidth;
    software_i2c_data_width[dev] = software_i2c_dataWidth;
    software_i2c_CHIPID[dev] = chipid;
    software_i2c_DELAY[dev] = delay;
    return RT_ERR_OK;
} /* end of drv_software_i2c_type_set */

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
int32 drv_software_i2c_type_get(uint32 * paddrWidth, uint32 * pdataWidth, uint32 * pchipid, uint32 * pdelay, uint32 dev)
{
    *paddrWidth = software_i2c_addr_width[dev];
    *pdataWidth = software_i2c_data_width[dev];
    *pchipid = software_i2c_CHIPID[dev];
    *pdelay = software_i2c_DELAY[dev];

    return RT_ERR_OK;
} /* end of drv_software_i2c_type_get */


/* Function Name:
 *      _general_software_i2c_readBit
 * Description:
 *      read bit function
 * Input:
 *      bitLen  - bit length
 *      dev     - dev id
 * Output:
 *      pRdata - data read
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
static int32 _general_software_i2c_readBit(uint32 bitLen, uint32 *pRdata, uint32 dev)
{
    uint32 unit = HWP_MY_UNIT_ID();
    uint32 data = 0;
    uint32 delay = software_i2c_DELAY[dev];
    int32  ret = RT_ERR_FAILED;

    CLK_DURATION(delay);

    RT_ERR_CHK(drv_generalCtrlGPIO_direction_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, GPIO_DIR_IN), ret);

    for (*pRdata = 0; bitLen > 0; bitLen--)
    {

        /* clocking */
        RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 1), ret);

        CLK_DURATION(delay);

        RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_get(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, &data), ret);

        CLK_DURATION(delay);

        RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 0), ret);

        *pRdata |= (data << (bitLen - 1));

        CLK_DURATION(delay);
    }

    /* change GPIO pin to Output only */
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 0), ret);
    RT_ERR_CHK(drv_generalCtrlGPIO_direction_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, GPIO_DIR_OUT), ret);

    return RT_ERR_OK;
} /* end of _general_software_i2c_readBit */

/* Function Name:
 *      _general_software_i2c_writeBit
 * Description:
 *      write bit function
 * Input:
 *      signal - ctrl code
 *      bitLen - bit length
 *      dev    - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 _general_software_i2c_writeBit(uint16 signal, uint32 bitLen, uint32 dev)
{
    uint32 unit = HWP_MY_UNIT_ID();
    int32 ret = RT_ERR_FAILED;
    uint32 delay = software_i2c_DELAY[dev];

    for ( ; bitLen > 0; bitLen--)
    {

        /* prepare data */
        if (signal & (1 << (bitLen - 1)))
        {
            RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 1), ret);

        }
        else
        {
            RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 0), ret);
        }

        CLK_DURATION(delay);

        /* clocking */
        RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 1), ret);

        CLK_DURATION(delay);
        RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 0), ret);

        CLK_DURATION(delay);
    }

    return RT_ERR_OK;
} /* end of _general_software_i2c_writeBit */


/* Function Name:
 *      _general_nbits_dev_start
 * Description:
 *      device start bit function
 * Input:
 *      dev - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
static int32 _general_nbits_dev_start(uint32 dev)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit = HWP_MY_UNIT_ID();
    uint32 delay = software_i2c_DELAY[dev];

    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 1), ret);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 1), ret);
    CLK_DURATION(delay);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 0), ret);
    CLK_DURATION(delay);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 0), ret);
    CLK_DURATION(delay);

    return ret;
} /* end of _general_nbits_dev_start */


/* Function Name:
 *      _general_nbits_dev_stop
 * Description:
 *      device stop bit function
 * Input:
 *      dev - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
static int32 _general_nbits_dev_stop(uint32 dev)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit = HWP_MY_UNIT_ID();
    uint32  delay = software_i2c_DELAY[dev];

    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 0), ret);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 0), ret);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 1), ret);
    CLK_DURATION(delay);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 1), ret);

    return ret;
} /* end of _general_nbits_dev_stop */

/* Function Name:
 *      _general_nbits_dev_read
 * Description:
 *      Random read function
 * Input:
 *      mAddrs  - address
 *      dev     - dev id
 * Output:
 *      pRdata - data read
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32 _general_nbits_dev_read(uint32 mAddrs, uint8 *pRdata, uint32 dev)
{
    uint32 ack = 0;
    uint32 slave = 0;
    uint32 rdata;
    int32  ret = RT_ERR_FAILED;
    uint32 loop_idx;

    /*Initial data memory*/
    slave = software_i2c_CHIPID[dev] & 0xff;
    RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x", slave, mAddrs, ack);
    mAddrs = mAddrs & 0xffff;
    *pRdata = 0;

    /* CTRL code: 7'slave adress */
    RT_ERR_CHK(_general_software_i2c_writeBit(slave, 7, dev), ret);

    /* 0: issue WRITE command */
    RT_ERR_CHK(_general_software_i2c_writeBit(0x0, 1, dev), ret);

    /* Read ack for issuing WRITE command*/
    RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);

    /* Check ack value, it should be zero. */
    if (0 != ack)
    {
        RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
        return RT_ERR_FAILED;
    }

    /* Reg Address Field*/
    for(loop_idx = software_i2c_addr_width[dev]; loop_idx > 0; loop_idx--)
    {
        /* Note: From MSB 8-bit Reg Address */
        RT_ERR_CHK(_general_software_i2c_writeBit(((mAddrs >> ((loop_idx-1)*8)) & 0xFF), 8, dev), ret);
        /* Read ack for issuing WRITE command*/
        RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);
        /* Check ack value, it should be zero. */
        if (0 != ack)
        {
            RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
            return RT_ERR_FAILED;
        }
     }

    /* Start signal for Read */
    RT_ERR_CHK(_general_nbits_dev_start(dev), ret);

    /* Device Address & Read */
    RT_ERR_CHK(_general_software_i2c_writeBit(slave, 7, dev), ret);

     /* 1: issue READ command */
    RT_ERR_CHK(_general_software_i2c_writeBit(0x1, 1, dev), ret);

    /* Read ack for issuing READ command*/
    RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);

    /* Check ack value, it should be zero. */
    if (ack != 0)
    {
        RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
        return RT_ERR_FAILED;
    }

    /* Reg Data Field*/
    for(loop_idx = 0; loop_idx < software_i2c_data_width[dev]; loop_idx++)
    {
        /* Note: From uint8 array index 0 */
        RT_ERR_CHK(_general_software_i2c_readBit(8, &rdata, dev), ret);
        *(pRdata+loop_idx) = (uint8)(rdata);
        if((loop_idx + 1) != software_i2c_data_width[dev])
        {
            /* Return ack = 1*/
            RT_ERR_CHK(_general_software_i2c_writeBit(0x0, 1, dev), ret);
        }else{
            /* Return Nack = 1*/
            RT_ERR_CHK(_general_software_i2c_writeBit(0x01, 1, dev), ret);
        }
     }
    return ret;
} /* end of _general_nbits_dev_read */

/* Function Name:
 *      _general_sequential_nbits_dev_read
 * Description:
 *      Sequential read function
 * Input:
 *      dev     - dev id
 * Output:
 *      pRdata - data read
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *  Tino Test for GS728TPv2 FAN controller
 *  Do traditional I2C read format
 *
 */
int32 _general_sequential_nbits_dev_read(uint8 *pRdata, uint32 dev)
{
    uint32 ack = 0;
    uint32 slave = 0;
    uint32 rdata;
    int32  ret = RT_ERR_FAILED;
    uint32 loop_idx;

    /*Initial data memory*/
    slave = software_i2c_CHIPID[dev] & 0xff;
    RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x ack 0x%x", slave, ack);
    *pRdata = 0;

    /* CTRL code: 7'slave adress */
    RT_ERR_CHK(_general_software_i2c_writeBit(slave, 7, dev), ret);

      /* 1: issue READ command */
    RT_ERR_CHK(_general_software_i2c_writeBit(0x1, 1, dev), ret);

    /* Read ack for issuing READ command*/
    RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);

    /* Check ack value, it should be zero. */
    if (ack != 0)
    {
        RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x ack 0x%x\n", slave, ack);
        return RT_ERR_FAILED;
    }

    /* Reg Data Field*/
    for(loop_idx = 0; loop_idx < software_i2c_data_width[dev]; loop_idx++)
    {
        /* Note: From uint8 array index 0 */
        RT_ERR_CHK(_general_software_i2c_readBit(8, &rdata, dev), ret);

        *(pRdata+loop_idx) = (uint8)(rdata);
        if((loop_idx + 1) != software_i2c_data_width[dev])
        {
            /* Return ack = 1*/
            RT_ERR_CHK(_general_software_i2c_writeBit(0x0, 1, dev), ret);
        }else{
            /* Return Nack = 1*/
            RT_ERR_CHK(_general_software_i2c_writeBit(0x01, 1, dev), ret);
        }
     }

    return ret;
} /* end of _general_nbits_dev_read */

/* Function Name:
 *      _general_nbits_dev_write
 * Description:
 *      write function
 * Input:
 *      mAddrs  - address
 *      wData   - data write
 *      dev     - dev id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      PD64012
 */
int32 _general_nbits_dev_write(uint32 mAddrs, uint8 *pWdata, uint32 dev)
{
    uint32 ack = 0;
    uint32 slave = 0;
    uint32 loop_idx;
    int32  ret = RT_ERR_FAILED;

    /*Initial data memory*/
    slave = software_i2c_CHIPID[dev] & 0xff;
    RT_LOG(LOG_DEBUG, MOD_GENERAL, "Write add: %02X ", slave);
    mAddrs = mAddrs & 0xffff;

    /* CTRL code: 7'slave adress */
    RT_ERR_CHK(_general_software_i2c_writeBit(slave, 7, dev), ret);

    /* 0: issue WRITE command */
    RT_ERR_CHK(_general_software_i2c_writeBit(0x0, 1, dev), ret);

    /* Read ack for issuing WRITE command*/
    RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);

    /* Check ack value, it should be zero. */
    if (0 != ack)
    {
        RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Write]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
        return RT_ERR_FAILED;
    }

    /* Reg Address Field*/
    for(loop_idx = software_i2c_addr_width[dev]; loop_idx > 0; loop_idx--)
    {
        /* Note: From MSB 8-bit Reg Address */
        RT_ERR_CHK(_general_software_i2c_writeBit(((mAddrs >> ((loop_idx-1)*8)) & 0xFF), 8, dev), ret);
        /* Read ack for issuing WRITE command*/
        RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);
        /* Check ack value, it should be zero. */
        if (0 != ack)
        {
            RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
            return RT_ERR_FAILED;
        }
     }

    /* Reg Data Field*/
    for(loop_idx = 0; loop_idx < software_i2c_data_width[dev]; loop_idx++)
    {
        /* Note: From uint8 array index 0 */
        RT_ERR_CHK(_general_software_i2c_writeBit(((*(pWdata+loop_idx)) & 0xFF), 8, dev), ret);
        /* Read ack for issuing WRITE command*/
        RT_ERR_CHK(_general_software_i2c_readBit(1, &ack, dev), ret);
        /* Check ack value, it should be zero. */
        if (0 != ack)
        {
            RT_LOG(LOG_DEBUG, MOD_GENERAL, "[Read]devid 0x%x addr 0x%x ack 0x%x\n", slave, mAddrs, ack);
            return RT_ERR_FAILED;
        }
     }

    return ret;
} /* end of _general_nbits_dev_write */



/* Function Name:
 *      drv_software_i2c_group_get
 * Description:
 *      SOFTWARE I2C configuration get function
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
int32 drv_software_i2c_group_get(uint32 * pDevSCK, uint32 * pPinSCK, uint32 * pDevSDA, uint32 * pPinSDA, uint32 dev)
{
    RT_INIT_CHK(software_i2c_INIT_FLAG[dev]);

    *pDevSCK = GPIO_PORT(general_software_i2c_SCK[dev].devId);
    *pPinSCK = GPIO_PIN(general_software_i2c_SCK[dev].pinId);
    *pDevSDA = GPIO_PORT(general_software_i2c_SDA[dev].devId);
    *pPinSDA = GPIO_PIN(general_software_i2c_SDA[dev].pinId);

    return RT_ERR_OK;
} /* end of drv_software_i2c_group_get */


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
int32 drv_software_i2c_init(uint32 devSCK, uint32 pinSCK, uint32 devSDA, uint32 pinSDA, uint32 dev)
{
    uint32 unit = HWP_MY_UNIT_ID();
    int32 ret = RT_ERR_FAILED;
    drv_generalCtrlGpio_pinConf_t gpioConfig;

    /* parameter check */

    RT_INIT_MSG("    software i2c init (unit %u)\n", unit);

    RT_PARAM_CHK(!SOFTWARE_I2C_DEVICE_CHK(dev), RT_ERR_OUT_OF_RANGE);

    /* Initialize GPIO port 'portSDA', pin 'pinSDA' as SMI SDA */
    software_i2c_CHIPID[dev] = SFP_CHIPID;
    software_i2c_DELAY[dev] = DELAY_4000;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    RT_ERR_CHK(drv_generalCtrlGPIO_pin_init(unit, devSDA, pinSDA, &gpioConfig), ret);

    general_software_i2c_SDA[dev].devId = devSDA;
    general_software_i2c_SDA[dev].pinId= pinSDA;

    /* Initialize GPIO port 'portSCK', pin 'pinSCK' as SMI SCK */

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    RT_ERR_CHK(drv_generalCtrlGPIO_pin_init(unit, devSCK, pinSCK, &gpioConfig), ret);

    general_software_i2c_SCK[dev].devId = devSCK;
    general_software_i2c_SCK[dev].pinId= pinSCK;

    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, 1), ret);
    RT_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, general_software_i2c_SCK[dev].devId, general_software_i2c_SCK[dev].pinId, 1), ret);

    software_i2c_INIT_FLAG[dev] = INIT_COMPLETED;
    return ret;
} /* end of drv_software_i2c_init */

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
int32 drv_software_i2c_write(uint32 mAddrs, uint8 *pWdata, uint32 dev)
{
    uint32  unit = HWP_MY_UNIT_ID();
    int32 ret = RT_ERR_FAILED;

    /* parameter check */
    RT_PARAM_CHK(!SOFTWARE_I2C_DEVICE_CHK(dev), RT_ERR_OUT_OF_RANGE);
    RT_INIT_CHK(software_i2c_INIT_FLAG[dev]);

    ret = drv_generalCtrlGPIO_direction_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, GPIO_DIR_OUT);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_write_end;
    }

    /* Start I2C */
    ret = _general_nbits_dev_start(dev);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_write_end;
    }

    ret = _general_nbits_dev_write(mAddrs, pWdata, dev);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, MOD_GENERAL, "SOFTWARE I2C write fail!");
    }

    /* Stop I2C */
    ret = _general_nbits_dev_stop(dev);

software_i2c_general_write_end:


    return ret;
} /* end of drv_software_i2c_write */

/* Function Name:
 *      drv_software_i2c_read
 * Description:
 *      SOFTWARE I2C random read wrapper function
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
int32 drv_software_i2c_read(uint32 mAddrs, uint8 *pRdata, uint32 dev)
{
    uint32  unit = HWP_MY_UNIT_ID();
    int32   ret = RT_ERR_FAILED;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRdata), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!SOFTWARE_I2C_DEVICE_CHK(dev), RT_ERR_OUT_OF_RANGE);
    RT_INIT_CHK(software_i2c_INIT_FLAG[dev]);

    ret = drv_generalCtrlGPIO_direction_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, GPIO_DIR_OUT);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_read_end;
    }

    /* Start I2C */
    ret = _general_nbits_dev_start(dev);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_read_end;
    }

    ret = _general_nbits_dev_read(mAddrs, pRdata, dev);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_read_end;
    }
    /* Stop I2C */
    ret = _general_nbits_dev_stop(dev);

software_i2c_general_read_end:

    return ret;
} /* end of drv_software_i2c_read */


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
int32 drv_software_i2c_sequential_read(uint32 mAddrs, uint8 *pRdata, uint32 dev)
{
    uint32  unit = HWP_MY_UNIT_ID();
    int32   ret = RT_ERR_FAILED;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRdata), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!SOFTWARE_I2C_DEVICE_CHK(dev), RT_ERR_OUT_OF_RANGE);
    RT_INIT_CHK(software_i2c_INIT_FLAG[dev]);

    ret = drv_generalCtrlGPIO_direction_set(unit, general_software_i2c_SDA[dev].devId, general_software_i2c_SDA[dev].pinId, GPIO_DIR_OUT);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_sequential_read_end;
    }

    /* Start I2C */
    ret = _general_nbits_dev_start(dev);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_sequential_read_end;
    }

    ret = _general_sequential_nbits_dev_read(pRdata, dev);
    if (RT_ERR_OK != ret)
    {
        goto software_i2c_general_sequential_read_end;
    }
    /* Stop I2C */
    ret = _general_nbits_dev_stop(dev);

software_i2c_general_sequential_read_end:

    return ret;
} /* end of drv_software_i2c_read */


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
int32 drv_software_i2c_module_init(uint32 unit)
{
    int32 i;

    for(i = 0; i < SOFTWARE_I2C_DEVICE_MAX; i++)
        software_i2c_INIT_FLAG[i] = INIT_NOT_COMPLETED;

    return RT_ERR_OK;
} /* end of drv_software_i2c_module_init */


#if defined(__BOOTLOADER__)
/* Function Name:
 *      drv_software_i2c_list
 * Description:
 *      List all initialed devices
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void drv_software_i2c_list(void)
{
    int i;
    osal_printf("\n Index SCK_DEV SCK_PIN SDA_DEV SDA_PIN TYPE   ID   Delay Name \n");
    osal_printf("=================================================================\n");
    for (i = 0; i < SOFTWARE_I2C_DEVICE_MAX; i++)
    {
        if (software_i2c_INIT_FLAG[i] == INIT_COMPLETED)
        {
            osal_printf("    %.2d      %.2d      %.2u      %.2d      %.2u ", i, general_software_i2c_SCK[i].devId, general_software_i2c_SCK[i].pinId,
            general_software_i2c_SDA[i].devId, general_software_i2c_SDA[i].pinId);
            osal_printf(" %.2d  ", (software_i2c_addr_width[i]*8));
            osal_printf("  %x   %.5u  %s\n", software_i2c_CHIPID[i], software_i2c_DELAY[i], software_i2c_NAME[i]);
        }
    }
    return;
}
#endif/* defined(__BOOTLOADER__) */

