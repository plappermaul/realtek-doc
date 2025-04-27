#include "addrspace.h"
#include "crypto.h"
#include <exports.h>

#define CONFIG_RTL865XC

#define __read_32bit_c0_register(source, sel)                           \
({ int __res;                                                           \
        if (sel == 0)                                                   \
                __asm__ __volatile__(                                   \
                        "mfc0\t%0, " #source "\n\t"                     \
                        : "=r" (__res));                                \
        else                                                            \
                __asm__ __volatile__(                                   \
                        ".set\tmips32\n\t"                              \
                        "mfc0\t%0, " #source ", " #sel "\n\t"           \
                        ".set\tmips0\n\t"                               \
                        : "=r" (__res));                                \
        __res;                                                          \
})

#define __write_32bit_c0_register(register, sel, value)                 \
do {                                                                    \
        if (sel == 0)                                                   \
                __asm__ __volatile__(                                   \
                        "mtc0\t%z0, " #register "\n\t"                  \
                        : : "Jr" (value));                              \
        else                                                            \
                __asm__ __volatile__(                                   \
                        ".set\tmips32\n\t"                              \
                        "mtc0\t%z0, " #register ", " #sel "\n\t"        \
                        ".set\tmips0"                                   \
                        : : "Jr" (value));                              \
} while (0)


#define read_c0_status()        __read_32bit_c0_register($12, 0)
#define write_c0_status(val)    __write_32bit_c0_register($12, 0, val)
#define UNCACHE_MASK            0x20000000
#define CACHED(addr)                    ((uint32)(addr) & ~(UNCACHE_MASK))
#define ST0_ISC                   0x00010000
#define ST0_IEC                 0x00000001
#define DCACHE_SIZE	8192
//static unsigned long dcache_size;

static void r3k_flush_dcache_range(unsigned long start, unsigned long end)
{
        /*Invalidate D-Cache*/
#if defined(CONFIG_RTL865XC) || defined(CONFIG_RTL8672)

#if 1
        /* Flush D-Cache using its range */
        unsigned char *p;
        unsigned int size;
        unsigned int flags;
        unsigned int i;

        size = end - start;

        /* correctness check : flush all if any parameter is illegal */
        if (    ( KSEGX(start) == KUSEG ) ||
                ( size >= DCACHE_SIZE )         )
        {
                __asm__ volatile(
                        "mtc0 $0,$20\n\t"
                        "nop\n\t"
                        "li $8,512\n\t"
                        "mtc0 $8,$20\n\t"
                        "nop\n\t"
                        "nop\n\t"
                        "mtc0 $0,$20\n\t"
                        "nop"
                        : /* no output */
                        : /* no input */
                                );
        } else
        {
                /* Start to isolate cache space */
                p = (char *)(PHYSADDR(start) | K0BASE);

                flags = read_c0_status();

                /* isolate cache space */
                write_c0_status( (ST0_ISC | flags) &~ ST0_IEC );

                for (i = 0; i < size; i += 0x040)
                {
                        asm (
                                "cache 0x15, 0x000(%0)\n\t"
                                "cache 0x15, 0x010(%0)\n\t"
                                "cache 0x15, 0x020(%0)\n\t"
                                "cache 0x15, 0x030(%0)\n\t"
                                :               /* No output registers */
                                :"r"(p)         /* input : 'p' as %0 */
                                );
                        p += 0x040;
                }

                write_c0_status(flags);
        }
#else
        __asm__ volatile(
                "mtc0 $0,$20\n\t"
                "nop\n\t"
                "li $8,512\n\t"
                "mtc0 $8,$20\n\t"
                "nop\n\t"
                "nop\n\t"
                "mtc0 $0,$20\n\t"
                "nop"
                : /* no output */
                : /* no input */
                        );
#endif

#if 0 /* Reset Prefetch Buffer If Necessary */
   *(volatile unsigned int *) 0xB8001010 = (*(volatile unsigned int *) 0xB8001010) | 0x10000;
#endif

#else
        __asm__ volatile(
                "mtc0 $0,$20\n\t"
                "nop\n\t"
                "li $8,1\n\t"
                "mtc0 $8,$20\n\t"
                "nop\n\t"
                "nop\n\t"
                "mtc0 $0,$20\n\t"
                "nop"
                : /* no output */
                : /* no input */
                        );
#endif

#if 0
        /*Invalidate D-Cache*/
        lx4180_writeCacheCtrl(0);
        lx4180_writeCacheCtrl(1);
        lx4180_writeCacheCtrl(0);
        return;

        unsigned long size, i, flags;
        volatile unsigned char *p;
    volatile unsigned int reg;
    save_flags(flags);cli();
    reg=read_c0_xcontext();
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    write_c0_xcontext((reg & (~0x1))); //write '0' to bit 0,1
    __asm__ volatile("nop");
    write_c0_xcontext((reg | (0x1))); //write '0' to bit 0,1
    __asm__ volatile("nop");
    write_c0_xcontext((reg & (~0x1))); //write '0' to bit 0,1
    __asm__ volatile("nop");
    restore_flags(flags);
        return;




        size = end - start;
        if (size > dcache_size || KSEGX(start) != KSEG0) {
                start = KSEG0;
                size = dcache_size;
        }
        p = (char *)start;

        flags = read_c0_status();

        /* isolate cache space */
        write_c0_status((ST0_ISC|flags)&~ST0_IEC);

        for (i = 0; i < size; i += 0x080) {
                asm (   "sb\t$0, 0x000(%0)\n\t"
                        "sb\t$0, 0x004(%0)\n\t"
                        "sb\t$0, 0x008(%0)\n\t"
                        "sb\t$0, 0x00c(%0)\n\t"
                        "sb\t$0, 0x010(%0)\n\t"
                        "sb\t$0, 0x014(%0)\n\t"
                        "sb\t$0, 0x018(%0)\n\t"
                        "sb\t$0, 0x01c(%0)\n\t"
                        "sb\t$0, 0x020(%0)\n\t"
                        "sb\t$0, 0x024(%0)\n\t"
                        "sb\t$0, 0x028(%0)\n\t"
                        "sb\t$0, 0x02c(%0)\n\t"
                        "sb\t$0, 0x030(%0)\n\t"
                        "sb\t$0, 0x034(%0)\n\t"
                        "sb\t$0, 0x038(%0)\n\t"
                        "sb\t$0, 0x03c(%0)\n\t"
                        "sb\t$0, 0x040(%0)\n\t"
                        "sb\t$0, 0x044(%0)\n\t"
                        "sb\t$0, 0x048(%0)\n\t"
                        "sb\t$0, 0x04c(%0)\n\t"
                        "sb\t$0, 0x050(%0)\n\t"
                        "sb\t$0, 0x054(%0)\n\t"
                        "sb\t$0, 0x058(%0)\n\t"
                        "sb\t$0, 0x05c(%0)\n\t"
                        "sb\t$0, 0x060(%0)\n\t"
                        "sb\t$0, 0x064(%0)\n\t"
                        "sb\t$0, 0x068(%0)\n\t"
                        "sb\t$0, 0x06c(%0)\n\t"
                        "sb\t$0, 0x070(%0)\n\t"
                        "sb\t$0, 0x074(%0)\n\t"
                        "sb\t$0, 0x078(%0)\n\t"
                        "sb\t$0, 0x07c(%0)\n\t"
                        : : "r" (p) );
                p += 0x080;
        }

        write_c0_status(flags);
#endif
}
unsigned long r3k_cache_size(unsigned long ca_flags)
{
	return DCACHE_SIZE;
}
#define dma_cache_wback_inv(start,size)    r3k_flush_dcache_range(start,size)
inline int32 rtlglue_flushDCache(uint32 start, uint32 size)
{
#ifdef CONFIG_RTL865XC
/*
 *Processor : RLX5181
 **/
//        rtlglue_drvMutexLock();
        dma_cache_wback_inv(    start?CACHED(start):0,
                                                (size == 0)?(r3k_cache_size(ST0_ISC)):size);
//       rtlglue_drvMutexUnlock();

#elif defined (CONFIG_RTL865XB)
/*
 *Processor : RLX5280
 **/
        lx4180_writeCacheCtrl(0);
        lx4180_writeCacheCtrl(1);
        lx4180_writeCacheCtrl(0);
#elif defined (CONFIG_RTL8316S)
	mips_cache_flush_all();
#endif
        return SUCCESS;
}
