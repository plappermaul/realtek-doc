#include <common.h>
#include <soc.h>
#include <swp.h>

#include <osc.h>

#ifndef CONFIG_ENABLE_BLOCK_ALIGNMEMT
	#define CONFIG_ENABLE_BLOCK_ALIGNMEMT 1
#endif

/* Should be consistent with part_info defined */
enum PART_IDX
{
	SWP_FL_CFGFS,
	SWP_FL_ENV2,
	SWP_FL_OPT3,
	SWP_FL_OPT4,
	SWP_FL_KERNEL1,
	SWP_FL_KERNEL2,
	SWP_FL_ROOTFS1,
	SWP_FL_ROOTFS2,
	SWP_FL_END,
	SWP_FL_ENV,
	SWP_FL_APP,
	SWP_FL_DPI,
	SWP_FL_UBIDEV,
	SWP_FL_CFG_ST,
	SWP_FL_BOOT // must be the last
};

#if CONFIG_RTK_ROOTFS_DM_VERITY
	#define THE_BOOT_CMD "source ${freeAddr}:script"
#else
	#define THE_BOOT_CMD "bootm ${freeAddr}"
#endif

#ifdef CONFIG_RTK_USE_ONE_UBI_DEVICE
struct parts_size_t
{
	const char *name;
	uint32_t size;
};
struct parts_size_t parts_size[] = {
	[SWP_FL_KERNEL1] = {"fl_kernel1_sz", 0},
	[SWP_FL_KERNEL2] = {"fl_kernel2_sz", 0},
	[SWP_FL_ROOTFS1] = {"fl_rootfs1_sz", 0},
	[SWP_FL_ROOTFS2] = {"fl_rootfs2_sz", 0},
	[SWP_FL_CFGFS]   = {"fl_cfgfs_sz", 0},

};
#endif

/* SPI flash setting */
#ifdef CONFIG_OTTO_SNOF
    /****** SPI NOR flash ******/
    #define SWP_CONFIG_SYS_NO_FLASH
    #define SWP_CONFIG_SYS_FLASH_BASE (0xb4000000)
    #define SWP_CONFIG_ENV_IS_IN_SPI_FLASH
    #define SWP_CONFIG_ENV_OFFSET (256 * 1024)
    #define SWP_CONFIG_ENV_SIZE (8 * 1024)
    #define SWP_CONFIG_ENV_SECT_SIZE (0x1000) /* size of one complete sector */
    #define SWP_CONFIG_ENV_OFFSET_REDUND (264 * 1024)
    #define SWP_CONFIG_SYS_REDUNDAND_ENVIRONMENT 1
#elif defined(CONFIG_OTTO_SNAF)
    /****** SPI NAND flash ******/
    #define SWP_CONFIG_ENV_OFFSET                      (CONFIG_ENV_OFFSET)
    #define SWP_CONFIG_ENV_OFFSET_REDUND               (CONFIG_ENV_OFFSET_REDUND)
    #define SWP_CONFIG_ENV_OFFSET_REDUND_STATIC        (SWP_CONFIG_ENV_OFFSET_REDUND)
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
    /****** ONFI flash ******/
    #define SWP_CONFIG_ENV_OFFSET (0xC0000) //(6 * 64 * 2048)
    #define SWP_CONFIG_SYS_NO_FLASH
    #define SWP_CONFIG_ENV_IS_IN_ONFI
    #define SWP_CONFIG_ENV_SIZE (0x4000)	  //(16 * 1024)
    #define SWP_CONFIG_ENV_SECT_SIZE (0x1000) /* size of one complete sector */
    #if CONFIG_ENABLE_BLOCK_ALIGNMEMT
    	#define SWP_CONFIG_ENV_OFFSET_REDUND_STATIC (0xE0000)
    	#define SWP_CONFIG_ENV_OFFSET_REDUND (SWP_CONFIG_ENV_OFFSET + onfi_block_size())
    #else
	    #define SWP_CONFIG_ENV_OFFSET_REDUND (0xE0000)
    #endif
    #define SWP_CONFIG_SYS_REDUNDAND_ENVIRONMENT 1
#else
    #error "Error!!! One of CONFIG_OTTO_SNOF or CONFIG_OTTO_SNAF  or CONFIG_OTTO_PARALLEL_NAND_FLASHmust be defined"
#endif

#ifdef CONFIG_OTTO_SNOF
    /****** SPI NOR flash ******/
    #define SWP_FL_8MB (8 * 1024 * 1024)
    #define SWP_FL_8MB_CFGFS_BASE (272 * 1024)
    #define SWP_FL_8MB_K0_BASE (512 * 1024)
    #define SWP_FL_8MB_K_SIZE (1408 * 1024)
    #define SWP_FL_8MB_R0_BASE (SWP_FL_8MB_K0_BASE + SWP_FL_8MB_K_SIZE)
    #define SWP_FL_8MB_R_SIZE (((SWP_FL_8MB - SWP_FL_8MB_K0_BASE) >> 1) - SWP_FL_8MB_K_SIZE)
    #define SWP_FL_8MB_K1_BASE (SWP_FL_8MB_R0_BASE + SWP_FL_8MB_R_SIZE)
    #define SWP_FL_8MB_R1_BASE (SWP_FL_8MB_K1_BASE + SWP_FL_8MB_K_SIZE)

    #define SWP_FL_16MB (16 * 1024 * 1024)
    #define SWP_FL_16MB_CFGFS_BASE SWP_FL_8MB_CFGFS_BASE
    #define SWP_FL_16MB_K0_BASE SWP_FL_8MB_K0_BASE
    #define SWP_FL_16MB_K_SIZE (3 * 1024 * 1024)
    #define SWP_FL_16MB_R0_BASE (SWP_FL_16MB_K0_BASE + SWP_FL_16MB_K_SIZE)
    #define SWP_FL_16MB_R_SIZE (((SWP_FL_16MB - SWP_FL_16MB_K0_BASE) >> 1) - SWP_FL_16MB_K_SIZE)
    #define SWP_FL_16MB_K1_BASE (SWP_FL_16MB_R0_BASE + SWP_FL_16MB_R_SIZE)
    #define SWP_FL_16MB_R1_BASE (SWP_FL_16MB_K1_BASE + SWP_FL_16MB_K_SIZE)

    #define SWP_FL_32MB (32 * 1024 * 1024)
    #define SWP_FL_32MB_CFGFS_BASE SWP_FL_8MB_CFGFS_BASE
    #define SWP_FL_32MB_K0_BASE SWP_FL_8MB_K0_BASE
    #define SWP_FL_32MB_K_SIZE (6 * 1024 * 1024)
    #define SWP_FL_32MB_R0_BASE (SWP_FL_32MB_K0_BASE + SWP_FL_32MB_K_SIZE)
    #define SWP_FL_32MB_R_SIZE (((SWP_FL_32MB - SWP_FL_32MB_K0_BASE) >> 1) - SWP_FL_32MB_K_SIZE)
    #define SWP_FL_32MB_K1_BASE (SWP_FL_32MB_R0_BASE + SWP_FL_32MB_R_SIZE)
    #define SWP_FL_32MB_R1_BASE (SWP_FL_32MB_K1_BASE + SWP_FL_32MB_K_SIZE)
#elif defined(CONFIG_OTTO_SNAF)
    /****** SPI NAND flash ******/
    #define SWP_FL_64MB (64 * 1024 * 1024)
    #define SWP_FL_MAX_USE_64MB (SWP_FL_64MB - (1280 * 1024)) //reserve 1280KB(10 blocks) for 512Mbit NAND flash
    #define SWP_FL_64MB_CFGFS_BASE ((CONFIG_ENV_OFFSET_REDUND) + (128 * 1024))			  // ENV2 + 128K(1 block size)
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_64MB_CFG_ST_BASE (11 * 1024 * 1024)
	#define SWP_FL_64MB_CFG_ST_SIZE (128 * 1024)
#endif
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_64MB_K0_BASE (11 * 1024 * 1024 + SWP_FL_64MB_CFG_ST_SIZE)
	#define SWP_FL_64MB_K_SIZE (7 * 1024 * 1024 - (64 * 1024))
#else
	#define SWP_FL_64MB_K0_BASE (11 * 1024 * 1024)
	#define SWP_FL_64MB_K_SIZE (7 * 1024 * 1024)
#endif
    #define SWP_FL_64MB_R0_BASE (SWP_FL_64MB_K0_BASE + SWP_FL_64MB_K_SIZE)
    #define SWP_FL_64MB_R_SIZE (((SWP_FL_MAX_USE_64MB - SWP_FL_64MB_K0_BASE) >> 1) - SWP_FL_64MB_K_SIZE)
    #define SWP_FL_64MB_K1_BASE (SWP_FL_64MB_R0_BASE + SWP_FL_64MB_R_SIZE)
    #define SWP_FL_64MB_R1_BASE (SWP_FL_64MB_K1_BASE + SWP_FL_64MB_K_SIZE)
    #define SWP_FL_64MB_OPT_BASE (1 * 1024 * 1024)
    #define SWP_FL_64MB_OPT3_BASE (SWP_FL_64MB_R1_BASE + SWP_FL_64MB_OPT_BASE)
    #define SWP_FL_64MB_OPT4_BASE (SWP_FL_64MB_OPT3_BASE + SWP_FL_64MB_OPT_BASE)
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
#ifdef CONFIG_STATIC_CONFIG
	#undef SWP_FL_64MB_CFG_ST_BASE
	#define SWP_FL_64MB_CFG_ST_BASE ((CONFIG_ENV_OFFSET_REDUND)	+ (128 * 1024))	// ENV2 + 128K(1 block size)
	//#define SWP_FL_64MB_CFG_ST_SIZE    (128*1024)
	#define SWP_FL_64MB_UBIDEV_BASE (SWP_FL_64MB_CFG_ST_BASE + SWP_FL_64MB_CFG_ST_SIZE)
#else
	#define SWP_FL_64MB_UBIDEV_BASE (1152 * 1024) // ENV2 + 128K(1 block size)
#endif
#define SWP_FL_64MB_UBIDEV_SIZE (SWP_FL_MAX_USE_64MB - SWP_FL_64MB_UBIDEV_BASE)
#define SWP_FL_64MB_CFGFS_SIZE (SWP_FL_64MB_K0_BASE - SWP_FL_64MB_CFGFS_BASE)
#endif

#define SWP_FL_128MB (128 * 1024 * 1024)
#define SWP_FL_MAX_USE_128MB (SWP_FL_128MB - (2560 * 1024)) //reserve 2560KB(20 blocks) for 1Gbit NAND flash
#define SWP_FL_128MB_CFGFS_BASE ((CONFIG_ENV_OFFSET_REDUND)	+ (128 * 1024))			// ENV2 + 128K(1 block size)
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_128MB_CFG_ST_BASE (4096*1024)
	#define SWP_FL_128MB_CFG_ST_SIZE (256*1024)
#endif
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_128MB_K0_BASE (4096*1024+SWP_FL_128MB_CFG_ST_SIZE)
	#define SWP_FL_128MB_K_SIZE (5376*1024-128*1024)
#else
	#define SWP_FL_128MB_K0_BASE (4096*1024)
	#define SWP_FL_128MB_K_SIZE (5376*1024)
#endif
#define SWP_FL_128MB_R0_BASE (SWP_FL_128MB_K0_BASE + SWP_FL_128MB_K_SIZE)
#define SWP_FL_128MB_R_SIZE (15 * 1024 * 1024)
#define SWP_FL_128MB_K1_BASE (SWP_FL_128MB_R0_BASE + SWP_FL_128MB_R_SIZE)
#define SWP_FL_128MB_R1_BASE (SWP_FL_128MB_K1_BASE + SWP_FL_128MB_K_SIZE)
#define SWP_FL_128MB_OPT_SIZE (16 * 1024 * 1024)
#define SWP_FL_128MB_OPT3_BASE (SWP_FL_128MB_R1_BASE + SWP_FL_128MB_R_SIZE)
#define SWP_FL_128MB_OPT4_BASE (SWP_FL_128MB_OPT3_BASE + SWP_FL_128MB_OPT_SIZE)
#define SWP_FL_128MB_APP_SIZE (43 * 1024 * 1024)
#define SWP_FL_128MB_APP_BASE (SWP_FL_128MB_OPT4_BASE + SWP_FL_128MB_OPT_SIZE)
#define SWP_FL_128MB_DPI_SIZE (6*1024*1024)	
#define SWP_FL_128MB_DPI_BASE (SWP_FL_128MB_APP_BASE+SWP_FL_128MB_APP_SIZE)

#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
#ifdef CONFIG_STATIC_CONFIG
	#undef SWP_FL_128MB_CFG_ST_BASE
	#define SWP_FL_128MB_CFG_ST_BASE ((CONFIG_ENV_OFFSET_REDUND) + (128 * 1024))	// ENV2 + 128K (1 block size)
	//#define SWP_FL_128MB_CFG_ST_SIZE   (128*1024)
	#define SWP_FL_128MB_UBIDEV_BASE (SWP_FL_128MB_CFG_ST_BASE + SWP_FL_128MB_CFG_ST_SIZE)
#else
	#define SWP_FL_128MB_UBIDEV_BASE (1152 * 1024) // ENV2
#endif
#define SWP_FL_128MB_UBIDEV_SIZE (SWP_FL_MAX_USE_128MB - SWP_FL_128MB_UBIDEV_BASE)
#define SWP_FL_128MB_CFGFS_SIZE (SWP_FL_128MB_K0_BASE - SWP_FL_128MB_CFGFS_BASE)
#endif

#define SWP_FL_256MB (256 * 1024 * 1024)
#define SWP_FL_MAX_USE_256MB (SWP_FL_256MB - (5120 * 1024)) //reserve 5120KB(40 blocks) for 2Gbit NAND flash
#define SWP_FL_256MB_CFGFS_BASE ((CONFIG_ENV_OFFSET_REDUND) + (128 * 1024))				// ENV2 + 128K (1 block size)
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_256MB_CFG_ST_BASE (12288 * 1024)
	#define SWP_FL_256MB_CFG_ST_SIZE (256 * 1024)
#endif
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_256MB_K0_BASE (12288 * 1024 + SWP_FL_256MB_CFG_ST_SIZE)
	#define SWP_FL_256MB_K_SIZE (8 * 1024 * 1024 - (128 * 1024))
#else
	#define SWP_FL_256MB_K0_BASE (12288 * 1024)
	#define SWP_FL_256MB_K_SIZE (8 * 1024 * 1024)
#endif
#define SWP_FL_256MB_R0_BASE (SWP_FL_256MB_K0_BASE + SWP_FL_256MB_K_SIZE)
#define SWP_FL_256MB_R_SIZE (48640 * 1024)
#define SWP_FL_256MB_K1_BASE (SWP_FL_256MB_R0_BASE + SWP_FL_256MB_R_SIZE)
#define SWP_FL_256MB_R1_BASE (SWP_FL_256MB_K1_BASE + SWP_FL_256MB_K_SIZE)
#define SWP_FL_256MB_OPT_SIZE   (16*1024*1024) 
#define SWP_FL_256MB_OPT3_BASE    (SWP_FL_256MB_R1_BASE+SWP_FL_256MB_R_SIZE)                                                                                        
#define SWP_FL_256MB_OPT4_BASE    (SWP_FL_256MB_OPT3_BASE+SWP_FL_256MB_OPT_SIZE)
#define SWP_FL_256MB_APP_SIZE (86* 1024 * 1024)
#define SWP_FL_256MB_APP_BASE (SWP_FL_256MB_OPT4_BASE + SWP_FL_256MB_OPT_SIZE)
#define SWP_FL_256MB_DPI_SIZE (10*1024*1024)	
#define SWP_FL_256MB_DPI_BASE    (SWP_FL_256MB_APP_BASE+SWP_FL_256MB_APP_SIZE)
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
#ifdef CONFIG_STATIC_CONFIG
	#undef SWP_FL_256MB_CFG_ST_BASE
	#define SWP_FL_256MB_CFG_ST_BASE ((CONFIG_ENV_OFFSET_REDUND) + (128 * 1024))	// ENV2 + 128K (1 block size)
	//#define SWP_FL_256MB_CFG_ST_SIZE   (256*1024)
	#define SWP_FL_256MB_UBIDEV_BASE (SWP_FL_256MB_CFG_ST_BASE + SWP_FL_256MB_CFG_ST_SIZE)
#else
	#define SWP_FL_256MB_UBIDEV_BASE (1152 * 1024)
#endif
#define SWP_FL_256MB_UBIDEV_SIZE (SWP_FL_MAX_USE_256MB - SWP_FL_256MB_UBIDEV_BASE)
#define SWP_FL_256MB_CFGFS_SIZE (SWP_FL_256MB_K0_BASE - SWP_FL_256MB_CFGFS_BASE)
#endif

#define SWP_FL_512MB (512 * 1024 * 1024)
#define SWP_FL_MAX_USE_512MB (SWP_FL_512MB - (10240 * 1024)) //reserve 10240KB(80 blocks) for 4Gbit NAND flash
#define SWP_FL_512MB_CFGFS_BASE ((CONFIG_ENV_OFFSET_REDUND) + (256 * 1024))		 // ENV2 + 128K (1 block size)
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_512MB_CFG_ST_BASE (12288 * 1024)
	#define SWP_FL_512MB_CFG_ST_SIZE (256 * 1024)
#endif
#ifdef CONFIG_STATIC_CONFIG
	#define SWP_FL_512MB_K0_BASE (12288 * 1024 + SWP_FL_512MB_CFG_ST_SIZE)
	#define SWP_FL_512MB_K_SIZE (8 * 1024 * 1024 - (256 * 1024))
#else
	#define SWP_FL_512MB_K0_BASE (12288 * 1024)
	#define SWP_FL_512MB_K_SIZE (8 * 1024 * 1024)
#endif
#define SWP_FL_512MB_R0_BASE (SWP_FL_512MB_K0_BASE + SWP_FL_512MB_K_SIZE)
#define SWP_FL_512MB_R_SIZE (173 * 1024 * 1024)
#define SWP_FL_512MB_K1_BASE (SWP_FL_512MB_R0_BASE + SWP_FL_512MB_R_SIZE)
#define SWP_FL_512MB_R1_BASE (SWP_FL_512MB_K1_BASE + SWP_FL_512MB_K_SIZE)
#define SWP_FL_512MB_APP_SIZE (118 * 1024 * 1024)
#define SWP_FL_512MB_APP_BASE (SWP_FL_512MB_R1_BASE + SWP_FL_512MB_R_SIZE)
#define SWP_FL_512MB_DPI_SIZE (10*1024*1024)	
#define SWP_FL_512MB_DPI_BASE (SWP_FL_512MB_APP_BASE+SWP_FL_512MB_APP_SIZE)
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
#ifdef CONFIG_STATIC_CONFIG
	#undef SWP_FL_512MB_CFG_ST_BASE
	#define SWP_FL_512MB_CFG_ST_BASE ((CONFIG_ENV_OFFSET_REDUND) + (256 * 1024))	// ENV2 + 256K (1 block size)
	//#define SWP_FL_512MB_CFG_ST_SIZE    (256*1024)
	#define SWP_FL_512MB_UBIDEV_BASE (SWP_FL_512MB_CFG_ST_BASE + SWP_FL_512MB_CFG_ST_SIZE)
#else
#define SWP_FL_512MB_UBIDEV_BASE (1152 * 1024) // ENV2 + 128K (1 block size)
#endif
#define SWP_FL_512MB_UBIDEV_SIZE (SWP_FL_MAX_USE_512MB - SWP_FL_512MB_UBIDEV_BASE)
#define SWP_FL_512MB_CFGFS_SIZE (SWP_FL_512MB_K0_BASE - SWP_FL_512MB_CFGFS_BASE)
#endif

#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
/****** Parallel NAND flash ******/
#define SWP_FL_64MB (64 * 1024 * 1024)
#define SWP_FL_MAX_USE_64MB (SWP_FL_64MB - (1280 * 1024)) //reserve 1280KB(10 blocks) for 512Mbit NAND flash
#define SWP_FL_64MB_CFGFS_BASE (1 * 1024 * 1024)
#define SWP_FL_64MB_K0_BASE (11 * 1024 * 1024)
#define SWP_FL_64MB_K_SIZE (8 * 1024 * 1024)
#define SWP_FL_64MB_R0_BASE (SWP_FL_64MB_K0_BASE + SWP_FL_64MB_K_SIZE)
#define SWP_FL_64MB_R_SIZE (((SWP_FL_MAX_USE_64MB - SWP_FL_64MB_K0_BASE) >> 1) - SWP_FL_64MB_K_SIZE)
#define SWP_FL_64MB_K1_BASE (SWP_FL_64MB_R0_BASE + SWP_FL_64MB_R_SIZE)
#define SWP_FL_64MB_R1_BASE (SWP_FL_64MB_K1_BASE + SWP_FL_64MB_K_SIZE)
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
	#define SWP_FL_64MB_UBIDEV_BASE (1 * 1024 * 1024)
	#define SWP_FL_64MB_UBIDEV_SIZE (SWP_FL_MAX_USE_64MB - SWP_FL_64MB_UBIDEV_BASE)
	#define SWP_FL_64MB_CFGFS_SIZE (SWP_FL_64MB_K0_BASE - SWP_FL_64MB_CFGFS_BASE)
#endif

#define SWP_FL_128MB (128 * 1024 * 1024)
#define SWP_FL_MAX_USE_128MB (SWP_FL_128MB - (2560 * 1024)) //reserve 2560KB(20 blocks) for 1Gbit NAND flash
#define SWP_FL_128MB_CFGFS_BASE (768 * 1024)
#define SWP_FL_128MB_K0_BASE (25 * 1024 * 1024)
#define SWP_FL_128MB_K_SIZE (8 * 1024 * 1024)
#define SWP_FL_128MB_R0_BASE (SWP_FL_128MB_K0_BASE + SWP_FL_128MB_K_SIZE)
#define SWP_FL_128MB_R_SIZE (((SWP_FL_MAX_USE_128MB - SWP_FL_128MB_K0_BASE) >> 1) - SWP_FL_128MB_K_SIZE)
#define SWP_FL_128MB_K1_BASE (SWP_FL_128MB_R0_BASE + SWP_FL_128MB_R_SIZE)
#define SWP_FL_128MB_R1_BASE (SWP_FL_128MB_K1_BASE + SWP_FL_128MB_K_SIZE)
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
	#define SWP_FL_128MB_UBIDEV_BASE (1 * 1024 * 1024)
	#define SWP_FL_128MB_UBIDEV_SIZE (SWP_FL_MAX_USE_128MB - SWP_FL_128MB_UBIDEV_BASE)
	#define SWP_FL_128MB_CFGFS_SIZE (SWP_FL_128MB_K0_BASE - SWP_FL_128MB_CFGFS_BASE)
#endif
#else
	#error "Error!!! One of CONFIG_OTTO_SNOF or CONFIG_OTTO_SNAF  or CONFIG_OTTO_PARALLEL_NAND_FLASHmust be defined"
#endif

//#define FL_DEBUG
#ifdef FL_DEBUG
	#define DEBUG(format, args...) printf(format, ##args)
#else
	#define DEBUG(args...)
#endif

#ifndef CONFIG_BOOT_IMG_RO
	#define CONFIG_BOOT_IMG_RO 1
#endif

#ifdef CONFIG_OTTO_SNOF
	#define SWP_RTK_MTD_DEV_NAME "rtk_spi_nor_mtd"
	#include <spi_flash.h>
	#include "nor_spi/nor_spif_core.h"
#elif defined CONFIG_OTTO_SNAF
	#define SWP_RTK_MTD_DEV_NAME "spinand"
#elif defined CONFIG_OTTO_PARALLEL_NAND_FLASH
	#define SWP_RTK_MTD_DEV_NAME "onfi"
#else
	#error "Error!!! One of CONFIG_OTTO_SNOF or CONFIG_OTTO_SNAF or CONFIG_OTTO_PARALLEL_NAND_FLASH must be defined"
#endif

#define SWP_GB_BITS (30)
#define SWP_MB_BITS (20)
#define SWP_KB_BITS (10)
#define SWP_GB_MASK ((0x1 << SWP_GB_BITS) - 1)
#define SWP_MB_MASK ((0x1 << SWP_MB_BITS) - 1)
#define SWP_KB_MASK ((0x1 << SWP_KB_BITS) - 1)

#define SWP_BOOT_KERNEL "linux"
#define SWP_BOOT_ROOTFS "rootfs"

typedef struct
{
	char *name;
	u32 base;
	u32 size;
} swp_part_info_t;

/* Move the following to different setting */
static swp_part_info_t part_info[] = {
	[SWP_FL_CFGFS]   = {"config",     0,                                   0}, /* CFGFS */
#if CONFIG_ENABLE_BLOCK_ALIGNMEMT
#ifdef CONFIG_ENV_OFFSET_REDUND_STATIC
	[SWP_FL_ENV2]    = {"env2",       SWP_CONFIG_ENV_OFFSET_REDUND_STATIC, 0}, /* ENV2    */
#else
	[SWP_FL_ENV2]    = {"env2",       SWP_CONFIG_ENV_OFFSET_REDUND,        0}, /* ENV2    */
#endif
#else
	[SWP_FL_ENV2]    = {"env2",       SWP_CONFIG_ENV_OFFSET_REDUND,        0}, /* ENV2   */
#endif
	[SWP_FL_OPT3]    = {"framework1", 0,                                   0}, /* opt3     */
	[SWP_FL_OPT4]    = {"framework2", 0,                                   0}, /* opt4     */
	[SWP_FL_KERNEL1] = {"k0",         0,                                   0}, /* kernel1 */
	[SWP_FL_KERNEL2] = {"k1",         0,                                   0}, /* kernel2 */
	[SWP_FL_ROOTFS1] = {"r0",         0,                                   0}, /* rootfs1  */
	[SWP_FL_ROOTFS2] = {"r1",         0,                                   0}, /* rootfs2  */
	[SWP_FL_END]     = {"END",        0,                                   0}, /* end       */
	[SWP_FL_ENV]     = {"env",        SWP_CONFIG_ENV_OFFSET,               0}, /* U-Boot environment partition */
	[SWP_FL_APP]     = {"apps",       0,                                   0}, /* app                                     */
	[SWP_FL_DPI]     = {"dpi",        0,                                   0}, /* dpi  									  */
	[SWP_FL_CFG_ST]  = {"static_conf",0,                                   0}, /* static_conf for static config     */
	[SWP_FL_UBIDEV]  = {"ubi_device", 0,                                   0}, /* ubi device partition                */
	[SWP_FL_BOOT]    = {"boot",       0,                                   0}, /* BOOTLOADER partition, must be the last entity in this array */
};

static swp_part_info_t *pi_ptr[ARRAY_SIZE(part_info)];

#ifdef CONFIG_OTTO_FL_TO_MTDPARTS
char *swp_to_KMGB(u32 val, char *output_p, u32 output_size)
{
	if ((val & SWP_GB_MASK) == 0)
	{ /* To GB */
		sprintf(output_p, "%uG", (val >> SWP_GB_BITS));
	}
	else if ((val & SWP_MB_MASK) == 0)
	{ /* To MB */
		sprintf(output_p, "%uM", (val >> SWP_MB_BITS));
	}
	else if ((val & SWP_KB_MASK) == 0)
	{ /* To KB */
		sprintf(output_p, "%uK", (val >> SWP_KB_BITS));
	}
	else
	{
		sprintf(output_p, "%u", (val));
	}
	return output_p;
}

#if CONFIG_BOOT_IMG_RO
#ifdef CONFIG_LUNA_MULTI_BOOT
	#define PROTECT_SUFFIX "ro"
#else
	#define PROTECT_SUFFIX ""
#endif
#endif

#define SWP_TS_SIZE (64) /* Magic number temp string size */
#define SWP_VIRTUAL_MTD_START 12
const u32 SWP_FL_KERNEL_ID[] = {SWP_FL_KERNEL1, SWP_FL_KERNEL2};
const u32 SWP_FL_ROOTFS_ID[] = {SWP_FL_ROOTFS1, SWP_FL_ROOTFS2};
//Generate mtdparts (root=31:? is also included)
__attribute__((__unused__)) static void swp_flash_layout_to_mtdparts(char *mtdparts_output, u32 img_id, u32 mtdparts_default)
{
	char tmp_str0[SWP_TS_SIZE] = {0};
	char tmp_str1[SWP_TS_SIZE] = {0};
	u32 i, j;
	char *p;

	p = mtdparts_output;
#if !defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
	u32 rootfs_partition = ARRAY_SIZE(part_info);
#endif

	p += sprintf(p, "mtdparts=%s:", SWP_RTK_MTD_DEV_NAME);
	for(i = 0, j = 0; i < ARRAY_SIZE(part_info); i++) {
		if(pi_ptr[i]->size != 0) {
#if !CONFIG_BOOT_IMG_RO
			p += sprintf(p, "%s(%s),", swp_to_KMGB(pi_ptr[i]->size, tmp_str0, SWP_TS_SIZE), pi_ptr[i]->name);
			//p += sprintf(p, "%s@%s(%s),", to_KMGB(pi_ptr[i]->size, tmp_str0, SWP_TS_SIZE), to_KMGB(pi_ptr[i]->base, tmp_str1, SWP_TS_SIZE), pi_ptr[i]->name);
#else
			if(pi_ptr[i]->name == part_info[SWP_FL_ROOTFS_ID[img_id]].name ||
				pi_ptr[i]->name == part_info[SWP_FL_KERNEL_ID[img_id]].name) {
				p += sprintf(p, "%s(%s)" PROTECT_SUFFIX ",", swp_to_KMGB(pi_ptr[i]->size, tmp_str0, SWP_TS_SIZE), pi_ptr[i]->name);
			}else{
				p += sprintf(p, "%s(%s),", swp_to_KMGB(pi_ptr[i]->size, tmp_str0, SWP_TS_SIZE), pi_ptr[i]->name);
			}
#endif

#if !defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
			/* Record rootfs partition */
			if(pi_ptr[i]->name == part_info[SWP_FL_ROOTFS_ID[img_id]].name){
				DEBUG("pi_ptr[i]->name=%s, part_info[SWP_FL_ROOTFS_ID[img_id]].name=%s,i=%d,j=%d\n", pi_ptr[i]->name, part_info[SWP_FL_ROOTFS_ID[img_id]].name, i, j);
				rootfs_partition = j;
			}
#endif
			j++;
		}
	}

	/* Generate default mtdparts for uboot/MTD, partition layout string only */
	if (mtdparts_default)
		return;

#ifdef CONFIG_LUNA_MULTI_BOOT
	/* Fill padding mtd partition */
	for (; j < SWP_VIRTUAL_MTD_START; j++){
		p += sprintf(p, "4K@0ro,");
		//p += sprintf(p, "4K@%s(m)ro,", to_KMGB(part_info[FL_END].size, tmp_str0, SWP_TS_SIZE));
	}
	/* Create alias partition for booting linux & kernel */
	p += sprintf(p, "%s@%s(%s),", swp_to_KMGB(part_info[SWP_FL_KERNEL_ID[img_id]].size, tmp_str0, SWP_TS_SIZE), swp_to_KMGB(part_info[SWP_FL_KERNEL_ID[img_id]].base, tmp_str1, SWP_TS_SIZE), SWP_BOOT_KERNEL);
	p += sprintf(p, "%s@%s(%s),", swp_to_KMGB(part_info[SWP_FL_ROOTFS_ID[img_id]].size, tmp_str0, SWP_TS_SIZE), swp_to_KMGB(part_info[SWP_FL_ROOTFS_ID[img_id]].base, tmp_str1, SWP_TS_SIZE), SWP_BOOT_ROOTFS);
#endif

	/* Replace the last ',' (comma sign) by a null character '\0' */
	if (mtdparts_output[strlen(mtdparts_output) - 1] == ',')
	{
		mtdparts_output[strlen(mtdparts_output) - 1] = '\0';
	}

#if !defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
	if (rootfs_partition == ARRAY_SIZE(part_info))
	{
		printf("Error: Cannot find rootfs partition\n");
	}
	else
	{
		DEBUG("rootfs_partition=%d\n", rootfs_partition);
		p = mtdparts_output + strlen(mtdparts_output);
		p += sprintf(p, " root=31:%d", rootfs_partition);
	}
#endif
}

#endif /* #ifdef CONFIG_OTTO_FL_TO_MTDPARTS */

void swp_sort_part_info(swp_part_info_t *input_pi_ptr[], u32 num)
{
	u32 i, j;
	swp_part_info_t *item_p;

	/* Use insertion sort as the number of elements in the array is not huge */
	for (i = 1; i < num; i++)
	{
		j = i;
		item_p = input_pi_ptr[j];
		while ((j > 0) && (item_p->base < input_pi_ptr[j - 1]->base))
		{
			input_pi_ptr[j] = input_pi_ptr[j - 1];
			j--;
		}
		input_pi_ptr[j] = item_p;
	}
}

#define SWP_FLASHI (norsf_info)
u32 swp_otto_get_flash_size(void)
{
	u32 ret_val = 0;
#if defined(CONFIG_OTTO_SNAF)
	spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
	ret_val = SNAF_PAGE_SIZE(snaf_info) * SNAF_NUM_OF_PAGE_PER_BLK(snaf_info) * SNAF_NUM_OF_BLOCK(snaf_info);
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	extern uint32_t onfi_chip_size(uint32_t idx);
	ret_val = onfi_chip_size(0);
	/* ToDo , multiple chip to be considered */
#elif defined(CONFIG_OTTO_SNOF)
	ret_val = SWP_FLASHI.num_chips * SWP_FLASHI.size_per_chip_b;
#endif
	return ret_val;
}

#if defined(CONFIG_OTTO_SNAF)
u32 swp_spi_nand_page_size(void)
{
	u32 ret_val;
	spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
	ret_val=SNAF_PAGE_SIZE(snaf_info);
	return ret_val;
}

u32 otto_get_nand_flash_block_size (void)
{
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    return SNAF_PAGE_SIZE(snaf_info) * SNAF_NUM_OF_PAGE_PER_BLK(snaf_info);
}
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
#endif /* #if defined(CONFIG_OTTO_SNAF) || defined(CONFIG_OTTO_PARALLEL_NAND_FLASH) */

#if CONFIG_ENABLE_BLOCK_ALIGNMEMT
#if defined(CONFIG_OTTO_SNAF) || defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
// if a partition has more than MAGIC_BLOCK_NUMBER blocks, than don't make adjust to next partition
#define MAGIC_BLOCK_NUMBER (5)
void swp_adjust_parts_alignment(swp_part_info_t *input_pi_ptr[], u32 num)
{
	u32 i;
	uint32_t adjust_len = 0;
	uint32_t block_len;
	block_len = otto_get_nand_flash_block_size();
	uint32_t is_first_adjust = 1;
	for (i = 1; i < num; ++i)
	{
		uint32_t adjust_base = input_pi_ptr[i]->base + adjust_len;
		DEBUG("BASE of %i is %x and adjust len is %x\n", i, input_pi_ptr[i]->base, adjust_len);
		if (adjust_base % block_len != 0)
		{
			adjust_base = ((adjust_base / block_len) + 1) * block_len;
			adjust_len = adjust_base - input_pi_ptr[i]->base;
		}
		if (i + 1 < num)
		{
			if ((input_pi_ptr[i + 1]->base - adjust_base) / block_len > MAGIC_BLOCK_NUMBER)
			{
				adjust_len = 0;
			}
		}
		if (input_pi_ptr[i]->base != adjust_base)
		{
			if (is_first_adjust)
			{
				printf("Adjust for block size 0x%x\n", block_len);
				is_first_adjust = 0;
			}
			printf("Adjust layout position of %s from 0x%04X to 0x%04X\n", input_pi_ptr[i]->name, input_pi_ptr[i]->base, adjust_base);
		}
		input_pi_ptr[i]->base = adjust_base;
		DEBUG("After adjustment is %x\n", input_pi_ptr[i]->base);
	}
}
#endif /* defined(CONFIG_OTTO_SNAF) || defined(CONFIG_OTTO_PARALLEL_NAND_FLASH) */
#endif /* CONFIG_ENABLE_BLOCK_ALIGNMEMT */

static inline void
removeTailComma(char *str)
{
	int len = strlen(str) - 1;
	if (len && ',' == str[len])
	{
		str[len] = '\0';
	}
}

void swp_part_info_init(void)
{
	u32 i, j;
	char *endp;
	(void)j;

	i = swp_otto_get_flash_size();
#ifdef CONFIG_OTTO_SNOF
	//j =  getenv_ulong("fl_size", 10, 0);
	char *val = env_get("fl_size");
	if (NULL == val)
	{
		j = 0;
	}
	else
	{
		j = simple_strtoul(env_get("fl_size"), &endp, 10);
	}

	if ((j != 0) && (j != i))
	{
		if ((j < i) && ((j == SWP_FL_8MB) || (j == SWP_FL_16MB)))
		{
			printf("INFO: flash size=%dMB, but %dMB layout is used.\n", (i >> 20), (j >> 20));
			i = j;
		}
		else
		{
			printf("ERROR: flash size=%dMB, %dMB layout unsupported.\n", (i >> 20), (j >> 20));
		}
	}
#elif defined(CONFIG_OTTO_SNAF)
	//j =  getenv_ulong("fl_size", 10, 0);
	char *val = env_get("fl_size");
	if (NULL == val)
	{
		j = 0;
	}
	else
	{
		j = simple_strtoul(env_get("fl_size"), &endp, 10);
	}
	if ((j != 0) && (j != i))
	{
		if ((j < i) && (j == SWP_FL_64MB))
		{
			printf("INFO: flash size=%dMB, but %dMB layout is used.\n", (i >> 20), (j >> 20));
			i = j;
		}
		else
		{
			printf("ERROR: flash size=%dMB, %dMB layout unsupported.\n", (i >> 20), (j >> 20));
		}
	}
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	j = getenv_ulong("fl_size", 10, 0);
	if ((j != 0) && (j != i))
	{
		if ((j < i) && (j == FL_64MB))
		{
			printf("INFO: flash size=%dMB, but %dMB layout is used.\n", (i >> 20), (j >> 20));
			i = j;
		}
		else
		{
			printf("ERROR: flash size=%dMB, %dMB layout unsupported.\n", (i >> 20), (j >> 20));
		}
	}
#endif
	switch (i)
	{
#ifdef CONFIG_OTTO_SNOF
	case SWP_FL_32MB:
		part_info[SWP_FL_CFGFS].base = SWP_FL_32MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_32MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_32MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_32MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_32MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_32MB;
		break;

	case SWP_FL_16MB:
		part_info[SWP_FL_CFGFS].base = SWP_FL_16MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_16MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_16MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_16MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_16MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_16MB;
		break;

	case SWP_FL_8MB:
		part_info[SWP_FL_CFGFS].base = SWP_FL_8MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_8MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_8MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_8MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_8MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_8MB;
		break;

	default:
		printf("INFO: layout for flash size=%dMB is not defined, %dMB layout is used.\n", (i >> 20), (SWP_FL_8MB >> 20));
		part_info[SWP_FL_CFGFS].base = SWP_FL_8MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_8MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_8MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_8MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_8MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_8MB;
		break;
#elif defined(CONFIG_OTTO_SNAF)
	case SWP_FL_64MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_64MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_64MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_64MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_64MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_64MB_R_SIZE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_64MB_CFG_ST_BASE;
#endif
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_64MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_64MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_64MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_64MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_64MB_R1_BASE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_64MB_CFG_ST_BASE;
#endif
		part_info[SWP_FL_OPT3].base = SWP_FL_64MB_OPT3_BASE;
		part_info[SWP_FL_OPT4].base = SWP_FL_64MB_OPT4_BASE;
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_64MB;
		break;

	case SWP_FL_128MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_128MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_128MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_128MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_128MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_128MB_R_SIZE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_128MB_CFG_ST_BASE;
#endif
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_128MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_128MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_128MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_128MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_128MB_R1_BASE;
		part_info[SWP_FL_OPT3].base = SWP_FL_128MB_OPT3_BASE;
		part_info[SWP_FL_OPT4].base = SWP_FL_128MB_OPT4_BASE;
		part_info[SWP_FL_APP].base = SWP_FL_128MB_APP_BASE;
		part_info[SWP_FL_DPI].base = SWP_FL_128MB_DPI_BASE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_128MB_CFG_ST_BASE;
#endif
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_128MB;
		break;

	case SWP_FL_256MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_256MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_256MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_256MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_256MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_256MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_256MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_256MB_R_SIZE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_256MB_CFG_ST_BASE;
#endif
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_256MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_256MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_256MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_256MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_256MB_R1_BASE;
		part_info[SWP_FL_OPT3].base = SWP_FL_256MB_OPT3_BASE;
		part_info[SWP_FL_OPT4].base = SWP_FL_256MB_OPT4_BASE;
		part_info[SWP_FL_APP].base = SWP_FL_256MB_APP_BASE;
		part_info[SWP_FL_DPI].base = SWP_FL_256MB_DPI_BASE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_256MB_CFG_ST_BASE;
#endif
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_256MB;
		break;

	case SWP_FL_512MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_512MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_512MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_512MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_512MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_512MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_512MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_512MB_R_SIZE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_512MB_CFG_ST_BASE;
#endif
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_512MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_512MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_512MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_512MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_512MB_R1_BASE;
		part_info[SWP_FL_APP].base = SWP_FL_512MB_APP_BASE;
		part_info[SWP_FL_DPI].base = SWP_FL_512MB_DPI_BASE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_512MB_CFG_ST_BASE;
#endif
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_512MB;
		break;

	default:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_128MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_128MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_64MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_64MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_64MB_R_SIZE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_128MB_CFG_ST_BASE;
#endif
		break;
#endif
		printf("INFO: layout for flash size=%dMB is not defined, %dMB layout is used.\n", (i >> 20), (SWP_FL_128MB >> 20));
		part_info[SWP_FL_CFGFS].base = SWP_FL_128MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_128MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_128MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_128MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_128MB_R1_BASE;
		part_info[SWP_FL_OPT3].base = SWP_FL_128MB_OPT3_BASE;
		part_info[SWP_FL_OPT4].base = SWP_FL_128MB_OPT4_BASE;
		part_info[SWP_FL_APP].base = SWP_FL_128MB_APP_BASE;
		part_info[SWP_FL_DPI].base = SWP_FL_128MB_DPI_BASE;
#ifdef CONFIG_STATIC_CONFIG
		part_info[SWP_FL_CFG_ST].base = SWP_FL_128MB_CFG_ST_BASE;
#endif
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_128MB;
		break;
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	case SWP_FL_64MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_64MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_64MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_64MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_64MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_64MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_64MB_R_SIZE;
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_64MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_64MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_64MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_64MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_64MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_64MB;
		break;

	case SWP_FL_128MB:
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_128MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_128MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_128MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_128MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_128MB_R_SIZE;
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_128MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_128MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_128MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_128MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_128MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_128MB;
		break;

	default:
		printf("INFO: layout for flash size=%dMB is not defined, %dMB layout is used.\n", (i >> 20), (SWP_FL_128MB >> 20));
#if defined(CONFIG_RTK_USE_ONE_UBI_DEVICE)
		part_info[SWP_FL_UBIDEV].base = SWP_FL_128MB_UBIDEV_BASE;
		part_info[SWP_FL_UBIDEV].size = SWP_FL_128MB_UBIDEV_SIZE;
		parts_size[SWP_FL_CFGFS].size = SWP_FL_128MB_CFGFS_SIZE;
		parts_size[SWP_FL_KERNEL1].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_KERNEL2].size = SWP_FL_128MB_K_SIZE;
		parts_size[SWP_FL_ROOTFS1].size = SWP_FL_128MB_R_SIZE;
		parts_size[SWP_FL_ROOTFS2].size = SWP_FL_128MB_R_SIZE;
		break;
#endif
		part_info[SWP_FL_CFGFS].base = SWP_FL_128MB_CFGFS_BASE;
		part_info[SWP_FL_KERNEL1].base = SWP_FL_128MB_K0_BASE;
		part_info[SWP_FL_ROOTFS1].base = SWP_FL_128MB_R0_BASE;
		part_info[SWP_FL_KERNEL2].base = SWP_FL_128MB_K1_BASE;
		part_info[SWP_FL_ROOTFS2].base = SWP_FL_128MB_R1_BASE;
		part_info[SWP_FL_END].base = SWP_FL_MAX_USE_128MB;
		break;
#endif
	}

	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		pi_ptr[i] = &part_info[i];
	}
	DEBUG("Original setting\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

	swp_sort_part_info(pi_ptr, ARRAY_SIZE(part_info));
	DEBUG("After sorting\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

#if CONFIG_ENABLE_BLOCK_ALIGNMEMT
#if defined(CONFIG_OTTO_SNAF) || defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	swp_adjust_parts_alignment(pi_ptr, ARRAY_SIZE(part_info));
	DEBUG("After Adjustment\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}
#endif
#endif

	/* Fill size */
	for (i = 0; i < (ARRAY_SIZE(part_info) - 1); i++)
	{
		pi_ptr[i]->size = pi_ptr[i + 1]->base - pi_ptr[i]->base;
	}
	DEBUG("After filling size\n");
	DEBUG("pi_ptr: name\tbase\tsize\n");
	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
	}

	DEBUG("Valid partitions\n");
	DEBUG("part_info: name\tbase\tsize\n");
	for (i = 0; i < ARRAY_SIZE(part_info); i++)
	{
		if (pi_ptr[i]->size != 0)
		{
			DEBUG("[%d]: %s\t0x%08x\t0x%08x\n", i, pi_ptr[i]->name, pi_ptr[i]->base, pi_ptr[i]->size);
		}
	}
}

#ifdef CONFIG_RTK_USE_ONE_UBI_DEVICE
static void
addPartsSizeInfo(struct parts_size_t parts[], int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (0 != parts[i].size && NULL != parts[i].name)
		{
			env_set_hex(parts[i].name, parts[i].size);
		}
	}
}

#define KERNEL_SIZE (6 * 1024 * 1024)
__attribute__((__unused__)) static void
addSingleUbiDeviceBoot(void)
{
	/* mtd and ubi partition */
#ifdef CONFIG_STATIC_CONFIG
	env_set("ubi_mtd", "4");
	env_set("root_mtd", "31:7");
#else
	env_set("ubi_mtd", "3");
	env_set("root_mtd", "31:6");
#endif
#if CONFIG_VOLUME_AUTO_RESIZE
	env_set("ubipart", "ubi part ${ubi_mtd_name} && ubi_resize_init");
#else
	env_set("ubipart", "ubi part ${ubi_mtd_name}");
#endif
	env_set("ubi_mtd_name", "ubi_device");
	env_set("ubi_device_img_name", "ubi_device_img.ubi");
	env_set_hex("fl_ubidevice", part_info[SWP_FL_UBIDEV].base);
	env_set_hex("fl_ubidevice_sz", part_info[SWP_FL_UBIDEV].size);
	env_set_hex("fl_kernel_sz", KERNEL_SIZE);
	/* boot */
	env_set("setmoreargs", "setenv more_args ubi.mtd=${ubi_mtd} root=${root_mtd} rootfs=squashfs");
	env_set("more_args", "ubi.mtd=${ubi_mtd} root=${root_mtd} rootfs=squashfs");
	env_set("set_commit0", "if itest.s ${sw_commit} != 0;then setenv sw_commit 0;saveenv; else true; fi");
#if CONFIG_VOLUME_AUTO_RESIZE
	env_set("updev", "setenv current_vol ubi_Config && if run check_vol; then run _updev_bk;  else run _updev; fi && if test \"${" VOL_UPDATED "}\" -eq \"1\"; then setenv " VOL_UPDATED " && saveenv; fi");
#else
	env_set("updev", "setenv current_vol ubi_Config && if run check_vol; then run _updev_bk;  else run _updev; fi");
#endif
	env_set("_updev", "tftp ${tftp_base} ${ubi_device_img_name} && nand erase ${fl_ubidevice} ${fl_ubidevice_sz} && nand write  ${fileaddr} ${fl_ubidevice} ${filesize} && run set_commit0");
	env_set("_updev_bk", "ubi read ${tftp_base} ubi_Config && ubi detach && setenv tftp_tmp ${tftp_base} &&  setenv tftp_base ${tftp_base_bk} && run _updev && setenv tftp_base ${tftp_tmp} && setenv tftp_tmp  && run ubipart && ubi write ${tftp_base} ubi_Config ${ubi_len_read}");
	env_set("upk", "setenv current_vol ubi_k0 && run check_vol && tftp ${tftp_base} uImage && ubi write ${tftp_base} ubi_k0 ${filesize}");
	env_set("upk1", "setenv current_vol ubi_k1 && run check_vol && tftp ${tftp_base} uImage && ubi write ${tftp_base} ubi_k1 ${filesize}");
	env_set("upr", "setenv current_vol ubi_r0 && run check_vol  && tftp ${tftp_base} rootfs && ubi write ${tftp_base} ubi_r0 ${filesize}");
	env_set("upr1", "setenv current_vol ubi_r1 && run check_vol  && tftp ${tftp_base} rootfs && ubi write ${tftp_base} ubi_r1 ${filesize}");
	env_set("setbootargs", "setenv bootargs ${bootargs_base} ${more_args} ${mtdparts} ${extra_args}");
	env_set("process0", "run ubipart && ubi read ${freeAddr} ubi_k0");
#ifdef CONFIG_STATIC_CONFIG
	env_set("ub0", "setenv root_mtd 31:7 && run process0 setmoreargs setbootargs;" THE_BOOT_CMD);
#else
	env_set("ub0", "setenv root_mtd 31:6 && run process0 setmoreargs setbootargs;" THE_BOOT_CMD);
#endif
	env_set("erase_apps", "setenv current_vol ubi_apps && run check_vol && ubi clean ubi_apps");
	env_set("erase_dpi", "setenv current_vol ubi_dpi && run check_vol && ubi clean ubi_dpi");
	env_set("erase_framework1", "setenv current_vol ubi_framework1 && run check_vol && ubi clean ubi_framework1");
	env_set("erase_framework2", "setenv current_vol ubi_framework2 && run check_vol && ubi clean ubi_framework2");
	env_set("check_framework", "setenv current_vol ubi_framework1 && run check_vol && setenv current_vol ubi_framework2 && run check_vol");
	env_set("upframework", "run check_framework && tftp ${tftp_base} framework.img && ubi write ${tftp_base} ubi_framework1 ${filesize} && ubi write ${tftp_base} ubi_framework2 ${filesize}");
	env_set("process1", "run ubipart && ubi read ${freeAddr} ubi_k1");
#ifdef CONFIG_STATIC_CONFIG
	env_set("ub1", "setenv root_mtd 31:9 && run process1 setmoreargs setbootargs;" THE_BOOT_CMD);
#else
	env_set("ub1", "setenv root_mtd 31:8 && run process1 setmoreargs setbootargs;" THE_BOOT_CMD);
#endif
	env_set("set_act0", "if itest.s ${sw_active} != 0;then setenv sw_active 0;saveenv;fi");
	env_set("set_act1", "if itest.s ${sw_active} != 1;then setenv sw_active 1;saveenv;fi");
	env_set("boot_by_commit", "if itest.s ${sw_commit} == 0;then run set_act0;run ub0;else run set_act1;run ub1;fi");
	env_set("boot_by_tryactive", "if itest.s ${sw_tryactive} == 0;then setenv sw_tryactive 2;setenv sw_active 0;saveenv;run en_wdt;run ub0;else setenv sw_tryactive 2;setenv sw_active 1;saveenv;run en_wdt;run ub1;fi");
	env_set("bootcmd", "if itest.s ${sw_tryactive} == 2; then run boot_by_commit;else run boot_by_tryactive;fi");

	/* clean config */
	env_set("cfg_name", "ubi_Config");
	env_set("erase_cfgfs", "setenv current_vol ${cfg_name} && run check_vol && ubi clean ${cfg_name}");

	/* check volume and set ubi parition */
	env_set("chk_volume", "ubi check ${current_vol}");
	env_set("check_vol", "if run chk_volume; then ; else if run ubipart; then if run chk_volume; then ; else echo ${current_vol} no exist; false; fi; fi; fi");

	/* add partition size info */
	addPartsSizeInfo(parts_size, sizeof(parts_size) / sizeof(parts_size[0]));

	/* creaet ubi volume */
	env_set("create_ubi_config", "setenv current_vol ${cfg_name} && run check_vol ;if run chk_volume  ; then ; else ; ubi create ${cfg_name} ${fl_cfgfs_sz} dynamic; fi;");
	env_set("create_k0", "setenv current_vol ubi_k0 && run check_vol ; if run chk_volume; then ; else  ubi create ubi_k0 ${fl_kernel1_sz} dynamic; fi;");
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("create_k1", "setenv current_vol ubi_k1 && run check_vol ; if run chk_volume; then ; else ubi create ubi_k1 ${fl_kernel2_sz} dynamic; fi;");
#endif
	env_set("create_r0", "setenv current_vol ubi_r0 && run check_vol ; if run chk_volume; then ; else ubi create ubi_r0 ${fl_rootfs1_sz} dynamic; fi;");
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("create_r1", "setenv current_vol ubi_r1 && run check_vol ; if run chk_volume; then ; else ubi create ubi_r1 ${fl_rootfs2_sz} dynamic; fi;");
#endif
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("create_ubi_device", "run create_ubi_config && run create_k0 && run create_r0 && run create_k1 && run create_r1");
#else
	env_set("create_ubi_device", "run create_ubi_config && run create_k0 && run create_r0");
#endif

	/* Update bootdelay timing*/
	env_set("bootdelay", "1");

	/* clean some variable*/
	env_set("b0", "");
	env_set("b1", "");
	env_set("fl_cfgfs", "");   // in ubi device
	env_set("fl_kernel1", ""); // in ubi device
	env_set("fl_kernel2", ""); // in ubi device
	env_set("fl_rootfs1", ""); // in ubi device
	env_set("fl_rootfs2", ""); // in ubi device
}
#endif

#define YFUNC_INIT "loady 80000000 && cp.b 80000000 81000000 ${filesize} && cmp.b 80000000 81000000 ${filesize} "
__attribute__((__unused__)) static void
addOttoDefault(void)
{
#ifdef CONFIG_OTTO_SNOF
	env_set("erase_cfgfs", "sf erase ${fl_cfgfs} +${fl_cfgfs_sz}");
	env_set("erase_env", "sf erase ${fl_env} +${fl_env_sz};sf erase ${fl_env2} +${fl_env_sz}");
	env_set("upb", "tftp ${tftp_base} plr.img && crc32 ${fileaddr} ${filesize} && sf probe && sf erase 0 +${fl_boot_sz} && sf write ${fileaddr} 0 ${filesize}");
	env_set("upk", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && sf probe && sf erase ${fl_kernel1} +${fl_kernel1_sz} && sf write ${fileaddr} ${fl_kernel1} ${filesize}");
	env_set("upr", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && sf probe && sf erase ${fl_rootfs1} +${fl_rootfs1_sz} && sf write ${fileaddr} ${fl_rootfs1} ${filesize}");
	env_set("yu", YFUNC_INIT "&& sf probe && sf erase 0 +${fl_boot_sz} && sf write 80000000 0 ${filesize}");
	env_set("yk", YFUNC_INIT "&& sf probe && sf erase ${fl_kernel1} +${fl_kernel1_sz} && sf write 80000000 ${fl_kernel1} ${filesize}");
	env_set("yr", YFUNC_INIT "&& sf probe && sf erase ${fl_rootfs1} +${fl_rootfs1_sz} && sf write 80000000 ${fl_rootfs1} ${filesize}");
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("upk1", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && sf probe && sf erase ${fl_kernel2} +${fl_kernel2_sz} && sf write ${fileaddr} ${fl_kernel2} ${filesize}");
	env_set("upr1", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && sf probe && sf erase ${fl_rootfs2} +${fl_rootfs2_sz} && sf write ${fileaddr} ${fl_rootfs2} ${filesize}");
	env_set("bootargs_base", "console=ttyS0,115200");
	env_set("b0", "setenv bootargs ${bootargs_base} ${mtdparts0} ${rst2dfl_flg}; bootm ${img0_kernel}");
	env_set("b1", "setenv bootargs ${bootargs_base} ${mtdparts1} ${rst2dfl_flg}; bootm ${img1_kernel}");
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#elif defined(CONFIG_OTTO_SNAF)
	env_set("erase_cfgfs", "nand erase ${fl_cfgfs} ${fl_cfgfs_sz}");
	env_set("erase_env", "nand erase ${fl_env} ${fl_env_sz} && nand erase ${fl_env2} ${fl_env_sz}");
	if (2048 == swp_spi_nand_page_size())
	{
#ifdef CONFIG_FIT
        env_set("upb", "tftp ${tftp_base} encode_uboot_auth.itb && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
#else
		env_set("upb", "tftp ${tftp_base} encode_uboot.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
#endif
        env_set("yu", YFUNC_INIT "&& nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
	}
	else if (4096 == swp_spi_nand_page_size())
	{
#ifdef CONFIG_FIT
        env_set("upb", "tftp ${tftp_base} encode_uboot_auth_4Kpage.itb && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
#else
		env_set("upb", "tftp ${tftp_base} encode_uboot_4Kpage.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 1080 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
#endif
        env_set("yu", YFUNC_INIT "&& nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 1080 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
	}
	else
	{
		printf("Unknown Page size, take care the uboot image name!\n");
		env_set("upb", "tftp ${tftp_base} encode_uboot.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
	}
	env_set("upk", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && nand erase ${fl_kernel1} ${fl_kernel1_sz} && nand write ${fileaddr} ${fl_kernel1} ${filesize}");
	env_set("upr", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && nand erase ${fl_rootfs1} ${fl_rootfs1_sz} && nand write ${fileaddr} ${fl_rootfs1} ${filesize}");
	env_set("yk", YFUNC_INIT "&& nand erase ${fl_kernel1} ${fl_kernel1_sz} && nand write 80000000 ${fl_kernel1} ${filesize}");
	env_set("yr", YFUNC_INIT "&& nand erase ${fl_rootfs1} ${fl_rootfs1_sz} && nand write 80000000 ${fl_rootfs1} ${filesize}");
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("upk1", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && nand erase ${fl_kernel2} ${fl_kernel2_sz} && nand write ${fileaddr} ${fl_kernel2} ${filesize}");
	env_set("upr1", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && nand erase ${fl_rootfs2} ${fl_rootfs2_sz} && nand write ${fileaddr} ${fl_rootfs2} ${filesize}");
	env_set("bootargs_base", "console=ttyS0,115200");
	env_set("b0", "setenv bootargs ${bootargs_base} ${mtdparts0};nand read ${freeAddr} ${fl_kernel1} ${fl_kernel1_sz};bootm ${freeAddr}");
	env_set("b1", "setenv bootargs ${bootargs_base} ${mtdparts1};nand read ${freeAddr} ${fl_kernel2} ${fl_kernel2_sz};bootm ${freeAddr}");
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	env_set("erase_cfgfs", "nand erase ${fl_cfgfs} ${fl_cfgfs_sz}");
	env_set("erase_env", "nand erase ${fl_env} ${fl_env_sz};nand erase ${fl_env2} ${fl_env_sz}");
	if (2048 == onfi_page_size())
	{
		env_set("upb", "tftp ${tftp_base} pnd_btldr_ecc.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
        env_set("yu", "loady 80000000 && cp.b 80000000 81000000 ${filesize} && cmp.b 80000000 81000000 ${filesize} && nand erase 0 ${filesize} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr}");
	}
	if (4096 == onfi_page_size())
	{
		env_set("upb", "tftp ${tftp_base} pnd_btldr_ecc_4Kpage.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 1080 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
        env_set("yu", "loady 80000000 && cp.b 80000000 81000000 ${filesize} && cmp.b 80000000 81000000 ${filesize} && nand erase 0 ${filesize} && setexpr pagecnt ${filesize} / 1080 && nand write.raw 80000000 0 ${filesize}");
	}
	else
	{
		printf("Unknown Page size, take care the uboot image name!\n");
		env_set("upb", "tftp ${tftp_base} pnd_btldr_ecc.img && crc32 ${fileaddr} ${filesize} && nand erase 0x0 ${fl_boot_sz} && setexpr pagecnt ${filesize} / 840 && nand write.raw ${fileaddr} 0x0 ${pagecnt}");
	}
	env_set("upk", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && nand erase ${fl_kernel1} ${fl_kernel1_sz} && nand write ${fileaddr} ${fl_kernel1} ${filesize}");
	env_set("upr", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && nand erase ${fl_rootfs1} ${fl_rootfs1_sz} && nand write ${fileaddr} ${fl_rootfs1} ${filesize}");
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set("upk1", "tftp ${tftp_base} uImage && crc32 ${fileaddr} ${filesize} && nand erase ${fl_kernel2} ${fl_kernel2_sz} && nand write ${fileaddr} ${fl_kernel2} ${filesize}");
	env_set("upr1", "tftp ${tftp_base} rootfs && crc32 ${fileaddr} ${filesize} && nand erase ${fl_rootfs2} ${fl_rootfs2_sz} && nand write ${fileaddr} ${fl_rootfs2} ${filesize}");
	env_set("bootargs_base", "console=ttyS0,115200");
	env_set("b0", "setenv bootargs ${bootargs_base} ${mtdparts0};nand read 0x82000000 ${fl_kernel1} ${fl_kernel1_sz};bootm 0x82000000");
	env_set("b1", "setenv bootargs ${bootargs_base} ${mtdparts1};nand read 0x82000000 ${fl_kernel2} ${fl_kernel2_sz};bootm 0x82000000");
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#endif
	if ((part_info[SWP_FL_OPT3].base != 0) && (part_info[SWP_FL_OPT3].size != 0) && (part_info[SWP_FL_OPT4].base != 0) && (part_info[SWP_FL_OPT4].size != 0))
	{
		env_set_hex("fl_framework1", part_info[SWP_FL_OPT3].base);
		env_set_hex("fl_framework1_sz", part_info[SWP_FL_OPT3].size);
		env_set_hex("fl_framework2", part_info[SWP_FL_OPT4].base);
		env_set_hex("fl_framework2_sz", part_info[SWP_FL_OPT4].size);
		env_set("upframework", "tftp ${tftp_base} framework.img && nand erase ${fl_framework1} ${fl_framework1_sz} && nand erase ${fl_framework2} ${fl_framework2_sz} && nand write ${tftp_base} ${fl_framework1} ${filesize} && nand write ${tftp_base} ${fl_framework2} ${filesize}");
		env_set("erase_framework", " nand erase ${fl_framework1} ${fl_framework1_sz} && nand erase ${fl_framework2} ${fl_framework2_sz}");
	}
	if((part_info[SWP_FL_APP].base!=0) && (part_info[SWP_FL_APP].size!=0))
	{
		env_set_hex("fl_app", part_info[SWP_FL_APP].base);
		env_set_hex("fl_app_sz", part_info[SWP_FL_APP].size);
		env_set("erase_apps", " nand erase ${fl_app} ${fl_app_sz}");
	}
	if((part_info[SWP_FL_DPI].base!=0) && (part_info[SWP_FL_DPI].size!=0))
	{
		env_set_hex("fl_dpi", part_info[SWP_FL_DPI].base);
		env_set_hex("fl_dpi_sz", part_info[SWP_FL_DPI].size);
		env_set("erase_dpi", " nand erase ${fl_dpi} ${fl_dpi_sz}");
	}
}

void swp_otto_flash_layout_init(void)
{
    printf("II: Configurate otto_flash_layout\n");

	swp_part_info_init();

	env_set_hex("fl_boot_sz", part_info[SWP_FL_BOOT].size);
	env_set_hex("fl_env", part_info[SWP_FL_ENV].base);
	env_set_hex("fl_env_sz", part_info[SWP_FL_ENV].size);
	env_set_hex("fl_env2", part_info[SWP_FL_ENV2].base);
	env_set_hex("fl_cfgfs", part_info[SWP_FL_CFGFS].base);
	env_set_hex("fl_cfgfs_sz", part_info[SWP_FL_CFGFS].size);
	env_set_hex("fl_kernel1", part_info[SWP_FL_KERNEL1].base);
	env_set_hex("fl_kernel1_sz", part_info[SWP_FL_KERNEL1].size);
	env_set_hex("fl_rootfs1", part_info[SWP_FL_ROOTFS1].base);
	env_set_hex("fl_rootfs1_sz", part_info[SWP_FL_ROOTFS1].size);
#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set_hex("fl_kernel2", part_info[SWP_FL_KERNEL2].base);
	env_set_hex("fl_kernel2_sz", part_info[SWP_FL_KERNEL2].size);
	env_set_hex("fl_rootfs2", part_info[SWP_FL_ROOTFS2].base);
	env_set_hex("fl_rootfs2_sz", part_info[SWP_FL_ROOTFS2].size);
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#ifdef CONFIG_OTTO_SNOF
	env_set_hex("img0_kernel", (NORSF_CFLASH_BASE + part_info[SWP_FL_KERNEL1].base));

#ifdef CONFIG_LUNA_MULTI_BOOT
	env_set_hex("img1_kernel", (NORSF_CFLASH_BASE + part_info[SWP_FL_KERNEL2].base));
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#endif /* #ifdef CONFIG_SPINOR_FLASH */

	addOttoDefault();
#ifdef CONFIG_RTK_USE_ONE_UBI_DEVICE
	addSingleUbiDeviceBoot();
#endif
#if defined(CONFIG_STATIC_CONFIG)
	env_set_hex("fl_cfg_st", part_info[SWP_FL_CFG_ST].base);
	env_set_hex("fl_cfg_st_sz", part_info[SWP_FL_CFG_ST].size);
#if defined(CONFIG_OTTO_SNAF)
	env_set("erase_cfg_st", "nand erase ${fl_cfg_st} ${fl_cfg_st_sz}");
#elif defined(CONFIG_OTTO_PARALLEL_NAND_FLASH)
	env_set("erase_cfg_st", "nand erase ${fl_cfg_st} ${fl_cfg_st_sz}");
#endif
#endif

#ifdef CONFIG_OTTO_FL_TO_MTDPARTS
    char str_tmp[256] = {0};
#ifdef CONFIG_CMD_MTDPARTS
	sprintf(str_tmp, "nand0=%s", SWP_RTK_MTD_DEV_NAME);
	env_set("mtdids", str_tmp);
	swp_flash_layout_to_mtdparts(str_tmp, 0, 1);
	removeTailComma(str_tmp);
	//printf("mtdparts=%s\n", str_tmp);
	env_set("mtdparts", str_tmp);
#endif /* #ifdef CONFIG_CMD_MTDPARTS */
	swp_flash_layout_to_mtdparts(str_tmp, 0, 0);
	removeTailComma(str_tmp);
	//printf("mtdparts0=%s\n", str_tmp);
	env_set("mtdparts0", str_tmp);
#ifdef CONFIG_LUNA_MULTI_BOOT
	swp_flash_layout_to_mtdparts(str_tmp, 1, 0);
	removeTailComma(str_tmp);
	//printf("mtdparts1=%s\n", str_tmp);
	env_set("mtdparts1", str_tmp);
#endif /* #ifdef CONFIG_LUNA_MULTI_BOOT */
#endif /* #ifdef CONFIG_OTTO_FL_TO_MTDPARTS */

    env_save();
}

PATCH_REG(swp_otto_flash_layout_init, 6);
