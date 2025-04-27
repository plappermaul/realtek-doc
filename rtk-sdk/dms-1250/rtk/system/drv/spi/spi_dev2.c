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
 * Purpose : SPI master driver to max.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SPI device-type-2 (dev2) register read and write
 *           2) dev2 Frame format:
 *              (A) 8-bit command byte: [reg4,reg3,reg2,reg1,reg0,X,Read/Write,X]
 *                  (a) reg4~reg0: 5-bit register address
 *                  (b) X: don't care
 *                  (c) Read/Write: 0-read, 1-write
 *              (B) Then, 8-bit read/write data: [First-bit, ......, Last-bit]
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
#include <hal/chipdef/chip.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#include <private/drv/spi/spi_dev2.h>


/*
 * ================================(Customization)================================
 */

/* define SPI_MASTER_INTF to choose which interface as SPI interface */
#define SPI_MASTER_INTF_839xSPI               1
#define SPI_MASTER_INTF_839xGPIO              2
#define SPI_MASTER_INTF_838xGPIO              3
#define SPI_MASTER_INTF                       SPI_MASTER_INTF_839xSPI

#define USING_728_GPIO_SPI                    1

/* define this will using another API to access GPIO, to get lower API overhead */
#define SPI_USING_DIRECT_GPIO_REG_ACCESS

/* semaphore, please define per your requirement */
//to sync tne usage of GPIO DATA register
#define SPI_GpioDataReg_LOCK()
#define SPI_GpioDataReg_UNLOCK()

//to sync the usage of SPI master interface
#define SPI_MasterIntf_LOCK()
#define SPI_MasterIntf_UNLOCK()

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


/* GPIO pin number definition to simulate SPI */
#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO
  #ifdef USING_728_GPIO_SPI
    typedef enum spiGpio_pin_define_e{
        SPI_GPIO_PIN_SCLK  =16,
        SPI_GPIO_PIN_MOSI  =17,
        SPI_GPIO_PIN_MISO  =18,
        SPI_GPIO_PIN_SS    =15,
        SPI_GPIO_PIN_RES   =25,
        SPI_GPIO_PIN_INT   =19,
    }spiGpio_pin_define_t;
  #else
    typedef enum spiGpio_pin_define_e{
        SPI_GPIO_PIN_SCLK  =13,
        SPI_GPIO_PIN_MOSI  =2,
        SPI_GPIO_PIN_MISO  =1,
        SPI_GPIO_PIN_SS    =14,
        SPI_GPIO_PIN_RES   =15,
        SPI_GPIO_PIN_INT   =16,
    }spiGpio_pin_define_t;
  #endif
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
typedef enum spiGpio_pin_define_e{
    SPI_GPIO_PIN_SCLK  =2,
    SPI_GPIO_PIN_MOSI  =0,
    SPI_GPIO_PIN_MISO  =1,
    SPI_GPIO_PIN_SS    =3,
    SPI_GPIO_PIN_RES   =4,
    SPI_GPIO_PIN_INT   =5,
}spiGpio_pin_define_t;
#else
typedef enum spiGpio_pin_define_e{
    SPI_GPIO_PIN_SCLK  =0,
    SPI_GPIO_PIN_MOSI  =1,
    SPI_GPIO_PIN_MISO  =2,
    SPI_GPIO_PIN_SS    =3,
    SPI_GPIO_PIN_RES   =4,
    SPI_GPIO_PIN_INT   =5,
}spiGpio_pin_define_t;

#endif

/*
 * ============================(End of customization)============================
 */

uint32 sgPin_SCLK  = SPI_GPIO_PIN_SCLK;
uint32 sgPin_MOSI  = SPI_GPIO_PIN_MOSI;
uint32 sgPin_MISO  = SPI_GPIO_PIN_MISO;
uint32 sgPin_SS    = SPI_GPIO_PIN_SS;
uint32 sgPin_RES   = SPI_GPIO_PIN_RES;
uint32 sgPin_INT   = SPI_GPIO_PIN_INT;
static uint8 spi_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

#define SS_ACTIVE       0
#define SS_INACTIVE     1


/* define GPIO register address */
#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO
    unsigned volatile int *gpio_data_register=(uint32 *)0x350C;
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    unsigned volatile int *gpio_data_register=(uint32 *)0x350C;
#else
    unsigned volatile int *gpio_data_register=(uint32 *)0x350C;
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
};

/* define command type for read/write API */
typedef enum{
    SPI_DEV2_CMD_READ     = 0x01,
    SPI_DEV2_CMD_WRITE    = 0x02,
    SPI_DEV2_CMD_NONE     = 0x03,
}spi_cmdType_t;

typedef struct spi_cmd_s{
    uint32        flags;
    spi_cmdType_t cmd_t;
    uint32        address;
    uint8         *buf;     /*request buffer*/
    uint32        size;     /*request IO size*/
}spi_cmd_t;

/* define read/write direction */
#define SPI_DEV2_DIR_WRITE                  (0x2)
#define SPI_DEV2_DIR_READ                   (0x0)

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
    data_reg_value |= gpioInfo[sgPin_SS].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SSlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[sgPin_SS].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKhi(uint32 data_reg_value)
{
    data_reg_value |= gpioInfo[sgPin_SCLK].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[sgPin_SCLK].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo_MOSIhi(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[sgPin_SCLK].pin_mask_not;
    data_reg_value |= gpioInfo[sgPin_MOSI].pin_mask;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_SCLKlo_MOSIlo(uint32 data_reg_value)
{
    data_reg_value &= gpioInfo[sgPin_SCLK].pin_mask_not;
    data_reg_value &= gpioInfo[sgPin_MOSI].pin_mask_not;
    *gpio_data_register = data_reg_value;
    return data_reg_value;
}

uint32 spiGpio_MISO_get(uint32 *data, uint32 data_reg_value)
{
    data_reg_value = *gpio_data_register;
    *data = data_reg_value & gpioInfo[sgPin_MISO].pin_mask;
    return data_reg_value;
}


#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI

  /* for using RTL839x SPI master */
  static  uint32  spi_reg_base;
  #define SFRB   (spi_reg_base)   /*SPI Flash Register Base*/
  #define SFCR   (SFRB)           /*SPI Flash Configuration Register*/
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

  #define SFCSR  (SFRB+0x08)      /*SPI Flash Control&Status Register*/
    #define SFCSR_SPI_CSB0          (1<<31)
    #define SFCSR_SPI_CSB1          (1<<30)
    #define SFCSR_SPI_CSB2          (1<<15)
    #define SFCSR_SPI_CSB3          (1<<14)
    #define SFCSR_SPI_CSALL         (SFCSR_SPI_CSB0|SFCSR_SPI_CSB1|SFCSR_SPI_CSB2|SFCSR_SPI_CSB3)
    #define SFCSR_LEN(val)          ((val)<<28)         /*2 bits*/
    #define SFCSR_SPI_RDY           (1<<27)
    #define SFCSR_IO_WIDTH(val)     ((val)<<25)         /*2 bits*/
    #define SFCSR_CHIP_SEL          (1<<24)
    #define SFCSR_CMD_BYTE(val)     ((val)<<16)         /*8 bit, 1111_1111 */

  #define SFDR                      (SFRB+0x0C)         /*SPI Flash Data Register*/
  #define SFDR2                     (SFRB+0x10)         /*SPI Flash Data Register - for post SPI bootup setting*/
  #define SPI_CS_INIT               (SFCSR_SPI_CSALL | SPI_LEN1)
  #define SPI_WIP                   (1)                 /* Write In Progress */
  #define SPI_WEL                   (1<<1)              /* Write Enable Latch*/
  #define SPI_SST_QIO_WIP           (1<<7)              /* SST QIO Flash Write In Progress */
  #define SPI_LEN_INIT              0xCFFFFFFF          /* and SFCSR to init   */
  #define SPI_LEN4                  0x30000000          /* or SFCSR to set */
  #define SPI_LEN3                  0x20000000          /* or SFCSR to set */
  #define SPI_LEN2                  0x10000000          /* or SFCSR to set */
  #define SPI_LEN1                  0x00000000          /* or SFCSR to set */
  #define SPI_SETLEN(val)           ((val-1)<<28)

  typedef enum{
      R_MODE      =0x04,
      CS0         =0x10,
      CS1         =0x20,
      CS2         =0x40,
      CS3         =0x80
  }spi_flags;

  #define SPI_CS_DEFINE             CS1
  #define SPI_REG(reg)              *((volatile uint32 *)(reg))
  #define CHECK_READY()             do{while( !(SPI_REG(SFCSR)&SFCSR_SPI_RDY) );}while(0)


  int _spi_dev2_8390spi_start_transfer(uint32 unit, spi_cmd_t *req)
  {
      uint32 ret = 0;
      uint32 cs_active=0xFFFFFFFF;
      uint32 sfcsr_value = 0;

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

      SPI_MasterIntf_LOCK();

      /* Memory Controller Constrain, deactive, active, deactive SPI Flash */
      CHECK_READY();
      SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
      CHECK_READY();
      SPI_REG(SFCSR) &= cs_active;    /* active CS */
      CHECK_READY();
      SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
      CHECK_READY();

      /* set SFCSR*/
      CHECK_READY();
      SPI_REG(SFCSR) = sfcsr_value;

      return ret;
  }


  int _spi_dev2_8390spi_stop_transfer(uint32 unit, spi_cmd_t *req)
  {
      uint32 ret = 0;
      uint32 cs_active=0xFFFFFFFF;

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

      SPI_MasterIntf_UNLOCK();

      return ret;
  }

  int _spi_dev2cmd_raw_transfer(uint32 unit, spi_cmd_t *req)
  {

      uint32 length = req->size;
      uint8 *buf = (uint8 *)req->buf;
      uint32 temp32=0;
      uint32 transfer_idx;
      uint32 ret = 0;


      /* set SFCSR*/
      switch( req->cmd_t )
      {
          case SPI_DEV2_CMD_WRITE:
//              osal_printf("\n[%s][%d] SPI_DEV2_CMD_WRITE, length = %d\nWrite Data: ",__FUNCTION__,__LINE__,length);
              for(transfer_idx = 0; transfer_idx < length; transfer_idx++)
              {
//                osal_printf("(0x%02x) ",*(buf + transfer_idx));
                CHECK_READY();
                SPI_REG(SFDR) = (*(buf + transfer_idx))<<24;
              }
              break;
          case SPI_DEV2_CMD_READ:
              for(transfer_idx = 0; transfer_idx < length; transfer_idx++)
              {
                CHECK_READY();
                temp32 = SPI_REG(SFDR);
                *(buf + transfer_idx) = (uint8)(temp32>>24);
              }
              break;
          default:
              break;
      };

      return ret;
  }



  int _spi_dev2cmd_8390spi(uint32 unit, spi_cmd_t *req)
  {
      uint32 regAddr = req->address;
      //uint32 size = req->size;
      uint8 *buf = (uint8 *)req->buf;
      uint32 temp32=0;
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

      SPI_MasterIntf_LOCK();

      /* Memory Controller Constrain, deactive, active, deactive SPI Flash */
      CHECK_READY();
      SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
      CHECK_READY();
      SPI_REG(SFCSR) &= cs_active;    /* active CS */
      CHECK_READY();
      SPI_REG(SFCSR) = SPI_CS_INIT;   /* deactive CS */
      CHECK_READY();

      /* set SFCSR*/
      CHECK_READY();
      SPI_REG(SFCSR) = sfcsr_value;

      /* write SPI command byte */
      CHECK_READY();
      sfdr_value |= regAddr<<3;
      if(req->cmd_t == SPI_DEV2_CMD_WRITE)
          sfdr_value |= SPI_DEV2_DIR_WRITE;
      else
          sfdr_value |= SPI_DEV2_DIR_READ;
      sfdr_value <<= 24;
      SPI_REG(SFDR) = sfdr_value;

      /* read/write data */
      /* set SFCSR*/
      switch( req->cmd_t )
      {
          case SPI_DEV2_CMD_WRITE:
              CHECK_READY();
              SPI_REG(SFDR) = (*buf)<<24;
              break;
          case SPI_DEV2_CMD_READ:
              CHECK_READY();
              temp32 = SPI_REG(SFDR);
              *buf = (uint8)(temp32>>24);
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

      SPI_MasterIntf_UNLOCK();

      return ret;
  }


  int32 _spi_dev2_839xSPI_init(uint32 unit)
  {
        unsigned int    temp;
  #if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO
        drv_generalCtrlGpio_devConf_t devConfig;
        drv_generalCtrlGpio_pinConf_t gpioConfig;
  #endif
        RT_INIT_MSG("    -- 839X SPI Master\n");

        if( ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &temp) != RT_ERR_OK)
            return RT_ERR_FAILED;

        spi_reg_base = temp + 0x1200;

        /* Set flash endian as big-endian */
        SPI_REG(SFCR) |= (SFCR_EnableWBO);
        SPI_REG(SFCR) |= (SFCR_EnableRBO);

        /* init CLI_DIV */
        SPI_REG(SFCR) &= SFCR_CLK_DIV_CLEAR;
        SPI_REG(SFCR) |= SFCR_CLK_DIV(0x7);

        /* Config SPI attribute  */
#ifndef USING_728_GPIO_SPI
        /* set cs 2,3 from GPIO to SPI */
        ioal_mem32_read(unit,RTL8390_MAC_IF_CTRL_ADDR,&temp);
        temp |= RTL8390_MAC_IF_CTRL_SPI_CS_IF_SEL_MASK;
        ioal_mem32_write(unit,RTL8390_MAC_IF_CTRL_ADDR,temp);
#endif

        /* set driving current */
        ioal_mem32_read(unit,RTL8390_MAC_IO_DRIV_ABLTY_ADDR,&temp);
        temp |= RTL8390_MAC_IO_DRIV_ABLTY_SPI_IO_DRIV_SEL_MASK;
        temp |= RTL8390_MAC_IO_DRIV_ABLTY_SPI_CLK_DRIV_SEL_MASK;
        ioal_mem32_write(unit,RTL8390_MAC_IO_DRIV_ABLTY_ADDR,temp);

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO

      /* init RTL83xx GPIO interface */
      drv_generalCtrlGPIO_dev_init(unit,sg_dev, &devConfig);
      drv_generalCtrlGPIO_devEnable_set(unit,sg_dev,ENABLED);

      gpioConfig.direction = GPIO_DIR_IN;
      gpioConfig.default_value = 1;
      gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
      gpioConfig.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
      drv_generalCtrlGPIO_pin_init(unit, sg_dev, sgPin_INT, &gpioConfig);

      gpioConfig.direction = GPIO_DIR_OUT;
      gpioConfig.default_value = 1;
      gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
      gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
#if 0
      {
        drv_generalCtrlGpio_devId_t dev_tmp;
  #ifdef USING_728_GPIO_SPI
        dev_tmp = sg_dev_ext;
  #else
        dev_tmp = sg_dev;
  #endif
        drv_generalCtrlGPIO_pin_init(unit, dev_tmp, sgPin_RES, &gpioConfig);
        // reset chip
        DRV_GPIO_SET(unit, dev_tmp, sgPin_RES, 0);
        osal_time_mdelay(1);
        DRV_GPIO_SET(unit, dev_tmp, sgPin_RES, 1);
      }
#endif
#endif
      //set full duplex mode
      {
          uint8 tmp=0xFF;
          spi_dev2Reg_write(unit,17,&tmp);
      }
      spi_init[unit] = INIT_COMPLETED;

      return RT_ERR_OK;
  }


#endif //SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI


#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO

  uint32 _spi_bitbang_bitSet(uint32 unit, uint8 data, uint32 data_reg_value)
  {
  #ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
      if(data)
        data_reg_value = spiGpio_SCLKlo_MOSIhi(data_reg_value);
      else
        data_reg_value = spiGpio_SCLKlo_MOSIlo(data_reg_value);
      data_reg_value = spiGpio_SCLKhi(data_reg_value);
  #else
      DRV_GPIO_SET(unit, sg_dev, sgPin_SCLK, 0);
      DRV_GPIO_SET(unit, sg_dev, sgPin_MOSI, data);
      DRV_GPIO_SET(unit, sg_dev, sgPin_SCLK, 1);
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
      DRV_GPIO_SET(unit, sg_dev, sgPin_SCLK, 0);
      DRV_GPIO_SET(unit, sg_dev, sgPin_SCLK, 1);
      DRV_GPIO_GET(unit, sg_dev, sgPin_MISO, data);
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

  int _spi_dev2cmd_83xxGpio(uint32 unit, spi_cmd_t *req)
  {
      uint32 regAddr = req->address;
      uint8 data=0;
      unsigned volatile int gpioDataReg_value;
      uint8 writeCmd[8],writeData[8];
      uint32 readData[8];
      int i;

      /* prepare command byte */
      data |= regAddr<<3;
      if(req->cmd_t == SPI_DEV2_CMD_WRITE)
          data |= SPI_DEV2_DIR_WRITE;
      else
          data |= SPI_DEV2_DIR_READ;

      for(i=0;i<8;i++)
          writeCmd[i] = ((data>>i) & 0x1);

      /* prepare data */
      if(req->cmd_t==SPI_DEV2_CMD_WRITE){
          for(i=0;i<8;i++)
              writeData[i] = (((*(req->buf))>>i) & 0x1);
      }

      SPI_GpioDataReg_LOCK();

      /* store GPIO data register and pass to each API, to get more performance by skip reading IO */
      gpioDataReg_value = *gpio_data_register;

      /* active chip select */
  #ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
      gpioDataReg_value = spiGpio_SSlo(gpioDataReg_value);
  #else
      DRV_GPIO_SET(unit, sg_dev, sgPin_SS, SS_ACTIVE);
  #endif

      /* write SPI command byte */
      gpioDataReg_value = _spi_bitbang_byteSet(unit,writeCmd,gpioDataReg_value);

      /* read or write data */
      switch( req->cmd_t )
      {
          case SPI_DEV2_CMD_READ:
              gpioDataReg_value = _spi_bitbang_byteGet(unit,readData,gpioDataReg_value);
              break;
          case SPI_DEV2_CMD_WRITE:
              gpioDataReg_value = _spi_bitbang_byteSet(unit,writeData,gpioDataReg_value);
              break;
          default:
              break;
      };

      /* inactive chip select */
  #ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
          spiGpio_SShi(gpioDataReg_value);
  #else
          DRV_GPIO_SET(unit, sg_dev, sgPin_SS, SS_INACTIVE);
  #endif

      SPI_GpioDataReg_UNLOCK();

      /* get read data */
      if(req->cmd_t==SPI_DEV2_CMD_READ){

          *(req->buf) = 0;
          for(i=0;i<8;i++)
          {
              if(readData[i])
                  *(req->buf) |= ( 0x1<<(7-i) );
          }
      }

      return RT_ERR_OK;
  }

int _spi_dev2cmd_83xxGpio_start_taransfer(uint32 unit, spi_cmd_t *req)
{

    unsigned volatile int gpioDataReg_value;

    SPI_GpioDataReg_LOCK();

    /* store GPIO data register and pass to each API, to get more performance by skip reading IO */
    gpioDataReg_value = *gpio_data_register;

    /* active chip select */
#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
    gpioDataReg_value = spiGpio_SSlo(gpioDataReg_value);
#else
    DRV_GPIO_SET(unit, sg_dev, sgPin_SS, SS_ACTIVE);
#endif

    SPI_GpioDataReg_UNLOCK();

    return RT_ERR_OK;
}

int _spi_dev2cmd_83xxGpio_stop_taransfer(uint32 unit, spi_cmd_t *req)
{

    unsigned volatile int gpioDataReg_value;

    SPI_GpioDataReg_LOCK();

    /* store GPIO data register and pass to each API, to get more performance by skip reading IO */
    gpioDataReg_value = *gpio_data_register;

    /* inactive chip select */
#ifdef SPI_USING_DIRECT_GPIO_REG_ACCESS
        spiGpio_SShi(gpioDataReg_value);
#else
        DRV_GPIO_SET(unit, sg_dev, sgPin_SS, SS_INACTIVE);
#endif

    SPI_GpioDataReg_UNLOCK();

    return RT_ERR_OK;
}


int _spi_dev2cmd_raw_83xxGpio(uint32 unit, spi_cmd_t *req)
{
    unsigned volatile int gpioDataReg_value;
    uint8 writeData[8];
    uint32 readData[8];
    int i, loop_idx;

    uint32 length = req->size;
    uint8 *buf = (uint8 *)req->buf;

//    if(req->cmd_t==SPI_DEV2_CMD_WRITE)
//        osal_printf("\nW[%s][%d] buf = 0x%02x, length = %d\n",__FUNCTION__,__LINE__,*buf, length);

    for(loop_idx = 0; loop_idx < length; loop_idx++)
    {
        /* prepare data */
        if(req->cmd_t==SPI_DEV2_CMD_WRITE){
            for(i=0;i<8;i++)
                writeData[i] = (((*((buf)+loop_idx))>>i) & 0x1);
        }

        SPI_GpioDataReg_LOCK();

        /* store GPIO data register and pass to each API, to get more performance by skip reading IO */
        gpioDataReg_value = *gpio_data_register;

        /* read or write data */
        switch( req->cmd_t )
        {
            case SPI_DEV2_CMD_READ:
                gpioDataReg_value = _spi_bitbang_byteGet(unit,readData,gpioDataReg_value);
                break;
            case SPI_DEV2_CMD_WRITE:
                gpioDataReg_value = _spi_bitbang_byteSet(unit,writeData,gpioDataReg_value);
                break;
            default:
                break;
        };


        SPI_GpioDataReg_UNLOCK();

        /* get read data */
        if(req->cmd_t==SPI_DEV2_CMD_READ){

            *((buf)+loop_idx) = 0;
            for(i=0;i<8;i++)
            {
                if(readData[i])
                    *((buf)+loop_idx) |= ( 0x1<<(7-i) );
            }
        }
    }

//    if(req->cmd_t==SPI_DEV2_CMD_READ)
//        osal_printf("\nR[%s][%d] buf = 0x%02x, length = %d\n",__FUNCTION__,__LINE__,*buf, length);

    return RT_ERR_OK;
}


#endif //SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO



int32 spi_dev2SpiPin_init(uint32 unit, spi_init_info_t *init_info)
{
    drv_generalCtrlGpio_devConf_t devConfig;
    drv_generalCtrlGpio_pinConf_t gpioConfig;

    RT_INIT_REENTRY_CHK(spi_init[unit]);

    /* init GPIO device */
    drv_generalCtrlGPIO_dev_init(unit,sg_dev, &devConfig);
    drv_generalCtrlGPIO_devEnable_set(unit,sg_dev,ENABLED);

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
    sgPin_MISO  = init_info->gpioNum_miso;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev, init_info->gpioNum_mosi, &gpioConfig);
    sgPin_MOSI  = init_info->gpioNum_mosi;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev,init_info->gpioNum_sclk, &gpioConfig);
    sgPin_SCLK  = init_info->gpioNum_sclk;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev,init_info->gpioNum_ss, &gpioConfig);
    sgPin_SS    = init_info->gpioNum_ss;

    gpioConfig.direction = GPIO_DIR_IN;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev, init_info->gpioNum_int, &gpioConfig);
    sgPin_INT   = init_info->gpioNum_int;

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    drv_generalCtrlGPIO_pin_init(unit, sg_dev, init_info->gpioNum_reset, &gpioConfig);
    sgPin_RES   = init_info->gpioNum_reset;

    //set CLOCK as default high
    DRV_GPIO_SET(unit, sg_dev, sgPin_SCLK, 1);

    //set CS high to inactive
    DRV_GPIO_SET(unit, sg_dev, sgPin_SS, SS_INACTIVE);
    spi_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}


int32 _spi_dev2_83xxGpio_init(uint32 unit)
  {
      spi_init_info_t init_info;
      uint32 pBaseAddr=0;

      ioal_init_memRegion_get(unit, IOAL_MEM_SOC, &pBaseAddr);
      gpio_data_register = (unsigned volatile int *)((uint8 *)gpio_data_register + pBaseAddr);

  #if SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
      {
          uint32 tmpRegData;

          /* disable system LED to use GPIO-0 */
          ioal_mem32_read(unit, 0xA000, &tmpRegData);
          tmpRegData &= 0xFFFF7FFF;
          ioal_mem32_write(unit, 0xA000, tmpRegData);
      }
  #endif


      init_info.gpioNum_sclk    = sgPin_SCLK;
      init_info.gpioNum_mosi    = sgPin_MOSI;
      init_info.gpioNum_miso    = sgPin_MISO;
      init_info.gpioNum_ss      = sgPin_SS;
      init_info.gpioNum_reset   = sgPin_RES;
      init_info.gpioNum_int     = sgPin_INT;
      spi_dev2SpiPin_init(unit, &init_info);


      // reset chip
      #if 0
      DRV_GPIO_SET(unit, sg_dev, sgPin_RES,0);
      osal_time_mdelay(1);
      DRV_GPIO_SET(unit, sg_dev, sgPin_RES,1);
      #endif

      //set max3421e as full duplex mode
      {
          uint8 tmp=0xFF;
          spi_dev2Reg_write(unit,17,&tmp);
      }
      spi_init[unit] = INIT_COMPLETED;

      return RT_ERR_OK;
  }


/* Function Name:
 *      spi_dev2Reg_write
 * Description:
 *      Write data to device registers through SPI interface.
 * Input:
 *      unit                - unit id
 *      regAddr             - address of register
 *      pBuff               - data to write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
spi_dev2Reg_write(uint32 unit, uint32 regAddr, uint8 *buff)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    osal_memset(&cmd, 0, sizeof(cmd));

    cmd.address = regAddr;
    cmd.buf = (uint8 *)buff;
    cmd.size = 1;
    cmd.cmd_t = SPI_DEV2_CMD_WRITE;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2cmd_8390spi(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_83xxGpio(unit, &cmd);
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      spi_dev2Reg_read
 * Description:
 *      Read data from device through SPI interface.
 * Input:
 *      unit                - unit id
 *      regAddr             - address of register
 *      pBuff               - to store data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
spi_dev2Reg_read(uint32 unit, uint32 regAddr, uint8 *buff)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);
    osal_memset(&cmd, 0, sizeof(cmd));
    *buff = 0;

    cmd.address = regAddr;
    cmd.buf = buff;
    cmd.size = 1;
    cmd.cmd_t = SPI_DEV2_CMD_READ;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2cmd_8390spi(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_83xxGpio(unit, &cmd);
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      spi_dev2_init
 * Description:
 *      Inital SPI interface which used to access registers of device 2 (refer to above file description).
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32 spi_dev2_init(uint32 unit)
{
    int32 ret=RT_ERR_FAILED;

    RT_INIT_REENTRY_CHK(spi_init[unit]);

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    ret = _spi_dev2_839xSPI_init(unit);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    ret = _spi_dev2_83xxGpio_init(unit);
#endif
    if (RT_ERR_OK == ret)
        spi_init[unit] = INIT_COMPLETED;

    return ret;
}

/* Function Name:
 *      spi_dev2Reg_transfer_start
 * Description:
 *      Start a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_transfer_start(uint32 unit)
{
    spi_cmd_t cmd;

    osal_memset(&cmd, 0, sizeof(cmd));

    cmd.size = 1;
    cmd.cmd_t = SPI_DEV2_CMD_NONE;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2_8390spi_start_transfer(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_83xxGpio_start_taransfer(unit, &cmd);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      spi_dev2Reg_transfer_stop
 * Description:
 *      Stop a SPI transfer process.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_transfer_stop(uint32 unit)
{
    spi_cmd_t cmd;

    osal_memset(&cmd, 0, sizeof(cmd));

    cmd.size = 1;
    cmd.cmd_t = SPI_DEV2_CMD_NONE;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2_8390spi_stop_transfer(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_83xxGpio_stop_taransfer(unit, &cmd);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      spi_dev2Reg_raw_read
 * Description:
 *      Read raw data from device through SPI interface.
 * Input:
 *      unit                - unit id
 *      pBuff               - to store data
 *      len                 - get back data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_raw_read(uint32 unit, uint8 *buff, uint32 len)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    osal_memset(&cmd, 0, sizeof(cmd));
    *buff = 0;

    cmd.buf = buff;
    cmd.size = len;
    cmd.cmd_t = SPI_DEV2_CMD_READ;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2cmd_raw_transfer(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_raw_83xxGpio(unit, &cmd);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      spi_dev2Reg_raw_write
 * Description:
 *      Write raw data buffer to SPI interface.
 * Input:
 *      unit                - unit id
 *      pBuff               - transfer data
 *      len                 - transfer data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
spi_dev2Reg_raw_write(uint32 unit, uint8 *buff, uint32 len)
{
    spi_cmd_t cmd;

    /* parameter check */
    RT_PARAM_CHK((NULL == buff), RT_ERR_NULL_POINTER);

    osal_memset(&cmd, 0, sizeof(cmd));

    cmd.buf = (uint8 *)buff;
    cmd.size = len;
    cmd.cmd_t = SPI_DEV2_CMD_WRITE;

#if SPI_MASTER_INTF == SPI_MASTER_INTF_839xSPI
    cmd.flags |= SPI_CS_DEFINE;
    _spi_dev2cmd_raw_transfer(unit, &cmd);
#elif SPI_MASTER_INTF == SPI_MASTER_INTF_839xGPIO || SPI_MASTER_INTF == SPI_MASTER_INTF_838xGPIO
    _spi_dev2cmd_raw_83xxGpio(unit, &cmd);
#endif

    return RT_ERR_OK;
}

