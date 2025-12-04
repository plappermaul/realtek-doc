/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
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
 * Purpose : Realtek Switch SDK Core Module.
 *
 * Feature : Realtek Switch SDK Core Module
 *
 */

/*
 * Include Files
 */
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
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#if 0
#include <common/rt_autoconf.h>
#endif /* if 0 */
#include <common/debug/rt_log.h>
#include <common/rtcore/init.h>
#include <osal/sem.h>
#include <osal/cache.h>
#if 0
#include <drv/swcore/chip.h>
#include <drv/nic/diag.h>
#include <drv/nic/nic.h>
#endif /* if 0 */
#include <rtcore/rtcore.h>
#if 0
#include <drv/gpio/gpio.h>
#endif /* if 0 */
#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
#include <dev_config.h>
#include <rtcore/user/rtcore_drv_usr.h>
#include <drv/swcore/rtl8389.h>
#include <drv/swcore/rtl8328.h>
#include <ioal/mem32.h>
#include <osal/isr.h>
#endif /* defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */ 

/*
 * Symbol Definition
 */
#define RTCORE_DRV_MAJOR            200
#define RTCORE_DRV_NAME             "rtcore"
#define MEM_RESERVED_SIZE           4096
#define SYS_DEFAULT_INIT_UNIT_ID    0

void rtcore_vma_open(struct vm_area_struct *vma);
void rtcore_vma_close(struct vm_area_struct *vma);
static int rtcore_open(struct inode *inode, struct file *file);
static int rtcore_release(struct inode *inode, struct file *file);
#if defined(CONFIG_KERNEL_2_6_30)
static int rtcore_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
#else
static int rtcore_ioctl( struct file *file, unsigned int cmd, unsigned long arg);
#endif
static int rtcore_mmap(struct file *filp, struct vm_area_struct *vma);

/*
 * Data Declaration
 */
#if defined(CONFIG_KERNEL_2_6_30)
static const struct file_operations rtcore_fops = {
    .owner		= THIS_MODULE,     
    .open		= rtcore_open,
    .release	= rtcore_release,
    .ioctl		= rtcore_ioctl,
    .mmap       = rtcore_mmap,
};
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops rtcore_fops = {
    .proc_open           = rtcore_open,
    .proc_release        = rtcore_release,
    .proc_ioctl          = rtcore_ioctl,
    .proc_mmap           = rtcore_mmap,
};
#else
static const struct file_operations rtcore_fops = {
    .owner		= THIS_MODULE,     
    .open		= rtcore_open,
    .release	= rtcore_release,
    .unlocked_ioctl		= rtcore_ioctl,
    .mmap       = rtcore_mmap,
};
#endif
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

#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
static wait_queue_head_t sw_intr_wait_queue;
static wait_queue_head_t nic_intr_wait_queue;
static atomic_t sw_wait_for_intr;
static atomic_t nic_wait_for_intr;
static uint32 _nic_intr_enabled = DISABLED;
static uint32 _sw_intr_enabled = DISABLED;
static uint32 sw_intr_status = 0;
static uint32 nic_intr_status = 0;
static uint32 sw_sisr0 = 0;
#endif /* defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */ 
/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
osal_isrret_t
_nic_intr_handler(void *isr_param)
{
#if defined(CONFIG_SDK_RTL8389)
    ioal_mem32_write(0, RTL8389_CPU_INTERFACE_INTERRUPT_MASK_ADDR, (0x00000000U));
    
    ioal_mem32_read(0, RTL8389_CPU_INTERFACE_INTERRUPT_STATUS_ADDR, &nic_intr_status);
    ioal_mem32_write(0, RTL8389_CPU_INTERFACE_INTERRUPT_STATUS_ADDR, nic_intr_status);        
#else
    ioal_mem32_write(0, RTL8328_CPUIIMR_ADDR, (0x00000000U));

    ioal_mem32_read(0, RTL8328_CPUIISR_ADDR, &nic_intr_status);
    ioal_mem32_write(0, RTL8328_CPUIISR_ADDR, nic_intr_status);        
#endif

    if (atomic_dec_return(&nic_wait_for_intr) >= 0) 
    {
        wake_up_interruptible(&nic_intr_wait_queue);
    }
    return OSAL_INT_HANDLED;
} /* end of _nic_isr_handler */ 

osal_isrret_t
_sw_intr_handler(void *isr_param)
{  
#if defined(CONFIG_SDK_RTL8389)
    ioal_mem32_read(0, RTL8389_SWITCH_INTERRUPT_SOURCE_STATUS_ADDR, &sw_intr_status);
    ioal_mem32_write(0, RTL8389_SWITCH_INTERRUPT_CONTROL0_ADDR, (0x00000000U));
    
    ioal_mem32_read(0, RTL8389_SWITCH_INTERRUPT_STATUS0_ADDR, &sw_sisr0);
	ioal_mem32_write(0, RTL8389_SWITCH_INTERRUPT_STATUS0_ADDR, sw_sisr0);
#else
    ioal_mem32_read(0, RTL8328_SWITCH_INTERRUPT_GLOBAL_SOURCE_STATUS_ADDR, &sw_intr_status);
    ioal_mem32_write(0, RTL8328_PER_PORT_LINK_CHANGE_INTERRUPT_CONTROL_ADDR, (0x00000000U));

    ioal_mem32_read(0, RTL8328_PER_PORT_LINK_CHANGE_INTERRUPT_STATUS_ADDR, &sw_sisr0);
	ioal_mem32_write(0, RTL8328_PER_PORT_LINK_CHANGE_INTERRUPT_STATUS_ADDR, sw_sisr0);
#endif

    if (atomic_dec_return(&sw_wait_for_intr) >= 0) 
    {
        wake_up_interruptible(&sw_intr_wait_queue);
    }

    return OSAL_INT_HANDLED;
} /* end of _nic_isr_handler */ 
#endif /* defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */ 

 
static int rtcore_open(struct inode *inode, struct file *file)
{   
    rtcore_dev_t *dev;

    dev = container_of(inode->i_cdev, rtcore_dev_t, rt_cdev);
    dev->rt_data = rtcore_devices[SYS_DEFAULT_INIT_UNIT_ID].rt_data;
    file->private_data = dev;

    return RT_ERR_OK;           
}

static int rtcore_release(struct inode *inode, struct file *file)
{ 
    return RT_ERR_OK;       
}

void rtcore_vma_open(struct vm_area_struct *vma)
{
	printk(KERN_DEBUG "VMA Open, Virt %lx, Phys %lx\n",
			vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void rtcore_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_DEBUG "VMA Close\n");
}

#if defined(CONFIG_KERNEL_2_6_30)
static int rtcore_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static int rtcore_ioctl( struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    rtcore_ioctl_t dio;
#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
    uint32 flags;
#endif

    if (copy_from_user(&dio, (void*)arg, sizeof(dio)))
    {
        return -EFAULT;
    }

    switch (cmd)
    {    
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        case RTCORE_CID_GET:
            dio.ret = drv_swcore_cid_get(dio.data[0], &dio.data[1], &dio.data[2]);
            break;

        case RTCORE_CID_CMP:
            dio.ret = drv_swcore_cid_cmp(dio.data[0], dio.data[1]);
            break;    

        case RTCORE_CACHE_FLUSH:
            dio.ret = osal_cache_memory_flush(dio.data[0], dio.data[1]);
            break;         
#if defined(CONFIG_SDK_OSAL_SEM_KERNEL)
        case RTCORE_SEM_CREATE:
            dio.ret = osal_sem_create(dio.data[0]);
            break;
            
        case RTCORE_SEM_DESTROY:
            osal_sem_destroy(dio.data[0]);
            return RT_ERR_OK;
            
        case RTCORE_SEM_TAKE:
            dio.ret = osal_sem_take(dio.data[0], dio.data[1]);
            break;
            
        case RTCORE_SEM_GIVE:
            dio.ret = osal_sem_give(dio.data[0]);
            break;
#endif /* defined(CONFIG_SDK_OSAL_SEM_KERNEL) */

#if defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE)
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
            dio.ret = drv_nic_pkthdr_mbuf_dump(dio.data[0], dio.data[1], dio.data[2], dio.data[3], dio.data[4]);
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
#endif /* defined(CONFIG_SDK_DRIVER_NIC_KERNEL_MODE) */ 
#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
        case RTCORE_INTR_ENABLE_SET:
            {
                local_irq_save(flags);                                
                
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
                local_irq_restore(flags);
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
                }
                else
                {                
            		if (sw_wait_for_intr.counter <= 0)
            	    {
            			atomic_inc(&sw_wait_for_intr);
            		}
            
                    wait_event_interruptible(sw_intr_wait_queue, atomic_read(&sw_wait_for_intr) <= 0);
                   /*                      
                    * only run the interrupt handler once.
                    */
                    atomic_set(&sw_wait_for_intr, 0);
                    dio.data[2] = sw_intr_status;
                    dio.data[3] = sw_sisr0;
                }                    
            }
            break;
#endif /* defined(CONFIG_SDK_DRIVER_NIC_USER_MODE) */ 
        case RTCORE_GPIO_DATABIT_GET:
            dio.ret = drv_gpio_dataBit_get(dio.data[0], &dio.data[1]);
            break;
        case RTCORE_GPIO_DATABIT_SET:
            dio.ret = drv_gpio_dataBit_set(dio.data[0], dio.data[1]);
            break;
        case RTCORE_GPIO_INIT:
            dio.ret = drv_gpio_init(dio.data[0], dio.data[1], dio.data[2], dio.data[3]);
            break;
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */  

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
        return -EAGAIN;         

    vma->vm_private_data = filp->private_data;    
    vma->vm_ops = &rtcore_remap_vm_ops;
    rtcore_vma_open(vma);
    
    return RT_ERR_OK;   
}

/* Function Name:
 *      rtcore_dev_setup
 * Description:
 *      Seteup Core Device 
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
static int32 rtcore_dev_setup(uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)

    int32 ret = RT_ERR_FAILED;    

    rtcore_dev_data_t *data = rtcore_devices[unit].rt_data;

    RT_ERR_HDL(rt_log_format_get(&data->log_format), setup_end, ret);
    RT_ERR_HDL(rt_log_level_get(&data->log_level), setup_end, ret);
    RT_ERR_HDL(rt_log_mask_get(&data->log_mask), setup_end, ret);
    RT_ERR_HDL(rt_log_moduleMask_get(&data->log_module_mask), setup_end, ret);
    RT_ERR_HDL(rt_log_type_get(&data->log_type), setup_end, ret);

    /* save the backup value */
    data->log_level_bak = data->log_level;
    data->log_mask_bak = data->log_mask;

setup_end:
    return ret;     

#else
    return RT_ERR_OK;

#endif 
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
    dev_t devno = MKDEV(RTCORE_DRV_MAJOR, 0);
    
#if defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED)
    struct page *page;     
#endif

    osal_printf("Init xPON/Switch Core Dev Module....");

    if (register_chrdev_region(devno, rtcore_num, RTCORE_DRV_NAME) < 0)
    {
		printk("unable to get major %d for %s dev\n", RTCORE_DRV_MAJOR, RTCORE_DRV_NAME);
		return RT_ERR_FAILED;
	}

    rtcore_devices = kmalloc(rtcore_num * sizeof(rtcore_dev_t), GFP_KERNEL);
    if (!rtcore_devices)
        return -ENOMEM;
    
    memset(rtcore_devices, 0, rtcore_num * sizeof(rtcore_dev_t));

    for (i = 0; i < rtcore_num; i++)
    {       
        devno = MKDEV(RTCORE_DRV_MAJOR, 0 + i);
        cdev_init(&rtcore_devices[i].rt_cdev, &rtcore_fops);
	    rtcore_devices[i].rt_cdev.owner = THIS_MODULE;
	    rtcore_devices[i].rt_cdev.ops = &rtcore_fops;

        if(cdev_add(&rtcore_devices[i].rt_cdev, devno, 1))
	    {
	        printk (KERN_NOTICE "**Error- dev %s(%d) adding error", RTCORE_DRV_NAME, RTCORE_DRV_MAJOR);
	        unregister_chrdev_region(devno, 1);
            return RT_ERR_FAILED;
        }

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        rtcore_devices[i].rt_data = (rtcore_dev_data_t *)kmalloc(MEM_RESERVED_SIZE, GFP_KERNEL);
        if(!rtcore_devices[i].rt_data)
        {
   	        unregister_chrdev_region(devno, 1);
            return RT_ERR_FAILED;
        }

        memset(rtcore_devices[i].rt_data, 0, MEM_RESERVED_SIZE);

#if defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED)
        for(page = virt_to_page(rtcore_devices[i].rt_data); page < virt_to_page(rtcore_devices[i].rt_data + MEM_RESERVED_SIZE); page++)
            SetPageReserved(page);
#endif /* defined(CONFIG_SDK_DRIVER_RTCORE_PAGE_RESERVED) */

#endif /* defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) */
    }

    /* Used by all layers, Initialize first */
    RT_ERR_CHK(rtcore_init(SYS_DEFAULT_INIT_UNIT_ID), ret);

    /* Setup the device */
    RT_ERR_CHK(rtcore_dev_setup(SYS_DEFAULT_INIT_UNIT_ID), ret);

    osal_printf("OK\n");

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

    kfree(rtcore_devices);
    unregister_chrdev_region(MKDEV(RTCORE_DRV_MAJOR, 0), rtcore_num);   
    
#if defined(CONFIG_SDK_DRIVER_RTCORE_MODULE)	
    printk("Exit RTCORE Driver Module....OK\n");
#endif
}

module_init(rtcore_dev_init);
module_exit(rtcore_dev_exit);
module_param(rtcore_num, int, S_IRUGO);

MODULE_DESCRIPTION ("Switch SDK Core Module");
MODULE_LICENSE("GPL");
