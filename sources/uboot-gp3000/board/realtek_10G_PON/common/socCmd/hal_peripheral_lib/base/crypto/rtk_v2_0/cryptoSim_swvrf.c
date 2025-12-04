#include <common.h>

#include "rtk_crypto_base_common.h"
#include "cryptoSim_auth.h"
#include "cryptoSim_cipher.h"
#include "cryptoSim_swvrf.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define SWVRF_CRITICAL  0

uint8_t _sw_hash_msg[CRYPTO_MAX_AAD_LEN + CRYPTO_MAX_MSG_LEN] = {0};

#if SWVRF_CRITICAL
osMutexDef(crypto_swvrf_mutex);
static osMutexId crypto_swvrf_mutex_id;

int sw_critical_init(void)
{
    crypto_swvrf_mutex_id = osMutexCreate( osMutex(crypto_swvrf_mutex) );
    if ( crypto_swvrf_mutex_id == NULL ) {
        __cryptoSim_err_printf("crypto_swvrf_mutex id is NULL \r\n");
        return FAIL;
    }

    return SUCCESS;
}

int sw_critical_enter(void)
{
    osStatus status;

    if ( crypto_swvrf_mutex_id == NULL ) {
        __cryptoSim_err_printf("crypto_swvrf_mutex id is NULL \r\n");
        return FAIL;
    }

    status = osMutexWait(crypto_swvrf_mutex_id, osWaitForever);
    if ( status != osOK ) {
        __cryptoSim_err_printf("crypto_swvrf_mutex wait failed, status=0x%x \r\n", status);
        return FAIL;
    }
    return SUCCESS;
}

int sw_critical_leave(void)
{
    osStatus status;

    if ( crypto_swvrf_mutex_id == NULL ) {
        __cryptoSim_err_printf("crypto_swvrf_mutex id is NULL \r\n");
        return FAIL;
    }

    status = osMutexRelease(crypto_swvrf_mutex_id);
    if ( status != osOK ) {
        __cryptoSim_err_printf("crypto_swvrf_mutex release failed, status=0x%x \r\n", status);
        return FAIL;
    }
    return SUCCESS;
}
#endif

#if 0
int rtl_crypto_crc_sw_setting(int order1,unsigned long polynom1, int direct1, unsigned long crcinit1, unsigned long crcxor1, int refin1, int refout1){
    int ret;
    ret = crc_swvrf_setting(order1,polynom1,direct1,crcinit1,crcxor1,refin1,refout1);
    return ret;
}

void rtl_crypto_crc_swvrf(const uint32_t crc_cal_type, const unsigned char *pText, const int textlen, unsigned char *pDigest){
    uint32_t crc;
    switch (crc_cal_type) {

        case CRC_BITBYBIT:
            crc = crc_swvrf_bitcal(pText, textlen);
            rtlc_memcpy((void *)pDigest, (void *)&crc, 4);
            break;

        case CRC_TABLE_DRIVEN:
            break;

        default:
            __cryptoSim_err_printf(" unknown crc_cal_type : %d\r\n",crc_cal_type);
            break;
    }
}
#endif

#if 1
void rtl_crypto_swvrf_init(void){
    
    mbedtls_mix_hmac_init();
#if SWVRF_CRITICAL    
    sw_critical_init();   //swvrf resource protect
#endif    
}

void rtl_crypto_auth_ghash_swvrf(const uint32_t authtype,
                                 const unsigned char *pKey, const int keylen, const unsigned char *pText, const int textlen,
                                 const unsigned char *iv, const unsigned char *pAAD, const int aadlen,
                                 unsigned char *pDigest, const int digestlen){
    switch (authtype){

        case CRYPTO_AUTH_AES_GMAC:
            aes_gmac_swvrf(pKey,keylen, iv, pText,textlen, pAAD, aadlen,pDigest);
            break;
        default:
            __cryptoSim_err_printf(" unknown authtype : %d\r\n",authtype);
            break;
    }
}

void rtl_crypto_auth_swvrf(const uint32_t authtype,
                           const unsigned char *pKey, const int keylen, const unsigned char *pText, const int textlen,
                           unsigned char *pDigest, const int digestlen){
    if (CRYPTO_AUTH_AES_GHASH == authtype) {
#if SWVRF_CRITICAL        
        sw_critical_enter();
#endif
    }
    switch (authtype){
    
        case CRYPTO_AUTH_MD5:
            mbedtls_md5_swvrf(pText, textlen, pDigest);
            break;
            
        case CRYPTO_AUTH_SHA1:
            mbedtls_sha1_swvrf(pText, textlen, pDigest);
            break;
    
        case CRYPTO_AUTH_SHA2_224:
        case CRYPTO_AUTH_SHA2_256:
            mbedtls_sha2_swvrf(pText, textlen, pDigest, (digestlen == 28)? 1:0);
            break;
        
        case CRYPTO_AUTH_HMAC_MD5:
            mbedtls_hmac_md5_swvrf(pKey, keylen, pText, textlen, pDigest);
            break;

        case CRYPTO_AUTH_HMAC_SHA1: 
            mbedtls_hmac_sha1_swvrf(pKey, keylen, pText, textlen, pDigest);
            break;
        
        case CRYPTO_AUTH_HMAC_SHA2_224: 
        case CRYPTO_AUTH_HMAC_SHA2_256: 
            mbedtls_hmac_sha2_swvrf(pKey, keylen, pText, textlen, pDigest ,(digestlen == 28)? 1:0);
            break;

        case CRYPTO_AUTH_AES_GHASH:
            aes_ghash_swvrf(pKey,keylen,pText,textlen,pDigest);
            break;
        default:
            __cryptoSim_err_printf(" unknown authtype : %d\r\n",authtype);
            break;
    }
    if (CRYPTO_AUTH_AES_GHASH == authtype) {
#if SWVRF_CRITICAL        
        sw_critical_leave();
#endif
    }    
}
#endif

int mbedtls_aes_swvrf(const uint32_t mode, const unsigned char *input, const int len, 
                             const unsigned char *key, const int lenKey, const unsigned char *iv,  
                             const unsigned char *aad, const int lenAAD, unsigned char *output, unsigned char *tag){
    //char _iv[16]; /* Since soft_AES_cbc mode will modify iv,we need to allocate one for it */
    char nonce_counter[16];
    char stream_block[16];
    int done;
    int ret = SUCCESS;
    size_t offset = 0;
    mbedtls_gcm_context ctx_gcm;    /* GCM need*/
    mbedtls_cipher_id_t cipher;      /* GCM need*/

    mbedtls_aes_context ctx;
    mbedtls_aes_init( &ctx );


    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode) || (SWDECRYPT_GCTR_AES == mode) ||
        (SWDECRYPT_GCM_AES == mode)) {
#if SWVRF_CRITICAL
            sw_critical_enter();
#endif
        if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode)) {
            /* GCM Init*/
            mbedtls_gcm_init( &ctx_gcm );
            cipher = MBEDTLS_CIPHER_ID_AES;
        }
    }
    switch (mode) {

        /*
         * DECRYPTION
         * */
        case SWDECRYPT_ECB_AES:
            ret = mbedtls_aes_setkey_dec( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            for (done = 0; done < len; done+=16) {
                // Handle 1byte(8bits), total=16bytes(128bits)
                ret = mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_DECRYPT, &input[done], &output[done]);
                if (SUCCESS != ret){
                    goto mbedtls_aes_end;
                }
            }

            break;
        case SWDECRYPT_CBC_AES:
            ret = mbedtls_aes_setkey_dec( &ctx, key, lenKey*8 );
            if (SUCCESS != ret){
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_DECRYPT, len, (unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWDECRYPT_CFB_AES:
            offset = 0;
            // set key of drcryption in CFB mode is same as encryption
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_DECRYPT, len, &offset, (unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }            
            break;

        case SWDECRYPT_OFB_AES:
            offset = 0;
            // set key of drcryption in OFB mode is same as encryption
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_ofb128( &ctx, len, &offset, (unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWDECRYPT_CTR_AES:
            rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
            rtlc_memset(stream_block, 0, sizeof(stream_block));
            offset = 0;
            // set key of drcryption in CTR mode is same as encryption
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret){
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_ctr( &ctx, len, &offset, (unsigned char *)nonce_counter, (unsigned char *)stream_block, input, output);
            if (SUCCESS != ret){
                goto mbedtls_aes_end;
            }
            break;

        case SWDECRYPT_GCTR_AES:
            rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));    
            ret = aes_gctr_swvrf(key, lenKey, (unsigned char *)nonce_counter, input, len, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWDECRYPT_GCM_AES:
            // need cipher_type(can choose other crypto_algorithms)
            ret = mbedtls_gcm_setkey( &ctx_gcm, cipher, key, lenKey*8 );
            if (SUCCESS != ret) {
                __cryptoSim_err_printf("gcm_setkey decrypt fail\r\n");
                goto mbedtls_aes_end;
            } else {
                // ivlen = 12 bytes(96bits), taglen = 16 bytes(128bits)
                ret = mbedtls_gcm_crypt_and_tag( &ctx_gcm, MBEDTLS_GCM_DECRYPT, len,
                                           (unsigned char *)iv, 12,
                                           aad, lenAAD,
                                           input, output, 16, tag);
                if (SUCCESS != ret) {
                    __cryptoSim_err_printf("gcm_crypt_and_tag decrypt fail\r\n");
                    goto mbedtls_aes_end;
                }
            }
            break;

        /*
         * ENCRYPTION
         * */
        case SWENCRYPT_ECB_AES:
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            for ( done = 0; done < len; done += 16) {
                // Handle 1byte(8bits), total=16bytes(128bits)
                ret = mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_ENCRYPT, &input[done], &output[done]);
                if (SUCCESS != ret) {
                    goto mbedtls_aes_end;
                }
            }
            break;

        case SWENCRYPT_CBC_AES:
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_ENCRYPT, len, (unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }            
            break;

        case SWENCRYPT_CFB_AES:
            offset = 0;
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_ENCRYPT, len, &offset,(unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWENCRYPT_OFB_AES:
            offset = 0;
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_ofb128( &ctx, len, &offset,(unsigned char *)iv, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }            
            break;

        case SWENCRYPT_CTR_AES:
            rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
            rtlc_memset(stream_block, 0, sizeof(stream_block));
            offset = 0;
            // set key of drcryption in CTR mode is same as encryption
            ret = mbedtls_aes_setkey_enc( &ctx, key, lenKey*8 );
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            ret = mbedtls_aes_crypt_ctr( &ctx, len, &offset, (unsigned char *)nonce_counter, (unsigned char *)stream_block, input, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWENCRYPT_GCTR_AES:
            rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));    
            ret = aes_gctr_swvrf(key, lenKey, (unsigned char *)nonce_counter, input, len, output);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
            break;

        case SWENCRYPT_GCM_AES:
            // need cipher_type(can choose other crypto_algorithms)
            //__dbg_printf("lenkey = %d\r\n",lenKey);
            ret = mbedtls_gcm_setkey( &ctx_gcm, cipher, key, lenKey*8 );
            if (SUCCESS != ret) {
                __cryptoSim_err_printf("gcm_setkey encrypt fail\r\n");
                goto mbedtls_aes_end;
            } else {
                // ivlen = 12 bytes(96bits), taglen = 16 bytes(128bits)
                ret = mbedtls_gcm_crypt_and_tag( &ctx_gcm, MBEDTLS_GCM_ENCRYPT, len,
                                                 (unsigned char *)iv, 12,
                                                  aad, lenAAD,
                                                  input, output, 16, tag);
                if (SUCCESS != ret) {
                    __cryptoSim_err_printf("gcm_crypt_and_tag encrypt fail\r\n");
                    goto mbedtls_aes_end;
                }
            }
            break;

    }

mbedtls_aes_end:
    mbedtls_aes_free( &ctx );
    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode)) {
        mbedtls_gcm_free( &ctx_gcm); //!!important
    }
    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode) || (SWDECRYPT_GCTR_AES == mode) ||
        (SWDECRYPT_GCM_AES == mode)) {
#if SWVRF_CRITICAL
        sw_critical_leave();
#endif
    }
    return (ret);
}


int rtl_crypto_cipher_swvrf(const uint32_t cipher_type,
                             const unsigned char *pKey, const int keylen, const unsigned char *pIv, const int ivLen, const int init_count,
                             const unsigned char *pAAD, const int aadlen, const unsigned char *pText, const int textlen,
                             unsigned char *pResult, unsigned char *pTag){
    
    int ret = SUCCESS;
    ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
    return (ret);
}

int rtl_crypto_swvrf_mix_process(const uint32_t mix_mode_type, const uint32_t cipher_type, const uint32_t auth_type,
                                 const unsigned char *pText, const int textlen, const unsigned char *pKey,const int keylen,
                                 const unsigned char *pIv, const int ivLen, const unsigned char *pAAD, const int aadlen,
                                 const unsigned char *phash_key, const int hash_keylen, unsigned char *pResult,
                                 unsigned char *pTag, const int digestlen) {
    int ret = SUCCESS;
    rtlc_memset(&_sw_hash_msg,0,sizeof(_sw_hash_msg));
    switch (mix_mode_type) {

        /* 
         * Encryption
         */
        case CRYPTO_MIX_MODE_SW_SSH_ENC:
            ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
            if (ret != SUCCESS) {
                goto mix_process_end;
            } else {
                rtlc_memcpy(_sw_hash_msg,pAAD,aadlen);
                rtlc_memcpy((_sw_hash_msg+aadlen),pText,textlen);
                //__crypto_mem_dump(_sw_hash_msg, (aadlen+textlen), "Hash msg: ");
                rtl_crypto_auth_swvrf(auth_type, phash_key, hash_keylen, _sw_hash_msg, (aadlen+textlen), pTag, digestlen);
            }

            break;

        case CRYPTO_MIX_MODE_SW_ESP_ENC:
            ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
            if (ret != SUCCESS) {
                goto mix_process_end;
            } else {
                rtlc_memcpy(_sw_hash_msg,pAAD,aadlen);
                rtlc_memcpy((_sw_hash_msg+aadlen),pResult,textlen);
                rtl_crypto_auth_swvrf(auth_type, phash_key, hash_keylen, _sw_hash_msg, (aadlen+textlen), pTag, digestlen);
            }

            break;
            
        /* 
         * Decryption
         */
        case CRYPTO_MIX_MODE_SW_SSH_DEC:
            ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
            if (ret != SUCCESS) {
                goto mix_process_end;
            } else {
                rtlc_memcpy(_sw_hash_msg,pAAD,aadlen);
                rtlc_memcpy((_sw_hash_msg+aadlen),pResult,textlen);
                rtl_crypto_auth_swvrf(auth_type, phash_key, hash_keylen, _sw_hash_msg, (aadlen+textlen), pTag, digestlen);
            }
            break;

        case CRYPTO_MIX_MODE_SW_ESP_DEC:
            ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
            if (ret != SUCCESS) {
                goto mix_process_end;
            } else {
                rtlc_memcpy(_sw_hash_msg,pAAD,aadlen);
                rtlc_memcpy((_sw_hash_msg+aadlen),pText,textlen);
                rtl_crypto_auth_swvrf(auth_type, phash_key, hash_keylen, _sw_hash_msg, (aadlen+textlen), pTag, digestlen);
            }
            break;
    }

mix_process_end:    
    return (ret);
}


#ifdef  __cplusplus
}
#endif
