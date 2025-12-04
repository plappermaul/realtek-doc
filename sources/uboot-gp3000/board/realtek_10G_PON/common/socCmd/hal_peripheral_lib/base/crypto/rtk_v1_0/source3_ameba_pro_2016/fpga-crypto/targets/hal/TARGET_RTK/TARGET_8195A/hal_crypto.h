/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef __HAL_CRYPTO_H__
#define __HAL_CRYPTO_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_api.h"
#include "basic_types.h"
#include "rtl8195a_crypto_util_rom.h"


#define CRYPTO_MAX_MSG_LENGTH		16383 // 64 // 16383
#define CRYPTO_MAX_DIGEST_LENGTH	32


// Mode Select
#define CRYPTO_MS_TYPE_CIPHER	(0)
#define CRYPTO_MS_TYPE_AUTH     (1)
#define CRYPTO_MS_TYPE_MIX1_sshenc_espdec  	(2)
#define CRYPTO_MS_TYPE_MIX2_sshdec_espenc  	(3)
#define CRYPTO_MS_TYPE_MIX3_sslenc  	(4)


// AUTH
#define SHA2_224	(224/8)
#define SHA2_256	(256/8)

//
#define _ERRNO_CRYPTO_DESC_NUM_SET_OutRange 		-2
#define _ERRNO_CRYPTO_BURST_NUM_SET_OutRange		-3
#define _ERRNO_CRYPTO_NULL_POINTER					-4
#define _ERRNO_CRYPTO_ENGINE_NOT_INIT				-5
#define _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned		-6
#define _ERRNO_CRYPTO_KEY_OutRange					-7
#define _ERRNO_CRYPTO_MSG_OutRange					-8
#define _ERRNO_CRYPTO_IV_OutRange					-9
#define _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH			-10
#define _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH 		-11
#define _ERRNO_CRYPTO_KEY_IV_LEN_DIFF				-12
#define _ERRNO_CRYPTO_AES_MSGLEN_NOT_16Byte_Aligned	-13


//
#define CIPHER_TYPE_DES_ECB			0x0
#define CIPHER_TYPE_DES_CBC 		0x1
#define CIPHER_TYPE_3DES_ECB		0x10
#define CIPHER_TYPE_3DES_CBC		0x11
#define CIPHER_TYPE_AES_ECB			0x20
#define CIPHER_TYPE_AES_CBC			0x21
#define CIPHER_TYPE_AES_CFB			0x22
#define CIPHER_TYPE_AES_OFB			0x23
#define CIPHER_TYPE_AES_CTR			0x24  // 0x29
#define CIPHER_TYPE_AES_GCTR		0x25
#define CIPHER_TYPE_AES_GMAC		0x26
#define CIPHER_TYPE_AES_GHASH		0x27
#define CIPHER_TYPE_AES_GCM			0x28

#define CIPHER_TYPE_CHACHA_POLY1305	0x30 // chacha+poly1305
#define CIPHER_TYPE_CHACHA			0x31
#define CIPHER_TYPE_POLY1305		0x33 // poly1305 - mode 2


typedef enum {
    MBEDTLS_MD_NONE=0,
    MBEDTLS_MD_MD2,
    MBEDTLS_MD_MD4,
    MBEDTLS_MD_MD5,
    MBEDTLS_MD_SHA1,
    MBEDTLS_MD_SHA224,
    MBEDTLS_MD_SHA256,
    MBEDTLS_MD_SHA384,
    MBEDTLS_MD_SHA512,
    MBEDTLS_MD_RIPEMD160,
} mbedtls_md_type_t;

//
// structures
// 
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
mbedtls_sha1_context;

/**
 * \brief          SHA-256 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
}
mbedtls_sha256_context;


/**
 * Message digest information.
 * Allows message digest functions to be called in a generic way.
 */
struct mbedtls_md_info_t
{
    /** Digest identifier */
    mbedtls_md_type_t type;

    /** Name of the message digest */
    const char * name;

    /** Output length of the digest function in bytes */
    int size;

    /** Block length of the digest function in bytes */
    int block_size;

    /** Digest initialisation function */
    void (*starts_func)( void *ctx );

    /** Digest update function */
    void (*update_func)( void *ctx, const unsigned char *input, size_t ilen );

    /** Digest finalisation function */
    void (*finish_func)( void *ctx, unsigned char *output );

    /** Generic digest function */
    void (*digest_func)( const unsigned char *input, size_t ilen,
                         unsigned char *output );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

    /** Clone state from a context */
    void (*clone_func)( void *dst, const void *src );

    /** Internal use only */
    void (*process_func)( void *ctx, const unsigned char *input );
};

typedef struct mbedtls_md_info_t mbedtls_md_info_t;

void mbedtls_init(void);



/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 */
void mbedtls_md5( const unsigned char *input, size_t ilen, unsigned char output[16] );
void mbedtls_md5_sw( const unsigned char *input, size_t ilen, unsigned char output[16] );
void mbedtls_md5_hw( const unsigned char *input, size_t ilen, unsigned char output[16] );

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
 
void mbedtls_sha1_init( mbedtls_sha1_context *ctx );
void mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );
void mbedtls_sha1_sw( const unsigned char *input, size_t ilen, unsigned char output[20] );
void mbedtls_sha1_hw( const unsigned char *input, size_t ilen, unsigned char output[20] );

void mbedtls_sha1_starts( mbedtls_sha1_context *ctx );
void mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen );
void mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] );

void mbedtls_sha1_free( mbedtls_sha1_context *ctx );


/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void mbedtls_sha256( const unsigned char *input, size_t ilen,
           unsigned char output[32], int is224 );

void mbedtls_sha256_sw( const unsigned char *input, size_t ilen,
           unsigned char output[32], int is224 );

void mbedtls_sha256_hw( const unsigned char *input, size_t ilen,
           unsigned char output[32], int is224 );

void mbedtls_sha256_init( mbedtls_sha256_context *ctx );

void mbedtls_sha256_free( mbedtls_sha256_context *ctx );

void mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 );

void mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen );

void mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char output[32] );



//


int mbedtls_md_hmac_md5(const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

int mbedtls_md_hmac_sha1(const unsigned char *key, size_t keylen,
			   const unsigned char *input, size_t ilen,
			   unsigned char *output );

int mbedtls_md_hmac_sha224(const unsigned char *key, size_t keylen,
			   const unsigned char *input, size_t ilen,
			   unsigned char *output );

int mbedtls_md_hmac_sha256(const unsigned char *key, size_t keylen,
			   const unsigned char *input, size_t ilen,
			   unsigned char *output );


//
//	External API Functions
//


// Crypto Engine
int rtl_cryptoEngine_init(void);
void rtl_cryptoEngine_info(void);



//
// Authentication 
//

// md5

int rtl_crypto_md5(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_md5_init(void);
int rtl_crypto_md5_process(IN const u8* message, const IN u32 msglen, OUT u8* pDigest);

int rtl_crypto_md5_update(IN const u8* message, IN const u32 msglen);

int rtl_crypto_md5_final(OUT u8* pDigest);


// sha1
int rtl_crypto_sha1(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_sha1_init(void);
int rtl_crypto_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);


int rtl_crypto_sha1_update(IN const u8* message, IN const u32 msglen);

int rtl_crypto_sha1_final(OUT u8* pDigest);


// sha2

int rtl_crypto_sha2(IN const int sha2type, 
								IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_sha2_init(IN const int sha2type);
int rtl_crypto_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_sha2_update(IN const u8* message, IN const u32 msglen);

int rtl_crypto_sha2_final(OUT u8* pDigest);



// HMAC-md5
int rtl_crypto_hmac_md5(IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

int rtl_crypto_hmac_md5_init(IN const u8* key, IN const u32 keylen);
int rtl_crypto_hmac_md5_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_hmac_md5_update(IN const u8* message, IN const u32 msglen);
int rtl_crypto_hmac_md5_final(OUT u8* pDigest);


// HMAC-sha1
int rtl_crypto_hmac_sha1(IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

int rtl_crypto_hmac_sha1_init(IN const u8* key, IN const u32 keylen);
int rtl_crypto_hmac_sha1_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_hmac_sha1_update(IN const u8* message, IN const u32 msglen);

int rtl_crypto_hmac_sha1_final(OUT u8* pDigest);


// HMAC-sha2
int rtl_crypto_hmac_sha2(IN const int sha2type, IN const u8* message, IN const u32 msglen, 
									IN const u8* key, IN const u32 keylen, OUT u8* pDigest);

int rtl_crypto_hmac_sha2_init(IN const int sha2type, IN const u8* key, IN const u32 keylen);
int rtl_crypto_hmac_sha2_process(IN const u8* message, IN const u32 msglen, OUT u8* pDigest);

int rtl_crypto_hmac_sha2_update(IN const u8* message, IN const u32 msglen);

int rtl_crypto_hmac_sha2_final(OUT u8* pDigest);

//
// Cipher Functions
//

// AES - CBC

int rtl_crypto_aes_cbc_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_aes_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - ECB

int rtl_crypto_aes_ecb_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_aes_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - CTR

int rtl_crypto_aes_ctr_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_ctr_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_aes_ctr_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - CFB

int rtl_crypto_aes_cfb_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_cfb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_aes_cfb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - OFB

int rtl_crypto_aes_ofb_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_ofb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_aes_ofb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// AES - GCM

int rtl_crypto_aes_gcm_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_aes_gcm_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv,  
	IN const u8* aad, IN const u32 aadlen, 
	OUT u8* pResult, OUT u8* pTag);

int rtl_crypto_aes_gcm_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv,  
	IN const u8* aad, IN const u32 aadlen, 
	OUT u8* pResult, OUT u8* pTag);


// 3DES - CBC

int rtl_crypto_3des_cbc_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_3des_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_3des_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// 3DES - ECB

int rtl_crypto_3des_ecb_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_3des_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_3des_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// DES - CBC

int rtl_crypto_des_cbc_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_des_cbc_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_des_cbc_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);


// DES - ECB

int rtl_crypto_des_ecb_init(IN const u8* key, IN const u32 keylen);

int rtl_crypto_des_ecb_encrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

int rtl_crypto_des_ecb_decrypt(
	IN const u8* message, IN const u32 msglen, 
	IN const u8* iv, IN const u32 ivlen, OUT u8* pResult);

// 

int rtl_crypto_chacha_init(IN const u8* key);

int rtl_crypto_chacha_encrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* iv, 		IN const u32 count,
			OUT u8* pResult);

int rtl_crypto_chacha_decrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* iv, 		IN const u32 ivlen, 
			OUT u8* pResult);


int rtl_crypto_poly1305(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* key, 		IN const u32 keylen, 
			OUT u8* pDigest);

//

int rtl_crypto_chacha_poly1305_init(IN const u8* key);

int rtl_crypto_chacha_poly1305_encrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* nonce,  
			IN const u8* aad,	IN const u32 aadlen, 
			OUT u8* pResult, OUT u8* pDigest);

int rtl_crypto_chacha_poly1305_decrypt(
			IN const u8* message, 	IN const u32 msglen, 
			IN const u8* nonce,  
			IN const u8* aad,	IN const u32 aadlen, 
			OUT u8* pResult, OUT u8* pDigest);


// crc32

int rtl_crypto_crc32(
	IN const u8* message,	IN const u32 msglen, 
	OUT u32* pCrc);

int rtl_crypto_crc_setting(int order, unsigned long polynom, int direct, unsigned long crcinit, unsigned long crcxor, int refin, int refout);

int rtl_crypto_crc(
	IN const u8* message,	IN const u32 msglen, 
	OUT u32* pCrc);




#ifdef __cplusplus
}
#endif

#endif /* __HAL_CRYPTO_H__ */

