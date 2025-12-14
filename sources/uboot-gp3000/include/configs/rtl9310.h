/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 */

#ifndef _RTL9310_CONFIG_H
#define _RTL9310_CONFIG_H

/* #define DEBUG */
#include <soc.h>

/*
 * System configuration
 */
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_SKIP_LOWLEVEL_INIT

/*
 * CPU Configuration
 */
#ifdef CONFIG_USE_EXT_TIMER
	#define CONFIG_SYS_MHZ (GET_LX_MHZ())
#else
	#define CONFIG_SYS_MHZ (GET_CPU_MHZ()) /* arbitrary value */
#endif
#define CONFIG_SYS_MIPS_TIMER_FREQ (CONFIG_SYS_MHZ * 1000000 >> 1)
#define CONFIG_MIPS

/*
 * Memory map
 */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_SDRAM_BASE		(0x80000000)
#define CONFIG_SYS_INIT_SP_OFFSET	(0x400000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x01000000)
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED   	(256 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(512 * 1024)
#define CONFIG_SYS_BOOTM_LEN		( 64 * 1024 * 1024)
/*
 * Serial driver
 */
#define CONFIG_SYS_NS16550_PORT_MAPPED
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_COM1		(0xB8002000)
#define CONFIG_SYS_NS16550_CLK ((GET_LX_MHZ())*1000*1000)

/*
 * Flash configuration
 */
#define CONFIG_SYS_FLASH_BASE 		0x94000000
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#ifdef CONFIG_OTTO_SNOF
#define CONFIG_SF_DEFAULT_CS	(0)
#define CONFIG_SF_DEFAULT_BUS	(0)
#define CONFIG_SF_DEFAULT_MODE	(0)
#define CONFIG_SF_DEFAULT_SPEED (1000000)
#endif

/*
 * NAND Flash configuration
 */
#define CONFIG_SYS_NAND_SELF_INIT 1
#define CONFIG_SYS_MAX_NAND_DEVICE 2

#ifdef CONFIG_CMD_JFFS2
#	define CONFIG_JFFS2_NAND 1
#endif

#define SNAF_PLANE_PATCH   (0)
/*
 * Environment
 */
#define OTTO_LPLR_ENTRY 0x9fc00000
#define OTTO_TEMP_SYM_OFF (OTTO_LPLR_ENTRY+0x20)
#define OTTO_TEMP_SYM_HEADER ((const symbol_table_entry_t *)(((soc_t *)OTTO_TEMP_SYM_OFF)->bios.header.export_symb_list))
#define OTTO_TEMP_SYM_END    ((const symbol_table_entry_t *)(((soc_t *)OTTO_TEMP_SYM_OFF)->bios.header.end_of_export_symb_list))
#define _lplr_bios (*(basic_io_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))


/*
 * Commands
 */

#endif /* _RTL9310_CONFIG_H */
