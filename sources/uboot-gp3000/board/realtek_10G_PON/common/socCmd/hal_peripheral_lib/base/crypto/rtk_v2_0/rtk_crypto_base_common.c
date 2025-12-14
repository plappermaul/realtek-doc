#include "rtk_crypto_base_common.h"

/**
 *  \fn          void *rtlc_memset(void *dst, int c, size_t n)
 *  \brief       Sets the first \em c bytes of the block of memory pointed by \em dst to the specified \em value .
 *  \param[in]   dst Pointer to destination address
 *  \param[in]   c Value to be set
 *  \param[in]   n Number of bytes to be set to the \em value
 *  \return      value  destination address
 */
void *rtlc_memset(void *dst, int c, size_t n)
{
    if (n != 0) {
        char *d = dst;

        do {
            *d++ = c;
        } while (--n > 0);
    }
    return dst;
}

/**
 *  \fn          void *rtlc_memcpy( void *s1, const void *s2, size_t n )
 *  \brief       Copies the values of \em n bytes from the location pointed to by \em s2 directly to the memory block pointed to by \em s1 .
 *  \param[in]   s1 Pointer to the destination array where the content is to be copied
 *  \param[in]   s2 Pointer to the source of data to be copied
 *  \param[in]   n Number of bytes to copy
 *  \return      value  destination address
 */
void *rtlc_memcpy(void *s1, const void *s2, size_t n)
{
    char *dst = (char *) s1;
    const char *src = (const char *) s2;

    while (n--) {
        *dst++ = *src++;
    }
    return s1;
}

int rtlc_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) {
            return (*p1 - *p2);
        } else {
            p1++;p2++;
        }
    }
    return 0;
}

void dump_for_one_bytes(u8 *pdata, u32 len)
{
    u8 *pbuf = pdata;  
    u32 length = len;
    u32 line_idx=0;
    u32 byte_idx;
    u32 offset;
    
    // __printk
    printf ("\r\n [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

    while(line_idx < length)
    {       
            printf("%lx: ", (size_t)(pbuf + line_idx));

            if ((line_idx + 16) < length) {
                offset = 16;
            } else {
                offset = length - line_idx;
            }
            
            for (byte_idx = 0; byte_idx<offset; byte_idx++) {
                printf("%02x ", pbuf[line_idx + byte_idx]);
            }

            for (byte_idx = 0; byte_idx < (16-offset); byte_idx++) { //a last line
                printf("   ");
            }

            printf("    ");        //between byte and char
            
            for (byte_idx = 0;  byte_idx < offset; byte_idx++) {                
                if( ' ' <= pbuf[line_idx + byte_idx]  && pbuf[line_idx + byte_idx] <= '~') {
                    printf("%c", pbuf[line_idx + byte_idx]);
                } else {
                    printf(".");
                }
            }
            
            printf("\n\r");
            line_idx += 16;
    }
    
}

void dump_for_one_words(u8 *src, u32 len)
{
    u32 i;
    
    src = (u8 *)(((size_t)src) & (~(0x03)));
    printf ("\r\n");
    for(i = 0; i < len; i+=16, src+=16) {   
        printf("%08x:    %08x", src, *(u32 *)(src));
        printf("    %08x", *(u32 *)(src+4));
        printf("    %08x", *(u32 *)(src+8));
        printf("    %08x\r\n", *(u32 *)(src+12));
    }
}

static uint32_t _rtl_seed;

static uint32_t rtlc_random1(uint32_t rtl_seed){
    
    uint32_t hi32, lo32;

    hi32 = (rtl_seed >> 16)*19;
    lo32 = (rtl_seed & 0xffff)*19 + 37;
    hi32 = hi32 ^ (hi32 << 16);
    return (hi32 ^ lo32);

}

void rtlc_srandom(uint32_t seed){
    
    _rtl_seed = seed;
}

uint32_t rtlc_random(void){
    
    _rtl_seed = rtlc_random1(_rtl_seed);
    return (_rtl_seed);
}