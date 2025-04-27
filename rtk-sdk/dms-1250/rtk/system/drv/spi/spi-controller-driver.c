/*
 * Copyright (C) 2009-2017 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 81017 $
 * $Date: 2017-08-07 10:29:42 +0800 (Mon, 07 Aug 2017) $
 *
 * Purpose : Middle way to connect Linux SPI architecure
 *           This is a Demo program
 *
 *
 */

#include <common/rt_autoconf.h>

#ifndef __BOOTLOADER__
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
/* Linux include Files */
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dmaengine.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/scatterlist.h>
#include <linux/spi/spi.h>
#include <asm/irq.h>
/* SDK include Files */
#include <osal/print.h>
#include <drv/spi/spi.h>
#include <private/drv/spi/spi_private.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <ioal/mem32.h>


//#define USED_IOAL
#define  CUSTOMER_IRQ                   24
#define  CUSTOMER_MODALIAS              "max3421-hcd"       /* This string should match with user's driver */
#define  TOTAL_CS_NUM                   2
#define  GPIO_PABC_ISR                  (0x3510UL)
#define  GPIO_PC_IMR                    (0x3518UL)
#define  PABC_ISR                       (0xb8003510UL)
#define  PC_IMR                         (0xb8003518UL)
#define  INTR_FALLING_MODE              (0x01)              /* Falling Edge*/
#define  INTR_DISABLE                   (0x00)              /* Disable*/
#define  SPIUSB_DEV_IP	                (0x1 << 11)
#define  SPIUSB_DEV_INTR_ENABLE         (INTR_FALLING_MODE << 22)
#define  SPIUSB_DEV_INTR_MASK           (0x3 << 22)

uint32   drv_spi_unit;

struct spi_device;

/**
 * struct rtkSwitch_spi_info - rtkSwitch specific SPI descriptor
 * @num_chipselect: number of chip selects on this board, must be
 *                  at least one
 * @use_dma: use DMA for the transfers
 */
struct rtkSwitch_spi_info {
    int	num_chipselect;
    bool	use_dma;
};

/**
 * struct rtkSwitch_spi_chip_ops - operation callbacks for SPI slave device
 * @setup: setup the chip select mechanism
 * @cleanup: cleanup the chip select mechanism
 * @cs_control: control the device chip select
 */
struct rtkSwitch_spi_chip_ops {
    int	(*setup)(struct spi_device *spi);
    void	(*cleanup)(struct spi_device *spi);
    void	(*cs_control)(struct spi_device *spi, int value);
};


/**
 * struct rtkSwitch_spi - rtkSwitch SPI controller structure
 * @pdev: pointer to platform device
 * @clk: clock for the controller
 * @regs_base: pointer to ioremap()'d registers
 * @sspdr_phys: physical address of the SSPDR register
 * @wait: wait here until given transfer is completed
 * @current_msg: message that is currently processed (or %NULL if none)
 * @tx: current byte in transfer to transmit
 * @rx: current byte in transfer to receive
 * @fifo_level: how full is FIFO (%0..%SPI_FIFO_SIZE - %1). Receiving one
 *              frame decreases this level and sending one frame increases it.
 * @zeropage: dummy page used as RX buffer when only TX buffer is passed in by
 *            the client
 */
struct rtkSwitch_spi {
    const struct platform_device	*pdev;
    struct clk			            *clk;
    void __iomem			        *regs_base;
    unsigned long			        sspdr_phys;
    struct completion		        wait;
    struct spi_message		        *current_msg;
    size_t				            tx;
    size_t				            rx;
    size_t				            fifo_level;
    void				            *zeropage;
};

/**
 * struct rtkSwitch_spi_chip - SPI device hardware settings
 * @spi: back pointer to the SPI device
 * @ops: private chip operations
 */
struct rtkSwitch_spi_chip {
    const struct spi_device		    *spi;
    struct rtkSwitch_spi_chip_ops	*ops;
};

/* converts bits per word to CR0.DSS value */
#define bits_per_word_to_dss(bpw)	((bpw) - 1)

/*************************************************************************
 * rtkSwitch SPI Driver handling
 *************************************************************************/
static int rtkSwitch_spi_enable(const struct rtkSwitch_spi *espi)
{
    return 0;
}

static void rtkSwitch_spi_disable(const struct rtkSwitch_spi *espi)
{

}
static void rtkSwitch_spi_cs_control(struct spi_device *spi, bool control)
{

}

/**
 * rtkSwitch_spi_setup() - setup an SPI device
 * @spi: SPI device to setup
 *
 * This function sets up SPI device mode, speed etc. Can be called multiple
 * times for a single device. Returns %0 in case of success, negative error in
 * case of failure. When this function returns success, the device is
 * deselected.
 */
static int rtkSwitch_spi_setup(struct spi_device *spi)
{
    return 0;
}

/**
 * rtkSwitch_spi_cleanup() - cleans up master controller specific state
 * @spi: SPI device to cleanup
 *
 * This function releases master controller specific state for given @spi
 * device.
 */
static void rtkSwitch_spi_cleanup(struct spi_device *spi)
{

}

/**
 * rtkSwitch_spi_chip_setup() - configures hardware according to given @chip
 * @espi: rtkSwitch SPI controller struct
 * @chip: chip specific settings
 * @speed_hz: transfer speed
 * @bits_per_word: transfer bits_per_word
 */
static int rtkSwitch_spi_chip_setup(const struct rtkSwitch_spi *espi,
                 const struct rtkSwitch_spi_chip *chip,
                 u32 speed_hz, u8 bits_per_word)
{
    return 0;
}


/**
 * rtkSwitch_spi_process_transfer() - processes one SPI transfer
 * @espi: rtkSwitch SPI controller struct
 * @msg: current message
 * @t: transfer to process
 *
 * This function processes one SPI transfer given in @t. Function waits until
 * transfer is complete (may sleep) and updates @msg->status based on whether
 * transfer was successfully processed or not.
 */
static void rtkSwitch_spi_process_transfer(struct rtkSwitch_spi *espi,
                    struct spi_message *msg,
                    struct spi_transfer *t)
{
    struct rtkSwitch_spi_chip *chip = spi_get_ctldata(msg->spi);
    int err;

    msg->state = t;

    err = rtkSwitch_spi_chip_setup(espi, chip, t->speed_hz, t->bits_per_word);
    if (err) {
        dev_err(&espi->pdev->dev,
            "failed to setup chip for transfer\n");
        msg->status = err;
        return;
    }

    if((t->tx_buf != NULL)&&(t->rx_buf != NULL)&&(t->len == 2))
    {
        drv_spi_raw_write(0, (uint8 *)t->tx_buf, ((t->len)/2));
        drv_spi_raw_read(0, (uint8 *)(t->rx_buf + 1), ((t->len)/2)); /*For Demo SPI_BSU_DEVICE, RX byte should put in data[1], Need to check more */
    }else if((t->tx_buf != NULL)&&(t->rx_buf == NULL)){
        drv_spi_raw_write(0, (uint8 *)t->tx_buf, t->len);
    }else if((t->rx_buf != NULL)&&(t->tx_buf == NULL)){
        drv_spi_raw_read(0, (uint8 *)t->rx_buf, t->len);
    }else{
        printk("\n[%s][%d] Unknow SPI transfer!!!\n",__FUNCTION__,__LINE__);
    }


    msg->status = 0;

    espi->rx = 0;
    espi->tx = 0;

    /*
     * In case of error during transmit, we bail out from processing
     * the message.
     */
    if (msg->status)
        return;

    msg->actual_length += t->len;

    /*
     * After this transfer is finished, perform any possible
     * post-transfer actions requested by the protocol driver.
     */
//	if (t->delay_usecs) {
//		set_current_state(TASK_UNINTERRUPTIBLE);
//		schedule_timeout(usecs_to_jiffies(t->delay_usecs));
//	}
    if (t->cs_change) {
        if (!list_is_last(&t->transfer_list, &msg->transfers)) {
            /*
             * In case protocol driver is asking us to drop the
             * chipselect briefly, we let the scheduler to handle
             * any "delay" here.
             */
            rtkSwitch_spi_cs_control(msg->spi, false);
            cond_resched();
            rtkSwitch_spi_cs_control(msg->spi, true);
        }
    }

}

/*
 * rtkSwitch_spi_process_message() - process one SPI message
 * @espi: rtkSwitch SPI controller struct
 * @msg: message to process
 *
 * This function processes a single SPI message. We go through all transfers in
 * the message and pass them to rtkSwitch_spi_process_transfer(). Chipselect is
 * asserted during the whole message (unless per transfer cs_change is set).
 *
 * @msg->status contains %0 in case of success or negative error code in case of
 * failure.
 */
static void rtkSwitch_spi_process_message(struct rtkSwitch_spi *espi,
                       struct spi_message *msg)
{
    struct spi_transfer *t;
    int err;

    /*
     * Enable the SPI controller and its clock.
     */
    err = rtkSwitch_spi_enable(espi);
    if (err) {
        dev_err(&espi->pdev->dev, "failed to enable SPI controller\n");
        msg->status = err;
        return;
    }
    /*
     * We explicitly handle FIFO level. This way we don't have to check TX
     * FIFO status using %SSPSR_TNF bit which may cause RX FIFO overruns.
     */
    espi->fifo_level = 0;

    /*
     * Assert the chipselect.
     */
    rtkSwitch_spi_cs_control(msg->spi, true);

    drv_spi_transfer_start(0);
    list_for_each_entry(t, &msg->transfers, transfer_list) {
        rtkSwitch_spi_process_transfer(espi, msg, t);
        if (msg->status)
            break;
    }
    drv_spi_transfer_stop(0);
    /*
     * Now the whole message is transferred (or failed for some reason). We
     * deselect the device and disable the SPI controller.
     */
    rtkSwitch_spi_cs_control(msg->spi, false);
    rtkSwitch_spi_disable(espi);
}

static int rtkSwitch_spi_transfer_one_message(struct spi_master *master,
                       struct spi_message *msg)
{
    struct rtkSwitch_spi *espi = spi_master_get_devdata(master);

    msg->state = NULL;
    msg->status = 0;
    msg->actual_length = 0;

    espi->current_msg = msg;
    rtkSwitch_spi_process_message(espi, msg);
    espi->current_msg = NULL;

    spi_finalize_current_message(master);

    return 0;
}

static int rtkSwitch_spi_probe(struct platform_device *pdev)
{
    struct spi_master *master;
    struct rtkSwitch_spi_info *info;
    struct rtkSwitch_spi *espi;
    struct resource *res;
    int error;

    info = dev_get_platdata(&pdev->dev);
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "unable to get iomem resource\n");
        return -ENODEV;
    }

    master = spi_alloc_master(&pdev->dev, sizeof(*espi));
    if (!master)
        return -ENOMEM;

    master->setup = rtkSwitch_spi_setup;
    master->transfer_one_message = rtkSwitch_spi_transfer_one_message;
    master->cleanup = rtkSwitch_spi_cleanup;
    master->bus_num = pdev->id;
    master->num_chipselect = TOTAL_CS_NUM; /*This is total CS pin count*/
    master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
    master->bits_per_word_mask = SPI_BPW_RANGE_MASK(4, 16);

    platform_set_drvdata(pdev, master);

    espi = spi_master_get_devdata(master);

    espi->pdev = pdev;

    espi->clk = devm_clk_get(&pdev->dev, NULL);
    if (IS_ERR(espi->clk)) {
        dev_err(&pdev->dev, "unable to get spi clock\n");
        error = PTR_ERR(espi->clk);
        goto fail_release_master;
    }
    init_completion(&espi->wait);

    error = devm_spi_register_master(&pdev->dev, master);
    if (error) {
        dev_err(&pdev->dev, "failed to register SPI master\n");
        goto fail_free_dma;
    }
    printk("[%s] Register SPI master Success.\n",__FUNCTION__);
    return 0;

fail_free_dma:
fail_release_master:
    spi_master_put(master);

    return error;
}

static int rtkSwitch_spi_remove(struct platform_device *pdev)
{

    return 0;
}

static struct platform_driver rtkSwitch_spi_driver = {
    .driver		= {
        .name	= "rtkSwitch-spi",
        .owner	= THIS_MODULE,
    },
    .probe		= rtkSwitch_spi_probe,
    .remove		= rtkSwitch_spi_remove,
};

void rtkSwitch_spi_controller_driver_init(void)
{
    platform_driver_register(&rtkSwitch_spi_driver);
}

/*************************************************************************
 * rtkSwitch SPI Device handling
 *************************************************************************/

struct outsideDevice_platform_data {
        int gpio_nreset;        /* GPIO driving Reset pin, if any */
        bool amutec_eq_bmutec;  /* flag to enable AMUTEC=BMUTEC */
        bool enable_soft_reset;
};

static struct outsideDevice_platform_data rtkSwitch_outsideDevice_data = {
    .gpio_nreset	= -EINVAL,	/* filled in later */
};

static int rtkSwitch_outsideDevice_hw_setup(struct spi_device *spi)
{
    printk("\n[%s][%d]\n",__FUNCTION__,__LINE__);
    return 0;
}

static void rtkSwitch_outsideDevice_hw_cleanup(struct spi_device *spi)
{
    printk("\n[%s][%d]\n",__FUNCTION__,__LINE__);
}

static void rtkSwitch_outsideDevice_hw_cs_control(struct spi_device *spi, int value)
{
    printk("\n[%s][%d]\n",__FUNCTION__,__LINE__);
}

static struct rtkSwitch_spi_chip_ops rtkSwitch_outsideDevice_hw = {
    .setup		= rtkSwitch_outsideDevice_hw_setup,
    .cleanup	= rtkSwitch_outsideDevice_hw_cleanup,
    .cs_control	= rtkSwitch_outsideDevice_hw_cs_control,
};

static struct spi_board_info rtkSwitch_spi_board_info[] __initdata = {
    {
        .modalias		    = CUSTOMER_MODALIAS,
        .platform_data		= &rtkSwitch_outsideDevice_data,
        .controller_data	= &rtkSwitch_outsideDevice_hw,
        .max_speed_hz		= 6000000,
        .bus_num		    = 0,
        .chip_select		= 1,
        .mode			    = SPI_MODE_3,
        .irq                = CUSTOMER_IRQ,
    },
};

static struct rtkSwitch_spi_info rtkSwitch_spi_master_data;

static struct resource rtkSwitch_spi_resources[] = {
    DEFINE_RES_MEM(0xb8001200, 0x18),
};

static struct platform_device rtkSwitch_spi_device = {
    .name		= "rtkSwitch-spi",
    .id		    = 0,    /* Linux SPI BUS ID */
    .dev		= {
        .platform_data		= &rtkSwitch_spi_master_data,
    },
    .num_resources	= ARRAY_SIZE(rtkSwitch_spi_resources),
    .resource	= rtkSwitch_spi_resources,
};

/**
 * drv_register_linux_spi_resource() - registers spi platform device
 *
 * This function registers platform device for the rtkSwitch SPI controller.
 */
int drv_register_linux_spi_resource(uint32 unit)
{
    int retval = 0;

    printk("[%s] Register SPI resource Success.\n",__FUNCTION__);

        retval = platform_device_register(&rtkSwitch_spi_device);
    if (retval) {
        printk("%s: fail to register rtkSwitch_spi_device! (%d [%x])\n", __FUNCTION__,retval,retval);
        return retval;
    }

    spi_register_board_info(rtkSwitch_spi_board_info,  ARRAY_SIZE(rtkSwitch_spi_board_info));

    rtkSwitch_spi_controller_driver_init();

    return retval;
}

/*************************************************************************
 * rtkSwitch SPI Device Interrupt Setup
 *************************************************************************/

extern void rtk_intr_func_hook(unsigned int ip, unsigned int irq, unsigned int handle_pending, int (*dispatch)(void), void (*enable)(void), void (*disable)(void));


int rtkSwitch_spiDev_ipDispatcher(void)
{
    unsigned int gpio_shared_ip = 0;
#ifdef USED_IOAL
    gpio_shared_ip = ioal_soc_mem32_read(drv_spi_unit, GPIO_PABC_ISR, &gpio_shared_ip);
#else
    gpio_shared_ip = REG32(PABC_ISR);
#endif
    if((gpio_shared_ip & SPIUSB_DEV_IP) != 0)
    {
        return 1; /*Match; To performace IRQ*/
    }
    return 0; /* Mis-match*/
}

void rtkSwitch_spiDev_irq_enable(void)
{
    unsigned int gpio_shared_ip = 0;
#ifdef USED_IOAL
    ioal_soc_mem32_read(drv_spi_unit, GPIO_PC_IMR, &gpio_shared_ip);
    gpio_shared_ip = (gpio_shared_ip & (~(SPIUSB_DEV_INTR_MASK)));
    gpio_shared_ip = (gpio_shared_ip | SPIUSB_DEV_INTR_ENABLE);
    ioal_soc_mem32_write(drv_spi_unit, GPIO_PC_IMR, gpio_shared_ip);
#else
    gpio_shared_ip = REG32(PC_IMR);
    gpio_shared_ip = (gpio_shared_ip & (~(SPIUSB_DEV_INTR_MASK)));
    REG32(PC_IMR) = (gpio_shared_ip | SPIUSB_DEV_INTR_ENABLE);
#endif
}

void rtkSwitch_spiDev_irq_disable(void)
{
    unsigned int gpio_shared_ip = 0;
#ifdef USED_IOAL
    ioal_soc_mem32_read(drv_spi_unit, GPIO_PC_IMR, &gpio_shared_ip);
    gpio_shared_ip = (gpio_shared_ip & (~(SPIUSB_DEV_INTR_MASK)));
    ioal_soc_mem32_write(drv_spi_unit, GPIO_PC_IMR, gpio_shared_ip);
#else
    gpio_shared_ip = REG32(PC_IMR);
    REG32(PC_IMR) = (gpio_shared_ip & (~(SPIUSB_DEV_INTR_MASK)));
#endif
}



int usb_demo_gpio_interrupt_callback(void *pBuff)
{
    return 0;
}

/* In this Example, the exetrnal SPI USB device interrupt pin connected to GPIO pin 19 */
int drv_spi_dev_customer_setup(uint32 unit)
{
    drv_generalCtrlGpio_devConf_t spi_usb_intr_dev;
    drv_generalCtrlGpio_pinConf_t spi_usb_intr_pin;

    drv_spi_unit = unit;

    /* SPI USB device interrupt pin connected to internal GPIO pin 19 */
    spi_usb_intr_dev.default_value = 1;
    spi_usb_intr_dev.direction = GPIO_DIR_IN;
    drv_generalCtrlGPIO_dev_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, &spi_usb_intr_dev);

    spi_usb_intr_pin.default_value = 1;
    spi_usb_intr_pin.direction = GPIO_DIR_IN;
    spi_usb_intr_pin.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    spi_usb_intr_pin.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
    spi_usb_intr_pin.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
    drv_generalCtrlGPIO_pin_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, 19, &spi_usb_intr_pin);

    drv_generalCtrlGPIO_intrHandler_register(unit, GEN_GPIO_DEV_ID0_INTERNAL, 19, &usb_demo_gpio_interrupt_callback);

    drv_generalCtrlGPIO_devEnable_set(unit, GEN_GPIO_DEV_ID0_INTERNAL, ENABLED);

    rtk_intr_func_hook(5, CUSTOMER_IRQ, 0, rtkSwitch_spiDev_ipDispatcher, rtkSwitch_spiDev_irq_enable, rtkSwitch_spiDev_irq_disable);

    return 0;
}

#endif /* defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) */
#endif /* __BOOTLOADER__ */


