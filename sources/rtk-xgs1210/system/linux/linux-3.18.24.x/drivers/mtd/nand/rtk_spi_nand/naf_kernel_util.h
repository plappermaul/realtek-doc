

#ifndef NAF_KERNEL_UTIL_H__
#define NAF_KERNEL_UTIL_H__

#ifndef inline_memcpy
#define inline_memcpy memcpy
#endif

#ifndef inline_memset
#define inline_memset(d, v, l) ({char *__d=(char*)d; char __v=(char)v; unsigned __l=l; while (__l-- >0) *(__d++)=__v;})
#endif




//#define DCACHE_AAA
//#ifdef DCACHE_AAA
#include <asm/io.h>
#define DMA_CACHE_WBACK_INV(start_addr, end_addr) dma_cache_wback_inv((start_addr), ((end_addr)-(start_addr)+1))
//#endif

#ifndef  dcache_wr_inv
#define dcache_wr_inv(start_addr, end_addr) dma_cache_wback_inv((start_addr), (end_addr)-(start_addr))
#endif



#define symb_pdefine(...)
#define symb_fdefine(...)

#ifndef SECTION_NAND_SPIF_PROBE_FUNC
#define SECTION_NAND_SPIF_PROBE_FUNC __attribute__ ((section (".nand_spif_probe_func")))
#endif
#define SECTION_SNAF_INIT_REST_FUNC     __attribute__ ((section (".snaf_init_rest_func")))
#define REG_SPI_NAND_INIT_REST_FUNC(fn) int* __snaf_ ## ## fn ## _ \
        SECTION_SNAF_INIT_REST_FUNC = (int*) fn


#define REG_SPI_NAND_PROBE_FUNC(fn) spi_nand_probe_t* __nspf_ ## ## fn ## _ \
        SECTION_NAND_SPIF_PROBE_FUNC = (spi_nand_probe_t*) fn

#define SECTION_ONFI_PROBE_FUNC __attribute__ ((section (".onfi_probe_func")))
#define REG_ONFI_PROBE_FUNC(fn) onfi_probe_t* __nspf_ ## ## fn ## _ \
        SECTION_ONFI_PROBE_FUNC = (onfi_probe_t*) fn


#define NOTALIGNED(mtd, x) ((x & (mtd->writesize-1)) != 0)   //czyao, 2010/0917


#define BLOCK_ADDR(block_0_1023) (block_0_1023<<6)
#define BLOCK_PAGE_ADDR(block_0_1023, page_0_63) ((0x00<<16)|(block_0_1023<<6)|(page_0_63))

#ifndef bzero
#define bzero(p,n) memset(p, '\0', n)
#endif

typedef int32_t (fps32_t)(void);
typedef uint32_t (fpu32_t)(void);



#define SECTION_SDATA
#define SECTION_UNS_TEXT
#define SECTION_RECYCLE
#define SECTION_ONFI_DATA

#define REG_INIT_FUNC(a, b)

#define NSU_DRIVER_IN_ROM 1
#define ONFI_DRIVER_IN_ROM 1
#define __DEVICE_REASSIGN 0

#define CONFIG_SPI_NAND_FLASH_INIT_FIRST 1
#define CONFIG_SPI_NAND_FLASH_INIT_REST 1

#endif // end NAF_KERNEL_UTIL_H__

