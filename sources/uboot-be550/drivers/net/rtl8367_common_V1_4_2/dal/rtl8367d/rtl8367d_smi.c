/*
* Copyright c                  Realtek Semiconductor Corporation, 2006
* All rights reserved.
*
* Program : Control smi connected RTL8366
* Abstract :
* Author : Yu-Mei Pan (ympan@realtek.com.cn)
*  $Id: smi.c,v 1.2 2008-04-10 03:04:19 shiehyy Exp $
*/
#include <rtk_types.h>
#include <rtl8367d_smi.h>
#include "rtk_error.h"

/* Define */
#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_START_REG          29
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_START_OP          0xFFFF
#define MDC_MDIO_ADDR_OP           0x000E
#define MDC_MDIO_READ_OP           0x0001
#define MDC_MDIO_WRITE_OP          0x0003

#define SPI_READ_OP                 0x3
#define SPI_WRITE_OP                0x2
#define SPI_READ_OP_LEN             0x8
#define SPI_WRITE_OP_LEN            0x8
#define SPI_REG_LEN                 16
#define SPI_DATA_LEN                16

#define GPIO_DIR_IN                 1
#define GPIO_DIR_OUT                0

#define ack_timer                   5

#define DELAY                        10000
#define CLK_DURATION(clk)            { int i; for(i=0; i<clk; i++); }


#if defined(MDC_MDIO_OPERATION)
/*******************************************************************************/
/*  MDC/MDIO porting                                                           */
/*******************************************************************************/
/* define the PHY ID currently used */
#define MDC_MDIO_PHY_ID     29  /* PHY ID 0 or 29 */

extern int ipq_mdio_read(int mii_id, int regnum, unsigned short *data);
extern int ipq_mdio_write(int mii_id, int regnum, unsigned short value);

static inline unsigned int rtk_mdio_write(int mii_id, int regnum, unsigned int value)
{
	return ipq_mdio_write(mii_id, regnum, (unsigned short)value);
}

static inline int rtk_mdio_read(int mii_id, int regnum, unsigned int *data)
{
	unsigned short temp_data = 0;

	ipq_mdio_read(mii_id, regnum, &temp_data);

	if (data != NULL)
		*data = temp_data;

	return temp_data;
}

/* MDC/MDIO, redefine/implement the following Macro */
#define MDC_MDIO_WRITE(preamableLength, phyID, regID, data) rtk_mdio_write(phyID, regID, data)
#define MDC_MDIO_READ(preamableLength, phyID, regID, pData) rtk_mdio_read(phyID, regID, pData)

#elif defined(SPI_OPERATION)
/*******************************************************************************/
/*  SPI porting                                                                */
/*******************************************************************************/
/* SPI, redefine/implement the following Macro */
#define SPI_WRITE(data, length)
#define SPI_READ(pData, length)





#else
/*******************************************************************************/
/*  I2C porting                                                                */
/*******************************************************************************/
/* Define the GPIO ID for SCK & SDA */
rtk_uint32  rtl8367d_smi_SCK = 1;    /* GPIO used for SMI Clock Generation */
rtk_uint32  rtl8367d_smi_SDA = 2;    /* GPIO used for SMI Data signal */

/* I2C, redefine/implement the following Macro */
#define GPIO_DIRECTION_SET(gpioID, direction)
#define GPIO_DATA_SET(gpioID, data)
#define GPIO_DATA_GET(gpioID, pData)





#endif

static void rtlglue_drvMutexLock(void)
{
    /* It is empty currently. Implement this function if Lock/Unlock function is needed */
    return;
}

static void rtlglue_drvMutexUnlock(void)
{
    /* It is empty currently. Implement this function if Lock/Unlock function is needed */
    return;
}



#if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION)
    /* No local function in MDC/MDIO & SPI mode */
#else
static void _smi_start(void)
{

    /* change GPIO pin to Output only */
    GPIO_DIRECTION_SET(rtl8367d_smi_SCK, GPIO_DIR_OUT);
    GPIO_DIRECTION_SET(rtl8367d_smi_SDA, GPIO_DIR_OUT);

    /* Initial state: SCK: 0, SDA: 1 */
    GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    GPIO_DATA_SET(rtl8367d_smi_SDA, 1);
    CLK_DURATION(DELAY);

    /* CLK 1: 0 -> 1, 1 -> 0 */
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    CLK_DURATION(DELAY);

    /* CLK 2: */
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SDA, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SDA, 1);

}



static void _smi_writeBit(rtk_uint16 signal, rtk_uint32 bitLen)
{
    for( ; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* prepare data */
        if ( signal & (1<<(bitLen-1)) )
        {
            GPIO_DATA_SET(rtl8367d_smi_SDA, 1);
        }
        else
        {
            GPIO_DATA_SET(rtl8367d_smi_SDA, 0);
        }
        CLK_DURATION(DELAY);

        /* clocking */
        GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
        CLK_DURATION(DELAY);
        GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    }
}



static void _smi_readBit(rtk_uint32 bitLen, rtk_uint32 *rData)
{
    rtk_uint32 u = 0;

    /* change GPIO pin to Input only */
    GPIO_DIRECTION_SET(rtl8367d_smi_SDA, GPIO_DIR_IN);

    for (*rData = 0; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* clocking */
        GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
        CLK_DURATION(DELAY);
        GPIO_DATA_GET(rtl8367d_smi_SDA, &u);
        GPIO_DATA_SET(rtl8367d_smi_SCK, 0);

        *rData |= (u << (bitLen - 1));
    }

    /* change GPIO pin to Output only */
    GPIO_DIRECTION_SET(rtl8367d_smi_SDA, GPIO_DIR_OUT);
}



static void _smi_stop(void)
{

    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SDA, 0);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SDA, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);

    /* add a click */
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(rtl8367d_smi_SCK, 1);


    /* change GPIO pin to Input only */
    GPIO_DIRECTION_SET(rtl8367d_smi_SDA, GPIO_DIR_IN);
    GPIO_DIRECTION_SET(rtl8367d_smi_SCK, GPIO_DIR_IN);
}

#endif /* End of #if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION) */

rtk_int32 rtl8367d_smi_read(rtk_uint32 mAddrs, rtk_uint32 *rData)
{
#if (!defined(MDC_MDIO_OPERATION) && !defined(SPI_OPERATION))
    rtk_uint32 rawData=0, ACK;
    rtk_uint8  con;
    rtk_uint32 ret = RT_ERR_OK;
#endif

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData == NULL)
        return RT_ERR_NULL_POINTER;

    *rData = 0;

#if defined(MDC_MDIO_OPERATION)

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write read control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Read data from register 25 */
    MDC_MDIO_READ(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_DATA_READ_REG, rData);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write 8 bits READ OP_CODE */
    SPI_WRITE(SPI_READ_OP, SPI_READ_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Read 16 bits data */
    SPI_READ(rData, SPI_DATA_LEN);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;

#else

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370 */

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x1, 1);                        /* 1: issue READ command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing READ command*/
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_readBit(8, &rawData);                    /* Read DATA [7:0] */
    *rData = rawData&0xff;

    _smi_writeBit(0x00, 1);                        /* ACK by CPU */

    _smi_readBit(8, &rawData);                    /* Read DATA [15: 8] */

    _smi_writeBit(0x01, 1);                        /* ACK by CPU */
    *rData |= (rawData<<8);

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}



rtk_int32 rtl8367d_smi_write(rtk_uint32 mAddrs, rtk_uint32 rData)
{
#if (!defined(MDC_MDIO_OPERATION) && !defined(SPI_OPERATION))
    rtk_int8 con;
    rtk_uint32 ACK;
    rtk_uint32 ret = RT_ERR_OK;
#endif

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData > 0xFFFF)
        return RT_ERR_INPUT;

#if defined(MDC_MDIO_OPERATION)

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write data to register 24 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write data control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write 8 bits WRITE OP_CODE */
    SPI_WRITE(SPI_WRITE_OP, SPI_WRITE_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Write 16 bits data */
    SPI_WRITE(rData, SPI_DATA_LEN);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;
#else

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370*/

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x0, 1);                        /* 0: issue WRITE command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing WRITE command*/
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);                /* Write Data [7:0] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);                    /* Write Data [15:8] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                        /* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}

