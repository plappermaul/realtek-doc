
#include <lib/lzma/tlzma.h>

#define HEAP_SIZE    (1024*1024)          /* Size in bytes allowing dynamic */
#define alloca(sz) __builtin_alloca(sz)
void *heap_base = 0;

#ifdef HAS_LIB_LZMA
lzma_decode_t *_lzma_decode SECTION_SDATA = VZERO;
#endif

SECTION_LZMA void 
*SzAlloc(void *p __attribute__((__unused__)),SizeT size)
{
	void *cur = heap_base;
	heap_base += size;
	return cur;
}

SECTION_LZMA void 
SzFree(void *p __attribute__((__unused__)), void *address __attribute__((__unused__)))
{
	return;
}

/************************************************
  LZMA compressed file format (lzma.txt)
  ---------------------------
  Offset Size Description
  0      1   Special LZMA properties (lc,lp, pb in encoded form)
  1      4   Dictionary size (little endian)
  5      8   Uncompressed size (little endian). -1 means unknown size
  13     -   Compressed data
************************************************/
//#define LZ_PROP    0
//#define LZ_DICT_SZ 1
#define LZ_UCOMP_SZ 5
#define LZ_COMP_DAT 13

SECTION_LZMA u32_t 
endian_swap(const u8_t *addr)
{
    return ((*(addr+3) << 24) + (*(addr+2) << 16) + (*(addr+1) <<  8) + (*(addr+0) <<  0));
}


SECTION_LZMA s32_t 
lzma_decompress(u8_t *src, u8_t *dest, u32_t *p_srclen_dstlen)
{
    /* "Uncompressed data size" is little endian in lzma header. */
    u32_t desLen = endian_swap(src+LZ_UCOMP_SZ);
    ELzmaStatus status;
    ISzAlloc myalloc = {SzAlloc, SzFree};
    heap_base = alloca(HEAP_SIZE);
    s32_t ret = (*_lzma_decode)(dest, &desLen,
                                src+LZ_COMP_DAT, p_srclen_dstlen,
                                src, 5,
                                LZMA_FINISH_ANY, &status, &myalloc);
    *p_srclen_dstlen = desLen;
    return ret;
}

SECTION_LZMA __attribute__ ((noinline)) s32_t 
lzma_chsp_jump(u8_t * const src, u8_t * const dest, u32_t *p_srclen_dstlen, u32_t new_sp)
{
    u32_t old_sp;
    asm volatile ("move %0, $29": "=r"(old_sp));
    asm volatile ("move $29, %0": : "r"(new_sp));
    s32_t ret = lzma_decompress(src, dest, p_srclen_dstlen);
    asm volatile ("move $29, %0": : "r"(old_sp));
    return ret;
}

