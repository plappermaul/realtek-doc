/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

extern "C" {

#include <stdlib.h>
#include <string.h>
#include "rtl8195a.h"

//simulation
#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/gcm.h"

#include "cryptoSim.h"
#include "rtl8195a_crypto_util_rom.h"

#define CRYPTO_MAX_KEY_LENGTH		32  // MAX  is  AES-256 : 32 byte,  3DES : 24 byte
#define CRYPTO_MD5_DIGEST_LENGTH 	16
#define CRYPTO_SHA1_DIGEST_LENGTH 	20
#define CRYPTO_SHA2_224_DIGEST_LENGTH 28
#define CRYPTO_SHA2_DIGEST_LENGTH 	32

#define CRYPTO_TEST_DMA_MODE 0

#define HMAC_MAX_MD_CBLOCK 64


    extern int rtl_memcmp(const void * m1 , const void * m2 , size_t n);

    extern void *memcpy( void *s1, const void *s2, SIZE_T n );


//extern HAL_TIMER_OP HalTimerOp;


#if 1
// TODO:
// will modify to see how to do assertion and print out in the future
// +++++++++++++++++++++++++++++++++++++++++


#define memDump rtl_memDump

#define DBG_CRYPTO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_CRYPTO_)) \
        _DbgDump("\r"IPSEC_INFO_PREFIX __VA_ARGS__);\
}while(0)


#define dbg_printk(fmt, args...) \
	     	DBG_CRYPTO_INFO("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args);

#define dbg_mem_dump(start, size, str_header) \
	if ( ConfigDebugInfo & _DBG_CRYPTO_ ) { \
	    	printk("%s():%d : memdump : address: %08x, size: %d\n", __FUNCTION__, __LINE__, start, size); \
	    	memDump((const u8*)start, size, (char*)str_header); \
	}

#if 0
#define rtl_assert(x)  do { \
					if((x) == 0) {\
							err_printk("\n\rAssert(" #x ") failed"); \
                            rtl_halt();\
						} \
					} while(0)




#else

#define rtl_assert(x)
#endif


#define err_printk(fmt, args...) \
			DBG_CRYPTO_ERR("%s():%d : ERROR : " fmt "\n", __FUNCTION__, __LINE__, ##args)

#define rtl_halt(x) \
							err_printk("\n\r HALT! \n\r"); \
							for(;;);

#endif


#define Crypto_CMD	"CRYPTO"



    u8 _asic_authkey[256] __attribute__((aligned(4)));


    u8 _asic_cryptkey[256] __attribute__((aligned(4)));


    u8 _asic_iv[256] __attribute__((aligned(4)));

    u8 _asic_aad[128];

    u8 _asic_orig[32+CRYPTO_MAX_MSG_LENGTH*1+32];


    u8 _asic_enc[32 + CRYPTO_MAX_MSG_LENGTH*1 + 32];

    u8 _sw_enc[32 + CRYPTO_MAX_MSG_LENGTH*1 + 32];

    u8 _asic_dec[32 + CRYPTO_MAX_MSG_LENGTH*1 + 32];

    u8 _sw_dec[32 + CRYPTO_MAX_MSG_LENGTH*1 + 32];

    u8 _AsicDigest[32+CRYPTO_MAX_DIGEST_LENGTH+32];

    u8 sw_digest[CRYPTO_MAX_DIGEST_LENGTH];

    u8 _sw_orig[32 + CRYPTO_MAX_MSG_LENGTH + 32];

//

    u32 this_rtlseed;

    uint32 dbg_level = 2;

    /******************************************************************************
     * External Function
     ******************************************************************************/

    extern u32
    Strtoul(
        IN  const u8 *nptr,
        IN  u8 **endptr,
        IN  u32 base
    );

    extern int rtl_crypto_mix(IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type, IN const u8* message, IN const u32 msglen,
                              IN u32 lenCipherKey, IN void* pCipherKey, IN u32 lenAuthKey, IN void* pAuthkey,
                              IN void* pIv, IN const u32 ivlen, IN u32 a2eo, OUT void* pResult,
                              OUT void* pDigest);

//    extern int authSim(const u32 mode, const u8 * data, const u32 dataLen, const u8 * key, const u32 keyLen,
//                       u8 * digest, const u32 digestlen);
    extern void rtl_crypto_auth_sim(const u32 authtype,
                                    const u8* pKey, const u32 keylen, const u8* pText, const u32 textlen, u8* pDigest, const u32 digestlen);

    /******************************************************************************
     * Functions
     ******************************************************************************/
    int32 ipsecSim(uint32 modeCrypto, uint32 modeAuth,
                   uint8 *data, uint8 *pCryptResult,
                   uint32 lenCryptoKey, uint8 *pCryptoKey,
                   uint32 lenAuthKey, uint8 *pAuthKey,
                   uint8 *pIv, uint8 *pPad, uint8 *pDigest,
                   uint32 lenA2eo, uint32 lenEnl, uint32 mode_select)
    {
        uint32 digestlen=0;
        uint32 sim_modeAuth;
        uint8 *data_padding;
        uint32 data_padding_len;
        uint32 crypto_block_size;
        uint32 i;

        switch (modeAuth) {
            case 2:
            case 6:
                sim_modeAuth = 1 + 3*((modeAuth>>2)&0x1);
                digestlen = CRYPTO_MD5_DIGEST_LENGTH;
                break;

            case 0:
            case 4:
                sim_modeAuth = 2 + 3*((modeAuth>>2)&0x1);
                digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
                break;

            case 0x11:
            case 0x15:
                sim_modeAuth = 3 + 3*((modeAuth>>2)&0x1);
                digestlen = CRYPTO_SHA2_224_DIGEST_LENGTH;
                break;

            case 0x21:
            case 0x25:
                sim_modeAuth = 3 + 3*((modeAuth>>2)&0x1);
                digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
                break;

            default:
                digestlen = CRYPTO_MD5_DIGEST_LENGTH;
                //return FAILED;
        }
//printf("(%s-%d)\n", __func__, __LINE__);

        if ((modeAuth != (uint32) -1) &&
                (modeCrypto != (uint32) -1)) {
            //printf("(%s-%d)\n", __func__, __LINE__);

            if (modeCrypto & 0x80) { // encrypt then auth
#ifdef CRYPTO_LOOPBACK_DEBUG
                memcpy(pCryptResult, data, lenA2eo + lenEnl);
#else
                if ((mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) || (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)) {
                    rtl_crypto_auth_sim((sim_modeAuth), pAuthKey, lenAuthKey, data, lenA2eo + lenEnl,
                                        pDigest, digestlen);
                }
                if (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
                    memcpy(data+lenA2eo + lenEnl, pDigest, digestlen);
                    data_padding = data+lenA2eo + lenEnl + digestlen;
                    if (modeCrypto & 0x20)
                        crypto_block_size=16;
                    else
                        crypto_block_size=8;
                    data_padding_len = crypto_block_size-((lenEnl + digestlen)%crypto_block_size);
                    for (i=0 ; i<data_padding_len ; i++) {
                        data_padding[i]=data_padding_len-1;
                    }
                    if (modeCrypto & 0x20) // aes
                        aesSim_aes(modeCrypto, data + lenA2eo, lenEnl+digestlen+data_padding_len, lenCryptoKey,
                                   pCryptoKey, pIv, 0/*aadLen*/, 0/*pAAD*/,(uint8*)(((uint32_t)pCryptResult) + lenA2eo), 0/*pTag*/);
                    else // des
                        desSim_des(modeCrypto, data + lenA2eo, lenEnl+digestlen+data_padding_len,
                                   pCryptoKey, pIv, (uint8*)(((uint32_t)pCryptResult) + lenA2eo));

                } else {
                    if (modeCrypto & 0x20) // aes
                        aesSim_aes(modeCrypto, data + lenA2eo, lenEnl, lenCryptoKey,
                                   pCryptoKey, pIv, 0/*aadLen*/, 0/*pAAD*/,(uint8*)(((uint32_t)pCryptResult) + lenA2eo), 0/*pTag*/);
                    else // des
                        desSim_des(modeCrypto, data + lenA2eo, lenEnl,
                                   pCryptoKey, pIv, (uint8*)(((uint32_t)pCryptResult) + lenA2eo));
                }
//printf("(%s-%d)\n", __func__, __LINE__);

                memcpy(pCryptResult, data, lenA2eo);
#endif
//printf("(%s-%d)\n", __func__, __LINE__);

                //authSim((modeAuth&7), pCryptResult, lenA2eo + lenEnl,
                if (mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) {
                    rtl_crypto_auth_sim((sim_modeAuth), pAuthKey, lenAuthKey, pCryptResult, lenA2eo + lenEnl,
                                        pDigest, digestlen);
                }
            } else { // auth then decrypt
                //authSim(modeAuth&7, data, lenA2eo + lenEnl,
                if (mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) {
                    rtl_crypto_auth_sim(sim_modeAuth, pAuthKey, lenAuthKey, data, lenA2eo + lenEnl,
                                        pDigest, digestlen);
                }

#ifdef CRYPTO_LOOPBACK_DEBUG
                memcpy(pCryptResult, data, lenA2eo + lenEnl);
#else
                if (modeCrypto & 0x20) // aes
                    aesSim_aes(modeCrypto, data + lenA2eo, lenEnl, lenCryptoKey,
                               pCryptoKey, pIv, 0/*aadLen*/, 0/*pAAD*/, (uint8*)(((uint32_t)pCryptResult) + lenA2eo), 0/*pTag*/);
                else // des
                    desSim_des(modeCrypto, data + lenA2eo, lenEnl,
                               pCryptoKey, pIv, (uint8*)(((uint32_t)pCryptResult) + lenA2eo));

                memcpy(pCryptResult, data, lenA2eo);
                if (mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) {
                    rtl_crypto_auth_sim((sim_modeAuth), pAuthKey, lenAuthKey, pCryptResult, lenA2eo + lenEnl,
                                        pDigest, digestlen);
                }
#endif
            }
        } else if (modeAuth != (uint32) -1) { // auth only
//		printf("(%s-%d)\n", __func__, __LINE__);

            rtl_crypto_auth_sim(sim_modeAuth, pAuthKey, lenAuthKey, data, lenA2eo + lenEnl,
                                pDigest, digestlen);

        } else if (modeCrypto != (uint32) -1) { // crypto only
//		printf("(%s-%d)\n", __func__, __LINE__);
#ifdef CRYPTO_LOOPBACK_DEBUG
            memcpy(pCryptResult, data, lenA2eo + lenEnl);
#else

            if (modeCrypto & 0x20) // aes
                aesSim_aes(modeCrypto, (uint8_t*)(((uint32_t)data) + lenA2eo), lenEnl, lenCryptoKey,
                           pCryptoKey, pIv, 0/*aadLen*/, 0/*pAAD*/, (uint8*)(((uint32_t)pCryptResult) + lenA2eo), 0/*pTag*/);
            else // des
                desSim_des(modeCrypto, ((uint8_t*)((uint32_t)data) + lenA2eo), lenEnl,
                           pCryptoKey, pIv, (uint8*)(((uint32_t)pCryptResult) + lenA2eo));
#endif
        } else {
            return FAILED;
        }

        return SUCCESS;
    }


    poly1305_state poly1305State;


    void rtl_crypto_auth_sim(const u32 authtype,
                             const u8* pKey, const u32 keylen, const u8* pText, const u32 textlen, u8* pDigest, const u32 digestlen)
    {
        int     ret;
        uint32_t crc;


        switch ( authtype ) {
            case 1 : //MD5
                mbedtls_md5_sw(pText, textlen, pDigest);
                break;
            case 2 : // SHA1
                mbedtls_sha1(pText, textlen, pDigest);
                break;
            case 3 : // SHA2
                mbedtls_sha256(pText, textlen, pDigest, (digestlen==28)?1:0 );
                break;
            case 4 : // HMAC-MD5
                mbedtls_md_hmac_md5(pKey, keylen, pText, textlen, pDigest);
                break;
            case 5 : // HMAC-SHA1
                mbedtls_md_hmac_sha1(pKey, keylen, pText, textlen, pDigest);
                break;
            case 6 : // HMAC-SHA2
                if ( digestlen == 224/8 ) {
                    mbedtls_md_hmac_sha224(pKey, keylen, pText, textlen, pDigest);
                } else {
                    mbedtls_md_hmac_sha256(pKey, keylen, pText, textlen, pDigest);
                }
                break;
            case 7 : // poly1305
                memset(&poly1305State, 0, sizeof(poly1305State));
                CRYPTO_sim_poly1305_init(&poly1305State, pKey);
                CRYPTO_sim_poly1305_update(&poly1305State, pText, textlen);
                CRYPTO_sim_poly1305_finish(&poly1305State, pDigest);
                break;
            case 8 : //crc32
                crc = crc_tester(pText, textlen);
                //crc = crc32(0, pText, textlen);
                memcpy((void*)pDigest, (void*)&crc, digestlen);
                break;
            default:
                printk(" %s : unknown authtype %d \r\n", __FUNCTION__, authtype);
                break;
        }

    }



    int rtl_crypto_esp_sim(const u32 modeCrypto,
                           const u8* pKey, const u32 keylen, const u8* pIv, const u32 ivLen, const u32 init_count, const u8* pAAD, const u32 aadLen, const u8* pText, const u32 textlen,
                           u8* pResult, u8* pTag)
    {
        int     ret = SUCCESS;



        dbg_printk("modeCrypto=0x%x, pText=%p, pResult=%p, keylen=%d, pKey=%p, pIv=%p, ivLen=%d, pAAD=%p, aadLen=%d, pText=%p textlen=%d\n",
                   modeCrypto, pText, pResult, keylen, pKey, pIv, ivLen, pAAD, aadLen, pText, textlen);

        dbg_mem_dump(pText, textlen, "Text :");
        dbg_mem_dump(pKey, keylen, "Key :");
        dbg_mem_dump(pIv, ivLen, "IV :");
        dbg_printk("init_count=%d\r\n", init_count);
        dbg_mem_dump(pAAD, aadLen, "AAD :");

        if ( (modeCrypto & 0x30) == 0x30 ) { // chacha20
            CRYPTO_sim_chacha_20(pResult, pText, textlen, pKey, pIv, init_count);
        } else if ( (modeCrypto & 0x20) == 0x20 ) { // aes
            ret = aesSim_aes(modeCrypto, pText,
                             textlen, keylen, pKey, pIv, aadLen, pAAD, pResult, pTag);
        } else { // 3des / des
            ret = desSim_des(modeCrypto, pText,
                             textlen, pKey, pIv, pResult);
        }

        return ret;
    }





    void rtl_crypto_usage(char *type)
    {

        if (type == NULL) {
            //printk("Usage: crypto <init|rand|throughput|asic|info|debug|vector|config>\n");
            printk("Usage: crypto <init|vector|test>\n");
        } else if (strcmp(type, "init") == 0) {
            printk("Usage:" Crypto_CMD "init\n");
            printk("  max_desc:  default: 8 descriptors\n");
            printk("  dma_burst: default: 2 (64 bytes)\n");
        } else if (strcmp(type, "debug") == 0) {
            printk("Usage:" Crypto_CMD "debug [mode]\n");
            printk("  mode:  0: turn off, 1: turn on \n");
        } else if (strcmp(type, "vector") == 0) {
            //
            //		vector 0 16 -1 -> 3DES-CBC : 0x10
            //		vector 0 18 -1 -> 3DES-ECB : 0x12
            //		vector 0 32 -1 -> AES-CBC :   0x20
            //		vector 0 34 -1 -> AES-ECB  :  0x22
            //		vector 0 35 -1 -> AES-CTR  : 0x23
            //
            //		vector 1 -1 0 -> SHA1
            //		vector 1 -1 1 -> SHA2
            //		vector 1 -1 2 -> MD5
            //		vector 1 -1 4 -> HMAC-SHA1
            //		vector 1 -1 5 -> HMAC-SHA2
            //		vector 1 -1 6 -> HMAC-MD5
            //           vector 1 -1 7 -> Poly1305

            printk("Usage:" Crypto_CMD "vector <mode> <cipher_type> <auth_type> \n");
            printk("  mode: 0: Auth, 1: Cipher\n");
            printk("  cipher_type: -1 : None, 16: 3DES-CBC, 18: 3DES-ECB, 32: AES_CBC, 34: AES-ECB, 35: AES-CTR \n");
            printk("  auth_type: -1: None, 0: SHA1, 1: SHA2, 2: MD5, 4:HMAC-SHA1, 5: HMAC-SHA2, 6: HMAC-MD5 \n");
        } else if (strcmp(type, "test") == 0) {
            printk("Usage:" Crypto_CMD "test\n");
            printk("  will do random test automatically\n");
//        printk("  seed: random seed\n");
//        printk("  round: test round, default: 100\n");
//        printk("  crypto_mode: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
//        printk("             : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
//        printk("             : 0xffffffff: no crypto, default: random \n");
//        printk("  auth_mode: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
//        printk("             : 0xffffffff: no auth, default: random\n");
//        printk("  cntScatter: 1..MAX_SCATTER: count scatter, -1: random, default: -1\n");
        }
#if 0
        else if (strcmp(type, "throughput") == 0) {
            printk("Usage:" Crypto_CMD "throughput <asic> <round> <crypto_start> <crypto_end>\n");
            printk("                         <auth_start> <auth_end> <ckey_len>\n");
            printk("                         <akey_len> <pkt_len> <a2eo> <cntScatter>\n");
            printk("  asic: 0: sw, 1: asic\n");
            printk("  round: test round\n");
            printk("  crypto: 0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES \n");
            printk("        : 0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES\n");
            printk("        : 0xffffffff: no crypto\n");
            printk("  auth: 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1\n");
            printk("      : 0xffffffff: no auth\n");
            printk("  ckey_len: 16: AES128, 24: AES192, 32: AES256\n");
            printk("  akey_len: authentication length\n");
            printk("  pkt_len: packet length\n");
            printk("  a2eo: authentication to encryption offset\n");
            printk("  cntScatter: 1..MAX_SCATTER: count scatter\n");
        } else if (strcmp(type, "asic") == 0) {
            printk("Usage:" Crypto_CMD "asic <round> <len_start> <len_end>\n");
            printk("                   <off_start> <off_end>\n");
            printk("                   <crypto_start> <crypto_end>\n");
            printk("                   <auth_start> <auth_end>\n");
            printk("                   <ckeylen_start> <ckeylen_end>\n");
            printk("                   <akeylen_start> <akeylen_start>\n");
            printk("                   <a2eo_start> <a2eo_end>\n");
            printk("                   <cntScatter>\n");
        } else if (strcmp(type, "debug") == 0) {
            printk("Usage:" Crypto_CMD " debug <level>\n");
            printk("  level: 0: show error, 1: show success, 2: show skip, 3: show all, default: 0\n");
        } else if (strcmp(type, "config") == 0) {
            printk("Usage:" Crypto_CMD "config <config_name> <config_value>\n");
            printk("  config_name: sawb, config_value: 0 = disabled, 1 = enabled\n");
            printk("  config_name: fr, config_value: 0 = failed exit, 1 = do failed reset\n");
            printk("  config_name: seed, config_value: seed_value\n");
        }
#endif
    }



    /********************************************************************
     *          input
     *         |     |
     *         v     v
     *       SWenc HWenc
     *         |     |
     *         Compare
     *         |     |
     *         v     v
     *       SWdec HWdec
     *         |     |
     *         Compare
     *
     * notes: total length (in scatters) = lenA2eo + lenEnl
     ********************************************************************/
    static int32 crypto_GeneralApiTestItem(uint32 modeCrypto, uint32 modeAuth,
                                           u8 *message, u32 msglen, void *pCryptResult,
                                           uint32 lenCryptoKey, void *pCryptoKey,
                                           uint32 lenAuthKey, void *pAuthKey,
                                           void *pIv, void *pPad, void *pDigest1,
                                           uint32 lenA2eo, uint32 lenEnl, uint32 mode_select1)
    {
        int i;
        int32 retval;
        //uint8 *cur;
        //uint32 data_len;
        //static uint8 sw_orig[MAX_PKTLEN];
        //static uint8 sw_enc[MAX_PKTLEN];
        //static uint8 sw_dec[MAX_PKTLEN];
        //static uint8 sw_digest[SHA_DIGEST_LENGTH];
        //void *pDecryptoKey;
        uint32 sawb;
        uint32 mode_select=mode_select1;
        uint32 ivlen;
        uint32 digestlen=0;
        uint32 temp;
        uint8 *pu8CryptResult = (uint8 *)pCryptResult;
        static uint8 *asic_dec;
        void *pDigest = pDigest1;

        //asic_dec = (void *) UNCACHED_ADDRESS(&_asic_dec[32]);
        asic_dec = (uint8 *) (&_asic_dec[32]);

        sawb = 0;//rtl_ipsecEngine_sawb();
        //if (!sawb && pCryptResult == NULL) {
        //    printk("no pCryptResult && no sawb!\n");
        //    return FAILED;
        //}

        if (modeAuth == (uint32)-1 && modeCrypto == (uint32)-1) {
            // do nothing, skip it
            return 1;
        }

        // -1:NONE
        // 0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1 --old
        // 0:HASH_SHA1, 2:HASH_MD5, 4:HMAC_SHA1, 6:HMAC_MD5, 0x11:HASH_SHA2-224, 0x21:HASH_SHA2-256
        // 0x15:HMAC_SHA2-224, 0x25:HMAC_SHA2-256
        //if (modeAuth != (uint32)-1 && modeAuth > 0x25)
        //{
        // auth type check failed
        //return 2;
        //}
        if (modeAuth != (uint32)-1) {
            switch (modeAuth) {
                case 2:
                case 6:
                    digestlen = CRYPTO_MD5_DIGEST_LENGTH;
                    break;

                case 0:
                case 4:
                    digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
                    break;

                case 0x11:
                case 0x15:
                    digestlen = CRYPTO_SHA2_224_DIGEST_LENGTH;
                    break;

                case 0x21:
                case 0x25:
                    digestlen = CRYPTO_SHA2_DIGEST_LENGTH;
                    break;

                default:
                    //auth type check failed
                    return 2;
            }
        } else
            pDigest = NULL;

        // -1:NONE
        // 0x00:ECB_DES,0x01:ECB_3DES,0x10:CBC_DES,0x11:CBC_3DES
        // 0x20:ECC_AES,0x21:CBC_AES,0x24:CTR_AES
        if (!(modeCrypto == (uint32)-1 ||
                modeCrypto==0x00 ||
                modeCrypto==0x01 ||
                modeCrypto==0x10 ||
                modeCrypto==0x11 ||
                modeCrypto==0x20 ||
                modeCrypto==0x21 ||
                modeCrypto==0x24)) {
            // crypto type check failed
            return 3;
        }
        switch (modeCrypto) {
            case 0:
            case 1:
            case 0x10:
            case 0x11:
                ivlen = 8;
                break;
            case 0x21:
                ivlen = 16;
                break;
            case 0x20:
                ivlen = 0;
                break;
            case 0x24:
                ivlen = 8;
                break;
            default:
                ivlen = 0;
        }

        // 0,1: DES_CBC or DES_ECB
        // 10,11: 3DES_CBC or 3DES_ECB
        // 20,21,24: AES_CBC or AES_ECB or AES_CTR
        if (((modeCrypto == 0 || modeCrypto == 1) && lenCryptoKey != 8) ||
                ((modeCrypto == 0x10 || modeCrypto == 0x11) && lenCryptoKey != 24) ||
                ((modeCrypto == 0x20 || modeCrypto == 0x21 || modeCrypto == 0x24) &&
                 (lenCryptoKey < 16 || lenCryptoKey > 32))) {
            // crypto length check failed
            return 4;
        }

        // mix mode check
        if (modeCrypto != (uint32)-1 && (!(modeAuth&0x4))) {
            // crypto + hash is not support
            //return 5; jwsyu 20160630
        }

        // enl length check
        if (modeCrypto != (uint32)-1) {
            if (modeCrypto & 0x20) { /* AES, ENL is the times of 16 bytes. */
                if (mode_select!=CRYPTO_MS_TYPE_MIX3_sslenc) {
                    if (lenEnl & 0xf)
                        return 6;
                }
            } else { /* DES, ENL is the times of 8 bytes. */
                if (mode_select!=CRYPTO_MS_TYPE_MIX3_sslenc) {
                    if (lenEnl & 0x7)
                        return 7;
                }
            }

            if ((int) lenEnl <= 0)
                return 8;
        }

        // a2eo check
        if (lenA2eo) {
            if ( lenA2eo & 3 ) {
                /* A2EO must be 4-byte times */
                //return 9; jwsyu 20160630
            } else if (modeAuth == (uint32)-1) {
                /* A2EO not support if no AUTH enable */
                return 10;
            }
        }

        if (msglen == 0) // pkt len = 0
            return 11;
//printf("(%s-%d)\n", __func__, __LINE__);

        // Mix Mode issue
        if (modeAuth != (uint32)-1 && modeCrypto != (uint32)-1) {
            int apl, authPadSpace;
            if (rtl_random()&1) {
                //mode_select = CRYPTO_MS_TYPE_MIX2_sshdec_espenc;
            } else {
                //mode_select = CRYPTO_MS_TYPE_MIX1_sshenc_espdec;
            }

            //mode_select = CRYPTO_MS_TYPE_MIX3_sslenc;
            if (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)
                pDigest = NULL;
            // lenA2eo limit
            //if ((cntScatter == 1) ||
            //	(!rtl_ipsecEngine_sawb()))
            if (1) {

                int max_lenA2eo;

                switch (CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/) {
                    case 0: // 16 bytes
                        max_lenA2eo = 156 - 4;
                        break;
                    case 1: // 32 bytes
                        max_lenA2eo = 172 - 4; // 172 = 156 + 16
                        break;
                    case 2: // 64 bytes
                    case 3:
                        max_lenA2eo = 204 - 4; // 204 = 172 + 32
                        break;
                    default:
                        printk("unknown dma mode (%d)!\n", CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/);
                        return FAILED;
                }

                // 201303: A2EO limit depends on DMA burst: it cause encryption faileda
#ifndef CRYPTO_FORCE_MODE  //jwsyu 20150323 disable check
                if (lenA2eo > max_lenA2eo)
                    return 12;
#endif

            } else { // multi-desc && use sawb
                // 201303: A2EO limit if Multi-Desc & SAWB: it cause encryption failed
                if (lenA2eo > 136) // 136 = 152 - 16
                    return 13;
            }

            // 201303: APL lenEnl limit -> encryption ok but hmac failed
            authPadSpace = 64 - ((lenA2eo + lenEnl) & 0x3f);
            apl = authPadSpace > 8 ? authPadSpace : authPadSpace + 64;

#ifndef CRYPTO_FORCE_MODE  //jwsyu 20150323 disable check
            //if (apl == 68 && lenEnl < 64)
            //    return 14;
#endif
            // 201303: Multi-Desc lenEnl limit -> 'sometimes' timeout if multi-desc

            //if ((0/*cntScatter > 1*/) && (lenEnl <= 48))
            //    return 15; jwsyu20160630
        } else {
            if (modeAuth != (uint32)-1)
                ;//mode_select = CRYPTO_MS_TYPE_AUTH;
            else
                ;//mode_select = CRYPTO_MS_TYPE_CIPHER;

        }

        // 20121009: scatter >= 8 will be failed
        // 2013.3 scatter >= 8 issue is SAWB related, it may:
        // 	- IPS_SDLEIP or IPS_DABFI error: can be recover via re-init
        // 	- own bit failed: need hw reset
        //if (cntScatter >= 8 && rtl_ipsecEngine_sawb())
        //	return 16;

        /*---------------------------------------------------------------------
         *                                 SW ENC/Auth
         *---------------------------------------------------------------------*/
#if 0
        // scatter to continuous buffer for ipsecSim
        data_len  = 0;
        for (i = 0; i < cntScatter; i++) {
            if (scatter[i].ptr == NULL) {
                printk("%s():%d Invalid scatter pointer: %p\n",
                       __FUNCTION__, __LINE__, scatter[i].ptr);
                return FAILED;
            }

            memcpy(&sw_orig[data_len], scatter[i].ptr, scatter[i].len);
            data_len += scatter[i].len;
        }
#else
        memcpy(&_sw_orig[0], message, msglen);
#endif
        memcpy(pCryptResult, message, lenA2eo);

#ifndef DISABLE_SW_CHECK
//printf("(%s-%d)\n", __func__, __LINE__);

#if 1
        retval = ipsecSim(modeCrypto|0x80/*enc*/, modeAuth, _sw_orig, _sw_enc,
                          lenCryptoKey, ((uint8*)pCryptoKey), lenAuthKey, ((uint8*)pAuthKey),
                          ((uint8*)pIv), ((uint8*)pPad), sw_digest, lenA2eo, lenEnl, mode_select);
#else

#endif

        if (retval != SUCCESS) {
            printf("(%s-%d,ipsecSim-FAILED)\n", __func__, __LINE__);

            return FAILED; // IPSec Simulator Engine unable to encrypt is mandatory testing failed
        }
#endif
//printf("(%s-%d)\n", __func__, __LINE__);

        /*---------------------------------------------------------------------
         *                                 ASIC ENC/Auth
         *---------------------------------------------------------------------*/
#if 0
        retval = rtl_ipsecEngine(modeCrypto|4/*enc*/, modeAuth,
                                 cntScatter, scatter, pCryptResult,
                                 lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
                                 pIv, pPad, pDigest, lenA2eo, lenEnl);
#else
        retval = rtl_crypto_mix(mode_select, modeCrypto|0x80/*enc*/, modeAuth,
                                message, msglen,
                                lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
                                pIv, ivlen, lenA2eo, (void*)(((u32)pCryptResult) + lenA2eo), pDigest);
#endif
//printf("(%s-%d)\n", __func__, __LINE__);

        if (retval != SUCCESS) {
            printk("(%s-%d,rtl_crypto_mix-FAILED)\n", __FUNCTION__, __LINE__);
            return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
        }

        /*---------------------------------------------------------------------
         *                                 Compare ENC/Auth
         *---------------------------------------------------------------------*/
#ifndef DISABLE_SW_CHECK
        if (modeCrypto != (uint32) -1) {
            if (0/*sawb*/) { // use sawb, ASIC write back to scatter buffer
                //cur = _sw_enc;
                //for (i = 0; i < cntScatter; i++)
                //{
                //	if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
                //	{
                //		printk("ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                //			i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                //		if (0/*g_failed_reset*/)
                //		{
                //			// no need more debug if auto reset
                //			return FAILED;
                //		}
                //		else
                //		{
                //			memDump(cur, scatter[i].len, "Software");
                //			memDump(scatter[i].ptr, scatter[i].len, "ASIC");
                //			return FAILED;
                //		}
                //	}
                //	cur += scatter[i].len;
                //}
            } else { // not use sawb, ASIC write to continuous buffer pCryptResult
                uint32 more_compare_len=0;
                uint32 crypto_block_size;
                uint32 data_padding_len;
                if (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc) {
                    if (modeCrypto & 0x20)
                        crypto_block_size=16;
                    else
                        crypto_block_size=8;
                    data_padding_len = crypto_block_size-((lenEnl + digestlen)%crypto_block_size);
                    more_compare_len = digestlen+data_padding_len;
                }
                if (memcmp(_sw_enc + lenA2eo, (void*)(((u32)pCryptResult) + lenA2eo), lenEnl+more_compare_len) != 0) {
                    printk("ENC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                           i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                    for (temp=4 ; temp<lenA2eo; temp+=4) {
                        if (_sw_enc[lenA2eo] == pu8CryptResult[lenA2eo-temp]) {
                            if (_sw_enc[lenA2eo+1] == pu8CryptResult[lenA2eo-temp+1])
                                break;
                        }
                    }
                    printk("shift=%d", temp);
                    if (memcmp(_sw_enc + lenA2eo, (void*)(((u32)pCryptResult) + lenA2eo - temp), lenEnl) == 0) {
                        printk("recheck = OK");
                        memDump(_sw_enc + lenA2eo, lenEnl, "Software");
                        memDump((uint8_t*)(((u32)pCryptResult) + lenA2eo), lenEnl, "ASIC");
                        memDump(_sw_enc , lenA2eo, "Software-lenA2eo");
                        memDump((uint8_t*)pCryptResult , lenA2eo, "ASIC-lenA2eo");
                        if (modeAuth != (uint32) -1) {
                            memDump(sw_digest, digestlen, "Software_ICV");
                            if (mode_select != CRYPTO_MS_TYPE_MIX3_sslenc)
                                memDump((uint8_t*)pDigest, digestlen, "ASIC_ICV");
                            memDump((uint8_t*)pAuthKey, lenAuthKey, "auth key");
                        }

                    }
                    if (0/*g_failed_reset*/) {
                        // no need more debug if auto reset
                        return FAILED;
                    } else {
                        memDump(_sw_enc + lenA2eo, lenEnl, "Software");
                        memDump((uint8_t*)(((u32)pCryptResult) + lenA2eo), lenEnl, "ASIC");
                        memDump(_sw_enc , lenA2eo, "Software-lenA2eo");
                        memDump((uint8_t*)pCryptResult , lenA2eo, "ASIC-lenA2eo");
                        if (modeAuth != (uint32) -1) {
                            memDump(sw_digest, digestlen, "Software_ICV");
                            if (mode_select != CRYPTO_MS_TYPE_MIX3_sslenc)
                                memDump((uint8_t*)pDigest, digestlen, "ASIC_ICV");
                            memDump((uint8_t*)pAuthKey, lenAuthKey, "auth key");
                        }
                        rtl_cryptoEngine_info();
                        return FAILED;
                    }
                }
            }
        }

        if (modeAuth != (uint32) -1) {
            //if (memcmp(sw_digest, pDigest, modeAuth&1?20/*SHA1*/:16/*MD5*/ ) != 0)
            // CRYPTO_MS_TYPE_MIX3_sslenc no compare digest
            if (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)
                return SUCCESS;
            if (memcmp(sw_digest, pDigest, digestlen ) != 0) {
                printk("ENC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                       modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                if (0/*g_failed_reset*/) {
                    // no need more debug if auto reset
                    return FAILED;
                } else {
                    memDump(sw_digest, digestlen, "Software");
                    memDump((uint8_t*)pDigest, digestlen, "ASIC");

                    memDump(_sw_enc, lenA2eo + lenEnl, "sw_enc");
                    memDump((uint8_t*)pCryptResult, lenA2eo + lenEnl, "asic_enc");
                    memDump((uint8_t*)pAuthKey, lenAuthKey, "auth key");
                    return FAILED;
                }
            }
        }
#endif

        /* Since no crypto employed, no need to hash again. */
        if (modeCrypto == (uint32) -1)
            return SUCCESS;

        /* AES_CTR mode need not verify. */
        if (modeCrypto == 0x23/*AES_CTR*/)
            return SUCCESS;

        if (mode_select == CRYPTO_MS_TYPE_MIX3_sslenc)
            return SUCCESS;

        /*---------------------------------------------------------------------
         *                                 SW Auth/DEC
         *---------------------------------------------------------------------*/
        // Mix Mode change mode_select
        if (mode_select == CRYPTO_MS_TYPE_MIX2_sshdec_espenc) {
            mode_select = CRYPTO_MS_TYPE_MIX1_sshenc_espdec;
        } else if (mode_select == CRYPTO_MS_TYPE_MIX1_sshenc_espdec) {
            mode_select = CRYPTO_MS_TYPE_MIX2_sshdec_espenc;
        }
#ifndef DISABLE_SW_CHECK
#if 1
        retval = ipsecSim(modeCrypto/*dec*/, modeAuth, _sw_enc, _sw_dec,
                          lenCryptoKey, ((uint8*)pCryptoKey), lenAuthKey, ((uint8*)pAuthKey),
                          ((uint8*)pIv), ((uint8*)pPad), sw_digest, lenA2eo, lenEnl, mode_select);
#else
#endif

        if (retval != SUCCESS) {
            return FAILED; // IPSec Simulator Engine unable to decrypt is mandatory testing failed
        }
#endif

        /*---------------------------------------------------------------------
         *                                 ASIC Auth/DEC
         *---------------------------------------------------------------------*/

        // create de-key for ASIC
#if 0 // 20150331 neo new api using pCryptoKey for input.
        if (modeCrypto == 0x20/*AES_CBC*/ || modeCrypto == 0x22/*AES_ECB*/) {
            AES_KEY aes_key;
            static uint8 __key_sch[32 + 256/8 + 32];
            static uint8 *key_sch = &__key_sch[32];

            /* IC accept the de-key in reverse order. */
            AES_set_encrypt_key(pCryptoKey, lenCryptoKey*8, &aes_key);
            switch (lenCryptoKey) {
                case 128/8:
                    memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*10], 16);
                    break;
                case 192/8:
                    memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*12], 16);
                    memcpy((void *) UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*11+2], 8);
                    break;
                case 256/8:
                    memcpy((void *) UNCACHED_ADDRESS(key_sch), &aes_key.rd_key[4*14], 16);
                    memcpy((void *) UNCACHED_ADDRESS(&key_sch[16]), &aes_key.rd_key[4*13], 16);
                    break;
                default:
                    printk("%s():%d unknown lenCryptoKey = %d\n",
                           __FUNCTION__, __LINE__, lenCryptoKey);
                    return FAILED;
            }
            pDecryptoKey = (void *) UNCACHED_ADDRESS(key_sch);
        } else {
            pDecryptoKey = pCryptoKey;
        }
#endif
        // if sawb disabled,
        // create scatter buffer from continuous software buffer
        //if (!sawb)
        //{
        //cur = pCryptResult;
        //for (i = 0; i < cntScatter; i++)
        //{
        //	memcpy(scatter[i].ptr, cur, scatter[i].len);
        //	cur += scatter[i].len;
        //}
        //}

#if 0
        retval = rtl_ipsecEngine(modeCrypto/*dec*/, modeAuth,
                                 cntScatter, scatter, pCryptResult,
                                 lenCryptoKey, pDecryptoKey, lenAuthKey, pAuthKey,
                                 pIv, pPad, pDigest, lenA2eo, lenEnl);
#else
        retval = rtl_crypto_mix(mode_select, modeCrypto/*dec*/, modeAuth,
                                (uint8_t*)pCryptResult, msglen,
                                lenCryptoKey, pCryptoKey, lenAuthKey, pAuthKey,
                                pIv, ivlen, lenA2eo, asic_dec + lenA2eo, pDigest);

#endif

        if (retval != SUCCESS) {
            return FAILED; // IPSec Engine unable to encrypt is mandatory testing failed
        }

        /*---------------------------------------------------------------------
         *                                 Compare Auth/DEC
         *---------------------------------------------------------------------*/
#ifdef DISABLE_SW_CHECK
        if (modeAuth != (uint32) -1) {
            if (0 /*sawb*/) { // use sawb, ASIC write back to scatter buffer
                //cur = _sw_orig; // sw_orig backup orig asic input
                //for (i = 0; i < cntScatter; i++)
                //{
                //	if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
                //	{
                //		printk("DEC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                //			i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
                //		return FAILED;
                //	}
                //	cur += scatter[i].len;
                //}
            } else { // not use sawb, ASIC write to continuous buffer pCryptResult
                if (memcmp(_sw_orig + lenA2eo, ((u32)pCryptResult) + lenA2eo, lenEnl) != 0) {
                    printk("DEC DIFF! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                           modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);
                    return FAILED;
                }
            }
        }
#else
        if (modeAuth != (uint32) -1) {
            if (memcmp(sw_digest, pDigest, digestlen) != 0) {
                printk("DEC DIGEST! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                       modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                if (0/*g_failed_reset*/) {
                    // no need more debug if auto reset
                    return FAILED;
                } else {
                    memDump(sw_digest, digestlen, "Orig");
                    memDump((uint8_t*)pDigest, digestlen, "Software");
                    rtl_cryptoEngine_info();
                    return FAILED;
                }
            }
        }

        if (modeCrypto != (uint32) -1) {
            if (memcmp(_sw_orig + lenA2eo, _sw_dec + lenA2eo, lenEnl) != 0) {
                printk("DEC DIFF-2! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                       modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                if (0/*g_failed_reset*/) {
                    // no need more debug if auto reset
                    return FAILED;
                } else {
                    memDump(_sw_orig + lenA2eo, lenEnl, "Orig");
                    memDump(_sw_dec + lenA2eo, lenEnl, "Software");
                    return FAILED;
                }
            }

            if (0/*sawb*/) { // use sawb, ASIC write back to scatter buffer
                //cur = _sw_dec;
#if 1

#else
                //for (i = 0; i < cntScatter; i++)
                //{
                //	if (memcmp(cur, scatter[i].ptr, scatter[i].len) != 0)
                //	{
                //		printk("DEC DIFF![%d]crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                //			i, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                //		if (0/*g_failed_reset*/)
                //		{
                //			// no need more debug if auto reset
                //			return FAILED;
                //		}
                //		else
                //		{
                //			memDump(cur, scatter[i].len, "Software");
                //			memDump(scatter[i].ptr, scatter[i].len, "ASIC");
                //			return FAILED;
                //		}
                //	}
                //	cur += scatter[i].len;
                //}
#endif
            } else { // not use sawb, ASIC write to continuous buffer asic_dec
                if (memcmp(_sw_dec + lenA2eo, asic_dec + lenA2eo, lenEnl) != 0) {
                    printk("DEC DIFF-4! crypt:%x auth:%x CKey:%d AKey:%d lenA2eo:%d\n",
                           modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo);

                    if (0/*g_failed_reset*/) {
                        // no need more debug if auto reset
                        return FAILED;
                    } else {
                        memDump(_sw_dec + lenA2eo, lenEnl, "Software");
                        memDump(asic_dec + lenA2eo, lenEnl, "ASIC");
                        return FAILED;
                    }
                }
            }
        }
#endif

        return SUCCESS;
    }

// 3DES - ECB : IP_SEC vector 0 18 ( 18 = 0x12)

    __attribute__ ((aligned (4)))
    static const unsigned char des3_test_keys[24] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
        0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
    };



    __attribute__ ((aligned (4)))
    static const unsigned char des3_test_iv[8] = {
        0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
    };


    static const unsigned char des3_test_buf[64] = {
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x21, 0x21,
    };
//	"Hello!!!"

// 3DES-SHA MtE

    __attribute__ ((aligned (4)))
    static const unsigned char des3_test_buf_encoded[8] = {
        0x8a, 0x2f, 0xe9, 0xb3, 0x4a, 0x4c, 0x2f, 0x47,
    };


// http://en.wikipedia.org/wiki/MD5
// MD5("") = d41d8cd98f00b204e9800998ecf8427e
//char *_md5_plaintext = "";
//char *_md5_digest =  "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04"
//					 "\xe9\x80\x09\x98\xec\xf8\x42\x7e";

// MD5("The quick brown fox jumps over the lazy dog") =
//  9e107d9d372bb6826bd81d3542a419d6

//__attribute__ ((aligned (4)))
    static const u8 _md5_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
    static const u8 _md5_digest[] 		=  "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82"
                                           "\x6b\xd8\x1d\x35\x42\xa4\x19\xd6";


// http://en.wikipedia.org/wiki/SHA-1
// SHA1("") = da39a3ee5e6b4b0d3255bfef95601890afd80709
// char *_sha1_plaintext = "";
// char *_sha1_digest =  "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d"
//					  "\x32\x55\xbf\xef\x95\x60\x18\x90"
//					  "\xaf\xd8\x07\x09";

// SHA1("The quick brown fox jumps over the lazy dog")
//   = 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12

//__attribute__ ((aligned (4)))
    static const u8 _sha1_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
    static const u8 _sha1_digest[] 	=  "\x2f\xd4\xe1\xc6\x7a\x2d\x28\xfc"
                                       "\xed\x84\x9e\xe1\xbb\x76\xe7\x39"
                                       "\x1b\x93\xeb\x12";

//char *_hmac_md5_key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
//char *_hmac_md5_plaintext = "Hi There";
//char *_hmac_md5_digest =  "\x92\x94\x72\x7a\x36\x38\xbb\x1c"
//            "\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d";


// http://en.wikipedia.org/wiki/Hash-based_message_authentication_code
// HMAC_MD5("", "") = 0x74e6f7298a9c2d168935f58c001bad88
//char *_hmac_md5_key = "";
//char *_hmac_md5_plaintext = "";
//char *_hmac_md5_digest =  "\x74\xe6\xf7\x29\x8a\x9c\x2d\x16"
//    "\x89\x35\xf5\x8c\x00\x1b\xad\x88";

// HMAC_MD5("key", "The quick brown fox jumps over the lazy dog") = 0x80070713463e7749b90c2dc24911e275


    __attribute__ ((aligned (4)))
    static const u8 _hmac_md5_key[] 		= "key";


    static const u8 _hmac_md5_plaintext[] 	= "The quick brown fox jumps over the lazy dog";


    static const u8 _hmac_md5_digest[] 	=  "\x80\x07\x07\x13\x46\x3e\x77\x49"
                                           "\xb9\x0c\x2d\xc2\x49\x11\xe2\x75";


// SHA-1 : 160 bit : 20 bytes
//
// http://en.wikipedia.org/wiki/Hash-based_message_authentication_code
// HMAC_SHA1("", "") = 0xfbdb1d1b18aa6c08324b7d64b71fb76370690e1d
//char *_hmac_sha1_key = "";
//char *_hmac_sha1_plaintext = "";
//char *_hmac_sha1_digest =  "\xfb\xdb\x1d\x1b\x18\xaa\x6c\x08"
//    "\x32\x4b\x7d\x64\xb7\x1f\xb7\x63"
//    "\x70\x69\x0e\x1d";

// HMAC_SHA1("key", "The quick brown fox jumps over the lazy dog") = 0xde7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9


    __attribute__ ((aligned (4)))
    static const u8 _hmac_sha1_key[] = "key";


//__attribute__ ((aligned (4)))
    static const u8 _hmac_sha1_plaintext[] = "The quick brown fox jumps over the lazy dog";


//__attribute__ ((aligned (4)))
    static const u8 _hmac_sha1_digest[] =  "\xde\x7c\x9b\x85\xb8\xb7\x8a\xa6"
                                           "\xbc\x8a\x7a\x36\xf7\x0a\x90\x70"
                                           "\x1c\x9d\xb4\xd9";


// SHA-2 : 224 bit: 28 bytes;
//
// http://en.wikipedia.org/wiki/SHA-2
//
// SHA224("The quick brown fox jumps over the lazy dog") =
//   0x 730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525


//__attribute__ ((aligned (4)))
    static const u8 _sha2_plaintext[] = "The quick brown fox jumps over the lazy dog";
//char *_sha2_digest =  "\x73\x0e\x10\x9b\xd7\xa8\xa3\x2b"
//					   "\x1c\xb9\xd9\xa0\x9a\xa2\x32\x5d"
//					   "\x24\x30\x58\x7d\xdb\xc0\xc3\x8b"
//					   "\xad\x91\x15\x25";

// SHA256("The quick brown fox jumps over the lazy dog") =
//    0x d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592


//__attribute__ ((aligned (4)))
    static const u8 _sha2_digest[] =  "\xd7\xa8\xfb\xb3\x07\xd7\x80\x94"
                                      "\x69\xca\x9a\xbc\xb0\x08\x2e\x4f"
                                      "\x8d\x56\x51\xe4\x6d\x3c\xdb\x76"
                                      "\x2d\x02\xd0\xbf\x37\xc9\xe5\x92";



// SHA384("The quick brown fox jumps over the lazy dog")
//   0x ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1

//char *_sha2_digest =  "\xca\x73\x7f\x10\x14\xa4\x8f\x4c"
//						"\x0b\x6d\xd4\x3c\xb1\x77\xb0\xaf"
//						"\xd9\xe5\x16\x93\x67\x54\x4c\x49"
//						"\x40\x11\xe3\x31\x7d\xbf\x9a\x50"
//						"\x9c\xb1\xe5\xdc\x1e\x85\xa9\x41"
//						"\xbb\xee\x3d\x7f\x2a\xfb\xc9\xb1";

// SHA512("The quick brown fox jumps over the lazy dog") =
//   0x 07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6

//char *_sha2_digest =
//	"\x07\xe5\x47\xd9\x58\x6f\x6a\x73"
//	"\xf7\x3f\xba\xc0\x43\x5e\xd7\x69"
//	"\x51\x21\x8f\xb7\xd0\xc8\xd7\x88"
//	"\xa3\x09\xd7\x85\x43\x6b\xbb\x64"
//	"\x2e\x93\xa2\x52\xa9\x54\xf2\x39"
//	"\x12\x54\x7d\x1e\x8a\x3b\x5e\xd6"
//	"\xe1\xbf\xd7\x09\x78\x21\x23\x3f"
//	"\xa0\x53\x8f\x3d\xb8\x54\xfe\xe6";


//
// rfc4231 case 1
// https://tools.ietf.org/html/rfc4231
//
// Key =          0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
//                  0b0b0b0b                          (20 bytes)
//   Data =         4869205468657265                  ("Hi There")
//
//   HMAC-SHA-224 = 896fb1128abbdf196832107cd49df33f
//                              47b4b1169912ba4f53684b22
//   HMAC-SHA-256 = b0344c61d8db38535ca8afceaf0bf12b
//                              881dc200c9833da726e9376c2e32cff7





    __attribute__ ((aligned (4)))
    static const u8 _hmac_sha2_key[] 		= "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
            "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
            "\x0b\x0b\x0b\x0b";


//__attribute__ ((aligned (4)))
    static const u8 _hmac_sha2_plaintext[] 	= "Hi There";


//__attribute__ ((aligned (4)))
    static const u8 _hmac_sha224_digest[] 	=  "\x89\x6f\xb1\x12\x8a\xbb\xdf\x19"
            "\x68\x32\x10\x7c\xd4\x9d\xf3\x3f"
            "\x47\xb4\xb1\x16\x99\x12\xba\x4f"
            "\x53\x68\x4b\x22";


    static const u8 _hmac_sha256_digest[] 	=  "\xb0\x34\x4c\x61\xd8\xdb\x38\x53"
            "\x5c\xa8\xaf\xce\xaf\x0b\xf1\x2b"
            "\x88\x1d\xc2\x00\xc9\x83\x3d\xa7"
            "\x26\xe9\x37\x6c\x2e\x32\xcf\xf7";


//
// http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06
// Poly1305 vector : Test vector #2


    __attribute__ ((aligned (4)))
    static const u8 poly1305_key[32] 		= {
        0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a,
        0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
        0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09,
        0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0
    };

//__attribute__ ((aligned (4)))
    static const u8 poly1305_plaintext[] 	=
        "'Twas brillig, and the slithy toves\n"
        "Did gyre and gimble in the wabe:\n"
        "All mimsy were the borogoves,\n"
        "And the mome raths outgrabe.";


//__attribute__ ((aligned (4)))
    static const u8 poly1305_digest[] 		= {
        0x45, 0x41, 0x66, 0x9a, 0x7e, 0xaa, 0xee, 0x61,
        0xe7, 0x08, 0xdc, 0x7c, 0xbc, 0xc5, 0xeb, 0x62
    };



    static const u8 crc_plaintext[] = "The quick brown fox jumps over the lazy dog";
    static const u8 crc32_digest[] =
    { 0x39, 0xa3, 0x4f, 0x41 };
    static const u8 crc_ccitt_digest[] =
    { 0xDD, 0x8F };
    static const u8 crc16_digest[] =
    { 0xDF, 0xFC };


//
// authtype : 0: SHA1, 1: SHA2, 2: MD5; 4: HMAC-SHA1, 5: HMAC-SHA2, 6: HMAC-MD5
//

    int vector_test_auth(int authtype)
    {
        int ret;
        const u8* key;
        u32 keylen;
        const u8 *plaintext;
        u32 textlen;
        u8 *message;
        const u8 *digest;
        u8 *pAsicDigest = &_AsicDigest[33];
        int digestlen = 0;


        // force not 4-byte alignment
        message = &_asic_orig[1];

        switch (authtype) {
            case 1: // MD5
                key = NULL;
                keylen = 0;
                rtl_memcpy(message, _md5_plaintext, sizeof(_md5_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _md5_digest;
                digestlen = 16;
                ret = rtl_crypto_md5(plaintext, textlen, pAsicDigest);
                break;

            case 2: // SHA1
                key = NULL;
                keylen = 0;
                rtl_memcpy(message, _sha1_plaintext, sizeof(_sha1_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _sha1_digest;
                digestlen = 20;
                ret = rtl_crypto_sha1(plaintext, textlen, pAsicDigest);
                break;

            case 3: // SHA2
                key = NULL;
                keylen = 0;
                rtl_memcpy(message, _sha2_plaintext, sizeof(_sha2_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _sha2_digest;
                digestlen = 32;
                ret = rtl_crypto_sha2(SHA2_256, plaintext, textlen, pAsicDigest);
                break;

            case 4: // HMAC-MD5
                key = _hmac_md5_key;
                keylen = strlen(key);
                rtl_memcpy(message, _hmac_md5_plaintext, sizeof(_hmac_md5_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _hmac_md5_digest;
                digestlen = 16;
                ret = rtl_crypto_hmac_md5(plaintext, textlen, key, keylen, pAsicDigest);
                break;


            case 5: // HMAC-SHA1
                key = _hmac_sha1_key;
                keylen = strlen(_hmac_sha1_key);
                rtl_memcpy(message, _hmac_sha1_plaintext, sizeof(_hmac_sha1_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _hmac_sha1_digest;
                digestlen = 20;
                ret = rtl_crypto_hmac_sha1(plaintext, textlen, key, keylen, pAsicDigest);
                break;

            case 6: // HMAC-SHA2
                key = _hmac_sha2_key;
                keylen = strlen(key);
                rtl_memcpy(message, _hmac_sha2_plaintext, sizeof(_hmac_sha2_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = _hmac_sha224_digest;
                digestlen = 28;
                ret = rtl_crypto_hmac_sha2(SHA2_224, plaintext, textlen, key, keylen, pAsicDigest);
                //digest = _hmac_sha256_digest;
                //digestlen = 32;
                //ret = rtl_crypto_hmac_sha2(SHA2_256, plaintext, textlen, key, keylen, pAsicDigest);
                break;


            case 7:
                key = poly1305_key;
                keylen = sizeof(poly1305_key);
                rtl_memcpy(message, poly1305_plaintext, sizeof(poly1305_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                digest = poly1305_digest;
                digestlen = 16;
                ret = rtl_crypto_poly1305(plaintext, textlen, key, keylen, pAsicDigest);
                break;

            case 8: // crc32
                key = NULL;
                keylen = 0;
                rtl_memcpy(message, crc_plaintext, sizeof(crc_plaintext));
                plaintext = message;
                textlen = strlen(plaintext);
                //digest = crc32_digest;
                //digestlen = 4;
                //digest = crc_ccitt_digest;
                digest = crc16_digest;
                digestlen = 2;
                //ret = rtl_crypto_crc32(plaintext, textlen, (u32*)pAsicDigest);
                //ret = rtl_crypto_crc_setting(16, 0x1021, 1, 0xFFFF, 0, 0, 0); // CCITT
                ret = rtl_crypto_crc_setting(16, 0x8005, 1, 0, 0, 1, 1);
                if ( ret != SUCCESS ) {
                    printk("crypto crc setting failed, ret=%d\n", ret);
                    return ret;
                }
                ret = rtl_crypto_crc(plaintext, textlen, (u32*)pAsicDigest);

                // for setting software crc32
                //crc_tester_setting(32, 0x4c11db7, 1, 0xffffffff, 0xffffffff, 1, 1);
                // for setting software crc-CCITT
                //crc_tester_setting(16, 0x1021, 1, 0xFFFF, 0, 0, 0);
                crc_tester_setting(16, 0x8005, 1, 0, 0, 1, 1);
                break;

            default:
                err_printk("authtype(%d) : no this type\n", authtype);
                return FAIL;
        }

        if ( ret != SUCCESS ) {
            printk("crypto digest failed, ret=%d\n", ret);
            return ret;
        }

        rtl_crypto_auth_sim(authtype, key, keylen, plaintext, textlen, sw_digest, digestlen);
        dbg_mem_dump(sw_digest, digestlen, "sw digest");

        if ( rtl_memcmpb(sw_digest, digest, digestlen) == 0) {
            printk("sw_digest is correct\n");
        } else {
            printk("sw_digest is WRONG\n");
        }


        if ( rtl_memcmpb(pAsicDigest, digest, digestlen) == 0) {
            dbg_mem_dump(pAsicDigest, digestlen, "test OK - digest:");
            return SUCCESS;
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(key, keylen, "KEY:");
            dbg_mem_dump(plaintext, textlen, "TEXT:");
            dbg_mem_dump(pAsicDigest, digestlen, "asic digest");
            dbg_mem_dump(sw_digest, digestlen, "sw digest");
            dbg_mem_dump(digest, digestlen, "correct digest");
        }

        return FAIL;
    }


//static u8 rand_pos_record[32];
    int vector_test_auth_rand(void)
    {

        int ret = SUCCESS;

        u8 *pAsicDigest;
        u8 dig_pos;

        u32 authtype;
        u32 keylen;
        u8 *key;
        u32 msglen, msglen1, msglen2, msglen3;
        u8 *message;
        int i;

        u32 digestlen;
        u32 msg_pos;

        this_rtlseed = rtl_random();
        //this_rtlseed = 0xd0f85a91;
        //rtl_srandom(this_rtlseed);



        // 1:MD5, 2 : SHA1, 3: SHA2, 4:HMAC-MD5, 5:HMAC-SHA1, 6: HMAC-SHA2, 7: poly-1305, 8: crc
        authtype = (rtl_random() % 8 ) +1;
        //authtype=8;

        // Suppose keylen = 0~ BLOCK(64)
        if ( (authtype < 4) || (authtype==8) ) { // no HMAC
            keylen = 0;
            key = NULL;
        } else {
            keylen = (authtype == 7) ? 32 : ( rtl_random() % 32)+1;
            key = &_asic_authkey[0];
            for (i=0; i<keylen; i++)
                key[i] = rtl_random() & 0xFF;
        }


        // Suppose msglen = 1~16000 , 2 descriptors
        msglen1 = ( rtl_random()%(CRYPTO_MAX_MSG_LENGTH/64) + 1 )*64;
        msglen2 = ( rtl_random()%(CRYPTO_MAX_MSG_LENGTH/64) + 1 )*64;
        msglen3 = rtl_random() % CRYPTO_MAX_MSG_LENGTH;
        msglen = msglen1 + msglen2 + msglen3;

        if ( authtype == 8 )
            msglen = rtl_random() % CRYPTO_MAX_MSG_LENGTH + 1;

        msg_pos = rtl_random() % 32;
        message = &_asic_orig[msg_pos];
        //rand_pos_record[msg_pos]++;

        for (i=0; i<msglen; i++)
            message[i] = rtl_random() & 0xFF;

        dig_pos = rtl_random() % 32;
        pAsicDigest = &_AsicDigest[dig_pos];
        rtl_memset(pAsicDigest, 0, 32);

        //printk("authtype=%d\n", authtype);


        switch (authtype) {
            case 1:
                digestlen = CRYPTO_MD5_DIGEST_LENGTH;

                ret = rtl_crypto_md5_init();
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_md5_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_md5_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_md5_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_md5_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                break;

            case 2:
                digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
                ret = rtl_crypto_sha1_init();
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_sha1_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha1_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha1_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha1_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-5!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                break;

            case 3:
                digestlen = (rtl_random()%2)?SHA2_256:SHA2_224;
                ret = rtl_crypto_sha2_init(digestlen);
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_sha2_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha2_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha2_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_sha2_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-5!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                break;

            case 4:
                digestlen = CRYPTO_MD5_DIGEST_LENGTH;

                ret = rtl_crypto_hmac_md5_init(key, keylen);
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_md5_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_md5_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_md5_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_hmac_md5_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                break;
            case 5:
                digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
                ret = rtl_crypto_hmac_sha1_init(key, keylen);
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_sha1_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_hmac_sha1_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_hmac_sha1_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_hmac_sha1_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-5!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                break;
            case 6:
                //digestlen = (rtl_random()%2)?SHA2_256:SHA2_224;
                //digestlen = SHA2_256;
                digestlen = SHA2_224;
                ret = rtl_crypto_hmac_sha2_init(digestlen, key, keylen);
                if (ret != SUCCESS) {
                    err_printk("failed-1!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_sha2_update(message,msglen1);
                if (ret != SUCCESS) {
                    err_printk("failed-2!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_sha2_update(message+msglen1,msglen2);
                if (ret != SUCCESS) {
                    err_printk("failed-3!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }

                ret = rtl_crypto_hmac_sha2_update(message+msglen1+msglen2,msglen3);
                if (ret != SUCCESS) {
                    err_printk("failed-4!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                ret = rtl_crypto_hmac_sha2_final(pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("failed-5!, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                break;
            case 7 :
                //poly1305
                ret = rtl_crypto_poly1305(message, msglen, key, keylen, pAsicDigest);
                if (ret != SUCCESS) {
                    err_printk("poly1305 failed, ret=%d\n", ret);
                    goto ret_vector_test_auth_rand;
                }
                break;

            case 8: {
                int order = 16;
                unsigned long polynom = 0x1021;
                unsigned long crcinit = 0xFFFF;
                unsigned long crcxor = 0;
                u8 refin = 0;
                u8 refout = 0;
                int choice;
                u32 mask;

                choice = rtl_random() % 8;
                polynom = rtl_random();
                crcinit = rtl_random();
                crcxor = rtl_random();
                refin = rtl_random() % 2;
                refout = rtl_random() % 2;
                switch (choice) {
                    case 0: // crc32
                        order = 32;
                        mask = 0xFFFFFFFF;
                        break;
                    case 1: // crc24
                        order = 24;
                        mask = 0x00FFFFFF;
                        break;
                    case 2: // crc16
                        order = 16;
                        mask = 0xFFFF;
                        break;
                    case 3: //. crc12
                        order = 12;
                        mask = 0xFFF;
                        break;
                    case 4: // crc10
                        order = 10;
                        mask = 0x3FF;
                        break;
                    case 5: // crc8
                        order = 8;
                        mask = 0xFF;
                        break;
                    case 6: //crc7
                        order = 7;
                        mask = 0x7F;
                        break;
                    case 7: //crc5;
                        order = 5;
                        mask = 0x1F;
                        break;
                }

                polynom &= mask;
                crcinit &= mask;
                crcxor &= mask;

                ret = rtl_crypto_crc_setting(order, polynom, 1, crcinit, crcxor, refin, refout);
                if ( ret != SUCCESS ) {
                    printk("crypto crc setting failed, ret=%d\n", ret);
                    return ret;
                }
                ret = rtl_crypto_crc(message, msglen, (u32*)pAsicDigest);

                // for setting software crc32
                //crc_tester_setting(32, 0x4c11db7, 1, 0xffffffff, 0xffffffff, 1, 1);
                crc_tester_setting(order, polynom, 1, crcinit, crcxor, refin, refout);
            }
            break;

            default:
                err_printk("authtype(%d) : no this type\n", authtype);
                ret = FAIL;
                goto ret_vector_test_auth_rand;
        }



        rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, sw_digest, digestlen);

        if ( rtl_memcmpb(pAsicDigest, sw_digest, digestlen) == 0) {
            dbg_printk("IPSec ok\n");
            ret = SUCCESS;
        } else {
            ret = FAIL;
        }

ret_vector_test_auth_rand:

        if ( ret == FAIL ) {
            printk("IPSec failed!\n");
            ("rtl_seed = 0x%x\n", this_rtlseed);
            DiagPrintf("auth_type = 0x%x\n", authtype);
            DiagPrintf("keylen = %d\n", keylen);
            DiagPrintf("msglen1 = %d\n", msglen1);
            DiagPrintf("msglen2 = %d\n", msglen2);
            DiagPrintf("msglen3 = %d\n", msglen3);
            DiagPrintf("msglen = %d\n", msglen);
            DiagPrintf("msg_pos = %d\n", msg_pos);
            DiagPrintf("dig_pos = %d\n", dig_pos);
            DiagPrintf("digestlen = %d\n", digestlen);

            memDump(key, keylen, "key: ");
            memDump(message, msglen, "message: ");
            memDump(pAsicDigest, digestlen, "asic digest: ");
            memDump(sw_digest, digestlen, "sw digest: ");
        }

        return ret;
    }





    __attribute__ ((aligned (4)))
    static const unsigned char chacha_test_key[32] = {
        0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a,
        0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
        0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09,
        0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0
    };

    static const unsigned char chacha_test_buf[] =
        "'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.";

    static const unsigned char chacha_test_iv[12] = {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00,
        0x0, 0x0, 0x0, 0x02
    };

    static const int chacha_init_count=42;

    static const unsigned char chacha_test_res[] = {

        0x62, 0xe6, 0x34, 0x7f, 0x95, 0xed, 0x87, 0xa4,
        0x5f, 0xfa, 0xe7, 0x42, 0x6f, 0x27, 0xa1, 0xdf,
        0x5f, 0xb6, 0x91, 0x10, 0x04, 0x4c, 0x0d, 0x73,
        0x11, 0x8e, 0xff, 0xa9, 0x5b, 0x01, 0xe5, 0xcf,
        0x16, 0x6d, 0x3d, 0xf2, 0xd7, 0x21, 0xca, 0xf9,
        0xb2, 0x1e, 0x5f, 0xb1, 0x4c, 0x61, 0x68, 0x71,
        0xfd, 0x84, 0xc5, 0x4f, 0x9d, 0x65, 0xb2, 0x83,
        0x19, 0x6c, 0x7f, 0xe4, 0xf6, 0x05, 0x53, 0xeb,
        0xf3, 0x9c, 0x64, 0x02, 0xc4, 0x22, 0x34, 0xe3,
        0x2a, 0x35, 0x6b, 0x3e, 0x76, 0x43, 0x12, 0xa6,
        0x1a, 0x55, 0x32, 0x05, 0x57, 0x16, 0xea, 0xd6,
        0x96, 0x25, 0x68, 0xf8, 0x7d, 0x3f, 0x3f, 0x77,
        0x04, 0xc6, 0xa8, 0xd1, 0xbc, 0xd1, 0xbf, 0x4d,
        0x50, 0xd6, 0x15, 0x4b, 0x6d, 0xa7, 0x31, 0xb1,
        0x87, 0xb5, 0x8d, 0xfd, 0x72, 0x8a, 0xfa, 0x36,
        0x75, 0x7a, 0x79, 0x7a, 0xc1, 0x88, 0xd1

    };


    int vector_test_cipher_rand(void)
    {
        int ret = FAIL;

        u32 cipher_type;
        u32 keylen, ivlen, aadLen;
        u8 *key, *pIv, *pAAD;
        u32 msglen;
        u8 *message;

        int i;

        u8 *pResult_asic;
        u8 *pResult_sw;

        u8 *pTag_asic;
        u8 *pTag_sw;

        u32 enl;

        int type;
        u32 msg_pos;
        u32 result_pos;

        u32 init_count;



        //
        this_rtlseed = rtl_random();
        //this_rtlseed = 0xebe007e5;
        //rtl_srandom(this_rtlseed);

        type = rtl_random() % 11;
        //type = 10;

        //
        pAAD = NULL;
        aadLen = 0;

        //
        init_count = 0;

        switch (type) {
            case 0:
                cipher_type = CIPHER_TYPE_DES_CBC; // DES-CBC
                keylen = 8;
                ivlen = keylen;
                break;
            case 1:
                cipher_type = CIPHER_TYPE_DES_ECB; // DES-ECB
                keylen = 8;
                ivlen = keylen;
                break;
            case 2:
                cipher_type = CIPHER_TYPE_3DES_CBC; // 3DES-CBC
                keylen = 24;
                ivlen = 8;
                break;
            case 3:
                cipher_type = CIPHER_TYPE_3DES_ECB; // 3DES-ECB
                keylen = 24;
                ivlen  = 8;
                break;
            case 4:
                cipher_type = CIPHER_TYPE_AES_CBC; // AES-CBC, 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 16;
                break;
            case 5:
                cipher_type = CIPHER_TYPE_AES_ECB; // AES-ECB, 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 0;
                break;
            case 6:
                cipher_type = CIPHER_TYPE_AES_CTR; // AES-CTR 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 16;
                break;
            case 7:
                cipher_type = CIPHER_TYPE_AES_CFB; // AES-CFB 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 16;
                break;
            case 8:
                cipher_type = CIPHER_TYPE_AES_OFB; // AES-OFB 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 16;
                break;
            case 9:
                cipher_type = CIPHER_TYPE_AES_GCM; // AES-OFB 128, 192, 256
                keylen = ((rtl_random() %3)+2)*8;
                ivlen = 12;
                pAAD = _asic_aad;
                aadLen = rtl_random()%64;
                break;
            case 10:
                cipher_type = CIPHER_TYPE_CHACHA; // CHACHA
                keylen = 32;
                ivlen = 12;
                init_count = rtl_random()%32768;
                break;

            default:
                err_printk("no this cipher type\n");
                return ret;
        }

        //printk("cipher_type=0x%x\n", cipher_type);


        key = &_asic_cryptkey[0];

        for (i=0; i<keylen; i++)
            key[i] = rtl_random() & 0xFF;
        key[keylen] = '\0';

        // chacha test
        //memcpy(key, chacha_test_key1, 32);

        pIv = _asic_iv;
        for (i=0; i<ivlen; i++)
            pIv[i] = rtl_random() & 0xFF;

        // chacha test only
        //memcpy(pIv, chacha_test_iv1, 12);

        if ( pAAD != NULL ) {
            for (i=0; i<aadLen; i++)
                pAAD[i] = rtl_random() & 0xFF;
        }

        // Suppose msglen = 1~16383*3
        if ( (cipher_type != CIPHER_TYPE_AES_GCM) && (cipher_type != CIPHER_TYPE_CHACHA) ) {
            msglen =  ((rtl_random() % ((CRYPTO_MAX_MSG_LENGTH-16)/16))+1)*3*16;
        } else {
            msglen = (rtl_random() % CRYPTO_MAX_MSG_LENGTH ) + 1;
            //chacha test only
            //msglen = strlen(chacha_test_buf1);
        }

        msg_pos = rtl_random() % 32;
        message = &_asic_orig[msg_pos];

        for (i=0; i<msglen; i++)
            message[i] = rtl_random() & 0xFF;
        message[msglen] = '\0';
        //memcpy(message, chacha_test_buf1, msglen+1);

        for (i=0; i<32; i++)
            message[msglen+i] = 0xA0;


        result_pos = rtl_random() % 32;
        pResult_asic = &_asic_enc[result_pos];
        pResult_sw	 = _sw_enc;

        pTag_asic = _AsicDigest;
        pTag_sw = sw_digest;

        enl = msglen;

        switch ( cipher_type) {
            case CIPHER_TYPE_DES_CBC: // DES-CBC
                ret = rtl_crypto_des_cbc_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_DES_ECB: // DES-ECB
                ret = rtl_crypto_des_ecb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
                ret = rtl_crypto_3des_cbc_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_3DES_ECB: // 3DES-ECB
                ret = rtl_crypto_3des_ecb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CBC: // AES-CBC
                ret = rtl_crypto_aes_cbc_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_ECB: // AES-ECB
                ret = rtl_crypto_aes_ecb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CTR: // AES-CTR
                ret = rtl_crypto_aes_ctr_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CFB: // AES-CFB
                ret = rtl_crypto_aes_cfb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_cfb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_OFB: // AES-CFB
                ret = rtl_crypto_aes_ofb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_ofb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_GCM: // AES-GCM
                ret = rtl_crypto_aes_gcm_init(key,keylen);
                if ( ret != SUCCESS ) return ret;

                ret = rtl_crypto_aes_gcm_encrypt(message, msglen, pIv, pAAD, aadLen, pResult_asic, pTag_asic);
                break;

            case CIPHER_TYPE_CHACHA: // chacha
                ret = rtl_crypto_chacha_init(key);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_chacha_encrypt(message, msglen, pIv, init_count, pResult_asic);
                break;

            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, init_count, pAAD, aadLen, message, msglen, pResult_sw, pTag_sw);
        if (ret != SUCCESS) {
            err_printk("ipsecSim encrypt failed!\n");
            memDump(pResult_asic, msglen, "ASIC enc msg: ");
            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
            if ( cipher_type == CIPHER_TYPE_AES_GCM ) {
                if ( rtl_memcmpb(pTag_asic, pTag_sw, 16) == 0 ) {
                    dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
                } else {
                    err_printk("test failed - TAG!\n");
                    memDump(pTag_asic, 16, "ASIC enc TAG: ");
                    memDump(pTag_sw, 16, "SW enc TAG: ");
                    ret = FAIL;
                    goto ret_vector_test_esp_rand;
                }
            }
        } else {
            err_printk("test failed!\n");
            memDump(pResult_asic, msglen, "ASIC enc msg: ");
            memDump(pResult_sw, msglen, "SW enc msg: ");
            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        // issue
        if ( message[msglen+1] != 0xA0 ) {
            err_printk("test failed!, message overwrite\n");
            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        // Decryption

        message = pResult_asic;
        for (i=0; i<32; i++)
            message[msglen+i] = 0xA0;


        result_pos = rtl_random() % 32;
        pResult_asic = &_asic_dec[result_pos];

        memset(pResult_asic, 0x0, 128);

        pResult_sw	= _sw_dec;

        switch ( cipher_type) {
            case CIPHER_TYPE_DES_CBC: // DES-CBC
                ret = rtl_crypto_des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_DES_ECB: // DES-ECB
                ret = rtl_crypto_des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
                ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_3DES_ECB: // 3DES-ECB
                ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CBC: // AES-CBC
                ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_ECB: // AES-ECB
                ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CTR: // AES-CTR
                ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CFB: // AES-CFB
                ret = rtl_crypto_aes_cfb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_OFB: // AES-OFB
                ret = rtl_crypto_aes_ofb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_GCM: // AES-GCM
                ret = rtl_crypto_aes_gcm_decrypt(message, msglen, pIv, pAAD, aadLen, pResult_asic, pTag_asic);
                break;

            case CIPHER_TYPE_CHACHA: // chacha
                ret = rtl_crypto_chacha_decrypt(message, msglen, pIv, init_count, pResult_asic);
                break;

            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec decrypt failed!\n");
            goto ret_vector_test_esp_rand;
        }


        ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, init_count, pAAD, aadLen, message, msglen, pResult_sw, pTag_sw);
        if (ret != SUCCESS) {
            err_printk("ipsecSim decrypt failed!\n");
            memDump(pResult_asic, msglen, "ASIC dec msg: ");
            goto ret_vector_test_esp_rand;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            if ( cipher_type == CIPHER_TYPE_AES_GCM ) {
                if ( rtl_memcmpb(pTag_asic, pTag_sw, 16) == 0 ) {
                    dbg_mem_dump(pResult_asic, msglen, "test OK - dec: Tag");
                } else {
                    err_printk("test failed - TAG!\n");
                    memDump(pTag_asic, 16, "ASIC enc TAG: ");
                    memDump(pTag_sw, 16, "SW enc TAG: ");
                    ret = FAIL;
                    goto ret_vector_test_esp_rand;
                }
            }
            ret = SUCCESS;
        } else {
            err_printk("test failed!\n");

            memDump(pResult_asic, msglen, "ASIC dec msg: ");

            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        // issue
        if ( message[msglen+1] != 0xA0 ) {
            err_printk("test failed!, message overwrite - decrypt\n");
            ret = FAIL;
            goto ret_vector_test_esp_rand;
        }

        return SUCCESS;

ret_vector_test_esp_rand:

        DiagPrintf("rtl_seed = 0x%x\n", this_rtlseed);
        DiagPrintf("type = %d\n", type);
        err_printk("cipher_type=0x%x\n", cipher_type);
        err_printk("msg_pos=%d\n", msg_pos);
        err_printk("result_pos=%d\n", result_pos);

        err_printk("message (%d) : \n", msglen);
        memDump(message, msglen, "MSG:");


        err_printk("key (%d) : \n", keylen);
        memDump(key, keylen, "key:");

        err_printk("IV (%d) : \n", ivlen);
        memDump(pIv, ivlen, "IV:");

        err_printk("AAD (%d) : \n", aadLen);
        memDump(pAAD, aadLen, "AAD:");

        err_printk("init_count (%d) : \n", init_count);

        return ret;

    }
    /****************************************
     * By 'crypto mix rand' command
     *   random key/iv/mode/len/offset/data
     ****************************************/
    int32 modeCrypto_val[8] = {-1, 0x00, 0x01, 0x10, 0x11, 0x20, 0x21, 0x24};
    //CIPHER_TYPE_DES_ECB, CIPHER_TYPE_DES_CBC, CIPHER_TYPE_3DES_ECB, CIPHER_TYPE_3DES_CBC
    //CIPHER_TYPE_AES_ECB, CIPHER_TYPE_AES_CBC, CIPHER_TYPE_AES_CTR
    int32 modeAuth_val[9] = {-1, 2, 0, 0x11, 0x21, 0x6, 0x4, 0x15, 0x25};
    //AUTH_TYPE_MD5, AUTH_TYPE_SHA1, AUTH_TYPE_SHA2, AUTH_TYPE_HMAC_MD5
    //AUTH_TYPE_HMAC_SHA1, AUTH_TYPE_HMAC_SHA2,
    int32 CryptoKey_val[4] = {8, 16, 24, 32};
#define LEN_CRTPTO_VALS (sizeof(modeCrypto_val)/sizeof(modeCrypto_val[0]))
#define LEN_AUTH_VALS (sizeof(modeAuth_val)/sizeof(modeAuth_val[0]))
#define LEN_CRYPTOKEY_VALS (sizeof(CryptoKey_val)/sizeof(CryptoKey_val[0]))
    int32 stat_tests[LEN_CRTPTO_VALS][LEN_AUTH_VALS][LEN_CRYPTOKEY_VALS];
    int32 crypto_GeneralApiRandTest(uint32 seed, uint32 round,
                                    int32 mode_crypto, int32 mode_auth, int32 maxScatter)
    {
        int i, j, k;
        int32 roundIdx;
        int32 pktLen, offset;
        int32 modeCrypto, modeAuth;
        int32 lenCryptoKey, lenAuthKey;
        int32 lenA2eo, lenEnl;
        int32 ret = SUCCESS;
        //static uint8 _cryptoKey[32 + 32 + 32]; // 32 for AES-256
        //static uint8 _cryptoIv[32 + 16 + 32]; // 16 for AES
        //static uint8 _authKey[32 + MAX_AUTH_KEY + 32];
        //static uint8 _pad[32 + 128 + 32]; // 128 for ipad and opad
        //static uint8 _asic_orig[32 + MAX_PKTLEN + 8 + 32]; // +8 for max offset
        //static uint8 _asic_enc[32 + MAX_PKTLEN + 32];
        //static uint8 _asic_digest[32 + SHA_DIGEST_LENGTH + 32]; // 20 for SHA1
        static uint8 *cryptoKey, *cryptoIv, *authKey;//, *pad;
        static uint8 *asic_orig, *asic_enc, *asic_digest;
        //rtl_ipsecScatter_t scatter[MAX_SCATTER];
        //int32 cntScatter, randScatter = 0;
        uint32 len_unused;
        uint32 mode_select;
        uint8 *asic_buf;
        int cnt_success, cnt_failed, cnt_ignore, cnt_scatters;
        int idxCrypto, idxAuth, idxKeyLen, cnt;

        int stop_run_keyin;
        printk(".seed=%x \n", seed);

        //cryptoKey = (void *) UNCACHED_ADDRESS(&_asic_cryptkey[32]);
        cryptoKey = (uint8 *) (&_asic_cryptkey[32]);
        //cryptoIv = (void *) UNCACHED_ADDRESS(&_asic_iv[32]);
        cryptoIv = (uint8 *) (&_asic_iv[32]);
        //authKey = (void *) UNCACHED_ADDRESS(&_asic_authkey[32]);
        authKey = (uint8 *) (&_asic_authkey[32]);
        //pad = (void *) UNCACHED_ADDRESS(&_pad[32]);
        //asic_orig = (void *) UNCACHED_ADDRESS(&_asic_orig[32]);
        asic_orig = (uint8 *) (&_asic_orig[32]);
        //asic_enc = (void *) UNCACHED_ADDRESS(&_asic_enc[32]);
        asic_enc = (uint8 *) (&_asic_enc[32]);
        //asic_digest = (void *) UNCACHED_ADDRESS(&_AsicDigest[32]);
        asic_digest = (uint8 *) (&_AsicDigest[32]);

        //if (maxScatter == 0xff)
        //	randScatter = 1;
        //else if (maxScatter < 1)
        //	maxScatter = 1;
        //else if (maxScatter >= MAX_SCATTER)
        //	maxScatter = MAX_SCATTER;

        cnt_success = 0;
        cnt_failed = 0;
        cnt_ignore = 0;
        cnt_scatters = 0;

        memset(stat_tests, 0, sizeof(stat_tests));

        if (mode_crypto == 0xff)
            ; // random case
        else if (mode_crypto >= 0 && mode_crypto <= 3)
            idxCrypto = mode_crypto + 1;
        else if (mode_crypto == 0x20)
            idxCrypto = 5;
        else if (mode_crypto >= 0x22 && mode_crypto <= 0x23)
            idxCrypto = mode_crypto - 0x22 + 6;
        else
            idxCrypto = 0; // no crypto
        if (mode_auth == 0xff)
            ; // random case
        else if (mode_auth >= 0 && mode_auth <= 3)
            idxAuth = mode_auth + 1;
        else
            idxAuth = 0; // no auth

        for (roundIdx=0; roundIdx<round; roundIdx++) {
            printk("============>  start TEST seed=%x <=================\n", seed);
            //if (tstc()) {
            if (0) {
                //          stop_run_keyin = getc();
                //          printf("stop_run_keyin=%c", stop_run_keyin);
                //          if (stop_run_keyin == 's') {
                //              printf("stop test\n");
                //              break;
                //          }
            }
            //rtlglue_srandom(seed++);
            rtl_srandom(seed++);
//		rtlglue_srandom(get_rand_seed());
//printf("(%s-%d)\n", __func__, __LINE__);

            // SAWB mode
            //i = rtlglue_random() & 1;
            //if ( i != rtl_ipsecEngine_sawb() )
            //{
            //	rtl_ipsecSetOption(RTL_IPSOPT_SAWB, i);
            //}

            // DMA burst mode
            //i = rtlglue_random() & 3;
            //if ( i != rtl_ipsecEngine_dma_mode() )
            //{
            //	j = rtl_ipsecEngine_descNum();
            //	rtl_ipsecEngine_init(j, i);
            //}

            if (mode_crypto == 0xff)
                idxCrypto = rtl_random() % LEN_CRTPTO_VALS;
#ifdef CRYPTO_FORCE_MODE
            idxCrypto = FORCE_modeCrypto;
#endif
            //idxCrypto = 4;
            modeCrypto = modeCrypto_val[idxCrypto];
            printk("\nidxCrypto=%x, modeCrypto=%x\n", idxCrypto, modeCrypto);

            if (mode_auth == 0xff)
                idxAuth = rtl_random() % LEN_AUTH_VALS;
#ifdef CRYPTO_FORCE_MODE
            idxAuth = FORCE_modeAuth;
#endif
            //idxAuth = 2;
            modeAuth = modeAuth_val[idxAuth];
            printk("\nidxAuth=%x, modeAuth=%x\n", idxAuth, modeAuth);
            if (modeCrypto == 0x00 || modeCrypto == 0x01)
                idxKeyLen = 0; // DES
            else if (modeCrypto == 0x10 || modeCrypto == 0x11)
                idxKeyLen = 2; // 3DES
            else
                idxKeyLen = (rtl_random() % 3) + 1; // AES 128,192,256

            lenCryptoKey = CryptoKey_val[idxKeyLen];

            stat_tests[idxCrypto][idxAuth][idxKeyLen]++;

            //lenAuthKey = rtlglue_random() & 0x3ff; // 0~1023
            //if (lenAuthKey > MAX_AUTH_KEY)
            //	lenAuthKey = MAX_AUTH_KEY;
            lenAuthKey = (rtl_random() & 0x3f)+1; // 1~64

            for(i=0; i<24; i++)
                cryptoKey[i] = rtl_random() & 0xFF;
            for(i=0; i<8; i++)
                cryptoIv[i] = rtl_random() & 0xFF;

            //if (randScatter)
            //{
            //	i = MAX_SCATTER < 7 ? MAX_SCATTER : 7;
            //	if (rtl_ipsecEngine_sawb()) // max scatter is 7 if enable sawb
            //		maxScatter = (rtlglue_random() % i) + 1;
            //	else
            //		maxScatter = (rtlglue_random() % MAX_SCATTER) + 1;
            //}

            if (modeCrypto == -1 && modeAuth == -1) {
                cnt_ignore++;
                continue;
            } else if (modeCrypto == -1) { // auth only
                //lenA2eo = rtl_random() & 0xFC; // a2eo must 4 byte aligna, 0..252
                lenA2eo=0;
                mode_select=CRYPTO_MS_TYPE_AUTH;

                if ((modeAuth&0x4) == 0)
                    lenAuthKey=0;//no hmac

                lenCryptoKey=0;
                //if (roundIdx % 10 > 1) // 80% case use 1..1518
                if (1) {//jwsyu pktLen random
                    pktLen = rtl_random() % 1518 + 1;
                    pktLen = rtl_random() % 318 + 1;

                } else if (modeAuth & 0x4) // HMAC
                    pktLen = rtl_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK) + 1; // 1..16247
                else // HASH only
                    pktLen = rtl_random() % (0x3FC0 - 9) + 1; // 1..16311
            } else if (modeAuth == -1) { // crypto only
                lenA2eo = 0; // lenA2eo must 0 if crypto only
                lenAuthKey=0;
                mode_select = CRYPTO_MS_TYPE_CIPHER;

                //if (roundIdx % 10 > 1) // 80% case use 1..1518
                if (1) { //jwsyu pktLen random
                    pktLen = rtl_random() % 1518 + 1;
                    pktLen = rtl_random() % 518 + 1;
                    pktLen = pktLen & ((modeCrypto & 0x20) ? 0x3FF0 : 0x3FF8);
                } else if (modeCrypto & 0x20) // aes
                    pktLen = rtl_random() & 0x3FF0; /* 16 ~ 16368, 0 will be skipped later */
                else // des
                    pktLen = rtl_random() & 0x3FF8; /* 8 ~ 16376, 0 will be skipped later */
            } else { // mix mode
                int max_a2eo;
                if (rtl_random()&1) {
                    mode_select = CRYPTO_MS_TYPE_MIX2_sshdec_espenc;
                } else {
                    mode_select = CRYPTO_MS_TYPE_MIX1_sshenc_espdec;
                }

                mode_select = CRYPTO_MS_TYPE_MIX3_sslenc;

                //modeAuth |= 0x4; // support hmac only if mix mode
                if ((modeAuth&0x4) == 0)
                    lenAuthKey=0;//no hmac

                if ((1 /*cntScatter == 1*/) || (!0/*rtl_ipsecEngine_sawb()*/)) {
#if 1
                    int dma_bytes[4] = {16, 32, 64, 64};
                    max_a2eo = 152 + dma_bytes[CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/] - 16;
#else
                    max_a2eo = 152 - 4;
#endif
                } else {
                    max_a2eo = 152 - 16;
                }
                //jwsyu limit max a2eo 20160108,
#ifdef MAX_A2EO_LIMIT
                max_a2eo=32;
                //max_a2eo=132;
#endif
                max_a2eo=32;
                //lenA2eo = (rtl_random() % max_a2eo) & 0xFC; // a2eo must 4 byte aligna, 0..252
                lenA2eo = (rtl_random() % max_a2eo);  // a2eo must 1 byte aligna, 0..32

                //if (roundIdx % 10 > 1) // 80% case use 1..1518
                if (0) //jwsyu pktLen random
                    lenEnl = rtl_random() % (1518 - lenA2eo) + 1;
                else
                    lenEnl = rtl_random() % (0x3FC0 - 9 - HMAC_MAX_MD_CBLOCK - lenA2eo) + 1; /* 1..16247 - a2eo*/
                //test short data
                lenEnl = rtl_random() % (268 - lenA2eo) + 1;
                if (modeCrypto & 0x20) {	// aes
                    if (mode_select!=CRYPTO_MS_TYPE_MIX3_sslenc)
                        lenEnl = lenEnl & 0x3FF0;
                    if (lenEnl <= 16)
                        lenEnl = 16;
                } else { // des
                    if (mode_select!=CRYPTO_MS_TYPE_MIX3_sslenc)
                        lenEnl = lenEnl & 0x3FF8;
                    if (lenEnl <= 8)
                        lenEnl = 8;
                }

                pktLen = lenEnl + lenA2eo;
            }
#ifdef CRYPTO_FORCE_MODE
            lenEnl = FORCE_pktLen - FORCE_lenA2eo;
            lenA2eo = FORCE_lenA2eo;
            pktLen = FORCE_pktLen;
#endif
            //pktLen = 68;
            //lenA2eo = 4;
            //lenEnl = pktLen-lenA2eo;

            offset = rtl_random() & 0x7;
//offset = 0;
            for(i=0; i<pktLen; i++) //8-byte for IV and 4-byte for DMA_addr (0-3) byte offset test
                asic_orig[i+offset] = rtl_random() & 0xFF;

            len_unused = pktLen;
            asic_buf = &asic_orig[offset];
            //for (cntScatter=0; cntScatter<maxScatter && len_unused > 0; cntScatter++)
            //{
            //	if (cntScatter == maxScatter - 1) // last one?
            //	{
            //		scatter[cntScatter].len = len_unused;
            //		scatter[cntScatter].ptr = asic_buf;
            //		cnt_scatters++;
            //	}
            //	else
            //	{
            //		scatter[cntScatter].len = (rtlglue_random() % len_unused) + 1;
            //		scatter[cntScatter].ptr = asic_buf;
            //		asic_buf += scatter[cntScatter].len;
            //		len_unused -= scatter[cntScatter].len;
            //		cnt_scatters++;
            //	}
            //}

            for(i=0; i<lenAuthKey; i++)
                authKey[i] = (i & 0xff);

//modeCrypto = -1;
//pktLen -= lenA2eo;
//lenA2eo = 0;
//modeCrypto = ;
//modeAuth = ;
            ret = crypto_GeneralApiTestItem(modeCrypto, modeAuth,
                                            asic_buf, pktLen, asic_enc,
                                            lenCryptoKey, cryptoKey,
                                            lenAuthKey, authKey,
                                            cryptoIv, NULL/*pad*/, asic_digest,
                                            lenA2eo, pktLen - lenA2eo, mode_select);

            if (ret == FAILED) {
                printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d [FAILED-%d]\n",
                       0/*rtl_ipsecEngine_sawb()*/, CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/,
                       pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo , __LINE__
                      );

                if (0/*g_failed_reset*/) {
                    //rtl_ipsecEngine_reset();
                    cnt_failed++;
                } else
                    return FAILED;
            } else if (ret > 0) {
                if (dbg_level >= 2)
                    printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d [SKIP %d]\n",
                           0/*rtl_ipsecEngine_sawb()*/, CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/,
                           pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo,
                           ret);

                cnt_ignore++;
            } else {
                if (dbg_level >= 1)
                    printk("SAWB:%d DMA:%d Len:%d Off:%d Enc:0x%x Auth:0x%x CK:%d AK:%d A2EO:%d [SUCCESS]\n",
                           0/*rtl_ipsecEngine_sawb()*/, CRYPTO_TEST_DMA_MODE/*rtl_ipsecEngine_dma_mode()*/,
                           pktLen, offset, modeCrypto, modeAuth, lenCryptoKey, lenAuthKey, lenA2eo
                          );

                cnt_success++;
            }
        }

        if (0/*g_failed_reset*/)
            printk("\nGeneral API Test Finished. (%d, %d, %d) %d\n", cnt_success, cnt_failed, cnt_ignore, cnt_scatters);
        else
            printk("\nGeneral API Random Test Finished. %d/%d, %d\n", cnt_success, cnt_success + cnt_ignore, cnt_scatters);

        for (i=0; i<LEN_CRTPTO_VALS; i++) {
            for (j=0; j<LEN_AUTH_VALS; j++) {
                switch (modeCrypto_val[i]) {
                    case -1: // no crypto
                        cnt = 0;
                        for (k=0; k<LEN_CRYPTOKEY_VALS; k++)
                            cnt += stat_tests[i][j][k];

                        printk("no crypto: stat(0x%x, 0x%x) = %d\n",
                               modeCrypto_val[i], modeAuth_val[j], cnt);
                        break;
                    case 0x00: // des ecb
                    case 0x01: // des cbc
                        for (k=0; k<LEN_CRYPTOKEY_VALS; k++) {
                            if (k == 0) {
                                printk("des: stat(0x%x, 0x%x) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], stat_tests[i][j][0]);
                                continue;
                            }

                            // check keylen != 8
                            if (stat_tests[i][j][k])
                                printk("!des: stat(0x%x, 0x%x, %d) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
                                       stat_tests[i][j][k]);
                        }
                        break;
                    case 0x10: // 3des ecb
                    case 0x11: // 3des cbc
                        for (k=0; k<LEN_CRYPTOKEY_VALS; k++) {
                            if (k == 2) {
                                printk("3des: stat(0x%x, 0x%x) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], stat_tests[i][j][2]);
                                continue;
                            }

                            // check keylen != 24
                            if (stat_tests[i][j][k])
                                printk("!3des: stat(0x%x, 0x%x, %d) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
                                       stat_tests[i][j][k]);
                        }
                        break;
                    default: // aes cbc/ecb/ctr 128/192/256
                        for (k=0; k<LEN_CRYPTOKEY_VALS; k++) {
                            if (k) {
                                printk("aes: stat(0x%x, 0x%x, %d) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
                                       stat_tests[i][j][k]);
                                continue;
                            }

                            // check keylen != 16/24/32
                            if (stat_tests[i][j][k])
                                printk("!aes: stat(0x%x, 0x%x, %d) = %d\n",
                                       modeCrypto_val[i], modeAuth_val[j], CryptoKey_val[k],
                                       stat_tests[i][j][k]);
                        }
                        break;
                }
            }
        }

        return SUCCESS;
    }
// Single DES Encryption in ECB mode
//
//char *crypto_msg = "Now is the time for all ";
//int crypto_msglen = 24;
//char *crypto_key = "0123456789ABCDEF";
//int crypto_keylen = 16;
//char *crypto_iv =  "1234567890ABCDEF";



//char *crypto_msg = "The quick brown fox jumped over the lazy dog";
//char *crypto_key = "1234567890123456ABCDEFGH";
//int crypto_keylen = 24;
//char *crypto_iv =  "";
//int crypto_iv_len = 0;


//char *crypto_msg = "Now is the time for all ";
//int crypto_msglen = 24;
//char *crypto_key = "0123456789ABCDEF";
//int crypto_keylen = 16;
//char *crypto_iv =  "1234567890ABCDEF";


// AES 128

//char *crypto_msgkey = "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
//	  "\x51\x2e\x03\xd5\x34\x12\x00\x06";

//char *crypto_iv = "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
//	  "\xb4\x22\xda\x80\x2c\x9f\xac\x41";

//char *crypto_encoded = "\xe3\x53\x77\x9c\x10\x79\xae\xb8"
//	  "\x27\x08\x94\x2d\xbe\x77\x18\x1a";

//char *crypto_text = "Single block msg";

//u8 _asic_enc[32 + 128 + 32];


    int vector_test_cipher_3DES(u32 cipher_type)
    {
        const u8 *key, *pIv;
        u32 keylen, ivlen;
        u8 *message;
        u8 *pResult_asic;
        u8 *pResult_sw;
        int ret;
        u32 msglen, enl;



        // Encryption

        key = des3_test_keys;
        keylen = 24;
        //message = &_asic_orig[32];
        // force not 4-byte alignmnet
        message = &_asic_orig[33];
        msglen = sizeof(des3_test_buf);
        rtl_memcpy(message, des3_test_buf, msglen);
        pIv = &des3_test_iv[0];
        ivlen = 8;

        // force not 4-byte alignment
        pResult_asic = &_asic_enc[1];
        pResult_sw   = _sw_enc;
        enl = msglen;


        switch ( cipher_type) {
            case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
                rtl_crypto_3des_cbc_init(key,keylen);
                ret = rtl_crypto_3des_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_3DES_ECB: // 3DES-ECB
                pIv = NULL;
                ivlen = 0;
                rtl_crypto_3des_ecb_init(key,keylen);
                dbg_mem_dump(pIv, ivlen, "3DES ECB IV: ");
                ret = rtl_crypto_3des_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!\n");
            return ret;
        }

        dbg_mem_dump(pIv, ivlen, "3DES CBC IV - before sim: ");


        ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, 0, NULL, 0, message, msglen, pResult_sw, NULL);
        if (ret != SUCCESS) {
            err_printk("ipsecSim encrypt failed!\n");
            return ret;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 ) {
            memDump(pResult_asic, msglen, "test OK - enc:");
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
            return FAIL;
        }

        // Decryption

        message = pResult_asic;

        // force not 4-byte alignment
        pResult_asic = &_asic_dec[1];
        pResult_sw   = _sw_dec;

        switch ( cipher_type) {
            case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
                dbg_mem_dump(pIv, ivlen, "3DES CBC IV: ");

                ret = rtl_crypto_3des_cbc_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            case CIPHER_TYPE_3DES_ECB : // 3DES-ECB
                ret = rtl_crypto_3des_ecb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;


            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!\n");
            return ret;
        }

        ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, 0, NULL, 0, message, msglen, pResult_sw, NULL);
        if (ret != SUCCESS) {
            err_printk("ipsecSim encrypt failed!\n");
            return ret;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            return SUCCESS;
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
            return FAIL;
        }


    }



// for chacha , poly1305


//
// http://www.inconteam.com/software-development/41-encryption/55-aes-test-vectors
//


// 128 bit - CBC

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    } ;


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_iv_1[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };



    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_buf[16] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };



    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_128[16] = {
        0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
        0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
    };


// 192 bit - CBC

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_key_192[24] = {
        0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
        0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
    };



    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_192_cbc[16] = {
        0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
        0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8
    };


// 256 bit - CBC

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_key_256[32] = {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
        0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
        0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
    };



    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_256_cbc[16] = {
        0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
        0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6
    };


// ECB

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_128_ECB[16] = {
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
    };


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_192_ECB[16] = {
        0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f,
        0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc
    };


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_256_ECB[16] = {
        0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
        0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8
    };



// CTR

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_iv[16] = {
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_128_CTR[16] = {
        0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
        0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
    };


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_res_192_CTR[16] = {
        0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
        0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b
    };

    u8 cipher_result[1024];

    static unsigned char aes_test_ecb_data[128];

    void hex_dump(unsigned char* buf, int len)
    {
        int i;

        for (i=0; i<len; i++) {
            printk("0x%x ", buf[i]);
            if (((i+1)%16) == 0 ) printk("\r\n");
        }
        printk("\r\n");
    }


    void issue_test(void)
    {
        const u8 *key, *pIv;
        u32 keylen= 0;
        u32 ivlen = 0;
        u8 *message;
        u32 msglen;
        u8 *pResult;

        int ret;

        printk("AES ECB test\r\n");

        key = aes_test_key;
        keylen = 16;

        pResult = cipher_result;

        ret = rtl_crypto_aes_ecb_init(key,keylen);
        if ( ret != 0 ) {
            printk("AES ECB init failed\r\n");
            return;
        }

        message = (unsigned char *)aes_test_ecb_data;
        msglen = sizeof(aes_test_ecb_data);

        rtl_memset(message, 0xAA, sizeof(message));
        rtl_memset(pResult, 0xA5, 144);

        printk("pResult : addr(0x%x)\r\n", pResult);
        hex_dump(pResult, 144);

        ret = rtl_crypto_aes_ecb_encrypt(message, msglen, NULL, 0, pResult);
        if ( ret != 0 ) {
            printk("AES ECB encrypt failed, ret=%d\r\n", ret);
            return;
        }

        printk("pResult : addr(0x%x)\r\n", pResult);
        hex_dump(pResult, 144);


        return;
    }

    void issue_test1(void)
    {
        const u8 *key, *pIv;
        u32 keylen= 0;
        u32 ivlen = 0;
        u8 *message;
        u32 msglen;
        u8 *pResult;

        int ret;

        printk("AES ECB test\r\n");

        key = aes_test_key;
        keylen = 16;

        pResult = cipher_result;

        ret = rtl_crypto_aes_ecb_init(key,keylen);
        if ( ret != 0 ) {
            printk("AES ECB init failed\r\n");
            return;
        }

        message = (unsigned char *)aes_test_buf;
        msglen = sizeof(aes_test_buf);

        rtl_memset(pResult, 0xA5, 20);

        printk("pResult : addr(0x%x)\r\n", pResult);
        hex_dump(pResult, 20);

        ret = rtl_crypto_aes_ecb_encrypt(message, msglen, NULL, 0, pResult);
        if ( ret != 0 ) {
            printk("AES ECB encrypt failed, ret=%d\r\n", ret);
            return;
        }

        printk("pResult : addr(0x%x)\r\n", pResult);
        hex_dump(pResult, 20);


        return;
    }


    static inline
    void write_memory(u32 addr, u32 value)
    {
        *((volatile u32*)(addr)) = value;

    }

    static const unsigned char test_key[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };

// set key
    static const unsigned char src_desc1[] = {
        0xa3, 0xa1, 0x80, 0x10, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x82, 0x64
    };

// set iv
    static const unsigned char src_desc2[] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

// set message
    static const unsigned char src_desc3[] = {
        0x80, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x71, 0x68
    };

    static const unsigned char test_msg[] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };

// dest descriptor
    static const unsigned char dst_desc1[] = {
        0x80, 0x00, 0x00, 0x10, 0x10, 0x01, 0xb5, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };



    void issue_test2(void)
    {
        printk("issue Test1 : all register setting \r\n");

        write_memory(0x40000210, 0x00011107);
        write_memory(0x40000210, 0x00111107);
        write_memory(0x40000244, 0x00000010);
        write_memory(0x40000244, 0x00000030);

        write_memory(0x40070008, 0x1);
        printk("After reset\r\n");

        write_memory(0x40070000, 0x10016b88);
        write_memory(0x40070004, 0x10016c10);
        write_memory(0x4007000c, 0x00000040);
        write_memory(0x40070008, 0x0000f000);
        // debug port
        write_memory(0x400000a0, 0x00000001);
        write_memory(0x4000002c, 0x00fde404);


        // set result to 0xa5 first, write 24 bytes
        rtl_memset((u8*)0x1001b540, 0xa5, 24);


        // set key
        memcpy((void*)0x10008264, test_key, sizeof(test_key));
        memcpy((void*)0x10007168, test_msg, sizeof(test_msg));

        // source descriptor
        memcpy((void*)0x10016b88, src_desc1, sizeof(src_desc1));
        memcpy((void*)0x10016b98, src_desc2, sizeof(src_desc2));
        memcpy((void*)0x10016ba8, src_desc3, sizeof(src_desc2));

        // dest descriptor
        memcpy((void*)0x10016c10, dst_desc1, sizeof(dst_desc1));

        // kick off
        write_memory(0x40070008, 0x0000f800);
        write_memory(0x40070008, 0x00000002);

    }


    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_key_gcm[16] = {
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
    };

    __attribute__ ((aligned (4)))
    static const unsigned char aes_test_iv_gcm[12] = {
        0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
        0xde, 0xca, 0xf8, 0x88
    };

    static const unsigned char aes_test_aad_gcm[20] = {
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xab, 0xad, 0xda, 0xd2
    };

    static const unsigned char aes_test_buf_gcm[] = {
        0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
        0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
        0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
        0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
        0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
        0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
        0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
        0xba, 0x63, 0x7b, 0x39
    };

    int vector_test_cipher_AES(u32 cipher_type)
    {
        const u8 *key, *pIv, *pAAD;
        u32 keylen= 0;
        u32 ivlen = 0;
        u32 aadLen = 0;
        u8 *message;
        u8 *pResult_asic;
        u8 *pResult_sw;
        u8 *pTag_asic;
        u8 *pTag_sw;

        int ret;
        u32 msglen, enl;
        int i;

        // Encryption

        key = aes_test_key;
        keylen = sizeof(aes_test_key);
        //message = &_asic_orig[32];
        // force message not 4-byte alignment
        message = &_asic_orig[33];
        msglen = sizeof(aes_test_buf);
        rtl_memcpy(message, aes_test_buf, msglen);
        pIv = aes_test_iv;
        ivlen = sizeof(aes_test_iv);


        pResult_asic = &_asic_enc[1];
        pResult_sw   = _sw_enc;

        pTag_asic = NULL;
        pTag_sw = NULL;

        enl = msglen;



        switch ( cipher_type) {

            case  CIPHER_TYPE_AES_CBC : // 33
                //pIv = aes_test_iv_1;
                //ivlen = sizeof(aes_test_iv);
                ret = rtl_crypto_aes_cbc_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                //pResult_correct = aes_test_res_128;
                break;

            case CIPHER_TYPE_AES_ECB: // 32
                ret = rtl_crypto_aes_ecb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ivlen = 0;
                ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                //pResult_correct = aes_test_res_128_ECB;
                break;

            case CIPHER_TYPE_AES_CTR : // 36
                msglen = 32;
                for (i=0; i<32; i++) {
                    message[i] = i;
                }
                ret = rtl_crypto_aes_ctr_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_aes_ctr_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                //pResult_correct = aes_test_res_128_CTR;
                break;

            case CIPHER_TYPE_AES_CFB : // AES-CFB
                msglen = 32;
                for (i=0; i<32; i++) {
                    message[i] = i;
                }
                ret = rtl_crypto_aes_cfb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_aes_cfb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                // AES-CFB
                // enc: 87 4d 61 91 b6 20 e3 26  1b ef 68 64 99 0d b6 ce
                break;

            case CIPHER_TYPE_AES_OFB : // AES-OFB
                msglen = 32;
                for (i=0; i<32; i++) {
                    message[i] = i;
                }
                ret = rtl_crypto_aes_ofb_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_aes_ofb_encrypt(message, msglen, pIv, ivlen, pResult_asic);
                // AES-OFB
                // enc: 87 4d 61 91 b6 20 e3 26   1b ef 68 64 99 0d b6 ce
                break;

            case CIPHER_TYPE_AES_GCM : // AES-GCM
                key = aes_test_key_gcm;
                keylen = sizeof(aes_test_key_gcm);
                pIv = aes_test_iv_gcm;
                ivlen = 12; // only support 96 bit
                pAAD = aes_test_aad_gcm;
                aadLen = sizeof(aes_test_aad_gcm);

                memcpy(message, aes_test_buf_gcm, sizeof(aes_test_buf_gcm));
                msglen = sizeof(aes_test_buf_gcm);

                pTag_asic = _AsicDigest;
                pTag_sw = sw_digest;

                ret = rtl_crypto_aes_gcm_init(key,keylen);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_aes_gcm_encrypt(message, msglen, pIv, pAAD, aadLen, pResult_asic, pTag_asic);
                break;

            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            return ret;
        }


        ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, pIv, ivlen, 0, pAAD, aadLen, message, msglen, pResult_sw, pTag_sw);
        if (ret != SUCCESS) {
            dbg_mem_dump(pResult_sw, msglen, "SW result:");
            dbg_mem_dump(pTag_sw, 16, "SW TAG:");
            err_printk("ipsecSim encrypt ret=0x%x\n", ret);
            return ret;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
            if ( pTag_asic != NULL ) {
                if ( rtl_memcmpb(pTag_asic, pTag_sw, 16) == 0 ) {
                    dbg_mem_dump(pTag_asic, msglen, "test OK - enc - TAG:");
                } else {
                    err_printk("test failed!\n");
                    dbg_mem_dump(pTag_asic, 16, "ASIC enc Tag: ");
                    dbg_mem_dump(pTag_sw, 16, "SW enc Tag: ");
                    return FAIL;
                }
            }
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
            return FAIL;
        }

        // Decryption

        message = pResult_asic;

        //pResult_asic = _asic_dec;
        // force not 4-byte alignment
        pResult_asic = &_asic_dec[1];
        pResult_sw   = _sw_dec;
        //pResult_correct = aes_test_buf;


        switch ( cipher_type) {

            case CIPHER_TYPE_AES_ECB  : // AES-ECB
                ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv,ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CBC : // AES-CBC
                ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv,ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CFB : // AES-CFB
                ret = rtl_crypto_aes_cfb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_OFB : // AES-OFB
                ret = rtl_crypto_aes_ofb_decrypt(message, msglen, pIv, ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_CTR : // AES-CTR
                ret = rtl_crypto_aes_ctr_decrypt(message, msglen, pIv,ivlen, pResult_asic);
                break;

            case CIPHER_TYPE_AES_GCM : // AES-GCM
                ret = rtl_crypto_aes_gcm_decrypt(message, msglen, pIv,pAAD, aadLen, pResult_asic, pTag_asic);
                break;

            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec decrypt failed!\n");
            return ret;
        }


        ret = rtl_crypto_esp_sim(cipher_type, key, keylen, pIv, ivlen, 0, pAAD, aadLen, message, msglen, pResult_sw, pTag_sw);
        if (ret != SUCCESS) {
            err_printk("ipsecSim decrypt failed!\n");
            return ret;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            if ( pTag_asic != NULL ) {
                if ( rtl_memcmpb(pTag_asic, pTag_sw, 16) == 0 ) {
                    dbg_mem_dump(pTag_asic, msglen, "test OK - dec - TAG:");
                } else {
                    err_printk("test failed!\n");
                    dbg_mem_dump(pTag_asic, 16, "ASIC dec Tag: ");
                    dbg_mem_dump(pTag_sw, 16, "SW dec Tag: ");
                    return FAIL;
                }
            }

            return SUCCESS;
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
            return FAIL;
        }


    }



//
// http://tools.ietf.org/html/draft-nir-cfrg-chacha20-poly1305-06
//
// 2.4.1 example and test vector
//



// Test Vector #1




    int vector_test_cipher_chacha(u32 cipher_type)
    {
        const u8 *key, *pIv;
        u8 *message;
        u8 *pResult_sw;
        u8 *pResult_asic;
        int ret;
        u32 msglen, enl;

        // Encryption

        key = chacha_test_key;
        message = &_asic_orig[32];
        msglen = strlen(chacha_test_buf);
        rtl_memcpy(message, chacha_test_buf, msglen);
        pIv = chacha_test_iv;

        pResult_sw   = _sw_enc;
        pResult_asic = &_asic_enc[1];

        enl = msglen;

        switch ( cipher_type) {
            case 0x30: // chacha
                ret = rtl_crypto_chacha_init(key);
                if ( ret != SUCCESS ) return ret;
                ret = rtl_crypto_chacha_encrypt(message, msglen, pIv, chacha_init_count, pResult_asic);
                // chacha
                break;


            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                ret = _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH;
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            return ret;
        }


        //CRYPTO_sim_chacha_20(pResult_sw, message, msglen, key, pIv, chacha_init_count);

        ret = rtl_crypto_esp_sim(cipher_type |0x80, key, 32, pIv, 12, chacha_init_count, NULL, 0, message, msglen, pResult_sw, NULL);

        if ( (ret=rtl_memcmpb(pResult_sw, chacha_test_res , enl)) == 0 ) {
            dbg_printk(" SW CHACHA test OK \r\n");
        }	else {
            dbg_printk(" SW CHACHA test failed, ret=%d \r\n", ret);
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, enl) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
            return FAIL;
        }

        // Decryption

        message = pResult_asic;

        pResult_asic = &_asic_dec[1];
        pResult_sw   = _sw_dec;

        switch ( cipher_type) {

            case 0x30 : // chacha
                ret = rtl_crypto_chacha_decrypt(message, msglen, pIv, chacha_init_count, pResult_asic);
                break;

            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        if (ret != SUCCESS) {
            err_printk("ipsec decrypt failed!, ret=%d\n", ret);
            return ret;
        }


        CRYPTO_sim_chacha_20(pResult_sw, message, msglen, key, pIv, chacha_init_count);



        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            return SUCCESS;
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
            return FAIL;
        }

        return ret;

    }


//


//static const unsigned char chacha_poly1305_key_str[] =
//"88e79a5e206254359db6e5b747bb1532b1e90596d836068e391ef1c0135d23b0";

//static const unsigned char chacha_poly1305_test_str[] =
//"011130303a45303a34433a38373a31333a31330a403403c8c8d793ed16431b373c1694363a9b32d0080c117a4db33f8ee5a93ed7d5510981129544bb8604f026c9c27c9fa9020c91776c6239f3f4d732220331610f";

//static const unsigned char chacha_poly1305_test_res_str[] =
//"09b665601b79525ac639613557cc6b5028eaa577768e3a7652069b93a4b796ce014cd953a653cca8ee417e4879b54d9ccf4f3ea7efdbf6769472d4190e6046ab87a8690104101a08466fff9443d7d27d4b8e101f0497781794aa9fd6feec852d9873b45f51";


    static const unsigned char chacha_poly1305_key_str[]="e4e44959a7c8664c95e304cb70ec91da05179a384a545ea548bd2d6524cfc3c8";

    static const unsigned char chacha_poly1305_test_str[]=
        "485454502f312e3120323030204f4b0d0a436f6e74656e742d547970653a206170706c69636174696f6e2f6861702b6a736f6e0d0a436f6e74656e742d4c656e6774683a2037310d0a0d0a7b0a0922636861726163746572697374696373223a095b7b0a09090922616964223a09312c0a09090922696964223a09392c0a0909092276616c7565223a09300a09097d5d0a7d";

    static const unsigned char chacha_poly1305_test_res_str[] =
        "b0b21fe4c6b6735fcdef38e5d60f0694ce880085e227befc9365701cc8d7abedd93054eca95b5abe003515c46d44d77a5b75b049bdbfb458671a2043ab4b7375f312b6f83b571d020c40d29eae5a2b5fc1bd7cf1367971acab2964f670be1e20b2b9cdf37e279637b00a72514522d61846bc2f9caaa418af1fefa22d5b3a2b0820d46676932b6cf90ac5c41332ca38caf61f271eee706fabb180b5831af184989fc2";

    static unsigned char chacha_poly1305_aad[] = {0x92, 0x00};

    static unsigned char hex_digit(unsigned char h)
    {
        if (h >= '0' && h <= '9')
            return h - '0';
        else if (h >= 'a' && h <= 'f')
            return h - 'a' + 10;
        else if (h >= 'A' && h <= 'F')
            return h - 'A' + 10;
    }

    static void hex_decode(uint8_t *out, const unsigned char* hex)
    {
        size_t j = 0;

        while (*hex != 0) {
            unsigned char v = hex_digit(*hex++);
            v <<= 4;
            v |= hex_digit(*hex++);
            out[j++] = v;
        }
    }


    static unsigned char chacha_poly1305_test[1024];
    static unsigned char chacha_poly1305_test_res[1024];


    int vector_test_chacha_poly1305(void)
    {
        u8 key[32];
        u8 nonce[12];
        u8 *aad;
        int aadlen;
        u8 *message;
        u8 *pResult_sw;
        u8 *pResult_asic;
        int ret;
        u32 msglen, enl;
        int datalen;


        // Encryption

        hex_decode(key, chacha_poly1305_key_str);

        message = &_asic_orig[32];
        msglen = strlen(chacha_poly1305_test_str)/2;
        hex_decode(chacha_poly1305_test, chacha_poly1305_test_str);
        rtl_memcpy(message, chacha_poly1305_test, msglen);

        // nonce
        //rtl_memset(&nonce[0], 0, 4);
        //memcpy(&nonce[4], "PV-Msg02", 8);
        memset(nonce, 0, 12);
        nonce[4] = 3;

        aad = &chacha_poly1305_aad[0];
        aadlen = 2;

        pResult_sw   = _sw_enc;
        pResult_asic = &_asic_enc[1];

        hex_decode(chacha_poly1305_test_res, chacha_poly1305_test_res_str);
        enl = msglen;


        ret = rtl_crypto_chacha_poly1305_init(key);
        if ( ret != SUCCESS ) return ret;

        ret = rtl_crypto_chacha_poly1305_encrypt(message, msglen, nonce, aad, aadlen, pResult_asic, pResult_asic+msglen);
        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            return ret;
        }


        datalen = aead_chacha20_poly1305_enc(pResult_sw, msglen+16, message, msglen, aad, aadlen, nonce, key);
        if ( datalen != (msglen+16) ) {
            dbg_printk(" SW CHACHA Poly1305 test failed, datalen=%d \r\n", datalen);
        }

        if ( (ret=rtl_memcmpb(pResult_sw, chacha_poly1305_test_res , msglen+16)) == 0 ) {
            dbg_printk(" SW CHACHA test OK \r\n");
            dbg_mem_dump(pResult_sw, msglen+16, "SW enc msg: After");
        }	else {
            dbg_printk(" SW CHACHA test failed, ret=%d \r\n", ret);
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
            if ( rtl_memcmpb(pResult_asic+msglen, pResult_sw+msglen, 16) == 0 ) {
                dbg_mem_dump(pResult_asic+msglen, 16, "test OK - TAG:");
            } else {
                err_printk("test failed!\n");
                dbg_mem_dump(pResult_asic+msglen, 16, "ASIC enc TAG: ");
                dbg_mem_dump(pResult_sw+msglen, 16, "SW enc TAG: ");
                return FAIL;
            }
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC enc msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW enc msg: ");
            return FAIL;
        }


        // decryption
        message = pResult_asic;
        pResult_asic = &_asic_dec[1];
        pResult_sw = _sw_dec;

        ret = rtl_crypto_chacha_poly1305_decrypt(message, msglen, nonce, aad, aadlen, pResult_asic, pResult_asic+msglen);
        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            return ret;
        }

        datalen = aead_chacha20_poly1305_dec(pResult_sw, msglen, message, msglen+16, aad, aadlen, nonce, key);
        if ( datalen != msglen ) {
            dbg_printk(" SW CHACHA Poly1305 test failed, datalen=%d \r\n", datalen);
        }

        if ( (ret=rtl_memcmpb(pResult_sw, chacha_poly1305_test , msglen)) == 0 ) {
            dbg_printk(" SW CHACHA test OK \r\n");
            dbg_mem_dump(pResult_sw, msglen, "SW enc msg: After");
        }	else {
            dbg_printk(" SW CHACHA test failed, ret=%d \r\n", ret);
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            if ( rtl_memcmpb(pResult_asic+msglen, message+msglen, 16) == 0 ) {
                dbg_mem_dump(pResult_asic+msglen, 16, "test OK - TAG:");
            } else {
                err_printk("test failed!\n");
                dbg_mem_dump(pResult_asic+msglen, 16, "ASIC dec TAG: ");
                dbg_mem_dump(message+msglen, 16, "SW dec TAG: ");
                return FAIL;
            }
        } else {
            err_printk("test failed!\n");
            dbg_mem_dump(pResult_asic, msglen, "ASIC dec msg: ");
            dbg_mem_dump(pResult_sw, msglen, "SW dec msg: ");
            return FAIL;
        }

        return ret;

    }


    int vector_test_mix_rand(void)
    {
        int ret = FAIL;

        u32 keylen;
        u8* key;

        u8 nonce[12];

        u32 msglen;
        u8 *message;

        int aadLen;
        u8* pAAD;

        u8 *pResult_asic;
        u8 *pResult_sw;

        u32 msg_pos;
        u32 result_pos;

        int i;


        //
        this_rtlseed = rtl_random();
        //this_rtlseed = 0xebe007e5;
        //rtl_srandom(this_rtlseed);

        // KEY
        keylen = 32;
        key = &_asic_cryptkey[0];

        for (i=0; i<keylen; i++)
            key[i] = rtl_random() & 0xFF;
        key[keylen] = '\0';

        // IV, nonce
        memset(&nonce[0], 0, 4);
        if ( (rtl_random()%2) == 0 ) { // nonce string
            for (i=0; i<8; i++) {
                nonce[i]= (rtl_random()%127)+1;
            }
        } else { // nonce number
            for (i=0; i<8; i++) {
                nonce[i]= (rtl_random()%256);
            }
        }

        // AAD;
        aadLen = rtl_random() & 0xFF;
        pAAD = (aadLen==0)? NULL : _asic_aad;

        //message
        msglen = rtl_random() % 16384 + 64;
        msg_pos = rtl_random() % 32;
        message = &_asic_orig[msg_pos];

        for (i=0; i<msglen; i++)
            message[i] = rtl_random() & 0xFF;
        message[msglen] = '\0';

        for (i=0; i<32; i++)
            message[msglen+i] = 0xA0;


        result_pos = rtl_random() % 32;
        pResult_asic = &_asic_enc[result_pos];
        pResult_sw	 = _sw_enc;

        ret = rtl_crypto_chacha_poly1305_init(key);
        if ( ret != SUCCESS ) return ret;

        ret = rtl_crypto_chacha_poly1305_encrypt(message, msglen, nonce, pAAD, aadLen, pResult_asic, pResult_asic+msglen);
        if (ret != SUCCESS) {
            err_printk("ipsec encrypt failed!, ret=%d\n", ret);
            ret = FAIL;
            goto ret_vector_test_mix_rand;
        }


        ret = aead_chacha20_poly1305_enc(pResult_sw, msglen+16, message, msglen, pAAD, aadLen, nonce, key);
        if ( ret != (msglen+16) ) {
            err_printk(" SW CHACHA Poly1305 test failed, datalen=%d \r\n", ret);
            ret = FAIL;
            goto ret_vector_test_mix_rand;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen+16) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - enc:");
        } else {
            err_printk("test failed!\n");
            memDump(pResult_asic, msglen+16, "ASIC enc msg: ");
            memDump(pResult_sw, msglen+16, "SW enc msg: ");
            ret = FAIL;
            goto ret_vector_test_mix_rand;
        }

        // issue
        if ( message[msglen+16] != 0xA0 ) {
            err_printk("test failed!, message overwrite\n");
            ret = FAIL;
            goto ret_vector_test_mix_rand;
        }

        // Decryption

        message = pResult_asic;

        result_pos = rtl_random() % 32;
        pResult_asic = &_asic_dec[result_pos];

        memset(pResult_asic, 0x0, 128);

        pResult_sw	= _sw_dec;

        ret = rtl_crypto_chacha_poly1305_decrypt(message, msglen, nonce, pAAD, aadLen, pResult_asic, pResult_asic+msglen);
        if (ret != SUCCESS) {
            err_printk("ipsec decrypt failed!\n");
            goto ret_vector_test_mix_rand;
        }


        ret = aead_chacha20_poly1305_dec(pResult_sw, msglen, message, msglen+16, pAAD, aadLen, nonce, key);
        if ( ret != msglen ) {
            err_printk("ipsecSim decrypt failed!\n");
            memDump(pResult_asic, msglen, "ASIC dec msg: ");
            goto ret_vector_test_mix_rand;
        }

        if ( rtl_memcmpb(pResult_asic, pResult_sw, msglen) == 0 ) {
            dbg_mem_dump(pResult_asic, msglen, "test OK - dec:");
            if ( rtl_memcmpb(pResult_asic+msglen, message+msglen, 16) == 0 ) {
                dbg_mem_dump(pResult_asic+msglen, 16, "test OK - dec: Tag");
            } else {
                err_printk("test failed - TAG!\n");
                memDump(pResult_asic+msglen, 16, "ASIC dec TAG: ");
                memDump(message+msglen, 16, "message TAG: ");
                ret = FAIL;
                goto ret_vector_test_mix_rand;
            }
            ret = SUCCESS;
        } else {
            err_printk("test failed!\n");

            memDump(pResult_asic, msglen, "ASIC dec msg: ");

            ret = FAIL;
            goto ret_vector_test_mix_rand;
        }


        return SUCCESS;

ret_vector_test_mix_rand:

        DiagPrintf("rtl_seed = 0x%x\n", this_rtlseed);
        err_printk("msg_pos=%d\n", msg_pos);
        err_printk("result_pos=%d\n", result_pos);

        err_printk("message (%d) : \n", msglen);
        memDump(message, msglen, "MSG:");

        memDump(key, keylen, "key:");

        memDump(nonce, 12, "nonce:");

        err_printk("AAD (%d) : \n", aadLen);
        memDump(pAAD, aadLen, "AAD:");

        return ret;
    }



// DES-CBC   : 0x00
// DES-ECB   :  0x02
// 3DES-CBC : 0x10
// 3DES-ECB  : 0x12
//

    int vector_test_cipher(u32 cipher_type)
    {
        int ret = FAIL;

        switch ( cipher_type) {
            case CIPHER_TYPE_3DES_CBC: // 3DES-CBC
            case CIPHER_TYPE_3DES_ECB: // 3DES-ECB
                ret = vector_test_cipher_3DES(cipher_type);
                break;
            case CIPHER_TYPE_AES_CBC: // AES-CBC
            case CIPHER_TYPE_AES_ECB : // AES-ECB
            case CIPHER_TYPE_AES_CTR : // AES-CTR
            case CIPHER_TYPE_AES_CFB : // AES-CFB
            case CIPHER_TYPE_AES_OFB : // AES-OFB
            case CIPHER_TYPE_AES_GCTR : // AES-GCTR
            case CIPHER_TYPE_AES_GMAC : // AES-GMAC
            case CIPHER_TYPE_AES_GHASH : // AES-GHASH
            case CIPHER_TYPE_AES_GCM : // AES-GCM
                ret = vector_test_cipher_AES(cipher_type);
                break;
            case 0x30 : // Chacha
                ret = vector_test_cipher_chacha(cipher_type);
                break;
            default :
                err_printk("no this crypto_type=0x%x\n", cipher_type);
                break;
        }

        return ret;

    }


    int throughput_test_cipher(u32 cipher_type,
                               void* key1, u32 keylen, u8* iv, int ivlen,
                               u32 digestlen, u8 *message, int msglen, u8 *pAAD, int aadLen,
                               void *pResult1, u32 resultLen, int loops)
    {
        int loop, ret;
//	u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
        u32 start_count, current_count, spend_time, spend_ticks, throughput;

        u8 *key, *pResult;
        u8 tag[16];

        key = (u8*)key1;
        pResult = (u8*)pResult1;

        printk("%s start\n", __FUNCTION__);
        //start_ticks = xTaskGetTickCount();
        start_count = HalTimerOp.HalTimerReadCount(1);

        switch (cipher_type) {
            case 0x1: // DES-CBC
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_des_cbc_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x0: // DES-ECB
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_des_ecb_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x11: // 3DES-CBC
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_3des_cbc_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_3des_cbc_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x10: // 3DES-ECB
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_3des_ecb_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_3des_ecb_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;


            case 0x21: // AES-CBC

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_cbc_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_cbc_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x20: // AES-ECB
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_ecb_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_ecb_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed! ret=%d\n", ret);
                        return ret;
                    }
                }
                break;

            case 0x24: // AES-CTR
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_ctr_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_ctr_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x22 : // AES-CFB
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_cfb_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_cfb_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x23 : // AES-OFB

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_ofb_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_ofb_encrypt(message, msglen, iv, ivlen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x28 : // AES-GCM

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_aes_gcm_init(key,keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_aes_gcm_encrypt(message, msglen, iv, pAAD, aadLen, pResult, &tag[0]);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 0x30 : // chacha+poly1305

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_chacha_poly1305_init(key);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_chacha_poly1305_encrypt(message, msglen, iv, pAAD, aadLen, pResult, &tag[0]);
                    if (ret != SUCCESS) {
                        err_printk("ipsec encrypt failed!, ret=%d\n", ret);
                        return ret;
                    }
                }
                break;


            case 0x31 : // CHACHA20
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_chacha_init(key);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }
                    ret = rtl_crypto_chacha_encrypt(message, msglen, iv, 42, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }

                break;


            default:
                err_printk("authtype(%d) : no this type\n", cipher_type);
                ret = FAIL;
                break;

        }


        //end_ticks = xTaskGetTickCount();
        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }



        //spend_ticks = end_ticks - start_ticks;
        //spend_time = spend_ticks * portTICK_RATE_MS;
        spend_time = spend_ticks * 30 / loops;
        throughput = (msglen*8) / spend_time;
        printk("HW spend time each:%d, ticks:%d, throughput: %d Mbps\n",
               spend_time, spend_ticks, throughput);

        //
        // run SW simulation
        //
        //start_ticks = xTaskGetTickCount();
        start_count = HalTimerOp.HalTimerReadCount(1);

        if ( cipher_type != 0x30 ) {
            for (loop=0; loop<loops; loop++) {
                ret = rtl_crypto_esp_sim(cipher_type |0x80, key, keylen, iv, ivlen, 0, NULL, 0, message, msglen, pResult, NULL);
            }
        } else {
            for (loop=0; loop<loops; loop++) {
                ret = aead_chacha20_poly1305_enc(pResult, msglen+16, message, msglen, pAAD, aadLen, iv, key);
            }
        }

        //end_ticks = xTaskGetTickCount();
        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }

        //spend_ticks = end_ticks - start_ticks;
        //spend_time = spend_ticks * portTICK_RATE_MS;

        spend_time = spend_ticks * 30 / loops;
        throughput = (msglen*8*1000) / spend_time;
        printk("SW spend time each:%d, ticks:%d, throughput: %d Kbps\n",
               spend_time, spend_ticks, throughput);

        return ret;
    }

#if 0
    int throughput_test_memcpy(void)
    {
        int loop, ret = SUCCESS;
        u32 start_count, current_count, spend_ticks;


        rtl_memset(_asic_orig, 0x0a, sizeof(_asic_orig));
        printk("%s start,  addr(0x%x),\n", __FUNCTION__, (unsigned long)(&_asic_dec[3]));

        start_count = HalTimerOp.HalTimerReadCount(1);

        for (loop=0; loop<1000; loop++)
            _memcpy(&_asic_enc[3], _asic_orig, sizeof(_asic_enc)-3);


        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }

        printk("_memcpy, ticks=%d, 0x%x\n", spend_ticks, _asic_enc[4]);


        rtl_memset(_asic_orig, 0xaa, sizeof(_asic_orig));
        start_count = HalTimerOp.HalTimerReadCount(1);

        for (loop=0; loop<1000; loop++)
            rtl_memcpy(&_asic_dec[3], _asic_orig, sizeof(_asic_dec)-3);


        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }

        printk("rtl_memcpy, ticks=%d, 0x%x\n", spend_ticks, _asic_dec[4]);


        return ret;
    }
#endif

    int throughput_test_auth(u32 authtype,
                             void* key1, u32 keylen,
                             u32 digestlen, u8 *message, int msglen,
                             void *pResult1, u32 resultLen, int loops)
    {
        int loop, ret = SUCCESS;
        //u32 start_ticks, end_ticks, spend_ticks, spend_time, throughput;
        u32 start_count, current_count, spend_time, spend_ticks, throughput;

        u8 *key, *pResult;

        key = (u8*)key1;
        pResult = (u8*)pResult1;

        printk("%s start\n", __FUNCTION__);
        //start_ticks = xTaskGetTickCount();
        start_count = HalTimerOp.HalTimerReadCount(1);

        switch (authtype) {
            case 1:
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_md5_init();
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }


                    ret = rtl_crypto_md5_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 4:
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_hmac_md5_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!, ret=%d\n", ret);
                        return ret;
                    }


                    ret = rtl_crypto_hmac_md5_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!, ret = %d\n", ret);
                        return ret;
                    }
                }
                break;

            case 2:

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_sha1_init();
                    if (ret != SUCCESS) {
                        err_printk("init failed! ret=%d\n", ret);
                        return ret;
                    }

                    ret = rtl_crypto_sha1_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!, ret=%d\n", ret);
                        return ret;
                    }
                }
                break;

            case 5:

                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_hmac_sha1_init(key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_hmac_sha1_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 3:
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_sha2_init(SHA2_256);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }

                    ret = rtl_crypto_sha2_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 6:
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_hmac_sha2_init(SHA2_256, key, keylen);
                    if (ret != SUCCESS) {
                        err_printk("init failed!\n");
                        return ret;
                    }


                    ret = rtl_crypto_hmac_sha2_process(message, msglen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            case 7 : //poly1305
                for (loop=0; loop<loops; loop++) {
                    ret = rtl_crypto_poly1305(message, msglen, key, keylen, pResult);
                    if (ret != SUCCESS) {
                        err_printk("failed!\n");
                        return ret;
                    }
                }
                break;

            default:
                err_printk("authtype(%d) : no this type\n", authtype);
                ret = FAIL;
        }

        //end_ticks = xTaskGetTickCount();
        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }

        //spend_ticks = end_ticks - start_ticks;
        //spend_time = spend_ticks * portTICK_RATE_MS;

        spend_time = spend_ticks * 30 / loops;
        throughput = (msglen*8) / spend_time;
        printk("HW spend time each:%d, ticks:%d, throughput: %d Mbps\n",
               spend_time, spend_ticks, throughput);


        //
        // test sofware
        //

        //start_ticks = xTaskGetTickCount();
        start_count = HalTimerOp.HalTimerReadCount(1);

        for (loop=0; loop<loops; loop++) {
            rtl_crypto_auth_sim(authtype, key, keylen, message, msglen, pResult, digestlen);
        }
        //end_ticks = xTaskGetTickCount();
        current_count = HalTimerOp.HalTimerReadCount(1);
        if ( start_count < current_count) {
            spend_ticks =  (0xFFFFFFFF - current_count) + start_count;
        } else {
            spend_ticks = start_count - current_count;
        }

        //spend_ticks = end_ticks - start_ticks;
        //spend_time = spend_ticks * portTICK_RATE_MS;
        spend_time = spend_ticks * 30 / loops;
        throughput = (msglen*8*1000) / spend_time;
        printk("SW spend time each:%d, ticks:%d, throughput: %d Kbps\n",
               spend_time, spend_ticks, throughput);


        return ret;
    }


    int throughput_test(u32 modetype, u32 cipher_type, u32 authtype, u32 loops)
    {
        u32 digestlen;

        u32 auth_keylen, crypt_keylen, ivlen;
        u8 *auth_key, *crypt_key, *pIv;
        u32 msglen;
        u8 *message;
        u8 *pAAD;
        int aadLen;
        int i;
        int ret = FAIL;

        u8 *pResult_asic;


        dbg_printk("modetype=0x%x, cipher_type=0x%x, authtype=0x%x\n",
                   modetype, cipher_type, authtype);


        ivlen = 8;
        pAAD = NULL;
        aadLen = 0;


        switch (cipher_type) {
            case _MD_NOCRYPTO:
                crypt_keylen = 0;
                break;
            case 0x00:
                crypt_keylen = 8;
                break;
            case 0x01:
                crypt_keylen = 8;
                break;
            case 0x10:
                crypt_keylen = 24;
                break;
            case 0x11:
                crypt_keylen = 24;
                break;
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x30:
            case 0x31: // chacha20
                crypt_keylen = 32;
                ivlen = 16;
                break;
            case 0x20:
                crypt_keylen = 32;
                ivlen = 0;
                break;
            case 0x28:
                crypt_keylen = 16;
                ivlen = 16;
                aadLen = 20;
                break;

            default:
                err_printk("no this cipher type\n");
                return FAIL;
        }

        switch (authtype) {
            case _MD_NOAUTH:
                digestlen = 0;
                break;
            case 1:
            case 4:
                digestlen = 16;
                break;
            case 2:
            case 5:
                digestlen = 20;
                break;
            case 3:
            case 6:
                digestlen = 32;
                break;
            case 7: // poly1305
                digestlen = 16;
                break;

            default:
                err_printk("authtype(%d) : no this type\n", authtype);
                return FAIL;
        }


        crypt_key = &_asic_cryptkey[0];
        for (i=0; i<crypt_keylen; i++)
            crypt_key[i] = rtl_random() & 0xFF;
        crypt_key[i] = '\0';



        // Suppose keylen = 0~ BLOCK(64)
        if ( (digestlen == 0) || (authtype < 3) ) { // no HMAC
            auth_keylen = 0;
            auth_key = NULL;
        } else {
            auth_keylen = ( rtl_random() % CRYPTO_MAX_KEY_LENGTH )+1;
            auth_key = &_asic_authkey[0];
            for (i=0; i<auth_keylen; i++)
                auth_key[i] = rtl_random() & 0xFF;
            auth_key[i] = '\0';
        }

        if ( cipher_type != _MD_NOCRYPTO ) {
            pIv = _asic_iv;
            for (i=0; i<ivlen; i++)
                pIv[i] = rtl_random() & 0xFF;
        } else {
            pIv = NULL;
            ivlen = 0;
        }

        // AAD
        if ( aadLen != 0 ) {
            pAAD = _asic_aad;
            for (i=0; i<aadLen; i++)
                pAAD[i] = rtl_random() & 0xFF;
        }

        // 32 byte * 50 = 1600
        msglen = 1600;

        message = &_asic_orig[32];

        for (i=0; i<msglen; i++)
            message[i] = rtl_random() & 0xFF;
        message[i] = '\0';



        pResult_asic = _asic_enc;

        if ( modetype == CRYPTO_MS_TYPE_AUTH ) {

            throughput_test_auth(authtype,
                                 auth_key, auth_keylen, digestlen, message, msglen, pResult_asic, sizeof(_asic_enc), loops);


        } else {

            throughput_test_cipher(cipher_type,
                                   crypt_key, crypt_keylen, pIv, ivlen, digestlen, message, msglen, pAAD, aadLen,  pResult_asic, sizeof(_asic_enc), loops);

        }

        ret = SUCCESS;


        return ret;


    }


//
// mbedtls md5
//

    /*
     * RFC 1321 test vectors
     */
    static const unsigned char md5_test_buf[7][81] = {
        { "" },
        { "a" },
        { "abc" },
        { "message digest" },
        { "abcdefghijklmnopqrstuvwxyz" },
        { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
        {
            "12345678901234567890123456789012345678901234567890123456789012" \
            "345678901234567890"
        }
    };

    static const int md5_test_buflen[7] = {
        0, 1, 3, 14, 26, 62, 80
    };

    static const unsigned char md5_test_sum[7][16] = {
        {
            0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
            0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E
        },
        {
            0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,
            0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61
        },
        {
            0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
            0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72
        },
        {
            0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,
            0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0
        },
        {
            0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,
            0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B
        },
        {
            0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,
            0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F
        },
        {
            0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
            0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A
        }
    };


    int mbedtls_md5_self_test(void)
    {
        int i;
        unsigned char md5sum[16];


        printk("mbedtls_md5_self_test \r\n");

        for( i = 0; i < 7; i++ ) {
            printk( "  MD5 test #%d: ", i + 1 );

            mbedtls_md5( md5_test_buf[i], md5_test_buflen[i], md5sum );

            if( rtl_memcmpb( md5sum, md5_test_sum[i], 16 ) != 0 ) {
                printk( "failed\n" );
                return _FALSE;
            }

            printk( "passed\n" );
        }

        printk( "\n" );

        return _TRUE;
    }

//
// SHA1
//
    /*
     * FIPS-180-1 test vectors
     */
    static const unsigned char sha_test_buf[3][57] = {
        { "abc" },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
        { "" }
    };

    static const int sha_test_buflen[3] = {
        3, 56, 1000
    };

    static const unsigned char sha1_test_sum[3][20] = {
        {
            0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
            0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D
        },
        {
            0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
            0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1
        },
        {
            0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
            0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F
        }
    };

    static const unsigned char sha256_test_sum[6][32] = {
        /*
         * SHA-224 test vectors
         */
        {
            0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
            0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
            0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
            0xE3, 0x6C, 0x9D, 0xA7
        },
        {
            0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
            0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
            0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
            0x52, 0x52, 0x25, 0x25
        },
        {
            0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
            0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
            0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
            0x4E, 0xE7, 0xAD, 0x67
        },

        /*
         * SHA-256 test vectors
         */
        {
            0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
            0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
            0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
            0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
        },
        {
            0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
            0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
            0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
            0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
        },
        {
            0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
            0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
            0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
            0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0
        }
    };

    /*
     * Checkup routine
     */
    int mbedtls_sha1_self_test(void)
    {
        int i, j, buflen, ret = _TRUE;
        unsigned char buf[1024];
        unsigned char sha1sum[20];
        mbedtls_sha1_context ctx;

        mbedtls_sha1_init( &ctx );

        /*
         * SHA-1
         */
        for( i = 0; i < 3; i++ ) {
            printk( "  SHA-1 test #%d: ", i + 1 );

            mbedtls_sha1_starts( &ctx );

            if( i == 2 ) {
                rtl_memset( buf, 'a', buflen = 1000 );

                for( j = 0; j < 1000; j++ )
                    mbedtls_sha1_update( &ctx, buf, buflen );
            } else
                mbedtls_sha1_update( &ctx, sha_test_buf[i],
                                     sha_test_buflen[i] );

            mbedtls_sha1_finish( &ctx, sha1sum );

            if( rtl_memcmpb( sha1sum, sha1_test_sum[i], 20 ) != 0 ) {
                printk( "failed\n" );

                ret = _FALSE;
                goto exit;
            }

            printk( "passed\n" );
        }

        printk( "\n" );

exit:
        mbedtls_sha1_free( &ctx );

        return( ret );
    }

    /*
     * Checkup routine
     */
    int mbedtls_sha256_self_test(void)
    {
        int i, j, k, buflen, ret = 0;
        unsigned char buf[1024];
        unsigned char sha256sum[32];
        mbedtls_sha256_context ctx;

        mbedtls_sha256_init( &ctx );

        for( i = 0; i < 6; i++ ) {
            j = i % 3;
            k = i < 3;

            printk( "  SHA-%d test #%d: ", 256 - k * 32, j + 1 );

            mbedtls_sha256_starts( &ctx, k );

            if( j == 2 ) {
                rtl_memset( buf, 'a', buflen = 1000 );

                for( j = 0; j < 1000; j++ )
                    mbedtls_sha256_update( &ctx, buf, buflen );
            } else
                mbedtls_sha256_update( &ctx, sha_test_buf[j],
                                       sha_test_buflen[j] );

            mbedtls_sha256_finish( &ctx, sha256sum );

            if( rtl_memcmpb( sha256sum, sha256_test_sum[i], 32 - k * 4 ) != 0 ) {
                printk( "failed\n" );

                ret = 1;
                goto exit;
            }

            printk( "passed\n" );
        }

        printk( "\n" );

exit:
        mbedtls_sha256_free( &ctx );

        return( ret );
    }


} //extern C




static int crc_test(void)
{
    int len;
    uint32_t crc;
    uint32_t crc_correct;


    crc_tester_setting(32, 0x4c11db7, 1, 0xffffffff, 0xffffffff, 1, 1);


    //
    len = 43;
    crc_correct = *((uint32_t*)(crc32_digest));
    //crc = crc32(0, (void*)(crc_plaintext), len);
    crc = crc_tester(crc_plaintext, len);
    if ( crc != crc_correct ) {
        printk("crc failed : expected : 0x%x, result : 0x%x \r\n", crc_correct, crc);
        return -1;
    } else {
        printk("crc OK=0x%x\r\n", crc);
        return 0;
    }
}


#include <cmsis_os.h>

osThreadId thread1_id;
osThreadId thread2_id;

void thread1(void const *argv);
void thread2(void const *argv);

osThreadDef(thread1, osPriorityNormal, DEFAULT_STACK_SIZE); //default stack size
osThreadDef(thread2, osPriorityNormal, DEFAULT_STACK_SIZE); //default stack size


void test_rtos(void)
{
    thread1_id = osThreadCreate(osThread(thread1), NULL);
    if (thread1_id == NULL ) {
        DiagPrintf("Thread1 create failed \r\n");
        return;
    }

    thread2_id = osThreadCreate(osThread(thread2), NULL);
    if (thread2_id == NULL ) {
        DiagPrintf("Thread2 create failed \r\n");
        return;
    }

}

void thread1(void const *argv)
{
    int i=0;
    u32 start_count, current_count, spend_count;

    DiagPrintf("thread1 : i=%d \r\n", i);
    start_count = HalTimerOp.HalTimerReadCount(1);
    while (i<1000) {
        osSignalWait(0x1, osWaitForever);
        i++;
        osSignalSet(thread2_id, 0x2);
    }
    current_count = HalTimerOp.HalTimerReadCount(1);

    if ( start_count < current_count) {
        spend_count =  (0xFFFFFFFF - current_count) + start_count;
    } else {
        spend_count = start_count - current_count;
    }
    DiagPrintf("spend time : %d \r\n", spend_count*1000/32);

}

void thread2(void const *argv)
{
    int i=0;
    while (1) {
        osSignalSet(thread1_id, 0x1);
        osSignalWait(0x2, osWaitForever);
    }
}

#if 0


#include "mbed/ualloc/ualloc.h"
#include "mbed/core-util/sbrk.h"

extern void * volatile mbed_sbrk_ptr;
extern void * volatile mbed_krbs_ptr;
extern volatile uintptr_t mbed_sbrk_diff;

#define TEST_SIZE_0 0x20

//
void ualloc_test(void)
{
    const char * current_test = "none";
    UAllocTraits_t traits;

    uintptr_t heap_start = reinterpret_cast<uintptr_t>(&__mbed_sbrk_start);
    uintptr_t heap_end   = reinterpret_cast<uintptr_t>(&__mbed_krbs_start);

    bool tests_pass = false;

    do {
        uintptr_t sbrk_ptv, krbs_ptv;
        void * ptr;
        // Make sure that stack variables are not on the heap
        current_test = "stack/heap";
        uint32_t testval;
        uintptr_t ptv = reinterpret_cast<uintptr_t>(&testval);
        if ((ptv > heap_start) && (ptv < heap_end)) {
            break;
        }

        // malloc
        current_test = "malloc";
        ptr = malloc(TEST_SIZE_0);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        DiagPrintf("ptr=0x%x, ptv=0x%x, heap_start=0x%x, sbrk_ptv=0x%x \r\n", ptr, ptv, heap_start, sbrk_ptv);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        // free
        free(ptr);

        // calloc
        current_test = "calloc";
        ptr = calloc(TEST_SIZE_0,4);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        // free
        free(ptr);

        // realloc
        current_test = "realloc";
        // malloc two blocks
        void * ptr0 = malloc(TEST_SIZE_0);
        if (ptr0 == NULL) {
            break;
        }
        void * ptr1 = malloc(TEST_SIZE_0);
        if (ptr1 == NULL) {
            break;
        }
        // realloc the first one and make sure the pointer moves
        void * ptr2 = realloc(ptr0, 2*TEST_SIZE_0);
        if (ptr2 == NULL) {
            break;
        }
        if (ptr2 == ptr0) {
            break;
        }
        // free
        free(ptr1);
        free(ptr2);

        // ualloc
        current_test = "ualloc no_flags";
        traits.flags = 0;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        mbed_ufree(ptr);

        traits.flags = UALLOC_TRAITS_BITMASK;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        mbed_ufree(ptr);

        current_test = "ualloc never_free";
        traits.flags = UALLOC_TRAITS_NEVER_FREE;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        krbs_ptv = reinterpret_cast<uintptr_t>(mbed_krbs_ptr);
        if (!((ptv >= krbs_ptv) && (ptv < heap_end))) {
            break;
        }
        // this call is ignored, but should generate a debug message
        mbed_ufree(ptr);

        current_test = "ualloc zero-fill";
        traits.flags = UALLOC_TRAITS_BITMASK;
        // allocate a block
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);
        // Validate the allocation
        ptv = reinterpret_cast<uintptr_t>(ptr0);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }

        // Fill the memory
        memset(ptr, 0xA5, TEST_SIZE_0);
        // free the memory
        mbed_ufree(ptr0);
        // allocate the same size block with ualloc zero-fill
        traits.flags = UALLOC_TRAITS_ZERO_FILL;
        ptr1 = mbed_ualloc(TEST_SIZE_0, traits);

        // Validate that the pointers are the same
        if (ptr1 != ptr0) {
            break;
        }

        // Check for zero-fill
        bool ok = true;
        for (unsigned i = 0; ok && (i < TEST_SIZE_0/sizeof(uint32_t)); i++) {
            ok = ((uint32_t *)ptr1)[i] == 0;
        }
        if (!ok) {
            break;
        }

        mbed_ufree(ptr1);

        // never_free + zero-fill
        current_test = "never free/zero-fill";
        // prefill the memory
        memset((char*) mbed_krbs_ptr - TEST_SIZE_0, 0xA5, TEST_SIZE_0);
        // Allocate the memory
        traits.flags = UALLOC_TRAITS_ZERO_FILL | UALLOC_TRAITS_NEVER_FREE;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);

        ok = true;
        for (unsigned i = 0; ok && (i < TEST_SIZE_0/sizeof(uint32_t)); i++) {
            ok = ((uint32_t *)ptr)[i] == 0;
        }
        if (!ok) {
            break;
        }

        current_test = "urealloc";
        traits.flags = 0;
        // malloc two blocks
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr0 == NULL) {
            break;
        }
        ptr1 = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr1 == NULL) {
            break;
        }
        // realloc the first one and make sure the pointer moves
        ptr2 = mbed_urealloc(ptr0, 2 * TEST_SIZE_0, traits);
        if (ptr2 == NULL) {
            break;
        }
        if (ptr2 == ptr0) {
            break;
        }
        // free
        mbed_ufree(ptr1);
        mbed_ufree(ptr2);

        // Test failing allocations
        // try to realloc a never-free pointer
        current_test = "urealloc/never-free";
        // Allocate the memory
        traits.flags = UALLOC_TRAITS_ZERO_FILL | UALLOC_TRAITS_NEVER_FREE;
        void * nfptr = mbed_ualloc(TEST_SIZE_0, traits);
        if (nfptr == NULL) {
            break;
        }
        traits.flags = 0;
        ptr = mbed_urealloc(nfptr, 2 * TEST_SIZE_0, traits);
        if (ptr != NULL) {
            break;
        }

        // try to allocate with reserved flags
        current_test = "ualloc/reserved flags";
        traits.flags = UALLOC_RESERVED_MASK | UALLOC_TRAITS_BITMASK;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr != NULL) {
            break;
        }

        // try urealloc with any flags
        current_test = "urealloc/flags";
        traits.flags = 0;
        // malloc two blocks
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);

        traits.flags = UALLOC_TRAITS_NEVER_FREE;
        ptr2 = mbed_urealloc(ptr0, 2 * TEST_SIZE_0, traits);
        if (ptr2 != NULL) {
            break;
        }
        // free
        mbed_ufree(ptr0);


        tests_pass = true;
    } while (0);

    if (!tests_pass) {
        printk("First failing test: %s\r\n", current_test);
    } else {
        printk("ualloc_test success\r\n");
    }

}

#endif

//

extern "C" {

    extern int rtl_crypto_test(IN u16 argc, IN u8 *argv[]);



    int rtl_crypto_test(u16 argc, u8 *argv[])
    {
        int ret;

        dbg_printk("argc=%d\n", argc);
        dbg_printk("argv[0]=%s\n", argv[0]);
        if (argc < 1) {
            rtl_crypto_usage(NULL);
            return _FALSE;
        }

        if (strcmp(argv[0], "init") == 0) {
            printk("New Style\n");
            ret = rtl_cryptoEngine_init();

            // for simulation
            mbedtls_init();

            if(  ret == SUCCESS ) {
                printk("Crypto Engine Init Success!!!\n");
                return _TRUE;
            } else {
                printk("Crypto Engine Init FAILED, ret=%d\n", ret);
                return _FALSE;
            }

        } else if (strcmp(argv[0], "info") == 0) {
            dbg_printk("Hello info\n");
            rtl_cryptoEngine_info();
        } else if (strcmp(argv[0], "test") == 0) {
            int loop=1;
            int ret;
            printk("Hello Test \r\n");
            // rtl_srandom(0xA0A0A0A0);
            rtl_srandom(0x66899c3c); // 2003700
            for (;;) {
                int choice;

                if ( (loop % 100) == 0 )  {
                    int i;

                    printk("Round %d, rand seed=0x%x\n", loop, this_rtlseed);

                }
                choice = loop % 3;
                //choice = 0;
                if ( choice == 0 ) {
                    ret = vector_test_auth_rand();
                } else if ( choice == 1 ) {
                    ret = vector_test_cipher_rand();
                } else if ( choice == 2 ) {
                    ret = vector_test_mix_rand();
                }

                if ( ret != SUCCESS ) {
                    printk("failed\n");
                    printk("Round %d\n", loop);
                    break;
                }
                loop++;
            }

        } else if (strcmp(argv[0], "debug") == 0) {
            u8 mode_select;

            if (argc != 2) {
                rtl_crypto_usage("debug");
                return _FALSE;
            }

            mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

            switch ( mode_select ) {
                case 0 :
                    DBG_INFO_MSG_OFF(_DBG_CRYPTO_);
                    printk("Crypto Debug Info turn OFF\n");
                    break;

                case 1 :
                    DBG_INFO_MSG_ON(_DBG_CRYPTO_);
                    printk("Crypto Debug Info turn ON\n");
                    break;

                default :
                    rtl_crypto_usage("debug");
                    break;
            }

        } else if (strcmp(argv[0], "rand") == 0) {
            static int seed = 0;
            int round;
            int mode_crypto;
            int mode_auth;
            int cntScatter;

            // Random test
            if (argc == 1) {
                crypto_GeneralApiRandTest(seed++, 100, 0xff, 0xff, 0xff); // 0xff for random
            } else if (argc == 3) {
                seed = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
                round = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
                printk("seed=%d, round=%d", seed, round);
                crypto_GeneralApiRandTest(seed, round, 0xff, 0xff, 0xff); // 0xff for random
            } else if (argc == 5 || argc ==6) {
                //todo
                // CRYPTO:
                //   0x00:CBC_DES,0x01:CBC_3DES,0x02:ECB_DES,0x03:ECB_3DES
                //   0x20:CBC_AES,0x22:ECB_AES,0x23:CTR_AES
                // AUTH:
                //   0:HASH_MD5,1:HASH_SHA1,2:HMAC_MD5,3:HMAC_SHA1

            } else {
                rtl_crypto_usage("rand");
                return 1;
            }
        } else if (strcmp(argv[0], "testrtos") == 0) {
            u8 mode_select;

            if (argc != 1) {
                printk("test rtos argc=%d fail \r\n", argc);
                return _FALSE;
            }

            test_rtos();

        }

//
//	Usage :
//
//
//		throughput 0 0 -1 -> DES-ECB : 0x0
//		throughput 0 1 -1 -> DES-CBC : 0x1
//		throughput 0 16 -1 -> 3DES-ECB : 0x10
//		throughput 0 17 -1 -> 3DES-CBC : 0x11
//		throughput 0 32 -1 -> AES-ECB :   0x20
//		throughput 0 33 -1 -> AES-CBC  :  0x21
//		throughput 0 34 -1 -> AES-CFB : 0x22
//		throughput 0 35 -1 -> AES-OFB  : 0x23
//		throughput 0 36 -1 -> AES-CTR  : 0x24
//		throughput 0 40 -1 -> AES-GCM  : 0x28
//		throughput 0 48 -1 -> chacha      : 0x30
//
//		throughput 1 -1 1 -> MD5
//		throughput 1 -1 2 -> SHA1
//		throughput 1 -1 3 -> SHA2
//		throughput 1 -1 4 -> HMAC-MD5
//		throughput 1 -1 5 -> HMAC-SHA1
//		throughput 1 -1 6 -> HMAC-SHA2
//		throughput 1 -1 7 -> poly1305
//

        else if (strcmp(argv[0], "throughput") == 0) {
            u32 modetype, crypttype, authtype, loops;

#if 0
            if ( argc == 2 ) {
                if ( strcmp(argv[1], "memcpy")==0 ) {
                    return throughput_test_memcpy();
                }
            }
#endif

            modetype = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
            crypttype = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
            authtype = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
            loops = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);

            switch ( modetype ) {
                case _MS_CRYPTO :
                    authtype = _MD_NOAUTH ;
                    break;
                case _MS_AUTH :
                    crypttype = _MD_NOCRYPTO;
                    break;
            }

            if ( throughput_test(modetype, crypttype, authtype, loops) != SUCCESS ) {
                printk("FAIL! \n");
            }

        }

//
//	Usage :
//
//		vector 0 16 -1 -> 3DES-ECB : 0x10
//		vector 0 17 -1 -> 3DES-CBC : 0x11
//		vector 0 32 -1 -> AES-ECB :   0x20
//		vector 0 33 -1 -> AES-CBC  :  0x21
//		vector 0 34 -1 -> AES-CFB  :  0x22
//		vector 0 35 -1 -> AES-OFB  :  0x23
//		vector 0 36 -1 -> AES-CTR  : 0x24
//		vector 0 48 -1 -> Chacha20 : 0x30
//
//		vector 1 -1 1 -> MD5
//		vector 1 -1 2 -> SHA1
//		vector 1 -1 3 -> SHA2
//		vector 1 -1 4 -> HMAC-MD5
//		vector 1 -1 5 -> HMAC-SHA1
//		vector 1 -1 6 -> HMAC-SHA2
//	       vector 1 -1 7 -> Poly1305
//		vector 1 -1 8 -> CRC32
//
//		vector 2 16 0 -> 3DES-CCB-SHA1 : MtE (MAC-then-Encrypt, for SSL)
//		MtE : HMAC(message) ->> Encrypt(message + HMAC )
//
        else if (strcmp(argv[0], "vector") == 0) {
            int ret;
            u32 mode_select, cipher_type, auth_type;

            if ( argc == 2 ) {
                if ( strcmp(argv[1], "md5")==0 ) {
                    return mbedtls_md5_self_test();
                } else if ( strcmp(argv[1], "sha1")==0 ) {
                    return mbedtls_sha1_self_test();
                } else if ( strcmp(argv[1], "sha256")==0 ) {
                    return mbedtls_sha256_self_test();
                } else if ( strcmp(argv[1], "des") == 0 ) {
                    ret = mbedtls_des_self_test(1);
                } else if ( strcmp(argv[1], "aes") == 0 ) {
                    ret = mbedtls_aes_self_test(1);
                } else if ( strcmp(argv[1], "gcm") == 0 ) {
                    ret = mbedtls_gcm_self_test(1);
                } else if ( strcmp(argv[1], "mix") == 0 ) {
                    ret = chacha20poly1305_homekit_test();
                } else if ( strcmp(argv[1], "crc") == 0 ) {
                    ret = crc_test();
                } else {
                    return _FALSE;
                }
                return (ret==0)? _TRUE : _FALSE;

            } else if (argc != 4) {
                rtl_crypto_usage("vector");
                return _FALSE;
            }

            mode_select = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
            cipher_type = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
            auth_type = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);

            switch (mode_select) {
                case 0 : // ESP algorithm
                    ret = vector_test_cipher(cipher_type);
                    break;
                case 1 : // Auth algorithm
                    ret = vector_test_auth(auth_type);
                    break;
                case 2 : // mix mode for chacha_poly1305
                    ret = vector_test_chacha_poly1305();
                    break;
                default:
                    err_printk("No this mode_select type:%d\n", mode_select);
                    ret = _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH;
                    break;
            }

            printk("vector test done. ret=%d\n", ret);
            return _TRUE;
        } else if (strcmp(argv[0], "testissue") == 0) {
            printk("test issue \n");
            issue_test1();
            printk("test issue done\n");
            return _TRUE;
        } else {
            rtl_crypto_usage(NULL);
            return _FALSE;
        }

        return _TRUE;
    }

} // extern C


