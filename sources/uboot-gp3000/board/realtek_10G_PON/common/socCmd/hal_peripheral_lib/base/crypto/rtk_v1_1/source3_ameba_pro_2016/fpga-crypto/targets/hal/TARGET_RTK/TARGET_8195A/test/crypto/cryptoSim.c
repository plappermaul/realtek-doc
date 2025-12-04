/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003
* All rights reserved.
*
* Program : Simulate DES behavior and providing final DES driver interface
* Abstract :
* $Id: aesSim.c,v 1.2 2006-07-13 16:03:06 chenyl Exp $
*/
#include "hal_crypto.h"
#include "cryptoSim.h"

#include "section_config.h"

#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"


mbedtls_aes_context ctx;
mbedtls_gcm_context ctx_gcm;

int aesSim_aes(const u32 mode, const u8 *input, const u32 len, const u32 lenKey,
               const u8 *key, const u8 *iv , const u32 lenAAD, const u8 *aad, u8* output, u8* tag)
{
    AES_KEY aesKey;
    int done;
    u8 _iv[16]; /* Since soft_AES_cbc_encrypt() will modify iv, we need to allocate one for it. */
    u8 counter[AES_BLOCK_SIZE];
    u8 ecount_buf[AES_BLOCK_SIZE];
    uint32 num;
    int ret;

    size_t offset;
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];

    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;


    mbedtls_aes_init( &ctx );
    mbedtls_gcm_init( &ctx_gcm );

    //CTR
    offset = 0;
    ret = SUCCESS;

    switch ( mode ) {
        case SWDECRYPT_CBC_AES:
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //AES_set_decrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //soft_AES_cbc_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, FALSE );
            mbedtls_aes_setkey_dec( &ctx, key, lenKey*8 );
            mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_DECRYPT, len, iv, input, output);
            break;

        case SWDECRYPT_ECB_AES:
            //AES_set_decrypt_key( (const u8 *) key, lenKey*8, &aesKey );

            //for( done = 0; done < len; done += 16 )
            //    AES_ecb_encrypt( (const u8 *) &input[done], (u8 *) &output[done], &aesKey, AES_DECRYPT );

            mbedtls_aes_setkey_dec( &ctx, key, lenKey*8 );
            for( done = 0; done < len; done += 16 ) {
                mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_DECRYPT, (const u8 *) &input[done], (u8 *) &output[done] );
            }

            break;

        case SWDECRYPT_CTR_AES:
            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( nonce_counter, iv, sizeof(nonce_counter) );
            rtl_memset( stream_block, 0, sizeof(stream_block) );
            offset = 0;

            mbedtls_aes_crypt_ctr( &ctx, len, &offset, nonce_counter, stream_block,
                                   input, output );
            break;

        case SWDECRYPT_CFB_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //num = 0;
            //AES_cfb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num, FALSE );

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( _iv, iv, sizeof(_iv) );
            offset = 0;

            mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_DECRYPT, len, &offset, _iv,
                                      input, output );

            break;

        case SWDECRYPT_OFB_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //num = 0;
            //AES_ofb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num);

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( _iv, iv, sizeof(_iv) );
            offset = 0;

            mbedtls_aes_crypt_ofb128( &ctx, len, &offset, _iv,
                                      input, output );

            break;

        case  SWDECRYPT_GCM_AES:
            ret = mbedtls_gcm_setkey( &ctx_gcm, cipher, key, lenKey*8 );
            if ( ret != 0 ) {
                DiagPrintf("%s : mbedtls_gcm_setkey return 0x%x \r\n", __FUNCTION__, ret);
                break;
            }

            ret = mbedtls_gcm_crypt_and_tag( &ctx_gcm, MBEDTLS_GCM_DECRYPT,
                                             len,
                                             iv, 12,
                                             aad, lenAAD,
                                             input, output, 16, tag);

            break;


        case SWENCRYPT_CBC_AES:
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //soft_AES_cbc_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, TRUE );
            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_ENCRYPT, len, iv, input, output);
            break;

        case SWENCRYPT_ECB_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //for( done = 0; done < len; done += 16 )
            //    AES_ecb_encrypt( (const u8 *) &input[done], (u8 *) &output[done], &aesKey, AES_ENCRYPT );

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            for( done = 0; done < len; done += 16 ) {
                mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_ENCRYPT, (const u8 *) &input[done], (u8 *) &output[done] );
            }
            break;

        case SWENCRYPT_CTR_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //rtl_memcpy( counter, iv, sizeof(counter) );
            //rtl_memset( ecount_buf, 0, sizeof(ecount_buf) );
            //num = 0;
            //AES_ctr128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, counter, ecount_buf, &num );

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( nonce_counter, iv, sizeof(nonce_counter) );
            rtl_memset( stream_block, 0, sizeof(stream_block) );
            offset = 0;

            mbedtls_aes_crypt_ctr( &ctx, len, &offset, nonce_counter, stream_block,
                                   input, output );
            break;

        case SWENCRYPT_CFB_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //num = 0;
            //AES_cfb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num, TRUE );

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( _iv, iv, sizeof(_iv) );
            offset = 0;

            mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_ENCRYPT, len, &offset, _iv,
                                      input, output );
            break;

        case SWENCRYPT_OFB_AES:
            //AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            //rtl_memcpy( _iv, iv, sizeof(_iv) );
            //num = 0;
            //AES_ofb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num);

            mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            rtl_memcpy( _iv, iv, sizeof(_iv) );
            offset = 0;

            mbedtls_aes_crypt_ofb128( &ctx, len, &offset, _iv,
                                      input, output );

            break;

        case  SWENCRYPT_GCM_AES:
            ret = mbedtls_gcm_setkey( &ctx_gcm, cipher, key, lenKey*8 );
            if ( ret != 0 ) {
                DiagPrintf("%s : mbedtls_gcm_setkey return 0x%x \r\n", __FUNCTION__, ret);
                break;
            }

            ret = mbedtls_gcm_crypt_and_tag( &ctx_gcm, MBEDTLS_GCM_ENCRYPT,
                                             len,
                                             iv, 12,
                                             aad, lenAAD,
                                             input, output, 16, tag);
            if ( ret != 0 ) {
                DiagPrintf("%s : mbedtls_gcm_crypt_and_tag return 0x%x \r\n", __FUNCTION__, ret);
            }
            break;

    }


    mbedtls_aes_free( &ctx );
    mbedtls_gcm_free( &ctx_gcm );
    return ret;
}

// DES



//data, key and iv does not have 4-byte alignment limitatiuon
int desSim_des(const u32 mode, const u8 *input, const u32 len, const u8 *key, const u8 *iv, u8 * output)
{
    int ret;
    mbedtls_des_context ctx;
    mbedtls_des3_context ctx3;

    //
    mbedtls_des_init( &ctx );
    mbedtls_des3_init( &ctx3 );

    ret = FAILED;

    switch(mode) {
        case  SWDECRYPT_CBC_DES:
            //return desSim_cbc_encrypt(input, output, len, key, iv, FALSE);
            ret = mbedtls_des_setkey_dec( &ctx, key);
            if ( ret != 0 ) goto desSim_des_end;
            ret = mbedtls_des_crypt_cbc( &ctx, MBEDTLS_DES_DECRYPT, len, iv, input, output);
            if ( ret != 0 ) goto desSim_des_end;
            break;

        case  SWDECRYPT_ECB_DES:
            //return desSim_ecb_encrypt(input, output, len, key, FALSE);
            ret = mbedtls_des_setkey_dec( &ctx, key);
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des_crypt_ecb( &ctx, len, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

        case  SWDECRYPT_CBC_3DES:
            //return desSim_ede_cbc_encrypt(input, output, len, key, iv, FALSE);
            ret = mbedtls_des3_set3key_dec( &ctx3, key );
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des3_crypt_cbc( &ctx3, MBEDTLS_DES_DECRYPT, len, iv, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;


        case  SWDECRYPT_ECB_3DES:
            //return desSim_ede_ecb_encrypt(input, output, len, key, FALSE);
            ret = mbedtls_des3_set3key_dec( &ctx3, key );
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des3_crypt_ecb( &ctx3, len, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

        case  SWENCRYPT_CBC_DES:
            //return desSim_cbc_encrypt(input, output, len, key, iv, TRUE);
            ret = mbedtls_des_setkey_enc( &ctx, key );
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des_crypt_cbc( &ctx, MBEDTLS_DES_ENCRYPT, len, iv, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

        case  SWENCRYPT_ECB_DES:
            //return desSim_ecb_encrypt(input, output, len, key, TRUE);
            ret = mbedtls_des_setkey_enc( &ctx, key);
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des_crypt_ecb( &ctx, len, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

        case  SWENCRYPT_CBC_3DES:
            //return desSim_ede_cbc_encrypt(input, output, len, key, iv, TRUE);
            ret = mbedtls_des3_set3key_enc( &ctx3, key );
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des3_crypt_cbc( &ctx3, MBEDTLS_DES_ENCRYPT, len, iv, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

        case  SWENCRYPT_ECB_3DES:
            //return desSim_ede_ecb_encrypt(input, output, len, key, TRUE);
            ret = mbedtls_des3_set3key_enc( &ctx3, key );
            if ( ret != 0 )  goto desSim_des_end;
            ret = mbedtls_des3_crypt_ecb( &ctx3, len, input, output);
            if ( ret != 0 )  goto desSim_des_end;
            break;

    }

    ret = SUCCESS;
desSim_des_end:
    mbedtls_des_free( &ctx );
    mbedtls_des3_free( &ctx3 );
    return ret;
}


//
// chacha20
//

static const char sigma[16] = "expand 32-byte k";

#define ROTATE(v, n) (((v) << (n)) | ((v) >> (32 - (n))))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(x, y) ((x) + (y))
#define PLUSONE(v) (PLUS((v), 1))

#define U32TO8_LITTLE(p, v) \
	{ (p)[0] = (v >>  0) & 0xff; (p)[1] = (v >>  8) & 0xff; \
	  (p)[2] = (v >> 16) & 0xff; (p)[3] = (v >> 24) & 0xff; }
#define U8TO32_LITTLE(p)   \
	(((uint32_t)((p)[0])      ) | ((uint32_t)((p)[1]) <<  8) | \
	 ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24)   )

/* QUARTERROUND updates a, b, c, d with a ChaCha "quarter" round. */
#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

/* chacha_core performs |num_rounds| rounds of ChaCha20 on the input words in
 * |input| and writes the 64 output bytes to |output|. */
static void chacha_core(unsigned char output[64], const uint32_t input[16],
                        int num_rounds)
{
    uint32_t x[16];
    int i;

    memcpy(x, input, sizeof(uint32_t) * 16);
    for (i = 20; i > 0; i -= 2)	{
        QUARTERROUND( 0, 4, 8,12)
        QUARTERROUND( 1, 5, 9,13)
        QUARTERROUND( 2, 6,10,14)
        QUARTERROUND( 3, 7,11,15)
        QUARTERROUND( 0, 5,10,15)
        QUARTERROUND( 1, 6,11,12)
        QUARTERROUND( 2, 7, 8,13)
        QUARTERROUND( 3, 4, 9,14)
    }

    for (i = 0; i < 16; ++i)
        x[i] = PLUS(x[i], input[i]);
    for (i = 0; i < 16; ++i)
        U32TO8_LITTLE(output + 4 * i, x[i]);
}

void CRYPTO_sim_chacha_20(unsigned char *out,
                          const unsigned char *in, size_t in_len,
                          const unsigned char key[32],
                          const unsigned char nonce[12],
                          size_t counter)
{
    uint32_t input[16];
    unsigned char buf[64];
    size_t todo, i;

    input[0] = U8TO32_LITTLE(sigma + 0);
    input[1] = U8TO32_LITTLE(sigma + 4);
    input[2] = U8TO32_LITTLE(sigma + 8);
    input[3] = U8TO32_LITTLE(sigma + 12);

    input[4] = U8TO32_LITTLE(key + 0);
    input[5] = U8TO32_LITTLE(key + 4);
    input[6] = U8TO32_LITTLE(key + 8);
    input[7] = U8TO32_LITTLE(key + 12);

    input[8] = U8TO32_LITTLE(key + 16);
    input[9] = U8TO32_LITTLE(key + 20);
    input[10] = U8TO32_LITTLE(key + 24);
    input[11] = U8TO32_LITTLE(key + 28);

    input[12] = counter;
    //input[13] = ((uint64_t) counter) >> 32;
    input[13] = U8TO32_LITTLE(nonce + 0);
    input[14] = U8TO32_LITTLE(nonce + 4);
    input[15] = U8TO32_LITTLE(nonce + 8);

    while (in_len > 0) {
        todo = sizeof(buf);
        if (in_len < todo)
            todo = in_len;

        chacha_core(buf, input, 20);
        for (i = 0; i < todo; i++)
            out[i] = in[i] ^ buf[i];

        out += todo;
        in += todo;
        in_len -= todo;

        input[12]++;
        if (input[12] == 0)
            input[13]++;
    }
}

//
// poly1305
//

static uint32_t U8TO32_LE(const unsigned char *m)
{
    return (uint32_t)m[0] |
           (uint32_t)m[1] << 8 |
           (uint32_t)m[2] << 16 |
           (uint32_t)m[3] << 24;
}

static void U32TO8_LE(unsigned char *m, uint32_t v)
{
    m[0] = v;
    m[1] = v >> 8;
    m[2] = v >> 16;
    m[3] = v >> 24;
}


static uint64_t
mul32x32_64(uint32_t a, uint32_t b)
{
    return (uint64_t)a * b;
}

/* poly1305_blocks updates |state| given some amount of input data. This
 * function may only be called with a |len| that is not a multiple of 16 at the
 * end of the data. Otherwise the input must be buffered into 16 byte blocks.
 * */
static void poly1305_update(poly1305_state *state,
                            const unsigned char *in, size_t len)
{
    uint32_t t0,t1,t2,t3;
    uint64_t t[5];
    uint32_t b;
    uint64_t c;
    size_t j;
    unsigned char mp[16];

    if (len < 16)
        goto poly1305_donna_atmost15bytes;

poly1305_donna_16bytes:
    t0 = U8TO32_LE(in);
    t1 = U8TO32_LE(in+4);
    t2 = U8TO32_LE(in+8);
    t3 = U8TO32_LE(in+12);

    in += 16;
    len -= 16;

    state->h0 += t0 & 0x3ffffff;
    state->h1 += ((((uint64_t)t1 << 32) | t0) >> 26) & 0x3ffffff;
    state->h2 += ((((uint64_t)t2 << 32) | t1) >> 20) & 0x3ffffff;
    state->h3 += ((((uint64_t)t3 << 32) | t2) >> 14) & 0x3ffffff;
    state->h4 += (t3 >> 8) | (1 << 24);

poly1305_donna_mul:
    t[0] = mul32x32_64(state->h0,state->r0) +
           mul32x32_64(state->h1,state->s4) +
           mul32x32_64(state->h2,state->s3) +
           mul32x32_64(state->h3,state->s2) +
           mul32x32_64(state->h4,state->s1);
    t[1] = mul32x32_64(state->h0,state->r1) +
           mul32x32_64(state->h1,state->r0) +
           mul32x32_64(state->h2,state->s4) +
           mul32x32_64(state->h3,state->s3) +
           mul32x32_64(state->h4,state->s2);
    t[2] = mul32x32_64(state->h0,state->r2) +
           mul32x32_64(state->h1,state->r1) +
           mul32x32_64(state->h2,state->r0) +
           mul32x32_64(state->h3,state->s4) +
           mul32x32_64(state->h4,state->s3);
    t[3] = mul32x32_64(state->h0,state->r3) +
           mul32x32_64(state->h1,state->r2) +
           mul32x32_64(state->h2,state->r1) +
           mul32x32_64(state->h3,state->r0) +
           mul32x32_64(state->h4,state->s4);
    t[4] = mul32x32_64(state->h0,state->r4) +
           mul32x32_64(state->h1,state->r3) +
           mul32x32_64(state->h2,state->r2) +
           mul32x32_64(state->h3,state->r1) +
           mul32x32_64(state->h4,state->r0);

    state->h0 = (uint32_t)t[0] & 0x3ffffff;
    c =           (t[0] >> 26);
    t[1] += c;
    state->h1 = (uint32_t)t[1] & 0x3ffffff;
    b = (uint32_t)(t[1] >> 26);
    t[2] += b;
    state->h2 = (uint32_t)t[2] & 0x3ffffff;
    b = (uint32_t)(t[2] >> 26);
    t[3] += b;
    state->h3 = (uint32_t)t[3] & 0x3ffffff;
    b = (uint32_t)(t[3] >> 26);
    t[4] += b;
    state->h4 = (uint32_t)t[4] & 0x3ffffff;
    b = (uint32_t)(t[4] >> 26);
    state->h0 += b * 5;

    if (len >= 16)
        goto poly1305_donna_16bytes;

    /* final bytes */
poly1305_donna_atmost15bytes:
    if (!len)
        return;

    for (j = 0; j < len; j++)
        mp[j] = in[j];
    mp[j++] = 1;
    for (; j < 16; j++)
        mp[j] = 0;
    len = 0;

    t0 = U8TO32_LE(mp+0);
    t1 = U8TO32_LE(mp+4);
    t2 = U8TO32_LE(mp+8);
    t3 = U8TO32_LE(mp+12);

    state->h0 += t0 & 0x3ffffff;
    state->h1 += ((((uint64_t)t1 << 32) | t0) >> 26) & 0x3ffffff;
    state->h2 += ((((uint64_t)t2 << 32) | t1) >> 20) & 0x3ffffff;
    state->h3 += ((((uint64_t)t3 << 32) | t2) >> 14) & 0x3ffffff;
    state->h4 += (t3 >> 8);

    goto poly1305_donna_mul;
}

void CRYPTO_sim_poly1305_init(poly1305_state *state, const unsigned char key[32])
{
    uint32_t t0,t1,t2,t3;

    t0 = U8TO32_LE(key+0);
    t1 = U8TO32_LE(key+4);
    t2 = U8TO32_LE(key+8);
    t3 = U8TO32_LE(key+12);

    /* precompute multipliers */
    state->r0 = t0 & 0x3ffffff;
    t0 >>= 26;
    t0 |= t1 << 6;
    state->r1 = t0 & 0x3ffff03;
    t1 >>= 20;
    t1 |= t2 << 12;
    state->r2 = t1 & 0x3ffc0ff;
    t2 >>= 14;
    t2 |= t3 << 18;
    state->r3 = t2 & 0x3f03fff;
    t3 >>= 8;
    state->r4 = t3 & 0x00fffff;

    state->s1 = state->r1 * 5;
    state->s2 = state->r2 * 5;
    state->s3 = state->r3 * 5;
    state->s4 = state->r4 * 5;

    /* init state */
    state->h0 = 0;
    state->h1 = 0;
    state->h2 = 0;
    state->h3 = 0;
    state->h4 = 0;

    state->buf_used = 0;
    memcpy(state->key, key + 16, sizeof(state->key));
}

void CRYPTO_sim_poly1305_update(poly1305_state *state, const unsigned char *in,
                                size_t in_len)
{
    unsigned int i;

    if (state->buf_used) {
        unsigned int todo = 16 - state->buf_used;
        if (todo > in_len)
            todo = in_len;
        for (i = 0; i < todo; i++)
            state->buf[state->buf_used + i] = in[i];
        state->buf_used += todo;
        in_len -= todo;
        in += todo;

        if (state->buf_used == 16) {
            poly1305_update(state, state->buf, 16);
            state->buf_used = 0;
        }
    }

    if (in_len >= 16) {
        size_t todo = in_len & ~0xf;
        poly1305_update(state, in, todo);
        in += todo;
        in_len &= 0xf;
    }

    if (in_len)	{
        for (i = 0; i < in_len; i++)
            state->buf[i] = in[i];
        state->buf_used = in_len;
    }
}

void CRYPTO_sim_poly1305_finish(poly1305_state *state, unsigned char mac[16])
{
    uint64_t f0,f1,f2,f3;
    uint32_t g0,g1,g2,g3,g4;
    uint32_t b, nb;

    if (state->buf_used)
        poly1305_update(state, state->buf, state->buf_used);

    b = state->h0 >> 26;
    state->h0 = state->h0 & 0x3ffffff;
    state->h1 +=     b;
    b = state->h1 >> 26;
    state->h1 = state->h1 & 0x3ffffff;
    state->h2 +=     b;
    b = state->h2 >> 26;
    state->h2 = state->h2 & 0x3ffffff;
    state->h3 +=     b;
    b = state->h3 >> 26;
    state->h3 = state->h3 & 0x3ffffff;
    state->h4 +=     b;
    b = state->h4 >> 26;
    state->h4 = state->h4 & 0x3ffffff;
    state->h0 += b * 5;

    g0 = state->h0 + 5;
    b = g0 >> 26;
    g0 &= 0x3ffffff;
    g1 = state->h1 + b;
    b = g1 >> 26;
    g1 &= 0x3ffffff;
    g2 = state->h2 + b;
    b = g2 >> 26;
    g2 &= 0x3ffffff;
    g3 = state->h3 + b;
    b = g3 >> 26;
    g3 &= 0x3ffffff;
    g4 = state->h4 + b - (1 << 26);

    b = (g4 >> 31) - 1;
    nb = ~b;
    state->h0 = (state->h0 & nb) | (g0 & b);
    state->h1 = (state->h1 & nb) | (g1 & b);
    state->h2 = (state->h2 & nb) | (g2 & b);
    state->h3 = (state->h3 & nb) | (g3 & b);
    state->h4 = (state->h4 & nb) | (g4 & b);

    f0 = ((state->h0      ) | (state->h1 << 26)) + (uint64_t)U8TO32_LE(&state->key[0]);
    f1 = ((state->h1 >>  6) | (state->h2 << 20)) + (uint64_t)U8TO32_LE(&state->key[4]);
    f2 = ((state->h2 >> 12) | (state->h3 << 14)) + (uint64_t)U8TO32_LE(&state->key[8]);
    f3 = ((state->h3 >> 18) | (state->h4 <<  8)) + (uint64_t)U8TO32_LE(&state->key[12]);

    U32TO8_LE(&mac[ 0], f0);
    f1 += (f0 >> 32);
    U32TO8_LE(&mac[ 4], f1);
    f2 += (f1 >> 32);
    U32TO8_LE(&mac[ 8], f2);
    f3 += (f2 >> 32);
    U32TO8_LE(&mac[12], f3);
}

//

/* chacha20-poly1305 for TLS uses the ORIGINAL chacha20 encrypt which has 64-bits nonce
 * chacha20-poly1305 for IETF uses the chacha20 encrypt which has 96-bits nonce
 * The 64-bits nonce chacha20 is implemented
 * To work for IETF, the attached chacha20 can only support the last 64 bits of 96-bits nonce
 */

static void poly1305_update_length(poly1305_state *poly1305, size_t len)
{
    size_t j = len;
    unsigned char length_bytes[8];
    unsigned i;

    for (i = 0; i < sizeof(length_bytes); i++) {
        length_bytes[i] = j;
        j >>= 8;
    }

    CRYPTO_sim_poly1305_update(poly1305, length_bytes, sizeof(length_bytes));
}

int aead_chacha20_poly1305_enc(unsigned char *out, size_t max_out_len,
                               const unsigned char *in, size_t in_len,
                               const unsigned char *ad, size_t ad_len,
                               const unsigned char *nonce, const unsigned char key[CHACHA20POLY1305_KEY_LEN])
{
    unsigned char poly1305_key[CHACHA20POLY1305_KEY_LEN];
    poly1305_state poly1305;
    const uint64_t in_len_64 = in_len;
    unsigned char padding[16];

    if (max_out_len < in_len + POLY1305_TAG_LEN) {
        // BUFFER_TOO_SMALL
        return -1;
    }

    memset(poly1305_key, 0, sizeof(poly1305_key));
    CRYPTO_sim_chacha_20(poly1305_key, poly1305_key, sizeof(poly1305_key), key, nonce, 0);
    CRYPTO_sim_chacha_20(out, in, in_len, key, nonce, 1);

    CRYPTO_sim_poly1305_init(&poly1305, poly1305_key);
    memset(padding, 0, 16);
    CRYPTO_sim_poly1305_update(&poly1305, ad, ad_len);
    CRYPTO_sim_poly1305_update(&poly1305, padding, (ad_len % 16) ? 16 - (ad_len % 16) : 0);
    CRYPTO_sim_poly1305_update(&poly1305, out, in_len);
    CRYPTO_sim_poly1305_update(&poly1305, padding, (in_len % 16) ? 16 - (in_len % 16) : 0);
    poly1305_update_length(&poly1305, ad_len);
    poly1305_update_length(&poly1305, in_len);
    CRYPTO_sim_poly1305_finish(&poly1305, out + in_len);

    return in_len + POLY1305_TAG_LEN;
}

int aead_chacha20_poly1305_dec(unsigned char *out, size_t max_out_len,
                               const unsigned char *in, size_t in_len,
                               const unsigned char *ad, size_t ad_len,
                               const unsigned char *nonce, const unsigned char key[CHACHA20POLY1305_KEY_LEN])
{
    unsigned char mac[POLY1305_TAG_LEN];
    unsigned char poly1305_key[CHACHA20POLY1305_KEY_LEN];
    size_t out_len;
    poly1305_state poly1305;
    const uint64_t in_len_64 = in_len;
    unsigned char padding[16];
    int i;

    if (in_len < POLY1305_TAG_LEN) {
        DiagPrintf("%s : in_len=%d < %d\r\n",
                   __FUNCTION__, in_len, POLY1305_TAG_LEN);
        return -1;
    }

    out_len = in_len - POLY1305_TAG_LEN;

    if (max_out_len < out_len) {
        // BUFFER_TOO_SMALL
        DiagPrintf("%s : buffer too small : max_out_len=%d, out_len=%d \r\n",
                   __FUNCTION__, max_out_len, out_len);
        return -1;
    }

    memset(poly1305_key, 0, sizeof(poly1305_key));
    CRYPTO_sim_chacha_20(poly1305_key, poly1305_key, sizeof(poly1305_key), key, nonce, 0);

    CRYPTO_sim_poly1305_init(&poly1305, poly1305_key);
    memset(padding, 0, 16);
    CRYPTO_sim_poly1305_update(&poly1305, ad, ad_len);
    CRYPTO_sim_poly1305_update(&poly1305, padding, (ad_len % 16) ? 16 - (ad_len % 16) : 0);
    CRYPTO_sim_poly1305_update(&poly1305, in, out_len);
    CRYPTO_sim_poly1305_update(&poly1305, padding, (out_len % 16) ? 16 - (out_len % 16) : 0);
    poly1305_update_length(&poly1305, ad_len);
    poly1305_update_length(&poly1305, out_len);
    CRYPTO_sim_poly1305_finish(&poly1305, mac);

    if ( rtl_memcmpb(mac, (u8*)(in + out_len), POLY1305_TAG_LEN) != 0) {
        uint8_t *ptr = in+out_len;

        // BAD_DECRYPT
        DiagPrintf(" %s : bad decrypt -1 \r\n", __FUNCTION__);

        DiagPrintf(" mac : \r\n");
        for (i=0; i<POLY1305_TAG_LEN; i++)
            DiagPrintf(" 0x%x", mac[i]);
        DiagPrintf("\r\n");

        DiagPrintf(" text : \r\n");
        for (i=0; i<POLY1305_TAG_LEN; i++)
            DiagPrintf(" 0x%x", ptr[i]);
        DiagPrintf("\r\n");

        memset(out, 0, out_len);
        return -1;
    }

    CRYPTO_sim_chacha_20(out, in, out_len, key, nonce, 1);
    return out_len;
}


//

enum noncetype {
    STRING,
    NUMBER
};

enum operation {
    ENCRYPT,
    DECRYPT
};

struct test_data {
    enum operation   op;
    enum noncetype   nonce_type;
    char*            str_nonce;
    uint32_t         num_nonce;
    uint8_t*         aad;
    size_t           aad_len;
    uint8_t*         key;
    char*            in_data;
    size_t           in_datalen;
    char*            out_data;
    size_t           out_datalen;
};

int test_num = 4;

struct test_data test[4] = {
    {
        ENCRYPT,
        STRING,
        "PV-Msg02",
        0,
        NULL,
        0,
        "88e79a5e206254359db6e5b747bb1532b1e90596d836068e391ef1c0135d23b0",
        "011130303a45303a34433a38373a31333a31330a403403c8c8d793ed16431b373c1694363a9b32d0080c117a4db33f8ee5a93ed7d5510981129544bb8604f026c9c27c9fa9020c91776c6239f3f4d732220331610f",
        85,
        "09b665601b79525ac639613557cc6b5028eaa577768e3a7652069b93a4b796ce014cd953a653cca8ee417e4879b54d9ccf4f3ea7efdbf6769472d4190e6046ab87a8690104101a08466fff9443d7d27d4b8e101f0497781794aa9fd6feec852d9873b45f51",
        101
    },
    {
        DECRYPT,
        STRING,
        "PV-Msg03",
        0,
        NULL,
        0,
        "88e79a5e206254359db6e5b747bb1532b1e90596d836068e391ef1c0135d23b0",
        "39966ec86347a586eeca47ec91ce016881613811f3bc13de536d26589f4fd7dad676187e5e42f5750d84311fbf40b6d6a687d8767997adf0c00d2a165eda34898d8b4404a30a94ad9938d12de22c6b98a241d7ffbe488f7a08cbd91686330698a65464d124ae5d20f09056bb8b951aabacc9c78883ebac17",
        120,
        "012439304545323645422d344538352d343037382d413445372d3631373530304246373344410a40dd1aa0d3b9e2fc507209715e70a7a5502bbf42d2bc7241ebcb77613afdb7a5148646b0e36e1ee7a082efa3242d1e42bddf5366d62524e1d4d4bf64c3ee4e9f05",
        104
    },
    {
        DECRYPT,
        NUMBER,
        NULL,
        2,
        "8a00",
        2,
        "6e53b771971ade0c12631ebb7a6eaa37a15ec6b7a4aba99f856d5a1367c19f7c",
        "11311fae1ccc43414b990d86baeca7ccad6c9dc5518878bbada44a925ef88d277bf2c7f0a79e7341fb1c12146919f8c5b8927b0538f6b275d474d97ad086a2c91c247a432d5949c87c315607a138b5355d3268d6d1ceb640986b037fd8eb769b0e3238b529e6f5d12b76c5cd09e6b36ee8f142d31fa55c77dd47895d64b7c412cf89f7179211702445a440254ba346bd878837fc28e10817a694",
        154,
        "505554202f63686172616374657269737469637320485454502f312e310d0a436f6e74656e742d4c656e6774683a2034390d0a436f6e74656e742d547970653a206170706c69636174696f6e2f6861702b6a736f6e0d0a0d0a7b22636861726163746572697374696373223a5b7b22616964223a312c22696964223a392c2276616c7565223a307d5d7d",
        138
    },
    {
        ENCRYPT,
        NUMBER,
        NULL,
        3,
        "9200",
        2,
        "e4e44959a7c8664c95e304cb70ec91da05179a384a545ea548bd2d6524cfc3c8",
        "485454502f312e3120323030204f4b0d0a436f6e74656e742d547970653a206170706c69636174696f6e2f6861702b6a736f6e0d0a436f6e74656e742d4c656e6774683a2037310d0a0d0a7b0a0922636861726163746572697374696373223a095b7b0a09090922616964223a09312c0a09090922696964223a09392c0a0909092276616c7565223a09300a09097d5d0a7d",
        146,
        "b0b21fe4c6b6735fcdef38e5d60f0694ce880085e227befc9365701cc8d7abedd93054eca95b5abe003515c46d44d77a5b75b049bdbfb458671a2043ab4b7375f312b6f83b571d020c40d29eae5a2b5fc1bd7cf1367971acab2964f670be1e20b2b9cdf37e279637b00a72514522d61846bc2f9caaa418af1fefa22d5b3a2b0820d46676932b6cf90ac5c41332ca38caf61f271eee706fabb180b5831af184989fc2",
        162
    }
};

static unsigned char hex_digit(char h)
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'a' && h <= 'f')
        return h - 'a' + 10;
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 10;
}

static void hex_decode(uint8_t *out, const char* hex)
{
    size_t j = 0;

    while (*hex != 0) {
        unsigned char v = hex_digit(*hex++);
        v <<= 4;
        v |= hex_digit(*hex++);
        out[j++] = v;
    }
}

uint8_t data1[1024];
uint8_t indata1[1024];
uint8_t outdata1[1024];

int chacha20poly1305_homekit_test(void)
{
    int i;
    int ret = -1;

    for(i = 0; i < test_num; i ++) {
        uint8_t aad[2], nonce[12], key[32];
        uint8_t *data, *indata, *outdata;
        int datalen;

        DiagPrintf("test %d: ", i);

        if(test[i].nonce_type == STRING) {
            memset(nonce, 0, sizeof(nonce));
            memcpy(nonce + 4, test[i].str_nonce, 8);
        }
        if(test[i].nonce_type == NUMBER) {
            nonce[ 0] = 0;
            nonce[ 1] = 0;
            nonce[ 2] = 0;
            nonce[ 3] = 0;
            nonce[ 4] = (uint8_t) ( test[i].num_nonce        & 0xFF);
            nonce[ 5] = (uint8_t) ((test[i].num_nonce >>  8) & 0xFF);
            nonce[ 6] = (uint8_t) ((test[i].num_nonce >> 16) & 0xFF);
            nonce[ 7] = (uint8_t) ((test[i].num_nonce >> 24) & 0xFF);
            nonce[ 8] = (uint8_t) ((test[i].num_nonce >> 32) & 0xFF);
            nonce[ 9] = (uint8_t) ((test[i].num_nonce >> 40) & 0xFF);
            nonce[10] = (uint8_t) ((test[i].num_nonce >> 48) & 0xFF);
            nonce[11] = (uint8_t) ((test[i].num_nonce >> 56) & 0xFF);
        }

        if(test[i].aad_len)
            hex_decode(aad, test[i].aad);

        if(strlen(test[i].key)/2 == 32) {
            hex_decode(key, test[i].key);
        } else {
            ret = -1;
            DiagPrintf("key err\n");
            break;
        }

        if(strlen(test[i].in_data)/2 == test[i].in_datalen) {
            //indata = malloc(test[i].in_datalen);
            indata = &indata1[0];
            hex_decode(indata, test[i].in_data);
        } else {
            ret = -2;
            DiagPrintf("indata err\n");
            break;
        }

        if(strlen(test[i].out_data)/2 == test[i].out_datalen) {
            //data = malloc(test[i].out_datalen);
            //outdata = malloc(test[i].out_datalen);
            data = &data1[0];
            outdata = &outdata1[0];
            hex_decode(outdata, test[i].out_data);
        } else {
            ret = -3;
            DiagPrintf("outdata err\n");
            break;
        }

        if(test[i].op == ENCRYPT) {
            datalen = aead_chacha20_poly1305_enc(data, test[i].out_datalen, indata, test[i].in_datalen,
                                                 test[i].aad_len ? aad : NULL, test[i].aad_len, nonce, key);
        }
        if(test[i].op == DECRYPT) {
            datalen = aead_chacha20_poly1305_dec(data, test[i].out_datalen, indata, test[i].in_datalen,
                                                 test[i].aad_len ? aad : NULL, test[i].aad_len, nonce, key);
        }

        if((datalen == test[i].out_datalen) && (rtl_memcmpb(data, outdata, datalen) == 0)) {
            ret = 0;
            DiagPrintf("pass, datalen=%d\n", datalen);
        } else {
            ret = -4;
            DiagPrintf("fail, datalen=%d, should be %d\n", datalen, test[i].out_datalen);

        }

        //free(indata);
        //free(outdata);
        //free(data);
    }

    return ret;
}


//
// CRC32
//

static uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t
crc32(uint32_t crc, const void *buf, size_t size)
{
    const uint8_t *p;

    p = buf;
    crc = crc ^ ~0U;

    while (size--)
        crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

    return crc ^ ~0U;
}





