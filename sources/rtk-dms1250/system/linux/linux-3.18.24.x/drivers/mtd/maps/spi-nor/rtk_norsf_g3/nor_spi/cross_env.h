#ifndef _CROSS_ENV_H_
#define _CROSS_ENV_H_

#if defined(__LUNA_KERNEL__)
#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <luna_cfg.h>
#include <asm/regdef.h>
#include <asm/cacheflush.h>
#define puts(...)          printk(__VA_ARGS__)
#define printf(...)        printk(__VA_ARGS__)
#define inline_memcpy(...) memcpy(__VA_ARGS__)
#ifndef GET_CPU_MHZ
    #define GET_CPU_MHZ()    (1000)
#endif

#ifdef noinline
#undef noinline
#endif

#define SFNOR_BASE_ADDR		0x94000000


extern void cache_flush_spinor_area(unsigned int addr, unsigned int len);


#define IDCACHE_FLUSH_ADDR(addr, len) do { \
        cache_flush_spinor_area(addr, len); \
    } while (0)


#define IDCACHE_FLUSH() do { \
        cache_flush_spinor_area(SFNOR_BASE_ADDR, 64*1024*1024); \
    } while (0)
// #define DCACHE_LINE_SZ_B CACHELINE_SIZE /*Mask by SDK*/

#define DCACHE_LINE_SZ_B    32

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
#define CONFIG_DEFAULTS_KERNEL_3_18     1
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
#define CONFIG_KERNEL_2_6_30            1
#endif

#elif defined(CONFIG_STANDALONE_UBOOT)
#include <common.h>
#include <asm/mipsregs.h>

extern u32_t otto_cg_query_freq(u32_t dev_type);

#define inline_memcpy(...) memcpy(__VA_ARGS__)
#define IDCACHE_FLUSH() do {	  \
        flush_cache(0, 0); \
    } while (0)
#ifndef GET_CPU_MHZ
    #define GET_CPU_MHZ()    otto_cg_query_freq(CG_DEV_OCP)
#endif
#define DCACHE_LINE_SZ_B CONFIG_SYS_CACHELINE_SIZE

/* So far RS is safe on MIPS so no actual disable code */
#define RETURN_STACK_SAVE(_bpctl_backup)
#define RETURN_STACK_RESTORE(_bpctl_backup)

#else
#include <cpu/cpu.h>
#define IDCACHE_FLUSH() do {      \
        _bios.dcache_writeback_invalidate_all(); \
        _bios.icache_invalidate_all(); \
    } while (0)
#ifndef GET_CPU_MHZ
    #define GET_CPU_MHZ()    cg_query_freq(CG_DEV_OCP)
#endif
#define DCACHE_LINE_SZ_B DCACHE_LINE_SIZE

#endif

#endif
