/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97155 $
 * $Date: 2019-11-10 14:22:30 +0800 (Sun, 10 Nov 2019) $
 *
 * Purpose : bsp early initialization code
 *
 */

#include <common/rt_type.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include <asm/addrspace.h>
#include <asm/bootinfo.h>
#include <asm/prom.h>

#include <asm/fw/fw.h>

#include <bspchip.h>
#include "rtk_bsp_driver.h"

const char *get_system_type(void)
{
    return "Management Switch";
}

void __init prom_free_prom_memory(void)
{
    return;
}

extern void plat_smp_init(void);
extern void early_uart_init(void);
extern void drv_swcore_earlyInit_start(void);


/* Function Name:
 *      prom_bsp_memsize_info
 * Description:
 *      Get Low/High/RTK_DMA size
 * Input:
 *      None
 * Output:
 *      low_memory_size     - Low Memory Size
 *      high_memory_size    - High Memory Size
 *      dma_reserved_size   - RTK DMA size
 * Return:
 *      None
 * Note:
 *
 */
void prom_bsp_memsize_info(unsigned int *low_memory_size, unsigned int *high_memory_size, unsigned int *dma_reserved_size)
{
    rtk_bspMemSizeInfo_get(low_memory_size, high_memory_size, dma_reserved_size);
}


/* Do basic initialization */
void __init prom_init(void)
{
    int32  ret;

    /* RTK SDK earlyInit code */
    drv_swcore_earlyInit_start();

    ret = rtk_bspDriver_init();
    if(ret != SDK_BSP_OK)
        return;

    rtk_bspL2Cache_init();
    rtk_bspConsole_init(RTK_EARLY_PRINTK_BAUDRATE);

    fw_init_cmdline();

#ifdef CONFIG_SERIAL_8250_CONSOLE
    if (!strstr(arcs_cmdline, "console=")) {
        strlcat(arcs_cmdline, " console=ttyS0,115200",
            COMMAND_LINE_SIZE);
    }
#endif

    mips_set_machine_name("Realtek Management Switch");

#ifdef CONFIG_EARLY_PRINTK
    early_uart_init();
#endif

#ifdef CONFIG_SMP
    plat_smp_init();
#endif

}
