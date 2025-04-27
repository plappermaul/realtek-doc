#ifndef TLZMA_H
#define TLZMA_H

#include <lib/lzma/lzma_interface.h>
#include <soc.h>

#define HAS_LIB_LZMA


extern lzma_decode_t *_lzma_decode;
extern s32_t lzma_decompress(u8_t *src, u8_t *dest, u32_t *p_srclen_dstlen);
extern s32_t lzma_chsp_jump(u8_t * const src, u8_t * const dest, u32_t *p_srclen_dstlen, u32_t new_sp);
#define DECOMPRESS_OK SZ_OK

#endif //TLZMA_H

