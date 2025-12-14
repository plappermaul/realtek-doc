/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _CRYPTOSIM_H_
#define _CRYPTOSIM_H_


#include "hal_crypto.h"
#include "hal_crypto_ram.h"




// DES

#define SWDECRYPT_ECB_DES		0x00
#define SWDECRYPT_CBC_DES		0x01
#define SWDECRYPT_ECB_3DES		0x10
#define SWDECRYPT_CBC_3DES		0x11

#define SWENCRYPT_ECB_DES		0x80
#define SWENCRYPT_CBC_DES		0x81
#define SWENCRYPT_ECB_3DES		0x90
#define SWENCRYPT_CBC_3DES		0x91


extern int desSim_des(const u32 mode, const u8 *input, const u32 len, const u8 *key, const u8 *iv , u8* output);



// AES 

int aesSim_aes(const u32 mode, const u8 *input, const u32 len, const u32 lenKey,  
                   const u8 *key, const u8 *iv , const u32 lenAAD, const u8 *aad, u8* output, u8* tag);


#define AES_ENCRYPT	1
#define AES_DECRYPT	0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
//#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16




#define SWDECRYPT_ECB_AES 0x20
#define SWDECRYPT_CBC_AES 0x21
#define SWDECRYPT_CFB_AES 0x22
#define SWDECRYPT_OFB_AES 0x23
#define SWDECRYPT_CTR_AES 0x24
#define SWDECRYPT_GCM_AES 0x28

#define SWENCRYPT_ECB_AES 0xa0
#define SWENCRYPT_CBC_AES 0xa1
#define SWENCRYPT_CFB_AES 0xa2
#define SWENCRYPT_OFB_AES 0xa3
#define SWENCRYPT_CTR_AES 0xa4
#define SWENCRYPT_GCM_AES 0xa8


#define GETU32(pt) (((uint32)(pt)[0] << 24) ^ ((uint32)(pt)[1] << 16) ^ ((uint32)(pt)[2] <<  8) ^ ((uint32)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (uint8)((st) >> 24); (ct)[1] = (uint8)((st) >> 16); (ct)[2] = (uint8)((st) >>  8); (ct)[3] = (uint8)(st); }


#define MAXKC   (256/32)
#define MAXKB   (256/8)
#define MAXNR   14


typedef RTL_SW_AES_KEY AES_KEY; 


extern int32 AES_set_encrypt_key(const uint8 *userKey, const int32 bits,
	AES_KEY *key);

extern int32 AES_set_decrypt_key(const uint8 *userKey, const int32 bits,
	AES_KEY *key);

extern void AES_encrypt(const uint8 *in, uint8 *out,
	const AES_KEY *key);

extern void AES_decrypt(const uint8 *in, uint8 *out,
	const AES_KEY *key);

extern void AES_ecb_encrypt(const uint8 *in, uint8 *out,
	const AES_KEY *key, const int32 enc);

extern void soft_AES_cbc_encrypt(const uint8 *in, uint8 *out,
	const uint32 length, const AES_KEY *key,
	uint8 *ivec, const int32 enc);

extern void AES_cfb128_encrypt(const uint8 *in, uint8 *out,
	const uint32 length, const AES_KEY *key,
	uint8 *ivec, int32 *num, const int32 enc);

extern void AES_ofb128_encrypt(const uint8 *in, uint8 *out,
	const uint32 length, const AES_KEY *key,
	uint8 *ivec, int32 *num);

extern void AES_ctr128_encrypt(const uint8 *in, uint8 *out,
	const uint32 length, const AES_KEY *key,
	uint8 counter[AES_BLOCK_SIZE],
	uint8 ecount_buf[AES_BLOCK_SIZE],
	uint32 *num);

extern void AES_key_gen(AES_KEY *key, uint8 *in, int32 keylen, int32 round);

 
//
// chacha 
//

extern void CRYPTO_sim_chacha_20(unsigned char *out,
		      const unsigned char *in, size_t in_len,
		      const unsigned char key[32],
		      const unsigned char nonce[8],
		      size_t counter);

//
// poly1305
//
struct poly1305_state_st
{
	uint32_t r0,r1,r2,r3,r4;
	uint32_t s1,s2,s3,s4;
	uint32_t h0,h1,h2,h3,h4;
	unsigned char buf[16];
	unsigned int buf_used;
	unsigned char key[16];
};

typedef struct poly1305_state_st poly1305_state;


void CRYPTO_sim_poly1305_init(poly1305_state *state, const unsigned char key[32]);
void CRYPTO_sim_poly1305_update(poly1305_state *state, const unsigned char *in,
		     size_t in_len);
void CRYPTO_sim_poly1305_finish(poly1305_state *state, unsigned char mac[16]);

//
#define CHACHA20_NONCE_LEN         8
#define POLY1305_TAG_LEN           16
#define CHACHA20POLY1305_KEY_LEN   32

int aead_chacha20_poly1305_enc(unsigned char *out, size_t max_out_len,
                               const unsigned char *in, size_t in_len,
                               const unsigned char *ad, size_t ad_len,
                               const unsigned char *nonce, const unsigned char key[CHACHA20POLY1305_KEY_LEN]);
int aead_chacha20_poly1305_dec(unsigned char *out, size_t max_out_len,
                               const unsigned char *in, size_t in_len,
                               const unsigned char *ad, size_t ad_len,
                               const unsigned char *nonce, const unsigned char key[CHACHA20POLY1305_KEY_LEN]);

int chacha20poly1305_homekit_test(void);


// CRC32

uint32_t
crc32(uint32_t crc, const void *buf, size_t size);

int crc_tester_setting(int order1, unsigned long polynom1, int direct1, unsigned long crcinit1, unsigned long crcxor1, int refin1, int refout1);

unsigned long crc_tester(const unsigned char* p, unsigned long len);


#endif
