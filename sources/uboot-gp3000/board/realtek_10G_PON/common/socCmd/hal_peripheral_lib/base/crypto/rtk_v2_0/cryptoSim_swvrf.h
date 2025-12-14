#ifndef __CRYPTO_SIM_SW_VRF_H__
#define __CRYPTO_SIM_SW_VRF_H__

#ifdef  __cplusplus
extern "C" {
#endif

// mode Auth
#define CRYPTO_NOAUTH               ((uint32_t) - 1)
#define CRYPTO_AUTH_MD5             (1)
#define CRYPTO_AUTH_SHA1            (2)
#define CRYPTO_AUTH_SHA2_224        (3)
#define CRYPTO_AUTH_SHA2_256        (4)
#define CRYPTO_AUTH_HMAC_MD5        (5)
#define CRYPTO_AUTH_HMAC_SHA1       (6)
#define CRYPTO_AUTH_HMAC_SHA2_224   (7)
#define CRYPTO_AUTH_HMAC_SHA2_256   (8)
#define CRYPTO_AUTH_POLY1305        (15)
#define CRYPTO_AUTH_AES_GHASH       (9)
#define CRYPTO_AUTH_AES_GMAC        (10)

#define CRYPTO_AUTH_PROCESS         (11)
#define CRYPTO_AUTH_UPDATE          (12)

#define CRYPTO_CRC_CMD              (13)
#define CRYPTO_CRC_DMA              (14)
#define CRC_BITBYBIT                (1)
#define CRC_TABLE_DRIVEN            (2)

//mode cipher,same as hal_crypto.h define
#define CRYPTO_CIPHER_DES_ECB             0x0
#define CRYPTO_CIPHER_DES_CBC             0x1
#define CRYPTO_CIPHER_3DES_ECB            0x10
#define CRYPTO_CIPHER_3DES_CBC            0x11
#define CRYPTO_CIPHER_AES_ECB             0x20
#define CRYPTO_CIPHER_AES_CBC             0x21
#define CRYPTO_CIPHER_AES_CFB             0x22
#define CRYPTO_CIPHER_AES_OFB             0x23
#define CRYPTO_CIPHER_AES_CTR             0x24
#define CRYPTO_CIPHER_AES_GCTR            0x25
#define CRYPTO_CIPHER_AES_GMAC            0x26
#define CRYPTO_CIPHER_AES_GHASH           0x27
#define CRYPTO_CIPHER_AES_GCM             0x28

#define CRYPTO_CIPHER_CHACHA_POLY1305     0x30
#define CRYPTO_CIPHER_CHACHA              0x31

#define CRYPTO_MIX_MODE_SW_SSH_ENC             0x11
#define CRYPTO_MIX_MODE_SW_SSH_DEC             0x01
#define CRYPTO_MIX_MODE_SW_ESP_ENC             0x12
#define CRYPTO_MIX_MODE_SW_ESP_DEC             0x02
#define CRYPTO_MIX_MODE_SW_SSL_TLS_ENC         0x13
#define CRYPTO_MIX_MODE_SW_MASK_TYPE           0x03    //SSH/ESP/SSL
#define CRYPTO_MIX_MODE_SW_MASK_FUNC           0x10

#define CRYPTO_MAX_AAD_LEN       496  // 16*((2^5)-1)
#define CRYPTO_MAX_MSG_LEN       65536 // 64k bytes


//DES
#define SWDECRYPT_ECB_DES           0X00
#define SWDECRYPT_CBC_DES           0X01
#define SWDECRYPT_ECB_3DES          0X10
#define SWDECRYPT_CBC_3DES          0X11

#define SWENCRYPT_ECB_DES           0X80
#define SWENCRYPT_CBC_DES           0X81
#define SWENCRYPT_ECB_3DES          0X90
#define SWENCRYPT_CBC_3DES          0X91

//AES
#define SWDECRYPT_ECB_AES           0X20
#define SWDECRYPT_CBC_AES           0X21
#define SWDECRYPT_CFB_AES           0X22
#define SWDECRYPT_OFB_AES           0X23
#define SWDECRYPT_CTR_AES           0X24
#define SWDECRYPT_GCTR_AES          0X25
#define SWDECRYPT_GCM_AES           0X28

#define SWENCRYPT_ECB_AES           0Xa0
#define SWENCRYPT_CBC_AES           0Xa1
#define SWENCRYPT_CFB_AES           0Xa2
#define SWENCRYPT_OFB_AES           0Xa3
#define SWENCRYPT_CTR_AES           0Xa4
#define SWENCRYPT_GCTR_AES          0Xa5
#define SWENCRYPT_GCM_AES           0Xa8

//CHACHA
#define SWDECRYPT_CHACHA            0X31
#define SWENCRYPT_CHACHA            0xb1

//CHACHA_POLY1305
#define SWDECRYPT_CHACHA_POLY1305   0X30
#define SWENCRYPT_CHACHA_POLY1305   0xb0


void rtl_crypto_swvrf_init(void);

int rtl_crypto_crc_sw_setting(int order1,unsigned long polynom1, int direct1, unsigned long crcinit1, unsigned long crcxor1, int refin1, int refout1);

void rtl_crypto_crc_swvrf(const uint32_t crc_cal_type, const unsigned char *pText, const int textlen, unsigned char *pDigest);

void rtl_crypto_auth_ghash_swvrf(const uint32_t authtype,
                                 const unsigned char *pKey, const int keylen, const unsigned char *pText, const int textlen,
                                 const unsigned char *iv, const unsigned char *pAAD, const int aadlen,
                                 unsigned char *pDigest, const int digestlen);

void rtl_crypto_auth_swvrf(const uint32_t authtype,
                           const unsigned char *pKey, const int keylen, const unsigned char *pText, const int textlen,
                           unsigned char *pDigest, const int digestlen);

int rtl_crypto_cipher_swvrf(const uint32_t cipher_type,
                             const unsigned char *pKey, const int keylen, const unsigned char *pIv, const int ivLen, const int init_count,
                             const unsigned char *pAAD, const int aadLen, const unsigned char *pText, const int textlen,
                             unsigned char *pResult, unsigned char *pTag);

int rtl_crypto_swvrf_mix_process(const uint32_t mix_mode_type, const uint32_t cipher_type, const uint32_t auth_type,
                                 const unsigned char *pText, const int textlen, const unsigned char *pKey,const int keylen,
                                 const unsigned char *pIv, const int ivLen, const unsigned char *pAAD, const int aadlen,
                                 const unsigned char *phash_key, const int hash_keylen, unsigned char *pResult,
                                 unsigned char *pTag, const int digestlen);


#ifdef  __cplusplus
}
#endif

#endif // __CRYPTO_SIM_SW_VRF_H__
