//#include <configs/autoconf.h>
#include <cross_env.h>
//#include <asm/otto_timer.h>

#if defined(CONFIG_UNDER_UBOOT) /* Add by Management Switch Projetc*/
#include <linux/delay.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif

#define	__DEVICE_REASSIGN 0	/* Management Switch Project doesn't use NSU_USING_SYMBOL_TABLE_FUNCTION */

#else
#define SPI_NAND_SEM_LOCK()
#define SPI_NAND_SEM_UNLOCK()

#define	__DEVICE_REASSIGN 0	/* Management Switch Project doesn't use NSU_USING_SYMBOL_TABLE_FUNCTION */
#endif /* Add by Switch Management Projetc*/


#define SECTION_SPI_NAND_PROBE_FUNC __attribute__ ((section (".spi_nand_probe_func")))

#define REG_SPI_NAND_PROBE_FUNC(fn) spi_nand_probe_t* __nspf_ ## ## fn ## _ \
        SECTION_SPI_NAND_PROBE_FUNC = (spi_nand_probe_t*) fn

#define _plr_soc_t              (*(soc_t *)(OTTO_SRAM_START+OTTO_HEADER_OFFSET))
#define _lplr_basic_io          _plr_soc_t.bios

#ifndef inline_memcpy
#define inline_memcpy           memcpy
#endif

#define otto_lx_timer_udelay    udelay
