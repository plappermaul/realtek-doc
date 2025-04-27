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
 * $Revision: 96729 $
 * $Date: 2019-05-08 14:03:36 +0800 (Wed, 08 May 2019) $
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
#include <osal/sem.h>
#include <osal/cache.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/nic/nic.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <drv/gpio/gpio.h>
#if defined(CONFIG_SDK_DRIVER_I2C)
#include <drv/i2c/i2c.h>
#endif
#include <drv/watchdog/watchdog.h>
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
  #include <dev_config.h>
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
#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */
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


/*
 * Symbol Definition
 */
#define RTCORE_DRV_MAJOR            200
#define RTCORE_DRV_NAME             "rtcore"
#define SYS_DEFAULT_DEVICE_ID       0

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

/* SWCORE SW scan time */
#define SWCORE_SWSCAN_TIME   msecs_to_jiffies(1000)

extern void prom_bsp_memsize_info(unsigned int *low_memory_size, unsigned int *high_memory_size, unsigned int *dma_reserved_size);

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

#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
static wait_queue_head_t sw_intr_wait_queue;
static wait_queue_head_t nic_intr_wait_queue;
static atomic_t sw_wait_for_intr;
static atomic_t nic_wait_for_intr;
static uint32 _nic_intr_enabled = DISABLED;
static uint32 _sw_intr_enabled = DISABLED;
static uint32 sw_intr_status = 0;
static uint32 nic_intr_status = 0;
static uint32 sw_sisr0 = 0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
static uint32 sw_sisr1 = 0;
#endif
uint32  l2NotifyEventCnt = 0;
uint32  curPos = 0;

extern uint32 totalEventCount;
#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */
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
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
void
_sw_portLinkChangeIMR_set(rtk_enable_t enable)
{
    uint32 unit;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
#if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            if(enable == ENABLED)
            {
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xffffffff);
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(32), 0xffffffff);
            }else{
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(0), 0x0);
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x0);
            }
        }
#endif
#if defined(CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
        {
            if(enable == ENABLED)
            {
                ioal_mem32_write(unit, RTL8380_IMR_PORT_LINK_STS_CHG_ADDR, 0xffffffff);
            }else{
                ioal_mem32_write(unit, RTL8380_IMR_PORT_LINK_STS_CHG_ADDR, 0x0);
            }
        }
#endif
#if defined(CONFIG_SDK_RTL9300)
        if (HWP_9300_FAMILY_ID(unit))
        {
            if(enable == ENABLED)
            {
                ioal_mem32_write(unit, RTL9300_IMR_PORT_LINK_STS_CHG_ADDR, 0xffffffff);
            }else{
                ioal_mem32_write(unit, RTL9300_IMR_PORT_LINK_STS_CHG_ADDR, 0x0);
            }
        }
#endif
#if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            if(enable == ENABLED)
            {
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xffffffff);
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x00ffffff);
            }else{
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(0), 0x0);
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x0);
            }
        }
#endif

    }
} /* end of _sw_linkChange_enable_set */

uint32
_sw_portLinkChange_status_get(void)
{
    uint32 unit;
    uint32 ret = 0;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
#if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            ioal_mem32_read(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(0),  &sw_sisr0);
            ioal_mem32_read(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(32), &sw_sisr1);
            ioal_mem32_write(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(0), sw_sisr0);
            ioal_mem32_write(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(32), sw_sisr1);
            ret = (sw_sisr0 | sw_sisr1);
        }
#endif
#if defined(CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
        {
            ioal_mem32_read(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, &sw_sisr0);
            /* clear status */
            ioal_mem32_write(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, sw_sisr0);
            ret = sw_sisr0;
        }
#endif
#if defined(CONFIG_SDK_RTL9300)
        if (HWP_9300_FAMILY_ID(unit))
        {
            ioal_mem32_read(unit, RTL9300_ISR_PORT_LINK_STS_CHG_ADDR, &sw_sisr0);
            /* clear status */
            ioal_mem32_write(unit, RTL9300_ISR_PORT_LINK_STS_CHG_ADDR, sw_sisr0);
            ret = sw_sisr0;
        }
#endif
#if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(0), &sw_sisr0);
            ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(32), &sw_sisr1);
            ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(0), sw_sisr0);
            ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(32), sw_sisr1);
            ret = (sw_sisr0 | sw_sisr1);
        }
#endif

    }

    return ret;
} /* end of _sw_intr_handler */


osal_isrret_t
_nic_intr_handler(void *isr_param)
{
    uint32 unit=HWP_MY_UNIT_ID();

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        ioal_mem32_write(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, (0x00000000U));
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
    {
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, (0x00000000U));
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_DONE_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, (0x00000000U));
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_DONE_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, (0x00000000U));
        ioal_mem32_write(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, (0x00000000U));
    }
#endif

    if (atomic_dec_return(&nic_wait_for_intr) >= 0)
    {
        wake_up_interruptible(&nic_intr_wait_queue);
    }
    return OSAL_INT_HANDLED;
} /* end of _nic_isr_handler */

#define SWCORE_INT_OCCUR    0
#define SWCORE_INT_CLEAR    1

#define SWCORE_INT_DBG(_fmt, args...)           //osal_printf(_fmt, ##args)
static int swcore_dbg_int_cnt = 0;
#if defined(CONFIG_SDK_LINKMON_MIXED_MODE)
static int swcore_dbg_wake_cnt_poll = 0;
#endif
static int swcore_dbg_wake_cnt_int = 0;
static int swcore_dbg_wake_cnt_timeoutWithIntEvent = 0;


osal_isrret_t
_sw_intr_handler(void *isr_param)
{
    uint32 unit;

    atomic_set(&sw_wait_for_intr, SWCORE_INT_OCCUR);
    HWP_UNIT_TRAVS_LOCAL(unit)
    {
#if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            ioal_mem32_read(unit, RTL8390_ISR_GLB_SRC_ADDR, &sw_intr_status);
        }
#endif
#if defined(CONFIG_SDK_RTL8380)
        if (HWP_8380_30_FAMILY(unit))
        {
            ioal_mem32_read(unit, RTL8380_ISR_GLB_SRC_ADDR, &sw_intr_status);
        }
#endif
#if defined(CONFIG_SDK_RTL9300)
        if (HWP_9300_FAMILY_ID(unit))
        {
            ioal_mem32_read(unit, RTL9300_ISR_GLB_ADDR, &sw_intr_status);
        }
#endif
#if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            ioal_mem32_read(unit, RTL9310_ISR_GLB_SRC_STS_ADDR, &sw_intr_status);
        }
#endif

    }
    swcore_dbg_int_cnt++;
    _sw_portLinkChangeIMR_set(DISABLED);
    wake_up_interruptible(&sw_intr_wait_queue);

    return OSAL_INT_HANDLED;
} /* end of _sw_intr_handler */
#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */


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
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
    long wait_ret;
#endif

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
            dio.ret = drv_nic_ringbuf_dump(dio.data[0]);
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
#if defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
        case RTCORE_INTR_ENABLE_SET:
            {
                if (INTR_TYPE_NIC == (dio.data[2]))
                {
                    if (ENABLED == dio.data[1])
                    {
                        init_waitqueue_head(&nic_intr_wait_queue);
                        if(DISABLED == _nic_intr_enabled)
                        {
                            dio.ret = osal_isr_register(RTK_DEV_NIC, _nic_intr_handler, NULL);
                            _nic_intr_enabled = ENABLED;
                        }
                    }
                    else
                    {
                        if(ENABLED == _nic_intr_enabled)
                        {
                            dio.ret = osal_isr_unregister(RTK_DEV_NIC);
                            _nic_intr_enabled = DISABLED;
                        }
                    }
                }
                else
                {
                    if (ENABLED == dio.data[1])
                    {
                        init_waitqueue_head(&sw_intr_wait_queue);
                        if(DISABLED == _sw_intr_enabled)
                        {
                            dio.ret = osal_isr_register(RTK_DEV_SWCORE, _sw_intr_handler, NULL);
                            _sw_intr_enabled = ENABLED;
                        }
                    }
                    else
                    {
                        if(ENABLED == _sw_intr_enabled)
                        {
                            dio.ret = osal_isr_unregister(RTK_DEV_SWCORE);
                            _sw_intr_enabled = DISABLED;
                        }
                    }
                }
            }
            break;
        case RTCORE_INTR_WAIT:
            {
                if (INTR_TYPE_NIC == (dio.data[1]))
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
                else
                {
                    int32 linkChangeHasIntEvent=0;
                    atomic_set(&sw_wait_for_intr, SWCORE_INT_CLEAR);
                    wait_ret = 0;
#if defined(CONFIG_SDK_LINKMON_ISR_MODE)
                    wait_ret = wait_event_interruptible(sw_intr_wait_queue, (atomic_read(&sw_wait_for_intr) == SWCORE_INT_OCCUR));
                    linkChangeHasIntEvent = _sw_portLinkChange_status_get();
#endif
#if defined(CONFIG_SDK_LINKMON_MIXED_MODE)
                    wait_ret = wait_event_interruptible_timeout(sw_intr_wait_queue, (atomic_read(&sw_wait_for_intr) == SWCORE_INT_OCCUR), SWCORE_SWSCAN_TIME);
                    linkChangeHasIntEvent = _sw_portLinkChange_status_get();
                    if( (wait_ret == 0) && (linkChangeHasIntEvent == 0) )
                    {
                        swcore_dbg_wake_cnt_poll++;
                        /*execute mix-mode sw swscan*/
                        /* dio.data[7] = swScan flag = 1*/
                        dio.data[7] = 1;
                        dio.data[2] = sw_intr_status;
                        dio.data[3] = 0;
                    }
                    else
#endif
                    {
                   /*
                    * only run the interrupt handler once.
                    */
                        swcore_dbg_wake_cnt_int++;
                        if( (wait_ret == 0) && (linkChangeHasIntEvent) )
                            swcore_dbg_wake_cnt_timeoutWithIntEvent++;
                        SWCORE_INT_DBG("swcoreLnkChg_intMode\n");
                        dio.data[2] = sw_intr_status;
#if defined(CONFIG_SDK_LINKMON_MIXED_MODE)
                        /* dio.data[7] = swScan flag = 0*/
                        dio.data[7] = 0;
#endif
                        dio.data[3] = sw_sisr0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
                        if (HWP_8390_50_FAMILY(dio.data[0]))
                        {
                            dio.data[4] = sw_sisr1;
                        }
                        if (HWP_9310_FAMILY_ID(dio.data[0]))
                        {
                            dio.data[4] = sw_sisr1;
                        }
#endif
#if defined(CONFIG_SDK_LINKMON_MIXED_MODE)
                        SWCORE_INT_DBG("swcoreIntHandlerWake: intCnt=%d, wakeCntInt=%d, wakeCntPoll=%d, timeoutWithInt=%d ",swcore_dbg_int_cnt, swcore_dbg_wake_cnt_int, swcore_dbg_wake_cnt_poll, swcore_dbg_wake_cnt_timeoutWithIntEvent);
#else
                        SWCORE_INT_DBG("swcoreIntHandlerWake: intCnt=%d, wakeCntInt=%d, timeoutWithInt=%d ",swcore_dbg_int_cnt, swcore_dbg_wake_cnt_int, swcore_dbg_wake_cnt_timeoutWithIntEvent);
#endif
                        SWCORE_INT_DBG("sw_sisr0=%x", sw_sisr0);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
                        SWCORE_INT_DBG(", sw_sisr1=%x, ", sw_sisr1);
#endif
                        SWCORE_INT_DBG("\n");
                    }
                    _sw_portLinkChangeIMR_set(ENABLED);
                }
            }
            break;
        case RTCORE_REG_READ:
            dio.data[2] = MEM32_READ(dio.data[1]);
            //debug_mem_read(dio.data[0], dio.data[1], &dio.data[2]);
            break;

        case RTCORE_REG_WRITE:
            MEM32_WRITE(dio.data[1], dio.data[2]);
            //debug_mem_write(dio.data[0], dio.data[1], dio.data[2]);
            break;

#endif /* defined(CONFIG_SDK_DRIVER_NIC) && defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)


#ifdef CONFIG_SDK_DRIVER_GPIO
        case RTCORE_GPIO_DATABIT_GET:
            dio.ret = drv_gpio_dataBit_get(dio.data[2], dio.data[0], &dio.data[1]);
            break;
        case RTCORE_GPIO_DATABIT_SET:
            dio.ret = drv_gpio_dataBit_set(dio.data[2], dio.data[0], dio.data[1]);
            break;
        case RTCORE_GPIO_INIT:
            dio.ret = drv_gpio_pin_init(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
        case RTCORE_GPIO_DATABIT_INIT:
            dio.ret = drv_gpio_dataBit_init(dio.data[0], dio.data[1]);
            break;
#endif /* CONFIG_SDK_DRIVER_GPIO */
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
        case RTCORE_MEM_SIZE_INFO:
            prom_bsp_memsize_info((uint32 *)&dio.data[0], (uint32 *)&dio.data[1], (uint32 *)&dio.data[2]);
            dio.ret = RT_ERR_OK;
            break;
#endif //CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
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
    unsigned long pfn;
    unsigned long start = vma->vm_start;
    size_t size = vma->vm_end - vma->vm_start;
    rtcore_dev_t *dev = filp->private_data;

    /* vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot); */
    pfn = (virt_to_phys((void *)dev->rt_data)) >> PAGE_SHIFT;

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


    osal_printf("RTCORE LKM Insert...\n");

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

osal_printf("\nallocate mem to rtcore_devices[%d].rt_data=0x%x\n",i,(uint32)rtcore_devices[i].rt_data);
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

