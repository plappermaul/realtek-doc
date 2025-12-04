#ifndef __RTK_CRYPTO_BASE_TEST_H__
#define __RTK_CRYPTO_BASE_TEST_H__

/*--------------------Define --------------------------------------------*/

#define _ENABLE  1
#define _DISABLE 0

#define IPSEC_VECTOR_TEST_ENABLE          _ENABLE

#define Crypto_CMD	"CRYPTO"
#define CRYPTO_BASE_ADDR 0x90500000
#define dbg_mem_dump_en 1
#define max_seq_update_num 10

#define byte_align cache_alinge

//#define threads_num     1
#define max_auth_msglen    256 // 65536
#define max_auth_keylen    64
#define max_auth_digestlen 64
#define max_cipher_msglen  256 // 65536
#define max_cipher_keylen  64
#define max_cipher_ivlen   16
#define max_cipher_aadlen  16
#define max_cipher_taglen  16

// throughput test config
#define thr_auth_msglen   256 // 65536
#define thr_auth_keylen   16
#define thr_auth_msglen_p 256 // 65536
#define thr_auth_seqnum   8

#define thr_cipher_des_msglen  256
#define thr_cipher_des_keylen  8
#define thr_cipher_3des_keylen 24
#define thr_cipher_des_ivlen   8

#define thr_cipher_aes_msglen    256
#define thr_cipher_aes128_keylen 16
#define thr_cipher_aes192_keylen 24
#define thr_cipher_aes256_keylen 32
#define thr_cipher_aes_ivlen     16
#define thr_cipher_aes_aadlen    14 // 112

#define thr_mix_msglen           256
#define thr_mix_cipher_keylen128 16
#define thr_mix_cipher_keylen192 24
#define thr_mix_cipher_keylen256 32
#define thr_mix_auth_keylen      16
#define thr_mix_ivlen            16
#define thr_mix_aadlen           14 // 112

/*--------------------Define Enum---------------------------------------*/

enum CRYPTO_TEST_MODE {
	TEST_MODE_DEFAULT         = 0x00,
	/*------------------------------------*/
	TEST_AUTH_HASH_NULL       = 0x01,
	TEST_AUTH_HASH_NORMAL     = 0x02,
	TEST_AUTH_HASH_SEQUENTIAL = 0x03,
	/*------------------------------------*/
	TEST_AUTH_HMAC_NORMAL     = 0x11,
	TEST_AUTH_HMAC_SEQUENTIAL = 0x12,
	/*------------------------------------*/
	TEST_CIPHER_DES           = 0x21,
	TEST_CIPHER_3DES          = 0x22,
	TEST_CIPHER_AES           = 0x31,
	/*------------------------------------*/
	TEST_MIX                  = 0x41,
	/*------------------------------------*/
	TEST_KEYSTORAGE           = 0x51,
	TEST_KEYPADSTORAGE        = 0x52
};

enum CRYPTO_TEST_ALGO {
	TEST_AUTH_MD5          = 0x01,
	TEST_AUTH_SHA1         = 0x02,
	TEST_AUTH_SHA2_224     = 0x03,
	TEST_AUTH_SHA2_256     = 0x04,
	/*------------------------------------*/
	TEST_AUTH_HMACMD5      = 0x11,
	TEST_AUTH_HMACSHA1     = 0x12,
	TEST_AUTH_HMACSHA2_224 = 0x13,
	TEST_AUTH_HMACSHA2_256 = 0x14,
	/*------------------------------------*/
	TEST_CIPHER_DES_ECB    = 0x21,
	TEST_CIPHER_DES_CBC    = 0x22,
	TEST_CIPHER_DES_CFB    = 0x23,
	TEST_CIPHER_DES_OFB    = 0x24,
	TEST_CIPHER_DES_CTR    = 0x25,
	TEST_CIPHER_3DES_ECB   = 0x26,
	TEST_CIPHER_3DES_CBC   = 0x27,
	TEST_CIPHER_3DES_CFB   = 0x28,
	TEST_CIPHER_3DES_OFB   = 0x29,
	TEST_CIPHER_3DES_CTR   = 0x2A,
	/*------------------------------------*/
	TEST_CIPHER_AES_ECB    = 0x31,
	TEST_CIPHER_AES_CBC    = 0x32,
	TEST_CIPHER_AES_CFB    = 0x33,
	TEST_CIPHER_AES_OFB    = 0x34,
	TEST_CIPHER_AES_CTR    = 0x35,
	TEST_CIPHER_AES_GCTR   = 0x36,
	TEST_CIPHER_AES_GMAC   = 0x37,
	TEST_CIPHER_AES_GHASH  = 0x38,
	TEST_CIPHER_AES_GCM    = 0x39,
	/*------------------------------------*/
	TEST_MIX_SSH           = 0x41,
	TEST_MIX_ESP           = 0x42,
	TEST_MIX_TLS           = 0x43,
	/*------------------------------------*/
	TEST_REG_RW            = 0x51,
	TEST_OTP_RW            = 0x52,
	TEST_REG_USAGE         = 0x53,
	TEST_OTP_USAGE         = 0x54,
	TEST_WRITE_BACK        = 0x55
};

/*--------------------Define MACRO--------------------------------------*/

#define CHK(ret) if(ret != SUCCESS){return ret;}

/*--------------------Define Struct---------------------------------------*/

/*--------------------Export global variable-------------------------------*/

/*--------------------Function declaration---------------------------------*/

extern u32 Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
);

s32 rtl_crypto_test(
	IN u16 argc,
	IN const char *argv[]
);

#endif  //__RTK_CRYPTO_BASE_TEST_H__