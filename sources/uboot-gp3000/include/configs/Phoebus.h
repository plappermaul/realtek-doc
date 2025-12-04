/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 */

#ifndef _PHOEBUS_CONFIG_H
#define _PHOEBUS_CONFIG_H

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
#define CONFIG_SYS_MHZ (GET_CPU_MHZ()) /* arbitrary value */
#define CONFIG_SYS_MIPS_TIMER_FREQ (CONFIG_SYS_MHZ * 1000000 >> 1)
#define CONFIG_MIPS

/*
 * Memory map
 */
#define CONFIG_SYS_MONITOR_BASE (CONFIG_SYS_TEXT_BASE)

#define CONFIG_SYS_SDRAM_BASE (0x80000000)
#define CONFIG_SYS_INIT_SP_OFFSET (0x400000)
#define CONFIG_SYS_LOAD_ADDR (CONFIG_SYS_SDRAM_BASE + 0x01000000)
#define CONFIG_MAX_MEM_MAPPED (256 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + (8 << 20))
#define CONFIG_SYS_BOOTM_LEN (64 * 1024 * 1024)
#define CONFIG_SYS_BOOTPARAMS_LEN (8 << 10)

/*
 * Serial driver
 */
#define CONFIG_SYS_NS16550_PORT_MAPPED
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE (-4)
#define CONFIG_SYS_NS16550_COM1 (0xB8002000)
#define CONFIG_SYS_NS16550_CLK (200*1000*1000)

/*
 * Flash configuration
 */
#ifdef CONFIG_OTTO_SNOF
#   define CONFIG_SYS_FLASH_BASE (0x94000000)
#	define CONFIG_SF_DEFAULT_CS (0)
#	define CONFIG_SF_DEFAULT_BUS (0)
#	define CONFIG_SF_DEFAULT_MODE (0)
#	define CONFIG_SF_DEFAULT_SPEED (1000000)
#ifndef CONFIG_ENV_OFFSET
    #define CONFIG_ENV_SIZE       (8 * 1024)
#endif
#ifndef CONFIG_ENV_SECT_SIZE
    #define CONFIG_ENV_SECT_SIZE  (CONFIG_ENV_SIZE) /* size of one complete sector */
#endif

    #undef CONFIG_EXTRA_ENV_SETTINGS
    #define CONFIG_EXTRA_ENV_SETTINGS \
       "ipaddr=192.168.1.3\0" \
       "serverip=192.168.1.7\0" \
       "tftp_base=83c50000\0" \
       "fl_boot_sz=c0000\0" \
       "fl_kernel1=b80000\0" \
       "fl_kernel1_sz=500000\0" \
       "freeAddr=83000000\0" \
       "bootargs=console=ttyS0,115200\0" \
       "ethaddr=00:E0:4C:86:70:01\0" \
       "upb=tftp ${tftp_base} plr.img && crc32 ${fileaddr} ${filesize} && sf erase 0x0 ${fl_boot_sz} && sf write.raw ${fileaddr} 0x0 ${filesize}\0" \
       "upk=tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && sf erase ${fl_kernel1} ${fl_kernel1_sz} && sf write ${fileaddr} ${fl_kernel1} ${filesize}\0" \
       "yu=loady 80000000 && cp.b 80000000 81000000 ${filesize} && cmp.b 80000000 81000000 ${filesize} && sf erase 0 ${fl_boot_sz} && sf write.raw 80000000 0 ${filesize}\0" \
       "bootcmd=sf read ${freeAddr} ${fl_kernel1} ${fl_kernel1_sz} && bootm ${freeAddr}\0"

#elif defined (CONFIG_OTTO_SNAF)
    /****** SPI NAND flash ******/
    #define CONFIG_SYS_NAND_SELF_INIT
    #define CONFIG_SYS_MAX_NAND_DEVICE 1

#ifndef CONFIG_ENV_OFFSET
    #define CONFIG_ENV_OFFSET        (0xC0000) //(6 * 64 * 2048)
#endif
#ifndef CONFIG_ENV_OFFSET_REDUND
    #define CONFIG_ENV_OFFSET_REDUND (0xE0000)
#endif
#ifndef CONFIG_ENV_SIZE
    #define CONFIG_ENV_SIZE          (0x4000) //(16 * 1024)
#endif
    #define CONFIG_ENV_SECT_SIZE     (0x1000)  /* size of one complete sector */

    #undef CONFIG_EXTRA_ENV_SETTINGS
    #define CONFIG_EXTRA_ENV_SETTINGS \
       "ipaddr=192.168.1.3\0" \
       "serverip=192.168.1.7\0" \
       "tftp_base=83c50000\0" \
       "fl_boot_sz=c0000\0" \
       "fl_kernel1=b80000\0" \
       "fl_kernel1_sz=500000\0" \
       "freeAddr=83000000\0" \
       "bootargs=console=ttyS0,115200\0" \
       "ethaddr=00:E0:4C:86:70:01\0" \
       "upb=tftp ${tftp_base} encode_uboot.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}\0" \
       "upk=tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && nand erase ${fl_kernel1} ${fl_kernel1_sz} && nand write ${fileaddr} ${fl_kernel1} ${filesize}\0" \
       "yu=loady 80000000 && cp.b 80000000 81000000 ${filesize} && cmp.b 80000000 81000000 ${filesize} && nand erase 0 ${fl_boot_sz} && nand write.raw 80000000 0 ${filesize}\0" \
       "bootcmd=nand read ${freeAddr} ${fl_kernel1} ${fl_kernel1_sz} && bootm ${freeAddr}\0"

#elif defined (CONFIG_OTTO_PARALLEL_NAND_FLASH)
    /****** ONFI flash ******/
    #define CONFIG_SYS_NAND_SELF_INIT
    #define CONFIG_SYS_MAX_NAND_DEVICE 1

    #define CONFIG_ENV_OFFSET     (0xC0000) //(6 * 64 * 2048)
    #define CONFIG_ENV_SIZE       (0x4000) //(16 * 1024)
    #define CONFIG_ENV_SECT_SIZE  (0x1000)  /* size of one complete sector */
    #if CONFIG_ENABLE_BLOCK_ALIGNMEMT
    #define CONFIG_ENV_OFFSET_REDUND_STATIC (0xE0000)
    #define CONFIG_ENV_OFFSET_REDUND ((CONFIG_ENV_OFFSET)+onfi_block_size())
    #else
    #define CONFIG_ENV_OFFSET_REDUND (0xE0000)
    #endif
#endif


/*
 * Commands
 */

#endif /* _PHOEBUS_CONFIG_H */
