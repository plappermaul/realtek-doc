/*
 * Configuration for Realtek Semiconductor Corp. Elnath platform. Parts were derived from
 * other ARM configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ELNATH_ASIC_H
#define __ELNATH_ASIC_H

#include <generated/autoconf.h>

#ifndef __ASSEMBLY__
#	include <asm/arch/registers.h>
#endif

#define KSEG1_ATU_XLAT(x) (x)

#if 0 /// RTK not use this options
#	define CONFIG_CORTINA_DUAL_IMAGE 1
#endif

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
#ifdef CONFIG_ARM64
#define CA_REG_READ(addr) readl((u64)addr)
#define CA_REG_WRITE(value, addr) writel(value, (u64)addr)
#else
#define CA_REG_READ(addr) readl((u32)addr)
#define CA_REG_WRITE(value, addr) writel(value, (u32)addr)
#endif
#define CONFIG_REMAKE_ELF

/* GICv2 or GICV3 ?? */
/* Bypass GICV3 initialization due to ASIC restriction */
#ifdef CONFIG_CMD_SOC_TEST
#define CONFIG_GICV3
#endif

/*#define CONFIG_ARMV8_SWITCH_TO_EL1*/

#define CONFIG_SUPPORT_RAW_INITRD

//#define CONFIG_ARMV8_MULTIENTRY
#ifdef CONFIG_ARMV8_MULTIENTRY
#	define CPU_RELEASE_ADDR 0x0740fff8 /* SMP jump address in DRAM */
#endif

/* Cache Definitions */
/* #define CONFIG_SYS_DCACHE_OFF */
/* #define CONFIG_SYS_ICACHE_OFF */

//#define DEBUG
//#define CONFIG_DELAY_ENVIRONMENT

#define CONFIG_SYS_INIT_SP_ADDR (0x100000)

/* Flat Device Tree Definitions */
/* #define CONFIG_OF_LIBFDT */

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY (25000000) /* 25MHz */

/* GIC600 base,  0x4_0000000 is high speed peripheal port which bypass ACE */
#define GICD_BASE (0x4F8000000)
#define GICR_BASE (0x4F8040000)
#define SYS_NUM_IRQS 128

#define CONFIG_SYS_MEMTEST_SCRATCH 0x100000
#define CONFIG_SYS_MEMTEST_START 0x5000000
#define CONFIG_SYS_MEMTEST_END 0xD000000

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + (8 << 20))

#define CONFIG_SYS_TIMER_BASE 0xf4321000

#if defined(CONFIG_RTK_V7_FPGA_PLATFORM) || defined(CONFIG_RTK_V8_FPGA_PLATFORM) && !defined(CONFIG_RTK_ZEBU_SIM)
#	define ELNATH_APB_CLK (25000000) /* Use external clock source */
#else
#	define ELNATH_APB_CLK (125000000)
#endif
#define CORTINA_PER_IO_FREQ (ELNATH_APB_CLK)
#define CONFIG_UART_CLOCK (ELNATH_APB_CLK)
#define CONFIG_BAUDRATE 115200
#define CONFIG_SYS_SERIAL0 PER_UART0_CFG
#define CONFIG_SYS_SERIAL1 PER_UART1_CFG

#define GLOBAL_BASE GLOBAL_JTAG_ID

/* SSP Definitions */
#define SSP_BASE DMA_SEC_SSP_ID

#ifdef CONFIG_LED_STATUS
#	ifdef CONFIG_RTK_LED_STYLE_GPIO
/* When use GPIO to control LED, must disabe CONFIG_CORTINA_LED */
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

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_PXE
#define CONFIG_BOOTP_PXE_CLIENTARCH 0x100

/* Miscellaneous configurable options */
#define DDR_BASE 0x00000000
#define DDR_ALIAS_BASE 0x100000000
#define CONFIG_SYS_LOAD_ADDR (DDR_BASE + 0x10000000)
//#define CONFIG_LAST_STAGE_INIT

/* Physical Memory Map */
#define PHYS_SDRAM_1 DDR_BASE /* SDRAM Bank #1 */
//#define PHYS_SDRAM_1_SIZE		0x40000000	/* 1GB */
#define PHYS_SDRAM_1_SIZE 0x80000000 /* 2GB */
//#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* Initial environment variables */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"ethaddr=" __stringify(CONFIG_ETHADDR) "\0" \
	"kernel_addr_r=00080000\0" \
	"initrd_addr_r=0x02000000\0" \
	"fdt_addr_r=0x01800000\0" \
	"fdt_high=0x00000000\0" \
	"active_port=1\0" \
	"auto_scan_active_port=0\0" \
	"recv_multicast_udp_in_booting=0"

#define CONFIG_BOOTARGS \
	"earlycon=serial,0xf4329188 console=ttyS0,115200 " \
	"initrd=0x02000000,0x800000 swiotlb=16384 root=/dev/ram0 rw"
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

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE 256 /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS 64 /* max command args */

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

#ifdef CONFIG_NAND_HIDE_BAD
#define NAND_BBL_OFFSET         (0x200000)  /* 2.0MB */
#define NAND_BBL_OFFSET_REDUND  (0x240000)  /* 2.0MB +  256K */
#define NAND_BBL_SIZE           (0x40000)   /* For 4K Page, the block size is 256K*/

#ifdef CONFIG_BL_IMAGE_INC_OPTEED
#undef NAND_BBL_OFFSET
#define NAND_BBL_OFFSET         (0x440000)  /* 0x440000 */
#undef NAND_BBL_OFFSET_REDUND
#define NAND_BBL_OFFSET_REDUND  (0x480000)  /* 0x440000 +  256K */
#endif
#endif /* #ifdef CONFIG_NAND_HIDE_BAD */

#ifdef CONFIG_CORTINA_SERIAL_NAND
    #define CONFIG_SYS_NAND_SELF_INIT
    #define CONFIG_SYS_NAND_BASE 0xE0000000
    #define CONFIG_SYS_MAX_NAND_CHIPS  1
    #define CONFIG_SYS_MAX_NAND_DEVICE 1

    #ifdef CONFIG_LUNA_MULTI_BOOT
        #ifdef CONFIG_ENV_IS_IN_NAND
            #ifndef CONFIG_SYS_REDUNDAND_ENVIRONMENT
                #define CONFIG_SYS_REDUNDAND_ENVIRONMENT
    	    #endif
		/* uboot config should define them */
            #ifndef CONFIG_ENV_OFFSET
                #define CONFIG_ENV_OFFSET         (0x280000)  /* 2.0MB +  256K x 2*/
                #define CONFIG_ENV_OFFSET_REDUND  (0x2C0000)
                #define CONFIG_ENV_RANGE          (0x40000)
                #define CONFIG_ENV_SIZE           (0x40000)
                #define CONFIG_ENV_SECT_SIZE      (CONFIG_ENV_SIZE)
            #endif
            #undef CONFIG_EXTRA_ENV_SETTINGS
            #define CONFIG_EXTRA_ENV_SETTINGS \
                        "pfl_env1_r=0x280000\0" \
                        "pfl_env2_r=0x2C0000\0" \
                        "pfl_env_size=0x40000\0" \
                        "unzip_src=0x5000000\0" \
                        "ftfp_base=0x5000000\0" \
                        "upenv=tftpboot ${ftfp_base} uboot-env.bin && nand erase " \
                        "${pfl_env1_r} ${pfl_env_size} && nand write ${ftfp_base} " \
                        "${pfl_env1_r} ${filesize} && nand erase ${pfl_env2_r} " \
                        "${pfl_env_size} && nand write ${ftfp_base} ${pfl_env2_r} " \
                        "${filesize}\0" \
                        "active_port=0\0" \
                        "auto_scan_active_port=0\0"
        #endif /* #ifdef CONFIG_ENV_IS_IN_NAND */
    #endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#endif /* #ifdef CONFIG_CORTINA_SPI_NAND */

#if defined(CONFIG_MTD_CORTINA_SERIAL_NOR)
#	undef CONFIG_BOOTCOMMAND
#	define CONFIG_BOOTCOMMAND \
		"sf read $kernel_addr_r 0x200000 0x900000;sf read $fdt_addr_r 0x100000 " \
		"0x10000;booti $kernel_addr_r - $fdt_addr_r"

    #define CONFIG_SF_DEFAULT_CS    (0)
    #define CONFIG_SF_DEFAULT_BUS   (0)
    #define CONFIG_SF_DEFAULT_MODE  (0)
    #define CONFIG_SF_DEFAULT_SPEED (1000000)

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
            "pfl_env_r=0x180000\0" \
            "pfl_env_size=0x40000\0" \
            "unzip_src=0x5000000\0" \
            "ftfp_base=0x5000000\0" \
            "upenv=tftpboot ${ftfp_base} uboot-env.bin && nand erase " \
            "${pfl_env_r} ${pfl_env_size} && nand write ${ftfp_base} " \
            "${pfl_env_r} ${filesize}\0" \
            "active_port=0\0" \
            "auto_scan_active_port=0\0"
#endif /* CONFIG_MTD_CORTINA_SERIAL_NOR */


/* Do not preserve environment */
#ifdef CONFIG_ENV_IS_NOWHERE
#   define CONFIG_ENV_SIZE           (0x40000)
#   define CONFIG_ENV_SECT_SIZE      (CONFIG_ENV_SIZE)
#endif

#endif /* __ELNATH_ASIC_H */
