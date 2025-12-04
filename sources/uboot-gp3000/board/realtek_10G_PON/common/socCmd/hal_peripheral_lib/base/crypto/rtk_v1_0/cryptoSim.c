/*
* --------------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2003  
* All rights reserved.
* 
* Program : Simulate DES behavior and providing final DES driver interface
* Abstract : 
* $Id: aesSim.c,v 1.2 2006-07-13 16:03:06 chenyl Exp $
*/
#include "platform_autoconf.h"
#include "diag.h"
#include "hal_crypto.h"
#include <common.h>
#include "cryptoSim.h"

#include "cryptoSim_md5.h"
#include "cryptoSim_sha.h"
#include "section_config.h"

//#define NEW_CRYPTO 1

uint8 tempDigest[CRYPTO_SHA2_DIGEST_LENGTH];


int authSim(const u32 mode, const u8 * data, const u32 dataLen, const u8 * key, const u32 keyLen, 
	u8 * digest, const u32 digestlen) 
{
    uint32 i, dmaLen;
	poly1305_state poly1305;

    
    dmaLen = dataLen;

    switch(mode) {
        case SWHASH_MD5:
            md5(data, dmaLen, tempDigest);
            break;
            
        case SWHASH_SHA1:
            sha1(data, dmaLen, tempDigest);
            break; 

        case SWHASH_SHA2:
            sha2(data, dmaLen, tempDigest, digestlen);
            break; 
            
        case SWHMAC_MD5:
            md5_hmac(key, keyLen, data, dmaLen, tempDigest);
            break;
        
        case SWHMAC_SHA1:
            sha1_hmac(key, keyLen, data, dmaLen, tempDigest);
            break;
			
		case SWHMAC_SHA2:
			sha256_hmac(key, keyLen, data, dmaLen, tempDigest, (digestlen==224/8)?1:0);
			break;
		case SW_POLY1305:
#ifdef NEW_CRYPTO
			CRYPTO_sim_poly1305_init(&poly1305, key);
			CRYPTO_sim_poly1305_update(&poly1305, data, dmaLen);
			CRYPTO_sim_poly1305_finish(&poly1305, tempDigest);
#endif
			break;
			
    }

		for(i=0;i<digestlen;i++) {
			digest[i]=tempDigest[i];
		}		
    
    return SUCCESS;
}



int aesSim_aes(const u32 mode, const u8 *input, const u32 len, const u32 lenKey, const u8 *key, const u8 *iv , u8* output)
{
    AES_KEY aesKey;
    int done;
    u8 _iv[16]; /* Since soft_AES_cbc_encrypt() will modify iv, we need to allocate one for it. */
    u8 counter[AES_BLOCK_SIZE];
    u8 ecount_buf[AES_BLOCK_SIZE];
    uint32 num;


    switch ( mode )
    {
        case SWDECRYPT_CBC_AES:
            rtl_memcpyb( _iv, iv, sizeof(_iv) );
            AES_set_decrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            soft_AES_cbc_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, FALSE );
            break;
            
        case SWDECRYPT_ECB_AES:
            AES_set_decrypt_key( (const u8 *) key, lenKey*8, &aesKey );
			
			for( done = 0; done < len; done += 16 )
                AES_ecb_encrypt( (const u8 *) &input[done], (u8 *) &output[done], &aesKey, AES_DECRYPT );
            break;
            
        case SWDECRYPT_CTR_AES:
            AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            rtl_memcpyb( counter, iv, sizeof(counter) );
            rtl_memsetb( ecount_buf, 0, sizeof(ecount_buf) );
            num = 0;
            AES_ctr128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, counter, ecount_buf, &num );
            break;
			
		case SWDECRYPT_CFB_AES:
			AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
			rtl_memcpyb( _iv, iv, sizeof(_iv) );
			num = 0;
			AES_cfb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num, FALSE );
			break;

            
        case SWENCRYPT_CBC_AES:
            rtl_memcpyb( _iv, iv, sizeof(_iv) );
            AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
			
            soft_AES_cbc_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, TRUE );
            break;
            
        case SWENCRYPT_ECB_AES:
            AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            for( done = 0; done < len; done += 16 )
                AES_ecb_encrypt( (const u8 *) &input[done], (u8 *) &output[done], &aesKey, AES_ENCRYPT );
            break;
            
        case SWENCRYPT_CTR_AES:
            AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            rtl_memcpyb( counter, iv, sizeof(counter) );
            rtl_memsetb( ecount_buf, 0, sizeof(ecount_buf) );
            num = 0;
            AES_ctr128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, counter, ecount_buf, &num );
            break;

		case SWENCRYPT_CFB_AES:
			AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
            rtl_memcpyb( _iv, iv, sizeof(_iv) );
			num = 0;
			AES_cfb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num, TRUE );
			break;

		case SWENCRYPT_OFB_AES:
			AES_set_encrypt_key( (const u8 *) key, lenKey*8, &aesKey );
			rtl_memcpyb( _iv, iv, sizeof(_iv) );
			num = 0;
			AES_ofb128_encrypt( (const u8 *) input, (u8 *) output, len, &aesKey, _iv, &num);
			break;

    }
    return SUCCESS;
}

// DES



//data, key and iv does not have 4-byte alignment limitatiuon
int desSim_des(const u32 mode, const u8 *input, const u32 len, const u8 *key, const u8 *iv, u8 * output) {
    switch(mode) {
        case  SWDECRYPT_CBC_DES:
            return desSim_cbc_encrypt(input, output, len, key, iv, FALSE);
        case  SWDECRYPT_CBC_3DES:
            return desSim_ede_cbc_encrypt(input, output, len, key, iv, FALSE);
        case  SWDECRYPT_ECB_DES:
            return desSim_ecb_encrypt(input, output, len, key, FALSE);
        case  SWDECRYPT_ECB_3DES:
            return desSim_ede_ecb_encrypt(input, output, len, key, FALSE);
        case  SWENCRYPT_CBC_DES:
            return desSim_cbc_encrypt(input, output, len, key, iv, TRUE);
        case  SWENCRYPT_CBC_3DES:
            return desSim_ede_cbc_encrypt(input, output, len, key, iv, TRUE);
        case  SWENCRYPT_ECB_DES:
            return desSim_ecb_encrypt(input, output, len, key, TRUE);
        case  SWENCRYPT_ECB_3DES:
            return desSim_ede_ecb_encrypt(input, output, len, key, TRUE);
    }
    return FAILED;
}

#ifdef NEW_CRYPTO
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

	_memcpy(x, input, sizeof(uint32_t) * 16);
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
		      const unsigned char nonce[8],
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
	input[13] = ((uint64_t) counter) >> 32;
	input[14] = U8TO32_LITTLE(nonce + 0);
	input[15] = U8TO32_LITTLE(nonce + 4);

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

	           state->h0 = (uint32_t)t[0] & 0x3ffffff; c =           (t[0] >> 26);
	t[1] += c; state->h1 = (uint32_t)t[1] & 0x3ffffff; b = (uint32_t)(t[1] >> 26);
	t[2] += b; state->h2 = (uint32_t)t[2] & 0x3ffffff; b = (uint32_t)(t[2] >> 26);
	t[3] += b; state->h3 = (uint32_t)t[3] & 0x3ffffff; b = (uint32_t)(t[3] >> 26);
	t[4] += b; state->h4 = (uint32_t)t[4] & 0x3ffffff; b = (uint32_t)(t[4] >> 26);
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
	state->r0 = t0 & 0x3ffffff; t0 >>= 26; t0 |= t1 << 6;
	state->r1 = t0 & 0x3ffff03; t1 >>= 20; t1 |= t2 << 12;
	state->r2 = t1 & 0x3ffc0ff; t2 >>= 14; t2 |= t3 << 18;
	state->r3 = t2 & 0x3f03fff; t3 >>= 8;
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
	_memcpy(state->key, key + 16, sizeof(state->key));
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

	                    b = state->h0 >> 26; state->h0 = state->h0 & 0x3ffffff;
	state->h1 +=     b; b = state->h1 >> 26; state->h1 = state->h1 & 0x3ffffff;
	state->h2 +=     b; b = state->h2 >> 26; state->h2 = state->h2 & 0x3ffffff;
	state->h3 +=     b; b = state->h3 >> 26; state->h3 = state->h3 & 0x3ffffff;
	state->h4 +=     b; b = state->h4 >> 26; state->h4 = state->h4 & 0x3ffffff;
	state->h0 += b * 5;

	g0 = state->h0 + 5; b = g0 >> 26; g0 &= 0x3ffffff;
	g1 = state->h1 + b; b = g1 >> 26; g1 &= 0x3ffffff;
	g2 = state->h2 + b; b = g2 >> 26; g2 &= 0x3ffffff;
	g3 = state->h3 + b; b = g3 >> 26; g3 &= 0x3ffffff;
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

	U32TO8_LE(&mac[ 0], f0); f1 += (f0 >> 32);
	U32TO8_LE(&mac[ 4], f1); f2 += (f1 >> 32);
	U32TO8_LE(&mac[ 8], f2); f3 += (f2 >> 32);
	U32TO8_LE(&mac[12], f3);
}
#endif

