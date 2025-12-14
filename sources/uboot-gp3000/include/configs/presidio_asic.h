/*
 * Configuration for Cortina-Access Presidio board. Parts were derived from
 * other ARM configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __PRESIDIO_ASIC_H
#define __PRESIDIO_ASIC_H

#include <generated/autoconf.h>

#ifndef __ASSEMBLY__
#	include <asm/arch/ca77xx_registers.h>
#endif

#define CONFIG_CA77XX   1
#define KSEG1_ATU_XLAT(x) (x)

/** RTK not use this
#define CONFIG_CORTINA_DUAL_IMAGE 1
*/
#ifdef CONFIG_NET
/* XRAM BASE ADDRESS */
#	define NI_XRAM_BASE 0xF4500000
#	define GET_NI_XRAM_BASE() (NI_XRAM_BASE)
#endif /* CONFIG_NET */

/* The following CA_READ_READ and WRITE MACROS are used by some Cortina drivers
 * to read and write to SoC registers.  Since some SoC require address
 * translation, we define them here to allow the platform to specify the
 * required tranlation, if any for this platform.
 */
#define CA_REG_READ(addr) readl((u64)addr)
#define CA_REG_WRITE(value, addr) writel(value, (u64)addr)

#define CONFIG_IP_DEFRAG

#define CONFIG_REMAKE_ELF

/* GICv2 or GICV3 ?? */
#define CONFIG_GICV2

/*#define CONFIG_ARMV8_SWITCH_TO_EL1*/

#define CONFIG_SUPPORT_RAW_INITRD

#define CONFIG_ARMV8_MULTIENTRY
#ifdef CONFIG_ARMV8_MULTIENTRY
#	define CPU_RELEASE_ADDR 0x0740fff8 /* SMP jump address in DRAM */
#endif

/* Cache Definitions */
/* #define CONFIG_SYS_DCACHE_OFF */
/* #define CONFIG_SYS_ICACHE_OFF */

//#define DEBUG
//#define CONFIG_DELAY_ENVIRONMENT

#define CONFIG_SYS_INIT_SP_ADDR (0x100000)

#define CONFIG_SYS_BOOTM_LEN (0xc00000)

/* Flat Device Tree Definitions */
/* #define CONFIG_OF_LIBFDT */
/* #define CONFIG_OF_BOARD_SETUP */
/* #define CONFIG_OF_SYSTEM_SETUP */

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY (25000000) /* 25MHz */

/* Generic Interrupt Controller Definitions */
#define GICD_BASE (0xf7011000)
#define GICC_BASE (0xf7012000)

#define CONFIG_SYS_MEMTEST_SCRATCH 0x100000
#define CONFIG_SYS_MEMTEST_START 0x5000000
#define CONFIG_SYS_MEMTEST_END 0xD000000

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + (8 << 20))

#define CONFIG_SYS_TIMER_BASE 0xf4321000

#define PRESIDIO_APB_CLK (125000000) /* Use external clock source */

/* Cortina Serial Configuration */
#define CONFIG_UART_CLOCK (PRESIDIO_APB_CLK)
#define CONFIG_BAUDRATE 115200
#define CONFIG_SYS_SERIAL0 PER_UART0_CFG
#define CONFIG_SYS_SERIAL1 PER_UART1_CFG

#define GLOBAL_BASE GLOBAL_JTAG_ID

/* SSP Definitions */
#define SSP_BASE DMA_SEC_SSP_ID

#ifdef CONFIG_LED_STATUS
#	ifdef CONFIG_RTK_LED_STYLE_GPIO
/* Use GPIO to control LED */
#		undef CONFIG_CORTINA_LED
#	else
#		define CONFIG_CORTINA_LED
#	endif
#endif

#ifdef CONFIG_CORTINA_LED
#	define CORTINA_LED_NUM 16

#	define TRIGGER_HW_RX 0
#	define TRIGGER_HW_TX 1
#	define TRIGGER_SW 2
#	define TRIGGER_NONE 3

#	define BLINK_RATE1 0
#	define BLINK_RATE2 1

/* only used for CONFIG_CMD_LED */
#	define CONFIG_STATUS_LED
#	define CONFIG_BOARD_SPECIFIC_LED
#	define STATUS_LED_BIT 0x0
#	define STATUS_LED_STATE STATUS_LED_OFF
#	define STATUS_LED_PERIOD (CONFIG_SYS_HZ / 4)

#	define CONFIG_CMD_LED
#endif

#if defined(CONFIG_MMC)
#	define CONFIG_CMD_FAT
#	define CONFIG_CMD_EXT2
#	define CONFIG_CMD_EXT4
#	define CONFIG_CMD_PART
#	define CONFIG_EFI_PARTITION
#	define CONFIG_CMD_MMC
#endif

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_PXE
#define CONFIG_BOOTP_PXE_CLIENTARCH 0x100

/* Miscellaneous configurable options */
#define DDR_BASE 0x00000000
#define DDR_ALIAS_BASE 0x100000000
#define CONFIG_SYS_LOAD_ADDR (DDR_BASE + 0x10000000)
#define CONFIG_LAST_STAGE_INIT

/* Physical Memory Map */
#define PHYS_SDRAM_1 DDR_BASE /* SDRAM Bank #1 */
//#define PHYS_SDRAM_1_SIZE		0x40000000	/* 1GB */
#define PHYS_SDRAM_1_SIZE 0x80000000 /* 2GB */
#define CONFIG_SYS_SDRAM_BASE PHYS_SDRAM_1

/* Initial environment variables */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"ethaddr=" __stringify(CONFIG_ETHADDR) "\0" \
	"kernel_addr_r=00080000\0" \
	"initrd_addr_r=0x02000000\0" \
	"fdt_addr_r=0x01000000\0" \
	"fdt_high=0x00000000\0" \
	"uboot_img_name=8277_demo_Board_loader.img\0" \
	"pfl_env1_r=0x400000\0" \
	"pfl_env2_r=0x500000\0" \
	"pfl_env_size=0x20000\0" \
	"tftp_base=0x5000000\0" \
	"upb=tftpboot ${tftp_base} ${8277_demo_Board_loader.img} && nand erase 0x0 " \
	"0x400000 && nand write ${tftp_base} 0x0 ${filesize}\0" \
	"upenv=tftpboot ${tftp_base} uboot-env.bin && nand erase ${pfl_env1_r} " \
	"${pfl_env_size} && nand write ${tftp_base} ${pfl_env1_r} ${filesize}\0"

#define CONFIG_BOOTARGS \
	"earlycon=serial,0xf4329148 console=ttyS0,115200 root=/dev/ram0"
/*
#define CONFIG_BOOTCOMMAND		"bootm $kernel_addr_r " \
          "$initrd_addr_r:$initrd_size $fdt_addr_r"
*/
#define CONFIG_BOOTCOMMAND "booti $kernel_addr_r - $fdt_addr_r"
#define CONFIG_IPADDR 192.168.1.1
#define CONFIG_NETMASK 255.255.255.0
#define CONFIG_SERVERIP 192.168.1.7
#define CONFIG_GATEWAYIP 192.168.1.2
#define CONFIG_ETHADDR 00:50:c2:01:02:03

/* Do not preserve environment */
//#define CONFIG_ENV_IS_NOWHERE		1
//#define CONFIG_ENV_SIZE			0x1000

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE 256 /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS 64 /* max command args */

//#define CONFIG_MTD_CORTINA_SERIAL_NOR
//#define CONFIG_MTD_CORTINA_G3_NAND

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*
 * Note that CONFIG_SYS_MAX_FLASH_SECT allows for a parameter block
 * i.e. the bottom "sector" (bottom boot), or top "sector"
 *      (top boot), is a seperate erase region divided into
 *      4 (equal) smaller sectors. This, notionally, allows
 *      quicker erase/rewrire of the most frequently changed
 *      area......
 *      CONFIG_SYS_MAX_FLASH_SECT is padded up to a multiple of 4
 */
#define CONFIG_SYS_FLASH_BASE 0xe0000000
#define PHYS_FLASH_SIZE 0x08000000 /* 128MB */

#define CONFIG_SYS_MAX_FLASH_BANKS 1 /* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT (512 * 2)

#ifdef CONFIG_ENV_IS_IN_MMC
#	define CONFIG_SYS_MMC_ENV_DEV 0
#	define CONFIG_SYS_MMC_ENV_PART 0
#	define CONFIG_ENV_OFFSET (0x400000)
#	define CONFIG_ENV_SIZE (512 * 1024)
#endif

#ifdef CONFIG_MTD_CORTINA_G3_NAND
#	if 0 // realtek not use UBI in u-boot.
#		define CONFIG_CMD_UBI
#		define CONFIG_RBTREE
#		define CONFIG_MTD_DEVICE
#		define CONFIG_MTD_PARTITIONS
#		define CONFIG_CMD_MTDPARTS
#		define CONFIG_LZO
#	endif
#	define CONFIG_SYS_NAND_ONFI_DETECTION 1
#	define CONFIG_NAND_G3 1
#	define CONFIG_CMD_NAND 1
#	define CONFIG_SYS_MAX_NAND_DEVICE 1
#	define CONFIG_SYS_NAND_MAX_CHIPS 2
#	define CONFIG_SYS_NAND_BASE 0xE0000000
#	define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE }
#	define CONFIG_SYS_NAND_SELF_INIT

#	if CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 0
#		define CONFIG_CS752X_NAND_ECC_HW_HAMMING_256
#	elif CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 1
#		define CONFIG_CS752X_NAND_ECC_HW_HAMMING_512
#	elif CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 2
#		define CONFIG_G3_NAND_ECC_HW_BCH_LEVEL 8
#	elif CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 3
#		define CONFIG_G3_NAND_ECC_HW_BCH_LEVEL 16
#	elif CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 4
#		define CONFIG_G3_NAND_ECC_HW_BCH_LEVEL 24
#	elif CONFIG_MTD_CORTINA_G3_NAND_ECC_LEVEL == 5
#		define CONFIG_G3_NAND_ECC_HW_BCH_LEVEL 40
#	endif
#	undef CONFIG_ENV_IS_IN_FLASH
#	define CONFIG_ENV_IS_IN_NAND
#	define PHYS_FLASH_1 (CONFIG_SYS_FLASH_BASE)
#	define CONFIG_ENV_OFFSET (0x400000)

/* RTK */
#	ifdef CONFIG_LUNA_MULTI_BOOT
/* SIZE= 0x20000, but reserve rest space */
#		define CONFIG_ENV_OFFSET_REDUND (0x500000)
#	endif

#	define CONFIG_ENV_SIZE (0x20000)
#	define CONFIG_ENV_RANGE (0x100000)
#	define CONFIG_ENV_SECT_SIZE (CONFIG_ENV_SIZE)
#	define CONFIG_ENV_ADDR (PHYS_FLASH_1 + CONFIG_ENV_OFFSET)
#	define CONFIG_SYS_MONITOR_BASE (PHYS_FLASH_1)
#	define CONFIG_SYS_MONITOR_LEN (0x100000) /* Reserve 1MB */
#endif

#if defined(CONFIG_MTD_CORTINA_SERIAL_NOR)
#	undef CONFIG_BOOTCOMMAND
#	define CONFIG_BOOTCOMMAND \
		"sf read $kernel_addr_r 0x200000 0x900000;sf read $fdt_addr_r 0x100000 " \
		"0x10000;booti $kernel_addr_r - $fdt_addr_r"
#	define CONFIG_CMD_SF
#	define CONFIG_CMD_SF_TEST
#	undef CONFIG_ENV_IS_IN_FLASH
#	define CONFIG_ENV_IS_IN_SPI_FLASH
#	define PHYS_FLASH_1 (CONFIG_SYS_FLASH_BASE)
#	define CONFIG_ENV_SPI_MAX_HZ 1000000
#	define CONFIG_ENV_SPI_MODE SPI_MODE_3
#	define CONFIG_ENV_SPI_BUS 0
#	define CONFIG_ENV_SPI_CS 1
#	define CONFIG_ENV_OFFSET (0x80000)
#	define CONFIG_ENV_SIZE (0x40000)
#	define CONFIG_ENV_RANGE (0x40000)
#	define CONFIG_ENV_SECT_SIZE (CONFIG_ENV_SIZE)
#	define CONFIG_ENV_ADDR (PHYS_FLASH_1 + CONFIG_ENV_OFFSET)
#	define CONFIG_SYS_MONITOR_BASE (PHYS_FLASH_1)
#	define CONFIG_SYS_MONITOR_LEN (0x100000) /* Reserve 1MB */
#endif /* CONFIG_MTD_CORTINA_SERIAL_NOR */

#ifndef CONFIG_ENV_SIZE
#	define CONFIG_ENV_SIZE (0x40000)
#endif

#endif /* __PRESIDIO_ASIC_H */
