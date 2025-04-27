/**
 *  SPI Flash probe code.
 *  (C) 2006 Atmark Techno, Inc.
 */

#include <common/rt_type.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <linux/mtd/map.h>

#include <linux/mtd/mtd.h>

#include <nor_spi/nor_spif_core.h>

/* Included for LOADER_START, etc. */
#include <linux/mtd/rtk_flash_common.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif

extern struct semaphore     spi_bus_semaphore;

extern int mtd_spi_erase(struct mtd_info *mtd, struct erase_info *instr);
extern int mtd_spi_read(struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf);
extern int mtd_spi_write(struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf);
extern void mtd_spi_sync(struct mtd_info *mtd);
extern int mtd_spi_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
extern int mtd_spi_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
extern int mtd_spi_suspend(struct mtd_info *mtd);
extern void mtd_spi_resume(struct mtd_info *mtd);

extern norsf_g2_info_t norsf_info;

static struct mtd_info *spi_probe(struct map_info *map);
struct mtd_info *spi_probe(struct map_info *map);
static void spi_destroy(struct mtd_info *mtd);

static struct mtd_chip_driver spi_chipdrv = {
    probe: spi_probe,
    destroy: spi_destroy,
    name: "spi_probe",
    module:THIS_MODULE,
};

struct mtd_info *spi_probe(struct map_info *map) {
    struct mtd_info *mtd = NULL;

    norsf_detect();
    mtd = kmalloc(sizeof(*mtd), GFP_KERNEL);
    if (!mtd) {
        printk(KERN_WARNING "Failed to allocate memory for MTD device\n");
        return NULL;
    }

    sema_init(&spi_bus_semaphore, 1);

    map->size = norsf_info.size_per_chip_b * norsf_info.num_chips;
    map->fldrv = &spi_chipdrv;

    memset(mtd, 0, sizeof(struct mtd_info));
    mtd->type  = MTD_NORFLASH;
    mtd->flags = MTD_CAP_NORFLASH;
    mtd->name  = map->name;
    mtd->size  = map->size;

    /* 150529,to be revised. */
    mtd->erasesize = 4096;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    mtd->erase   = mtd_spi_erase;
    mtd->read    = mtd_spi_read;
    mtd->write   = mtd_spi_write;
    mtd->sync    = mtd_spi_sync;
    mtd->suspend = mtd_spi_suspend;
    mtd->resume  = mtd_spi_resume;
#else
    mtd->_erase   = mtd_spi_erase;
    mtd->_read    = mtd_spi_read;
    mtd->_write   = mtd_spi_write;
    mtd->_sync    = mtd_spi_sync;
    mtd->_lock    = mtd_spi_lock;
    mtd->_unlock  = mtd_spi_unlock;
    mtd->_suspend = mtd_spi_suspend;
    mtd->_resume  = mtd_spi_resume;
#endif
    mtd->priv    = (void *)map;
    mtd->writesize = 1;

    return mtd;
}

int __init spi_probe_function_init(void) {
    register_mtd_chip_driver(&spi_chipdrv);
    return 0;
}

static void spi_destroy(struct mtd_info *mtd) {
    printk("%s\n", __FUNCTION__);
}

int __init spi_probe_init(void) {
//    register_mtd_chip_driver(&spi_chipdrv);
    return 0;
}

void __exit spi_probe_exit(void) {
    unregister_mtd_chip_driver(&spi_chipdrv);
}

module_init(spi_probe_init);
module_exit(spi_probe_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Masahiro Nakai <nakai@atmark-techno.com> et al.");
MODULE_DESCRIPTION("Probe code for NOR SPI-F G2 controller.");

