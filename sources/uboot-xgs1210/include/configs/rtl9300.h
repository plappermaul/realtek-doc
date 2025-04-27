/*
 * (C) Copyright 2008 Stefan Roese <sr@denx.de>, DENX Software Engineering
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "autoconf.h"
#include "asm/arch/soc.h"
#ifndef __UBOOT__
#define __UBOOT__
#endif
#include "rtk_flash_common.h"

#define CONFIG_SYS_HZ     (board_LX_freq_mhz() * 1000 * 1000)

#define CONFIG_SYS_MONITOR_BASE    CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_MONITOR_LEN     (256 << 10)
#define CONFIG_STACKSIZE           (256 << 10)
#define CONFIG_SYS_MALLOC_LEN      (1 << 20)
#define CONFIG_SYS_BOOTPARAMS_LEN  (128 << 10)
#define CONFIG_SYS_INIT_SP_OFFSET  0x400000
#define CONFIG_SYS_TEMP_STACK_ADDR (0x80004000)

/*
 * UART
 */
#define CONSOLE_NUM_PORTS   2
#if defined(CONFIG_UART0)
#define CONSOLE_CH_INDEX 0
#else
#define CONSOLE_CH_INDEX 1
#endif

#define CONFIG_BAUDRATE			115200
/* This following instructs u-boot to disable UART when br_div@soc_t=0 */
//#define CONFIG_FOLLOW_PLR_TO_DIS_UART
#define CONFIG_SYS_BAUDRATE_TABLE { 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_UART_DIVISOR_MOD   (-1)

#define CONFIG_PRE_CONSOLE_BUFFER
//#define CONFIG_PRE_CON_BUF_ADDR (DRAM_CADDR) /* From release/soc.h */
#define CONFIG_PRE_CON_BUF_ADDR (0x80000000)
#define CONFIG_PRE_CON_BUF_SZ   (0x00100000)

/*
 * SDRAM
 */
#define CONFIG_SYS_MEMTEST_START 0x80200000
#define CONFIG_SYS_MEMTEST_END   0x80400000
#define CONFIG_SYS_LOAD_ADDR     0x80000000 /* default load address */
#define CONFIG_SYS_CACHELINE_SIZE (32)
#define CONFIG_SYS_SDRAM_BASE (0x80000000)

/*
 * Commands
 */
// #include <config_cmd_default.h> /*Mask by SDK*/

/* for NOR SPI flash */

//#define CONFIG_CMD_SF 1

//#undef CONFIG_CMD_NET
//#undef CONFIG_CMD_NFS
//#undef CONFIG_CMD_DNS
//#undef CONFIG_CMD_RARP
//#undef CONFIG_CMD_SNTP

#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_MISC

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_SUBNETMASK

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP            /* undef to save memory   */
#define CONFIG_SYS_PROMPT  "RTL9300# " /* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE  512         /* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE  (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS 16          /* max number of command args */
#define CONFIG_TIMESTAMP               /* Print image info with timestamp */
// #define CONFIG_CMDLINE_EDITING         /* add command line history */
#define CONFIG_SYS_CONSOLE_INFO_QUIET  /* don't print console @ startup*/

/*
 * We need special accessor functions for the CFI FLASH driver. This
 * can be enabled via the CONFIG_CFI_FLASH_USE_WEAK_ACCESSORS option.
 */
//#define CONFIG_CFI_FLASH_USE_WEAK_ACCESSORS

/*
 * For the non-memory-mapped NOR FLASH, we need to define the
 * NOR FLASH area. This can't be detected via the addr2info()
 * function, since we check for flash access in the very early
 * U-Boot code, before the NOR FLASH is detected.
 */


//#define CONFIG_CMD_JFFS2

/* SPI flash setting */
#ifdef CONFIG_CMD_SF
#define CONFIG_SPINOR_FLASH 1
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_FLASH_BASE 0xb4000000
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET     flash_partition_addr_ret(FLASH_INDEX_LOADER_BDINFO)
#define CONFIG_ENV_SIZE       LOADER_BDINFO_SIZE   /* size of environment */
#define CONFIG_ENV_SECT_SIZE  LOADER_BDINFO_SIZE  /* size of one complete sector */
#endif
#ifdef CONFIG_CMD_SPI_NAND
    #ifndef CONFIG_ENV_IS_IN_SPI_FLASH
    #define CONFIG_ENV_IS_IN_SPI_NAND 1
    #endif
    #ifndef CONFIG_ENV_OFFSET
    #define  CONFIG_ENV_OFFSET    (0xe0000) //(6 * 64 * 2048)
    #endif
#ifdef CONFIG_BOOT_SPI_NAND 
	#define CONFIG_SYS_NO_FLASH
	#ifndef CONFG_ENV_SIZE
		#define CONFIG_ENV_SIZE       (LOADER_BDINFO_SIZE*2)   /* size of environment */
	#endif
	#ifndef CONFIG_ENV_SECT_SIZE
		#define CONFIG_ENV_SECT_SIZE  (LOADER_BDINFO_SIZE*2)  /* size of one complete sector */
	#endif
	#define	CONFIG_SPI_NAND_FLASH_INIT_REST
#endif
#endif


//#define CONFIG_BOOTCOMMAND "bootm"
//#define CONFIG_BOOTDELAY   5 /* autoboot after 5 seconds */
//#define CONFIG_IPADDR      192.168.1.3
//#define CONFIG_SERVERIP    192.168.1.7
//#define CONFIG_NETMASK     255.255.255.0
//#define CONFIG_ETHADDR     00:E0:4C:86:70:01

#ifndef SYSTEM_RESET
#define SYSTEM_RESET() do {        \
        *((volatile u32_t *)0xbb00000C) = 0x00000001; \
	mdelay(1000); \
        } while(0)
#endif

#define NORSF_CHIP_NUM     (1)
#define NORSF_MMIO_4B_EN   (1)
#define NORSF_XREAD_EN     (0)
#define NORSF_WBUF_LIM_B   (128)
#define NORSF_CFLASH_BASE  (0x94000000)
#define NORSF_UCFLASH_BASE (NORSF_CFLASH_BASE | 0x20000000)

#define TIMER_FREQ_MHZ     (board_LX_freq_mhz())
#define TIMER_STEP_PER_US  (4)
#define TIMER1_BASE        (0xb8003210)
#define UDELAY_TIMER_BASE  TIMER1_BASE

#endif  /* __CONFIG_H */
