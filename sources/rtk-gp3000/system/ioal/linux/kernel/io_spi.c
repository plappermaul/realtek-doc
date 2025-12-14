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
 * Purpose : For linux kernel mode
 *           I/O read/write APIs using SPI interface in the SDK.
 *
 * Feature : I/O read/write APIs, by access swich register by SPI interface
 *
 */

#if defined(LINUX_KERNEL_SPI_IO)
/*
 * Include Files
 */
#include <linux/kernel.h>
#include <common/error.h>
#include <common/util.h>
#include <ioal/ioal_init.h>
#include <ioal/io_mii.h>
#include <osal/time.h>
#include <osal/spl.h>

/*
 * Symbol Definition
 */
#if defined(LINUX_KERNEL_SPI_IO_9607C)
    #define GPIO_SOC_ADDR_FOR_CS   0xB800330C
#elif defined(LINUX_KERNEL_SPI_IO_6266)
    #define GPIO_SOC_ADDR_FOR_CS   0xBB0000D8
#else
    #define GPIO_SOC_ADDR_FOR_CS   0xBB000100
#endif

#ifdef RTL_SPI_SLOW_CLK
    #define RTL_SPI_CLK_DIV (0x3F)
#else
    #define RTL_SPI_CLK_DIV (0xF)
#endif

#define SOC_BOARD_GPIO_SPI_CS0_PIN  (1<<FPGA_SPI_CS_GPIO_PIN)

/*
 * Data Declaration
 */
static int32 isInit = 0;

static uint32 GPIO_SPI_CS0_PIN;


typedef struct rtl_8676_reg
{
    volatile unsigned int SPICNR;
    volatile unsigned int SPISTR;
    volatile unsigned int SPICKDIV;
    volatile unsigned int SPIRDR;
    volatile unsigned int SPITCR;    /*transaction config. reg. */
    volatile unsigned int SPICDTCR0; /*cs deselect time counter reg. 0 */
    volatile unsigned int SPICDTCR1; /*cs deselect time counter reg. 1 */
    volatile unsigned int SPITCALR;  /*SPI Timing Calibration Register */
} rtl_8676_reg_t ;

/*
 * Macro Declaration
 */

 /*-- SPICNR --*/
#define SPI_CSTIME(i)           ((i) << 26)   /* 0: 8-bit times , 1: 16-bit times of CS raise time */
#define SPI_WRIE(i)             ((i) << 25)   /* 0: Disable , 1: Enable write ok interrupt */
#define SPI_RDIE(i)             ((i) << 24)   /* 0: Disable , 1: Enable read ok interrupt */
#define SPI_LSB(i)              ((i) << 7)    /* 0: MSB, 1: LSB*/
#define SPI_CMD(i)              ((i) << 5)    /* 0: read, 1: write*/
#define SPI_START(i)            ((i) << 4)    /* 1: Start transfer*/
#define SPI_SCLK_TYPE(i)        ((i) << 3)    /*SCLK TYPE 0: type I SCLK, 1: type II SCLK*/
#define SPI_CSP(i)              ((i) << 1)    /* 0: Low active, 1: High active, */



 /*-- SPISTR --*/
#define SPI_RDIP(i)             ((i) << 31)   /* Read ok interrupt pending bit, 1 to clear  */
#define SPI_WDIP(i)             ((i) << 30)   /* Write ok interrupt pending bit, 1 to clear  */

/*-- SPICLKDIV --*/
#define SPI_DIV(i)              ((i) << 24)   /* Clock divisor, 0: /2, 1: /4, 2: /6, ... , 255: /512*/

/*-- SPITCR --*/
#define SPI_SEL_CS(i)           (1 << (31-(i))) /*select SPI chip 0~5*/
#define SPI_CTL_EN(i)           ((i) << 23)   /* CTL_EN*/
#define SPI_ADD_EN(i)           ((i) << 22)   /* ADD_EN*/
#define SPI_D0_EN(i)            ((i) << 21)   /* D0_EN*/
#define SPI_D1_EN(i)            ((i) << 20)   /* D1_EN*/


#define APOLLOMP_SPI_SEL_CS   0

#define RTL8676_SPI_BASE                         0xB8009000

#define get_spi_reg() (rtl_8676_reg_t *)(RTL8676_SPI_BASE)


int32 spi_init(void)
{
    GPIO_SPI_CS0_PIN = SOC_BOARD_GPIO_SPI_CS0_PIN;

    rtl_8676_reg_t *spi_reg=get_spi_reg();

#if defined(LINUX_KERNEL_SPI_IO_9607C)
    /*IP Enable*/
    *((uint32 *)0xB8000600) |= 0x1;
#endif

    /*Initialize Registers*/
    spi_reg->SPICKDIV = SPI_DIV(RTL_SPI_CLK_DIV);

    spi_reg->SPITCR = SPI_D0_EN(1) | 0x1823;

    isInit = 1;

#if defined(LINUX_KERNEL_SPI_IO_9607C)
    /*using gpio pin as CS*/
    *((uint32 *)0xB8003308) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xB800330C) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xB8003300) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xBB000038) = GPIO_SPI_CS0_PIN;
#elif defined(LINUX_KERNEL_SPI_IO_6266)
    /*using gpio pin as CS*/
    *((uint32 *)0xBB0000D8) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xBB0000FC) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xBB0000F0) = GPIO_SPI_CS0_PIN;
#else
    /*using gpio pin as CS*/
    *((uint32 *)0xBB000100) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xBB0001d4) = GPIO_SPI_CS0_PIN;
    *((uint32 *)0xBB000118) = GPIO_SPI_CS0_PIN;
#endif
    /* enable SPI clock */
    *((uint32 *)0xB8000600) |= 5;


    return RT_ERR_OK;
}


void spi_write(unsigned int data)
{
    unsigned int               spicnr;

    rtl_8676_reg_t *spi_reg=get_spi_reg();
    int flags;

    if(0==isInit)
        spi_init();

    /*trans. conf*/
    spi_reg->SPITCR &= ~((0x3f) << 26); /*clear chip selection*/
    spi_reg->SPITCR |= SPI_SEL_CS( APOLLOMP_SPI_SEL_CS );

    /*clear all interrupt*/
    spi_reg->SPISTR = SPI_RDIP(1)|SPI_WDIP(1);/*0xC0000000*/

    /*ctrl*/
    spicnr = SPI_CSTIME(0) | SPI_WRIE(1)/*| SPI_RDIE(1)*/| SPI_LSB(0) | SPI_CMD(1) | SPI_START(1) | SPI_CSP(0);

    /*data*/
    spi_reg->SPIRDR = (data) << 24;

    /*start transfer*/
    spi_reg->SPICNR = spicnr;

    /*wait unitl finish*/
    while ((spi_reg->SPISTR & SPI_WDIP(1))  == 0) ;

    spi_reg->SPISTR |= SPI_WDIP(1);

    while ((spi_reg->SPICNR & SPI_START(1)) != 0) ;
}



void spi_read(unsigned char *data)
{
    unsigned int               spicnr;
    uint8 buf;
    rtl_8676_reg_t *spi_reg=get_spi_reg();
    int flags;

    if(0==isInit)
        spi_init();

    /*trans. conf*/
    spi_reg->SPITCR &= ~((0x3f) << 26); /*clear chip selection*/
    spi_reg->SPITCR |= SPI_SEL_CS( APOLLOMP_SPI_SEL_CS );

    /*clear all interrupt*/
    spi_reg->SPISTR = 0xC0000000;

    /*ctrl*/
    spicnr = SPI_CSTIME(0)/*| SPI_WRIE(1)*/| SPI_RDIE(1) | SPI_LSB(0) | SPI_CMD(0) | SPI_START(1) | SPI_CSP(0);

    /*clear data*/
    spi_reg->SPIRDR = 0x0;

    /*start transfer*/
    spi_reg->SPICNR = spicnr;

    /*wait unitl finish*/
    while ((spi_reg->SPISTR & SPI_RDIP(1))  == 0) ;

    spi_reg->SPISTR |= SPI_RDIP(1);

    while ((spi_reg->SPICNR & SPI_START(1)) != 0) ;

    buf = (unsigned char) (spi_reg->SPIRDR >> 24);
    *data = buf;
}




int io_spi_phyReg_write(uint8 phyId,uint8 reg, uint16 value)
{
    uint32 tmpData;
    rtl_8676_reg_t *spi_reg=get_spi_reg();


    *((uint32 *)GPIO_SOC_ADDR_FOR_CS) &= ~(GPIO_SPI_CS0_PIN);

    spi_write(0x02);
    /*address*/
    spi_write(phyId);
    spi_write(reg);

    /*data [15:8]*/
    tmpData = value >>8;
    tmpData = tmpData&0xFF;
    spi_write((uint8)tmpData);


    /*data [7:0]*/
    tmpData = value&0x00FF;
    spi_write((uint8)tmpData);

    /*trans. conf*/
    spi_reg->SPITCR &= ~((0x3f) << 26); /*clear chip selection*/
    spi_reg->SPITCR |= SPI_SEL_CS( APOLLOMP_SPI_SEL_CS );

    *((uint32 *)GPIO_SOC_ADDR_FOR_CS) |= GPIO_SPI_CS0_PIN;

    return RT_ERR_OK;
}

int io_spi_phyReg_read(uint8 phyId,uint8 reg, uint16 *value)
{
    uint16 tmpData=0;
    uint8  spiData;
    rtl_8676_reg_t *spi_reg=get_spi_reg();

    *((uint32 *)GPIO_SOC_ADDR_FOR_CS) &= ~(GPIO_SPI_CS0_PIN);

    spi_write(0x03);
    /*address*/
    spi_write(phyId);
    spi_write(reg);


    /*data [15:8]*/
    spi_read(&spiData);
    tmpData = spiData<<8;

    /*data [7:0]*/
    spi_read(&spiData);
    tmpData = tmpData|spiData;

    *value = tmpData;

    /*trans. conf*/
    spi_reg->SPITCR &= ~((0x3f) << 26); /*clear chip selection*/
    spi_reg->SPITCR |= SPI_SEL_CS( APOLLOMP_SPI_SEL_CS );


    *((uint32 *)GPIO_SOC_ADDR_FOR_CS) |= GPIO_SPI_CS0_PIN;

    return RT_ERR_OK;
}

#endif

