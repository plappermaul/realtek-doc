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
 * Purpose : 8390 SPI master driver to 8380 slave.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spi read and write
 *
 */
#include <common/rt_autoconf.h>

#include <osal/sem.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_dev1.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <hal/chipdef/chip.h>
#include <ioal/ioal_init.h>
#include <hwp/hw_profile.h>

static uint32  spi_reg_base;
static uint8   spi_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

#define SFRB   (spi_reg_base)   /*SPI Flash Register Base*/
#define SFCR   (SFRB)       /*SPI Flash Configuration Register*/
    #define SFCR_CLK_DIV(val)       ((val)<<29)
    #define SFCR_CLK_DIV_CLEAR      (0x1FFFFFFF)
    #define SFCR_EnableRBO          (1<<28)
    #define SFCR_EnableWBO          (1<<27)
    #define SFCR_SPI_TCS(val)       ((val)<<22) /*5 bit, 11111 */
#define SFCR2  (SFRB+0x04)          /*For memory mapped I/O */
    #define SFCR2_SFCMD(val)        ((val)<<24) /*8 bit, 1111_1111 */
    #define SFCR2_SIZE(val)         ((val)<<21) /*3 bit, 111 */
    #define SFCR2_RDOPT             (1<<20)
    #define SFCR2_CMDIO(val)        ((val)<<18) /*2 bit, 11 */
    #define SFCR2_ADDRIO(val)       ((val)<<16) /*2 bit, 11 */
    #define SFCR2_DUMMYCYCLE(val)   ((val)<<13) /*3 bit, 111 */
    #define SFCR2_DATAIO(val)       ((val)<<11) /*2 bit, 11 */
    #define SFCR2_HOLD_TILL_SFDR2   (1<<10)
    #define SFCR2_GETSIZE(x)        (((x)&0x00E00000)>>21)

#define SFCSR  (SFRB+0x08)  /*SPI Flash Control&Status Register*/
    #define SFCSR_SPI_CSB0          (1<<31)
    #define SFCSR_SPI_CSB1          (1<<30)
    #define SFCSR_SPI_CSB2          (1<<15)
    #define SFCSR_SPI_CSB3          (1<<14)
    #define SFCSR_SPI_CSALL         (SFCSR_SPI_CSB0|SFCSR_SPI_CSB1|SFCSR_SPI_CSB2|SFCSR_SPI_CSB3)
    #define SFCSR_LEN(val)          ((val)<<28)  /*2 bits*/
    #define SFCSR_SPI_RDY           (1<<27)
    #define SFCSR_IO_WIDTH(val)     ((val)<<25)  /*2 bits*/
    #define SFCSR_CHIP_SEL          (1<<24)
    #define SFCSR_CMD_BYTE(val)     ((val)<<16)  /*8 bit, 1111_1111 */

#define SFDR                        (SFRB+0x0C) /*SPI Flash Data Register*/
#define SFDR2                       (SFRB+0x10) /*SPI Flash Data Register - for post SPI bootup setting*/

#define SPI_CS_INIT                 (SFCSR_SPI_CSALL | SPI_LEN1)
#define SPI_WIP                     (1)                 /* Write In Progress */
#define SPI_WEL                     (1<<1)              /* Write Enable Latch*/
#define SPI_SST_QIO_WIP             (1<<7)              /* SST QIO Flash Write In Progress */
#define SPI_LEN_INIT                0xCFFFFFFF          /* and SFCSR to init   */
#define SPI_LEN4                    0x30000000          /* or SFCSR to set */
#define SPI_LEN3                    0x20000000          /* or SFCSR to set */
#define SPI_LEN2                    0x10000000          /* or SFCSR to set */
#define SPI_LEN1                    0x00000000          /* or SFCSR to set */
#define SPI_SETLEN(val)             ((val-1)<<28)

typedef enum{
    R_MODE      =0x04,
    CS0         =0x10,
    CS1         =0x20,
    CS2         =0x40,
    CS3         =0x80
}spi_flags;

typedef enum{
    SPI_C_READ  = 0x03,     /* Read */
    SPI_C_FREAD = 0x0B,     /* Fast Read */
    SPI_C_RDID  = 0x9F,     /* Read ID */
    SPI_C_WREN  = 0x06,     /* Write Enable */
    SPI_C_WRDI  = 0x04,     /* Write Disable */
    SPI_C_BE    = 0xD8,     /* Block Erase */
    SPI_C_CE    = 0xC7,     /* Chip Erase */
    SPI_C_SE    = 0x20,     /* Sector Erase */
    SPI_C_PP    = 0x02,     /* Page Program */
    SPI_C_RDSR  = 0x05,     /* Read Status Register */
    SPI_C_WRSR  = 0x01,     /* Write Status Register */
    SPI_C_DP    = 0xB9,     /* Deep Power Down */
    SPI_C_RES   = 0xAB,     /* Read Electronic ID */
    SPI_C_AAI   = 0xAD      /* Auto Address Increment*/
}spi_cmdType_t;

typedef struct spi_cmd_s{
    uint32        flags;
    spi_cmdType_t cmd_t;
    uint32        address;
    uint8         *buf;     /*request buffer*/
    uint32        size;     /*request IO size*/
}spi_cmd_t;

#if (RT_SPI_USE_FLASH_SEMA == 1) && defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) && defined(__KERNEL__)
  #include <linux/version.h>
  #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
    #include <asm/semaphore.h>
  #else
    #include <linux/semaphore.h>
  #endif
  #include <linux/slab.h>
  extern struct semaphore     spi_bus_semaphore;
  #define SPI_SEM_LOCK()      down(&spi_bus_semaphore)
  #define SPI_SEM_UNLOCK()    up(&spi_bus_semaphore)
#else
  #define SPI_SEM_LOCK()
  #define SPI_SEM_UNLOCK()
#endif

#define SPI_REG(reg)        *((volatile uint32 *)(reg))
#define CHECK_READY()       do{while( !(SPI_REG(SFCSR)&SFCSR_SPI_RDY) );}while(0)


/* Function Name:
 *      _spi_cmd_exec
 * Description:
 *      All SPI commands
 *      Control the SFCSR and SFDR.
 * Input:
 *      Unit ID(unit)
 *      SPI request command(req)
 * Output:
 *      None
 * Return:
 *      None
 *      Device ID
 * Note:
 *      The basic flash driver function
 *      Support Commands:
 *        Write Enable, Write Disable, Chip Erase, Power Down,
 *        Write Status Register, Read Status Register, Read Device ID,
 *        Read, Block Erase, Auto Address Increment,
 *        Page Program
 */
static int _spi_cmd_exec(uint32 unit, spi_cmd_t *req)
{
    uint32 addr = 0;
    uint32 size = req->size;
    uint8 *buf = (uint8 *)req->buf;
    uint32 sfcsr_value = 0, sfdr_value = 0;
    uint32 ret = 0;
    uint32 cs_active=0xFFFFFFFF;

    /* sfcar default value */
    sfcsr_value = (SPI_SETLEN(1)|SFCSR_SPI_CSALL|SFCSR_IO_WIDTH(0));

    /* chip select */
    if(CS0 & req->flags){
        cs_active = (~SFCSR_SPI_CSB0);
    }
    if(CS1 & req->flags){
        cs_active = (~SFCSR_SPI_CSB1);
    }
    if(CS2 & req->flags){
        cs_active = (~SFCSR_SPI_CSB2);
    }
    if(CS3 & req->flags){
        cs_active = (~SFCSR_SPI_CSB3);
    }

    /* set command and address */
    sfcsr_value &= cs_active;
    sfdr_value = (req->cmd_t)<<24;
    addr = req->address;

    /* Memory Controller Constrain, deactive, active, deactive SPI Flash */
    CHECK_READY();
    SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
    CHECK_READY();
    SPI_REG(SFCSR) &= cs_active;    /* active CS */
    CHECK_READY();
    SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
    CHECK_READY();

    switch( req->cmd_t )
    {
    /*No Address/Dummy Bytes, data length = 1*/
        case SPI_C_WREN:
        case SPI_C_WRDI:
        case SPI_C_CE:
        case SPI_C_DP:
            /* Write 1 byte command & 0 byte data, read 0 byte data */
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            break;

        case SPI_C_WRSR:
            /* Write 1 byte command & 1 byte data, read 0 byte data */
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value | SPI_LEN2;
            sfdr_value |= addr;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            break;
        case SPI_C_RDSR:
        case SPI_C_RDID:
            /* Write 1 byte command & 1 byte data, read 4 byte data */
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            CHECK_READY();
            SPI_REG(SFCSR) |= SPI_LEN4;
            CHECK_READY();
            ret = SPI_REG(SFDR);
            break;
        case SPI_C_READ:
            /* Write 1 byte command & 3 byte addr */
            sfcsr_value |=SPI_LEN4;
            sfdr_value |= addr;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            /* Read data, 4 byte/iteration */
            while( size>=4 ){
                CHECK_READY();
                *((uint32*) buf) = SPI_REG(SFDR);
                buf+=4;
                size-=4;
            }

            /* Read data, 1 byte/iteration */
            sfcsr_value &= SPI_LEN_INIT|SPI_LEN1;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value;
            while( size>0 ){
                CHECK_READY();
                *(buf) = SPI_REG(SFDR)>>24;
                buf++;
                size--;
            }
            break;
        case SPI_C_FREAD:
            /* Fast Read, not Implement */
            sfcsr_value |=SPI_LEN4;
            sfdr_value |= addr;
            break;
        case SPI_C_BE:
            /* Write 1 byte command & 3 byte addr(erased address) */
            sfcsr_value |=SPI_LEN4;
            sfdr_value |= addr;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            break;
        case SPI_C_AAI:
            /* Auto Address Increment */
            if( 0==size ){
                break;
            }
            /* Write 1 byte command & 3 byte addr */
            sfdr_value |= addr;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value | SPI_LEN4;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value | SPI_LEN2;

            /*
             * Write 1 or 2 byte data at the first time
             */
            /* only write 1 byte data */
            if( 1==size ){
                CHECK_READY();
                SPI_REG(SFDR) = (((uint32)(*((uint16*) buf)))<<16 | 0x00FF0000);
                buf += 1;
                size = 0;
                CHECK_READY();
                req->cmd_t = SPI_C_WRDI;
                _spi_cmd_exec(unit, req);
                CHECK_READY();
                break;
            }
            CHECK_READY();

            /* only write 2 byte data */
            SPI_REG(SFDR) = ((uint32)(*((uint16*) buf)))<<16;
            buf+=2;
            size-=2;

            /* Write data, 2 byte/iteration */
            while( size>=2 ){
                CHECK_READY();
                SPI_REG(SFCSR) = SPI_CS_INIT;
                CHECK_READY();
                SPI_REG(SFCSR) = sfcsr_value | SPI_LEN3;
                CHECK_READY();
                SPI_REG(SFDR) = (SPI_C_AAI<<24) | ((uint32)(*((uint16*) buf)))<<8;
                buf+=2;
                size-=2;
            }

            /* Write data, 1 byte/iteration */
            while( size>0 ){
                CHECK_READY();
                SPI_REG(SFCSR) = SPI_CS_INIT;
                CHECK_READY();
                SPI_REG(SFCSR) = sfcsr_value | SPI_LEN3;
                /* or 0x0000FF00 to keep the original data in spi flash (AAI mode only)*/
                CHECK_READY();
                SPI_REG(SFDR) = (SPI_C_AAI<<24) | ((uint32)(*((uint16*) buf)))<<8 | 0x0000FF00;
                buf++;
                size--;
            }

            /* Wait Ready */
            CHECK_READY();
            req->cmd_t = SPI_C_WRDI;
            _spi_cmd_exec(unit, req);
            CHECK_READY();
            break;
        case SPI_C_PP:
            /* Write 1 byte command & 3 byte addr */
            sfdr_value |= addr;
            CHECK_READY();
            SPI_REG(SFCSR) = sfcsr_value | SPI_LEN4;
            CHECK_READY();
            SPI_REG(SFDR) = sfdr_value;

            /* Write data, 4 byte/iteration */
            if( 0==(((int)buf)%sizeof(uint32)) ){
                while( sizeof(uint32)<=size ){
                    CHECK_READY();
                    SPI_REG(SFDR) = *((uint32*) buf);
                    buf  += sizeof(uint32);
                    size -= sizeof(uint32);
                }
            }
            CHECK_READY();

            /* Write data, 1 byte/iteration */
            SPI_REG(SFCSR) = sfcsr_value | SPI_LEN1;
            while( size>0 ){
                CHECK_READY();
                SPI_REG(SFDR) = ((uint32)(*buf)) << 24;
                buf++;
                size--;
            }

            /* Wait Ready */
            CHECK_READY();
            break;
        case SPI_C_RES:
            /* Not implement */
            sfcsr_value |= SPI_LEN4;
            break;
        default:
            break;
    };

    /* Memory Controller Constrain, deactive, active, deactive SPI Flash,
     * MMIO read Flash data
     */
    CHECK_READY();
    SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
    CHECK_READY();
    SPI_REG(SFCSR) &= cs_active;    /* active CS*/
    CHECK_READY();
    SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
    CHECK_READY();

    return ret;
}

static int32 _drv_spi_write(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    osal_memset(&cmd, 0, sizeof(cmd));
    switch(cs)
    {
    case SPI_CS_PIN0:
        cmd.flags |= CS0;
        break;
    case SPI_CS_PIN1:
        cmd.flags |= CS1;
        break;
    case SPI_CS_PIN2:
        cmd.flags |= CS2;
        break;
    case SPI_CS_PIN3:
        cmd.flags |= CS3;
        break;
    default:
        return RT_ERR_FAILED;
    }

    cmd.address = mAddrs<<8;   /*for swcore register address, the LSB 8bit is dummy*/
    cmd.buf = (uint8 *)buff;
    cmd.size = 4;
    cmd.cmd_t = SPI_C_PP;

//    SPI_SEM_LOCK();
    _spi_cmd_exec(unit, &cmd);
//    SPI_SEM_UNLOCK();

    return RT_ERR_OK;
}


static int32 _drv_spi_read(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    osal_memset(&cmd, 0, sizeof(cmd));
    switch(cs)
    {
    case SPI_CS_PIN0:
        cmd.flags |= CS0;
        break;
    case SPI_CS_PIN1:
        cmd.flags |= CS1;
        break;
    case SPI_CS_PIN2:
        cmd.flags |= CS2;
        break;
    case SPI_CS_PIN3:
        cmd.flags |= CS3;
        break;
    default:
        return RT_ERR_FAILED;
    }

    cmd.address = mAddrs<<8;   /*for swcore register address, the LSB 8bit is dummy*/
    cmd.buf = (uint8 *)buff;
    cmd.size = 4;
    cmd.cmd_t = SPI_C_READ;

    //SPI_SEM_LOCK();
    _spi_cmd_exec(unit, &cmd);
    //SPI_SEM_UNLOCK();

    return RT_ERR_OK;
}
/*
 * _drv_spi_read() is real spi read function, spi_dev1Reg_read() tries to call it twice or more
 *  to workaround for the unstability of SPI bus. Same with the spi_dev1Reg_write()
 */
int32 spi_dev1Reg_read(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff)
{
    uint32 spi_retry=0;
    uint32 firstRead;

    SPI_SEM_LOCK();
    do
    {
        spi_retry=0;
        _drv_spi_read(unit, cs, mAddrs, buff);
        firstRead = *buff;
        _drv_spi_read(unit, cs, mAddrs, buff);
        if(firstRead!=*buff)
            spi_retry=1;
    }while(spi_retry);
    SPI_SEM_UNLOCK();

    return RT_ERR_OK;
}

int32 spi_dev1Reg_write(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff)
{
    uint32 spi_retry=0;
    uint32 firstRead;
    uint32 secondRead;

    SPI_SEM_LOCK();
    do
    {
        spi_retry=0;
        _drv_spi_write(unit, cs,mAddrs,buff);
        _drv_spi_read(unit, cs,mAddrs, &firstRead);
        _drv_spi_write(unit, cs,mAddrs,buff);
        _drv_spi_read(unit, cs,mAddrs, &secondRead);
        if(firstRead!=secondRead)
            spi_retry=1;
    }while(spi_retry);
    SPI_SEM_UNLOCK();

    return RT_ERR_OK;
}


int32 spi_dev1_init(uint32 unit)
{
    unsigned int    temp;
    RT_INIT_REENTRY_CHK(spi_init[unit]);
    RT_INIT_MSG("    SPI of Device Type 1 init (unit %u)\n",unit);

    if( ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &temp) != RT_ERR_OK)
        return RT_ERR_FAILED;

    spi_reg_base = temp + 0x1200;

    /* Set flash endian as big-endian */
    SPI_REG(SFCR) |= (SFCR_EnableWBO);
    SPI_REG(SFCR) |= (SFCR_EnableRBO);

    /* CLI_DIV = 6 */
    SPI_REG(SFCR) &= SFCR_CLK_DIV_CLEAR;
    SPI_REG(SFCR) |= SFCR_CLK_DIV(0x7);

    /* Config SPI attribute  */
  #if defined(CONFIG_SDK_RTL8390)
    if((HWP_MY_CHIP_FAMILY_ID()==RTL8390_FAMILY_ID)
        ||(HWP_MY_CHIP_FAMILY_ID()==RTL8350_FAMILY_ID))
    {
        /* set cs 2,3 from GPIO to SPI */
        ioal_mem32_read(unit,RTL8390_MAC_IF_CTRL_ADDR,&temp);
        temp |= RTL8390_MAC_IF_CTRL_SPI_CS_IF_SEL_MASK;
        ioal_mem32_write(unit,RTL8390_MAC_IF_CTRL_ADDR,temp);

        /* set driving current */
        ioal_mem32_read(unit,RTL8390_MAC_IO_DRIV_ABLTY_ADDR,&temp);
        temp |= RTL8390_MAC_IO_DRIV_ABLTY_SPI_IO_DRIV_SEL_MASK;
        temp |= RTL8390_MAC_IO_DRIV_ABLTY_SPI_CLK_DRIV_SEL_MASK;
        ioal_mem32_write(unit,RTL8390_MAC_IO_DRIV_ABLTY_ADDR,temp);
    }
  #endif
    spi_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}



