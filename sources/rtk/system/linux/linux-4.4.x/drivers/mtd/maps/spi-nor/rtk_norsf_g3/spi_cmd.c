/**
 *  SPI Flash common control code.
 *  (C) 2006 Atmark Techno, Inc.
 */
#include <common/rt_type.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/mutex.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/gen_probe.h>

// #include <bspchip.h>  //By SDK
// #include <kernel_soc.h> //By SDK

#include <nor_spi/nor_spif_core.h>

#if defined(MTD_SPI_DEBUG)
#define KDEBUG(args...) printk(args)
#else
#define KDEBUG(args...)
#endif

extern struct semaphore     spi_bus_semaphore;
#define SPI_SEM_LOCK()      down(&spi_bus_semaphore)
#define SPI_SEM_UNLOCK()    up(&spi_bus_semaphore)


#define USE_MUTEX_FOR_CTRITICAL_SECTIONS
#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    DECLARE_MUTEX(snf_mutex);/* spi nor flash mutex */
#else
/* After 2.6.36, DEFINE_MUTEX is replaced by DEFINE_SEMAPHORE.
 * And DEFINE_MUTEX is "use semaphore to emulate mutex" at 2.6.30.
 * So snf_mutex is type semaphore, not mutex. Don't confuse.
 */
    DEFINE_SEMAPHORE(snf_mutex);
#endif

#else
    spinlock_t spi_lock = SPIN_LOCK_UNLOCKED;
#endif /* ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS */

int mtd_spi_erase(struct mtd_info *mtd, struct erase_info *instr) {
    unsigned long adr, len;
    int ret = 0;
#ifndef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    unsigned long  flags;
#endif

    if (instr->addr & (mtd->erasesize - 1))
        return -EINVAL;

    if ((instr->len + instr->addr) > mtd->size)
        return -EINVAL;

    adr = instr->addr;
    len = instr->len;

    KDEBUG("mtd_spi_erase():: adr: 0x%08lx, len: 0x%08lx\n", adr, len);

#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    down_interruptible(&snf_mutex);
#else
    down(&snf_mutex);
#endif
#else
    spin_lock_irqsave(&spi_lock, flags);
#endif

    SPI_SEM_LOCK();

    ret = NORSF_ERASE(instr->addr, instr->len, 0, 0);

    SPI_SEM_UNLOCK();

#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    up(&snf_mutex);
#else
    spin_unlock_irqrestore(&spi_lock, flags);
#endif

    if (ret) {
        return -EINVAL;
    } else {
        instr->state = MTD_ERASE_DONE;
        if (instr->callback)
            instr->callback(instr);
        return 0;
    }
}

int mtd_spi_read(struct mtd_info *mtd, loff_t from,
                 size_t len, size_t * retlen, u_char * buf) {
    int ret = 0;
#ifndef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    unsigned long flags;
#endif

    KDEBUG("mtd_spi_read():: adr: 0x%08x, len: %08x\n", (u32) from, len);
#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    down_interruptible(&snf_mutex);
#else
    down(&snf_mutex);
#endif
#else
    spin_lock_irqsave(&spi_lock, flags);
#endif

    SPI_SEM_LOCK();

    ret = NORSF_READ(from, len, buf, 0);

    SPI_SEM_UNLOCK();

#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    up(&snf_mutex);
#else
    spin_unlock_irqrestore(&spi_lock, flags);
#endif

    if (ret) {
        return -EINVAL;
    } else {
        if (retlen) (*retlen) = len;
        return 0;
    }
}

extern u32_t wr_boundary;
int mtd_spi_write(struct mtd_info *mtd, loff_t to, size_t len,
                  size_t * retlen, const u_char * buf) {
    int ret = 0;
#ifndef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    unsigned long flags;
#endif

    KDEBUG("mtd_spi_write():: adr: 0x%08x, len: 0x%08x, buf: %p\n", (u32) to, len, buf);

#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    down_interruptible(&snf_mutex);
#else
    down(&snf_mutex);
#endif
#else
    spin_lock_irqsave(&spi_lock, flags);
#endif

    SPI_SEM_LOCK();

    ret = NORSF_PROG(to, len, buf, 0);

    SPI_SEM_UNLOCK();

#ifdef USE_MUTEX_FOR_CTRITICAL_SECTIONS
    up(&snf_mutex);
#else
    spin_unlock_irqrestore(&spi_lock, flags);
#endif

    if (ret) {
        return -EINVAL;
    } else {
        if (retlen) (*retlen) = len;
        return 0;
    }
}

void mtd_spi_sync(struct mtd_info *mtd) {
    /* Operation not supported on transport endpoint */
}

int mtd_spi_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len) {
    /* Operation not supported on transport endpoint */
    return -EOPNOTSUPP;
}

int mtd_spi_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len) {
    /* Operation not supported on transport endpoint */
    return -EOPNOTSUPP;
}

int mtd_spi_suspend(struct mtd_info *mtd) {
    /* Operation not supported on transport endpoint */
    return -EOPNOTSUPP;
}

void mtd_spi_resume(struct mtd_info *mtd) {
    /* Operation not supported on transport endpoint */
}

EXPORT_SYMBOL(mtd_spi_erase);
EXPORT_SYMBOL(mtd_spi_read);
EXPORT_SYMBOL(mtd_spi_write);
EXPORT_SYMBOL(mtd_spi_sync);
EXPORT_SYMBOL(mtd_spi_lock);
EXPORT_SYMBOL(mtd_spi_unlock);
EXPORT_SYMBOL(mtd_spi_suspend);
EXPORT_SYMBOL(mtd_spi_resume);
