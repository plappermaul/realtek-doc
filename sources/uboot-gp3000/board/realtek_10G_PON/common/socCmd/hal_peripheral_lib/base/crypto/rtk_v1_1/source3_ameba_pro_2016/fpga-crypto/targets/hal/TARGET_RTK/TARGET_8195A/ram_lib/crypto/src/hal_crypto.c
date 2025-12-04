/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

#include "rtl_stdlib.h"

#include "hal_crypto.h"
#include "hal_crypto_ram.h"
#include "rtl8195a_crypto_rom.h"

#undef printk
#define printk DiagPrintf

#define rtl_assert(x)

#define CRYPTO_USE_SW_ALSO

//
// stdlib for crypto
//


int rtl_memcmpb(const u8 *dst, const u8 *src, int bytes)
{
    return __rtl_memcmpb(dst, src, bytes);
}

#ifdef __ICCARM__
#pragma data_alignment=4
#endif
static u32 _rtl_seed
#ifdef __GNUC__
__attribute__ ((aligned (4)))
#endif
;
#ifdef __ICCARM__
#pragma data_alignment=1
#endif


void rtl_srandom( u32 seed )
{
    _rtl_seed = seed;
}

u32 rtl_grandom( void )
{
    return(_rtl_seed);
}

u32 rtl_random( void )
{
    _rtl_seed = __rtl_random(_rtl_seed);
    return ( _rtl_seed );
}


int rtl_align_to_be32(u32 *pData, int bytes4_num )
{
    if ( pData == NULL ) return FAIL;
    if ( ((u32)(pData) & 0x3) != 0 ) return FAIL ;
    __rtl_align_to_be32(pData, bytes4_num);
    return SUCCESS;
}




void rtl_memDump(const u8 *start, u32 size, char * strHeader)
{
    __rtl_memDump(start, size, strHeader);
}


//
// HAL crypto functions
//

static hal_crypto_adapter_t g_rtl_cipherEngine;




void rtl_cryptoEngine_info(void)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    int i;
    u32 desc_num;

    rtl_assert(pIE != NULL);

    desc_num = pIE->desc_num;

    if ( pIE->isInit == _TRUE ) {
        printk("Crypto Engine has been initialized\n");
    } else {
        printk("Crypto Engine is NOT initialized\n");
    }

    printk("=========================================\n");
    printk("Crypto Engine \n");
    printk("=========================================\n");

    printk(" desc_num : %d\n", desc_num);

    __rtl_cryptoEngine_info();
}



int rtl_cryptoEngine_init(void)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    pIE->desc_num = CRYPTO_MAX_DESP;
    pIE->dma_burst_num= 8; // 8x4 = 32 byte

    return __rtl_cryptoEngine_init(pIE);

}



#define MBEDTLS_MD_MAX_SIZE 32

#define MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE                -0x5080  /**< The selected feature is not available. */
#define MBEDTLS_ERR_MD_BAD_INPUT_DATA                     -0x5100  /**< Bad input parameters to function. */
#define MBEDTLS_ERR_MD_ALLOC_FAILED                       -0x5180  /**< Failed to allocate memory. */
#define MBEDTLS_ERR_MD_FILE_IO_ERROR                      -0x5200  /**< Opening or reading of file failed. */



// MD5

#ifdef CRYPTO_USE_SW_ALSO

/**
 * \brief          MD5 context structure
 */
typedef struct {
    uint32_t total[2];			/*!< number of bytes processed	*/
    uint32_t state[4];			/*!< intermediate digest state	*/
    unsigned char buffer[64];	/*!< data block being processed */
}
mbedtls_md5_context;




static const unsigned char padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};



/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = v;
    while( n-- ) *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (little endian)
 */
#ifndef GET_UINT32_LE
#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ]       )             \
        | ( (uint32_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 2] << 16 )             \
        | ( (uint32_t) (b)[(i) + 3] << 24 );            \
}
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n,b,i)                                    \
{                                                               \
    (b)[(i)    ] = (unsigned char) ( ( (n)       ) & 0xFF );    \
    (b)[(i) + 1] = (unsigned char) ( ( (n) >>  8 ) & 0xFF );    \
    (b)[(i) + 2] = (unsigned char) ( ( (n) >> 16 ) & 0xFF );    \
    (b)[(i) + 3] = (unsigned char) ( ( (n) >> 24 ) & 0xFF );    \
}
#endif

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
do {                                                    \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
} while( 0 )
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
do {                                                    \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
} while( 0 )
#endif

void mbedtls_md5_init( mbedtls_md5_context *ctx )
{
    rtl_memset( ctx, 0, sizeof( mbedtls_md5_context ) );
}

void mbedtls_md5_free( mbedtls_md5_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_md5_context ) );
}

void mbedtls_md5_clone( mbedtls_md5_context *dst,
                        const mbedtls_md5_context *src )
{
    *dst = *src;
}

/*
 * MD5 context setup
 */
void mbedtls_md5_starts( mbedtls_md5_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

void mbedtls_md5_process( mbedtls_md5_context *ctx, const unsigned char data[64] )
{
    uint32_t X[16], A, B, C, D;

    GET_UINT32_LE( X[ 0], data,  0 );
    GET_UINT32_LE( X[ 1], data,  4 );
    GET_UINT32_LE( X[ 2], data,  8 );
    GET_UINT32_LE( X[ 3], data, 12 );
    GET_UINT32_LE( X[ 4], data, 16 );
    GET_UINT32_LE( X[ 5], data, 20 );
    GET_UINT32_LE( X[ 6], data, 24 );
    GET_UINT32_LE( X[ 7], data, 28 );
    GET_UINT32_LE( X[ 8], data, 32 );
    GET_UINT32_LE( X[ 9], data, 36 );
    GET_UINT32_LE( X[10], data, 40 );
    GET_UINT32_LE( X[11], data, 44 );
    GET_UINT32_LE( X[12], data, 48 );
    GET_UINT32_LE( X[13], data, 52 );
    GET_UINT32_LE( X[14], data, 56 );
    GET_UINT32_LE( X[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                \
{                                                       \
    a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x,y,z) (z ^ (x & (y ^ z)))

    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );

#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))

    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef F

#define F(x,y,z) (x ^ y ^ z)

    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef F

#define F(x,y,z) (y ^ (x | ~z))

    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

/*
 * MD5 process buffer
 */
void mbedtls_md5_update( mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    size_t fill;
    uint32_t left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill ) {
        rtl_memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_md5_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 ) {
        mbedtls_md5_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 ) {
        rtl_memcpy( (void *) (ctx->buffer + left), input, ilen );
    }
}

/*
 * MD5 final digest
 */
void mbedtls_md5_finish( mbedtls_md5_context *ctx, unsigned char output[16] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_LE( low,  msglen, 0 );
    PUT_UINT32_LE( high, msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    mbedtls_md5_update( ctx, padding, padn );
    mbedtls_md5_update( ctx, msglen, 8 );

    PUT_UINT32_LE( ctx->state[0], output,  0 );
    PUT_UINT32_LE( ctx->state[1], output,  4 );
    PUT_UINT32_LE( ctx->state[2], output,  8 );
    PUT_UINT32_LE( ctx->state[3], output, 12 );
}


void mbedtls_md5_sw( const unsigned char *input, size_t ilen, unsigned char output[16] )
{
    mbedtls_md5_context ctx;

    mbedtls_md5_init( &ctx );
    mbedtls_md5_starts( &ctx );
    mbedtls_md5_update( &ctx, input, ilen );
    mbedtls_md5_finish( &ctx, output );
    mbedtls_md5_free( &ctx );
}


// sha1

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    rtl_memset( ctx, 0, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    *dst = *src;
}

/*
 * SHA-1 context setup
 */
void mbedtls_sha1_starts_sw( mbedtls_sha1_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
}

void mbedtls_sha1_process_sw( mbedtls_sha1_context *ctx, const unsigned char data[64] )
{
    uint32_t temp, W[16], A, B, C, D, E;

    GET_UINT32_BE( W[ 0], data,  0 );
    GET_UINT32_BE( W[ 1], data,  4 );
    GET_UINT32_BE( W[ 2], data,  8 );
    GET_UINT32_BE( W[ 3], data, 12 );
    GET_UINT32_BE( W[ 4], data, 16 );
    GET_UINT32_BE( W[ 5], data, 20 );
    GET_UINT32_BE( W[ 6], data, 24 );
    GET_UINT32_BE( W[ 7], data, 28 );
    GET_UINT32_BE( W[ 8], data, 32 );
    GET_UINT32_BE( W[ 9], data, 36 );
    GET_UINT32_BE( W[10], data, 40 );
    GET_UINT32_BE( W[11], data, 44 );
    GET_UINT32_BE( W[12], data, 48 );
    GET_UINT32_BE( W[13], data, 52 );
    GET_UINT32_BE( W[14], data, 56 );
    GET_UINT32_BE( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                            \
(                                                       \
    temp = W[( t -  3 ) & 0x0F] ^ W[( t - 8 ) & 0x0F] ^ \
           W[( t - 14 ) & 0x0F] ^ W[  t       & 0x0F],  \
    ( W[t & 0x0F] = S(temp,1) )                         \
)

#define P(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P( A, B, C, D, E, W[0]  );
    P( E, A, B, C, D, W[1]  );
    P( D, E, A, B, C, W[2]  );
    P( C, D, E, A, B, W[3]  );
    P( B, C, D, E, A, W[4]  );
    P( A, B, C, D, E, W[5]  );
    P( E, A, B, C, D, W[6]  );
    P( D, E, A, B, C, W[7]  );
    P( C, D, E, A, B, W[8]  );
    P( B, C, D, E, A, W[9]  );
    P( A, B, C, D, E, W[10] );
    P( E, A, B, C, D, W[11] );
    P( D, E, A, B, C, W[12] );
    P( C, D, E, A, B, W[13] );
    P( B, C, D, E, A, W[14] );
    P( A, B, C, D, E, W[15] );
    P( E, A, B, C, D, R(16) );
    P( D, E, A, B, C, R(17) );
    P( C, D, E, A, B, R(18) );
    P( B, C, D, E, A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P( A, B, C, D, E, R(20) );
    P( E, A, B, C, D, R(21) );
    P( D, E, A, B, C, R(22) );
    P( C, D, E, A, B, R(23) );
    P( B, C, D, E, A, R(24) );
    P( A, B, C, D, E, R(25) );
    P( E, A, B, C, D, R(26) );
    P( D, E, A, B, C, R(27) );
    P( C, D, E, A, B, R(28) );
    P( B, C, D, E, A, R(29) );
    P( A, B, C, D, E, R(30) );
    P( E, A, B, C, D, R(31) );
    P( D, E, A, B, C, R(32) );
    P( C, D, E, A, B, R(33) );
    P( B, C, D, E, A, R(34) );
    P( A, B, C, D, E, R(35) );
    P( E, A, B, C, D, R(36) );
    P( D, E, A, B, C, R(37) );
    P( C, D, E, A, B, R(38) );
    P( B, C, D, E, A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P( A, B, C, D, E, R(40) );
    P( E, A, B, C, D, R(41) );
    P( D, E, A, B, C, R(42) );
    P( C, D, E, A, B, R(43) );
    P( B, C, D, E, A, R(44) );
    P( A, B, C, D, E, R(45) );
    P( E, A, B, C, D, R(46) );
    P( D, E, A, B, C, R(47) );
    P( C, D, E, A, B, R(48) );
    P( B, C, D, E, A, R(49) );
    P( A, B, C, D, E, R(50) );
    P( E, A, B, C, D, R(51) );
    P( D, E, A, B, C, R(52) );
    P( C, D, E, A, B, R(53) );
    P( B, C, D, E, A, R(54) );
    P( A, B, C, D, E, R(55) );
    P( E, A, B, C, D, R(56) );
    P( D, E, A, B, C, R(57) );
    P( C, D, E, A, B, R(58) );
    P( B, C, D, E, A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P( A, B, C, D, E, R(60) );
    P( E, A, B, C, D, R(61) );
    P( D, E, A, B, C, R(62) );
    P( C, D, E, A, B, R(63) );
    P( B, C, D, E, A, R(64) );
    P( A, B, C, D, E, R(65) );
    P( E, A, B, C, D, R(66) );
    P( D, E, A, B, C, R(67) );
    P( C, D, E, A, B, R(68) );
    P( B, C, D, E, A, R(69) );
    P( A, B, C, D, E, R(70) );
    P( E, A, B, C, D, R(71) );
    P( D, E, A, B, C, R(72) );
    P( C, D, E, A, B, R(73) );
    P( B, C, D, E, A, R(74) );
    P( A, B, C, D, E, R(75) );
    P( E, A, B, C, D, R(76) );
    P( D, E, A, B, C, R(77) );
    P( C, D, E, A, B, R(78) );
    P( B, C, D, E, A, R(79) );

#undef K
#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}


/*
 * SHA-1 process buffer
 */
void mbedtls_sha1_update_sw( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    size_t fill;
    uint32_t left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill ) {
        rtl_memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_sha1_process_sw( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 ) {
        mbedtls_sha1_process_sw( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        rtl_memcpy( (void *) (ctx->buffer + left), input, ilen );
}

/*
 * SHA-1 final digest
 */
void mbedtls_sha1_finish_sw( mbedtls_sha1_context *ctx, unsigned char output[20] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    mbedtls_sha1_update( ctx, padding, padn );
    mbedtls_sha1_update( ctx, msglen, 8 );

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );
}

void mbedtls_sha1_sw( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init( &ctx );
    mbedtls_sha1_starts( &ctx );
    mbedtls_sha1_update( &ctx, input, ilen );
    mbedtls_sha1_finish( &ctx, output );
    mbedtls_sha1_free( &ctx );
}

//
// sha256
//
void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    rtl_memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if( is224 == 0 ) {
        /* SHA-256 */
        ctx->state[0] = 0x6A09E667;
        ctx->state[1] = 0xBB67AE85;
        ctx->state[2] = 0x3C6EF372;
        ctx->state[3] = 0xA54FF53A;
        ctx->state[4] = 0x510E527F;
        ctx->state[5] = 0x9B05688C;
        ctx->state[6] = 0x1F83D9AB;
        ctx->state[7] = 0x5BE0CD19;
    } else {
        /* SHA-224 */
        ctx->state[0] = 0xC1059ED8;
        ctx->state[1] = 0x367CD507;
        ctx->state[2] = 0x3070DD17;
        ctx->state[3] = 0xF70E5939;
        ctx->state[4] = 0xFFC00B31;
        ctx->state[5] = 0x68581511;
        ctx->state[6] = 0x64F98FA7;
        ctx->state[7] = 0xBEFA4FA4;
    }

    ctx->is224 = is224;
}

static const uint32_t K[] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
(                                               \
    W[t] = S1(W[t -  2]) + W[t -  7] +          \
           S0(W[t - 15]) + W[t - 16]            \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
{                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
}

void mbedtls_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    uint32_t temp1, temp2, W[64];
    uint32_t A[8];
    unsigned int i;

    for( i = 0; i < 8; i++ )
        A[i] = ctx->state[i];

    for( i = 0; i < 64; i++ ) {
        if( i < 16 )
            GET_UINT32_BE( W[i], data, 4 * i );
        else
            R( i );

        P( A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], W[i], K[i] );

        temp1 = A[7];
        A[7] = A[6];
        A[6] = A[5];
        A[5] = A[4];
        A[4] = A[3];
        A[3] = A[2];
        A[2] = A[1];
        A[1] = A[0];
        A[0] = temp1;
    }

    for( i = 0; i < 8; i++ )
        ctx->state[i] += A[i];
}

/*
 * SHA-256 process buffer
 */
void mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input,
                            size_t ilen )
{
    size_t fill;
    uint32_t left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill ) {
        rtl_memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_sha256_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 ) {
        mbedtls_sha256_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        rtl_memcpy( (void *) (ctx->buffer + left), input, ilen );
}

/*
 * SHA-256 final digest
 */
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char output[32] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
           | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    mbedtls_sha256_update( ctx, padding, padn );
    mbedtls_sha256_update( ctx, msglen, 8 );

    PUT_UINT32_BE( ctx->state[0], output,  0 );
    PUT_UINT32_BE( ctx->state[1], output,  4 );
    PUT_UINT32_BE( ctx->state[2], output,  8 );
    PUT_UINT32_BE( ctx->state[3], output, 12 );
    PUT_UINT32_BE( ctx->state[4], output, 16 );
    PUT_UINT32_BE( ctx->state[5], output, 20 );
    PUT_UINT32_BE( ctx->state[6], output, 24 );

    if( ctx->is224 == 0 )
        PUT_UINT32_BE( ctx->state[7], output, 28 );
}

void mbedtls_sha256_sw( const unsigned char *input, size_t ilen,
                        unsigned char output[32], int is224 )
{
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init( &ctx );
    mbedtls_sha256_starts( &ctx, is224 );
    mbedtls_sha256_update( &ctx, input, ilen );
    mbedtls_sha256_finish( &ctx, output );
    mbedtls_sha256_free( &ctx );
}

#endif // ifdef CRYPTO_USE_SW_ALSO



void mbedtls_md5( const unsigned char *input, size_t ilen, unsigned char output[16] )
{
    mbedtls_md5_sw(input, ilen, output);
    //mbedtls_md5_hw(input, ilen, output);
}

/*
 * output = SHA-1( input buffer )
 */

void mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    mbedtls_sha1_sw(input, ilen, output);
}

void mbedtls_sha1_starts_hw( mbedtls_sha1_context *ctx )
{
    rtl_crypto_sha1_init();
}

void mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    mbedtls_sha1_starts_sw(ctx);
}

void mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    mbedtls_sha1_update_sw(ctx, input, ilen);
}

void mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] )
{
    mbedtls_sha1_finish_sw(ctx, output);
}


//
// MD
//

static const int supported_digests[] = {
    MBEDTLS_MD_SHA256,
    MBEDTLS_MD_SHA224,
    MBEDTLS_MD_SHA1,
    MBEDTLS_MD_MD5,
    MBEDTLS_MD_NONE
};

// MD5
static void md5_starts_wrap( void *ctx )
{
    mbedtls_md5_starts( (mbedtls_md5_context *) ctx );
}

static void md5_update_wrap( void *ctx, const unsigned char *input,
                             size_t ilen )
{
    mbedtls_md5_update( (mbedtls_md5_context *) ctx, input, ilen );
}

static void md5_finish_wrap( void *ctx, unsigned char *output )
{
    mbedtls_md5_finish( (mbedtls_md5_context *) ctx, output );
}


static void md5_clone_wrap( void *dst, const void *src )
{
    mbedtls_md5_clone( (mbedtls_md5_context *) dst,
                       (const mbedtls_md5_context *) src );
}

static void md5_process_wrap( void *ctx, const unsigned char *data )
{
    mbedtls_md5_process( (mbedtls_md5_context *) ctx, data );
}


const mbedtls_md_info_t mbedtls_md5_info = {
    MBEDTLS_MD_MD5,
    "MD5",
    16,
    64,
    md5_starts_wrap,
    md5_update_wrap,
    md5_finish_wrap,
    mbedtls_md5,
    NULL,
    NULL,
    md5_clone_wrap,
    md5_process_wrap,
};

// sha1

static void sha1_starts_wrap( void *ctx )
{
    mbedtls_sha1_starts( (mbedtls_sha1_context *) ctx );
}

static void sha1_update_wrap( void *ctx, const unsigned char *input,
                              size_t ilen )
{
    mbedtls_sha1_update( (mbedtls_sha1_context *) ctx, input, ilen );
}

static void sha1_finish_wrap( void *ctx, unsigned char *output )
{
    mbedtls_sha1_finish( (mbedtls_sha1_context *) ctx, output );
}


static void sha1_clone_wrap( void *dst, const void *src )
{
    mbedtls_sha1_clone( (mbedtls_sha1_context *) dst,
                        (const mbedtls_sha1_context *) src );
}

static void sha1_process_wrap( void *ctx, const unsigned char *data )
{
    mbedtls_sha1_process_sw( (mbedtls_sha1_context *) ctx, data );
}

const mbedtls_md_info_t mbedtls_sha1_info = {
    MBEDTLS_MD_SHA1,
    "SHA1",
    20,
    64,
    sha1_starts_wrap,
    sha1_update_wrap,
    sha1_finish_wrap,
    mbedtls_sha1,
    NULL,
    NULL,
    sha1_clone_wrap,
    sha1_process_wrap,
};

// SHA224

static void sha224_starts_wrap( void *ctx )
{
    mbedtls_sha256_starts( (mbedtls_sha256_context *) ctx, 1 );
}

static void sha224_update_wrap( void *ctx, const unsigned char *input,
                                size_t ilen )
{
    mbedtls_sha256_update( (mbedtls_sha256_context *) ctx, input, ilen );
}

static void sha224_finish_wrap( void *ctx, unsigned char *output )
{
    mbedtls_sha256_finish( (mbedtls_sha256_context *) ctx, output );
}

static void sha224_wrap( const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    mbedtls_sha256( input, ilen, output, 1 );
}


static void sha224_clone_wrap( void *dst, const void *src )
{
    mbedtls_sha256_clone( (mbedtls_sha256_context *) dst,
                          (const mbedtls_sha256_context *) src );
}

static void sha224_process_wrap( void *ctx, const unsigned char *data )
{
    mbedtls_sha256_process( (mbedtls_sha256_context *) ctx, data );
}

const mbedtls_md_info_t mbedtls_sha224_info = {
    MBEDTLS_MD_SHA224,
    "SHA224",
    28,
    64,
    sha224_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha224_wrap,
    NULL,
    NULL,
    sha224_clone_wrap,
    sha224_process_wrap,
};


// SHA256

static void sha256_starts_wrap( void *ctx )
{
    mbedtls_sha256_starts( (mbedtls_sha256_context *) ctx, 0 );
}

static void sha256_wrap( const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    mbedtls_sha256( input, ilen, output, 0 );
}

const mbedtls_md_info_t mbedtls_sha256_info = {
    MBEDTLS_MD_SHA256,
    "SHA256",
    32,
    64,
    sha256_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha256_wrap,
    NULL,
    NULL,
    sha224_clone_wrap,
    sha224_process_wrap,
};


const mbedtls_md_info_t *mbedtls_md_info_from_type( mbedtls_md_type_t md_type )
{
    switch( md_type ) {
        case MBEDTLS_MD_MD5:
            return( &mbedtls_md5_info );
        case MBEDTLS_MD_SHA1:
            return( &mbedtls_sha1_info );
        case MBEDTLS_MD_SHA224:
            return( &mbedtls_sha224_info );
        case MBEDTLS_MD_SHA256:
            return( &mbedtls_sha256_info );
        default:
            return( NULL );
    }
}

/*
 * output = SHA-256( input buffer )
 */

void mbedtls_sha256( const unsigned char *input, size_t ilen,
                     unsigned char output[32], int is224 )
{
    mbedtls_sha256_sw(input, ilen, output, is224);
}


//
// HMAC
//
typedef struct {
    /** Information about the associated message digest */
    const mbedtls_md_info_t *md_info;

    /** Digest-specific context */
    void *md_ctx;

    /** HMAC part of the context */
    void *hmac_ctx;
} mbedtls_md_context_t;



int mbedtls_md_hmac_starts( mbedtls_md_context_t *ctx, const unsigned char *key, size_t keylen )
{
    unsigned char sum[MBEDTLS_MD_MAX_SIZE];
    unsigned char *ipad, *opad;
    size_t i;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    if( keylen > (size_t) ctx->md_info->block_size ) {
        ctx->md_info->starts_func( ctx->md_ctx );
        ctx->md_info->update_func( ctx->md_ctx, key, keylen );
        ctx->md_info->finish_func( ctx->md_ctx, sum );

        keylen = ctx->md_info->size;
        key = sum;
    }

    ipad = (unsigned char *) ctx->hmac_ctx;
    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    memset( ipad, 0x36, ctx->md_info->block_size );
    memset( opad, 0x5C, ctx->md_info->block_size );

    for( i = 0; i < keylen; i++ ) {
        ipad[i] = (unsigned char)( ipad[i] ^ key[i] );
        opad[i] = (unsigned char)( opad[i] ^ key[i] );
    }

    mbedtls_zeroize( sum, sizeof( sum ) );

    ctx->md_info->starts_func( ctx->md_ctx );
    ctx->md_info->update_func( ctx->md_ctx, ipad, ctx->md_info->block_size );

    return( 0 );
}

int mbedtls_md_hmac_update( mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    ctx->md_info->update_func( ctx->md_ctx, input, ilen );

    return( 0 );
}

int mbedtls_md_hmac_finish( mbedtls_md_context_t *ctx, unsigned char *output )
{
    unsigned char tmp[MBEDTLS_MD_MAX_SIZE];
    unsigned char *opad;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    ctx->md_info->finish_func( ctx->md_ctx, tmp );
    ctx->md_info->starts_func( ctx->md_ctx );
    ctx->md_info->update_func( ctx->md_ctx, opad, ctx->md_info->block_size );
    ctx->md_info->update_func( ctx->md_ctx, tmp, ctx->md_info->size );
    ctx->md_info->finish_func( ctx->md_ctx, output );

    return( 0 );
}

//

void mbedtls_md_init( mbedtls_md_context_t *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_md_context_t ) );
}



int mbedtls_md_hmac( const mbedtls_md_context_t *ctx, const unsigned char *key, size_t keylen,
                     const unsigned char *input, size_t ilen,
                     unsigned char *output )
{
    int ret;


    mbedtls_md_hmac_starts(ctx, key, keylen );
    mbedtls_md_hmac_update( ctx, input, ilen );
    mbedtls_md_hmac_finish( ctx, output );

    return( 0 );
}


//

mbedtls_md_context_t md_hmac_md5_con;
mbedtls_md_context_t md_hmac_sha1_con;
mbedtls_md_context_t md_hmac_sha224_con;
mbedtls_md_context_t md_hmac_sha256_con;

mbedtls_md5_context md_hmac_md5_ctx;
mbedtls_sha1_context md_hmac_sha1_ctx;
mbedtls_sha256_context md_hmac_sha224_ctx;
mbedtls_sha256_context md_hmac_sha256_ctx;


u8 hmac_ctx_md5[2*64];
u8 hmac_ctx_sha1[2*64];
u8 hmac_ctx_sha224[2*64];
u8 hmac_ctx_sha256[2*64];

void mbedtls_init(void)
{
    md_hmac_md5_con.md_ctx = &md_hmac_md5_ctx;
    md_hmac_md5_con.hmac_ctx = &hmac_ctx_md5[0];
    md_hmac_md5_con.md_info = &mbedtls_md5_info;

    md_hmac_sha1_con.md_ctx = &md_hmac_sha1_ctx;
    md_hmac_sha1_con.hmac_ctx = &hmac_ctx_sha1[0];
    md_hmac_sha1_con.md_info = &mbedtls_sha1_info;

    md_hmac_sha224_con.md_ctx = &md_hmac_sha224_ctx;
    md_hmac_sha224_con.hmac_ctx = &hmac_ctx_sha224[0];
    md_hmac_sha224_con.md_info = &mbedtls_sha224_info;

    md_hmac_sha256_con.md_ctx = &md_hmac_sha256_ctx;
    md_hmac_sha256_con.hmac_ctx = &hmac_ctx_sha256[0];
    md_hmac_sha256_con.md_info = &mbedtls_sha256_info;
}

int mbedtls_md_hmac_md5(const unsigned char *key, size_t keylen,
                        const unsigned char *input, size_t ilen,
                        unsigned char *output )
{
    mbedtls_md_hmac(&md_hmac_md5_con, key, keylen, input, ilen, output);
}

int mbedtls_md_hmac_sha1(const unsigned char *key, size_t keylen,
                         const unsigned char *input, size_t ilen,
                         unsigned char *output )
{
    mbedtls_md_hmac(&md_hmac_sha1_con, key, keylen, input, ilen, output);
}

int mbedtls_md_hmac_sha224(const unsigned char *key, size_t keylen,
                           const unsigned char *input, size_t ilen,
                           unsigned char *output )
{
    mbedtls_md_hmac(&md_hmac_sha224_con, key, keylen, input, ilen, output);
}

int mbedtls_md_hmac_sha256(const unsigned char *key, size_t keylen,
                           const unsigned char *input, size_t ilen,
                           unsigned char *output )
{
    mbedtls_md_hmac(&md_hmac_sha256_con, key, keylen, input, ilen, output);
}

//
// rtl_crypto
//

int rtl_crypto_md5_init(void)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    return _rtl_crypto_md5_init(pIE);
}


const static u8 md5_null_msg_result[16] = {
    0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
    0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E
};

int rtl_crypto_md5_process(
    IN const u8* message, IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if (msglen == 0) {
        rtl_memcpy(pDigest, md5_null_msg_result, 16);
        return 0;
    }

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);


}

int rtl_crypto_md5_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    void *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_md5_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, CRYPTO_MD5_DIGEST_LENGTH );
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);


}


/**
  *  @brief	rtl_crypto_md5()
  * 			 :  Authentication function : MD5
  *
  *				Output = MD5(input buffer)
  *
  *  @param 	message : input buffer
  *  @param    msglen 	: input buffer length
  *  @param    pDigest	: the result of MD5 function
  *  @return     0        	: SUCCESS <br>
  *                  others 	: fail, refer to ERRNO
  */

int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_md5_init();
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_md5_process(message, msglen, pDigest);

    return ret;
}


// SHA1


int rtl_crypto_sha1_init(void)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    return _rtl_crypto_sha1_init(pIE);
}



int rtl_crypto_sha1_process(
    IN const u8* message, 	IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}




int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_sha1_init();
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_sha1_process(message, msglen, pDigest);

    return ret;
}

int rtl_crypto_sha1_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    u8 *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }


    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_sha1_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, CRYPTO_SHA1_DIGEST_LENGTH);
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}


// SHA2


int rtl_crypto_sha2_init(IN const int sha2type)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    return _rtl_crypto_sha2_init(pIE, sha2type);
}



int rtl_crypto_sha2_process(
    IN const u8* message, IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);
}




int rtl_crypto_sha2(
    IN const int sha2type, IN const u8* message, IN const u32 msglen,
    OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_sha2_init(sha2type);
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_sha2_process(message, msglen, pDigest);

    return ret;
}

int rtl_crypto_sha2_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    u8 *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }
    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_sha2_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, pIE->sha2type);
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}



// HMAC-MD5


int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


    return _rtl_crypto_hmac_md5_init(pIE, key, keylen);
}



int rtl_crypto_hmac_md5_process(
    IN const u8* message, 	IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}




int rtl_crypto_hmac_md5(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* key, 		IN const u32 keylen,
    OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_hmac_md5_init(key, keylen);
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_hmac_md5_process(message, msglen, pDigest);

    return ret;
}

int rtl_crypto_hmac_md5_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    u8 *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }
    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_hmac_md5_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_MD5 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;


    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, CRYPTO_MD5_DIGEST_LENGTH);
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);
}


// HMAC-SHA1


int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


    return _rtl_crypto_hmac_sha1_init(pIE, key, keylen);
}



int rtl_crypto_hmac_sha1_process(
    IN const u8* message, 	IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);
}




int rtl_crypto_hmac_sha1(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* key, 		IN const u32 keylen,
    OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_hmac_sha1_init(key, keylen);
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_hmac_sha1_process(message, msglen, pDigest);

    return ret;
}


int rtl_crypto_hmac_sha1_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    u8 *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }
    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_hmac_sha1_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet


    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA1 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, CRYPTO_SHA1_DIGEST_LENGTH);
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}


// HMAC-SHA2


int rtl_crypto_hmac_sha2_init(
    IN const int sha2type,
    IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet
    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4 byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_KEY_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


    return _rtl_crypto_hmac_sha2_init(pIE, sha2type, key, keylen);
}



int rtl_crypto_hmac_sha2_process(
    IN const u8* message, 	IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    if ( msglen < 0 ) return _ERRNO_CRYPTO_MSG_OutRange;
    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);
}




int rtl_crypto_hmac_sha2(
    IN const int sha2type,
    IN const u8* message, 			IN const u32 msglen,
    IN const u8* key, 				IN const u32 keylen,
    OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_hmac_sha2_init(sha2type, key, keylen);
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_hmac_sha2_process(message, msglen, pDigest);

    return ret;
}

int rtl_crypto_hmac_sha2_update(
    IN const u8* message, IN const u32 msglen)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;
    u8 *pDigest;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) {
        return _ERRNO_CRYPTO_NULL_POINTER;
    }


    if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (msglen == 0) || ((msglen % 64) != 0) ) { // not 64 byte alignment, which means the lastone;
        pDigest = pIE->digest_data;
    } else {
        pDigest = NULL;
    }
    return __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}

int rtl_crypto_hmac_sha2_final(
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet


    if ( (pIE->auth_type & AUTH_TYPE_MASK_HMAC) == 0 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( (pIE->auth_type & AUTH_TYPE_MASK_FUNC) != AUTH_TYPE_SHA2 )
        return _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;

    if ( pIE->hmac_seq_hash_last == 1 ) { //already set
        memcpy(pDigest, pIE->digest_data, pIE->sha2type);
        return SUCCESS;
    }

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_cryptoEngine(pIE, NULL, 0, pIV, ivlen, NULL, a2eo, pDigest, NULL);

}


// poly1305

int rtl_crypto_poly1305_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_POLY1305, key, 32);

}



int rtl_crypto_poly1305_process(
    IN const u8* message, 	IN const u32 msglen,
    OUT u8* pDigest)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    //if ( msglen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_MSG_OutRange;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_POLY1305, message, msglen, NULL, 0, NULL, 0, NULL, pDigest);
}




int rtl_crypto_poly1305(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* key, 		IN const u32 keylen,
    OUT u8* pDigest)
{
    int ret;


    ret = rtl_crypto_poly1305_init(key, keylen);
    if ( ret != SUCCESS ) return ret;

    ret = rtl_crypto_poly1305_process(message, msglen, pDigest);

    return ret;
}

// crc32
int rtl_crypto_crc32(
    IN const u8* message,	IN const u32 msglen,
    OUT u32* pCrc)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    int ret;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    ret = __rtl_crypto_crc32(pIE, message, msglen, pCrc);

    return ret;
}

int rtl_crypto_crc_setting(int order, unsigned long polynom, int direct, unsigned long crcinit, unsigned long crcxor, int refin, int refout)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    int ret;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    ret = __rtl_crypto_crc_setting(pIE, order, polynom, direct, crcinit, crcxor, refin, refout);

    return ret;
}

int rtl_crypto_crc(
    IN const u8* message,	IN const u32 msglen,
    OUT u32* pCrc)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    int ret;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    ret = __rtl_crypto_crc(pIE, message, msglen, pCrc);

    return ret;
}



// 3DES length 8 byte alignment


int rtl_crypto_aes_cbc_init(IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_CBC, key, keylen);

}


int rtl_crypto_aes_cbc_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;


    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_cbc_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen != 16 ) return _ERRNO_CRYPTO_IV_OutRange;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_ECB, key, keylen);

}


int rtl_crypto_aes_ecb_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

    if ( ivlen != 0 ) return _ERRNO_CRYPTO_IV_OutRange; // ECB mode : ivlen = 0


    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_ecb_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

    if ( ivlen != 0 ) return _ERRNO_CRYPTO_IV_OutRange; // ECB mode : ivlen = 0

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_ctr_init(IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_CTR, key, keylen);
    //return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_GCTR, key, keylen);
}



int rtl_crypto_aes_ctr_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);


}



int rtl_crypto_aes_ctr_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;


    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_CTR, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);


}


// cfb

int rtl_crypto_aes_cfb_init(IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_CFB, key, keylen);

}

int rtl_crypto_aes_cfb_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_CFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_cfb_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;


    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}

// ofb

int rtl_crypto_aes_ofb_init(IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_OFB, key, keylen);

}

int rtl_crypto_aes_ofb_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;

    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}



int rtl_crypto_aes_ofb_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (msglen % 16) != 0 ) return _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned;


    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_OFB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


// gcm

int rtl_crypto_aes_gcm_init(IN const u8* key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( (keylen != 128/8) && (keylen != 192/8) && (keylen != 256/8) ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_AES_GCM, key, keylen);

}

static u8 gcm_iv[16];
static u8 gcm_iv_tail[] = { 0x00, 0x00, 0x00, 0x01 };

int rtl_crypto_aes_gcm_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv,
    IN const u8* aad, 		IN const u32 aadlen,
    OUT u8* pResult, OUT u8* pTag)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pTag == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    memcpy(gcm_iv, iv, 96/8);
    memcpy(&gcm_iv[12], gcm_iv_tail, 4);
    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_AES_GCM, message, msglen, gcm_iv, 16, aad, aadlen, pResult, pTag);

}



int rtl_crypto_aes_gcm_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv,
    IN const u8* aad, 		IN const u32 aadlen,
    OUT u8* pResult, OUT u8* pTag)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    memcpy(gcm_iv, iv, 96/8);
    memcpy(&gcm_iv[12], gcm_iv_tail, 4);

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_AES_GCM, message, msglen, gcm_iv, 16, aad, aadlen, pResult, pTag);

}


//
// 3DES-CBC
//

int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_3DES_CBC, key, keylen);

}


int rtl_crypto_3des_cbc_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


int rtl_crypto_3des_cbc_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_3DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


//
// 3DES-ECB
//

int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;


    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_3DES_ECB, key, keylen);

}

int rtl_crypto_3des_ecb_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_3DES_ECB, message, msglen, NULL, 0, NULL, 0, pResult, NULL);

}

int rtl_crypto_3des_ecb_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_3DES_ECB, message, msglen, NULL, 0, NULL, 0, pResult, NULL);

}

//
// DES-CBC
//
int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_DES_CBC, key, keylen);

}

int rtl_crypto_des_cbc_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}

int rtl_crypto_des_cbc_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_DES_CBC, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


//
// DES-ECB
//

int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( keylen < 0 ) return _ERRNO_CRYPTO_KEY_OutRange;
    if ( keylen > CRYPTO_MAX_MSG_LENGTH ) return _ERRNO_CRYPTO_KEY_OutRange;

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_DES_ECB, key, keylen);

}

int rtl_crypto_des_ecb_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 ivlen,
    OUT u8* pResult)
{

    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;

    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


int rtl_crypto_des_ecb_decrypt(
    IN const u8* message, 		IN const u32 msglen,
    IN const u8* iv, 			IN const u32 ivlen,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;



    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment
    if ( ivlen < 0 ) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen > CRYPTO_MAX_KEY_LENGTH) return _ERRNO_CRYPTO_IV_OutRange;
    if ( ivlen != pIE->lenCipherKey ) return _ERRNO_CRYPTO_KEY_IV_LEN_DIFF;

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_DES_ECB, message, msglen, iv, ivlen, NULL, 0, pResult, NULL);

}


//
// keylen = 32
//
int rtl_crypto_chacha_init(IN const u8* key)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_CHACHA, key, 32);

}


int rtl_crypto_chacha_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 count,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    u32 count_be;
    u8 count_str[4];
    int i,j, pos;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment


    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    //
    count_be=count;
    __rtl_cpu_to_be32(count_be);
    memcpy(count_str, &count_be, sizeof(u32));
    memcpy(&gcm_iv[0], count_str, 4);

    memcpy(&gcm_iv[4], iv, 96/8);

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_CHACHA, message, msglen, gcm_iv, 16, NULL, 0, pResult, NULL);

}



int rtl_crypto_chacha_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* iv, 		IN const u32 count,
    OUT u8* pResult)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    u32 count_be;
    u8 count_str[4];
    int i,j, pos;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( iv == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(iv) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;

    //
    count_be=count;
    __rtl_cpu_to_be32(count_be);
    memcpy(count_str, &count_be, sizeof(u32));
    memcpy(&gcm_iv[0], count_str, 4);


    memcpy(&gcm_iv[4], iv, 96/8);

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_CHACHA, message, msglen, gcm_iv, 16, NULL, 0, pResult, NULL);


}


// chacha_poly1305

int rtl_crypto_chacha_poly1305_init(IN const u8* key)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( key == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( (u32)(key) & 0x3 ) return _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned; // need to be 4-byte alignment

    return __rtl_crypto_cipher_init(pIE, CIPHER_TYPE_CHACHA_POLY1305, key, 32);

}


int rtl_crypto_chacha_poly1305_encrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* nonce,
    IN const u8* aad,	IN const u32 aadlen,
    OUT u8* pResult, OUT u8* pDigest)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    u32 count_be;
    u8 count_str[4];
    int i,j, pos;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    //
    count_be=1;
    __rtl_cpu_to_be32(count_be);
    memcpy(count_str, &count_be, sizeof(u32));
    memcpy(&gcm_iv[0], count_str, 4);

    memcpy(&gcm_iv[4], nonce, 96/8);

    return __rtl_crypto_cipher_encrypt(pIE, CIPHER_TYPE_CHACHA_POLY1305, message, msglen, gcm_iv, 16, aad, aadlen, pResult, pDigest);

}


int rtl_crypto_chacha_poly1305_decrypt(
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* nonce,
    IN const u8* aad,	IN const u32 aadlen,
    OUT u8* pResult, OUT u8* pDigest)
{
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
    u32 count_be;
    u8 count_str[4];
    int i,j, pos;


    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    if ( message == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pResult == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;
    if ( pDigest == NULL ) return _ERRNO_CRYPTO_NULL_POINTER;


    //
    count_be=1;
    __rtl_cpu_to_be32(count_be);
    memcpy(count_str, &count_be, sizeof(u32));
    memcpy(&gcm_iv[0], count_str, 4);

    memcpy(&gcm_iv[4], nonce, 96/8);

    return __rtl_crypto_cipher_decrypt(pIE, CIPHER_TYPE_CHACHA_POLY1305, message, msglen, gcm_iv, 16, aad, aadlen, pResult, pDigest);

}



//
// HAL CRYPTO RAM Wrapper functions
//






/**************************************************************************
 * External Functions in RAM
 **************************************************************************/






//
// Public Functions
//


int _rtl_crypto_md5_init(void* pIE1)
{
    hal_crypto_adapter_t* pIE = (hal_crypto_adapter_t*) pIE1;

    const u8* pCipherKey 	= NULL;
    const u32 lenCipherKey 	= 0;
    const u8* pAuthKey 		= NULL;
    const u32 lenAuthKey 	= 0;

    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

    return __rtl_cryptoEngine_set_security_mode(pIE,
            CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_MD5,
            pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}


#if 0
int _rtl_crypto_auth_process(void *pIE1,
                             IN const u8 *message, 	IN const u32 msglen,
                             OUT u8 *pDigest)
{
    HAL_CRYPTO_ADAPTER *pIE = (HAL_CRYPTO_ADAPTER*) pIE1;
    int ret;
    const u8* pIV 	= NULL;
    const u32 ivlen = 0;
    const u32 a2eo 	= 0;

    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

    ret = __rtl_cryptoEngine(pIE, message, msglen, pIV, ivlen, NULL, a2eo, pDigest, NULL);


    return ret;
}
#endif




int _rtl_crypto_sha1_init(void *pIE1)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    const u8* pCipherKey 	= NULL;
    const u32 lenCipherKey 	= 0;
    const u8* pAuthKey 		= NULL;
    const u32 lenAuthKey 	= 0;

    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

    return __rtl_cryptoEngine_set_security_mode(pIE, CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_SHA1,
            pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}




int _rtl_crypto_sha2_init(void *pIE1, IN const int sha2type)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    int auth_type;
    const u8* pCipherKey 	= NULL;
    const u32 lenCipherKey 	= 0;
    const u8* pAuthKey 		= NULL;
    const u32 lenAuthKey 	= 0;

    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

    auth_type = AUTH_TYPE_SHA2;

    switch (sha2type) {
        case SHA2_224 :
            auth_type |= AUTH_TYPE_SHA2_224;
            break;
        case SHA2_256 :
            auth_type |= AUTH_TYPE_SHA2_256;
            break;
        case SHA2_384 :
        case SHA2_512 :
        default:
            return FAIL;
    }

    return __rtl_cryptoEngine_set_security_mode(pIE,
            CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, auth_type,
            pCipherKey, lenCipherKey, pAuthKey, lenAuthKey);

}




int _rtl_crypto_hmac_md5_init(void *pIE1,
                              IN const u8 *key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    u8* pCipherKey = NULL;
    u32 lenCipherKey = 0;


    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );


    return __rtl_cryptoEngine_set_security_mode(pIE, CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_MD5,
            pCipherKey, lenCipherKey, key, keylen);

}




int _rtl_crypto_hmac_sha1_init(void *pIE1,
                               IN const u8 *key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    u8* pCipherKey = NULL;
    u32 lenCipherKey = 0;


    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );


    return __rtl_cryptoEngine_set_security_mode(pIE, CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, AUTH_TYPE_HMAC_SHA1,
            pCipherKey, lenCipherKey, key, keylen);

}





int _rtl_crypto_hmac_sha2_init(void *pIE1,
                               IN const int sha2type, 	IN const u8 *key, 	IN const u32 keylen)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    int auth_type;
    u8* pCipherKey = NULL;
    u32 lenCipherKey = 0;

    rtl_assert( (pIE != NULL) && (pIE->isInit == _TRUE) );

    auth_type = AUTH_TYPE_HMAC_SHA2;

    switch (sha2type) {
        case SHA2_224 :
            auth_type |= AUTH_TYPE_SHA2_224;
            break;
        case SHA2_256 :
            auth_type |= AUTH_TYPE_SHA2_256;
            break;
        case SHA2_384 :
        case SHA2_512 :
        default:
            return FAIL;
    }

    return __rtl_cryptoEngine_set_security_mode(pIE,
            CRYPTO_MS_TYPE_AUTH, CIPHER_TYPE_NO_CIPHER, auth_type,
            pCipherKey, lenCipherKey, key, keylen);

}

int _rtl_crypto_mix_init(void *pIE1,
                         IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
                         IN const void* pCipherKey, IN const u32 lenCipherKey,
                         IN const void* pAuthKey, IN const u32 lenAuthKey)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;

    printk("(%s-%d)\n", __func__, __LINE__);
    return __rtl_cryptoEngine_set_security_mode(pIE, mode_select, cipher_type, auth_type, pCipherKey,
            lenCipherKey, pAuthKey, lenAuthKey);
}

int rtl_crypto_mix_init(IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
                        IN const void* pCipherKey, IN const u32 lenCipherKey,
                        IN const void* pAuthKey, IN const u32 lenAuthKey)
{
#if 1
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
#else
    HAL_CRYPTO_ADAPTER *pIE = (void *) (0x9fe00000+1024*6);
#endif

    printk("(%s-%d)\n", __func__, __LINE__);
    if ( pIE->isInit != _TRUE ) return _ERRNO_CRYPTO_ENGINE_NOT_INIT; // not init yet

    printk("(%s-%d)\n", __func__, __LINE__);
    return _rtl_crypto_mix_init(pIE, mode_select, cipher_type, auth_type, pCipherKey,
                                lenCipherKey, pAuthKey, lenAuthKey);
}

int _rtl_crypto_mix_process( void *pIE1,
                             IN const u8 *message, IN const u32 msglen, IN const u8 *pIv,
                             IN const u32 ivlen, IN const u32 a2eo, OUT void *pResult,
                             OUT void *pDigest)
{
    hal_crypto_adapter_t *pIE = (hal_crypto_adapter_t*) pIE1;
    printk("(%s-%d)\n", __func__, __LINE__);

    return __rtl_cryptoEngineMix(pIE, message, msglen, pIv, ivlen, a2eo, pResult, pDigest);
}


int rtl_crypto_mix_process(IN const u8 *message, IN const u32 msglen, IN const u8 *pIv,
                           IN const u32 ivlen, IN const u32 a2eo, OUT void *pResult,
                           OUT void *pDigest)
{
#if 1
    hal_crypto_adapter_t *pIE = &g_rtl_cipherEngine;
#else
    HAL_CRYPTO_ADAPTER *pIE = (void *) (0x9fe00000+1024*6);
#endif

    return _rtl_crypto_mix_process(pIE, message, msglen, pIv, ivlen, a2eo, pResult, pDigest);
}

int rtl_crypto_mix(IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type, IN const u8* message, IN const u32 msglen,
                   IN u32 lenCipherKey, IN void* pCipherKey, IN u32 lenAuthKey, IN void* pAuthkey,
                   IN void* pIv, IN const u32 ivlen, IN u32 a2eo, OUT void* pResult,
                   OUT void* pDigest)
{
    int ret;

    printk("(%s-%d)\n", __func__, __LINE__);
    ret = rtl_crypto_mix_init(mode_select, cipher_type, auth_type, pCipherKey,
                              lenCipherKey, pAuthkey, lenAuthKey);
    if ( ret != SUCCESS ) return ret;
    printk("(%s-%d)\n", __func__, __LINE__);

    ret = rtl_crypto_mix_process(message, msglen, pIv, ivlen, a2eo, pResult, pDigest);

    printk("(%s-%d)\n", __func__, __LINE__);
    return ret;
}
