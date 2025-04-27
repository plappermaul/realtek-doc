#ifndef PRELOADER_H
#define PRELOADER_H

#include <soc.h>

/***********************************************************
  Changing following addresses may screw up memory layout!
 ***********************************************************/
#define UCSRAM_BASE  (SRAM_BASE  | 0x20000000)
#define UCFLASH_BASE (FLASH_BASE | 0x20000000)
#define STACK0_BASE  (SRAM_BASE + SRAM_SIZE - 8)
#define STACK1_BASE  (SRAM_BASE + SRAM_SIZE - CACHE_ALIGN(sizeof(parameter_to_bootloader_t)) - 8)
#define STACK2_BASE  (0x80200000 - 8)     /* Fixed the stack area to the first 2MB. */
#define HEAP_SIZE    (1024*1024)          /* Size in bytes allowing dynamic
                                             memory allocation. Majorly used
                                             when decompression(). */
//#define SRAM_SZ_MASK (SRAM_SIZE - 1)                                             
#if (SRAM_SIZE <= 0x8000)
    #define SRAM_SZ_MASK 0x7fff
#elif (SRAM_SIZE <= 0x10000)
    #define SRAM_SZ_MASK 0xffff
#elif (SRAM_SIZE <= 0x20000)
    #define SRAM_SZ_MASK 0x1ffff
#elif (SRAM_SIZE <= 0x40000)
    #define SRAM_SZ_MASK 0x3ffff
#elif (SRAM_SIZE <= 0x80000)
    #define SRAM_SZ_MASK 0x7ffff
#elif (SRAM_SIZE <= 0x100000)
    #define SRAM_SZ_MASK 0xfffff
#else
    #error SRAM_SZ_MASK was not declared completely
#endif

#ifdef __ASSEMBLER__

#if (OTTO_NOR_SPI_FLASH == 1)
/* Since LPLR in NOR environment is built at SRAM but
   started from UCFLASH, every jump before `ld_to_sram'
   should be rebased.	*/
#define REBASED_CALL(addr, base) \
	la   s0, addr;         \
	li   s1, SRAM_SZ_MASK; \
	and  s0, s0, s1;       \
	li   s1, base;         \
	or   s0, s0, s1;       \
	jalr s0;               \
	nop

#define UCFLASH_CALL(addr)  \
    la s0, addr;            \
    li s1, 0xAFC00000;      \
    or s0, s1;              \
    jalr s0;                \
    nop

#elif (OTTO_NAND_FLASH == 1)
/* on the other hand, LPLR in nand environment is built and
   started at SRAM, so needs NOT rebasing. */
#define REBASED_CALL(addr, base) \
	la   s0, addr;         \
	jalr s0;               \
	nop

#define UCFLASH_CALL(addr) REBASED_CALL(addr, 0xAFC00000)
	
#define UNCACHE_CALL(addr)  \
    la s0, addr;            \
    li s1, 0xA0000000;      \
    or s0, s1;              \
    jalr s0;                \
    nop
#endif //(OTTO_NOR_SPI_FLASH == 1)

#else // __ASSEMBLER__
#include <stddef.h>
#include <pblr.h>

// PATCH level defines the order of execution, the level 0~2 runs SRAM stack,
//  and 4~6 MAY run on DRAM stack if DRAM initial successfully
#define PATCH_REG(x, lvl) \
	void_func * __swp_##x __attribute__ ((section (".soft_patch." #lvl))) = x
typedef void (sw_patch_t) (void);


#define alloca(sz) __builtin_alloca(sz)

#define ACCESS_REG(addr) (*((volatile u32_t *)(addr)))

/* functions in lplr. */
void c_start_prologue(void);
void parameters_init(void);
void cpu_init(void);

void *pblr_memcpy(u8_t *, const u8_t *, u32_t);
#define memcpy pblr_memcpy

#if (OTTO_NAND_FLASH == 1)
void nand_loader_init(void);
#define NAND_SPARE_AREA_SIZE (64)
#endif

/* functions in plr. */
void bzero(void *buf, u32_t nbyte);
void c_start_epilogue(void);
void platform_init_phase_2(void);
void pre_flash_init(void);
void flash_init(void);
void post_flash_init(void);
void uboot_setup(void);
void nursing_uboot(void);
s32_t decompress(u8_t *, u8_t *, void *);

void pblr_udelay(u32_t usecs);
#define udelay pblr_udelay

#define plr_getc        (parameters._uart_getc)
#define plr_tstc        (parameters._uart_tstc)
#define plr_putc        (parameters._uart_putc)
#define plr_query_freq  (parameters._pll_query_freq)


/* support functions for plr. */
unsigned int pblr_soc_printf(const char *fmt, ...);
#define printf pblr_soc_printf

u32_t endian_swap(const u8_t *);
void NS16550_putc(u8_t c);
void pblr_puts(const char *s);
void pblr_putc(const char c);
int pblr_strnlen(const char * s, int count);
#endif // __ASSEMBLER__

/* Enable self test to verify DRAM setup and decompression. */
#ifndef SELFTEST
#define SELFTEST 0
#endif

#if ((DECOMP_ALGO_LZMA + DECOMP_ALGO_GZIP) != 1)
#error EE: Choose exact ONE decomplression algorithm.
#endif

#if ((OTTO_NAND_FLASH + OTTO_NOR_SPI_FLASH) != 1)
#error EE: Choose exact ONE flash type.
#endif

#define PRINT_PLR_INFO(chip_ver)                           \
        printf("SoC Preloader %x.%x.%d.%d.%d.%s.%s.%s (%s)\n" \
               "II: Stack @ %p (parameter %dB)\n" \
               "II: Console... OK\n",             \
               (PLR_VERSION >> 16) & 0xFF, \
               (PLR_VERSION >> 8)  & 0xFF,  \
               (PLR_VERSION >> 0)  & 0xFF, \
               chip_ver, parameters.soc_id,       \
               REL_VER, REVISION, DECOMP_ALGO, DATE_TAG,   \
               STACK1_BASE, CACHE_ALIGN(sizeof(parameter_to_bootloader_t)))


#endif /* #ifndef PRELOADER_H */
