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
 * Purpose : 9300 SPI master to max slave
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spi read and write from 9300 SPI master to 9300 SPI slave
 *
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <soc/type.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/time.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <drv/spi/spi.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <hwp/hw_profile.h>


/* MAX3421E reg definition */
/* define read/write direction */
#define SPI_DEV2_DIR_WRITE                  (0x2)
#define SPI_DEV2_DIR_READ                   (0x0)

#define MAX3421E_DATA_ReadReg(regAddr)      ((regAddr<<3 |SPI_DEV2_DIR_READ) & 0xFF)
#define MAX3421E_DATA_WriteReg(regAddr)     ((regAddr<<3 |SPI_DEV2_DIR_WRITE) & 0xFF)
#define MAX3421E_REG_PINCTL                 17
#define MAX3421E_REG_REV                    18


#define endian32_xchg(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))

static uint8        spi_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t spiMaster_sem[RTK_MAX_NUM_OF_UNIT];
#define SPI_MASTER_SEM_LOCK    \
do {\
    if (osal_sem_mutex_take(spiMaster_sem[HWP_MY_UNIT_ID()], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_BSP), "spiMaster semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define SPI_MASTER_SEM_UNLOCK   \
do {\
    if (osal_sem_mutex_give(spiMaster_sem[HWP_MY_UNIT_ID()]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_BSP), "spiMaster semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)



#define SPI_DEV3_READY_NOT_READ_YET 999
int32
_spi_dev4Ready_wait(uint32 unit)
{
    uint32 value=SPI_DEV3_READY_NOT_READ_YET, retry=0;


    do{
        retry++;
        ioal_mem32_field_read(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK, &value);

        osal_time_udelay(1);

    }while(value==1);

    return RT_ERR_OK;
}


int32 spi_dev4Reg_read(uint32 unit, uint32 mAddrs, uint8 *buff)
{
    uint32 value, tmp=0;
    uint32 master_unit = HWP_MY_UNIT_ID();


    SPI_MASTER_SEM_LOCK;

    /* command (dummy write-reg) */
    tmp = 0;
    tmp =  MAX3421E_DATA_WriteReg(MAX3421E_REG_PINCTL);
    ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_SPI_CMD_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_MASK, tmp);

osal_printf("\n%s(%d):cmd=0x%x\n",__FUNCTION__,__LINE__,tmp);

    /* address  */
    tmp = 0;
    tmp |= 0xFF << 8; /* (dummay write-data) */
    tmp |= MAX3421E_DATA_ReadReg(mAddrs); /* MAX read */
    ioal_mem32_write(master_unit, RTL9300_SPI_ADDR_ADDR, tmp);

osal_printf("%s(%d):addr=0x%x\n",__FUNCTION__,__LINE__,tmp);

    if((HWP_CHIP_REV(unit) < 3) && ((HWP_CHIP_ID(unit) == RTL9301_CHIP_ID_24G)||(HWP_CHIP_ID(unit) == RTL9303_CHIP_ID_8XG)))
    {
        /* command type (read) */
        ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK, 0x1);

    }else{
        /* command type (read) */
        ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK, 0x0);
    }

    /* trig */
    ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK, 0x1);

    /* check if ready */
    _spi_dev4Ready_wait(master_unit);

    /* get data */
    for(tmp=0;tmp<128;tmp++)
    {
        ioal_mem32_read(master_unit, RTL9300_SPI_DATA_ADDR(tmp), &value);
    osal_printf("%s(%d):(tmp=%d),read value=0x%x\n",__FUNCTION__,__LINE__,tmp,value);
    }

    *buff = (uint8)(value & 0x000000FF);


    SPI_MASTER_SEM_UNLOCK;

    return RT_ERR_OK;

}

int32 spi_dev4Reg_write(uint32 unit, uint32 mAddrs, uint8 *buff)
{
    uint32 master_unit = HWP_MY_UNIT_ID();
    uint32 tmp=0;


    SPI_MASTER_SEM_LOCK;

    /* command (dummy write-reg) */
    tmp = 0;
    tmp =  MAX3421E_DATA_WriteReg(MAX3421E_REG_PINCTL);
    ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
                RTL9300_SPI_CTRL1_SPI_CMD_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_MASK,tmp);

osal_printf("\n%s(%d):cmd=0x%x\n",__FUNCTION__,__LINE__,tmp);

    /* address (dummay write-data) */
    tmp = 0;
    tmp |= 0xFF << 8; /* (dummay write-data) */
    tmp |= MAX3421E_DATA_WriteReg(mAddrs); /* MAX write */
    ioal_mem32_write(master_unit, RTL9300_SPI_ADDR_ADDR, tmp);

osal_printf("%s(%d):addr=0x%x\n",__FUNCTION__,__LINE__,tmp);

    /* data (as MAX write reg+data) */
    ioal_mem32_write(master_unit, RTL9300_SPI_DATA_ADDR(0), (uint32)*buff );


osal_printf("%s(%d):write_whole_cmd=0x%x\n",__FUNCTION__,__LINE__,(uint32)*buff);


    if((HWP_CHIP_REV(unit) < 3) && ((HWP_CHIP_ID(unit) == RTL9301_CHIP_ID_24G)||(HWP_CHIP_ID(unit) == RTL9303_CHIP_ID_8XG)))
    {
        /* command type (write) */
        ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK, 0x0);

    }else{

        /* command type (write) */
        ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK, 0x1);
    }

    /* trig */
    ioal_mem32_field_write(master_unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK, 0x1);

    /* check if ready */
    _spi_dev4Ready_wait(master_unit);


    SPI_MASTER_SEM_UNLOCK;

    return RT_ERR_OK;
}


int32 spi_dev4_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(spi_init[unit]);
    RT_INIT_MSG("  SPI (9300 Master,max3421e) init (unit %u)\n",unit);

    /* create semaphore */
    spiMaster_sem[unit] = osal_sem_mutex_create();
    if (0 == spiMaster_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_BSP), "spiMaster semaphore create failed");
        return RT_ERR_FAILED;
    }
    if((HWP_CHIP_REV(unit) < 3) && ((HWP_CHIP_ID(unit) == RTL9301_CHIP_ID_24G)||(HWP_CHIP_ID(unit) == RTL9303_CHIP_ID_8XG)))
    {
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET,RTL9300_INT_SPI_CTRL1_DATA_WIDTH_MASK, 0x0);
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET,RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_MASK, 0x1);

    }else{
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_DATA_WIDTH_OFFSET,RTL9300_SPI_CTRL1_DATA_WIDTH_MASK, 0x0);
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_ADDR_WIDTH_OFFSET,RTL9300_SPI_CTRL1_ADDR_WIDTH_MASK, 0x1);
    }

    /* set SPI CLK DIV (0~7) */
    ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_SPI_CLK_DIV_OFFSET,RTL9300_SPI_CTRL1_SPI_CLK_DIV_MASK, 0x7);

    /* shared pin selected as SPI, instead of GPIO */
    ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_GPIO_SPI_SEL_OFFSET,RTL9300_SPI_CTRL1_GPIO_SPI_SEL_MASK, 0x1);

    /* set GPIO 3~5 as SPI (SCK, MISO, MOSI) */
    ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_GPIO_SPI_SEL_OFFSET,RTL9300_SPI_CTRL1_GPIO_SPI_SEL_MASK, 0x1);

    /* Set chip select */
    /* set GPIO 7 as SPI CS#0 */
    ioal_mem32_field_write(unit, RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_GPIO7_CSB0_SEL_OFFSET, RTL9300_SPI_CTRL1_GPIO7_CSB0_SEL_MASK, 0x1);
    /* Using CS#0 */
    ioal_mem32_field_write(unit, RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_CSB_OUT_SEL_OFFSET, RTL9300_SPI_CTRL1_CSB_OUT_SEL_MASK, 0x0);

    spi_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}



