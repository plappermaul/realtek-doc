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
 * Purpose : 9310/9300 SPI master to 9300 SPI slave
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spi read and write from 9310/9300 SPI master to 9300 SPI slave
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
#include <private/drv/swcore/swcore_rtl9310.h>
#include <private/drv/spi/spi_dev3.h>
#include <hwp/hw_profile.h>

#define REG_FIELD_SET(_pData, _val, _fOffset, _fMask)       (*_pData) = ((*_pData) & ~_fMask) | ((_val << _fOffset) & _fMask)
#define endian32_xchg(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))

static const spi_para_t  rtl9300_spi_config[] = {
    {SPI_CLK_1M,        400, 4, 0x7},
    {SPI_CLK_1M,        150, 4, 0x7},
    {SPI_CLK_1_25M,     400, 3, 0x6},
    {SPI_CLK_1_25M,     150, 3, 0x6},
    {SPI_CLK_1_875M,    150, 1, 0x5},
    {SPI_CLK_2M,        400, 1, 0x5},
    {SPI_CLK_4_69M,     150, 0, 0x4},
    {SPI_CLK_9_325M,    150, 0, 0x3},
    {SPI_CLK_12_5M,     400, 0, 0x4},
    {SPI_CLK_18_75M,    150, 0, 0x2},
    {SPI_CLK_25M,       400, 0, 0x3},
    {SPI_CLK_25M,       150, 0, 0x1},
    {SPI_CLK_37_5M,     150, 0, 0x0},
    {SPI_CLK_40M,       400, 0, 0x2},
    {SPI_CLK_50M,       400, 0, 0x1},
    {SPI_CLK_66_7M,     400, 0, 0x0},
};

static const spi_para_t  rtl9310_spi_config[] = {
    {SPI_CLK_1M,        250, 0, 0x7},
};

static uint8           spi_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t    spiMaster_sem[RTK_MAX_NUM_OF_UNIT];
static spi_para_t      spiCurr = {SPI_CLK_1M, 400, 4, 0x7};
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
_rtl9300_spi_clk_set(uint32 unit,     uint32 spi_clk)
{
    uint32 i;
    uint32 val;
    uint32 system_clk;

    /*Get system clock*/
    ioal_mem32_field_read(unit,RTL9300_BONDING_REDEFINE_REG_ADDR,
        RTL9300_BONDING_REDEFINE_REG_CHIP_MODE_3_0_MASK, RTL9300_BONDING_REDEFINE_REG_CHIP_MODE_3_0_MASK, &val);

    switch (val)
    {
        case 0xC:
        case 0xD:
            system_clk = 150; /*0xc:24G+4G; 0xd:24FE+4G*/
            break;
        default:
            system_clk = 400;
            break;
    }

    SPI_MASTER_SEM_LOCK;

    for (i = 0; i < (sizeof(rtl9300_spi_config)/sizeof(spi_para_t)); i++)
    {
        if((spi_clk == rtl9300_spi_config[i].spi_clk) && (system_clk == rtl9300_spi_config[i].system_clk))
        {
            spiCurr = rtl9300_spi_config[i];

            /* set SPI CLK DIV (0~7) */
            ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_SPI_CLK_DIV_OFFSET, RTL9300_SPI_CTRL1_SPI_CLK_DIV_MASK, spiCurr.clk_regVal);

            SPI_MASTER_SEM_UNLOCK;
            return RT_ERR_OK;
        }
    }

    SPI_MASTER_SEM_UNLOCK;
    return RT_ERR_FAILED;
}
/*
 For RTL931x serieal,
 the DIV value should be set between 8 ~ 250.
 SPI_CTRL0.SPI_CLK_SEL = 0x2 (DIV = 8)
 SPI_CTRL0.SPI_CLK_SEL = 0x7 (DIV = 250)
*/
int32
_rtl9310_spi_clk_set(uint32 unit,     uint32 spi_clk)
{
    uint32 i;

    SPI_MASTER_SEM_LOCK;

    for (i = 0; i < (sizeof(rtl9310_spi_config)/sizeof(spi_para_t)); i++)
    {
        if((spi_clk == rtl9310_spi_config[i].spi_clk))
        {
            spiCurr = rtl9310_spi_config[i];

            /* set SPI CLK DIV (0~7) */
            ioal_mem32_field_write(unit, RTL9310_SPI_CTRL0_ADDR,
            RTL9310_SPI_CTRL0_SPI_CLK_SEL_OFFSET, RTL9310_SPI_CTRL0_SPI_CLK_SEL_MASK, spiCurr.clk_regVal);

            SPI_MASTER_SEM_UNLOCK;
            return RT_ERR_OK;
        }
    }

    SPI_MASTER_SEM_UNLOCK;
    return RT_ERR_FAILED;
}


int32
_spi_dev3Ready_wait(uint32 unit)
{
    uint32 value=SPI_DEV3_READY_NOT_READ_YET, retry=0;

    do{
        retry++;
        if(HWP_9300_FAMILY_ID(unit))
        {
            ioal_mem32_field_read(unit,RTL9300_SPI_CTRL1_ADDR,
                RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK, &value);
        }
        else if(HWP_9310_FAMILY_ID(unit))
        {
            ioal_mem32_field_read(unit,RTL9310_SPI_CTRL2_ADDR,
                RTL9310_SPI_CTRL2_SPI_TRIG_OFFSET, RTL9310_SPI_CTRL2_SPI_TRIG_MASK, &value);
        }
    }while(value == 1);

    /*SS-370*/
    if((SPI_EXTRA_MDELAY)!=0)
        osal_time_udelay(SPI_EXTRA_MDELAY);

    return RT_ERR_OK;
}

void _rtl9300_spi_master_init(uint32 unit)
{
    /* data,address width  */
    if((HWP_CHIP_REV(unit) < 3) && IF_CHIP_TYPE_1(unit))
    {
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET,RTL9300_INT_SPI_CTRL1_DATA_WIDTH_MASK, 0x3);
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET,RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_MASK, 0x2);

    }else{
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_DATA_WIDTH_OFFSET,RTL9300_SPI_CTRL1_DATA_WIDTH_MASK, 0x3);
        ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
            RTL9300_SPI_CTRL1_ADDR_WIDTH_OFFSET,RTL9300_SPI_CTRL1_ADDR_WIDTH_MASK, 0x2);
    }

    /* set default SPI CLK*/
    _rtl9300_spi_clk_set(unit, SPI_CLK_1M);

    /* shared pin selected as SPI, instead of GPIO */
    ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_GPIO_SPI_SEL_OFFSET,RTL9300_SPI_CTRL1_GPIO_SPI_SEL_MASK, 0x1);

    /* set GPIO 3~5 as SPI (SCK, MISO, MOSI) */
    ioal_mem32_field_write(unit,RTL9300_SPI_CTRL1_ADDR,
        RTL9300_SPI_CTRL1_GPIO_SPI_SEL_OFFSET,RTL9300_SPI_CTRL1_GPIO_SPI_SEL_MASK, 0x1);
}
void _rtl9310_spi_master_init(uint32 unit)
{
    /* data,address width  */
     ioal_mem32_field_write(unit,RTL9310_SPI_CTRL0_ADDR,
         RTL9310_SPI_CTRL0_DATA_WIDTH_OFFSET,RTL9310_SPI_CTRL0_DATA_WIDTH_MASK, 0x3);

     ioal_mem32_field_write(unit,RTL9310_SPI_CTRL0_ADDR,
         RTL9310_SPI_CTRL0_ADDR_WIDTH_OFFSET,RTL9310_SPI_CTRL0_ADDR_WIDTH_MASK, 0x2);

     /* set default SPI CLK*/
     _rtl9310_spi_clk_set(unit, SPI_CLK_1M);

     ioal_mem32_field_write(unit,RTL9310_SPI_CTRL0_ADDR,
         RTL9310_SPI_CTRL0_GPIO12_CSB1_SEL_OFFSET, RTL9310_SPI_CTRL0_GPIO12_CSB1_SEL_MASK, 0x1);

     /* set GPIO 8~10 as SPI (SCK, MISO, MOSI) */
     ioal_mem32_field_write(unit,RTL9310_SPI_CTRL0_ADDR,
         RTL9310_SPI_CTRL0_GPIO_SPI_SEL_OFFSET,RTL9310_SPI_CTRL0_GPIO_SPI_SEL_MASK, 0x1);

}

int32 spi_dev3Reg_read(uint32 unit, uint32 mAddrs, uint32 *buff)
{
    uint32 value, address_tmp, cs;
    uint32 org_val;
    uint32 master_unit = HWP_MY_UNIT_ID();


    SPI_MASTER_SEM_LOCK;
    if (HWP_9310_FAMILY_ID(master_unit))
    {
        ioal_mem32_read(master_unit, RTL9310_SPI_CTRL2_ADDR, &org_val);

        /* chip select */
        if ((cs = HWP_SWCORE_SPI_CHIP_SEL(unit)) != HWP_NOT_USED)
        {
            REG_FIELD_SET(&org_val, cs, RTL9310_SPI_CTRL2_CSB_OUT_SEL_OFFSET, RTL9310_SPI_CTRL2_CSB_OUT_SEL_MASK);
        }

        /* command */
        REG_FIELD_SET(&org_val, 0x3, RTL9310_SPI_CTRL2_SPI_CMD_OFFSET, RTL9310_SPI_CTRL2_SPI_CMD_MASK);

        /* address */
        address_tmp = mAddrs<<8;
        ioal_mem32_write(master_unit, RTL9310_SPI_ADDR_ADDR, address_tmp);

        /* command type (read) */
        REG_FIELD_SET(&org_val, 0x0, RTL9310_SPI_CTRL2_SPI_CMD_TYPE_OFFSET, RTL9310_SPI_CTRL2_SPI_CMD_TYPE_MASK);

        /* trig */
        REG_FIELD_SET(&org_val, 0x1, RTL9310_SPI_CTRL2_SPI_TRIG_OFFSET, RTL9310_SPI_CTRL2_SPI_TRIG_MASK);

        ioal_mem32_write(master_unit, RTL9310_SPI_CTRL2_ADDR, org_val);

        /* check if ready */
        _spi_dev3Ready_wait(master_unit);

        /* get data */
        ioal_mem32_read(master_unit, RTL9310_SPI_DATA_ADDR(0), &value);
        *buff = value;
    }
    else if (HWP_9300_FAMILY_ID(master_unit))
    {
        ioal_mem32_read(master_unit, RTL9300_SPI_CTRL1_ADDR, &org_val);

        /* command */
        REG_FIELD_SET(&org_val, 0x3, RTL9300_SPI_CTRL1_SPI_CMD_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_MASK);

        /* address */
        address_tmp = mAddrs<<8;
        ioal_mem32_write(master_unit, RTL9300_SPI_ADDR_ADDR, address_tmp);


        if((HWP_CHIP_REV(master_unit) < 3) && HWP_9300_FAMILY_ID(master_unit) && IF_CHIP_TYPE_1(master_unit))
        {
            /* command type (read) */
            REG_FIELD_SET(&org_val, 0x1, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK);
        }else{
            /* command type (read) */
            REG_FIELD_SET(&org_val, 0x0, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK);
        }

        /* trig */
        REG_FIELD_SET(&org_val, 0x1, RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK);

        ioal_mem32_write(master_unit, RTL9300_SPI_CTRL1_ADDR, org_val);

        /* check if ready */
        _spi_dev3Ready_wait(master_unit);

        /* get data */
        ioal_mem32_read(master_unit, RTL9300_SPI_DATA_ADDR(0), &value);
        *buff = endian32_xchg(value);
    }
    SPI_MASTER_SEM_UNLOCK;

    return RT_ERR_OK;

}

int32 spi_dev3Reg_write(uint32 unit, uint32 mAddrs, uint32 *buff)
{
    uint32 master_unit = HWP_MY_UNIT_ID(), cs;
    uint32 org_val;


    SPI_MASTER_SEM_LOCK;
    if (HWP_9310_FAMILY_ID(master_unit))
    {
        ioal_mem32_read(master_unit, RTL9310_SPI_CTRL2_ADDR, &org_val);

        /* chip select */
        if ((cs = HWP_SWCORE_SPI_CHIP_SEL(unit)) != HWP_NOT_USED)
        {
            REG_FIELD_SET(&org_val, cs, RTL9310_SPI_CTRL2_CSB_OUT_SEL_OFFSET, RTL9310_SPI_CTRL2_CSB_OUT_SEL_MASK);
        }

        /* command */
        REG_FIELD_SET(&org_val, 0x2, RTL9310_SPI_CTRL2_SPI_CMD_OFFSET, RTL9310_SPI_CTRL2_SPI_CMD_MASK);

        /* address */
        ioal_mem32_write(master_unit, RTL9310_SPI_ADDR_ADDR, (mAddrs<<8));

        /* data */
        ioal_mem32_write(master_unit, RTL9310_SPI_DATA_ADDR(0), *buff);
         /* command type (write) */
        REG_FIELD_SET(&org_val, 0x1, RTL9310_SPI_CTRL2_SPI_CMD_TYPE_OFFSET, RTL9310_SPI_CTRL2_SPI_CMD_TYPE_MASK);

         /* trig */
         REG_FIELD_SET(&org_val, 0x1, RTL9310_SPI_CTRL2_SPI_TRIG_OFFSET, RTL9310_SPI_CTRL2_SPI_TRIG_MASK);
         ioal_mem32_write(master_unit, RTL9310_SPI_CTRL2_ADDR, org_val);


         /* check if ready */
         _spi_dev3Ready_wait(master_unit);

    }
    else if (HWP_9300_FAMILY_ID(master_unit))
    {
        ioal_mem32_read(master_unit, RTL9300_SPI_CTRL1_ADDR, &org_val);

        /* command */
        REG_FIELD_SET(&org_val, 0x2, RTL9300_SPI_CTRL1_SPI_CMD_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_MASK);

        /* address */
        ioal_mem32_write(master_unit, RTL9300_SPI_ADDR_ADDR, (mAddrs<<8));

        /* data */
        ioal_mem32_write(master_unit, RTL9300_SPI_DATA_ADDR(0), endian32_xchg(*buff) );

        if((HWP_CHIP_REV(master_unit) < 3) && HWP_9300_FAMILY_ID(master_unit) && IF_CHIP_TYPE_1(master_unit))
        {
            /* command type (write) */
            REG_FIELD_SET(&org_val, 0x0, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK);
        }else{
            /* command type (write) */
            REG_FIELD_SET(&org_val, 0x1, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_OFFSET, RTL9300_SPI_CTRL1_SPI_CMD_TYPE_MASK);
        }


        /* trig */
        REG_FIELD_SET(&org_val, 0x1, RTL9300_SPI_CTRL1_SPI_TRIG_OFFSET, RTL9300_SPI_CTRL1_SPI_TRIG_MASK);
        ioal_mem32_write(master_unit, RTL9300_SPI_CTRL1_ADDR, org_val);


        /* check if ready */
        _spi_dev3Ready_wait(master_unit);
    }

    SPI_MASTER_SEM_UNLOCK;

    return RT_ERR_OK;
}

int32 spi_dev3_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(spi_init[unit]);

    if( unit == HWP_MY_UNIT_ID())
    {
        RT_INIT_MSG("    SPI Master init\n");

        /* create semaphore */
        spiMaster_sem[unit] = osal_sem_mutex_create();
        if (0 == spiMaster_sem[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_BSP), "spiMaster semaphore create failed");
            return RT_ERR_FAILED;
        }

        if(HWP_9300_FAMILY_ID(unit))
        {
            _rtl9300_spi_master_init(unit);
        }
        else if(HWP_9310_FAMILY_ID(unit))
        {
            _rtl9310_spi_master_init(unit);
        }
    }
    else
    {
        uint32 master_unit = HWP_MY_UNIT_ID();

        RT_INIT_MSG("    SPI Slave init\n");

        /* Set chip select */
        if( 0 == HWP_SWCORE_SPI_CHIP_SEL(unit))
        {
            if(HWP_9300_FAMILY_ID(master_unit))
            {
                /* set GPIO 7 as SPI CS#0 */
                ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
                    RTL9300_SPI_CTRL1_GPIO7_CSB0_SEL_OFFSET, RTL9300_SPI_CTRL1_GPIO7_CSB0_SEL_MASK, 0x1);

                /* Using CS#0 */
                ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
                    RTL9300_SPI_CTRL1_CSB_OUT_SEL_OFFSET, RTL9300_SPI_CTRL1_CSB_OUT_SEL_MASK, 0x0);
            }
            else if(HWP_9310_FAMILY_ID(master_unit))
            {
                /* set GPIO 11 as SPI CS#0 */
                ioal_mem32_field_write(master_unit,RTL9310_SPI_CTRL0_ADDR,
                    RTL9310_SPI_CTRL0_GPIO11_CSB0_SEL_OFFSET, RTL9310_SPI_CTRL0_GPIO11_CSB0_SEL_MASK, 0x1);
                /* Using CS#0 */
                ioal_mem32_field_write(master_unit, RTL9310_SPI_CTRL2_ADDR,
                    RTL9310_SPI_CTRL2_CSB_OUT_SEL_OFFSET, RTL9310_SPI_CTRL2_CSB_OUT_SEL_MASK, 0x0);
            }
        }
        else if( 1 == HWP_SWCORE_SPI_CHIP_SEL(unit))
        {
            if(HWP_9300_FAMILY_ID(master_unit))
            {
                /* set GPIO 6 as SPI CS#1 */
                ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
                    RTL9300_SPI_CTRL1_GPIO6_CSB1_SEL_OFFSET, RTL9300_SPI_CTRL1_GPIO6_CSB1_SEL_MASK, 0x1);

                /* Using CS#1 */
                ioal_mem32_field_write(master_unit, RTL9300_SPI_CTRL1_ADDR,
                    RTL9300_SPI_CTRL1_CSB_OUT_SEL_OFFSET, RTL9300_SPI_CTRL1_CSB_OUT_SEL_MASK, 0x1);
            }
            else if(HWP_9310_FAMILY_ID(master_unit))
            {
                /* set GPIO 12 as SPI CS#1 */
                ioal_mem32_field_write(master_unit,RTL9310_SPI_CTRL0_ADDR,
                    RTL9310_SPI_CTRL0_GPIO12_CSB1_SEL_OFFSET, RTL9310_SPI_CTRL0_GPIO12_CSB1_SEL_MASK, 0x1);
                /* Using CS#1 */
                ioal_mem32_field_write(master_unit, RTL9310_SPI_CTRL2_ADDR,
                    RTL9310_SPI_CTRL2_CSB_OUT_SEL_OFFSET, RTL9310_SPI_CTRL2_CSB_OUT_SEL_MASK, 0x1);
            }

        }else{

            RT_INIT_MSG("  SPI Slave chip select definition in hwp is wrong! (unit %u)\n",unit);
            return RT_ERR_FAILED;
        }
        if(HWP_9300_FAMILY_ID(master_unit))
        {
            /*Change the data output format to prevent timing issue.*/
            if (SPI_TARGET_CLK >= SPI_CLK_12_5M)
            {
                ioal_mem32_field_write(unit,RTL9300_MAC_IF_CTRL_ADDR,
                    RTL9300_MAC_IF_CTRL_SPI_SO_REF_OFFSET,RTL9300_MAC_IF_CTRL_SPI_SO_REF_MASK, 0x1);
            }
            _rtl9300_spi_clk_set(master_unit, SPI_TARGET_CLK);
        }

    }
    spi_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}


