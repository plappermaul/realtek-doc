/*
 * Realtek Semiconductor Corp.
 *
 * bsp/setup.c
 *     bsp interrult initialization and handler code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/console.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <asm/addrspace.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/reboot.h>

#include <asm/smp-ops.h>
#include <asm/mips-cm.h>
#include <asm/mips-cpc.h>
#include "bspchip.h"
#include "bspcpu.h"

#include <chip_probe.h>
#include "bsp_ioal.h"


#define SWCORE_VIRT_BASE    0xBB000000

#define RT_ERR_OK 0
#define RT_ERR_FAILED -1

#define BSP_MEM32_READ(reg)         big_endian(BSP_REG32(reg))
#define BSP_MEM32_WRITE(reg,val)    BSP_REG32(reg) = big_endian(val)


/* Function Name:
 *      bsp_mem32_read
 * Description:
 *      Get the value from register.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32
bsp_mem32_read(uint32 unit, uint32 addr, uint32 *pVal)
{
    int32 ret = RT_ERR_OK;

    /* Upper layer have check the unit, and don't need to check again */

    *pVal = BSP_MEM32_READ(SWCORE_VIRT_BASE | addr);

    return ret;
} /* end of bsp_mem32_read */


/* Function Name:
 *      bsp_mem32_write
 * Description:
 *      Set the value to register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32 bsp_mem32_write(uint32 unit, uint32 addr, uint32 val)
{
    int32 ret = RT_ERR_OK;

    BSP_MEM32_WRITE(SWCORE_VIRT_BASE | addr, val);

    return ret;
} /* end of bsp_mem32_write */

void (* hook_restart_func)(void) = NULL;

void rtk_hook_restart_function(void (*func)(void))
{
    hook_restart_func = func;
    return;
}


/*
 * Reset whole chip
 */
void
rtk_chip_reset(char *command)
{
#if defined(CONFIG_RTL9310_SERIES)
    if (bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        BSP_REG32(0xBB000400) = 0x1;
    }
    else
#endif
#if defined(CONFIG_RTL9300_SERIES)
    if (bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        BSP_REG32(0xBB00000C) = 0x1;
    }
    else
#endif
#if defined(CONFIG_RTL8390_SERIES)
    if ((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
    {
        BSP_REG32(0xBB000014) = 0xFFFFFFFF;
    }
    else
#endif
#if defined(CONFIG_RTL8380_SERIES)
    if ((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        uint32 tmp = 0;
        OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE();
        BSP_REG32(0xBB000040) = 0x1;    /* Reset Global Control Register */
    }
    else
#endif
    {

    }
}

static void bsp_machine_restart(char *command)
{
    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }


    printk("System restart.\n");
#if 1
    rtk_chip_reset(command);   /* Reset whole chip */
#else
       smp_call_function_single(0, bsp_cpc_reset, NULL,0);
#endif

}

static void bsp_machine_halt(void)
{

    printk("System halted.\n");
    while(1);


}

#ifdef CONFIG_MIPS_MT_SMTC
extern struct plat_smp_ops bsp_smtc_smp_ops;
#endif


extern void __init prom_meminit(void);

extern int bsp_serial_init(void);
/* callback function */

extern unsigned int PCI_DMA_BUS_IS_PHYS;

void __init plat_mem_setup(void)
{
    /* define io/mem region */
// 	set_io_port_base((unsigned long) KSEG1);
//  	ioport_resource.start = 0x10000000;
//  	ioport_resource.end = 0x1ffffff;
//
//  	iomem_resource.start = 0x10000000;
//  	iomem_resource.end = 0x1fffffff;
        prom_meminit();

        /* fake pci bus to avoid bounce buffers */
        PCI_DMA_BUS_IS_PHYS = 1;

         _machine_restart = bsp_machine_restart;
         _machine_halt = bsp_machine_halt;

    bsp_serial_init();

}

/* ===== Export BSP Symbol ===== */
EXPORT_SYMBOL(rtk_hook_restart_function);
EXPORT_SYMBOL(drv_swcore_ioalCB_register);
EXPORT_SYMBOL(drv_swcore_earlyInit_end);
EXPORT_SYMBOL(prom_bsp_memsize_info);
EXPORT_SYMBOL(bsp_mem32_read);
EXPORT_SYMBOL(bsp_mem32_write);

