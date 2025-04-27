/*
 * Realtek Semiconductor Corp.
 *
 * bsp/setup.c
 *     bsp interrult initialization and handler code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */

#include <common/rt_type.h>
#include <linux/export.h>
#include <linux/types.h>
#include "chip_probe.h"
#include "prom.h"
#include "rtk_bsp_driver.h"

#define SWCORE_VIRT_BASE    0xBB000000

#define RT_ERR_OK 0
#define RT_ERR_FAILED -1

/*
 * Register access macro
 */
#ifndef REG32
#define REG32(reg)		(*(volatile unsigned int   *)(reg))
#endif

#if defined(__little_endian__)
#define big_endian32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))
#define big_endian16(x) (((x) >> 8) | (((x) & 0x000000ff) << 8))
#else
#define big_endian32(x) (x)
#define big_endian16(x) (x)
#endif
#define big_endian(x) big_endian32(x) /* backward-compatible */

#define BSP_MEM32_READ(reg)         big_endian(REG32(reg))
#define BSP_MEM32_WRITE(reg,val)    REG32(reg) = big_endian(val)

static int rtk_watchdog_default_func(void);

void (* hook_restart_func)(void) = NULL;
int (*rtk_watchdog_kick_func)(void) = rtk_watchdog_default_func;

static int rtk_watchdog_default_func(void)
{
    return 0;
}

/* Function Name:
 *      rtk_watchdog_kick
 * Description:
 *      Execute kick watchdog function.
 * Input:
 *      N/A
 * Output:
 *      None
 * Return:
 *      N/A
 * Note:
 *      N/A
 */
int rtk_watchdog_kick(void)
{
    int ret = 0;
    ret = rtk_watchdog_kick_func();
    return ret;
}


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

/* Function Name:
 *      rtk_hook_restart_function
 * Description:
 *      Provide the hook function to kernel, and this function
 *      could be executed before chip reset.
 * Input:
 *      func - callback function pointer
 * Output:
 *      None
 * Return:
 *      N/A
 * Note:
 *      N/A
 */
void rtk_hook_restart_function(void (*func)(void))
{
    hook_restart_func = func;
    return;
}

/* Function Name:
 *      rtk_restartCallBack_execute
 * Description:
 *      Execute hook function if this pointer is hook.
 * Input:
 *      N/A
 * Output:
 *      None
 * Return:
 *      N/A
 * Note:
 *      N/A
 */
void rtk_restartCallBack_execute(void)
{
    if(hook_restart_func != NULL)
    {
        hook_restart_func();
    }
}


/* ===== Export BSP Symbol ===== */
EXPORT_SYMBOL(bsp_mem32_read);
EXPORT_SYMBOL(bsp_mem32_write);
EXPORT_SYMBOL(drv_swcore_ioalCB_register);
EXPORT_SYMBOL(drv_swcore_earlyInit_end);
EXPORT_SYMBOL(rtk_bspDev_irq_get);
EXPORT_SYMBOL(rtk_bspUSB_irq_get);
EXPORT_SYMBOL(prom_bsp_memsize_info);
EXPORT_SYMBOL(rtk_hook_restart_function);
EXPORT_SYMBOL(rtk_watchdog_kick_func);


