/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 58581 $
 * $Date: 2015-05-12 15:02:48 +0800 (Tue, 12 May 2015) $
 *
 */

#ifndef _RTK_FLASH_COMMON_H
#define _RTK_FLASH_COMMON_H
#ifndef __UBOOT__
#include <common/rt_autoconf.h>
#endif /*__UBOOT__*/

#ifndef FLASH_BASE
#define FLASH_BASE 0xBD000000
#endif

#define SPI_FLASH_BASE 0xb4000000

#define UNIT_SIZE 65536  //only for this file

#define FLASH_SIZE_8MB		(0x00800000U)
#define FLASH_SIZE_16MB		(0x01000000U)
#define FLASH_SIZE_32MB		(0x02000000U)
#define FLASH_SIZE_64MB		(0x04000000U)
#define FLASH_SIZE_128MB	(0x08000000U)

#define SIZE_IDX_NOT_FOUND	0xffffffff

#ifdef CONFIG_DUAL_IMAGE
#define RTK_MTD_PARTITION_NUM		9
#else
#define RTK_MTD_PARTITION_NUM		6
#endif

/* The LOADER_BDINFO_SIZE & SYSINFO_SIZE is assigned by MACRO to suit
    the original U-Boot design and no need to modify the U-Boot common source code*/
/*
#define LOADER_BDINFO_SIZE			UNIT_SIZE
#define SYSINFO_SIZE 				UNIT_SIZE*/

struct rtk_partition_info_t{
    unsigned int  flash_total_size;
    unsigned int  loader_offset;
    unsigned int  loader_size;
    unsigned int  env_offset;
    unsigned int  env_size;
    unsigned int  env2_offset;
    unsigned int  env2_size;
    unsigned int  bdinfo_offset;
    unsigned int  bdinfo_size;
    unsigned int  jffs2_offset;
    unsigned int  jffs2_size;
    unsigned int  kernel_offset;
    unsigned int  kernel_size;
    unsigned int  rootfs_offset;
    unsigned int  rootfs_size;
#ifdef CONFIG_DUAL_IMAGE
    unsigned int  kernel2_offset;
    unsigned int  kernel2_size;
    unsigned int  rootfs2_offset;
    unsigned int  rootfs2_size;
#endif
};

typedef enum rtk_flash_partition_idx_e
{
    FLASH_INDEX_LOADER = 0,
    FLASH_INDEX_LOADER_ENV,
    FLASH_INDEX_LOADER_ENV2,
    FLASH_INDEX_BDINFO,
    FLASH_INDEX_JFFS2,
    FLASH_INDEX_KERNEL,
    FLASH_INDEX_ROOTFS,
#ifdef CONFIG_DUAL_IMAGE
    FLASH_INDEX_KERNEL2,
    FLASH_INDEX_ROOTFS2,
#endif
    FLASH_INDEX_END,
}rtk_flash_partition_idx_t;

extern unsigned int rtk_flash_partition_offset_get(unsigned int partition_idx);
extern unsigned int rtk_flash_partition_size_get(unsigned int partition_idx);
extern int rtk_flash_fill_mtd_table(unsigned int total_size);

#if defined(__UBOOT__)
#define LOADER_ENV_START (unsigned int)flash_partition_addr_ret(FLASH_INDEX_LOADER_ENV)
#define BDINFO_START     (unsigned int)flash_partition_addr_ret(FLASH_INDEX_BDINFO)

extern unsigned int flash_partition_size_ret(rtk_flash_partition_idx_t partition);
extern unsigned int flash_partition_addr_ret(rtk_flash_partition_idx_t partition);
#endif

#endif /*_RTK_FLASH_COMMON_H*/

