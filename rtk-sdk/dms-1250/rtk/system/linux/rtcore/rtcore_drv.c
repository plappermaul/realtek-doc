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
 * $Revision: 103112 $
 * $Date: 2019-12-27 16:17:03 +0800 (Fri, 27 Dec 2019) $
 *
 * Purpose : Realtek Switch SDK Core Module.
 *
 * Feature : Realtek Switch SDK Core Module
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/kdev_t.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#include <common/rt_chip.h>
#include <osal/spl.h>
#include <osal/sem.h>
#include <osal/cache.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <private/drv/swcore/swcore.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <common/util/rt_util_time.h>
#include <common/util/rt_util_intr.h>
#include <common/util/rt_util_intrk.h>
#include <rtcore/rtcore.h>
#include <drv/gpio/gpio.h>
#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif
#include <drv/watchdog/watchdog.h>
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
  #include <rtcore/user/rtcore_drv_usr.h>
  #include <private/drv/swcore/swcore_rtl8390.h>
  #include <private/drv/swcore/swcore_rtl8380.h>
  #include <private/drv/swcore/swcore_rtl9300.h>
  #include <private/drv/swcore/swcore_rtl9310.h>
  #include <ioal/mem32.h>
  #include <osal/isr.h>
  #include <drv/l2ntfy/l2ntfy.h>
  #include <drv/gpio/generalCtrl_gpio.h>
  #if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
    #include <linux/sched.h>
  #endif
#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */
#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
  #include <rtcore/rtcore_pci.h>
#endif
#include <drv/gpio/ext_gpio.h>
#include <private/drv/rtl8231/rtl8231.h>
#include <private/drv/rtl8231/rtl8231_probe.h>
#if defined(CONFIG_SDK_UART1)
  #include <drv/uart/uart.h>
#endif
#include <drv/spi/spi.h>
#include <common/debug/mem.h>
#include <ioal/ioal_log.h>
#include <ioal/ioal_init.h>

#include <linux/uaccess.h>
#include <dev_config.h>


/*
 * Symbol Definition
 */
#define RTCORE_DRV_MAJOR            200
#define RTCORE_DRV_NAME             "rtcore"
#define SYS_DEFAULT_DEVICE_ID       0

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
#define SWCORE_INT_OCCUR    0
#define SWCORE_INT_CLEAR    1
#endif

void rtcore_vma_open(struct vm_area_struct *vma);
void rtcore_vma_close(struct vm_area_struct *vma);
static int rtcore_open(struct inode *inode, struct file *file);
static int rtcore_release(struct inode *inode, struct file *file);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
static int rtcore_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static long rtcore_ioctl( struct file *file, unsigned int cmd, unsigned long arg);
#endif
static int rtcore_mmap(struct file *filp, struct vm_area_struct *vma);

#if !defined(CONFIG_SDK_EXTERNAL_CPU)
extern void prom_bsp_memsize_info(unsigned int *low_memory_size, unsigned int *high_memory_size, unsigned int *dma_reserved_size);
#endif/* #if !defined(CONFIG_SDK_EXTERNAL_CPU) */

/*
 * Data Declaration
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
static const struct file_operations rtcore_fops = {
    .owner		= THIS_MODULE,
    .open		= rtcore_open,
    .release	= rtcore_release,
    .ioctl		= rtcore_ioctl,
        .mmap		= rtcore_mmap,
    };
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static const struct file_operations rtcore_fops = {
    .owner		= THIS_MODULE,
    .open		= rtcore_open,
    .release	= rtcore_release,
    .unlocked_ioctl		= rtcore_ioctl,
    .mmap       = rtcore_mmap,
};
#endif
static struct vm_operations_struct rtcore_remap_vm_ops = {
    .open       = rtcore_vma_open,
    .close      = rtcore_vma_close,
};

typedef struct rtcore_dev_s
{
    void *rt_data;
    struct cdev rt_cdev;
} rtcore_dev_t;

rtcore_dev_t *rtcore_devices;
int rtcore_num = RTCORE_DEV_NUM;

wait_queue_head_t nic_intr_wait_queue;
atomic_t nic_wait_for_intr;

#if (defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
static long unsigned int _nic_intr_enabled = DISABLED;
static uint32 nic_intr_status = 0;
uint32  l2NotifyEventCnt = 0;
uint32  curPos = 0;
extern uint32 totalEventCount;
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
static wait_queue_head_t ntfy_intr_wait_queue;
static atomic_t ntfy_wait_for_intr;
#endif
#endif /* (defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */


#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
static wait_queue_head_t sw_intr_wait_queue;
static atomic_t sw_wait_for_intr;
static uint32 _sw_intr_enabled = DISABLED;
static uint32 rtcore_isr_glb_sts = 0;
static uint32 _sw_intr_wait_queue_init = FALSE;
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */



#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#else
static uint32 rtcoreUsr_init_status = INIT_NOT_COMPLETED;
#endif

rtk_uk_shared_t *rtk_uk_sharedMem=NULL;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
osal_isrret_t
_nic_intr_handler(void *isr_param)
{
    uint32 unit=HWP_MY_UNIT_ID();
#if defined (CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 val1, val2;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 val3, val4;
#endif

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        ioal_mem32_read(unit, RTL8390_DMA_IF_INTR_STS_ADDR, &val1);
        ioal_mem32_read(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, &val2);

        if (val1 & 0xFFFFF)
        {
            val2 &= 0x700000;
            ioal_mem32_write(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, val2);            
            if (atomic_dec_return(&nic_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&nic_intr_wait_queue);
            }

            if ((val1 >> 20) == 0)
                return OSAL_INT_HANDLED;
        }
        
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
        if (val1 >> 20)
        {
            val2 &= 0xfffff;
            ioal_mem32_write(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, val2);            
            if (atomic_dec_return(&ntfy_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&ntfy_intr_wait_queue);
            }
            return OSAL_INT_HANDLED;
        }
#endif
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
    {
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, (0x00000000U));
        
        if (atomic_dec_return(&nic_wait_for_intr) >= 0)
        {
            wake_up_interruptible(&nic_intr_wait_queue);
        }
        return OSAL_INT_HANDLED;
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        ioal_mem32_read(unit, RTL9300_DMA_IF_INTR_RX_DONE_STS_ADDR, &val1);
        ioal_mem32_read(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, &val2);
        ioal_mem32_read(unit, RTL9300_DMA_IF_INTR_TX_DONE_STS_ADDR, &val3);
        ioal_mem32_read(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, &val4);
        if (val1 || val2 || val3)
        {
            ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_DONE_MSK_ADDR, (0x00000000U));
            ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, (0x00000000U));
            ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, (0x00000000U));
            if (atomic_dec_return(&nic_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&nic_intr_wait_queue);
            }

            if (val4 == 0)
                return OSAL_INT_HANDLED;
        }

        if (val4)
        {
            ioal_mem32_write(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, (0x00000000U));
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
            if (atomic_dec_return(&ntfy_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&ntfy_intr_wait_queue);
            }
#endif
            return OSAL_INT_HANDLED;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_DONE_STS_ADDR, &val1);
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, &val2);
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_TX_DONE_STS_ADDR, &val3);
        ioal_mem32_read(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, &val4);
        if (val1 || val2 || val3)
        {
            ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_DONE_MSK_ADDR, (0x00000000U));
            ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, (0x00000000U));
            ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, (0x00000000U));
            if (atomic_dec_return(&nic_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&nic_intr_wait_queue);
            }

            if (val4 == 0)
                return OSAL_INT_HANDLED;
        }
        
        if (val4)
        {
            ioal_mem32_write(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, (0x00000000U));
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
            if (atomic_dec_return(&ntfy_wait_for_intr) >= 0)
            {
                wake_up_interruptible(&ntfy_intr_wait_queue);
            }
#endif
            return OSAL_INT_HANDLED;
        }
    }
#endif

    return OSAL_INT_HANDLED;
} /* end of _nic_isr_handler */



#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */



#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
void
rtcore_swcoreIntr_wakeup(uint32 *pisr_glb_sts)
{
    if (_sw_intr_wait_queue_init == FALSE)
    {
        init_waitqueue_head(&sw_intr_wait_queue);
        _sw_intr_wait_queue_init = TRUE;
    }

    rtcore_isr_glb_sts |= *pisr_glb_sts;
    atomic_set(&sw_wait_for_intr, SWCORE_INT_OCCUR);
    wake_up_interruptible(&sw_intr_wait_queue);
}
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */


static int rtcore_open(struct inode *inode, struct file *file)
{
    rtcore_dev_t *dev;

    dev = container_of(inode->i_cdev, rtcore_dev_t, rt_cdev);
    dev->rt_data = rtcore_devices[SYS_DEFAULT_DEVICE_ID].rt_data;
    file->private_data = dev;

    return RT_ERR_OK;
}

static int rtcore_release(struct inode *inode, struct file *file)
{
    return RT_ERR_OK;
}

void rtcore_vma_open(struct vm_area_struct *vma)
{
    osal_printf(KERN_DEBUG "VMA Open, Virt %lx, Phys %lx\n",
            vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void rtcore_vma_close(struct vm_area_struct *vma)
{
    osal_printf(KERN_DEBUG "VMA Close\n");
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
static int rtcore_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static long rtcore_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    rtcore_ioctl_t dio;

    if (copy_from_user(&dio, (void*)arg, sizeof(dio)))
    {
        return -EFAULT;
    }

    switch (cmd)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)

        case RTCORE_CACHE_FLUSH:
            dio.ret = osal_cache_memory_flush(dio.data[0], dio.data[1]);
            break;

#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
        case RTCORE_PCI_INFO_GET:
        {
            rtcore_pci_dev_t   *pci_info;
            pci_info = (rtcore_pci_dev_t *)&dio.data[1];
            dio.ret = rtcore_pci_info_get(dio.data[0], pci_info);
            break;
        }
#endif

#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
        case RTCORE_NIC_DBG_GET:
            dio.ret = drv_nic_dbg_get(dio.data[0], &dio.data[1]);
            break;

        case RTCORE_NIC_DBG_SET:
            dio.ret = drv_nic_dbg_set(dio.data[0], dio.data[1]);
            break;

        case RTCORE_NIC_CNTR_CLEAR:
            dio.ret = drv_nic_cntr_clear(dio.data[0]);
            break;

        case RTCORE_NIC_CNTR_DUMP:
            dio.ret = drv_nic_cntr_dump(dio.data[0]);
            break;

        case RTCORE_NIC_BUF_DUMP:
            dio.ret = drv_nic_ringbuf_dump(dio.data[0], dio.data[1]);
            break;

        case RTCORE_NIC_PHMBUF_DUMP:
            dio.ret = drv_nic_pktHdrMBuf_dump(dio.data[0], dio.data[1], dio.data[2], dio.data[3], dio.data[4]);
            break;

        case RTCORE_NIC_RX_START:
            dio.ret = drv_nic_rx_start(dio.data[0]);
            break;

        case RTCORE_NIC_RX_STOP:
            dio.ret = drv_nic_rx_stop(dio.data[0]);
            break;

        case RTCORE_NIC_RX_STATUS_GET:
            dio.ret = drv_nic_rx_status_get(dio.data[0], &dio.data[1]);
            break;

        case RTCORE_NIC_PKT_TX:
            dio.ret = drv_nic_pkt_tx(dio.data[0], (drv_nic_pkt_t *)dio.data[1], (drv_nic_tx_cb_f) dio.data[2], (void *) dio.data[3]);
            break;

        case RTCORE_NIC_RESET:
            dio.ret = drv_nic_reset(dio.data[0]);
            break;

#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE) */

        case RTCORE_INTR_ENABLE_SET:
            {
                rtcore_intr_type_t  intr_type;
                intr_type = dio.data[2];
#if (defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
                if (INTR_TYPE_NIC == intr_type)
                {
                    if (ENABLED == dio.data[1])
                    {
                        if(DISABLED == test_and_set_bit(0, &_nic_intr_enabled))
                        {
                            init_waitqueue_head(&nic_intr_wait_queue);
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
                            init_waitqueue_head(&ntfy_intr_wait_queue);
#endif
#if !defined(CONFIG_SDK_DRIVER_EXTC_NIC)
                            dio.ret = osal_isr_register(RTK_DEV_NIC, _nic_intr_handler, NULL);
#endif
                        }
                    }
                    else
                    {
#if !defined(CONFIG_SDK_DRIVER_EXTC_NIC)
                        if(ENABLED == test_and_clear_bit(0, &_nic_intr_enabled))
                        {
                            dio.ret = osal_isr_unregister(RTK_DEV_NIC);
                        }
#endif
                    }
                }
                else
#endif
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
                if (INTR_TYPE_SWCORE == intr_type)
                {
                    if (ENABLED == dio.data[1])
                    {
                        atomic_set(&sw_wait_for_intr, SWCORE_INT_CLEAR);
                        if (_sw_intr_wait_queue_init == FALSE)
                        {
                            init_waitqueue_head(&sw_intr_wait_queue);
                            _sw_intr_wait_queue_init = TRUE;
                        }
                        if(DISABLED == _sw_intr_enabled)
                        {
                            _sw_intr_enabled = ENABLED;
                        }
                    }
                    else
                    {
                        if(ENABLED == _sw_intr_enabled)
                        {
                            _sw_intr_enabled = DISABLED;
                        }
                    }
                }
                else
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */
                {
                    RT_LOG(LOG_DEBUG, MOD_RTDRV, "RTCORE_INTR_ENABLE_SET %u type=%u", dio.data[1], intr_type);
                }
            }
            break;
        case RTCORE_INTR_WAIT:
            {
                rtcore_intr_type_t  intr_type;
                intr_type = dio.data[1];
#if (defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
                if (INTR_TYPE_NIC == intr_type)
                {
                    if (nic_wait_for_intr.counter <= 0)
                    {
                        atomic_inc(&nic_wait_for_intr);
                    }
                    wait_event_interruptible(nic_intr_wait_queue, atomic_read(&nic_wait_for_intr) <= 0);
                   /*
                    * only run the interrupt handler once.
                    */
                    atomic_set(&nic_wait_for_intr, 0);
                    dio.data[2] = nic_intr_status;
                    dio.data[3] = l2NotifyEventCnt;
                    dio.data[4] = curPos;
                }
                else if (INTR_TYPE_NTFY == intr_type)
                {
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
                    if (ntfy_wait_for_intr.counter <= 0)
                    {
                        atomic_inc(&ntfy_wait_for_intr);
                    }
                    wait_event_interruptible(ntfy_intr_wait_queue, atomic_read(&ntfy_wait_for_intr) <= 0);
                   /*
                    * only run the interrupt handler once.
                    */
                    atomic_set(&ntfy_wait_for_intr, 0);
#endif
                }
                else
#endif /* (defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
                if (INTR_TYPE_SWCORE == intr_type)
                {
                    wait_event_interruptible(sw_intr_wait_queue, (atomic_read(&sw_wait_for_intr) == SWCORE_INT_OCCUR));
                    atomic_set(&sw_wait_for_intr, SWCORE_INT_CLEAR);
                    dio.data[2] = rtcore_isr_glb_sts;
                    rtcore_isr_glb_sts = 0;
                }
                else
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */
                {
                    RT_LOG(LOG_DEBUG, MOD_RTDRV, "RTCORE_INTR_WAIT type=%u", intr_type);
                }
            }
            break;

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        case RTCORE_INTR_ISR_ID_SET:
            {
                drv_intr_isr_t isr_id;
                uint32  os_register;
                isr_id = dio.data[0];
                os_register = dio.data[1];
                if (os_register)
                {
                    dio.ret = _rt_util_intr_os_register(isr_id);
                }
                else
                {
                    dio.ret = _rt_util_intr_os_unregister(isr_id);
                }
            }
            break;

  #if defined(CONFIG_SDK_DRIVER_GPIO)
        case RTCORE_GPIO_ISR_STS_SHADOW_GET:
            dio.ret = drv_gpio_isrStsShadow_get(dio.data[0], dio.data[1], &dio.data[2]);
            break;
  #endif /* defined(CONFIG_SDK_DRIVER_GPIO) */

#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */

#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        case RTCORE_REG_READ:
            dio.data[2] = MEM32_READ(dio.data[1]);
            //debug_mem_read(dio.data[0], dio.data[1], &dio.data[2]);
            break;

        case RTCORE_REG_WRITE:
            MEM32_WRITE(dio.data[1], dio.data[2]);
            //debug_mem_write(dio.data[0], dio.data[1], dio.data[2]);
            break;

#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)


#if defined(CONFIG_SDK_DRIVER_I2C)
        case RTCORE_I2C_INIT:
            dio.ret = drv_i2c_init(dio.data[0]);
            break;
        case RTCORE_I2C_DEV_INIT:
            i2c_device_struct.device_id = dio.data[1];
            i2c_device_struct.mem_addr_width = dio.data[2];
            i2c_device_struct.dev_addr = dio.data[3];
            i2c_device_struct.data_width = dio.data[4];
            i2c_device_struct.clk_freq = dio.data[5];
            i2c_device_struct.scl_delay = dio.data[6];
            i2c_device_struct.scl_dev = dio.data[7];
            i2c_device_struct.scl_pin_id = dio.data[8];
            i2c_device_struct.sda_dev = dio.data[9];
            i2c_device_struct.sda_pin_id = dio.data[10];
            i2c_device_struct.i2c_interface_id = dio.data[11];
            i2c_device_struct.read_type = dio.data[12];
            dio.ret = drv_i2c_dev_init(dio.data[0], &i2c_device_struct);
            break;
        case RTCORE_I2C_READ:
            dio.ret = drv_i2c_read(dio.data[0], dio.data[1],  dio.data[2], &dio.data[3]);
            break;
        case RTCORE_I2C_WRITE:
            dio.ret = drv_i2c_write(dio.data[0], dio.data[1],  dio.data[2], &dio.data[3]);
            break;
#endif
#if defined(CONFIG_SDK_RTL8231)
        /* EXT_GPIO */
        case RTCORE_EXTGPIO_REG_WRITE:
            dio.ret = drv_extGpio_reg_write(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_EXTGPIO_DEV_INIT:
            {
                drv_extGpio_devConf_t devconf;
                devconf.access_mode = dio.data[2];
                devconf.address = dio.data[3];
                devconf.page = dio.data[4];
                dio.ret = drv_extGpio_dev_init(dio.data[0], dio.data[1], &devconf);
            }
            break;
        case RTCORE_EXTGPIO_DEVENABLE_GET:
            {
                rtk_enable_t    enable;
                dio.ret = drv_extGpio_devEnable_get(dio.data[0], dio.data[1], &enable);
                dio.data[2] = enable;
            }
            break;
        case RTCORE_EXTGPIO_REG_READ:
            dio.ret = drv_extGpio_reg_read(dio.data[0], dio.data[1], dio.data[2], &dio.data[3]);
            break;
        case RTCORE_EXTGPIO_DEVREADY_GET:
            dio.ret = drv_extGpio_devReady_get(dio.data[0], dio.data[1], &dio.data[2]);
            break;
        case RTCORE_EXTGPIO_DEV_GET:
            {
                drv_extGpio_devConf_t devconf;
                dio.ret = drv_extGpio_dev_get(dio.data[0], dio.data[1], &devconf);
                dio.data[2] = devconf.access_mode;
                dio.data[3] = devconf.address;
                dio.data[4] = devconf.page;
            }
            break;
        case RTCORE_EXTGPIO_DEVENABLE_SET:
            dio.ret = drv_extGpio_devEnable_set(dio.data[0], dio.data[1], dio.data[2]);
            break;
        case RTCORE_EXTGPIO_SYNCENABLE_GET:
            {
                rtk_enable_t    enable;
                dio.ret = drv_extGpio_syncEnable_get(dio.data[0], dio.data[1], &enable);
                dio.data[2] = enable;
            }
            break;
        case RTCORE_EXTGPIO_SYNCENABLE_SET:
            dio.ret = drv_extGpio_syncEnable_set(dio.data[0], dio.data[1], dio.data[2]);
            break;
        case RTCORE_EXTGPIO_SYNCSTATUS_GET:
            dio.ret = drv_extGpio_syncStatus_get(dio.data[0], dio.data[1], &dio.data[2]);
            break;
        case RTCORE_EXTGPIO_SYNC_START:
            dio.ret = drv_extGpio_sync_start(dio.data[0], dio.data[1]);
            break;
        case RTCORE_EXTGPIO_PIN_GET:
            {
                drv_extGpio_conf_t  conf;
                dio.ret = drv_extGpio_pin_get(dio.data[0], dio.data[1], dio.data[2], &conf);
                dio.data[3] = conf.direction;
                dio.data[4] = conf.debounce;
                dio.data[5] = conf.inverter;
            }
            break;
        case RTCORE_EXTGPIO_PIN_INIT:
            {
                drv_extGpio_conf_t  conf;
                conf.direction = dio.data[3];
                conf.debounce = dio.data[4];
                conf.inverter = dio.data[5];
                dio.ret = drv_extGpio_pin_init(dio.data[0], dio.data[1], dio.data[2], &conf);
            }
            break;
        case RTCORE_EXTGPIO_DATABIT_GET:
            dio.ret = drv_extGpio_dataBit_get(dio.data[0], dio.data[1], dio.data[2], &dio.data[3]);
            break;
        case RTCORE_EXTGPIO_DATABIT_SET:
            dio.ret = drv_extGpio_dataBit_set(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_EXTGPIO_DEVRECOVERY_START:
            dio.ret = drv_extGpio_devRecovery_start(dio.data[0], dio.data[1]);
            break;
        /* RTL8231 */
        case RTCORE_RTL8231_I2C_READ:
            dio.ret = drv_rtl8231_i2c_read(dio.data[0], dio.data[1], dio.data[2], &dio.data[3]);
            break;
        case RTCORE_RTL8231_I2C_WRITE:
            dio.ret = drv_rtl8231_i2c_write(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_RTL8231_MDC_READ:
            dio.ret = drv_rtl8231_mdc_read(dio.data[0], dio.data[1], dio.data[2], dio.data[3], &dio.data[4]);
            break;
        case RTCORE_RTL8231_MDC_WRITE:
            dio.ret = drv_rtl8231_mdc_write(dio.data[0], dio.data[1], dio.data[2], dio.data[3], dio.data[4]);
            break;
        case RTCORE_EXTGPIO_DIRECTION_GET:
            dio.ret = drv_extGpio_direction_get(dio.data[0], dio.data[1], dio.data[2], (drv_gpio_direction_t *)&dio.data[3]);
            break;
        case RTCORE_EXTGPIO_DIRECTION_SET:
            dio.ret = drv_extGpio_direction_set(dio.data[0], dio.data[1], dio.data[2], (drv_gpio_direction_t)dio.data[3]);
            break;
        case RTCORE_EXTGPIO_I2C_INIT:
            dio.ret = drv_extGpio_i2c_init(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_EXTGPIO_I2C_READ:
            dio.ret = drv_extGpio_i2c_read(dio.data[0], dio.data[1], dio.data[2], &dio.data[3]);
            break;
        case RTCORE_EXTGPIO_I2C_WRITE:
            dio.ret = drv_extGpio_i2c_write(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
#endif /* defined(CONFIG_SDK_RTL8231) */

#ifdef CONFIG_SDK_DRIVER_GPIO
         /* General GPIO */
        case RTCORE_GENCTRL_GPIO_DEV_INIT:
            {
                drv_generalCtrlGpio_devConf_t config_dev;
                if(dio.data[1] == GEN_GPIO_DEV_ID0_INTERNAL)
                {

                }
                else
                {
                    config_dev.direction = dio.data[3];
                    config_dev.default_value = dio.data[4];
                    config_dev.ext_gpio.access_mode = dio.data[5];
                    config_dev.ext_gpio.address = dio.data[6];
                    config_dev.ext_gpio.page= dio.data[7];
                }
                dio.ret = drv_generalCtrlGPIO_dev_init(dio.data[0], dio.data[1], dio.data[2], &config_dev);
            }
            break;
        case RTCORE_GENCTRL_GPIO_DEV_ENABLE_SET:
            dio.ret = drv_generalCtrlGPIO_devEnable_set(dio.data[0], dio.data[1], dio.data[2]);
            break;
        case RTCORE_GENCTRL_GPIO_PIN_INIT:
            {
                drv_generalCtrlGpio_pinConf_t config_pin;
                if(dio.data[1] == GEN_GPIO_DEV_ID0_INTERNAL)
                {
                    config_pin.direction = dio.data[3];
                    config_pin.default_value = dio.data[4];
                    config_pin.int_gpio.function = dio.data[5];
                    config_pin.int_gpio.interruptEnable = dio.data[6];
                }
                else
                {
                    config_pin.direction = dio.data[3];
                    config_pin.default_value = dio.data[4];
                    config_pin.ext_gpio.direction = dio.data[5];
                    config_pin.ext_gpio.debounce = dio.data[6];
                    config_pin.ext_gpio.inverter = dio.data[7];
                }
                dio.ret = drv_generalCtrlGPIO_pin_init(dio.data[0], dio.data[1], dio.data[2], &config_pin);
            }
            break;
        case RTCORE_GENCTRL_GPIO_DATABIT_SET:
            dio.ret = drv_generalCtrlGPIO_dataBit_set(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_GENCTRL_GPIO_DATABIT_GET:
            dio.ret = drv_generalCtrlGPIO_dataBit_get(dio.data[0], dio.data[1], dio.data[2], &dio.data[3]);
            break;
#endif /* CONFIG_SDK_DRIVER_GPIO */

         case RTCORE_WATCHDOG_INIT:
            dio.ret = drv_watchdog_init((uint32)dio.data[0]);
            break;
         case RTCORE_WATCHDOG_SCALE_SET:
            dio.ret = drv_watchdog_scale_set((uint32)dio.data[0], (drv_watchdog_scale_t)dio.data[1]);
            break;
         case RTCORE_WATCHDOG_SCALE_GET:
            dio.ret = drv_watchdog_scale_get((uint32)dio.data[0], (drv_watchdog_scale_t*)&dio.data[1]);
            break;
         case RTCORE_WATCHDOG_ENABLE_SET:
            dio.ret = drv_watchdog_enable_set((uint32)dio.data[0], (uint32)dio.data[1]);
            break;
         case RTCORE_WATCHDOG_ENABLE_GET:
            dio.ret = drv_watchdog_enable_get((uint32)dio.data[0], (uint32*)&dio.data[1]);
            break;
         case RTCORE_WATCHDOG_KICK:
            dio.ret = drv_watchdog_kick((uint32)dio.data[0]);
            break;
         case RTCORE_WATCHDOG_THRESHOLD_SET:
            {
                drv_watchdog_threshold_t watchdog_threshold;
                watchdog_threshold.phase_1_threshold = dio.data[1];
                watchdog_threshold.phase_2_threshold = dio.data[2];
                dio.ret = drv_watchdog_threshold_set((uint32)dio.data[0], &watchdog_threshold);
                break;
            }
         case RTCORE_WATCHDOG_THRESHOLD_GET:
            {
                drv_watchdog_threshold_t watchdog_threshold;
                dio.ret = drv_watchdog_threshold_get((uint32)dio.data[0], &watchdog_threshold);
                dio.data[1] = watchdog_threshold.phase_1_threshold ;
                dio.data[2] = watchdog_threshold.phase_2_threshold ;
                break;
            }

#if defined(CONFIG_SDK_UART1)
         case RTCORE_UART1_CHAR_GET:
            dio.ret = drv_uart_getc((uint32)dio.data[0], &uart_data, (uint32) dio.data[2]);
            dio.data[1] = (int32)uart_data;
            break;
         case RTCORE_UART1_CHAR_PUT:
            dio.ret = drv_uart_putc((uint32)dio.data[0], (uint8)dio.data[1]);
            break;
         case RTCORE_UART1_BAUDRATE_GET:
            dio.ret = drv_uart_baudrate_get((uint32)dio.data[0], &uart_baudrate);
            dio.data[1] = (int32)uart_baudrate;
            break;
         case RTCORE_UART1_BAUDRATE_SET:
            dio.ret = drv_uart_baudrate_set((uint32)dio.data[0], (drv_uart_baudrate_t)dio.data[1]);
            break;
         case RTCORE_UART1_INTERFACE_SET:
            dio.ret = drv_uart_interface_set((uint32)dio.data[0], (drv_uart_interface_t)dio.data[1]);
            break;
#endif /* defined(CONFIG_SDK_UART1) */

#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */

#if !defined(CONFIG_SDK_EXTERNAL_CPU)
        case RTCORE_MEM_SIZE_INFO:
            prom_bsp_memsize_info((uint32 *)&dio.data[0], (uint32 *)&dio.data[1], (uint32 *)&dio.data[2]);
            dio.ret = RT_ERR_OK;
            break;
#endif/* #if !defined(CONFIG_SDK_EXTERNAL_CPU) */
#endif //CONFIG_SDK_KERNEL_LINUX_USER_MODE
        default:
            return -ENOTTY;
    }

    if (copy_to_user((void*)arg, &dio, sizeof(dio)))
    {
        return -EFAULT;
    }

    return RT_ERR_OK;
}

static int rtcore_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long pfn=0;
    unsigned long start = vma->vm_start;
    size_t size = vma->vm_end - vma->vm_start;
    rtcore_dev_t *dev = filp->private_data;
    uint32 cmd = vma->vm_pgoff;


    switch(cmd)
    {
        case RTCORE_MMAP_REVMEM:
            /* vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot); */
            pfn = (virt_to_phys((void *)dev->rt_data)) >> PAGE_SHIFT;
            break;
      #if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
        case RTCORE_MMAP_NICDMA:
        {
            rtcore_pci_dev_t info;
            rtcore_pci_info_get(RT_PCIDEV_NIC_DMA, &info);
            if (info.vir_base_addr != 0)
            {
                pfn = (virt_to_phys((void *)info.vir_base_addr)) >> PAGE_SHIFT;
            }
            break;
        }
      #endif/* CONFIG_SDK_DRIVER_EXTC_PCI */
        default:
            return 0;
    }

    if (remap_pfn_range(vma,
                        start,
                        pfn,
                        size,
                        vma->vm_page_prot))
    {
        return -EAGAIN;
    }
    vma->vm_private_data = filp->private_data;
    vma->vm_ops = &rtcore_remap_vm_ops;
    rtcore_vma_open(vma);

    return RT_ERR_OK;
}

#if !defined(CONFIG_SDK_EXTERNAL_CPU)
static int
rtcore_dev_irq_get(void)
{
    int ret = RT_ERR_FAILED;
    int is_upatded = 0;
    uint32 dev_idx = 0;
    uint32 dev_irq = 0;

    for (dev_idx = 0; dev_idx < RTK_DEV_MAX; dev_idx++)
    {
        ret = rtk_bspDev_irq_get(dev_idx, &dev_irq, &is_upatded);
        /* Check if the dev_idx is invalid for this BSP chip */
        if (ret == RT_ERR_INPUT)
            continue;
        if (ret != RT_ERR_OK)
            return ret;
        if (is_upatded == TRUE)
            rtk_dev[dev_idx].irq = dev_irq;
    }

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      rtcore_dev_init
 * Description:
 *      Core Driver Init
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 __init rtcore_dev_init(void)
{
    int i, ret = RT_ERR_FAILED;
    //hwp_identifier_t *force_hwp;
    dev_t devno = MKDEV(RTCORE_DRV_MAJOR, 0);
//#if defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED)
    struct page *page;
//#endif

#if !defined(CONFIG_SDK_EXTERNAL_CPU)
    ret = rtcore_dev_irq_get();
    if (ret != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, (MOD_INIT), "Get IRQ number failed\n");
        return RT_ERR_FAILED;
    }
#endif

    /* disable/enable booting message */
    {
        uint32 initMsgCfg, initErrCfg;
        uint32 initMsgEn, initErrEn;
        ioal_log_bootMsgLevel_get(&initMsgCfg, &initErrCfg);

        if ( TRUE == initMsgCfg)
            initMsgEn = ENABLED;
        else
            initMsgEn = DISABLED;

        if ( TRUE == initErrCfg)
            initErrEn = ENABLED;
        else
            initErrEn = DISABLED;

        rt_log_bootMsgEnable_set(initMsgEn, initErrEn);
    }


    /*osal_printf("RTCORE LKM Insert...\n");*/

    if (register_chrdev_region(devno, rtcore_num, RTCORE_DRV_NAME) < 0)
    {
        RT_INIT_ERR(ret, (MOD_INIT), "unable to get major %d for %s dev\n", RTCORE_DRV_MAJOR, RTCORE_DRV_NAME);
        return RT_ERR_FAILED;
    }

    rtcore_devices = osal_alloc(rtcore_num * sizeof(rtcore_dev_t));
    if (!rtcore_devices)
    {
        RT_INIT_ERR(ret, (MOD_INIT), "memory allocate fail.\n");
        return -ENOMEM;
    }

    osal_memset(rtcore_devices, 0, rtcore_num * sizeof(rtcore_dev_t));

    for (i = 0; i < rtcore_num; i++)
    {
        devno = MKDEV(RTCORE_DRV_MAJOR, 0 + i);
        cdev_init(&rtcore_devices[i].rt_cdev, &rtcore_fops);
        rtcore_devices[i].rt_cdev.owner = THIS_MODULE;
        rtcore_devices[i].rt_cdev.ops = &rtcore_fops;

        if(cdev_add(&rtcore_devices[i].rt_cdev, devno, 1))
        {
            RT_INIT_ERR(ret, (MOD_INIT), "dev %s(%d) adding error\n", RTCORE_DRV_NAME, RTCORE_DRV_MAJOR);
            unregister_chrdev_region(devno, 1);
            return RT_ERR_FAILED;
        }

        rtcore_devices[i].rt_data = (rtcore_dev_data_t *)osal_alloc(MEM_RESERVED_SIZE);

        if(!rtcore_devices[i].rt_data)
        {
            RT_INIT_ERR(ret, (MOD_INIT), "malloc fail.\n");
            unregister_chrdev_region(devno, 1);
            return RT_ERR_FAILED;
        }

        osal_memset(rtcore_devices[i].rt_data, 0, MEM_RESERVED_SIZE);

  //#if defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED)
        for(page = virt_to_page(rtcore_devices[i].rt_data); page < virt_to_page(rtcore_devices[i].rt_data + MEM_RESERVED_SIZE); page++)
            SetPageReserved(page);
  //#endif /* defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED) */

    }

    rtk_uk_sharedMem = (rtk_uk_shared_t *)rtcore_devices[RTCORE_DEV_NUM_DEFAULT].rt_data;

#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
    {
        rtcore_pci_dev_t info;

        /* PCI registration */
        ret = rtcore_pci_init();
        if (RT_ERR_OK != ret) {
            RT_INIT_ERR(ret, (MOD_INIT), "PCI device driver not installed.\n");
            return -ENODEV;
        }
        rtcore_pci_info_get(RT_PCIDEV_NIC, &info);
    }
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    /* Used by all layers, Initialize first */
    RT_ERR_CHK_EHDL(rtcore_init(), ret,
                    RT_INIT_ERR(ret, (MOD_INIT), "rtcore_init fail %d.\n", ret););
#else
    RT_ERR_CHK_EHDL(rtcore_init_userModeLKM(), ret,
                    RT_INIT_ERR(ret, (MOD_INIT), "rtcore_init fail %d.\n", ret););

    rtcoreUsr_init_status = INIT_COMPLETED;
#endif

    return ret;
}

/* Function Name:
 *      rtcore_dev_exit
 * Description:
 *      Core Driver Exit
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void __exit rtcore_dev_exit(void)
{
    int i;

#if defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED)
    struct page *page;

    for (i = 0; i < rtcore_num; i++)
    {
        for(page = virt_to_page(rtcore_devices[i].rt_data); page < virt_to_page(rtcore_devices[i].rt_data + MEM_RESERVED_SIZE); page++)
            ClearPageReserved(page);
    }
#endif /* defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED) */

#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
    if ((rtcore_pci_exit()) != RT_ERR_OK)
    {
        RT_INIT_MSG("rtcore pci exit fail\n");
    }
#endif
    for (i = 0; i < rtcore_num; i++)
        cdev_del(&rtcore_devices[i].rt_cdev);

    osal_free(rtcore_devices);
    unregister_chrdev_region(MKDEV(RTCORE_DRV_MAJOR, 0), rtcore_num);

    RT_INIT_MSG("Exit RTCORE Driver Module....OK\n");

}

module_init(rtcore_dev_init);
module_exit(rtcore_dev_exit);
module_param(rtcore_num, int, S_IRUGO);

MODULE_DESCRIPTION ("Switch SDK Core Module");
MODULE_LICENSE("GPL");

