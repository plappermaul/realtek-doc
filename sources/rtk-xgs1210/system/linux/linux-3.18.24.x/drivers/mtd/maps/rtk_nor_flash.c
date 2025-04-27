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
 * $Revision: 9021 $
 * $Date: 2010-04-13 15:45:15 +0800 (Tue, 13 Apr 2010) $
 *
 */

#undef _DEBUG_

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
//#include <linux/config.h>
#include <linux/autoconf.h>

#include <linux/mtd/rtk_norflash.h>

static struct mtd_info *rtk_mtd;

struct map_info rtk_map = {
    .name = "Physically mapped flash",
    .size = WINDOW_SIZE,
    .bankwidth = BUSWIDTH,
    .phys = WINDOW_ADDR,
};

#if defined(CONFIG_FLASH_LAYOUT_TYPE1)
static struct mtd_partition rtk_parts[] = {
    [0] = {
        .name = "LOADER",
        .offset = LOADER_START,
        .size = LOADER_SIZE,
        .mask_flags = MTD_WRITEABLE //force a read-only partition
    },
    [1] = {
        .name = "BDINFO",
        .offset = LOADER_BDINFO_START,
        .size = LOADER_BDINFO_SIZE,
        .mask_flags = 0
    },
    [2] = {
        .name = "JFFS2",
        .offset = JFFS2_START,
        .size = JFFS2_SIZE,
        .mask_flags = 0
    },
    [3] = {
        .name = "RUNTIME",
        .offset = KERNEL_START,
        .size = KERNEL_SIZE,
        .mask_flags = 0
    },
  #if defined(CONFIG_DUAL_IMAGE)
    [4] = {
        .name = "RUNTIME2",
        .offset = KERNEL2_START,
        .size = KERNEL2_SIZE,
        .mask_flags = 0
    },
    [5] = {
        .name = "SYSINFO",
        .offset = SYSINFO_START,
        .size = SYSINFO_SIZE,
        .mask_flags = 0
    },
  #endif /* CONFIG_DUAL_IMAGE */
};

#elif defined(CONFIG_FLASH_LAYOUT_TYPE2)
static struct mtd_partition rtk_parts[] = {
    [0] = {
        .name = "LOADER",
        .offset = LOADER_START,
        .size = LOADER_SIZE,
        .mask_flags = MTD_WRITEABLE //force a read-only partition
    },
    [1] = {
        .name = "BDINFO",
        .offset = LOADER_BDINFO_START,
        .size = LOADER_BDINFO_SIZE,
        .mask_flags = 0
    },
    [2] = {
        .name = "SYSINFO",
        .offset = SYSINFO_START,
        .size = SYSINFO_SIZE,
        .mask_flags = 0
    },
    [3] = {
        .name = "JFFS2",
        .offset = JFFS2_START,
        .size = JFFS2_SIZE,
        .mask_flags = 0
    },
    [4] = {
        .name = "RUNTIME",
        .offset = KERNEL_START,
        .size = KERNEL_SIZE,
        .mask_flags = 0
    },
  #if defined(CONFIG_DUAL_IMAGE)
    [5] = {
        .name = "RUNTIME2",
        .offset = KERNEL2_START,
        .size = KERNEL2_SIZE,
        .mask_flags = 0
    }
  #endif /* CONFIG_DUAL_IMAGE */
};
#elif defined(CONFIG_FLASH_LAYOUT_TYPE3) || defined(CONFIG_FLASH_LAYOUT_TYPE4) || defined(CONFIG_FLASH_LAYOUT_TYPE5)
static struct mtd_partition rtk_parts[] = {
    [0] = {
        .name = "LOADER",
        .offset = LOADER_START,
        .size = LOADER_SIZE,
        .mask_flags = MTD_WRITEABLE /* force a read-only partition */
    },
    [1] = {
        .name = "BDINFO",
        .offset = LOADER_BDINFO_START,
        .size = LOADER_BDINFO_SIZE,
        .mask_flags = 0
    },
    [2] = {
        .name = "SYSINFO",
        .offset = SYSINFO_START,
        .size = SYSINFO_SIZE,
        .mask_flags = 0
    },
    [3] = {
        .name = "JFFS2 CFG",
        .offset = JFFS2_CFG_START,
        .size = JFFS2_CFG_SIZE,
        .mask_flags = 0
    },
    [4] = {
        .name = "JFFS2 LOG",
        .offset = JFFS2_LOG_START,
        .size = JFFS2_LOG_SIZE,
        .mask_flags = 0
    },
    [5] = {
        .name = "RUNTIME",
        .offset = KERNEL_START,
        .size = KERNEL_SIZE,
        .mask_flags = 0
    },
#if defined(CONFIG_DUAL_IMAGE)
    [6] = {
        .name = "RUNTIME2",
        .offset = KERNEL2_START,
        .size = KERNEL2_SIZE,
        .mask_flags = 0
    }
#endif /* CONFIG_DUAL_IMAGE */
};
#endif /* CONFIG_FLASH_LAYOUT_TYPE3 */


static int __init init_rtk_map(void)
{
    printk(KERN_NOTICE "flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);

    rtk_map.virt = (void *) WINDOW_ADDR;

    if (!rtk_map.virt) {
        printk("Failed to ioremap\n");
        return -EIO;
    }



    rtk_mtd = do_map_probe("cfi_probe", &rtk_map);

    if (rtk_mtd) {
        rtk_mtd->owner = THIS_MODULE;
        add_mtd_partitions(rtk_mtd, rtk_parts, sizeof(rtk_parts)/sizeof(rtk_parts[0]));
        return 0;
    }
    return -ENXIO;
}

static void __exit cleanup_rtk_map(void)
{
    if (rtk_mtd) {
        del_mtd_partitions(rtk_mtd);
        map_destroy(rtk_mtd);
    }
    if (rtk_map.virt) {
        iounmap((void *)rtk_map.virt);
        rtk_map.map_priv_1 = 0;
    }
}

MODULE_LICENSE("GPL");
module_init(init_rtk_map);
module_exit(cleanup_rtk_map);

