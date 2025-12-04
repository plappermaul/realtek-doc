/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include <stdarg.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>

#include "pltfm_cb.h"
#include "halmac/type.h"
#include "../../../../base/pcie/dw_4_60a/dw_pcie_base.h"

extern int printf(const char* fmt, ...);
extern void udelay(unsigned long);
extern void mdelay(unsigned long);

static u8 _pltfm_cb_reg_r8(void *drv_adapter, u32 addr)
{
    return HalPCIeEPMemRead8(PCIE_MODULE_SEL_0, addr);
}

static u16 _pltfm_cb_reg_r16(void *drv_adapter, u32 addr)
{
    return HalPCIeEPMemRead16(PCIE_MODULE_SEL_0, addr);
}

static u32 _pltfm_cb_reg_r32(void *drv_adapter, u32 addr)
{
    return HalPCIeEPMemRead32(PCIE_MODULE_SEL_0, addr);
}

static void _pltfm_cb_reg_w8(void *drv_adapter, u32 addr, u8 value)
{
    HalPCIeEPMemWrite8(PCIE_MODULE_SEL_0, addr, value);
}

static void _pltfm_cb_reg_w16(void *drv_adapter, u32 addr, u16 value)
{
    HalPCIeEPMemWrite16(PCIE_MODULE_SEL_0, addr, value);
}

static void _pltfm_cb_reg_w32(void *drv_adapter, u32 addr, u32 value)
{
    HalPCIeEPMemWrite32(PCIE_MODULE_SEL_0, addr, value);
}

static u32 _pltfm_cb_tx(void *drv_adapter, u8 *buf, u32 len)
{
    printf("_pltfm_cb_tx is called, not implement now!n");
	return 0;
}

static void _pltfm_cb_free(void *drv_adapter, void *buf, u32 size)
{
	free(buf);
}

static void* _pltfm_cb_malloc(void *drv_adapter, u32 size)
{
	return malloc(size);
}

static void _pltfm_cb_memcpy(void *drv_adapter, void *dest, void *src, u32 size)
{
	memcpy(dest, src, size);
}

static void _pltfm_cb_memset(void *drv_adapter, void *addr, u8 value, u32 size)
{
	memset(addr, value, size);
}

static u32 _pltfm_cb_memcmp(void *drv_adapter, void *ptr1, void *ptr2, u32 num)
{
	return (u32)memcmp(ptr1, ptr2, num);
}

static void _pltfm_cb_delay_us(void *drv_adapter, u32 us)
{
    udelay(us);
}

static void _pltfm_cb_delay_ms(void *drv_adapter, u32 ms)
{
    mdelay(ms);
}

static void _pltfm_cb_mutex_init(void *drv_adapter, mac_ax_mutex *mutex)
{
    //printf("_pltfm_cb_mutex_init is called, not implement now!n");
}

static void _pltfm_cb_mutex_deinit(void *drv_adapter, mac_ax_mutex *mutex)
{
    //printf("_pltfm_cb_mutex_deinit is called, not implement now!n");
}

static void _pltfm_cb_mutex_lock(void *drv_adapter, mac_ax_mutex *mutex)
{
    printf("_pltfm_cb_mutex_lock is called, not implement now!n");
}

static void _pltfm_cb_mutex_unlock(void *drv_adapter, mac_ax_mutex *mutex)
{
    printf("_pltfm_cb_mutex_unlock is called, not implement now!n");
}

static void _pltfm_cb_msg_print(void *drv_adapter, s8 *fmt, ...)
{
    va_list args;
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);

	/*
	 * For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);
	va_end(args);

	/* Print the string */
	puts(printbuffer);
}

struct mac_ax_pltfm_cb pltfm_cb = {
#if MAC_AX_SDIO_SUPPORT
	_pltfm_cb_cmd52_r8, /* sdio_cmd52_r8 */
	_pltfm_cb_cmd53_r8, /* sdio_cmd53_r8 */
	_pltfm_cb_cmd53_r16, /* sdio_cmd53_r16 */
	_pltfm_cb_cmd53_r32, /* sdio_cmd53_r32 */
	_pltfm_cb_cmd53_rn, /* sdio_cmd53_rn */
	_pltfm_cb_cmd52_w8, /* sdio_cmd52_w8 */
	_pltfm_cb_cmd53_w8, /* sdio_cmd53_w8 */
	_pltfm_cb_cmd53_w16, /* sdio_cmd53_w16 */
	_pltfm_cb_cmd53_w32, /* sdio_cmd53_w32 */
	_pltfm_cb_cia_cmd52_r8, /* sdio_cmd52_cia_r8 */
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	_pltfm_cb_reg_r8, /* reg_r8 */
	_pltfm_cb_reg_r16, /* reg_r16 */
	_pltfm_cb_reg_r32, /* reg_r32 */
	_pltfm_cb_reg_w8, /* reg_w8 */
	_pltfm_cb_reg_w16, /* reg_w16 */
	_pltfm_cb_reg_w32, /* reg_w32 */
#endif
	_pltfm_cb_tx, /* tx */
	_pltfm_cb_free, /* rtl_free */
	_pltfm_cb_malloc, /* rtl_malloc */
	_pltfm_cb_memcpy, /* rtl_memcpy */
	_pltfm_cb_memset, /* rtl_memset */
	_pltfm_cb_memcmp, /* rtl_memcmp */
	_pltfm_cb_delay_us, /* rtl_delay_us */
	_pltfm_cb_delay_ms, /* rtl_delay_ms */
	_pltfm_cb_mutex_init, /* mutex_init */
	_pltfm_cb_mutex_deinit, /* mutex_deinit */
	_pltfm_cb_mutex_lock, /* mutex_lock */
	_pltfm_cb_mutex_unlock, /* mutex_unlock */
	_pltfm_cb_msg_print, /* msg_print */
};

