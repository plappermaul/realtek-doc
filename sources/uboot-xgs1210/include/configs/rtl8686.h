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


#define CONFIG_SYS_HZ     (board_LX_freq_mhz() * 1000 * 1000)

//#define CONFIG_SKIP_LOWLEVEL_INIT	/* SDRAM is initialized by the bootstrap code */

#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)
#define CONFIG_STACKSIZE		(256 << 10)
#define CONFIG_SYS_BOOTPARAMS_LEN	(128 << 10)
#define CONFIG_SYS_INIT_SP_OFFSET	0x400000
#define CONFIG_SYS_TEMP_STACK_ADDR	(0x80004000)


#ifdef CONFIG_CMD_NAND
  #define CONFIG_SYS_MALLOC_LEN		(1 << 21) //old is 20 ,change by eric
#else
  #define CONFIG_SYS_MALLOC_LEN		(1 << 20) /* bohungwu, 20120924, roll back to the original setting as uimage decomporessing overlaps U-Boot stack */
  /* for NOR SPI flash */
  #define CONFIG_CMD_SF 1
#endif
/*
 * UART
 */
//#define CONFIG_SYS_NS16550_CLK		(200*1024*1024)
//#define CONFIG_SYS_NS16550_CLK		(200*1000*1000)
//#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_UART_DIVISOR_MOD   (-1)

/*
 * SDRAM
 */
#define CONFIG_SYS_SDRAM_BASE		0x80000000
//#define CONFIG_SYS_SDRAM_BASE		0xa0000000
//#define CONFIG_SYS_MBYTES_SDRAM		128
#define CONFIG_SYS_MEMTEST_START	0x80200000
#define CONFIG_SYS_MEMTEST_END		0x80400000
#define CONFIG_SYS_LOAD_ADDR		0x80400000	/* default load address */

/*
 * Commands
 */
#include <config_cmd_default.h>



#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_DNS
#undef CONFIG_CMD_RARP
#undef CONFIG_CMD_SNTP
//#define CONFIG_CMD_ELF
//#define CONFIG_CMD_EEPROM

#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS 
#undef CONFIG_CMD_MISC

#ifdef CONFIG_CMD_NAND

//eric.chung add for NAND/MTD support
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#define CONFIG_NAND_RTL8686 

#define CONFIG_ENV_IS_IN_NAND 1
#define CONFIG_CMD_NAND_YAFFS 1
#define CONFIG_YAFFS2 1
//only yaffs 2 support
//#define CONFIG_YAFFS_NO_YAFFS1
#define YAFFS_IGNORE_TAGS_ECC 1
#define CONFIG_SYS_NAND_BASE		0xB801A000	/* NAND control Base Address	*/
#undef CONFIG_ENV_IS_IN_FLASH
#endif


//#ifndef CONFIG_SPINOR_FLASH  //no SPI flash support
//#define CONFIG_SYS_NO_FLASH
//#undef CONFIG_CMD_IMLS 
//#undef CONFIG_CMD_FLASH
//#else //support spi flash
//#define CONFIG_ENV_IS_IN_FLASH
//
//#endif


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
#define CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT	"RTL8686# "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_CBSIZE	512		/* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				 sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	16		/* max number of command args	*/
#define CONFIG_TIMESTAMP			/* Print image info with timestamp */
#define CONFIG_CMDLINE_EDITING			/* add command line history	*/
#define CONFIG_SYS_CONSOLE_INFO_QUIET		/* don't print console @ startup*/

/*
 * FLASH and environment organization
 */
//#define CONFIG_ENV_IS_IN_FLASH
//#define CONFIG_FLASH_NOT_MEM_MAPPED

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
//#define CONFIG_FLASH_BASE		0xbd000000
//#define CONFIG_FLASH_END		0xbdffffff


//#define CONFIG_CMD_JFFS2

/* bohungwu, 20120918, replace SPI NOR FLASH driver */
#ifdef CONFIG_CMD_NAND /* NAND flash */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_CMD_NET
/*
 * CFI driver settings
 */
//#define CONFIG_SYS_FLASH_CFI			/* The flash is CFI compatible	*/
//#define CONFIG_FLASH_CFI_DRIVER		/* Use common CFI driver	*/
//#define CONFIG_SYS_FLASH_CFI_AMD_RESET	1	/* Use AMD (Spansion) reset cmd */
//#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT	/* no byte writes on IXP4xx	*/

#define CONFIG_SYS_FLASH_BASE		0xbd000000
#define CONFIG_SYS_FLASH_BANKS_LIST    { CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	512	/* max number of sectors on one chip	*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#ifdef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	0x10000		/* size of one complete sector	*/
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
#define	CONFIG_ENV_SIZE		0x4000	/* Total Size of Environment Sector	*/

/* Address and size of Redundant Environment Sector	*/
//#define CONFIG_ENV_ADDR_REDUND	(CONFIG_ENV_ADDR + CONFIG_ENV_SECT_SIZE)
//#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)
#endif /* CONFIG_ENV_IS_IN_FLASH */
#else
/* SPI flash setting */
#define CONFIG_SPINOR_FLASH 1
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_FLASH_BASE 0xbd000000
#define CONFIG_ENV_IS_IN_SPI_FLASH

/* Retrieve ENV_OFFSET & ENV_SIZE from preloader */
#ifndef __ASSEMBLY__
#include <soc.h>
#define CONFIG_ENV_OFFSET     (parameters.soc.layout.env_addr)
#define CONFIG_ENV_SIZE       (parameters.soc.layout.env_size)  /* size of environment */
#endif /* #ifndef __ASSEMBLY__ */
/* END of Retrieve ENV_OFFSET & ENV_SIZE from preloader */

#define CONFIG_ENV_SECT_SIZE  0x1000  /* size of one complete sector */
#endif /* CONFIG_CMD_NAND */

#ifdef CONFIG_ENV_IS_IN_NAND
//flash offset and size for Environment Sector
#define CONFIG_ENV_OFFSET 0xA0000
#define CONFIG_ENV_SIZE 0x4000

#define CONFIG_ENV_OFFSET_REDUND 0xC0000
#define CONFIG_ENV_SIZE 0x4000

#endif
/*
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_SIZE		16384
#define CONFIG_SYS_ICACHE_SIZE		16384
#define CONFIG_SYS_CACHELINE_SIZE	32

#define CONFIG_BOOTCOMMAND	"bootm"
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#define CONFIG_IPADDR		192.168.1.3
#define CONFIG_SERVERIP		192.168.1.7
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_ETHADDR		00:E0:4C:86:70:01

#endif  /* __CONFIG_H */
