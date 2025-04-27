/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * $Revision: 78657 $
 * $Date: 2017-05-11 18:28:49 +0800 (Thu, 11 May 2017) $
 *
 */
#include <common/rt_type.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/rtk_flash_common.h>


struct rtk_partition_info_t rtk_flash_partition_size[4] = {
     [0] = {
         .flash_total_size = FLASH_SIZE_16MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00680000U),
         .kernel2_offset = (0x00980000U),
         .kernel2_size = (0x00680000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     },
     [1] = {
         .flash_total_size = FLASH_SIZE_32MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00e80000U),
         .kernel2_offset = (0x01180000U),
         .kernel2_size = (0x00e80000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x01d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     },
     [2] = {
         .flash_total_size = FLASH_SIZE_64MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x01e80000U),
         .kernel2_offset = (0x02180000U),
         .kernel2_size = (0x01e80000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x03d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
      },
      [3] = {
          .flash_total_size = FLASH_SIZE_128MB,
#if defined(CONFIG_DUAL_IMAGE)
          .loader_offset = (0x00000000U),
          .loader_size = (UNIT_SIZE*14),
          .bdinfo_offset = (0x000e0000U),
          .bdinfo_size = LOADER_BDINFO_SIZE,
          .sysinfo_offset = (0x000f0000U),
          .sysinfo_size = SYSINFO_SIZE,
          .jffs2_cfg_offset = (0x00100000U),
          .jffs2_cfg_size = (UNIT_SIZE*16),
          .jffs2_log_offset = (0x00200000U),
          .jffs2_log_size = (UNIT_SIZE*16),
          .kernel_offset = (0x00300000U),
          .kernel_size = (0x03e80000U),
          .kernel2_offset = (0x04180000U),
          .kernel2_size = (0x03e80000U)
#else
          .loader_offset = (0x00000000U),
          .loader_size = (UNIT_SIZE*14),
          .bdinfo_offset = (0x000e0000U),
          .bdinfo_size = LOADER_BDINFO_SIZE,
          .sysinfo_offset = (0x000f0000U),
          .sysinfo_size = SYSINFO_SIZE,
          .jffs2_cfg_offset = (0x00100000U),
          .jffs2_cfg_size = (UNIT_SIZE*16),
          .jffs2_log_offset = (0x00200000U),
          .jffs2_log_size = (UNIT_SIZE*16),
          .kernel_offset = (0x00300000U),
          .kernel_size = (0x07d00000U),
          .kernel2_offset = (0x0U),
          .kernel2_size = (0x0U)
#endif
     }
 };

#if defined(CONFIG_FLASH_LAYOUT_TYPE5)
 struct mtd_partition rtk_sdk_parts[RTK_MTD_PARTITION_NUM] = {
     [FLASH_INDEX_LOADER] = {
         .name = "LOADER",
         .offset = 0,
         .size = 0,
         .mask_flags = MTD_WRITEABLE /* force a read-only partition */
     },
     [FLASH_INDEX_LOADER_BDINFO] = {
         .name = "BDINFO",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     },
     [FLASH_INDEX_SYSINFO] = {
         .name = "SYSINFO",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     },
     [FLASH_INDEX_JFFS2_CFG] = {
         .name = "JFFS2 CFG",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     },
     [FLASH_INDEX_JFFS2_LOG] = {
         .name = "JFFS2 LOG",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     },
     [FLASH_INDEX_KERNEL] = {
         .name = "RUNTIME",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     },
#if defined(CONFIG_DUAL_IMAGE)
     [FLASH_INDEX_KERNEL2] = {
         .name = "RUNTIME2",
         .offset = 0,
         .size = 0,
         .mask_flags = 0
     }
#endif /* CONFIG_DUAL_IMAGE */
 };
#endif /* CONFIG_FLASH_LAYOUT_TYPE5 */

unsigned int rtk_flash_partition_size_get(unsigned int partition_idx)
{
    unsigned int	size;

    switch(partition_idx){
        case FLASH_INDEX_LOADER:			/* LOADER */
        case FLASH_INDEX_LOADER_BDINFO: 	/* LOADER_BDINFO */
        case FLASH_INDEX_SYSINFO:			/* SYSINFO */
        case FLASH_INDEX_JFFS2_CFG: 		/* JFFS2_CFG */
        case FLASH_INDEX_JFFS2_LOG: 		/* JFFS2_LOG */
        case FLASH_INDEX_KERNEL:			/* KERNEL */
#if defined(CONFIG_DUAL_IMAGE)
        case FLASH_INDEX_KERNEL2:			/* KERNEL2 */
#endif
            size = rtk_sdk_parts[partition_idx].size;
            break;
        default:
            size = 0;
            break;
    }
    return size;
}

unsigned int rtk_flash_partition_offset_get(unsigned int partition_idx)
{
    unsigned int	offset;
    switch(partition_idx){
        case FLASH_INDEX_LOADER:			/* LOADER */
        case FLASH_INDEX_LOADER_BDINFO: 	/* LOADER_BDINFO */
        case FLASH_INDEX_SYSINFO:			/* SYSINFO */
        case FLASH_INDEX_JFFS2_CFG: 		/* JFFS2_CFG */
        case FLASH_INDEX_JFFS2_LOG: 		/* JFFS2_LOG */
        case FLASH_INDEX_KERNEL:			/* KERNEL */
#if defined(CONFIG_DUAL_IMAGE)
        case FLASH_INDEX_KERNEL2:			/* KERNEL2 */
#endif
            offset = rtk_sdk_parts[partition_idx].offset;
            break;
        default:
            offset = 0;
            break;
    }
    return offset;
}

int rtk_flash_fill_mtd_table(unsigned int total_size)
{
    unsigned int partition_idx;
    unsigned int size_idx = SIZE_IDX_NOT_FOUND;
    unsigned int loop_idx;

    for(loop_idx = 0; loop_idx < (sizeof(rtk_flash_partition_size)/sizeof(rtk_flash_partition_size[0])); loop_idx++)
    {
        if(rtk_flash_partition_size[loop_idx].flash_total_size == total_size){
            size_idx = loop_idx;
        }
    }

    if(size_idx == SIZE_IDX_NOT_FOUND)
        return -1;

    for(partition_idx = 0; partition_idx < RTK_MTD_PARTITION_NUM; partition_idx++)
    {
        switch(partition_idx){
            case FLASH_INDEX_LOADER:			/* LOADER */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].loader_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].loader_size;
                break;
            case FLASH_INDEX_LOADER_BDINFO:		/* LOADER_BDINFO */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].bdinfo_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].bdinfo_size;
                break;
            case FLASH_INDEX_SYSINFO:			/* SYSINFO */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].sysinfo_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].sysinfo_size;
                break;
            case FLASH_INDEX_JFFS2_CFG:			/* JFFS2_CFG */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].jffs2_cfg_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].jffs2_cfg_size;
                break;
            case FLASH_INDEX_JFFS2_LOG:			/* JFFS2_LOG */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].jffs2_log_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].jffs2_log_size;
                break;
            case FLASH_INDEX_KERNEL:			/* KERNEL */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].kernel_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].kernel_size;
                break;
#if defined(CONFIG_DUAL_IMAGE)
            case FLASH_INDEX_KERNEL2:			/* KERNEL2 */
                rtk_sdk_parts[partition_idx].offset = rtk_flash_partition_size[size_idx].kernel2_offset;
                rtk_sdk_parts[partition_idx].size = rtk_flash_partition_size[size_idx].kernel2_size;
                break;
#endif
            default:
                break;
        }
    }
    return 0;
}

EXPORT_SYMBOL(rtk_flash_partition_offset_get);
EXPORT_SYMBOL(rtk_flash_partition_size_get);


