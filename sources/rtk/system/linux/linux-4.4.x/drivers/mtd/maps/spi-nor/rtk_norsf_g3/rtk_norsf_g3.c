#include <common/rt_type.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/root_dev.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <asm/cacheflush.h>

/* Included for LOADER_START, etc. */
#include <linux/mtd/rtk_flash_common.h>
#include "chip_probe.h"

#define BUSWIDTH 2

#define SPI_NOR_FLASH_START_ADDR (0xb4000000)

static struct mtd_info *luna_nor_spi_mtd = NULL;

struct map_info luna_nor_spi_map = {
 name:      "rtk_norsf_g3",
 bankwidth: BUSWIDTH,
 phys:      SPI_NOR_FLASH_START_ADDR,
};


typedef enum {
    SPI_T        = 0x0,
    I2C_EEPROM_T = 0x1,
    NAND_T       = 0x2
} FLASH_TYPE_T;

typedef enum {
    cache_l1     = 0x0,
    cache_l2     = 0x1,
    no_cache     = 0x2
} cache_type_t;

#define DELIMITER_LINE "=================================================================\n"

extern int __init spi_probe_function_init(void);

/* Following APIs are listed in mtdcore.h */
extern int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
extern int del_mtd_partitions(struct mtd_info *);
extern int parse_mtd_partitions(struct mtd_info *master, const char * const *types,struct mtd_partition **pparts,struct mtd_part_parser_data *data);
/* Above APIs are listed in mtdcore.h */

extern struct mtd_partition rtk_sdk_parts[RTK_MTD_PARTITION_NUM];

void cache_flush_spinor_area(unsigned int addr, unsigned int len)
{
    dma_cache_wback_inv(addr, len);
}

__init static int init_luna_nor_spi_map(void) {

    uint32 chip_id, chip_rev_id;
    int32  ret;

    /*RTL9300/9310 will use this flash driver*/
    if((ret = drv_swcore_cid_get((uint32)0, (uint32 *)&chip_id, (uint32 *)&chip_rev_id)) != 0)
    {
        printk("RTK_NORSFG3 driver cannot get chip ID\n");
        return 0;
    }else{
        if((FAMILY_ID(chip_id) == RTL9310_FAMILY_ID) || (FAMILY_ID(chip_id) == RTL9300_FAMILY_ID))
        {
            printk("RTK_NORSFG3 driver is used\n");
        }else{
            printk("RTK_NORSFG3 driver is bypassed\n");
            return 0;
        }
    }

    ret = spi_probe_function_init();

    printk(KERN_NOTICE DELIMITER_LINE);
    printk(KERN_NOTICE "%s: flash map at 0x%x\n", __FUNCTION__, (u32)luna_nor_spi_map.phys);

    luna_nor_spi_map.virt = (void *)SPI_NOR_FLASH_START_ADDR;
    if (!luna_nor_spi_map.virt) {
        printk(KERN_ERR "Failed to ioremap_nocache\n");
        return -EIO;
    }

    simple_map_init(&luna_nor_spi_map);

    /* We only support SPI NOR FLASH */
    if (!luna_nor_spi_mtd) {
        printk("SPI NOR driver probe...\n");
        luna_nor_spi_mtd = do_map_probe("spi_probe", &luna_nor_spi_map);
        if(rtk_flash_fill_mtd_table(luna_nor_spi_map.size) != 0)
        {
            printk(KERN_ERR "MTD Table FIll failed!\n");
            return 0;
        }
        if (luna_nor_spi_mtd) {
            struct  mtd_partition *parts;
            int nr_parts = 0;
            const char*part_probes[] = {"cmdlinepart", NULL,};

            nr_parts = parse_mtd_partitions(luna_nor_spi_mtd, part_probes, &parts, 0);

            printk("add SPI NOR partition\n");
            luna_nor_spi_mtd->owner = THIS_MODULE;
            if(nr_parts <= 0) {
                printk("MTD partitions obtained from built-in array\n");
                add_mtd_partitions(luna_nor_spi_mtd, rtk_sdk_parts, ARRAY_SIZE(rtk_sdk_parts));
            } else {
                printk("MTD partitions obtained from kernel command line\n");
                add_mtd_partitions(luna_nor_spi_mtd, parts, nr_parts);
            }

            printk(KERN_NOTICE DELIMITER_LINE);
            return 0;
        }
        printk("ERROR: SPI NOR partition invalid\n");
    } else {
        printk("%s: probe failed!\n", __func__);
    }

    iounmap((void *)luna_nor_spi_map.virt);
    printk(KERN_NOTICE DELIMITER_LINE);
    return -ENXIO;
}

__exit static void cleanup_luna_nor_spi_map(void) {
    if (luna_nor_spi_mtd) {
        del_mtd_partitions(luna_nor_spi_mtd);
        map_destroy(luna_nor_spi_mtd);
    }
    if (luna_nor_spi_map.virt) {
        iounmap((void *)luna_nor_spi_map.virt);
        luna_nor_spi_map.map_priv_1 = 0;
    }
}

MODULE_LICENSE("GPL");
module_init(init_luna_nor_spi_map);
module_exit(cleanup_luna_nor_spi_map);

