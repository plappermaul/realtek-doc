/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
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
 *           I/O read/write APIs using MDIO interface in the SDK.
 *
 * Feature : I/O read/write APIs, by access swich register by MDIO interface
 *
 */

#if defined(RTL_RLX_IO) || defined(LINUX_KERNEL_MDIO_IO) || defined(CYGWIN_MDIO_IO)
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
#include <osal/print.h>

/*
 * Symbol Definition
 */
#define MII_ADDR  10

#define MII_REG0 0
#define MII_REG1 1
#define MII_REG2 2
#define MII_REG3 3
#define MII_REG4 4
#define MII_REG5 5
#define MII_REG6 6




#if defined(LINUX_KERNEL_SPI_IO)
    #include <ioal/io_spi.h>
    #define IO_MII_PHYREG_READ     io_spi_phyReg_read
    #define IO_MII_PHYREG_WRITE    io_spi_phyReg_write
#else
extern void miiar_read(unsigned char phyaddr,unsigned char regaddr,unsigned short *value);
extern void miiar_write(unsigned char phyaddr,unsigned char regaddr,unsigned short value);

    #define IO_MII_PHYREG_READ     miiar_read
    #define IO_MII_PHYREG_WRITE    miiar_write
#endif


/*
 * Data Declaration
 */
osal_spinlock_t mii_spinlock;
int32 mii_lock=0;
int32 lock_reentry=0;
static spinlock_t i2c_mii_spinlock;
/*
 * Macro Declaration
 */
#if defined(LINUX_KERNEL_I2C_IO)

#define RTL960X_I2C_BUS0_CTRL_ADDR                0xBB023004
#define RTL960X_IO_CTRL_ADDR                      0xBB023018
#define RTL960X_I2C_BUS0_CTRL_ADDR                0xBB023004
#define RTL960X_I2C_BUS0_CTRL_ADDR                0xBB023004
#define RTL960X_I2C_IND_WD                        0xBB000028
#define RTL960X_I2C_IND_CMD                       0xBB000030
#define RTL960X_I2C_IND_RD                        0xBB000038

void io_mii_i2cMemIoInit(void)
{
    uint32 data;

    *((uint32 *)RTL960X_I2C_BUS0_CTRL_ADDR) = 0x55407;
    data = *((uint32 *)RTL960X_IO_CTRL_ADDR);
    data = data | 0x2000;
    *((uint32 *)RTL960X_IO_CTRL_ADDR)= data;

    i2c_mii_spinlock = SPIN_LOCK_UNLOCKED;
}

void io_mii_i2cBusy_check(void)
{
    uint32 data;
    uint32 i;

    i=0;
    do{
        if(i>10000)
        {
            printk("timeout %d  ~~\n",__LINE__);
            break;
        }
        data =  *((uint32 *)RTL960X_I2C_IND_RD);
        i++;
    }while(data&(1<<16));
}

void io_mii_i2cFpgaBusy_check(void)
{
    uint32 i;
    uint32 data;

    i=0;
    do{
        if(i>100000)
        {
            printk("fpga timeout %d  ~~\n",__LINE__);
            break;
        }
        *((uint32 *)RTL960X_I2C_IND_CMD) = 0x10000;
        io_mii_i2cBusy_check();
        data =  *((uint32 *)RTL960X_I2C_IND_RD);
        i++;

    }while((data & 0x4)==0x4);
}

static int i2cmem_wr_flag=0;
uint32 io_mii_i2cMemRead(uint32 memaddr)
{
    uint32 data,rData;
    unsigned long flags;

    spin_lock_irqsave(&i2c_mii_spinlock, flags);

    i2cmem_wr_flag++;

    io_mii_i2cBusy_check();

    /*register set 0x28 $regaddrl         ; # the address [15:0]*/
    data = memaddr&0x0000FFFF;
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*set the register address to read
    register set 0x30 0x30002
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30002;
    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    register set 0x28 $regaddrh         ; # the address [23:16]
    */
    data = (memaddr&0x00FF0000)>>16;
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*
    register set 0x30 0x30012
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30012;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    trigger the read cmd
    register set 0x28 0x1               ; # the read cmd
    */
    *((uint32 *)RTL960X_I2C_IND_WD) = 0x1;
    /*
    register set 0x30 0x30000
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30000;

    /*wait ready*/
    io_mii_i2cBusy_check();

    io_mii_i2cFpgaBusy_check();

    /*
    register set 0x30 0x10001           ; # the data [15:0] read back
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x10001;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    register get 0x38
    */
    data =  *((uint32 *)RTL960X_I2C_IND_RD);
    rData = data&0x0000FFFF;
    /*wait ready*/

    /*
    register set 0x30 0x10011           ; # the data [31:23] read back
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x10011;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    register get 0x38
    */
    data =  *((uint32 *)RTL960X_I2C_IND_RD);
    rData = rData|((data&0x0000FFFF)<<16);

    i2cmem_wr_flag--;

    if(i2cmem_wr_flag!=0)
    {
        osal_printf("io_mii_i2cMemRead [0x%8.8x]=%x\n\r, i2cmem_wr_flag=%d\n\r",memaddr, rData, i2cmem_wr_flag);
    }

    spin_unlock_irqrestore (&i2c_mii_spinlock, flags);

    return rData;

}

uint32 io_mii_i2cMemWrite(uint32 memaddr,uint32 wData)
{
    uint32 data;
    unsigned long flags;

    spin_lock_irqsave(&i2c_mii_spinlock, flags);

    i2cmem_wr_flag++;
    /*
    # set the register address to write
    register set 0x28 $regaddrl         ; # the address [15:0]
    */
    data = (memaddr&0x0000FFFF);
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*
    register set 0x30 0x30002
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30002;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    register set 0x28 $regaddrh         ; # the address [23:16]
    */
    data = (memaddr&0x00FF0000)>>16;
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*
    register set 0x30 0x30012
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30012;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    # set the register data to write
    register set 0x28 $datal         ; # the data [15:0] to write
    */
    data = (wData&0x0000FFFF);
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*
    register set 0x30 0x30003
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30003;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    register set 0x28 $datah         ; # the data [23:16] to write
    */
    data = (wData&0xFFFF0000)>>16;
    *((uint32 *)RTL960X_I2C_IND_WD) = data;

    /*
    register set 0x30 0x30013
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30013;

    /*wait ready*/
    io_mii_i2cBusy_check();

    /*
    # trigger the read cmd
    register set 0x28 0x3               ; # the write cmd
    */
    *((uint32 *)RTL960X_I2C_IND_WD) = 0x3;

    /*
    register set 0x30 0x30000
    */
    *((uint32 *)RTL960X_I2C_IND_CMD) = 0x30000;

    io_mii_i2cBusy_check();

    io_mii_i2cFpgaBusy_check();

    i2cmem_wr_flag--;

    if(i2cmem_wr_flag!=0)
    {
        osal_printf("io_mii_i2cMemWrite [0x%8.8x]=%x\n\r, i2cmem_wr_flag=%d\n\r",memaddr, wData, i2cmem_wr_flag);
    }

    spin_unlock_irqrestore (&i2c_mii_spinlock, flags);

    return RT_ERR_OK;
}
#endif


void io_mii_init(void)
{
#if defined(LINUX_KERNEL_SPI_IO)
    spi_init();
#endif

#if defined(LINUX_KERNEL_I2C_IO)
    io_mii_i2cMemIoInit();
#endif


}

void io_mii_deinit(void)
{
}

int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value)
{
    osal_spl_spin_lock(&mii_spinlock);

    IO_MII_PHYREG_WRITE(phy_id,reg,value);
    osal_time_mdelay(10);

    osal_spl_spin_unlock(&mii_spinlock);
    return RT_ERR_OK;

}

int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue)
{
    osal_spl_spin_lock(&mii_spinlock);

    IO_MII_PHYREG_READ(phy_id,reg,pValue);

    osal_spl_spin_unlock(&mii_spinlock);
    return RT_ERR_OK;
}


int io_mii_memory_write(uint32 memaddr,uint32 data)
{
#if 0
    unsigned short value;
    value = (unsigned short)data;
    IO_MII_PHYREG_WRITE(8,(unsigned char) memaddr,value);
    printk("\nio_mii_memory_write:0x%x value:0x%x\n",memaddr,value);

    return RT_ERR_OK;
#else
    unsigned short value;
    uint32 tmp_data;

    //printk("\nio_mii_memory_write:0x%x\n",memaddr);


#if defined(LINUX_KERNEL_I2C_IO)
    uint32 spiIO=0;

    if((memaddr >= 0x1BF00000) && (memaddr <= 0x1BFFFFFF))
    {
        spiIO = 1;
    }
    else if((memaddr & 0xFF000000) == 0x18000000)
    {
        spiIO = 1;
    }
    else
    {
        spiIO = 0;
    }
    if(0 == spiIO)
    {
        return io_mii_i2cMemWrite(memaddr,data);
    }
#endif

    osal_spl_spin_lock(&mii_spinlock);

    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG1,value);


    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG0,value);

    /*data msb to REG3  lsb to REG2*/
    tmp_data = data & 0xFFFF0000;
    tmp_data = tmp_data>>16;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG3,value);


    tmp_data = data & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG2,value);


    /*set REG6 to 0x804b trigger write procedure*/
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG6,0x804B);

    osal_spl_spin_unlock(&mii_spinlock);

    return RT_ERR_OK;
#endif
}

uint32 io_mii_memory_read(uint32 memaddr)
{
#if 0
   unsigned short value;
   IO_MII_PHYREG_READ(8,(unsigned char) memaddr,&value);

   printk("\nio_mii_memory_read:0x%x value:0x%x\n",memaddr,value);
   return value;
#else
    unsigned short value;
    uint32 tmp_data;

#if defined(LINUX_KERNEL_I2C_IO)
    uint32 spiIO=0;


    if((memaddr >= 0x1BF00000) && (memaddr <= 0x1BFFFFFF))
    {
        spiIO = 1;
    }
    else if((memaddr & 0xFF000000) == 0x18000000)
    {
        spiIO = 1;
    }
    else
    {
        spiIO = 0;
    }

    if(0 == spiIO)
    {
        return io_mii_i2cMemRead(memaddr);
    }
#endif

    osal_spl_spin_lock(&mii_spinlock);

    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG1,value);

    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG0,value);

    /*set REG6 to 0x800b trigger read procedure*/
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG6,0x800B);


    /*read data from data msb from REG5  lsb from REG4*/
    tmp_data = 0;
    IO_MII_PHYREG_READ(MII_ADDR,MII_REG5,&value);
    tmp_data = value;
    tmp_data = tmp_data <<16;
    IO_MII_PHYREG_READ(MII_ADDR,MII_REG4,&value);
    tmp_data = tmp_data | value;

    osal_spl_spin_unlock(&mii_spinlock);

    return tmp_data;


#endif
}

#endif

