/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "rtl8195a_crypto_util_rom.h"


#if 0

/* MACROS */

/* Nonzero if either X or Y is not aligned on a word boundary. */
#define CHECK_STR_UNALIGNED(X, Y) \
     (((u32)(X) & (sizeof (u32) - 1)) | \
      ((u32)(Y) & (sizeof (u32) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop in the
 * optimised string implementation
 */
#define STR_OPT_BIGBLOCKSIZE     (sizeof(u32) << 2)


/* How many bytes are copied each iteration of the word copy loop in the
 * optimised string implementation
 */
#define STR_OPT_LITTLEBLOCKSIZE (sizeof (u32))

/* EXPORTED SYMBOLS */


/* FUNCTIONS */
HAL_ROM_TEXT_SECTION
_LONG_CALL_ void *
_memcpy( void *s1, const void *s2, unsigned int n )
{
    char *dst = (char *) s1;
    const char *src = (const char *) s2;

    u32 *aligned_dst;
    const u32 *aligned_src;


    /* If the size is small, or either SRC or DST is unaligned,
     * then punt into the byte copy loop.  This should be rare.
     */
    if (n < sizeof(u32) || CHECK_STR_UNALIGNED (src, dst)) {
        while (n--)
            *dst++ = *src++;

        return s1;
    } /* if */

    aligned_dst = (u32 *)dst;
    aligned_src = (const u32 *)src;

    /* Copy 4X long words at a time if possible.  */
    while (n >= STR_OPT_BIGBLOCKSIZE) {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        n -= STR_OPT_BIGBLOCKSIZE;
    } /* while */

    /* Copy one long word at a time if possible.  */
    while (n >= STR_OPT_LITTLEBLOCKSIZE) {
        *aligned_dst++ = *aligned_src++;
        n -= STR_OPT_LITTLEBLOCKSIZE;
    } /* while */

    /* Pick up any residual with a byte copier.  */
    dst = (char*)aligned_dst;
    src = (const char*)aligned_src;
    while (n--)
        *dst++ = *src++;

    return s1;
} /* _memcpy() */


HAL_ROM_TEXT_SECTION
_LONG_CALL_
void __rtl_memsetb(u8 *pData, u8 data, int bytes)
{
    int i;
    u8 *ptr;

    ptr = pData;

    for (i=0; i<bytes; i++) {
        *ptr = data;
        ptr++;
    }

}

#endif

