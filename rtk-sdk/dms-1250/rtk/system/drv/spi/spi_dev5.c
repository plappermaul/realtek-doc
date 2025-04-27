/*
 * Copyright (C) 2009-2020 Realtek Semiconductor Corp.
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
 * Purpose : 9310 GPIO as SPI master to two 9300 SPI slaves
 *           and dispatch to dev3 when the unit is using actual SPI master.
 *           The 9300 SPI slave command:
 *              cmd byte0: 0x2 - write; 0x3 - read
 *              cmd byte1: address, bit[23:16]  -- set to register address bit[15:8]
 *              cmd byte2: address, bit[15:8]   -- set to register address bit[7:0]
 *              cmd byte3: address, bit[7:0]    -- dummy bits shall set to 0
 *              cmd byte4: in/out data byte0
 *              cmd byte5: in/out data byte1
 *              cmd byte6: in/out data byte2
 *              cmd byte7: in/out data byte3
 * Feature : The file have include the following module and sub-modules
 *           1) spi read and write from 9310 SPI master to 9300 SPI slave
 *
 */
#include <common/rt_autoconf.h>
#include <osal/sem.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <private/drv/swcore/swcore_rtl9310.h>
#include <hal/chipdef/chip.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#include <private/drv/spi/spi_dev5.h>
#include <private/drv/spi/spi_dev3.h>
#include <hwp/hw_profile.h>


/*
 * ================================(Customization)================================
 */

/* define SPI_MASTER_INTF to choose which interface as SPI interface */
#define SPI_MASTER_INTF_839xGPIO              2
#define SPI_MASTER_INTF_838xGPIO              3
#define SPI_MASTER_INTF_9310GPIO              4
#define SPI_MASTER_INTF                       SPI_MASTER_INTF_9310GPIO

//#define USING_728_GPIO_SPI                    1

/* define this will using another API to access GPIO, to get lower API overhead */
//#define SPI_USING_DIRECT_GPIO_REG_ACCESS       1

/* semaphore, please define per your requirement */
//to sync tne usage of GPIO DATA register
#define SPI_GpioDataReg_LOCK()
#define SPI_GpioDataReg_UNLOCK()

static osal_mutex_t    spiMaster_sem[RTK_MAX_NUM_OF_UNIT];
//to sync the usage of SPI master interface
#define SPI_MasterIntf_LOCK()       \
do {\
    if (osal_sem_mutex_take(spiMaster_sem[HWP_MY_UNIT_ID()], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_BSP), "spiMaster semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define SPI_MasterIntf_UNLOCK()     \
do {\
    if (osal_sem_mutex_give(spiMaster_sem[HWP_MY_UNIT_ID()]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_BSP), "spiMaster semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


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

typedef struct spiGpio_pin_s
{
    uint32 SCLK;
    uint32 MOSI;
    uint32 MISO;
    uint32 SS;
    uint32 INT;
} spiGpio_pin_t;


/* GPIO pin number definition to simulate SPI */
#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO
  #ifdef USING_728_GPIO_SPI
    spiGpio_pin_t    spiGpioPin[1] =
    {
        [0] = {
                .SCLK  =16,
                .MOSI  =17,
                .MISO  =18,
                .SS    =15,
                .INT   =19,
              },
    };
  #else
    spiGpio_pin_t    spiGpioPin[1] =
    {
        [0] = {
                .SCLK  =13,
                .MOSI  =2,
                .MISO  =1,
                .SS    =14,
                .INT   =16,
              },
    };
  #endif
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    spiGpio_pin_t    spiGpioPin[1] =
    {
        [0] = {
                .SCLK  =1,
                .MOSI  =14,
                .MISO  =2,
                .SS    =21,
                .INT   =22,
              },
    };
#elif SPI_MASTER_INTF ==  SPI_MASTER_INTF_9310GPIO
    spiGpio_pin_t    spiGpioPin[2] =
    {
        [0] = {
                .SCLK = 1,
                .MOSI = 14,
                .MISO = 2,
                .SS   = 21,
                .INT  = 22,
              },
        [1] = {
                .SCLK = 23,
                .MOSI = 25,
                .MISO = 24,
                .SS   = 26,
                .INT  = 31,
              },
    };
#endif

#define SPI_MAX_CS_NUM      (sizeof(spiGpioPin)/sizeof(spiGpio_pin_t))

int32 _spi_dev5_init(uint32 unit);
int32 _spi_dev5Reg_write(uint32 unit, uint32 regAddr, uint32 *buff);
int32 _spi_dev5Reg_read(uint32 unit, uint32 regAddr, uint32 *buff);

typedef struct spi_devDrv_mapper_s {
    int32   (*init)(uint32);
    int32   (*read)(uint32, uint32, uint32 *);
    int32   (*write)(uint32, uint32, uint32 *);
} spi_devDrv_mapper_t;

spi_devDrv_mapper_t     spi_devDrv_mapper[RTK_MAX_NUM_OF_UNIT_LOCAL] = {
    [0] = {NULL, NULL, NULL}, /* master */
    [1] = {spi_dev3_init, spi_dev3Reg_read, spi_dev3Reg_write},     /* 9310 spi master */
    [2] = {spi_dev3_init, spi_dev3Reg_read, spi_dev3Reg_write},     /* 9310 spi master */
    [3] = {_spi_dev5_init, _spi_dev5Reg_read, _spi_dev5Reg_write},  /* gpio spi master */
    [4] = {_spi_dev5_init, _spi_dev5Reg_read, _spi_dev5Reg_write},  /* gpio spi master */
};

/*
 * ============================(End of customization)============================
 */

static uint8 spi_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static uint8 gpio_dev_init[RTK_MAX_NUM_OF_UNIT] = {FALSE};
static uint32  spi_chipSel = 0;

#define SS_ACTIVE       0
#define SS_INACTIVE     1


/* define GPIO register address */
#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO
    unsigned volatile int *gpio_data_register=(uint32 *)0x350C;
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    unsigned volatile int *gpio_data_register=(uint32 *)0x350C;
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_9310GPIO
    unsigned volatile int *gpio_data_register=(uint32 *)0x330C;
#else
    unsigned volatile int *gpio_data_register=(uint32 *)0x330C;
#endif

/* define GPIO pin bitmap for operation */
typedef struct gpio_pin_info_s{
    uint8   pin_num;        /* which GPIO pin */
    uint32  pin_mask;       /* turn on the register bit of this GPIO pin */
    uint32  pin_mask_not;   /* ~pin_mask */
}gpio_pin_info_t;


gpio_pin_info_t gpioInfo[]={
    { 0,0x01000000,0xFEFFFFFF}, /* port A */
    { 1,0x02000000,0xFDFFFFFF},
    { 2,0x04000000,0xFBFFFFFF},
    { 3,0x08000000,0xF7FFFFFF},
    { 4,0x10000000,0xEFFFFFFF},
    { 5,0x20000000,0xDFFFFFFF},
    { 6,0x40000000,0xBFFFFFFF},
    { 7,0x80000000,0x7FFFFFFF},
    { 8,0x00010000,0xFFFEFFFF}, /* port B */
    { 9,0x00020000,0xFFFDFFFF},
    {10,0x00040000,0xFFFBFFFF},
    {11,0x00080000,0xFFF7FFFF},
    {12,0x00100000,0xFFEFFFFF},
    {13,0x00200000,0xFFDFFFFF},
    {14,0x00400000,0xFFBFFFFF},
    {15,0x00800000,0xFF7FFFFF},
    {16,0x00000100,0xFFFFFEFF}, /* port C */
    {17,0x00000200,0xFFFFFDFF},
    {18,0x00000400,0xFFFFFBFF},
    {19,0x00000800,0xFFFFF7FF},
    {20,0x00001000,0xFFFFEFFF},
    {21,0x00002000,0xFFFFDFFF},
    {22,0x00004000,0xFFFFBFFF},
    {23,0x00008000,0xFFFF7FFF},
#if SPI_MASTER_INTF == SPI_MASTER_INTF_9310GPIO
    {24,0x00000001,0xFFFFFFFE}, /* port D */
    {25,0x00000002,0xFFFFFFFD},
    {26,0x00000004,0xFFFFFFFB},
    {27,0x00000008,0xFFFFFFF7},
    {28,0x00000010,0xFFFFFFEF},
    {29,0x00000020,0xFFFFFFDF},
    {30,0x00000040,0xFFFFFFBF},
    {31,0x00000080,0xFFFFFF7F},
#endif /* end of SPI_MASTER_INTF == SPI_MASTER_INTF_9310GPIO */
};

/* define command type for read/write API */
typedef enum{
    SPI_DEV5_CMD_READ     = 0x01,
    SPI_DEV5_CMD_WRITE    = 0x02,
    SPI_DEV5_CMD_NONE     = 0x03,
}spi_cmdType_t;

typedef struct spi_cmd_s{
    uint32        flags;
    spi_cmdType_t cmd_t;
    uint32        address;
    uint8         *buf;     /*request buffer*/
    uint32        size;     /*request IO size*/
}spi_cmd_t;

/* define read/write direction */
#define SPI_DEV5_DIR_WRITE                  (0x2)
#define SPI_DEV5_DIR_READ                   (0x3)

/* for using RTK general GPIO API */
drv_generalCtrlGpio_devId_t                 sg_dev = GEN_GPIO_DEV_ID0_INTERNAL;
drv_generalCtrlGpio_devId_t                 sg_dev_ext = GEN_GPIO_DEV_ID1;

#define DRV_GPIO_SET(unit,dev,pin,data)     drv_generalCtrlGPIO_dataBit_set(unit,dev,pin,data);
#define DRV_GPIO_GET(unit,dev,pin,pData)    drv_generalCtrlGPIO_dataBit_get(unit,dev,pin,pData);

/*
 * GPIO pin operation
 */

uint32 spiGpio_SShi(uint32 data_reg_value)
{
    data_reg_value |= gpioInfo[spiGpioPin[spi_chipSel].SS].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SSlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[spiGpioPin[spi_chipSel].SS].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKhi(uint32 data_reg_value)
{
    data_reg_value |= gpioInfo[spiGpioPin[spi_chipSel].SCLK].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[spiGpioPin[spi_chipSel].SCLK].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo_MOSIhi(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[spiGpioPin[spi_chipSel].SCLK].pin_mask_not;
    data_reg_value |= gpioInfo[spiGpioPin[spi_chipSel].MOSI].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo_MOSIlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[spiGpioPin[spi_chipSel].SCLK].pin_mask_not;
    data_reg_value &= gpioInfo[spiGpioPin[spi_chipSel].MOSI].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_MISO_get(uint32 *data, uint32 data_reg_value)
{
    data_reg_value = *gpio_data_register;
    *data = data_reg_value & gpioInfo[spiGpioPin[spi_chipSel].MISO].pin_mask;
    return data_reg_value;
}


uint32 _spi_bitbang_bitSet(uint32 unit, uint8 data, uint32 data_reg_value)
{
#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
    if(data)
    data_reg_value = spiGpio_SCLKlo_MOSIhi(data_reg_value);
    else
    data_reg_value = spiGpio_SCLKlo_MOSIlo(data_reg_value);
    data_reg_value = spiGpio_SCLKhi(data_reg_value);
#else
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 0);
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].MOSI, data);
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 1);
#endif
    return data_reg_value;
}


uint32 _spi_bitbang_bitGet(uint32 unit, uint32 *data, uint32 data_reg_value)
{

#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
    data_reg_value = spiGpio_SCLKlo(data_reg_value);
    data_reg_value = spiGpio_SCLKhi(data_reg_value);
    data_reg_value = spiGpio_MISO_get(data,data_reg_value);
#else
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 0);
    DRV_GPIO_GET(unit, sg_dev, spiGpioPin[spi_chipSel].MISO, data);
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 1);
#endif
    return data_reg_value;
}


uint32 _spi_bitbang_byteSet(uint32 unit, uint8 *writeData, uint32 data_reg_value)
{
    int i;

    for(i=7;i>=0;i--)
        data_reg_value = _spi_bitbang_bitSet(unit, writeData[i], data_reg_value);

    return data_reg_value;
}

uint32 _spi_bitbang_byteGet(uint32 unit, uint32 *data, uint32 data_reg_value)
{
    uint8 i;

    for(i=0;i<8;i++)
        data_reg_value = _spi_bitbang_bitGet(unit, &data[i],data_reg_value);

    return data_reg_value;
}

int32 _spi_dev5cmd_9300Gpio(uint32 unit, spi_cmd_t *req)
{
    uint32 regAddr = req->address << 8; /* Address byte A23-A8 mapping to switch register LSB 16 bits (0xBB00-xxxx) */
    uint8 data=0;
    unsigned volatile int gpioDataReg_value;
    uint8 writeCmd[8],writeData[8];
    uint32 readData[8];
    uint8 spiCmdBytes[8];
    int i;
    int byte;

    /*
     * byte1 = 0x03(Read), 0x02(Write)
     * byte2 = A23-A16
     * byte3 = A15-A8
     * byte4 = A7-A0
     * byte5 = D31-D24
     * byte6 = D23-D16
     * byte7 = D15-D8
     * byte8 = D7-D0
     *
     * Where address byte A23-A8 mapping to switch register LSB 16 bits (0xBB00-xxxx), A7-A0 is dummy byte, set to 0.
     */

    /* byte1 = 0x03(Read), 0x02(Write) */
    if(req->cmd_t == SPI_DEV5_CMD_WRITE)
    {
        spiCmdBytes[0] = SPI_DEV5_DIR_WRITE;
    }
    else
    {
        spiCmdBytes[0] = SPI_DEV5_DIR_READ;
    }


    /* Regiater address
     *      byte2 = A23-A16
     *      byte3 = A15-A8
     *      byte4 = A7-A0
     */
    spiCmdBytes[1] = (regAddr >> 16) & 0xFF;
    spiCmdBytes[2] = (regAddr >> 8) & 0xFF;
    spiCmdBytes[3] = regAddr & 0xFF;

    /* Data
     *      byte5 = D31-D24
     *      byte6 = D23-D16
     *      byte7 = D15-D8
     *      byte8 = D7-D0
     */
    if(req->cmd_t == SPI_DEV5_CMD_WRITE)
    {
        spiCmdBytes[4] = req->buf[0];
        spiCmdBytes[5] = req->buf[1];
        spiCmdBytes[6] = req->buf[2];
        spiCmdBytes[7] = req->buf[3];
    }

    SPI_GpioDataReg_LOCK();

    /* store GPIO data register and pass to each API, to get more performance by skip reading IO */
    gpioDataReg_value = *gpio_data_register;

    /* active chip select */
#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
    gpioDataReg_value = spiGpio_SSlo(gpioDataReg_value);
#else
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SS, SS_ACTIVE);
#endif

    /* write SPI command byte */
    for (byte=0; byte<=3; byte++)
    {
        data = spiCmdBytes[byte];
        for(i=0;i<8;i++)
            writeCmd[i] = ((data>>i) & 0x1);

        gpioDataReg_value = _spi_bitbang_byteSet(unit,writeCmd,gpioDataReg_value);
    }


    /* read or write data */
    switch( req->cmd_t )
    {
        case SPI_DEV5_CMD_READ:
            for (byte=4; byte<=7; byte++)
            {
                gpioDataReg_value = _spi_bitbang_byteGet(unit,readData,gpioDataReg_value);
                data = 0;
                for(i=0;i<8;i++)
                {
                    if(readData[i])
                        data |= ( 0x1<<(7-i) );
                }
                spiCmdBytes[byte] = data;
            }
            break;
        case SPI_DEV5_CMD_WRITE:
            for (byte=4; byte<=7; byte++)
            {
                data = spiCmdBytes[byte];
                for(i=0;i<8;i++)
                    writeData[i] = ((data>>i) & 0x1);
                gpioDataReg_value = _spi_bitbang_byteSet(unit,writeData,gpioDataReg_value);
            }
            break;
        default:
            break;
    };
    /* inactive chip select */
#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
        spiGpio_SShi(gpioDataReg_value);
#else
        DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 0);
        DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SS, SS_INACTIVE);
#endif

    if(req->cmd_t == SPI_DEV5_CMD_WRITE)
    {
        for (i=0; i<4; i++)
        {
            DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 0);
            DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 1);
        }
        DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SCLK, 0);
    }

    SPI_GpioDataReg_UNLOCK();

    /* get read data */
    if(req->cmd_t==SPI_DEV5_CMD_READ)
    {
        req->buf[0] = spiCmdBytes[4];
        req->buf[1] = spiCmdBytes[5];
        req->buf[2] = spiCmdBytes[6];
        req->buf[3] = spiCmdBytes[7];
    }

    return RT_ERR_OK;
}


int32 spi_dev5SpiPin_init(uint32 unit, spi_init_info_t *init_info)
{
    int32   ret;
    drv_generalCtrlGpio_devConf_t devConfig;
    drv_generalCtrlGpio_pinConf_t gpioConfig;

    if (gpio_dev_init[unit] == FALSE)
    {
        /* init GPIO device */
        ret = drv_generalCtrlGPIO_dev_init(unit,sg_dev, &devConfig);
        if (ret != RT_ERR_OK)
        {
            RT_INIT_MSG("  SPI GPIO dev init fail (unit %u)\n", unit);
        }
        ret = drv_generalCtrlGPIO_devEnable_set(unit,sg_dev,ENABLED);
        if (ret != RT_ERR_OK)
        {
            RT_INIT_MSG("  SPI GPIO dev enable fail (unit %u)\n", unit);
        }
        gpio_dev_init[unit] = TRUE;
    }
#ifdef USING_728_GPIO_SPI
    {
        drv_generalCtrlGpio_devConf_t devConfig_ext;

        devConfig_ext.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
        devConfig_ext.ext_gpio.address = 31;
        devConfig_ext.default_value = 1;
        devConfig_ext.direction = GPIO_DIR_OUT;
        drv_generalCtrlGPIO_dev_init(unit,sg_dev_ext, &devConfig_ext);
        drv_generalCtrlGPIO_devEnable_set(unit,sg_dev_ext,ENABLED);
    }
#endif

    /* init GPIO pin */
    gpioConfig.direction = GPIO_DIR_IN;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev,init_info->gpioNum_miso, &gpioConfig);
    spiGpioPin[spi_chipSel].MISO  = init_info->gpioNum_miso;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev, init_info->gpioNum_mosi, &gpioConfig);
    spiGpioPin[spi_chipSel].MOSI  = init_info->gpioNum_mosi;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 0;   /* default low */
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev,init_info->gpioNum_sclk, &gpioConfig);
    spiGpioPin[spi_chipSel].SCLK  = init_info->gpioNum_sclk;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev,init_info->gpioNum_ss, &gpioConfig);
    spiGpioPin[spi_chipSel].SS    = init_info->gpioNum_ss;

    gpioConfig.direction = GPIO_DIR_IN;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev, init_info->gpioNum_int, &gpioConfig);
    spiGpioPin[spi_chipSel].INT   = init_info->gpioNum_int;

    //set CS high to inactive
    DRV_GPIO_SET(unit, sg_dev, spiGpioPin[spi_chipSel].SS, SS_INACTIVE);

    return RT_ERR_OK;
}


int32 _spi_dev5_83xxGpio_init(uint32 unit)
{
    spi_init_info_t init_info;
    uint32 pBaseAddr=0;

    if(unit == HWP_MY_UNIT_ID())
    {
        ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &pBaseAddr);
        gpio_data_register = (unsigned volatile int *)((uint8 *)gpio_data_register + pBaseAddr);
    }
    else
    {
        uint32 master_unit = HWP_MY_UNIT_ID();

        RT_INIT_MSG("    SPI Slave init\n");

        /* 9310 GPIO init */
        if(0 == (spi_chipSel = HWP_SWCORE_SPI_CHIP_SEL(unit)))
        {
#if SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
            uint32 tmpRegData;

            /* disable system LED to use GPIO-0 */
            ioal_mem32_read(master_unit, 0xA000, &tmpRegData);
            tmpRegData &= 0xFFFF7FFF;
            ioal_mem32_write(master_unit, 0xA000, tmpRegData);
#endif
        }

        if (spi_chipSel < SPI_MAX_CS_NUM)
        {
            init_info.gpioNum_sclk    = spiGpioPin[spi_chipSel].SCLK;
            init_info.gpioNum_mosi    = spiGpioPin[spi_chipSel].MOSI;
            init_info.gpioNum_miso    = spiGpioPin[spi_chipSel].MISO;
            init_info.gpioNum_ss      = spiGpioPin[spi_chipSel].SS;
            init_info.gpioNum_int     = spiGpioPin[spi_chipSel].INT;
            spi_dev5SpiPin_init(master_unit, &init_info);
        }
    }

      return RT_ERR_OK;
}



int32 _spi_dev5_931xGpio_init(uint32 unit)
{
    spi_init_info_t init_info;
    uint32 pBaseAddr=0;

    if(unit == HWP_MY_UNIT_ID())
    {
        RT_INIT_MSG("    SPI Master init\n");

        ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &pBaseAddr);
        gpio_data_register = (unsigned volatile int *)((uint8 *)gpio_data_register + pBaseAddr);
    }
    else
    {
        uint32 master_unit = HWP_MY_UNIT_ID();

        RT_INIT_MSG("    SPI Slave init\n");

        /* 9310 GPIO init */
        if(0 == (spi_chipSel = HWP_SWCORE_SPI_CHIP_SEL(unit)))
        {
            //combo-pin GPIO2~GPIO1 set to GPIO mode
            ioal_mem32_field_write(master_unit, RTL9310_EXT_GPIO_GLB_CTRL_ADDR, RTL9310_EXT_GPIO_GLB_CTRL_EXT_GPIO_EN_OFFSET,RTL9310_EXT_GPIO_GLB_CTRL_EXT_GPIO_EN_MASK, 0x0);
            //combo-pin GPIO14~GPIO13 set to GPIO mode
            ioal_mem32_field_write(master_unit, RTL9310_I2C_MST_IF_SEL_ADDR, RTL9310_I2C_MST_IF_SEL_GPIO_SCL_SEL_OFFSET,RTL9310_I2C_MST_IF_SEL_GPIO_SCL_SEL_MASK, 0x0);
        }
        else if(1 == (spi_chipSel = HWP_SWCORE_SPI_CHIP_SEL(unit)))
        {
            //combo-pin GPIO26~GPIO15 set to GPIO mode
            ioal_mem32_field_write(master_unit, RTL9310_I2C_MST_IF_SEL_ADDR, RTL9310_I2C_MST_IF_SEL_GPIO_SDA_SEL_OFFSET,RTL9310_I2C_MST_IF_SEL_GPIO_SDA_SEL_MASK, 0x0);
            //combo-pin GPIO31
            ioal_mem32_field_write(master_unit, RTL9310_MAC_L2_GLOBAL_CTRL2_ADDR, RTL9310_MAC_L2_GLOBAL_CTRL2_LED_SYNC_SEL_OFFSET,RTL9310_MAC_L2_GLOBAL_CTRL2_LED_SYNC_SEL_MASK, 0x0);
        }

        if (spi_chipSel < SPI_MAX_CS_NUM)
        {
            init_info.gpioNum_sclk    = spiGpioPin[spi_chipSel].SCLK;
            init_info.gpioNum_mosi    = spiGpioPin[spi_chipSel].MOSI;
            init_info.gpioNum_miso    = spiGpioPin[spi_chipSel].MISO;
            init_info.gpioNum_ss      = spiGpioPin[spi_chipSel].SS;
            init_info.gpioNum_int     = spiGpioPin[spi_chipSel].INT;
            spi_dev5SpiPin_init(master_unit, &init_info);
        }
    }

    return RT_ERR_OK;
}

int32 _spi_dev5Reg_write(uint32 unit, uint32 regAddr, uint32 *buff)
{
    uint32 master_unit = HWP_MY_UNIT_ID();
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    SPI_MasterIntf_LOCK();
    osal_memset(&cmd, 0, sizeof(cmd));

    cmd.address = regAddr;
    cmd.buf = (uint8 *)buff;
    cmd.size = 4;
    cmd.cmd_t = SPI_DEV5_CMD_WRITE;

    if (HWP_SWCORE_SPI_CHIP_SEL(unit) != HWP_NOT_USED)
    {
        spi_chipSel = HWP_SWCORE_SPI_CHIP_SEL(unit);
    }

    _spi_dev5cmd_9300Gpio(master_unit, &cmd);
    SPI_MasterIntf_UNLOCK();

    return RT_ERR_OK;
}

int32 _spi_dev5Reg_read(uint32 unit, uint32 regAddr, uint32 *buff)
{
    uint32 master_unit = HWP_MY_UNIT_ID();
    spi_cmd_t cmd;


    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    SPI_MasterIntf_LOCK();

    osal_memset(&cmd, 0, sizeof(cmd));
    *buff = 0;

    cmd.address = regAddr;
    cmd.buf = (uint8 *)buff;
    cmd.size = 4;
    cmd.cmd_t = SPI_DEV5_CMD_READ;

    /* set chip select */
    if (HWP_SWCORE_SPI_CHIP_SEL(unit) != HWP_NOT_USED)
    {
        spi_chipSel = HWP_SWCORE_SPI_CHIP_SEL(unit);
    }
    else
    {
        spi_chipSel = 0;
    }

    _spi_dev5cmd_9300Gpio(master_unit, &cmd);

    SPI_MasterIntf_UNLOCK();

    return RT_ERR_OK;
}

int32 _spi_dev5_init(uint32 unit)
{
    int32 ret=RT_ERR_FAILED;

    RT_INIT_REENTRY_CHK(spi_init[unit]);

    if (unit == HWP_MY_UNIT_ID())
    {
        /* create semaphore */
        spiMaster_sem[unit] = osal_sem_mutex_create();
        if (0 == spiMaster_sem[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_BSP), "spiMaster semaphore create failed");
            return RT_ERR_FAILED;
        }
    }

#if SPI_MASTER_INTF == SPI_MASTER_INTF_9310GPIO
    ret = _spi_dev5_931xGpio_init(unit);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    ret = _spi_dev5_83xxGpio_init(unit);
#endif

    if (RT_ERR_OK == ret)
        spi_init[unit] = INIT_COMPLETED;

    return ret;
}


/* Function Name:
 *      spi_dev5Reg_write
 * Description:
 *      Write data to device registers through SPI interface.
 *
 *      How-To-implement:
 *          (a) In your hwp (e.g., rtl9303_6x8254L_6xspi.c),
 *              set hwp_swDescp_t.swcore_access_method to HWP_SW_ACC_SPI,
 *              set swcore_access_method.swcore_spi_chip_select to the chip select ID connected to this switch chip
 *              (A hwp_swDescp_t entry represents a switch chip which has an unit ID)
 *          (b) Use the HWP_SWCORE_SPI_CHIP_SEL(unit) macro to get the chip select ID
 *          (c) Dispatch to your corresponding SPI device driver for the chip select
 * Input:
 *      unit            - unit id
 *      mAddrs          - SPI address
 *      pBuff           - transfer data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Init OK
 *      RT_ERR_FAILED   - Init failed
 * Note:
 *      None
 */
int32
spi_dev5Reg_write(uint32 unit, uint32 regAddr, uint32 *buff)
{
    if (spi_devDrv_mapper[unit].write == NULL)
    {
        return RT_ERR_DRIVER_NOT_FOUND;
    }
    return spi_devDrv_mapper[unit].write(unit, regAddr, buff);
}

/* Function Name:
 *      spi_dev5Reg_read
 * Description:
 *      Read data from device through SPI interface.
 *
 *      How-To-implement:
 *          (a) In your hwp (e.g., rtl9303_6x8254L_6xspi.c),
 *              set hwp_swDescp_t.swcore_access_method to HWP_SW_ACC_SPI,
 *              set swcore_access_method.swcore_spi_chip_select to the chip select ID connected to this switch chip
 *              (A hwp_swDescp_t entry represents a switch chip which has an unit ID)
 *          (b) Use the HWP_SWCORE_SPI_CHIP_SEL(unit) macro to get the chip select ID
 *          (c) Dispatch to your corresponding SPI device driver for the chip select
 * Input:
 *      unit            - unit id
 *      mAddrs          - SPI address
 * Output:
 *      pBuff           - received data
 * Return:
 *      RT_ERR_OK       - Read OK
 *      RT_ERR_FAILED   - Read failed
 * Note:
 *      None
 */
int32
spi_dev5Reg_read(uint32 unit, uint32 regAddr, uint32 *buff)
{
    if (spi_devDrv_mapper[unit].read == NULL)
    {
        return RT_ERR_DRIVER_NOT_FOUND;
    }
    return spi_devDrv_mapper[unit].read(unit, regAddr, buff);
}

/* Function Name:
 *      spi_dev5_init
 * Description:
 *      Initalize SPI interface used to access registers of device 5.
 *
 *      How-To-implement:
 *          (a) In your hwp (e.g., rtl9303_6x8254L_6xspi.c),
 *              set hwp_swDescp_t.swcore_access_method to HWP_SW_ACC_SPI,
 *              set swcore_access_method.swcore_spi_chip_select to the chip select ID connected to this switch chip
 *              (A hwp_swDescp_t entry represents a switch chip which has an unit ID)
 *          (b) Use the HWP_SWCORE_SPI_CHIP_SEL(unit) macro to get the chip select ID
 *          (c) Dispatch to your corresponding SPI device driver for the chip select
 * Input:
 *      unit            - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Init OK
 *      RT_ERR_FAILED   - Init failed
 * Note:
 *      None
 */
int32
spi_dev5_init(uint32 unit)
{
    int32   ret;

    if (unit == HWP_MY_UNIT_ID())
    {
        /* SPI master init */
        if ((ret = spi_dev3_init(unit)) != RT_ERR_OK)
        {
            RT_INIT_MSG("  SPI master dev3 init fail (unit %u)\n",unit);
        }

        if ((ret = _spi_dev5_init(unit)) != RT_ERR_OK)
        {
            RT_INIT_MSG("  SPI master dev5 init fail (unit %u)\n",unit);
        }
        return ret;
    }
    else
    {
        if (spi_devDrv_mapper[unit].init == NULL)
        {
            return RT_ERR_DRIVER_NOT_FOUND;
        }
        ret =  spi_devDrv_mapper[unit].init(unit);
        return ret;
    }
}



