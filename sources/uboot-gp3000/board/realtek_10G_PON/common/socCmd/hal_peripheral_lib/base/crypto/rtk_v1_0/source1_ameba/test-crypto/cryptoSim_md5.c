/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "cryptoSim_md5.h"
#include "section_config.h"

#include "hal_crypto.h"


#define LOCAL_LITTLE_ENDIAN 1234
#define LOCAL_BIG_ENDIAN 4321

#ifdef _LITTLE_ENDIAN
#define BYTE_ORDER  LOCAL_LITTLE_ENDIAN
#else
#define BYTE_ORDER  LOCAL_BIG_ENDIAN
#endif

#if !defined(BYTE_ORDER) || (BYTE_ORDER != LOCAL_LITTLE_ENDIAN && BYTE_ORDER != LOCAL_BIG_ENDIAN)
#error Define BYTE_ORDER to be equal to either LOCAL_LITTLE_ENDIAN or LOCAL_BIG_ENDIAN
#endif


#if !defined(POLARSSL_MD5_ALT)

/*
 * 32-bit integer manipulation macros (little endian)
 */
#if 0
#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3] );            \
}

#define PUT_UINT32_LE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) );       \
}


#else
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
#define PUT_UINT32_LE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif
#endif
/*
 * MD5 context setup
 */
void md5_starts( md5_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

void md5_process( md5_context *ctx, const unsigned char data[64] )
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
void md5_update( md5_context *ctx, const unsigned char *input, size_t ilen )
{
    size_t fill;
    uint32_t left;

    if( ilen <= 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        rtl_memcpyb( (u8*) (ctx->buffer + left), input, fill );
        md5_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        md5_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
    {
        rtl_memcpyb( (u8*) (ctx->buffer + left), input, ilen );
    }
}

static const unsigned char md5_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * MD5 final digest
 */
void md5_finish( md5_context *ctx, unsigned char output[16] )
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

    md5_update( ctx, md5_padding, padn );
    md5_update( ctx, msglen, 8 );

    PUT_UINT32_LE( ctx->state[0], output,  0 );
    PUT_UINT32_LE( ctx->state[1], output,  4 );
    PUT_UINT32_LE( ctx->state[2], output,  8 );
    PUT_UINT32_LE( ctx->state[3], output, 12 );

#if 0
    u32 i =0;
	printf("%s ==>pad len: %d\n", __FUNCTION__ ,padn);
    for (i=0; i<padn; i=i+4) {
        printf("Index: %d ==> ",i);
        printf("Address: 0x%x: %02x, ",&md5_padding, md5_padding[i]);
        printf("%02x, ",md5_padding[i+1]);
        printf("%02x, ",md5_padding[i+2]);
        printf("%02x\n",md5_padding[i+3]);
    }    

    printf("%s ==>last length: \n", __FUNCTION__);
    for (i=0; i<(8); i=i+4) {
        printf("Index: %d ==> ",i);
        printf("Address: 0x%x: %02x, ",&msglen, msglen[i]);
        printf("%02x, ",msglen[i+1]);
        printf("%02x, ",msglen[i+2]);
        printf("%02x\n",msglen[i+3]);
    }    
#endif
}

#endif /* !POLARSSL_MD5_ALT */

/*
 * output = MD5( input buffer )
 */
void md5( const unsigned char *input, size_t ilen, unsigned char output[16] )
{
    md5_context ctx;

    md5_starts( &ctx );
    md5_update( &ctx, input, ilen );
    md5_finish( &ctx, output );

    rtl_memsetb( (u8*)&ctx, 0, sizeof( md5_context ) );
}


/*
 * MD5 HMAC context setup
 */
void md5_hmac_starts( md5_context *ctx, const unsigned char *key, size_t keylen )
{
    size_t i;
    unsigned char sum[16];

    if( keylen > 64 )
    {
        md5( key, keylen, sum );
        keylen = 16;
        key = sum;
    }

    rtl_memsetw( (u32*)(&ctx->ipad[0]), 0x36363636, 64 /4);
    rtl_memsetw( (u32*)(&ctx->opad[0]), 0x5C5C5C5C, 64 /4);

    for( i = 0; i < keylen; i++ )
    {
        ctx->ipad[i] = (unsigned char)( ctx->ipad[i] ^ key[i] );
        ctx->opad[i] = (unsigned char)( ctx->opad[i] ^ key[i] );
    }

#if 0
    printf("%s ==>IPAD: \n", __FUNCTION__);
    for (i=0; i<64; i = i+4) {      
        printf("%02x ", ctx->ipad[i]);
        printf("%02x ", ctx->ipad[i+1]);
        printf("%02x ", ctx->ipad[i+2]);
        printf("%02x \n", ctx->ipad[i+3]);
    }

    printf("%s ==>OPAD: \n", __FUNCTION__);
    for (i=0; i<64; i = i+4) {      
        printf("%02x ", ctx->opad[i]);
        printf("%02x ", ctx->opad[i+1]);
        printf("%02x ", ctx->opad[i+2]);
        printf("%02x \n", ctx->opad[i+3]);
    }
#endif

    md5_starts( ctx );
    md5_update( ctx, ctx->ipad, 64 );

    rtl_memsetb( sum, 0, sizeof( sum ) );
}

/*
 * MD5 HMAC process buffer
 */
void md5_hmac_update( md5_context *ctx, const unsigned char *input, size_t ilen )
{
    md5_update( ctx, input, ilen );
}

/*
 * MD5 HMAC final digest
 */
void md5_hmac_finish( md5_context *ctx, unsigned char output[16] )
{
    unsigned char tmpbuf[16];

    md5_finish( ctx, tmpbuf );
    md5_starts( ctx );
    md5_update( ctx, ctx->opad, 64 );
    md5_update( ctx, tmpbuf, 16 );
    md5_finish( ctx, output );

    rtl_memsetb( tmpbuf, 0, sizeof( tmpbuf ) );
}

/*
 * MD5 HMAC context reset
 */
void md5_hmac_reset( md5_context *ctx )
{
    md5_starts( ctx );
    md5_update( ctx, ctx->ipad, 64 );
}

/*
 * output = HMAC-MD5( hmac key, input buffer )
 */
void md5_hmac( const unsigned char *key, size_t keylen,
               const unsigned char *input, size_t ilen,
               unsigned char output[16] )
{
    md5_context ctx;

    md5_hmac_starts( &ctx, key, keylen );
    md5_hmac_update( &ctx, input, ilen );
    md5_hmac_finish( &ctx, output );

    rtl_memsetb( (u8*)&ctx, 0, sizeof( md5_context ) );
}


